
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

  The functions in this file are called to send very primitive graphics
  elements back to the Java code for drawing.  These are all called from 
  the DisplayList class, via the old EasyX drawing functions.  EasyX takes
  care of creating the clipped primitives to be drawn, and Java will then
  draw them.

  These are very thin wraps around calls to jni functions.  I have put this
  level in to allow for possible debug running of the native side only.  In
  that case, I can replace the few jni functions with something suitable for
  debugging and run everything on the native side from a native main program.

 ***************************************************************************
*/

/*
 * The native side of the graphics system may need to be 
 * debugged via a native main program.  If this is the case
 * there will be no java side to send back to.  In the debug
 * mode, the functions in this file will write their parameters
 * out to a file named "native_ezx.dbg", on the current default
 * directory.
 */
#define DEBUG_NATIVE_ONLY     0

#include "csw/utils/include/csw_.h"

#include "csw/jeasyx/private_include/DisplayListJNI.h"
#include "csw/jeasyx/private_include/EZXJavaArea.h"
#include "csw/jeasyx/private_include/DLContour.h"


void EZXJavaArea::ezx_SetImageIDInJavaArea (int id)
{
    ImageID = id;
}

void EZXJavaArea::ezx_SetAlphaValue (int ival)
{
    AlphaValue = ival;
}

void EZXJavaArea::ezx_SetScreenDPI (int ival)
{
    ScreenDPI = ival;
}

int EZXJavaArea::ezx_SetDrawingPriority (int ival)
{
    DrawingPriority = ival;
    return 1;
}

int EZXJavaArea::ezx_SetFrameInJavaArea (int frame_num)
{
    CurrentFrame = frame_num;
    return 1;
}


void EZXJavaArea::SetZoomPan (char *fname,
                              double oldx1,
                              double oldy1,
                              double oldx2,
                              double oldy2,
                              double fx1,
                              double fy1,
                              double fx2,
                              double fy2)
{
    jni_call_set_zoom_pan_data_method (
        v_jenv,
        v_jobj,
        fname,
        oldx1,
        oldy1,
        oldx2,
        oldy2,
        fx1,
        fy1,
        fx2,
        fy2);
}
  


int EZXJavaArea::ezx_AppendFillToJavaArea (
    CSW_F          *xy,
    CSW_F          thickness,
    CSW_F          patscale,
    int            npts,
    int            red,
    int            green,
    int            blue,
    int            pattern,
    int            selectable
) {
    int            sflag;

    if (pattern < 0  ||  pattern > 9999) pattern = 0;

    sflag = DrawingPriority;

/*
 * A positive thickness means this is a border.
 */
    if (DrawingPriority == 0  &&  thickness > 0.0) {
        sflag = 9;
    }
    pattern += sflag * 10000;

#if DEBUG_NATIVE_ONLY
    if (DebugFile == NULL) {
        DebugFile = fopen ("C:\\glenn\\src\\native_ezx.dbg", "wb");
    }
    if (DebugFile == NULL) return 1;
    sprintf (Lbuff, 
        "\nFill Call\n"
        "thickness = %f  npts = %d  red = %d  green = %d  blue = %d\n"
        "pattern = %d  selectable = %d\n",
        thickness, npts, red, green, blue, pattern, selectable);
    fputs (Lbuff, DebugFile);
    for (int i=0; i<npts; i++) {
        sprintf (Lbuff, "%2f  %.2f\n", xy[2*i], xy[2*i+1]);
        fputs (Lbuff, DebugFile);
    }
#endif

    float  *fxy;
    int    freeflag = 0;

    freeflag = ConvertToFloat (xy, npts, &fxy);

    jni_call_add_fill_method (
        v_jenv,
        v_jobj,
        fxy,
        thickness * ScreenDPI,
        patscale,
        npts,
        red,
        green,
        blue,
        AlphaValue,
        pattern,
        CurrentFrame,
        selectable
    );

    if  (freeflag) {
        csw_Free (fxy);
        fxy = NULL;
    }

    return 1;

}



