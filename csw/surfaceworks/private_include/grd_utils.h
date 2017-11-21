
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_utils.h

    Define the CSWGrdUtils class, which refactors the stuff that
    used to be in grd_utils.c.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grd_utils.h.
#endif


/*
    Add nothing above this line
*/
#ifndef GRD_UTILS_H
#  define GRD_UTILS_H

#include "csw/surfaceworks/include/grd_shared_structs.h"

/*
    Define some constants for the class
*/

#define MAX_BINS              2000
#define MAX_PLANE             100


#define _VALLEY_              1
#define _PLATEAU_             2

#define SOFT_NULL_VALUE       -1.e15f


class CSWGrdUtils;

#include "csw/surfaceworks/private_include/grd_arith.h"
#include "csw/surfaceworks/private_include/grd_calc.h"
#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_triangle_class.h"

class CSWGrdUtils
{

  private:

    CSWGrdArith    *grd_arith_ptr {NULL};
    CSWGrdCalc     *grd_calc_ptr {NULL};
    CSWGrdFault    *grd_fault_ptr {NULL};
    CSWGrdTriangle   *grd_triangle_ptr {NULL};

  public:

    CSWGrdUtils () {};
    ~CSWGrdUtils () {};

    void SetGrdArithPtr (CSWGrdArith *p) {grd_arith_ptr = p;};
    void SetGrdCalcPtr (CSWGrdCalc *p) {grd_calc_ptr = p;};
    void SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};
    void SetGrdTrianglePtr (CSWGrdTriangle *p) {grd_triangle_ptr = p;};

  private:

/*
  Old static file variables become private class vaariables
*/
    int            WildInteger {99999999};
    CSW_Float      WildFloat {1.e20f};
    int            GridErrNum {0};

    int            FlatStyle {0};

    CSW_Float      TinySum {0.0};

    CSW_Float      ExpandZmin {-1.e30f};
    CSW_Float      ExpandZmax {1.e30f};
    CSW_Float      GridZmin {-1.e30f};
    CSW_Float      GridZmax {1.e30f};


