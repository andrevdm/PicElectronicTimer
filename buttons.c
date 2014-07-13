#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <plib/timers.h>
#include <pic18f4550.h>

#include "chipConfig.h"
#include "timerConfig.h"
#include "buttons.h"
#include "lcd.h"

volatile ButtonStateFunc currentButtonState;
volatile EEventType lastPressedButton;
volatile short buttonNextDelayMs = 0;

EEventType GetButtonPress()
{
    if( BUT_MODE_BIT )
    {
        backLightOnForSeconds = 5;
        return BUT_MODE;
    }

    if( BUT_LEFT_BIT )
    {
        backLightOnForSeconds = 5;
        return BUT_LEFT;
    }

    if( BUT_RIGHT_BIT )
    {
        backLightOnForSeconds = 5;
        return BUT_RIGHT;
    }

    if( BUT_TOGGLE_BIT )
    {
        backLightOnForSeconds = 5;
        return BUT_TOGGLE;
    }

    return NONE;
}

void StateButton_Off( short approxElapsedMs )
{
    lastPressedButton = GetButtonPress();

    if( lastPressedButton == NONE )
    {
        return;
    }

    buttonNextDelayMs = BUTTON_DEBOUNCE_MS;
    currentButtonState = StateButton_Debounce;
}

void StateButton_Debounce( short approxElapsedMs )
{
    buttonNextDelayMs -= approxElapsedMs;

    if( buttonNextDelayMs > 0 )
    {
        return;
    }

    buttonNextDelayMs = BUTTON_SHORTPRESS_MS;
    currentButtonState = StateButton_WaitingForShortPress;
    currentButtonState( 0 );
}

void StateButton_WaitingForShortPress( short approxElapsedMs )
{
    buttonNextDelayMs -= approxElapsedMs;

    EEventType pressedButton = GetButtonPress();

    if( lastPressedButton != pressedButton )
    {
        displayEvent = lastPressedButton;
        currentButtonState = StateButton_WaitingForRelease;
        return;
    }

    if( buttonNextDelayMs > 0 )
    {
        return;
    }

    //ugly hack, only hold support long press
    if( lastPressedButton == BUT_MODE )
    {
        currentButtonState = StateButton_WaitingForLongPress;
        buttonNextDelayMs = BUTTON_LONGPRESS_MS - BUTTON_SHORTPRESS_MS;
        return;
    }
    else
    {
        //Normal press
        displayEvent = lastPressedButton;
        currentButtonState = StateButton_WaitingForRelease;
    }
}

void StateButton_WaitingForLongPress( short approxElapsedMs )
{
    buttonNextDelayMs -= approxElapsedMs;

    EEventType pressedButton = GetButtonPress();

    if( lastPressedButton != pressedButton )
    {
        displayEvent = lastPressedButton;
        currentButtonState = StateButton_WaitingForRelease;
        currentButtonState( 0 );
        return;
    }

    if( buttonNextDelayMs <= 0 )
    {
        displayEvent = BUT_MODE_HOLD;
        currentButtonState = StateButton_WaitingForRelease;
        currentButtonState( 0 );
    }
}

void StateButton_WaitingForRelease( short approxElapsedMs )
{
    EEventType pressedButton = GetButtonPress();

    if( pressedButton != lastPressedButton )
    {
        lastPressedButton = NONE;
        currentButtonState = StateButton_Off;
    }
}