int EZXJavaArea::ezx_AppendLineToJavaArea (
    CSW_F          *xy,
    int            npts,
    int            red,
    int            green,
    int            blue,
    int            pattern,
    CSW_F          thickness,
    int            image_id,
    int            selectable
) {
    if (pattern < 0  ||  pattern > 9999) pattern = 0;
#if DEBUG_NATIVE_ONLY
    pattern += DrawingPriority * 10000;
    if (DebugFile == NULL) {
        DebugFile = fopen ("C:\\glenn\\src\\native_ezx.dbg", "wb");
    }
    if (DebugFile == NULL) return 1;
    sprintf (Lbuff, 
        "\nLine Call\n"
        "npts = %d  red = %d  green = %d  blue = %d\n"
        "pattern = %d  thickness = %.3f  selectable = %d\n",
        npts, red, green, blue, pattern, thickness, selectable);
    fputs (Lbuff, DebugFile);
    for (int i=0; i<npts; i++) {
        sprintf (Lbuff, "%.2f  %.2f\n", xy[2*i], xy[2*i+1]);
        fputs (Lbuff, DebugFile);
    }
#endif

    pattern += DrawingPriority * 10000;

    float  *fxy = NULL;
    int    freeflag = 0;

    freeflag = ConvertToFloat (xy, npts, &fxy);

    jni_call_add_line_method (
        v_jenv,
        v_jobj,
        fxy,
        npts,
        red,
        green,
        blue,
        AlphaValue,
        pattern,
        thickness * ScreenDPI,
        CurrentFrame,
        image_id,
        selectable
    );

    if (freeflag) {
        csw_Free (fxy);
    }

    return 1;

}



int EZXJavaArea::ezx_AppendTextToJavaArea (
    CSW_F          x,
    CSW_F          y,
    char           *text,
    int            red,
    int            green,
    int            blue,
    CSW_F          angle,
    CSW_F          size,
    int            font,
    int            image_id,
    int            selectable)
{

#if DEBUG_NATIVE_ONLY
    if (DebugFile == NULL) {
        DebugFile = fopen ("C:\\glenn\\src\\native_ezx.dbg", "wb");
    }
    if (DebugFile == NULL) return 1;
    sprintf (Lbuff, 
        "\nText Call\n"
        "x = %.2f  y = %.2f\n"
        "text = %s\n"
        "red = %d  green = %d  blue = %d\n"
        "angle = %.2f  size = %.2f  font = %d  selectable = %d\n",
        x, y, text, red, green, blue, angle, size, font, selectable);
    fputs (Lbuff, DebugFile);
#endif

    jni_call_add_text_method (
        v_jenv,
        v_jobj,
        (float)x,
        (float)y,
        text,
        red,
        green,
        blue,
        AlphaValue,
        angle,
        size,
        font,
        CurrentFrame,
        image_id,
        selectable
    );

    return 1;

}



int EZXJavaArea::ezx_AppendArcToJavaArea (
    CSW_F          x,
    CSW_F          y,
    CSW_F          r1,
    CSW_F          r2,
    CSW_F          ang1,
    CSW_F          ang2,
    int            closure,
    int            red,
    int            green,
    int            blue,
    CSW_F          thickness,
    CSW_F          angle,
    int            selectable
) {
    if (closure < 0  ||  closure > 9999) closure = 0;
#if DEBUG_NATIVE_ONLY
    closure += DrawingPriority * 10000;
    if (DebugFile == NULL) {
        DebugFile = fopen ("C:\\glenn\\src\\native_ezx.dbg", "wb");
    }
    if (DebugFile == NULL) return 1;
    sprintf (Lbuff, 
        "\nArc Call\n"
        "x = %.2f  y = %.2f\n"
        "r1 = %.2f  r2 = %.2f\n"
        "ang1 = %.2f ang2 = %.2f closure = %d\n"
        "red = %d  green = %d  blue = %d\n"
        "thickness = %.2f  angle = %.2f  selectable = %d\n",
        x, y, r1, r2, ang1, ang2, closure, red, green, blue,
        thickness, angle, selectable);
    fputs (Lbuff, DebugFile);
#endif

    closure += DrawingPriority * 10000;

    jni_call_add_arc_method (
        v_jenv,
        v_jobj,
        (float)x,
        (float)y,
        r1,
        r2,
        ang1,
        ang2,
        closure,
        red,
        green,
        blue,
        AlphaValue,
        thickness * ScreenDPI,
        angle,
        CurrentFrame,
        selectable
    );

    return 1;

}


