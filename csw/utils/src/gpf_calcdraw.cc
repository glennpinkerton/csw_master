
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_calcdraw.cc

    Implementation of the GPFCalcdraw methods.

    This file has generic functions to assist the drawing process.
    No specific drawing device is supported by these functions.
    The functions generally break a complex object, such as a polygon,
    a polyline, or an arc into very simple objects which can be
    passed to the device by the calling function.
*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/include/csw_errnum.h"

#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_calc.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/gpf_font.h"


#include "csw/utils/private_include/gpf_calcdraw.h"


/*  define some macros for the file  */



/*
  ****************************************************************

             g p f _ S e t C l i p W i n d o w

  ****************************************************************

*/

int GPFCalcdraw::gpf_SetClipWindow (CSW_F x1_in, CSW_F y1_in, CSW_F x2_in, CSW_F y2_in)
{
    CSW_F     x1, y1, x2, y2;

    if (x1_in < x2_in) {
        x1 = x1_in;
        x2 = x2_in;
    }
    else {
        x1 = x2_in;
        x2 = x1_in;
    }
    if (y1_in < y2_in) {
        y1 = y1_in;
        y2 = y2_in;
    }
    else {
        y1 = y2_in;
        y2 = y1_in;
    }

    fxmin = x1;
    fymin = y1;
    fxmax = x2;
    fymax = y2;
    return 1;

}  /*  end of function gpf_SetClipWindow  */



/*
******************************************************************

                 g p f _ t r a n s x y l i n e

******************************************************************

  function name:    gpf_transxyline      (int)

  call sequence:    gpf_transxyline (x, y, angle, xy, npts)

  purpose:          Translate lines and fill boundaries by rotating
                    and then translating by x,y.  This function is
                    used when a polyline aproximation of a curve is
                    needed.

  return value:     always returns zero

  calling parameters:

    x        r     CSW_F     plot x coordinate of xy origin
    y        r     CSW_F     plot y coordinate of xy origin
    angle    r     CSW_F     rotation angle in radians
    xy       r/w   CSW_F*    array of x and y coordinates
    npts     r     int       number of points in x and y

*/

int GPFCalcdraw::gpf_transxyline (CSW_F x, CSW_F y, CSW_F angle, CSW_F *xy, int npts)
{
    int       i, j;
    CSW_F     xt;
    double    dang, cosang, sinang;


    dang = (double)angle;
    sinang = sin(dang);
    cosang = cos(dang);

    i = 0;
    while (i < npts) {
        j = i+1;
        xt = (CSW_F)(xy[i]*cosang - xy[j]*sinang);
        xy[j] = (CSW_F)(xy[i]*sinang + xy[j]*cosang);
        xy[i] = xt;
        xy[i] += x;
        xy[j] += y;

        i += 2;
    }

    return 0;

}  /*  end of function gpf_transxyline  */





/*
******************************************************************

                     g p f _ p c l i p r e c t

******************************************************************

  function name:    gpf_pcliprect      (int)

  call sequence:    gpf_pcliprect (x1, y1, x2, y2,
                                   xy, npts, holflg,
                                   maxout, maxcout,
                                   xyout, npout, nhout, icout)

  purpose:          clip a polygon to a rectangular region.
                    The polygon is passed to the function as
                    an array of x,y x,y points.  Polygon holes
                    are flagged by an x value greater than holflg.
                    Holflg must be greater than 1.e15 or less than
                    -1.e15.  If holflg does not meet these conditions,
                    holflg is set to 1.e29 internally.

  return value:     status code

                    -1 = memory allocation error
                     1 = normal successful completion
                     2 = output arrays will overflow the space available

  calling parameters:

    x1         r    CSW_F        Minimum x of clip rectangle
    y1         r    CSW_F        Minimum y of clip rectangle
    x2         r    CSW_F        Maximum x of clip rectangle
    y2         r    CSW_F        Maximum y of clip rectangle
    xy         r    CSW_F*       Array of x,y values for polygon,
                                 with hole flags imbedded if needed
    npts       r    int          Number of polygon points, including hole flags
    holflg     r    CSW_F        Flag for start of hole.  If x is greater than
                                 holflg, a hole starts with the next point
    maxout     r    int          Maximum number of output points allowed
    maxcout    r    int          Maximum number of output components allowed
    xyout      w    CSW_F*       Output polygon x,y points, without hole flags
    npout      w    int*         Number of distinct polygons output.
    nhout      w    int*         Array with number of components for each distinct polygon
    icout      w    int*         Array with number of points per output component

*/

int GPFCalcdraw::gpf_pcliprect
                  (CSW_F x1_in, CSW_F y1_in, CSW_F x2_in, CSW_F y2_in,
                   CSW_F *xy, int npts, CSW_F holflg,
                   int maxout, int maxcout,
                   CSW_F *xyout, int *npout, int *nhout, int *icout)
{
    CSW_F       x1, y1, x2, y2;
    CSW_F       *xyw = NULL, **xywp = NULL, *xysav = NULL, *xysav2 = NULL;
    double      xt, *xw = NULL, *yw = NULL, *xw2 = NULL, *yw2 = NULL,
                    *xwhn = NULL, *ywhn = NULL;
    int         nflags, i, j, k, n, n2, ncomp, nptmp, npts2, istat, nptstmp,
                *nhsav = NULL, *icsav = NULL, *icomp = NULL, *icomp2 = NULL,
                *iholes = NULL, nestcomp, *ihnest = NULL, *icnest = NULL;
    CSW_F       xx1, yy1, xx2, yy2;

    CSW_F       *xyw_orig = NULL, **xywp_orig = NULL;
    int         *icomp_orig = NULL, *icomp2_orig = NULL,
                *iholes_orig = NULL;
    double      *xw_orig = NULL, *yw_orig = NULL,
                *xw2_orig = NULL, *yw2_orig = NULL;

    CSWPolyUtils  ply_utils_obj;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
        if (xyw_orig != XYwork) csw_Free (xyw_orig);
        if (xywp_orig != XYworkp) csw_Free (xywp_orig);
        if (icomp_orig != Iwork1) csw_Free (icomp_orig);
        if (icomp2_orig != Iwork2) csw_Free (icomp2_orig);
        if (iholes_orig != Iwork3) csw_Free (iholes_orig);
        if (xw_orig != DXwork) csw_Free (xw_orig);
        if (yw_orig != DYwork) csw_Free (yw_orig);
        if (xw2_orig != DXwork2) csw_Free (xw2_orig);
        if (yw2_orig != DYwork2) csw_Free (yw2_orig);
    };
    CSWScopeGuard  func_scope_guard (fscope);


    if (x1_in < x2_in) {
        x1 = x1_in;
        x2 = x2_in;
    }
    else {
        x1 = x2_in;
        x2 = x1_in;
    }
    if (y1_in < y2_in) {
        y1 = y1_in;
        y2 = y2_in;
    }
    else {
        y1 = y2_in;
        y2 = y1_in;
    }

    *npout = 0;

/*  check if entire polygon is outside the clip area  */

    gpf_xylimits2 (xy, npts*2, &xx1, &yy1, &xx2, &yy2);
    if (xx1 >= x2  ||  xx2 <= x1  ||  yy1 >= y2  ||  yy2 <= y1) {
        return 1;
    }

/*  adjust hole flag to positive  */

    if (holflg > -1.e15f  &&  holflg < 1.e15f) {
        holflg = 1.e29f;
    }
    if (holflg < 0.0f) {
        holflg = -holflg;
    }

/*  assign pointers for work arrays, either by allocating
    memory or using file work arrays if possible  */

    //if (npts*2 >= MAXWORKSIZE2) {
    if (npts*2 >= 0) {
        xyw = (CSW_F *)csw_Malloc (npts*2*sizeof(CSW_F));
        if (!xyw) {
            return -1;
        }
    }
    else {
        xyw = XYwork;
    }
    xyw_orig = xyw;

/*  count the input components  */

    nflags = 1;
    i = 0;
    while (i<npts) {
        if (xy[i] >= holflg  ||  xy[i] <= -holflg) {
            nflags++;
        }
        i += 2;
    }
    nflags++;

/*  allocate memory for component workspace  */

    //if (nflags >= MAXPOLYCOMP) {
    if (nflags >= 0) {
        xywp = (CSW_F**)csw_Malloc (nflags*sizeof(CSW_F*));
        if (!xywp) {
            return -1;
        }
        icomp = (int*)csw_Malloc (nflags * sizeof(int));
        if (!icomp) {
            return -1;
        }
        icomp2 = (int*)csw_Malloc (nflags * sizeof(int));
        if (!icomp2) {
            return -1;
        }
        iholes = (int*)csw_Malloc (nflags * sizeof(int));
        if (!iholes) {
            return -1;
        }
    }
    else {
        xywp = XYworkp;
        nflags = MAXPOLYCOMP - 1;
        icomp = Iwork1;
        icomp2 = Iwork2;
        iholes = Iwork3;
    }

    xywp_orig = xywp;
    icomp_orig = icomp;
    icomp2_orig = icomp2;
    iholes_orig = iholes;

/*  separate holes into separate components  */

    istat = gpf_polyholesep (xy, npts, holflg, nflags,
                             xywp, &ncomp, icomp);
    if (istat != 1) {
        return istat;
    }

    gpf_removepolyholeflags (xywp, ncomp, icomp, xyw);

/*  allocate memory for hole nest check and correction  */

    npts2 = 0;
    for (i=0; i<ncomp; i++) {
        npts2 += icomp[i];
    }

    //if (npts2 >= MAXWORKSIZE) {
    if (npts2 >= 0) {
        xw = (double *)csw_Malloc (npts2 * sizeof(double));
        if (!xw) {
            return -1;
        }
        yw = (double *)csw_Malloc (npts2 * sizeof(double));
        if (!yw) {
            return -1;
        }
        xw2 = (double *)csw_Malloc (npts2 * sizeof(double));
        if (!xw2) {
            return -1;
        }
        yw2 = (double *)csw_Malloc (npts2 * sizeof(double));
        if (!yw2) {
            return -1;
        }
    }
    else {
        xw = DXwork;
        yw = DYwork;
        xw2 = DXwork2;
        yw2 = DYwork2;
    }

    xw_orig = xw;
    yw_orig = yw;
    xw2_orig = xw2;
    yw2_orig = yw2;

