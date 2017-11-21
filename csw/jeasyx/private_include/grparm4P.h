
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
#error Illegal attempt to include private header file grparm4P.h.
#endif




typedef struct {

    int         cnum;
    void        *LastCurvePtr,
                *LastPointPtr,
                *LastBarPtr;
    int         LastCurveFlag,
                LastPointFlag,
                LastBarFlag;
    int         LayerNumber,
                ItemNumber,
                FrameNumber;
    int         AxisLabelFont,
                AxisLabelColor,
                AxisLabelFillColor,
                AxisLineColor,
                AxisCaptionColor,
                AxisCaptionFillColor,
                AxisCaptionFont,
                AxisLabelAnchor,
                AxisMinorGridPattern,
                AxisMinorGridColor,
                AxisMajorGridPattern,
                AxisMajorGridColor,
                AxisOverflowFlag,
                AxisDirectionFlag,
                AxisTimeFormat,
                AxisBaseTime,
                AxisMonthFlag,
                GraphTitleFont,
                GraphTitleColor,
                GraphTitleFillColor,
                GraphTitlePos,
                LegendLabelFont,
                LegendLabelColor,
                LegendLabelFillColor,
                LegendPosition,
                LegendBorderColor,
                CurveSmoothFlag,
                CurveEbarColor,
                CurveSymbolColor,
                CurveSymbolSpacing,
                CurveSymbolMask,
                CurveSymbol,
                CurvePattern,
                CurveColor,
                CurveFillPattern,
                CurveFillColor,
                CurveStyle,
                CurveShadowColor,
                CurveBarStack,
                CurveLabelSpacing,
                CurveLabelColor,
                CurveLabelFillColor,
                CurveLabelFont,
                CurveBaseColor,
                CurvePatternColor,
                CurveBasePattern,
                PieDirection,
                PieStartAngle,
                PieSortFlag,
                PieInsideLabels,
                PieEdgeLabels,
                PieLegendLabels;

    CSW_F       AxisLabelSize,
                AxisLabelGap,
                AxisLabelAngle,
                AxisCaptionSize,
                AxisCaptionGap,
                AxisMajorInterval,
                AxisMinorInterval,
                AxisMinValue,
                AxisMaxValue,
                AxisMinorLength,
                AxisMajorLength,
                AxisMinorThick,
                AxisMajorThick,
                AxisMajorGridThick,
                AxisMinorGridThick,
                AxisMajorGridDashscale,
                AxisMinorGridDashscale,
                AxisLineThick,
                AxisLabelThick,
                AxisCaptionThick,
                GraphTitleSize,
                GraphTitleThick,
                GraphTitleGap,
                LegendLabelSize,
                LegendLabelThick,
                LegendBorderThick,
                CurveThick,
                CurveEbarThick,
                CurveEbarWidth,
                CurveSymbolSize,
                CurveSymbolThick,
                CurveFillPatternSize,
                CurveLabelSize,
                CurveLabelThick,
                CurveBaseValue,
                CurveBaseThick,
                CurveDashscale,
                CurveBaseDashscale,
                BarWidthPct,
                BarGapPct;

}  GRaphSaveRec;
