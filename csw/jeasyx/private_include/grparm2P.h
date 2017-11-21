
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
#error Illegal attempt to include private header file grparm2P.h.
#endif




    switch (tag) {

        case GTX_AXIS_LABEL_FONT:
            AxisLabelFont = value.ival;
            break;

        case GTX_AXIS_LABEL_COLOR:
            AxisLabelColor = value.ival;
            break;

        case GTX_AXIS_LABEL_FILL_COLOR:
            AxisLabelFillColor = value.ival;
            break;

        case GTX_AXIS_LINE_COLOR:
            AxisLineColor = value.ival;
            break;

        case GTX_AXIS_CAPTION_COLOR:
            AxisCaptionColor = value.ival;
            break;

        case GTX_AXIS_CAPTION_FILL_COLOR:
            AxisCaptionFillColor = value.ival;
            break;

        case GTX_AXIS_CAPTION_FONT:
            AxisCaptionFont = value.ival;
            break;

        case GTX_AXIS_MAJOR_GRID_PATTERN:
            AxisMajorGridPattern = value.ival;
            break;

        case GTX_AXIS_MINOR_GRID_PATTERN:
            AxisMinorGridPattern = value.ival;
            break;

        case GTX_AXIS_MAJOR_GRID_COLOR:
            AxisMajorGridColor = value.ival;
            break;

        case GTX_AXIS_MINOR_GRID_COLOR:
            AxisMinorGridColor = value.ival;
            break;

        case GTX_AXIS_DIRECTION_FLAG:
            AxisDirectionFlag = value.ival;
            break;

        case GTX_AXIS_TIME_FORMAT:
            AxisTimeFormat = value.ival;
            break;

        case GTX_AXIS_BASE_TIME:
            AxisBaseTime = value.ival;
            break;

        case GTX_AXIS_MONTH_FLAG:
            AxisMonthFlag = value.ival;
            break;

        case GTX_AXIS_OVERFLOW_FLAG:
            AxisOverflowFlag = value.ival;
            break;

        case GTX_GRAPH_TITLE_FONT:
            GraphTitleFont = value.ival;
            break;

        case GTX_GRAPH_TITLE_COLOR:
            GraphTitleColor = value.ival;
            break;

        case GTX_GRAPH_TITLE_FILL_COLOR:
            GraphTitleFillColor = value.ival;
            break;

        case GTX_GRAPH_TITLE_POS:
            GraphTitlePos = value.ival;
            break;

        case GTX_LEGEND_LABEL_FONT:
            LegendLabelFont = value.ival;
            break;

        case GTX_LEGEND_LABEL_FILL_COLOR:
            LegendLabelFillColor = value.ival;
            break;

        case GTX_LEGEND_LABEL_COLOR:
            LegendLabelColor = value.ival;
            break;

        case GTX_LEGEND_POSITION:
            LegendPosition = value.ival;
            break;

        case GTX_LEGEND_BORDER_COLOR:
            LegendBorderColor = value.ival;
            break;

        case GTX_CURVE_SMOOTH_FLAG:
            CurveSmoothFlag = value.ival;
            if (CurveSmoothFlag < 0) CurveSmoothFlag = 0;
            break;

        case GTX_CURVE_EBAR_COLOR:
            CurveEbarColor = value.ival;
            break;

        case GTX_CURVE_SYMBOL_COLOR:
            CurveSymbolColor = value.ival;
            break;

        case GTX_CURVE_SYMBOL_SPACING:
            CurveSymbolSpacing = value.ival;
            break;

        case GTX_CURVE_SYMBOL_MASK:
            CurveSymbolMask = value.ival;
            break;

        case GTX_CURVE_SYMBOL:
            CurveSymbol = value.ival;
            break;

        case GTX_CURVE_PATTERN:
            CurvePattern = value.ival;
            break;

        case GTX_CURVE_COLOR:
            CurveColor = value.ival;
            break;

        case GTX_CURVE_FILL_PATTERN:
            CurveFillPattern = value.ival;
            break;

        case GTX_CURVE_FILL_COLOR:
            CurveFillColor = value.ival;
            break;

        case GTX_CURVE_LABEL_COLOR:
            CurveLabelColor = value.ival;
            break;

        case GTX_CURVE_LABEL_FILL_COLOR:
            CurveLabelFillColor = value.ival;
            break;

        case GTX_CURVE_LABEL_FONT:
            CurveLabelFont = value.ival;
            break;

        case GTX_CURVE_LABEL_SPACING:
            CurveLabelSpacing = value.ival;
            break;

        case GTX_CURVE_STYLE:
            CurveStyle = value.ival;
            break;

        case GTX_BAR_SHADOW_COLOR:
            CurveShadowColor = value.ival;
            break;

        case GTX_BAR_STACK:
            CurveBarStack = value.ival;
            break;

        case GTX_CURVE_BASE_COLOR:
            CurveBaseColor = value.ival;
            break;

        case GTX_CURVE_PATTERN_COLOR:
            CurvePatternColor = value.ival;
            break;

        case GTX_CURVE_BASE_PATTERN:
            CurveBasePattern = value.ival;
            break;

        case GTX_PIE_DIRECTION:
			PieDirection = value.ival;
			break;

		case GTX_PIE_START_ANGLE:
			PieStartAngle = value.ival;
			break;

		case GTX_PIE_SORT_FLAG:
			PieSortFlag = value.ival;
			break;

		case GTX_PIE_INSIDE_LABELS:
			PieInsideLabels = value.ival;
			break;

		case GTX_PIE_EDGE_LABELS:
			PieEdgeLabels = value.ival;
			break;

		case GTX_PIE_LEGEND_LABELS:
			PieLegendLabels = value.ival;
			break;

        case GTX_CURVE_BASE_THICK:
            CurveBaseThick = value.fval;
            break;

        case GTX_CURVE_LABEL_SIZE:
            CurveLabelSize = value.fval;
            break;

        case GTX_CURVE_LABEL_THICK:
            CurveLabelThick = value.fval;
            break;

        case GTX_CURVE_BASE_VALUE:
            CurveBaseValue = value.fval;
            break;

        case GTX_AXIS_LABEL_SIZE:
            AxisLabelSize = value.fval;
            break;

        case GTX_AXIS_LABEL_GAP:
            AxisLabelGap = value.fval;
            break;

        case GTX_AXIS_CAPTION_SIZE:
            AxisCaptionSize = value.fval;
            break;

        case GTX_AXIS_CAPTION_GAP:
            AxisCaptionGap = value.fval;
            break;

        case GTX_AXIS_MAJOR_INTERVAL:
            AxisMajorInterval = value.fval;
            break;

        case GTX_AXIS_MINOR_INTERVAL:
            AxisMinorInterval = value.fval;
            break;

        case GTX_AXIS_MIN_VALUE:
            AxisMinValue = value.fval;
            break;

        case GTX_AXIS_MAX_VALUE:
            AxisMaxValue = value.fval;
            break;

        case GTX_AXIS_MINOR_LENGTH:
            AxisMinorLength = value.fval;
            break;

        case GTX_AXIS_MAJOR_LENGTH:
            AxisMajorLength = value.fval;
            break;

        case GTX_AXIS_MINOR_THICK:
            AxisMinorThick = value.fval;
            break;

        case GTX_AXIS_MAJOR_THICK:
            AxisMajorThick = value.fval;
            break;

        case GTX_AXIS_MAJOR_GRID_THICK:
            AxisMajorGridThick = value.fval;
            break;

        case GTX_AXIS_MINOR_GRID_THICK:
            AxisMinorGridThick = value.fval;
            break;

        case GTX_AXIS_LINE_THICK:
            AxisLineThick = value.fval;
            break;

        case GTX_AXIS_LABEL_THICK:
            AxisLabelThick = value.fval;
            break;

        case GTX_AXIS_CAPTION_THICK:
            AxisCaptionThick = value.fval;
            break;

        case GTX_GRAPH_TITLE_SIZE:
            GraphTitleSize = value.fval;
            break;

        case GTX_GRAPH_TITLE_THICK:
            GraphTitleThick = value.fval;
            break;

        case GTX_GRAPH_TITLE_GAP:
            GraphTitleGap = value.fval;
            break;

        case GTX_LEGEND_LABEL_SIZE:
            LegendLabelSize = value.fval;
            break;

        case GTX_LEGEND_LABEL_THICK:
            LegendLabelThick = value.fval;
            break;

        case GTX_LEGEND_BORDER_THICK:
            LegendBorderThick = value.fval;
            break;

        case GTX_CURVE_THICK:
            CurveThick = value.fval;
            break;

        case GTX_CURVE_EBAR_THICK:
            CurveEbarThick = value.fval;
            break;

        case GTX_CURVE_EBAR_WIDTH:
            CurveEbarWidth = value.fval;
            break;

        case GTX_CURVE_SYMBOL_SIZE:
            CurveSymbolSize = value.fval;
            break;

        case GTX_CURVE_SYMBOL_THICK:
            CurveSymbolThick = value.fval;
            break;

        case GTX_CURVE_FILL_SIZE:
            CurveFillPatternSize = value.fval;
            break;

        case GTX_BAR_WIDTH_PCT:
            if (value.fval > 1.0) value.fval /= 100.;
            BarWidthPct = value.fval;
            break;

        case GTX_BAR_GAP_PCT:
            if (value.fval > 1.0) value.fval /= 100.;
            BarGapPct = value.fval;
            break;

        case GTX_AXIS_MAJOR_GRID_DASHSCALE:
            AxisMajorGridDashscale = value.fval;
            break;

        case GTX_AXIS_MINOR_GRID_DASHSCALE:
            AxisMinorGridDashscale = value.fval;
            break;

        case GTX_CURVE_BASE_DASHSCALE:
            CurveBaseDashscale = value.fval;
            break;

        case GTX_CURVE_DASHSCALE:
            CurveDashscale = value.fval;
            break;

        default:
            return -3;

    }