int EZXJavaArea::ezx_AppendFilledArcToJavaArea (
    CSW_F          x,
    CSW_F          y,
    CSW_F          r1,
    CSW_F          r2,
    CSW_F          ang1,
    CSW_F          ang2,
    int            closure,
    int            red,
    int            green,
    int            blue,
    CSW_F          thickness,
    CSW_F          angle,
    int            pattern,
    int            selectable
) {
    int sflag;

    if (closure < 0  ||  closure > 9999) closure = 0;
    sflag = DrawingPriority;

/*
 * A positive thickness means this is a border.
 */
    if (DrawingPriority == 0  &&  thickness > 0.0) {
        sflag = 9;
    }
    closure += sflag * 10000;

#if DEBUG_NATIVE_ONLY
    if (DebugFile == NULL) {
        DebugFile = fopen ("C:\\glenn\\src\\native_ezx.dbg", "wb");
    }
    if (DebugFile == NULL) return 1;
    sprintf (Lbuff, 
        "\nFilled Arc Call\n"
        "x = %.2f  y = %.2f\n"
        "r1 = %.2f  r2 = %.2f\n"
        "ang1 = %.2f ang2 = %.2f closure = %d\n"
        "red = %d  green = %d  blue = %d\n"
        "angle = %.2f  pattern = %d selectable = %d\n",
        x, y, r1, r2, ang1, ang2, closure, red, green, blue,
        angle, pattern, selectable);
    fputs (Lbuff, DebugFile);
#endif

    jni_call_add_filled_arc_method (
        v_jenv,
        v_jobj,
        (float)x,
        (float)y,
        r1,
        r2,
        ang1,
        ang2,
        closure,
        red,
        green,
        blue,
        AlphaValue,
        thickness * ScreenDPI,
        angle,
        pattern,
        CurrentFrame,
        selectable
    );

    return 1;

}


int EZXJavaArea::ezx_AppendImageToJavaArea (
    CSW_F           x1,
    CSW_F           y1,
    CSW_F           x2,
    CSW_F           y2,
    int             ncol,
    int             nrow,
    unsigned char   *red,
    unsigned char   *green,
    unsigned char   *blue,
    unsigned char   *transparency,
    int             has_lines,
    int             image_id,
    int             selectable
) {
#if DEBUG_NATIVE_ONLY
    red = red;
    green = green;
    blue = blue;
    transparency = transparency;
    if (DebugFile == NULL) {
        DebugFile = fopen ("C:\\glenn\\src\\native_ezx.dbg", "wb");
    }
    if (DebugFile == NULL) return 1;
    sprintf (Lbuff, 
        "\nImage Call\n"
        "x1 = %.2f  y1 = %.2f\n"
        "x2 = %.2f  y2 = %.2f\n"
        "ncol = %d  nrow = %d  selectable = %d\n",
        x1, y1, x2, y2, ncol, nrow, selectable);
    fputs (Lbuff, DebugFile);
#endif

    jni_call_add_image_method (
        v_jenv,
        v_jobj,
        (float)x1,
        (float)y1,
        (float)x2,
        (float)y2,
        ncol,
        nrow,
        red,
        green,
        blue,
        transparency,
        CurrentFrame,
        has_lines,
        image_id,
        selectable
    );

    return 1;

}


int EZXJavaArea::ezx_AppendFrameToJavaArea (
    CSW_F          x1,
    CSW_F          y1,
    CSW_F          x2,
    CSW_F          y2,
    double         fx1,
    double         fy1,
    double         fx2,
    double         fy2,
    int            borderflag,
    int            clipflag,
    int            scaleable,
    int            scale_to_attach_frame,
    int            frame_num,
    char           *fname
) {

#if DEBUG_NATIVE_ONLY
    if (DebugFile == NULL) {
        DebugFile = fopen ("C:\\glenn\\src\\native_ezx.dbg", "wb");
    }
    if (DebugFile == NULL) return 1;
    sprintf (Lbuff, 
        "\nFrame Call\n"
        "x1 = %f  y1 = %f  x2 = %f  y2 = %f\n"
        "fx1 = %f  fy1 = %f  fx2 = %f  fy2 = %f\n"
        "borderflag = %d  clipflag = %d  frame_num = %d\n"
        x1, y1, x2, y2, fx1, fy1, fx2, fy2,
        borderflag, clipflag, frame_num);
    fputs (Lbuff, DebugFile);
#endif

    jni_call_add_frame_method (
        v_jenv,
        v_jobj,
        (float)x1, (float)y1, (float)x2, (float)y2,
        fx1, fy1, fx2, fy2,
        borderflag,
        clipflag,
        scaleable,
        scale_to_attach_frame,
        frame_num,
        fname
    );

    return 1;

}


