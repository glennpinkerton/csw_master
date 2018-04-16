
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    con_api.cc

    This file has methods for the CSWContourApi class.  An
    application should create a CSWContourApi object and
    use the methods as needed to calculate contours.
*/

#include <stdio.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/surfaceworks/include/contour_api.h"
#include "csw/surfaceworks/include/grid_api.h"
#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_triangle_class.h"




/*
  ****************************************************************

              c o n _ S e t C a l c O p t i o n

  ****************************************************************

  function name:    con_SetCalcOption         (int)

  call sequence:    con_SetCalcOption (tag, ival, fval)

  purpose:          Set a contour calculation option or reset
                    all options to their default values.

  return value:     status code

                    1 = success
                   -1 = error

  errors:          2 = unknown tag value

  calling parameters:

    tag        r    int         Identifier for the option.  The symbols and
                                literal values for tags are defined in the
                                csw/surfaceworks/include/contour_api.h file.
    ival       r    int         Option value for an integer option.
    fval       r    CSW_F   Option value for a CSW_F option.

*/

int CSWContourApi::con_SetCalcOption
    (int tag, int ival, CSW_F fval)
{
    int           istat;

    istat = con_calc_obj.con_set_calc_option (tag, ival, fval);
    return istat;

}  /*  end of function con_SetCalcOption  */




/*
  ****************************************************************

             c o n _ S e t C a l c O p t i o n s

  ****************************************************************

  function name:    con_SetCalcOptions        (int)

  call sequence:    con_SetCalcOptions (options)

  purpose:          Set all contour calculation options to the values
                    specified in the COntourCalcOptions structure.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           2 = A NULL options pointer is specified.

  calling parameters:

    options   r   COntourCalcOptions*    Pointer to a filled in contour calculation
                                         options structure.

*/

int CSWContourApi::con_SetCalcOptions (COntourCalcOptions *options)
{
    int           istat;

    istat = con_calc_obj.con_set_calc_options (options);
    return istat;

}  /*  end of function con_SetCalcOptions  */




/*
  ****************************************************************

             c o n _ D e f a u l t C a l c O p t i o n s

  ****************************************************************

  function name:    con_DefaultCalcOptions        (int)

  call sequence:    con_DefaultCalcOptions (options)

  purpose:          Set all members of a contour calculation options
                    structure to their default values.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           2 = A NULL options pointer is specified.

  calling parameters:

    options   r   COntourCalcOptions*    Pointer to a filled in contour calculation
                                         options structure.

*/

int CSWContourApi::con_DefaultCalcOptions (COntourCalcOptions *options)
{
    int           istat;

    istat = con_calc_obj.con_default_calc_options (options);
    return istat;

}  /*  end of function con_DefaultCalcOptions  */




/*
  ****************************************************************

           c o n _ S e t C o n t o u r I n t e r v a l s

  ****************************************************************

  function name:  con_SetContourIntervals           (int)

  call sequence:  con_SetContourIntervals (interval, major, first, last,
                                           minor_values, nminor,
                                           major_values, nmajor)

  purpose:        Specify the contour interval information for subsequent
                  contour calculations.

  return value:   status code

                  1 = success
                 -1 = failure

  errors:         currently there are no possible errors

  calling parameters:

    interval      r    CSW_F      Contour interval
    major         r    int        Modulus of major contours
    first         r    CSW_F      minimum contour value
    last          r    CSW_F      maximum contour value
    minor_values  r    CSW_F*     array of specific minor values
                                  or NULL if there are none.
    major_values  r    CSW_F*     array of specific major values or NULL
    nminor        r    int        number of minor values in array
    nmajor        r    int        number of major values in array

*/

int CSWContourApi::con_SetContourIntervals
                            (CSW_F interval, int major,
                             CSW_F first, CSW_F last,
                             CSW_F *minor_values, int nminor,
                             CSW_F *major_values, int nmajor)
{
    int              istat;

    istat = con_calc_obj.con_set_contour_intervals
                                      (interval, major, first, last,
                                       minor_values, nminor,
                                       major_values, nmajor);
    return istat;

}  /*  end of function con_SetContourIntervals  */






/*
  ****************************************************************

                 c o n _ C a l c C o n t o u r s

  ****************************************************************

  function name:  con_CalcContours              (int)

  call sequence:  con_CalcContours (grid, ncol, nrow,
                                    x1, y1, x2, y2, scale,
                                    contours, ncontours,
                                    faults, nfaults,
                                    options)

  purpose:        Calculate contour lines as set up via con_SetContourIntervals
                  of the specified grid.  The current contour options are also
                  used.  The grid may have null values which will not be contoured.

  return value:   status code

                  1 = success
                 -1 = error

  errors:         1 = memory allocation error
                  2 = wild input parameter
                  3 = NULL grid pointer
                  4 = ncol and/or nrow less than 2.
                  5 = contours or ncontours is NULL
                  6 = specified limits are inconsistent
                  7 = more than 500 million nodes in the grid
                  8 = the grid will not support logarithmic contours
                      which are in effect from the most recent contour
                      option setting.
                  9 = The contour interval currently specified will
                      produce more than 2000 contour levels.
                  10= The grid is a flat surface and cannot be contoured.
                  11= The fault data are inconsistent.  Either faults is not
                      null and nfaults is zero, or faults is NULL and nfaults
                      is greater than zero.
                  12= No valid grid nodes are found in the grid.
                  13= The grid values do not support the specified thickness flag.

                  99 = range/precision problem

  calling parameters:

    grid          r   CSW_F*            2-d grid array of elevations
    ncol          r   int               number of columns in the grid
    nrow          r   int               number of rows in the grid
    x1            r   CSW_F             x coordinate of the lower left
                                        corner of the grid
    y1            r   CSW_F             y coordinate of lower left
    x2            r   CSW_F             x of upper right
    y2            r   CSW_F             y of upper right
    scale         r   CSW_F             scale in grid units per inch or 0.0
                                        if scaling is not known.  The scale
                                        is only used to suppress crowded
                                        contours and to help determine where
                                        smoothing of contours is needed.  The
                                        returned contour points are always in
                                        the grid units specified by x1, y1, x2,
                                        and y2.

    contours      w  COntourOutputRec**  Pointer to receive an array of contour
                                        lines.  The array is allocated by this
                                        function, but the application must csw_Free
                                        it by calling con_FreeContours at an
                                        appropriate time.
    ncontours     w  int*               The number of contour lines returned.

                        The contours array returned is NULL terminated, so
                        you can either use the returned ncontours value to
                        read the data or you can use the terminating NULL.

    options       r  COntourCalcOptions*  Optional Contour calc options structure.

*/

