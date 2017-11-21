
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_flistP.h

    This header file has the top level information for each
    text font.  This is included in gpf_font.c and gpf_fontlen.c
    for use by the server code and client code.  This is not a public
    header file.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_flistP.h.
#endif


/*
    Add nothing above this ifndef
*/
#ifndef GPF_FLISTP_H
#define GPF_FLISTP_H

#  include "csw/utils/include/csw_.h"

/*
    Define the top level font structure.
*/
typedef struct {
    long int    fontnum,
                curveflag,
                fillflag,
                shadownum;
    CSW_F       aspect,
                xslant,
                yslant;
} VFont;

/*
    Information for EasyX fonts.
*/
const  VFont        VFontList[] = {
            {1, 0, 0, 0, 1.0f, 0.0f, 0.0f},    /*  simple block unslanted font  */
            {1, 0, 0, 0, 1.0f, 0.25f, 0.0f},   /*  simple block slanted font  */
            {1, 1, 0, 0, 1.0f, 0.0f, 0.0f},    /*  simple curved unslantedfont  */
            {1, 1, 0, 0, 1.0f, 0.25f, 0.0f},   /*  simple curved slanted font  */
            {2, 0, 0, 0, 1.0f, 0.0f, 0.0f},    /*  complex block unslanted font  */
            {2, 0, 0, 0, 1.0f, 0.25f, 0.0f},   /*  complex block slanted font  */
            {2, 1, 0, 0, 1.0f, 0.0f, 0.0f},    /*  complex curved unslantedfont  */
            {2, 1, 0, 0, 1.0f, 0.25f, 0.0f},   /*  complex curved slanted font  */
            {3, 0, 0, 0, 1.0f, 0.0f, 0.0f},    /*  outline block polygon font  */
            {3, 0, 0, 0, 1.0f, 0.25f, 0.0f},   /*  slanted outline block polygon font */
            {4, 0, 0, 0, 1.0f, 0.0f, 0.0f},    /*  outline smooth polygon font */
            {4, 0, 0, 0, 1.0f, 0.25f, 0.0f},   /*  slanted outline smooth polygon font */
            {3, 0, 0, 1, 1.0f, 0.0f, 0.0f},    /*  left shade block polygon font */
            {3, 0, 0, 1, 1.0f, 0.25f, 0.0f},   /*  slanted left shade block polygon font */
            {3, 0, 0, 2, 1.0f, 0.0f, 0.0f},    /*  right shade block polygon font */
            {3, 0, 0, 2, 1.0f, 0.25f, 0.0f},   /*  slanted right shade block polygon font */
            {4, 0, 0, 3, 1.0f, 0.0f, 0.0f},    /*  left shade smooth polygon font */
            {4, 0, 0, 3, 1.0f, 0.25f, 0.0f},   /*  slanted left shade smooth polygon font */
            {4, 0, 0, 4, 1.0f, 0.0f, 0.0f},    /*  right shade smooth polygon font */
            {4, 0, 0, 4, 1.0f, 0.25f, 0.0f}    /*  slanted right shade smooth polygon font */
            };

/*
    Information for hershey fonts.
*/
const  VFont        HFontList[] = {
            {101, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 1      */
            {102, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 2      */
            {103, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 3      */
            {104, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 4      */
            {105, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 5      */
            {106, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 6      */
            {107, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 7      */
            {108, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 8      */
            {109, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 9      */
            {110, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 10     */
            {111, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 11     */
            {112, 0, 0, 0, 1.0f, 0.0f, 0.0f},  /*  hershey font number 12     */
            };

const  int          Numvfont=20; 

/*
    Add nothing below this endif
*/
#endif
