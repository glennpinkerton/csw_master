
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_fileno.c

      This is a thin wrap around the fileno system function.  Various
    combinations of POSIX and ANSI flags in different compilers can make
    fileno be defined as a macro, which then causes a warning for an
    undeclared function or possibly even a syntax error.

      All of the compilers I have seen that make fileno a macro also
    have a function fileno.  If this is not the case, then this wrap will
    need to be modified.  Any use of fileno should always call this
    wrapper function rather than calling fileno directly.

*/

#undef _POSIX_SOURCE
#ifdef _OS_FREEBSD_
# undef _POSIX_C_SOURCE
# define _ANSI_SOURCE
#endif

#include <stdio.h>

#include "csw/utils/include/csw_hack.h"

#  undef fileno

  /*
      If a C++ compiler is running, all of the prototypes
      must be declared extern "C".
  */
#    if defined(__cplusplus)  ||  defined(c_plusplus)
        extern "C"
        {
#    endif

#ifndef _COMP_VISUALC_
    int fileno (FILE *);
#endif

  /*
      The extern "C" block needs to be closed if this
      is being run through a C++ compiler.
  */
#    if defined (__cplusplus)  ||  defined (c_plusplus)
        }
#    endif


/*
  ****************************************************************

                      c s w _ f i l e n o

  ****************************************************************

    Wrapper around system fileno function.

*/

int csw_fileno (FILE *fptr)
{
    int           istat;

#ifdef _COMP_VISUALC_
    istat = _fileno (fptr);
#else
    istat = fileno (fptr);
#endif
    return istat;

}  /*  end of function csw_fileno  */

