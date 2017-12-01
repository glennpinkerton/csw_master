
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_graphP.h

    Define the GPFGraph class.  This refactors the old gpf_graph.c
    functions.

    This function defines constants and structures needed to
    interface with the functions in gpf_graph.c.  This is a private
    header file, not intended for use with application programs.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_graphP.h.
#endif



/*
  add nothing above this ifdef
*/
#ifndef GPF_GRAPHP_H
#define GPF_GRAPHP_H

#include "csw/utils/include/csw_errnum.h"
#include "csw/utils/include/csw_timer.h"
#include "csw/utils/include/csw_.h"

#include "csw/jeasyx/include/gtx_graph.h"
#include "csw/jeasyx/private_include/GraphBase.h"

#include "csw/utils/private_include/gpf_font.h"
#include "csw/utils/private_include/gpf_calcdraw.h"
#include "csw/utils/private_include/csw_time.h"


/*
    define constants used only in this file
    These came heree from the old gpf_graph.c file.
*/
#define MIN_LOG_BASE        1.01f
#define MIN_MINOR_LOG_BASE  2.99f
#define NUM_EBAR_PTS        6
#define MINOR_GRID_THICK    AxisMinorGridThick
#define MAJOR_GRID_THICK    AxisMajorGridThick

#define NEAR_ZERO           1.e-10f

#define LEGEND_LINE         1
#define LEGEND_FILL         2
#define LEGEND_SYMB         3
#define LEGEND_POINT        4

#define LABEL_LENGTH        30
#define MIN_LABEL_GAP       (0.05f * Units)
#define MAX_LABEL_GAP       (0.5f * Units)

#define SHADOW_FRACTION     0.5f
#define GAP_FRACTION        0.85f

#define MAX_LEGEND_POSITION 4

#define LEGEND_ONLY_STRING  ".only."
#define LEGEND_ONLY_LEN     6

#define MAXFONT 1000
#define MAXPARTS 100

/*
    define tags for graph attributes
*/
#define GPF_AXIS_LABEL_ANCHOR         GTX_AXIS_LABEL_ANCHOR
#define GPF_AXIS_LABEL_FONT           GTX_AXIS_LABEL_FONT
#define GPF_AXIS_LABEL_COLOR          GTX_AXIS_LABEL_COLOR
#define GPF_AXIS_LABEL_FILL_COLOR     GTX_AXIS_LABEL_FILL_COLOR
#define GPF_AXIS_LINE_COLOR           GTX_AXIS_LINE_COLOR
#define GPF_AXIS_CAPTION_COLOR        GTX_AXIS_CAPTION_COLOR
#define GPF_AXIS_CAPTION_FILL_COLOR   GTX_AXIS_CAPTION_FILL_COLOR
#define GPF_AXIS_CAPTION_FONT         GTX_AXIS_CAPTION_FONT
#define GPF_AXIS_OVERFLOW_FLAG        GTX_AXIS_OVERFLOW_FLAG
#define GPF_AXIS_DIRECTION_FLAG       GTX_AXIS_DIRECTION_FLAG
#define GPF_AXIS_TIME_FORMAT          GTX_AXIS_TIME_FORMAT
#define GPF_AXIS_BASE_TIME            GTX_AXIS_BASE_TIME
#define GPF_AXIS_MONTH_FLAG           GTX_AXIS_MONTH_FLAG

