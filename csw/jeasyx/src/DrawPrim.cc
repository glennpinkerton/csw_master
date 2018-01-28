
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    DrawPrim.cc

    This file implements the methods of the GTXDrawPrim class. 
    No drawing is actually done here.  These methods basically
    calculate graphic primitives (lines, fills, text, etc) in
    device units and then call java jni stuff to send these
    primitives back to java for actual drawing.
*/

#include <math.h>
#include <string.h>
#include <ctype.h>

#include "csw/utils/include/csw_.h"

#include "csw/jeasyx/private_include/gtx_graphP.h"
#include "csw/jeasyx/private_include/gtx_wrksizP.h"

#include "csw/utils/private_include/gpf_font.h"
#include "csw/utils/private_include/gpf_symb.h"
#include "csw/utils/private_include/gpf_ldash.h"
#include "csw/utils/private_include/gpf_line.h"
#include "csw/utils/private_include/gpf_shape.h"
#include "csw/utils/private_include/gpf_utils.h" 
#include "csw/utils/private_include/gpf_color.h" 
#include "csw/utils/private_include/gpf_spline.h" 
#include "csw/utils/private_include/ply_protoP.h"
#include "csw/utils/private_include/gpf_calcdraw.h"
#include "csw/utils/private_include/TextBounds.h"

#include "csw/jeasyx/private_include/EZXJavaArea.h"
#include "csw/jeasyx/private_include/gtx_drawprim.h"


void GTXDrawPrim::gtx_SetImageIDForDraw (int id)
{
    ImageID = id;
}

void GTXDrawPrim::gtx_SetImageHasLinesForDraw (int val)
{
    ImageHasLines = val;
}





/*
******************************************************************

              g t x _ I n i t D r a w P r i m

******************************************************************

  allocate memory for drawprim work space
  returns 1 on success or -1 if csw_Malloc failed

*/

int GTXDrawPrim::gtx_InitDrawPrim (void)
{

    int              size, *ibuf;
    CSW_F            *fbuf;

    if (initdone) {
        return 1;
    }

    XYStatic = (CSW_F *)csw_Malloc (2 * XYMAX * sizeof(CSW_F));
    if (!XYStatic) {
        return -1;
    }

    size = XYMAX2 * 3 + MAXFONT * 2 + MAXFONT2 + 
           MAXWORK * 2 + MAXLINE * 4 + MAXSMOOTH * 2 + 1;
    fbuf = (CSW_F *)csw_Malloc (size * sizeof(CSW_F));
    if (!fbuf) {
        csw_Free (XYStatic);
        XYStatic = NULL;
        return -1;
    }

    size = 3 * IWORKSIZE + MAXWORK + 1;
    ibuf = (int *)csw_Malloc (size * sizeof(int));
    if (!ibuf) {
        csw_Free (XYStatic);
        csw_Free (fbuf);
        XYStatic = NULL;
        return -1;
    }

    XYworkbuf = fbuf;
    XYwork2 = fbuf + XYMAX2;
    XYarc = XYwork2 + XYMAX2;
    Fontx = XYarc + XYMAX2;
    Fonty = Fontx + MAXFONT;
    Fontxy = Fonty + MAXFONT;
    XYclip = Fontxy + MAXFONT2;
    Xthin = XYclip + MAXWORK * 2;
    Ythin = Xthin + MAXLINE;
    XYthin = Ythin + MAXLINE;
    XYsmooth = XYthin + MAXLINE * 2;
    
    Iwork1 = ibuf;
    Iwork2 = ibuf + IWORKSIZE;
    Iwork3 = Iwork2 + IWORKSIZE;
    Iclip = Iwork3 + IWORKSIZE;

/*
    initialize line dash patterns
*/
    gpf_ldash_obj.gpf_initserverdashdata ();

    DashFlag = 0;

    initdone = 1;

    return 1;

}  /*  end of function gtx_InitDrawPrim  */




/*
  ****************************************************************

               g t x _ F r e e D r a w P r i m

  ****************************************************************

    Free the space allocated by gtx_InitDrawPrim.

*/

int GTXDrawPrim::gtx_FreeDrawPrim (void)
{

    csw_Free (XYStatic);
    XYStatic = NULL;
    csw_Free (XYworkbuf);
    XYworkbuf = NULL;
    csw_Free (Iwork1);
    Iwork1 = NULL;
    
    XYwork2 = NULL;
    XYarc = NULL;
    XYsmooth = NULL;
    Fontx = NULL;
    Fonty = NULL;
    Fontxy = NULL;
    Xthin = NULL;
    Ythin = NULL;
    XYthin = NULL;
    XYclip = NULL;
    Iwork2 = NULL;
    Iwork3 = NULL;
    Iclip = NULL;

    return 1;

}  /*  end of function gtx_FreeDrawPrim  */






/*
******************************************************************

                         g t x _ s c a l e f

******************************************************************

  function name:    gtx_scalef   (integer)

  call sequence:    gtx_scalef (x, y, fx, fy)

  purpose:          scale page units coordinates to window coordinates

  return value:     always returns zero

  calling parameters:

    x          r    CSW_F     x coordinate in plot page units
    y          r    CSW_F     y coordinate in plot page units
    fx         w    CSW_F*    window coordinate
    fy         w    CSW_F*    window coordinate

*/

int GTXDrawPrim::gtx_scalef (CSW_F x, CSW_F y, CSW_F *fx, CSW_F *fy)
{
       
    *fx = (x - OrigXmin) * scalx + ixmin;
    *fy = iymax - ((y - OrigYmin) * scaly + iymin);

    return 0;

}  /*  end of function gtx_scalef  */



int GTXDrawPrim::gtx_scale_size (CSW_F *size)
{
    CSW_F      w, h, mult;

    if (*size < 0.0001) {
        return 1;
    }

    w = *size * scalx;
    h = *size * scaly;
    w = (w + h) / 2.0f;
    w /= ScreenUnitsPerInch;
    if (w < 0.0001) {
        w = 0.0001f;
    }

    mult = *size / w;
    *size = *size * mult;

    return 1;

}

    



/*
******************************************************************

               g t x _ s c a l e _ a r r a y f

******************************************************************

  function name:    gtx_scale_arrayf   (integer)

  call sequence:    gtx_scale_arrayf (fxy, fxyout, npt)

  purpose:          scale plotter units coordinates to window coordinates

  return value:     always returns zero

  calling parameters:

    fxy        r    CSW_F*    array of x,y x,y coordinates in plotter or world units
    fxyout     w    CSW_F*    array of window coordinates for fxy
    npt        r    int       number of points in fxy

*/

int GTXDrawPrim::gtx_scale_arrayf (CSW_F *fxy, CSW_F *fxyout, int npt)
{
    int        i;
    CSW_F      xt, yt;

    for (i=0; i<npt; i++) {
       
        xt = (*fxy - OrigXmin) * scalx + ixmin; 
        fxy++;
        yt = iymax - ((*fxy - OrigYmin) * scaly + iymin);
        fxy++;

        *fxyout = xt;
        fxyout++;
        *fxyout = yt;
        fxyout++;

    }

    return 0;

}  /*  end of function gtx_scale  */




/*
******************************************************************

                g t x _ s c a l e _a r r a y _ 2 f

******************************************************************

  function name:    gtx_scale_array_2f   (integer)

  call sequence:    gtx_scalef (x, y, npts, fx, fy)

  purpose:          scale plotter units coordinates to window coordinates

  return value:     always returns zero

  calling parameters:

    x          r    CSW_F*    x coordinates in plot page units
    y          r    CSW_F*    y coordinates in plot page units
    npts       r    int       number of points
    fx         w    CSW_F*    window coordinates
    fy         w    CSW_F*    window coordinates

*/

int GTXDrawPrim::gtx_scale_array_2f (CSW_F *sx, CSW_F *sy, int npts, CSW_F *fx, CSW_F *fy)
{
    int       i;

    for (i=0; i<npts; i++) {
        fx[i] = (sx[i] - OrigXmin) * scalx + ixmin;
        fy[i] = iymax - ((sy[i] - OrigYmin) * scaly + iymin);
    }

    return 0;

}  /*  end of function gtx_scale_array_2f  */




/*
******************************************************************

                 g t x _ b a c k s c a l e f

******************************************************************

  function name:    gtx_backscale2   (integer)

  call sequence:    gtx_backscale2 (sx, sy, x, y)

  purpose:          calculate plot coordinates from screen coordinates

  return value:     always returns zero

  calling parameters:

    sx     r    CSW_F*      screen x coordinate
    sy     r    CSW_F*      screen y coordinate
    x      w    CSW_F*      calculated plot page x coordinate
    y      w    CSW_F*      calculated plot page y coordinate

*/

int GTXDrawPrim::gtx_backscalef (CSW_F sx, CSW_F sy, CSW_F *x, CSW_F *y)
{

    *x = (sx-ixmin) / scalx + OrigXmin;
    *y = (iymax-iymin-sy) / scaly + OrigYmin;

    return 0;

}  /*  end of function gtx_backscalef  */






/*
******************************************************************

           g t x _ b a c k s c a l e _ a r r a y f

******************************************************************

  function name:    gtx_backscale_arrayf    (integer)

  call sequence:    gtx_backscale_arrayf (sxy, xyout, npts)

  purpose:          calculate plot coordinates from screen coordinates

  return value:     always returns zero

  calling parameters:

    sxy    r    CSW_F*      array of screen x coordinates
    xyout  w    CSW_F*      array of calculated plot x coordinates
    npts   r    int         number of points

*/

int GTXDrawPrim::gtx_backscale_arrayf (CSW_F *sxy, CSW_F *xyout, int npts)
{
    int           i;
    CSW_F         *st, *xt, sx, sy;

    st = sxy;
    xt = xyout;
    for (i=0; i<npts; i++) {
        sx = *st;
        st++;
        sy = *st;
        st++;
        *xt = (sx-ixmin) / scalx + OrigXmin;
        xt++;
        *xt = (iymax-iymin-sy) / scaly + OrigYmin;
        xt++;
    }

    return 0;

}  /*  end of function gtx_backscale_arrayf  */




/*
******************************************************************

           g t x _ b a c k s c a l e _ a r r a y _ 2 f

******************************************************************

  function name:    gtx_backscale_array_2f    (integer)

  call sequence:    gtx_backscale_array_2f (sx, sy, x, y)

  purpose:          calculate plot coordinates from screen coordinates

  return value:     always returns zero

  calling parameters:

    sx     r    CSW_F*      array of screen x coordinates
    sy     r    CSW_F*      array of screen y coordinates
    x      w    CSW_F*      array of calculated plot x coordinates
    y      w    CSW_F*      array of calculated plot y coordinates
    npts   r    int         number of points

*/

int GTXDrawPrim::gtx_backscale_array_2f (CSW_F *sx, CSW_F *sy, int npts, CSW_F *x, CSW_F *y)
{
    int           i;

    for (i=0; i<npts; i++) {
        x[i] = (sx[i]-ixmin) / scalx + OrigXmin;
        y[i] = (iymax-iymin-sy[i]) / scaly + OrigYmin;
    }

    return 0;

}  /*  end of function gtx_backscale_arrayf  */


