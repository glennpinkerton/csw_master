
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_hack.h

    This file has function prototypes for the csw_ functions
    that are wraps around individual system library calls.
    Some of these will not compile without warnings across
    platforms due to different flags defined in the system
    header files (e.g POSIX, X_OPEN, etc.).
*/



#ifndef CSW_HACK_H
#define CSW_HACK_H

#include <stdio.h>

    /*
        extern C if C++
    */
#   if defined(c_plusplus) || defined(__cplusplus)
        extern "C" {
#   endif

        int csw_fileno (FILE *);
        int csw_putenv (const char *);
        int csw_fsync (int);
        int csw_getpid (void);
        FILE *csw_popen (const char *, const char *);
        int csw_pclose (FILE *);
        int csw_gethostname (char *, int);
        char *csw_cuserid(char *);
        int csw_errno(void);

    /*
        end of extern C if C++
    */
#   if defined(c_plusplus) || defined(__cplusplus)
        }
#   endif

#endif