int CSWContourApi::con_CalcContours
                     (CSW_F *grid, int ncol, int nrow,
                      CSW_F x1, CSW_F y1,
                      CSW_F x2, CSW_F y2, CSW_F scale,
                      COntourOutputRec **contours, int *ncontours,
                      FAultLineStruct *faults, int nfaults,
                      COntourCalcOptions *options)
{
    int               istat, fsave;
    int               do_write;

    CSWGrdFault       grd_fault_obj;

    if ((faults && nfaults<1)  ||  (!faults && nfaults>0)) {
        grd_utils_obj.grd_set_err (11);
        return -1;
    }

    istat = csw_CheckRange2 (x1, y1, x2, y2);
    if (istat == 0) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }
    grd_fault_obj.grd_set_fault_end_flag (1);

    do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_triangle_obj.grd_WriteXYZGridFile (
        "pre_contour.grd",
                grid, ncol, nrow, 1,
                (double)x1, (double)y1, (double)x2, (double)y2);
        grd_calc_obj.grd_WriteFaultLines (faults, nfaults, "pre_contour.flt");
    }

/*
 * Use a tri mesh to contour faulted grids and
 * use the rectangular grid for contouring non
 * faulted grids.
 */
    fsave = 0;
    if (faults  &&  nfaults > 0) {
        con_calc_obj.con_set_calc_option (CON_FAULTED_GRID_FLAG, 1, 0.0f);
        if (options) {
            fsave = options->faulted_flag;
            options->faulted_flag = 1;
        }
        istat = con_triangle_obj.con_contour_grid_using_trimesh
                               (grid, ncol, nrow, 1.e30,
                                (double)x1, (double)y1,
                                (double)x2, (double)y2,
                                faults, nfaults,
                                contours, ncontours, options);
    }
    else {
        istat = con_calc_obj.con_calc_contours
                              (grid, ncol, nrow,
                               x1, y1, x2, y2, scale,
                               contours, ncontours, options);
    }

    if (istat == -1  &&  options) {
        options->error_number = con_GetErr ();
    }

    if (faults  &&  nfaults > 0) {
        if (options) {
            options->faulted_flag = fsave;
        }
        con_calc_obj.con_set_calc_option (CON_FAULTED_GRID_FLAG, 0, 0.0f);
    }

    grd_fault_obj.grd_set_fault_end_flag (1);

    return istat;

}  /*  end of function con_CalcContours  */





/*
  ****************************************************************

                  c o n _ F r e e C o n t o u r s

  ****************************************************************

  function name:  con_FreeContours           (int)

  call sequence:  con_FreeContours (contour_pointer, ncontours)

  purpose:        Free the memory used by a contour list that was
                  returned from con_CalcContours.  You should always
                  use this function to csw_Free the memory.  If you just
                  call the csw_Free function you will end up with a lot
                  of memory leaks.

  return value:   status code

                  1 = success
                 -1 = error

  errors:         2 = contour_pointer is NULL

  calling parameters:

    contour_pointer    r    COntourOutputRec*    The pointer to a contour
                                                 list returned from the
                                                 con_CalcContours function.
    ncontours          r    int                  Number of contours as
                                                 returned from the con_CalcContours
                                                 function.

*/

int CSWContourApi::con_FreeContours
          (COntourOutputRec *contour_pointer, int ncontours)
{
    int                  istat;

    istat = con_calc_obj.con_free_contours (contour_pointer, ncontours);
    return istat;

}  /*  end of function con_FreeContours  */







/*
  ****************************************************************

               c o n _ S e t D r a w S c a l e

  ****************************************************************

  function name:  con_SetDrawScale                (int)

  call sequence:  con_SetDrawScale (gx1, gy1, gx2, gy2,
                                    px1, py1, px2, py2)

  purpose:        Set up scaling and transformation to use on
                  subsequent contour line drawing calls.  The
                  results of contour calculation are in grid units.
                  The contours need to be converted to plot (drawing)
                  units before display.

  return value:   status code

                  -1 = error
                   1 = success

  errors:          2 = mimimums are greater than or equal to maximums

  calling parameters:

    gx1    r    CSW_F     Lower left x grid coordinate.
    gy1    r    CSW_F     Lower left y grid coordinate.
    gx2    r    CSW_F     Upper right x grid coordinate.
    gy2    r    CSW_F     Upper right y grid coordinate.
    px1    r    CSW_F     Lower left plot page x coordinate.
    py1    r    CSW_F     Lower left plot page y coordinate.
    px2    r    CSW_F     Upper right plot page x
    py2    r    CSW_F     Upper right plot page y

*/

int CSWContourApi::con_SetDrawScale
      (CSW_F gx1, CSW_F gy1, CSW_F gx2, CSW_F gy2,
       CSW_F px1, CSW_F py1, CSW_F px2, CSW_F py2)
{
    int               istat;

    istat = csw_CheckRange2 (gx1, gy1, gx2, gy2);
    if (istat == 0) {
        grd_utils_obj.grd_set_err (99);
        return -1;
    }

    istat = con_draw_obj.con_set_draw_scale
                               (gx1, gy1, gx2, gy2,
                                px1, py1, px2, py2, 0);
    return istat;

}  /*  end of function con_SetDrawScale  */





/*
  ****************************************************************

              c o n _ S e t D r a w O p t i o n

  ****************************************************************

  function name:    con_SetDrawOption              (int)

  call sequence:    con_SetDrawOption (tag, ival, fval)

  purpose:          Set a parameter to use in subsequent calls to
                    con_DrawLine.  These parameters control tick
                    marks and in line labels generated by con_DrawLine.

  return value:     status code

                    -1 = error
                     1 = success

  errors:            3 = unknown tag

  calling parameters:

    tag        r     int           Identifier tag of option to set.
                                   These are defined symbolically
                                   when you include csw/surfaceworks/include/contour_api.h.

    The following table shows the tag symbols, literal values, and the
  type of data (CSW_F or int) that need to be set to define the option
  associated with the tag.

     CON_DEFAULT_OPTIONS          0     (none)
     CON_MAJOR_TICK_SPACING       1     (CSW_F)  default = 0.5
     CON_MAJOR_TEXT_SPACING       2     (CSW_F)  default = 6.0
     CON_MAJOR_TICK_LEN           3     (CSW_F)  default = 0.07
     CON_MAJOR_TEXT_SIZE          4     (CSW_F)  default = 0.08
     CON_MINOR_TICK_SPACING       5     (CSW_F)  default = 0.0 (no ticks)
     CON_MINOR_TEXT_SPACING       6     (CSW_F)  default = 0.0 (no labels)
     CON_MINOR_TICK_LEN           7     (CSW_F)  default = 0.04
     CON_MINOR_TEXT_SIZE          8     (CSW_F)  default = 0.07
     CON_TICK_DIRECTION           9     (int)        (1 to left -1 to right)
                                                     default = 1

    ival       r     int           Integer option value.
    fval       r     CSW_F     Float option value.

*/

int CSWContourApi::con_SetDrawOption (int tag, int ival, CSW_F fval)
{
    int               istat;

    istat = con_draw_obj.con_set_draw_option (tag, ival, fval);
    return istat;

}  /*  end of function con_SetDrawParam  */