int EZXJavaArea::ezx_AppendSelectedFillToJavaArea (
    FIllPrim       *fptr,
    char           *frame_name,
    char           *layer_name,
    char           *item_name
) 
{
    double         *xp, *yp;
    int            i, nptot;

    if (fptr == NULL) {
        return 0;
    }

    if (fptr->x_orig == NULL  ||
        fptr->y_orig == NULL  ||
        fptr->npts_orig == NULL) {
        return 0;
    }

    if (fptr->selectable_object_num < 0) {
        return 0;
    }

    nptot = 0;
    for (i=0; i<fptr->ncomp_orig; i++) {
        nptot += fptr->npts_orig[i];
    }
    xp = (double *)csw_Malloc (2 * nptot * sizeof(double));
    if (xp == NULL) {
        return -1;
    }
    yp = xp + nptot;

    jni_call_add_selected_fill (
        v_jenv,
        v_jobj,
        fptr->selectable_object_num, 
        xp,
        yp,
        fptr->npts_orig,
        fptr->ncomp_orig,
        fptr->thick,
        fptr->patscale,
        fptr->dashscale,
        fptr->fill_red,
        fptr->fill_green,
        fptr->fill_blue,
        fptr->fill_alpha,
        fptr->pat_red,
        fptr->pat_green,
        fptr->pat_blue,
        fptr->pat_alpha,
        fptr->border_red,
        fptr->border_green,
        fptr->border_blue,
        fptr->border_alpha,
        frame_name,
        layer_name,
        item_name,
        fptr->pattern,
        fptr->linepatt,
        fptr->prim_num
    );

    csw_Free (xp);

    return 1;

}


int EZXJavaArea::ezx_AppendSelectedLineToJavaArea (
    LInePrim       *lptr,
    char           *frame_name,
    char           *layer_name,
    char           *item_name
) 
{
    CSW_F          *xt;
    double         *xp, *yp;
    int            i;

    if (lptr == NULL) {
        return 0;
    }

    if (lptr->selectable_object_num < 0) {
        return 0;
    }

    xp = (double *)csw_Malloc (2 * lptr->npts * sizeof(double));
    if (xp == NULL) {
        return -1;
    }
    yp = xp + lptr->npts;

    xt = lptr->xypts;
    for (i=0; i<lptr->npts; i++) {
        xp[i] = *xt;
        xt++;
        yp[i] = *xt;
        xt++;
    }

    jni_call_add_selected_line (
        v_jenv,
        v_jobj,
        lptr->selectable_object_num, 
        xp,
        yp,
        lptr->npts,
        lptr->thick,
        lptr->dashscale,
        lptr->red,
        lptr->green,
        lptr->blue,
        lptr->alpha,
        frame_name,
        layer_name,
        item_name,
        lptr->dashpat,
        lptr->symbol,
        lptr->arrowstyle,
        lptr->prim_num
    );

    csw_Free (xp);

    return 1;

}