/*  unnest multiply nested holes  */

    n = 0;
    xysav = xyw;
    for (i=0; i<npts2; i++) {
        xw[i] = *xyw;
        xyw++;
        yw[i] = *xyw;
        xyw++;
    }

    xt = (double)maxout;
    ply_utils_obj.ply_parms ('w', "maxpout", &xt);
    xt = (double)maxcout;
    ply_utils_obj.ply_parms ('w', "maxhout", &xt);

    istat = ply_utils_obj.ply_holnest (xw, yw, ncomp, icomp,
                         xw2, yw2, &nptmp, iholes, icomp2);
    if (istat == -1) {
        return -1;
    }
    if (istat == 1) {
        return 2;
    }
    if (istat == 0) {
        xwhn = xw2;
        ywhn = yw2;
        nestcomp = nptmp;
        ihnest = iholes;
        icnest = icomp2;
    }
    else {
        xwhn = xw;
        ywhn = yw;
        nestcomp = 1;
        ihnest = iholes;
        iholes[0] = ncomp;
        icnest = icomp;
    }

/*  pack back into xyw array and clip each individual polygon  */

    n = 0;
    xysav = xyw;
    xysav2 = xyout;
    icsav = icout;
    nhsav = nhout;
    for (i=0; i<nestcomp; i++) {
        n2 = n;
        for (j=0; j<ihnest[i]; j++) {
            for (k=0; k<icnest[n]; k++) {
                *xyw = (CSW_F)*xwhn;
                xyw++;
                *xyw = (CSW_F)*ywhn;
                xyw++;
                xwhn++;
                ywhn++;
            }
            n++;
        }

        istat = gpf_polycliprect2 (x1, y1, x2, y2,
                                   xysav, ihnest[i], icnest+n2,
                                   maxout, maxcout,
                                   xyout, &npts2, nhout, icout, &nptstmp);
        if (istat < 1) {
            return istat;
        }
        if (istat == 3) {
            *npout = 0;
            return 1;
        }
        if (istat == 1) {
            xyout += nptstmp;
            icout += *nhout;
            *npout += npts2;
            nhout += npts2;
            maxout -= nptstmp;
            maxcout -= *nhout;
        }

        xyw = xysav;
    }

    xyout = xysav2;
    icout = icsav;
    nhout = nhsav;

    return 1;

}  /*  end of function gpf_pcliprect  */



/*
******************************************************************

                g p f _ p o l y c l i p r e c t 2

******************************************************************

  function name:    gpf_polycliprect2      (int)

  call sequence:    gpf_polycliprect2 (x1, y1, x2, y2,
                                       xypoly, ncomp, icomp, maxout, maxcout,
                                       xypout, npout, nhout, ipout, nptsout)

  purpose:          Clip a complex non self intersecting polygon
                    to a rectangular clip window.  This function
                    is only called from gpf_pcliprect.  The calling
                    function is responsible for formatting the polygon
                    appropriately for this function.

  return value:     status code

                    -1 = memory allocation error
                     0 = insufficient output polygon space
                     1 = normal successful completion
                     2 = could not orient the main input component
                     3 = polygon does not clip the rectangle

  calling parameters:

    x1        r    CSW_F        Minimum x of clip region
    y1        r    CSW_F        Minimum y of clip region
    x2        r    CSW_F        Maximum x of clip region
    y2        r    CSW_F        Maximum y of clip region
    xypoly    r    CSW_F*       Array of x,y coords of the polygon
    ncomp     r    int          number of components
    icomp     r    int*         Array with number of points per component
    maxout    r    int          Maximum number of points that can fit in the output
    maxcout   r    int          Maximum number of components that can fit in the output
    xypout    w    CSW_F*       Array for x,y components of output polygons
    npout     w    int*         number of components in the output
    nhout     w    int*         Array with number of holes per ouput component
    ipout     w    int*         Array with number of points per output hole.
    nptsout   w    int*         Number of points written to xypout

*/

int GPFCalcdraw::gpf_polycliprect2
                      (CSW_F x1_in, CSW_F y1_in, CSW_F x2_in, CSW_F y2_in,
                       CSW_F *xypoly, int ncomp, int *icomp, int maxout, int maxcout,
                       CSW_F *xypout, int *npout, int *nhout, int *ipout, int *nptsout)
{
    CSW_F             x1, y1, x2, y2;
    double            xclip[5], yclip[5], *xpolys= NULL, *ypolys = NULL,
                        *dxpout = NULL, *dypout = NULL;
    int               nclip, iclip[1];
    int               i, j, istat, n, k, n2;
    CSW_F             xt1, yt1, xt2, yt2, *xysav;

    CSWErrNum         err_obj;
    CSWPolyCalc       ply_calc_obj;


// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
        csw_Free (xpolys);
        csw_Free (ypolys);
        csw_Free (dxpout);
        csw_Free (dypout);
    };
    CSWScopeGuard  func_scope_guard (fscope);


    if (x1_in < x2_in) {
        x1 = x1_in;
        x2 = x2_in;
    }
    else {
        x1 = x2_in;
        x2 = x1_in;
    }
    if (y1_in < y2_in) {
        y1 = y1_in;
        y2 = y2_in;
    }
    else {
        y1 = y2_in;
        y2 = y1_in;
    }

/*  if the bounding box is completely inside the clip rectangle,
    copy input to output and return  */

    n = 0;
    for (i=0; i<ncomp; i++) {
        n += icomp[i];
    }

    gpf_xylimits (xypoly, n*2, &xt1, &yt1, &xt2, &yt2);

    if (xt1 >= x1  &&  xt1 <= x2  &&  xt2 >= x1  &&  xt2 <= x2  &&
        yt1 >= y1  &&  yt1 <= y2  &&  yt2 >= y1  &&  yt2 <= y2) {

        for (i=0; i<n*2; i++) {
            xypout[i] = xypoly[i];
        }
        *npout = 1;
        nhout[0] = ncomp;
        for (i=0; i<ncomp; i++) {
            ipout[i] = icomp[i];
        }

        return 1;
    }

/*  allocate memory for all the double arrays  */

/*  x, y point arrays  */

    n = 0;
    for (i=0; i<ncomp; i++) {
        n += icomp[i];
    }
    //if (n >= MAXWORKSIZE) {
    if (n >= 0) {
        xpolys = (double *)csw_Malloc (n * sizeof(double));
        if (!xpolys) {
            return -1;
        }
        ypolys = (double *)csw_Malloc (n * sizeof(double));
        if (!ypolys) {
            return -1;
        }
    }
    else {
        xpolys = DXwork;
        ypolys = DYwork;
    }

/*  output arrays  */

    //if (maxout >= MAXWORKSIZE) {
    if (maxout >= 0) {
        dxpout = (double *)csw_Malloc (maxout * sizeof(double));
        if (!dxpout) {
            return -1;
        }
        dypout = (double *)csw_Malloc (maxout * sizeof(double));
        if (!dypout) {
            return -1;
        }
    }
    else {
        dxpout = DXwork2;
        dypout = DYwork2;
    }

/*  copy the polygon into double arrays  */

    n = 0;
    xysav = xypoly;
    for (i=0; i<ncomp; i++) {
        for (j=0; j<icomp[i]; j++) {
            xpolys[n] = *xypoly;
            xypoly++;
            ypolys[n] = *xypoly;
            xypoly++;
            n++;
        }
    }

    xypoly = xysav;

/*  copy limits into clip array  */

    xclip[0] = x1;
    yclip[0] = y1;
    xclip[1] = x1;
    yclip[1] = y2;
    xclip[2] = x2;
    yclip[2] = y2;
    xclip[3] = x2;
    yclip[3] = y1;
    xclip[4] = x1;
    yclip[4] = y1;
    nclip = 1;
    iclip[0] = 5;

/*
    calculate the intersection
*/

    istat = ply_calc_obj.ply_CalcIntersect (err_obj,
                               xpolys, ypolys, icomp, ncomp,
                               xclip, yclip, iclip, nclip,
                               dxpout, dypout, npout, nhout, ipout,
                               maxcout, maxcout, maxout);
    if (istat != 1) {
        istat = err_obj.csw_ErrNum ();
        if (istat == 1) {
            return -1;
        }
        if (istat == 2) {
            return 0;
        }
        if (istat == 3) {
            return 2;
        }
        return 3;
    }


/*  put results back into CSW_F arrays  */

    n2 = 0;
    n = 0;
    for (i=0; i<*npout; i++) {
        for (j=0; j<nhout[i]; j++) {
            for (k=0; k<ipout[n]; k++) {
                *xypout = (CSW_F)*dxpout;
                xypout++;
                *xypout = (CSW_F)*dypout;
                xypout++;
                dxpout++;
                dypout++;
                n2 += 2;
            }
            n++;
        }
    }

    *nptsout = n2;
    return 1;

}  /*  end of function gpf_polycliprect2 */






/*
******************************************************************

                   g p f _ p o l y h o l e s e p

******************************************************************

  function name:    gpf_polyholesep

  call sequence:    gpf_polyholesep (xy, npts, holflg, maxout,
                                     xyout, nholes, iholes)

  purpose:          separate polygon holes and return pointers to them
                    The holes each must start with holflg as the x coordinate
                    (except for the first point in xy).

  return value:     status code

                    -1 = maxout is too small for the number of holes
                     1 = normal successful completion

  calling parameters:

    xy          r    CSW_F*      Array of xy coordinates with hole flags embedded
    npts        r    int         number of polygon points, including hole flags
    holflg      r    CSW_F       Flag signaling start of hole
    maxout      r    int         Maximum number of holes allowed
    xyout       w    CSW_F**     array of pointers to start of holes
    nholes      w    int*        number of holes found
    iholes      w    int*        array with number of points in each hole

*/

