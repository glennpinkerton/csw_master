
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_calc.h

    Define the CSWPolyCalc class.  This class refactors the functionality
    that was in the old ply_calc.c file.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_calc.h.
#endif



#ifndef PLY_CALC_H
#define PLY_CALC_H


#  include "csw/utils/include/csw_.h"
#  include "csw/utils/include/csw_errnum.h"

#  include "csw/utils/private_include/ply_gridvec.h"

/*
    define error codes
*/

#  define PLY_BADMALLOC     1    /* error in malloc call, probably out of memory */
#  define PLY_WORKSIZE      2    /* the work or output array sizes are too small */
#  define PLY_IDENTPOLYS    3    /* two components of a polygon set are identical */
#  define PLY_BADORIENT     4    /* could not figure out orientation of a polygon
                                    component, probably because of either a zero
                                    area component or a self intersecting component*/

class CSWPolyCalc
{

  public:

    CSWPolyCalc () {};

bool  _bbr_ = false;

  private:

    int       ReparentPolySet (double *, double *,
                                  int *, int,
                                  double, double, double, double,
                                  double *, double *,
                                  int *);
    int       ProcessExtra (CSWErrNum &err_obj,
                               double*, double*, int, int*, int*,
                               double*, double*, int, int*, int, int,
                               double*, double*, int*, int*);

    CSWPolyGridvec    ply_gridvec_obj;


  public:

    int ply_CalcIntersect (CSWErrNum &err_obj_ptr,
                           double *xs, double *ys, int *icomps, int ncomps,
                           double *xc, double *yc, int *icompc, int ncompc,
                           double *xout, double *yout, int *npout, int *icompout,
                           int *iholeout, int maxcomp, int maxhole, int maxpts);
    int ply_CalcXor (CSWErrNum &err_obj,
                     double *xs, double *ys, int *icomps, int ncomps,
                     double *xc, double *yc, int *icompc, int ncompc,
                     double *xout, double *yout,
                     int *npout, int *icompout, int *iholeout,
                     int maxcomp, int maxhole, int maxpts);
    int ply_CalcXor1 (CSWErrNum &err_obj,
                      CSW_F *xs, CSW_F *ys, int *icomps, int ncomps,
                      CSW_F *xc, CSW_F *yc, int *icompc, int ncompc,
                      CSW_F *xout, CSW_F *yout,
                      int *npout, int *icompout, int *iholeout,
                      int maxcomp, int maxhole, int maxpts);
/*
    int ply_CalcIntersect1 (CSWErrNum &err_obj,
                            CSW_F *xs, CSW_F *ys, int *icomps, int ncomps,
                            CSW_F *xc, CSW_F *yc, int *icompc, int ncompc,
                            CSW_F *xout, CSW_F *yout,
                            int *npout, int *icompout, int *iholeout,
                            int maxcomp, int maxhole, int maxpts);
*/
    int ply_ClipPlineToArea (int flag, double *xp, double *yp, int *ic, int ncin,
                             double *xvc, double *yvc, int *ivc, int nvc,
                             double *xline, double *yline, int nline,
                             double *xout, double *yout, int *iout, int *nout,
                             int maxpts, int maxcomps);
    int ply_ClipToBox1 (CSWErrNum &err_obj,
                        CSW_F *xs, CSW_F *ys, int *icomps, int ncomps,
                        CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                        CSW_F *xout, CSW_F *yout, int *icompout, int *ncompout,
                        int maxcomp, int maxpts);

}; // End of main class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
