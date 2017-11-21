
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_fsync.c

      This is a thin wrap around the fsync system function.  Various
    combinations of POSIX and ANSI flags in different compilers can make
    warnings show up when fsync is defined via system header files.  This
    wrapper does not use any system headers and defines the function
    prototype for fsync itself.

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

    int fsync (int);
  /*
      The extern "C" block needs to be closed if this
      is being run through a C++ compiler.
  */
#    if defined (__cplusplus)  ||  defined (c_plusplus)
        }
#    endif


/*
  ****************************************************************

                        c s w _ f s y n c

  ****************************************************************

    Wrapper around system fsync function.

*/

int csw_fsync (int fd)
{
    int           istat;

    istat = fsync (fd);
    return istat;

}  /*  end of function csw_fsync  */

