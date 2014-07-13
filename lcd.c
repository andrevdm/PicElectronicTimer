#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib/timers.h>
#include <pic18f4550.h>

#include "chipConfig.h"
#include "lcd.h"

int backLightOnForSeconds = 0;

void lcdStrobe()
{
    LCD_E = 0;
    __delay_ms(1);
    LCD_E = 1;
    __delay_ms(2);
}

void lcdSendCommand( int command )
{
    LCD_RS = 0;//RS

    lcdWriteToData( command );
    lcdStrobe();
}

void lcdWriteChar( char c )
{
    LCD_RS = 1;//RS

    lcdWriteToData( c );
    lcdStrobe();
}

void lcdWriteString( const char *s )
{
    while( *s )
    {
        lcdWriteChar( *s++ );
    }
}

void lcdWriteToData( int data )
{
    LCD_DATA_PORT = data;
//    LCD_DATA_0 = data & 1;
//    LCD_DATA_1 = data & 2;
//    LCD_DATA_2 = data & 4;
//    LCD_DATA_3 = data & 8;
//    LCD_DATA_4 = data & 16;
//    LCD_DATA_5 = data & 32;
//    LCD_DATA_6 = data & 64;
//    LCD_DATA_7 = data & 128;
}