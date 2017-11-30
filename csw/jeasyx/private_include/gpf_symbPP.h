
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_symbPP.h

      private header file for internal symbol plotting functions
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_symbP.h.
#endif


#ifndef GPF_SYMBPP_H
#define GPF_SYMBPP_H

#  include "csw/utils/include/csw_.h"

/*  define structures for symbol plotting  */

typedef struct {
    CSW_F          x, y;
    CSW_F          thick, angle, size;
    CSW_F          xmin, ymin, xmax, ymax;
    int            red, green, blue, alpha;
    int            symb_num;
    int            grid_num;
    int            frame_num, selectable_object_num;
    int            prim_num, graph_num;
    int            layer_num, item_num;
    int            xaxis_num, yaxis_num;
    char           mask_flag;
    char           visible_flag;
    char           draw_flag, plot_flag;
    char           selected_flag;
    char           editable_flag;
    char           selectable_flag;
    char           deleted_flag;
    char           smooth_flag;
    char           in_extra_flag;
    int            sub_prims[2];
    int            numsub;
    int            image_id;
    int            scaleable;
} SYmbPrim;

/*  do not add anything after this endif  */
#endif