/*
  ****************************************************************

                  c o n _ D r a w L i n e

  ****************************************************************

  function name:  con_DrawLine               (int)

  call sequence:  con_DrawLine (outputrec, charwidths,
                                lineprims, nlines,
                                textprims, ntext)

  purpose:        Given a contour output record returned from con_CalcContours,
                  generate line and text primitives that can be used to draw
                  the contour line.  The processing handles any tick marks and
                  in line labels.  The returned primitives are positioned and
                  ready for drawing.

                  After you are finished with the lineprims and textprims pointers,
                  csw_Free them using con_FreeDrawing.  Do not call csw_Free using the
                  lineprims or textprims pointers directly as this will produce
                  memory leaks.

  return value:   status code

                  -1 = error
                   1 = success

  errors:          1 = memory allocation error
                   3 = A null pointer is specified for a parameter.
                   4 = No scaling has been set up yet (see con_SetDrawScale)
                   5 = There are less than 2 points in the contour record.
                   98= The scaling is set to a different precision than the
                       contour output rec structure.

  calling parameters:

    outputrec      r    COntourOutputRec*    pointer to a single contour output
                                             structure returned from con_CalcContours
    charwidths     r    CSW_F*           An array with the character widths of each
                                             character to be drawn in the label.  Set this
                                             NULL to use constant width characters .9 times
                                             the text size specified in con_SetDrawParam.
    lineprims      w    COntourLinePrim*     Array of contour line primitive structures.
    nlines         w    int*                 Number of line primitives
    textprims      w    COntourTextPrim      Array of contour line text character primitives.
    ntext          w    int*                 Number of text primitives.

*/

int CSWContourApi::con_DrawLine
                 (COntourOutputRec *outputrec, CSW_F *charwidths,
                  COntourLinePrim **lineprims, int *nlines,
                  COntourTextPrim **textprims, int *ntext,
                  COntourDrawOptions *options)
{
    int                     istat;

    istat = con_draw_obj.con_draw_line
                          (outputrec, charwidths,
                           lineprims, nlines,
                           textprims, ntext, options);

    if (istat == -1  &&  options) {
        options->error_number = con_GetErr ();
    }

    return istat;

}  /*  end of con_DrawLine function  */





/*
  ****************************************************************

                c o n _ F r e e D r a w i n g

  ****************************************************************

    Free line and text primitives returned from a previous call
  to con_DrawLine.  You should never directly csw_Free the pointers
  returned from con_DrawLine.  If you do this, memory leaks will
  occur.

    This function always returns 1.  No errors are associated with it.

*/

int CSWContourApi::con_FreeDrawing
                    (COntourLinePrim *lineprims, int nlines,
                     COntourTextPrim *textprims, int ntext,
                     COntourFillPrim *fillprims, int nfill)
{

    con_draw_obj.con_free_drawing
                     (lineprims, nlines,
                      textprims, ntext,
                      fillprims, nfill);
    return 1;

}  /*  end of function con_FreeDrawing  */







/*
  ****************************************************************

                      c o n _ G e t E r r

  ****************************************************************

  function name:    con_GetErr          (int)

  call sequence:    con_GetErr ()

  purpose:          Return the most recent error number
                    generated from a contouring function.
                    If no error has been generated, this
                    function returns zero.  Call this immediately
                    after a contouring function returns with
                    status equal to -1.  The error is not
                    guaranteed to be preserved if any other grid or
                    contouring functions are called before checking
                    the error,

  return value:     Integer number of the most recent error.

*/

int CSWContourApi::con_GetErr (void)
{
    int            istat;

    istat = grd_utils_obj.grd_get_err ();
    return istat;

}  /*  end of function con_GetErr  */





/*
  ****************************************************************

                c o n _ S e t C o l o r B a n d s

  ****************************************************************

  function name:    con_SetColorBands             (int)

  call sequence:    con_SetColorBands (lowlist, highlist, colorlist, nlist)

  purpose:          Set up minimum values, maximum values and colors to
                    between these values for subsequent calls to the
                    con_CalcColorfills function.

  return value:     status code

                   -1 = error
                    1 = success

  errors:          2 = lowlist, highlist or colorlist is NULL
                   3 = nlist is less than zero or greater than 200
                   4 = the cumulative min and max of the bands is
                       inconsistent (i.e. the min is >= the max)
                   5 = at least one of the colorlist entries is
                       outside of the permitted range (0-254)


  calling parameters:

    lowlist      r    CSW_F*  Array with low end of each band.
    highlist     r    CSW_F*  Array with high end of each band.
    colorlist    r    int*        Array with "color" to fill in the
                                  band.  This must be between 0 and 254.
    nlist        r    int         Number of color bands defined.

*/

int CSWContourApi::con_SetColorBands
                      (CSW_F *lowlist, CSW_F *highlist,
                       int *colorlist, int nlist)

{
    int              istat;

    istat = con_calc_obj.con_set_color_bands
        (lowlist, highlist, colorlist, nlist);
    if (istat == 1) {
        grd_image_obj.grd_setup_image_color_bands
            (lowlist, highlist, colorlist, nlist);
    }
    return istat;

}  /*  end of function con_SetColorBands  */






/*
  ****************************************************************

                c o n _ S e t Z e r o F i l l C o l o r

  ****************************************************************

  function name:    con_SetZeroFillColor             (int)

  call sequence:    con_SetZeroFillColor (color)

  purpose:          Set up zero fill color.  This is used to control the
                    output of color fill polygons for negative and zero
                    values in thickness grids.  If the color is set to -1,
                    and the thickness option is set, then no zero or negative
                    color fills will be output.  If this is set to anything
                    other than -1, and the thickness option is set, then
                    all zero and negative areas will be filled with this color.

  return value:     status code

                   -1 = error
                    1 = success

  calling parameters:

    color        r    int         Color index or -1.
*/

int CSWContourApi::con_SetZeroFillColor (int color)
{
    int              istat;

    istat = con_calc_obj.con_set_zero_fill_color (color);
    return istat;

}  /*  end of function con_SetZeroFillColor  */






