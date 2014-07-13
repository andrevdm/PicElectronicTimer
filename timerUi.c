#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib/timers.h>
#include <pic18f4550.h>

#include "chipConfig.h"
#include "timerConfig.h"
#include "lcd.h"
#include "timerUi.h"
#include "dateTime.h"
#include "timer.h"

StateFunc displayCurrentState;
volatile EEventType displayEvent;

volatile bit tick = 0;
volatile long bres = 0;
volatile bit dirty = 0;
long currentPgm = 1;
bit stopClock = 0;
volatile int dt_second = 0;
volatile int dt_minute = 3;
volatile int dt_hour = 8;
volatile int dt_day = 15;
volatile int dt_month = 5;
volatile int dt_year = 14;


void DisplayRelayState()
{
    lcdSendCommand( 0xC0 );//Line 2

    for( int i = 1; i <= MAX_PGMS; ++i )
    {
        PgmData *pgm = getPgmData( i );

        if( isPgmForceOff(pgm) )
        {
            lcdWriteChar('x');
            continue;
        }

        if( isPgmForceOn(pgm) )
        {
            lcdWriteChar('*');
            continue;
        }

        lcdWriteChar( pgm->isOn ? '1' : '0' );
    }
}

void DisplayTime()
{
    char sz[9];
    sprintf( sz, "%02d:%02d:%02d", dt_hour, dt_minute, dt_second );

    lcdSendCommand( 0x02 );//Home
    lcdWriteString( sz );
}

void StateTime_TimeDisplay( EEventType evt )
{
    switch( evt )
    {
        case BUT_MODE:
        {
            displayCurrentState = StatePgm_MainDisplay;
            return;
        }

        case BUT_MODE_HOLD:
        {
            displayCurrentState = StateTime_TimeEdit;
            return;
        }

        case BUT_RIGHT:
        {
            displayCurrentState = StateTime_DateDisplay;
            return;
        }

        case BUT_LEFT:
        {
            displayCurrentState = StateVer_Display;
            return;
        }

        case INITIALISE:
        {
            lcdSendCommand( 0x01 ); //CLS
            break;
        }
    }

    DisplayTime();
    DisplayRelayState();
}

void IncDigit( volatile int *val, int multiplier, int max )
{
    int before = *val;
    before /= multiplier * 10;
    before *= multiplier * 10;

    int after = *val;
    after -= before;
    int digit = after / multiplier;
    after -= digit * multiplier;

    if( digit < max )
    {
        *val = before + after + ((digit + 1) * multiplier);
    }
    else
    {
        *val = before + after;
    }
}

void StateTime_TimeEdit( EEventType evt )
{
    static short row = 0;
    static short col = 0;

    switch( evt )
    {
        case INITIALISE:
        {
            row = 0;
            col = 0;
            stopClock = 1;
            lcdSendCommand( 0x01 ); //CLS

            break;
        }

        case TICK:
        {
            return;
        }

        case BUT_MODE:
        {
            dirty = 1;
            stopClock = 0;
            SaveTime();
            lcdSendCommand( 0x0C );// | 1:function, 1:display, 0:cursor, 0:blink
            displayCurrentState = StateTime_TimeDisplay;

            return;
        }

        case BUT_MODE_HOLD:
        {
            return;
        }

        case BUT_RIGHT:
        {
            switch( col )
            {
                case 0: col += 1; break;
                case 1: col += 2; break;
                case 3: col += 1; break;
                case 4: col += 2; break;
                case 6: col += 1; break;
            }

            break;
        }

        case BUT_LEFT:
        {
            switch( col )
            {
                case 1: col -= 1; break;
                case 3: col -= 2; break;
                case 4: col -= 1; break;
                case 6: col -= 2; break;
                case 7: col -= 1; break;
            }
            break;
        }

        case BUT_TOGGLE:
        {
            switch( col )
            {
                case 0:
                    IncDigit( &dt_hour, 10, 2 );
                    break;

                case 1:
                    IncDigit( &dt_hour, 1, 9 );
                    break;

                case 3:
                    IncDigit( &dt_minute, 10, 5 );
                    break;

                case 4:
                    IncDigit( &dt_minute, 1, 9 );
                    break;

                case 6:
                    IncDigit( &dt_second, 10, 5 );
                    break;

                case 7:
                    IncDigit( &dt_second, 1, 9 );
                    break;
            }

            break;
        }
    }

    lcdSendCommand( 0x02 );//Home
    char sz[9];
    sprintf( sz, "%02d:%02d:%02d", dt_hour, dt_minute, dt_second );
    lcdWriteString( sz );

    lcdSendCommand( 0x80 + (0x40 * row) + col );//position
    lcdSendCommand( 0x0F );// | 1:function, 1:display, 1:cursor, 1:blink
}