int GPFCalcdraw::gpf_polyholesep (CSW_F *xy, int npts, CSW_F holflg, int maxout,
                     CSW_F **xyout, int *nholes, int *iholes)
{
    int          i, nh, last, ilast;
    CSW_F        xt;

/*  separate holes  */

    xyout[0] = xy;
    i = 0;
    nh = 0;
    ilast = 0;
    last = 0;

    while (i < npts*2) {
        xt = xy[i];
        if (xt > holflg  ||  xt < -holflg) {
            iholes[nh] = i/2 - ilast;
            if (iholes[nh] < 2) {
                i += 2;
                continue;
            }
            i += 2;
            ilast = i/2;
            nh++;
            if (nh > maxout) {
                return -1;
            }
            xyout[nh] = xy + i;
            last = 1;
        }
        else {
            i += 2;
            last = 0;
        }
    }

/*  last hole if needed  */

    if (!last) {
        iholes[nh] = npts - ilast;
        nh++;
    }

    *nholes = nh;

    return 1;

}  /*  end of function gpf_polyholesep  */





/*
******************************************************************

                g p f _ a d d h o l e f l a g s

******************************************************************

  function name:    gpf_addholeflags     (int)

  call sequence:    gpf_addholeflags (x, y, npts, ncomp,
                                      x1, y1, nptotal, memflg)

  purpose:          add hole flags prior to sending polygon coordinates
                    to the plot server.

  return value:     status code

                    -1 = memory allocation error
                     0 = ncomp is less than 1
                     1 = normal successful completion

  calling parameters:

    x        r    CSW_F*    array of x coordinates
    y        r    CSW_F*    array of y coordinates
    npts     r    int*      array with points per component
    ncomp    r    int       number of components
    x1       w    CSW_F**   x coordinates with hole flags
    y1       w    CSW_F**   y coordinates with hole flags
    nptotal  w    int*      total number of points in all components
    memflg   w    int*      flag for whether x1 and y1 need to be csw_Freed
                            0 = do not csw_Free after use
                            1 = csw_Free after use

*/

int GPFCalcdraw::gpf_addholeflags
                     (CSW_F *x, CSW_F *y, int *npts, int ncomp,
                      CSW_F **x1, CSW_F **y1, int *nptotal, int *memflg)
{
    int        i, j, imax, n;
    CSW_F      *xt = NULL, *yt = NULL;
    CSW_F      *xt_orig = NULL, *yt_orig = NULL;

    bool  bsuccess = false;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
        if (!bsuccess) {
            csw_Free (xt_orig);
            csw_Free (yt_orig);
            x1[0] = NULL;
            y1[0] = NULL;
            *nptotal = 0;
        }
    };
    CSWScopeGuard  func_scope_guard (fscope);


    if (ncomp < 1) {
        return -1;
    }

/*  allocate memory for output  */

    imax = 0;
    for (i=0; i<ncomp; i++) {
        imax += npts[i];
    }
    imax += ncomp;

    xt = (CSW_F *)csw_Malloc (imax * sizeof(CSW_F));
    if (!xt) {
        return -1;
    }
    yt = (CSW_F *)csw_Malloc (imax * sizeof(CSW_F));
    if (!yt) {
        return -1;
    }
    xt_orig = xt;
    yt_orig = yt;
    *memflg = 1;

/*  add hole flags  */

    n = 0;
    for (i=0; i<ncomp; i++) {
        if (i > 0) {
            *xt = HOLEFLAG;
            *yt = HOLEFLAG;
            xt++;
            yt++;
            n++;
        }
        for (j=0; j<npts[i]; j++) {
            *xt = *x;
            *yt = *y;
            xt++;
            yt++;
            x++;
            y++;
            n++;
        }
    }

    x1[0] = xt_orig;
    y1[0] = yt_orig;
    *nptotal = n;

    bsuccess = true;

    return 1;

}  /*  end of function gpf_addholeflags  */




/*
******************************************************************

                g p f _ a d d h o l e f l a g s 2

******************************************************************

  function name:    gpf_addholeflags2    (int)

  call sequence:    gpf_addholeflags2 (xy, npts, ncomp,
                                       xyout, nptotal)

  purpose:          add hole flags prior to sending coordinates
                    to the easyx server.  The coordinates are
                    passed in packed x,y format.

  return value:     status code

                     0 = ncomp is less than 1
                     1 = normal successful completion

  calling parameters:

    xy       r    CSW_F*    array of packed x,y coordinates
    npts     r    int*      array with points per component
    ncomp    r    int       number of components
    xyout    w    CSW_F*    x,y coordinates with hole flags
                            this must be 2 * (npts+ncomp) at least
    nptotal  w    int*      total number of points in all components

*/

int GPFCalcdraw::gpf_addholeflags2 (CSW_F *xy, int *npts, int ncomp,
                       CSW_F *xyout, int *nptotal)
{
    int        i, j, n;
    CSW_F      *xyt, *x;


    if (ncomp < 1) {
        return 0;
    }

    xyt = xyout;

/*  add hole flags  */

    x = xy;
    n = 0;
    for (i=0; i<ncomp; i++) {
        if (i > 0) {
            *xyt = HOLEFLAG;
            xyt++;
            *xyt = HOLEFLAG;
            xyt++;
            n++;
        }
        for (j=0; j<npts[i]; j++) {
            *xyt = *x;
            x++;
            xyt++;
            *xyt = *x;
            x++;
            xyt++;
            n++;
        }
    }

    *nptotal = n;
    return 1;

}  /*  end of function gpf_addholeflags2  */




/*
******************************************************************

            g p f _ r e m o v e p o l y h o l e f l a g s

******************************************************************

  function name:    gpf_removepolyholeflags        (int)

  call sequence:    gpf_removepolyholeflags (xyp, nh, icomp, xyout)

  purpose:          make a copy of the polygon coordinates that doesn't
                    include the polygon hole flags

  return value:     always returns 1

  calling parameters:

    xyp        r     CSW_F**    array of pointers to polygon components
    nh         r     int        number of pointers in xyp
    icomp      r     int*       array with number of points per component
    xyout      r     CSW_F*     array of compressed x,y values

*/

int GPFCalcdraw::gpf_removepolyholeflags (CSW_F **xyp, int nh, int *icomp, CSW_F *xyout)
{
    int          i, j;
    CSW_F        *tmp, *xt;


    xt = xyout;
    for (i=0; i<nh; i++) {
        tmp = xyp[i];
        for (j=0; j<icomp[i]*2; j++) {
            *xt = tmp[j];
            xt++;
        }
    }

    return 1;

}  /*  end of function gpf_removepolyholeflags  */




/*
******************************************************************

                  g p f _ c l i p l i n e p r i m

******************************************************************

  function name:    gpf_cliplineprim     (int)

  call sequence:    gpf_cliplineprim  (xyin, nin, x1in, y1in, x2in, y2in,
                                       xyout, ncout, icout)

  purpose:          clip a line primitive to the window defined by x1in
                    y1in, x2in, y2in.

  return value:     status code

                    -1 = memory allocation error
                     1 = normal successful completion

  calling parameters:

    xyin      r    CSW_F*    array of packed x,y  x,y coordinates
    nin       r    int       number of points in the xyin array
    x1in      r    CSW_F     minimum x of clip window
    y1in      r    CSW_F     minimum y of clip window
    x2in      r    CSW_F     maximum x of clip window
    y2in      r    CSW_F     maximum y of clip window
    xyout     w    CSW_F*    output x,y coordinate array
                             (this should be at least nin * 4 CSW_F words)
    ncout     w    int*      number of output components
    icout     w    int*      array with number of points per output component
                             (this should be at least nin * 2 int words)

*/

int GPFCalcdraw::gpf_cliplineprim (CSW_F *xyin, int nin,
                      CSW_F x1_in, CSW_F y1_in, CSW_F x2_in, CSW_F y2_in,
                      CSW_F *xyout, int *ncout, int *icout)
{
    CSW_F       x1in, y1in, x2in, y2in;
    int         i, n, in1, in2;
    CSW_F       *xyw = NULL, *xywsav = NULL,
                *xyw_orig = NULL,
                xt, yt, xt2, yt2, x1, y1, x, y;

    bool   need_free = false;

    auto fscope = [&]()
    {
      if (need_free) {
        csw_Free (xyw_orig);
      }
    };
    CSWScopeGuard  func_scope_guard (fscope);


    if (x1_in < x2_in) {
        x1in = x1_in;
        x2in = x2_in;
    }
    else {
        x1in = x2_in;
        x2in = x1_in;
    }
    if (y1_in < y2_in) {
        y1in = y1_in;
        y2in = y2_in;
    }
    else {
        y1in = y2_in;
        y2in = y1_in;
    }

    fxmin = x1in;
    fymin = y1in;
    fxmax = x2in;
    fymax = y2in;

    Tiny = (x2in-x1in+y2in-y1in) / 200000.f ;

    *ncout = 0;

    //if (nin*5 >= MAXWORKSIZE2) {
    if (nin*5 >= 0) {
        need_free = true;
        xyw = (CSW_F *)csw_Malloc (nin * 5 * sizeof (CSW_F));
        if (!xyw) {
            return -1;
        }
    }
    else {
        xyw = XYwork;
    }

    xyw_orig = xyw;

/*  check if the first point is inside or outside  */

    x = xyin[0];
    y = xyin[1];
    in1 = (x>fxmin && x<fxmax && y>fymin && y<fymax);

    xywsav = xyw;
    if (in1) {
        xyw[0] = x;
        xyw[1] = y;
        n = 1;
        xyw += 2;
    }
    else {
        n = 0;
    }

    x1 = x;
    y1 = y;

/*  loop through the coordinates, extracting pieces
    of the polyline that intersect the window, and
    output those pieces  */

    xyin += 2;
    for (i=1; i< nin; i++) {

        x = *xyin;
        xyin++;
        y = *xyin;
        xyin++;
        in2 = (x>fxmin && x<fxmax && y>fymin && y<fymax);

/*      previous point was inside  */

        if (in1) {
            if (in2) {
                *xyw = x;
                xyw++;
                *xyw = y;
                xyw++;
                x1 = x;
                y1 = y;
                n++;
            }
            else {
                gpf_clipvec1 (x1, y1, x, y, &xt, &yt);
                *xyw = xt;
                xyw++;
                *xyw = yt;
                n++;
                gpf_addlineclipcomp (xywsav, n, xyout, ncout, icout);
                n = 0;
                xyw = xywsav;
                in1 = 0;
                x1 = x;
                y1 = y;
            }
        }

/*      previous point was outside  */

        else {
            if (in2) {
                gpf_clipvec1 (x1, y1, x, y, &xt, &yt);
                *xyw = xt;
                xyw++;
                *xyw = yt;
                xyw++;
                *xyw = x;
                xyw++;
                *xyw = y;
                xyw++;
                n += 2;
                in1 = 1;
                x1 = x;
                y1 = y;
            }
            else {
                if (gpf_clipvec2(x1, y1, x, y, &xt, &yt, &xt2, &yt2)) {
                    *xyw = xt;
                    xyw++;
                    *xyw = yt;
                    xyw++;
                    *xyw = xt2;
                    xyw++;
                    *xyw = yt2;
                    gpf_addlineclipcomp (xywsav, 2, xyout, ncout, icout);
                    n=0;
                    xyw = xywsav;
                    in1 = 0;
                }
                x1 = x;
                y1 = y;
            }
        }
    }

/*  flush the rest of the line if needed  */

    if (n > 1) {
        gpf_addlineclipcomp (xywsav, n, xyout, ncout, icout);
    }

    return 1;

}  /*  end of function gpf_cliplineprim  */