/*
******************************************************************

                  g t x _ c l i p l i n e p r i m

******************************************************************

  function name:    gtx_cliplineprim     (int)

  call sequence:    gtx_cliplineprim  (xyorig, norig, smflag, thick, 
                                       red, green, blue,
                                       pattern, dashscale, arrowstyle)

  purpose:          clip a line primitive to the current window and
                    draw it.

  return value:     status code

                    -1 = memory allocation error
                    anything else, successful

  calling parameters:  

    xyorig    r    CSW_F*    array of packed x,y  x,y coordinates
    norig     r    int       number of points in the xyin array
    smflag    r    char      'y' for smooth, otherwise no smooth
    thick     r    CSW_F     line thickness in inches
    red       r    int       red value for line color
    green     r    int       green value for line color
    blue      r    int       blue value for line color
    pattern   r    int       line dash pattern number
    dashscale r    CSW_F     scale factor for dash pattern
    arrowstyle r   int       flag for drawing arrow at last point

*/

int GTXDrawPrim::gtx_cliplineprim (CSW_F *xyorig, int norig, char smflag,
                      CSW_F thick, int red, int green, int blue,
                      int pattern, CSW_F dashscale, int arrowstyle)

{
    int         ncomp, toobig, i, j, *icomp, npts,
                istat, nout, nin2, nin, nholes, ido;
    CSW_F       *xyw, xx1, yy1, xx2, yy2, x1, y1t, x2, y2,
                ax1, ay1, ax2, ay2, asize, fdum;
    CSW_F       **xyseg, *xyin, xarrow[10], *xyout;
    int         pattern_with_scale;

    if (pattern <= 0) {
        pattern = 0;
        dashscale = 1.0;
    }
    if (dashscale < 0.1) dashscale = 0.1f;
    if (dashscale > 9.99) dashscale = 9.99f;
    pattern_with_scale = pattern + (int)(dashscale * 1000.0f + .5f);

/*  check if there are too many points  */

    if (norig > MAXWORK) {
        norig = MAXWORK;
    }

/*  set up clip window  */

    xx1 = fxmin;
    yy1 = fymin;
    xx2 = fxmax;
    yy2 = fymax;

    xyout = NULL;

    /*
        Smooth the line if needed.
        Try the smoothing before clip to frame (2017 refactor)
    */
        if (smflag > 0) {
            istat = gpf_spline_obj.gpf_SplineFit2 (xyorig, norig, 0, 
                                    XYsmooth, MAXSMOOTH, 2.0f/scalx,
                                    &nout);
            if (istat == 1) {
                xyorig = XYsmooth;
                norig = nout;
            }
        }
        
/*  separate each frame clipped segment of the line  */

    xyseg = (CSW_F **)XYclip;
    fdum = 1.e18f;

    gpf_calcdraw_obj.gpf_polyholesep (xyorig, norig, fdum, MAXWORK,
                     xyseg, &nholes, Iclip);

/*  loop through each frame clipped segment  */

    xyin = NULL;
    nin2 = -1;
    for (ido=0; ido<nholes; ido++) {

        nin = Iclip[ido];
        xyin = xyseg[ido];

        if (nin * 2 < XYMAX) {
            xyw = XYworkbuf;
            icomp = Iwork1;
            toobig = 0;
        }
        else {
            xyw = (CSW_F *)csw_Malloc (nin * 2 * sizeof(CSW_F));
            if (!xyw) {
                if (xyout) csw_Free (xyout);
                return -1;
            }
            icomp = (int*)csw_Malloc (nin * 2 * sizeof(int));
            if (!icomp) {
                csw_Free (xyw);
                if (xyout) csw_Free (xyout);
                return -1;
            }
            toobig = 1;
        }

/*      thin if needed  */

        nin2 = nin;
        if (ThinFlag) {
            gpf_xyxythin1 (xyin, nin, ThinDist, ThinDist, 1,
                           XYthin, &nin2);
            xyin = XYthin;
        }

/*      separate each window clipped component  */ 

        gpf_calcdraw_obj.gpf_cliplineprim (xyin, nin2, xx1, yy1, xx2, yy2,
                          xyw, &ncomp, icomp);

        j = 0;
        for (i=0; i<ncomp; i++) {
            gtx_drawlineprim (xyw + j, icomp[i], thick,
                              red, green, blue, pattern_with_scale);

            j = j + 2 * icomp[i];
        }

        if (toobig) {
            csw_Free (xyw);
            csw_Free (icomp);
        }

/*  end of ido loop  */

    }

/*
    draw arrow tip if needed
*/
    if (arrowstyle != 0  &&  nin2 > 1  &&  xyin != NULL) {
        asize = thick * 5.0f;
        if (asize < 0.025) asize = 0.025f;
        nin2 *= 2;
        x1 = xyin[nin2-4];
        y1t = xyin[nin2-3];
        x2 = xyin[nin2-2];
        y2 = xyin[nin2-1];
        if (x2 < xx1  ||  x2 > xx2) {
            if (xyout) csw_Free (xyout);
            return 0;
        }
        if (y2 < yy1  ||  y2 > yy2) {
            if (xyout) csw_Free (xyout);
            return 0;
        }

        ax1 = x2 - asize;
        ax2 = x2 + asize;
        ay1 = y2 - asize;
        ay2 = y2 + asize;

        if (ax1 < xx1  ||  ax2 > xx2) {
            if (xyout) csw_Free (xyout);
            return 0;
        }
        if (ay1 < yy1  ||  ay2 > yy2) {
            if (xyout) csw_Free (xyout);
            return 0;
        }

        ArrowCalc (x1, y1t, x2, y2, asize,
                   arrowstyle, xarrow);
        npts = 3;
        if (arrowstyle > 1) npts = 4;

        if (arrowstyle == 1  ||  arrowstyle == 2  ||  arrowstyle == 4) {
            gtx_drawlineprim (xarrow, npts, thick, red, green, blue, 0);
        }
        else {
            gtx_drawfillprim (xarrow, npts, red, green, blue);
        }

    }

    if (xyout) csw_Free (xyout);

    return 0;

}  /*  end of function gtx_cliplineprim  */





/*
******************************************************************

                   g t x _ d r a w f i l l p r i m

******************************************************************

  function name:    gtx_drawfillprim    (integer)

  call sequence:    gtx_drawfillprim (coords, npt, red, green, blue)

  purpose:          draw a polyfill primitive to the window

  return value:     status code

                    0 = normal successful completion
                    -1 = could not allocate memory for polyfill

  calling parameters:

    coords       r     CSW_F*    array of x,y x,y coordinates in plotter inches
    npt          r     int       number of points in coords
    red          r     int       red value for fill color
    green        r     int       green value for fill color
    blue         r     int       blue value for fill color

*/

int GTXDrawPrim::gtx_drawfillprim (CSW_F *coords, int npt, int red, int green, int blue)

{
    int         toobig;
    CSW_F       *xyout[1];
    int         nout, memflag;
    CSW_F       *xy;

/*  if fill color is not valid, return without drawing  */

    if (red < 0  ||  green < 0  ||  blue < 0) {
        return 0;
    }

/*  add cut lines to complex polygons  */

    memflag = 0;
    gpf_calcdraw_obj.gpf_addcutlines (coords, npt, xyout, &nout, &memflag);
    
/*  if npt is gt XYMAX, allocate temporary space for polyline  */

    if (nout > XYMAX) {
        xy = (CSW_F *)csw_Malloc ((nout+2) * sizeof (CSW_F));
        if (!xy) {
            return -1;
        }
        toobig = 1;
    }

    else {
        xy = XYStatic;
        toobig = 0;
    }

/*  rescale xy arrays in window coordinates  */

    gtx_scale_arrayf (xyout[0], xy, nout);

    ezx_java_ptr->ezx_AppendFillToJavaArea (xy, 0.0f, PatternScale, nout, red, green, blue,
                              FillPattern, SelectedFlag);

/*  csw_Free temporary memory if needed  */

    if (toobig) {
        csw_Free (xy);
    }
    if (memflag) {
        csw_Free (xyout[0]);
    }
    xy = NULL;

    return 0;

}  /*  end of function gtx_drawfillprim  */



/*
******************************************************************

                   g t x _ d r a w l i n e p r i m

******************************************************************

  function name:    gtx_drawlineprim    (integer)

  call sequence:    gtx_drawlineprim (coords, npt, thick,
                                      red, green, blue, pattern)

  purpose:          draw a polyline primitive to the window

  return value:     status code

                    0 = normal successful completion
                    -1 = could not allocate memory for polyline

  calling parameters:

    coordsin     r     CSW_F*    array of x,y x,y coordinates in plotter inches
    npt          r     int       number of points in coords
    thick        r     CSW_F     line thickness in plotter inches
    red          r     int       red value for line color
    green        r     int       green value
    blue         r     int       blue value
    pattern      r     int       line dash pattern number

*/

int GTXDrawPrim::gtx_drawlineprim (CSW_F *coords, int npt, CSW_F thick, 
                      int red, int green, int blue, int pattern)

{
    int         toobig;
    CSW_F       *xy;

/*
    if npt is gt XYMAX, allocate temporary space for polyline
*/
    if (npt > XYMAX) {
        xy = (CSW_F *)csw_Malloc ((npt*2) * sizeof (CSW_F));
        if (!xy) {
            return -1;
        }
        toobig = 1;
    }

    else {
        xy = XYStatic;
        toobig = 0;
    }

/*
    rescale xy arrays in window coordinates
*/
    gtx_scale_arrayf (coords, xy, npt);

/*
 * Output the line in screen coordinates to the java memory area.
 */
    if (PolygonBorderFlag == 1) {
        ezx_java_ptr->ezx_AppendFillToJavaArea (xy, thick, PatternScale, npt, red, green, blue, pattern,
                                  SelectedFlag);
    }
    else {
        ezx_java_ptr->ezx_AppendLineToJavaArea (xy, npt, red, green, blue, pattern, thick,
                                  ImageID, SelectedFlag);
    }

/*
    csw_Free temporary memory if needed
*/
    if (toobig) {
        csw_Free (xy);
    }

    return 0;

}  /*  end of function gtx_drawlineprim  */


/*
******************************************************************

               g t x _ i n i t  _ d r a w i n g

******************************************************************

  function name:    gtx_init_drawing  (integer)

  call sequence:    gtx_init_drawing
                                 (page_xmin, page_ymin,
                                  page_xmax, page_ymax,
                                  screen_xmin, screen_ymin,
                                  screen_xmax, screen_ymax,
                                  screen_dpi, page_units_type,
                                  dfile)

  purpose:          set scaling factors for drawing the display list.

  return value:     status code

                    0 = normal successful completion
                    1 = error in setting variables

*/


int GTXDrawPrim::gtx_init_drawing 
                     (CSW_F page_xmin, CSW_F page_ymin,
                      CSW_F page_xmax, CSW_F page_ymax,
                      CSW_F lclip_xmin, CSW_F lclip_ymin,
                      CSW_F lclip_xmax, CSW_F lclip_ymax,
                      CSW_F screen_xmin, CSW_F screen_ymin,
                      CSW_F screen_xmax, CSW_F screen_ymax,
                      int screen_dpi, int page_units_type,
                      FILE *dfile_in)