void DisplayDate()
{
    lcdSendCommand( 0x02 );//Home

    char sz[9];

    int wd = getDayOfWeek( 2000 + dt_year, dt_month, dt_day );
    lcdWriteString( getDayOfWeekName( wd ) );

    lcdWriteChar( ' ' );

    lcdWriteString( getMonthName( dt_month ) );

    lcdSendCommand( 0xC0 );//Home
    sprintf( sz, "%02d 20%02d", dt_day, dt_year );
    lcdWriteString( sz );
}

void StateTime_DateDisplay( EEventType evt )
{
    switch( evt )
    {
        case BUT_MODE:
        {
            displayCurrentState = StatePgm_MainDisplay;
            return;
        }

        case BUT_MODE_HOLD:
        {
            displayCurrentState = StateTime_DateEdit;
            return;
        }

        case BUT_LEFT:
        {
            displayCurrentState = StateTime_TimeDisplay;
            return;
        }

        case BUT_RIGHT:
        {
            displayCurrentState = StateVer_Display;
            return;
        }

        case INITIALISE:
        {
            lcdSendCommand( 0x01 ); //CLS
            break;
        }
    }

    DisplayDate();
}

void StateTime_DateEdit( EEventType evt )
{
    static short row = 0;
    static short col = 0;

    switch( evt )
    {
        case INITIALISE:
        {
            row = 0;
            col = 0;
            stopClock = 1;
            lcdSendCommand( 0x01 ); //CLS

            break;
        }

        case TICK:
        {
            return;
        }

        case BUT_MODE:
        {
            dirty = 1;
            stopClock = 0;
            SaveTime();
            lcdSendCommand( 0x0C );// | 1:function, 1:display, 0:cursor, 0:blink
            displayCurrentState = StateTime_DateDisplay;
            return;
        }

        case BUT_MODE_HOLD:
        {
            return;
        }

        case BUT_RIGHT:
        {
            switch( col )
            {
                case 0: col += 1; break;
                case 1: col += 2; break;
                case 3: col += 1; break;
                case 4: col += 2; break;
                case 6: col += 1; break;
            }

            break;
        }

        case BUT_LEFT:
        {
            switch( col )
            {
                case 1: col -= 1; break;
                case 3: col -= 2; break;
                case 4: col -= 1; break;
                case 6: col -= 2; break;
                case 7: col -= 1; break;
            }
            break;
        }

        case BUT_TOGGLE:
        {
             switch( col )
            {
                case 0:
                    IncDigit( &dt_day, 10, 3 );
                    break;

                case 1:
                    IncDigit( &dt_day, 1, 9 );
                    break;

                case 3:
                    IncDigit( &dt_month, 10, 1 );
                    break;

                case 4:
                    IncDigit( &dt_month, 1, 9 );
                    break;

                case 6:
                    IncDigit( &dt_year, 10, 1 );
                    break;

                case 7:
                    IncDigit( &dt_year, 1, 9 );
                    break;
            }

            break;
        }
    }

    lcdSendCommand( 0x02 );//Home
    char sz[9];

    sprintf( sz, "%02d/%02d/%02d", dt_day, dt_month, dt_year );
    lcdWriteString( sz );

    lcdSendCommand( 0x80 + (0x40 * row) + col );//position
    lcdSendCommand( 0x0F );// | 1:function, 1:display, 1:cursor, 1:blink
}

