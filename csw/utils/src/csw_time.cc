
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_time.cc

    Implement the methods og the CSWTimeUtil class, which
    refactors the old csw_time.c file.

    The class has functions to convert seconds since the 
    epoch (i.e. real time) into formatted time strings.
*/


/*
    system header files needed
*/
#include <stdio.h>
#include <string.h>
#include <time.h>

/*
    private header files.
*/
#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_time.h"



/*
  ********************************************************************

               c s w _ S e t T i m e F o r m a t

  ********************************************************************

    Set the private FormatType and MonthFlag variables.

*/

int CSWTimeUtil::csw_SetTimeFormat (int format, int monthflag)
{
    
    FormatType = format;
    MonthFlag = monthflag;

    return 1;

}  /*  end of function csw_SetTimeFormat  */






/*
  *********************************************************************

                 c s w _ S e t B a s e T i m e

  *********************************************************************

    The BaseTime private variable is added to the seconds parameter
  passed to csw_FormatTime.  This is useful if you get time data
  relative to the start of an event, and the absolute time for the
  start of the event is known.

*/

int CSWTimeUtil::csw_SetBaseTime (int basetime)
{

    BaseTime = basetime;

    return 1;

}  /*  end of function csw_SetBaseTime  */





/*
  ***************************************************************************

                    c s w _ F o r m a t T i m e

  ***************************************************************************

    Given a time in seconds since the epoch or since the previously 
  specified BaseTime, return a formatted string according to the most
  recent csw_SetTimeFormat call.  The string should be at least 30 bytes
  for the longest available date and time format.

*/

int CSWTimeUtil::csw_FormatTime (int seconds, char *tstring)
{
    int           cent, wday, yday, mon, mday, year, sec, min, hour;
    time_t        loctime;
    struct tm     *ts;
    char          *ctmp;

    const char   *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    const char   *days[] =   {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    seconds += BaseTime;

    loctime = (time_t)seconds;
    ts = localtime(&loctime);

    sec = ts->tm_sec;
    min = ts->tm_min;
    hour = ts->tm_hour;
    mday = ts->tm_mday;
    year = ts->tm_year;
    wday = ts->tm_wday;
    yday = ts->tm_yday;
    mon = ts->tm_mon;

    cent = 19;
    if (year < 50  ||  year > 99)
        cent = 20;

    switch (FormatType) {

        case CSW_NO_TIME_FORMAT:
            sprintf (tstring, "%d", seconds);
            break;

        case CSW_HOUR_MIN_SEC:
            sprintf (tstring, "%d:%02d:%02d", hour, min, sec);
            break;

        case CSW_MIN_SEC:
            sprintf (tstring, "%d:%02d", min, sec); 
            break;

        case CSW_HOUR_MIN:
            sprintf (tstring, "%d:%02d", hour, min);
            break;

        case CSW_HOUR:
            sprintf (tstring, "%d", hour);
            break;

        case CSW_MIN:
            sprintf (tstring, "%d", min);
            break;

        case CSW_DAY_HOUR_MIN:
            sprintf (tstring, "%s %d:%02d", days[wday], hour, min);
            break;

        case CSW_DAY_HOUR:
            sprintf (tstring, "%s %d", days[wday], hour);
            break;

        case CSW_DAY:
            sprintf (tstring, "%s", days[wday]);
            break;

        case CSW_MONTH_DATE:
            if (MonthFlag == CSW_USE_MONTH_NUMBER) {
                sprintf (tstring, "%d %d", mon+1, mday);
            }
            else {
                sprintf (tstring, "%s %d", months[mon], mday);
            }
            break;

        case CSW_MONTH:
            if (MonthFlag == CSW_USE_MONTH_NUMBER) {
                sprintf (tstring, "%d", mon+1);
            }
            else {
                sprintf (tstring, "%s", months[mon]);
            }
            break;

        case CSW_YEAR_MONTH_DATE:
            if (MonthFlag == CSW_USE_MONTH_NAME) {
                sprintf (tstring, "%d%02d %s %d", cent, year, months[mon], mday);
            }
            else {
                sprintf (tstring, "%d%02d %d %d", cent, year, mon+1, mday);
            }
            break;

        case CSW_YEAR_MONTH:
            if (MonthFlag == CSW_USE_MONTH_NAME) {
                sprintf (tstring, "%d%02d %s", cent, year, months[mon]);
            }
            else {
                sprintf (tstring, "%d%02d %d", cent, year, mon+1);
            }
            break;

        case CSW_YEAR:
            sprintf (tstring, "%d%02d", cent, year);
            break;

        case CSW_MONTH_DATE_YEAR:
            if (MonthFlag == CSW_USE_MONTH_NAME) {
                sprintf (tstring, "%s %d %d%02d", months[mon], mday, cent, year);
            }
            else {
                sprintf (tstring, "%d %d %d%02d", mon+1, mday, cent, year);
            }
            break;

        case CSW_MONTH_YEAR:
            if (MonthFlag == CSW_USE_MONTH_NAME) {
                sprintf (tstring, "%s %d%02d", months[mon], cent, year);
            }
            else {
                sprintf (tstring, "%d %d %d", mon+1, cent, year);
            }
            break;

        case CSW_DATE:
            sprintf (tstring, "%d", mday);
            break;

        case CSW_HOUR_MIN_DATE:
            sprintf (tstring, "%d:%02d %d", hour, min, mday); 
            break;

        case CSW_JUL_DAY:
            sprintf (tstring, "%d", yday);
            break;

        case CSW_JUL_DAY_YEAR:
            sprintf (tstring, "%d %d%02d", yday, cent, year);
            break;

        case CSW_DATE_MONTH_YEAR:
            if (MonthFlag == CSW_USE_MONTH_NAME) {
                sprintf (tstring, "%d %s %d%02d", mday, months[mon], cent, year);
            }
            else {
                sprintf (tstring, "%d %d %d%02d", mday, mon+1, cent, year);
            }
            break;

        case CSW_DATE_MONTH:
            if (MonthFlag == CSW_USE_MONTH_NAME) {
                sprintf (tstring, "%d %s", mday, months[mon]);
            }
            else {
                sprintf (tstring, "%d %d", mday, mon+1);
            }
            break;

        case CSW_FULL_DATE:
            ctmp = (char *)ctime (&loctime);
            strcpy (tstring, ctmp);
            break;

        default:
            sprintf (tstring, "%d", seconds);
            break;

    }

    return 1;

}  /*  end of function csw_FormatTime  */
