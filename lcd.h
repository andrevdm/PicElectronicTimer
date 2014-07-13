/* 
 * File:   lcd.h
 * Author: andre
 *
 * Created on 15 May 2014, 12:38 PM
 */

#ifndef LCD_H
#define	LCD_H

#ifdef	__cplusplus
extern "C" {
#endif

#define LCD_E  LATCbits.LC0
#define LCD_RS LATCbits.LC1
//#define LCD_DATA_0 LATDbits.LD0
//#define LCD_DATA_1 LATDbits.LD1
//#define LCD_DATA_2 LATDbits.LD2
//#define LCD_DATA_3 LATDbits.LD3
//#define LCD_DATA_4 LATDbits.LD4
//#define LCD_DATA_5 LATDbits.LD5
//#define LCD_DATA_6 LATDbits.LD6
//#define LCD_DATA_7 LATDbits.LD7
#define LCD_DATA_PORT PORTD
#define LCD_BACKLIGHT LATCbits.LC7

void lcdSendCommand( int command );
void lcdWriteChar( char c );
void lcdWriteString( const char *s );
void lcdStrobe();
void lcdWriteToData( int data );

extern int backLightOnForSeconds;

#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

