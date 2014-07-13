/*
 * File:   timer.h
 * Author: andre
 *
 * Created on 18 May 2014, 7:01 AM
 */

#ifndef TIMER_H
#define	TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define RELAY1 LATAbits.LA4
#define RELAY2 LATAbits.LA5
#define RELAY3 LATEbits.LE0
#define RELAY4 LATEbits.LE1
#define RELAY5 LATEbits.LE2

typedef struct PgmData
{
    short pgm;
    short status;

    short weekDays;
    int fromH;
    int fromM;
    int toH;
    int toM;

    int everyM;
    int forS;
    short isRepOn;
    long toggleAt;

    short isOn;
} PgmData;

PgmData *getPgmData( int i );
bit isPgmOnAuto( PgmData *pgm );
bit isPgmForceOn( PgmData *pgm );
bit isPgmForceOff( PgmData *pgm );
void setPgmAuto( PgmData *pgm );
void setPgmForceOn( PgmData *pgm );
void setPgmForceOff( PgmData *pgm );
const char *getPgmAutoState( PgmData *pgm );
void setRelayOnOff( int rn, int on );
void updateRelays();
void initialisePgms();
void SavePgmToEeprom( PgmData *pgm );
void LoadPgmFromEeprom( PgmData *pgm );
void LoadTime();
void SaveTime();

extern const short PGM_AUTO;
extern const short PGM_FORCE_ON;
extern const short PGM_FORCE_OFF;
extern const short PGM_HAS_PERIOD;
extern const short PGM_HAS_REP;

void delay( int ms );

extern bit timeEdited;

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */
