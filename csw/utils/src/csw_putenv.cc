
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_putenv.c

      This is a thin wrap around the putenv system function.  Various
    combinations of POSIX and ANSI flags can cause putenv to be undeclared
    on some compilers.  Putting this in a file of its own isolates the 
    workaround needed for getting it to compile on various systems.

*/

#include "csw/utils/include/csw_hack.h"

  /*
      If a C++ compiler is running, all of the prototypes
      must be declared extern "C".
  */
#    if defined(__cplusplus)  ||  defined(c_plusplus)
        extern "C"
        {
#    endif

int putenv (char *);

  /*
      The extern "C" block needs to be closed if this
      is being run through a C++ compiler.
  */
#    if defined (__cplusplus)  ||  defined (c_plusplus)
        }
#    endif


/*
  ****************************************************************

                       c s w _ p u t e n v

  ****************************************************************

    Wrapper around system putenv function.

*/

int csw_putenv (const char *name)
{
    int           istat;

    istat = putenv ((char *)name);
    return istat;

}  /*  end of function csw_putenv  */