{
    CSW_F            ias, fas, ftmp;
    CSW_F            itmp, scavg;

    PageUnitsType = page_units_type;

    dfile = dfile_in;
    dfile = dfile;

    ixmin = screen_xmin;
    iymin = screen_ymin;
    ixmax = screen_xmax;
    iymax = screen_ymax;

    if (ixmin >= ixmax  ||  iymin >= iymax) {
        return 1;
    }

    fxmin = page_xmin;
    fymin = page_ymin;
    fxmax = page_xmax;
    fymax = page_ymax;

    if (fxmin >= fxmax  ||  fymin >= fymax) {
        return 1;
    }

    ScreenUnitsPerInch = (CSW_F)screen_dpi;

/*  expand limits a little for clipping  */

    ftmp = (fxmax - fxmin) / 50.0f;
    fxmin -= ftmp;
    fxmax += ftmp;
    ftmp = (fymax - fymin) / 50.0f;
    fymin -= ftmp;
    fymax += ftmp;

/*  scale factors in x and y directions  */

    fas = (fxmax-fxmin) / (fymax-fymin);
    ias = ((CSW_F)(ixmax-ixmin)) / ((CSW_F)(iymax-iymin));
    if (fas > ias) {
        scalx = (ixmax-ixmin) / (fxmax-fxmin);
        scaly = scalx;
        itmp = (fymax - fymin) * scaly;
        itmp = (iymax - iymin) - itmp;
        itmp = itmp / 2.0f;
        ftmp = itmp / scaly;
        fymin -= ftmp;
        fymax += ftmp;
    }
    else {
        scaly = (iymax-iymin) / (fymax-fymin);
        scalx = scaly;
        itmp = (fxmax - fxmin) * scalx;
        itmp = (ixmax - ixmin) - itmp;
        itmp = itmp / 2.0f;
        ftmp = itmp / scalx;
        fxmin -= ftmp;
        fxmax += ftmp;
    }

    if (PageUnitsType == 2) {
        scalx = 1.0f;
        scaly = 1.0f;
        fxmin = page_xmin;
        fymin = page_ymin;
        fxmax = page_xmax;
        fymax = page_ymax;
    }

    scavg = (scalx + scaly) / 2.0f;
    PageUnitsPerInch = 1.0f / scavg;
    PageUnitsPerInch *= ScreenUnitsPerInch;

    OrigXmin = fxmin;
    OrigYmin = fymin;

    Tiny = (fxmax - fxmin + fymax - fymin) / 10000.0f;
    Tiny = Tiny;

    if (scalx > 0.0) {
        ThinDist = 1.5f / scalx;
    }
    else {
        ThinDist = 0.0;
    }

    DashFlag = 0;

    gpf_calcdraw_obj.gpf_SetClipWindow (fxmin, fymin, fxmax, fymax);

    return 0;

}  /*  end of function gtx_init_drawing  */



void GTXDrawPrim::gtx_get_drawing_clip_limits (
    CSW_F     *xmin,
    CSW_F     *ymin,
    CSW_F     *xmax,
    CSW_F     *ymax) 
{
    *xmin = fxmin;
    *ymin = fymin;
    *xmax = fxmax;
    *ymax = fymax;

    return;
} 




void GTXDrawPrim::gtx_get_page_units_per_inch (CSW_F *punits)
{
    *punits = PageUnitsPerInch;
    return;
}



/*
  ****************************************************************

           g t x _ G e t C u r r e n t S c a l e

  ****************************************************************

    Returns the scalex and scaley values for the current drawing
  window.  Scales are in screen pixels per page unit.

*/

int GTXDrawPrim::gtx_GetCurrentScale (CSW_F *sx, CSW_F *sy)
{

    *sx = scalx;
    *sy = scaly;

    return 1;

}  /*  end of function gtx_GetCurrentScale  */



/*
******************************************************************

                  g t x _ c l i p t e x t p r i m

******************************************************************

  function name:    gtx_cliptextprim      (int)

  call sequence:    gtx_cliptextprim (x, y, text, anchor, thick,
                                      red, green, blue,
                                      fred, fgreen, fblue,
                                      angle, size, font)

  purpose:          clip a single text string to the current window

  return value:     always returns zero

  calling parameters:

    x         r    CSW_F        x coordinate of lower left corner of text
    y         r    CSW_F        y coordinate of lower left
    text      r    char*        text string to plot
    anchor    r    int          anchor for text
    thick     r    CSW_F        thickness of text lines in plot inches
    red       r    int          red value for text "stroke" color
    green     r    int          green value for "stroke"
    blue      r    int          blue value for "stroke"
    fred      r    int          red value for text fill color
    fgreen    r    int          green value for fill
    fblue     r    int          blue value for fill
    angle     r    CSW_F        text angle in radians
    size      r    CSW_F        text height in inches
    font      r    int          text font number

*/

int GTXDrawPrim::gtx_cliptextprim
                     (CSW_F xin, CSW_F yin, char *text, int anchor, 
                      CSW_F thick, int red, int green, int blue,
                      int fred, int fgreen, int fblue,
                      CSW_F angle, CSW_F size, int font)

{
    CSW_F       x, y;
    CSW_F       tlen;
    CSW_F       cx[4], cy[4];
    int         nc;

/*
 * convert size to be correct on the device.
 */
    if (TextFromSymbol == 0) {
        gtx_scale_size (&size);
    }

    nc = strlen (text);
    if (nc <= 0) {
        return 1;
    }

/*  calculate corners of rectangle bounding text  */

    tlen = exact_text_length (text, size, font);

    gpf_calcdraw_obj.gpf_find_box_corners (xin, yin, anchor,
                          tlen, size, angle,
                          cx, cy);
    x = cx[0];
    y = cy[0];

/*  text is at least partially inside window, use vector clipping from here  */

    TextPolyFlag = 1;
    ThinFlag = 0;
    gtx_drawtextprim (x, y, text, nc, thick, 
                      red, green, blue, fred, fgreen, fblue,
                      angle, size, font);
    ThinFlag = 1;
    TextPolyFlag = 0;

    return 0;

}  /*  end of function gtx_cliptextprim  */



/*
******************************************************************

                  g t x _ c l i p f i l l p r i m

******************************************************************

  function name:    gtx_clipfillprim      (int)

  call sequence:    gtx_clipfillprim (coords, npt, smoothflag,
                                      fred, fgreen, fblue, 
                                      pred, pgreen, pblue,
                                      bred, bgreen, bblue,
                                      thick, outline, pattern, patscale, linepatt, dashscale,
                                      x1, y1t, x2, y2)

  purpose:          clip a single polygon fill primitive to the current window

  return value:     always returns zero

  calling parameters:

    coords      r    CSW_F*       array of x, y outline coordinates
    npts        r    int          number of points in the outline
    smoothflag  r    char         'y' to smooth, else no smooth
    fred        r    int          fill red value
    fgreen      r    int          fill green value
    fblue       r    int          fill blue value
    pred        r    int          pattern red value
    pgreen      r    int          pattern green value
    pblue       r    int          pattern blue value
    bred        r    int          boundary red value
    bgreen      r    int          boundary green value
    bblue       r    int          boundary blue value
    thick       r    CSW_F        boundary thickness
    outline     r    int          flag to outline polygon components
                                  0 = no outline
                                  1 = outline in line color and thickness
                                      connecting first and last points
                                  2 = outline only, do not connect first
                                      and last points
    pattern     r    int          cross hatch pattern number
    patscale    r    CSW_F        cross hatch pattern scale
    linepatt    r    int          dash pattern for outline of the boundary
    dashscale   r    CSW_F        boundary dash pattern scale
    x1          r    CSW_F        minimum x coordinate
    y1t         r    CSW_F        minimum y coordinate
    x2          r    CSW_F        maximum x coordinate
    y2          r    CSW_F        maximum y coordinate

*/

int GTXDrawPrim::gtx_clipfillprim (CSW_F *coords, int npt, char smoothflag, 
                      int fred, int fgreen, int fblue,
                      int pred, int pgreen, int pblue,
                      int bred, int bgreen, int bblue,
                      CSW_F thick, int outline, int pattern, 
                      CSW_F patscale, int linepatt, CSW_F dashscale,
                      CSW_F x1, CSW_F y1t, CSW_F x2, CSW_F y2)

{
    CSW_F       *xyfill[1];
    int         istat, nstat, np2, memflag;
    int         dflagsav;
    CSW_F       *xyclip;

    if (coords == NULL  ||  npt < 3) {
        return 0;
    }

    dflagsav = DashFlag;
    DashFlag = 0;
    patscale = patscale;

    x1 = x1;
    y1t = y1t;
    x2 = x2;
    y2 = y2;

    DashFlag = dflagsav;
    nstat = 0;

    xyclip = NULL;
        
/*
 * let java clip solid filled polygons
 */
    memflag = 0;
    istat = gpf_compressfillpoints (coords, npt, xyfill, &np2, &memflag);
    if (istat == -1) {
        DashFlag = dflagsav;
        if (xyclip) csw_Free (xyclip);
        return 0;
    }
    FillPattern = 0;
    ezx_java_ptr->ezx_SetAlphaValue (FillAlpha);
    gtx_drawfillprim (*xyfill, np2, fred, fgreen, fblue);

    if (pattern < 1  ||  pred < 0  ||  nstat == 1) {
        if (pattern < 1) {
            if (memflag) {
                csw_Free (*xyfill);
            }
            DashFlag = dflagsav;
            if (outline) {
                ezx_java_ptr->ezx_SetAlphaValue (BorderAlpha);
                gtx_clipfilloutline (outline, coords, npt, smoothflag, bred, bgreen, bblue,
                                     thick, linepatt, dashscale, x1, y1t, x2, y2);
            }
            if (xyclip) csw_Free (xyclip);
            return 0;
        }
    }

/*
    approximate cross hatch with stipple fill
*/
    FillPattern = pattern;
    PatternScale = patscale;
    ezx_java_ptr->ezx_SetAlphaValue (PatternAlpha);
    gtx_drawfillprim (*xyfill, np2, pred, pgreen, pblue);
    ShapeFillFlag = 0;
    FillPattern = 0;
    PatternScale = 1.0f;

    if (outline) {
        ezx_java_ptr->ezx_SetAlphaValue (BorderAlpha);
        gtx_clipfilloutline (outline, coords, npt, smoothflag, bred, bgreen, bblue, thick, linepatt,
                             dashscale, x1, y1t, x2, y2);
    }

    if (memflag) {
        csw_Free (*xyfill);
    }

    DashFlag = dflagsav;

    if (xyclip) csw_Free (xyclip);

    return 0;

}  /*  end of function gtx_clipfillprim  */




/*
******************************************************************

               g t x _ c l i p f i l l o u t l i n e

******************************************************************

  function name:    gtx_clipfilloutline      (int)

  call sequence:    gtx_clipfilloutline (outline, xyin, nin, smflag, red, green, blue,
                                         thick, linepatt, dashscale, x1, y1t, x2, y2)

  purpose:          Clip the outline of the filled polygon to the current
                    window and draw it.  This is only called from gtx_clipfillprim.

  return value:     Always returns zero

  calling parameters:

    outline   r   int          1 = connect 1st and last   2 = no connect
    xyin      r   CSW_F*       array of polygon boundary points, with hole flags
    nin       r   int          number of boundary points (including hole flags)
    smflag    r   char         flag for smoothing in the draw function
                               'y' = smooth, otherwise no smooth
    red       r   int          red value of boundary color
    green     r   int          green value
    blue      r   int          blue value
    thick     r   CSW_F        thickness of boundary
    linepatt  r   int          dash pattern for boundary
    dashscale r   CSW_F        boundary dash scale factor
    x1        r   CSW_F        minimum x
    y1t       r   CSW_F        minimum y
    x2        r   CSW_F        maximum x
    y2        r   CSW_F        maximum y

*/

int GTXDrawPrim::gtx_clipfilloutline (int outline, CSW_F *xyin, int nin, char smflag, 
                         int red, int green, int blue, CSW_F thick, int linepatt,
                         CSW_F dashscale, CSW_F x1, CSW_F y1t, CSW_F x2, CSW_F y2)

