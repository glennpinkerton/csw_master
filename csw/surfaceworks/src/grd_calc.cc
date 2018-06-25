
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_calc.c

      This file has functions used by various grid api functions.  The
    API functions are wrappers which do some special error checking and
    then call functions in this file to do the actual work.

*/

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/simulP.h"

#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/gpf_calcdraw.h"
#include "csw/utils/include/csw_.h"

#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_triangle_class.h"
#include "csw/surfaceworks/private_include/grd_utils.h"
#include "csw/surfaceworks/private_include/grd_calc.h"



/*
  ****************************************************************

                g r d _ s e t _ c a l c _ o p t i o n

  ****************************************************************

  function name:    grd_set_calc_option               (int)

  call sequence:    grd_set_calc_option (tag, ival, fval)

  purpose:          Set an option prior to calling grd_calc_grid.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           2 = unknown tag

  calling parameters:

        tag        r      int          Identifier tag for the option.
                                       These are listed in csw/surfaceworks/include/grid_api.h.
        ival       r      int          Value for an int parameter.
        fval       r      CSW_F        Value for a CSW_F parameter.

*/

int CSWGrdCalc::grd_set_calc_option
        (int tag, int ival, CSW_F fval)

{

    if (tag == GRD_DEFAULT_OPTIONS) {
        OptPreferredStrike = -1000.0f;
        OptOutsideBoundaryMargin = 3;
        OptInsideBoundaryMargin = 100;
        OptFastGridFlag = 0;
        OptReportLines = 20;
        OptGridMinValue = -1.e30f;
        OptGridMaxValue = 1.e30f;
        OptHardMinValue = -1.e30f;
        OptHardMaxValue = 1.e30f;
        OptAnisotropyFlag = 1;
        OptDistancePower = 2;
        OptStrikePower = 6;
        OptNumLocalPoints = MAX_LOCAL;
        OptLocalSearchPattern = GRD_QUADRANT_SEARCH;
        OptMaxSearchDistance = 1.e30f;
        OptThicknessFlag = 0;
        OptStepGridFlag = 0;
        OptWorkMargin = -1;
        OptFaultedFlag = 0;
        OptDefaultSizeMultiplier = 1;
        DefaultSizeMultiplier = 1;
        OptTriangulateFlag = 0;
    }

    else if (tag == GRD_PREFERRED_STRIKE) {
        if (ival < 0) ival += 360;
        if (ival > 360) ival -= 360;;
        OptPreferredStrike = (CSW_F)ival;
        if (OptPreferredStrike < 0.0f) OptPreferredStrike += 360.0f;
        if (OptPreferredStrike < 0.f  ||  OptPreferredStrike > 360.f) {
            OptPreferredStrike = -1000.0f;
        }
    }

    else if (tag == GRD_DEFAULT_SIZE_MULTIPLIER) {
        if (ival < 1) ival = 1;
        if (ival > 10) ival = 10;
        OptDefaultSizeMultiplier = ival;
    }

    else if (tag == GRD_OUTSIDE_MARGIN) {
        if (ival < 1) ival = 1;
        OptOutsideBoundaryMargin = ival;
    }

    else if (tag == GRD_INSIDE_MARGIN) {
        if (ival < 2) ival = 2;
        OptInsideBoundaryMargin = ival;
    }

    else if (tag == GRD_FAST_FLAG) {
        if (ival != 1) ival = 0;
        OptFastGridFlag = ival;
    }

    else if (tag == GRD_REPORT_LINES) {
        if (ival < 1) ival = 1;
        if (ival > MAX_REPORT_LINES) ival = MAX_REPORT_LINES;
        OptReportLines = ival;
    }

    else if (tag == GRD_ANISOTROPY_FLAG) {
        if (ival < 1  ||  ival > 2) ival = 0;
        OptAnisotropyFlag = ival;
    }

    else if (tag == GRD_DISTANCE_POWER) {
        if (ival > 6) ival = 6;
        if (ival == 3) ival = 2;
        if (ival == 5) ival = 4;
        OptDistancePower = ival;
    }

    else if (tag == GRD_STRIKE_POWER) {
        if (ival < 2) ival = 2;
        if (ival > 6) ival = 6;
        if (ival == 3) ival = 2;
        if (ival == 5) ival = 4;
        OptStrikePower = ival;
    }

    else if (tag == GRD_NUM_LOCAL_POINTS) {
        if (ival < 1  ||  ival > MAX_LOCAL) ival = MAX_LOCAL;
        OptNumLocalPoints = ival;
    }

    else if (tag == GRD_LOCAL_SEARCH_PATTERN) {
        if (ival < 1  ||  ival > 3) ival = 1;
        OptLocalSearchPattern = ival;
    }

    else if (tag == GRD_MAX_SEARCH_DISTANCE) {
        OptMaxSearchDistance = fval;
    }

    else if (tag == GRD_THICKNESS_FLAG) {
        OptThicknessFlag = ival;
    }

    else if (tag == GRD_STEP_GRID_FLAG) {
        OptStepGridFlag = ival;
    }

    else if (tag == GRD_FAULTED_GRID_FLAG) {
        OptFaultedFlag = ival;
    }

    else if (tag == GRD_TRIANGULATE_FLAG) {
        OptTriangulateFlag = ival;
    }

    else if (tag == GRD_WORK_MARGIN) {
        OptWorkMargin = ival;
    }

    else {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    return 1;

}  /*  end of function grd_set_calc_option  */





/*
  ****************************************************************

             g r d _ s e t _ c a l c _ o p t i o n s

  ****************************************************************

    Set all options based on the specified GRidCalcOptions pointer.
  Each member of the structure is passed to grd_set_calc_option
  using the appropriate tag.

*/

int CSWGrdCalc::grd_set_calc_options (GRidCalcOptions *options)
{

    if (options == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    grd_set_calc_option (GRD_PREFERRED_STRIKE, options->preferred_strike, 0.0f);
    grd_set_calc_option (GRD_OUTSIDE_MARGIN, options->outside_margin, 0.0f);
    grd_set_calc_option (GRD_INSIDE_MARGIN, options->inside_margin, 0.0f);
    grd_set_calc_option (GRD_FAST_FLAG, options->fast_flag, 0.0f);
    grd_set_calc_option (GRD_MIN_VALUE, 0, options->min_value);
    grd_set_calc_option (GRD_MAX_VALUE, 0, options->max_value);
    grd_set_calc_option (GRD_REPORT_LINES, options->report_lines, 0.0f);
    grd_set_calc_option (GRD_ANISOTROPY_FLAG, options->anisotropy_flag, 0.0f);
    grd_set_calc_option (GRD_DISTANCE_POWER, options->distance_power, 0.0f);
    grd_set_calc_option (GRD_STRIKE_POWER, options->strike_power, 0.0f);
    grd_set_calc_option (GRD_NUM_LOCAL_POINTS, options->num_local_points, 0.0f);
    grd_set_calc_option (GRD_LOCAL_SEARCH_PATTERN, options->local_search_pattern, 0.0f);
    grd_set_calc_option (GRD_MAX_SEARCH_DISTANCE, 0, options->max_search_distance);
    grd_set_calc_option (GRD_THICKNESS_FLAG, options->thickness_flag, 0.0f);
    grd_set_calc_option (GRD_FAULTED_GRID_FLAG, options->faulted_flag, 0.0f);
    grd_set_calc_option (GRD_TRIANGULATE_FLAG, options->triangulate_flag, 0.0f);
    grd_set_calc_option (GRD_WORK_MARGIN, options->work_margin, 0.0f);

    return 1;

}  /*  end of function grd_set_calc_options  */



/*
  ****************************************************************

       g r d _ d e f a u l t _ c a l c _ o p t i o n s

  ****************************************************************

    Set options in the specified structure to their default values.

*/

int CSWGrdCalc::grd_default_calc_options (GRidCalcOptions *options)
{

    if (options == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    options->preferred_strike = -1000;
    options->outside_margin = 3;
    options->inside_margin = 10;
    options->fast_flag = 0;
    options->report_lines = 20;
    options->min_value = -1.e30f;
    options->max_value = 1.e30f;
    options->hard_min = -1.e30f;
    options->hard_max = 1.e30f;
    options->anisotropy_flag = 1;
    options->distance_power = 0;
    options->strike_power = 6;
    options->num_local_points = MAX_LOCAL;
    options->local_search_pattern = GRD_QUADRANT_SEARCH;
    options->max_search_distance = 1.e30f;
    options->thickness_flag = 0;
    options->step_flag = 0;
    options->faulted_flag = 0;
    options->triangulate_flag = 0;
    options->work_margin = -1;

    options->error_number = 0;

    return 1;

}  /*  end of function grd_default_calc_options  */





/*
  ****************************************************************

            g r d _ s e t _ c o n t r o l _ p o i n t s

  ****************************************************************

  function name:      grd_set_control_points          (int)

  call sequence:      grd_set_controlPoints (list, nlist)

  purpose:            Set up control points for subsequent grid
                      calculations.  Control points specify that
                      the grid must be less than a value, greater than
                      a value, or between two values at a location.
                      They do not say exactly what the value should be
                      at the location.  The type member of the control
                      point structure specifies the greater than, less
                      than or between behavior. (1 = greater than,
                      2 = less than, 3 = between).

  return value:       1 on success
                      -1 on error

  errors:             1 = memory allocation failure

  calling parameters:

    list    r    GRidControlPoint*     Array of control point structures.
    nlist   r    int                   Number of structure.

        note:  To disable the control point usage, set list to NULL or
               set nlist to zero.

*/

int CSWGrdCalc::grd_set_control_points (GRidControlPoint *list, int nlist)
{
    int                     i, n;

    if (ControlPoints != NULL) {
        csw_Free (ControlPoints);
        ControlPoints = NULL;
        NumControlPoints = 0;
    }

    if (list == NULL  ||  nlist == 0) {
        return 1;
    }

    ControlPoints = (GRidControlPoint *)csw_Calloc (nlist * sizeof(GRidControlPoint));
    if (ControlPoints == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    n = 0;
    for (i=0; i<nlist; i++) {
        if (list[i].type < GRD_CONTROL_GREATER  ||
            list[i].type > GRD_CONTROL_BETWEEN) {
            continue;
        }
        if (list[i].type == GRD_CONTROL_BETWEEN) {
            if (list[i].z1 >= list[i].z2) {
                continue;
            }
        }
        memcpy ((char *)(ControlPoints+n), (char *)(list+i), sizeof(GRidControlPoint));
        n++;
    }

    if (n == 0) {
        csw_Free (ControlPoints);
        ControlPoints = NULL;
        NumControlPoints = 0;
    }

    NumControlPoints = n;
    return 1;

}  /*  end of function grd_set_control_points  */





/*
  ****************************************************************************

                g r d _ s e t _ c o n t r o l _ s u r f a c e

  ****************************************************************************

    Set up or unset a control surface for subsequent grid calculations.

*/

int CSWGrdCalc::grd_set_control_surface
                    (CSW_F *grid, int ncol, int nrow,
                     CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2, int type)
{
    bool csuccess = false;
    bool bsuccess = false;
    bool tsuccess = false;

    auto fscope = [&]()
    {
        if (csuccess == false) {
            csw_Free (ConformalGrid.grid);
            ConformalGrid.grid = NULL;
        }
        if (bsuccess == false) {
            csw_Free (BaselapGrid.grid);
            BaselapGrid.grid = NULL;
        }
        if (tsuccess == false) {
            csw_Free (TruncationGrid.grid);
            TruncationGrid.grid = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (grid != NULL) {
        if (x1 >= x2  ||  y1 >= y2) {
            grd_utils_ptr->grd_set_err (3);
            return -1;
        }
    }

    if (type == GRD_CONFORMABLE) {

        if (grid == NULL) {
            ConformalFlag = 0;
            return 1;
        }
        if (ConformalGrid.grid) {
            csw_Free (ConformalGrid.grid);
            ConformalGrid.grid = NULL;
        }
        ConformalGrid.grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (ConformalGrid.grid == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        memcpy (ConformalGrid.grid, grid, ncol*nrow*sizeof(CSW_F));
        ConformalGrid.ncol = ncol;
        ConformalGrid.nrow = nrow;
        ConformalGrid.x1 = x1;
        ConformalGrid.y1 = y1;
        ConformalGrid.x2 = x2;
        ConformalGrid.y2 = y2;
        ConformalFlag = 1;

        csuccess = true;

        return 1;

    }

    if (type == GRD_BASELAP) {

        if (grid == NULL) {
            BaselapFlag = 0;
            return 1;
        }
        if (BaselapGrid.grid) {
            csw_Free (BaselapGrid.grid);
            BaselapGrid.grid = NULL;
        }
        BaselapGrid.grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (BaselapGrid.grid == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        memcpy (BaselapGrid.grid, grid, ncol*nrow*sizeof(CSW_F));
        BaselapGrid.ncol = ncol;
        BaselapGrid.nrow = nrow;
        BaselapGrid.x1 = x1;
        BaselapGrid.y1 = y1;
        BaselapGrid.x2 = x2;
        BaselapGrid.y2 = y2;
        BaselapFlag = 1;

        bsuccess = true;

        return 1;

    }

    if (type == GRD_TRUNCATION) {

        if (grid == NULL) {
            TruncationFlag = 0;
            return 1;
        }
        if (TruncationGrid.grid) {
            csw_Free (TruncationGrid.grid);
            TruncationGrid.grid = NULL;
        }
        TruncationGrid.grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (TruncationGrid.grid == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        memcpy (TruncationGrid.grid, grid, ncol*nrow*sizeof(CSW_F));
        TruncationGrid.ncol = ncol;
        TruncationGrid.nrow = nrow;
        TruncationGrid.x1 = x1;
        TruncationGrid.y1 = y1;
        TruncationGrid.x2 = x2;
        TruncationGrid.y2 = y2;
        TruncationFlag = 1;

        tsuccess = true;

        return 1;

    }

    grd_utils_ptr->grd_set_err (2);

    return -1;

}  /*  end of function grd_set_control_surface  */






/*
  ****************************************************************

                     g r d _ c a l c _ g r i d

  ****************************************************************

  function name:    grd_calc_grid              (int)

  call sequence:    grd_calc_grid (x, y, z, err, npts,
                                   grid, mask, report, ncol, nrow,
                                   x1, y1, x2, y2, options)

  purpose:          Calculate a uniform grid by interpolating the
                    specified data points.  The errors between input
                    points and the grid's estimate can be returned
                    in the error array.  A mask specifying which
                    grid points are outside the data area or
                    inside but far from control points can also
                    be returned in the mask array.  This function only
                    supports non faulted continuous surfaces.
-
                    The grid must have at least 8 columns and 8 rows.
                    The upper limit of grid size is controlled by available
                    virtual memory.  The algorithm needs workspace about 5
                    times the size of the grid, plus workspace about twice
                    the size of the data array.  For example, calculating
                    a 1000 by 1000 grid from 100000 points would need about
                    43 to 44 megabytes of work space.  Memory allocation errors
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
                    6 = npts less than 1
                    7 = number of points actually in the data area
                        is less than 1
                    8 = The points are distributed in a manner that
                        cannot be fit with a trend surface (probably
                        all points are coincident or colinear).
                    9 = One of the x, y or z points is bad (less than
                        -1.e20 or greater than 1.e20).


  calling parameters:

    x        r    CSW_F*    Array of x coordinates
    y        r    CSW_F*    Array of y coordinates
    z        r    CSW_F*    Array of z values at the x,y coordinates
    err      w    CSW_F*    Optional array to receive errors at the
                            input points.  Set to NULL if not wanted.
    npts     r    int       Number of points in x, y, z and error.
    in_grid  w    CSW_F*    Array to receive the grid values.
    mask     w    char*     Optional array to receive a mask specifying
                            whether a grid node is outside the data
                            area, inside and well defined, or inside
                            and poorly defined.  A well defined node
                            has a mask value of zero.  A node outside
                            the data area has a mask value of 1.  A
                            poorly defined inside node has a mask
                            value of 2.  Specify a NULL pointer to
                            disable this feature.
    report   w    char**    If this is not NULL, a report is written
                            into space allocated by the funtion.
    ncol     r    int       Number of columns in the grid and mask.
    nrow     r    int       Number of rows in the grid and mask.
    x1       r    CSW_F     Minimum x coordinate of the grid.
    y1       r    CSW_F     Minimum y coordinate of the grid.
    x2       r    CSW_F     Maximum x coordinate of the grid.
    y2       r    CSW_F     Maximum y coordinate of the grid.
    options  r    GRidCalcOptions
                            Optional grid calc option structure.

*/

int CSWGrdCalc::grd_calc_grid
    (CSW_F *x, CSW_F *y, CSW_F *z, CSW_F *err, int npts,
     CSW_F *in_grid, char *in_mask, char **report, int ncol, int nrow,
     CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
     GRidCalcOptions *options)
{
    int       n, i, j, k, k2, istat, ismooth, nbins, offset;
    CSW_F     savsmooth, datax1, datay1, datax2, datay2, margin1;
    int       orig_ncol, orig_nrow, ncolr, nrowr, idenom, resampleflag;
    int       dpsav, fault_resampleflag, pre_fault_ncol, pre_fault_nrow;
    CSW_F     x1r, y1r, x2r, y2r,
              *newgrid = NULL, *local_grid = NULL, *grid = NULL;
    char      *newmask = NULL, *local_mask = NULL, *mask = NULL;
    CSW_F     tiny, ftmp, xt1, yt1, xt2, yt2, xyratio;
    CSW_F     xmarg, ymarg, orig_min_val, orig_max_val;
    FAultLineStruct   *faults = NULL;
    int       nfaults, fistat;
    int       recalc_grid_flag, erflag;
    double    node_ratio, smooth_mult;
    int       num_pre_pass1;
    int       do_write;
    char      fname[200];


    auto fscope = [&]()
    {
        csw_Free (local_mask);
        csw_Free (local_grid);
        csw_Free (newgrid);
        csw_Free (newmask);
        FreeMem ();
    };


    CSWScopeGuard func_scope_guard (fscope);



/*
    check obvious errors in input parameters
*/
    if (ncol < -WildInteger  ||  ncol > WildInteger  ||
        nrow < -WildInteger  ||  nrow > WildInteger  ||
        x1 < -WildFloat  ||  x1 > WildFloat  ||
        y1 < -WildFloat  ||  y1 > WildFloat ||
        x2 < -WildFloat  ||  x2 > WildFloat ||
        y2 < -WildFloat  ||  y2 > WildFloat) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (x == NULL  ||  y == NULL  ||  z == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (ncol < MIN_COLS_GCALC  ||  nrow < MIN_ROWS_GCALC) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    if (x2 <= x1  ||  y2 <= y1) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

    if (npts < 1) {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

/*
 * Check for bad values on x, y and z data.  This check is based on allowing
 * a map to stretch from pole to pole in millimeters, which would be 10 billion
 * millimeters and then adding a couple of orders of magnitude for good measure.
 */
    if (x1 < -1.e12  ||  x1 > 1.e12  ||
        y1 < -1.e12  ||  y1 > 1.e12 ||
        x2 < -1.e12  ||  x2 > 1.e12 ||
        y2 < -1.e12  ||  y2 > 1.e12) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    for (i=0; i<npts; i++) {
        if (x[i] < -1.e12  ||  x[i] > 1.e12  ||
            y[i] < -1.e12  ||  y[i] > 1.e12  ||
            z[i] < -1.e12  ||  z[i] > 1.e12) {
            grd_utils_ptr->grd_set_err (9);
            return -1;
        }
    }



/*
 * !!!! debug only
 */
    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        strcpy (fname, "grid_input.xyz");
        grd_fileio_ptr->grd_write_float_points (x, y, z, npts, fname);
    }


    PointNodeRatio = (double)npts / (double)(ncol * nrow);

    grid = in_grid;
    mask = in_mask;
    local_grid = NULL;
    local_mask = NULL;

/*
    Check if all the values are the same.
*/
    istat = csw_CheckRangeZero (z, npts);
    FlatGridFlag = 0;
    if (istat == 0) {
        FlatGridFlag = 1;
        StepGridFlag = 0;
        ThicknessFlag = 0;
    }

    if (npts < 2) {
        FlatGridFlag = 1;
    }

/*
    If there are control points, reset the flat grid flag to zero
    if any of the control point limits are violated by the flat
    grid value.
*/
    if (NumControlPoints > 0  &&  FlatGridFlag == 1) {
        istat = CheckFlatControl (x, y, z, npts);
        if (istat == 0) {
            FlatGridFlag = 0;
            if (NumFlatControl < MAX_CONTROL  &&  ConformalFlag == 0) {
                x = Xcontrol;
                y = Ycontrol;
                z = Zcontrol;
                npts = NumFlatControl;
            }
        }
    }

/*
    If there is a conformal shaping surface, set the flat flag to zero.
*/
    if (ConformalFlag) FlatGridFlag = 0;

/*
    Calculate the number of bins to use for
    global anisotropy.
*/
    nbins = (ncol + nrow) / 4;
    if (nbins < 20) nbins = 20;
    if (nbins > 500) nbins = 500;

/*
    Transfer either the global or structure options to the working
    option set.
*/
    if (options == NULL) {
        PreferredStrike = OptPreferredStrike;
        OutsideBoundaryMargin = OptOutsideBoundaryMargin;
        InsideBoundaryMargin = OptInsideBoundaryMargin;
        ReportLines = OptReportLines;
        GridMinValue = OptGridMinValue;
        GridMaxValue = OptGridMaxValue;
        HardMinValue = OptHardMinValue;
        HardMaxValue = OptHardMaxValue;
        AnisotropyFlag = OptAnisotropyFlag;
        DistancePower = OptDistancePower;
        if (DistancePower == 0) DistancePower = 2;
        StrikePower = OptStrikePower;
        NumLocalPoints = OptNumLocalPoints;
        LocalSearchPattern = OptLocalSearchPattern;
        MaxSearchDistance = OptMaxSearchDistance;
        FastGridFlag = OptFastGridFlag;
        ThicknessFlag = OptThicknessFlag;
        StepGridFlag = OptStepGridFlag;
        WorkMargin = OptWorkMargin;
        FaultedFlag = OptFaultedFlag;
        TriangulateFlag = OptTriangulateFlag;
        DefaultSizeMultiplier = OptDefaultSizeMultiplier;
        MovingAvgOnly = 0;
    }

    else {
        PreferredStrike = (CSW_F)options->preferred_strike;
        OutsideBoundaryMargin = options->outside_margin;
        InsideBoundaryMargin = options->inside_margin;
        ReportLines = options->report_lines;

        GridMinValue = options->min_value;
        GridMaxValue = options->max_value;
        HardMinValue = options->hard_min;
        HardMaxValue = options->hard_max;
        AnisotropyFlag = options->anisotropy_flag;
        DistancePower = options->distance_power;
        if (DistancePower == 0) DistancePower = 2;
        StrikePower = options->strike_power;
        NumLocalPoints = options->num_local_points;
        LocalSearchPattern = options->local_search_pattern;
        MaxSearchDistance = options->max_search_distance;
        FastGridFlag = options->fast_flag;
        ThicknessFlag = options->thickness_flag;
        StepGridFlag = options->step_flag;
        WorkMargin = options->work_margin;
        FaultedFlag = options->faulted_flag;
        TriangulateFlag = options->triangulate_flag;;
        DefaultSizeMultiplier = 1;
        MovingAvgOnly = options->moving_avg_only;
    }

/*
 * Extract the NoTrendFlag from the TriangulateFlag value.
 */
    erflag = TriangulateFlag / 1000;
    NoTrendFlag = (TriangulateFlag % 1000) / 100;
    TriangulateFlag = TriangulateFlag % 100;
    EmptyRegionFlag = 1;
    if (erflag == 1) EmptyRegionFlag = 0;

    VerticalFaultFlag = 0;
    if (TriangulateFlag == 6) {
        TriangulateFlag = 3;
        VerticalFaultFlag = 1;
        EmptyRegionFlag = 1;
        AnisotropyFlag = 0;
        PreferredStrike = -1000.0f;
    }
    else {
        if (AnisotropyFlag == 1  ||  PreferredStrike >= 0.0) {
            TriangulateFlag = 0;
        }
    }

/*
 * If a vertical fault is being calculated, and the z values are
 * very small relative to the xy range, then the vertical fault
 * is planar and, since the z values represent distance from the
 * plane, a grid set to all zeros is returned.
 */
    if (VerticalFaultFlag == 1) {
        Zmin = 1.e30f;
        Zmax = -1.e30f;
        for (i=0; i<npts; i++) {
            if (z[i] < Zmin) Zmin = z[i];
            if (z[i] > Zmax) Zmax = z[i];
        }
        xt1 = Zmax - Zmin;
        xt2 = (x2 - x1);
        if (y2 - y1 > xt2) xt2 = y2 - y1;
        if (xt2 > 1000 * xt1) {
            memset (in_grid, 0, ncol * nrow * sizeof(CSW_F));
            if (in_mask) {
                memset (in_mask, 0, ncol * nrow * sizeof(char));
            }
            if (err) {
                memset (err, 0, npts * sizeof(CSW_F));
            }
            return 1;
        }
    }

/*
    Change any garbage option values to their defaults.
*/
    if (PreferredStrike < 0.0f) PreferredStrike += 360.0f;
    if (PreferredStrike < 0  ||  PreferredStrike > 360) PreferredStrike = -1000.0f;
    if (OutsideBoundaryMargin < 1) OutsideBoundaryMargin = 1;

  /*
   * Previously, if the inside boundary margin was less than 2, the default value
   * of 10 was used.  Now, any InsideBoundaryMargin value 1 or greater is accepted.
   * Values less than 2 will make ugly maps, but that is the application code's
   * responsibility to avoid if it chooses to do so.
   */
    if (InsideBoundaryMargin < 1) InsideBoundaryMargin = 1;

    if (ReportLines < 1) ReportLines = 20;
    if (AnisotropyFlag < 1  ||  AnisotropyFlag > 2) AnisotropyFlag = 0;
    if (DistancePower < 2) DistancePower = 2;
    if (DistancePower > 6) DistancePower = 6;
    if (DistancePower == 3) DistancePower = 2;
    if (DistancePower == 5) DistancePower = 4;
    if (StrikePower < 2) StrikePower = 2;
    if (StrikePower > 6) StrikePower = 6;
    if (StrikePower == 3) StrikePower = 4;
    if (StrikePower == 5) StrikePower = 6;
    if (NumLocalPoints < 1  ||  NumLocalPoints > MAX_LOCAL) NumLocalPoints = MAX_LOCAL;
    if (LocalSearchPattern < 1  ||  LocalSearchPattern > 3) LocalSearchPattern = 1;
    if (WorkMargin < 0) WorkMargin = -1;
    if (GridMinValue >= GridMaxValue) {
        GridMinValue = -1.e30f;
        GridMaxValue = 1.e30f;
    }
    if (GridMinValue > 1.e20f) {
        GridMinValue = -1.e30f;
    }
    if (GridMaxValue < -1.e20f) {
        GridMaxValue = 1.e30f;
    }

    if (GridMinValue < HardMinValue) GridMinValue = HardMinValue;
    if (GridMaxValue > HardMaxValue) GridMaxValue = HardMaxValue;

    orig_min_val = GridMinValue;
    orig_max_val = GridMaxValue;

    if (ThicknessFlag == GRD_POSITIVE_THICKNESS) {
        GridMinValue = -1.e30f;
    }
    if (ThicknessFlag == GRD_NEGATIVE_THICKNESS) {
        GridMaxValue = 1.e30f;
    }

/*
    Turn off preferred strike and anisotropy for fast grid flag.
*/
    if (FastGridFlag) {
        AnisotropyFlag = 0;
        PreferredStrike = -1000.0f;
    }

/*
    Reset some options if the step grid flag is set.
*/
    if (StepGridFlag) {
        AnisotropyFlag = 0;
        PreferredStrike = -1000.0f;
        MaxSearchDistance = 1.e30f;
        InsideBoundaryMargin = 100000;
        LocalSearchPattern = GRD_RADIAL_SEARCH;
        NumLocalPoints = 100;
        ThicknessFlag = 0;
    }

/*
    Turn off anisotropy if the grid is flat.
*/
    if (FlatGridFlag == 1) {
        AnisotropyFlag = 0;
        PreferredStrike = -1000.0f;
    }

/*
    If a preferred strike has been explicitly specified,
    set the anisotropy flag to zero.
*/
    if (PreferredStrike >= 0.0f) {
        AnisotropyFlag = 0;
    }

/*
    If there are 2 points, turn off anisotropy if it is on.
    Also, generate a third point in a position where a plane
    can be calculated between the 2 points.  The plane will
    represent the gradient along a line between the 2 points.
*/
    GPFCalcdraw     gpf_calcdraw_obj;
    OrigNdata = npts;
    if (npts == 2) {
        AnisotropyFlag = 0;
        PreferredStrike = -1000.0f;
        gpf_calcdraw_obj.gpf_shortperpbisector
            (x[0], y[0], x[1], y[1], &xt1, &yt1, &xt2, &yt2);
        memcpy (Xsmall, x, 2 * sizeof(CSW_F));
        memcpy (Ysmall, y, 2 * sizeof(CSW_F));
        memcpy (Zsmall, z, 2 * sizeof(CSW_F));
        Xsmall[2] = xt2;
        Ysmall[2] = yt2;
        Zsmall[2] = (Zsmall[0] + Zsmall[1]) / 2.0f;
        npts = 3;
        x = Xsmall;
        y = Ysmall;
        z = Zsmall;
    }

/*
    Save some of the parameters in private class variables.
*/
    OrigXmin = Xmin = x1;
    OrigYmin = Ymin = y1;
    OrigXmax = Xmax = x2;
    OrigYmax = Ymax = y2;
    ReportNcol = ncol;
    ReportNrow = nrow;

/*
    Find the "optimum" number of rows and columns given
    the data distribution.  If the requested number is
    far different, use the optimal and then, at the end
    of the grid calculation, resample to the requested
    geometry.
*/
    x1r = x1;
    y1r = y1;
    x2r = x2;
    y2r = y2;

    pre_fault_nrow = nrow;
    pre_fault_ncol = ncol;
    fault_resampleflag = 0;
    if (FaultedFlag) {
        while (ncol < 4  ||  nrow < 4) {
            ncol *= 2;
            nrow *= 2;
            fault_resampleflag = 1;
        }
    }
    orig_nrow = nrow;
    orig_ncol = ncol;

    if (fault_resampleflag == 1) {
        local_grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (local_grid == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        if (mask) {
            local_mask = (char *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
            if (local_mask == NULL) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }
        grid = local_grid;
        mask = local_mask;
    }

    ncolr = ncol;
    nrowr = nrow;

    resampleflag = 0;
    newgrid = NULL;
    newmask = NULL;
    idenom = ncol * nrow / 100000;
    if (idenom < 4)
        idenom = 4;
    if (idenom > 7)
        idenom = 7;

    recalc_grid_flag = 0;
    if (DefaultSizeMultiplier < 1) DefaultSizeMultiplier = 1;
    if (StepGridFlag == 0  &&  FlatGridFlag == 0) {
        istat = CheckForNodePoints (x, y, npts,
                                    (double)x1r, (double)y1r,
                                    (double)x2r, (double)y2r,
                                    ncolr, nrowr);
        if (istat == 0) {
            grd_utils_ptr->grd_recommended_size (x, y, npts, 0,
                                  &x1r, &y1r, &x2r, &y2r,
                                  &ncolr, &nrowr);
            ncolr *= DefaultSizeMultiplier;
            nrowr *= DefaultSizeMultiplier;
        }
        else {
            recalc_grid_flag = 1;
        }
        if (FaultedFlag) {
            while (ncolr < 4  ||  nrowr < 4) {
                ncolr *= 2;
                nrowr *= 2;
            }
        }
        if (ConformalFlag == 1) {
            while (ncolr < ConformalGrid.ncol / 2  &&
                   nrowr < ConformalGrid.nrow / 2) {
                ncolr *= 3;
                nrowr *= 3;
                ncolr /= 2;
                nrowr /= 2;
            }
        }
        if (DistancePower == 6) {
            ncolr *= 3;
            nrowr *= 3;
            ncolr /= 2;
            nrowr /= 2;
        }
        xt1 = (x2 - x1) / (CSW_F)(ncol - 1);
        yt1 = (y2 - y1) / (CSW_F)(nrow - 1);
        if (yt1 != 0.0f) {
            xyratio = xt1 / yt1;
        }
        else if (xt1 != 0.0f) {
            xyratio = yt1 / xt1;
        }
        else {
            xyratio = 1.0f;
        }

        if (ncolr < ncol * 3 / idenom  ||  nrowr < nrow * 3 / idenom  ||
            xyratio < 0.99f  ||  xyratio > 1.01f) {
            ncol = ncolr * idenom / 3;
            nrow = nrowr * idenom / 3;
            if (xyratio >= 0.99f  &&  xyratio <= 1.01f) {
                if (ncol > orig_ncol)
                    ncol = orig_ncol;
                if (nrow > orig_nrow)
                    nrow = orig_nrow;
            }
            else {
                xt1 = (xt1 + yt1) / 2.0f;
                i = (int)((y2 - y1) / xt1);
                if (i < 2) i = 2;
                j = (int)((x2 - x1) / xt1);
                if (j < 2) j = 2;
                if (i*j > orig_ncol*orig_nrow) {
                    ftmp = (CSW_F)(orig_ncol*orig_nrow) / (CSW_F)(i*j);
                    i = (int)((CSW_F)i * ftmp);
                    j = (int)((CSW_F)j * ftmp);
                    if (i < 2) i = 2;
                    if (j < 2) j = 2;
                }
                if (ncol*nrow > orig_ncol*orig_nrow) {
                    ncol = j;
                    nrow = i;
                }
            }
MSL
            newgrid = (CSW_F *)csw_Malloc (orig_ncol * orig_nrow * sizeof(CSW_F));
            if (!newgrid) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            if (mask) {
MSL
                newmask = (char *)csw_Malloc (orig_ncol * orig_nrow * sizeof(char));
                if (!newmask) {
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
            }
            resampleflag = 1;
        }
    }

    OrigNcol = ncol;
    OrigNrow = nrow;
    Ncol = ncol;
    Nrow = nrow;
    Xspace = (x2 - x1) / (CSW_F)(ncol - 1);
    Yspace = (y2 - y1) / (CSW_F)(nrow - 1);

  /*
   * If x spacing or y spacing is
   * ridiculously large, return an error 3, for a wild parameter.
   * The cutoffs are based on .01 * spacing squared being a
   * reasonable float number.
   */
    if (Xspace > 1.e8  ||  Yspace > 1.e8)  {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }
    OrigXdata = x;
    OrigYdata = y;
    OrigZdata = z;
    OrigNdata = npts;
    Xdata = x;
    Ydata = y;

/*
 * Bug 8290,  A copy of the Z array is made, so the clipping to
 *            hard min and max will not affect the original data.
 */
    Zdata = (CSW_F *)csw_Malloc (npts * sizeof(CSW_F));
    FreeZdataFlag = 1;
    if (Zdata == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    memcpy (Zdata, z, npts * sizeof(CSW_F));

    Ndata = npts;
    MaxData = Ndata;
    Extension = 4;

    Zmin = 1.e30f;
    Zmax = -1.e30f;
    for (i=0; i<Ndata; i++) {
/*
 * Bug 8290, if the z data is outside of the hard data limits,
 *           clip it to the hard data limits.
 */
        if (Zdata[i] > HardMaxValue) Zdata[i] = HardMaxValue;
        if (Zdata[i] < HardMinValue) Zdata[i] = HardMinValue;

        if (Zdata[i] < Zmin) Zmin = Zdata[i];
        if (Zdata[i] > Zmax) Zmax = Zdata[i];
    }
    Zrange = Zmax - Zmin;
    TinySum = Zrange / 100000.0f;
    if (TinySum > 1.e-10) TinySum = 1.e-10f;

    MaxNodeDistance = 100000;
    if (MaxSearchDistance / (Xspace + Yspace) < 50000) {
        MaxNodeDistance = (int) (MaxSearchDistance / (Xspace + Yspace));
        MaxNodeDistance *= 2;
        MaxNodeDistance++;
    }
    if (MaxNodeDistance < 1) {
        MaxNodeDistance = 1;
    }

/*
    set the skip factor for the coarse grid
*/
    Ncoarse = 1;
    if (Ncol + Nrow > 12)  Ncoarse = 2;
    if (Ncol + Nrow > 70)  Ncoarse = 4;
    if (Ncol + Nrow > 192)  Ncoarse = 8;
    if (Ncol + Nrow > 384)  Ncoarse = 16;
    if (Ncol + Nrow > 1536)  Ncoarse = 32;
    if (Ncol + Nrow > 3072)  Ncoarse = 64;
    if (Ncol + Nrow > 6144)  Ncoarse = 128;
    if (Ncol + Nrow > 12288)  Ncoarse = 256;

    if (TriangulateFlag != 4  &&  Ndata > Ncol*Nrow/2) {
         Ncoarse /= 2;
    }

    if (Ncoarse < 1) Ncoarse = 1;

    if (Ndata == 3) Ncoarse = 1;

/*
    Set the smoothing factor based on the number of points
    in the input data set.
*/
    SmoothingFactor = 3.0f;
    if (npts > 2000) SmoothingFactor = 6.0f;
    if (npts > 5000) SmoothingFactor = 10.0f;
    if (npts > 10000) SmoothingFactor = 12.0f;
    if (FastGridFlag) {
        SmoothingFactor -= 2.0f;
        if (SmoothingFactor < 2) SmoothingFactor = 2.0f;
    }

/*
    If the margin size is much greater than the data limits
    use the data limits.  If less than zero, use 2 coarse
    grid cells.
*/
    grd_utils_ptr->grd_limits_xy (x, y, npts,
                   &datax1, &datay1, &datax2, &datay2);
    if (datax1 > x1) datax1 = x1;
    if (datay1 > y1) datay1 = y1;
    if (datax2 < x2) datax2 = x2;
    if (datay2 < y2) datay2 = y2;
    DXmin = datax1;
    DYmin = datay1;
    DXmax = datax2;
    DYmax = datay2;
    margin1 = x1 - datax1 + y1 - datay1 + datax2 - x2 + datay2 - y2;
    margin1 /= 2;
    margin1 /= (Xspace + Yspace);
    margin1 += 0.5f;
    if (WorkMargin > 2 * (int)margin1) {
        WorkMargin = (int)margin1;
    }
    if (WorkMargin < 0) {
        WorkMargin = 4 * Ncoarse;
        xmarg = WorkMargin * Xspace;
        ymarg = WorkMargin * Yspace;
        while (DXmin < x1 - xmarg  ||  DYmin < y1 - ymarg  ||
               DXmax > x2 + xmarg  ||  DYmax > y2 + ymarg) {
            WorkMargin *= 2;
            if (WorkMargin >= 8 * Ncoarse) {
                break;
            }
            xmarg = WorkMargin * Xspace;
            ymarg = WorkMargin * Yspace;
        }
    }
    if (WorkMargin < Ncoarse) {
        WorkMargin = Ncoarse;
    }

/*
    Expand the grid limits used internally so that an integral number of coarse
    grid cells are present in the calculated grid.  The internal grid is
    copied to the output grid at the end, with appropriate offsets being
    used in the copy operation.
*/
WorkMargin = 4 * Ncoarse;
if (WorkMargin < 16) WorkMargin = 16;
    ismooth = (int) (SmoothingFactor + .5f);
    grd_utils_ptr->grd_expand_limits (x1, y1, x2, y2, ncol, nrow, Ncoarse, WorkMargin,
                       &Xmin, &Ymin, &Xmax, &Ymax,
                       &Ncol, &Nrow, &ColZero, &RowZero);

    Nccol = Ncol / Ncoarse + 1;
    Ncrow = Nrow / Ncoarse + 1;
    if (Ncoarse == 1) {
        Nccol = Ncol;
        Ncrow = Nrow;
    }

/*
    Make sure the coarse grid is no larger than 128 rows
    or columns.
*/
    while (Nccol > 128  ||  Ncrow > 128) {
        Ncoarse *= 2;
        grd_utils_ptr->grd_expand_limits (x1, y1, x2, y2, ncol, nrow, Ncoarse, WorkMargin,
                           &Xmin, &Ymin, &Xmax, &Ymax,
                           &Ncol, &Nrow, &ColZero, &RowZero);
        Nccol = Ncol / Ncoarse + 1;
        Ncrow = Nrow / Ncoarse + 1;
    }
    NcoarseOrig = Ncoarse;

/*
    Count the number of points in the area of interest and
    return error 7 if there is less than 1.
*/
    n = 0;
    for (i=0; i<npts; i++) {
        if (x[i] < OrigXmin  ||  x[i] > OrigXmax  ||
            y[i] < OrigYmin  ||  y[i] > OrigYmax) {
            continue;
        }
        n = 1;
        break;
    }

    if (n < 1) {
        grd_utils_ptr->grd_set_err (7);
        TinySum = 0.0;
        return -1;
    }

/*
    If there are an absurdly large number of grid nodes relative
    to the number of data points, set a private class variable that will
    greatly increase the search distances used in the coarse grid
    node calculation.
*/
    Ndivide = 1;
    if (n < ncol * nrow / 10000) {
        Ndivide = 4;
    }
    else if (n < ncol * nrow / 1000) {
        Ndivide = 2;
    }

    if (StepGridFlag == 1) {
        Ndivide = 1;
    }

/*
    If the program is in parameter error simulation mode,
    return success (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        TinySum = 0.0;
        return 1;
    }

/*
    If the thickness flag is set, check for zero or negative data values.
    If none are found, do not use thickness gridding.
*/
    OriginalThicknessFlag = ThicknessFlag;
    if (ThicknessFlag == 1) {
        CheckForZeroThickness ();
    }

/*
    Create index grids for looking up fault vectors.
*/
    faults = NULL;
    nfaults = 0;
    if (FaultedFlag) {
        grd_fault_ptr->grd_set_fault_zlimits (Zmin, Zmax);
        grd_fault_ptr->grd_extend_faults_to_margins (OrigXmin, OrigYmin,
                                      OrigXmax, OrigYmax,
                                      Xmin, Ymin,
                                      Xmax, Ymax);
        grd_fault_ptr->grd_set_fault_index_extent (Ncoarse);
        istat = grd_fault_ptr->grd_build_fault_indices (NULL, Ncol, Nrow, Xmin, Ymin, Xmax, Ymax);
        grd_fault_ptr->grd_set_fault_index_extent (1);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            TinySum = 0.0;
            return -1;
        }
        istat = grd_fault_ptr->grd_get_current_fault_structs (&faults, &nfaults);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            TinySum = 0.0;
            return -1;
        }
        do_write = csw_GetDoWrite ();
        if (do_write) {
            grd_WriteFaultLines (
                faults, nfaults, (char *)"currentfaults.xyz");
        }

        istat = grd_fault_ptr->grd_set_fault_data_grazing (Xdata, Ydata, Ndata);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (11);
            TinySum = 0.0;
            return -1;
        }
        grd_fault_ptr->grd_get_closest_fault_ptr (&ClosestFault);

        OrigIdata = (int *)csw_Malloc (OrigNdata * sizeof(int));
        if (!OrigIdata) {
            TinySum = 0.0;
            return -1;
        }

        if (FreeZdataFlag == 1) {
            csw_Free (Zdata);
            Zdata = NULL;
            FreeZdataFlag = 0;
        }
        Xdata = (CSW_F *)csw_Malloc (OrigNdata * 3 * sizeof(CSW_F));
        if (!Xdata) {
            Xdata = OrigXdata;
        }
        else {
            Ydata = Xdata + OrigNdata;
            Zdata = Ydata + OrigNdata;
            Ndata = 0;
            for (i=0; i<OrigNdata; i++) {
                if (OrigXdata[i] < 1.e30) {
                    Xdata[Ndata] = OrigXdata[i];
                    Ydata[Ndata] = OrigYdata[i];
                    Zdata[Ndata] = OrigZdata[i];
                    OrigIdata[Ndata] = i;
                    Ndata++;
                }
            }
        }
    /*
     * Only do anisotropy on faulted grids if there are a lot of data points
     * relative to the number of fault vectors.
     */
        if (nfaults > 0) {
            if ((double)OrigNdata / (double)nfaults < 5.0  ||  npts < 10) {
                AnisotropyFlag = 0;
                AnisotropyRatio = 1.0f;
                PreferredStrike = -1000.0f;
            }
        }
    }

/*
 * Make a copy of the data if this is a vertical fault surface.
 */
    if (VerticalFaultFlag == 1) {

        OrigIdata = (int *)csw_Malloc (OrigNdata * sizeof(int));
        if (!OrigIdata) {
            TinySum = 0.0;
            return -1;
        }

        MaxData = OrigNdata * 16;
        if (FreeZdataFlag) {
            csw_Free (Zdata);
            Zdata = NULL;
            FreeZdataFlag = 0;
        }
        Xdata = (CSW_F *)csw_Malloc (MaxData * 3 * sizeof(CSW_F));
        if (!Xdata) {
            grd_utils_ptr->grd_set_err (1);
            TinySum = 0.0;
            return -1;
        }
        else {
            Ydata = Xdata + MaxData;
            Zdata = Ydata + MaxData;
            Ndata = 0;
            for (i=0; i<OrigNdata; i++) {
                if (OrigXdata[i] < 1.e30) {
                    Xdata[Ndata] = OrigXdata[i];
                    Ydata[Ndata] = OrigYdata[i];
                    Zdata[Ndata] = OrigZdata[i];
                    OrigIdata[Ndata] = i;
                    Ndata++;
                }
            }
        }
    }

/*
    If global anisotropy calculations are wanted and also there
    is no preferred strike specified, calculate a preferred strike
    and strike power using the global anisotropy algorithm.
*/
    AnisotropyRatio = 1.0f;
    if (PreferredStrike < 0.0f  &&  AnisotropyFlag == GRD_GLOBAL_ANISOTROPY) {
        tiny = (x2 - x1 + y2 - y1) / 100.0f;
        istat = grd_utils_ptr->grd_colinear_check (Xdata, Ydata, Ndata, tiny, NULL);
        if (istat == 0) {
            for (;;) {
                istat = grd_stats_ptr->grd_global_anisotropy
                        (Xdata, Ydata, Zdata, npts, nbins,
                         &PreferredStrike, &StrikePower, &AnisotropyRatio, ThicknessFlag);
                if (PreferredStrike < 0  &&  PreferredStrike >= -200) {
                    PreferredStrike += 360.0f;
                }
                if (istat == -2) {
                    if (nbins <= 10) break;
                    nbins -= 5;
                    continue;
                }
                else {
                    break;
                }
            }
        }
        else {
            AnisotropyFlag = 0;
            PreferredStrike = -1000.0f;
        }
        if (PreferredStrike < 0.0f) {
            AnisotropyFlag = 0;
        }
        if (PreferredStrike >= 0.0  &&  StrikePower > 3) {
            AnisotropyFlag = 0;
        }
    }
    if (AnisotropyRatio < 1.0f) AnisotropyRatio = 1.0f;

/*
 *  Use triangulation to add some points to empty regions
 *  of the area of interest.  These points are used to
 *  build the EmptyAreaGrid.  If this grid is not NULL,
 *  it is used in extending the coarse grid nodes into
 *  empty areas.
 */
    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        grd_fileio_ptr->grd_write_float_points (
            Xdata, Ydata, Zdata, Ndata,
            (char *)"pre_empty.xyz");
    }

    num_pre_pass1 = Ndata;
    AddPointsToEmptyRegions ();

    if (do_write == 1) {
        grd_fileio_ptr->grd_write_float_points (
            Xdata, Ydata, Zdata, Ndata,
            (char *)"post_empty.xyz");
    }

/*
    Allocate the workspace and set up the data tables used for
    rapid lookup of local points at grid nodes.  An error returned
    by this function indicates a memory allocation error.
*/
    istat = SetupDataTable (0, 1);
    if (istat == -1) {
        TinySum = 0.0;
        return -1;
    }

/*
    Create a table that has the distance to the closest
    data point at each grid node.  This distance is expressed
    in number of grid cells.  It is used to speed up searches
    only.  The function always returns 1, so I don't check
    the return status.
*/
    SetupDistanceTable ();

/*
    If the local anisotropy flag is turned on,
    calculate the local anisotropy grids.
*/
    if (PreferredStrike < 0.0f  &&  AnisotropyFlag == GRD_LOCAL_ANISOTROPY) {
        CalcLocalAnisotropy ();
    }

/*
    Calculate a second order trend surface and a plane.
    Use the plane unless the second order fit is far better
    than the plane fit.
*/
    if (FlatGridFlag == 0) {
        istat = FitBestTrendSurface ();
        if (istat == -1) {
            if (grd_utils_ptr->grd_get_err() == 3) grd_utils_ptr->grd_set_err (8);
            TinySum = 0.0;
            return -1;
        }
    }

/*
    A flat grid also has a flat trend surface grid.
*/
    else {
        for (i=0; i<Nccol*Ncrow; i++) {
            TrendGrid[i] = Zdata[0];
        }
    }

/*
    If there is a conformal shaping surface, resample it to the
    same geometry as the work grid being calculated.
*/
    BuildShapeGrid ();

/*
    Change the TrendDistance based on how well the trend surface fits.
    For a better fit, the trend surface is used closer to data.
*/
    TrendDistance = (int) ((Xrange + Yrange) / ((Xspace + Yspace) * 5.0f));
    if (FaultedFlag) TrendDistance *= 4;
    TrendPointErrors ();

/*
    Calculate coarse grid nodes based on local data where the node
    has a sufficient local data population.
*/
    if (FlatGridFlag == 0) {
        CoincidentDistance = (Xspace + Yspace) / 20.0f;
        CalcLocalNodes ();
    }
    CoincidentDistance = 0.0;

/*
 * If additional points were created to highlight a preferred
 * strike direction, redo the data table and distance table
 * without these points.
 */
   if (num_pre_pass1 != Ndata) {
        Ndata = num_pre_pass1;
        istat = SetupDataTable (0, 0);
        if (istat == -1) {
            TinySum = 0.0;
            return -1;
        }

        SetupDistanceTable ();
    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        grd_triangle_ptr->grd_WriteXYZGridFile (
            (char *)"localnodes.xyz",
            Grid, Ncol, Nrow, Ncoarse,
            Xmin, Ymin, Xmax, Ymax);
    }

/*
 * Calculate a measure of grid sparseness, ie nodes per
 * data point.  For sparser grids, more smoothing is done.
 */
    node_ratio = (double)(Ncol * Nrow) / (double)Ndata;
    smooth_mult = node_ratio / 36.0;

/*
    Use the trend surface for coarse nodes
    very far from data points.
*/
    if (FlatGridFlag == 0  &&  NoisyDataFlag == 0) {
        AssignTrendNodes ();
    }

/*
    Assign elevations to all perimeter nodes that
    have not already been calculated.  This is
    accomplished by linear interpolation between
    calculated nodes on the perimeter.  This is done
    at the coarse grid interval.
*/
    if (FlatGridFlag == 0) FillInPerimeter ();

/*
    fill in the rest of the coarse grid nodes.
*/
    if (FlatGridFlag == 0) FillInCoarse ();

    if (StepGridFlag == 0  &&  Ndata > 3) {

    /*
        Smooth the coarse grid lightly.
    */
        savsmooth = SmoothingFactor;
        SmoothingFactor = 1.0f;
        SmoothingFactor *= (CSW_F)smooth_mult;
        if (SmoothingFactor > 3) SmoothingFactor = 3;
        if (PreferredStrike >= 0  ||  AnisotropyFlag) {
            StrikeSmoothGrid ();
        }
        else {
            SimpleSmoothGrid ();
        }

    /*
        Perform more smoothing passes which do not effect
        nodes very close to data.
    */
        ismooth = (int)(SmoothingFactor + 1.5f);
        SmoothingFactor = 1.0f / DistancePower;
        SmoothingFactor *= (CSW_F)smooth_mult;
        if (SmoothingFactor > 5) SmoothingFactor = 5;
        if (DistancePower == 6  &&  Ncoarse == 1) {
            ismooth = 0;
        }
        ismooth = ismooth * (TriangulateFlag + 1);
        for (i=0; i<ismooth/2; i++) {
            if (PreferredStrike >= 0  ||  AnisotropyFlag) {
                StrikeSmoothGrid ();
            }
            else {
                SimpleSmoothGrid ();
            }
        }
        SmoothingFactor = savsmooth;
    }

    if (do_write == 1) {
        grd_triangle_ptr->grd_WriteXYZGridFile (
            (char *)"smoothnodes.xyz",
            Grid, Ncol, Nrow, Ncoarse,
            Xmin, Ymin, Xmax, Ymax);
    }

/*
    The initial coarse grid is now complete.  The
    final grid is calculated by iteratively correcting
    for errors and subdividing the coarse grid until
    the final grid is obtained.
*/
    if (FlatGridFlag == 0  &&  Ndata > 3) {
        if (StepGridFlag == 0) {
            IterateToFinalGrid ();
        }
        else {
            FillInStepGrid ();
            SmoothStepGrid ();
        }
    }

/*
    If the distance power is 6, do a special median filter to
    sharpen up the features.
*/
    if (DistancePower == 6) {
        istat = MedianFilter ();
        if (istat == -1) {
            TinySum = 0.0;
            return -1;
        }
        MedianFlag = 1;
        DistancePower = 2;
        AdjustForErrors ();
        MedianFlag = 0;
        DistancePower = 6;
    }

/*
    Final smoothing pass.
*/
    dpsav = DistancePower;
    if (StepGridFlag == 0  &&  FlatGridFlag == 0  &&  Ndata > 3) {
        savsmooth = SmoothingFactor;
        SmoothingFactor = -1.0f;
        ismooth = (int)(SmoothingFactor + 3.5f);
        ismooth /= 3;
        if (DistancePower > 3) ismooth--;
        if (DistancePower > 5) ismooth--;
        if (ismooth < 1) ismooth = 1;
        if (ConformalFlag == 1  &&  UseShapeGrid == 1) {
            ismooth++;
        }
        if (DistancePower == 6  &&  Ncoarse == 1) {
            ismooth = 1;
            DistancePower = 2;
        }
        if (recalc_grid_flag == 1) {
            ismooth = 1;
        }
        ismooth = ismooth * (TriangulateFlag + 1);
        for (i=0; i<ismooth; i++) {
            if (PreferredStrike >= 0.0f  &&  AnisotropyFlag == 0  &&  StrikePower > 2) {
                if (StrikePower > 4) {
                    StrikeSmoothGrid ();
                }
                else {
                    if (i == 0) {
                        StrikeSmoothGrid ();
                    }
                    else {
                        SimpleSmoothGrid ();
                    }
                }
            }
            else {
                SimpleSmoothGrid ();
            }
        }
        SmoothingFactor = savsmooth;
    }
    DistancePower = dpsav;

/*
    Clip to min max if needed.
*/
    if (StepGridFlag == 0  &&  FlatGridFlag == 0) {
        ClipToMinMax ();
    }

/*
    If the mask array is not NULL, calculate the grid mask.
*/
    if (mask) {
        MaskBadNodes ();
    }

/*
    Special case for a flat grid (all same value).
*/
    if (FlatGridFlag == 1) {
        grd_utils_ptr->grd_constant_grid (Grid, NULL, Ncol, Nrow, z[0]);
        if (err) {
            memset (err, 0, npts*sizeof(CSW_F));
        }
        memset (Zerr, 0, npts * sizeof(CSW_F));
        memset (ZerrBC, 0, npts * sizeof(CSW_F));
    }


/*
 * special debug stuff.
 */
    do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_triangle_ptr->grd_WriteXYZGridFile ((char *)"pre_resamp.grd",
                     Grid, Ncol, Nrow, 1,
                     Xmin, Ymin, Xmax, Ymax);
    }

/*
    Copy the grid and mask into the output space.
*/
    j = 0;
    for (i=RowZero; i<RowZero+OrigNrow; i++) {
        k = i * Ncol + ColZero;
        k2 = j * OrigNcol;
        j++;
        memcpy (grid+k2, Grid+k, OrigNcol*sizeof(CSW_F));
        if (mask) {
            memcpy (mask+k2, NodeMask+k, OrigNcol*sizeof(char));
        }
    }

/*
    Resample back to the requested geometry if needed.
*/
    if (resampleflag == 1) {
        if (DistancePower != 6) {
            grd_arith_ptr->grd_resample_grid (grid, mask, ncol, nrow,
                               x1, y1, x2, y2,
                               faults, nfaults,
                               newgrid, newmask, orig_ncol, orig_nrow,
                               x1, y1, x2, y2, GRD_BICUBIC);
        }
        else {
            grd_arith_ptr->grd_resample_grid (grid, mask, ncol, nrow,
                               x1, y1, x2, y2,
                               faults, nfaults,
                               newgrid, newmask, orig_ncol, orig_nrow,
                               x1, y1, x2, y2, GRD_BILINEAR);
        }
        ncol = orig_ncol;
        nrow = orig_nrow;
        Xspace = (x2 - x1) / (CSW_F)(ncol - 1);
        Yspace = (y2 - y1) / (CSW_F)(nrow - 1);
        memcpy (grid, newgrid, ncol*nrow*sizeof(CSW_F)); /*lint !e668*/
        csw_Free (newgrid);
        newgrid = NULL;
        if (mask) {
            memcpy (mask, newmask, ncol*nrow*sizeof(char)); /*lint !e668*/
            csw_Free (newmask);
            newmask = NULL;
        }
        if (FlatGridFlag == 1) {
            grd_utils_ptr->grd_constant_grid (grid, NULL, ncol, nrow, z[0]);
        }
    }

/*
    Resample again if a very small grid was expanded to help the faulted calculations.
*/
    if (fault_resampleflag == 1) {
        grd_arith_ptr->grd_resample_grid (grid, mask, ncol, nrow,
                           x1, y1, x2, y2,
                           faults, nfaults,
                           in_grid, in_mask, pre_fault_ncol, pre_fault_nrow,
                           x1, y1, x2, y2, GRD_BICUBIC);
        ncol = pre_fault_ncol;
        nrow = pre_fault_nrow;
        Xspace = (x2 - x1) / (CSW_F)(ncol - 1);
        Yspace = (y2 - y1) / (CSW_F)(nrow - 1);
        if (FlatGridFlag == 1) {
            grd_utils_ptr->grd_constant_grid (in_grid, NULL, ncol, nrow, z[0]);
        }
        grid = in_grid;
        mask = in_mask;
        orig_ncol = pre_fault_ncol;
        orig_nrow = pre_fault_nrow;
        ncol = orig_ncol;
        nrow = orig_nrow;
        csw_Free (local_grid);
        csw_Free (local_mask);
        local_grid = NULL;
        local_mask = NULL;
    }

    if (faults) {
        grd_fault_ptr->grd_free_fault_line_structs (faults, nfaults);
        faults = NULL;
        nfaults = 0;
    }

/*
    If the BaselapFlag is true, clip the grid to the baselap surface.
*/
    if (BaselapFlag == 1) {
        ClippingGrid = (CSW_F *)csw_Malloc (orig_ncol * orig_nrow * sizeof(CSW_F));
        if (!ClippingGrid) {
            grd_utils_ptr->grd_set_err (1);
            TinySum = 0.0;
            return -1;
        }
        grd_arith_ptr->grd_resample_grid (BaselapGrid.grid,
                           NULL,
                           BaselapGrid.ncol,
                           BaselapGrid.nrow,
                           BaselapGrid.x1,
                           BaselapGrid.y1,
                           BaselapGrid.x2,
                           BaselapGrid.y2,
                           NULL, 0,
                           ClippingGrid, NULL, orig_ncol, orig_nrow,
                           OrigXmin, OrigYmin, OrigXmax, OrigYmax, GRD_BICUBIC);
        for (k=0; k<orig_ncol*orig_nrow; k++) {
            if (grid[k] < ClippingGrid[k]) {
                grid[k] = ClippingGrid[k];
                if (mask) {
                    mask[k] = GRD_MASK_INVALID_OUTSIDE;
                }
            }
        }
        csw_Free (ClippingGrid);
        ClippingGrid = NULL;
    }

/*
    If the TruncationFlag is true, clip the grid to the truncation surface.
*/
    if (TruncationFlag == 1) {
        ClippingGrid = (CSW_F *)csw_Malloc (orig_ncol * orig_nrow * sizeof(CSW_F));
        if (!ClippingGrid) {
            grd_utils_ptr->grd_set_err (1);
            TinySum = 0.0;
            return -1;
        }
        grd_arith_ptr->grd_resample_grid (TruncationGrid.grid,
                           NULL,
                           TruncationGrid.ncol,
                           TruncationGrid.nrow,
                           TruncationGrid.x1,
                           TruncationGrid.y1,
                           TruncationGrid.x2,
                           TruncationGrid.y2,
                           NULL, 0,
                           ClippingGrid, NULL, orig_ncol, orig_nrow,
                           OrigXmin, OrigYmin, OrigXmax, OrigYmax, GRD_BICUBIC);
        for (k=0; k<orig_ncol*orig_nrow; k++) {
            if (grid[k] > ClippingGrid[k]) {
                grid[k] = ClippingGrid[k];
                if (mask) {
                    mask[k] = GRD_MASK_INVALID_OUTSIDE;
                }
            }
        }
        csw_Free (ClippingGrid);
        ClippingGrid = NULL;
    }

/*
 * Clip the output grid to any min and max specified in the options.
 * The min or max may have been changed for a thickness grid so they
 * are reset prior to the clipping.
 */
    if (GridMinValue < orig_min_val) GridMinValue = orig_min_val;
    if (GridMaxValue > orig_max_val) GridMaxValue = orig_max_val;

    ClipOutputGridToMinMax (grid, ncol, nrow);

/*
 * The grid is now finished and stored in the grid array (same as in_grid).
 * The remainder of this function is used for error estimation at input points
 * and for report generation.
 */
    if (err) {

        fistat = 0;
        if (FaultedFlag) {
            fistat = grd_fault_ptr->grd_build_fault_indices (
                grid, ncol, nrow,
                OrigXmin, OrigYmin, OrigXmax, OrigYmax);
        }
        if (FaultedFlag  &&  fistat == 1) {
            if (ncol > 3  &&  nrow > 3) {
                istat = grd_fault_ptr->grd_build_fault_indices (
                    grid, ncol, nrow,
                    OrigXmin, OrigYmin, OrigXmax, OrigYmax);
                grd_fault_ptr->grd_back_interpolate_faulted (grid, ncol, nrow,
                                              OrigXmin, OrigYmin, OrigXmax, OrigYmax,
                                              Xdata, Ydata, Zerr, Ndata,
                                              GRD_BILINEAR);
            }
            else {
                for (i=0; i<Ndata; i++) {
                    Zerr[i] = 1.e30f;
                }
            }
        }
        else {
            grd_arith_ptr->grd_back_interpolate (grid, ncol, nrow,
                                  OrigXmin, OrigYmin, OrigXmax, OrigYmax,
                                  NULL, 0,
                                  Xdata, Ydata, Zerr, Ndata, GRD_BILINEAR);
        }
        MaxError = -1.e30f;
        for (i=0; i<Ndata; i++) {
            if (Zerr[i] < 1.e20f) {
                Zerr[i] -= Zdata[i];
                ftmp = Zerr[i];
                if (ftmp < 0.0f)
                    ftmp = - ftmp;
                if (ftmp > MaxError)
                    MaxError = ftmp;
            }
            else {
                Zerr[i] = 0.0f;
            }
        }
        if (FaultedFlag  &&  fistat == 1) {
            if (ncol > 3  &&  nrow > 3) {
                grd_fault_ptr->grd_back_interpolate_faulted (grid, ncol, nrow,
                                              OrigXmin, OrigYmin, OrigXmax, OrigYmax,
                                              Xdata, Ydata, ZerrBC, Ndata, GRD_BICUBIC);
            }
            else {
                for (i=0; i<Ndata; i++) {
                    ZerrBC[i] = 1.e30f;
                }
            }
        }
        else {
            grd_arith_ptr->grd_back_interpolate (grid, ncol, nrow,
                                  OrigXmin, OrigYmin, OrigXmax, OrigYmax,
                                  NULL, 0,
                                  Xdata, Ydata, ZerrBC, Ndata, GRD_BICUBIC);
        }
        for (i=0; i<Ndata; i++) {
            if (ZerrBC[i] < 1.e20f) {
                ZerrBC[i] -= Zdata[i];
            }
            else {
                ZerrBC[i] = 0.0f;
            }
        }

    /*
        Copy the errors to the output array if needed.
    */
        if (FlatGridFlag == 0) {
            memcpy (err, Zerr, npts*sizeof(CSW_F));
        }
    }

/*
    Generate a report if needed.
*/
    if (report) {

    /*
        Find grid min/max for report.
    */
        if (FlatGridFlag == 0) {
            Gmin = 1.e30f;
            Gmax = -1.e30f;
            for (i=0; i<nrow; i++) {
                offset = i * ncol;
                for (j=0; j<ncol; j++) {
                    k = offset + j;
                    if (grid[k] < Gmin) {
                        Gmin = grid[k];
                        Rminrow = i;
                        Rmincol = j;
                        Rgxmin = OrigXmin + j * Xspace;
                        Rgymin = OrigYmin + i * Yspace;
                    }
                    if (grid[k] > Gmax) {
                        Gmax = grid[k];
                        Rmaxrow = i;
                        Rmaxcol = j;
                        Rgxmax = OrigXmin + j * Xspace;
                        Rgymax = OrigYmin + i * Yspace;
                    }
                }
            }
        }
        else {
            Gmin = z[0];
            Gmax = z[0];
        }

    /*
        Write the report to a string and return it in the report pointer.
    */
        GenerateReport (report);
    }

/*
    Make sure the points that were set as nearly on top of faults
    are reset back to their original coordinates.
*/
    if (FaultedFlag) {
        grd_fault_ptr->grd_reset_fault_data_grazing (x, NULL, NULL);
    }

    if (in_mask) {
        if ((int)in_mask[0] == -3) {
            in_mask[0] = -3;
        }
    }

/*
    thats all folks.
*/
    TinySum = 0.0;
    return 1;

}  /*  end of function grd_calc_grid  */








/*
  ****************************************************************

                   S e t u p D a t a T a b l e

  ****************************************************************

    Set up the DataTable grid and the link list of pointers to
  data points that occupy each grid cell.

*/

int CSWGrdCalc::SetupDataTable (int zflag, int first)
{
    int            *lastpoint = NULL, icc, icr, ic, ir, idx, izc, izr,
                   i, nmax, ncells, nmean, zeroflag;
    int            idxc;
    CSW_F          x1, y1, x2, y2, xspacec, yspacec;
    CSW_F          xt, yt;
    CSW_F          *bin_min = NULL, *bin_max = NULL;

    bool      bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (lastpoint);
        csw_Free (bin_min);
        if (bsuccess == false) {
            FreeMem ();
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    zeroflag = 0;
    if (zflag == 1  &&  ThicknessFlag == 1) zeroflag = 1;

    if (first == 1) {

    /*
        Allocate space for the private work arrays.
    */
        if (true) {
            nmax = Ncol * Nrow * 5 + Nccol * Ncrow * 6 + Ndata * 2;
MSL
            Grid = (CSW_F *)csw_Malloc (nmax * sizeof (CSW_F));
            if (!Grid) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }

            nmax = Ncol * Nrow * 2 + Ndata + 2 * Nrow;
MSL
            DataTable = (int *)csw_Malloc (nmax * sizeof(int));
            if (!DataTable) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }

            nmax = 2 * Nccol * Ncrow + 2 * Ncol * Nrow;
MSL
            Icoarse = (char *)csw_Calloc (nmax * sizeof(char));
            if (!Icoarse) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }

        /*
            assign various pointers into the allocated space.
        */
            Gwork1 = Grid + Ncol * Nrow;
            Gwork2 = Gwork1 + Ncol * Nrow;
            Gwork3 = Gwork2 + Ncol * Nrow;
            TrendGrid = Gwork3 + Ncol * Nrow;
            AratioGrid = TrendGrid + Nccol * Ncrow;
            AnisoStrike = AratioGrid + Nccol * Ncrow;
            AnisoPower = AnisoStrike + Nccol *Ncrow;
            Zerr = AnisoPower + Nccol * Ncrow;
            ZerrBC = Zerr + Ndata;
            ShapeGrid = ZerrBC + Ndata;
            DifferenceGrid = ShapeGrid + Ncol * Nrow;

            ClosestPoint = DataTable + Ncol * Nrow;
            LinkList = ClosestPoint + Ncol * Nrow;
            Left = LinkList + Ndata;
            Right = Left + Nrow;

            NodeMask = Icoarse + Nccol * Ncrow;
            ZeroCoarse = NodeMask + Ncol * Nrow;
        }

    }

/*
    allocate a temporary array to use while building
    the link lists for data in each cell
*/
MSL
    lastpoint = (int *)csw_Malloc (Ncol * Nrow * sizeof(int));
    if (!lastpoint) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    initialize some of the workspace arrays
*/
    for (i=0; i<Ncol*Nrow; i++) {
        DataTable[i] = -1;
    }
    if (first == 1) {
        for (i=0; i<Ncol*Nrow; i++) {
            Grid[i] = 1.e30f;
        }
    }

    memset ((char *)ClosestPoint, 0, Ncol*Nrow*sizeof(int));
    memset ((char *)Icoarse, 0, Nccol*Ncrow*sizeof(char));
    memset ((char *)ZeroCoarse, 0, Nccol*Ncrow*sizeof(char));

/*
  Allocate and initialize the nosy data bins (min and max in each bin).
*/
    bool bnoise = false;
    if (NoisyDataFlag == 0) {
      bin_min = (CSW_F *)csw_Malloc (Nccol * Ncrow * 2 * sizeof(CSW_F));
      if (bin_min == NULL) {
        return -1;
      }
      bin_max = bin_min + Nccol * Ncrow;
      for (i=0; i<Nccol * Ncrow; i++) {
        bin_min[i] = 1.e30;
        bin_max[i] = -1.e30;
      }
      bnoise = true;
    }

/*
    build the data table and link list
*/
    Zmin = 1.e30f;
    Zmax = -1.e30f;
    x1 = 1.e30f;
    y1 = 1.e30f;
    x2 = -1.e30f;
    y2 = -1.e30f;
    Zmean = 0.0f;
    nmean = 0;
    ncells = 0;
    xspacec = Xspace * Ncoarse;
    yspacec = Yspace * Ncoarse;

    for (i=0; i<Ndata; i++) {

        if (Xdata[i] < x1) x1 = Xdata[i];
        if (Ydata[i] < y1) y1 = Ydata[i];
        if (Xdata[i] > x2) x2 = Xdata[i];
        if (Ydata[i] > y2) y2 = Ydata[i];

    /*
        If the thickness flag is set, no negative z values are allowed.
    */
        if (ThicknessFlag == 1) {
            if (Zdata[i] < 0.0f) Zdata[i] = 0.0f;

        /*
            Set the closest node in the coarse zero grid to 1.
            This is used to control extrapolation from the non
            zero data area.
        */
            if (Zdata[i] <= 0.0f  &&  first == 1) {
                izc = (int) ((Xdata[i] - Xmin) / xspacec + 0.5f);
                izr = (int) ((Ydata[i] - Ymin) / yspacec + 0.5f);
                if (izc >= 0  &&  izr >= 0  &&  izc < Nccol  &&  izr < Ncrow) {
                    ZeroCoarse[izr*Nccol + izc] = 1;
                }
            }
        }

    /*
        If the zeroflag is set, no zero or very near zero
        values are indexed.
    */
        if (zeroflag == 1  &&  Zdata[i] <= 0.0f) {
            continue;
        }

        if (Xdata[i] < Xmin  ||  Xdata[i] > Xmax  ||
            Ydata[i] < Ymin  ||  Ydata[i] > Ymax) {
        /*
         * Bug 9735 fix, update z limits even if the point
         * is not in the area of interest.
         */
            if (Zdata != NULL) {
                if (Zdata[i] < Zmin) Zmin = Zdata[i];
                if (Zdata[i] > Zmax) Zmax = Zdata[i];
            }
            AddToExtraPoints (i);
            continue;
        }

        if (Zdata[i] < Zmin) {
            Zmin = Zdata[i];
            Rxmin = Xdata[i];
            Rymin = Ydata[i];
        }
        if (Zdata[i] > Zmax) {
            Zmax = Zdata[i];
            Rxmax = Xdata[i];
            Rymax = Ydata[i];
        }
        Zmean += Zdata[i];
        nmean++;

    /*
        which detail grid cell holds the point
    */
        ic = (int) ((Xdata[i] - Xmin) / Xspace);
        if (ic < 0  ||  ic >= Ncol) {
        /*
         * Bug 9735 fix, update z limits even if the point
         * is not in the area of interest.
         */
            if (Zdata != NULL) {
                if (Zdata[i] < Zmin) Zmin = Zdata[i];
                if (Zdata[i] > Zmax) Zmax = Zdata[i];
            }
            AddToExtraPoints (i);
            continue;
        }
        ir = (int) ((Ydata[i] - Ymin) / Yspace);
        if (ir < 0  ||  ir >= Nrow) {
        /*
         * Bug 9735 fix, update z limits even if the point
         * is not in the area of interest.
         */
            if (Zdata != NULL) {
                if (Zdata[i] < Zmin) Zmin = Zdata[i];
                if (Zdata[i] > Zmax) Zmax = Zdata[i];
            }
            AddToExtraPoints (i);
            continue;
        }

    /*
        set the coarse grid cell to occupied
    */
        icc = ic / Ncoarse;
        icr = ir / Ncoarse;
        idxc = Nccol * icr + icc;
        *(Icoarse + idxc) = 1;

    /*
        Set the detail grid data table and link list
    */
        idx = ic + Ncol * ir;
        if (DataTable[idx] == -1) {
            DataTable[idx] = i;
            LinkList[i] = -1;
            ncells++;
        }
        else {
            LinkList[lastpoint[idx]] = i;
            LinkList[i] = -1;
        }
        lastpoint[idx] = i;

    /*
       Update the bin min and max values.
    */
        if (bnoise) {
          CSW_F  zt = Zdata[i];
          if (zt < 1.e20  &&  zt > -1.e20) {
            if (zt < bin_min[idxc]) bin_min[idxc] = zt;
            if (zt > bin_max[idxc]) bin_max[idxc] = zt;
          }
        }

    }

/*
 * Check if data are noisy.  This is pretty much empirical.
 */
    if (bnoise && Zmax > Zmin) {
      CSW_F zt;
      int ng4 = 0;
      CSW_F  zr4 = (Zmax - Zmin) / 3.0;
      for (i=0; i<Nccol*Ncrow; i++) {
        if (bin_min[i] < bin_max[i]) {
          zt = bin_max[i] - bin_min[i];
          if (zt > zr4) {
            ng4++;
          }
        }
      }
      if (ng4 > Nccol * Ncrow / 10) {
        CSW_F  nprat = (CSW_F)Ndata / 100000.0;
        if (nprat > 1.5) nprat = 1.5;
        if (nprat < .05) nprat = .05;
        CSW_F  drat = (CSW_F)ng4 / (CSW_F)(Nccol * Ncrow);
        NoisyDataFlag = (int)(drat * 10.0 * nprat + 1.5);
      }
    }
    else {
      if (NoisyDataFlag == 1) NoisyDataFlag = 10;
    }

/*
 * Add the control points if specified.
 */
    if (UseControlInDataTable == 1) {

        for (i=0; i<NumControlPoints; i++) {

            xt = ControlPoints[i].x;
            yt = ControlPoints[i].y;

            if (xt < Xmin  ||  xt > Xmax  ||
                yt < Ymin  ||  yt > Ymax) {
                continue;
            }

        /*
            which detail grid cell holds the point
        */
            ic = (int) ((xt - Xmin) / Xspace);
            if (ic < 0  ||  ic >= Ncol) {
                continue;
            }
            ir = (int) ((yt - Ymin) / Yspace);
            if (ir < 0  ||  ir >= Nrow) {
                continue;
            }

        /*
            set the coarse grid cell to occupied
        */
            icc = ic / Ncoarse;
            icr = ir / Ncoarse;
            *(Icoarse + icc + Nccol * icr) = 1;

        /*
            Set the detail grid data table and link list
        */
            idx = ic + Ncol * ir;
            if (DataTable[idx] == -1) {
                DataTable[idx] = i;
                LinkList[i] = -1;
                ncells++;
            }
            else {
                LinkList[lastpoint[idx]] = i;
                LinkList[i] = -1;
            }
            lastpoint[idx] = i;
        }

    } /* end of control point if block */

    if (x1 < OrigXmin) x1 = OrigXmin;
    if (x2 > OrigXmax) x2 = OrigXmax;
    if (y1 < OrigYmin) y1 = OrigYmin;
    if (y2 > OrigYmax) y2 = OrigYmax;
    Xrange = x2 - x1;
    Yrange = y2 - y1;
    Zrange = Zmax - Zmin;
    if (ThicknessFlag == 1) {
        Zrange = Zmax;
    }
    ZminusVal = Zrange / 2.0f;
    ZminusVal = -ZminusVal;
    Ztiny = Zrange / 100000.0f;
    if (nmean > 0) {
        Zmean /= (CSW_F)nmean;
    }
    if (Zrange <= 0.0) {
        FlatGridFlag = 1;
    }

/*
    The point density is used to determine how far
    to extend the coarse grid outward from well controlled
    areas.  The extension is done after the local nodes
    are calculated by extrapolating local node gradients
    to neighboring uncalculated nodes.
*/
    PointDensity = (CSW_F)ncells / (CSW_F)(OrigNcol * OrigNrow);

    bsuccess = true;

    return 1;


}  /*  end of private SetupDataTable function  */




/*
  ****************************************************************

               S e t u p D i s t a n c e T a b l e

  ****************************************************************

  Fill in the ClosestPoint array with the l1 distance to the
  nearest data point.  The distance is based on a square of cells
  around each grid point.  If the nearest point is in the square
  that lies 5 grid cells away, the distance assigned is 5.  This
  is not a euclidean distance.  The distance is only used to
  speed up searching and combining different components of the
  grid calculations.

*/

int CSWGrdCalc::SetupDistanceTable (void)
{
    int           i, j, k, ii, kk, ncm1, nrm1, done,
                  offset, offset2, start, maxii, i1, i2, j0, j2;
    int           irow, jcol;
    CSW_F         xt, yt, xt2, yt2, tiny, dx, dy;

/*
    the maximum distance is about half the grid width and height
*/
    maxii = (Ncol + Nrow) / 4;
    if (StepGridFlag) maxii = Ncol + Nrow;
    ncm1 = Ncol - 1;
    nrm1 = Nrow - 1;

/*
    if lower left coarse grid is empty,
    start out at the coarse grid interval
*/
    if (Icoarse[0] == 0) {
        start = Ncoarse;
    }
    else {
        start = 0;
    }

/*
    loop through each row and column
*/
    offset = 0;
    for (i=0; i<Nrow; i++) {

    /*
        for all but the first row, use the value of the
        first column in the previous row as a starting point
    */
        if (i > 0)     {
            start = *(ClosestPoint + offset) - 1;
        }
        offset = i * Ncol;

        for (j=0; j<Ncol; j++) {

            ClosestPoint[offset+j] = maxii;

        /*
            check if there are any points in the square of cells
            start distance from the grid node.  Increment the distance
            until a point is found or until the maximum distance is reached.
        */
            done = 0;
            if (start < 0) start = 0;
            for (ii=start; ii<maxii; ii++) {

            /*
                indices of the corners of a square ii grid cells
                from the node
            */
                j0 = j-ii-1;
                if (j0 < 0) j0 = 0;
                i1 = i - ii - 1;
                if (i1 < 0) i1 = 0;
                j2 = ii+j;
                if (j2 > ncm1) j2 = ncm1;
                i2 = ii+i;
                if (i2 > nrm1) i2 = nrm1;

            /*
                check the top row of the square
            */
                offset2 = i2 * Ncol;
                for (k=j0; k<=j2; k++) {
                    kk = offset2 + k;
                    if (DataTable[kk] != -1) {
                        ClosestPoint[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

            /*
                check the bottom row of the square
            */
                offset2 = i1 * Ncol;
                for (k=j0; k<=j2; k++) {
                    kk = offset2 + k;
                    if (DataTable[kk] != -1) {
                        ClosestPoint[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

            /*
                check the left side of the square
            */
                for (k=i1+1; k<i2; k++) {
                    kk = k * Ncol + j0;
                    if (DataTable[kk] != -1) {
                        ClosestPoint[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

            /*
                check the right side of the square
            */
                for (k=i1+1; k<i2; k++) {
                    kk = k * Ncol + j2;
                    if (DataTable[kk] != -1) {
                        ClosestPoint[offset + j] = ii;
                        start = ii - 1;
                        done = 1;
                        break;
                    }
                }
                if (done == 1) break;

            }
        }
    }

/*
    Check the x and y input points for nearly exact correspondence
    with grid nodes.  If such a correspondance exists, set the ClosestPoint
    array at the node to -(datapoint number +1).  This acts as a flag in
    other parts of the gridding process to set the node to the exact
    value.
*/
    tiny = (Xspace + Yspace) / 100.0f;
    for (i=0; i<Ndata; i++) {
        if (Xdata[i] < Xmin  ||  Xdata[i] > Xmax  ||
            Ydata[i] < Ymin  ||  Ydata[i] > Ymax) {
            continue;
        }
        xt = Xdata[i];
        yt = Ydata[i];
        jcol = (int)((xt - Xmin) / Xspace + .5f);
        irow = (int)((yt - Ymin) / Yspace + .5f);
        xt2 = Xmin + jcol * Xspace;
        yt2 = Ymin + irow * Yspace;
        dx = xt - xt2;
        dy = yt - yt2;
        dx = dx * dx + dy * dy;
        dx = (CSW_F)(sqrt ((double)dx));
        k = irow * Ncol + jcol;
        if (dx < tiny) {
            ClosestPoint[k] = -(i+4);
        }
        else if (dx < tiny * 5.0f) {
            ClosestPoint[k] = -3;
        }
        else if (dx < tiny * 12.0f) {
            ClosestPoint[k] = -2;
        }
        else if (dx < tiny * 25.0f) {
            ClosestPoint[k] = -1;
        }
    }

    return 1;

}  /*  end of private SetupDistanceTable function  */






/*
  ****************************************************************

              C o l l e c t L o c a l P o i n t s

  ****************************************************************

    Collect points from each quadrant surrounding the
  specified grid node.  Levels are searched until enough
  points are found or until the search is too far away.

*/

int CSWGrdCalc::CollectLocalPoints
          (int i, int j, int start, int end,
           int maxquad, int maxloc,
           int *listout, int *nlist, int *nquad)
{
    int              jj, ii, i0, i2, j0, j2, level, astat, istat,
                     nmax, nq1, nq2, nq3, nq4, offset, nt, ipt;
    int              noctant[8], osearch, maxoct, minquad;
    int              *list = ploc_int1;
    CSW_F            x1, y1, x2, y2, swgt;

/*
    Initialize some stuff to suppress lint messages.
*/
    x1 = 0.0f;
    y1 = 0.0f;
    x2 = 0.0f;
    y2 = 0.0f;
    memset (list, 0, MAX_LOCAL * sizeof(int));

/*
    initialize octant, quadrant and total counters to zero
*/
    memset (noctant, 0, 8 * sizeof(int));
    osearch = 0;
    maxoct = maxquad;
    if (LocalSearchPattern == GRD_OCTANT_SEARCH) {
        osearch = 1;
        maxoct = maxquad / 2;
        if (maxoct < 1) maxoct = 1;
        if (maxoct > MAX_OCTANT) maxoct = MAX_OCTANT;
    }
    if (NumLocalPoints > 0  &&  NumLocalPoints < maxloc) maxloc = NumLocalPoints;
    if (LocalSearchPattern == GRD_RADIAL_SEARCH) maxquad = maxloc;

    nq1 = 0;
    nq2 = 0;
    nq3 = 0;
    nq4 = 0;

    Firstq1 = -1;
    Firstq2 = -1;
    Firstq3 = -1;
    Firstq4 = -1;

    nt = 0;

    minquad = maxquad;
    start--;
    if (start < 0) start = 0;

    astat = 0;
    if (PreferredStrike >= 0  ||  AnisotropyFlag) {
        astat = CalcStrikeLine (i, j, &x1, &y1, &x2, &y2, &swgt);
    }

/*
    Make the default outermost search radius larger and the
    maximum octant and quadrant numbers larger if faulting
    is specified.  This is done to make it more likely that
    enough points will be available that are not blocked off
    by faults to calculate the node.
*/
    if (FaultedFlag  &&  ErrorSearch == 0) {
        end *= 2;
        maxquad *= 2;
        maxoct *= 2;
        if (maxoct > MAX_OCTANT) maxoct = MAX_OCTANT;
        if (maxquad > MAX_QUAD) maxquad = MAX_QUAD;
    }

/*
    Adjust the end if the max search distance option was set.
*/
    if (end > MaxNodeDistance) end = MaxNodeDistance;

/*
    loop through levels for each quadrant
*/
    for (level=start; level<=end; level++) {

    /*
        points in first quadrant (northeast)
    */
        if (nq1 < minquad) {

            if (!osearch  || (osearch  &&  noctant[0] < maxoct)) {
                ii = i + level;
                if (ii < Nrow - 1) {
                    offset = Ncol * ii;
                    j0 = j;
                    j2 = j + level;
                    if (j2 > Ncol - 2) j2 = Ncol - 2;
                    for (jj=j0; jj<=j2; jj++) {
                        ipt = DataTable[offset+jj];
                        while (ipt >= 0) {
                            if (Firstq1 < 0)
                                Firstq1 = ipt;
                            list[nt] = ipt;
                            nt++;
                            nq1++;
                            if (nt >= maxloc) {
                                *nlist = nt;
                                goto LEVELS_DONE;
                            }
                            ipt = LinkList[ipt];
                            noctant[0]++;
                        }
                    }
                }
            }

            if (!osearch  || (osearch  &&  noctant[1] < maxoct)) {
                jj = j + level;
                if (jj < Ncol-1) {
                    i0 = i;
                    i2 = i + level - 1;
                    if (i2 > Nrow - 2) i2 = Nrow - 2;
                    for (ii = i0; ii<=i2; ii++) {
                        ipt = DataTable[jj + Ncol * ii];
                        while (ipt >= 0) {
                            if (Firstq1 < 0)
                                Firstq1 = ipt;
                            list[nt] = ipt;
                            nt++;
                            nq1++;
                            if (nt >= maxloc) {
                                *nlist = nt;
                                goto LEVELS_DONE;
                            }
                            ipt = LinkList[ipt];
                            noctant[1]++;
                        }
                    }
                }
            }
        }

    /*
        points in second quadrant (northwest)
    */
        if (nq2 < minquad) {

            if (!osearch  ||  (osearch  &&  noctant[2] < maxoct)) {
                ii = i + level;
                if (ii < Nrow - 1) {
                    offset = Ncol * ii;
                    j0 = j - level - 1;
                    if (j0 < 0) j0 = 0;
                    j2 = j - 1;
                    for (jj=j0; jj<=j2; jj++) {
                        ipt = DataTable[offset+jj];
                        while (ipt >= 0) {
                            if (Firstq2 < 0)
                                Firstq2 = ipt;
                            list[nt] = ipt;
                            nt++;
                            nq2++;
                            if (nt >= maxloc) {
                                *nlist = nt;
                                goto LEVELS_DONE;
                            }
                            ipt = LinkList[ipt];
                            noctant[2]++;
                        }
                    }
                }
            }

            if (!osearch  || (osearch  &&  noctant[3] < maxoct)) {
                jj = j - level - 1;
                if (jj >= 0) {
                    i0 = i;
                    i2 = i + level - 1;
                    if (i2 > Nrow - 2) i2 = Nrow - 2;
                    for (ii = i0; ii<=i2; ii++) {
                        ipt = DataTable[jj + Ncol * ii];
                        while (ipt >= 0) {
                            if (Firstq2 < 0)
                                Firstq2 = ipt;
                            list[nt] = ipt;
                            nt++;
                            nq2++;
                            if (nt >= maxloc) {
                                *nlist = nt;
                                goto LEVELS_DONE;
                            }
                            ipt = LinkList[ipt];
                            noctant[3]++;
                        }
                    }
                }
            }
        }

    /*
        points in third quadrant (southwest)
    */
        if (nq3 < minquad) {

            if (!osearch  ||  (osearch  &&  noctant[4] < maxoct)) {
                ii = i - level - 1;
                if (ii >= 0) {
                    offset = Ncol * ii;
                    j0 = j - level - 1;
                    if (j0 < 0) j0 = 0;
                    j2 = j - 1;
                    for (jj=j0; jj<=j2; jj++) {
                        ipt = DataTable[offset+jj];
                        while (ipt >= 0) {
                            if (Firstq3 < 0)
                                Firstq3 = ipt;
                            list[nt] = ipt;
                            nt++;
                            nq3++;
                            if (nt >= maxloc) {
                                *nlist = nt;
                                goto LEVELS_DONE;
                            }
                            ipt = LinkList[ipt];
                            noctant[4]++;
                        }
                    }
                }
            }

            if (!osearch  || (osearch  &&  noctant[5] < maxoct)) {
                jj = j - level - 1;
                if (jj >= 0) {
                    i0 = i - level;
                    if (i0 < 0) i0 = 0;
                    i2 = i - 1;
                    for (ii = i0; ii<=i2; ii++) {
                        ipt = DataTable[jj + Ncol * ii];
                        while (ipt >= 0) {
                            if (Firstq3 < 0)
                                Firstq3 = ipt;
                            list[nt] = ipt;
                            nt++;
                            nq3++;
                            if (nt >= maxloc) {
                                *nlist = nt;
                                goto LEVELS_DONE;
                            }
                            ipt = LinkList[ipt];
                            noctant[5]++;
                        }
                    }
                }
            }
        }

    /*
        points in fourth quadrant (southeast)
    */
        if (nq4 < minquad) {

            if (!osearch  ||  (osearch  &&  noctant[6] < maxoct)) {
                ii = i - level - 1;
                if (ii >= 0) {
                    offset = Ncol * ii;
                    j0 = j;
                    j2 = j + level;
                    if (j2 > Ncol - 2) j2 = Ncol - 2;
                    for (jj=j0; jj<=j2; jj++) {
                        ipt = DataTable[offset+jj];
                        while (ipt >= 0) {
                            if (Firstq4 < 0)
                                Firstq4 = ipt;
                            list[nt] = ipt;
                            nt++;
                            nq4++;
                            if (nt >= maxloc) {
                                *nlist = nt;
                                goto LEVELS_DONE;
                            }
                            ipt = LinkList[ipt];
                            noctant[6]++;
                        }
                    }
                }
            }

            if (!osearch  ||  (osearch  &&  noctant[7] < maxoct)) {
                jj = j + level;
                if (jj < Ncol-1) {
                    i0 = i - level;
                    if (i0 < 0) i0 = 0;
                    i2 = i - 1;
                    for (ii = i0; ii<=i2; ii++) {
                        ipt = DataTable[jj + Ncol * ii];
                        while (ipt >= 0) {
                            if (Firstq4 < 0)
                                Firstq4 = ipt;
                            list[nt] = ipt;
                            nt++;
                            nq4++;
                            if (nt >= maxloc) {
                                *nlist = nt;
                                goto LEVELS_DONE;
                            }
                            ipt = LinkList[ipt];
                            noctant[7]++;
                        }
                    }
                }
            }
        }

        if (level < 2) {
            istat = CheckForCoincidentPointInList (i, j, list, nt);
            if (istat == 1) {
                goto LEVELS_DONE;
            }
        }

    }

    goto LEVELS_DONE;


/*
    A goto to this label is used in the search code above in case
    the maximum number of points is exceeded.  Even if the max is
    exceeded, the points may still need to be filtered for a
    preferred strike direction.
*/

  LEVELS_DONE:

    LocalPointAreaSize = (CSW_F)(level + 1);

    nmax = 0;
    if (nq1 > 0) nmax++;
    if (nq2 > 0) nmax++;
    if (nq3 > 0) nmax++;
    if (nq4 > 0) nmax++;

    *nquad = nmax;

    nmax = 0;
    if (nq1 > maxquad) nq1 = maxquad;
    nmax += nq1;
    if (nq2 > maxquad) nq2 = maxquad;
    nmax += nq2;
    if (nq3 > maxquad) nq3 = maxquad;
    nmax += nq3;
    if (nq4 > maxquad) nq4 = maxquad;
    nmax += nq4;

/*
    If anisotropy is enabled, filter the local points to
    use only those relatively nearest the strike direction.
*/
//StrikeSearch = 0;
    if (astat == 1  &&  StrikeSearch == 1) {
        FilterPointsForStrike (list, nt, listout, nlist, nmax, *nquad,
                               i, j, x1, y1, x2, y2);
        nt = *nlist;
        if (nt < maxloc  &&  Firstq1 >= 0) {
            listout[nt] = Firstq1;
            nt++;
        }
        if (nt < maxloc  &&  Firstq2 >= 0) {
            listout[nt] = Firstq2;
            nt++;
        }
        if (nt < maxloc  &&  Firstq3 >= 0) {
            listout[nt] = Firstq3;
            nt++;
        }
        if (nt < maxloc  &&  Firstq4 >= 0) {
            listout[nt] = Firstq4;
            nt++;
        }
        *nlist = nt;
    }

    else {
        memcpy (listout, list, nt*sizeof(int));
        *nlist = nt;
    }

    return 1;

}  /*  end of private CollectLocalPoints function  */




/*
  ****************************************************************

                       F r e e M e m

  ****************************************************************

    Free the memory in the private workspace arrays.

*/

int CSWGrdCalc::FreeMem (void)
{

    csw_Free (DataTable);
    csw_Free (Grid);
    csw_Free (Icoarse);
    csw_Free (ExtraPoints);

    DataTable = NULL;
    Grid = NULL;
    Icoarse = NULL;
    ExtraPoints = NULL;
    Nextra = 0;

    if (OrigIdata) {
        csw_Free (OrigIdata);
        OrigIdata = NULL;
    }

/*
 * Bug 8290, if the Zdata had to be allocated, csw_Free it here.
 */
    if (FreeZdataFlag == 1) {
        csw_Free (Zdata);
        Zdata = NULL;
        FreeZdataFlag = 0;
    }

    if (Xdata != OrigXdata) {
        if (Xdata) {
            csw_Free (Xdata);
        }
        Xdata = OrigXdata = NULL;
        Ydata = OrigYdata = NULL;
        Zdata = OrigZdata = NULL;
    }

    return 1;

}  /*  end of private FreeMem function  */





/*
  ****************************************************************

                P r o c e s s L o c a l P o i n t s

  ****************************************************************

    Given a set of local points surrounding a grid node, calculate
    the inverse distance squared estimate and also several local
    plane estimates for the elevation at the grid node.  The distance
    average and the plane fit estimates are combined depending on how
    good the plane fits are.

    If a successful value is calculated, or if the node is too
    far from data to be calculated, 1 is returned.  If the node
    is a problem node, (close to data, but less than 3 points in
    the local data set) zero is returned.

*/

int CSWGrdCalc::ProcessLocalPoints
          (int *list, int nlist, int nquad,
           int irow, int jcol, int cp,
           CSW_F *value)
{

    CSW_F     *xloc = p_xloc,
              *yloc = p_yloc,
              *zloc = p_zloc,
              *dsq = p_dsq,
              *xp = p_xp,
              *yp = p_yp,
              *zp = p_zp,
              *ep = p_ep,
              *pdsq = p_pdsq;

    CSW_F        x0, y0, xt, yt,
                 avperr[8],
                 avpwgt[8],
                 hpval[8],
                 xcos, ysin,
                 zpct, zpct2, zpt, zt, pdsqmin, sum, sum2, sum3, sum4, valstrike,
                 besterr, ztiny, tiny, valplane, wgtplane, valdsq, coef[3],
                 wgtdsq, mean, sdev, wgt, stwgt, px, py, fdum, fault_weight,
                 x1, y1, x2, y2, st, st0, wt, strikepx, strikepy, wgtplane2,
                 tiny_value_2, tiny_value, tiny_coef,
                 stiny, xyspace, wptmp, wpdsq, xycoarse;
    double       zloc_min, zloc_max, zloc_delta;
    double       dist, dist_min, dcrit;
    int          astat, ido, np, i, j, k, istat, npmin, cf;
    int          nthick, q1, q2, q3, q4;
    double       valplane_pct, plane_only_wgt;
    int          do_write;

    static const CSW_F  hpang[8] = {0.0f, .7854f, 1.5708f, 2.3562f,
                                    0.0f, .7854f, 1.5708f, 2.3562f};

    *value = 1.e30f;
    stiny = (Xspace + Yspace) / 20.0f;
    tiny = stiny;
    stiny *= stiny;
    xyspace = Xspace + Yspace;

/*
    If nlist is less than 1, return -2
*/
    if (nlist < 1) {
        return -2;
    }

/*
    Initialize half plane arrays.
*/
    memset ((char *)avperr, 0, 8 * sizeof(CSW_F));
    memset ((char *)avpwgt, 0, 8 * sizeof(CSW_F));
    memset ((char *)hpval, 0, 8 * sizeof(CSW_F));

/*
    Calculate endpoints for a line at the preferred
    strike with the grid node as origin.
*/
    wt = Xspace + Yspace;
    astat = CalcStrikeLine (irow, jcol, &x1, &y1, &x2, &y2, &fdum);

/*
    Calculate offsets to apply to extra points generated
    to support the preferred strike direction at the node.
*/
    if (astat == 1) {
        xcos = (CSW_F)atan2 ((double)(y2-y1), (double)(x2-x1));
        ysin = (CSW_F)sin ((double)xcos);
        xcos = (CSW_F)cos ((double)xcos);
        zt = (CSW_F)Ncoarse / 8.0f;
        strikepx = wt * xcos * zt;;
        strikepy = wt * ysin * zt;;
    }
    else {
        strikepx = 0.0f;
        strikepy = 0.0f;
    }
    wt *= wt;

/*
    Make a copy of the data with origin at the grid node.
*/
    x0 = jcol * Xspace + Xmin;
    y0 = irow * Yspace + Ymin;

    if (nlist > MAX_LOCAL / 2) nlist = MAX_LOCAL / 2;

    ZlocMin = 1.e30f;
    ZlocMax = -1.e30f;
    k = 0;
    for (i=0; i<nlist; i++) {
        j = list[i];
        if (j >= 400000000) {
            j -= 400000000;
            px = -2 * strikepx;
            py = -2 * strikepy;
        }
        else if (j >= 300000000) {
            j -= 300000000;
            px = 2 * strikepx;
            py = 2 * strikepy;
        }
        else if (j >= 200000000) {
            j -= 200000000;
            px = -strikepx;
            py = -strikepy;
        }
        else if (j >= 100000000) {
            j -= 100000000;
            px = strikepx;
            py = strikepy;
        }
        else {
            px = 0.0f;
            py = 0.0f;
        }

    /*
        If the faulted flag is specified, only use points that
        are not blocked off from the node in the local data set.
    */
        if (FaultedFlag) {
            if (Xdata[j] >= OnFault)
                continue;
            istat = grd_fault_ptr->grd_check_fault_blocking (
                irow, jcol, Xdata[j]+px, Ydata[j]+py,
                &fault_weight);
            if (istat != 0)
                continue;
        }

        xloc[k] = Xdata[j] - x0 + px;
        yloc[k] = Ydata[j] - y0 + py;
        zloc[k] = Zdata[j];
        if (zloc[k] < ZlocMin) ZlocMin = zloc[k];
        if (zloc[k] > ZlocMax) ZlocMax = zloc[k];
        k++;
    }

/*
    If the faults threw out too many points, try getting more distant
    points in the local data set.
*/
    nlist = k;

    if (nlist < 1) {
        return -2;
    }

    dist_min = 1.e30;
    dcrit = Ncoarse * (Xspace + Yspace) / 2.0;
    if (AnisotropyFlag == 1  ||  PreferredStrike >= 0.0) {
        for (i=0; i<nlist; i++) {
            dist = xloc[i] * xloc[i] + yloc[i] * yloc[i];
            dist = sqrt (dist);
            if (dist < dist_min) {
                dist_min = dist;
            }
        }
    }

/*
    If the points left after fault processing are farther than the
    critical first point distance, do not process the local node.
    It will be filled in later via etension of grid nodes on the
    same fault block which are close to data on that fault block.
*/
    plane_only_wgt = 1.0;
    if (FaultedFlag) {
        double   xldmin, xldist;
        xldmin = 1.e30;
        for (i=0; i<nlist; i++) {
            xt = xloc[i];
            yt = yloc[i];
            xldist = xt * xt + yt * yt;
            xldist = sqrt (xldist);
            if (xldist < xldmin) xldmin = xldist;
        }
        if (xldmin > FaultedCriticalDistance) {
            *value = 1.e30f;
            return 1;
        }
        plane_only_wgt = xldmin / dcrit;
        if (plane_only_wgt > 1.0) plane_only_wgt = 1.0;
    }

/*
    If this is a thickness grid, check for all zero values or for
    a node with less than 3 quadrants that has its nearest point
    zero.  In either case, set the node elevation to ZminusVal.
*/
    if (ThicknessFlag) {
        istat = AdjustForZeros (xloc, yloc, zloc, &nlist);
        if (istat == 1) {
            *value = ZminusVal;
            return 1;
        }
        nthick = 0;
        if (ThicknessFlag == GRD_POSITIVE_THICKNESS) {
            for (i=0; i<nlist; i++) {
                if (zloc[i] > 0.0) {
                    xloc[nthick] = xloc[i];
                    yloc[nthick] = yloc[i];
                    zloc[nthick] = zloc[i];
                    nthick++;
                }
            }
        }
        else {
            for (i=0; i<nlist; i++) {
                if (zloc[i] < 0.0) {
                    xloc[nthick] = xloc[i];
                    yloc[nthick] = yloc[i];
                    zloc[nthick] = zloc[i];
                    nthick++;
                }
            }
        }
        nlist = nthick;

    /*
     * Recalculate number of quads based on non zero points.
     */
        nquad = 0;
        q1 = q2 = q3 = q4 = 0;
        for (i=0; i<nlist; i++) {
            if (xloc[i] >= 0.0  &&  yloc[i] >= 0.0) {
                q1 = 1;
            }
            else if (xloc[i] < 0.0  &&  yloc[i] >= 0.0) {
                q2 = 1;
            }
            else if (xloc[i] < 0.0  &&  yloc[i] < 0.0) {
                q3 = 1;
            }
            else if (xloc[i] >= 0.0  &&  yloc[i] < 0.0) {
                q2 = 1;
            }
        }

        nquad = q1 + q2 + q3 + q4;

    }

/*
    If the StepGridFlag is set, return the elevation of the closest point.
*/
    if (StepGridFlag) {
        grd_utils_ptr->grd_get_closest_point_value (xloc, yloc, zloc, nlist, &valdsq);
        *value = valdsq;
        return 1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_fileio_ptr->grd_write_float_points (xloc, yloc, zloc, nlist,
                         (char *)"nodepoints.xyz");
    }

/*
    Calculate the distance squared at each point and an elevation
    estimate using a weighted average based on inverse distance squared.
*/
    istat = grd_utils_ptr->grd_inverse_distance_average
             (xloc, yloc, zloc, nlist, DistancePower,
              Xspace + Yspace, dsq, &valdsq);

    if (MovingAvgOnly) {
        *value = valdsq;
        return 1;
    }

/*
 *  If the z range of the local data is huge,
 *  float overflows can happen in the plane
 *  fit and evaluation.  In this case, only
 *  an inverse distance value is returned.
 */
    zloc_min = (double)ZlocMin;
    zloc_max = (double)ZlocMax;
    zloc_delta = zloc_max - zloc_min;

    if (zloc_delta > 1.e15) {
        *value = valdsq;
        return 1;
    }

/*
 * If the local data set is all the same z, find a
 * point with a different z and add it to the local
 * data set.
 */
    if (zloc_delta <= Zrange / 2000.0) {
        xt = x0;
        yt = y0;
        zt = zloc[0];
        istat = FindDifferentZValue (&xt, &yt, &zt);
        if (istat == 1) {
            xloc[nlist] = xt - x0;
            yloc[nlist] = yt - y0;
            zloc[nlist] = zt;
            nlist++;
        }
        else {
            *value = valdsq;
            return 1;
        }
    }

/*
    If there is a preferred strike or the anisotropy
    flag is true, then get a strike weighted average also.
*/
    stwgt = 0.0f;
    astat = 0;
    astat = StrikeAverage (xloc, yloc, zloc, nlist, irow, jcol, &valstrike, &stwgt);

    stwgt *= StrikePower;
    if (dist_min < 1.e30) {
        wgt = (CSW_F)(dist_min / dcrit);
        if (wgt < 1.0) wgt = 1.0f;
        wgt *= wgt;
        stwgt *= wgt;
    }

/*
    If a data point is essentially on top of the node,
    use the inverse distance power estimate only.
*/
    if (istat == 999) {
        *value = valdsq;
        return 1;
    }

/*
    Weight the inverse distance estimate along with the other
    estimates.
*/
    if (astat != 1) {
        valstrike = valdsq;
    }

    wgtdsq = DistancePower / 2.0f;

    if (astat == 1) {
        wgtdsq /= StrikePower;
    }

    if (NoisyDataFlag > 1) {
        wgtdsq *= (CSW_F)NoisyDataFlag;
    }

    wgtdsq *= wgtdsq;
    wgtplane = 0.0;
    wgtplane2 = 0.0;

/*
    If the Distance power is more than the default, the grid
    should accentuate high frequency features rather than
    smoothly fit the data.  Thus, for nodes close to data
    and distance power > 2, I use only the inverse distance
    estimate for the grid node.
*/
    if (DistancePower == 4  &&  cp < 1) {
        *value = valdsq;
        return 1;
    }

    if (DistancePower == 6  &&  cp < 1) {
        *value = valdsq;
        return 1;
    }

/*
    If the data are very nearly colinear, special case.
*/
    if (nlist > 2) {
        if (InLineFlag  ||  grd_utils_ptr->grd_colinear_check (xloc, yloc, nlist, tiny, NULL)) {
            if (VerticalFaultFlag == 0) {
                if (TriangulateFlag > 1  &&  TriangulateFlag < 4) {
                    return -2;
                }
            }
            if (FaultedFlag == 1) {
                k = irow * Ncol + jcol;
                if (ClosestFault[k] <= Ncoarse) {
                    *value = (valdsq * wgtdsq + valstrike * stwgt) / (wgtdsq + stwgt);
                    return 1;
                }
            }
            istat = grd_utils_ptr->grd_calc_colinear_plane (xloc, yloc, zloc, nlist,
                                             Xspace, Yspace, &valplane);
            if (istat == -1) {
                *value = (valdsq * wgtdsq + valstrike * stwgt) / (wgtdsq + stwgt);
                return 1;
            }
            wgt = (CSW_F)cp / (CSW_F)Ncoarse;
            if (wgt > 1.0f) wgt = 1.0f;
            wgt *= wgt;
            valplane *= wgt;
            valdsq *= wgtdsq;
            *value = (valdsq + valplane) / (wgtdsq + wgt);
            if (astat == 1) {
                *value = (valdsq + valplane + valstrike * stwgt) / (wgtdsq + wgt + stwgt);
            }
            return 1;
        }
    }

/*
    If less than 3 points are available, use the distance
    square estimate and the strike estimate only.
*/
    if (nlist < 3) {
        *value = valdsq;
        if (astat == 1) {
            *value = (valdsq * wgtdsq + valstrike * stwgt) / (wgtdsq + stwgt);
        }
        return 1;
    }

/*
    Fit a plane to all local points.
*/
    istat = grd_utils_ptr->grd_calc_plane (xloc, yloc, zloc, nlist, coef);

/*
    Evaluate the plane at each point and calculate the error
    between the plane and the actual z value.  If the average
    fit is less than about .1 percent of the input z range,
    the plane value at the node is returned as the node elevation.

    Note that since the grid node is at the origin of the local
    coordinate system, only the coef[0] value is needed to get
    the estimated elevation at the grid node.
*/
    ztiny = Zrange / 1000.f;
    tiny_value = ztiny / 100.0f;
    if (tiny_value < Z_ABSOLUTE_TINY) {
        tiny_value = Z_ABSOLUTE_TINY;
        ztiny = tiny_value * 100.0f;
    }

    ztiny /= Zrange;
    tiny_value_2 = (CSW_F)(sqrt ((double)tiny_value));
    cf = 0;
    if (istat == 1) {

        sum = 0.0f;
        for (i=0; i<nlist; i++) {
            zt = coef[0] + coef[1] * xloc[i] + coef[2] * yloc[i];
            zpt = zloc[i];
            if (zpt > -tiny_value  &&  zpt < tiny_value) {
                zpt = 0.0;
            }
            zt -= zpt;
            if (zt > -tiny_value_2  &&  zt < tiny_value_2) {
                zt = 0.0;
            }
            zt *= zt;
            sum += zt;
        }

        sum /= (CSW_F)nlist;
        sum2 = (CSW_F)sqrt ((double)sum);
        if (Zrange > 0.0f) {
            sum2 /= Zrange;
            sum /= Zrange;
        }

        valplane_pct = sum2;
        if (valplane_pct < 0.01) valplane_pct = 0.01;
        zpct = ZPCT_BASE * nquad;
        if (FaultedFlag == 1) {
            cf = ClosestFault[irow*Ncol+ jcol] + 1;
            zpct2 = 1.0f;
            if (cf <= Ncoarse+1) {
                zpct2 = (CSW_F)(Ncoarse+1) / (CSW_F)cf;
                zpct2 *= zpct2;
            }
            zpct *= zpct2;
        }

        valplane = coef[0];
        if (nlist <= nquad) {
            wgtplane = 1.0f / (CSW_F)(6 - nlist);
        }
        else {
            wgtplane = 1.0f / (CSW_F)(5 - nquad);
        }
        wgtplane *= (CSW_F)plane_only_wgt;
        if (NoisyDataFlag > 1) {
            wgtplane /= (CSW_F)NoisyDataFlag;
        }

// If trying to extrapolate a long way using a plane calculated from a dense
// cluster of points, weight the plane much lower.

        if (cp > Ncoarse * 2) {
            CSW_F  wprat = LocalPointAreaSize / (CSW_F)(cp + 1);
            if (wprat > 1.0) wprat = 1.0;
//TODO            wprat *= wprat;
            wgtplane *= wprat;
        }

        wgtplane *= wgtplane;
        if (sum2 <= zpct  &&  nquad == 4) {
            *value = (valplane * wgtplane + valdsq) / (wgtplane + 1);
            return 1;
        }


// TODO
*value = (valplane * wgtplane + valdsq) / (wgtplane + 1);
return 1;



        if (nlist < 4) {
            wgtplane /= 2.0;
            *value = (valplane * wgtplane + valdsq) / (wgtplane + 1);
            return 1;
        }
        if (ThicknessFlag  &&  nlist < 6) {
            wgtplane *= (nlist - 3);
            *value = (valplane * wgtplane + valdsq) / (wgtplane + 1);
            return 1;
        }
        if (sum2 >= zpct / 10.0) {
            wgtplane2 = zpct / sum2;
            wgtplane2 *= 10.0;
        }
        else {
            wgtplane2 = 10.0;
        }


    }

/*
    If the plane fit calculation failed for the entire data set,
    most likely a singular or near singular matrix condition exists.
    The planes for subsets of the entire set will also fail.

    By far the most likely reason for this is that data are in straight
    line profile distribution.  I cannot think of any other remotely
    reasonable case.  Thus, I set the Extension parameter to PROFILE_EXTENSION
    when this condition occurs.
*/
    else {
        Extension = PROFILE_EXTENSION;
        return -2;
    }

    npmin = 4;

/*
    The plane fit to all of the points was not good enough, so the
    local data set is divided into half planes.  Each half plane is
    defined by a line drawn through the grid node.  The lines start
    with the x axis and have a 45 degree separation between them.
    Thus, the first half plane is all points with y greater than
    the grid node origin.  The second half plane is all points
    where y is greater than x, the third all points where x is less
    than zero, etc.

    If there is a dominant trend in the local data, the plane closest
    to that trend should have the best fitting data.  The plane
    evaluations at the grid nodes are combined using a weighted
    average where the planes with the best fits and with the closest
    data are weighted highest.
*/
    xycoarse = (Xspace + Yspace) * Ncoarse;
    xycoarse /= 2.0f;

    for (ido=0; ido<8; ido++) {

        np = 0;
        HalfPlaneSwitch (ido, xloc, yloc, zloc, dsq, nlist,
                         xp, yp, zp, ep, pdsq, &np);

    /*
        Calculate a plane through the subset of the local data.
    */
        if (np >= npmin) {

        /*
            If the points to be fit are nearly colinear,
            set the weight for this half plane to zero
            and continue with the next half plane.
        */
            if (grd_utils_ptr->grd_colinear_check (xp, yp, np, tiny, NULL)) {
                hpval[ido] = 0.0f;
                avpwgt[ido] = 0.0f;
                avperr[ido] = 0.0f;
                continue;
            }

            istat = grd_utils_ptr->grd_calc_plane (xp, yp, zp, np, coef);

        /*
            If an error occurred calculating the plane,
            go to the next half space.
        */
            if (istat != 1  ||  coef[0] < Zmin - Zrange  ||  coef[0] > Zmax + Zrange) {
                hpval[ido] = 0.0f;
                avpwgt[ido] = 0.0f;
                avperr[ido] = 0.0f;
                continue;
            }

        /*
         * Bug 7957  If a half plane has nearly zero slope (a flat plane) then
         *           do not use it.
         */
            tiny_coef = Zrange * 0.0001f;
            tiny_coef /= (Xspace + Yspace);
            if (coef[1] > -tiny_coef  &&  coef[1] < tiny_coef  &&
                coef[2] > -tiny_coef  &&  coef[2] < tiny_coef) {
                hpval[ido] = 0.0f;
                avpwgt[ido] = 0.0f;
                avperr[ido] = 0.0f;
                continue;
            }

        /*
            Calculate the average squared error for the plane and store it.
            Also find the minimum distance squared and store it.
        */
            sum = 0.0f;
            pdsqmin = 1.e30f;
            for (i=0; i<np; i++) {
                zt = coef[0] + coef[1] * xp[i] + coef[2] * yp[i];
                zpt = zp[i];
                if (zpt > -tiny_value  &&  zpt < tiny_value) {
                    zpt = 0.0;
                }
                zt -= zpt;
                if (zt > -tiny_value_2  &&  zt < tiny_value_2) {
                    zt = 0.0;
                }
                zt *= zt;
                sum += zt;
                if (pdsq[i] < pdsqmin) pdsqmin = pdsq[i];
            }
            if (pdsqmin < tiny) pdsqmin = tiny;

        /*
            If the minimum distance is more than 4 coarse grid intervals,
            do not use the half plane contribution at all.  If less than 4
            coarse intervals, severly lower the weight of more distant
            sets of points.
        */
            wpdsq = (CSW_F)sqrt((double)pdsqmin);
            if (wpdsq < xycoarse / 2.0f) {
                wpdsq = xycoarse / 2.0f;
            }
            wpdsq /= xycoarse;
            if (wpdsq > 4  &&  NoTrendFlag == 0) {
                hpval[ido] = 0.0f;
                avpwgt[ido] = 0.0f;
                avperr[ido] = 0.0f;
                continue;
            }
            if (wpdsq > 8  &&  NoTrendFlag == 1) {
                hpval[ido] = 0.0f;
                avpwgt[ido] = 0.0f;
                avperr[ido] = 0.0f;
                continue;
            }
            wptmp = wpdsq;
            wpdsq *= wpdsq;
            if (wptmp > 2) {
                wpdsq *= wpdsq;
            }
            if (wptmp > 3) {
                wpdsq *= wpdsq;
            }
            if (NoTrendFlag == 1) {
                wpdsq = wptmp;
            }

            zt = sum / (CSW_F)np;

        /*
         * NEWCODE  use the actual error as a percent of the Z range
         *          rather than the squared error.
         */
            zt = (CSW_F)(sqrt((double)zt));
            zt /= Zrange;
            if (zt < ztiny) zt = ztiny;
            zt *= wpdsq;
            avperr[ido] = 1.0f / zt;
            avpwgt[ido] = 1.0f;
            hpval[ido] = coef[0];

        /*
         * bug 9162
         * Make sure there is no underflow from a near zero half
         * plane value.
         */
            if (hpval[ido] > -tiny_value  &&  hpval[ido] < tiny_value) {
                hpval[ido] = 0.0;
            }

            if (astat == 1) {
                px = hpang[ido];
                py = xyspace * (CSW_F)sin ((double)px);
                px = xyspace * (CSW_F)cos ((double)px);
                gpf_perpdsquare (x1, y1, x2, y2, px, py, &st0);
                if (st0 < stiny) st0 = stiny;
                st = st0;
                if (StrikePower > 2) st *= st;
                if (StrikePower > 4) st *= st0;
                st = 1.0f / st;
                avpwgt[ido] = st;
            }

            if (grd_utils_ptr->grd_flat_check (xp, yp, zp, np,
                                Xmin, Ymin, Xmax, Ymax, Zrange)) {
                avperr[ido] /= 10000.f;
            }

        }

    /*
        Too few points to calculate a plane.
    */
        else {
            avperr[ido] = 0.0f;
            avpwgt[ido] = 0.0f;
            hpval[ido] = 0.0f;
        }

    }  /*  end of ido loop through half planes  */

/*
    Reduce weight for plane fits that are skewed from the average.
    This code is included to reduce artifacts that can show up in
    the vicinity of colinear profile data.
*/

/*
    calculate the mean plane fit value for
    planes that currently have non zero weights.
*/
    sum = 0.0f;
    sum2 = 0.0f;
    besterr = 1.e30f;
    for (i=0; i<8; i++) {
        if (avpwgt[i] > 0.0f && avperr[i] > 0.0f) {
            sum ++;
            sum2 += hpval[i];
            zt = 1.0f / avperr[i];
            if (zt < besterr) {
                besterr = zt;
            }
        }
    }

    if (sum <= 0.0f) {
        *value = (valdsq * wgtdsq + valplane * wgtplane2) / (wgtdsq + wgtplane2);
        if (astat == 1) {
            valdsq *= wgtdsq;
            *value = (valdsq + valstrike * stwgt + valplane * wgtplane2) /
                     (wgtdsq + stwgt + wgtplane2);
        }
        return 1;
    }

    mean = sum2 / sum;

/*
    compute the standard deviation.
*/
    sum = 0.0f;
    sum2 = 0.0f;
    for (i=0; i<8; i++) {
        if (avpwgt[i] > 0.0f) {
            sum ++;
            sum4 = hpval[i] - mean;

        /*
         * bug 9162
         * avoid underflows
         */
            if (sum4 < 0.0) sum4 = -sum4;
            if (sum4 < 1.e-15f) {
                sum4 = 0.0f;
            }
            sum4 *= sum4;
            sum2 += sum4;
        }
    }

/*
 * bug 9162
 * make sure at least one sample was used for standard deviation.
 */
    if (sum > 0.0) {
        sum3 = sum2 / sum;
        sum3 = (CSW_F)sqrt ((double)sum3);
    }
    else {
        sum3 = 0.0;
    }

/*
    If the standard deviation is very high relative to the
    range of input data, ignore all the plane fit values and
    return the inverse distance squared estimate only.
*/
    if (sum3 > Zrange / 2.0f) {
        *value = valdsq;
        if (astat == 1) {
            valdsq *= wgtdsq;
            *value = (valdsq + valstrike * stwgt) / (wgtdsq + stwgt);
        }
        if (nquad < 3  &&  cp > Ncoarse / 2) *value = 1.e30f;
        return 1;
    }

    sdev = sum3;

/*
    Reduce the weight of fits that are 1 to 2 standard deviations from
    the mean.  Fits more than 2 standard deviations are weighted zero.
*/
    for (i=0; i<8; i++) {

    /*
     * bug 9162
     * skip if standard deviation is near zero.
     */
        if (sdev <= tiny_value) {
            avperr[i] = 0.0;
            avpwgt[i] = 0.0;
        }
        else {
            if (avpwgt[i] > 0.0f) {
                sum ++;
                sum4 = hpval[i] - mean;
                if (sum4 < 0.0f) sum4 = -sum4;

            /*
             * bug 9162
             * avoid underflows when using sum4 in computations.
             */
                if (sum4 < tiny_value) {
                    sum4 = 0.0;
                }
                if (sum4 > sdev) {
                    sum3 = 1.0f - (sum4 - sdev) / sdev;
                    if (sum3 < tiny_value) sum3 = 0.0f;
                    sum3 *= sum3;
                    if (sum3 < tiny_value) sum3 = 0.0f;
                    sum3 *= sum3;
                    if (sum3 < tiny_value) sum3 = 0.0f;
                    avpwgt[i] *= sum3;
                    avperr[i] *= sum3;
                    if (avperr[i] < tiny_value) avperr[i] = 0.0;
                    if (avpwgt[i] < tiny_value) avpwgt[i] = 0.0;
                }
            }
        }
    }

/*
    Combine the half plane values using a weighted average.
*/
    sum = 0.0f;
    sum2 = 0.0f;
    for (i=0; i<8; i++) {
        sum += avperr[i] * avpwgt[i];
        sum2 += avperr[i] * avpwgt[i] * hpval[i];
    }

/*
    Return elevation estimate that combines the plane fits
    and the inverse distance square estimate.  The plane
    fits are weighted based on the best average error of
    any of the planes used in the fit.  The error is converted
    into a fraction of the z data range, and then scaled so that
    very good fits will make the plane fits overwhelm the
    inverse distance square estimate.
*/
    if (sum < tiny_value) {
        zt = valplane;
    } else {
        zt = sum2 / sum;
    }

    besterr /= (CSW_F)valplane_pct;

    besterr *= 2.0f;
    if (besterr > 1.0f) besterr = 1.0f;
    if (besterr < 0.01f) besterr = 0.01f;
    besterr = 1.0f / besterr;
    if (FaultedFlag && cf <= Ncoarse) {
        if (besterr > 5.0f)
            besterr = 5.0f;
    }

    *value = (valdsq * wgtdsq + valplane * wgtplane + zt * besterr) /
             (wgtdsq + wgtplane + besterr);

    if (astat == 1) {
        if (dist_min < 1.e30) {
            if (dist_min > dcrit) {
                wgt = (CSW_F)(dist_min / dcrit);
                wgt *= wgt;
                besterr /= wgt;
                wgtdsq /= wgt;
            }
        }
        *value = (valdsq * wgtdsq + zt * besterr + valstrike * stwgt + valplane * wgtplane) /
                 (wgtdsq + besterr + stwgt + wgtplane);
    }

    return 1;

}  /*  end of private ProcessLocalPoints function  */







/*
  ****************************************************************

                  H a l f P l a n e S w i t c h

  ****************************************************************

    This function was originally part of the ProcessLocalPoints
    function.  It has been separated out due to an internal error
    reported by the Microsoft Visual C 5.0 compiler.  The error
    seems to be associated with a function too large to optimize.

*/

int CSWGrdCalc::HalfPlaneSwitch
   (int ido,
    CSW_F *xloc, CSW_F *yloc, CSW_F *zloc, CSW_F *dsq,
    int nlist,
    CSW_F *xp, CSW_F *yp, CSW_F *zp, CSW_F *ep, CSW_F *pdsq,
    int *npout)
{
    int                    i, np;

    np = 0;
    for (i=0; i<nlist; i++) {

        switch (ido) {

            case 0:
                if (yloc[i] >= 0.0f) {
                    xp[np] = xloc[i];
                    yp[np] = yloc[i];
                    zp[np] = zloc[i];
                    pdsq[np] = dsq[i];
                    ep[np] = 0.0f;
                    np++;
                }
                break;

            case 1:
                if (yloc[i] >= xloc[i]) {
                    xp[np] = xloc[i];
                    yp[np] = yloc[i];
                    zp[np] = zloc[i];
                    pdsq[np] = dsq[i];
                    ep[np] = 0.0f;
                    np++;
                }
                break;

            case 2:
                if (xloc[i] <= 0.0f) {
                    xp[np] = xloc[i];
                    yp[np] = yloc[i];
                    zp[np] = zloc[i];
                    pdsq[np] = dsq[i];
                    ep[np] = 0.0f;
                    np++;
                }
                break;

            case 3:
                if (yloc[i] <= -xloc[i]) {
                    xp[np] = xloc[i];
                    yp[np] = yloc[i];
                    zp[np] = zloc[i];
                    pdsq[np] = dsq[i];
                    ep[np] = 0.0f;
                    np++;
                }
                break;

            case 4:
                if (yloc[i] <= 0.0f) {
                    xp[np] = xloc[i];
                    yp[np] = yloc[i];
                    zp[np] = zloc[i];
                    pdsq[np] = dsq[i];
                    ep[np] = 0.0f;
                    np++;
                }
                break;

            case 5:
                if (yloc[i] <= xloc[i]) {
                    xp[np] = xloc[i];
                    yp[np] = yloc[i];
                    zp[np] = zloc[i];
                    pdsq[np] = dsq[i];
                    ep[np] = 0.0f;
                    np++;
                }
                break;

            case 6:
                if (xloc[i] >= 0.0f) {
                    xp[np] = xloc[i];
                    yp[np] = yloc[i];
                    zp[np] = zloc[i];
                    pdsq[np] = dsq[i];
                    ep[np] = 0.0f;
                    np++;
                }
                break;

            case 7:
                if (yloc[i] >= -xloc[i]) {
                    xp[np] = xloc[i];
                    yp[np] = yloc[i];
                    zp[np] = zloc[i];
                    pdsq[np] = dsq[i];
                    ep[np] = 0.0f;
                    np++;
                }
                break;

        /*
            This will never happen, but I include it to suppress
            a lint message.
        */
            default:
                break;

        }  /*  end of switch  */

    }  /*  end of loop through local points  */

    *npout = np;

    return 1;

}  /*  end of private HalfPlaneSwitch function  */







/*
  ****************************************************************

                     C a l c L o c a l N o d e s

  ****************************************************************

  Calculate elevations for coarse grid nodes where the node has
  a local data set sufficient for the purpose.

*/

int CSWGrdCalc::CalcLocalNodes (void)
{
    int           i, j, k, j1, *list = ploc_int1, nlist, nquad, start, maxquad,
                  icrit, offset, istat, iend, iendmax, ncmax,
                  nc2, nmin, cf, fcrit, smallmult, ic, jc, kc, tmult;
    CSW_F         value;
    int           do_write;

/*
    Set some private class variables and search range
    variables depending on the point density.
*/
    if (PointDensity < 0.1f) {
        Extension = 5;
    }
    if (PointDensity < 0.01f) {
        Extension = 6;
    }
    Extension += Ndivide;

    if (FastGridFlag) {
        Ndivide /= 2;
        if (Ndivide == 0) Ndivide = 1;
        Extension = 3;
    }

    icrit = Ncoarse * Ndivide + 1;
    iendmax = (Ncol + Nrow) / 4;
    ncmax = (Ncol + Nrow) / 6;
    nc2 = Ncoarse * 2;

    maxquad = Ndata / 100;
    if (maxquad < 4) maxquad = 4;
    if (maxquad > 10) maxquad = 10;

    if (TriangulateFlag > 1) {
        maxquad *= 2;
        iendmax *= 2;
    }

    if (PreferredStrike >= 0.0f  ||  AnisotropyFlag) {
        if (AnisotropyFlag == 0) {
            icrit += Ncoarse / 2;
            if (icrit < 8) icrit = 8;
            maxquad *= StrikePower;
            ncmax *= 4;
            iendmax *= 4;
        }
        else if (AnisotropyFlag == 1) {
            icrit += Ncoarse / 2;
            if (icrit < 8) icrit = 8;
            maxquad *= 2;
            ncmax *= 2;
            iendmax *= 2;
        }
        else {
            icrit += Ncoarse / 4;
            if (icrit < 5) icrit = 5;
        }
    }
    if (ThicknessFlag == 1) {
        maxquad *= 2;
        iendmax *= 2;
    }

    if (StepGridFlag) {
        icrit = Ncol + Nrow;
    }

    fcrit = 0;
    if (FaultedFlag) {
        fcrit = Ncoarse * 2 + 1;
        iendmax = (Ncol + Nrow) / 2;
        maxquad = MAX_QUAD;
    }

    smallmult = SMALL_NUMBER_OF_POINTS / Ndata;
    if (smallmult < 1) smallmult = 1;
    ncmax *= smallmult;
    icrit *= smallmult;
    if (UseShapeGrid == 1) {
        icrit = Ncoarse - 1;
        if (icrit < 1) icrit = 1;
    }

    FaultedCriticalDistance = icrit * (Xspace + Yspace) / 2.0f;

    if (maxquad > MAX_QUAD) maxquad = MAX_QUAD;

/*
 * When TriangulateFlag is 4, a steep fault surface is
 * being extended.  The data will be well distributed
 * and we do not want to calculate nodes very far from
 * the data nor use data points very far from the node.
 */
    if (TriangulateFlag == 4) {
        if (icrit >= Ncoarse) {
            icrit = Ncoarse - 1;
        }
        if (maxquad > 8) {
            maxquad = 8;
        }
    }

/*
    Loop through every Ncoarse'th row and column.
*/
    cf = 100000000;
    for (i=0; i<Nrow; i+=Ncoarse) {

        offset = i * Ncol;

        for (j=0; j<Ncol; j+=Ncoarse) {

            start = ClosestPoint[offset+j];
            if (start > MaxNodeDistance) {
                continue;
            }
            if (start < 0) start = 0;
            if (FaultedFlag) {
                cf = ClosestFault[i*Ncol+j];
            }
            if (start >= icrit) {
                continue;
            }
            cf = cf;

            if (StepGridFlag) {
                iend = start * 2;
            }
            else {
                iend = icrit * Extension / 2;
                if (fcrit > icrit)
                    iend = fcrit * Extension / 2;
                if (PreferredStrike >= 0.0f  &&  AnisotropyFlag == 0) {
                    iend *= StrikePower;
                }
                if (iend > ncmax) iend = ncmax;
                if (iend < nc2) iend = nc2;
            }

            for (;;) {
                nlist = 0;
                LocalPointAreaSize = 0.0;
                CollectLocalPoints (i, j, start, iend * Ndivide,
                                    maxquad, MAX_LOCAL / 2, list, &nlist,
                                    &nquad);
                istat = ProcessLocalPoints (list, nlist, nquad, i, j, start,
                                            &value);
                LocalPointAreaSize = 0.0;

                if (istat == -2) {
                    iend *= 2;
                    nlist = 0;
                    if (iend > iendmax) break;
                    continue;
                }
                break;
            }

            if (nlist < 1) {
                continue;
            }

            Grid[offset+j] = value;

        }

    }




/*
 *  !!!!  debug only
 */
    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        grd_triangle_ptr->grd_WriteXYZGridFile (
            (char *)"pre_extend.xyz",
            Grid, Ncol, Nrow, Ncoarse,
            Xmin, Ymin, Xmax, Ymax);
    }




/*
    If there is a conformal grid, use it to extend the coarse grid.
*/
    if (ConformalFlag == 1  &&  UseShapeGrid == 1) {
        ExtendUsingShapeGrid ();
        return 1;
    }

/*
 * Use the EmptyAreaGrid to fill in empty areas if it is available.
 */
    if (EmptyAreaGrid != NULL) {

        tmult = 3;
        if (TriangulateFlag == 3) {
            tmult = 2;
        }
        for (i=Ncoarse; i<Nrow-1; i+=Ncoarse ) {
            j1 = i * Ncol;
            for (j=Ncoarse; j<Ncol-1; j++) {
                k = j1 + j;
                jc = j / Ncoarse;
                ic = i / Ncoarse;
                kc = ic * Nccol + jc;
                if (Grid[k] > 1.e19) {
                    if (ClosestPoint[k] > Ncoarse * tmult) {
                        Grid[k] = EmptyAreaGrid[kc];
                    }
                }
            }
        }
    }

/*
    Extend the coarse grid by extrapolating local gradients.
*/
    if (DefaultSizeMultiplier < 1) DefaultSizeMultiplier = 1;
    nmin = 4;

// This extension will help fill in holes and bays in the 
// "interior" of the data
    j = (Ncol + Nrow) / Ncoarse;
    j /= 3;
    if (j < Extension) j = Extension;
    if (j < 2) j = 2;
    if (j > 8) j = 8;
    j *= DefaultSizeMultiplier;
    for (i=0; i<j; i++) {
        ExtendCoarseNodes (i, nmin);
    }
    offset = j;

// This extension will help extend the grid out from the edges
// of the data
    nmin = 1;
    j = Extension / 2;
    if (j < 2) j = 2;
    if (ThicknessFlag == 1  ||  FaultedFlag) {
        j *= 2;
    }
    if (j > 4) j = 4;
    if (FaultedFlag) {
        if (j < 8) j = 8;
    }
    for (i=0; i<j; i++) {
        ExtendCoarseNodes (i+offset, nmin);
    }

    return 1;

}  /*  end of private CalcLocalNodes function  */





/*
  ****************************************************************

                A s s i g n T r e n d N o d e s

  ****************************************************************

  Use the trend surface grid to set the elevation of any nodes
  that are very far from data points and that have not yet been
  calculated.  For points already calculated that are far from
  data, the trend value is combined with the current value.

*/

int CSWGrdCalc::AssignTrendNodes (void)
{
    int            i, j, k, offset, coff;
    CSW_F      zt, wgt, ft, fcp, fc2, minusval;

    if (StepGridFlag  ||  UseShapeGrid) {
        return 1;
    }

    ft = (CSW_F)TrendDistance;
    fc2 = Xrange / Xspace + Yrange / Yspace;
    fc2 /= 6.0f;
    if (fc2 < Ncoarse * 2.0f) fc2 = (CSW_F) (Ncoarse * 2.0f);

    if (PreferredStrike >= 0  ||  AnisotropyFlag) {
        fc2 *= StrikePower;
    }

    minusval = Zrange / 20.0f;
    minusval = -minusval;

/*
    If the coarse extension parameter is higher, make the
    trend distance parameter higher also.
*/
    if (Extension > 1  &&  InLineFlag == 0  &&  TrendSmoothFlag == 0) {
        ft = (CSW_F)TrendDistance * Extension;
    }
    if (ft > fc2) ft = fc2;

    if (FaultedFlag) {
        ft *= 400.0f;
    }

    if (NoTrendFlag == 1) {
        ft *= (TriangulateFlag + 1);
    }

    if (ft < 0.01f) ft = 0.01f;
    if (ThicknessFlag == 1) ft *= 4.0f;

    if (InLineFlag == 1) {
        i = Ncol + Nrow;
        i /= 8;
        if (ft > (CSW_F)i) {
            ft = (CSW_F)i;
        }
    }

/*
    Loop through each coarse node, assigning or combining
    trend surface values as needed.
*/
//ft = (CSW_F)Ncoarse * 1.5;
    for (i=0; i<Nrow; i+=Ncoarse) {

        offset = i * Ncol;
        coff = i / Ncoarse * Nccol;

        for (j=0; j<Ncol; j+=Ncoarse) {

            zt = TrendGrid[coff+j/Ncoarse];
            k = j + offset;

        /*
         * if there are only 3 points, every node is from the trend grid
         */
            if (Ndata == 3) {
                Grid[k] = zt;
                continue;
            }

            fcp = (CSW_F)(ClosestPoint[k]);
            if (fcp < 0.0f  &&  Grid[k] < 1.e29f) {
                continue;
            }
            wgt = (fcp - fc2) / fc2;
            if (wgt < 0.0f) wgt = 0.0f;
            if (TriangulateFlag > 0  &&  TriangulateFlag < 4) {
                if (wgt > 1.0) wgt = 1.0;
                wgt /= (TriangulateFlag * TriangulateFlag);
                if (NoTrendFlag == 1) wgt = 1.0;
            }
            wgt *= wgt;
            if (Ndata > 3  &&  TrendSmoothFlag == 0) {
                zt = (zt + Zmean * wgt) / (1.0f + wgt);
            }
            if (ThicknessFlag == 1  ||  OriginalThicknessFlag == 1) {
                if (ZeroCoarse[coff] == 1  &&  zt > minusval * 10.0f) {
                    zt = minusval * 10.0f;
                }
            }

        /*
            Uncalculated nodes are set at 1.e30f,
            so set these with trend estimates if
            they are far enough from data or on a grid
            corner.
        */
            if (Grid[k] > 1.e29f) {
                if (fcp > ft) {
                    Grid[k] = zt;
                }
                else if (k == 0  ||  k == Ncol-1  ||
                         k == Nrow * Ncol - Ncol  ||
                         k == Nrow * Ncol -1) {
                    Grid[k] = zt;
                }
            }

        /*
            Combine the previously calculated value with the
            trend value if a previous value is available.
        */
            else {
                fcp += 1.0f;
fcp *= 2.0;
                wgt = fcp / ft;
                if (TriangulateFlag > 0  &&  TriangulateFlag < 4) {
                    wgt /= (TriangulateFlag * TriangulateFlag);
                }
                wgt *= wgt;
                if (ThicknessFlag == 1  ||  OriginalThicknessFlag == 1) {
                    if (Grid[k] < 0.0  &&  zt > minusval) {
                        zt = minusval;
                    }
                }
                Grid[k] = (Grid[k] + zt * wgt) / (1.0f + wgt);
            }
            if (Ndata <= 3  ||  TrendSmoothFlag == 1) {
                Grid[k] = zt;
            }

        }

    }

    return 1;


}  /*  end of private AssignTrendNodes function  */





/*
  ****************************************************************

                 F i l l I n P e r i m e t e r

  ****************************************************************

    Calculate elevations for all nodes on the edge of the coarse grid that
  are not already calculated.  If a corner point is not yet calculated,
  use the trend fit value.  All other edge nodes are filled in by
  linear interpolation between edge nodes that are calculated.

*/

int CSWGrdCalc::FillInPerimeter (void)
{
    int          i, j, k, j1, j2, flag, janc;
    int          ic, jc, kc;
    CSW_F        z1, z2, pct;

    if (StepGridFlag) {
        return 1;
    }

/*
 * If the EmptyAreaGrid is available, use it as the first
 * estimate of the coarse node value.
 */
    if (EmptyAreaGrid != NULL) {
        for (i=Ncoarse; i<Nrow-1; i+=Ncoarse ) {
            j1 = i * Ncol;
            for (j=Ncoarse; j<Ncol-1; j++) {
                k = j1 + j;
                if (Grid[k] > 1.e19) {
                    jc = j / Ncoarse;
                    ic = i / Ncoarse;
                    kc = ic * Nccol + jc;
                    Grid[k] = EmptyAreaGrid[kc];
                }
            }
        }
    }

/*
    Fill in the bottom edge
*/
    flag = 1;
    if (Grid[Ncoarse] < 1.e29f) flag = 0;
    z1 = Grid[0];
    j = Ncoarse;
    j1 = 0;

    while (j < Ncol) {
        z2 = Grid[j];
        if (flag) {
            if (z2 < 1.e29f) {
                j2 = j;
                pct = (z2 - z1) / (CSW_F)(j2 - j1);
                for (k=j1+Ncoarse; k<j2; k+=Ncoarse) {
                    Grid[k] = z1 + pct * (CSW_F)(k-j1);
                }
                flag = 0;
                z1 = z2;
                j1 = j;
            }
        }
        else {
            if (z2 > 1.e29f) {
                flag = 1;
            }
            else {
                z1 = z2;
                j1 = j;
            }
        }
        j+= Ncoarse;
    }

/*
    Fill in the right edge
*/
    flag = 1;
    if (Grid[(Ncoarse+1)*Ncol-1] < 1.e29f) flag = 0;
    z1 = Grid[Ncol-1];
    j = Ncoarse;
    j1 = 0;

    while (j < Nrow) {
        z2 = Grid[(j+1)*Ncol-1];
        if (flag) {
            if (z2 < 1.e29f) {
                j2 = j;
                pct = (z2 - z1) / (CSW_F)(j2 - j1);
                for (k=j1+Ncoarse; k<j2; k+=Ncoarse) {
                    Grid[(k+1)*Ncol-1] = z1 + pct * (CSW_F)(k-j1);
                }
                flag = 0;
                z1 = z2;
                j1 = j;
            }
        }
        else {
            if (z2 > 1.e29f) {
                flag = 1;
            }
            else {
                z1 = z2;
                j1 = j;
            }
        }
        j+= Ncoarse;
    }

/*
    Fill in the top edge
*/
    flag = 1;
    janc = (Nrow-1)*Ncol;
    if (Grid[janc+Ncoarse] < 1.e29f) flag = 0;
    z1 = Grid[janc];
    j = Ncoarse;
    j1 = 0;

    while (j < Ncol) {
        z2 = Grid[janc+j];
        if (flag) {
            if (z2 < 1.e29f) {
                j2 = j;
                pct = (z2 - z1) / (CSW_F)(j2 - j1);
                for (k=j1+Ncoarse; k<j2; k+=Ncoarse) {
                    Grid[janc+k] = z1 + pct * (CSW_F)(k-j1);
                }
                flag = 0;
                z1 = z2;
                j1 = j;
            }
        }
        else {
            if (z2 > 1.e29f) {
                flag = 1;
            }
            else {
                z1 = z2;
                j1 = j;
            }
        }
        j+= Ncoarse;
    }

/*
    Fill in the left edge
*/
    flag = 1;
    if (Grid[Ncoarse*Ncol] < 1.e29f) flag = 0;
    z1 = Grid[0];
    j = Ncoarse;
    j1 = 0;

    while (j < Nrow) {
        z2 = Grid[Ncol*j];
        if (flag) {
            if (z2 < 1.e29f) {
                j2 = j;
                pct = (z2 - z1) / (CSW_F)(j2 - j1);
                for (k=j1+Ncoarse; k<j2; k+=Ncoarse) {
                    Grid[Ncol*k] = z1 + pct * (CSW_F)(k-j1);
                }
                flag = 0;
                z1 = z2;
                j1 = j;
            }
        }
        else {
            if (z2 > 1.e29f) {
                flag = 1;
            }
            else {
                z1 = z2;
                j1 = j;
            }
        }
        j+= Ncoarse;
    }

    return 1;

}  /*  end of private FillInPerimeter function  */




/*
  ****************************************************************

                S t r i k e S m o o t h G r i d

  ****************************************************************

  Smooth the grid with a 3 by 3 moving average.  The smoothed
  value is combined with the original value depending upon the
  closest data point to the node and the smoothing factor.  A
  radial smoothing estimate is combined with an estimate based
  on the preferred strike specified for the surface.

    This smoothing function is only called for the coarsest grid
  interval.  As the grid is refined, radial smoothing only is
  done by calling SimpleSmoothGrid.

*/

int CSWGrdCalc::StrikeSmoothGrid (void)
{

    int       cp, ismt, i, j, k, offset, off2, ki, kj, astat;
    CSW_F     wt, avg, sum3, sum4, st, x1, y1, x2, y2, x, y;
    CSW_F     st0, stiny, fdum, datafact, fncc, cpmult;
    CSW_F     cpmult2, z0, z1, z2, zt, zt2;
    CSW_F     zgrid;

    AdjustForUnderflow ();

    if (FaultedFlag) {
        StrikeSmoothFaultedGrid ();
        return 1;
    }

    if (StepGridFlag  ||  Ndata < 4  ||  TrendSmoothFlag == 1) {
        return 1;
    }

/*
    Calculate endpoints for a line at the preferred
    strike with the grid node as origin.
*/
    if (PreferredStrike < 0.0f  &&  AnisotropyFlag == 0) {
        return 1;
    }

    wt = Xspace + Yspace;
    stiny = wt / 200.f;
    stiny *= stiny;

/*
    Adjust the weighting of smoothed and original according to
    the current coarse grid interval and the number of data points
    used to calculate the grid.  Do more smoothing for more data points.
*/
    ismt = (int)(SmoothingFactor * 2.0f + .5f);
    if (ismt < 1) ismt = 1;

    datafact = (CSW_F)Ndata / 2000.0f;
    if (datafact < 0.1f) datafact = 0.1f;
    if (SmoothingFactor > 0.0) {
        datafact *= SmoothingFactor;
    }
    if (datafact > 1.0f) {
        datafact = (CSW_F)sqrt ((double)datafact);
        datafact = (CSW_F)sqrt ((double)datafact);
    }

    if (ConformalFlag == 1  &&  UseShapeGrid == 1) {
        if (Ncoarse == NcoarseOrig) {
            ismt = 1;
            datafact = 1.0f;
        }
    }

    if (FastGridFlag) {
        datafact = 10.0f;
    }

    fncc = (CSW_F)Ncoarse + .5f;

    if (Ncoarse == 1) {
        datafact = (CSW_F)Ndata / 2000.0f;
        if (DistancePower > 2) datafact /= 2.0f;
        if (DistancePower > 4) datafact /= 2.0f;
        if (datafact < 0.1f) datafact = 0.1f;
        if (SmoothingFactor > 0.0) {
            datafact *= SmoothingFactor;
        }
        if (datafact > 1.0f) {
            datafact = (CSW_F)sqrt ((double)datafact);
            datafact = (CSW_F)sqrt ((double)datafact);
        }
        if (datafact > 10.0f) datafact = 10.0f;
        fncc = 2.0f;
    }

/*
    Smooth and put the smoothed results into the Gwork1 array
*/
    for (i=0; i<Nrow; i+=Ncoarse) {

        offset = i * Ncol;

        for (j=0; j<Ncol; j+=Ncoarse) {

            k = offset + j;
            astat = CalcStrikeLine (i, j, &x1, &y1, &x2, &y2, &fdum);

        /*
            Use the perpendicular distance from the node neighbors to the
            strike line as the weighting factor for the average of the nodes.
        */
            sum3 = 0.0f;
            sum4 = 0.0f;
            z0 = Grid[k];
            z1 = 1.e30f;
            z2 = -1.e30f;
            for (ki=i-Ncoarse; ki<=i+Ncoarse; ki+=Ncoarse) {
                if (ki<0  ||  ki>=Nrow) {
                    continue;
                }
                off2 = ki*Ncol;
                y = (ki - i) * Yspace;
                y /= Ncoarse;
                for (kj=j-Ncoarse; kj<=j+Ncoarse; kj+=Ncoarse) {
                    if (kj == j  &&  ki == i) continue;
                    if (kj>=0  &&  kj<Ncol) {
                        if (astat == 1) {
                            x = (kj - j) * Xspace;
                            x /= Ncoarse;
                            gpf_perpdsquare (x1, y1, x2, y2, x, y, &st0);
                            if (st0 < stiny) st0 = stiny;
                            st = st0;
                            st = 1.0f / st;
                            if (StrikePower > 2) st *= st;
                            if (StrikePower > 4) st *= st0;
                        }
                        else {
                            st = 1.0f;
                        }
                        zt = Grid[off2+kj];
                        sum3 += Grid[off2+kj] * st;
                        sum4 += st;
                        if (off2+kj != k) {
                            if (zt < z1) z1 = zt;
                            if (zt > z2) z2 = zt;
                        }
                    }
                }
            }

            if (sum4 > 0.0f) {
                avg = sum3 / sum4;
            }
            else {
                avg = Grid[k];
            }

        /*
            Combine the smoothed and original value according to
            the distance from the closest data point at the node.
        */
            cp = ClosestPoint[k];
            cpmult2 = 1.0f;
            if (cp < 2) {
                if (z0 > z2  ||  z0 < z1) {
                    zt = (z1 + z2) / 2.0f;
                    zt2 = z0 - zt;
                    if (zt2 < 0.0f) zt2 = -zt2;
                    cpmult2 = (z2 - z1) / zt2;
                    cpmult2 /= 2.0f;
                    cpmult2 *= cpmult2;
                    cpmult2 *= cpmult2;
                }
            }
            zgrid = Grid[k];
            if (cp < -3) {
                cp = -cp;
                cp -= 4;
                zgrid = Zdata[cp];
                cp = 0;
                cpmult = .01f;
            }
            else {
                cpmult = 1.0f;
                if (cp == -3) {
                    cpmult = .01f;
                }
                else if (cp == -2) {
                    cpmult = .05f;
                }
                else if (cp == -2) {
                    cpmult = .25f;
                }
            }
            wt = (CSW_F)(cp + ismt) / fncc;
            wt *= datafact;
            wt *= wt;
            wt *= cpmult;
            Gwork1[k] = (zgrid + avg * wt) / (1.0f + wt);

        }  /*  end of j loop through columns  */

    }  /*  end of i loop through rows  */

/*
    Copy the smoothed results back to the Grid array.
*/
    for (i=0; i<Nrow; i+=Ncoarse) {
        offset = i * Ncol;
        for (j=0; j<Ncol; j+=Ncoarse) {
            k = offset + j;
            Grid[k] = Gwork1[k];
        }
    }

    return 1;

}  /*  end of private StrikeSmoothGrid function  */





/*
  ****************************************************************

                    F i l l I n C o a r s e

  ****************************************************************

    Fill in any coarse grid nodes that still have no values by
  interpolating linearly in both the x and y directions between
  the closest nodes that do have values.

*/

int CSWGrdCalc::FillInCoarse (void)
{
    int          i, j, k, j1, j2, flag, janc;
    int          ic, jc, kc;
    CSW_F        z1, z2, zt, pct;

    if (StepGridFlag) {
        return 1;
    }

/*
 * If the EmptyAreaGrid is available, use it as the first
 * estimate of the coarse node value.
 */
    if (EmptyAreaGrid != NULL) {
        for (i=Ncoarse; i<Nrow-1; i+=Ncoarse ) {
            j1 = i * Ncol;
            for (j=Ncoarse; j<Ncol-1; j++) {
                k = j1 + j;
                if (Grid[k] > 1.e19) {
                    jc = j / Ncoarse;
                    ic = i / Ncoarse;
                    kc = ic * Nccol + jc;
                    Grid[k] = EmptyAreaGrid[kc];
                }
            }
        }
    }

/*
    Fill in a row at a time by interpolating linearly
    between good nodes in the row.  These are stored
    in Gwork1 to be averaged with the column at a time
    interpolation.
*/
    for (i=Ncoarse; i<Nrow-1; i+=Ncoarse) {

        janc = i * Ncol;
        flag = 1;
        if (Grid[janc+Ncoarse] < 1.e29f) flag = 0;
        z1 = Grid[janc];
        j = Ncoarse;
        j1 = 0;

        while (j < Ncol) {
            z2 = Grid[janc+j];
            if (flag) {
                if (z2 < 1.e29f) {
                    j2 = j;
                    pct = (z2 - z1) / (CSW_F)(j2 - j1);
                    for (k=j1+Ncoarse; k<j2; k+=Ncoarse) {
                        Gwork1[janc+k] = z1 + pct * (CSW_F)(k-j1);
                    }
                    flag = 0;
                    z1 = z2;
                    j1 = j;
                }
            }
            else {
                if (z2 > 1.e29f) {
                    flag = 1;
                }
                else {
                    z1 = z2;
                    j1 = j;
                }
            }
            j+= Ncoarse;
        }
    }

/*
    Interpolate a column at a time and average with the
    previous row interpolations.
*/
    for (i=Ncoarse; i<Ncol-1; i+=Ncoarse) {

        flag = 1;
        if (Grid[Ncol*Ncoarse+i] < 1.e29f) flag = 0;
        z1 = Grid[i];
        j = Ncoarse;
        j1 = 0;

        while (j < Nrow) {
            z2 = Grid[Ncol*j+i];
            if (flag) {
                if (z2 < 1.e29f) {
                    j2 = j;
                    pct = (z2 - z1) / (CSW_F)(j2 - j1);
                    for (k=j1+Ncoarse; k<j2; k+=Ncoarse) {
                        zt = z1 + pct * (CSW_F)(k-j1);
                        Grid[Ncol*k+i] = (zt + Gwork1[Ncol*k+i]) / 2.0f;
                    }
                    flag = 0;
                    z1 = z2;
                    j1 = j;
                }
            }
            else {
                if (z2 > 1.e29f) {
                    flag = 1;
                }
                else {
                    z1 = z2;
                    j1 = j;
                }
            }
            j+= Ncoarse;
        }
    }

    return 1;


}  /*  end of private FillInCoarse function  */






/*
  ****************************************************************

                  C a l c E r r o r A t N o d e

  ****************************************************************

    Given a list of points near the node, do either a
  bilinear estimate of the z value at each point.  The estimate is
  subtracted from the actual z value to get the error.  The errors
  are then averaged with inverse distance squared weighting to get
  the error at the node.

*/

int CSWGrdCalc::CalcErrorAtNode
           (int irow, int jcol, int *list, int nlist,
            CSW_F *error)
{
    int         i, istat;
    CSW_F       x0, y0, *x = ploc_f1, *y = ploc_f2,
                *z = ploc_f3, *z2 = ploc_f4, locerror;
    double      dmin, dist, wgt;
    int         closest;

/*
    Initialize some local stuff to suppress lint messages
*/
    memset (z, 0, MAX_LOCAL * sizeof(CSW_F));

/*
    put points into local arrays
*/
    for (i=0; i<nlist; i++) {
        x[i] = Xdata[list[i]];
        y[i] = Ydata[list[i]];
        z[i] = Zdata[list[i]];
    }

/*
    Do a bilinear interpolation at the local points.
*/
    grd_utils_ptr->grd_bilin_interp (x, y, z2, nlist,
                      Grid, Ncol, Nrow, Ncoarse,
                      Xmin, Ymin, Xmax, Ymax);

/*
    reorigin points at the grid node and
    calculate the errors in elevation
*/
    dmin = 1.e30;
    closest = -1;
    x0 = jcol * Xspace + Xmin;
    y0 = irow * Yspace + Ymin;
    for (i=0; i<nlist; i++) {
        x[i] -= x0;
        y[i] -= y0;
        if (z2[i] > 1.e20f  ||  z2[i] < -1.e20f) {
            z2[i] = z[i];
        }
        if (ThicknessFlag == 1) {
            if (z[i] < Ztiny  &&  z2[i] < Ztiny) {
                z2[i] = z[i];
            }
            else if (z[i] < Ztiny) {
                z2[i] *= 2.0f;
            }
        }
        z[i] -= z2[i];
        dist = x[i] * x[i] + y[i] * y[i];
        dist = sqrt (dist);
        if (dist < dmin) {
            dmin = dist;
            closest = i;
        }
    }

/*
    Do inverse distance squared average of errors at points.
*/
    istat = grd_utils_ptr->grd_inverse_distance_average
                   (x, y, z, nlist, DistancePower,
                    Xspace + Yspace, (CSW_F *)NULL, &locerror);

/*
 * Combine the inverse distance error with the error of the closest
 * point to the node, depending upon the closest distance.
 */
    if (closest < 0) {
        *error = locerror;
    }
    else {
        wgt = dmin / (Xspace + Yspace);
        wgt /= 2.0;
        if (wgt < 0.5) {
            *error = z[closest];
            istat = 998;
        }
        else {
            wgt = 1.0 / wgt;
            *error = (CSW_F)((locerror + wgt * z[closest]) / (1.0 + wgt));
        }
    }

    return istat;

}  /*  end of private CalcErrorAtNode function  */






/*
  ****************************************************************

                    C h e c k F o r B i c u b

  ****************************************************************

    Return 1 if bicubic interpolation is needed inside a grid cell
  or zero if bilinear will suffice.  If the center elevation of the
  cell is nearly the same from averaging the cell corners or from
  averaging the corners of the 3 by 3 cell group that is centered
  on the specified cell, then the surface is planar enough to use
  bilinear interpolation instead of bicubic.

*/

int CSWGrdCalc::CheckForBicub (int i, int j)
{
    int                  i1, i2, j1, j2;
    double               tiny, z1, z2, a1, a2, dz, zcrit;

    tiny = (double)Z_ABSOLUTE_TINY;
    zcrit = Zrange / BicubCutoff;
    zcrit *= (double)DistancePower / 2.0;
    zcrit *= zcrit;

    if (zcrit < tiny) return 0;

    if (DistancePower > 4) {
        return 0;
    }

/*
    indices of 3x3 subgrid centered on the specified cell
*/
    i1 = i - Ncoarse;
    if (i1 < 0) i1 = 0;
    j1 = j - Ncoarse;
    if (j1 < 0) j1 = 0;
    i2 = i1 + 3 * Ncoarse;
    if (i2 >= Nrow) {
        i2 = Nrow - 1;
        i1 = i2 - 3 * Ncoarse;
    }
    j2 = j1 + 3 * Ncoarse;
    if (j2 >= Ncol) {
        j2 = Ncol - 1;
        j1 = j2 - 3 * Ncoarse;
    }
    if (j1 < 0  ||  i1 < 0) {
        return 0;
    }

/*
    Compare center elevations based on the lower left
    to upper right diagonal.
*/
    z1 = Grid[i1 * Ncol + j1];
    z2 = Grid[i2 * Ncol + j2];
    a1 = (z1 + z2) / 2.0;
    z1 = Grid[(i1+Ncoarse)*Ncol+j1+Ncoarse];
    z2 = Grid[(i1+2*Ncoarse)*Ncol+j2-Ncoarse];
    a2 = (z1 + z2) / 2.0;
    dz = a1 - a2;
    dz *= dz;
    if (dz < tiny) dz = 0.0;
    if (dz > zcrit) return 1;

/*
    Compare center elevations based on the upper left
    to lower right diagonal.
*/
    z1 = Grid[i2 * Ncol + j1];
    z2 = Grid[i1 * Ncol + j2];
    a1 = (z1 + z2) / 2.0f;
    z1 = Grid[(i2-Ncoarse)*Ncol+j1+Ncoarse];
    z2 = Grid[(i1+Ncoarse)*Ncol+j2-Ncoarse];
    a2 = (z1 + z2) / 2.0f;
    dz = a1 - a2;
    dz *= dz;
    if (dz < tiny) dz = 0.0;
    if (dz > zcrit) return 1;

/*
    If neither diagonal had a large difference,
    return zero.
*/
    return 0;

}  /*  end of private CheckForBicub function  */





/*
  ****************************************************************

                   C a l c E r r o r G r i d

  ****************************************************************

  Calculate the error at each coarse grid node.  The error is the
  inverse distance weighted average of the errors between the actual
  and interpolated values at data points near the node.

  In order to distribute error smoothly away from a node, the error
  is multiplied by a scaling factor which is a function of closest
  data point to the node.  These overcorrections are smoothed out
  using a simple 3 by 3 moving average.

*/

int CSWGrdCalc::CalcErrorGrid (int iter)
{
    int            i, j, offset, cp, nquad, istat,
                   *list = ploc_int1, nlist, nc2, nc22;
    CSW_F          err, emultmax, wgt;
    static const CSW_F   embase[] = {3.0f, 2.5f, 2.0f, 1.5f, 1.0f, 1.0f};

    if (iter > 5) iter = 5;
    if (iter < 0) iter = 0;

    nc2 = Ncoarse / 2 + 1;
    nc22 = nc2;
    emultmax = embase[iter] - .1f * SmoothingFactor;
    if (DistancePower == 4) emultmax -= .5f;
    if (DistancePower == 6) emultmax -= 1.5f;
    if (emultmax < 1.0f) emultmax = 1.0f;
    if (Ncoarse == 1) nc22 = 2;

    if (MedianFlag == 1) {
        nc2 = 0;
    }

/*
    Calculate the errors at the nodes.
*/
    ErrorSearch = 1;
    StrikeSearch = 0;
    for (i=0; i<Nrow; i+= Ncoarse) {

        offset = i * Ncol;

        for (j=0; j<Ncol; j+=Ncoarse) {

        /*
            Only nodes with data points relatively close
            have errors calculated.  The rest of the nodes
            have errors set to zero.
        */
            cp = ClosestPoint[offset+j];
            if (cp < 0) cp = 0;
            err = 0.0f;
            wgt = 1.0f;
            if (cp <= nc2) {
                CollectLocalPoints (i, j, cp, nc2,
                                    8, MAX_LOCAL / 2,
                                    list, &nlist, &nquad);
                if (nlist > 0) {
                    if (FaultedFlag  &&  grd_fault_ptr->grd_fault_check_needed (i, j, nc2+1)) {
                        istat = CalcFaultedErrorAtNode (i, j, list, nlist, &err);
                    }
                    else {
                        istat = CalcErrorAtNode (i, j, list, nlist, &err);
                    }
                    if (istat == 999) {
                        wgt = 0.0f;
                    }
                    else if (istat == 998) {
                        wgt = 1.0f;
                    }
                    else {
                        wgt = (CSW_F)(cp + 1) / (CSW_F)nc22;
                        wgt *= wgt;
                        wgt = 1.0f / wgt;
                        if (wgt > emultmax) wgt = emultmax;
                        if (wgt < 1.0f) wgt = 1.0f;
                    }
                }
                else {
                    wgt = 0.0f;
                }
            }

            Gwork1[offset + j] = err * wgt;

        }
    }
    StrikeSearch = 1;
    ErrorSearch = 0;

/*
    Adjust for control points if needed.
*/
    AdjustForControlPoints ();

/*
    Smooth the error grid.  This distributes the overshoot of
    errors assigned above by the wgt factor out away from the
    node.  The nodes originally assigned zero error values
    will now get part of the neighboring non zero error
    correction.  This provides a smoother transition from
    areas of sparse data to areas with good data coverage.
*/

    SmoothErrors ();

/*
    Add the smoothed errors (Gwork2) to the original
    grid values.
*/
    for (i=0; i<Nrow; i+=Ncoarse) {
        offset = i * Ncol;
        for (j=0; j<Ncol; j+= Ncoarse) {
            Grid[offset+j] += Gwork2[offset+j];
        }
    }

    return 1;

}  /*  end of private CalcErrorGrid function  */





/*
  **************************************************************************

                         M a s k B a d N o d e s

  **************************************************************************

    Set the NodeMask array to 1 for nodes outside the data area and to 2
  for nodes inside but a long way from the closest data point.  The options
  GRD_OUTSIDE_MARGIN and GRD_INSIDE_MARGIN can be used to control the distances
  for each definition of bad nodes.

*/

int CSWGrdCalc::MaskBadNodes (void)
{
    int             i, j, k, start, end, offset, cp;

    if (Ndata == 3) {
        memset (NodeMask, 0, Ncol*Nrow*sizeof(char));
        return 1;
    }

    if (NumControlPoints > 0) {
        UseControlInDataTable = 1;
        SetupDataTable (0, 0);
        SetupDistanceTable ();
        UseControlInDataTable = 0;
    }

/*
    Setup the Left and Right arrays with the column of the first and last
    points encountered in each row.  This basically produces a blocky
    boundary around the data points.
*/
    for (i=0; i<Nrow; i++) {
        offset = i * Ncol;
        j = 0;
        for (;;) {
            cp = ClosestPoint[offset+j];
            if (cp < 0) cp = 0;
            if (cp <= OutsideBoundaryMargin  ||
                j == Ncol-1) {
                Left[i] = j;
                break;
            }
            j++;
        }

        if (j == Ncol-1) {
            Right[i] = 0;
        }
        else {
            j = Ncol - 1;
            for (;;) {
                cp = ClosestPoint[offset+j];
                if (cp < 0) cp = 0;
                if (cp <= OutsideBoundaryMargin  ||
                    j == 0) {
                    Right[i] = j;
                    break;
                }
                j--;
            }
        }
    }

/*
    Find the first and last rows that are not completely outside.
*/
    start = end = -1;
    for (i=0; i<Nrow; i++) {
        if (Left[i] < Ncol-1) {
            start = i;
            break;
        }
    }

    if (start < 0) return -1;

    for (i=Nrow-1; i>=0; i--) {
        if (Left[i] < Ncol-1) {
            end = i;
            break;
        }
    }

    if (end < 0) return -1;
    if (end <= start) return -1;

/*
    Recursively divide the boundaries so that the dividing
    point of each segment is the point farthest from the line
    that connects the end points of the segment.
*/
    DivideLeftBoundary (start, end);
    DivideRightBoundary (start, end);

/*
    Fill in the NodeMask to reflect points outside the boundary.
*/
    memset (NodeMask, 0, Ncol*Nrow*sizeof(char));

    for (i=0; i<Nrow; i++) {
        offset = Ncol * i;
        for (j=0; j<=Left[i]; j++) {
            NodeMask[offset+j] = 1;
        }
        for (j=Right[i]; j<Ncol; j++) {
            NodeMask[offset+j] = 1;
        }
    }

/*
    Flag the points inside the array that are too far from control.
*/
    for (i=0; i<Nrow; i++) {
        offset = i * Ncol;
        for (j=0; j<Ncol; j++) {
            k = offset + j;
            cp = ClosestPoint[k];
            if (cp < 0) cp = 0;
            if (NodeMask[k] == 0  &&  cp > InsideBoundaryMargin) {
                NodeMask[k] = 2;
            }
        }
    }

    return 1;

}  /*  end of private MaskBadNodes function  */






/*
  ******************************************************************************

                    D i v i d e R i g h t B o u n d a r y

  ******************************************************************************

    Recursively divide the right boundary with the dividing point of each segment
  being the point to the right of the line connecting the segment end points that
  is the farthest point to the right of the line.

*/

int CSWGrdCalc::DivideRightBoundary (int start, int end)
{
    int       i, k, mid, jmid, j1, j2;
    CSW_F     fk, dj;

    j1 = Right[start];
    j2 = Right[end];
    dj = (CSW_F)(j2 - j1) / (CSW_F)(end - start);

    mid = 0;
    jmid = -1;
    for (i=start+1; i<end; i++) {
        if (Right[i] > j1+dj*(i-start)) {
            k = (int) (Right[i] - (j1 + dj * (i - start)));
            if (k > mid) {
                mid = k;
                jmid = i;
            }
        }
    }

/*
    If a dividing point was found, recursively divide each segment
    created by the division.
*/
    if (jmid != -1) {
        DivideRightBoundary (start, jmid);
        DivideRightBoundary (jmid, end);
    }

/*
    No dividing point was found, so this segment forms part of
    the right side boundary.  Set the Right array where it is
    crossed by this segment.
*/
    else {
        for (i=start+1; i<end; i++) {
            fk = dj * (CSW_F)(i - start) + .5f;
            k = (int)fk;;
            Right[i] = j1 + k;
        }
    }

    return 1;

}  /*  end of private DivideRightBoundary function  */






/*
  ******************************************************************************

                    D i v i d e L e f t B o u n d a r y

  ******************************************************************************

    Recursively divide the left boundary with the dividing point of each segment
  being the point to the left of the line connecting the segment end points that
  is the farthest left of the line.

*/

int CSWGrdCalc::DivideLeftBoundary (int start, int end)
{
    int       i, k, mid, jmid, j1, j2;
    CSW_F     fk, dj;

    j1 = Left[start];
    j2 = Left[end];
    dj = (CSW_F)(j2 - j1) / (CSW_F)(end - start);

    mid = 0;
    jmid = -1;
    for (i=start+1; i<end; i++) {
        if (Left[i] < j1+dj*(i-start)) {
            k = (int) ((j1 + dj * (i - start)) - Left[i]);
            if (k > mid) {
                mid = k;
                jmid = i;
            }
        }
    }

/*
    If a dividing point was found, recursively divide each segment
    created by the division.
*/
    if (jmid != -1) {
        DivideLeftBoundary (start, jmid);
        DivideLeftBoundary (jmid, end);
    }

/*
    No dividing point was found, so this segment forms part of
    the right side boundary.  Set the Right array where it is
    crossed by this segment.
*/
    else {
        for (i=start+1; i<end; i++) {
            fk = (CSW_F)dj * (CSW_F)(i - start) + .5f;
            k = (int)fk;;
            Left[i] = j1 + k;
        }
    }

    return 1;

}  /*  end of private DivideLeftBoundary function  */




/*
  ****************************************************************

                 S m o o t h E r r o r s

  ****************************************************************

      Smooth the error grid (Gwork1) and output to Gwork2.
  This is a simple 3 by 3 moving average.  The smoothing
  factor and strike direction are ignored.

*/

int CSWGrdCalc::SmoothErrors (void)
{
    int       i, j, k, offset, off2, ki, kj, mult, cp;
    int       ndo, ido, cpmax;
    CSW_F     sum, sum2, cwgt, cpwgt, emin, emax, tfact;
    int       do_write;

    if (FaultedFlag) {
        SmoothFaultedErrors ();
        return 1;
    }

    if (StepGridFlag) {
        return 1;
    }

/*
    Smooth the errors less if the distance power is greater than 2.
*/
    cwgt = 0.0f;
    if (DistancePower > 2) {
        cwgt = (CSW_F) (DistancePower / 2.0f);
        cwgt *= cwgt;
    }

    ndo = 1;
    if (NumControlPoints > 0) {
        ndo = 3;
    }
    if (Ncoarse == 1  &&  Ncoarse != NcoarseOrig) {
        ndo = 1;
    }

    cpmax = 100000000;
    if (MedianFlag == 1) {
        cpmax = 2;
    }

    tfact = 1.0;

/*
    Smooth and put the smoothed results into the Gwork2 array
*/
    for (ido = 0; ido < ndo; ido++) {
        mult = 1;
        for (i=0; i<Nrow; i+=Ncoarse) {

            offset = i * Ncol;

            for (j=0; j<Ncol; j+=Ncoarse) {

        /*
            Sum the values of the immediate neighbors of the node.
        */
                k = offset + j;
                cp = ClosestPoint[k];
                if (cp < 0) {
                    Gwork2[k] = Gwork1[k];
                    continue;
                }
                if (cp > cpmax) {
                    Gwork2[k] = 0.0f;
                    continue;
                }
                if (ido > 1) {
                    if (cp <= Ncoarse) {
                        continue;
                    }
                }
                if (NumControlPoints > 0) {
                    mult = cp / Ncoarse / 4;
                    if (mult < 1) mult = 1;
                    if (mult > 3) mult = 3;
                }
                sum = 0.0f;
                sum2 = 0.0f;
                cpwgt = (CSW_F)(cp + 1) / (CSW_F)Ncoarse;
                cpwgt /= tfact;
                cpwgt *= cpwgt;
                if (cwgt > 0.0) {
                    cpwgt /= cwgt;
                }

            /*
             * Do not use the highest or lowest neighbor.
             */
                emin = 1.e30f;
                emax = -1.e30f;
                for (ki=i-Ncoarse*mult; ki<=i+Ncoarse*mult; ki+=Ncoarse) {
                    if (ki<0  ||  ki>=Nrow) {
                        continue;
                    }
                    off2 = ki*Ncol;
                    for (kj=j-Ncoarse*mult; kj<=j+Ncoarse*mult; kj+=Ncoarse) {
                        if (kj>=0  &&  kj<Ncol) {
                            CSW_F zt;
                            zt = Gwork1[off2+kj];
                            if (zt < emin) emin = zt;
                            if (zt > emax) emax = zt;
                            sum += zt;
                            sum2 += 1.0f;
                        }
                    }
                }

                if (sum < TinySum  &&  sum > -TinySum) {
                    sum = 0.0;
                }
                if (sum2 > 2) {
                    sum -= emin;
                    sum -= emax;
                    sum2 -= 2;
                }
                Gwork2[k] = (Gwork1[k] + cpwgt * sum / sum2) / (1.0f + cpwgt);

            }

        }

        do_write = csw_GetDoWrite ();
        if (do_write == 1) {
            grd_triangle_ptr->grd_WriteXYZGridFile (
                (char *)"gwork2.xyz",
                Gwork2, Ncol, Nrow, Ncoarse,
                Xmin, Ymin, Xmax, Ymax);
        }


        if (ido < ndo - 1) {
            memcpy (Gwork1, Gwork2, Ncol * Nrow * sizeof(CSW_F));
        }

    }

    return 1;

}  /*  end of private SmoothErrors function  */





/*
  ****************************************************************

                I t e r a t e T o F i n a l G r i d

  ****************************************************************

    Calculate the errors at the coarse grid spacing, adjust the coarse
  grid and recalculate the errors.  This is repeated up to 4 times at
  a given coarse grid interval, or until the error gets much better or
  a little worse.  Divide the coarse interval by 2, interpolate at these
  points, then repeat the error correction process.

    This is repeated until the coarse interval is one.

*/

int CSWGrdCalc::IterateToFinalGrid (void)
{
    int       i, lastpass, maxiter;
    CSW_F     smsave, maxerr, avgerr, lastmax, lastavg, tinyavg, tinymax;
    int       do_write;

    if (PointNodeRatio > 20  &&  Ncoarse == 1) {
        return 1;
    }

/*
    Refine the grid until the spacing is 1.
*/
    lastavg = 1.e30f;
    lastmax = 1.e30f;
    tinymax = 0.0f;
    avgerr = 0.0f;
    maxerr = 0.0f;

    lastpass = 0;

    maxiter = MAX_ITER * 2;

    if (PointNodeRatio > 2.0) {
        maxiter /= 2;
    }
    if (PointNodeRatio > 5.0) {
        maxiter /= 2;
    }
    if (PointNodeRatio > 10.0) {
        maxiter /= 2;
    }

    if (maxiter < 1) maxiter = 1;

    for (;;) {

    /*
        For each coarse level, clip the grid to the min and max if needed
    */
        ClipToMinMax ();

    /*
        For each grid spacing, iterate through error correction loops
    */
        i = 0;

        FaultedErrorMax = (Zmax - Zmin) / 20.0f;

        while (i < maxiter) {

            FaultedErrorMax *= .8f;

            if (FastGridFlag  &&  Ncoarse < 4  &&  lastavg < tinymax) {
                break;
            }

        /*
            Calculate the errors at points near nodes and use
            them to create an error grid.  The error grid is
            smoothed and then added to the elevation grid.
        */
            NumControlAdjust = 0;
            CoincidentDistance = (Xspace + Yspace) / 20.0f;
            CalcErrorGrid (i);
            CoincidentDistance = 0.0;

        /*
            Calculate errors at all data points based
            on the current elevation grid.
        */
            maxerr = 1.e30f;
            avgerr = 1.e30f;
            if (FaultedFlag) {
                FaultedDataPointErrors (&maxerr, &avgerr);
            }
            else {
                DataPointErrors (&maxerr, &avgerr);
            }

            if (FastGridFlag) {
                break;
            }

        /*
            Set "good enough" error values for this coarseness.
        */
            tinyavg = Zrange / AVG_ERROR_DIVISOR / 8.0f;
            tinymax = Zrange / MAX_ERROR_DIVISOR / 8.0f;

            if (FastGridFlag) {
                tinyavg *= 10.f;
                tinymax *= 10.f;
            }

        /*
            If the errors are better than the target errors,
            go to the next grid spacing level.
        */
            if (maxerr < tinymax  &&  avgerr < tinyavg  &&  NumControlAdjust == 0) {
                break;
            }

            if (i > 0  &&  NumControlAdjust == 0) {

            /*
                If the errors are considerably better than the
                previous iteration, go to the next grid spacing level.
            */
                if (avgerr < lastavg / 100.0f  &&
                    maxerr < lastmax / 100.0f) {
                    break;
                }

            }

            lastmax = 2.0f * maxerr;
            lastavg = avgerr;
            memcpy ((char *)Gwork3, (char *)Grid, Ncol*Nrow*sizeof(CSW_F));

            i++;

            if (lastavg > 0.0f  &&
                lastavg < Zrange / AVG_ERROR_DIVISOR / 2.0f) i = maxiter;
            if (lastmax > 0.0f  &&
                lastmax < Zrange / MAX_ERROR_DIVISOR / 2.0f) i = maxiter;

        }  /*  end of error correction iterations  */

        do_write = csw_GetDoWrite ();
        if (do_write == 1) {
            grd_triangle_ptr->grd_WriteXYZGridFile (
                (char *)"iter.xyz",
                Grid, Ncol, Nrow, Ncoarse,
                Xmin, Ymin, Xmax, Ymax);
        }

    /*
        If a conformable surface is being used, adjust the grid
        to be conformable with the conformable surface before
        refining to the next level or exiting in the case of
        alrady being at the finest level.
    */
        if (ConformalFlag == 1  &&  UseShapeGrid == 1) {
            AdjustForShapeGrid ();
        }

        if (lastpass == 1) break;

    /*
        Smooth before refining.
    */
        smsave = SmoothingFactor;
        if (DistancePower == 2) {
            SmoothingFactor = 6.0f;
        }
        else {
            SmoothingFactor = 12.0f / (CSW_F)(DistancePower);
        }
        if (DistancePower == 6) SmoothingFactor = 1.0f;
        if (PreferredStrike >= 0  ||  AnisotropyFlag) {
            StrikeSmoothGrid ();
        }
        else {
            SimpleSmoothGrid ();
        }
        if (ConformalFlag == 1  &&  UseShapeGrid == 1) {
            if (PreferredStrike >= 0  ||  AnisotropyFlag) {
                StrikeSmoothGrid ();
            }
            else {
                SimpleSmoothGrid ();
            }
        }
        SmoothingFactor = smsave;

        if (do_write == 1) {
            grd_triangle_ptr->grd_WriteXYZGridFile (
                (char *)"smoothiter.xyz",
                Grid, Ncol, Nrow, Ncoarse,
                Xmin, Ymin, Xmax, Ymax);
        }

    /*
        Interpolate the grid at half the current coarse interval.
    */
        if (Ncoarse > 1) {
            if (FaultedFlag) {
                RefineFaultedGrid ();
            }
            else {
                RefineGrid ();
            }
        }
        Ncoarse /= 2;
        if (Ncoarse < 1) {
            Ncoarse = 1;
            lastpass = 1;
        }

        if (do_write == 1) {
            grd_triangle_ptr->grd_WriteXYZGridFile (
                (char *)"refineiter.xyz",
                Grid, Ncol, Nrow, Ncoarse,
                Xmin, Ymin, Xmax, Ymax);
        }

    }  /*  end of grid refinement iterations  */

    AvgError = avgerr;
    MaxError = maxerr;

    return 1;

}  /*  end of private IterateToFinalGrid function  */





/*
  ****************************************************************

                 D a t a P o i n t E r r o r s

  ****************************************************************

    Calculate the errors between data points and the estimated
  elevation at the point for the current coarse grid.  The max
  error and average error are returned.

*/

int CSWGrdCalc::DataPointErrors
        (CSW_F *maxerrptr, CSW_F *avgerrptr)
{
    int            k, n;
    CSW_F          maxerr, avgerr, zerr;

    maxerr = 0.0f;
    avgerr = 0.0f;
    n = 0;

/*
    loop through all data points.
*/
    for (k=0; k<Ndata; k++) {

    /*
        ignore if outside of the grid
    */
        if (Ydata[k] < Ymin  ||  Ydata[k] > Ymax  ||
            Xdata[k] < Xmin  ||  Xdata[k] > Xmax) {
            Zerr[k] = 1.e30f;
            continue;
        }

    /*
        do bilinear interpolation
    */
        grd_utils_ptr->grd_bilin_interp (Xdata+k, Ydata+k, Zerr+k, 1,
                          Grid, Ncol, Nrow, Ncoarse,
                          Xmin, Ymin, Xmax, Ymax);

        Zerr[k] -= Zdata[k];
        if (ThicknessFlag == 1  &&  Zdata[k] < Ztiny) {
            Zerr[k] = 0.0f;
        }
        zerr = Zerr[k];
        if (zerr < 0.0f) zerr = -zerr;
        if (zerr > maxerr) {
            maxerr = zerr;
        }

        avgerr += zerr;
        n++;

    }

/*
    return the average and max.
*/
    *avgerrptr = avgerr / (CSW_F)n;
    *maxerrptr = maxerr;

    return 1;

}  /*  end of private DataPointErrors function  */






/*
  ****************************************************************

                       R e f i n e G r i d

  ****************************************************************

    Interpolate the current coarse grid at half the coarse spacing
  to produce the next coarse grid.  Bicubic or bilinear interpolation
  is used as appropriate.

*/

int CSWGrdCalc::RefineGrid (void)
{
    int      istat, i, j, k, n, offset, nrlast, nc2, off2;
    CSW_F    x0, y0, xsp, ysp, xsp2, ysp2;
    int      bcflag;

    CSW_F    x[10], y[10], z[10];

/*
    set some counting constants
*/
    xsp = Xspace * Ncoarse;
    ysp = Yspace * Ncoarse;
    xsp2 = xsp / 2.0f;
    ysp2 = ysp / 2.0f;
    nrlast = Nrow - Ncoarse - 1;
    nc2 = Ncoarse / 2;
    off2 = nc2 * Ncol;

    bcflag = 1;

/*
    loop through columns and rows of coarse grid.
*/
    for (i=0; i<Nrow-1; i+=Ncoarse) {

        offset = Ncol * i;
        y0 = i * Yspace + Ymin;

        for (j=0; j<Ncol-1; j+=Ncoarse) {

        /*
            Set the x and y arrays with the point locations
            of the refined grid, at half the current coarse spacing.
        */
            x0 = j * Xspace + Xmin;
            k = offset + j;
            n = 0;

        /*
            left side only for first column
        */
            if (j == 0) {
                x[0] = Xmin;
                y[0] = y0 + ysp2;
                n = 1;
            }

        /*
            center, bottom side and right side for all cells.
        */
            x[n] = x0 + xsp2;
            y[n] = y0;
            n++;
            x[n] = x0 + xsp2;
            y[n] = y0 + ysp2;
            n++;
            x[n] = x0 + xsp;
            y[n] = y0 + ysp2;
            n++;

        /*
            top side only for top row.
        */
            if (i == nrlast) {
                x[n] = x0 + xsp2;
                y[n] = y0 + ysp;
                n++;
            }

        /*
            do bicubic or bilinear interpolation
        */
            if (CheckForBicub (i, j)  &&  bcflag) {
                istat = grd_utils_ptr->grd_bicub_interp (x, y, z, n, 0.0f,
                                  Grid, Ncol, Nrow, Ncoarse,
                                  Xmin, Ymin, Xmax, Ymax,
                                  -1, -1);
            }
            else {
                istat = grd_utils_ptr->grd_bilin_interp (x, y, z, n,
                                  Grid, Ncol, Nrow, Ncoarse,
                                  Xmin, Ymin, Xmax, Ymax);
            }
            if (istat == -1) {
                grd_utils_ptr->grd_set_err(1);
                return -1;
            }

        /*
            stuff the z values into the grid at the proper locations
            starting with the left side if it is the first column.
        */
            n = 0;
            if (j == 0) {
                *(Grid+k+off2) = z[n];
                n++;
            }

        /*
            assign bottom side, center and right side
        */
            *(Grid+k+nc2) = z[n];
            n++;

            *(Grid+k+off2+nc2) = z[n];
            n++;

            *(Grid+k+off2+Ncoarse) = z[n];
            n++;

        /*
            assign top side if it is the last row
        */
            if (i == nrlast) {
                *(Grid+k+Ncoarse*Ncol+nc2) = z[n];
            }

        }  /*  end of loop through columns  */

    }  /*  end of loop through rows  */

    return 1;

}  /*  end of private RefineGrid function  */






/*
  ****************************************************************

                 T r e n d P o i n t E r r o r s

  ****************************************************************

  Calculate the errors between data points and the estimated
  elevation at the point for the trend surface grid.  The max
  error and average error are used to adjust the distance at
  which the trend elevation is combined with the local elevation.

*/

int CSWGrdCalc::TrendPointErrors (void)
{
    int            k, n;
    CSW_F          maxerr, avgerr, ft, z4, zerr;

    maxerr = 0.0f;
    avgerr = 0.0f;
    n = 0;

/*
    loop through all data points.
*/
    for (k=0; k<Ndata; k++) {

    /*
        ignore if outside of the grid
    */
        if (Ydata[k] < Ymin  ||  Ydata[k] > Ymax  ||
            Xdata[k] < Xmin  ||  Xdata[k] > Xmax) {
            zerr = 1.e30f;
            continue;
        }

    /*
        do bilinear interpolation
    */
        grd_utils_ptr->grd_bilin_interp (Xdata+k, Ydata+k, &zerr, 1,
                          TrendGrid, Nccol, Ncrow, 1,
                          Xmin, Ymin, Xmax, Ymax);

        zerr -= Zdata[k];
        if (zerr < 0.0f) zerr = -zerr;
        if (zerr > maxerr) {
            maxerr = zerr;
        }

        avgerr += zerr;
        n++;

    }

/*
    adjust the trend distance depending on the average and max errors
*/
    avgerr = avgerr / (CSW_F)n;

    ft = (CSW_F)TrendDistance;
    z4 = Zrange / 20.0f;

    if (Zrange <= 0.0f) return 1;

/*
    If the trend almost exactly fits the data, combine all nodes.
*/
    TrendSmoothFlag = 0;
    if (UseShapeGrid == 0) {
        if (avgerr < Zrange / 5000.f  &&  maxerr < Zrange / 1000.0f) {
            TrendDistance = 0;
            TrendSmoothFlag = 1;
        }

    /*
        Trend nearly fits the data, so set the minimum combining
        distance to a small value.
    */
        else if (avgerr < Zrange / 1000.f  &&  maxerr < Zrange / 200.0f) {
            TrendDistance = Ncoarse / 2;
        }

    /*
        Generally calculate the minimum combining distance,
        never less than Ncoarse.
    */
        else {
            avgerr = (avgerr + maxerr) / 2.0f;
            ft *= avgerr / z4;
            if (ft < 1.0f) ft = 1.0f;
            if (ft > (CSW_F)Ncoarse * 8) ft = (CSW_F)Ncoarse * 8;
            TrendDistance = (int)ft;
            if (TrendDistance < Ncoarse) TrendDistance = Ncoarse;
        }
    }

    return 1;

}  /*  end of private TrendPointErrors function  */






/*
  ****************************************************************

               S i m p l e S m o o t h G r i d

  ****************************************************************

  Smooth the grid with a 3 by 3 moving average.  The smoothed
  value is combined with the original value depending upon the
  closest data point to the node and the smoothing factor.
  No strike weighted smoothing is done.

*/

int CSWGrdCalc::SimpleSmoothGrid (void)
{
    int       i, j, k, offset, off2, ki, kj, ismt, cp;
    int       cpcrit;
    CSW_F     fncc, sum, sum2, wt, avg, datafact, cpmult, cpmult2;
    CSW_F     z0, z1, z2, zt, zt2, zgrid;

    AdjustForUnderflow ();

    if (FaultedFlag) {
        SimpleSmoothFaultedGrid ();
        return 1;
    }

    if (StepGridFlag  ||  Ndata < 4  ||  TrendSmoothFlag == 1) {
        return 1;
    }

    datafact = (CSW_F)Ndata / 2000.0f;
    if (datafact < 0.1f) datafact = 0.1f;
    if (SmoothingFactor > 0.0) {
        datafact *= SmoothingFactor;
    }
    if (datafact > 1.0f) {
        datafact = (CSW_F)sqrt ((double)datafact);
        datafact = (CSW_F)sqrt ((double)datafact);
    }

    if (FastGridFlag) {
        datafact = 10.0f;
    }

    ismt = (int)(SmoothingFactor * 2.0f + .5f);
    if (ismt < 1) ismt = 1;

    fncc = (CSW_F)Ncoarse + .5f;
    if (Ncoarse == 1) {
        datafact = (CSW_F)Ndata / 2000.0f;
        if (FastGridFlag) datafact = (CSW_F)Ndata / 5000.0f;
        if (DistancePower > 2) datafact /= 2.0f;
        if (DistancePower > 4) datafact /= 2.0f;
        if (datafact < 0.1f) datafact = 0.1f;
        if (SmoothingFactor > 0.0) {
            datafact *= SmoothingFactor;
        }
        if (datafact > 1.0f) {
            datafact = (CSW_F)sqrt ((double)datafact);
            datafact = (CSW_F)sqrt ((double)datafact);
        }
        if (datafact > 10.0f) datafact = 10.0f;
        fncc = 2.0f;
    }

    cpcrit = 2;
    if (TriangulateFlag > 0) {
        cpcrit = 1;
    }

/*
    Smooth and put the smoothed results into the Gwork1 array
*/
    for (i=0; i<Nrow; i+=Ncoarse) {

        offset = i * Ncol;

        for (j=0; j<Ncol; j+=Ncoarse) {

            k = offset + j;

        /*
            Sum the values of the immediate neighbors of the node.
        */
            sum = 0.0f;
            sum2 = 0.0f;
            z0 = Grid[k];
            z1 = 1.e30f;
            z2 = -1.e30f;
            for (ki=i-Ncoarse; ki<=i+Ncoarse; ki+=Ncoarse) {
                if (ki<0  ||  ki>=Nrow) {
                    continue;
                }
                off2 = ki*Ncol;
                for (kj=j-Ncoarse; kj<=j+Ncoarse; kj+=Ncoarse) {
                    if (kj>=0  &&  kj<Ncol) {
                        zt = Grid[off2+kj];
                        sum += zt;
                        sum2 += 1.0f;
                        if (off2+kj != k) {
                            if (zt < z1) z1 = zt;
                            if (zt > z2) z2 = zt;
                        }
                    }
                }
            }

            if (sum2 > 0.0) {
                if (sum < TinySum  &&  sum > -TinySum) {
                    sum = 0.0;
                }
                avg = sum / sum2;
            }
            else {
                avg = Grid[k];
            }

        /*
            Combine the smoothed and original value according to
            the distance from the closest data point at the node.
        */
            cp = ClosestPoint[k];
            cpmult2 = 1.0f;
            if (cp < cpcrit) {
                if (z0 > z2  ||  z0 < z1) {
                    zt = (z1 + z2) / 2.0f;
                    zt2 = z0 - zt;
                    if (zt2 < 0.0f) zt2 = -zt2;
                    cpmult2 = (z2 - z1) / zt2;
                    cpmult2 /= 2.0f;
                    cpmult2 *= cpmult2;
                    cpmult2 *= cpmult2;
                }
            }
            else {
                cp += TriangulateFlag;
                cpmult2 = (CSW_F)(TriangulateFlag + 1);
            }
            zgrid = Grid[k];
            if (cp < -3) {
                cp = -cp;
                cp -= 4;
                zgrid = Zdata[cp];
                cp = 0;
                cpmult = .01f;
            }
            else {
                cpmult = 1.0f;
                if (cp == -3) {
                    cpmult = .01f;
                }
                else if (cp == -2) {
                    cpmult = .05f;
                }
                else if (cp == -1) {
                    cpmult = .25f;
                }
            }
            wt = (CSW_F)(cp + ismt) / fncc;
            wt *= datafact;
            wt *= wt;
            wt *= cpmult;
            wt *= cpmult2;
            Gwork1[k] = (zgrid + avg * wt) / (1.0f + wt);

        }

    }

/*
    Copy the smoothed results back to the Grid array.
*/
    for (i=0; i<Nrow; i+=Ncoarse) {
        offset = i * Ncol;
        for (j=0; j<Ncol; j+=Ncoarse) {
            Grid[offset+j] = Gwork1[offset+j];
        }
    }

    return 1;

}  /*  end of private SimpleSmoothGrid function  */






/*
  ****************************************************************

                E x t e n d C o a r s e N o d e s

  ****************************************************************

  Assign elevations to some uncalculated coarse nodes by extending
  the gradients defined by near neighbor nodes that have been
  calculated.  This may be called multiple times to extend further.

*/

int CSWGrdCalc::ExtendCoarseNodes (int iter, int nmin)
{

    int        i, j, i1, i2, j1, j2, off1, off2,
               numpos, astat, n, cp;
    CSW_F      fiter, z1, z2, zt, sum3, sum4,
               x1, y1, x2, y2, st, fdum, minusvalue,
               xsp2, ysp2, tiny, st0;

/*
    If a faulted grid is being calculated, call the version
    of this function that honors the fault vectors.
*/
    if (FaultedFlag) {
        ExtendFaultedCoarseNodes (iter, nmin);
        return 1;
    }

    minusvalue = 0.0f;
    if (ThicknessFlag == 1) {
        minusvalue = Zrange / 20.0f * (iter + 1);
        minusvalue = -minusvalue;
    }

/*
    Calculate endpoints for a line at the preferred
    strike with the grid node as origin.
*/
    xsp2 = Xspace * Ncoarse;
    ysp2 = Yspace * Ncoarse;
    tiny = (xsp2 + ysp2) / 200.0f;
    tiny *= tiny;

    astat = 0;

/*
    weight of the extrapolated gradient goes down
    with more iterations through this function.
*/
    fiter = (CSW_F)iter;
    if (PreferredStrike >= 0  ||  AnisotropyFlag) {
        fiter--;
    }

    fiter /= 3.0f;
    if (TriangulateFlag == 4) {
        fiter /= 200.0;
        if (nmin > 2) nmin = 2;
    }
    if (ThicknessFlag == 1) {
        fiter /= 2.0f;
    }

    if (fiter < 0.0f) fiter = 0.0f;

    if (TriangulateFlag > 0  &&  TriangulateFlag < 4) {
        fiter /= (TriangulateFlag * TriangulateFlag);
    }

/*
    loop through the coarse nodes looking for uncalculated
    locations.
*/
    off2 = 0;
    numpos = 0;
    for (i=0; i<Nrow; i+=Ncoarse) {

        off1 = Ncol * i;

        for (j=0; j<Ncol; j+=Ncoarse) {

            astat = CalcStrikeLine (i, j, &x1, &y1, &x2, &y2, &fdum);
            if (TriangulateFlag == 4) {
                astat = 0;
            }

        /*
            Transfer already calculated nodes directly to the results grid.
        */
            if (Grid[off1+j] < 1.e29f) {
                Gwork1[off1+j] = Grid[off1+j];
                continue;
            }

        /*
            Find and average the local gradients if possible.
        */
            sum3 = 0.0f;
            sum4 = 0.0f;
            CSW_F z1sum3 = 0.0f;
            CSW_F z1sum4 = 0.0f;
            n = 0;

        /*
            lower left gradient
        */
            i1 = i - Ncoarse;
            i2 = i - 2 * Ncoarse;
            j1 = j - Ncoarse;
            j2 = j - 2 * Ncoarse;
            if (i2 >= 0  &&  j2 >= 0) {
                z1 = Grid[i1*Ncol+j1];
                if (z1 < 1.e29) {
                    z1sum3 += z1 * .7;
                    z1sum4 += .7;
                }
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, -xsp2, -ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    n++;
                }
            }

        /*
            lower center gradient
        */
            j1 = j;
            j2 = j;
            if (i2 >= 0) {
                z1 = Grid[i1*Ncol+j1];
                if (z1 < 1.e29) {
                    z1sum3 += z1;
                    z1sum4 ++;
                }
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, 0.0f, -ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    n++;
                }
            }

        /*
            lower right gradient
        */
            j1 = j + Ncoarse;
            j2 = j + 2 * Ncoarse;
            if (i2 >= 0  &&  j2 < Ncol) {
                z1 = Grid[i1*Ncol+j1];
                if (z1 < 1.e29) {
                    z1sum3 += z1 * .7;
                    z1sum4 += .7;
                }
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, xsp2, -ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    n++;
                }
            }

        /*
            left gradient
        */
            i1 = i;
            i2 = i;
            j1 = j - Ncoarse;
            j2 = j - 2 * Ncoarse;
            if (j2 >= 0) {
                z1 = Grid[i1*Ncol+j1];
                if (z1 < 1.e29) {
                    z1sum3 += z1;
                    z1sum4 ++;
                }
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, -xsp2, 0.0f, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    n++;
                }
            }

        /*
            right gradient
        */
            j1 = j + Ncoarse;
            j2 = j + 2 * Ncoarse;
            if (j2 < Ncol) {
                z1 = Grid[i1*Ncol+j1];
                if (z1 < 1.e29) {
                    z1sum3 += z1;
                    z1sum4 ++;
                }
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, xsp2, 0.0f, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    n++;
                }
            }

        /*
            upper left gradient
        */
            i1 = i + Ncoarse;
            i2 = i + 2 * Ncoarse;
            j1 = j - Ncoarse;
            j2 = j - 2 * Ncoarse;
            if (i2 < Nrow  &&  j2 >= 0) {
                z1 = Grid[i1*Ncol+j1];
                if (z1 < 1.e29) {
                    z1sum3 += z1 * .7;
                    z1sum4 += .7;
                }
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, -xsp2, ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    n++;
                }
            }

        /*
            upper center gradient
        */
            j1 = j;
            j2 = j;
            if (i2 < Nrow) {
                z1 = Grid[i1*Ncol+j1];
                if (z1 < 1.e29) {
                    z1sum3 += z1;
                    z1sum4 ++;
                }
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, 0.0f, ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    n++;
                }
            }

        /*
            upper right gradient
        */
            j1 = j + Ncoarse;
            j2 = j + 2 * Ncoarse;
            if (i2 < Nrow  &&  j2 < Ncol) {
                z1 = Grid[i1*Ncol+j1];
                if (z1 < 1.e29) {
                    z1sum3 += z1 * .7;
                    z1sum4 += .7;
                }
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, xsp2, ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    n++;
                }
            }


        /* 
         *  There are no valid gradient extensions, but there
         *  is a closest neighbor value available.
         */
            if (sum4 <= 0.0  &&  z1sum4 > 0.0  &&  n >= nmin) {
                Gwork1[off1+j] = z1sum3 / z1sum4;
            }
                
        /*
            Put the average of valid gradient extensions into the
            results grid, or assign 1.e30f as the result if there
            are no valid gradient extensions.
        */
            else if (sum4 > 0.0f  &&  n >= nmin) {

                CSW_F ss11 = sum3 / sum4;
                CSW_F ss22 = z1sum3 / z1sum4;

                cp = ClosestPoint[off1+j];
                if (cp < 0) cp = 0;
                CSW_F z1wgt = (double)cp / (double)Ncoarse;
                if (z1wgt < 1.0) z1wgt = 1.0;
                z1wgt *= z1wgt;
                
                Gwork1[off1+j] = (z1wgt * ss22 + ss11) /
                                 (z1wgt + 1.0);

                if (ThicknessFlag == 1  ||  OriginalThicknessFlag == 1) {
                    if (ZeroCoarse[off2] == 1) {
                        if (Gwork1[off1+j] > minusvalue * 10.0f) {
                            Gwork1[off1+j] = minusvalue * 10.0f;
                        }
                    }
                    else if (cp > 2 * Ncoarse  &&  numpos == 0) {
                        if (Gwork1[off1+j] > minusvalue) {
                            Gwork1[off1+j] = minusvalue;
                        }
                    }
                }
            }
            else {
                Gwork1[off1+j] = 1.e30f;
            }
            off2++;

        }  /*  end of j column loop  */

    }  /*  end of i row loop  */


// Smooth far from data nodes and transfer back to Grid

    int    ii, jj, kk, iioff;
    int    cpmin;

    cpmin = (iter / 2 + 2) * Ncoarse;

cpmin = 4 * Ncoarse;

    for (i=0; i<Nrow; i+=Ncoarse) {
        off1 = i * Ncol;
        i1 = i - Ncoarse;
        i2 = i + Ncoarse;
        if (i1 < 0) i1 = 0;
        if (i2 > Nrow - 1) i2 = Nrow - 1;
        for (j=0; j<Ncol; j+=Ncoarse) {
            cp = ClosestPoint[off1+j];
            if (Gwork1[off1+j] > 1.e29  ||  cp < cpmin) {
                Grid[off1+j] = Gwork1[off1+j];
                continue;
            }
            sum3 = 0.0;
            sum4 = 0.0;
            j1 = j - Ncoarse;
            j2 = j + Ncoarse;
            if (j1 < 0) j1 = 0;
            if (j2 > Ncol - 1) j2 = Ncol - 1;
            for (ii=i1; ii<=i2; ii+=Ncoarse) { 
               iioff = ii * Ncol;
               for (jj=j1; jj<=j2; jj+=Ncoarse) {
                   kk = iioff + jj;
                   if (Gwork1[kk] < 1.e29) {
                       sum3 += Gwork1[kk];
                       sum4 ++;
                   }
               }
            }
            if (sum4 > 0.0) {
                Grid[off1+j] = sum3 / sum4;
            }
            else {
                Grid[off1+j] = Gwork1[off1+j];
            }
        }
    }

    for (i=0; i<Nrow; i+=Ncoarse) {
        off1 = i * Ncol;
        for (j=0; j<Ncol; j+=Ncoarse) {
            Grid[off1+j] = Gwork1[off1+j];
        }
    }

    return 1;

}  /*  end of private ExtendCoarseNodes function  */







/*
  ****************************************************************

             F i t B e s t T r e n d S u r f a c e

  ****************************************************************

    Calculate a plane fit and a second order fit to the points.
  Use the plane unless the second order errors are far better
  than the plane errors.

*/

int CSWGrdCalc::FitBestTrendSurface (void)
{
    int          istat, iorder, n, i;
    CSW_F        tiny, pdmax, ratio;
    CSW_F        *x, *y, *z;
    CSW_F        xt1, yt1, xt2, yt2, zavg, minusval;
    int          ndata, idelta, nskip;
    CSW_F        *xwork = p_work1, *ywork = p_work2, *zwork = p_work3;

    GPFCalcdraw    gpf_calcdraw_obj;

    if (ThicknessFlag == 0) {
        x = Xdata;
        y = Ydata;
        z = Zdata;
        ndata = Ndata;
    }
    else {
        minusval = Zrange / 2.0f;
        minusval = -minusval;
        if (minusval > -0.1f) minusval = 0.1f;
        x = xwork;
        y = ywork;
        z = zwork;
        idelta = Ndata / 10000 + 1;
        n = 0;
        for (i=0; i<Ndata; i+=idelta) {
            if (Zdata[i] >= Ztiny) {
                x[n] = Xdata[i];
                y[n] = Ydata[i];
                z[n] = Zdata[i];
                n++;
                if (n >= 10000) break;
            }
            else {
                x[n] = Xdata[i];
                y[n] = Ydata[i];
                z[n] = minusval;
                n++;
                if (n >= 10000) break;
            }
        }
        ndata = n;
    }

/*
    If there are no useable data points, return an error.
*/
    if (ndata < 1) {
        return -1;
    }

/*
    If all points are at the same x,y location, return a flat
    trend grid.
*/
    xt1 = 1.e30f;
    yt1 = 1.e30f;
    xt2 = -1.e30f;
    yt2 = -1.e30f;
    zavg = 0.0f;
    for (i=0; i<ndata; i++) {
        if (x[i] < xt1) xt1 = x[i];
        if (x[i] > xt2) xt2 = x[i];
        if (y[i] < yt1) yt1 = y[i];
        if (y[i] > yt2) yt2 = y[i];
        zavg += z[i];
    }
    zavg /= (CSW_F)(ndata);
    tiny = (Xmax - Xmin + Ymax - Ymin) / 2000.0f;
    if (xt2 - xt1 < tiny  &&  yt2 - yt1 < tiny) {
        ndata = 1;
    }

/*
    If there is only one useable point, set all trend
    nodes to the value of that point.
*/
    if (ndata == 1) {
        for (i=0; i<Nccol*Ncrow; i++) {
            TrendGrid[i] = zavg;
        }
        return 1;
    }

/*
    For 10000 points or less, always use the local arrays for
    points in case the point set needs to be expanded for any reason.
*/
    if (ndata <= 10000) {
        x = xwork;
        y = ywork;
        z = zwork;
        memcpy (x, Xdata, ndata * sizeof(CSW_F));
        memcpy (y, Ydata, ndata * sizeof(CSW_F));
        memcpy (z, Zdata, ndata * sizeof(CSW_F));
    }

/*
    If there are 2 points, generate a third point so a
    plane can be fit.  The plane reflects the gradient
    between the two points.
*/
    if (ndata == 2) {
        gpf_calcdraw_obj.gpf_perpbisector
            (x[0], y[0], x[1], y[1], &xt1, &yt1, &xt2, &yt2);
        x[2] = xt2;
        y[2] = yt2;
        z[2] = (z[0] + z[1]) / 2.0f;
        ndata = 3;
    }

/*
    If the points are essentially all in a line, generate additional
    points off the line.  The assumption is that the strike of the
    surface is perpendicular to the line.   If the anisotropy flag
    is on in this case, turn it off.
*/
    InLineFlag = 0;
    ratio = 0.1f;
    tiny = (Xmax - Xmin + Ymax - Ymin) / 100.0f;
    if (ndata <= 10000) {
        istat = grd_utils_ptr->grd_colinear_check (x, y, ndata, tiny, &pdmax);
        if (pdmax < tiny) pdmax = tiny;
        if (istat == 1) {
            nskip = 1;
            InLineFlag = 1;
            SmoothingFactor *= 3;
        }
        else {
            nskip = (int)(pdmax * pdmax / tiny);
            if (pdmax < tiny * 5.0f) {
                InLineFlag = 1;
                SmoothingFactor *= 2;
            }
        }

        if (InLineFlag == 1) {
            if (AnisotropyFlag) {
                AnisotropyFlag = 0;
                PreferredStrike = -1000.0f;
            }
            if (nskip < 1) nskip = 1;
            if (nskip > ndata-1) nskip = ndata - 1;
            if (ndata/nskip > 5) nskip = ndata / 5 + 1;
            n = ndata;
            ratio = tiny / pdmax;
            ratio *= ratio;
            if (ratio > .5f) {
                nskip = ndata / 2 + 1;
            }
            if (ratio > 0.1f) ratio = 0.1f;
            for (i=0; i<ndata-nskip; i+=nskip) {
                gpf_calcdraw_obj.gpf_perpbisector
                                 (x[i], y[i], x[i+1], y[i+1],
                                  &xt1, &yt1, &xt2, &yt2);
                x[n] = xt1 + (xt2 - xt1) * ratio;
                y[n] = yt1 + (yt2 - yt1) * ratio;
                z[n] = (z[i] + z[i+1]) / 2.0f;
                n++;
            }
            ndata = n;
        }
    }

    if (InLineFlag == 0) {
        if (TrendDistance < Ncoarse) TrendDistance = Ncoarse;
    }

/*
    Fit a plane to the data points.
*/
    iorder = 1;
    istat = grd_tsurf_ptr->grd_calc_trend_grid (x, y, z, ndata, iorder,
                                 TrendGrid, Nccol, Ncrow,
                                 Xmin, Ymin, Xmax, Ymax);
    if (istat == -1) {
        return -1;
    }

    return 1;

}  /*  end of private FitBestTrendSurface function  */





/*
  ****************************************************************

                  g r d _ s m o o t h _ g r i d

  ****************************************************************

  Smooth a previously calculated grid.  The smoothing factor is
  qualitative.  A factor of 1 appears to have little smoothing.  A
  factor of 9 has a lot of smoothing.

  If the output grid is NULL, the smoothed data will overwrite
  the input.  If output smgrid is not null, the function allocates
  space for the smoothed output.

*/

int CSWGrdCalc::grd_smooth_grid
    (CSW_F *grid, int ncol, int nrow, int smfact,
     FAultLineStruct *faults, int nfaults,
     CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
     CSW_F minval, CSW_F maxval, CSW_F **smgrid)
{
    int      nc, nc2, nc3, nc4, i, j, k, ii, jj, istat,
             n, noff, noff2, endrow, endcol, nncol, nnrow,
             i1, i2, j1, j2, residflag;
    int      lightflag;
    int      ncout, nrout, skip;
    CSW_F    *xrow = NULL, *yrow = NULL,
             xmin, ymin, xmax, ymax,
             pivot, sum1, sum2;
    CSW_F    *smg = NULL, *fwork = NULL, *fwork2 = NULL;
    CSW_F    zmin, zmax, zrange, zt, zt2, smfact2;
    CSW_F    *gwork3 = NULL;

    int      do_write;

    bool     bsmg = false;

    auto fscope = [&]()
    {
        csw_Free (xrow);
        csw_Free (gwork3);
        csw_Free (GGwork1);
        GGwork1 = GGwork2 = NULL;
        csw_Free (fwork);
        csw_Free (fwork2);
        if (bsmg) csw_Free (smg);
    };
    CSWScopeGuard func_scope_guard (fscope);



/*
    check obvious errors
*/
    if (ncol > WildInteger  ||  ncol < -WildInteger  ||
        nrow > WildInteger  ||  nrow < -WildInteger  ||
        smfact > WildInteger  ||  smfact < -WildInteger) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (!grid) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
    If less than 2 columns or rows, return an error.
*/
    if (ncol < MIN_COLS_GCALC  ||  nrow < MIN_ROWS_GCALC) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

/*
    Deal with bad minval and maxval parameters.
*/
    if (minval >= maxval) {
        minval = -1.e30f;
        maxval = 1.e30f;
    }

/*
    if smgrid is NULL, set it to grid
*/
    if (smgrid == NULL) {
        smg = grid;
        smgrid = &grid;
    }

    else {
MSL
        bsmg = true;
        smg = (CSW_F *)csw_Malloc (ncol*nrow*sizeof(CSW_F));
        if (!smg) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        *smgrid = smg;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    Set flag for residual or smoothed output.
*/
    residflag = 0;
    if (smfact < 0) {
        smfact = -smfact;
        residflag = 1;
    }

/*
 * Moving average only for smfact > 1000, regardless of faulting
 */
    if (smfact >= 1000) {

        smfact -= 1000;

        gwork3 = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (gwork3 == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

        memcpy (gwork3, grid, ncol*nrow*sizeof(CSW_F));

        for (i=0; i<smfact; i++) {
            MovingAverage (gwork3, ncol, nrow, smfact, 0);
        }

        memcpy (smg, gwork3, ncol * nrow * sizeof(CSW_F)); /*lint !e669 !e670*/
        return 1;
    }


/*
 * If the smoothing factor is greater than 100, light smoothing
 * should be turned on and the smoothing factor is 100 less than
 * the factor specified.
 */
    lightflag = 0;
    if (smfact > 100) {
        lightflag = 1;
        smfact -= 100;
    }

    if (smfact < 1) smfact = 1;
    if (smfact > 10) smfact = 10;

    zmin = 1.e30f;
    zmax = -1.e30f;
    for (i=0; i<ncol * nrow; i++) {
        if (grid[i] > 1.e20  ||  grid[i] < -1.e20) {
            continue;
        }
        if (grid[i] < zmin) zmin = grid[i];
        if (grid[i] > zmax) zmax = grid[i];
    }

    TinySum = 0.0;
    if (zmax > zmin) {
        zrange = zmax - zmin;
        TinySum = zrange / 100000.0f;
        if (TinySum > 1.e-10) TinySum = 1.e-10f;
    }

/*
    If the grid is faulted, use the faulted smoothing functions.
*/
    if (faults  &&  nfaults > 0) {
        do_write = csw_GetDoWrite ();
        if (do_write) {
            grd_WriteFaultLines (faults, nfaults, "SmoothFault.line");
        }
        istat = grd_fault_ptr->grd_define_fault_vectors (faults, nfaults);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

    /*
        resample the grid, coarser for higher smoothing factors.
    */
        skip = smfact / 3;
        if (skip < 1) skip = 1;
        ncout = ncol / skip;
        nrout = nrow / skip;
        if (ncout < 2) ncout = 2;
        if (nrout < 2) nrout = 2;
        grd_fault_ptr->grd_set_fault_index_extent (Ncoarse);
        istat = grd_fault_ptr->grd_build_fault_indices (grid, ncol, nrow,
                                         x1, y1, x2, y2);
        grd_fault_ptr->grd_set_fault_index_extent (1);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

        fwork = (CSW_F *)csw_Malloc (ncout * nrout * sizeof(CSW_F));
        if (!fwork) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

        istat = grd_fault_ptr->grd_resample_faulted_grid (fwork, ncout, nrout,
                                           x1, y1,
                                           x2, y2,
                                           GRD_BILINEAR, 1);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

        int do_write = csw_GetDoWrite ();
        if (do_write) {
            grd_triangle_ptr->grd_WriteXYZGridFile ("AfterResamp.xyz",
                                  fwork, ncout, nrout,
                                  1,
                                  x1, y1,
                                  x2, y2);
        }

    /*
        Smooth the resampled grid;
    */
        grd_fault_ptr->grd_set_fault_index_extent (Ncoarse);
        istat = grd_fault_ptr->grd_build_fault_indices (fwork, ncout, nrout,
                                         x1, y1,
                                         x2, y2);
        grd_fault_ptr->grd_set_fault_index_extent (1);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

        istat = grd_fault_ptr->grd_faulted_smoothing (NULL, smfact);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

    /*
        Resample back to the original geometry.
    */
        fwork2 = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (!fwork2) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        istat = grd_fault_ptr->grd_resample_faulted_grid (fwork2, ncol, nrow,
                                           x1, y1, x2, y2,
                                           GRD_BICUBIC, 1);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

    /*
        Copy to the output grid.
    */
        memcpy (*smgrid, fwork2, ncol * nrow * sizeof(CSW_F));

    /*
        Clip the output grid if needed.
    */
        if (minval < 1.e20f  ||  maxval < 1.e20f) {
            smg = *smgrid;
            for (i=0; i<ncol*nrow; i++) {
                if (minval < 1.e20f) {
                    if (smg[i] < minval) smg[i] = minval;
                }
                if (maxval < 1.e20f) {
                    if (smg[i] > maxval) smg[i] = maxval;
                }
            }
        }

        return 1;
    }

    gwork3 = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (gwork3 == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    memcpy (gwork3, grid, ncol*nrow*sizeof(CSW_F));

/*
  Calculate a coarse interval for the grid.  The coarse interval
  is larger for large rsmoothing factors and for larger grids.
*/
    nc = (int) ((double)(ncol + nrow) / 50 + .5);

    if (nc > 64) nc = 64;
    else if (nc > 32) nc = 32;
    else if (nc > 16) nc = 16;
    else if (nc > 8) nc = 8;
    else if (nc > 4) nc = 4;
    else if (nc > 2) nc = 2;
    else nc = 1;

    if (lightflag) nc /= 2;

    if (nc <= 1) {
        if (ncol > 9  &&  nrow > 9) {
            nc = 2;
        }
    }

    if (nc < 1) nc = 1;

/*
  More moving average and spike removal passes for large grids.
*/
    int   nma = smfact / 2 + 1;
    if (ncol * nrow > 100000) nma++;
    if (ncol * nrow > 1000000) nma++;

    int  nsr = nma;
    if (nsr < 2) nsr = 2;

    for (i=0; i<nsr; i++) {
        RemoveSpikes (gwork3, ncol, nrow, smfact, nc);
    }

    for (i=0; i<nma; i++) {
        MovingAverage (gwork3, ncol, nrow, smfact, 0);
    }

//memcpy (smg, gwork3, ncol * nrow * sizeof(CSW_F)); /*lint !e669 !e670*/
//return 1;





/*
 * If the coarse interval is less than 2,
 * there are not enough rows or columns in the grid
 * to support bicubic smoothing.  In this case, do
 * multiple passes of moving average smoothing and return.
 */
    if (nc < 2) {
        for (i=0; i<smfact; i++) {
            MovingAverage (gwork3, ncol, nrow, smfact, lightflag);
            for (j=0; j<ncol*nrow; j++) {
              if (gwork3[j] < 1.e20) {
                if (gwork3[j] < minval) gwork3[j] = minval;
                if (gwork3[j] > maxval) gwork3[j] = maxval;
              }
            }
            if (lightflag) break;
        }
        memcpy (smg, gwork3, ncol * nrow * sizeof(CSW_F)); /*lint !e669 !e670*/
        return 1;
    }

    if (NoisyDataFlag > 0) {
        int  nmado = smfact / 2;
        if (nmado < 2) nmado = 2;
        for (i=0; i<nmado; i++) {
            MovingAverage (gwork3, ncol, nrow, smfact, lightflag);
            for (j=0; j<ncol*nrow; j++) {
              if (gwork3[j] < 1.e20) {
                if (gwork3[j] < minval) gwork3[j] = minval;
                if (gwork3[j] > maxval) gwork3[j] = maxval;
              }
            }
        }
        memcpy (smg, gwork3, ncol * nrow * sizeof(CSW_F)); /*lint !e669 !e670*/
        return 1;
    }

    if (nc > 500) nc = 500;

/*
    adjust coarse spacing if it is greater than half the
    number of columns or half the number of rows.
*/
    while (nc > ncol/2  ||  nc > nrow/2) {
        nc /= 2;
    }

/*
    Allocate work grids that have room for margins of two
    coarse intervals on all sides of the grid to be smoothed.
    This requires 5 times the coarse interval more nodes in each
    dimension of the work grids.
*/
    nncol = (ncol - 1) / nc * nc + 5 * nc;
    nnrow = (nrow - 1) / nc * nc + 5 * nc;
MSL
    GGwork1 = (CSW_F *)csw_Malloc (nncol*nnrow*2*sizeof(CSW_F));
    if (!GGwork1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    GGwork2 = GGwork1 + nncol * nnrow;

/*
    Allocate space for bicubic interpolation x and y arrays.
*/
MSL
    xrow = (CSW_F *)csw_Malloc (nncol * 3 * sizeof(CSW_F));
    if (!xrow) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    yrow = xrow + nncol;

    nc2 = nc * 2;
    nc3 = nc * 3;
    nc4 = nc * 4;

/*
    Copy the grid into the center part of the first work array.
*/
    n = ncol * sizeof(CSW_F);
    for (i=0; i<nrow; i++) {
        k = (nc2 + i) * nncol + nc2;
        memcpy ((char *)(GGwork1+k), (char *)(gwork3+i*ncol), n); /*lint !e670*/
    }

/*
    The endrow and endcol variables are the last coarse interval
    inside the original grid data after it has been copied to the
    central part of the work space.
*/
    endrow = (nrow - 1) / nc * nc + nc2;
    endcol = (ncol - 1) / nc * nc + nc2;
//    endrow = nrow / nc * nc + nc2;
//    endcol = ncol / nc * nc + nc2;

    CSW_F  tz1 = 0.0;
    CSW_F  tz2 = 0.0;
    CSW_F  tz3 = 0.0;

    CSW_F  tz = 0.0;

    CSW_F  dnoise = (CSW_F)NoisyDataFlag;

/*
    Fill in the left and right margins at the coarse interval.
*/
    for (i = nc2; i <= endrow; i+=nc) {
        noff = i * nncol;
        sum1 = 0.0;
        sum2 = 0.0;
        for (ii=i-nc; ii<=i+nc; ii+=nc) {
            if (ii >= nc2  &&  ii <= endrow) {
                int  iin = ii * nncol;
                for (jj=nc2; jj<=nc3; jj+=nc) {
                    sum1 += GGwork1[iin+jj];
                    sum2++;
                }
            }
        }
        pivot = sum1 / sum2;
        sum1 = 0.0;
        sum2 = 0.0;
        for (ii=i-nc; ii<=i+nc; ii+=nc) {
            if (ii >= nc2  &&  ii <= endrow) {
                int  iin = ii * nncol;
                for (jj=nc3; jj<=nc4; jj+=nc) {
                    sum1 += GGwork1[iin+jj];
                    sum2++;
                }
            }
        }
        tz1 = sum1 / sum2;
        tz2 = 2.0 * pivot - tz1;
        GGwork1[noff+nc] = (tz2 + pivot * dnoise) / (1.0 + dnoise);
        tz2 = GGwork1[noff+nc];
        tz = 2.0 * tz2 - pivot;
        GGwork1[noff] = (tz + tz2 * dnoise) / (1.0 + dnoise);

        sum1 = 0.0;
        sum2 = 0.0;
        for (ii=i-nc; ii<=i+nc; ii+=nc) {
            if (ii >= nc2  &&  ii <= endrow) {
                int  iin = ii * nncol;
                for (jj=endcol-nc; jj<=endcol; jj+=nc) {
                    sum1 += GGwork1[iin+jj];
                    sum2++;
                }
            }
        }
        pivot = sum1 / sum2;
        sum1 = 0.0;
        sum2 = 0.0;
        for (ii=i-nc; ii<=i+nc; ii+=nc) {
            if (ii >= nc2  &&  ii <= endrow) {
                int  iin = ii * nncol;
                for (jj=endcol-nc2; jj<=endcol-nc; jj+=nc) {
                    sum1 += GGwork1[iin+jj];
                    sum2++;
                }
            }
        }
        tz1 = sum1 / sum2;
        tz2 = 2.0 * pivot - tz1;
        GGwork1[noff+endcol+nc] = (tz2 + pivot * dnoise) / (1.0 + dnoise);
        tz2 = GGwork1[noff+endcol+nc];
        tz3 = 2.0 * tz2 - pivot;
        GGwork1[noff+endcol+nc2] = (tz3 + tz2 * dnoise) / (1.0 + dnoise);
        tz3 = GGwork1[noff+endcol+nc2];
        tz = 2.0 * tz3 - tz2;
        GGwork1[noff+endcol+nc3] = (tz + tz3 * dnoise) / (1.0 + dnoise);

    }

/*
    Fill in the bottom and top margins at the coarse interval.
*/
    for (j=nc2; j<=endcol; j+=nc) {

  // bottom
        sum1 = 0.0;
        sum2 = 0.0;
        for (jj=j-nc; jj<=j+nc; jj+=nc) {
            if (jj >= nc2  &&  jj <= endcol) {
                for (ii=nc2; ii<=nc3; ii+=nc) {
                    int  iin = ii * nncol;
                    sum1 += GGwork1[iin+jj];
                    sum2++;
                }
            }
        }
        pivot = sum1 / sum2;
        sum1 = 0.0;
        sum2 = 0.0;
        for (jj=j-nc; jj<=j+nc; jj+=nc) {
            if (jj >= nc2  &&  jj <= endcol) {
                for (ii=nc3; ii<=nc4; ii+=nc) {
                    int  iin = ii * nncol;
                    sum1 += GGwork1[iin+jj];
                    sum2++;
                }
            }
        }
        tz1 = sum1 / sum2;
        tz2 = 2.0 * pivot - tz1;
        noff = nc2 * nncol;
        GGwork1[nc*nncol+j] = (tz2 + pivot * dnoise) / (1.0 + dnoise);
        tz2 = GGwork1[nc*nncol+j];
        tz = 2.0 * tz2 - pivot;
        GGwork1[j] = (tz + tz2 * dnoise) / (1.0 + dnoise);

    // top
        sum1 = 0.0;
        sum2 = 0.0;
        for (jj=j-nc; jj<=j+nc; jj+=nc) {
            if (jj >= nc2  &&  jj <= endcol) {
                for (ii=endrow-nc2; ii<=endrow; ii+=nc) {
                    int  iin = ii * nncol;
                    sum1 += GGwork1[iin+jj];
                    sum2++;
                }
            }
        }
        pivot = sum1 / sum2;
        sum1 = 0.0;
        sum2 = 0.0;
        for (jj=j-nc; jj<=j+nc; jj+=nc) {
            if (jj >= nc2  &&  jj <= endcol) {
                for (ii=endrow-nc3; ii<=endrow-nc; ii+=nc) {
                    int  iin = ii * nncol;
                    sum1 += GGwork1[iin+jj];
                    sum2++;
                }
            }
        }
        tz1 = sum1 / sum2;
        tz2 = 2.0 * pivot - tz1;
        noff = (endrow + nc) * nncol;
        GGwork1[noff+j] = (tz2 + pivot * dnoise) / (1.0 + dnoise);
        tz2 = GGwork1[noff+j];
        tz3 = 2.0 * tz2 - pivot;
        noff = (endrow + nc2) * nncol;
        GGwork1[noff+j] = (tz3 + tz2 * dnoise) / (1.0 + dnoise);
        tz3 = GGwork1[noff+j];
        noff = (endrow + nc3) * nncol;
        tz = 2.0 * tz3 - tz2;
        GGwork1[noff+j] = (tz + tz3 * dnoise) / (1.0 + dnoise);
    }

    CSW_F   xbl[9], ybl[9], zbl[9];
    CSW_F   blbox[4];

    CSWGrdUtils  gutils;

/*
    Fill in bottom left corner.
*/
    xbl[0] = 0;
    xbl[1] = nc;
    xbl[2] = 0;
    xbl[3] = nc;
    ybl[0] = 0;
    ybl[1] = 0;
    ybl[2] = nc;
    ybl[3] = nc;
    blbox[1] = GGwork1[nc2];
    blbox[2] = GGwork1[nc2 * nncol];
    blbox[3] = GGwork1[nc2*nncol + nc2];
    pivot = 2.0 * blbox[3];
    tz1 = pivot - GGwork1[nc4*nncol+nc4];
    blbox[0] = tz1;

    gutils.grd_bilin_interp
       (xbl, ybl, zbl, 4,
        blbox, 2, 2, 1,
        0, 0, nc2, nc2);
    
    GGwork1[0] = zbl[0];
    GGwork1[nc] = zbl[1];
    GGwork1[nc*nncol] = zbl[2];
    GGwork1[nc*nncol + nc] = zbl[3];

/*
    Fill in bottom right corner.
*/
    xbl[0] = nc;
    xbl[1] = nc2;
    xbl[2] = nc3;
    xbl[3] = nc;
    xbl[4] = nc2;
    xbl[5] = nc3;
    ybl[0] = 0;
    ybl[1] = 0;
    ybl[2] = 0;
    ybl[3] = nc;
    ybl[4] = nc;
    ybl[5] = nc;
    blbox[0] = GGwork1[endcol];
    blbox[2] = GGwork1[nc2*nncol+endcol];
    blbox[3] = GGwork1[nc2*nncol+endcol+nc3];
    pivot = 2.0 * blbox[2];
    tz1 = pivot - GGwork1[nc4*nncol+endcol-nc3];
    blbox[1] = tz1;

    gutils.grd_bilin_interp
       (xbl, ybl, zbl, 6,
        blbox, 2, 2, 1,
        0, 0, nc3, nc2);

    GGwork1[endcol+nc] = zbl[0];
    GGwork1[endcol+nc2] = zbl[1];
    GGwork1[endcol+nc3] = zbl[2];
    GGwork1[nc*nncol+endcol+nc] = zbl[3];
    GGwork1[nc*nncol+endcol+nc2] = zbl[4];
    GGwork1[nc*nncol+endcol+nc3] = zbl[5];
    
/*
    Fill in top left corner.
*/
    xbl[0] = 0;
    xbl[1] = nc;
    xbl[2] = 0;
    xbl[3] = nc;
    ybl[0] = nc;
    ybl[1] = nc;
    ybl[2] = nc2;
    ybl[3] = nc2;
    blbox[0] = GGwork1[endrow*nncol];
    blbox[1] = GGwork1[endrow*nncol+nc2];
    blbox[3] = GGwork1[(endrow+nc2)*nncol+nc2];
    pivot = 2.0 * blbox[1];
    tz1 = pivot - GGwork1[(endrow-nc2)*nncol+nc4];
    blbox[2] = tz1;

    gutils.grd_bilin_interp
       (xbl, ybl, zbl, 4,
        blbox, 2, 2, 1,
        0, 0, nc2, nc2);
    
    GGwork1[(endrow+nc)*nncol] = zbl[0];
    GGwork1[(endrow+nc)*nncol+nc] = zbl[1];
    GGwork1[(endrow+nc2)*nncol] = zbl[2];
    GGwork1[(endrow+nc2)*nncol+nc] = zbl[3];

/*
    Fill in top right corner.
*/
    xbl[0] = nc;
    xbl[1] = nc2;
    xbl[2] = nc3;
    xbl[3] = nc;
    xbl[4] = nc2;
    xbl[5] = nc3;
    ybl[0] = nc;
    ybl[1] = nc;
    ybl[2] = nc;
    ybl[3] = nc2;
    ybl[4] = nc2;
    ybl[5] = nc2;
    blbox[0] = GGwork1[endrow*nncol+endcol];
    blbox[1] = GGwork1[endrow*nncol+endcol+nc3];
    blbox[2] = GGwork1[(endrow+nc2)*nncol+endcol];
    pivot = 2.0 * blbox[0];
    tz1 = pivot - GGwork1[(endrow-nc2)*nncol+endcol-nc3];
    blbox[3] = tz1;

    gutils.grd_bilin_interp
       (xbl, ybl, zbl, 6,
        blbox, 2, 2, 1,
        0, 0, nc3, nc2);

    GGwork1[(endrow+nc)*nncol+endcol+nc] = zbl[0];
    GGwork1[(endrow+nc)*nncol+endcol+nc2] = zbl[1];
    GGwork1[(endrow+nc)*nncol+endcol+nc3] = zbl[2];
    GGwork1[(endrow+nc2)*nncol+endcol+nc] = zbl[3];
    GGwork1[(endrow+nc2)*nncol+endcol+nc2] = zbl[4];
    GGwork1[(endrow+nc2)*nncol+endcol+nc3] = zbl[5];
 
/*
    Smooth the coarse nodes using a 3 by 3 moving average operator with equal
    weight at each operator point.  The results are stored in GGwork2.
*/
    double  dsf = (double) smfact;
    for (i=0; i<nnrow; i+=nc) {

        noff = i * nncol;
        i1 = i-nc;
        if (i1 < 0) i1 = 0;
        i2 = i+nc;
        if (i2 > nnrow-1) i2 = nnrow-1;

        for (j=0; j<nncol; j+=nc) {

            j1 = j-nc;
            if (j1 < 0) j1 = 0;
            j2 = j+nc;
            if (j2 > nncol-1) j2 = nncol-1;

            sum1 = 0.0f;
            sum2 = 0.0f;

            for (ii=i1; ii<=i2; ii+=nc) {
                noff2 = ii * nncol;
                for (jj=j1; jj<=j2; jj+=nc) {
                    sum1 += GGwork1[noff2+jj];
                    sum2++;
                }
            }

        /*
         * If plateau smoothing was specified, any nodes in
         * the original at or outside the plateau limits are
         * not changed.
         */
            if (GGwork1[noff+j] <= LowPlateau  ||
                GGwork1[noff+j] >= HighPlateau) {
                GGwork2[noff+j] = GGwork1[noff+j];
            }
            else {
                GGwork2[noff+j] = 
                  (GGwork1[noff+j] + dsf * sum1 / sum2) / (1.0 + dsf);
                if (lightflag  &&  GGwork1[noff+j] < 1.e20) {
                    GGwork2[noff+j] += GGwork1[noff+j];
                    GGwork2[noff+j] /= 2.0;
                }
            }
        }
    }

/*
    Do a bicubic interpolation of the smoothed coarse nodes currently
    in Grid2 to fill in all the remaining nodes.  The results are
    put back into GGwork1.
*/
    xmin = 0.0;
    ymin = 0.0;
    xmax = (CSW_F)(nncol-1);
    ymax = (CSW_F)(nnrow-1);

    for (i=0; i<nnrow; i++) {
        noff = i * nncol;
        for (j=0; j<nncol; j++) {
            xrow[j] = (CSW_F)j;
            yrow[j] = (CSW_F)i;
        }
        istat = grd_utils_ptr->grd_bicub_interp
                         (xrow, yrow, GGwork1+noff, nncol, 0.0f,
                          GGwork2, nncol, nnrow, nc,
                          xmin, ymin, xmax, ymax,
                          -1, -1);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err(1);
            return -1;
        }
    }

/*
    Copy smoothed results to output grid.
*/
    smfact2 = 10.0f - smfact;
    for (i=nc2; i<nc2+nrow; i++) {
        noff = i * nncol;
        for (j=nc2; j<nc2+ncol; j++) {
            k = (i - nc2) * ncol + j - nc2;
            zt = GGwork1[noff+j];
            if (lightflag) {
                zt2 = grid[k] * smfact2;
                zt = (zt + zt2) / (1.0f + smfact2);
            }
            if (residflag == 0) {
                smg[k] = zt;
            }
            else {
                smg[k] = grid[k] - zt;
            }
        }
    }

/*
    Clip the output grid if needed.
*/
    if (minval < 1.e20f  ||  maxval < 1.e20f) {
        for (i=0; i<ncol*nrow; i++) {
            if (minval < 1.e20f) {
                if (smg[i] < minval) smg[i] = minval;
            }
            if (maxval < 1.e20f) {
                if (smg[i] > maxval) smg[i] = maxval;
            }
        }
    }

/*
    All done.
*/
    return 1;

}  /*  end of grd_smooth_grid function  */





/*
  ****************************************************************

                    C l i p T o M i n M a x

  ****************************************************************

  Clip the current coarse grid to values 5 percent outside the min
  and max specified by the grid options.  This insures that the
  doesn't deviate too much from the specified limits while it is
  being calculated.  After the grid is calculated, it is clipped
  to the exact min max prior to returning from the main grid
  calculation function.

*/

int CSWGrdCalc::ClipToMinMax (void)
{
    int          i, j, k;
    CSW_F        gmin, gmax, dg;

    if (GridMinValue < -1.e20f  &&  GridMaxValue > 1.e20f) {
        return 1;
    }

    if (GridMinValue > -1.e20f  &&  GridMaxValue < 1.e20f) {
        dg = (GridMaxValue - GridMinValue) / 20.0f;
        gmin = GridMinValue - dg;
        gmax = GridMaxValue + dg;
    }
    else {
        gmin = GridMinValue;
        gmax = GridMaxValue;
    }

    for (i=0; i<Nrow; i+=Ncoarse) {
        k = i * Ncol;
        for (j=0; j<Ncol; j+=Ncoarse) {
            if (Grid[k+j] < gmin) Grid[k+j] = gmin;
            if (Grid[k+j] > gmax) Grid[k+j] = gmax;
        }
    }

    return 1;

}  /*  end of private ClipToMinMax function  */





/*
  ****************************************************************

                  G e n e r a t e R e p o r t

  ****************************************************************

    Allocate space for and fill in a report on the grid just
  calculated.  The grid geometry, and the fit of the grid at
  the data points makes up the bulk of the report.

*/

int CSWGrdCalc::GenerateReport (char **report)
{
    int       nrep, i, j, n, jmax, nc, replines[MAX_REPORT_LINES],
              n1000, n500, n200, n100, n50, nrep2, iline;
    char      *cbuf = NULL, cline[2000], listfmt[2000];
    CSW_F     z1000, z500, z200, z100, z50, zt, zmax,
              esave[MAX_REPORT_LINES], sdev, zt2, zt3,
              f1000, f500, f200, f100, f50;
    CSW_F     dmean, dstd, gmean, gstd, zrange;
    int       *flist = NULL, nflist;
    int       formatlist[MAX_REPORT_LINES];

    bool    bsuccess = false;

    *report = NULL;

    auto fscope = [&]()
    {
        if (!bsuccess) {
            csw_Free (cbuf);
            *report = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Initialize some local stuff to suppress lint messages.
*/
    memset ((char *)replines, 0, MAX_REPORT_LINES * sizeof(int));
    memset ((char *)esave, 0, MAX_REPORT_LINES * sizeof(CSW_F));

/*
    Get the list of data points grazing faults if needed.
*/
    nflist = 0;
    flist = NULL;
    if (FaultedFlag) {
        grd_fault_ptr->grd_reset_fault_data_grazing (Xdata, &flist, &nflist);
    }

    zrange = Zrange;
    if (zrange == 0.0f  ||  FlatGridFlag == 1) {
        zrange = 1.0f;
    }

/*
    compute the mean, std dev of input data.
*/
    j = 0;
    zt = 0.0f;
    for (i=0; i<Ndata; i++) {
        if (Xdata[i] < Xmin  ||  Xdata[i] > Xmax  ||
            Ydata[i] < Ymin  ||  Ydata[i] > Ymax) {
            continue;
        }
        zt += Zdata[i];
        j++;
    }

    dmean = 0.0f;
    if (i > 0) {
        dmean = zt / (CSW_F)i;
    }

    j = 0;
    zt3 = 0.0f;
    for (i=0; i<Ndata; i++) {
        if (Xdata[i] < Xmin  ||  Xdata[i] > Xmax  ||
            Ydata[i] < Ymin  ||  Ydata[i] > Ymax) {
            continue;
        }
        zt = (Zdata[i] - dmean);
        zt2 = zt * zt;
        zt3 += zt2;
        j++;
    }

    zt3 /= (CSW_F)j;
    dstd = (CSW_F)(sqrt ((double)zt3));

/*
    compute the mean, std dev of grid data.
*/
    zt = 0.0f;
    for (i=0; i<Ncol*Nrow; i++) {
        zt += Grid[i];
    }

    gmean = zt / (CSW_F)(Ncol*Nrow);

    zt3 = 0.0f;
    for (i=0; i<Ncol*Nrow; i++) {
        zt = (Grid[i] - gmean);
        zt2 = zt * zt;
        zt3 += zt2;
    }

    zt3 /= (CSW_F)(Ncol*Nrow);
    gstd = (CSW_F)sqrt ((double)zt3);

/*
    Count the number of points with errors worse than certain
    percentages of the data range.
*/
    n1000 = 0;
    n500 = 0;
    n200 = 0;
    n100 = 0;
    n50 = 0;

    z1000 = zrange / 1000.f;
    z500 = zrange / 500.f;
    z200 = zrange / 200.f;
    z100 = zrange / 100.f;
    z50 = zrange / 50.f;

    zt3 = 0.0f;
    j = 0;
    for (i=0; i<Ndata; i++) {
        if (StepGridFlag) Zerr[i] = 0.0f;
        zt = Zerr[i];
        if (zt > 1.e20f) continue;
        if (zt < 0.0f) zt = -zt;
        if (zt > z1000) n1000++;
        if (zt > z500) n500++;
        if (zt > z200) n200++;
        if (zt > z100) n100++;
        if (zt > z50) n50++;
        zt2 = zt - AvgError;
        zt2 *= zt2;
        zt3 += zt2;
        j++;
    }

/*
    Compute standard deviation of the error
*/
    zt3 /= (CSW_F)j;
    sdev = (CSW_F)sqrt ((double)zt3);

/*
    Find the ReportLines worst points.
*/
    nrep = ReportLines;
    if (nrep > Ndata) nrep = Ndata;

    nrep2 = nrep;
    for (i=0; i<nrep; i++) {
        zmax = -1.0f;
        jmax = -1;
        for (j=0; j<Ndata; j++) {
            if (ThicknessFlag == 1  &&  Zdata[j] <= 0.0f) {
                continue;
            }
            zt = Zerr[j];
            if (zt > 1.e20f) continue;
            if (zt < 0.0f) zt = -zt;
            if (zt > zmax) {
                zmax = zt;
                jmax = j;
            }
        }
        if (jmax == -1) {
            nrep2 = i;
            break;
        }
        replines[i] = jmax;
        esave[i] = Zerr[jmax];
        Zerr[jmax] = 1.e30f;
    }

    nrep = nrep2;

/*
    Put the errors back into Zerr.
*/
    for (i=0; i<nrep; i++) {
        Zerr[replines[i]] = esave[i];
    }

/*
    Allocate a character array for the report.
*/
    nc = nrep * 100 + 2500;
    nc += nflist * 40;
MSL
    cbuf = (char *)csw_Malloc (nc * sizeof(char));
    if (!cbuf) {
        return -1;
    }

/*
    Put the grid geometry stuff into the report.
*/
    sprintf (cline,
"The grid has %d columns and %d rows and is calculated from %d data points.\n",
ReportNcol, ReportNrow, Ndata);
    strcpy (cbuf, cline);

    sprintf (cline,
"Minimum X = %f,  Minimum Y = %f,  Maximum X = %f,  Maximum Y = %f\n\n",
OrigXmin + XOutputShift, OrigYmin + YOutputShift,
OrigXmax + XOutputShift, OrigYmax + YOutputShift);
    strcat (cbuf, cline);

    if (nflist > 0) {
        sprintf (cline,
"The %d points listed below lie almost exactly on top of fault lines.\n\
These data points are not used in calculating the grid\n\n\
    Point          X            Y            Z\n\
---------------------------------------------------\n", nflist);
        strcat (cbuf, cline);
    /*
        Determine a format for the list that will line up the data
        correctly and use appropriate precision in the values.
    */
        FaultFmt = 1;
        DetermineListFormat (flist, nflist, listfmt);
        FaultFmt = 0;

        if (flist) {
            for (i=0; i<nflist; i++) {
                n = flist[i];
                sprintf (cline, listfmt, n, Xdata[n], Ydata[n], Zdata[n]);
                strcat (cbuf, cline);
            }
            csw_Free (flist);
        }
        strcat (cbuf, "\n\n");
    }

    if (FlatGridFlag == 0) {
        sprintf (cline,
"Minimum Z of data = %f,  Maximum Z of data = %f\n\
The data minimum is located at x = %f, y = %f\n\
The data maximum is located at x = %f, y = %f\n\n\
Minimum Z of grid = %f,  Maximum Z of grid = %f\n\
The grid minimum is located at x = %f, y = %f,  column %d and row %d\n\
The grid maximum is located at x = %f, y = %f,  column %d and row %d\n\n",
Zmin, Zmax,
Rxmin, Rymin,
Rxmax, Rymax,
Gmin, Gmax,
Rgxmin, Rgymin, Rmincol, Rminrow,
Rgxmax, Rgymax, Rmaxcol, Rmaxrow);
        strcat (cbuf, cline);

        sprintf (cline,
"Mean Z value of the data = %f,  Standard deviation of data = %f\n\
Mean Z value of the grid = %f,  Standard deviation of the grid = %f\n",
dmean, dstd, gmean, gstd);
        strcat (cbuf, cline);
    }
    else {
        sprintf (cline,
"\nThe grid is all at the same elevation, so no Z data statistics are reported\n\n");
        strcat (cbuf, cline);
    }

/*
    Max error, average error, standard deviation
*/
    sprintf (cline,
"\nMaximum absolute bilinear error = %f (%3.2f percent of data range)\n",
    MaxError, MaxError/zrange*100.);
    strcat (cbuf, cline);

    sprintf (cline,
"Average absolute bilinear error = %f (%3.2f percent of data range)\n",
    AvgError, AvgError/zrange*100.f);
    strcat (cbuf, cline);

    sprintf (cline,
"Standard deviation of bilinear error = %f (%3.2f percent of data range)\n\n",
    sdev, sdev/zrange*100.f);
    strcat (cbuf, cline);

/*
    Report the number of points with errors greater
    than certain percents of the data range.
*/
    f1000 = (CSW_F)n1000 / (CSW_F)Ndata * 100.f;
    f500 = (CSW_F)n500 / (CSW_F)Ndata * 100.f;
    f200 = (CSW_F)n200 / (CSW_F)Ndata * 100.f;
    f100 = (CSW_F)n100 / (CSW_F)Ndata * 100.f;
    f50 = (CSW_F)n50 / (CSW_F)Ndata * 100.f;

    sprintf (cline,
"  Number of bilinear errors greater than 1/1000th of the data range: %d (%.1f percent)\n",
    n1000, f1000);
    strcat (cbuf, cline);
    sprintf (cline,
"  Number of bilinear errors greater than 1/500th of the data range:  %d (%.1f percent)\n",
    n500, f500);
    strcat (cbuf, cline);
    sprintf (cline,
"  Number of bilinear errors greater than 1/200th of the data range:  %d (%.1f percent)\n",
    n200, f200);
    strcat (cbuf, cline);
    sprintf (cline,
"  Number of bilinear errors greater than 1/100th of the data range:  %d (%.1f percent)\n",
    n100, f100);
    strcat (cbuf, cline);
    sprintf (cline,
"  Number of bilinear errors greater than 1/50th of the data range:   %d (%.1f percent)\n",
    n50, f50);
    strcat (cbuf, cline);

/*
    Determine a format for lists that will line up the data
    correctly and use appropriate precision in the values.
*/
    if (OrigIdata) {
        for (i=0; i<nrep; i++) {
            j = replines[i];
            formatlist[i] = OrigIdata[j];
        }
        DetermineListFormat (formatlist, nrep, listfmt);
    }
    else {
        DetermineListFormat (replines, nrep, listfmt);
    }

/*
    Output the header for the list of worst points.
*/
    sprintf (cline,
"\nThe %d data points with the worst errors are listed below:\n\n", nrep);
    strcat (cbuf, cline);

    sprintf (cline, "\n\
                                                       Bicubic      Bilinear    Bilinear\n\
    Point          X            Y            Z          Error        Error      Error Pct\n\
-----------------------------------------------------------------------------------------\n");
    strcat (cbuf, cline);

/*
    Output the list of worst points.
*/
    for (i=0; i<nrep; i++) {

        j = replines[i];
        zt = Zerr[j];
        zt2 = zt / zrange * 100.f;
        if (zt2 < 0.0f) zt2 = -zt2;

        if (OrigIdata) {
            iline = OrigIdata[j];
        }
        else {
            iline = j;
        }

        sprintf (cline, listfmt,
                 iline+1, Xdata[j], Ydata[j], Zdata[j], ZerrBC[j], zt, zt2);
        strcat (cbuf, cline);
    }

    *report = cbuf;

    bsuccess = true;

    return 1;

}  /*  end of private GenerateReport function  */






/*
  ****************************************************************

          D e t e r m i n e L i s t F o r m a t

  ****************************************************************

    Return a format string that can be used to make a nice looking
  list of the worst data points for the grid report.

*/

int CSWGrdCalc::DetermineListFormat
        (int *lines, int nlines, char *fmt)
{
    int       i, i2, j;
    CSW_F     xt, xt2;
    char      c20[20];

/*
    Format for point numbers.
*/
    i2 = -1;
    for (i=0; i<nlines; i++) {
        j = lines[i];
        if (j > i2) i2 = j;
    }
    i2++;

    strcpy (c20, "   %7d   ");
    if (i2 < 1000000) strcpy (c20, "   %6d    ");
    if (i2 < 100000) strcpy (c20, "    %5d    ");
    if (i2 < 10000) strcpy (c20, "    %4d     ");
    if (i2 < 1000) strcpy (c20, "     %3d     ");
    if (i2 < 100) strcpy (c20, "     %2d      ");
    if (i2 < 10) strcpy (c20, "      %1d      ");

    strcpy (fmt, c20);

/*
    Format for x point locations.
*/
    xt = OrigXmax;
    xt2 = OrigXmin;
    if (xt2 < 0.0f) xt2 = -xt2;
    if (xt2 > xt) xt = xt2;
    strcpy (c20, " %11.0f ");
    if (xt < 1000000000) strcpy (c20, "  %10.0f ");
    if (xt < 10000000) strcpy (c20, "   %8.0f  ");
    if (xt < 1000000) strcpy (c20, "  %9.1f  ");
    if (xt < 10000) strcpy (c20, "  %8.2f   ");
    if (xt < 100) strcpy (c20, "   %7.3f   ");
    if (xt < 10) strcpy (c20, "   %7.4f   ");
    if (xt < 1) strcpy (c20, "   %7.5f   ");
    if (xt < .1f) strcpy (c20, "   %7.6f   ");

    strcat (fmt, c20);

/*
    Format for y point locations.
*/
    xt = OrigYmax;
    xt2 = OrigYmin;
    if (xt2 < 0.0f) xt2 = -xt2;
    if (xt2 > xt) xt = xt2;
    strcpy (c20, " %11.0f ");
    if (xt < 1000000000) strcpy (c20, "  %10.0f ");
    if (xt < 10000000) strcpy (c20, "   %8.0f  ");
    if (xt < 1000000) strcpy (c20, "  %9.1f  ");
    if (xt < 10000) strcpy (c20, "  %8.2f   ");
    if (xt < 100) strcpy (c20, "   %7.3f   ");
    if (xt < 10) strcpy (c20, "   %7.4f   ");
    if (xt < 1) strcpy (c20, "   %7.5f   ");
    if (xt < .1f) strcpy (c20, "   %7.6f   ");

    strcat (fmt, c20);

/*
    Format for z data values.
*/
    xt = Zmax;
    xt2 = Zmin;
    if (xt2 < 0.0f) xt2 = -xt2;
    if (xt2 > xt) xt = xt2;
    strcpy (c20, " %11.0f ");
    if (xt < 1000000000) strcpy (c20, " %11.0f ");
    if (xt < 10000000) strcpy (c20, "  %9.0f  ");
    if (xt < 1000000) strcpy (c20, "  %9.1f  ");
    if (xt < 10000) strcpy (c20, " %9.2f   ");
    if (xt < 100) strcpy (c20, "  %8.3f   ");
    if (xt < 10) strcpy (c20, "  %8.4f   ");
    if (xt < 1) strcpy (c20, "  %8.5f   ");
    if (xt < .1f) strcpy (c20, "  %8.6f   ");

    strcat (fmt, c20);

    if (FaultFmt) {
        strcat (fmt, "\n");
        return 1;
    }

/*
    Format for error values.
*/
    xt = MaxError;
    strcpy (c20, " %11.0f ");
    if (xt < 1000000000) strcpy (c20, "  %10.0f ");
    if (xt < 10000000) strcpy (c20, "   %8.0f  ");
    if (xt < 1000000) strcpy (c20, "  %9.1f  ");
    if (xt < 10000) strcpy (c20, "  %8.2f   ");
    if (xt < 100) strcpy (c20, "   %7.2f   ");
    if (xt < 10) strcpy (c20, "   %7.3f   ");
    if (xt < 1) strcpy (c20, "   %7.4f   ");

    strcat (fmt, c20);
    strcat (fmt, c20);

/*
    Final part of format is the error percent and newline
*/
    strcat (fmt, "    %5.2f\n");

    return 1;

}  /*  end of private DetermineListFormat function  */







/*
  ****************************************************************

                   S t r i k e A v e r a g e

  ****************************************************************

    Average the local data weighted on the pependicular distance
  of each point to a strike line.

*/

int CSWGrdCalc::StrikeAverage
         (CSW_F *x, CSW_F *y, CSW_F *z,
          int n, int irow, int jcol,
          CSW_F *val, CSW_F *stwgt)
{

    int       i, nc, astat;
    CSW_F     sum3, sum4, st, st0, x1, y1, x2, y2, tiny, dmin, dcrit,
              fdum, spower, dy, dx, slope, yint, storig;

/*
    Intialize output in case of an error return.
*/
    *stwgt = 0.0f;
    *val = 1.e30f;

/*
    Do nothing if the preferred strike and anisotropy are
    both turned off.
*/
    if (PreferredStrike < 0.0f  &&  AnisotropyFlag == 0) {
        return 0;
    }

/*
    Calculate endpoints for a line at the preferred
    strike with the grid node as origin.
*/
    astat = CalcStrikeLine (irow, jcol, &x1, &y1, &x2, &y2, &fdum);
    if (astat == 0) {
        return 0;
    }

    tiny = (Xspace + Yspace) / 200.0f;
    tiny *= tiny;
    st0 = tiny;
    dcrit = (Xmax - Xmin + Ymax - Ymin) / 200.0f;

/*
    The initial strike weight is determined by the StrikePower or
    by the anisotropy data.
*/
    spower = (CSW_F)StrikePower;
    st = spower / 2.0f;
    st *= st;
    storig = st;
    dcrit *= spower;

    if (spower > 2)
        tiny *= tiny;
    if (spower > 4)
        tiny *= st0;

/*
    If all points are on the same side of the strike line,
    reduce the strike weight.
*/
    dx = x2 - x1;
    dy = y2 - y1;
    if (dx < tiny  &&  dx > -tiny) {
        nc = 0;
        for (i=0; i<n; i++) {
            if (x[i] < 0.0f) nc++;
        }
        if (nc == 0  ||  nc == n) {
            st /= 10;
            dcrit /= spower;
        }
    }
    else if (dy < tiny  &&  dy > -tiny) {
        nc = 0;
        for (i=0; i<n; i++) {
            if (y[i] < 0.0f) nc++;
        }
        if (nc == 0  ||  nc == n) {
            st /= 10;
            dcrit /= spower;
        }
    }
    else {
        slope = dy / dx;
        yint = y1 - slope * x1;
        nc = 0;
        for (i=0; i<n; i++) {
            if (y[i] < slope * x[i] + yint) nc++;
        }
        if (nc == 0  ||  nc == n) {
            st /= 10;
            dcrit /= spower;
        }
    }

    *stwgt = st;

/*
    Calculate the weighted average of all points in the local
    data set based on the distance to the strike line.
*/
    sum3 = 0.0f;
    sum4 = 0.0f;
    dmin = 1.e30f;
    for (i=0; i<n; i++) {
        gpf_perpdsquare (x1, y1, x2, y2, x[i], y[i], &st0);
        if (st0 > 1.e15f  &&  spower > 2)
            st0 = 1.e15f;
        if (st0 > 1.e10f  &&  spower > 4)
            st0 = 1.e10f;
        if (st0 < tiny) st0 = tiny;
        st = st0;
        if (st < dmin) dmin = st;
        st = 1.0f / st;
        if (spower > 2) st *= st;
        if (spower > 4) st *= st0;
        sum3 += z[i] * st;
        sum4 += st;
    }

/*
    If the closest perpendicular distance if relatively large,
    reduce the strike weight.
*/
    dmin = (CSW_F)sqrt ((double)dmin);
    st = dcrit / dmin;
    st *= st;
    *stwgt *= st;
    if (*stwgt > storig) *stwgt = storig;

    if (sum4 > 0.0f) {
        *val = sum3 / sum4;
        return 1;
    }
    else {
        return 0;
    }

}  /*  end of private StrikeAverage function  */




/*
  ****************************************************************

                    C a l c S t r i k e L i n e

  ****************************************************************

    Return the two points defining a strike line at a grid node.
  Also return the "raw" strike weight at the node.

*/

int CSWGrdCalc::CalcStrikeLine
          (int irow, int jcol,
           CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2,
           CSW_F *swgt)
{
    CSW_F     wt, strike, spower, st;
    int       ic, jc, kc;

/*
    Get the preferred strike, strike power and anisotropy ratio
    from the local anisotropy grids if needed.
*/
    if (AnisotropyFlag == GRD_LOCAL_ANISOTROPY) {
        ic = irow / NcoarseOrig;
        jc = jcol / NcoarseOrig;
        kc = ic * Nccol + jc;
        PreferredStrike = AnisoStrike[kc];
        StrikePower = (int)(AnisoPower[kc] / 10.0f);
        AnisotropyRatio = AratioGrid[kc];
        if (PreferredStrike < -500.0f) PreferredStrike = -1000.0f;
        if (StrikePower < 2) StrikePower = 2;
        if (StrikePower > 6) StrikePower = 6;
    }

/*
    Calculate endpoints for a line at the preferred
    strike with the grid node as origin.
*/
    strike = PreferredStrike * .01745329f;
    if (strike < 0) {
        return 0;
    }
    if (strike > 3.1415926f)
        strike -= 3.1415926f;

    wt = Xspace + Yspace;
    *x1 = (CSW_F)cos ((double)strike);
    *y1 = (CSW_F)sin ((double)strike);
    *x1 *= wt * 40;
    *y1 *= wt * 40;
    *x2 = -*x1;
    *y2 = -*y1;

/*
    The initial strike weight is determined by the StrikePower or
    by the anisotropy data.
*/
    spower = (CSW_F)StrikePower;
    st = spower / 2.0f;
    *swgt = st + 1;

    return 1;

}  /*  end of private function CalcStrikeLine  */






/*
  ****************************************************************

          F i l t e r P o i n t s F o r S t r i k e

  ****************************************************************

  Return the relatively closest points to the specified strike
  line from the local data set.

*/

int CSWGrdCalc::FilterPointsForStrike
         (int *list1, int n1, int *list2, int *n2,
          int nmax, int nquad, int irow, int jcol,
          CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2)
{

    int          i, j, k, kk, n, jmin, maxm2, maxloc, ipt;
    int          *count = ploc_int1, *bin = ploc_int2;
    CSW_F        dt, pd1, pd2, x, y, x0, y0,
                 xmin, ymin, xmax, ymax, dz, z1, z2;
    CSW_F        *pdist = ploc_f1, pdmax, *zdist = ploc_f2;

/*
    Initialize some local arrays to suppress lint messages.
*/
    memset ((char *)zdist, 0, MAX_LOCAL * sizeof(CSW_F));
    memset ((char *)pdist, 0, MAX_LOCAL * sizeof(CSW_F));
    memset ((char *)bin, 0, MAX_LOCAL * sizeof(int));

/*
    Calculate the coordinates at the grid node.
*/
    x0 = jcol * Xspace + Xmin;
    y0 = irow * Yspace + Ymin;

/*
    The maximum number of output points is higher if
    the grid node is covered in many directions.  These
    numbers result from a lot of experimenting with
    various data sets.
*/
    if (nquad == 4) {
        maxm2 = 13;
    }
    else if (nquad == 3) {
        maxm2 = 10;
    }
    else if (nquad == 2) {
        maxm2 = 7;
    }
    else {
        maxm2 = 5;
    }

    if (maxm2 > Ndata) maxm2 = Ndata;

/*
    Allow more points output if the strike strength is lower.
    This rather complicated sequence is a result of much
    experimenting to find a good expansion factor vs. anisotropy
    ratio relationship.
*/
    if (AnisotropyFlag) {
        dt = 7.0f - AnisotropyRatio;
        if (dt < 2.0f) dt = 2.0f;
        if (AnisotropyFlag == 2) dt *= 2.0f;
        dt /= 2.0f;
        dt *= dt;
        i = (int)dt;
        maxm2 += i;
    }

    if (maxm2 > Ndata) maxm2 = Ndata;

    if (FaultedFlag == 0) {
        nmax /= 2;
    }
    if (nmax > 20) nmax = 20;
    if (nmax < 5) nmax = 5;

/*
    put perpendicular distances into the pdist array
    and get the bounding box of the local region.
*/
    pd2 = -1.e30f;
    xmin = 1.e30f;
    ymin = 1.e30f;
    xmax = -1.e30f;
    ymax = -1.e30f;
    for (i=0; i<n1; i++) {
        x = Xdata[list1[i]] - x0;
        y = Ydata[list1[i]] - y0;
        zdist[i] = Zdata[list1[i]];
        gpf_perpdsquare (x1, y1, x2, y2, x, y, &dt);
        pdist[i] = (CSW_F)sqrt ((double)dt);
        if (pdist[i] > pd2) pd2 = pdist[i];
        if (x < xmin) xmin = x;
        if (y < ymin) ymin = y;
        if (x > xmax) xmax = x;
        if (y > ymax) ymax = y;
    }

/*
    Set up bins so that zero to max distance is covered
    by _NBINS intervals.

    Note that it is possible for pd1 and pd2 to be zero if
    all the points are exactly on the strike line.  In this
    case, all the points will be in the first bin of the histogram.
    If there are more than maxloc points in that first bin, then
    whichever maxloc points were put in first are used.
*/
    pd1 = pd2 * 1.01f / (CSW_F)_NBINS;

/*
    Build the perpendicular distance histogram.
*/
    memset ((char *)count, 0, _NBINS * sizeof(int));
    for (i=0; i<n1; i++) {
        if (pd1 > 0.0) {
            j = (int)(pdist[i] / pd1);
        }
        else {
            j = 0;
        }
        if (j < 0) j = 0;
        if (j > _NBINS - 1) j = _NBINS - 1;
        bin[i] = j;
        count[j]++;
    }

/*
    find the bin where the total number of points
    exceeds the specified nmax.
*/
    n = 0;
    j = 0;
    jmin = -1;
    if (pd1 > 0.0) {
        while (n < nmax  &&  j < _NBINS) {
            if (count[j] > 0  &&  jmin == -1) jmin = j;
            n += count[j];
            j++;
        }

        /*
          the maximum permitted perpendicular distance is
          a function of the bounding box and the StrikePower
        */
        pdmax = (xmax - xmin + ymax - ymin) / (CSW_F)StrikePower;

        if (j * pd1 >= pdmax) j = (int)(pdmax / pd1 + .5f);
        if (jmin == -1) jmin = j;
        j /= 2;
        if (j < jmin+1) j = jmin+1;
    }
    else {
        jmin = 0;
        j = 1;
    }

/*
    Output the points having bin values less
    than the determined cutoff.  This is done
    by outputting the closer points first followed
    by more outlying points.  The output is stopped
    when enough points have been found.
*/
    n = 0;
    dz = Zrange / 10000.0f;
    z1 = 1.e30f;
    z2 = -1.e30f;

    maxloc = maxm2 * 2;
    if (maxloc > Ndata) maxloc = Ndata;

    kk = 0;
    for (k=0; k<_NBINS; k++) {
        if (count[k] == 0) continue;
        kk++;
        for (i=0; i<n1; i++) {
            if (bin[i] == k) {
                ipt = list1[i];
                if (Xdata[ipt] >= x0  &&  Ydata[ipt] >= y0) {
                    Firstq1 = -1;
                }
                if (Xdata[ipt] < x0  &&  Ydata[ipt] >= y0) {
                    Firstq2 = -1;
                }
                if (Xdata[ipt] < x0  &&  Ydata[ipt] < y0) {
                    Firstq3 = -1;
                }
                if (Xdata[ipt] >= x0  &&  Ydata[ipt] < y0) {
                    Firstq4 = -1;
                }
                list2[n] = ipt;
                n++;
                if (zdist[i] < z1) z1 = zdist[i];
                if (zdist[i] > z2) z2 = zdist[i];
            }
            if (n >= maxloc) break;
        }

    /*
        If the z values are not nearly identical, and there are
        enough points to define a plane, that is good enough.
    */
        if (n > maxm2  &&  z2 - z1 > dz) {
            break;
        }

    /*
        If the z values are nearly identical, more than
        one bin must have been sampled to satisfy the
        filter conditions.  This makes a colinear data
        set very unlikely.
    */
        if (kk > 2) {
            if (n > maxm2) break;
        }
    }

    *n2 = n;

    return 1;

}  /*  end of private FilterPointsForStrike function  */







/*
  ****************************************************************

               C a l c L o c a l A n i s o t r o p y

  ****************************************************************

    Calculate grids at the coarse interval of the local strike, power
  and anisotropy ratio.

*/

int CSWGrdCalc::CalcLocalAnisotropy (void)
{
    int          i, j, k, k2, i1, i2, j1, j2, ii, jj, start, iend, jend,
                 offset, off2, list[1000], nlist, nquad,
                 searchsave, aflagsave, nlocalsave, maxnodesave,
                 nav, nminus, ndone, power, strikesearchsave;
    CSW_F        aratio, sum1, sum2, sum3, sumcrit, spread, angles[8], strike;
    CSW_F        awork1[5000], awork2[5000], awork3[5000];

/*
    Initialize some local arrays to suppress lint warnings.
*/
    memset (awork1, 0, 5000 * sizeof(CSW_F));
    memset (awork2, 0, 5000 * sizeof(CSW_F));
    memset (awork3, 0, 5000 * sizeof(CSW_F));

/*
    Save some of the private variable search parameters.
*/
    strikesearchsave = StrikeSearch;
    maxnodesave = MaxNodeDistance;
    nlocalsave = NumLocalPoints;
    searchsave = LocalSearchPattern;
    aflagsave = AnisotropyFlag;

/*
    Reset search parameters for local anisotropy points.
*/
    StrikeSearch = 0;
    MaxNodeDistance = 100000;
    NumLocalPoints = 100000;
    LocalSearchPattern = GRD_RADIAL_SEARCH;
    AnisotropyFlag = 0;

    for (k=0; k<Nccol*Ncrow; k++) {
        AnisoStrike[k] = -1000.0f;
        AnisoPower[k] = 0.0f;
        AratioGrid[k] = 1.0f;
    }

/*
    Find the local points in the vicinity of each coarse
    grid node and use them as the base points for local
    anisotropy calculations.
*/
    for (i=0; i<Ncrow; i+=2) {

        ii = i * Ncoarse;
        offset = ii * Ncol;
        off2 = i * Nccol;

        for (j=0; j<Nccol; j+=2) {

            jj = j * Ncoarse;
            k = off2 + j;

            start = ClosestPoint[offset+jj];
            if (start < 0) start = 0;
            if (start > Ncoarse) continue;
            iend = start * 2;
            if (iend < Ncoarse + 1) iend = Ncoarse + 1;
            jend = iend;

            for (;;) {
                nlist = 0;
                CollectLocalPoints (ii, jj, start, jend,
                                    100, 100, list, &nlist,
                                    &nquad);
                if (nlist > 5  ||  jend > 2 * iend) {
                    break;
                }
                jend += Ncoarse;
            }

            if (nlist < 5) continue;

            grd_stats_ptr->grd_local_anisotropy (list, nlist,
                                  Xdata, Ydata, Zdata, Ndata,
                                  &strike, &power, &aratio, ThicknessFlag);
            if (aratio > 9.0f) aratio = 9.0f;

            AnisoStrike[k] = (CSW_F)strike;
            AnisoPower[k] = (CSW_F)(power * 10.0f);
            AratioGrid[k] = aratio;

        }
    }

/*
    Reset private variable search parameters.
*/
    StrikeSearch = strikesearchsave;
    MaxNodeDistance = maxnodesave;
    NumLocalPoints = nlocalsave;
    LocalSearchPattern = searchsave;
    AnisotropyFlag = aflagsave;

/*
    Fill in local anisotropy grid where needed.  For each loop, fill in
    unset nodes that have at least 3 set neighbors.  If an iteration
    has no nodes filled in, but still has some needed, then reduce the
    required number of neighbors.  Once all nodes are filled in, the
    loop is exited.
*/
    sumcrit = 1.1f;
    for (;;) {

        ndone = 0;
        nminus = 0;

        for (i=0; i<Ncrow; i++) {

            offset = i * Nccol;
            i1 = i - 1;
            if (i1 < 0) i1 = 0;
            i2 = 1 + 1;
            if (i2 > Ncrow - 1) i2 = Ncrow - 1;

            for (j=0; j<Nccol; j++) {

                k = offset + j;

            /*
                Average adjacent nodes for unset nodes.
            */
                if (AnisoStrike[k] < -500.0f) {

                    nminus++;
                    j1 = j - 1;
                    if (j1 < 0) j1 = 0;
                    j2 = j + 1;
                    if (j2 > Nccol-1) j2 = Nccol - 1;

                    nav = 0;
                    sum1 = 0.0f;
                    sum2 = 0.0f;
                    sum3 = 0.0f;

                    for (ii=i1; ii<=i2; ii++) {
                        off2 = ii * Nccol;
                        for (jj=j1; jj<=j2; jj++) {
                            k2 = off2 + jj;
                            if (AnisoStrike[k2] > -500.0f) {
                                sum1 += 1.0f;
                                sum2 += AnisoPower[k2];
                                sum3 += AratioGrid[k2];
                                angles[nav] = AnisoStrike[k2];
                                nav++;
                            }
                        }
                    }

                /*
                    Fill in the node if there are enough valid neighbors.
                */
                    if (sum1 > sumcrit  &&  nav > 0) {
                        awork2[k] = sum2 / sum1;
                        if (awork2[k] > 40) awork2[k] = 40.0f;
                        awork3[k] = sum3 / sum1;
                        grd_utils_ptr->grd_average_angles (angles, nav, awork1+k, &spread);
                        awork2[k] /= spread;
                        ndone++;
                    }
                    else {
                        awork1[k] = -1000.0f;
                        awork2[k] = 0.0f;
                        awork3[k] = 1.0f;
                    }
                }

                else {
                    awork1[k] = AnisoStrike[k];
                    awork2[k] = AnisoPower[k];
                    awork3[k] = AratioGrid[k];
                }

            }
        }

    /*
        Copy the work grids back into the Aniso data grids.
    */
        k = Nccol * Ncrow * sizeof(CSW_F);
        memcpy ((char *)AnisoStrike, (char *)awork1, k);
        memcpy ((char *)AnisoPower, (char *)awork2, k);
        memcpy ((char *)AratioGrid, (char *)awork3, k);

    /*
        Break the loop if all nodes are done or decrease
        the required number of neighbors if there is a problem.
    */
        if (ndone == 0) {
            if (nminus == 0) {
                sumcrit -= 1.0f;
                if (sumcrit < 0.2f) {
                    break;
                }
                continue;
            }
            break;
        }

    }  /*  end of while (1) loop  */

    return 1;

}  /*  end of private CalcLocalAnisotropy function  */





/*
  ****************************************************************

                  F i l l I n S t e p G r i d

  ****************************************************************

    After a step grid (closest point grid) has been calculated at
  the coarse interval, fill it in at the fine interval.

*/

int CSWGrdCalc::FillInStepGrid (void)
{
    int         ii, jj, i, j, k, n, off1, nc3, j1, i1,
                i2, cp1, list[1000], nlist, nquad;
    CSW_F       z1, z2, z3, z4, xt, yt;
    CSW_F       xloc[1000], yloc[1000], zloc[1000];

    TinyGraze = (Zmax - Zmin) / 10000.0f;
    nc3 = Ncoarse * Ncol;

    for (i=0; i<Nrow-1; i+=Ncoarse) {

        off1 = i * Ncol;
        i1 = 1;
        if (i == 0) i1 = 0;

        for (j=0; j<Ncol-1; j+=Ncoarse) {

            j1 = 1;
            if (j == 0) j1 = 0;

            z1 = Grid[off1+j];
            z2 = Grid[off1+j+Ncoarse];
            z3 = Grid[off1+nc3+j+Ncoarse];
            z4 = Grid[off1+nc3+j];

        /*
            If all four corners of the coarse grid are the same,
            fill in with a uniform value.
        */
            if (StepGraze(z1,z2)  &&  StepGraze(z1,z3)  &&  StepGraze(z1,z4)) {
                for (ii=i+i1; ii<=i+Ncoarse; ii++) {
                    i2 = ii*Ncol;
                    for (jj=j+j1; jj<=j+Ncoarse; jj++) {
                        Grid[i2+jj] = z1;
                    }
                }
            }

        /*
            The 4 corners differ so search for the closest data point
            at each node inside the coarse grid cell.
        */
            else {

                for (ii=i+i1; ii<=i+Ncoarse; ii++) {
                    i2 = ii*Ncol;
                    yt = ii * Yspace + Ymin;
                    for (jj=j+j1; jj<=j+Ncoarse; jj++) {
                        xt = jj * Xspace + Xmin;
                        cp1 = ClosestPoint[i2+jj];
                        if (cp1 < 0) cp1 = 0;
                        CollectLocalPoints (ii, jj, cp1, cp1*2,
                                            100, 500, list, &nlist, &nquad);
                        for (k=0; k<nlist; k++) {
                            n = list[k];
                            xloc[k] = Xdata[n] - xt;
                            yloc[k] = Ydata[n] - yt;
                            zloc[k] = Zdata[n];
                        }
                        grd_utils_ptr->grd_get_closest_point_value
                                             (xloc, yloc, zloc, nlist,
                                              Grid+i2+jj);
                    }
                }

            }

        }  /*  end of j loop through a column of the coarse grid.  */

    }  /*  end of i loop through rows of the coarse grid  */

    return 1;

}  /*  end of private FillInStepGrid function  */





/*
  ****************************************************************

                       S t e p G r a z e

  ****************************************************************

*/

int CSWGrdCalc::StepGraze (CSW_F z1, CSW_F z2)
{
    CSW_F         z3;

    z3 = z2 - z1;
    if (z3 < 0.0f) z3 = -z3;

    if (z3 < TinyGraze) {
        return 1;
    }

    return 0;

}  /*  end of private StepGraze function  */



/*
  ****************************************************************

        E x t e n d F a u l t e d C o a r s e N o d e s

  ****************************************************************

    Assign elevations to some uncalculated coarse nodes by extending
  the gradients defined by near neighbor nodes that have been
  calculated.  This may be called multiple times to extend further.

    If an extension would cross a fault, it is not done.

*/

int CSWGrdCalc::ExtendFaultedCoarseNodes (int iter, int nmin)
{

    int        i, j, i1, i2, j1, j2, off1, off2, cp,
               astat, n, istat1, istat2, closef;
    CSW_F      fiter, z1, z2, zt, sum3, sum4,
               x1, y1, x2, y2, st, fdum, minusvalue,
               xsp2, ysp2, tiny, st0, fwgt;
    CSW_F      grads[10], values[10], tmax, gwgt,
               gradvalue, gs1, gs2;
    int        kk, numpos;

    minusvalue = 0.0f;
    if (ThicknessFlag == 1) {
        minusvalue = Zrange / 20.0f * (iter + 1);
        minusvalue = -minusvalue;
    }

    memset (grads, 0, 10 * sizeof(CSW_F));
    memset (values, 0, 10 * sizeof(CSW_F));

/*
    Calculate endpoints for a line at the preferred
    strike with the grid node as origin.
*/
    xsp2 = Xspace * Ncoarse;
    ysp2 = Yspace * Ncoarse;
    tiny = (xsp2 + ysp2) / 200.0f;
    tiny *= tiny;

    astat = 0;

/*
    weight of the extrapolated gradient goes down
    with more iterations through this function.
*/
    fiter = (CSW_F)iter;
    if (PreferredStrike >= 0  ||  AnisotropyFlag) {
        fiter--;
    }

    fiter /= 3.0f;
    if (ThicknessFlag == 1) {
        fiter /= 2.0f;
    }

    if (fiter < 0.0f) fiter = 0.0f;

/*
    loop through the coarse nodes looking for uncalculated
    locations.
*/
    off2 = 0;
    numpos = 0;
    for (i=0; i<Nrow; i+=Ncoarse) {

        off1 = Ncol * i;

        for (j=0; j<Ncol; j+=Ncoarse) {

            astat = CalcStrikeLine (i, j, &x1, &y1, &x2, &y2, &fdum);

        /*
            Transfer already calculated nodes directly to the results grid.
        */
            if (Grid[off1+j] < 1.e29f) {
                Gwork1[off1+j] = Grid[off1+j];
                continue;
            }

        /*
            Find and average the local gradients if possible.
        */
            sum3 = 0.0f;
            sum4 = 0.0f;
            n = 0;

        /*
            lower left gradient
        */
            i1 = i - Ncoarse;
            i2 = i - 2 * Ncoarse;
            j1 = j - Ncoarse;
            j2 = j - 2 * Ncoarse;
            closef = 1;
            istat1 = 0;
            istat2 = 0;
            if (ClosestFault[i*Ncol+j] > 2 * Ncoarse) {
                closef = 0;
            }
            if (closef) {
                istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j1, i1, &fwgt);
                istat2 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j2, i2, &fwgt);
            }
            if (i2 >= 0  &&  j2 >= 0  &&  istat1 == 0  &&  istat2 == 0) {
                z1 = Grid[i1*Ncol+j1];
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, -xsp2, -ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    grads[n] = z2 - z1;
                    values[n] = zt;
                    n++;
                }
            }

        /*
            lower center gradient
        */
            j1 = j;
            j2 = j;
            if (closef) {
                istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j1, i1, &fwgt);
                istat2 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j2, i2, &fwgt);
            }
            if (i2 >= 0  &&  istat1 == 0  &&  istat2 == 0) {
                z1 = Grid[i1*Ncol+j1];
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, 0.0f, -ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    grads[n] = z2 - z1;
                    values[n] = zt;
                    n++;
                }
            }

        /*
            lower right gradient
        */
            j1 = j + Ncoarse;
            j2 = j + 2 * Ncoarse;
            if (closef) {
                istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j1, i1, &fwgt);
                istat2 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j2, i2, &fwgt);
            }
            if (i2 >= 0  &&  j2 < Ncol  &&  istat1 == 0  &&  istat2 == 0) {
                z1 = Grid[i1*Ncol+j1];
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, xsp2, -ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    grads[n] = z2 - z1;
                    values[n] = zt;
                    n++;
                }
            }

        /*
            left gradient
        */
            i1 = i;
            i2 = i;
            j1 = j - Ncoarse;
            j2 = j - 2 * Ncoarse;
            if (closef) {
                istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j1, i1, &fwgt);
                istat2 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j2, i2, &fwgt);
            }
            if (j2 >= 0  &&  istat1 == 0  &&  istat2 == 0) {
                z1 = Grid[i1*Ncol+j1];
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, -xsp2, 0.0f, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    grads[n] = z2 - z1;
                    values[n] = zt;
                    n++;
                }
            }

        /*
            right gradient
        */
            j1 = j + Ncoarse;
            j2 = j + 2 * Ncoarse;
            if (closef) {
                istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j1, i1, &fwgt);
                istat2 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j2, i2, &fwgt);
            }
            if (j2 < Ncol  &&  istat1 == 0  &&  istat2 == 0) {
                z1 = Grid[i1*Ncol+j1];
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, xsp2, 0.0f, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    grads[n] = z2 - z1;
                    values[n] = zt;
                    n++;
                }
            }

        /*
            upper left gradient
        */
            i1 = i + Ncoarse;
            i2 = i + 2 * Ncoarse;
            j1 = j - Ncoarse;
            j2 = j - 2 * Ncoarse;
            if (closef) {
                istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j1, i1, &fwgt);
                istat2 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j2, i2, &fwgt);
            }
            if (i2 < Nrow  &&  j2 >= 0  &&  istat1 == 0  &&  istat2 == 0) {
                z1 = Grid[i1*Ncol+j1];
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, -xsp2, ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    grads[n] = z2 - z1;
                    values[n] = zt;
                    n++;
                }
            }

        /*
            upper center gradient
        */
            j1 = j;
            j2 = j;
            if (closef) {
                istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j1, i1, &fwgt);
                istat2 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j2, i2, &fwgt);
            }
            if (i2 < Nrow  &&  istat1 == 0  &&  istat2 == 0) {
                z1 = Grid[i1*Ncol+j1];
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, 0.0f, ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    grads[n] = z2 - z1;
                    values[n] = zt;
                    n++;
                }
            }

        /*
            upper right gradient
        */
            j1 = j + Ncoarse;
            j2 = j + 2 * Ncoarse;
            if (closef) {
                istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j1, i1, &fwgt);
                istat2 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, j2, i2, &fwgt);
            }
            if (i2 < Nrow  &&  j2 < Ncol  &&  istat1 == 0  &&  istat2 == 0) {
                z1 = Grid[i1*Ncol+j1];
                z2 = Grid[i2*Ncol+j2];
                if (z1 < 1.e29f  &&  z2 < 1.e29f) {
                    if (z1 > 0.0f) numpos++;
                    zt = z1 - (z2 - z1);
                    zt = (fiter * z1 + zt) / (fiter + 1.0f);
                    if (astat) {
                        gpf_perpdsquare (x1, y1, x2, y2, xsp2, ysp2, &st0);
                        if (st0 < tiny) {st0 = tiny; n+=nmin;}
                        st = st0;
                        st = 1.0f / st;
                        if (StrikePower > 2) st *= st;
                        if (StrikePower > 4) st *= st0;
                        zt *= st;
                        sum3 += zt;
                        sum4 += st;
                    }
                    else {
                        sum3 += zt;
                        sum4++;
                    }
                    grads[n] = z2 - z1;
                    values[n] = zt;
                    n++;
                }
            }

            tmax = 0.0f;
            for (kk=0; kk<n; kk++) {
                if (grads[kk] < 0.0) grads[kk] = -grads[kk];
                if (grads[kk] > tmax) tmax = grads[kk];
            }

            gs1 = 0.0f;
            gs2 = 0.0f;
            if (tmax > 0.0) {
                for (kk=0; kk<n; kk++) {
                    gwgt = grads[kk] / tmax;
                    gwgt *= gwgt;
                    gs1 += values[kk] * gwgt;
                    gs2 += gwgt;
                }
            }
            else {
                for (kk=0; kk<n; kk++) {
                    gwgt = 1.0;
                    gs1 += values[kk] * gwgt;
                    gs2 += gwgt;
                }
            }


            if (n > 0  &&  gs2 > 0.0) {
                gradvalue = gs1 / gs2;
            }
            else {
                gradvalue = 1.e30f;
            }

            gradvalue = gradvalue;

        /*
            Put the average of valid gradient extensions into the
            results grid, or assign 1.e30f as the result if there
            are no valid gradient extensions.
        */
            if (ClosestFault[i*Ncol+j] <= Ncoarse) {
                if (nmin > 2) nmin = 2;
            }
            if (sum4 > 0.0f  &&  n >= nmin) {
                Gwork1[off1+j] = sum3 / sum4;
                if (ThicknessFlag == 1) {
                    cp = ClosestPoint[off1+j];
                    if (cp < 0) cp = 0;
                    if (ZeroCoarse[off2] == 1) {
                        if (Gwork1[off1+j] > minusvalue * 10.0f) {
                            Gwork1[off1+j] = minusvalue * 10.0f;
                        }
                    }
                    else if (cp > 2 * Ncoarse  &&  numpos == 0) {
                        if (Gwork1[off1+j] > minusvalue) {
                            Gwork1[off1+j] = minusvalue;
                        }
                    }
                }
            }
            else {
                Gwork1[off1+j] = 1.e30f;
            }
            off2++;

        }  /*  end of j column loop  */

    }  /*  end of i row loop  */

/*
    Transfer back to Grid array
*/
    for (i=0; i<Nrow; i+=Ncoarse) {
        off1 = i * Ncol;
        for (j=0; j<Ncol; j+=Ncoarse) {
            Grid[off1+j] = Gwork1[off1+j];
        }
    }

    return 1;

}  /*  end of private ExtendFaultedCoarseNodes function  */




/*
  ****************************************************************

          S t r i k e S m o o t h F a u l t e d G r i d

  ****************************************************************

  Smooth the grid with a 3 by 3 moving average.  The smoothed
  value is combined with the original value depending upon the
  closest data point to the node and the smoothing factor.  A
  radial smoothing estimate is combined with an estimate based
  on the preferred strike specified for the surface.

    This smoothing function is only called for the coarsest grid
  interval.  As the grid is refined, radial smoothing only is
  done by calling SimpleSmoothGrid.

*/

int CSWGrdCalc::StrikeSmoothFaultedGrid (void)
{

    int       i, j, k, offset, off2, ki, kj, astat;
    int       istat1, cp, istat, closef, ismt;
    CSW_F     wt, avg, sum3, sum4, st, x1, y1, x2, y2, x, y;
    CSW_F     st0, stiny, fdum, fwgt, cpmult, fncc, datafact;
    CSW_F     zgrid;
    char      flags[9];
    int       kflag;

    if (StepGridFlag  ||  Ndata < 4  ||  TrendSmoothFlag == 1) {
        return 1;
    }

/*
    Calculate endpoints for a line at the preferred
    strike with the grid node as origin.
*/
    if (PreferredStrike < 0.0f  &&  AnisotropyFlag == 0) {
        return 1;
    }

    wt = Xspace + Yspace;
    stiny = wt / 200.f;
    stiny *= stiny;

    datafact = (CSW_F)Ndata / 2000.0f;
    if (datafact < 0.1f) datafact = 0.1f;
    if (SmoothingFactor > 0.0) {
        datafact *= SmoothingFactor;
    }
    if (datafact > 1.0f) {
        datafact = (CSW_F)sqrt ((double)datafact);
        datafact = (CSW_F)sqrt ((double)datafact);
    }

    if (FastGridFlag) {
        datafact = 10.0f;
    }

    ismt = (int)(SmoothingFactor * 2.0f + .5f);
    if (ismt < 1) ismt = 1;

    fncc = (CSW_F)Ncoarse + .5f;
    if (Ncoarse == 1) {
        datafact = (CSW_F)Ndata / 2000.0f;
        if (FastGridFlag) datafact = (CSW_F)Ndata / 5000.0f;
        if (DistancePower > 2) datafact /= 2.0f;
        if (DistancePower > 4) datafact /= 2.0f;
        if (datafact < 0.1f) datafact = 0.1f;
        if (SmoothingFactor > 0.0) {
            datafact *= SmoothingFactor;
        }
        if (datafact > 1.0f) {
            datafact = (CSW_F)sqrt ((double)datafact);
            datafact = (CSW_F)sqrt ((double)datafact);
        }
        if (datafact > 10.0f) datafact = 10.0f;
        fncc = 2.0f;
    }

/*
    Smooth and put the smoothed results into the Gwork1 array
*/
    for (i=0; i<Nrow; i+=Ncoarse) {

        offset = i * Ncol;

        for (j=0; j<Ncol; j+=Ncoarse) {

            k = offset + j;
            astat = CalcStrikeLine (i, j, &x1, &y1, &x2, &y2, &fdum);

        /*
            Use the perpendicular distance from the node neighbors to the
            strike line as the weighting factor for the average of the nodes.
        */
            sum3 = 0.0f;
            sum4 = 0.0f;
            closef = 1;
            if (ClosestFault[i*Ncol+j] > Ncoarse) {
                closef = 0;
                istat1 = 0;
            }

        /*
         * Set the flags array to 1 if the node is blocked
         * by a fault or zero if not blocked.
         */
            memset (flags, 0, 9 * sizeof(char));
            kflag = 0;
            if (closef) {
                for (ki=i-Ncoarse; ki<=i+Ncoarse; ki+=Ncoarse) {
                    if (ki<0  ||  ki>=Nrow) {
                        kflag += 3;
                        continue;
                    }
                    off2 = ki*Ncol;
                    for (kj=j-Ncoarse; kj<=j+Ncoarse; kj+=Ncoarse) {
                        if (kj == j  &&  ki == i) {
                            kflag++;
                            continue;
                        }
                        istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, kj, ki, &fwgt);
                        if (istat1 != 0) flags[kflag] = 1;
                        kflag++;
                    }
                }
            }

        /*
         * If a node is blocked, then do not use its mirror about
         * the center either.  This eliminates phase shifts near
         * faults.
         */
            for (ki=0; ki<9; ki++) {
                if (ki == 4) continue;
                if (flags[8-ki] == 1) {
                    flags[ki] = 1;
                }
            }

        /*
         * Only average the node values with flags set to zero.
         */
            kflag = 0;
            for (ki=i-Ncoarse; ki<=i+Ncoarse; ki+=Ncoarse) {
                if (ki<0  ||  ki>=Nrow) {
                    kflag += 3;
                    continue;
                }
                off2 = ki*Ncol;
                y = (ki - i) * Yspace;
                y /= Ncoarse;
                for (kj=j-Ncoarse; kj<=j+Ncoarse; kj+=Ncoarse) {
                    if (kj == j  &&  ki == i) {
                        kflag++;
                        continue;
                    }
                    if (closef) {
                        if (flags[kflag]) {
                            kflag++;
                            continue;
                        }
                        kflag++;
                    }
                    if (kj>=0  &&  kj<Ncol) {
                        if (astat == 1) {
                            x = (kj - j) * Xspace;
                            x /= Ncoarse;
                            gpf_perpdsquare (x1, y1, x2, y2, x, y, &st0);
                            if (st0 < stiny) st0 = stiny;
                            st = st0;
                            st = 1.0f / st;
                            if (StrikePower > 2) st *= st;
                            if (StrikePower > 4) st *= st0;
                        }
                        else {
                            st = 1.0f;
                        }
                        sum3 += Grid[off2+kj] * st;
                        sum4 += st;
                    }
                }
            }

            if (sum4 > 0.0f) {
                avg = sum3 / sum4;
            }
            else {
                avg = Grid[k];
            }

        /*
            Combine the smoothed and original value according to
            the distance from the closest data point at the node.
        */
            cp = ClosestPoint[k];
            if (cp <= Ncoarse) {
                istat = CheckClosePointFaulting (i, j, Ncoarse);
                if (istat == -1  &&  cp >= -3) {
                    cp = Ncoarse + 1;
                }
            }
            zgrid = Grid[k];
            if (cp < -3) {
                cp = -cp;
                cp -= 4;
                zgrid = Zdata[cp];
                cp = 0;
                cpmult = .01f;
            }
            else {
                cpmult = 1.0f;
                if (cp == -3) {
                    cpmult = .01f;
                }
                else if (cp == -2) {
                    cpmult = .05f;
                }
                else if (cp == -1) {
                    cpmult = .25f;
                }
            }
            wt = (CSW_F)(cp + ismt) / fncc;
            wt *= datafact;
            wt *= wt;
            wt *= cpmult;
            Gwork1[k] = (zgrid + avg * wt) / (1.0f + wt);

        }  /*  end of j loop through columns  */

    }  /*  end of i loop through rows  */

/*
    Copy the smoothed results back to the Grid array.
*/
    for (i=0; i<Nrow; i+=Ncoarse) {
        offset = i * Ncol;
        for (j=0; j<Ncol; j+=Ncoarse) {
            Grid[offset+j] = Gwork1[offset+j];
        }
    }

    return 1;

}  /*  end of private StrikeSmoothFaultedGrid function  */




/*
  ****************************************************************

            S m o o t h F a u l t e d E r r o r s

  ****************************************************************

      Smooth the error grid (Gwork1) and output to Gwork2.
  This is a simple 3 by 3 moving average.  The smoothing
  factor and strike direction are ignored.

*/

int CSWGrdCalc::SmoothFaultedErrors (void)
{
    int       i, j, k, offset, off2, ki, kj, istat1, closef;
    CSW_F     sum, sum2, cwgt, fwgt;
    char      flags[9];
    int       kflag;

    if (StepGridFlag) {
        return 1;
    }

/*
    Smooth the errors less if the distance poswer is greater than 2.
*/
    cwgt = 0.0f;
    if (DistancePower > 2) {
        i = DistancePower / 2;
        cwgt = (CSW_F)i;
        cwgt *= cwgt;
    }

/*
    Smooth and put the smoothed results into the Gwork1 array
*/
    for (i=0; i<Nrow; i+=Ncoarse) {

        offset = i * Ncol;

        for (j=0; j<Ncol; j+=Ncoarse) {

    /*
        Sum the values of the immediate neighbors of the node.
    */
            k = offset + j;
            sum = 0.0f;
            sum2 = 0.0f;
            closef = 1;
            if (ClosestFault[k] > Ncoarse) {
                closef = 0;
                istat1 = 0;
            }

        /*
         * Set the flags array to 1 if the node is blocked
         * by a fault or zero if not blocked.
         */
            memset (flags, 0, 9 * sizeof(char));
            kflag = 0;
            if (closef) {
                for (ki=i-Ncoarse; ki<=i+Ncoarse; ki+=Ncoarse) {
                    if (ki<0  ||  ki>=Nrow) {
                        kflag += 3;
                        continue;
                    }
                    off2 = ki*Ncol;
                    for (kj=j-Ncoarse; kj<=j+Ncoarse; kj+=Ncoarse) {
                        if (kj == j  &&  ki == i) {
                            kflag++;
                            continue;
                        }
                        istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, kj, ki, &fwgt);
                        if (istat1 != 0) flags[kflag] = 1;
                        kflag++;
                    }
                }
            }

        /*
         * If a node is blocked, then do not use its mirror about
         * the center either.  This eliminates phase shifts near
         * faults.
         */
            for (ki=0; ki<9; ki++) {
                if (ki == 4) continue;
                if (flags[8-ki] == 1) {
                    flags[ki] = 1;
                }
            }
            flags[4] = 0;  /* center point always used */

        /*
         * Only average the node values with flags set to zero.
         */
            kflag = 0;
            for (ki=i-Ncoarse; ki<=i+Ncoarse; ki+=Ncoarse) {
                if (ki<0  ||  ki>=Nrow) {
                    kflag += 3;
                    continue;
                }
                off2 = ki*Ncol;
                for (kj=j-Ncoarse; kj<=j+Ncoarse; kj+=Ncoarse) {
                    if (closef) {
                        if (flags[kflag]) {
                            kflag++;
                            continue;
                        }
                        kflag++;
                    }
                    if (kj>=0  &&  kj<Ncol) {
                        sum += Gwork1[off2+kj];
                        sum2 += 1.0f;
                    }
                }
            }

            if (sum2 > 0.0) {
                if (sum < TinySum  &&  sum > -TinySum) {
                    sum = 0.0;
                }
                Gwork2[k] = (Gwork1[k] * cwgt + sum / sum2) / (cwgt + 1.0f);
            }
            else {
                Gwork2[k] = Gwork1[k];
            }

        }

    }

    return 1;

}  /*  end of private SmoothFaultedErrors function  */






/*
  ****************************************************************

          S i m p l e S m o o t h F a u l t e d G r i d

  ****************************************************************

  Smooth the grid with a 3 by 3 moving average.  The smoothed
  value is combined with the original value depending upon the
  closest data point to the node and the smoothing factor.
  No strike weighted smoothing is done.

*/

int CSWGrdCalc::SimpleSmoothFaultedGrid (void)
{
    int       istat1, i, j, k, offset, off2, ki, kj, ismt;
    int       cp, istat, closef;
    CSW_F     fncc, sum, sum2, wt, avg, datafact, fwgt, cpmult;
    CSW_F     zgrid;
    char      flags[9];
    int       kflag;

    if (StepGridFlag  ||  Ndata < 4  ||  TrendSmoothFlag == 1) {
        return 1;
    }

    datafact = (CSW_F)Ndata / 2000.0f;
    if (datafact < 0.1f) datafact = 0.1f;
    if (SmoothingFactor > 0.0) {
        datafact *= SmoothingFactor;
    }
    if (datafact > 1.0f) {
        datafact = (CSW_F)sqrt ((double)datafact);
        datafact = (CSW_F)sqrt ((double)datafact);
    }

    if (FastGridFlag) {
        datafact = 10.0f;
    }

    ismt = (int)(SmoothingFactor * 2.0f + .5f);
    if (ismt < 1) ismt = 1;

    fncc = (CSW_F)Ncoarse + .5f;
    if (Ncoarse == 1) {
        datafact = (CSW_F)Ndata / 2000.0f;
        if (FastGridFlag) datafact = (CSW_F)Ndata / 5000.0f;
        if (DistancePower > 2) datafact /= 2.0f;
        if (DistancePower > 4) datafact /= 2.0f;
        if (datafact < 0.1f) datafact = 0.1f;
        if (SmoothingFactor > 0.0) {
            datafact *= SmoothingFactor;
        }
        if (datafact > 1.0f) {
            datafact = (CSW_F)sqrt ((double)datafact);
            datafact = (CSW_F)sqrt ((double)datafact);
        }
        if (datafact > 10.0f) datafact = 10.0f;
        fncc = 2.0f;
    }

/*
    Smooth and put the smoothed results into the Gwork1 array
*/
    for (i=0; i<Nrow; i+=Ncoarse) {

        offset = i * Ncol;

        for (j=0; j<Ncol; j+=Ncoarse) {

            k = offset + j;

        /*
            Sum the values of the immediate neighbors of the node.
        */
            sum = 0.0f;
            sum2 = 0.0f;
            closef = 1;
            if (ClosestFault[i*Ncol+j] > Ncoarse) {
                closef = 0;
                istat1 = 0;
            }

        /*
         * Set the flags array to 1 if the node is blocked
         * by a fault or zero if not blocked.
         */
            memset (flags, 0, 9 * sizeof(char));
            kflag = 0;
            if (closef) {
                for (ki=i-Ncoarse; ki<=i+Ncoarse; ki+=Ncoarse) {
                    if (ki<0  ||  ki>=Nrow) {
                        kflag += 3;
                        continue;
                    }
                    off2 = ki*Ncol;
                    for (kj=j-Ncoarse; kj<=j+Ncoarse; kj+=Ncoarse) {
                        if (kj == j  &&  ki == i) {
                            kflag++;
                            continue;
                        }
                        istat1 = grd_fault_ptr->grd_check_grid_fault_blocking (j, i, kj, ki, &fwgt);
                        if (istat1 != 0) flags[kflag] = 1;
                        kflag++;
                    }
                }
            }

        /*
         * If a node is blocked, then do not use its mirror about
         * the center either.  This eliminates phase shifts near
         * faults.
         */
            for (ki=0; ki<9; ki++) {
                if (ki == 4) continue;
                if (flags[8-ki] == 1) {
                    flags[ki] = 1;
                }
            }
            flags[4] = 0;  /* center point always used */

        /*
         * Only average the node values with flags set to zero.
         */
            kflag = 0;
            for (ki=i-Ncoarse; ki<=i+Ncoarse; ki+=Ncoarse) {
                if (ki<0  ||  ki>=Nrow) {
                    kflag += 3;
                    continue;
                }
                off2 = ki*Ncol;
                for (kj=j-Ncoarse; kj<=j+Ncoarse; kj+=Ncoarse) {
                    if (closef) {
                        if (flags[kflag]) {
                            kflag++;
                            continue;
                        }
                        kflag++;
                    }
                    if (kj>=0  &&  kj<Ncol) {
                        sum += Grid[off2+kj];
                        sum2 += 1.0f;
                    }
                }
            }

            if (sum2 > 0.0f) {
                if (sum < TinySum  &&  sum > -TinySum) {
                    sum = 0.0;
                }
                avg = sum / sum2;
            }
            else {
                avg = Grid[offset+j];
            }

        /*
            Combine the smoothed and original value according to
            the distance from the closest data point at the node.
        */
            cp = ClosestPoint[offset+j];
            if (cp <= Ncoarse) {
                istat = CheckClosePointFaulting (i, j, Ncoarse);
                if (istat == -1  &&  cp >= -3) {
                    cp = Ncoarse + 1;
                }
            }
            zgrid = Grid[k];
            if (cp < -3) {
                cp = -cp;
                cp -= 4;
                zgrid = Zdata[cp];
                cp = 0;
                cpmult = .01f;
            }
            else {
                cpmult = 1.0f;
                if (cp == -3) {
                    cpmult = .01f;
                }
                else if (cp == -2) {
                    cpmult = .05f;
                }
                else if (cp == -1) {
                    cpmult = .25f;
                }
            }
            wt = (CSW_F)(cp + ismt) / fncc;
            wt *= datafact;
            wt *= wt;
            wt *= cpmult;
            Gwork1[k] = (zgrid + avg * wt) / (1.0f + wt);

        }

    }

/*
    Copy the smoothed results back to the Grid array.
*/
    for (i=0; i<Nrow; i+=Ncoarse) {
        offset = i * Ncol;
        for (j=0; j<Ncol; j+=Ncoarse) {
            Grid[offset+j] = Gwork1[offset+j];
        }
    }

    return 1;

}  /*  end of private SimpleSmoothFaultedGrid function  */





/*
  ****************************************************************

           C a l c F a u l t e d E r r o r A t N o d e

  ****************************************************************

    This is called when errors need to be calculated near a fault.
  The points that are on the same side of the faults as the node
  are analysed.  The grid estimate at the points is made by doing
  an inverse distance squared average of the nodes (at the current
  grid coarseness) that are not blocked off from a given point.

    The difference between the original value and the estimated
  value at each point is then averaged at the node.

*/

int CSWGrdCalc::CalcFaultedErrorAtNode
         (int irow, int jcol, int *list, int nlist,
          CSW_F *error)
{
    int         i, n, istat;
    CSW_F       x0, y0, *x = ploc_f1, *y = ploc_f2,
                locerror;
    CSW_F       fwgt;
    CSW_F       *z = p_zp, *z2 = p_yp;

/*
    put points into local arrays if they are not blocked off
*/
    n = 0;
    for (i=0; i<nlist; i++) {
        istat = grd_fault_ptr->grd_check_fault_blocking (irow, jcol,
                                          Xdata[list[i]], Ydata[list[i]], &fwgt);
        if (istat != 0) continue;
        x[n] = Xdata[list[i]];
        y[n] = Ydata[list[i]];
        z[n] = Zdata[list[i]];
        n++;
    }

    if (n < 1) {
        *error = 0.0f;
        return 1;
    }

/*
    Calculate faulted elevation at each point using only nodes on
    the same side of a fault as the points.
*/
    grd_fault_ptr->grd_interp_in_faulted_area (x, y, z2, irow, jcol, n, Grid, Ncoarse);

/*
    reorigin points at the grid node and
    calculate the errors in elevation
*/
    x0 = jcol * Xspace + Xmin;
    y0 = irow * Yspace + Ymin;
    for (i=0; i<n; i++) {
        x[i] -= x0;
        y[i] -= y0;
        if (z2[i] > 1.e20f  ||  z2[i] < -1.e20f) {
            z2[i] = z[i];
        }
        if (ThicknessFlag == 1) {
            if (z[i] < Ztiny  &&  z2[i] < Ztiny) {
                z2[i] = z[i];
            }
            else if (z[i] < Ztiny) {
                z2[i] *= 2.0f;
            }
        }
        z[i] -= z2[i];

    /*
     * If faults are quite close to each other relative to the
     * grid cell size, it is possible to get large errors from
     * communication across a fault.  To mitigate this problem,
     * I do not allow large error corrections for any point.
     */
        if (z[i] > FaultedErrorMax  ||  z[i] < -FaultedErrorMax) {
            z[i] = 0.0;
        }
    }

/*
    Do inverse distance squared average of errors at points.
*/
    istat = grd_utils_ptr->grd_inverse_distance_average
                   (x, y, z, n, DistancePower,
                    Xspace + Yspace, (CSW_F *)NULL, &locerror);

    *error = locerror;

    return istat;

}  /*  end of private CalcFaultedErrorAtNode function  */







/*
  ****************************************************************

            F a u l t e d D a t a P o i n t E r r o r s

  ****************************************************************

    Calculate the errors between data points and the estimated
  elevation at the point for the current coarse grid.  The max
  error and average error are returned.

*/

int CSWGrdCalc::FaultedDataPointErrors
         (CSW_F *maxerrptr, CSW_F *avgerrptr)
{
    int            irow, jcol, k, n;
    CSW_F          maxerr, avgerr, zerr;

    maxerr = 0.0f;
    avgerr = 0.0f;
    n = 0;

/*
    loop through all data points.
*/
    for (k=0; k<Ndata; k++) {

    /*
        ignore if outside of the grid
    */
        if (Ydata[k] < Ymin  ||  Ydata[k] > Ymax  ||
            Xdata[k] < Xmin  ||  Xdata[k] > Xmax) {
            Zerr[k] = 1.e30f;
            continue;
        }

    /*
        ignore if "exactly" on a fault line
    */
        if (Zdata[k] >= OnFault) {
            Zerr[k] = OnFault * 10.0f;
            continue;
        }

    /*
        Use only nodes that are on the same fault block as the
        points to determine errors.
    */
        jcol = (int)((Xdata[k] - Xmin) / (Xspace*Ncoarse));
        irow = (int)((Ydata[k] - Ymin) / (Yspace*Ncoarse));
        if (grd_fault_ptr->grd_fault_check_needed (irow, jcol, Ncoarse)) {
            grd_fault_ptr->grd_interp_in_faulted_area (Xdata+k, Ydata+k, Zerr+k,
                                        irow, jcol, 1, Grid, Ncoarse);
        }

    /*
        do bilinear interpolation if no fault is nearby
    */
        else {
            grd_utils_ptr->grd_bilin_interp (Xdata+k, Ydata+k, Zerr+k, 1,
                              Grid, Ncol, Nrow, Ncoarse,
                              Xmin, Ymin, Xmax, Ymax);
        }

        if (Zerr[k] > 1.e20f  ||  Zerr[k] < -1.e20f)
            Zerr[k] = 0.0f;
        else
            Zerr[k] -= Zdata[k];

        if (ThicknessFlag == 1  &&  Zdata[k] < Ztiny) {
            Zerr[k] = 0.0f;
        }
        zerr = Zerr[k];
        if (zerr < 0.0f) zerr = -zerr;
        if (zerr > maxerr) {
            maxerr = zerr;
        }

        avgerr += zerr;
        n++;

    }

/*
    return the average and max.
*/
    *avgerrptr = avgerr / (CSW_F)n;
    *maxerrptr = maxerr;

    return 1;

}  /*  end of private DataPointErrors function  */






/*
  ****************************************************************

                R e f i n e F a u l t e d G r i d

  ****************************************************************

    Interpolate the current coarse grid at half the coarse spacing
  to produce the next coarse grid.  Bicubic or bilinear interpolation
  is used as appropriate.

    If the cell being refined is close to a fault, then use special
  techniques to refine it.

*/

int CSWGrdCalc::RefineFaultedGrid (void)
{
    int      istat, i, j, k, n, offset, nrlast, nc2, off2;
    CSW_F    x0, y0, xsp, ysp, xsp2, ysp2;
    int      bcflag;

    CSW_F    x[10], y[10], z[10];

/*
    set some counting constants
*/
    xsp = Xspace * Ncoarse;
    ysp = Yspace * Ncoarse;
    xsp2 = xsp / 2.0f;
    ysp2 = ysp / 2.0f;
    nrlast = Nrow - Ncoarse - 1;
    nc2 = Ncoarse / 2;
    off2 = nc2 * Ncol;

    bcflag = 0;
    if (TriangulateFlag < 1  ||  TriangulateFlag > 3) {
        bcflag = 1;
    }

/*
    loop through columns and rows of coarse grid.
*/
    for (i=0; i<Nrow-1; i+=Ncoarse) {

        offset = Ncol * i;
        y0 = i * Yspace + Ymin;

        for (j=0; j<Ncol-1; j+=Ncoarse) {

        /*
            Set the x and y arrays with the point locations
            of the refined grid, at half the current coarse spacing.
        */
            x0 = j * Xspace + Xmin;
            k = offset + j;
            n = 0;

        /*
            left side only for first column
        */
            if (j == 0) {
                x[0] = Xmin;
                y[0] = y0 + ysp2;
                n = 1;
            }

        /*
            center, bottom side and right side for all cells.
        */
            x[n] = x0 + xsp2;
            y[n] = y0;
            n++;
            x[n] = x0 + xsp2;
            y[n] = y0 + ysp2;
            n++;
            x[n] = x0 + xsp;
            y[n] = y0 + ysp2;
            n++;

        /*
            top side only for top row.
        */
            if (i == nrlast) {
                x[n] = x0 + xsp2;
                y[n] = y0 + ysp;
                n++;
            }

        /*
            If this cell is close to a fault, account for fault
            blocking in the refinement.  The i,j row and column
            are the lower left of the cell being refined.  A center
            point at i+nc2, j+nc2 with a half width of nc2 * 3 will
            encompass the entire area needed for bicubic interpolation.
            If this area is csw_Free of fault vectors, normal non faulted
            interpolation can be used.
        */
            istat = grd_fault_ptr->grd_fault_check_needed (i+nc2, j+nc2, nc2*3);
            if (istat == 1) {
                grd_fault_ptr->grd_interp_in_faulted_area (x, y, z, i, j,
                                            n, Grid, Ncoarse);
            }

        /*
            otherwise do bicubic or bilinear interpolation as needed
        */
            else {
                if (CheckForBicub (i, j)  &&  bcflag) {
                    istat = grd_utils_ptr->grd_bicub_interp (x, y, z, n, 0.0f,
                                      Grid, Ncol, Nrow, Ncoarse,
                                      Xmin, Ymin, Xmax, Ymax,
                                      -1, -1);
                }
                else {
                    istat = grd_utils_ptr->grd_bilin_interp (x, y, z, n,
                                      Grid, Ncol, Nrow, Ncoarse,
                                      Xmin, Ymin, Xmax, Ymax);
                }
                if (istat == -1) {
                    grd_utils_ptr->grd_set_err(1);
                    return -1;
                }
            }

        /*
            stuff the z values into the grid at the proper locations
            starting with the left side if it is the first column.
        */
            n = 0;
            if (j == 0) {
                *(Grid+k+off2) = z[n];
                n++;
            }

        /*
            assign bottom side, center and right side
        */
            *(Grid+k+nc2) = z[n];
            n++;

            *(Grid+k+off2+nc2) = z[n];
            n++;

            *(Grid+k+off2+Ncoarse) = z[n];
            n++;

        /*
            assign top side if it is the last row
        */
            if (i == nrlast) {
                *(Grid+k+Ncoarse*Ncol+nc2) = z[n];
            }

        }  /*  end of loop through columns  */

    }  /*  end of loop through rows  */

    return 1;

}  /*  end of private RefineFaultedGrid function  */





/*
  ****************************************************************

          C h e c k C l o s e P o i n t F a u l t i n g

  ****************************************************************

  Check the points withing Ncoarse + 1 of the specified node
  to see if all of the "close" nodes are faulted out.  This is
  used when smoothing a faulted grid.  If the "close" points are
  all faulted out, the smoothed values should be weighted highly
  in combining smooth and original data.

  If all points are faulted, -1 is returned.  If at least one
  point is not faulted, the "level" of the unfaulted point is
  returned.

*/

int CSWGrdCalc::CheckClosePointFaulting
         (int irow, int jcol, int nc)
{
    int       start, k, list[500], i, nlist, nquad, istat;
    int       index, search_save, d1, d2;
    CSW_F     x1, y1, weight, x0, y0;

/*
    Find the closest points regardless of faulting.
*/
    k = irow * Ncol + jcol;
    start = ClosestPoint[k];
    if (start < 0) start = 0;

    search_save = LocalSearchPattern;
    LocalSearchPattern = GRD_RADIAL_SEARCH;
    CollectLocalPoints (irow, jcol, start, nc+1,
                        4, 100,
                        list, &nlist, &nquad);
    LocalSearchPattern = search_save;

    if (nlist < 1) {
        return -1;
    }

/*
    if a point is not blocked by faulting,
    return it's distance "level"
*/
    for (i=0; i<nlist; i++) {
        index = list[i];
        index %= 100000000;
        x1 = Xdata[index];
        y1 = Ydata[index];
        istat = grd_fault_ptr->grd_check_fault_blocking (irow, jcol, x1, y1, &weight);
        if (istat == 0) {
            x0 = Xspace * jcol + Xmin;
            y0 = Yspace * irow + Ymin;
            d1 = (int)((x1 - x0) / Xspace);
            if (d1 < 0)
                d1 = -d1;
            d2 = (int)((y1 - y0) / Yspace);
            if (d2 < 0)
                d2 = -d2;
            if (d1 > d2)
                return d1;
            return d2;
        }
    }

    return -1;

}  /*  end of private CheckClosePointFaulting function  */







/*
  ****************************************************************

                  S m o o t h S t e p G r i d

  ****************************************************************

    Eliminate single node spikes in the step grid unless the
  smoothing factor is zero.

*/

int CSWGrdCalc::SmoothStepGrid (void)
{
    int       i, j, offset, off2, ki, kj, n, k, ivals[8];
    int       n2, k2, match, center, cp;
    CSW_F     vals2[8], vals[8], z1;

    if (FaultedFlag) {
        return 1;
    }

    if (SmoothingFactor == 0.0f) {
        return 1;
    }

    memset (vals, 0, 8 * sizeof(CSW_F));

/*
    Remove spikes and put results into Gwork1
*/
    for (i=1; i<Nrow-1; i++) {

        offset = i * Ncol;

        for (j=1; j<Ncol-1; j++) {

            center = offset + j;

            cp = ClosestPoint[center];
            if (cp < 0) cp = 0;

            if (cp < 2) {
                Gwork1[center] = Grid[center];
                continue;
            }

        /*
            Put the nearest neighbors into the vals work array.
        */
            n = 0;
            for (ki=i-1; ki<=i+1; ki++) {
                off2 = ki*Ncol;
                for (kj=j-1; kj<=j+1; kj++) {
                    if (off2+kj == center)
                        continue;
                    vals[n] = Grid[off2+kj];
                    n++;
                }
            }

        /*
            Find how many different values are neighbors.
        */
            memset ((char *)ivals, 0, 8 * sizeof(int));
            memset ((char *)vals2, 0, 8 * sizeof(int));

            vals2[0] = vals[0];
            n2 = 1;
            for (k=1; k<n; k++) {
                match = 0;
                for (k2=0; k2<n2; k2++) {
                    if (StepGraze(vals2[k2], vals[k])) {
                        match = 1;
                        break;
                    }
                }
                if (match == 0) {
                    vals2[n2] = vals[k];
                    n2++;
                }
            }

        /*
            If there are other than 2 values, do not check
            for a spike.
        */
            if (n2 != 2) {
                Gwork1[center] = Grid[center];
                continue;
            }

        /*
            Count the number of neighbors with each value.
        */
            for (k=0; k<2; k++) {
                z1 = vals2[k];
                for (k2=0; k2<n; k2++) {
                    if (StepGraze(vals[k2], z1)) {
                        ivals[k]++;
                    }
                }
            }

        /*
            If the center point has only one neighbor with its
            value, use the predominant value in the neighbors.
        */
            if (ivals[0] == n-1) {
                if (!StepGraze (Grid[center], vals2[0])) {
                    Gwork1[center] = vals2[0];
                }
                else {
                    Gwork1[center] = Grid[center];
                }
            }
            else if (ivals[1] == n-1) {
                if (!StepGraze (Grid[center], vals2[1])) {
                    Gwork1[center] = vals2[1];
                }
                else {
                    Gwork1[center] = Grid[center];
                }
            }
            else {
                Gwork1[center] = Grid[center];
            }

        }

    }

/*
    Copy the smoothed results back to the Grid array.
*/
    for (i=1; i<Nrow-1; i++) {
        offset = i * Ncol;
        for (j=1; j<Ncol-1; j++) {
            Grid[offset+j] = Gwork1[offset+j];
        }
    }

    return 1;

}  /*  end of private SmoothStepGrid function  */





/*
  ****************************************************************

                  A d j u s t F o r Z e r o s

  ****************************************************************

    For a thickness grid, if all quadrants with data have a zero
  or negative value as the closest point in the quadrant, or if
  quadrants with positive values are much more distant than
  those with zero or negative values, then the grid node is set
  to ZminusVal.  If this is not the case, the function returns zero.

*/

int CSWGrdCalc::AdjustForZeros
        (CSW_F *x, CSW_F *y, CSW_F *z, int *nlist)
{
    int                 i, j, n, nzero;
    CSW_F               dquad[4], valquad[4];
    CSW_F               xt, yt, dt, d1, d2, dmult, dcrit;

    n = *nlist;

/*
    Initialize quadrant distances and values to absurd numbers.
*/
    for (i=0; i<4; i++) {
        dquad[i] = 1.e30f;
        valquad[i] = 1.e30f;
    }

/*
    find the closest point in each quadrant
*/
    for (i=0; i<n; i++) {

        xt = x[i];
        yt = y[i];
        dt = xt * xt + yt * yt;
        if (xt <= 0.0) {
            if (yt >= 0.0) {
                if (dt < dquad[1]) {
                    dquad[1] = dt;
                    valquad[1] = z[i];
                }
            }
            else {
                if (dt < dquad[2]) {
                    dquad[2] = dt;
                    valquad[2] = z[i];
                }
            }
        }
        else {
            if (yt >= 0.0) {
                if (dt < dquad[0]) {
                    dquad[0] = dt;
                    valquad[0] = z[i];
                }
            }
            else {
                if (dt < dquad[3]) {
                    dquad[3] = dt;
                    valquad[3] = z[i];
                }
            }
        }
    }

/*
    Check the closest point in each quadrant.
*/
    nzero = 0;
    j = 0;

    for (i=0; i<4; i++) {
        if (valquad[i] <= 0.0) {
            nzero++;
        }
        if (valquad[i] < 1.e20f) {
            j++;
        }
    }

    if (j == nzero) {
        return 1;
    }

/*
    check the distance ratio between zero and
    non zero quadrants.
*/
    dmult = 2.0f;
    dcrit = (Xspace + Yspace) * Ncoarse;
    dcrit *= dcrit;
    d1 = 1.e30f;
    d2 = 1.e30f;

    for (i=0; i<4; i++) {
        if (valquad[i] <= 0.0) {
            if (dquad[i] < d1) {
                d1 = dquad[i];
            }
        }
        else {
            if (dquad[i] < d2) {
                d2 = dquad[i];
            }
        }
    }

    if (d2 > dcrit  &&  d2 > dmult * d1) {
        return 1;
    }

    return 0;

}  /*  end of private AdjustForZeros function  */




/*
  ****************************************************************

            C h e c k F o r Z e r o T h i c k n e s s

  ****************************************************************

    Check the input data for zero or negative values.  If none
  are found, reset the Thickness flag to zero.

*/

int CSWGrdCalc::CheckForZeroThickness (void)
{
    int               i;

    for (i=0; i<Ndata; i++) {
        if (Zdata[i] <= 0.0f) {
            if (Xdata[i] >= Xmin  &&  Xdata[i] <= Xmax  &&
                Ydata[i] >= Ymin  &&  Ydata[i] <= Ymax) {
                return 1;
            }
        }
    }

    ThicknessFlag = 0;

    return 1;

}  /*  end of private CheckForZeroThickness function  */





/*
  ****************************************************************************

                 A d j u s t F o r C o n t r o l P o i n t s

  ****************************************************************************

    If there are any control points, make sure the grid honors them.  This is
  called as part of the iterative process of smoothing and fitting done during
  the second pass of the grid calculation.

*/

int CSWGrdCalc::AdjustForControlPoints (void)
{
    int            i, type;
    CSW_F          z1, z2, zt, zerr, z20, xt, yt;
    CSW_F          ztiny;

    if (NumControlPoints < 1  ||  ControlPoints == NULL) {
        return 1;
    }

    z20 = Zrange / 40.0f;

    for (i=0; i<NumControlPoints; i++) {

        xt = ControlPoints[i].x;
        yt = ControlPoints[i].y;
        grd_utils_ptr->grd_bilin_interp (&xt, &yt, &zt, 1,
                          Grid, Ncol, Nrow, Ncoarse,
                          Xmin, Ymin, Xmax, Ymax);

        type = ControlPoints[i].type;
        if (type == GRD_CONTROL_GREATER) {
            if (ControlPoints[i].active_flag == 0) {
                z1 = ControlPoints[i].z1 + z20;
                if (zt >= z1) {
                    continue;
                }
                ControlPoints[i].zval = z1;
                ControlPoints[i].active_flag = 1;
            }
            else {
                z1 = ControlPoints[i].zval;
            }
            if (zt >= z1) continue;
            zerr = z1 - zt;
            AdjustCornersForControl (xt, yt, zerr);
        }
        else if (type == GRD_CONTROL_LESS) {
            if (ControlPoints[i].active_flag == 0) {
                z1 = ControlPoints[i].z1 - z20;
                if (zt <= z1) {
                    continue;
                }
                ControlPoints[i].zval = z1;
                ControlPoints[i].active_flag = 1;
            }
            else {
                z1 = ControlPoints[i].zval;
            }
            if (zt <= z1) continue;
            zerr = z1 - zt;
            AdjustCornersForControl (xt, yt, zerr);
        }
        else {
            z1 = ControlPoints[i].z1;
            z2 = ControlPoints[i].z2;
            if (z1 >= z2) {
                continue;
            }
            ztiny = (z2 - z1) / 10.0f;
            z1 += ztiny;
            z2 -= ztiny;
            if (zt >= z1  &&  zt <= z2) {
                continue;
            }
            if (zt < z1) {
                zerr = z1 - zt;
            }
            else {
                zerr = z2 - zt;
            }
            AdjustCornersForControl (xt, yt, zerr);
        }

    }

    return 1;

}  /*  end of private AdjustForControlPoints function  */





/*
  ****************************************************************************

               A d j u s t C o r n e r s F o r C o n t r o l

  ****************************************************************************

    Adjust the corners of the coarse grid cell containing the control point
  so that when smoothed, the cell will fit the control point much better.

*/

int CSWGrdCalc::AdjustCornersForControl (CSW_F x, CSW_F y, CSW_F zerr)
{
    int             i1, i2, j1, j2, k, cp;
    int             mult;

    j1 = (int)((x - Xmin) / Xspace);
    i1 = (int)((y - Ymin) / Yspace);
    x -= Xmin;
    y -= Ymin;

    i1 /= Ncoarse;
    i1 *= Ncoarse;
    j1 /= Ncoarse;
    j1 *= Ncoarse;

    i2 = i1 + Ncoarse;
    j2 = j1 + Ncoarse;

    k = i1 * Ncol + j1;
    cp = ClosestPoint[k];
    mult = cp / NcoarseOrig / 4;
    if (mult < 1) mult = 1;
    if (mult > 2) mult = 2;
    Gwork1[k] = zerr * mult * mult;

    k = i1 * Ncol + j2;
    cp = ClosestPoint[k];
    mult = cp / NcoarseOrig / 4;
    if (mult < 1) mult = 1;
    if (mult > 2) mult = 2;
    Gwork1[k] = zerr * mult * mult;

    k = i2 * Ncol + j1;
    cp = ClosestPoint[k];
    mult = cp / NcoarseOrig / 4;
    if (mult < 1) mult = 1;
    if (mult > 2) mult = 2;
    Gwork1[k] = zerr * mult * mult;

    k = i2 * Ncol + j2;
    cp = ClosestPoint[k];
    mult = cp / NcoarseOrig / 4;
    if (mult < 1) mult = 1;
    if (mult > 2) mult = 2;
    Gwork1[k] = zerr * mult * mult;

    NumControlAdjust++;

    return 1;

}  /*  end of private AdjustCornersForControl function  */





/*
  ****************************************************************************

                       C h e c k F l a t C o n t r o l

  ****************************************************************************

*/

int CSWGrdCalc::CheckFlatControl (CSW_F *x, CSW_F *y, CSW_F *z, int npts)
{
    int              i, n, size;
    CSW_F            z0;

    z0 = z[0];

    if (npts < MAX_CONTROL) {
        size = npts * sizeof(CSW_F);
        memcpy (Xcontrol, x, size);
        memcpy (Ycontrol, y, size);
        memcpy (Zcontrol, z, size);
    }
    n = npts;

    for (i=0; i<NumControlPoints; i++) {
        if (ControlPoints[i].type == GRD_CONTROL_GREATER) {
            if (ControlPoints[i].z1 > z0) {
                if (n < MAX_CONTROL) {
                    Xcontrol[n] = ControlPoints[i].x;
                    Ycontrol[n] = ControlPoints[i].y;
                    Zcontrol[n] = ControlPoints[i].z1;
                }
                n++;
            }
        }
        else if (ControlPoints[i].type == GRD_CONTROL_LESS) {
            if (ControlPoints[i].z1 < z0) {
                if (n < MAX_CONTROL) {
                    Xcontrol[n] = ControlPoints[i].x;
                    Ycontrol[n] = ControlPoints[i].y;
                    Zcontrol[n] = ControlPoints[i].z1;
                }
                n++;
            }
        }
    }

    NumFlatControl = n;

    if (n == npts) {
        return 1;
    }

    return 0;

}  /*  end of private CheckFlatControl function  */






/*
  ****************************************************************************

                       B u i l d S h a p e G r i d

  ****************************************************************************

  Resample the ConformalGrid data into the same geometry as the coarse
  grid currently being calculated.  Then, get estimates of the shape grid
  values at the direct input x,y locations.  The direct z values are cross
  plotted against the values interpolated from the shape grid.  This gives
  a function to convert shape grid values to direct data.  The shape grid
  is converted and then the function returns.  The UseShapeGrid flag is set
  to 1 upon success.

*/

int CSWGrdCalc::BuildShapeGrid (void)
{
    int             istat;

    UseShapeGrid = 0;
    if (ConformalFlag == 0) {
        return 1;
    }

    istat = grd_arith_ptr->grd_resample_grid (ConformalGrid.grid,
                               NULL,
                               ConformalGrid.ncol,
                               ConformalGrid.nrow,
                               ConformalGrid.x1,
                               ConformalGrid.y1,
                               ConformalGrid.x2,
                               ConformalGrid.y2,
                               NULL, 0,
                               ShapeGrid,
                               NULL,
                               Ncol,
                               Nrow,
                               Xmin,
                               Ymin,
                               Xmax,
                               Ymax,
                               GRD_BILINEAR);

    if (istat == -1) {
        return 1;
    }

    UseShapeGrid = 1;

    return 1;

}  /*  end of private BuildShapeGrid function  */





/*
  ****************************************************************************

                 E x t e n d U s i n g S h a p e G r i d

  ****************************************************************************

    Fill in the as yet unassigned coarse grid nodes by projecting the values
  of the shape grid.  The average difference between non null neighbor nodes
  and the shape grid is used to set the null node value.

*/

int CSWGrdCalc::ExtendUsingShapeGrid (void)
{
    int         i, j, k, ii, jj, kk, ic, kc, nok, ido, ndo,
                i1, i2, j1, j2, ndone, n, offset, offsetc;
    CSW_F       sum, sum2, zt, avg, gwgt;

/*
    Find the average difference for all nodes calculated
    between the grid being calculated and the shape grid
*/
    sum = 0.0f;
    sum2 = 0.0f;
    for (i=0; i<Nrow; i++) {

        offset = i * Ncol;

        for (j=0; j<Ncol; j++) {

            k = offset + j;
            if (Grid[k] < 1.e19f) {
                zt = Grid[k] - ShapeGrid[k];
                sum += zt;
                sum2 += 1.0f;
            }
        }
    }

    if (sum2 > 0.5f) {
        if (sum < TinySum  &&  sum > -TinySum) {
            sum = 0.0;
        }
        avg = sum / sum2;
    }
    else {
        avg = 0.0f;
    }

    nok = 1;
/*
    if (sum2 > 20.0f) nok = 2;
    if (sum2 > 50.0f) nok = 3;
*/

/*
    Use local differences combined with the global average to
    fill in the rest of the coarse grid.
*/
    gwgt = 0.0f;
    for (;;) {

        ndone = 0;

        for (i=0; i<Nrow; i+=Ncoarse) {

            i1 = i - Ncoarse;
            i2 = i + Ncoarse;
            if (i1 < 0) i1 = 0;
            if (i2 > Nrow-1) i2 = Nrow - 1;

            for (j=0; j<Ncol; j+=Ncoarse) {

                k = i * Ncol + j;
                if (Grid[k] < 1.e19f) {
                    Gwork1[k] = Grid[k];
                    continue;
                }

                gwgt = (CSW_F)ClosestPoint[k];
                gwgt /= (CSW_F)Ncoarse;
                gwgt = (CSW_F)sqrt (gwgt);

                gwgt = 0.0;

                j1 = j - Ncoarse;
                j2 = j + Ncoarse;
                if (j1 < 0) j1 = 0;
                if (j2 > Ncol-1) j2 = Ncol - 1;

                n = 0;
                sum = 0.0f;
                sum2 = 0.0f;

                for (ii=i1; ii<=i2; ii+=Ncoarse) {

                    offset = ii * Ncol;

                    for (jj=j1; jj<=j2; jj+=Ncoarse) {

                        kk = offset + jj;
                        if (Grid[kk] < 1.e19f) {
                            zt = Grid[kk] - ShapeGrid[kk];
                            sum += zt;
                            sum2 += 1.0f;
                            n++;
                        }
                    }
                }

                if (n >= nok) {
                    zt = 0.0f;
                    if (sum2 > 0.5f) {
                        if (sum < TinySum  &&  sum > -TinySum) {
                            sum = 0.0;
                        }
                        zt = sum / sum2;
                    }
                    zt = (zt + avg * gwgt) / (1.0f + gwgt);
                    Gwork1[k] = zt + ShapeGrid[k];
                    ndone++;
                }
                else {
                    Gwork1[k] = 1.e30f;
                }

            }
        }

        gwgt += 0.1f;

    /*
        Copy the work grid back into Grid for the next pass.
    */
        memcpy (Grid, Gwork1, Ncol*Nrow*sizeof(CSW_F));

    /*
        When no node has been processed the extension has finished.
    */
        if (ndone == 0) {
            break;
        }

    }

/*
    The coarse grid is now extended to the limits of
    the grid geometry.  Calculate a difference grid
    between the coarse Grid and the ShapeGrid.  The
    difference grid will be used during the iterative
    grid refinement process to keep the calculated
    surface semi conformable with the shape surface
    at each refinement step.
*/
    for (i=0; i<Nrow; i+=Ncoarse) {
        offset = i * Ncol;
        ic = i / Ncoarse;
        offsetc = ic * Nccol;
        for (j=0; j<Ncol; j+=Ncoarse) {
            k = offset + j;
            kc = offsetc + j / Ncoarse;
            DifferenceGrid[kc] = ShapeGrid[k] - Grid[k];
        }
    }

/*
    Smooth the difference grid.
*/
    ndo = 3;
    if (Ndata > 40) ndo = 4;

    for (ido=0; ido<ndo; ido++) {

        for (i=0; i<Ncrow; i++) {
            i1 = i - 1;
            i2 = i + 1;
            if (i1 < 0) i1 = 0;
            if (i2 > Ncrow-1) i2 = Ncrow - 1;
            offset = i * Nccol;
            for (j=0; j<Nccol; j++) {
                j1 = j - 1;
                j2 = j + 1;
                if (j1 < 0) j1 = 0;
                if (j2 > Nccol-1) j2 = Nccol - 1;
                sum = 0.0f;
                sum2 = 0.0f;
                for (ii=i1; ii<=i2; ii++) {
                    for (jj=j1; jj<=j2; jj++) {
                        sum += DifferenceGrid[ii*Nccol+jj];
                        sum2++;
                    }
                }
                if (sum < TinySum  &&  sum > -TinySum) {
                    sum = 0.0;
                }
                Gwork1[offset+j] = sum / sum2;
            }
        }

        memcpy (DifferenceGrid, Gwork1, Nccol*Ncrow*sizeof(CSW_F));

    }

    return 1;

}  /*  end of private ExtendUsingShapeGrid function  */






/*
  ****************************************************************************

                   A d j u s t F o r S h a p e G r i d

  ****************************************************************************

    Combine the difference grid, the shape grid and the current coarse
    calculated grid together.  Nodes further from data will be influenced
    more by the shape grid and difference grid.

*/

int CSWGrdCalc::AdjustForShapeGrid (void)
{
    int         i, j, k, ic, jc, kc, cp, offset, offsetc, nc2;
    CSW_F       wgt, zt1, zt2, z1, z2, z3, z4,
                diff, zshape, xt, yt, w, h;

    w = (CSW_F)NcoarseOrig;
    h = w;

    nc2 = Ncoarse * 100;
    if (nc2 > NcoarseOrig) nc2 = NcoarseOrig;

    for (i=0; i<Nrow; i+=Ncoarse) {
        offset = i * Ncol;
        ic = i / NcoarseOrig;
        if (ic > Ncrow-2) ic = Ncrow-2;
        offsetc = ic * Nccol;
        yt = (CSW_F)(i - ic*NcoarseOrig);
        yt /= h;

        for (j=0; j<Ncol; j+=Ncoarse) {
            k = offset + j;
            jc = j / NcoarseOrig;
            if (jc > Nccol-2) jc = Nccol-2;
            kc = offsetc + jc;
            xt = (CSW_F)(j - jc * NcoarseOrig);
            xt /= w;

            z1 = DifferenceGrid[kc];
            z2 = DifferenceGrid[kc+1];
            z3 = DifferenceGrid[kc+Nccol];
            z4 = DifferenceGrid[kc+Nccol+1];
            zt1 = (z2 - z1) * xt + z1;
            zt2 = (z4 - z3) * xt + z3;
            diff = (zt2 - zt1) * yt + zt1;
            zshape = ShapeGrid[k] - diff;

            cp = ClosestPoint[k];
            cp--;
            if (cp < 0) cp = 0;
            wgt = (CSW_F)cp / (CSW_F)nc2;
            wgt *= wgt;
            Grid[k] = (Grid[k] + wgt * zshape) / (1.0f + wgt);
        }
    }

    return 1;

}  /*  end of private AdjustForShapeGrid function  */





/*
  ****************************************************************************

                      M e d i a n F i l t e r

  ****************************************************************************

*/

#define _CUTOFF_ 2

int CSWGrdCalc::MedianFilter (void)
{
    int            istat, i, k, ntot, i1, i2, j1, j2,
                   ii, jj, n1, n2, offset, cp;
    CSW_F          *smgrid, pt, nt, ratio, zt;

/*
    Smooth the grid a great deal and put the results into
    Gwork1.
*/
    istat = grd_smooth_grid (Grid, Ncol, Nrow, 9,
                             NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f,
                             1.e30f, 1.e30f, &smgrid);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = grd_smooth_grid (smgrid, Ncol, Nrow, 9,
                             NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f,
                             1.e30f, 1.e30f, NULL);
    if (istat == -1) {
        csw_Free (smgrid);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    istat = grd_smooth_grid (smgrid, Ncol, Nrow, 9,
                             NULL, 0, 0.0f, 0.0f, 0.0f, 0.0f,
                             1.e30f, 1.e30f, NULL);
    if (istat == -1) {
        csw_Free (smgrid);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    ntot = Ncol * Nrow;
    memcpy (Gwork1, smgrid, ntot * sizeof(CSW_F));
    csw_Free (smgrid);
    smgrid = NULL;

/*
    Put the residual between the original Grid and the smoothed
    grid into Gwork2.
*/
    for (i=0; i<ntot; i++) {
        Gwork2[i] = Grid[i] - Gwork1[i];
    }

/*
    If a node is more than 2 spacings away from data, set its
    residual to zero.  Reduce residuals appropriately for 2 and
    1 spacing away.
*/
    for (i=0; i<ntot; i++) {
        cp = ClosestPoint[i];
        if (cp > _CUTOFF_) {
            Gwork2[i] = 0.0f;
        }
        else if (cp > 1) {
            Gwork2[i] *= 0.1f;
        }
        else if (cp > 0) {
            Gwork2[i] *= 0.5f;
        }
    }

/*
    Find the average positive and negative residuals.
*/
    nt = 0.0f;
    pt = 0.0f;
    n1 = 0;
    n2 = 0;

    for (i=0; i<ntot; i++) {
        cp = ClosestPoint[i];
        if (cp > _CUTOFF_) continue;
        zt = Gwork2[i];
        if (zt < 0.0f) {
            n1++;
            nt += zt;
        }
        else {
            n2++;
            pt += zt;
        }
    }

    if (n1 > 0) {
        nt /= (CSW_F)n1;
        nt = -nt;
    }
    if (n2 > 0) {
        pt /= (CSW_F)n2;
    }
    if (n1 > 0) {
        ratio = pt / nt;
        ratio *= ratio;
    }
    else {
        ratio = 1000.0f;
    }

/*
    If a residual value is much smaller than its average of the same
    sign, set it to zero because it is essentially uninterpretable noise.
*/
    for (i=0; i<ntot; i++) {
        cp = ClosestPoint[i];
        if (cp > _CUTOFF_) continue;
        zt = Gwork2[i];
        if (zt < 0.0f) {
            zt = -zt;
            if (zt < nt / 10.0f) {
                if (cp > 0) {
                    zt = 0.0f;
                }
            }
            else {
                zt = -zt;
            }
        }
        else {
            if (zt < pt / 10.0f) {
                if (cp > 0) {
                    zt = 0.0f;
                }
            }
        }
        Gwork2[i] = zt;
    }

/*
    If the ratio of the positive to negative average residuals is
    much different from 1, then the sign that is not dominant
    probably represents ringing around anomalies of the dominant
    sign.  These ringing nodes are suppressed according to the
    ratio.
*/
    for (i=0; i<ntot; i++) {
        cp = ClosestPoint[i];
        if (cp > _CUTOFF_) continue;
        zt = Gwork2[i];
        if (zt < 0.0f  &&  pt > nt) {
            if (cp <= 0) {
                zt *= 0.75f;
            }
            else {
                zt /= ratio;
            }
        }
        else if (zt > 0.0f  &&  nt > pt) {
            if (cp <= 0) {
                zt *= 0.75f;
            }
            else {
                zt *= ratio;
            }
        }
        Gwork2[i] = zt;
    }

/*
    Nodes either 0, 1 or 2 spacings from data are checked against other nodes in the
    vicinity.  The mean positive and mean negative residuals are calculated.  If
    the node is opposite in sign to the sign of the dominant residual, it's value
    is lessened according to the ratio of the two residuals.  This should reduce the
    ringing effect in the vicinity of a local anomaly.
*/
    for (i=0; i<ntot; i++) {
        Gwork3[i] = Gwork2[i];
        if (ClosestPoint[i] > _CUTOFF_) {
            continue;
        }

        i1 = i / Ncol;
        j1 = i % Ncol;
        i2 = i1 + MEDIAN_WIDTH;
        j2 = j1 + MEDIAN_WIDTH;
        i1 -= MEDIAN_WIDTH;
        j1 -= MEDIAN_WIDTH;

        if (i1 < 0) i1 = 0;
        if (i1 > Nrow-1) i1 = Nrow - 1;
        if (j1 < 0) j1 = 0;
        if (j1 > Ncol-1) j2 = Ncol - 1;

        nt = 0.0f;
        pt = 0.0f;
        n1 = 0;
        n2 = 0;

        for (ii=i1; ii<=i2; ii++) {
            offset = ii * Ncol;
            for (jj=j1; jj<=j2; jj++) {
                k = offset + jj;
                cp = ClosestPoint[k];
                if (cp > _CUTOFF_) continue;
                zt = Gwork2[k];
                if (zt < 0.0f) {
                    n1++;
                    nt += zt;
                }
                else {
                    n2++;
                    pt += zt;
                }
            }
        }

        if (n1 > 0) {
            nt /= (CSW_F)n1;
            nt = -nt;
        }
        if (n2 > 0) {
            pt /= (CSW_F)n2;
        }

        if (n1 > 0) {
            ratio = pt / nt;
        }
        else {
            ratio = 1000.0f;
        }

        cp = ClosestPoint[i];
        zt = Gwork2[i];
        if (zt < 0.0) {
            if (ratio > 1.0f) {
                if (cp > 0) {
                    zt = 0.0f;
                }
                else {
                    zt /= ratio;
                }
            }
        }
        else {
            if (ratio < 1.0f) {
                if (cp > 0) {
                    zt = 0.0f;
                }
                else {
                    zt *= ratio;
                }
            }
        }
        Gwork3[i] = zt;
    }

/*
    Add the smoothed and processed residual back together to form
    the output Grid.
*/
    for (i=0; i<ntot; i++) {
        Grid[i] = Gwork1[i] + Gwork3[i];
    }

    return 1;

}  /*  end of private MedianFilter function  */

#undef _CUTOFF_





/*
  ****************************************************************

                A d j u s t F o r E r r o r s

  ****************************************************************

    This function iterates through the grid, doing error corrections
  and smoothing of the errors.  It does not subsequently refine the
  grid as IterateToFinalGrid does.  This essentially does the same
  thing as the last pass of IterateToFinalGrid.  This is called when
  post processing (e.g. median filter for local anomalies) is needed
  and the grid nodes subsequently need to be brought back into agreement
  with the data points.

*/

int CSWGrdCalc::AdjustForErrors (void)
{
    int       i, maxiter, dpsave;
    CSW_F     smsave, maxerr, avgerr, lastmax, lastavg, tinyavg, tinymax;

/*
    Refine the grid until the spacing is 1.
*/
    lastavg = 1.e30f;
    lastmax = 1.e30f;
    tinymax = 0.0f;
    avgerr = 0.0f;
    maxerr = 0.0f;

    maxiter = MAX_ITER;

    i = 0;

    while (i < maxiter) {

        if (FastGridFlag  &&  Ncoarse < 4  &&  lastavg < tinymax) {
            break;
        }

    /*
        Calculate the errors at points near nodes and use
        them to create an error grid.  The error grid is
        smoothed and then added to the elevation grid.
    */
        NumControlAdjust = 0;
        CalcErrorGrid (i);

    /*
        Calculate errors at all data points based
        on the current elevation grid.
    */
        maxerr = 1.e30f;
        avgerr = 1.e30f;
        if (FaultedFlag) {
            FaultedDataPointErrors (&maxerr, &avgerr);
        }
        else {
            DataPointErrors (&maxerr, &avgerr);
        }

        if (FastGridFlag) {
            break;
        }

    /*
        Set "good enough" error values for this coarseness.
    */
        tinyavg = Zrange / AVG_ERROR_DIVISOR / 2.0f;
        tinymax = Zrange / MAX_ERROR_DIVISOR / 2.0f;

        if (FastGridFlag) {
            tinyavg *= 10.f;
            tinymax *= 10.f;
        }

    /*
        If the errors are better than the target errors,
        go to the next grid spacing level.
    */
        if (maxerr < tinymax  &&  avgerr < tinyavg  &&  NumControlAdjust == 0) {
            break;
        }

        if (i > 0  &&  NumControlAdjust == 0) {

        /*
            If the errors are considerably better than the
            previous iteration, break out of the loop.
        */
            if (avgerr < lastavg / 100.0f  &&
                maxerr < lastmax / 100.0f) {
                break;
            }

        /*
            If the errors are any worse,
            break out of the loop.
        */
            if (avgerr > lastavg  ||  maxerr > lastmax) {
                memcpy ((char *)Grid, (char *)Gwork3, Ncol*Nrow*sizeof(CSW_F));
                break;
            }
        }

        lastmax = 2.0f * maxerr;
        lastavg = avgerr;
        memcpy ((char *)Gwork3, (char *)Grid, Ncol*Nrow*sizeof(CSW_F));

        i++;

        if (lastavg > 0.0f  &&
            lastavg < Zrange / AVG_ERROR_DIVISOR / 2.0f) i = MAX_ITER;
        if (lastmax > 0.0f  &&
            lastmax < Zrange / MAX_ERROR_DIVISOR / 2.0f) i = MAX_ITER;

        dpsave = DistancePower;
        DistancePower = 2;
        smsave = SmoothingFactor;
        SmoothingFactor = 7.0f - (CSW_F)i;
        if (SmoothingFactor < 3.0f) SmoothingFactor = 3.0f;
        if (PreferredStrike >= 0  ||  AnisotropyFlag) {
            StrikeSmoothGrid ();
        }
        else {
            SimpleSmoothGrid ();
        }
        DistancePower = dpsave;
        SmoothingFactor = smsave;

    }  /*  end of error correction iterations  */

/*
    If a conformable surface is being used, adjust the grid
    to be conformable with the conformable surface before
    refining to the next level or exiting in the case of
    already being at the finest level.
*/
    if (ConformalFlag == 1  &&  UseShapeGrid == 1) {
        AdjustForShapeGrid ();
    }

    AvgError = avgerr;
    MaxError = maxerr;

    return 1;

}  /*  end of private AdjustErrors function  */



/*
 ************************************************************************

                  A d j u s t F o r U n d e r f l o w

 ************************************************************************

  If any values in the Grid array, at the Ncoarse decimation, are very
  near zero, make them equal to zero.
*/

void CSWGrdCalc::AdjustForUnderflow (void)
{
    int             i, j0, j, k;
    double          tiny;

    tiny = (double)Z_ABSOLUTE_TINY;

    for (i=0; i<Nrow; i+=Ncoarse) {
        j0 = i * Ncol;
        for (j=0; j<Ncol; j+=Ncoarse) {
            k = j0 + j;
            if (Grid[k] < tiny  &&  Grid[k] > -tiny) {
                Grid[k] = 0.0f;
            }
        }
    }

    return;

}  /* end of private AdjustForUnderflow function */




/*
  ****************************************************************

                    C l i p T o M i n M a x

  ****************************************************************

    Clip the current coarse grid to the min and max specified by
  the grid options.

*/

int CSWGrdCalc::ClipOutputGridToMinMax
        (CSW_F *grid, int ncol, int nrow)
{
    int          i;

    if (GridMinValue < -1.e20f  &&  GridMaxValue > 1.e20f) {
        return 1;
    }

    for (i=0; i<ncol*nrow; i++) {
        if (grid[i] < -1.e20f  ||  grid[i] > 1.e20f) continue;
        if (grid[i] < GridMinValue) grid[i] = GridMinValue;
        if (grid[i] > GridMaxValue) grid[i] = GridMaxValue;
    }

    return 1;

}  /*  end of private ClipOutputGridToMinMax function  */




/*
 ************************************************************************

                    A d d T o E x t r a P o i n t s

 ************************************************************************

  If a point is outside the data index, add it to the list of points
  outside the area.  These may be used in gridding if nothing better
  is available.
*/

int CSWGrdCalc::AddToExtraPoints (int i)
{
    if (Ndata < 1) return 1;

    if (ExtraPoints == NULL) {
        ExtraPoints = (int *)csw_Malloc (Ndata * sizeof(int));
        if (ExtraPoints == NULL) {
            return -1;
        }
        Nextra = 0;
    }

    ExtraPoints[Nextra] = i;
    Nextra++;

    return 1;

}  /* end of private AddToExtraPoints function */





/*
 ******************************************************************************

                    A d d P a s s 1 P o i n t

 ******************************************************************************

*/

void CSWGrdCalc::AddPass1Point
        (double x1, double y1, double z1,
         double x2, double y2, double z2,
         double dzstrike, double avspace)

{
    int            ido, n;
    double         dz, xt, yt, zt, dx, dy, dist;

    if (Npass1 >= MaxPass1) {
        return;
    }

/*
 * If the dzstrike is negative, a single point from the
 * center of a triangle is added.
 */
    if (dzstrike < 0.0  &&  VerticalFaultFlag == 0) {
        Pass1X[Npass1] = (CSW_F)x1;
        Pass1Y[Npass1] = (CSW_F)y1;
        Pass1Z[Npass1] = (CSW_F)z1;
        Npass1++;
        return;
    }

    if (z1 < -1.e10  ||  z1 > 1.e10  ||
        z2 < -1.e10  ||  z2 > 1.e10) {
        return;
    }

    dz = z2 - z1;
    if (dz < 0.0) {
        dz = -dz;
    }
    if (dz >= dzstrike  &&  VerticalFaultFlag == 0) {
        return;
    }
    if (dz <= dzstrike / 3.0) {
        n = 3;
    }
    else {
        n = (int)(dzstrike / dz);
    }
    if (n > 3) n = 3;
    if (TriangulateFlag > 1) {
        n = 1;
    }

    if (VerticalFaultFlag == 1) {
        if (avspace > 0.0) {
            dx = x2 - x1;
            dy = y2 - y1;
            dist = dx * dx + dy * dy;
            n = (int)(dist / avspace + .5);
        }
        else {
            n = 1;
        }
        if (n > 4) n = 4;
    }

    dx = (x2 - x1) / (double)(n+1);
    dy = (y2 - y1) / (double)(n+1);
    dz = (z2 - z1) / (double)(n+1);

    for (ido=1; ido<=n; ido++) {
        xt = x1 + dx * ido;
        yt = y1 + dy * ido;
        if (xt < Xmin  ||  xt > Xmax  ||
            yt < Ymin  ||  yt > Ymax) {
            continue;
        }

        zt = z1 + dz * ido;
        if (VerticalFaultFlag == 0) {
            Pass1X[Npass1] = (CSW_F)xt;
            Pass1Y[Npass1] = (CSW_F)yt;
            Pass1Z[Npass1] = (CSW_F)zt;
            Npass1++;
            if (Npass1 >= MaxPass1-2) {
                break;
            }
        }
        else {
            AddDataPoint (xt, yt, zt);
        }

      /*
       * If the triangulate flag is 1 or less,
       * continue the loop.  If the flag is 2 or more,
       * a fault surface is being calculated.  In the fault case,
       * extrapolation is also done.
       */
        if (TriangulateFlag <= 1) {
            continue;
        }

        xt = x1 + dx * 2.0;
        yt = y1 + dy * 2.0;
        zt = z1 + dz * 2.0;
        if (VerticalFaultFlag == 0) {
            Pass1X[Npass1] = (CSW_F)xt;
            Pass1Y[Npass1] = (CSW_F)yt;
            Pass1Z[Npass1] = (CSW_F)zt;
            Npass1++;
        }
        else {
            AddDataPoint (xt, yt, zt);
        }

        xt = x1 - dx;
        yt = y1 - dy;
        zt = z1 - dz;
        if (VerticalFaultFlag == 0) {
            Pass1X[Npass1] = (CSW_F)xt;
            Pass1Y[Npass1] = (CSW_F)yt;
            Pass1Z[Npass1] = (CSW_F)zt;
            Npass1++;
        }
        else {
            AddDataPoint (xt, yt, zt);
        }

    }

    return;

} /* end of private AddPass1Point function */





/*
 ******************************************************************************

              A d d P o i n t s T o E m p t y R e g i o n s

 ******************************************************************************

  Triangulate the data points using the fault constraints.  For edges
  longer than a few grid cell sizes, and which have close to the same
  z value on each end point, add points to the Pass1 arrays by linearly
  interpolating the edge.

  If the TriangulateFlag is set to zero, this is all skipped.

*/

void CSWGrdCalc::AddPointsToEmptyRegions (void)
{
    NOdeStruct         *nodes = NULL;
    EDgeStruct         *edges = NULL, *eptr = NULL;
    TRiangleStruct     *triangles = NULL, *tptr = NULL;
    int                numnodes;
    int                numedges;
    int                numtris;
    double             *dxdata = NULL, *dydata = NULL, *dzdata = NULL;
    double             *xline = NULL, *yline = NULL, *zline = NULL;
    int                *linepts = NULL, *linetypes = NULL, nlines;
    int                istat, i, n1, n2, n3;
    double             x1, y1, z1, x2, y2, z2, dz, ztiny,
                       dx, dy, dist, avspace, avspace2, dzstrike;
    double             d1, d2, d3, avspace_edge;
    int                do_write;
    char               fname[200];


    auto fscope = [&]()
    {
        csw_Free (dxdata);
        csw_Free (xline);
        csw_Free (linepts);
        csw_Free (linetypes);
        csw_Free (triangles);
        csw_Free (edges);
        csw_Free (nodes);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (EmptyAreaGrid != NULL) {
        csw_Free (EmptyAreaGrid);
        EmptyAreaGrid = NULL;
    }

    Npass1 = 0;

/*
 * If the application has determined that a surface is not well
 * fit by a plane, then it can turn off triangulation by setting
 * the empty region flag to zero.  By default it is set to 1.
 */
    if (EmptyRegionFlag == 0) {
        return;
    }

/*
 * No triangulation at all is to be done unless the flag
 * is 1, 2 or 3
 */
    if (TriangulateFlag < 1  ||  TriangulateFlag > 3) {
        return;
    }

/*
 *  The triangulation needs double coordinates, so
 *  allocate and assign them here.
 */
    dxdata = (double *)csw_Malloc (Ndata * 3 * sizeof(double));
    if (dxdata == NULL) {
        return;
    }
    dydata = dxdata + Ndata;
    dzdata = dydata + Ndata;

    for (i=0; i<Ndata; i++) {
        dxdata[i] = (double)Xdata[i];
        dydata[i] = (double)Ydata[i];
        dzdata[i] = (double)Zdata[i];
    }

/*
 *  The current fault vectors are used as constraints
 *  in the triangulation.
 */
    istat = grd_fault_ptr->grd_get_current_fault_vectors (&xline, &yline, &zline,
                                           &linepts, &nlines);
    if (istat == -1) {
        return;
    }

    linetypes = (int *)csw_Malloc (nlines * sizeof(int));
    if (linetypes == NULL) {
        return;
    }
    for (i=0; i<nlines; i++) {
        linetypes[i] = GRD_DISCONTINUITY_CONSTRAINT;
    }

/*
 *  Calculate a tri mesh from the points only.
 */
    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    numnodes = 0;
    numedges = 0;
    numtris = 0;
    grd_triangle_ptr->grd_set_remove_zero_flag (0);
    istat = grd_triangle_ptr->grd_calc_trimesh
                             (dxdata, dydata, dzdata, Ndata,
                              xline, yline, zline,
                              linepts, linetypes, -nlines,
                              &nodes, &edges, &triangles,
                              &numnodes, &numedges, &numtris);
    grd_triangle_ptr->grd_set_remove_zero_flag (1);

    do_write = csw_GetDoWrite ();
    if (do_write) {
        strcpy (fname, "empty.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            triangles, numtris,
            edges, numedges,
            nodes, numnodes,
            fname);
    }

    if (istat == -1) {
        return;
    }

/*
 *  Use about 3 coarse grid node spacings as the cutoff for
 *  edge length to add a point.  Also, only add points
 *  are on edges with similar z values at each end.
 *  For large data sets, the edge distance must be
 *  relatively larger and the z difference relatively
 *  smaller to get points added.  This helps distribute
 *  the points through the entire large data set.
 */
    avspace = (Xspace + Yspace) / 2.0;
    if (VerticalFaultFlag == 0) {
        avspace *= Ncoarse;
        avspace_edge = avspace;
        avspace *= 3;
    }
    else {
        avspace *= 2.0;
        avspace_edge = avspace;
    }

    if (AnisotropyFlag != 1) {
        dzstrike = (Zmax - Zmin) / 10.0;
    }
    else {
        dzstrike = (Zmax - Zmin) / 20.0;
    }
    if (Ndata > 2 * MaxPass1) {
        avspace *= 1.5;
        dzstrike *= 0.8;
    }
    if (Ndata > 5 * MaxPass1) {
        avspace *= 1.5;
        dzstrike *= 0.8;
    }
    if (Ndata > 10 * MaxPass1) {
        avspace *= 1.5;
        dzstrike *= 0.8;
    }

    if (TriangulateFlag == 1) {
        avspace *= 2.0;
    }
    if (TriangulateFlag == 2) {
        dzstrike *= 5.0;
    }
    if (TriangulateFlag > 2) {
        dzstrike *= 20.0;
    }

    avspace2 = avspace;
    avspace *= avspace;
    avspace_edge *= avspace_edge;

/*
 * Add points approximately in the middle of triangles as needed.
 * This is not done if a vertical fault surface is being calculated.
 */
    if (VerticalFaultFlag == 0) {
      istat =
        grd_triangle_ptr->grd_calc_trimesh_node_normals
         (nodes, numnodes,
          edges, numedges,
          triangles, numtris);
      if (istat == -1) {
        return;
      }
      grd_triangle_ptr->grd_set_trimesh
                      (nodes, numnodes,
                       edges, numedges,
                       triangles, numtris);
      for (i=0; i<numtris; i++) {
        tptr = triangles + i;
        if (tptr->deleted) {
          continue;
        }
        grd_triangle_ptr->grd_get_nodes_for_triangle
            (tptr, edges, &n1, &n2, &n3);
        x1 = nodes[n1].x;
        y1 = nodes[n1].y;
        x2 = nodes[n2].x;
        y2 = nodes[n2].y;
        dx = x2 - x1;
        dy = y2 - y1;
        d1 = dx * dx + dy * dy;
        x1 = nodes[n1].x;
        y1 = nodes[n1].y;
        x2 = nodes[n3].x;
        y2 = nodes[n3].y;
        dx = x2 - x1;
        dy = y2 - y1;
        d2 = dx * dx + dy * dy;
        x1 = nodes[n2].x;
        y1 = nodes[n2].y;
        x2 = nodes[n3].x;
        y2 = nodes[n3].y;
        dx = x2 - x1;
        dy = y2 - y1;
        d3 = dx * dx + dy * dy;
        if (d1 + d2 + d3 > avspace * 3) {
          x1 = (nodes[n1].x + nodes[n2].x + nodes[n3].x) / 3.0;
          y1 = (nodes[n1].y + nodes[n2].y + nodes[n3].y) / 3.0;
          z1 = grd_triangle_ptr->grd_calc_z_for_triangle_normal
              (x1, y1, tptr);
          if (z1 < 1.e20) {
            AddPass1Point (x1, y1, z1,
                           0.0, 0.0, 0.0,
                           -100.0, 0.0);
          }
        }
      }
    }

/*
 *  Add points by interpolating edges as needed.  Do not use
 *  edges that are on faults.  If the vertical fault
 *  flag is set, only add if the z1 and z2 values are very close.
 */
    if (Zmax > Zmin) {
        ztiny = (Zmax - Zmin) / 50.0;
    }
    else {
        ztiny = .0001;
    }

    for (i=0; i<numedges; i++) {
        eptr = edges + i;
/*
        if (eptr->deleted  ||  eptr->flag  ||  eptr->tri2 == -1) {
            continue;
        }
*/
        if (eptr->deleted == 1) {
            continue;
        }
        if (eptr->flag == GRD_TRIMESH_FAULT_CONSTRAINT) {
            continue;
        }
        n1 = eptr->node1;
        n2 = eptr->node2;
        if (n1 < 0  ||  n2 < 0  ||
            n1 >= numnodes  ||  n2 >= numnodes) {
            continue;
        }
        x1 = nodes[n1].x;
        y1 = nodes[n1].y;
        z1 = nodes[n1].z;
        x2 = nodes[n2].x;
        y2 = nodes[n2].y;
        z2 = nodes[n2].z;
        if (VerticalFaultFlag == 1) {
            dz = z2 - z1;
            if (dz < 0.0) dz = -dz;
            if (dz > ztiny) continue;
        }
        dx = x2 - x1;
        dy = y2 - y1;
        dist = dx * dx + dy * dy;
        if (dist > avspace_edge) {
            AddPass1PointForEdge
                          (x1, y1, z1,
                           x2, y2, z2,
                           eptr,
                           dzstrike, avspace2);
        }
        if (Npass1 >= MaxPass1) {
            break;
        }
    }

    grd_triangle_ptr->grd_unset_trimesh ();
    grd_triangle_ptr->grd_free_trimesh_normals
       (nodes, numnodes,
        edges, numedges,
        triangles, numtris);


/*
 *  If no points were added, return.
 */
    if (Npass1 < 1) {
        Npass1 = 0;
        return;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        sprintf (fname, "pass1.xyz");
        grd_fileio_ptr->grd_write_float_points (
            Pass1X, Pass1Y, Pass1Z, Npass1, fname);
        sprintf (fname, "vfault.xyz");
        grd_fileio_ptr->grd_write_float_points (
            Xdata, Ydata, Zdata, Ndata, fname);
    }

/*
 * There are points in the empty areas.  Create
 * a grid at the same geometry as the coarse grid
 * to make use of these points.
 */
    CalcEmptyAreaGrid ();

    return;

}  /* end of private AddPointsToEmptyRegions function */




/*
 * Calculate a grid of empty area nodes.  This  grid is at the same
 * geometry as the coarse grid array.  If a node in the empty area
 * grid has any empty area points within 1/2 coarse grid spacing of
 * a grid node location, the empty area grid node will be assigned
 * a value that is the simple average of all points within the 1/2
 * grid node spacing.
 */
void CSWGrdCalc::CalcEmptyAreaGrid (void)
{
    int         i, j, k, ido, ntot;
    CSW_F       *grid = NULL, xsp, ysp, xmin, ymin;
    int         *npgrid = NULL;

    bool   bsuccess = false;

    auto fscope = [&]()
    {
        if (!bsuccess) {
            csw_Free (grid);
        }
        csw_Free (npgrid);
    };
    CSWScopeGuard func_scope_guard (fscope);


    ntot = Nccol * Ncrow;

    grid = (CSW_F *)csw_Malloc (ntot * sizeof(CSW_F));
    if (grid == NULL) {
        return;
    }

    npgrid = (int *)csw_Malloc (ntot * sizeof(int));
    if (npgrid == NULL) {
        return;
    }

    for (i=0; i<ntot; i++) {
        grid[i] = 1.e30f;
    }

    xsp = Xspace * Ncoarse;
    ysp = Yspace * Ncoarse;
    xmin = Xmin + xsp / 2.0f;
    ymin = Ymin + ysp / 2.0f;

    for (ido=0; ido<Npass1; ido++) {

        if (Pass1Z[ido] > 1.e20) {
            continue;
        }

        j = (int)((Pass1X[ido] - xmin) / xsp);
        if (j < 0  ||  j >= Nccol) continue;
        i = (int)((Pass1Y[ido] - ymin) / ysp);
        if (i < 0  ||  i >= Ncrow) continue;

        k = i * Nccol + j;

        if (grid[k] > 1.e20) {
            grid[k] = Pass1Z[ido];
            npgrid[k] = 1;
        }
        else {
            grid[k] += Pass1Z[ido];
            npgrid[k]++;
        }

    }

    for (i=0; i<ntot; i++) {
        if (grid[i] < 1.e20  &&  npgrid[i] > 0) {
            grid[i] = grid[i] / npgrid[i];
        }
    }

    EmptyAreaGrid = grid;

    bsuccess = true;

    return;

}


/*
 ********************************************************************

          g r d _ s m o o t h _ p l a t e a u _ g r i d

 ********************************************************************

  set the low and high plateau values and then smooth the grid

*/

int CSWGrdCalc::grd_smooth_plateau_grid
        (CSW_F *grid, int ncol, int nrow, int smfact,
         FAultLineStruct *faults, int nfaults,
         CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
         CSW_F minval, CSW_F maxval, CSW_F **smgrid,
         CSW_F low, CSW_F high)
{
    int          istat;

    LowPlateau = low;
    HighPlateau = high;

/*
 * The fixes for bug 8054 caused significant reproduceability
 * problems, so the plateau smoothing (which was only used for
 * the fix to this bug) is essentially disabled now.
 */
    LowPlateau = -1.e30f;
    HighPlateau = 1.e30f;

    istat = grd_smooth_grid (grid, ncol, nrow, smfact,
                     faults, nfaults,
                     x1, y1, x2, y2,
                     minval, maxval, smgrid);

    LowPlateau = -1.e30f;
    HighPlateau = 1.e30f;

    return istat;

}  /* end of function grd_smooth_plateau_grid */


/*
 *****************************************************************************

         C h e c k F o r C o i n c i d e n t P o i n t I n L i s t

 *****************************************************************************

  Return 1 if any point in the specified list is less than CoincidentDistance
  of the specified node.  This can be disables by setting the
  CoincidentDistance value to zero or less than zero.

*/
int CSWGrdCalc::CheckForCoincidentPointInList
          (int irow, int jcol,
           int *list, int nlist)
{
    double          xnode, ynode, xt, yt, dist, dchk;
    int             i, j;

    if (CoincidentDistance <= 0.0) {
        return 0;
    }
    if (list == NULL  ||  nlist < 1) {
        return 0;
    }

    xnode = Xmin + jcol * Xspace;
    ynode = Ymin + irow * Yspace;
    dchk = CoincidentDistance * CoincidentDistance;

    for (i=0; i<nlist; i++) {

        j = list[i];
        xt = Xdata[j] - xnode;
        yt = Ydata[j] - ynode;
        dist = xt * xt + yt * yt;

        if (dist <= dchk) {
            return 1;
        }
    }

    return 0;
}

/*
 *********************************************************************************

                      C h e c k F o r N o d e P o i n t s

 *********************************************************************************

*/

int CSWGrdCalc::CheckForNodePoints
         (CSW_F *x, CSW_F *y, int npts,
          double x1, double y1, double x2, double y2,
          int nc, int nr)
{
    int            i, it, jt, numnodes, n;
    double         xt, yt, xt2, yt2, tiny, dist;
    double         xspace, yspace;

    numnodes = nc * nr;
    if (npts < numnodes / 10) {
        return 0;
    }
    if (nc < 2  ||  nr < 2) {
        return 0;
    }

    xspace = (x2 - x1) / (double)(nc - 1);
    yspace = (y2 - y1) / (double)(nr - 1);

    x2 -= x1;
    y2 -= y1;

    tiny = (xspace + yspace) / 20.0;

    n = 0;
    for (i=0; i<npts; i++) {

        xt = x[i] - x1;
        yt = y[i] - y1;

        if (xt < 0.0  ||  yt < 0.0  ||
            xt > x2  ||  yt > y2) {
            continue;
        }

        jt = (int)(xt / xspace + .5);
        it = (int)(yt / yspace + .5);
        xt2 = jt * xspace;
        yt2 = it * yspace;

        xt -= xt2;
        yt -= yt2;
        dist = xt * xt + yt * yt;
        if (dist <= tiny) {
            n++;
        }
    }

    if (n >= numnodes / 10) {
        return 1;
    }

    return 0;

}



void CSWGrdCalc::grd_set_output_shifts (double x, double y)
{
    XOutputShift = x;
    YOutputShift = y;
    XOutputShift = XOutputShift;
    YOutputShift = YOutputShift;
}

void CSWGrdCalc::grd_set_debug_output_flag (int ival)
{
    debug_output_flag = ival;
    debug_output_flag = debug_output_flag;
}



/*-------------------------------------------------------------------------------*/

/*
 * Identify grid nodes that differ drastically from their nearest neighbors.
 * The z values at these nodes will be replaced with the plane fit through the
 * nearest neighbors.
 */

int CSWGrdCalc::grd_filter_grid_spikes (
    CSW_F      *grid,
    int        ncol,
    int        nrow,
    int        isfact)
{
    double     xclose[9], yclose[9], zclose[9];
    double     sfact, zsum, zt, zfitavg, zmin, zmax,
               zfitmin, coef[3];
    CSW_F      *gwork = NULL;
    int        i, j, k, ii, jj, kk, i1, i2, j1, j2,
               n, nclose, offset, offset2, yt2;
    int        ido, ndo, istat, ndone;


    auto fscope = [&]()
    {
        csw_Free (gwork);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Check for obvious errors.
 */
    if (grid == NULL  ||  ncol < 2  ||  nrow < 2) {
        return -1;
    }

/*
 * If less than 3 columns or rows, do not filter, just
 * return the input grid unchanged.
 */
    if (ncol < 3  ||  nrow < 3) {
        return 1;
    }

    if (isfact < 1) isfact = 1;
    if (isfact > 10) isfact = 10;

/*
 * Use the range of the input grid to calculate a minimum
 * averazge zfit for local planes.  For a smoothing factor
 * of 5, this if 1/200th of the grid z range.  For higher
 * moothing factors, the minimum zfit is smaller.  For lower
 * smoothing factors, the zfit min is larger.
 */
    zmin = 1.e30;
    zmax = -1.e30;
    for (i=0; i<ncol*nrow; i++) {
        if (grid[i] < zmin) zmin = (double)grid[i];
        if (grid[i] > zmax) zmax = (double)grid[i];
    }

    if (zmax - zmin < 1.e-7) {
        return 1;
    }

    zfitmin = (zmax - zmin) / (20.0 * isfact * isfact);
    zfitmin *= zfitmin;

/*
 * Allocate a work grid as large as the input grid.
 */
    gwork = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (gwork == NULL) {
        return -1;
    }

/*
 * Loop through the spike removal several times, depending
 * on the specified isfact.  The isfact value is also used
 * as the basis for what is considered a spike.  For larger
 * isfact values, a smaller deviation from the nearest neighbors
 * is considered a spike and removed.  The overall effect is
 * that smoother surfaces are produced by higher isfact values.
 */
    ndo = isfact;
    if (ndo > 9) ndo = 9;
    sfact = 15.0 / isfact;
    if (sfact < 1.5) sfact = 1.5;
    if (sfact > 10.0) sfact = 10.0;

    for (ido=0; ido<ndo; ido++) {

        ndone = 0;
        for (i=0; i<nrow; i++) {
            offset = i * ncol;
            i1 = i - 1;
            i2 = i + 1;
            if (i1 < 0) {
                i1 = 0;
                i2 = 2;
            }
            if (i2 > nrow-1) {
                i2 = nrow - 1;
                i1 = nrow - 3;
            }
            for (j=0; j<ncol; j++) {
                k = offset + j;
                j1 = j - 1;
                j2 = j + 1;
                if (j1 < 0) {
                    j1 = 0;
                    j2 = 2;
                }
                if (j2 > ncol-1) {
                    j2 = ncol - 1;
                    j1 = ncol - 3;
                }

            /*
             * Collect the nearest neighbors to the kth node,
             * not including the kth node itself.
             */
                n = 0;
                for (ii=i1; ii<=i2; ii++) {
                    offset2 = ii * ncol;
                    yt2 = ii - i;
                    for (jj=j1; jj<=j2; jj++) {
                        kk = offset2 + jj;
                        if (kk != k) {
                            xclose[n] = jj - j;
                            yclose[n] = yt2;
                            zclose[n] = grid[kk];
                            n++;
                        }
                    }
                } /* end of neighbor collection loops. */

            /*
             * Make the work grid node value the same as the grid node
             * value in case no filtering is needed.
             */
                gwork[k] = grid[k];

            /*
             * If less than 3 close points, cannot filter.
             */
                if (n < 3) {
                    continue;
                }

                nclose = n;

            /*
             * Fit a plane to the close points and calculate
             * the squared vertical distance to that plane at each
             * close point.  The average squared vertical distance
             * is calculated from the individual values at each point.
             */
                istat =
                  grd_triangle_ptr->grd_CalcPreciseDoublePlane (
                    xclose, yclose, zclose, nclose, coef);
                if (istat == -1) {
                    continue;
                }
                zsum = 0.0;
                for (n=0; n<nclose; n++) {
                    zt = coef[0] + coef[1] * xclose[n] + coef[2] * yclose[n];
                    zt = zclose[n] - zt;
                    zt *= zt;
                    zsum += zt;
                }
                zfitavg = zsum / nclose;
                if (zfitavg < zfitmin) zfitavg = zfitmin;

            /*
             * Compare the squared vertical distance to the plane at the kth
             * node (by definition at 0,0 of the close points) to the
             * average squared vertical distance.
             */
                zt = grid[k] - coef[0];
                zt *= zt;

                if (zt >= zfitavg * sfact) {
                    gwork[k] = (CSW_F) (coef[0]);
                    ndone++;
                }

            } /* end of j loop through columns of a single row */

        } /* end of i loop through rows */

    /*
     * If no spikes were removed, exit the ido loop.
     */
        if (ndone < 1) {
            break;
        }

    /*
     * Copy the gwork to the grid for the next iteration.
     */
        memcpy (grid, gwork, ncol * nrow * sizeof(CSW_F));

    } /* end of ido loop specified by the input isfact */

    return 1;

}

/*---------------------------------------------------------------------------------*/

void CSWGrdCalc::MovingAverage
     (CSW_F *grid, int ncol, int nrow,
      int smfact, int lightflag)
{
    int           i, j, k, ii, jj, kk, i1, i2, j1, j2;
    int           offset, offset2;
    CSW_F         sum1, sum2, *gw = NULL;


    auto fscope = [&]()
    {
        csw_Free (gw);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (grid == NULL  ||  ncol < 5  ||  nrow < 5) {
        return;
    }

    gw = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (gw == NULL) {
      return;
    }

    for (i=0; i<nrow; i++) {
      i1 = i - 1;
      i2 = i + 1;
      if (i1 < 0) i1 = 0;
      if (i2 > nrow-1) i2 = nrow - 1;
      offset = i * ncol;
      for (j=0; j<ncol; j++) {
        j1 = j - 1;
        j2 = j + 1;
        if (j1 < 0) j1 = 0;
        if (j2 > ncol-1) j2 = ncol-1;
        k = offset + j;

        sum1 = 0.0;
        sum2 = 0.0;
        for (ii=i1; ii<=i2; ii++) {
          offset2 = ii * ncol;
          for (jj=j1; jj<=j2; jj++) {
            kk = offset2 + jj;
            if (grid[kk] < 1.e20) {
              sum1 += grid[kk];
              sum2++;
            }
          }
        }

        if (sum2 < 0.1) {
          gw[k] = grid[k];
        }
        else {
          gw[k] = sum1 / sum2;
        }
      }
    }

    if (lightflag == 0) {
      memcpy (grid, gw, ncol * nrow * sizeof(CSW_F));
    }
    else {
      sum2 = smfact / 5.0f;
      sum2 *= sum2;
      for (i=0; i<ncol*nrow; i++) {
        if (grid[i] < 1.e20  &&  gw[i] < 1.e20) {
          sum1 = gw[i] * sum2;
          grid[i] = (grid[i] + sum1) / (1.0f + sum2);
        }
      }
    }

    return;

}


/*----------------------------------------------------------------------------------*/

int CSWGrdCalc::FindDifferentZValue
         (CSW_F *x, CSW_F *y, CSW_F *z)
{
    CSW_F         xt, yt, zt, z1, z2, dx, dy, dist, dmin;
    int           i, imin;

    if (FaultedFlag == 1) {
        return 0;
    }

    xt = *x;
    yt = *y;
    zt = *z;

    z1 = zt - Zrange / 1000.0f;
    z2 = zt + Zrange / 1000.0f;

    dmin = 1.e30f;
    imin = -1;

    for (i=0; i<Ndata; i++) {
        if (Zdata[i] <= z1  ||
            Zdata[i] >= z2) {
            dx = xt - Xdata[i];
            dy = yt - Ydata[i];
            dist = dx * dx + dy * dy;
            if (dist < dmin) {
                imin = i;
                dmin = dist;
            }
        }
    }

    if (imin >= 0) {
        *x = Xdata[imin];
        *y = Ydata[imin];
        *z = Zdata[imin];
        return 1;
    }

    return 0;

}



/*----------------------------------------------------------------*/

void CSWGrdCalc::AddDataPoint (double xt, double yt, double zt)
{

    if (Ndata >= MaxData) {
        return;
    }

    Xdata[Ndata] = (CSW_F)xt;
    Ydata[Ndata] = (CSW_F)yt;
    Zdata[Ndata] = (CSW_F)zt;
    Ndata++;

    return;
}

/*
 ******************************************************************************

                 A d d P a s s 1 P o i n t F o r E d g e

 ******************************************************************************

*/

void CSWGrdCalc::AddPass1PointForEdge
          (double x1, double y1, double z1,
           double x2, double y2, double z2,
           EDgeStruct *eptr,
           double dzstrike, double avspace)

{
    int            ido, n;
    double         dz, xt, yt, zt, dx, dy, dist;

    if (Npass1 >= MaxPass1) {
        return;
    }

/*
 * If the dzstrike is negative, a single point from the
 * center of a triangle is added.
 */
    if (dzstrike < 0.0  &&  VerticalFaultFlag == 0) {
        Pass1X[Npass1] = (CSW_F)x1;
        Pass1Y[Npass1] = (CSW_F)y1;
        Pass1Z[Npass1] = (CSW_F)z1;
        Npass1++;
        return;
    }

    if (z1 < -1.e10  ||  z1 > 1.e10  ||
        z2 < -1.e10  ||  z2 > 1.e10) {
        return;
    }

    dz = z2 - z1;
    if (dz < 0.0) {
        dz = -dz;
    }
    if (dz >= dzstrike  &&  VerticalFaultFlag == 0) {
        return;
    }
    if (dz <= dzstrike / 3.0) {
        n = 3;
    }
    else {
        n = (int)(dzstrike / dz);
    }
    if (n > 3) n = 3;
    if (TriangulateFlag > 1) {
        n = 1;
    }

    if (VerticalFaultFlag == 1) {
        if (avspace > 0.0) {
            dx = x2 - x1;
            dy = y2 - y1;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            n = (int)(dist / avspace + .5);
        }
        else {
            n = 1;
        }
        if (n > 4) n = 4;
    }

    dx = (x2 - x1) / (double)(n+1);
    dy = (y2 - y1) / (double)(n+1);
    dz = (z2 - z1) / (double)(n+1);

    for (ido=1; ido<=n; ido++) {
        xt = x1 + dx * ido;
        yt = y1 + dy * ido;
        if (xt < Xmin  ||  xt > Xmax  ||
            yt < Ymin  ||  yt > Ymax) {
            continue;
        }

        zt = grd_triangle_ptr->grd_calc_z_for_edge_normal
            (xt, yt, eptr);
        if (zt > 1.e20) {
            zt = z1 + dz * ido;
        }
        if (VerticalFaultFlag == 0) {
            Pass1X[Npass1] = (CSW_F)xt;
            Pass1Y[Npass1] = (CSW_F)yt;
            Pass1Z[Npass1] = (CSW_F)zt;
            Npass1++;
            if (Npass1 >= MaxPass1-2) {
                break;
            }
        }
        else {
            AddDataPoint (xt, yt, zt);
        }

      /*
       * If the triangulate flag is 1 or less,
       * continue the loop.  If the flag is 2 or more,
       * a fault surface is being calculated.  In the fault case,
       * extrapolation is also done.
       */
        if (TriangulateFlag <= 1  ||  VerticalFaultFlag == 1) {
            continue;
        }

        xt = x1 + dx * 2.0;
        yt = y1 + dy * 2.0;
        zt = z1 + dz * 2.0;
        if (VerticalFaultFlag == 0) {
            Pass1X[Npass1] = (CSW_F)xt;
            Pass1Y[Npass1] = (CSW_F)yt;
            Pass1Z[Npass1] = (CSW_F)zt;
            Npass1++;
        }
        else {
            AddDataPoint (xt, yt, zt);
        }

        xt = x1 - dx;
        yt = y1 - dy;
        zt = z1 - dz;
        if (VerticalFaultFlag == 0) {
            Pass1X[Npass1] = (CSW_F)xt;
            Pass1Y[Npass1] = (CSW_F)yt;
            Pass1Z[Npass1] = (CSW_F)zt;
            Npass1++;
        }
        else {
            AddDataPoint (xt, yt, zt);
        }

    }

    return;

} /* end of private AddPass1Point function */




int CSWGrdCalc::grd_WriteFaultLines (
    FAultLineStruct   *faults,
    int               nfaults,
    const char        *fname)
{
    CSW_F     *xlines = NULL, *ylines = NULL, *zlines = NULL;
    int       i, *icomp = NULL, *ivec = NULL, *itype = NULL,
              nlines, istat;


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

    istat = grd_fault_ptr->grd_fault_structs_to_arrays (
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

    grd_fileio_ptr->grd_write_float_lines (
        xlines, ylines, zlines,
        nlines, icomp, ivec,
        fname);

    return 1;
}




/*
  ****************************************************************

        g r d _ c a l c _ g r i d _ f r o m _ d o u b l e

  ****************************************************************

  function name:    grd_calc_grid_from_double              (int)

  call sequence:    grd_calc_grid_from_double (x, y, z, error, npts,
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

int CSWGrdCalc::grd_calc_grid_from_double
       (double *x, double *y, double *z, CSW_F *error, int npts,
        CSW_F *grid, char *mask, char **report,
        int ncol, int nrow,
        double x1, double y1, double x2, double y2,
        FAultLineStruct *faults, int nfaults,
        GRidCalcOptions *options)
{
    int           istat, i;
    CSW_F         *xt = NULL, *yt = NULL, *zt = NULL,
                  xt1, yt1, xt2, yt2;
    double        xmin, ymin;
    int           fsave;


    auto fscope = [&]()
    {
        csw_Free (xt);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    errors that will screw up this function
*/
    if (npts < 1) {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

    if (faults  &&  nfaults < 1) {
        grd_utils_ptr->grd_set_err (9);
        return -1;
    }

    if (!faults  &&  nfaults > 0) {
        grd_utils_ptr->grd_set_err (9);
        return -1;
    }

/*
    allocate CSW_F arrays
*/
MSL
    xt = (CSW_F *)csw_Malloc (npts * 3 * sizeof(CSW_F));
    if (!xt) {
        grd_utils_ptr->grd_set_err (1);
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

    grd_set_output_shifts (xmin, ymin);

/*
 * Set the conformable grid shifted to the same origin.
 */
    if (ConformableFlag == 1) {
        grd_set_control_surface (
            ConformableGrid,
            ConformableNcol,
            ConformableNrow,
            (CSW_F)(ConformableGXmin - xmin),
            (CSW_F)(ConformableGYmin - ymin),
            (CSW_F)(ConformableGXmax - xmin),
            (CSW_F)(ConformableGYmax - ymin),
            GRD_CONFORMABLE);
    }
    else {
        grd_set_control_surface (
            NULL, 0, 0,
            1.e30f, 1.e30f, -1.e30f, -1.e30f,
            GRD_CONFORMABLE);
    }

/*
    copy to the CSW_F arrays
*/
    for (i=0; i<npts; i++) {
        xt[i] = (CSW_F)x[i];
        yt[i] = (CSW_F)y[i];
        zt[i] = (CSW_F)z[i];
    }

    xt1 = (CSW_F)x1;
    yt1 = (CSW_F)y1;
    xt2 = (CSW_F)x2;
    yt2 = (CSW_F)y2;

/*
    Define the fault vectors and set the faulting option.
*/
    fsave = 0;
    grd_fault_ptr->grd_free_faults ();
    if (faults  &&  nfaults > 0) {

        istat = grd_fault_ptr->grd_define_and_shift_fault_vectors (faults, nfaults, xmin, ymin);
        if (istat == -1) {
            return -1;
        }
        grd_set_calc_option (GRD_FAULTED_GRID_FLAG, 1, 0.0f);
        if (options) {
            fsave = options->faulted_flag;
            options->faulted_flag = 1;
        }
    }
    else {
        grd_fault_ptr->grd_free_faults ();
    }

/*
    calculate a CSW_F grid
*/
    istat = grd_calc_grid (xt, yt, zt, error, npts,
                           grid, mask, report, ncol, nrow,
                           xt1, yt1, xt2, yt2, options);

    if (istat == -1  &&  options) {
        options->error_number = grd_utils_ptr->grd_get_err ();
    }

/*
    add back the minimum x and y to the input double data
*/
    for (i=0; i<npts; i++) {
        x[i] += xmin;
        y[i] += ymin;
    }

    if (faults  &&  nfaults > 0) {
        if (options) {
            options->faulted_flag = fsave;
        }
    }

    grd_set_output_shifts (0.0, 0.0);

    return istat;

}  /*  end of function grd_calc_grid_from_double  */




/*
  ****************************************************************

  g r d _ s e t _ c o n f o r m a b l e _ s u r f a c e _ f r o m _ d o u b l e

  ****************************************************************

  function name:        grd_set_conformable_surface_from_double         (int)

  call sequence:        grd_set_conformable_surface_from_double
                                              (grid, ncol, nrow,
                                               x1, y1, x2, y2)

  purpose:              Set or unset the grid surface that will be used to
                        control the shape of the next grid calculated with
                        grd_calc_grid_from_double.  Call this with grid set
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

int CSWGrdCalc::grd_set_conformable_surface_from_double
      (CSW_F *grid, int ncol, int nrow,
       double x1, double y1, double x2, double y2)

{

    ConformableGrid = grid;
    ConformableNcol = ncol;
    ConformableNrow = nrow;
    ConformableGXmin = x1;
    ConformableGYmin = y1;
    ConformableGXmax = x2;
    ConformableGYmax = y2;

    ConformableFlag = 1;

    return 1;

}  /*  end of function grd_set_conformable_surface_from_double  */


/*---------------------------------------------------------------------------------*/

void CSWGrdCalc::RemoveSpikes
     (CSW_F *grid, int ncol, int nrow, int smfact, int nc)
{
    int           i, j, k, ii, jj, kk, i1, i2, j1, j2;
    int           offset, offset2;
    CSW_F         sum1, sum2, *gw = NULL;
    CSW_F         zmin, zmax, zt, zr;
    CSW_F         smult = 1.0;
    CSW_F         smult2 = 1.0;
    CSW_F         zrange;

    auto fscope = [&]()
    {
        csw_Free (gw);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (grid == NULL  ||  ncol < 2  ||  nrow < 2  ||  smfact < 1) {
        return;
    }

    gw = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (gw == NULL) {
      return;
    }

    zmin = 1.e30;
    zmax = -1.e30;
    for (i=0; i<ncol * nrow; i++) {
        zt = grid[i];
        if (zt < 1.e20) {
            if (zt < zmin) zmin = zt;
            if (zt > zmax) zmax = zt;
        }
    }
    if (zmin > zmax) {
        return;
    }

    zrange = zmax - zmin;
    if (zrange <= 0.0) {
        return;
    }

    zrange /= 40.0;
    smult = 2.0 / (CSW_F)smfact;

    if (nrow * ncol > 250000) {
        zrange *= .75;
        smult *= .75;
    }
    zrange *= .75;
    smult *= .75;
    smult2 = 1.0;

    double sm2;
    int  nc2 = nc * 2;
    bool iedge, jedge;

    for (i=0; i<nrow; i++) {
      i1 = i - 1;
      i2 = i + 1;
      if (i1 < 0) i1 = 0;
      if (i2 > nrow-1) i2 = nrow - 1;
      iedge = false;
      if (i1 < nc2  ||  i2 > nrow - nc2) iedge = true;
      offset = i * ncol;
      for (j=0; j<ncol; j++) {
        j1 = j - 1;
        j2 = j + 1;
        if (j1 < 0) j1 = 0;
        if (j2 > ncol-1) j2 = ncol-1;
        jedge = false;
        if (j1 < nc2  ||  j2 > ncol - nc2) jedge = true;
        k = offset + j;

        sum1 = 0.0;
        sum2 = 0.0;
        zmin = 1.e30;
        zmax = -1.e30;
        for (ii=i1; ii<=i2; ii++) {
          offset2 = ii * ncol;
          for (jj=j1; jj<=j2; jj++) {
            kk = offset2 + jj;
            if (kk == k) continue;
            zt = grid[kk];
            if (zt < 1.e20) {
              sum1 += zt;
              sum2++;
              if (zt < zmin) zmin = zt;
              if (zt > zmax) zmax = zt;
            }
          }
        }

        if (sum2 < 2.1) {
          gw[k] = grid[k];
        }
        else {
          sm2 = smult2;
          if (iedge  ||  jedge) {
              sm2 = .5;
          }
          zr = fabs (grid[k] - sum1 / sum2);
          zt = zmax - zmin;
          if (zr > sm2 * smult * zt  &&  zr >= sm2 * zrange) {
              gw[k] = sum1 / sum2;
          }
          else {
              gw[k] = grid[k];
          }
        }
      }
    }

    memcpy (grid, gw, ncol * nrow * sizeof(CSW_F));

    return;

}


void CSWGrdCalc::grd_set_noisy_data_flag (int ndf) {
      NoisyDataFlag = 0;
      if (ndf != 0) NoisyDataFlag = 1;
};

