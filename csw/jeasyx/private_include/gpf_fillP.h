
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    file  gpf_fillp.h

      private header file for internal fill drawing functions
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_fillP.h.
#endif



#ifndef GPF_FILLP_H

#define GPF_FILLP_H

#  include "csw/utils/include/csw_.h"

/*  macros for polyfill plotting  */

#ifndef HOLEFLAG
#define HOLEFLAG             1.e19f
#endif
#ifndef NOSOLIDFILLCOLOR
#define NOSOLIDFILLCOLOR     -10000
#endif

/*  define some data structures for polyfill plotting  */

typedef struct {
    CSW_F          *xypts = NULL;
    int            npts, maxpts;
    CSW_F          xmin, ymin, xmax, ymax;
    CSW_F          thick, patscale, dashscale;
    int            fill_red, fill_green, fill_blue, fill_alpha;
    int            pat_red, pat_green, pat_blue, pat_alpha;
    int            border_red, border_green, border_blue, border_alpha;
    int            pattern;
    int            outline, linepatt;
    int            axis_num, border_num, frame_num, graph_num;
    int            selectable_object_num;
    int            xaxis_num, yaxis_num;
    int            layer_num, item_num;
    int            prim_num;
    char           smooth_flag;
    char           editable_flag;
    char           selectable_flag;
    char           visible_flag;
    char           draw_flag;
    char           plot_flag;
    char           selected_flag;
    char           deleted_flag;
    char           in_extra_flag;
    int            sub_prims[2];
    int            numsub;
    int            scaleable;
    CSW_F          *x_orig = NULL,
                   *y_orig = NULL;
    int            *npts_orig = NULL,
                   ncomp_orig;
} FIllPrim;

/*  do not add anything after this endif  */
#endif