/*
******************************************************************

                    g p f _ c l i p v e c 1

******************************************************************

  function name:    gpf_clipvec1      (int)

  call sequence:    gpf_clipvec1 (x1, y1, x2, y2, x, y)

  purpose:          Calculate point where a vector intersects the
                    current map window.  This is only called from
                    gpf_cliplineprim when one end point is outside
                    the window and the second endpoint is inside
                    the window.

  return value:     status code

                    0 = no intersection point found
                    1 = normal successful completion

  calling parameters:

    x1        r    CSW_F     x coordinate of first vector endpoint
    y1        r    CSW_F     y coordinate of first vector endpoint
    x2        r    CSW_F     x coordinate of second vector endpoint
    y2        r    CSW_F     y coordinate of second vector endpoint
    x         w    CSW_F*    x coordinate of intersection
    y         w    CSW_F*    y coordinate of intersection

*/

int GPFCalcdraw::gpf_clipvec1 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                  CSW_F *x, CSW_F *y)
{
    CSW_F       xt, yt, dx, dy, slope, yint;


/*  calculate slope and intercept of the line  */

    dx = x1 - x2;
    dy = y1 - y2;

    if (dx > -Tiny  &&  dx < 0.0f) {
        dx = -Tiny;
        x2 = x1 + Tiny;
    }
    else if (dx >= 0.0f  &&  dx < Tiny) {
        dx = Tiny;
        x2 = x1 - Tiny;
    }

    if (dy > -Tiny  &&  dy < 0.0f) {
        dy = -Tiny;
        y2 = y1 + Tiny;
    }
    else if (dy >= 0.0f  &&  dy < Tiny) {
        dy = Tiny;
        y2 = y1 - Tiny;
    }

    slope = dy / dx;
    yint = y1 - slope * x1;

/*  check each side for intersection  */

    if ((x1-fxmax)*(fxmax-x2) >= 0.0f) {
        yt = slope * fxmax + yint;
        if (yt <= fymax+Tiny  &&  yt >= fymin-Tiny) {
            *y = yt;
            *x = fxmax;
            return 1;
        }
    }

    if ((x1-fxmin)*(fxmin-x2) >= 0.0f) {
        yt = slope * fxmin + yint;
        if (yt <= fymax+Tiny  &&  yt >= fymin-Tiny) {
            *y = yt;
            *x = fxmin;
            return 1;
        }
    }

    if ((y1-fymax)*(fymax-y2) >= 0.0f) {
        xt = (fymax - yint) / slope;
        if (xt <= fxmax+Tiny  &&  xt >= fxmin-Tiny) {
            *x = xt;
            *y = fymax;
            return 1;
        }
    }

    if ((y1-fymin)*(fymin-y2) >= 0.0f) {
        xt = (fymin - yint) / slope;
        if (xt <= fxmax+Tiny  &&  xt >= fxmin-Tiny) {
            *x = xt;
            *y = fymin;
            return 1;
        }
    }

    return 0;

}  /*  end of function gpf_clipvec1  */






/*
******************************************************************

                    g p f _ c l i p v e c 2

******************************************************************

  function name:    gpf_clipvec2     (int)

  call sequence:    gpf_clipvec2 (x1, y1, x2, y2, x3, y3, x4, y4)

  purpose:          Calculate intersection of a vector with the
                    current clip window when both vector endpoints
                    are outside of the window.

  return value:     status code

                    0 = no intersection with the window
                    1 = intersection is present, returned
                        in x3, y3, x4, y4

  calling parameters:

    x1    r    CSW_F   x coordinate of input vector
    y1    r    CSW_F   y coordinate of input vector
    x2    r    CSW_F   x coordinate of 2nd input point
    y2    r    CSW_F   y coordinate of second input point
    x3    w    CSW_F*  first output x coordinate
    y3    w    CSW_F*  first output y coordinate
    x4    w    CSW_F*  second output x coordinate
    y4    w    CSW_F*  second output y coordinate

            x3, y3, x4, and y4 are not valid if the return status is zero

*/

int GPFCalcdraw::gpf_clipvec2 (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                  CSW_F *x3, CSW_F *y3, CSW_F *x4, CSW_F *y4)
{
    CSW_F       xt, yt, dx, dy, slope, yint, x, y;
    int         count;

/*  check for trivial rejection  */

    if ((x1>fxmax && x2>fxmax)  ||
        (x1<fxmin && x2<fxmin)  ||
        (y1>fymax && y2>fymax)  ||
        (y1<fymin && y2<fymin)) {

        return 0;
    }

/*
    If the vector endpoints are exactly on the clipping boundaries,
    the output is the same as the input.
*/
    if (x1 - fxmin == 0.0  &&  y1 - fymin == 0.0  &&
        x2 - fxmax == 0.0  &&  y2 - fymax == 0.0) {
        *x3 = x1;
        *y3 = y1;
        *x4 = x2;
        *y4 = y2;
        return 1;
    }
    if (x2 - fxmin == 0.0  &&  y2 - fymin == 0.0  &&
        x1 - fxmax == 0.0  &&  y1 - fymax == 0.0) {
        *x3 = x1;
        *y3 = y1;
        *x4 = x2;
        *y4 = y2;
        return 1;
    }
    if (x1 - fxmin == 0.0  &&  y1 - fymax == 0.0  &&
        x2 - fxmax == 0.0  &&  y2 - fymin == 0.0) {
        *x3 = x1;
        *y3 = y1;
        *x4 = x2;
        *y4 = y2;
        return 1;
    }
    if (x2 - fxmin == 0.0  &&  y2 - fymax == 0.0  &&
        x1 - fxmax == 0.0  &&  y1 - fymin == 0.0) {
        *x3 = x1;
        *y3 = y1;
        *x4 = x2;
        *y4 = y2;
        return 1;
    }

    dx = x1 - x2;
    dy = y1 - y2;

/*  reject zero length vector  */

    if (dx>-Tiny && dx<Tiny && dy>-Tiny && dy<Tiny) {
        return 0;
    }

/*  calculate slope and intercept of the line  */

    if (dx > -Tiny  &&  dx < 0.0f) {
        dx = -Tiny;
        x2 = x1 + Tiny;
    }
    else if (dx >= 0.0f  &&  dx < Tiny) {
        dx = Tiny;
        x2 = x1 - Tiny;
    }

    if (dy > -Tiny  &&  dy < 0.0f) {
        dy = -Tiny;
        y2 = y1 + Tiny;
    }
    else if (dy >= 0.0f  &&  dy < Tiny) {
        dy = Tiny;
        y2 = y1 - Tiny;
    }

    slope = dy / dx;
    yint = y1 - slope * x1;

    count = 0;

/*  check each side for intersection  */

    if ((x1-fxmax)*(fxmax-x2) >= 0.0f) {
        yt = slope * fxmax + yint;
        if (yt <= fymax+Tiny  &&  yt >= fymin-Tiny) {
            *x3 = fxmax;
            *y3 = yt;
            count++;
        }
    }

    if ((x1-fxmin)*(fxmin-x2) >= 0.0f) {
        yt = slope * fxmin + yint;
        if (yt <= fymax+Tiny  &&  yt >= fymin-Tiny) {
            y = yt;
            x = fxmin;
            if (count == 0) {
                *x3 = x;
                *y3 = y;
                count++;
            }
            else {
                *x4 = x;
                *y4 = y;
                if (!(*x3 - *x4 == 0.0  &&  *y3 - *y4 == 0)) {
                    count++;
                }
            }
        }
    }

    if (count == 2) {
        return 1;
    }

    if ((y1-fymax)*(fymax-y2) >= 0.0f) {
        xt = (fymax - yint) / slope;
        if (xt <= fxmax+Tiny  &&  xt >= fxmin-Tiny) {
            x = xt;
            y = fymax;
            if (count == 0) {
                *x3 = x;
                *y3 = y;
                count++;
            }
            else {
                *x4 = x;
                *y4 = y;
                if (!(*x3 - *x4 == 0.0  &&  *y3 - *y4 == 0)) {
                    count++;
                }
            }
        }
    }

    if (count == 2) {
        return 1;
    }

    if ((y1-fymin)*(fymin-y2) >= 0.0f) {
        xt = (fymin - yint) / slope;
        if (xt <= fxmax+Tiny  &&  xt >= fxmin-Tiny) {
            x = xt;
            y = fymin;
            if (count == 0) {
                *x3 = x;
                *y3 = y;
            }
            else {
                *x4 = x;
                *y4 = y;
                if (!(*x3 - *x4 == 0.0  &&  *y3 - *y4 == 0)) {
                    count++;
                }
            }
        }
    }

/*  output success if exactly two intersections found  */

    if (count == 2) {
        return 1;
    }
    else {
        return 0;
    }

}  /*  end of function gpf_clipvec2  */





/*
******************************************************************

                g p f _ a d d l i n e c l i p c o m p

******************************************************************

  function name:    gpf_addlineclipcomp      (int)

  call sequence:    gpf_addlineclipcomp (xyin, nin, xyout, ncout, icout)

  purpose:          add a clipped line component to the output arrays
                    only called from gpf_cliplineprim

  return value:     always returns zero

  calling parameters:

    xyin        r    CSW_F*       array of component coordinates
    nin         r    int          number of points in xyin
    xyout       w    CSW_F*       array of all output component coordinates
    ncout       r/w  int*         number of components in xyout before/after
                                  adding this component
    icout       r/w  int*         array with number of points per component

*/

