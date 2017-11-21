
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_graph.h

      This function defines constants and structures needed to
    interface with the graph functions in the EasyX API.  
*/


/*
  add nothing above this ifdef
*/
#ifndef GTX_GRAPH_H
#define GTX_GRAPH_H

#include "csw/utils/include/csw_.h"

/*
    define tags for graph attributes
*/
#define GTX_AXIS_LABEL_FONT             1
#define GTX_AXIS_LABEL_COLOR            2
#define GTX_AXIS_LINE_COLOR             3
#define GTX_AXIS_CAPTION_COLOR          4
#define GTX_AXIS_CAPTION_FONT           5
#define GTX_AXIS_CAPTION_FILL_COLOR     6
#define GTX_AXIS_LABEL_FILL_COLOR       7
#define GTX_AXIS_LABEL_ANCHOR           8

#define GTX_AXIS_LABEL_SIZE             1006
#define GTX_AXIS_LABEL_GAP              1007
#define GTX_AXIS_CAPTION_SIZE           1008
#define GTX_AXIS_CAPTION_GAP            1009
#define GTX_AXIS_MINOR_LENGTH           1010
#define GTX_AXIS_MAJOR_LENGTH           1011
#define GTX_AXIS_MINOR_THICK            1012
#define GTX_AXIS_MAJOR_THICK            1013
#define GTX_AXIS_LABEL_THICK            1014
#define GTX_AXIS_CAPTION_THICK          1015
#define GTX_AXIS_MAJOR_INTERVAL         1016
#define GTX_AXIS_MINOR_INTERVAL         1017
#define GTX_AXIS_MIN_VALUE              1018
#define GTX_AXIS_MAX_VALUE              1019
#define GTX_AXIS_FIRST_VALUE            GTX_AXIS_MIN_VALUE
#define GTX_AXIS_LAST_VALUE             GTX_AXIS_MAX_VALUE
#define GTX_AXIS_MAJOR_GRID_THICK       1020
#define GTX_AXIS_MINOR_GRID_THICK       1021
#define GTX_AXIS_LINE_THICK             1022
#define GTX_AXIS_MAJOR_GRID_DASHSCALE   1023
#define GTX_AXIS_MINOR_GRID_DASHSCALE   1024
#define GTX_AXIS_LABEL_ANGLE            1025

#define GTX_AXIS_OVERFLOW_FLAG        20
#define GTX_AXIS_MAJOR_GRID_PATTERN   21
#define GTX_AXIS_MAJOR_GRID_COLOR     22
#define GTX_AXIS_MINOR_GRID_PATTERN   23
#define GTX_AXIS_MINOR_GRID_COLOR     24
#define GTX_AXIS_DIRECTION_FLAG       25

#define GTX_AXIS_TIME_FORMAT          26
#define GTX_AXIS_BASE_TIME            27
#define GTX_AXIS_MONTH_FLAG           28

#define GTX_GRAPH_TITLE_SIZE         1100
#define GTX_GRAPH_TITLE_FONT         101
#define GTX_GRAPH_TITLE_THICK        1102
#define GTX_GRAPH_TITLE_COLOR        103
#define GTX_GRAPH_TITLE_POS          104
#define GTX_GRAPH_TITLE_GAP          1105
#define GTX_GRAPH_TITLE_FILL_COLOR   106

#define GTX_LEGEND_LABEL_COLOR     200
#define GTX_LEGEND_LABEL_SIZE      1201
#define GTX_LEGEND_LABEL_FONT      202
#define GTX_LEGEND_LABEL_THICK     1203
#define GTX_LEGEND_POSITION        204
#define GTX_LEGEND_BORDER_COLOR    205
#define GTX_LEGEND_BORDER_THICK    1206
#define GTX_LEGEND_LABEL_FILL_COLOR     207

#define GTX_CURVE_SYMBOL_SIZE      1300
#define GTX_CURVE_THICK            1301
#define GTX_CURVE_COLOR            302
#define GTX_CURVE_PATTERN          303
#define GTX_CURVE_SYMBOL           304
#define GTX_CURVE_SYMBOL_COLOR     305
#define GTX_CURVE_FILL_PATTERN     306
#define GTX_CURVE_FILL_COLOR       307
#define GTX_CURVE_EBAR_COLOR       308
#define GTX_CURVE_EBAR_THICK       1309
#define GTX_CURVE_EBAR_WIDTH       1310
#define GTX_CURVE_SYMBOL_SPACING   311
#define GTX_CURVE_SYMBOL_MASK      312
#define GTX_CURVE_SYMBOL_THICK     1313
#define GTX_CURVE_SMOOTH_FLAG      314
#define GTX_CURVE_FILL_SIZE        1315
#define GTX_CURVE_STYLE            316
#define GTX_CURVE_LABEL_SPACING    317
#define GTX_CURVE_LABEL_COLOR      318
#define GTX_CURVE_LABEL_FONT       319
#define GTX_CURVE_LABEL_SIZE       1320
#define GTX_CURVE_LABEL_FILL_COLOR 321
#define GTX_CURVE_LABEL_THICK      1322
#define GTX_CURVE_BASE_VALUE       1323
#define GTX_CURVE_BASE_COLOR       324
#define GTX_CURVE_BASE_PATTERN     325
#define GTX_CURVE_BASE_THICK       1326
#define GTX_BAR_SHADOW_COLOR       327
#define GTX_BAR_STACK              328
#define GTX_CURVE_PATTERN_COLOR    329
#define GTX_BAR_WIDTH_PCT          1330
#define GTX_BAR_GAP_PCT            1331
#define GTX_CURVE_DASHSCALE        1332
#define GTX_CURVE_BASE_DASHSCALE   1333

