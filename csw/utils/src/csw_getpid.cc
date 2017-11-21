
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_getpid.c

      This is a thin wrap around the getpid system function.  Various
    combinations of POSIX and ANSI flags in different compilers can make
    warnings show up when getpid is defined via system header files.  This
    wrapper does not use any system headers and defines the function
    prototype for getpid itself.

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

    int getpid (void);

  /*
      The extern "C" block needs to be closed if this
      is being run through a C++ compiler.
  */
#    if defined (__cplusplus)  ||  defined (c_plusplus)
        }
#    endif


/*
  ****************************************************************

                      c s w _ g e t p i d

  ****************************************************************

    Wrapper around system getpid function.

*/

int csw_getpid (void)
{
    int           istat;

    istat = getpid ();
    return istat;

}  /*  end of function csw_getpid  */