#define GPF_AXIS_LABEL_ANGLE          GTX_AXIS_LABEL_ANGLE
#define GPF_AXIS_LABEL_SIZE           GTX_AXIS_LABEL_SIZE
#define GPF_AXIS_LABEL_GAP            GTX_AXIS_LABEL_GAP
#define GPF_AXIS_CAPTION_SIZE         GTX_AXIS_CAPTION_SIZE
#define GPF_AXIS_CAPTION_GAP          GTX_AXIS_CAPTION_GAP
#define GPF_AXIS_MINOR_LENGTH         GTX_AXIS_MINOR_LENGTH
#define GPF_AXIS_MAJOR_LENGTH         GTX_AXIS_MAJOR_LENGTH
#define GPF_AXIS_MINOR_THICK          GTX_AXIS_MINOR_THICK
#define GPF_AXIS_MAJOR_THICK          GTX_AXIS_MAJOR_THICK
#define GPF_AXIS_LABEL_THICK          GTX_AXIS_LABEL_THICK
#define GPF_AXIS_CAPTION_THICK        GTX_AXIS_CAPTION_THICK
#define GPF_AXIS_MINOR_INTERVAL       GTX_AXIS_MINOR_INTERVAL
#define GPF_AXIS_MAJOR_INTERVAL       GTX_AXIS_MAJOR_INTERVAL
#define GPF_AXIS_MIN_VALUE            GTX_AXIS_MIN_VALUE     
#define GPF_AXIS_MAX_VALUE            GTX_AXIS_MAX_VALUE     
#define GPF_AXIS_MAJOR_GRID_PATTERN   GTX_AXIS_MAJOR_GRID_PATTERN
#define GPF_AXIS_MAJOR_GRID_COLOR     GTX_AXIS_MAJOR_GRID_COLOR
#define GPF_AXIS_MINOR_GRID_PATTERN   GTX_AXIS_MINOR_GRID_PATTERN
#define GPF_AXIS_MINOR_GRID_COLOR     GTX_AXIS_MINOR_GRID_COLOR
#define GPF_AXIS_MAJOR_GRID_THICK     GTX_AXIS_MAJOR_GRID_THICK
#define GPF_AXIS_MINOR_GRID_THICK     GTX_AXIS_MINOR_GRID_THICK
#define GPF_AXIS_LINE_THICK           GTX_AXIS_LINE_THICK
#define GPF_AXIS_MAJOR_GRID_DASHSCALE GTX_AXIS_MAJOR_GRID_DASHSCALE
#define GPF_AXIS_MINOR_GRID_DASHSCALE GTX_AXIS_MINOR_GRID_DASHSCALE

#define GPF_GRAPH_TITLE_SIZE          GTX_GRAPH_TITLE_SIZE
#define GPF_GRAPH_TITLE_FONT          GTX_GRAPH_TITLE_FONT
#define GPF_GRAPH_TITLE_THICK         GTX_GRAPH_TITLE_THICK
#define GPF_GRAPH_TITLE_COLOR         GTX_GRAPH_TITLE_COLOR
#define GPF_GRAPH_TITLE_FILL_COLOR    GTX_GRAPH_TITLE_FILL_COLOR
#define GPF_GRAPH_TITLE_POS           GTX_GRAPH_TITLE_POS  
#define GPF_GRAPH_TITLE_GAP           GTX_GRAPH_TITLE_GAP  

#define GPF_LEGEND_LABEL_COLOR        GTX_LEGEND_LABEL_COLOR
#define GPF_LEGEND_LABEL_FILL_COLOR   GTX_LEGEND_LABEL_FILL_COLOR
#define GPF_LEGEND_LABEL_SIZE         GTX_LEGEND_LABEL_SIZE
#define GPF_LEGEND_LABEL_FONT         GTX_LEGEND_LABEL_FONT
#define GPF_LEGEND_LABEL_THICK        GTX_LEGEND_LABEL_THICK
#define GPF_LEGEND_POSITION           GTX_LEGEND_POSITION
#define GPF_LEGEND_BORDER_COLOR       GTX_LEGEND_BORDER_COLOR
#define GPF_LEGEND_BORDER_THICK       GTX_LEGEND_BORDER_THICK

