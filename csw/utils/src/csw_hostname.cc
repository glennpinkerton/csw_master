
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_hostname.c

      This is a thin wrap around the gethostname system function.  Various
    combinations of POSIX and ANSI flags can cause gethostname to be undeclared
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

int gethostname (char *, int);

  /*
      The extern "C" block needs to be closed if this
      is being run through a C++ compiler.
  */
#    if defined (__cplusplus)  ||  defined (c_plusplus)
        }
#    endif


/*
  ****************************************************************

                 c s w _ g e t h o s t n a m e

  ****************************************************************

    Wrapper around system gethostname function.

*/

int csw_gethostname (char *name, int namelen)
{
    int           istat;

    istat = gethostname (name, namelen);
    return istat;

}  /*  end of function csw_gethostname  */

