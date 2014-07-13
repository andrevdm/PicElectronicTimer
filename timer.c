#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib/timers.h>
#include <pic18f4550.h>

#include "chipConfig.h"
#include "timerConfig.h"
#include "lcd.h"
#include "timerUi.h"
#include "buttons.h"
#include "timer.h"

const short PGM_AUTO = 1;
const short PGM_FORCE_ON = 2;
const short PGM_FORCE_OFF = 4;
const short PGM_HAS_PERIOD = 8;
const short PGM_HAS_REP = 16;

const short MAGIC = 0b10011000;
const short EEPROM_LAYOUT_VER = 2 % 0b111;

bit timeEdited = 0;

PgmData pgmDataCache[MAX_PGMS];

void initialisePgms()
{
    for( int i = 1; i <= MAX_PGMS; ++i )
    {
        PgmData *pgm = getPgmData( i );

        pgm->pgm = i;
        pgm->weekDays = 0xff;
        pgm->status = PGM_FORCE_OFF;
        pgm->fromH = 0;
        pgm->toH = 23;
        pgm->toM = 59;
        pgm->everyM = i;
        pgm->forS = 10  * i;

        //Set toggle to now and next = on so that when the rep is enabled the
        // relay will be switched on and the period will be calculated again
        pgm->isRepOn = 0;
        pgm->toggleAt = -1;
    }

    int marker = eeprom_read(0x00);
    int magic = marker & 0b11111000;
    short isValid = 1;

    //Ensure that EEPROM was previously written to
    isValid &= (magic == MAGIC);

    //Get the EEPROM layout version.
    int version = marker & 0b111;
    isValid &= (version == EEPROM_LAYOUT_VER);

    //If valid then load, else overwrite with defaults
    if( isValid )
    {
        LoadTime();

        for( int i = 1; i <= MAX_PGMS; ++i )
        {
            PgmData *pgm = getPgmData( i );
            LoadPgmFromEeprom(pgm);
        }
    }
    else
    {
        marker = MAGIC | EEPROM_LAYOUT_VER;
        eeprom_write( 0x00, marker );

        for( int i = 1; i <= MAX_PGMS; ++i )
        {
            PgmData *pgm = getPgmData( i );
            SavePgmToEeprom(pgm);
        }
    }
}

short maxs( short a, short b )
{
    return a > b ? a : b;
}

void LoadTime()
{
    dt_year = maxs( 14, eeprom_read( 0xff - 0 ) % 20 );
    dt_month = maxs( 1, eeprom_read( 0xff - 1 ) % 12 );
    dt_day = maxs( 1, eeprom_read( 0xff - 2 ) % 31 );
    dt_hour = maxs( 0, eeprom_read( 0xff - 3 ) % 23 );
    dt_minute = maxs( 0, eeprom_read( 0xff - 4 ) % 59 );
}

void SaveTime()
{
    eeprom_write( 0xff - 0, dt_year );
    eeprom_write( 0xff - 1, dt_month );
    eeprom_write( 0xff - 2, dt_day );
    eeprom_write( 0xff - 3, dt_hour );
    eeprom_write( 0xff - 4, dt_minute );
    timeEdited = 1;
}

void LoadPgmFromEeprom( PgmData *pgm )
{
    int i = pgm->pgm;
    pgm->weekDays = maxs( 0, eeprom_read( (i * 8) + 1 ) % 128 );
    pgm->status = maxs( 0, eeprom_read( (i * 8) + 2 ) );
    pgm->fromH = maxs( 0, eeprom_read( (i * 8) + 3 ) % 23 );
    pgm->fromM = maxs( 0, eeprom_read( (i * 8) + 4 ) % 59 );
    pgm->toH = maxs( 0, eeprom_read( (i * 8) + 5 ) % 23 );
    pgm->toM = maxs( 0, eeprom_read( (i * 8) + 6 ) % 59 );
    pgm->everyM = maxs( 0, eeprom_read( (i * 8) + 7 ) % 999 );
    pgm->forS = maxs( 0, eeprom_read( (i * 8) + 8 ) % 999 );
}

void SavePgmToEeprom( PgmData *pgm )
{
    int i = pgm->pgm;
    eeprom_write( (i * 8) + 1, pgm->weekDays );
    eeprom_write( (i * 8) + 2, pgm->status );
    eeprom_write( (i * 8) + 3, pgm->fromH );
    eeprom_write( (i * 8) + 4, pgm->fromM );
    eeprom_write( (i * 8) + 5, pgm->toH );
    eeprom_write( (i * 8) + 6, pgm->toM );
    eeprom_write( (i * 8) + 7, pgm->everyM );
    eeprom_write( (i * 8) + 8, pgm->forS );
}

PgmData *getPgmData( int i )
{
    return &pgmDataCache[i-1];
}

bit isPgmOnAuto( PgmData *pgm )
{
    return (pgm->status & PGM_AUTO) !=  0;
}

bit isPgmForceOn( PgmData *pgm )
{
    if( isPgmOnAuto( pgm ) )
    {
        return 0;
    }

    return (pgm->status & PGM_FORCE_ON) != 0;
}

