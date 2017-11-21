
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/




/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grparm1.h.
#endif


#ifndef GRPARM1_H
#define GRPARM1_H

#include  "csw/jeasyx/include/gtx_graph.h"

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





class GPFGrparm1
{

  protected:

       int         AxisLabelFont            = GPF_DEFAULT_AXIS_LABEL_FONT,
                   AxisLabelColor           = GPF_DEFAULT_AXIS_LABEL_COLOR,
                   AxisLabelFillColor       = GPF_DEFAULT_AXIS_LABEL_COLOR,
                   AxisLabelAnchor          = -1,
                   AxisLineColor            = GPF_DEFAULT_AXIS_LINE_COLOR,
                   AxisCaptionColor         = GPF_DEFAULT_AXIS_CAPTION_COLOR,
                   AxisCaptionFillColor     = GPF_DEFAULT_AXIS_CAPTION_COLOR,
                   AxisCaptionFont          = GPF_DEFAULT_AXIS_CAPTION_FONT,
                   AxisMinorGridPattern     = GPF_DEFAULT_AXIS_GRID_PATTERN,
                   AxisMinorGridColor       = GPF_DEFAULT_AXIS_GRID_COLOR,
                   AxisMajorGridPattern     = GPF_DEFAULT_AXIS_GRID_PATTERN,
                   AxisMajorGridColor       = GPF_DEFAULT_AXIS_GRID_COLOR,
                   AxisOverflowFlag         = GTX_AXIS_CLIP,
                   AxisDirectionFlag        = GTX_AXIS_MIN_FIRST,
                   AxisTimeFormat           = GTX_NO_TIME_FORMAT,
                   AxisBaseTime             = 0,
                   AxisMonthFlag            = GTX_USE_MONTH_NAME,
                   GraphTitleFont           = GPF_DEFAULT_GRAPH_TITLE_FONT,
                   GraphTitleColor          = GPF_DEFAULT_GRAPH_TITLE_COLOR,
                   GraphTitleFillColor      = GPF_DEFAULT_GRAPH_TITLE_COLOR,
                   GraphTitlePos            = GPF_DEFAULT_GRAPH_TITLE_POS,
                   LegendLabelFont          = GPF_DEFAULT_LEGEND_LABEL_FONT,
                   LegendLabelColor         = GPF_DEFAULT_LEGEND_LABEL_COLOR,
                   LegendLabelFillColor     = GPF_DEFAULT_LEGEND_LABEL_FILL_COLOR,
                   LegendPosition           = GPF_DEFAULT_LEGEND_POSITION,
                   LegendBorderColor        = GPF_DEFAULT_LEGEND_BORDER_COLOR,
                   CurveSmoothFlag          = GPF_DEFAULT_CURVE_SMOOTH_FLAG,
                   CurveEbarColor           = GPF_DEFAULT_CURVE_EBAR_COLOR,
                   CurveSymbolColor         = GPF_DEFAULT_CURVE_SYMBOL_COLOR,
                   CurveSymbolSpacing       = GPF_DEFAULT_CURVE_SYMBOL_SPACING,
                   CurveSymbolMask          = GPF_DEFAULT_CURVE_SYMBOL_MASK,
                   CurveSymbol              = GPF_DEFAULT_CURVE_SYMBOL,
                   CurvePattern             = GPF_DEFAULT_CURVE_PATTERN,
                   CurveColor               = GPF_DEFAULT_CURVE_COLOR,
                   CurveFillPattern         = GPF_DEFAULT_CURVE_FILL_PATTERN,
                   CurveFillColor           = GPF_DEFAULT_CURVE_FILL_COLOR,
                   CurveStyle               = GPF_DEFAULT_CURVE_STYLE,
                   CurveShadowColor         = -1,
                   CurveBarStack            = GTX_STACK_HORIZONTAL,
                   CurveLabelSpacing        = GPF_DEFAULT_CURVE_LABEL_SPACING,
                   CurveLabelColor          = GPF_DEFAULT_CURVE_LABEL_COLOR,
                   CurveLabelFillColor      = GPF_DEFAULT_CURVE_LABEL_COLOR,
                   CurveLabelFont           = GPF_DEFAULT_CURVE_LABEL_FONT,
                   CurveBaseColor           = GPF_DEFAULT_CURVE_BASE_COLOR,
                   CurvePatternColor        = GPF_DEFAULT_CURVE_PATTERN_COLOR,
                   CurveBasePattern         = GPF_DEFAULT_CURVE_BASE_PATTERN,
                   PieDirection             = GPF_PIE_CLOCKWISE,
                   PieStartAngle            = GPF_PIE_DEFAULT_START_ANGLE,
                   PieSortFlag              = GPF_PIE_SORT_DESCENDING,
                   PieInsideLabels          = GPF_PIE_LABEL_NONE,
                   PieEdgeLabels            = GPF_PIE_LABEL_VALUES,
                   PieLegendLabels          = GPF_PIE_LABEL_LABELS;

