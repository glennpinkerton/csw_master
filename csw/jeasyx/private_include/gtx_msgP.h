
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_msgP.h

    This defines the command message numbers for the java to easyx communication
    on the easyx side.  The same numbers and symbols need to be defined on the
    Java side also.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gtx_msgP.h.
#endif


/*
    add nothing above this ifndef
*/
#ifndef GTX_MSGP_H
#define GTX_MSGP_H

#  include "csw/utils/include/csw_.h"

#define GTX_MSG_STRING_SEPARATOR "~|~"
/*
    define constants for message types

    control messages
*/
#define GTX_LOG_COMMENT          8887
#define GTX_OPEN_LOG_FILE        8888
#define GTX_CLOSE_LOG_FILE       8889
#define GTX_DRAW_CURRENT_VIEW    9999
#define GTX_DRAW_SELECTED        9998
#define GTX_UNSELECT_ALL         9997

#define GTX_DELETE_SELECTED      9000
#define GTX_HIDE_SELECTED        9001
#define GTX_UNHIDE_ALL           9002

#define GTX_GET_SYMBOL_PARTS     5000

#define GTX_CREATEWINDOW         1
#define GTX_OPENWINDOW           2
#define GTX_SETCLIPAREA          3
#define GTX_PANFRAME             4
#define GTX_SETLAYER             7
#define GTX_SETITEM              8
#define GTX_SETFRAME             9
#define GTX_ZOOMFRAME           10
#define GTX_ZOOMEXTENTS         11
#define GTX_ZOOMOUT             12
#define GTX_FRAME_GAP           13
#define GTX_FRAME_AXIS_VALUES   14
#define GTX_UNSETFRAME          15
#define GTX_RESETFRAME          16
#define GTX_CREATEFRAME         17
#define GTX_DEFAULT_ATTRIB      18
#define GTX_DELETEWINDOW        19
#define GTX_UNSETLAYER          20
#define GTX_UNSETITEM           21
#define GTX_DISPLAYFILE         22
#define GTX_SAVEINFILE          23
#define GTX_REDRAWALL           24
#define GTX_SETSCREENSIZE       26
#define GTX_SETFRAMECLIP        27
#define GTX_ADD_AXIS            28
#define GTX_FRAME_NAME_EXTENTS  29

#define GTX_FRAME_NO_BORDER       0
#define GTX_FRAME_PLAIN_BORDER    1
#define GTX_FRAME_LABEL_LEFT_BOTTOM_BORDER    2
#define GTX_FRAME_LABEL_LEFT_TOP_BORDER       3
#define GTX_FRAME_LABEL_RIGHT_BOTTOM_BORDER   4
#define GTX_FRAME_LABEL_RIGHT_TOP_BORDER      5
#define GTX_FRAME_LABEL_ALL_SIDES_BORDER      6

#define GTX_FRAME_ATTACH_LEFT_MIN       1
#define GTX_FRAME_ATTACH_LEFT_MIDDLE    2
#define GTX_FRAME_ATTACH_LEFT_MAX       3
#define GTX_FRAME_ATTACH_TOP_MIN        4
#define GTX_FRAME_ATTACH_TOP_MIDDLE     5
#define GTX_FRAME_ATTACH_TOP_MAX        6
#define GTX_FRAME_ATTACH_RIGHT_MIN      7
#define GTX_FRAME_ATTACH_RIGHT_MIDDLE   8
#define GTX_FRAME_ATTACH_RIGHT_MAX      9
#define GTX_FRAME_ATTACH_BOTTOM_MIN     10
#define GTX_FRAME_ATTACH_BOTTOM_MIDDLE  11
#define GTX_FRAME_ATTACH_BOTTOM_MAX     12

#define GTX_FRAME_LEFT_AXIS             1
#define GTX_FRAME_RIGHT_AXIS            2
#define GTX_FRAME_BOTTOM_AXIS           3
#define GTX_FRAME_TOP_AXIS              4

#define GTX_FORCE_UNIT_FRAME_ASPECT_RATIO   0
/* strange number discourages accidental use of any aspect ratio */
#define GTX_ALLOW_ANY_FRAME_ASPECT_RATIO    9217

/*
 * units constants for attached frames.
 */
#define GTX_PAGE_UNITS                  1
#define GTX_DEVICE_INCHES               2
#define GTX_DEVICE_CM                   3
#define GTX_PERCENT_BASE_FRAME          4
#define GTX_PERCENT_WINDOW              5

/*
    graphic attribute messages
*/
#define GTX_SETLINETHICK        30
#define GTX_SETFONT             31
#define GTX_SETCOLOR            32
#define GTX_SETFILLPATTERN      33
#define GTX_SETLINEPATTERN      34
#define GTX_SETVISIBILITY       35
#define GTX_SETTEXTTHICK        36
#define GTX_SETFILLSCALE        37
#define GTX_IMAGECOLORBANDS     38
#define GTX_SYMBOLMASK          39
#define GTX_SETSMOOTH           40
#define GTX_ARROW_STYLE         41
#define GTX_SETBGCOLOR          42
#define GTX_SETFGCOLOR          43
#define GTX_IMAGENAME           44
#define GTX_IMAGEOPTIONS        45
#define GTX_TEXTANCHOR          46
#define GTX_TEXTOFFSETS         47
#define GTX_TEXTBACKGROUND      48

