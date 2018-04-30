/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/
    
/*
    gtx_drawprim.h
    
    This header file defines the GTXDrawPrim class.  This class
    refactors the old functions in gtx_drawprim.c.
*/
    
    
/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gtx_drawprimP.h.
#endif
    
    
    
#ifndef GTX_DRAWPRIM_H
#define GTX_DRAWPRIM_H

#include <stdio.h>
#include "csw/jeasyx/private_include/gtx_frinfoP.h"
    
    
#define XYMAX 2000
#define XYMAX2 5000
#define MAXSMOOTH 1000
#define IWORKSIZE 2000
#define MAXFONT 1000
#define MAXFONT2 2000
#define MAXPARTS 100

#define ARCRESOLUTION           2.0

#ifndef HOLEFLAG
#define HOLEFLAG   1.e19f
#endif

#define SCREEN_UNITS_PER_INCH   72.0f


#include "csw/jeasyx/private_include/EZXJavaArea.h"


#include "csw/utils/private_include/gpf_ldash.h"
#include "csw/utils/private_include/gpf_calcdraw.h"
#include "csw/utils/private_include/gpf_spline.h"
#include "csw/utils/private_include/gpf_font.h"
#include "csw/utils/private_include/gpf_symb.h"
#include "csw/utils/private_include/gpf_shape.h"
#include "csw/utils/private_include/ply_patfill.h"



class GTXDrawPrim
{

  private:

    GPFLdash       gpf_ldash_obj;
    GPFCalcdraw    gpf_calcdraw_obj;
    GPFSpline      gpf_spline_obj;
    GPFFont        gpf_font_obj;
    GPFSymbol      gpf_symbol_obj;
    GPFShape       gpf_shape_obj;
    CSWPolyPatfill ply_patfill_obj;

    EZXJavaArea    *ezx_java_ptr = NULL;

    int            dlist_index = -1;


  public:

    GTXDrawPrim () {gtx_InitDrawPrim ();};
    ~GTXDrawPrim () {gtx_FreeDrawPrim ();};

  private:

    GTXDrawPrim (const GTXDrawPrim &other) {};
    const GTXDrawPrim &operator= (const GTXDrawPrim &other) {return *this;};
    GTXDrawPrim (const GTXDrawPrim &&other) {};
    const GTXDrawPrim &operator= (const GTXDrawPrim &&other) {return *this;};

  public:

    void SetEZXJavaPtr (EZXJavaArea *p) {ezx_java_ptr = p;};
    void SetDlistIndex (int index) {dlist_index = index;};


  private:    

    int                    initdone = 0;

    int                    ImageID = -1;
    int                    ImageHasLines = 0;

    CSW_F                  ScreenUnitsPerInch = SCREEN_UNITS_PER_INCH;

    CSW_F                  ExactTextLength = -1.0f;

    CSW_F                  *XYworkbuf = NULL, 
                           *XYwork2 = NULL,
                           *XYarc = NULL,
                           *XYsmooth = NULL,
                           *Fontx = NULL, 
                           *Fonty = NULL,
                           *Fontxy = NULL,
                           *Xthin = NULL,
                           *Ythin = NULL,
                           *XYthin = NULL,
                           *XYclip = NULL;
    int                    *Iwork1 = NULL,
                           *Iwork2 = NULL,
                           *Iwork3 = NULL,
                           *Iclip = NULL,
                           FontComp[20],
                           FontHole[50];
    CSW_F                  fxmin, Fx1sav,
                           fymin, Fy1sav,
                           fxmax, Fx2sav,
                           fymax, Fy2sav,
                           scalx,
                           scaly,
                           ThinDist = 0.0,
                           Tiny;
    CSW_F                  OrigXmin,
                           OrigYmin;
    CSW_F                  *XYStatic = NULL;
    CSW_F                  ixmin,
                           iymin,
                           ixmax,
                           iymax;