       CSW_F       AxisLabelSize            = GPF_DEFAULT_AXIS_LABEL_SIZE,
                   AxisLabelGap             = GPF_DEFAULT_AXIS_LABEL_GAP,
                   AxisLabelAngle           = 0.0f,
                   AxisCaptionSize          = GPF_DEFAULT_AXIS_CAPTION_SIZE,
                   AxisCaptionGap           = GPF_DEFAULT_AXIS_CAPTION_GAP,
                   AxisMajorInterval        = GPF_DEFAULT_AXIS_MAJOR_INTERVAL,
                   AxisMinorInterval        = GPF_DEFAULT_AXIS_MINOR_INTERVAL,
                   AxisMinValue             = GPF_DEFAULT_AXIS_MIN_VALUE,
                   AxisMaxValue             = GPF_DEFAULT_AXIS_MAX_VALUE,
                   AxisMinorLength          = GPF_DEFAULT_AXIS_MINOR_LENGTH,
                   AxisMajorLength          = GPF_DEFAULT_AXIS_MAJOR_LENGTH,
                   AxisMinorThick           = GPF_DEFAULT_AXIS_MINOR_THICK,
                   AxisMajorThick           = GPF_DEFAULT_AXIS_MAJOR_THICK,
                   AxisMajorGridThick       = GPF_DEFAULT_AXIS_MAJOR_THICK / 2,
                   AxisMinorGridThick       = GPF_DEFAULT_AXIS_MINOR_THICK / 2,
                   AxisMajorGridDashscale   = GPF_DEFAULT_DASHSCALE,
                   AxisMinorGridDashscale   = GPF_DEFAULT_DASHSCALE,
                   AxisLineThick            = GPF_DEFAULT_AXIS_MAJOR_THICK,
                   AxisLabelThick           = GPF_DEFAULT_AXIS_LABEL_THICK,
                   AxisCaptionThick         = GPF_DEFAULT_AXIS_CAPTION_THICK,
                   GraphTitleSize           = GPF_DEFAULT_GRAPH_TITLE_SIZE,
                   GraphTitleThick          = GPF_DEFAULT_GRAPH_TITLE_THICK,
                   GraphTitleGap            = GPF_DEFAULT_GRAPH_TITLE_GAP,
                   LegendLabelSize          = GPF_DEFAULT_LEGEND_LABEL_SIZE,
                   LegendLabelThick         = GPF_DEFAULT_LEGEND_LABEL_THICK,
                   LegendBorderThick        = GPF_DEFAULT_LEGEND_BORDER_THICK,
                   CurveThick               = GPF_DEFAULT_CURVE_THICK,
                   CurveEbarThick           = GPF_DEFAULT_CURVE_EBAR_THICK,
                   CurveEbarWidth           = GPF_DEFAULT_CURVE_EBAR_WIDTH,
                   CurveSymbolSize          = GPF_DEFAULT_CURVE_SYMBOL_SIZE,
                   CurveSymbolThick         = GPF_DEFAULT_CURVE_THICK / 2.0,
                   CurveFillPatternSize     = GPF_DEFAULT_CURVE_FILL_SIZE,
                   CurveLabelSize           = GPF_DEFAULT_CURVE_LABEL_SIZE,
                   CurveLabelThick          = GPF_DEFAULT_CURVE_EBAR_THICK,
                   CurveBaseValue           = GPF_DEFAULT_CURVE_BASE_VALUE,
                   CurveBaseThick           = GPF_DEFAULT_CURVE_BASE_THICK,
                   CurveDashscale           = GPF_DEFAULT_DASHSCALE,
                   CurveBaseDashscale       = GPF_DEFAULT_DASHSCALE,
                   BarWidthPct              = GPF_DEFAULT_BAR_WIDTH_PCT,
                   BarGapPct                = GPF_DEFAULT_BAR_GAP_PCT;

};

#endif