int GPFCalcdraw::gpf_addlineclipcomp (CSW_F *xyin, int nin,
                         CSW_F *xyout, int *ncout, int *icout)
{
    CSW_F      *tmp;
    int        i, n;

    n = 0;
    for (i=0; i<*ncout; i++) {
        n += icout[i];
    }
    n *= 2;
    tmp = xyout + n;

    icout[*ncout] = nin;
    (*ncout)++;

    csw_memcpy ((char *)tmp, (char *)xyin, nin * 2 * sizeof(CSW_F));

    return 1;

}  /*  end of function gpf_addlineclipcomp  */





/*
******************************************************************

                 g p f _ c l i p t e x t i t e m

******************************************************************

  function name:    gpf_cliptextitem      (int)

  call sequence:    gpf_cliptextitem (x, y, text, angle, size, font,
                                      x1, y1, x2, y2,
                                      textout, xout, yout)

  purpose:          Clip a line of text to a rectangular window.
                    If the text is partially inside and partially
                    outside, only characters fully inside the window
                    are copied to textout.

  return value:     status code

                    -1 = completely outside window
                     1 = at least one character is inside

  calling parameters:

    x         r    CSW_F    x coordinate of text lower left
    y         r    CSW_F    y coordinate of text lower left
    anchor    r    int      anchor position
    text      r    char*    text string to clip
    angle     r    CSW_F    text angle in degrees
    size      r    CSW_F    height and width of an individual character
    font      r    int      font number for drawing the text
    x1        r    CSW_F    minimum x of window
    y1        r    CSW_F    minimum y of window
    x2        r    CSW_F    maximum x of window
    y2        r    CSW_F    maximum y of window
    textout   w    char*    output text string, must be at least as long as text
    xout      w    CSW_F*   x coordinate of clipped text anchor
    yout      w    CSW_F*   y coordinate of clipped text anchor

*/

int GPFCalcdraw::gpf_cliptextitem (CSW_F x, CSW_F y,
                      int anchor,
                      const char *text,
                      CSW_F angle, CSW_F size, int font,
                      CSW_F x1_in, CSW_F y1_in, CSW_F x2_in, CSW_F y2_in,
                      char *textout, CSW_F *xout, CSW_F *yout)
{
    CSW_F        x1, y1, x2, y2;
    CSW_F        tx[4], ty[4], tlen2,
                 tlen, txmin, tymin, txmax, tymax, dx, dy;
    int          first, nc, n1, n2, i;
    double       cosang, sinang;
    char         local[MAXTEXTLEN];

    GPFFont      font_obj;

    if (font >= 1000) {
        strcpy (textout, text);
        *xout = x;
        *yout = y;
        return 1;
    }

    anchor = anchor;

    if (x1_in < x2_in) {
        x1 = x1_in;
        x2 = x2_in;
    }
    else {
        x1 = x2_in;
        x2 = x1_in;
    }
    if (y1_in < y2_in) {
        y1 = y1_in;
        y2 = y2_in;
    }
    else {
        y1 = y2_in;
        y2 = y1_in;
    }

/*  setup for input text geometry calculation  */

    csw_StrTruncate (local, text, MAXTEXTLEN);
    nc = strlen (local);
    font_obj.gpf_TextLength2 (text, nc, font, size, &tlen);

    gpf_find_box_corners (x, y, anchor,
                          tlen, size, angle,
                          tx, ty);

/*  find x,y limits of text  */

    txmin = tx[0];
    for (i=1; i<4; i++) {
        if (tx[i] < txmin) {
            txmin = tx[i];
        }
    }

    txmax = tx[0];
    for (i=1; i<4; i++) {
        if (tx[i] > txmax) {
            txmax = tx[i];
        }
    }

    tymin = ty[0];
    for (i=1; i<4; i++) {
        if (ty[i] < tymin) {
            tymin = ty[i];
        }
    }

    tymax = ty[0];
    for (i=1; i<4; i++) {
        if (ty[i] > tymax) {
            tymax = ty[i];
        }
    }

/*  return if completely outside  */

    if (txmax < x1  ||  txmin > x2  ||  tymax < y1  ||  tymin > y2) {
        textout[0] = '\0';
        *xout = x;
        *yout = y;
        return -1;
    }

/*  return if completely inside  */

    if (txmin >= x1  &&  txmax <= x2  &&  tymin >= y1  &&  tymax <= y2) {
        *xout = x;
        *yout = y;
        strcpy (textout, text);
        return 1;
    }

    x = tx[0];
    y = ty[0];
    cosang = cos ((double)(angle * 3.1415926 / 180.0));
    sinang = sin ((double)(angle * 3.1415926 / 180.0));

/*  partly inside and partly outside  */

    n1 = 0;
    n2 = 0;

/*  which characters are inside  */

    first = 1;
    for (i=0; i<nc; i++) {

        font_obj.gpf_TextLength2 (text, i, font, size, &tlen);
        tx[0] = (CSW_F)(x + tlen * cosang);
        ty[0] = (CSW_F)(y + tlen * sinang);

        font_obj.gpf_TextLength2 (text+i, 1, font, size, &tlen2);
        dx = (CSW_F)(tlen2 * cosang);
        dy = (CSW_F)(tlen2 * sinang);

        tx[1] = tx[0] + dx;
        ty[1] = ty[0] + dy;
        tx[2] = tx[1] - dy;
        ty[2] = ty[1] + dx;
        tx[3] = tx[0] - dy;
        ty[3] = ty[0] + dx;

        if (tx[0] < x1  ||  tx[0] > x2  ||
            tx[1] < x1  ||  tx[1] > x2  ||
            tx[2] < x1  ||  tx[2] > x2  ||
            tx[3] < x1  ||  tx[3] > x2  ||
            ty[0] < y1  ||  ty[0] > y2  ||
            ty[1] < y1  ||  ty[1] > y2  ||
            ty[2] < y1  ||  ty[2] > y2  ||
            ty[3] < y1  ||  ty[3] > y2) {

            if (first) {
                continue;
            }
            else {
                n2 = i-1;
                break;
            }
        }
        else {
            if (first) {
                n1 = i;
                n2 = nc - 1;
                *xout = tx[0];
                *yout = ty[0];
                first = 0;
            }
        }
    }


/*  return clipped text string  */

    if (first) {
        textout[0] = '\0';
        *xout = x;
        *yout = y;
        return -1;
    }

    nc = n2 - n1 + 1;
    csw_StrTruncate (textout, text + n1, nc+1);

    return 1;

}  /*  end of function gpf_cliptextitem  */



/*
  ****************************************************************

                  g p f _ a d d c u t l i n e s

  ****************************************************************

  function name:    gpf_addcutlines        (int)

  call sequence:    gpf_addcutlines (xyin, nin, xyout, nout, memflag)

  purpose:          This function rearranges the points in a complex
                    polygon so that all holes are connected to the
                    main component with cut lines.  These cut lines
                    overlap each other but they may extend outside of
                    the polygon.  This works for the X window system, but
                    not for some other graphics systems.  A slower but
                    more stringent cut line algorithm can be found in
                    function ply_AddCutLines2.

  return value:     status code

                    -1 = memory allocation error
                     1 = normal successful completion

  calling parameters:

    xyin        r    CSW_F*    Array of x,y polygon points
    nin         r    int       Number of points in xyin
    xyout       w    CSW_F**   Returned pointer to xy array with cut lines
    nout        w    int*      Number of points in xyout
    memflag     w    int*      Flag for whether the xyout pointer may be csw_Freed.
                               0 = Do not csw_Free xyout memory
                               1 = xyout memory may be csw_Freed

*/

int GPFCalcdraw::gpf_addcutlines (CSW_F *xyin, int nin,
                     CSW_F **xyout, int *nout, int *memflag)
{
    int          jout, i, j, mflag, n2, n3, n4, n;
    CSW_F        *xy = NULL, *xyhole = NULL,
                 xmin, ymin, xmax, ymax;
    double       dd;

    bool    bsuccess = false;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
        if (!bsuccess) {
            csw_Free (xy);
            xyout[0] = NULL;
            *nout = 0;
            *memflag = 0;
        }
    };
    CSWScopeGuard  func_scope_guard (fscope);


/*  allocate memory for cut line version of the polygon  */

    //if (nin >= MAXWORKSIZE / 2) {
    if (nin >= 0) {
        xy = (CSW_F *)csw_Malloc (nin * 8 * sizeof (CSW_F));
        if (!xy) {
            return -1;
        }
        i = nin * 4;
        xyhole = xy + i;
        *memflag = 1;
        mflag = 1;
    }
    else {
        xy = XYcutwork;
        xyhole = XYcuthole;
        *memflag = 0;
        mflag = 0;
    }

/*  return input if less than 3 points  */

    if (nin < 3) {
        j = 0;
        i = 0;
        for (i=0; i<nin; i++) {
            xy[j] = xyin[j];
            j++;
            xy[j] = xyin[j];
            j++;
        }
        xyout[0] = xy;
        *nout = nin;
        bsuccess = true;
        return 1;
    }

/*  check if the polygon has any holes  */

    gpf_xylimits2 (xyin, nin * 2, &xmin, &ymin, &xmax, &ymax);
    dd = xmax - xmin + ymax - ymin;
    dd /= 100000.f;

    gpf_findholeclosure (xyin, nin, xy, &j, dd);

    if (j >= nin - 1) {
        xyout[0] = xyin;
        *nout = nin;
        *memflag = 0;
        if (mflag) {
            csw_Free (xy);
            xy = NULL;
        }
        bsuccess = true;
        return 1;
    }

/*  copy main component to output  */

    n = j * 2;
    for (i=0; i<n; i++) {
        xy[i] = xyin[i];
    }

/*  get each hole and insert it into the main array  */

    jout = j;
    for (;;) {
        n2 = nin - j;
        if (n2 < 2) {
            break;
        }
        gpf_findholeclosure (xyin + n, n2, xyhole, &n3, dd);
        gpf_insertholefast (xy, jout, xyhole, n3, &n4);
        j += n3;
        jout = n4;
        n = j * 2;
    }

    xyout[0] = xy;
    *nout = jout;

    bsuccess = true;

    return 1;

}  /*  end of function gpf_addcutlines  */



