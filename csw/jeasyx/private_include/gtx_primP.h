
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_primP.h

    This file has some general constants defined for the graphic primitive
    display list functions.

*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gtx_primP.h.
#endif


/*  add nothing above this ifdef  */

#ifndef GTX_PRIMP_H
#  define GTX_PRIMP_H

#  include "csw/utils/include/csw_.h"

#  define GRIDSIZE          5000
#  define MOATSIZE          0.5

#  define PFILLCHUNK        100
#  define SHAPECHUNK        100
#  define LINECHUNK         300
#  define SYMBCHUNK         400
#  define TEXTCHUNK         300

#endif

/*  add nothing below this endif  */
