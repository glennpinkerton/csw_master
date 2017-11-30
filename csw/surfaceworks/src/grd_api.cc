
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_api.cc

    Implement the methods of the CSWGrdAPI class.

    The publicly documented and supported grid calculation functions
    are contained in this file.  In the current implementation these are
    just a thin wrap over nearly identical undocumented functions.  I made
    the separation to try and limit the application interface complexity
    (at least somewhat).

    Over time the limited complexity goal has fallen apart.  I probably 
    need another class or possibly a derived class which presents a very 
    simple interface to only the most popular functionality.

*/

#include <assert.h>
#include <math.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"
#include "csw/utils/private_include/simulP.h"
#include "csw/utils/private_include/gpf_utils.h"

#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_graph.h"

#include "csw/surfaceworks/include/grid_api.h"




/*
  ****************************************************************

                g r d _ S e t C a l c O p t i o n

  ****************************************************************

  function name:    grd_SetCalcOption               (int)

  call sequence:    grd_SetCalcOption (tag, ival, fval)

  purpose:          Set an option prior to calling grd_CalcGrid.

  return value:     status code

                    1 = success
                   -1 = unknown tag

  calling parameters:

        tag        r      int          Identifier tag for the option.
                                       These are listed in csw/surfaceworks/include/grid_api.h.
        ival       r      int          Value for an int parameter.
        fval       r      CSW_F    Value for a CSW_F parameter.

*/

int CSWGrdAPI::grd_SetCalcOption (int tag, int ival, CSW_F fval)
{
    int           istat;

    istat = grd_calc_obj.grd_set_calc_option (tag, ival, fval);
    return istat;

}  /*  end of function grd_SetCalcOption  */




/*
  ****************************************************************

              g r d _ S e t C a l c O p t i o n s

  ****************************************************************

  function name:    grd_SetCalcOptions               (int)

  call sequence:    grx_SetCalcOption (options)

  purpose:          Set all options prior to calling grd_CalcGrid.

  return value:     status code

                    1 = success
                   -1 = unknown tag

  calling parameters:

    options    r  GRidCalcOptions*   Pointer to grid option structure.

*/

int CSWGrdAPI::grd_SetCalcOptions (GRidCalcOptions *options)
{
    int           istat;

    istat = grd_calc_obj.grd_set_calc_options (options);
    return istat;

}  /*  end of function grd_SetCalcOptions  */




/*
  ****************************************************************

           g r d _ D e f a u l t C a l c O p t i o n s

  ****************************************************************

  function name:    grd_DefaultCalcOptions               (int)

  call sequence:    grx_DefaultCalcOption (options)

  purpose:          Default all options prior to calling grd_CalcGrid.

  return value:     status code

                    1 = success
                   -1 = unknown tag

  calling parameters:

    options    r  GRidCalcOptions*   Pointer to grid option structure.

*/

int CSWGrdAPI::grd_DefaultCalcOptions (GRidCalcOptions *options)
{
    int           istat;

    istat = grd_calc_obj.grd_default_calc_options (options);
    return istat;

}  /*  end of function grd_DefaultCalcOptions  */





/*
  ****************************************************************

                     g r d _ C a l c G r i d

  ****************************************************************

  function name:    grd_CalcGrid              (int)

  call sequence:    grd_CalcGrid (x, y, z, error, npts,
                                  grid, mask, report, ncol, nrow,
                                  x1, y1, x2, y2,
                                  faults, nfaults, options)

  purpose:          Calculate a uniform grid by interpolating the
                    specified data points.  The errors between input
                    points and the grid's estimate can be returned
                    in the error array.  A mask specifying which
                    grid points are outside the data area or
                    inside but far from control points can also
                    be returned in the mask array.  This function only
                    supports non faulted continuous surfaces.

                    The grid must have at least 2 columns and 2 rows.
                    The upper limit of grid size is controlled by available
                    virtual memory.  The algorithm needs workspace about 5
                    times the size of the grid, plus workspace about twice
                    the size of the data array.  For example, calculating
                    a 1000 by 1000 grid from 100000 points would need about
                    21 to 22 megabytes of work space.  Memory allocation errors
                    are generally caused by grids too large for available
                    memory.

  return value:     status code
                    -1 = error
                     1 = success

  errors:           1 = memory allocation error
                    2 = A NULL x, y, z, or grid array is specified.
                    3 = A wild parameter was encountered.
                    4 = ncol or nrow is less than 2
                    5 = x2 less than x1 or y2 less than y1
                    6 = npts less than 3
                    7 = number of points actually in the data area
                        is less than 3
                    8 = The points are distributed in a manner that
                        cannot be fit with a trend surface (probably
                        all points are coincident or colinear).
                    9 = The fault data are inconsistent.  Either faults is not
                        null and nfaults is zero, or faults is NULL and nfaults
                        is greater than zero.
                    11= Too many (>5000) points lie almost exactly on top of
                        a fault.
                    99 = The range of the x and or y values is too small
                        relative to the values to support CSW_F arithmetic.


  calling parameters:

    x        r    CSW_F*    Array of x coordinates
    y        r    CSW_F*    Array of y coordinates
    z        r    CSW_F*    Array of z values at the x,y coordinates
    error    w    CSW_F*    Optional array to receive errors at the
                                input points.  Set to NULL if not wanted.
    npts     r    int           Number of points in x, y, z and error.
    grid     w    CSW_F*    Array to receive the grid values.
    mask     w    char*         Optional array to receive a mask specifying
                                whether a grid node is outside the data
                                area, inside and well defined, or inside
                                and poorly defined.  A well defined node
                                has a mask value of zero.  A node outside
                                the data area has a mask value of 1.  A
                                poorly defined inside node has a mask
                                value of 2.  Specify a NULL pointer to
                                disable this feature.
    report   w    char**        If this is not null a report of the grid
                                vital statistics is returned in the pointer.
    ncol     r    int           Number of columns in the grid and mask.
    nrow     r    int           Number of rows in the grid and mask.
    x1       r    CSW_F     Minimum x coordinate of the grid.
    y1       r    CSW_F     Minimum y coordinate of the grid.
    x2       r    CSW_F     Maximum x coordinate of the grid.
    y2       r    CSW_F     Maximum y coordinate of the grid.
    faults   r    FAultLineStruct*
                                List of fault lines structures.
    nfaults  r    int           Number of fault line structures.
    options  r    GRidCalcOptions*
                                Optional option record pointer.  If NULL,
                                the global option state is used.  If not
                                NULL, options are taken from this structure
                                for this calculation only.


*/

int CSWGrdAPI::grd_CalcGrid (CSW_F *x, CSW_F *y, CSW_F *z, CSW_F *error, int npts,
                  CSW_F *grid, char *mask, char **report, int ncol, int nrow,
                  CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                  FAultLineStruct *faults, int nfaults,
                  GRidCalcOptions *options)
{
    int           istat, fsave, i;
    CSW_F         xmin, ymin;


    if (npts < -WildInt  ||  npts > WildInt) {
        grd_utils_obj.grd_set_err (3);
        return -1;
    }

    if (grid == NULL) {
        grd_utils_obj.grd_set_err (2);
        return -1;
    }

/*
    check the range of the input x and y values to see
    if it can support CSW_F precision arithmetic.
*/
    istat = csw_CheckRange (x, npts);
    if (!istat) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = csw_CheckRange (y, npts);
    if (!istat) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    if (faults  &&  nfaults < 1) {
        grd_utils_obj.grd_set_err (9);
        return -1;
    }

    if (!faults  &&  nfaults > 0) {
        grd_utils_obj.grd_set_err (9);
        return -1;
    }

/*
    subtract minimum x and y values from point arrays
*/
    xmin = 1.e30f;
    ymin = 1.e30f;
    for (i=0; i<npts; i++) {
        if (x[i] < xmin) xmin = x[i];
        if (y[i] < ymin) ymin = y[i];
    }

    for (i=0; i<npts; i++) {
        x[i] -= xmin;
        y[i] -= ymin;
    }
    x1 -= xmin;
    y1 -= ymin;
    x2 -= xmin;
    y2 -= ymin;

    grd_calc_obj.grd_set_output_shifts (xmin, ymin);

/*
    Define the fault vectors and set the faulting option.
*/
    fsave = 0;
    grd_fault_obj.grd_free_faults ();
    if (faults  &&  nfaults > 0) {

        istat = grd_fault_obj.grd_define_and_shift_fault_vectors (faults, nfaults, xmin, ymin);
        if (istat == -1) {
            return -1;
        }
        grd_calc_obj.grd_set_calc_option (GRD_FAULTED_GRID_FLAG, 1, 0.0f);
        if (options) {
            fsave = options->faulted_flag;
            options->faulted_flag = 1;
        }
    }

    istat = grd_calc_obj.grd_calc_grid (x, y, z, error, npts,
                           grid, mask, report, ncol, nrow,
                           x1, y1, x2, y2, options);

    if (istat == -1  &&  options) {
        options->error_number = grd_GetErr ();
    }

    if (faults  &&  nfaults > 0) {
        if (options) {
            options->faulted_flag = fsave;
        }
        grd_calc_obj.grd_set_calc_option (GRD_FAULTED_GRID_FLAG, 0, 0.0f);
    }

    grd_calc_obj.grd_set_output_shifts (0.0, 0.0);

    for (i=0; i<npts; i++) {
        x[i] += xmin;
        y[i] += ymin;
    }

    return istat;

}  /*  end of function grd_CalcGrid  */




/*
 *****************************************************************

            g r d _ C a l c P e r p P l a n e F i t

 *****************************************************************

  Calculate the coefficients of the plane that fits the set of
  points in the sense of minimizing the perpendicular distance
  to the plane.

*/

int CSWGrdAPI::grd_CalcPerpPlaneFit (
    double  *x,
    double  *y,
    double  *z,
    int     npts,
    double  *coef)
{
    int     istat;

    istat = grd_tsurf_obj.grd_calc_perp_plane_fit (
        x, y, z, npts, coef);
    return istat;
}


/*
  ****************************************************************

               g r d _ C a l c T r e n d S u r f a c e

  ****************************************************************

  function name:  grd_CalcTrendSurface        (int)

  call sequence:  grd_CalcTrendSurface (x, y, z, npts, iorder,
                                        coef)

  purpose:        Calculate the coefficients for an iorder trend surface
                  with a least squares best fit through the specified
                  points.

  return value:   1 = success
                 -1 = failure

  errors:         1 =  error allocating workspace
                  2 =  too few points for the order or
                       the order is out of the 1-8 range
                  3 =  the data cannot be fit, probably
                       because they are colinear or all
                       in the same location


  calling parameters:

    x       CSW_F*    r       array of x coordinates
    y       CSW_F*    r       array of y coordinates
    z       CSW_F*    r       array of z coordinates
    npts    int           r       number of points in the arrays
    iorder  int           r       order of the polynomial to calculate
                                  this must be 1 - 8
    coef    CSW_F*    w       returned coefficients

        Note:  You must supply a coef array large enough for the
               number of coefficients returned.  For the highest
               order polynomial supported, 45 coefficients are
               returned.


*/

int CSWGrdAPI::grd_CalcTrendSurface (CSW_F *x, CSW_F *y, CSW_F *z, int npts, int iorder,
                          CSW_F *coef)
{
    int          istat;

    istat = grd_tsurf_obj.grd_calc_trend_surface (x, y, z, npts, iorder,
                                    coef);
    return istat;

}  /*  end of function grd_CalcTrendSurface  */







/*
  ****************************************************************

             g r d _ E v a l T r e n d S u r f a c e

  ****************************************************************

  function name:  grd_EvalTrendSurface            (int)

  call sequence:  grd_EvalTrendSurface (x, y, z, npts, iorder, coef)

  purpose:        Evaluate the specified trend surface at the set of
                  x and y points.  The trend surface coefficients
                  should have been calculated by grd_CalcTrendSurface.

  return value:   1 = success
                 -1 = error

  errors:        1 = memory allocation failure
                 2 = order must be 1-8

  calling parameters:

    x        CSW_F*     r        Array of x coordinates.
    y        CSW_F*     r        Array of y coordinates.
    z        CSW_F*     w        Array for calculated z values.
    npts     int            r        number of points
    iorder   int            r        order of the polynomial
    coef     CSW_F*     r        coefficients as calculated by
                                     grd_CalcTrendSurface

        Note:  The coefficients should be the same as returned from
               grd_CalcTrendSurface.  If you rearrange the order of
               the coefficient array, the evaluation results will
               be bad.

*/

int CSWGrdAPI::grd_EvalTrendSurface (CSW_F *x, CSW_F *y, CSW_F *z, int npts,
                          int iorder, CSW_F *coef)
{
    int             istat;

    istat =grd_tsurf_obj.grd_eval_trend_surface (x, y, z, npts,
                                    iorder, coef);
    return istat;

}  /*  end of function grd_EvalTrendSurface  */






/*
  ****************************************************************

                g r d _ C a l c T r e n d G r i d

  ****************************************************************

  function name:  grd_CalcTrendGrid              (int)

  call sequence:  grd_CalcTrendGrid (x, y, z, npts, iorder,
                                     grid, ncol, nrow,
                                     xmin, ymin, xmax, ymax)

  purpose:        Calculate a grid for a trend surface.  The
                  surface coefficients are calculated and then
                  evaluated at the nodes of the specified grid.

  return value:   1 = success
                 -1 = error

  errors:        1 = memory allocation error
                 2 = iorder is outside of 1-8
                     range or npts is too low
                     for the order specified.
                 3 = The data cannot be fit, probably
                     because they are colinear or all
                     at the same location
                 4 = ncol, nrow is less than 2 or
                     xmin >= xmax or ymin >= ymax
                 5 = x, y, z or grid is NULL
                 99= x,y limits of data are too small for the
                     magnitude.  Use the double version of this
                     function.

  calling parameters:

    x         CSW_F*  r    array of x coordinates
    y         CSW_F*  r    array of y coordinates
    z         CSW_F*  r    array of z coordinates
    npts      int         r    number of points
    iorder    int         r    order of polynomial
    grid      CSW_F*  w    output grid
    ncol      int         r    number of columns in grid
    nrow      int         r    number of rows in grid
    xmin      CSW_F   r    minimum x coordinate of grid
    ymin      CSW_F   r    minimum y coordinate of grid
    xmax      CSW_F   r    maximum x or grid
    ymax      CSW_F   r    maximum y of grid

        Note:  grid must be large enough for ncol * nrow CSW_F numbers.

*/

int CSWGrdAPI::grd_CalcTrendGrid (CSW_F *x, CSW_F *y, CSW_F *z, int npts, int iorder,
                       CSW_F *grid, int ncol, int nrow,
                       CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax)
{
    int              istat;

    if (grid == NULL  ||  x == NULL  ||
        y == NULL  ||  z == NULL) {
        grd_utils_obj.grd_set_err (5);
        return -1;
    }

    istat = csw_CheckRange (x, npts);
    if (!istat) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = csw_CheckRange (y, npts);
    if (!istat) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = csw_CheckRange (z, npts);
    if (!istat) {
        ConstantGrid (grid, NULL, ncol, nrow, z[0]);
        return 1;
    }

    istat = grd_tsurf_obj.grd_calc_trend_grid (x, y, z, npts, iorder,
                                 grid, ncol, nrow,
                                 xmin, ymin, xmax, ymax);
    return istat;

}  /*  end of function grd_CalcTrendGrid  */





/*
  ****************************************************************

                       g r d _ G e t E r r

  ****************************************************************

  function name:    grd_GetErr               (int)

  call sequence:    grd_GetErr ()

  purpose:          Return the error number of the most recent
                    failure in the gridding library.  The number
                    does not reset upon a successful function call.
                    The comments for each function describe the
                    errors that can be returned.

  return value:     the error number

  errors:           none

  calling parameters:

    none

*/

int CSWGrdAPI::grd_GetErr (void)
{
    int          istat;

    istat = grd_utils_obj.grd_get_err ();
    return istat;

}  /*  end of function grd_GetErr  */





/*
  ****************************************************************

             g r d _ R e c o m m e n d e d S i z e

  ****************************************************************

  function name: grd_RecommendedSize              (int)

  call sequence: grd_RecommendedSize (x, y, npts,
                                      x1, y1, x2, y2,
                                      ncol, nrow)

  purpose:       Return the recommended number of columns and rows
                 for a specified set of points.  If the bounds data
                 are specified, only points within the bounds are
                 used to calculate the recommended size.  If the
                 bounds are specified as any of the following, the
                 limits of the specified data are used for the bounds.

                   If x1, y1, x2 or y2 is greater than 1.e20 or less
                   than -1.e20.

                   If x1 greater than or equal to x2 or if y1 greater
                   than or equal to y2.

  return value:    status code

                   -1 = error
                    1 = success

  errors:          1 = Error allocating memory.
                   2 = You specified less than 1 point.
                   3 = Either x or y is NULL.
                   4 = Either ncol or nrow is null.
                   5 = One of the boundary pointers is NULL.
                   7 = All points are in the same location.
                   99 = The range of the x and or y values is too small
                       relative to the values to support CSW_F arithmetic.

  calling parameters:

    x        r    CSW_F*     Array of x coordinates.
    y        r    CSW_F*     Array of y coordinates.
    npts     r    int            Number of points in x,y.
    x1       r/w  CSW_F*     Minimum x value
    y1       r/w  CSW_F*     Minimum y value
    x2       r/w  CSW_F*     Maximum x value
    y2       r/w  CSW_F*     Maximum y value
    ncol     w    int*           Recommended number of columns.
    nrow     w    int*           Recommended number of rows.

*/

int CSWGrdAPI::grd_RecommendedSize (CSW_F *x, CSW_F *y, int npts,
                         CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2,
                         int *ncol, int *nrow)
{
    int           istat;

    if (npts < -WildInt  ||  npts > WildInt) {
        grd_utils_obj.grd_set_err (3);
        return -1;
    }

/*
    check the range of the input x and y values to see
    if it can support CSW_F precision arithmetic.
*/
    istat = csw_CheckRange (x, npts);
    if (!istat) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = csw_CheckRange (y, npts);
    if (!istat) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = grd_utils_obj.grd_recommended_size (x, y, npts,
                                  x1, y1, x2, y2,
                                  ncol, nrow);
    return istat;

}  /*  end of function grd_RecommendedSize  */






/*
  ****************************************************************

            g r d _ S e t H a r d N u l l V a l u e s

  ****************************************************************

  function name:    grd_SetHardNullValues       (int)

  call sequence:    grd_SetHardNullValues (grid, mask, ncol, nrow,
                                           nullval, flag, gridout)

  purpose:          Set the elevation value in the grid to the specified
                    null value if the mask value at the node meets the
                    conditions specified by flag.  If flag = 0, only the
                    grid nodes outside of the data area are nulled.  If
                    flag = 1, the poorly controlled inside nodes are
                    also nulled.  If flag is 999, any non zero mask value
                    will have its z value set to the hard null value. For
                    any other flag value, zero is assumed.

  return value:     status code

                    -1 = error
                     1 = success

  errors:            2 = NULL grid or mask pointer
                     3 = wild parameter passed
                     4 = ncol or nrow is less than 2

  calling parameters:

    grid     r/w    CSW_F*    A 2-d grid previously cvalculated by
                                  grd_CalcGrid.
    mask     r      char*         The bad node mask associated with the grid.
    ncol     r      int           Number of grid columns.
    nrow     r      int           Number of grid rows.
    nullval  r      CSW_F     Hard null value for the grid.  This could
                                  be a huge positive number (e.g. 1.e30).
    flag     r      int           1 = null for inside bad nodes as well as
                                  outside
                                  any other value, null only for outside
                                  bad nodes.
    gridout  w      CSW_F**   Optional pointer to receive the output grid.
                                  If this is NULL, the input grid is overwritten.
                                  If not NULL, the function allocates space for the
                                  output grid and returns the pointer in this parameter.
                                  The calling function must csw_Free the space when it is
                                  finished with it.

*/

int CSWGrdAPI::grd_SetHardNullValues (CSW_F *grid, char *mask, int ncol, int nrow,
                           CSW_F nullval, int flag, CSW_F **gridout)
{
    int             istat;

    istat = grd_utils_obj.grd_set_hard_null_values (grid, mask, ncol, nrow,
                                      nullval, flag, gridout);

    return istat;

}  /*  end of function grd_SetHardNullValues  */





/*
  ****************************************************************

                     g r d _ S m o o t h G r i d

  ****************************************************************

  function name:    grd_SmoothGrid            (int)

  call sequence:    grd_SmoothGrid (grid, ncol, nrow, smfact,
                                    faults, nfaults,
                                    x1, y1, x2, y2,
                                    minval, maxval, smgrid)

  purpose:          Smooth a previously calculated grid.  The input
                    grid must not have any null values and must have
                    at least 8 columns and 8 rows.  The output can
                    overwrite the input grid if the smgrid pointer is
                    NULL.

                    The smoothing factor ranges from 1 (minimal smoothing)
                    to 9 (maximum smoothing).  For higher smoothing, you
                    will have to call this function again using the output
                    of the smoothing as the input to the next smoothing
                    pass.

  return value:     status code
                    -1 = error
                     1 = success

  errors:           1 = memory allocation error
                    2 = A NULL grid array is specifies.
                    3 = A wild parameter was encountered.
                    4 = ncol or nrow is less than 2

  calling parameters:

    grid     r    CSW_F*    Array of grid values.
    ncol     r    int           Number of columns in grid
    nrow     r    int           Number of rows in grid.
    smfact   r    int           qualitative smoothing factoe (1-9)
                                If this is less than zero (-1 to -9) then
                                the absolute value is used and the difference
                                (residual) between the original grid and the
                                smoothed grid is output.
    faults   r    FAultLineStruct    Array of fault line structures or NULL if
                                     this is not a faulted grid.
    nfaults  r    int           Number of fault line structures.
    x1       r    CSW_F     Minimum x of grid (used for faulted grid only)
    y1       r    CSW_F     Minimum y of grid (used for faulted grid only)
    x2       r    CSW_F     Maximum x of grid (used for faulted grid only)
    y2       r    CSW_F     Maximum y of grid (used for faulted grid only)
    minval   r    CSW_F     The minimum value allowed in the output grid.
    maxval   r    CSW_F     The maximum value allowed in the output grid.
    smgrid   w    CSW_F**   Array to store smoothing results.  If this is
                                NULL, the output overwrites the grid input.  If
                                this is not NULL, the function allocates memory
                                for the smoothed grid.

*/

int CSWGrdAPI::grd_SmoothGrid (CSW_F *grid, int ncol, int nrow, int smflag,
                    FAultLineStruct *faults, int nfaults,
                    CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                    CSW_F minval, CSW_F maxval, CSW_F **smgrid)
{
    int           istat;

    istat = grd_calc_obj.grd_smooth_grid (grid, ncol, nrow, smflag,
                             faults, nfaults,
                             x1, y1, x2, y2,
                             minval, maxval, smgrid);

    return istat;

}  /*  end of function grd_SmoothGrid  */






/*
  ****************************************************************

                      g r d _ W r i t e F i l e

  ****************************************************************

  function name:    grd_WriteFile                (int)

  call sequence:    grd_WriteFile (filename, comment,
                                   grid, mask,
                                   ncol, nrow,
                                   xmin, ymin, xmax, ymax,
                                   gridtype,
                                   faults, nfaults)

  purpose:          Write the specified grid and mask to a portable
                    binary data file.

  return value:     status code
                    -1 = error
                     1 = success

  errors:           1 = Error writing to disk.
                    2 = Cannot create the specified file name.
                    3 = The grid is a null pointer.
                    4 = A wild parameter value is specified.
                    5 = A hard null value is present in the grid.

  calling parameters:

    filename    r    char*      File to write to, relative to the current
                                directory.
    comment     r    char*      Optional comment up to 200 bytes long.
    grid        r    CSW_F* Grid values array, without hard nulls.
    mask        r    char*      Optional mask array or NULL if not used.
    ncol        r    int        Number of columns in the grid.
    nrow        r    int        Number of rows in the grid.
    xmin        r    double     Minimum x of the grid.
    ymin        r    double     Minimum y of the grid.
    xmax        r    double     Maximum x of the grid.
    ymax        r    double     Maximum y of the grid.
    gridtype    r    int        Flag specifying the type of grid. The
                                constants GRD_NORMAL_GRID_FILE, GRD_THICKNESS_GRID_FILE
                                and GRD_STEP_GRID_FILE can be used or you can use
                                some other numbers of your own.
    faults      r    FAultLineStruct*
                                List of faults (vertical, nonvertical and polygon blocks)
                                that were used to calculate the grid.  If no faults were
                                used, set this to NULL.
    nfaults     r    int        Number of structures in the faults array.

*/

int CSWGrdAPI::grd_WriteFile (const char *filename, const char *comment,
                   CSW_F *grid, char *mask, char *mask2,
                   int ncol, int nrow,
                   double xmin, double ymin, double xmax, double ymax,
                   int gridtype, FAultLineStruct *faults, int nfaults)
{
    int            istat;

    istat = grd_fileio_obj.grd_write_file (filename, comment,
                            grid, mask, mask2,
                            ncol, nrow,
                            xmin, ymin, xmax, ymax, gridtype,
                            faults, nfaults);
    return istat;

}  /*  end of grd_WriteFile function  */






/*
  ****************************************************************

                      g r d _ R e a d F i l e

  ****************************************************************

  function name:    grd_ReadFile                (int)

  call sequence:    grd_ReadFile (filename, comment,
                                  grid, mask,
                                  ncol, nrow,
                                  xmin, ymin, xmax, ymax,
                                  gridtype, faults, nfaults)

  purpose:          Read the specified file and return the grid, mask
                    and geometry data.  The grid and mask are allocated
                    by this function.  The calling function should csw_Free
                    them when done with them.

  return value:     status code
                    -1 = error
                     1 = success

  errors:           1 = Error reading from disk.
                    2 = Cannot open the specified file name.
                    3 = The grid is a null pointer.
                    4 = One of the geometry parameters is a NULL pointer.
                    5 = Error allocating grid, mask or fault data.
                    6 = The version of the file cannot be determined.
                    8 = The file is actually a multiple grid file.  Use
                        grd_ReadMultipleFile to read it.

  calling parameters:

    filename    r    char*      File to read from, relative to the current
                                directory.
    comment     w    char*      Optional comment at least 200 bytes long.
    grid        w    CSW_F**    Grid values array, without hard nulls.
    mask        w    char**     Optional mask array or NULL if not used.
    ncol        w    int*       Number of columns in the grid.
    nrow        w    int*       Number of rows in the grid.
    xmin        w    double*    Minimum x of the grid.
    ymin        w    double*    Minimum y of the grid.
    xmax        w    double*    Maximum x of the grid.
    ymax        w    double*    Maximum y of the grid.
    gridtype    w    int*       Flag specifying the type of grid.  The
                                constants GRD_NORMAL_GRID_FILE, GRD_THICKNESS_GRID_FILE
                                and GRD_STEP_GRID_FILE can be used or you can use
                                some other numbers of your own.
    faults      w    FAultLineStruct**
                                Array of fault lines if there are any in the file or
                                returned as NULL if no faults are in the file.
    nfaults     w    int*       Number of fault structures or zero if no faults exist.

*/