/*
  ****************************************************************

                c o n _ C a l c C o l o r F i l l s

  ****************************************************************

  function name:    con_CalcColorFills           (int)

  call sequence:    con_CalcColorFills (grid, ncol, nrow,
                                        xmin, ymin, xmax, ymax, scale,
                                        fills, nfills,
                                        faults, nfaults, options)

  purpose:          Return the outlines of polygons that can be converted
                    to page coordinates and drawn to show color filled
                    contours.

  return value:     status code

                    -1 = error
                     1 = success

  errors:           1 = memory allocation error
                    2 = NULL grid pointer
                    3 = either ncol or nrow is less than 2
                    4 = either xmin is >= xmax or ymin >= ymax
                    5 = either fills or nfills is NULL
                    6 = no color bands were set up by con_SetColorBands
                        prior to calling this function.
                    7 = The fault data are inconsistent.  Either faults is not
                        null and nfaults is zero, or faults is NULL and nfaults
                        is greater than zero.

  calling parameters:

    grid       r   CSW_F*        2-d grid array to contour
    ncol       r   int               number of columns in the grid
    nrow       r   int               number of rows in the grid
    xmin       r   CSW_F         minimum x of the grid
    ymin       r   CSW_F         minimum y of the grid
    xmax       r   CSW_F         maximum x of the grid
    ymax       r   CSW_F         maximum y of the grid
    scale      r   CSW_F         grid units per page unit
    fills      w   COntourFillRec**  returned array of fill structures
    nfills     w   int*              number of fill structures returned
    faults     r   FAultLineStruct*  array of fault line structures
    nfaults    r   int               number of fault line structures
    options    r   COntourCalcOptions*  optional options structure

*/

int CSWContourApi::con_CalcColorFills
                       (CSW_F *grid, int ncol, int nrow,
                        CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax,
                        CSW_F scale, COntourFillRec **fills, int *nfills,
                        FAultLineStruct *faults, int nfaults,
                        COntourCalcOptions *options)
{
    int               istat, fsave;

    CSWGrdFault       grd_fault_obj;

    if ((faults && nfaults<1)  ||  (!faults && nfaults>0)) {
        grd_utils_obj.grd_set_err (11);
        return -1;
    }

    grd_fault_obj.grd_set_fault_end_flag (1);

    fsave = 0;
    if (faults  &&  nfaults > 0) {
        istat = grd_fault_obj.grd_define_fault_vectors (faults, nfaults);
        if (istat == -1) {
            return -1;
        }
        con_calc_obj.con_set_calc_option (CON_FAULTED_GRID_FLAG, 1, 0.0f);
        if (options) {
            fsave = options->faulted_flag;
            options->faulted_flag = 1;
        }
    }

    istat = con_calc_obj.con_calc_color_fills
                                 (grid, ncol, nrow,
                                  xmin, ymin, xmax, ymax, scale,
                                  fills, nfills, options);

    if (faults  &&  nfaults > 0) {
        if (options) {
            options->faulted_flag = fsave;
        }
        con_calc_obj.con_set_calc_option (CON_FAULTED_GRID_FLAG, 0, 0.0f);
    }

    grd_fault_obj.grd_set_fault_end_flag (1);

    return istat;

}  /*  end of function con_CalcColorFills  */





/*
  ****************************************************************

               c o n _ F r e e C o l o r F i l l s

  ****************************************************************

    Free the memory for the specified contour fill pointer.  You must use
  this function to csw_Free contour fill memory.  Calling csw_Free directly using the
  contour fill pointer will create memory leaks.

*/

int CSWContourApi::con_FreeColorFills (COntourFillRec *list, int nlist)
{
    int                 istat;

    istat = con_calc_obj.con_free_color_fills (list, nlist);
    return istat;

}  /*  end of function con_FreeColorFills  */





/*
  ****************************************************************

              c o n _ B u i l d C o l o r B a n d s

  ****************************************************************

  function name:   con_BuildColorBands                (int)

  call sequence:   con_BuildColorBands (conint, first, last, maxbands,
                                        low, high, nband)

  purpose:         Given a contour interval and first and last contours,
                   return lists of low and high values for color bands.

  return value:    status code

                   -1 = error
                    1 = success

  errors:          3 = first greater than or equal to last
                   4 = maxband less than 1
                   5 = low, high or nband pointer is NULL

  calling parameters:

    conint     r   CSW_F    Contour interval for color changes.  Each
                                color band will be conint wide.
    first      r   CSW_F    Minimum value to color.
    last       r   CSW_F    Maximum value to color.
    maxbands   r   int          Size of the low and high arrays.
    low        w   CSW_F*   Array to receive low edge of each band.
    high       w   CSW_F*   Array to receive high edge of each band.
    nband      w   int*         Number of color bands returned.

                The low and high arrays are allocated by the calling
                application.  They must be large enough for at least
                maxbands CSW_F numbers each.

*/

int CSWContourApi::con_BuildColorBands
     (CSW_F conint, CSW_F first, CSW_F last, int maxband,
      CSW_F *low, CSW_F *high, int *nband)
{
    int             istat;

    istat = con_calc_obj.con_build_color_bands
                                  (conint, first, last, maxband,
                                   low, high, nband);
    return istat;

}  /*  end of function con_BuildColorBands  */





/*
  ****************************************************************

                  c o n _ D r a w F i l l

  ****************************************************************

  function name:  con_DrawFill               (int)

  call sequence:  con_DrawFill (fillrec,
                                fillprims, nfills)

  purpose:        Given a contour fill record returned from con_CalcColorFills,
                  generate line primitives that can be used to draw the contour
                  color fills.  The x and y members of the fillprim structures
                  have the outlines of the polygons.

                  After you are finished with the fillprims structures,
                  csw_Free them using con_FreeDrawing.  Do not call csw_Free using the
                  fillprims pointers directly as this will produce memory leaks.

  return value:   status code

                  -1 = error
                   1 = success

  errors:          1 = memory allocation error
                   3 = A null pointer is specified for a parameter.
                   4 = No scaling has been set up yet (see con_SetDrawScale)

  calling parameters:

    outputrec      r    COntourFillRec*      pointer to a single contour fill
                                             structure returned from con_CalcColorFills
    fillprims      w    COntourFillPrim*     Array of contour fill primitive structures.
    nfills         w    int*                 Number of fill primitives

*/

int CSWContourApi::con_DrawFill
                 (COntourFillRec *fillrec,
                  COntourFillPrim **fillprims, int *nfills)
{
    int                     istat;

    istat = con_draw_obj.con_draw_fill
                          (fillrec,
                           fillprims, nfills);
    return istat;

}  /*  end of con_DrawFill function  */





/*
  ****************************************************************

             c o n _ S e t D r a w O p t i o n s

  ****************************************************************

  function name:    con_SetDrawOptions        (int)

  call sequence:    con_SetDrawOptions (options)

  purpose:          Set all contour drawing options to the values
                    specified in the COntourDrawOptions structure.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           2 = A NULL options pointer is specified.

  calling parameters:

    options   r   COntourDrawOptions*    Pointer to a filled in contour drawing
                                         options structure.

*/

int CSWContourApi::con_SetDrawOptions (COntourDrawOptions *options)
{
    int           istat;

    istat = con_draw_obj.con_set_draw_options (options);
    return istat;

}  /*  end of function con_SetDrawOptions  */




/*
  ****************************************************************

             c o n _ D e f a u l t D r a w O p t i o n s

  ****************************************************************

  function name:    con_DefaultDrawOptions        (int)

  call sequence:    con_DefaultDrawOptions (options)

  purpose:          Set all members of a contour drawing options
                    structure to their default values.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           2 = A NULL options pointer is specified.

  calling parameters:

    options   r   COntourDrawOptions*    Pointer to a filled in contour drawing
                                         options structure.

*/