int EZXJavaArea::ezx_AppendSelectedContourToJavaArea (
    void           *vp1,
    char           *frame_name,
    char           *layer_name,
    char           *item_name,
    char           *surface_name,
    void           *vp2
) 
{
    DLContour      *cptr = static_cast <DLContour *> (vp1);
    DLContourProperties *dlprop = static_cast <DLContourProperties *> (vp2);

    double         *xp, *yp;
    double         thick, lsize, lspace, tlen, tspace;
    int            major, font, tdir, red, green, blue, alpha;
    CSW_F          *fxp, *fyp;
    int            i, nptot;
    COntourOutputRec  *crec;

    if (cptr == NULL) {
        return 0;
    }

    if (cptr->selectable_object_num < 0) {
        return 0;
    }

    cptr->GetContourPoints (&fxp, &fyp, &nptot);

    xp = (double *)csw_Malloc (2 * nptot * sizeof(double));
    if (xp == NULL) {
        return -1;
    }
    yp = xp + nptot;

    for (i=0; i<nptot; i++) {
        xp[i] = fxp[i];
        yp[i] = fyp[i];
    }

    crec = cptr->GetCrec ();

    if (crec->major != 0) {
        major = 1;
        thick = dlprop->majorThickness;
        red = dlprop->redMajor;
        green = dlprop->greenMajor;
        blue = dlprop->blueMajor;
        alpha = dlprop->alphaMajor;
        if (dlprop->labelMajor == 0) {
            font = -1;
            lsize = 0.0;
            lspace = 0.0;
        }
        else {
            font = dlprop->contourFont;
            lsize = dlprop->majorLabelSize;
            lspace = dlprop->majorLabelSpacing;
        }
        if (dlprop->tickMajor == 0) {
            tlen = 0.0;
            tspace = 0.0;
            tdir = 0;
        }
        else {
            tlen = dlprop->majorTickLength;
            tspace = dlprop->majorTickSpacing;
            tdir = dlprop->tickMajor;
        }
    }
    else {
        major = 0;
        thick = dlprop->minorThickness;
        red = dlprop->redMinor;
        green = dlprop->greenMinor;
        blue = dlprop->blueMinor;
        alpha = dlprop->alphaMinor;
        if (dlprop->labelMinor == 0) {
            font = -1;
            lsize = 0.0;
            lspace = 0.0;
        }
        else {
            font = dlprop->contourFont;
            lsize = dlprop->minorLabelSize;
            lspace = dlprop->minorLabelSpacing;
        }
        if (dlprop->tickMinor == 0) {
            tlen = 0.0;
            tspace = 0.0;
            tdir = 0;
        }
        else {
            tlen = dlprop->minorTickLength;
            tspace = dlprop->minorTickSpacing;
            tdir = dlprop->tickMinor;
        }
    }

    jni_call_add_selected_contour (
        v_jenv,
        v_jobj,
        cptr->selectable_object_num, 
        xp,
        yp,
        nptot,
        crec->zvalue,
        thick,
        major,
        red,
        green,
        blue,
        alpha,
        crec->text,
        font,
        lsize,
        lspace,
        tlen,
        tspace,
        tdir,
        frame_name,
        layer_name,
        item_name,
        surface_name,
        cptr->prim_num
    );

    csw_Free (xp);

    return 1;

}


int EZXJavaArea::ezx_AppendSelectedRectangleToJavaArea (
    SHapePrim      *rptr,
    char           *frame_name,
    char           *layer_name,
    char           *item_name
) 
{
    if (rptr == NULL) {
        return 0;
    }

    if (rptr->selectable_object_num < 0) {
        return 0;
    }

    jni_call_add_selected_rectangle (
        v_jenv,
        v_jobj,
        rptr->selectable_object_num, 
        rptr->fval[0],
        rptr->fval[1],
        rptr->fval[2],
        rptr->fval[3],
        rptr->fval[4],
        rptr->fval[5],
        rptr->thick,
        rptr->patscale,
        rptr->dashscale,
        rptr->fill_red,
        rptr->fill_green,
        rptr->fill_blue,
        rptr->fill_alpha,
        rptr->pat_red,
        rptr->pat_green,
        rptr->pat_blue,
        rptr->pat_alpha,
        rptr->border_red,
        rptr->border_green,
        rptr->border_blue,
        rptr->border_alpha,
        frame_name,
        layer_name,
        item_name,
        rptr->pattern,
        rptr->linepatt,
        rptr->prim_num
    );

    return 1;

}


int EZXJavaArea::ezx_AppendSelectedArcToJavaArea (
    SHapePrim      *aptr,
    char           *frame_name,
    char           *layer_name,
    char           *item_name
) 
{
    int            closure;
    if (aptr == NULL) {
        return 0;
    }

    if (aptr->selectable_object_num < 0) {
        return 0;
    }

    closure = (int)(aptr->fval[7] + 0.01);

    jni_call_add_selected_arc (
        v_jenv,
        v_jobj,
        aptr->selectable_object_num, 
        aptr->fval[0],
        aptr->fval[1],
        aptr->fval[2],
        aptr->fval[3],
        aptr->fval[4],
        aptr->fval[5],
        aptr->fval[6],
        closure,
        aptr->thick,
        aptr->patscale,
        aptr->dashscale,
        aptr->fill_red,
        aptr->fill_green,
        aptr->fill_blue,
        aptr->fill_alpha,
        aptr->pat_red,
        aptr->pat_green,
        aptr->pat_blue,
        aptr->pat_alpha,
        aptr->border_red,
        aptr->border_green,
        aptr->border_blue,
        aptr->border_alpha,
        frame_name,
        layer_name,
        item_name,
        aptr->pattern,
        aptr->linepatt,
        aptr->prim_num
    );

    return 1;

}


