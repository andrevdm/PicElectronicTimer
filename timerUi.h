/* 
 * File:   timerUi.h
 * Author: andre
 *
 * Created on 15 May 2014, 12:43 PM
 */

#ifndef TIMERUI_H
#define	TIMERUI_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum
{
    NONE,
    TICK,
    INITIALISE,
    BUT_MODE,
    BUT_MODE_HOLD,
    BUT_LEFT,
    BUT_RIGHT,
    BUT_TOGGLE
} EEventType;


//Type of method that represents a state
typedef void (*StateFunc)( EEventType evt );

extern StateFunc displayCurrentState;

//Display state machine states
void StateTime_TimeDisplay ( EEventType evt );
void StateTime_TimeEdit ( EEventType evt );
void StateTime_DateDisplay ( EEventType evt );
void StateTime_DateEdit ( EEventType evt );
void StateVer_Display ( EEventType evt );

void StatePgm_MainDisplay ( EEventType evt );
void StatePgm_MainEdit( EEventType evt );
void StatePgm_WeekDisplay( EEventType evt );
void StatePgm_WeekEdit( EEventType evt );
void StatePgm_FromDisplay( EEventType evt );
void StatePgm_FromEdit( EEventType evt );
void StatePgm_ToDisplay( EEventType evt );
void StatePgm_ToEdit( EEventType evt );
void StatePgm_EveryDisplay( EEventType evt );
void StatePgm_EveryEdit( EEventType evt );
void StatePgm_ForDisplay( EEventType evt );
void StatePgm_ForEdit( EEventType evt );
void StatePgm_StatusDisplay( EEventType evt );

void DisplayCurrentPgm();

volatile EEventType displayEvent;

extern volatile bit tick;
extern volatile long bres;
extern volatile bit dirty;
extern long currentPgm;
extern bit stopClock;
extern volatile int dt_second;
extern volatile int dt_minute;
extern volatile int dt_hour;
extern volatile int dt_day;
extern volatile int dt_month;
extern volatile int dt_year;


#ifdef	__cplusplus
}
#endif

#endif	/* TIMERUI_H */