#define GTX_SET_ALL_COLORS_FLAG              0
#define GTX_SET_FILL_COLOR_FLAG              1
#define GTX_SET_LINE_COLOR_FLAG              2
#define GTX_SET_TEXT_COLOR_FLAG              3
#define GTX_SET_SYMBOL_COLOR_FLAG            4
#define GTX_SET_PATTERN_COLOR_FLAG           5
#define GTX_SET_TEXTFILL_COLOR_FLAG          6
#define GTX_SET_BORDER_COLOR_FLAG            7

#define GTX_NO_ARROW                         0
#define GTX_SIMPLE_ARROW                     1
#define GTX_TRIANGLE                         2
#define GTX_FILLED_TRIANGLE                  3
#define GTX_OFFSET_TRIANGLE                  4
#define GTX_FILLED_OFFSET_TRIANGLE           5

/*
    primitive drawing messages
*/
#define GTX_DRAWLINE            60
#define GTX_FILLPOLY            61
#define GTX_DRAWSYMBOL          62
#define GTX_DRAWTEXT            63
#define GTX_DRAWSHAPE           64
#define GTX_DRAWIMAGE           65
#define GTX_DRAWNUMBER          66

/*
    picking messages
*/
#define GTX_SELECTGROUPNAME     70
#define GTX_SET_SELECTABLE      71
#define GTX_PICKFILTER          72
#define GTX_PICKPRIM            73
#define GTX_UNPICK              74
#define GTX_PICKGROUP           75
#define GTX_ERASEPRIM           76
#define GTX_ERASEFLAG           77
#define GTX_PICKLINE            78
#define GTX_PICKMOVE            79
#define GTX_EDITPRIM            80
#define GTX_ERASE_SELECTABLE    81
#define GTX_CONVERT_TO_FRAME    82
#define GTX_SET_SELECT_STATE    83
#define GTX_GET_PRIM_NUM        84

#define GTX_SET_ASYNC_FLAG      90
#define GTX_POLL_ASYNC_PICK     91
#define GTX_CANCEL_ASYNC_PICK   92

/*
    grid, contour and trimesh messages.
*/
#define GTX_TRIMESH_DATA       101
#define GTX_CONTOUR_PROPERTIES 110
#define GTX_CONTOUR_LINE       111
#define GTX_GRID_DATA          121
#define GTX_FAULT_LINE_DATA    131
#define GTX_BLENDED_GRID_DATA  132
#define GTX_BLEND_TRIMESH_DATA 133
#define GTX_NDP_GRAPHIC_PROPS  134
#define GTX_IMAGE_FAULT_DATA   135

/*
    graph and chart messages
*/
#define GTX_CREATE_GRAPH       200
#define GTX_CREATE_AXIS        201
#define GTX_CREATE_CURVE       202
#define GTX_DRAW_GRAPH         203
#define GTX_END_GRAPH          204
#define GTX_GRAPH_PARAM        205
#define GTX_CREATE_CURVE_FILL  206
#define GTX_CREATE_POINTS      207
#define GTX_CONVERT_GRAPH      208
#define GTX_AXIS_LABELS        209
#define GTX_CREATE_BARS        210
#define GTX_CREATE_LEGEND      211
#define GTX_END_LEGEND         212
#define GTX_OPEN_GRAPH         213
#define GTX_OPEN_LEGEND        214
#define GTX_DELETE_CURVE       215
#define GTX_DELETE_POINTS      216
#define GTX_DELETE_BARS        217
#define GTX_DELETE_CURVE_FILL  218
#define GTX_DELETE_AXIS        219
#define GTX_CHANGE_GRAPH_TITLE 220
#define GTX_RESET_GRAPH_PARAM  221
#define GTX_PRIM_AXES          222

#define GTX_CREATE_PIE_CHART   270
#define GTX_CREATE_PIE_SLICE   271
#define GTX_DELETE_PIE_SLICE   272
#define GTX_EDIT_PIE_SLICE     273
#define GTX_OPEN_PIE_CHART     274
#define GTX_END_PIE_CHART      275
#define GTX_DRAW_PIE_CHART     276
#define GTX_CONVERT_PIE        277

/*
    misc. messages
*/
#define GTX_SETCLIENT         1000
#define GTX_FREECLIENT        1001
#define GTX_EDITFLAG          2000
#define GTX_XEVENT            3000
#define GTX_LIMIT_MSG         4000
#define GTX_XFLUSH            9999

#define GTX_SWAPOUT           9998
#define GTX_MEMLOCK           9997


#endif
/*
    add nothing below this endif
*/