int CSWGrdAPI::grd_ReadFile (const char *filename, char *comment,
                  CSW_F **grid, char **mask, char **mask2,
                  int *ncol, int *nrow,
                  double *xmin, double *ymin, double *xmax, double *ymax,
                  int *gridtype, FAultLineStruct **faults, int *nfaults)
{
    int           istat;

    istat = grd_fileio_obj.grd_read_file (filename, comment,
                           grid, mask, mask2,
                           ncol, nrow,
                           xmin, ymin, xmax, ymax,
                           gridtype, faults, nfaults);
    return istat;

}  /*  end of grd_ReadFile function  */






/*
  ****************************************************************

                 g r d _ O n e G r i d A r i t h

  ****************************************************************

  function name:    grd_OneGridArith             (int)

  call sequence:    grd_OneGridArith (grid, gridout,
                                      ncol, nrow, tag, constant,
                                      nullvalue, funcptr, client_data)

  purpose:          Calculate a new grid by doing arithmetic
                    operations between a constant and each value in
                    an input grid.

  return value:     status code

                    -1 = error
                     1 = success

  errors:           2 = attempt to divide by zero
                    3 = bad log base
                    4 = unrecognized tag
                    5 = null function pointer when tag is GRD_CALL_FUNC
                    6 = zero or negative constant in GRD_POWER call
                    7 = ncol or nrow less than 2
                    98= aborted by the operator function

  calling parameters:

    grid      r/w   CSW_F*    Input grid data array.
    gridout   w     CSW_F*    Output grid array or NULL to
                                  overwrite the input.
    ncol      r     int           Number of columns in the grid.
    nrow      r     int           Number of rows in the grid.
    tag       r     int           Operation to perform from the following list.
                                  GRD_CALL_FUNC, GRD_ADD, GRD_SUBTRACT,
                                  GRD_CONSTANT_MINUS, GRD_MULTIPLY, GRD_DIVIDE,
                                  GRD_RECIPROCAL, GRD_EXPONENT, GRD_LOG,
                                  GRD_MINIMUM, GRD_MAXIMUM
    constant  r     CSW_F     The constant value to use in the operation.
    nullvalue r     CSW_F     Value which "bad" grid nodes are set to.  If
                                  this value is found in the input grid, no
                                  operation is done, and the output is set to
                                  the null value.
    funcptr   r     void (*)()    Pointer to a function to call to do the
                                  operation.  The function has a single parameter,
                                  a pointer to a GRidArithData structure.  This is
                                  only used if it is not NULL and if the tag is set
                                  to GRD_CALL_FUNC.
    client_data  r  void*         Pointer to data that the application wants to
                                  pass to the called function.


*/

int CSWGrdAPI::grd_OneGridArith (CSW_F *grid, CSW_F *gridout,
                      int ncol, int nrow, int tag, CSW_F constant,
                      CSW_F nullvalue, void (*funcptr)(GRidArithData*),
                      void *client_data)
{
    int           istat;

    istat = grd_arith_obj.grd_one_grid_arith (grid, gridout,
                                ncol, nrow, tag, constant,
                                nullvalue, funcptr, client_data);
    return istat;

}  /*  end of function grd_OneGridArith  */






/*
  ****************************************************************

                 g r d _ T w o G r i d A r i t h

  ****************************************************************

  function name:    grd_TwoGridArith             (int)

  call sequence:    grd_TwoGridArith (grid1, mask1, x11, y11, x12, y12, ncol1, nrow1,
                                      faults1, nfaults1,
                                      grid2, mask2, x21, y21, x22, y22, ncol2, nrow2,
                                      faults2, nfaults2,
                                      nullvalue,
                                      gridout, x1out, y1out, x2out, y2out, ncout, nrout,
                                      tag, funcptr, client_data)

  purpose:          Calculate a new grid by doing arithmetic
                    operations between two input grids.

  return value:     status code

                    -1 = error
                     1 = success

  errors:           1 = error allocating memory
                    2 = no intersection between the two grids
                    4 = unrecognized tag
                    5 = Either ncol or nrow is less than 2.
                    6 = A null pointer for an output parameter.
                    8 = tag is GRD_CALL_FUNC but the funcptr is NULL
                    98= calculation aborted by operator function.
                    99= x,y range is too low for magnitude.  Use the
                        double version of this function.

  calling parameters:

    grid1     r     CSW_F*    First input grid data array.
    mask1     r     char*         Mask array for bad nodes in first grid.
    x11       r     CSW_F     minimum x of first grid
    y11       r     CSW_F     minimum y of first grid
    x12       r     CSW_F     maximum x of first grid
    y12       r     CSW_F     maximum y of first grid
    ncol1     r     int           number of columns in first grid
    nrow1     r     int           number of rows in first grid
    faults1   r     FAultLineStruct*  Array of fault lines for the first grid.
    nfaults1  r     int           number of fault lines for the first grid.
    grid2     r     CSW_F*    Second input grid data array.
    mask2     r     char*         Mask array for bad nodes in second grid.
    x21       r     CSW_F     minimum x of second grid
    y21       r     CSW_F     minimum y of second grid
    x22       r     CSW_F     maximum x of second grid
    y22       r     CSW_F     maximum y of second grid
    ncol2     r     int           number of columns in second grid
    nrow2     r     int           number of rows in second grid
    faults2   r     FAultLineStruct*  Array of fault lines for the second grid.
    nfaults2  r     int           number of fault lines for the second grid.
    nullvalue r     CSW_F     Value which "bad" grid nodes are set to.  If
                                  this value is found in either input grid, no
                                  operation is done, and the output is set to
                                  the null value.
    gridout   w     CSW_F**   Returned output grid.  This is allocated by the
                                  function, and the application must csw_Free it.
    maskout   w     char**        Bad node mask for output grid.
    x1out     w     CSW_F*    Minimum x of output grid.
    y1out     w     CSW_F*    Minimum y of output grid.
    x2out     w     CSW_F*    Maximum x of output grid.
    y2out     w     CSW_F*    Maximum y of output grid.
    ncout     w     int*          Number of columns in output grid.
    nrout     w     int*          Number of rows in output grid.
    tag       r     int           Operation to perform from the following list.
                                  GRD_CALL_FUNC, GRD_ADD, GRD_SUBTRACT,
                                  GRD_MULTIPLY, GRD_DIVIDE,
                                  GRD_EXPONENT, GRD_MINIMUM, GRD_MAXIMUM
    funcptr   r     void (*)()    Pointer to a function to call to do the
                                  operation.  The function has a single parameter,
                                  a pointer to a GRidArithData structure.  This is
                                  only used if it is not NULL and if the tag is set
                                  to GRD_CALL_FUNC.
    client_data  r  void*         Pointer to data that the application wants to
                                  pass to the called function.


*/
int CSWGrdAPI::grd_TwoGridArith (CSW_F *grid1, char *mask1,
                      CSW_F x11, CSW_F y11, CSW_F x12, CSW_F y12,
                      int ncol1, int nrow1,
                      FAultLineStruct *faults1, int nfaults1,
                      CSW_F *grid2, char *mask2,
                      CSW_F x21, CSW_F y21, CSW_F x22, CSW_F y22,
                      int ncol2, int nrow2,
                      FAultLineStruct *faults2, int nfaults2,
                      CSW_F nullvalue,
                      CSW_F **gridout, char **maskout,
                      CSW_F *x1out, CSW_F *y1out,
                      CSW_F *x2out, CSW_F *y2out, int *ncout, int *nrout,
                      int tag, void (*funcptr)(GRidArithData*), void *client_data)
{
    int           istat;

    istat = csw_CheckRange2 (x11, y11, x12, y12);
    if (istat == 0) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = csw_CheckRange2 (x21, y21, x22, y22);
    if (istat == 0) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = grd_arith_obj.grd_two_grid_arith (grid1, mask1, x11, y11, x12, y12, ncol1, nrow1,
                                faults1, nfaults1,
                                grid2, mask2, x21, y21, x22, y22, ncol2, nrow2,
                                faults2, nfaults2,
                                nullvalue,
                                gridout, maskout,
                                x1out, y1out, x2out, y2out, ncout, nrout,
                                tag, funcptr, client_data);
    return istat;

}  /*  end of function grd_TwoGridArith  */




/*
  ****************************************************************

                g r d _ R e s a m p l e G r i d

  ****************************************************************

  function name:  grd_ResampleGrid                  (int)

  call sequence:  grd_ResampleGrid (grid, mask, ncol, nrow,
                                    x1, y1, x2, y2,
                                    faults, nfaults,
                                    newgrid, newmask, newncol, newnrow,
                                    newx1, newy1, newx2, newy2, flag)

  purpose:        Resample a subset of an existing grid using a different
                  grid geometry.

  return value:   status code

                  1 = success
                 -1 = error

  errors:        1 = memory allocation error
                 2 = either grid or newgrid is NULL
                 3 = if mask is not null, newmask must also be non null
                 4 = either the original or new limits are inconsistent
                 6 = less than 2 columns or rows specified for either the
                     original or new grid.
                 7 = The new grid has more than 2000 times as many nodes
                     as the original grid.  This applies only to a faulted
                     grid.
                 99= The limits of the grid or new grid are too small for
                     the magnitude of the numbers.  Use the double version
                     of this function.

  calling parameters:

    grid      r   CSW_F*      Original grid data.
    mask      r   char*           Optional mask of original grid.
    ncol      r   int             Number of columns in original grid.
    nrow      r   int             Number of rows in original grid.
    x1        r   CSW_F       Minimum x of original grid.
    y1        r   CSW_F       Minimum y of original grid.
    x2        r   CSW_F       Maximum x or original grid.
    y2        r   CSW_F       Maximum y of original grid.
    faults    r   FAultLineStruct*  Array of fault line structures used to
                                    calculate the input grid.
    nfaults   r   int             Number of fault lines.
    newgrid   w   CSW_F*      New grid data.
    newmask   w   char*           Optional mask of new grid.
    newncol   r   int             Number of columns in new grid.
    newnrow   r   int             Number of rows in new grid.
    newx1     r   CSW_F       Minimum x of new grid.
    newy1     r   CSW_F       Minimum y of new grid.
    newx2     r   CSW_F       Maximum x or new grid.
    newy2     r   CSW_F       Maximum y of new grid.
    flag      r   int             Either GRD_BILINEAR, GRD_BICUBIC or
                                  GRD_STEP_GRID to specify the type of
                                  interpolation to use when resampling.

*/

int CSWGrdAPI::grd_ResampleGrid (CSW_F *grid, char *mask, int ncol, int nrow,
                      CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                      FAultLineStruct *faults, int nfaults,
                      CSW_F *newgrid, char *newmask, int newncol, int newnrow,
                      CSW_F newx1, CSW_F newy1, CSW_F newx2, CSW_F newy2,
                      int flag)
{
    int         istat;

    istat = csw_CheckRange2 (x1, y1, x2, y2);
    if (istat == 0) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = csw_CheckRange2 (newx1, newy1, newx2, newy2);
    if (istat == 0) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = grd_arith_obj.grd_resample_grid (grid, mask, ncol, nrow,
                               x1, y1, x2, y2,
                               faults, nfaults,
                               newgrid, newmask, newncol, newnrow,
                               newx1, newy1, newx2, newy2,
                               flag);

    return istat;

}  /*  end of function grd_ResampleGrid  */





/*
  ****************************************************************

             g r d _ B a c k I n t e r p o l a t e

  ****************************************************************

  function name:  grd_BackInterpolate             (int)

  call sequence:  grd_BackInterpolate (grid, ncol, nrow,
                                       x1, y1, x2, y2,
                                       faults, nfaults,
                                       x, y, z, npts,
                                       flag)

  purpose:        Return the zvalues calculated from interpolating
                  the grid at the specified x,y locations.  If a
                  location is outside the grid or if the grid has
                  null values nearest the point, the z value for the
                  point is set to 1.e30.

  return value:   status code

                  1 = success
                 -1 = error

  errors:         2 = A null grid, x, y, or z pointer was passed.
                  3 = Either ncol or nrow is less than 2.
                  4 = The grid limits are inconsistent (x1 >= x2 or y1 >= y2).
                  5 = The npts parameter is less than 1.
                  99= The x,y limits and/or coordinates cannot be supported
                      by single precision arithmetic.

  calling parameters:

    grid    r   CSW_F*    Array of grid data.
    ncol    r   int           Number of grid columns.
    nrow    r   int           Number of grid rows.
    x1      r   CSW_F     Minimum x coordinate of the grid.
    y1      r   CSW_F     Minimum y of the grid.
    x2      r   CSW_F     Maximum x of the grid.
    y2      r   CSW_F     Maximum y of the grid.
    faults  r   FAultLineStruct*   Array of fault lines used to calculate
                              the original grid.
    nfaults r   int           Number of fault lines.
    x       r   CSW_F*    Array of point x coordinates.
    y       r   CSW_F*    Array of point y coordinates.
    z       w   CSW_F*    Returned array of elevations.
    npts    r   int           Number of points in x, y and z.
    flag    r   int           Set to GRD_BILINEAR or to
                              GRD_BICUBIC to specify the type
                              of interpolation to use.

*/

int CSWGrdAPI::grd_BackInterpolate (CSW_F *grid, int ncol, int nrow,
                         CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                         FAultLineStruct *faults, int nfaults,
                         CSW_F *x, CSW_F *y, CSW_F *z, int npts,
                         int flag)
{
    int             istat;

/*
    Make sure the grid limits can be supported by
    CSW_F precision arithmetic.
*/
    istat = csw_CheckRange2 (x1, y1, x2, y2);
    if (istat == 0) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

/*
    check the range of the input x and y values to see
    if it can support CSW_F precision arithmetic.
*/
    istat = csw_CheckRange (x, npts);
    if (!istat) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = csw_CheckRange (y, npts);
    if (!istat) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = grd_arith_obj.grd_back_interpolate (grid, ncol, nrow,
                                  x1, y1, x2, y2,
                                  faults, nfaults,
                                  x, y, z, npts, flag);
    return istat;

}  /*  end of function grd_BackInterpolate  */






/*
  ****************************************************************

             g r d _ H o r i z o n t a l G r a d i e n t

  ****************************************************************

  function name:    grd_HorizontalGradient            (int)

  call sequence:    grd_HorizontalGradient (grid, ncol, nrow,
                                            x1, y1, x2, y2,
                                            faults, nfaults,
                                            slope_grid, direction_grid)

  purpose:          Calculate the slope and direction at each node
                    in a grid.  The grid should not have null values.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           2 = NULL grid, slope_grid or direction_grid
                    3 = ncol or nrow less than 2
                    4 = x1 >= x2 or y1 >= y2
                    5 = the grid has hard null values
                    99= The grid limits are too close together for
                        the magnitude.  Use the double version.

  calling parameters:

    grid            r   CSW_F*    Input grid data.
    ncol            r   int           Number of columns in grid.
    nrow            r   int           Number of rows in grid.
    x1              r   CSW_F     Minimum x of grid.
    y1              r   CSW_F     Minimum y of grid.
    x2              r   CSW_F     Maximum x of grid.
    y2              r   CSW_F     Maximum y of grid.
    faults          r   FAultLineStruct*  Array of fault lines.
    nfaults         r   int           Number of faults.
    slope_grid      w   CSW_F*    Output magnitude of the slope.
    direction_grid  w   CSW_F*    Uphill direction in degrees.

*/

int CSWGrdAPI::grd_HorizontalGradient (CSW_F *grid, int ncol, int nrow,
                            CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                            FAultLineStruct *faults, int nfaults,
                            CSW_F *slope_grid, CSW_F *direction_grid)
{
    int              istat;

    istat = csw_CheckRange2 (x1, y1, x2, y2);
    if (istat == 0) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = grd_arith_obj.grd_horizontal_gradient (grid, ncol, nrow,
                                     x1, y1, x2, y2,
                                     faults, nfaults,
                                     slope_grid, direction_grid);
    return istat;

}  /*  end of function grd_HorizontalGradient  */








/*
  ****************************************************************

                      g r d _ V e r s i o n

  ****************************************************************

    Return the version string in a buffer that must be at least 6 chars long.

*/

int CSWGrdAPI::grd_Version (char *buff)
{

    strcpy (buff, VERSION);
    return 1;

}  /*  end of function grd_Version  */





/*
  ****************************************************************

            g r d _ C a l c G r i d F r o m D o u b l e

  ****************************************************************

  function name:    grd_CalcGridFromDouble              (int)

  call sequence:    grd_CalcGridFromDouble (x, y, z, error, npts,
                                        grid, mask, report, ncol, nrow,
                                        x1, y1, x2, y2,
                                        options)

  purpose:          Calculate a uniform grid by interpolating the
                    specified data points.  The errors between input
                    points and the grid's estimate can be returned
                    in the error array.  A mask specifying which
                    grid points are outside the data area or
                    inside but far from control points can also
                    be returned in the mask array.  This function only
                    supports non faulted continuous surfaces.

                    The grid must have at least 2 columns and 2 rows.
                    The upper limit of grid size is controlled by available
                    virtual memory.  The algorithm needs workspace about 5
                    times the size of the grid, plus workspace about twice
                    the size of the data array.  For example, calculating
                    a 1000 by 1000 grid from 100000 points would need about
                    21 to 22 megabytes of work space.  Memory allocation errors
                    are generally caused by grids too large for available
                    memory.

                    This is nearly identical to grd_CalcGrid, except that the
                    x, y, z and grid corners are specified as doubles rather
                    than CSW_Fs.  This function just shifts the x,y points
                    to be relative to their minimums and then converts them to
                    CSW_F arrays for regular gridding.  As such, the final
                    grid is still single precision, but large x and y coordinates
                    can be handled.

  return value:     status code
                    -1 = error
                     1 = success

  errors:           1 = memory allocation error
                    2 = A NULL x, y, z, or grid array is specified.
                    3 = A wild parameter was encountered.
                    4 = ncol or nrow is less than 2
                    5 = x2 less than x1 or y2 less than y1
                    6 = npts less than 1
                    7 = number of points actually in the data area
                        is less than 1
                    8 = The points are distributed in a manner that
                        cannot be fit with a trend surface (probably
                        all points are coincident or colinear).
                    9 = The faults and nfaults parameters are inconsistent.
                        Either faults is NULL and nfaults is > 0 or faults
                        is not NULL and nfaults is <= zero.


  calling parameters:

    x        r    double*       Array of x coordinates
    y        r    double*       Array of y coordinates
    z        r    double*       Array of z values at the x,y coordinates
    error    w    CSW_F*    Optional array to receive errors at the
                                input points.  Set to NULL if not wanted.
    npts     r    int           Number of points in x, y, z and error.
    grid     w    CSW_F*    Array to receive the grid values.
    mask     w    char*         Optional array to receive a mask specifying
                                whether a grid node is outside the data
                                area, inside and well defined, or inside
                                and poorly defined.  A well defined node
                                has a mask value of zero.  A node outside
                                the data area has a mask value of 1.  A
                                poorly defined inside node has a mask
                                value of 2.  Specify a NULL pointer to
                                disable this feature.
    report   w    char**        If this is not null a report of the grid
                                vital statistics is returned in the pointer.
    ncol     r    int           Number of columns in the grid and mask.
    nrow     r    int           Number of rows in the grid and mask.
    x1       r    double        Minimum x coordinate of the grid.
    y1       r    double        Minimum y coordinate of the grid.
    x2       r    double        Maximum x coordinate of the grid.
    y2       r    double        Maximum y coordinate of the grid.
    options  r    GRidCalcOptions*
                                Optional option record pointer.  If NULL,
                                the global option state is used.  If not
                                NULL, options are taken from this structure
                                for this calculation only.


*/

int CSWGrdAPI::grd_CalcGridFromDouble (double *x, double *y, double *z, CSW_F *error, int npts,
                            CSW_F *grid, char *mask, char **report,
                            int ncol, int nrow,
                            double x1, double y1, double x2, double y2,
                            FAultLineStruct *faults, int nfaults,
                            GRidCalcOptions *options)
{
    int  istat = 0;

    istat = grd_calc_obj.grd_calc_grid_from_double
        (x, y, z, error, npts,
         grid, mask, report,
         ncol, nrow,
         x1, y1, x2, y2,
         faults, nfaults, options);
    return istat;

}  /*  end of function grd_CalcGridFromDouble  */






/*
  ****************************************************************

     g r d _ R e c o m m e n d e d S i z e F r o m D o u b l e

  ****************************************************************

  function name: grd_RecommendedSizeFromDouble              (int)

  call sequence: grd_RecommendedSizeFromDouble (x, y, npts,
                                      x1, y1, x2, y2,
                                      ncol, nrow)

  purpose:       Return the recommended number of columns and rows
                 for a specified set of points.  If the bounds data
                 are specified, only points within the bounds are
                 used to calculate the recommended size.  If the
                 bounds are specified as any of the following, the
                 limits of the specified data are used for the bounds.

                   If x1, y1, x2 or y2 is greater than 1.e20 or less
                   than -1.e20.

                   If x1 greater than or equal to x2 or if y1 greater
                   than or equal to y2.

  return value:    status code

                   -1 = error
                    1 = success

  errors:          1 = Error allocating memory.
                   2 = You specified less than 1 points.
                   3 = Either x or y is NULL.
                   4 = Either ncol or nrow is null.
                   5 = One of the boundary pointers is NULL.
                   6 = Less than 1 points in specified boundary.
                   7 = All points are in the same location.

  calling parameters:

    x        r    double*        Array of x coordinates.
    y        r    double*        Array of y coordinates.
    npts     r    int            Number of points in x,y.
    x1       r/w  double*        Minimum x value
    y1       r/w  double*        Minimum y value
    x2       r/w  double*        Maximum x value
    y2       r/w  double*        Maximum y value
    ncol     w    int*           Recommended number of columns.
    nrow     w    int*           Recommended number of rows.

*/

