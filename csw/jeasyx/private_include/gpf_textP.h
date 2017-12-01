
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_textP.h

      private header file for internal text plotting functions
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_textP.h.
#endif


#ifndef GPF_TEXTP_H
#define GPF_TEXTP_H

#  include "csw/utils/include/csw_.h"

/*  define structures for text plotting  */

typedef struct {
    CSW_F          x, y;
    char           *chardata = NULL;
    CSW_F          xmin, ymin, xmax, ymax;
    CSW_F          bg_thick, thick, angle, size;
    CSW_F          xoff, yoff;
    int            anchor;
    int            border_red, border_green, border_blue, border_alpha;
    int            fill_red, fill_green, fill_blue, fill_alpha;
    int            bgflag;
    int            bg_red, bg_green, bg_blue, bg_alpha;
    int            bg_border_red, bg_border_green, bg_border_blue, bg_border_alpha;
    int            font_num;
    int            axis_num, border_num, frame_num, selectable_object_num;
    int            grid_num;
    int            prim_num, graph_num;
    int            xaxis_num, yaxis_num;
    int            layer_num, item_num;
    char           visible_flag;
    char           draw_flag, plot_flag;
    char           selected_flag;
    char           editable_flag;
    char           deleted_flag;
    char           selectable_flag;
    char           in_extra_flag;
    int            sub_prims[2];
    int            numsub;
    int            image_id;
    int            scaleable;
} TExtPrim;

/*  do not add anything after this endif  */
#endif