int CSWContourApi::con_DefaultDrawOptions (COntourDrawOptions *options)
{
    int           istat;

    istat = con_draw_obj.con_default_draw_options (options);
    return istat;

}  /*  end of function con_DefaultDrawOptions  */





/*
  ****************************************************************

                    c o n _ D r a w T e x t

  ****************************************************************

  function name:  con_DrawText                 (int)

  call sequence:  con_DrawText (textptr, lineptr, nlines)

  purpose:        Return line primitives for the vectors needed to
                  draw a contour label.  A single contour label is
                  input via the textptr parameter.  A number of
                  output line primitives are returned.  You should
                  use the con_FreeDrawing function to csw_Free the line
                  primitive list after you are finished with it.

  return value:   status code

                  1 = success
                 -1 = failure

  errors:
                 1 = memory allocation failure
                 2 = null textptr or null lineptr

  calling parameters:

    textptr    r    COntourTextPrim*       Pointer to a text primitive structure
                                           as returned from con_DrawLine.
    lineptr    w    COntourLinePrim**      Returned list of line primitives.
    nlines     w    int*                   Number of line primitives returned.

*/

int CSWContourApi::con_DrawText
             (COntourTextPrim *textptr, COntourLinePrim **lineptr,
              int *nlines)
{
    int                     istat;

    istat = con_draw_obj.con_draw_text (textptr, lineptr, nlines);
    return istat;

}  /*  end of function con_DrawText  */






/*
  ****************************************************************

        c o n _ C a l c C o n t o u r s F r o m D o u b l e

  ****************************************************************

  function name:  con_CalcContoursFromDouble              (int)

  call sequence:  con_CalcContoursFromDouble
                                   (grid, ncol, nrow,
                                    x1, y1, x2, y2, scale,
                                    contours, ncontours,
                                    faults, nfaults,
                                    options)

  purpose:        Calculate contour lines as set up via con_SetContourIntervals
                  of the specified grid.  The current contour options are also
                  used.  The grid may have null values which will not be contoured.

                  This is almost identical to con_CalcContours.  The limits of
                  the grid are adjusted to be relative to the lower left corner.
                  The x and y arrays returned in the contour line structures
                  are CSW_F values relative to the lower left corner.

  return value:   status code

                  1 = success
                 -1 = error

  errors:         1 = memory allocation error
                  2 = wild input parameter
                  3 = NULL grid pointer
                  4 = ncol and/or nrow less than 2.
                  5 = contours or ncontours is NULL
                  6 = specified limits are inconsistent
                  7 = more than 500 million nodes in the grid
                  8 = the grid will not support logarithmic contours
                      which are in effect from the most recent contour
                      option setting.
                  9 = The contour interval currently specified will
                      produce more than 2000 contour levels.

  calling parameters:

    grid          r   CSW_F*            2-d grid array of elevations
    ncol          r   int               number of columns in the grid
    nrow          r   int               number of rows in the grid
    x1            r   double            x coordinate of the lower left
                                        corner of the grid
    y1            r   double            y coordinate of lower left
    x2            r   double            x of upper right
    y2            r   double            y of upper right
    scale         r   CSW_F             scale in grid units per inch or 0.0
                                        if scaling is not known.  The scale
                                        is only used to suppress crowded
                                        contours and to help determine where
                                        smoothing of contours is needed.  The
                                        returned contour points are always in
                                        the grid units specified by x1, y1, x2,
                                        and y2.

    contours      r  COntourOutputRec**  Pointer to receive an array of contour
                                        lines.  The array is allocated by this
                                        function, but the application must csw_Free
                                        it by calling con_FreeContours at an
                                        appropriate time.

    ncontours     r  int*               The number of contour lines returned.
                        The contours array returned is NULL terminated, so
                        you can either use the returned ncontours value to
                        read the data or you can use the terminating NULL.

    faults        r  FAultLineStruct*     List of fault lines to use in contouring.
                                          Any discontinuity in the underlying grid
                                          should be put in this list.
    nfaults       r  int                  Number of fault lines.
    options       r  COntourCalcOptions*  Optional contour calc options structure.

*/

int CSWContourApi::con_CalcContoursFromDouble
                     (CSW_F *grid, int ncol, int nrow,
                      double x1, double y1, double x2, double y2, CSW_F scale,
                      COntourOutputRec **contours, int *ncontours,
                      FAultLineStruct *faults, int nfaults,
                      COntourCalcOptions *options)
{
    int               istat, i, j, nt;
    CSW_F         xt1, yt1, xt2, yt2;
    COntourOutputRec  *cptr;
    FAultLineStruct   *fp;
    POint3D           *p3d;

/*
 *  If faults are present, shift them to origin at
 *  the lower left of the grid.
 */
    if (faults && nfaults > 0) {
        for (i=0; i<nfaults; i++) {
            fp = faults + i;
            p3d = fp->points;
            nt = fp->num_points;
            for (j=0; j<nt; j++) {
                p3d[j].x -= x1;
                p3d[j].y -= y1;
            }
        }
    }

    grd_triangle_obj.grd_set_shifts_for_debug (x1, y1);

    xt1 = 0.0f;
    yt1 = 0.0f;
    xt2 = (CSW_F)(x2 - x1);
    yt2 = (CSW_F)(y2 - y1);

    istat = con_calc_obj.con_calc_contours
                             (grid, ncol, nrow,
                              xt1, yt1, xt2, yt2, scale,
                              contours, ncontours,
                              options);

/*
 *  If faults are present, shift them back to the proper origin
 */
    if (faults && nfaults > 0) {
        for (i=0; i<nfaults; i++) {
            fp = faults + i;
            p3d = fp->points;
            nt = fp->num_points;
            for (j=0; j<nt; j++) {
                p3d[j].x += x1;
                p3d[j].y += y1;
            }
        }
    }

/*
 *  Shift contour coordinates back to true coordinates.
 */
    for (i=0; i<*ncontours; i++) {
        cptr = *contours + i;
        for (j=0; j<cptr->npts; j++) {
            cptr->x[j] += (CSW_F)x1;
            cptr->y[j] += (CSW_F)y1;
        }
        cptr->expect_double = 0;
    }

    if (istat == -1  &&  options) {
        options->error_number = con_GetErr ();
    }

    grd_triangle_obj.grd_set_shifts_for_debug (0.0, 0.0);

    return istat;

}  /*  end of function con_CalcContoursFromDouble  */







