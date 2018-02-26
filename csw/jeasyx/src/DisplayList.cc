
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 *  DisplayList.cc
 *
 *  This is the implementation of the CDisplayList class.
 *  This class encapsulates all graphics drawn to a given
 *  graphics window.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "csw/hlevutils/src/simulate_new.h"

#include "csw/jeasyx/private_include/DisplayList.h"
#include "csw/jeasyx/private_include/DisplayListJNI.h"
#include "csw/jeasyx/private_include/EZXJavaArea.h"
#include "csw/jeasyx/private_include/gtx_drawprim.h"
#include "csw/jeasyx/private_include/gtx_msgP.h"

#include "csw/surfaceworks/include/contour_api.h"
#include "csw/surfaceworks/include/grid_api.h"
#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"
#include "csw/utils/private_include/ply_protoP.h"
#include "csw/utils/private_include/gpf_calcdraw.h"
#include "csw/utils/private_include/gpf_graphP.h"
#include "csw/utils/private_include/gpf_shape.h"
#include "csw/utils/private_include/gpf_spline.h"
#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/TextBounds.h"

#include "csw/utils/include/csw_errnum.h"
#include "csw/utils/include/csw_timer.h"

/*  constants for the file  */

#define _DL_XY_TOO_BIG_         1.e15f
#define _DL_IS_SUB_PRIM_FLAG_   -9837

template<typename T>
void CDisplayList::ZeroInit (T p, int n) {
    memset((void *)p, 0, (size_t)n);
}


/*
 * Constructor
 */
CDisplayList::CDisplayList()
{
    int          i;

    gtx_drawprim_obj.SetEZXJavaPtr (&ezx_java_obj);

    dfile = NULL;

    page_units_type = 1;

    ImageID = -1;

/*
 * Initialize graphic attribute states to defaults.
 */
    current_frame_clip_flag = 0;
    current_border_num = -1;
    current_axis_num = -1;
    current_grid_num = -1;
    current_frame_num = -1;
    current_contour_index = -1;
    current_graph_num = -1;
    current_selectable_object_num = -1;
    current_selectable_object = NULL;
    current_xaxis_num = -1;
    current_yaxis_num = -1;

    current_layer_num = -1;
    current_item_num = -1;
    current_size_units = 0;

    current_line_thickness = 0.001f;
    current_line_pattern = -1;
    current_line_pattern_size = 1.0f;
    current_line_symbol = -1;
    current_line_arrow_style = 0;

    current_text_font = -1;
    current_text_thickness = 0.0f;
    current_text_anchor = 1;
    current_text_x_offset = 0.0f;
    current_text_y_offset = 0.0f;
    current_text_background = 0;

    current_fill_outline_flag = 1;
    current_fill_pattern = -1;
    current_fill_pattern_size = 1.0f;

/*
 * default has no image data or color bands.
 */
    memset (current_image_low_values, 0, 1000 * sizeof(CSW_F));
    memset (current_image_high_values, 0, 1000 * sizeof(CSW_F));
    memset (current_image_red_values, 0, 1000 * sizeof(int));
    memset (current_image_green_values, 0, 1000 * sizeof(int));
    memset (current_image_blue_values, 0, 1000 * sizeof(int));
    memset (current_image_transparency_values, 0, 1000 * sizeof(int));
    current_number_image_values = 0;
    current_image_name[0] = '\0';
    current_image_id = 0;

    current_image_origin = 0;
    current_image_column1 = 0;
    current_image_row1 = 0;
    current_image_colspace = 1;
    current_image_rowspace = 1;

/*
 * default background is white, foreground is black
 * and all default object colors are black.
 */
    current_background_red = 255;
    current_background_green = 255;
    current_background_blue = 255;
    current_background_alpha = 255;
    current_foreground_red = 5;
    current_foreground_green = 5;
    current_foreground_blue = 5;
    current_foreground_alpha = 255;

    current_global_red = 0;
    current_global_green = 0;
    current_global_blue = 0;
    current_global_alpha = 255;
    current_line_red = 0;
    current_line_green = 0;
    current_line_blue = 0;
    current_line_alpha = 255;
    current_fill_red = 0;
    current_fill_green = 0;
    current_fill_blue = 0;
    current_fill_alpha = 255;
    current_symbol_red = 0;
    current_symbol_green = 0;
    current_symbol_blue = 0;
    current_symbol_alpha = 255;
    current_text_red = 0;
    current_text_green = 0;
    current_text_blue = 0;
    current_text_alpha = 255;
    current_text_fill_red = 0;
    current_text_fill_green = 0;
    current_text_fill_blue = 0;
    current_text_fill_alpha = 255;
    current_fill_pattern_red = 0;
    current_fill_pattern_green = 0;
    current_fill_pattern_blue = 0;
    current_fill_pattern_alpha = 255;
    current_fill_border_red = 0;
    current_fill_border_green = 0;
    current_fill_border_blue = 0;
    current_fill_border_alpha = 255;

    current_image_null_value = 0.0;

/*
 * default is no line smoothing, and objects are
 * editable and selectable.
 */
    current_line_smooth_flag = 0;
    current_editable_flag = 1;
    current_selectable_flag = 1;

/*
 * initialize the frame coordinates to invalid.
 */
    Fx1 = 1.e30f;
    Fy1 = 1.e30f;
    Fx2 = -1.e30f;
    Fy2 = -1.e30f;
    Px1 = 1.e30f;
    Py1 = 1.e30f;
    Px2 = -1.e30f;
    Py2 = -1.e30f;

    Pickx1 = 1.e30f;
    Picky1 = 1.e30f;
    Pickx2 = -1.e30f;
    Picky2 = -1.e30f;

    HideFlag = 0;

    patch_draw_flag = 0;

/*
 * Initialize spatial index arrays to NULL
 */
    line_spatial_index = NULL;
    fill_spatial_index = NULL;
    symb_spatial_index = NULL;
    text_spatial_index = NULL;
    shape_spatial_index = NULL;
    contour_spatial_index = NULL;

    num_line_index = 0;
    num_fill_index = 0;
    num_text_index = 0;
    num_symb_index = 0;
    num_shape_index = 0;
    num_contour_index = 0;

/*
 * Initialize geometry to bad values
 */
    raw_page_xmin = 1.e30;
    raw_page_ymin = 1.e30;
    raw_page_xmax = -1.e30;
    raw_page_ymax = -1.e30;

    raw_from_hints_flag = 0;

    page_xmin = 1.e30;
    page_ymin = 1.e30;
    page_xmax = -1.e30;
    page_ymax = -1.e30;

    screen_xmin = 1.e30;
    screen_ymin = 1.e30;
    screen_xmax = -1.e30;
    screen_ymax = -1.e30;

    clip_xmin = 1.e30f;
    clip_ymin = 1.e30f;
    clip_xmax = -1.e30f;
    clip_ymax = -1.e30f;

    x_page_to_screen_scale = 1.e30;
    y_page_to_screen_scale = 1.e30;

    page_units_per_inch = 10.0;

    index_xmin = 1.e30;
    index_ymin = 1.e30;
    index_xmax = -1.e30;
    index_ymax = -1.e30;
    index_xspace = 1.e30;
    index_yspace = 1.e30;

    index_ncol = 0;
    index_nrow = 0;

    page_index_xmin = 1.e30;
    page_index_ymin = 1.e30;
    page_index_xmax = -1.e30;
    page_index_ymax = -1.e30;

    screen_dpi = 72;

/*
 * Initialize the structured graphics lists to NULL and empty
 */
    frame_list = NULL;
    num_frame_list = 0;
    max_frame_list = 0;

    graph_list = NULL;
    num_graph_list = 0;
    max_graph_list = 0;

    selectable_object_list = NULL;
    num_selectable_object_list = 0;
    max_selectable_object_list = 0;

    layer_list = NULL;
    num_layer_list = 0;
    max_layer_list = 0;

    item_list = NULL;
    num_item_list = 0;
    max_item_list = 0;

/*
 * Initialize the color band index data.
 */
    memset (color_band_index, 0, _DL_NUM_COLOR_BAND_INDEX_ * sizeof(int));
    min_color_band_index = 1.e30f;
    max_color_band_index = -1.e30f;
    delta_color_band_index = 1.e30f;

/*
 * initialize the work arrays.
 */
    xywork = (CSW_F *)csw_Calloc (_DL_MAX_WORK_ * 4 * sizeof(CSW_F));
    xywork2 = (CSW_F *)csw_Calloc (_DL_MAX_WORK_ * 4 * sizeof(CSW_F));
    iwork = (int *)csw_Calloc (_DL_MAX_WORK_ * 4 * sizeof(int));

/*
 * names for size units
 */
    for (i=0; i<_DL_NUM_SIZE_UNITS_LIST_; i++) {
        size_units_list[i] = size_units_buffer + 20 * i;
    }
    strcpy (size_units_list[0], "inches");
    strcpy (size_units_list[1], "cm");
    strcpy (size_units_list[2], "mm");

    spline_used_flag = 0;
    frame_layout_needed = 1;
    screen_was_resized = 0;

    minimum_frame_separation = 0.1f;

    border_margin_only_flag = 0;

/*
 * Initialize graphic attribute states to defaults.
 */
    saved_frame_clip_flag = 0;
    saved_border_num = -1;
    saved_axis_num = -1;
    saved_frame_num = -1;
    saved_graph_num = -1;
    saved_selectable_object_num = -1;
    saved_selectable_object = NULL;
    saved_xaxis_num = -1;
    saved_yaxis_num = -1;

    saved_layer_num = -1;
    saved_item_num = -1;
    saved_size_units = 0;

    saved_line_thickness = 0.001f;
    saved_line_pattern = -1;
    saved_line_pattern_size = 1.0f;
    saved_line_symbol = -1;
    saved_line_arrow_style = 0;

    saved_text_font = -1;
    saved_text_thickness = 0.0f;
    saved_text_anchor = 1;
    saved_text_x_offset = 0.0f;
    saved_text_y_offset = 0.0f;
    saved_text_background = 0;

    saved_fill_outline_flag = 1;
    saved_fill_pattern = -1;
    saved_fill_pattern_size = 1.0f;

/*
 * default has no image data or color bands.
 */
    memset (saved_image_low_values, 0, 1000 * sizeof(CSW_F));
    memset (saved_image_high_values, 0, 1000 * sizeof(CSW_F));
    memset (saved_image_red_values, 0, 1000 * sizeof(int));
    memset (saved_image_green_values, 0, 1000 * sizeof(int));
    memset (saved_image_blue_values, 0, 1000 * sizeof(int));
    memset (saved_image_transparency_values, 0, 1000 * sizeof(int));
    saved_number_image_values = 0;
    saved_image_name[0] = '\0';
    saved_image_id = 0;

    saved_image_origin = 0;
    saved_image_column1 = 0;
    saved_image_row1 = 0;
    saved_image_colspace = 1;
    saved_image_rowspace = 1;

/*
 * default background is white, foreground is black
 * and all default object colors are black.
 */
    saved_background_red = 255;
    saved_background_green = 255;
    saved_background_blue = 255;
    saved_background_alpha = 255;
    saved_foreground_red = 5;
    saved_foreground_green = 5;
    saved_foreground_blue = 5;
    saved_foreground_alpha = 255;

    saved_global_red = 0;
    saved_global_green = 0;
    saved_global_blue = 0;
    saved_global_alpha = 255;
    saved_line_red = 0;
    saved_line_green = 0;
    saved_line_blue = 0;
    saved_line_alpha = 255;
    saved_fill_red = 0;
    saved_fill_green = 0;
    saved_fill_blue = 0;
    saved_fill_alpha = 255;
    saved_symbol_red = 0;
    saved_symbol_green = 0;
    saved_symbol_blue = 0;
    saved_symbol_alpha = 255;
    saved_text_red = 0;
    saved_text_green = 0;
    saved_text_blue = 0;
    saved_text_alpha = 255;
    saved_text_fill_red = 0;
    saved_text_fill_green = 0;
    saved_text_fill_blue = 0;
    saved_text_fill_alpha = 255;
    saved_fill_pattern_red = 0;
    saved_fill_pattern_green = 0;
    saved_fill_pattern_blue = 0;
    saved_fill_pattern_alpha = 255;
    saved_fill_border_red = 0;
    saved_fill_border_green = 0;
    saved_fill_border_blue = 0;
    saved_fill_border_alpha = 255;

    saved_image_null_value = 0.0;

/*
 * default is no line smoothing, and objects are
 * editable and selectable.
 */
    saved_line_smooth_flag = 0;
    saved_editable_flag = 1;
    saved_selectable_flag = 1;

    memset (&tmp_contour_props, 0, sizeof(DLContourProperties));

    tmp_contour_faults = NULL;
    num_tmp_contour_faults = 0;

    ImageXFault = NULL;
    ImageYFault = NULL;
    ImageNFaultPoints = NULL;
    ImageNFaults = 0;
    ImageNFaultTotal = 0;


}

/*
 * If the copy constructor or assignment operator
 * is called, assert for now.
 */
CDisplayList :: CDisplayList (const CDisplayList &old)
{
    if (this == &old) {
        return;
    }

    assert (0);
//    CopyFromOld (old);
}


CDisplayList &CDisplayList :: operator=(const CDisplayList &old)
{
    if (this == &old) {
        return (*this);
    }

    assert (0);
//    CopyFromOld (old);

    return (*this);
}


/*
 * Destructor.
 */
CDisplayList::~CDisplayList()
{

    csw_Free (xywork);
    csw_Free (xywork2);
    csw_Free (iwork);

    clean_frame_list ();

    csw_Free (frame_list);
    csw_Free (graph_list);
    csw_Free (layer_list);
    csw_Free (item_list);

    free_lines ();
    free_fills ();
    free_texts ();
    free_symbs ();
    free_shapes ();
    free_images ();
    free_axes ();

    grdapi_ptr->grd_FreeFaultLineStructs (tmp_contour_faults,
                              num_tmp_contour_faults);

    int i;

    if (selectable_object_list != NULL) {
        for (i=0; i<num_selectable_object_list; i++) {
            if (selectable_object_list[i] != NULL) {
                delete (selectable_object_list[i]);
            }
        }
        csw_Free (selectable_object_list);
        selectable_object_list = NULL;
        num_selectable_object_list = 0;
        max_selectable_object_list = 0;
    }

    int        sf_size = surf_list.size();
    DLSurf     **sf_data = surf_list.data();

    if (sf_data != NULL  &&  sf_size > 0) {
        for (i=0; i<sf_size; i++) {
            if (sf_data[i] != NULL) {
                delete (sf_data[i]);
            }
            sf_data[i] = NULL;
        }
        surf_list.clear();
    }

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();
    
    if (cl_data != NULL  &&  cl_size > 0) {
        DLContour     *dlc;
        for (i=0; i<cl_size; i++) {
            dlc = cl_data[i];
            if (dlc != NULL) {
                delete (dlc);
            }
        }
    }
    contour_list.clear();

    csw_Free (ImageXFault);
    csw_Free (ImageNFaultPoints);

}




/*--------------------------------------------------------------------------*/

void CDisplayList::SetPageUnitsType (int type) {
    if (type < 1  ||  type > 2) {
        type = 1;
    }
    page_units_type = type;
    return;
}


void CDisplayList::clean_frame_list (void)
{
    int             ido, i, ntot;
    FRameStruct     *frptr;

    if (num_frame_list < 1  ||  frame_list == NULL) {
        return;
    }

    for (ido=0; ido<num_frame_list; ido++) {

        frptr = frame_list + ido;

        ntot = frptr->ncol * frptr->nrow + 1;
        if (ntot <= 0) {
            continue;
        }

        if (frptr->line_index) {
            for (i=0; i<ntot; i++) {
                csw_Free (frptr->line_index[i]);
                frptr->line_index[i] = NULL;
            }
        }


        if (frptr->fill_index) {
            for (i=0; i<ntot; i++) {
                csw_Free (frptr->fill_index[i]);
                frptr->fill_index[i] = NULL;
            }
        }


        if (frptr->text_index) {
            for (i=0; i<ntot; i++) {
                csw_Free (frptr->text_index[i]);
                frptr->text_index[i] = NULL;
            }
        }


        if (frptr->symb_index) {
            for (i=0; i<ntot; i++) {
                csw_Free (frptr->symb_index[i]);
                frptr->symb_index[i] = NULL;
            }
        }


        if (frptr->shape_index) {
            for (i=0; i<ntot; i++) {
                csw_Free (frptr->shape_index[i]);
                frptr->shape_index[i] = NULL;
            }
        }


        if (frptr->contour_index) {
            for (i=0; i<ntot; i++) {
                csw_Free (frptr->contour_index[i]);
                frptr->contour_index[i] = NULL;
            }
        }

        csw_Free (frptr->line_index);
        frptr->line_index = NULL;
        csw_Free (frptr->fill_index);
        frptr->fill_index = NULL;
        csw_Free (frptr->text_index);
        frptr->text_index = NULL;
        csw_Free (frptr->symb_index);
        frptr->symb_index = NULL;
        csw_Free (frptr->shape_index);
        frptr->shape_index = NULL;
        csw_Free (frptr->contour_index);
        frptr->contour_index = NULL;

    }

    return;

}


void CDisplayList::test_frame_indexes (void *vptr)
{
    int         ntot, i;
    int         *itest = 0;
    FRameStruct *frptr;

    static const int *ip0 = 0;
    static const int *ip10 = reinterpret_cast<const int *> (10);

    frptr = (FRameStruct *)vptr;

    if (frptr == NULL) {
        return;
    }

    ntot = frptr->ncol * frptr->nrow + 1;
    if (ntot <= 0) {
        return;
    }

    if (frptr->line_index) {
        for (i=0; i<ntot; i++) {
            itest = frptr->line_index[i];
            if (itest > ip0  &&  itest < ip10) {
                printf ("bad line index %d %p\n", i, itest);
                assert (false);
            }
        }
    }

    if (frptr->fill_index) {
        for (i=0; i<ntot; i++) {
            itest = frptr->fill_index[i];
            if (itest > ip0  &&  itest < ip10) {
                printf ("bad fill index %d %p\n", i, itest);
                assert (false);
            }
        }
    }

    if (frptr->text_index) {
        for (i=0; i<ntot; i++) {
            itest = frptr->text_index[i];
            if (itest > ip0  &&  itest < ip10) {
                printf ("bad text index %d %p\n", i, itest);
                assert (false);
            }
        }
    }

    if (frptr->symb_index) {
        for (i=0; i<ntot; i++) {
            itest = frptr->symb_index[i];
            if (itest > ip0  &&  itest < ip10) {
                printf ("bad symb index %d %p\n", i, itest);
                assert (false);
            }
        }
    }

    if (frptr->shape_index) {
        for (i=0; i<ntot; i++) {
            itest = frptr->shape_index[i];
            if (itest > ip0  &&  itest < ip10) {
                printf ("bad shape index %d %p\n", i, itest);
                assert (false);
            }
        }
    }

    if (frptr->contour_index) {
        for (i=0; i<ntot; i++) {
            itest = frptr->contour_index[i];
            if (itest > ip0  &&  itest < ip10) {
                printf ("bad contour index %d %p\n", i, itest);
                assert (false);
            }
        }
    }

    printf ("no bad pointers found in indexes\n\n");
    fflush (stdout);

    return;

}


/*
 ****************************************************************

                      D r a w

 ****************************************************************

  Draw all the primitives in the display list.

*/

void CDisplayList::Draw (FILE *dfile_in)
{
    dfile = dfile_in;
    Draw ();
}

void CDisplayList::Draw (void)
{
    CSW_F            xmin, ymin, xmax, ymax;
    double           dx, dy;
    double           xminsav, yminsav, xmaxsav, ymaxsav;
    int              i, save_fnum;
    FRameStruct      *frptr;

/*
 * Save the clip limits and reset back to them
 * at the end of the function.
 */
    xminsav = clip_xmin;
    yminsav = clip_ymin;
    xmaxsav = clip_xmax;
    ymaxsav = clip_ymax;

/*
 * If page clipping limits have not been set, use
 * the overall limits of all drawn primitives.
 */
    if (clip_xmin >= clip_xmax  ||
        clip_ymin >= clip_ymax) {
        if (frame_layout_needed) {
            page_xmin = raw_page_xmin;
            page_ymin = raw_page_ymin;
            page_xmax = raw_page_xmax;
            page_ymax = raw_page_ymax;
            dx = (page_xmax - page_xmin) / 25.0;
            dy = (page_ymax - page_ymin) / 25.0;
            if (spline_used_flag) {
                dx *= 2.0;
                dy *= 2.0;
            }
            clip_xmin = page_xmin - dx;
            clip_ymin = page_ymin - dy;
            clip_xmax = page_xmax + dx;
            clip_ymax = page_ymax + dy;
        }
        else {
            clip_xmin = page_xmin;
            clip_ymin = page_ymin;
            clip_xmax = page_xmax;
            clip_ymax = page_ymax;
        }
    }

    CSW_F   lcxmin, lcxmax, lcymin, lcymax;

    lcxmin = page_xmin;
    lcymin = page_ymin;
    lcxmax = page_xmax;
    lcymax = page_ymax;

/*
 * If the overall limits have not been set, there is
 * nothing to draw.
 */
    if (clip_xmin >= clip_xmax  ||
        clip_ymin >= clip_ymax) {
        return;
    }

    if (page_units_type == 2) {
        clip_xmin = 0;
        clip_ymin = 0;
        clip_xmax = screen_xmax;
        clip_ymax = screen_ymax;
    }

    gtx_drawprim_obj.gtx_init_drawing
                     ((CSW_F)clip_xmin, (CSW_F)clip_ymin,
                      (CSW_F)clip_xmax, (CSW_F)clip_ymax,
                      lcxmin, lcymin, lcxmax, lcymax, 
                      (CSW_F)screen_xmin, (CSW_F)screen_ymin,
                      (CSW_F)screen_xmax, (CSW_F)screen_ymax,
                      screen_dpi, page_units_type, dfile);
    gtx_drawprim_obj.gtx_get_drawing_clip_limits (&xmin, &ymin, &xmax, &ymax);
    gtx_drawprim_obj.gtx_get_page_units_per_inch (&page_units_per_inch);

    clip_xmin = (double)xmin;
    clip_xmax = (double)xmax;
    clip_ymin = (double)ymin;
    clip_ymax = (double)ymax;

/*
 * Recalculate trimesh and grid contours and color fills
 * if needed.
 */
    recalc_surfaces ();

/*
 * Layout the frames and readjust the scaling for the frame layout.
 */
    if (frame_layout_needed) {
        calc_frame_layout ();
        frame_layout_needed = 0;

        if (frame_list != NULL) {
            for (i=0; i<num_frame_list; i++) {
                frptr = frame_list + i;
                if (frptr->rescale_needed == 1) {
                    continue;
                }

                save_fnum = current_frame_num;
                current_frame_num = i;
                update_frame_limits ();

                patch_draw_flag = frptr->patch_draw_flag;

                reclip_frame_contours (i);
                delete_frame_grid_images (i);
                reclip_frame_grid_images (i);

                current_frame_num = save_fnum;
                update_frame_limits ();
            }
        }
    }

/*
 * If any frames need to be rescaled, do it now.
 */
    if (frame_list != NULL) {
        for (i=0; i<num_frame_list; i++) {
            frptr = frame_list + i;
            if (frptr->rescale_needed == 1) {
                rescale_frame (frptr);
            }
            frptr->rescale_needed = 0;
        }
    }

/*
 * If any frames need to be rebordered, do it now.
 */
    if (frame_list != NULL) {
        for (i=0; i<num_frame_list; i++) {
            frptr = frame_list + i;
            if (frptr->reborder_needed == 1) {
                reborder_frame (frptr);
            }
            frptr->reborder_needed = 0;
        }
    }

/*
 * Draw axes internal to the frames.
 */
    if (frame_list != NULL) {
        for (i=0; i<num_frame_list; i++) {
            frptr = frame_list + i;
            reaxis_frame (frptr);
        }
    }

/*
 * Now that everything has been rescaled and rebordered,
 * draw the primitives.
 */
    gtx_drawprim_obj.gtx_set_selected_flag_for_drawing (0);
    ezx_java_obj.ezx_SetAlphaValue (255);
    DrawFrameBorders ();
    DrawAllFills ();
    DrawAllLines ();
    DrawAllContourLines ();
    DrawAllTexts ();
    ezx_java_obj.ezx_SetDrawingPriority (1);
    DrawAllSymbs ();
    ezx_java_obj.ezx_SetDrawingPriority (0);

    DrawAllShapes ();
    DrawAllImages ();

    DrawSelected ();

    clip_xmin = xminsav;
    clip_ymin = yminsav;
    clip_xmax = xmaxsav;
    clip_ymax = ymaxsav;

    return;
}



/*
 ****************************************************************

          S e t D r a w i n g B o u n d s H i n t

 ****************************************************************

  Set the approximate boundaries of the drawing, in page units.

*/
int CDisplayList::SetDrawingBoundsHint (double page_xmin_in,
                                        double page_ymin_in,
                                        double page_xmax_in,
                                        double page_ymax_in)
{

/*
 * Assert if the bounds are screwed up in the call.
 */
    if (page_xmax_in <= page_xmin_in  ||
        page_ymax_in <= page_ymin_in) {
        assert (0);
    }

/*
 * The spatial indexing is changed to a frame by frame basis.
 * Thus, do not create these arrays here.  The min and max values
 * are assigned for use in calculating tolerances, etc.
 */
    page_index_xmin = page_xmin_in;
    page_index_ymin = page_ymin_in;
    page_index_xmax = page_xmax_in;
    page_index_ymax = page_ymax_in;

    if (page_units_per_inch > 1.e20) {
        page_units_per_inch = (CSW_F)(page_xmax_in - page_xmin_in +
                               page_ymax_in - page_ymin_in) / 20.0f;
    }

    Px1 = (CSW_F)page_xmin_in;
    Py1 = (CSW_F)page_ymin_in;
    Px2 = (CSW_F)page_xmax_in;
    Py2 = (CSW_F)page_ymax_in;

    raw_page_xmin = Px1;
    raw_page_ymin = Py1;
    raw_page_xmax = Px2;
    raw_page_ymax = Py2;

    raw_from_hints_flag = 1;

    return 1;

}


void CDisplayList::SetScreenDPI (int ival) {
    if (ival < 25) {
        ival = 72;
    }

    if (ival > 10000) {
        ival = 72;
    }

    screen_dpi = ival;
}


/*
 *********************************************************************

             S e t S c r e e n B o u n d s

 *********************************************************************

  Set the size of the screen window, in screen units.  The size is
  defined with the lower left and upper right points of the area.
  These values are used at redraw time to scale from page coords
  to screen coords.
*/
void CDisplayList::SetScreenBounds (double x_upper_left,
                                    double y_upper_left,
                                    double x_lower_right,
                                    double y_lower_right)
{

    if (screen_xmin - x_upper_left  != 0.0  ||
        screen_ymin - y_upper_left  != 0.0  ||
        screen_xmax - x_lower_right  != 0.0  ||
        screen_ymax - y_lower_right  != 0.0) {
        frame_layout_needed = 1;
        screen_was_resized = 1;
    }

    screen_xmin = x_upper_left;
    screen_ymin = y_upper_left;
    screen_xmax = x_lower_right;
    screen_ymax = y_lower_right;

    return;
}

/*
 ********************************************************************

                 G e t D r a w i n g B o u n d s H i n t

 ********************************************************************

*/
void CDisplayList::GetDrawingBoundsHint (double *xmin,
                                         double *ymin,
                                         double *xmax,
                                         double *ymax)
{
    *xmin = index_xmin;
    *ymin = index_ymin;
    *xmax = index_xmax;
    *ymax = index_ymax;

    return;
}

/*
 ******************************************************************

                   G e t D r a w i n g B o u n d s

 ******************************************************************

*/
void CDisplayList::GetDrawingBounds (double *xmin,
                                     double *ymin,
                                     double *xmax,
                                     double *ymax)
{
    *xmin = page_xmin;
    *ymin = page_ymin;
    *xmax = page_xmax;
    *ymax = page_ymax;

    return;
}



/*
 ******************************************************************

                   G e t S c r e e n B o u n d s

 ******************************************************************

*/
void CDisplayList::GetScreenBounds (double *xmin,
                                    double *ymin,
                                    double *xmax,
                                    double *ymax)
{
    *xmin = screen_xmin;
    *ymin = screen_ymin;
    *xmax = screen_xmax;
    *ymax = screen_ymax;

    return;
}


/*
 * This is used to set a "global" clip region (i.e.
 * from a zoom in or zoom out of the whole picture)
 */
void CDisplayList::SetPageClipLimits (double x1, double y1t,
                                      double x2, double y2)
{
    double         xmin, ymin, xmax, ymax;

    xmin = x1;
    if (x2 < x1) xmin = x2;
    ymin = y1t;
    if (y2 < y1t) ymin = y2;

    xmax = x2;
    if (x1 > x2) xmax = x1;
    ymax = y2;
    if (y1t > y2) ymax = y1t;

    clip_xmin = xmin;
    clip_ymin = ymin;
    clip_xmax = xmax;
    clip_ymax = ymax;

    return;
}


/*
 * Set the clip limits by passing screen coordinates, which are
 * converted to page given the current caling factors.
 */
void CDisplayList::SetScreenClipLimits (double sx1, double sy1,
                                        double sx2, double sy2)
{
    double          px1, py1, px2, py2;
    double          swidth, sheight, pwidth, pheight;
    double          xpct, ypct;

    if (screen_xmin >= screen_xmax  ||
        screen_ymin >= screen_ymax) {
        return;
    }

    if (clip_xmin >= clip_xmax  ||
        clip_ymin >= clip_ymax) {
        return;
    }

    swidth = screen_xmax - screen_xmin;
    sheight = screen_ymax - screen_ymin;
    pwidth = clip_xmax - clip_xmin;
    pheight = clip_ymax - clip_ymin;

    xpct = (sx1 - screen_xmin) / swidth;
    px1 = clip_xmin + xpct * pwidth;
    xpct = (sx2 - screen_xmin) / swidth;
    px2 = clip_xmin + xpct * pwidth;

    ypct = (sy1 - screen_ymin) / sheight;
    py1 = clip_ymin + ypct * pheight;
    ypct = (sy2 - screen_ymin) / sheight;
    py2 = clip_ymin + ypct * pheight;

    SetPageClipLimits (px1, py1, px2, py2);

    return;

}


/*
******************************************************************

                          A d d L i n e

******************************************************************

  function name:    AddLine

  purpose:          set up line primitive in the graphics display system
                    As of the 2017 refactor, if a frame is current, the
                    frame x and y are put into the display list, and the
                    lineprim frame_num is set to >= 0 (the index into the
                    framelist).  No conversion to page units is done here.
                    It is better to convert to page units at draw time.

  return value:     1 on success, -1 on a memory allocation failure
                    A return value of zero means a calling parameter
                    was bad.

  calling parameters:

    xptsin          r    double*   Array of x coordinates
    yptsin          r    double*   Array of y coordinates.
    npts            r    int       Number of points.

*/

int CDisplayList::AddLine (double *xptsin, double *yptsin, int npts)
{
    int  istat =
      AddLine (xptsin, yptsin, npts, false);
    return istat;
}


int CDisplayList::AddLine (double *xptsin, double *yptsin,
                           int npts, bool from_graph)
{
    CSW_F          *xyp = NULL;
    double         xmin, ymin, xmax, ymax;
    LInePrim       *lptr = NULL;
    int            istat;
    CSW_F          *xypack = NULL;
    int            next_line;

    int            local_frame_num = -1;
    int            local_scaleable = 0;


    auto fscope = [&]()
    {
        csw_Free (xypack);
    };
    CSWScopeGuard  func_scope_guard (fscope);


/*
 * check obvious errors in parameters.
 */
    if (npts < 2) {
        return 0;
    }
    if (xptsin == NULL  ||  yptsin == NULL) {
        return 0;
    }

    if (xywork == NULL  ||
        xywork2 == NULL  ||
        iwork == NULL) {
        return 0;
    }

    if (npts > _DL_MAX_WORK_) npts = _DL_MAX_WORK_;

    next_line = get_available_line ();

    if (next_line < 0) {
        try {
            SNF;
            LInePrim  lpr;
            ZeroInit (&lpr, sizeof(lpr));
            line_prim_list.push_back (lpr);
            next_line = (int)line_prim_list.size() - 1;
        }
        catch (...) {
            printf ("Exception in line_prim_list pushback\n");
            return 0;
        }
    }

/*
 * Pack points into the line structure format
 */
    xypack = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
    if (xypack == NULL) {
        return -1;
    }
    istat = gpf_packxy2 (xptsin, yptsin, npts,
                         xypack);
    if (istat != 1) {
        return -1;
    }

    if (from_graph) {
        local_frame_num = current_frame_num;
        local_scaleable = 1;
    }
    else {
        if (current_frame_num >= 0  &&
            current_frame_num < num_frame_list  &&
            frame_list != NULL  &&
            current_axis_num == -1) {
            local_frame_num = current_frame_num;
            local_scaleable = 1;
        }
    }

/*
 * Copy the xy coordinates as specified (without conversion to page units).
 * The lptr->frame_num tells the drawing code to convert to page units if needed.
 */
    LInePrim   *lp_data = line_prim_list.data();

    lptr = lp_data + next_line;

    xyp = (CSW_F *)csw_Malloc (2 * npts * sizeof(CSW_F));
    if (xyp == NULL) {
        return -1;
    }
    memcpy (xyp, xypack, npts * 2 * sizeof(CSW_F));

    lptr->xypts = xyp;
    lptr->npts = npts;
    lptr->maxpts = npts;

    lptr->thick = current_line_thickness;
    lptr->dashpat = current_line_pattern;
    lptr->dashscale = current_line_pattern_size;
    lptr->symbol = current_line_symbol;

    lptr->red = current_line_red;
    lptr->green = current_line_green;
    lptr->blue = current_line_blue;
    lptr->alpha = current_line_alpha;

    lptr->grid_num = current_grid_num;
    lptr->border_num = current_border_num;
    lptr->axis_num = current_axis_num;
    lptr->frame_num = local_frame_num;
    lptr->graph_num = current_graph_num;
    lptr->selectable_object_num = current_selectable_object_num;
    lptr->xaxis_num = current_xaxis_num;
    lptr->yaxis_num = current_yaxis_num;

    lptr->layer_num = current_layer_num;
    lptr->item_num = current_item_num;
    lptr->arrowstyle = current_line_arrow_style;

    lptr->smooth_flag = (char)current_line_smooth_flag;
    lptr->editable_flag = (char)current_editable_flag;
    lptr->selectable_flag = (char)current_selectable_flag;

    lptr->visible_flag = 1;
    lptr->draw_flag = 1;
    lptr->plot_flag = 1;
    lptr->selected_flag = 0;
    lptr->deleted_flag = 0;

    lptr->image_id = ImageID;
    lptr->contour_index = -1;

    lptr->scaleable = local_scaleable;

    lptr->prim_num = next_line;

    CalcLineBounds (next_line,
                    &xmin, &ymin, &xmax, &ymax);
    lptr->xmin = (CSW_F)xmin;
    lptr->ymin = (CSW_F)ymin;
    lptr->xmax = (CSW_F)xmax;
    lptr->ymax = (CSW_F)ymax;

    SetSpatialIndexForLine (next_line);

    if (current_selectable_object) {
        current_selectable_object->AddLine (next_line);
    }

    return 1;

}  /*  end of function AddLine  */



/*
 *****************************************************************

              D r a w A l l L i n e s

 *****************************************************************

  Draw all the lines in the display list.
*/

int CDisplayList::DrawAllLines (void)
{
    int            i, ido, nloop;
    LInePrim       *lptr;
    bool           bpatch = false;

    if ((int)line_prim_list.size() < 1) {
        return 0;
    }

    int       pt_size = (int)line_patch_list.size();
    int       *pt_data = line_patch_list.data();

    nloop = (int)line_prim_list.size();
    if (pt_data != NULL  &&  pt_size > 0  &&
        patch_draw_flag == 1) {
        nloop = pt_size;
        bpatch = true;
    }

    bool  bframe = false;

    for (ido=0; ido<nloop; ido++) {

        i = ido;
        if (bpatch) {
            i = pt_data[ido];
        }

        lptr = line_prim_list.data() + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }
        if (lptr->visible_flag == 0) {
            continue;
        }

    /*
     * If the primitive is from a border, do not draw it here.
     */
        if (lptr->border_num >= 0) {
            continue;
        }

        if (lptr->axis_num >= 0) {
            if (lptr->frame_num < 0) {
                continue;
            }
        }

    /*
     * Check the line bbox against the line frame.
     */
        bframe = false;
        if (lptr->frame_num >= 0) {
            bool bbchk = CheckInsideFrame (lptr->frame_num,
                                           lptr->xmin, lptr->ymin,
                                           lptr->xmax, lptr->ymax);
            if (!bbchk) {
                continue;
            }
            convert_frame_array (lptr->frame_num, lptr->xypts, lptr->npts);
            bframe = true;
        }

        ezx_java_obj.ezx_SetFrameInJavaArea (lptr->frame_num);
        ezx_java_obj.ezx_SetAlphaValue (lptr->alpha);

        gtx_drawprim_obj.gtx_SetImageIDForDraw (lptr->image_id);


        gtx_drawprim_obj.gtx_cliplineprim
                        (lptr->xypts,
                         lptr->npts,
                         lptr->smooth_flag,
                         (CSW_F)lptr->thick,
                         lptr->red,
                         lptr->green,
                         lptr->blue,
                         lptr->dashpat,
                         (CSW_F)lptr->dashscale,
                         lptr->arrowstyle);

        gtx_drawprim_obj.gtx_SetImageIDForDraw (-1);

        if (bframe) {
            unconvert_frame_array (lptr->frame_num, lptr->xypts, lptr->npts);
        }

    }

    return 0;

}  /*  end of function DrawAllLines */



/*
******************************************************************

       S e t S p a t i a l I n d e x F o r C o n t o u r

******************************************************************

  Get the contour points and set the spatial
  index grid to point to this contour number.
*/

int CDisplayList::SetSpatialIndexForContour (int contour_num)
{
    CSW_F           *xpts, *ypts;
    int             npts, istat, i;
    DLContour       *cptr;
    double          x1, y1t, x2, y2;
    FRameStruct     *fp;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return 0;
    }
    if (contour_num < 0  ||  contour_num >= cl_size) {
        return 0;
    }

    cptr = cl_data[contour_num];
    if (cptr == NULL) {
        return 0;
    }

    istat = SetupSpatialIndexForFrame (cptr->frame_num);
    if (istat == -1) {
        return 0;
    }

    cptr->GetContourPoints (&xpts, &ypts, &npts);

    if (xpts == NULL  ||  ypts == NULL  ||  npts < 2) {
        return 0;
    }

    for (i=0; i<npts-1; i++) {
        x1 = xpts[i];
        x2 = xpts[i+1];
        y1t = ypts[i];
        y2 = ypts[i+1];
        istat = set_grid_for_contour_vec (x1, y1t, x2, y2, contour_num);
        if (istat == -1) {
            return -1;
        }
    }

    if (frame_list != NULL  &&  num_frame_list > 0) {
        fp = frame_list + cptr->frame_num;
        fp->num_contour_index++;
    }

    return 1;

}  /*  end of function SetSpatialIndexForContour  */


/*
******************************************************************

          S e t S p a t i a l I n d e x F o r L i n e

******************************************************************

  Get the line points (or use those specified) and set the spatial
  index grid to point to this line prim number.
*/

int CDisplayList::SetSpatialIndexForLine (int line_prim_num)
{
    CSW_F           *xypts, *xyp;
    int             npts, istat, i, nout, maxpts;
    LInePrim        *lptr;
    double          x1, y1t, x2, y2;
    FRameStruct     *fp;

    if ((int)line_prim_list.size() < 1) {
        return 0;
    }
    if (line_prim_num < 0  ||  line_prim_num >= (int)line_prim_list.size()) {
        return 0;
    }

    lptr = line_prim_list.data() + line_prim_num;
    if (lptr->frame_num < 0) {
        return 0;
    }

    npts = lptr->npts;
    xypts = lptr->xypts;

    istat = SetupSpatialIndexForFrame (lptr->frame_num);
    if (istat == -1) {
        return 0;
    }

    if (xypts == NULL  ||  npts < 2) {
        return 0;
    }

    if (lptr->smooth_flag <= 0) {
        xyp = xypts;
    }
    else {
        if (xywork == NULL) {
            return 0;
        }
        maxpts = npts * lptr->smooth_flag;
        if (maxpts > _DL_MAX_WORK_) maxpts = _DL_MAX_WORK_;
        istat = gpf_spline_obj.gpf_SplineFit2 (xypts, npts, 0,
                                xywork, maxpts, 0.0,
                                &nout);
        if (istat == -1) {
            xyp = xypts;
        }
        else {
            xyp = xywork;
            npts = nout;
        }
    }

    for (i=0; i<npts-1; i++) {
        x1 = *xyp;
        x2 = *(xyp+2);
        xyp++;
        y1t = *xyp;
        y2 = *(xyp+2);
        xyp++;
        istat = set_grid_for_vec (x1, y1t, x2, y2, lptr);
        if (istat == -1) {
            return -1;
        }
    }

    if (frame_list != NULL  &&  num_frame_list > 0) {
        fp = frame_list + lptr->frame_num;
        fp->num_line_index++;
    }

    return 1;

}  /*  end of function SetSpatialIndexforContour  */





int CDisplayList::set_grid_for_vec (double x1, double y1p, double x2, double y2,
                                    LInePrim *prim)

{
    double            bint, slope, x, y, dx, tiny;
    int               row1, row2, col1, col2, itmp, offset, c1, c2, c2sav, i, j;
    int               *cell_list, num_cell, max_cell;

/*
    if there is a hole flag on either end of the vector,
    return with no processing
*/
    if (x1 > _DL_XY_TOO_BIG_  ||  x1 < -_DL_XY_TOO_BIG_  ||
        x2 > _DL_XY_TOO_BIG_  ||  x2 < -_DL_XY_TOO_BIG_  ||
        y1p > _DL_XY_TOO_BIG_  ||  y1p < -_DL_XY_TOO_BIG_  ||
        y2 > _DL_XY_TOO_BIG_  ||  y2 < -_DL_XY_TOO_BIG_) {
        return 1;
    }

/*
 * Do nothing if the line spatial index is not set up yet.
 */
    if (line_spatial_index == NULL) {
        return -1;
    }

/*  return if either end of the vector is outside of the grid limits  */;

    if (x2 < index_xmin  ||  x2 > index_xmax) {
        return (set_in_extra (prim));
    }

    if (x1 < index_xmin  ||  x1 > index_xmax) {
        return (set_in_extra (prim));
    }

    if (y2 < index_ymin  ||  y2 > index_ymax) {
        return (set_in_extra (prim));
    }

    if (y1p < index_ymin  ||  y1p > index_ymax) {
        return (set_in_extra (prim));
    }

/*  calculate slope and intercept if non vertical  */

    slope = 1.e30;
    bint = 1.e30;
    tiny = index_xspace / 100.0;
    dx = x2 - x1;
    if (dx < 0.0) dx = -dx;

    if (dx > tiny) {
        slope = (y1p-y2) / (x1-x2);
        bint = y1p - slope * x1;
    }

/*  grid start and end rows and columns  */

    row1 = (int)((y1p-index_ymin) / index_yspace);
    row2 = (int)((y2-index_ymin) / index_yspace);
    if (row1 > row2) {
        i = row1;
        row1 = row2;
        row2 = i;
    }
    col1 = (int)((x1-index_xmin) / index_xspace);
    col2 = (int)((x2-index_xmin) / index_xspace);
    if (col1 > col2) {
        i = col1;
        col1 = col2;
        col2 = i;
    }

/*
 * Each cell in the spatial index array has a list of line primitive
 * numbers that intersect the cell.  If the list is null, no lines have
 * yet intersected the cell.  The first element in each list is the
 * current maximum size of the list.  The second element is the current
 * total number of primitive numbers in the list.
 */

/*  special case for near horizontal vector with endpoints in the same row  */

    if (row1 == row2 ) {

        if (row1 < 0  ||  row1 >= index_nrow) {
            return (set_in_extra (prim));
        }

        if (col1 > col2) {
            itmp = col1;
            col1 = col2;
            col2 = itmp;
        }

        if (col2 < 0) {
            return (set_in_extra (prim));
        }
        if (col1 >= index_ncol) {
            return (set_in_extra (prim));
        }
        if (col1 < 0) {
            col1 = 0;
        }
        if (col2 >= index_ncol) {
            col2 = index_ncol-1;
        }

        offset = row1 * index_ncol;

        for (i=col1; i<=col2; i++) {

            cell_list = line_spatial_index[offset+i];
            if (cell_list == NULL) {
                cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
                if (cell_list == NULL) {
                    return -1;
                }
                cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
                cell_list[1] = 0;
            }
            max_cell = cell_list[0];
            num_cell = cell_list[1];
            if (num_cell >= max_cell - 2) {
                max_cell += _SMALL_CHUNK_SIZE_;
                cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
                if (cell_list) {
                    cell_list[0] += _SMALL_CHUNK_SIZE_;
                }
            }
            if (cell_list == NULL) {
                return -1;
            }

            if (num_cell > 0  &&  cell_list[num_cell+1] == prim->prim_num) {
                num_cell = num_cell;
            }
            else {
                cell_list[num_cell+2] = prim->prim_num;
                cell_list[1]++;
            }

            line_spatial_index[offset+i] = cell_list;

        }
    }

/*  special case for near vertical vector in only one column  */

    else if (col1 == col2) {

        if (row1 > row2) {
            itmp = row1;
            row1 = row2;
            row2 = itmp;
        }

        if (row2 < 0) {
            return (set_in_extra (prim));
        }
        if (row1 >= index_nrow) {
            return (set_in_extra (prim));
        }
        if (row1 < 0) {
            row1 = 0;
        }
        if (row2 >= index_nrow) {
            row2 = index_nrow-1;
        }

        for (i=row1; i<=row2; i++) {
            offset = i * index_ncol;
            cell_list = line_spatial_index[offset+col1];
            if (cell_list == NULL) {
                cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
                if (cell_list == NULL) {
                    return -1;
                }
                cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
                cell_list[1] = 0;
            }
            max_cell = cell_list[0];
            num_cell = cell_list[1];
            if (num_cell >= max_cell - 2) {
                max_cell += _SMALL_CHUNK_SIZE_;
                cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
                if (cell_list) {
                    cell_list[0] += _SMALL_CHUNK_SIZE_;
                }
            }
            if (cell_list == NULL) {
                return -1;
            }

            if (num_cell > 0  &&  cell_list[num_cell+1] == prim->prim_num) {
                num_cell = num_cell;
            }
            else {
                cell_list[num_cell+2] = prim->prim_num;
                cell_list[1]++;
            }

            line_spatial_index[offset+col1] = cell_list;

        }
    }

/*  general case where the vector crosses rows and columns  */

    else {

        if (row2 < 0) {
            return (set_in_extra (prim));
        }
        if (row1 >= index_nrow) {
            return (set_in_extra (prim));
        }

        y = y1p;
        if (y2 < y1p) y = y2;
        x = (y - bint) / slope;
        c1 = (int)((x - index_xmin) / index_xspace);

        if (row1 < 0) {
            row1 = 0;
        }
        if (row2 >= index_nrow) {
            row2 = index_nrow-1;
        }

        for (i=row1; i<=row2; i++) {
            y = index_ymin + (i+1) * index_yspace;
            if (i == row2) {
                y = y2;
                if (y1p > y2) y = y1p;
            }
            x = (y - bint) / slope;
            c2 = (int)((x - index_xmin) / index_xspace);
            c2sav = c2;
            if (c1 > c2) {
                itmp = c1;
                c1 = c2;
                c2 = itmp;
            }
            if (c1 < 0) {
                c1 = 0;
            }
            if (c2 >= index_ncol) {
                c2 = index_ncol-1;
            }

            offset = i * index_ncol;

            for (j=c1; j<=c2; j++) {
                cell_list = line_spatial_index[offset+j];
                if (cell_list == NULL) {
                    cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
                    if (cell_list == NULL) {
                        return -1;
                    }
                    cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
                    cell_list[1] = 0;
                }
                max_cell = cell_list[0];
                num_cell = cell_list[1];
                if (num_cell >= max_cell - 2) {
                    max_cell += _SMALL_CHUNK_SIZE_;
                    cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
                    if (cell_list) {
                        cell_list[0] += _SMALL_CHUNK_SIZE_;
                    }
                }
                if (cell_list == NULL) {
                    return -1;
                }

                if (num_cell > 0  &&  cell_list[num_cell+1] == prim->prim_num) {
                    num_cell = num_cell;
                }
                else {
                    cell_list[num_cell+2] = prim->prim_num;
                    cell_list[1]++;
                }

                line_spatial_index[offset+j] = cell_list;

            }
            c1 = c2sav;
        }
    }

    if (x1 < index_xmin) {
        return (set_in_extra (prim));
    }

    if (x2 > index_xmax) {
        return (set_in_extra (prim));
    }

    if (y1p < index_ymin) {
        return (set_in_extra (prim));
    }

    if (y2 > index_ymax) {
        return (set_in_extra (prim));
    }

    return 0 ;

}  /*  end of function set_grid_for_vec  */






int CDisplayList::set_in_extra (LInePrim *prim)

{
    int                offset;
    int                *cell_list, num_cell, max_cell;

    if (prim->in_extra_flag == 1) {
        return 1;
    }

/*
 * Do nothing if the line spatial index is not set up yet.
 */
    if (line_spatial_index == NULL) {
        return -1;
    }

    offset = index_ncol * index_nrow;

    cell_list = line_spatial_index[offset];
    if (cell_list == NULL) {
        cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
        if (cell_list == NULL) {
            return -1;
        }
        cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
        cell_list[1] = 0;
    }
    max_cell = cell_list[0];
    num_cell = cell_list[1];
    if (num_cell >= max_cell - 2) {
        max_cell += _SMALL_CHUNK_SIZE_;
        cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
        if (cell_list) {
            cell_list[0] += _SMALL_CHUNK_SIZE_;
        }
    }
    if (cell_list == NULL) {
        return -1;
    }

    prim->in_extra_flag = 1;

    cell_list[num_cell+2] = prim->prim_num;
    cell_list[1]++;

    line_spatial_index[offset] = cell_list;

    return 0 ;

}  /*  end of private set_in_extra function  */




/*
  ****************************************************************

                    C a l c L i n e B o u n d s

  ****************************************************************

*/

int CDisplayList::CalcLineBounds (int line_prim_num,
                                  double *xmin, double *ymin,
                                  double *xmax, double *ymax)

{
    int             i;
    CSW_F           *xypts, *xyp;
    int             npts;
    LInePrim        *lptr;
    int             nsout;
    CSW_F           xysmooth[2200];

    *xmin = 1.e30;
    *ymin = 1.e30;
    *xmax = -1.e30;
    *ymax = -1.e30;

    if ((int)line_prim_list.size() < 1) {
        return -1;
    }

    if (line_prim_num < 0  ||  line_prim_num >= (int)line_prim_list.size()) {
        return -1;
    }

    lptr = line_prim_list.data() + line_prim_num;

    npts = lptr->npts;
    xypts = lptr->xypts;

    if (npts < 1  ||  xypts == NULL) {
        return -1;
    }

    if (lptr->smooth_flag > 0) {
        gpf_spline_obj.gpf_SplineFit2 (xypts, npts, 0,
                        xysmooth, 1000,
                        -1.0f, &nsout);
        xyp = xysmooth;
        npts = nsout;
    }
    else {
        xyp = xypts;
    }

    for (i=0; i<npts; i++) {
        if (*xyp < *xmin) *xmin = *xyp;
        if (*xyp > *xmax) *xmax = *xyp;
        xyp++;
        if (*xyp < *ymin) *ymin = *xyp;
        if (*xyp > *ymax) *ymax = *xyp;
        xyp++;
    }

    return 1;

}  /*  end of function CalcLineBounds  */




/*
 * private methods for managing the available primitive lists.
 */
void CDisplayList::free_available_lists (void)
{
    contour_available_list.clear();
    contour_line_available_list.clear();
    line_available_list.clear();
    text_available_list.clear();
    fill_available_list.clear();
    symb_available_list.clear();
    image_available_list.clear();
    shape_available_list.clear();
    axis_available_list.clear();

    return;

}


int CDisplayList::get_available_contour (void) {

    int        cv_size = contour_available_list.size();
    int        *cv_data = contour_available_list.data();

    if (cv_data == NULL  ||  cv_size < 1) {
        return -1;
    }

    int ival = contour_available_list.back();
    contour_available_list.pop_back();

    return ival;

}


int CDisplayList::add_available_contour (int prim_num) {

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (prim_num < 0  ||  prim_num >= cl_size  ||  cl_data == NULL) {
        return 0;
    }

    try {
        contour_available_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::get_available_contour_line (void) {

    int        cv_size = contour_line_available_list.size();
    int        *cv_data = contour_line_available_list.data();

    if (cv_data == NULL  ||  cv_size < 1) {
        return -1;
    }

    int ival = contour_line_available_list.back();
    contour_line_available_list.pop_back();

    return ival;

}

int CDisplayList::add_available_contour_line (int prim_num) {

    int            cp_size = (int)contour_line_prim_list.size();
    LInePrim       *cp_data = contour_line_prim_list.data();

    if (prim_num < 0  ||  prim_num >= cp_size ||  cp_data == NULL) {
        return 0;
    }

    try {
        contour_line_available_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}



int CDisplayList::add_available_line (int prim_num) {

    int        lp_size = (int)line_prim_list.size();
    LInePrim   *lp_data = line_prim_list.data();

    if (lp_data == NULL  ||  lp_size < 1) {
        return 0;
    }

    if (prim_num < 0  ||  prim_num >= lp_size) {
        return 0;
    }

    try {
        line_available_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::get_available_line (void) {

    int        cv_size = line_available_list.size();
    int        *cv_data = line_available_list.data();

    if (cv_data == NULL  ||  cv_size < 1) {
        return -1;
    }

    int ival = line_available_list.back();
    line_available_list.pop_back();

    return ival;

}


int CDisplayList::add_available_fill (int prim_num) {

    int          fp_size = (int)fill_prim_list.size();
    FIllPrim     *fp_data = fill_prim_list.data();

    if (prim_num < 0  ||  prim_num >= fp_size  ||  fp_data == NULL) {
        return 0;
    }

    try {
        fill_available_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::get_available_fill (void) {

    int        cv_size = fill_available_list.size();
    int        *cv_data = fill_available_list.data();

    if (cv_data == NULL  ||  cv_size < 1) {
        return -1;
    }

    int ival = fill_available_list.back();
    fill_available_list.pop_back();

    return ival;

}


int CDisplayList::add_available_text (int prim_num) {

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (prim_num < 0  ||  prim_num >= tp_size  ||  tp_data == NULL) {
        return 0;
    }

    try {
        text_available_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::get_available_text (void) {

    int        cv_size = text_available_list.size();
    int        *cv_data = text_available_list.data();

    if (cv_data == NULL  ||  cv_size < 1) {
        return -1;
    }

    int ival = text_available_list.back();
    text_available_list.pop_back();

    return ival;

}


int CDisplayList::add_available_symb (int prim_num) {

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (prim_num < 0  ||  prim_num >= sp_size  ||  sp_data == NULL) {
        return 0;
    }

    try {
        symb_available_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::get_available_symb (void) {

    int        cv_size = symb_available_list.size();
    int        *cv_data = symb_available_list.data();

    if (cv_data == NULL  ||  cv_size < 1) {
        return -1;
    }

    int ival = symb_available_list.back();
    symb_available_list.pop_back();

    return ival;

}


int CDisplayList::add_available_shape (int prim_num) {

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (prim_num < 0  ||  prim_num >= hp_size  ||  hp_data == NULL) {
        return 0;
    }

    try {
        shape_available_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::get_available_shape (void) {

    int        cv_size = shape_available_list.size();
    int        *cv_data = shape_available_list.data();

    if (cv_data == NULL  ||  cv_size < 1) {
        return -1;
    }

    int ival = shape_available_list.back();
    shape_available_list.pop_back();

    return ival;

}


int CDisplayList::add_available_axis (int prim_num) {

    int          ap_size = (int)axis_prim_list.size();
    AXisPrim     *ap_data = axis_prim_list.data();

    if (prim_num < 0  ||  prim_num >= ap_size  ||  ap_data == NULL) {
        return 0;
    }

    try {
        axis_available_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::get_available_axis (void) {

    int        cv_size = axis_available_list.size();
    int        *cv_data = axis_available_list.data();

    if (cv_data == NULL  ||  cv_size < 1) {
        return -1;
    }

    int ival = axis_available_list.back();
    axis_available_list.pop_back();

    return ival;

}


int CDisplayList::add_available_image (int prim_num) {

    int            ip_size = (int)image_prim_list.size();
    IMagePrim      *ip_data = image_prim_list.data();

    if (prim_num < 0  ||  prim_num >= ip_size  ||  ip_data == NULL) {
        return 0;
    }

    try {
        image_available_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::get_available_image (void) {

    int        cv_size = image_available_list.size();
    int        *cv_data = image_available_list.data();

    if (cv_data == NULL  ||  cv_size < 1) {
        return -1;
    }

    int ival = image_available_list.back();
    image_available_list.pop_back();

    return ival;

}


/*
 * private methods for managing the patch primitive lists.
 */
void CDisplayList::free_patch_lists (void)
{
    line_patch_list.clear();
    contour_line_patch_list.clear();
    fill_patch_list.clear();
    symb_patch_list.clear();
    text_patch_list.clear();
    shape_patch_list.clear();

    return;

}


void CDisplayList::free_pick_lists (void)
{
    line_pick_list.clear();
    contour_line_pick_list.clear();
    fill_pick_list.clear();
    symb_pick_list.clear();
    text_pick_list.clear();
    shape_pick_list.clear();

    return;

}



int CDisplayList::add_contour_line_patch_prim (int prim_num)
{
    int            cp_size = (int)contour_line_prim_list.size();
    LInePrim       *cp_data = contour_line_prim_list.data();

    if (prim_num < 0  ||  prim_num >= cp_size  ||  cp_data == NULL) {
        return 0;
    }

    try {
        if (patch_pick_flag == 1) {
            contour_line_pick_list.push_back(prim_num);
        }
        else {
            contour_line_patch_list.push_back(prim_num);
        }
    }
    catch (...) {
        return -1;
    }

    return 1;
}



int CDisplayList::add_line_patch_prim (int prim_num)
{
    int            cp_size = (int)line_prim_list.size();
    LInePrim       *cp_data = line_prim_list.data();

    if (prim_num < 0  ||  prim_num >= cp_size  ||  cp_data == NULL) {
        return 0;
    }

    try {
        if (patch_pick_flag == 1) {
            line_pick_list.push_back(prim_num);
        }
        else {
            line_patch_list.push_back(prim_num);
        }
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::add_fill_patch_prim (int prim_num)
{
    int          fp_size = (int)fill_prim_list.size();
    FIllPrim     *fp_data = fill_prim_list.data();

    if (prim_num < 0  ||  prim_num >= fp_size  ||  fp_data == NULL) {
        return 0;
    }

    try {
        if (patch_pick_flag == 1) {
            fill_pick_list.push_back(prim_num);
        }
        else {
            fill_patch_list.push_back(prim_num);
        }
    }
    catch (...) {
        return -1;
    }

    return 1;
}



int CDisplayList::add_text_patch_prim (int prim_num)
{
    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (prim_num < 0  ||  prim_num >= tp_size  ||  tp_data == NULL) {
        return 0;
    }

    try {
        if (patch_pick_flag == 1) {
            text_pick_list.push_back(prim_num);
        }
        else {
            text_patch_list.push_back(prim_num);
        }
    }
    catch (...) {
        return -1;
    }

    return 1;
}



int CDisplayList::add_symb_patch_prim (int prim_num)
{
    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (prim_num < 0  ||  prim_num >= sp_size  ||  sp_data == NULL) {
        return 0;
    }

    try {
        if (patch_pick_flag == 1) {
            symb_pick_list.push_back(prim_num);
        }
        else {
            symb_patch_list.push_back(prim_num);
        }
    }
    catch (...) {
        return -1;
    }

    return 1;
}




int CDisplayList::add_shape_patch_prim (int prim_num)
{
    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (prim_num < 0  ||  prim_num >= hp_size  ||  hp_data == NULL) {
        return 0;
    }

    try {
        if (patch_pick_flag == 1) {
            shape_pick_list.push_back(prim_num);
        }
        else {
            shape_patch_list.push_back(prim_num);
        }
    }
    catch (...) {
        return -1;
    }

    return 1;
}


/*
 *************************************************************************************

                  c o n v e r t _ f r a m e _ a r r a y

 *************************************************************************************

  Convert from frame to page coordinates.  Overwrites the input array.

*/
void CDisplayList::convert_frame_array (CSW_F *xy, int npts)
{
    CSW_F           *x, *y;
    int             i;
    CSW_F           sx, sy;

    if (npts < 1) {
        return;
    }

    if (current_frame_num < 0  ||  current_frame_num >= num_frame_list) {
        return;
    }

/*
 * scales are in page units per frame unit
 */
    sx = (Px2 - Px1) / (Fx2 - Fx1);
    sy = (Py2 - Py1) / (Fy2 - Fy1);

    x = xy;
    y = xy+1;

    for (i=0; i<npts; i++) {
        if (*x < _DL_XY_TOO_BIG_  &&  *y < _DL_XY_TOO_BIG_) {
            *x = (*x - Fx1) * sx + Px1;
            *y = (*y - Fy1) * sy + Py1;
        }
        if (i < npts-1) {
            x+=2;
            y+=2;
        }
    }

    return;
}


void CDisplayList::convert_frame_array (CSW_F *x, CSW_F *y, int npts)
{
    int             i;
    CSW_F           sx, sy;

    if (npts < 1) {
        return;
    }

    if (current_frame_num < 0  ||  current_frame_num >= num_frame_list) {
        return;
    }

/*
 * scales are in page units per frame unit
 */
    sx = (Px2 - Px1) / (Fx2 - Fx1);
    sy = (Py2 - Py1) / (Fy2 - Fy1);

    for (i=0; i<npts; i++) {
        if (x[i] < _DL_XY_TOO_BIG_  &&  y[i] < _DL_XY_TOO_BIG_) {
            x[i] = (x[i] - Fx1) * sx + Px1;
            y[i] = (y[i] - Fy1) * sy + Py1;
        }
    }

    return;
}


void CDisplayList::convert_frame_point (CSW_F *x, CSW_F *y)
{
    CSW_F         sx, sy;

    if (current_frame_num < 0  ||  current_frame_num >= num_frame_list) {
        return;
    }

/*
 * scales are in page units per frame unit
 */
    sx = (Px2 - Px1) / (Fx2 - Fx1);
    sy = (Py2 - Py1) / (Fy2 - Fy1);

    if (*x < _DL_XY_TOO_BIG_  &&  *y < _DL_XY_TOO_BIG_) {
        *x = (*x - Fx1) * sx + Px1;
        *y = (*y - Fy1) * sy + Py1;
    }

    return;
}


void CDisplayList::unconvert_frame_point (CSW_F *x, CSW_F *y)
{
    CSW_F         sx, sy;

    if (current_frame_num < 0  ||  current_frame_num >= num_frame_list) {
        return;
    }

/*
 * scales are in page units per frame unit
 */
    sx = (Fx2 - Fx1) / (Px2 - Px1);
    sy = (Fy2 - Fy1) / (Py2 - Py1);

    if (*x < _DL_XY_TOO_BIG_  &&  *y < _DL_XY_TOO_BIG_) {
        *x = (*x - Px1) * sx + Fx1;
        *y = (*y - Py1) * sy + Fy1;
    }

    return;
}

void CDisplayList::unconvert_frame_array (CSW_F *xy, int npts)
{
    CSW_F           *x, *y;
    int             i;
    CSW_F           sx, sy;

    if (npts < 1) {
        return;
    }

    if (current_frame_num < 0  ||  current_frame_num >= num_frame_list) {
        return;
    }

/*
 * scales are in Frame units per page unit
 */
    sx = (Fx2 - Fx1) / (Px2 - Px1);
    sy = (Fy2 - Fy1) / (Py2 - Py1);

    x = xy;
    y = xy+1;

    for (i=0; i<npts; i++) {
        if (*x < _DL_XY_TOO_BIG_  &&  *y < _DL_XY_TOO_BIG_) {
            *x = (*x - Px1) * sx + Fx1;
            *y = (*y - Py1) * sy + Fy1;
        }
        if (i < npts-1) {
            x+=2;
            y+=2;
        }
    }

    return;
}


void CDisplayList::unconvert_frame_array (CSW_F *x, CSW_F *y, int npts)
{
    int             i;
    CSW_F           sx, sy;

    if (npts < 1) {
        return;
    }

    if (current_frame_num < 0  ||  current_frame_num >= num_frame_list) {
        return;
    }

/*
 * scales are in Frame units per page unit
 */
    sx = (Fx2 - Fx1) / (Px2 - Px1);
    sy = (Fy2 - Fy1) / (Py2 - Py1);

    for (i=0; i<npts; i++) {
        if (x[i] < _DL_XY_TOO_BIG_  &&  y[i] < _DL_XY_TOO_BIG_) {
            x[i] = (x[i] - Px1) * sx + Fx1;
            y[i] = (y[i] - Py1) * sy + Fy1;
        }
    }

    return;
}


/*
******************************************************************

                          A d d F i l l

******************************************************************

  function name:    AddFill

  purpose:          set up fill primitive in the graphics display system
                    As of the 2017 refactor, if a frame is current, the
                    frame x and y are put into the display list, and the
                    lineprim frame_num is set to >= 0 (the index into the
                    framelist).  No conversion to page units is done here.
                    It is better to convert to page units at draw time.

  return value:     1 on success, -1 on a memory allocation failure
                    A return value of zero means a calling parameter
                    was bad.

  calling parameters:

    xptsin          r    double*   Array of x coordinates
    yptsin          r    double*   Array of y coordinates.
    icomp           r    int*      Array with number of points per component.
    npts            r    int       Number of components.
    outline_flag    r    int       0 = do not draw boundary
                                   1 = draw boundary using current line attributes

*/


int CDisplayList::AddFill (double *xptsin, double *yptsin,
                           int *icompin, int ncompin,
                           int outline_flag)
{
    double         xmin = 0.0, ymin = 0.0, xmax = 0.0, ymax = 0.0;
    FIllPrim       *fptr = NULL;
    int            i = 0, npts = 0;
    int            *npts_closed = NULL, *npts_cut = NULL;
    int            ncomp = 0, istat = 0;
    CSW_F          *x_closed = NULL, *y_closed = NULL;
    CSW_F          *x_cut = NULL, *y_cut = NULL;
    int            nptot = 0;
    CSW_F          *x_orig = NULL, *y_orig = NULL;
    int            *npts_orig = NULL;
    int            next_fill;

    CSWErrNum      err_obj;

    int            local_frame_num = -1;
    int            local_scaleable = 0;

    bool    bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (x_closed);
        csw_Free (y_closed);
        csw_Free (npts_closed);
        csw_Free (x_cut);
        csw_Free (y_cut);
        csw_Free (npts_cut);
        if (bsuccess == false) {
            csw_Free (x_orig);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * check obvious errors in parameters.
 */
    if (ncompin < 1) {
        return 0;
    }
    if (xptsin == NULL  ||  yptsin == NULL) {
        return 0;
    }

    if (icompin == NULL) {
        return 0;
    }

    npts = 0;
    for (i=0; i<ncompin; i++) {
        npts += icompin[i];
    }
    if (npts > _DL_MAX_WORK_) npts = _DL_MAX_WORK_;

    next_fill = get_available_fill ();
    if (next_fill < 0) {
        try {
            SNF;
            FIllPrim  fpr;
            ZeroInit (&fpr, sizeof(fpr));
            fill_prim_list.push_back (fpr);
            next_fill = (int)fill_prim_list.size() - 1;
        }
        catch (...) {
            printf ("Exception in fill_prim_list pushback\n");
            return 0;
        }
    }

/*
    If any polygon components are not closed, close them.
*/
    istat = gpf_calcdraw_obj.gpf_close_polygon_holes (xptsin, yptsin, icompin, ncompin,
                                     &x_closed, &y_closed, &npts_closed);
    if (istat == -1) {
        return -1;
    }

/*
    Remove any cut lines and separate the resulting
    holes.  This is needed to make sure that the
    polygon can be clipped.  All polygons need to have
    this done before being put into the display list.
*/
    istat = gpf_calcdraw_obj.gpf_removecutlines (x_closed, y_closed, ncompin, npts_closed,
                                &x_cut, &y_cut, &ncomp, &npts_cut);
    csw_Free (x_closed);
    csw_Free (y_closed);
    csw_Free (npts_closed);
    x_closed = NULL;
    y_closed = NULL;
    npts_closed = NULL;
    if (istat == -1) {
        return -1;
    }

/*
 * If a frame is active, put it's number in the frame_num member
 * of the fill structure.  A non negative frame_num tells the
 * drawing code that the x and y points are in frame coordinates.
 */
    if (current_frame_num >= 0  &&
        current_frame_num < num_frame_list  &&
        frame_list != NULL  &&
        current_axis_num < 0) {
        local_frame_num = current_frame_num;
        local_scaleable = 1;
    }
    nptot = 0;
    for (i=0; i<ncomp; i++) {
        nptot += npts_cut[i];
    }
    x_orig = (CSW_F *)csw_Malloc (nptot * 2 * sizeof(CSW_F));
    if (x_orig == NULL) {
        return -1;
    }
    y_orig = x_orig + nptot;
    npts_orig = (int *)csw_Malloc (ncomp * sizeof(int));
    if (npts_orig == NULL) {
        return -1;
    }
    memcpy (x_orig, x_cut, nptot * sizeof(CSW_F));
    memcpy (y_orig, y_cut, nptot * sizeof(CSW_F));
    memcpy (npts_orig, npts_cut, ncomp * sizeof(int));

    FIllPrim   *fp_data = fill_prim_list.data();

    fptr = fp_data + next_fill;
    fptr->x_orig = x_orig;
    fptr->y_orig = y_orig;
    fptr->npts_orig = npts_orig;
    fptr->ncomp_orig = ncomp;

    fptr->maxpts = nptot;
    fptr->npts = nptot;

    fptr->thick = current_line_thickness;
    fptr->patscale = current_fill_pattern_size;
    fptr->dashscale = current_line_pattern_size;

    fptr->fill_red = current_fill_red;
    fptr->fill_green = current_fill_green;
    fptr->fill_blue = current_fill_blue;
    fptr->fill_alpha = current_fill_alpha;

    fptr->pat_red = current_fill_pattern_red;
    fptr->pat_green = current_fill_pattern_green;
    fptr->pat_blue = current_fill_pattern_blue;
    fptr->pat_alpha = current_fill_pattern_alpha;

    fptr->border_red = current_fill_border_red;
    fptr->border_green = current_fill_border_green;
    fptr->border_blue = current_fill_border_blue;
    fptr->border_alpha = current_fill_border_alpha;

    fptr->pattern = current_fill_pattern;
    fptr->outline = outline_flag;
    fptr->linepatt = current_line_pattern;

    fptr->border_num = current_border_num;
    fptr->axis_num = current_axis_num;
    fptr->frame_num = local_frame_num;
    fptr->graph_num = current_graph_num;
    fptr->selectable_object_num = current_selectable_object_num;
    fptr->xaxis_num = current_xaxis_num;
    fptr->yaxis_num = current_yaxis_num;

    fptr->layer_num = current_layer_num;
    fptr->item_num = current_item_num;

    fptr->smooth_flag = (char)current_line_smooth_flag;
    fptr->editable_flag = (char)current_editable_flag;
    fptr->selectable_flag = (char)current_selectable_flag;

    fptr->visible_flag = 1;
    fptr->draw_flag = 1;
    fptr->plot_flag = 1;
    fptr->selected_flag = 0;
    fptr->deleted_flag = 0;

    fptr->scaleable = local_scaleable;

    fptr->prim_num = next_fill;

    CalcFillBounds (next_fill,
                    &xmin, &ymin, &xmax, &ymax);
    fptr->xmin = (CSW_F)xmin;
    fptr->ymin = (CSW_F)ymin;
    fptr->xmax = (CSW_F)xmax;
    fptr->ymax = (CSW_F)ymax;

    SetSpatialIndexForFill (next_fill);

    if (current_selectable_object) {
        current_selectable_object->AddFill (next_fill);
    }

    bsuccess = true;

    return 1;

}  /*  end of function AddFill  */



/*
 *****************************************************************

                    D r a w A l l F i l l s

 *****************************************************************

  Draw all the fills in the display list.
*/

int CDisplayList::DrawAllFills (void)
{
    int            i, ido, nloop;
    FIllPrim       *fptr;

    double *xylocal = NULL;
    int    free_xylocal = false;
    bool   bpatch = false;

    auto fscope = [&]()
    {
        if (free_xylocal) {
            csw_Free (xylocal);
        }
        xylocal = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

    int        fp_size = (int)fill_prim_list.size();
    FIllPrim   *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return 0;
    }

    int        pt_size = (int)fill_patch_list.size();
    int        *pt_data = fill_patch_list.data();

    nloop = fp_size;
    if (pt_data != NULL  &&  pt_size > 0  &&
        patch_draw_flag == 1) {
        nloop = pt_size;
        bpatch = true;
    }

    if (nloop < 1) {
        return 0;
    }

    int max_ppts = 0;

    for (ido=0; ido<nloop; ido++) {

        i = ido;
        if (bpatch) {
            i = pt_data[ido];
        }

        fptr = fp_data + i;
        if (fptr->maxpts > max_ppts) max_ppts = fptr->maxpts;
    }

    max_ppts += (fp_size + 100);

    if (max_ppts < max_static_ppts) {
        xylocal = static_xy_local;
    }
    else {
        xylocal =
            (double *)csw_Malloc (max_ppts * 2 * sizeof(double));
        if (xylocal == NULL) {
            return 0;
        }
        free_xylocal = true;
    }

    for (ido=0; ido<nloop; ido++) {

        i = ido;
        if (bpatch) {
            i = pt_data[ido];
        }

        fptr = fp_data + i;

        if (fptr->deleted_flag == 1) {
            continue;
        }
        if (fptr->visible_flag == 0) {
            continue;
        }

    /*
     * If the primitive is from a border, do not draw it here.
     */
        if (fptr->border_num >= 0) {
            continue;
        }
        if (fptr->axis_num >= 0) {
            continue;
        }

        gpf_packxy2 (fptr->x_orig, fptr->y_orig, fptr->maxpts, xylocal);

    /*
     * Check the fill bbox against the fill frame.
     */
        if (fptr->frame_num >= 0) {
            bool bbchk = CheckInsideFrame (fptr->frame_num,
                                           fptr->xmin, fptr->ymin,
                                           fptr->xmax, fptr->ymax);
            if (!bbchk) {
                continue;
            }
            convert_frame_array (fptr->frame_num, xylocal, fptr->maxpts);
        }

        ezx_java_obj.ezx_SetFrameInJavaArea (fptr->frame_num);
        gtx_drawprim_obj.gtx_set_poly_alpha (
                            fptr->fill_alpha,
                            fptr->pat_alpha,
                            fptr->border_alpha);

        gtx_drawprim_obj.gtx_clipfillprim
                        (xylocal,
                         fptr->npts,
                         fptr->smooth_flag,
                         fptr->fill_red,
                         fptr->fill_green,
                         fptr->fill_blue,
                         fptr->pat_red,
                         fptr->pat_green,
                         fptr->pat_blue,
                         fptr->border_red,
                         fptr->border_green,
                         fptr->border_blue,
                         fptr->thick,
                         fptr->outline,
                         fptr->pattern,
                         fptr->patscale,
                         fptr->linepatt,
                         fptr->dashscale,
                         fptr->xmin,
                         fptr->ymin,
                         fptr->xmax,
                         fptr->ymax);

        gtx_drawprim_obj.gtx_set_poly_alpha (255, 255, 255);

    }

    return 0;

}  /*  end of function DrawAllFills */



/*
******************************************************************

          S e t S p a t i a l I n d e x F o r F i l l

******************************************************************

  Get the fill points (or use those specified) and set the spatial
  index grid to point to this fill prim number.
*/

int CDisplayList::SetSpatialIndexForFill (int fill_prim_num)
{
    int             istat;
    FIllPrim        *fptr;
    FRameStruct     *fp;

    int             fp_size = (int)fill_prim_list.size();
    FIllPrim        *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return 0;
    }
    if (fill_prim_num < 0  ||  fill_prim_num >= fp_size) {
        return 0;
    }

    fptr = fp_data + fill_prim_num;

    istat = SetupSpatialIndexForFrame (fptr->frame_num);
    if (istat == -1) {
        return 0;
    }

    if (fptr->x_orig == NULL  ||  fptr->y_orig == NULL  ||
        fptr->npts_orig == NULL  ||  fptr->ncomp_orig < 1) {
        return 0;
    }

    set_grid_for_area (fptr);

    if (frame_list != NULL  &&  num_frame_list > 0) {
        fp = frame_list + fptr->frame_num;
        fp->num_fill_index++;
    }

    return 1;

}  /*  end of function SetSpatialIndexForFill  */



/*
 * Set the fill spatial index for the bounding box of this polygon.
 */
int CDisplayList::set_grid_for_area (FIllPrim *fptr)
{
    int            i, j, k, offset, i1, i2, j1t, j2;
    int            *cell_list, max_cell, num_cell;
    int            ntot, narea;
    double         pct;

    if (fptr == NULL) {
        return 0;
    }

    if (fill_spatial_index == NULL) {
        return 0;
    }

/*
 * Get the row and column limits of the bounding box.  If the
 * bounding box extends outside of the index grid, set it in
 * the extra cell.
 */
    i1 = (int)((fptr->ymin - index_ymin) / index_yspace);
    i2 = (int)((fptr->ymax - index_ymin) / index_yspace);
    i2++;
    j1t = (int)((fptr->xmin - index_xmin) / index_xspace);
    j2 = (int)((fptr->xmax - index_xmin) / index_xspace);
    j2++;

    ntot = index_ncol * index_nrow;
    narea = (i2 - i1) * (j2 - j1t);
    pct = (double)narea / (double)ntot;
    if (pct > 0.1) {
        set_in_extra (fptr);
        return 0;
    }

    if (i1 < 0  ||  i2 >= index_nrow) {
        set_in_extra (fptr);
        return 0;
    }

    if (j1t < 0  ||  j2 >= index_ncol) {
        set_in_extra (fptr);
        return 0;
    }

/*
 * add the prim num to each bounding box cell's list.
 */
    for (i=i1; i<=i2; i++) {
        offset = i * index_ncol;
        for (j=j1t; j<=j2; j++) {
            k = offset + j;
            cell_list = fill_spatial_index[k];
            if (cell_list == NULL) {
                cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
                if (cell_list == NULL) {
                    return -1;
                }
                cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
                cell_list[1] = 0;
            }
            max_cell = cell_list[0];
            num_cell = cell_list[1];
            if (num_cell >= max_cell - 2) {
                max_cell += _SMALL_CHUNK_SIZE_;
                cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
                if (cell_list) {
                    cell_list[0] += _SMALL_CHUNK_SIZE_;
                }
            }
            if (cell_list == NULL) {
                return -1;
            }

            cell_list[num_cell+2] = fptr->prim_num;
            cell_list[1]++;

            fill_spatial_index[k] = cell_list;
        }
    }

    return 1;

}


int CDisplayList::set_in_extra (FIllPrim *prim)

{
    int                offset;
    int                *cell_list, num_cell, max_cell;

    if (prim->in_extra_flag == 1) {
        return 1;
    }

/*
 * Do nothing if the fill spatial index is not set up yet.
 */
    if (fill_spatial_index == NULL) {
        return -1;
    }

    offset = index_ncol * index_nrow;

    cell_list = fill_spatial_index[offset];
    if (cell_list == NULL) {
        cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
        if (cell_list == NULL) {
            return -1;
        }
        cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
        cell_list[1] = 0;
    }
    max_cell = cell_list[0];
    num_cell = cell_list[1];
    if (num_cell >= max_cell - 2) {
        max_cell += _SMALL_CHUNK_SIZE_;
        cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
        if (cell_list) {
            cell_list[0] += _SMALL_CHUNK_SIZE_;
        }
    }
    if (cell_list == NULL) {
        return -1;
    }

    prim->in_extra_flag = 1;

    cell_list[num_cell+2] = prim->prim_num;
    cell_list[1]++;

    fill_spatial_index[offset] = cell_list;

    return 0 ;

}  /*  end of private set_in_extra function  */




/*
  ****************************************************************

                    C a l c F i l l B o u n d s

  ****************************************************************

*/

int CDisplayList::CalcFillBounds (int fill_prim_num,
                                  double *xmin, double *ymin,
                                  double *xmax, double *ymax)

{
    int             i;
    CSW_F           *xp, *yp;
    int             npts;
    FIllPrim        *fptr;

    *xmin = 1.e30;
    *ymin = 1.e30;
    *xmax = -1.e30;
    *ymax = -1.e30;

    int        fp_size = (int)fill_prim_list.size();
    FIllPrim   *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return -1;
    }

    if (fill_prim_num < 0  ||  fill_prim_num >= fp_size) {
        return -1;
    }

    fptr = fp_data + fill_prim_num;

/*
 * use the x_orig and y_orig arrays if the prim is rescaleable.
 */
    if (fptr->scaleable == 1) {
        npts = 0;
        for (i=0; i<fptr->ncomp_orig; i++) {
            npts += fptr->npts_orig[i];
        }
        xp = fptr->x_orig;
        yp = fptr->y_orig;
        for (i=0; i<npts; i++) {
            if (xp[i] < _DL_XY_TOO_BIG_  &&
                yp[i] < _DL_XY_TOO_BIG_) {
                if (xp[i] < *xmin) *xmin = xp[i];
                if (yp[i] < *ymin) *ymin = yp[i];
                if (xp[i] > *xmax) *xmax = xp[i];
                if (yp[i] > *ymax) *ymax = yp[i];
            }
        }
        return 1;
    }

    return 0;

}  /*  end of function CalcFillBounds  */




/*
******************************************************************

                     A d d N u m b e r

******************************************************************

  function name:    AddNumber

  purpose:          This just formats the number into a string and then
                    adds the string with AddText.

  return value:     1 on success, -1 on a memory allocation failure
                    A return value of zero means a calling parameter
                    was bad.

  calling parameters:

    x               r    double    x location
    y               r    double    y location
    size            r    double    height in size units
    angle           r    double    angle from +x axis, in degrees
    value           r    double    numerical value to draw
    ndec            r    int       number of decimal points or
                                   less than or equal to zero for
                                   drawing as an integer.
    comma_flag      r    int       0 = normal g format
                                   1 = comma separation for big numbers
                                   The comma_flag is only use if ndec <= 0
                                   (drawing as an integer)

*/


int CDisplayList::AddNumber (double x, double y, double size, double angle,
                             double value, int ndec, int comma_flag)
{
    int             ival, istat, forcefloat;

    char            c100[101];
    char            cfmt[100];

/*
 * The comma flag is only used if the ndec is zero or less
 */
    if (ndec > 0) {
        comma_flag = 0;
    }

    forcefloat = 0;
    if (value < -2.e9  ||  value > 2.e9) {
        forcefloat = 1;
    }

    if (ndec > 0  ||  forcefloat == 1) {
        if (ndec < 0) ndec = 0;
        strcpy (cfmt, "%");
        if (value > -1.e7  &&  value < 1.e7) {
            sprintf (cfmt+1, ".%df", ndec);
        }
        else {
            sprintf (cfmt+1, ".%dg", ndec+1);
        }
        sprintf (c100, cfmt, value);
    }
    else {
        ival = (int)(floor(value+.01));
        if (ival >= -9999  &&  ival <= 9999) {
            comma_flag = 0;
        }
        sprintf (c100, "%d", ival);
    }

    if (comma_flag == 1) {
        csw_StrInsertCommas (c100, 100);
    }

    istat = AddText (x, y, size, angle, c100);

    return istat;

}



/*
******************************************************************

                          A d d T e x t

******************************************************************

  function name:    AddText

  purpose:          set up text primitive in the graphics display system

  return value:     1 on success, -1 on a memory allocation failure
                    A return value of zero means a calling parameter
                    was bad.

  calling parameters:

    x               r    double    x location
    y               r    double    y location
    size            r    double    height in size units
    angle           r    double    angle from +x axis, in degrees
    textin          r    char*     text string to draw

*/


int CDisplayList::AddText (double x, double y, double size, double angle,
                           char *textinput)
{
    int  istat = AddText (x, y, size, angle, textinput, false);
    return istat;
}

int CDisplayList::AddText (double x, double y, double size, double angle,
                           char *textinput, bool from_graph)
{
    TExtPrim       *tptr;
    int            nc;
    char           *cdata;
    char           text[500], *txt;
    double         xmin, ymin, xmax, ymax;
    int            tfont;
    int            nchar;
    CSW_F          tthick;
    char           textin[500];
    int            next_text;

    int            local_frame_num = -1;
    int            local_scaleable = 0;

/*
 * check obvious errors in parameters.
 */
    if (textinput == NULL) {
        return 0;
    }

/*
 * copy text to a local array so we can strip trailing
 * white space without modifying the input text string.
 */
    strncpy (textin, textinput, 499);
    textin[499] = '\0';
    csw_StrRightStrip (textin);

    next_text = get_available_text ();
    if (next_text < 0) {
        try {
            SNF;
            TExtPrim  tpr;
            ZeroInit (&tpr, sizeof(tpr));
            text_prim_list.push_back (tpr);
            next_text = (int)text_prim_list.size() - 1;
        }
        catch (...) {
            printf ("Exception in text_prim_list pushback\n");
            return 0;
        }
    }

    tfont = current_text_font;
    if (tfont < 0) {
        tfont = 1000;
    }

    tthick = current_text_thickness;
    if (tfont < 1000) {
        if (tthick < 0.001) {
            tthick = 0.005f;
            if (size > 0.07) {
                tthick = 0.01f;
            }
            if (size > 0.12) {
                tthick = 0.012f;
            }
            if (size > 0.16) {
                tthick = 0.017f;
            }
            if (size > 0.20) {
                tthick = 0.023f;
            }
            if (size > 0.30) {
                tthick = 0.035f;
            }
            if (size > 0.40) {
                tthick = 0.040f;
            }
            if (size > 0.50) {
                tthick = 0.05f;
            }

      // thinner lines for polygon fonts
            if (tfont >= 8  &&  tfont <= 19) {
                tthick /= 2.5f;
            }
        }
    }

    nchar = strlen (textin);

/*
 * copy the leftmost nchar characters into a null terminated string
 */
    if (nchar > 499) nchar = 499;
    strncpy (text, textin, nchar);
    text[nchar] = '\0';
    txt = text;

    if (from_graph) {
        local_frame_num = current_frame_num;
        local_scaleable = 1;
    }
    else {
        if (current_frame_num >= 0  &&
            current_frame_num < num_frame_list  &&
            current_axis_num < 0) {
            local_frame_num = current_frame_num;
            local_scaleable = 1;
        }
    }

/*
 * Populate a text prim structure.
 */
    nc = nchar + 1;
    if (nc > 500) nc = 500;
    cdata = (char *)csw_Malloc ((nc + 1) * sizeof(char));
    if (cdata == NULL) {
        return -1;
    }

    TExtPrim    *tp_data = text_prim_list.data();

    tptr = tp_data + next_text;

    strncpy (cdata, txt, nc * sizeof(char));
    cdata[nc] = '\0';

    tptr->x = (CSW_F)x;
    tptr->y = (CSW_F)y;
    tptr->chardata = cdata;

    tptr->bg_thick = current_line_thickness;
    tptr->thick = tthick;
    tptr->angle = (CSW_F)angle;
    tptr->size = (CSW_F)size;

    tptr->xoff = current_text_x_offset;
    tptr->yoff = current_text_y_offset;

    tptr->anchor = current_text_anchor;

    tptr->border_red = current_text_red;
    tptr->border_green = current_text_green;
    tptr->border_blue = current_text_blue;
    tptr->border_alpha = current_text_alpha;

    tptr->fill_red = current_text_fill_red;
    tptr->fill_green = current_text_fill_green;
    tptr->fill_blue = current_text_fill_blue;
    tptr->fill_alpha = current_text_fill_alpha;

    tptr->font_num = tfont;

    tptr->grid_num = current_grid_num;

    tptr->border_num = current_border_num;
    tptr->axis_num = current_axis_num;
    tptr->frame_num = local_frame_num;
    tptr->selectable_object_num = current_selectable_object_num;
    tptr->graph_num = current_graph_num;
    tptr->xaxis_num = current_xaxis_num;
    tptr->yaxis_num = current_yaxis_num;

    tptr->layer_num = current_layer_num;
    tptr->item_num = current_item_num;

    tptr->editable_flag = (char)current_editable_flag;
    tptr->selectable_flag = (char)current_selectable_flag;
    if (current_selectable_flag) {
        num_selectable_text++;
    }

    tptr->visible_flag = 1;
    tptr->draw_flag = 1;
    tptr->plot_flag = 1;
    tptr->selected_flag = 0;
    tptr->deleted_flag = 0;

    tptr->scaleable = local_scaleable;

    tptr->bgflag = current_text_background;
    tptr->bg_red = current_fill_red;
    tptr->bg_green = current_fill_green;
    tptr->bg_blue = current_fill_blue;
    tptr->bg_alpha = current_fill_alpha;
    tptr->bg_border_red = current_fill_border_red;
    tptr->bg_border_green = current_fill_border_green;
    tptr->bg_border_blue = current_fill_border_blue;
    tptr->bg_border_alpha = current_fill_border_alpha;

    tptr->prim_num = next_text;

    CalcTextBounds (next_text,
                    &xmin, &ymin, &xmax, &ymax);

    tptr->xmin = (CSW_F)xmin;
    tptr->ymin = (CSW_F)ymin;
    tptr->xmax = (CSW_F)xmax;
    tptr->ymax = (CSW_F)ymax;

    tptr->image_id = ImageID;

    tptr->scaleable = 1;

    SetSpatialIndexForText (next_text);

    if (current_selectable_object) {
        current_selectable_object->AddText (next_text);
    }

    return 1;

}  /*  end of function AddText  */



/*
 *****************************************************************

              D r a w A l l T e x t s

 *****************************************************************

  Draw all the texts in the display list.
*/

int CDisplayList::DrawAllTexts (void)
{
    int            i, n, nmod, ido, nloop;
    TExtPrim       *tptr;
    CSW_F          xoff, yoff;
    bool           bpatch = false;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return 0;
    }

    int        pt_size = (int)text_patch_list.size();
    int        *pt_data = text_patch_list.data();

    nloop = tp_size;
    if (pt_data != NULL  &&  pt_size > 0  &&
        patch_draw_flag == 1) {
        nloop = pt_size;
        bpatch = true;
    }

    bool  bframe = false;

// Count the number of text items to draw.  If there are
// over 2000, only 2000 will be drawn.  Even with this, 
// 2000 text items are likely to be unreadable.

    n = 0;
    for (ido=0; ido<nloop; ido++) {

        i = ido;
        if (bpatch) {
            i = pt_data[ido];
        }

        tptr = tp_data + i;

        if (tptr->deleted_flag == 1) {
            continue;
        }
        if (tptr->visible_flag == 0) {
            continue;
        }

    /*
     * If the text is from a border or axis,
     * do not count it here.
     */
        if (tptr->border_num >= 0) {
            continue;
        }
        if (tptr->axis_num >= 0) {
            continue;
        }

        n++;

    }

    nmod = n / 2000;
    if (nmod < 1) nmod = 1;

    n = 0;
    for (ido=0; ido<nloop; ido++) {

        i = ido;
        if (bpatch) {
            i = pt_data[ido];
        }

        tptr = tp_data + i;

        if (tptr->deleted_flag == 1) {
            continue;
        }
        if (tptr->visible_flag == 0) {
            continue;
        }

    /*
     * If the text is from a border,
     * do not draw it here.
     */
        if (tptr->border_num >= 0) {
            continue;
        }

    /*
     * Check the text bbox against the text frame.
     */
        bframe = false;
        if (tptr->frame_num >= 0) {
            bool bbchk = CheckInsideFrame (tptr->frame_num,
                                           tptr->xmin, tptr->ymin,
                                           tptr->xmax, tptr->ymax);
            if (!bbchk) {
                continue;
            }
            convert_frame_point (tptr->frame_num,
                                 &tptr->x, &tptr->y);
            bframe = true;
        }

        n++;

    /*
     * If there are a huge number of text items visible in the current zoom
     * window, skip a lot of them since none will be legible anyway.  All
     * inside frame axes labels (as opposed to frame border axes labels)
     * will be drawn.
     */
        if (n % nmod != 0  &&  tptr->axis_num < 0) {
            if (bframe) {
              unconvert_frame_point (tptr->frame_num,
                                     &tptr->x, &tptr->y);
            }
            continue;
        }

        ezx_java_obj.ezx_SetFrameInJavaArea (tptr->frame_num);
        ezx_java_obj.ezx_SetAlphaValue (
            (tptr->border_alpha + tptr->fill_alpha +
             tptr->bg_alpha + tptr->bg_border_alpha) / 4
        );

        gtx_drawprim_obj.gtx_reset_exact_text_length ();

        xoff = tptr->xoff * page_units_per_inch;
        yoff = tptr->yoff * page_units_per_inch;

     /*
      * If the text needs a background rectangle, draw it here.
      * These rectangles have a priority of 2, just below the text
      * character fills and strokes.
      */
        gtx_drawprim_obj.gtx_SetImageIDForDraw (tptr->image_id);
        if (tptr->bgflag != 0  &&  tptr->image_id == -1) {
            ezx_java_obj.ezx_SetDrawingPriority (2);
            gtx_drawprim_obj.gtx_cliptextrect(tptr->x + xoff,
                             tptr->y + yoff,
                             tptr->chardata,
                             tptr->anchor,
                             tptr->bg_thick,
                             tptr->bg_border_red,
                             tptr->bg_border_green,
                             tptr->bg_border_blue,
                             tptr->bg_red,
                             tptr->bg_green,
                             tptr->bg_blue,
                             tptr->angle,
                             tptr->size,
                             tptr->font_num,
                             tptr->bgflag);
        }

     /*
      * The text character fills and strokes have the
      * highest priority and are set to 3.
      */
        ezx_java_obj.ezx_SetDrawingPriority (3);
        gtx_drawprim_obj.gtx_cliptextprim(tptr->x + xoff,
                         tptr->y + yoff,
                         tptr->chardata,
                         tptr->anchor,
                         tptr->thick,
                         tptr->border_red,
                         tptr->border_green,
                         tptr->border_blue,
                         tptr->fill_red,
                         tptr->fill_green,
                         tptr->fill_blue,
                         tptr->angle,
                         tptr->size,
                         tptr->font_num);

        gtx_drawprim_obj.gtx_reset_exact_text_length ();
        gtx_drawprim_obj.gtx_SetImageIDForDraw (-1);

        if (bframe) {
            unconvert_frame_point (tptr->frame_num,
                                   &tptr->x, &tptr->y);
        }
    }

    return 0;

}  /*  end of function DrawAllTexts */



/*
******************************************************************

          S e t S p a t i a l I n d e x F o r T e x t

******************************************************************

    use the text bounding box to set its spatial index
*/

int CDisplayList::SetSpatialIndexForText (int text_prim_num)
{
    TExtPrim        *tptr;
    int             istat;
    FRameStruct     *fp;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return 0;
    }
    if (text_prim_num < 0  ||  text_prim_num >= tp_size) {
        return 0;
    }

    tptr = tp_data + text_prim_num;

    istat = SetupSpatialIndexForFrame (tptr->frame_num);
    if (istat == -1) {
        return 0;
    }

    set_grid_for_area (tptr);

    if (frame_list != NULL  &&  num_frame_list > 0) {
        fp = frame_list + tptr->frame_num;
        fp->num_text_index++;
    }

    return 1;

}  /*  end of function SetSpatialIndexforText  */


/*
 * Set the text spatial index for the bounding box of the text.
 */
int CDisplayList::set_grid_for_area (TExtPrim *tptr)
{
    int            i, j, k, offset, i1, i2, j1t, j2;
    int            *cell_list, max_cell, num_cell;
    int            ntot, narea;
    double         pct;

    if (tptr == NULL) {
        return 0;
    }

    if (text_spatial_index == NULL) {
        return 0;
    }

/*
 * Get the row and column limits of the bounding box.  If the
 * bounding box extends outside of the index grid, set it in
 * the extra cell.
 */
    i1 = (int)((tptr->ymin - index_ymin) / index_yspace);
    i2 = (int)((tptr->ymax - index_ymin) / index_yspace);
    i2++;
    j1t = (int)((tptr->xmin - index_xmin) / index_xspace);
    j2 = (int)((tptr->xmax - index_xmin) / index_xspace);
    j2++;

    ntot = index_ncol * index_nrow;
    narea = (i2 - i1) * (j2 - j1t);
    pct = (double)narea / (double)ntot;
    if (pct > 0.1) {
        set_in_extra (tptr);
        return 0;
    }

    if (i1 < 0  ||  i2 >= index_nrow) {
        set_in_extra (tptr);
        return 0;
    }

    if (j1t < 0  ||  j2 >= index_ncol) {
        set_in_extra (tptr);
        return 0;
    }

/*
 * add the prim num to each bounding box cell's list.
 */
    for (i=i1; i<=i2; i++) {
        offset = i * index_ncol;
        for (j=j1t; j<=j2; j++) {
            k = offset + j;
            cell_list = text_spatial_index[k];
            if (cell_list == NULL) {
                cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
                if (cell_list == NULL) {
                    return -1;
                }
                cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
                cell_list[1] = 0;
            }
            max_cell = cell_list[0];
            num_cell = cell_list[1];
            if (num_cell >= max_cell - 2) {
                max_cell += _SMALL_CHUNK_SIZE_;
                cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
                if (cell_list) {
                    cell_list[0] += _SMALL_CHUNK_SIZE_;
                }
            }
            if (cell_list == NULL) {
                return -1;
            }

            cell_list[num_cell+2] = tptr->prim_num;
            cell_list[1]++;

            text_spatial_index[k] = cell_list;
        }
    }

    return 1;

}

int CDisplayList::set_in_extra (TExtPrim *prim)

{
    int                offset;
    int                *cell_list, num_cell, max_cell;

    if (prim->in_extra_flag == 1) {
        return 1;
    }

/*
 * Do nothing if the text spatial index is not set up yet.
 */
    if (text_spatial_index == NULL) {
        return -1;
    }

    offset = index_ncol * index_nrow;

    cell_list = text_spatial_index[offset];
    if (cell_list == NULL) {
        cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
        if (cell_list == NULL) {
            return -1;
        }
        cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
        cell_list[1] = 0;
    }
    max_cell = cell_list[0];
    num_cell = cell_list[1];
    if (num_cell >= max_cell - 2) {
        max_cell += _SMALL_CHUNK_SIZE_;
        cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
        if (cell_list) {
            cell_list[0] += _SMALL_CHUNK_SIZE_;
        }
    }
    if (cell_list == NULL) {
        return -1;
    }

    prim->in_extra_flag = 1;

    cell_list[num_cell+2] = prim->prim_num;
    cell_list[1]++;

    text_spatial_index[offset] = cell_list;

    return 0 ;

}  /*  end of private set_in_extra function  */




/*
  ****************************************************************

                    C a l c T e x t B o u n d s

  ****************************************************************

*/

int CDisplayList::CalcTextBounds (int text_prim_num,
                              double *xmin, double *ymin,
                              double *xmax, double *ymax)

{
    TExtPrim        *tptr;
    CSW_F           width;
    int             i;
    CSW_F           cx[4], cy[4];
    CSW_F           fsize, xt, yt;
    CSW_F           bounds[10];
    double          dx, dy;

    *xmin = 1.e30;
    *ymin = 1.e30;
    *xmax = -1.e30;
    *ymax = -1.e30;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return -1;
    }

    if (text_prim_num < 0  ||  text_prim_num >= tp_size) {
        return -1;
    }

    tptr = tp_data + text_prim_num;

/*
 * Get the text bounds in inches.
 */
    gtx_GetTextBounds (
        tptr->chardata,
        tptr->font_num,
        tptr->size,
        bounds);

/*
 * Convert the position and bounds to page units and get
 * the rectangular outline in page units.
 */
    xt = tptr->x;
    yt = tptr->y;
    if (tptr->scaleable != 0) {
        convert_frame_point (&xt, &yt);
    }
    width = bounds[0] * page_units_per_inch;
    fsize = bounds[1] * page_units_per_inch;

    gpf_calcdraw_obj.gpf_find_box_corners (xt, yt,
                          tptr->anchor,
                          width, fsize, tptr->angle,
                          cx, cy);

/*
 * Convert the outline to frame coordinates and then determine
 * the bounding box.
 */
    unconvert_frame_array (cx, cy, 4);

    for (i=0; i<4; i++) {
        if (cx[i] < *xmin) *xmin = cx[i];
        if (cx[i] > *xmax) *xmax = cx[i];
        if (cy[i] < *ymin) *ymin = cy[i];
        if (cy[i] > *ymax) *ymax = cy[i];
    }

/*
 * If a java font (number >= 1000) is being drawn, expand the extents
 * about 10 percent in both x and y
 */
    if (tptr->font_num >= 1000) {
        dx = *xmax - *xmin;
        dy = *ymax - *ymin;
        dx /= 10.0;
        dy /= 10.0;
        *xmin -= dx;
        *xmax += dx;
        *ymin -= dy;
        *ymax += dy;
    }

    return 1;

}  /*  end of function CalcTextBounds  */




/*
******************************************************************

                          A d d S y m b

******************************************************************

  function name:    AddSymb

  purpose:          set up symb primitive in the graphics display system

  return value:     1 on success, -1 on a memory allocation failure
                    A return value of zero means a calling parameter
                    was bad.

  calling parameters:

    x               r    double    x location
    y               r    double    y location
    size            r    double    height in size units
    angle           r    double    angle from +x axis, in degrees
    symb_num        r    int       number of the symbol to draw

*/


int CDisplayList::AddSymb (double x, double y, double size, double angle,
                           int symb_num)
{
    SYmbPrim       *sptr;
    int            next_symb;

    next_symb = get_available_symb ();
    if (next_symb < 0) {
        try {
            SNF;
            SYmbPrim  spr;
            ZeroInit (&spr, sizeof(spr));
            symb_prim_list.push_back (spr);
            next_symb = (int)symb_prim_list.size() - 1;
        }
        catch (...) {
            printf ("Exception in symb_prim_list pushback\n");
            return 0;
        }
    }

    SYmbPrim   *sp_data = symb_prim_list.data();

/*
 * Populate the symb prim structure.
 */
    sptr = sp_data + next_symb;

    sptr->x = (CSW_F)x;
    sptr->y = (CSW_F)y;

    sptr->xmin = x;
    sptr->ymin = y;
    sptr->xmax = x;
    sptr->ymax = y;

    sptr->thick = current_line_thickness;
    sptr->angle = (CSW_F)angle;
    sptr->size = (CSW_F)size;

    sptr->red = current_symbol_red;
    sptr->green = current_symbol_green;
    sptr->blue = current_symbol_blue;
    sptr->alpha = current_symbol_alpha;

    sptr->symb_num = symb_num;
    sptr->mask_flag = 0;

    sptr->grid_num = current_grid_num;

    sptr->frame_num = current_frame_num;
    sptr->selectable_object_num = current_selectable_object_num;
    sptr->graph_num = current_graph_num;
    sptr->xaxis_num = current_xaxis_num;
    sptr->yaxis_num = current_yaxis_num;

    sptr->layer_num = current_layer_num;
    sptr->item_num = current_item_num;

    sptr->editable_flag = (char)current_editable_flag;
    sptr->selectable_flag = (char)current_selectable_flag;

    sptr->visible_flag = 1;
    sptr->draw_flag = 1;
    sptr->plot_flag = 1;
    sptr->selected_flag = 0;
    sptr->deleted_flag = 0;

    sptr->image_id = ImageID;

    sptr->prim_num = next_symb;
    sptr->scaleable = 1;

    SetSpatialIndexForSymb (next_symb);

    if (current_selectable_object) {
        current_selectable_object->AddSymb (next_symb);
    }

    return 1;

}  /*  end of function AddSymb  */



/*
 *****************************************************************

              D r a w A l l S y m b s

 *****************************************************************

  Draw all the symbs in the display list.
*/

int CDisplayList::DrawAllSymbs (void)
{

    int            i, n, nmod, ido, nloop;
    SYmbPrim       *sptr;
    bool           bpatch = false;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return 0;
    }

    int         pt_size = (int)symb_patch_list.size();
    int         *pt_data = symb_patch_list.data();

    nloop = sp_size;
    if (pt_data != NULL  &&  pt_size > 0  &&
        patch_draw_flag == 1) {
        nloop = pt_size;
        bpatch = true;
    }

    n = 0;
    for (ido=0; ido<nloop; ido++) {

        i = ido;
        if (bpatch) {
            i = pt_data[ido];
        }

        sptr = sp_data + i;

        if (sptr->deleted_flag == 1) {
            continue;
        }
        if (sptr->visible_flag == 0) {
            continue;
        }

        n++;

    }

    nmod = n / 5000;
    if (nmod < 1) nmod = 1;
    n = 0;

    double  xpsym = 0.0;
    double  ypsym = 0.0;

    for (ido=0; ido<nloop; ido++) {

        i = ido;
        if (bpatch) {
            i = pt_data[ido];
        }

        sptr = sp_data + i;

        if (sptr->deleted_flag == 1) {
            continue;
        }
        if (sptr->visible_flag == 0) {
            continue;
        }

    /*
     * If there are a huge number of symbols in the zoom area, only
     * draw a subset since they are illegible anyway.
     */
        if (i % nmod != 0) {
            continue;
        }

        ezx_java_obj.ezx_SetFrameInJavaArea (sptr->frame_num);
        ezx_java_obj.ezx_SetAlphaValue (sptr->alpha);

        xpsym = sptr->x;
        ypsym = sptr->y;

    /*
     * Check the symbol bbox against the symbol frame.
     */
        if (sptr->frame_num >= 0) {
            bool bbchk = CheckInsideFrame (sptr->frame_num,
                                           sptr->xmin, sptr->ymin,
                                           sptr->xmax, sptr->ymax);
            if (!bbchk) {
                continue;
            }

            convert_frame_point (sptr->frame_num, &xpsym, &ypsym);
        }


        gtx_drawprim_obj.gtx_SetImageIDForDraw (sptr->image_id);
        gtx_drawprim_obj.gtx_clipsymbprim
                        (xpsym,
                         ypsym,
                         sptr->symb_num,
                         sptr->size,
                         sptr->thick,
                         sptr->red,
                         sptr->green,
                         sptr->blue,
                         sptr->angle,
                         sptr->mask_flag);
        gtx_drawprim_obj.gtx_SetImageIDForDraw (-1);

    }

    return 0;

}  /*  end of function DrawAllSymbs */



/*
******************************************************************

          S e t S p a t i a l I n d e x F o r S y m b

******************************************************************

    use the symb bounding box to set its spatial index
*/

int CDisplayList::SetSpatialIndexForSymb (int symb_prim_num)
{
    SYmbPrim        *sptr;
    int             istat;
    FRameStruct     *fp;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return 0;
    }

    if (symb_prim_num < 0  ||  symb_prim_num >= sp_size) {
        return 0;
    }

    sptr = sp_data + symb_prim_num;

    istat = SetupSpatialIndexForFrame (sptr->frame_num);
    if (istat == -1) {
        return 0;
    }

    set_grid_for_area (sptr);

    if (frame_list != NULL  &&  num_frame_list > 0) {
        fp = frame_list + sptr->frame_num;
        fp->num_symb_index++;
    }

    return 1;

}  /*  end of function SetSpatialIndexforSymb  */


/*
 * Set the symb spatial index for the bounding box of the symb.
 */
int CDisplayList::set_grid_for_area (SYmbPrim *sptr)
{
    int            i, j, k, offset, i1, i2, j1t, j2;
    int            *cell_list, max_cell, num_cell;
    int            ntot, narea;
    double         pct;

    if (sptr == NULL) {
        return 0;
    }

    if (symb_spatial_index == NULL) {
        return 0;
    }

/*
 * Get the row and column limits of the bounding box.  If the
 * bounding box extends outside of the index grid, set it in
 * the extra cell.
 */
    i1 = (int)((sptr->ymin - index_ymin) / index_yspace);
    i2 = (int)((sptr->ymax - index_ymin) / index_yspace);
    i2++;
    j1t = (int)((sptr->xmin - index_xmin) / index_xspace);
    j2 = (int)((sptr->xmax - index_xmin) / index_xspace);
    j2++;

    ntot = index_ncol * index_nrow;
    narea = (i2 - i1) * (j2 - j1t);
    pct = (double)narea / (double)ntot;
    if (pct > 0.1) {
        set_in_extra (sptr);
        return 0;
    }

    if (i1 < 0  ||  i2 >= index_nrow) {
        set_in_extra (sptr);
        return 0;
    }

    if (j1t < 0  ||  j2 >= index_ncol) {
        set_in_extra (sptr);
        return 0;
    }

/*
 * add the prim num to each bounding box cell's list.
 */
    for (i=i1; i<=i2; i++) {
        offset = i * index_ncol;
        for (j=j1t; j<=j2; j++) {
            k = offset + j;
            cell_list = symb_spatial_index[k];
            if (cell_list == NULL) {
                cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
                if (cell_list == NULL) {
                    return -1;
                }
                cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
                cell_list[1] = 0;
            }
            max_cell = cell_list[0];
            num_cell = cell_list[1];
            if (num_cell >= max_cell - 2) {
                max_cell += _SMALL_CHUNK_SIZE_;
                cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
                if (cell_list) {
                    cell_list[0] += _SMALL_CHUNK_SIZE_;
                }
            }
            if (cell_list == NULL) {
                return -1;
            }

            cell_list[num_cell+2] = sptr->prim_num;
            cell_list[1]++;

            symb_spatial_index[k] = cell_list;
        }
    }

    return 1;

}
/*
  ****************************************************************

                      s e t _ i n _ e x t r a

  ****************************************************************

  Set any symb primitives that are outside of the grid in an extra
  cell at the end of the symb grid array.

*/

int CDisplayList::set_in_extra (SYmbPrim *prim)

{
    int                offset;
    int                *cell_list, num_cell, max_cell;

    if (prim->in_extra_flag == 1) {
        return 1;
    }

/*
 * Do nothing if the symb spatial index is not set up yet.
 */
    if (symb_spatial_index == NULL) {
        return -1;
    }

    offset = index_ncol * index_nrow;

    cell_list = symb_spatial_index[offset];
    if (cell_list == NULL) {
        cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
        if (cell_list == NULL) {
            return -1;
        }
        cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
        cell_list[1] = 0;
    }
    max_cell = cell_list[0];
    num_cell = cell_list[1];
    if (num_cell >= max_cell - 2) {
        max_cell += _SMALL_CHUNK_SIZE_;
        cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
        if (cell_list) {
            cell_list[0] += _SMALL_CHUNK_SIZE_;
        }
    }
    if (cell_list == NULL) {
        return -1;
    }

    prim->in_extra_flag = 1;

    cell_list[num_cell+2] = prim->prim_num;
    cell_list[1]++;

    symb_spatial_index[offset] = cell_list;

    return 0 ;

}  /*  end of private set_in_extra function  */




/*
  ****************************************************************

                    C a l c S y m b B o u n d s

  ****************************************************************

*/

int CDisplayList::CalcSymbBounds (int symb_prim_num,
                                  double *xmin, double *ymin,
                                  double *xmax, double *ymax)

{
    SYmbPrim        *sptr;
    CSW_F           width, x, y;
    CSW_F           fsize, fzero, px1, py1, xzero, yzero;

    *xmin = 1.e30;
    *ymin = 1.e30;
    *xmax = -1.e30;
    *ymax = -1.e30;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return -1;
    }

    if (symb_prim_num < 0  ||  symb_prim_num >= sp_size) {
        return -1;
    }

    sptr = sp_data + symb_prim_num;

/*
 * Convert the symbol size in inches to a symbol size in
 * frame units.
 */
    if (page_units_per_inch < 1.e20) {
        fsize = sptr->size * page_units_per_inch;
    }
    else {
        fsize = sptr->size * 10.0f;
    }


    px1 = Px1;
    py1 = Py1;
    if (Px2 < Px1  ||  Py2 < Py1) {
        px1 = 0.0;
        py1 = 0.0;
    }

    xzero = px1;
    yzero = py1;
    fsize += px1;
    fzero = py1;
    unconvert_frame_point (&xzero, &yzero);
    unconvert_frame_point (&fsize, &fzero);
    fsize -= xzero;
    if (fsize < 0.0) fsize = -fsize;

    x = sptr->x;
    y = sptr->y;

    width = fsize / 2.0f;
    *xmin = x - width;
    *xmax = x + width;
    *ymin = y - width;
    *ymax = y + width;

    return 1;

}  /*  end of function CalcSymbBounds  */




/*
******************************************************************

                          A d d S h a p e

******************************************************************

  function name:    AddShape

  purpose:          set up a shape primitive in the graphics display system
                    shapes are either arcs or rectangles

  return value:     1 on success, -1 on a memory allocation failure
                    A return value of zero means a calling parameter
                    was bad.

  calling parameters:

    shape_type       r    int      1 for centered rectangle
                                  -1 for corner anchored rectangle
                                   2 for arc
                                   3 for 3 point circle
    shape_values     r    double*  Array of values defining the shape.

    For a rectangle:
       shape_values[0] = x of the center in page or frame units as needed
       shape_values[1] = y of the center in page or frame units as needed
       shape_values[2] = width of the box in page or frame units as needed
       shape_values[3] = height of the box in page or frame units as needed
       shape_values[4] = corner radius in page or frame units as needed
       shape_values[5] = rotation angle in degrees, ccw from +x axis

    For an arc:
       shape_values[0] = x of the center in page or frame units as needed
       shape_values[1] = y of the center in page or frame units as needed
       shape_values[2] = radius of arc along the x axis (prior to rotation)
       shape_values[3] = radius along y axis (prior to rotation)
       shape_values[4] = unrotated angle for start of arc drawing (degrees)
       shape_values[5] = angular length for arc drawing (degrees)
       shape_values[6] = rotation angle of the entire arc (degrees ccw from + x)
       shape_values[7] = flag for how to connect the arc end points
                         1 = pie slice
                         2 = chord
                         anything else, do not connect
                         This double value is converted to an integer internally.

    For a 3 point circle:
       shape_values[0] = x1
       shape_values[1] = y1
       shape_values[2] = x2
       shape_values[3] = y2
       shape_values[4] = x3
       shape_values[5] = y3
*/


int CDisplayList::AddShape (int shape_type,
                            double *shape_values)
{
    SHapePrim      *shptr;
    int            istat;
    int            i, nvals;
    double         xmin, ymin, xmax, ymax;
    CSW_F          fvals[10];
    CSW_F          fxc, fyc, radius;
    int            next_shape;

    int            local_frame_num = -1;
    int            local_scaleable = 0;

    if (shape_values[2] <= 0.0  ||  shape_values[3] <= 0.0) {
        return -1;
    }

    next_shape = get_available_shape ();
    if (next_shape < 0) {
        try {
            SNF;
            SHapePrim  hpr;
            ZeroInit (&hpr, sizeof(hpr));
            shape_prim_list.push_back (hpr);
            next_shape = (int)shape_prim_list.size() - 1;
        }
        catch (...) {
            printf ("Exception in shape_prim_list pushback\n");
            return 0;
        }
    }

    memset (fvals, 0, 10 * sizeof(CSW_F));
    nvals = 7;
    if (shape_type == 2) {
        nvals = 8;
    }
    for (i=0; i<nvals; i++) {
        fvals[i] = (CSW_F)shape_values[i];
    }

/*
 * Convert lower left anchored box to centered box.
 */
    if (shape_type == -1) {
        gpf_shape_obj.gpf_boxlltocenter (
            (float)shape_values[0],
            (float)shape_values[1],
            (float)shape_values[2],
            (float)shape_values[3],
            (float)shape_values[4],
            &fxc, &fyc);
            fvals[0] = fxc;
            fvals[1] = fyc;
            shape_type = 1;
    }

/*
 * convert 3 point circle to general arc.
 */
    else if (shape_type == 3) {
        istat = gpf_calcdraw_obj.gpf_3pointcircle (
            (float)shape_values[0],
            (float)shape_values[1],
            (float)shape_values[2],
            (float)shape_values[3],
            (float)shape_values[4],
            (float)shape_values[5],
            &fxc, &fyc, &radius);
        if (istat != 1) {
            return -1;
        }
        fvals[0] = fxc;
        fvals[1] = fyc;
        fvals[2] = radius;
        fvals[3] = radius;
        fvals[4] = 0.0f;
        fvals[5] = 360.0f;
        fvals[6] = 0.0f;
        fvals[7] = 0.0f;
        shape_type = 2;
    }

/*
 *  If a frame is active, put the frame number into the shape structure.
 */
    if (current_frame_num >= 0  &&  current_frame_num < num_frame_list) {
        if (frame_list != NULL) {
            local_frame_num = current_frame_num;
            local_scaleable = 1;
        }
    }

    SHapePrim     *hp_data = shape_prim_list.data();

    shptr = hp_data + next_shape;

    shptr->type = shape_type;
    memset (shptr->fval, 0, 10 * sizeof(CSW_F));
    memcpy (shptr->fval, fvals, nvals * sizeof(CSW_F));

    shptr->thick = current_line_thickness;
    shptr->patscale = current_fill_pattern_size;
    shptr->dashscale = current_line_pattern_size;

    shptr->fill_red = current_fill_red;
    shptr->fill_green = current_fill_green;
    shptr->fill_blue = current_fill_blue;
    shptr->fill_alpha = current_fill_alpha;

    shptr->border_red = current_fill_border_red;
    shptr->border_green = current_fill_border_green;
    shptr->border_blue = current_fill_border_blue;
    shptr->border_alpha = current_fill_border_alpha;

    shptr->pat_red = current_fill_pattern_red;
    shptr->pat_green = current_fill_pattern_green;
    shptr->pat_blue = current_fill_pattern_blue;
    shptr->pat_alpha = current_fill_pattern_alpha;

    shptr->pattern = current_fill_pattern;
    shptr->linepatt = current_line_pattern;

    shptr->frame_num = local_frame_num;
    shptr->selectable_object_num = current_selectable_object_num;
    shptr->graph_num = current_graph_num;
    shptr->xaxis_num = current_xaxis_num;
    shptr->yaxis_num = current_yaxis_num;

    shptr->layer_num = current_layer_num;
    shptr->item_num = current_item_num;

    shptr->editable_flag = (char)current_editable_flag;
    shptr->selectable_flag = (char)current_selectable_flag;

    shptr->visible_flag = 1;
    shptr->draw_flag = 1;
    shptr->plot_flag = 1;
    shptr->selected_flag = 0;
    shptr->deleted_flag = 0;

    shptr->scaleable = local_scaleable;

    shptr->prim_num = next_shape;

    CalcShapeBounds (next_shape, &xmin, &ymin, &xmax, &ymax);
    shptr->xmin = (CSW_F)xmin;
    shptr->ymin = (CSW_F)ymin;
    shptr->xmax = (CSW_F)xmax;
    shptr->ymax = (CSW_F)ymax;

    SetSpatialIndexForShape (next_shape);

    if (current_selectable_object) {
        current_selectable_object->AddShape (next_shape);
    }

    return 1;

}  /*  end of function AddShape  */



/*
 *****************************************************************

              D r a w A l l S h a p e s

 *****************************************************************

  Draw all the shapes in the display list.

*/

int CDisplayList::DrawAllShapes (void)
{
    int            i, ido, nloop;
    SHapePrim      *shptr;
    bool           bpatch = false;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return 0;
    }

    int        pt_size = (int)shape_patch_list.size();
    int        *pt_data = shape_patch_list.data();

    nloop = hp_size;
    if (pt_data != NULL  &&  pt_data > 0  &&
        patch_draw_flag == 1) {
        nloop = pt_size;
        bpatch = true;
    }

    bool  bframe = false;

    for (ido=0; ido<nloop; ido++) {

        i = ido;
        if (bpatch) {
            i = pt_data[ido];
        }

        shptr = hp_data + i;

        if (shptr->deleted_flag == 1) {
            continue;
        }
        if (shptr->visible_flag == 0) {
            continue;
        }

    /*
     * Check the shape bbox against the shape frame.
     */
        bframe = false;
        if (shptr->frame_num >= 0) {
            bool bbchk = CheckInsideFrame (shptr->frame_num,
                                           shptr->xmin, shptr->ymin,
                                           shptr->xmax, shptr->ymax);
            if (!bbchk) {
                continue;
            }
            bframe = true;
        }

        ezx_java_obj.ezx_SetFrameInJavaArea (shptr->frame_num);

        gtx_drawprim_obj.gtx_set_poly_alpha (shptr->fill_alpha,
                            shptr->pat_alpha,
                            shptr->border_alpha);

        switch (shptr->type) {

        /*
            rectangle
        */
            case 1:

                if (bframe) {
                    convert_frame_point (shptr->frame_num,
                                         shptr->fval,
                                         shptr->fval+1);
                  // width
                    convert_frame_dist (shptr->frame_num,
                                        shptr->fval+2);
                  // height
                    convert_frame_dist (shptr->frame_num,
                                        shptr->fval+3);
                }

                gtx_drawprim_obj.gtx_cliprectprim
                                 (shptr->fval[0],
                                  shptr->fval[1],
                                  shptr->fval[2],
                                  shptr->fval[3],
                                  shptr->fval[5],
                                  shptr->fval[4],
                                  shptr->fill_red,
                                  shptr->fill_green,
                                  shptr->fill_blue,
                                  shptr->pat_red,
                                  shptr->pat_green,
                                  shptr->pat_blue,
                                  shptr->border_red,
                                  shptr->border_green,
                                  shptr->border_blue,
                                  shptr->thick,
                                  shptr->pattern,
                                  shptr->patscale,
                                  shptr->linepatt,
                                  shptr->dashscale);

                if (bframe) {
                    unconvert_frame_point (shptr->frame_num,
                                         shptr->fval,
                                         shptr->fval+1);
                  // width
                    unconvert_frame_dist (shptr->frame_num,
                                          shptr->fval+2);
                  // height
                    unconvert_frame_dist (shptr->frame_num,
                                          shptr->fval+3);
                }

                break;

        /*
            arc
        */
            case 2:

                if (bframe) {
                    convert_frame_point (shptr->frame_num,
                                         shptr->fval,
                                         shptr->fval+1);
                  // r1
                    convert_frame_dist (shptr->frame_num,
                                        shptr->fval+2);
                  // r2
                    convert_frame_dist (shptr->frame_num,
                                        shptr->fval+3);
                }

                gtx_drawprim_obj.gtx_cliparcprim
                                (shptr->fval[0],
                                 shptr->fval[1],
                                 shptr->fval[2],
                                 shptr->fval[3],
                                 shptr->fval[4],
                                 shptr->fval[5],
                                 shptr->fval[6],
                                 (int)(shptr->fval[7]+.01),
                                 shptr->fill_red,
                                 shptr->fill_green,
                                 shptr->fill_blue,
                                 shptr->pat_red,
                                 shptr->pat_green,
                                 shptr->pat_blue,
                                 shptr->border_red,
                                 shptr->border_green,
                                 shptr->border_blue,
                                 shptr->thick,
                                 shptr->pattern,
                                 shptr->patscale,
                                 shptr->linepatt,
                                 shptr->dashscale);

                if (bframe) {
                    unconvert_frame_point (shptr->frame_num,
                                           shptr->fval,
                                           shptr->fval+1);
                  // r1
                    unconvert_frame_dist (shptr->frame_num,
                                          shptr->fval+2);
                  // r2
                    unconvert_frame_dist (shptr->frame_num,
                                          shptr->fval+3);
                }

                break;

            default:
                return 0;

        }
        gtx_drawprim_obj.gtx_set_poly_alpha (255, 255, 255);

    }

    return 0;

}  /*  end of function DrawAllShapes */



/*
******************************************************************

          S e t S p a t i a l I n d e x F o r S h a p e

******************************************************************

    use the shape bounding box to set its spatial index
*/

int CDisplayList::SetSpatialIndexForShape (int shape_prim_num)
{
    SHapePrim       *shptr;
    int             istat;
    FRameStruct     *fp;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return 0;
    }
    if (shape_prim_num < 0  ||  shape_prim_num >= hp_size) {
        return 0;
    }

    shptr = hp_data + shape_prim_num;

    istat = SetupSpatialIndexForFrame (shptr->frame_num);
    if (istat == -1) {
        return 0;
    }

    set_grid_for_area (shptr);

    if (frame_list != NULL  &&  num_frame_list > 0) {
        fp = frame_list + shptr->frame_num;
        fp->num_shape_index++;
    }

    return 1;

}  /*  end of function SetSpatialIndexforShape  */


/*
 * Set the shape spatial index for the bounding box of the shape.
 */
int CDisplayList::set_grid_for_area (SHapePrim *shptr)
{
    int            i, j, k, offset, i1, i2, j1t, j2;
    int            *cell_list, max_cell, num_cell;
    int            ntot, narea;
    double         pct;

    if (shptr == NULL) {
        return 0;
    }

    if (shape_spatial_index == NULL) {
        return 0;
    }

/*
 * Get the row and column limits of the bounding box.  If the
 * bounding box extends outside of the index grid, set it in
 * the extra cell.
 */
    i1 = (int)((shptr->ymin - index_ymin) / index_yspace);
    i2 = (int)((shptr->ymax - index_ymin) / index_yspace);
    i2++;
    j1t = (int)((shptr->xmin - index_xmin) / index_xspace);
    j2 = (int)((shptr->xmax - index_xmin) / index_xspace);
    j2++;

    ntot = index_ncol * index_nrow;
    narea = (i2 - i1) * (j2 - j1t);
    pct = (double)narea / (double)ntot;
    if (pct > 0.1) {
        set_in_extra (shptr);
        return 0;
    }

    if (i1 < 0  ||  i2 >= index_nrow) {
        set_in_extra (shptr);
        return 0;
    }

    if (j1t < 0  ||  j2 >= index_ncol) {
        set_in_extra (shptr);
        return 0;
    }

/*
 * add the prim num to each bounding box cell's list.
 */
    for (i=i1; i<=i2; i++) {
        offset = i * index_ncol;
        for (j=j1t; j<=j2; j++) {
            k = offset + j;
            cell_list = shape_spatial_index[k];
            if (cell_list == NULL) {
                cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
                if (cell_list == NULL) {
                    return -1;
                }
                cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
                cell_list[1] = 0;
            }
            max_cell = cell_list[0];
            num_cell = cell_list[1];
            if (num_cell >= max_cell - 2) {
                max_cell += _SMALL_CHUNK_SIZE_;
                cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
                if (cell_list) {
                    cell_list[0] += _SMALL_CHUNK_SIZE_;
                }
            }
            if (cell_list == NULL) {
                return -1;
            }

            cell_list[num_cell+2] = shptr->prim_num;
            cell_list[1]++;

            shape_spatial_index[k] = cell_list;
        }
    }

    return 1;

}
/*
  ****************************************************************

                      s e t _ i n _ e x t r a

  ****************************************************************

  Set any shape primitives that are outside of the grid in an extra
  cell at the end of the shape grid array.

*/

int CDisplayList::set_in_extra (SHapePrim *prim)

{
    int                offset;
    int                *cell_list, num_cell, max_cell;

    if (prim->in_extra_flag == 1) {
        return 1;
    }

/*
 * Do nothing if the shape spatial index is not set up yet.
 */
    if (shape_spatial_index == NULL) {
        return -1;
    }

    offset = index_ncol * index_nrow;

    cell_list = shape_spatial_index[offset];
    if (cell_list == NULL) {
        cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
        if (cell_list == NULL) {
            return -1;
        }
        cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
        cell_list[1] = 0;
    }
    max_cell = cell_list[0];
    num_cell = cell_list[1];
    if (num_cell >= max_cell - 2) {
        max_cell += _SMALL_CHUNK_SIZE_;
        cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
        if (cell_list) {
            cell_list[0] += _SMALL_CHUNK_SIZE_;
        }
    }
    if (cell_list == NULL) {
        return -1;
    }

    prim->in_extra_flag = 1;

    cell_list[num_cell+2] = prim->prim_num;
    cell_list[1]++;

    shape_spatial_index[offset] = cell_list;

    return 0 ;

}  /*  end of private set_in_extra function  */




/*
  ****************************************************************

              C a l c S h a p e P a g e B o u n d s

  ****************************************************************

*/

int CDisplayList::CalcShapePageBounds (int shape_prim_num,
                                       double *xmin, double *ymin,
                                       double *xmax, double *ymax)

{
    SHapePrim        *shptr;
    double           xc, yc, w, h, ang, cang, sang;
    double           xt[4], yt[4], x, y;
    int              i;
//    FRameStruct      *frptr;

    *xmin = 1.e30;
    *ymin = 1.e30;
    *xmax = -1.e30;
    *ymax = -1.e30;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return -1;
    }

    if (shape_prim_num < 0  ||  shape_prim_num >= hp_size) {
        return -1;
    }

    shptr = hp_data + shape_prim_num;

    xc = shptr->fval[0];
    yc = shptr->fval[1];
    w = shptr->fval[2];
    h = shptr->fval[3];
    ang = shptr->fval[6];
    if (shptr->type == 1) {
        w /= 2.0;
        h /= 2.0;
        ang = shptr->fval[5];
    }

    ang = ang * 3.1415926 / 180.0;
    cang = cos (ang);
    sang = sin (ang);

    xt[0] = -w;
    yt[0] = -h;
    xt[1] = w;
    yt[1] = -h;
    xt[2] = w;
    yt[2] = h;
    xt[3] = -w;
    yt[3] = h;

    for (i=0; i<4; i++) {
        x = xt[i];
        y = yt[i];
        xt[i] = x * cang - y * sang;
        yt[i] = x * sang + y * cang;
        xt[i] += xc;
        yt[i] += yc;
        if (xt[i] < *xmin) *xmin = xt[i];
        if (yt[i] < *ymin) *ymin = yt[i];
        if (xt[i] > *xmax) *xmax = xt[i];
        if (yt[i] > *ymax) *ymax = yt[i];
    }

/*
    if (shptr->frame_num >= 0  &&
        shptr->frame_num < num_frame_list  &&
        shptr->scaleable == 0  &&
        frame_list != NULL) {
        frptr = frame_list + shptr->frame_num;
        if (frptr->clipflag == 1) {
            if (*xmin < frptr->px1) *xmin = frptr->px1;
            if (*ymin < frptr->py1) *ymin = frptr->py1;
            if (*xmax > frptr->px2) *xmax = frptr->px2;
            if (*ymax > frptr->py2) *ymax = frptr->py2;
        }
    }
*/

    return 1;

}  /*  end of function CalcShapePageBounds  */


int CDisplayList::CalcShapePageBounds (CSW_F *fval, int type, int frame_num,
                                       double *xmin, double *ymin,
                                       double *xmax, double *ymax)

{
    double           xc, yc, w, h, ang, cang, sang;
    double           xt[4], yt[4], x, y;
    int              i;

    frame_num = frame_num;

    *xmin = 1.e30;
    *ymin = 1.e30;
    *xmax = -1.e30;
    *ymax = -1.e30;

    xc = fval[0];
    yc = fval[1];
    w = fval[2];
    h = fval[3];
    ang = fval[6];
    if (type == 1) {
        w /= 2.0;
        h /= 2.0;
        ang = fval[5];
    }

    ang = ang * 3.1415926 / 180.0;
    cang = cos (ang);
    sang = sin (ang);

    xt[0] = -w;
    yt[0] = -h;
    xt[1] = w;
    yt[1] = -h;
    xt[2] = w;
    yt[2] = h;
    xt[3] = -w;
    yt[3] = h;

    for (i=0; i<4; i++) {
        x = xt[i];
        y = yt[i];
        xt[i] = x * cang - y * sang;
        yt[i] = x * sang + y * cang;
        xt[i] += xc;
        yt[i] += yc;
        if (xt[i] < *xmin) *xmin = xt[i];
        if (yt[i] < *ymin) *ymin = yt[i];
        if (xt[i] > *xmax) *xmax = xt[i];
        if (yt[i] > *ymax) *ymax = yt[i];
    }

/*
    if (frame_num >= 0  &&
        frame_num < num_frame_list  &&
        frame_list != NULL) {
        frptr = frame_list + frame_num;
        if (frptr->clipflag == 1) {
            if (*xmin < frptr->px1) *xmin = frptr->px1;
            if (*ymin < frptr->py1) *ymin = frptr->py1;
            if (*xmax > frptr->px2) *xmax = frptr->px2;
            if (*ymax > frptr->py2) *ymax = frptr->py2;
        }
    }
*/

    return 1;

}  /*  end of function CalcShapePageBounds  */



/*
 *************************************************************************

                          A d d D a t a I m a g e

 *************************************************************************

   Convert to a color image and then add as a color image.

*/

/*
    Add an image based on an array of double values, using the current
    image value arrays from the most recent call to SetImageColors and
    also the options from SetImageOptions.  The name is the most recent
    name from SetImageName.
*/
int CDisplayList::AddDataImage (double *data,
                                int ncol,
                                int nrow,
                                double null_value,
                                double xmin,
                                double ymin,
                                double xmax,
                                double ymax)
{
    int            istat, i, j, k, offset, ntot;
    unsigned char  *red_ptr = NULL, *green_ptr = NULL,
                   *blue_ptr = NULL, *trans_ptr = NULL;
    int            red, green, blue, trans;

    auto fscope = [&]()
    {
        csw_Free (red_ptr);
        csw_Free (green_ptr);
        csw_Free (blue_ptr);
        csw_Free (trans_ptr);
    };
    CSWScopeGuard  func_scope_guard (fscope);


    if (xmax <= xmin || ymax <= ymin ||
        data == NULL || ncol < 2  ||  nrow < 2) {
        return -1;
    }

/*
 * Allocate space for color image data.
 */
    ntot = ncol * nrow;
    red_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    green_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    blue_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    trans_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    if (red_ptr == NULL  ||  green_ptr == NULL  ||  blue_ptr == NULL  ||
        trans_ptr == NULL) {
        return -1;
    }

/*
 * convert data image to color image.
 */
    current_image_null_value = (double)null_value;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            lookup_image_color ((double)data[k],
                                &red, &green, &blue, &trans);
            red_ptr[k] = (unsigned char)red;
            green_ptr[k] = (unsigned char)green;
            blue_ptr[k] = (unsigned char)blue;
            trans_ptr[k] = (unsigned char)trans;
        }
    }

    istat = AddColorImage (red_ptr,
                           green_ptr,
                           blue_ptr,
                           trans_ptr,
                           -ncol,
                           nrow,
                           xmin,
                           ymin,
                           xmax,
                           ymax);

    return istat;

}  /*  end of function AddDataImage (double) */



/*
 *****************************************************************

              D r a w A l l I m a g e s

 *****************************************************************

  Draw all the images in the display list.
*/

int CDisplayList::DrawAllImages (void)
{
    int            i;
    IMagePrim      *imptr;
    double         x1, y1, x2, y2;

    int            ip_size = (int)image_prim_list.size();
    IMagePrim      *ip_data = image_prim_list.data();

    if (ip_data == NULL  ||  ip_size < 1) {
        return 0;
    }

    for (i=0; i<ip_size; i++) {

        imptr = ip_data + i;

        if (imptr->deleted_flag == 1) {
            continue;
        }
        if (imptr->visible_flag == 0) {
            continue;
        }

    /*
     * Check the image bbox against the image frame.
     */
        x1 = imptr->xmin;
        y1 = imptr->ymin;
        x2 = imptr->xmax;
        y2 = imptr->ymax;
        if (imptr->frame_num >= 0) {
            bool bbchk = CheckInsideFrame (imptr->frame_num,
                                           imptr->xmin, imptr->ymin,
                                           imptr->xmax, imptr->ymax);
            if (!bbchk) {
                continue;
            }
            convert_frame_point (imptr->frame_num, &x1, &y1);
            convert_frame_point (imptr->frame_num, &x2, &y2);
        }

        ezx_java_obj.ezx_SetFrameInJavaArea (imptr->frame_num);
        ezx_java_obj.ezx_SetAlphaValue (255);

        gtx_drawprim_obj.gtx_SetImageIDForDraw (imptr->image_id);
        gtx_drawprim_obj.gtx_SetImageHasLinesForDraw (imptr->has_lines);

        gtx_drawprim_obj.gtx_clip_image_prim (
                             x1, y1, x2, y2,
                             imptr->ncol,
                             imptr->nrow,
                             imptr->red_data,
                             imptr->green_data,
                             imptr->blue_data,
                             imptr->transparency_data);

        gtx_drawprim_obj.gtx_SetImageIDForDraw (-1);
        gtx_drawprim_obj.gtx_SetImageHasLinesForDraw (0);

    }

    return 0;

}  /*  end of function DrawAllImages */

/*
    Add an image based on an array of float values, using the current
    image value arrays from the most recent call to SetImageColors and
    also the options from SetImageOptions.  The name is the most recent
    name from SetImageName.
*/
int CDisplayList::AddDataImage (float *data,
                                int ncol,
                                int nrow,
                                float null_value,
                                double xmin,
                                double ymin,
                                double xmax,
                                double ymax)
{
    int            istat, i, j, k, offset, ntot;
    unsigned char  *red_ptr, *green_ptr, *blue_ptr, *trans_ptr;
    int            red, green, blue, trans;

    if (xmax <= xmin || ymax <= ymin ||
        data == NULL || ncol < 2  ||  nrow < 2) {
        return 0;
    }

/*
 * Allocate space for color image data.
 */
    ntot = ncol * nrow;
    red_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    green_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    blue_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    trans_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    if (red_ptr == NULL  ||  green_ptr == NULL  ||  blue_ptr == NULL  ||
        trans_ptr == NULL) {
        csw_Free (red_ptr);
        csw_Free (green_ptr);
        csw_Free (blue_ptr);
        csw_Free (trans_ptr);
        return -1;
    }

/*
 * convert data image to color image.
 */
    current_image_null_value = (double)null_value;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            lookup_image_color ((double)data[k],
                                &red, &green, &blue, &trans);
            red_ptr[k] = (unsigned char)red;
            green_ptr[k] = (unsigned char)green;
            blue_ptr[k] = (unsigned char)blue;
            trans_ptr[k] = (unsigned char)trans;
        }
    }

    istat = AddColorImage (red_ptr,
                           green_ptr,
                           blue_ptr,
                           trans_ptr,
                           -ncol,
                           nrow,
                           xmin,
                           ymin,
                           xmax,
                           ymax);

    return istat;

}  /*  end of function AddDataImage  (float) */


/*
    Add an image based on an array of int values, using the current
    image value arrays from the most recent call to SetImageColors and
    also the options from SetImageOptions.  The name is the most recent
    name from SetImageName.
*/
int CDisplayList::AddDataImage (int *data,
                                int ncol,
                                int nrow,
                                int null_value,
                                double xmin,
                                double ymin,
                                double xmax,
                                double ymax)
{
    int            istat, i, j, k, offset, ntot;
    unsigned char  *red_ptr, *green_ptr, *blue_ptr, *trans_ptr;
    int            red, green, blue, trans;

    if (xmax <= xmin || ymax <= ymin ||
        data == NULL || ncol < 2  ||  nrow < 2) {
        return 0;
    }

/*
 * Allocate space for color image data.
 */
    ntot = ncol * nrow;
    red_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    green_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    blue_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    trans_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    if (red_ptr == NULL  ||  green_ptr == NULL  ||  blue_ptr == NULL  ||
        trans_ptr == NULL) {
        csw_Free (red_ptr);
        csw_Free (green_ptr);
        csw_Free (blue_ptr);
        csw_Free (trans_ptr);
        return -1;
    }

/*
 * convert data image to color image.
 */
    current_image_null_value = (double)null_value;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            lookup_image_color ((double)data[k],
                                &red, &green, &blue, &trans);
            red_ptr[k] = (unsigned char)red;
            green_ptr[k] = (unsigned char)green;
            blue_ptr[k] = (unsigned char)blue;
            trans_ptr[k] = (unsigned char)trans;
        }
    }

    istat = AddColorImage (red_ptr,
                           green_ptr,
                           blue_ptr,
                           trans_ptr,
                           -ncol,
                           nrow,
                           xmin,
                           ymin,
                           xmax,
                           ymax);

    return istat;

}  /*  end of function AddDataImage  (int) */



/*
    Add an image based on an array of short int values, using the current
    image value arrays from the most recent call to SetImageColors and
    also the options from SetImageOptions.  The name is the most recent
    name from SetImageName.
*/
int CDisplayList::AddDataImage (short int *data,
                                int ncol,
                                int nrow,
                                short int null_value,
                                double xmin,
                                double ymin,
                                double xmax,
                                double ymax)
{
    int            istat, i, j, k, offset, ntot;
    unsigned char  *red_ptr, *green_ptr, *blue_ptr, *trans_ptr;
    int            red, green, blue, trans;

    if (xmax <= xmin || ymax <= ymin ||
        data == NULL || ncol < 2  ||  nrow < 2) {
        return 0;
    }

/*
 * Allocate space for color image data.
 */
    ntot = ncol * nrow;
    red_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    green_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    blue_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    trans_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    if (red_ptr == NULL  ||  green_ptr == NULL  ||  blue_ptr == NULL  ||
        trans_ptr == NULL) {
        csw_Free (red_ptr);
        csw_Free (green_ptr);
        csw_Free (blue_ptr);
        csw_Free (trans_ptr);
        return -1;
    }

/*
 * convert data image to color image.
 */
    current_image_null_value = (double)null_value;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            lookup_image_color ((double)data[k],
                                &red, &green, &blue, &trans);
            red_ptr[k] = (unsigned char)red;
            green_ptr[k] = (unsigned char)green;
            blue_ptr[k] = (unsigned char)blue;
            trans_ptr[k] = (unsigned char)trans;
        }
    }

    istat = AddColorImage (red_ptr,
                           green_ptr,
                           blue_ptr,
                           trans_ptr,
                           -ncol,
                           nrow,
                           xmin,
                           ymin,
                           xmax,
                           ymax);

    return istat;

}  /*  end of function AddDataImage  (short int) */




/*
    Add an image based on an array of unsigned char values, using the current
    image value arrays from the most recent call to SetImageColors and
    also the options from SetImageOptions.  The name is the most recent
    name from SetImageName.
*/
int CDisplayList::AddDataImage (unsigned char *data,
                                int ncol,
                                int nrow,
                                char null_value,
                                double xmin,
                                double ymin,
                                double xmax,
                                double ymax)
{
    int            istat, i, j, k, offset, ntot;
    unsigned char  *red_ptr, *green_ptr, *blue_ptr, *trans_ptr;
    int            red, green, blue, trans;

    if (xmax <= xmin || ymax <= ymin ||
        data == NULL || ncol < 2  ||  nrow < 2) {
        return 0;
    }

/*
 * Allocate space for color image data.
 */
    ntot = ncol * nrow;
    red_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    green_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    blue_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    trans_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    if (red_ptr == NULL  ||  green_ptr == NULL  ||  blue_ptr == NULL  ||
        trans_ptr == NULL) {
        csw_Free (red_ptr);
        csw_Free (green_ptr);
        csw_Free (blue_ptr);
        csw_Free (trans_ptr);
        return -1;
    }

/*
 * convert data image to color image.
 */
    current_image_null_value = (double)null_value;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            lookup_image_color ((double)data[k],
                                &red, &green, &blue, &trans);
            red_ptr[k] = (unsigned char)red;
            green_ptr[k] = (unsigned char)green;
            blue_ptr[k] = (unsigned char)blue;
            trans_ptr[k] = (unsigned char)trans;
        }
    }

    istat = AddColorImage (red_ptr,
                           green_ptr,
                           blue_ptr,
                           trans_ptr,
                           -ncol,
                           nrow,
                           xmin,
                           ymin,
                           xmax,
                           ymax);

    return istat;

}  /*  end of function AddDataImage  (char) */



void CDisplayList::setup_color_band_index (void)
{
    int          i, j, jt1, j2;
    CSW_F        fmin, fmax, dz, z1, z2;

    for (i=0; i<_DL_NUM_COLOR_BAND_INDEX_; i++) {
        color_band_index[i] = -1;
    }

    if (current_number_image_values < 1) {
        return;
    }

    fmin = 1.e30f;
    fmax = -1.e30f;

    for (i=0; i<current_number_image_values; i++) {
        if (current_image_low_values[i] < fmin) fmin = current_image_low_values[i];
        if (current_image_low_values[i] > fmax) fmax = current_image_low_values[i];
        if (current_image_high_values[i] < fmin) fmin = current_image_high_values[i];
        if (current_image_high_values[i] > fmax) fmax = current_image_high_values[i];
    }

    if (fmin >= fmax) {
        return;
    }

    dz = (fmax - fmin) / 1000.0f;
    fmin -= dz;
    fmax += dz;

    min_color_band_index = fmin;
    max_color_band_index = fmax;
    delta_color_band_index = (fmax - fmin) / (CSW_F)(_DL_NUM_COLOR_BAND_INDEX_ - 1);

    for (i=0; i<current_number_image_values; i++) {
        z1 = current_image_low_values[i];
        z2 = current_image_high_values[i];
        jt1 = (int)((z1 - fmin) / delta_color_band_index);
        j2 = (int)((z2 - fmin) / delta_color_band_index);
        for (j=jt1; j<=j2; j++) {
            color_band_index[j] = i;
        }
    }

    return;

}  /* end of setup_color_band_index function */


void CDisplayList::lookup_image_color (double fval,
                                       int *red,
                                       int *green,
                                       int *blue,
                                       int *trans)
{
    int           i, j;

/*
 * initialize to completely transparent.
 */
    *red = 0;
    *green = 0;
    *blue = 0;
    *trans = 0;

    if (current_image_null_value != 0.0) {
        if (fval >= current_image_null_value) {
            return;
        }
    }

/*
 * if the value is out of the range or otherwise doesn't
 * have color defined for it, leave the colors transparent.
 */
    if (fval <= min_color_band_index  ||  fval >= max_color_band_index) {
        return;
    }

    i = (int)((fval - min_color_band_index) / delta_color_band_index + 0.5f);
    if (i < 0) i = 0;
    if (i >= _DL_NUM_COLOR_BAND_INDEX_) {
        i = _DL_NUM_COLOR_BAND_INDEX_ - 1;
    }

    j = color_band_index[i];
    if (j < 0  ||  j >= current_number_image_values) {
        return;
    }

/*
 * this redundant check on j is to make lint happy
 */
    if (j >= 0) {
        *red = current_image_red_values[j];
        *green = current_image_green_values[j];
        *blue = current_image_blue_values[j];
        *trans = current_image_transparency_values[j];
    }

    return;

}


int CDisplayList::find_frame (char *name)
{
    int            i, cval;
    char           local1[100], local2[100];
    FRameStruct    *frptr;

    if (name == NULL) {
        return -1;
    }

    csw_StrClean2 (local1, name, 100);
    if (frame_list != NULL) {
        for (i=0; i<num_frame_list; i++) {
            frptr = frame_list + i;
            csw_StrClean2 (local2, frptr->name, 100);
            cval = strcmp (local1, local2);
            if (cval == 0) {
                return i;
            }
        }
    }

    return -1;
}


int CDisplayList::SetFrame (char *name)
{
    int            i, cval;
    char           local1[100], local2[100];
    FRameStruct    *frptr;

    if (name == NULL) {
        current_frame_num = -1;
        return -1;
    }

    csw_StrClean2 (local1, name, 100);
    if (frame_list != NULL) {
        for (i=0; i<num_frame_list; i++) {
            frptr = frame_list + i;
            csw_StrClean2 (local2, frptr->name, 100);
            cval = strcmp (local1, local2);
            if (cval == 0) {
                current_frame_num = i;
                update_frame_limits ();
                return i;
            }
        }
    }

    return -1;
}

int CDisplayList::ZoomOut (int frame_num)
{
    FRameStruct  *frptr;
    double       xmin, ymin, xmax, ymax;
    double       dx, dy;

    if (frame_num < 0  ||  frame_num >= num_frame_list  ||  frame_list == NULL) {
        return -1;
    }

    frptr = frame_list + frame_num;

    if (frptr->rescaleable == 0) {
        return -1;
    }

    xmin = frptr->x1;
    ymin = frptr->y1;
    xmax = frptr->x2;
    ymax = frptr->y2;

    dx = xmax - xmin;
    dy = ymax - ymin;
    dx /= 2.0;
    dy /= 2.0;

    xmin -= dx;
    xmax += dx;
    ymin -= dy;
    ymax += dy;

    RescaleFrame (frptr->name, 0,
                  xmin, ymin, xmax, ymax);

    return 1;
}


int CDisplayList::ZoomExtents (int frame_num)
{
    FRameStruct  *frptr;
    double       xmin, ymin, xmax, ymax;
    double       x1, y1t, x2, y2, dx, dy, adx, ady;

    if (frame_num < 0  ||  frame_num >= num_frame_list  ||  frame_list == NULL) {
        return -1;
    }

    frptr = frame_list + frame_num;

    if (frptr->at_zoom_extents == 1) {
        return 1;
    }

    if (frptr->rescaleable == 0) {
        return -1;
    }

    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;

    find_frame_limits (frame_num, &xmin, &ymin, &xmax, &ymax);
    if (xmin >= xmax  ||  ymin >= ymax) {
        xmin = frptr->xmin;
        xmax = frptr->xmax;
        ymin = frptr->ymin;
        ymax = frptr->ymax;
    }

    dx = frptr->x2 - frptr->x1;
    dy = frptr->y2 - frptr->y1;

    adx = xmax - xmin;
    ady = ymax - ymin;
    adx /= 50.0;
    ady /= 50.0;

    if (dx > 0.0) {
        x1 = xmin - adx - frptr->left_inset;
        x2 = xmax + adx + frptr->right_inset;
    }
    else {
        x1 = xmax + adx + frptr->left_inset;
        x2 = xmin - adx - frptr->right_inset;
    }
    if (dy > 0.0) {
        y1t = ymin - ady - frptr->bottom_inset;
        y2 = ymax + ady + frptr->top_inset;
    }
    else {
        y1t = ymax + ady + frptr->bottom_inset;
        y2 = ymin - ady - frptr->top_inset;
    }

    RescaleFrame (frptr->name, 0,
                  x1, y1t, x2, y2);

    frptr->at_zoom_extents = 1;

    return 1;
}

/*
 * This version of the rescale gets a window in screen coordinates.
 * It is used by the zooming code.
 */
int CDisplayList::RescaleFrame (int frame_num,
                                int resize_border_flag,
                                int ix1,
                                int iy1,
                                int ix2,
                                int iy2)
{
    FRameStruct       *frptr;
    CSW_F             px1sav, py1sav, px2sav, py2sav;
    CSW_F             xt1, yt1, xt2, yt2, w, h;
    double            fx1, fy1, fx2, fy2;
    double            fx1sav, fy1sav, fx2sav, fy2sav;
    int               framesav;
    int               idx, idy;

    if (frame_num < 0  ||  frame_num >= num_frame_list  ||  frame_list == NULL) {
        return -1;
    }

    idx = ix2 - ix1;
    if (idx < 0) idx = -idx;
    if (idx < 2) return -1;
    idy = iy2 - iy1;
    if (idy < 0) idy = -idy;
    if (idy < 2) return -1;

    frptr = frame_list + frame_num;

    px1sav = Px1;
    py1sav = Py1;
    px2sav = Px2;
    py2sav = Py2;
    fx1sav = Fx1;
    fy1sav = Fy1;
    fx2sav = Fx2;
    fy2sav = Fy2;
    framesav = current_frame_num;

    Px1 = frptr->px1;
    Py1 = frptr->py1;
    Px2 = frptr->px2;
    Py2 = frptr->py2;
    Fx1 = (CSW_F)frptr->x1;
    Fy1 = (CSW_F)frptr->y1;
    Fx2 = (CSW_F)frptr->x2;
    Fy2 = (CSW_F)frptr->y2;
    current_frame_num = frame_num;

    backscalef ((CSW_F)ix1, (CSW_F)iy1, &xt1, &yt1);
    backscalef ((CSW_F)ix2, (CSW_F)iy2, &xt2, &yt2);
    w = xt2 - xt1;
    if (w < 0.0) {
        xt1 = xt2;
        w = -w;
        xt2 = xt1 + w;
    }
    h = yt2 - yt1;
    if (h < 0.0) {
        yt1 = yt2;
        h = -h;
        yt2 = yt1 + h;
    }

    unconvert_frame_point (&xt1, &yt1);
    fx1 = xt1;
    fy1 = yt1;
    unconvert_frame_point (&xt2, &yt2);
    fx2 = xt2;
    fy2 = yt2;

    Px1 = px1sav;
    Py1 = py1sav;
    Px2 = px2sav;
    Py2 = py2sav;
    Fx1 = (CSW_F)fx1sav;
    Fy1 = (CSW_F)fy1sav;
    Fy2 = (CSW_F)fx2sav;
    Fy2 = (CSW_F)fy2sav;
    current_frame_num = framesav;

    RescaleFrame (frptr->name,
                  resize_border_flag,
                  fx1, fy1, fx2, fy2);

    return 1;

}


int CDisplayList::RescaleFrame (char *name, int resize_border_flag,
                                double fx1in, double fy1in, double fx2in, double fy2in)
{
    int            i, cval, main_frame_num;
    char           local1[100], local2[100];
    CSW_F          pxmin, pymin, pxmax, pymax;
    FRameStruct    *frptr, *frmain;
    double         dfx, dfy, dfx2, dfy2, xmid, ymid;
    double         fx1, fy1, fx2, fy2;

    double         oldx1, oldy1, oldx2, oldy2;

    if (name == NULL) {
        return -1;
    }

    oldx1 = oldy1 = oldx2 = oldy2 = 1.e30;

    fx1 = fx1in;
    fy1 = fy1in;
    fx2 = fx2in;
    fy2 = fy2in;

    main_frame_num = -1;
    frmain = NULL;

    csw_StrClean2 (local1, name, 100);
    if (frame_list != NULL) {

    /*
     * Rescale the specified frame.
     */
        for (i=0; i<num_frame_list; i++) {
            frptr = frame_list + i;

        /*
         * The frame will not be zoomed by a factor larger than 100000.
         * If the min/max of the frame are not valid, the frame cannot
         * be rescaled.
         */
            dfx = frptr->xmax - frptr->xmin;
            dfy = frptr->ymax - frptr->ymin;
            if (dfx <= 0.0  ||  dfy <= 0.0) {
                continue;
            }
            dfx /= 200000.0;
            dfy /= 200000.0;

            csw_StrClean2 (local2, frptr->name, 100);
            cval = strcmp (local1, local2);
            if (cval == 0) {
                if (frptr->rescaleable == 0) {
                    return -1;
                }
                if (frptr->scale_width_to_attach_frame == 1  ||
                    frptr->scale_height_to_attach_frame == 1) {
                    return -1;
                }

                oldx1 = frptr->x1;
                oldy1 = frptr->y1;
                oldx2 = frptr->x2;
                oldy2 = frptr->y2;

                pxmin = frptr->px1;
                pymin = frptr->py1;
                pxmax = frptr->px2;
                pymax = frptr->py2;
                force_frame_aspect (
                    frptr->aspect_flag, resize_border_flag,
                    &pxmin, &pymin,
                    &pxmax, &pymax,
                    &fx1, &fy1, &fx2, &fy2);

            /*
             * Don't allow rescale to a very small subset of the frame.
             * If this is allowed, we risk exceptions from zero divides and
             * other nasty things.
             */
                dfx2 = fx2 - fx1;
                dfy2 = fy2 - fy1;
                if (dfx2 < 0.0) dfx2 = -dfx2;
                if (dfy2 < 0.0) dfy2 = -dfy2;
                if (dfx2 < dfx  ||  dfy2 < dfy) {
                    xmid = (fx1 + fx2) / 2.0;
                    fx1 = xmid - dfx;
                    fx2 = xmid + dfx;
                    ymid = (fy1 + fy2) / 2.0;
                    fy1 = ymid - dfy;
                    fy2 = ymid + dfy;
                    force_frame_aspect (
                        frptr->aspect_flag, resize_border_flag,
                        &pxmin, &pymin,
                        &pxmax, &pymax,
                        &fx1, &fy1, &fx2, &fy2);
                }

                frptr->px1orig = pxmin;
                frptr->py1orig = pymin;
                frptr->px2orig = pxmax;
                frptr->py2orig = pymax;
                frptr->px1 = pxmin;
                frptr->py1 = pymin;
                frptr->px2 = pxmax;
                frptr->py2 = pymax;
                frptr->x1 = fx1;
                frptr->y1 = fy1;
                frptr->x2 = fx2;
                frptr->y2 = fy2;
                frptr->rescale_needed = 1;
                frptr->reborder_needed = 1;
                frame_layout_needed = 1;
                frptr->at_zoom_extents = 0;
                main_frame_num = i;
                frmain = frptr;

                ezx_java_obj.SetZoomPan (
                    frptr->name,
                    oldx1, oldy1, oldx2, oldy2,
                    fx1, fy1, fx2, fy2);

                break;
            }
        }

        if (main_frame_num == -1  ||  frmain == NULL) {
            return -1;
        }

        fx1in = fx1;
        fy1in = fy1;
        fx2in = fx2;
        fy2in = fy2;

    /*
     * Rescale any frames attached to the main frame that need it.
     */
        for (i=0; i<num_frame_list; i++) {

            if (i == main_frame_num) {
                continue;
            }

            frptr = frame_list + i;
            if (frptr->scale_width_to_attach_frame == 0  &&
                frptr->scale_height_to_attach_frame == 0) {
                continue;
            }
            if (frptr->attach_frame != main_frame_num) {
                continue;
            }

        /*
         * Rescale x and y as needed.
         */
            fx1 = frptr->x1;
            fx2 = frptr->x2;
            fy1 = frptr->y1;
            fy2 = frptr->y2;

            if (frptr->scale_height_to_attach_frame == 1) {
                fy1 = fy1in;
                fy2 = fy2in;
                frptr->py1 = frmain->py1;
                frptr->py2 = frmain->py2;
            }
            if (frptr->scale_width_to_attach_frame == 1) {
                fx1 = fx1in;
                fx2 = fx2in;
                frptr->px1 = frmain->px1;
                frptr->px2 = frmain->px2;
            }

        /*
         * The frame will not be zoomed by a factor larger than 100000.
         * If the min/max of the frame are not valid, the frame cannot
         * be rescaled.
         */
            dfx = frptr->xmax - frptr->xmin;
            dfy = frptr->ymax - frptr->ymin;
            if (dfx <= 0.0  ||  dfy <= 0.0) {
                continue;
            }
            dfx /= 200000.0;
            dfy /= 200000.0;

            pxmin = frptr->px1;
            pymin = frptr->py1;
            pxmax = frptr->px2;
            pymax = frptr->py2;
            force_frame_aspect (
                frptr->aspect_flag, resize_border_flag,
                &pxmin, &pymin,
                &pxmax, &pymax,
                &fx1, &fy1, &fx2, &fy2);

        /*
         * Don't allow rescale to a very small subset of the frame.
         * If this is allowed, we risk exceptions from zero divides and
         * other nasty things.
         */
            dfx2 = fx2 - fx1;
            dfy2 = fy2 - fy1;
            if (dfx2 < 0.0) dfx2 = -dfx2;
            if (dfy2 < 0.0) dfy2 = -dfy2;
            if (dfx2 < dfx  ||  dfy2 < dfy) {
                xmid = (fx1 + fx2) / 2.0;
                fx1 = xmid - dfx;
                fx2 = xmid + dfx;
                ymid = (fy1 + fy2) / 2.0;
                fy1 = ymid - dfy;
                fy2 = ymid + dfy;
                force_frame_aspect (
                    frptr->aspect_flag, resize_border_flag,
                    &pxmin, &pymin,
                    &pxmax, &pymax,
                    &fx1, &fy1, &fx2, &fy2);
            }

            frptr->px1orig = pxmin;
            frptr->py1orig = pymin;
            frptr->px2orig = pxmax;
            frptr->py2orig = pymax;
            frptr->px1 = pxmin;
            frptr->py1 = pymin;
            frptr->px2 = pxmax;
            frptr->py2 = pymax;
            frptr->x1 = fx1;
            frptr->y1 = fy1;
            frptr->x2 = fx2;
            frptr->y2 = fy2;
            frptr->rescale_needed = 1;
            frptr->reborder_needed = 1;
            frame_layout_needed = 1;
            frptr->at_zoom_extents = 0;
        }
    }

    return main_frame_num;
}


int CDisplayList::UnsetFrame (void)
{
    current_frame_num = -1;
    return 1;
}

int CDisplayList::CreateFrame (int rescaleable,
                               int moveable,
                               int aspect_flag,
                               double xmin,
                               double ymin,
                               double xmax,
                               double ymax,
                               double fx1,
                               double fy1,
                               double fx2,
                               double fy2,
                               CSW_F px1,
                               CSW_F py1,
                               CSW_F px2,
                               CSW_F py2,
                               int borderflag,
                               int attach_position,
                               double extra_gap,
                               double perp_move,
                               char *cdata,
                               int scale_width_to_attach_frame,
                               int scale_height_to_attach_frame)
{
    FRameStruct       *frptr, *fp2;
    double            dt;
    CSW_F             ft;
    int               ilist, i;
    char              name[100];
    char              attach_name[200];
    char              left_caption[200],
                      right_caption[200],
                      top_caption[200],
                      bottom_caption[200];
    int               attach_num;
    int               sindex[10];
    int               cval, ntot, nr, nc;
    char              local1[100], local2[100];
    char              c2000[2000];
    CSW_F             dx1, dx2;
    double            dx, dy, aspect, ratio;

    if (cdata == NULL) {
        return 0;
    }

    if (frame_list == NULL  ||  num_frame_list >= max_frame_list) {
        ilist = max_frame_list;
        max_frame_list += _SMALL_CHUNK_SIZE_;
        frame_list = (FRameStruct *)csw_Realloc (frame_list,
                              max_frame_list * sizeof(FRameStruct));
        if (frame_list) {
            memset (frame_list + ilist, 0,
                    _SMALL_CHUNK_SIZE_ * sizeof(FRameStruct));
        }

    }

    if (frame_list == NULL) {
        return -1;
    }

  /*
   * extract names from cdata string
   */
    strncpy (c2000, cdata, 1999);
    c2000[1999] = '\0';
    name[0] = '\0';
    attach_name[0] = '\0';
    left_caption[0] = '\0';
    right_caption[0] = '\0';
    top_caption[0] = '\0';
    bottom_caption[0] = '\0';

    extract_sub_strings (c2000, sindex, 4);

    strncpy (name, c2000 + sindex[0], 99);
    name[99] = '\0';
    if (sindex[1] >= 0) {
        strncpy (attach_name, c2000+sindex[1], 99);
        attach_name[99] = '\0';
    }
    if (sindex[2] >= 0) {
        strncpy (bottom_caption, c2000+sindex[2], 199);
        bottom_caption[199] = '\0';
        strcpy (top_caption, bottom_caption);
    }
    if (sindex[3] >= 0) {
        strncpy (left_caption, c2000+sindex[3], 199);
        left_caption[199] = '\0';
        strcpy (right_caption, left_caption);
    }

/*
 * return -2 if name is null string or already used.
 */
    if (name[0] == '\0') {
        return -2;
    }

    csw_StrClean2 (local1, name, 100);
    for (i=0; i<num_frame_list; i++) {
        frptr = frame_list + i;
        csw_StrClean2 (local2, frptr->name, 100);
        cval = strcmp (local1, local2);
        if (cval == 0) {
            return -2;
        }
    }

    attach_num = find_frame (attach_name);

  /*
   * Make sure all the attachment flags are consistent.
   */
    if (attach_num < 0  ||  attach_position == 0) {
        scale_width_to_attach_frame = 0;
        scale_height_to_attach_frame = 0;
    }

    if (scale_width_to_attach_frame == 1  ||
        scale_height_to_attach_frame == 1) {
        rescaleable = 1;
    }

  /*
   * make sure the page corners are lower left and upper right.
   */
    if (px2 < px1) {
        ft = px1;
        px1 = px2;
        px2 = ft;
        dt = fx1;
        fx1 = fx2;
        fx2 = dt;
    }
    if (py2 < py1) {
        ft = py1;
        py1 = py2;
        py2 = ft;
        dt = fy1;
        fy1 = fy2;
        fy2 = dt;
    }

  /*
   * if the aspect flag is zero, force the frame limits to the
   * same aspect ratio as the page limits.  Do not allow the
   * page units border to change in this operation.
   */
    if (aspect_flag != GTX_ALLOW_ANY_FRAME_ASPECT_RATIO) {
        aspect_flag = 0;
    }
    force_frame_aspect (aspect_flag, 0,
                        &px1, &py1, &px2, &py2,
                        &fx1, &fy1, &fx2, &fy2);

    frptr = frame_list + num_frame_list;

    frptr->attach_frame = attach_num;
    frptr->attach_position = attach_position;
    frptr->extra_gap = (CSW_F)extra_gap;
    frptr->perpendicular_move = (CSW_F)perp_move;

    default_frame_axes (frptr);
    frptr->borderflag = borderflag;
    if (borderflag == GTX_FRAME_LABEL_LEFT_BOTTOM_BORDER) {
        frptr->left_axis.label_flag = 1;
        frptr->left_axis.tick_flag = 1;
        frptr->bottom_axis.label_flag = 1;
        frptr->bottom_axis.tick_flag = 1;
    }
    if (borderflag == GTX_FRAME_LABEL_LEFT_TOP_BORDER) {
        frptr->left_axis.label_flag = 1;
        frptr->left_axis.tick_flag = 1;
        frptr->top_axis.label_flag = 1;
        frptr->top_axis.tick_flag = 1;
    }
    if (borderflag == GTX_FRAME_LABEL_RIGHT_BOTTOM_BORDER) {
        frptr->right_axis.label_flag = 1;
        frptr->right_axis.tick_flag = 1;
        frptr->bottom_axis.label_flag = 1;
        frptr->bottom_axis.tick_flag = 1;
    }
    if (borderflag == GTX_FRAME_LABEL_RIGHT_TOP_BORDER) {
        frptr->right_axis.label_flag = 1;
        frptr->right_axis.tick_flag = 1;
        frptr->top_axis.label_flag = 1;
        frptr->top_axis.tick_flag = 1;
    }
    if (borderflag == GTX_FRAME_LABEL_ALL_SIDES_BORDER) {
        frptr->left_axis.label_flag = 1;
        frptr->left_axis.tick_flag = 1;
        frptr->bottom_axis.label_flag = 1;
        frptr->bottom_axis.tick_flag = 1;
        frptr->right_axis.label_flag = 1;
        frptr->right_axis.tick_flag = 1;
        frptr->top_axis.label_flag = 1;
        frptr->top_axis.tick_flag = 1;
    }

    strcpy (frptr->right_axis.caption, right_caption);
    strcpy (frptr->left_axis.caption, left_caption);
    strcpy (frptr->top_axis.caption, top_caption);
    strcpy (frptr->bottom_axis.caption, bottom_caption);

    dx = (xmax - xmin) / 100.0;
    dy = (ymax - ymin) / 100.0;
    frptr->xmin = xmin - dx;
    frptr->ymin = ymin - dy;
    frptr->xmax = xmax + dx;
    frptr->ymax = ymax + dy;
    frptr->x1 = fx1;
    frptr->y1 = fy1;
    frptr->x2 = fx2;
    frptr->y2 = fy2;
    frptr->px1 = px1;
    frptr->py1 = py1;
    frptr->px2 = px2;
    frptr->py2 = py2;
    frptr->px1orig = px1;
    frptr->py1orig = py1;
    frptr->px2orig = px2;
    frptr->py2orig = py2;
    strncpy (frptr->name, name, 99);
    frptr->name[99] = '\0';
    frptr->rescaleable = rescaleable;
    frptr->scale_width_to_attach_frame = scale_width_to_attach_frame;
    frptr->scale_height_to_attach_frame = scale_height_to_attach_frame;
    frptr->moveable = moveable;
    frptr->clipflag = current_frame_clip_flag;

    frptr->reborder_needed = 1;

    frptr->aspect_flag = aspect_flag;
    frptr->fill_index = NULL;
    frptr->line_index = NULL;
    frptr->text_index = NULL;
    frptr->symb_index = NULL;
    frptr->shape_index = NULL;
    frptr->contour_index = NULL;

    frptr->ncol = 0;
    frptr->nrow = 0;
    frptr->xspace = 0.0;
    frptr->yspace = 0.0;

    frptr->aspect_flag = aspect_flag;

    current_frame_num = num_frame_list;
    num_frame_list++;

    update_frame_limits ();

    if (borderflag != 0) {
        if (px1 < page_xmin) page_xmin = px1;
        if (py1 < page_ymin) page_ymin = py1;
        if (px2 > page_xmax) page_xmax = px2;
        if (py2 > page_ymax) page_ymax = py2;

        if (raw_from_hints_flag == 1) {
            raw_page_xmin = px1;
            raw_page_ymin = py1;
            raw_page_xmax = px2;
            raw_page_ymax = py2;
            raw_from_hints_flag = 0;
        }
        else {
            if (px1 < raw_page_xmin) raw_page_xmin = px1;
            if (py1 < raw_page_ymin) raw_page_ymin = py1;
            if (px2 > raw_page_xmax) raw_page_xmax = px2;
            if (py2 > raw_page_ymax) raw_page_ymax = py2;
        }

    }

    frptr->xpct = -1.0f;
    frptr->ypct = -1.0f;
    if (attach_num >= 0  &&  attach_num < num_frame_list) {
        fp2 = frame_list + attach_num;
        dx2 = fp2->px2orig - fp2->px1orig;
        dx1 = px2 - px1;
        frptr->xpct = dx1 / dx2;
        dx2 = fp2->py2orig - fp2->py1orig;
        dx1 = py2 - py1;
        frptr->ypct = dx1 / dx2;
    }

    frame_layout_needed = 1;

    frptr->patch_draw_flag = 0;

    frptr->xmin_spatial = xmin;
    frptr->ymin_spatial = ymin;
    frptr->xmax_spatial = xmax;
    frptr->ymax_spatial = ymax;

/*
 * Allocate spatial indexes for the frame.
 */
    if (xmax > xmin  &&  ymax > ymin) {
        dx = xmax - xmin;
        dy = ymax - ymin;
        double    dxy_10 = (dx + dy) / 20.0;
        frptr->xmin_spatial = xmin - dxy_10;
        frptr->ymin_spatial = ymin - dxy_10;
        frptr->xmax_spatial = xmax + dxy_10;
        frptr->ymax_spatial = ymax + dxy_10;
        aspect = dy / dx;
        if (rescaleable != 0) {
            nc = (int)50;
            nr = (int)(50 * aspect + .5);
            ntot = nc * nr;
            ratio = (double)ntot / 2500.0;
            ratio = sqrt (ratio);
            nc = (int)(nc / ratio + 0.5);
            nr = (int)(nr / ratio + 0.5);
            if (nc < 2) nc = 2;
            if (nr < 2) nr = 2;
            nc *= 2;
            nr *= 2;
            frptr->ncol = nc;
            frptr->nrow = nr;
            frptr->xspace = (frptr->xmax_spatial - frptr->xmin_spatial) / (nc - 1);
            frptr->yspace = (frptr->ymax_spatial - frptr->ymin_spatial) / (nr - 1);
        }
        else {
            nc = (int)10;
            nr = (int)(10 * aspect + .5);
            ntot = nc * nr;
            ratio = (double)ntot / 100.0;
            ratio = sqrt (ratio);
            nc = (int)(nc / ratio + 0.5);
            nr = (int)(nr / ratio + 0.5);
            if (nc < 2) nc = 2;
            if (nr < 2) nr = 2;
            frptr->ncol = nc;
            frptr->nrow = nr;
            frptr->xspace = (frptr->xmax_spatial - frptr->xmin_spatial) / (nc - 1);
            frptr->yspace = (frptr->ymax_spatial - frptr->ymin_spatial) / (nr - 1);
        }
        ntot = frptr->ncol * frptr->nrow + 1;
        frptr->line_index = (int **)csw_Calloc (ntot * sizeof(int *));
        frptr->fill_index = (int **)csw_Calloc (ntot * sizeof(int *));
        frptr->text_index = (int **)csw_Calloc (ntot * sizeof(int *));
        frptr->symb_index = (int **)csw_Calloc (ntot * sizeof(int *));
        frptr->shape_index = (int **)csw_Calloc (ntot * sizeof(int *));
        frptr->contour_index = (int **)csw_Calloc (ntot * sizeof(int *));

    }

    frptr->num_line_index = 0;
    frptr->num_fill_index = 0;
    frptr->num_text_index = 0;
    frptr->num_symb_index = 0;
    frptr->num_shape_index = 0;
    frptr->num_contour_index = 0;

    return (num_frame_list - 1);
}



/*
 * Force the frame aspect ratio to be the same as the
 * aspect ratio of its page rectangle.  The page rectangle
 * must be defined by the lower left and upper right corners
 * in page units.
 */
void CDisplayList::force_frame_aspect (int aflag, int resize_border_flag,
                                       CSW_F *pxmin_in, CSW_F *pymin_in,
                                       CSW_F *pxmax_in, CSW_F *pymax_in,
                                       double *frx1, double *fry1,
                                       double *frx2, double *fry2)
{
    double          fx1, fy1, fx2, fy2;
    double          dx, dy, dpx, dpy,
                    orig_dx, orig_dy,
                    adjust_dx, adjust_dy;
    double          dpx_in, dpy_in;
    double          pratio, fratio;
    double          pxmin, pymin, pxmax, pymax;

    resize_border_flag = resize_border_flag;

    if (aflag != 0) {
        return;
    }

    pxmin = *pxmin_in;
    pymin = *pymin_in;
    pxmax = *pxmax_in;
    pymax = *pymax_in;

    dpx_in = pxmax - pxmin;
    dpy_in = pymax - pymin;

    if (dpx_in <= 0.0  ||  dpy_in <= 0.0) {
        return;
    }

    fx1 = *frx1;
    fy1 = *fry1;
    fx2 = *frx2;
    fy2 = *fry2;

/*
 * get the absolute values of each dimension
 * if any dimension is zero, bail out.
 */
    dx = fx2 - fx1;
    dy = fy2 - fy1;
    if (dx < 0.0) dx = -dx;
    if (dy < 0.0) dy = -dy;
    if (dx <= 0.0  ||  dy <= 0.0) return;

    dpx = pxmax - pxmin;
    dpy = pymax - pymin;
    if (dpx <= 0.0  ||  dpy <= 0.0) return;

    orig_dx = dx;
    orig_dy = dy;

    pratio = dpx / dpy;
    fratio = dx / dy;

/*
 * If the borders are to be resized, adjust the
 * plot page limits to the same ratio as the
 * frame limits.
 */
    if (resize_border_flag == 1) {
        if (pratio > fratio) {
            dpy = dpx / fratio;
        }
        else {
            dpx = dpy * fratio;
        }
        pratio = dpx / dpy;
        if (dpx > dpx_in) {
            dpx = dpx_in;
            dpy = dpx / pratio;
        }
        else if (dpy > dpy_in) {
            dpy = dpy_in;
            dpx = dpy * pratio;
        }

        pxmax = pxmin + dpx;
        pymax = pymin + dpy;
    }

/*
 * compare the ratios.  fix mismatches by always
 * making the frame coordinates window larger.
 */
    if (fratio > pratio) {
        dy = dx / pratio;
    }
    else {
        dx = dy * pratio;
    }

    adjust_dx = (dx - orig_dx) / 2.0;
    adjust_dy = (dy - orig_dy) / 2.0;

/*
 * adjust the originals outward from the center and return.
 */
    if (fx2 > fx1) {
        fx1 -= adjust_dx;
        fx2 += adjust_dx;
    }
    else {
        fx1 += adjust_dx;
        fx2 -= adjust_dx;
    }

    if (fy2 > fy1) {
        fy1 -= adjust_dy;
        fy2 += adjust_dy;
    }
    else {
        fy1 += adjust_dy;
        fy2 -= adjust_dy;
    }

    *frx1 = fx1;
    *frx2 = fx2;
    *fry1 = fy1;
    *fry2 = fy2;

    *pxmin_in = (CSW_F)pxmin;
    *pymin_in = (CSW_F)pymin;
    *pxmax_in = (CSW_F)pxmax;
    *pymax_in = (CSW_F)pymax;

    return;

}




void CDisplayList::update_frame_limits (void)
{
    FRameStruct       *frptr;

    if (current_frame_num < 0  ||  current_frame_num >= num_frame_list) {
        return;
    }

    if (frame_list == NULL) {
        return;
    }

    frptr = frame_list + current_frame_num;

    Fx1 = (CSW_F)frptr->x1;
    Fy1 = (CSW_F)frptr->y1;
    Fx2 = (CSW_F)frptr->x2;
    Fy2 = (CSW_F)frptr->y2;
    Px1 = frptr->px1;
    Py1 = frptr->py1;
    Px2 = frptr->px2;
    Py2 = frptr->py2;

    return;

}



void CDisplayList::SetFrameClip (int ival)
{
    current_frame_clip_flag = ival;
}

void CDisplayList::SetFrameNum (int ival)
{
    current_frame_num = ival;
    update_frame_limits ();
}

int CDisplayList::SetGraphName (char *name)
{
    int            i, cval;
    char           local1[100], local2[100];
    GRaphStruct    *grptr;

    current_graph_num = -1;
    current_xaxis_num = -1;
    current_yaxis_num = -1;

    if (name == NULL) {
        return -1;
    }

    csw_StrClean2 (local1, name, 100);
    if (graph_list != NULL) {
        for (i=0; i<num_graph_list; i++) {
            grptr = graph_list + i;
            csw_StrClean2 (local2, grptr->name, 100);
            cval = strcmp (local1, local2);
            if (cval == 0) {
                current_graph_num = i;
                return i;
            }
        }
    }

    return -1;
}

int CDisplayList::CreateGraph (GRaphRec *grec_ptr, char *name)
{
    GRaphStruct      *grptr;
    current_xaxis_num = -1;
    current_yaxis_num = -1;
    current_graph_num = -1;

    if (name == NULL) {
        return -1;
    }

    if (graph_list == NULL  ||  num_graph_list >= max_graph_list) {
        max_graph_list += _SMALL_CHUNK_SIZE_;
        graph_list = (GRaphStruct *)csw_Realloc (graph_list,
                              max_graph_list * sizeof(GRaphStruct));
    }

    if (graph_list == NULL) {
        return -1;
    }

    grptr = graph_list + num_graph_list;
    grptr->graph_rec = grec_ptr;
    strncpy (grptr->name, name, 99);
    grptr->name[99] = '\0';

    num_graph_list++;

    return (num_graph_list - 1);

}

void CDisplayList::SetGraphNum (int ival)
{
    current_graph_num = ival;
}

void CDisplayList::SetSelectableNum (int ival)
{
    current_selectable_object_num = ival;
    if (ival >= 0) {
        AddSelectableObject (ival);
    }
    else {
        current_selectable_object = NULL;
    }
}

void CDisplayList::EraseSelectableNum (int index)
{
    DLSelectable        *dls;

    if (selectable_object_list == NULL) {
        return;
    }
    if (index < 0  ||  index >= num_selectable_object_list) {
        return;
    }

    dls = selectable_object_list[index];
    if (dls == NULL) {
        return;
    }

    erase_selected_lines (dls);
    erase_selected_fills (dls);
    erase_selected_texts (dls);
    erase_selected_symbs (dls);
    erase_selected_shapes (dls);
    erase_selected_contours (dls);
    erase_selected_axes (dls);
}

void CDisplayList::AddSelectableObject (int object_index)
{
    int               jlast, j;
    DLSelectable      *dls;

    if (object_index < 0) {
        return;
    }
    if (object_index > 10000000) {
        return;
    }

    if (object_index >= max_selectable_object_list) {
        jlast = max_selectable_object_list;
        while (object_index > max_selectable_object_list) {
            max_selectable_object_list *= 2;
        }
        max_selectable_object_list += 100;
        selectable_object_list = (DLSelectable **)csw_Realloc
                                 (selectable_object_list,
                                  max_selectable_object_list * sizeof(DLSelectable *));
        if (selectable_object_list != NULL) {
            for (j=jlast; j<max_selectable_object_list; j++) {
                selectable_object_list[j] = NULL;
            }
        }
    }

    if (selectable_object_list == NULL) {
        return;
    }

    try {
        SNF;
        dls = new DLSelectable ();
    }
    catch (...) {
        dls = NULL;
    }
    selectable_object_list[object_index] = dls;
    current_selectable_object = dls;

    if (object_index >= num_selectable_object_list) {
        num_selectable_object_list = object_index + 1;
    }

    return;

}

/*void CDisplayList::SetSelectState(
    int index,
    int ival
) {

    DLSelectable   *dls;


    if (selectable_object_list == NULL  ||
        num_selectable_object_list < 1) {
        return;
    }
    if (index >= num_selectable_object_list) {
        return;
    }
    dls = selectable_object_list[index];
    if (dls == NULL)
      return;
    dls->SetSelectState (ival);
    return;
} */


void CDisplayList::SetXaxisNum (int ival)
{
    current_xaxis_num = ival;
}

void CDisplayList::SetYaxisNum (int ival)
{
    current_yaxis_num = ival;
}

int CDisplayList::SetLayerName (char *name)
{
    int            i, cval;
    char           local1[100], local2[100];
    LAyerStruct    *lptr;

    current_layer_num = -1;

    if (name == NULL) {
        return -1;
    }

    csw_StrClean2 (local1, name, 100);
    if (layer_list != NULL) {
        for (i=0; i<num_layer_list; i++) {
            lptr = layer_list + i;
            csw_StrClean2 (local2, lptr->name, 100);
            cval = strcmp (local1, local2);
            if (cval == 0) {
                current_layer_num = i;
                return i;
            }
        }
    }

    return -1;
}

int CDisplayList::UnsetLayer (void)
{
    current_layer_num = -1;
    return 1;
}

int CDisplayList::CreateLayer (int layer_num, int sel_flag, char *name)
{
    LAyerStruct      *lptr;
    current_layer_num = -1;

    if (name == NULL) {
        return -1;
    }

    if (layer_list == NULL  ||  num_layer_list >= max_layer_list) {
        max_layer_list += _SMALL_CHUNK_SIZE_;
        layer_list = (LAyerStruct *)csw_Realloc (layer_list,
                              max_layer_list * sizeof(LAyerStruct));
    }

    if (layer_list == NULL) {
        return -1;
    }

    lptr = layer_list + num_layer_list;
    lptr->number = layer_num;
    lptr->selectable_flag = sel_flag;
    strncpy (lptr->name, name, 99);
    lptr->name[99] = '\0';

    num_layer_list++;

    return (num_layer_list - 1);

}

void CDisplayList::SetLayerNum (int ival)
{
    current_layer_num = ival;
}

void CDisplayList::SetLayerSelectableFlag (int layer_num, int flag)
{
    LAyerStruct         *lptr;

    if (layer_list == NULL) {
        return;
    }

    if (layer_num < 0  ||  layer_num >= num_layer_list) {
        return;
    }

    lptr = layer_list + layer_num;

    lptr->selectable_flag = flag;

}


void CDisplayList::SetLayerSelectableFlag (char *name, int flag)
{
    int                 i, cval;
    char                local1[100], local2[100];
    LAyerStruct         *lptr;

    if (name == NULL) {
        return;
    }

    csw_StrClean2 (local1, name, 100);
    if (layer_list != NULL) {
        for (i=0; i<num_layer_list; i++) {
            lptr = layer_list + i;
            csw_StrClean2 (local2, lptr->name, 100);
            cval = strcmp (local1, local2);
            if (cval == 0) {
                lptr->selectable_flag = flag;
                return;
            }
        }
    }

    return;
}



int CDisplayList::SetItemName (char *name)
{
    int            i, cval;
    char           local1[100], local2[100];
    ITemStruct     *iptr;

    current_item_num = -1;

    if (name == NULL) {
        return -1;
    }

    csw_StrClean2 (local1, name, 100);
    if (item_list != NULL) {
        for (i=0; i<num_item_list; i++) {
            iptr = item_list + i;
            csw_StrClean2 (local2, iptr->name, 100);
            cval = strcmp (local1, local2);
            if (cval == 0) {
                current_item_num = i;
                return i;
            }
        }
    }

    return -1;
}

int CDisplayList::UnsetItem (void)
{
    current_item_num = -1;
    return 1;
}

int CDisplayList::CreateItem (int item_num, char *name)
{
    ITemStruct       *iptr;

    current_item_num = -1;

    if (name == NULL) {
        return -1;
    }

    if (item_list == NULL  ||  num_item_list >= max_item_list) {
        max_item_list += _SMALL_CHUNK_SIZE_;
        item_list = (ITemStruct *)csw_Realloc (item_list,
                              max_item_list * sizeof(ITemStruct));
    }

    if (item_list == NULL) {
        return -1;
    }

    iptr = item_list + num_item_list;
    iptr->number = item_num;
    strncpy (iptr->name, name, 99);
    iptr->name[99] = '\0';

    num_item_list++;

    return (num_item_list - 1);

}

void CDisplayList::SetItemNum (int ival)
{
    current_item_num = ival;
}

int CDisplayList::SetSizeUnits (char *name)
{
    int            i, cval;
    char           local1[100], local2[100];

    current_size_units = -1;

    if (name == NULL) {
        return -1;
    }

    csw_StrClean2 (local1, name, 100);
    if (item_list != NULL) {
        for (i=0; i<_DL_NUM_SIZE_UNITS_LIST_; i++) {
            csw_StrClean2 (local2, size_units_list[i], 100);
            cval = strcmp (local1, local2);
            if (cval == 0) {
                current_size_units = i;
                return i;
            }
        }
    }

    return -1;
}

void CDisplayList::SetSizeUnitsNum (int ival)
{
    if (ival < 0  ||  ival > _DL_NUM_SIZE_UNITS_LIST_) {
        ival = 0;
    }
    current_size_units = ival;
}

void CDisplayList::SetLineThickness (double dval)
{
    if (dval < 0.001) dval = 0.001;
    if (dval > 10.00) dval = 10.00;
    current_line_thickness = (CSW_F)dval;
}

void CDisplayList::SetLinePattern (int ival, double dval)
{
    if (ival < 0) ival = 0;
    if (ival > 30 ) ival = 0;
    current_line_pattern = ival;
    if (dval < 0.01) dval = 1.0;
    if (dval > 100.0) dval = 1.0;
    current_line_pattern_size = (CSW_F)dval;
}

void CDisplayList::SetLineArrow (int ival)
{
    current_line_arrow_style = ival;
}

void CDisplayList::SetLineSymbol (int ival)
{
    if (ival < 0) ival = -1;
    current_line_symbol = ival;
}

void CDisplayList::SetTextFont (int ival)
{
    current_text_font = ival;
}

void CDisplayList::SetTextBackground (int ival)
{
    current_text_background = ival;
}

void CDisplayList::SetTextThickness (double dval)
{
    if (dval < 0.001) dval = 0.001;
    if (dval > 10.00) dval = 10.00;
    current_text_thickness = (CSW_F)dval;
}

void CDisplayList::SetTextAnchor (int ival)
{
    current_text_anchor = ival;
}

void CDisplayList::SetTextOffsets (double xoff, double yoff)
{
    current_text_x_offset = (CSW_F)xoff;
    current_text_y_offset = (CSW_F)yoff;
}


void CDisplayList::SetFillPattern (int ival, double dval1)
{
    if (ival < 0) ival = -1;
    current_fill_pattern = ival;
    if (dval1 < 0.1) dval1 = 0.1;
    if (dval1 > 100.0) dval1 = 100.0;
    current_fill_pattern_size = (CSW_F)dval1;
}


void CDisplayList::SetImageColors (double *low, double *high,
                                   int *red, int *green, int *blue, int *alpha,
                                   int ncolors)
{
    int                i;

    current_number_image_values = 0;

    if (low == NULL  ||  high == NULL  || alpha == NULL  ||
        red == NULL  ||  green == NULL  ||  blue == NULL) {
        return;
    }

    if (ncolors < 0) ncolors = 0;
    if (ncolors > 1000) ncolors = 1000;

    for (i=0; i<ncolors; i++) {
        current_image_low_values[i] = (CSW_F)low[i];
        current_image_high_values[i] = (CSW_F)high[i];
        current_image_red_values[i] = red[i];
        current_image_green_values[i] = green[i];
        current_image_blue_values[i] = blue[i];
        current_image_transparency_values[i] = alpha[i];
    }

    current_number_image_values = ncolors;

    setup_color_band_index ();

    return;
}


void CDisplayList::SetImageName (char *name)
{
    current_image_name[0] = '\0';
    if (name == NULL) {
        return;
    }
    strncpy (current_image_name, name, 99);
    current_image_name[99] = '\0';
}

void CDisplayList::SetImageID (int id)
{
    current_image_id = id;
}

void CDisplayList::SetImageOptions (int origin_flag,
                                    int column1,
                                    int row1,
                                    int colspace,
                                    int rowspace)
{
    current_image_origin = origin_flag;
    current_image_column1 = column1;
    current_image_row1 = row1;
    current_image_colspace = colspace;
    current_image_rowspace = rowspace;

    return;
}

void CDisplayList::SetBackgroundColor (int red, int green, int blue, int alpha)
{
    current_background_red = red;
    current_background_green = green;
    current_background_blue = blue;
    current_background_alpha = alpha;
    return;
}

void CDisplayList::SetForegroundColor (int red, int green, int blue, int alpha)
{
    current_foreground_red = red;
    current_foreground_green = green;
    current_foreground_blue = blue;
    current_foreground_blue = alpha;
    return;
}

void CDisplayList::SetColor (int red, int green, int blue, int alpha)
{
    if (alpha < 0  ||  alpha > 255) {
        alpha = 255;
    }
    SetLineColor (red, green, blue, alpha);
    SetFillColor (red, green, blue, alpha);
    SetSymbolColor (red, green, blue, alpha);
    SetTextColor (red, green, blue, alpha);
    SetTextFillColor (red, green, blue, alpha);
    SetFillPatternColor (red, green, blue, alpha);
    SetFillBorderColor (red, green, blue, alpha);

    return;
}

void CDisplayList::SetLineColor (int red, int green, int blue, int alpha)
{
    if (alpha < 0  ||  alpha > 255) {
        alpha = 255;
    }
    current_line_red = red;
    current_line_green = green;
    current_line_blue = blue;
    current_line_alpha = alpha;
    return;
}

void CDisplayList::SetFillColor (int red, int green, int blue, int alpha)
{
    if (alpha < 0  ||  alpha > 255) {
        alpha = 255;
    }
    current_fill_red = red;
    current_fill_green = green;
    current_fill_blue = blue;
    current_fill_alpha = alpha;
    return;
}

void CDisplayList::SetSymbolColor (int red, int green, int blue, int alpha)
{
    if (alpha < 0  ||  alpha > 255) {
        alpha = 255;
    }
    current_symbol_red = red;
    current_symbol_green = green;
    current_symbol_blue = blue;
    current_symbol_alpha = alpha;
    return;
}

void CDisplayList::SetTextColor (int red, int green, int blue, int alpha)
{
    if (alpha < 0  ||  alpha > 255) {
        alpha = 255;
    }
    current_text_red = red;
    current_text_green = green;
    current_text_blue = blue;
    current_text_alpha = alpha;
    return;
}

void CDisplayList::SetTextFillColor (int red, int green, int blue, int alpha)
{
    if (alpha < 0  ||  alpha > 255) {
        alpha = 255;
    }
    current_text_fill_red = red;
    current_text_fill_green = green;
    current_text_fill_blue = blue;
    current_text_fill_alpha = alpha;
    return;
}

void CDisplayList::SetFillPatternColor (int red, int green, int blue, int alpha)
{
    if (alpha < 0  ||  alpha > 255) {
        alpha = 255;
    }
    current_fill_pattern_red = red;
    current_fill_pattern_green = green;
    current_fill_pattern_blue = blue;
    current_fill_pattern_alpha = alpha;
    return;
}

void CDisplayList::SetFillBorderColor (int red, int green, int blue, int alpha)
{
    if (alpha < 0  ||  alpha > 255) {
        alpha = 255;
    }
    current_fill_border_red = red;
    current_fill_border_green = green;
    current_fill_border_blue = blue;
    current_fill_border_alpha = alpha;

    return;
}


void CDisplayList::SetLineSmoothFlag (int ival)
{
    if (ival > 100) ival = 100;
    if (ival < 0) ival = 0;

    current_line_smooth_flag = ival;
    if (spline_used_flag == 0) {
        if (ival > 0) {
            spline_used_flag = 1;
        }
    }
}

void CDisplayList::SetEditableFlag (int ival)
{
    current_editable_flag = ival;
}

void CDisplayList::SetSelectableFlag (int ival)
{
    current_selectable_flag = ival;
}




void CDisplayList::free_axes (void)
{
    int           i;
    AXisPrim      *prim;

    int          ap_size = (int)axis_prim_list.size();
    AXisPrim     *ap_data = axis_prim_list.data();

    if (ap_data == NULL  ||  ap_size < 1) {
        return;
    }

    for (i=0; i<ap_size; i++) {
        prim = ap_data + i;
        csw_Free (prim->ap);
    }

    axis_prim_list.clear();

    return;
}

void CDisplayList::free_lines (void)
{
    int           i;
    LInePrim      *prim;

    int  lp_size = (int)line_prim_list.size();
    if (lp_size > 0) {
        LInePrim  *lp_data = line_prim_list.data();
        if (lp_data != NULL) {
            for (i=0; i<lp_size; i++) {
                prim = lp_data + i;
                csw_Free (prim->xypts);
                prim->xypts = NULL;
            }
        }
    }
    line_prim_list.clear();

    int  cp_size = (int)contour_line_prim_list.size();
    if (cp_size > 0) {
        LInePrim  *cp_data = contour_line_prim_list.data();
        if (cp_data != NULL) {
            for (i=0; i<cp_size; i++) {
                prim = cp_data + i;
                csw_Free (prim->xypts);
                prim->xypts = NULL;
            }
        }
    }
    contour_line_prim_list.clear();

    return;
}

void CDisplayList::free_fills (void)
{
    int           i;
    FIllPrim      *prim;

    int        fp_size = (int)fill_prim_list.size();
    FIllPrim   *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    for (i=0; i<fp_size; i++) {
        prim = fp_data + i;
        csw_Free (prim->xypts);
        csw_Free (prim->x_orig);
        csw_Free (prim->npts_orig);
    }

    fill_prim_list.clear();

    return;
}


void CDisplayList::free_texts (void)
{
    int           i;
    TExtPrim      *prim;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }

    for (i=0; i<tp_size; i++) {
        prim = tp_data + i;
        csw_Free (prim->chardata);
    }

    text_prim_list.clear();

    return;
}


void CDisplayList::free_symbs (void)
{

    symb_prim_list.clear();

    return;
}


void CDisplayList::free_shapes (void)
{
    shape_prim_list.clear();
    return;
}

void CDisplayList::free_images (void)
{
    int           i;
    IMagePrim     *prim;

    int            ip_size = (int)image_prim_list.size();
    IMagePrim      *ip_data = image_prim_list.data();

    if (ip_data == NULL  ||  ip_size < 1) {
        return;
    }

    for (i=0; i<ip_size; i++) {
        prim = ip_data + i;
        csw_Free (prim->red_data);
        csw_Free (prim->green_data);
        csw_Free (prim->blue_data);
        csw_Free (prim->transparency_data);
    }

    image_prim_list.clear();

    return;
}


/*
  Add an image where the color values are already known.  The
  specified red, green, blue and trans values are copied by
  this method.  The calling function still owns the arrays
  passed in and the calling function needs to free the memory
  if appropriate.
*/
int CDisplayList::AddColorImage (unsigned char *red,
                                 unsigned char *green,
                                 unsigned char *blue,
                                 unsigned char *trans,
                                 int ncol,
                                 int nrow,
                                 double xmin,
                                 double ymin,
                                 double xmax,
                                 double ymax)
{
    IMagePrim      *imptr = NULL;
    int            i, j, k, k2, offset, ntot;
    unsigned char  *red_ptr = NULL, *green_ptr = NULL,
                   *blue_ptr = NULL, *trans_ptr = NULL;
    int            next_image;

    bool           bsuccess = false;

    auto fscope = [&]()
    {
      if (!bsuccess) {
        csw_Free (red_ptr);
        csw_Free (green_ptr);
        csw_Free (blue_ptr);
        csw_Free (trans_ptr);
      }
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (xmax <= xmin || ymax <= ymin ||
        red == NULL   ||  green == NULL  ||  blue == NULL  ||  trans == NULL  ||
        ncol < 2  ||  nrow < 2) {
        return -1;
    }

    next_image = get_available_image ();
    if (next_image < 0) {
        try {
            SNF;
            IMagePrim  ipr;
            ZeroInit (&ipr, sizeof(ipr));
            image_prim_list.push_back (ipr);
            next_image = (int)image_prim_list.size() - 1;
        }
        catch (...) {
            printf ("Exception in image_prim_list pushback\n");
            return -1;
        }
    }

/*
 * Allocate space for color image data.
 */
    ntot = ncol * nrow;
    red_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    green_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    blue_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    trans_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    if (red_ptr == NULL  ||  green_ptr == NULL  ||  blue_ptr == NULL  ||
        trans_ptr == NULL) {
        return -1;
    }

/*
 * Copy the red, green blue and alpha to arrays owned by the display list.
 */
    k2 = 0;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            red_ptr[k2] = red[k];
            green_ptr[k2] = green[k];
            blue_ptr[k2] = blue[k];
            trans_ptr[k2] = trans[k];
            k2++;
        }
    }

/*
 * Populate the image prim structure.
 */
    IMagePrim   *ip_data = image_prim_list.data();

    imptr = ip_data + next_image;

    strncpy (imptr->name, current_image_name, 99);
    imptr->name[99] = '\0';
    imptr->image_id = current_image_id;

    imptr->red_data = red_ptr;
    imptr->green_data = green_ptr;
    imptr->blue_data = blue_ptr;
    imptr->transparency_data = trans_ptr;

    imptr->ncol = ncol;
    imptr->nrow = nrow;

    imptr->xmin = xmin;
    imptr->ymin = ymin;
    imptr->xmax = xmax;
    imptr->ymax = ymax;

    imptr->grid_num = current_grid_num;
    imptr->frame_num = current_frame_num;
    imptr->selectable_object_num = current_selectable_object_num;
    imptr->graph_num = current_graph_num;
    imptr->xaxis_num = current_xaxis_num;
    imptr->yaxis_num = current_yaxis_num;

    imptr->layer_num = current_layer_num;
    imptr->item_num = current_item_num;

    imptr->editable_flag = (char)current_editable_flag;
    imptr->selectable_flag = (char)current_selectable_flag;

    imptr->visible_flag = 1;
    imptr->draw_flag = 1;
    imptr->plot_flag = 1;
    imptr->selected_flag = 0;
    imptr->deleted_flag = 0;

    imptr->prim_num = next_image;
    imptr->scaleable = 0;

    bsuccess = true;

    return 1;

}  /*  end of function AddColorImage  */





void CDisplayList::delete_frame_lines (int fnum)
{
    int            i;
    LInePrim       *lptr;

    int            lp_size = (int)line_prim_list.size();
    LInePrim       *lp_data = line_prim_list.data();

    if (lp_size < 1  ||  lp_data == NULL) {
        return;
    }

    for (i=0; i<lp_size; i++) {

        lptr = lp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }

        if (lptr->frame_num != fnum) {
            continue;
        }

        if (lptr->grid_num >= 0) {
           continue;
        }

    }

    return;

}  /*  end of function delete_frame_lines */






void CDisplayList::delete_surf_contour_lines (int surf_num)
{
    int            i;
    LInePrim       *lptr;

    int            cp_size = (int)contour_line_prim_list.size();
    LInePrim       *cp_data = contour_line_prim_list.data();

    if (cp_data == NULL  ||  cp_size < 1) {
        return;
    }

    for (i=0; i<cp_size; i++) {

        lptr = cp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }

        if (lptr->grid_num < 0  ||  lptr->grid_num >= 10000) {
            continue;
        }

        if (lptr->grid_num != surf_num) {
            continue;
        }

        csw_Free (lptr->xypts);
        lptr->xypts = NULL;
        lptr->npts = 0;
        lptr->maxpts = 0;
        lptr->deleted_flag = 1;
        add_available_contour_line (i);

    }

    return;

}  /*  end of function delete_surf_contour_lines */





void CDisplayList::delete_frame_cell_edges (int fnum)
{
    int            i;
    LInePrim       *lptr;

    int            lp_size = (int)line_prim_list.size();
    LInePrim       *lp_data = line_prim_list.data();

    if (lp_size < 1  ||  lp_data == NULL) {
        return;
    }

    for (i=0; i<lp_size; i++) {

        lptr = lp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }

        if (lptr->frame_num != fnum) {
            continue;
        }

        if (lptr->grid_num < 10000) {
           continue;
        }

    }

    return;

}  /*  end of function delete_frame_cell_edges */





void CDisplayList::delete_frame_fills (int fnum)
{
    int            i;
    FIllPrim       *fptr;

    int        fp_size = (int)fill_prim_list.size();
    FIllPrim   *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    for (i=0; i<fp_size; i++) {

        fptr = fp_data + i;

        if (fptr->deleted_flag == 1) {
            continue;
        }

        if (fptr->frame_num != fnum) {
            continue;
        }

    }

    return;

}  /*  end of function delete_frame_fills */



void CDisplayList::delete_frame_texts (int fnum)
{
    int            i;
    TExtPrim       *tptr;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }

    for (i=0; i<tp_size; i++) {

        tptr = tp_data + i;

        if (tptr->deleted_flag == 1) {
            continue;
        }

        if (tptr->frame_num != fnum) {
            continue;
        }

    }

    return;

}  /*  end of function delete_frame_texts */



void CDisplayList::delete_frame_symbs (int fnum)
{
    int            i;
    SYmbPrim       *sptr;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return;
    }

    for (i=0; i<sp_size; i++) {

        sptr = sp_data + i;

        if (sptr->deleted_flag == 1) {
            continue;
        }

        if (sptr->frame_num != fnum) {
            continue;
        }

    }

    return;

}  /*  end of function delete_frame_symbs */



void CDisplayList::delete_frame_shapes (int fnum)
{
    int            i;
    SHapePrim      *shptr;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return;
    }

    for (i=0; i<hp_size; i++) {

        shptr = hp_data + i;

        if (shptr->deleted_flag == 1) {
            continue;
        }

        if (shptr->frame_num != fnum) {
            continue;
        }

    }

    return;

}  /*  end of function delete_frame_shapes */



void CDisplayList::delete_frame_images (int fnum)
{
    int            i;
    IMagePrim      *imptr;

    int            ip_size = (int)image_prim_list.size();
    IMagePrim      *ip_data = image_prim_list.data();

    if (ip_data == NULL  ||  ip_size < 1) {
        return;
    }

    for (i=0; i<ip_size; i++) {

        imptr = ip_data + i;

        if (imptr->deleted_flag == 1) {
            continue;
        }

        if (imptr->frame_num != fnum) {
            continue;
        }

    }

    return;

}  /*  end of function delete_frame_images */




void CDisplayList::delete_frame_grid_images (int fnum)
{
    int            i;
    IMagePrim      *imptr, *imp2;

    int            ip_size = (int)image_prim_list.size();
    IMagePrim      *ip_data = image_prim_list.data();

    if (ip_data == NULL  ||  ip_size < 1) {
        return;
    }

    int        sf_size = surf_list.size();
    DLSurf     **sf_data = surf_list.data();

    if (sf_data == NULL  ||  sf_size < 1) {
        return;
    }

    for (i=0; i<ip_size; i++) {

        imptr = ip_data + i;

        if (imptr->deleted_flag == 1) {
            continue;
        }

        if (imptr->frame_num != fnum) {
            continue;
        }

        if (imptr->grid_num < 0  ||  imptr->grid_num >= 10000) {
            continue;
        }

        if (imptr->grid_num >= 0) {
            if (sf_data[imptr->grid_num]->needs_reclip == 0) {
                continue;
            }
        }

        imp2 = imptr;
        imp2->deleted_flag = 1;
        csw_Free (imp2->red_data);
        csw_Free (imp2->green_data);
        csw_Free (imp2->blue_data);
        csw_Free (imp2->transparency_data);
        imp2->red_data = NULL;
        imp2->green_data = NULL;
        imp2->blue_data = NULL;
        imp2->transparency_data = NULL;
        add_available_image (i);

    }

    return;

}  /*  end of function delete_frame_grid_images */


void CDisplayList::rescale_frame (int fnum)
{
    FRameStruct     *frptr;

    if (fnum < 0  ||  fnum >= num_frame_list  ||
        frame_list == NULL) {
        return;
    }

    frptr = frame_list + fnum;

    rescale_frame (frptr);

    return;
}


/*
 * The new fx1, fy1 etc. must have been set in this frame struct
 * prior to calling this function.
 */
void CDisplayList::rescale_frame (FRameStruct *frptr)
{
    int           fnum, save_fnum;
    double        area1, area2;

/*
 * Make sure the frame is valid and rescaleable.
 */
    if (frptr == NULL) {
        return;
    }
    if (frame_list == NULL) {
        return;
    }

    fnum = frptr - frame_list;
    if (fnum < 0  ||  fnum >= num_frame_list) {
        return;
    }

    if (frptr->rescaleable == 0) {
        return;
    }

    SetupSpatialIndexForFrame (fnum);

    delete_frame_images (fnum);
    delete_frame_grid_images (fnum);
    delete_frame_cell_edges (fnum);

/*
 * temporarily switch the current frame num and
 * update the frame clipping limits.
 */
    save_fnum = current_frame_num;
    current_frame_num = fnum;
    update_frame_limits ();

    patch_draw_flag = 0;
    if (frptr->xmax > frptr->xmin  &&  frptr->ymax > frptr->ymin) {
        area1 = (frptr->xmax - frptr->xmin) * (frptr->ymax - frptr->ymin);
        area2 = (Fx2 - Fx1) * (Fy2 - Fy1);
        if (area2 < 0.0) area2 = - area2;
        if (area2 < 0.25 * area1) {
            patch_draw_flag = 1;
        }
    }

    frptr->patch_draw_flag = patch_draw_flag;

/*
 *  Create primitive patch lists for the frame and any other
 *  frame synced to it.  All the primitives of a type (e.g. all lines) 
 *  are put into the same list.  The DrawAll methods will use the
 *  frame number member of the primitive structure to draw the
 *  primitive in the correct frame.
 */
    contour_line_patch_list.clear();
    line_patch_list.clear();
    fill_patch_list.clear();
    symb_patch_list.clear();
    text_patch_list.clear();
    shape_patch_list.clear();

    if (patch_draw_flag == 1) {
        for (int i=0; i<num_frame_list; i++) {
            if (!IsFrameSynced (fnum, i)) {
                continue;
            }
            frame_list[i].patch_draw_flag = 1;
            frame_list[i].aspect_flag = 0;
            SetupSpatialIndexForFrame (i);
            reclip_frame_lines (i);
            reclip_frame_fills (i);
            reclip_frame_texts (i);
            reclip_frame_symbs (i);
            reclip_frame_shapes (i);
            reclip_frame_contours (i);
        }
    }

    reclip_frame_grid_images (fnum);

    current_frame_num = save_fnum;
    update_frame_limits ();

    return;
}



/*
 * If either frame is attached to the other, and if either
 * has a true scale to attach frame flag, then I consider the
 * two frames synced.
 */
bool CDisplayList::IsFrameSynced (int pick_fnum, int fnum)
{
    if (pick_fnum == fnum) {
        return true;
    }

    FRameStruct  *fr1 = frame_list + pick_fnum;
    FRameStruct  *fr2 = frame_list + fnum;

    if (!(fr1->attach_frame == fnum || fr2->attach_frame == pick_fnum)) {
        return false;
    }

    if (fr1->scale_width_to_attach_frame  ||
        fr1->scale_height_to_attach_frame  ||
        fr2->scale_width_to_attach_frame  ||
        fr2->scale_height_to_attach_frame) {
        return true;
    }

    return false;

}




/*
 *******************************************************************************
 *
 *  The following group of functions are used to rescale and reclip frame
 *  primitives after the frame holding them has its geometry changed.
 *
 *******************************************************************************
*/
void CDisplayList::reclip_frame_lines (int fnum)
{

    if ((int)line_prim_list.size() < 1  ||  line_prim_list.data() == NULL) {
        return;
    }

    if (xywork == NULL  ||
        xywork2 == NULL  ||
        iwork == NULL) {
        return;
    }

    PopulateLinePatches (Fx1, Fy1, Fx2, Fy2);

    return;

}  /*  end of function reclip_frame_lines */



void CDisplayList::reclip_frame_fills (int fnum)
{

    int        fp_size = (int)fill_prim_list.size();
    FIllPrim   *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    PopulateFillPatches (Fx1, Fy1, Fx2, Fy2);

    return;

}  /*  end of function reclip_frame_fills */



void CDisplayList::reclip_frame_texts (int fnum)
{

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }

    PopulateTextPatches (Fx1, Fy1, Fx2, Fy2);

    return;

}  /*  end of function reclip_frame_texts */



void CDisplayList::reclip_frame_symbs (int fnum)
{

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return;
    }

    PopulateSymbPatches (Fx1, Fy1, Fx2, Fy2);

    return;

}  /*  end of function reclip_frame_symbs */



void CDisplayList::reclip_frame_shapes (int fnum)
{

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return;
    }

    PopulateShapePatches (Fx1, Fy1, Fx2, Fy2);

    return;

}  /*  end of function reclip_frame_shapes */




/*
 *****************************************************************

                 D r a w F r a m e B o r d e r s

 *****************************************************************

  Draw lines for drawable frame borders and also set java side
  clip windows for clippable frame borders.  All frames, whether
  clippable or not, will have a java side entry so the frame
  numbers match up.

*/

int CDisplayList::DrawFrameBorders (void)
{
    int            i;
    FRameStruct    *frptr;

    if (frame_list == NULL) {
        return 0;
    }

    for (i=0; i<num_frame_list; i++) {

        frptr = frame_list + i;

        draw_frame_border_prims (i);

        gtx_drawprim_obj.gtx_clip_frame_border (
            i,
            frptr
        );

    }

    return 0;

}  /*  end of function DrawFrameBorders */



/*
 * Calculate the margins needed for labels around a frame at the
 * given device size and scaling.  This is private and only called
 * from the Draw method.
 */
void CDisplayList::calc_frame_margins (FRameStruct *frptr)
{
    CSW_F          page_gap10;

    page_gap10 = minimum_frame_separation * page_units_per_inch;

/*
 * If border is not labelled, use a .1 inch gap.
 */
    if (frptr->borderflag == GTX_FRAME_NO_BORDER  ||
        frptr->borderflag == GTX_FRAME_PLAIN_BORDER) {
        frptr->left_margin = page_gap10;
        frptr->right_margin = page_gap10;
        frptr->top_margin = page_gap10;
        frptr->bottom_margin = page_gap10;
        return;
    }

/*
 * For a labeled border, use the max size of labels to calculate
 * the margins.
 */
    frptr->left_margin = frptr->left_axis.page_margin;

    frptr->right_margin = frptr->right_axis.page_margin;

    frptr->top_margin = frptr->top_axis.page_margin;

    frptr->bottom_margin = frptr->bottom_axis.page_margin;

    return;

}


void CDisplayList::format_border_label (char *label,
                                        double value,
                                        double range,
                                        int maxlen)
{
    char         fmt[100];
    double aval;

    aval = value;
    if (value < 0.0) aval = -aval;

    strcpy (fmt, "%.4g");
    if (range < 2.e9  &&  aval < 2.e9) {
        strcpy (fmt, "%d");
        if (range < 500.0) {
            strcpy (fmt, "%.1f");
        }
        if (range < 50.0) {
            strcpy (fmt, "%.2f");
        }
        if (range < 5.0) {
            strcpy (fmt, "%.3f");
        }
        if (range < .5) {
            strcpy (fmt, "%.4f");
        }
        if (range < 0.05) {
            strcpy (fmt, "%.4g");
        }
    }

    sprintf (label, fmt, value);
    csw_StrInsertCommas (label, maxlen);

    return;
}


/*
 *  Calculate the frame sizes and attachments based on the
 *  current window scaling.  The scaling is then updated for
 *  the layout, and the layout is recalculated.  The method
 *  either stops when very little change in the page clip limits
 *  is done or in 10 iterations, whichever comes first.
 *
 *  At the end of this, the scaling factors and clip limits to go
 *  from page to screen are set and ready for drawing.  Also, the
 *  page corners of each frame are properly located.
 *
 *  This is only called from the Draw method.  It is private and
 *  needs to stay that way.
 */
void CDisplayList::calc_frame_layout (void)
{
    int           ido, jdo, i, ndone, apos, fnum;
    CSW_F         pxmin, pymin, pxmax, pymax,
                  height, width, gap, pmove, mid,
                  xmin, ymin, xmax, ymax, tiny, t1, t2, t3, t4;
    CSW_F         dx, dy, adx, ady;
    CSW_F         px1, py1, px2, py2, aspect, pmult, porig;
    FRameStruct   *fp, *fp2, *fpbase;

    int           a1 = GTX_FRAME_ATTACH_LEFT_MIN;
    int           a2 = GTX_FRAME_ATTACH_LEFT_MIDDLE;
    int           a3 = GTX_FRAME_ATTACH_LEFT_MAX;
    int           a4 = GTX_FRAME_ATTACH_TOP_MIN;
    int           a5 = GTX_FRAME_ATTACH_TOP_MIDDLE;
    int           a6 = GTX_FRAME_ATTACH_TOP_MAX;
    int           a7 = GTX_FRAME_ATTACH_RIGHT_MIN;
    int           a8 = GTX_FRAME_ATTACH_RIGHT_MIDDLE;
    int           a9 = GTX_FRAME_ATTACH_RIGHT_MAX;
    int           a10 = GTX_FRAME_ATTACH_BOTTOM_MIN;
    int           a11 = GTX_FRAME_ATTACH_BOTTOM_MIDDLE;
    int           a12 = GTX_FRAME_ATTACH_BOTTOM_MAX;

/*
 * Do nothing if there are no frames.
 */
    if (frame_list == NULL  ||
        num_frame_list < 1) {
        return;
    }

/*
 * Shift the prim coordinates for any attachable frame prims back
 * to their original page positions.
 */
    if (page_index_xmax > page_index_xmin  &&  page_index_ymax > page_index_ymin) {
        tiny = (CSW_F)((page_index_xmax - page_index_xmin +
                        page_index_ymax - page_index_ymin) / 20000.0);
    }
    else {
        tiny = 0.0f;
    }

    for (i=0; i<num_frame_list; i++) {

        fp = frame_list + i;
        if (fp->attach_frame < 0  ||  fp->attach_frame >= num_frame_list) {
            continue;
        }

        dx = fp->px1orig - fp->px1;
        dy = fp->py1orig - fp->py1;
        adx = dx;
        if (adx < 0.0) adx = -adx;
        ady = dy;
        if (ady < 0.0) ady = -ady;

        if (adx <= tiny  &&  ady <= tiny) {
            continue;
        }

        //shift_frame_prims (i, dx, dy);
    }

/*
 * Any frame with non unit aspect ratio can be reshaped, in page coordinates,
 * to reflect the new window's aspect ratio.
 */
    if (screen_was_resized  &&  page_units_type == 1) {
        dx = (CSW_F) (screen_xmax - screen_xmin);
        dy = (CSW_F) (screen_ymax - screen_ymin);
        if (dx < 2) dx = 2;
        if (dy < 2) dy = 2;
        aspect = dy / dx;

    /*
     * First, resize the "base" frames.
     */
        for (i=0; i<num_frame_list; i++) {

            fp = frame_list + i;
            if (fp->aspect_flag != GTX_ALLOW_ANY_FRAME_ASPECT_RATIO) {
                continue;
            }
            if (fp->attach_frame >= 0) {
                continue;
            }

            porig = fp->px2orig - fp->px1orig + fp->py2orig + fp->py1orig;

            px1 = fp->px1orig;
            py1 = fp->py1orig;
            if (aspect > 1.0f) {
                py2 = fp->py2orig;
                px2 = px1 + (py2 - py1) / aspect;
            }
            else {
                px2 = fp->px2orig;
                py2 = py1 + (px2 - px1) * aspect;
            }

            pmult = porig / (px2 - px1 + py2 - py1);
            px2 = px1 + (px2 - px1) * pmult;
            py2 = py1 + (py2 - py1) * pmult;

            fp->px2orig = px2;
            fp->py2orig = py2;
            fp->px1 = px1;
            fp->py1 = py1;
            fp->px2 = px2;
            fp->py2 = py2;

            RescaleFrame (fp->name, 0,
                          fp->x1, fp->y1, fp->x2, fp->y2);
        }

    /*
     * Resize the attached frames to agree with the base frames.
     */
        for (i=0; i<num_frame_list; i++) {

            fp = frame_list + i;
            if (fp->aspect_flag != GTX_ALLOW_ANY_FRAME_ASPECT_RATIO) {
                continue;
            }
            if (fp->attach_frame < 0) {
                continue;
            }

            fpbase = frame_list + fp->attach_frame;
            if (fp->scale_width_to_attach_frame == 1) {
                fp->px1 = fpbase->px1;
                fp->px2 = fpbase->px2;
            }
            if (fp->scale_height_to_attach_frame == 1) {
                fp->py1 = fpbase->py1;
                fp->py2 = fpbase->py2;
            }

            RescaleFrame (fp->name, 0,
                          fp->x1, fp->y1, fp->x2, fp->y2);
        }
    }

    screen_was_resized = 0;

/*
 * Start with the current page limits and scales.  Calculate
 * frame sizes and attachments.  Then, get the page limits for
 * that candidate layout.  Update scales to reflect the
 * candidate layout and iterate.  If the page limits change
 * very little between iterations, break out of the loop.
 * Always break out after 10 tries regardless.
 */
    for (ido=0; ido<10; ido++) {

    /*
     * Calculate the margins for each frame based on the frame
     * border types and the current page units per inch.  Also
     * set each frames limits to defaults in case there are
     * problems attaching frames.
     */
        for (i=0; i<num_frame_list; i++) {

            fp = frame_list + i;

            fp->reborder_needed = 1;
            border_margin_only_flag = 1;
            reborder_frame (fp);
            border_margin_only_flag = 0;

            calc_frame_margins (fp);

            fp->is_positioned = 0;
            fp->px1 = fp->px1orig;
            fp->py1 = fp->py1orig;
            fp->px2 = fp->px2orig;
            fp->py2 = fp->py2orig;
            fp->page_left = fp->px1;
            fp->page_bottom = fp->py1;
            fp->page_right = fp->px2;
            fp->page_top = fp->py2;
        }

    /*
     * Calculate the attachment points for all frames that do
     * not attach to any other frame.  These form the "base"
     * frames for other attachments.
     */
        for (i=0; i<num_frame_list; i++) {
            fp = frame_list + i;
            fnum = fp->attach_frame;
            if (fnum >= 0  &&  fnum < num_frame_list) {
                fp2 = frame_list + fnum;
                if (fp2->deleted == 0) {
                    continue;
                }
            }
            fp->page_left = fp->px1 - fp->left_margin;
            fp->page_right = fp->px2 + fp->right_margin;
            fp->page_bottom = fp->py1 - fp->bottom_margin;
            fp->page_top = fp->py2 + fp->top_margin;
            fp->is_positioned = 1;
        }

    /*
     * Attach frames to the base frames.  Once a frame is attached it
     * can be used as a base for another attachment.  Loop through
     * the attachments until none are attached in an iteration or
     * until num_frame_list iterations have been done.
     */
        for (jdo = 0; jdo < num_frame_list; jdo++) {

            ndone = 0;
            for (i=0; i<num_frame_list; i++) {

                fp = frame_list + i;
                if (fp->is_positioned == 1) {
                    continue;
                }

                fnum = fp->attach_frame;
                if (fnum < 0  ||  fnum >= num_frame_list) {
                    continue;
                }

            /*
             * If the frame to attach to has not been positioned or if
             * it has been deleted, there is nothing to attach to so
             * skip this frame.
             */
                fp2 = frame_list + fnum;
                if (fp2->is_positioned == 0  ||
                    fp2->deleted == 1) {
                    continue;
                }

                apos = fp->attach_position;
                height = fp->py2 - fp->py1;
                width = fp->px2 - fp->px1;
                pmove = fp->perpendicular_move * page_units_per_inch;
                gap = fp->extra_gap * page_units_per_inch;

            /*
             * Attach to the left side of the "base" frame.
             */
                if (apos >= a1  &&  apos <= a3) {
                    fp->page_right = fp2->page_left - gap;
                    fp->px2 = fp->page_right - fp->right_margin;
                    fp->px1 = fp->px2 - width;
                    fp->page_left = fp->px1 - fp->left_margin;

                /*
                 * left min.
                 */
                    if (apos == a1) {
                        fp->py1 = fp2->py1 + pmove;
                        fp->page_bottom = fp->py1 - fp->bottom_margin;
                        fp->py2 = fp->py1 + height;
                        fp->page_top = fp->py2 + fp->top_margin;
                    }

                /*
                 * left mid
                 */
                    else if (apos == a2) {
                        mid = (fp2->py1 + fp2->py2) / 2.0f + pmove;
                        fp->py1 = mid - height / 2.0f;
                        fp->page_bottom = fp->py1 - fp->bottom_margin;
                        fp->py2 = fp->py1 + height;
                        fp->page_top = fp->py2 + fp->top_margin;
                    }

                /*
                 * left max
                 */
                    else {
                        fp->py2 = fp2->py2 + pmove;
                        fp->py1 = fp->py2 - height;
                        fp->page_bottom = fp->py1 - fp->bottom_margin;
                        fp->page_top = fp->py2 + fp->top_margin;
                    }
                }

            /*
             * Attach to the right side of the "base" frame.
             */
                if (apos >= a7  &&  apos <= a9) {
                    fp->page_left = fp2->page_right + gap;
                    fp->px1 = fp->page_left + fp->left_margin;
                    fp->px2 = fp->px1 + width;
                    fp->page_right = fp->px2 + fp->right_margin;

                /*
                 * right min
                 */
                    if (apos == a7) {
                        fp->py1 = fp2->py1 + pmove;
                        fp->page_bottom = fp->py1 - fp->bottom_margin;
                        fp->py2 = fp->py1 + height;
                        fp->page_top = fp->py2 + fp->top_margin;
                    }

                /*
                 * right mid
                 */
                    else if (apos == a8) {
                        mid = (fp2->py1 + fp2->py2) / 2.0f + pmove;
                        fp->py1 = mid - height / 2.0f;
                        fp->page_bottom = fp->py1 - fp->bottom_margin;
                        fp->py2 = fp->py1 + height;
                        fp->page_top = fp->py2 + fp->top_margin;
                    }

                /*
                 * right max
                 */
                    else {
                        fp->py2 = fp2->py2 + pmove;
                        fp->py1 = fp->py2 - height;
                        fp->page_bottom = fp->py1 - fp->bottom_margin;
                        fp->page_top = fp->py2 + fp->top_margin;
                    }
                }

            /*
             * Attach to the top side of the "base" frame.
             */
                if (apos >= a4  &&  apos <= a6) {
                    fp->page_bottom = fp2->page_top + gap;
                    fp->py1 = fp->page_bottom + fp->bottom_margin;
                    fp->py2 = fp->py1 + height;
                    fp->page_top = fp->py2 + fp->top_margin;

                /*
                 * top min
                 */
                    if (apos == a4) {
                        fp->px1 = fp2->px1 + pmove;
                        fp->page_left = fp->px1 - fp->left_margin;
                        fp->px2 = fp->px1 + width;
                        fp->page_right = fp->px2 + fp->right_margin;
                    }

                /*
                 * top mid
                 */
                    else if (apos == a5) {
                        mid = (fp2->px1 + fp2->px2) / 2.0f + pmove;
                        fp->px1 = mid - width / 2.0f;
                        fp->page_left = fp->px1 - fp->left_margin;
                        fp->px2 = fp->px1 + width;
                        fp->page_right = fp->px2 + fp->right_margin;
                    }

                /*
                 * top max
                 */
                    else {
                        fp->px2 = fp2->px2 + pmove;
                        fp->px1 = fp->px2 - width;
                        fp->page_left = fp->px1 - fp->left_margin;
                        fp->page_right = fp->px2 + fp->right_margin;
                    }
                }

            /*
             * Attach to the bottom side of the "base" frame.
             */
                if (apos >= a10  &&  apos <= a12) {
                    fp->page_top = fp2->page_bottom - gap;
                    fp->py2 = fp->page_top - fp->top_margin;
                    fp->py1 = fp->py2 - height;
                    fp->page_bottom = fp->py1 - fp->bottom_margin;

                /*
                 * top min
                 */
                    if (apos == a10) {
                        fp->px1 = fp2->px1 + pmove;
                        fp->page_left = fp->px1 - fp->left_margin;
                        fp->px2 = fp->px1 + width;
                        fp->page_right = fp->px2 + fp->right_margin;
                    }

                /*
                 * top mid
                 */
                    else if (apos == a11) {
                        mid = (fp2->px1 + fp2->px2) / 2.0f + pmove;
                        fp->px1 = mid - width / 2.0f;
                        fp->page_left = fp->px1 - fp->left_margin;
                        fp->px2 = fp->px1 + width;
                        fp->page_right = fp->px2 + fp->right_margin;
                    }

                /*
                 * top max
                 */
                    else {
                        fp->px2 = fp2->px2 + pmove;
                        fp->px1 = fp->px2 - width;
                        fp->page_left = fp->px1 - fp->left_margin;
                        fp->page_right = fp->px2 + fp->right_margin;
                    }
                }

                ndone++;
                fp->is_positioned = 1;

            }

        /*
         * If no frames were positioned in this pass,
         * break from the attached frame positioning loop.
         */
            if (ndone == 0) {
                break;
            }

        }  /* end of attached frame positioning loop */

    /*
     * If the page units are actually screen units (page_units_type is 2)
     * then no resizing of page units is done.
     */
        if (page_units_type == 2) {
            break;
        }

    /*
     * Find the new page limits and compare to the old.
     * If they are very close, exit the itertion loop.
     * Do not exit the first time through the loop.
     */
        pxmin = 1.e30f;
        pymin = 1.e30f;
        pxmax = -1.e30f;
        pymax = -1.e30f;
        for (i=0; i<num_frame_list; i++) {
            fp = frame_list + i;
            if (fp->page_left < pxmin) pxmin = fp->page_left;
            if (fp->page_bottom < pymin) pymin = fp->page_bottom;
            if (fp->page_right > pxmax) pxmax = fp->page_right;
            if (fp->page_top > pymax) pymax = fp->page_top;
        }

        if (ido != 0) {
            tiny = (CSW_F)((page_xmax - page_xmin + page_ymax - page_ymin) / 2000.0);
            t1 = (CSW_F)(page_xmin - pxmin);
            if (t1 < 0.0) t1 = -t1;
            t2 = (CSW_F)(page_ymin - pymin);
            if (t2 < 0.0) t2 = -t2;
            t3 = (CSW_F)(page_xmax - pxmax);
            if (t3 < 0.0) t3 = -t3;
            t4 = (CSW_F)(page_ymax - pymax);
            if (t4 < 0.0) t4 = -t4;
            if (t1 <= tiny  &&
                t2 <= tiny  &&
                t3 <= tiny  &&
                t4 <= tiny) {
                break;
            }
        }

    /*
     * Set the scaling and clip factors and do another iteration
     */
        page_xmin = pxmin;
        page_ymin = pymin;
        page_xmax = pxmax;
        page_ymax = pymax;
        clip_xmin = pxmin;
        clip_ymin = pymin;
        clip_xmax = pxmax;
        clip_ymax = pymax;

        gtx_drawprim_obj.gtx_init_drawing
                         ((CSW_F)clip_xmin, (CSW_F)clip_ymin,
                          (CSW_F)clip_xmax, (CSW_F)clip_ymax,
                          (CSW_F)clip_xmin, (CSW_F)clip_ymin,
                          (CSW_F)clip_xmax, (CSW_F)clip_ymax,
                          (CSW_F)screen_xmin, (CSW_F)screen_ymin,
                          (CSW_F)screen_xmax, (CSW_F)screen_ymax,
                          screen_dpi, page_units_type, dfile);
        gtx_drawprim_obj.gtx_get_drawing_clip_limits (&xmin, &ymin, &xmax, &ymax);
        gtx_drawprim_obj.gtx_get_page_units_per_inch (&page_units_per_inch);

        clip_xmin = (double)xmin;
        clip_xmax = (double)xmax;
        clip_ymin = (double)ymin;
        clip_ymax = (double)ymax;

    }  /* end of the ido loop */

/*
 * Shift the prim coordinates for any attachable frame prims
 * to their new page positions.
 */
    if (page_index_xmax > page_index_xmin  &&  page_index_ymax > page_index_ymin) {
        tiny = (CSW_F)((page_index_xmax -
                        page_index_xmin +
                        page_index_ymax -
                        page_index_ymin) / 20000.0);
    }
    else {
        tiny = 0.0f;
    }

    for (i=0; i<num_frame_list; i++) {

        fp = frame_list + i;
        if (fp->attach_frame < 0  ||  fp->attach_frame >= num_frame_list) {
            continue;
        }

        dx = fp->px1 - fp->px1orig;
        dy = fp->py1 - fp->py1orig;
        adx = dx;
        if (adx < 0.0) adx = -adx;
        ady = dy;
        if (ady < 0.0) ady = -ady;

        if (adx <= tiny  &&  ady <= tiny) {
            continue;
        }

        //shift_frame_prims (i, dx, dy);
    }

    return;

}


/*
 * Private utility to separate a string into substrings when the original
 * string has GTX_MSG_STRING_SEPARATOR character sequences inside of it.
 * When multiple strings are needed in a command, they are separated by
 * this separator.
 *
 * This function will modify the original input buffer.
 */
void CDisplayList::extract_sub_strings (char *buffer,
                                        int *sindex,
                                        int nstr)
{
    int          i, j, n3, n;
    char         *ctmp, csep[100];

    strcpy (csep, GTX_MSG_STRING_SEPARATOR);
    n3 = strlen (csep);

    sindex[0] = 0;
    for (i=1; i<nstr; i++) {
        sindex[i] = -1;
    }
    n = 0;
    for (i=1; i<nstr; i++) {
        ctmp = strstr (buffer+n, csep);
        if (ctmp == NULL) {
            break;
        }
        for (j=0; j<n3; j++) {
            *ctmp = '\0';
            ctmp++;
        }
        n = ctmp - buffer;
        sindex[i] = n;
    }

    return;
}


/*
 * Private method to set all axis structs for a specified
 * frame to default contents.
 */
void CDisplayList::default_frame_axes (FRameStruct *fp)
{
    FRameAxisStruct    *ap;

    if (fp == NULL) {
        return;
    }

    ap = &(fp->left_axis);
    ap->label_flag = 0;
    ap->tick_flag = 1;
    ap->tick_direction = 1;
    ap->caption[0] = '\0';
    ap->major_interval = -1.0f;
    ap->line_red = 0;
    ap->line_green = 0;
    ap->line_blue = 0;
    ap->text_red = 0;
    ap->text_green = 0;
    ap->text_blue = 0;
    ap->line_thickness = .007f;
    ap->text_size = .10f;
    ap->text_thickness = .01f;
    ap->text_font = 1000;
    ap->selectable_item = -1;
    ap->label_anchor = -1;
    ap->label_angle = 0.0;

    ap = &(fp->right_axis);
    ap->label_flag = 0;
    ap->tick_flag = 1;
    ap->tick_direction = 1;
    ap->caption[0] = '\0';
    ap->major_interval = -1.0f;
    ap->line_red = 0;
    ap->line_green = 0;
    ap->line_blue = 0;
    ap->text_red = 0;
    ap->text_green = 0;
    ap->text_blue = 0;
    ap->line_thickness = .007f;
    ap->text_size = .10f;
    ap->text_thickness = .01f;
    ap->text_font = 1000;
    ap->selectable_item = -1;
    ap->label_anchor = -1;
    ap->label_angle = 0.0;

    ap = &(fp->bottom_axis);
    ap->label_flag = 0;
    ap->tick_flag = 1;
    ap->tick_direction = 1;
    ap->caption[0] = '\0';
    ap->major_interval = -1.0f;
    ap->line_red = 0;
    ap->line_green = 0;
    ap->line_blue = 0;
    ap->text_red = 0;
    ap->text_green = 0;
    ap->text_blue = 0;
    ap->line_thickness = .007f;
    ap->text_size = .10f;
    ap->text_thickness = .01f;
    ap->text_font = 1000;
    ap->selectable_item = -1;
    ap->label_anchor = -1;
    ap->label_angle = 0.0;

    ap = &(fp->top_axis);
    ap->label_flag = 0;
    ap->tick_flag = 1;
    ap->tick_direction = 1;
    ap->caption[0] = '\0';
    ap->major_interval = -1.0f;
    ap->line_red = 0;
    ap->line_green = 0;
    ap->line_blue = 0;
    ap->text_red = 0;
    ap->text_green = 0;
    ap->text_blue = 0;
    ap->line_thickness = .007f;
    ap->text_size = .10f;
    ap->text_thickness = .01f;
    ap->text_font = 1000;
    ap->selectable_item = -1;
    ap->label_anchor = -1;
    ap->label_angle = 0.0;

    return;

}



/*
 * Public function to modify frame axis values.
 */
int CDisplayList::SetFrameAxisValues (
    char         *cdata,
    int          axis_id,
    int          label_flag,
    int          tick_flag,
    int          tick_direction,
    double       major_interval,
    int          line_red,
    int          line_green,
    int          line_blue,
    int          text_red,
    int          text_green,
    int          text_blue,
    double       line_thickness,
    double       text_size,
    double       text_thickness,
    int          text_font,
    int          caption_flag)
{
    int               fnum;
    FRameStruct       *fp;
    FRameAxisStruct   *ap;
    char              frame_name[100],
                      caption[200];
    int               sindex[4];
    char              c2000[2000];

    if (cdata == NULL  ||
        frame_list == NULL) {
        return -2;
    }

    frame_name[0] = '\0';
    caption[0] = '\0';

    strncpy (c2000, cdata, 1999);
    c2000[1999] = '\0';
    extract_sub_strings (c2000, sindex, 2);

    strncpy (frame_name, c2000 + sindex[0], 99);
    frame_name[99] = '\0';
    if (sindex[1] >= 0) {
        strncpy (caption, c2000+sindex[1], 199);
        caption[199] = '\0';
    }
    fnum = find_frame (frame_name);
    if (fnum < 0  ||  fnum >= num_frame_list) {
        return -2;
    }
    fp = frame_list + fnum;

    ap = NULL;
    if (axis_id == GTX_FRAME_LEFT_AXIS) {
        ap = &(fp->left_axis);
    }
    else if (axis_id == GTX_FRAME_RIGHT_AXIS) {
        ap = &(fp->right_axis);
    }
    else if (axis_id == GTX_FRAME_BOTTOM_AXIS) {
        ap = &(fp->bottom_axis);
    }
    else if (axis_id == GTX_FRAME_TOP_AXIS) {
        ap = &(fp->top_axis);
    }

    if (ap == NULL) {
        return -2;
    }

    ap->label_flag = label_flag;
    ap->tick_flag = tick_flag;
    ap->tick_direction = tick_direction;
    strncpy (ap->caption, caption, 199);
    ap->caption[199] = '\0';
    ap->major_interval = (CSW_F)major_interval;
    ap->line_red = line_red;
    ap->line_green = line_green;
    ap->line_blue = line_blue;
    ap->text_red = text_red;
    ap->text_green = text_green;
    ap->text_blue = text_blue;
    ap->line_thickness = (CSW_F)line_thickness;
    ap->text_size = (CSW_F)text_size;
    ap->text_thickness = (CSW_F)text_thickness;
    ap->text_font = text_font;
    ap->caption_flag = caption_flag;
    ap->label_anchor = -1;
    ap->label_angle = 0.0;

    frame_layout_needed = 1;

    return 1;

}

void CDisplayList::shift_xy (CSW_F *xy, int npts, CSW_F dx, CSW_F dy)
{
    int i;

    for (i=0; i<npts; i++) {
        *xy += dx;
        xy++;
        *xy += dy;
        xy++;
    }

    return;
}

void CDisplayList::shift_frame_prims (int fnum,
                                      CSW_F dx,
                                      CSW_F dy)
{
    int            i;
    FIllPrim       *fptr;
    LInePrim       *lptr;
    TExtPrim       *tptr;
    SYmbPrim       *sptr;
    SHapePrim      *shptr;
    IMagePrim      *imptr;

    int            fp_size = (int)fill_prim_list.size();
    FIllPrim       *fp_data = fill_prim_list.data();

    if (fp_data != NULL  &&  fp_size > 0) {
        for (i=0; i<fp_size; i++) {
            fptr = fp_data + i;
            if (fptr->frame_num != fnum) {
                continue;
            }
            if (fptr->deleted_flag == 1) {
                continue;
            }
            if (fptr->visible_flag == 0) {
                continue;
            }
            if (fptr->scaleable == 1) {
                continue;
            }
            shift_xy (fptr->xypts, fptr->npts, dx, dy);
        }
    }

    int        lp_size = (int)line_prim_list.size();
    LInePrim   *lp_data = line_prim_list.data();

    if (lp_data != NULL  &&  lp_size > 0) {
        for (i=0; i<lp_size; i++) {
            lptr = lp_data + i;
            if (lptr->frame_num != fnum) {
                continue;
            }
            if (lptr->deleted_flag == 1) {
                continue;
            }
            if (lptr->visible_flag == 0) {
                continue;
            }
            if (lptr->scaleable == 1) {
                continue;
            }
            shift_xy (lptr->xypts, lptr->npts, dx, dy);
        }
    }

    int        tp_size = (int)text_prim_list.size();
    TExtPrim   *tp_data = text_prim_list.data();

    if (tp_data != NULL  &&  tp_size > 0) {
        for (i=0; i<tp_size; i++) {
            tptr = tp_data + i;
            if (tptr->frame_num != fnum) {
                continue;
            }
            if (tptr->deleted_flag == 1) {
                continue;
            }
            if (tptr->visible_flag == 0) {
                continue;
            }
            if (tptr->scaleable == 1) {
                continue;
            }
            tptr->x += dx;
            tptr->y += dy;
        }
    }

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data != NULL  &&  sp_size > 0) {
        for (i=0; i<sp_size; i++) {
            sptr = sp_data + i;
            if (sptr->frame_num != fnum) {
                continue;
            }
            if (sptr->deleted_flag == 1) {
                continue;
            }
            if (sptr->visible_flag == 0) {
                continue;
            }
            if (sptr->scaleable == 1) {
                continue;
            }
            sptr->x += dx;
            sptr->y += dy;
        }
    }

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data != NULL  &&  hp_size > 0) {
        for (i=0; i<hp_size; i++) {
            shptr = hp_data + i;
            if (shptr->frame_num != fnum) {
                continue;
            }
            if (shptr->deleted_flag == 1) {
                continue;
            }
            if (shptr->visible_flag == 0) {
                continue;
            }
            if (shptr->scaleable == 1) {
                continue;
            }
            shptr->fval[0] += dx;
            shptr->fval[1] += dy;
        }
    }

    int           ip_size = (int)image_prim_list.size();
    IMagePrim     *ip_data = image_prim_list.data();

    if (ip_data != NULL  &&  ip_size > 0) {
        for (i=0; i<ip_size; i++) {
            imptr = ip_data + i;
            if (imptr->frame_num != fnum) {
                continue;
            }
            if (imptr->deleted_flag == 1) {
                continue;
            }
            if (imptr->visible_flag == 0) {
                continue;
            }
            if (imptr->scaleable == 1) {
                continue;
            }
        }
    }

    return;

}  /*  end of function shift_frame_prims */



void CDisplayList::SetMinimumFrameSeparation (double fval)
{
    minimum_frame_separation = (CSW_F)fval / 2.0f;
}



/*
 * Labeled frame borders can have lines, text and fills.  These
 * need to be drawn, but without java side frame clipping.  Any
 * primitive which has a border_num value the same as the fnum
 * value passed to this function is drawn here.
 */
void CDisplayList::draw_frame_border_prims (int fnum) {

    int            i;
    LInePrim       *lptr;
    FIllPrim       *fptr;
    TExtPrim       *tptr;
    CSW_F          xoff, yoff;

    if (fnum < 0  ||  fnum >= num_frame_list) {
        return;
    }

    int  lp_size = (int)line_prim_list.size();
    LInePrim  *lp_data = line_prim_list.data();

    if (lp_size > 0  &&  lp_data != NULL) {

        for (i=0; i<lp_size; i++) {

            lptr = lp_data + i;

            if (lptr->border_num != fnum) {
                continue;
            }

            if (lptr->deleted_flag == 1) {
                continue;
            }

            if (lptr->axis_num >= 0) {
                continue;
            }

            ezx_java_obj.ezx_SetFrameInJavaArea (-1);
            ezx_java_obj.ezx_SetAlphaValue (255);

            gtx_drawprim_obj.gtx_cliplineprim(lptr->xypts,
                             lptr->npts,
                             lptr->smooth_flag,
                             (CSW_F)lptr->thick,
                             lptr->red,
                             lptr->green,
                             lptr->blue,
                             lptr->dashpat,
                             (CSW_F)lptr->dashscale,
                             lptr->arrowstyle);

        }
    }

    int  fp_size = (int)fill_prim_list.size();
    FIllPrim  *fp_data = fill_prim_list.data();

    if (fp_data != NULL  &&  fp_size > 0) {

        for (i=0; i<fp_size; i++) {

            fptr = fp_data + i;

            if (fptr->border_num != fnum) {
                continue;
            }

            if (fptr->deleted_flag == 1) {
                continue;
            }

            ezx_java_obj.ezx_SetFrameInJavaArea (-1);
            ezx_java_obj.ezx_SetAlphaValue (255);

            gtx_drawprim_obj.gtx_clipfillprim(fptr->xypts,
                             fptr->npts,
                             fptr->smooth_flag,
                             fptr->fill_red,
                             fptr->fill_green,
                             fptr->fill_blue,
                             fptr->pat_red,
                             fptr->pat_green,
                             fptr->pat_blue,
                             fptr->border_red,
                             fptr->border_green,
                             fptr->border_blue,
                             fptr->thick,
                             fptr->outline,
                             fptr->pattern,
                             fptr->patscale,
                             fptr->linepatt,
                             fptr->dashscale,
                             fptr->xmin,
                             fptr->ymin,
                             fptr->xmax,
                             fptr->ymax);
        }
    }

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data != NULL  &&  tp_size > 0) {

        for (i=0; i<tp_size; i++) {

            tptr = tp_data + i;

            if (tptr->border_num != fnum) {
                continue;
            }

            if (tptr->deleted_flag == 1) {
                continue;
            }

            ezx_java_obj.ezx_SetFrameInJavaArea (-1);
            ezx_java_obj.ezx_SetAlphaValue (255);

            gtx_drawprim_obj.gtx_reset_exact_text_length ();

            xoff = tptr->xoff * page_units_per_inch;
            yoff = tptr->yoff * page_units_per_inch;

         /*
          * The text character fills and strokes themselves have the
          * highest priority and are set to 3.
          */
            ezx_java_obj.ezx_SetDrawingPriority (3);
            gtx_drawprim_obj.gtx_cliptextprim(tptr->x + xoff,
                             tptr->y + yoff,
                             tptr->chardata,
                             tptr->anchor,
                             tptr->thick,
                             tptr->border_red,
                             tptr->border_green,
                             tptr->border_blue,
                             tptr->fill_red,
                             tptr->fill_green,
                             tptr->fill_blue,
                             tptr->angle,
                             tptr->size,
                             tptr->font_num);

            gtx_drawprim_obj.gtx_reset_exact_text_length ();
        }
    }

    ezx_java_obj.ezx_SetDrawingPriority (0);

    return;

}


void CDisplayList::reborder_frame (int fnum)
{
    FRameStruct     *frptr;

    if (fnum < 0  ||  fnum >= num_frame_list  ||
        frame_list == NULL) {
        return;
    }

    frptr = frame_list + fnum;

    reborder_frame (frptr);

    return;
}


/*
 * Remove any existing border label primitives that exist for
 * this frame and add new primitives reflecting the frame's
 * current setup.
 */
void CDisplayList::reborder_frame (FRameStruct *frptr)
{
    int           fnum, fsave;

/*
 * Make sure the frame is valid and needs a labeled border.
 */
    if (frptr == NULL) {
        return;
    }
    if (frame_list == NULL) {
        return;
    }

    fnum = frptr - frame_list;
    if (fnum < 0  ||  fnum >= num_frame_list) {
        return;
    }

    if (frptr->borderflag < 2) {
        return;
    }

/*
 * Delete all the sub prims of all frame prims.
 */
    if (border_margin_only_flag == 0) {
        delete_frame_border_lines (fnum);
        delete_frame_border_fills (fnum);
        delete_frame_border_texts (fnum);
    }

    fsave = current_frame_num;
    current_frame_num = -1;
    current_border_num = fnum;

    calc_frame_border_prims (frptr);

    current_border_num = -1;
    current_frame_num = fsave;

    return;
}



void CDisplayList::delete_frame_border_lines (int fnum)
{
    int            i;
    LInePrim       *lptr;

    int        lp_size = (int)line_prim_list.size();
    LInePrim   *lp_data = line_prim_list.data();

    if (lp_data == NULL  ||  lp_size < 1) {
        return;
    }

    for (i=0; i<lp_size; i++) {

        lptr = lp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }

        if (lptr->border_num != fnum) {
            continue;
        }

        csw_Free (lptr->xypts);
        lptr->xypts = NULL;
        lptr->deleted_flag = 1;
        lptr->npts = 0;
        lptr->maxpts = 0;
        add_available_line (i);

    }

    return;

}  /*  end of function delete_frame_border_lines */



void CDisplayList::delete_frame_border_fills (int fnum)
{
    int            i;
    FIllPrim       *fptr;

    int        fp_size = (int)fill_prim_list.size();
    FIllPrim   *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    for (i=0; i<fp_size; i++) {

        fptr = fp_data + i;

        if (fptr->deleted_flag == 1) {
            continue;
        }

        if (fptr->border_num != fnum) {
            continue;
        }

        csw_Free (fptr->xypts);
        fptr->xypts = NULL;
        fptr->npts = 0;
        fptr->maxpts = 0;
        fptr->deleted_flag = 1;
        add_available_fill (i);

    }

    return;

}  /*  end of function delete_frame_border_fills */



void CDisplayList::delete_frame_border_texts (int fnum)
{
    int            i;
    TExtPrim       *tptr;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }

    for (i=0; i<tp_size; i++) {

        tptr = tp_data + i;

        if (tptr->deleted_flag == 1) {
            continue;
        }

        if (tptr->border_num != fnum) {
            continue;
        }

        csw_Free (tptr->chardata);
        tptr->chardata = NULL;
        tptr->deleted_flag = 1;
        add_available_text (i);

    }

    return;

}  /*  end of function delete_frame_border_texts */




/*
 ***************************************************************************

    Recalculate the lines and texts and (in the future) fills for any border
    labels, tick marks, captions etc.

 ***************************************************************************

*/

void CDisplayList::calc_frame_border_prims (FRameStruct *frptr)
{
    FRameAxisStruct     *axptr;
    CSW_F               pxmin, pymin, pxmax, pymax;
    CSW_F               fx1, fy1, fx2, fy2;

    if (frptr == NULL) {
        return;
    }

    pxmin = frptr->px1;
    pymin = frptr->py1;
    pxmax = frptr->px2;
    pymax = frptr->py2;

    fx1 = (CSW_F)frptr->x1;
    fy1 = (CSW_F)frptr->y1;
    fx2 = (CSW_F)frptr->x2;
    fy2 = (CSW_F)frptr->y2;

    axptr = &(frptr->bottom_axis);
    if (axptr->label_flag == 1  ||
        axptr->caption_flag == 1  ||
        axptr->tick_flag == 1) {
        calc_frame_axis_prims (axptr,
                               pxmin,
                               pymin,
                               pxmax,
                               pymax,
                               fx1,
                               fx2,
                               1);
    }

    axptr = &(frptr->left_axis);
    if (axptr->label_flag == 1  ||
        axptr->caption_flag == 1  ||
        axptr->tick_flag == 1) {
        calc_frame_axis_prims (axptr,
                               pxmin,
                               pymin,
                               pxmax,
                               pymax,
                               fy1,
                               fy2,
                               2);
    }

    axptr = &(frptr->top_axis);
    if (axptr->label_flag == 1  ||
        axptr->caption_flag == 1  ||
        axptr->tick_flag == 1) {
        calc_frame_axis_prims (axptr,
                               pxmin,
                               pymin,
                               pxmax,
                               pymax,
                               fx1,
                               fx2,
                               3);
    }

    axptr = &(frptr->right_axis);
    if (axptr->label_flag == 1  ||
        axptr->caption_flag == 1  ||
        axptr->tick_flag == 1) {
        calc_frame_axis_prims (axptr,
                               pxmin,
                               pymin,
                               pxmax,
                               pymax,
                               fy1,
                               fy2,
                               4);
    }
    return;

}



/*
 ******************************************************************************

    Use the graph calculation utilities developed for easyx to generate
    line and text primitives for this axis.  All the references to functions
    and constants in the easyx utilities are done in this function only.

 ******************************************************************************

*/

int CDisplayList::calc_frame_axis_prims (FRameAxisStruct *axptr,
                                         CSW_F px1,
                                         CSW_F py1,
                                         CSW_F px2,
                                         CSW_F py2,
                                         CSW_F val1,
                                         CSW_F val2,
                                         int position)
{
    GRaphUnion         uval;
    GRaphLineRec       *glines, *lp;
    GRaphTextRec       *gtexts, *tp;
    int                nglines,
                       ngtexts;
    int                i, j, istat,
                       gpos;
    int                save_border;
    CSW_F              axmin,
                       axmax,
                       axmajor;
    double             *xd, *yd;

/*
 * Set the grpah axis position.
 */
    if (position == 1) {
        gpos = GPF_BOTTOM_AXIS;
    }
    else if (position == 2) {
        gpos = GPF_LEFT_AXIS;
    }
    else if (position == 3) {
        gpos = GPF_TOP_AXIS;
    }
    else if (position == 4) {
        gpos = GPF_RIGHT_AXIS;
    }
    else {
        return 0;
    }

/*
 * Set all the graph parameters to their defaults.
 */
    gpf_graph_obj.gpf_SetGraphDefaults ();

/*
 * Define the position of the frame on the plot page for the
 * graph utilities.
 */
    gpf_graph_obj.gpf_StartGraph (px1,
                    py1,
                    px2,
                    py2,
                    page_units_per_inch,
                    0,    /* only used for time graphs */
                    page_units_per_inch
                   );

/*
 * Set the axis parameters that are controlled by the frame axis structure.
 */

  //label anchor and angle.
    uval.ival = axptr->label_anchor;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_ANCHOR, uval);
    uval.fval = (float)axptr->label_angle;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_ANGLE, uval);

  // text fonts
    uval.ival = axptr->text_font;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_FONT, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_FONT, uval);

  // text sizes
    uval.fval = axptr->text_size * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_SIZE, uval);
    uval.fval *= 1.1f;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_SIZE, uval);

  // major interval
    if (axptr->major_interval > 0.0) {
        uval.fval = axptr->major_interval;
        gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_INTERVAL, uval);
    }

  // min and max values on the axis
    uval.fval = val1;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MIN_VALUE, uval);
    uval.fval = val2;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAX_VALUE, uval);

  // minor and major tick lengths
    uval.fval = .05f * page_units_per_inch * axptr->tick_direction;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_LENGTH, uval);
    uval.fval = .08f * page_units_per_inch * axptr->tick_direction;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_LENGTH, uval);

    uval.fval = .05f * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_GAP, uval);
    uval.fval = .08f * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_GAP, uval);

  // line thickness
    uval.fval = axptr->line_thickness * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LINE_THICK, uval);

  // text thickness
    uval.fval = axptr->text_thickness * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_THICK, uval);

/*
 * Generate graph primitive structures for the axis.
 */
    istat = gpf_graph_obj.gpf_CalcGraphAxis (
        gpos,
        axptr->caption,
        val1,
        val2,
        0.0, // linear axis
        axptr->label_flag,
        NULL, // no custom labels
        NULL,
        0,
        &axmin,
        &axmax,
        &axmajor,
        &glines,
        &nglines,
        &gtexts,
        &ngtexts
    );

    if (istat == -1) {
        return -1;
    }

    axptr->page_margin = gpf_graph_obj.gpf_GetAxisMarginSize ();

    if (border_margin_only_flag == 1) {
        gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
        gpf_graph_obj.gpf_FreeGraphText (gtexts, ngtexts);
        return 1;
    }

    save_border = current_border_num;
    save_current_graphic_attributes ();
    SetDefaultGraphicAttributes ();
    current_border_num = save_border;

    current_line_red = axptr->line_red;
    current_line_green = axptr->line_green;
    current_line_blue = axptr->line_blue;
    current_line_alpha = 255;
    current_line_thickness = axptr->line_thickness;

    for (i=0; i<nglines; i++) {
        lp = glines + i;
        xd = (double *)csw_Malloc (lp->npts * 2 * sizeof(double));
        if (xd == NULL) {
            continue;
        }
        yd = xd + lp->npts;
        for (j=0; j<lp->npts; j++) {
            xd[j] = lp->x[j];
            yd[j] = lp->y[j];
        }
        AddLine (xd,
                 yd,
                 lp->npts);
        csw_Free (xd);
        xd = NULL;
        yd = NULL;
    }

    current_text_red = axptr->text_red;
    current_text_green = axptr->text_green;
    current_text_blue = axptr->text_blue;
    current_text_alpha = 255;
    current_text_thickness = axptr->text_thickness;
    current_text_font = axptr->text_font;

    for (i=0; i<ngtexts; i++) {
        tp = gtexts + i;
        AddText ((double)tp->x,
                 (double)tp->y,
                 tp->size / page_units_per_inch,
                 tp->angle,
                 tp->text);
    }

    gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
    gpf_graph_obj.gpf_FreeGraphText (gtexts, ngtexts);

    unsave_current_graphic_attributes ();

    return 1;

}


void CDisplayList::SetDefaultGraphicAttributes (void)
{
    current_border_num = -1;
    current_axis_num = -1;
    current_frame_num = -1;
    current_graph_num = -1;
    current_selectable_object_num = -1;
    current_xaxis_num = -1;
    current_yaxis_num = -1;

    current_layer_num = -1;
    current_item_num = -1;
    current_size_units = 0;

    current_line_thickness = 0.001f;
    current_line_pattern = -1;
    current_line_pattern_size = 1.0f;
    current_line_symbol = -1;
    current_line_arrow_style = 0;

    current_text_font = -1;
    current_text_thickness = 0.0f;
    current_text_anchor = 1;
    current_text_x_offset = 0.0f;
    current_text_y_offset = 0.0f;
    current_text_background = 0;

    current_fill_outline_flag = 1;
    current_fill_pattern = -1;
    current_fill_pattern_size = 1.0f;

/*
 * default has no image data or color bands.
 */
    memset (current_image_low_values, 0, 1000 * sizeof(CSW_F));
    memset (current_image_high_values, 0, 1000 * sizeof(CSW_F));
    memset (current_image_red_values, 0, 1000 * sizeof(int));
    memset (current_image_green_values, 0, 1000 * sizeof(int));
    memset (current_image_blue_values, 0, 1000 * sizeof(int));
    memset (current_image_transparency_values, 0, 1000 * sizeof(int));
    current_number_image_values = 0;
    current_image_name[0] = '\0';
    current_image_id = 0;

    current_image_origin = 0;
    current_image_column1 = 0;
    current_image_row1 = 0;
    current_image_colspace = 1;
    current_image_rowspace = 1;

/*
 * default background is white, foreground is black
 * and all default object colors are black.
 */
    current_background_red = 255;
    current_background_green = 255;
    current_background_blue = 255;
    current_background_alpha = 255;
    current_foreground_red = 5;
    current_foreground_green = 5;
    current_foreground_blue = 5;
    current_foreground_alpha = 255;

    current_global_red = 0;
    current_global_green = 0;
    current_global_blue = 0;
    current_global_alpha = 255;
    current_line_red = 0;
    current_line_green = 0;
    current_line_blue = 0;
    current_line_alpha = 255;
    current_fill_red = 0;
    current_fill_green = 0;
    current_fill_blue = 0;
    current_fill_alpha = 255;
    current_symbol_red = 0;
    current_symbol_green = 0;
    current_symbol_blue = 0;
    current_symbol_alpha = 255;
    current_text_red = 0;
    current_text_green = 0;
    current_text_blue = 0;
    current_text_alpha = 255;
    current_text_fill_red = 0;
    current_text_fill_green = 0;
    current_text_fill_blue = 0;
    current_text_fill_alpha = 255;
    current_fill_pattern_red = 0;
    current_fill_pattern_green = 0;
    current_fill_pattern_blue = 0;
    current_fill_pattern_alpha = 255;
    current_fill_border_red = 0;
    current_fill_border_green = 0;
    current_fill_border_blue = 0;
    current_fill_border_alpha = 255;

    current_image_null_value = 0.0;
}


void CDisplayList::unsave_current_graphic_attributes (void)
{
    current_frame_clip_flag = saved_frame_clip_flag;
    current_border_num = saved_border_num;
    current_axis_num = saved_axis_num;
    current_frame_num = saved_frame_num;
    current_graph_num = saved_graph_num;
    current_selectable_object_num = saved_selectable_object_num;
    current_selectable_object = saved_selectable_object;
    current_xaxis_num = saved_xaxis_num;
    current_yaxis_num = saved_yaxis_num;

    current_layer_num = saved_layer_num;
    current_item_num = saved_item_num;
    current_size_units = saved_size_units;

    current_line_thickness = saved_line_thickness;
    current_line_pattern = saved_line_pattern;
    current_line_pattern_size = saved_line_pattern_size;
    current_line_symbol = saved_line_symbol;
    current_line_arrow_style = saved_line_arrow_style;

    current_text_font = saved_text_font;
    current_text_thickness = saved_text_thickness;
    current_text_anchor = saved_text_anchor;
    current_text_x_offset = saved_text_x_offset;
    current_text_y_offset = saved_text_y_offset;
    current_text_background = saved_text_background;

    current_fill_outline_flag = saved_fill_outline_flag;
    current_fill_pattern = saved_fill_pattern;
    current_fill_pattern_size = saved_fill_pattern_size;

    memcpy (current_image_low_values, saved_image_low_values, 1000 * sizeof(CSW_F));
    memcpy (current_image_high_values, saved_image_high_values, 1000 * sizeof(CSW_F));
    memcpy (current_image_red_values, saved_image_red_values, 1000 * sizeof(int));
    memcpy (current_image_green_values, saved_image_green_values, 1000 * sizeof(int));
    memcpy (current_image_blue_values, saved_image_blue_values, 1000 * sizeof(int));
    memcpy (current_image_transparency_values, saved_image_transparency_values, 1000 * sizeof(int));
    current_number_image_values = saved_number_image_values;
    strcpy (current_image_name, saved_image_name);
    current_image_id = saved_image_id;

    current_image_origin = saved_image_origin;
    current_image_column1 = saved_image_column1;
    current_image_row1 = saved_image_row1;
    current_image_colspace = saved_image_colspace;
    current_image_rowspace = saved_image_rowspace;

    current_background_red = saved_background_red;
    current_background_green = saved_background_green;
    current_background_blue = saved_background_blue;
    current_background_alpha = saved_background_alpha;
    current_foreground_red = saved_foreground_red;
    current_foreground_green = saved_foreground_green;
    current_foreground_blue = saved_foreground_blue;
    current_foreground_alpha = saved_foreground_alpha;

    current_global_red = saved_global_red;
    current_global_green = saved_global_green;
    current_global_blue = saved_global_blue;
    current_global_alpha = saved_global_alpha;
    current_line_red = saved_line_red;
    current_line_green = saved_line_green;
    current_line_blue = saved_line_blue;
    current_line_alpha = saved_line_alpha;
    current_fill_red = saved_fill_red;
    current_fill_green = saved_fill_green;
    current_fill_blue = saved_fill_blue;
    current_fill_alpha = saved_fill_alpha;
    current_symbol_red = saved_symbol_red;
    current_symbol_green = saved_symbol_green;
    current_symbol_blue = saved_symbol_blue;
    current_symbol_alpha = saved_symbol_alpha;
    current_text_red = saved_text_red;
    current_text_green = saved_text_green;
    current_text_blue = saved_text_blue;
    current_text_alpha = saved_text_alpha;
    current_text_fill_red = saved_fill_red;
    current_text_fill_green = saved_fill_green;
    current_text_fill_blue = saved_fill_blue;
    current_text_fill_alpha = saved_fill_alpha;
    current_fill_pattern_red = saved_fill_pattern_red;
    current_fill_pattern_green = saved_fill_pattern_green;
    current_fill_pattern_blue = saved_fill_pattern_blue;
    current_fill_pattern_alpha = saved_fill_pattern_alpha;
    current_fill_border_red = saved_fill_border_red;
    current_fill_border_green = saved_fill_border_green;
    current_fill_border_blue = saved_fill_border_blue;
    current_fill_border_alpha = saved_fill_border_alpha;

    current_image_null_value = saved_image_null_value;
}


void CDisplayList::save_current_graphic_attributes (void)
{
    saved_frame_clip_flag = current_frame_clip_flag;
    saved_border_num = current_border_num;
    saved_axis_num = current_axis_num;
    saved_frame_num = current_frame_num;
    saved_graph_num = current_graph_num;
    saved_selectable_object_num = current_selectable_object_num;
    saved_selectable_object = current_selectable_object;
    saved_xaxis_num = current_xaxis_num;
    saved_yaxis_num = current_yaxis_num;

    saved_layer_num = current_layer_num;
    saved_item_num = current_item_num;
    saved_size_units = current_size_units;

    saved_line_thickness = current_line_thickness;
    saved_line_pattern = current_line_pattern;
    saved_line_pattern_size = current_line_pattern_size;
    saved_line_symbol = current_line_symbol;
    saved_line_arrow_style = current_line_arrow_style;

    saved_text_font = current_text_font;
    saved_text_thickness = current_text_thickness;
    saved_text_anchor = current_text_anchor;
    saved_text_x_offset = current_text_x_offset;
    saved_text_y_offset = current_text_y_offset;
    saved_text_background = current_text_background;

    saved_fill_outline_flag = current_fill_outline_flag;
    saved_fill_pattern = current_fill_pattern;
    saved_fill_pattern_size = current_fill_pattern_size;

    memcpy (saved_image_low_values, current_image_low_values, 1000 * sizeof(CSW_F));
    memcpy (saved_image_high_values, current_image_high_values, 1000 * sizeof(CSW_F));
    memcpy (saved_image_red_values, current_image_red_values, 1000 * sizeof(int));
    memcpy (saved_image_green_values, current_image_green_values, 1000 * sizeof(int));
    memcpy (saved_image_blue_values, current_image_blue_values, 1000 * sizeof(int));
    memcpy (saved_image_transparency_values, current_image_transparency_values, 1000 * sizeof(int));
    saved_number_image_values = current_number_image_values;
    strcpy (saved_image_name, current_image_name);
    saved_image_id = current_image_id;

    saved_image_origin = current_image_origin;
    saved_image_column1 = current_image_column1;
    saved_image_row1 = current_image_row1;
    saved_image_colspace = current_image_colspace;
    saved_image_rowspace = current_image_rowspace;

    saved_background_red = current_background_red;
    saved_background_green = current_background_green;
    saved_background_blue = current_background_blue;
    saved_background_alpha = current_background_alpha;
    saved_foreground_red = current_foreground_red;
    saved_foreground_green = current_foreground_green;
    saved_foreground_blue = current_foreground_blue;
    saved_background_alpha = current_background_alpha;

    saved_global_red = current_global_red;
    saved_global_green = current_global_green;
    saved_global_blue = current_global_blue;
    saved_global_alpha = current_global_alpha;
    saved_line_red = current_line_red;
    saved_line_green = current_line_green;
    saved_line_blue = current_line_blue;
    saved_line_alpha = current_line_alpha;
    saved_fill_red = current_fill_red;
    saved_fill_green = current_fill_green;
    saved_fill_blue = current_fill_blue;
    saved_fill_alpha = current_fill_alpha;
    saved_symbol_red = current_symbol_red;
    saved_symbol_green = current_symbol_green;
    saved_symbol_blue = current_symbol_blue;
    saved_symbol_alpha = current_symbol_alpha;
    saved_text_red = current_text_red;
    saved_text_green = current_text_green;
    saved_text_blue = current_text_blue;
    saved_text_alpha = current_text_alpha;
    saved_text_fill_red = current_fill_red;
    saved_text_fill_green = current_fill_green;
    saved_text_fill_blue = current_fill_blue;
    saved_text_fill_alpha = current_fill_alpha;
    saved_fill_pattern_red = current_fill_pattern_red;
    saved_fill_pattern_green = current_fill_pattern_green;
    saved_fill_pattern_blue = current_fill_pattern_blue;
    saved_fill_pattern_alpha = current_fill_pattern_alpha;
    saved_fill_border_red = current_fill_border_red;
    saved_fill_border_green = current_fill_border_green;
    saved_fill_border_blue = current_fill_border_blue;
    saved_fill_border_alpha = current_fill_border_alpha;

    saved_image_null_value = current_image_null_value;
}


void CDisplayList::find_frame_limits (int frame_num,
                                      double *xmin_out,
                                      double *ymin_out,
                                      double *xmax_out,
                                      double *ymax_out)
{
    double         xmin, ymin, xmax, ymax;
    double         x1, yt1, x2, y2;
    double         cx1, cy1, cx2, cy2;
    int            i;
    LInePrim       *lptr;
    FIllPrim       *fptr;
    TExtPrim       *tptr;
    SYmbPrim       *sptr;
    SHapePrim      *shptr;
    IMagePrim      *imptr;
    AXisPrim       *aptr;
    DLSurf         *surf;
    DLContour      *cont;

    xmin = *xmin_out;
    ymin = *ymin_out;
    xmax = *xmax_out;
    ymax = *ymax_out;

    int        lp_size = (int)line_prim_list.size();
    LInePrim   *lp_data = line_prim_list.data();

    if (lp_data != NULL  &&  lp_size > 0) {
        for (i=0; i<lp_size; i++) {
            lptr = lp_data + i;
            if (lptr->deleted_flag == 1  ||
                lptr->scaleable == 0  ||
                lptr->frame_num != frame_num) {
                continue;
            }
            CalcLineBounds (i, &x1, &yt1, &x2, &y2);
            if (x1 < xmin) xmin = x1;
            if (yt1 < ymin) ymin = yt1;
            if (x2 > xmax) xmax = x2;
            if (y2 > ymax) ymax = y2;
        }
    }

    int        fp_size = (int)fill_prim_list.size();
    FIllPrim   *fp_data = fill_prim_list.data();

    if (fp_data != NULL  &&  fp_size > 0) {
        for (i=0; i<fp_size; i++) {
            fptr = fp_data + i;
            if (fptr->deleted_flag == 1  ||
                fptr->scaleable == 0  ||
                fptr->frame_num != frame_num) {
                continue;
            }
            CalcFillBounds (i, &x1, &yt1, &x2, &y2);
            if (x1 < xmin) xmin = x1;
            if (yt1 < ymin) ymin = yt1;
            if (x2 > xmax) xmax = x2;
            if (y2 > ymax) ymax = y2;
        }
    }

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data != NULL  &&  tp_size > 0) {
        for (i=0; i<tp_size; i++) {
            tptr = tp_data + i;
            if (tptr->deleted_flag == 1  ||
                tptr->scaleable == 0  ||
                tptr->frame_num != frame_num) {
                continue;
            }
            CalcTextBounds (i, &x1, &yt1, &x2, &y2);
            if (x1 < xmin) xmin = x1;
            if (yt1 < ymin) ymin = yt1;
            if (x2 > xmax) xmax = x2;
            if (y2 > ymax) ymax = y2;
        }
    }

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data != NULL  &&  sp_size > 0) {
        for (i=0; i<sp_size; i++) {
            sptr = sp_data + i;
            if (sptr->deleted_flag == 1  ||
                sptr->scaleable == 0  ||
                sptr->frame_num != frame_num) {
                continue;
            }
            CalcSymbBounds (i, &x1, &yt1, &x2, &y2);
            if (x1 < xmin) xmin = x1;
            if (yt1 < ymin) ymin = yt1;
            if (x2 > xmax) xmax = x2;
            if (y2 > ymax) ymax = y2;
        }
    }

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data != NULL  &&  hp_size > 0) {
        for (i=0; i<hp_size; i++) {
            shptr = hp_data + i;
            if (shptr->deleted_flag == 1  ||
                shptr->scaleable == 0  ||
                shptr->frame_num != frame_num) {
                continue;
            }
            CalcShapeBounds (i, &x1, &yt1, &x2, &y2);
            if (x1 < xmin) xmin = x1;
            if (yt1 < ymin) ymin = yt1;
            if (x2 > xmax) xmax = x2;
            if (y2 > ymax) ymax = y2;
        }
    }

    int           ip_size = (int)image_prim_list.size();
    IMagePrim     *ip_data = image_prim_list.data();

    if (ip_data != NULL  &&  ip_size > 0) {
        for (i=0; i<ip_size; i++) {
            imptr = ip_data + i;
            if (imptr->deleted_flag == 1  ||
                imptr->scaleable == 0  ||
                imptr->frame_num != frame_num) {
                continue;
            }
            if (imptr->xmin < xmin) xmin = xmin;
            if (imptr->ymin < ymin) ymin = ymin;
            if (imptr->xmax > xmax) xmax = xmax;
            if (imptr->ymax > ymax) ymax = ymax;
        }
    }

    int           ap_size = (int)axis_prim_list.size();
    AXisPrim      *ap_data = axis_prim_list.data();

    if (ap_data != NULL  &&  ap_size > 0) {
        for (i=0; i<ap_size; i++) {
            aptr = ap_data + i;
            if (aptr->frame_num != frame_num) {
                continue;
            }
            CalcAxisBounds (aptr,
                            &x1, &yt1, &x2, &y2);
            if (x1 < xmin) xmin = x1;
            if (yt1 < ymin) ymin = yt1;
            if (x2 > xmax) xmax = x2;
            if (y2 > ymax) ymax = y2;
        }
    }

    int        sf_size = surf_list.size();
    DLSurf     **sf_data = surf_list.data();

    if (sf_data != NULL  &&  sf_size > 0) {
        for (i=0; i<sf_size; i++) {
            surf = sf_data[i];
            if (surf->deleted_flag != 0  ||
                surf->frame_num != frame_num) {
                continue;
            }
            if (surf->gxmin < xmin) xmin = surf->gxmin;
            if (surf->gymin < ymin) ymin = surf->gymin;
            if (surf->gxmax > xmax) xmax = surf->gxmax;
            if (surf->gymax > ymax) ymax = surf->gymax;
        }
    }

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data != NULL  &&  cl_size > 0) {
        for (i=0; i<cl_size; i++) {
            cont = cl_data[i];
            if (cont->deleted_flag != 0  ||
                cont->frame_num != frame_num) {
                continue;
            }
            cont->CalcBounds (&cx1, &cy1, &cx2, &cy2);
            if (cx1 < xmin) xmin = cx1;
            if (cy1 < ymin) ymin = cy1;
            if (cx2 > xmax) xmax = cx2;
            if (cy2 > ymax) ymax = cy2;
        }
    }

    *xmin_out = xmin;
    *ymin_out = ymin;
    *xmax_out = xmax;
    *ymax_out = ymax;

    return;
}



int CDisplayList::PanFrame (int frame_num,
                            int ix1,
                            int iy1,
                            int ix2,
                            int iy2)
{
    FRameStruct       *frptr;
    CSW_F             px1sav, py1sav, px2sav, py2sav;
    CSW_F             xt1, yt1, xt2, yt2;
    double            fx1, fy1, fx2, fy2;
    double            fx1sav, fy1sav, fx2sav, fy2sav;
    double            dx, dy;
    int               framesav;

    if (frame_num < 0  ||  frame_num >= num_frame_list  ||  frame_list == NULL) {
        return -1;
    }

    frptr = frame_list + frame_num;

    px1sav = Px1;
    py1sav = Py1;
    px2sav = Px2;
    py2sav = Py2;
    fx1sav = Fx1;
    fy1sav = Fy1;
    fx2sav = Fx2;
    fy2sav = Fy2;
    framesav = current_frame_num;

    Px1 = frptr->px1;
    Py1 = frptr->py1;
    Px2 = frptr->px2;
    Py2 = frptr->py2;
    Fx1 = (CSW_F)frptr->x1;
    Fy1 = (CSW_F)frptr->y1;
    Fx2 = (CSW_F)frptr->x2;
    Fy2 = (CSW_F)frptr->y2;
    current_frame_num = frame_num;

    backscalef ((CSW_F)ix1, (CSW_F)iy1, &xt1, &yt1);
    backscalef ((CSW_F)ix2, (CSW_F)iy2, &xt2, &yt2);

    unconvert_frame_point (&xt1, &yt1);
    unconvert_frame_point (&xt2, &yt2);

    dx = xt1 - xt2;
    dy = yt1 - yt2;

    fx1 = frptr->x1 + dx;
    fy1 = frptr->y1 + dy;
    fx2 = frptr->x2 + dx;
    fy2 = frptr->y2 + dy;

    Px1 = px1sav;
    Py1 = py1sav;
    Px2 = px2sav;
    Py2 = py2sav;
    Fx1 = (CSW_F)fx1sav;
    Fy1 = (CSW_F)fy1sav;
    Fy2 = (CSW_F)fx2sav;
    Fy2 = (CSW_F)fy2sav;
    current_frame_num = framesav;

    RescaleFrame (frptr->name,
                  0,
                  fx1, fy1, fx2, fy2);

    return 1;

}

int CDisplayList::AddContour (
    COntourOutputRec *crec,
    int    grid_index,
    int    image_id)
{
    DLContour *dlc;
    DLSurf    *grid;
    int       nextprim;

    if (grid_index < 0) {
        return 0;
    }

    grid = NULL;

    int        sf_size = surf_list.size();
    DLSurf     **sf_data = surf_list.data();

    if (sf_data == NULL  ||  sf_size < 1) {
        return -1;
    }
    grid = sf_data[grid_index];
    if (grid == NULL) {
        return -1;
    }

    try {
        SNF;
        dlc = new DLContour ();
    }
    catch (...) {
        dlc = NULL;
        return -1;
    }

    dlc->SetCrec (crec);
    dlc->grid_index = grid_index;
    dlc->image_id = image_id;
    if (grid) {
        dlc->frame_num = grid->frame_num;
        dlc->layer_num = grid->layer_num;
    }

    nextprim = get_available_contour ();
    if (nextprim < 0) {

        try {
            contour_list.push_back (dlc);
        }
        catch (...) {
            return -1;
        }
        nextprim = contour_list.size() - 1;
    }
    else {
        contour_list[nextprim] = dlc;
    }

    dlc->prim_num = nextprim;

    dlc = NULL;

    if (current_selectable_object) {
        current_selectable_object->AddContour (nextprim);
    }

    return 1;
}


void CDisplayList::delete_surf_contours (int snum)
{
    int   i;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    DLContour     *dlc;
    for (i=0; i<cl_size; i++) {
        if (cl_data[i]->grid_index == snum) {
            dlc = cl_data[i];
            if (dlc != NULL) {
                delete (dlc);
            }
            cl_data[i] = NULL;
            add_available_contour (i);
        }
    }
}



int CDisplayList::AddGrid (
    char   *name,
    double *data,
    int    ncol,
    int    nrow,
    double xmin,
    double ymin,
    double width,
    double height,
    double angle)
{
    DLSurf    *dlg;
    int       i;
    double    dlow[1000], dhigh[1000];

    if (ncol < 2  ||  nrow < 2  ||
        width <= 0.0  ||  height <= 0.0) {
        return -1;
    }

    try {
        SNF;
        dlg = new DLSurf ();
    }
    catch (...) {
        dlg = NULL;
        return -1;
    }

    try {
        surf_list.push_back (dlg);
    }
    catch (...) {
        return -1;
    }

    int        sf_size = surf_list.size();

    dlg->index_num = sf_size - 1;
    strncpy (dlg->name, name, 99);
    dlg->name[99] = '\0';
    dlg->frame_num = current_frame_num;
    dlg->layer_num = current_layer_num;

    dlg->SetContourProperties (&tmp_contour_props);

    memset (dlow, 0, 1000 * sizeof(double));
    memset (dhigh, 0, 1000 * sizeof(double));
    int  num_hi_low = current_number_image_values;
    if (num_hi_low > 1000) num_hi_low = 1000;
    for (i=0; i<num_hi_low; i++) {
        dlow[i] = current_image_low_values[i];
        dhigh[i] = current_image_high_values[i];
    }

    dlg->SetImageBands (
        dlow,
        dhigh,
        current_image_red_values,
        current_image_green_values,
        current_image_blue_values,
        current_image_transparency_values,
        num_hi_low
    );

    dlg->SetGridData (data,
                  ncol,
                  nrow,
                  xmin,
                  ymin,
                  width,
                  height,
                  angle,
                  tmp_contour_faults,
                  num_tmp_contour_faults);

    dlg->SetSurfaceID (sf_size);

    return 1;
}


int CDisplayList::SetTmpFaultLines (int nlines,
                                    int ntot,
                                    int *npts,
                                    double *x,
                                    double *y,
                                    double *z)
{
    int              istat;

    ntot = ntot;

    grdapi_ptr->grd_FreeFaultLineStructs (tmp_contour_faults,
                              num_tmp_contour_faults);
    tmp_contour_faults = NULL;
    num_tmp_contour_faults = 0;

    istat =
    grdapi_ptr->grd_DoubleFaultArraysToStructs (
        x,
        y,
        z,
        npts,
        NULL,
        nlines,
        &tmp_contour_faults,
        &num_tmp_contour_faults);

    return istat;
}


int CDisplayList::SetTmpContourProperties (int *idata, double *ddata)
{
    DLContourProperties     *p;

    default_contour_properties ();

    if (idata == NULL  ||  ddata == NULL) {
        return 0;
    }

    p = &tmp_contour_props;

    p->showContours = idata[0];
    p->showColorFills = idata[1];
    p->showNodes = idata[2];
    p->showNodeValues = idata[3];
    p->showCellEdges = idata[4];
    p->showFaultLines = idata[5];
    p->majorInterval = idata[6];
    p->tickMajor = idata[7];
    p->tickMinor = idata[8];
    p->redMajor = idata[9];
    p->greenMajor = idata[10];
    p->blueMajor = idata[11];
    p->alphaMajor = idata[12];
    p->redMinor = idata[13];
    p->greenMinor = idata[14];
    p->blueMinor = idata[15];
    p->alphaMinor = idata[16];
    p->redNode = idata[17];
    p->greenNode = idata[18];
    p->blueNode = idata[19];
    p->alphaNode = idata[20];
    p->redNodeValue = idata[21];
    p->greenNodeValue = idata[22];
    p->blueNodeValue = idata[23];
    p->alphaNodeValue = idata[24];
    p->redCellEdge = idata[25];
    p->greenCellEdge = idata[26];
    p->blueCellEdge = idata[27];
    p->alphaCellEdge = idata[28];
    p->redFaultLine = idata[29];
    p->greenFaultLine = idata[30];
    p->blueFaultLine = idata[31];
    p->alphaFaultLine = idata[32];
    p->contourFont = idata[33];
    p->nodeFont = idata[34];
    p->nodeSymbol = idata[35];
    p->labelMajor = idata[36];
    p->labelMinor = idata[37];
    p->isThickness = idata[38];
    p->showOutline = idata[39];
    p->redOutline = idata[40];
    p->greenOutline = idata[41];
    p->blueOutline = idata[42];
    p->alphaOutline = idata[43];
    p->contourSmoothing = idata[44];

    p->contourMinValue = ddata[0];
    p->contourMaxValue = ddata[1];
    p->contourInterval = ddata[2];
    p->dataLogBase = ddata[3];
    p->fillMinValue = ddata[4];
    p->fillMaxValue = ddata[5];
    p->minorThickness = ddata[6];
    p->majorThickness = ddata[7];
    p->cellEdgeThickness = ddata[8];
    p->faultThickness = ddata[9];
    p->minorLabelSize = ddata[10];
    p->majorLabelSize = ddata[11];
    p->minorLabelSpacing = ddata[12];
    p->majorLabelSpacing = ddata[13];
    p->minorTickLength = ddata[14];
    p->majorTickLength = ddata[15];
    p->minorTickSpacing = ddata[16];
    p->majorTickSpacing = ddata[17];
    p->nodeSymbolSize = ddata[18];
    p->nodeValueSize = ddata[19];
    p->hardMin = ddata[20];
    p->hardMax = ddata[21];
    p->zUnitsConversionFactor = ddata[22];

    return 1;
}

void CDisplayList::default_contour_properties (void)
{
    DLContourProperties     *p;

    p = &tmp_contour_props;

    p->showContours = 1;
    p->showColorFills = 1;
    p->showNodes = 0;
    p->showNodeValues = 0;
    p->showCellEdges = 0;
    p->showFaultLines = 1;
    p->majorInterval = -1;
    p->tickMajor = 0;
    p->tickMinor = 0;
    p->redMajor = 0;
    p->greenMajor = 0;
    p->blueMajor = 0;
    p->alphaMajor = 0;
    p->redMinor = 0;
    p->greenMinor = 0;
    p->blueMinor = 0;
    p->alphaMinor = 0;
    p->redNode = 0;
    p->greenNode = 0;
    p->blueNode = 0;
    p->alphaNode = 0;
    p->redNodeValue = 0;
    p->greenNodeValue = 0;
    p->blueNodeValue = 0;
    p->alphaNodeValue = 0;
    p->redNodeValue = 0;
    p->greenNodeValue = 0;
    p->blueNodeValue = 0;
    p->alphaNodeValue = 0;
    p->redCellEdge = 0;
    p->greenCellEdge = 0;
    p->blueCellEdge = 0;
    p->alphaCellEdge = 0;
    p->redFaultLine = 0;
    p->greenFaultLine = 0;
    p->blueFaultLine = 0;
    p->alphaFaultLine = 0;
    p->contourFont = 102;
    p->nodeFont = 102;
    p->nodeSymbol = 7;
    p->labelMajor = 1;
    p->labelMinor = 0;
    p->isThickness = 0;
    p->showOutline = 0;
    p->redOutline = 0;
    p->greenOutline = 0;
    p->blueOutline = 0;
    p->alphaOutline = 0;
    p->contourSmoothing = 3;

    p->contourMinValue = 1.e30;
    p->contourMaxValue = -1.e30;
    p->contourInterval = -1.0;
    p->dataLogBase = 0.0;
    p->fillMinValue = 1.e30;
    p->fillMaxValue = -1.e30;
    p->minorThickness = .005;
    p->majorThickness = .01;
    p->cellEdgeThickness = .005;
    p->faultThickness = .015;
    p->minorLabelSize = .07;
    p->majorLabelSize = .10;
    p->minorLabelSpacing = 6.0;
    p->majorLabelSpacing = 6.0;
    p->minorTickLength = 0.05;
    p->majorTickLength = .07;
    p->minorTickSpacing = .15;
    p->majorTickSpacing = .20;
    p->nodeSymbolSize = .05;
    p->nodeValueSize = .07;
    p->hardMin = -1.e30;
    p->hardMax = 1.e30;
    p->zUnitsConversionFactor = 1.0;

    return;
}


void CDisplayList::SetGridNum (int ival)
{
    current_grid_num = ival;
}



void CDisplayList::recalc_surfaces (void)
{
    DLSurf          *grid;
    int             i, fnum;

    save_current_graphic_attributes ();

    int        sf_size = surf_list.size();
    DLSurf     **sf_data = surf_list.data();

    if (sf_data != NULL  &&  sf_size > 0) {
        for (i=0; i<sf_size; i++) {
            grid = sf_data[i];
            if (grid == NULL) {
                continue;
            }
            if (grid->deleted_flag == 1) {
                continue;
            }
            if (grid->needs_recalc == 1) {
                delete_surf_contours (i);
                delete_surf_contour_lines (i);
                delete_grid_prims (i);
                grid->CalcContours ((void *)this, i);
                grid->CalcImage ((void *)this);
                grid->CalcNodes ((void *)this);
                grid->CalcEdges ((void *)this);
                grid->CalcFaultLines ((void *)this);
                grid->needs_recalc = 0;
                grid->needs_reclip = 0;
                grid->needs_contour_reclip = 1;
                if (frame_list != NULL) {
                    fnum = grid->frame_num;
                    if (fnum >= 0  &&  fnum < num_frame_list) {
                        frame_list[fnum].needs_surf_prims = 1;
                        frame_layout_needed = 1;
                    }
                }
            }
        }
    }

    unsave_current_graphic_attributes ();

    return;
}




void CDisplayList::delete_grid_prims (int gridnum)
{
    delete_grid_contours (gridnum);
    delete_grid_lines (gridnum);
    delete_grid_texts (gridnum);
    delete_grid_symbs (gridnum);
    delete_grid_images (gridnum);
}

void CDisplayList::delete_grid_contours (int gridnum)
{
    int            i;
    DLContour      *cp1;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return;
    }

    for (i=0; i<cl_size; i++) {

        cp1 = cl_data[i];

        if (cp1 == NULL) {
            continue;
        }

        if (cp1->grid_index != gridnum) {
            continue;
        }

        delete (cp1);
        cl_data[i] = NULL;
        add_available_contour (i);

    }

    return;

}  /*  end of function delete_grid_contours */


void CDisplayList::delete_grid_lines (int gridnum)
{
    int            i;
    LInePrim       *lptr;

    int            lp_size = (int)line_prim_list.size();
    LInePrim       *lp_data = line_prim_list.data();

    if (lp_data == NULL  ||  lp_size < 1) {
        return;
    }

    for (i=0; i<lp_size; i++) {

        lptr = lp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }

        if (lptr->grid_num != gridnum) {
            continue;
        }

    }

    return;

}  /*  end of function delete_grid_lines */



void CDisplayList::delete_grid_texts (int gridnum)
{
    int            i;
    TExtPrim       *tptr;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }

    for (i=0; i<tp_size; i++) {

        tptr = tp_data + i;

        if (tptr->deleted_flag == 1) {
            continue;
        }

        if (tptr->grid_num != gridnum) {
            continue;
        }

    }

    return;

}  /*  end of function delete_grid_texts */



void CDisplayList::delete_grid_symbs (int gridnum)
{
    int            i;
    SYmbPrim       *sptr;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return;
    }

    for (i=0; i<sp_size; i++) {

        sptr = sp_data + i;

        if (sptr->deleted_flag == 1) {
            continue;
        }

        if (sptr->grid_num != gridnum) {
            continue;
        }

    }

    return;

}  /*  end of function delete_grid_symbs */



void CDisplayList::delete_grid_images (int gridnum)
{
    int            i;
    IMagePrim      *imptr;

    int            ip_size = (int)image_prim_list.size();
    IMagePrim      *ip_data = image_prim_list.data();

    if (ip_data == NULL  ||  ip_size < 1) {
        return;
    }

    for (i=0; i<ip_size; i++) {

        imptr = ip_data + i;

        if (imptr->deleted_flag == 1) {
            continue;
        }

        if (imptr->grid_num != gridnum) {
            continue;
        }

        imptr->deleted_flag = 1;

        csw_Free (imptr->red_data);
        csw_Free (imptr->green_data);
        csw_Free (imptr->blue_data);
        csw_Free (imptr->transparency_data);
        imptr->red_data = NULL;
        imptr->green_data = NULL;
        imptr->blue_data = NULL;
        imptr->transparency_data = NULL;
        add_available_image (i);

    }

    return;

}  /*  end of function delete_grid_images */




void CDisplayList::reclip_frame_contours (int fnum)
{
    DLContour           *contour, *page_con;
    DLContourProperties *conprop;
    int                 grid_num;
    int                 nprim, ido, i, j, k, n, npts, istat, ncout, np2;
    int                 fsave;
    DLSurf              *grid;
    COntourDrawOptions const  *draw_options;
    COntourDrawOptions  local_options;
    COntourOutputRec    *crec, *crec2;

    CSW_F               *xpts, *ypts, *xypack, *xp;
    CSW_F               *xpts2, *ypts2;

    int                 layer_save,
                        grid_save;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return;
    }

    if (xywork == NULL  ||
        xywork2 == NULL  ||
        iwork == NULL) {
        return;
    }

    layer_save = current_layer_num;
    grid_save = current_grid_num;

/*
 * Set the contour drawing scale.  Since everything is converted to page units,
 * this just needs to have the same frame and page corners.
 */
    conapi_obj.con_SetDrawScale (0.0f, 0.0f, 100.0f, 100.0f,
                      0.0f, 0.0f, 100.0f, 100.0f);

    save_current_graphic_attributes ();

    fsave = current_frame_num;
    current_frame_num = fnum;
    update_frame_limits ();

    nprim = cl_size;

    int        sf_size = surf_list.size();
    DLSurf     **sf_data = surf_list.data();

    for (ido=0; ido<nprim; ido++) {

        i = ido;

        current_contour_index = i;

        contour = cl_data[i];
        if (contour == NULL) {
            continue;
        }

        if (contour->deleted_flag == 1) {
            continue;
        }

        if (contour->frame_num != fnum) {
            continue;
        }

        if (contour->grid_index < 0) {
            continue;
        }

        crec = contour->GetCrec ();
        if (crec == NULL) {
            continue;
        }

        grid_num = contour->grid_index;

        if (grid_num < 0  ||  sf_data == NULL  ||  sf_size < 1  ||
            grid_num >= sf_size) {
            continue;
        }

        draw_options = NULL;
        conprop = NULL;
        if (grid_num >= 0  &&  sf_data != NULL  &&  grid_num < sf_size) {
            grid = sf_data[grid_num];
            if (grid->needs_contour_reclip != 1) {
                continue;
            }
            grid->turn_off_reclip = 1;
            draw_options = grid->GetDrawOptions ();
            conprop = (DLContourProperties *)grid->GetContourProperties ();
        }

        if (draw_options == NULL  ||  conprop == NULL) {
            continue;
        }

        memcpy (&local_options, draw_options, sizeof(COntourDrawOptions));

        local_options.major_tick_spacing  *= page_units_per_inch;
        local_options.major_text_spacing  *= page_units_per_inch;
        local_options.major_tick_len  *= page_units_per_inch;
        local_options.major_text_size  *= page_units_per_inch;
        local_options.minor_tick_spacing  *= page_units_per_inch;
        local_options.minor_text_spacing  *= page_units_per_inch;
        local_options.minor_tick_len  *= page_units_per_inch;
        local_options.minor_text_size  *= page_units_per_inch;

        if (conprop->labelMinor == 0) {
            local_options.minor_text_size = -1.0f;
            local_options.minor_text_spacing = -1.0f;
        }
        if (conprop->labelMajor == 0) {
            local_options.major_text_size = -1.0f;
            local_options.major_text_spacing = -1.0f;
        }
        if (conprop->tickMinor == 0) {
            local_options.minor_tick_len = -1.0f;
            local_options.minor_tick_spacing = -1.0f;
        }
        if (conprop->tickMajor == 0) {
            local_options.major_tick_len = -1.0f;
            local_options.major_tick_spacing = -1.0f;
        }

        try {
            SNF;
            page_con = new DLContour ();
        }
        catch (...) {
            page_con = NULL;
            continue;
        }

        crec = contour->GetCrec ();
        if (crec == NULL) {
            continue;
        }

        npts = crec->npts;
        if (npts < 2) {
            continue;
        }

    /*
     * Convert the contour points to page coordinates.
     */
        xpts = (CSW_F *)csw_Malloc (npts * 6 * sizeof(CSW_F));
        if (xpts == NULL) {
            continue;
        }
        ypts = xpts + npts;
        xpts2 = ypts + npts;
        ypts2 = xpts2 + 2 * npts;

        memcpy (xpts, crec->x, npts * sizeof(CSW_F));
        memcpy (ypts, crec->y, npts * sizeof(CSW_F));

        crec2 = (COntourOutputRec *)csw_Malloc (sizeof(COntourOutputRec));
        if (crec2 == NULL) {
            csw_Free (xpts);
            xpts = NULL;
            ypts = NULL;
            continue;
        }

        memcpy (crec2, crec, sizeof (COntourOutputRec));

        xypack = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
        if (xypack == NULL) {
            csw_Free (xpts);
            xpts = NULL;
            ypts = NULL;
            xpts2 = NULL;
            ypts2 = NULL;
            csw_Free (crec2);
            crec2 = NULL;
            continue;
        }
        istat = gpf_packxy2 (xpts, ypts, npts,
                             xypack);
        if (istat != 1) {
            csw_Free (xpts);
            xpts = NULL;
            ypts = NULL;
            csw_Free (xypack);
            xypack = NULL;
            xpts2 = NULL;
            ypts2 = NULL;
            csw_Free (crec2);
            crec2 = NULL;
            continue;
        }

        page_con->frame_num = contour->frame_num;
        page_con->layer_num = contour->layer_num;
        page_con->grid_index = contour->grid_index;
        page_con->image_id = contour->image_id;

    /*
     * Clip the contour to the page frame corners.
     */
        gpf_calcdraw_obj.gpf_cliplineprim (xypack, npts,
                          Fx1, Fy1, Fx2, Fy2,
                          xywork2, &ncout, iwork);
        csw_Free (xypack);
        xypack = NULL;

    /*
     * Draw each clipped contour.
     */
        n = 0;
        for (j=0; j<ncout; j++) {

            xp = xywork2 + n;
            np2 = iwork[j];
            for (k=0; k<np2; k++) {
                xpts2[k] = *xp;
                xp++;
                n++;
                ypts2[k] = *xp;
                xp++;
                n++;
            }

            crec2->x = xpts2;
            crec2->y = ypts2;
            crec2->npts = np2;

            page_con->SetCrec (crec2);
            page_con->drawContour (&local_options,
                                   conprop,
                                   (void *)this);
        }

        csw_Free (xpts);
        xpts = NULL;
        ypts = NULL;
        xpts2 = NULL;
        ypts2 = NULL;
        delete (page_con);
        page_con = NULL;
        csw_Free (crec2);
        crec2 = NULL;

    }

    if (sf_data != NULL  &&  sf_size > 0) {
        for (i=0; i<sf_size; i++) {
            DLSurf  *dls = sf_data[i];
            if (dls->turn_off_reclip == 1) {
                dls->needs_contour_reclip = 0;
                dls->turn_off_reclip = 0;
            }
        }
    }

    current_contour_index = -1;

    unsave_current_graphic_attributes ();

    current_layer_num = layer_save;
    current_grid_num = grid_save;

    current_frame_num = fsave;
    update_frame_limits ();

    return;

}  /*  end of function reclip_frame_contours */



/*
 *  Add the line primitives, including label gaps and ticks for contours.
 *  These are usually calculated once for each contout and put into the
 *  display list for drawing.
 */
int CDisplayList::AddContourLine (CSW_F *xpts_in, CSW_F *ypts_in, int npts)
{
    LInePrim       *lptr = NULL;
    int            istat;
    CSW_F          *xypack = NULL;
    int            next_line;

    bool           bsuccess = false;

    auto fscope = [&]()
    {
        if (!bsuccess) {
            csw_Free (xypack);
        }
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
 * check obvious errors in parameters.
 */
    if (npts < 2) {
        return 0;
    }
    if (xpts_in == NULL  ||  ypts_in == NULL) {
        return 0;
    }

    if (npts > _DL_MAX_WORK_) npts = _DL_MAX_WORK_;

    next_line = get_available_contour_line ();

    if (next_line < 0) {
        try {
            SNF;
            LInePrim  cpr;
            ZeroInit (&cpr, sizeof(cpr));
            contour_line_prim_list.push_back (cpr);
            next_line = (int)contour_line_prim_list.size() - 1;
        }
        catch (...) {
            printf ("Exception in contour_line_prim_list pushback\n");
            return -1;
        }
    }

/*
 * Pack points into the line structure format
 */
    xypack = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
    if (xypack == NULL) {
        return -1;
    }
    istat = gpf_packxy2 (xpts_in, ypts_in, npts,
                         xypack);
    if (istat != 1) {
        return -1;
    }

/*
 * Populate the line prim structure.
 */
    LInePrim      *cp_data = contour_line_prim_list.data();

    lptr = cp_data + next_line;

    lptr->xypts = xypack;
    xypack = NULL;
    lptr->npts = npts;
    lptr->maxpts = npts;

    lptr->thick = current_line_thickness;
    lptr->dashpat = current_line_pattern;
    lptr->dashscale = current_line_pattern_size;
    lptr->symbol = current_line_symbol;

    lptr->red = current_line_red;
    lptr->green = current_line_green;
    lptr->blue = current_line_blue;
    lptr->alpha = current_line_alpha;

    lptr->grid_num = current_grid_num;
    lptr->border_num = current_border_num;
    lptr->axis_num = current_axis_num;
    lptr->frame_num = current_frame_num;
    lptr->graph_num = current_graph_num;
    lptr->selectable_object_num = current_selectable_object_num;
    lptr->xaxis_num = current_xaxis_num;
    lptr->yaxis_num = current_yaxis_num;

    lptr->layer_num = current_layer_num;
    lptr->item_num = current_item_num;
    lptr->arrowstyle = current_line_arrow_style;

    lptr->smooth_flag = (char)current_line_smooth_flag;
    lptr->editable_flag = (char)current_editable_flag;
    lptr->selectable_flag = (char)current_selectable_flag;

    lptr->visible_flag = 1;
    lptr->draw_flag = 1;
    lptr->plot_flag = 1;
    lptr->selected_flag = 0;
    lptr->deleted_flag = 0;

    lptr->prim_num = next_line;
    lptr->scaleable = 0;

    lptr->image_id = ImageID;
    lptr->contour_index = current_contour_index;

    CalcLineBounds (lptr);

    bsuccess = true;

    return 1;

}  /*  end of function AddContourLine  */



double CDisplayList::GetFrameUnitsPerPixel (int fnum)
{
    double         rval;
    double         dx, dy;
    FRameStruct    *fp;

    rval = 1.e30;
    if (fnum < 0  ||  fnum >= num_frame_list) {
        return rval;
    }
    if (screen_dpi <= 0) {
        return rval;
    }
    if (frame_list == NULL) {
        return rval;
    }

    fp = frame_list + fnum;

    dx = fp->x2 - fp->x1;
    if (dx < 0.0) dx = -dx;
    dy = fp->y2 - fp->y1;
    if (dy < 0.0) dy = -dy;

    if (fp->px2 - fp->px1 <= 0.0  ||
        fp->py2 - fp->py1 <= 0.0) {
        return rval;
    }

    dx /= (fp->px2 - fp->px1);
    dy /= (fp->py2 - fp->py1);

    dx *= page_units_per_inch;
    dy *= page_units_per_inch;

    dx = (dx + dy) / 2.0;

    dx /= screen_dpi;

    return dx;

}



void CDisplayList::GetFrameClipLimits (
         int fnum,
         double *fx1, double *fy1,
         double *fx2, double *fy2)
{
    FRameStruct    *fp;

    *fx1 = 1.e30;
    *fy1 = 1.e30;
    *fx2 = 1.e30;
    *fy2 = 1.e30;

    if (frame_list == NULL) {
        return;
    }

    if (fnum < 0  ||  fnum >= num_frame_list) {
        return;
    }

    fp = frame_list + fnum;
    *fx1 = fp->x1;
    *fy1 = fp->y1;
    *fx2 = fp->x2;
    *fy2 = fp->y2;

    return;

}

/*
    Add an image based on an array of float values, using the current
    image value arrays from the most recent call to SetImageColors and
    also the options from SetImageOptions.  The name is the most recent
    name from SetImageName.

    This does not set up a "master" image for rescaling.
*/

/*
 * This version is deprecated, but it can still be called.  It calls
 * the modern version with an image id of -1 .
 */
int CDisplayList::AddGridImage (int fnum,
                                float *data,
                                int ncol,
                                int nrow,
                                float null_value,
                                double xmin,
                                double ymin,
                                double xmax,
                                double ymax)
{
    int   istat;

    istat =
      AddGridImage (
        fnum, -1, data, ncol, nrow,
        null_value, xmin, ymin, xmax, ymax);

    return istat;
}

/*
 * This version is needed to tie together images and vector
 * primitives tightly bound to the image.  The image id is used here
 * and for the vector primitives.
 */
int CDisplayList::AddGridImage (int fnum,
                                int image_id,
                                float *data,
                                int ncol,
                                int nrow,
                                float null_value,
                                double xmin,
                                double ymin,
                                double xmax,
                                double ymax)
{
    int            istat, i, j, k, offset, ntot;
    unsigned char  *red_ptr, *green_ptr, *blue_ptr, *trans_ptr;
    int            red, green, blue, trans;

    auto fscope = [&]()
    {
        csw_Free (red_ptr);
        csw_Free (green_ptr);
        csw_Free (blue_ptr);
        csw_Free (trans_ptr);
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (xmax <= xmin || ymax <= ymin ||
        data == NULL || ncol < 2  ||  nrow < 2) {
        return 0;
    }

/*
 * Allocate space for color image data.
 */
    ntot = ncol * nrow;
    red_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    green_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    blue_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    trans_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    if (red_ptr == NULL  ||  green_ptr == NULL  ||  blue_ptr == NULL  ||
        trans_ptr == NULL) {
        return -1;
    }

/*
 * convert data image to color image.
 */
    current_image_null_value = (double)null_value;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            lookup_image_color ((double)data[k],
                                &red, &green, &blue, &trans);
            red_ptr[k] = (unsigned char)red;
            green_ptr[k] = (unsigned char)green;
            blue_ptr[k] = (unsigned char)blue;
            trans_ptr[k] = (unsigned char)trans;
        }
    }

    istat = AddGridColorImage (fnum,
                           image_id,
                           red_ptr,
                           green_ptr,
                           blue_ptr,
                           trans_ptr,
                           ncol,
                           nrow,
                           xmin,
                           ymin,
                           xmax,
                           ymax);

    return istat;

}  /*  end of function AddGridDataImage  (float) */


/*
 Add a color image for a grid or trimesh.  The color data are not
 copied here.  The color image takes over ownership of these data.
 Do not csw_Free these from the calling function.
*/
int CDisplayList::AddGridColorImage (int fnum,
                                 unsigned char *red,
                                 unsigned char *green,
                                 unsigned char *blue,
                                 unsigned char *trans,
                                 int ncol,
                                 int nrow,
                                 double xmin,
                                 double ymin,
                                 double xmax,
                                 double ymax)
{
    int       istat;

    istat =
      AddGridColorImage (fnum,
                         -1,
                         red, green, blue, trans,
                         ncol, nrow,
                         xmin, ymin, xmax, ymax);

    return istat;
}


int CDisplayList::AddGridColorImage (int fnum,
                                 int image_id,
                                 unsigned char *red,
                                 unsigned char *green,
                                 unsigned char *blue,
                                 unsigned char *trans,
                                 int ncol,
                                 int nrow,
                                 double xmin,
                                 double ymin,
                                 double xmax,
                                 double ymax)
{
    IMagePrim      *imptr = NULL;
    int            i, j, k, k2, offset;
    unsigned char  *red_ptr = NULL, *green_ptr = NULL,
                   *blue_ptr = NULL, *trans_ptr = NULL;
    int            next_image;

    bool           bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (red_ptr);
            csw_Free (green_ptr);
            csw_Free (blue_ptr);
            csw_Free (trans_ptr);
        }
    };
    CSWScopeGuard  func_scope_guard (fscope);


    if (xmax <= xmin || ymax <= ymin ||
        red == NULL   ||  green == NULL  ||  blue == NULL  ||  trans == NULL  ||
        ncol < 2  ||  nrow < 2) {
        return -1;
    }

    next_image = get_available_image ();
    if (next_image < 0) {
        try {
            SNF;
            IMagePrim  ipr;
            ZeroInit (&ipr, sizeof(ipr));
            image_prim_list.push_back (ipr);
            next_image = (int)image_prim_list.size() - 1;
        }
        catch (...) {
            printf ("Exception in image_prim_list pushback\n");
            return -1;
        }
    }

/*
 * Allocate space for color image data in the display list.
 */
    int  ntot = ncol * nrow;

    red_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    green_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    blue_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    trans_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    if (red_ptr == NULL  ||  green_ptr == NULL  ||
        blue_ptr == NULL  ||  trans_ptr == NULL) {
        return -1;
    }

/*
 * convert to clipped color image.
 */
    k2 = 0;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            red_ptr[k2] = red[k];
            green_ptr[k2] = green[k];
            blue_ptr[k2] = blue[k];
            trans_ptr[k2] = trans[k];
            k2++;
        }
    }

/*
 * Populate the image prim structure.
 */
    IMagePrim      *ip_data = image_prim_list.data();

    imptr = ip_data + next_image;

    strncpy (imptr->name, current_image_name, 99);
    imptr->name[99] = '\0';
    imptr->image_id = current_image_id;

    imptr->red_data = red_ptr;
    imptr->green_data = green_ptr;
    imptr->blue_data = blue_ptr;
    imptr->transparency_data = trans_ptr;

    imptr->ncol = ncol;
    imptr->nrow = nrow;

    imptr->xmin = xmin;
    imptr->ymin = ymin;
    imptr->xmax = xmax;
    imptr->ymax = ymax;

    imptr->frame_num = current_frame_num;
    imptr->selectable_object_num = current_selectable_object_num;
    imptr->graph_num = current_graph_num;
    imptr->xaxis_num = current_xaxis_num;
    imptr->yaxis_num = current_yaxis_num;

    imptr->layer_num = current_layer_num;
    imptr->item_num = current_item_num;

    imptr->editable_flag = (char)current_editable_flag;
    imptr->selectable_flag = (char)current_selectable_flag;

    imptr->visible_flag = 1;
    imptr->draw_flag = 1;
    imptr->plot_flag = 1;
    imptr->selected_flag = 0;
    imptr->deleted_flag = 0;

    imptr->prim_num = next_image;
    imptr->scaleable = 0;

    imptr->image_id = image_id;
    imptr->has_lines = 0;
    if (image_id >= 0) imptr->has_lines = 1;

    update_frame_limits ();

    bsuccess = true;

    return 1;

}  /*  end of function AddGridColorImage  */




void CDisplayList::reclip_frame_grid_images (int fnum)

{
    int             i;
    DLSurf          *grid;

    int        sf_size = surf_list.size();
    DLSurf     **sf_data = surf_list.data();

    if (sf_data == NULL  ||  sf_size < 1) {
        return;
    }

    save_current_graphic_attributes ();

    for (i=0; i<sf_size; i++) {

        grid = sf_data[i];
        if (grid == NULL) {
            continue;
        }

        if (grid->deleted_flag == 1) {
            continue;
        }

        if (grid->frame_num != fnum) {
            continue;
        }

        if (grid->needs_reclip == 0) {
            grid->needs_reclip = 1;
            continue;
        }

        grid->CalcImage ((void *)this);

    }

    unsave_current_graphic_attributes ();

    return;

}  /*  end of function reclip_frame_grid_images */



int CDisplayList::AddTriMesh (
    char   *name,
    double *xnode,
    double *ynode,
    double *znode,
    int *nodeflag,
    int numnode,
    int *n1edge,
    int *n2edge,
    int *t1edge,
    int *t2edge,
    int *edgeflag,
    int numedge,
    int *e1tri,
    int *e2tri,
    int *e3tri,
    int *triflag,
    int numtri)
{
    DLSurf    *dlg;
    int       i;
    double    dlow[1000], dhigh[1000];

/*
 * Check obvious errors.  Note that the flag arrays can be
 * NULL.  They are optional.
 */
    if (xnode == NULL  ||
        ynode == NULL  ||
        znode == NULL  ||
        numnode < 3  ||
        n1edge == NULL  ||
        n2edge == NULL  ||
        t1edge == NULL  ||
        t2edge == NULL  ||
        numedge < 3  ||
        e1tri == NULL  ||
        e2tri == NULL  ||
        e3tri == NULL  ||
        numtri < 1)
    {
        return 0;
    }


    try {
        SNF;
        dlg = new DLSurf ();
        surf_list.push_back (dlg);
    }
    catch (...) {
        dlg = NULL;
        return -1;
    }

    int        sf_size = surf_list.size();

    dlg->index_num = sf_size - 1;
    strncpy (dlg->name, name, 99);
    dlg->name[99] = '\0';
    dlg->frame_num = current_frame_num;
    dlg->layer_num = current_layer_num;

    dlg->SetContourProperties (&tmp_contour_props);

    memset (dlow, 0, 1000 * sizeof(double));
    memset (dhigh, 0, 1000 * sizeof(double));
    int  num_hi_low = current_number_image_values;
    if (num_hi_low > 1000) num_hi_low = 1000;
    for (i=0; i<num_hi_low; i++) {
        dlow[i] = current_image_low_values[i];
        dhigh[i] = current_image_high_values[i];
    }

    dlg->SetImageBands (
        dlow,
        dhigh,
        current_image_red_values,
        current_image_green_values,
        current_image_blue_values,
        current_image_transparency_values,
        num_hi_low
    );

    dlg->SetTriMesh (
        xnode,
        ynode,
        znode,
        nodeflag,
        numnode,
        n1edge,
        n2edge,
        t1edge,
        t2edge,
        edgeflag,
        numedge,
        e1tri,
        e2tri,
        e3tri,
        triflag,
        numtri);

    dlg->SetSurfaceID (sf_size);

    return 1;
}




int CDisplayList::set_grid_for_contour_vec (double x1, double y1p, double x2, double y2,
                                            int contour_num)

{
    double            bint, slope, x, y, dx, tiny;
    int               row1, row2, col1, col2, itmp, offset, c1, c2, c2sav, i, j;
    int               *cell_list, num_cell, max_cell;

/*
    if there is a hole flag on either end of the vector,
    return with no processing
*/
    if (x1 > _DL_XY_TOO_BIG_  ||  x1 < -_DL_XY_TOO_BIG_  ||
        x2 > _DL_XY_TOO_BIG_  ||  x2 < -_DL_XY_TOO_BIG_  ||
        y1p > _DL_XY_TOO_BIG_  ||  y1p < -_DL_XY_TOO_BIG_  ||
        y2 > _DL_XY_TOO_BIG_  ||  y2 < -_DL_XY_TOO_BIG_) {
        return 1;
    }

/*
 * Do nothing if the contour spatial index is not set up yet.
 */
    if (contour_spatial_index == NULL) {
        return -1;
    }

/*  return if either end of the vector is outside of the grid limits  */;

    if (x2 < index_xmin  ||  x2 > index_xmax) {
        return (set_in_contour_extra (contour_num));
    }

    if (x1 < index_xmin  ||  x1 > index_xmax) {
        return (set_in_contour_extra (contour_num));
    }

    if (y2 < index_ymin  ||  y2 > index_ymax) {
        return (set_in_contour_extra (contour_num));
    }

    if (y1p < index_ymin  ||  y1p > index_ymax) {
        return (set_in_contour_extra (contour_num));
    }

/*  calculate slope and intercept if non vertical  */

    slope = 1.e30;
    bint = 1.e30;
    tiny = index_xspace / 100.0;
    dx = x2 - x1;
    if (dx < 0.0) dx = -dx;

    if (dx > tiny) {
        slope = (y1p-y2) / (x1-x2);
        bint = y1p - slope * x1;
    }

/*  grid start and end rows and columns  */

    row1 = (int)((y1p-index_ymin) / index_yspace);
    row2 = (int)((y2-index_ymin) / index_yspace);
    if (row1 > row2) {
        i = row1;
        row1 = row2;
        row2 = i;
    }
    col1 = (int)((x1-index_xmin) / index_xspace);
    col2 = (int)((x2-index_xmin) / index_xspace);
    if (col1 > col2) {
        i = col1;
        col1 = col2;
        col2 = i;
    }

/*
 * Each cell in the spatial index array has a list of contour
 * numbers that intersect the cell.  If the list is null, no contours have
 * yet intersected the cell.  The first element in each list is the
 * current maximum size of the list.  The second element is the current
 * total number of contour numbers in the list.
 */

/*  special case for near horizontal vector with endpoints in the same row  */

    if (row1 == row2 ) {

        if (row1 < 0  ||  row1 >= index_nrow) {
            return (set_in_contour_extra (contour_num));
        }

        if (col1 > col2) {
            itmp = col1;
            col1 = col2;
            col2 = itmp;
        }

        if (col2 < 0) {
            return (set_in_contour_extra (contour_num));
        }
        if (col1 >= index_ncol) {
            return (set_in_contour_extra (contour_num));
        }
        if (col1 < 0) {
            col1 = 0;
        }
        if (col2 >= index_ncol) {
            col2 = index_ncol-1;
        }

        offset = row1 * index_ncol;

        for (i=col1; i<=col2; i++) {

            cell_list = contour_spatial_index[offset+i];
            if (cell_list == NULL) {
                cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
                if (cell_list == NULL) {
                    return -1;
                }
                cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
                cell_list[1] = 0;
            }
            max_cell = cell_list[0];
            num_cell = cell_list[1];
            if (num_cell >= max_cell - 2) {
                max_cell += _SMALL_CHUNK_SIZE_;
                cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
                if (cell_list) {
                    cell_list[0] += _SMALL_CHUNK_SIZE_;
                }
            }
            if (cell_list == NULL) {
                return -1;
            }

            if (num_cell > 0  &&  cell_list[num_cell+1] == contour_num) {
                num_cell = num_cell;
            }
            else {
                cell_list[num_cell+2] = contour_num;
                cell_list[1]++;
            }

            contour_spatial_index[offset+i] = cell_list;

        }
    }

/*  special case for near vertical vector in only one column  */

    else if (col1 == col2) {

        if (row1 > row2) {
            itmp = row1;
            row1 = row2;
            row2 = itmp;
        }

        if (row2 < 0) {
            return (set_in_contour_extra (contour_num));
        }
        if (row1 >= index_nrow) {
            return (set_in_contour_extra (contour_num));
        }
        if (row1 < 0) {
            row1 = 0;
        }
        if (row2 >= index_nrow) {
            row2 = index_nrow-1;
        }

        for (i=row1; i<=row2; i++) {
            offset = i * index_ncol;
            cell_list = contour_spatial_index[offset+col1];
            if (cell_list == NULL) {
                cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
                if (cell_list == NULL) {
                    return -1;
                }
                cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
                cell_list[1] = 0;
            }
            max_cell = cell_list[0];
            num_cell = cell_list[1];
            if (num_cell >= max_cell - 2) {
                max_cell += _SMALL_CHUNK_SIZE_;
                cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
                if (cell_list) {
                    cell_list[0] += _SMALL_CHUNK_SIZE_;
                }
            }
            if (cell_list == NULL) {
                return -1;
            }

            if (num_cell > 0  &&  cell_list[num_cell+1] == contour_num) {
                num_cell = num_cell;
            }
            else {
                cell_list[num_cell+2] = contour_num;
                cell_list[1]++;
            }

            contour_spatial_index[offset+col1] = cell_list;

        }
    }

/*  general case where the vector crosses rows and columns  */

    else {

        if (row2 < 0) {
            return (set_in_contour_extra (contour_num));
        }
        if (row1 >= index_nrow) {
            return (set_in_contour_extra (contour_num));
        }

        y = y1p;
        if (y2 < y1p) y = y2;
        x = (y - bint) / slope;
        c1 = (int)((x - index_xmin) / index_xspace);

        if (row1 < 0) {
            row1 = 0;
        }
        if (row2 >= index_nrow) {
            row2 = index_nrow-1;
        }

        for (i=row1; i<=row2; i++) {
            y = index_ymin + (i+1) * index_yspace;
            if (i == row2) {
                y = y2;
                if (y1p > y2) y = y1p;
            }
            x = (y - bint) / slope;
            c2 = (int)((x - index_xmin) / index_xspace);
            c2sav = c2;
            if (c1 > c2) {
                itmp = c1;
                c1 = c2;
                c2 = itmp;
            }
            if (c1 < 0) {
                c1 = 0;
            }
            if (c2 >= index_ncol) {
                c2 = index_ncol-1;
            }

            offset = i * index_ncol;

            for (j=c1; j<=c2; j++) {
                cell_list = contour_spatial_index[offset+j];
                if (cell_list == NULL) {
                    cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
                    if (cell_list == NULL) {
                        return -1;
                    }
                    cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
                    cell_list[1] = 0;
                }
                max_cell = cell_list[0];
                num_cell = cell_list[1];
                if (num_cell >= max_cell - 2) {
                    max_cell += _SMALL_CHUNK_SIZE_;
                    cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
                    if (cell_list) {
                        cell_list[0] += _SMALL_CHUNK_SIZE_;
                    }
                }
                if (cell_list == NULL) {
                    return -1;
                }

                if (num_cell > 0  &&  cell_list[num_cell+1] == contour_num) {
                    num_cell = num_cell;
                }
                else {
                    cell_list[num_cell+2] = contour_num;
                    cell_list[1]++;
                }

                contour_spatial_index[offset+j] = cell_list;

            }
            c1 = c2sav;
        }
    }

    if (x1 < index_xmin) {
        return (set_in_contour_extra (contour_num));
    }

    if (x2 > index_xmax) {
        return (set_in_contour_extra (contour_num));
    }

    if (y1p < index_ymin) {
        return (set_in_contour_extra (contour_num));
    }

    if (y2 > index_ymax) {
        return (set_in_contour_extra (contour_num));
    }

    return 0 ;

}  /*  end of function set_grid_for_contour_vec  */






int CDisplayList::set_in_contour_extra (int contour_num)

{
    int                offset, in_extra;
    int                *cell_list, num_cell, max_cell;
    DLContour          *cptr;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return 0;
    }

    cptr = cl_data[contour_num];
    in_extra = cptr->GetInExtra ();
    if (in_extra == 1) {
        return 0;
    }

/*
 * Do nothing if the line spatial index is not set up yet.
 */
    if (contour_spatial_index == NULL) {
        return -1;
    }

    offset = index_ncol * index_nrow;

    cell_list = contour_spatial_index[offset];
    if (cell_list == NULL) {
        cell_list = (int *)csw_Malloc (_SMALL_CHUNK_SIZE_ * sizeof(int));
        if (cell_list == NULL) {
            return -1;
        }
        cell_list[0] = _SMALL_CHUNK_SIZE_ - 2;
        cell_list[1] = 0;
    }
    max_cell = cell_list[0];
    num_cell = cell_list[1];
    if (num_cell >= max_cell - 2) {
        max_cell += _SMALL_CHUNK_SIZE_;
        cell_list = (int *)csw_Realloc (cell_list, max_cell * sizeof(int));
        if (cell_list) {
            cell_list[0] += _SMALL_CHUNK_SIZE_;
        }
    }
    if (cell_list == NULL) {
        return -1;
    }

    cell_list[num_cell+2] = contour_num;
    cell_list[1]++;

    cptr->SetInExtra (1);

    contour_spatial_index[offset] = cell_list;

    return 0 ;

}  /*  end of private set_in_contour_extra function  */



/*
 *****************************************************************

             r e i n d e x _ p r i m i t i v e s

 *****************************************************************

*/

int CDisplayList::reindex_primitives (int fnum)
{
    ClearFrameSpatialIndex (fnum);
    reindex_lines (fnum);
    reindex_fills (fnum);
    reindex_texts (fnum);
    reindex_symbs (fnum);
    reindex_shapes (fnum);
    reindex_contours (fnum);

    return 1;
}

void CDisplayList::reindex_lines (int fnum)
{
    int            i;
    LInePrim       *lptr;

    int            lp_size = (int)line_prim_list.size();
    LInePrim       *lp_data = line_prim_list.data();

    if (lp_data == NULL  ||  lp_size < 1) {
        return;
    }

    for (i=0; i<lp_size; i++) {

        lptr = lp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }
        if (lptr->frame_num != fnum) {
            continue;
        }
        if (lptr->scaleable != 1) {
            continue;
        }

        SetSpatialIndexForLine (i);

    }

    return;

}  /*  end of function reindex_lines */



void CDisplayList::reindex_fills (int fnum)
{
    int            i;
    FIllPrim       *fptr;

    int            fp_size = (int)fill_prim_list.size();
    FIllPrim       *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    for (i=0; i<fp_size; i++) {

        fptr = fp_data + i;

        if (fptr->deleted_flag == 1) {
            continue;
        }
        if (fptr->frame_num != fnum) {
            continue;
        }
        if (fptr->scaleable != 1) {
            continue;
        }

        SetSpatialIndexForFill (i);

    }

    return;

}  /*  end of function reindex_fills */


void CDisplayList::reindex_texts (int fnum)
{
    int            i;
    TExtPrim       *tptr;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }

    for (i=0; i<tp_size; i++) {

        tptr = tp_data + i;

        if (tptr->deleted_flag == 1) {
            continue;
        }
        if (tptr->frame_num != fnum) {
            continue;
        }
        if (tptr->scaleable != 1) {
            continue;
        }

        SetSpatialIndexForText (i);

    }

    return;

}  /*  end of function reindex_texts */



void CDisplayList::reindex_symbs (int fnum)
{
    int            i;
    SYmbPrim       *sptr;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return;
    }

    for (i=0; i<sp_size; i++) {

        sptr = sp_data + i;

        if (sptr->deleted_flag == 1) {
            continue;
        }
        if (sptr->frame_num != fnum) {
            continue;
        }
        if (sptr->scaleable != 1) {
            continue;
        }

        SetSpatialIndexForSymb (i);

    }

    return;

}  /*  end of function reindex_symbs */

void CDisplayList::reindex_shapes (int fnum)
{
    int            i;
    SHapePrim      *shptr;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return;
    }

    for (i=0; i<hp_size; i++) {

        shptr = hp_data + i;

        if (shptr->deleted_flag == 1) {
            continue;
        }
        if (shptr->frame_num != fnum) {
            continue;
        }
        if (shptr->scaleable != 1) {
            continue;
        }

        SetSpatialIndexForShape (i);

    }

    return;

}  /*  end of function reindex_shapes */


void CDisplayList::reindex_contours (int fnum)
{
    int            i;
    DLContour      *cptr;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return;
    }

    for (i=0; i<cl_size; i++) {

        cptr = cl_data[i];
        if (cptr == NULL) {
            continue;
        }
        if (cptr->frame_num != fnum) {
            continue;
        }
        if (cptr->deleted_flag == 1) {
            continue;
        }

    }

    return;

}  /*  end of function reindex_contours */



int CDisplayList::SetupSpatialIndexForFrame (int fnum)
{
    FRameStruct        *frptr;

    if (frame_list == NULL) {
        return -1;
    }

    line_spatial_index = NULL;
    fill_spatial_index = NULL;
    symb_spatial_index = NULL;
    text_spatial_index = NULL;
    shape_spatial_index = NULL;
    contour_spatial_index = NULL;

    index_xmin = 1.e30;
    index_ymin = 1.e30;
    index_xmax = -1.e30;
    index_ymax = -1.e30;
    index_xspace = 1.e30;
    index_yspace = 1.e30;

    index_ncol = 0;
    index_nrow = 0;

    if (fnum < 0  ||  fnum >= num_frame_list) {
        return -1;
    }

    frptr = frame_list + fnum;

    if (frptr->aspect_flag != 0) {
        return -1;
    }

    line_spatial_index = frptr->line_index;
    fill_spatial_index = frptr->fill_index;
    text_spatial_index = frptr->text_index;
    symb_spatial_index = frptr->symb_index;
    shape_spatial_index = frptr->shape_index;
    contour_spatial_index = frptr->contour_index;

    index_xmin = frptr->xmin_spatial;
    index_ymin = frptr->ymin_spatial;
    index_xmax = frptr->xmax_spatial;
    index_ymax = frptr->ymax_spatial;
    index_xspace = frptr->xspace;
    index_yspace = frptr->yspace;

    index_ncol = frptr->ncol;
    index_nrow = frptr->nrow;

    num_line_index = frptr->num_line_index;
    num_fill_index = frptr->num_fill_index;
    num_text_index = frptr->num_text_index;
    num_symb_index = frptr->num_symb_index;
    num_shape_index = frptr->num_shape_index;
    num_contour_index = frptr->num_contour_index;

    if (line_spatial_index == NULL) {
        return -1;
    }

    return 1;

}

void CDisplayList::ClearFrameSpatialIndex (int fnum)
{
    FRameStruct      *frptr;
    int              i, ntot;

    if (fnum < 0  ||  fnum >= num_frame_list) {
        return;
    }

    if (frame_list == NULL) {
        return;
    }

    frptr = frame_list + fnum;

    ntot = frptr->ncol * frptr->nrow + 1;
    if (ntot <= 0) {
        return;
    }

    if (frptr->line_index) {
        for (i=0; i<ntot; i++) {
            csw_Free (frptr->line_index[i]);
            frptr->line_index[i] = NULL;
        }
    }

    if (frptr->fill_index) {
        for (i=0; i<ntot; i++) {
            csw_Free (frptr->fill_index[i]);
            frptr->fill_index[i] = NULL;
        }
    }

    if (frptr->text_index) {
        for (i=0; i<ntot; i++) {
            csw_Free (frptr->text_index[i]);
            frptr->text_index[i] = NULL;
        }
    }

    if (frptr->symb_index) {
        for (i=0; i<ntot; i++) {
            csw_Free (frptr->symb_index[i]);
            frptr->symb_index[i] = NULL;
        }
    }

    if (frptr->shape_index) {
        for (i=0; i<ntot; i++) {
            csw_Free (frptr->shape_index[i]);
            frptr->shape_index[i] = NULL;
        }
    }

    if (frptr->contour_index) {
        for (i=0; i<ntot; i++) {
            csw_Free (frptr->contour_index[i]);
            frptr->contour_index[i] = NULL;
        }
    }

    return;

}



/*
******************************************************************

             P o p u l a t e L i n e P a t c h e s

******************************************************************

*/

int CDisplayList::PopulateLinePatches (double x1, double y1p,
                                   double x2, double y2)

{
    int                  *icell, nlist, prim_num;
    int                  nc, nr, offset, j, k, kk,
                         row1, row2, col1, col2;
    double               gx1, gy1, gxs, gys, tmp, tiny;
    LInePrim             *lptr;

    int              lp_size = (int)line_prim_list.size();
    LInePrim         *lp_data = line_prim_list.data();

    if (lp_data == NULL  ||  lp_size < 1) {
        return 0;
    }

    if (patch_draw_flag == 0) {
        return 0;
    }

    if (num_line_index == 0) {
        return 0;
    }

/*
 * If the line spatial indexing has not been set up,
 * draw all lines instead of patch.
 */
    if (line_spatial_index == NULL) {
        return 0;
    }

    if (index_xmin >= index_xmax  ||
        index_ymin >= index_ymax  ||
        index_xspace > 1.e20  ||
        index_yspace > 1.e20  ||
        index_ncol < 2  ||
        index_nrow < 2) {
        return 0;
    }

    gxs = index_xspace;
    gys = index_yspace;
    tiny = (gxs + gys) / 20.0f;

    nc = index_ncol;
    nr = index_nrow;
    gx1 = index_xmin;
    gy1 = index_ymin;

    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1p > y2) {
        tmp = y1p;
        y1p = y2;
        y2 = tmp;
    }

    x1 -= tiny;
    x2 += tiny;
    y1p -= tiny;
    y2 += tiny;

/*
    start and end rows and columns for patch area
*/
    col1 = (int)((x1 - gx1) / gxs);
    col2 = (int)((x2 - gx1) / gxs);
    row1 = (int)((y1p - gy1) / gys);
    row2 = (int)((y2 - gy1) / gys);
    if (col2 < col1) {
        col2 = col1;
    }
    if (row2 < row1) {
        row2 = row1;
    }

    if (col1 < 0) {
        col1 = 0;
    }
    if (col2 < 0) {
        col2 = 0;
    }
    if (col1 > nc-1) {
        col1 = nc - 1;
    }
    if (col2 > nc-1) {
        col2 = nc - 1;
    }
    if (row1 < 0) {
        row1 = 0;
    }
    if (row2 < 0) {
        row2 = 0;
    }
    if (row1 > nr-1) {
        row1 = nr - 1;
    }
    if (row2 > nr-1) {
        row2 = nr - 1;
    }

/*
    set all of the primitive drawflags to 1
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = line_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= lp_size) {
                    continue;
                }
                lptr = lp_data + prim_num;
                lptr->draw_flag = 1;
            }
        }
    }

/*
    redraw the line primitives in the region
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = line_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= lp_size) {
                    continue;
                }
                lptr = lp_data + prim_num;
                if (lptr->draw_flag == 0  ||
                    lptr->deleted_flag == 1  ||
                    lptr->visible_flag == 0) {
                    continue;
                }
                add_line_patch_prim (prim_num);
                lptr->draw_flag = 0;
            }
        }
    }

/*
    Populate patch with all prims in the extra grid cell if needed
*/
    int  pt_size = 0;
    int  *pt_data = NULL;

    if (patch_pick_flag == 0) {
        pt_size = (int)line_patch_list.size();
        pt_data = line_patch_list.data();
    }
    else {
        pt_size = (int)line_pick_list.size();
        pt_data = line_pick_list.data();
    }

    if (pt_data != NULL  &&  pt_size > 0) {
        for (j=0; j<pt_size; j++) {
            lp_data[pt_data[j]].draw_flag = 2;
        }
    }

    offset = nc * nr;
    icell = line_spatial_index [offset];
    if (icell != NULL) {
        nlist = icell[1];
        if (nlist > 0) {
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= lp_size) {
                    continue;
                }
                lptr = lp_data + prim_num;
                if (lptr->draw_flag == 2  ||
                    lptr->deleted_flag == 1  ||
                    lptr->visible_flag == 0) {
                    continue;
                }
                add_line_patch_prim (prim_num);
            }
        }
    }

    if (patch_pick_flag == 0) {
        pt_size = (int)line_patch_list.size();
        pt_data = line_patch_list.data();
    }
    else {
        pt_size = (int)line_pick_list.size();
        pt_data = line_pick_list.data();
    }

    if (pt_data != NULL  &&  pt_size > 0) {
        for (j=0; j<pt_size; j++) {
            lp_data[pt_data[j]].draw_flag = 0;
        }
    }

    return 1;

}  /*  end of function PopulateLinePatches  */



/*
******************************************************************

               P o p u l a t e F i l l P a t c h e s

******************************************************************

*/

int CDisplayList::PopulateFillPatches (double x1, double y1p,
                                   double x2, double y2)

{
    int                  *icell, nlist, prim_num;
    int                  nc, nr, offset, j, k, kk,
                         row1, row2, col1, col2;
    double               gx1, gy1, gxs, gys, tmp, tiny;
    FIllPrim             *fptr;

    int            fp_size = (int)fill_prim_list.size();
    FIllPrim       *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return 0;
    }

    if (patch_draw_flag == 0) {
        return 0;
    }

    if (num_fill_index == 0) {
        return 0;
    }

/*
 * If the fill spatial indexing has not been set up,
 * draw all fills instead of patch.
 */
    if (fill_spatial_index == NULL) {
        return 0;
    }

    if (index_xmin >= index_xmax  ||
        index_ymin >= index_ymax  ||
        index_xspace > 1.e20  ||
        index_yspace > 1.e20  ||
        index_ncol < 2  ||
        index_nrow < 2) {
        return 0;
    }

    gxs = index_xspace;
    gys = index_yspace;
    tiny = (gxs + gys) / 20.0;

    nc = index_ncol;
    nr = index_nrow;
    gx1 = index_xmin;
    gy1 = index_ymin;

    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1p > y2) {
        tmp = y1p;
        y1p = y2;
        y2 = tmp;
    }

    x1 -= tiny;
    x2 += tiny;
    y1p -= tiny;
    y2 += tiny;

/*
    start and end rows and columns for patch area
*/
    col1 = (int)((x1 - gx1) / gxs);
    col2 = (int)((x2 - gx1) / gxs);
    row1 = (int)((y1p - gy1) / gys);
    row2 = (int)((y2 - gy1) / gys);
    if (col2 < col1) {
        col2 = col1;
    }
    if (row2 < row1) {
        row2 = row1;
    }

    if (col1 < 0) {
        col1 = 0;
    }
    if (col2 < 0) {
        col2 = 0;
    }
    if (col1 > nc-1) {
        col1 = nc - 1;
    }
    if (col2 > nc-1) {
        col2 = nc - 1;
    }
    if (row1 < 0) {
        row1 = 0;
    }
    if (row2 < 0) {
        row2 = 0;
    }
    if (row1 > nr-1) {
        row1 = nr - 1;
    }
    if (row2 > nr-1) {
        row2 = nr - 1;
    }

/*
    set all of the primitive drawflags to 1
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = fill_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= fp_size) {
                    continue;
                }
                fptr = fp_data + prim_num;
                fptr->draw_flag = 1;
            }
        }
    }

/*
    redraw the fill primitives in the region
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = fill_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= fp_size) {
                    continue;
                }
                fptr = fp_data + prim_num;
                if (fptr->draw_flag == 0  ||
                    fptr->deleted_flag == 1  ||
                    fptr->visible_flag == 0) {
                    continue;
                }
                add_fill_patch_prim (prim_num);
                fptr->draw_flag = 0;
            }
        }
    }

/*
    Populate patch with all prims in the extra grid cell if needed
*/
    int  pt_size = 0;
    int  *pt_data = NULL;

    if (patch_pick_flag == 0) {
        pt_size = (int)fill_patch_list.size();
        pt_data = fill_patch_list.data();
    }
    else {
        pt_size = (int)fill_pick_list.size();
        pt_data = fill_pick_list.data();
    }

    if (pt_data != NULL  &&  pt_size > 0) {
        for (j=0; j<pt_size; j++) {
            fp_data[pt_data[j]].draw_flag = 2;
        }
    }

    offset = nc * nr;
    icell = fill_spatial_index [offset];
    if (icell != NULL) {
        nlist = icell[1];
        if (nlist > 0) {
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= fp_size) {
                    continue;
                }
                fptr = fp_data + prim_num;
                if (fptr->draw_flag == 2  ||
                    fptr->deleted_flag == 1  ||
                    fptr->visible_flag == 0) {
                    continue;
                }
                add_fill_patch_prim (prim_num);
            }
        }
    }

    if (patch_pick_flag == 0) {
        pt_size = (int)fill_patch_list.size();
        pt_data = fill_patch_list.data();
    }
    else {
        pt_size = (int)fill_pick_list.size();
        pt_data = fill_pick_list.data();
    }

    if (pt_data != NULL  &&  pt_size > 0) {
        for (j=0; j<pt_size; j++) {
            fp_data[pt_data[j]].draw_flag = 0;
        }
    }

    return 1;

}  /*  end of function PopulateFillPatches  */



/*
******************************************************************

         P o p u l a t e T e x t P a t c h e s

******************************************************************

*/

int CDisplayList::PopulateTextPatches (double x1, double y1p,
                                       double x2, double y2)

{
    int                  *icell, nlist, prim_num;
    int                  nc, nr, offset, j, k, kk,
                         row1, row2, col1, col2;
    double               gx1, gy1, gxs, gys, tmp, tiny;
    TExtPrim             *tptr;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return 0;
    }

    if (patch_draw_flag == 0) {
        return 0;
    }

    if (num_text_index == 0) {
        return 0;
    }

/*
 * If the text spatial indexing has not been set up,
 * draw all texts instead of patch.
 */
    if (text_spatial_index == NULL) {
        return 0;
    }

    if (index_xmin >= index_xmax  ||
        index_ymin >= index_ymax  ||
        index_xspace > 1.e20  ||
        index_yspace > 1.e20  ||
        index_ncol < 2  ||
        index_nrow < 2) {
        return 0;
    }

    gxs = index_xspace;
    gys = index_yspace;
    tiny = (gxs + gys) / 20.0f;

    nc = index_ncol;
    nr = index_nrow;
    gx1 = index_xmin;
    gy1 = index_ymin;

    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1p > y2) {
        tmp = y1p;
        y1p = y2;
        y2 = tmp;
    }

    x1 -= tiny;
    x2 += tiny;
    y1p -= tiny;
    y2 += tiny;

/*
    start and end rows and columns for patch area
*/
    col1 = (int)((x1 - gx1) / gxs);
    col2 = (int)((x2 - gx1) / gxs);
    row1 = (int)((y1p - gy1) / gys);
    row2 = (int)((y2 - gy1) / gys);
    if (col2 < col1) {
        col2 = col1;
    }
    if (row2 < row1) {
        row2 = row1;
    }

    if (col1 < 0) {
        col1 = 0;
    }
    if (col2 < 0) {
        col2 = 0;
    }
    if (col1 > nc-1) {
        col1 = nc - 1;
    }
    if (col2 > nc-1) {
        col2 = nc - 1;
    }
    if (row1 < 0) {
        row1 = 0;
    }
    if (row2 < 0) {
        row2 = 0;
    }
    if (row1 > nr-1) {
        row1 = nr - 1;
    }
    if (row2 > nr-1) {
        row2 = nr - 1;
    }

/*
    set all of the primitive drawflags to 1
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = text_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= tp_size) {
                    continue;
                }
                tptr = tp_data + prim_num;
                tptr->draw_flag = 1;
            }
        }
    }

/*
    Add the text primitives in the region to the patch.
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = text_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= tp_size) {
                    continue;
                }
                tptr = tp_data + prim_num;
                if (tptr->draw_flag == 0  ||
                    tptr->deleted_flag == 1  ||
                    tptr->visible_flag == 0) {
                    continue;
                }

                add_text_patch_prim (prim_num);
                tptr->draw_flag = 0;
            }
        }
    }

/*
    Populate patch with all prims in the extra grid cell if needed
*/
    int  pt_size = 0;
    int  *pt_data = NULL;

    if (patch_pick_flag == 0) {
        pt_size = (int)text_patch_list.size();
        pt_data = text_patch_list.data();
    }
    else {
        pt_size = (int)text_pick_list.size();
        pt_data = text_pick_list.data();
    }

    if (pt_data != NULL  &&  pt_size > 0) {
        for (j=0; j<pt_size; j++) {
            tp_data[pt_data[j]].draw_flag = 2;
        }
    }

    offset = nc * nr;
    icell = text_spatial_index [offset];
    if (icell != NULL) {
        nlist = icell[1];
        if (nlist > 0) {
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= tp_size) {
                    continue;
                }
                tptr = tp_data + prim_num;
                if (tptr->draw_flag == 2  ||
                    tptr->deleted_flag == 1  ||
                    tptr->visible_flag == 0) {
                    continue;
                }
                add_text_patch_prim (prim_num);
            }
        }
    }

    if (patch_pick_flag == 0) {
        pt_size = (int)text_patch_list.size();
        pt_data = text_patch_list.data();
    }
    else {
        pt_size = (int)text_pick_list.size();
        pt_data = text_pick_list.data();
    }

    if (pt_data != NULL  &&  pt_size > 0) {
        for (j=0; j<pt_size; j++) {
            tp_data[pt_data[j]].draw_flag = 0;
        }
    }

    return 1;

}  /*  end of function PopulateTextPatches  */





/*
******************************************************************

               P o p u l a t e S y m b P a t c h e s

******************************************************************

*/

int CDisplayList::PopulateSymbPatches (double x1, double y1p,
                                   double x2, double y2)

{
    int                  *icell, nlist, prim_num;
    int                  nc, nr, offset, j, k, kk,
                         row1, row2, col1, col2;
    double               gx1, gy1, gxs, gys, tmp, tiny;
    SYmbPrim             *sptr;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return 0;
    }

    if (patch_draw_flag == 0) {
        return 0;
    }

    if (num_symb_index == 0) {
        return 0;
    }

/*
 * If the symb spatial indexing has not been set up,
 * draw all symbs instead of patch.
 */
    if (symb_spatial_index == NULL) {
        return 0;
    }

    if (index_xmin >= index_xmax  ||
        index_ymin >= index_ymax  ||
        index_xspace > 1.e20  ||
        index_yspace > 1.e20  ||
        index_ncol < 2  ||
        index_nrow < 2) {
        return 0;
    }

    gxs = index_xspace;
    gys = index_yspace;
    tiny = (gxs + gys) / 20.0f;

    nc = index_ncol;
    nr = index_nrow;
    gx1 = index_xmin;
    gy1 = index_ymin;

    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1p > y2) {
        tmp = y1p;
        y1p = y2;
        y2 = tmp;
    }

    x1 -= tiny;
    x2 += tiny;
    y1p -= tiny;
    y2 += tiny;

/*
    start and end rows and columns for patch area
*/
    col1 = (int)((x1 - gx1) / gxs);
    col2 = (int)((x2 - gx1) / gxs);
    row1 = (int)((y1p - gy1) / gys);
    row2 = (int)((y2 - gy1) / gys);
    if (col2 < col1) {
        col2 = col1;
    }
    if (row2 < row1) {
        row2 = row1;
    }

    if (col1 < 0) {
        col1 = 0;
    }
    if (col2 < 0) {
        col2 = 0;
    }
    if (col1 > nc-1) {
        col1 = nc - 1;
    }
    if (col2 > nc-1) {
        col2 = nc - 1;
    }
    if (row1 < 0) {
        row1 = 0;
    }
    if (row2 < 0) {
        row2 = 0;
    }
    if (row1 > nr-1) {
        row1 = nr - 1;
    }
    if (row2 > nr-1) {
        row2 = nr - 1;
    }

/*
    set all of the primitive drawflags to 1
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = symb_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= sp_size) {
                    continue;
                }
                sptr = sp_data + prim_num;
                sptr->draw_flag = 1;
            }
        }
    }

/*
    redraw the symb primitives in the region
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = symb_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= sp_size) {
                    continue;
                }
                sptr = sp_data + prim_num;
                if (sptr->draw_flag == 0  ||
                    sptr->deleted_flag == 1  ||
                    sptr->visible_flag == 0) {
                    continue;
                }
                add_symb_patch_prim (prim_num);
                sptr->draw_flag = 0;
            }
        }
    }

/*
    Populate patch with all prims in the extra grid cell if needed
*/
    offset = nc * nr;
    icell = symb_spatial_index [offset];
    if (icell != NULL) {
        nlist = icell[1];
        if (nlist > 0) {
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= sp_size) {
                    continue;
                }
                sptr = sp_data + prim_num;
                sptr->draw_flag = 1;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= sp_size) {
                    continue;
                }
                sptr = sp_data + prim_num;
                if (sptr->draw_flag == 0  ||
                    sptr->deleted_flag == 1  ||
                    sptr->visible_flag == 0) {
                    continue;
                }
                add_symb_patch_prim (prim_num);
                sptr->draw_flag = 0;
            }
        }
    }

    return 1;

}  /*  end of function PopulateSymbPatches  */




/*
******************************************************************

               P o p u l a t e S h a p e P a t c h e s

******************************************************************

*/

int CDisplayList::PopulateShapePatches (double x1, double y1p,
                                   double x2, double y2)

{
    int                  *icell, nlist, prim_num;
    int                  nc, nr, offset, j, k, kk,
                         row1, row2, col1, col2;
    double               gx1, gy1, gxs, gys, tmp, tiny;
    SHapePrim            *shptr;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return 0;
    }

    if (patch_draw_flag == 0) {
        return 0;
    }

    if (num_shape_index == 0) {
        return 0;
    }

/*
 * If the shape spatial indexing has not been set up,
 * draw all shapes instead of patch.
 */
    if (shape_spatial_index == NULL) {
        return 0;
    }

    if (index_xmin >= index_xmax  ||
        index_ymin >= index_ymax  ||
        index_xspace > 1.e20  ||
        index_yspace > 1.e20  ||
        index_ncol < 2  ||
        index_nrow < 2) {
        return 0;
    }

    gxs = index_xspace;
    gys = index_yspace;
    tiny = (gxs + gys) / 20.0f;

    nc = index_ncol;
    nr = index_nrow;
    gx1 = index_xmin;
    gy1 = index_ymin;

    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1p > y2) {
        tmp = y1p;
        y1p = y2;
        y2 = tmp;
    }

    x1 -= tiny;
    x2 += tiny;
    y1p -= tiny;
    y2 += tiny;

/*
    start and end rows and columns for patch area
*/
    col1 = (int)((x1 - gx1) / gxs);
    col2 = (int)((x2 - gx1) / gxs);
    row1 = (int)((y1p - gy1) / gys);
    row2 = (int)((y2 - gy1) / gys);
    if (col2 < col1) {
        col2 = col1;
    }
    if (row2 < row1) {
        row2 = row1;
    }

    if (col1 < 0) {
        col1 = 0;
    }
    if (col2 < 0) {
        col2 = 0;
    }
    if (col1 > nc-1) {
        col1 = nc - 1;
    }
    if (col2 > nc-1) {
        col2 = nc - 1;
    }
    if (row1 < 0) {
        row1 = 0;
    }
/*
    if (row2 < 0) {
        row2 = 0;
    }
    if (row1 > nr-1) {
        row1 = nr - 1;
    }
*/
    if (row2 > nr-1) {
        row2 = nr - 1;
    }

/*
    set all of the primitive drawflags to 1
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = shape_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= hp_size) {
                    continue;
                }
                shptr = hp_data + prim_num;
                shptr->draw_flag = 1;
            }
        }
    }

/*
    Populate patch with the shape primitives in the region
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = shape_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= hp_size) {
                    continue;
                }
                shptr = hp_data + prim_num;
                if (shptr->draw_flag == 0  ||
                    shptr->deleted_flag == 1  ||
                    shptr->visible_flag == 0) {
                    continue;
                }
                add_shape_patch_prim (prim_num);
                shptr->draw_flag = 0;
            }
        }
    }

/*
    Populate patch with all prims in the extra grid cell if needed
*/
    int  pt_size = 0;
    int  *pt_data = NULL;

    if (patch_pick_flag == 0) {
        pt_size = (int)shape_patch_list.size();
        pt_data = shape_patch_list.data();
    }
    else {
        pt_size = (int)shape_pick_list.size();
        pt_data = shape_pick_list.data();
    }

    if (pt_data != NULL  &&  pt_size > 0) {
        for (j=0; j<pt_size; j++) {
            hp_data[pt_data[j]].draw_flag = 2;
        }
    }

    offset = nc * nr;
    icell = shape_spatial_index [offset];
    if (icell != NULL) {
        nlist = icell[1];
        if (nlist > 0) {
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= hp_size) {
                    continue;
                }
                shptr = hp_data + prim_num;
                if (shptr->draw_flag == 2  ||
                    shptr->deleted_flag == 1  ||
                    shptr->visible_flag == 0) {
                    continue;
                }
                add_shape_patch_prim (prim_num);
            }
        }
    }

    if (patch_pick_flag == 0) {
        pt_size = (int)shape_patch_list.size();
        pt_data = shape_patch_list.data();
    }
    else {
        pt_size = (int)shape_pick_list.size();
        pt_data = shape_pick_list.data();
    }

    if (pt_data != NULL  &&  pt_size > 0) {
        for (j=0; j<pt_size; j++) {
            hp_data[pt_data[j]].draw_flag = 0;
        }
    }

    return 1;

}  /*  end of function PopulateShapePatches  */





/*
******************************************************************

               P o p u l a t e C o n t o u r P a t c h e s

******************************************************************

*/

int CDisplayList::PopulateContourLinePatches (double x1, double y1p,
                                              double x2, double y2)

{
    int                  *icell, nlist, prim_num;
    int                  nc, nr, offset, j, k, kk,
                         row1, row2, col1, col2;
    double               gx1, gy1, gxs, gys, tmp, tiny;
    DLContour            *cptr;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return 0;
    }

    if (patch_draw_flag == 0) {
        return 0;
    }

    if (num_contour_index == 0) {
        return 0;
    }

/*
 * If the contour spatial indexing has not been set up,
 * draw all lines instead of patch.
 */
    if (contour_spatial_index == NULL) {
        return 0;
    }

    if (index_xmin >= index_xmax  ||
        index_ymin >= index_ymax  ||
        index_xspace > 1.e20  ||
        index_yspace > 1.e20  ||
        index_ncol < 2  ||
        index_nrow < 2) {
        return 0;
    }

    gxs = index_xspace;
    gys = index_yspace;
    tiny = (gxs + gys) / 20.0f;

    nc = index_ncol;
    nr = index_nrow;
    gx1 = index_xmin;
    gy1 = index_ymin;

    if (x1 > x2) {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1p > y2) {
        tmp = y1p;
        y1p = y2;
        y2 = tmp;
    }

    x1 -= tiny;
    x2 += tiny;
    y1p -= tiny;
    y2 += tiny;

/*
    start and end rows and columns for patch area
*/
    col1 = (int)((x1 - gx1) / gxs);
    col2 = (int)((x2 - gx1) / gxs);
    row1 = (int)((y1p - gy1) / gys);
    row2 = (int)((y2 - gy1) / gys);
    if (col2 < col1) {
        col2 = col1;
    }
    if (row2 < row1) {
        row2 = row1;
    }

    if (col1 < 0) {
        col1 = 0;
    }
    if (col2 < 0) {
        col2 = 0;
    }
    if (col1 > nc-1) {
        col1 = nc - 1;
    }
    if (col2 > nc-1) {
        col2 = nc - 1;
    }
    if (row1 < 0) {
        row1 = 0;
    }
    if (row2 < 0) {
        row2 = 0;
    }
    if (row1 > nr-1) {
        row1 = nr - 1;
    }
    if (row2 > nr-1) {
        row2 = nr - 1;
    }

/*
    set all of the primitive drawflags to 1
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = contour_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= cl_size) {
                    continue;
                }
                cptr = cl_data[prim_num];
                cptr->draw_flag = 1;
            }
        }
    }

/*
    redraw the line primitives in the region
*/
    for (j=row1; j<=row2; j++) {
        offset = j*nc;
        for (k=col1; k<=col2; k++) {
            icell = contour_spatial_index [offset + k];
            if (icell == NULL) {
                continue;
            }
            nlist = icell[1];
            if (nlist < 1) {
                continue;
            }
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= cl_size) {
                    continue;
                }
                cptr = cl_data[prim_num];
                if (cptr->draw_flag == 0  ||
                    cptr->deleted_flag == 1  ||
                    cptr->visible_flag == 0) {
                    continue;
                }
                add_contour_line_patch_prim (prim_num);
                cptr->draw_flag = 0;
            }
        }
    }

/*
    Populate patch with all prims in the extra grid cell if needed
*/
    int  pt_size = 0;
    int  *pt_data = NULL;

    if (patch_pick_flag == 0) {
        pt_size = (int)contour_line_patch_list.size();
        pt_data = contour_line_patch_list.data();
    }
    else {
        pt_size = (int)contour_line_pick_list.size();
        pt_data = contour_line_pick_list.data();
    }

    if (pt_data != NULL  &&  pt_size > 0) {
        for (j=0; j<pt_size; j++) {
            cl_data[pt_data[j]]->draw_flag = 2;
        }
    }

    offset = nc * nr;
    icell = contour_spatial_index [offset];
    if (icell != NULL) {
        nlist = icell[1];
        if (nlist > 0) {
            for (kk=2; kk<=nlist+1; kk++) {
                prim_num = icell[kk];
                if (prim_num < 0  ||  prim_num >= cl_size) {
                    continue;
                }
                cptr = cl_data[prim_num];
                if (cptr->draw_flag == 2  ||
                    cptr->deleted_flag == 1  ||
                    cptr->visible_flag == 0) {
                    continue;
                }
                add_contour_line_patch_prim (prim_num);
            }
        }
    }

    if (patch_pick_flag == 0) {
        pt_size = (int)contour_line_patch_list.size();
        pt_data = contour_line_patch_list.data();
    }
    else {
        pt_size = (int)contour_line_pick_list.size();
        pt_data = contour_line_pick_list.data();
    }

    if (pt_data != NULL  &&  pt_size > 0) {
        for (j=0; j<pt_size; j++) {
            cl_data[pt_data[j]]->draw_flag = 0;
        }
    }

    return 1;

}  /*  end of function PopulateContourLinePatches  */





/*
  ****************************************************************

              C a l c S h a p e P a g e B o u n d s

  ****************************************************************

*/

int CDisplayList::CalcShapeBounds (int shape_prim_num,
                                   double *xmin, double *ymin,
                                   double *xmax, double *ymax)

{
    SHapePrim        *shptr;
    double           xc, yc, w, h, ang, cang, sang;
    double           xt[4], yt[4], x, y;
    int              i;

    *xmin = 1.e30;
    *ymin = 1.e30;
    *xmax = -1.e30;
    *ymax = -1.e30;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (shape_prim_num < 0  ||  shape_prim_num >= hp_size) {
        return -1;
    }
    if (hp_data == NULL  ||  hp_size < 1) {
        return -1;
    }

    shptr = hp_data + shape_prim_num;

    xc = shptr->fval[0];
    yc = shptr->fval[1];
    w = shptr->fval[2];
    h = shptr->fval[3];
    ang = shptr->fval[6];
    if (shptr->type == 1) {
        w /= 2.0;
        h /= 2.0;
        ang = shptr->fval[5];
    }

    ang = ang * 3.1415926 / 180.0;
    cang = cos (ang);
    sang = sin (ang);

    xt[0] = -w;
    yt[0] = -h;
    xt[1] = w;
    yt[1] = -h;
    xt[2] = w;
    yt[2] = h;
    xt[3] = -w;
    yt[3] = h;

    for (i=0; i<4; i++) {
        x = xt[i];
        y = yt[i];
        xt[i] = x * cang - y * sang;
        yt[i] = x * sang + y * cang;
        xt[i] += xc;
        yt[i] += yc;
        if (xt[i] < *xmin) *xmin = xt[i];
        if (yt[i] < *ymin) *ymin = yt[i];
        if (xt[i] > *xmax) *xmax = xt[i];
        if (yt[i] > *ymax) *ymax = yt[i];
    }

    return 1;

}  /*  end of function CalcShapeBounds  */






/*
  ****************************************************************

                C a l c T e x t O u t l i n e

  ****************************************************************

*/

int CDisplayList::CalcTextOutline (int text_prim_num,
                              double *xmin, double *ymin,
                              double *xmax, double *ymax,
                              double *xpoly, double *ypoly, int *npoly)

{
    TExtPrim        *tptr;
    CSW_F           width;
    int             i;
    CSW_F           cx[4], cy[4];
    CSW_F           fsize, xzero, yzero, fzero;
    CSW_F           px1, py1;

    *xmin = 1.e30;
    *ymin = 1.e30;
    *xmax = -1.e30;
    *ymax = -1.e30;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return -1;
    }

    if (text_prim_num < 0  ||  text_prim_num >= tp_size) {
        return -1;
    }

    if (xpoly == NULL  ||  ypoly == NULL  ||  npoly == NULL) {
        return -1;
    }

    tptr = tp_data + text_prim_num;

/*
 * Convert the text size in inches to a text size in
 * frame units.
 */
    if (page_units_per_inch < 1.e20) {
        fsize = tptr->size * page_units_per_inch;
    }
    else {
        fsize = tptr->size * 10.0f;
    }

    px1 = Px1;
    py1 = Py1;
    if (Px2 < Px1  ||  Py2 < Py1) {
        px1 = 0.0;
        py1 = 0.0;
    }

    xzero = px1;
    yzero = py1;
    fsize += px1;
    fzero = py1;
    unconvert_frame_point (&xzero, &yzero);
    unconvert_frame_point (&fsize, &fzero);
    fsize -= xzero;
    if (fsize < 0.0) fsize = -fsize;

    width = strlen(tptr->chardata) * fsize;

    gpf_calcdraw_obj.gpf_find_box_corners (tptr->x, tptr->y,
                          tptr->anchor,
                          width, fsize, tptr->angle,
                          cx, cy);

    for (i=0; i<4; i++) {
        if (cx[i] < *xmin) *xmin = cx[i];
        if (cx[i] > *xmax) *xmax = cx[i];
        if (cy[i] < *ymin) *ymin = cy[i];
        if (cy[i] > *ymax) *ymax = cy[i];
        xpoly[i] = cx[i];
        ypoly[i] = cy[i];
    }
    xpoly[4] = cx[0];
    ypoly[4] = cy[0];

    npoly[0] = 5;

    return 1;

}  /*  end of function CalcTextOutline  */



int CDisplayList::PickFrameObject (int frame_num,
                                   int ix,
                                   int iy)
{
    CSW_F             xt1, yt1;
    int               framesav, isel;

    if (frame_num < 0  ||  frame_num >= num_frame_list  ||  frame_list == NULL) {
        return -1;
    }

    framesav = current_frame_num;

    current_frame_num = frame_num;
    update_frame_limits ();

    backscalef ((CSW_F)ix, (CSW_F)iy, &xt1, &yt1);
    unconvert_frame_point (&xt1, &yt1);

    isel = ProcessFramePick (frame_num, xt1, yt1);

    ReturnSelected ();

    current_frame_num = framesav;
    update_frame_limits ();

    return isel;

}

int CDisplayList::GetFrameObject (int frame_num,
                                  int ix,
                                  int iy)
{
    CSW_F             xt1, yt1;
    int               framesav, isel;

    if (frame_num < 0  ||  frame_num >= num_frame_list  ||  frame_list == NULL) {
        return -1;
    }

    framesav = current_frame_num;

    current_frame_num = frame_num;
    update_frame_limits ();

    backscalef ((CSW_F)ix, (CSW_F)iy, &xt1, &yt1);
    unconvert_frame_point (&xt1, &yt1);

    isel = GetSelectableIndex (frame_num, xt1, yt1);

    current_frame_num = framesav;
    update_frame_limits ();

    return isel;

}

int CDisplayList::SetSelectableState (int index,
                                      int ival)
{
    DLSelectable      *dls;

    if (ival < 0) {
        return -1;
    }

    if (index >= num_selectable_object_list) {
        printf
          ("\n********  index of selectable is bad  ********\n\n");
        fflush (stdout);
        return 1;
    }

    if (selectable_object_list != NULL) {
        dls = selectable_object_list[index];
        if (dls == NULL) {
            return -1;
        }
        dls->SetSelectState (ival);
    }
    else {
        return -1;
    }

    ReturnSelected ();
    return 1;

}


int CDisplayList::ConvertToFrame (int frame_num,
                                  int ix,
                                  int iy)
{
    CSW_F             xt1, yt1, zt1;
    int               framesav;

    if (frame_num < 0  ||  frame_num >= num_frame_list  ||  frame_list == NULL) {
        return -1;
    }

    framesav = current_frame_num;

    current_frame_num = frame_num;
    update_frame_limits ();

    backscalef ((CSW_F)ix, (CSW_F)iy, &xt1, &yt1);
    unconvert_frame_point (&xt1, &yt1);

    zt1 = 1.e30f;

    current_frame_num = framesav;
    update_frame_limits ();

    jni_return_converted_xyz ((double)xt1, (double)yt1, (double)zt1);

    return 1;

}


int CDisplayList::GetSelectableIndex (int frame_num,
                                      CSW_F x,
                                      CSW_F y)
{
    int               isel, index, type, istat;
    LInePrim          *lptr;
    FIllPrim          *fptr;
    TExtPrim          *tptr;
    SYmbPrim          *sptr;
    SHapePrim         *shptr;
    DLContour         *cptr;
    AXisPrim          *aptr;

    if (frame_num < 0  ||  frame_num >= num_frame_list  ||  frame_list == NULL) {
        return -1;
    }

    istat = ClosestPickPrim (frame_num, x, y,
                             &index, &type);
    if (istat == -1) {
        return -1;
    }

    isel = -1;

    int             lp_size = (int)line_prim_list.size();
    LInePrim        *lp_data = line_prim_list.data();

    int             fp_size = (int)fill_prim_list.size();
    FIllPrim        *fp_data = fill_prim_list.data();

    int             sp_size = (int)symb_prim_list.size();
    SYmbPrim        *sp_data = symb_prim_list.data();

    int             tp_size = (int)text_prim_list.size();
    TExtPrim        *tp_data = text_prim_list.data();

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    int          ap_size = (int)axis_prim_list.size();
    AXisPrim     *ap_data = axis_prim_list.data();

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (type == 1  &&  lp_data != NULL  &&  lp_size > index) {
        lptr = lp_data + index;
        isel = lptr->selectable_object_num;
    }
    else if (type == 2  &&  fp_data != NULL  &&  fp_size > index) {
        fptr = fp_data + index;
        isel = fptr->selectable_object_num;
    }
    else if (type == 3  &&  tp_data != NULL  &&  tp_size > 0) {
        tptr = tp_data + index;
        isel = tptr->selectable_object_num;
    }
    else if (type == 4  &&  sp_data != NULL  &&  sp_size > 0) {
        sptr = sp_data + index;
        isel = sptr->selectable_object_num;
    }
    else if (type == 5  &&  hp_data != NULL  &&  hp_size > 0) {
        shptr = hp_data + index;
        isel = shptr->selectable_object_num;
    }
    else if (type == 6  &&  cl_data != NULL  &&  cl_size > 0) {
        cptr = cl_data[index];
        isel = cptr->selectable_object_num;
    }
    else if (type == 7  &&  ap_data != NULL  &&  ap_size > 0) {
        aptr = ap_data + index;
        isel = aptr->selectable_object_num;
    }

    return isel;
}


int CDisplayList::ProcessFramePick (int frame_num,
                                    CSW_F x,
                                    CSW_F y)
{
    int               isel;
    DLSelectable      *dls;

    if (frame_num < 0  ||  frame_num >= num_frame_list  ||  frame_list == NULL) {
        return -1;
    }

    isel = GetSelectableIndex(frame_num, x, y);

    if (isel < 0) {
        return -1;
    }

    if (selectable_object_list != NULL) {
        dls = selectable_object_list[isel];
        if (dls == NULL) {
            return -1;
        }
        dls->SetSelectState (1);
    }
    else {
        isel = -1;
    }

    return isel;

}



int CDisplayList::ClosestPickPrim (int fnum,
                                   CSW_F x,
                                   CSW_F y,
                                   int *prim_index,
                                   int *prim_type)
{
    FRameStruct   *frptr;
    int           save_fnum, index, patch_save;
    double        pdmax, dx, dy;
    CSW_F         pdist;

    auto fscope = [&]()
    {
        patch_pick_flag = 0;
        free_pick_lists ();
        patch_draw_flag = patch_save;
        current_frame_num = save_fnum;
        update_frame_limits ();
        SetupSpatialIndexForFrame (save_fnum);
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
 * Initialize return parameters
 */
    *prim_index = -1;
    *prim_type = -1;

/*
 * Make sure the frame is valid.
 */
    if (frame_list == NULL) {
        return -1;
    }
    if (fnum < 0  ||  fnum >= num_frame_list) {
        return -1;
    }

    frptr = frame_list + fnum;

    dx = (frptr->x2 - frptr->x1);
    dy = (frptr->y2 - frptr->y1);
    pdmax = dx * dx + dy * dy;
    pdmax = sqrt (pdmax);
    pdmax /= 100.0;

/*
 * temporarily switch the current frame num and
 * update the frame clipping limits.
 */
    save_fnum = current_frame_num;
    current_frame_num = fnum;
    update_frame_limits ();
    SetupSpatialIndexForFrame (fnum);

    patch_save = patch_draw_flag;
    patch_draw_flag = 1;

    Pickx1 = (CSW_F)(x - frptr->xspace * 1.5);
    Picky1 = (CSW_F)(y - frptr->yspace * 1.5);
    Pickx2 = (CSW_F)(x + frptr->xspace * 1.5);
    Picky2 = (CSW_F)(y + frptr->yspace * 1.5);

    free_pick_lists ();
    patch_pick_flag = 1;
/*
 * find the type and index of the closest frame scaleable primitive
 * All of the distance checking should be done using page coordinates,
 * since the frame coordinates can differ wildly in x and y scaling.
 */
    pdist = 1.e30f;

    convert_frame_point (&x, &y);

    index = -1;
    closest_frame_line (fnum, x, y, &index, &pdist);
    if (index >= 0) {
        *prim_index = index;
        *prim_type = 1;
    }
    index = -1;
    closest_frame_fill_border (fnum, x, y, &index, &pdist);
    if (index >= 0) {
        *prim_index = index;
        *prim_type = 2;
    }
    index = -1;
    closest_frame_text (fnum, x, y, &index, &pdist);
    if (index >= 0) {
        *prim_index = index;
        *prim_type = 3;
    }
    index = -1;
    closest_frame_symb (fnum, x, y, &index, &pdist);
    if (index >= 0) {
        *prim_index = index;
        *prim_type = 4;
    }
    index = -1;
    closest_frame_shape_border (fnum, x, y, &index, &pdist);
    if (index >= 0) {
        *prim_index = index;
        *prim_type = 5;
    }
    index = -1;
    closest_frame_contour (fnum, x, y, &index, &pdist);
    if (index >= 0) {
        *prim_index = index;
        *prim_type = 6;
    }

    index = -1;
    closest_frame_axis (fnum, x, y, &index, &pdist);
    if (index >= 0) {
        *prim_index = index;
        *prim_type = 7;
    }

    if (pdist <= pdmax) {
        return 1;
    }

/*
 * If no outline primitive was close enough to be selected,
 * check if the cursor is inside any shape or polygon fill.
 */
    index = -1;
    closest_frame_shape (fnum, x, y, &index);
    if (index >= 0) {
        *prim_index = index;
        *prim_type = 5;
        return 1;
    }

    index = -1;
    closest_frame_fill (fnum, x, y, &index);
    if (index >= 0) {
        *prim_index = index;
        *prim_type = 2;
        return 1;
    }

/*
 * The cursor is not close enough to anything to select it.
 */

    return -1;
}


void CDisplayList::closest_frame_contour (int fnum, CSW_F xin, CSW_F yin,
                                          int *indexout, CSW_F *pdistout)
{
    DLContour           *contour;
    int                 istat, nprim, ido, i, npts, index;
    COntourOutputRec    *crec;
    CSW_F               dist, dmin;

    *indexout = -1;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return;
    }

    contour_line_pick_list.clear();

    istat = PopulateContourLinePatches (Pickx1, Picky1, Pickx2, Picky2);

    int      pt_size = (int)contour_line_pick_list.size();
    int      *pt_data = contour_line_pick_list.data();

    nprim = cl_size;
    if (istat == 1) {
        nprim = pt_size;
    }

    dmin = *pdistout;
    index = -1;

    for (ido=0; ido<nprim; ido++) {

        i = ido;
        if (pt_size > 0  &&
            pt_data != NULL  &&
            patch_draw_flag == 1) {
            i = pt_data[ido];
        }

        contour = cl_data[i];
        if (contour == NULL) {
            continue;
        }

        if (contour->selectable_object_num < 0) {
            continue;
        }

        if (contour->deleted_flag == 1) {
            continue;
        }

        if (contour->frame_num != fnum) {
            continue;
        }

        crec = contour->GetCrec ();
        if (crec == NULL) {
            continue;
        }

        npts = crec->npts;
        if (npts < 2) {
            continue;
        }

        gpf_calclinedist1xy (crec->x, crec->y, npts,
                             xin, yin, &dist);

        if (dist < dmin) {
            dmin = dist;
            index = i;
        }

    }

    if (index >= 0  &&  dmin < 1.e20) {
        *indexout = index;
        *pdistout = dmin;
    }

    return;

}  /*  end of function closest_frame_contour */



void CDisplayList::closest_frame_line (int fnum, CSW_F xin, CSW_F yin,
                                       int *indexout, CSW_F *pdistout)
{
    LInePrim            *lptr;
    int                 istat, nprim, ido, i, index;
    CSW_F               dist, dmin;
    CSW_F               *xyp;
    int                 maxpts, npts, nout;

    *indexout = -1;

    int             lp_size = (int)line_prim_list.size();
    LInePrim        *lp_data = line_prim_list.data();

    if (lp_data == NULL  ||  lp_size < 1) {
        return;
    }

    line_pick_list.clear();

    istat = PopulateLinePatches (Pickx1, Picky1, Pickx2, Picky2);

    int       pt_size = (int)line_pick_list.size();
    int       *pt_data = line_pick_list.data();

    nprim = lp_size;
    if (istat == 1) {
        nprim = pt_size;
    }

    dmin = *pdistout;
    index = -1;

    for (ido=0; ido<nprim; ido++) {

        i = ido;
        if (pt_size > 0  &&
            pt_data != NULL  &&  pt_size > 0  &&
            patch_draw_flag == 1) {
            i = pt_data[ido];
        }

        lptr = lp_data + i;
        if (lptr == NULL) {
            continue;
        }

        if (lptr->selectable_object_num < 0) {
            continue;
        }

        if (lptr->deleted_flag == 1) {
            continue;
        }

        if (lptr->frame_num != fnum) {
            continue;
        }

        if (lptr->scaleable != 1) {
            continue;
        }

        if (lptr->npts < 2) {
            continue;
        }

        if (lptr->smooth_flag <= 0) {
            xyp = lptr->xypts;
            npts = lptr->npts;
        }
        else {
            if (xywork == NULL) {
                return;
            }
            maxpts = lptr->npts * lptr->smooth_flag;
            if (maxpts > _DL_MAX_WORK_) maxpts = _DL_MAX_WORK_;
            istat = gpf_spline_obj.gpf_SplineFit2 (lptr->xypts, lptr->npts, 0,
                                    xywork, maxpts, 0.0,
                                    &nout);
            if (istat == -1) {
                xyp = lptr->xypts;
                npts = lptr->npts;
            }
            else {
                xyp = xywork;
                npts = nout;
            }
        }

        convert_frame_array (xyp, npts);
        gpf_calclinedist1 (xyp, npts,
                           xin, yin, &dist);
        unconvert_frame_array (xyp, npts);

        if (dist < dmin) {
            dmin = dist;
            index = i;
        }

    }

    if (index >= 0  &&  dmin < 1.e20) {
        *indexout = index;
        *pdistout = dmin;
    }

    return;

}  /*  end of function closest_frame_line */




void CDisplayList::closest_frame_fill_border (int fnum, CSW_F xin, CSW_F yin,
                                              int *indexout, CSW_F *pdistout)
{
    int            istat, i, nprim, ido, inside;
    int            j, n, ntot, index;
    CSW_F          dist, dmin;
    FIllPrim       *fptr;
    CSW_F          **xpc, **ypc;

    int            fp_size = (int)fill_prim_list.size();
    FIllPrim       *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    fill_pick_list.clear();

    istat = PopulateFillPatches (Pickx1, Picky1, Pickx2, Picky2);

    int        pt_size = (int)fill_pick_list.size();
    int        *pt_data = fill_pick_list.data();

    nprim = fp_size;
    if (istat == 1) {
        nprim = pt_size;
    }

    dmin = *pdistout;
    index = -1;

    for (ido=0; ido<nprim; ido++) {

        i = ido;
        if (pt_size > 0  &&  pt_data != NULL  &&
            patch_draw_flag == 1) {
            i = pt_data[ido];
        }

        fptr = fp_data + i;

        if (fptr->selectable_object_num < 0) {
            continue;
        }

        if (fptr->deleted_flag == 1) {
            continue;
        }

        if (fptr->frame_num != fnum) {
            continue;
        }

        if (fptr->scaleable != 1) {
            continue;
        }

        if (fptr->x_orig == NULL  ||
            fptr->y_orig == NULL  ||
            fptr->npts_orig == NULL  ||
            fptr->ncomp_orig < 1) {
            continue;
        }

    /*
     * Check if the point is inside the fill.  If it is, then
     * the distance will be made smaller.
     */
        xpc = (CSW_F **)csw_Calloc (2 * fptr->ncomp_orig * sizeof(CSW_F *));
        if (xpc == NULL) {
            continue;
        }
        ypc = xpc + fptr->ncomp_orig;
        n = 0;
        for (j=0; j<fptr->ncomp_orig; j++) {
            xpc[j] = fptr->x_orig + n;
            ypc[j] = fptr->y_orig + n;
            n += fptr->npts_orig[j];
        }

        ntot = n;
        convert_frame_array (fptr->x_orig, fptr->y_orig, ntot);
        inside = ply_utils_obj.ply_pointpa1 (xpc,
                               ypc,
                               fptr->ncomp_orig,
                               fptr->npts_orig,
                               xin,
                               yin);
        csw_Free (xpc);
        xpc = NULL;
        ypc = NULL;

        n = 0;
        for (j=0; j<fptr->ncomp_orig; j++) {
            gpf_calclinedist1xy (fptr->x_orig+n, fptr->y_orig+n,
                                 fptr->npts_orig[j],
                                 xin, yin, &dist);
            if (inside >= 0) {
                dist /= 5.0;
            }

            if (dist < dmin) {
                dmin = dist;
                index = i;
            }
            n += fptr->npts_orig[j];
        }
        unconvert_frame_array (fptr->x_orig, fptr->y_orig, ntot);

    }

    if (index >= 0  &&  dmin < 1.e20) {
        *indexout = index;
        *pdistout = dmin;
    }

    return;

}  /*  end of function closest_frame_fill_border */



void CDisplayList::closest_frame_text (int fnum, CSW_F xin, CSW_F yin,
                                       int *indexout, CSW_F *pdistout)
{
    int            istat, ido, i, nprim, index;
    TExtPrim       *tptr;
    CSW_F          fsize, width, xt, yt;
    CSW_F          cx[5], cy[5], dist, dmin;
    CSW_F          bounds[10];

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }

//  Using patches to search for "local" text is a bit tricky.
//  Since text is drawn to be close to the same size on the screen
//  regardless of drawing scaling or zoom, then a piece of text 
//  can be outside of the pick aperture.  If there are a small
//  number of text primitives relative to the number of spatial
//  index grid cells, I don't bother with the patch stuff.

    bool bpatch = false;
    nprim = tp_size;

    int  nst = num_selectable_text;
    if (nst > tp_size) {
        nst = tp_size;
    }

    double    pmult = 1.5;

    int        pt_size = 0;
    int        *pt_data = 0;

    double   px1, py1, px2, py2;
    double   pw = (Pickx2 - Pickx1);
    double   ph = (Picky2 - Picky1);
    pw = (pw + ph) / 2.0;

    if (pw < index_xspace) pw = index_xspace;
    if (pw < index_yspace) pw = index_yspace;

    px1 = Pickx1 - pw * pmult;
    px2 = Pickx2 + pw * pmult;
    py1 = Picky1 - pw * pmult;
    py2 = Picky2 + pw * pmult;

    text_pick_list.clear();

    istat = PopulateTextPatches (px1, py1, px2, py2);

    pt_size = (int)text_pick_list.size();
    pt_data = text_pick_list.data();

    if (istat == 1  &&  pt_size > 0  &&
        pt_data != NULL) {
        nprim = pt_size;
        bpatch = true;
    }

    dmin = 1.e30;
    index = -1;

    for (ido=0; ido<nprim; ido++) {

        i = ido;
        if (bpatch) {
            i = pt_data[ido];
        }

        tptr = tp_data + i;

        if (tptr->selectable_object_num < 0) {
            continue;
        }

        if (tptr->deleted_flag == 1) {
            continue;
        }

        if (tptr->frame_num != fnum) {
            continue;
        }

        if (tptr->scaleable != 1) {
            continue;
        }

    /*
     * Get the text bounds in inches.
     */
        gtx_GetTextBounds (tptr->chardata,
                           tptr->font_num,
                           tptr->size,
                           bounds);

    /*
     * Convert the text bounds and position to page units
     * and find the rectangle around the text in page units.
     */
        xt = tptr->x;
        yt = tptr->y;
        if (tptr->scaleable != 0) {
            convert_frame_point (&xt, &yt);
        }
        width = bounds[0] * page_units_per_inch;
        fsize = bounds[1] * page_units_per_inch;

        gpf_calcdraw_obj.gpf_find_box_corners (xt, yt,
                              tptr->anchor,
                              width, fsize, tptr->angle,
                              cx, cy);
        cx[4] = cx[0];
        cy[4] = cy[0];

        gpf_calclinedist1xy (cx, cy, 5,
                             xin, yin, &dist);

    /*
     * If the pick point is inside the text bounding box,
     * make the distance much smaller
     */
        istat = ply_utils_obj.ply_point1 (cx, cy, 5,
                            xin, yin);
        if (istat == 1) {
            dist /= 10.0;
        }

        if (dist < dmin) {
            dmin = dist;
            index = i;
        }

    }

    unconvert_frame_dist (fnum, &dmin);
    if (dmin > *pdistout) {
        return;
    }

    if (index >= 0  &&  dmin < 1.e20) {
        *indexout = index;
        *pdistout = dmin;
    }

    return;

}  /*  end of function closest_frame_text */





void CDisplayList::closest_frame_symb (int fnum, CSW_F xin, CSW_F yin,
                                       int *indexout, CSW_F *pdistout)
{
    int            istat, ido, i, nprim, index;
    CSW_F          dist, dmin, xt, yt;;
    SYmbPrim       *sptr;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return;
    }

    symb_pick_list.clear();

    istat = PopulateSymbPatches (Pickx1, Picky1, Pickx2, Picky2);

    int         pt_size = (int)symb_pick_list.size();
    int         *pt_data = symb_pick_list.data();

    nprim = sp_size;
    if (istat == 1) {
        nprim = pt_size;
    }

    dmin = *pdistout;
    index = -1;

    for (ido=0; ido<nprim; ido++) {

        i = ido;
        if (pt_size > 0  &&  pt_data != NULL  &&
            patch_draw_flag == 1) {
            i = pt_data[ido];
        }

        sptr = sp_data + i;

        if (sptr->selectable_object_num < 0) {
            continue;
        }

        if (sptr->deleted_flag == 1) {
            continue;
        }

        if (sptr->frame_num != fnum) {
            continue;
        }

        if (sptr->scaleable != 1) {
            continue;
        }

        xt = sptr->x;
        yt = sptr->y;
        convert_frame_point (&xt, &yt);
        gpf_calcdistance1 (xin, yin, xt, yt, &dist);
        if (dist <= sptr->size * .7) {
            index = i;
            dmin = 0.0;
        }
        else if (dist < dmin) {
            dmin = dist;
            index = i;
        }
    }

    if (index >= 0  &&  dmin < 1.e20) {
        *indexout = index;
        *pdistout = dmin;
    }

    return;

}  /*  end of function closest_frame_symb */




void CDisplayList::closest_frame_shape_border (int fnum, CSW_F xin, CSW_F yin,
                                               int *indexout, CSW_F *pdistout)
{
    int            istat, ido, i, nprim, index;
    CSW_F          xc, yc, w, h, ang;
    CSW_F          cx[5], cy[5], dist, dmin;
    SHapePrim      *shptr;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return;
    }

    shape_pick_list.clear();

    istat = PopulateShapePatches (Pickx1, Picky1, Pickx2, Picky2);

    int        pt_size = (int)shape_pick_list.size();
    int        *pt_data = shape_pick_list.data();

    nprim = hp_size;
    if (istat == 1) {
        nprim = pt_size;
    }

    index = -1;
    dmin = *pdistout;

    for (ido=0; ido<nprim; ido++) {

        i = ido;
        if (pt_size > 0  &&  pt_data != NULL  &&
            patch_draw_flag == 1) {
            i = pt_data[ido];
        }

        shptr = hp_data + i;

        if (shptr->selectable_object_num < 0) {
            continue;
        }

        if (shptr->deleted_flag == 1) {
            continue;
        }

        if (shptr->frame_num != fnum) {
            continue;
        }

        if (shptr->scaleable != 1) {
            continue;
        }

    /*
     * Get the rectangular boundary of the rotated shape and use
     * it for distance calculations.
     */
        xc = shptr->fval[0];
        yc = shptr->fval[1];
        w = shptr->fval[2];
        h = shptr->fval[3];
        ang = shptr->fval[6];
        if (shptr->type == 1) {
            w /= 2.0;
            h /= 2.0;
            ang = shptr->fval[5];
        }
        w *= 2.0f;
        h *= 2.0f;

        gpf_calcdraw_obj.gpf_find_box_corners (xc, yc, 5,
                              w, h, ang,
                              cx, cy);
        cx[4] = cx[0];
        cy[4] = cy[0];

        convert_frame_array (cx, cy, 5);
        gpf_calclinedist1xy (cx, cy, 5,
                             xin, yin, &dist);
        if (dist < dmin) {
            dmin = dist;
            index = i;
        }

    }

    if (index >= 0  &&  dmin < 1.e20) {
        *indexout = index;
        *pdistout = dmin;
    }

    return;

}  /*  end of function closest_frame_shape_border */




void CDisplayList::closest_frame_fill (int fnum, CSW_F xin, CSW_F yin,
                                       int *indexout)
{
    int            i, nprim, ido, istat;
    int            index, j, n;
    FIllPrim       *fptr;
    CSW_F          **xpc, **ypc;

    int            fp_size = (int)fill_prim_list.size();
    FIllPrim       *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    fill_pick_list.clear();

    istat = PopulateFillPatches (Pickx1, Picky1, Pickx2, Picky2);

    int        pt_size = (int)fill_pick_list.size();
    int        *pt_data = fill_pick_list.data();

    nprim = fp_size;
    if (istat == 1) {
        nprim = pt_size;
    }

    index = -1;

    for (ido=0; ido<nprim; ido++) {

        i = ido;
        if (pt_size > 0  &&  pt_data != NULL  &&
            patch_draw_flag == 1) {
            i = pt_data[ido];
        }

        fptr = fp_data + i;

        if (fptr->selectable_object_num < 0) {
            continue;
        }

        if (fptr->deleted_flag == 1) {
            continue;
        }

        if (fptr->frame_num != fnum) {
            continue;
        }

        if (fptr->scaleable != 1) {
            continue;
        }

    /*
     * If the polygon is not color or pattern filled, then it is
     * not a candidate for selection via its interior.
     */
        if (fptr->pattern <= 0) {
            if (fptr->fill_red < 0  ||
                fptr->fill_green < 0  ||
                fptr->fill_blue < 0) {
                continue;
            }
        }
        else {
            if (fptr->pat_red < 0  ||
                fptr->pat_green < 0  ||
                fptr->pat_blue < 0) {
                continue;
            }
        }

        if (fptr->x_orig == NULL  ||
            fptr->y_orig == NULL  ||
            fptr->npts_orig == NULL  ||
            fptr->ncomp_orig < 1) {
            continue;
        }

        xpc = (CSW_F **)csw_Calloc (2 * fptr->ncomp_orig * sizeof(CSW_F *));
        if (xpc == NULL) {
            continue;
        }
        ypc = xpc + fptr->ncomp_orig;
        n = 0;
        for (j=0; j<fptr->ncomp_orig; j++) {
            xpc[j] = fptr->x_orig + n;
            ypc[j] = fptr->y_orig + n;
            n += fptr->npts_orig[j];
        }

        convert_frame_array (fptr->x_orig, fptr->y_orig, n);

        istat = ply_utils_obj.ply_pointpa1 (xpc,
                              ypc,
                              fptr->ncomp_orig,
                              fptr->npts_orig,
                              xin,
                              yin);

        unconvert_frame_array (fptr->x_orig, fptr->y_orig, n);

        csw_Free (xpc);
        xpc = NULL;
        ypc = NULL;
        if (istat >= 0) {
            index = i;
            break;
        }

    }

    if (index >= 0) {
        *indexout = index;
    }

    return;

}  /*  end of function closest_frame_fill */




void CDisplayList::closest_frame_shape (int fnum, CSW_F xin, CSW_F yin,
                                        int *indexout)
{
    int            ido, i, nprim, index, istat;
    CSW_F          xc, yc, w, h, ang;
    CSW_F          cx[5], cy[5];
    SHapePrim      *shptr;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return;
    }

    shape_pick_list.clear();

    istat = PopulateShapePatches (Pickx1, Picky1, Pickx2, Picky2);

    int        pt_size = (int)shape_pick_list.size();
    int        *pt_data = shape_pick_list.data();

    nprim = hp_size;
    if (istat == 1) {
        nprim = pt_size;
    }

    index = -1;

    for (ido=0; ido<nprim; ido++) {

        i = ido;
        if (pt_size > 0  &&  pt_data != NULL  &&
            patch_draw_flag == 1) {
            i = pt_data[ido];
        }

        shptr = hp_data + i;

        if (shptr->selectable_object_num < 0) {
            continue;
        }

        if (shptr->deleted_flag == 1) {
            continue;
        }

        if (shptr->frame_num != fnum) {
            continue;
        }

        if (shptr->scaleable != 1) {
            continue;
        }

    /*
     * If the shape is not color or pattern filled, then it is
     * not a candidate for selection via its interior.
     */
        if (shptr->pattern <= 0) {
            if (shptr->fill_red < 0  ||
                shptr->fill_green < 0  ||
                shptr->fill_blue < 0) {
                continue;
            }
        }
        else {
            if (shptr->pat_red < 0  ||
                shptr->pat_green < 0  ||
                shptr->pat_blue < 0) {
                continue;
            }
        }

    /*
     * Get the rectangular boundary of the rotated shape and use
     * it for distance calculations.
     */
        xc = shptr->fval[0];
        yc = shptr->fval[1];
        w = shptr->fval[2];
        h = shptr->fval[3];
        ang = shptr->fval[6];
        if (shptr->type == 1) {
            w /= 2.0;
            h /= 2.0;
            ang = shptr->fval[5];
        }
        w *= 2.0f;
        h *= 2.0f;

        gpf_calcdraw_obj.gpf_find_box_corners (xc, yc, 5,
                              w, h, ang,
                              cx, cy);
        cx[4] = cx[0];
        cy[4] = cy[0];

        convert_frame_array (cx, cy, 5);

        istat = ply_utils_obj.ply_point1 (cx, cy, 5, xin, yin);
        if (istat >= 0) {
            index = i;
            break;
        }
    }

    if (index >= 0) {
        *indexout = index;
    }

    return;

}  /*  end of function closest_frame_shape */



/*
 * Return the selected primitives to the java side.
 */
void CDisplayList::ReturnSelected (void)
{
    int            i, save_fnum;
    DLSelectable   *dls;

    if (selectable_object_list == NULL  ||
        num_selectable_object_list < 1) {
        return;
    }

    save_current_graphic_attributes ();
    save_fnum = current_frame_num;

    for (i=0; i<num_selectable_object_list; i++) {
        dls = selectable_object_list[i];
        if (dls == NULL) {
            continue;
        }
        if (dls->deleted_flag == 1) {
            continue;
        }
        if (dls->GetSelectState () == 0) {
            continue;
        }

        return_selected_lines (dls);
        return_selected_fills (dls);
        return_selected_texts (dls);
        return_selected_symbs (dls);
        return_selected_rectangles (dls);
        return_selected_arcs (dls);
        return_selected_contours (dls);
        return_selected_axes (dls);

    }

    unsave_current_graphic_attributes ();
    current_frame_num = save_fnum;
    update_frame_limits ();
}

void CDisplayList::return_selected_lines (DLSelectable *dls)
{
    int             ido, i, nprim, *lines;
    LInePrim        *lptr;
    FRameStruct     *frptr;
    char            *fname,
                    *lname,
                    *iname;

    int             lp_size = (int)line_prim_list.size();
    LInePrim        *lp_data = line_prim_list.data();

    if (lp_data == NULL  ||  lp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumLines ();
    lines = dls->GetLines ();
    if (lines == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = lines[ido];

        lptr = lp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }

        if (frame_list == NULL  ||
            lptr->frame_num < 0) {
            continue;
        }

        frptr = frame_list + lptr->frame_num;
        fname = frptr->name;

        lname = NULL;
        if (lptr->layer_num >= 0  &&  layer_list != NULL) {
            lname = layer_list[lptr->layer_num].name;
        }

        iname = NULL;
        if (lptr->item_num >= 0  &&  item_list != NULL) {
            iname = item_list[lptr->item_num].name;
        }

        ezx_java_obj.ezx_AppendSelectedLineToJavaArea (lptr,
                                          fname,
                                          lname,
                                          iname);
    }

}


void CDisplayList::return_selected_fills (DLSelectable *dls)
{
    int             ido, i, nprim, *fills;
    FIllPrim        *fptr;
    FRameStruct     *frptr;
    char            *fname,
                    *lname,
                    *iname;

    int             fp_size = fill_prim_list.size();
    FIllPrim        *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumFills ();
    fills = dls->GetFills ();
    if (fills == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = fills[ido];

        fptr = fp_data + i;

        if (fptr->deleted_flag == 1) {
            continue;
        }

        if (frame_list == NULL  ||
            fptr->frame_num < 0) {
            continue;
        }

        frptr = frame_list + fptr->frame_num;
        fname = frptr->name;

        lname = NULL;
        if (fptr->layer_num >= 0  &&  layer_list != NULL) {
            lname = layer_list[fptr->layer_num].name;
        }

        iname = NULL;
        if (fptr->item_num >= 0  &&  item_list != NULL) {
            iname = item_list[fptr->item_num].name;
        }

        ezx_java_obj.ezx_AppendSelectedFillToJavaArea (fptr,
                                          fname,
                                          lname,
                                          iname);
    }

}


void CDisplayList::return_selected_texts (DLSelectable *dls)
{

    int             ido, i, nprim, *texts;
    TExtPrim        *tptr;
    FRameStruct     *frptr;
    char            *fname,
                    *lname,
                    *iname;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumTexts ();
    texts = dls->GetTexts ();
    if (texts == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = texts[ido];

        tptr = tp_data + i;

        if (tptr->deleted_flag == 1) {
            continue;
        }

        if (frame_list == NULL  ||
            tptr->frame_num < 0) {
            continue;
        }

        frptr = frame_list + tptr->frame_num;
        fname = frptr->name;

        lname = NULL;
        if (tptr->layer_num >= 0  &&  layer_list != NULL) {
            lname = layer_list[tptr->layer_num].name;
        }

        iname = NULL;
        if (tptr->item_num >= 0  &&  item_list != NULL) {
            iname = item_list[tptr->item_num].name;
        }

        ezx_java_obj.ezx_AppendSelectedTextToJavaArea (tptr,
                                          fname,
                                          lname,
                                          iname);
    }

}


void CDisplayList::return_selected_symbs (DLSelectable *dls)
{
    int             ido, i, nprim, *symbs;
    SYmbPrim        *sptr;
    FRameStruct     *frptr;
    DLSurf          *surf;
    char            *fname,
                    *lname,
                    *iname;
    char            *sname;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumSymbs ();
    symbs = dls->GetSymbs ();
    if (symbs == NULL  ||  nprim < 1) {
        return;
    }

    int        sf_size = surf_list.size();
    DLSurf     **sf_data = surf_list.data();

    for (ido=0; ido<nprim; ido++) {

        i = symbs[ido];

        sptr = sp_data + i;

        if (sptr->deleted_flag == 1) {
            continue;
        }

        if (frame_list == NULL  ||
            sptr->frame_num < 0) {
            continue;
        }

        frptr = frame_list + sptr->frame_num;
        fname = frptr->name;

        lname = NULL;
        if (sptr->layer_num >= 0  &&  layer_list != NULL) {
            lname = layer_list[sptr->layer_num].name;
        }

        iname = NULL;
        if (sptr->item_num >= 0  &&  item_list != NULL) {
            iname = item_list[sptr->item_num].name;
        }

        sname = NULL;
        if (sptr->grid_num >= 0  &&  sf_data != NULL  &&  sptr->grid_num < sf_size) {
            surf = sf_data[sptr->grid_num];
            sname = surf->name;
        }

        ezx_java_obj.ezx_AppendSelectedSymbToJavaArea (sptr,
                                          fname,
                                          lname,
                                          iname,
                                          sname);
    }

}


void CDisplayList::return_selected_rectangles (DLSelectable *dls)
{
    int             ido, i, nprim, *arcs;
    SHapePrim       *rptr;
    FRameStruct     *frptr;
    char            *fname,
                    *lname,
                    *iname;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumShapes ();
    arcs = dls->GetShapes ();
    if (arcs == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = arcs[ido];

        rptr = hp_data + i;

        if (rptr->deleted_flag == 1) {
            continue;
        }

        if (rptr->type != 1) {
            continue;
        }

        if (frame_list == NULL  ||
            rptr->frame_num < 0) {
            continue;
        }

        frptr = frame_list + rptr->frame_num;
        fname = frptr->name;

        lname = NULL;
        if (rptr->layer_num >= 0  &&  layer_list != NULL) {
            lname = layer_list[rptr->layer_num].name;
        }

        iname = NULL;
        if (rptr->item_num >= 0  &&  item_list != NULL) {
            iname = item_list[rptr->item_num].name;
        }

        ezx_java_obj.ezx_AppendSelectedRectangleToJavaArea (rptr,
                                          fname,
                                          lname,
                                          iname);
    }

}


void CDisplayList::return_selected_arcs (DLSelectable *dls)
{
    int             ido, i, nprim, *arcs;
    SHapePrim       *shptr;
    FRameStruct     *frptr;
    char            *fname,
                    *lname,
                    *iname;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumShapes ();
    arcs = dls->GetShapes ();
    if (arcs == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = arcs[ido];

        shptr = hp_data + i;

        if (shptr->deleted_flag == 1) {
            continue;
        }

        if (shptr->type != 2) {
            continue;
        }

        if (frame_list == NULL  ||
            shptr->frame_num < 0) {
            continue;
        }

        frptr = frame_list + shptr->frame_num;
        fname = frptr->name;

        lname = NULL;
        if (shptr->layer_num >= 0  &&  layer_list != NULL) {
            lname = layer_list[shptr->layer_num].name;
        }

        iname = NULL;
        if (shptr->item_num >= 0  &&  item_list != NULL) {
            iname = item_list[shptr->item_num].name;
        }

        ezx_java_obj.ezx_AppendSelectedArcToJavaArea (shptr,
                                         fname,
                                         lname,
                                         iname);
    }

}


void CDisplayList::return_selected_contours (DLSelectable *dls)
{
    int             ido, i, nprim, *conts;
    DLContour       *cptr;
    FRameStruct     *frptr;
    DLSurf          *surf;
    char            *fname,
                    *lname,
                    *iname;
    char            *sname;
    DLContourProperties    *prop;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumContours ();
    conts = dls->GetContours ();
    if (conts == NULL  ||  nprim < 1) {
        return;
    }

    int        sf_size = surf_list.size();
    DLSurf     **sf_data = surf_list.data();

    for (ido=0; ido<nprim; ido++) {

        i = conts[ido];

        cptr = cl_data[i];

        if (cptr->deleted_flag == 1) {
            continue;
        }

        if (frame_list == NULL  ||
            cptr->frame_num < 0) {
            continue;
        }

        frptr = frame_list + cptr->frame_num;
        fname = frptr->name;

        lname = NULL;
        if (cptr->layer_num >= 0  &&  layer_list != NULL) {
            lname = layer_list[cptr->layer_num].name;
        }

        iname = NULL;
        if (cptr->item_num >= 0  &&  item_list != NULL) {
            iname = item_list[cptr->item_num].name;
        }

        sname = NULL;
        prop = NULL;
        if (cptr->grid_index >= 0  &&  sf_data != NULL  &&
            cptr->grid_index < sf_size) {
            surf = sf_data[cptr->grid_index];
            sname = surf->name;
            prop = (DLContourProperties *)surf->GetContourProperties ();
        }
        if (prop == NULL) {
            continue;
        }

        ezx_java_obj.ezx_AppendSelectedContourToJavaArea (cptr,
                                             fname,
                                             lname,
                                             iname,
                                             sname,
                                             prop);
    }

}



/*
 *******************************************************************************
 *
 *  The following functions are used to draw selected primitives.
 *
 *******************************************************************************
*/
void CDisplayList::DrawSelected (void)
{
    int            i, save_fnum;
    DLSelectable   *dls;

    if (selectable_object_list == NULL  ||
        num_selectable_object_list < 1) {
        return;
    }

    save_current_graphic_attributes ();
    save_fnum = current_frame_num;
    gtx_drawprim_obj.gtx_set_selected_flag_for_drawing (1);

    for (i=0; i<num_selectable_object_list; i++) {
        dls = selectable_object_list[i];
        if (dls == NULL) {
            continue;
        }
        if (dls->deleted_flag == 1) {
            continue;
        }
        if (dls->GetSelectState () == 0) {
            continue;
        }

        ezx_java_obj.ezx_SetDrawingPriority (0);
        reclip_and_draw_selected_lines (dls);
        reclip_and_draw_selected_fills (dls);
        reclip_and_draw_selected_texts (dls);
        ezx_java_obj.ezx_SetDrawingPriority (1);
        reclip_and_draw_selected_symbs (dls);
        ezx_java_obj.ezx_SetDrawingPriority (0);
        reclip_and_draw_selected_shapes (dls);
        reclip_and_draw_selected_contours (dls);
        reclip_and_draw_selected_axes (dls);
    }

    unsave_current_graphic_attributes ();
    current_frame_num = save_fnum;
    update_frame_limits ();
    gtx_drawprim_obj.gtx_set_selected_flag_for_drawing (0);
}



void CDisplayList::reclip_and_draw_selected_lines (DLSelectable *dls)
{
    int            i, nprim, ido;
    LInePrim       *lptr;
    int            ncout, nplot, npts;
    int            *lines;
    CSW_F          *xy;

    int            lp_size = (int)line_prim_list.size();
    LInePrim       *lp_data = line_prim_list.data();

    if (lp_data == NULL  ||  lp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    if (xywork == NULL  ||
        xywork2 == NULL  ||
        iwork == NULL) {
        return;
    }

    nprim = dls->GetNumLines ();
    lines = dls->GetLines ();
    if (lines == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = lines[ido];

        lptr = lp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }

        if (frame_list == NULL  ||
            lptr->frame_num < 0) {
            continue;
        }
        current_frame_num = lptr->frame_num;
        update_frame_limits ();

    /*
     * Clip the master frame prim to the current
     * frame limits and scale to the current
     * page location.
     */
        gpf_calcdraw_obj.gpf_cliplineprim (lptr->xypts, lptr->npts,
                          Fx1, Fy1, Fx2, Fy2,
                          xywork2, &ncout, iwork);
        if (ncout < 1) {
            continue;
        }

    /*
     * If the line is smoothed, use the original points and
     * let the java side do the clipping.
     */
        if (lptr->smooth_flag > 0) {
            memcpy (xywork, lptr->xypts, lptr->npts * 2 * sizeof(CSW_F));
            nplot = lptr->npts;
        }
        else {
            gpf_calcdraw_obj.gpf_addholeflags2 (xywork2, iwork, ncout,
                               xywork, &nplot);
        }
        xy = xywork;
        npts = nplot;

        convert_frame_array (xy, npts);

        ezx_java_obj.ezx_SetFrameInJavaArea (lptr->frame_num);

        gtx_drawprim_obj.gtx_cliplineprim(xy,
                         npts,
                         lptr->smooth_flag,
                         (CSW_F)lptr->thick,
                         lptr->red,
                         lptr->green,
                         lptr->blue,
                         lptr->dashpat,
                         (CSW_F)lptr->dashscale,
                         lptr->arrowstyle);

    }

    return;

}  /*  end of function reclip_and_draw_selected_lines */



void CDisplayList::reclip_and_draw_selected_fills (DLSelectable *dls)
{
    int            i, nptot, istat, nprim, ido;
    FIllPrim       *fptr;
    int            *nc0, *fills;
    int            nc1, memflg1;
    CSW_F          *x_with_hole_flags, *y_with_hole_flags;
    CSW_F          *xt0, *yt0, *xyp;

    CSWErrNum      err_obj;

    int            fp_size = (int)fill_prim_list.size();
    FIllPrim       *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumFills ();
    fills = dls->GetFills ();

    if (fills == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = fills[ido];

        fptr = fp_data + i;

        if (fptr->deleted_flag == 1) {
            continue;
        }

        if (fptr->x_orig == NULL  ||
            fptr->y_orig == NULL  ||
            fptr->npts_orig == NULL  ||
            fptr->ncomp_orig < 1) {
            continue;
        }

        if (frame_list == NULL  ||
            fptr->frame_num < 0) {
            continue;
        }
        current_frame_num = fptr->frame_num;
        update_frame_limits ();

        istat = ply_calc_obj.ply_ClipToBox1 (err_obj,
                                fptr->x_orig, fptr->y_orig,
                                fptr->npts_orig, fptr->ncomp_orig,
                                Fx1, Fy1, Fx2, Fy2,
                                xywork, xywork2, iwork, &nc1,
                                _DL_MAX_WORK_ / 2, _DL_MAX_WORK_ / 2);
        if (istat <= 0) {
            continue;
        }
        xt0 = xywork;
        yt0 = xywork2;
        nc0 = iwork;

    /*
     *  flag the holes before putting in the display list
     */
        istat = gpf_calcdraw_obj.gpf_addholeflags (xt0, yt0, nc0, nc1,
                                  &x_with_hole_flags, &y_with_hole_flags,
                                  &nptot, &memflg1);
        if (istat != 1) {
            return;
        }
        if (nptot < 3) {
            return;
        }

        xyp = (CSW_F *)csw_Malloc (2 * nptot * sizeof(CSW_F));
        if (xyp == NULL) {
            return;
        }
        gpf_packxy2 (x_with_hole_flags, y_with_hole_flags, nptot,
                     xyp);

      /*
       * convert from frame units to page units
       */
        convert_frame_array (xyp, nptot);

      /*
       * The addholeflags function may return pointers to private
       * memory.  If this is the case do not csw_Free the returned
       * memory.  However, always set the pointers to NULL.
       */
        if (memflg1) {
            csw_Free (x_with_hole_flags);
            csw_Free (y_with_hole_flags);
        }
        x_with_hole_flags = NULL;
        y_with_hole_flags = NULL;

        ezx_java_obj.ezx_SetFrameInJavaArea (fptr->frame_num);

        gtx_drawprim_obj.gtx_clipfillprim(xyp,
                         nptot,
                         fptr->smooth_flag,
                         fptr->fill_red,
                         fptr->fill_green,
                         fptr->fill_blue,
                         fptr->pat_red,
                         fptr->pat_green,
                         fptr->pat_blue,
                         fptr->border_red,
                         fptr->border_green,
                         fptr->border_blue,
                         fptr->thick,
                         fptr->outline,
                         fptr->pattern,
                         fptr->patscale,
                         fptr->linepatt,
                         fptr->dashscale,
                         0.0f, 0.0f, 0.0f, 0.0f
                         );
        csw_Free (xyp);
        xyp = NULL;
    }

    return;

}  /*  end of function reclip_and_draw_selected_fills */



void CDisplayList::reclip_and_draw_selected_texts (DLSelectable *dls)
{
    int            ido, i, istat, nprim;
    TExtPrim       *tptr;
    CSW_F          x, y, xt, yt;
    char           textout[500];
    int            *texts;
    CSW_F          fsize, xzero, yzero, fzero;
    CSW_F          fx1, fy1, fx2, fy2;
    CSW_F          xoff, yoff;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }
    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumTexts ();
    texts = dls->GetTexts ();

    for (ido=0; ido<nprim; ido++) {

        i = texts[ido];
        tptr = tp_data + i;

        if (tptr->deleted_flag == 1) {
            continue;
        }

        if (frame_list == NULL  ||
            tptr->frame_num < 0) {
            continue;
        }
        current_frame_num = tptr->frame_num;
        update_frame_limits ();

    /*
     * Convert the text size in inches to a text size in
     * frame units.
     */
        fsize = tptr->size * page_units_per_inch;
        xzero = Px1;
        yzero = Py1;
        fsize += Px1;
        fzero = Py1;
        unconvert_frame_point (&xzero, &yzero);
        unconvert_frame_point (&fsize, &fzero);
        fsize -= xzero;
        if (fsize < 0.0) fsize = -fsize;
        if (Fx2 > Fx1) {
            fx1 = Fx1 - 2.0f * fsize;
            fx2 = Fx2 + 2.0f * fsize;
        }
        else {
            fx1 = Fx1 + 2.0f * fsize;
            fx2 = Fx2 - 2.0f * fsize;
        }
        if (Fy2 > Fy1) {
            fy1 = Fy1 - 2.0f * fsize;
            fy2 = Fy2 + 2.0f * fsize;
        }
        else {
            fy1 = Fy1 + 2.0f * fsize;
            fy2 = Fy2 - 2.0f * fsize;
        }
        istat = gpf_calcdraw_obj.gpf_cliptextitem
            (tptr->x, tptr->y,
             tptr->anchor,
             tptr->chardata,
             tptr->angle, fsize,
             tptr->font_num,
             fx1, fy1, fx2, fy2,
             textout, &xt, &yt);
        if (istat < 0) {
            continue;
        }
        x = xt;
        y = yt;
        if (strlen (textout) < 1) {
            continue;
        }

        convert_frame_point (&x, &y);

        ezx_java_obj.ezx_SetFrameInJavaArea (tptr->frame_num);

        gtx_drawprim_obj.gtx_reset_exact_text_length ();

        xoff = tptr->xoff * page_units_per_inch;
        yoff = tptr->yoff * page_units_per_inch;

     /*
      * No background rectangle is drawn for selected text because
      * it would be the same color as the text characters.
      */
        if (tptr->bgflag != 0) {
/*
            ezx_java_obj.ezx_SetDrawingPriority (2);
            gtx_drawprim_obj.gtx_cliptextrect(x + xoff,
                             y + yoff,
                             textout,
                             tptr->anchor,
                             tptr->bg_thick,
                             tptr->bg_border_red,
                             tptr->bg_border_green,
                             tptr->bg_border_blue,
                             tptr->bg_red,
                             tptr->bg_green,
                             tptr->bg_blue,
                             tptr->angle,
                             tptr->size,
                             tptr->font_num,
                             tptr->bgflag);
*/
        }

     /*
      * The text character fills and strokes themselves have the
      * highest priority and are set to 3.
      */
        ezx_java_obj.ezx_SetDrawingPriority (3);
        gtx_drawprim_obj.gtx_cliptextprim(x + xoff,
                         y + yoff,
                         textout,
                         tptr->anchor,
                         tptr->thick,
                         tptr->border_red,
                         tptr->border_green,
                         tptr->border_blue,
                         tptr->fill_red,
                         tptr->fill_green,
                         tptr->fill_blue,
                         tptr->angle,
                         tptr->size,
                         tptr->font_num);

        gtx_drawprim_obj.gtx_reset_exact_text_length ();

    }

    return;

}  /*  end of function reclip_and_draw_selected_texts */



void CDisplayList::reclip_and_draw_selected_symbs (DLSelectable *dls)
{
    int            ido, i, nprim;
    int            *symbs;
    CSW_F          xt, yt, dt1, dt2;
    SYmbPrim       *sptr;
    CSW_F          xzero, yzero, fsize, fzero;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return;
    }
    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumSymbs ();
    symbs = dls->GetSymbs ();
    if (nprim < 1  ||  symbs == NULL) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = symbs[ido];
        sptr = sp_data + i;

        if (sptr->deleted_flag == 1) {
            continue;
        }

        if (frame_list == NULL  ||
            sptr->frame_num < 0) {
            continue;
        }
        current_frame_num = sptr->frame_num;
        update_frame_limits ();

    /*
     * Convert the symbol size in inches to a symbol size in
     * frame units.
     */
        fsize = sptr->size * page_units_per_inch;
        xzero = Px1;
        yzero = Py1;
        fsize += Px1;
        fzero = Py1;
        unconvert_frame_point (&xzero, &yzero);
        unconvert_frame_point (&fsize, &fzero);
        fsize -= xzero;
        if (fsize < 0.0) fsize = -fsize;

    /*
     * If the entire bounding box is outside the frame
     * don't draw the symbol.
     */
        fsize /= 2.0f;
        xt = sptr->x;
        yt = sptr->y;

        dt1 = xt - fsize;
        dt2 = xt + fsize;
        if ((Fx1 - dt1) * (dt1 - Fx2) < 0.0  &&
            (Fx1 - dt2) * (dt2 - Fx2) < 0.0) {
            continue;
        }

        dt1 = yt - fsize;
        dt2 = yt + fsize;
        if ((Fy1 - dt1) * (dt1 - Fy2) < 0.0  &&
            (Fy1 - dt2) * (dt2 - Fy2) < 0.0) {
            continue;
        }

        convert_frame_point (&xt, &yt);

        ezx_java_obj.ezx_SetFrameInJavaArea (sptr->frame_num);

        gtx_drawprim_obj.gtx_clipsymbprim(xt,
                         yt,
                         sptr->symb_num,
                         sptr->size,
                         sptr->thick,
                         sptr->red,
                         sptr->green,
                         sptr->blue,
                         sptr->angle,
                         sptr->mask_flag);

    }

    return;

}  /*  end of function reclip_and_draw_selected_symbs */



void CDisplayList::reclip_and_draw_selected_shapes (DLSelectable *dls)
{
    int            ido, i, nprim;
    int            *shapes;
    SHapePrim      *shptr;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return;
    }
    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumShapes ();
    shapes = dls->GetShapes ();
    if (nprim < 1  ||  shapes == NULL) {
        return;
    }

    bool  bframe = false;
    for (ido=0; ido<nprim; ido++) {

        i = shapes[ido];
        shptr = hp_data + i;

        if (shptr->deleted_flag == 1) {
            continue;
        }

        if (frame_list == NULL  ||
            shptr->frame_num < 0) {
            continue;
        }
        current_frame_num = shptr->frame_num;
        update_frame_limits ();
        bframe = false;
        if (current_frame_num >= 0) bframe = true;

        ezx_java_obj.ezx_SetFrameInJavaArea (shptr->frame_num);

        switch (shptr->type) {

        /*
            rectangle
        */
            case 1:

                if (bframe) {
                  // center point
                    convert_frame_point (shptr->frame_num,
                                         shptr->fval+0,
                                         shptr->fval+1);
                  // width
                    convert_frame_dist (shptr->frame_num,
                                        shptr->fval+2);
                  // height
                    convert_frame_dist (shptr->frame_num,
                                        shptr->fval+3);
                }

                gtx_drawprim_obj.gtx_cliprectprim (
                                  shptr->fval[0],
                                  shptr->fval[1],
                                  shptr->fval[2],
                                  shptr->fval[3],
                                  shptr->fval[5],
                                  shptr->fval[4],
                                  shptr->fill_red,
                                  shptr->fill_green,
                                  shptr->fill_blue,
                                  shptr->pat_red,
                                  shptr->pat_green,
                                  shptr->pat_blue,
                                  shptr->border_red,
                                  shptr->border_green,
                                  shptr->border_blue,
                                  shptr->thick,
                                  shptr->pattern,
                                  shptr->patscale,
                                  shptr->linepatt,
                                  shptr->dashscale);

                if (bframe) {
                  // center point
                    unconvert_frame_point (shptr->frame_num,
                                           shptr->fval+0,
                                           shptr->fval+1);
                  // width
                    unconvert_frame_dist (shptr->frame_num,
                                          shptr->fval+2);
                  // height
                    unconvert_frame_dist (shptr->frame_num,
                                          shptr->fval+3);
                }

                break;

        /*
            arc
        */
            case 2:

                if (bframe) {
                  // center point
                    convert_frame_point (shptr->frame_num,
                                         shptr->fval+0,
                                         shptr->fval+1);
                  // r1
                    convert_frame_dist (shptr->frame_num,
                                        shptr->fval+2);
                  // r2
                    convert_frame_dist (shptr->frame_num,
                                        shptr->fval+3);
                }

                gtx_drawprim_obj.gtx_cliparcprim (
                                 shptr->fval[0],
                                 shptr->fval[1],
                                 shptr->fval[2],
                                 shptr->fval[3],
                                 shptr->fval[4],
                                 shptr->fval[5],
                                 shptr->fval[6],
                                 (int)(shptr->fval[7]+.01),
                                 shptr->fill_red,
                                 shptr->fill_green,
                                 shptr->fill_blue,
                                 shptr->pat_red,
                                 shptr->pat_green,
                                 shptr->pat_blue,
                                 shptr->border_red,
                                 shptr->border_green,
                                 shptr->border_blue,
                                 shptr->thick,
                                 shptr->pattern,
                                 shptr->patscale,
                                 shptr->linepatt,
                                 shptr->dashscale);

                if (bframe) {
                  // center point
                    unconvert_frame_point (shptr->frame_num,
                                           shptr->fval+0,
                                           shptr->fval+1);
                  // r1
                    unconvert_frame_dist (shptr->frame_num,
                                          shptr->fval+2);
                  // r2
                    unconvert_frame_dist (shptr->frame_num,
                                          shptr->fval+3);
                }

                break;

            default:
                return;

        }  /* end of switch on shape type */

    }  /* end of loop through shapes */

    return;

}  /*  end of function reclip_and_draw_selected_shapes */




void CDisplayList::reclip_and_draw_selected_contours (DLSelectable *dls)
{
    DLContour           *contour, *page_con;
    DLContourProperties *conprop;
    int                 *contours;
    int                 grid_num;
    int                 nprim, ido, i, j, k, n, npts, istat, ncout, np2;
    DLSurf              *grid;
    COntourDrawOptions const  *draw_options;
    COntourDrawOptions  local_options;
    COntourOutputRec    *crec, *crec2;

    CSW_F               *xpts, *ypts, *xypack, *xp;
    CSW_F               *xpts2, *ypts2;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    if (xywork == NULL  ||
        xywork2 == NULL  ||
        iwork == NULL) {
        return;
    }

    int        sf_size = surf_list.size();
    DLSurf     **sf_data = surf_list.data();

/*
 * Set the contour drawing scale.  Since everything is converted to page units,
 * this just needs to have the same frame and page corners.
 */
    conapi_obj.con_SetDrawScale (0.0f, 0.0f, 100.0f, 100.0f,
                      0.0f, 0.0f, 100.0f, 100.0f);

    nprim = dls->GetNumContours ();
    contours = dls->GetContours ();
    for (ido=0; ido<nprim; ido++) {

        i = contours[ido];

        contour = cl_data[i];
        if (contour == NULL) {
            continue;
        }

        if (contour->deleted_flag == 1) {
            continue;
        }

        if (frame_list == NULL  ||
            contour->frame_num < 0) {
            continue;
        }
        current_frame_num = contour->frame_num;
        update_frame_limits ();

        crec = contour->GetCrec ();
        if (crec == NULL) {
            continue;
        }

        grid_num = contour->grid_index;

        if (grid_num < 0) {
            continue;
        }

        draw_options = NULL;
        conprop = NULL;
        if (grid_num >= 0  &&  sf_data != NULL  &&  grid_num < sf_size) {
            grid = sf_data[grid_num];
            if (grid != NULL) {
                draw_options = grid->GetDrawOptions ();
                conprop = (DLContourProperties *)grid->GetContourProperties ();
            }
        }

        if (draw_options == NULL  ||  conprop == NULL) {
            continue;
        }

        memcpy (&local_options, draw_options, sizeof(COntourDrawOptions));

        local_options.major_tick_spacing  *= page_units_per_inch;
        local_options.major_text_spacing  *= page_units_per_inch;
        local_options.major_tick_len  *= page_units_per_inch;
        local_options.major_text_size  *= page_units_per_inch;
        local_options.minor_tick_spacing  *= page_units_per_inch;
        local_options.minor_text_spacing  *= page_units_per_inch;
        local_options.minor_tick_len  *= page_units_per_inch;
        local_options.minor_text_size  *= page_units_per_inch;

        if (conprop->labelMinor == 0) {
            local_options.minor_text_size = -1.0f;
            local_options.minor_text_spacing = -1.0f;
        }
        if (conprop->labelMajor == 0) {
            local_options.major_text_size = -1.0f;
            local_options.major_text_spacing = -1.0f;
        }
        if (conprop->tickMinor == 0) {
            local_options.minor_tick_len = -1.0f;
            local_options.minor_tick_spacing = -1.0f;
        }
        if (conprop->tickMajor == 0) {
            local_options.major_tick_len = -1.0f;
            local_options.major_tick_spacing = -1.0f;
        }

        try {
            SNF;
            page_con = new DLContour ();
        }
        catch (...) {
            page_con = NULL;
            continue;
        }

        crec = contour->GetCrec ();
        if (crec == NULL) {
            continue;
        }

        npts = crec->npts;
        if (npts < 2) {
            continue;
        }

    /*
     * Convert the contour points to page coordinates.
     */
        xpts = (CSW_F *)csw_Malloc (npts * 6 * sizeof(CSW_F));
        if (xpts == NULL) {
            continue;
        }
        ypts = xpts + npts;
        xpts2 = ypts + npts;
        ypts2 = xpts2 + 2 * npts;

        memcpy (xpts, crec->x, npts * sizeof(CSW_F));
        memcpy (ypts, crec->y, npts * sizeof(CSW_F));

        crec2 = (COntourOutputRec *)csw_Malloc (sizeof(COntourOutputRec));
        if (crec2 == NULL) {
            csw_Free (xpts);
            xpts = NULL;
            ypts = NULL;
            continue;
        }

        memcpy (crec2, crec, sizeof (COntourOutputRec));

        xypack = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
        if (xypack == NULL) {
            csw_Free (xpts);
            xpts = NULL;
            ypts = NULL;
            xpts2 = NULL;
            ypts2 = NULL;
            csw_Free (crec2);
            crec2 = NULL;
            continue;
        }
        istat = gpf_packxy2 (xpts, ypts, npts,
                             xypack);
        if (istat != 1) {
            csw_Free (xpts);
            xpts = NULL;
            ypts = NULL;
            csw_Free (xypack);
            xypack = NULL;
            xpts2 = NULL;
            ypts2 = NULL;
            csw_Free (crec2);
            crec2 = NULL;
            continue;
        }

        convert_frame_array (xypack, npts);

        page_con->frame_num = contour->frame_num;
        page_con->layer_num = contour->layer_num;
        page_con->grid_index = contour->grid_index;

    /*
     * Clip the contour to the page frame corners.
     */
        gpf_calcdraw_obj.gpf_cliplineprim (xypack, npts,
                          Px1, Py1, Px2, Py2,
                          xywork2, &ncout, iwork);
        csw_Free (xypack);
        xypack = NULL;

    /*
     * Draw each clipped contour.
     */
        n = 0;
        for (j=0; j<ncout; j++) {

            xp = xywork2 + n;
            np2 = iwork[j];
            for (k=0; k<np2; k++) {
                xpts2[k] = *xp;
                xp++;
                n++;
                ypts2[k] = *xp;
                xp++;
                n++;
            }

            crec2->x = xpts2;
            crec2->y = ypts2;
            crec2->npts = np2;

            page_con->SetCrec (crec2);
            page_con->drawContourDirect
                                  (&local_options,
                                   conprop,
                                   (void *)this);
        }

        csw_Free (xpts);
        xpts = NULL;
        ypts = NULL;
        xpts2 = NULL;
        ypts2 = NULL;
        delete (page_con);
        csw_Free (crec2);
        crec2 = NULL;

    }

    return;

}  /*  end of function reclip_and_draw_selected_contours */

void CDisplayList::UnselectAll (void)
{
    int            i;
    DLSelectable   *dls;

    if (selectable_object_list == NULL  ||
        num_selectable_object_list < 1) {
        return;
    }

    for (i=0; i<num_selectable_object_list; i++) {
        dls = selectable_object_list[i];
        if (dls == NULL) {
            continue;
        }
        dls->SetSelectState (0);
    }

}

void CDisplayList::DeleteSelected (void)
{
    int            i;
    DLSelectable   *dls;

    if (selectable_object_list == NULL  ||
        num_selectable_object_list < 1) {
        return;
    }

    for (i=0; i<num_selectable_object_list; i++) {

        dls = selectable_object_list[i];
        if (dls == NULL) {
            continue;
        }

        if (dls->deleted_flag == 1) {
            continue;
        }
        if (dls->GetSelectState () == 0) {
            continue;
        }

        EraseSelectableNum (i);

        dls->deleted_flag = 1;

    }
}


void CDisplayList::HideSelected (void)
{
    int            i;
    DLSelectable   *dls;

    if (selectable_object_list == NULL  ||
        num_selectable_object_list < 1) {
        return;
    }

    for (i=0; i<num_selectable_object_list; i++) {

        dls = selectable_object_list[i];
        if (dls == NULL) {
            continue;
        }

        if (dls->deleted_flag == 1) {
            continue;
        }
        if (dls->GetSelectState () == 0) {
            continue;
        }

        HideFlag = 1;
        EraseSelectableNum (i);
        HideFlag = 0;

        dls->SetSelectState (0);

    }
}

void CDisplayList::UnhideAll (void)
{
    int             i;
    LInePrim        *lptr;
    FIllPrim        *fptr;
    TExtPrim        *tptr;
    SYmbPrim        *sptr;
    SHapePrim       *rptr;
    AXisPrim        *aptr;
    DLContour       *cptr;

    int             hsize = 0;
    int             *hdata = NULL;

    int             lp_size = (int)line_prim_list.size();
    LInePrim        *lp_data = line_prim_list.data();

    hsize =  (int)line_hidden_list.size();
    hdata =  line_hidden_list.data();

    if (lp_data != NULL  &&  lp_size > 0  &&  hdata != NULL  &&  hsize > 0) {
        for (i=0; i<hsize; i++) {
            lptr = lp_data + hdata[i];
            lptr->visible_flag = 1;
        }
    }

    int             fp_size = (int)fill_prim_list.size();
    FIllPrim        *fp_data = fill_prim_list.data();

    hsize =  (int)fill_hidden_list.size();
    hdata =  fill_hidden_list.data();

    if (fp_data != NULL  &&  fp_size > 0  &&  hdata != NULL  &&  hsize > 0) {
        for (i=0; i<hsize; i++) {
            fptr = fp_data + hdata[i];
            fptr->visible_flag = 1;
        }
    }

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    hsize =  (int)text_hidden_list.size();
    hdata =  text_hidden_list.data();

    if (tp_data != NULL  &&  tp_size > 0  &&  hdata != NULL  &&  hsize > 0) {
        for (i=0; i<hsize; i++) {
            tptr = tp_data + hdata[i];
            tptr->visible_flag = 1;
        }
    }

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    hsize =  (int)symb_hidden_list.size();
    hdata =  symb_hidden_list.data();

    if (sp_data != NULL  &&  sp_size > 0  &&  hdata != NULL  &&  hsize > 0) {
        for (i=0; i<hsize; i++) {
            sptr = sp_data + hdata[i];
            sptr->visible_flag = 1;
        }
    }

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    hsize =  (int)shape_hidden_list.size();
    hdata =  shape_hidden_list.data();

    if (hp_data != NULL  &&  hp_size > 0  &&  hdata != NULL  &&  hsize > 0) {
        for (i=0; i<hsize; i++) {
            rptr = hp_data + hdata[i];
            rptr->visible_flag = 1;
        }
    }

    int          ap_size = (int)axis_prim_list.size();
    AXisPrim     *ap_data = axis_prim_list.data();

    hsize =  (int)axis_hidden_list.size();
    hdata =  axis_hidden_list.data();

    if (ap_data != NULL  &&  ap_size > 0  &&  hdata != NULL  &&  hsize > 0) {
        for (i=0; i<hsize; i++) {
            aptr = ap_data + hdata[i];
            aptr->visible_flag = 1;
        }
    }

    hsize =  (int)contour_hidden_list.size();
    hdata =  contour_hidden_list.data();

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data != NULL  &&  cl_size > 0  &&  
          hdata != NULL  &&  hsize > 0) {
        for (i=0; i<hsize; i++) {
            cptr = contour_list[hdata[i]];
            cptr->visible_flag = 1;
        }
    }

    line_hidden_list.clear();
    fill_hidden_list.clear();
    text_hidden_list.clear();
    symb_hidden_list.clear();
    shape_hidden_list.clear();
    axis_hidden_list.clear();
    contour_hidden_list.clear();

}


/*
 * Public function to add an axis that is not part of a frame border.
 */
int CDisplayList::AddAxis (
    char         *cdata,
    int          label_dir,
    int          label_flag,
    int          tick_flag,
    int          tick_direction,
    double       major_interval,
    int          line_red,
    int          line_green,
    int          line_blue,
    int          text_red,
    int          text_green,
    int          text_blue,
    double       line_thickness,
    double       text_size,
    double       text_thickness,
    int          text_font,
    int          caption_flag,
    double       x1,
    double       y1t,
    double       x2,
    double       y2,
    double       firstValue,
    double       lastValue,
    int          label_anchor,
    double       label_angle)
{
    AXisPrim          *aptr;
    FRameAxisStruct   *fap = NULL;
    char              caption[200];
    int               next_axis;

    bool    bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (fap);
        }
    };
    CSWScopeGuard  func_scope_guard (fscope);



    caption[0] = '\0';
    if (cdata != NULL) {
        strncpy (caption, cdata, 199);
        caption[199] = '\0';
    }

    fap = (FRameAxisStruct *)csw_Calloc (sizeof(FRameAxisStruct));
    if (fap == NULL) {
        return -1;
    }

    fap->label_flag = label_flag;
    fap->tick_flag = tick_flag;
    fap->tick_direction = tick_direction;
    strncpy (fap->caption, caption, 199);
    fap->caption[199] = '\0';
    fap->major_interval = (CSW_F)major_interval;
    fap->line_red = line_red;
    fap->line_green = line_green;
    fap->line_blue = line_blue;
    fap->text_red = text_red;
    fap->text_green = text_green;
    fap->text_blue = text_blue;
    fap->line_thickness = (CSW_F)line_thickness;
    fap->text_size = (CSW_F)text_size;
    fap->text_thickness = (CSW_F)text_thickness;
    fap->text_font = text_font;
    fap->caption_flag = caption_flag;
    fap->label_anchor = label_anchor;
    fap->label_angle = label_angle;

    next_axis = get_available_axis ();
    if (next_axis < 0) {
        try {
            SNF;
            AXisPrim  apr;
            ZeroInit (&apr, sizeof(apr));
            axis_prim_list.push_back (apr);
            next_axis = (int)axis_prim_list.size() - 1;
        }
        catch (...) {
            printf ("Exception in axis_prim_list pushback\n");
            return 0;
        }
    }

    AXisPrim    *ap_data = axis_prim_list.data();

    aptr = ap_data + next_axis;
    aptr->ap = fap;
    fap = NULL;
    aptr->fx1 = (CSW_F)x1;
    aptr->fy1 = (CSW_F)y1t;
    aptr->fx2 = (CSW_F)x2;
    aptr->fy2 = (CSW_F)y2;
    aptr->frame_num = current_frame_num;
    aptr->layer_num = current_layer_num;
    aptr->item_num = current_item_num;
    aptr->afirst = (CSW_F)firstValue;
    aptr->alast = (CSW_F)lastValue;
    aptr->label_dir = label_dir;
    aptr->prim_num = next_axis;
    aptr->selectable_object_num = current_selectable_object_num;
    aptr->deleted_flag = 0;
    aptr->visible_flag = 1;
    aptr->calc_prims_needed = true;

    if (current_selectable_object) {
        current_selectable_object->AddAxis (next_axis);
    }

    bsuccess = true;

    return 1;

}

/*
 *  Break down axes into lines, text, etc and add these
 *  primitives to the display list as axis primitives.
 *  Since scale to screen drawing is done at draw time
 *  (rather than at add to display list time) then the
 *  lines, fills, etc. only need to be "calculated" one
 *  time rather than with all zoom and pan and other 
 *  things on the display list.
 *
 *  Note that this affects axes inside a frame, not the axes
 *  that are a part of a frame border.
 */
void CDisplayList::reaxis_frame (FRameStruct *frptr)
{
    int           fnum, fsave;
    int           i, ldir, tdir, ipos;
    AXisPrim      *aptr;
    FRameAxisStruct  fax;
    CSW_F         ax1, ay1, ax2, ay2, dx, dy, dist;
    CSW_F         aax1, aay1, aax2, aay2;
    double        ang, cang, sang;

/*
 * Make sure the frame is valid and there are axes to draw.
 */
    if (frptr == NULL) {
        return;
    }
    if (frame_list == NULL) {
        return;
    }

    fnum = frptr - frame_list;
    if (fnum < 0  ||  fnum >= num_frame_list) {
        return;
    }

    int          ap_size = (int)axis_prim_list.size();
    AXisPrim     *ap_data = axis_prim_list.data();

    if (ap_data == NULL  ||  ap_size < 1) {
        return;
    }

    fsave = current_frame_num;
    current_frame_num = fnum;
    current_border_num = -1;
    current_axis_num = 1;

    for (i=0; i<ap_size; i++) {

        aptr = ap_data + i;
        if (aptr->frame_num != fnum) {
            continue;
        }
        if (aptr->ap == NULL) {
            continue;
        }
        if (aptr->visible_flag == 0) {
            continue;
        }
        if (aptr->calc_prims_needed == false) {
            continue;
        }

        memcpy (&fax, aptr->ap, sizeof(FRameAxisStruct));

        tdir = fax.tick_direction;
        ldir = aptr->label_dir;

        ax1 = aptr->fx1;
        ay1 = aptr->fy1;
        ax2 = aptr->fx2;
        ay2 = aptr->fy2;

        dx = ax2 - ax1;
        dy = ay2 - ay1;

        ang = atan2 ((double)dy, (double)dx);
        cang = cos (ang);
        sang = sin (ang);

        dist = dx * dx + dy * dy;
        dist = (CSW_F)sqrt ((double)dist);


    /*
     * Draw a horizontal axis and then rotate it.
     */
        if (sang < 0.1  &&  sang > -0.1) {
            ax2 = ax1 + dist;
            ay2 = ay1;
            ipos = 1;
            fax.tick_direction = 1;
            if (ldir == 1  &&  tdir < 0) {
                ipos = 1;
                fax.tick_direction = -1;
            }
            else if (ldir == 1  &&  tdir > 0) {
                ipos = 3;
                fax.tick_direction = -1;
            }
            else if (ldir == -1  &&  tdir < 0) {
                ipos = 3;
                fax.tick_direction = 1;
            }
            else if (ldir == -1  &&  tdir > 0) {
                ipos = 1;
                fax.tick_direction = 1;
            }
            aax1 = ax1;
            aax2 = ax2;
            if (ipos == 1) {
                aay1 = ay1;
                aay2 = ay2;
            }
            else {
                aay1 = ay1 - dist;
                aay2 = ay1;
            }
            calc_and_rotate_frame_axis_prims (
                fnum,
                &fax,
                aax1,
                aay1,
                aax2,
                aay2,
                aptr->afirst,
                aptr->alast,
                ipos,
                ax1,
                ay1,
                (CSW_F)cang,
                (CSW_F)sang);
        }

    /*
     * Draw a vertical axis and then rotate it.
     */
        else {

            ax2 = ax1;
            ay2 = ay1 + dist;
            ipos = 2;
            fax.tick_direction = 1;
            if (ldir == 1  &&  tdir < 0) {
                ipos = 4;
                fax.tick_direction = -1;
            }
            else if (ldir == 1  &&  tdir > 0) {
                ipos = 2;
                fax.tick_direction = -1;
            }
            else if (ldir == -1  &&  tdir < 0) {
                ipos = 2;
                fax.tick_direction = 1;
            }
            else if (ldir == -1  &&  tdir > 0) {
                ipos = 4;
                fax.tick_direction = 1;
            }
            aay1 = ay1;
            aay2 = ay2;
            if (ipos == 2) {
                aax1 = ax1;
                aax2 = ax2;
            }
            else {
                aax1 = ax1 - dist;
                aax2 = ax1;
            }
            calc_and_rotate_frame_axis_prims (
                fnum,
                &fax,
                aax1,
                aay1,
                aax2,
                aay2,
                aptr->afirst,
                aptr->alast,
                ipos,
                ax1,
                ay1,
                (CSW_F)cang,
                (CSW_F)sang);
        }

        aptr->calc_prims_needed = false;

    }

    current_border_num = -1;
    current_axis_num = -1;
    current_frame_num = fsave;

    return;
}





void CDisplayList::delete_frame_axis_lines (int fnum)
{
    int            i;
    LInePrim       *lptr;

    int            lp_size = (int)line_prim_list.size();
    LInePrim       *lp_data = line_prim_list.data();

    if (lp_data == NULL  ||  lp_size < 1) {
        return;
    }

    for (i=0; i<lp_size; i++) {

        lptr = lp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }

        if (lptr->frame_num != fnum) {
            continue;
        }

        if (lptr->axis_num <= 0) {
            continue;
        }

        csw_Free (lptr->xypts);
        lptr->xypts = NULL;
        lptr->deleted_flag = 1;
        lptr->npts = 0;
        lptr->maxpts = 0;
        add_available_line (i);

    }

    return;

}  /*  end of function delete_frame_axis_lines */



void CDisplayList::delete_frame_axis_fills (int fnum)
{
    int            i;
    FIllPrim       *fptr;

    int            fp_size = (int)fill_prim_list.size();
    FIllPrim       *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    for (i=0; i<fp_size; i++) {

        fptr = fp_data + i;

        if (fptr->deleted_flag == 1) {
            continue;
        }

        if (fptr->frame_num != fnum) {
            continue;
        }

        if (fptr->axis_num <= 0) {
            continue;
        }

        csw_Free (fptr->xypts);
        fptr->xypts = NULL;
        fptr->npts = 0;
        fptr->maxpts = 0;
        fptr->deleted_flag = 1;
        add_available_fill (i);

    }

    return;

}  /*  end of function delete_frame_axis_fills */



void CDisplayList::delete_frame_axis_texts (int fnum)
{
    int            i;
    TExtPrim       *tptr;

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }

    for (i=0; i<tp_size; i++) {

        tptr = tp_data + i;

        if (tptr->deleted_flag == 1) {
            continue;
        }

        if (tptr->frame_num != fnum) {
            continue;
        }

        if (tptr->axis_num <= 0) {
            continue;
        }

        csw_Free (tptr->chardata);
        tptr->chardata = NULL;
        tptr->deleted_flag = 1;
        if (tptr->selectable_object_num >= 0) {
            num_selectable_text--;
        }
        add_available_text (i);

    }

    if (num_selectable_text < 0) num_selectable_text = 0;

    return;

}  /*  end of function delete_frame_axis_texts */




int CDisplayList::calc_and_rotate_frame_axis_prims
                                        (int fnum,
                                         FRameAxisStruct *axptr,
                                         CSW_F px1,
                                         CSW_F py1,
                                         CSW_F px2,
                                         CSW_F py2,
                                         CSW_F val1,
                                         CSW_F val2,
                                         int position,
                                         CSW_F xorigin,
                                         CSW_F yorigin,
                                         CSW_F cang,
                                         CSW_F sang)
{
    GRaphUnion         uval;
    GRaphLineRec       *glines, *lp;
    GRaphTextRec       *gtexts, *tp;
    int                nglines,
                       ngtexts;
    int                i, j, istat,
                       gpos;
    int                save_border;
    CSW_F              axmin,
                       axmax,
                       axmajor;
    double             *xd, *yd;
    int                rflag;
    double             xt, yt, xr, yr;

/*
 * Set the graph axis position.
 */
    if (position == 1) {
        gpos = GPF_BOTTOM_AXIS;
    }
    else if (position == 2) {
        gpos = GPF_LEFT_AXIS;
    }
    else if (position == 3) {
        gpos = GPF_TOP_AXIS;
    }
    else if (position == 4) {
        gpos = GPF_RIGHT_AXIS;
    }
    else {
        return 0;
    }

    rflag = 0;
    if (position == 1  ||  position == 3) {
        if (sang > 0.001  ||  sang < -0.001) {
            rflag = 1;
        }
    }
    else {
        if (cang > 0.001  ||  cang < -0.001) {
            rflag = 1;
        }
    }

/*
 * Set all the graph parameters to their defaults.
 */
    gpf_graph_obj.gpf_SetGraphDefaults ();

/*
 * Define the position of the frame on the plot page for the
 * graph utilities.
 */
    gpf_graph_obj.gpf_StartGraph (px1,
                    py1,
                    px2,
                    py2,
                    page_units_per_inch,
                    0,    /* only used for time graphs */
                    page_units_per_inch
                   );

/*
 * Set the axis parameters that are controlled by the frame axis structure.
 */
  // label anchor and position
    uval.ival = axptr->label_anchor;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_ANCHOR, uval);
    uval.fval = (CSW_F)axptr->label_angle;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_ANGLE, uval);

  // text fonts
    uval.ival = axptr->text_font;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_FONT, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_FONT, uval);

  // text sizes
    uval.fval = axptr->text_size * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_SIZE, uval);
    uval.fval *= 1.1f;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_SIZE, uval);

  // major interval
    if (axptr->major_interval > 0.0) {
        uval.fval = axptr->major_interval;
        gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_INTERVAL, uval);
    }

  // min and max values on the axis
    uval.fval = val1;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MIN_VALUE, uval);
    uval.fval = val2;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAX_VALUE, uval);

  // minor and major tick lengths
    uval.fval = .05f * page_units_per_inch * axptr->tick_direction;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_LENGTH, uval);
    uval.fval = .08f * page_units_per_inch * axptr->tick_direction;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_LENGTH, uval);

    uval.fval = .05f * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_GAP, uval);
    uval.fval = .08f * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_GAP, uval);

  // line thickness
    uval.fval = axptr->line_thickness * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LINE_THICK, uval);

  // text thickness
    uval.fval = axptr->text_thickness * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_THICK, uval);

/*
 * Generate graph primitive structures for the axis.
 */
    istat = gpf_graph_obj.gpf_CalcGraphAxis (
        gpos,
        axptr->caption,
        val1,
        val2,
        0.0, // linear axis
        axptr->label_flag,
        NULL, // no custom labels
        NULL,
        0,
        &axmin,
        &axmax,
        &axmajor,
        &glines,
        &nglines,
        &gtexts,
        &ngtexts
    );

    if (istat == -1) {
        return -1;
    }

    save_border = current_border_num;
    save_current_graphic_attributes ();
    SetDefaultGraphicAttributes ();
    current_border_num = -1;
    current_axis_num = 1;
    current_frame_num = fnum;

    current_line_red = axptr->line_red;
    current_line_green = axptr->line_green;
    current_line_blue = axptr->line_blue;
    current_line_alpha = 255;
    current_line_thickness = axptr->line_thickness;

    for (i=0; i<nglines; i++) {
        lp = glines + i;
        xd = (double *)csw_Malloc (lp->npts * 2 * sizeof(double));
        if (xd == NULL) {
            continue;
        }
        yd = xd + lp->npts;
        for (j=0; j<lp->npts; j++) {
            xd[j] = lp->x[j];
            yd[j] = lp->y[j];
        }
        if (rflag == 0) {
            AddLine (xd,
                     yd,
                     lp->npts,
                     true);
            csw_Free (xd);
            xd = NULL;
            yd = NULL;
        }
        else {
            for (j=0; j<lp->npts; j++) {
                xt = xd[j] - xorigin;
                yt = yd[j] - yorigin;
                xr = xt * cang - yt * sang;
                yr = xt * sang + yt * cang;
                xd[j] = xr + xorigin;
                yd[j] = yr + yorigin;
            }
            AddLine (xd,
                     yd,
                     lp->npts,
                     true);
            csw_Free (xd);
            xd = NULL;
            yd = NULL;
        }
    }

    current_text_red = axptr->text_red;
    current_text_green = axptr->text_green;
    current_text_blue = axptr->text_blue;
    current_text_alpha = 255;
    current_text_thickness = axptr->text_thickness;
    current_text_font = axptr->text_font;
    current_text_anchor = 1;

    for (i=0; i<ngtexts; i++) {
        tp = gtexts + i;
        if (rflag == 0) {
            AddText ((double)tp->x,
                     (double)tp->y,
                     tp->size / page_units_per_inch,
                     tp->angle,
                     tp->text,
                     true);
        }
        else {
            xt = (double)tp->x;
            yt = (double)tp->y;
            xr = xt * cang - yt * sang;
            yr = xt * sang + yt * cang;
            AddText (xr,
                     yr,
                     //tp->size / page_units_per_inch,
                     tp->size,
                     tp->angle,
                     tp->text,
                     true);
        }
    }

    gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
    gpf_graph_obj.gpf_FreeGraphText (gtexts, ngtexts);

    unsave_current_graphic_attributes ();

    current_border_num = save_border;

    return 1;

}


void CDisplayList::closest_frame_axis (int fnum, CSW_F xin, CSW_F yin,
                                       int *indexout, CSW_F *pdistout)
{
    AXisPrim            *aptr;
    int                  nprim, ido, i, index;
    CSW_F               dist, dmin, xline[2], yline[2];
    double              ddmin;

    *indexout = -1;

    int          ap_size = (int)axis_prim_list.size();
    AXisPrim     *ap_data = axis_prim_list.data();

    if (ap_data == NULL  ||  ap_size < 1) {
        return;
    }

    nprim = ap_size;

    dmin = *pdistout;
    index = -1;

    for (ido=0; ido<nprim; ido++) {

        i = ido;

        aptr = ap_data + i;

        if (aptr->frame_num != fnum) {
            continue;
        }

        if (aptr->selectable_object_num < 0) {
            continue;
        }

        xline[0] = aptr->fx1;
        yline[0] = aptr->fy1;
        xline[1] = aptr->fx2;
        yline[1] = aptr->fy2;

        convert_frame_array (xline, yline, 2);

        gpf_calclinedist1xy (xline, yline, 2,
                             xin, yin, &dist);
        if (dist < index_xspace) {
            dist /= 2.0f;
        }

        ddmin = 1.e30;
        axis_text_pick (
            aptr,
            &ddmin,
            (double)xin,
            (double)yin);

        if (dist < dmin  ||  ddmin < dmin) {
            dmin = dist;
            if (ddmin < dist) dmin = (CSW_F)ddmin;
            index = i;
        }

    }

    if (index >= 0  &&  dmin < 1.e20) {
        *indexout = index;
        *pdistout = dmin;
        return;
    }

    return;

}  /*  end of function closest_frame_axis */


int CDisplayList::axis_text_pick (AXisPrim *aptr,
                                  double *dmin,
                                  double xpick,
                                  double ypick)
{
    int           fnum;
    int           istat, ldir, tdir, ipos;
    FRameAxisStruct  fax;
    CSW_F         ax1, ay1, ax2, ay2, dx, dy, dist;
    CSW_F         aax1, aay1, aax2, aay2;
    double        ang, cang, sang;

    fnum = aptr->frame_num;
    if (fnum < 0  ||  fnum >= num_frame_list) {
        return 0;
    }

    if (aptr->visible_flag == 0) {
        return 0;
    }

    memcpy (&fax, aptr->ap, sizeof(FRameAxisStruct));

    tdir = fax.tick_direction;
    ldir = aptr->label_dir;

    ax1 = aptr->fx1;
    ay1 = aptr->fy1;
    ax2 = aptr->fx2;
    ay2 = aptr->fy2;

    convert_frame_point (&ax1, &ay1);
    convert_frame_point (&ax2, &ay2);

    dx = ax2 - ax1;
    dy = ay2 - ay1;

    ang = atan2 ((double)dy, (double)dx);
    cang = cos (ang);
    sang = sin (ang);

    dist = dx * dx + dy * dy;
    dist = (CSW_F)sqrt ((double)dist);

/*
 * Draw a horizontal axis and then rotate it.
 */
    if (sang < 0.1  &&  sang > -0.1) {
        ax2 = ax1 + dist;
        ay2 = ay1 + dist;
        ipos = 1;
        fax.tick_direction = 1;
        if (ldir == 1  &&  tdir < 0) {
            ipos = 1;
            fax.tick_direction = -1;
        }
        else if (ldir == 1  &&  tdir > 0) {
            ipos = 3;
            fax.tick_direction = -1;
        }
        else if (ldir == -1  &&  tdir < 0) {
            ipos = 3;
            fax.tick_direction = 1;
        }
        else if (ldir == -1  &&  tdir > 0) {
            ipos = 1;
            fax.tick_direction = 1;
        }
        aax1 = ax1;
        aax2 = ax2;
        if (ipos == 1) {
            aay1 = ay1;
            aay2 = ay2;
        }
        else {
            aay1 = ay1 - dist;
            aay2 = ay1;
        }
        istat =
        pick_axis_text_prims (
            xpick,
            ypick,
            dmin,
            fnum,
            &fax,
            aax1,
            aay1,
            aax2,
            aay2,
            aptr->afirst,
            aptr->alast,
            ipos,
            (CSW_F)cang,
            (CSW_F)sang);
    }

/*
 * Draw a vertical axis and then rotate it.
 */
    else {

        ax2 = ax1 + dist;
        ay2 = ay1 + dist;
        ipos = 2;
        fax.tick_direction = 1;
        if (ldir == 1  &&  tdir < 0) {
            ipos = 4;
            fax.tick_direction = -1;
        }
        else if (ldir == 1  &&  tdir > 0) {
            ipos = 2;
            fax.tick_direction = -1;
        }
        else if (ldir == -1  &&  tdir < 0) {
            ipos = 2;
            fax.tick_direction = 1;
        }
        else if (ldir == -1  &&  tdir > 0) {
            ipos = 4;
            fax.tick_direction = 1;
        }
        aay1 = ay1;
        aay2 = ay2;
        if (ipos == 2) {
            aax1 = ax1;
            aax2 = ax2;
        }
        else {
            aax1 = ax1 - dist;
            aax2 = ax1;
        }
        istat =
        pick_axis_text_prims (
            xpick,
            ypick,
            dmin,
            fnum,
            &fax,
            aax1,
            aay1,
            aax2,
            aay2,
            aptr->afirst,
            aptr->alast,
            ipos,
            (CSW_F)cang,
            (CSW_F)sang);
    }

    return istat;
}



int CDisplayList::pick_axis_text_prims (
     double xpick,
     double ypick,
     double *dmin,
     int fnum,
     FRameAxisStruct *axptr,
     CSW_F px1,
     CSW_F py1,
     CSW_F px2,
     CSW_F py2,
     CSW_F val1,
     CSW_F val2,
     int position,
     CSW_F cang,
     CSW_F sang)
{
    GRaphUnion         uval;
    GRaphLineRec       *glines;
    GRaphTextRec       *gtexts, *tp;
    int                nglines,
                       ngtexts;
    int                i, istat,
                       gpos;
    CSW_F              axmin,
                       axmax,
                       axmajor;
    CSW_F              fdmin;
    int                rflag;
    double             xt, yt, xr, yr;

    fnum = fnum;

/*
 * Set the graph axis position.
 */
    if (position == 1) {
        gpos = GPF_BOTTOM_AXIS;
    }
    else if (position == 2) {
        gpos = GPF_LEFT_AXIS;
    }
    else if (position == 3) {
        gpos = GPF_TOP_AXIS;
    }
    else if (position == 4) {
        gpos = GPF_RIGHT_AXIS;
    }
    else {
        return 0;
    }

    rflag = 0;
    if (position == 1  ||  position == 3) {
        if (sang > 0.001  ||  sang < -0.001) {
            rflag = 1;
        }
    }
    else {
        if (cang > 0.001  ||  cang < -0.001) {
            rflag = 1;
        }
    }

/*
 * Set all the graph parameters to their defaults.
 */
    gpf_graph_obj.gpf_SetGraphDefaults ();

/*
 * Define the position of the frame on the plot page for the
 * graph utilities.
 */
    gpf_graph_obj.gpf_StartGraph (px1,
                    py1,
                    px2,
                    py2,
                    page_units_per_inch,
                    0,    /* only used for time graphs */
                    page_units_per_inch
                   );

/*
 * Set the axis parameters that are controlled by the frame axis structure.
 */
  // label anchor and position
    uval.ival = axptr->label_anchor;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_ANCHOR, uval);
    uval.fval = (CSW_F)axptr->label_angle;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_ANGLE, uval);

  // text fonts
    uval.ival = axptr->text_font;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_FONT, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_FONT, uval);

  // text sizes
    uval.fval = axptr->text_size * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_SIZE, uval);
    uval.fval *= 1.1f;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_SIZE, uval);

  // major interval
    if (axptr->major_interval > 0.0) {
        uval.fval = axptr->major_interval;
        gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_INTERVAL, uval);
    }

  // min and max values on the axis
    uval.fval = val1;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MIN_VALUE, uval);
    uval.fval = val2;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAX_VALUE, uval);

  // minor and major tick lengths
    uval.fval = .05f * page_units_per_inch * axptr->tick_direction;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_LENGTH, uval);
    uval.fval = .08f * page_units_per_inch * axptr->tick_direction;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_LENGTH, uval);

    uval.fval = .05f * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_GAP, uval);
    uval.fval = .08f * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_GAP, uval);

  // line thickness
    uval.fval = axptr->line_thickness * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LINE_THICK, uval);

  // text thickness
    uval.fval = axptr->text_thickness * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_THICK, uval);

/*
 * Generate graph primitive structures for the axis.
 */
    istat = gpf_graph_obj.gpf_CalcGraphAxis (
        gpos,
        axptr->caption,
        val1,
        val2,
        0.0, // linear axis
        axptr->label_flag,
        NULL, // no custom labels
        NULL,
        0,
        &axmin,
        &axmax,
        &axmajor,
        &glines,
        &nglines,
        &gtexts,
        &ngtexts
    );

    if (istat == -1) {
        return -1;
    }

    fdmin = (CSW_F)*dmin;
    for (i=0; i<ngtexts; i++) {
        tp = gtexts + i;
        if (rflag != 0) {
            xt = (double)tp->x;
            yt = (double)tp->y;
            xr = xt * cang - yt * sang;
            yr = xt * sang + yt * cang;
            xt = xr;
            yt = yr;
        }
        else {
            xt = (double)tp->x;
            yt = (double)tp->y;
        }
        distance_to_text (
            (CSW_F)xpick,
            (CSW_F)ypick,
            xt,
            yt,
            (double)tp->size,
            (double)tp->angle,
            tp->font,
            1,
            tp->text,
            &fdmin);
    }
    *dmin = (double)fdmin;

    gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
    gpf_graph_obj.gpf_FreeGraphText (gtexts, ngtexts);

    return 1;

}


void CDisplayList::distance_to_text (CSW_F xin,
                                     CSW_F yin,
                                     double xtext,
                                     double ytext,
                                     double tsize,
                                     double tang,
                                     int font_num,
                                     int anchor,
                                     char *chardata,
                                     CSW_F *pdistout)
{
    CSW_F          fsize, width;
    CSW_F          cx[5], cy[5], dist, dmin;
    CSW_F          bounds[10];
    int            istat;

    dmin = *pdistout;

/*
 * The size passed to this function is in page units.  It
 * needs to be converted to inches prior to calling the
 * text bounds function.
 */
    fsize = (CSW_F)tsize / page_units_per_inch;

/*
 * Get the rotated rectangle bounding the text
 * and check for distance to it.
 */
    gtx_GetTextBounds (chardata,
                     font_num,
                     fsize,
                     bounds);

/*
 * Convert the bounds results back to page units.
 */
    width = bounds[0] * page_units_per_inch;
    fsize = bounds[1] * page_units_per_inch;

    gpf_calcdraw_obj.gpf_find_box_corners ((CSW_F)xtext, (CSW_F)ytext,
                          anchor,
                          width, fsize, (CSW_F)tang,
                          cx, cy);
    cx[4] = cx[0];
    cy[4] = cy[0];

    gpf_calclinedist1xy (cx, cy, 5,
                         xin, yin, &dist);
    if (dist < dmin) {
        dmin = dist;
    }

/*
 * If the pick point is inside the text bounding box,
 * return a very small distance.
 */
    istat = ply_utils_obj.ply_point1 (cx, cy, 5,
                        xin, yin);
    if (istat == 1) {
        dmin /= 100.0;
    }

    *pdistout = dmin;

    return;

}  /*  end of function distance_to_text */



/*
 * Remove any existing line, fill and text primitives that are associated
 * with an axis in this frame and add new primitives reflecting the frame's
 * current setup.
 */
void CDisplayList::reclip_and_draw_selected_axes (DLSelectable *dls)
{
    int           *ilist, nlist;
    int           fsave;
    int           i, ldir, tdir, ipos;
    AXisPrim      *aptr;
    FRameAxisStruct  fax;
    CSW_F         ax1, ay1, ax2, ay2, dx, dy, dist;
    CSW_F         aax1, aay1, aax2, aay2;
    double        ang, cang, sang;

    int          ap_size = (int)axis_prim_list.size();
    AXisPrim     *ap_data = axis_prim_list.data();

    if (ap_data == NULL  ||  ap_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    ilist = dls->GetAxes ();
    if (ilist == NULL) {
        return;
    }

    nlist = dls->GetNumAxes ();
    if (nlist < 1) {
        return;
    }

    fsave = current_frame_num;

    for (i=0; i<nlist; i++) {

        aptr = ap_data + ilist[i];
        if (aptr->ap == NULL) {
            continue;
        }

        current_frame_num = aptr->frame_num;

        memcpy (&fax, aptr->ap, sizeof(FRameAxisStruct));

        tdir = fax.tick_direction;
        ldir = aptr->label_dir;

        ax1 = aptr->fx1;
        ay1 = aptr->fy1;
        ax2 = aptr->fx2;
        ay2 = aptr->fy2;

        //convert_frame_point (&ax1, &ay1);
        //convert_frame_point (&ax2, &ay2);

        dx = ax2 - ax1;
        dy = ay2 - ay1;

        ang = atan2 ((double)dy, (double)dx);
        cang = cos (ang);
        sang = sin (ang);

        dist = dx * dx + dy * dy;
        dist = (CSW_F)sqrt ((double)dist);

    /*
     * Draw a horizontal axis and then rotate it.
     */
        if (sang < 0.1  &&  sang > -0.1) {

            ax2 = ax1 + dist;
            ay2 = ay1 + dist;
            ipos = 1;
            fax.tick_direction = 1;
            if (ldir == 1  &&  tdir < 0) {
                ipos = 1;
                fax.tick_direction = -1;
            }
            else if (ldir == 1  &&  tdir > 0) {
                ipos = 3;
                fax.tick_direction = -1;
            }
            else if (ldir == -1  &&  tdir < 0) {
                ipos = 3;
                fax.tick_direction = 1;
            }
            else if (ldir == -1  &&  tdir > 0) {
                ipos = 1;
                fax.tick_direction = 1;
            }
            aax1 = ax1;
            aax2 = ax2;
            if (ipos == 1) {
                aay1 = ay1;
                aay2 = ay2;
            }
            else {
                aay1 = ay1 - dist;
                aay2 = ay1;
            }
            calc_and_rotate_frame_axis_prims_direct (
                &fax,
                aax1,
                aay1,
                aax2,
                aay2,
                aptr->afirst,
                aptr->alast,
                ipos,
                ax1,
                ay1,
                (CSW_F)cang,
                (CSW_F)sang);
        }

    /*
     * Draw a vertical axis and then rotate it.
     */
        else {

            ax2 = ax1 + dist;
            ay2 = ay1 + dist;
            ipos = 2;
            fax.tick_direction = 1;
            if (ldir == 1  &&  tdir < 0) {
                ipos = 4;
                fax.tick_direction = -1;
            }
            else if (ldir == 1  &&  tdir > 0) {
                ipos = 2;
                fax.tick_direction = -1;
            }
            else if (ldir == -1  &&  tdir < 0) {
                ipos = 2;
                fax.tick_direction = 1;
            }
            else if (ldir == -1  &&  tdir > 0) {
                ipos = 4;
                fax.tick_direction = 1;
            }
            aay1 = ay1;
            aay2 = ay2;
            if (ipos == 2) {
                aax1 = ax1;
                aax2 = ax2;
            }
            else {
                aax1 = ax1 - dist;
                aax2 = ax1;
            }
            calc_and_rotate_frame_axis_prims_direct (
                &fax,
                aax1,
                aay1,
                aax2,
                aay2,
                aptr->afirst,
                aptr->alast,
                ipos,
                ax1,
                ay1,
                (CSW_F)cang,
                (CSW_F)sang);
        }

    }

    fflush (stdout);

    current_border_num = -1;
    current_axis_num = -1;
    current_frame_num = fsave;

    return;
}



int CDisplayList::calc_and_rotate_frame_axis_prims_direct
                                        (FRameAxisStruct *axptr,
                                         CSW_F px1,
                                         CSW_F py1,
                                         CSW_F px2,
                                         CSW_F py2,
                                         CSW_F val1,
                                         CSW_F val2,
                                         int position,
                                         CSW_F xorigin,
                                         CSW_F yorigin,
                                         CSW_F cang,
                                         CSW_F sang)
{
    GRaphUnion         uval;
    GRaphLineRec       *glines, *lp;
    GRaphTextRec       *gtexts, *tp;
    int                nglines,
                       ngtexts;
    int                i, j, istat,
                       gpos;
    CSW_F              axmin,
                       axmax,
                       axmajor;
    double             *xd, *yd;
    int                rflag;
    double             xt, yt, xr, yr;
    CSW_F              *xy;

/*
 * Set the graph axis position.
 */
    if (position == 1) {
        gpos = GPF_BOTTOM_AXIS;
    }
    else if (position == 2) {
        gpos = GPF_LEFT_AXIS;
    }
    else if (position == 3) {
        gpos = GPF_TOP_AXIS;
    }
    else if (position == 4) {
        gpos = GPF_RIGHT_AXIS;
    }
    else {
        return 0;
    }

    rflag = 0;
    if (position == 1  ||  position == 3) {
        if (sang > 0.001  ||  sang < -0.001) {
            rflag = 1;
        }
    }
    else {
        if (cang > 0.001  ||  cang < -0.001) {
            rflag = 1;
        }
    }

/*
 * Set all the graph parameters to their defaults.
 */
    gpf_graph_obj.gpf_SetGraphDefaults ();

/*
 * Define the position of the frame on the plot page for the
 * graph utilities.
 */
    gpf_graph_obj.gpf_StartGraph (px1,
                    py1,
                    px2,
                    py2,
                    page_units_per_inch,
                    0,    /* only used for time graphs */
                    page_units_per_inch
                   );

/*
 * Set the axis parameters that are controlled by the frame axis structure.
 */
  // label anchor and position
    uval.ival = axptr->label_anchor;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_ANCHOR, uval);
    uval.fval = (CSW_F)axptr->label_angle;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_ANGLE, uval);

  // text fonts
    uval.ival = axptr->text_font;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_FONT, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_FONT, uval);

  // text sizes
    uval.fval = axptr->text_size * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_SIZE, uval);
    uval.fval *= 1.1f;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_SIZE, uval);

  // major interval
    if (axptr->major_interval > 0.0) {
        uval.fval = axptr->major_interval;
        gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_INTERVAL, uval);
    }

  // min and max values on the axis
    uval.fval = val1;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MIN_VALUE, uval);
    uval.fval = val2;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAX_VALUE, uval);

  // minor and major tick lengths
    uval.fval = .05f * page_units_per_inch * axptr->tick_direction;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_LENGTH, uval);
    uval.fval = .08f * page_units_per_inch * axptr->tick_direction;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_LENGTH, uval);

    uval.fval = .05f * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_GAP, uval);
    uval.fval = .08f * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_GAP, uval);

  // line thickness
    uval.fval = axptr->line_thickness * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LINE_THICK, uval);

  // text thickness
    uval.fval = axptr->text_thickness * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_THICK, uval);

/*
 * Generate graph primitive structures for the axis.
 */
    istat = gpf_graph_obj.gpf_CalcGraphAxis (
        gpos,
        axptr->caption,
        val1,
        val2,
        0.0, // linear axis
        axptr->label_flag,
        NULL, // no custom labels
        NULL,
        0,
        &axmin,
        &axmax,
        &axmajor,
        &glines,
        &nglines,
        &gtexts,
        &ngtexts
    );

    if (istat == -1) {
        return -1;
    }

    for (i=0; i<nglines; i++) {
        lp = glines + i;
        xd = (double *)csw_Malloc (lp->npts * 2 * sizeof(double));
        if (xd == NULL) {
            continue;
        }
        yd = xd + lp->npts;
        for (j=0; j<lp->npts; j++) {
            xd[j] = lp->x[j];
            yd[j] = lp->y[j];
        }
        if (rflag == 0) {
            xy = (CSW_F *)csw_Malloc (lp->npts * 2 * sizeof(CSW_F));
            if (xy == NULL) {
                csw_Free (xd);
                xd = NULL;
                yd = NULL;
                continue;
            }
            istat = gpf_packxy2 (lp->x, lp->y, lp->npts,
                                 xy);
            if (istat != 1) {
                csw_Free (xd);
                xd = NULL;
                yd = NULL;
                csw_Free (xy);
                xy = NULL;
                continue;
            }

            if (current_frame_num >= 0) {
                convert_frame_array (current_frame_num, xy, lp->npts);
            }
            gtx_drawprim_obj.gtx_cliplineprim(xy,
                             lp->npts,
                             0,
                             axptr->line_thickness,
                             axptr->line_red,
                             axptr->line_green,
                             axptr->line_blue,
                             0,
                             1.0f,
                             0);
            csw_Free (xd);
            xd = NULL;
            yd = NULL;
            csw_Free (xy);
            xy = NULL;
        }
        else {
            for (j=0; j<lp->npts; j++) {
                xt = xd[j] - xorigin;
                yt = yd[j] - yorigin;
                xr = xt * cang - yt * sang;
                yr = xt * sang + yt * cang;
                xd[j] = xr + xorigin;
                yd[j] = yr + yorigin;
            }

            xy = (CSW_F *)csw_Malloc (lp->npts * 2 * sizeof(CSW_F));
            if (xy == NULL) {
                csw_Free (xd);
                xd = NULL;
                yd = NULL;
                continue;
            }

            istat = gpf_packxy_double (xd, yd, lp->npts, xy);
            if (istat != 1) {
                csw_Free (xd);
                xd = NULL;
                yd = NULL;
                csw_Free (xy);
                xy = NULL;
                continue;
            }

            if (current_frame_num >= 0) {
                convert_frame_array (current_frame_num, xy, lp->npts);
            }
            gtx_drawprim_obj.gtx_cliplineprim(xy,
                             lp->npts,
                             0,
                             axptr->line_thickness,
                             axptr->line_red,
                             axptr->line_green,
                             axptr->line_blue,
                             0,
                             1.0f,
                             0);
            csw_Free (xd);
            xd = NULL;
            yd = NULL;
            csw_Free (xy);
            xy = NULL;
        }
    }

    for (i=0; i<ngtexts; i++) {
        tp = gtexts + i;
        if (rflag == 0) {
            ezx_java_obj.ezx_SetDrawingPriority (3);
            double  xxt1 = tp->x;
            double  yyt1 = tp->y;
            if (current_frame_num >= 0) {
                convert_frame_point (current_frame_num, &xxt1, &yyt1);
            }
            gtx_drawprim_obj.gtx_cliptextprim(xxt1,
                             yyt1,
                             tp->text,
                             1,
                             axptr->text_thickness,
                             axptr->text_red,
                             axptr->text_green,
                             axptr->text_blue,
                             axptr->text_red,
                             axptr->text_green,
                             axptr->text_blue,
                             tp->angle,
                             tp->size / page_units_per_inch,
                             axptr->text_font);
        }
        else {
            double  xxt1 = tp->x;
            double  yyt1 = tp->y;
            if (current_frame_num >= 0) {
                convert_frame_point (current_frame_num, &xxt1, &yyt1);
            }
            xt = (double)xxt1;
            yt = (double)yyt1;
            xr = xt * cang - yt * sang;
            yr = xt * sang + yt * cang;
            ezx_java_obj.ezx_SetDrawingPriority (3);
            gtx_drawprim_obj.gtx_cliptextprim((CSW_F)xr,
                             (CSW_F)yr,
                             tp->text,
                             1,
                             axptr->text_thickness,
                             axptr->text_red,
                             axptr->text_green,
                             axptr->text_blue,
                             axptr->text_red,
                             axptr->text_green,
                             axptr->text_blue,
                             tp->angle,
                             tp->size / page_units_per_inch,
                             axptr->text_font);
        }
    }

    gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
    gpf_graph_obj.gpf_FreeGraphText (gtexts, ngtexts);

    return 1;

}

/*
 * Return the description of an axis to the java code.
 */
void CDisplayList::return_selected_axes (DLSelectable *dls)
{
    int           *ilist, nlist;
    int           i;
    char          *fname, *iname, *lname;
    FRameStruct   *frptr;
    AXisPrim      *aptr;

    int          ap_size = (int)axis_prim_list.size();
    AXisPrim     *ap_data = axis_prim_list.data();

    if (ap_data == NULL  ||  ap_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    ilist = dls->GetAxes ();
    if (ilist == NULL) {
        return;
    }

    nlist = dls->GetNumAxes ();
    if (nlist < 1) {
        return;
    }

    for (i=0; i<nlist; i++) {

        aptr = ap_data + ilist[i];
        if (aptr->ap == NULL) {
            continue;
        }

        if (frame_list == NULL  ||
            aptr->frame_num < 0) {
            continue;
        }

        frptr = frame_list + aptr->frame_num;
        fname = frptr->name;

        lname = NULL;
        if (aptr->layer_num >= 0  &&  layer_list != NULL) {
            lname = layer_list[aptr->layer_num].name;
        }

        iname = NULL;
        if (aptr->item_num >= 0  &&  item_list != NULL) {
            iname = item_list[aptr->item_num].name;
        }

        ezx_java_obj.ezx_AppendSelectedAxisToJavaArea (aptr,
                                          fname,
                                          lname,
                                          iname);
    }

    return;
}



/*
 * Find the selectable object for the specified index
 * and return all its primitives to the java side.
 */
int CDisplayList::ReadSelectableObject (int ival)
{
    DLSelectable       *dls;

    if (selectable_object_list == NULL) {
        return -1;
    }

    if (num_selectable_object_list < 1) {
        return -1;
    }

    if (ival < 0  ||  ival >= num_selectable_object_list) {
        return -1;
    }

    dls = selectable_object_list[ival];
    if (dls == NULL) {
        return -1;
    }

    return_selected_lines (dls);
    return_selected_fills (dls);
    return_selected_texts (dls);
    return_selected_symbs (dls);
    return_selected_rectangles (dls);
    return_selected_arcs (dls);
    return_selected_contours (dls);
    return_selected_axes (dls);

    return 1;

}


void CDisplayList::erase_selected_lines (DLSelectable *dls)
{
    int             ido, i, nprim, *lines;
    LInePrim        *lptr;

    int             lp_size = (int)line_prim_list.size();
    LInePrim        *lp_data = line_prim_list.data();

    if (lp_data == NULL  ||  lp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumLines ();
    lines = dls->GetLines ();
    if (lines == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = lines[ido];

        lptr = lp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }

        if (HideFlag == 0) {
            csw_Free (lptr->xypts);
            lptr->xypts = NULL;
            lptr->npts = 0;
            lptr->maxpts = 0;
            lptr->deleted_flag = 1;
            add_available_line (i);
        }
        else {
            lptr->visible_flag = 0;
            add_hidden_line (i);
        }

    }

}


void CDisplayList::erase_selected_fills (DLSelectable *dls)
{
    int             ido, i, nprim, *fills;
    FIllPrim        *fptr;

    int            fp_size = (int)fill_prim_list.size();
    FIllPrim       *fp_data = fill_prim_list.data();

    if (fp_data == NULL  ||  fp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumFills ();
    fills = dls->GetFills ();
    if (fills == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = fills[ido];

        fptr = fp_data + i;

        if (fptr->deleted_flag == 1) {
            continue;
        }

        if (HideFlag == 0) {
            csw_Free (fptr->xypts);
            fptr->xypts = NULL;
            fptr->npts = 0;
            fptr->maxpts = 0;
            fptr->deleted_flag = 1;
            add_available_fill (i);
        }
        else {
            fptr->visible_flag = 0;
            add_hidden_fill (i);
        }

    }

}


void CDisplayList::erase_selected_texts (DLSelectable *dls)
{

    int             ido, i, nprim, *texts;
    TExtPrim        *tptr;

    int             tp_size = (int)text_prim_list.size();
    TExtPrim        *tp_data = text_prim_list.data();

    if (tp_data == NULL  ||  tp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumTexts ();
    texts = dls->GetTexts ();
    if (texts == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = texts[ido];

        tptr = tp_data + i;

        if (tptr->deleted_flag == 1) {
            continue;
        }

        if (HideFlag == 0) {
            csw_Free (tptr->chardata);
            tptr->chardata = NULL;
            tptr->deleted_flag = 1;
            add_available_text (i);
        }
        else {
            tptr->visible_flag = 0;
            add_hidden_text (i);
        }

    }

}


void CDisplayList::erase_selected_symbs (DLSelectable *dls)
{
    int             ido, i, nprim, *symbs;
    SYmbPrim        *sptr;

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (sp_data == NULL  ||  sp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumSymbs ();
    symbs = dls->GetSymbs ();
    if (symbs == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = symbs[ido];

        sptr = sp_data + i;

        if (sptr->deleted_flag == 1) {
            continue;
        }

        if (HideFlag == 0) {
            sptr->deleted_flag = 1;
            add_available_symb (i);
        }
        else {
            sptr->visible_flag = 0;
            add_hidden_symb (i);
        }

    }

}


void CDisplayList::erase_selected_shapes (DLSelectable *dls)
{
    int             ido, i, nprim, *arcs;
    SHapePrim       *rptr;

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (hp_data == NULL  ||  hp_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumShapes ();
    arcs = dls->GetShapes ();
    if (arcs == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = arcs[ido];

        rptr = hp_data + i;

        if (rptr->deleted_flag == 1) {
            continue;
        }

        if (HideFlag == 0) {
            rptr->deleted_flag = 1;
            add_available_shape (i);
        }
        else {
            rptr->visible_flag = 0;
            add_hidden_shape (i);
        }

    }

}



void CDisplayList::erase_selected_contours (DLSelectable *dls)
{
    int             ido, i, nprim, *conts;
    DLContour       *cptr;

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (cl_data == NULL  ||  cl_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    nprim = dls->GetNumContours ();
    conts = dls->GetContours ();
    if (conts == NULL  ||  nprim < 1) {
        return;
    }

    for (ido=0; ido<nprim; ido++) {

        i = conts[ido];

        cptr = cl_data[i];

        if (cptr->deleted_flag == 1) {
            continue;
        }

        if (HideFlag == 0) {
            COntourOutputRec *crec = cptr->GetCrec ();
            conapi_obj.con_FreeContours (crec, 1);
            cptr->SetCrec (NULL);
            cptr->deleted_flag = 1;
            add_available_contour (i);
        }
        else {
            cptr->visible_flag = 0;
            add_hidden_contour (i);
        }

    }

}


void CDisplayList::erase_selected_axes (DLSelectable *dls)
{
    int           *ilist, nlist;
    int           i;
    AXisPrim      *aptr;

    int          ap_size = (int)axis_prim_list.size();
    AXisPrim     *ap_data = axis_prim_list.data();

    if (ap_data == NULL  ||  ap_size < 1) {
        return;
    }

    if (dls == NULL) {
        return;
    }

    ilist = dls->GetAxes ();
    if (ilist == NULL) {
        return;
    }

    nlist = dls->GetNumAxes ();
    if (nlist < 1) {
        return;
    }

    for (i=0; i<nlist; i++) {

        aptr = ap_data + ilist[i];
        if (aptr->ap == NULL) {
            continue;
        }

        if (HideFlag == 0) {
            csw_Free (aptr->ap);
            aptr->ap = NULL;
            aptr->deleted_flag = 1;
            add_available_axis (i);
        }
        else {
            aptr->visible_flag = 0;
            add_hidden_axis (i);
        }
    }

    return;
}


/*------------------------------------------------------------------------*/


/*
 * private methods for managing the hidden primitive lists.
 */
void CDisplayList::free_hidden_lists (void)
{
    line_hidden_list.clear();
    fill_hidden_list.clear();
    text_hidden_list.clear();
    symb_hidden_list.clear();
    shape_hidden_list.clear();
    axis_hidden_list.clear();
    contour_hidden_list.clear();

    return;
}


int CDisplayList::add_hidden_contour (int prim_num) {

    int        cl_size = (int)contour_list.size();
    DLContour  **cl_data = contour_list.data();

    if (prim_num < 0  ||  prim_num >= cl_size  ||
          cl_data == NULL  ||  cl_size < 1) {
        return 0;
    }

    try {
        contour_hidden_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::add_hidden_line (int prim_num) {

    int               lp_size = (int)line_prim_list.size();
    LInePrim          *lp_data = line_prim_list.data();

    if (prim_num < 0  ||  prim_num >= lp_size  ||  lp_data == NULL) {
        return 0;
    }

    try {
        line_hidden_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::add_hidden_fill (int prim_num) {

    int            fp_size = (int)fill_prim_list.size();
    FIllPrim       *fp_data = fill_prim_list.data();

    if (prim_num < 0  ||  prim_num >= fp_size  ||  fp_data == NULL) {
        return 0;
    }

    try {
        fill_hidden_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::add_hidden_text (int prim_num) {

    int           tp_size = (int)text_prim_list.size();
    TExtPrim      *tp_data = text_prim_list.data();

    if (prim_num < 0  ||  prim_num >= tp_size  ||  tp_data == NULL) {
        return 0;
    }

    try {
        text_hidden_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::add_hidden_symb (int prim_num) {

    int        sp_size = (int)symb_prim_list.size();
    SYmbPrim   *sp_data = symb_prim_list.data();

    if (prim_num < 0  ||  prim_num >= sp_size  ||  sp_data == NULL) {
        return 0;
    }

    try {
        symb_hidden_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::add_hidden_shape (int prim_num) {

    int           hp_size = (int)shape_prim_list.size();
    SHapePrim     *hp_data = shape_prim_list.data();

    if (prim_num < 0  ||  prim_num >= hp_size  ||  hp_data == NULL) {
        return 0;
    }

    try {
        shape_hidden_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}


int CDisplayList::add_hidden_axis (int prim_num) {

    int          ap_size = (int)axis_prim_list.size();
    AXisPrim     *ap_data = axis_prim_list.data();

    if (prim_num < 0  ||  prim_num >= ap_size  ||  ap_data == NULL) {
        return 0;
    }

    try {
        axis_hidden_list.push_back (prim_num);
    }
    catch (...) {
        return -1;
    }

    return 1;
}



/*-----------------------------------------------------------------------------*/


int CDisplayList::ZoomExtentsForFrameName (
    char *name,
    double top_inset,
    double left_inset,
    double bottom_inset,
    double right_inset)
{
    char         local1[100], local2[100];
    int          i, cval;
    FRameStruct  *frptr;

    if (frame_list == NULL) {
        return -1;
    }

    csw_StrClean2 (local1, name, 100);
    for (i=0; i<num_frame_list; i++) {
        frptr = frame_list + i;
        csw_StrClean2 (local2, frptr->name, 100);
        cval = strcmp (local1, local2);
        if (cval == 0) {
            frptr->top_inset = top_inset;
            frptr->left_inset = left_inset;
            frptr->bottom_inset = bottom_inset;
            frptr->right_inset = right_inset;
            ZoomExtents (i);
            return 1;
        }
    }

    return -1;

}


int CDisplayList::CalcAxisBounds (AXisPrim *aptr,
                                  double *xmin_out,
                                  double *ymin_out,
                                  double *xmax_out,
                                  double *ymax_out)
{
    int           fnum;
    int           istat, ldir, tdir, ipos;
    FRameAxisStruct  fax;
    CSW_F         xt, yt;
    CSW_F         ax1, ay1, ax2, ay2, dx, dy, dist;
    CSW_F         aax1, aay1, aax2, aay2;
    double        ang, sang;
    double        xmin, ymin, xmax, ymax;

    *xmin_out = 1.e30;
    *ymin_out = 1.e30;
    *xmax_out = -1.e30;
    *ymax_out = -1.e30;

    fnum = aptr->frame_num;
    if (fnum < 0  ||  fnum >= num_frame_list) {
        return 0;
    }

    if (aptr->visible_flag == 0) {
        return 0;
    }

    memcpy (&fax, aptr->ap, sizeof(FRameAxisStruct));

    tdir = fax.tick_direction;
    ldir = aptr->label_dir;

    ax1 = aptr->fx1;
    ay1 = aptr->fy1;
    ax2 = aptr->fx2;
    ay2 = aptr->fy2;

    convert_frame_point (&ax1, &ay1);
    convert_frame_point (&ax2, &ay2);

    dx = ax2 - ax1;
    dy = ay2 - ay1;

    ang = atan2 ((double)dy, (double)dx);
    sang = sin (ang);

    dist = dx * dx + dy * dy;
    dist = (CSW_F)sqrt ((double)dist);

/*
 * Draw a horizontal axis and then rotate it.
 */
    if (sang < 0.1  &&  sang > -0.1) {
        ax2 = ax1 + dist;
        ay2 = ay1 + dist;
        ipos = 1;
        fax.tick_direction = 1;
        if (ldir == 1  &&  tdir < 0) {
            ipos = 1;
            fax.tick_direction = -1;
        }
        else if (ldir == 1  &&  tdir > 0) {
            ipos = 3;
            fax.tick_direction = -1;
        }
        else if (ldir == -1  &&  tdir < 0) {
            ipos = 3;
            fax.tick_direction = 1;
        }
        else if (ldir == -1  &&  tdir > 0) {
            ipos = 1;
            fax.tick_direction = 1;
        }
        aax1 = ax1;
        aax2 = ax2;
        if (ipos == 1) {
            aay1 = ay1;
            aay2 = ay2;
        }
        else {
            aay1 = ay1 - dist;
            aay2 = ay1;
        }
        istat =
        calc_axis_limits (
            fnum,
            &fax,
            aax1,
            aay1,
            aax2,
            aay2,
            aptr->afirst,
            aptr->alast,
            ipos,
            &xmin,
            &ymin,
            &xmax,
            &ymax);
    }

/*
 * Draw a vertical axis and then rotate it.
 */
    else {

        ax2 = ax1 + dist;
        ay2 = ay1 + dist;
        ipos = 2;
        fax.tick_direction = 1;
        if (ldir == 1  &&  tdir < 0) {
            ipos = 4;
            fax.tick_direction = -1;
        }
        else if (ldir == 1  &&  tdir > 0) {
            ipos = 2;
            fax.tick_direction = -1;
        }
        else if (ldir == -1  &&  tdir < 0) {
            ipos = 2;
            fax.tick_direction = 1;
        }
        else if (ldir == -1  &&  tdir > 0) {
            ipos = 4;
            fax.tick_direction = 1;
        }
        aay1 = ay1;
        aay2 = ay2;
        if (ipos == 2) {
            aax1 = ax1;
            aax2 = ax2;
        }
        else {
            aax1 = ax1 - dist;
            aax2 = ax1;
        }
        istat =
        calc_axis_limits (
            fnum,
            &fax,
            aax1,
            aay1,
            aax2,
            aay2,
            aptr->afirst,
            aptr->alast,
            ipos,
            &xmin,
            &ymin,
            &xmax,
            &ymax);
    }

    if (istat == -1) {
        return -1;
    }

    xt = (CSW_F)xmin;
    yt = (CSW_F)ymin;
    unconvert_frame_point (&xt, &yt);
    xmin = xt;
    ymin = yt;

    xt = (CSW_F)xmax;
    yt = (CSW_F)ymax;
    unconvert_frame_point (&xt, &yt);
    xmax = xt;
    ymax = yt;

    *xmin_out = xmin;
    *ymin_out = ymin;
    *xmax_out = xmax;
    *ymax_out = ymax;

    return istat;
}



int CDisplayList::calc_axis_limits (
     int fnum,
     FRameAxisStruct *axptr,
     CSW_F px1,
     CSW_F py1,
     CSW_F px2,
     CSW_F py2,
     CSW_F val1,
     CSW_F val2,
     int position,
     double *xmin,
     double *ymin,
     double *xmax,
     double *ymax)
{
    GRaphUnion         uval;
    GRaphLineRec       *glines;
    GRaphTextRec       *gtexts;
    int                nglines,
                       ngtexts;
    int                istat,
                       gpos;
    CSW_F              axmin,
                       axmax,
                       axmajor;
    CSW_F              ax1, ay1, ax2, ay2;

    fnum = fnum;

/*
 * Set the graph axis position.
 */
    if (position == 1) {
        gpos = GPF_BOTTOM_AXIS;
    }
    else if (position == 2) {
        gpos = GPF_LEFT_AXIS;
    }
    else if (position == 3) {
        gpos = GPF_TOP_AXIS;
    }
    else if (position == 4) {
        gpos = GPF_RIGHT_AXIS;
    }
    else {
        return 0;
    }

/*
 * Set all the graph parameters to their defaults.
 */
    gpf_graph_obj.gpf_SetGraphDefaults ();

/*
 * Define the position of the frame on the plot page for the
 * graph utilities.
 */
    gpf_graph_obj.gpf_StartGraph (px1,
                    py1,
                    px2,
                    py2,
                    page_units_per_inch,
                    0,    /* only used for time graphs */
                    page_units_per_inch
                   );

/*
 * Set the axis parameters that are controlled by the frame axis structure.
 */
  // label anchor and position
    uval.ival = axptr->label_anchor;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_ANCHOR, uval);
    uval.fval = (CSW_F)axptr->label_angle;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_ANGLE, uval);

  // text fonts
    uval.ival = axptr->text_font;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_FONT, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_FONT, uval);

  // text sizes
    uval.fval = axptr->text_size * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_SIZE, uval);
    uval.fval *= 1.1f;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_SIZE, uval);

  // major interval
    if (axptr->major_interval > 0.0) {
        uval.fval = axptr->major_interval;
        gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_INTERVAL, uval);
    }

  // min and max values on the axis
    uval.fval = val1;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MIN_VALUE, uval);
    uval.fval = val2;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAX_VALUE, uval);

  // minor and major tick lengths
    uval.fval = .05f * page_units_per_inch * axptr->tick_direction;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_LENGTH, uval);
    uval.fval = .08f * page_units_per_inch * axptr->tick_direction;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_LENGTH, uval);

    uval.fval = .05f * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_GAP, uval);
    uval.fval = .08f * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_GAP, uval);

  // line thickness
    uval.fval = axptr->line_thickness * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LINE_THICK, uval);

  // text thickness
    uval.fval = axptr->text_thickness * page_units_per_inch;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_THICK, uval);
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_THICK, uval);

/*
 * Generate graph primitive structures for the axis.
 */
    istat = gpf_graph_obj.gpf_CalcGraphAxis (
        gpos,
        axptr->caption,
        val1,
        val2,
        0.0, // linear axis
        axptr->label_flag,
        NULL, // no custom labels
        NULL,
        0,
        &axmin,
        &axmax,
        &axmajor,
        &glines,
        &nglines,
        &gtexts,
        &ngtexts
    );

    if (istat == -1) {
        return -1;
    }

/*
 * Find the limits for primitives in the axis.
 */
    gpf_graph_obj.gpf_get_axis_drawing_limits (
        &ax1, &ay1, &ax2, &ay2
    );

    *xmin = ax1;
    *ymin = ay1;
    *xmax = ax2;
    *ymax = ay2;

    gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
    gpf_graph_obj.gpf_FreeGraphText (gtexts, ngtexts);

    return 1;

}


void CDisplayList::backscalef (
    CSW_F     sx,
    CSW_F     sy,
    CSW_F     *px,
    CSW_F     *py)
{
    gtx_drawprim_obj.gtx_init_drawing (
                      (CSW_F)page_xmin, (CSW_F)page_ymin,
                      (CSW_F)page_xmax, (CSW_F)page_ymax,
                      (CSW_F)page_xmin, (CSW_F)page_ymin,
                      (CSW_F)page_xmax, (CSW_F)page_ymax,
                      (CSW_F)screen_xmin, (CSW_F)screen_ymin,
                      (CSW_F)screen_xmax, (CSW_F)screen_ymax,
                      screen_dpi, page_units_type, NULL);

    gtx_drawprim_obj.gtx_backscalef (sx, sy, px, py);

}


/*----------------------------------------------------------------------------*/


int CDisplayList::SetImageFaultData (
    double    *xf,
    double    *yf,
    int       *npf,
    int       nf,
    int       ntot)
{
    csw_Free (ImageXFault);
    csw_Free (ImageNFaultPoints);

    ImageXFault = NULL;
    ImageYFault = NULL;
    ImageNFaultPoints = NULL;
    ImageNFaults = 0;
    ImageNFaultTotal = 0;

    if (nf < 1) {
        return 1;
    }

    int    i, nt2;

    nt2 = 0;
    for (i=0; i<nf; i++) {
        nt2 += npf[i];
    }

    if (nt2 != ntot) {
        printf ("ntot not equal to nt2 in SetImageFaultData\n");
        if (nt2 > ntot) ntot = nt2;
    }

    ImageXFault = (double *)csw_Malloc (2 * ntot * sizeof(double));
    if (ImageXFault == NULL) {
        return -1;
    }
    ImageYFault = ImageXFault + ntot;
    ImageNFaultPoints = (int *)csw_Malloc (nf * sizeof(int));
    if (ImageNFaultPoints == NULL) {
        csw_Free (ImageXFault);
        ImageXFault = NULL;
        ImageYFault = NULL;
        return -1;
    }

    memcpy (ImageXFault, xf, ntot * sizeof(double));
    memcpy (ImageYFault, yf, ntot * sizeof(double));
    memcpy (ImageNFaultPoints, npf, nf * sizeof(int));

    ImageNFaults = nf;
    ImageNFaultTotal = ntot;

    return 1;

}

/*----------------------------------------------------------------------------*/


/*
 * This version is needed to tie together images and vector
 * primitives tightly bound to the image.  The image id is used here
 * and for the vector primitives.  The image data is passed as a CSW_F
 * pointer in this version.
 */
int CDisplayList::AddGridImage (int fnum,
                                int image_id,
                                CSW_F *data,
                                int ncol,
                                int nrow,
                                float null_value,
                                double xmin,
                                double ymin,
                                double xmax,
                                double ymax)
{
    int            istat, i, j, k, offset, ntot;
    unsigned char  *red_ptr = NULL, *green_ptr = NULL,
                   *blue_ptr = NULL, *trans_ptr = NULL;
    int            red, green, blue, trans;

    auto fscope = [&]()
    {
        csw_Free (red_ptr);
        csw_Free (green_ptr);
        csw_Free (blue_ptr);
        csw_Free (trans_ptr);
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (xmax <= xmin || ymax <= ymin ||
        data == NULL || ncol < 2  ||  nrow < 2) {
        return 0;
    }

/*
 * Allocate space for color image data.
 */
    ntot = ncol * nrow;
    red_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    green_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    blue_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    trans_ptr = (unsigned char *)csw_Calloc (ntot * sizeof(unsigned char));
    if (red_ptr == NULL  ||  green_ptr == NULL  ||  blue_ptr == NULL  ||
        trans_ptr == NULL) {
        return -1;
    }

/*
 * convert data image to color image.
 */
    current_image_null_value = (double)null_value;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            lookup_image_color (data[k],
                                &red, &green, &blue, &trans);
            red_ptr[k] = (unsigned char)red;
            green_ptr[k] = (unsigned char)green;
            blue_ptr[k] = (unsigned char)blue;
            trans_ptr[k] = (unsigned char)trans;
        }
    }

    istat = AddGridColorImage (fnum,
                           image_id,
                           red_ptr,
                           green_ptr,
                           blue_ptr,
                           trans_ptr,
                           ncol,
                           nrow,
                           xmin,
                           ymin,
                           xmax,
                           ymax);

    return istat;

}  /*  end of function AddGridDataImage  (CSW_F) */





bool CDisplayList::CheckInsideFrame (int fnum,
                                     double x1, double y1,
                                     double x2, double y2)
{
    bool    bret = false;

    if (frame_list == NULL  ||  num_frame_list < 1) {
        return bret;
    }

    if (fnum < 0  ||  fnum > num_frame_list - 1) {
        return bret;
    }

    double   dt;
    if (x1 > x2) {
        dt = x1;
        x1 = x2;
        x2 = dt;
    }
    if (y1 > y2) {
        dt = y1;
        y1 = y2;
        y2 = dt;
    }

    FRameStruct  *frptr = frame_list + fnum;

    bool    xout = false;
    bool    yout = false;
 
    if (frptr->x1 < frptr->x2) { 
      if (x1 > frptr->x2  ||
          x2 < frptr->x1) {
        xout = true;
      }
    }
    else {
      if (x2 < frptr->x2  ||
          x1 > frptr->x1) {
        xout = true;
      }
    }

    if (frptr->y1 < frptr->y2) { 
      if (y1 > frptr->y2  ||
          y2 < frptr->y1) {
        yout = true;
      }
    }
    else {
      if (y2 < frptr->y2  ||
          y1 > frptr->y1) {
        yout = true;
      }
    }

    if (xout == false  &&  yout == false) {
        bret = true;
    }

    return bret;

}




void CDisplayList::convert_frame_array (int fnum, CSW_F *xy, int npts)
{
    int             i;
    CSW_F           sx, sy;
    CSW_F           fx1, fy1, fx2, fy2;
    CSW_F           px1, py1, px2, py2;

    if (npts < 1) {
        return;
    }

    if (fnum < 0  ||  fnum >= num_frame_list  ||  frame_list == NULL) {
        return;
    }

    FRameStruct  *frptr = frame_list + fnum;

    px1 = frptr->px1;
    py1 = frptr->py1;
    px2 = frptr->px2;
    py2 = frptr->py2;
    fx1 = frptr->x1;
    fy1 = frptr->y1;
    fx2 = frptr->x2;
    fy2 = frptr->y2;

/*
 * scales are in page units per frame unit
 */
    sx = (px2 - px1) / (fx2 - fx1);
    sy = (py2 - py1) / (fy2 - fy1);

    double *x = xy;
    double *y = xy + 1;

    for (i=0; i<npts; i++) {
        if (*x < _DL_XY_TOO_BIG_  &&  *y < _DL_XY_TOO_BIG_) {
            *x = (*x - fx1) * sx + px1;
            *y = (*y - fy1) * sy + py1;
        }
        if (i < npts-1) {
            x+=2;
            y+=2;
        }
    }

    return;
}



void CDisplayList::unconvert_frame_array (int fnum, CSW_F *xy, int npts)
{
    CSW_F           *x, *y;
    int             i;
    CSW_F           sx, sy;
    CSW_F           fx1, fy1, fx2, fy2;
    CSW_F           px1, py1, px2, py2;

    if (npts < 1) {
        return;
    }

    if (fnum < 0  ||  fnum >= num_frame_list  ||  frame_list == NULL) {
        return;
    }

    FRameStruct  *frptr = frame_list + fnum;

    px1 = frptr->px1;
    py1 = frptr->py1;
    px2 = frptr->px2;
    py2 = frptr->py2;
    fx1 = frptr->x1;
    fy1 = frptr->y1;
    fx2 = frptr->x2;
    fy2 = frptr->y2;

/*
 * scales are in Frame units per page unit
 */
    sx = (fx2 - fx1) / (px2 - px1);
    sy = (fy2 - fy1) / (py2 - py1);

    x = xy;
    y = xy+1;

    for (i=0; i<npts; i++) {
        if (*x < _DL_XY_TOO_BIG_  &&  *y < _DL_XY_TOO_BIG_) {
            *x = (*x - px1) * sx + fx1;
            *y = (*y - py1) * sy + fy1;
        }
        if (i < npts-1) {
            x+=2;
            y+=2;
        }
    }

    return;
}





void CDisplayList::convert_frame_point (int fnum, CSW_F *x, CSW_F *y)
{
    CSW_F           sx, sy;
    CSW_F           fx1, fy1, fx2, fy2;
    CSW_F           px1, py1, px2, py2;

    if (fnum < 0  ||  fnum >= num_frame_list  ||  frame_list == NULL) {
        return;
    }

    FRameStruct  *frptr = frame_list + fnum;

    px1 = frptr->px1;
    py1 = frptr->py1;
    px2 = frptr->px2;
    py2 = frptr->py2;
    fx1 = frptr->x1;
    fy1 = frptr->y1;
    fx2 = frptr->x2;
    fy2 = frptr->y2;

/*
 * scales are in page units per frame unit
 */
    sx = (px2 - px1) / (fx2 - fx1);
    sy = (py2 - py1) / (fy2 - fy1);

    if (*x < _DL_XY_TOO_BIG_  &&  *y < _DL_XY_TOO_BIG_) {
        *x = (*x - fx1) * sx + px1;
        *y = (*y - fy1) * sy + py1;
    }

    return;
}



void CDisplayList::unconvert_frame_point (int fnum, CSW_F *x, CSW_F *y)
{
    CSW_F           sx, sy;
    CSW_F           fx1, fy1, fx2, fy2;
    CSW_F           px1, py1, px2, py2;

    if (fnum < 0  ||  fnum >= num_frame_list  ||  frame_list == NULL) {
        return;
    }

    FRameStruct  *frptr = frame_list + fnum;

    px1 = frptr->px1;
    py1 = frptr->py1;
    px2 = frptr->px2;
    py2 = frptr->py2;
    fx1 = frptr->x1;
    fy1 = frptr->y1;
    fx2 = frptr->x2;
    fy2 = frptr->y2;

/*
 * scales are in Frame units per page unit
 */
    sx = (fx2 - fx1) / (px2 - px1);
    sy = (fy2 - fy1) / (py2 - py1);

    if (*x < _DL_XY_TOO_BIG_  &&  *y < _DL_XY_TOO_BIG_) {
        *x = (*x - px1) * sx + fx1;
        *y = (*y - py1) * sy + fy1;
    }

    return;
}


/*
 *****************************************************************

              D r a w A l l C o n t o u r L i n e s

 *****************************************************************

  Draw all the contour lines in the display list.
*/

int CDisplayList::DrawAllContourLines (void)
{
    int            i, ido, nloop;
    LInePrim       *lptr;
    bool           bpatch = false;

    int            cp_size = (int)contour_line_prim_list.size();
    LInePrim       *cp_data = contour_line_prim_list.data();

    if (cp_data == NULL  ||  cp_size < 1) {
        return 0;
    }

    nloop = cp_size;
    int      pt_size = (int)contour_line_patch_list.size();
    int      *pt_data = contour_line_patch_list.data();

    if (pt_data != NULL  &&  pt_size > 0  &&  patch_draw_flag == 1) {
        nloop = pt_size;
        bpatch = true;
    }

    bool  bframe = false;

    for (ido=0; ido<nloop; ido++) {

        i = ido;
        if (bpatch) {
            i = pt_data[ido];
        }

        lptr = cp_data + i;

        if (lptr->deleted_flag == 1) {
            continue;
        }
        if (lptr->visible_flag == 0) {
            continue;
        }

    /*
     * Check the line bbox against the line frame.
     */
        bframe = false;
        if (lptr->frame_num >= 0) {
            bool bbchk = CheckInsideFrame (lptr->frame_num,
                                           lptr->xmin, lptr->ymin,
                                           lptr->xmax, lptr->ymax);
            if (!bbchk) {
                continue;
            }
            convert_frame_array (lptr->frame_num, lptr->xypts, lptr->npts);
            bframe = true;
        }

        ezx_java_obj.ezx_SetFrameInJavaArea (lptr->frame_num);
        ezx_java_obj.ezx_SetAlphaValue (lptr->alpha);

        gtx_drawprim_obj.gtx_SetImageIDForDraw (lptr->image_id);

        gtx_drawprim_obj.gtx_cliplineprim
                        (lptr->xypts,
                         lptr->npts,
                         lptr->smooth_flag,
                         (CSW_F)lptr->thick,
                         lptr->red,
                         lptr->green,
                         lptr->blue,
                         lptr->dashpat,
                         (CSW_F)lptr->dashscale,
                         lptr->arrowstyle);

        gtx_drawprim_obj.gtx_SetImageIDForDraw (-1);

        if (bframe) {
            unconvert_frame_array (lptr->frame_num, lptr->xypts, lptr->npts);
        }

    }

    return 0;

}  /*  end of function DrawAllContourLines */




/*
 * Calculate line bounds from a LInePrim pointer.
 * Populate the pointer with the results.
 */
int CDisplayList::CalcLineBounds (LInePrim *lptr)

{
    int             i;
    CSW_F           *xypts, *xyp;
    int             npts;
    int             nsout;
    CSW_F           xysmooth[2200];

    if (lptr == NULL) {
        return -1;
    }

    npts = lptr->npts;
    xypts = lptr->xypts;

    if (npts < 1  ||  xypts == NULL) {
        return -1;
    }

    if (lptr->smooth_flag > 0) {
        gpf_spline_obj.gpf_SplineFit2 (xypts, npts, 0,
                        xysmooth, 1000,
                        -1.0f, &nsout);
        xyp = xysmooth;
        npts = nsout;
    }
    else {
        xyp = xypts;
    }

    double xmin, ymin, xmax, ymax;

    xmin = ymin = 1.e30;
    xmax = ymax = -1.e30;

    for (i=0; i<npts; i++) {
        if (*xyp < xmin) xmin = *xyp;
        if (*xyp > xmax) xmax = *xyp;
        xyp++;
        if (*xyp < ymin) ymin = *xyp;
        if (*xyp > ymax) ymax = *xyp;
        xyp++;
    }

    lptr->xmin = xmin;
    lptr->ymin = ymin;
    lptr->xmax = xmax;
    lptr->ymax = ymax;

    return 1;

}  /*  end of function CalcLineBounds  */




void CDisplayList::convert_frame_dist (int fnum, CSW_F *dist)
{
    CSW_F           sx, sy;
    CSW_F           fx1, fy1, fx2, fy2;
    CSW_F           px1, py1, px2, py2;

    if (fnum < 0  ||  fnum >= num_frame_list  ||  frame_list == NULL) {
        return;
    }

    FRameStruct  *frptr = frame_list + fnum;

    px1 = frptr->px1;
    py1 = frptr->py1;
    px2 = frptr->px2;
    py2 = frptr->py2;
    fx1 = frptr->x1;
    fy1 = frptr->y1;
    fx2 = frptr->x2;
    fy2 = frptr->y2;

/*
 * scales are in page units per frame unit
 */
    sx = (px2 - px1) / (fx2 - fx1);
    sy = (py2 - py1) / (fy2 - fy1);

    sx = (sx + sy) / 2.0;
    if (sx < 0.0) sx = -sx;

    *dist *= sx;

    return;
}



void CDisplayList::unconvert_frame_dist (int fnum, CSW_F *dist)
{
    CSW_F           sx, sy;
    CSW_F           fx1, fy1, fx2, fy2;
    CSW_F           px1, py1, px2, py2;

    if (fnum < 0  ||  fnum >= num_frame_list  ||  frame_list == NULL) {
        return;
    }

    FRameStruct  *frptr = frame_list + fnum;

    px1 = frptr->px1;
    py1 = frptr->py1;
    px2 = frptr->px2;
    py2 = frptr->py2;
    fx1 = frptr->x1;
    fy1 = frptr->y1;
    fx2 = frptr->x2;
    fy2 = frptr->y2;

/*
 * scales are in Frame units per page unit
 */
    sx = (fx2 - fx1) / (px2 - px1);
    sy = (fy2 - fy1) / (py2 - py1);
    sx = (sx + sy) / 2.0;

    if (sx < 0.0) sx = -sx;

    *dist *= sx;

    return;
}