{
    CSW_F      *xyw, *tmp, xyt[8], xanc, yanc;
    CSW_F      *xylast;
    double     dx, dy, dxy;
    int        i, n, n2;

/*  hole closure grazing distance  */

    dxy = (x2 - x1 + y2 - y1t) / 100000.;

/*  separate each hole and outline it  */

    tmp = xyin;
    xyw = xyin;
    n = 0;
    xanc = *tmp;
    yanc = *(tmp + 1);

    smflag = 0;

    if (TextPolyFlag == 0) {
        PolygonBorderFlag = 1;
    }

    for (i=0; i<nin; i++) {
        if (*tmp >= HoleFlag2  ||  *tmp <= -HoleFlag2) {
            if (n > 2) {
                gtx_cliplineprim (xyw, n-1, smflag, thick, red, green, blue,
                                  linepatt, dashscale, 0);
                n = n * 2 - 4;
                gtx_cliplineprim (xyw+n, 2, smflag, thick, red, green, blue,
                    linepatt, dashscale, 0);
                if (outline == 1) {
                    n2 = n + 2;
                    if (*xyw - *(xyw+n2) != 0.0  ||  *(xyw+1) - *(xyw+n2+1) != 0.0) {
                        xyt[0] = *xyw;
                        xyt[1] = *(xyw+1);
                        xyt[2] = *(xyw+n+2);
                        xyt[3] = *(xyw+n+3);
                        gtx_cliplineprim (xyt, 2, smflag, thick, red, green, blue,
                                          linepatt, dashscale, 0);
                    }
                }
            }
            n = 0;
            tmp += 2;
            xyw = tmp;
        }
        else {
            dx = *tmp - xanc;
            tmp++;
            dy = *tmp - yanc;
            tmp++;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            if (dx <= dxy  &&  dy <= dxy  &&  n > 2) {
                n++;
                gtx_cliplineprim (xyw, n, smflag, thick, red, green, blue,
                                  linepatt, dashscale, 0);
                n = 0;
                if (i < nin -1) {
                    xyw = tmp;
                    xanc = *tmp;
                    yanc = *(tmp + 1);
                }
            }
            else {
                n++;
            }
        }
    }

/*  last hole if needed  */

    if (n > 2) {
        xylast = (CSW_F *)csw_Malloc ((n + 1) * 2 * sizeof(CSW_F));
        if (xylast) {
            memcpy (xylast, xyw, n * 2 * sizeof(CSW_F));
            if (outline == 1) {
                xylast[2 * n] = xylast[0];
                xylast[2 * n + 1] = xylast[1];
                n++;
            }
            gtx_cliplineprim (xylast, n, smflag, thick, red, green, blue,
                              linepatt, dashscale, 0);
        }
        csw_Free (xylast);
        xylast = NULL;
    }

    PolygonBorderFlag = 0;

    return 0;

}  /*  end of function gtx_clipfilloutline  */




/*
******************************************************************

                  g t x _ c l i p s y m b p r i m

******************************************************************

  function name:    gtx_clipsymbprim      (int)

  call sequence:    gtx_clipsymbprim (x, y, symb, size, thick, red, green, blue, angle, mask)

  purpose:          clip a single symbol to the current window

  return value:     always returns zero

  calling parameters:

    x         r    CSW_F        x coordinate of lower left corner of text
    y         r    CSW_F        y coordinate of lower left
    symb      r    int          number of the symbol to plot
    size      r    CSW_F        text height in inches
    thick     r    CSW_F        thickness of text lines in plot inches
    red       r    int          red value of symbol color
    green     r    int          green value
    blue      r    int          blue value
    angle     r    CSW_F        text angle in radians
    mask      r    int          0 = no symbol masking,   1 = do symbol masking

*/

int GTXDrawPrim::gtx_clipsymbprim 
     (CSW_F x, CSW_F y, int symb, CSW_F size, CSW_F thick, 
      int red, int green, int blue,
      CSW_F angle, int mask)

{

/*
 * convert size to be correct on the device.
 */
    gtx_scale_size (&size);

/*  symbol is at least partially inside window,
 *  convert to lines, fills, etc and let java clip
 *  these simple elements.
 */

    TextPolyFlag = 1;
    ThinFlag = 0;
    ezx_java_ptr->ezx_SetImageIDInJavaArea (ImageID);
    gtx_drawsymbprim (x, y, symb, size, thick, red, green, blue, angle, mask);
    ezx_java_ptr->ezx_SetImageIDInJavaArea (-1);
    ThinFlag = 1;
    TextPolyFlag = 0;

    return 0;

}  /*  end of function gtx_clipsymbprim  */





/*
******************************************************************

                  g t x _ d r a w t e x t p r i m

******************************************************************

  function name:    gtx_drawtextprim      (int)

  call sequence:    gtx_drawtextprim (x, y, text, nc, thick, red, green, blue,
                                      fred, fgreen, fblue, angle, size, font)

  purpose:          draw a single text string

  return value:     always returns zero

  calling parameters:

    x         r    CSW_F        x coordinate of lower left corner of text
    y         r    CSW_F        y coordinate of lower left
    text      r    char*        text string to plot
    nc        r    int          number of characters in text
    thick     r    CSW_F        thickness of text lines in plot inches
    red       r    int          red value for "strokes" in text
    green     r    int          green value for strokes
    blue      r    int          blue value for strokes
    fred      r    int          red value for fill
    fgreen    r    int          green value for fill
    fblue     r    int          blue value for fill
    angle     r    CSW_F        text angle in radians
    size      r    CSW_F        text height in inches
    font      r    int          text font number

*/
int GTXDrawPrim::gtx_drawtextprim (CSW_F x, CSW_F y, char *text, int nc, CSW_F thick,
                      int red, int green, int blue,
                      int fred, int fgreen, int fblue,
                      CSW_F angle, CSW_F size, int font)

{
    int         dflagsav;
    CSW_F       scaled_x, scaled_y;
    char        local_text[500];

    dflagsav = DashFlag;
    DashFlag = 0;

    if (nc <= 0  ||  nc > 498) {
        strncpy (local_text, text, 499);
        local_text[499] = '\0';
    }
    else {
        strncpy (local_text, text, nc);
        local_text[nc] = '\0';
    }

/*
 * If the font is 200 or greater, that is a Java font 
 * and let Java draw the text.
 */
    if (font >= 200) {
        gtx_scalef (x, y, &scaled_x, &scaled_y);
        ezx_java_ptr->ezx_AppendTextToJavaArea (scaled_x, scaled_y, local_text,
                                  red, green, blue,
                                  angle, size/PageUnitsPerInch, font, 
                                  ImageID, SelectedFlag);
    }

/*
 * Other fonts are stroked from the EasyX font data.
 */
    else {
        if (font < 8  ||  font > 100) {
            gtx_drawstroketext (x, y, text, nc, thick, red, green, blue, 
                                angle, size, font);
        }
        else {
            gtx_drawpolytext (x, y, text, nc, 
                              thick, red, green, blue, fred, fgreen, fblue,
                              angle, size, font);
        }
    }

    DashFlag = dflagsav;
 
    return 0;

}  /*  end of function gtx_drawtextprim  */






/*
******************************************************************

               g t x _ d r a w s t r o k e t e x t

******************************************************************

  function name:    gtx_drawstroketext       (int)

  call sequence:    gtx_drawstroketext (x, y, text, nc, thick, red, green, blue,
                                        angle, size, font)

  purpose:          Draw text to the window using vector and polygon fill
                    calls instead of server fonts.  This is used when no
                    server font is close enough to the font and size wanted.

  return value:     Always returns zero.

  calling parameters:

    x          r    CSW_F      x coordinate of text lower left corner
    y          r    CSW_F      y coordinate of text
    text       r    char*      text string to plot
    nc         r    int        number of characters to plot
    thick      r    CSW_F      text thickness in plot units
    red        r    int        red value for text color
    green      r    int        green value
    blue       r    int        blue value
    angle      r    CSW_F      text angle in radians
    size       r    CSW_F      text height in plot units
    font       r    int        vector font number

*/

int GTXDrawPrim::gtx_drawstroketext (CSW_F x, CSW_F y, char *text, int nc, 
                        CSW_F thick, int red, int green, int blue,
                        CSW_F angle, CSW_F size, int font)

{
    CSW_F          textlen, charwidth, cosang, sinang, xchar, ychar, *fp;
    int            istat, i, j, k, kk, n, nparts;

    int            parts[MAXPARTS];

/*  handle default text width  */

    if (thick > size) {
        thick = size / 20.0f;
        if (thick < 0.005) {
            thick = 0.005f;
        }
    }

    sinang = (CSW_F)(sin((double)angle * 3.1415926 / 180.0));
    cosang = (CSW_F)(cos((double)angle * 3.1415926 / 180.0));

    gpf_font_obj.gpf_setfont (font);

/*  loop through text, plotting each character  */

    xchar = x;
    ychar = y;
    textlen = 0.0;

    for (i=0; i<nc; i++) {

        if (isspace(text[i])) {
            gpf_font_obj.gpf_CharWidth (text[i], size, &charwidth);
            textlen += charwidth;
            xchar = textlen * cosang + x;
            ychar = textlen * sinang + y;
            continue;
        }
        
/*      get strokes for a character  */

        istat = gpf_font_obj.gpf_getfontstrokes (text[i], size, Fontx, Fonty, parts, &nparts, 
                                    MAXFONT, MAXPARTS);
        if (!istat) {
            continue;
        }

        n = 0;
        for (j=0; j<nparts; j++) {
            n += parts[j];
        }

/*      rotate the character if not at a zero angle  */

        if (angle < -0.005  ||  angle > 0.005) {
            gpf_font_obj.gpf_rotatechar (Fontx, Fonty, n, cosang, sinang);
        }

/*      translate to plot coordinates  */

        for (j=0; j<n; j++) {
            Fontx[j] += xchar;
            Fonty[j] += ychar;
        }

/*      plot each piece of the character  */

        kk = 0;
        for (j=0; j<nparts; j++) {
            n = parts[j];
            fp = Fontxy;
            for (k=0; k<n; k++) {
                *fp = Fontx[kk];
                fp++;
                *fp = Fonty[kk];
                fp++;
                kk++;
            }
            gtx_cliplineprim (Fontxy, n, 0, thick, red, green, blue, 0, (CSW_F)1.0, 0);
        }

/*      get position of next character  */

        gpf_font_obj.gpf_CharWidth (text[i], size, &charwidth);
        textlen += charwidth;
        xchar = textlen * cosang + x;
        ychar = textlen * sinang + y;

    }

    return 0;

}  /*  end of function gtx_drawstroketext  */





/*
******************************************************************

               g t x _ d r a w p o l y t e x t

******************************************************************

  function name:    gtx_drawpolytext       (int)

  call sequence:    gtx_drawpolytext (x, y, text, nc, thick,
                                      red, green, blue, fred, fgreen, fblue,
                                      angle, size, font)

  purpose:          Draw text to the window using vector and polygon fill
                    functions.  This is called when the current font is
                    a polygon fill font, as opposed to a vector font.

  return value:     Always returns zero.

  calling parameters:

    x          r    CSW_F      x coordinate of text lower left corner
    y          r    CSW_F      y coordinate of text
    text       r    char*      text string to plot
    nc         r    int        number of characters to plot
    thick      r    CSW_F      text thickness in plot units
    red        r    int        red value for outline color
    green      r    int        green value for outline
    blue       r    int        blue value for outline
    fred       r    int        red value for fill color
    fgreen     r    int        green value for fill color
    fblue      r    int        blue value for fill
    angle      r    CSW_F      text angle in radians
    size       r    CSW_F      text height in plot units
    font       r    int        vector font number

*/

