
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_graph.cc

    Implement the methods in the GPFGraph class.  This is a
    refactor of the old gpf_graph.c functions.

    This file has generic functions used to support graph drawing.
    The functions act as services to calculate graphical primitives
    that make up the axes, legend, curves etc. of a graph.
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "csw/jeasyx/private_include/gtx_graphP.h"

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/ply_calc.h"
#include "csw/utils/private_include/gpf_font.h"
#include "csw/utils/private_include/gpf_calcdraw.h"

#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/TextBounds.h"


#include "csw/utils/private_include/gpf_graphP.h"


/*
  ****************************************************************

                  g p f _ S t a r t G r a p h

  ****************************************************************

    Set the page coordinates for the corners of the graph to be drawn.

*/

int GPFGraph::gpf_StartGraph (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                    CSW_F units, int basetime, CSW_F page_units_per_inch)

{
    CSW_F         tiny;

    PageUnitsPerInch = page_units_per_inch;

    /* unused grparm1P.h variables */
    AxisOverflowFlag = AxisOverflowFlag;
    PieDirection = PieDirection;
    PieStartAngle = PieStartAngle;
    PieSortFlag = PieSortFlag;
    PieInsideLabels = PieInsideLabels;
    PieEdgeLabels = PieEdgeLabels;
    PieLegendLabels = PieLegendLabels;

    AxisBaseTime = basetime;
    timeutil_obj.csw_SetBaseTime (basetime);

    PageX1 = x1;
    PageY1 = y1;
    PageX2 = x2;
    PageY2 = y2;

    tiny = (x2 - x1 + y2 - y1) / 2000.f;

    PageX11 = x1 + tiny;
    PageY11 = y1 + tiny;
    PageX22 = x2 - tiny;
    PageY22 = y2 - tiny;

    GXmin = x1;
    GYmin = y1;
    GXmax = x2;
    GYmax = y2;

    Units = units;

    Xwide = (x2 - x1) * 100.f;
    Ywide = (y2 - y1) * 100.f;

    if (Xwide < 1000.) Xwide = 1000.;
    if (Ywide < 1000.) Ywide = 1000.;

    NumDec = 7;

    AutoLegendAnchor = 0;

    AxisMinValue = GPF_DEFAULT_AXIS_MIN_VALUE;
    AxisMaxValue = GPF_DEFAULT_AXIS_MAX_VALUE;
    AxisMajorInterval = GPF_DEFAULT_AXIS_MAJOR_INTERVAL;
    AxisMinorInterval = GPF_DEFAULT_AXIS_MINOR_INTERVAL;

    AxisTickGap = 0.0;

    return 1;

}  /*  end of function gpf_StartGraph  */




/*
  ****************************************************************

            g p f _ S e t G r a p h P a r a m e t e r

  ****************************************************************

    Set a parameter for calculating graph primitives.  One parameter
  at a time is set with this function.

*/

int GPFGraph::gpf_SetGraphParameter (int tag, GRaphUnion value)
{
    CSW_F      atmp;

    switch (tag) {

        case GPF_AXIS_TIME_FORMAT:
            AxisTimeFormat = value.ival;
            break;

        case GPF_AXIS_BASE_TIME:
            AxisBaseTime = value.ival;
            break;

        case GPF_AXIS_MONTH_FLAG:
            AxisMonthFlag = value.ival;
            break;

        case GPF_AXIS_DIRECTION_FLAG:
            AxisDirectionFlag = value.ival;
            break;

        case GPF_AXIS_LABEL_ANCHOR:
            AxisLabelAnchor = value.ival;
            break;

        case GPF_AXIS_LABEL_FONT:
            AxisLabelFont = value.ival;
            break;

        case GPF_AXIS_LABEL_COLOR:
            AxisLabelColor = value.ival;
            break;

        case GPF_AXIS_LABEL_FILL_COLOR:
            AxisLabelFillColor = value.ival;
            if (AxisLabelFillColor < 0) AxisLabelFillColor = -100000;
            break;

        case GPF_AXIS_LINE_COLOR:
            AxisLineColor = value.ival;
            break;

        case GPF_AXIS_CAPTION_COLOR:
            AxisCaptionColor = value.ival;
            break;

        case GPF_AXIS_CAPTION_FILL_COLOR:
            AxisCaptionFillColor = value.ival;
            if (AxisCaptionFillColor < 0) AxisCaptionFillColor = -100000;
            break;

        case GPF_AXIS_CAPTION_FONT:
            AxisCaptionFont = value.ival;
            break;

        case GPF_AXIS_MAJOR_GRID_PATTERN:
            AxisMajorGridPattern = value.ival;
            break;

        case GPF_AXIS_MAJOR_GRID_COLOR:
            AxisMajorGridColor = value.ival;
            break;

        case GPF_AXIS_MINOR_GRID_PATTERN:
            AxisMinorGridPattern = value.ival;
            break;

        case GPF_AXIS_MINOR_GRID_COLOR:
            AxisMinorGridColor = value.ival;
            break;

        case GPF_GRAPH_TITLE_FONT:
            GraphTitleFont = value.ival;
            break;

        case GPF_GRAPH_TITLE_COLOR:
            GraphTitleColor = value.ival;
            break;

        case GPF_GRAPH_TITLE_FILL_COLOR:
            GraphTitleFillColor = value.ival;
            if (GraphTitleFillColor < 0) GraphTitleFillColor = -100000;
            break;

        case GPF_GRAPH_TITLE_POS:
            GraphTitlePos = value.ival;
            break;

        case GPF_LEGEND_LABEL_FONT:
            LegendLabelFont = value.ival;
            break;

        case GPF_LEGEND_LABEL_COLOR:
            LegendLabelColor = value.ival;
            break;

        case GPF_LEGEND_LABEL_FILL_COLOR:
            LegendLabelFillColor = value.ival;
            break;

        case GPF_LEGEND_POSITION:
            LegendPosition = value.ival;
            if (LegendPosition < 1  ||  LegendPosition > MAX_LEGEND_POSITION) {
                LegendPosition = GTX_NO_LEGEND;
            }
            break;

        case GPF_LEGEND_BORDER_COLOR:
            LegendBorderColor = value.ival;
            break;

        case GPF_CURVE_EBAR_COLOR:
            CurveEbarColor = value.ival;
            break;

        case GPF_CURVE_SMOOTH_FLAG:
            CurveSmoothFlag = value.ival;
            break;

        case GPF_CURVE_SYMBOL_COLOR:
            CurveSymbolColor = value.ival;
            break;

        case GPF_CURVE_SYMBOL_SPACING:
            CurveSymbolSpacing = value.ival;
            break;

        case GPF_CURVE_SYMBOL_MASK:
            CurveSymbolMask = value.ival;
            break;

        case GPF_CURVE_SYMBOL:
            CurveSymbol = value.ival;
            break;

        case GPF_CURVE_PATTERN:
            CurvePattern = value.ival;
            break;

        case GPF_CURVE_COLOR:
            CurveColor = value.ival;
            break;

        case GPF_CURVE_FILL_PATTERN:
            CurveFillPattern = value.ival;
            break;

        case GPF_CURVE_FILL_COLOR:
            CurveFillColor = value.ival;
            if (CurveFillColor < 0) CurveFillColor = -100000;
            break;

        case GPF_CURVE_LABEL_COLOR:
            CurveLabelColor = value.ival;
            break;

        case GPF_CURVE_LABEL_FILL_COLOR:
            CurveLabelFillColor = value.ival;
            if (CurveLabelFillColor < 0) CurveLabelFillColor = -100000;
            break;

        case GPF_CURVE_LABEL_FONT:
            CurveLabelFont = value.ival;
            break;

        case GPF_CURVE_LABEL_SPACING:
            CurveLabelSpacing = value.ival;
            break;

        case GPF_CURVE_STYLE:
            CurveStyle = value.ival;
            break;

        case GPF_BAR_SHADOW_COLOR:
            CurveShadowColor = value.ival;
            break;

        case GPF_CURVE_BASE_COLOR:
            CurveBaseColor = value.ival;
            break;

        case GPF_CURVE_PATTERN_COLOR:
            CurvePatternColor = value.ival;
            break;

        case GPF_CURVE_BASE_PATTERN:
            CurveBasePattern = value.ival;
            break;

        case GPF_CURVE_BASE_THICK:
            CurveBaseThick = value.fval;
            break;

        case GPF_CURVE_BASE_VALUE:
            CurveBaseValue = value.fval;
            break;

        case GPF_CURVE_LABEL_SIZE:
            CurveLabelSize = value.fval;
            break;

        case GPF_CURVE_LABEL_THICK:
            CurveLabelThick = value.fval;
            break;

        case GPF_AXIS_LABEL_ANGLE:
            AxisLabelAngle = value.fval;
            break;

        case GPF_AXIS_LABEL_SIZE:
            AxisLabelSize = value.fval;
            break;

        case GPF_AXIS_LABEL_GAP:
            AxisLabelGap = value.fval;
            break;

        case GPF_AXIS_CAPTION_SIZE:
            AxisCaptionSize = value.fval;
            break;

        case GPF_AXIS_CAPTION_GAP:
            AxisCaptionGap = value.fval;
            break;

        case GPF_AXIS_MAJOR_INTERVAL:
            AxisMajorInterval = value.fval;
            break;

        case GPF_AXIS_MINOR_INTERVAL:
            AxisMinorInterval = value.fval;
            break;

        case GPF_AXIS_MIN_VALUE:
            AxisMinValue = value.fval;
            break;

        case GPF_AXIS_MAX_VALUE:
            AxisMaxValue = value.fval;
            break;

        case GPF_AXIS_MINOR_LENGTH:
            AxisMinorLength = value.fval;
            if (AxisMinorLength < 0.0) {
                atmp = -AxisMinorLength;
                if (atmp > AxisTickGap) {
                    AxisTickGap = atmp;
                }
            }
            break;

        case GPF_AXIS_MAJOR_LENGTH:
            AxisMajorLength = value.fval;
            if (AxisMajorLength < 0.0) {
                atmp = -AxisMajorLength;
                if (atmp > AxisTickGap) {
                    AxisTickGap = atmp;
                }
            }
            break;

        case GPF_AXIS_MINOR_THICK:
            AxisMinorThick = value.fval;
            break;

        case GPF_AXIS_MAJOR_THICK:
            AxisMajorThick = value.fval;
            break;

        case GPF_AXIS_MAJOR_GRID_THICK:
            AxisMajorGridThick = value.fval;
            break;

        case GPF_AXIS_MINOR_GRID_THICK:
            AxisMinorGridThick = value.fval;
            break;

        case GPF_AXIS_LINE_THICK:
            AxisLineThick = value.fval;
            break;

        case GPF_AXIS_LABEL_THICK:
            AxisLabelThick = value.fval;
            break;

        case GPF_AXIS_CAPTION_THICK:
            AxisCaptionThick = value.fval;
            break;

        case GPF_GRAPH_TITLE_SIZE:
            GraphTitleSize = value.fval;
            break;

        case GPF_GRAPH_TITLE_THICK:
            GraphTitleThick = value.fval;
            break;

        case GPF_GRAPH_TITLE_GAP:
            GraphTitleGap = value.fval;
            break;

        case GPF_LEGEND_LABEL_SIZE:
            LegendLabelSize = value.fval;
            break;

        case GPF_LEGEND_LABEL_THICK:
            LegendLabelThick = value.fval;
            break;

        case GPF_LEGEND_BORDER_THICK:
            LegendBorderThick = value.fval;
            break;

        case GPF_CURVE_THICK:
            CurveThick = value.fval;
            break;

        case GPF_CURVE_EBAR_THICK:
            CurveEbarThick = value.fval;
            break;

        case GPF_CURVE_EBAR_WIDTH:
            CurveEbarWidth = value.fval;
            break;

        case GPF_CURVE_SYMBOL_SIZE:
            CurveSymbolSize = value.fval;
            break;

        case GPF_CURVE_SYMBOL_THICK:
            CurveSymbolThick = value.fval;
            break;

        case GPF_BAR_WIDTH_PCT:
            if (value.fval > 1.0) value.fval /= 100.;
            BarWidthPct = value.fval;
            break;

        case GPF_BAR_GAP_PCT:
            if (value.fval > 1.0) value.fval /= 100.;
            BarGapPct = value.fval;
            break;

        case GPF_AXIS_MAJOR_GRID_DASHSCALE:
            AxisMajorGridDashscale = value.fval;
            break;

        case GPF_AXIS_MINOR_GRID_DASHSCALE:
            AxisMinorGridDashscale = value.fval;
            break;

        case GPF_CURVE_DASHSCALE:
            CurveDashscale = value.fval;
            break;

        case GPF_CURVE_BASE_DASHSCALE:
            CurveBaseDashscale = value.fval;
            break;

        case GPF_CURVE_FILL_SIZE:
            CurveFillPatternSize = value.fval;
            break;

        default:
            return -1;

    }

    if (AxisLabelThick > .3f * AxisLabelSize) {
        AxisLabelThick = .3f * AxisLabelSize;
    }
    if (AxisCaptionThick > .3f * AxisCaptionSize) {
        AxisCaptionThick = .3f * AxisCaptionSize;
    }

    AxisLabelAngle = AxisLabelAngle;
    AxisLabelAnchor = AxisLabelAnchor;

    return 1;


}  /*  end of function gpf_SetGraphParameter  */

/*
 *****************************************************************

  Call this directly after calculating an axis to get
  the axis margin size.

 *****************************************************************

*/

CSW_F GPFGraph::gpf_GetAxisMarginSize (void)
{
    return AxisMarginSize;
}


/*
  ****************************************************************

             g p f _ C a l c G r a p h A x i s

  ****************************************************************

  function name:  gpf_CalcGraphAxis        (int)

  call sequence:  gpf_CalcGraphAxis (position, caption, datamin, datamax,
                                     logbase, labelflag,
                                     labels, labelpos, nlabels,
                                     axmin, axmax, axmajor,
                                     lines, nlines, text, ntext)

  purpose:        Calculate lines and text for an axis.  The axis is based on
                  the page coordinate values of the graph corners that have
                  been specified by a call to gpf_SetGraphArea.  This function
                  is called only from the EasyX server code and therefore it
                  does no error checking on the input parameters.

  return value:   status code

                  -1 = error allocating memory for graphical primitives
                   1 = normal successful completion

  calling parameters:

    position    r   int             GPF_TOP_AXIS, GPF_BOTTOM_AXIS, GPF_LEFT_AXIS,
                                    or GPF_RIGHT_AXIS
    caption     r   char*           Text to draw along the axis or NULL if no
                                    caption is desired.
    datamin     r   CSW_F           Minimum value of data to be graphed on the axis.
    datamax     r   CSW_F           Maximum data value.
    logbase     r   CSW_F           Base for lagarithmic axis.  If less than or equal
                                    to 1, a linear axis is calculated.
    labelflag   r   int             1 = label major intervals, 0 = no labels
    labels      r   char**          Array of pointers to custom label strings.
    labelpos    r   CSW_F*          Array of custom label positions.
    nlabels     r   int             Number of custom labels.
    axmin       w   CSW_F *         Returned axis minimum value.
    axmax       w   CSW_F *         Returned axis maximum value.
    axmajor     w   CSW_F *         Returned major axis interval.
    lines       w   GRaphLineRec**  lines to draw for the axis
    nlines      w   int*            number of line structures
    text        w   GRaphTextRec**  text to draw for the axis
    ntext       w   int*            number of text structures

*/

int GPFGraph::gpf_CalcGraphAxis (int position, const char *caption,
                       CSW_F datamin, CSW_F datamax,
                       CSW_F logbase, int labelflag,
                       char **labels, CSW_F *labelpos, int nlabels,
                       CSW_F *axmin, CSW_F *axmax, CSW_F *axmajor,
                       GRaphLineRec **lines, int *nlines,
                       GRaphTextRec **text, int *ntext)
{
    int             nmajor, nminor, n, nc, majorgrid, minorgrid, nl,
                    i, istat, centerflag, nmtotal;
    CSW_F           major, minor, axismin, axismax, scale, lnbase;
    CSW_F           *xptr = NULL, *yptr = NULL, xt, yt,
                    tiny, tiny2, tlen, value, theight;
    CSW_F           pagedelta, xtmax, offsety, tang, diff,
                    *gridxptr = NULL, *gridyptr = NULL;
    CSW_F           xtlast, ytlast, aminsav, amaxsav, f1, xanchor;
    CSW_F           gxmin_orig, gxmax_orig, fdum;
    char            buf[200];
    GRaphLineRec    *lptr = NULL, *lptrlist = NULL;
    GRaphTextRec    *tptr = NULL, *tptrlist = NULL;

    CSW_F           page_width, page_height;
    CSW_F           label_width, label_height;
    int             ianchor;
    CSW_F           atmp;

    double          ddata;

    bool     bsuccess = false;
 
    auto fscope = [&]()
    {
        AxisLimitsFlag = 0;
        if (bsuccess == false) {
            csw_Free (xptr);
            gpf_FreeGraphLines (lptrlist, *nlines, 1);
            gpf_FreeGraphText (tptrlist, n);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    ddata = (double)(datamax - datamin);
    if (ddata < 0.0) ddata = -ddata;
    if (logbase > MIN_LOG_BASE) {
        if (ddata < 1.e-30) {
            datamax = datamin + 1.0f;
        }
    }
    else {
        if (ddata < 1.e-12) {
            datamax = datamin + 1.0f;
        }
    }

    AxisLimitsFlag = 1;
    AXmin = 1.e20f;
    AYmin = 1.e20f;
    AXmax = -1.e20f;
    AYmax = -1.e20f;

    AxisMarginSize = 0.0f;
    page_width = 0.0f;
    page_height = 0.0f;

    xt = 0.0f;
    yt = 0.0f;
    lnbase = 1.0f;

    lptrlist = NULL;
    tptrlist = NULL;
    lptr = NULL;
    tptr = NULL;
    *nlines = 0;

    majorgrid = 0;
    minorgrid = 0;
    if (AxisMajorGridPattern >= 0  &&  AxisMajorGridColor > 0) majorgrid = 1;
    if (AxisMinorGridPattern >= 0  &&  AxisMinorGridColor > 0) minorgrid = 1;

/*
    The number of major tick marks will draw about 10
    major tick marks but not closer than about once per plot unit.
*/
    nmajor = 10;

    if (position == GPF_TOP_AXIS  ||
        position == GPF_BOTTOM_AXIS) {
        n = (int)((PageX2 - PageX1) / Units);
    }
    else {
        n = (int)((PageY2 - PageY1) / Units);
    }

    if (n < nmajor) nmajor = n;

/*
    Convert the datamin and datamax values to their logs
    if the logbase is greater than 1.  If the axis min and
    max are manually specified, these need to be converted also.
    The original min and max are saved since they are private
    and the original values are restored after the converted
    values are not used any more.
*/
    aminsav = AxisMinValue;
    amaxsav = AxisMaxValue;
    AxisLogBase = 0;
    if (logbase > MIN_LOG_BASE) {
        lnbase = (CSW_F)log ((double)logbase);
        datamin = (CSW_F)log ((double)datamin) / lnbase;
        datamax = (CSW_F)log ((double)datamax) / lnbase;
        AxisLogBase = logbase;
        if (AxisMinValue < 1.e19f  &&  AxisMaxValue > -1.e19f) {
            if (AxisMinValue <= 0.0f) AxisMinValue = 1.e-10f;
            if (AxisMaxValue <= 0.0f) AxisMaxValue = 1.e-10f;
            if (AxisMinValue >= AxisMaxValue) {
                AxisMinValue = 1.e20f;
                AxisMaxValue = -1.e20f;
            }
            else {
                AxisMinValue = (CSW_F)log ((double)AxisMinValue) / lnbase;
                AxisMaxValue = (CSW_F)log ((double)AxisMaxValue) / lnbase;
            }
        }
    }

/*
    calculate the major and minor tick intervals
*/
    gpf_CalcAxisIntervals (datamin, datamax, nmajor,
                           &axismin, &axismax, &major, &minor);
    nmajor = (int)((axismax - axismin) / major + 1.1f);
    nminor = (int)((axismax - axismin) / minor + 1.1f);

    tiny = minor / 10.f;
    tiny2 = major - tiny;
    if (tiny < 0.0) tiny = -tiny;
    if (tiny2 < 0.0) tiny2 = -tiny2;

/*
    Restore the original min and max for the axis.
*/
    AxisMinValue = aminsav;
    AxisMaxValue = amaxsav;

/*
    get scale for drawing the axis
*/
    if (position == GPF_TOP_AXIS  ||  position == GPF_BOTTOM_AXIS) {
        pagedelta = PageX2 - PageX1;
    }
    else {
        pagedelta = PageY2 - PageY1;
    }
    scale = (axismax - axismin) / pagedelta;

/*
    Allocate space for the major tick mark points.
    The major ticks are output as a single polyline
    that includes the axis line and the tick marks.
*/
    xptr = (CSW_F *)csw_Malloc (nmajor * 16 * sizeof(CSW_F));
    if (!xptr) {
        return -1;
    }
    yptr = xptr + nmajor * 8;

/*
    Allocate space for the line structures needed
    to hold the axis lines and the optional axis grid lines.
*/
    n = 3;
    if (majorgrid) n += nmajor;
    if (minorgrid) n += nminor;
    lptrlist = (GRaphLineRec *)csw_Calloc (n * sizeof(GRaphLineRec));
    if (!lptrlist) {
        return -1;
    }
    lptr = lptrlist;

/*
    Calculate the major tick points and
    optional major grid lines for a top axis.
*/
    gxmin_orig = GXmin;
    gxmax_orig = GXmax;
    if (position == GPF_TOP_AXIS) {
        xptr[0] = PageX1;
        yptr[0] = PageY2 - AxisMajorLength;
        xptr[1] = PageX1;
        yptr[1] = PageY2;
        value = axismin + major;
        value = value / major + .01f;
        value = (CSW_F)floor((double)value) * major;
        if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
            fdum = value;
            RoundOffTime (&value, &fdum, major);
        }
        n = 2;
        for (;;) {
            if (axismin < axismax) {
                if (value > axismax - tiny) break;
            }
            else {
                if (value < axismax + tiny) break;
            }
            xt = (value - axismin) / scale + PageX1;
            xptr[n] = xt;
            yptr[n] = PageY2;
            n++;
            xptr[n] = xt;
            yptr[n] = PageY2 - AxisMajorLength;
            n++;
            xptr[n] = xt;
            yptr[n] = PageY2;
            n++;
            value += major;
            if (majorgrid) {
                gridxptr = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
                if (!gridxptr) {
                    return -1;
                }
                gridyptr = gridxptr + 2;
                gridxptr[0] = xt;
                gridxptr[1] = xt;
                gridyptr[0] = PageY1 + AxisMajorLength;
                gridyptr[1] = PageY2 - AxisMajorLength;
                lptr->x = gridxptr;
                lptr->y = gridyptr;
                lptr->npts = 2;
                lptr->linepat = AxisMajorGridPattern;
                lptr->linecolor = AxisMajorGridColor;
                lptr->linethick = MAJOR_GRID_THICK;
                lptr->dashscale = AxisMajorGridDashscale;
                lptr->bgflag = 1;
                lptr->smoothflag = 1;
                (*nlines)++;
                lptr++;
            }
        }
        xptr[n] = PageX2;
        yptr[n] = PageY2;
        n++;
        xptr[n] = PageX2;
        yptr[n] = PageY2 - AxisMajorLength;
        n++;
    }

/*
    Calculate the major tick points and
    optional major grid lines for a bottom axis.
*/
    if (position == GPF_BOTTOM_AXIS) {
        xptr[0] = PageX1;
        yptr[0] = PageY1 + AxisMajorLength;
        xptr[1] = PageX1;
        yptr[1] = PageY1;
        value = axismin + major;
        value = value / major + .01f;
        value = (CSW_F)floor((double)value) * major;
        if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
            fdum = value;
            RoundOffTime (&value, &fdum, major);
        }
        n = 2;
        for (;;) {
            if (axismin < axismax) {
                if (value > axismax - tiny) break;
            }
            else {
                if (value < axismax + tiny) break;
            }
            xt = (value - axismin) / scale + PageX1;
            xptr[n] = xt;
            yptr[n] = PageY1;
            n++;
            xptr[n] = xt;
            yptr[n] = PageY1 + AxisMajorLength;
            n++;
            xptr[n] = xt;
            yptr[n] = PageY1;
            n++;
            value += major;
            if (majorgrid) {
                gridxptr = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
                if (!gridxptr) {
                    return -1;
                }
                gridyptr = gridxptr + 2;
                gridxptr[0] = xt;
                gridxptr[1] = xt;
                gridyptr[0] = PageY1 + AxisMajorLength;
                gridyptr[1] = PageY2 - AxisMajorLength;
                lptr->x = gridxptr;
                lptr->y = gridyptr;
                lptr->npts = 2;
                lptr->linepat = AxisMajorGridPattern;
                lptr->linecolor = AxisMajorGridColor;
                lptr->linethick = MAJOR_GRID_THICK;
                lptr->dashscale = AxisMajorGridDashscale;
                lptr->bgflag = 1;
                lptr->smoothflag = 1;
                (*nlines)++;
                lptr++;
            }
        }
        xptr[n] = PageX2;
        yptr[n] = PageY1;
        n++;
        xptr[n] = PageX2;
        yptr[n] = PageY1 + AxisMajorLength;
        n++;
    }

/*
    Calculate the major tick points and
    optional major grid lines for a left axis.
*/
    if (position >= GPF_LEFT_AXIS  &&  position <= GPF_LEFT_AXIS + NAXIS) {
        atmp = AxisMajorLength;
        if (position == GPF_LEFT_AXIS) {
            xanchor = PageX1;
        }
        else {
            xanchor = gxmin_orig - atmp * 3.0f;
        }
        xptr[0] = xanchor + AxisMajorLength;
        yptr[0] = PageY1;
        xptr[1] = xanchor;
        yptr[1] = PageY1;
        value = axismin + major;
        value = value / major + .01f;
        value = (CSW_F)floor((double)value) * major;
        if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
            RoundOffTime (&value, &fdum, major);
        }
        n = 2;
        for (;;) {
            if (axismin < axismax) {
                if (value > axismax - tiny) break;
            }
            else {
                if (value < axismax + tiny) break;
            }
            yt = (value - axismin) / scale + PageY1;
            xptr[n] = xanchor;
            yptr[n] = yt;
            n++;
            xptr[n] = xanchor + AxisMajorLength;
            yptr[n] = yt;
            n++;
            xptr[n] = xanchor;
            yptr[n] = yt;
            n++;
            value += major;
            if (majorgrid) {
                gridxptr = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
                if (!gridxptr) {
                    return -1;
                }
                gridyptr = gridxptr + 2;
                gridxptr[0] = PageX1 + AxisMajorLength;
                gridxptr[1] = PageX2 - AxisMajorLength;
                if (position != 2) {
                    gridxptr[0] = PageX1;
                    gridxptr[1] = PageX2;
                }
                gridyptr[0] = yt;
                gridyptr[1] = yt;
                lptr->x = gridxptr;
                lptr->y = gridyptr;
                lptr->npts = 2;
                lptr->linepat = AxisMajorGridPattern;
                lptr->linecolor = AxisMajorGridColor;
                lptr->linethick = MAJOR_GRID_THICK;
                lptr->dashscale = AxisMajorGridDashscale;
                lptr->bgflag = 1;
                lptr->smoothflag = 1;
                (*nlines)++;
                lptr++;
            }
        }
        xptr[n] = xanchor;
        yptr[n] = PageY2;
        n++;
        xptr[n] = xanchor + AxisMajorLength;
        yptr[n] = PageY2;
        n++;
    }

