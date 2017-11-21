
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    con_calc.cc

    Implement the methods of the CSWConCalc class.  Thgese methods are
    used to calculate contour lines from grids or triangles.  The lines
    are not actually drawn by this class.

*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"
#include "csw/utils/private_include/ply_calc.h"

#include "csw/utils/private_include/simulP.h"
#include "csw/utils/private_include/gpf_utils.h"
#include "csw/surfaceworks/private_include/grd_fault.h"

#include "csw/surfaceworks/private_include/con_calc.h"


/*
  ****************************************************************

              c o n _ s e t _ c a l c _ o p t i o n

  ****************************************************************

  Set an option to use for subsequent contour calculation or
  set all contour calculation options to their defaults.

*/

int CSWConCalc::con_set_calc_option
            (int tag, int ival,CSW_F fval)
{

/*
    Set options, adjusting for obviously bogus values.
*/
    switch (tag) {

        case CON_DEFAULT_OPTIONS:
            OptContourSmoothing = 3;
            OptContourBaseValue = 0.0f;
            OptContourLogBase = 0.0f;
            OptContourNullValue = 1.e28f;
            OptContourMinorCrowd = 0.0f;
            OptContourMajorCrowd = 0.0f;
            OptContourFastFlag = 0;
            OptContourLogConvert = 0;
            OptFillPrecision = 0.02f;
            OptStepGridFlag = 0;
            OptFaultedFlag = 0;
            OptContourResampleFlag = 1;
            OptContourThicknessFlag = 0;
            break;

        case CON_CONVERT_TO_LOG:
            if (ival != 1) ival = 0;
            OptContourLogConvert = ival;
            break;

        case CON_SMOOTHING:
            OptContourSmoothing = ival;
            if (OptContourSmoothing <= 0) OptContourSmoothing = 0;
            if (OptContourSmoothing > 9) OptContourSmoothing = 9;
            break;

        case CON_BASE_VALUE:
            OptContourBaseValue = fval;
            if (OptContourBaseValue < 0.0f) OptContourBaseValue = 0.0f;
            break;

        case CON_LOG_BASE:
            OptContourLogBase = fval;
            if (OptContourLogBase <= 1.0f) OptContourLogBase = 0.0f;
            break;

        case CON_NULL_VALUE:
            OptContourNullValue = fval;
            break;

        case CON_MINOR_CROWD:
            OptContourMinorCrowd = fval;
            if (OptContourMinorCrowd < 0.0f) OptContourMinorCrowd = 0.0f;
            break;

        case CON_MAJOR_CROWD:
            OptContourMajorCrowd = fval;
            if (OptContourMajorCrowd < 0.0f) OptContourMajorCrowd = 0.0f;
            break;

        case CON_FAST_FLAG:
            OptContourFastFlag = ival;
            if (OptContourFastFlag != 1) OptContourFastFlag = 0;
            break;

        case CON_FILL_PRECISION:
            if (fval < 0.001f) fval = 0.001f;
            if (fval > 0.5f) fval = 0.5f;
            OptFillPrecision = fval;
            break;

        case CON_STEP_GRID_FLAG:
            OptStepGridFlag = ival;
            break;

        case CON_FAULTED_GRID_FLAG:
            OptFaultedFlag = ival;
            break;

        case CON_RESAMPLE_FLAG:
            OptContourResampleFlag = ival;
            break;

        case CON_THICKNESS_FLAG:
            OptContourThicknessFlag = ival;
            break;

        case CON_CONTOUR_IN_FAULTS:
            OptContourInFaultsFlag = ival;
            break;

        default:
            grd_utils_ptr->grd_set_err (2);
            return -1;

    }

    return 1;


}  /*  end of function con_set_calc_option  */






/*
  ****************************************************************

            c o n _ s e t _ c a l c _ o p t i o n s

  ****************************************************************

    Set all of the contour calculation options to the values specified
  in the COntourCalcOptions structure.

*/

int CSWConCalc::con_set_calc_options (COntourCalcOptions *options)
{

    if (options == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return 1;
    }

    con_set_calc_option (CON_SMOOTHING, options->smoothing, 0.0f);
    con_set_calc_option (CON_BASE_VALUE, 0, options->base_value);
    con_set_calc_option (CON_LOG_BASE, 0, options->log_base);
    con_set_calc_option (CON_NULL_VALUE, 0, options->null_value);
    con_set_calc_option (CON_MINOR_CROWD, 0, options->minor_crowd);
    con_set_calc_option (CON_MAJOR_CROWD, 0, options->major_crowd);
    con_set_calc_option (CON_FAST_FLAG, options->fast_flag, 0.0f);
    con_set_calc_option (CON_CONVERT_TO_LOG, options->convert_to_log, 0.0f);
    con_set_calc_option (CON_FILL_PRECISION, 0, options->fill_precision);
    con_set_calc_option (CON_STEP_GRID_FLAG, options->step_flag, 0.0f);
    con_set_calc_option (CON_FAULTED_GRID_FLAG, options->faulted_flag, 0.0f);
    con_set_calc_option (CON_RESAMPLE_FLAG, options->resample_flag, 0.0f);
    con_set_calc_option (CON_THICKNESS_FLAG, options->thickness_flag, 0.0f);
    con_set_calc_option (CON_CONTOUR_IN_FAULTS, options->contour_in_faults_flag, 0.0f);

    return 1;

}  /*  end of function con_set_calc_options  */






/*
  ****************************************************************

        c o n _ d e f a u l t _ c a l c _ o p t i o n s

  ****************************************************************

    Set all of the members of the specified contour calculation options
  structure to their default values.

*/

int CSWConCalc::con_default_calc_options (COntourCalcOptions *options)
{

    if (options == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return 1;
    }

    memset (options, 0, sizeof(COntourCalcOptions));

    options->convert_to_log = 0;
    options->smoothing = 3;
    options->resample_flag = 1;
    options->thickness_flag = 0;
    options->base_value = 0.0f;
    options->log_base = 0.0f;
    options->null_value = 1.e28f;
    options->minor_crowd = 0.0f;
    options->major_crowd = 0.0f;
    options->fast_flag = 0;
    options->faulted_flag = 0;
    options->contour_in_faults_flag = 0;
    options->step_flag = 0;
    options->fill_precision = .02f;
    options->contour_interval = -1.0f;
    options->major_spacing = 0;
    options->first_contour = 1.0f;
    options->base_grid_value = -1.e30f;
    options->top_grid_value = 1.e30f;
    options->last_contour = 0.0f;
    options->nminor = 0;
    options->nmajor = 0;

    options->error_number = 0;

    return 1;

}  /*  end of function con_default_calc_options  */






/*
  ****************************************************************

         c o n _ s e t _ c o n t o u r _ i n t e r v a l s

  ****************************************************************

    Specify contour interval, major spacing, minimum contour,
  maximum contour, and a list of specific contours if desired.
  If the contour interval is greater than zero, then the lists
  of major and minor contours are ignored.  If the contour
  interval is less than or equal to zero and both lists of
  specific contours are NULL (or both counts are less than 1)
  then the contour calculation function will decide on a
  contour interval based on the min and max values of the grid
  to be contoured.  If this function is never called, the automatic
  contour interval is used.

*/

int CSWConCalc::con_set_contour_intervals
                  (CSW_F contour_interval, int major_spacing,
                  CSW_F first_contour,CSW_F last_contour,
                  CSW_F *minor_contours, int nminor,
                  CSW_F *major_contours, int nmajor)
{

    ContourInterval = contour_interval;
    MajorSpacing = major_spacing;
    if (MajorSpacing < 0) MajorSpacing = 0;
    FirstContour = first_contour;
    LastContour = last_contour;
    if (minor_contours  &&  nminor > 0) {
        if (nminor > MAX_CONTOUR_LEVELS) nminor = MAX_CONTOUR_LEVELS;
        memcpy (MinorContours, minor_contours, nminor*sizeof(CSW_F));
    }
    NumMinor = nminor;
    if (major_contours  &&  nmajor > 0) {
        if (nmajor > MAX_CONTOUR_LEVELS) nmajor = MAX_CONTOUR_LEVELS;
        memcpy (MajorContours, major_contours, nmajor*sizeof(CSW_F));
    }
    NumMajor = nmajor;

    if (ContourInterval > WildFloat  ||  ContourInterval < -WildFloat) {
        ContourInterval = -1.f;
    }
    if (MajorSpacing > WildInt  ||  MajorSpacing < -WildInt) {
        MajorSpacing = -1;
    }
    if (FirstContour > WildFloat  ||  FirstContour < -WildFloat) {
        FirstContour = 0.0f;
    }
    if (LastContour > WildFloat  ||  LastContour < -WildFloat) {
        LastContour = 0.0f;
    }

    return 1;

}  /*  end of function con_set_contour_intervals  */








/*
  ****************************************************************

                 c o n _ c a l c _ c o n t o u r s

  ****************************************************************

  Calculate contours through a rectangular grid.  The contours
  are output as an array of COntourOutputRec structures.  The
  output space is allocated here and must be csw_Freed by the application
  by calling con_FreeContours.  This is an internal function.  The
  application should call con_CalcContours rather than calling this
  function directly.

*/