    int                    PageUnitsType = 1;

    int                    ThinFlag = 1,
                           DashFlag = 0,
                           ShapeFillFlag = 0,
                           AlreadyClipping = 0;

    CSW_F                  HoleFlag2 = HOLEFLAG / 10.0f;

    int                    FillPattern = 0;
    CSW_F                  PatternScale = 1.0f;

    int                    RedFG = 0,
                           GreenFG = 0,
                           BlueFG = 0;
    int                    RedBG = 255,
                           GreenBG = 255,
                           BlueBG = 255;

    int                    FillAlpha = 255,
                           PatternAlpha = 255,
                           BorderAlpha = 255;

    int                    SelectedFlag = 0;
    int                    TextFromSymbol = 0;

    int                    PolygonBorderFlag = 0;
    int                    TextPolyFlag = 0;

    CSW_F                  PageUnitsPerInch = 1.e30f;

    FILE                   *dfile = NULL;


    int ArrowCalc (CSW_F x1, CSW_F y1t, CSW_F x2, CSW_F y2, CSW_F size,
                   int style, CSW_F *x);

    CSW_F exact_text_length (char *text, CSW_F size, int font);
    CSW_F stroke_text_length (char *text, CSW_F size, int font);

 
  public:
    
    void gtx_SetImageIDForDraw (int id);
    void gtx_SetImageHasLinesForDraw (int val);
    
    void gtx_set_poly_alpha (int afill, int apat, int abord);
    
    void gtx_set_page_shift (CSW_F xshift, CSW_F yshift);
    
    int gtx_setpcliptest(
        CSW_F x1,
        CSW_F y1,
        CSW_F x2,
        CSW_F y2);
    
    int gtx_setextarc(
        CSW_F a1,
        CSW_F a2);
    
    int gtx_InitDrawPrim(
        void);
    
    int gtx_FreeDrawPrim(
        void);
    
    int gtx_scale_size (CSW_F *size);
    
    int gtx_scalef (
        CSW_F x,
        CSW_F y,
        CSW_F *fxout,
        CSW_F *fyout);
    
    int gtx_scale_array_2f (
        CSW_F *x,
        CSW_F *y,
        int   npts,
        CSW_F *xout,
        CSW_F *yout);
    
    int gtx_scale_arrayf (
        CSW_F *xy,
        CSW_F *xyout,
        int   npts);
    
    int gtx_backscalef (
        CSW_F x,
        CSW_F y,
        CSW_F *fxout,
        CSW_F *fyout);
    
    int gtx_backscale_arrayf (
        CSW_F *xy,
        CSW_F *xyout,
        int   npts);
    
    int gtx_backscale_array_2f (
        CSW_F *x,
        CSW_F *y,
        int   npts,
        CSW_F *xout,
        CSW_F *yout);
    
    int gtx_cliplineprim(
        CSW_F * xyorig,
        int norig,
        char smflag,
        CSW_F thick,
        int red,
        int green,
        int blue,
        int pattern,
        CSW_F dashscale,
        int arrowstyle);
    
    int gtx_drawfillprim(
        CSW_F * coords,
        int npt,
        int fred,
        int fgreen,
        int fblue);
    
    int gtx_drawlineprim(
        CSW_F * coords,
        int npt,
        CSW_F thick,
        int red,
        int green,
        int blue,
        int pattern);
    
    int gtx_set_fg_bg_for_drawing (
        int red_fg, int green_fg, int blue_fg,
        int red_bg, int green_bg, int blue_bg);
    
    int gtx_setthick(
        CSW_F thickin);
    
    int gtx_setlinedashes(
        int patnum);
    
    int gtx_get_init_widget(
        void);
    