/*
    Calculate the major tick points and
    optional major grid lines for a right axis.
*/
    if (position >= GPF_RIGHT_AXIS  &&  position <= GPF_RIGHT_AXIS + NAXIS) {
        atmp = AxisMajorLength;
        if (position == GPF_RIGHT_AXIS) {
            xanchor = PageX2;
        }
        else {
            xanchor = gxmax_orig + atmp * 3.0f;
        }
        xptr[0] = xanchor - AxisMajorLength;
        yptr[0] = PageY1;
        xptr[1] = xanchor;
        yptr[1] = PageY1;
        value = axismin + major;
        value = value / major + .01f;
        value = (CSW_F)floor((double)value) * major;
        if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
            RoundOffTime (&value, &fdum, major);
        }
        n = 2;
        for (;;) {
            if (axismin < axismax) {
                if (value > axismax - tiny) break;
            }
            else {
                if (value < axismax + tiny) break;
            }
            yt = (value - axismin) / scale + PageY1;
            xptr[n] = xanchor;
            yptr[n] = yt;
            n++;
            xptr[n] = xanchor - AxisMajorLength;
            yptr[n] = yt;
            n++;
            xptr[n] = xanchor;
            yptr[n] = yt;
            n++;
            value += major;
            if (majorgrid) {
                gridxptr = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
                if (!gridxptr) {
                    return -1;
                }
                gridyptr = gridxptr + 2;
                gridxptr[0] = PageX1 + AxisMajorLength;
                gridxptr[1] = PageX2 - AxisMajorLength;
                if (position != 6) {
                    gridxptr[0] = PageX1;
                    gridxptr[1] = PageX2;
                }
                gridyptr[0] = yt;
                gridyptr[1] = yt;
                lptr->x = gridxptr;
                lptr->y = gridyptr;
                lptr->npts = 2;
                lptr->linepat = AxisMajorGridPattern;
                lptr->linecolor = AxisMajorGridColor;
                lptr->linethick = MAJOR_GRID_THICK;
                lptr->dashscale = AxisMajorGridDashscale;
                lptr->bgflag = 1;
                lptr->smoothflag = 1;
                (*nlines)++;
                lptr++;
            }
        }
        xptr[n] = xanchor;
        yptr[n] = PageY2;
        n++;
        xptr[n] = xanchor - AxisMajorLength;
        yptr[n] = PageY2;
        n++;
    }

/*
    Add the major tick line to the list of returned line structures.
*/
    lptr->x = xptr;
    lptr->y = yptr;
    xptr = yptr = NULL;
    lptr->npts = n;
    lptr->linepat = 0;
    lptr->linecolor = AxisLineColor;
    lptr->linethick = AxisMajorThick;
    lptr->dashscale = 1.0;

    (*nlines)++;
    lptr++;

/*
    Add the axis line to the list of line structures.
*/
    xptr = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
    if (!xptr) {
        return -1;
    }
    yptr = xptr + 2;

    if (position == GPF_TOP_AXIS) {
        xptr[0] = PageX1;
        xptr[1] = PageX2;
        yptr[0] = PageY2;
        yptr[1] = PageY2;
    }
    else if (position == GPF_BOTTOM_AXIS) {
        xptr[0] = PageX1;
        xptr[1] = PageX2;
        yptr[0] = PageY1;
        yptr[1] = PageY1;
    }
    else if (position >= GPF_LEFT_AXIS  && position <= GPF_LEFT_AXIS + NAXIS) {
        atmp = AxisMajorLength;
        if (position == GPF_LEFT_AXIS) {
             xanchor = PageX1;
        }
        else {
            xanchor = gxmin_orig - atmp * 3.0f;
        }
        xptr[0] = xanchor;
        xptr[1] = xanchor;
        yptr[0] = PageY1;
        yptr[1] = PageY2;
    }
    else if (position >= GPF_RIGHT_AXIS  &&  position <= GPF_RIGHT_AXIS + NAXIS) {
        atmp = AxisMajorLength;
        if (position == GPF_RIGHT_AXIS) {
            xanchor = PageX2;
        }
        else {
            xanchor = gxmax_orig + atmp * 3.0f;
        }
        xptr[0] = xanchor;
        xptr[1] = xanchor;
        yptr[0] = PageY1;
        yptr[1] = PageY2;
    }

    lptr->x = xptr;
    lptr->y = yptr;
    xptr = yptr = NULL;
    lptr->npts = 2;
    lptr->linepat = 0;
    lptr->linecolor = AxisLineColor;
    lptr->linethick = AxisLineThick;
    lptr->dashscale = 1.0;

    (*nlines)++;
    lptr++;

/*
    If this is a logarithmic axis, and the log base is less than 3,
    do not draw any minor tick marks or minor grid lines.
*/
    if (logbase > MIN_LOG_BASE  &&  logbase < MIN_MINOR_LOG_BASE) {
        goto END_OF_MINOR_TICKS;
    }

/*
    Allocate space for the minor tick mark points.
    The minor ticks are output as a single polyline
    that includes the axis line and the tick marks.
*/
    xptr = (CSW_F *)csw_Malloc (nminor * 12 * sizeof(CSW_F));
    if (!xptr) {
        return -1;
    }
    yptr = xptr + nminor * 6;

    tiny = minor / 10.f;
    tiny2 = major - tiny;
    if (tiny < 0.0) tiny = -tiny;
    if (tiny2 < 0.0) tiny2 = -tiny2;

/*
    Calculate the minor tick points and
    the optional minor grid lines for a top axis.
*/
    if (position == GPF_TOP_AXIS) {
        xptr[0] = PageX1;
        yptr[0] = PageY2;
        value = axismin + minor;
        value = value / minor + .01f;
        value = (CSW_F)floor((double)value) * minor;
        if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
            RoundOffTime (&value, &fdum, major);
        }
        xtlast = PageX1;
        n = 1;
        for (;;) {
            if (axismin < axismax) {
                if (value > axismax - tiny) break;
            }
            else {
                if (value < axismax + tiny) break;
            }
            diff = csw_FloatMod (value, major);
            if (diff < 0.0) diff = -diff;
            if (diff < tiny  ||  diff > tiny2) {
                xtlast = (value - axismin) / scale + PageX1;
                value += minor;
                continue;
            }
            xt = (value - axismin) / scale + PageX1;
            if (logbase > MIN_LOG_BASE) {
                xt = (CSW_F)log ((double)(diff * logbase)) / lnbase;
                if (xt < tiny) {
                    value += minor;
                    continue;
                }
                xt *= major / scale;
                xt += xtlast;
            }
            xptr[n] = xt;
            yptr[n] = PageY2;
            n++;
            xptr[n] = xt;
            yptr[n] = PageY2 - AxisMinorLength;
            n++;
            xptr[n] = xt;
            yptr[n] = PageY2;
            n++;
            value += minor;
            if (minorgrid) {
                gridxptr = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
                if (!gridxptr) {
                    return -1;
                }
                gridyptr = gridxptr + 2;
                gridxptr[0] = xt;
                gridxptr[1] = xt;
                gridyptr[0] = PageY1 + AxisMinorLength;
                gridyptr[1] = PageY2 - AxisMinorLength;
                lptr->x = gridxptr;
                lptr->y = gridyptr;
                lptr->npts = 2;
                lptr->linepat = AxisMinorGridPattern;
                lptr->linecolor = AxisMinorGridColor;
                lptr->linethick = MINOR_GRID_THICK;
                lptr->dashscale = AxisMinorGridDashscale;
                lptr->bgflag = 1;
                (*nlines)++;
                lptr++;
            }
        }
        xptr[n] = PageX2;
        yptr[n] = PageY2;
        n++;
    }

/*
    Calculate the minor tick points and
    the optional minor grid lines for a bottom axis.
*/
    if (position == GPF_BOTTOM_AXIS) {
        xptr[0] = PageX1;
        yptr[0] = PageY1;
        value = axismin + minor;
        value = value / minor + .01f;
        value = (CSW_F)floor((double)value) * minor;
        if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
            RoundOffTime (&value, &fdum, major);
        }
        xtlast = PageX1;
        n = 1;
        for (;;) {
            if (axismin < axismax) {
                if (value > axismax - tiny) break;
            }
            else {
                if (value < axismax + tiny) break;
            }
            diff = csw_FloatMod (value, major);
            if (diff < 0.0) diff = -diff;
            if (diff < tiny  ||  diff > tiny2) {
                xtlast = (value - axismin) / scale + PageX1;
                value += minor;
                continue;
            }
            xt = (value - axismin) / scale + PageX1;
            if (logbase > MIN_LOG_BASE) {
                xt = (CSW_F)log ((double)(diff * logbase)) / lnbase;
                if (xt < tiny) {
                    value += minor;
                    continue;
                }
                xt *= major / scale;
                xt += xtlast;
            }
            xptr[n] = xt;
            yptr[n] = PageY1;
            n++;
            xptr[n] = xt;
            yptr[n] = PageY1 + AxisMinorLength;
            n++;
            xptr[n] = xt;
            yptr[n] = PageY1;
            n++;
            value += minor;
            if (minorgrid) {
                gridxptr = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
                if (!gridxptr) {
                    return -1;
                }
                gridyptr = gridxptr + 2;
                gridxptr[0] = xt;
                gridxptr[1] = xt;
                gridyptr[0] = PageY1 + AxisMinorLength;
                gridyptr[1] = PageY2 - AxisMinorLength;
                lptr->x = gridxptr;
                lptr->y = gridyptr;
                lptr->npts = 2;
                lptr->linepat = AxisMinorGridPattern;
                lptr->linecolor = AxisMinorGridColor;
                lptr->linethick = MINOR_GRID_THICK;
                lptr->dashscale = AxisMinorGridDashscale;
                lptr->bgflag = 1;
                (*nlines)++;
                lptr++;
            }
        }
        xptr[n] = PageX2;
        yptr[n] = PageY1;
        n++;
    }

/*
    Calculate the minor tick points and
    the optional minor grid lines for a left axis.
*/
    if (position >= GPF_LEFT_AXIS  &&  position <= GPF_LEFT_AXIS + NAXIS) {
        atmp = AxisMajorLength;
        if (position == GPF_LEFT_AXIS) {
            xanchor = PageX1;
        }
        else {
            xanchor = gxmin_orig - atmp * 3.0f;
        }
        xptr[0] = xanchor;
        yptr[0] = PageY1;
        value = axismin + minor;
        value = value / minor + .01f;
        value = (CSW_F)floor((double)value) * minor;
        if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
            RoundOffTime (&value, &fdum, major);
        }
        ytlast = PageY1;
        n = 1;
        for (;;) {
            if (axismin < axismax) {
                if (value > axismax - tiny) break;
            }
            else {
                if (value < axismax + tiny) break;
            }
            diff = csw_FloatMod (value, major);
            if (diff < 0.0) diff = -diff;
            if (diff < tiny  ||  diff > tiny2) {
                ytlast = (value - axismin) / scale + PageY1;
                value += minor;
                continue;
            }
            yt = (value - axismin) / scale + PageY1;
            if (logbase > MIN_LOG_BASE) {
                yt = (CSW_F)log ((double)(diff * logbase)) / lnbase;
                if (yt < tiny) {
                    value += minor;
                    continue;
                }
                yt *= major / scale;
                yt += ytlast;
            }
            xptr[n] = xanchor;
            yptr[n] = yt;
            n++;
            xptr[n] = xanchor + AxisMinorLength;
            yptr[n] = yt;
            n++;
            yptr[n] = yt;
            xptr[n] = xanchor;
            n++;
            value += minor;
            if (minorgrid) {
                gridxptr = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
                if (!gridxptr) {
                    return -1;
                }
                gridyptr = gridxptr + 2;
                gridxptr[0] = PageX1 + AxisMinorLength;
                gridxptr[1] = PageX2 - AxisMinorLength;
                gridyptr[0] = yt;
                gridyptr[1] = yt;
                lptr->x = gridxptr;
                lptr->y = gridyptr;
                lptr->npts = 2;
                lptr->linepat = AxisMinorGridPattern;
                lptr->linecolor = AxisMinorGridColor;
                lptr->linethick = MINOR_GRID_THICK;
                lptr->dashscale = AxisMinorGridDashscale;
                lptr->bgflag = 1;
                (*nlines)++;
                lptr++;
            }
        }
        xptr[n] = xanchor;
        yptr[n] = PageY2;
        n++;
    }

/*
    Calculate the minor tick points and
    the optional minor grid lines for a right axis.
*/
    if (position >= GPF_RIGHT_AXIS  &&  position <= GPF_RIGHT_AXIS + NAXIS) {
        atmp = AxisMajorLength;
        if (position == GPF_RIGHT_AXIS) {
            xanchor = PageX2;
        }
        else {
            xanchor = gxmax_orig + atmp * 3.0f;
        }
        xptr[0] = xanchor;
        yptr[0] = PageY1;
        value = axismin + minor;
        value = value / minor + .01f;
        value = (CSW_F)floor((double)value) * minor;
        if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
            RoundOffTime (&value, &fdum, major);
        }
        ytlast = PageY1;
        n = 1;
        for (;;) {
            if (axismin < axismax) {
                if (value > axismax - tiny) break;
            }
            else {
                if (value < axismax + tiny) break;
            }
            diff = csw_FloatMod (value, major);
            if (diff < 0.0) diff = -diff;
            if (diff < tiny  ||  diff > tiny2) {
                ytlast = (value - axismin) / scale + PageY1;
                value += minor;
                continue;
            }
            yt = (value - axismin) / scale + PageY1;
            if (logbase > MIN_LOG_BASE) {
                yt = (CSW_F)log ((double)(diff * logbase)) / lnbase;
                if (yt < tiny) {
                    value += minor;
                    continue;
                }
                yt *= major / scale;
                yt += ytlast;
            }
            xptr[n] = xanchor;
            yptr[n] = yt;
            n++;
            xptr[n] = xanchor - AxisMinorLength;
            yptr[n] = yt;
            n++;
            yptr[n] = yt;
            xptr[n] = xanchor;
            n++;
            value += minor;
            if (minorgrid) {
                gridxptr = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
                if (!gridxptr) {
                    return -1;
                }
                gridyptr = gridxptr + 2;
                gridxptr[0] = PageX1 + AxisMinorLength;
                gridxptr[1] = PageX2 - AxisMinorLength;
                gridyptr[0] = yt;
                gridyptr[1] = yt;
                lptr->x = gridxptr;
                lptr->y = gridyptr;
                lptr->npts = 2;
                lptr->linepat = AxisMinorGridPattern;
                lptr->linecolor = AxisMinorGridColor;
                lptr->linethick = MINOR_GRID_THICK;
                lptr->dashscale = AxisMinorGridDashscale;
                lptr->bgflag = 1;
                (*nlines)++;
                lptr++;
            }
        }
        xptr[n] = xanchor;
        yptr[n] = PageY2;
        n++;
    }

/*
    add the minor tick line to the list of returned line structures
*/
    lptr->x = xptr;
    lptr->y = yptr;
    xptr = yptr = NULL;
    lptr->npts = n;
    lptr->linepat = 0;
    lptr->linecolor = AxisLineColor;
    lptr->linethick = AxisMinorThick;
    lptr->dashscale = 1.0;

    (*nlines)++;

/*
    The code branches to this label if a logarithmic axis has too
    small of a log base to support minor tick marks.
*/
  END_OF_MINOR_TICKS:

/*
    The remainder of this function calculates the
    labels and caption for the axis.
*/

    tptrlist = NULL;

/*
    Count the number of custom label lines.
*/
    if (labels  &&  nlabels > 0) {
        nmajor = 2;
        for (i=0; i<nlabels; i++) {
            gpf_HorTextBoxSize (dlist_index,
                                labels[i], AxisLabelFont, AxisLabelSize,
                                AxisLabelSize/3.0f, &tlen, &theight, &nl,
                                PageUnitsPerInch);
            nmajor += nl;
        }
    }

/*
    Use the number of major ticks for automatic labelling count.
*/
    else {
        if (labelflag) {
            nmajor += 2;
        }
        else {
            nmajor = 1;
        }
    }

/*
    Allocate space for text structures.
    The number of structures includes both axis
    labels (if desired) and the axis caption.

    I have allocated extra space here because a
    non repeatable crash happened when the graph
    was zoomed in a huge amount.  The crash appears to
    have accessed a pointer outside the array, so for insurance
    I have increased the size of the array.
*/
    nmtotal = nmajor * 2;
    if (nmtotal < 20) nmtotal = 20;
    tptrlist = (GRaphTextRec *)csw_Malloc (nmtotal * sizeof (GRaphTextRec));
    if (!tptrlist) {
        return -1;
    }
    tptr = tptrlist;

/*
    Calculate the custom labels
*/
    if (labels  &&  nlabels > 0) {

        n = 0;
        offsety = 0.0;
        xtmax = 0.0;
        for (i=0; i<nlabels; i++) {

            gpf_HorTextBoxSize (dlist_index,
                                labels[i], AxisLabelFont, AxisLabelSize,
                                AxisLabelSize/3.0f, &tlen, &theight, &nl,
                                PageUnitsPerInch);
            if (theight > offsety) offsety = theight;
            if (tlen > xtmax) xtmax = tlen;
            if (theight > page_height) page_height = theight;
            if (tlen > page_width) page_width = tlen;
            centerflag = 2;

            if (position == GPF_TOP_AXIS) {
                xt = (labelpos[i] - axismin) / scale + PageX1;
                yt = PageY2 + AxisTickGap + AxisLabelGap + (nl-1) * AxisLabelSize * 1.3f;
                centerflag = 2;
            }

            else if (position == GPF_BOTTOM_AXIS) {
                xt = (labelpos[i] - axismin) / scale + PageX1;
                yt = PageY1 - AxisTickGap - AxisLabelGap - AxisLabelSize;
                centerflag = 2;
            }

            else if (position >= GPF_LEFT_AXIS  &&  position <= GPF_LEFT_AXIS + NAXIS) {
                if (position == GPF_LEFT_AXIS) {
                    xanchor = PageX1;
                }
                else {
                    xanchor = gxmin_orig - AxisMajorLength * 3.0f;
                }
                yt = (labelpos[i] - axismin) / scale + PageY1;
                yt += theight / 2.0f;
                yt -= AxisLabelSize;
                xt = xanchor - AxisTickGap - AxisLabelGap - tlen;
                centerflag = 0;
            }

            else if (position >= GPF_RIGHT_AXIS  &&  position <= GPF_RIGHT_AXIS + NAXIS) {
                if (position == GPF_RIGHT_AXIS) {
                    xanchor = PageX2;
                }
                else {
                    xanchor = gxmax_orig + AxisMajorLength * 3.0f;
                }
                yt = (labelpos[i] - axismin) / scale + PageY1;
                yt += theight / 2.0f;
                yt -= AxisLabelSize;
                xt = xanchor + AxisTickGap + AxisLabelGap;
                centerflag = 0;
            }

            istat = MultiLineTextPrims (&tptr, labels[i], &n, centerflag,
                                        xt, yt, AxisLabelSize, AxisLabelSize/3.0f,
                                        AxisLabelThick, 0.0f, AxisLabelColor,
                                        AxisLabelFillColor, AxisLabelFont);
            if (istat == -1) {
                return -1;
            }

            UpdateLimits (xt-tlen/2.0f - AxisLabelSize*.2f,
                          yt - (nl-1)*AxisLabelSize*1.3f - AxisLabelSize*.7f);
            UpdateLimits (xt+tlen/2.0f + AxisLabelSize*.2f, yt + AxisLabelSize*1.2f);

        }

        xtmax += AxisLabelGap;
        xtmax += AxisTickGap;

        goto END_OF_AXIS_LABELS;

    }