#define GPF_CURVE_SYMBOL_SIZE         GTX_CURVE_SYMBOL_SIZE
#define GPF_CURVE_THICK               GTX_CURVE_THICK
#define GPF_CURVE_COLOR               GTX_CURVE_COLOR
#define GPF_CURVE_PATTERN             GTX_CURVE_PATTERN
#define GPF_CURVE_SYMBOL              GTX_CURVE_SYMBOL
#define GPF_CURVE_DASHSCALE           GTX_CURVE_DASHSCALE
#define GPF_CURVE_SYMBOL_COLOR        GTX_CURVE_SYMBOL_COLOR
#define GPF_CURVE_SYMBOL_SPACING      GTX_CURVE_SYMBOL_SPACING
#define GPF_CURVE_SYMBOL_MASK         GTX_CURVE_SYMBOL_MASK
#define GPF_CURVE_FILL_PATTERN        GTX_CURVE_FILL_PATTERN
#define GPF_CURVE_FILL_COLOR          GTX_CURVE_FILL_COLOR
#define GPF_CURVE_EBAR_COLOR          GTX_CURVE_EBAR_COLOR
#define GPF_CURVE_EBAR_THICK          GTX_CURVE_EBAR_THICK
#define GPF_CURVE_EBAR_WIDTH          GTX_CURVE_EBAR_WIDTH
#define GPF_CURVE_SYMBOL_THICK        GTX_CURVE_SYMBOL_THICK
#define GPF_CURVE_SYMBOL_FLAG         GTX_CURVE_SYMBOL_FLAG
#define GPF_CURVE_SMOOTH_FLAG         GTX_CURVE_SMOOTH_FLAG
#define GPF_CURVE_STYLE               GTX_CURVE_STYLE
#define GPF_CURVE_LABEL_SPACING       GTX_CURVE_LABEL_SPACING
#define GPF_CURVE_LABEL_COLOR         GTX_CURVE_LABEL_COLOR
#define GPF_CURVE_LABEL_FILL_COLOR    GTX_CURVE_LABEL_FILL_COLOR
#define GPF_CURVE_LABEL_FONT          GTX_CURVE_LABEL_FONT 
#define GPF_CURVE_LABEL_SIZE          GTX_CURVE_LABEL_SIZE
#define GPF_CURVE_LABEL_THICK         GTX_CURVE_LABEL_THICK
#define GPF_CURVE_BASE_VALUE          GTX_CURVE_BASE_VALUE
#define GPF_CURVE_BASE_COLOR          GTX_CURVE_BASE_COLOR
#define GPF_CURVE_BASE_PATTERN        GTX_CURVE_BASE_PATTERN
#define GPF_CURVE_BASE_THICK          GTX_CURVE_BASE_THICK
#define GPF_CURVE_BASE_DASHSCALE      GTX_CURVE_BASE_DASHSCALE
#define GPF_BAR_SHADOW_COLOR          GTX_BAR_SHADOW_COLOR
#define GPF_BAR_STACK                 GTX_BAR_STACK
#define GPF_CURVE_PATTERN_COLOR       GTX_CURVE_PATTERN_COLOR
#define GPF_BAR_WIDTH_PCT             GTX_BAR_WIDTH_PCT
#define GPF_BAR_GAP_PCT               GTX_BAR_GAP_PCT
#define GPF_CURVE_FILL_SIZE           GTX_CURVE_FILL_SIZE

#define GPF_TOP_AXIS                  GTX_TOP_AXIS
#define GPF_BOTTOM_AXIS               GTX_BOTTOM_AXIS
#define GPF_LEFT_AXIS                 GTX_LEFT_AXIS
#define GPF_RIGHT_AXIS                GTX_RIGHT_AXIS

#define GPF_PIE_CLOCKWISE             GTX_PIE_CLOCKWISE
#define GPF_PIE_COUNTER_CLOCKWISE     GTX_PIE_COUNTER_CLOCKWISE

#define GPF_PIE_SORT_ASCENDING        GTX_PIE_SORT_ASCENDING
#define GPF_PIE_SORT_DESCENDING       GTX_PIE_SORT_DESCENDING
#define GPF_PIE_UNSORTED              GTX_PIE_UNSORTED

#define GPF_PIE_LABEL_LABELS          GTX_PIE_LABEL_LABELS
#define GPF_PIE_LABEL_VALUES          GTX_PIE_LABEL_VALUES
#define GPF_PIE_LABEL_ALL             GTX_PIE_LABEL_ALL
#define GPF_PIE_LABEL_NONE            GTX_PIE_LABEL_NONE


