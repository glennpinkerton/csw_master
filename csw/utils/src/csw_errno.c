
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    This is an encapsulation of references to the errno global
    variable.  In a program linked with the MFC library from
    microsoft, errno is not available.  All direct references
    to errno should be replaced with calls to csw_errno(void)
    This function can hopefully have a version that works with 
    MFC (if we can find out what it is) and a version that
    works without MFC.
*/

#include <errno.h>
#include "csw/utils/include/csw_hack.h"

/*
  ****************************************************************

                       c s w _ e r r n o

  ****************************************************************

    Wrapper around system errno variable.

*/

int csw_errno (void)
{
    return errno;

}  /*  end of function csw_errno  */