int GTXDrawPrim::gtx_drawpolytext (CSW_F x, CSW_F y, char *text, int nc, CSW_F thick,
                      int red, int green, int blue, int fred, int fgreen, int fblue,
                      CSW_F angle, CSW_F size, int font)

{
    CSW_F          textlen, charwidth, cosang, sinang, xchar, ychar, *fp,
                   x1, y1t, x2, y2;
    int            i, j, k, l, kk, n, nn, n2,
                   nparts, nholes, nh;

/*  handle default text width  */

    if (thick > size) {
        thick = size / 30.0f;
        if (thick < 0.005) {
            thick = 0.005f;
        }
    }

    sinang = (CSW_F)(sin((double)angle * 3.1415926 / 180.0));
    cosang = (CSW_F)(cos((double)angle * 3.1415926 / 180.0));

    gpf_font_obj.gpf_setfont (font);

/*  set up fill color  */

/*  loop through text, plotting each character  */

    xchar = x;
    ychar = y;
    textlen = 0.0;

    for (i=0; i<nc; i++) {

        if (isspace(text[i])) {
            gpf_font_obj.gpf_CharWidth (text[i], size, &charwidth);
            textlen += charwidth;
            xchar = textlen * cosang + x;
            ychar = textlen * sinang + y;
            continue;
        }
        
/*      get shadow points if there are any and draw the shadow polygons  */

        if (gpf_font_obj.gpf_getfontshadow (text[i], size,
                               Fontx, Fonty, &nparts, FontComp, FontHole)) {

/*          get number of holes and points for the shadows  */

            nholes = 0;
            for (j=0; j<nparts; j++) {
                nholes += FontComp[j];
            }

            n = 0;
            for (j=0; j<nholes; j++) {
                n += FontHole[j];
            }

/*          rotate the shadows if not at a zero angle  */

            if (angle < -0.005  ||  angle > 0.005) {
                gpf_font_obj.gpf_rotatechar (Fontx, Fonty, n, cosang, sinang);
            }

/*          translate to shadow coordinates and get limits  */

            x1 = 1.e30f;
            y1t = 1.e30f;
            x2 = -1.e30f;
            y2 = -1.e30f;
            for (j=0; j<n; j++) {
                Fontx[j] += xchar;
                Fonty[j] += ychar;
                if (Fontx[j] < x1) x1 = Fontx[j];
                if (Fonty[j] < y1t) y1t = Fonty[j];
                if (Fontx[j] > x2) x2 = Fontx[j];
                if (Fonty[j] > y2) y2 = Fonty[j];
            }

/*          plot each piece of the shadow  */

            nh = 0;
            kk = 0;
            for (j=0; j<nparts; j++) {
                n = FontComp[j];
                fp = Fontxy;
                nn = 0;
                for (k=0; k<n; k++) {
                    n2 = FontHole[nh];
                    nh++;
                    for (l=0; l<n2; l++) {
                        *fp = Fontx[kk];
                        fp++;
                        *fp = Fonty[kk];
                        fp++;
                        kk++;
                        nn++;
                    }
                }
                gtx_clipfillprim (Fontxy, nn, 0, 
                                  RedFG, GreenFG, BlueFG, 
                                  -1, -1, -1, -1, -1, -1,
                                  0.0, 0, -1, (CSW_F)1.0, 0, (CSW_F)1.0,
                                  x1, y1t, x2, y2);
            }
        }

/*      get points, components and holes for a character  */

        if (!gpf_font_obj.gpf_getpolyfontpts (text[i], size, 
                                 Fontx, Fonty, &nparts, FontComp, FontHole)) { 
            continue;
        }

/*      get number of holes and points  */

        nholes = 0;
        for (j=0; j<nparts; j++) {
            nholes += FontComp[j];
        }

        n = 0;
        for (j=0; j<nholes; j++) {
            n += FontHole[j];
        }

/*      rotate the character if not at a zero angle  */

        if (angle < -0.005  ||  angle > 0.005) {
            gpf_font_obj.gpf_rotatechar (Fontx, Fonty, n, cosang, sinang);
        }

/*      translate to plot coordinates and get limits  */

        x1 = 1.e30f;
        y1t = 1.e30f;
        x2 = -1.e30f;
        y2 = -1.e30f;
        for (j=0; j<n; j++) {
            Fontx[j] += xchar;
            Fonty[j] += ychar;
            if (Fontx[j] < x1) x1 = Fontx[j];
            if (Fonty[j] < y1t) y1t = Fonty[j];
            if (Fontx[j] > x2) x2 = Fontx[j];
            if (Fonty[j] > y2) y2 = Fonty[j];
        }

/*      plot each piece of the character  */

        nh = 0;
        kk = 0;
        for (j=0; j<nparts; j++) {
            n = FontComp[j];
            fp = Fontxy;
            nn = 0;
            for (k=0; k<n; k++) {
                n2 = FontHole[nh];
                nh++;
                for (l=0; l<n2; l++) {
                    *fp = Fontx[kk];
                    fp++;
                    *fp = Fonty[kk];
                    fp++;
                    kk++;
                    nn++;
                }
            }
            gtx_clipfillprim (Fontxy, nn, 0, 
                              fred, fgreen, fblue, -1, -1, -1, red, green, blue,
                              thick, 1, -1, (CSW_F)1.0, 0, (CSW_F)1.0,
                              x1, y1t, x2, y2);
        }

/*      get position of next character  */

        gpf_font_obj.gpf_CharWidth (text[i], size, &charwidth);
        textlen += charwidth;
        xchar = textlen * cosang + x;
        ychar = textlen * sinang + y;

    }

    return 0;

}  /*  end of function gtx_drawpolytext  */




/*
******************************************************************

                  g t x _ d r a w s y m b p r i m

******************************************************************

  function name:    gtx_drawsymbprim      (int)

  call sequence:    gtx_drawsymbprim (x, y, symb, size, thick,
                                      red, green, blue, angle, mask)

  purpose:          draw a single symbol

  return value:     always returns zero

  calling parameters:

    x         r    CSW_F        x coordinate of lower left corner of text
    y         r    CSW_F        y coordinate of lower left
    symb      r    int          symbol number to draw
    size      r    CSW_F        text height in inches
    thick     r    CSW_F        thickness of text lines in plot inches
    red       r    int          red value for symbol color
    green     r    int          green value
    blue      r    int          blue value
    angle     r    CSW_F        symbol angle in radians

*/

int GTXDrawPrim::gtx_drawsymbprim (CSW_F x, CSW_F y, int symb, CSW_F size, CSW_F thick, 
                      int red, int green, int blue, CSW_F angle, int mask)

{

    static const SYmVecList    *veclist[MAXSYMITEM];
    static const SYmText       *textlist[MAXSYMITEM];
    static const SYmCircle     *circlelist[MAXSYMITEM];
    CSW_F                      xylocal[MAXVECTORXY];

    int         istat, ifill, i, nc, font, 
                npts, ncircle, nvec, ntext;
    CSW_F       x1, y1t, x2, y2, ang1, ang2, xt, yt, tsize;
    CSW_F       r1, r2;
    int         dflagsav;

    dflagsav = DashFlag;

    symb--;
    gpf_symbol_obj.gpf_setsymbolparms (symb, x, y, size, angle);

/*  get the pieces for the symbol  */

    istat = gpf_symbol_obj.gpf_getsymbolvec (symb, veclist, &nvec);
    if (!istat) {
        DashFlag = dflagsav;
        return 0;
    }

    istat = gpf_symbol_obj.gpf_getsymbolcircle (symb, circlelist, &ncircle);
    if (!istat) {
        DashFlag = dflagsav;
        return 0;
    }

    istat = gpf_symbol_obj.gpf_getsymboltext (symb, textlist, &ntext);
    if (!istat) {
        DashFlag = dflagsav;
        return 0;
    }

/*
    draw the mask shapes if specified
*/
    if (mask) {

/*
        for (i=0; i<ncircle; i++) {
            if (circlelist[i]->ifill >= 10) {
                xt = circlelist[i]->x;
                yt = circlelist[i]->y;
                r1 = circlelist[i]->r1;
                r2 = circlelist[i]->r2;
                ang1 = circlelist[i]->ang1;
                ang2 = circlelist[i]->ang2;

                gpf_symbol_obj.gpf_symcircletrans (&xt, &yt, &r1, &r2, &ang1);
                gtx_clipfillcircle (xt, yt, r1, r2, ang1, ang2, 2,
                                    RedBG, GreenBG, BlueBG, angle);
            }
        }

        for (i=0; i<nvec; i++) {
            if (veclist[i]->fill >= 10) {
                istat = gpf_symbol_obj.gpf_getsymbolxy (veclist[i], xylocal, &npts);
                if (!istat  ||  npts < 2) {
                    continue;
                }
                gpf_symbol_obj.gpf_symlinetrans (xylocal, npts);
                gpf_xylimits (xylocal, npts, &x1, &y1t, &x2, &y2);
                gtx_clipfillprim (xylocal, npts/2, 0,
                                  RedBG, GreenBG, BlueBG,
                                  RedBG, GreenBG, BlueBG,
                                  RedBG, GreenBG, BlueBG,
                                  0.005f, 0, 0, 1.0f, 0, 1.0f,
                                  x1, y1t, x2, y2);
            }
        }
*/
    }

/*  plot unfilled circles  */

    for (i=0; i<ncircle; i++) {
        ifill = circlelist[i]->ifill;
        if (ifill > 1) ifill -= 10;
        if (ifill == 0) {
            xt = circlelist[i]->x;
            yt = circlelist[i]->y;
            r1 = circlelist[i]->r1;
            r2 = circlelist[i]->r2;
            ang1 = circlelist[i]->ang1;
            ang2 = circlelist[i]->ang2;

            gpf_symbol_obj.gpf_symcircletrans (&xt, &yt, &r1, &r2, &ang1);
            gtx_clipcircle (xt, yt, r1, r2, ang1, ang2, 2,
                            red, green, blue, thick, angle);
        }
    }

/*  plot polygonal fill areas  */

    for (i=0; i<nvec; i++) {
        ifill = veclist[i]->fill;
        if (ifill > 1) ifill -= 10;
        if (ifill) {
            istat = gpf_symbol_obj.gpf_getsymbolxy (veclist[i], xylocal, &npts);
            if (!istat  ||  npts < 2) {
                continue;
            }
            gpf_symbol_obj.gpf_symlinetrans (xylocal, npts);
            gpf_xylimits (xylocal, npts, &x1, &y1t, &x2, &y2);
            gtx_clipfillprim (xylocal, npts/2, 0, 
                              red, green, blue,
                              red, green, blue,
                              red, green, blue,
                              0.005f, 0, 0, 1.0f, 0, 1.0f,
                              x1, y1t, x2, y2);
        }
    }

/*  plot filled circles  */

    for (i=0; i<ncircle; i++) {
        ifill = circlelist[i]->ifill;
        if (ifill > 1) ifill -= 10;
        if (ifill) {
            xt = circlelist[i]->x;
            yt = circlelist[i]->y;
            r1 = circlelist[i]->r1;
            r2 = circlelist[i]->r2;
            ang1 = circlelist[i]->ang1;
            ang2 = circlelist[i]->ang2;

            gpf_symbol_obj.gpf_symcircletrans (&xt, &yt, &r1, &r2, &ang1);
            gtx_clipfillcircle (xt, yt, r1, r2, ang1, ang2, 2,
                                red, green, blue, angle);
        }
    }

/*  plot lines  */

    for (i=0; i<nvec; i++) {
        ifill = veclist[i]->fill;
        if (ifill > 1) ifill -= 10;
        if (!ifill) {
            istat = gpf_symbol_obj.gpf_getsymbolxy (veclist[i], xylocal, &npts);
            if (!istat || npts<2) {
                continue;
            }
            gpf_symbol_obj.gpf_symlinetrans (xylocal, npts);
            gtx_cliplineprim (xylocal, npts/2, 0, thick, 
                              red, green, blue, 0, (CSW_F)1.0, 0);
        }
    }

/*  plot text  */

    TextFromSymbol = 1;
    for (i=0; i<ntext; i++) {
        xt = textlist[i]->x;
        yt = textlist[i]->y;
        tsize = textlist[i]->size;
        ang1 = textlist[i]->angle;
        nc = textlist[i]->nc;
        font = textlist[i]->font;

        gpf_symbol_obj.gpf_symtexttrans (&xt, &yt, &tsize, &ang1);
        gtx_cliptextprim (xt, yt,
                          const_cast <char *> (textlist[i]->text),
                          nc, thick, 
                          red, green, blue, red, green, blue,
                          ang1, tsize, font);
    }
    TextFromSymbol = 0;

    DashFlag = dflagsav;

    return 1;
    
}  /*  end of function gtx_drawsymbprim  */