bit isPgmForceOff( PgmData *pgm )
{
    if( isPgmForceOn( pgm ) )
    {
        return 0;
    }

    return (pgm->status & PGM_FORCE_OFF) != 0;
}

void setPgmAuto( PgmData *pgm )
{
    pgm->status |= PGM_AUTO;
    pgm->status &= ~PGM_FORCE_ON;
    pgm->status &= ~PGM_FORCE_OFF;
}

void setPgmForceOn( PgmData *pgm )
{
    pgm->status |= PGM_FORCE_ON;
    pgm->status &= ~PGM_AUTO;
    pgm->status &= ~PGM_FORCE_OFF;
}

void setPgmForceOff( PgmData *pgm )
{
    pgm->status |= PGM_FORCE_OFF;
    pgm->status &= ~PGM_FORCE_ON;
    pgm->status &= ~PGM_AUTO;
}

const char *getPgmAutoState( PgmData *pgm )
{
    if( isPgmOnAuto( pgm ) )
    {
        return "Auto";
    }

    if( isPgmForceOn( pgm ) )
    {
        return "set-On";
    }

    return "set-Off";
}

void setRelayOnOff( int rn, int on )
{
    switch( rn )
    {
        case 0: {RELAY1 = on; break;}
        case 1: {RELAY2 = on; break;}
        case 2: {RELAY3 = on; break;}
        case 3: {RELAY4 = on; break;}
        case 4: {RELAY5 = on; break;}
    }
}

void updateRelays()
{
    for( int i = 0; i < MAX_PGMS; ++i )
    {
        PgmData *pgm = getPgmData( i + 1 );

        if( isPgmForceOff( pgm ) )
        {
            pgm->isOn = 0;
            setRelayOnOff( i, 0 );
            continue;
        }

        if( isPgmForceOn( pgm ) )
        {
            pgm->isOn = 1;
            setRelayOnOff( i, 1 );
            continue;
        }

        //pgm is on auto

        //pgm has neither period no rep, so assume off
        if( ((pgm->status & PGM_HAS_PERIOD) == 0) && ((pgm->status & PGM_HAS_REP) == 0) )
        {
            pgm->isOn = 0;
            setRelayOnOff( i, 0 );
            continue;
        }

        if( pgm->status & PGM_HAS_PERIOD )
        {
            long periodStart = (pgm->fromH * 60) + pgm->fromM;
            long periodEnd = (pgm->toH * 60) + pgm->toM;

            long now = (dt_hour * 60) + dt_minute;

            //if not in period - then relay is off
            // also switch off the rep so that it can start at the next period start
            if( ((periodStart <= periodEnd) && (now < periodStart || now >= periodEnd)) || //Same day e.g. 10:00 - 13:00
                ((periodStart > periodEnd) && (now >= periodEnd && now < periodStart))     //Over day e.g. 10:00 - 03:00
              )
            {
                pgm->isOn = 0;
                pgm->isRepOn = 0;
                pgm->toggleAt = -1;
                setRelayOnOff( i, 0 );
                continue;
            }
        }

        if( pgm->status & PGM_HAS_REP )
        {
          //Current HH:MM:SS in seconds
          long at = (((long)dt_hour * 60L * 60L) + ((long)dt_minute * 60L) + (long)dt_second) % (long)(24L * 60L * 60L);

          //when the next toggle is expected. The < 7000 check is to see if the time wrapped
          // i.e. a simple less that check will always be true after a wrap so the <7000 check
          // that the at and toggleAt are in the same day
          short toggleNow = (pgm->toggleAt < 0) || ((at >= pgm->toggleAt) && (at - pgm->toggleAt < 7000) );

          //If the time was edited, restart off period. It is likely that the time will be edited again shortly
          // e.g. time and then date. So switch off to get the most stable rep period
            if( timeEdited )
            {
              #ifdef TESTING
              printf( "**\r\n" );
              #endif

              timeEdited = 0;
              toggleNow = 1;
              pgm->isRepOn = 1;
            }

            #ifdef TESTING
            printf( "at = %d, toggleAt=%d, toggleNow=%d \r\n", (int)at, (int)pgm->toggleAt, (int)toggleNow );
            #endif

            //If not toggling just ensure that the relay in the correct state
            if( !toggleNow )
            {
              setRelayOnOff( i, pgm->isRepOn );
              continue;
            }

            if( pgm->isRepOn )
            {
              pgm->isOn = 0;
              pgm->isRepOn = 0;
              setRelayOnOff( i, 0 );
              pgm->toggleAt = (((long)dt_hour * 60L * 60L) + ((long)dt_minute * 60L) + (long)dt_second + ((long)pgm->everyM * 60L)) % (long)(24L * 60L * 60L);
              continue;
            }
            else
            {
              pgm->isOn = 1;
              pgm->isRepOn = 1;
              setRelayOnOff( i, 1 );
              pgm->toggleAt = (((long)dt_hour * 60L * 60L) + ((long)dt_minute * 60L) + (long)dt_second + (long)pgm->forS) % (long)(24L * 60L * 60L);
              continue;
            }
        }
        else
        {
            pgm->isOn = 1;
            setRelayOnOff( i, 1 );
            continue;
        }
    }
}