/*
  Old static file functions become private class member functions.
*/
    int ExpandForValue (CSW_F *grid1, CSW_F *grid2, int ncol, int nrow,
                        CSW_F nval, int eflag, CSW_F value, CSW_F fudge);
    int ExpandForZero (CSW_F *grid1, CSW_F *grid2, int ncol, int nrow,
                       CSW_F nval, int eflag, int flag);
    int FillFlatSpot (CSW_F *grid1, int ncol, int nrow,
                      CSW_F value, CSW_F null_value);

    int FindBestColumnAndRow (double x, double y,
                              CSW_F *grid, int ncol, int nrow, int nskip,
                              double x1, double y1, double xsp, double ysp,
                              int *jcol, int *irow);


  public:

    int grd_set_err (int);
    int grd_collect_points (CSW_F *x, CSW_F *y, CSW_F *z, int npts, int maxp,
                            CSW_F *xw, CSW_F *yw, CSW_F *zw, int *nw);
    int grd_limits_xy (CSW_F *x, CSW_F *y, int n,
                       CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int grd_bilin_interp (CSW_F *x, CSW_F *y, CSW_F *z, int npts,
                          CSW_F *grid, int ncol, int nrow, int nskip,
                          CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax);
    int grd_bicub_interp (CSW_F *xpts, CSW_F *ypts, CSW_F *zpts, int npts,
                          CSW_F nullval,
                          CSW_F *grid, int ncol, int nrow, int nskip,
                          CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax,
                          int irforce, int icforce);
    int grd_calc_plane (CSW_F *x, CSW_F *y, CSW_F *z, int nptsin, CSW_F *coef);
    int grd_calc_double_plane (double *x, double *y, double *z, int nptsin,
                               double *coef);
    int grd_expand_limits (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                           int ncol, int nrow, int ncoarse, int marginsize,
                           CSW_F *xx1, CSW_F *yy1, CSW_F *xx2, CSW_F *yy2,
                           int *ncol2, int *nrow2, int *col0, int *row0);
    int grd_simulation (void);
    int grd_step_interp
        (CSW_F *xloc, CSW_F *yloc, CSW_F *zloc, int nloc,
         CSW_F *grid, int ncol, int nrow,
         CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int grd_inverse_distance_average
        (CSW_F *x, CSW_F *y, CSW_F *z, int n, int dpower,
         CSW_F tiny, CSW_F *dsq, CSW_F *avg);
    int grd_colinear_check
        (CSW_F *x, CSW_F *y, int npts, CSW_F tiny, CSW_F *distmax);
    int grd_flat_check
        (CSW_F *x, CSW_F *y, CSW_F *z, int n,
         CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax, CSW_F zrange);
    int grd_calc_colinear_plane
        (CSW_F *x, CSW_F *y, CSW_F *z, int n, CSW_F xspace, CSW_F yspace,
         CSW_F *value);
    int grd_average_angles (CSW_F *angles, int nang,
                            CSW_F *avgout, CSW_F *spread);
    int grd_constant_grid (CSW_F *grid, char *mask, int ncol, int nrow,
                           CSW_F value);
    int grd_get_closest_point_value
        (CSW_F *x, CSW_F *y, CSW_F *z,
         int n, CSW_F *val);
    int grd_expand_one_level_new
        (CSW_F *grid1, CSW_F *grid2, int ncol, int nrow,
         CSW_F nval, int eflag, int flag, char *process_flags);
    int grd_fill_blended_cell
        (CSW_Blended *grid, char *done,
         int irow, int jcol, int nc, int ncol);
    int grd_expand_blended_values
        (CSW_Blended *grid, int ncol, int nrow,
         int nc, char *done,
         int fault_flag, int *closest_fault);
    int grd_fill_plateau
        (CSW_F *grid, int nc, int nr,
         CSW_F value, CSW_F null_value, CSW_F zrange);
    int grd_fill_valley (CSW_F *grid, int nc, int nr,
                         CSW_F value, CSW_F null_value, CSW_F zrange);
    double grd_vector_angle (double x0, double y0, double z0,
                             double x1, double y1, double z1,
                             double x2, double y2, double z2);
    int grd_compare_geoms (
        double x1, double y1, double x2, double y2, int nc1, int nr1,
        double x3, double y3, double x4, double y4, int nc2, int nr2);


    int grd_set_blend_min_max (int min, int max);
    int grd_interpolate_blended_grid
        (CSW_BlendedNode *grid, int ncol, int nrow,
         CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
         FAultLineStruct *faults, int nfaults,
         int firstflag,
         CSW_F x, CSW_F y, CSW_F range, CSW_Mixture *mixture);
    int grd_get_blended_grid_node
        (CSW_BlendedNode *grid, int ncol, int nrow,
         int col, int row,
         CSW_F range, CSW_Mixture *mixture);
    int grd_get_err (void);
    int grd_recommended_size
        (CSW_F *x, CSW_F *y, int npts,
         CSW_F *x1p, CSW_F *y1p, CSW_F *x2p, CSW_F *y2p,
         int *ncol, int *nrow);

    int grd_set_hard_null_values (CSW_F *, char *, int, int,
                                  CSW_F, int, CSW_F**);

    int grd_fill_nulls_new (CSW_F*, int, int, CSW_F, CSW_F*, char*, int);
    int grd_xyz_from_grid (CSW_F*, int, int, CSW_F,
                           CSW_F, CSW_F, CSW_F, CSW_F,
                           CSW_F*, CSW_F*, CSW_F*, int*, int);
    int grd_triangles_from_grid (CSW_F*, int, int,
                                 CSW_F, CSW_F, CSW_F, CSW_F,
                                 POint3D**, int*,
                                 TRiangleIndex**, int*);
    int grd_triangles_from_index (POint3D*,
                                  TRiangleIndex*, int,
                                  TRiangle3D*);

    int grd_hexagons_from_grid (CSW_F*, int, int,
                                CSW_F, CSW_F, CSW_F, CSW_F,
                                POint3D**, int*,
                                SIdeIndex**, int*,
                                HExagonIndex**, int*);
    int grd_hexagons_from_index (POint3D*,
                                SIdeIndex*,
                                HExagonIndex*, int,
                                HExagon3D*);


    int grd_contour_to_grid (COntourDataStruct *clinesin,
                             int nclines,
                             double *xin, double *yin, double *zin,
                             int nin,
                             double x1, double y1, double x2, double y2,
                             int ncol, int nrow,
                             CSW_F *grid, char *mask,
                             GRidCalcOptions *options);
    int grd_refine_contour_data (COntourDataStruct *clines,
                                 int nclines, int spline_flag,
                                 double spacing,
                                 COntourDataStruct **clout);
    void grd_cleanup_contour_data (COntourDataStruct *clist,
                                   int nlist);

    int grd_mask_with_polygon (char *mask, int ncol, int nrow,
                               double xmin, double ymin,
                               double xmax, double ymax,
                               int flag,
                               double *xpin, double *ypin,
                               int npin, int *icin, int *ipin);



}; // end of main class


/*
    Add nothing to this file below the following endif
*/
#endif