/*
******************************************************************

               g t x _ d r a w c i r c l e

******************************************************************

  function name:    gtx_drawcircle     (int)

  call sequence:    gtx_drawcircle (x, y, r1, r2, ang1, ang2,
                                    red, green, blue, thick, angle)

  purpose:          Draw a open circle centered at x, y.

  return value:     Always returns zero

  calling parameters:

    x       r    CSW_F      x coordinate of center in plotter units
    y       r    CSW_F      y coordinate of center in plotter units
    r1      r    CSW_F      x radius of circle in plotter units
    r2      r    CSW_F      y radius of circle in plotter units
    ang1    r    CSW_F      start angle of circular arc in degrees
    ang2    r    CSW_F      angle length of circular arc
    red     r    int        red value of circle color
    green   r    int        green value
    blue    r    int        blue value
    thick   r    CSW_F      thickness of circle outline
    angle   r    CSW_F      angle if the arc is an ellipse, ignored if r1 = r2

*/

int GTXDrawPrim::gtx_drawcircle (CSW_F x, CSW_F y, CSW_F r1, CSW_F r2, 
                    CSW_F ang1, CSW_F ang2, int closure,
                    int red, int green, int blue,
                    CSW_F thick, CSW_F angle)

{
    CSW_F      scaled_x, scaled_y;
    CSW_F      scaled_r1, scaled_r2;
    CSW_F      xt, yt;

    gtx_scalef (x, y, &scaled_x, &scaled_y);
    gtx_scalef (x + r1, y, &xt, &yt);
    scaled_r1 = xt - scaled_x;
    gtx_scalef (x + r2, y, &xt, &yt);
    scaled_r2 = xt - scaled_x;
    ezx_java_ptr->ezx_AppendArcToJavaArea (scaled_x, scaled_y, scaled_r1, scaled_r2,
                             ang1, ang2, closure, red, green, blue, thick, angle,
                             SelectedFlag);

    return 0;

}  /*  end of function gtx_drawcircle  */




/*
******************************************************************

               g t x _ d r a w f i l l c i r c l e

******************************************************************

  function name:    gtx_drawfillcircle     (int)

  call sequence:    gtx_drawfillcircle (x, y, r1, r2, ang1, ang2, red, green, blue, angle)

  purpose:          Fill all or part of a circle centered at x, y.

  return value:     Always returns zero

  calling parameters:

    x       r    CSW_F      x coordinate of center in plotter units
    y       r    CSW_F      y coordinate of center in plotter units
    r1      r    CSW_F      x radius of circle in plotter units
    r2      r    CSW_F      y radius of circle in plotter units
    ang1    r    CSW_F      first angle for circle fill in degrees
    ang2    r    CSW_F      angle length for circle fill
    red     r    int        red value for filled circle color
    green   r    int        green value
    blue    r    int        blue value
    angle   r    CSW_F      angle of r1 in ellipse in radians
    
        The circle will be filled in a pie slice manner, starting at
        ang1 and going counter clockwise to ang2,

*/

int GTXDrawPrim::gtx_drawfillcircle (CSW_F x, CSW_F y, CSW_F r1, CSW_F r2, 
                        CSW_F ang1, CSW_F ang2, int closure,
                        int red, int green, int blue, CSW_F angle)

{
    CSW_F      scaled_x, scaled_y;
    CSW_F      scaled_r1, scaled_r2;
    CSW_F      xt, yt;

    gtx_scalef (x, y, &scaled_x, &scaled_y);
    gtx_scalef (x + r1, y, &xt, &yt);
    scaled_r1 = xt - scaled_x;
    gtx_scalef (x + r2, y, &xt, &yt);
    scaled_r2 = xt - scaled_x;
    ezx_java_ptr->ezx_AppendFilledArcToJavaArea (scaled_x, scaled_y, scaled_r1, scaled_r2,
                                   ang1, ang2, closure, red, green, blue, 0.0f, angle,
                                   FillPattern, SelectedFlag);

    return 0;

}  /*  end of function gtx_drawfillcircle  */





/*
******************************************************************

               g t x _ c l i p f i l l c i r c l e

******************************************************************

  function name:    gtx_clipfillcircle       (int)

  call sequence:    gtx_clipfillcircle (x, y, r1, r2, ang1, ang2,
                                        red, green, blue, angle) 

  purpose:          clip a filled circle to the current viewport and draw it.

  return value:     always returns zero

  calling parameters:

    x       r    CSW_F   x coordinate of circle center
    y       r    CSW_F   y coordinate of circle center
    r1      r    CSW_F   x radius of circle
    r2      r    CSW_F   y radius of circle
    ang1    r    CSW_F   first fill angle in degrees
    ang2    r    CSW_F   angle length of arc in degrees
    closure r    int     0 = no closure
                         1 = pie
                         2 = chord
    red     r    int     red value for fill circle color
    green   r    int     green value
    blue    r    int     blue value
    angle   r    CSW_F   ellipse r1 angle in radians

*/

int GTXDrawPrim::gtx_clipfillcircle (CSW_F x, CSW_F y, CSW_F r1, CSW_F r2, 
                        CSW_F ang1, CSW_F ang2, int closure,
                        int red, int green, int blue, CSW_F angle)

{
    CSW_F      x1, y1t, x2, y2, rmax;

    if (r1 > r2) {
        rmax = r1;
    }
    else {
        rmax = r2;
    }

    x1 = x-rmax;
    y1t = y-rmax;
    x2 = x+rmax;
    y2 = y+rmax;

    if (x1 > fxmax  ||  y1t > fymax  ||  x2 < fxmin  ||  y2 < fymin) {
        return 0;
    }

    gtx_drawfillcircle (x, y, r1, r2, ang1, ang2, closure,
                        red, green, blue, angle);

    return 0;

}  /*  end of function gtx_clipfillcircle  */





/*
******************************************************************

               g t x _ c l i p c i r c l e

******************************************************************

  function name:    gtx_clipcircle       (int)

  call sequence:    gtx_clipcircle (x, y, r1, r2, ang1, ang2, red, green, blue, thick, angle) 

  purpose:          clip a circle to the current viewport and draw it.

  return value:     always returns zero

  calling parameters:

    x       r    CSW_F   x coordinate of circle center
    y       r    CSW_F   y coordinate of circle center
    r1      r    CSW_F   x radius of circle
    r2      r    CSW_F   y radius of circle
    ang1    r    CSW_F   start angle of circular arc in degrees
    ang2    r    CSW_F   angle length of circular arc in degrees
    closure r    int     0 = no closure
                         1 = pie
                         2 = chord
    red     r    int     red value for circle color
    green   r    int     green value
    blue    r    int     blue value
    thick   r    CSW_F   thickness of arc
    angle   r    CSW_F   for ellipses, this is the angle of the x radius

*/

int GTXDrawPrim::gtx_clipcircle (CSW_F x, CSW_F y, CSW_F r1, CSW_F r2, 
                    CSW_F ang1, CSW_F ang2, int closure,
                    int red, int green, int blue, CSW_F thick, CSW_F angle)

{
    CSW_F      x1, y1t, x2, y2, rmax;

    if (r1 > r2) {
        rmax = r1;
    }
    else {
        rmax = r2;
    }

    x1 = x-rmax;
    y1t = y-rmax;
    x2 = x+rmax;
    y2 = y+rmax;

    if (x1 > fxmax  ||  y1t > fymax  ||  x2 < fxmin  ||  y2 < fymin) {
        return 0;
    }

    gtx_drawcircle (x, y, r1, r2, ang1, ang2, closure,
                    red, green, blue, thick, angle);

    return 0;

}  /*  end of function gtx_clipcircle  */




/*
  ****************************************************************

                 g t x _ c l i p b o x p r i m

  ****************************************************************

*/

int GTXDrawPrim::gtx_clipboxprim (CSW_F x1, CSW_F y1t, CSW_F x2, CSW_F y2, 
                     CSW_F thick, int red, int green, int blue)

{
    CSW_F         xylocal[10];

    xylocal[0] = x1;
    xylocal[1] = y1t;
    xylocal[2] = x1;
    xylocal[3] = y2;
    xylocal[4] = x2;
    xylocal[5] = y2;
    xylocal[6] = x2;
    xylocal[7] = y1t;
    xylocal[8] = x1;
    xylocal[9] = y1t;

    gtx_cliplineprim (xylocal, 5, 0, thick, 
                      red, green, blue, 0, (CSW_F)1.0, 0);

    return 0;

}  /*  end of function gtx_clipboxprim  */



/*
  ****************************************************************

                g t x _ c l i p r e c t p r i m

  ****************************************************************

    Create an array of points representing the rectangle primitive
  and draw it as a polygon.

*/

int GTXDrawPrim::gtx_cliprectprim
                     (CSW_F xc, CSW_F yc, CSW_F wide, CSW_F high, 
                      CSW_F angd, CSW_F crad,
                      int fred, int fgreen, int fblue,
                      int pred, int pgreen, int pblue,
                      int bred, int bgreen, int bblue,
                      CSW_F thick, int pattern, CSW_F patscale,
                      int linepatt, CSW_F dashscale)