void StateVer_Display( EEventType evt )
{
    switch( evt )
    {
        case BUT_MODE:
        {
            displayCurrentState = StatePgm_MainDisplay;
            return;
        }

        case BUT_LEFT:
        {
            displayCurrentState = StateTime_DateDisplay;
            return;
        }

        case BUT_RIGHT:
        {
            displayCurrentState = StateTime_TimeDisplay;
            return;
        }

        default:
            return;

        case INITIALISE:
        {
            lcdSendCommand( 0x01 ); //CLS
            break;
        }
    }

    lcdSendCommand( 0x02 );//Home
    lcdWriteString( "S v0.5" );

    lcdSendCommand( 0xC0 );
    lcdWriteString( "H v0.1" );
}

void PgmNext()
{
    currentPgm++;

    if( currentPgm > MAX_PGMS )
    {
        displayCurrentState = StateTime_TimeDisplay;
        currentPgm = 1;
    }
    else
    {
        displayCurrentState = StatePgm_MainDisplay;
        lcdSendCommand( 0x01 );
        DisplayCurrentPgm();
    }
}

void StatePgm_MainDisplay( EEventType evt )
{
    static PgmData *pgm;

    switch( evt )
    {
        case INITIALISE:
        {
            pgm = getPgmData(currentPgm);
            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case BUT_MODE:
        {
            PgmNext();
            return;
        }

        case BUT_MODE_HOLD:
        {
            displayCurrentState = StatePgm_MainEdit;
            return;
        }

        case BUT_LEFT:
        {
          displayCurrentState = StatePgm_StatusDisplay;
            return;
        }

        case BUT_RIGHT:
        {
            displayCurrentState = StatePgm_WeekDisplay;
            return;
        }

        default:
            return;
    }

    DisplayCurrentPgm();
}

void DisplayCurrentPgm()
{
    PgmData *pgm = getPgmData( currentPgm );

    lcdSendCommand( 0x02 );//Home

    lcdWriteChar( 'p' );
    lcdWriteChar( '0' + currentPgm );

    if( pgm->isOn )
    {
        lcdWriteString( " On " );
    }
    else
    {
        lcdWriteString( " Off" );
    }

    lcdSendCommand( 0xC0 );
    lcdWriteString( getPgmAutoState(pgm) ); //Auto, set-on, set-off
}

void StatePgm_MainEdit( EEventType evt )
{
    static bit isOn;
    static short autoState;
    static PgmData *pgm;

    switch( evt )
    {
        case INITIALISE:
        {
            pgm = getPgmData( currentPgm );
            break;
        }

        case TICK:
        {
            return;
        }

        case BUT_MODE:
        {
            SavePgmToEeprom(pgm);
            lcdSendCommand( 0x0C );// | 1:function, 1:display, 0:cursor, 0:blink
            displayCurrentState = StatePgm_MainDisplay;
            return;
        }

        case BUT_TOGGLE:
        {
            if( isPgmOnAuto(pgm) )
            {
                setPgmForceOn(pgm);
            }
            else
            {
                if( isPgmForceOn(pgm) )
                {
                    setPgmForceOff(pgm);
                }
                else
                {
                    setPgmAuto(pgm);
                }
            }

            break;
        }

        case BUT_LEFT:
        case BUT_RIGHT:
        {
            break;
        }
    }

    lcdSendCommand( 0x01 ); //CLS
    DisplayCurrentPgm();

    lcdSendCommand( 0xC0 );
    lcdSendCommand( 0x0F );// | 1:function, 1:display, 1:cursor, 1:blink
}

void DisplayPgmWeek( short days )
{
    lcdSendCommand( 0x02 );//Home
    lcdWriteString( "smtwtfs " );
    lcdSendCommand( 0xC0 );

    for( int i = 64; i > 0; i /= 2 )
    {
        lcdWriteChar( (days & i) != 0 ? '1' : '0' );
    }
}

void StatePgm_WeekDisplay( EEventType evt )
{
    switch( evt )
    {
        case INITIALISE:
        {
            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case BUT_MODE:
        {
            PgmNext();
            return;
        }

        case BUT_MODE_HOLD:
        {
            displayCurrentState = StatePgm_WeekEdit;
            return;
        }

        case BUT_LEFT:
        {
            displayCurrentState = StatePgm_MainDisplay;
            return;
        }

        case BUT_RIGHT:
        {
            displayCurrentState = StatePgm_FromDisplay;
            return;
        }

        default:
            return;
    }

    PgmData *pgm = getPgmData(currentPgm);
    DisplayPgmWeek(pgm->weekDays);
}

void StatePgm_WeekEdit( EEventType evt )
{
    static const int pow2[] = {64,32,16,8,4,2,1};
    static int pos = 0;
    PgmData *pgm;

    switch( evt )
    {
        case INITIALISE:
        {
            pos = 0;
            pgm = getPgmData(currentPgm);
            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case TICK:
        {
            return;
        }

        case BUT_MODE:
        {
            SavePgmToEeprom(pgm);
            lcdSendCommand( 0x0C );// | 1:function, 1:display, 0:cursor, 0:blink
            displayCurrentState = StatePgm_WeekDisplay;
            return;
        }

        case BUT_LEFT:
        {
            pos = pos > 1 ? pos - 1 : 7;
            break;
        }

        case BUT_RIGHT:
        {
            pos = pos < 7 ? pos + 1 : 0;
            break;
        }

        case BUT_TOGGLE:
        {
            if( pos != 7 )
            {
                pgm->weekDays ^= pow2[pos];
            }
            else
            {
                pgm->weekDays = (pgm->weekDays == 0 ? 127 : 0);
            }

            break;
        }

        default:
            return;
    }

    DisplayPgmWeek(pgm->weekDays);
    lcdWriteChar('.');

    lcdSendCommand( 0xC0 + pos );
    lcdSendCommand( 0x0F );// | 1:function, 1:display, 1:cursor, 1:blink
}

void DisplayPgmFrom( PgmData *pgm, short editing )
{
    lcdSendCommand( 0x02 );// home
    lcdWriteString( "From" );

    lcdSendCommand( 0xC0 );

    if( pgm->status & PGM_HAS_PERIOD )
    {
        char sz[9];
        sprintf(sz, "%02d:%02d", pgm->fromH, pgm->fromM );
        lcdWriteString(sz);

        if( editing == 0 )
        {
            lcdWriteString("   ");
        }
        else
        {
            lcdWriteString(".  ");
        }
    }
    else
    {
        if( editing != 0 )
        {
            lcdWriteString(". --:-- ");
        }
        else
        {
            lcdWriteString("--:--   ");
        }
    }
}

void StatePgm_FromDisplay( EEventType evt )
{
    static PgmData *pgm;

    switch( evt )
    {
        case INITIALISE:
        {
            lcdSendCommand( 0x01 ); //CLS
            pgm = getPgmData(currentPgm);
            break;
        }

        case BUT_MODE:
        {
            PgmNext();
            return;
        }

        case BUT_MODE_HOLD:
        {
            displayCurrentState = StatePgm_FromEdit;
            return;
        }

        case BUT_LEFT:
        {
            displayCurrentState = StatePgm_WeekDisplay;
            return;
        }

        case BUT_RIGHT:
        {
            if( pgm->status & PGM_HAS_PERIOD )
            {
                displayCurrentState = StatePgm_ToDisplay;
            }
            else
            {
                displayCurrentState = StatePgm_EveryDisplay;
            }
            return;
        }

        default:
            return;
    }

    DisplayPgmFrom( pgm, 0 );
}

void StatePgm_FromEdit( EEventType evt )
{
    static int pos;
    static PgmData *pgm;

    switch( evt )
    {
        case INITIALISE:
        {
            pos = 0;
            pgm = getPgmData(currentPgm);

            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case TICK:
        {
            return;
        }

        case BUT_MODE:
        {
            SavePgmToEeprom(pgm);
            lcdSendCommand( 0x0C );// | 1:function, 1:display, 0:cursor, 0:blink
            displayCurrentState = StatePgm_FromDisplay;
            return;
        }

        case BUT_LEFT:
        {
            if( pgm->status & PGM_HAS_PERIOD )
            {
                switch(pos)
                {
                    case 0: pos = 5; break;
                    case 1: pos = 0; break;
                    case 3: pos = 1; break;
                    case 4: pos = 3; break;
                    case 5: pos = 4; break;
                }
            }
            break;
        }

        case BUT_RIGHT:
        {
            if( pgm->status & PGM_HAS_PERIOD )
            {
                switch(pos)
                {
                    case 0: pos = 1; break;
                    case 1: pos = 3; break;
                    case 3: pos = 4; break;
                    case 4: pos = 5; break;
                    case 5: pos = 0; break;
                }
            }
            break;
        }

        case BUT_TOGGLE:
        {
            if( pgm->status & PGM_HAS_PERIOD )
            {
                switch(pos)
                {
                    case 0: IncDigit( &(pgm->fromH), 10, 2 ); break;
                    case 1: IncDigit( &(pgm->fromH), 1, 9 ); break;
                    case 3: IncDigit( &(pgm->fromM), 10, 5 ); break;
                    case 4: IncDigit( &(pgm->fromM), 1, 9 ); break;
                    case 5:
                    {
                        pgm->status &= ~PGM_HAS_PERIOD;
                        pos = 0;
                        break;
                    }
                }
            }
            else
            {
                pgm->status |= PGM_HAS_PERIOD;
                pos = 5;
            }
            break;
        }

        default:
            return;
    }

    DisplayPgmFrom( pgm, 1 );

    lcdSendCommand( 0xC0 + pos );
    lcdSendCommand( 0x0F );// | 1:function, 1:display, 1:cursor, 1:blink
}

void DisplayPgmTo( PgmData *pgm, short editing ) //TODO Combine from DisplayPgmFrom
{
    lcdSendCommand( 0x02 );// home
    lcdWriteString( "To" );

    lcdSendCommand( 0xC0 );

    char sz[9];
    sprintf(sz, "%02d:%02d   ", pgm->toH, pgm->toM );
    lcdWriteString(sz);
}

void StatePgm_ToDisplay( EEventType evt )
{
    static PgmData *pgm;

    switch( evt )
    {
        case INITIALISE:
        {
            pgm = getPgmData( currentPgm );
            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case BUT_MODE:
        {
            PgmNext();
            return;
        }

        case BUT_MODE_HOLD:
        {
            displayCurrentState = StatePgm_ToEdit;
            return;
        }

        case BUT_LEFT:
        {
            displayCurrentState = StatePgm_FromDisplay;
            return;
        }

        case BUT_RIGHT:
        {
            displayCurrentState = StatePgm_EveryDisplay;
            return;
        }

        default:
            return;
    }

    DisplayPgmTo( pgm, 0 );
}

void StatePgm_ToEdit( EEventType evt ) //TODO combine with from-edit?
{
    static int pos;
    static PgmData *pgm;

    switch( evt )
    {
        case INITIALISE:
        {
            pos = 0;
            pgm = getPgmData(currentPgm);

            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case TICK:
        {
            return;
        }

        case BUT_MODE:
        {
            SavePgmToEeprom(pgm);
            lcdSendCommand( 0x0C );// | 1:function, 1:display, 0:cursor, 0:blink
            displayCurrentState = StatePgm_ToDisplay;
            return;
        }

        case BUT_LEFT:
        {
            switch(pos)
            {
                case 0: pos = 4; break;
                case 1: pos = 0; break;
                case 3: pos = 1; break;
                case 4: pos = 3; break;
            }
            break;
        }

        case BUT_RIGHT:
        {
            switch(pos)
            {
                case 0: pos = 1; break;
                case 1: pos = 3; break;
                case 3: pos = 4; break;
                case 4: pos = 0; break;
            }
            break;
        }

        case BUT_TOGGLE:
        {
            switch(pos)
            {
                case 0: IncDigit( &(pgm->toH), 10, 2 ); break;
                case 1: IncDigit( &(pgm->toH), 1, 9 ); break;
                case 3: IncDigit( &(pgm->toM), 10, 5 ); break;
                case 4: IncDigit( &(pgm->toM), 1, 9 ); break;
            }

            break;
        }

        default:
            return;
    }

    DisplayPgmTo( pgm, 1 );

    lcdSendCommand( 0xC0 + pos );
    lcdSendCommand( 0x0F );// | 1:function, 1:display, 1:cursor, 1:blink
}

void StatePgm_EveryDisplay( EEventType evt )
{
    static PgmData *pgm;

    switch( evt )
    {
        case INITIALISE:
        {
            pgm = getPgmData(currentPgm);
            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case BUT_MODE:
        {
            PgmNext();
            return;
        }

        case BUT_MODE_HOLD:
        {
            displayCurrentState = StatePgm_EveryEdit;
            return;
        }

        case BUT_LEFT:
        {
            if( pgm->status & PGM_HAS_PERIOD )
            {
                displayCurrentState = StatePgm_ToDisplay;
            }
            else
            {
                displayCurrentState = StatePgm_FromDisplay;
            }

            return;
        }

        case BUT_RIGHT:
        {
            if( pgm->status & PGM_HAS_REP )
            {
                displayCurrentState = StatePgm_ForDisplay;
            }
            else
            {
                displayCurrentState = StatePgm_StatusDisplay;
            }
            return;
        }

        default:
            return;
    }

    lcdSendCommand( 0x02 );// home
    lcdWriteString( "Every  " );

    lcdSendCommand( 0xC0 );

    if( pgm->status & PGM_HAS_REP )
    {
        char sz[9];
        sprintf(sz, "%03d mins", pgm->everyM );
        lcdWriteString(sz);
    }
    else
    {
        lcdWriteString("---     ");
    }
}

void StatePgm_EveryEdit( EEventType evt )
{
    static PgmData *pgm;
    static int pos;

    switch( evt )
    {
        case INITIALISE:
        {
            pos = 0;
            pgm = getPgmData(currentPgm);
            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case TICK:
        {
            return;
        }

        case BUT_MODE:
        {
            SavePgmToEeprom(pgm);
            lcdSendCommand( 0x0C );// | 1:function, 1:display, 0:cursor, 0:blink
            displayCurrentState = StatePgm_EveryDisplay;
            return;
        }

        case BUT_LEFT:
        {
            if( pos > 0 )
            {
                pos = pos - 1;
            }
            else
            {
                pos = 3;
            }

            break;
        }

        case BUT_RIGHT:
        {
            if( pos < 3 )
            {
                pos = pos + 1;
            }
            else
            {
                pos = 0;
            }

            break;
        }

        case BUT_TOGGLE:
        {
            if( pgm->status & PGM_HAS_REP )
            {
                switch( pos )
                {
                    case 0: IncDigit( &(pgm->everyM), 100, 9 ); break;
                    case 1: IncDigit( &(pgm->everyM), 10, 9 ); break;
                    case 2: IncDigit( &(pgm->everyM), 1, 9 ); break;
                    case 3:
                    {
                        pos = 0;
                        pgm->status ^= PGM_HAS_REP;
                        break;
                    }
                }
            }
            else
            {
                pos = 3;
                pgm->status ^= PGM_HAS_REP;
            }

            break;
        }

        default:
            return;
    }


    lcdSendCommand( 0x02 );// home
    lcdWriteString( "Every  " );

    lcdSendCommand( 0xC0 );

    if( pgm->status & PGM_HAS_REP )
    {
        char sz[9];
        sprintf(sz, "%03d. m  ", pgm->everyM );
        lcdWriteString(sz);
    }
    else
    {
        lcdWriteString(". ---   ");
    }

    lcdSendCommand( 0xC0 + pos );
    lcdSendCommand( 0x0F );// | 1:function, 1:display, 1:cursor, 1:blink
}

void StatePgm_ForDisplay( EEventType evt )
{
    static PgmData *pgm;

    switch( evt )
    {
        case INITIALISE:
        {
            pgm = getPgmData(currentPgm);
            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case BUT_MODE:
        {
            PgmNext();
            return;
        }

        case BUT_MODE_HOLD:
        {
            displayCurrentState = StatePgm_ForEdit;
            return;
        }

        case BUT_LEFT:
        {
            displayCurrentState = StatePgm_EveryDisplay;
            return;
        }

        case BUT_RIGHT:
        {
            displayCurrentState = StatePgm_StatusDisplay;
            return;
        }

        default:
            return;
    }

    lcdSendCommand( 0x02 );// home
    lcdWriteString( "For   " );

    lcdSendCommand( 0xC0 );

    char sz[9];
    sprintf(sz, "%03d secs", pgm->forS );
    lcdWriteString(sz);
}

void StatePgm_ForEdit( EEventType evt )
{
    static PgmData *pgm;
    static int pos;

    switch( evt )
    {
        case INITIALISE:
        {
            pos = 0;
            pgm = getPgmData(currentPgm);
            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case TICK:
        {
            return;
        }

        case BUT_MODE:
        {
            SavePgmToEeprom(pgm);
            lcdSendCommand( 0x0C );// | 1:function, 1:display, 0:cursor, 0:blink
            displayCurrentState = StatePgm_ForDisplay;
            return;
        }

        case BUT_LEFT:
        {
            if( pos > 0 )
            {
                pos = pos - 1;
            }
            else
            {
                pos = 2;
            }

            break;
        }

        case BUT_RIGHT:
        {
            if( pos < 2 )
            {
                pos = pos + 1;
            }
            else
            {
                pos = 0;
            }

            break;
        }

        case BUT_TOGGLE:
        {
            switch( pos )
            {
                case 0: IncDigit( &(pgm->forS), 100, 9 ); break;
                case 1: IncDigit( &(pgm->forS), 10, 9 ); break;
                case 2: IncDigit( &(pgm->forS), 1, 9 ); break;
            }

            break;
        }

        default:
            return;
    }


    lcdSendCommand( 0x02 );// home
    lcdWriteString( "For  " );

    lcdSendCommand( 0xC0 );

    char sz[9];
    sprintf(sz, "%03d s   ", pgm->forS );
    lcdWriteString(sz);

    lcdSendCommand( 0xC0 + pos );
    lcdSendCommand( 0x0F );// | 1:function, 1:display, 1:cursor, 1:blink
}

void StatePgm_StatusDisplay( EEventType evt )
{
    static PgmData *pgm;

    switch( evt )
    {
        case INITIALISE:
        {
            pgm = getPgmData(currentPgm);
            lcdSendCommand( 0x01 ); //CLS
            break;
        }

        case BUT_MODE:
        {
            PgmNext();
            return;
        }

        case BUT_LEFT:
        {
            if( pgm->status & PGM_HAS_REP )
            {
                displayCurrentState = StatePgm_ForDisplay;
            }
            else
            {
                displayCurrentState = StatePgm_EveryDisplay;
            }
            return;
        }

        case BUT_RIGHT:
        {
            displayCurrentState = StatePgm_MainDisplay;
            return;
        }

        case TICK:
            break;

        default:
            return;
    }

    lcdSendCommand( 0x02 );// home
    lcdWriteString( "Toggle" );

    lcdSendCommand( 0xC0 );
    long at = (((long)dt_hour * 60L * 60L) + ((long)dt_minute * 60L) + (long)dt_second) % (long)(24L * 60L * 60L);

    if( pgm->toggleAt >=0 )
    {
        long left = pgm->toggleAt - at;
        char sz[9];
        sprintf(sz, "%04d", (int)left );
        lcdWriteString(sz);
    }
    else
    {
        char sz[9];
        sprintf(sz, "-1      " );
        lcdWriteString(sz);
    }
}
