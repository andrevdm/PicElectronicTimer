/* 
 * File:   timerMain.c
 * Author: andre
 *
 * Created on 13 May 2014, 10:42 AM
 */

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

bit hourRollover = 0;

int main(int argc, char** argv)
{
    initialisePgms();

    displayEvent = NONE;
    displayCurrentState = StateTime_TimeDisplay;

    currentButtonState = StateButton_Off;

    TRISA = 0b00001111;
    TRISB = 0b00000000;
    TRISC = 0b00000000;
    TRISD = 0b00000000;
    TRISE = 0b00000000;
    ADCON1 = 0x0f;
    ADCON0 = 0x0f;
    CMCON = 0x7;
    //CVRCON = 0x0;

    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;

    delay(100);

    lcdSendCommand( 0x38 );
    delay(10);
    lcdSendCommand( 0x38 );
    __delay_us(120);
    lcdSendCommand( 0x38 );
    delay(10);
    delay(10);

    lcdSendCommand( 0x38 );// 1:function, 1:8bit | 1:2 line, 0:5x8, 0, 0
    lcdSendCommand( 0x0C );// | 1:function, 1:display, 0:cursor, 0:blink
    lcdSendCommand( 0x02 );//Home
    lcdSendCommand( 0x01 );//CLS

    currentPgm = 1;

    OpenTimer0( TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_4 ); //If this changes also update _INT0_PRESCALE
    INTCONbits.TMR0IF = 0;
    ei();

    while(1)
    {
        if( backLightOnForSeconds > 0 )
        {
            LCD_BACKLIGHT = 1;
        }

        if( hourRollover )
        {
            hourRollover = 0;
            SaveTime();
        }

        if( tick )
        {
            updateRelays();

            if( --backLightOnForSeconds < 0 )
            {
                LCD_BACKLIGHT = 0;
                backLightOnForSeconds = 0;
            }
        }

        if( tick && displayEvent == NONE )
        {
            displayEvent = TICK;
            tick = 0;
        }

        if( displayEvent != NONE )
        {
            StateFunc lastState = displayCurrentState;

            displayCurrentState( displayEvent );
            displayEvent = NONE;

            if( lastState != displayCurrentState )
            {
                displayCurrentState( INITIALISE );
            }
        }
    }

    return (EXIT_SUCCESS);
}

void delay( int ms )
{
    while( ms > 0 )
    {
        __delay_ms(1);
        --ms;
    }
}

void interrupt TimerOverflow()
{
    short setTick = 0;

    if( INTCONbits.TMR0IF == 1 )
    {
        INTCONbits.TMR0IF = 0;

        bres += (65536 * _INT0_PRESCALE);

        if( bres > _CLOCK_FREQ )
        {
            LED_FLASH = ~LED_FLASH;

            bres -= _CLOCK_FREQ;

            if( !stopClock )
            {
               ++dt_second;

                if( dt_second >= 60 )
                {
                    dt_minute++;
                    dt_second=0;
                    hourRollover = 1;

                    if( dt_minute >= 60 )
                    {
                        dt_hour++;
                        dt_minute=0;

                        if( dt_hour >= 24)
                        {
                            dt_day++;
                            dt_hour=0;

                            if( (dt_day >= 29 && dt_month == 2 && !((dt_year%4) == 0))                                 // February in leap year
                             || (dt_day >= 30 && dt_month == 2 )                                                       // February in normal years
                             || (dt_day >= 31 && (dt_month == 4 || dt_month == 6 || dt_month == 9 || dt_month == 11 )) // All months with 30 days
                             || (dt_day >= 32 ) )
                            {
                                dt_month++;
                                dt_day=1;
                            }

                            if( dt_month >= 13 )
                            {
                                dt_year++;
                                dt_month = 1;
                            }
                        }
                    }
                }

                setTick = 1;
                dirty = 0;
            }
        }

        currentButtonState( BUTTON_CHECK_FREQ );

        tick = setTick;
    }
}