/*
  ****************************************************************

              g p f _ r e m o v e c u t l i n e s

  ****************************************************************

    Remove the cut lines that connect holes with the origin of the
  main polygon component.  This must be done on any polygons that
  may subsequently be run through the polygon clipping stuff.

    The output arrays are allocated here.  The calling function
  should csw_Free them when finished with them.

    The function returns 1 on success or -1 if a csw_Malloc failed.

*/

int GPFCalcdraw::gpf_removecutlines (CSW_F *xin, CSW_F *yin, int ncompin, int *nptsin,
                        CSW_F **xout, CSW_F **yout, int *ncout, int **npout)
{
    CSW_F         *x = NULL, *y = NULL, *xx = NULL, *yy = NULL,
                  *xw = NULL, *yw = NULL, *xt = NULL, *yt = NULL,
                  x1, y1, x2, y2, xorig, yorig;
    int           i, j, n, n1, n2, n2orig, nw, nt, ntot, ntot2, *ic = NULL;
    int           holeflag, ncomp, *npts = NULL, sf, nn;
    double        dd;

    CSWPolyUtils  ply_utils_obj;

    bool  bsuccess = false;


    auto fscope = [&]()
    {
        csw_Free (x);
        csw_Free (npts);
        csw_Free (xw);
        if (!bsuccess) {
            csw_Free (xx);
            csw_Free (yy);
            csw_Free (ic);
            *xout = NULL;
            *yout = NULL;
            *npout = NULL;
            *ncout = 0;
        }
    };
    CSWScopeGuard  func_scope_guard (fscope);


/*
    how many total points in input arrays
*/
    nt = 0;
    for (i=0; i<ncompin; i++) {
        nt += nptsin[i];
    }
    ntot2 = nt / 2;
    ntot = nt * 2;

    holeflag = 0;
    for (i=0; i<nt; i++) {
        if (xin[i] >= HOLEFLAG  ||
            yin[i] >= HOLEFLAG  ||
            xin[i] <= -HOLEFLAG  ||
            yin[i] <= -HOLEFLAG) {
            holeflag = 1;
        }
    }

/*
    Allocate local arrays for the points.  Remove any redundant closing
    points from the input polygon and put the cleaned up version in the
    local x, y and npts arrays.
*/
    sf = sizeof(CSW_F);
    x = (CSW_F *)csw_Malloc (ntot*2*sf);
    if (!x) {
        return -1;
    }
    y = x + ntot;
    npts = (int *)csw_Malloc (ncompin*sizeof(int));
    if (!npts) {
        return -1;
    }

    if (holeflag == 1) {
        memcpy (x, xin, nt*sf);
        memcpy (y, yin, nt*sf);
        ncomp = ncompin;
        memcpy (npts, nptsin, ncomp*sizeof(int));
    }

    else {
        ncomp = 0;
        n = 0;
        n1 = 0;
        for (i=0; i<ncompin; i++) {
            x1 = xin[n1];
            y1 = yin[n1];
            n2 = n1 + nptsin[i] - 1;
            n2orig = n2;
            while (ply_utils_obj.ply_graze1 (x1, y1, xin[n2], yin[n2]) == 1) {
                n2--;
                if (n2 == n1) {
                    break;
                }
            }
            if (n2 == n1) {
                n1 += nptsin[i];
                continue;
            }
            if (n2 != n2orig) {
                n2++;
            }
            for (j=n1; j<=n2; j++) {
                x[n] = xin[j];
                y[n] = yin[j];
                n++;
            }
            npts[ncomp] = n2 - n1 + 1;
            ncomp++;
            n1 += nptsin[i];
        }
    }

/*
    allocate output and workspace memory
*/
    xx = (CSW_F *)csw_Malloc (ntot * sf);
    if (!xx) {
        return -1;
    }
    yy = (CSW_F *)csw_Malloc (ntot * sf);
    if (!yy) {
        return -1;
    }
    ic = (int *)csw_Malloc (ntot2 * sizeof(int));
    if (!ic) {
        return -1;
    }

    xw = (CSW_F *)csw_Malloc (ntot * 2 * sf);
    if (!xw) {
        return -1;
    }
    yw = xw + ntot;

    *xout = xx;
    *yout = yy;
    *npout = ic;

/*
    if there are any hole flags in the input,
    copy input to output and return
*/
    if (holeflag == 1) {
        csw_memcpy (xx, x, nt * sf);
        csw_memcpy (yy, y, nt * sf);
        csw_memcpy (ic, npts, ncomp * sizeof(int));
        *ncout = ncomp;

        bsuccess = true;
        return 1;
    }

/*
    get the limits and grazing distance
*/

    gpf_xandylimits2 (x, y, nt,
                      &x1, &y1, &x2, &y2);
    dd = x2 - x1 + y2 - y1;
    dd /= 100000.f;

/*
    find the initial component
*/

    gpf_findholeclosure2 (x, y, nt, xw, yw, &nw, dd);

/*
    copy the main component to output arrays
*/

    csw_memcpy (xx, xw, nw * sf);
    csw_memcpy (yy, yw, nw * sf);
    ic[0] = nw;

    n = nw;
    xt = xx + n;
    yt = yy + n;

/*
    append each closed hole to the output arrays
*/

    nn = 1;
    xorig = x[0];
    yorig = y[0];

    for (;;) {

        n2 = nt - n;
        if (n2 < 2) {
            break;
        }

        gpf_findholeclosure2 (x + n, y + n, n2, xw, yw, &nw, dd);
        csw_memcpy (xt, xw, nw * sf);
        csw_memcpy (yt, yw, nw * sf);

        xt += nw;
        yt += nw;
        ic[nn] = nw;
        nn++;

        n += nw;
        if (n >= nt) {
            break;
        }
        while (Graze (x[n], y[n], xorig, yorig, dd)) {
            n++;
            if (n >= nt) {
                break;
            }
        }
        if (n >= nt) {
            break;
        }
    }

    *ncout = nn;

    bsuccess = true;

    return 1;

}  /*  end of function gpf_removecutlines  */




/*
  ****************************************************************

                         G r a z e

  ****************************************************************

*/

int GPFCalcdraw::Graze (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2, double dd)
{
    double       dx, dy;


    dx = x2 - x1;
    if (dx < 0.0f) dx = -dx;
    dy = y2 - y1;
    if (dy < 0.0f) dy = -dy;

    if (dx <= dd  &&  dy <= dd) {
        return 1;
    }

    return 0;

}  /*  end of private Graze function  */





/*
  ****************************************************************

            g p f _ f i n d h o l e c l o s u r e

  ****************************************************************

*/

int GPFCalcdraw::gpf_findholeclosure (CSW_F *xyin, int nin, CSW_F *xyout, int *nout, double dd)
{
    int           i, j, n, m;
    CSW_F         xanc, yanc, xt, yt;
    double        dx, dy;


    xanc = xyin[0];
    yanc = xyin[1];
    n = 2;
    dd /= 100000.0;

    xyout[0] = xanc;
    xyout[1] = yanc;
    m = 2;
    j = -1;

    for (i=1; i<nin; i++) {
        xt = xyin[n];
        n++;
        yt = xyin[n];
        n++;
        dx = xt - xanc;
        dy = yt - yanc;
        if (dx < 0.f) dx = -dx;
        if (dy < 0.f) dy = -dy;
        if (dx < dd  &&  dy < dd) {
            j = i;
            xyout[m] = xt;
            m++;
            xyout[m] = yt;
            m++;
            break;
        }
        xyout[m] = xt;
        m++;
        xyout[m] = yt;
        m++;
    }

    if (j == -1) {
        xyout[m] = xyout[0];
        m++;
        xyout[m] = xyout[1];
        m++;
    }

    *nout = m / 2;

    return 1;

}  /*  end of function gpf_findholeclosure  */



/*
  ****************************************************************

            g p f _ f i n d h o l e c l o s u r e 2

  ****************************************************************

*/

int GPFCalcdraw::gpf_findholeclosure2 (CSW_F *xin, CSW_F *yin, int nin,
                          CSW_F *xout, CSW_F *yout, int *nout, double dd)
{
    int           i, j, n, m;
    CSW_F         xanc, yanc, xt, yt;
    double        dx, dy;


    xanc = xin[0];
    yanc = yin[0];
    n = 1;
    dd /= 100000.0;

    xout[0] = xanc;
    yout[0] = yanc;
    m = 1;
    j = -1;

    for (i=1; i<nin; i++) {
        xt = xin[n];
        yt = yin[n];
        n++;
        dx = xt - xanc;
        dy = yt - yanc;
        if (dx < 0.f) dx = -dx;
        if (dy < 0.f) dy = -dy;
        while (dx < dd  &&  dy < dd) {
            j = i;
            xout[m] = xt;
            yout[m] = yt;
            m++;
            if (n >= nin) goto CLOSURE_FOUND;
            xt = xin[n];
            yt = yin[n];
            n++;
            dx = xt - xanc;
            dy = yt - yanc;
            if (dx < 0.f) dx = -dx;
            if (dy < 0.f) dy = -dy;
            if (dx >= dd  ||  dy >= dd) goto CLOSURE_FOUND;
        }
        xout[m] = xt;
        yout[m] = yt;
        m++;
    }

  CLOSURE_FOUND:

    if (j == -1) {
        xout[m] = xout[0];
        yout[m] = yout[0];
        m++;
    }

    *nout = m;

    return 1;

}  /*  end of function gpf_findholeclosure2  */






/*
  ****************************************************************

             g p f _ i n s e r t h o l e f a s t

  ****************************************************************

*/

int GPFCalcdraw::gpf_insertholefast (CSW_F *xyin, int nin,
                        CSW_F *xyhole, int nhole, int *nout)
{
    int          i, n1, n2;
    CSW_F        xt, yt;


    n1 = nin * 2 - 2;
    xt = xyin[n1];
    yt = xyin[n1+1];

    n1 = nin * 2;
    n2 = 0;

    for (i=0; i<nhole; i++) {
        xyin[n1] = xyhole[n2];
        n1++;
        n2++;
        xyin[n1] = xyhole[n2];
        n1++;
        n2++;
    }

    xyin[n1] = xt;
    n1++;
    xyin[n1] = yt;
    n1++;

    *nout = n1 / 2;

    return 1;

}