/*
  ****************************************************************

       c o n _ S e t D r a w S c a l e F r o m D o u b l e

  ****************************************************************

  function name:  con_SetDrawScaleFromDouble         (int)

  call sequence:  con_SetDrawScaleFromDouble
                                   (gx1, gy1, gx2, gy2,
                                    px1, py1, px2, py2)

  purpose:        Set up scaling and transformation to use on
                  subsequent contour line drawing calls.  The
                  results of contour calculation are in grid units.
                  The contours need to be converted to plot (drawing)
                  units before display.

  return value:   status code

                  -1 = error
                   1 = success

  errors:          2 = mimimums are greater than or equal to maximums

  calling parameters:

    gx1    r    double        Lower left x grid coordinate.
    gy1    r    double        Lower left y grid coordinate.
    gx2    r    double        Upper right x grid coordinate.
    gy2    r    double        Upper right y grid coordinate.
    px1    r    CSW_F     Lower left plot page x coordinate.
    py1    r    CSW_F     Lower left plot page y coordinate.
    px2    r    CSW_F     Upper right plot page x
    py2    r    CSW_F     Upper right plot page y

*/

int CSWContourApi::con_SetDrawScaleFromDouble
                     (double gx1, double gy1, double gx2, double gy2,
                      CSW_F px1, CSW_F py1, CSW_F px2, CSW_F py2)
{
    int               istat;
    CSW_F         xt1, yt1, xt2, yt2;

    xt1 = (CSW_F)gx1;
    yt1 = (CSW_F)gy1;
    xt2 = (CSW_F)gx2;
    yt2 = (CSW_F)gy2;

    istat = con_draw_obj.con_set_draw_scale
                               (xt1, yt1, xt2, yt2,
                                px1, py1, px2, py2, 1);

    return istat;

}  /*  end of function con_SetDrawScaleFromDouble  */






/*
  ****************************************************************

       c o n _ C a l c C o l o r F i l l s F r o m D o u b l e

  ****************************************************************

  function name:    con_CalcColorFillsFromDouble       (int)

  call sequence:    con_CalcColorFillsFromDouble
                                       (grid, ncol, nrow,
                                        xmin, ymin, xmax, ymax, scale,
                                        fills, nfills,
                                        faults, nfaults, options)

  purpose:          Return the outlines of polygons that can be converted
                    to page coordinates and drawn to show color filled
                    contours.

  return value:     status code

                    -1 = error
                     1 = success

  errors:           1 = memory allocation error
                    2 = NULL grid pointer
                    3 = either ncol or nrow is less than 2
                    4 = either xmin is >= xmax or ymin >= ymax
                    5 = either fills or nfills is NULL
                    6 = no color bands were set up by con_SetColorBands
                        prior to calling this function.

  calling parameters:

    grid       r   CSW_F*        2-d grid array to contour
    ncol       r   int               number of columns in the grid
    nrow       r   int               number of rows in the grid
    xmin       r   double            minimum x of the grid
    ymin       r   double            minimum y of the grid
    xmax       r   double            maximum x of the grid
    ymax       r   double            maximum y of the grid
    scale      r   CSW_F         grid units per page unit
    fills      w   COntourFillRec**  returned array of fill structures
    nfills     w   int*              number of fill structures returned
    faults     r   FAultLineStruct*  Array of fault line structures.
    nfaults    r   int               Number of fault line structures.
    options    r   COntourCalcOptions*  optional options structure

*/

int CSWContourApi::con_CalcColorFillsFromDouble
                       (CSW_F *grid, int ncol, int nrow,
                        double xmin, double ymin, double xmax, double ymax,
                        CSW_F scale, COntourFillRec **fills, int *nfills,
                        FAultLineStruct *faults, int nfaults,
                        COntourCalcOptions *options)
{
    int                  istat, i, j, nt;
    CSW_F                xt1, yt1, xt2, yt2;
    COntourFillRec       *cptr;
    FAultLineStruct      *fp;
    POint3D              *p3d;

/*
 *  If faults are present, reorigin them to the lower left corner
 */
    if (faults && nfaults > 0) {
        for (i=0; i<nfaults; i++) {
            fp = faults + i;
            p3d = fp->points;
            nt = fp->num_points;
            for (j=0; j<nt; j++) {
                p3d[j].x -= xmin;
                p3d[j].y -= ymin;
            }
        }
    }

    xt1 = 0.0f;
    yt1 = 0.0f;
    xt2 = (CSW_F)(xmax - xmin);
    yt2 = (CSW_F)(ymax - ymin);

    istat = con_calc_obj.con_calc_color_fills
                               (grid, ncol, nrow,
                                xt1, yt1, xt2, yt2, scale,
                                fills, nfills,
                                options);

/*
 *  If faults are present, shift them back to the proper origin
 */
    if (faults && nfaults > 0) {
        for (i=0; i<nfaults; i++) {
            fp = faults + i;
            p3d = fp->points;
            nt = fp->num_points;
            for (j=0; j<nt; j++) {
                p3d[j].x += xmin;
                p3d[j].y += ymin;
            }
        }
    }

/*
 *  Shift contour coordinates back to true coordinates.
 */
    for (i=0; i<*nfills; i++) {
        cptr = *fills + i;
        for (j=0; j<cptr->npts; j++) {
            cptr->x[j] += (CSW_F)xmin;
            cptr->y[j] += (CSW_F)ymin;
        }
        cptr->expect_double = 0;
    }

    return istat;

}  /*  end of function con_CalcColorFillsFromDouble  */






/*
  ****************************************************************

         c o n _ R e s a m p l e F o r S m o o t h i n g

  ****************************************************************

  function name:    con_ResampleForSmoothing           int

  call sequence:    con_ResampleForSmoothing (grid, mask, ncol, nrow,
                                              x1, y1, x2, y2,
                                              gridout, maskout, ncolout, nrowout)

  purpose:          Resample a grid in exactly the same way as the internal
                    contouring code will resample it if smoothed contours
                    are specified.

  return value:     -1 = error allocating memory
                     1 = success

  calling parameters:

    grid       r   CSW_F*     Input grid array.
    mask       r   char*      Optional input mask array.
    ncol       r   int        Number of columns in the input grid.
    nrow       r   int        Number of rows in the input grid.
    x1         r   CSW_F      X coordinate of the lower left corner of he grid.
    y1         r   CSW_F      Y coordinate of the lower left corner.
    x2         r   CSW_F      X or upper right.
    y2         r   CSW_F      Y of upper right.
    gridout    w   CSW_F**    Pointer to receive resampled grid.
    maskout    w   char**     Optional pointer to receive resampled mask.
    ncolout    w   int*       Resampled number of columns.
    nrowout    w   int*       Resampled number of rows.

*/

int CSWContourApi::con_ResampleForSmoothing
                             (CSW_F *grid, char *mask, int ncol, int nrow,
                              CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                              CSW_F **gridout, char **maskout, int *ncolout,
                              int *nrowout)
{
    int            istat;

    istat = con_calc_obj.con_resample_for_smoothing
                                       (grid, mask, -ncol, nrow,
                                        x1, y1, x2, y2,
                                        gridout, maskout, ncolout, nrowout);
    return istat;

}  /*  end of function con_ResampleForSmoothing  */