/*
    define default attribute value constants
*/
#define GPF_DEFAULT_AXIS_LABEL_FONT      0
#define GPF_DEFAULT_AXIS_LABEL_COLOR     1
#define GPF_DEFAULT_AXIS_LINE_COLOR      1
#define GPF_DEFAULT_AXIS_CAPTION_COLOR   1
#define GPF_DEFAULT_AXIS_CAPTION_FONT    0
#define GPF_DEFAULT_AXIS_GRID_PATTERN    0
#define GPF_DEFAULT_AXIS_GRID_COLOR      0
#define GPF_DEFAULT_AXIS_LABEL_ANCHOR    -1

#define GPF_DEFAULT_AXIS_LABEL_ANGLE     0.0f
#define GPF_DEFAULT_AXIS_LABEL_SIZE      0.07f
#define GPF_DEFAULT_AXIS_LABEL_GAP       0.04f
#define GPF_DEFAULT_AXIS_CAPTION_SIZE    0.10f
#define GPF_DEFAULT_AXIS_CAPTION_GAP     0.05f
#define GPF_DEFAULT_AXIS_MINOR_LENGTH    0.04f
#define GPF_DEFAULT_AXIS_MAJOR_LENGTH    0.08f
#define GPF_DEFAULT_AXIS_MINOR_THICK     0.005f
#define GPF_DEFAULT_AXIS_MAJOR_THICK     0.02f
#define GPF_DEFAULT_AXIS_LABEL_THICK     0.005f
#define GPF_DEFAULT_AXIS_CAPTION_THICK   0.005f
#define GPF_DEFAULT_AXIS_MAJOR_INTERVAL  -1.0e20f
#define GPF_DEFAULT_AXIS_MINOR_INTERVAL  -1.0e20f
#define GPF_DEFAULT_AXIS_MAX_VALUE       -1.0e20f
#define GPF_DEFAULT_AXIS_MIN_VALUE       1.0e20f

#define GPF_DEFAULT_DASHSCALE            1.0f

#define GPF_DEFAULT_GRAPH_TITLE_SIZE     0.2f
#define GPF_DEFAULT_GRAPH_TITLE_FONT     0
#define GPF_DEFAULT_GRAPH_TITLE_THICK    0.01f
#define GPF_DEFAULT_GRAPH_TITLE_COLOR    1
#define GPF_DEFAULT_GRAPH_TITLE_POS      GTX_TOP_CENTER_TITLE
#define GPF_DEFAULT_GRAPH_TITLE_GAP      0.2f

#define GPF_DEFAULT_LEGEND_LABEL_COLOR   1
#define GPF_DEFAULT_LEGEND_LABEL_FILL_COLOR   0
#define GPF_DEFAULT_LEGEND_LABEL_SIZE    0.07f
#define GPF_DEFAULT_LEGEND_LABEL_FONT    0
#define GPF_DEFAULT_LEGEND_LABEL_THICK   0.005f
#define GPF_DEFAULT_LEGEND_POSITION      GTX_LEGEND_ON_RIGHT
#define GPF_DEFAULT_LEGEND_BORDER_COLOR  1
#define GPF_DEFAULT_LEGEND_BORDER_THICK  .01f

#define GPF_DEFAULT_CURVE_SYMBOL_SIZE    .07f
#define GPF_DEFAULT_CURVE_THICK          .01f
#define GPF_DEFAULT_CURVE_SYMBOL_THICK   .005f
#define GPF_DEFAULT_CURVE_COLOR          1
#define GPF_DEFAULT_CURVE_PATTERN        0
#define GPF_DEFAULT_CURVE_FILL_SIZE      1.0f
#define GPF_DEFAULT_CURVE_SYMBOL         0
#define GPF_DEFAULT_CURVE_SYMBOL_COLOR   1
#define GPF_DEFAULT_CURVE_SYMBOL_SPACING 1
#define GPF_DEFAULT_CURVE_SYMBOL_MASK    1
#define GPF_DEFAULT_CURVE_FILL_PATTERN   0
#define GPF_DEFAULT_CURVE_FILL_COLOR     0
#define GPF_DEFAULT_CURVE_EBAR_COLOR     1
#define GPF_DEFAULT_CURVE_EBAR_THICK     .005f
#define GPF_DEFAULT_CURVE_EBAR_WIDTH     .1f
#define GPF_DEFAULT_CURVE_SMOOTH_FLAG    0
#define GPF_DEFAULT_CURVE_STYLE          GTX_CONNECT_POINTS
#define GPF_DEFAULT_CURVE_LABEL_SPACING  0
#define GPF_DEFAULT_CURVE_LABEL_COLOR    1
#define GPF_DEFAULT_CURVE_LABEL_FONT     0
#define GPF_DEFAULT_CURVE_LABEL_SIZE     0.07f
#define GPF_DEFAULT_CURVE_BASE_VALUE     0.0f
#define GPF_DEFAULT_CURVE_BASE_COLOR     1
#define GPF_DEFAULT_CURVE_PATTERN_COLOR  1
#define GPF_DEFAULT_CURVE_BASE_PATTERN   0
#define GPF_DEFAULT_CURVE_BASE_THICK     0.005f
#define GPF_DEFAULT_BAR_WIDTH_PCT        .6f
#define GPF_DEFAULT_BAR_GAP_PCT          .4f