    int gtx_init_drawing(
        CSW_F page_xmin,
        CSW_F page_ymin,
        CSW_F page_xmax,
        CSW_F page_ymax,
        CSW_F lclip_xmin,
        CSW_F lclip_ymin,
        CSW_F lclip_xmax,
        CSW_F lclip_ymax,
        CSW_F screen_for_xmin,
        CSW_F screen_for_ymin,
        CSW_F screen_for_xmax,
        CSW_F screen_for_ymax,
        int   screen_dpi,
        int   page_units_type,
        FILE *dfile);
    
    void gtx_get_drawing_clip_limits (
        CSW_F *xmin,
        CSW_F *ymin,
        CSW_F *xmax,
        CSW_F *ymax
    );
    
    void gtx_get_drawing_line_clip_limits (
        CSW_F *xmin,
        CSW_F *ymin,
        CSW_F *xmax,
        CSW_F *ymax
    );
    
    void gtx_get_page_units_per_inch (CSW_F *punits);
    
    int gtx_GetCurrentScale(
        CSW_F * sx,
        CSW_F * sy);
    
    int gtx_cliptextprim(
        CSW_F x,
        CSW_F y,
        char * text,
        int anchor,
        CSW_F thick,
        int red, int green, int blue,
        int fred, int fgreen, int fblue,
        CSW_F angle,
        CSW_F size,
        int font);
    
    int gtx_cliptextrect(
        CSW_F x,
        CSW_F y,
        char * text,
        int anchor,
        CSW_F thick,
        int red, int green, int blue,
        int fred, int fgreen, int fblue,
        CSW_F angle,
        CSW_F size,
        int font,
        int bgflag);
    
    int gtx_clipfillprim(
        CSW_F * coords,
        int npt,
        char smoothflag,
        int fred, int fgreen, int fblue,
        int pred, int pgreen, int pblue,
        int bred, int bgreen, int bblue,
        CSW_F thick,
        int outline,
        int pattern,
        CSW_F patscale,
        int linepatt,
        CSW_F dashscale,
        CSW_F x1,
        CSW_F y1,
        CSW_F x2,
        CSW_F y2);
    
    int gtx_clipfilloutline(
        int outline,
        CSW_F * xyin,
        int nin,
        char smflag,
        int red, int green, int blue,
        CSW_F thick,
        int linepatt,
        CSW_F dashscale,
        CSW_F x1,
        CSW_F y1,
        CSW_F x2,
        CSW_F y2);
    
    int gtx_clipsymbprim(
        CSW_F x,
        CSW_F y,
        int symb,
        CSW_F size,
        CSW_F thick,
        int red, int green, int blue,
        CSW_F angle,
        int mask);
    
    int gtx_drawtextprim(
        CSW_F x,
        CSW_F y,
        char * text,
        int nc,
        CSW_F thick,
        int red, int green, int blue,
        int fred, int fgreen, int fblue,
        CSW_F angle,
        CSW_F size,
        int font);
    
    int gtx_drawtextasline(
        CSW_F xanc,
        CSW_F yanc,
        char * text,
        int nc,
        CSW_F size,
        CSW_F angle,
        int red, int green, int blue);
    
    int gtx_drawstroketext(
        CSW_F x,
        CSW_F y,
        char * text,
        int nc,
        CSW_F thick,
        int red, int green, int blue,
        CSW_F angle,
        CSW_F size,
        int font);
    
    int gtx_drawpolytext(
        CSW_F x,
        CSW_F y,
        char * text,
        int nc,
        CSW_F thick,
        int red, int green, int blue,
        int fred, int fgreen, int fblue,
        CSW_F angle,
        CSW_F size,
        int font);
    
    int gtx_drawsymbprim(
        CSW_F x,
        CSW_F y,
        int symb,
        CSW_F size,
        CSW_F thick,
        int red, int green, int blue,
        CSW_F angle,
        int mask);
    
    int gtx_drawcircle(
        CSW_F x,
        CSW_F y,
        CSW_F r1,
        CSW_F r2,
        CSW_F ang1,
        CSW_F ang2,
        int closure,
        int red, int green, int blue,
        CSW_F thick,
        CSW_F angle);
    