/*
  ****************************************************************

            c o n _ C a l c C r o s s S e c t i o n

  ****************************************************************

  function name:    con_CalcCrossSection         (int)

  call sequence:    con_CalcCrossSection (gridlist, ngrid,
                                          sectionlist, nsection,
                                          lines, nlines,
                                          fills, nfills)

  purpose:          Calculate lines and polygons for cross
                    sections through a set of grids.  The returned
                    graphical objects are in the grid coordinate
                    system.

  return value:     -1 = error
                    1 = success

  errors:           1 = Memory allocation failure.
                    2 = The gridlist pointer is NULL.
                    3 = The ngrid parameter is less than 1.
                    4 = The sectionlist parameter is NULL.
                    5 = The nsection parameter is less than 1.
                    6 = One of the output parameters is a NULL pointer.
                    7 = A hard null value was found in one of the grids.

  calling parameters:

    gridlist    r    GRidStruct*              Array of grid structures with the cross
                                              section surfaces sorted from top down.
    ngrid       r    int                      Number of grid structures.
    sectionlist r    CRossSectionTraceStruct* Array of cross section structures.
    nsection    r    int                      Number of cross section structures.
    lines       w    CRossSectionLineStruct** Pointer which will receive an array of
                                              cross section line structures.
    nlines      w    int*                     Number of returned line structures.
    fills       w    CRossSectionFillStruct** Pointer which will receive an array of
                                              cross section fill structures.
    nfills      w    int*                     Number of returned fill structures.
    page_inits_per_inch
                r    CSW_F                    Scale factor to convert from screen inches to page units.


*/

int CSWContourApi::con_CalcCrossSection
                         (GRidStruct *gridlist, int ngrid,
                          CRossSectionTraceStruct *sectionlist, int nsection,
                          CRossSectionLineStruct **lines, int *nlines,
                          CRossSectionFillStruct **fills, int *nfills,
                          CSW_F page_units_per_inch)

{
    int                           istat;

    istat = con_xsect_obj.con_calc_cross_section
                                   (gridlist, ngrid,
                                    sectionlist, nsection,
                                    lines, nlines,
                                    fills, nfills,
                                    page_units_per_inch);
    return istat;

}  /*  end of function con_CalcCrossSection  */





/*
  ****************************************************************

    c o n _ C a l c C r o s s S e c t i o n P o l y g o n s

  ****************************************************************

  function name:    con_CalcCrossSectionPolygons       (int)

  call sequence:    con_CalcCrossSectionPolygons (topline, bottomline,
                                                  fills, nfills)

  purpose:          Calculate polygons between the two lines.  The
                    con_CalcCrossSection function also returns all
                    interlayer polygons.  This function is intended
                    for use when a line is edited, and you need to
                    adjust the polygons accordingly.

  return value:     -1 = error
                     1 = success

  errors:           1 = Memory allocation failure.

  calling parameters:

    topline      r   CRossSectionLineStruct*     Pointer to the top line structure.
    bottomline   r   CRossSectionLineStruct*     Pointer to the bottom line structure.
    fills        w   CRossSectionFillStruct**    Pointer that will receive the array
                                                 of cross section fill structures.
    nfills       w   int*                        Number of fills.
    page_inits_per_inch
                 r   CSW_F                       Scale factor to convert from screen inches to page units.

*/

int CSWContourApi::con_CalcCrossSectionPolygons
                                 (CRossSectionLineStruct *topline,
                                  CRossSectionLineStruct *bottomline,
                                  CRossSectionFillStruct **fills,
                                  int *nfills,
                                  CSW_F page_units_per_inch)

{
    int                         istat;

    istat = con_xsect_obj.con_calc_cross_section_polygons
                                            (topline, bottomline,
                                             fills, nfills,
                                             page_units_per_inch);
    return istat;

}  /*  end of function con_CalcCrossSectionPolygons  */





/*
  ****************************************************************

             c o n _ F r e e C r o s s S e c t i o n

  ****************************************************************

  function name:    con_FreeCrossSection        (int)

  call sequence:    con_FreeCrossSection (lines, nlines,
                                          fills, nfills)

  purpose:          Free the output from con_CalcCrossSection after
                    you have finished with it.  If any pointer is NULL
                    or any number of items is zero, that list of
                    structures is ignored.

  return value:     Always returns 1.

  errors:           None

  calling parameters:

    lines     r    CRossSectionLineStruct*     Array of line structures.
    nlines    r    int                         Number of line structures.
    fills     r    CRossSectionFillStruct*     Array of fill structures.
    nfills    r    int                         Number of fill structures.

*/

int CSWContourApi::con_FreeCrossSection
                         (CRossSectionLineStruct *lines, int nlines,
                          CRossSectionFillStruct *fills, int nfills)
{
    int                       istat;

    istat = con_xsect_obj.con_free_cross_section
                                   (lines, nlines,
                                    fills, nfills);
    return istat;

}  /*  end of function con_FreeCrossSection  */





/*
  ****************************************************************

       c o n _ F r e e C r o s s S e c t i o n D r a w i n g

  ****************************************************************

  function name:    con_FreeCrossSectionDrawing        (int)

  call sequence:    con_FreeCrossSectionDrawing (lines, nlines,
                                                 fills, nfills)

  purpose:          Free the output from con_DrawCrossSection after
                    you have finished with it.  If any pointer is NULL
                    or any number of items is zero, that list of
                    structures is ignored.

  return value:     Always returns 1.

  errors:           None

  calling parameters:

    lines     r    CRossSectionLinePrimitive*  Array of line primitive structures.
    nlines    r    int                         Number of line structures.
    fills     r    CRossSectionFillPrimitive*  Array of fill primitive structures.
    nfills    r    int                         Number of fill structures.

*/

int CSWContourApi::con_FreeCrossSectionDrawing
                         (CRossSectionLinePrimitive *lines, int nlines,
                          CRossSectionFillPrimitive *fills, int nfills)
{
    int                       istat;

    istat = con_xsect_obj.con_free_cross_section_drawing
                                   (lines, nlines,
                                    fills, nfills);
    return istat;

}  /*  end of function con_FreeCrossSectionDrawing  */