int CSWConCalc::con_calc_contours
        (CSW_F *rawgridinput, int ncol, int nrow,
        CSW_F x1,CSW_F y1,CSW_F x2,CSW_F y2,
        CSW_F scale, COntourOutputRec **contours, int *ncont,
        COntourCalcOptions *options)
{
    int                i, istat, maxcol, maxrow, ncnew, nrnew;
    CSW_F              zt1, zt2, crowd1, crowd2, csav,
                       tiny, firstsav, lastsav,
                       *gridin = NULL, *grid = NULL, *tgrid = NULL;
    CSW_F              cnull, top_adjust;
    char               *cenv = NULL;
    CSW_F              *narrowgrid = NULL;
    int                narrowncol, narrownrow;
    CSW_F              *rawgridin = NULL;
    CSW_F              smooth_size2, null_ratio;
    int                num_null;

    bool         b_success = false;

    firstsav = FirstContour;
    lastsav = LastContour;
    csav = ContourInterval;

    auto fscope = [&]()
    {
        if (b_success == false) {
            *ncont = 0;
            *contours = NULL;
            if (rawgridin != rawgridinput) {
                csw_Free (rawgridin);
                rawgridin = NULL;
            }
            csw_Free (gridin);
            gridin = NULL;
            con_free_contours (ContourData, NconData);
            FirstContour = firstsav;
            LastContour = lastsav;
            ContourInterval = csav;
            ContourData = NULL;
            NconData = 0;
            MaxConData = 0;
        }
        csw_Free (grid);
        grid = NULL;
        csw_Free (tgrid);
        tgrid = NULL;
        FreeMem ();
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
 * Initialize output pointers in case of error.
 */
    *contours = NULL;
    *ncont = 0;

/*
    Check for obvious errors.
*/
    if (ncol < -WildInt  ||  ncol > WildInt  ||
        nrow < -WildInt  ||  nrow > WildInt  ||
        x1 < -WildFloat  ||  x1 > WildFloat  ||
        y1 < -WildFloat  ||  y1 > WildFloat  ||
        x2 < -WildFloat  ||  x2 > WildFloat  ||
        y2 < -WildFloat  ||  y2 > WildFloat  ||
        scale < -WildFloat  ||  scale > WildFloat) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (!rawgridinput) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (ncol < MIN_COLS_CON_CALC  ||  nrow < MIN_ROWS_CON_CALC) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    if (ncol * nrow > MAX_NODES) {
        grd_utils_ptr->grd_set_err (7);
        return -1;
    }

    if (contours == NULL  ||  ncont == NULL) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

    if (x1 >= x2  ||  y1 >= y2) {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

    if (options ) {
        ContourNullValue = options->null_value;
    }
    else {
        ContourNullValue = OptContourNullValue;
    }

/*
    Count the number of null grid nodes.  The ratio of null to
    total is used later to help decide if contour smoothing is
    appropriate for this particular contour set.
*/
    num_null = 0;
    for (i=0; i<ncol*nrow; i++) {
        if (ContourNullValue > 1.e12  &&
            rawgridinput[i] > ContourNullValue) {
            num_null++;
        }
        else if (ContourNullValue < -1.e12  &&
                 rawgridinput[i] < ContourNullValue) {
            num_null++;
        }
    }
    null_ratio = (CSW_F)(ncol * nrow - num_null) / (CSW_F)(ncol*nrow);
    null_ratio *= null_ratio;
    null_ratio *= null_ratio;
    smooth_size2 = SMOOTH_SIZE2 * null_ratio;

/*
    Remove the null nodes on the border of the grid and
    only use the non null interior for contouring.
*/
    RemoveNullBorder (rawgridinput,
                      &ncol, &nrow,
                      &x1, &y1, &x2, &y2,
                      ContourNullValue,
                      &rawgridin);

    RangeCheckNeeded = 1;
    istat = CheckGridRange (rawgridin, ncol*nrow, &i);
    if (istat == 0) {
        grd_utils_ptr->grd_set_err (10);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return success (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        b_success = true;
        return 1;
    }

/*
    Allocate a copy of the raw grid so that it can be adjusted
    in various ways and not change the raw input grid.
*/
    gridin = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (!gridin) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    memcpy (gridin, rawgridin, ncol*nrow*sizeof(CSW_F));

    if (rawgridin != rawgridinput) {
      csw_Free (rawgridin);
      rawgridin = NULL;
    }

/*
 * Reset the base and top in case they were modified in a previous
 * ShiftInputGrid call.
 */
    BaseGridValue = -1.e30f;
    TopGridValue = 1.e30f;

/*
    Use global options or options from the options structure.
*/
    if (options ) {
        ContourSmoothing = options->smoothing;
        ContourResampleFlag = options->resample_flag;
        ContourThicknessFlag = options->thickness_flag;
        ContourBaseValue = options->base_value;
        ContourLogBase = options->log_base;
        ContourNullValue = options->null_value;
        ContourMinorCrowd = options->minor_crowd;
        ContourMajorCrowd = options->major_crowd;
        ContourFastFlag = options->fast_flag;
        ContourLogConvert = options->convert_to_log;
        FillPrecision = options->fill_precision;
        ContourInterval = options->contour_interval;
        FirstContour = options->first_contour;
        LastContour = options->last_contour;
        BaseGridValue = options->base_grid_value;
        TopGridValue = options->top_grid_value;
        MajorSpacing = options->major_spacing;
        StepGridFlag = options->step_flag;
        FaultedFlag = options->faulted_flag;
        ContourInFaultsFlag = options->contour_in_faults_flag;
        NumMinor = options->nminor;
        NumMajor = options->nmajor;
        if (NumMajor > 0) memcpy ((char *)MajorContours,
                                (char *)options->major_contours,
                                NumMajor * sizeof(CSW_F));
        if (NumMinor > 0) memcpy ((char *)MinorContours,
                                (char *)options->minor_contours,
                                NumMinor * sizeof(CSW_F));
    }

    else {
        ContourSmoothing = OptContourSmoothing;
        ContourResampleFlag = OptContourResampleFlag;
        ContourThicknessFlag = OptContourThicknessFlag;
        ContourBaseValue = OptContourBaseValue;
        ContourLogBase = OptContourLogBase;
        ContourNullValue = OptContourNullValue;
        ContourMinorCrowd = OptContourMinorCrowd;
        ContourMajorCrowd = OptContourMajorCrowd;
        ContourFastFlag = OptContourFastFlag;
        ContourLogConvert = OptContourLogConvert;
        FillPrecision = OptFillPrecision;
        StepGridFlag = OptStepGridFlag;
        FaultedFlag = OptFaultedFlag;
        ContourInFaultsFlag = OptContourInFaultsFlag;
        BaseGridValue = -1.e30f;
        TopGridValue = 1.e30f;
    }

/*
    Make sure options are reasonable.
*/
    if (ContourInterval > 1.e20) ContourInterval = -1.0f;
    if (FirstContour < -1.e20  ||  FirstContour > 1.e20) {
        FirstContour = 1.e20f;
    }
    if (LastContour < -1.e20  ||  LastContour > 1.e20) {
        LastContour = -1.e20f;
    }
    if (BaseGridValue > 1.e20) {
        BaseGridValue = -1.e30f;
    }
    if (TopGridValue < -1.e20) {
        TopGridValue = 1.e30f;
    }
    if (BaseGridValue >= TopGridValue) {
        BaseGridValue = -1.e30f;
        TopGridValue = 1.e30f;
    }

    if (ContourSmoothing <= 0) ContourSmoothing = 0;
    if (ContourSmoothing > 9) ContourSmoothing = 9;
    if (ContourBaseValue < 0.0f) ContourBaseValue = 0.0f;
    if (ContourLogBase <= 1.0f) ContourLogBase = 0.0f;
    if (ContourMinorCrowd < 0.0f) ContourMinorCrowd = 0.0f;
    if (ContourMajorCrowd < 0.0f) ContourMajorCrowd = 0.0f;
    if (ContourFastFlag != 1) ContourFastFlag = 0;
    if (FillPrecision < 0.001f) FillPrecision = 0.001f;
    if (ContourThicknessFlag > 2) ContourThicknessFlag = 0;
    if (ContourThicknessFlag < 0) ContourThicknessFlag = 0;
    if (StepGridFlag) FaultedFlag = 0;
    if (ContourNullValue < 0.0  &&  ContourNullValue > -1.e10) {
        ContourNullValue = -1.e28f;
    }
    if (ContourNullValue >= 0.0  &&  ContourNullValue < 1.e10) {
        ContourNullValue = 1.e28f;
    }

/*
 * Bug 9761
 *
 * If the ContourThicknessFlag is non zero, and logarithmic contours
 * are desired, the log contours request trumps the thickness request and
 * ContourThicknessflag is set to zero.
 */
    if (ContourLogBase > 0.0) {
        ContourThicknessFlag = 0;
    }

/*
    As of release 2.0.1, no contour across faults is provided.
*/
    ContourInFaultsFlag = 0;

/*
    If the faulted flag is true, and the step grid flag is false,
    there must be at least 4 columns and 4 rows.  If this is not
    the case, resample the grid to get 4 columns and 4 rows.
*/
    if (FaultedFlag  &&  !StepGridFlag) {
        if (ncol < 4  ||  nrow < 4) {
            istat = grd_fault_ptr->grd_build_fault_indices (gridin, ncol, nrow,
                                             x1, y1, x2, y2);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            istat = grd_fault_ptr->con_correct_narrow_grid (&narrowgrid, &narrowncol, &narrownrow);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            ncol = narrowncol;
            nrow = narrownrow;
            csw_Free (gridin);
            gridin = narrowgrid;
        }
    }

/*
    Find the z range of the input grid.  If it is very small relative
    to the actual values of the grid, then shift the z values so that
    the range is a larger percentage of the absolute values.  This
    prevents some precision related problems that can occur during the
    contour calculations.
*/
    GridShift = 0.0f;
    if (!StepGridFlag  &&  ContourLogBase <= 1.0f) {
        ShiftInputGrid (gridin, ncol, nrow);
    }

    if (StepGridFlag) {
        ContourSmoothing = 0;
        if (ContourInterval > 0.0) ContourBaseValue = ContourInterval / 2.0f;
        ContourLogBase = 0.0f;
        ContourMinorCrowd = 0.0f;
        ContourMajorCrowd = 0.0f;
        ContourFastFlag = 0;
        FaultedFlag = 0;
    }

    if (ContourFastFlag == 1) {
        if (FillPrecision < 0.1f) {
            FillPrecision = 0.1f;
        }
        ContourMinorCrowd = 0.0f;
        ContourMajorCrowd = 0.0f;
        ContourSmoothing = 0;
    }

/*
    Set private class variables from the input parameters.
*/
    Ncol = ncol;
    Nrow = nrow;
    Xmin = x1;
    Ymin = y1;
    Xmax = x2;
    Ymax = y2;
    Scale = scale;
    if (Scale <= 0.0f) Scale = -1.0f;

/*
    Convert to log grid if needed.
*/
    if (ContourLogBase > 1.0f) {
        if (ContourInterval > 0.0f  ||  (NumMajor <= 0  &&  NumMinor <= 0)) {
            DoLogContours = 1;
            istat = ConvertToLogBase (gridin);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (8);
                return -1;
            }
        }
    }

/*
    Find the limits of the grid before I can set up for smoothing
    or thickness contouring.
*/
    ThicknessFlagIsSet = 1;
    FindHistoGridLimits (gridin, Ncol*Nrow);
    ThicknessFlagIsSet = 0;
    tiny = (Zmax - Zmin);
    if (tiny < Z_ABSOLUTE_TINY) {
        tiny = Z_ABSOLUTE_TINY;
    }
    if (BaseGridValue < -1.e20) {
        BaseGridValue = Zmin - tiny;
    }
    if (TopGridValue > 1.e20) {
        TopGridValue = Zmax + tiny;
    }

/*
    If the ContourThicknessFlag is set, that is the same as setting
    either the BaseGridValue or the TopGridValue to zero.
*/
    tgrid = NULL;
    if (ContourThicknessFlag == CON_POSITIVE_THICKNESS) {
        if (Zmax <= 0.0) {
            grd_utils_ptr->grd_set_err (13);
            return -1;
        }
        if (BaseGridValue < 0.0f) {
            BaseGridValue = 0.0f;
        }
    }
    if (ContourThicknessFlag == CON_NEGATIVE_THICKNESS) {
        if (Zmin >= 0.0) {
            grd_utils_ptr->grd_set_err (13);
            return -1;
        }
        if (TopGridValue > 0.0f) {
            TopGridValue = 0.0f;
        }
    }

    top_adjust = 0.0f;
    if (BaseGridValue > -1.e20  ||  TopGridValue < 1.e20) {
        tiny = (Zmax - Zmin) / Z_TINY_DIVISOR;
        if (tiny < Z_ABSOLUTE_TINY) {
          tiny = Z_ABSOLUTE_TINY;
        }
        cnull = ContourNullValue / 10.0f;
        tgrid = (CSW_F *)csw_Malloc (Ncol * Nrow * sizeof(CSW_F));
        if (!tgrid) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

        memcpy (tgrid, gridin, Ncol * Nrow * sizeof(CSW_F));

        if (TopGridValue < 1.e20f  &&  TopGridValue <= Zmax) {

    /*
     *  bug 8122 fix.  The top grid value to be clipped to is made slightly
     *  lower, allowing better gradient extrapolation if the grid is almost
     *  flat near the plateau edge.  The fudge factor is 1 part in 1000 of
     *  the grid range.
     *
     *  QWER   This can be changed to reflect the data set in the future.
     *         Using histograms to determine an appropriate plateau edge
     *         value might be a future enhancement.
     */
            top_adjust = tiny * 50.0f;
            istat = grd_utils_ptr->grd_fill_plateau (tgrid, Ncol, Nrow,
                                      TopGridValue - top_adjust, cnull, Zmax - Zmin);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }

        if (BaseGridValue > -1.e20f  &&  BaseGridValue >= Zmin) {
            istat = grd_utils_ptr->grd_fill_valley (tgrid, Ncol, Nrow,
                                     BaseGridValue, cnull, Zmax - Zmin);
            if (istat == -1) {
                FreeMem ();
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }

        if (FirstContour >= LastContour) {
            if (BaseGridValue > -1.e20) FirstContour = BaseGridValue;
            if (TopGridValue < 1.e20) LastContour = TopGridValue;
        }
        csw_Free (gridin);
        gridin = tgrid;
        tgrid = NULL;
    }

#if DEBUG_WRITE_GRID_FILE
    grd_write_file ("debug_rs_0.grd", NULL,
                    gridin, NULL, NULL,
                    ncol, nrow,
                    (double)x1, (double)y1, (double)x2, (double)y2,
                    GRD_NORMAL_GRID_FILE, NULL, 0);
#endif

/*
    If contour smoothing is desired, and there are a small number
    of grid nodes, resample the grid to make saddle points better
    behaved.
*/
    grid = NULL;

    if (FaultedFlag) {
        grd_fault_ptr->grd_set_fault_option (CON_CONTOUR_IN_FAULTS,
                              ContourInFaultsFlag, 0.0f, NULL);
    }

    istat = con_resample_for_smoothing (gridin, NULL, ncol, nrow,
                                        x1, y1, x2, y2,
                                        &grid, NULL, &ncnew, &nrnew);
    if (istat == -1) {
        return -1;
    }

    if (istat == 1) {
        Ncol = ncnew;
        Nrow = nrnew;
        csw_Free (gridin);
        gridin = NULL;
    }
    else {
        grid = gridin;
        Ncol = ncol;
        Nrow = nrow;
    }

    if (istat == 1) {
        ThicknessFlagIsSet = 1;
        FindHistoGridLimits (grid, Ncol*Nrow);
        ThicknessFlagIsSet = 0;
    }
    if (BaseGridValue < -1.e20) {
        BaseGridValue = Zmin;
    }
    if (TopGridValue > 1.e20) {
        TopGridValue = Zmax;
    }

    maxcol = Ncol / 4;
    if (maxcol < 100) maxcol = 100;
    maxrow = Nrow / 4;
    if (maxrow < 100) maxrow = 100;

    Xspace = (x2 - x1) / (Ncol - 1);
    Yspace = (y2 - y1) / (Nrow - 1);

/*
    Initialize the contour output stuff to NULL and zero
    in case of an error.
*/
    *contours = NULL;
    *ncont = 0;
    ContourData = NULL;
    MaxConData = 0;
    NconData = 0;
    DoLogContours = 0;

    firstsav = FirstContour;
    lastsav = LastContour;

/*
    Copy the grid into allocated space so it can be adjusted.
    Also keep a copy in the OriginalGrid array.  These values
    will not be adjusted to keep nodes from being exactly on
    a contour level.  The original non adjusted nodes are
    needed to precisely calculate saddle point elevations.
*/
MSL
    Grid = (CSW_F *)csw_Malloc (2 * Ncol * Nrow * sizeof(CSW_F));
    if (!Grid) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    memcpy (Grid, grid, Ncol*Nrow*sizeof(CSW_F));

    OriginalGrid = Grid + Ncol * Nrow;
    memcpy (OriginalGrid, grid, Ncol*Nrow*sizeof(CSW_F));

    NoNullGrid = (CSW_F *)csw_Malloc (Ncol * Nrow * sizeof(CSW_F));
    if (!NoNullGrid) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    memcpy (NoNullGrid, grid, Ncol*Nrow*sizeof(CSW_F));

/*
    If the grid null value is less than zero,
    convert all nulls to positives.
*/
    if (ContourNullValue < 0.0f) {
        SetToPositiveNulls ();
    }

/*
 * Keep a copy of the grid with hard null values filled
 * in to use for smoothing near null values.
 */
    grd_utils_ptr->grd_fill_nulls_new (NoNullGrid, Ncol, Nrow, ContourNullValue,
                        NULL, NULL, 0);

/*
    Allocate workspace memory
*/
MSL
    BottomSide = (int *)csw_Malloc ((Ncol * 2 + Nrow * 2) * sizeof(int));
    if (!BottomSide) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    TopSide = BottomSide + Ncol;
    LeftSide = TopSide + Ncol;
    RightSide = LeftSide + Nrow;

MSL
    Hcrossing = (MYSIGNED char *)csw_Malloc (Ncol * Nrow * 4 * sizeof(char));
    if (!Hcrossing) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    Vcrossing = Hcrossing + Ncol * Nrow;
    Hcrossing2 = Vcrossing + Ncol * Nrow;
    Vcrossing2 = Hcrossing2 + Ncol * Nrow;

MSL
    Xbuf1 = (CSW_F *)csw_Malloc (LINE_BUFFER_CHUNK * sizeof(CSW_F));
    if (!Xbuf1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
MSL
    Ybuf1 = (CSW_F *)csw_Malloc (LINE_BUFFER_CHUNK * sizeof(CSW_F));
    if (!Ybuf1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
MSL
    CellBuf = (int *)csw_Malloc (LINE_BUFFER_CHUNK * sizeof(int));
    if (!CellBuf) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
MSL
    CrowdBuf = (char *)csw_Malloc (LINE_BUFFER_CHUNK * sizeof(char));
    if (!CrowdBuf) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
MSL
    CrowdWork = (char *)csw_Malloc (LINE_BUFFER_CHUNK * sizeof(char));
    if (!CrowdWork) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
MSL
    Dibuf1 = (char *)csw_Malloc (LINE_BUFFER_CHUNK * sizeof(char));
    if (!Dibuf1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
MSL
    Xbuf2 = (CSW_F *)csw_Malloc (2 * LINE_BUFFER_CHUNK * sizeof(CSW_F));
    if (!Xbuf2) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
MSL
    Ybuf2 = (CSW_F *)csw_Malloc (2 * LINE_BUFFER_CHUNK * sizeof(CSW_F));
    if (!Ybuf2) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    Nbuf1 = 0;
    Nbuf2 = 0;
    MaxBuf1 = LINE_BUFFER_CHUNK;
    MaxBuf2 = 2 * LINE_BUFFER_CHUNK;

    cenv = csw_getenv ("CON_SMALL_BUFFERS");
    if (cenv) {
        MaxBuf1 = 100;
        MaxBuf2 = 100;
    }

/*
    Allocate initial space for contour output.
*/
MSL
    ContourData = (COntourOutputRec *)
                   csw_Calloc (LINE_BUFFER_CHUNK * sizeof(COntourOutputRec));
    if (!ContourData) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    NconData = 0;
    MaxConData = LINE_BUFFER_CHUNK;

/*
    If contour smoothing is desired and either the x or y spacing
    is large enough so smoothing makes sense, allocate an array
    ofCSW_F pointers that will hold subgrids used for smoothing.
    Also allocate and setup a grid of flags indicating whether
    cells need smoothing or not.
*/
    FaultSmoothing = ContourSmoothing;
    if (FaultedFlag) ContourSmoothing = 0;

    DoSmoothing = ContourSmoothing;
    if (ContourSmoothing  &&  ContourFastFlag == 0) {

        SmoothMargin = ContourSmoothing / 2 + 1;
        if (SmoothMargin > SMOOTH_MARGIN) {
            SmoothMargin = SMOOTH_MARGIN;
        }

        if (Xspace / Scale < smooth_size2  &&  Yspace / Scale < smooth_size2) {
            DoSmoothing = 0;
            if (Scale <= 0.0) {
                DoSmoothing = 1;
MSL
                SmoothSubgrids = (CSW_F **)csw_Calloc (Ncol * Nrow * sizeof(CSW_F *));
                if (!SmoothSubgrids) {
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
MSL
                SmoothFlags = (MYSIGNED char *)csw_Calloc (Ncol * Nrow * sizeof(char));
                if (!SmoothFlags) {
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
                SubgridCols = 4;
                SubgridRows = 4;
                SetSmoothFlags ();
            }
        }

        else {
MSL
            SmoothSubgrids = (CSW_F **)csw_Calloc (Ncol * Nrow * sizeof(CSW_F *));
            if (!SmoothSubgrids) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
MSL
            SmoothFlags = (MYSIGNED char *)csw_Calloc (Ncol * Nrow * sizeof(char));
            if (!SmoothFlags) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            SubgridRows = (int) (Yspace / Scale / SMOOTH_SIZE + 1.5f);
            SubgridCols = (int) (Xspace / Scale / SMOOTH_SIZE + 1.5f);
            if (SubgridRows < 3) SubgridRows = 3;
            if (SubgridCols < 3) SubgridCols = 3;
            if (SubgridRows > 9) SubgridRows = 9;
            if (SubgridCols > 9) SubgridCols = 9;
            SetSmoothFlags ();
            DoSmoothing = 1;
        }
    }

    if (ContourFastFlag == 1) {
        DoSmoothing = 0;
    }

/*
    Generate a list of the contour levels.
*/
    csav = ContourInterval;

    istat = BuildContourArrays ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (9);
        return -1;
    }

/*
    If crowded contours are to be suppressed, calculate
    the vertical and horizontal maximum grid side elevation
    changes that will actually have contours drawn.
*/
    if (Scale > 0.0f  &&  ContourInterval > 0.0f) {
        crowd1 = ContourMinorCrowd * Scale;
        if (crowd1 <= 0.0f) {
            MinorCrowd = 0;
        }
        else {
            zt1 = Yspace / crowd1 * ContourInterval;
            zt2 = Xspace / crowd1 * ContourInterval;
            MinorGradient = (CSW_F)sqrt ((double)(zt1*zt1 + zt2*zt2));
            MinorCrowd = 1;
        }
        crowd2 = ContourMajorCrowd * Scale;
        if (crowd2 <= 0.0f  ||  MajorSpacing < 1) {
            MajorCrowd = 0;
        }
        else {
            zt1 = Yspace / crowd2 * ContourInterval;
            zt2 = Xspace / crowd2 * ContourInterval;
            MajorGradient = (CSW_F)sqrt ((double)(zt1*zt1 + zt2*zt2));
            MajorCrowd = 1;
        }
    }

    else {
        MinorCrowd = 0;
        MajorCrowd = 0;
    }

/*
    If the faulted contour flag is set, get the pointer to
    the array specifying which grid cells have faults in them.
    If this pointer is NULL, then turn off the faulted contouring
    flag.
*/
    if (FaultedFlag == 1) {
        istat = grd_fault_ptr->grd_build_fault_indices (Grid, Ncol, Nrow,
                                         Xmin, Ymin, Xmax, Ymax);
        if (istat == -1) {
            return -1;
        }
        grd_fault_ptr->con_faulted_smoothing (FaultSmoothing);
        grd_fault_ptr->con_get_fault_cell_crossings (&FaultCellCrossings,
                                      &FaultColumnCrossings,
                                      &FaultRowCrossings,
                                      &ClosestFault,
                                      &FaultNodeGraze);
        if (FaultCellCrossings == NULL  ||
            FaultColumnCrossings == NULL  ||
            FaultRowCrossings == NULL  ||
            FaultNodeGraze == NULL) {
            FaultedFlag = 0;
            FaultCellCrossings = NULL;
            FaultColumnCrossings = NULL;
            FaultRowCrossings = NULL;
            ClosestFault = NULL;
            FaultNodeGraze = NULL;
        }
    }

    if (ContourInFaultsFlag == 0  &&  FaultedFlag == 1) {
        istat = grd_fault_ptr->con_null_inside_faults ();
        if (istat == -1) {
            return -1;
        }
    }

/*
    Trace contours through the grid and output them
    in the ContourData array.
*/
    if (FaultedFlag) {
        grd_fault_ptr->grd_set_fault_option (CON_CONTOUR_IN_FAULTS,
                              ContourInFaultsFlag, 0.0f, NULL);
    }

    istat = TraceContours ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Free work memory, set output and return.
*/
    *contours = ContourData;
    *ncont = NconData;
    ContourInterval = csav;
    FirstContour = firstsav;
    LastContour = lastsav;

    b_success = true;

    return 1;

}  /*  end of function con_calc_contours  */





/*
  ****************************************************************

              c o n _ f r e e _ c o n t o u r s

  ****************************************************************

    Free a contour output list and the coordinates associated with
  each contour segment in the list.

*/

int CSWConCalc::con_free_contours (COntourOutputRec *list, int nlist)
{
    int                   i;
    COntourOutputRec      *ptr;

    if (list == NULL) return 1;

    for (i=0; i<nlist; i++) {
        ptr = list + i;
        if (ptr->x) csw_Free (ptr->x);
    }

    csw_Free (list);

    return 1;

}  /*  end of function con_free_contours  */



/*
 ****************************************************************

 c o n _ c a l c _ d e f a u l t _ c o n t o u r _ i n t e r v a l

 ****************************************************************

  Given a list of z values, return a reasonable contour interval,
  first contour, last contour and major spacing.  This is only called
  from the trimesh contouring functions.  Do not use this function
  any other way.

*/

int CSWConCalc::con_calc_default_contour_interval
                          (CSW_F *zlist, int nlist,
                           CSW_F *conint,
                           CSW_F *conmin, CSW_F *conmax,
                           int *major)
{
    int              istat;
    CSW_F            base_sav, top_sav, tiny;

/*
 * Bug 8841
 *
 *  The base and top grid values must be set prior
 *  to calling BuildContourArrays.  If not set, the
 *  values left over from the previous contour calculation
 *  will be used erroneously.
 *
 *  To isolate the fix, I save the values, change them temporarily
 *  and then revert back to the saved values.
 */
    base_sav = BaseGridValue;
    top_sav = TopGridValue;

    FindHistoGridLimits (zlist, nlist);

    tiny = Zmax - Zmin;
    if (tiny <= 0.0) {
        return -1;
    }
    BaseGridValue = Zmin - tiny;
    TopGridValue = Zmax + tiny;

    ContourInterval = -1.0f;
    FirstContour = 0.0f;
    LastContour = -1.0f;
    NumMinor = 0;
    NumMajor = 0;

    istat = BuildContourArrays ();
    BaseGridValue = base_sav;
    TopGridValue = top_sav;
    if (istat == -1) {
        return -1;
    }

    if (conint != NULL) *conint = ContourInterval;
    if (conmin != NULL) *conmin = FirstContour;
    if (conmax != NULL) *conmax = LastContour;
    if (major != NULL) *major = MajorSpacing;

    return 1;

}  /* end of function con_calc_default_contour_interval */




/*
  ****************************************************************

                B u i l d C o n t o u r A r r a y s

  ****************************************************************

    Given the contour interval, specific contours and first and
  last contours, build the array of elevations to contour.  The
  private Cvals array of contour value structures is filled.

*/

int CSWConCalc::BuildContourArrays (void)
{
    int               i, itmp, n;
    double            zt, zt2, zt3, zt4, tiny, tiny2;

    if (HistoZmin < BaseGridValue) {
        HistoZmin = BaseGridValue;
    }
    if (HistoZmax > TopGridValue) {
        HistoZmax = TopGridValue;
    }

/*
    Adjust first and last if needed
*/
    if (FirstContour > LastContour) {
        FirstContour = HistoZmin;
        LastContour = HistoZmax;
    }
    if (FirstContour < Zmin) FirstContour = Zmin;
    if (LastContour > Zmax) LastContour = Zmax;
    if (LastContour < FirstContour) LastContour = Zmax;

    tiny = (LastContour - FirstContour) / 5000000.f;

/*
    Calculate interval if needed
*/
    DefaultNumberOfContours = 20.0f;
    if (FaultedFlag != 0) {
        DefaultNumberOfContours = 30.0f;
    }
    if (ContourInterval <= 0.0f  &&  NumMinor + NumMajor < 1) {
        zt = (LastContour - FirstContour) / DefaultNumberOfContours;
        if (zt > 0.0f) {
            zt2 = log10 ((double)zt);
            zt2 = floor ((double)zt2);
            zt3 = zt2;
            if (zt3 < 0.0f) zt3 = -zt3;
            zt4 = pow ((double)10.0f, (double)zt3);
            if (zt2 < 0.0f) {
                zt4 = 1.0f / zt4;
            }
            ContourInterval = (CSW_F)zt4;
            MajorSpacing = 5;
            if (ContourInterval < zt/8.0f) {
                ContourInterval *= 10.0f;
                MajorSpacing = 5;
            }
            else if (ContourInterval < zt/4.5f) {
                ContourInterval *= 5.0f;
                MajorSpacing = 4;
            }
            else if (ContourInterval < zt/3.5f) {
                ContourInterval *= 4.0f;
                MajorSpacing = 5;
            }
            else if (ContourInterval < zt/1.5f) {
                ContourInterval *= 2.0f;
                MajorSpacing = 5;
            }
        /*
            Special cases for some commonly used logarithmic contours.
        */
            if (ContourLogBase > 9.9  &&  ContourLogBase < 10.1) {
                ContourInterval = 0.2f;
                MajorSpacing = 5;
            }
            else if (ContourLogBase > 1.9  &&  ContourLogBase < 2.1) {
                ContourInterval = 0.5f;
                MajorSpacing = 2;
            }
        }
        else {
            ContourInterval = FirstContour;
            BaseValueForContour = 0.0f;
            MajorSpacing = 1;
        }
    }

/*
    Return an error if too many contour
    levels would be generated.
*/
    if (ContourInterval > 0.0f) {
        itmp = (int) ((LastContour - FirstContour) / ContourInterval + 3.0f);
        if (itmp > 2 * MAX_CONTOUR_LEVELS) return -1;
    }

/*
    Load array for contour intervals.
*/
    tiny2 = tiny;
    if (ContourInterval > 0.0f) {

        tiny2 = ContourInterval / 100.0;
        BaseValueForContour = ContourInterval - ContourBaseValue;
        if (ContourBaseValue == 0.0f) BaseValueForContour = 0.0f;
    /*
        Make sure the first contour is an integral
        multiple of the contour interval.
    */
        itmp = (int) (FirstContour / ContourInterval);
        zt = itmp * ContourInterval;
        if (zt < FirstContour) {
            zt += ContourInterval;
        }
        FirstContour = (CSW_F)zt;

    /*
        Adjust first contour for the contour base.
    */
        if (ContourBaseValue >= ContourInterval) {
            BaseValueForContour = 0.0f;
        }
        FirstContour -= BaseValueForContour;
        if (FirstContour < Zmin) FirstContour += ContourInterval;

    /*
        Load up the array from first to last contour.
    */
        n = 0;
        zt = FirstContour;
        for (;;) {
            if (zt < tiny  &&  zt > -tiny) zt = 0.0f;
            Cvals[n].z = (CSW_F)zt;
            Cvals[n].major = 0;
            zt2 = zt;
            if (zt2 < 0.0f) zt2 = -zt2;
            itmp = (int) (zt2 / ContourInterval + 0.01f);
            if (MajorSpacing > 0) {
                if (itmp % MajorSpacing == 0) {
                    Cvals[n].major = 1;
                }
            }
            n++;
            zt += ContourInterval;
            if (zt > LastContour + tiny2) break;
        }

    }

/*
    Transfer specified contours to Cvals array
*/
    else {

    /*
     * Bug 8297
     *
     * If contour levels were explicitly specified,
     * they need to be shifted also.
     */
        n = 0;
        for (i=0; i<NumMinor; i++) {
            Cvals[n].z = MinorContours[i] - (CSW_F)GridShift;
            Cvals[n].major = 0;
            n++;
        }
        for (i=0; i<NumMajor; i++) {
            Cvals[n].z = MajorContours[i] - (CSW_F)GridShift;
            Cvals[n].major = 1;
            n++;
        }

    }

  /*
   * bug 8383,  If a contour level is very near zero, make it
   *            exactly equal to zero.
   */
    for (i=0; i<n; i++) {
        if (Cvals[i].z > -Z_ABSOLUTE_TINY  &&
            Cvals[i].z < Z_ABSOLUTE_TINY) {
            Cvals[i].z = 0.0;
        }
    }

    Ncvals = n;

    return 1;

}  /*  end of BuildContourArrays function  */





/*
  ****************************************************************

                         F r e e M e m

  ****************************************************************

    Free Workspace memory and reset the pointers to NULL.

*/

int CSWConCalc::FreeMem (void)
{
    int           i;

    if (Grid) csw_Free (Grid);
    if (NoNullGrid) csw_Free (NoNullGrid);
    if (BottomSide) csw_Free (BottomSide);
    if (Hcrossing) csw_Free (Hcrossing);
    if (Xbuf1) csw_Free (Xbuf1);
    if (Ybuf1) csw_Free (Ybuf1);
    if (CellBuf) csw_Free (CellBuf);
    if (CrowdBuf) csw_Free (CrowdBuf);
    if (CrowdWork) csw_Free (CrowdWork);
    if (Dibuf1) csw_Free (Dibuf1);
    if (Xbuf2) csw_Free (Xbuf2);
    if (Ybuf2) csw_Free (Ybuf2);
    if (SmoothFlags) csw_Free (SmoothFlags);

    if (SmoothSubgrids) {
        for (i=0; i<Ncol*Nrow; i++) {
            if (SmoothSubgrids[i]) csw_Free (SmoothSubgrids[i]);
        }
        csw_Free (SmoothSubgrids);
    }

    Grid = NULL;
    NoNullGrid = NULL;
    OriginalGrid = NULL;

    BottomSide = NULL;
    TopSide = NULL;
    LeftSide = NULL;
    RightSide = NULL;

    Hcrossing = NULL;
    Vcrossing = NULL;
    Hcrossing2 = NULL;
    Vcrossing2 = NULL;

    Xbuf1 = NULL;
    Ybuf1 = NULL;
    CellBuf = NULL;
    CrowdBuf = NULL;
    CrowdWork = NULL;
    Xbuf2 = NULL;
    Ybuf2 = NULL;
    Dibuf1 = NULL;

    SmoothSubgrids = NULL;
    SmoothFlags = NULL;

    if (FaultedFlag == 1) {
        grd_fault_ptr->grd_free_faults ();
    }

    return 1;

}  /*  end of private FreeMem function  */







/*
  ****************************************************************

                     T r a c e C o n t o u r s

  ****************************************************************

    Loop through each contour level and trace all of each level's
  contours through the grid.


*/

int CSWConCalc::TraceContours (void)
{
    int             istat, i, j, k, offset, ilev;
    int             edgelevel, topedge, bottomedge, leftedge, rightedge;
    CSW_F           zlev, z1, z2, dz, ztiny;

/*
 *  If a side of the grid has a very tiny gradient, do not start
 *  a contour there.  This will suppress ugly contours generated
 *  over flat spots in the grid.
 */
    ztiny = (HistoZmax - HistoZmin) / 1000000.0f;

/*
    Set up the bottom, top, left and right arrays, which
    have the index of the first non null cell from the
    respective edge of the grid.  If there are no good
    grid cells in a respective column or row, then the
    side flag corresponding to the intersection of the
    column or row with the edge of the grid is set to -1.
*/

/*
    Bottom side
*/
    bottomedge = 0;
    for (j=0; j<Ncol-1; j++) {

        BottomSide[j] = -1;

        for (i=0; i<Nrow; i++) {

            k = i * Ncol + j;
            if (Grid[k] >= ContourNullValue  ||
                Grid[k+1] >= ContourNullValue) {
                continue;
            }
            BottomSide[j] = i;
            if (i > bottomedge) bottomedge = i;
            break;
        }

    }

/*
    Top Side
*/
    topedge = 100000000;
    for (j=0; j<Ncol-1; j++) {

        TopSide[j] = -1;

        for (i=Nrow-1; i>=0; i--) {

            k = i * Ncol + j;
            if (Grid[k] >= ContourNullValue  ||
                Grid[k+1] >= ContourNullValue) {
                continue;
            }
            TopSide[j] = i - 1;
            if (i-1 < topedge) topedge = i - 1;
            break;
        }

    }

/*
    Left side
*/
    leftedge = 0;
    for (i=0; i<Nrow-1; i++) {

        LeftSide[i] = -1;

        for (j=0; j<Ncol; j++) {

            k = i * Ncol + j;
            if (Grid[k] >= ContourNullValue  ||
                Grid[k+Ncol] >= ContourNullValue) {
                continue;
            }
            LeftSide[i] = j;
            if (j > leftedge) leftedge = j;
            break;
        }

    }

/*
    Right Side
*/
    rightedge = 100000000;
    for (i=0; i<Nrow-1; i++) {

        RightSide[i] = -1;

        for (j=Ncol-1; j>=0; j--) {

            k = i * Ncol + j;
            if (Grid[k] >= ContourNullValue  ||
                Grid[k+Ncol] >= ContourNullValue) {
                continue;
            }
            RightSide[i] = j - 1;
            if (j-1 < rightedge) rightedge = j - 1;
            break;
        }

    }

/*
    If there is a constant contour interval, adjust grid nodes
    a little if they are almost exactly on a contour interval.
*/
    if (ContourInterval > 0.0f) {
        AdjustGridForContourInterval ();
    }

/*
 * Added to help debug problems with contour calculation.
 * See bug 8052 for an example.
#if DEBUG_WRITE_GRID_FILE
    grd_write_file ("debug.grd",
                   NULL,
                   Grid,
                   NULL,
                   NULL,
                   Ncol, Nrow,
                   Xmin, Ymin, Xmax, Ymax,
                   GRD_NORMAL_GRID_FILE,
                   NULL, 0);
#endif
 */

/*
    Loop through the contour levels.  At each level check for
    contours intersecting each side of the grid and the center
    cells in the grid.  Trace the contours and output them.
*/
    for (ilev=0; ilev<Ncvals; ilev++) {

        zlev = Cvals[ilev].z;
        OutputZlev = zlev;
        grd_fault_ptr->grd_set_faulted_contour_level (zlev);
        OutputMajor = Cvals[ilev].major;

    /*
        Duplicated contours have been set to 1.e30,
        so continue with next level if a duplicate
        is encountered.
    */
        if (zlev > 1.e20f) continue;

    /*
        Modify grid nodes by a tiny amount if they are
        almost exactly at the contour level.  This eliminates
        the case where a contour goes exactly through a
        corner of the grid cell.

        This is only done here if a specific list of contours
        has been specified, as opposed to a contour interval.
    */
        if (ContourInterval <= 0.0f) {
            AdjustGridForContourLevel (zlev);
        }

    /*
        Zero the Hcrossing and Vcrossing arrays.  These
        are used to keep track of which cells have already been
        traced through for a given contour level.
    */
        memset (Hcrossing, 0, Ncol*Nrow*sizeof(MYSIGNED char));
        memset (Vcrossing, 0, Ncol*Nrow*sizeof(MYSIGNED char));

        MiddleStart = 0;

    /*
        contours intersecting the bottom edge
    */
        for (edgelevel=0; edgelevel<=bottomedge; edgelevel++) {
            for (j=0; j<Ncol-1; j++) {

                i = BottomSide[j];

                if (i < 0) {
                    continue;
                }

                if (i != edgelevel) {
                    continue;
                }

                offset = i * Ncol;
                if (Hcrossing[offset+j]) {
                    continue;
                }

            /*
                If a fault intersects this side of the cell, don't start
                a contour here.  The cell will eventually be entered from
                another side not having a fault, or if all 4 sides are faulted,
                no contours will go through the cell.
            */
                if (FaultRowCrossings) {
                    if (FaultRowCrossings[offset+j] == 1) {
                        continue;
                    }
                }
                if (FaultNodeGraze) {
                    if (FaultNodeGraze[offset+j] == 1  ||
                        FaultNodeGraze[offset+j+1] == 1) {
                        continue;
                    }
                }
                z1 = Grid[offset+j];
                z2 = Grid[offset+j+1];
                if (z1 >= ContourNullValue  ||  z2 >= ContourNullValue) {
                    continue;
                }

                if (StepGridFlag) {
                    if (StepGraze(z1,z2) == 1) continue;
                }
                else {
                    if ((zlev-z1)*(zlev-z2) >= 0.0f) continue;
                    dz = z1 - z2;
                    if (dz < 0.0) dz = -dz;
                    if (dz <= ztiny) continue;
                }

                istat = TraceCellToCell (i, j, 0, zlev);
                if (istat == -1) {
                    return -1;
                }
                if (DoSmoothing) {
                    istat = SmoothContour ();
                    if (istat == -1) {
                        return -1;
                    }
                }
MSL
                istat = OutputContours ();
                if (istat == -1) {
                    return -1;
                }
            }
        }

    /*
        contours intersecting the right edge
    */
        for (edgelevel=Ncol-1; edgelevel>=rightedge; edgelevel--) {
            for (i=0; i<Nrow-1; i++) {

                j = RightSide[i];

                if (j < 0) {
                    continue;
                }

                if (j != edgelevel) {
                    continue;
                }

                offset = i * Ncol;
                if (Vcrossing[offset+j+1]) {
                    continue;
                }
            /*
                If a fault intersects this side of the cell, don't start
                a contour here.  The cell will eventually be entered from
                another side not having a fault, or if all 4 sides are faulted,
                no contours will go through the cell.
            */
                if (FaultColumnCrossings) {
                    if (FaultColumnCrossings[offset+j+1] == 1) {
                        continue;
                    }
                }
                if (FaultNodeGraze) {
                    if (FaultNodeGraze[offset+j+1] == 1  ||
                        FaultNodeGraze[offset+j+Ncol+1] == 1) {
                        continue;
                    }
                }
                z1 = Grid[offset+j+1];
                z2 = Grid[offset+j+Ncol+1];
                if (z1 >= ContourNullValue  ||  z2 >= ContourNullValue) {
                    continue;
                }

                if (StepGridFlag) {
                    if (StepGraze(z1,z2) == 1) continue;
                }
                else {
                    if ((zlev-z1)*(zlev-z2) >= 0.0f) continue;
                    dz = z1 - z2;
                    if (dz < 0.0) dz = -dz;
                    if (dz <= ztiny) continue;
                }

                istat = TraceCellToCell (i, j, 1, zlev);
                if (istat == -1) {
                    return -1;
                }
                if (DoSmoothing) {
                    istat = SmoothContour ();
                    if (istat == -1) {
                        return -1;
                    }
                }
MSL
                istat = OutputContours ();
                if (istat == -1) {
                    return -1;
                }
            }
        }

    /*
        contours intersecting the top edge
    */
        for (edgelevel=Nrow-1; edgelevel>=topedge; edgelevel--) {
            for (j=0; j<Ncol-1; j++) {

                i = TopSide[j];

                if (i < 0) {
                    continue;
                }

                if (i != edgelevel) {
                    continue;
                }

                offset = i * Ncol + Ncol;
                if (Hcrossing[offset+j]) {
                    continue;
                }
            /*
                If a fault intersects this side of the cell, don't start
                a contour here.  The cell will eventually be entered from
                another side not having a fault, or if all 4 sides are faulted,
                no contours will go through the cell.
            */
                if (FaultRowCrossings) {
                    if (FaultRowCrossings[offset+j] == 1) {
                        continue;
                    }
                }
                if (FaultNodeGraze) {
                    if (FaultNodeGraze[offset+j] == 1  ||
                        FaultNodeGraze[offset+j+1] == 1) {
                        continue;
                    }
                }
                z1 = Grid[offset+j];
                z2 = Grid[offset+j+1];
                if (z1 >= ContourNullValue  ||  z2 >= ContourNullValue) {
                    continue;
                }

                if (StepGridFlag) {
                    if (StepGraze(z1,z2) == 1) continue;
                }
                else {
                    if ((zlev-z1)*(zlev-z2) >= 0.0f) continue;
                    dz = z1 - z2;
                    if (dz < 0.0) dz = -dz;
                    if (dz <= ztiny) continue;
                }

                istat = TraceCellToCell (i, j, 2, zlev);
                if (istat == -1) {
                    return -1;
                }
                if (DoSmoothing) {
                    istat = SmoothContour ();
                    if (istat == -1) {
                        return -1;
                    }
                }
MSL
                istat = OutputContours ();
                if (istat == -1) {
                    return -1;
                }
            }
        }

    /*
        contours intersecting the left edge
    */
        for (edgelevel=0; edgelevel<=leftedge; edgelevel++) {
            for (i=0; i<Nrow-1; i++) {

                j = LeftSide[i];

                if (j < 0) {
                    continue;
                }

                if (j != edgelevel) {
                    continue;
                }

                offset = i * Ncol;
                if (Vcrossing[offset+j]) {
                    continue;
                }
            /*
                If a fault intersects this side of the cell, don't start
                a contour here.  The cell will eventually be entered from
                another side not having a fault, or if all 4 sides are faulted,
                no contours will go through the cell.
            */
                if (FaultColumnCrossings) {
                    if (FaultColumnCrossings[offset+j] == 1) {
                        continue;
                    }
                }
                if (FaultNodeGraze) {
                    if (FaultNodeGraze[offset+j] == 1  ||
                        FaultNodeGraze[offset+j+Ncol] == 1) {
                        continue;
                    }
                }
                z1 = Grid[offset+j];
                z2 = Grid[offset+j+Ncol];
                if (z1 >= ContourNullValue  ||  z2 >= ContourNullValue) {
                    continue;
                }

                if (StepGridFlag) {
                    if (StepGraze(z1,z2) == 1) continue;
                }
                else {
                    if ((zlev-z1)*(zlev-z2) >= 0.0f) continue;
                    dz = z1 - z2;
                    if (dz < 0.0) dz = -dz;
                    if (dz <= ztiny) continue;
                }

                istat = TraceCellToCell (i, j, 3, zlev);
                if (istat == -1) {
                    return -1;
                }
                if (DoSmoothing) {
                    istat = SmoothContour ();
                    if (istat == -1) {
                        return -1;
                    }
                }
MSL
                istat = OutputContours ();
                if (istat == -1) {
                    return -1;
                }
            }
        }

        MiddleStart = 1;

    /*
        intersecting a vertical edge in the middle
    */
        for (i=0; i<Nrow-1; i++) {

            offset = i * Ncol;

            for (j=1; j<Ncol-1; j++) {

                if (Vcrossing[offset+j]) {
                    continue;
                }
            /*
                If a fault intersects this side of the cell, don't start
                a contour here.  The cell will eventually be entered from
                another side not having a fault, or if all 4 sides are faulted,
                no contours will go through the cell.
            */
                if (FaultColumnCrossings) {
                    if (FaultColumnCrossings[offset+j] == 1) {
                        continue;
                    }
                }
                if (FaultNodeGraze) {
                    if (FaultNodeGraze[offset+j] == 1  ||
                        FaultNodeGraze[offset+j+Ncol] == 1) {
                        continue;
                    }
                }

                z1 = Grid[offset+j];
                z2 = Grid[offset+j+Ncol];
                if (z1 >= ContourNullValue  ||  z2 >= ContourNullValue) {
                    continue;
                }

                if (StepGridFlag) {
                    if (StepGraze(z1,z2) == 1) continue;
                }
                else {
                    if ((zlev-z1)*(zlev-z2) >= 0.0f) continue;
                    dz = z1 - z2;
                    if (dz < 0.0) dz = -dz;
                    if (dz <= ztiny) continue;
                }

                istat = TraceCellToCell (i, j, 3, zlev);
                if (istat == -1) {
                    return -1;
                }
                if (DoSmoothing) {
                    istat = SmoothContour ();
                    if (istat == -1) {
                        return -1;
                    }
                }
MSL
                istat = OutputContours ();
                if (istat == -1) {
                    return -1;
                }
            }
        }

    /*
        intersecting a horizontal edge in the middle
    */
        for (i=1; i<Nrow-1; i++) {

            offset = i * Ncol;

            for (j=0; j<Ncol-1; j++) {

                if (Hcrossing[offset+j]) {
                    continue;
                }
            /*
                If a fault intersects this side of the cell, don't start
                a contour here.  The cell will eventually be entered from
                another side not having a fault, or if all 4 sides are faulted,
                no contours will go through the cell.
            */
                if (FaultRowCrossings) {
                    if (FaultRowCrossings[offset+j] == 1) {
                        continue;
                    }
                }
                if (FaultNodeGraze) {
                    if (FaultNodeGraze[offset+j] == 1  ||
                        FaultNodeGraze[offset+j+1] == 1) {
                        continue;
                    }
                }

                z1 = Grid[offset+j];
                z2 = Grid[offset+j+1];
                if (z1 >= ContourNullValue  ||  z2 >= ContourNullValue) {
                    continue;
                }

                if (StepGridFlag) {
                    if (StepGraze(z1,z2) == 1) continue;
                }
                else {
                    if ((zlev-z1)*(zlev-z2) >= 0.0f) continue;
                    dz = z1 - z2;
                    if (dz < 0.0) dz = -dz;
                    if (dz <= ztiny) continue;
                }

                istat = TraceCellToCell (i, j, 0, zlev);
                if (istat == -1) {
                    return -1;
                }
                if (DoSmoothing) {
                    istat = SmoothContour ();
                    if (istat == -1) {
                        return -1;
                    }
                }
MSL
                istat = OutputContours ();
                if (istat == -1) {
                    return -1;
                }
            }
        }

    }  /*  end of loop through contour levels  */

    return 1;

}  /*  end of private TraceContours function  */





/*
  ****************************************************************

                   T r a c e C e l l T o C e l l

  ****************************************************************

    Trace a contour from cell to cell given the starting cell and
  starting side on the cell.  This is only called from TraceContours.
  The contour points are stored in the private Xbuf1 and Ybuf1 arrays.
  The grid offsets of the cells are stored in the CellBuf array and
  the too crowded flag for each contour segment is stored in the
  CrowdBuf array.

*/

int CSWConCalc::TraceCellToCell
                    (int irow, int jcol, int side, CSW_F zlev)
{
    int            i, j, k, nexit, nextcol, nextrow, nextside, n,
                   firstcrowd, lastcrowd, ncrowd, lsize, lsize2, cross,
                   closed, *ibuf, nowside, iside, iside2, di,
                   ii, igood, lastside,
                   predicted_exit, saddle_status;
    CSW_F          xt, yt, x0, y0, z1, z2, pct, pct2,
                   null, xexit, yexit, xfirst, *xrev, *yrev,
                   yfirst, tiny, *fbuf, xentrance, yentrance;
    char           *cwork, *ctmp, *cbuf;
    CSW_F          xf[4], yf[4], zf[4];
    CSW_F          xfexit[4], yfexit[4];
    CSW_F          xref, yref, xfint, yfint;
    int            istat, usefaults, ncrossmax, first_reverse;
    int            orig_side, orig_cell, fault_end, reverse_index,
                   next_fault_cell, orig_col, orig_row, *crev;
    int            nextexit[4], nowexit[4], ncexit[4], nrexit[4];
    int            lastf1, lastf2, local_endpointflag;
    CSW_F          zf1last, zf2last, xendpoint, yendpoint;
    CSW_F          dx, dy, dmax, dist;

    static const int     opposite_side[4] = {2, 3, 0, 1};

/*
    The following are initialized to remove some unimportant
    compiler warnings.
*/
    xexit = 0.0f;
    yexit = 0.0f;
    xt = 0.0f;
    yt = 0.0f;
    cross = 0;
    nowside = 0;
    nextside = 0;
    nextrow = 0;
    nextcol = 0;
    z1 = 0.0f;
    z2 = 0.0f;
    iside2 = 0;
    fault_end = 0;
    xendpoint = 0.0f;
    yendpoint = 0.0f;

    memset (nrexit, 0, 4 * sizeof(int));
    memset (ncexit, 0, 4 * sizeof(int));
    memset (nowexit, 0, 4 * sizeof(int));
    memset (nextexit, 0, 4 * sizeof(int));
    memset (xfexit, 0, 4 * sizeof(CSW_F));
    memset (yfexit, 0, 4 * sizeof(CSW_F));
    memset (zf, 0, 4 * sizeof(CSW_F));

/*
    These initializations are actually needed for the
    function to work correctly.
*/
    Nbuf1 = 0;
    reverse_index = -1;
    lastside = -1;
    next_fault_cell = -1;
    ncrowd = 0;
    null = ContourNullValue;
    tiny = (Xspace + Yspace) / 1000.0f;
    x0 = jcol * Xspace + Xmin;
    y0 = irow * Yspace + Ymin;
    k = irow * Ncol + jcol;

/*
    If the first cell being traversed has faults, interpolate
    new corner points taking the faults into account.
*/
    usefaults = 0;
    if (FaultedFlag == 1) {
        if (FaultCellCrossings[k] != 0) {
            xf[0] = x0;
            yf[0] = y0;
            xf[1] = x0 + Xspace;
            yf[1] = y0;
            xf[2] = xf[1];
            yf[2] = y0 + Yspace;
            xf[3] = xf[0];
            yf[3] = yf[2];
            if (side == 0) {
                xref = xf[0];
                yref = yf[0];
            }
            else if (side == 1) {
                xref = xf[1];
                yref = yf[1];
            }
            else if (side == 2) {
                xref = xf[2];
                yref = yf[2];
            }
            else {
                xref = xf[3];
                yref = yf[3];
            }
            grd_fault_ptr->con_faulted_bilin_interp (Grid, Ncol, Nrow, ContourNullValue,
                                      xref, yref, irow, jcol,
                                      xf, yf, zf, 4);
            usefaults = 1;
        }
    }

/*
    The crossing grids for this line tracing need to be
    initialized to zero here.
*/
    memset (Hcrossing2, 0, Ncol*Nrow*sizeof(MYSIGNED char));
    memset (Vcrossing2, 0, Ncol*Nrow*sizeof(MYSIGNED char));

/*
    Find the starting point in the first grid cell.  Note that
    the crossing grids are not set here in case the contour
    closes on itself.  The index of the crossing grid is saved
    in the cross variable so the appropriate crossing grid can
    be set at the end of this function if the contour does not
    close.  For horizontal crossings cross is positive.  For
    vertical crossings cross is negative and offset by -1 to
    avoid conflicting with the zeroeth index in the horizontal
    crossing grid.

    The node number of the node downhill from the first contour
    crossing is set here.  After the contour is finished, this
    node will be determined to be on the left or right side of
    the contour.
*/
    orig_col = jcol;
    orig_row = irow;
    orig_side = side;
    orig_cell = k;
    if (side == 0) {
        z1 = Grid[k];
        z2 = Grid[k+1];
        if (usefaults) {
            z1 = zf[0];
            z2 = zf[1];
        }
        pct = (zlev-z1) / (z2-z1);
        if (pct < TINY_PERCENT) {
            pct = TINY_PERCENT;
        }
        if (pct > BIG_PERCENT) {
            pct = BIG_PERCENT;
        }
        if (StepGridFlag) {
            pct2 = 0.5f;
            if (pct < 0.0f) pct2 = -.5f;
            pct = pct2;
        }
        xt = x0 + pct * Xspace;
        yt = y0;
        cross = k;
        if (z1 < zlev) {
            OutputDownhill = k;
        }
        else {
            OutputDownhill = k+1;
        }
    }

    else if (side == 1) {
        z1 = Grid[k+1];
        z2 = Grid[k+1+Ncol];
        if (usefaults) {
            z1 = zf[1];
            z2 = zf[2];
        }
        pct = (zlev-z1) / (z2-z1);
        if (pct < TINY_PERCENT) {
            pct = TINY_PERCENT;
        }
        if (pct > BIG_PERCENT) {
            pct = BIG_PERCENT;
        }
        if (StepGridFlag) {
            pct2 = 0.5f;
            if (pct < 0.0f) pct2 = -.5f;
            pct = pct2;
        }
        xt = x0 + Xspace;
        yt = y0 + pct * Yspace;
        cross = -(k+2);
        if (z1 < zlev) {
            OutputDownhill = k+1;
        }
        else {
            OutputDownhill = k+1+Ncol;
        }
    }

    else if (side == 2) {
        z1 = Grid[k+Ncol];
        z2 = Grid[k+1+Ncol];
        if (usefaults) {
            z1 = zf[3];
            z2 = zf[2];
        }
        pct = (zlev-z1) / (z2-z1);
        if (pct < TINY_PERCENT) {
            pct = TINY_PERCENT;
        }
        if (pct > BIG_PERCENT) {
            pct = BIG_PERCENT;
        }
        if (StepGridFlag) {
            pct2 = 0.5f;
            if (pct < 0.0f) pct2 = -.5f;
            pct = pct2;
        }
        xt = x0 + pct * Xspace;
        yt = y0 + Yspace;
        cross = k + Ncol;
        if (z1 < zlev) {
            OutputDownhill = k+Ncol;
        }
        else {
            OutputDownhill = k+1+Ncol;
        }
    }

    else {
        z1 = Grid[k];
        z2 = Grid[k+Ncol];
        if (usefaults) {
            z1 = zf[0];
            z2 = zf[3];
        }
        pct = (zlev-z1) / (z2-z1);
        if (pct < TINY_PERCENT) {
            pct = TINY_PERCENT;
        }
        if (pct > BIG_PERCENT) {
            pct = BIG_PERCENT;
        }
        if (StepGridFlag) {
            pct2 = 0.5f;
            if (pct < 0.0f) pct2 = -.5f;
            pct = pct2;
        }
        xt = x0;
        yt = y0 + pct * Yspace;
        cross = -(k+1);
        if (z1 < zlev) {
            OutputDownhill = k;
        }
        else {
            OutputDownhill = k+Ncol;
        }
    }

/*
    Set this point as the first point in the contour buffer
    and the cell offset as the first cell to be traversed.
*/
    Xbuf1[0] = xt;
    Ybuf1[0] = yt;
    CellBuf[0] = k + side * MAX_NODES;

/*
    Check if the side is too crowded and flag if so.
*/
    CrowdBuf[0] = 0;
    if (usefaults == 0  &&  TooCrowded (k)) {
        CrowdBuf[0] = 1;
        ncrowd++;
    }

    Nbuf1 = 1;
    xfirst = xt;
    yfirst = yt;
    xentrance = xt;
    yentrance = yt;
    closed = 0;
    local_endpointflag = 0;

    lastf1 = -1;
    lastf2 = -1;
    zf1last = 1.e30f;
    zf2last = 1.e30f;

/*
    Run a loop that traverses from cell to cell until an already
    done side, a null value or no exit is found.  In each
    cell there will be zero, one or three exit points.  If
    there are zero, the contour tracing is finished.  If there
    is one, then the exit side is used to determine the next cell
    to enter and the exit point is put into the contour line
    buffers.  If there are more than 1, the exit point closest
    to the entrance point is used as the output.
*/
    for (;;) {

    /*
        find the exit side and exit point
    */
        nexit = 0;

    /*
        If the cell is a saddle point, exit without crossing
        the spill point of the saddle.  If the cell has a fault
        running through it, skip the saddle point stuff.
    */
        di = 1;
        if (usefaults == 0) {
            switch (side) {
                case 0:
                    if (Grid[k] < Grid[k+1]) {
                        di = -1;
                    }
                    break;
                case 1:
                    if (Grid[k+1] < Grid[k+1+Ncol]) {
                        di = -1;
                    }
                    break;
                case 2:
                    if (Grid[k+1+Ncol] < Grid[k+Ncol]) {
                        di = -1;
                    }
                    break;
                case 3:
                    if (Grid[k+Ncol] < Grid[k]) {
                        di = -1;
                    }
                    break;
                default:
                    break;
            }
            SaddleTweak (k, &di, zlev);
        }

        predicted_exit = side + di;
        predicted_exit += 4;
        predicted_exit %= 4;

    /*
        This label is used to retry an exit loop if, due to faulting,
        a cell has no exit from its entry point.
    */
      RETRY_EXIT_LOOP:

        for (i=side+di; i!=side+4*di; i+=di) {

            j = (i + 4) % 4;

        /*
            check for exit on bottom side
        */
            ncrossmax = 0;
            if (usefaults) {
/*
                if (FaultRowCrossings[k] == 1  ||
                    FaultNodeGraze[k+1] == 1  ||
                    FaultNodeGraze[k] == 1) {
                    ncrossmax = 1;
                }
*/
                ncrossmax = 1;
            }
            if (j == 0  &&  Hcrossing[k] <= ncrossmax  &&  Hcrossing2[k] == 0) {

                z1 = Grid[k];
                z2 = Grid[k+1];
                if (usefaults) {
                    z1 = zf[0];
                    z2 = zf[1];
                }

                if (z1 < null  &&  z2 < null) {
                    if (zlev - z1 == 0.0  ||
                        (zlev-z1)*(zlev-z2) < 0.0f) {
                        if (nexit > 0  &&  usefaults == 0) {
                            nexit++;
                            continue;
                        }
                        pct = (zlev - z1) / (z2 - z1);
                        if (StepGridFlag) {
                            pct2 = 0.5f;
                            if (pct < 0.0f) pct2 = -.5f;
                            pct = pct2;
                        }
                        if (pct < TINY_PERCENT) {
                            pct = TINY_PERCENT;
                        }
                        if (pct > BIG_PERCENT) {
                            pct = BIG_PERCENT;
                        }
                        xt = x0 + pct * Xspace;
                        yt = y0;
                        xexit = xt;
                        yexit = yt;
                        nextcol = jcol;
                        nextrow = irow - 1;
                        nowside = j;
                        nextside = 2;
                        if (usefaults) {
                            xfexit[nexit] = xt;
                            yfexit[nexit] = yt;
                            nextexit[nexit] = nextside;
                            nowexit[nexit] = nowside;
                            ncexit[nexit] = nextcol;
                            nrexit[nexit] = nextrow;
                        }
                        nexit++;
                    }
                }
            }

        /*
            check for exit on right side
        */
            ncrossmax = 0;
            if (usefaults) {
/*
                if (FaultColumnCrossings[k+1] == 1  ||
                    FaultNodeGraze[k+1] == 1  ||
                    FaultNodeGraze[k+1+Ncol] == 1) {
                    ncrossmax = 1;
                }
*/
                ncrossmax = 1;
            }
            if (j == 1  &&  Vcrossing[k+1] <= ncrossmax  &&
                Vcrossing2[k+1] == 0) {

                z1 = Grid[k+1];
                z2 = Grid[k+1+Ncol];
                if (usefaults) {
                    z1 = zf[1];
                    z2 = zf[2];
                }

                if (z1 < null  &&  z2 < null) {
                    if (zlev - z1 == 0.0  ||
                        (zlev-z1)*(zlev-z2) < 0.0f) {

                        if (nexit > 0  &&  usefaults == 0) {
                            nexit++;
                            continue;
                        }
                        pct = (zlev - z1) / (z2 - z1);
                        if (StepGridFlag) {
                            pct2 = 0.5f;
                            if (pct < 0.0f) pct2 = -.5f;
                            pct = pct2;
                        }
                        if (pct < TINY_PERCENT) {
                            pct = TINY_PERCENT;
                        }
                        if (pct > BIG_PERCENT) {
                            pct = BIG_PERCENT;
                        }
                        xt = x0 + Xspace;
                        yt = y0 + pct * Yspace;
                        xexit = xt;
                        yexit = yt;
                        nextcol = jcol + 1;
                        nextrow = irow;
                        nowside = j;
                        nextside = 3;
                        if (usefaults) {
                            xfexit[nexit] = xt;
                            yfexit[nexit] = yt;
                            nextexit[nexit] = nextside;
                            nowexit[nexit] = nowside;
                            ncexit[nexit] = nextcol;
                            nrexit[nexit] = nextrow;
                        }
                        nexit++;
                    }
                }
            }

        /*
            check for exit on top side
        */
            ncrossmax = 0;
            if (usefaults) {
/*
                if (FaultRowCrossings[k+Ncol] == 1  ||
                    FaultNodeGraze[k+Ncol] == 1  ||
                    FaultNodeGraze[k+Ncol+1] == 1) {
                    ncrossmax = 1;
                }
*/
                ncrossmax = 1;
            }
            if (j == 2  &&  Hcrossing[k+Ncol] <= ncrossmax  &&
                Hcrossing2[k+Ncol] == 0) {

                z1 = Grid[k+Ncol];
                z2 = Grid[k+1+Ncol];
                if (usefaults) {
                    z1 = zf[3];
                    z2 = zf[2];
                }

                if (z1 < null  &&  z2 < null) {
                    if (zlev - z2 == 0.0  ||
                        (zlev-z1)*(zlev-z2) < 0.0f) {

                        if (nexit > 0  &&  usefaults == 0) {
                            nexit++;
                            continue;
                        }
                        pct = (zlev - z1) / (z2 - z1);
                        if (StepGridFlag) {
                            pct2 = 0.5f;
                            if (pct < 0.0f) pct2 = -.5f;
                            pct = pct2;
                        }
                        if (pct < TINY_PERCENT) {
                            pct = TINY_PERCENT;
                        }
                        if (pct > BIG_PERCENT) {
                            pct = BIG_PERCENT;
                        }
                        xt = x0 + pct * Xspace;
                        yt = y0 + Yspace;
                        xexit = xt;
                        yexit = yt;
                        nextcol = jcol;
                        nextrow = irow + 1;
                        nowside = j;
                        nextside = 0;
                        if (usefaults) {
                            xfexit[nexit] = xt;
                            yfexit[nexit] = yt;
                            nextexit[nexit] = nextside;
                            nowexit[nexit] = nowside;
                            ncexit[nexit] = nextcol;
                            nrexit[nexit] = nextrow;
                        }
                        nexit++;
                    }
                }
            }

        /*
            check for exit on left side
        */
            ncrossmax = 0;
            if (usefaults) {
/*
                if (FaultColumnCrossings[k] == 1  ||
                    FaultNodeGraze[k] == 1  ||
                    FaultNodeGraze[k+Ncol] == 1) {
                    ncrossmax = 1;
                }
*/
                ncrossmax = 1;
            }
            if (j == 3  &&  Vcrossing[k] <= ncrossmax  &&
                Vcrossing2[k] == 0) {

                z1 = Grid[k];
                z2 = Grid[k+Ncol];
                if (usefaults) {
                    z1 = zf[0];
                    z2 = zf[3];
                }

                if (z1 < null  &&  z2 < null) {
                    if (zlev - z2 == 0.0  ||
                        (zlev-z1)*(zlev-z2) < 0.0f) {

                        if (nexit > 0  &&  usefaults == 0) {
                            nexit++;
                            continue;
                        }
                        pct = (zlev - z1) / (z2 - z1);
                        if (StepGridFlag) {
                            pct2 = 0.5f;
                            if (pct < 0.0f) pct2 = -.5f;
                            pct = pct2;
                        }
                        if (pct < TINY_PERCENT) {
                            pct = TINY_PERCENT;
                        }
                        if (pct > BIG_PERCENT) {
                            pct = BIG_PERCENT;
                        }
                        xt = x0;
                        yt = y0 + pct * Yspace;
                        xexit = xt;
                        yexit = yt;
                        nextcol = jcol - 1;
                        nextrow = irow;
                        nowside = j;
                        nextside = 1;
                        if (usefaults) {
                            xfexit[nexit] = xt;
                            yfexit[nexit] = yt;
                            nextexit[nexit] = nextside;
                            nowexit[nexit] = nowside;
                            ncexit[nexit] = nextcol;
                            nrexit[nexit] = nextrow;
                        }
                        nexit++;
                    }
                }
            }

        }  /*  end of for loop through possible exit sides  */

    /*
        break out of the cell to cell traversal loop if there
        were no exit points from the cell.
    */
        if (nexit == 0  &&  local_endpointflag == 1) {
            xexit = xendpoint;
            yexit = yendpoint;
            nexit = 1;
        }

        if (nexit == 0) {

        /*
            If the previous cell traced through was faulted and the current
            cell being traced through is also faulted, expand from the previous
            cell and use these corner points to try and find an exit from the cell.
        */
            if (next_fault_cell >= 0  &&  lastside != -1) {
                if (lastf1 < 0  ||  lastf2 < 0) {
                    break;
                }
                zf[lastf1] = zf1last;
                zf[lastf2] = zf2last;
                next_fault_cell = -1;
                goto RETRY_EXIT_LOOP;
            }
            break;
        }

        if (usefaults == 0) {
            saddle_status = SaddleCheck (Grid[k], Grid[k+1], Grid[k+1+Ncol], Grid[k+Ncol]);
            if (saddle_status == 1) {
                if (nowside != predicted_exit) {
                    di = -di;
                }
            }
        }

    /*
        If there was more than 1 exit in a faulted cell, use the exit
        closest to the entrance point.
    */
        if (usefaults  &&  nexit > 1) {
            igood = -1;
            dmax = 1.e30f;
            for (ii=0; ii<nexit; ii++) {

            /*
                If the exit is on the side directly across from the cell entrance,
                always discard it as an exit choice.
            */
                if (nowexit[ii] == side  ||  nowexit[ii] == side-2  ||
                    nowexit[ii] == side+2) {
                    continue;
                }
                xt = xfexit[ii];
                yt = yfexit[ii];
                dx = xentrance - xt;
                dy = yentrance - yt;
                dist = dx * dx + dy * dy;
                if (dist < dmax) {
                    dmax = dist;
                    igood = ii;
                }
            }

            if (igood >= 0) {
                xexit = xfexit[igood];
                yexit = yfexit[igood];
                nextside = nextexit[igood];
                nowside = nowexit[igood];
                nextcol = ncexit[igood];
                nextrow = nrexit[igood];
            }
            else {
                xexit = xfexit[0];
                yexit = yfexit[0];
                nextside = nextexit[0];
                nowside = nowexit[0];
                nextcol = ncexit[0];
                nextrow = nrexit[0];
            }
        }

    /*
        If there is a fault in the cell and the contour segment intersects
        the fault, set the exit point to the intersection.  It is possible
        that the contour needs to be traced back from its original point
        to another fault.  If that is the case, set the nextside and k
        variables, which tell the tracing where to go next, to the values
        obtained from orig_side and orig_cell.  Also set the reverse_index
        variable to record where the discontinuity in the contour line is.
        If the reverse_index variable has already been set, break out
        of the cell to cell traversal loop.
    */
        fault_end = 0;
        first_reverse = 0;
        if (usefaults == 1) {

            istat = grd_fault_ptr->grd_get_fault_end_point (k, &xfint, &yfint);

            if (istat == 0) {
                grd_fault_ptr->grd_set_fault_end_flag (0);
                istat = grd_fault_ptr->con_find_fault_intersection (xentrance, yentrance,
                                                     xexit, yexit, k, 0,
                                                     &xfint, &yfint);
                grd_fault_ptr->grd_set_fault_end_flag (1);
                if (istat == -1) {
                    return -1;
                }
                next_fault_cell = k;
                lastside = nextside;
                if (nextside == 0) {
                    lastf1 = 0;
                    lastf2 = 1;
                    zf1last = zf[3];
                    zf2last = zf[2];
                }
                else if (nextside == 1) {
                    lastf1 = 1;
                    lastf2 = 2;
                    zf1last = zf[0];
                    zf2last = zf[3];
                }
                else if (nextside == 2) {
                    lastf1 = 2;
                    lastf2 = 3;
                    zf1last = zf[1];
                    zf2last = zf[0];
                }
                else {
                    lastf1 = 3;
                    lastf2 = 0;
                    zf1last = zf[2];
                    zf2last = zf[1];
                }

                if (istat < 1) {
                    if (nextcol < 0  ||  nextcol >= Ncol-1  ||
                        nextrow < 0  ||  nextrow >= Nrow-1) {
                        istat = 1;
                        xfint = xexit;
                        yfint = yexit;
                    }
                }
            }

            if (istat >= 1) {
                Xbuf1[Nbuf1] = xfint;
                Ybuf1[Nbuf1] = yfint;
                CellBuf[Nbuf1] = -(k+1);
                CrowdBuf[Nbuf1] = 0;
                Nbuf1++;
                fault_end = 1;
                next_fault_cell = -1;

            /*
                If the contour line has already been reversed or if the
                original point on the contour line was on the edge of
                the grid, break the cell to cell tracing loop.
            */
                if (reverse_index != -1  ||  MiddleStart == 0) {
                    break;
                }
                reverse_index = Nbuf1;
                first_reverse = 1;
                nowside = orig_side;
                if (orig_side == 0) {
                    nextside = 2;
                    nextcol = orig_col;
                    nextrow = orig_row - 1;
                }
                else if (orig_side == 1) {
                    nextside = 3;
                    nextcol = orig_col + 1;
                    nextrow = orig_row;
                }
                else if (orig_side == 2) {
                    nextside = 0;
                    nextcol = orig_col;
                    nextrow = orig_row + 1;
                }
                else if (orig_side == 3) {
                    nextside = 1;
                    nextcol = orig_col - 1;
                    nextrow = orig_row;
                }
                k = orig_cell;
                lastside = -1;
                xexit = Xbuf1[0];
                yexit = Ybuf1[0];
            }
        }

    /*
        set the crossing grids and the numexit grid
    */
        if (nextside == 0) {
            Hcrossing[k+Ncol] += (MYSIGNED char)1;
            Hcrossing2[k+Ncol] = (MYSIGNED char)1;
        }
        else if (nextside == 1) {
            Vcrossing[k] += (MYSIGNED char)1;
            Vcrossing2[k] = (MYSIGNED char)1;
        }
        else if (nextside == 2) {
            Hcrossing[k] += (MYSIGNED char)1;
            Hcrossing2[k] = (MYSIGNED char)1;
        }
        else {
            Vcrossing[k+1] += (MYSIGNED char)1;
            Vcrossing2[k+1] = (MYSIGNED char)1;
        }

    /*
        expand allocated line buffer memory if needed
    */
        if (Nbuf1 >= MaxBuf1 - 2) {

            MaxBuf1 += LINE_BUFFER_CHUNK;
            lsize = MaxBuf1 * sizeof(CSW_F);
MSL
            fbuf = (CSW_F *)csw_Realloc (Xbuf1, lsize);
            if (!fbuf) return -1;
            Xbuf1 = fbuf;

MSL
            fbuf = (CSW_F *)csw_Realloc (Ybuf1, lsize);
            if (!fbuf) return -1;
            Ybuf1 = fbuf;

            lsize = MaxBuf1 * sizeof(int);
MSL
            ibuf = (int *)csw_Realloc (CellBuf, lsize);
            if (!ibuf) return -1;
            CellBuf = ibuf;

            lsize = MaxBuf1 * sizeof(char);
MSL
            cbuf = (char *)csw_Realloc (CrowdBuf, lsize);
            if (!cbuf) return -1;
            CrowdBuf = cbuf;
MSL
            cbuf = (char *)csw_Realloc (CrowdWork, lsize);
            if (!cbuf) return -1;
            CrowdWork = cbuf;
MSL
            cbuf = (char *)csw_Realloc (Dibuf1, lsize);
            if (!cbuf) return -1;
            Dibuf1 = cbuf;
        }

    /*
        If a step grid is being contoured, and there are more than 2 steps
        in the cell being traversed, connect via the center of the cell.
    */
        if (StepGridFlag  &&  MoreThanTwoSteps (irow, jcol)) {
            Xbuf1[Nbuf1] = x0 + Xspace / 2.0f;
            Ybuf1[Nbuf1] = y0 + Yspace / 2.0f;
            CrowdBuf[Nbuf1] = 0;
            Nbuf1++;
        }

    /*
        put the exit point in the contour buffers and
        set the entry parameters for the next cell.
    */
        if (fault_end == 0) {
            Xbuf1[Nbuf1] = xexit;
            Ybuf1[Nbuf1] = yexit;
            Dibuf1[Nbuf1-1] = (char)di;

        /*
            set the crowd and cell number buffers
        */
            CrowdBuf[Nbuf1] = 0;
            if (TooCrowded (k)) {
                CrowdBuf[Nbuf1] = 1;
                ncrowd++;
            }
            k = nextrow * Ncol + nextcol;
            if (k < 0) k = 0;
            CellBuf[Nbuf1] = k + nowside * MAX_NODES;
        }
        else {
            k = nextrow * Ncol + nextcol;
            if (k < 0) k = 0;
            CrowdBuf[Nbuf1] = 0;
        }

    /*
        break the cell to cell loop if the next cell would be outside the grid
    */
        if (nextcol < 0  ||  nextcol >= Ncol-1  ||
            nextrow < 0  ||  nextrow >= Nrow-1) {
            Nbuf1++;
            break;
        }

    /*
        break the loop if the contour has closed on itself
    */
        if (fault_end == 0  &&
            Nbuf1>3 && SamePoint(xfirst, yfirst, xexit, yexit, tiny)) {
            Xbuf1[Nbuf1] = xfirst;
            Ybuf1[Nbuf1] = yfirst;
            Nbuf1++;
            closed = 1;
            break;
        }

        if (fault_end == 0) {
            Nbuf1++;
        }

    /*
        transfer last cell's exit stuff to next cell's entrance stuff
    */
        side = nextside;
        x0 = nextcol * Xspace + Xmin;
        y0 = nextrow * Yspace + Ymin;
        irow = nextrow;
        jcol = nextcol;
        xentrance = xexit;
        yentrance = yexit;

    /*
        Check if the next cell along the contour path
        needs fault handling.
    */
        usefaults = 0;
        local_endpointflag = 0;
        if (FaultedFlag == 1) {
            if (FaultCellCrossings[k] != 0) {
                if (first_reverse == 1) {
                    xexit = Xbuf1[0];
                    yexit = Ybuf1[0];
                    first_reverse = 0;
                }
                xf[0] = x0;
                yf[0] = y0;
                xf[1] = x0 + Xspace;
                yf[1] = y0;
                xf[2] = xf[1];
                yf[2] = y0 + Yspace;
                xf[3] = xf[0];
                yf[3] = yf[2];
                xref = xexit;
                yref = yexit;
                grd_fault_ptr->con_faulted_bilin_interp (Grid, Ncol, Nrow, ContourNullValue,
                                          xref, yref, irow, jcol,
                                          xf, yf, zf, 4);
                usefaults = 1;
                istat = grd_fault_ptr->grd_get_fault_end_point (k, &xendpoint, &yendpoint);
                local_endpointflag = istat;
            }
        }

    }  /*  end of loop tracing cell to cell through the grid  */

/*
    If a faulted termination of the contour required reordering
    of a part of the line, get everything into the correct order here.
*/
    if (reverse_index != -1) {
        xrev = (CSW_F *)csw_Malloc (Nbuf1 * 2 * sizeof(CSW_F));
        if (xrev == NULL) {
            return -1;
        }
        yrev = xrev + Nbuf1;
        crev = (int *)csw_Malloc (Nbuf1 * sizeof(int));
        if (crev == NULL) {
            csw_Free (xrev);
            return -1;
        }
        n = 0;
        for (i=Nbuf1-1; i>=reverse_index; i--) {
            xrev[n] = Xbuf1[i];
            yrev[n] = Ybuf1[i];
            if (n == 0) {
                crev[n] = CellBuf[i];
            }
            else {
                k = CellBuf[i] % MAX_NODES;
                if (i > reverse_index) {
                    j = CellBuf[i-1] / MAX_NODES;
                }
                else {
                    j = opposite_side[orig_side];
                }
                crev[n] = k + j * MAX_NODES;
            }
            n++;
        }
        for (i=0; i<reverse_index; i++) {
            xrev[n] = Xbuf1[i];
            yrev[n] = Ybuf1[i];
            crev[n] = CellBuf[i];
            n++;
        }
        memcpy (Xbuf1, xrev, Nbuf1 * sizeof(CSW_F));
        memcpy (Ybuf1, yrev, Nbuf1 * sizeof(CSW_F));
        memcpy (CellBuf, crev, Nbuf1 * sizeof(int));
        csw_Free (xrev);
        csw_Free (crev);
    }

/*
    Return zero points if there are not at least 2 points in
    the buffer.
*/
    if (Nbuf1 < 2) {
        Nbuf1 = 0;
        return 1;
    }

/*
    If the polygon is closed determine if it is clockwise or ccw.
*/
    if (closed == 1) {
        CheckContourOrientation ();
    }
    else {
        OutputClosure = 0;
    }

/*
    Smooth out the crowded segments so that only segments
    continuous over 3 points are actually suppressed.

    First, any crowd flags with uncrowded flags on each side
    are set to zero (uncrowded).  The results of this pass
    are stored in the CrowdWork array.
*/
    CrowdWork[0] = CrowdBuf[0];
    CrowdWork[Nbuf1-1] = CrowdBuf[Nbuf1-1];
    for (i=1; i<Nbuf1-1; i++) {
        if (CrowdBuf[i-1] == 0  &&  CrowdBuf[i+1] == 0) {
            CrowdWork[i] = 0;
        }
        else {
            CrowdWork[i] = CrowdBuf[i];
        }
    }

/*
    Second pass, any uncrowded (zeros) surrounded by crowded
    (ones) are set to crowded (one).  The results are put back
    into the CrowdBuf array.
*/
    if (CrowdWork[1] == 1) CrowdBuf[0] = 1;
    if (CrowdWork[Nbuf1-2] == 1) CrowdBuf[Nbuf1-1] = 1;
    for (i=1; i<Nbuf1-1; i++) {
        if (CrowdWork[i-1] == 1  &&  CrowdWork[i+1] == 1) {
            CrowdBuf[i] = 1;
        }
        else {
            CrowdBuf[i] = CrowdWork[i];
        }
    }

/*
    Rearrange the line buffers for closed contours that also
    have sections crowded out.  The points in the contour
    after the last crowded out section are moved to the start
    of the buffer and the points from the original buffer start
    to the end of the last crowded out segment are put in the
    contour buffer after the points that originally followed
    the last crowded section.  This makes the uncrowded section
    of the contour that was originally broken by the arbitrary
    start and end of the contour buffer continuous as it should
    be.
*/

/*
    Return success if there are no crowded contours or if
    the contour length is very short or if the contour does
    not close on itself.
*/
    if (ncrowd == 0  ||  Nbuf1 < 3  ||  closed == 0) {
        return 1;
    }

/*
    find the first and last crowded segment.
*/
    firstcrowd = -1;
    lastcrowd = -1;
    for (i=0; i<Nbuf1; i++) {
        if (CrowdBuf[i]) {
            firstcrowd = i;
            break;
        }
    }

    for (i=Nbuf1-1; i>=0; i--) {
        if (CrowdBuf[i]) {
            lastcrowd = i + 1;
            break;
        }
    }

/*
    If there was no crowded found or if the last is
    the last point in the contour, return success.
*/
    if (firstcrowd < 0  ||  lastcrowd < 0  ||  lastcrowd == Nbuf1) {
        return 1;
    }

/*
    Allocate temporary storage for swapping points in the buffers.  In
    case an int or char is larger than aCSW_F, allocate extra space.
*/
    lsize = (Nbuf1 - 1) * sizeof(CSW_F);
    lsize2 = (Nbuf1 - lastcrowd - 1) * sizeof(CSW_F);

MSL
    cwork = (char *)csw_Malloc (2 * (lsize + sizeof(CSW_F)));
    if (!cwork) {
        return -1;
    }

/*
    copy all of each buffer into the work array and then
    rearrange by copying back in the new order.
*/
    ctmp = (char *)Xbuf1;
    memcpy (cwork, ctmp, lsize);
    memcpy (ctmp, cwork + lsize - lsize2, lsize2);
    memcpy (ctmp + lsize2, cwork, lsize - lsize2);
    Xbuf1[Nbuf1-1] = Xbuf1[0];

    ctmp = (char *)Ybuf1;
    memcpy (cwork, ctmp, lsize);
    memcpy (ctmp, cwork + lsize - lsize2, lsize2);
    memcpy (ctmp + lsize2, cwork, lsize - lsize2);
    Ybuf1[Nbuf1-1] = Ybuf1[0];

/*
    The first CellBuf entry has the entrance side
    for the first cell.  Subsequent have exit sides
    of the previous cell, so I change all to exit
    sides and then change the first (after reordering)
    back to entrance side.
*/
    iside = CellBuf[0] / MAX_NODES;
    k = CellBuf[0] % MAX_NODES;
    if (iside == 3) iside2 = 1;
    if (iside == 2) iside2 = 0;
    if (iside == 1) iside2 = 3;
    if (iside == 0) iside2 = 2;
    CellBuf[0] = k + iside2 * MAX_NODES;

/*
    do the reordering
*/
    ctmp = (char *)CellBuf;
    lsize = (Nbuf1 - 1) * sizeof(int);
    lsize2 = (Nbuf1 - lastcrowd - 1) * sizeof(int);
    memcpy (cwork, ctmp, lsize);
    memcpy (ctmp, cwork + lsize - lsize2, lsize2);
    memcpy (ctmp + lsize2, cwork, lsize - lsize2);
    CellBuf[Nbuf1-1] = CellBuf[0];

/*
    reset first cell as entrance side.
*/
    iside = CellBuf[0] / MAX_NODES;
    k = CellBuf[0] % MAX_NODES;
    if (iside == 3) iside2 = 1;
    if (iside == 2) iside2 = 0;
    if (iside == 1) iside2 = 3;
    if (iside == 0) iside2 = 2;
    CellBuf[0] = k + iside2 * MAX_NODES;

    ctmp = (char *)CrowdBuf;
    lsize = (Nbuf1 - 1) * sizeof(char);
    lsize2 = (Nbuf1 - lastcrowd - 1) * sizeof(char);
    memcpy (cwork, ctmp, lsize);
    memcpy (ctmp, cwork + lsize - lsize2, lsize2);
    memcpy (ctmp + lsize2, cwork, lsize - lsize2);
    CrowdBuf[Nbuf1-1] = CrowdBuf[0];

/*
    This line is needed to prevent a warning on some compilers.
*/
    cross = cross;

/*
    Free the work space and return.
*/
    csw_Free (cwork);

    return 1;

}  /*  end of private TraceCellToCell function  */






/*
  ****************************************************************

                     T o o C r o w d e d

  ****************************************************************

    Check if the grid cell has too steep a gradient relative to the
  contour suppression parameters in effect.  If the cell is too
  crowded and contour drawing should be suppressed, one is returned.
  If the cell is ok for contour drawing, zero is returned.

*/

int CSWConCalc::TooCrowded (int k)
{
    CSW_F            z1, z2, z3, z4, zt1, zt2;

/*
    Return zero if crowding is not to be checked.
*/
    if (FaultedFlag) return 0;
    if (OutputMajor) {
        if (MajorCrowd == 0) {
            return 0;
        }
    }
    else {
        if (MinorCrowd == 0) {
            return 0;
        }
    }

/*
    Check each diagonal of the cell.  If either is
    too crowded, return 1.
*/
    z1 = Grid[k];
    z2 = Grid[k+1];
    z3 = Grid[k+Ncol];
    z4 = Grid[k+Ncol+1];

/*
    Calculate the gradient on each diagonal of the cell.
*/
    zt1 = z4 - z1;
    if (zt1 < 0.0f) zt1 = -zt1;
    zt2 = z3 - z2;
    if (zt2 < 0.0f) zt2 = -zt2;

/*
    Check if it passes the major or minor crowding tests
*/
    if (OutputMajor) {

        if (zt1 > MajorGradient  ||  zt2 > MajorGradient) {
            return 1;
        }
    }

    else {
        if (zt1 > MinorGradient  ||  zt2 > MinorGradient) {
            return 1;
        }
    }

    return 0;

}  /*  end of private TooCrowded function  */





/*
  ****************************************************************

               S e t T o P o s i t i v e N u l l s

  ****************************************************************

    If the grid has negative null values, convert them to positive
  null values of the same magnitude.  This eliminates the need for
  checking positive and negative nulls throughout the contour
  calculations.

*/

int CSWConCalc::SetToPositiveNulls (void)
{
    int             i;

    for (i=0; i<Ncol*Nrow; i++) {

        if (Grid[i] <= ContourNullValue) {
            Grid[i] = -Grid[i];
        }

    }

    ContourNullValue = -ContourNullValue;

    return 1;

}  /*  end of private SetToPositiveNulls function  */




/*
  ****************************************************************

      A d j u s t G r i d F o r C o n t o u r I n t e r v a l

  ****************************************************************

    If any grid node is very close to any multiple of the contour
  interval, as offset by the contour base value, then adjust it
  a little bit so it is not as close.  This keeps contours from
  intersecting exactly on a corner of a grid cell.

*/

int CSWConCalc::AdjustGridForContourInterval (void)
{
    int             i, ilev;
    CSW_F           sign, tiny, tiny3, zt, zt2, zlev;

    if (StepGridFlag) return 1;

    tiny = (Zmax - Zmin) / Z_TINY_DIVISOR;
    if (tiny < Z_ABSOLUTE_TINY) {
      tiny = Z_ABSOLUTE_TINY;
    }
    tiny3 = ContourInterval - tiny;

    for (i=0; i<Ncol*Nrow; i++) {

        if (Grid[i] >= ContourNullValue) {
            continue;
        }

        zt = Grid[i] - BaseValueForContour;
        sign = 1.0f;
        if (zt < 0.0f) {
            zt = -zt;
            sign = -1.0f;
        }
        ilev = (int) (zt / ContourInterval);
        zlev = ilev * ContourInterval;
        zt2 = zt - zlev;

        if (zt2 < tiny) {
            Grid[i] += tiny  *  sign;
        }
        else if (zt2 > tiny3) {
            Grid[i] -= tiny  *  sign;
        }

    }

    return 1;

}  /*  end of private AdjustGridForContourInterval function  */



/*
  ****************************************************************

        A d j u s t G r i d F o r C o n t o u r L e v e l

  ****************************************************************

    If any grid node is very close to the specified value, adjust it
  a little bit so it is not as close.  This keeps contours at this
  level from intersecting exactly on a corner of a grid cell.

*/

int CSWConCalc::AdjustGridForContourLevel (CSW_F zlev)
{
    int             i;
    CSW_F           tiny, zt;

    if (StepGridFlag) return 1;

    tiny = (Zmax - Zmin) / Z_TINY_DIVISOR;
    if (tiny < Z_ABSOLUTE_TINY) {
      tiny = Z_ABSOLUTE_TINY;
    }

    for (i=0; i<Ncol*Nrow; i++) {

        if (Grid[i] >= ContourNullValue) {
            continue;
        }

        zt = Grid[i] - zlev;
        if (zt < 0.0f) {
            zt = -zt;
        }

        if (zt < tiny) {

            if (Grid[i] > zlev) {
                Grid[i] = zlev + tiny;
            }

            else {
                Grid[i] = zlev - tiny;
            }
        }

    }

    return 1;

}  /*  end of private AdjustGridForContourLevel function  */





/*
  ****************************************************************

                   A d j u s t S u b g r i d

  ****************************************************************

    If any subgrid node is very close to the specified value, adjust it
  a little bit so it is not as close.  This keeps contours at this
  level from intersecting exactly on a corner of a grid cell.

*/

int CSWConCalc::AdjustSubgrid (CSW_F *grid, int n, CSW_F zlev)
{
    int             i;
    CSW_F           tiny, zt;

    if (StepGridFlag) return 1;

    tiny = (Zmax - Zmin) / Z_TINY_DIVISOR;
    if (tiny < Z_ABSOLUTE_TINY) {
      tiny = Z_ABSOLUTE_TINY;
    }

    for (i=0; i<n; i++) {

        zt = grid[i] - zlev;
        if (zt < 0.0f) zt = -zt;

        if (zt < tiny) {

            if (grid[i] > zlev) {
                grid[i] = zlev + tiny;
            }

            else {
                grid[i] = zlev - tiny;
            }
        }

    }

    return 1;

}  /*  end of private AdjustSubgrid function  */




/*
  ****************************************************************

                      S a m e P o i n t

  ****************************************************************

    Return 1 if the two points are almost the same or return zero
  otherwise.  Almost is defined by the tiny parameter.

*/

int CSWConCalc::SamePoint (CSW_F x1, CSW_F y1,
                           CSW_F x2, CSW_F y2, CSW_F tiny)
{
    CSW_F        tx, ty;

    tx = x2 - x1;
    if (tx < 0.0f) tx = -tx;
    ty = y2 - y1;
    if (ty < 0.0f) ty = -ty;

    if (tx < tiny  &&  ty < tiny) return 1;

    return 0;

}  /*  end of private SamePoint function  */







/*
  ****************************************************************

                     S m o o t h C o n t o u r

  ****************************************************************

  Smooth a contour line by adding interpolated points where the line
  crosses cells with high curvature.  The unsmoothed points are read
  from Xbuf1 and Ybuf1, with the cells traversed read from CellBuf.
  The smoothed points are stored in Xbuf2 and Ybuf2, with the count
  stored in Nbuf2.

*/

int CSWConCalc::SmoothContour (void)
{
    int              i, k, side, side2, istat, lastsmooth;
    int              jcol, irow;
    CSW_F            dx, dy, dist, tiny, fnull;
    CSW_F            cs, x1, y1, x2, y2;

    static const int       ksides[] = {2, 3, 0, 1};

    Nbuf2 = 0;

    if (Nbuf1 < 2) return 1;

    tiny = (Xspace + Yspace) / 2000.f;
    lastsmooth = 0;
    fnull = 1.e30f;

    cs = ContourSmoothing / 3.0f;
    cs *= cs;

    XCellMin = -1.e30f;
    YCellMin = -1.e30f;
    XCellMax = 1.e30f;
    YCellMax = 1.e30f;

/*
    If the contour line is very short, do not
    smooth it.
*/
    gpf_xandylimits (Xbuf1, Ybuf1, Nbuf1,
                     &x1, &y1, &x2, &y2);
    dx = x2 - x1;
    dy = y2 - y1;

    if (dx < Xspace / 5.0f  &&  dy < Yspace / 5.0f) {
        memcpy (Xbuf2, Xbuf1, Nbuf1*sizeof(CSW_F));
        memcpy (Ybuf2, Ybuf1, Nbuf1*sizeof(CSW_F));
        Nbuf2 = Nbuf1;
        return 1;
    }

/*
    If the unsmoothed line has 19 or less points and is closed,
    smooth the grid cells traversed by the line, unless
    another line has already crossed the cell.  Basically,
    the first contour to cross a currently unsmoothed cell
    determines whether the cell will be smoothed or not.  All
    contours drawn through the cell will be the same (i.e. all
    smoothed or all unsmoothed).  If the contours were mixed,
    then they would probably cross each other (a bad thing).
*/
    dx = Xbuf1[0] - Xbuf1[Nbuf1 -1];
    dy = Ybuf1[0] - Ybuf1[Nbuf1 -1];
    if (dx < 0.0f) dx = -dx;
    if (dy < 0.0f) dy = -dy;
    if (Nbuf1 < 20  &&  dx < tiny  &&  dy < tiny) {
        for (i=0; i<Nbuf1-1; i++) {
            k = CellBuf[i] % MAX_NODES;
            k %= MAX_NODES;
            if (SmoothFlags[k] == 0) {
                SmoothFlags[k] = (MYSIGNED char)1;
            }
        }
    }

/*
    The first line drawn through the cells is not short and
    connected, so the cells are set to -1 to keep them from
    being smoothed by a future short connected contour.
*/
    else {
        for (i=0; i<Nbuf1-1; i++) {
            k = CellBuf[i];
            if (k < 0) {
                k = -k - 1;
            }
            k %= MAX_NODES;
            if (SmoothFlags[k] == 0) {
                SmoothFlags[k] = (MYSIGNED char)-1;
            }
        }
    }

/*
    Loop through the unsmoothed contour line and add smoothing
    points where needed.
*/
    for (i=0; i<Nbuf1-1; i++) {

    /*
        If the point is on a side that is too crowded, append 1.e30
        values to x and y smoothed buffers and go to the next point.
    */
        if (CrowdBuf[i]  ||  CrowdBuf[i+1]) {
            istat = AppendToSmooth (fnull, fnull);
            if (istat == -1) {
                return -1;
            }
            lastsmooth = 0;
            continue;
        }

    /*
        Skip smoothing if the surface in the cell is not curved enough
        or if there are more than 1 exit from the cell.
    */
        k = CellBuf[i+1];
        side2 = k / MAX_NODES;
        k = CellBuf[i];
        side = k / MAX_NODES;
        if (i > 0) {
            side = ksides[side];
        }
        k %= MAX_NODES;

        irow = k / Ncol;
        jcol = k % Ncol;
        XCellMin = Xmin + jcol * Xspace;
        YCellMin = Ymin + irow * Yspace;
        XCellMax = XCellMin + Xspace;
        YCellMax = YCellMin + Yspace;

        if (SmoothFlags[k] <= 0) {
            if (lastsmooth == 0) {
                istat = AppendToSmooth (Xbuf1[i], Ybuf1[i]);
                if (istat == -1) {
                    return -1;
                }
            }
            lastsmooth = 0;
            continue;
        }

    /*
        Smooth through the cell and append to the
        Xbuf2, Ybuf2 arrays.
    */
        DiMajor = (int)Dibuf1[i];

      /*
       * Bug 8052, pass the exit point from the cell to the smoothing
       *   function in case the smoothed grid exits on a different side.
       */
        istat = SmoothCell (k, side, side2, Xbuf1[i+1], Ybuf1[i+1]);
        if (istat == -1) {
            if (lastsmooth == 0) {
                istat = AppendToSmooth (Xbuf1[i], Ybuf1[i]);
                if (istat == -1) {
                    return -1;
                }
            }
            lastsmooth = 0;
        }

    /*
        Malloc error extending smoothed contour buffer.
    */
        else if (istat == -2) {
            return -1;
        }

        else {
            lastsmooth = 1;
        }

    }

/*
    Append last point to the smoothed contour buffer if needed.
*/
    if (lastsmooth == 0) {
        if (CrowdBuf[Nbuf1-1]) {
            istat = AppendToSmooth (fnull, fnull);
            if (istat == -1) {
                return -1;
            }
            return 1;
        }
        else {
            istat = AppendToSmooth (Xbuf1[Nbuf1-1], Ybuf1[Nbuf1-1]);
            if (istat == -1) {
                return -1;
            }
        }
    }

/*
    If the unsmoothed contour is closed, make sure the smoothed
    contour is also closed.
*/
    dx = Xbuf1[0] - Xbuf1[Nbuf1-1];
    dy = Ybuf1[0] - Ybuf1[Nbuf1-1];
    dist = dx * dx + dy * dy;
    dist = (CSW_F)sqrt ((double)dist);
    if (dist < tiny) {
        Xbuf2[0] = Xbuf2[Nbuf2-1];
        Ybuf2[0] = Ybuf2[Nbuf2-1];
/*
        Xbuf2[Nbuf2-1] = Xbuf2[0];
        Ybuf2[Nbuf2-1] = Ybuf2[0];
*/
        CheckSmoothedContourOrientation ();
    }

    return 1;

}  /*  end of private SmoothContour function  */






/*
  ****************************************************************

                      S m o o t h C e l l

  ****************************************************************

    Set up a subgrid in a cell and trace the contour through it.

*/

int CSWConCalc::SmoothCell
                      (int node, int side1, int side2,
                       CSW_F xendin, CSW_F yendin)
{
    int           i, j, k, n, irow, jcol, good, istat, nr2, nc2;
    int           i1, i2, j1, j2, offset;
    CSW_F         xp[1000], yp[1000], xref, yref;
    CSW_F         x0, y0, dx, dy, xt, yt, z1, z2,
                  zlev, xend, yend, *zgrid, *zgridsav;
    CSW_F         zgridtmp[MAX_SUB_GRID_SIZE];

/*
    initializations to prevent unimportant compiler warnings
*/
    xt = 0.0f;
    yt = 0.0f;

/*
    initializations actually needed
*/
    zlev = OutputZlev;

/*
    Create the subgrid using bicubic interpolation
*/
    i = node / Ncol;
    j = node % Ncol;
    irow = i;
    jcol = j;
    k = node;

/*
 * If there are less than 4 columns or rows, skip this.
 */
    if (Ncol < 4  ||  Nrow < 4) {
        return -1;
    }

/*
    If there are any null nodes in the bicubic area,
    return -1 and use the unsmoothed line through the cell.
*/
    i1 = i - 1;
    if (i1 < 0) i1 = 0;
    j1 = j - 1;
    if (j1 < 0) j1 = 0;
    i2 = i1 + 3;
    j2 = j1 + 3;
    if (i2 > Nrow-1) {
        i2 = Nrow-1;
        i1 = i2 - 3;
    }
    if (j2 > Ncol-1) {
        j2 = Ncol-1;
        j1 = j2 - 3;
    }
    for (i=i1; i<=i2; i++) {
        offset = i*Ncol;
        for (j=j1; j<=j2; j++) {
            if (NoNullGrid[offset+j] >= ContourNullValue) {
                return -1;
            }
        }
    }

    dx = Xspace / (CSW_F)(SubgridCols - 1);
    dy = Yspace / (CSW_F)(SubgridRows - 1);
    x0 = Xmin + jcol * Xspace - SmoothMargin * dx;
    y0 = Ymin + irow * Yspace - SmoothMargin * dy;

    nc2 = SubgridCols + SmoothMargin * 2;
    nr2 = SubgridRows + SmoothMargin * 2;

    if (Nbuf2 < 1) {
        xref = Xbuf1[0];
        yref = Ybuf1[0];
    }
    else {
        xref = Xbuf2[Nbuf2-1];
        yref = Ybuf2[Nbuf2-1];
    }

    if (!SmoothSubgrids[k]) {
        n = 0;
        for (i=0; i<nr2; i++) {
            yt = y0 + i * dy;
            if (i == nr2 - 1)
                yt = y0 + Yspace;
            for (j=0; j<nc2; j++) {
                yp[n] = yt;
                xp[n] = x0 + j * dx;
                if (j == nc2 - 1)
                    xp[n] = x0 + Xspace;
                n++;
            }
        }
MSL
        zgrid = (CSW_F *)csw_Malloc (n * sizeof(CSW_F));
        if (!zgrid) {
            return -1;
        }
        if (FaultedFlag == 1) {
            istat = grd_fault_ptr->con_faulted_bicub_interp (Grid, Ncol, Nrow, 1.e20f,
                                              xref, yref, irow, jcol,
                                              xp, yp, zgrid, n);
            if (istat == -1) {
                return -1;
            }
        }
        else {
            istat = grd_utils_ptr->grd_bicub_interp (xp, yp, zgrid, n, 0.0f,
                                      NoNullGrid, Ncol, Nrow, 1,
                                      Xmin, Ymin, Xmax, Ymax,
                                      irow, jcol);
            if (istat == -1) {
                return -1;
            }

            zgrid[0] = Grid[irow*Ncol + jcol];
            zgrid[nc2-1] = Grid[irow*Ncol + jcol + 1];
            zgrid[(nr2-1)*nc2] = Grid[(irow+1)*Ncol + jcol];
            zgrid[nr2*nc2-1] = Grid[(irow+1)*Ncol + jcol + 1];
        }

        SmoothSubgrids[k] = zgrid;

    }
    else {
        zgrid = SmoothSubgrids[k];
    }

/*
    make a copy of the subgrid for adjustment.  zgridtmp is
    a local array guaranteed to be big enough for the subgrid.
*/
    zgridsav = zgrid;
    zgrid = zgridtmp;

    memcpy (zgrid, zgridsav, nc2*nr2*sizeof(CSW_F));

/*
    Make sure no subgrid node is exactly equal to the
    current contour level.
*/
    AdjustSubgrid (zgrid, nr2*nc2, zlev);

/*
    Find the starting node in the subgrid.
*/
    good = 0;
    if (side1 == 0) {
        irow = SmoothMargin;
        yt = SmoothMargin * dy;
        for (j=SmoothMargin; j<SubgridCols-1+SmoothMargin; j++) {
            z1 = zgrid[j + SmoothMargin*nc2];
            z2 = zgrid[j+1 + SmoothMargin*nc2];
            if ((zlev >= z1  &&  zlev <= z2)  ||  (zlev >= z2  &&  zlev <= z1)) {
                jcol = j;
                xt = (zlev - z1) / (z2 - z1) * dx + j * dx;
                good = 1;
                break;
            }
        }
    }
    else if (side1 == 1) {
        jcol = SubgridCols - 2 + SmoothMargin;
        xt = Xspace + SmoothMargin * dx;
        for (i=SmoothMargin; i<SubgridRows-1+SmoothMargin; i++) {
            z1 = zgrid[i*nc2 + nc2 - 1 - SmoothMargin];
            z2 = zgrid[(i+1)*nc2 + nc2 - 1 - SmoothMargin];
            if ((zlev >= z1  &&  zlev <= z2)  ||  (zlev >= z2  &&  zlev <= z1)) {
                irow = i;
                yt = (zlev - z1) / (z2 - z1) * dy + i * dy;
                good = 1;
                break;
            }
        }
    }
    else if (side1 == 2) {
        irow = SubgridRows - 2 + SmoothMargin;
        yt = Yspace + SmoothMargin * dy;
        for (j=SmoothMargin; j<SubgridCols-1+SmoothMargin; j++) {
            z1 = zgrid[j + nc2 * (nr2 - 1 - SmoothMargin)];
            z2 = zgrid[j+1 + nc2 * (nr2 - 1 - SmoothMargin)];
            if ((zlev >= z1  &&  zlev <= z2)  ||  (zlev >= z2  &&  zlev <= z1)) {
                jcol = j;
                xt = (zlev - z1) / (z2 - z1) * dx + j * dx;
                good = 1;
                break;
            }
        }
    }
    else if (side1 == 3) {
        jcol = SmoothMargin;
        xt = SmoothMargin * dx;
        for (i=SmoothMargin; i<SubgridRows-1+SmoothMargin; i++) {
            z1 = zgrid[i*nc2+SmoothMargin];
            z2 = zgrid[(i+1)*nc2+SmoothMargin];
            if ((zlev >= z1  &&  zlev <= z2)  ||  (zlev >= z2  &&  zlev <= z1)) {
                irow = i;
                yt = (zlev - z1) / (z2 - z1) * dy + i * dy;
                good = 1;
                break;
            }
        }
    }
    else {
        return -1;
    }

/*
    If no good start point was found, return -1 and use the
    unsmoothed line through the grid cell.
*/
    if (good == 0) {
        return -1;
    }

/*
    Add the start point to the smoothed line.
*/
    istat = AppendToSmooth (xt+x0, yt+y0);
    if (istat == -1) {
        return -2;
    }

/*
    Find the ending point in the subgrid.
*/
/*
 * Bug 8052
 *   use the good flag to see if there is a calculated exit from
 *   the smoothed grid that lies on the same cell side as the
 *   unsmoothed exit.
 */
    good = 0;
    if (side2 == 0) {
        yt = SmoothMargin * dy;
        for (j=SmoothMargin; j<SubgridCols-1+SmoothMargin; j++) {
            z1 = zgrid[j + SmoothMargin*nc2];
            z2 = zgrid[j+1 + SmoothMargin*nc2];
            if ((zlev >= z1  &&  zlev <= z2)  ||  (zlev >= z2  &&  zlev <= z1)) {
                xt = (zlev - z1) / (z2 - z1) * dx + j * dx;
                good = 1;
                break;
            }
        }
    }
    else if (side2 == 1) {
        xt = Xspace + SmoothMargin * dx;
        for (i=SmoothMargin; i<SubgridRows-1+SmoothMargin; i++) {
            z1 = zgrid[i*nc2 + nc2 - 1 - SmoothMargin];
            z2 = zgrid[(i+1)*nc2 + nc2 - 1 - SmoothMargin];
            if ((zlev >= z1  &&  zlev <= z2)  ||  (zlev >= z2  &&  zlev <= z1)) {
                yt = (zlev - z1) / (z2 - z1) * dy + i * dy;
                good = 1;
                break;
            }
        }
    }
    else if (side2 == 2) {
        yt = Yspace + SmoothMargin * dy;
        for (j=SmoothMargin; j<SubgridCols-1+SmoothMargin; j++) {
            z1 = zgrid[j + nc2 * (nr2 - 1 - SmoothMargin)];
            z2 = zgrid[j+1 + nc2 * (nr2 - 1 - SmoothMargin)];
            if ((zlev >= z1  &&  zlev <= z2)  ||  (zlev >= z2  &&  zlev <= z1)) {
                xt = (zlev - z1) / (z2 - z1) * dx + j * dx;
                good = 1;
                break;
            }
        }
    }
    else if (side2 == 3) {
        xt = SmoothMargin * dx;
        for (i=SmoothMargin; i<SubgridRows-1+SmoothMargin; i++) {
            z1 = zgrid[i*nc2+SmoothMargin];
            z2 = zgrid[(i+1)*nc2+SmoothMargin];
            if ((zlev >= z1  &&  zlev <= z2)  ||  (zlev >= z2  &&  zlev <= z1)) {
                yt = (zlev - z1) / (z2 - z1) * dy + i * dy;
                good = 1;
                break;
            }
        }
    }
    else {
        return -1;
    }

/*
 * Bug 8052
 *  If no exit was found, use the original end of the contour as
 *  passed to the function.
 */
    if (good == 1) {
        xend = x0 + xt;
        yend = y0 + yt;
    }
    else {
        xend = xendin;
        yend = yendin;
    }

/*
    Trace through the subgrid
*/
    istat = TraceSubgrid (irow, jcol, side1, zlev, xend, yend,
                          zgrid, nr2, nc2,
                          dx, dy, x0, y0);
    if (istat == -1) {
        return -2;
    }

    return 1;

}  /*  end of private SmoothCell function  */







/*
  ****************************************************************

                       T r a c e S u b g r i d

  ****************************************************************

    Called from the contour smoothing functions to trace a line
  through the subgrid in a cell.  Each exit point is appended to
  the smoothed contour line.

*/

int CSWConCalc::TraceSubgrid
              (int irow, int jcol, int side,
               CSW_F zlev, CSW_F xend, CSW_F yend,
               CSW_F *grid, int nrow, int ncol,
               CSW_F xspace, CSW_F yspace, CSW_F xmin, CSW_F ymin)
{
    int            i, j, k, kk, nexit, istat, nmax, ncount, di,
                   nextcol, nextrow, nextside, orignbuf2, good;
    CSW_F          xt, yt, x0, y0, z1, z2, pct, dx, dy, tiny,
                   xexit, yexit, zz1, zz2;

    orignbuf2 = Nbuf2;

/*
    The following are initialized to remove some unimportant
    compiler warnings.
*/
    xexit = 0.0f;
    yexit = 0.0f;
    xt = 0.0f;
    yt = 0.0f;
    nextside = 0;
    nextrow = 0;
    nextcol = 0;
    z1 = 0.0f;
    z2 = 0.0f;

/*
    These initializations are actually needed for the
    function to work right.
*/
    x0 = jcol * xspace + xmin;
    y0 = irow * yspace + ymin;

    tiny = (xspace + yspace) / 100.f;

    nmax = (ncol + nrow) * 4;
    ncount = 0;

/*
    Run a loop that traverses from cell to cell until an already
    done side or no exit is found.  In each cell there will be
    one or zero or three exit points.  If more than one exit point
    is found, use the point on the side that has the same orientation
    from the entrance side as the coarse contour.  This orientation
    is in the private DiMajor variable.
*/
    for (;;) {

    /*
        find the exit side and exit point
    */
        nexit = 0;
        k = irow * ncol + jcol;

        di = DiMajor;
        for (i=side+di; i!=side+4*di; i+=di) {

            j = (i + 4) % 4;

        /*
            check for exit on bottom side
        */
            if (j == 0) {

                z1 = grid[k];
                z2 = grid[k+1];

                if (zlev - z1 == 0.0  ||
                    (zlev-z1)*(zlev-z2) < 0.0f) {

                    pct = (zlev - z1) / (z2 - z1);
                    xt = x0 + pct * xspace;
                    yt = y0;

                    xexit = xt;
                    yexit = yt;
                    if (irow > 0) {
                        nextcol = jcol;
                        nextrow = irow - 1;
                        nextside = 2;
                    }

                /*
                    If the exit is at the bottom of the subgrid and that point
                    is not the exit from the subgrid (xend, yend), then draw a
                    segment along the bottom edge to the other intersection of
                    the contour level.
                */
                    else {
                        dx = xend - xexit;
                        dy = yend - yexit;
                        if (dx < 0.0f) dx = -dx;
                        if (dy < 0.0f) dy = -dy;
                        if (dx >= tiny  ||  dy >= tiny) {
                            istat = AppendToSmooth (xexit, yexit);
                            if (istat == -1) {
                                return -1;
                            }
                            good = 0;
                            for (kk=0; kk<ncol-1; kk++) {
                                if (kk == jcol) continue;
                                zz1 = grid[kk];
                                zz2 = grid[kk+1];
                                if ((zlev-zz1)*(zlev-zz2) < 0.0f) {
                                    pct = (zlev - zz1) / (zz2 - zz1);
                                    x0 = kk * xspace + xmin;
                                    xexit = x0 + pct * xspace;
                                    yexit = y0;
                                    nextcol = kk;
                                    nextrow = 0;
                                    nextside = 0;
                                    good = 1;
                                    break;
                                }
                            }
                            if (!good) {
                                nextcol = jcol;
                                nextrow = -1;
                                nextside = 2;
                            }
                        }
                    }

                    nexit++;
                    break;
                }
            }

        /*
            check for exit on right side
        */
            if (j == 1) {

                z1 = grid[k+1];
                z2 = grid[k+1+ncol];

                if (zlev - z1 == 0.0  ||
                    (zlev-z1)*(zlev-z2) < 0.0f) {

                    pct = (zlev - z1) / (z2 - z1);
                    xt = x0 + xspace;
                    yt = y0 + pct * yspace;
                    xexit = xt;
                    yexit = yt;
                    if (jcol < ncol-1) {
                        nextcol = jcol + 1;
                        nextrow = irow;
                        nextside = 3;
                    }

                /*
                    If the exit is at the right of the subgrid and that point
                    is not the exit from the subgrid (xend, yend), then draw a
                    segment along the right edge to the other intersection of
                    the contour level.
                */
                    else {
                        dx = xend - xexit;
                        dy = yend - yexit;
                        if (dx < 0.0f) dx = -dx;
                        if (dy < 0.0f) dy = -dy;
                        if (dx >= tiny  ||  dy >= tiny) {
                            istat = AppendToSmooth (xexit, yexit);
                            if (istat == -1) {
                                return -1;
                            }
                            good = 0;
                            for (kk=ncol-1; kk<ncol*nrow-ncol-1; kk+=ncol) {
                                if (kk/ncol == irow) continue;
                                zz1 = grid[kk];
                                zz2 = grid[kk+ncol];
                                if ((zlev-zz1)*(zlev-zz2) < 0.0f) {
                                    pct = (zlev - zz1) / (zz2 - zz1);
                                    xexit = x0 + xspace;
                                    y0 = kk/ncol * yspace + ymin; /*lint !e653*/
                                    yexit = y0 + pct * yspace;
                                    nextcol = jcol;
                                    nextrow = kk/ncol;
                                    nextside = 1;
                                    good = 1;
                                    break;
                                }
                            }
                            if (!good) {
                                nextcol = ncol;
                                nextrow = irow;
                                nextside = 3;
                            }
                        }
                    }
                    nexit++;
                    break;
                }
            }

        /*
            check for exit on top side
        */
            if (j == 2) {

                z1 = grid[k+ncol];
                z2 = grid[k+1+ncol];

                if (zlev - z2 == 0.0  ||
                    (zlev-z1)*(zlev-z2) < 0.0f) {

                    pct = (zlev - z1) / (z2 - z1);
                    xt = x0 + pct * xspace;
                    yt = y0 + yspace;
                    xexit = xt;
                    yexit = yt;
                    if (irow < nrow) {
                        nextcol = jcol;
                        nextrow = irow + 1;
                        nextside = 0;
                    }

                /*
                    If the exit is at the top of the subgrid and that point
                    is not the exit from the subgrid (xend, yend), then draw a
                    segment along the top edge to the other intersection of
                    the contour level.
                */
                    else {
                        dx = xend - xexit;
                        dy = yend - yexit;
                        if (dx < 0.0f) dx = -dx;
                        if (dy < 0.0f) dy = -dy;
                        if (dx >= tiny  ||  dy >= tiny) {
                            istat = AppendToSmooth (xexit, yexit);
                            if (istat == -1) {
                                return -1;
                            }
                            good = 0;
                            for (kk=ncol*nrow-3; kk<ncol*nrow-1; kk++) {
                                if (kk%ncol == jcol) continue;
                                zz1 = grid[kk];
                                zz2 = grid[kk+1];
                                if ((zlev-zz1)*(zlev-zz2) < 0.0f) {
                                    pct = (zlev - zz1) / (zz2 - zz1);
                                    x0 = kk%ncol * xspace + xmin;
                                    xexit = x0 + pct * xspace;
                                    yexit = y0 + yspace;
                                    nextcol = kk%ncol;
                                    nextrow = nrow-1;
                                    nextside = 2;
                                    good = 1;
                                    break;
                                }
                            }
                            if (!good) {
                                nextcol = jcol;
                                nextrow = nrow;
                                nextside = 0;
                            }
                        }
                    }
                    nexit++;
                    break;
                }
            }

        /*
            check for exit on left side
        */
            if (j == 3) {

                z1 = grid[k];
                z2 = grid[k+ncol];

                if (zlev - z2 == 0.0  ||
                    (zlev-z1)*(zlev-z2) < 0.0f) {

                    pct = (zlev - z1) / (z2 - z1);
                    xt = x0;
                    yt = y0 + pct * yspace;
                    xexit = xt;
                    yexit = yt;
                    if (jcol > 0) {
                        nextcol = jcol - 1;
                        nextrow = irow;
                        nextside = 1;
                    }

                /*
                    If the exit is at the left of the subgrid and that point
                    is not the exit from the subgrid (xend, yend), then draw a
                    segment along the left edge to the other intersection of
                    the contour level.
                */
                    else {
                        dx = xend - xexit;
                        dy = yend - yexit;
                        if (dx < 0.0f) dx = -dx;
                        if (dy < 0.0f) dy = -dy;
                        if (dx >= tiny  ||  dy >= tiny) {
                            istat = AppendToSmooth (xexit, yexit);
                            if (istat == -1) {
                                return -1;
                            }
                            good = 0;
                            for (kk=0; kk<ncol*nrow-ncol-1; kk+=ncol) {
                                if (kk/ncol == irow) continue;
                                zz1 = grid[kk];
                                zz2 = grid[kk+ncol];
                                if ((zlev-zz1)*(zlev-zz2) < 0.0f) {
                                    pct = (zlev - zz1) / (zz2 - zz1);
                                    xexit = x0;
                                    y0 = kk/ncol * yspace + ymin; /*lint !e653*/
                                    yexit = y0 + pct * yspace;
                                    nextcol = 0;
                                    nextrow = kk/ncol;
                                    nextside = 3;
                                    good = 1;
                                    break;
                                }
                            }
                            if (!good) {
                                nextcol = -1;
                                nextrow = irow;
                                nextside = 1;
                            }
                        }
                    }
                    nexit++;
                    break;
                }
            }

        }  /*  end of for loop through possible exit sides  */

    /*
        break out of the cell to cell traversal loop if there
        were no exit points from the cell.
    */
        if (nexit == 0) {
            break;
        }

    /*
        put the exit point in the contour buffers and
        set the entry parameters for the next cell.
    */
        istat = AppendToSmooth (xexit, yexit);
        if (istat == -1) {
            return -1;
        }

    /*
        If a large number of points have been added, there
        is probably a problem with multiple exits from a
        subgrid cell and the program is looping.  In this
        case, abort the smoothing through this cell.
    */
        ncount++;
        if (ncount > nmax) {
            Nbuf2 = orignbuf2;
            istat = AppendToSmooth (xend, yend);
            if (istat == -1) {
                return -1;
            }
            break;
        }

    /*
        If the exit from the cell is the same as the exit
        from the entire subgrid, the tracing is done.
    */
        dx = xend - xexit;
        dy = yend - yexit;
        if (dx < 0.0f) dx = -dx;
        if (dy < 0.0f) dy = -dy;

        if (dx < tiny  &&  dy < tiny) {
            break;
        }

    /*
        If the next cell is outside the subgrid, append
        the end point instead of the last point calculated
        and return.
    */
        if (nextcol < 0  ||  nextcol >= ncol-1  ||
            nextrow < 0  ||  nextrow >= nrow-1) {
            Nbuf2 = orignbuf2;
            istat = AppendToSmooth (xend, yend);
            if (istat == -1) {
                return -1;
            }
            break;
        }

    /*
        transfer last cell's exit stuff to next cell's entrance stuff
    */
        side = nextside;
        x0 = nextcol * xspace + xmin;
        y0 = nextrow * yspace + ymin;
        irow = nextrow;
        jcol = nextcol;

    }  /*  end of loop tracing cell to cell through the subgrid  */

    return 1;

}  /*  end of private TraceSubgrid function  */








/*
  ****************************************************************

                 A p p e n d T o S m o o t h

  ****************************************************************

  Append the specified point to the smooth contour buffers,
  csw_Reallocating space if needed.

*/

int CSWConCalc::AppendToSmooth (CSW_F x, CSW_F y)
{
    CSW_F       *fbuf;

/*
 * If the x and y are not null flags, clip them to the
 * current cell limits.
 */
    if (x < 1.e20  &&  y < 1.e20) {
        if (x < XCellMin) x = XCellMin;
        if (y < YCellMin) y = YCellMin;
        if (x > XCellMax) x = XCellMax;
        if (y > YCellMax) y = YCellMax;
    }

/*
    Extend the smoothed buffer if needed.
*/
    if (Nbuf2 + 2 > MaxBuf2) {
        MaxBuf2 += LINE_BUFFER_CHUNK;
MSL
        fbuf = (CSW_F *)csw_Realloc
                (Xbuf2, MaxBuf2 * sizeof(CSW_F));
        if (!fbuf) {
            DoSmoothing = 0;
            return -1;
        }
        Xbuf2 = fbuf;
MSL
        fbuf = (CSW_F *)csw_Realloc
                (Ybuf2, MaxBuf2 * sizeof(CSW_F));
        if (!fbuf) {
            DoSmoothing = 0;
            return -1;
        }
        Ybuf2 = fbuf;
    }

    Xbuf2[Nbuf2] = x;
    Ybuf2[Nbuf2] = y;
    Nbuf2++;

    return 1;

}  /*  end of private AppendToSmooth function  */





/*
  ****************************************************************

                    O u t p u t C o n t o u r s

  ****************************************************************

    Output contour segments to the ContourData array of structures.
  A contour segment is a continuous line between crowded out points.
  Thus, several contour segments can be output with each call to
  this function.

*/

int CSWConCalc::OutputContours (void)
{
    int             nbuf, i, istat, len, first, last;
    CSW_F           *xbuf, *ybuf, *xptr, *yptr, *xout, *yout;

/*
    Output either the raw or smoothed points depending
    upon the contour smoothing flag.
*/
    if (DoSmoothing) {
        xbuf = Xbuf2;
        ybuf = Ybuf2;
        nbuf = Nbuf2;
    }

    else {

        for (i=0; i<Nbuf1; i++) {
            if (CrowdBuf[i]) {
                Xbuf1[i] = 1.e30f;
                Ybuf1[i] = 1.e30f;
            }
        }

        xbuf = Xbuf1;
        ybuf = Ybuf1;
        nbuf = Nbuf1;
    }

/*
    Separate contour segments between crowded out points
    and put them into the list of output contour structures.
*/
    first = 0;
    for (i=0; i<nbuf; i++) {

        if (xbuf[i] > 1.e20f  ||  i == nbuf - 1) {

            last = i;
            if (i == nbuf-1  &&  xbuf[i] < 1.e20f) last = nbuf;

            if (last-first > 1) {

                xptr = xbuf + first;
                yptr = ybuf + first;
                len = last - first;

            /*
                Note that the memory allocated below is put into the
                COntourOutputRec structure.  The application must csw_Free
                this by calling con_FreeContours.
            */
                xout = (CSW_F *)csw_Malloc (len * 2 * sizeof(CSW_F));
                if (!xout) {
                    return -1;
                }
                yout = xout + len;
                memcpy ((char *)xout, (char *)xptr, len*sizeof(CSW_F));
                memcpy ((char *)yout, (char *)yptr, len*sizeof(CSW_F));
                istat = PutInOutputStruct (xout, yout, len);
                if (istat == -1) {
                    return -1;
                }
                first = last + 1;
            }
            else {
                first = last + 1;
            }
        }
    }

    return 1;

}  /*  end of private OutputContours function  */






/*
  ****************************************************************

                  P u t I n O u t p u t S t r u c t

  ****************************************************************

    Put a contour segment and its attributes into an output structure.

*/

int CSWConCalc::PutInOutputStruct (CSW_F *xptr, CSW_F *yptr, int len)
{
    CSW_F          zlev;
    double         xt, yt, x1, y1, x2, y2, dx, dy, a1, a2, ang;
    int            col, row;

/*
    Extend the number of output structures if needed
*/
    if (NconData >= MaxConData) {

        MaxConData += LINE_BUFFER_CHUNK;
MSL
        ContourData = (COntourOutputRec *)csw_Realloc
                          (ContourData, MaxConData * sizeof(COntourOutputRec));
        if (!ContourData) {
            return -1;
        }
    }

/*
    OutputDownhill has the node index of the downhill node
    from the initial contour crossing.  This must be compared
    to the contour line to see if it is left or right of the
    line.  The first segment of the line is used for comparison.
*/
    x1 = (double)xptr[0];
    y1 = (double)yptr[0];
    x2 = (double)xptr[1];
    y2 = (double)yptr[1];
    col = OutputDownhill % Ncol;
    row = OutputDownhill / Ncol;

    xt = Xmin + col * Xspace;
    yt = Ymin + row * Yspace;

    dx = x2 - x1;
    dy = y2 - y1;
    a1 = atan2(dy, dx);
    if (a1 < 0.0) {
        a1 += TWOPI;
    }
    dx = xt - x1;
    dy = yt - y1;
    a2 = atan2(dy, dx);
    while (a2 < a1) {
        a2 += TWOPI;
    }

    ang = a2 - a1;
    if (ang <= PI) {
        OutputDownhill = 1;
    }
    else {
        OutputDownhill = -1;
    }

/*
    Put the points, number of points and the current
    values of the private output attributes into the
    structure.
*/
    zlev = OutputZlev;
    if (DoLogContours == 1) {
        zlev = (CSW_F)(pow ((double)ContourLogBase, (double)OutputZlev));
    }
    ContourData[NconData].x = xptr;
    ContourData[NconData].y = yptr;
    ContourData[NconData].npts = len;
    ContourData[NconData].zvalue = zlev + (CSW_F)GridShift;
    ContourData[NconData].major = (char)OutputMajor;
    ContourData[NconData].downhill = (char)OutputDownhill;
    ContourData[NconData].closure = (char)OutputClosure;
    ContourData[NconData].local_min_max = 0;
    ContourData[NconData].expect_double = 0;
    ContourData[NconData].faultflag = 0;

    FormatNumber (OutputZlev + (CSW_F)GridShift, ContourLogBase,
                  ContourData[NconData].text);

/*
    Increment the output counter and return success.
*/
    NconData++;

    return 1;

}  /*  end of private PutInOutputStruct function  */





/*
  ****************************************************************

                   S e t S m o o t h F l a g s

  ****************************************************************

    If smoothing is needed, check each cell to see if bicubic
  subgrids are needed.  The SmoothFlags array is set to one if
  the subgrid is needed or to zero if not needed.

*/

int CSWConCalc::SetSmoothFlags (void)
{
    int            i, j, offset, istat, k;

    for (i=0; i<Nrow; i++) {
        offset = i * Ncol;
        for (j=0; j<Ncol; j++) {
            k = offset + j;
            istat = CheckForBicub (i, j);
            SmoothFlags[k] = (MYSIGNED char)istat;
            if (i < Nrow-1  &&  j < Ncol-1) {
                if (FaultedFlag == 1) {
                    if (FaultCellCrossings  &&  FaultCellCrossings[k] == 0) {
                        istat = SaddleCheck
                            (Grid[k], Grid[k+1], Grid[k+Ncol+1], Grid[k+Ncol]);
                        if (istat == 1) {
                            SmoothFlags[k] = (MYSIGNED char)-1;
                        }
                    }
                }
                else {
                    istat = SaddleCheck
                        (Grid[k], Grid[k+1], Grid[k+Ncol+1], Grid[k+Ncol]);
                    if (istat == 1) {
                        SmoothFlags[k] = (MYSIGNED char)-1;
                    }
                }
            }
        }
    }

    return 1;

}  /*  end of private SetSmoothFlags function  */






/*
  ****************************************************************

                    C h e c k F o r B i c u b

  ****************************************************************

    Return 1 if bicubic interpolation is needed inside a grid cell
  or zero if no interpolation is needed.  If the center of the
  cell is nearly the same from averaging the cell corners or from
  averaging the corners of the 3 by 3 cell group that is centered
  on the specified cell, then the surface is planar enough to not
  need contoursmoothing.

*/

int CSWConCalc::CheckForBicub (int i, int j)
{
    int                  i1, i2, j1, j2;
    int                  offset, n, m, k;
    CSW_F                z1, z2, a1, a2, dz, zcrit, zcrit_save, zc1, zc2, zc3;
    CSW_F                cnull;
    double               dtmp;

/*
    If there are less than 4 columns and rows, always do bilinear.
*/
    if (Ncol < 4  ||  Nrow < 4) return 0;

/*
    If there is a fault in the cell, never do bicubic.  If the cell
    is close to a fault, but doesn't actually contain a fault,
    always do bicubic.
*/
    if (FaultedFlag) {
        k = i * Ncol + j;
        if (FaultCellCrossings  &&  FaultCellCrossings[k] != 0) {
            return 0;
        }
        if (ClosestFault  &&  ClosestFault[k] < 2) {
            return 1;
        }
    }

    dtmp = (double)FLT_MIN;
    dtmp = sqrt(dtmp);
    dtmp *= 1000.0;
    dtmp *= BicubCutoff;
    if ((double) (Zmax - Zmin) < dtmp) return 0;

/*
    The critical "curvature" to trigger smoothing through
    a cell is large for small smoothing factors and much smaller
    for large smoothing factors.  The default smoothing factor
    of three produces a critical curvature of about 2 percent
    of the total range of z in the grid.
*/
    z1 = (CSW_F)ContourSmoothing;
    z1 *= z1;
    z1 /= 2.0f;
    if (z1 < 1.0f) z1 = 1.0f;
    zcrit = (Zmax - Zmin) / BicubCutoff;
    zcrit /= z1;
    zcrit_save = zcrit;
    zcrit *= zcrit;

/*
    indices of 4x4 subgrid centered on the specified cell
*/
    i1 = i - 1;
    if (i1 < 0) i1 = 0;
    j1 = j - 1;
    if (j1 < 0) j1 = 0;
    i2 = i1 + 3;
    if (i2 >= Nrow) {
        i2 = Nrow - 1;
        i1 = i2 - 3;
    }
    j2 = j1 + 3;
    if (j2 >= Ncol) {
        j2 = Ncol - 1;
        j1 = j2 - 3;
    }
    if (j1 < 0  ||  i1 < 0) {
        return 0;
    }

/*
    Find local data range and adjust the critical
    z range if needed.
*/
    cnull = ContourNullValue / 10.0f;
    zc1 = 1.e30f;
    zc2 = -1.e30f;
    for (n=i1; n<=i2; n++) {
        offset = n * Ncol;
        for (m = j1; m<=j2; m++) {
            k = offset + m;
            if (NoNullGrid[k] > cnull)
                return 0;
            if (NoNullGrid[k] < zc1) zc1 = NoNullGrid[k];
            if (NoNullGrid[k] > zc2) zc2 = NoNullGrid[k];
        }
    }
    zc3 = zc2 - zc1;
    zc3 /= z1;
    if (zc3 < zcrit_save) return 0;
    zc3 /= ContourSmoothing;
    zc3 /= ContourSmoothing;
    zc3 *= zc3;

    if (zc3 < zcrit) zcrit = zc3;

/*
    Compare center estimates from averaging the corners
    of the local cell.
*/
    i1 = i;
    if (i >= Nrow-1) i1 = Nrow - 2;
    j1 = j;
    if (j >= Ncol-1) j1 = Ncol - 2;
    z1 = NoNullGrid[i1*Ncol+j1];
    z2 = NoNullGrid[(i1+1)*Ncol+j1+1];
    if (z1 > cnull  ||  z2 > cnull) return 0;
    a1 = (z1 + z2) / 2.0f;
    z1 = NoNullGrid[i1*Ncol+j1+1];
    z2 = NoNullGrid[(i1+1)*Ncol+j1];
    if (z1 > cnull  ||  z2 > cnull) return 0;
    a2 = (z1 + z2) / 2.0f;
    dz = a1 - a2;
    dz *= dz;
    if (dz > zcrit) return 1;

/*
    indices of 3x3 subgrid centered on the specified cell
*/
    i1 = i - 1;
    if (i1 < 0) i1 = 0;
    j1 = j - 1;
    if (j1 < 0) j1 = 0;
    i2 = i1 + 3;
    if (i2 >= Nrow) {
        i2 = Nrow - 1;
        i1 = i2 - 3;
    }
    j2 = j1 + 3;
    if (j2 >= Ncol) {
        j2 = Ncol - 1;
        j1 = j2 - 3;
    }
    if (j1 < 0  ||  i1 < 0) {
        return 0;
    }

/*
    Compare center elevations based on the lower left
    to upper right diagonal.
*/
    z1 = NoNullGrid[i1 * Ncol + j1];
    z2 = NoNullGrid[i2 * Ncol + j2];
    if (z1 > cnull  ||  z2 > cnull) return 0;
    a1 = (z1 + z2) / 2.0f;
    z1 = NoNullGrid[(i1+1)*Ncol+j1+1];
    z2 = NoNullGrid[(i1+2)*Ncol+j2-1];
    if (z1 > cnull ||  z2 > cnull) return 0;
    a2 = (z1 + z2) / 2.0f;
    dz = a1 - a2;
    dz *= dz;
    if (dz > zcrit) return 1;

/*
    Compare center elevations based on the upper left
    to lower right diagonal.
*/
    z1 = NoNullGrid[i2 * Ncol + j1];
    z2 = NoNullGrid[i1 * Ncol + j2];
    if (z1 > cnull  ||  z2 > cnull) return 0;
    a1 = (z1 + z2) / 2.0f;
    z1 = NoNullGrid[(i2-1)*Ncol+j1+1];
    z2 = NoNullGrid[(i1+1)*Ncol+j2-1];
    if (z1 > cnull  ||  z2 > cnull) return 0;
    a2 = (z1 + z2) / 2.0f;
    dz = a1 - a2;
    dz *= dz;
    if (dz > zcrit) return 1;

/*
    If neither diagonal had a large difference,
    return zero.
*/
    return 0;

}  /*  end of private CheckForBicub function  */





/*
  ****************************************************************

                 C o n v e r t T o L o g B a s e

  ****************************************************************

    Convert the current private Grid in memory to the current log
  base.  If any zero or negative values are encountered in grid,
  an error is returned.

*/

int CSWConCalc::ConvertToLogBase (CSW_F *grid)
{
    double        lnbase, zt;
    int           i;

    if (ContourLogConvert == 0) return 1;

    lnbase = log ((double)ContourLogBase);
    for (i=0; i<Ncol*Nrow; i++) {
        zt = grid[i];
        if (zt >= ContourNullValue) continue;
        if (zt <= 0.0f) {
            return -1;
        }
        zt = log(zt) / lnbase;
        grid[i] = (CSW_F)zt;
    }

    if (FirstContour > 0.0f  &&  LastContour > 0.0f  &&  FirstContour != LastContour) { /*lint !e777*/
        FirstContour = (CSW_F)(log ((double)FirstContour) / lnbase);
        LastContour = (CSW_F)(log ((double)LastContour) / lnbase);
    }

    return 1;

}  /*  end of ConvertToLogBase function  */




/*
 ****************************************************************

                c o n _ f o r m a t _ n u m b e r

 ****************************************************************

*/

int CSWConCalc::con_format_number
            (CSW_F value, CSW_F logbase, char *buf)
{
    int istat;

    istat = FormatNumber (value, logbase, buf);
    return istat;
}


/*
  ****************************************************************

                  F o r m a t N u m b e r

  ****************************************************************

    Encode a linear or logarithmic number into a text buffer for
  subsequent use in a contour text structure.  This is used for
  contour labels.

*/

int CSWConCalc::FormatNumber
         (CSW_F value, CSW_F logbase, char *buf)
{
    char         fmt[20], fmt2[10];
    int          ndec, ilog;
    CSW_F        fval;
    double       dvalue, fudge;

    ndec = 7;
    dvalue = value;
    fval = value - (CSW_F)GridShift;
    if (fval < 0.0f) fval = -fval;

    if (fval == 0.0f) {
        ilog = 1;
    }
    else {
        ilog = (int)log10 ((double)fval);
    }
    ilog--;
    if (fval < 0.5f) {
        ndec = -ilog + 2;
    }
    else if (fval < 5.0f) {
        ndec = 3;
    }
    else if (fval < 50.0f) {
        ndec = 2;
    }
    else if (fval < 500.0f) {
        ndec = 1;
    }
    else {
        ndec = 0;
    }
    if (ndec > 7) ndec = 7;
    fudge = pow (10.0, (double)(-ndec * 2));
    if (fudge > 1.e-7) fudge = 1.e-7;
    if (fudge < 1.e-12) fudge = 1.e-12;

    if (logbase > MIN_LOG_BASE) {
        ilog = (int) (fval + .01f);
        if (fval - ilog < .05f) {
            fval = (CSW_F)ilog;
            if (value < 0.0f) fval = -fval;
        }
        else {
            fval = value;
        }
        dvalue = pow ((double)logbase, (double)fval);
        if (fval < -4  ||  fval > 6) {
            strcpy (fmt, "%8.1e");
            if (logbase < 6.0f) strcpy (fmt, "%8.2e");
            if (logbase < 3.0f) strcpy (fmt, "%8.3e");
        }
        else {
            strcpy (fmt, "%10.5f");
        }
    }
    else {
        if (dvalue > 0.0) {
            dvalue += fudge;
        }
        else {
            dvalue -= fudge;
        }
        strcpy (fmt, "%10.");
        sprintf (fmt2, "%d", ndec);
        strcat (fmt, fmt2);
        if (ndec >= 6) {
            strcat (fmt, "g");
        }
        else {
            strcat (fmt, "f");
        }
    }
    if (dvalue > 9000000  ||  dvalue < -9000000) {
        strcpy (fmt, "%10.2e");
    }

    sprintf (buf, fmt, dvalue);

    csw_StrLeftJust (buf);

    CleanupZeros (buf);

    if (dvalue <= -1.0  ||  dvalue >= 1.0) {
        if (buf[0] == '0') {
            if (buf[1] != '\0') {
                buf[0] = ' ';
                csw_StrLeftJust (buf);
            }
        }

        if (!strcmp (buf, "-0")) {
            strcpy (buf, "0");
        }
    }

/*
 * clean up -0 if it is found.
 */
    if (strlen (buf) == 2) {
        if (buf[0] == '-'  &&  buf[1] == '0') {
            sprintf (buf, "0");
        }
    }

    return 1;

}  /*  end of private FormatNumber function  */





/*
  ****************************************************************

                     C l e a n u p Z e r o s

  ****************************************************************

    Delete repeating decimal zeros at the end of a number and adjust the
  significant digits appropriately.

*/

void CSWConCalc::CleanupZeros (char *buf)
{
    char      clast, *cdot;
    int       i;

    cdot = strchr (buf, '.');
    if (!cdot) {
        return;
    }

    cdot++;
    if (*cdot == '\0') {
        return;
    }

    i = strlen (buf) - 1;

    clast = buf[i];
    if (clast != '0') return;

    while (buf[i-1] == clast) {
        buf[i] = '\0';
        i--;
    }

    if (buf[i-1] == '.') {
        buf[i-1] = '\0';
        return;
    }

    buf[i] = '\0';

    return;

}  /*  end of private CleanupZeros function  */




/*
    The remainder of the file has functions for inter contour color
    fills.  These functions use the same grid geometry private variables
    and some of the contour calculation options, so they are here instead
    of in a file of their own.
*/


/*
  ****************************************************************

            c o n _ b u i l d _ c o l o r _ b a n d s

  ****************************************************************

    Given a contour interval, first and last contours, return arrays
  of low and high sides of color bands suitable for use in the
  con_set_color_bands function.  The actual color numbers are not
  generated here.  The application must

*/

int CSWConCalc::con_build_color_bands
        (CSW_F cint,CSW_F first,CSW_F last, int maxband,
        CSW_F *low,CSW_F *high, int *nband)
{
    int             istat, n, itmp, major;
    CSW_F           conint, base, cmin, zt, zt2;
    CSW_F           tmp[2];

/*
    obvious errors
*/
    if (first > last) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (maxband < 1) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    if (!low  ||  !high  || !nband) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    special case for first nearly equal to last
*/
    tmp[0] = first;
    tmp[1] = last;
    istat = csw_CheckRange (tmp, 2);

    if (istat == 0) {
        *nband = 1;
        zt = first / 10.0f;
        if (zt < 0.0) zt = -zt;
        if (zt < 1.0f) zt = 1.0f;
        low[0] = first - zt;
        high[0] = last + zt;
        return 1;
    }

/*
    For some reason, my debugger will not let me
    examine the parameter cint, so I have to use
    a local auto variable assignment.
*/
    conint = cint;

/*
    Calculate automatic contour interval.
*/
    if (conint <= 0.0f) {
        zt = (last - first) / 20.0f;
        zt2 = (CSW_F)(log10 ((double)zt));
        zt2 = (CSW_F)(floor ((double)zt2));
        conint = (CSW_F)(pow ((double)10.0f, (double)zt2));
        major = 5;
        if (conint < zt/8.0f) {
            conint *= 10.0f;
            major = 5;
        }
        else if (conint < zt/4.5f) {
            conint *= 5.0f;
            major = 4;
        }
        else if (conint < zt/3.5f) {
            conint *= 4.0f;
            major = 5;
        }
        else if (conint < zt/1.5f) {
            conint *= 2.0f;
            major = 5;
        }
        conint *= major;
        if (conint <= 0.0) {
            conint = 1.0f;
        }
    }

    base = conint - ContourBaseValue;

/*
    Make a base contour less than or equal to
    first that is an even multiple of the contour interval.
*/
    itmp = (int)(first / conint);
    zt = itmp * conint;
    while (zt > first) {
        zt -= conint;
    }
    cmin = zt;

/*
    Adjust first contour for the contour base.
*/
    cmin -= base;
    while (cmin + conint < first) {
        cmin += conint;
    }

/*
    Load up the array from first to last color band.
*/
    n = 0;
    zt = cmin;
    for (;;) {
        low[n] = zt;
        high[n] = zt + conint;
        n++;
        if (n == maxband) break;
        zt += conint;;
        if (zt >= last) break;
    }

    low[0] = first;
    if (zt >= last) high[n-1] = last;
    *nband = n;

    return 1;

}  /*  end of function con_build_color_bands  */







/*
  **************************************************************************

                   c o n _ s e t _ c o l o r _ b a n d s

  **************************************************************************

    Set the low, high and color values for filled contours on the grid.

*/

int CSWConCalc::con_set_color_bands
              (CSW_F *lowlist,CSW_F *highlist,
               int *colorlist, int nlist)
{
    int              i, j, n, i1, i2;
    CSW_F            dz, z1, z2;

/*
    check obvious errors
*/
    if (lowlist == NULL  ||  highlist == NULL  ||
        colorlist == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (nlist < 0  ||  nlist > MAX_BANDS) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    transfer lists to private members
*/
    NColorBands = nlist;
    n = nlist * sizeof(CSW_F);
    memcpy (ColorBandLow, lowlist, n);
    memcpy (ColorBandHigh, highlist, n);
    n = nlist * sizeof(int);
    memcpy (ColorBandColor, colorlist, n);

/*
    find min and max z levels in the band lists
*/
    z1 = 1.e30f;
    z2 = -1.e30f;
    for (i=0; i<nlist; i++) {
        if (lowlist[i] < z1) z1 = lowlist[i];
        if (highlist[i] > z2) z2 = highlist[i];
    }

/*
    error if no range exists in the band values.
*/
    if (z1 >= z2) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    ColorZmin = z1;

/*
    Split the range up into MAX_LOOKUP intervals.
    The lookup table is used for fast conversion from
    a z value to a color associated with the z value.
*/
    dz = (z2 - z1) / (CSW_F)(MAX_LOOKUP - 1);
    ColorZdelta = dz;

/*
    initialize the lookup table to all blank colors.
*/
    for (i=0; i<MAX_LOOKUP; i++) {
        ColorLookup[i] = (COnColor)NO_COLOR_FLAG;
    }

/*
    Fill in the lookup table with the index of the list
    element that bounds the point in the table.
*/
    for (i=0; i<nlist; i++) {
        i1 = (int) ((lowlist[i] - z1) / dz);
        i2 = (int) ((highlist[i] - z1) / dz);
        if (i1 < 0) i1 = 0;
        if (i2 > MAX_LOOKUP-1) i2 = MAX_LOOKUP - 1;
        for (j=i1; j<=i2; j++) {
            ColorLookup[j] = (COnColor)i;
        }
    }

    return 1;

}  /*  end of con_set_color_bands function  */




/*
  **************************************************************************

                   c o n _ s e t _ z e r o _ f i l l _ c o l o r

  **************************************************************************

    Set the zero fill color.

*/

int CSWConCalc::con_set_zero_fill_color (int color)
{

    ZeroFillColor = color;
    TempZeroFillColor = color;

    return 1;

}  /*  end of con_set_zero_fill_color function  */




/*
  **************************************************************************

                  c o n _ c a l c _ c o l o r _ f i l l s

  **************************************************************************

    Calculate a set of polygons that can be used to draw filled contours
  through the specified grid.  The definition of the color bands must have
  been set up using con_set_color_bands prior to calling this function.

*/

int CSWConCalc::con_calc_color_fills
     (CSW_F *gridin, int ncol, int nrow,
     CSW_F xmin,CSW_F ymin,CSW_F xmax,CSW_F ymax,
     CSW_F scale, COntourFillRec **fills, int *nfills,
      COntourCalcOptions *options)
{
    int                  i, j, j0, k, n, offset, istat;
    COnColor             *cband;
    CSW_F                xpoly[MAX_FILL_POLY], ypoly[MAX_FILL_POLY],
                         null, xt1, yt1, yt2, yfudge;
    CSW_F                cnull, *grid, *tgrid;
    int                  icolor, nflag, ncnew, nrnew, fillflag;
    char                 *infault;

/*
    check for obvious errors
*/
    if (!gridin) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (ncol < MIN_COLS_CON_CALC  ||  nrow < MIN_ROWS_CON_CALC) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (xmin >= xmax  ||  ymin >= ymax) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    if (!fills  ||  !nfills) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

    if (NColorBands < 1) {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

    if (grd_utils_ptr->grd_simulation()) {
        return -1;
    }

    grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (grid == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    memcpy (grid, gridin, ncol * nrow * sizeof(CSW_F));

    tgrid = NULL;

/*
    initialize output stuff
*/
    FillPolys = NULL;
    NFillPolys = 0;
    MaxFillPolys = 0;
    *fills = NULL;
    *nfills = 0;
    FirstDetail = 1;

/*
    if the grid is flat and has no null values, return a
    single rectangle in the fills array.
*/
    istat = CheckGridRange (grid, ncol*nrow, &nflag);
    if (istat == 0) {
        if (nflag == 0) {
            xpoly[1] = xmin;
            ypoly[1] = ymin;
            xpoly[3] = xmax;
            ypoly[3] = ymax;
            i = SearchRawColorBands (grid[0]);
            OutputContourFill (xpoly, ypoly, 4, i, 1);

            if (SubGrid) csw_Free (SubGrid);
            if (SubCband) csw_Free (SubCband);
            SubGrid = NULL;
            SubCband = NULL;

            *fills = FillPolys;
            *nfills = NFillPolys;

            csw_Free (grid);

            return 1;
        }
        ContourThicknessFlag = 0;
    }

/*
    Use the fill precision and thickness flag from the options structure
    if the structure pointer is not NULL.
*/
    StepGridFlag = OptStepGridFlag;
    FillPrecision = OptFillPrecision;
    ContourThicknessFlag = OptContourThicknessFlag;
    FaultedFlag = OptFaultedFlag;
    ContourSmoothing = OptContourSmoothing;
    ContourInFaultsFlag = OptContourInFaultsFlag;
    BaseGridValue = -1.e30f;
    TopGridValue = 1.e30f;
    if (options) {
        FillPrecision = options->fill_precision;
        ContourThicknessFlag = options->thickness_flag;
        StepGridFlag = options->step_flag;
        FaultedFlag = options->faulted_flag;
        ContourSmoothing = options->smoothing;
        ContourInFaultsFlag = options->contour_in_faults_flag;
        BaseGridValue = options->base_grid_value;
        TopGridValue = options->top_grid_value;
    }
    if (FillPrecision < 0.001f) FillPrecision = 0.001f;
    if (ContourThicknessFlag > 2) ContourThicknessFlag = 0;
    if (ContourThicknessFlag < 0) ContourThicknessFlag = 0;

    TempZeroFillColor = ZeroFillColor;

    if (StepGridFlag) {
        FaultedFlag = 0;
    }

    infault = NULL;
    if (FaultedFlag) {
        grd_fault_ptr->grd_set_fault_option (CON_CONTOUR_IN_FAULTS,
                              ContourInFaultsFlag, 0.0f, NULL);
        istat = con_resample_for_smoothing (grid, NULL, ncol, nrow,
                                            xmin, ymin, xmax, ymax,
                                            &tgrid, NULL, &ncnew, &nrnew);
        if (istat == -1) {
            return -1;
        }

        if (istat == 1) {
            ncol = ncnew;
            nrow = nrnew;
            csw_Free (grid);
            grid = tgrid;
        }

        istat = grd_fault_ptr->grd_build_fault_indices (grid, ncol, nrow,
                                         xmin, ymin, xmax, ymax);
        if (istat == -1) {
            csw_Free (grid);
            return -1;
        }
        istat = grd_fault_ptr->con_faulted_smoothing (3);
        if (istat == -1) {
            csw_Free (grid);
            return -1;
        }
        infault = (char *)csw_Malloc (ncol * nrow * sizeof(char));
        if (!infault) {
            csw_Free (grid);
            return -1;
        }
        istat = grd_fault_ptr->con_mask_inside_faults (infault);
        if (istat == -1) {
            csw_Free (grid);
            csw_Free (infault);
            return -1;
        }
    }
    else {
        istat = con_resample_for_smoothing (grid, NULL, ncol, nrow,
                                            xmin, ymin, xmax, ymax,
                                            &tgrid, NULL, &ncnew, &nrnew);
        if (istat == -1) {
            return -1;
        }

        if (istat == 1) {
            ncol = ncnew;
            nrow = nrnew;
            csw_Free (grid);
            grid = tgrid;
        }

    }

/*
    assign the parameters to class private variables
*/
    Ncol = ncol;
    Nrow = nrow;

    Xmin = xmin;
    Ymin = ymin;
    Xmax = xmax;
    Ymax = ymax;

    Grid = grid;
    NoNullGrid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (NoNullGrid == NULL) {
        FreeMem ();
        grd_utils_ptr->grd_set_err(1);
        return -1;
    }
    memcpy (NoNullGrid, grid, ncol * nrow * sizeof(CSW_F));
    grd_utils_ptr->grd_fill_nulls_new (NoNullGrid, Ncol, Nrow, ContourNullValue,
                        NULL, NULL, 0);

    Xspace = (Xmax - Xmin) / (Ncol - 1);
    Yspace = (Ymax - Ymin) / (Nrow - 1);

    null = ContourNullValue / 100.f;

    Zmin = 1.e30f;
    Zmax = -1.e30f;
    for (i=0; i<ncol*nrow; i++) {
        if (grid[i] < null) {
            if (grid[i] < Zmin) Zmin = grid[i];
            if (grid[i] > Zmax) Zmax = grid[i];
        }
    }
    if (BaseGridValue < -1.e20) {
        BaseGridValue = Zmin;
    }
    if (TopGridValue > 1.e20) {
        TopGridValue = Zmax;
    }

    SubGridSize = 1.0f / FillPrecision;

/*
    If the ContourThicknessFlag is set, any very near zero values
    are set to nullval, and then the null values are filled in
    by extrapolation of gradients.
*/
    FirstContour = -1.e30f;
    LastContour = 1.e30f;

    if (ContourThicknessFlag == CON_POSITIVE_THICKNESS) {
        BaseGridValue = 0.0;
    }
    if (ContourThicknessFlag == CON_NEGATIVE_THICKNESS) {
        TopGridValue = 0.0;
    }

    if (BaseGridValue > -1.e20  ||  TopGridValue < 1.e20) {
        fillflag = 1;
        if (TopGridValue < 1.e20) fillflag = 2;
        cnull = ContourNullValue / 10.0f;
        tgrid = (CSW_F *)csw_Malloc (Ncol * Nrow * sizeof(CSW_F));
        if (!tgrid) {
            FreeMem ();
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        for (i=0; i<Nrow; i++) {
            offset = i * Ncol;
            for (j=0; j<Ncol; j++) {
                k = offset + j;
                tgrid[k] = grid[k];
                if (grid[k] >= cnull) continue;

            /*
             * Bug 7923, the following used to check for < TopGridValue and set tgrid to zero
             * if either plateau check was violated.  I think this looks better now.
             */
                if (BaseGridValue > -1.e20) {
                    if (grid[k] < BaseGridValue) {
                        tgrid[k] = BaseGridValue;
                    }
                }
                if (TopGridValue > -1.e20) {
                    if (grid[k] > TopGridValue) {
                        tgrid[k] = TopGridValue;
                    }
                }
            }
        }

        istat = grd_utils_ptr->grd_fill_nulls_new (tgrid, Ncol, Nrow, ContourNullValue,
                                    NULL, NULL, fillflag);
        if (istat == -1) {
            FreeMem ();
            grd_utils_ptr->grd_set_err (1);
            csw_Free (tgrid);
            return -1;
        }
        if (FirstContour >= LastContour) {
            if (BaseGridValue > -1.e20) FirstContour = BaseGridValue;
            if (TopGridValue < 1.e20) LastContour = TopGridValue;
        }
        else {
            if (ZeroFillColor == -1) {
                if (FirstContour < 0.0f  &&
                    ContourThicknessFlag == CON_POSITIVE_THICKNESS) {
                    FirstContour = 0.0f;
                }
                else if (LastContour > 0.0f  &&
                    ContourThicknessFlag == CON_NEGATIVE_THICKNESS) {
                    LastContour = 0.0f;
                }
            }

        /*
            Bug 6368 addressed here.  If the first or last contour
            is within the thickness range (e.g positive first
            contour and positive thickness) then the ZeroFillColor
            is set to NO_COLOR_FLAG.  I introduce the TempZeroFillColor
            variable so the permanent zero fill color is not changed.
        */
            else {
                if (FirstContour > 0.0f  &&
                    ContourThicknessFlag == CON_POSITIVE_THICKNESS) {
                    TempZeroFillColor = NO_COLOR_FLAG;
                }
                else if (LastContour < 0.0f  &&
                         ContourThicknessFlag == CON_NEGATIVE_THICKNESS) {
                         TempZeroFillColor = NO_COLOR_FLAG;
                }
            }
        }
        csw_Free (Grid);
        grid = tgrid;
        Grid = tgrid;
        tgrid = NULL;
    }

/*
    Calculate number of subgrid cells to use for
    details of grid cells with multiple colors.
*/
    if (scale > 0.0f) {
        xt1 = Xspace / scale;
        SubCols = (int) (xt1 * SubGridSize);
        xt1 = Yspace / scale;
        SubRows = (int) (xt1 * SubGridSize);
        if (SubCols < 1) SubCols = 1;
        if (SubRows < 1) SubRows = 1;
    }
    else {
        SubCols = 5;
        SubRows = 5;
        SubGridSize = 5.0f;
    }

    if (SubCols > MAX_DETAIL - 2) SubCols = MAX_DETAIL - 2;
    if (SubRows > MAX_DETAIL - 2) SubRows = MAX_DETAIL - 2;

/*
    Allocate space for the color band index grid.
    This has the index of the color band which contains
    the z value at the node.  A value in this index grid
    of NO_COLOR_FLAG indicates that no color fill is to
    be made for the area around the node.
*/
MSL
    cband = (COnColor *)csw_Malloc (Ncol*Nrow*sizeof(COnColor));
    if (!cband) {
        grd_utils_ptr->grd_set_err (1);
        if (tgrid) csw_Free (tgrid);
        if (infault) csw_Free (infault);
        FreeMem ();
        return -1;
    }

/*
    Fill in the color band grid.
*/
    for (i=0; i<Ncol*Nrow; i++) {
        if (Grid[i] > null) {
            cband[i] = NO_COLOR_FLAG;
            continue;
        }
        if (Grid[i] < FirstContour  ||  Grid[i] > LastContour) {
            cband[i] = NO_COLOR_FLAG;
            continue;
        }
        cband[i] = FindColorBand (Grid[i]);
    }

/*
    Calculate the polygon fills by traversing each row in
    the grid starting from the lower left.  Adjacent cells
    with the same color are merged into one polygon.  When
    a cell has more than one color, it is subdivided into
    several small rectangles and these are output.
*/
    yfudge = Yspace / 100.0f;
    for (i=0; i<Nrow-1; i++) {

        offset = i * Ncol;
        n = 0;
        yt1 = Ymin + i * Yspace;
        yt2 = yt1 + Yspace + yfudge;
        j0 = 0;

        for (j=0; j<Ncol; j++) {

            k = offset + j;

        /*
            Start a polygon with the vertical side of the
            left edge of the current grid cell if n = 0.
        */
            if (n == 0) {
                xpoly[0] = j * Xspace + Xmin;
                ypoly[0] = yt2;
                xpoly[1] = j * Xspace + Xmin;
                ypoly[1] = yt1;
                n = 2;
                j0 = j;
            }

        /*
            The polygon is generated if the next cell
            has more than one color in it.
        */
            icolor = cband[k];

            if (j < Ncol-1) {
                if (FaultedFlag  &&  ContourInFaultsFlag == 0  &&  infault) {

                    if (infault[k] == 1  &&
                        infault[k+1] == 1  &&
                        infault[k+Ncol] == 1  &&
                        infault[k+Ncol+1] == 1) {
                        if (j > j0) {
                            xpoly[n] = j * Xspace + Xmin;
                            ypoly[n] = yt1;
                            n++;
                            xpoly[n] = j * Xspace + Xmin;
                            ypoly[n] = yt2;
                            n++;
                            xpoly[n] = xpoly[0];
                            ypoly[n] = ypoly[0];
                            n++;
MSL
                            istat = OutputContourFill (xpoly, ypoly, n, icolor, 1);
                            if (istat == -1) {
                                csw_Free (cband);
                                con_free_color_fills (FillPolys, NFillPolys);
                                if (SubGrid) csw_Free (SubGrid);
                                if (SubCband) csw_Free (SubCband);
                                SubGrid = NULL;
                                SubCband = NULL;
                                FillPolys = NULL;
                                NFillPolys = 0;
                                MaxFillPolys = 0;
                                if (tgrid) csw_Free (tgrid);
                                if (infault) csw_Free (infault);
                                FreeMem();
                                return -1;
                            }
                        }
                        n = 0;
                        continue;
                    }

                    if (infault[k] == 1  ||
                        infault[k+1] == 1  ||
                        infault[k+Ncol] == 1  ||
                        infault[k+Ncol+1] == 1) {
                        if (j > j0) {
                            xpoly[n] = j * Xspace + Xmin;
                            ypoly[n] = yt1;
                            n++;
                            xpoly[n] = j * Xspace + Xmin;
                            ypoly[n] = yt2;
                            n++;
                            xpoly[n] = xpoly[0];
                            ypoly[n] = ypoly[0];
                            n++;
MSL
                            istat = OutputContourFill (xpoly, ypoly, n, icolor, 1);
                            if (istat == -1) {
                                csw_Free (cband);
                                con_free_color_fills (FillPolys, NFillPolys);
                                if (SubGrid) csw_Free (SubGrid);
                                if (SubCband) csw_Free (SubCband);
                                SubGrid = NULL;
                                SubCband = NULL;
                                FillPolys = NULL;
                                NFillPolys = 0;
                                MaxFillPolys = 0;
                                if (tgrid) csw_Free (tgrid);
                                if (infault) csw_Free (infault);
                                FreeMem();
                                return -1;
                            }
                        }
                        istat = DoDetailColor (i, j, cband);
                        if (istat == -1) {
                            csw_Free (cband);
                            if (infault) csw_Free (infault);
                            con_free_color_fills (FillPolys, NFillPolys);
                            if (SubGrid) csw_Free (SubGrid);
                            if (SubCband) csw_Free (SubCband);
                            SubGrid = NULL;
                            SubCband = NULL;
                            FillPolys = NULL;
                            NFillPolys = 0;
                            MaxFillPolys = 0;
                            if (tgrid) csw_Free (tgrid);
                            FreeMem();
                            return -1;
                        }
                        n = 0;
                        continue;
                    }
                }

                if (icolor != cband[k+1]  ||
                    icolor != cband[k+Ncol]  ||
                    icolor != cband[k+Ncol+1]) {
                    if (j > j0) {
                        xpoly[n] = j * Xspace + Xmin;
                        ypoly[n] = yt1;
                        n++;
                        xpoly[n] = j * Xspace + Xmin;
                        ypoly[n] = yt2;
                        n++;
                        xpoly[n] = xpoly[0];
                        ypoly[n] = ypoly[0];
                        n++;
MSL
                        istat = OutputContourFill (xpoly, ypoly, n, icolor, 1);
                        if (istat == -1) {
                            csw_Free (cband);
                            con_free_color_fills (FillPolys, NFillPolys);
                            if (SubGrid) csw_Free (SubGrid);
                            if (SubCband) csw_Free (SubCband);
                            SubGrid = NULL;
                            SubCband = NULL;
                            FillPolys = NULL;
                            NFillPolys = 0;
                            MaxFillPolys = 0;
                            if (tgrid) csw_Free (tgrid);
                            if (infault) csw_Free (infault);
                            FreeMem();
                            return -1;
                        }
                    }
                    istat = DoDetailColor (i, j, cband);
                    if (istat == -1) {
                        csw_Free (cband);
                        con_free_color_fills (FillPolys, NFillPolys);
                        if (SubGrid) csw_Free (SubGrid);
                        if (SubCband) csw_Free (SubCband);
                        SubGrid = NULL;
                        SubCband = NULL;
                        FillPolys = NULL;
                        NFillPolys = 0;
                        MaxFillPolys = 0;
                        if (tgrid) csw_Free (tgrid);
                        if (infault) csw_Free (infault);
                        FreeMem();
                        return -1;
                    }
                    n = 0;
                    continue;
                }
            }

        /*
            Generate the polygon if the end of the row is encountered.
        */
            else if (j > j0) {

                xpoly[n] = Xmax;
                ypoly[n] = yt1;
                n++;
                xpoly[n] = Xmax;
                ypoly[n] = yt2;
                n++;
                xpoly[n] = xpoly[0];
                ypoly[n] = ypoly[0];
                n++;
MSL
                istat = OutputContourFill (xpoly, ypoly, n, icolor, 1);
                if (istat == -1) {
                    csw_Free (cband);
                    con_free_color_fills (FillPolys, NFillPolys);
                    if (SubGrid) csw_Free (SubGrid);
                    if (SubCband) csw_Free (SubCband);
                    SubGrid = NULL;
                    SubCband = NULL;
                    FillPolys = NULL;
                    NFillPolys = 0;
                    MaxFillPolys = 0;
                    if (tgrid) csw_Free (tgrid);
                    if (infault) csw_Free (infault);
                    FreeMem();
                    return -1;
                }
                n = 0;
            }

        }  /*  end of j loop through a row  */

    }  /*  end of i loop through rows  */

    csw_Free (cband);
    if (infault) csw_Free (infault);
    if (SubGrid) csw_Free (SubGrid);
    if (SubCband) csw_Free (SubCband);
    SubGrid = NULL;
    SubCband = NULL;

    *fills = FillPolys;
    *nfills = NFillPolys;

    if (tgrid) csw_Free (tgrid);
    tgrid = NULL;

    FreeMem();

    return 1;

}  /*  end of function con_calc_color_fills  */






/*
  ***************************************************************************

                     F i n d C o l o r B a n d

  ***************************************************************************

    Find the color band index for aCSW_F value.  If the value is outside of
  the cumulative color band limits, return -1.

*/

int CSWConCalc::FindColorBand (CSW_F zval)
{
    int              j, n, i1, i2, color;

    if (zval > ContourNullValue / 100.0f) {
        return NO_COLOR_FLAG;
    }

    if (ZeroFillColor != -1) {
      if (ContourThicknessFlag == CON_POSITIVE_THICKNESS && zval <= 0.0) {
        return TempZeroFillColor;
      }
      if (ContourThicknessFlag == CON_NEGATIVE_THICKNESS && zval >= 0.0) {
        return TempZeroFillColor;
      }
    }

/*
    Find the lookup table entry for the z level.
*/
    n = (int) ((zval - ColorZmin) / ColorZdelta);
    if (n < 0  ||  n >= MAX_LOOKUP) {
        return NO_COLOR_FLAG;
    }

    color = (int)ColorLookup[n];

/*
    If the previous or next lookup table entry is
    different, the input color bands need to be
    used to determine the proper color accurately.
*/
    i1 = n-1;
    i2 = n+1;
    if (i1 < 0) i1 = 0;
    if (i2 > MAX_LOOKUP - 1) i2 = MAX_LOOKUP - 1;

    for (j=i1; j<=i2; j++) {
        if ((int)ColorLookup[j] != color  ||  first == 1) {
            color = SearchRawColorBands (zval);
            first = 0;
            break;
        }
    }

    return color;

}  /*  end of private FindColorBand function  */





/*
  **************************************************************************

                    S e a r c h R a w C o l o r B a n d s

  **************************************************************************

    Search the color band data to find the first band that contains the
  specified z value.  If none is found, return NO_COLOR_FLAG.  If one
  is found, return the index of the band that contains the z level.

*/

int CSWConCalc::SearchRawColorBands (CSW_F zval)
{
    int                i;

    for (i=0; i<NColorBands; i++) {
        if (zval >= ColorBandLow[i]  &&  zval <= ColorBandHigh[i]) {
            return i;
        }
    }

    return NO_COLOR_FLAG;

}  /*  end of private SearchRawColorBands function  */




/*
  *****************************************************************************

                    O u t p u t C o n t o u r F i l l

  *****************************************************************************

    Add a polygon to the FillPolys list.  The list is expanded if needed to
  add the polygon.  When the fill calculations are finished, the FillPolys
  pointer is returned to the calling function.  The calling function must
  csw_Free the space when it is safe to do so.

*/

int CSWConCalc::OutputContourFill
        (CSW_F *x, CSW_F *y, int npts, int index, int rflag)
{
    COntourFillRec    *fptr;
    int               n;

    if (index < 0  ||  index > NColorBands-1) {
        return 1;
    }

/*
    Allocate space if needed.
*/
    if (NFillPolys >= MaxFillPolys) {
        MaxFillPolys += FILL_CHUNK;
        if (FillPolys) {
            FillPolys = (COntourFillRec *)csw_Realloc
              (FillPolys, MaxFillPolys * sizeof(COntourFillRec));
        }
        else {
            FillPolys = (COntourFillRec *)csw_Malloc
              (MaxFillPolys * sizeof(COntourFillRec));
        }
        if (!FillPolys) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

/*
    fill in the structure
*/
    fptr = FillPolys + NFillPolys;
    n = npts * sizeof(CSW_F);

    if (rflag == 0) {
        memcpy ((char *)fptr->x, (char *)x, n);
        memcpy ((char *)fptr->y, (char *)y, n);
        fptr->npts = npts;
    }
    else {
        fptr->x[0] = x[1];
        fptr->y[0] = y[1];
        fptr->x[1] = x[3];
        fptr->y[1] = y[3];
        fptr->x[2] = x[3] - x[1];
        fptr->y[2] = y[3] - y[1];
        fptr->npts = 2;
    }

    fptr->zlow = ColorBandLow[index];
    fptr->zhigh = ColorBandHigh[index];
    fptr->color = ColorBandColor[index];
    fptr->expect_double = 0;

    NFillPolys++;

    return 1;

}  /*  end of private OutputContourFill function  */





/*
  ******************************************************************************

               c o n _ f r e e _ c o l o r _ f i l l s

  ******************************************************************************

    Free the memory for the specified contour fill pointer.  You must use
  this function to csw_Free contour fill memory.  Calling csw_Free directly using the
  contour fill pointer will create memory leaks.

*/

int CSWConCalc::con_free_color_fills (COntourFillRec *list, int nlist)
{

    nlist = nlist;

    if (list == NULL) {
        return 1;
    }

    csw_Free (list);

    return 1;

}  /*  end of private csw_Free_color_fills function  */



#define RETURN_ERROR \
            con_free_color_fills (FillPolys, NFillPolys); \
            if (SubGrid) csw_Free (SubGrid); \
            if (SubCband) csw_Free (SubCband); \
            SubGrid = NULL; \
            SubCband = NULL; \
            FillPolys = NULL; \
            NFillPolys = 0; \
            MaxFillPolys = 0; \
            return -1; \

/*
  ******************************************************************************

                         D o D e t a i l C o l o r

  ******************************************************************************

    Fill in a subgrid for the cell and calculate polygons through the subgrid.

*/

int CSWConCalc::DoDetailColor (int irow, int jcol, COnColor *cband)
{
    CSW_F            x0, y0, xsp, ysp, xh, yh, yt;
    CSW_F     xpos[MAX_DETAIL*MAX_DETAIL], ypos[MAX_DETAIL*MAX_DETAIL];
    CSW_F     xpoly[MAX_FILL_POLY], ypoly[MAX_FILL_POLY];
    CSW_F            yt1, yt2, null, xc[4], yc[4], zc[4];
    int              icolor, icolor2, istat, offset;
    int              i, j, n, k, kf, k2;
    int              hrow[MAX_DETAIL];

    if (FaultedFlag == 0) {
        istat = QuickDetail (irow, jcol, cband);
        if (istat == 1) {
            return 1;
        }
    }

/*
    Initialize some local arrays to satisfy lint warnings
*/
    memset ((char *)hrow, 0, MAX_DETAIL * sizeof(int));
    memset ((char *)xpos, 0, MAX_DETAIL * MAX_DETAIL * sizeof(CSW_F));
    memset ((char *)ypos, 0, MAX_DETAIL * MAX_DETAIL * sizeof(CSW_F));

/*
    Allocate space the first time this is called
    for a particular color fill task.
*/
    if (FirstDetail) {
        n = SubCols * SubRows;
MSL
        SubGrid = (CSW_F *)csw_Malloc (n * sizeof(CSW_F));
        if (!SubGrid) {
            return -1;
        }
MSL
        SubCband = (COnColor *)csw_Malloc (n * sizeof(COnColor));
        if (!SubCband) {
            csw_Free (SubGrid);
            SubGrid = NULL;
            return -1;
        }

        FirstDetail = 0;
    }

/*
    Calculate local subgrid geometry stuff.
*/
    x0 = jcol * Xspace + Xmin;
    y0 = irow * Yspace + Ymin;
    xsp = Xspace / (CSW_F)SubCols;
    ysp = Yspace / (CSW_F)SubRows;
    xh = xsp / 2.0f;
    yh = ysp / 2.0f;

    k = irow * Ncol + jcol;
    k2 = k;

/*
    If there are more than one color and a corner is null,
    return without doing anything.
*/
    null = ContourNullValue / 100.0f;
    if (Grid[k] > null  ||  Grid[k+1] > null  ||
        Grid[k+Ncol] > null  ||  Grid[k+Ncol+1] > null) {
        return 1;
    }

/*
    If a step grid is being filled, fill in the subgrid
    with the closest corner point value from the main grid.
*/
    if (StepGridFlag == 1) {

        for (i=0; i<SubRows; i++) {

            yt = y0 + i * ysp + yh;

            k = i * SubCols;

            for (j=0; j<SubCols; j++) {
                xpos[j] = j * xsp + xh + x0;
                ypos[j] = yt;
            }

            for (j=0; j<SubCols; j++) {
                xc[0] = x0 - xpos[j];
                yc[0] = y0 - ypos[j];
                xc[1] = xc[0] + Xspace;
                yc[1] = yc[0];
                xc[2] = xc[1];
                yc[2] = yc[0] + Yspace;
                xc[3] = xc[0];
                yc[3] = yc[2];
                zc[0] = Grid[k2];
                zc[1] = Grid[k2+1];
                zc[2] = Grid[k2+Ncol+1];
                zc[3] = Grid[k2+Ncol];
                GetClosestPointValue (xc, yc, zc, 4, SubGrid+k+j);
            }
        }
    }

/*
    This is not a step grid, so fill in the subgrid values
    via bicubic interpolation.
*/
    else {

        for (i=0; i<SubRows; i++) {

            yt = y0 + i * ysp + yh;

            k = i * SubCols;

            for (j=0; j<SubCols; j++) {
                xpos[k+j] = j * xsp + xh + x0;
                ypos[k+j] = yt;
            }
        }

        if (FaultedFlag) {
            istat = grd_fault_ptr->con_faulted_bicub_interp_2 (Grid, Ncol, Nrow, 1.e20f, k2,
                                                xpos, ypos, SubGrid, SubRows*SubCols);
        }
        else {
            istat = grd_utils_ptr->grd_bicub_interp (xpos, ypos, SubGrid, SubRows*SubCols, 0.0f,
                                      Grid, Ncol, Nrow, 1,
                                      Xmin, Ymin, Xmax, Ymax,
                                      irow, jcol);
        }
        if (istat == -1) {
            return -1;
        }
        if (FaultedFlag  &&  ContourInFaultsFlag == 0) {
            istat = grd_fault_ptr->con_null_points_in_faults (xpos, ypos, SubGrid, SubRows*SubCols);
            if (istat == -1) {
                return -1;
            }
        }
    }

/*
    Fill in color bands for the subgrid and see if all
    detail cells are the same color.
*/
    if (SubGrid[0] < FirstContour  ||  SubGrid[0] > LastContour) {
        icolor = NO_COLOR_FLAG;
    }
    else {
        icolor = FindColorBand (SubGrid[0]);
    }
    n = 0;
    for (i=0; i<SubRows*SubCols; i++) {
        if (SubGrid[i] < FirstContour  ||  SubGrid[i] > LastContour) {
            SubCband[i] = NO_COLOR_FLAG;
        }
        else {
            SubCband[i] = FindColorBand (SubGrid[i]);
        }
        if (SubCband[i] != icolor) n++;
    }

/*
    If all subgrid cells are the same color, output the grid cell
    as a single rectangle.
*/
    if (n == 0) {
        xpoly[0] = x0;
        ypoly[0] = y0 + Yspace;
        xpoly[1] = x0;
        ypoly[1] = y0;
        xpoly[2] = x0 + Xspace;
        ypoly[2] = y0;
        xpoly[3] = x0 + Xspace;
        ypoly[3] = y0 + Yspace;
        xpoly[4] = x0;
        ypoly[4] = y0 + Yspace;
MSL
        istat = OutputContourFill (xpoly, ypoly, 5, icolor, 1);
        if (istat == -1) {
            RETURN_ERROR
        }
        return 1;
    }

/*
    Put the color value into the hrow array if a row
    is all the same color.  If not, put -1 in hrow.
*/
    icolor2 = -1;;
    for (i=0; i<SubRows; i++) {
        offset = i * SubCols;
        icolor2 = SubCband[offset];
        for (j=0; j<SubCols; j++) {
            k = offset + j;
            if (SubCband[k] != icolor2) {
                hrow[i] = -1;
                icolor2 = -1;
                break;
            }
        }
        if (icolor2 != -1) {
            hrow[i] = icolor2;
        }
    }

/*
    Draw adjacent rows with homogeneous colors.
*/
    xpoly[0] = x0;
    xpoly[1] = x0;
    xpoly[2] = x0 + Xspace;
    xpoly[3] = x0 + Xspace;
    xpoly[4] = x0;
    kf = -1;
    for (i=0; i< SubRows; i++) {
        if (hrow[i] >= 0  &&  kf == -1) {
            kf = i;
            icolor2 = hrow[i];
            continue;
        }
        else if (hrow[i] != icolor2  ||  i == SubRows-1) {
            if (kf >= 0) {
                ypoly[0] = y0 + i * ysp;
                if (i == SubRows-1) ypoly[0] = y0 + Yspace;
                ypoly[1] = y0 + kf * ysp;
                ypoly[2] = y0 + kf * ysp;
                ypoly[3] = ypoly[0];
                ypoly[4] = ypoly[0];
MSL
                istat = OutputContourFill (xpoly, ypoly, 5, icolor2, 1);
                if (istat == -1) {
                    RETURN_ERROR
                }
                if (hrow[i] >= 0) {
                    kf = i;
                    icolor2 = hrow[i];
                }
                else {
                    kf = -1;
                }
            }
        }
    }

/*
    If the last row alone is homogeneous.
*/
    if (kf >= 0) {
        ypoly[0] = y0 + Yspace;
        ypoly[1] = y0 + kf * ysp;
        ypoly[2] = y0 + kf * ysp;
        ypoly[3] = ypoly[0];
        ypoly[4] = ypoly[0];
MSL
        istat = OutputContourFill (xpoly, ypoly, 5, hrow[kf], 1);
        if (istat == -1) {
            RETURN_ERROR
        }
    }

/*
    Scan the subgrid and output rectangles that have
    homogeneous color if the row is not entirely the same color.
*/
    for (i=0; i<SubRows; i++) {

        if (hrow[i] >= 0) continue;

        offset = i * SubCols;
        n = 0;
        yt1 = y0 + i * ysp;
        yt2 = yt1 + ysp;
        n = 0;

        for (j=0; j<SubCols; j++) {

            k = offset + j;

        /*
            If n is zero, start a new polygon.
        */
            if (n == 0) {
                xpoly[0] = x0 + j * xsp;
                ypoly[0] = yt2;
                xpoly[1] = x0 + j * xsp;
                ypoly[1] = yt1;
                n = 2;
            }

        /*
            The polygon is terminated if the next cell
            has a different color or if this is the last
            cell in the subgrid row.
        */
            icolor = SubCband[k];

            if (j == SubCols-1) {
                xpoly[n] = Xspace + x0;
                ypoly[n] = yt1;
                n++;
                xpoly[n] = xpoly[n-1];
                ypoly[n] = yt2;
                n++;
                xpoly[n] = xpoly[0];
                ypoly[n] = ypoly[0];
                n++;
MSL
                istat = OutputContourFill (xpoly, ypoly, n, icolor, 1);
                if (istat == -1) {
                    RETURN_ERROR
                }
                n = 0;
            }

            else if (icolor != SubCband[k+1]) {
                xpoly[n] = (j + 1) * xsp + x0;
                ypoly[n] = yt1;
                n++;
                xpoly[n] = xpoly[n-1];
                ypoly[n] = yt2;
                n++;
                xpoly[n] = xpoly[0];
                ypoly[n] = ypoly[0];
                n++;
MSL
                istat = OutputContourFill (xpoly, ypoly, n, icolor, 1);
                if (istat == -1) {
                    RETURN_ERROR
                }
                n = 0;
            }

        }  /*  end of j loop through columns in a row  */

    }  /*  end of i loop through rows  */

    return 1;

}  /*  end of private DoDetailColor function  */





/*
  ****************************************************************

                    Q u i c k D e t a i l

  ****************************************************************

    If possible, divide a cell into two polygons.  If only two
  colors are needed in the cell and the colors represent continuous
  z values and the cell does not have any blanks or nulls and
  the cell does not require bicubic interpolation, then this
  quick method can be used.

*/

int CSWConCalc::QuickDetail (int irow, int jcol, COnColor *cband)
{
    int              istat, k, nside, side1, side2,
                     c1, c2, c3, c4;
    CSW_F            x0, y0, x1, y1, x2, y2, zt, z1, z2,
                     z3, z4, zlev, xpoly[10], ypoly[10],
                     zmin, zmax;

/*
    Cannot use this if smoothing is needed in this cell.
*/
    if (StepGridFlag == 0) {
        istat = CheckForBicub (irow, jcol);
        if (istat == 1) return 0;
    }

    x0 = jcol * Xspace + Xmin;
    y0 = irow * Yspace + Ymin;

    k = Ncol * irow + jcol;

/*
    Silence some meaningless warnings.
*/
    x1 = x2 = y1 = y2 = 0.0f;
    side1 = side2 = 0;

/*
    Decide if the quick detail can be used.
*/
    c1 = 10000;
    c2 = -10000;
    if ((int)cband[k] < c1) c1 = (int)cband[k];
    if ((int)cband[k] > c2) c2 = (int)cband[k];
    if ((int)cband[k+1] < c1) c1 = (int)cband[k+1];
    if ((int)cband[k+1] > c2) c2 = (int)cband[k+1];
    if ((int)cband[k+1+Ncol] < c1) c1 = (int)cband[k+1+Ncol];
    if ((int)cband[k+1+Ncol] > c2) c2 = (int)cband[k+1+Ncol];
    if ((int)cband[k+Ncol] < c1) c1 = (int)cband[k+Ncol];
    if ((int)cband[k+Ncol] > c2) c2 = (int)cband[k+Ncol];
    if (c2 < 0) c2 = c1;

    if (c1 < 0  ||  c1 > MAX_BANDS) return 0;
    if (c2 < 0  ||  c2 > MAX_BANDS) return 0;
    if (StepGridFlag == 0) {
        if (c2 - c1 > 1) return 0;

        zt = ColorBandLow[c2] - ColorBandHigh[c1];
        if (zt < 0.0f) zt = -zt;
        if (zt > (Zmax - Zmin) / 1000.f) return 0;
    }
    zlev = ColorBandLow[c2];

/*
    Which sides of the cell are intersected by a color change.
*/
    nside = 0;
    z1 = Grid[k];
    z2 = Grid[k+1];
    z3 = Grid[k+Ncol+1];
    z4 = Grid[k+Ncol];

    if (StepGridFlag) {
        c1 = cband[k];
        c2 = cband[k+1];
        c3 = cband[k+Ncol+1];
        c4 = cband[k+Ncol];
        if (c1 != c2) {
            if (c1 != c3  &&  c2 != c3) return 0;
            if (c1 != c4  &&  c2 != c4) return 0;
        }
        if (c1 != c3) {
            if (c1 != c4  &&  c3 != c4) return 0;
        }
        zmin = z1;
        if (z2 < zmin) zmin = z2;
        if (z3 < zmin) zmin = z3;
        if (z4 < zmin) zmin = z4;
        zmax = z1;
        if (z2 > zmax) zmax = z2;
        if (z3 > zmax) zmax = z3;
        if (z4 > zmax) zmax = z4;

        zlev = (zmax +zmin) / 2.0f;
    }

    if ((z1 <= zlev  &&  z2 > zlev)  ||
        (z1 >= zlev  &&  z2 < zlev)) {
        x1 = (zlev - z1) / (z2 - z1) * Xspace + x0;
        y1 = y0;
        nside++;
        side1 = 0;
    }

    if ((z2 <= zlev  &&  z3 > zlev)  ||
        (z2 >= zlev  &&  z3 < zlev)) {
        if (nside == 0) {
            y1 = (zlev - z2) / (z3 - z2) * Yspace + y0;
            x1 = x0 + Xspace;
            side1 = 1;
        }
        else if (nside == 1) {
            y2 = (zlev - z2) / (z3 - z2) * Yspace + y0;
            x2 = x0 + Xspace;
            side2 = 1;
        }
        nside++;
    }

    if ((z3 <= zlev  &&  z4 > zlev)  ||
        (z3 >= zlev  &&  z4 < zlev)) {
        if (nside > 1) {
            return 0;
        }
        if (nside == 0) {
            x1 = Xspace - (zlev - z3) / (z4 - z3) * Xspace + x0;
            y1 = y0 + Yspace;
            side1 = 2;
        }
        else if (nside == 1) {
            x2 = Xspace - (zlev - z3) / (z4 - z3) * Xspace + x0;
            y2 = y0 + Yspace;
            side2 = 2;
        }
        nside++;
    }

    if ((z4 <= zlev  &&  z1 > zlev)  ||
        (z4 >= zlev  &&  z1 < zlev)) {
        if (nside > 1) {
            return 0;
        }
        if (nside == 0) {
            return 0;
        }
        else if (nside == 1) {
            y2 = Yspace - (zlev - z4) / (z1 - z4) * Yspace + y0;
            x2 = x0;
            side2 = 3;
        }
        nside++;
    }

    if (nside != 2) {
        return 0;
    }

/*
    Start at the lower left corner and trace around the cell until
    a crossed side is encountered.  Once the side is found, trace
    to the second crossed side and continue around to the origin.
    Go back to the first crossing and follow the cell to the
    second crossing.
*/
    xpoly[0] = x0;
    ypoly[0] = y0;

/*
    First crossing on bottom.
*/
    if (side1 == 0) {
        xpoly[1] = x1;
        ypoly[1] = y1;
        xpoly[2] = x2;
        ypoly[2] = y2;

    /*
        Second crossing on right.
    */
        if (side2 == 1) {
            xpoly[3] = Xspace + x0;
            ypoly[3] = Yspace + y0;
            xpoly[4] = x0;
            ypoly[4] = y0 + Yspace;
            xpoly[5] = xpoly[0];
            ypoly[5] = ypoly[0];
MSL
            istat = OutputContourFill (xpoly, ypoly, 6, cband[k], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            xpoly[0] = x1;
            ypoly[0] = y1;
            xpoly[1] = x0 + Xspace;
            ypoly[1] = y0;
            xpoly[2] = x2;
            ypoly[2] = y2;
            xpoly[3] = x1;
            ypoly[3] = y1;
MSL
            istat = OutputContourFill (xpoly, ypoly, 4, cband[k+1], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            return 1;
        }

    /*
        Second crossing on top.
    */
        if (side2 == 2) {
            xpoly[3] = x0;
            ypoly[3] = y0 + Yspace;
            xpoly[4] = xpoly[0];
            ypoly[4] = ypoly[0];
MSL
            istat = OutputContourFill (xpoly, ypoly, 5, cband[k], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            xpoly[0] = x1;
            ypoly[0] = y1;
            xpoly[1] = x0 + Xspace;
            ypoly[1] = y0;
            xpoly[2] = Xspace + x0;
            ypoly[2] = Yspace + y0;
            xpoly[3] = x2;
            ypoly[3] = y2;
            xpoly[4] = x1;
            ypoly[4] = y1;
MSL
            istat = OutputContourFill (xpoly, ypoly, 5, cband[k+1], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            return 1;
        }

    /*
        Second crossing on left.
    */
        if (side2 == 3) {
            xpoly[3] = xpoly[0];
            ypoly[3] = ypoly[0];
MSL
            istat = OutputContourFill (xpoly, ypoly, 4, cband[k], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            xpoly[0] = x1;
            ypoly[0] = y1;
            xpoly[1] = x0 + Xspace;
            ypoly[1] = y0;
            xpoly[2] = Xspace + x0;
            ypoly[2] = Yspace + y0;
            xpoly[3] = x0;
            ypoly[3] = y0 + Yspace;
            xpoly[4] = x2;
            ypoly[4] = y2;
            xpoly[5] = x1;
            ypoly[5] = y1;
MSL
            istat = OutputContourFill (xpoly, ypoly, 6, cband[k+1], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            return 1;
        }
    }

/*
    First crossing on right.
*/
    if (side1 == 1) {
        xpoly[1] = x0 + Xspace;
        ypoly[1] = y0;
        xpoly[2] = x1;
        ypoly[2] = y1;
        xpoly[3] = x2;
        ypoly[3] = y2;

    /*
        Second crossing on top.
    */
        if (side2 == 2) {
            xpoly[4] = x0;
            ypoly[4] = y0 + Yspace;
            xpoly[5] = xpoly[0];
            ypoly[5] = ypoly[0];
MSL
            istat = OutputContourFill (xpoly, ypoly, 6, cband[k], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            xpoly[0] = x1;
            ypoly[0] = y1;
            xpoly[1] = Xspace + x0;
            ypoly[1] = Yspace + y0;
            xpoly[2] = x2;
            ypoly[2] = y2;
            xpoly[3] = x1;
            ypoly[3] = y1;
MSL
            istat = OutputContourFill (xpoly, ypoly, 4, cband[k+Ncol+1], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            return 1;
        }

    /*
        Second crossing on left.
    */
        if (side2 == 3) {
            xpoly[4] = xpoly[0];
            ypoly[4] = ypoly[0];
MSL
            istat = OutputContourFill (xpoly, ypoly, 5, cband[k], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            xpoly[0] = x1;
            ypoly[0] = y1;
            xpoly[1] = Xspace + x0;
            ypoly[1] = Yspace + y0;
            xpoly[2] = x0;
            ypoly[2] = y0 + Yspace;
            xpoly[3] = x2;
            ypoly[3] = y2;
            xpoly[4] = x1;
            ypoly[4] = y1;
MSL
            istat = OutputContourFill (xpoly, ypoly, 5, cband[k+Ncol+1], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            return 1;
        }
    }

/*
    First crossing on top.
*/
    if (side1 == 2) {
        xpoly[1] = x0 + Xspace;
        ypoly[1] = y0;
        xpoly[2] = x0 + Xspace;
        ypoly[2] = y0 + Yspace;
        xpoly[3] = x1;
        ypoly[3] = y1;
        xpoly[4] = x2;
        ypoly[4] = y2;

    /*
        Second crossing on left.
    */
        if (side2 == 3) {
            xpoly[5] = xpoly[0];
            ypoly[5] = ypoly[0];
MSL
            istat = OutputContourFill (xpoly, ypoly, 6, cband[k], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            xpoly[0] = x1;
            ypoly[0] = y1;
            xpoly[1] = x0;
            ypoly[1] = y0 + Yspace;
            xpoly[2] = x2;
            ypoly[2] = y2;
            xpoly[3] = x1;
            ypoly[3] = y1;
MSL
            istat = OutputContourFill (xpoly, ypoly, 4, cband[k+Ncol], 0);
            if (istat == -1) {
                RETURN_ERROR
            }
            return 1;
        }
    }

    return 0;

}  /*  end of private QuickDetail function  */

#undef RETURN_ERROR






/*
  ****************************************************************

                       S t e p G r a z e

  ****************************************************************

*/

int CSWConCalc::StepGraze (CSW_F z1, CSW_F z2)
{
    CSW_F         z3, tiny;

    tiny = (Zmax - Zmin) / 1000.f;
    z3 = z2 - z1;
    if (z3 < 0.0) z3 = -z3;

    if (z3 < tiny) {
        return 1;
    }

    return 0;

}  /*  end of private StepGraze function  */




/*
  ****************************************************************

              G e t C l o s e s t P o i n t V a l u e

  ****************************************************************

    Return the value of the closest point in the local data set.

*/

int CSWConCalc::GetClosestPointValue
                      (CSW_F *x, CSW_F *y, CSW_F *z, int n,
                       CSW_F *val)
{
    int             i;
    CSW_F           dt, tiny;

    tiny = 1.e30f;
    *val = 1.e30f;
    for (i=0; i<n; i++) {
        dt = x[i] * x[i] + y[i] * y[i];
        if (dt < tiny) {
            tiny = dt;
            *val = z[i];
        }
    }

    if (*val > 1.e20) {
        return -1;
    }

    return 1;

}  /*  end of private GetClosestPointValue function  */





/*
  ****************************************************************

                  M o r e T h a n T w o S t e p s

  ****************************************************************

    Check a grid cell in a step grid to see if there are more than
  two distinct corner values.  Return 1 if there are more than two
  or return zero otherwise.  This is only used by TraceCellToCell.

*/

int CSWConCalc::MoreThanTwoSteps (int irow, int jcol)
{
    int          k;
    CSW_F        c1, c2, c3, c4;

    if (StepGridFlag) {
        k = irow * Ncol + jcol;
        c1 = Grid[k];
        c2 = Grid[k+1];
        c3 = Grid[k+Ncol+1];
        c4 = Grid[k+Ncol];
        if (StepGraze (c1, c2) == 0) {
            if (StepGraze(c1, c3) == 0  &&  StepGraze(c2, c3) == 0) return 1;
            if (StepGraze(c1, c4) == 0  &&  StepGraze(c2, c4) == 0) return 1;
        }
        if (StepGraze (c1, c3) == 0) {
            if (StepGraze(c1, c4) == 0  &&  StepGraze(c3, c4) == 0) return 1;
        }
    }

    return 0;

}  /*  end of private MoreThanTwoSteps function  */






/*
  ****************************************************************

                   C h e c k G r i d R a n g e

  ****************************************************************

    Given an array of CSW_F numbers, check if the range of the list
  is large enough relative to the mid point value of the list to allow
  the list to be manipulated withCSW_F precision arithmetic.  If
  the range can work with CSW_F precision, 1 is returned.  If not,
  zero is returned.  If the pointer to the values is NULL, -1 is
  returned.  This ignores NULL values.

*/

int CSWConCalc::CheckGridRange (CSW_F *vlist, int npts, int *nflag)
{
    int           i, n;
    CSW_F         v1, v2, nullval;
    double        range, vmid;

    if (vlist == NULL) return -1;

    nullval = ContourNullValue / 100.0f;
    if (nullval < 0.0) nullval = -nullval;
    if (nullval < 1.e10) nullval = 1.e10;

    v1 = 1.e30f;
    v2 = -1.e30f;
    n = 0;
    for (i=0; i<npts; i++) {
        if (vlist[i] < -nullval  ||  vlist[i] > nullval) {
            n++;
            continue;
        }
        if (vlist[i] < v1) v1 = vlist[i];
        if (vlist[i] > v2) v2 = vlist[i];
    }
    range = (double)v2 - (double)v1;
    vmid = ((double)v1 + (double)v2) / 2.0;
    if (vmid < 0.0) vmid = -vmid;

    *nflag = 0;
    if (n > 0) *nflag = 1;

    if (vmid >= 50000.0 * range) {
        return 0;
    }

    Zmin = v1;
    Zmax = v2;
    RangeCheckNeeded = 0;

    return 1;

}  /*  end of private CheckGridRange function  */




/*
  ****************************************************************

                  S h i f t I n p u t G r i d

  ****************************************************************

    If the grid has a small range relative to its absolute values,
  then the grid is shifted to alleviate this.  The shift prevents
  some precision problems in the contour calculations.

*/

int CSWConCalc::ShiftInputGrid (CSW_F *grid, int ncol, int nrow)
{
    int               i, n, ilog, itmp;
    double            dt, d1, d2, d3, range, mid;

    n = ncol * nrow;
    GridShift = 0.0f;

/*
    Make the null values positive if needed
*/
    if (ContourNullValue < 0.0f) {
        for (i=0; i<n; i++) {
            if (grid[i] <= ContourNullValue) {
                grid[i] = -grid[i];
            }
        }
        ContourNullValue = -ContourNullValue;
    }

/*
    Find the range of the grid
*/
    d1 = 1.e30f;
    d2 = -1.e30f;
    for (i=0; i<n; i++) {
        if (grid[i] >= ContourNullValue)
            continue;
        dt = grid[i];
        if (dt < d1)
            d1 = dt;
        if (dt > d2)
            d2 = dt;
    }

/*
    If the range is more than a tiny percent of the absolute values,
    return without further action.
*/
    if (d2 < d1) return 1;

    range = d2 - d1;
    mid = d1 + range / 2.0f;
    if (mid < 0.0)
        mid = -mid;

    if (range > mid / 100.0)
        return 1;

/*
    Find the absolute value that is less than the range and also
    an even multiple of the decade just larger than the range.
*/
    dt = log10 (range);
    ilog = (int)dt;
    ilog++;
    dt = pow ((double)10.0, (double)ilog);

    itmp = (int)(d1 / dt);
    d3 = itmp * dt;
    while (d3 >= d1) {
        d3 -= dt;
    }

/*
    Make sure the shift value is an even multiple of the contour interval.
*/
    if (ContourInterval > 0.0f) {
        itmp = (int)((CSW_F)d3 / ContourInterval);
        d3 = itmp * ContourInterval;
        while (d3 >= d1) {
            d3 -= ContourInterval;
        }
    }

/*
    Shift the grid.
*/
    for (i=0; i<n; i++) {
        if (grid[i] >= ContourNullValue)
            continue;
        grid[i] -= (CSW_F)d3;
    }

    FirstContour -= (CSW_F)d3;
    LastContour -= (CSW_F)d3;

    if (BaseGridValue > -1.e20  &&  BaseGridValue < 1.e20) {
        BaseGridValue -= (CSW_F)d3;
    }
    if (TopGridValue > -1.e20  &&  TopGridValue < 1.e20) {
        TopGridValue -= (CSW_F)d3;
    }

    GridShift = d3;

    return 1;

}

/*  end of private ShiftInputGrid function  */






/*
  ****************************************************************************

                            S a d d l e T w e a k

  ****************************************************************************

    If the cell is a saddle point, and the contour level is greater
  than the spill point of the saddle, negate the direction.

*/

int CSWConCalc::SaddleTweak (int k, int *di, CSW_F zlev)
{
    int             i, j;
    CSW_F           z[4], zt;

/*
    Find the 4 corners of cell k in the original, non adjusted grid.
*/
    z[0] = OriginalGrid[k];
    z[1] = OriginalGrid[k+1];
    z[2] = OriginalGrid[k+1+Ncol];
    z[3] = OriginalGrid[k+Ncol];

/*
    Sort in ascending numerical order.
*/
    for (i=0; i<4; i++) {
        for (j=i+1; j<4; j++) {
            if (z[i] > z[j]) {
                zt = z[i];
                z[i] = z[j];
                z[j] = zt;
            }
        }
    }

/*
    The zlev contour value and the z array of cell corner
    points are all based on the original unadjusted grid,
    so no bit tweaking to recreate float imprecision is
    needed.
*/
    Zt2 = z[1] + z[2];
    Zt2 /= 2.0f;
    zt = Zt2;

    if (zlev > zt) {
        *di = -(*di);
    }

    return 1;

}  /*  end of private SaddleTweak function  */





/*
  ****************************************************************************

           c o n _ r e s a m p l e _ f o r _ s m o o t h i n g

  ****************************************************************************

    Resample the grid in preparation for contour smoothing.

*/

int CSWConCalc::con_resample_for_smoothing
           (CSW_F *grid, char *mask, int ncol, int nrow,
            CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
            CSW_F **gridout, char **maskout,
            int *ncolout, int *nrowout)
{
    char             *localmask;
    CSW_F            *localgrid;
    int              nmult, ncol2, nrow2, istat, forceflag;
    int              maxnodes, nmultmax;

    CSWGrdFault      grd_fault_obj;

    forceflag = 0;
    if (ncol < 0) {
        forceflag = 1;
        ncol = -ncol;
    }

    localgrid = NULL;
    localmask = NULL;

    *ncolout = 0;
    *nrowout = 0;
    *gridout = NULL;
    if (maskout) *maskout = NULL;

    maxnodes = 20000;
    if (FaultedFlag == 1) {
        maxnodes = 100000;
    }

    nmultmax = 3;
    if (FaultedFlag) {
        nmultmax = 5;
    }

    nmult = 2;
    for (;;) {
        ncol2 = (ncol - 1) * nmult + 1;
        nrow2 = (nrow - 1) * nmult + 1;
        if (ncol2 * nrow2 >= maxnodes  ||  nmult >= nmultmax) {
            break;
        }
        nmult++;
    }

    if (forceflag == 0) {
        if (ContourResampleFlag == 0  ||
            ContourSmoothing <= 0  ||
            ncol * nrow >= maxnodes) {
            ncol2 = ncol;
            nrow2 = nrow;
        }
    }
    else {
        if (ncol * nrow >= maxnodes) {
            ncol2 = ncol;
            nrow2 = nrow;
        }
    }

    if (ncol == ncol2  &&  nrow == nrow2) {
        return 0;
    }

    localgrid = (CSW_F *)csw_Malloc (ncol2 * nrow2 * sizeof(CSW_F));
    if (!localgrid) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    if (mask  &&  maskout) {
        localmask = (char *)csw_Malloc (ncol2 * nrow2 * sizeof(CSW_F));
        if (!localmask) {
            csw_Free (localgrid);
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

    if (FaultedFlag == 1) {
        grd_fault_ptr->grd_build_fault_indices (grid, ncol, nrow,
                                 x1, y1, x2, y2);
        istat = grd_fault_ptr->grd_resample_faulted_grid (localgrid, ncol2, nrow2,
                                           x1, y1, x2, y2,
                                           GRD_BICUBIC, 1);
    }
    else {
        istat = grd_arith_ptr->grd_resample_grid (grid, mask, ncol, nrow,
                                   x1, y1, x2, y2,
                                   NULL, 0,
                                   localgrid, localmask, ncol2, nrow2,
                                   x1, y1, x2, y2, GRD_BICUBIC);
    }
    if (istat == -1) {
        csw_Free (localgrid);
        return -1;
    }

/*
 * For a thickness grid, make sure none of the new grid nodes introduced
 * from resampling has a zero or negative value if the old nodes surrounding
 * it do not have zero or negative values.
 */
    if (ContourThicknessFlag != 0) {
        CorrectResampledThickness (localgrid, ncol2, nrow2, nmult);
    }
    *ncolout = ncol2;
    *nrowout = nrow2;

    *gridout = localgrid;
    if (maskout) {
        *maskout = localmask;
    }

#if DEBUG_WRITE_GRID_FILE
    grd_write_file ("debug_rs.grd", NULL,
                    localgrid, NULL, NULL,
                    ncol2, nrow2,
                    (double)x1, (double)y1, (double)x2, (double)y2,
                    GRD_NORMAL_GRID_FILE, NULL, 0);
#endif

    return 1;

}  /*  end of function con_resample_for_smoothing  */





/*
  ****************************************************************

          C h e c k C o n t o u r O r i e n t a t i o n

  ****************************************************************

    This is called if a contour is closed.  The area of the polygon
  is computed.  If the area is positive, the contour is clockwise
  if negative, ccw.
*/

int CSWConCalc::CheckContourOrientation (void)
{
    int            i;
    CSW_F          xt, sum;

    if (Nbuf1 < 100) {
        return CheckDoubleContourOrientation ();
    }

    sum = 0.0f;
    for (i=0; i<Nbuf1-1; i++) {
        xt = Xbuf1[i] * Ybuf1[i+1] - Xbuf1[i+1] * Ybuf1[i];
        sum += xt;
    }

    if (sum > 0.0) {
        OutputClosure = 1;
    }
    else if (sum < 0.0) {
        OutputClosure = -1;
    }
    else {
        OutputClosure = 0;
    }

    return 1;

}  /*  end of private CheckContourOrientation function  */





/*
  ****************************************************************

     C h e c k D o u b l e C o n t o u r O r i e n t a t i o n

  ****************************************************************

    This is called if a short contour is closed.  The area of the polygon
  is computed.  If the area is positive, the contour is clockwise
  if negative, ccw.  This function uses double precision.  The
  function is only called from CheckContourOrientation if there are
  less than 100 points in Xbuf1 and Ybuf1.

*/

int CSWConCalc::CheckDoubleContourOrientation (void)
{
    int            i;
    double         xt, sum;
    double         dx[200], dy[200];

    for (i=0; i<Nbuf1; i++) {
        dx[i] = (double)Xbuf1[i];
        dy[i] = (double)Ybuf1[i];
    }

    sum = 0.0;
    for (i=0; i<Nbuf1-1; i++) {
        xt = dx[i] * dy[i+1] - dx[i+1] * dy[i];
        sum += xt;
    }

    if (sum > 0.0) {
        OutputClosure = 1;
    }
    else if (sum < 0.0) {
        OutputClosure = -1;
    }
    else {
        OutputClosure = 0;
    }

    return 1;

}  /*  end of private CheckDoubleContourOrientation function  */






int CSWConCalc::FindSimpleGridLimits (CSW_F *grid, int ndata)

{

    int              i;
    CSW_F            zt;

    if (RangeCheckNeeded == 0) {
        return 1;
    }

/*
    Find absolute limits of the grid.
*/
    Zmin = 1.e30f;
    Zmax = -1.e30f;
    for (i=0; i<ndata; i++) {
        zt = grid[i];
        if (zt >= ContourNullValue  ||  zt <= -ContourNullValue) {
            continue;
        }
        if (zt < Zmin) Zmin = zt;
        if (zt > Zmax) Zmax = zt;
    }

    return 1;

}



/*
  ****************************************************************************

                   F i n d H i s t o G r i d L i m i t s

  ****************************************************************************

    Find the absolute grid limits and also use a histogram to find reasonable
  grid limits for contouring and color fills.

*/

int CSWConCalc::FindHistoGridLimits (CSW_F *grid, int ndata)
{
    int              istat, itry, i, histo[MAX_HISTO], n, nhisto;
    int              n1, n2, ncheck, ntot, n10, n90, nt;
    CSW_F            zt, zsize, zmin, zmax, zmaxlast, zminlast, tiny;
    CSW_F            tinydiv, zmean, zvar;

/*
 * If the contour levels are explicitly set,
 * only a simple zmin, zmax is needed.
 */
    if (NumMinor > 0  ||  NumMajor > 0) {
        istat = FindSimpleGridLimits (grid, ndata);
        HistoZmin = 1.e30f;
        HistoZmax = -1.e30f;
        return istat;
    }

/*
    Find absolute limits of the grid.
*/
    Zmin = 1.e30f;
    Zmax = -1.e30f;
    zmean = 0.0;
    ntot = 0;
    for (i=0; i<ndata; i++) {
        zt = grid[i];
        if (zt >= ContourNullValue  ||  zt <= -ContourNullValue) {
            continue;
        }
        if (zt < Zmin) Zmin = zt;
        if (zt > Zmax) Zmax = zt;
        zmean += zt;
        ntot++;
    }

    HistoZmin = Zmin;
    HistoZmax = Zmax;

/*
    Return if there are not enough grid nodes to do
    a statistical analysis.
*/
    if (ntot < 10) {
        return 1;
    }

    if (ThicknessFlagIsSet == 0) {
        ContourThicknessFlag = OptContourThicknessFlag;
    }

/*
    Find the mean and the average variation from the mean.
*/
    zmean /= (CSW_F)ntot;
    zvar = 0.0f;
    nt = 0;
    for (i=0; i<ndata; i++) {
        zt = grid[i];
        if (zt >= ContourNullValue  ||  zt <= -ContourNullValue) {
            continue;
        }
        zt -= zmean;
        if (zt < 0.0f) zt = -zt;
        zvar += zt;
        nt++;
    }
    zvar /= (CSW_F)nt;

    zt = zmean - Zmin;
    if (Zmax - zmean < zt) zt = Zmax - zmean;
    if (zt > 0.0) {
        tinydiv = (Zmax - Zmin) / zt;
    }
    else {
        tinydiv = 100.0f;
    }

/*
    In a perfectly planar surface, the zvar will be .25 of the range
    of the grid.  With spikes, the zvar can be much smaller a percentage.
    If the zvar is smaller than 1/20th of the zrange, adjust the tinydiv
    higher to enable the histogram processing which will calculate
    first and last contour levels that ignore spikes.
*/
    if (zvar > 0.0) {
        zt = (Zmax - Zmin) / zvar;
        zt /= 20.0f;
        if (zt < 1.0) zt = 1.0;
        tinydiv *= zt;
        if (tinydiv > 100.0f) tinydiv = 100.0f;
    }

    if (ContourThicknessFlag == CON_POSITIVE_THICKNESS) Zmin = 0.0f;
    if (ContourThicknessFlag == CON_NEGATIVE_THICKNESS) Zmax = 0.0f;

    zmin = Zmin;
    zmax = Zmax;
    tiny = (zmax - zmin) / tinydiv;
    zminlast = zmin + tiny;
    zmaxlast = zmax - tiny;

/*
    Set up a histogram of the grid values.
*/
    nhisto = ntot / 100;
    if (nhisto > MAX_HISTO) nhisto = MAX_HISTO;
    if (nhisto < 10) nhisto = 10;
    zsize = 0.0f;
    n1 = 0;
    n2 = nhisto;

    ncheck = ntot * 93 / 100;

    for (itry=0; itry<10; itry++) {

        memset (histo, 0, MAX_HISTO * sizeof(int));
        zsize = (zmax - zmin) / (CSW_F)(nhisto - 1);

        ntot = 0;
        for (i=0; i<ndata; i++) {
            if (grid[i] >= ContourNullValue  ||  grid[i] <= -ContourNullValue) {
                continue;
            }
            if (grid[i] <= zmin  ||  grid[i] >= zmax) continue;
            zt = grid[i] - zmin;
            if (zt >= zsize) {
                n = (int)(zt / zsize);
            }
            else {
                n = 0;
            }
            histo[n]++;
            ntot++;
        }

        if (ntot < ncheck) break;

        n = 0;
        n10 = -1;
        n90 = -1;
        for (i=0; i<nhisto; i++) {
            n += histo[i];
            if (n10 == -1  &&  n >= ntot / 100) {
                n10 = i;
            }
            if (n90 == -1  &&  n >= ntot * 99 / 100) {
                n90 = i;
                break;
            }
        }
        if (n10 == -1) n10 = 0;
        if (n90 == -1) n90 = nhisto - 1;

        n1 = n10 - 1;
        n2 = n90 + 1;

        if (n1 < 0) n1 = 0;
        if (n2 > nhisto) n2 = nhisto;

        zmin = zmin + zsize * n1;
        zmax = zmin + zsize * n2;

        if (zmin <= zminlast  &&  zmax >= zmaxlast) {
            if (itry == 0) {
                zmin = Zmin;
                zmax = Zmax;
            }
            break;
        }

        tinydiv *= 1.25f;
        tiny = (zmax - zmin) / tinydiv;
        zminlast = zmin + tiny;
        zmaxlast = zmax - tiny;

    }

    HistoZmin = zmin;
    HistoZmax = zmax;
    if (HistoZmax > Zmax) HistoZmax = Zmax;

    return 1;

}  /*  end of private FindHistoGridLimits function  */





/*
  ****************************************************************************

                            S a d d l e C h e c k

  ****************************************************************************

    Check if the 4 points specified form a saddle grid cell.  The points must
  either be the grid corners clockwise or ccw around the cell.  If the cell is
  a saddle, return 1.  If not, return zero.

*/

int CSWConCalc::SaddleCheck (CSW_F z1, CSW_F z2, CSW_F z3, CSW_F z4)
{
    if (z2 > z1  &&  z3 < z2  &&  z4 > z3  &&  z1 < z4) {
        return 1;
    }
    if (z2 < z1  &&  z3 > z2  &&  z4 < z3  &&  z1 > z4) {
        return 1;
    }

    return 0;

}  /*  end of private SaddleCheck function  */





/*
  ****************************************************************************

                 c o n _ a u t o _ g r i d _ l i m i t s

  ****************************************************************************

    Calculate the HistoZmin and HistoZmax for a grid and return them.

*/

int CSWConCalc::con_auto_grid_limits
               (CSW_F *grid, int ncol, int nrow,
                CSW_F *zmin, CSW_F *zmax)
{

/*
    Check obvious errors
*/
    if (grid == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (ncol < 2  ||  ncol > WildInt) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (nrow < 2  ||  nrow > WildInt) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (zmin == NULL  ||  zmax == NULL) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    FindHistoGridLimits (grid, ncol*nrow);

    *zmin = HistoZmin;
    *zmax = HistoZmax;

    return 1;

}  /*  end of function con_auto_grid_limits  */





/*
  ****************************************************************

   C h e c k S m o o t h e d C o n t o u r O r i e n t a t i o n

  ****************************************************************

    This is called if a contour is closed.  The area of the polygon
  is computed.  If the area is positive, the contour is clockwise
  if negative, ccw.
*/

int CSWConCalc::CheckSmoothedContourOrientation (void)
{
    int            i;
    CSW_F          xt, sum;

    if (Nbuf2 < 1000) {
        return CheckSmoothedDoubleContourOrientation ();
    }

    sum = 0.0f;
    for (i=0; i<Nbuf2-1; i++) {
        xt = Xbuf2[i] * Ybuf2[i+1] - Xbuf2[i+1] * Ybuf2[i];
        sum += xt;
    }

    if (sum > 0.0) {
        OutputClosure = 1;
    }
    else if (sum < 0.0) {
        OutputClosure = -1;
    }
    else {
        OutputClosure = 0;
    }

    return 1;

}  /*  end of private CheckSmoothedContourOrientation function  */





/*
  ****************************************************************

  C h e c k S m o o t h e d D o u b l e C o n t o u r O r i e n t a t i o n

  ****************************************************************

    This is called if a short contour is closed.  The area of the polygon
  is computed.  If the area is positive, the contour is clockwise
  if negative, ccw.  This function uses double precision.  The
  function is only called from CheckContourOrientation if there are
  less than 100 points in Xbuf1 and Ybuf1.

*/

int CSWConCalc::CheckSmoothedDoubleContourOrientation (void)
{
    int            i;
    double         xt, sum;
    double  dx[2000], dy[2000];

    for (i=0; i<Nbuf2; i++) {
        dx[i] = (double)Xbuf2[i];
        dy[i] = (double)Ybuf2[i];
    }

    sum = 0.0;
    for (i=0; i<Nbuf2-1; i++) {
        xt = dx[i] * dy[i+1] - dx[i+1] * dy[i];
        sum += xt;
    }

    if (sum > 0.0) {
        OutputClosure = 1;
    }
    else if (sum < 0.0) {
        OutputClosure = -1;
    }
    else {
        OutputClosure = 0;
    }

    return 1;

}  /*  end of private CheckSmoothedDoubleContourOrientation function  */




/*
  ****************************************************************

                R e m o v e N u l l B o r d e r

  ****************************************************************

  Output the non null interior of the specified grid into another
  grid array.  Modify the grid geometry to agree with the output
  interior grid.  If no null margin exists, the output grid is simply
  set to the input.  This should only be called from con_calc_contours.

*/

void CSWConCalc::RemoveNullBorder
                             (CSW_F *grid,
                              int *ncol, int *nrow,
                              CSW_F *xmin, CSW_F *ymin,
                              CSW_F *xmax, CSW_F *ymax,
                              CSW_F nullvalue,
                             CSW_F **gridout)
{
    int           i, j, k, nc, nr, kout, nc2, nr2,
                  offset, nt, left, right, top, bottom;
    CSW_F         x1, y1, x2, y2, zt, znull, *gout, dx, dy;

    *gridout = grid;
    nr = *nrow;
    nc = *ncol;
    x1 = *xmin;
    y1 = *ymin;
    x2 = *xmax;
    y2 = *ymax;

    znull = nullvalue;
    if (znull < 0.0f) {
        znull = -znull;
    }
    znull /= 100.0f;

    nt = nc * nr;
    bottom = -1;
    for (i=0; i<nt; i++) {
        zt = grid[i];
        if (zt < -znull  ||  zt > znull) {
            continue;
        }
        bottom = i / nc;
        break;
    }
    if (bottom == -1) {
        return;
    }

    top = -1;
    for (i=nt-1; i>=0; i--) {
        zt = grid[i];
        if (zt < -znull  ||  zt > znull) {
            continue;
        }
        top = i / nc;
        break;
    }

    left = -1;
    for (j=0; j<nc; j++) {
        for (i=0; i<nr; i++) {
            k = i * nc + j;
            zt = grid[k];
            if (zt < -znull  ||  zt > znull) {
                continue;
            }
            left = j;
            break;
        }
        if (left != -1) {
            break;
        }
    }

    right = -1;
    for (j=nc-1; j>=0; j--) {
        for (i=0; i<nr; i++) {
            k = i * nc + j;
            zt = grid[k];
            if (zt < -znull  ||  zt > znull) {
                continue;
            }
            right = j;
            break;
        }
        if (right != -1) {
            break;
        }
    }

    if (left == -1  ||  right == -1  ||  top == -1  ||  bottom == -1) {
        return;
    }

    if (left == 0  &&  bottom == 0  &&
        right == nc-1  &&  top == nr-1) {
        return;
    }

    nc2 = right - left + 1;
    nr2 = top - bottom + 1;
    gout = (CSW_F *)csw_Malloc (nc2 * nr2 * sizeof(CSW_F));
    if (gout == NULL) {
        return;
    }

    dx = (x2 - x1) / (CSW_F)(nc - 1);
    dy = (y2 - y1) / (CSW_F)(nr - 1);
    x1 += left * dx;
    y1 += bottom * dy;
    x2 = x1 + (nc2 - 1) * dx;
    y2 = y1 + (nr2 - 1) * dy;

    kout = 0;
    for (i=bottom; i<=top; i++) {
        offset = i * nc;
        for (j=left; j<=right; j++) {
            k = offset + j;
            gout[kout] = grid[k];
            kout++;
        }
    }

    *ncol = nc2;
    *nrow = nr2;
    *xmin = x1;
    *ymin = y1;
    *xmax = x2;
    *ymax = y2;
    *gridout = gout;

    return;

}  /*  end of private RemoveNullBorder function  */



/*
 *********************************************************************************

            c o n _ c l i p _ c o n t o u r s _ t o _ p o l y g o n

 *********************************************************************************

  Clip the specified contour lines to the specified set of polygons.  The results
  are returned in the concliup array, which is allocated as large as needed by
  this function.  Ownership of the conclip array is relinquished to the calling
  routine upon successful completion.  In other words, you have to csw_Free it.

*/

int CSWConCalc::con_clip_contours_to_polygon
          (int inside_outside_flag,
           COntourOutputRec *contours, int ncontours,
           double *xpoly, double *ypoly, int *ipoly, int npoly,
           COntourOutputRec **conclip, int *nconclip)
{
    double                 *xline, *yline, *xout, *yout;
    int                    *iout;
    int                    maxline, i, j, n, ido, istat;
    int                    nout, maxout, maxlineout, maxcomp, nconout;
    COntourOutputRec       *c1, *c2, *conout;

    CSWPolyCalc            ply_calc_obj;

/*
    Initialize output in case of error.
*/
    *conclip = NULL;
    *nconclip = 0;

/*
    Find the maximum length input line and allocate
    a clip array four times as large.
*/
    maxline = 0;
    for (ido=0; ido<ncontours; ido++) {
        c1 = contours + ido;
        if (c1->npts > maxline) {
            maxline = c1->npts;
        }
    }

    maxline += 1;
    if (maxline < 200) {
        maxline = 200;
    }

/*
    The xline pointer has space for 2 input arrays of size maxline each
    and 2 output arrays of size 8 * maxline.  This is not the theoretical
    max output size, but it works for reasonable conditions.
*/
    xline = (double *)csw_Malloc (maxline * 18 * sizeof(double));
    if (xline == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    yline = xline + maxline;
    xout = yline + maxline;
    yout = xout + 8 * maxline;
    maxlineout = maxline * 8;

    maxcomp = npoly * 10;
    if (maxcomp < 100) {
        maxcomp = 100;
    }
    iout = (int *)csw_Malloc (maxcomp * sizeof(int));
    if (iout == NULL) {
        grd_utils_ptr->grd_set_err (1);
        csw_Free (xline);
        return -1;
    }

/*
    Allocate as many contour records for output as was specified in the input.
    This will be expanded if needed.
*/
    maxout = ncontours;
    if (maxout < 20) {
        maxout = 20;
    }
    conout = (COntourOutputRec *)csw_Calloc (maxout * sizeof(COntourOutputRec));
    if (conout == NULL) {
        csw_Free (xline);
        csw_Free (iout);
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Loop through each contour and clip it.
*/
    nconout = 0;
    for (ido=0; ido<ncontours; ido++) {

        c1 = contours + ido;
        for (i=0; i<c1->npts; i++) {
            xline[i] = (double)c1->x[i];
            yline[i] = (double)c1->y[i];
        }

        istat = ply_calc_obj.ply_ClipPlineToArea (inside_outside_flag,
                                     xpoly, ypoly, ipoly, npoly,
                                     xpoly, ypoly, ipoly, npoly,
                                     xline, yline, c1->npts,
                                     xout, yout, iout, &nout,
                                     maxlineout, maxcomp);
        if (istat == -1) {
            con_free_contours (conout, nconout);
            csw_Free (xline);
            csw_Free (iout);
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

        if (nout+nconout >= maxout) {
            maxout += nout * 4;
            conout = (COntourOutputRec *)csw_Realloc (conout, maxout * sizeof(COntourOutputRec));
            if (conout == NULL) {
                con_free_contours (conout, nconout);
                csw_Free (xline);
                csw_Free (iout);
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }

        n = 0;
        for (i=0; i<nout; i++) {
            c2 = conout + nconout;
            memcpy (c2, c1, sizeof(COntourOutputRec));
            c2->x = (CSW_F*)csw_Malloc (2 * iout[i] * sizeof(CSW_F));
            if (c2->x == NULL) {
                con_free_contours (conout, nconout);
                csw_Free (xline);
                csw_Free (iout);
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            c2->y = c2->x + iout[i];
            c2->npts = iout[i];
            for (j=0; j<iout[i]; j++) {
                c2->x[j] = (CSW_F)xout[n];
                c2->y[j] = (CSW_F)yout[n];
                n++;
            }
            nconout++;
        }
    }

    if (nconout < 1) {
        if (conout != NULL) {
            con_free_contours (conout, nconout);
        }
        conout = NULL;
        nconout = 0;
    }

    csw_Free (xline);
    csw_Free (iout);

    *conclip = conout;
    *nconclip = nconout;

    return 1;

}  /* end of function con_clip_contours_to_polygon */



/*
 *************************************************************************************

                 C o r r e c t R e s a m p l e d T h i c k n e s s

 *************************************************************************************

  Change grid nodes that should not be negative.  In the resampled grid, which
  is passed to this function, every nskip row and column is an original node.
  If a resampled node value is less than or equal to zero, but its surrounding
  original nodes are all greater than zero, then a new value is placed in the
  resampled node using bilinear interpolation of the surrounding original nodes.

*/

void CSWConCalc::CorrectResampledThickness
          (CSW_F *grid, int ncol, int nrow, int nskip)
{
    int          i, j, k, r1, r2, c1, c2,
                 offset, k1, k2, k3, k4, npos;
    CSW_F        z1, z2, z3, z4, zt1, zt2, zt, xpct, ypct;

    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        r1 = i / nskip * nskip;
        r2 = r1 + nskip;
        if (i == nrow - 1) {
            r2 = nrow - 1;
            r1 = r2 - nskip;
        }
        r1 *= ncol;
        r2 *= ncol;
        for (j=0; j<ncol; j++) {
            if (i % nskip == 0  &&  j % nskip == 0) {
                continue;
            }
            k = offset + j;
            c1 = j / nskip * nskip;
            c2 = c1 + nskip;
            if (j == ncol - 1) {
                c2 = ncol - 1;
                c1 = c2 - nskip;
            }
            k1 = r1 + c1;
            k2 = r1 + c2;
            k3 = r2 + c1;
            k4 = r2 + c2;
            z1 = grid[k1];
            z2 = grid[k2];
            z3 = grid[k3];
            z4 = grid[k4];
            npos = 0;
            if (z1 > 0.0f) npos++;
            if (z2 > 0.0f) npos++;
            if (z3 > 0.0f) npos++;
            if (z4 > 0.0f) npos++;
            if ((npos == 4  &&  grid[k] <= 0.0)  ||
                (npos == 0  &&  grid[k] > 0.0)) {
                xpct = (CSW_F)(j % nskip) / (CSW_F)nskip;
                ypct = (CSW_F)(i % nskip) / (CSW_F)nskip;
                zt1 = z1 + xpct * (z2 - z1);
                zt2 = z3 + xpct * (z4 - z3);
                zt = zt1 + ypct * (zt2 - zt1);
                grid[k] = zt;
            }
        }
    }

    return;

}  /* end of private CorrectResampledThickness function */