#define GTX_PIE_DIRECTION          401
#define GTX_PIE_START_ANGLE        402
#define GTX_PIE_SORT_FLAG          403
#define GTX_PIE_INSIDE_LABELS      404
#define GTX_PIE_EDGE_LABELS        405
#define GTX_PIE_LEGEND_LABELS      406

#define GTX_NO_TIME_FORMAT     CSW_NO_TIME_FORMAT
#define GTX_HOUR_MIN_SEC       CSW_HOUR_MIN_SEC
#define GTX_MIN_SEC            CSW_MIN_SEC
#define GTX_HOUR_MIN           CSW_HOUR_MIN
#define GTX_HOUR               CSW_HOUR
#define GTX_MIN                CSW_MIN
#define GTX_DAY_HOUR_MIN       CSW_DAY_HOUR_MIN
#define GTX_DAY_HOUR           CSW_DAY_HOUR
#define GTX_DAY                CSW_DAY
#define GTX_MONTH_DATE         CSW_MONTH_DATE
#define GTX_MONTH              CSW_MONTH
#define GTX_YEAR_MONTH_DATE    CSW_YEAR_MONTH_DATE
#define GTX_YEAR_MONTH         CSW_YEAR_MONTH
#define GTX_YEAR               CSW_YEAR
#define GTX_MONTH_DATE_YEAR    CSW_MONTH_DATE_YEAR
#define GTX_DATE               CSW_DATE
#define GTX_HOUR_MIN_DATE      CSW_HOUR_MIN_DATE
#define GTX_JUL_DAY            CSW_JUL_DAY
#define GTX_JUL_DAY_YEAR       CSW_JUL_DAY_YEAR
#define GTX_DATE_MONTH_YEAR    CSW_DATE_MONTH_YEAR
#define GTX_DATE_MONTH         CSW_DATE_MONTH

#define GTX_FULL_DATE          CSW_FULL_DATE

#define GTX_USE_MONTH_NUMBER   CSW_USE_MONTH_NUMBER
#define GTX_USE_MONTH_NAME     CSW_USE_MONTH_NAME


#define GTX_CONNECT_POINTS         1
#define GTX_DRAW_TO_BASELINE       2
#define GTX_VERTICAL_STEP          3
#define GTX_HORIZONTAL_STEP        4
#define GTX_CENTER_STEP            5

#define GTX_LEGEND_ON_RIGHT        1
#define GTX_LEGEND_ON_LEFT         2
#define GTX_NO_LEGEND              3
#define GTX_GLOBAL_LEGEND          4

#define GTX_TOP_CENTER_TITLE       1
#define GTX_TOP_LEFT_TITLE         2
#define GTX_BOTTOM_CENTER_TITLE    3
#define GTX_BOTTOM_LEFT_TITLE      4
#define GTX_TITLE_IN_LEGEND        5

#define GTX_TOP_AXIS               0
#define GTX_BOTTOM_AXIS            1
#define GTX_LEFT_AXIS              2
#define GTX_RIGHT_AXIS             12

#define GTX_AXIS_CLIP              1
#define GTX_AXIS_RESCALE           2
#define GTX_SCROLL_LOW             3
#define GTX_SCROLL_HIGH            4
#define GTX_AXIS_SCROLL_LOW        3
#define GTX_AXIS_SCROLL_HIGH       4

#define GTX_AXIS_MIN_FIRST         1
#define GTX_AXIS_MAX_FIRST         2

#define GRAPH_DEFAULTS_TAG         -1
#define GTX_GRAPH_DEFAULTS_TAG     GRAPH_DEFAULTS_TAG

#define GTX_GRAPH_TO_PAGE          0
#define GTX_PAGE_TO_GRAPH          1

#define GTX_MAX_AXIS_LABEL_LENGTH  1000
#define GTX_MAX_AXIS_LABELS        1000

#define GTX_CENTER_ON_POSITION     1
#define GTX_CENTER_IN_INTERVAL     2

#define GTX_BARS_AT_VALUES         1
#define GTX_BARS_AT_LABELS         2

#define GTX_STACK_HORIZONTAL       1
#define GTX_STACK_VERTICAL         2

#define GTX_ANCHOR_LEFT_EDGE       1
#define GTX_ANCHOR_RIGHT_EDGE      2

#define GTX_USE_OLD_PARAMETERS     1
#define GTX_USE_NEW_PARAMETERS     2

#define GTX_NO_MIRROR              0
#define GTX_MIRROR_AXIS            1
#define GTX_MIRROR_LABELS          2
#define GTX_MIRROR_CAPTION         3
#define GTX_MIRROR_ALL             4

#define GTX_PIE_CLOCKWISE          1
#define GTX_PIE_COUNTER_CLOCKWISE  2

#define GTX_PIE_SORT_ASCENDING     1
#define GTX_PIE_SORT_DESCENDING    2
#define GTX_PIE_UNSORTED           3

#define GTX_PIE_LABEL_NONE         0
#define GTX_PIE_LABEL_LABELS       1
#define GTX_PIE_LABEL_VALUES       2
#define GTX_PIE_LABEL_ALL          3

/*
  add nothing below this endif
*/
#endif