/*
  ****************************************************************

                 g p f _ 3 p o i n t c i r c l e

  ****************************************************************

    Given 3 non colinear points, calculate the center point and the
  radius of a circle passing through the points.  On success, 1 is
  returned.  If the points are very nearly or exactly collinear,
  -1 is returned.

*/

int GPFCalcdraw::gpf_3pointcircle (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                      CSW_F x3, CSW_F y3,
                      CSW_F *xc, CSW_F *yc, CSW_F *r)
{
    CSW_F           d1, d2, d3, tiny, xt1, yt1, xt2, yt2;
    double          dxt1, dyt1, dxt2, dyt2,
                    dxt3, dyt3, dxt4, dyt4, dxc, dyc;
    int             istat;

    CSWPolyUtils    ply_utils_obj;

/*
    make sure points are not collinear
*/
    gpf_calcdistance1 (x1, y1, x2, y2, &d1);
    gpf_calcdistance1 (x1, y1, x3, y3, &d2);
    gpf_calcdistance1 (x2, y2, x3, y3, &d3);

    tiny = (d1 + d2 + d3) / 300000.f;
    if (tiny <= 1.e-10f) {
        return -1;
    }

    gpf_perpdistance1 (x1, y1, x2, y2, x3, y3, &d1);
    if (d1 < tiny) {
        return -1;
    }

/*
    get points defining two radial lines
*/
    gpf_perpbisector (x1, y1, x2, y2,
                      &xt1, &yt1, &xt2, &yt2);
    dxt1 = xt1;
    dyt1 = yt1;
    dxt2 = xt2;
    dyt2 = yt2;

    gpf_perpbisector (x1, y1, x3, y3,
                      &xt1, &yt1, &xt2, &yt2);
    dxt3 = xt1;
    dyt3 = yt1;
    dxt4 = xt2;
    dyt4 = yt2;

/*
    center of circle is at intersection of radial lines
*/
    istat = ply_utils_obj.ply_segint (dxt1, dyt1, dxt2, dyt2,
                        dxt3, dyt3, dxt4, dyt4,
                        &dxc, &dyc);
    if (!(istat == 0  ||  istat == 1)) {
        return -1;
    }

    *xc = (CSW_F)dxc;
    *yc = (CSW_F)dyc;

    gpf_calcdistance1 (*xc, *yc, x1, y1, r);

    return 1;

}  /*  end of function gpf_3pointcircle  */


/*
  ****************************************************************

               g p f _ p e r p b i s e c t o r

  ****************************************************************

    Return 2 points defining the perpendicular bisector to a
  line segment.  This function assumes a non zero length line
  segment is specified.

*/

int GPFCalcdraw::gpf_perpbisector (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                      CSW_F *xb1, CSW_F *yb1, CSW_F *xb2, CSW_F *yb2)
{
    double       dx, dy, ax, ay, tiny, dtiny;
    double       xt1, yt1, xt2, yt2;
    double       ang, cang, sang;


    xt1 = x1;
    if (xt1 < 0.0) xt1 = -xt1;
    xt2 = x2;
    if (xt2 < 0.0) xt2 = -xt2;
    yt1 = y1;
    if (yt1 < 0.0) yt1 = -yt1;
    yt2 = y2;
    if (yt2 < 0.0) yt2 = -yt2;

    dx = xt1 + yt1 + xt2 + yt2;

    dtiny = dx * CSW_DOUBLE_EPSILON * 2.0;

/*
    first output point is simply the mid point of the specified segment
*/
    *xb1 = (CSW_F)((x1 + x2) / 2.f);
    *yb1 = (CSW_F)((y1 + y2) / 2.f);

/*
    get a tiny value to define vertical and horizontal slopes
*/
    dx = x2 - x1;
    ax = dx;
    if (ax < 0.f) ax = -dx;

    dy = y2 - y1;
    ay = dy;
    if (ay < 0.f) ay = -dy;

    if (ax < dtiny  &&  ay < dtiny) {
        *xb2 = *xb1;
        *yb2 = *yb1;
        return 0;
    }

    tiny = (ax + ay) / 1.e9f;

/*
    vertical input segment
*/
    if (ax < tiny) {
        *xb2 = (CSW_F)(*xb1 + ay);
        *yb2 = *yb1;
        return 1;
    }

/*
    horizontal input segment
*/
    if (ay < tiny) {
        *xb2 = *xb1;
        *yb2 = (CSW_F)(*yb1 + ax);
        return 1;
    }

/*
    general input segment
*/
    ax = dx * dx + dy * dy;
    ax = sqrt (ax);
    ax *= 10.f;
    ang = atan2 (dy, dx);
    cang = cos (ang);
    sang = sin (ang);
    *xb2 = (CSW_F)(*xb1 + ax * sang);
    *yb2 = (CSW_F)(*yb1 + ax * cang);

    return 1;

}  /*  end of function gpf_perpbisector  */






/*
******************************************************************

               g p f _ c l i p l i n e p r i m _ z

******************************************************************

  function name:    gpf_cliplineprim_z     (int)

  call sequence:    gpf_cliplineprim_z  (xyin, zin, nin,
                                         x1in, y1in, x2in, y2in, znull,
                                         xyout, ncout, icout)

  purpose:          Clip a line primitive to the window defined by x1in
                    y1in, x2in, y2in.  Also, keep the integer z array
                    associated with the original line in sync with the
                    clipped line components.

  return value:     status code

                    -1 = memory allocation error
                     1 = normal successful completion

  calling parameters:

    xyin      r    CSW_F*    array of packed x,y coordinates
    zin       r    int*      array of z values associated with the line
    nin       r    int       number of points in the xyin array
    x1in      r    CSW_F     minimum x of clip window
    y1in      r    CSW_F     minimum y of clip window
    x2in      r    CSW_F     maximum x of clip window
    y2in      r    CSW_F     maximum y of clip window
    znull     r    int       z value to use for clipped points
    xyout     w    CSW_F*    output x,y coordinate array
                             (this should be at least nin * 4 CSW_F words)
    zout      w    int*      output z values
                             (big enough for at least nin * 2 integers)
    ncout     w    int*      number of output components
    icout     w    int*      array with number of points per output component
                             (this should be at least nin * 2 int words)

*/

int GPFCalcdraw::gpf_cliplineprim_z (CSW_F *xyin, int *zin, int nin,
                        CSW_F x1_in, CSW_F y1_in, CSW_F x2_in, CSW_F y2_in, int znull,
                        CSW_F *xyout, int *zout, int *ncout, int *icout)
{
    CSW_F       x1in, y1in, x2in, y2in;
    int         i, n, in1, in2;
    CSW_F       *xyw = NULL, *xywsav = NULL,
                xt, yt, xt2, yt2, x1, y1, x, y;
    int         *zw = NULL, *zsav = NULL, z;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
        csw_Free (xyw);
        csw_Free (zw);
    };
    CSWScopeGuard  func_scope_guard (fscope);


    if (x1_in < x2_in) {
        x1in = x1_in;
        x2in = x2_in;
    }
    else {
        x1in = x2_in;
        x2in = x1_in;
    }
    if (y1_in < y2_in) {
        y1in = y1_in;
        y2in = y2_in;
    }
    else {
        y1in = y2_in;
        y2in = y1_in;
    }

    fxmin = x1in;
    fymin = y1in;
    fxmax = x2in;
    fymax = y2in;
    Tiny = (x2in-x1in+y2in-y1in) / 200000.f ;

    *ncout = 0;

    //if (nin*5 >= MAXWORKSIZE2) {
    if (nin*5 >= 0) {
        xyw = (CSW_F *)csw_Malloc (nin * 5 * sizeof(CSW_F));
        if (!xyw) {
            return -1;
        }
        zw = (int *)csw_Malloc (nin * 3 * sizeof(int));
        if (!zw) {
            return -1;
        }
    }
    else {
        xyw = XYwork;
        zw = Zwork;
    }

/*  check if the first point is inside or outside  */

    x = xyin[0];
    y = xyin[1];
    in1 = (x>fxmin && x<fxmax && y>fymin && y<fymax);

    xywsav = xyw;
    zsav = zw;
    if (in1) {
        xyw[0] = x;
        xyw[1] = y;
        n = 1;
        xyw += 2;
        zw[0] = zin[0];
        zw++;
    }
    else {
        n = 0;
    }

    x1 = x;
    y1 = y;

/*  loop through the coordinates, extracting pieces
    of the polyline that intersect the window, and
    output those pieces  */

    xyin += 2;
    for (i=1; i< nin; i++) {

        x = *xyin;
        xyin++;
        y = *xyin;
        xyin++;
        z = zin[i];
        in2 = (x>fxmin && x<fxmax && y>fymin && y<fymax);

/*      previous point was inside  */

        if (in1) {
            if (in2) {
                *xyw = x;
                xyw++;
                *xyw = y;
                xyw++;
                *zw = z;
                zw++;
                x1 = x;
                y1 = y;
                n++;
            }
            else {
                gpf_clipvec1 (x1, y1, x, y, &xt, &yt);
                *xyw = xt;
                xyw++;
                *xyw = yt;
                *zw = znull;
                n++;
                gpf_addlineclipcomp_z (xywsav, zsav, n, xyout, zout, ncout, icout);
                n = 0;
                xyw = xywsav;
                zw = zsav;
                in1 = 0;
                x1 = x;
                y1 = y;
            }
        }

/*      previous point was outside  */

        else {
            if (in2) {
                gpf_clipvec1 (x1, y1, x, y, &xt, &yt);
                *xyw = xt;
                xyw++;
                *xyw = yt;
                xyw++;
                *xyw = x;
                xyw++;
                *xyw = y;
                xyw++;
                *zw = znull;
                zw++;
                *zw = z;
                zw++;
                n += 2;
                in1 = 1;
                x1 = x;
                y1 = y;
            }
            else {
                if (gpf_clipvec2(x1, y1, x, y, &xt, &yt, &xt2, &yt2)) {
                    *xyw = xt;
                    xyw++;
                    *xyw = yt;
                    xyw++;
                    *xyw = xt2;
                    xyw++;
                    *xyw = yt2;
                    *zw = znull;
                    zw++;
                    *zw = znull;
                    gpf_addlineclipcomp_z (xywsav, zsav, 2, xyout, zout, ncout, icout);
                    n=0;
                    xyw = xywsav;
                    zw = zsav;
                    in1 = 0;
                }
                x1 = x;
                y1 = y;
            }
        }
    }

