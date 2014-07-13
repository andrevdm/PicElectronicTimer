/* 
 * File:   dateTime.h
 * Author: andre
 *
 * Created on 17 May 2014, 2:11 PM
 */

#ifndef DATETIME_H
#define	DATETIME_H

#ifdef	__cplusplus
extern "C" {
#endif

int getDayOfWeek(long y, long m, long d );
const char* getMonthName( short i );
const char* getDayOfWeekName( short i );

#ifdef	__cplusplus
}
#endif

#endif	/* DATETIME_H */