#define GPF_PIE_DEFAULT_START_ANGLE      90

#define MAX_PARAM_STRING                 2000

#define MAX_LEG_TEXT                     500


/*
    define structures
*/
typedef struct {
    int       symnum,
              mask,
              color;
    CSW_F     x,
              y,
              thick,
              size;
}  GRaphSymRec;

typedef struct {
    char      *text = NULL;
    CSW_F     x,
              y,
              size,
              thick,
              angle;
    int       nchar,
              fillcolor,
              color,
              font;
}  GRaphTextRec;

typedef struct {
    int       smoothflag;
    int       fillpat,
              fillcolor,
              pattcolor,
              linecolor,
              outline;
    CSW_F     *x = NULL,
              *y = NULL;
    CSW_F     linethick,
              patsize;
    int       npts;
}  GRaphFillRec;

typedef struct {
    int       fillpat,
              fillcolor,
              pattcolor,
              linecolor,
              outline,
              type;
    CSW_F     ang1,
              ang_width,
              xc,
              yc,
              rotang;
    CSW_F     linethick,
              patsize;
}  GRaphArcRec;

typedef struct {
    int       smoothflag,
              layer,
              item,
              frame;
    int       bgflag;
    int       linepat,
              linecolor;
    CSW_F     *x = NULL,
              *y = NULL,
              dashscale,
              linethick;
    CSW_F     x1,
              y1,
              x2,
              y2;
    int       npts;
}  GRaphLineRec;

typedef union {
    CSW_F     fval;
    int       ival;
    char      *cval = NULL;
}  GRaphUnion;

typedef struct {
    CSW_F        height,
                 size,
                 fillsize,
                 dashscale,
                 thick;
    CSW_F        textsize,
                 textthick;
    int          textcolor,
                 textfillcolor,
                 textfont;
    int          type,
                 nlines,
                 linecolor,
                 fillcolor,
                 pattnum,
                 pattcolor,
                 symbol;
    char         text[MAX_LEG_TEXT];
    void         *prev = NULL, *next = NULL;
    int          prevoffset, nextoffset;
    void         *element = NULL;
    int          element_type;
}  LEgendRec;




class GPFGraph  :  public GraphBase
{

  private:

    GPFCalcdraw      calcdraw_obj;
    CSWTimeUtil      timeutil_obj;
    GPFFont          font_obj;

// Old static file variables become private class variables.
    CSW_F       AxisMarginSize = 0.0f;

    CSW_F       AxisTickGap = 0.0f;

    CSW_F       PageX1 = 0.0,
                PageY1 = 0.0,
                PageX2 = 0.0,
                PageY2 = 0.0;

    CSW_F       PageX11 = 0.0,
                PageY11 = 0.0,
                PageX22 = 0.0,
                PageY22 = 0.0;

    CSW_F       GXmin = 0.0,
                GYmin = 0.0,
                GXmax = 0.0,
                GYmax = 0.0;

    CSW_F       AXmin = 0.0,
                AYmin = 0.0,
                AXmax = 0.0,
                AYmax = 0.0;