/*  flush the rest of the line if needed  */

    if (n > 1) {
        gpf_addlineclipcomp_z (xywsav, zsav, n, xyout, zout, ncout, icout);
    }

    return 1;

}  /*  end of function gpf_cliplineprim_z  */





/*
******************************************************************

           g p f _ a d d l i n e c l i p c o m p _ z

******************************************************************

  function name:    gpf_addlineclipcomp_z      (int)

  call sequence:    gpf_addlineclipcomp_z (xyin, zin, nin,
                                           xyout, zout, ncout, icout)

  purpose:          add a clipped line component to the output arrays
                    only called from gpf_cliplinerect

  return value:     always returns zero

  calling parameters:

    xyin        r    CSW_F*       array of component coordinates
    zin         r    int*         array of z values associated with xyin
    nin         r    int          number of points in xyin
    xyout       w    CSW_F*       array of all output component coordinates
    zout        w    int*         array of z values associated with xyout
    ncout       r/w  int*         number of components in xyout before/after
                                  adding this component
    icout       r/w  int*         array with number of points per component

*/

int GPFCalcdraw::gpf_addlineclipcomp_z (CSW_F *xyin, int *zin, int nin,
                           CSW_F *xyout, int *zout, int *ncout, int *icout)
{
    CSW_F      *tmp = NULL;
    int        *ztmp = NULL;
    int        i, n;

/*
    update component counts
*/
    n = 0;
    for (i=0; i<*ncout; i++) {
        n += icout[i];
    }

    icout[*ncout] = nin;
    (*ncout)++;

    ztmp = zout + n;
    n *= 2;
    tmp = xyout + n;

/*
    transfer the x,y and z values
*/
    csw_memcpy ((char *)tmp, (char *)xyin, nin * 2 * sizeof(CSW_F));
    csw_memcpy ((char *)ztmp, (char *)zin, nin * sizeof(int));

    return 1;

}  /*  end of function gpf_addlineclipcomp_z  */







/*
  *************************************************************************

           g p f _ c l o s e _ p o l y g o n _ h o l e s

  *************************************************************************

    Given arrays of x and y coordinates, and the components array, output
  a new array of x and y where all of the holes are closed.

*/

int GPFCalcdraw::gpf_close_polygon_holes
                            (CSW_F *x, CSW_F *y, int *npts, int ncomp,
                             CSW_F **xout, CSW_F **yout, int **npout)
{
    CSW_F         *xx = NULL, *yy = NULL, x1, y1, x2, y2;
    int           i, n, nt, ntot, *ic = NULL;
    int           nout, nn, sf;
    double        dd;

    bool  bsuccess = false;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xx);
            csw_Free (yy);
            csw_Free (ic);
            *xout = NULL;
            *yout = NULL;
            *npout = NULL;
        }
    };
    CSWScopeGuard  func_scope_guard (fscope);


/*
    how many total points in input arrays
*/
    nt = 0;
    for (i=0; i<ncomp; i++) {
        nt += npts[i];
    }
    ntot = nt * 2;

/*
    allocate output and workspace memory
*/
    sf = sizeof(CSW_F);
    xx = (CSW_F *)csw_Malloc (ntot * sf);
    if (!xx) {
        return -1;
    }
    yy = (CSW_F *)csw_Malloc (ntot * sf);
    if (!yy) {
        return -1;
    }
    ic = (int *)csw_Malloc (ncomp * sizeof(int));
    if (!ic) {
        return -1;
    }

    *xout = xx;
    *yout = yy;
    *npout = ic;

/*
    if there are any hole flags in the input,
    copy input to output and return
*/
    for (i=0; i<nt; i++) {
        if (x[i] >= HOLEFLAG  ||
            y[i] >= HOLEFLAG  ||
            x[i] <= -HOLEFLAG  ||
            y[i] <= -HOLEFLAG) {

            csw_memcpy (xx, x, nt * sf);
            csw_memcpy (yy, y, nt * sf);
            csw_memcpy (ic, npts, ncomp * sizeof(int));

            bsuccess = true;

            return 1;
        }
    }

/*
    get the limits and grazing distance
*/
    gpf_xandylimits2 (x, y, nt,
                      &x1, &y1, &x2, &y2);
    dd = x2 - x1 + y2 - y1;
    dd /= 10000.f;

/*
    loop through each component and close it if needed
*/
    n = 0;
    nout = 0;
    for (i=0; i<ncomp; i++) {
        nn = npts[i];
        ic[i] = nn;
        csw_memcpy (xx+nout, x+n, nn * sizeof(CSW_F));
        csw_memcpy (yy+nout, y+n, nn * sizeof(CSW_F));
        nout += nn;
        if (!Graze (x[n], y[n], x[n+nn-1], y[n+nn-1], dd)) {
            xx[nout] = x[n];
            yy[nout] = y[n];
            ic[i]++;
            nout++;
        }
        else {
            xx[nout-1] = x[n];
            yy[nout-1] = y[n];
        }
        n += nn;
    }

    bsuccess = true;

    return 1;

}  /*  end of function gpf_close_polygon_holes  */



/*
 * return the rotated corners of an anchored box.  The corners are
 * lower left, lower right, upper right and upper left in order.
 */
int GPFCalcdraw::gpf_find_box_corners (CSW_F x, CSW_F y, int anchor,
                          CSW_F w, CSW_F h, CSW_F angle,
                          CSW_F *cx, CSW_F *cy)
{
    double       cang, sang;
    CSW_F        x1, y1, x2, y2, ca, sa;

    switch (anchor) {

        case 1:  /* bottom left */
            x1 = 0.0f;
            x2 = w;
            y1 = 0.0f;
            y2 = h;
            break;

        case 2:  /* bottom center */
            x1 = -w / 2.0f;
            x2 = w / 2.0f;
            y1 = 0.0f;
            y2 = h;
            break;

        case 3:  /* bottom right */
            x1 = -w;
            x2 = 0.0f;
            y1 = 0.0f;
            y2 = h;
            break;

        case 4:  /* center left */
            x1 = 0.0f;
            x2 = w;
            y1 = -h / 2.0f;
            y2 = h / 2.0f;
            break;

        case 5:  /* center center */
            x1 = -w / 2.0f;
            x2 = w / 2.0f;
            y1 = -h / 2.0f;
            y2 = h / 2.0f;
            break;

        case 6:  /* center right */
            x1 = -w;
            x2 = 0.0f;
            y1 = -h / 2.0f;
            y2 = h / 2.0f;
            break;

        case 7:  /* top left */
            x1 = 0.0f;
            x2 = w;
            y1 = -h;
            y2 = 0.0f;
            break;

        case 8:  /* top center */
            x1 = -w / 2.0f;
            x2 = w / 2.0f;
            y1 = -h;
            y2 = 0.0f;
            break;

        case 9:  /* top right */
            x1 = -w;
            x2 = 0.0f;
            y1 = -h;
            y2 = 0.0f;
            break;

        default:  /* same as lower left */
            x1 = 0.0f;
            x2 = w;
            y1 = 0.0f;
            y2 = h;
            break;

    }

    cang = cos ((double)angle * 3.1415926 / 180.0);
    sang = sin ((double)angle * 3.1415926 / 180.0);

    ca = (CSW_F)cang;
    sa = (CSW_F)sang;

    cx[0] = x + x1 * ca - y1 * sa;
    cy[0] = y + y1 * ca + x1 * sa;
    cx[1] = x + x2 * ca - y1 * sa;
    cy[1] = y + y1 * ca + x2 * sa;
    cx[2] = x + x2 * ca - y2 * sa;
    cy[2] = y + y2 * ca + x2 * sa;
    cx[3] = x + x1 * ca - y2 * sa;
    cy[3] = y + y2 * ca + x1 * sa;

    return 1;

}


/*
  ****************************************************************

          g p f _ s h o r t p e r p b i s e c t o r

  ****************************************************************

    Return 2 points defining the perpendicular bisector to a
  line segment.  This function assumes a non zero length line
  segment is specified.

*/

int GPFCalcdraw::gpf_shortperpbisector (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                           CSW_F *xb1, CSW_F *yb1, CSW_F *xb2, CSW_F *yb2)
{
    double       dx, dy, ax, ay, tiny, dtiny;
    double       xt1, yt1, xt2, yt2;
    double       ang, cang, sang;

    xt1 = x1;
    if (xt1 < 0.0) xt1 = -xt1;
    xt2 = x2;
    if (xt2 < 0.0) xt2 = -xt2;
    yt1 = y1;
    if (yt1 < 0.0) yt1 = -yt1;
    yt2 = y2;
    if (yt2 < 0.0) yt2 = -yt2;

    dx = xt1 + yt1 + xt2 + yt2;

    dtiny = dx * CSW_DOUBLE_EPSILON * 2.0;

/*
    first output point is simply the mid point of the specified segment
*/
    *xb1 = (CSW_F)((x1 + x2) / 2.f);
    *yb1 = (CSW_F)((y1 + y2) / 2.f);

/*
    get a tiny value to define vertical and horizontal slopes
*/
    dx = x2 - x1;
    ax = dx;
    if (ax < 0.f) ax = -dx;

    dy = y2 - y1;
    ay = dy;
    if (ay < 0.f) ay = -dy;

    if (ax < dtiny  &&  ay < dtiny) {
        *xb2 = *xb1;
        *yb2 = *yb1;
        return 0;
    }

    tiny = (ax + ay) / 1.e9f;

/*
    vertical input segment
*/
    if (ax < tiny) {
        *xb2 = (CSW_F)(*xb1 + ay);
        *yb2 = *yb1;
        return 1;
    }

/*
    horizontal input segment
*/
    if (ay < tiny) {
        *xb2 = *xb1;
        *yb2 = (CSW_F)(*yb1 + ax);
        return 1;
    }

/*
    general input segment
*/
    ax = dx * dx + dy * dy;
    ax = sqrt (ax);
    ang = atan2 (dy, dx);
    cang = cos(ang);
    sang = sin (ang);
    *xb2 = (CSW_F)(*xb1 + ax * sang);
    *yb2 = (CSW_F)(*yb1 - ax * cang);

    return 1;

}  /*  end of function gpf_shortperpbisector  */
