
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*  
    csw_filenam.c

    Functions to build file names for different paths.
    Currently the users home directory and the EASYX_PATH
    directory are supported. 

*/

#include <stdio.h>
#include <string.h>

#include "csw/utils/private_include/csw_window_system.h"

#include "csw/utils/include/csw_.h"




/*
******************************************************************

                    c s w _ H o m e F i l e
                 
******************************************************************

  function name:    csw_HomeFile    (int)

  call sequence:    csw_HomeFile (name1, name2)

  purpose:          build a home directory pathname to a file

  return value:     status code

                    0 = error in home translation
                    1 = normal successful completion

  calling parameters:

    name1     r    char*     string with file name
    name2     w    char*     full path name

        name2 must be long enough for the longest possible pathname

*/

int csw_HomeFile (const char *name1,
                  char *name2)
{
    char    *cdum;

/*
    get home environment variable translation
    and prepend it to the file name
*/
    cdum = csw_getenv ("home");
    if (cdum) {
        sprintf (name2, "%s/%s", cdum, name1);
    }
    else {
        cdum = csw_getenv ("HOME");
        if (cdum) {
            sprintf (name2, "%s/%s", cdum, name1);
        }
        else {
            return 0;
        }
    }

    return 1;

}  /*  end of function csw_HomeFile  */




/*
******************************************************************

                c s w _ S y s t e m F i l e

******************************************************************

  function name:    csw_SystemFile      (int)

  call sequence:    csw_SystemFile (name1, name2)

  purpose:          build a system file name based on file name1

  return value:     status code

                    0 = error building file name
                    1 = normal successful completion

  calling parameters:

    name1     r    char*     file name
    name2     w    char*     system file pathname

        name2 must be long enough for the longest possible pathname

*/

int csw_SystemFile (const char *name1,
                    char *name2)
{
    char      syspathdef[] = WINDOW_SYSTEM_DEFAULT_DIRECTORY;
    char      syspath[500];
    char      *ctmp;
   
    ctmp = csw_getenv ("EASYX_PATH");
    if (ctmp) {
        strcpy (syspath, ctmp);
    }
    else {
        strcpy (syspath, syspathdef);
    } 

    sprintf (name2, "%s/%s", syspath, name1);

    return 1;

}  /*  end of function csw_SystemFile  */