{
    CSW_F           tiny, *xylocal, *xyout, *xyt, *x, *y, xr[6], yr[6],
                    w2, h2, xmin, ymin, xmax, ymax;
    int             npt, npout, icout[20];
    int             outline, ido, no_outline;

    no_outline = 0;
    if (crad < 0.0) {
        no_outline = 1;
        crad = 0.0;
    }

/*
    adjust crad to be no larger than half the 
    smallest rectangle dimension
*/
    tiny = (wide + high) / 1000.0f;
    if (crad < tiny) {
        crad = -1.0f;
    }

    w2 = wide / 2.0f;
    h2 = high / 2.0f;

    if (crad > w2) crad = w2;
    if (crad > h2) crad = h2;

/*
    points for box without curved corners
*/
    gpf_shape_obj.gpf_makeoriginbox (wide, high, xr, yr);

/*
    add curved corners if needed
*/
    x = NULL;
    y = NULL;
    if (crad < tiny) {
        npt = 5;
        x = xr;
        y = yr;
    }
    else {
        npt = 0;
        gpf_shape_obj.gpf_curveboxcorners (xr, yr, crad, &x, &y, &npt);
    }

    if (npt < 5) {
        if (crad >= tiny) {
            csw_Free (x);
            csw_Free (y);
        }
        return -1;
    }

/*
    allocate workspace array
*/
    xylocal = (CSW_F *)csw_Malloc ((npt + 1) * 2 * sizeof(CSW_F));
    if (!xylocal) {
        if (crad >= tiny) {
            csw_Free (x); /*lint !e424*/
            csw_Free (y); /*lint !e424*/
        }
        return -1;
    }

/*
    rotate and translate the rectangle
*/
    gpf_shape_obj.gpf_rotatepoints (x, y, npt, angd);
    gpf_shape_obj.gpf_translatepoints (x, y, npt, xc, yc);
    gpf_packxy2 (x, y, npt, xylocal);

    xyout = NULL;
    xyt = xylocal;
    npout = 1;
    icout[0] = npt;
        
/*
    send the points to the polygon clip function
*/
    for (ido=0; ido<npout; ido++) {
        npt = icout[ido];
        gpf_xylimits (xyt, npt * 2, &xmin, &ymin, &xmax, &ymax);

        ply_patfill_obj.ply_SetPattClipSaveFlag (1);
        ShapeFillFlag = 1;
        outline = 1;
        if (xyout  ||  no_outline) outline = 0;
        FillPattern = 0;
        gtx_clipfillprim (xyt, npt, 0,
                          fred, fgreen, fblue,
                          pred, pgreen, pblue,
                          bred, bgreen, bblue,
                          thick, outline, 0, patscale, linepatt, dashscale,
                          xmin, ymin, xmax, ymax);
        ply_patfill_obj.ply_SetPattClipSaveFlag (0);
        if (pattern > 0  &&  ShapeFillFlag == 1) {
            FillPattern = pattern;
            gtx_clipfillprim (xyt, npt, 0,
                              -1, -1, -1,
                              pred, pgreen, pblue,
                              bred, bgreen, bblue,
                              thick, 0, pattern, patscale, linepatt, dashscale,
                              xmin, ymin, xmax, ymax);
        }
        FillPattern = 0;
        ShapeFillFlag = 0;
        xyt += npt * 2;
    }

    csw_Free (xylocal);
    if (crad >= tiny) {
        csw_Free (x); /*lint !e424*/
        csw_Free (y); /*lint !e424*/
    }
    if (xyout) csw_Free (xyout);

    return 1;

}  /*  end of function gtx_cliprectprim  */



/*
  ****************************************************************

                  g t x _ c l i p a r c p r i m

  ****************************************************************

*/

int GTXDrawPrim::gtx_cliparcprim
                    (CSW_F xc, CSW_F yc, CSW_F r1, CSW_F r2, 
                     CSW_F ang1, CSW_F anglen, CSW_F rang, int fillflag,
                     int fred, int fgreen, int fblue,
                     int pred, int pgreen, int pblue,
                     int bred, int bgreen, int bblue,
                     CSW_F thick, int pattern, CSW_F patscale, int linepatt, 
                     CSW_F dashscale)
{
    CSW_F            tiny, x1, y1t, x2, y2, *x, *y, *xylocal, *xyout, *xyt;
    int              outline, npts, narc, istat;
    int              npout, icout[20], ido;

/*
    decide how many points for the arc at the current scale
*/
    double _scalx = scalx;
    if (_scalx < 0.0) _scalx = -_scalx;
    tiny = ARCRESOLUTION  / _scalx;
    npts = (int)((r1 + r2) * 3.14159f / tiny);
    npts += 10;
    if (npts < 20) npts = 20;

/*
    allocate work space for the arc points
*/
    x = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
    if (!x) {
        return -1;
    }
    y = x + npts;

    xylocal = (CSW_F *)csw_Malloc ((npts + 1) * 2 * sizeof(CSW_F));
    if (!xylocal) {
        csw_Free (x);
        return -1;
    }
    
/*
    calculate points for an arc centered at 0,0
*/
    istat = gpf_shape_obj.gpf_calcarcpoints2 (r1, r2, ang1, anglen,
                                npts, fillflag,
                                x, y, &narc);

    if (istat != 1) {
        csw_Free (x);
        csw_Free (xylocal);
        return -1;
    }

/*
    rotate and translate the points
*/
    gpf_shape_obj.gpf_rotatepoints (x, y, narc, rang);
    gpf_shape_obj.gpf_translatepoints (x, y, narc, xc, yc);
    gpf_packxy2 (x, y, narc, xylocal);

    xyout = NULL;
    xyt = xylocal;
    npout = 1;
    icout[0] = narc;
        
/*
    draw the points as a polygon
*/
    for (ido=0; ido<npout; ido++) {
        narc = icout[ido];
        outline = 2;
        if (fillflag == 1  ||  fillflag == 2  ||  
            anglen >= 360.  ||  anglen <= -360.) {
            outline = 1;
        }
        gpf_xylimits (xyt, narc * 2, &x1, &y1t, &x2, &y2);

        ply_patfill_obj.ply_SetPattClipSaveFlag (1);
        ShapeFillFlag = 1;
        FillPattern = 0;
        gtx_clipfillprim (xyt, narc, 0,
                          fred, fgreen, fblue,
                          pred, pgreen, pblue,
                          bred, bgreen, bblue,
                          thick, outline, 0, patscale, linepatt, dashscale,
                          x1, y1t, x2, y2);
        ply_patfill_obj.ply_SetPattClipSaveFlag (0);
        if (pattern > 0  &&  ShapeFillFlag == 1) {
            FillPattern = pattern;
            gtx_clipfillprim (xyt, narc, 0, 
                              -1, -1, -1,
                              pred, pgreen, pblue,
                              bred, bgreen, bblue,
                              thick, 0, pattern, patscale, linepatt, dashscale,
                              x1, y1t, x2, y2);
        }
        FillPattern = 0;
        ShapeFillFlag = 0;
        xyt += narc * 2;
    }

    csw_Free (xylocal);
    csw_Free (x);
    if (xyout) csw_Free (xyout);

    return 1;

}  /*  end of function gtx_cliparcprim  */




/*
  ****************************************************************

                         A r r o w C a l c

  ****************************************************************

    Calculate the points for an arrow tip and return them packed
  in the x array.

*/

int GTXDrawPrim::ArrowCalc (CSW_F x1, CSW_F y1t, 
                                   CSW_F x2, CSW_F y2, CSW_F size,
                                   int style, CSW_F *x)
{
    double         angle, dx, dy, ang1, ang2;
    int            i;

    dx = x2 - x1;
    dy = y2 - y1t;

    if (dx == 0.0  &&  dy == 0.0) {
        angle = 0.0;
    }
    else {
        angle = atan2 (dy, dx);
    }

    ang1 = angle + 2.617993;
    ang2 = angle + 3.665191;

    x[0] = (CSW_F)(size * cos(ang1) + x2);
    x[1] = (CSW_F)(size * sin(ang1) + y2);
    x[2] = x2;
    x[3] = y2;
    x[4] = (CSW_F)(size * cos(ang2) + x2);
    x[5] = (CSW_F)(size * sin(ang2) + y2);
    if (style > 1) {
        x[6] = x[0];
        x[7] = x[1];
    }

    if (style > 3) {
        dx = size *.866 * cos(angle);
        dy = size *.866 * sin(angle);
        for (i=0; i<8; i++) {
            if (i % 2 == 0) x[i] += (CSW_F)dx;
            if (i % 2 == 1) x[i] += (CSW_F)dy;
        }
    }

    return 1;

}  /*  end of private ArrowCalc function  */






/*
******************************************************************

            g t x _ c l i p _ i m a g e _ p r i m

******************************************************************

*/

int GTXDrawPrim::gtx_clip_image_prim (CSW_F xmin,
                         CSW_F ymin,
                         CSW_F xmax,
                         CSW_F ymax,
                         int ncol,
                         int nrow,
                         unsigned char *red_data,
                         unsigned char *green_data,
                         unsigned char *blue_data,
                         unsigned char *trans_data)
{
    CSW_F       scaled_x1, scaled_y1, scaled_x2, scaled_y2;

    if (red_data == NULL  ||  green_data == NULL  ||  blue_data == NULL  ||
        trans_data == NULL  ||  ncol < 2  ||  nrow < 2) {
        return 0;
    }

    if (xmin >= xmax  ||  ymin >= ymax) {
        return 0;
    }

/*  if both corners are gt max or lt min of window, do not plot  */

    if (xmax < fxmin  ||  xmin > fxmax) {
        return 0;
    }
    if (ymax < fymin  ||  ymin > fymax) {
        return 0;
    }

    gtx_scalef (xmin, ymin, &scaled_x1, &scaled_y1);
    gtx_scalef (xmax, ymax, &scaled_x2, &scaled_y2);
    ezx_java_ptr->ezx_AppendImageToJavaArea (scaled_x1, scaled_y1, scaled_x2, scaled_y2,
                               ncol, nrow,
                               red_data, green_data, blue_data, trans_data,
                               ImageHasLines, ImageID, SelectedFlag);

    return 0;

}  /*  end of function gtx_clip_image_prim  */


void GTXDrawPrim::gtx_set_selected_flag_for_drawing (int ival)
{
    SelectedFlag = ival;
    return;
}

/*
******************************************************************

          g t x _ c l i p _ f r a m e _ b o r d e r

******************************************************************

*/

int GTXDrawPrim::gtx_clip_frame_border (int frame_num,
                           FRameStruct *frptr)

{
    CSW_F       px1, py1, px2, py2, fx1, fy1, fx2, fy2;
    CSW_F       sx1, sy1, sx2, sy2;
    int         borderflag, clipflag, scale_to_attach_frame;

    px1 = frptr->px1;
    py1 = frptr->py1;
    px2 = frptr->px2;
    py2 = frptr->py2;
    fx1 = (CSW_F)frptr->x1;
    fy1 = (CSW_F)frptr->y1;
    fx2 = (CSW_F)frptr->x2;
    fy2 = (CSW_F)frptr->y2;
    borderflag = frptr->borderflag;
    clipflag = frptr->clipflag;

/*
 * Convert to screen units.
 */
    gtx_scalef (px1, py1, &sx1, &sy1);        
    gtx_scalef (px2, py2, &sx2, &sy2);        

/*
 * Send to java side for frame definition there.
 */
    scale_to_attach_frame = 0;
    if (frptr->scale_width_to_attach_frame != 0  ||
        frptr->scale_height_to_attach_frame != 0) {
        scale_to_attach_frame = 1;
    }

    ezx_java_ptr->ezx_AppendFrameToJavaArea (sx1, sy1, sx2, sy2,
                               fx1, fy1, fx2, fy2,
                               borderflag, clipflag,
                               frptr->rescaleable,
                               scale_to_attach_frame,
                               frame_num,
                               frptr->name);

    return 1;

}

/*
******************************************************************

                  g t x _ c l i p t e x t r e c t

******************************************************************

  function name:    gtx_cliptextrect      (int)

  call sequence:    gtx_cliptextrect (x, y, text, anchor, thick,
                                      red, green, blue,
                                      fred, fgreen, fblue,
                                      angle, size, font, flag)

  purpose:          clip a rectangle surrounding a single text string
                    to the current window

  return value:     always returns zero

  calling parameters:

    x         r    CSW_F        x coordinate of lower left corner of text
    y         r    CSW_F        y coordinate of lower left
    text      r    char*        text string to plot
    anchor    r    int          anchor for text
    thick     r    CSW_F        thickness of text lines in plot inches
    red       r    int          red value for text "stroke" color
    green     r    int          green value for "stroke"
    blue      r    int          blue value for "stroke"
    fred      r    int          red value for text fill color
    fgreen    r    int          green value for fill
    fblue     r    int          blue value for fill
    angle     r    CSW_F        text angle in radians
    size      r    CSW_F        text height in inches
    font      r    int          text font number
    bgflag    r    int          type of rectangle flag

*/

