
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_calcdraw.h

    Define the GPFCalcdraw class.  This is a refactor of the old
    gpf_calcdraw.c functions.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_calcdrawP.h.
#endif



#ifndef GPF_CALCDRAW_H
#define GPF_CALCDRAW_H

#include "csw/utils/include/csw_.h"


/*  define some macros for the file  */

#define MAXPOLYCOMP          10
#define MAXWORKSIZE          200
#define MAXWORKSIZE2         400
#define MAXTEXTLEN           256
#define HOLEFLAG             1.e19f



class GPFCalcdraw
{

  private:

    CSW_F                 XYwork[MAXWORKSIZE2],
                          XYcutwork[MAXWORKSIZE2],
                          XYcuthole[MAXWORKSIZE2],
                          *XYworkp[MAXPOLYCOMP];
    int                   Zwork[MAXWORKSIZE];
    double                DXwork[MAXWORKSIZE],
                          DYwork[MAXWORKSIZE],
                          DXwork2[MAXWORKSIZE],
                          DYwork2[MAXWORKSIZE];
    int                   Iwork1[MAXPOLYCOMP],
                          Iwork2[MAXPOLYCOMP],
                          Iwork3[MAXPOLYCOMP];
    CSW_F                 fxmin = 0.0,
                          fymin = 0.0,
                          fxmax = 0.0,
                          fymax = 0.0,
                          Tiny = 0.0;

    int                   Graze (CSW_F x1, CSW_F y1,
                                 CSW_F x2, CSW_F y2, double eps);

  public:

    int gpf_SetClipWindow (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int gpf_transxyline (CSW_F x, CSW_F y, CSW_F angle, CSW_F *xy, int npts);
    int gpf_pcliprect (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                       CSW_F *xy, int npts, CSW_F holflg,
                       int maxout, int maxcout,
                       CSW_F *xyout, int *npout, int *nhout, int *icout);
    int gpf_polycliprect2 (CSW_F, CSW_F, CSW_F, CSW_F,
                           CSW_F*, int, int*, int, int,
                           CSW_F*, int*, int*, int*, int*);
    int gpf_polyholesep (CSW_F *xy, int npts, CSW_F holflg, int maxout,
                         CSW_F **xyout, int *nholes, int *iholes);
    int gpf_addholeflags (CSW_F *x, CSW_F *y, int *npts, int ncomp,
                          CSW_F **x1, CSW_F **y1, int *nptotal, int *memflg);
    int gpf_addholeflags2 (CSW_F *xy, int *npts, int ncomp,
                           CSW_F *xyout, int *nptotal);
    int gpf_removepolyholeflags (CSW_F **xyp, int nh, int *icomp, CSW_F *xyout);
    int gpf_cliplineprim (CSW_F *xyin, int nin,
                          CSW_F x1in, CSW_F y1in, CSW_F x2in, CSW_F y2in,
                          CSW_F *xyout, int *ncout, int *icout);
    int gpf_clipvec1 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                      CSW_F *x, CSW_F *y);
    int gpf_clipvec2 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                      CSW_F *x3, CSW_F *y3, CSW_F *x4, CSW_F *y4);
    int gpf_addlineclipcomp (CSW_F *xyin, int nin,
                             CSW_F *xyout, int *ncout, int *icout);
    int gpf_cliptextitem (CSW_F x, CSW_F y, int anchor, const char *text,
                          CSW_F angle, CSW_F size, int font,
                          CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                          char *textout, CSW_F *xout, CSW_F *yout);
    int gpf_find_box_corners (CSW_F  x, CSW_F  y, int anchor,
                              CSW_F  width, CSW_F  height, CSW_F  angle,
                              CSW_F  *cx, CSW_F  *cy);


    int gpf_addcutlines (CSW_F *xyin, int nin,
                         CSW_F **xyout, int *nout, int *memflag);
    int gpf_removecutlines (CSW_F *x, CSW_F *y, int ncomp, int *npts,
                            CSW_F **xout, CSW_F **yout, int *ncout, int **npout);
    int gpf_findholeclosure (CSW_F *xyin, int nin, CSW_F *xyout, int *nout, double dd);
    int gpf_findholeclosure2 (CSW_F *xin, CSW_F *yin, int nin,
                              CSW_F *xout, CSW_F *yout, int *nout, double dd);
    int gpf_insertholefast (CSW_F *xyin, int nin,
                            CSW_F *xyhole, int nhole, int *nout);
    int gpf_3pointcircle (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                          CSW_F x3, CSW_F y3,
                          CSW_F *xc, CSW_F *yc, CSW_F *r);
    int gpf_shortperpbisector (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                               CSW_F *xb1, CSW_F *yb1, CSW_F *xb2, CSW_F *yb2);
    int gpf_perpbisector (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                          CSW_F *xb1, CSW_F *yb1, CSW_F *xb2, CSW_F *yb2);
    int gpf_cliplineprim_z (CSW_F *xyin, int *zin, int nin,
                            CSW_F x1in, CSW_F y1in, CSW_F x2in, CSW_F y2in, int znull,
                            CSW_F *xyout, int *zout, int *ncout, int *icout);
    int gpf_addlineclipcomp_z (CSW_F *xyin, int *zin, int nin,
                               CSW_F *xyout, int *zout, int *ncout, int *icout);
    int gpf_close_polygon_holes (CSW_F *x, CSW_F *y, int *npts, int ncomp,
                                 CSW_F **xout, CSW_F **yout, int **npout);

};

/*
    end of header file
    add nothing below this endif
*/

#endif