/*
    Draw numeric value labels at major ticks if desired.
*/
    n = 0;
    xtmax = 0.0;
    offsety = AxisLabelSize;
    if (labelflag) {

    /*
        Labels for a top axis
    */
        AxisFormatFlag = 1;
        if (position == GPF_TOP_AXIS) {
            value = axismin;
            f1 = value / major + .01f;
            f1 = (CSW_F)floor ((double)f1) * major;
            if (!csw_NearEqual (f1, value, tiny/100.f)) {
                value += major;
                value = value / major + .01f;
                value = (CSW_F)floor ((double)value) * major;
                if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
                    RoundOffTime (&value, &fdum, major);
                }
            }
            for (;;) {
                if (axismin < axismax) {
                    if (value > axismax + tiny) break;
                }
                else {
                    if (value < axismax - tiny) break;
                }
                xt = (value - axismin) / scale + PageX1;
                FormatAxisNumber (value, logbase, buf);
                nc = strlen (buf);
                tlen = exact_text_length (buf, AxisLabelSize, AxisLabelFont);

              /*
               * The label is horizontal and has default anchoring.
               */
                if (AxisLabelAngle > -1.0  &&  AxisLabelAngle < 1.0 &&
                    AxisLabelAnchor == -1) {
                    xt = xt - tlen / 2.0f;
                    yt = PageY2 + AxisTickGap + AxisLabelGap;
                    label_height = AxisLabelSize;
                }

              /*
               * The label is not horizontal or not the default anchor.
               */
                else {
                    ianchor = AxisLabelAnchor;
                    if (ianchor < 1  ||  ianchor > 9) {
                        ianchor = 2;
                    }
                    CalcAxisTextAnchor (
                        xt,
                        PageY2,
                        ianchor,
                        tlen,
                        AxisLabelSize,
                        AxisLabelAngle,
                        &xt,
                        &yt,
                        &label_width,
                        &label_height);
                        yt += AxisTickGap + AxisLabelGap;
                }

                if (yt - PageY2 + label_height > page_height) {
                    page_height = yt - PageY2 + label_height;
                }

                tptr->text = (char *)csw_Malloc ((nc + 1) * sizeof(char));
                if (!tptr->text) {
                    return -1;
                }
                strcpy (tptr->text, buf);
                tptr->nchar = nc;
                tptr->x = xt;
                tptr->y = yt;
                tptr->thick = AxisLabelThick;
                tptr->size = AxisLabelSize;
                tptr->angle = AxisLabelAngle;
                tptr->color = AxisLabelColor;
                tptr->fillcolor = AxisLabelFillColor;
                tptr->font = AxisLabelFont;

                UpdateLimits (xt-AxisLabelSize*.2f, yt-AxisLabelSize*.2f);
                UpdateLimits (xt+tlen+AxisLabelSize*.2f, yt+AxisLabelSize*1.2f);

                value += major;
                if (AxisTimeFormat == CSW_NO_TIME_FORMAT) {
                    value = value / major + .01f;
                    value = (CSW_F)floor((double)value) * major;
                }
                tptr++;
                n++;
            }
        }

    /*
        Labels for a bottom axis
    */
        if (position == GPF_BOTTOM_AXIS) {
            value = axismin;
            f1 = value / major + .01f;
            f1 = (CSW_F)floor ((double)f1) * major;
            if (!csw_NearEqual (f1, value, tiny/100.f)) {
                value += major;
                value = value / major + .01f;
                value = (CSW_F)floor ((double)value) * major;
                if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
                    RoundOffTime (&value, &fdum, major);
                }
            }
            n = 0;
            for (;;) {
                if (axismin < axismax) {
                    if (value > axismax + tiny) break;
                }
                else {
                    if (value < axismax - tiny) break;
                }
                xt = (value - axismin) / scale + PageX1;
                FormatAxisNumber (value, logbase, buf);
                nc = strlen (buf);
                tlen = exact_text_length (buf, AxisLabelSize, AxisLabelFont);

              /*
               * The label is horizontal and has default anchoring.
               */
                if (AxisLabelAngle > -1.0  &&  AxisLabelAngle < 1.0 &&
                    AxisLabelAnchor == -1) {
                    xt = xt - tlen / 2.0f;
                    yt = PageY1 - AxisTickGap - AxisLabelGap - AxisLabelSize;
                }

              /*
               * The label is not horizontal or not the default anchor.
               */
                else {
                    ianchor = AxisLabelAnchor;
                    if (ianchor < 1  ||  ianchor > 9) {
                        ianchor = 8;
                    }
                    CalcAxisTextAnchor (
                        xt,
                        PageY1,
                        ianchor,
                        tlen,
                        AxisLabelSize,
                        AxisLabelAngle,
                        &xt,
                        &yt,
                        &label_width,
                        &label_height);
                        yt -= AxisTickGap + AxisLabelGap;
                }

                if (PageY1 - yt > page_height) {
                    page_height = PageY1 - yt;
                }

                tptr->text = (char *)csw_Malloc ((nc + 1) * sizeof(char));
                if (!tptr->text) {
                    return -1;
                }
                strcpy (tptr->text, buf);
                tptr->nchar = nc;
                tptr->x = xt;
                tptr->y = yt;
                tptr->thick = AxisLabelThick;
                tptr->size = AxisLabelSize;
                tptr->angle = AxisLabelAngle;
                tptr->color = AxisLabelColor;
                tptr->fillcolor = AxisLabelFillColor;
                tptr->font = AxisLabelFont;

                UpdateLimits (xt-AxisLabelSize*.2f, yt - AxisLabelSize*.7f);
                UpdateLimits (xt+tlen+AxisLabelSize*.2f, yt+AxisLabelSize*1.2f);

                value += major;
                if (AxisTimeFormat == CSW_NO_TIME_FORMAT) {
                    value = value / major + .01f;
                    value = (CSW_F)floor((double)value) * major;
                }
                tptr++;
                n++;
            }
        }

    /*
        Labels for a left axis
    */
        if (position >= GPF_LEFT_AXIS  &&  position <= GPF_LEFT_AXIS + NAXIS) {
            if (position == GPF_LEFT_AXIS) {
                xanchor = PageX1;
            }
            else {
                xanchor = gxmin_orig - AxisMajorLength * 3.0f;
            }
            value = axismin;
            f1 = value / major + .01f;
            f1 = (CSW_F)floor ((double)f1) * major;
            if (!csw_NearEqual (f1, value, tiny/100.f)) {
                value += major;
                value = value / major + .01f;
                value = (CSW_F)floor ((double)value) * major;
                if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
                    RoundOffTime (&value, &fdum, major);
                }
            }
            n = 0;
            for (;;) {
                if (axismin < axismax) {
                    if (value > axismax + tiny) break;
                }
                else {
                    if (value < axismax - tiny) break;
                }
                yt = (value - axismin) / scale + PageY1;
                FormatAxisNumber (value, logbase, buf);
                nc = strlen (buf);
                tlen = exact_text_length (buf, AxisLabelSize, AxisLabelFont);

              /*
               * The label is horizontal and has default anchoring.
               */
                if (AxisLabelAngle > -1.0  &&  AxisLabelAngle < 1.0 &&
                    AxisLabelAnchor == -1) {
                    xt = xanchor - tlen - AxisTickGap - AxisLabelGap;
                    if (xanchor - xt > xtmax) xtmax = xanchor - xt;
                    yt = yt - AxisLabelSize / 2.0f;
                }

              /*
               * The label is not horizontal or not the default anchor.
               */
                else {
                    ianchor = AxisLabelAnchor;
                    if (ianchor < 1  ||  ianchor > 9) {
                        ianchor = 6;
                    }
                    CalcAxisTextAnchor (
                        xanchor,
                        yt,
                        ianchor,
                        tlen,
                        AxisLabelSize,
                        AxisLabelAngle,
                        &xt,
                        &yt,
                        &label_width,
                        &label_height);
                        xt -= AxisTickGap + AxisLabelGap;
                }

                if (xanchor - xt > page_width) {
                    page_width = xanchor - xt;
                }

                tptr->text = (char *)csw_Malloc ((nc + 1) * sizeof(char));
                if (!tptr->text) {
                    return -1;
                }
                strcpy (tptr->text, buf);
                tptr->nchar = nc;
                tptr->x = xt;
                tptr->y = yt;
                if (axismin < axismax) {
                    if (value < axismin + tiny) {
                        tptr->y += AxisLabelSize/4.0f;
                    }
                    else if (value > axismax - tiny) {
                        tptr->y -= AxisLabelSize/4.0f;
                    }
                }
                else {
                    if (value > axismin - tiny) {
                        tptr->y += AxisLabelSize/4.0f;
                    }
                    else if (value < axismax + tiny) {
                        tptr->y -= AxisLabelSize/4.0f;
                    }
                }
                tptr->thick = AxisLabelThick;
                tptr->size = AxisLabelSize;
                tptr->angle = AxisLabelAngle;
                tptr->color = AxisLabelColor;
                tptr->fillcolor = AxisLabelFillColor;
                tptr->font = AxisLabelFont;

                UpdateLimits (xt-AxisLabelSize*.2f, yt-AxisLabelSize*.2f);
                UpdateLimits (xt+tlen+AxisLabelSize*.2f, yt+AxisLabelSize*1.2f);

                value += major;
                if (AxisTimeFormat == CSW_NO_TIME_FORMAT) {
                    value = value / major + .01f;
                    value = (CSW_F)floor((double)value) * major;
                }
                tptr++;
                n++;
            }
        }

    /*
        Labels for a right axis
    */
        if (position >= GPF_RIGHT_AXIS  &&  position <= GPF_RIGHT_AXIS + NAXIS) {
            if (position == GPF_RIGHT_AXIS) {
                xanchor = PageX2;
            }
            else {
                xanchor = gxmax_orig + AxisMajorLength * 3.0f;
            }
            value = axismin;
            f1 = value / major + .01f;
            f1 = (CSW_F)floor ((double)f1) * major;
            if (!csw_NearEqual (f1, value, tiny/100.f)) {
                value += major;
                value = value / major + .01f;
                value = (CSW_F)floor ((double)value) * major;
                if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
                    RoundOffTime (&value, &fdum, major);
                }
            }
            n = 0;
            for (;;) {
                if (axismin < axismax) {
                    if (value > axismax + tiny) break;
                }
                else {
                    if (value < axismax - tiny) break;
                }
                yt = (value - axismin) / scale + PageY1;
                FormatAxisNumber (value, logbase, buf);
                nc = strlen (buf);
                tlen = exact_text_length (buf, AxisLabelSize, AxisLabelFont);

              /*
               * The label is horizontal and has default anchoring.
               */
                if (AxisLabelAngle > -1.0  &&  AxisLabelAngle < 1.0 &&
                    AxisLabelAnchor == -1) {
                    xt = xanchor + AxisTickGap + AxisLabelGap;
                    if (xt + tlen - xanchor > xtmax) xtmax = xt + tlen - xanchor;
                    yt = yt - AxisLabelSize / 2.0f;
                    label_width = tlen;
                }

              /*
               * The label is not horizontal or not the default anchor.
               */
                else {
                    ianchor = AxisLabelAnchor;
                    if (ianchor < 1  ||  ianchor > 9) {
                        ianchor = 2;
                    }
                    CalcAxisTextAnchor (
                        xanchor,
                        yt,
                        ianchor,
                        tlen,
                        AxisLabelSize,
                        AxisLabelAngle,
                        &xt,
                        &yt,
                        &label_width,
                        &label_height);
                    xt += AxisTickGap + AxisLabelGap;
                }

                if (label_width + AxisTickGap + AxisLabelGap > page_width) {
                    page_width = label_width + AxisTickGap + AxisLabelGap;
                }

                tptr->text = (char *)csw_Malloc ((nc + 1) * sizeof(char));
                if (!tptr->text) {
                    return -1;
                }
                strcpy (tptr->text, buf);
                tptr->nchar = nc;
                tptr->x = xt;
                tptr->y = yt;
                if (axismin < axismax) {
                    if (value < axismin + tiny) {
                        tptr->y += AxisLabelSize/4.0f;
                    }
                    else if (value > axismax - tiny) {
                        tptr->y -= AxisLabelSize/4.0f;
                    }
                }
                else {
                    if (value > axismin - tiny) {
                        tptr->y += AxisLabelSize/4.0f;
                    }
                    else if (value < axismax + tiny) {
                        tptr->y -= AxisLabelSize/4.0f;
                    }
                }
                tptr->thick = AxisLabelThick;
                tptr->size = AxisLabelSize;
                tptr->angle = AxisLabelAngle;
                tptr->color = AxisLabelColor;
                tptr->fillcolor = AxisLabelFillColor;
                tptr->font = AxisLabelFont;

                UpdateLimits (xt-AxisLabelSize*.2f, yt-AxisLabelSize*.2f);
                UpdateLimits (xt+tlen+AxisLabelSize*.2f, yt+AxisLabelSize*1.2f);

                value += major;
                if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
                    value = value / major + .01f;
                    value = (CSW_F)floor((double)value) * major;
                }
                tptr++;
                n++;
            }
        }

/*
    End of axis labelling code block.
*/
    }

  END_OF_AXIS_LABELS:

/*
    Draw caption text parallel and centered
    on the axis if desired.
*/
    AxisFormatFlag = 0;
    if (caption) {

        csw_StrTruncate (buf, caption, 200);
        csw_StrLeftJust (buf);

        if (buf[0] != '\0') {
            nc = strlen (buf);
            tlen = exact_text_length (buf, AxisCaptionSize, AxisCaptionFont);
            tang = 0.0;

            if (position == GPF_TOP_AXIS) {
                xt = (PageX2 - PageX1 - tlen) / 2.0f + PageX1;
                yt = PageY2 + AxisCaptionGap + AxisCaptionSize / 3.0f;
                tang = 0.0f;
                yt += AxisTickGap;
                if (labelflag) yt += AxisLabelGap + offsety;
                UpdateLimits (xt-AxisCaptionSize*.2f, yt-AxisCaptionSize*.2f);
                UpdateLimits (xt+tlen+AxisCaptionSize*.2f, yt+AxisCaptionSize*1.2f);
                page_height += AxisCaptionGap + AxisCaptionSize;
            }
            if (position == GPF_BOTTOM_AXIS) {
                xt = (PageX2 - PageX1 - tlen) / 2.0f + PageX1;
                yt = PageY1 - AxisCaptionGap - AxisCaptionSize;
                tang = 0.0f;
                yt -= AxisTickGap;
                if (labelflag) yt -= AxisLabelGap + offsety;
                UpdateLimits (xt-AxisCaptionSize*.2f, yt-AxisCaptionSize);
                UpdateLimits (xt+tlen+AxisCaptionSize*.2f, yt+AxisCaptionSize*1.2f);
                page_height += AxisCaptionGap + AxisCaptionSize;
            }
            if (position >= GPF_LEFT_AXIS  &&  position <= GPF_LEFT_AXIS + NAXIS) {
                if (position == GPF_LEFT_AXIS) {
                    xanchor = PageX1;
                }
                else {
                    xanchor = gxmin_orig - AxisMajorLength * 3.0f;
                }
                yt = (PageY2 - PageY1 - tlen) / 2.0f + PageY1;
                xt = xanchor - AxisCaptionGap - xtmax - AxisCaptionSize / 3.0f;
                tang = 90.0f;
                UpdateLimits (xt-AxisCaptionSize*1.2f, yt-AxisCaptionSize*.2f);
                UpdateLimits (xt+AxisCaptionSize*.2f, yt+tlen+AxisCaptionSize*.2f);
                page_width += AxisCaptionGap + AxisCaptionSize;
            }
            if (position >= GPF_RIGHT_AXIS  &&  position <= GPF_RIGHT_AXIS + NAXIS) {
                if (position == GPF_RIGHT_AXIS) {
                    xanchor = PageX2;
                }
                else {
                    xanchor = gxmax_orig + AxisMajorLength * 3.0f;
                }
                yt = (PageY2 - PageY1 - tlen) / 2.0f + PageY1;
                xt = xanchor + AxisCaptionGap + xtmax + AxisCaptionSize;
                tang = 90.0f;
                UpdateLimits (xt-AxisCaptionSize*1.2f, yt-AxisCaptionSize*.2f);
                UpdateLimits (xt+AxisCaptionSize*.7f, yt+tlen-AxisCaptionSize*.2f);
                page_width += AxisCaptionGap + AxisCaptionSize;
            }

            tptr->text = (char *)csw_Malloc ((nc + 1) * sizeof(char));
            if (!tptr->text) {
                return -1;
            }
            strcpy (tptr->text, buf);
            tptr->nchar = nc;
            tptr->x = xt;
            tptr->y = yt;
            tptr->thick = AxisCaptionThick;
            tptr->size = AxisCaptionSize;
            tptr->angle = tang;
            tptr->color = AxisCaptionColor;
            tptr->fillcolor = AxisCaptionFillColor;
            tptr->font = AxisCaptionFont;

            tptr++;
            n++;
        }

    /*
        Free the tptrlist if there are no labels
        or captions for the axis.
    */
        else {
            if (!labelflag) {
                if (tptrlist) {
                    csw_Free (tptrlist);
                    tptrlist = NULL;
                }
            }
        }
    }

/*
    NULL caption pointer
*/
    else {
        if (!labelflag) {
            if (tptrlist) {
                csw_Free (tptrlist);
                tptrlist = NULL;
            }
        }
    }
/*
    End of block to process axis caption.
*/

/*
    Convert log axis limits back to linear
*/
    if (logbase > MIN_LOG_BASE) {
        axismin = (CSW_F)pow ((double)logbase, (double)axismin);
        axismax = (CSW_F)pow ((double)logbase, (double)axismax);
        AxisTimeFormat = 0;
    }

/*
    Return the line and text structures.
*/
    *lines = lptrlist;
    *text = tptrlist;
    *ntext = n;
    *axmin = axismin;
    *axmax = axismax;
    *axmajor = major;

    AxisLimitsFlag = 0;

    if (position == GPF_TOP_AXIS) {
        AxisMarginSize = page_height;
    }
    else if (position == GPF_BOTTOM_AXIS) {
        AxisMarginSize = page_height;
    }
    else if (position == GPF_LEFT_AXIS) {
        AxisMarginSize = page_width;
    }
    else if (position == GPF_RIGHT_AXIS) {
        AxisMarginSize = page_width;
    }

    bsuccess = true;

    return 1;

}  /*  end of function gpf_CalcGraphAxis  */




/*
  ****************************************************************

           g p f _ C a l c A x i s I n t e r v a l s

  ****************************************************************

  function name:  gpf_CalcAxisIntervals           (int)

  call sequence:  gpf_CalcAxisIntervals (datamin, datamax, nmajor,
                                         axismin, axismax, axismajor, axisminor)

  purpose:        Calculate good "even" values for major tick spacing, minor
                  tick spacing and the end points of a graph axis.  The values
                  are calculated to produce around nmajor tick marks on the
                  axis.

  return value:   status code

                    -1 = nmajor less than 1 or datamax less than or equal
                         to datamin.
                     1 = normal successful completion

  calling parameters:

    datamin    r   CSW_F     minimum value of data that will be graphed
                             on the axis.
    datamax    r   CSW_F     maximum data value
    nmajor     r   int       nominal number of major tick mark
    axismin    w   CSW_F*    returned minimum of the axis
    axismax    w   CSW_F*    returned maximum of the axis
    axismajor  w   CSW_F*    returned major interval on the axis
    axisminor  w   CSW_F*    returned minor interval on the axis

*/

int GPFGraph::gpf_CalcAxisIntervals (CSW_F datamin, CSW_F datamax, int nmajor,
                           CSW_F *axismin, CSW_F *axismax,
                           CSW_F *axismajor, CSW_F *axisminor)
{
    CSW_F      major, minor, min, max, origdelta, ftmp,
               delta, logdelta, base, mantissa;

/*
    If the min and max of the axis have been manually set,
    use the manual values to calculate major and minor intervals.
*/
    if (AxisMinValue < 1.e19f  &&  AxisMaxValue > -1.e19f) {
        delta = (AxisMaxValue - AxisMinValue) / 100.f;
        datamin = AxisMinValue + delta;
        datamax = AxisMaxValue - delta;
    }

/*
    Make sure axis min and max agree with direction flag.
*/
    else {
        if (AxisDirectionFlag == GTX_AXIS_MIN_FIRST) {
            if (datamin > datamax) {
                ftmp = datamin;
                datamin = datamax;
                datamax = ftmp;
            }
        }
        else {
            if (datamin < datamax) {
                ftmp = datamin;
                datamin = datamax;
                datamax = ftmp;
            }
        }
    }

/*
    check for obvious errors
*/
    if (nmajor < 1) {
        nmajor = 1;
    }

    origdelta = (datamax - datamin) / nmajor;
    delta = origdelta;
    if (origdelta < 0.0f) {
        delta = -delta;
        ftmp = datamin;
        datamin = datamax;
        datamax = ftmp;
    }
    if (delta <= NEAR_ZERO) {
        delta = NEAR_ZERO;
    }

    if (AxisMajorInterval > 0.0f) {
        delta = AxisMajorInterval;
    }

/*
    get the largest value that is a power of 10 and less than
    the previously calculated delta
*/
    logdelta = (CSW_F)log10 ((double)delta);
    logdelta = (CSW_F)floor ((double)logdelta);
    base = (CSW_F)pow ((double)10.0, (double)logdelta);
    mantissa = delta / base;

/*
    use a "round" value fairly near the actual delta value
    for the major interval and adjust the minor interval
    according to the major interval.
*/
    if (mantissa < 1.5f) {
        major = base;
        if (AxisMajorInterval > 0.0f) {
            major = AxisMajorInterval;
        }
        minor = major / 5.0f;
    }
    else if (mantissa < 3.0f) {
        major = 2.0f * base;
        if (AxisMajorInterval > 0.0f) {
            major = AxisMajorInterval;
        }
        minor = major / 4.0f;
    }
    else if (mantissa < 4.5f) {
        major = 4.0f * base;
        if (AxisMajorInterval > 0.0f) {
            major = AxisMajorInterval;
        }
        minor = major / 4.0f;
    }
    else if (mantissa < 7.5f) {
        major = 5.0f * base;
        if (AxisMajorInterval > 0.0f) {
            major = AxisMajorInterval;
        }
        minor = major / 5.0f;
    }
    else {
        major = 10.0f * base;
        if (AxisMajorInterval > 0.0f) {
            major = AxisMajorInterval;
        }
        minor = major / 5.0f;
    }

    if (AxisLogBase > MIN_LOG_BASE) {
        major = 1.0f;
        minor = 1.0f / AxisLogBase;
    }

/*
    time axis labels round differently
*/
    if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {

        if (delta < 5) {
            major = 5;
            minor = 1;
        }
        else if (delta < 15) {
            major = 10;
            minor = 2;
        }
        else if (delta < 25) {
            major = 20;
            minor = 4;
        }
        else if (delta < 45) {
            major = 30;
            minor = 5;
        }
        else if (delta < 95) {
            major = 60;
            minor = 10;
        }
        else if (delta < 150) {
            major = 120;
            minor = 30;
        }
        else if (delta < 450) {
            major = 300;
            minor = 60;
        }
        else if (delta < 750) {
            major = 600;
            minor = 120;
        }
        else if (delta < 1150) {
            major = 900;
            minor = 300;
        }
        else if (delta < 1350) {
            major = 1200;
            minor = 600;
        }
        else if (delta < 2700) {
            major = 1800;
            minor = 600;
        }
        else if (delta < 4500) {
            major = 3600;
            minor = 1200;
        }
        else if (delta < 3600 * 3) {
            major = 3600 * 2;
            minor = 1800;
        }
        else if (delta < 3600 * 8) {
            major = 3600 * 5;
            minor = 3600;
        }
        else if (delta < 3600 * 18) {
            major = 3600 * 12;
            minor = 3600 * 3;
        }
        else if (delta < 3600 * 72) {
            major = 3600 * 24;
            minor = 3600 * 6;
        }
        else {
            major = 3600 * 24 * 7;
            minor = 3600 * 24;
        }
    }

/*
    The axis minimum is the largest value evenly divisible
    by major and less than the datamin.  Axis max is smallest
    number evenly divisible by major and larger than datamax.
*/
    min = (CSW_F)floor ((double)(datamin/major)) * major;
    max = (CSW_F)ceil ((double)(datamax/major)) * major;

/*
    If this is a reversed direction axis, adjust for that.
*/
    if (origdelta < 0.0) {
        major = -major;
        minor = -minor;
        ftmp = min;
        min = max;
        max = ftmp;
    }

/*
    special handling for time data
*/
    RoundOffTime (&min, &max, major);

/*
    Return either the calculated values or the
    values set by the calling program.
*/
    *axismin = min;
    *axismax = max;
    *axismajor = major;
    *axisminor = minor;

    if (AxisMinValue < 1.e19f  &&  AxisMaxValue > -1.e19f) {
        *axismin = AxisMinValue;
        *axismax = AxisMaxValue;
    }

    if (AxisLogBase > MIN_LOG_BASE) {
        return 1;
    }

    if (AxisMajorInterval > 0.0  &&  AxisMinorInterval > 0.0) {
        *axismajor = AxisMajorInterval;
        *axisminor = AxisMinorInterval;
    }

    return 1;

}  /*  end of function gpf_CalcAxisIntervals  */




/*
  ****************************************************************

                  F o r m a t N u m b e r

  ****************************************************************

    Encode a linear or logarithmic number into a text buffer for
  subsequent use in a graph text structure.  This is used for
  axis labels.

*/

int GPFGraph::FormatNumber (CSW_F value, CSW_F logbase, char *buf)
{
    char         fmt[20], fmt2[10];
    int          ndec, ilog;
    CSW_F        fval;

/*
    calculate a time format if needed
*/
    if (AxisFormatFlag == 1) {
        if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
            timeutil_obj.csw_SetTimeFormat (AxisTimeFormat, AxisMonthFlag);
            timeutil_obj.csw_FormatTime ((int)value, buf);
            return 1;
        }
    }


    ndec = NumDec;
    fval = value;
    if (fval < 0.0) fval = -fval;
    if (ndec > 6) {
        if (fval == 0.0) {
            ilog = 1;
        }
        else {
            ilog = (int)log10 ((double)fval);
        }
        ilog--;
        if (fval < 0.5) {
            ndec = -ilog + 2;
        }
        else if (fval < 5.0) {
            ndec = 3;
        }
        else if (fval < 50.0) {
            ndec = 2;
        }
        else if (fval < 500.0) {
            ndec = 1;
        }
        else {
            ndec = 0;
        }
        if (ndec > 7) ndec = 7;
    }

    if (logbase > MIN_LOG_BASE) {
        value = (CSW_F)pow ((double)logbase, (double)value);
        strcpy (fmt, "%8.6g");
    }
    else {
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

    sprintf (buf, fmt, value);

    csw_StrLeftJust (buf);

    CleanupZeros (buf);

    if (buf[0] == '0') {
        if (buf[1] != '\0') {
            buf[0] = ' ';
            csw_StrLeftJust (buf);
        }
    }

    if (!strcmp (buf, "-0")) {
        strcpy (buf, "0");
    }

/*
    clean up repeating 9's at the end of a
    decimal number.
*/
    CleanupNines (buf);

    return 1;

}  /*  end of private FormatNumber function  */




/*
  ****************************************************************

                     C l e a n u p N i n e s

  ****************************************************************

    Delete repeating decimal nines at the end of a number and adjust the
  significant digits appropriately.

*/

void GPFGraph::CleanupNines (char *buf)
{
    char      clast, *cdot;
    int       i, idot, i1;

    cdot = strchr (buf, '.');
    if (!cdot) {
        return;
    }

    cdot++;
    if (*cdot == '\0') {
        return;
    }

    i = strlen (buf) - 1;
    i1 = i;

    clast = buf[i];
    if (clast != '9') return;

    while (buf[i-1] == clast) {
        buf[i] = '\0';
        i--;
    }

    if (buf[i-1] == '.'  ||  i1 == i) {
        return;
    }

    buf[i] = '\0';

    sscanf (cdot, "%d", &idot);
    idot++;
    sprintf (cdot, "%d", idot);

    return;

}  /*  end of private CleanupNines function  */




/*
  ****************************************************************

                     C l e a n u p Z e r o s

  ****************************************************************

    Delete repeating decimal zeros at the end of a number and adjust the
  significant digits appropriately.

*/

void GPFGraph::CleanupZeros (char *buf)
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
  ****************************************************************

             g p f _ F r e e G r a p h L i n e s

  ****************************************************************

    Free memory for graph line structures.  If flag = 1, csw_Free everything.
  If flag = 0, csw_Free everything but the x array.  If flag = 2, csw_Free only
  the x array.

*/

int GPFGraph::gpf_FreeGraphLines (GRaphLineRec *lines, int nlines, int flag)
{
    GRaphLineRec      *lptr;
    int               i;

    if (!lines) return 1;

    lptr = lines;
    for (i=0; i<nlines; i++) {
        if (lptr->npts <= 0  ||  lptr->x == NULL) {
            lptr++;
            continue;
        }
        if (!(flag == 0  &&  lptr->smoothflag == 1)) {
            csw_Free (lptr->x);
        }
        lptr++;
    }
    if (flag != 2) {
        csw_Free (lines);
    }

    return 1;

}  /*  end of function gpf_FreeGraphLines  */


/*
  ****************************************************************

             g p f _ F r e e G r a p h F i l l s

  ****************************************************************

    Free memory for graph fill structures.

*/

int GPFGraph::gpf_FreeGraphFills (GRaphFillRec *fills, int nfills)
{
    GRaphFillRec      *fptr;
    int               i;

    if (!fills) return 1;

    fptr = fills;
    for (i=0; i<nfills; i++) {
        csw_Free (fptr->x);
        fptr++;
    }
    csw_Free (fills);

    return 1;

}  /*  end of function gpf_FreeGraphFills  */



/*
  ****************************************************************

             g p f _ F r e e G r a p h A r c s

  ****************************************************************

    Free memory for graph arc structures.

*/

int GPFGraph::gpf_FreeGraphArcs (GRaphArcRec *arcs, int narcs)
{

    if (!arcs) return 1;

    csw_Free (arcs);

    narcs = narcs;

    return 1;

}  /*  end of function gpf_FreeGraphArcs  */




/*
  ****************************************************************

             g p f _ F r e e G r a p h T e x t

  ****************************************************************

    Free memory for graph text structures.

*/

int GPFGraph::gpf_FreeGraphText (GRaphTextRec *text, int ntext)
{
    GRaphTextRec      *tptr;
    int               i;

    if (!text) return 1;

    tptr = text;
    for (i=0; i<ntext; i++) {
        csw_Free (tptr->text);
        tptr++;
    }
    csw_Free (text);

    return 1;

}  /*  end of function gpf_FreeGraphText  */



/*
  ****************************************************************

              g p f _ C a l c G r a p h C u r v e

  ****************************************************************

  function name:   gpf_CalcGraphCurve      (int)

  call sequence:   gpf_CalcGraphCurve (x, y, ebar, npts, legtext, vflag,
                                       x2, y2, npts2,
                                       axmin, axmax, xlog,
                                       aymin, aymax, ylog,
                                       syms, nsyms, lines, nlines, text, ntext,
                                       baseline)

  purpose:         Calculate line and symbol primitives that make
                   up a graph curve.  Clip the primitives to the
                   current clipping limits.

  return value:    status code

                   1 = normal success
                  -1 = csw_Malloc error getting work space

  calling parameters:

    x          r   CSW_F*          array of graph x coordinates
    y          r   CSW_F*          array of graph y coordinates
    ebar       r   CSW_F*          array of error bars at points
    npts       r   int             number of points
    legtext    r   char*           legend text entry
    vflag      r   int             0 for horizontal independent axis,
                                   1 for vertical independent axis
    x2         r   CSW_F*          original x coordinates
    y2         r   CSW_F*          original y coordinates
    npts2      r   int             original number of coordinates
    axmin      r   CSW_F           minimum x axis value
    axmax      r   CSW_F           maximum x axis value
    xlog       r   CSW_F           log base for x axis
    aymin      r   CSW_F           minimum y axis value
    aymax      r   CSW_F           maximum y axis value
    ylog       r   CSW_F           log base for y axis
    syms       w   GRaphSymRec**   returned list of symbols
    nsyms      w   int*            number of returned symbols
    lines      w   GRaphLineRec**  returned list of lines
    nlinesin   w   int*            number of returned lines
    text       w   GRaphTextRec**  returned list of text primitives
    ntext      w   int*            number of returned text primititves
    baseline   w   GRaphLineRec**  returned base line pointer

*/

