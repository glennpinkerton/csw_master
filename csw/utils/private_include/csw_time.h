
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_time.h
*/


/*
 *  This header defines the CSWTimeUtil class.
 *  
 *  This class refactors the functions found in the old 
 *  csw_time.c file.
 *
 */

#ifndef CSW_TIME_H
#define CSW_TIME_H

class CSWTimeUtil
{

  private:

// Old file static variables are now class private variables.

    int            FormatType = 0;
    int            MonthFlag = 1;
    int            BaseTime = 0;


  public:

    CSWTimeUtil () {};
    ~CSWTimeUtil () {};


// It makes no sense to copy construct, move construct,
// assign or move assign an object of this class.  The
// various copy methods are flagged "delete" to prevent
// their use.

    CSWTimeUtil (const CSWTimeUtil &old) = delete;
    const CSWTimeUtil &operator=(const CSWTimeUtil &old) = delete;
    CSWTimeUtil (CSWTimeUtil &&old) = delete;
    const CSWTimeUtil &operator=(CSWTimeUtil &&old) = delete;

    int csw_SetTimeFormat (int format, int monthflag);
    int csw_FormatTime (int seconds, char *tstring);
    int csw_SetBaseTime (int basetime);

}; // end of class definition

#endif

/*
    end of header file
    add nothing below this endif
*/
