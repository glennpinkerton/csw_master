
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/


/*
    system header files needed
*/
#include <time.h>


#include "csw/utils/include/csw_.h"

#include "csw/utils/include/csw_timer.h"

#include "csw/utils/private_include/csw_fileio.h"




/*
  ****************************************************************

                     c s w _ C p u D i s p

  ****************************************************************

  function name:    csw_CpuDisp   (integer)

  call sequence:    csw_CpuDisp()

  synopsis:         displays absolute and relative cpu time

  return values:    Always returns 0

*/

int CSWTimer::csw_CpuDisp(void)
{
    double           atime, dtime;
    long int         idirt;

/*  get absolute time from system calls  */

    idirt = csw_CpuTime(&atime);

    if(idirt) {
        puts("\nerror getting cpu time, system calls failed\n");
        return 0;
    }

/*  calculate delta time and display  */

    dtime = atime - basetime;
    basetime = atime;

    printf("\ntotal cpu = %9.2f\ndelta cpu = %9.2f\n", atime, dtime);

    return 0;

}  /*  end of csw_CpuDisp function  */


/*
  ****************************************************************

                    c s w _ C p u T i m e

  ****************************************************************

  function name:    csw_CpuTime  (integer)

  call sequence:    csw_CpuTime (&time)

  synopsis:         Report the total cpu time using system calls.

  return values:    status code

                    0 = successful execution
                    1 = system call failed

*/

int CSWTimer::csw_CpuTime(double *dltime)
{
    double           dirt;
    int              idirt;

/*  function to get cpu time in microseconds  */

    idirt = clock();

/*  convert to seconds (only about 36 minutes are available)  */

    dirt = (double) idirt;
    *dltime = dirt/(double)(CLOCKS_PER_SEC);

    return 0;

}    /*  end of csw_CpuTime function  */




/*
  **************************************************************************

                      c s w _ S t a r t T i m e r

  **************************************************************************

*/

int CSWTimer::csw_StartTimer (const char *text)
{
    char            *cenv, *c2, *cstr;
    char            line[500];
    time_t          ltime;

    CSWFileioUtil   fileio_util_obj;

    cenv = (char *)csw_getenv ("EASYX_TIMER_FILE");
    if (!cenv) {
        return 1;
    }
    c2 = (char *)csw_getenv ("EASYX_TIMER_APPEND");
    if (c2) {
        if (fileio_util_obj.csw_DoesFileExist (cenv)) {
            Timerfd = fileio_util_obj.csw_OpenFile (cenv, "w");
            fileio_util_obj.csw_SetFilePosition (Timerfd, 0, 2);
        }
        else {
            Timerfd = fileio_util_obj.csw_CreateFile (cenv, "w");
        }
    }
    else {
        Timerfd = fileio_util_obj.csw_CreateFile (cenv, "w");
    }
    if (Timerfd == -1) {
        return 1;
    }

    if (Timerfd == -1) {
        return 1;
    }

    if (BaseSec != -1) {
        return 1;
    }

    if (TimeLock == 1) return 1;

    line[0] = '\0';
    if (text) {
        if (text[0]) {
            ltime = (time_t)time(NULL);
            cstr = (char *)ctime (&ltime);
            sprintf (line, "\n\nTimer start for task: %s\non %s\n", text, cstr);
        }
    }
    if (line[0] == '\0') {
        BaseSec = -1;
        return 1;
    }

    fileio_util_obj.csw_BinFileWrite (line, strlen(line), 1, Timerfd);

    BaseSec = (int)time(NULL);
    csw_CpuTime (&BaseCpu);

    return 1;

}


/*
  **************************************************************************

                     c s w _ S t o p T i m e r

  **************************************************************************

*/

int CSWTimer::csw_StopTimer (void)
{
    int          sec, asec;
    double       dtime, atime;
    char         line[500];

    if (Timerfd == -1) return 1;
    if (BaseSec == -1) return 1;
    if (TimeLock == 1) return 1;

    sec = (int)time (NULL);
    asec = sec - BaseSec;

    sprintf (line, "    Clock time for task: %d\n", asec);
    fileio_util_obj.csw_BinFileWrite (line, strlen (line), 1, Timerfd);

    csw_CpuTime (&atime);
    dtime = atime - BaseCpu;

    sprintf (line, "    Cpu time for task  : %9.2f\n", dtime);
    fileio_util_obj.csw_BinFileWrite (line, strlen (line), 1, Timerfd);

    BaseSec = -1;

    return 1;

}  /*  end of function csw_StopTimer  */


/*
  **************************************************************************

                         c s w _ C l o s e T i m e r

  **************************************************************************
*/

int CSWTimer::csw_CloseTimer (void)
{
    if (Timerfd != -1) {
        fileio_util_obj.csw_CloseFile (Timerfd);
        Timerfd = -1;
    }

    return -1;

}  /*  end of function csw_CloseTimer  */




/*
  **************************************************************************

                         c s w _ L o c k T i m e r

  **************************************************************************
*/

int CSWTimer::csw_LockTimer (int val)
{

    TimeLock = val;

    return 1;

}  /*  end of function csw_LockTimer  */

