
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_pipe.c

      This is a thin wrap around the popen and pclose system functions.
    Combinations of POSIX and ANSI flags in different compilers can make
    compiler warnings when these functions are prototyped.  I have 
    separated the functionality into this file to ease the porting from
    system to system.

*/

#include <stdio.h>
#include "csw/utils/include/csw_hack.h"

  /*
      If a C++ compiler is running, all of the prototypes
      must be declared extern "C".
  */
#    if defined(__cplusplus)  ||  defined(c_plusplus)
        extern "C"
        {
#    endif

    FILE *popen (const char *, const char *);
    int pclose (FILE *);
    FILE *_popen (const char *, const char *);
    int _pclose (FILE *);

  /*
      The extern "C" block needs to be closed if this
      is being run through a C++ compiler.
  */
#    if defined (__cplusplus)  ||  defined (c_plusplus)
        }
#    endif




/*
  ****************************************************************

                      c s w _ p o p e n

  ****************************************************************

    Wrapper around system popen function.

*/

FILE *csw_popen (const char *c1, const char *c2)
{

#ifdef WINNT
    return _popen (c1, c2);
#else
    return popen (c1, c2);
#endif

}  /*  end of function csw_popen  */





/*
  ****************************************************************

                      c s w _ p c l o s e

  ****************************************************************

    Wrapper around system pclose function.

*/

int csw_pclose (FILE *fptr)
{

#ifdef WINNT
    return _pclose (fptr);
#else
    return pclose (fptr);
#endif

}  /*  end of function csw_pclose  */
