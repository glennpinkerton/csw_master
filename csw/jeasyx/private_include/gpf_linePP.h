
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_linePP.h

      private header file for internal line plotting functions
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_lineP.h.
#endif


#ifndef GPF_LINEPP_H
#  define GPF_LINEPP_H

#  include "csw/utils/include/csw_.h"

/*
    define constants for line plotting
*/

#ifndef HOLEFLAG
#define HOLEFLAG            1.e19f
#endif

/*  define some data structures for polyline plotting  */

typedef struct {
    CSW_F          *xypts;
    int            npts, maxpts;
    CSW_F          xmin, ymin, xmax, ymax;
    CSW_F          thick, dashscale;
    int            red, green, blue, alpha;
    int            dashpat, symbol, arrowstyle;
    int            axis_num, border_num, frame_num, selectable_object_num;
    int            prim_num, graph_num;
    int            layer_num, item_num;
    int            xaxis_num, yaxis_num;
    int            grid_num;
    char           smooth_flag;
    char           editable_flag;
    char           selectable_flag;
    char           visible_flag;
    char           draw_flag, plot_flag;
    char           selected_flag;
    char           deleted_flag;
    char           in_extra_flag;
    int            sub_prims[2];
    int            numsub;
    int            image_id;
    int            scaleable;
    int            contour_index;
} LInePrim;

/*  do not add anything after this endif  */
#endif

