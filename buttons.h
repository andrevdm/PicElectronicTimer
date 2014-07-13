/* 
 * File:   buttons.h
 * Author: andre
 *
 * Created on 15 May 2014, 12:55 PM
 */

#ifndef BUTTONS_H
#define	BUTTONS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "timerUi.h"

//Button press state machine states
typedef void (*ButtonStateFunc)( short approxElapsedMs );

void StateButton_Off( short approxElapsedMs );
void StateButton_Debounce( short approxElapsedMs );
void StateButton_WaitingForShortPress( short approxElapsedMs );
void StateButton_WaitingForLongPress( short approxElapsedMs );
void StateButton_WaitingForRelease( short approxElapsedMs );

EEventType GetButtonPress();

extern volatile ButtonStateFunc currentButtonState;
extern volatile EEventType lastPressedButton;
extern volatile short buttonNextDelayMs;


#define BUTTON_DEBOUNCE_MS 30
#define BUTTON_SHORTPRESS_MS 100
#define BUTTON_LONGPRESS_MS 2000
#define BUTTON_CHECK_FREQ 22 // (1 / (_CLOCK_FREQ / _INT0_PRESCALE / 65536)) * 1000

#define BUT_MODE_BIT PORTAbits.RA0
#define BUT_LEFT_BIT PORTAbits.RA1
#define BUT_RIGHT_BIT PORTAbits.RA2
#define BUT_TOGGLE_BIT PORTAbits.RA3


#ifdef	__cplusplus
}
#endif

#endif	/* BUTTONS_H */

