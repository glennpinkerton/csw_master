
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_wrksizP.h

    defines size of work arrays for line and polygon plotting
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gtx_wrksizP.h.
#endif


/*  add nothing above this ifdef  */

#ifndef GTX_WRKSIZP_H
#define GTX_WRKSIZP_H

#  include "csw/utils/include/csw_.h"

#  define MAXPOLYPOINTS    100000
#  define MAXLINEPOINTS    100000

/*  
    MAXWORK and MAXLINE should be at least twice as large as the largest 
    of MAXPOLYPOINTS and MAXLINEPOINTS  
*/

#  define MAXWORK          200000
#  define MAXLINE          200000

/*  add nothing below this endif  */

#endif