    int gtx_SetArcDebugFlag(
        int val);
    
    int gtx_drawfillcircle(
        CSW_F x,
        CSW_F y,
        CSW_F r1,
        CSW_F r2,
        CSW_F ang1,
        CSW_F ang2,
        int closure,
        int red, int green, int blue,
        CSW_F angle);
    
    int gtx_clipfillcircle(
        CSW_F x,
        CSW_F y,
        CSW_F r1,
        CSW_F r2,
        CSW_F ang1,
        CSW_F ang2,
        int closure,
        int red, int green, int blue,
        CSW_F angle);
    
    int gtx_clipcircle(
        CSW_F x,
        CSW_F y,
        CSW_F r1,
        CSW_F r2,
        CSW_F ang1,
        CSW_F ang2,
        int closure,
        int red, int green, int blue,
        CSW_F thick,
        CSW_F angle);
    
    int gtx_getwidgethw(
        int widg,
        int * wide,
        int * hgt);
    
    int gtx_get_window_hw(
        int wid,
        int * width,
        int * height);
    
    int gtx_getwidgetxy(
        int widg,
        int * x,
        int * y);
    
    int gtx_SetConstantScaleHomeFlag(
        int val);
    
    int gtx_SetRbandDrawFlag(
        int flag);
    
    int gtx_getgcontext(
        void);
    
    int gtx_drawpointprim(
        CSW_F x,
        CSW_F y,
        int red, int green, int blue);
    
    int gtx_clippointprim(
        CSW_F x,
        CSW_F y,
        int red, int green, int blue);
    
    int gtx_clipboxprim(
        CSW_F x1,
        CSW_F y1,
        CSW_F x2,
        CSW_F y2,
        CSW_F thick,
        int red, int green, int blue);
    
    int gtx_SetFillPattGC(
        int patnum);
    
    int gtx_BuildInitialPixmapsForFills(
        void);
    
    int gtx_SetFillPattGC32(
        int istipple);
    
    int gtx_SetFillPattGC64(
        int istipple);
    
    int gtx_cliprectprim(
        CSW_F xc,
        CSW_F yc,
        CSW_F wide,
        CSW_F high,
        CSW_F angd,
        CSW_F crad,
        int fred, int fgreen, int fblue,
        int pred, int pgreen, int pblue,
        int bred, int bgreen, int bblue,
        CSW_F thick,
        int pattern,
        CSW_F patscale,
        int linepatt,
        CSW_F dashscale);
    
    int gtx_cliparcprim(
        CSW_F xc,
        CSW_F yc,
        CSW_F r1,
        CSW_F r2,
        CSW_F ang1,
        CSW_F anglen,
        CSW_F rang,
        int fillflag,
        int fred, int fgreen, int fblue,
        int pred, int pgreen, int pblue,
        int bred, int bgreen, int bblue,
        CSW_F thick,
        int pattern,
        CSW_F patscale,
        int linepatt,
        CSW_F dashscale);
    
    int gtx_clip_image_prim (
        CSW_F xmin,
        CSW_F ymin,
        CSW_F xmax,
        CSW_F ymax,
        int   ncol,
        int   nrow,
        unsigned char *red_data,
        unsigned char *green_data,
        unsigned char *blue_data,
        unsigned char *trans_data);
    
    void gtx_set_selected_flag_for_drawing (int ival);
    
    int gtx_clip_frame_border (int frame_num, FRameStruct *frptr);
    
    int gtx_draw_frame_border (
        int      frame_num,
        CSW_F    px1,
        CSW_F    py1,
        CSW_F    px2,
        CSW_F    py2,
        int      borderflag,
        int      clipflag);
    
    void gtx_reset_exact_text_length (void);
    
}; // end of main class


    
#endif
/*
    end of header file
    add nothing below this endif
*/
