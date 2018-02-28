
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/


/*
 ***************************************************************************

  Prototypes for jni related functions.

 ***************************************************************************
*/

#ifndef _DISPLAY_LIST_JNI_H_
#define _DISPLAY_LIST_JNI_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


    void jni_call_add_frame_method (
        void *v_jenv,
        void *v_jobj,
        float x1,
        float y1,
        float x2,
        float y2,
        double fx1,
        double fy1,
        double fx2,
        double fy2,
        int   borderflag,
        int   clipflag,
        int   scaleable,
        int   scale_to_attach_frame,
        int   frame_num,
        char  *fname
    );
    void jni_call_add_fill_method (
        void *v_jenv,
        void *v_jobj,
        float *xy,
        float thickness,
        float patscale,
        int npts,
        int red,
        int green,
        int blue,
        int alpha,
        int pattern,
        int frame_num,
        int selectable
    );
    void jni_call_add_line_method (
        void *v_jenv,
        void *v_jobj,
        float *xy,
        int npts,
        int red,
        int green,
        int blue,
        int alpha,
        int pattern,
        float thickness,
        int frame_num,
        int image_id,
        int selectable
    );
   void jni_call_add_text_method (
        void *v_jenv,
        void *v_jobj,
        float x,
        float y,
        char *text,
        int red,
        int green,
        int blue,
        int alpha,
        float angle,
        float size,
        int font,
        int frame_num,
        int image_id,
        int selectable
    );
   void jni_call_add_arc_method (
        void *v_jenv,
        void *v_jobj,
        float x,
        float y,
        float r1,
        float r2,
        float ang1,
        float ang2,
        int closure,
        int red,
        int green,
        int blue,
        int alpha,
        float thickness,
        float angle,
        int frame_num,
        int selectable
    );
   void jni_call_add_filled_arc_method (
        void *v_jenv,
        void *v_jobj,
        float x,
        float y,
        float r1,
        float r2,
        float ang1,
        float ang2,
        int closure,
        int red,
        int green,
        int blue,
        int alpha,
        float thickness,
        float angle,
        int pattern,
        int frame_num,
        int selectable
    );
   void jni_call_add_image_method (
        void *v_jenv,
        void *v_jobj,
        float x1,
        float y1,
        float x2,
        float y2,
        int ncol,
        int nrow,
        unsigned char *red,
        unsigned char *green,
        unsigned char *blue,
        unsigned char *transparency,
        int frame_num,
        int has_lines,
        int image_id,
        int selectable
    );




    void jni_call_add_selected_fill (
       int             selectable_index,
       double          *xp,
       double          *yp,
       int             *npts,
       int             ncomp,
       double          thick,
       double          patscale,
       double          dashscale,
       int             red_fill,
       int             green_fill,
       int             blue_fill,
       int             alpha_fill,
       int             red_pat,
       int             green_pat,
       int             blue_pat,
       int             alpha_pat,
       int             red_border,
       int             green_border,
       int             blue_border,
       int             alpha_border,
       char            *fname,
       char            *lname,
       char            *iname,
       int             fillpat,
       int             linepat,
       int             native_index
    );

    void jni_call_add_selected_line (
        int             selectable_index,
        double          *xp,
        double          *yp,
        int             npts,
        double          thick,
        double          dashscale,
        int             red_line,
        int             green_line,
        int             blue_line,
        int             alpha_line,
        char            *fname,
        char            *lname,
        char            *iname,
        int             linepat,
        int             symbol,
        int             arrow_style,
        int             native_index
    );

    void jni_call_add_selected_contour (
        int             selectable_index,
        double          *xp,
        double          *yp,
        int             npts,
        double          zval,
        double          thick,
        int             major,
        int             red_cont,
        int             green_cont,
        int             blue_cont,
        int             alpha_cont,
        char            *label,
        int             label_font,
        double          label_size,
        double          label_space,
        double          tick_len,
        double          tick_space,
        int             tick_dir,
        char            *fname,
        char            *lname,
        char            *iname,
        char            *sname,
        int             native_index
    );
    
    void jni_call_add_selected_rectangle (
        int             selectable_index,
        double          xc,
        double          yc,
        double          xr,
        double          yr,
        double          crad,
        double          rang,
        double          thick,
        double          patscale,
        double          dashscale,
        int             red_fill,
        int             green_fill,
        int             blue_fill,
        int             alpha_fill,
        int             red_pat,
        int             green_pat,
        int             blue_pat,
        int             alpha_pat,
        int             red_border,
        int             green_border,
        int             blue_border,
        int             alpha_border,
        char            *fname,
        char            *lname,
        char            *iname,
        int             fillpat,
        int             linepat,
        int             native_index
    );
    
    void jni_call_add_selected_arc (
        int             selectable_index,
        double          xc,
        double          yc,
        double          xr,
        double          yr,
        double          ang1,
        double          anglen,
        double          rang,
        int             closure,
        double          thick,
        double          patscale,
        double          dashscale,
        int             red_fill,
        int             green_fill,
        int             blue_fill,
        int             alpha_fill,
        int             red_pat,
        int             green_pat,
        int             blue_pat,
        int             alpha_pat,
        int             red_border,
        int             green_border,
        int             blue_border,
        int             alpha_border,
        char            *fname,
        char            *lname,
        char            *iname,
        int             fillpat,
        int             linepat,
        int             native_index
    );
    
    void jni_call_add_selected_text (
        int             selectable_index,
        double          xp,
        double          yp,
        int             anchor,
        double          thick,
        double          bgthick,
        double          angle,
        double          size,
        double          xoff,
        double          yoff,
        int             red_text,
        int             green_text,
        int             blue_text,
        int             alpha_text,
        int             red_fill,
        int             green_fill,
        int             blue_fill,
        int             alpha_fill,
        int             red_bgfill,
        int             green_bgfill,
        int             blue_bgfill,
        int             alpha_bgfill,
        int             red_bgborder,
        int             green_bgborder,
        int             blue_bgborder,
        int             alpha_bgborder,
        int             bgflag,
        int             font,
        char            *textdata,
        char            *fname,
        char            *lname,
        char            *iname,
        int             native_index
    );
    
    void jni_call_add_selected_symb (
        int             selectable_index,
        double          xp,
        double          yp,
        int             number,
        double          thick,
        double          angle,
        double          size,
        int             red_symb,
        int             green_symb,
        int             blue_symb,
        int             alpha_symb,
        char            *fname,
        char            *lname,
        char            *iname,
        char            *sname,
        int             native_index
    );

    void jni_call_add_selected_axis (
        int         label_flag,
        int         tick_flag,
        int         tick_direction,
        char        *caption,
        double      major_interval,
        int         line_red,
        int         line_green,
        int         line_blue,
        int         text_red,
        int         text_green,
        int         text_blue,
        double      line_thickness,
        double      text_size,
        double      text_thickness,
        int         text_font,
        double      fx1,
        double      fy1,
        double      fx2,
        double      fy2,
        int         label_dir,
        int         selectable_object_num,
        double      afirst,
        double      alast,
        char        *frame_name,
        char        *layer_name,
        char        *item_name,
        int         prim_num
    );

    void jni_call_add_symb_fill_method (
        float *xy,
        float thickness,
        float patscale,
        int npts,
        int red,
        int green,
        int blue,
        int alpha,
        int pattern,
        int frame_num,
        int image_id,
        int selectable
    );
    void jni_call_add_symb_line_method (
        float *xy,
        int npts,
        int red,
        int green,
        int blue,
        int alpha,
        int pattern,
        float thickness,
        int frame_num,
        int image_id,
        int selectable
    );
   void jni_call_add_symb_arc_method (
        float x,
        float y,
        float r1,
        float r2,
        float ang1,
        float ang2,
        int closure,
        int red,
        int green,
        int blue,
        int alpha,
        float thickness,
        float angle,
        int frame_num,
        int image_id,
        int selectable
    );
   void jni_call_add_symb_filled_arc_method (
        float x,
        float y,
        float r1,
        float r2,
        float ang1,
        float ang2,
        int closure,
        int red,
        int green,
        int blue,
        int alpha,
        float thickness,
        float angle,
        int pattern,
        int frame_num,
        int image_id,
        int selectable
    );

    void jni_call_set_zoom_pan_data_method (
        void  *v_jenv,
        void  *v_jobj,
        char       *fname,
        double     ox1,
        double     oy1,
        double     ox2,
        double     oy2,
        double     nx1,
        double     ny1,
        double     nx2,
        double     ny2
    );

    void jni_msg (char const *text);

    int jni_get_text_bounds (
        int     dlist_index,
        const char    *text,
        int     font_num,
        float   fsize,
        float   *bounds);

    void jni_return_converted_xyz (
        double  x,
        double  y,
        double  z);

    FILE *jni_get_prim_file_ezx ();
    void jni_set_prim_file_ezx (FILE *pf);
    void jni_open_prim_file_ezx (char *fname);
    void jni_close_prim_file_ezx ();

#ifdef __cplusplus
}
#endif

#endif
