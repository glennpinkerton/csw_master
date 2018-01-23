
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_shapeP.h

      private header file for internal fill plotting functions
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_shapeP.h.
#endif



#ifndef GPF_SHAPEP_H
#define GPF_SHAPEP_H

#  include "csw/utils/include/csw_.h"

/*  constants for shape plotting  */

/*  define some data structures for shape plotting  */

typedef struct {
    int            type;
    CSW_F          fval[10];
    CSW_F          thick, patscale, dashscale;
    CSW_F          xmin, ymin, xmax, ymax;
    int            fill_red, fill_green, fill_blue, fill_alpha;
    int            pat_red, pat_green, pat_blue, pat_alpha;
    int            border_red, border_green, border_blue, border_alpha;
    int            pattern, linepatt;
    int            frame_num, selectable_object_num;
    int            prim_num, graph_num;
    int            layer_num, item_num;
    int            xaxis_num, yaxis_num;
    char           visible_flag;
    char           draw_flag, plot_flag;
    char           selected_flag;
    char           selectable_flag;
    char           editable_flag;
    char           deleted_flag;
    char           smooth_flag;
    char           in_extra_flag = 0;
    int            sub_prims[2];
    int            scaleable;
} SHapePrim ;

/*  do not add anything after this endif  */

#endif