int EZXJavaArea::ezx_AppendSelectedTextToJavaArea (
    TExtPrim       *tptr,
    char           *frame_name,
    char           *layer_name,
    char           *item_name
) 
{
    if (tptr == NULL) {
        return 0;
    }

    if (tptr->selectable_object_num < 0) {
        return 0;
    }

    jni_call_add_selected_text (
        v_jenv,
        v_jobj,
        tptr->selectable_object_num, 
        tptr->x,
        tptr->y,
        tptr->anchor,
        tptr->thick,
        tptr->bg_thick,
        tptr->angle,
        tptr->size,
        tptr->xoff,
        tptr->yoff,
        tptr->border_red,
        tptr->border_green,
        tptr->border_blue,
        tptr->border_alpha,
        tptr->fill_red,
        tptr->fill_green,
        tptr->fill_blue,
        tptr->fill_alpha,
        tptr->bg_red,
        tptr->bg_green,
        tptr->bg_blue,
        tptr->bg_alpha,
        tptr->bg_border_red,
        tptr->bg_border_green,
        tptr->bg_border_blue,
        tptr->bg_border_alpha,
        tptr->bgflag,
        tptr->font_num,
        tptr->chardata,
        frame_name,
        layer_name,
        item_name,
        tptr->prim_num
    );

    return 1;

}


int EZXJavaArea::ezx_AppendSelectedSymbToJavaArea (
    SYmbPrim       *sptr,
    char           *frame_name,
    char           *layer_name,
    char           *item_name,
    char           *surface_name
) 
{
    if (sptr == NULL) {
        return 0;
    }

    if (sptr->selectable_object_num < 0) {
        return 0;
    }

    jni_call_add_selected_symb (
        v_jenv,
        v_jobj,
        sptr->selectable_object_num, 
        sptr->x,
        sptr->y,
        sptr->symb_num,
        sptr->thick,
        sptr->angle,
        sptr->size,
        sptr->red,
        sptr->green,
        sptr->blue,
        sptr->alpha,
        frame_name,
        layer_name,
        item_name,
        surface_name,
        sptr->prim_num
    );

    return 1;

}

int EZXJavaArea::ezx_AppendSelectedAxisToJavaArea (
    AXisPrim       *aptr,
    char           *frame_name,
    char           *layer_name,
    char           *item_name
) 
{
    FRameAxisStruct   *ap;

    if (aptr == NULL) {
        return 0;
    }

    if (aptr->selectable_object_num < 0) {
        return 0;
    }

    ap = aptr->ap;

    jni_call_add_selected_axis (
        v_jenv,
        v_jobj,
        ap->label_flag,
        ap->tick_flag,
        ap->tick_direction,
        ap->caption,
        ap->major_interval,
        ap->line_red,
        ap->line_green,
        ap->line_blue,
        ap->text_red,
        ap->text_green,
        ap->text_blue,
        ap->line_thickness,
        ap->text_size,
        ap->text_thickness,
        ap->text_font,
        aptr->fx1,
        aptr->fy1,
        aptr->fx2,
        aptr->fy2,
        aptr->label_dir,
        aptr->selectable_object_num, 
        aptr->afirst,
        aptr->alast,
        frame_name,
        layer_name,
        item_name,
        aptr->prim_num
    );

    return 1;

}



int EZXJavaArea::ConvertToFloat (CSW_F *xy, int npts, float **fxy)
{
    float   *fw = NULL;
    *fxy = NULL;
    int freeflag = 0;
    if (npts < MAX_FLOAT_CONVERT) {
        fw = _p_fxy;
    }
    else {
        fw = (float *)csw_Malloc (2 * npts * sizeof(float));
        if (fw == NULL) {
            return 0;
        }
        freeflag = 1;
    }

    for (int i = 0; i < 2 * npts; i++) {
        fw[i] = (float) xy[i];
    }

    *fxy = fw;

    return freeflag;
}

