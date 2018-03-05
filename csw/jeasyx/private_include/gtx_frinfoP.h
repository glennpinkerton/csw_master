
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_frinfoP.h

      This file has structure and constant definitions for the 
    frame list support in the EasyX server.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gtx_frinfoP.h.
#endif


/*
    add nothing above this ifdef
*/

#ifndef GTX_FRINFOP_H
#define GTX_FRINFOP_H

#  include "csw/utils/include/csw_.h"

/*
    define constants for frame data
*/

#define MAXFRAMES        500
#define NOFRAMEVAL       1.e30


/*
    define frame structures
*/
typedef struct {
    int          label_flag;
    int          caption_flag;
    int          tick_flag;
    int          tick_direction;
    char         caption[200];
    CSW_F        major_interval;
    int          line_red,
                 line_green,
                 line_blue;
    int          text_red,
                 text_green,
                 text_blue;
    CSW_F        line_thickness,
                 text_size,
                 text_thickness;
    int          text_font;
    int          selectable_item;
    CSW_F        page_margin;
    int          label_anchor;
    double       label_angle;
}  FRameAxisStruct;

typedef struct {
    FRameAxisStruct    *ap = NULL;
    CSW_F              fx1,
                       fy1,
                       fx2,
                       fy2;
    CSW_F              afirst,
                       alast;
    int                frame_num;
    int                layer_num,
                       item_num;
    int                label_dir;
    int                prim_num;
    int                selectable_object_num;
    int                deleted_flag;
    int                visible_flag;
    bool               calc_prims_needed;
}  AXisPrim;

typedef struct {
    double       xmin, ymin, xmax, ymax;
    double       xmin_spatial, ymin_spatial,
                 xmax_spatial, ymax_spatial;
    double       x1, y1, x2, y2;
    double       orig_x1, orig_y1, orig_x2, orig_y2;
    CSW_F        px1orig, py1orig, px2orig, py2orig;
    CSW_F        px1, py1, px2, py2;
    CSW_F        left_margin, right_margin, top_margin, bottom_margin;
    CSW_F        page_left, page_right, page_top, page_bottom;
    CSW_F        xpct, ypct;
    double       top_inset,
                 left_inset,
                 bottom_inset,
                 right_inset;
    int          borderflag;
    int          at_zoom_extents;
    FRameAxisStruct  left_axis,
                     right_axis,
                     bottom_axis,
                     top_axis;
    int          needs_surf_prims;
    int          clipflag;
    int          rescaleable;
    int          moveable;
    int          **line_index = NULL,
                 **fill_index = NULL,
                 **text_index = NULL,
                 **symb_index = NULL,
                 **shape_index = NULL,
                 **contour_index = NULL;
    int          num_line_index,
                 num_fill_index,
                 num_text_index,
                 num_symb_index,
                 num_shape_index,
                 num_contour_index;
    int          ncol, nrow;
    double       xspace, yspace;
    int          aspect_flag;
    int          rescale_needed;
    int          reborder_needed;
    char         name[100];
    int          attach_frame;
    int          attach_position;
    int          scale_width_to_attach_frame;
    int          scale_height_to_attach_frame;
    int          is_positioned;
    int          deleted;
    CSW_F        extra_gap,
                 perpendicular_move;
    int          patch_draw_flag;

    int          scale_text_sizes = 0;

} FRameStruct;

/*
    add nothing below this endif
*/

#endif

