
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_ldash.h

    Define structures and constants needed to support dashed line 
    drawing to the graphics system.

    This functionality is now refactored into the GPFLdash class,
    which replaces to old gpf_ldash.c functions.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_ldashP.h.
#endif


#ifndef GPF_LDASH_H
#define GPF_LDASH_H

#  include "csw/utils/include/csw_.h"

/*  define constants  */

#define MAXDASHPAT          100
#define MAXDASHPARTS        10

#define NDEFAULTDASH        30

/*  define data structures for dashed lines  */

typedef struct {
    int       ndash;
    CSW_F     dash[MAXDASHPARTS];
} LIneDashRec;


static const CSW_F                Fdash[] = {
                                       .05f, .05f,
                                       .1f, .1f,
                                       .25f, .25f,
                                       .5f, .5f,
                                       .05f, .1f,
                                       .1f, .05f,
                                       .1f, .25f,
                                       .25f, .1f,
                                       .25f, .05f,
                                       .25f, .5f,
                                       .5f, .05f,
                                       .5f, .1f,
                                       .5f, .25f,
                                       .05f, .05f, .05f, .1f,
                                       .05f, .1f, .05f, .25f,
                                       .1f, .05f, .1f, .25f,
                                       .1f, .1f, .1f, .25f,
                                       .25f, .1f, .25f, .25f,
                                       .25f, .25f, .25f, .5f,
                                       .05f, .05f, .1f, .05f,
                                       .05f, .05f, .25f, .05f,
                                       .05f, .05f, .5f, .05f,
                                       .1f, .05f, .25f, .05f,
                                       .1f, .05f, .5f, .05f,
                                       .25f, .05f, .5f, .05f,
                                       .05f, .1f, .1f, .1f,
                                       .05f, .1f, .25f, .1f,
                                       .1f, .1f, .25f, .1f,
                                       .1f, .1f, .5f, .1f,
                                       .25f, .25f, .5f, .1f
                                      };

static const int                   Idash[] = {2,2,2,2,2,2,2,2,2,2,2,2,2,
                                       4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4};



class GPFLdash
{

  public:

    GPFLdash () {};
    ~GPFLdash () {};

  private:



    LIneDashRec          DashData[MAXDASHPAT];

  public:

    int gpf_getserverdashdata (int ipat, int *ndash, CSW_F *fdash);
    int gpf_setserverdashdata (int ipat, int ndash, CSW_F *fdash);
    int gpf_initserverdashdata (void);

};  // end of main class definition


/*
    do not add anything after this endif
*/
#endif
