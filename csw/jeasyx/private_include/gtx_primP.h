
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
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

/*
    csw_portP.h is needed for the PC386IX define
*/

#  include "csw/utils/private_include/csw_portP.h"

#  define GRIDSIZE          5000
#  define MOATSIZE          0.5

#  ifdef PC386IX

#    define PFILLCHUNK        50
#    define SHAPECHUNK        50
#    define LINECHUNK         200
#    define SYMBCHUNK         200
#    define TEXTCHUNK         200

#  else

#    define PFILLCHUNK        100
#    define SHAPECHUNK        100
#    define LINECHUNK         300
#    define SYMBCHUNK         400
#    define TEXTCHUNK         300

#  endif

#endif

/*  add nothing below this endif  */
