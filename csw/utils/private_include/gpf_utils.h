
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_utils.h

    This header has function prototypes for private functions
    int gpf_utils.c.  The refactor to C++  (2017) doesn't absolutely
    require "classifying" the utils functions.  No static variables
    are used in the gpf_utils.cc file, so (hopefully) these functions
    will be thread safe as is.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_utils.h.
#endif


#include "csw/utils/include/csw_.h"

#ifndef GPF_UTILS_P_H
#define GPF_UTILS_P_H


/*
    Functions from gpf_utils.cc
*/
    int gpf_xylimits (CSW_F *xy, int npt,
                      CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gpf_xylimits2 (CSW_F *xy, int npt,
                       CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gpf_calclinedist1 (CSW_F *xy, int npts,
                           CSW_F xin, CSW_F yin, CSW_F *dist);
    int gpf_perpintpoint1 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                           CSW_F x, CSW_F y, CSW_F *xp, CSW_F *yp);
    int gpf_calclinedist2 (double *xy, int npts, double xin, double yin, double *dist);
    int gpf_calclinedist1xy (CSW_F *x, CSW_F *y, int npts,
                             CSW_F xin, CSW_F yin, CSW_F *dist);
    int gpf_calclinedist2xy (double *x, double *y, int npts,
                             double xin, double yin, double *dist);
    int gpf_linearinterp2 (double x1, double y1, double z1,
			               double x2, double y2, double z2,
                           double xpt, double ypt, double *zret);
    int gpf_perpintpoint2 (double x1, double y1, double x2, double y2,
                           double x, double y, double *xp, double *yp);
    int gpf_perpintpoint3 (double x1, double y1, double x2, double y2,
                           double x3, double y3,
                           double x, double y, double *xp, double *yp);
    int gpf_calctextdist1 (CSW_F xt, CSW_F yt, const char *text, int nc,
                           CSW_F size, int font, CSW_F ang,
                           CSW_F x, CSW_F y, CSW_F *dist);
    int gpf_calcdistance1 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2, CSW_F *dist);
    int gpf_calctextdist2 (double xt, double yt, const char *text, int nc,
                           double size, int font, double ang,
                           double x, double y, double *dist);
    int gpf_calcdistance2 (double x1, double y1, double x2, double y2,
                           double *dist);
    int gpf_CalcCirclePoints (CSW_F x, CSW_F y, CSW_F r,
                              CSW_F *xout, CSW_F *yout, int npts);
    int gpf_xandylimits (CSW_F *x, CSW_F *y, int npt,
                         CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gpf_xandylimits2 (CSW_F *x, CSW_F *y, int npt,
                          CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gpf_xandylimits3d (double *x, double *y, double *z, int npt,
                           double *x1, double *y1,
                           double *x2, double *y2,
                           double *z1, double *z2);
    int gpf_checkboxintersect (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                               CSW_F x11, CSW_F y11, CSW_F x22, CSW_F y22);
    int gpf_perpdistance1 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                           CSW_F x, CSW_F y, CSW_F *dist);
    int gpf_perpdistance2 (double x1, double y1, double x2, double y2,
                           double x, double y, double *dist);
    int gpf_perpdistancexyz (double x1, double y1, double z1,
                             double x2, double y2, double z2,
                             double x, double y, double z, double *dist);
    int gpf_perpdsquare (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                         CSW_F x, CSW_F y, CSW_F *dist);
    int gpf_HorTextBoxSize (int dlist_index,
                            const char *text, int font, CSW_F size, CSW_F gap,
                            CSW_F *width, CSW_F *height, int *nlinesout,
                            CSW_F page_units_per_inch);
    int gpf_FindClosest (CSW_F x, CSW_F y, CSW_F *xa, CSW_F *ya, int npts);
    int gpf_xlimits (CSW_F *x, int npt, CSW_F *x1, CSW_F *x2);
    int gpf_xlimits2 (CSW_F *x, int npt, CSW_F *x1, CSW_F *x2);
    int gpf_FormatNumber (CSW_F value, CSW_F logbase, int ndecin, char *buf);
    int gpf_polygonalize_line (double *x, double *y, void **tag, int npts, double tiny,
                               double *xout, double *yout, void **tagout, int *nout, int maxout);
    int gpf_check_for_self_intersection (double *x, double *y, int npts,
                                         int *seglist, int *nseg, double tiny,
                                         int nsegmax);
    int gpf_check_for_intersection (double *x, double *y, int npts,
                                double *xx, double *yy, int npts2,
                                double tiny);
    int gpf_resample_line_xy (
        double *xin, double *yin, double *zin, int npts,
        double avspace,
        double **xout, double **yout, double **zout, int *n_out);
    int gpf_resample_line (
        double *xin, double *yin, double *zin, int npts,
        double avspace,
        double **xout, double **yout, double **zout, int *n_out);
    int gpf_resample_variable_line (
        double *xin, double *yin, double *zin, int npts,
        double *spacings,
        double **xout, double **yout, double **zout, int *n_out);
    int gpf_resample_line_segments (
        double *xin, double *yin, double *zin, int npts,
        double avspace,
        double **xout, double **yout, double **zout, int *n_out);
    int gpf_CalcPolygonAreaAndCentroid (
        double      *x,
        double      *y,
        int         n,
        double      *xcenter,
        double      *ycenter,
        double      *area);
    int gpf_CalcXYDistanceAlongLine (
        double      *x,
        double      *y,
        int         npts,
        double      *dout);
    int gpf_CalcXYZDistanceAlongLine (
        double      *x,
        double      *y,
        double      *z,
        int         npts,
        double      *dout);

    int gpf_concat_lines_xy (
        double      *xlines,
        double      *ylines,
        double      *zlines,
        int         *lplines,
        int         *lflines,
        int         *nlines,
        double      tiny);
    int gpf_organize_crossing_lines_xy (
        double      *xlines,
        double      *ylines,
        double      *zlines,
        int         *lplines,
        int         *lflines,
        int         *nlines,
        int         npmax,
        int         nlmax,
        double      tiny);

/*
    functions from gpf_thinxy.cc

    As is the case for gpf_utils.cc, no static variables are used in
    gpf_thinxy.cc.  So, hopefully, these functions do not need to be
    "classified" to be thread safe.
*/
    int gpf_xythin2 (double *x, double *y, void **tag, int nin,
                     double dist1, double dist2,
                     double *xout, double *yout, void **tagout, int *nout);
    int gpf_xythin1 (CSW_F *x, CSW_F *y, int nin, CSW_F dist1, CSW_F dist2,
                     int qflag, CSW_F *xout, CSW_F *yout, int *nout);
    int gpf_xyxythin1 (CSW_F *xy, int nin, CSW_F dist1, CSW_F dist2,
                       int qflag, CSW_F *xyout, int *nout);
    int gpf_xyseparate (CSW_F *xy, int npt, CSW_F **xy2, int *memflg);
    int gpf_packxy (CSW_F *x, CSW_F *y, int npts, CSW_F **xy, int *memflg);
    int gpf_compressfillpoints (CSW_F *xyin, int npt, CSW_F **xy,
                                int *np2, int *memflag);
    int gpf_packxy2 (CSW_F *x, CSW_F *y, int npts, CSW_F *xy);
    int gpf_packxy_double (double *x, double *y, int npts, CSW_F *xy);

#endif
/*
    end of header file
    add nothing below this endif
*/
