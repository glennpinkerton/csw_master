
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_cuserid.c

      This is a thin wrap around the cuserid system function.  Various
    combinations of POSIX and ANSI flags can cause cuserid to be undeclared
    on some compilers.  Putting this in a file of its own isolates the 
    workaround needed for getting it to compile on various systems.

    From Linux "man cuserid":
      Nobody knows precisely what cuserid() does - avoid  it  in
      portable  programs  -  avoid  it  altogether  -  use getp­
      wuid(geteuid()) instead, if that is what  you  meant.   DO
      NOT USE cuserid().

    From OpenBSD 2.9 "man cuserid":
      Due to irreconcilable differences in historic implementations,
      cuserid() was removed from the IEEE Std1003.1-1990 (``POSIX'')
      standard.

    From OSF/1 4.0 "man cuserid":
      The cuserid() function is scheduled to be withdrawn from a future
      version of the X/Open CAE Specification.

*/

#include "csw/utils/include/csw_hack.h"

#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

/*
  ****************************************************************

                 c s w _ c u s e r i d

  ****************************************************************

    Wrapper around system getcuserid function.

*/

char *csw_cuserid (char *name)
{
    char          *ctmp;

    ctmp = (char *) getpwuid (geteuid());
    name = name;

    return ctmp;

}  /*  end of function csw_cuserid  */