    CSW_F       ClipX1,
                ClipY1,
                ClipX2,
                ClipY2,
                Xwide = 1000.,
                Ywide = 1000.;

    CSW_F       *Xclip = NULL,
                *Yclip = NULL;
    int         Nclip = 0;

    CSW_F       Units = 1.0;

    LEgendRec   *LegendList = NULL;
    int         Nlegend = 0;
    int         *FreeLegend = NULL;
    int         NfreeLegend = 0;
    LEgendRec   *LastLegPtr = NULL,
                *FirstLegPtr = NULL;
    int         ReportedLastLegPtr = -1;
    int         FirstLegOffset = 0,
                LastLegOffset = 0;
    CSW_F       LegendBorderWidth = 0.0;

    CSW_F       LegendY1,
                LegendY2,
                LegendX1;
    int         LegendAnchor;

    CSW_F       AxisLogBase = 0.0;
    int         AxisFormatFlag = 0;

    CSW_F       BarPointBases[GTX_MAX_AXIS_LABELS];
    CSW_F       BarMajor,
                BarAxmin,
                OrigBarWidthPct,
                OrigBarGapPct;
    int         NbarSets,
                TotalBarSets;

    int         NumDec = 7;

    int         InitialGraphBarFlag = 0;

    int         LegendTitleBorderFlag = 0,
                SkipLegendFlag = 0,
                DrawAllLegendFlag = 0,
                AxisLimitsFlag = 0,
                AutoLegendAnchor = 0;

    CSW_F       AutoLegendX1 = 0.0;

    CSW_F       Fontx[MAXFONT],
                Fonty[MAXFONT];

    CSW_F       PageUnitsPerInch = 1.0;

/*
    Old static file functions become private class methods.
*/
    int         FormatAxisNumber (CSW_F, CSW_F, char *);
    int         FormatNumber (CSW_F, CSW_F, char *);
    void        CleanupNines (char *);
    void        CleanupZeros (char *);
    int         ClipLine (CSW_F *, CSW_F *, int);
    int         AddToLegend (int, int, int, int, int, int, const char*);
    int         MultiLineTextPrims (GRaphTextRec **, const char *, int *, int,
                                       CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F,
                                       int, int, int);
    int         RotateEbar (CSW_F *, CSW_F *, int, CSW_F, CSW_F);
    int         LabelLocation (GRaphTextRec *, int, CSW_F);
    int         BarLabelLocation (GRaphTextRec *, CSW_F, CSW_F, CSW_F, CSW_F,
                                     int, CSW_F, CSW_F);
    int         UpdateLimits (CSW_F, CSW_F);
    int         SetPointBase (CSW_F, CSW_F);
    CSW_F       GetPointBase (CSW_F);

    int         DrawLegendOnly (int, int, int, int, int, int,
                                   const char *);

    int         RoundOffTime (CSW_F *, CSW_F *, CSW_F);

    CSW_F       exact_text_length (char *text, CSW_F size, int font);
    CSW_F       stroke_text_length (char *text, CSW_F size, int font);

    void        CalcAxisTextAnchor (
                       CSW_F xanc,
                       CSW_F yanc,
                       int   ianchor,
                       CSW_F tlen,
                       CSW_F tsize,
                       CSW_F angle,
                       CSW_F *xt,
                       CSW_F *yt,
                       CSW_F *width,
                       CSW_F *height);



  public:

