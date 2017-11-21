
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
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
    The actual working part of this header is only
    included if the simulations are enabled in the make file.
*/
/*lint -emacro(525,MSL) suppress negative indentation messages when expanding */
#ifdef CSW_ENABLE_SIMULATIONS

#  define MSL    wrap_set_file_and_line_for_malloc((__FILE__), (__LINE__));

#  ifdef __STDC__

  /*
      functions from sim_wrappers.c or sim_stubs.c
  */
      int wrap_set_file_and_line_for_malloc (const char *file, int line);

#  endif

#else

/*
    If simulations are not enabled, define a do nothing MSL macro
*/
#  define MSL      csw_do_nothing();

#endif

/*  add nothing below this endif  */
#endif
