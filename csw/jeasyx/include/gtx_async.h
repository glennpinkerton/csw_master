
/*
         ************************************************
         *                                              *
         *    Copyright (1997-1998) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_async.h

      This include file defines symbolic constants used for
    flags in the gtx_SetAsyncPicking function.  Structures
    used in async picking are also defined here.

*/

/*
    Add nothing above this ifndef
*/
#ifndef GTX_ASYNC_H
#define GTX_ASYNC_H

#  include "csw/utils/include/csw_.h"

#  include "csw/easyx/include/gtx_read.h"

/*
    constant definitions
*/
#  define   GTX_NO_ASYNC                  0
#  define   GTX_SINGLE_ASYNC              1

#  define   GTX_PICK_PRIM_CB              "pickprimcb"
#  define   GTX_PICK_POINT_CB             "pickpointcb"
#  define   GTX_PICK_MANY_CB              "pickmanycb"
#  define   GTX_PICK_LINE_CB              "picklinecb"
#  define   GTX_PICK_MOVE_CB              "pickmovecb"
#  define   GTX_PICK_GRAPH_CB             "pickgraphcb"
#  define   GTX_PICK_MOVE_GRAPH_CB        "pickmovegraphcb"
#  define   GTX_PICK_MOTION_FEEDBACK_CB   "pickmotionfeedbackcb"

#  define   PICK_PRIM_TAG                 1
#  define   PICK_POINT_TAG                2
#  define   PICK_MANY_TAG                 3
#  define   PICK_LINE_TAG                 4
#  define   PICK_MOVE_TAG                 5
#  define   PICK_GRAPH_TAG                6
#  define   PICK_MOVE_GRAPH_TAG           7
#  define   PICK_MOTION_FEEDBACK_TAG      8

#  define   GTX_NO_ACTION                 0
#  define   GTX_LINE_BEING_PICKED         1
#  define   GTX_GRAPHICS_BEING_MOVED      2

#  define   GTX_CANCEL_BY_USER            1
#  define   GTX_CANCEL_BY_CLIENT          2

#  define   ASYNC_NAME_LEN                32

/*
    structure definitions
*/
typedef struct {
    int           cancel_flag,
                  wnum;
}  GTxAsyncData;

typedef struct {
    CSW_F         *x,
                  *y;
    int           cancel_flag;
    int           npt,
                  wnum,
                  fnum,
                  imagenum;
}  GTxAsyncLineData;

typedef struct {
    int           cancel_flag,
                  wnum;
    CSW_F         x,
                  y;
    double        fx,
                  fy;
}  GTxAsyncPointData;

typedef struct {
    int           action;
    int           wnum,
                  fnum,
                  imagenum;
    CSW_F         pagex,
                  pagey;
    char          graphname[ASYNC_NAME_LEN];
}  GTxAsyncMotionData;

/*
    Add nothing below this endif
*/
#endif