int GTXDrawPrim::gtx_cliptextrect (CSW_F xin, CSW_F yin, char *text, int anchor, 
                      CSW_F thick, int red, int green, int blue,
                      int fred, int fgreen, int fblue,
                      CSW_F angle, CSW_F size, int font, int bgflag)

{
    CSW_F       x, y, tlen, xc, yc, wide, high, crad;
    CSW_F       cx[4], cy[4], dx, dy;
    int         nc, bred, bgreen, bblue;

/*
 * convert size to be correct on the device.
 */
    gtx_scale_size (&size);

    nc = strlen (text);
    if (nc <= 0) {
        return 0;
    }

/*
 * Setup box clip parameters.
 */
    tlen = exact_text_length (text, size, font);

    x = xin;
    y = yin;
    gpf_calcdraw_obj.gpf_find_box_corners (x, y, anchor,
                          tlen, size, angle,
                          cx, cy);

/*
 * correct for descenders (i.e. jg) in fonts
 */
    dx = cx[3] - cx[0];
    dy = cy[3] - cy[0];
    dx *= 0.3f;
    dy *= 0.3f;
    cx[0] -= dx;
    cy[0] -= dy;
    dx = cx[2] - cx[1];
    dy = cy[2] - cy[1];
    dx *= 0.3f;
    dy *= 0.3f;
    cx[1] -= dx;
    cy[1] -= dy;

    size *= 1.3f;

    xc = (cx[0] + cx[2]) / 2.0f;
    yc = (cy[0] + cy[2]) / 2.0f;

    wide = tlen + size * .15f;
    high = size * 1.35f;
    crad = 0.0f;
    bred = red;
    bgreen = green;
    bblue = blue;
    if (bgflag == 3  ||  bgflag == 4) {
        crad = .2f * high;
        wide += 2.0f * crad;
        high += 2.0f * crad;
    }
    if (bgflag == 1  ||  bgflag == 3) {
        bred = -1;
        bgreen = -1;
        bblue = -1;
    }

/*
 * clip as a centered box.
 */
    TextPolyFlag = 1;
    gtx_cliprectprim (xc, yc, wide, high, 
                      angle, crad,
                      fred, fgreen, fblue,
                      -1, -1, -1,  /* no pattern fill */
                      bred, bgreen, bblue,
                      thick, 0, 1.0f,
                      0, 1.0f);
    TextPolyFlag = 0;

    return 0;

}  /*  end of function gtx_cliptextrect  */



/*
******************************************************************

             e x a c t _ t e x t _ l e n g t h

******************************************************************

*/
CSW_F GTXDrawPrim::exact_text_length (char *text, CSW_F size, int font)

{
    int      nc;
    CSW_F    tlen, tsize;
    CSW_F    bounds[10];

    if (ExactTextLength >= 0.0) {
        return ExactTextLength;
    }

    nc = strlen (text);
    if (nc < 1) {
        ExactTextLength = 0.0f;
        return 0.0f;
    }

/*
 * get either stroke or fill length
 */
    if (font < 8  ||  (font > 100  &&  font < 200)) {
        tlen = stroke_text_length (text, size, font);
    }
    else {

    /*
     * Convert to inches and get the bounds of the text.
     */
        tsize = size / PageUnitsPerInch;
        gtx_GetTextBounds (
            text,
            font,
            tsize,
            bounds);
        tlen = bounds[0] * PageUnitsPerInch;
    }

    ExactTextLength = tlen;

    return tlen;

}  /*  end of function exact_text_length  */


void GTXDrawPrim::gtx_reset_exact_text_length (void)
{
    ExactTextLength = -1.0f;
}





CSW_F GTXDrawPrim::stroke_text_length (char *text, CSW_F size, int font)

{
    CSW_F    xt, textlen, charwidth;
    int      i, j, n, nparts, nc, istat;
    int      parts[MAXPARTS];

    gpf_font_obj.gpf_setfont (font);

/*  loop through text, getting bounds for each character  */

    textlen = 0.0;
    nc = strlen (text);

    if (nc < 1) {
        return textlen;
    }

    for (i=0; i<nc; i++) {

        if (isspace(text[i])) {
            gpf_font_obj.gpf_CharWidth (text[i], size, &charwidth);
            textlen += charwidth;
            continue;
        }
        
        istat = gpf_font_obj.gpf_getfontstrokes (text[i], size, Fontx, Fonty, parts, &nparts, 
                                    MAXFONT, MAXPARTS);
        if (!istat) {
            continue;
        }

        n = 0;
        for (j=0; j<nparts; j++) {
            n += parts[j];
        }

        xt = 0.0f;
        for (j=0; j<n; j++) {
            if (Fontx[j] > xt) xt = Fontx[j];
        }

        if (i < nc-1) {
            gpf_font_obj.gpf_CharWidth (text[i], size, &charwidth);
            textlen += charwidth;
        }
        else {
            textlen += xt;
        }

    }

    return textlen;

}  /*  end of private stroke_text_length function  */


void GTXDrawPrim::gtx_set_poly_alpha (int afill, int apat, int abord)
{
    FillAlpha = afill;
    PatternAlpha = apat;
    BorderAlpha = abord;
}

/*
******************************************************************

       g t x _ S e n d _ B a c k _ S y m b o l _ P a r t s

******************************************************************

  Return the symbol parts to the java side without drawing them.
  The location is always 0, 0.  This is used to get symbol parts
  for display in the graphic properties dialogs.

*/

int GTXDrawPrim::gtx_SendBackSymbolParts (int symb,
                             double dsize,
                             double dangle,
                             int dpi)

{

    static const  SYmVecList    *veclist[MAXSYMITEM];
    static const  SYmText       *textlist[MAXSYMITEM];
    static const  SYmCircle     *circlelist[MAXSYMITEM];
    CSW_F                       xylocal[MAXVECTORXY];

    int         istat, ifill, i, nc, font, 
                npts, ncircle, nvec, ntext;
    CSW_F       x1, y1t, x2, y2, ang1, ang2, xt, yt, tsize;
    CSW_F       r1, r2, size, angle, fdpi;
    int         dflagsav;

    gtx_InitDrawPrim ();

    size = (CSW_F)dsize;
    angle = (CSW_F)dangle;
    fdpi = (CSW_F)dpi;

    gtx_init_drawing (
        -size, -size, 2.0f * size, 2.0f * size,
        -size, -size, 2.0f * size, 2.0f * size,
        -fdpi * size, -fdpi * size, 2.0f * fdpi * size, 2.0f * fdpi * size,
        dpi, 1, NULL);

    dflagsav = DashFlag;

    symb--;
    gpf_symbol_obj.gpf_setsymbolparms (symb, 0.0f, 0.0f, size, angle);

/*  get the pieces for the symbol  */

    istat = gpf_symbol_obj.gpf_getsymbolvec (symb, veclist, &nvec);
    if (!istat) {
        DashFlag = dflagsav;
        return 0;
    }

    istat = gpf_symbol_obj.gpf_getsymbolcircle (symb, circlelist, &ncircle);
    if (!istat) {
        DashFlag = dflagsav;
        return 0;
    }

    istat = gpf_symbol_obj.gpf_getsymboltext (symb, textlist, &ntext);
    if (!istat) {
        DashFlag = dflagsav;
        return 0;
    }

    EZXJavaArea    exa;
    EZXJavaArea    *exp = NULL;
    EZXJavaArea    *expsav = ezx_java_ptr;

    exp = ezx_java_ptr;
    if (exp == NULL) {
        exp = &exa;
        ezx_java_ptr = exp;
    }

    exp->ezx_SetStaticSymbolFlag (1);

/*  send back unfilled circles  */

    for (i=0; i<ncircle; i++) {
        ifill = circlelist[i]->ifill;
        if (ifill > 1) ifill -= 10;
        if (ifill == 0) {
            xt = circlelist[i]->x;
            yt = circlelist[i]->y;
            r1 = circlelist[i]->r1;
            r2 = circlelist[i]->r2;
            ang1 = circlelist[i]->ang1;
            ang2 = circlelist[i]->ang2;

            gpf_symbol_obj.gpf_symcircletrans (&xt, &yt, &r1, &r2, &ang1);
            gtx_clipcircle (xt, yt, r1, r2, ang1, ang2, 2,
                            0, 0, 0, .01f, angle);
        }
    }

/*  send back polygonal fill areas  */

    for (i=0; i<nvec; i++) {
        ifill = veclist[i]->fill;
        if (ifill > 1) ifill -= 10;
        if (ifill) {
            istat = gpf_symbol_obj.gpf_getsymbolxy (veclist[i], xylocal, &npts);
            if (!istat  ||  npts < 2) {
                continue;
            }
            gpf_symbol_obj.gpf_symlinetrans (xylocal, npts);
            gpf_xylimits (xylocal, npts, &x1, &y1t, &x2, &y2);
            gtx_clipfillprim (xylocal, npts/2, 0, 
                              0, 0, 0,
                              0, 0, 0,
                              0, 0, 0,
                              0.005f, 0, 0, 1.0f, 0, 1.0f,
                              x1, y1t, x2, y2);
        }
    }

/*  send back filled circles  */

    for (i=0; i<ncircle; i++) {
        ifill = circlelist[i]->ifill;
        if (ifill > 1) ifill -= 10;
        if (ifill) {
            xt = circlelist[i]->x;
            yt = circlelist[i]->y;
            r1 = circlelist[i]->r1;
            r2 = circlelist[i]->r2;
            ang1 = circlelist[i]->ang1;
            ang2 = circlelist[i]->ang2;

            gpf_symbol_obj.gpf_symcircletrans (&xt, &yt, &r1, &r2, &ang1);
            gtx_clipfillcircle (xt, yt, r1, r2, ang1, ang2, 2,
                                0, 0, 0, angle);
        }
    }

/*  send back lines  */

    for (i=0; i<nvec; i++) {
        ifill = veclist[i]->fill;
        if (ifill > 1) ifill -= 10;
        if (!ifill) {
            istat = gpf_symbol_obj.gpf_getsymbolxy (veclist[i], xylocal, &npts);
            if (!istat || npts<2) {
                continue;
            }
            gpf_symbol_obj.gpf_symlinetrans (xylocal, npts);
            gtx_cliplineprim (xylocal, npts/2, 0, .01f, 
                              0, 0, 0, 0, (CSW_F)1.0, 0);
        }
    }

/*  send back text  */

    TextFromSymbol = 1;
    for (i=0; i<ntext; i++) {
        xt = textlist[i]->x;
        yt = textlist[i]->y;
        tsize = textlist[i]->size;
        ang1 = textlist[i]->angle;
        nc = textlist[i]->nc;
        font = textlist[i]->font;

        gpf_symbol_obj.gpf_symtexttrans (&xt, &yt, &tsize, &ang1);
        gtx_cliptextprim (xt, yt,
                          const_cast <char *> (textlist[i]->text),
                          nc, .01f, 
                          0, 0, 0, 0, 0, 0,
                          ang1, tsize, font);
    }
    TextFromSymbol = 0;

    DashFlag = dflagsav;

    exp->ezx_SetStaticSymbolFlag (0);

    ezx_java_ptr = expsav;

    return 1;
    
}  /*  end of function gtx_get_symbol_parts  */