/*
  ****************************************************************

              c o n _ D r a w C r o s s S e c t i o n

  ****************************************************************

  function name:    con_DrawCrossSection           (int)

  call sequence:    con_DrawCrossSection (lines, nlines,
                                          fills, nfills,
                                          zflag, traceid,
                                          hscale, vscale,
                                          lprim, nlprim,
                                          fprim, nfprim)

  purpose:          Calculate line and fill primitives that can be
                    used to draw a cross section.  The cross section
                    will have labelled and ticked axes along with the
                    lines. text, and fills specified in the input.

  return value:     -1 = error
                     1 = success

  errors:           1 = Memory allocation error.
                    2 = NULL input array or zero input array count.
                    3 = NULL output pointer.
                    4 = The traceid does not exist in the input lines or fills.

  calling parameters:

    lines     r  CRossSectionLineStruct*      List of lines returned from
                                              con_CalcCrossSection.
    nlines    r  int                          Number of lines.
    fills     r  CRossSectionFillStruct*      List of fill structures returned
                                              from con_CalcCrossSection.
    nfills    r  int                          Number of fill structures.
    zflag     r  int                          Flag specifying if the z values in
                                              the cross section are positive
                                              upwards (GRD_POSITIVE_UP) or positive
                                              downwards (GRD_POSITIVE_DOWN).
    traceid   r  int                          Identifier of the cross section trace
                                              to draw.  If only one trace is present
                                              in the input structures, this can be
                                              set to -1, and every line, text and
                                              fill will be drawn.
    hscale    r  CSW_F                        World units per drawing unit for the horizontal
                                              axis of the cross section.
    vscale    r  CSW_F                        World units per drawing unit for the vertical axis.
    lprim     w  CRossSectionLinePrimitive**  Returned array of line primitives.
    nlprim    w  int*                         Number of line primitives.
    fprim     w  CRossSectionFillPrimitive**  Returned array of fill primitives.
    nfprim    w  int*                         Number of fill primitives.

*/

int CSWContourApi::con_DrawCrossSection
                         (CRossSectionLineStruct *lines, int nlines,
                          CRossSectionFillStruct *fills, int nfills,
                          int zflag, int traceid,
                          char *xcaption, char *zcaption,
                          CSW_F hscale, CSW_F vscale,
                          CRossSectionLinePrimitive **lprim, int *nlprim,
                          CRossSectionFillPrimitive **fprim, int *nfprim)
{
    int                        istat;

    istat = con_xsect_obj.con_draw_cross_section
                                   (lines, nlines,
                                    fills, nfills,
                                    zflag, traceid,
                                    xcaption, zcaption,
                                    hscale, vscale,
                                    lprim, nlprim,
                                    fprim, nfprim);
    return istat;

}  /*  end of function con_DrawCrossSection  */






/*
  ****************************************************************

               c o n _ A u t o G r i d L i m i t s

  ****************************************************************

  function name:    con_AutoGridLimits             (int)

  call sequence:    con_AutoGridLimits (grid, ncol, nrow,
                                        zmin, zmax)

  purpose:          Run the same algorithm as is used by contour calculation
                    to decide the min and max contours.  If there are
                    spikes in the grid, they will lie outside of the
                    returned min and max.  This function is especially
                    useful in calculating color bands for color fills where
                    you want the color fills to agree with the contour lines.

  return value:     1 = success
                   -1 = error

  errors:           2 = NULL grid array
                    3 = ncol or nrow is less than 2 or an unreasonably large number
                    4 = NULL zmin or zmax

  calling parameters:

*/

int CSWContourApi::con_AutoGridLimits
                       (CSW_F *grid, int ncol, int nrow,
                        CSW_F *zmin, CSW_F *zmax)
{
    int              istat;

    istat = con_calc_obj.con_auto_grid_limits (grid, ncol, nrow, zmin, zmax);
    return istat;

}  /*  end of function con_AutoGridLimits  */



/*
 **************************************************************************************

                          c o n _ S m o o t h T r i M e s h

 **************************************************************************************

  Subdivide and smooth the specified trimesh, replacing it with the smooth version.

*/
int CSWContourApi::con_SmoothTriMesh (
    NOdeStruct      **nodes, int *numnodes,
    EDgeStruct      **edges, int *numedges,
    TRiangleStruct  **tris,  int *numtris,
    int             sfact)
{

    int istat;

    istat =
      con_triangle_obj.con_smooth_trimesh (
        nodes, numnodes,
        edges, numedges,
        tris, numtris,
        sfact);

    return istat;

}





/*
 **************************************************************************************

            c o n _ C a l c T r i M e s h C o n t o u r s

 **************************************************************************************

  Trace contours through a tri mesh.  The trimesh must be structured in the same
  fashion as returned from the various tri mesh calculation funtions.

*/

int CSWContourApi::con_CalcTriMeshContours
                            (NOdeStruct *nodes, int nnodes,
                             EDgeStruct *edges, int nedges,
                             TRiangleStruct *triangles, int ntriangles,
                             double nullval,
                             COntourOutputRec **contours, int *ncontours,
                             COntourCalcOptions *options)
{
    int                   istat;

    istat = con_triangle_obj.con_contours_from_trimesh
                                      (triangles, ntriangles,
                                       edges, nedges,
                                       nodes, nnodes,
                                       nullval,
                                       contours, ncontours,
                                       options);
    return istat;

}  /* end of function con_CalcTriMeshContours */


/*
 *************************************************************************

             c o n _ C l i p C o n t o u r s T o P o l y g o n

 *************************************************************************

  Clip the specified contour lines to the specified set of polygons.  The
  output is another set of contour lines.  If an error occurs, the conclip
  pointer is set to NULL, the nconclip is zero and the return value is -1.
  The only possible reason for an error is memory allocation failure.

*/

int CSWContourApi::con_ClipContoursToPolygon
                              (int inside_outside_flag,
                               COntourOutputRec *contours, int ncontours,
                               double *xpoly, double *ypoly, int *ipoly, int npoly,
                               COntourOutputRec **conclip, int *nconclip)
{
    int                    istat;

    istat = con_calc_obj.con_clip_contours_to_polygon
                                         (inside_outside_flag,
                                          contours, ncontours,
                                          xpoly, ypoly, ipoly, npoly,
                                          conclip, nconclip);
    return istat;

}  /* end of function con_ClipContoursToPolygon */



/*
 * Make a copy of the specified contour output record.
 */
COntourOutputRec* CSWContourApi::con_CopyOutputRec (COntourOutputRec *cp)
{
    CSW_F              *x = NULL, *y = NULL;
    COntourOutputRec   *cp2 = NULL;

    bool  b_success = false;

    auto fscope = [&]()
    {
        if (!b_success) {
            csw_Free (cp2);
            cp2 = NULL;
            csw_Free (x);
            x = NULL;
            y = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

    if (cp == NULL) {
        return NULL;
    }

    cp2 = (COntourOutputRec *)csw_Malloc (sizeof(COntourOutputRec));
    if (cp2 == NULL) {
        return NULL;
    }

    memcpy (cp2, cp, sizeof(COntourOutputRec));

    x = (CSW_F *)csw_Malloc (cp->npts * 2 * sizeof(CSW_F));
    if (x == NULL) {
        return NULL;
    }
    y = x + cp->npts;

    memcpy (x, cp->x, cp->npts * sizeof(CSW_F));
    memcpy (y, cp->y, cp->npts * sizeof(CSW_F));

    cp2->x = x;
    cp2->y = y;

    b_success = true;

    return cp2;
}
