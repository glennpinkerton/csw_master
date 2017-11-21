
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_tsurf.h

    Define the CSWGrdTsurf class.  This refactors the old code in
    grd_tsurf.c.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grd_tsurf.h.
#endif


/*
    Add nothing above this line
*/
#ifndef GRD_TSURF_H
#  define GRD_TSURF_H

#  include "csw/surfaceworks/include/grd_shared_structs.h"

/*
    Define constants and macros for the class.
*/
#define MAX_COEFS            50
#define MIN_COLS_TSURF       2
#define MIN_ROWS_TSURF       2

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
        (dmaxarg1) : (dmaxarg2))

#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
        (iminarg1) : (iminarg2))


class CSWGrdTsurf;

#include "csw/surfaceworks/private_include/grd_utils.h"
#include "csw/surfaceworks/private_include/grd_fileio.h"

class CSWGrdTsurf
{

  private:

    CSWGrdFileio *grd_fileio_ptr = NULL;
    CSWGrdUtils  *grd_utils_ptr = NULL;

    double  dmaxarg1 = 0.0, dmaxarg2 = 0.0;
    int     iminarg1 = 0, iminarg2 = 0;


  public:

    CSWGrdTsurf () {};
    ~CSWGrdTsurf () {};

    void  SetGrdFileioPtr (CSWGrdFileio *p) {grd_fileio_ptr = p;};
    void  SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};



    void grd_get_saved_svd_results (double *w, double *v, double *pn, int size);
    int grd_calc_perp_plane_fit
        (double *x, double *y, double *z, int npts, double *coef);
    int grd_calc_double_trend_surface (double*, double*, double*, int, int,
                                       double*);
    int grd_calc_trend_surface (CSW_F*, CSW_F*, CSW_F*, int, int,
                                CSW_F*);
    int grd_eval_trend_surface (CSW_F*, CSW_F*, CSW_F*, int, int,
                                CSW_F*);
    int grd_calc_trend_grid (CSW_F*, CSW_F*, CSW_F*, int, int,
                             CSW_F*, int, int,
                             CSW_F, CSW_F, CSW_F, CSW_F);

  private:

/*
 * Private arrays used for reporting the SVD results.
 * These are accessed by testing programs only.
 */
    double Svd_w_save[10],
           Svd_v_save[100],
           Svd_pn_save[10];

/*
  Old static functions become private class methods.
*/
    int    SurfaceFit (double*, double*, double*, int, int,
                       double*, int*);
    int    PerpPlaneFit (double *x, double *y, double *z, int npts,
                         double *coefs);
    int    PolyCoef (double, double, double*, int);
    int    SvdBackSub (double**, double*, double**,
                       int, int, double*, double*);
    int    SvdCmp (double **, int, int,
                   double*, double**);
    int    eval_trend_surface2
                  (double *dx, double *dy, double *dz, int npts,
                   int iorder, double *dcoef);

};  // end of main class definition

/*
    Add nothing to this file below the following endif
*/
#endif