int CSWGrdAPI::grd_RecommendedSizeFromDouble
                        (double *x, double *y, int npts,
                         double *x1, double *y1, double *x2, double *y2,
                         int *ncol, int *nrow)
{
    int           istat, i;
    CSW_F         *xt = NULL, *yt = NULL, xt1, yt1, xt2, yt2;
    double        xmin, ymin;


    auto fscope = [&]()
    {
        csw_Free (xt);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    check obvious errors
*/
    if (npts < 1) {
        grd_utils_obj.grd_set_err (2);
        return -1;
    }

/*
    allocate CSW_F work space.
*/
MSL
    xt = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
    if (!xt) {
        grd_utils_obj.grd_set_err (1);
        return -1;
    }
    yt = xt + npts;

/*
    subtract minimum x and y values
*/
    xmin = 1.e30;
    ymin = 1.e30;

    for (i=0; i<npts; i++) {
        if (x[i] < xmin) xmin = x[i];
        if (y[i] < ymin) ymin = y[i];
    }

    for (i=0; i<npts; i++) {
        x[i] -= xmin;
        y[i] -= ymin;
    }

/*
    convert to CSW_F values
*/
    for (i=0; i<npts; i++) {
        xt[i] = (CSW_F)x[i];
        yt[i] = (CSW_F)y[i];
    }
    xt1 = (CSW_F)(*x1 - xmin);
    yt1 = (CSW_F)(*y1 - ymin);
    xt2 = (CSW_F)(*x2 - xmin);
    yt2 = (CSW_F)(*y2 - ymin);

    if (*x1 < -1.e20  ||  *x1 > 1.e20  ||
        *y1 < -1.e20  ||  *y1 > 1.e20  ||
        *x2 < -1.e20  ||  *x2 > 1.e20  ||
        *y2 < -1.e20  ||  *y2 > 1.e20) {
        xt1 = 1.e30f;
        yt1 = 1.e30f;
        xt2 = -1.e30f;
        yt2 = -1.e30f;
    }

    istat = grd_utils_obj.grd_recommended_size (xt, yt, npts,
                                  &xt1, &yt1, &xt2, &yt2,
                                  ncol, nrow);

    *x1 = xt1 + xmin;
    *y1 = yt1 + ymin;
    *x2 = xt2 + xmin;
    *y2 = yt2 + ymin;

/*
    add back the minimum x and y to the input double data
*/
    for (i=0; i<npts; i++) {
        x[i] += xmin;
        y[i] += ymin;
    }

    return istat;

}  /*  end of function grd_RecommendedSizeFromDouble  */






/*
  ****************************************************************

               g r d _ F i l l N u l l V a l u e s

  ****************************************************************

  function name:    grd_FillNullValues             (int)

  call sequence:    grd_FillNullValues (grid, ncol, nrow,
                                        x1, y1, x2, y2,
                                        faults, nfaults,
                                        nullval, gridout, maskout)

  purpose:          Expand the non null values in a grid out to the
                    edge of the grid to replace the nulls.  The expansion
                    is done by extrapolating gradients in nearest non null
                    nodes where that is possible.  If not possible, or if
                    the null node is far from non null nodes, a simple
                    average of non null nodes is also used.  For most
                    filled nodes, the values will be a combination of the
                    gradient extrapolation and the simple average.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           1 = memory allocation error
                    2 = original grid is all null values
                    3 = grid pointer is null
                    4 = ncol or nrow less than 2

  calling parameters:

    grid      r/w  CSW_F*       Grid array with null values in it.
    ncol      r    int          Number of columns in grid.
    nrow      r    int          Number of rows in grid.
    x1        r    CSW_F        Minimum x of the grid.
    y1        r    CSW_F        Minimum y of the grid.
    x2        r    CSW_F        Maximum x of the grid.
    y2        r    CSW_F        Maximum y of the grid.
    faults    r    FAultLineStruct*   Array of fault lines used to calculate
                                the original grid.
    nfaults   r    int          Number of fault lines.
    nullval   r    CSW_F        Null value for the grid.
    gridout   w    CSW_F*       Optional output grid, or NULL to
                                overwrite the input grid.
    maskout   w    char*        Optional mask for output grid.  Each
                                hard null value in the input grid will
                                have its mask node set to 1, meaning
                                uncontrolled outside the data area.
                                If this is NULL, it is ignored.

*/

int CSWGrdAPI::grd_FillNullValues (CSW_F *grid, int ncol, int nrow,
                        CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                        FAultLineStruct *faults, int nfaults,
                        CSW_F nullval, CSW_F *gridout, char *maskout)
{
    int          istat;

    grd_fault_obj.grd_free_faults ();
    if (faults  &&  nfaults > 0) {
        istat = grd_fault_obj.grd_define_fault_vectors (faults, nfaults);
        if (istat == -1) {
            grd_utils_obj.grd_set_err (1);
            return -1;
        }
        istat = grd_fault_obj.grd_build_fault_indices (grid, ncol, nrow,
                                         x1, y1, x2, y2);
        if (istat == -1) {
            grd_utils_obj.grd_set_err (1);
            return -1;
        }
        istat = grd_fault_obj.grd_fill_faulted_nulls (nullval);
        if (istat == -1) {
            grd_utils_obj.grd_set_err (1);
            return -1;
        }
    }
    else {
        istat = grd_utils_obj.grd_fill_nulls_new (grid, ncol, nrow, nullval, gridout, maskout, 0);
    }

    return istat;

}  /*  end of function grd_FillNullValues  */






/*
  ****************************************************************

                       C o n s t a n t G r i d

  ****************************************************************

    Fill in the grid with a constant and make all mask values zero.

*/

int CSWGrdAPI::ConstantGrid (CSW_F *grid, char *mask, int ncol, int nrow,
                         CSW_F val)
{
    int          i;

    for (i=0; i<ncol*nrow; i++) {
        grid[i] = val;
    }

    if (mask) {
        memset (mask, 0, ncol*nrow*sizeof(char));
    }

    return 1;

}  /*  end of private ConstantGrid function  */






/*
  ****************************************************************

                  g r d _ X Y Z F r o m G r i d

  ****************************************************************

  function name:    grd_XYZFromGrid                (int)

  call sequence:    XYZFromGrid (grid, ncol, nrow, nullval,
                                 x1, y1, x2, y2,
                                 x, y, z, npts, maxpts)

  purpose:          Convert the non null grid nodes into x, y and z
                    points.  The grid will be decimated if needed to
                    keep the number of output points below maxpts.
                    Do not use this if the grid corner point values
                    are very large (e.g. > 1 million or less than
                    -1 million) and the grid width or height is
                    relatively small (e.g < 100).  Use the function
                    grd_XYZFromGridDouble for these types of data.

                    note:
                       The original grid values will be returned as
                    they are input, but the internal implementation
                    will need to temporarily reset null values if they
                    are negative.  Because of this, you cannot use
                    a const pointer for the grid.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           1 = memory allocation error
                    2 = grid pointer is NULL
                    3 = Either ncol or nrow is less than 2.
                    4 = Either x, y, z or npts pointer is NULL.
                    5 = maxpts is less than 1.
                    6 = Either x >= x2 or y1 >= y2
                    7 = Wild value of x1, y1, x2 or y2

  calling parameters:

    grid      r/w  CSW_F*       Grid array.
    ncol      r    int          Number of columns in grid.
    nrow      r    int          Number of rows in grid.
    nullval   r    CSW_F        Null value for the grid.
    x1        r    CSW_F        minimum x of grid.
    y1        r    CSW_F        minimum y of grid.
    x2        r    CSW_F        maximum x of grid.
    y2        r    CSW_F        maximum y of grid.
    x         w    CSW_F*       array of x data point coordinates
    y         w    CSW_F*       array of y data point coordinates
    z         w    CSW_F*       array of z data point values
    npts      w    int*         actual number of points returned
    maxpts    r    int          maximum size of x, y and z

*/

int CSWGrdAPI::grd_XYZFromGrid (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                     CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                     CSW_F *x, CSW_F *y, CSW_F *z, int *npts, int maxpts)
{
    int          istat;

    istat = grd_utils_obj.grd_xyz_from_grid (grid, ncol, nrow, nullval,
                               x1, y1, x2, y2,
                               x, y, z, npts, maxpts);
    return istat;

}  /*  end of function grd_XYZFromGrid  */






/*
  ****************************************************************

         g r d _ X Y Z F r o m G r i d D o u b l e

  ****************************************************************

  function name:    grd_XYZFromGridDouble          (int)

  call sequence:    XYZFromGridDouble (grid, ncol, nrow, nullval,
                                       x1, y1, x2, y2,
                                       x, y, z, npts, maxpts)

  purpose:          Convert the non null grid nodes into x, y and z
                    points.  The grid will be decimated if needed to
                    keep the number of output points below maxpts.
                    You should use this if the grid corner point values
                    are very large (e.g. > 1 million or less than
                    -1 million) and the grid width or height is
                    relatively small (e.g < 100).  You can use
                    grd_XYZFromGrid for data that do not meet these
                    criteria.

                    The grid is still an array of CSW_F values.  Only
                    the grid geometry is double when using this
                    function.

                    note:
                       The original grid values will be returned as
                    they are input, but the internal implementation
                    will need to temporarily reset null values if they
                    are negative.  Because of this, you cannot use
                    a const pointer for the grid.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           1 = memory allocation error
                    2 = grid pointer is NULL
                    3 = Either ncol or nrow is less than 2.
                    4 = Either x, y, z or npts pointer is NULL.
                    5 = maxpts is less than 1.

  calling parameters:

    grid      r/w  CSW_F*       Grid array.
    ncol      r    int          Number of columns in grid.
    nrow      r    int          Number of rows in grid.
    nullval   r    CSW_F        Null value for the grid.
    x1        r    double       minimum x of grid.
    y1        r    double       minimum y of grid.
    x2        r    double       maximum x of grid.
    y2        r    double       maximum y of grid.
    x         w    double*      array of x data point coordinates
    y         w    double*      array of y data point coordinates
    z         w    double*      array of z data point values
    npts      w    int*         actual number of points returned
    maxpts    r    int          maximum size of x, y and z

*/

int CSWGrdAPI::grd_XYZFromGridDouble (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                           double x1, double y1, double x2, double y2,
                           double *x, double *y, double *z, int *npts, int maxpts)
{
    int          istat, i;
    CSW_F        *xx = NULL, *yy = NULL, *zz = NULL,
                 xx1, yy1, xx2, yy2;


    auto fscope = [&]()
    {
        csw_Free (xx);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    allocate memory for maxpts CSW_F numbers to temporarily
    use as x, y and z.
*/
MSL
    xx = (CSW_F *)csw_Malloc (maxpts * 3 * sizeof(CSW_F));
    if (!xx) {
        *npts = 0;
        grd_utils_obj.grd_set_err (1);
        return -1;
    }
    yy = xx + maxpts;
    zz = yy + maxpts;

/*
    translate the double corner points so the
    lower left is at 0, 0
*/
    xx1 = 0.0f;
    yy1 = 0.0f;
    xx2 = (CSW_F)(x2 - x1);
    yy2 = (CSW_F)(y2 - y1);

/*
    Do the actual work using CSW_F geometry, but the range versus
    absolute value problem is fixed by the translation above.
*/
    istat = grd_utils_obj.grd_xyz_from_grid (grid, ncol, nrow, nullval,
                               xx1, yy1, xx2, yy2,
                               xx, yy, zz, npts, maxpts);

/*
    Translate the results back to double values.
*/
    for (i=0; i<*npts; i++) {
        x[i] = xx[i] + x1;
        y[i] = yy[i] + y1;
        z[i] = zz[i];
    }

    return istat;

}  /*  end of function grd_XYZFromGrid  */





/*
  ****************************************************************

        g r d _ R e s a m p l e G r i d F r o m D o u b l e

  ****************************************************************

  function name:  grd_ResampleGridFromDouble        (int)

  call sequence:  grd_ResampleGridFromDouble (grid, mask, ncol, nrow,
                                    dx1, dy1, dx2, dy2,
                                    faults, nfaults,
                                    newgrid, newmask, newncol, newnrow,
                                    dnewx1, dnewy1, dnewx2, dnewy2, flag)

  purpose:        Resample a subset of an existing grid using a different
                  grid geometry.

  return value:   status code

                  1 = success
                 -1 = error

  errors:        2 = either grid or newgrid is NULL
                 3 = if mask is not null, newmask must also be non null
                 4 = either the original or new limits are inconsistent
                 5 = the new grid is not entirely inside the original
                 6 = less than 2 columns or rows specified for either the
                     original or new grid.

  calling parameters:

    grid      r   CSW_F*      Original grid data.
    mask      r   char*           Optional mask of original grid.
    ncol      r   int             Number of columns in original grid.
    nrow      r   int             Number of rows in original grid.
    dx1       r   double          Minimum x of original grid.
    dy1       r   double          Minimum y of original grid.
    dx2       r   double          Maximum x or original grid.
    dy2       r   double          Maximum y of original grid.
    faults    r   FAultLineStruct*  Array of fault line structures.
    nfaults   r   int             Number of fault line structures.
    newgrid   w   CSW_F*      New grid data.
    newmask   w   char*           Optional mask of new grid.
    newncol   r   int             Number of columns in new grid.
    newnrow   r   int             Number of rows in new grid.
    dnewx1    r   double          Minimum x of new grid.
    dnewy1    r   double          Minimum y of new grid.
    dnewx2    r   double          Maximum x or new grid.
    dnewy2    r   double          Maximum y of new grid.
    flag      r   int             Either GRD_BILINEAR, GRD_BICUBIC or
                                  GRD_STEP_GRID to specify the type of
                                  interpolation to use when resampling.

*/

int CSWGrdAPI::grd_ResampleGridFromDouble
                     (CSW_F *grid, char *mask, int ncol, int nrow,
                      double dx1, double dy1, double dx2, double dy2,
                      FAultLineStruct *faults, int nfaults,
                      CSW_F *newgrid, char *newmask, int newncol, int newnrow,
                      double dnewx1, double dnewy1, double dnewx2, double dnewy2,
                      int flag)
{
    int         istat;
    CSW_F       x1, y1, x2, y2, newx1, newy1, newx2, newy2;
    FAultLineStruct  *fout;
    int              nfout;

    dx2 -= dx1;
    dy2 -= dy1;
    dnewx1 -= dx1;
    dnewy1 -= dy1;
    dnewx2 -= dx1;
    dnewy2 -= dy1;

    x1 = 0.0f;
    y1 = 0.0f;
    x2 = (CSW_F)dx2;
    y2 = (CSW_F)dy2;

    newx1 = (CSW_F)dnewx1;
    newy1 = (CSW_F)dnewy1;
    newx2 = (CSW_F)dnewx2;
    newy2 = (CSW_F)dnewy2;

    fout = faults;
    nfout = nfaults;
    if (faults  &&  nfaults > 0) {
        istat =
          grd_fault_obj.grd_shift_fault_vectors (faults, nfaults,
                                   dx1, dy1,
                                   &fout, &nfout);
    }

    istat = grd_arith_obj.grd_resample_grid (grid, mask, ncol, nrow,
                               x1, y1, x2, y2,
                               fout, nfout,
                               newgrid, newmask, newncol, newnrow,
                               newx1, newy1, newx2, newy2,
                               flag);

    if (fout != faults) {
        grd_FreeFaultLineStructs (fout, nfout);
    }

    return istat;

}  /*  end of function grd_ResampleGridFromDouble  */





/*
  ****************************************************************

     g r d _ B a c k I n t e r p o l a t e F r o m D o u b l e

  ****************************************************************

  function name:  grd_BackInterpolateFromDouble         (int)

  call sequence:  grd_BackInterpolateFromDouble (grid, ncol, nrow,
                                       dx1, dy1, dx2, dy2,
                                       faults, nfaults,
                                       dx, dy, dz, npts,
                                       flag)

  purpose:        Return the zvalues calculated from interpolating
                  the grid at the specified x,y locations.  If a
                  location is outside the grid or if the grid has
                  null values nearest the point, the z value for the
                  point is set to 1.e30.

  return value:   status code

                  1 = success
                 -1 = error

  errors:         2 = A null grid, x, y, or z pointer was passed.
                  3 = Either ncol or nrow is less than 2.
                  4 = The grid limits are inconsistent (x1 >= x2 or y1 >= y2).
                  5 = The npts parameter is less than 1.

  calling parameters:

    grid    r   CSW_F*    Array of grid data.
    ncol    r   int           Number of grid columns.
    nrow    r   int           Number of grid rows.
    dx1     r   double        Minimum x coordinate of the grid.
    dy1     r   double        Minimum y of the grid.
    dx2     r   double        Maximum x of the grid.
    dy2     r   double        Maximum y of the grid.
    faults  r   FAultLineStruct*   Array of fault lines used to calculate
                              the original grid.
    nfaults r   int           Number of fault lines.
    dx      r   double   *    Array of point x coordinates.
    dy      r   double   *    Array of point y coordinates.
    dz      w   double   *    Returned array of elevations.
    npts    r   int           Number of points in x, y and z.
    flag    r   int           Set to GRD_BILINEAR or to
                              GRD_BICUBIC to specify the type
                              of interpolation to use.

*/

int CSWGrdAPI::grd_BackInterpolateFromDouble (CSW_F *grid, int ncol, int nrow,
                         double dx1, double dy1, double dx2, double dy2,
                         FAultLineStruct *faults, int nfaults,
                         double *dx, double *dy, double *dz, int npts,
                         int flag)
{
    int             istat, i, nfout;
    FAultLineStruct *fout;
    CSW_F           *xt = NULL, *yt = NULL, *zt = NULL,
                    xt1, yt1, xt2, yt2;
    double          xmin, ymin;



    auto fscope = [&]()
    {
        csw_Free (xt);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    errors that will screw up this function
*/
    if (npts < 1) {
        grd_utils_obj.grd_set_err (5);
        return -1;
    }

/*
    allocate CSW_F arrays
*/
MSL
    xt = (CSW_F *)csw_Malloc (npts * 3 * sizeof(CSW_F));
    if (!xt) {
        grd_utils_obj.grd_set_err (1);
        return -1;
    }
    yt = xt + npts;
    zt = yt + npts;

/*
    make all coordinates relative to the grid x1, y1
*/
    xmin = dx1;
    ymin = dy1;

    dx1 = 0.0;
    dy1 = 0.0;
    dx2 -= xmin;
    dy2 -= ymin;

    for (i=0; i<npts; i++) {
        dx[i] -= xmin;
        dy[i] -= ymin;
    }

/*
    copy to the CSW_F arrays
*/
    for (i=0; i<npts; i++) {
        xt[i] = (CSW_F)dx[i];
        yt[i] = (CSW_F)dy[i];
        zt[i] = 1.e30f;
    }

    xt1 = (CSW_F)dx1;
    yt1 = (CSW_F)dy1;
    xt2 = (CSW_F)dx2;
    yt2 = (CSW_F)dy2;

    if (faults &&  nfaults > 0) {
        grd_fault_obj.grd_shift_fault_vectors (
            faults, nfaults,
            xmin, ymin,
            &fout, &nfout);
    }
    else {
        fout = NULL;
        nfout = 0;
    }

/*
    Interpolate to the CSW_F coordinates
*/
    istat = grd_arith_obj.grd_back_interpolate (grid, ncol, nrow,
                                  xt1, yt1, xt2, yt2,
                                  fout, nfout,
                                  xt, yt, zt, npts, flag);

    grd_FreeFaultLineStructs (fout, nfout);

/*
    add back the minimum x and y to the input double data
*/
    for (i=0; i<npts; i++) {
        dx[i] += xmin;
        dy[i] += ymin;
        dz[i] = zt[i];
    }

    return istat;

}  /*  end of function grd_BackInterpolateFromDouble  */





/*
  ****************************************************************

  g r d _ H o r i z o n t a l G r a d i e n t F r o m D o u b l e

  ****************************************************************

  function name:    grd_HorizontalGradientFromDouble      (int)

  call sequence:    grd_HorizontalGradientFromDouble (grid, ncol, nrow,
                                            dx1, dy1, dx2, dy2,
                                            faults, nfaults,
                                            slope_grid, direction_grid)

  purpose:          Calculate the slope and direction at each node
                    in a grid.  The grid should not have null values.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           2 = NULL grid, slope_grid or direction_grid
                    3 = ncol or nrow less than 2
                    4 = x1 >= x2 or y1 >= y2
                    5 = the grid has hard null values

  calling parameters:

    grid            r   CSW_F*    Input grid data.
    ncol            r   int           Number of columns in grid.
    nrow            r   int           Number of rows in grid.
    dx1             r   CSW_F     Minimum x of grid.
    dy1             r   CSW_F     Minimum y of grid.
    dx2             r   CSW_F     Maximum x of grid.
    dy2             r   CSW_F     Maximum y of grid.
    faults          r   FAultLineStruct*  Array of fault lines.
    nfaults         r   int           Number of faults.
    slope_grid      w   CSW_F*    Output magnitude of the slope.
    direction_grid  w   CSW_F*    Uphill direction in degrees.

*/

int CSWGrdAPI::grd_HorizontalGradientFromDouble (CSW_F *grid, int ncol, int nrow,
                            double dx1, double dy1, double dx2, double dy2,
                            FAultLineStruct *faults, int nfaults,
                            CSW_F *slope_grid, CSW_F *direction_grid)
{
    int              istat;
    CSW_F            x1, y1, x2, y2;

    dx2 -= dx1;
    dy2 -= dy1;

    x1 = 0.0f;
    y1 = 0.0f;
    x2 = (CSW_F)dx2;
    y2 = (CSW_F)dy2;

    istat = grd_arith_obj.grd_horizontal_gradient (grid, ncol, nrow,
                                     x1, y1, x2, y2,
                                     faults, nfaults,
                                     slope_grid, direction_grid);
    return istat;

}  /*  end of function grd_HorizontalGradientFromDouble  */






/*
  ****************************************************************

      g r d _ C a l c T r e n d G r i d F r o m D o u b l e

  ****************************************************************

  function name:  grd_CalcTrendGridFromDouble         (int)

  call sequence:  grd_CalcTrendGridFromDouble
                                    (dx, dy, dz, npts, iorder,
                                     grid, ncol, nrow,
                                     dx1, dy1, dx2, dy2)

  purpose:        Calculate a grid for a trend surface.  The
                  surface coefficients are calculated and then
                  evaluated at the nodes of the specified grid.

  return value:   1 = success
                 -1 = error

  errors:        1 = memory allocation error
                 2 = iorder is outside of 1-8
                     range or npts is too low
                     for the order specified.
                 3 = The data cannot be fit, probably
                     because they are colinear or all
                     at the same location
                 4 = ncol, nrow is less than 2 or
                     xmin >= xmax or ymin >= ymax
                 5 = x, y, z or grid is NULL

  calling parameters:

    dx        CSW_F*  r    array of x coordinates
    dy        CSW_F*  r    array of y coordinates
    dz        CSW_F*  r    array of z coordinates
    npts      int         r    number of points
    iorder    int         r    order of polynomial
    grid      CSW_F*  w    output grid
    ncol      int         r    number of columns in grid
    nrow      int         r    number of rows in grid
    dx1       CSW_F   r    minimum x coordinate of grid
    dy1       CSW_F   r    minimum y coordinate of grid
    dx2       CSW_F   r    maximum x or grid
    dy2       CSW_F   r    maximum y of grid

        Note:  grid must be large enough for ncol * nrow CSW_F numbers.

*/

int CSWGrdAPI::grd_CalcTrendGridFromDouble
                      (double *dx, double *dy, double *dz, int npts, int iorder,
                       CSW_F *grid, int ncol, int nrow,
                       double dx1, double dy1, double dx2, double dy2)
{
    int              istat, i;
    CSW_F            *xt = NULL, *yt = NULL, *zt = NULL,
                     xt1, yt1, xt2, yt2;
    double           xmin, ymin;



    auto fscope = [&]()
    {
        csw_Free (xt);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    errors that will screw up this function
*/
    if (npts < 3) {
        grd_utils_obj.grd_set_err (2);
        return -1;
    }

    if (grid == NULL  ||  dx == NULL  ||
        dy == NULL  ||  dz == NULL) {
        grd_utils_obj.grd_set_err (5);
        return -1;
    }

    istat = csw_CheckDoubleRange (dz, npts);
    if (!istat) {
        ConstantGrid (grid, NULL, ncol, nrow, (CSW_F)dz[0]);
        return 1;
    }

/*
    allocate CSW_F arrays
*/
MSL
    xt = (CSW_F *)csw_Malloc (npts * 3 * sizeof(CSW_F));
    if (!xt) {
        grd_utils_obj.grd_set_err (1);
        return -1;
    }
    yt = xt + npts;
    zt = yt + npts;

/*
    subtract minimum x and y values from double arrays
*/
    xmin = 1.e30;
    ymin = 1.e30;
    for (i=0; i<npts; i++) {
        if (dx[i] < xmin) xmin = dx[i];
        if (dy[i] < ymin) ymin = dy[i];
    }

    for (i=0; i<npts; i++) {
        dx[i] -= xmin;
        dy[i] -= ymin;
    }
    dx1 -= xmin;
    dy1 -= ymin;
    dx2 -= xmin;
    dy2 -= ymin;

/*
    copy to the CSW_F arrays
*/
    for (i=0; i<npts; i++) {
        xt[i] = (CSW_F)dx[i];
        yt[i] = (CSW_F)dy[i];
        zt[i] = (CSW_F)dz[i];
    }

    xt1 = (CSW_F)dx1;
    yt1 = (CSW_F)dy1;
    xt2 = (CSW_F)dx2;
    yt2 = (CSW_F)dy2;

/*
    calculate a CSW_F grid
*/
    istat = grd_tsurf_obj.grd_calc_trend_grid (xt, yt, zt, npts, iorder,
                                 grid, ncol, nrow,
                                 xt1, yt1, xt2, yt2);

/*
    add back the minimum x and y to the input double data
*/
    for (i=0; i<npts; i++) {
        dx[i] += xmin;
        dy[i] += ymin;
    }

    return istat;

}  /*  end of function grd_CalcTrendGridFromDouble  */





/*
  ****************************************************************

       g r d _ T w o G r i d A r i t h F r o m D o u b l e

  ****************************************************************

  function name:    grd_TwoGridArithFromDouble        (int)

  call sequence:    grd_TwoGridArithFromDouble
                                     (grid1, mask1,
                                      x11, y11, x12, y12, ncol1, nrow1,
                                      faults1, nfaults1,
                                      grid2, mask2,
                                      x21, y21, x22, y22, ncol2, nrow2,
                                      faults2, nfaults2,
                                      nullvalue, gridout,
                                      x1out, y1out, x2out, y2out,
                                      ncout, nrout,
                                      tag, funcptr, client_data)

  purpose:          Calculate a new grid by doing arithmetic
                    operations between two input grids.

  return value:     status code

                    -1 = error
                     1 = success

  errors:           1 = error allocating memory
                    2 = no intersection between the two grids
                    4 = unrecognized tag
                    5 = Either ncol or nrow is less than 2.
                    6 = A null pointer for an output parameter.
                    7 = One mask is null, the other is not.
                    8 = tag is GRD_CALL_FUNC but the funcptr is NULL
                    98= calculation aborted by operator function.

  calling parameters:

    grid1     r     CSW_F*    First input grid data array.
    mask1     r     char*         Mask array for bad nodes in first grid.
    x11       r     double        minimum x of first grid
    y11       r     double        minimum y of first grid
    x12       r     double        maximum x of first grid
    y12       r     double        maximum y of first grid
    ncol1     r     int           number of columns in first grid
    nrow1     r     int           number of rows in first grid
    faults1   r     FAultLineStruct*  Array of fault lines for the first grid.
    nfaults1  r     int           number of fault lines for the first grid.
    grid2     r     CSW_F*    Second input grid data array.
    mask2     r     char*         Mask array for bad nodes in second grid.
    x21       r     double        minimum x of second grid
    y21       r     double        minimum y of second grid
    x22       r     double        maximum x of second grid
    y22       r     double        maximum y of second grid
    ncol2     r     int           number of columns in second grid
    nrow2     r     int           number of rows in second grid
    faults2   r     FAultLineStruct*  Array of fault lines for the second grid.
    nfaults2  r     int           number of fault lines for the second grid.
    nullvalue r     CSW_F     Value which "bad" grid nodes are set to.  If
                                  this value is found in either input grid, no
                                  operation is done, and the output is set to
                                  the null value.
    gridout   w     CSW_F**   Returned output grid.  This is allocated by the
                                  function, and the application must csw_Free it.
    maskout   w     char**        Bad node mask for output grid.
    x1out     w     double*       Minimum x of output grid.
    y1out     w     double*       Minimum y of output grid.
    x2out     w     double*       Maximum x of output grid.
    y2out     w     double*       Maximum y of output grid.
    ncout     w     int*          Number of columns in output grid.
    nrout     w     int*          Number of rows in output grid.
    tag       r     int           Operation to perform from the following list.
                                  GRD_CALL_FUNC, GRD_ADD, GRD_SUBTRACT,
                                  GRD_MULTIPLY, GRD_DIVIDE,
                                  GRD_EXPONENT, GRD_MINIMUM, GRD_MAXIMUM
    funcptr   r     void (*)()    Pointer to a function to call to do the
                                  operation.  The function has a single parameter,
                                  a pointer to a GRidArithData structure.  This is
                                  only used if it is not NULL and if the tag is set
                                  to GRD_CALL_FUNC.
    client_data  r  void*         Pointer to data that the application wants to
                                  pass to the called function.

*/
int CSWGrdAPI::grd_TwoGridArithFromDouble
                     (CSW_F *grid1, char *mask1,
                      double dx11, double dy11, double dx12, double dy12,
                      int ncol1, int nrow1,
                      FAultLineStruct *faults1, int nfaults1,
                      CSW_F *grid2, char *mask2,
                      double dx21, double dy21, double dx22, double dy22,
                      int ncol2, int nrow2,
                      FAultLineStruct *faults2, int nfaults2,
                      CSW_F nullvalue,
                      CSW_F **gridout, char **maskout,
                      double *dx1out, double *dy1out,
                      double *dx2out, double *dy2out, int *ncout, int *nrout,
                      int tag, void (*funcptr)(GRidArithData*), void *client_data)
{
    int      istat;
    CSW_F    x11, y11, x12, y12;
    CSW_F    x21, y21, x22, y22;
    CSW_F    x1out, y1out, x2out, y2out;

    dx12 -= dx11;
    dy12 -= dy11;
    dx21 -= dx11;
    dy21 -= dy11;
    dx22 -= dx11;
    dy22 -= dy11;

    x11 = 0.0f;
    y11 = 0.0f;
    x12 = (CSW_F)dx12;
    y12 = (CSW_F)dy12;
    x21 = (CSW_F)dx21;
    y21 = (CSW_F)dy21;
    x22 = (CSW_F)dx22;
    y22 = (CSW_F)dy22;

    istat = grd_arith_obj.grd_two_grid_arith (grid1, mask1, x11, y11, x12, y12, ncol1, nrow1,
                                faults1, nfaults1,
                                grid2, mask2, x21, y21, x22, y22, ncol2, nrow2,
                                faults2, nfaults2,
                                nullvalue,
                                gridout, maskout,
                                &x1out, &y1out, &x2out, &y2out, ncout, nrout,
                                tag, funcptr, client_data);

    *dx1out = (double)x1out + dx11;
    *dy1out = (double)y1out + dy11;
    *dx2out = (double)x2out + dx11;
    *dy2out = (double)y2out + dy11;

    return istat;

}  /*  end of function grd_TwoGridArithFromDouble  */




/*
  ****************************************************************

           g r d _ W r i t e M u l t i p l e F i l e

  ****************************************************************

  function name:  grd_WriteMultipleFile          (int)

  call sequence:  grd_WriteMultipleFile (filename, list, nlist)

  purpose:        Write several grids to a single file.

  return value:   status code

                  -1 = error
                   1 = success

  errors:         1 = Error writing to disk.
                  2 = Cannot create the specified file name.
                  3 = A grid structure has a null grid data pointer.
                  4 = A wild parameter value is specified in a grid structure.
                  5 = A hard null value is present in the grid data for
                      a structure.
                  6 = The tag has been used in a previous structure.
                  7 = A ncol or nrow value is less than 2.
                  8 = xmin >= xmax or ymin >= ymax
                  9 = Filename is invalid.
                  10= The list pointer is NULL.
                  11= The specified nlist is less than 1 or greater
                      than 10000.

              note:   The errors are generated (potentially) for each grid
                      specified.  The error member of the GRidFileRec
                      structure will have the error or zero if no error
                      occurred.  The error returned from grd_GetErr is the
                      most recent error encountered.

  calling parameters:

    filename      r    char*             File to write to, relative to the
                                         current directory.
    list          r    GRidFileRec*      List of filled in structures with
                                         grid data and geometry.
    nlist         r    int               Number of grids to write.

*/

int CSWGrdAPI::grd_WriteMultipleFile (char *filename, GRidFileRec *list, int nlist)
{
    int               istat;

    istat = grd_fileio_obj.grd_write_multiple_file (filename, list, nlist);
    return istat;

}  /*  end of function grd_WriteMultipleFile  */




/*
  ****************************************************************

             g r d _ R e a d M u l t i p l e F i l e

  ****************************************************************

  function name:  grd_ReadMultipleFile          (int)

  call sequence:  grd_ReadMultipleFile (filename, list, nlist)

  purpose:        Read one or more grids from a single file.

  return value:   status code

                  -1 = error
                   1 = success

  errors:         1 = Error reading from disk.
                  2 = Cannot open the specified file name.
                  5 = Error allocating grid or mask arrays.
                  6 = Filename is invalid.
                  7 = The list pointer is NULL.
                  8 = The specified nlist is less than 1.
                  10= The file is a single grid file.
                  11= The tag specified in a structure is not
                      found.

                note:  Errors 1-10 are returned by grd_GetErr and refer
                       to the entire reading process.  Error 11 is
                       specific to a particular structure and is put
                       into the structure's error member if it occurs.

  calling parameters:

    filename      r    char*             File to read from, relative to the
                                         current directory.
    list          r    GRidFileRec*      List of structures with tags filled
                                         in and which return the remaining
                                         members.
    nlist         r    int               Number of structures.

*/

int CSWGrdAPI::grd_ReadMultipleFile (char *filename, GRidFileRec *list, int nlist)
{
    int               istat;

    istat = grd_fileio_obj.grd_read_multiple_file (filename, list, nlist);
    return istat;

}  /*  end of function grd_ReadMultipleFile  */






/*
  ****************************************************************

       g r d _ S e t C o n t r o l P o i n t S t r u c t s

  ****************************************************************

  function name:        grd_SetControlPointStructs          (int)

  call sequence:        grd_SetControlPointStructs (list, nlist)

  purpose:              Set up control points to use for all subsequent gridding.
                        Control points state that the surface must be below a
                        value, above a value, or between two values at a
                        point.  To stop using the control points, call this
                        function with a NULL list or zero nlist.

  return value:         On success, returns 1.
                        On error, returns -1.

  errors:               1 = An error occurred allocating memory.

  calling parameters:

    list     r      GRidControlPoint*      Array of control point structures.
                                           typedef struct {
                                               CSW_F              x,
                                                                  y,
                                                                  z1,
                                                                  z2;
                                               int                type;
                                           }  GRidControlPoint;
    nlist    r      int                    Number of structures in the list

*/

int CSWGrdAPI::grd_SetControlPointStructs (GRidControlPoint *list, int nlist)
{
    int                     istat;

    istat = grd_calc_obj.grd_set_control_points (list, nlist);
    return istat;

}  /*  end of function grd_SetControlPointStructs  */





/*
  ****************************************************************

               g r d _ S e t C o n t r o l P o i n t s

  ****************************************************************

  function name:    grd_SetControlPoints              (int)

  call sequence:    grd_SetControlPoints (x, y, z1, z2, type, nlist)

  purpose:              Set up control points to use for all subsequent gridding.
                        Control points state that the surface must be below a
                        value, above a value, or between two values at a
                        point.  To stop using the control points, call this
                        function with a NULL list or zero nlist.

  return value:         On success, returns 1.
                        On error, returns -1.

  errors:               1 = An error occurred allocating memory.

  calling parameters:

    x    r    CSW_F*    Array of x locations of control points
    y    r    CSW_F*    Array of y locations of control points
    z1   r    CSW_F*    Array of single z values or minimum of two z
                        values for control points.
    z2   r    CSW_F*    Array of maximum z value if two are needed.
    type r    int*      Array of control point types.   1 = always
                        greater than z1, 2 = always less than z1,
                        3 = always between z1 and z2.
    nlist r   int       number of points.

*/

int CSWGrdAPI::grd_SetControlPoints (CSW_F *x, CSW_F *y, CSW_F *z1, CSW_F *z2,
                          int *type, int nlist)
{
    int                  i, istat;
    GRidControlPoint     *list = NULL;


    auto fscope = [&]()
    {
        csw_Free (list);
    };
    CSWScopeGuard func_scope_guard (fscope);

    if (nlist == 0) {
        grd_calc_obj.grd_set_control_points (NULL, 0);
        return 1;
    }

    list = (GRidControlPoint *)csw_Malloc (nlist * sizeof(GRidControlPoint));
    if (!list) {
        grd_utils_obj.grd_set_err (1);
        return 1;
    }

    for (i=0; i<nlist; i++) {
        list[i].x = x[i];
        list[i].y = y[i];
        list[i].z1 = z1[i];
        list[i].z2 = z2[i];
        list[i].type = type[i];
    }

    istat = grd_calc_obj.grd_set_control_points (list, nlist);

    return istat;

}  /*  end of function grd_SetControlPoints  */





/*
  ****************************************************************

             g r d _ S e t C o n t r o l S u r f a c e

  ****************************************************************

  function name:        grd_SetControlSurface         (int)

  call sequence:        grd_SetControlSurface (grid, ncol, nrow,
                                               x1, y1, x2, y2, type)

  purpose:              Set or unset a grid surface that will be used to
                        control the shape of subsequent calculated grids.
                        The control grids can be used to truncate the top
                        of the calculated grid, baselap the bottom, or
                        supply a conformable layer which the calculated grid
                        will folow where it does not have data control.

  return value:         1 on success
                       -1 on error

  errors:               1 = memory allocation failure
                        2 = unknown type
                        3 = inconsistent corner points

  calling parameters:

    grid     r    CSW_F*        Grid array.
    ncol     r    int           Number of columns in the grid.
    nrow     r    int           Number of rows in the grid.
    x1       r    CSW_F         Minimum x of grid.
    y1       r    CSW_F         Minimum y of grid.
    x2       r    CSW_F         Maximum x of grid.
    y2       r    CSW_F         Maximum y of grid.
    type     r    int           Type of grid.  GRD_CONFORMABLE, GRD_BASELAP
                                or GRD_TRUNCATION

*/

int CSWGrdAPI::grd_SetControlSurface (CSW_F *grid, int ncol, int nrow,
                           CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2, int type)
{
    int          istat;

    istat = grd_calc_obj.grd_set_control_surface (grid, ncol, nrow,
                                     x1, y1, x2, y2, type);
    return istat;

}  /*  end of function grd_SetControlSurface  */





/*
  ****************************************************************

             g r d _ T r i a n g l e s F r o m G r i d

  ****************************************************************

  function name:    grd_TrianglesFromGrid                 (int)

  call sequence:    grd_TrianglesFromGrid (grid, ncol, nrow,
                                           x1, y1, x2, y2,
                                           points, num_points,
                                           triangles, num_triangles)

  purpose:          Calculate triangles from the grid and return them
                    as an array of 3D points and as an array of triangle indices
                    pointing to the 3D points.  The triangles are calculated
                    by splitting each grid cell from upper left to lower
                    right where possible.  If this is not possible because
                    of null values in the grid, the split is attempted from lower
                    left to upper right.

  return value:     -1 = error
                     1 = success

  errors:           1 = Memory allocation failure.
                    2 = NULL grid pointer.
                    3 = ncol or nrow < 2 or > 100000000
                    4 = x1 > x2 or y1 > y2
                    5 = either triangles or num_triangles is NULL
                   99 = the corner points do not support single precision.

  calling parameters:

    grid       r  CSW_F*       Grid array with or without nulls.
                               If there are null values, they must be
                               greater than 1.e20 or less than -1.e20.
    ncol       r  int          Number of columns in the grid.
    nrow       r  int          Number of rows in the grid.
    x1         r  CSW_F        X coordinate of the lower left corner of the grid.
    y1         r  CSW_F        Y coordinate of lower left.
    x2         r  CSW_F        X coordinate of upper right.
    y2         r  CSW_F        Y coordinate of upper right.
    points     w  POint3D**    List of x,y,z points used to construct triangles.
    num_points w  int*         Number of 3D points.
    triangles  w  TRiangleIndex  **
                               Pointer to accept the array of output triangles.
    num_triangles  w   int*    Number of output triangles returned.

*/

int CSWGrdAPI::grd_TrianglesFromGrid (CSW_F *grid, int ncol, int nrow,
                           CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                           POint3D **points, int *num_points,
                           TRiangleIndex   **triangles, int *num_triangles)
{
    int             istat;

    istat = csw_CheckRange2 (x1, y1, x2, y2);
    if (istat == 0) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = grd_utils_obj.grd_triangles_from_grid (grid, ncol, nrow,
                                     x1, y1, x2, y2,
                                     points, num_points,
                                     triangles, num_triangles);
    return istat;

}  /*  end of function grd_TrianglesFromGrid  */





/*
  ****************************************************************

      g r d _ T r i a n g l e s F r o m G r i d D o u b l e

  ****************************************************************

  function name:    grd_TrianglesFromGridDouble           (int)

  call sequence:    grd_TrianglesFromGridDouble
                                          (grid, ncol, nrow,
                                           x1, y1, x2, y2,
                                           points, num_points,
                                           triangles, num_triangles)

  purpose:          Calculate triangles from the grid and return them
                    as an array of 3D points and as an array of triangle indices
                    pointing to the 3D points.  The triangles are calculated
                    by splitting each grid cell from upper left to lower
                    right where possible.  If this is not possible because
                    of null values in the grid, the split is attempted from lower
                    left to upper right.  The only difference between this and
                    grd_TrianglesFromGrid is that here the grid limits are
                    specified as doubles rather than CSW_F values.

  return value:     -1 = error
                     1 = success

  errors:           1 = Memory allocation failure.
                    2 = NULL grid pointer.
                    3 = ncol or nrow < 2 or > 100000000
                    4 = x1 > x2 or y1 > y2
                    5 = one or more of the output pointers is null

  calling parameters:

    grid       r  CSW_F*       Grid array with or without nulls.
                               If there are null values, they must be
                               greater than 1.e20 or less than -1.e20.
    ncol       r  int          Number of columns in the grid.
    nrow       r  int          Number of rows in the grid.
    x1         r  CSW_F        X coordinate of the lower left corner of the grid.
    y1         r  CSW_F        Y coordinate of lower left.
    x2         r  CSW_F        X coordinate of upper right.
    y2         r  CSW_F        Y coordinate of upper right.
    points     w  POint3D**    List of x,y,z points used to construct triangles.
    num_points w  int*         Number of 3D points.
    triangles  w  TRiangleIndex  **
                               Pointer to accept the array of output triangles.
    num_triangles  w   int*    Number of output triangles returned.

*/

int CSWGrdAPI::grd_TrianglesFromGridDouble
                          (CSW_F *grid, int ncol, int nrow,
                           double x1, double y1, double x2, double y2,
                           POint3D **points, int *num_points,
                           TRiangleIndex   **triangles, int *num_triangles)
{
    int             istat, i;
    CSW_F           fx1, fy1, fx2, fy2;
    POint3D         *pts;

    fx1 = 0.0f;
    fy1 = 0.0f;
    fx2 = (CSW_F)(x2 - x1);
    fy2 = (CSW_F)(y2 - y1);

    istat = grd_utils_obj.grd_triangles_from_grid (grid, ncol, nrow,
                                     fx1, fy1, fx2, fy2,
                                     points, num_points,
                                     triangles, num_triangles);

    pts = *points;
    for (i=0; i<*num_points; i++) {
        pts[i].x += x1;
        pts[i].y += y1;
    }

    return istat;

}  /*  end of function grd_TrianglesFromGridDouble  */






/*
  ****************************************************************

           g r d _ T r i a n g l e s F r o m I n d e x

  ****************************************************************

  function name:    grd_TrianglesFromIndex          (int)

  call sequence:    grd_TrianglesFromIndex (points,
                                            tri_index, ntriangles,
                                            triangles)

  purpose:          Convert 3D triangles from the indexed form to
                    the self contained form.

  return value:     -1 = error
                     1 = success

  errors:           1 = Malloc error.
                    2 = NULL or zero input.
                    3 = NULL output.

  calling parameters:

    points       r  POint3D*          Array of x,y,z points that goes with
                                      the triangle index array.
    tri_index    r  TRiangleIndex     Triangle index array.
    ntriangles   r  int               Number of triangles input and output.
    triangles    w  TRiangle3D*       Array of separate triangles.

*/

int CSWGrdAPI::grd_TrianglesFromIndex (POint3D *points,
                            TRiangleIndex   *tri_index, int ntriangles,
                            TRiangle3D *triangles)
{
    int               istat;

    istat = grd_utils_obj.grd_triangles_from_index (points,
                                      tri_index, ntriangles,
                                      triangles);
    return istat;

}  /*  end of function grd_TrianglesFromIndex  */





/*
  ****************************************************************

           g r d _ H e x a g o n s F r o m G r i d

  ****************************************************************

  function name:    grd_HexagonsFromGrid             (int)

  call sequence:    grd_HexagonsFromGrid (grid, ncol, nrow,
                                          xmin, ymin, xmax, ymax,
                                          points, npoints,
                                          sides, nsides,
                                          hexagons, nhex)

  purpose:          Calculate a set of closely packed hexagons from
                    a rectangular grid.  The hexagons are all regular
                    (equal sided and equal angles).  The hexagon array
                    has pointers to sides in the side array.  The side
                    array has pointers to points in the points array.

                    If the input grid has null values in it, the null
                    values are filled in by interpolating from the
                    surrounding non null values before the hexagons
                    are calculated.

  return value:     1 = success
                   -1 = failure

  errors:           1 = memory allocation failure
                    2 = NULL grid array
                    3 = ncol or nrow is less than 2 or greater than 1000000
                    4 = xmin >= xmax or ymin >= ymax
                    5 = output pointer is NULL
                   99 = the corner points do not support single precision.

  calling parameters:

    grid       r   CSW_F*            Rectangular grid array.
    ncol       r   int               number of grid columns
    nrow       r   int               number of grid rows
    xmin       r   CSW_F             minimum x of grid
    ymin       r   CSW_F             minimum y of grid
    xmax       r   CSW_F             maximum x of grid
    ymax       r   CSW_F             maximum y of grid
    points     w   POint3D**         pointer to receive point array
    npoints    w   int*              number of points
    sides      w   SIdeIndex**       pointer to receive side array
    nsides     w   int*              number of sides
    hexagons   w   HExagonIndex**    pointer to receive hexagon array
    nhex       w   int*              number of hexagons

*/

int CSWGrdAPI::grd_HexagonsFromGrid (CSW_F *grid, int ncol, int nrow,
                          CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax,
                          POint3D **points, int *npoints,
                          SIdeIndex **sides, int *nsides,
                          HExagonIndex **hexagons, int *nhex)
{
    int                istat;

    istat = csw_CheckRange2 (xmin, ymin, xmax, ymax);
    if (istat == 0) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = grd_utils_obj.grd_hexagons_from_grid (grid, ncol, nrow,
                                    xmin, ymin, xmax, ymax,
                                    points, npoints,
                                    sides, nsides,
                                    hexagons, nhex);
    return istat;

}  /*  end of function grd_HexagonsFromGrid  */






/*
  ****************************************************************

       g r d _ H e x a g o n s F r o m G r i d D o u b l e

  ****************************************************************

  function name:    grd_HexagonsFromGridDouble       (int)

  call sequence:    grd_HexagonsFromGridDouble
                                         (grid, ncol, nrow,
                                          xmin, ymin, xmax, ymax,
                                          points, npoints,
                                          sides, nsides,
                                          hexagons, nhex)

  purpose:          Calculate a set of closely packed hexagons from
                    a rectangular grid.  The hexagons are all regular
                    (equal sided and equal angles).  The hexagon array
                    has pointers to sides in the side array.  The side
                    array has pointers to points in the points array.
                    In this version, the grid limits are double precision.

  return value:     1 = success
                   -1 = failure

  errors:           1 = memory allocation failure
                    2 = NULL grid array
                    3 = ncol or nrow is less than 2 or greater than 1000000
                    4 = xmin >= xmax or ymin >= ymax
                    5 = output pointer is NULL

  calling parameters:

    grid       r   CSW_F*            Rectangular grid array.
    ncol       r   int               number of grid columns
    nrow       r   int               number of grid rows
    xmin       r   double            minimum x of grid
    ymin       r   double            minimum y of grid
    xmax       r   double            maximum x of grid
    ymax       r   double            maximum y of grid
    points     w   POint3D**         pointer to receive point array
    npoints    w   int*              number of points
    sides      w   SIdeIndex**       pointer to receive side array
    nsides     w   int*              number of sides
    hexagons   w   HExagonIndex**    pointer to receive hexagon array
    nhex       w   int*              number of hexagons

*/

int CSWGrdAPI::grd_HexagonsFromGridDouble
                         (CSW_F *grid, int ncol, int nrow,
                          double xmin, double ymin, double xmax, double ymax,
                          POint3D **points, int *npoints,
                          SIdeIndex **sides, int *nsides,
                          HExagonIndex **hexagons, int *nhex)
{
    int                istat, i;
    CSW_F              fx1, fy1, fx2, fy2;
    POint3D            *pts;

    fx1 = 0.0f;
    fy1 = 0.0f;
    fx2 = (CSW_F)(xmax - xmin);
    fy2 = (CSW_F)(ymax - xmin);

    istat = grd_utils_obj.grd_hexagons_from_grid (grid, ncol, nrow,
                                    fx1, fy1, fx2, fy2,
                                    points, npoints,
                                    sides, nsides,
                                    hexagons, nhex);

    pts = *points;
    for (i=0; i<*npoints; i++) {
        pts[i].x += xmin;
        pts[i].y += ymin;
    }

    return istat;

}  /*  end of function grd_HexagonsFromGridDouble  */





/*
  ****************************************************************

            g r d _ H e x a g o n s F r o m I n d e x

  ****************************************************************

  function name:    grd_HexagonsFromIndex           (int)

  call sequence:    grd_HexagonsFromIndex (points,
                                           sides,
                                           hex_index, nhex,
                                           hexagons)

  purpose:          Calculate individual hexagons from the list
                    of points, sides, and hexagon indices.

  return value:     -1 = error
                     1 = success

  errors:            2 = one or more input parameters is NULL
                     3 = the output pointer is NULL

  calling parameters:

    points       r   POint3D*       Array of 3D points.
    sides        r   SIdeIndex*     Array of sides.
    hex_index    r   HExagonIndex*  Array of hexagon indices.
    nhex         r   int            Number of input and output hexagons.
    hexagons     r   HExagon3D*     Array of individual hexagon structures.

*/

int CSWGrdAPI::grd_HexagonsFromIndex (POint3D *points,
                           SIdeIndex *sides,
                           HExagonIndex *hex_index, int nhex,
                           HExagon3D *hexagons)
{
    int                 istat;

    istat = grd_utils_obj.grd_hexagons_from_index (points,
                                     sides,
                                     hex_index, nhex,
                                     hexagons);
    return istat;

}  /*  end of function grd_HexagonsFromIndex  */




/*
  ****************************************************************

     g r d _ G e t C u r r e n t F a u l t S t r u c t s

  ****************************************************************

  function name:    grd_GetCurrentFaultStructs          (int)

  call sequence:    grd_GetCurrentFaultStructs (faults, num_faults)

  purpose:          Return a list of the fault vectors currently being
                    used in the grid or contour calculations.  The most
                    recent call to grd_CalcGrid or to con_CalcContours
                    set up these faults.  If you want the exact faults
                    used to calculate the grid in a grid file, you
                    should use the faults returned by this function.

  return value:     -1 = error
                     0 = no faults are available
                     1 = success

  errors:           1 = Memory allocation error.
                    2 = Either faults or num_faults is NULL.

  calling parameters:

    faults      w  FAultLineStruct**    Pointer to receive a list of
                                        FAultLineStructs.  The calling
                                        function must csw_Free this when finished
                                        using it.
    num_faults  w  int*                 Number of FAultLineStructs in the list.

*/

int CSWGrdAPI::grd_GetCurrentFaultStructs (FAultLineStruct **faults, int *num_faults)
{
    int                    istat;

    if (faults == NULL  ||  num_faults == NULL) {
        grd_utils_obj.grd_set_err (2);
        return -1;
    }

    istat = grd_fault_obj.grd_get_current_fault_structs (faults, num_faults);

    return istat;

}  /*  end of function grd_GetCurrentFaultStructs  */







/*
  ****************************************************************

         g r d _ F r e e F a u l t L i n e S t r u c t s

  ****************************************************************

  function name:    grd_FreeFaultLineStructs        (int)

  call sequence:    grd_FreeFaultLineStructs (faults, num_faults)

  purpose:          Free all the memory associated with a list of
                    FAultLineStruct structures.

  return value:     Always returns 1.

  calling parameters:

    faults      r  FAultLineStruct*    List of structures to csw_Free.
    num_faults  r  int                 Number of structures in the list.

*/

int CSWGrdAPI::grd_FreeFaultLineStructs (FAultLineStruct *faults, int num_faults)
{
    int                  istat;

    istat = grd_fault_obj.grd_free_fault_line_structs (faults, num_faults);

    return istat;

}  /*  end of function grd_FreeFaultLineStructs  */



/*
  ****************************************************************

         g r d _ F a u l t S t r u c t s T o A r r a y s

  ****************************************************************

  function name:    grd_FaultStructsToArrays          (int)

  call sequence:    grd_FaultStructsToArrays (fault_list, nlist,
                                              xfault, yfault, zfault,
                                              fault_points, linetypes, nfault)

  purpose:          Convert a list of fault structures (e.g. as returned from
                    grd_GetCurrentFaultStructs or from grd_ReadFile) into fault
                    point arrays (as needed by grd_CalcFaultedGrid or
                    con_CalcFaultedContours).

  return value:     -1 = error
                     1 = success

  errors:           1 = Memory allocation arror.
                    2 = A NULL fault_list or zero nlist is specified.
                    3 = One or more of xfault, yfault, fault_points or nfault
                        is NULL.

  calling parameters:

    fault_list     r   FAultLineStruct*     Array of fault line structures.
    nlist          r   int                  Number of structures in the array.
    xfault         w   CSW_F**              Returned array of x coordinates.
    yfault         w   CSW_F**              Returned array of y coordinates.
    zfault         w   CSW_F**              Returned array of z coordinates.
    fault_points   w   int**                Returned array with number of points
                                            per fault line.
    linetypes      w   int**                Returned array with the type of each
                                            line in the fault list.  The types are
                                            either GRD_BOUNDARY_CONSTRAINT or
                                            GRD_DISCONTINUITY_CONSTRAINT.
    nfault         w   int*                 Returned number of faults.

          The calling function must csw_Free the xfault, yfault and fault_points
          pointers when they are no longer needed.

*/

int CSWGrdAPI::grd_FaultStructsToArrays (FAultLineStruct *fault_list, int nlist,
                              CSW_F **xfault, CSW_F **yfault, CSW_F **zfault,
                              int **fault_points, int **linetypes,
                              int *nfault)
{
    int                 istat, *local_ltype, **ltype;

    ltype = linetypes;
    if (linetypes == NULL) {
        ltype = &local_ltype;
    }

    istat = grd_fault_obj.grd_fault_structs_to_arrays (fault_list, nlist,
                                         xfault, yfault, zfault,
                                         fault_points, ltype, nfault);
    if (linetypes == NULL) {
        csw_Free (*ltype);
    }

    return istat;

}  /*  end of function grd_FaultStructsToArrays  */





/*
  ****************************************************************

        g r d _ F a u l t A r r a y s T o S t r u c t s

  ****************************************************************

  function name:    grd_FaultArraysToStructs        (int)

  call sequence:    grd_FaultArraysToStructs (xfault, yfault, zfault,
                                              fault_points, ltypes, nfault,
                                              fault_list, nlist)

  purpose:          Convert compacted fault points (as used for grid and
                    contour calculation) into an array of FAultLineStruct
                    structures (as used to write to a file).

  return value:     -1 = error
                     1 = success

  errors:           1 = Memory allocation error.
                    2 = A NULL input pointer or zero nfault value is specified.
                    3 = A NULL output pointer is specified.

  calling parameters:

    xfault        r  CSW_F*             Array of fault x coordinates.
    yfault        r  CSW_F*             Array of fault y coordinates.
    zfault        r  CSW_F*             Array of fault z coordinates.
    fault_points  r  int*               Array with number of points per fault.
    ltypes        r  int*               Array with the type of each fault.
    nfault        r  int                Number of faults.
    fault_list    w  FAultLineStruct**  Pointer to receive an array of fault
                                        line structures.
    nlist         w  int*               Number of returned fault line structures.

*/

int CSWGrdAPI::grd_FaultArraysToStructs (CSW_F *xfault, CSW_F *yfault, CSW_F *zfault,
                              int *fault_points, int *ltypes, int nfault,
                              FAultLineStruct **fault_list, int *nlist)
{
    int                istat;

    istat = grd_fault_obj.grd_fault_arrays_to_structs (xfault, yfault, zfault,
                                         fault_points, ltypes, nfault,
                                         fault_list, nlist);

    return istat;

}  /*  end of function grd_FaultArraysToStructs  */




/*
  ****************************************************************

            g r d _ R e a d F a u l t L i n e F i l e

  ****************************************************************

  function name:    grd_ReadFaultLineFile         (int)

  call sequence:    grd_ReadFaultLineFile (filename, faults, nfaults)

  purpose:          Read an array of fault line structures from a text file.

  return value:     -1 = error
                     1 = success

  errors:           1 = memory allocation error
                    2 = cannot open the specified file for reading
                    3 = error reading the file

  calling parameters:

    filename    r   char*                Name of file to read.
    faults      w   FAultLineStruct**    Returned array of fault line structures.
    nfaults     w   int*                 Number of returned structures.

        note:  The application is responsible for csw_Freeing the array of fault
               line structures by calling grd_FreeFaultLineStructs when it
               is finished with the structures.

*/

int CSWGrdAPI::grd_ReadFaultLineFile (char *filename, FAultLineStruct **faults, int *nfaults)
{
    int                 istat;

    istat = grd_fault_obj.grd_read_fault_line_file (filename, faults, nfaults);
    return istat;

}  /*  end of function grd_ReadFaultLineFile  */




/*
  ****************************************************************

              g r d _ C o m b i n e F a u l t s

  ****************************************************************

  function name:      grd_CombineFaults          (int)

  call sequence:      grd_CombineFaults (faults1, nfaults1,
                                         faults2, nfaults2,
                                         faultsout, nfaultsout)

  purpose:            Combine two arrays of faults into a single array.  The
                      output faultsout array is allocated by the function
                      and it must be csw_Freed by the application when the
                      application is finished with it.

  return value:       status code

                      -1 = error
                       1 = success

  errors:             1 = Memory allocation error
                      2 = NULL faults1 or NULL faults2 or less than 1 nfaults1
                          or less than 1 nfaults2.
                      3 = NULL faultsout or NULL nfaultsout.

  calling parameters:

    faults1      r  FAultLineStruct*      Array of fault line structures.
    nfaults1     r  int                   Number of structures in faults1.
    faults2      r  FAultLineStruct*      Array of fault line structures.
    nfaults2     r  int                   Number of structures in faults2.
    faultsout    w  FAultLineStruct**     Pointer to receive an array of fault
                                          line structures.
    nfaultsout   w  int*                  Number of returned fault line structures.

*/

int CSWGrdAPI::grd_CombineFaults (FAultLineStruct *faults1, int nfaults1,
                       FAultLineStruct *faults2, int nfaults2,
                       FAultLineStruct **faultsout, int *nfaultsout)
{
    int                  istat;

    istat = grd_fault_obj.grd_combine_faults (faults1, nfaults1,
                                faults2, nfaults2,
                                faultsout, nfaultsout);

    return istat;

}  /*  end of function grd_CombineFaults  */




/*
  ****************************************************************

             g r d _ C r e a t e C l i p M a s k

  ****************************************************************

  function name:      grd_CreateClipMask        (GRdImage *)

  call sequence:      grd_CreateClipMask (xpoly, ypoly,
                                          npoly, ncomps, nverts,
                                          x1, y1, x2, y2,
                                          ncol, nrow)



  purpose:            Create an image that has 1 values inside the
                      specified polygon(s) and zero values outside.
                      This is meant to be used prior to calling
                      grd_CreateImage to specify that a color fill
                      be limited to the inside of a polygon.

  return value:       Returns a valid pointer to a filled in GRdImage
                      structure on success.  Returns NULL on failure.
                      The calling function is responsible for csw_Freeing
                      the returned structure (via grd_FreeImageData)
                      after it is no longer needed.

  errors:             1 = memory allocation failure




  calling parameters:

    xpoly     r    double*    Array of polygon x coordinates.
    ypoly     r    double*    Array of polygon y coordinates.
    npoly     r    int        Number of polygons.
    ncomps    r    int*       Array with number of components per polygon.
    nverts    r    int*       Array with number of points per component.
    x1        r    double     Minimum x of the image mask to be created.
    y1        r    double     Minimum y of image mask.
    x2        r    double     Maximum x of mask.
    y2        r    double     Maximum y of mask.
    ncol      r    int        Number of columns in the image mask.
    nrow      r    int        Number of rows in the mask.

*/

GRdImage* CSWGrdAPI::grd_CreateClipMask (double *xpoly, double *ypoly,
                             int npoly, int *ncomps, int *nverts,
                             double x1, double y1, double x2, double y2,
                             int ncol, int nrow)
{
    GRdImage      *result;

    result = grd_image_obj.grd_create_clip_mask (xpoly, ypoly,
                                   npoly, ncomps, nverts,
                                   x1, y1, x2, y2,
                                   ncol, nrow);
    return result;

}  /*  end of function grd_CreateClipMask  */





/*
  ****************************************************************

           g r d _ C r e a t e I m a g e G e o m e t r y

  ****************************************************************

  function name:      grd_CreateImageGeometry    GRdImage*

  call sequence:      grd_CreateImageGeometry (x1, y1, x2, y2,
                                               ncol, nrow)

  purpose:            This is a convenience function to create
                      a grid image structure with no data in it.
                      This structure can then be used as the
                      output image in grd_CreateImage.

  return value:       Returns a pointer to a GRDImage structure.
                      This structure has a NULL data member and
                      an undefined image_type member.  If an error
                      occurs a NULL pointer is returned.

  errors:             1 = memory allocation error

  calling parameters:

    x1     r      double     Minimum x of the image.
    y1     r      double     Minimum y of the image.
    x2     r      double     Maximum x of the image.
    y2     r      double     Maximum y of the image.


*/

GRdImage* CSWGrdAPI::grd_CreateImageGeometry (double x1, double y1,
                                   double x2, double y2,
                                   int ncol, int nrow)
{
    GRdImage      *result = NULL;
    bool          bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (result);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    result = (GRdImage *)csw_Calloc (sizeof(GRdImage));
    if (!result) {
        grd_utils_obj.grd_set_err(1);
        return NULL;
    }

    result->x1 = x1;
    result->y1 = y1;
    result->x2 = x2;
    result->y2 = y2;
    result->ncol = ncol;
    result->nrow = nrow;

    bsuccess = true;

    return result;

}  /*  end of function grd_CreateImageGeometry  */





/*
  ****************************************************************

                  g r d _ C r e a t e I m a g e

  ****************************************************************

  function name:     grd_CreateImage        (int)

  call sequence:     grd_CreateImage (grid, ncol, nrow,
                                      gx1, gy1, gx2, gy2,
                                      clip_mask, options,
                                      faults, nfaults,
                                      output_image)

  purpose:           Create a color image of the specified grid.
                     The colors used will be from the most recent
                     call to con_SetColorBands.  An optional clip
                     mask can limit the image to any arbitrary
                     area.  You can create a mask from a set of
                     polygons by running the grd_CreateClipMask
                     function prior to calling this function.  The
                     output image geometry must be filled into the
                     output_image structure prior to calling this.

  return value:      status value

                     -1 = error
                      1 = success

  errors:            1 = memory allocation error
                     2 = Either grid or output_image is NULL.
                     3 = The grid has less than 2 rows
                         or less than 2 columns.
                     4 = The mimimum grid x is >= the max x or the
                         min y is >= the max y.
                     5 = The clip_mask and output_image geometries
                         do not match.

  calling parameters:

    grid      r    CSW_F*     Grid data.
    ncol      r    int        Number of columns in the grid.
    nrow      r    int        Number of rows in the grid.
    gx1       r    double     Minimum x coordinate of the grid.
    gy1       r    double     Minimum y of the grid.
    gx2       r    double     Maximum x of the grid.
    gy2       r    double     Maximum y of the grid.
    clip_mask r    GRdImage*  Optional clip mask for the image.
    options   r    GRdImageOptions*
                              Optional options structure.
    faults    r    FAultLineStruct*
                              Array of fault lines used to calculate
                              the grid or NULL if there are none.
    nfaults   r    int        Number of fault lines.
    output_image
              r    GRdImage*  Grid image structure with the geometry
                              filled in and the data not defined.



*/

int CSWGrdAPI::grd_CreateImage (CSW_F *grid, int ncol, int nrow,
                     double gx1, double gy1,
                     double gx2, double gy2,
                     GRdImage *clip_mask,
                     GRdImageOptions *options,
                     FAultLineStruct *faults,
                     int nfaults,
                     GRdImage *output_image)
{
    int              istat;

    istat = grd_image_obj.grd_create_image (grid, ncol, nrow,
                              gx1, gy1, gx2, gy2,
                              clip_mask, options,
                              faults, nfaults,
                              output_image);

    return istat;

}  /*  end of function grd_CreateImage  */





/*
  ****************************************************************

              g r d _ F r e e I m a g e D a t a

  ****************************************************************

  function name:      grd_FreeImageData         (void)

  call sequence:      grd_FreeImageData (image)

  purpose:            Free the image structure specified.  This
                      function csw_Frees the internal data in the
                      structure and the structure itself.  Make
                      sure you do not reference the image structure
                      after calling this.  It is best to set the
                      pointer to NULL after this function returns.

  return value:       none

  errors:             none

  calling parameters:

    image    r    GRdImage*     Pointer to an image structure.

*/

void CSWGrdAPI::grd_FreeImageData (GRdImage *image)
{
    if (image) {
        if (image->data) csw_Free (image->data);
        image->data = NULL;
        csw_Free (image);
    }

    return;

}  /*  end of function grd_FreeImageData  */



/*
  ****************************************************************************

                       g r d _ T r i a n g u l a t e

  ****************************************************************************

    Calculate a triangular network from the points and lines specified.

*/

int CSWGrdAPI::grd_Triangulate  (double *xpts, double *ypts, double *zpts, int npts,
                      double *xlines, double *ylines, double *zlines,
                      int *linepoints, int *linetypes, int nlines,
                      NOdeStruct **nodes_out, EDgeStruct **edges_out,
                      TRiangleStruct **triangles_out,
                      int *num_nodes_out, int *num_edges_out, int *num_triangles_out)
{
    int                    istat;

/*
 * !!!! debug only
 */
    int                    i, icomp[500];
    int                    do_write;

/*
 * First, calculate the trimesh without constraints.
 */
    grd_triangle_obj.grd_set_shifts_for_debug (0.0, 0.0);
    istat = grd_triangle_obj.grd_calc_trimesh  (xpts, ypts, zpts, npts,
                               NULL, NULL, NULL,
                               NULL, NULL, 0,
                               nodes_out, edges_out, triangles_out,
                               num_nodes_out, num_edges_out, num_triangles_out);
    if (istat == -1) {
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write  &&  nlines <= 500) {
        for (i=0; i<nlines; i++) {
            icomp[i] = 1;
        }
        grd_WriteLines (xlines, ylines, zlines,
                        nlines, icomp, linepoints,
                        (char *)"fromgrid.xyz");
    }
    if (do_write) {
        grd_triangle_obj.grd_WriteTextTriMeshFile (
          0, NULL,
          *triangles_out, *num_triangles_out,
          *edges_out, *num_edges_out,
          *nodes_out, *num_nodes_out,
          (char *)"fromgrid.tri");
    }

/*
 * Add constraints to the trimesh if needed.
 */
    if (xlines != NULL  &&  nlines > 0) {
        istat =
          grd_triangle_obj.grd_add_lines_to_trimesh (
            xlines, ylines, zlines,
            linepoints, linetypes, nlines,
            0,
            nodes_out, edges_out, triangles_out,
            num_nodes_out, num_edges_out, num_triangles_out);
    }

    return istat;

}  /*  end of function grd_Triangulate  */




/*
 ***************************************************************************************

            g r d _ R e m o v e N o d e s F r o m T r i M e s h

 ***************************************************************************************

  Remove the specified nodes from the specified trimesh.  The trimesh is defined
  in the nodelist, edgelist and trilist arrays.  These can be the same as were
  returned from the grd_Triangulate function.

*/

int CSWGrdAPI::grd_RemoveNodesFromTriMesh (NOdeStruct **nodelist, int *numnodes,
                                EDgeStruct **edgelist, int *numedges,
                                TRiangleStruct **trilist, int *numtriangles,
                                int *nodes_to_remove, int num_nodes_to_remove)
{
    int                   istat;

    istat = grd_triangle_obj.grd_remove_nodes_from_trimesh (nodelist, numnodes,
                                           edgelist, numedges,
                                           trilist, numtriangles,
                                           nodes_to_remove, num_nodes_to_remove);


    return istat;

}  /* end of function grd_RemoveNodesFromTriMesh */



/*
 *******************************************************************************

               g r d _ C a l c T r i M e s h F r o m G r i d

 *******************************************************************************

  Calculate a topologically correct mesh of triangles from a rectangular grid.

*/

int CSWGrdAPI::grd_CalcTriMeshFromGrid (CSW_F *grid, int nc, int nr,
                             double x1, double y1, double x2, double y2,
                             double *xlines, double *ylines, double *zlines,
                             int *linepoints, int *linetypesin, int nlines,
                             int trimesh_style,
                             NOdeStruct **nodes_out, EDgeStruct **edges_out,
                             TRiangleStruct **triangles_out,
                             int *num_nodes_out, int *num_edges_out,
                             int *num_triangles_out)
{
    int                 istat, i, *ltypes = NULL, nl;
    int                 icomp[500], do_write;


    auto fscope = [&]()
    {
        csw_Free (ltypes);
    };
    CSWScopeGuard func_scope_guard (fscope);

    nl = nlines;
    if (nl < 0) nl = -nl;

    ltypes = NULL;
    if (nl > 0) {
        ltypes = (int *)csw_Malloc (nl * sizeof(int));
        if (ltypes == NULL) {
            grd_utils_obj.grd_set_err (1);
            return -1;
        }

        for (i=0; i<nl; i++) {
            if (linetypesin != NULL) {
                if (linetypesin[i] < -1000  ||
                    linetypesin[i] > 1000) {
                    linetypesin[i] = 0;
                }
                ltypes[i] = linetypesin[i];
            }
            else {
                ltypes[i] = 0;
            }
        }
    }

/*
 * Calculate an unconstrained trimesh from the grid.
 */
    if (trimesh_style != GRD_EQUILATERAL  &&
        trimesh_style != GRD_CELL_DIAGONALS) {
        trimesh_style = GRD_EQUILATERAL;
    }
    grd_triangle_obj.grd_set_shifts_for_debug (0.0, 0.0);
    istat = grd_triangle_obj.grd_grid_to_trimesh (grid, nc, nr,
                                 x1, y1, x2, y2,
                                 xlines, ylines, zlines,
                                 linepoints, ltypes, nlines,
                                 trimesh_style,
                                 nodes_out, edges_out, triangles_out,
                                 num_nodes_out, num_edges_out, num_triangles_out);
    if (istat == -1) {
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write  &&  nlines <= 500) {
        for (i=0; i<nlines; i++) {
            icomp[i] = 1;
        }
        grd_WriteLines (xlines, ylines, zlines,
                        nlines, icomp, linepoints,
                        (char *)"fromgrid.xyz");
    }
    if (do_write) {
        grd_triangle_obj.grd_WriteTextTriMeshFile (
          0, NULL,
          *triangles_out, *num_triangles_out,
          *edges_out, *num_edges_out,
          *nodes_out, *num_nodes_out,
          (char *)"fromgrid.tri");
    }

    return istat;

}  /* end of grd_CalcTriMeshFromGrid function */



/*
 *******************************************************************************

           g r d _ C a l c F a u l t e d T r i M e s h F r o m G r i d

 *******************************************************************************

  Calculate a topologically correct mesh of triangles from a rectangular grid.
  The faults are added as constraints on the trimesh.

*/

int CSWGrdAPI::grd_CalcFaultedTriMeshFromGrid
                            (CSW_F *grid, int nc, int nr,
                             double x1, double y1, double x2, double y2,
                             double *xlines, double *ylines, double *zlines,
                             int *linepoints, int *linetypesin, int nlines,
                             int trimesh_style,
                             NOdeStruct **nodes_out, EDgeStruct **edges_out,
                             TRiangleStruct **triangles_out,
                             int *num_nodes_out, int *num_edges_out,
                             int *num_triangles_out)

{
    int                 istat, i, *ltypes = NULL, nl;
    int                 icomp[500], do_write;


    auto fscope = [&]()
    {
        csw_Free (ltypes);
    };
    CSWScopeGuard func_scope_guard (fscope);


    nl = nlines;
    if (nl < 0) nl = -nl;

    if (nl > 0) {
        ltypes = (int *)csw_Malloc (nl * sizeof(int));
        if (ltypes == NULL) {
            grd_utils_obj.grd_set_err (1);
            return -1;
        }

        for (i=0; i<nl; i++) {
            if (linetypesin != NULL) {
                if (linetypesin[i] < -1000  ||
                    linetypesin[i] > 1000) {
                    linetypesin[i] = 0;
                }
                ltypes[i] = linetypesin[i];
            }
            else {
                ltypes[i] = 0;
            }
        }
    }

/*
 * First, calculate an unconstrained trimesh from the grid.
 */
    trimesh_style = GRD_EQUILATERAL;
    grd_triangle_obj.grd_set_shifts_for_debug (0.0, 0.0);
    istat = grd_triangle_obj.grd_grid_to_trimesh (grid, nc, nr,
                                 x1, y1, x2, y2,
                                 xlines, ylines, zlines,
                                 linepoints, ltypes, nlines,
                                 trimesh_style,
                                 nodes_out, edges_out, triangles_out,
                                 num_nodes_out, num_edges_out, num_triangles_out);
    if (istat == -1) {
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write  &&  nlines <= 500) {
        for (i=0; i<nlines; i++) {
            icomp[i] = 1;
        }
        grd_WriteLines (xlines, ylines, zlines,
                        nlines, icomp, linepoints,
                        (char *)"fromgrid.xyz");
    }
    if (do_write) {
        grd_triangle_obj.grd_WriteTextTriMeshFile (
          0, NULL,
          *triangles_out, *num_triangles_out,
          *edges_out, *num_edges_out,
          *nodes_out, *num_nodes_out,
          (char *)"fromgrid.tri");
    }

/*
 * Add constraints to the trimesh if needed.
 */
    if (xlines != NULL  &&  nlines > 0) {
        istat =
          grd_triangle_obj.grd_add_lines_to_trimesh (
            xlines, ylines, zlines,
            linepoints, ltypes, nlines,
            0,
            nodes_out, edges_out, triangles_out,
            num_nodes_out, num_edges_out, num_triangles_out);
    }

    return istat;

}  /* end of grd_CalcFaultedTriMeshFromGrid function */


/*
 ************************************************************************************

         g r d _ I n t e r p o l a t e U n f a u l t e d T r i M e s h

 ************************************************************************************

*/

int CSWGrdAPI::grd_InterpolateUnfaultedTriMesh (
    CSW_F   *grid, int ncol, int nrow,
    double xmin, double ymin, double xmax, double ymax,
    NOdeStruct *nodes, int num_nodes,
    int flag)
{

    int    istat;

    istat =
    grd_triangle_obj.grd_interpolate_unfaulted_trimesh_values (
        grid, ncol, nrow,
        xmin, ymin, xmax, ymax,
        nodes, num_nodes,
        flag);

    return istat;

}



/*
 ************************************************************************************

            g r d _ R e m o v e N u l l s F r o m T r i M e s h

 ************************************************************************************

  Remove the null nodes from the trimesh.  If an edge can be swapped to not
  use a null node, do the swapping based on the edge swap flag before removing
  the nodes.

*/

int CSWGrdAPI::grd_RemoveNullsFromTriMesh (NOdeStruct *nodes, int *numnodes,
                                EDgeStruct *edges, int *numedges,
                                TRiangleStruct *triangles, int *numtriangles,
                                double nullval, int edge_swap_flag)
{
    int                 istat;

    istat = grd_triangle_obj.grd_remove_nulls_from_trimesh (triangles, numtriangles,
                                           edges, numedges,
                                           nodes, numnodes,
                                           nullval, edge_swap_flag);
    return istat;

}  /* end of grd_RemoveNullsFromTriMesh function */



/*
 *******************************************************************************

        g r d _ C a l c T r i M e s h F r o m P o i n t e r G r i d

 *******************************************************************************

  Calculate a topologically correct mesh of triangles from a rectangular grid.
  The grid passed should be a grid of pointers to other data.  These pointers
  will be placed in the client_data member of the nodes returned in the trimesh.

*/

int CSWGrdAPI::grd_CalcTriMeshFromPointerGrid
                            (void **grid, int nc, int nr,
                             double x1, double y1, double x2, double y2,
                             double *xlines, double *ylines, double *zlines,
                             int *linepoints, int *linetypes, int nlines,
                             int trimesh_style,
                             NOdeStruct **nodes_out, EDgeStruct **edges_out,
                             TRiangleStruct **triangles_out,
                             int *num_nodes_out, int *num_edges_out,
                             int *num_triangles_out)
{
    int                 istat;
    linetypes = linetypes;

    grd_triangle_obj.grd_set_shifts_for_debug (0.0, 0.0);
    istat = grd_triangle_obj.grd_pointer_grid_to_trimesh
                                (grid, nc, nr,
                                 x1, y1, x2, y2,
                                 xlines, ylines, zlines,
                                 linepoints, nlines,
                                 trimesh_style,
                                 nodes_out, edges_out, triangles_out,
                                 num_nodes_out, num_edges_out, num_triangles_out);
    return istat;

}  /* end of grd_CalcTriMeshFromPointerGrid function */




/*
 *********************************************************************************

                g r d _ C a l c G r i d F r o m T r i M e s h

 *********************************************************************************

  Return a rectangular grid with its z values determined by linear interpolation
  of the nodes of the specified tri mesh.  Any Grid nodes not inside any triangle
  are set to the null value specified.

*/

int CSWGrdAPI::grd_CalcGridFromTriMesh (NOdeStruct *nodes, int numnodes,
                             EDgeStruct *edges, int numedges,
                             TRiangleStruct *triangles, int numtriangles,
                             double nullvalue,
                             CSW_F *grid, int ncol, int nrow,
                             double x1, double y1, double x2, double y2)
{
    int             istat;

    istat = grd_triangle_obj.grd_calc_grid_from_trimesh (nodes, numnodes,
                                        edges, numedges,
                                        triangles, numtriangles,
                                        nullvalue,
                                        grid, ncol, nrow,
                                        x1, y1, x2, y2);
    return istat;

}  /* end of function grd_CalcGridFromTriMesh */




/*
 ********************************************************************************

            g r d _ C a l c T r i M e s h B o u n d i n g B o x

 ********************************************************************************

  Return the coordinates of the trimesh bounding rectangle.

*/

int CSWGrdAPI::grd_CalcTriMeshBoundingBox (NOdeStruct *nodes, int numnodes,
                                EDgeStruct *edges, int numedges,
                                TRiangleStruct *triangles, int numtriangles,
                                double *x1, double *y1,
                                double *x2, double *y2,
                                double *z1, double *z2)
{
    int                 istat;

    istat = grd_triangle_obj.grd_calc_trimesh_bounding_box (nodes, numnodes,
                                           edges, numedges,
                                           triangles, numtriangles,
                                           x1, y1, x2, y2, z1, z2);
    return istat;

}  /* end of grd_CalcTriMeshBoundingBox function */



/*
 **************************************************************************

              g r d _ O u t l i n e T r i a n g l e s

 **************************************************************************

  Calculate a polygon that outlines the specified set of triangles.  The
  triangles do not need to be topologically connected.  They can intersect
  each other or not touch each other.  A set of polygons is returned.
  Each polygon represents the boundary of one set of interconnecting
  triangles from the input data.  There can be many different sets of
  interconnecting triangles, and therefore there can be many different
  polygons returned.

  errors:

    1 = Memory allocation failure.
    2 = Either nodes, edges or triangles is a NULL pointer.
    3 = Either num_nodes < 3, num_edges < 3 or num_triangles < 1.
    4 = Either xout or yout is a NULL pointer.
    5 = Either npout, ncout or nvout is a NULL pointer.
    6 = Either maxpts or maxcomp < 1.

*/

int CSWGrdAPI::grd_OutlineTriangles (NOdeStruct *nodes, int num_nodes,
                          EDgeStruct *edges, int num_edges,
                          TRiangleStruct *triangles, int num_triangles,
                          double *xout, double *yout,
                          int *npout, int *ncout, int *nvout,
                          int maxpts, int maxcomp)
{
    int             istat;

    istat = grd_triangle_obj.grd_outline_triangles (nodes, num_nodes,
                                   edges, num_edges,
                                   triangles, num_triangles,
                                   xout, yout,
                                   npout, ncout, nvout,
                                   maxpts, maxcomp);
    return istat;

}  /* end of function grd_OutlineTriangles */




/*
 **************************************************************************

                g r d _ G e t B u g L o c a t i o n s

 **************************************************************************

  Return the fault intersection bugs if any were found in the previous
  trimesh calculation.  This should be called after con_CalcContours
  to report any problems back to the user.

*/

int CSWGrdAPI::grd_GetBugLocations (double *bx, double *by, int *nbugs, int maxbugs)
{
    int             istat;

    istat = grd_triangle_obj.grd_get_bug_locations (bx, by, nbugs, maxbugs);
    return istat;

}  /* end of function grd_GetBugLocations */



/*
 ****************************************************************************

          g r d _ C o p y F a u l t L i n e S t r u c t s

 ****************************************************************************

*/

FAultLineStruct* CSWGrdAPI::grd_CopyFaultLineStructs (FAultLineStruct *in, int n)
{
    FAultLineStruct  *fls;

    fls = grd_fault_obj.grd_copy_fault_line_structs (in, n);

    return fls;
}



/*
 *****************************************************************************

         g r d _ C a l c u l a t e D r a i n a g e P o l y g o n s

 *****************************************************************************

*/

int CSWGrdAPI::grd_CalculateDrainagePolygons (
            CSW_F *grid, int ncol, int nrow,
            double x1, double y1, double x2, double y2,
            int future_flag,
            SPillpointStruct *spill_filter_list,
            int num_spill_filters,
            DRainagesStruct **drainages)
{
    int            istat;

    istat = grd_triangle_obj.grd_calculate_drainage_polygons (
            grid, ncol, nrow,
            x1, y1, x2, y2,
            future_flag,
            spill_filter_list,
            num_spill_filters,
            drainages);

    return istat;

}  /* end of function grd_CalculateDrainagePolygons */



/*
 **********************************************************************************

             g r d _ D e l e t e E d g e s F r o m T r i m e s h

 **********************************************************************************

*/

int CSWGrdAPI::grd_DeleteEdgesFromTriMesh (NOdeStruct *nodes, int numnodes,
                                EDgeStruct *edges, int numedges,
                                TRiangleStruct *triangles, int numtriangles,
                                int *edgenumlist, int nlist, int swapflag)
{
    int                  istat;

    istat = grd_triangle_obj.grd_delete_edges_from_trimesh (nodes, numnodes,
                                           edges, numedges,
                                           triangles, numtriangles,
                                           edgenumlist, nlist,
                                           swapflag);
    return istat;

}  /* end of function grd_DeleteEdgesFromTriMesh */



/*
 ************************************************************************************

                  g r d _ C l e a n u p T r i m e s h

 ************************************************************************************

  Remove deleted nodes, edges, and triangles and reset the pointers to the new lists.
  The input node, edge and triangle arrays are modified by this function.

*/

int CSWGrdAPI::grd_CleanupTriMesh (NOdeStruct *nodes, int *numnodesptr,
                        EDgeStruct *edges, int *numedgesptr,
                        TRiangleStruct *triangles, int *numtrianglesptr)
{
    int                 *elookup = NULL, *nlookup = NULL, *tlookup = NULL;
    int                 numnodes, numedges, numtriangles,
                        i, n, n1, n2, t1, t2, e1, e2, e3;
    int                 ntri, ndel, ndo, ndone;

    EDgeStruct          *eptr;
    TRiangleStruct      *tptr;


    auto fscope = [&]()
    {
        csw_Free (nlookup);
    };
    CSWScopeGuard func_scope_guard (fscope);


    numnodes = *numnodesptr;
    numedges = *numedgesptr;
    numtriangles = *numtrianglesptr;

    ndo = 0;

    for (;;) {

        ndone = 0;

        for (i=0; i<numedges; i++) {
            eptr = edges + i;
            if (eptr->deleted == 1) continue;
            if (eptr->tri1 < 0  &&  eptr->tri2 < 0) {
                eptr->deleted = 1;
                ndone++;
                continue;
            }
            ndel = 0;
            ntri = 0;
            if (eptr->tri1 >= 0) {
                ntri++;
                tptr = triangles + eptr->tri1;
                if (tptr->deleted == 1) {
                    ndel++;
                }
            }
            if (eptr->tri2 >= 0) {
                ntri++;
                tptr = triangles + eptr->tri2;
                if (tptr->deleted == 1) {
                    ndel++;
                }
            }
            if (ndel >= ntri) {
                eptr->deleted = 1;
                ndone++;
            }
        }

        for (i=0; i<numtriangles; i++) {
            tptr = triangles + i;
            if (tptr->deleted == 1) continue;
            if (tptr->edge1 < 0  ||
                tptr->edge2 < 0  ||
                tptr->edge3 < 0) {
                tptr->deleted = 1;
                ndone++;
                continue;
            }
            eptr = edges + tptr->edge1;
            if (eptr->deleted == 1) {
                tptr->deleted = 1;
                ndone++;
                continue;
            }
            eptr = edges + tptr->edge2;
            if (eptr->deleted == 1) {
                tptr->deleted = 1;
                ndone++;
                continue;
            }
            eptr = edges + tptr->edge3;
            if (eptr->deleted == 1) {
                tptr->deleted = 1;
                ndone++;
                continue;
            }
        }
        if (ndone < 1  ||  ndo > 10) {
            break;
        }
        ndo++;
    }

    n = numnodes + numedges + numtriangles;
    nlookup = (int *)csw_Malloc (n * sizeof(int));
    if (nlookup == NULL) {
        return -1;
    }

    elookup = nlookup + numnodes;
    tlookup = elookup + numedges;

    n = 0;
    for (i=0; i<numnodes; i++) {
        if (nodes[i].deleted) {
            nlookup[i] = -1;
        }
        else {
            nlookup[i] = n;
            n++;
        }
    }

    n = 0;
    for (i=0; i<numedges; i++) {
        if (edges[i].deleted  ||  edges[i].tri1 < 0) {
            elookup[i] = -1;
        }
        else {
            elookup[i] = n;
            n++;
        }
    }

    n = 0;
    for (i=0; i<numtriangles; i++) {
        if (triangles[i].deleted) {
            tlookup[i] = -1;
        }
        else {
            tlookup[i] = n;
            n++;
        }
    }

    n = 0;
    for (i=0; i<numnodes; i++) {
        if (nlookup[i] != -1) {
            if (n != i) {
                memcpy (nodes+n, nodes+i, sizeof(NOdeStruct));
            }
            n++;
        }
    }
    *numnodesptr = n;

    n = 0;
    for (i=0; i<numedges; i++) {
        if (elookup[i] != -1) {
            eptr = edges + i;
            n1 = eptr->node1;
            n2 = eptr->node2;
            eptr->node1 = nlookup[n1];
            eptr->node2 = nlookup[n2];
            t1 = eptr->tri1;
            t2 = eptr->tri2;
            eptr->tri1 = tlookup[t1];
            if (t2 >= 0) {
                eptr->tri2 = tlookup[t2];
            }
            if (i != n) {
                memcpy (edges+n, eptr, sizeof(EDgeStruct));
            }
            n++;
        }
    }
    *numedgesptr = n;

    n = 0;
    for (i=0; i<numtriangles; i++) {
        if (tlookup[i] != -1) {
            tptr = triangles + i;
            e1 = tptr->edge1;
            e2 = tptr->edge2;
            e3 = tptr->edge3;
            tptr->edge1 = elookup[e1];
            tptr->edge2 = elookup[e2];
            tptr->edge3 = elookup[e3];
            if (i != n) {
                memcpy (triangles+n, triangles+i, sizeof(TRiangleStruct));
            }
            n++;
        }
    }
    *numtrianglesptr = n;

    return 1;

}  /* end of function grd_CleanupTriMesh */



/*
 **********************************************************************************

         g r d _ D e l e t e T r i a n g l e s F r o m T r i m e s h

 **********************************************************************************

*/

int CSWGrdAPI::grd_DeleteTrianglesFromTriMesh (NOdeStruct *nodes, int numnodes,
                                EDgeStruct *edges, int numedges,
                                TRiangleStruct *triangles, int numtriangles,
                                int *trianglenumlist, int nlist)
{
    int                  istat;

    istat = grd_triangle_obj.grd_delete_triangles_from_trimesh (nodes, numnodes,
                                           edges, numedges,
                                           triangles, numtriangles,
                                           trianglenumlist, nlist);
    return istat;

}  /* end of function grd_DeleteTrianglesFromTriMesh */




/*
 ***************************************************************************************

                    g r d _ C a l c T r i M e s h D i p s

 ***************************************************************************************

*/

int CSWGrdAPI::grd_CalcTriMeshDips (NOdeStruct *nodes, int numnodes,
                         EDgeStruct *edges, int numedges,
                         TRiangleStruct *triangles, int numtriangles,
                         int degree_flag,
                         double *direction, double *amplitude)
{
    int                istat;

    istat = grd_triangle_obj.grd_calc_triangle_dips (nodes, numnodes,
                                    edges, numedges,
                                    triangles, numtriangles,
                                    degree_flag,
                                    direction, amplitude);
    return istat;

}  /* end of function grd_CalcTriMeshDips */




/*
 **************************************************************************

          g r d _ F r e e P o l y g o n S t r u c t s

 **************************************************************************

*/

int CSWGrdAPI::grd_FreePolygonStructs (POlygonStruct *polys, int npolys)
{
    int               i;

    for (i=0; i<npolys; i++) {
        if (polys[i].x) csw_Free (polys[i].x);
    }
    csw_Free (polys);

    return 1;

}  /* end of function grd_FreePolygonStructs */



/*
 *****************************************************************************

            g r d _ F r e e D r a i n a g e s S t r u c t

 *****************************************************************************

*/

int CSWGrdAPI::grd_FreeDrainagesStruct (DRainagesStruct *dptr)
{
    int                 i;
    SPillpointStruct    *sptr;

    if (dptr == NULL) return 1;

    if (dptr->polygon_list != NULL) {
        grd_FreePolygonStructs (dptr->polygon_list,
                                dptr->total_polygons);
    }

    if (dptr->ridgelist != NULL) {
        grd_FreePolygonStructs (dptr->ridgelist,
                                dptr->numridges);
    }

    if (dptr->nodes) csw_Free (dptr->nodes);
    if (dptr->edges) csw_Free (dptr->edges);
    if (dptr->triangles) csw_Free (dptr->triangles);

    if (dptr->spillpoint_list != NULL) {
        for (i=0; i<dptr->total_spillpoints; i++) {
            sptr = dptr->spillpoint_list + i;
            if (sptr->polygons_using) {
                csw_Free (sptr->polygons_using);
            }
        }
        csw_Free (dptr->spillpoint_list);
    }

    csw_Free (dptr);

    return 1;

}  /* end of function grd_FreeDrainagesStruct */




/*
 ************************************************************************************

            g r d _ W r i t e T r i M e s h F i l e

 ************************************************************************************

*/

int CSWGrdAPI::grd_WriteTriMeshFile
                 (TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  double xshift, double yshift,
                  const char *filename)
{

    int  istat =
    grd_fileio_obj.grd_write_tri_mesh_file 
                 (triangles, ntriangles,
                  edges, nedges,
                  nodes, numnodes,
                  xshift, yshift,
                  filename);

    return istat;

#if 0

    FILE         *fptr;
    int          i;
    char         line[500];

    fptr = fopen (filename, "w");
    if (fptr == NULL) {
        printf ("Cannot open the new tri mesh file\n");
        return -1;
    }

	sprintf (line, "!TXT_TMESH\n");
	fputs (line, fptr);

	sprintf (line, "0 0.0 0.0 0.0 0.0 0.0 0.0\n");
	fputs (line, fptr);

    sprintf (line, "%d %d %d\n", numnodes, nedges, ntriangles);
    fputs (line, fptr);

    for (i=0; i<numnodes; i++) {
        sprintf (line, "%g %g %g %d\n",
            nodes[i].x + xshift, nodes[i].y + yshift,
            nodes[i].z, nodes[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<nedges; i++) {
        sprintf (line, "%d %d %d %d %d\n",
            edges[i].node1, edges[i].node2, edges[i].tri1,
            edges[i].tri2, edges[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<ntriangles; i++) {
        sprintf (line, "%d %d %d %d\n",
            triangles[i].edge1, triangles[i].edge2,
            triangles[i].edge3, triangles[i].flag);
        fputs (line, fptr);
    }

    fclose (fptr);

    return 1;

#endif

}


/*
 *****************************************************************************

                g r d _ G e t N o d e s F o r T r i a n g l e

 *****************************************************************************

*/

int CSWGrdAPI::grd_GetNodesForTriangle (TRiangleStruct *triangle,
                             EDgeStruct *edgelist,
                             int *n1, int *n2, int *n3)
{
    int                  istat;

    istat = grd_triangle_obj.grd_get_nodes_for_triangle (triangle, edgelist,
                                        n1, n2, n3);
    return istat;

}  /* end of function grd_GetNodesForTriangle */



/*
 *********************************************************************************

        g r d _ B a c k I n t e r p o l a t e T r i M e s h S t r u c t

 *********************************************************************************

*/

int CSWGrdAPI::grd_BackInterpolateTriMeshStruct
                               (TRimeshStruct *tmptr,
                                double *x, double *y, double *z,
                                int npts)
{
    int                     istat;

    istat = grd_triangle_obj.grd_back_interpolate_tri_mesh
        (tmptr->triangles, tmptr->numtriangles,
         tmptr->edges, tmptr->numedges,
         tmptr->nodes, tmptr->numnodes,
         x, y, z,
         npts);
    return istat;

}  /* end of function grd_BackInterpolateTriMeshStruct */





/*
 *********************************************************************************

            g r d _ B a c k I n t e r p o l a t e T r i M e s h

 *********************************************************************************

*/

int CSWGrdAPI::grd_BackInterpolateTriMesh
                               (TRiangleStruct *triangles, int ntriangles,
                                EDgeStruct *edges, int nedges,
                                NOdeStruct *nodes, int numnodes,
                                double *x, double *y, double *z,
                                int npts)
{
    int                     istat;

    istat = grd_triangle_obj.grd_back_interpolate_tri_mesh
        (triangles, ntriangles,
         edges, nedges,
         nodes, numnodes,
         x, y, z,
         npts);
    return istat;

}  /* end of function grd_BackInterpolateTriMesh */


/*
 **************************************************************************************

                g r d _ T r i M e s h F r o m N o d e T r i a n g l e s

 **************************************************************************************

  Convert a set of triangles that has only corner nodes to a full triangular mesh with
  nodes edges and triangles.

*/

int CSWGrdAPI::grd_TriMeshFromNodeTriangles (NOdeTriangleStruct *node_triangles,
                                  int num_node_triangles,
                                  NOdeStruct *nodes, int num_nodes,
                                  EDgeStruct **edges, int *numedges,
                                  TRiangleStruct **triangles, int *numtriangles)
{
    int                     istat;

    istat = grd_triangle_obj.grd_tri_mesh_from_node_triangles (node_triangles,
                                              num_node_triangles,
                                              nodes,
                                              num_nodes,
                                              edges,
                                              numedges,
                                              triangles,
                                              numtriangles);
    return istat;

}  /* end of function grd_TriMeshFromNodeTriangles */




/*
 *********************************************************************************

                 g r d _ D r a p e L i n e s O n T r i M e s h

 *********************************************************************************

  Return x,y and z arrays for the draping of the specified lines on the specified
  triangular mesh.  The output lines are allocated by this function but ownership
  is relinquished back to the calling function, which must csw_Free them when needed.
  Each output array is allocated separately and must be csw_Freed separately.

*/

int CSWGrdAPI::grd_DrapeLinesOnTriMesh (int id1, int id2,
                             NOdeStruct *nodes, int numnodes,
                             EDgeStruct *edges, int numedges,
                             TRiangleStruct *triangles, int numtriangles,
                             double *xlines, double *ylines,
                             int *ilines, int nlines,
                             double **xout, double **yout, double **zout,
                             int **iout, int *nout)

{
    int             istat;

    istat = grd_triangle_obj.grd_drape_lines_on_tri_mesh (id1, id2,
                                         nodes, numnodes,
                                         edges, numedges,
                                         triangles, numtriangles,
                                         xlines, ylines, ilines, nlines,
                                         xout, yout, zout,
                                         iout, nout);
    return istat;

}  /* end of function grd_DrapeLinesOnTriMesh */


void CSWGrdAPI::grd_SetTriangleIndex (
    void *index,
    double    xmin,
    double    ymin,
    double    xmax,
    double    ymax,
    int       ncol,
    int       nrow,
    double    xspace,
    double    yspace)
{
    grd_triangle_obj.grd_set_triangle_index (
        index,
        xmin, ymin, xmax, ymax,
        ncol, nrow, xspace, yspace);
}


void CSWGrdAPI::grd_GetTriangleIndex (
    void **index,
    double    *xmin,
    double    *ymin,
    double    *xmax,
    double    *ymax,
    int       *ncol,
    int       *nrow,
    double    *xspace,
    double    *yspace)
{
    grd_triangle_obj.grd_get_triangle_index (
        index,
        xmin, ymin, xmax, ymax,
        ncol, nrow, xspace, yspace);
}


void CSWGrdAPI::grd_FreeTriangleIndex (
    void *index,
    int  ncells)
{
    grd_triangle_obj.grd_free_triangle_index (index, ncells);
    return;
}




/*
 **************************************************************************************

             g r d _ C a l c H o l e s I n T r i M e s h

 **************************************************************************************

  If either node of a triangle edge is NULL, then whack the edge, leaving a hole
  in the trimesh topology.

  The process may delete nodes, edges and triangles from the specifed arrays.  The
  number of (nodes, edges, triangles) returned can therefore be different form
  those passed.  The tri mesh is resynced to agree with these new numbers.

*/

int CSWGrdAPI::grd_CalcHolesInTriMesh (NOdeStruct *nodes, int *numnodes,
                            EDgeStruct *edges, int *numedges,
                            TRiangleStruct *triangles, int *numtriangles,
                            double zmin, double zmax)
{
    int               istat;

    istat = grd_triangle_obj.grd_calc_holes_in_tri_mesh (nodes, numnodes,
                                        edges, numedges,
                                        triangles, numtriangles,
                                        zmin, zmax);
    return istat;
}




/*
 **********************************************************************************

               g r d _ C l i p T r i M e s h T o P o l y g o n

 **********************************************************************************

  Remove edges from the polygon if their mid point is outside of the specified
  set of polygons.  The flag is set to 1 to remove outside edges or to -1 to
  remove inside edges.  For best results, the polygon should have been used
  as constraints in calculating the tri mesh.

*/

int CSWGrdAPI::grd_ClipTriMeshToPolygon (NOdeStruct **nodes, int *num_nodes,
                              EDgeStruct **edges, int *num_edges,
                              TRiangleStruct **triangles, int *num_triangles,
                              double *xpoly, double *ypoly,
                              int *polypoints, int npoly, int flag)

{
    int             istat;

    istat = grd_triangle_obj.grd_clip_tri_mesh_to_polygon (nodes, num_nodes,
                                          edges, num_edges,
                                          triangles, num_triangles,
                                          xpoly, ypoly,
                                          polypoints, npoly, flag);
    return istat;

}  /* end of grd_ClipTriMeshToPolygon function */




/*
 ******************************************************************************

              g r d _ T r i M e s h F r o m G r i d P o i n t s

 ******************************************************************************

  Calculate a tri mesh from the node locations of the grid, combined with the
  point locations.  The closest grid node to each point is not used in the tri
  mesh.

*/

int CSWGrdAPI::grd_TriMeshFromGridPoints (
    double *x, double *y, double *z, int npts,
    double *xlines, double *ylines, double *zlines,
    int *line_points, int *line_types, int nlines,
    CSW_F *grid, int ncol, int nrow,
    double xmin, double ymin, double xmax, double ymax,
    NOdeStruct **nodes, int *num_nodes,
    EDgeStruct **edges, int *num_edges,
    TRiangleStruct **triangles, int *num_triangles)
{
    int         istat;

    istat =  grd_triangle_obj.grd_tri_mesh_from_grid_points (x, y, z, npts,
                                            xlines, ylines, zlines,
                                            line_points, line_types, nlines,
                                            grid, ncol, nrow,
                                            xmin, ymin, xmax, ymax,
                                            nodes, num_nodes,
                                            edges, num_edges,
                                            triangles, num_triangles);

    return istat;

}  /* end of function grd_TriMeshFromGridPoints */



/*
 ***********************************************************************************

                g r d _ S e t P o l y C o n s t r a i n t F l a g

 ***********************************************************************************

*/

int CSWGrdAPI::grd_SetPolyConstraintFlag (int val)
{
    grd_triangle_obj.grd_set_poly_constraint_flag (val);
    return 1;
}




/*
 **********************************************************************************

        g r d _ C l i p T r i M e s h T o M u l t i p l e P o l y g o n s

 **********************************************************************************

  Remove edges from the polygon if their mid point is outside of the specified
  set of polygons.  The flag is set to 1 to remove outside edges or to -1 to
  remove inside edges.  For best results, the polygon should have been used
  as constraints in calculating the tri mesh.

*/

int CSWGrdAPI::grd_ClipTriMeshToMultiplePolygons
                             (NOdeStruct *nodes, int *num_nodes,
                              EDgeStruct *edges, int *num_edges,
                              TRiangleStruct *triangles, int *num_triangles,
                              double *xpoly, double *ypoly,
                              int *polycomps, int *polypoints, int npoly, int flag)
{
    int             istat;

    istat = grd_triangle_obj.grd_clip_tri_mesh_to_multiple_polygons
                                         (nodes, num_nodes,
                                          edges, num_edges,
                                          triangles, num_triangles,
                                          xpoly, ypoly,
                                          polycomps, polypoints, npoly, flag);
    return istat;

}  /* end of grd_ClipTriMeshToMultiplePolygons function */




int CSWGrdAPI::grd_FilterDataSpikes (double *xin, double *yin, double *zin,
                          int *ibad, int nin,
                          SPikeFilterOptions *options)
{
    int           istat;

    istat = grd_triangle_obj.grd_filter_data_spikes (xin, yin, zin, ibad, nin,
                                    options);

    return istat;

}  /* end of function grd_FilterDataSpikes */




int CSWGrdAPI::grd_FilterGridSpikes (CSW_F *grid, int ncol, int nrow,
                          int sfact)

{
    int           istat;

    istat = grd_calc_obj.grd_filter_grid_spikes (grid, ncol, nrow,
                                    sfact);

    return istat;

}  /* end of function grd_FilterGridSpikes */







/*
 *****************************************************************************

                g r d _ G e t X Y Z F o r T r i a n g l e

 *****************************************************************************

*/

int CSWGrdAPI::grd_GetXYZForTriangle (TRiangleStruct *triangle,
                           EDgeStruct *edgelist,
                           NOdeStruct *nodelist,
                           double *x, double *y, double *z)
{
    int                  istat, n1, n2, n3;
    NOdeStruct           *nptr;

    istat = grd_triangle_obj.grd_get_nodes_for_triangle (triangle, edgelist,
                                        &n1, &n2, &n3);
    if (istat == -1) {
        return -1;
    }

    nptr = nodelist+n1;
    x[0] = nptr->x;
    y[0] = nptr->y;
    z[0] = nptr->z;

    nptr = nodelist+n2;
    x[1] = nptr->x;
    y[1] = nptr->y;
    z[1] = nptr->z;

    nptr = nodelist+n3;
    x[2] = nptr->x;
    y[2] = nptr->y;
    z[2] = nptr->z;

    return 1;

}  /* end of function grd_GetXYZForTriangle */




/*
 *************************************************************************

           g r d _ C a l c D o u b l e P l a n e

 *************************************************************************
*/

int CSWGrdAPI::grd_CalcDoublePlane (double *x, double *y, double *z,
                         int npts, double *coef)
{
    int     istat;

    istat = grd_utils_obj.grd_calc_double_plane (x, y, z, npts, coef);
    return istat;

} /* end of function grd_CalcDoublePlane */


/*
 *************************************************************************

           g r d _ C a l c P r e c i s e D o u b l e P l a n e

 *************************************************************************

  Calculate the coefs of a plane in a "precise" manner.  The function uses
  singular value decomposition, which will handle a lot more "degenerate"
  cases than other methods.  The algorithm is quite a bit slower also,
  so only call this when degenerate cases may need to be handled.

*/

int CSWGrdAPI::grd_CalcPreciseDoublePlane (double *x, double *y, double *z,
                         int npts, double *coef)
{
#if 0
    int     istat;
    double  coef1[3];

    if (npts < 3) {
        grd_utils_obj.grd_set_err (2);
        return -1;
    }

/*
 * Calculate a plane with the original coordinates.
 */
    istat = grd_calc_double_trend_surface (x, y, z, npts, 1, coef1);
    if (istat == -1) {
        return istat;
    }
    memcpy (coef, coef1, 3 * sizeof(double));
#endif

// grd_triangle_obj.  etc.

    return 1;

} /* end of function grd_CalcPreciseDoublePlane */





/*
 ***************************************************************************

                g r d _ M a s k W i t h P o l y g o n

 ***************************************************************************

*/

int CSWGrdAPI::grd_MaskWithPolygon (char *mask, int ncol, int nrow,
                         double xmin, double ymin,
                         double xmax, double ymax,
                         int flag,
                         double *xpin, double *ypin,
                         int npin, int *icin, int *ipin)

{
    int        istat;

    istat = grd_utils_obj.grd_mask_with_polygon (mask, ncol, nrow,
                                   xmin, ymin, xmax, ymax,
                                   flag,
                                   xpin, ypin, npin, icin, ipin);
    return istat;

} /* end of function grd_MaskWithPolygon */



/*
 ****************************************************************************

            p l y _ B u i l d P o l y g o n s F r o m L i n e s

 ****************************************************************************

*/
int CSWGrdAPI::ply_BuildPolygonsFromLines (double *xline, double *yline,
                                int nline, int *line_pts,
                                double *xpout, double *ypout,
                                int *npolyout, int *nptsout,
                                int maxpts, int maxpoly)
{
    int          istat;
    CSWPolyGraph ply_graph_obj;

    istat = ply_graph_obj.ply_build_polygons_from_lines (xline, yline,
                                           nline, line_pts,
                                           xpout, ypout,
                                           npolyout, nptsout,
                                           maxpts, maxpoly);
    return istat;
}




/*
 ****************************************************************************

      p l y _ B u i l d P o l y g o n s F r o m T a g g e d L i n e s

 ****************************************************************************

*/

int CSWGrdAPI::ply_BuildPolygonsFromTaggedLines
                               (double *xline, double *yline, void **tags,
                                int nline, int *line_pts,
                                double *xpout, double *ypout, void **tagout,
                                int *npolyout, int *nptsout,
                                int maxpts, int maxpoly)
{
    int          istat;

    CSWPolyGraph ply_graph_obj;

    istat = ply_graph_obj.ply_build_polygons_from_tagged_lines
                                          (xline, yline, tags,
                                           nline, line_pts,
                                           xpout, ypout, tagout,
                                           npolyout, nptsout,
                                           maxpts, maxpoly);
    return istat;
}




/*
 *****************************************************************************

                  g r d _ D i s t a n c e T o L i n e

 *****************************************************************************

  Return the shortest distance between the specified point and the
  specified line in the dist parameter.  On success, the return value
  if the function is 1.  On error (bad input parameters) the return
  value is -1.

*/

int CSWGrdAPI::grd_DistanceToLine (double *x, double *y,
                        int npts,
                        double xp, double yp, double *dist)
{
    int             istat;

    if (x == NULL  ||  y == NULL  ||  npts < 2) {
        return -1;
    }

    istat = gpf_calclinedist2xy (x, y, npts,
                                 xp, yp, dist);
    return istat;
}



/*
 ******************************************************************************

                          p l y _ N e s t H o l e s

 ******************************************************************************

  Given a list of closed polygon components, return a list of polygons with
  the components properly nested.

*/

int ply_NestHoles (double *xpoly, double *ypoly,
                   int npoly, int *polypoints,
                   double *xpout, double *ypout,
                   int *npout, int *icout, int *ipout,
                   int max_points, int max_comps)
{
    int         istat;
    CSWPolyGraph  ply_graph_obj;

    istat = ply_graph_obj.ply_nest_holes (xpoly, ypoly,
                            npoly, polypoints,
                            xpout, ypout,
                            npout, icout, ipout,
                            max_points, max_comps);
    return istat;
}



/*
 *******************************************************************************

                  g r d _ R e f i n e C o n t o u r D a t a

 *******************************************************************************

  Insert points in the specified contour lines so that no adjacent points are
  more than the specified spacing distance apart in the output.  If the specified
  spacing would produce a huge amount of output points (aproximately 100 times
  the number of input points or more) then the spacing is adjusted to produce
  a reasonable number of output points.

  QWER The spline flag is ignored for now.  The interpolation is always linear.
  (7/10/02)

*/

int CSWGrdAPI::grd_RefineContourData (COntourDataStruct *clines,
                           int nclines, int spline_flag,
                           double spacing,
                           COntourDataStruct **clout)
{
    int           istat;

    istat = grd_utils_obj.grd_refine_contour_data (clines,
                                     nclines, spline_flag,
                                     spacing,
                                     clout);
    return istat;
}



/*
 *******************************************************************************

                g r d _ C l e a n u p C o n t o u r D a t a

 *******************************************************************************

*/

void CSWGrdAPI::grd_CleanupContourData (COntourDataStruct *clist, int nlist)
{
    grd_utils_obj.grd_cleanup_contour_data (clist, nlist);
}


/*
 *******************************************************************************

                  g r d _ C o n t o u r T o G r i d

 *******************************************************************************

  Calculate a grid from contour lines and optional point arrays.

*/

int CSWGrdAPI::grd_ContourToGrid (COntourDataStruct *clinesin,
                       int nclines,
                       double *xin, double *yin, double *zin,
                       int nin,
                       double x1, double y1, double x2, double y2,
                       int ncol, int nrow,
                       CSW_F *grid, char *mask,
                       GRidCalcOptions *options)
{
    int            istat;

    istat = grd_utils_obj.grd_contour_to_grid (clinesin, nclines,
                                 xin, yin, zin, nin,
                                 x1, y1, x2, y2,
                                 ncol, nrow,
                                 grid, mask,
                                 options);

    return istat;
}


/*
 *************************************************************************************

                    g r d _ C l e a n F i l e R e c L i s t

 *************************************************************************************

  Clean out the internals of a list of grid file records.  The list itself is
  not csw_Freed.  The calling function needs to csw_Free the list if appropriate.

*/

void CSWGrdAPI::grd_CleanFileRecList (GRidFileRec *list, int nlist)
{
    grd_fileio_obj.grd_clean_file_rec_list (list, nlist);
    return;
}



/*
 **************************************************************************

         g r d _ O u t l i n e T r i M e s h B o u n d a r y

 **************************************************************************

  Calculate a polygon that outlines the specified topologically connected
  tri mesh.  This follows the edges that are only used by one triangle to
  calculate the boundary.  If there are holes in the tri mesh, many
  components of the bounding polygon may be created.

  errors:

    1 = Memory allocation failure.
    2 = Either nodes, edges or triangles is a NULL pointer.
    3 = Either num_nodes < 3, num_edges < 3 or num_triangles < 1.
    4 = Either xout or yout is a NULL pointer.
    5 = Either npout, ncout or nvout is a NULL pointer.
    6 = Either maxpts or maxcomp < 1.

*/

int CSWGrdAPI::grd_OutlineTriMeshBoundary
                         (NOdeStruct *nodes, int num_nodes,
                          EDgeStruct *edges, int num_edges,
                          TRiangleStruct *triangles, int num_triangles,
                          double *xout, double *yout, double *zout,
                          int *nodeout,
                          int *npout, int *ncout, int *nvout,
                          int maxpts, int maxcomp)
{
    int             istat;

    istat = grd_triangle_obj.grd_outline_tri_mesh_boundary
                                  (nodes, num_nodes,
                                   edges, num_edges,
                                   triangles, num_triangles,
                                   xout, yout, zout,
                                   nodeout,
                                   npout, ncout, nvout,
                                   maxpts, maxcomp);
    return istat;

}  /* end of function grd_OutlineTriMeshBoundary */

/*
  ****************************************************************

   g r d _ D o u b l e F a u l t A r r a y s T o S t r u c t s

  ****************************************************************

  function name:    grd_DoubleFaultArraysToStructs        (int)

  call sequence:    grd_DoubleFaultArraysToStructs (xfault, yfault, zfault,
                                              fault_points, ltypes, nfault,
                                              fault_list, nlist)

  purpose:          Convert compacted fault points arrays to fault line
                    structures.

  return value:     -1 = error
                     1 = success

  errors:           1 = Memory allocation error.
                    2 = A NULL input pointer or zero nfault value is specified.
                    3 = A NULL output pointer is specified.

  calling parameters:

    xfault        r  double*            Array of fault x coordinates.
    yfault        r  double*            Array of fault y coordinates.
    zfault        r  double*            Array of fault z coordinates.
    fault_points  r  int*               Array with number of points per fault.
    ltypes        r  int*               Array with the type of each fault.
    nfault        r  int                Number of faults.
    fault_list    w  FAultLineStruct**  Pointer to receive an array of fault
                                        line structures.
    nlist         w  int*               Number of returned fault line structures.

*/

int CSWGrdAPI::grd_DoubleFaultArraysToStructs (double *xfault, double *yfault, double *zfault,
                              int *fault_points, int *ltypes, int nfault,
                              FAultLineStruct **fault_list, int *nlist)

{
    int                istat;

    istat = grd_fault_obj.grd_fault_arrays_to_structs_2 (xfault, yfault, zfault,
                                           fault_points, ltypes, nfault,
                                           fault_list, nlist);

    return istat;

}  /*  end of function grd_FaultArraysToStructs  */


/*
 ************************************************************************************

            g r d _ W r i t e T r i M e s h F i l e 2

 ************************************************************************************

*/

int CSWGrdAPI::grd_WriteTriMeshFile2
                 (TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  double xshift, double yshift,
                  double *xb, double *yb, int nb,
                  const char *filename)

{

    int  istat =
    grd_fileio_obj.grd_write_tri_mesh_file_2
                 (triangles, ntriangles,
                  edges, nedges,
                  nodes, numnodes,
                  xshift, yshift,
                  xb, yb, nb,
                  filename);

    return istat;

#if 0

    FILE         *fptr;
    int          i;
    char         line[500];

    fptr = fopen (filename, "w");
    if (fptr == NULL) {
        printf ("Cannot open the new tri mesh file\n");
        return -1;
    }

    if (xb != NULL  &&  yb != NULL  &&  nb > 3) {
        sprintf (line, "boundary\n");
        fputs (line, fptr);
        sprintf (line, "%d\n", nb);
        fputs (line, fptr);
        for (i=0; i<nb; i++) {
            sprintf (line, "%g %g\n", xb[i], yb[i]);
            fputs (line, fptr);
        }
    }

    sprintf (line, "%d %d %d\n", numnodes, nedges, ntriangles);
    fputs (line, fptr);

    for (i=0; i<numnodes; i++) {
        sprintf (line, "%g %g %g %d\n",
            nodes[i].x + xshift, nodes[i].y + yshift,
            nodes[i].z, nodes[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<nedges; i++) {
        sprintf (line, "%d %d %d %d %d\n",
            edges[i].node1, edges[i].node2, edges[i].tri1,
            edges[i].tri2, edges[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<ntriangles; i++) {
        sprintf (line, "%d %d %d %d\n",
            triangles[i].edge1, triangles[i].edge2,
            triangles[i].edge3, triangles[i].flag);
        fputs (line, fptr);
    }

    fclose (fptr);

    return 1;

#endif

}



/*
 ******************************************************************************

                  g r d _ C a l c C o n v e x H u l l

 ******************************************************************************

  Calculate a convex hull polygon around a set of x,y points.  On success, the
  xpoly_out, ypoly_out and npoly_out values are filled in and 1 is returned.  If there is
  an error, -1 is returned.  All errors are memory allocation errors.  If the
  specified number of points is less than 3, zero is returned.  If either zero
  or -1 is returned, the xpoly_out and ypoly_out values are set to NULL and the npoly_out
  value is set to zero.

*/

int CSWGrdAPI::grd_CalcConvexHull (
    double      seglen,
    double      *xpts,
    double      *ypts,
    int         npts,
    double      **xpoly_out,
    double      **ypoly_out,
    int         *npoly_out)
{
    double      *xpoly = NULL, *ypoly = NULL, *zpoly = NULL;
    double      *xchop = NULL, *ychop = NULL, *dchop = NULL;

    int         *nodeout = NULL, istat;
    int         npoly, maxpts, maxcomp, ipoly[100], icomp[100];

    NOdeStruct  *nodes = NULL;
    EDgeStruct  *edges = NULL;
    TRiangleStruct    *triangles = NULL;
    int         nnodes, nedges, ntris;
    int         chop_flag;
    int         np1, n, maxchop, i, j, jdo;
    double      dx, dy, dist, dtot, dtmin, dtavg;

    double      *zpts = NULL;

    bool        bpoly = false;
    bool        bchop = false;


    auto fscope = [&]()
    {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
        csw_Free (nodeout);
        if (bpoly == false) {
            csw_Free (xpoly);
            csw_Free (ypoly);
            csw_Free (zpoly);
        }
        if (bchop == false) {
            csw_Free (xchop);
            csw_Free (ychop);
            csw_Free (dchop);
        }
        csw_Free (zpts);

    };
    CSWScopeGuard func_scope_guard (fscope);


    chop_flag = 0;
    if (seglen > 0.0) {
        chop_flag = 1;
    }

/*
 * initialize output in case of error.
 */
    *xpoly_out = NULL;
    *ypoly_out = NULL;
    *npoly_out = 0;

    if (npts < 3) {
        return 0;
    }

/*
 * Allocate a temporary zero valued zpts to use for
 * the trimesh calculation.
 */
    zpts = (double *)csw_Calloc (npts * sizeof(double));
    if (zpts == NULL) {
        return -1;
    }

/*
 * Calculate a trimesh.  The negative npts signals to use
 * the convex hull rather than allowing slight indentations.
 * There are no constraint lines, thus the NULLs and zero in the
 * second and third lines of parameters.
 */
    nnodes = 0;
    nedges = 0;
    ntris = 0;
    grd_triangle_obj.grd_set_remove_zero_flag (0);
    istat = grd_triangle_obj.grd_calc_trimesh (
        xpts, ypts, zpts, -npts,
        NULL, NULL, NULL,
        NULL, NULL, 0,
        &nodes, &edges, &triangles,
        &nnodes, &nedges, &ntris);
    grd_triangle_obj.grd_set_remove_zero_flag (1);

    if (istat == -1) {
        return -1;
    }

/*
 * get the outside boundary polygon of the trimesh.
 * There cannot be any more boundary points than the
 * total edges plus 1.
 */
    maxpts = nedges + 1;
    if (maxpts < 100) maxpts = 100;
    maxcomp = 100;

    xpoly = NULL;
    ypoly = NULL;
    zpoly = NULL;
    nodeout = NULL;

    xpoly = (double *)csw_Malloc (maxpts * sizeof (double));
    ypoly = (double *)csw_Malloc (maxpts * sizeof (double));
    zpoly = (double *)csw_Malloc (maxpts * sizeof (double));
    nodeout = (int *)csw_Malloc (maxpts * sizeof(int));

    if (xpoly == NULL  ||
        ypoly == NULL  ||
        zpoly == NULL  ||
        nodeout == NULL) {
        return -1;
    }

    istat = grd_triangle_obj.grd_outline_tri_mesh_boundary (
        nodes, nnodes,
        edges, nedges,
        triangles, ntris,
        xpoly, ypoly, zpoly,
        nodeout,
        &npoly, ipoly, icomp,
        maxpts, maxcomp);

    csw_Free (nodes);
    nodes = NULL;
    csw_Free (edges);
    edges = NULL;
    csw_Free (triangles);
    triangles = NULL;
    csw_Free (zpoly);
    zpoly = NULL;
    csw_Free (nodeout);
    nodeout = NULL;

    if (istat == -1) {
        return -1;
    }

    xchop = ychop = dchop = NULL;
    if (chop_flag == 1) {
        np1 = icomp[0];
        maxchop = np1 * 20;
        xchop = (double *)csw_Malloc (maxchop * sizeof(double));
        ychop = (double *)csw_Malloc (maxchop * sizeof(double));
        dchop = (double *)csw_Malloc (np1 * sizeof(double));
        if (xchop == NULL  ||  ychop == NULL  ||  dchop == NULL) {
            return -1;
        }

        dtot = 0.0;
        for (i=0; i<np1-1; i++) {
            dx = xpoly[i+1] - xpoly[i];
            dy = ypoly[i+1] - ypoly[i];
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            dtot += dist;
            dchop[i] = dist;
        }

        if (dtot <= 0.0) {
            return -1;
        }

        dtavg = seglen;
        dtmin = dtot / (double)maxchop;
        dtmin *= 2.0;
        if (dtavg < dtmin) dtavg = dtmin;

        xchop[0] = xpoly[0];
        ychop[0] = ypoly[0];
        n = 1;

        for (i=0; i<np1-1; i++) {
            dist = dchop[i];
            if (dist <= dtavg) {
                xchop[n] = xpoly[i+1];
                ychop[n] = ypoly[i+1];
                n++;
                continue;
            }

            jdo = (int)(dist / dtavg + 0.5);
            dx = xpoly[i+1] - xpoly[i];
            dx /= jdo;
            dy = ypoly[i+1] - ypoly[i];
            dy /= jdo;
            for (j=1; j<jdo; j++) {
                xchop[n] = xpoly[i] + dx * j;
                ychop[n] = ypoly[i] + dy * j;
                n++;
            }

            xchop[n] = xpoly[i+1];
            ychop[n] = ypoly[i+1];
            n++;

        }

        csw_Free (xpoly);
        xpoly = NULL;
        csw_Free (ypoly);
        ypoly = NULL;
        csw_Free (dchop);
        zpoly = NULL;
        dchop = NULL;

        *xpoly_out = xchop;
        *ypoly_out = ychop;
        *npoly_out = n;
        bchop = true;
    }

    else {
        *xpoly_out = xpoly;
        *ypoly_out = ypoly;
        *npoly_out = icomp[0];
        bpoly = true;
    }

    return 1;

}




/*
 ************************************************************************************

            g r d _ W r i t e B i n a r y T r i M e s h F i l e

 ************************************************************************************

*/

int CSWGrdAPI::grd_WriteBinaryTriMeshFile
                 (int vused,
                  double *vbase,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename)

{
    int                  istat;

    istat = grd_fileio_obj.grd_write_binary_trimesh_file (
        vused, vbase,
        triangles, ntriangles,
        edges, nedges,
        nodes, numnodes,
        filename);
    return istat;
}

/*
 ************************************************************************************

            g r d _ Append B i n a r y T r i M e s h F i l e

 ************************************************************************************

*/

long CSWGrdAPI::grd_AppendBinaryTriMeshFile(
  int vused,
  double *vbase,
  TRiangleStruct *triangles, int ntriangles,
  EDgeStruct *edges, int nedges,
  NOdeStruct *nodes, int numnodes,
  char *filename
) {
  long istat;

  istat = grd_fileio_obj.grd_append_binary_trimesh_file (
    vused, vbase,
    triangles, ntriangles,
    edges, nedges,
    nodes, numnodes,
    filename
  );
  return istat;
}



/*
 ************************************************************************************

               g r d _ R e a d T r i M e s h F i l e

 ************************************************************************************

*/

int CSWGrdAPI::grd_ReadTriMeshFile
                 (char *filename,
                  int *vused_out, double **vbase_out,
                  TRiangleStruct **triangles_out, int *ntriangles_out,
                  EDgeStruct **edges_out, int *nedges_out,
                  NOdeStruct **nodes_out, int *numnodes_out)

{
    int          istat;
    FILE         *fptr = NULL;
    int          i;
    char         line[500], *ctmp = NULL;

    TRiangleStruct   *triangles = NULL;
    EDgeStruct       *edges = NULL;
    NOdeStruct       *nodes = NULL;
    int              numnodes, nedges, ntriangles;
    int              vused;
    double           *vbase = NULL;

    double           xt, yt, zt;
    int              n1, n2, t1, t2, e1, e2, e3;

    bool    bsuccess = false;

    auto fscope = [&]()
    {
        if (fptr != NULL) fclose (fptr);
        if (bsuccess == false) {
            csw_Free (vbase);
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * First attempt a binary file read.
 */
    istat = grd_fileio_obj.grd_read_binary_trimesh_file (
        filename,
        vused_out, vbase_out,
        triangles_out, ntriangles_out,
        edges_out, nedges_out,
        nodes_out, numnodes_out);

    if (istat == 1) {
        return 1;
    }

    if (istat == -1) {
        return -1;
    }

/*
 * This is not a binary file, attempt a text file read.
 */

    *triangles_out = NULL;
    *edges_out = NULL;
    *nodes_out = NULL;
    *ntriangles_out = 0;
    *nedges_out = 0;
    *numnodes_out = 0;
    *vused_out = -1;
    *vbase_out = NULL;

    fptr = fopen (filename, "r");
    if (fptr == NULL) {
        return -1;
    }

    ctmp =
    fileio_util.csw_fgets (line, 500, fptr);
    if (ctmp == NULL) {
        return -1;
    }

    if (strncmp (line, "!TXT_TMESH", 10) != 0) {
        return -1;
    }

    ctmp =
    fileio_util.csw_fgets (line, 500, fptr);
    if (ctmp == NULL) {
        return -1;
    }

    vbase = (double *)csw_Malloc (6 * sizeof(double));
    if (vbase == NULL) {
        return -1;
    }

    istat =
    sscanf (line, "%d %lf %lf %lf %lf %lf %lf",
            &vused, vbase, vbase+1, vbase+2, vbase+3, vbase+4, vbase+5);
    if (istat != 7) {
        return -1;
    }

    ctmp =
    fileio_util.csw_fgets (line, 500, fptr);
    if (ctmp == NULL) {
        return -1;
    }

    sscanf (line, "%d %d %d", &numnodes, &nedges, &ntriangles);
    if (numnodes < 3  ||  nedges < 3  ||  ntriangles < 1) {
        return -1;
    }

    nodes = (NOdeStruct *)csw_Calloc (numnodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Calloc (nedges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Calloc (ntriangles * sizeof(TRiangleStruct));
    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    for (i=0; i<numnodes; i++) {
        ctmp = fileio_util.csw_fgets (line, 500, fptr);
        if (ctmp == NULL) {
            return -1;
        }
        sscanf (line, "%lf %lf %lf %d", &xt, &yt, &zt, &n1);
        nodes[i].x = xt;
        nodes[i].y = yt;
        nodes[i].z = zt;
        nodes[i].flag = n1;
    }

    for (i=0; i<nedges; i++) {
        ctmp = fileio_util.csw_fgets (line, 500, fptr);
        if (ctmp == NULL) {
            return -1;
        }
        sscanf (line, "%d %d %d %d %d\n",
                &n1, &n2, &t1, &t2, &e1);
        edges[i].node1 = n1;
        edges[i].node2 = n2;
        edges[i].tri1 = t1;
        edges[i].tri2 = t2;
        edges[i].flag = e1;
    }

    for (i=0; i<ntriangles; i++) {
        ctmp = fileio_util.csw_fgets (line, 500, fptr);
        if (ctmp == NULL) {
            return -1;
        }
        sscanf (line, "%d %d %d %d\n",
                &e1, &e2, &e3, &t2);
        triangles[i].edge1 = e1;
        triangles[i].edge2 = e2;
        triangles[i].edge3 = e3;
        triangles[i].flag = t2;
    }

    fclose (fptr);

    bsuccess = true;

    *triangles_out = triangles;
    *ntriangles_out = ntriangles;
    *edges_out = edges;
    *nedges_out = nedges;
    *nodes_out = nodes;
    *numnodes_out = numnodes;
    *vused_out = vused;
    *vbase_out = vbase;

    return 1;
}


/*
 ************************************************************************************

               g r d _ R e a d T r i M e s h F r o m M u l t i F i l e

 ************************************************************************************

*/

int CSWGrdAPI::grd_ReadTriMeshFromMultiFile
                 (char *filename, long position,
                  int *vused_out, double **vbase_out,
                  TRiangleStruct **triangles_out, int *ntriangles_out,
                  EDgeStruct **edges_out, int *nedges_out,
                  NOdeStruct **nodes_out, int *numnodes_out)

{
    int          istat;
    FILE         *fptr = NULL;
    int          i;
    char         line[500], *ctmp = NULL;

    TRiangleStruct   *triangles = NULL;
    EDgeStruct       *edges = NULL;
    NOdeStruct       *nodes = NULL;
    int              numnodes, nedges, ntriangles;
    int              vused;
    double           *vbase = NULL;

    double           xt, yt, zt;
    int              n1, n2, t1, t2, e1, e2, e3;

    bool        bsuccess = false;

    auto fscope = [&]()
    {
        if (fptr != NULL) fclose (fptr);
        if (bsuccess == false) {
            csw_Free (vbase);
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * First attempt a binary file read.
 */
    istat = grd_fileio_obj.grd_read_binary_trimesh_from_multi_file (
        filename, position,
        vused_out, vbase_out,
        triangles_out, ntriangles_out,
        edges_out, nedges_out,
        nodes_out, numnodes_out);

    if (istat == 1) {
        return 1;
    }

    if (istat == -1) {
        return -1;
    }

/*
 * This is not a binary file, attempt a text file read.
 */

    *triangles_out = NULL;
    *edges_out = NULL;
    *nodes_out = NULL;
    *ntriangles_out = 0;
    *nedges_out = 0;
    *numnodes_out = 0;
    *vused_out = -1;
    *vbase_out = NULL;

    fptr = fopen (filename, "r");
    if (fptr == NULL) {
        return -1;
    }

    ctmp =
    fileio_util.csw_fgets (line, 500, fptr);
    if (ctmp == NULL) {
        return -1;
    }

    if (strncmp (line, "!TXT_TMESH", 10) != 0) {
        return -1;
    }

    ctmp =
    fileio_util.csw_fgets (line, 500, fptr);
    if (ctmp == NULL) {
        return -1;
    }

    vbase = (double *)csw_Malloc (6 * sizeof(double));
    if (vbase == NULL) {
        return -1;
    }

    istat =
    sscanf (line, "%d %lf %lf %lf %lf %lf %lf",
            &vused, vbase, vbase+1, vbase+2, vbase+3, vbase+4, vbase+5);
    if (istat != 7) {
        return -1;
    }

    ctmp =
    fileio_util.csw_fgets (line, 500, fptr);
    if (ctmp == NULL) {
        return -1;
    }

    sscanf (line, "%d %d %d", &numnodes, &nedges, &ntriangles);
    if (numnodes < 3  ||  nedges < 3  ||  ntriangles < 1) {
        return -1;
    }

    nodes = (NOdeStruct *)csw_Calloc (numnodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Calloc (nedges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Calloc (ntriangles * sizeof(TRiangleStruct));
    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    for (i=0; i<numnodes; i++) {
        ctmp = fileio_util.csw_fgets (line, 500, fptr);
        if (ctmp == NULL) {
            return -1;
        }
        sscanf (line, "%lf %lf %lf %d", &xt, &yt, &zt, &n1);
        nodes[i].x = xt;
        nodes[i].y = yt;
        nodes[i].z = zt;
        nodes[i].flag = n1;
    }

    for (i=0; i<nedges; i++) {
        ctmp = fileio_util.csw_fgets (line, 500, fptr);
        if (ctmp == NULL) {
            return -1;
        }
        sscanf (line, "%d %d %d %d %d\n",
                &n1, &n2, &t1, &t2, &e1);
        edges[i].node1 = n1;
        edges[i].node2 = n2;
        edges[i].tri1 = t1;
        edges[i].tri2 = t2;
        edges[i].flag = e1;
    }

    for (i=0; i<ntriangles; i++) {
        ctmp = fileio_util.csw_fgets (line, 500, fptr);
        if (ctmp == NULL) {
            return -1;
        }
        sscanf (line, "%d %d %d %d\n",
                &e1, &e2, &e3, &t2);
        triangles[i].edge1 = e1;
        triangles[i].edge2 = e2;
        triangles[i].edge3 = e3;
        triangles[i].flag = t2;
    }

    bsuccess = true;

    *triangles_out = triangles;
    *ntriangles_out = ntriangles;
    *edges_out = edges;
    *nedges_out = nedges;
    *nodes_out = nodes;
    *numnodes_out = numnodes;
    *vused_out = vused;
    *vbase_out = vbase;

    return 1;
}



/*
 ************************************************************************************

                g r d _ W r i t e T e x t T r i M e s h F i l e

 ************************************************************************************

*/

#define TEXT_TMESH_VERSION            "1.00"

int CSWGrdAPI::grd_WriteTextTriMeshFile
                 (int vused,
                  double *vbasein,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename)

{

    int  istat =
    grd_fileio_obj.grd_write_text_tri_mesh_file
                 (vused,
                  vbasein,
                  triangles, ntriangles,
                  edges, nedges,
                  nodes, numnodes,
                  filename);
    return  istat;

}



/*
 * Returns position in file where we wrote the data, or -1 on i/o error.
 */
long CSWGrdAPI::grd_AppendTextTriMeshFile
                 (int vused,
                  double *vbase,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename)

{

    long    istat =

    grd_fileio_obj.grd_append_text_tri_mesh_file
                 (vused,
                  vbase,
                  triangles, ntriangles,
                  edges, nedges,
                  nodes, numnodes,
                  filename);
    return istat;


}




/*
 *************************************************************************************

                  g r d _ A d d L i n e s T o T r i M e s h

 *************************************************************************************

  Add a set of constraint lines to a pre existing trimesh.

*/

int CSWGrdAPI::grd_AddLinesToTriMesh
    (
     double *xlinesin, double *ylinesin, double *zlinesin,
     int *linepointsin, int *lineflagsin, int nlinesin,
     int exact_flag,
     NOdeStruct **nodes_out, EDgeStruct **edges_out,
     TRiangleStruct **triangles_out,
     int *num_nodes_out, int *num_edges_out, int *num_triangles_out)
{
    int istat;

    istat = grd_triangle_obj.grd_add_lines_to_trimesh
        (xlinesin, ylinesin, zlinesin,
         linepointsin, lineflagsin, nlinesin,
         exact_flag,
         nodes_out, edges_out,
         triangles_out,
         num_nodes_out, num_edges_out, num_triangles_out);

    return istat;
}



/*
 ******************************************************************************

             g r d _ C a l c A v e r a g e E d g e L e n g t h

 ******************************************************************************

  Return the average length of the specifed edges, applying the specified
  long_short_flag.  If the flag is set to zero, then all edges will be used
  to calculate the average length.  If the flag is set to GRD_IGNORE_LONG_EDGES,
  then the short edges are the only ones averaged.  If flag is set to
  GRD_IGNORE_SHORT_EDGES, then only the long edges are used.

  A histogram of number of edges vs edge length is used to ignore long or
  short edges in a relatively intelligent way.

  This function will only return an exact arithmetic mean if the flag is
  set to zero.  In all other cases a "nominal best average" is returned.

  If no result can be calculated, 1.e30 is returned.

*/

double CSWGrdAPI::grd_CalcAverageEdgeLength (
    NOdeStruct        *nodes,
    EDgeStruct        *edges,
    int               num_edges,
    int               long_short_flag)
{
    double            value;

    value =
    grd_triangle_obj.grd_calc_average_edge_length (
        nodes,
        edges,
        num_edges,
        long_short_flag);

    return value;
}






double CSWGrdAPI::grd_CalcAverageConstraintSegmentLength (
    double *xline, double *yline, int *npline, int nline)
{
    double       dx, dy, dist, dmin, dmax, dtot, avlen;
    int          i, j, n, ntot;

    avlen = 1.e30;

    if (xline == NULL  ||  yline == NULL  ||
        npline == NULL  ||  nline < 1) {
        return avlen;
    }

    dmin = 1.e30;
    dmax = 0.0;
    dtot = 0.0;
    n = 0;
    ntot = 0;
    for (i=0; i<nline; i++) {
        for (j=0; j<npline[i]-1; j++) {
            dx = xline[n] - xline[n+1];
            dy = yline[n] - yline[n+1];
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist < dmin) dmin = dist;
            if (dist > dmax) dmax = dist;
            dtot += dist;
            ntot++;
            n++;
        }
        n++;
    }

    if (ntot < 1) {
        return avlen;
    }

    dtot /= ntot;
    avlen = dtot;

    return avlen;

}



/*
 ******************************************************************************

               g r d _ D e c i m a t e C o n s t r a i n t

 ******************************************************************************

  Remove points in the line that are less than the specified distance apart.
  This is a "dumb" decimation.  No attempt is made to identify the points that
  are more significant in defining the shape of the line.

*/

int CSWGrdAPI::grd_DecimateConstraint (
    double          *xline,
    double          *yline,
    double          *zline,
    int             *nline,
    double          decimation_distance)
{
    int             istat;

    istat =
    grd_triangle_obj.grd_decimate_constraint (
        xline,
        yline,
        zline,
        nline,
        decimation_distance);

    return istat;
}



/*
 *********************************************************************************

                     g r d _ W r i t e P o i n t s

 *********************************************************************************

  Write a set of xyz points out to the specified ascii file.  This is used
  almost exclusively for debugging purposes.

*/

int CSWGrdAPI::grd_WritePoints (double *x,
                     double *y,
                     double *z,
                     int npts,
                     char *fname)
{
    int  istat =
    grd_fileio_obj.grd_write_points 
                    (x,
                     y,
                     z,
                     npts,
                     fname);

    return istat;

#if 0

    FILE         *fptr;
    int          i;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||  z == NULL  ||
        npts < 1  ||  fname == NULL) {
        return -1;
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    for (i=0; i<npts; i++) {
        sprintf (c200, "%g %g %g\n", x[i], y[i], z[i]);
        fputs (c200, fptr);
    }

    fclose (fptr);

    return 1;

#endif

}



/*
 *********************************************************************************

                     g r d _ W r i t e F l o a t P o i n t s

 *********************************************************************************

  Write a set of xyz points out to the specified ascii file.  This is used
  almost exclusively for debugging purposes.

*/

int CSWGrdAPI::grd_WriteFloatPoints (CSW_F  *x,
                          CSW_F  *y,
                          CSW_F  *z,
                          int npts,
                          const char *fname)
{
    int   istat = grd_fileio_obj.
        grd_write_float_points (x,
                          y,
                          z,
                          npts,
                          fname);
    return istat;

#if 0

    FILE         *fptr;
    int          i;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||  z == NULL  ||
        npts < 1  ||  fname == NULL) {
        return -1;
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    for (i=0; i<npts; i++) {
        sprintf (c200, "%g %g %g\n", x[i], y[i], z[i]);
        fputs (c200, fptr);
    }

    fclose (fptr);

    return 1;

#endif

}



/*
 *********************************************************************************

                     g r d _ W r i t e L i n e s

 *********************************************************************************

  Write a set of x, y and (optional) z coordinates to a file as individual
  polylines.

*/

int CSWGrdAPI::grd_WriteLines (double *x,
                   double *y,
                   double *z,
                   int    np,
                   int    *nc,
                   int    *nv,
                   char *fname)
{
    FILE         *fptr;
    int          i, j, k, n1, n2;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||
        nc == NULL  ||  nv == NULL  ||
        np < 1  ||  fname == NULL) {
        return -1;
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    n1 = 0;
    for (i=0; i<np; i++) {
        n1 += nc[i];
    }
    sprintf (c200, "%d\n", n1);
    fputs (c200, fptr);

    n1 = 0;
    n2 = 0;
    for (i=0; i<np; i++) {
        for (j=0; j<nc[i]; j++) {
            sprintf (c200, "%d\n", nv[n1]);
            fputs (c200, fptr);
            for (k=0; k<nv[n1]; k++) {
                if (z != NULL) {
                    sprintf (c200, "%g %g %g\n", x[n2], y[n2], z[n2]);
                    fputs (c200, fptr);
                }
                else {
                    sprintf (c200, "%g %g 0.0\n", x[n2], y[n2]);
                    fputs (c200, fptr);
                }
                n2++;
            }
            n1++;
        }
    }

    fclose (fptr);

    return 1;

}




/*
 ******************************************************************************

            g r d _ R e m o v e S l i v e r T r i a n g l e s

 ******************************************************************************

*/

int CSWGrdAPI::grd_RemoveSliverTriangles (
    NOdeStruct         **nodes,
    EDgeStruct         **edges,
    TRiangleStruct     **triangles,
    int                *num_nodes,
    int                *num_edges,
    int                *num_triangles,
    double             height_base_ratio)
{
    int                istat;

    istat = grd_triangle_obj.grd_remove_sliver_triangles (
        nodes, edges, triangles,
        num_nodes, num_edges, num_triangles,
        height_base_ratio);

    return istat;

}



/*
 *********************************************************************************

          g r d _ C a l c G r i d F r o m T r i M e s h N o d e s

 *********************************************************************************

  Use the xyz coordinates from the specified node list to calculate a non
  faulted grid.

*/

int CSWGrdAPI::grd_CalcGridFromTriMeshNodes
                            (NOdeStruct *nodes, int numnodes,
                             CSW_F *grid, int ncol, int nrow,
                             double x1, double y1, double x2, double y2)

{
    int             istat, i;
    double          *x = NULL, *y = NULL, *z = NULL;


    auto fscope = [&]()
    {
        csw_Free (x);
    };
    CSWScopeGuard func_scope_guard (fscope);



    if (numnodes < 3) {
        return -1;
    }

    x = (double *)csw_Malloc (numnodes * 3 * sizeof(double));
    if (x == NULL) {
        return -1;
    }
    y = x + numnodes;
    z = y + numnodes;

    for (i=0; i<numnodes; i++) {
        x[i] = nodes[i].x;
        y[i] = nodes[i].y;
        z[i] = nodes[i].z;
    }

    istat =
    grd_CalcGridFromDouble (
        x, y, z, NULL, numnodes,
        grid, NULL, NULL, ncol, nrow,
        x1, y1, x2, y2,
        NULL, 0,
        NULL);

    return istat;

}  /* end of function grd_CalcGridFromTriMeshNodes */


/*
 ***********************************************************************

       g r d _ I n s e r t C r o s s i n g P o i n t s

 ***********************************************************************

  Insert points into each line where the lines intersect each other.

*/
int CSWGrdAPI::grd_InsertCrossingPoints (
    double *x1, double *y1, double *z1, int n1,
    double *x2, double *y2, double *z2, int n2,
    double *x3, double *y3, double *z3, int *tag3, int *n3,
    double *x4, double *y4, double *z4, int *tag4, int *n4,
    int maxout,
    double *xw, double *yw, double *zw, int *tagw, int maxwork)
{
    int     istat;

    istat = grd_triangle_obj.grd_insert_crossing_points (
        x1, y1, z1, NULL, n1,
        x2, y2, z2, NULL, n2,
        x3, y3, z3, tag3, n3,
        x4, y4, z4, tag4, n4,
        maxout,
        xw, yw, zw, tagw, maxwork);

    return istat;

}

/*
 *************************************************************************

              g r d _ D e c i m a t e T a g g e d L i n e

 *************************************************************************

  Decimate the specified line, without removing any points with a
  non zero tag value.

*/

int CSWGrdAPI::grd_DecimateTaggedLine (
    double *x, double *y, double *z, int *tag, int *n)
{
    int      istat;

    istat = grd_triangle_obj.grd_decimate_tagged_line (
        x, y, z, tag, n);

    return istat;

}


int CSWGrdAPI::grd_DumpGridValues (
    char *fname,
    CSW_F   *grid,
    int     ncol,
    int     nrow)
{
    FILE     *fptr;
    int      i, j, offset, k;
    char     c50[50];

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    for (i=0; i<nrow; i++) {
        fputs ("\n", fptr);
        offset = i * ncol;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            if (grid[k] > 1.e20  ||  grid[k] < -1.e20) {
                sprintf (c50, "%7.2g ", grid[k]);
            }
            else {
                sprintf (c50, "%7.2f ", grid[k]);
            }
            fputs (c50, fptr);
            if (j != 0  && j % 8 == 0) {
                fputs ("\n", fptr);
            }
        }
        if ((ncol - 1) % 8 != 0) {
            fputs ("\n", fptr);
        }
    }

    fclose (fptr);

    return 1;

}



int CSWGrdAPI::grd_WriteXYZGridFile (
    const char *fname,
    CSW_F   *grid,
    int     ncol,
    int     nrow,
    int     ncoarse,
    double x1, double y1, double x2, double y2)
{
    FILE     *fptr;
    int      i, j, offset, k;
    char     c50[200];
    double   dx, dy, xt, yt;

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    dx = (x2 - x1) / (ncol - 1);
    dy = (y2 - y1) / (nrow - 1);

    for (i=0; i<nrow; i+=ncoarse) {
        fputs ("\n", fptr);
        offset = i * ncol;
        yt = y1 + i * dy;
        for (j=0; j<ncol; j+=ncoarse) {
            xt = x1 + j * dx;
            k = offset + j;
            if (grid[k] > 1.e20  ||  grid[k] < -1.e20) {
                sprintf (c50, "%f %f %7.2g\n", xt, yt, grid[k]);
            }
            else {
                sprintf (c50, "%f %f %7.2f\n", xt, yt, grid[k]);
            }
            fputs (c50, fptr);
        }
    }

    fclose (fptr);

    return 1;

}


/*
 *********************************************************************************

                g r d _ D r a p e P o i n t s O n T r i M e s h

 *********************************************************************************

  Return x,y and z arrays for the draping of the specified points on the specified
  triangular mesh.  The output points are allocated by this function but ownership
  is relinquished back to the calling function, which must csw_Free them when needed.
  Each output array is allocated separately and must be csw_Freed separately.

*/

int CSWGrdAPI::grd_DrapePointsOnTriMesh (int id1, int id2,
                             NOdeStruct *nodes, int numnodes,
                             EDgeStruct *edges, int numedges,
                             TRiangleStruct *triangles, int numtriangles,
                             double *xpts, double *ypts, int npts,
                             double **xout, double **yout, double **zout,
                             int *nout)

{
    int             istat;

    istat = grd_triangle_obj.grd_drape_points_on_tri_mesh (id1, id2,
                                         nodes, numnodes,
                                         edges, numedges,
                                         triangles, numtriangles,
                                         xpts, ypts, npts,
                                         xout, yout, zout,
                                         nout);
    return istat;

}  /* end of function grd_DrapePointsOnTriMesh */



/*
 ***********************************************************************************

                       g r d _ R e m o v e T r i I n d e x

 ***********************************************************************************

  Remove the cached triangle index that matches the specified identifiers.  On
  success 1 is returned.  If no such identifers exist in the cache, zero is returned.

*/

int CSWGrdAPI::grd_RemoveTriIndex (int id1, int id2)
{
    int            istat;

    istat = grd_triangle_obj.grd_remove_tri_index (id1, id2);
    return istat;

}



/*
 ***********************************************************************************

                     g r d _ F r e e T r i I n d e x L i s t

 ***********************************************************************************

  Free all the index grids in the current cache.

*/

void CSWGrdAPI::grd_FreeTriIndexList (void)
{
    grd_triangle_obj.grd_free_tri_index_list ();
    return;
}




/*-------------------------------------------------------------------------------*/

int CSWGrdAPI::grd_ResampleXYZLine (
           double           *x,
           double           *y,
           double           *z,
           int              npts,
           double           avspace,
           double           *xout,
           double           *yout,
           double           *zout,
           int              *nout,
           int              maxout)

{
    double          *xwork, *ywork, *zwork;
    int             nwork, istat;

    xwork = ywork = zwork = NULL;

    istat = gpf_resample_line (
        x, y, z, npts,
        avspace,
        &xwork, &ywork, &zwork, &nwork);
    if (istat == -1  ||  xwork == NULL  ||  ywork == NULL  ||  zwork == NULL) {
        return -1;
    }

    if (nwork < 2) {
        csw_Free (xwork);
        return -1;
    }

    if (nwork > maxout) {
        csw_Free (xwork);
        return -1;
    }

    memcpy (xout, xwork, nwork * sizeof(double));
    memcpy (yout, ywork, nwork * sizeof(double));
    memcpy (zout, zwork, nwork * sizeof(double));
    *nout = nwork;

    csw_Free (xwork);

    return 1;
}

/*-------------------------------------------------------------------------------*/



/*
  ****************************************************************

    g r d _ S e t C o n f o r m a b l e S u r f a c e F r o m D o u b l e

  ****************************************************************

  function name:        grd_SetConformableSurfaceFromDouble         (int)

  call sequence:        grd_SetConformableSurfaceFromDouble
                                              (grid, ncol, nrow,
                                               x1, y1, x2, y2)

  purpose:              Set or unset the grid surface that will be used to
                        control the shape of the next grid calculated with
                        grd_CalcGridFromDouble.  Call this with grid set
                        to NULL to not use any shape grid.

  return value:         Always returns 1.

  calling parameters:

    grid     r    CSW_F*        Grid array.
    ncol     r    int           Number of columns in the grid.
    nrow     r    int           Number of rows in the grid.
    x1       r    double        Minimum x of grid.
    y1       r    double        Minimum y of grid.
    x2       r    double        Maximum x of grid.
    y2       r    double        Maximum y of grid.

*/

int CSWGrdAPI::grd_SetConformableSurfaceFromDouble
                          (CSW_F *grid, int ncol, int nrow,
                           double x1, double y1, double x2, double y2)

{

    int   istat = grd_calc_obj.grd_set_conformable_surface_from_double
      (grid, ncol, nrow,
       x1, y1, x2, y2);
    return istat;

}  /*  end of function grd_SetConformableSurfaceFromDouble  */



/*
 ************************************************************************

              g r d _ C l o s e T o S a m e L i n e X Y

 ************************************************************************

  Return 1 if the average distance of the points in line 2 (xa2, ya2)
  from line 1 (xa1, ya1) is less than or equal to dcrit.  Only points
  from line 2 that can be projected perpendicularly to segments in line
  1 are used for the average distance calculation.

  Return zero if the average distance is greater than dcrit.

*/

int CSWGrdAPI::grd_CloseToSameLineXY (
    double     *xa1,
    double     *ya1,
    int        npts1,
    double     *xa2,
    double     *ya2,
    int        npts2,
    double     dcrit)
{
    double     dist, dsum;
    int        istat, i, nsum;

    if (xa1 == NULL  ||  ya1 == NULL  ||
        npts1 < 2  ||
        xa2 == NULL  ||  ya2 == NULL  ||
        npts2 < 1  ||  dcrit < 0.0) {
        assert (0);
    }

    nsum = 0;
    dsum = 0.0;
    for (i=0; i<npts2; i++) {
        istat =
        gpf_calclinedist2xy (
            xa1, ya1, npts1,
            xa2[i], ya2[i], &dist);
        if (istat == 0) {
            dsum += dist;
            nsum++;
        }
    }

    if (nsum < 1) {
        return 0;
    }

    dist = dsum / nsum;

    if (dist <= dcrit) {
        return 1;
    }

    return 0;

}


/*
 **********************************************************************************

                    g r d _ C h e w U p T r i a n g l e s

 **********************************************************************************

  Delete triangles topologically connected to the start node until blocked
  by constraint edges.  This can be used to trim trimeshes to closed boundaries
  that have been set as exact constraints in the trimesh.

*/

int CSWGrdAPI::grd_ChewUpTriangles (
    int            startnode,
    NOdeStruct     *nodes,
    int            *numnodes,
    EDgeStruct     *edges,
    int            *numedges,
    TRiangleStruct *tris,
    int            *numtris)

{
    int            istat;

    istat = grd_triangle_obj.grd_chew_up_triangles (
        startnode,
        nodes, numnodes,
        edges, numedges,
        tris, numtris);

    return istat;
}



/*
 **************************************************************************************

           g r d _ R e m o v e D e l e t e d T r i M e s h E l e m e n t s

 **************************************************************************************

*/

int CSWGrdAPI::grd_RemoveDeletedTriMeshElements (
    NOdeStruct      *nodes,
    int             *num_nodes,
    EDgeStruct      *edges,
    int             *num_edges,
    TRiangleStruct  *tris,
    int             *num_tris)
{
    int             istat;

    istat =
      grd_triangle_obj.grd_remove_deleted_trimesh_elements (
        nodes, num_nodes,
        edges, num_edges,
        tris, num_tris);
    return istat;
}


/*
 **************************************************************************************

                      g r d _ R e s h a p e T r i a n g l e s

 **************************************************************************************

*/

int CSWGrdAPI::grd_ReshapeTriangles (
    NOdeStruct      *nodes,
    int             *num_nodes,
    EDgeStruct      *edges,
    int             *num_edges,
    TRiangleStruct  *tris,
    int             *num_tris)
{
    int             istat;

    istat =
      grd_triangle_obj.grd_reshape_triangles (
        nodes, num_nodes,
        edges, num_edges,
        tris, num_tris);
    return istat;
}


/*
 *********************************************************************************

                   g r d _ A d d L i n e T o F i l e

 *********************************************************************************

  Write a set of x, y and (optional) z coordinates to a file as individual
  polylines.

*/

int CSWGrdAPI::grd_StartLineFile (char *name)
{

    LineFilePtr = fopen (name, "wb");
    if (LineFilePtr == NULL) {
        return -1;
    }
    return 1;
}

void CSWGrdAPI::grd_EndLineFile (void)
{
    if (LineFilePtr) fclose (LineFilePtr);
    LineFilePtr = NULL;
}

int CSWGrdAPI::grd_AddLineToFile (double *x,
                   double *y,
                   double *z,
                   int    np)
{
    FILE         *fptr;
    int          k;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||  np < 1) {
        return -1;
    }

    fptr = LineFilePtr;
    if (fptr == NULL) {
        return -1;
    }

    sprintf (c200, "%d\n", np);
    fputs (c200, fptr);
    for (k=0; k<np; k++) {
        if (z != NULL) {
            sprintf (c200, "%g %g %g\n", x[k], y[k], z[k]);
            fputs (c200, fptr);
        }
        else {
            sprintf (c200, "%g %g 0.0\n", x[k], y[k]);
            fputs (c200, fptr);
        }
    }

    return 1;

}





/*
  ****************************************************************

                g r d _ W r i t e T e x t F i l e

  ****************************************************************

*/

int CSWGrdAPI::grd_WriteTextFile
                   (CSW_F *grid, int ncol, int nrow,
                   double xmin, double ymin, double xmax, double ymax,
                   char *filename)

{
    int  istat =
    grd_fileio_obj.grd_write_text_file
                   (grid, ncol, nrow,
                   xmin, ymin, xmax, ymax,
                   filename);

    return istat;

}  /*  end of grd_WriteTextFile function  */


/*
 ************************************************************************

 ************************************************************************
*/
int CSWGrdAPI::grd_ResetTriMeshZValuesFromGrid (
    NOdeStruct     *nodes,
    int            num_nodes,
    EDgeStruct     *edges,
    int            num_edges,
    TRiangleStruct *tris,
    int            num_tris,
    CSW_F          *grid,
    int            ncol,
    int            nrow,
    double         gxmin,
    double         gymin,
    double         gxmax,
    double         gymax,
    void           *faults,
    int            nfaults,
    int            interp_flag,
    int            *lineids
    )
{
    int            istat;

    istat =
      grd_triangle_obj.grd_reset_trimesh_zvalues_from_grid (
        nodes,
        num_nodes,
        edges,
        num_edges,
        tris,
        num_tris,
        grid,
        ncol,
        nrow,
        gxmin,
        gymin,
        gxmax,
        gymax,
        (FAultLineStruct *)faults,
        nfaults,
        interp_flag,
        lineids
        );

    return istat;

}



/*
 *********************************************************************************

                   g r d _ W r i t e F l o a t L i n e s

 *********************************************************************************

  Write a set of x, y and (optional) z coordinates to a file as individual
  polylines.

*/

int CSWGrdAPI::grd_WriteFloatLines (
                   CSW_F  *x,
                   CSW_F  *y,
                   CSW_F  *z,
                   int    np,
                   int    *nc,
                   int    *nv,
                   const char *fname)
{
    FILE         *fptr;
    int          i, j, k, n1, n2;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||
        nc == NULL  ||  nv == NULL  ||
        np < 1  ||  fname == NULL) {
        return -1;
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    n1 = 0;
    for (i=0; i<np; i++) {
        n1 += nc[i];
    }
    sprintf (c200, "%d\n", n1);
    fputs (c200, fptr);

    n1 = 0;
    n2 = 0;
    for (i=0; i<np; i++) {
        for (j=0; j<nc[i]; j++) {
            sprintf (c200, "%d\n", nv[n1]);
            fputs (c200, fptr);
            for (k=0; k<nv[n1]; k++) {
                if (z != NULL) {
                    sprintf (c200, "%g %g %g\n", x[n2], y[n2], z[n2]);
                    fputs (c200, fptr);
                }
                else {
                    sprintf (c200, "%g %g 0.0\n", x[n2], y[n2]);
                    fputs (c200, fptr);
                }
                n2++;
            }
            n1++;
        }
    }

    fclose (fptr);

    return 1;

}


int CSWGrdAPI::grd_WriteFaultLines (
    FAultLineStruct   *faults,
    int               nfaults,
    const char        *fname)
{
    CSW_F     *xlines = NULL, *ylines = NULL, *zlines = NULL;
    int       i, *icomp = NULL, *ivec = NULL, *itype = NULL, nlines, istat;


    auto fscope = [&]()
    {
        csw_Free (xlines);
        csw_Free (ivec);
        csw_Free (icomp);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (faults == NULL  ||  nfaults < 1) {
        return -1;
    }

    istat = grd_FaultStructsToArrays (
        faults, nfaults,
        &xlines, &ylines, &zlines,
        &ivec, &itype, &nlines);
    if (istat != 1) {
        return -1;
    }

    icomp = (int *)csw_Malloc (nlines * sizeof(int));
    if (icomp == NULL) {
        return -1;
    }

    for (i=0; i<nlines; i++) {
        icomp[i] = 1;
    }

    grd_WriteFloatLines (
        xlines, ylines, zlines,
        nlines, icomp, ivec,
        fname);

    return 1;
}




/*
 ********************************************************************************

                 g r d _ S m o o t h T r i M e s h N o d e s

 ********************************************************************************

  Smooth the nodes in the trimesh by averaging the node with its nearest
  connected neighbors.

*/

int CSWGrdAPI::grd_SmoothTriMeshNodes (
    NOdeStruct     *nodes,
    int            numnodes,
    EDgeStruct     *edges,
    int            numedges,
    TRiangleStruct *tris,
    int            numtris,
    int            sfact,
    double         avlen)

{
    int            istat;

    istat =
      grd_triangle_obj.grd_smooth_trimesh_nodes (
        nodes,
        numnodes,
        edges,
        numedges,
        tris,
        numtris,
        sfact,
        avlen);

    return istat;

}




/*
 *********************************************************************************

                     g r d _ W r i t e F l a g g e d L i n e s

 *********************************************************************************

  Write a set of x, y and (optional) z coordinates to a file as individual
  polylines.

*/

int CSWGrdAPI::grd_WriteFlaggedLines (
                   double *x,
                   double *y,
                   double *z,
                   int    np,
                   int    *nc,
                   int    *nv,
                   int    *flags,
                   char *fname)
{
    FILE         *fptr;
    int          i, j, k, n1, n2, flag;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||
        nc == NULL  ||  nv == NULL  ||
        np < 1  ||  fname == NULL) {
        return -1;
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    n1 = 0;
    for (i=0; i<np; i++) {
        n1 += nc[i];
    }
    sprintf (c200, "%d\n", n1);
    fputs (c200, fptr);

    n1 = 0;
    n2 = 0;
    for (i=0; i<np; i++) {
        for (j=0; j<nc[i]; j++) {
            flag = 0;
            if (flags != NULL) {
                flag = flags[n1];
            }
            sprintf (c200, "%d %d\n", nv[n1], flag);
            fputs (c200, fptr);
            for (k=0; k<nv[n1]; k++) {
                if (z != NULL) {
                    sprintf (c200, "%g %g %g\n", x[n2], y[n2], z[n2]);
                    fputs (c200, fptr);
                }
                else {
                    sprintf (c200, "%g %g 0.0\n", x[n2], y[n2]);
                    fputs (c200, fptr);
                }
                n2++;
            }
            n1++;
        }
    }

    fclose (fptr);

    return 1;

}
