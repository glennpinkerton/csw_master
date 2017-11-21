
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
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
#error Illegal attempt to include private header file grparm3P.h.
#endif




    AxisLabelFont             = GPF_DEFAULT_AXIS_LABEL_FONT;
    AxisLabelColor            = GPF_DEFAULT_AXIS_LABEL_COLOR;
    AxisLabelFillColor        = GPF_DEFAULT_AXIS_LABEL_COLOR;
    AxisLineColor             = GPF_DEFAULT_AXIS_LINE_COLOR;
    AxisCaptionColor          = GPF_DEFAULT_AXIS_CAPTION_COLOR;
    AxisCaptionFillColor      = GPF_DEFAULT_AXIS_CAPTION_COLOR;
    AxisCaptionFont           = GPF_DEFAULT_AXIS_CAPTION_FONT;
    AxisMinorGridPattern      = GPF_DEFAULT_AXIS_GRID_PATTERN;
    AxisMinorGridColor        = GPF_DEFAULT_AXIS_GRID_COLOR;
    AxisMajorGridPattern      = GPF_DEFAULT_AXIS_GRID_PATTERN;
    AxisMajorGridColor        = GPF_DEFAULT_AXIS_GRID_COLOR;
    AxisOverflowFlag          = GTX_AXIS_CLIP;
    AxisDirectionFlag         = GTX_AXIS_MIN_FIRST;
    AxisTimeFormat            = GTX_NO_TIME_FORMAT;
    AxisBaseTime              = 0;
    AxisMonthFlag             = GTX_USE_MONTH_NAME;
    GraphTitleFont            = GPF_DEFAULT_GRAPH_TITLE_FONT;
    GraphTitleColor           = GPF_DEFAULT_GRAPH_TITLE_COLOR;
    GraphTitleFillColor       = GPF_DEFAULT_GRAPH_TITLE_COLOR;
    GraphTitlePos             = GPF_DEFAULT_GRAPH_TITLE_POS;
    LegendLabelFont           = GPF_DEFAULT_LEGEND_LABEL_FONT;
    LegendLabelColor          = GPF_DEFAULT_LEGEND_LABEL_COLOR;
    LegendLabelFillColor      = GPF_DEFAULT_LEGEND_LABEL_FILL_COLOR;
    LegendPosition            = GPF_DEFAULT_LEGEND_POSITION;
    LegendBorderColor         = GPF_DEFAULT_LEGEND_BORDER_COLOR;
    LegendBorderThick         = GPF_DEFAULT_LEGEND_BORDER_THICK;
    CurveSmoothFlag           = GPF_DEFAULT_CURVE_SMOOTH_FLAG;
    CurveEbarColor            = GPF_DEFAULT_CURVE_EBAR_COLOR;
    CurveSymbolColor          = GPF_DEFAULT_CURVE_SYMBOL_COLOR;
    CurveSymbolSpacing        = GPF_DEFAULT_CURVE_SYMBOL_SPACING;
    CurveSymbolMask           = GPF_DEFAULT_CURVE_SYMBOL_MASK;
    CurveSymbol               = GPF_DEFAULT_CURVE_SYMBOL;
    CurvePattern              = GPF_DEFAULT_CURVE_PATTERN;
    CurveColor                = GPF_DEFAULT_CURVE_COLOR;
    CurveFillPattern          = GPF_DEFAULT_CURVE_FILL_PATTERN;
    CurveFillColor            = GPF_DEFAULT_CURVE_FILL_COLOR;
    CurveStyle                = GPF_DEFAULT_CURVE_STYLE;
    CurveShadowColor          = -1;
    CurveBarStack             = GTX_STACK_HORIZONTAL;
    CurveLabelSpacing         = GPF_DEFAULT_CURVE_LABEL_SPACING;
    CurveLabelColor           = GPF_DEFAULT_CURVE_LABEL_COLOR;
    CurveLabelFillColor       = GPF_DEFAULT_CURVE_LABEL_COLOR;
    CurveLabelFont            = GPF_DEFAULT_CURVE_LABEL_FONT;
    CurveBaseColor            = GPF_DEFAULT_CURVE_BASE_COLOR;
    CurvePatternColor         = GPF_DEFAULT_CURVE_PATTERN_COLOR;
    CurveBasePattern          = GPF_DEFAULT_CURVE_BASE_PATTERN;
    PieDirection              = GPF_PIE_CLOCKWISE;
    PieStartAngle             = GPF_PIE_DEFAULT_START_ANGLE;
    PieSortFlag               = GPF_PIE_SORT_DESCENDING;
    PieInsideLabels           = GPF_PIE_LABEL_NONE;
    PieEdgeLabels             = GPF_PIE_LABEL_VALUES;
    PieLegendLabels           = GPF_PIE_LABEL_LABELS;

    AxisLabelSize             = GPF_DEFAULT_AXIS_LABEL_SIZE * Units;
    AxisLabelGap              = GPF_DEFAULT_AXIS_LABEL_GAP * Units;
    AxisCaptionSize           = GPF_DEFAULT_AXIS_CAPTION_SIZE * Units;
    AxisCaptionGap            = GPF_DEFAULT_AXIS_CAPTION_GAP * Units;
    AxisMajorInterval         = GPF_DEFAULT_AXIS_MAJOR_INTERVAL;
    AxisMinorInterval         = GPF_DEFAULT_AXIS_MINOR_INTERVAL;
    AxisMinValue              = GPF_DEFAULT_AXIS_MIN_VALUE;
    AxisMaxValue              = GPF_DEFAULT_AXIS_MAX_VALUE;
    AxisMinorLength           = GPF_DEFAULT_AXIS_MINOR_LENGTH * Units;
    AxisMajorLength           = GPF_DEFAULT_AXIS_MAJOR_LENGTH * Units;
    AxisMinorThick            = GPF_DEFAULT_AXIS_MINOR_THICK * Units;
    AxisMajorThick            = GPF_DEFAULT_AXIS_MAJOR_THICK * Units;
    AxisMajorGridThick        = GPF_DEFAULT_AXIS_MINOR_THICK * Units;
    AxisMinorGridThick        = GPF_DEFAULT_AXIS_MINOR_THICK * Units;
    AxisLineThick             = GPF_DEFAULT_AXIS_MAJOR_THICK * Units;
    AxisLabelThick            = GPF_DEFAULT_AXIS_LABEL_THICK * Units;
    AxisCaptionThick          = GPF_DEFAULT_AXIS_CAPTION_THICK * Units;
    GraphTitleSize            = GPF_DEFAULT_GRAPH_TITLE_SIZE * Units;
    GraphTitleThick           = GPF_DEFAULT_GRAPH_TITLE_THICK * Units;
    GraphTitleGap             = GPF_DEFAULT_GRAPH_TITLE_GAP * Units;
    LegendLabelSize           = GPF_DEFAULT_LEGEND_LABEL_SIZE * Units;
    LegendLabelThick          = GPF_DEFAULT_LEGEND_LABEL_THICK * Units;
    CurveThick                = GPF_DEFAULT_CURVE_THICK * Units;
    CurveEbarThick            = GPF_DEFAULT_CURVE_EBAR_THICK * Units;
    CurveEbarWidth            = GPF_DEFAULT_CURVE_EBAR_WIDTH * Units;
    CurveSymbolSize           = GPF_DEFAULT_CURVE_SYMBOL_SIZE * Units;
    CurveSymbolThick          = GPF_DEFAULT_CURVE_THICK / 2.0f * Units;
    CurveFillPatternSize      = GPF_DEFAULT_CURVE_FILL_SIZE * Units;
    CurveLabelSize            = GPF_DEFAULT_CURVE_LABEL_SIZE * Units;
    CurveLabelThick           = GPF_DEFAULT_CURVE_EBAR_THICK * Units;
    CurveBaseValue            = GPF_DEFAULT_CURVE_BASE_VALUE * Units;
    CurveBaseThick            = GPF_DEFAULT_CURVE_BASE_THICK * Units;
    BarWidthPct               = GPF_DEFAULT_BAR_WIDTH_PCT;
    BarGapPct                 = GPF_DEFAULT_BAR_GAP_PCT;
    CurveDashscale            = GPF_DEFAULT_DASHSCALE;
    CurveBaseDashscale        = GPF_DEFAULT_DASHSCALE;
    AxisMajorGridDashscale    = GPF_DEFAULT_DASHSCALE;
    AxisMinorGridDashscale    = GPF_DEFAULT_DASHSCALE;