int GPFGraph::gpf_CalcGraphCurve (CSW_F *x, CSW_F *y, CSW_F *ebar, int npts,
                        const char *legtext, int vflag,
                        CSW_F *x2, CSW_F *y2, int npts2,
                        CSW_F axmin, CSW_F axmax, CSW_F xlog,
                        CSW_F aymin, CSW_F aymax, CSW_F ylog,
                        GRaphSymRec **syms, int *nsyms,
                        GRaphLineRec **lines, int *nlinesin,
                        GRaphTextRec **text, int *ntext,
                        GRaphLineRec **baseline)
{
    CSW_F         *xwork = NULL, *ywork = NULL, *ework = NULL, *ework2 = NULL,
                  *xptr = NULL, *yptr = NULL, *xebar = NULL, *yebar = NULL,
                  lnbasex, lnbasey, scalex, scaley, tiny, et, et2, xt, yt, ebw,
                  curvebase, savecurvebaseval, xzero, yzero,
                  *xptr2 = NULL, *yptr2 = NULL,
                  *xwork2 = NULL, *ywork2 = NULL;
    int           i, logflagx, logflagy, linesize, n, istat, nlines;
    GRaphSymRec   *sptr = NULL, *sptrlist = NULL;
    GRaphLineRec  *lptr = NULL, *lptrlist = NULL;
    GRaphTextRec  *tptr = NULL, *tptrlist = NULL;

    bool     bsuccess = false;

    savecurvebaseval = CurveBaseValue;

    auto fscope = [&]()
    {
        CurveBaseValue = savecurvebaseval;
        if (xptr != xptr2) csw_Free (xptr2);
        csw_Free (xptr);
        if (xwork != xwork2) csw_Free (xwork2);
        csw_Free (xwork);
        if (bsuccess == false) {
            gpf_FreeGraphLines (lptrlist, nlines, 1);
            csw_Free (sptrlist);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    *nsyms = 0;
    nlines = 0;
    if (nlinesin) {
        *nlinesin = 0;
    }
    *ntext = 0;

    if (lines == NULL) {
        i = LEGEND_POINT;
    }
    else {
        if (CurveSymbol > 0) {
            i = LEGEND_SYMB;
        }
        else {
            i = LEGEND_LINE;
        }
    }

    istat = DrawLegendOnly (i, CurveColor, CurveSymbolColor, CurvePattern, 0,
                            CurveSymbol, legtext);
    if (istat == 1) {
        return 1;
    }

    xzero = 0.0f;
    yzero = 0.0f;
    curvebase = 0.0f;
    lnbasex = 1.0f;
    lnbasey = 1.0f;

/*
    Check for logarithmic axes.
*/
    logflagx = 0;
    logflagy = 0;
    if (xlog > MIN_LOG_BASE) {
        xzero = axmin;
        lnbasex = (CSW_F)log ((double)xlog);
        axmin = (CSW_F)log ((double)axmin) / lnbasex;
        axmax = (CSW_F)log ((double)axmax) / lnbasex;
        logflagx = 1;
    }

    savecurvebaseval = CurveBaseValue;
    if (ylog > MIN_LOG_BASE) {
        yzero = aymin / 100.f;
        lnbasey = (CSW_F)log ((double)ylog);
        aymin = (CSW_F)log ((double)aymin) / lnbasey;
        aymax = (CSW_F)log ((double)aymax) / lnbasey;
        if (CurveBaseValue > 0.0f) {
            CurveBaseValue = (CSW_F)log ((double)CurveBaseValue) / lnbasey;
        }
        logflagy = 1;
    }
    tiny = (aymax - aymin) / 1000.0f;
    if (logflagy) tiny = 0.0f;

/*
    allocate work space for the line primitives
*/
    xwork = (CSW_F *)csw_Malloc (4 * npts * sizeof(CSW_F));
    if (!xwork) {
        return -1;
    }
    ywork = xwork + npts;
    ework = ywork + npts;
    ework2 = ework + npts;

    if (x != x2  &&  x2 != NULL) {
        xwork2 = (CSW_F *)csw_Malloc (4 * npts2 * sizeof(CSW_F));
        if (!xwork2) {
            return -1;
        }
        ywork2 = xwork2 + npts2;
        ework = ywork2 + npts2;
        ework2 = ework + npts2;
    }
    else {
        xwork2 = xwork;
        ywork2 = ywork;
        npts2 = npts;
    }

/*
    Put curve data into the work arrays, converting
    to log values if needed.
*/
    for (i=0; i<npts; i++) {

        xwork[i] = x[i];
        ywork[i] = y[i];

        if (logflagx) {
            xt = x[i];
            if (xt < xzero) xt = xzero;
            xwork[i] = (CSW_F)log ((double)xt) / lnbasex;
        }
        if (logflagy) {
            yt = y[i];
            if (yt < yzero) yt = yzero;
            ywork[i] = (CSW_F)log ((double)yt) / lnbasey;
        }
    }

/*
    Put ebar data into the work arrays, converting
    to log values if needed.
*/
    if (x2 != NULL  &&  y2 != NULL) {
        for (i=0; i<npts2; i++) {

            xwork2[i] = x2[i];
            ywork2[i] = y2[i];

            if (logflagx) {
                xt = x2[i];
                if (xt < xzero) xt = xzero;
                xwork2[i] = (CSW_F)log ((double)xt) / lnbasex;
            }
            if (logflagy) {
                yt = y2[i];
                if (yt < yzero) yt = yzero;
                ywork2[i] = (CSW_F)log ((double)yt) / lnbasey;
            }

            if (ebar) {
                if (ebar[i] < 0.0f) {
                    ework[i] = -1.0f;
                }
                else if (ebar[i] < tiny) {
                    ework[i] = 0.0f;
                }
                else {
                    ework[i] = ebar[i];
                }
            }

            if (logflagy) {
                yt = y2[i];
                if (yt < yzero) yt = yzero;
                if (ebar) {
                    if (ebar[i] < 0.0f) {
                        ework[i] = -1.0f;
                    }
                    else if (ebar[i] < tiny) {
                        ework[i] = ywork[i];
                        ework2[i] = ywork[i];
                    }
                    else {
                        et = ebar[i];
                        if (et > yt * 1.9999f) et = yt * 1.9999f;
                        ework[i] = (CSW_F)log ((double)(yt - et / 2.0)) / lnbasey;
                        ework2[i] = (CSW_F)log ((double)(yt + et / 2.0)) / lnbasey;
                    }
                }
            }
        }
    }

/*
    Allocate points for clipping of the curve and
    scale them into plotter coordinates.
*/
    xptr = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
    if (!xptr) {
        return -1;
    }
    yptr = xptr + npts;

    if (vflag) {
        scalex = (axmax - axmin) / (PageY2 - PageY1);
        scaley = (aymax - aymin) / (PageX2 - PageX1);
    }
    else {
        scalex = (axmax - axmin) / (PageX2 - PageX1);
        scaley = (aymax - aymin) / (PageY2 - PageY1);
    }

    for (i=0; i<npts; i++) {
        if (vflag) {
            yptr[i] = (xwork[i] - axmin) / scalex + PageY1;
            xptr[i] = (ywork[i] - aymin) / scaley + PageX1;
        }
        else {
            xptr[i] = (xwork[i] - axmin) / scalex + PageX1;
            yptr[i] = (ywork[i] - aymin) / scaley + PageY1;
        }
    }

/*
    If there is a separate coordinate array for symbols and ebars,
    make and scale a separate set of local arrays for these points
*/
    if (x2 != x  &&  x2 != NULL) {
        xptr2 = (CSW_F *)csw_Malloc (npts2 * 2 * sizeof(CSW_F));
        if (!xptr2) {
            return -1;
        }
        yptr2 = xptr2 + npts2;

        if (vflag) {
            scalex = (axmax - axmin) / (PageY2 - PageY1);
            scaley = (aymax - aymin) / (PageX2 - PageX1);
        }
        else {
            scalex = (axmax - axmin) / (PageX2 - PageX1);
            scaley = (aymax - aymin) / (PageY2 - PageY1);
        }

        for (i=0; i<npts2; i++) {
            if (vflag) {
                yptr2[i] = (xwork2[i] - axmin) / scalex + PageY1;
                xptr2[i] = (ywork2[i] - aymin) / scaley + PageX1;
            }
            else {
                xptr2[i] = (xwork2[i] - axmin) / scalex + PageX1;
                yptr2[i] = (ywork2[i] - aymin) / scaley + PageY1;
            }
        }
    }
    else {
        xptr2 = xptr;
        yptr2 = yptr;
        npts2 = npts;
    }

/*
    If the lines parameter specified in calling this function is NULL,
    then only symbols are to be calculated, so skip to the SYMBOLS_ONLY
    label.
*/
    if (lines == NULL) {
        lptrlist = NULL;
        goto SYMBOLS_ONLY;
    }

/*
    Clip the curve if needed.  Results are stored in
    Xclip, Yclip, and Nclip private variables.

    If the curve style is to draw to the axis rather
    than connect the points, the clipping is skipped.
*/
    if (CurveStyle != GTX_DRAW_TO_BASELINE) {
        istat = ClipLine (xptr, yptr, npts);
        if (istat == -1) {
            return -1;
        }
        if (npts == 0) {
            return 1;
        }
    }

/*
    allocate line primitive structures for
    the curve and error bars
*/
    if (CurveStyle != GTX_DRAW_TO_BASELINE) {
        linesize = sizeof(GRaphLineRec);
        if (ebar) {
            linesize = (npts2 + 1) * sizeof(GRaphLineRec);
        }
    }
    else {
        linesize = (npts + 1) * sizeof(GRaphLineRec);
        ebar = NULL;
    }

    lptrlist = (GRaphLineRec *)csw_Calloc (linesize);
    if (!lptrlist) {
        return -1;
    }
    lptr = lptrlist;
    *baseline = NULL;

/*
    Fill in line primitive struct for the clipped curve.
*/
    if (CurveStyle != GTX_DRAW_TO_BASELINE) {
        lptr->x = Xclip;
        lptr->y = Yclip;
        lptr->npts = Nclip;
        lptr->smoothflag = CurveSmoothFlag;
        lptr->linepat = CurvePattern;
        lptr->linecolor = CurveColor;
        lptr->linethick = CurveThick;
        lptr->dashscale = CurveDashscale;
        lptr++;
        nlines = 1;
        if (Xclip == NULL  ||  Yclip == NULL  ||  Nclip < 2)
            nlines = 0;
    }

/*
    Connect points to base value if that style is wanted.
*/
    else {
        nlines = 0;
        if (vflag) {
            curvebase = (CurveBaseValue - aymin) /scaley + PageX1;
            if (curvebase < PageX1) curvebase = PageX1;
            if (curvebase > PageX2) curvebase = PageX2;
        }
        else {
            curvebase = (CurveBaseValue - aymin) /scaley + PageY1;
            if (curvebase < PageY1) curvebase = PageY1;
            if (curvebase > PageY2) curvebase = PageY2;
        }
        for (i=0; i<npts; i++) {
            if (vflag) {
                if (yptr[i] < PageY1  ||  yptr[i] > PageY2) {
                    continue;
                }
                lptr->x1 = xptr[i];
                if (lptr->x1 < PageX11) lptr->x1 = PageX11;
                if (lptr->x1 > PageX22) lptr->x1 = PageX22;
                lptr->x2 = curvebase;
                lptr->y1 = yptr[i];
                lptr->y2 = yptr[i];
            }
            else {
                if (xptr[i] < PageX1  ||  xptr[i] > PageX2) {
                    continue;
                }
                lptr->x1 = xptr[i];
                lptr->x2 = xptr[i];
                lptr->y1 = yptr[i];
                if (lptr->y1 < PageY11) lptr->y1 = PageY11;
                if (lptr->y1 > PageY22) lptr->y1 = PageY22;
                lptr->y2 = curvebase;
            }
            lptr->npts = -2;
            lptr->smoothflag = 0;
            lptr->linepat = CurvePattern;
            lptr->linecolor = CurveColor;
            lptr->linethick = CurveThick;
            lptr->dashscale = CurveDashscale;
            lptr++;
            nlines++;
        }

    /*
        Return the base line in its own primitive if needed.
    */
        if (vflag) {
            if (CurveBaseColor >= 0  &&  curvebase > PageX11  &&  curvebase < PageX22) {
                lptr = (GRaphLineRec *)csw_Malloc (sizeof(GRaphLineRec));
                if (!lptr) {
                    *baseline = NULL;
                }
                else {
                    *baseline = lptr;
                    lptr->x1 = curvebase;
                    lptr->x2 = curvebase;
                    lptr->y1 = PageY1;
                    lptr->y2 = PageY2;
                    lptr->npts = -2;
                    lptr->smoothflag = 0;
                    lptr->linepat = CurveBasePattern;
                    lptr->linecolor = CurveBaseColor;
                    lptr->linethick = CurveBaseThick;
                    lptr->dashscale = CurveBaseDashscale;
                }
            }
        }
        else {
            if (CurveBaseColor >= 0  &&  curvebase > PageY11  &&  curvebase < PageY22) {
                lptr = (GRaphLineRec *)csw_Malloc (sizeof(GRaphLineRec));
                if (!lptr) {
                    *baseline = NULL;
                }
                else {
                    *baseline = lptr;
                    lptr->x1 = PageX1;
                    lptr->x2 = PageX2;
                    lptr->y1 = curvebase;
                    lptr->y2 = curvebase;
                    lptr->npts = -2;
                    lptr->smoothflag = 0;
                    lptr->linepat = CurveBasePattern;
                    lptr->linecolor = CurveBaseColor;
                    lptr->linethick = CurveBaseThick;
                    lptr->dashscale = CurveBaseDashscale;
                }
            }
        }
    }

    *lines = lptrlist;

/*
    Calculate line primitives for the
    errorbars if desired.
*/
    if (ebar && lptr != NULL) {

        ebw = CurveEbarWidth / 2.0f;

        for (i=0; i<npts2; i++) {

        /*
            If the central point is outside the clip area,
            skip this error bar.
        */
            if (xptr2[i] < ClipX1  ||  xptr2[i] > ClipX2  ||
                yptr2[i] < ClipY1  ||  yptr2[i] > ClipY2) {
                continue;
            }

            if (ebar[i] >= 0.0) {
                xebar = (CSW_F *)csw_Malloc (2 * NUM_EBAR_PTS * sizeof(CSW_F));
                if (!xebar) {
                    return -1;
                }
                yebar = xebar + NUM_EBAR_PTS;
                xt = xptr2[i];
                yt = yptr2[i];
                if (ebar[i] > tiny) {
                    if (logflagy) {
                        if (vflag) {
                            et = (ework[i] - aymin) / scaley + PageX1;
                            et2 = (ework2[i] - aymin) / scaley + PageX1;
                            et -= xt;
                            et2 -= xt;
                        }
                        else {
                            et = (ework[i] - aymin) / scaley + PageY1;
                            et2 = (ework2[i] - aymin) / scaley + PageY1;
                            et -= yt;
                            et2 -= yt;
                        }
                        xebar[0] = xt - ebw;
                        yebar[0] = yt + et;
                        xebar[1] = xt + ebw;
                        yebar[1] = yebar[0];
                        xebar[2] = xt;
                        yebar[2] = yebar[0];
                        xebar[3] = xt;
                        yebar[3] = yt + et2;
                        xebar[4] = xt - ebw;
                        yebar[4] = yebar[3];
                        xebar[5] = xt + ebw;
                        yebar[5] = yebar[3];
                    }
                    else {
                        et = ework[i] / scaley / 2.0f;
                        xebar[0] = xt - ebw;
                        yebar[0] = yt - et;
                        xebar[1] = xt + ebw;
                        yebar[1] = yebar[0];
                        xebar[2] = xt;
                        yebar[2] = yebar[0];
                        xebar[3] = xt;
                        yebar[3] = yt + et;
                        xebar[4] = xt - ebw;
                        yebar[4] = yebar[3];
                        xebar[5] = xt + ebw;
                        yebar[5] = yebar[3];
                    }
                    n = 6;
                }
                else {
                    xebar[0] = xt - ebw;
                    yebar[0] = yt;
                    xebar[1] = xt + ebw;
                    yebar[1] = yt;
                    n = 2;
                }
                if (vflag) {
                    RotateEbar (xebar, yebar, 6, xt, yt);
                }
                lptr->x = xebar;
                lptr->y = yebar;
                lptr->npts = n;
                lptr->linepat = 0;
                lptr->linecolor = CurveEbarColor;
                lptr->linethick = CurveEbarThick;
                lptr->dashscale = 1.0;
                nlines++;
                lptr++;
            }
        }
    }

/*
    This label is branched to if the lines parameter is NULL.
*/
  SYMBOLS_ONLY:

    if (xwork != xwork2) csw_Free (xwork2);
    csw_Free (xwork);
    xwork = xwork2 = NULL;

/*
    Put symbols on the curve if desired.
*/
    if (CurveSymbol > 0) {
        sptrlist = (GRaphSymRec *)csw_Malloc (npts2 * sizeof(GRaphSymRec));
        if (!sptrlist) {
            return -1;
        }
        sptr = sptrlist;
        *nsyms = 0;
        for (i=0; i<npts2; i++) {

        /*
            If the central point is outside the clip area,
            skip this symbol.
        */
            if (xptr2[i] < ClipX1  ||  xptr2[i] > ClipX2  ||
                yptr2[i] < ClipY1  ||  yptr2[i] > ClipY2) {
                continue;
            }

            if (i % CurveSymbolSpacing == 0  ||  lines == NULL) {
                sptr->symnum = CurveSymbol;
                sptr->mask = CurveSymbolMask;
                sptr->x = xptr2[i];
                sptr->y = yptr2[i];
                sptr->color = CurveSymbolColor;
                sptr->thick = CurveSymbolThick;
                sptr->size = CurveSymbolSize;
                sptr++;
                (*nsyms)++;
            }
        }
        *syms = sptrlist;
    }
    else {
        sptrlist = NULL;
        *syms = NULL;
        *nsyms = 0;
    }

    if (*nsyms == 0) {
        if (nlines == 0) {
            if (lptrlist)
                csw_Free (lptrlist);
            if (sptrlist)
                csw_Free (sptrlist);
            goto ALL_DONE;
        }
    }

    if (*nsyms == 0  &&  lines == NULL) {
        if (lptrlist)
            csw_Free (lptrlist);
        if (sptrlist)
            csw_Free (sptrlist);
        goto ALL_DONE;
    }

/*
    Label the curve points with the Y value if specified.
*/
    if (CurveLabelSpacing > 0) {
        tptrlist = (GRaphTextRec*)csw_Malloc (npts2 * sizeof(GRaphTextRec));
        if (!tptrlist) {
            return -1;
        }
        tptr = tptrlist;
        *ntext = 0;
        for (i=0; i<npts2; i++) {

        /*
            If the central point is outside the clip area,
            skip this label.
        */
            if (xptr2[i] < ClipX1  ||  xptr2[i] > ClipX2  ||
                yptr2[i] < ClipY1  ||  yptr2[i] > ClipY2) {
                continue;
            }

            if (i % CurveLabelSpacing == 0) {
                tptr->text = (char *)csw_Malloc (LABEL_LENGTH * sizeof(char));
                if (!tptr->text) {
                    return -1;
                }
                if (y2) FormatNumber (y2[i], 0.0, tptr->text);
                tptr->size = CurveLabelSize;
                tptr->thick = CurveLabelThick;
                tptr->angle = 0.0;
                tptr->nchar = strlen(tptr->text);
                tptr->fillcolor = CurveLabelFillColor;
                tptr->color = CurveLabelColor;
                tptr->font = CurveLabelFont;
                tptr->x = xptr2[i];
                tptr->y = yptr2[i];
                LabelLocation (tptr, vflag, curvebase);
                tptr++;
                (*ntext)++;
            }
        }
        *text = tptrlist;
    }
    else {
        tptrlist = NULL;
        *text = NULL;
        *ntext = 0;
    }

/*
    Add a legend entry if needed.
*/
    if (lines == NULL) {
        i = LEGEND_POINT;
    }
    else {
        if (CurveSymbol > 0) {
            i = LEGEND_SYMB;
        }
        else {
            i = LEGEND_LINE;
        }
    }

    istat = AddToLegend (i, CurveColor, CurveSymbolColor, CurvePattern, 0,
                         CurveSymbol, legtext);
    if (!istat) {
        return -1;
    }

 ALL_DONE:

    if (nlinesin) {
        *nlinesin = nlines;
    }

    bsuccess = true;

    return 1;

}  /*  end of function gpf_CalcGraphCurve  */





/*
  ****************************************************************

             g p f _ S e t G r a p h D e f a u l t s

  ****************************************************************

    Set all of the graph calculation parameters to their defaults.

*/

int GPFGraph::gpf_SetGraphDefaults (void)
{

/*
    The same symbols and values are used for defaults in
    the gtx layer and in this layer, so use a header file
    to maintain the default assignments.
*/
#include "csw/jeasyx/private_include/grparm3P.h"

    PageX1 = 0.0;
    PageY1 = 0.0;
    PageX2 = 0.0;
    PageY2 = 0.0;

    return 1;

}  /*  end of function gpf_SetGraphDefaults  */



/*
  ****************************************************************

                   U p d a t e L i m i t s

  ****************************************************************

*/

int GPFGraph::UpdateLimits (CSW_F x, CSW_F y)
{

    if (x < GXmin) GXmin = x;
    if (x > GXmax) GXmax = x;
    if (y < GYmin) GYmin = y;
    if (y > GYmax) GYmax = y;

    if (AxisLimitsFlag) {
        if (x < AXmin) AXmin = x;
        if (x > AXmax) AXmax = x;
        if (y < AYmin) AYmin = y;
        if (y > AYmax) AYmax = y;
    }

    return 1;

}  /*  end of function UpdateLimits  */




/*
  ****************************************************************

      g p f _ s e t _ g r a p h _ d r a w i n g _ l i m i t s

  ****************************************************************

*/

int GPFGraph::gpf_set_graph_drawing_limits (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2)
{

    GXmin = x1;
    GYmin = y1;
    GXmax = x2;
    GYmax = y2;

    return 1;

}  /*  end of function gtx_set_graph_drawing_limits  */




/*
  ****************************************************************

      g p f _ g e t _ g r a p h _ d r a w i n g _ l i m i t s

  ****************************************************************

*/

int GPFGraph::gpf_get_graph_drawing_limits (CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{

    *x1 = GXmin;
    *y1 = GYmin;
    *x2 = GXmax;
    *y2 = GYmax;

    return 1;

}  /*  end of function gtx_get_graph_drawing_limits  */





/*
  ****************************************************************

      g p f _ g e t _ a x i s _ d r a w i n g _ l i m i t s

  ****************************************************************

*/

int GPFGraph::gpf_get_axis_drawing_limits (CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{

    *x1 = AXmin;
    *y1 = AYmin;
    *x2 = AXmax;
    *y2 = AYmax;

    return 1;

}  /*  end of function gtx_get_axis_drawing_limits  */





/*
  ****************************************************************

             g p f _ C a l c G r a p h T i t l e

  ****************************************************************

    Return a text primitive struct for the graph title.

*/

int GPFGraph::gpf_CalcGraphTitle (const char *title,
                        GRaphTextRec **text, int *ntext)
{
    int             nlines;
    CSW_F           xt, yt, tlen, theight;
    GRaphTextRec    *tptr = NULL;

/*
    Check for obvious errors.
*/
    if (!title  ||  !text  ||  !ntext) {
        return -1;
    }

    *text = NULL;
    *ntext = 0;

/*
    Return success if the title is to be put in the legend.
*/
    if (GraphTitlePos == GTX_TITLE_IN_LEGEND) {
        return 1;
    }

/*
    Get the height and width of the text.
*/
    gpf_HorTextBoxSize (dlist_index,
                        title, GraphTitleFont, GraphTitleSize,
                        GraphTitleSize / 3.0f, &tlen, &theight, &nlines,
                        PageUnitsPerInch);

/*
    Set the text position depending on the TitlePosition flag,
    defaulting to top center.
*/
    switch (GraphTitlePos) {

        case GTX_TOP_LEFT_TITLE:
            xt = PageX1;
            yt = GYmax + GraphTitleGap;
            yt += (nlines - 1) * 1.33333f * GraphTitleSize;
            break;

        case GTX_BOTTOM_LEFT_TITLE:
            xt = PageX1;
            yt = GYmin - GraphTitleSize - GraphTitleGap;
            break;

        case GTX_BOTTOM_CENTER_TITLE:
            xt = PageX1 + (PageX2 - PageX1 - tlen) / 2.0f;
            yt = GYmin - GraphTitleSize - GraphTitleGap;
            break;

        default:
            xt = PageX1 + (PageX2 - PageX1 - tlen) / 2.0f;
            yt = GYmax + GraphTitleGap;
            yt += (nlines - 1) * 1.33333f * GraphTitleSize;
            break;
    }

/*
    Allocate the text structures.
*/
    tptr = (GRaphTextRec *)csw_Malloc (nlines * sizeof (GRaphTextRec));
    if (!tptr) {
        return -1;
    }

/*
    Fill in the text structures.
*/
    *ntext = 0;
    *text = tptr;
    MultiLineTextPrims (&tptr, title, ntext, 1,
                        xt, yt, GraphTitleSize, GraphTitleSize / 3.0f,
                        GraphTitleThick, 0.0f,
                        GraphTitleColor, GraphTitleFillColor, GraphTitleFont);


    return 1;

}  /*  end of function gpf_CalcGraphTitle  */




/*
  ****************************************************************

           g p f _ S e t G r a p h C l i p F l a g s

  ****************************************************************

    Set the clipping limits based on the plot limits and the
  clip flags specified.

*/

int GPFGraph::gpf_SetGraphClipFlags (int xflag, int yflag)
{

    if (!xflag) {
        ClipX1 = -Xwide;
        ClipX2 = Xwide;
    }
    else {
        ClipX1 = PageX1;
        ClipX2 = PageX2;
    }

    if (!yflag) {
        ClipY1 = -Ywide;
        ClipY2 = Ywide;
    }
    else {
        ClipY1 = PageY1;
        ClipY2 = PageY2;
    }

    return 1;

}  /*  end of function gpf_SetGraphClipFlags  */



/*
  ****************************************************************

                      C l i p L i n e

  ****************************************************************

  Clip the specified line to the current graph clip limits.
  The clipped segments are stored in private variables Xclip,
  Yclip, and Nclip.

*/

/*
    The following FREE_DATA macro is only used in function ClipLine.
    It is undefined at the end of this function.
*/
#define FREE_DATA      csw_Free (xyout);\
                       csw_Free (icout);\
                       csw_Free (xyin);\
                       csw_Free (xy2[0]); csw_Free (xy2[1]); \
                       csw_Free (Xclip); Xclip = NULL; Yclip = NULL;


int GPFGraph::ClipLine (CSW_F *xin, CSW_F *yin, int npts)
{
    CSW_F            *xyin = NULL, *xyout = NULL, *xyout2 = NULL, *xy2[2];
    int              n, ncout, *icout = NULL, istat, mflag1, mflag2,
                     memsize, nptsout2;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (xyout2);
        if (bsuccess == false) {
            FREE_DATA;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Initialize stuff so that FREE_DATA works correctly.
*/
    xyout = NULL;
    xyout2 = NULL;
    icout = NULL;
    mflag1 = 0;
    mflag2 = 0;
    Xclip = NULL;
    Yclip = NULL;
    Nclip = 0;
    xy2[0] = NULL;
    xy2[1] = NULL;

/*
    Pack the coordinates for the clip function.
*/
    istat = gpf_packxy (xin, yin, npts,
                        &xyin, &mflag1);
    if (istat == -1) {
        return -1;
    }

/*
    Allocate space for clipping results.
*/
    xyout = (CSW_F *)csw_Malloc (npts * 5 * sizeof(CSW_F));
    if (!xyout) {
        return -1;
    }

    xyout2 = (CSW_F *)csw_Malloc (npts * 6 * sizeof(CSW_F));
    if (!xyout2) {
        return -1;
    }

    icout = (int *)csw_Malloc (npts * 2 * sizeof(int));
    if (!icout) {
        return -1;
    }

/*
    Clip the line.
*/
    istat = calcdraw_obj.gpf_cliplineprim (xyin, npts, ClipX1, ClipY1, ClipX2, ClipY2,
                              xyout, &ncout, icout);
    if (istat == -1) {
        return -1;
    }
    if (ncout < 1) {
        return 0;
    }

/*
    We don't need xyin any more, so csw_Free it.
*/
    if (mflag1) {
        csw_Free (xyin);
        xyin = NULL;
        mflag1 = 0;
    }

    calcdraw_obj.gpf_addholeflags2 (xyout, icout, ncout,
                       xyout2, &nptsout2);
    csw_Free (xyout);
    csw_Free (icout);
    xyout = NULL;
    icout = NULL;

/*
    Transfer the flagged data from the xy format
    to separate x and y arrays.  The segments are
    separated by flags and should all be output as
    a single line primitive.
*/
    n = nptsout2;

/*
    Allocate space for the clipped line.
*/
    Xclip = (CSW_F *)csw_Calloc (n * 2 * sizeof(CSW_F));
    if (!Xclip) {
        return -1;
    }
    Yclip = Xclip + n;

    mflag2 = 0;
    istat = gpf_xyseparate (xyout2, n, xy2, &mflag2);
    memsize = n * sizeof(CSW_F);

/*
    Copy x and y data into Xclip and Yclip
*/
    csw_memcpy ((char *)Xclip, (char *)xy2[0], memsize);
    csw_memcpy ((char *)Yclip, (char *)xy2[1], memsize);
    if (mflag2) {
        csw_Free (xy2[0]);
        csw_Free (xy2[1]);
        mflag2 = 0;
    }

    Nclip = n;

    bsuccess = true;

    return 1;

}  /*  end of private ClipLine function  */

/*
    FREE_DATA is only used for the ClipLine function
    so it is undefined now.
*/
#undef FREE_DATA



/*
  ****************************************************************

                       A d d T o L e g e n d

  ****************************************************************

    Add an item to the list of legend items to be drawn later.

*/

int GPFGraph::AddToLegend (int type, int lcolor, int fcolor,
                        int patt, int pcolor, int symb, const char *text)
{
    LEgendRec   *lptr;
    int         loffset;

    if (SkipLegendFlag > 0) return 1;

    ReportedLastLegPtr = -1;
    if (LegendPosition == GTX_NO_LEGEND) {
        return 1;
    }

    loffset = 0;
    if (SkipLegendFlag < 0) {
        loffset = -SkipLegendFlag;
        loffset--;
        lptr = LegendList + loffset;
    }

    else {
        if (NfreeLegend > 0  &&  LegendList) {
            NfreeLegend--;
            lptr = LegendList + FreeLegend[NfreeLegend];
        }
        else {
            LegendList = (LEgendRec *)csw_Realloc
                                      (LegendList, (Nlegend + 1) * sizeof(LEgendRec));
            if (!LegendList) {
                return -1;
            }
            lptr = LegendList + Nlegend;
            Nlegend++;
        }
    }

    if (LegendList) {
        if (LastLegOffset >= 0) {
            LastLegPtr = LegendList + LastLegOffset;
        }
        else {
            LastLegPtr = NULL;
        }
        if (FirstLegOffset >= 0) {
            FirstLegPtr = LegendList + FirstLegOffset;
        }
        else {
            FirstLegPtr = lptr;
            FirstLegOffset = lptr - LegendList;
        }
    }
    else {
        LastLegPtr = NULL;
        FirstLegPtr = NULL;
    }

    lptr->size = CurveEbarWidth;
    lptr->thick = CurveThick;
    lptr->dashscale = CurveDashscale;
    lptr->type = type;
    lptr->linecolor = lcolor;
    lptr->fillcolor = fcolor;
    lptr->fillsize = CurveFillPatternSize;
    lptr->pattnum = patt;
    lptr->pattcolor = pcolor;
    lptr->symbol = symb;
    csw_StrTruncate (lptr->text, text, MAX_LEG_TEXT);
    csw_NewLineInsert (lptr->text);

    lptr->textsize = LegendLabelSize;
    lptr->textthick = LegendLabelThick;
    lptr->textfont = LegendLabelFont;
    lptr->textcolor = LegendLabelColor;
    lptr->textfillcolor = LegendLabelFillColor;

    if (SkipLegendFlag == 0) {
        lptr->prev = (void *)LastLegPtr;
        lptr->next = NULL;
        lptr->nextoffset = -1;
        lptr->prevoffset = -1;
        if (LastLegPtr) {
            lptr->prevoffset = LastLegPtr - LegendList;
            LastLegPtr->next = (void *)lptr;
            LastLegPtr->nextoffset = lptr - LegendList;
        }
        LastLegPtr = lptr;
        LastLegOffset = lptr - LegendList;
        ReportedLastLegPtr = LastLegOffset;
    }
    else {
        ReportedLastLegPtr = loffset;
    }

    return 1;

}  /*  end of private AddToLegend function  */



/*
  ****************************************************************

              g p f _ r o l l b a c k _ l e g e n d

  ****************************************************************

    Delete the most recent legend entry.

*/

int GPFGraph::gpf_rollback_legend (void)
{

/*
    Nlegend--;
    if (Nlegend < 0) Nlegend = 0;

    LastLegOffset--;
    if (LastLegOffset < 0) LastLegOffset = 0;
*/

    return 1;

}  /*  end of function gtx_rollback_legend  */





/*
  ****************************************************************

           g p f _ g e t _ l a s t _ l e g _ p t r

  ****************************************************************

    Return the legend pointer created by the last graph element
  calculation.  If the last element had a NULL legend text, this
  is returned as NULL.

*/

int GPFGraph::gpf_get_last_leg_ptr (int *ptr)
{

    *ptr = ReportedLastLegPtr;
    return 1;

}  /*  end of function gpf_get_last_leg_ptr  */




/*
  ****************************************************************

               g p f _ s e t _ l e g _ l i s t

  ****************************************************************

    Specify the LegendList pointer, Nlegend count, FreeLegend pointer,
  NfreeLegend count and LastLegPtr to use for legend stuff.

*/

int GPFGraph::gpf_set_leg_list (void *leglist, int nleg, int *freelist,
                      int nfree, int firstptr, int lastptr,
                      CSW_F y1, CSW_F y2, CSW_F x1, int anchor, CSW_F bwidth)
{

    LegendList = (LEgendRec *)leglist;
    Nlegend = nleg;
    FreeLegend = (int *)freelist;
    NfreeLegend = nfree;

    LastLegOffset = lastptr;
    if (lastptr >= 0  &&  LegendList) {
        LastLegPtr = LegendList + LastLegOffset;
    }
    else {
        LastLegPtr = NULL;
    }

    FirstLegOffset = firstptr;
    if (firstptr >= 0  &&  LegendList) {
        FirstLegPtr = LegendList + FirstLegOffset;
    }
    else {
        FirstLegPtr = NULL;
    }

    LegendY1 = y1;
    LegendY2 = y2;
    LegendX1 = x1;
    LegendAnchor = anchor;
    LegendBorderWidth = bwidth;

    return 1;

}  /*  end of function gpf_set_leg_list  */



/*
  ****************************************************************

               g p f _ g e t _ l e g _ l i s t

  ****************************************************************

    Retrieve the LegendList pointer, Nlegend count, FreeLegend pointer,
  NfreeLegend count and LastLegPtr being used for legend stuff.

*/

int GPFGraph::gpf_get_leg_list (void **leglist, int *nleg,
                      int **freelist, int *nfree,
                      int *firstptr, int *lastptr, CSW_F *bwidth)
{

/*printf ("In get, Nlegend = %d\n", Nlegend);*/

    if (leglist) *leglist = (void *)LegendList;
    if (nleg) *nleg = Nlegend;
    if (freelist) *freelist = (int *)FreeLegend;
    if (nfree) *nfree = NfreeLegend;
    if (lastptr) {
        *lastptr = LastLegOffset;
    }
    if (firstptr) {
        *firstptr = FirstLegOffset;
    }
    if (bwidth) *bwidth = LegendBorderWidth;

    return 1;

}  /*  end of function gpf_get_leg_list  */



/*
  ****************************************************************

              g p f _ C a l c G r a p h L e g e n d

  ****************************************************************

    Calculate graphical primitives for the legend entries that
  have been added via function AddToLegend.

*/

/*
    Define some macros used only in this function.
*/
#define LEG_VGAP         (.2f * units)
#define LEG_HGAP         (.2f * units)
#define LEG_HMARGIN      (.3f * units)
#define LEG_VMARGIN      (.2f * units)
#define LEG_THICK        (.01f * units)
#define LEG_LINE_LEN     (1.0f * units)
#define LEG_FILL_HEIGHT  (.6f * units)
#define LEG_SYMBOL_SIZE  (.1f * units)
#define LEG_SEPARATION   (.5f * units)

#define CLEANUP_LISTS    gpf_FreeGraphLines (linelist, nlines, 1); \
                         gpf_FreeGraphFills (filllist, nfills); \
                         gpf_FreeGraphText (textlist, ntexts); \
                         csw_Free (symlist);

int GPFGraph::gpf_CalcGraphLegend (const char *legendtitle, int drawtitle,
                         GRaphSymRec **syms, int *numsyms,
                         GRaphLineRec **lines, int *numlines,
                         GRaphFillRec **fills, int *numfills,
                         GRaphTextRec **texts, int *numtexts)
{
    CSW_F             width, wt, ht, units,
                      theight, twidth, maxh, titleheight,
                      columnwidth, th2, oldht, titlewidth, symsiz;
    CSW_F             x1, y1, x2, y2, yt, xt1, xt2, ytoff;
    int               ncol, maxline, maxfill, maxtext, maxsym,
                      nsyms, nlines, nfills, ntexts, ntextline, istat;
    GRaphSymRec       *symlist = NULL, *sptr = NULL;
    GRaphLineRec      *linelist = NULL, *lptr = NULL;
    GRaphFillRec      *filllist = NULL, *fptr = NULL;
    GRaphTextRec      *textlist = NULL, *tptr = NULL;
    LEgendRec         *legptr = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            CLEANUP_LISTS;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Initialize returned stuff in case of an error.
*/
    *syms = NULL;
    *numsyms = 0;
    *lines = NULL;
    *numlines = 0;
    *fills = NULL;
    *numfills = 0;
    *texts = NULL;
    *numtexts = 0;

/*
    When calling this from the plotgf stuff, no primitives
    should be calculated until all legend entries have been
    defined for a global legend.  If LegendTitleBorderFlag
    has been set to -2, then this whole function does nothing.
*/
    if (LegendTitleBorderFlag == -2) return 1;

    if (Nlegend < 1  ||  !LegendList  ||  !FirstLegPtr) {
        return 1;
    }

/*
    Initialize local variables.
*/
    maxline = 0;
    maxsym = 0;
    maxfill = 0;
    maxtext = 0;

    nlines = 0;
    nsyms = 0;
    nfills = 0;
    ntexts = 0;

    symlist = NULL;
    linelist = NULL;
    filllist = NULL;
    textlist = NULL;

    units = Units;

/*
    Figure out the size needed for the legend box.
*/
    width = 0.0f;
    ht = LEG_VMARGIN;
    maxh = 0.0f;

/*
    First, figure out the height and width of the title
    if it is to be drawn in the legend area.
*/
    titleheight = 0.0f;
    titlewidth = 0.0f;
    if (drawtitle) {
        if (GraphTitlePos == GTX_TITLE_IN_LEGEND  ||  LegendPosition == GTX_GLOBAL_LEGEND) {
            if (legendtitle) {
                gpf_HorTextBoxSize (dlist_index,
                                    legendtitle, GraphTitleFont, GraphTitleSize,
                                    GraphTitleSize / 3.0f, &titlewidth, &titleheight,
                                    &ntextline, PageUnitsPerInch);
                width = titlewidth;
                ht += titleheight;
                ht += GraphTitleGap;
                maxtext += ntextline;
            }
        }
    }

/*
    Figure out height of legend assuming one column.
*/
    symsiz = 0.0f;
    legptr = FirstLegPtr;
    while (legptr) {

        if (legptr->text[0] == '\0') {
            if (legptr->nextoffset >= 0) {
                legptr = LegendList + legptr->nextoffset;
            }
            else {
                legptr = NULL;
            }
            continue;
        }

        gpf_HorTextBoxSize
                    (dlist_index,
                     legptr->text, legptr->textfont, legptr->textsize,
                     legptr->textsize / 3.0f, &twidth, &theight, &ntextline,
                     PageUnitsPerInch);
        theight += legptr->textthick * 2.0f;
        ht += LEG_VGAP;
        oldht = ht;
        maxtext += ntextline;
        legptr->nlines = ntextline;

        switch (legptr->type) {

            case LEGEND_POINT:
                maxsym--;
                maxline--;
                symsiz = legptr->size;
                maxsym += 2;
                wt = LEG_LINE_LEN + LEG_HGAP + twidth;
                if (wt > width) width = wt;
                if (symsiz > theight) theight = symsiz;
                ht += theight;
                if (theight > maxh) maxh = theight;
                maxline++;
                break;

            case LEGEND_SYMB:
                maxsym += 2;
                wt = LEG_LINE_LEN + LEG_HGAP + twidth;
                if (wt > width) width = wt;
                if (symsiz > theight) theight = symsiz;
                ht += theight;
                if (theight > maxh) maxh = theight;
                maxline++;
                break;

            case LEGEND_LINE:
                wt = LEG_LINE_LEN + LEG_HGAP + twidth;
                if (wt > width) width = wt;
                if (symsiz > theight) theight = symsiz;
                if (legptr->thick > theight) theight = legptr->thick;
                ht += theight;
                if (theight > maxh) maxh = theight;
                maxline++;
                break;

            case LEGEND_FILL:
                wt = LEG_LINE_LEN + LEG_HGAP + twidth;
                if (wt > width) width = wt;
                if (theight > LEG_FILL_HEIGHT) {
                    ht += theight;
                    if (theight > maxh) maxh = theight;
                }
                else {
                    ht += LEG_FILL_HEIGHT;
                    if (LEG_FILL_HEIGHT > maxh) maxh = LEG_FILL_HEIGHT;
                }
                maxfill++;
                break;

            default:
                break;

        }

        legptr->height = ht - oldht;

        if (legptr->nextoffset >= 0) {
            legptr = LegendList + legptr->nextoffset;
        }
        else {
            legptr = NULL;
        }

    }

/*
    Adjust for the borders.
*/
    columnwidth = width + LEG_HGAP;
    width = width + 2.0f * LEG_HMARGIN;
    ht = ht + 2.0f * LEG_VMARGIN;

/*
    Make sure height is sufficient for at least
    the title and the highest legend entry.
*/
    y1 = PageY1;
    y2 = PageY2;
    if (LegendPosition == GTX_GLOBAL_LEGEND) {
        y1 = LegendY1;
        y2 = LegendY2;
    }
    maxh += LEG_VMARGIN * 2.0f + LEG_VGAP + titleheight;
    if (y2 - y1 < maxh) {
        y1 = (y2 + y1) /2.0f;
        y2 = y1;
        y2 += maxh / 2.0f;
        y1 -= maxh / 2.0f;
    }

/*
    Split into multiple columns if needed.
*/
    theight = y2 - y1 - LEG_VMARGIN;
    if (ht > theight) {
        ncol = 1;
        th2 = titleheight + LEG_VMARGIN;
        legptr = FirstLegPtr;
        while (legptr) {
            th2 += legptr->height;
            th2 += LEG_VGAP;
            if (th2 > theight) {
                th2 = titleheight + LEG_VMARGIN;
                ncol++;
            }

            if (legptr->nextoffset >= 0) {
                legptr = LegendList + legptr->nextoffset;
            }
            else {
                legptr = NULL;
            }

        }
        width = columnwidth * ncol + 2.0f * LEG_HMARGIN;
    }

/*
    Calculate coordinates of legend box corners.
*/
    if (LegendPosition == GTX_GLOBAL_LEGEND) {
        y1 = LegendY1;
        y2 = LegendY2;
        if (width < LegendBorderWidth) {
            width = LegendBorderWidth;
        }
        if (width > LegendBorderWidth) {
            LegendBorderWidth = width;
        }
        if (LegendAnchor == GTX_ANCHOR_LEFT_EDGE) {
            x1 = LegendX1;
            x2 = x1 + width;
        }
        else {
            x2 = LegendX1;
            x1 = x2 - width;
        }
    }
    else {
        if (width > LegendBorderWidth) {
            LegendBorderWidth = width;
        }
        if (LegendPosition == GTX_LEGEND_ON_LEFT) {
            if (AutoLegendAnchor) {
                x1 = AutoLegendX1;
            }
            else {
                x1 = GXmin - LEG_SEPARATION - width;
                AutoLegendX1 = x1;
                AutoLegendAnchor = 1;
                GXmin -= width;
                GXmin -= LEG_SEPARATION;
            }
        }
        else {
            if (AutoLegendAnchor) {
                x1 = AutoLegendX1;
            }
            else {
                x1 = GXmax + LEG_SEPARATION;
                AutoLegendX1 = x1;
                AutoLegendAnchor = 1;
                GXmax += LEG_SEPARATION;
                GXmax += width;
            }
        }
        x2 = x1 + width;
    }

    if (LegendBorderColor != 0  &&  LegendBorderThick > 0.001) {
        maxline++;
    }

/*
    Allocate space for the graphic primitive records.
*/
    if (maxfill > 0) {
        maxfill += 10;
        filllist = (GRaphFillRec *)csw_Calloc (maxfill * sizeof(GRaphFillRec));
        if (!filllist) {
            return -1;
        }
    }

    if (maxline > 0) {
        maxline += 10;
        linelist = (GRaphLineRec *)csw_Calloc (maxline * sizeof(GRaphLineRec));
        if (!linelist) {
            return -1;
        }
    }

    if (maxsym > 0) {
        maxsym += 10;
        symlist = (GRaphSymRec *)csw_Calloc (maxsym * sizeof(GRaphSymRec));
        if (!symlist) {
            return -1;
        }
    }

    if (maxtext > 0) {
        maxtext += 10;
        textlist = (GRaphTextRec *)csw_Calloc (maxtext * sizeof(GRaphTextRec));
        if (!textlist) {
            return -1;
        }
    }

    sptr = symlist;
    lptr = linelist;
    tptr = textlist;
    fptr = filllist;

/*
    Calculate text primitives for the title.
*/
    if (drawtitle) {
        if (GraphTitlePos == GTX_TITLE_IN_LEGEND  ||  LegendPosition == GTX_GLOBAL_LEGEND) {
            if (legendtitle  &&  LegendTitleBorderFlag != -1) {
                yt = y2 - LEG_VMARGIN;
                twidth = (x2 - x1 - LEG_HMARGIN * 2.0f - titlewidth) / 2.0f;
                istat = MultiLineTextPrims
                        (&tptr, legendtitle, &ntexts, 1,
                         x1 + LEG_HMARGIN + twidth, yt - GraphTitleSize,
                         GraphTitleSize, GraphTitleSize / 3.0f,
                         GraphTitleThick, 0.0f,
                         GraphTitleColor, GraphTitleFillColor,
                         GraphTitleFont);
                if (istat == -1) {
                    return -1;
                }
            }
        }
    }

/*
    If the legend title and border flag is set to title and border only
    (value of 1), skip the other primitives in the legend by going to
    the DRAW_BORDER label.
*/
    if (LegendTitleBorderFlag == 1) {
        goto DRAW_BORDER;
    }

/*
    Calculate graphical primitives for the legend entries.
*/
    ncol = 0;
    yt = y2 - titleheight - GraphTitleGap - LEG_VMARGIN;

    legptr = FirstLegPtr;
    while (legptr) {

        if (legptr->text[0] == '\0') {
            if (legptr->nextoffset >= 0) {
                legptr = LegendList + legptr->nextoffset;
            }
            else {
                legptr = NULL;
            }
            continue;
        }

        switch (legptr->type) {

        /*
            Line entry in the legend.
        */
            case LEGEND_LINE:

                gpf_HorTextBoxSize
                    (dlist_index,
                     legptr->text, legptr->textfont, legptr->textsize,
                     legptr->textsize / 3.0f, &twidth, &theight, &ntextline,
                     PageUnitsPerInch);
                if (legptr->thick > theight) theight = legptr->thick;
                theight += legptr->textthick * 2.0f;
                if (yt - theight < y1 + LEG_VMARGIN) {
                    yt = y2 - titleheight - GraphTitleGap - LEG_VMARGIN;
                    ncol++;
                }

                xt1 = x1 + LEG_HMARGIN + ncol * columnwidth;
                xt2 = xt1 + LEG_LINE_LEN;
                if (lptr) {
                    lptr->x = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
                    if (!lptr->x) {
                        return -1;
                    }
                    lptr->y = lptr->x + 2;
                    lptr->x[0] = xt1;
                    lptr->y[0] = yt - theight / 2.0f;
                    lptr->x[1] = xt2;
                    lptr->y[1] = yt - theight / 2.0f;
                    lptr->smoothflag = 0;
                    lptr->linepat = legptr->pattnum;
                    lptr->linecolor = legptr->linecolor;
                    lptr->dashscale = legptr->dashscale;
                    lptr->linethick = legptr->thick;
                    lptr->npts = 2;
                    lptr++;
                    nlines++;
                }

                istat = MultiLineTextPrims
                        (&tptr, legptr->text, &ntexts, 0,
                         xt2 + LEG_HGAP, yt - legptr->textsize,
                         legptr->textsize, legptr->textsize / 3.0f,
                         legptr->textthick, 0.0f,
                         legptr->textcolor, legptr->textfillcolor,
                         legptr->textfont);
                if (istat == -1) {
                    return -1;
                }

                yt -= theight;
                yt -= LEG_VGAP;

                break;

            /*
                Symbolized line in the legend.
            */
            case LEGEND_SYMB:

                gpf_HorTextBoxSize
                    (dlist_index,
                     legptr->text, legptr->textfont, legptr->textsize,
                     legptr->textsize / 3.0f, &twidth, &theight, &ntextline,
                     PageUnitsPerInch);
                theight += legptr->textthick * 2.0f;
                if (yt - theight < y1 + LEG_VMARGIN) {
                    yt = y2 - titleheight - LEG_VGAP - LEG_VMARGIN;
                    ncol++;
                }

                xt1 = x1 + LEG_HMARGIN + ncol * columnwidth;
                xt2 = xt1 + LEG_LINE_LEN;
                if (lptr) {
                    lptr->x = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
                    if (!lptr->x) {
                        return -1;
                    }
                    lptr->y = lptr->x + 2;
                    lptr->x[0] = xt1;
                    lptr->y[0] = yt - theight / 2.0f;
                    lptr->x[1] = xt2;
                    lptr->y[1] = yt - theight / 2.0f;
                    lptr->smoothflag = 0;
                    lptr->linepat = legptr->pattnum;
                    lptr->dashscale = legptr->dashscale;
                    lptr->linecolor = legptr->linecolor;
                    lptr->linethick = legptr->thick;
                    lptr->npts = 2;
                    lptr++;
                    nlines++;
                }

                if (sptr) {
                    sptr->symnum = legptr->symbol;
                    sptr->mask = 1;
                    sptr->color = legptr->fillcolor;
                    sptr->x = xt1;
                    sptr->y = yt - theight / 2.0f;
                    sptr->thick = LEG_THICK;
                    sptr->size = LEG_SYMBOL_SIZE;
                    sptr++;
                    nsyms++;

                    sptr->symnum = legptr->symbol;
                    sptr->mask = 1;
                    sptr->color = legptr->fillcolor;
                    sptr->x = xt2;
                    sptr->y = yt - theight / 2.0f;
                    sptr->thick = LEG_THICK;
                    sptr->size = LEG_SYMBOL_SIZE;
                    sptr++;
                    nsyms++;
                }

                istat = MultiLineTextPrims
                        (&tptr, legptr->text, &ntexts, 0,
                         xt2 + LEG_HGAP, yt - legptr->textsize,
                         legptr->textsize, legptr->textsize / 3.0f,
                         legptr->textthick, 0.0f,
                         legptr->textcolor, legptr->textfillcolor,
                         legptr->textfont);
                if (istat == -1) {
                    return -1;
                }

                yt -= theight;
                yt -= LEG_VGAP;

                break;

            /*
                Symbolized point in the legend.
            */
            case LEGEND_POINT:

                gpf_HorTextBoxSize
                    (dlist_index,
                     legptr->text, legptr->textfont, legptr->textsize,
                     legptr->textsize / 3.0f, &twidth, &theight, &ntextline,
                     PageUnitsPerInch);
                theight += legptr->textthick * 2.0f;
                if (yt - theight < y1 + LEG_VMARGIN) {
                    yt = y2 - titleheight - LEG_VGAP - LEG_VMARGIN;
                    ncol++;
                }

                xt1 = x1 + LEG_HMARGIN + ncol * columnwidth;
                xt2 = xt1 + LEG_LINE_LEN;

                if (sptr) {
                    sptr->symnum = legptr->symbol;
                    sptr->mask = 1;
                    sptr->color = legptr->fillcolor;
                    sptr->x = (xt1 + xt2 ) / 2.0f;
                    sptr->thick = LEG_THICK;
                    sptr->size = legptr->size;
                    symsiz = sptr->size;
                    sptr->y = yt - symsiz / 2.0f;
                    if (theight > symsiz) sptr->y = yt - theight / 2.0f;
                    sptr++;
                    nsyms++;
                }

                if (theight > symsiz) {
                    ytoff = legptr->textsize;
                }
                else {
                    ytoff = (symsiz - theight) / 2.0f + legptr->textsize;
                }

                istat = MultiLineTextPrims
                        (&tptr, legptr->text, &ntexts, 0,
                         xt2 + LEG_HGAP, yt - ytoff,
                         legptr->textsize, legptr->textsize / 3.0f,
                         legptr->textthick, 0.0f,
                         legptr->textcolor, legptr->textfillcolor,
                         legptr->textfont);
                if (istat == -1) {
                    return -1;
                }

                if (symsiz > theight) theight = symsiz;

                yt -= theight;
                yt -= LEG_VGAP;

                break;

            /*
                Filled pattern legend entry.
            */
            case LEGEND_FILL:

                gpf_HorTextBoxSize
                    (dlist_index,
                     legptr->text, legptr->textfont, legptr->textsize,
                     legptr->textsize / 3.0f, &twidth, &theight, &ntextline,
                     PageUnitsPerInch);
                theight += legptr->textthick * 2.0f;
                if (yt - theight < y1 + LEG_VMARGIN) {
                    yt = y2 - titleheight - LEG_VGAP - LEG_VMARGIN;
                    ncol++;
                }

                xt1 = x1 + LEG_HMARGIN;
                xt2 = xt1 + LEG_LINE_LEN;
                if (fptr) {
                    fptr->x = (CSW_F *)csw_Malloc (10 * sizeof(CSW_F));
                    if (!fptr->x) {
                        return -1;
                    }
                    fptr->y = fptr->x + 5;
                    fptr->x[0] = xt1;
                    fptr->y[0] = yt;
                    fptr->x[1] = xt2;
                    fptr->y[1] = yt;
                    fptr->x[2] = xt2;
                    fptr->y[2] = yt - LEG_FILL_HEIGHT;
                    fptr->x[3] = xt1;
                    fptr->y[3] = yt - LEG_FILL_HEIGHT;
                    fptr->x[4] = xt1;
                    fptr->y[4] = yt;
                    fptr->smoothflag = 0;
                    fptr->fillpat = legptr->pattnum + 2000;
                    fptr->fillcolor = legptr->fillcolor;
                    fptr->pattcolor = legptr->pattcolor;
                    fptr->patsize = legptr->fillsize;
                    fptr->linecolor = legptr->linecolor;
                    fptr->npts = 5;
                    fptr->linethick = LEG_THICK;
                    fptr->outline = 1;
                    fptr++;
                    nfills++;
                }

                if (theight > LEG_FILL_HEIGHT) {
                    ytoff = legptr->textsize;
                }
                else {
                    ytoff = (LEG_FILL_HEIGHT - theight + legptr->textsize) / 2.0f;
                }
                istat = MultiLineTextPrims
                        (&tptr, legptr->text, &ntexts, 0,
                         xt2 + LEG_HGAP, yt - ytoff,
                         legptr->textsize, legptr->textsize / 3.0f,
                         legptr->textthick, 0.0f,
                         legptr->textcolor, legptr->textfillcolor,
                         legptr->textfont);
                if (istat == -1) {
                    return -1;
                }

                if (theight > LEG_FILL_HEIGHT) {
                    yt -= theight;
                }
                else {
                    yt -= LEG_FILL_HEIGHT;
                }
                yt -= LEG_VGAP;

                break;

            default:
		break;

        }

        if (legptr->nextoffset >= 0) {
            legptr = LegendList + legptr->nextoffset;
        }
        else {
            legptr = NULL;
        }

    }  /*  end of primitive calculation loop  */

/*
    This label is used as a goto destination when only title and border
    primitives are desired (LegendTitleBorderFlag = 1)
*/
  DRAW_BORDER:

/*
    Add a line primitive for the border outline if needed.
*/
    if (LegendBorderColor != 0  &&
        LegendBorderThick > 0.001  &&  LegendTitleBorderFlag != -1) {

        if (lptr) {
            lptr->x = (CSW_F *)csw_Malloc (10 * sizeof(CSW_F));
            if (!lptr->x) {
                return -1;
            }
            lptr->y = lptr->x + 5;
            lptr->x[0] = x1;
            lptr->y[0] = y1;
            lptr->x[1] = x2;
            lptr->y[1] = y1;
            lptr->x[2] = x2;
            lptr->y[2] = y2;
            lptr->x[3] = x1;
            lptr->y[3] = y2;
            lptr->x[4] = x1;
            lptr->y[4] = y1;
            lptr->smoothflag = 0;
            lptr->linepat = 0;
            lptr->linecolor = LegendBorderColor;
            lptr->linethick = LegendBorderThick;
            lptr->npts = 5;
            lptr++;
            nlines++;
        }
    }

/*
    Return the pointers to the primitive lists
    and the number of primitives in each list.
*/
    *fills = filllist;
    *numfills = nfills;
    *texts = textlist;
    *numtexts = ntexts;
    *syms = symlist;
    *numsyms = nsyms;
    *lines = linelist;
    *numlines = nlines;

    bsuccess = true;

    return 1;

}  /*  end of function gpf_CalcGraphLegend  */

/*
    Undefine the macros used only in this function.
*/
#undef LEG_VGAP
#undef LEG_HGAP
#undef LEG_HMARGIN
#undef LEG_VMARGIN
#undef LEG_THICK
#undef LEG_LINE_LEN
#undef LEG_FILL_HEIGHT
#undef LEG_SEPARATION
#undef LEG_SYMBOL_SIZE

#undef CLEANUP_LISTS





/*
  ****************************************************************

               M u l t i L i n e T e x t P r i m s

  ****************************************************************

  Calculate text primitives for legend text.  There can be
  multiple legend text lines per entry, so multiple text structures
  can be produced here.  The tptrin parameter specifies the starting
  text pointer and returns the next text pointer to use after the
  function.

*/

int GPFGraph::MultiLineTextPrims (GRaphTextRec **tptrin, const char *text,
                               int *ntextin, int centerflag,
                               CSW_F x, CSW_F y, CSW_F size,
                               CSW_F gap, CSW_F thick, CSW_F angle,
                               int color, int fillcolor, int font)
{
    int               nc, i, nctot, nlines, maxlines;
    GRaphTextRec      *tptr = NULL;
    char              *ctmp = NULL, **clines = NULL;
    CSW_F             twidth, theight, tlen;


    auto fscope = [&]()
    {
        csw_Free (ctmp);
        csw_Free (clines);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Get stuff from the appropriate LegendList entry.
*/
    tptr = *tptrin;
    nctot = strlen (text);
    maxlines = 1;
    for (i=0; i<nctot; i++) {
        if (text[i] == '\n') {
            maxlines++;
        }
    }

/*
    Allocate buffers for separating text lines.
*/
    ctmp = (char *)csw_Malloc ((nctot + 1) * sizeof(char));
    if (!ctmp) {
        return -1;
    }

    clines = (char **)csw_Malloc (maxlines * sizeof(char *));
    if (!clines) {
        return -1;
    }

/*
    Calculate width of text for centering.
*/
    gpf_HorTextBoxSize (dlist_index,
                        text, font, size, gap,
                        &twidth, &theight, &nlines,
                        PageUnitsPerInch);
    if (centerflag == 2) x -= twidth / 2.0f;

/*
    Copy text into work buffer and separate lines.
*/
    strcpy (ctmp, text);
    csw_SeparateLines (ctmp, clines, &nlines, maxlines);

/*
    Calculate a text primitive for each line of text.
*/
    for (i=0; i<nlines; i++) {
        nc = strlen (clines[i]);
        tptr->text = (char *)csw_Malloc ((nc + 1) * sizeof(char));
        if (!tptr->text) {
            return -1;
        }
        strcpy (tptr->text, clines[i]);
        tptr->nchar = nc;
        tptr->x = x;
        if (centerflag == 1) {
            tlen = exact_text_length (clines[i], size, font);
            tptr->x = x + (twidth - tlen) / 2.0f;
        }
        tptr->y = y - i * (gap + size);
        tptr->size = size;
        tptr->thick = thick;
        tptr->angle = angle;
        tptr->fillcolor = fillcolor;
        tptr->color = color;
        tptr->font = font;
        tptr++;
    }

/*
    Cleanup work space and return.
*/
    *ntextin += nlines;
    *tptrin = tptr;

    return 1;

}  /*  end of private MultiLineTextPrims function  */




/*
  ****************************************************************

            g p f _ C a l c G r a p h C u r v e F i l l

  ****************************************************************

    Calculate polygon primitives for an inter curve fill.  The top
  and bottom curve points are specified along with the axis limits.
  The primitives are returned in the fills array of structures.
  The calling function should csw_Free the fills array when done with the
  polygon primitives.

*/

/*
    The CLEANUP_MEM macro is only defined for this function.
*/
#define CLEANUP_MEM   csw_Free (xtop); \
                      csw_Free (xbottom); \
                      csw_Free (xwork); \
                      csw_Free (xwork2); \
                      csw_Free (itop); \
                      csw_Free (ibottom); \
                      csw_Free (iwork2);

int GPFGraph::gpf_CalcGraphCurveFill (CSWErrNum &err_obj,
                            const char *legtext, int vflag,
                            CSW_F *xtopin, CSW_F *ytopin, int ntopin,
                            CSW_F *xbottomin, CSW_F *ybottomin, int nbottomin,
                            CSW_F axmin, CSW_F axmax, CSW_F xlog,
                            CSW_F aymin, CSW_F aymax, CSW_F ylog,
                            GRaphFillRec **fills, int *nfills)
{
    int              logflagx, logflagy;
    int              ntop, nbottom, nwork, nctop, ncbottom,
                     istat, i, j, n, memflag, nprimpts, npmax, ncmax,
                     *itop = NULL, *ibottom = NULL;
    int              nwork2, maxcomp, maxholes, maxpts,
                     *iwork2 = NULL, *iwork3 = NULL;
    CSW_F            lnbasex, lnbasey, scalex, scaley;
    CSW_F            *xtop = NULL, *ytop = NULL, *xbottom = NULL,
                     *ybottom = NULL, *xwork = NULL, *ywork = NULL,
                     *xprim = NULL, *yprim = NULL, *xwork2 = NULL,
                     *ywork2 = NULL,
                     x1, y1, x2, y2, yfudge, xt, yt;
    CSW_F            clipx1, clipy1, clipx2, clipy2;
    GRaphFillRec     *fillout = NULL;
    int              topinside, bottominside;

    CSWPolyCalc      ply_calc_obj;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        CLEANUP_MEM;
        if (bsuccess == false) {
            gpf_FreeGraphFills (fillout, *nfills);
            csw_Free (fillout);
            csw_Free (xprim);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    *nfills = 0;

    istat = DrawLegendOnly (LEGEND_FILL, 1, CurveFillColor,
                            CurveFillPattern, CurvePatternColor, CurveSymbol, legtext);
    if (istat == 1) {
        return 1;
    }

/*
    Initialize local variables.
*/
    xtop = NULL;
    xbottom = NULL;
    xwork = NULL;
    xwork2 = NULL;
    itop = NULL;
    ibottom = NULL;
    iwork2 = NULL;

    ntop = 0;
    nbottom = 0;
    lnbasex = 1.0;
    lnbasey = 1.0;
    ncbottom = 1;
    nctop = 1;

/*
    Initialize the fills pointer to NULL in case a warning occurs.
*/
    *fills = NULL;
    *nfills = 0;

    if (xbottomin == NULL  &&  xtopin == NULL) {
        return 1;
    }

/*
    Check for logarithmic axes.
*/
    logflagx = 0;
    logflagy = 0;
    if (xlog > MIN_LOG_BASE) {
        lnbasex = (CSW_F)log ((double)xlog);
        axmin = (CSW_F)log ((double)axmin) / lnbasex;
        axmax = (CSW_F)log ((double)axmax) / lnbasex;
        logflagx = 1;
    }

    if (ylog > MIN_LOG_BASE) {
        lnbasey = (CSW_F)log ((double)ylog);
        aymin = (CSW_F)log ((double)aymin) / lnbasey;
        aymax = (CSW_F)log ((double)aymax) / lnbasey;
        logflagy = 1;
    }

    yfudge = (aymax - aymin) / 100.f;

    clipx1 = axmin;
    clipy1 = aymin;
    clipx2 = axmax;
    clipy2 = aymax;
    if (clipx1 > clipx2) {
        xt = clipx1;
        clipx1 = clipx2;
        clipx2 = xt;
    }
    if (clipy1 > clipy2) {
        xt = clipy1;
        clipy1 = clipy2;
        clipy2 = xt;
    }

/*
    Square off the top curve and clip it to the axes.
*/
    xtop = NULL;
    ytop = NULL;
    topinside = 0;
    bottominside = 0;

    if (xtopin) {

    /*
        Allocate space for squared off polygon.
    */
        nwork = ntopin + 3;
        xwork = (CSW_F *)csw_Malloc (nwork * 2 * sizeof(CSW_F));
        if (!xwork) {
            return -1;
        }
        ywork = xwork + nwork;

    /*
        Use the minimum y of the curve or the bottom of
        the y axis for squaring off, whichever is less.
    */
        gpf_xandylimits (xtopin, ytopin, ntopin,
                         &x1, &y1, &x2, &y2);
        if (y1 > clipy1) {
            y1 = clipy1;
        }
        if (x1 >= clipx1  &&  x2 <= clipx2  &&  y1 >= clipy1  &&  y2 <= clipy2) {
            topinside = 1;
        }
        else {
            topinside = 0;
            y1 -= yfudge;
        }

    /*
        Construct the squared off polygon by copying the curve
        points and then appending points to close the curve.
    */
        memcpy ((char *)xwork, (char *)xtopin, ntopin * sizeof(CSW_F));
        memcpy ((char *)ywork, (char *)ytopin, ntopin * sizeof(CSW_F));
        xwork[ntopin] = xtopin[ntopin-1];
        ywork[ntopin] = y1;
        xwork[ntopin+1] = xtopin[0];
        ywork[ntopin+1] = y1;
        xwork[ntopin+2] = xtopin[0];
        ywork[ntopin+2] = ytopin[0];

    /*
        Convert to log values if needed.
    */
        if (logflagx) {
            for (i=0; i<ntopin; i++) {
                xwork[i] = (CSW_F)log ((double)xwork[i]) / lnbasex;
            }
        }
        if (logflagy) {
            for (i=0; i<ntopin; i++) {
                ywork[i] = (CSW_F)log ((double)ywork[i]) / lnbasey;
            }
        }

    /*
        Allocate space for the results of clipping
        this squared off curve.
    */
        npmax = nwork * 4;
        xtop = (CSW_F *)csw_Malloc (npmax * 2 * sizeof(CSW_F));
        if (!xtop) {
            return -1;
        }
        ytop = xtop + npmax;

        ncmax = nwork;
        itop = (int *)csw_Malloc (ncmax * sizeof(int));
        if (!itop) {
            return -1;
        }

    /*
        Clip the squared curve to the axes.
    */
        if (topinside) {
            memcpy ((char *)xtop, (char *)xwork, (ntopin + 3) * sizeof(CSW_F));
            memcpy ((char *)ytop, (char *)ywork, (ntopin + 3) * sizeof(CSW_F));
            itop[0] = ntopin + 3;
            nctop = 1;
        }
        else {
            istat = ply_calc_obj.ply_ClipToBox1 (err_obj,
                                    xwork, ywork, &nwork, 1,
                                    clipx1, clipy1, clipx2, clipy2,
                                    xtop, ytop, itop, &nctop,
                                    ncmax, npmax);

            if (istat <= 0) {
                if (err_obj.csw_ErrNum() == PLY_BADMALLOC) {
                    return -1;
                }
                else {
                    *fills = NULL;
                    *nfills = 0;
                    return 1;
                }
            }
        }

        csw_Free (xwork);
        xwork = NULL;

        ntop = 0;
        for (i=0; i<nctop; i++) {
            ntop += itop[i];
        }

    }  /*  end of block for squaring the top curve  */

/*
    Square off the bottom curve and clip it to the axes.
*/
    xbottom = NULL;
    ybottom = NULL;

    if (xbottomin) {

    /*
        Allocate space for squared off polygon.
    */
        nwork = nbottomin + 3;
        xwork = (CSW_F *)csw_Malloc (nwork * 2 * sizeof(CSW_F));
        if (!xwork) {
            return -1;
        }
        ywork = xwork + nwork;

    /*
        Use the maximum y of the curve or the top of
        the y axis for squaring off, whichever is greater.
    */
        gpf_xandylimits (xbottomin, ybottomin, nbottomin,
                         &x1, &y1, &x2, &y2);
        if (y2 < clipy2) {
            y2 = clipy2;
        }
        if (x1 >= clipx1  &&  x2 <= clipx2  &&  y1 >= clipy1  &&  y2 <= clipy2) {
            bottominside = 1;
        }
        else {
            bottominside = 0;
            y2 += yfudge;
        }

    /*
        Construct the squared off polygon by copying the curve
        points and then appending points to close the curve.
    */
        memcpy ((char *)xwork, (char *)xbottomin, nbottomin * sizeof(CSW_F));
        memcpy ((char *)ywork, (char *)ybottomin, nbottomin * sizeof(CSW_F));
        xwork[nbottomin] = xbottomin[nbottomin-1];
        ywork[nbottomin] = y2;
        xwork[nbottomin+1] = xbottomin[0];
        ywork[nbottomin+1] = y2;
        xwork[nbottomin+2] = xbottomin[0];
        ywork[nbottomin+2] = ybottomin[0];

    /*
        Convert to log values if needed.
    */
        if (logflagx) {
            for (i=0; i<nbottomin; i++) {
                xwork[i] = (CSW_F)log ((double)xwork[i]) / lnbasex;
            }
        }
        if (logflagy) {
            for (i=0; i<nbottomin; i++) {
                ywork[i] = (CSW_F)log ((double)ywork[i]) / lnbasey;
            }
        }

    /*
        Allocate space for the results of clipping
        this squared off curve.
    */
        npmax = nwork * 4;
        xbottom = (CSW_F *)csw_Malloc (npmax * 2 * sizeof(CSW_F));
        if (!xbottom) {
            return -1;
        }
        ybottom = xbottom + npmax;

        ncmax = nwork;
        ibottom = (int *)csw_Malloc (ncmax * sizeof(int));
        if (!ibottom) {
            return -1;
        }

    /*
        Clip the squared curve to the axes.
    */
        if (bottominside) {
            memcpy ((char *)xbottom, (char *)xwork, (nbottomin + 3) * sizeof(CSW_F));
            memcpy ((char *)ybottom, (char *)ywork, (nbottomin + 3) * sizeof(CSW_F));
            ibottom[0] = nbottomin + 3;
            ncbottom = 1;
        }
        else {
            istat = ply_calc_obj.ply_ClipToBox1 (err_obj,
                                    xwork, ywork, &nwork, 1,
                                    clipx1, clipy1, clipx2, clipy2,
                                    xbottom, ybottom, ibottom, &ncbottom,
                                    ncmax, npmax);

            if (istat <= 0) {
                if (err_obj.csw_ErrNum() == PLY_BADMALLOC) {
                    return -1;
                }
                else {
                    *fills = NULL;
                    *nfills = 0;
                    return 1;
                }
            }
            if (istat == -1) {
                return -1;
            }
        }

        csw_Free (xwork);
        xwork = NULL;

        nbottom = 0;
        for (i=0; i<ncbottom; i++) {
            nbottom += ibottom[i];
        }

    }  /*  end of block for squaring the bottom curve  */

/*
    Calculate x and y scaling factors to convert
    from graph to plot coordinates.
*/
    if (vflag) {
        scalex = (axmax - axmin) / (PageY2 - PageY1);
        scaley = (aymax - aymin) / (PageX2 - PageX1);
    }
    else {
        scalex = (axmax - axmin) / (PageX2 - PageX1);
        scaley = (aymax - aymin) / (PageY2 - PageY1);
    }

/*
    If the bottom curve is not specified, use the clipped
    top curve as the fill primitive.  All of the checks for NULL
    pointers in itop, xtop and ytop are here because of bullshit
    warnings output by the microsoft visual C compiler.
*/
    if (xtopin  &&  !xbottom  &&
        itop != NULL  &&
        xtop != NULL  &&
        ytop != NULL) {

    /*
        Allocate space for a single fill prim.
    */
        fillout = (GRaphFillRec *)csw_Malloc (sizeof(GRaphFillRec));
        if (!fillout) {
            return -1;
        }

    /*
        Count the points in the top polygon and
        scale xtop and ytop to plot coordinates.
    */
        n = 0;
        for (i=0; i<nctop; i++) {
            for (j=0; j<itop[i]; j++) {
                if (vflag) {
                    yt = ytop[n];
                    ytop[n] = (xtop[n] - axmin) / scalex + PageY1;
                    xtop[n] = (yt - aymin) / scaley + PageX1;
                }
                else {
                    xtop[n] = (xtop[n] - axmin) / scalex + PageX1;
                    ytop[n] = (ytop[n] - aymin) / scaley + PageY1;
                }
                n++;
            }
        }

    /*
        Allocate space for primitive points.  The primitive has
        embedded hole flags, so allocate enough space for the
        points and the hole flags.
    */
        n += nctop * 2;
        n += 2;
        xprim = (CSW_F *)csw_Malloc (n * 2 * sizeof(CSW_F));
        if (!xprim) {
            return -1;
        }
        yprim = xprim + n;

    /*
        Convert points into nested hole flag format.
    */
        istat = calcdraw_obj.gpf_addholeflags (xtop, ytop, itop, nctop,
                                  &xwork, &ywork, &nprimpts, &memflag);
        if (istat == -1) {
            return -1;
        }

    /*
        Copy into the primitive point arrays.
    */
        memcpy ((char *)xprim, (char *)xwork, nprimpts * sizeof(CSW_F));
        memcpy ((char *)yprim, (char *)ywork, nprimpts * sizeof(CSW_F));

        if (memflag) {
            csw_Free (xwork);
            csw_Free (ywork);
            xwork = NULL;
            ywork = NULL;
        }

    /*
        Fill in the fill primitive structure and return.
    */
        fillout->smoothflag = 0;
        fillout->fillpat = CurveFillPattern + 2000;
        fillout->fillcolor = CurveFillColor;
        fillout->pattcolor = CurvePatternColor;
        fillout->x = xprim;
        fillout->y = yprim;
        fillout->linethick = 0.001f;
        fillout->npts = nprimpts;
        fillout->outline = 0;

        *fills = fillout;
        fillout = NULL;
        *nfills = 1;

        istat = AddToLegend (LEGEND_FILL, 1, CurveFillColor,
                             CurveFillPattern, CurvePatternColor, CurveSymbol, legtext);
        if (!istat) {
            return -1;
        }

        return 1;

    }  /*  end of block to output top curve only  */

/*
    If the top curve is not specified, use the clipped
    bottom curve as the fill primitive.  All the NULL
    pointer checks are here because of bullshit warnings
    produced by the microsoft visual C compiler.
*/
    if (xbottomin  &&  !xtop  &&
        ibottom != NULL  &&
        xbottom != NULL  &&
        ybottom != NULL) {

    /*
        Allocate space for a single fill prim.
    */
        fillout = (GRaphFillRec *)csw_Malloc (sizeof(GRaphFillRec));
        if (!fillout) {
            return -1;
        }

    /*
        Count the points in the bottom polygon and
        scale xbottom and ybottom to plot coordinates.
    */
        n = 0;
        for (i=0; i<ncbottom; i++) {
            for (j=0; j<ibottom[i]; j++) {
                if (vflag) {
                    yt = ybottom[n];
                    ybottom[n] = (xbottom[n] - axmin) / scalex + PageY1;
                    xbottom[n] = (yt - aymin) / scaley + PageX1;
                }
                else {
                    xbottom[n] = (xbottom[n] - axmin) / scalex + PageX1;
                    ybottom[n] = (ybottom[n] - aymin) / scaley + PageY1;
                }
                n++;
            }
        }

    /*
        Allocate space for primitive points.  The primitive has
        embedded hole flags, so allocate enough space for the
        points and the hole flags.
    */
        n += ncbottom * 2;
        n += 2;
        xprim = (CSW_F *)csw_Malloc (n * 2 * sizeof(CSW_F));
        if (!xprim) {
            return -1;
        }
        yprim = xprim + n;

    /*
        Convert points into nested hole flag format.
    */
        istat = calcdraw_obj.gpf_addholeflags (xbottom, ybottom, ibottom, ncbottom,
                                  &xwork, &ywork, &nprimpts, &memflag);
        if (istat == -1) {
            return -1;
        }

    /*
        Copy into the primitive point arrays.
    */
        memcpy ((char *)xprim, (char *)xwork, nprimpts * sizeof(CSW_F));
        memcpy ((char *)yprim, (char *)ywork, nprimpts * sizeof(CSW_F));

        if (memflag) {
            csw_Free (xwork);
            csw_Free (ywork);
            xwork = NULL;
            ywork = NULL;
        }

    /*
        Fill in the fill primitive structure and return.
    */
        fillout->smoothflag = 0;
        fillout->fillpat = CurveFillPattern + 2000;
        fillout->fillcolor = CurveFillColor;
        fillout->pattcolor = CurvePatternColor;
        fillout->x = xprim;
        fillout->y = yprim;
        fillout->linethick = 0.0001f;
        fillout->npts = nprimpts;
        fillout->outline = 0;

        *fills = fillout;
        *nfills = 1;

        CLEANUP_MEM;

        istat = AddToLegend (LEGEND_FILL, 1, CurveFillColor,
                             CurveFillPattern, CurvePatternColor, CurveSymbol, legtext);
        if (!istat) {
            return -1;
        }

        return 1;

    }  /*  end of block to output bottom curve only  */

    if (xtop == NULL  ||  ytop == NULL  ||  xbottom == NULL ||
        ybottom == NULL  ||  itop == NULL  ||  ibottom == NULL) {
        return -1;
    }

/*
    Output a fill primitive representing the intersection
    of the top and bottom squared off polygons.  First,
    allocate space for the polygon clipping results.
*/
    maxcomp = ntop + nbottom;
    maxholes = maxcomp * 2;
    maxpts = maxholes * 4;

    xwork2 = (CSW_F *)csw_Malloc (maxpts * 2 * sizeof(CSW_F));
    if (!xwork2) {
        return -1;
    }
    ywork2 = xwork2 + maxpts;

    iwork2 = (int *)csw_Malloc ((maxcomp + maxholes) * 2 * sizeof(int));
    if (!iwork2) {
        return -1;
    }
    iwork3 = iwork2 + maxcomp;

/*
    Intersect the two polygons.
*/
    istat = ply_calc_obj.ply_CalcIntersect (err_obj,
                                xtop, ytop, itop, nctop,
                                xbottom, ybottom, ibottom, ncbottom,
                                xwork2, ywork2, &nwork2, iwork2, iwork3,
                                maxcomp, maxholes, maxpts);
    if (istat == -1) {
        if (err_obj.csw_ErrNum() == PLY_BADMALLOC) {
            return -1;
        }
        else {
            *fills = NULL;
            *nfills = 0;
            return 1;
        }
    }
    if (nwork2 == 0) {
        *fills = NULL;
        *nfills = 0;
        return 1;
    }

/*
    Compress multiple components into a list of holes.
*/
    n = 0;
    for (i=0; i<nwork2; i++) {
        n += iwork2[i];
    }

    nwork2 = n;

/*
    Allocate space for a single fill prim.
*/
    fillout = (GRaphFillRec *)csw_Malloc (sizeof(GRaphFillRec));
    if (!fillout) {
        return -1;
    }

/*
    Count the points in the output polygon and
    scale xwork2 and ywork2 to plot coordinates.
*/
    n = 0;
    for (i=0; i<nwork2; i++) {
        for (j=0; j<iwork3[i]; j++) {
            if (vflag) {
                yt = ywork2[n];
                ywork2[n] = (xwork2[n] - axmin) / scalex + PageY1;
                xwork2[n] = (yt - aymin) / scaley + PageX1;
            }
            else {
                xwork2[n] = (xwork2[n] - axmin) / scalex + PageX1;
                ywork2[n] = (ywork2[n] - aymin) / scaley + PageY1;
            }
            n++;
        }
    }

/*
    Allocate space for primitive points.  The primitive has
    embedded hole flags, so allocate enough space for the
    points and the hole flags.
*/
    n += nwork2 * 2;
    n += 2;
    xprim = (CSW_F *)csw_Malloc (n * 2 * sizeof(CSW_F));
    if (!xprim) {
        return -1;
    }
    yprim = xprim + n;

/*
    Convert points into nested hole flag format.
*/
    istat = calcdraw_obj.gpf_addholeflags (xwork2, ywork2, iwork3, nwork2,
                              &xwork, &ywork, &nprimpts, &memflag);
    if (istat == -1) {
        return -1;
    }

/*
    Copy into the primitive point arrays.
*/
    memcpy ((char *)xprim, (char *)xwork, nprimpts * sizeof(CSW_F));
    memcpy ((char *)yprim, (char *)ywork, nprimpts * sizeof(CSW_F));

    if (memflag) {
        csw_Free (xwork);
        csw_Free (ywork);
        xwork = NULL;
        ywork = NULL;
    }

/*
    Fill in the fill primitive structure and return.
*/
    fillout->smoothflag = 0;
    fillout->fillpat = CurveFillPattern + 2000;
    fillout->fillcolor = CurveFillColor;
    fillout->pattcolor = CurvePatternColor;
    fillout->x = xprim;
    fillout->y = yprim;
    fillout->linethick = 0.0001f;
    fillout->npts = nprimpts;
    fillout->outline = 0;

    *fills = fillout;
    *nfills = 1;

    istat = AddToLegend (LEGEND_FILL, 1, CurveFillColor,
                         CurveFillPattern, CurvePatternColor, CurveSymbol, legtext);
    if (!istat) {
        return -1;
    }

    bsuccess = true;

    return 1;

}  /*  end of function gpf_CalcGraphCurveFill  */

/*
    Undefine the CLEANUP_MEM macro that is only used for this function.
*/
#undef CLEANUP_MEM





/*
  ****************************************************************

                    R o t a t e E b a r

  ****************************************************************

    Rotate the points in x and y 90 degrees clockwise.

*/

int GPFGraph::RotateEbar (CSW_F *x, CSW_F *y, int n, CSW_F x0, CSW_F y0)
{
    int         i;
    CSW_F       xt;

    for (i=0; i<n; i++) {

        x[i] -= x0;
        y[i] -= y0;

        xt = x[i];
        x[i] = y[i];
        y[i] = -xt;

        x[i] += x0;
        y[i] += y0;
    }

    return 1;

}  /*  end of private RotateEbar function  */



/*
  ****************************************************************

                     L a b e l L o c a t i o n

  ****************************************************************

    Adjust the x and y origin of the curve label based on the
  curve style and shape.

*/

int GPFGraph::LabelLocation (GRaphTextRec *tptr, int vflag, CSW_F curvebase)
{
    CSW_F             twidth, gap;

    gap = 0.0f;
    if (CurveSymbol > 0) {
        gap = CurveSymbolSize / 2.0f;
    }
    gap += MIN_LABEL_GAP;
    if (gap < MIN_LABEL_GAP) gap = MIN_LABEL_GAP;

    twidth = exact_text_length (tptr->text, tptr->size, tptr->font);

    if (CurveStyle == GTX_DRAW_TO_BASELINE) {
        if (vflag) {
            tptr->y -= tptr->size / 2.0f;
            if (tptr->x <= curvebase) {
                tptr->x -= gap;
                tptr->x -= twidth;
            }
            else {
                tptr->x += gap;
            }
        }
        else {
            tptr->x -= twidth / 2.0f;
            if (tptr->y <= curvebase) {
                tptr->y -= gap;
                tptr->y -= tptr->size;
            }
            else {
                tptr->y += gap;
            }
        }
        return 1;
    }

    tptr->x += gap;
    tptr->y += gap;

    return 1;

}  /*  end of private LabelLocation function  */




/*
  ****************************************************************

                 B a r L a b e l L o c a t i o n

  ****************************************************************

    Adjust the x and y origin of the bar label based on the
  stacking flag and orientation of the bar.

*/

int GPFGraph::BarLabelLocation (GRaphTextRec *tptr,
                             CSW_F xt1, CSW_F yt1, CSW_F xt2, CSW_F yt2,
                             int vflag, CSW_F curvebase, CSW_F sthick)
{
    CSW_F             twidth, gap;

    twidth = exact_text_length (tptr->text, tptr->size, tptr->font);

    if (CurveBarStack == GTX_STACK_VERTICAL) {
        tptr->x = (xt1 + xt2 - twidth) / 2.0f;
        tptr->y = (yt1 + yt2 - tptr->size) / 2.0f;

        if (tptr->x < PageX11  ||  tptr->x > PageX22) return -1;
        if (tptr->y < PageY11  ||  tptr->y > PageY22) return -1;
        if (tptr->x + twidth < PageX11  ||  tptr->x + twidth > PageX22) return -1;
        if (tptr->y + tptr->size < PageY11  ||  tptr->y + tptr->size > PageY22) return -1;

        return 1;
    }

    gap = tptr->size / 2.0f;
    if (gap < MIN_LABEL_GAP) gap = MIN_LABEL_GAP;
    if (gap > MAX_LABEL_GAP) gap = MAX_LABEL_GAP;
    gap += sthick;

    if (vflag) {
        tptr->y = (yt1 + yt2) / 2.0f;
        tptr->y -= tptr->size / 2.0f;
        if (tptr->x <= curvebase) {
            tptr->x -= gap;
            tptr->x -= twidth;
        }
        else {
            tptr->x += gap;
        }
    }
    else {
        tptr->x = (xt1 + xt2) / 2.0f;
        tptr->x -= twidth / 2.0f;
        if (tptr->y <= curvebase) {
            tptr->y -= gap;
            tptr->y -= tptr->size;
        }
        else {
            tptr->y += gap;
        }
    }

    if (tptr->x < PageX11  ||  tptr->x > PageX22) return -1;
    if (tptr->y < PageY11  ||  tptr->y > PageY22) return -1;
    if (tptr->x + twidth < PageX11  ||  tptr->x + twidth > PageX22) return -1;
    if (tptr->y + tptr->size < PageY11  ||  tptr->y + tptr->size > PageY22) return -1;

    return 1;

}  /*  end of private BarLabelLocation function  */




/*
  ****************************************************************

               g p f _ I n i t G r a p h B a r s

  ****************************************************************

   This must be called to start the calculation of a set of bars
  on a graph.  All of the bars in the set must share the same X
  and Y axes.  The CurveBarStack is set via the flag parameter to
  insure that all curves share the same value of it.

*/

int GPFGraph::gpf_InitGraphBars (int flag, int nset, CSW_F major,
                       CSW_F axmin, CSW_F axmax, CSW_F xlog)
{
    CSW_F       lnbasex;
    int         i;

    CurveBarStack = flag;
    BarMajor = major;
    BarAxmin = axmin;
    axmax = axmax;
    /* BarAxmax = axmax; */

    if (xlog > 1.01f) {
        BarMajor = 1;
        lnbasex = (CSW_F)log ((double)xlog);
        BarAxmin = (CSW_F)log ((double)axmin) / lnbasex;
        /* BarAxmax = (CSW_F)log ((double)axmax) / lnbasex; */
    }

    NbarSets = 0;
    TotalBarSets = nset;
    for (i=0; i<GTX_MAX_AXIS_LABELS; i++) {
        BarPointBases[i] = -1.e20f;
    }

    if (flag == GTX_STACK_HORIZONTAL) {
        BarWidthPct = BarWidthPct / (CSW_F)(nset);
        BarGapPct = BarGapPct / (CSW_F)(nset + 1);
        if (nset > 1) {
            BarGapPct *= GAP_FRACTION;
        }
    }
    else {
        BarGapPct /= 2.0f;
    }

    OrigBarWidthPct = BarWidthPct;
    OrigBarGapPct = BarGapPct;

    if (CurveShadowColor >= 0) BarWidthPct -= BarGapPct * SHADOW_FRACTION;

    return 1;

}  /*  end of function gpf_InitGraphBars  */




/*
  ****************************************************************

                    G e t P o i n t B a s e

  ****************************************************************

  Return the current base value of a vertically stacked bar
  chart interval.  If the bar chart is horizontally stacked,
  return the CurveBaseValue.  The base value is looked up from
  the BarPointBases private array, which was initialized to
  -1.e20 values by gpf_InitGraphBars.

*/

CSW_F GPFGraph::GetPointBase (CSW_F xval)
{
    CSW_F        tiny;
    int          i;

    if (CurveBarStack == GTX_STACK_HORIZONTAL) {
        return CurveBaseValue;
    }

    tiny = BarMajor / 100.f;
    i = (int)((xval - BarAxmin + tiny) / BarMajor);
    if (i < 0) i = 0;
    if (i >= GTX_MAX_AXIS_LABELS) i = GTX_MAX_AXIS_LABELS - 1;

    if (BarPointBases[i] < -1.e15f) return CurveBaseValue;

    return BarPointBases[i];

}  /*  end of private function GetPointBase  */




/*
  ****************************************************************

                    S e t P o i n t B a s e

  ****************************************************************

    Set the BarPointsBase array element corresponding to xval to
  the value specified in yval.

*/

int GPFGraph::SetPointBase (CSW_F xval, CSW_F yval)
{
    CSW_F        tiny;
    int          i;

    tiny = BarMajor / 100.f;
    i = (int)((xval - BarAxmin + tiny) / BarMajor);
    if (i < 0) i = 0;
    if (i >= GTX_MAX_AXIS_LABELS) i = GTX_MAX_AXIS_LABELS - 1;

    BarPointBases[i] = yval;

    return 1;

}  /*  end of private function SetPointBase  */



/*
  ****************************************************************

       g p f _ S e t I n i t i a l G r a p h B a r F l a g

  ****************************************************************

*/

int GPFGraph::gpf_SetInitialGraphBarFlag (int val)
{

    InitialGraphBarFlag = val;
    return 1;

}  /*  end of function gpf_SetInitialGraphBarFlag  */




/*
  ****************************************************************

              g p f _ C a l c G r a p h B a r s

  ****************************************************************

  function name:   gpf_CalcGraphBars       (int)

  call sequence:   gpf_CalcGraphBars (x, y, npts, legtext, vflag,
                                      axmin, axmax, xlog,
                                      aymin, aymax, ylog,
                                      lines, nlines, text, ntext, fills, nfills,
                                      baseline)

  purpose:         Calculate primitive shapes that make up a graph
                   bar set.

  return value:    status code

                   1 = normal success
                  -1 = csw_Malloc error getting work space

  calling parameters:

    x       r   CSW_F*          array of graph x coordinates
    y       r   CSW_F*          array of graph y coordinates
    npts    r   int             number of points
    legtext r   char*           legend text entry
    vflag   r   int             0 for horizontal independent axis,
                                1 for vertical independent axis
    axmin   r   CSW_F           minimum x axis value
    axmax   r   CSW_F           maximum x axis value
    xlog    r   CSW_F           log base for x axis
    aymin   r   CSW_F           minimum y axis value
    aymax   r   CSW_F           maximum y axis value
    ylog    r   CSW_F           log base for y axis
    lines   w   GRaphLineRec**  returned list of lines
    nlines  w   int*            number of returned lines
    text    w   GRaphTextRec**  returned list of text primitives
    ntext   w   int*            number of returned text primititves
    fills   w   GRaphFillRec**  returned list of fill areas
    nfills  w   int*            number of returned fill primitives
    baseline w  GRaphLineRec**  returned base line record

*/

int GPFGraph::gpf_CalcGraphBars  (CSW_F *x, CSW_F *y, int npts, const char *legtext, int vflag,
                        CSW_F axmin, CSW_F axmax, CSW_F xlog,
                        CSW_F aymin, CSW_F aymax, CSW_F ylog,
                        GRaphLineRec **lines, int *nlines,
                        GRaphTextRec **text, int *ntext,
                        GRaphFillRec **fills, int *nfills,
                        GRaphLineRec **baseline)
{
    CSW_F         *xwork = NULL, *ywork = NULL, *xptr = NULL, *yptr = NULL,
                  lnbasex, lnbasey, scalex, scaley, xt, yt,
                  curvebase, xt1, yt1, xt2, yt2, shadowthick, ybase,
                  xzero, yzero, xtmp;
    int           i, logflagx, logflagy, istat;
    int           nmem, itmp, nbarsets, vdir;
    GRaphLineRec  *lptr = NULL, *lptrlist = NULL;
    GRaphTextRec  *tptr = NULL, *tptrlist = NULL;
    GRaphFillRec  *fptr = NULL, *fptrlist = NULL;
    CSW_F         savecurvebaseval;

    bool     bsuccess = false;

    savecurvebaseval = CurveBaseValue;

    auto fscope = [&]()
    {
        CurveBaseValue = savecurvebaseval;
        if (bsuccess == false) {
            gpf_FreeGraphLines (lptrlist, *nlines, 1);
            gpf_FreeGraphFills (fptrlist, *nfills);
            gpf_FreeGraphText (tptrlist, *ntext);
        }
        csw_Free (xwork);
        csw_Free (xptr);
    };
    CSWScopeGuard func_scope_guard (fscope);


    *nlines = 0;
    *ntext = 0;
    *nfills = 0;

    istat = DrawLegendOnly (LEGEND_FILL, CurveColor, CurveFillColor, CurveFillPattern,
                            CurvePatternColor, CurveSymbol, legtext);
    if (istat == 1) {
        return 1;
    }

    lnbasex = 1.0;
    lnbasey = 1.0;

/*
    Initialize local pointers.
*/
    lptrlist = NULL;
    fptrlist = NULL;
    tptrlist = NULL;
    lptr = NULL;
    fptr = NULL;
    tptr = NULL;

    *lines = NULL;
    *text = NULL;
    *fills = NULL;
    *nlines = 0;
    *ntext = 0;
    *nfills = 0;

    xzero = 0.0;
    yzero = 0.0;

/*
    Convert the curve base value to log if needed.
*/
    savecurvebaseval = CurveBaseValue;
    if (ylog > MIN_LOG_BASE) {
        if (CurveBaseValue > 0.0) {
            lnbasey = (CSW_F)log ((double)ylog);
            CurveBaseValue = (CSW_F)log ((double)CurveBaseValue) / lnbasey;
        }
    }

/*
    Check for logarithmic axes.
*/
    logflagx = 0;
    logflagy = 0;
    if (xlog > MIN_LOG_BASE) {
        xzero = axmin / 100.f;
        lnbasex = (CSW_F)log ((double)xlog);
        axmin = (CSW_F)log ((double)axmin) / lnbasex;
        axmax = (CSW_F)log ((double)axmax) / lnbasex;
        logflagx = 1;
    }

    if (ylog > MIN_LOG_BASE) {
        yzero = aymin / 100.f;
        lnbasey = (CSW_F)log ((double)ylog);
        aymin = (CSW_F)log ((double)aymin) / lnbasey;
        aymax = (CSW_F)log ((double)aymax) / lnbasey;
        logflagy = 1;
    }

/*
    allocate work space for the bar calculations
*/
    xwork = (CSW_F *)csw_Malloc (4 * npts * sizeof(CSW_F));
    if (!xwork) {
        return -1;
    }
    ywork = xwork + npts;

/*
    Put x and y bar values into the work arrays,
    converting to log values if needed.
*/
    for (i=0; i<npts; i++) {

        xwork[i] = x[i];
        ywork[i] = y[i];

        if (logflagx) {
            xt = x[i];
            if (xt < xzero) xt = xzero;
            xwork[i] = (CSW_F)(log ((double)xt) / lnbasex);
        }
        if (logflagy) {
            yt = y[i];
            if (yt < yzero) yt = yzero;
            ywork[i] = (CSW_F)(log ((double)yt) / lnbasey);
        }
    }

/*
    Allocate points for scaling into plotter coordinates.
*/
    xptr = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
    if (!xptr) {
        return -1;
    }
    yptr = xptr + npts;

    if (vflag) {
        scalex = (axmax - axmin) / (PageY2 - PageY1);
        scaley = (aymax - aymin) / (PageX2 - PageX1);
    }
    else {
        scalex = (axmax - axmin) / (PageX2 - PageX1);
        scaley = (aymax - aymin) / (PageY2 - PageY1);
    }

/*
    Scale points into plotter coordinates.  Shift each
    x coordinate to the minimum of its major interval first.
    Special handling here for vertically stacked bar sets.
*/
    for (i=0; i<npts; i++) {
        itmp = (int)((xwork[i] - axmin) / BarMajor);
        xwork[i] = itmp * BarMajor + axmin;
        if (InitialGraphBarFlag  ||  CurveBarStack != GTX_STACK_VERTICAL) {
            ybase = GetPointBase (xwork[i]) - CurveBaseValue;
        }
        else {
            ybase = GetPointBase (xwork[i]);
        }
        if (ylog > MIN_LOG_BASE  &&
            !InitialGraphBarFlag  &&
            CurveBarStack == GTX_STACK_VERTICAL) {
            xt = (CSW_F)(pow ((double)ylog, (double)ywork[i]) +
                         pow ((double)ylog, (double)ybase));
            if (xt < yzero) xt = yzero;
            ywork[i] = (CSW_F)(log ((double)xt) / lnbasey);
        }
        else {
            ywork[i] += ybase;
        }
        if (vflag) {
            yptr[i] = (xwork[i] - axmin) / scalex + PageY1;
            xptr[i] = (ywork[i] - aymin) / scaley + PageX1;
        }
        else {
            xptr[i] = (xwork[i] - axmin) / scalex + PageX1;
            yptr[i] = (ywork[i] - aymin) / scaley + PageY1;
        }
    }

/*
    allocate line primitive structures for the bar set
*/
    nmem = npts + 2;
    if (CurveShadowColor >= 0) {
        nmem *= 3;
    }
    lptrlist = (GRaphLineRec *)csw_Calloc (nmem * sizeof(GRaphLineRec));
    if (!lptrlist) {
        return -1;
    }
    lptr = lptrlist;

/*
    allocate fill primitive structures for the bar set
*/
    nmem = npts + 1;
    if (CurveShadowColor >= 0) {
        nmem *= 3;
    }
    fptrlist = (GRaphFillRec *)csw_Calloc (nmem * sizeof(GRaphFillRec));
    if (!fptrlist) {
        return -1;
    }
    fptr = fptrlist;

/*
    allocate text primitive structures for the bar set
*/
    if (CurveLabelSpacing > 0) {
        tptrlist = (GRaphTextRec *)csw_Calloc ((npts+1) * sizeof(GRaphTextRec));
        if (!tptrlist) {
            return -1;
        }
        tptr = tptrlist;
    }

    *nlines = 0;
    *ntext = 0;
    *nfills = 0;

/*
    Convert the bar base value to page units.
*/
    if (vflag) {
        curvebase = (CurveBaseValue - aymin) /scaley + PageX1;
        if (curvebase < PageX1) curvebase = PageX1;
        if (curvebase > PageX2) curvebase = PageX2;
    }
    else {
        curvebase = (CurveBaseValue - aymin) /scaley + PageY1;
        if (curvebase < PageY1) curvebase = PageY1;
        if (curvebase > PageY2) curvebase = PageY2;
    }

    nbarsets = NbarSets;
    if (CurveBarStack == GTX_STACK_VERTICAL) nbarsets = 0;

/*
    Calculate the line, fill and text primitives for each bar in the set.
*/
    for (i=0; i<npts; i++) {

        if (CurveShadowColor < 0) {
            shadowthick = 0.0;
        }
        else {
            if (vflag) {
                shadowthick = BarGapPct * BarMajor / scalex * SHADOW_FRACTION;
            }
            else {
                shadowthick = BarGapPct * BarMajor / scalex * SHADOW_FRACTION;
            }
            if (shadowthick < 0.0) shadowthick = -shadowthick;
        }

    /*
        Get rectangle corner points for a vertical independent axis.
    */
        if (vflag) {
            if (yptr[i] < PageY1  ||  yptr[i] > PageY2) {
                continue;
            }
            xt1 = (GetPointBase (xwork[i]) - aymin) / scaley + PageX1;
            SetPointBase (xwork[i], ywork[i]);
            xt2 = xptr[i];
            if (xt1 < PageX11) xt1 = PageX11;
            if (xt1 > PageX22) xt1 = PageX22;
            if (xt2 < PageX11) xt2 = PageX11;
            if (xt2 > PageX22) xt2 = PageX22;
            yt1 = OrigBarGapPct * (nbarsets + 1) + OrigBarWidthPct * nbarsets;
            yt1 *= BarMajor;
            yt1 = yt1 / scalex + yptr[i];
            yt2 = yt1 + BarWidthPct * BarMajor / scalex;
            xt1 -= shadowthick;
            xt2 -= shadowthick;
            if (xt1 < PageX11) xt1 = PageX11;
            if (xt1 > PageX22) xt1 = PageX22;
            if (xt2 < PageX11) xt2 = PageX11;
            if (xt2 > PageX22) xt2 = PageX22;
        }

    /*
        Get rectangle corner points for a horizontal independent axis.
    */
        else {
            if (xptr[i] < PageX1  ||  xptr[i] > PageX2) {
                continue;
            }
            yt1 = (GetPointBase (xwork[i]) - aymin) / scaley + PageY1;
            SetPointBase (xwork[i], ywork[i]);
            yt2 = yptr[i];
            if (yt1 < PageY11) yt1 = PageY11;
            if (yt1 > PageY22) yt1 = PageY22;
            if (yt2 < PageY11) yt2 = PageY11;
            if (yt2 > PageY22) yt2 = PageY22;
            xt1 = OrigBarGapPct * (nbarsets + 1) + OrigBarWidthPct * nbarsets;
            xt1 *= BarMajor;
            xt1 = xt1 / scalex + xptr[i];
            xt2 = xt1 + BarWidthPct * BarMajor / scalex;
            yt1 -= shadowthick;
            yt2 -= shadowthick;
            if (yt1 < PageY11) yt1 = PageY11;
            if (yt1 > PageY22) yt1 = PageY22;
            if (yt2 < PageY11) yt2 = PageY11;
            if (yt2 > PageY22) yt2 = PageY22;
        }

    /*
        Adjust for right to left axes.
    */
        vdir = TotalBarSets - 1;
        if (xt1 > xt2) {
            xtmp = xt1;
            xt1 = xt2;
            xt2 = xtmp;
            vdir = 0;
        }
        if (yt1 > yt2) {
            xtmp = yt1;
            yt1 = yt2;
            yt2 = xtmp;
            vdir = 0;
        }

    /*
        Draw a line on 4 sides of bar rectangle.
    */
        lptr->x = (CSW_F *)csw_Malloc (10 * sizeof(CSW_F));
        if (!lptr->x) {
            return -1;
        }
        lptr->y = lptr->x + 5;
        lptr->x[0] = xt1;
        lptr->y[0] = yt1;
        lptr->x[1] = xt2;
        lptr->y[1] = yt1;
        lptr->x[2] = xt2;
        lptr->y[2] = yt2;
        lptr->x[3] = xt1;
        lptr->y[3] = yt2;
        lptr->x[4] = xt1;
        lptr->y[4] = yt1;
        lptr->npts = 5;
        lptr->smoothflag = 0;
        lptr->linepat = CurvePattern;
        lptr->linecolor = CurveColor;
        lptr->linethick = CurveThick;
        lptr->dashscale = CurveDashscale;
        lptr++;
        (*nlines)++;

    /*
         Create a fill for the rectangular part of the bar
         unless the fill color is less than zero.
    */
        if (CurveFillColor >= 0) {
            fptr->x = (CSW_F *)csw_Malloc (10 * sizeof (CSW_F));
            if (!fptr->x) {
                return -1;
            }
            fptr->y = fptr->x + 5;
            fptr->x[0] = xt1;
            fptr->y[0] = yt1;
            fptr->x[1] = xt2;
            fptr->y[1] = yt1;
            fptr->x[2] = xt2;
            fptr->y[2] = yt2;
            fptr->x[3] = xt1;
            fptr->y[3] = yt2;
            fptr->x[4] = xt1;
            fptr->y[4] = yt1;
            fptr->smoothflag = 0;
            fptr->fillpat = CurveFillPattern + 2000;
            fptr->fillcolor = CurveFillColor;
            fptr->pattcolor = CurvePatternColor;
            fptr->npts = 5;
            fptr->outline = 0;
            fptr++;
            (*nfills)++;
        }

    /*
         Create a polygon for the side shadow if needed.
    */
        if (CurveShadowColor >= 0) {

            fptr->x = (CSW_F *)csw_Malloc (10 * sizeof(CSW_F));
            if (!fptr->x) {
                return -1;
            }
            fptr->y = fptr->x + 5;
            if (vflag) {
                fptr->x[0] = xt1;
                fptr->y[0] = yt2;
                fptr->x[1] = xt1+shadowthick;
                fptr->y[1] = yt2+shadowthick;
                fptr->x[2] = xt2+shadowthick;
                fptr->y[2] = yt2+shadowthick;
                fptr->x[3] = xt2;
                fptr->y[3] = yt2;
                fptr->x[4] = xt1;
                fptr->y[4] = yt2;
            }
            else {
                fptr->x[0] = xt2;
                fptr->y[0] = yt2;
                fptr->x[1] = xt2+shadowthick;
                fptr->y[1] = yt2+shadowthick;
                fptr->x[2] = xt2+shadowthick;
                fptr->y[2] = yt1+shadowthick;
                fptr->x[3] = xt2;
                fptr->y[3] = yt1;
                fptr->x[4] = xt2;
                fptr->y[4] = yt2;
            }
            fptr->smoothflag = 0;
            fptr->fillpat = 0;
            fptr->fillcolor = CurveShadowColor;
            fptr->npts = 5;
            fptr->outline = 0;
            fptr++;
            (*nfills)++;

        /*
            Create edge outlines for the side shadow.
        */
            lptr->x = (CSW_F *)csw_Malloc (8 * sizeof(CSW_F));
            if (!lptr->x) {
                return -1;
            }
            lptr->y = lptr->x + 4;
            if (vflag) {
                lptr->x[0] = xt1;
                lptr->y[0] = yt2;
                lptr->x[1] = xt1 + shadowthick;
                lptr->y[1] = yt2 + shadowthick;
                lptr->x[2] = xt2 + shadowthick;
                lptr->y[2] = yt2 + shadowthick;
                lptr->x[3] = xt2;
                lptr->y[3] = yt2;
            }
            else {
                lptr->x[0] = xt2;
                lptr->y[0] = yt2;
                lptr->x[1] = xt2 + shadowthick;
                lptr->y[1] = yt2 + shadowthick;
                lptr->x[2] = xt2 + shadowthick;
                lptr->y[2] = yt1 + shadowthick;
                lptr->x[3] = xt2;
                lptr->y[3] = yt1;
            }
            lptr->npts = 4;
            lptr->smoothflag = 0;
            lptr->linepat = CurvePattern;
            lptr->linecolor = CurveColor;
            lptr->linethick = CurveThick;
            lptr->dashscale = CurveDashscale;
            lptr++;
            (*nlines)++;

        /*
            Create fill and outline for the end shadow.
        */
            if (NbarSets == vdir  ||  CurveBarStack == GTX_STACK_HORIZONTAL) {
                lptr->x = (CSW_F *)csw_Malloc (6 * sizeof(CSW_F));
                if (!lptr->x) {
                    return -1;
                }
                lptr->y = lptr->x + 3;
                if (vflag) {
                    lptr->x[0] = xt2 + shadowthick;
                    lptr->y[0] = yt2 + shadowthick;
                    lptr->x[1] = xt2 + shadowthick;
                    lptr->y[1] = yt1 + shadowthick;
                    lptr->x[2] = xt2;
                    lptr->y[2] = yt1;
                }
                else {
                    lptr->x[0] = xt2 + shadowthick;
                    lptr->y[0] = yt2 + shadowthick;
                    lptr->x[1] = xt1 + shadowthick;
                    lptr->y[1] = yt2 + shadowthick;
                    lptr->x[2] = xt1;
                    lptr->y[2] = yt2;
                }
                lptr->npts = 3;
                lptr->smoothflag = 0;
                lptr->linepat = CurvePattern;
                lptr->linecolor = CurveColor;
                lptr->linethick = CurveThick;
                lptr->dashscale = CurveDashscale;
                lptr++;
                (*nlines)++;

                fptr->x = (CSW_F *)csw_Malloc (10 * sizeof(CSW_F));
                if (!fptr->x) {
                    return -1;
                }
                fptr->y = fptr->x + 5;
                if (vflag) {
                    fptr->x[0] = xt2 + shadowthick;
                    fptr->y[0] = yt2 + shadowthick;
                    fptr->x[1] = xt2 + shadowthick;
                    fptr->y[1] = yt1 + shadowthick;
                    fptr->x[2] = xt2;
                    fptr->y[2] = yt1;
                    fptr->x[3] = xt2;
                    fptr->y[3] = yt2;
                    fptr->x[4] = xt2 + shadowthick;
                    fptr->y[4] = yt2 + shadowthick;
                }
                else {
                    fptr->x[0] = xt2 + shadowthick;
                    fptr->y[0] = yt2 + shadowthick;
                    fptr->x[1] = xt1 + shadowthick;
                    fptr->y[1] = yt2 + shadowthick;
                    fptr->x[2] = xt1;
                    fptr->y[2] = yt2;
                    fptr->x[3] = xt2;
                    fptr->y[3] = yt2;
                    fptr->x[4] = xt2 + shadowthick;
                    fptr->y[4] = yt2 + shadowthick;
                }
                fptr->smoothflag = 0;
                fptr->fillpat = 0;
                fptr->fillcolor = CurveShadowColor;
                fptr->npts = 5;
                fptr->outline = 0;
                fptr++;
                (*nfills)++;
            }
        }

    /*
        Label the bar with its y value if specified.
    */
        if (CurveLabelSpacing > 0  &&  tptr != NULL) {

            if (i % CurveLabelSpacing == 0) {
                tptr->text = (char *)csw_Malloc (LABEL_LENGTH * sizeof(char));
                if (!tptr->text) {
                    return -1;
                }
                FormatNumber (y[i], 0.0, tptr->text);
                tptr->size = CurveLabelSize;
                tptr->thick = CurveLabelThick;
                tptr->angle = 0.0;
                tptr->nchar = strlen(tptr->text);
                tptr->fillcolor = CurveLabelFillColor;
                tptr->color = CurveLabelColor;
                tptr->font = CurveLabelFont;
                tptr->x = xptr[i];
                tptr->y = yptr[i];
                istat = BarLabelLocation (tptr, xt1, yt1, xt2, yt2, vflag,
                                          curvebase, shadowthick);
                if (istat == -1) {
                    csw_Free (tptr->text);
                    tptr->text = NULL;
                    continue;
                }
                tptr++;
                (*ntext)++;
            }
        }

    }

/*
    Calculate a line primitive for the curve base value if needed.
*/
    *baseline = NULL;
    if (vflag) {
        if (CurveBaseColor >= 0  &&  curvebase > PageX11  &&  curvebase < PageX22) {
            *baseline = lptr;
            lptr->x1 = curvebase;
            lptr->x2 = curvebase;
            lptr->y1 = PageY1;
            lptr->y2 = PageY2;
            lptr->npts = -2;
            lptr->smoothflag = 0;
            lptr->linepat = CurveBasePattern;
            lptr->linecolor = CurveBaseColor;
            lptr->linethick = CurveBaseThick;
            lptr->dashscale = CurveBaseDashscale;
        }
    }
    else {
        if (CurveBaseColor >= 0  &&  curvebase > PageY11  &&  curvebase < PageY22) {
            *baseline = lptr;
            lptr->x1 = PageX1;
            lptr->x2 = PageX2;
            lptr->y1 = curvebase;
            lptr->y2 = curvebase;
            lptr->npts = -2;
            lptr->smoothflag = 0;
            lptr->linepat = CurveBasePattern;
            lptr->linecolor = CurveBaseColor;
            lptr->linethick = CurveBaseThick;
            lptr->dashscale = CurveBaseDashscale;
        }
    }

    *lines = lptrlist;

/*
    Add a legend entry if needed.
*/
    if (lines == NULL) {
        i = LEGEND_POINT;
    }
    else {
        if (CurveSymbol > 0) {
            i = LEGEND_SYMB;
        }
        else {
            i = LEGEND_LINE;
        }
    }

    istat = AddToLegend (LEGEND_FILL, CurveColor, CurveFillColor, CurveFillPattern,
                         CurvePatternColor, CurveSymbol, legtext);
    if (!istat) {
        return -1;
    }

    NbarSets++;

    if (lines != NULL) {
        *lines = lptrlist;
    }
    *text = tptrlist;
    *fills = fptrlist;

    bsuccess = true;

    return 1;

}  /*  end of function gpf_CalcGraphBars  */



/*
  ****************************************************************

      g p f _ s e t _ g r a p h _ d e c i m a l _ p l a c e s

  ****************************************************************

*/

int GPFGraph::gpf_set_graph_decimal_places (CSW_F val)
{
    double       logval;

    if (val < 0.0) val = -val;
    val /= 10.0;
    if (val == 0.0) {
        NumDec = 7;
        return 1;
    }

    logval = log10 ((double)val);
    if (logval > 0.0) {
        NumDec = 7;
        return 1;
    }

    logval = -logval;
    NumDec = (int)(logval + 1.5);

    return 1;

}  /*  end of function gpf_set_graph_decimal_places  */



/*
  ****************************************************************

  g p f _ s e t _ l e g e n d _ t i t l e _ b o r d e r _ f l a g

  ****************************************************************

*/

int GPFGraph::gpf_set_legend_title_border_flag (int val)
{

    if (val < -2) val = -2;
    if (val > 2) val = 2;

    LegendTitleBorderFlag = val;

    return 1;

}  /*  end of function gpf_set_legend_title_border_flag  */



/*
  ****************************************************************

           g p f _ S e t D r a w A l l L e g e n d F l a g

  ****************************************************************

*/

int GPFGraph::gpf_SetDrawAllLegendFlag (int val)
{

    DrawAllLegendFlag = val;

    /* bypass compiler warning */
    DrawAllLegendFlag = DrawAllLegendFlag;

    return 1;

}  /*  end of function gpf_SetDrawAllLegendFlag  */




/*
  ****************************************************************

       g p f _ s e t _ l e g e n d _ a u t o _ a n c h o r

  ****************************************************************

*/

int GPFGraph::gpf_set_legend_auto_anchor (CSW_F x1, int anchor)
{

    AutoLegendAnchor = anchor;
    AutoLegendX1 = x1;

    return 1;

}  /*  end of function gpf_set_legend_auto_anchor  */




/*
  ****************************************************************

       g p f _ g e t _ l e g e n d _ a u t o _ a n c h o r

  ****************************************************************

*/

int GPFGraph::gpf_get_legend_auto_anchor (CSW_F *x1, int *anchor)
{

    *anchor = AutoLegendAnchor;
    *x1 = AutoLegendX1;

    return 1;

}  /*  end of function gpf_set_legend_auto_anchor  */




/*
  ****************************************************************

       g p f _ C a l c A u t o A x i s I n t e r v a l s

  ****************************************************************

    This is very similar to gpf_CalcAxisIntervals but it does
  not have any manual override.

*/

int GPFGraph::gpf_CalcAutoAxisIntervals (CSW_F datamin, CSW_F datamax, int nmajor,
                               CSW_F *axismin, CSW_F *axismax,
                               CSW_F *axismajor, CSW_F *axisminor)
{
    CSW_F      major, minor, min, max, origdelta, ftmp,
               delta, logdelta, base, mantissa;

/*
    Make sure axis min and max agree with direction flag.
*/
    if (AxisDirectionFlag == GTX_AXIS_MIN_FIRST) {
        if (datamin > datamax) {
            ftmp = datamin;
            datamin = datamax;
            datamax = ftmp;
        }
    }
    else {
        if (datamin < datamax) {
            ftmp = datamin;
            datamin = datamax;
            datamax = ftmp;
        }
    }

/*
    check for obvious errors
*/
    if (nmajor < 1) {
        nmajor = 1;
    }

    origdelta = (datamax - datamin) / nmajor;
    delta = origdelta;
    if (origdelta < 0.0f) {
        delta = -delta;
        ftmp = datamin;
        datamin = datamax;
        datamax = ftmp;
    }
    if (delta <= NEAR_ZERO) {
        delta = NEAR_ZERO;
    }

/*
    get the largest value that is a power of 10 and less than
    the previously calculated delta
*/
    logdelta = (CSW_F)log10 ((double)delta);
    logdelta = (CSW_F)floor ((double)logdelta);
    base = (CSW_F)pow ((double)10.0, (double)logdelta);
    mantissa = delta / base;

/*
    use a "round" value fairly near the actual delta value
    for the major interval and adjust the minor interval
    according to the major interval.
*/
    if (mantissa < 1.5f) {
        major = base;
        minor = major / 5.0f;
    }
    else if (mantissa < 3.0f) {
        major = 2.0f * base;
        minor = major / 4.0f;
    }
    else if (mantissa < 4.5f) {
        major = 4.0f * base;
        minor = major / 4.0f;
    }
    else if (mantissa < 7.5f) {
        major = 5.0f * base;
        minor = major / 5.0f;
    }
    else {
        major = 10.0f * base;
        minor = major / 5.0f;
    }

    if (AxisLogBase > MIN_LOG_BASE) {
        major = 1.0f;
        minor = 1.0f / AxisLogBase;
    }

/*
    time axis labels round differently
*/
    if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {

        if (delta < 5) {
            major = 5;
            minor = 1;
        }
        else if (delta < 15) {
            major = 10;
            minor = 2;
        }
        else if (delta < 25) {
            major = 20;
            minor = 4;
        }
        else if (delta < 45) {
            major = 30;
            minor = 5;
        }
        else if (delta < 95) {
            major = 60;
            minor = 10;
        }
        else if (delta < 150) {
            major = 120;
            minor = 30;
        }
        else if (delta < 450) {
            major = 300;
            minor = 60;
        }
        else if (delta < 750) {
            major = 600;
            minor = 120;
        }
        else if (delta < 1150) {
            major = 900;
            minor = 300;
        }
        else if (delta < 1350) {
            major = 1200;
            minor = 600;
        }
        else if (delta < 2700) {
            major = 1800;
            minor = 600;
        }
        else if (delta < 4500) {
            major = 3600;
            minor = 1200;
        }
        else if (delta < 3600 * 3) {
            major = 3600 * 2;
            minor = 1800;
        }
        else if (delta < 3600 * 8) {
            major = 3600 * 5;
            minor = 3600;
        }
        else if (delta < 3600 * 18) {
            major = 3600 * 12;
            minor = 3600 * 3;
        }
        else if (delta < 3600 * 72) {
            major = 3600 * 24;
            minor = 3600 * 6;
        }
        else {
            major = 3600 * 24 * 7;
            minor = 3600 * 24;
        }
    }

/*
    The axis minimum is the largest value evenly divisible
    by major and less than the datamin.  Axis max is smallest
    number evenly divisible by major and larger than datamax.
*/
    min = (CSW_F)(floor ((double)(datamin/major)) * major);
    max = (CSW_F)(ceil ((double)(datamax/major)) * major);

/*
    If this is a reversed direction axis, adjust for that.
*/
    if (origdelta < 0.0) {
        major = -major;
        minor = -minor;
        ftmp = min;
        min = max;
        max = ftmp;
    }

/*
    special handling for time data
*/
    RoundOffTime (&min, &max, major);

/*
    Return the calculated values.
*/
    *axismin = min;
    *axismax = max;
    *axismajor = major;
    *axisminor = minor;

    return 1;

}  /*  end of function gpf_CalcAutoAxisIntervals  */




/*
  ****************************************************************

         g p f _ s e t _ s k i p _ l e g e n d _ f l a g

  ****************************************************************

*/

int GPFGraph::gpf_set_skip_legend_flag (int val)
{

    SkipLegendFlag = - (val + 1);
    return 1;

}  /*  end of function gpf_set_skip_legend_flag  */




/*
  ****************************************************************

                  D r a w L e g e n d O n l y

  ****************************************************************

    Check the legtext for the preamble ".only.".  If it is found,
  the rest of the legtext string is passed to AddToLegend and
  1 is returned.  Otherwise, zero is returned.

*/

int GPFGraph::DrawLegendOnly (int type, int lcolor, int fcolor,
                           int patt, int pcolor, int symb,
                           const char *text)
{
    char        ltext[MAX_LEG_TEXT];

    csw_StrTruncate (ltext, text, MAX_LEG_TEXT);
    csw_StrLeftJust (ltext);

    if (strncmp (ltext, LEGEND_ONLY_STRING, LEGEND_ONLY_LEN)) {
        return 0;
    }

    AddToLegend (type, lcolor, fcolor, patt, pcolor, symb,
                 ltext + LEGEND_ONLY_LEN);

    return 1;

}  /*  end of private DrawLegendOnly function  */




/*
  ***************************************************************

                     R o u n d O f f T i m e

  ***************************************************************

    Given a time in seconds since the epoch, adjust it so that
  the formatted time string has even numbers of minutes, seconds,
  days, etc.

*/

int GPFGraph::RoundOffTime (CSW_F *f_min, CSW_F *f_max, CSW_F fmajor)
{
    int            i, dt, n,
                   min, max, major;
    time_t         sec;
    struct tm      *ts;

    if (AxisTimeFormat == 0) return 1;

    min = (int) (*f_min + .01);
    max = (int) (*f_max + .01);
    major = (int) (fmajor + .01);

    n = (max - min) / major;

    sec = min;
    sec += AxisBaseTime;

    ts = localtime (&sec);

    dt = 0;
    if (major < 60  ||  major % 60 != 0) {
        dt = ts->tm_sec % major;
    }

    else if (major < 3600  ||  major % 3600 != 0) {
        i = major / 60;
        dt = ts->tm_sec + (ts->tm_min % i) * 60;
    }

    else if (major < 24 * 3600  ||  major % (24 * 3600) != 0) {
        i = major / 3600;
        dt = ts->tm_sec + ts->tm_min * 60 +
             (ts->tm_hour % i) * 3600;
    }

    else {
        i = major / (3600 * 24);
        dt = ts->tm_sec + ts->tm_min * 60 +
             ts->tm_hour * 3600 + (ts->tm_wday % i) * 3600 * 24;
    }

    sec -= dt;

    min -= dt;
    i = min + n * major;
    while (i < max) {
        i += major;
    }

    *f_min = (CSW_F)min;
    *f_max = (CSW_F)i;

    return 1;

}




/*
  ****************************************************************

         g p f _ u p d a t e _ g r a p h _ l i m i t s

  ****************************************************************

    Change the page corners for pie chart title drawing.

*/

int GPFGraph::gpf_update_graph_limits (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2)
{
    CSW_F         tiny;

    PageX1 = x1;
    PageY1 = y1;
    PageX2 = x2;
    PageY2 = y2;

    tiny = (x2 - x1 + y2 - y1) / 2000.f;

    PageX11 = x1 + tiny;
    PageY11 = y1 + tiny;
    PageX22 = x2 - tiny;
    PageY22 = y2 - tiny;

    GXmin = x1;
    GYmin = y1;
    GXmax = x2;
    GYmax = y2;

    return 1;

}  /*  end of function gpf_update_graph_limits  */


CSW_F GPFGraph::exact_text_length (char *text, CSW_F size, int font)

{
    int      nc;
    CSW_F    tlen;
    CSW_F    bounds[10], tsize;

    nc = strlen (text);
    if (nc < 1) {
        return 0.0f;
    }

/*
 * get either stroke or fill length
 */
    if ((font < 8  ||  font > 100)  && font < 200) {
        tlen = stroke_text_length (text, size, font);
    }
    else {
        tsize = size / PageUnitsPerInch;
        gtx_GetTextBounds (
            dlist_index,
            text,
            font,
            tsize,
            bounds);
        tlen = bounds[0] * PageUnitsPerInch;
    }

    return tlen;

}  /*  end of function exact_text_length  */


CSW_F GPFGraph::stroke_text_length (char *text, CSW_F size, int font)

{
    CSW_F    xt, textlen, charwidth;
    int      i, j, n, nparts, nc, istat;
    int      parts[MAXPARTS];

    font_obj.gpf_setfont (font);

/*  loop through text, plotting each character  */

    textlen = 0.0;
    nc = strlen (text);

    if (nc < 1) {
        return textlen;
    }

    for (i=0; i<nc; i++) {

        if (isspace(text[i])) {
            font_obj.gpf_CharWidth (text[i], size, &charwidth);
            textlen += charwidth;
            continue;
        }

        istat = font_obj.gpf_getfontstrokes (text[i], size, Fontx, Fonty, parts, &nparts,
                                    MAXFONT, MAXPARTS);
        if (!istat) {
            continue;
        }

        n = 0;
        for (j=0; j<nparts; j++) {
            n += parts[j];
        }

        xt = 0.0f;
        for (j=0; j<n; j++) {
            if (Fontx[j] > xt) xt = Fontx[j];
        }

        if (i < nc-1) {
            font_obj.gpf_CharWidth (text[i], size, &charwidth);
            textlen += charwidth;
        }
        else {
            textlen += xt;
        }

    }

    return textlen;

}  /*  end of private stroke_text_length function  */


/*
  ****************************************************************

                F o r m a t A x i s N u m b e r

  ****************************************************************

  Encode a linear or logarithmic number into a text buffer for
  subsequent use in a graph text structure.  This is used for
  axis labels.

*/

int GPFGraph::FormatAxisNumber (CSW_F value, CSW_F logbase, char *buf)
{
    char         fmt[20], fmt2[10];
    int          ndec, ilog;
    CSW_F        fval, delta, amajor;
    double       dvalue;

/*
    calculate a time format if needed
*/
    if (AxisFormatFlag == 1) {
        if (AxisTimeFormat != CSW_NO_TIME_FORMAT) {
            timeutil_obj.csw_SetTimeFormat (AxisTimeFormat, AxisMonthFlag);
            timeutil_obj.csw_FormatTime ((int)value, buf);
            return 1;
        }
    }

    delta = AxisMaxValue - AxisMinValue;
    if (delta < 0.0) delta = -delta;
    delta /= 10.0;
    amajor = AxisMajorInterval;
    if (amajor < 0.0) amajor = -amajor;
    if (amajor > 0.0  &&  amajor < 1.e19) {
        delta = amajor;
    }

    ndec = NumDec;
    fval = value;
    if (delta < 1.e20) {
        fval = delta;
    }

    if (fval < 0.0) fval = -fval;
    if (ndec > 6) {
        if (fval == 0.0) {
            ilog = 1;
        }
        else {
            ilog = (int)log10 ((double)fval);
        }
        ilog--;
        if (fval < 0.1) {
            ndec = -ilog;
        }
        else if (fval < 1.0) {
            ndec = 1;
        }
        else {
            ndec = 0;
        }
        if (ndec > 7) ndec = 7;
    }

    if (logbase > MIN_LOG_BASE) {
        value = (CSW_F)pow ((double)logbase, (double)value);
        strcpy (fmt, "%8.6g");
    }
    else {
        strcpy (fmt, "%12.");
        sprintf (fmt2, "%d", ndec);
        strcat (fmt, fmt2);
        if (ndec >= 6) {
            strcat (fmt, "g");
        }
        else {
            strcat (fmt, "f");
        }
    }

    dvalue = (double)value;
    sprintf (buf, fmt, dvalue);

    csw_StrLeftJust (buf);

    if (buf[0] == '0') {
        if (buf[1] != '\0') {
            buf[0] = ' ';
            csw_StrLeftJust (buf);
        }
    }

    if (!strcmp (buf, "-0")) {
        strcpy (buf, "0");
    }

    return 1;

}  /*  end of private FormatAxisNumber function  */



/*
 *****************************************************************

             C a l c A x i s T e x t A n c h o r

 *****************************************************************

  Return the anchor and dimensions of a rotated axis label.

*/

void GPFGraph::CalcAxisTextAnchor (
    CSW_F xanc,
    CSW_F yanc,
    int   ianchor,
    CSW_F tlen,
    CSW_F tsize,
    CSW_F angle,
    CSW_F *xt,
    CSW_F *yt,
    CSW_F *width,
    CSW_F *height
)
{
    CSW_F     cx[5], cy[5];
    CSW_F     xmin, xmax, ymin, ymax;
    int       i;

    calcdraw_obj.gpf_find_box_corners (
        xanc,
        yanc,
        ianchor,
        tlen,
        tsize,
        angle,
        cx,
        cy);

    *xt = cx[0];
    *yt = cy[0];

    xmin = 1.e30f;
    ymin = 1.e30f;
    xmax = -1.e30f;
    ymax = -1.e30f;

    for (i=0; i<4; i++) {
        if (cx[i] < xmin) xmin = cx[i];
        if (cy[i] < ymin) ymin = cy[i];
        if (cx[i] > xmax) xmax = cx[i];
        if (cy[i] > ymax) ymax = cy[i];
    }

    *width = xmax - xmin;
    *height = ymax - ymin;

    return;

}

