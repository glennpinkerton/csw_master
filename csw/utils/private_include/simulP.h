
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    simulP.h

    This file has prototypes and macro definitions used for simulating
    function failures in the software.  These are used in conjunction
    with the testcenter product from Centerline Software.  These are
    only used internally for test purposes by Colorado Softworks.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file simulP.h.
#endif


#ifndef SIMULP_H
#  define SIMULP_H

/*
  This does nothing any more.
*/
#  define MSL      csw_do_nothing();

/*  add nothing below this endif  */
#endif