    int gpf_StartGraph (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                        CSW_F units, int basetime,
                        CSW_F page_units_per_inch);
    int gpf_SetGraphParameter (int tag, GRaphUnion value);
    CSW_F gpf_GetAxisMarginSize (void);
    int gpf_CalcGraphAxis (int position, const char *caption,
                           CSW_F datamin, CSW_F datamax,
                           CSW_F logbase, int labelflag,
                           char **labels, CSW_F *labelpos, int nlabels,
                           CSW_F *axmin, CSW_F *axmax, CSW_F *axmajor,
                           GRaphLineRec **lines, int *nlines,
                           GRaphTextRec **text, int *ntext);
    int gpf_CalcAxisIntervals (CSW_F datamin, CSW_F datamax, int nmajor,
                               CSW_F *axismin, CSW_F *axismax,
                               CSW_F *axismajor, CSW_F *axisminor);
    int gpf_FreeGraphLines (GRaphLineRec *lines, int nlines, int flag);
    int gpf_FreeGraphFills (GRaphFillRec *fills, int nfills);
    int gpf_FreeGraphArcs (GRaphArcRec *arcs, int narcs);
    int gpf_FreeGraphText (GRaphTextRec *text, int ntext);
    int gpf_CalcGraphCurve (CSW_F *x, CSW_F *y, CSW_F *ebar, int npts,
                            const char *legtext, int vflag,
                            CSW_F *x2, CSW_F *y2, int npts2,
                            CSW_F axmin, CSW_F axmax, CSW_F xlog,
                            CSW_F aymin, CSW_F aymax, CSW_F ylog,
                            GRaphSymRec **syms, int *nsyms,
                            GRaphLineRec **lines, int *nlines,
                            GRaphTextRec **text, int *ntext,
                            GRaphLineRec **baseline);
    int gpf_SetGraphDefaults (void);
    int gpf_set_graph_drawing_limits (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int gpf_get_graph_drawing_limits (CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gpf_get_axis_drawing_limits (CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gpf_CalcGraphTitle (const char *title,
                            GRaphTextRec **text, int *ntext);
    int gpf_SetGraphClipFlags (int xflag, int yflag);
    int gpf_get_last_leg_ptr (int *ptr);
    int gpf_set_leg_list (void *leglist, int nleg, int *freelist,
                          int nfree, int firstptr, int lastptr,
                          CSW_F y1, CSW_F y2, CSW_F x1, int anchor, CSW_F bwidth);
    int gpf_get_leg_list (void **leglist, int *nleg,
                          int **freelist, int *nfree,
                          int *firstptr, int *lastptr, CSW_F *bwidth);
    int gpf_CalcGraphLegend (const char *legendtitle, int drawtitle,
                             GRaphSymRec **syms, int *numsyms,
                             GRaphLineRec **lines, int *numlines,
                             GRaphFillRec **fills, int *numfills,
                             GRaphTextRec **texts, int *numtexts);
    int gpf_CalcGraphCurveFill (CSWErrNum &err_obj,
                                const char *legtext, int vflag,
                                CSW_F *xtopin, CSW_F *ytopin, int ntopin,
                                CSW_F *xbottomin, CSW_F *ybottomin, int nbottomin,
                                CSW_F axmin, CSW_F axmax, CSW_F xlog,
                                CSW_F aymin, CSW_F aymax, CSW_F ylog,
                                GRaphFillRec **fills, int *nfills);
    int gpf_InitGraphBars (int flag, int nset, CSW_F major,
                           CSW_F axmin, CSW_F axmax, CSW_F xlog);
    int gpf_SetInitialGraphBarFlag (int val);
    int gpf_CalcGraphBars  (CSW_F *x, CSW_F *y, int npts, const char *legtext, int vflag,
                            CSW_F axmin, CSW_F axmax, CSW_F xlog,
                            CSW_F aymin, CSW_F aymax, CSW_F ylog,
                            GRaphLineRec **lines, int *nlines,
                            GRaphTextRec **text, int *ntext,
                            GRaphFillRec **fills, int *nfills,
                            GRaphLineRec **baseline);
    int gpf_set_graph_decimal_places (CSW_F val);
    int gpf_set_legend_title_border_flag (int val);
    int gpf_SetDrawAllLegendFlag (int val);
    int gpf_set_legend_auto_anchor (CSW_F x1, int anchor);
    int gpf_get_legend_auto_anchor (CSW_F *x1, int *anchor);
    int gpf_CalcAutoAxisIntervals (CSW_F datamin, CSW_F datamax, int nmajor,
                                   CSW_F *axismin, CSW_F *axismax,
                                   CSW_F *axismajor, CSW_F *axisminor);
    int gpf_set_skip_legend_flag (int val);
    int gpf_rollback_legend (void);
    int gpf_update_graph_limits (CSW_F, CSW_F, CSW_F, CSW_F);

};

/*
  add nothing below this endif
*/
#endif

