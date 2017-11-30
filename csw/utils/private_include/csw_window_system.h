
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    csw_window_system.h

    This file defines some constants usually associated with the
    window system but possible referenced elsewhere as well.  Since
    the constants can be referenced elsewhere, I separate this header
    from csw/easyx/private_include/gtx_window_system.h so that the
    actual X header files are not needed.

*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gtx_window_system.h.
#endif

/*  add nothing above this ifdef  */

# ifndef CSW_WINDOW_SYSTEM_H
#  define CSW_WINDOW_SYSTEM_H

#ifndef WINNT

# ifdef SOLARIS
#  define WINDOW_SYSTEM_DEFAULT_DIRECTORY \
"/usr/openwin/lib/app-defaults/"

# else
#  define WINDOW_SYSTEM_DEFAULT_DIRECTORY \
"/usr/lib/X11/app-defaults/"

# endif

#else

# define WINDOW_SYSTEM_DEFAULT_DIRECTORY "."

#endif

#endif
/*  add nothing below this endif  */
