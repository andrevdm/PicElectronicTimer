#include "dateTime.h"

static const char *dt_monthNames[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dev"};
static const char *dt_dayOfWeekNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

//From http://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Implementation-dependent_methods_of_Sakamoto.2C_Lachman.2C_Keith_and_Craver
//day 1-31, month 1-12, year = ccyy
int getDayOfWeek(long y, long m, long d )
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= (m < 3) ? 1 : 0;
    return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

const char* getMonthName( short i )
{
    return dt_monthNames[i - 1];
}

const char* getDayOfWeekName( short i )
{
    return dt_dayOfWeekNames[i];
}
