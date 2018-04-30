
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_timer.h
*/



#ifndef CSW_TIMER_H
#define CSW_TIMER_H

#include  "csw/utils/private_include/csw_fileio.h"

class CSWTimer
{

  private:

    CSWFileioUtil  fileio_util_obj;

    int            Timerfd {-1};
    int            BaseSec {-1};
    int            TimeLock {0};
    double         BaseCpu {0.0};
    double         basetime {0.0};

  public:

    CSWTimer () {};
    ~CSWTimer () {csw_CloseTimer ();};

// Do not allow copy or move stuff.

  private:

    CSWTimer (const CSWTimer &other) {};
    const CSWTimer &operator= (const CSWTimer &other) {return *this;};
    CSWTimer (const CSWTimer &&other) {};
    const CSWTimer &operator= (const CSWTimer &&other) {return *this;};

  public:

    int csw_CpuDisp (void);
    int csw_CpuTime (double *);
    int csw_StartTimer (const char *);
    int csw_StopTimer (void);
    int csw_CloseTimer (void);
    int csw_LockTimer (int);

}; // end of class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
