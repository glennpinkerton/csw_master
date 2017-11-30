
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_fpatP.h

    This file has constants and structure definitions for the
    polygon pattern fill functions.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_fpatP.h.
#endif


#ifndef PLY_FPATP_H
#define PLY_FPATP_H

#  include "csw/utils/include/csw_.h"

/*
    constant definitions
*/

#define FILLPATINT               unsigned char
#define PLYMAXPAT                45
#define POINTS_PER_ARC           32
#define MAXCPRIMPARTS            5
#define MAXCPRIMPARTS2           2



/*
    structure definitions
*/

typedef struct {
    int            start,
                   npts,
                   flag;
}  POlygonClipPrimRec;

typedef struct {
    int          vec1,
                 nvec,
                 fvec1,
                 nfvec,
                 arc1,
                 narc,
                 farc1,
                 nfarc;
}  FIllPatRec;

typedef struct {
    int          pt1,
                 npt;
}  FIllVecRec;

typedef struct {
    int          x,
                 y,
                 r;
}  FIllArcRec;

/*
    add nothing below this endif
*/

#endif
