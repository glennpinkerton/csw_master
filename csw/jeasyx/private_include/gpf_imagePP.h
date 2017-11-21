
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_imagePP.h

      private header file for internal image plotting functions
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_imagePP.h.
#endif


#ifndef GPF_IMAGEPP_H
#define GPF_IMAGEPP_H

#  include "csw/utils/include/csw_.h"
#  include "csw/utils/private_include/gpf_image.h"


/*  
    define structures for image plotting
*/
typedef struct {
    char               name[100];
    unsigned char      *red_data,
                       *green_data,
                       *blue_data,
                       *transparency_data;
    int                ncol, nrow;
    CSW_F              xmin, ymin, xmax, ymax;
    int                grid_num;
    int                frame_num, graph_num;
    int                layer_num, item_num;
    int                xaxis_num, yaxis_num;
    int                selectable_object_num;
    int                prim_num;
    char               visible_flag;
    char               draw_flag, plot_flag;
    char               selected_flag;
    char               editable_flag;
    char               selectable_flag;
    char               deleted_flag;
    char               smooth_flag;
    int                sub_prims[2];
    int                numsub;
    int                scaleable;
    int                image_id;
    int                has_lines;
} IMagePrim;

/*  
    do not add anything after this endif
*/
#endif
