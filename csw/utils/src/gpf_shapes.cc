
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_shapes.cc

    This file has utility functions used in calculating shapes 
    (i.e. boxes, arcs, ellipses).  The functions can be used to 
    convert various shapes into general polygons and to get the
    spatial limits of rotated shapes, among other things.

    These functions could probably have been put in gpf_calcdraw.c,
    but they are here because the other file was getting too big.
*/

/*
    system header files
*/
#include <stdio.h>
#include <math.h>

/*
    application header files
*/
#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/gpf_shape.h"



/*
  ****************************************************************

                     g p f _ a r c l i m i t s              

  ****************************************************************

  function name:     gpf_arclimits             (int)

  call sequence:     gpf_arclimits (xc, yc, r1, r2, ang1, anglen, 
                                    rotang, flag, 
                                    x1, y1, x2, y2)

  purpose:           Calculate the x,y limits of a full or partial
                     arc (circle or ellipse).  The arc may be rotated
                     at any angle.

  return value:      status code

                     1 = successful completion
                    -1 = memory allocation failed in trying to
                         calculate the limits of a partial arc.
                         In this case, the limits of a full
                         nonrotated arc are returned.

  calling parameters:

    xc           r      CSW_F       x coordinate of center of arc
    yc           r      CSW_F       y coordinate of center of arc
    r1           r      CSW_F       radius of unrotated arc along the
                                    x axis
    r2           r      CSW_F       radius of unrotated arc along the
                                    y axis (for a circle r1 = r2)
    ang1         r      CSW_F       angle to start drawing the arc 
                                    (degrees positive ccw from +x axis)
    anglen       r      CSW_F       angular length of arc in degrees
                                    positive, ccw  negative cw
                                    for a full arc make this >= 360.
    rotang       r      CSW_F       rotation axis of the arc in degrees. 
                                    The r1 radius will be rotated through 
                                    this angle.
    flag         r      int         flag for partial arc representation
                                    1 = pie slice
                                    2 = chord
    x1           w      CSW_F*      pointer to minimum x of the arc
    y1           w      CSW_F*      pointer to minimum y of the arc
    x2           w      CSW_F*      pointer to maximum x of the arc
    y2           w      CSW_F*      pointer to maximum y of the arc 

*/

int GPFShape::gpf_arclimits (CSW_F xc, CSW_F yc, CSW_F r1, CSW_F r2, 
                   CSW_F ang1, CSW_F anglen, CSW_F rotang, int flag,
                   CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{
    CSW_F              xyw[300], *xy2[2];
    int                istat, memflg = 0, nout;

    xy2[0] = NULL;
    xy2[1] = NULL;

    auto fscope = [&]()
    {
        if (memflg) {
            csw_Free (xy2[0]);
            csw_Free (xy2[1]);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    initialize limits as if the arc were full and
    unrotated in case of an error
*/
    *x1 = xc - r1;
    *y1 = yc - r2;
    *x2 = xc + r1;
    *y2 = yc + r2;

/*
    special case for a complete arc with no rotation
*/
    if (rotang == 0.0f) {
        if (anglen >= 360.f  ||  anglen <= -360.f) {
            return 1;
        }
    }

/*
    general case for partial arc or rotated full arc
    generate the arc points and find the limits of the set of points
*/

/*
    get points for the arc
*/
    istat = gpf_calcarcpoints (r1, r2, ang1, anglen, 128, flag, xyw, &nout);
    if (istat != 1) {
        return -1;
    }

/*
    separate into x and y arrays
*/
    nout /= 2;
    istat = gpf_xyseparate (xyw, nout, xy2, &memflg);
    if (istat == -1) {
        return -1;
    }

/*
    rotate and translate the points
*/
    gpf_rotatepoints (xy2[0], xy2[1], nout, rotang);
    gpf_translatepoints (xy2[0], xy2[1], nout, xc, yc);

/*
    find limits of final set of points
*/
    gpf_xandylimits (xy2[0], xy2[1], nout, x1, y1, x2, y2);

/*
    csw_Free memory for separated arrays if needed
*/
    return 1;

}  /*  end of function gpf_arclimits  */


/*
  ****************************************************************

                g p f _ m a k e o r i g i n b o x

  ****************************************************************

    Calculate points for a rectangle of the specified width and
  height centered at 0,0.  The x and y arrays must have space
  for at least 5 CSW_Fs each.  The function always returns 1.

*/

int GPFShape::gpf_makeoriginbox (CSW_F width, CSW_F height, CSW_F *x, CSW_F *y)
{
    CSW_F            w2, h2;

    w2 = width / 2.f;
    h2 = height / 2.f;

    x[0] = -w2;
    x[1] = -w2;
    x[2] = w2;
    x[3] = w2;
    x[4] = -w2;
    
    y[0] = -h2;
    y[1] = h2;
    y[2] = h2;
    y[3] = -h2;
    y[4] = -h2;

    return 1;

}  /*  end of function gpf_makeoriginbox  */



/*
  ****************************************************************

               g p f _ c u r v e b o x c o r n e r s

  ****************************************************************

  function name:    gpf_curveboxcorners       (int)

  call sequence:    gpf_curveboxcorners (xr, yr, crad, xout, yout, nout)

  purpose:          Given a rectangle, return a polygon that has the 
                    rectangles corners rounded at radius crad.

  return value:     status code

                    1 = normal successful completion
                   -1 = one of the output pointers is NULL

  calling parameters:

    xr        r      CSW_F    array of rectangle x coordinates from lower left,
                              clockwise around the rectangle
    yr        r      CSW_F    array of rectangle y coordinates from lower left,
                              clockwise around the rectangle
    crad      r      CSW_F    radius of the corner curves
                              if this is bigger than half the smallest dimension
                              of the rectangle, it is set to half the smallest 
                              dimension.  It is assumed that crad is positive
                              and significant relative to the rectangle size.
    xout      w      CSW_F**  pointer with the x coordinates of the curved box
    yout      w      CSW_F**  pointer with the y coordinates of the curved box

                              xout and yout must be csw_Freed by the calling function
                              when they are no longer in use.

    nout      w      int*     number of points in xout, yout

*/

int GPFShape::gpf_curveboxcorners (CSW_F *xr, CSW_F *yr, CSW_F crad, 
                         CSW_F **xout, CSW_F **yout, int *nout)
{
    int              i, j, nq, narc, nt;
    CSW_F            wide, high, tiny, *x = NULL, *y = NULL, 
                     xc[300], yc[300], x0, y0;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (x);
            csw_Free (y);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    adjust crad if it is too big
*/
    wide = xr[2] - xr[0];
    high = yr[1] - yr[0];
    wide /= 2;
    high /= 2;

    if (crad > wide) crad = wide;
    if (crad > high) crad = high;

/*
    figure out a reasonable number of points for the curves
*/
    tiny = wide + high ;
    tiny /= 200;

    narc = (int)(crad * TWOPI / tiny);
    if (narc < 30) narc = 30;
    if (narc > 256) narc = 256;
   
/*
    allocate output array space
*/
    x = (CSW_F *)csw_Malloc ((narc + 20) * sizeof(CSW_F)) ;
    if (!x) {
        return -1;
    }
    y = (CSW_F *)csw_Malloc ((narc + 20) * sizeof(CSW_F)) ;
    if (!y) {
        return -1;
    }

/*
    return input points only if crad is very small
*/
    if (crad < tiny) {
        csw_memcpy ((char *)x, (char *)xr, 5 * sizeof(CSW_F));
        csw_memcpy ((char *)y, (char *)yr, 5 * sizeof(CSW_F));
        xout[0] = x;
        yout[0] = y;
        *nout = 5;
        bsuccess = true;
        return 1;
    }

/*
    calculate a circle with crad radius
*/
    gpf_fastcirclegen (crad, narc, xc, yc, &nt);
    nq = nt / 4;
    if (nq < 0) nq = 0;

/*
    points for right side and upper right curve
*/
    x[0] = xr[2];
    y[0] = yr[0] + crad;
    x[1] = x[0];
    y[1] = yr[1] - crad;
    
    j = 2;
    x0 = xr[2] - crad;
    y0 = yr[1] - crad;
    
    for (i=0; i<nq; i++) {
        x[j] = x0 + xc[i];
        y[j] = y0 + yc[i];
        j++;
    }

/*
    points for top side and upper left curve
*/
    x[j] = xr[0] + crad;
    y[j] = yr[1];
    j++;

    x0 = xr[0] + crad;
    y0 = yr[1] - crad;

    for (i=nq; i<nq*2; i++) {
        x[j] = x0 + xc[i];
        y[j] = y0 + yc[i];
        j++;
    }

/*
    points for left side and lower left curve
*/
    x[j] = xr[0];
    y[j] = yr[0] + crad;
    j++;

    x0 = xr[0] + crad;
    y0 = yr[0] + crad;

    for (i=nq*2; i<nq*3; i++) {
        x[j] = x0 + xc[i];
        y[j] = y0 + yc[i];
        j++;
    }

/*
    points for bottom side and lower right curve
*/
    x[j] = xr[2] - crad;
    y[j] = yr[0];
    j++;

    x0 = xr[2] - crad;
    y0 = yr[0] + crad;

    for (i=nq*3; i<nq*4; i++) {
        x[j] = x0 + xc[i];
        y[j] = y0 + yc[i];
        j++;
    }

/*
    return the points in xout, yout, nout
*/
    xout[0] = x;
    yout[0] = y;
    *nout = j;

    bsuccess = true;

    return 1;

}  /*  end of function gpf_curveboxcorners  */



/*
  ****************************************************************

               g p f _ f a s t c i r c l e g e n

  ****************************************************************

  function name:    gpf_fastcirclegen                (int)

  call sequence:    gpf_fastcirclegen (r, n, x, y, nout)

  purpose:          Calculate x,y points along a full circular arc
                    centered at 0,0.  The number of points returned
                    will be the power of 2 just less than the maximum
                    number of points requested, up to 1024 points.  
                    The circle starts at a point ccw from the +x axis
                    and circles ccw with the last point on the +x axis.

  return value:     always returns 1

  calling parameters:

    r          r     CSW_F      radius of circle
    n          r     int        maximum number of points in circle
    x          w     CSW_F*     array of x coordinates of circle
    y          w     CSW_F*     array of y coordinates of circle
    nout       w     int*       actual number of points returned

*/

int GPFShape::gpf_fastcirclegen (CSW_F r, int n, CSW_F *x, CSW_F *y, int *nout)
{
    double      *xc = p_xc,
                *yc = p_yc;
    int         i, j, nskip, istart;
    double      darc, ang;

    darc = TWOPI / (CSW_F)MAXFASTCIRCLE;

/*
    first time called, calculate points on a unit circle
*/

    if (p_first) {
        ang = darc;
        for (i=0; i<MAXFASTCIRCLE; i++) {
            xc[i] = cos (ang);
            yc[i] = sin (ang);
            ang += darc;
        }
        p_first = 0;
    }

/*
    figure out how many points to output
*/
    i = 1;
    while (i < n  &&  i <= MAXFASTCIRCLE) {
        i *= 2;
    }
    i /= 2;
    if (i < 1) {
        i = 1;
    }
    nskip = MAXFASTCIRCLE / i;

/*
    calculate output points
*/

    istart = nskip - 1;
    j = 0;
    for (i=istart; i<MAXFASTCIRCLE; i+=nskip) {
        x[j] = (CSW_F)(xc[i] * r);
        y[j] = (CSW_F)(yc[i] * r);
        j++;
    }

    *nout = j;

    return 1;
    
}  /*  end of function gpf_fastcirclegen  */



/*
  ****************************************************************

                   g p f _ r o t a t e p o i n t s

  ****************************************************************

    Rotate an array of points around angle ang, expressed in degrees
  ccw from the +x axis.

*/

int GPFShape::gpf_rotatepoints (CSW_F *x, CSW_F *y, int npt, CSW_F ang)
{
    double            dang, sang, cang;
    CSW_F             xt;
    int               i;

/*
    check for nearly zero angle
*/

    if (ang > -0.01f  &&  ang < 0.01f) {
        return 1;
    }

    dang = (double)ang * DTORAD;
    sang = sin (dang);
    cang = cos (dang);

/*
    rotate the points
*/

    for (i=0; i<npt; i++) {
        xt = x[i];
        x[i] = (CSW_F)(xt * cang - y[i] * sang);
        y[i] = (CSW_F)(xt * sang + y[i] * cang);
    }

    return 1;

}  /*  end of function gpf_rotatepoints  */



/*
  ****************************************************************

             g p f _ t r a n s l a t e p o i n t s

  ****************************************************************

    Add x0,y0 to each point.

*/

int GPFShape::gpf_translatepoints (CSW_F *x, CSW_F *y, int npt, 
                         CSW_F x0, CSW_F y0)
{
    int             i;

    for (i=0; i<npt; i++) {
        x[i] += x0;
        y[i] += y0;
    }

    return 1;

}  /*  end of function gpf_translatepoints  */



/*
  ****************************************************************

                    g p f _ f u d g e a n g l e

  ****************************************************************

    convert an angle in degrees to be between zero and mod degrees.

*/

int GPFShape::gpf_fudgeangle (CSW_F *ang, CSW_F mod)
{
    CSW_F              tang;

    tang = *ang;
    while (tang < 0.0f) {
        tang += 360.f;
    }

    while (tang > mod) {
        tang -= 360.f;
    }

    *ang = tang;

    return 1;

}  /*  end of function gpf_fudgeangle  */





/*
******************************************************************

                  g p f _ c a l c a r c p o i n t s

******************************************************************

  function name:    gpf_calcarcpoints        (int)

  call sequence:    gpf_calcarcpoints (r1, r2, ang1, ang2, nmax, fillflag,
                                       xy, npts)

  purpose:          Convert an elliptical arc into a series of line segments.
                    The xy coordinates are relative to an origin at the 
                    center of the arc.

  return value:     status code

                    0 = error in calling parameters
                        (*npts is set to zero in this case)
                    1 = normal successful completion

  calling parameters:

    r1          r    CSW_F       x axis radius of the arc
    r2          r    CSW_F       y axis radius of the arc
                                 both r1 and r2 must be greater than 0
    ang1        r    CSW_F       angular start of the arc in degrees
    ang2        r    CSW_F       angular length of the arc in degrees
                                 (+ is ccw, - is clockwise)
    nmax        r    int         maximum number of CSW_Fing point numbers to return
    fillflag    r    int         flag for if this is a filled arc.
                                 1 = pie slice fill
                                 2 = chord fill
                                 anything else, no fill
    xy          w    CSW_F*      array to hold x,y x,y output coordinates
    npts        w    int*        number of values in xy

*/

int GPFShape::gpf_calcarcpoints (CSW_F r1, CSW_F r2, CSW_F ang1, CSW_F ang2,
                       int nmax, int fillflag,
                       CSW_F *xy, int *npts)
{
    CSW_F       *x = NULL, *y = NULL, *xyp[1];
    int         istat, nout, memflg = 0;

    xyp[0] = NULL;

    auto fscope = [&]()
    {
        csw_Free (x);
        if (memflg == 1) {
            csw_Free (xyp[0]);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    allocate work space memory
*/
    x = (CSW_F *)csw_Malloc (nmax * 2 * sizeof (CSW_F) );
    if (!x) {
        return -1;
    }
    y = x + nmax;

    istat = gpf_calcarcpoints2 (r1, r2, ang1, ang2, nmax, fillflag,
                                x, y, &nout);

/*
    pack back into xy array
*/
    memflg = 0;
    if (istat == 1) {
        gpf_packxy (x, y, nout, xyp, &memflg);
        csw_memcpy ((char *)xy, (char *)xyp[0], nout * 2 * sizeof(CSW_F));
        if (memflg == 1) {
            csw_Free (xyp[0]);
            xyp[0] = NULL;
        }
    }
   
    *npts = nout * 2;

    return istat;

}  /*  end of function gpf_calcarcpoints  */ 





/*
  ****************************************************************

                g p f _ c a l c a r c p o i n t s 2

  ****************************************************************

  function name:    gpf_calcarcpoints2         (int)

  call sequence:    gpf_calcarcpoints2 (r1, r2, ang1in, anglen, nmax, fillflag,
                                        xout, yout, nout)

  purpose:          calculate the points along an arc and output them in
                    arrays of x and y coordinates.  The coordinates are
                    based on an origin at 0,0 and the r1 axis in the x direction.

  return value:     status code

                    1 = normal successful completion
                   -1 = memory allocation error

  calling parameters:

    r1          r     CSW_F        x axis radius of the arc
    r2          r     CSW_F        y axis radius of the arc
    ang1in      r     CSW_F        start angle of arc in degrees
    anglen      r     CSW_F        angular length in degrees
    nmax        r     int          maximum points in output
    fillflag    r     int          1 = pie slice
                                   2 = chord
                                   anything else, unclosed arc
    xout        w     CSW_F*       x coordinates of output 
    yout        w     CSW_F*       y coordinates of output
    nout        w     int*         number of output points

*/

int GPFShape::gpf_calcarcpoints2 (CSW_F r1, CSW_F r2, CSW_F ang1in, CSW_F anglen, 
                        int nmax, int fillflag,
                        CSW_F *xout, CSW_F *yout, int *nout)
{
    int           i, j, k, i1, i2, j1, j2, narc, nq;
    CSW_F         pct, *x = NULL, *y = NULL, ang1, ang2, tang, dang, a1, 
                  rat1, r12, r22, rat, tiny;


    auto fscope = [&]()
    {
        csw_Free (x);
    };
    CSWScopeGuard func_scope_guard (fscope);


    ang1 = ang1in;
    if (anglen > 359.99f) ang1 = 0.0f; 
    
/*
    allocate work space memory
*/
    x = (CSW_F *)csw_Malloc (nmax * 2 * sizeof(CSW_F));
    if (!x) {
        return -1;
    }
    y = x + nmax;

/*
    get a circle radius r1 centered at 0,0
*/
    gpf_fastcirclegen (r1, nmax-5, x, y, &narc);

/*
    transform circle into an ellipse
*/
    tiny = (r1 + r2) / 200.f;
    pct = r1 - r2;

    if (pct < -tiny  ||  pct > tiny) {

        nq = narc / 4;
        r22 = r2 * r2;
        r12 = r1 * r1;
        rat = r22/r12;

/*  
        first and second quadrants
*/
        for (i=0; i<nq * 2 - 1; i++) {
            y[i] = r22 - rat * x[i] * x[i];
            y[i] = (CSW_F)(sqrt ((double)y[i]));
        }
        y[nq * 2 - 1] = 0.f;

/*
        third and fourth quadrants
*/
        for (i=nq*2; i<nq*4 - 1; i++) {        
            y[i] = r22 - rat * x[i] * x[i];
            y[i] = (CSW_F)(-sqrt ((double)y[i]));
        }
        y[nq * 4 - 1] = 0.f;

    }

/*
    output if full ellipse wanted
*/
    if (anglen >= 359.99f  ||  anglen <= -359.99f) {
        x[narc] = x[0];
        y[narc] = y[0];
        narc++;
        *nout = narc;
        csw_memcpy ((char *)xout, (char *)x, narc * sizeof(CSW_F));
        csw_memcpy ((char *)yout, (char *)y, narc * sizeof(CSW_F));
        return 1;
    }

/*
    output partial arc of ellipse

    convert start and end angles so that both are positive 
    and start angle is less than end angle
*/
    ang2 = ang1 + anglen;
    if (ang2 < ang1) {
        tang = ang1;
        ang1 = ang2;
        ang2 = tang;
    }

    dang = 360.f / narc;
    tang = ang2 - ang1;
    gpf_fudgeangle (&ang1, 360.f);
    ang2 = ang1 + tang;

/*
    interpolate for start of arc
*/
    i1 = (int)(ang1 / dang);
    a1 = i1 * dang;
    i2 = i1 + 1;
    if (i1 >= narc)
        i1 -= narc;
    if (i2 >= narc)
        i2 -= narc;
    rat1 = (ang1 - a1) / dang;
    
    j1 = i1 % narc;
    j2 = i2 % narc;
    j1--;
    j2--;
    if (j1 < 0)
        j1 += narc;
    if (j2 < 0)
        j2 += narc;

    xout[0] = (x[j2] - x[j1]) * rat1 + x[j1];
    yout[0] = (y[j2] - y[j1]) * rat1 + y[j1];

/*
    fill in middle of arc
*/
    j = 1;
    i2 = (int)(ang2 / dang);
    for (i=i1+1; i<=i2; i++) {
        k = i % narc;
        k--;
        if (k < 0) k = narc - 1;
        xout[j] = x[k];
        yout[j] = y[k];
        j++;
    }

/*
    interpolate for the end of the arc
*/

    i1 = (int)(ang2 / dang);
    a1 = i1 * dang;
    i2 = i1 + 1;
    if (i1 >= narc)
        i1 -= narc;
    if (i2 >= narc)
        i2 -= narc;
    
    rat1 = (ang2 - a1) / dang;;

    j1 = i1 % narc;
    j2 = i2 % narc;
    j1--;
    j2--;
    if (j1 < 0)
        j1 += narc;
    if (j2 < 0)
        j2 += narc;

    xout[j] = (x[j2] - x[j1]) * rat1 + x[j1];
    yout[j] = (y[j2] - y[j1]) * rat1 + y[j1];

    j++;

/*
    connect ends according to fillflag
*/

    if (fillflag == 1) {
        xout[j] = 0.f;
        yout[j] = 0.f;
        j++;
        xout[j] = xout[0];
        yout[j] = yout[0];
        j++;
    }

    else if (fillflag == 2) {
        xout[j] = xout[0];
        yout[j] = yout[0];
        j++;
    }

    *nout = j;

    return 1;

}  /*  end of function gpf_calcarcpoints2  */





/*
  ****************************************************************

           g p f _ c a l c _ p o i n t _ o n _ a r c

  ****************************************************************

    Given arc parameters and an angle, calculate the x and y coordinates
  of the point on the arc at that angle.  This is an internal utility
  function.  The angle and rotation angle must be in radians.

*/

int GPFShape::gpf_calc_point_on_arc (CSW_F xc, CSW_F yc, CSW_F r1, CSW_F r2, 
                           CSW_F rotang, CSW_F angle,
                           CSW_F *x, CSW_F *y)
{
    CSW_F         xt, yt, r12, r22, rat, yt2, cang, sang;

/*
    First, calculate the point as if the arc is a circle.
*/
    xt = (CSW_F)(r1 * cos ((double)angle));
    yt = (CSW_F)(r1 * sin ((double)angle));

    if (r1 - r2  ==  0.0) {
        *x = xt + xc;
        *y = yt + yc;
        return 1;
    }

/*
    stretch the point to the ellipse.
*/
    r22 = r2 * r2;
    r12 = r1 * r1;
    rat = r22 / r12;

    yt2 = r22 - rat * xt * xt;
    if (yt2 < 0.0f)
        yt2 = 0.0f;
    yt2 = (CSW_F)(sqrt ((double)yt2));
    if (yt < 0.0f) yt2 = -yt2;

    if (rotang == 0.0f) {
        *x = xt + xc;
        *y = yt2 + yc;
        return 1;
    }

/*
    rotate the point.
*/
    cang = (CSW_F)(cos ((double)rotang));
    sang = (CSW_F)(sin ((double)rotang));
    *x = xt * cang - yt2 * sang + xc;
    *y = xt * sang + yt2 * cang + yc;

    return 1;

}  /*  end of function gpf_calc_point_on_arc  */





/*
  ****************************************************************

                 g p f _ b o x l l t o c e n t e r

  ****************************************************************

    Convert a box lower left coordinates to center coordinates.

*/

int GPFShape::gpf_boxlltocenter (CSW_F x, CSW_F y, CSW_F w, CSW_F h, CSW_F ang, 
                       CSW_F *xc, CSW_F *yc)
{
    double        dang, cang, sang;
    CSW_F         w2, h2, xt, yt;

    w2 = w / 2.f;
    h2 = h / 2.f;

    if (ang == 0.0f) {
        *xc = x + w2;
        *yc = y + h2;
        return 1;
    }

    dang = ang * DTORAD;
    cang = cos (dang);
    sang = sin (dang);

    xt = (CSW_F)(w2 * cang - h2 * sang);
    yt = (CSW_F)(w2 * sang + h2 * cang);

    *xc = x + xt;
    *yc = y + yt;

    return 1;

}  /*  end of function gpf_boxlltocenter  */



/*
  ****************************************************************

                g p f _ b o x c e n t e r t o l l

  ****************************************************************

    Convert a box center coordinates to lower left coordinates.

*/

int GPFShape::gpf_boxcentertoll (CSW_F xc, CSW_F yc, CSW_F w, CSW_F h, CSW_F ang, 
                       CSW_F *x, CSW_F *y)
{
    double        dang, cang, sang;
    CSW_F         w2, h2, xt, yt;

    w2 =  -w / 2.f;
    h2 =  -h / 2.f;

    if (ang == 0.0f) {
        *x = xc + w2;
        *y = yc + h2;
        return 1;
    }

    dang = ang * DTORAD;
    cang = cos (dang);
    sang = sin (dang);

    xt = (CSW_F)(w2 * cang - h2 * sang);
    yt = (CSW_F)(w2 * sang + h2 * cang);

    *x = xc + xt;
    *y = yc + yt;

    return 1;

}  /*  end of function gpf_boxcentertoll  */


/*
  ****************************************************************

                   g p f _ b o x l i m i t s

  ****************************************************************

*/

int GPFShape::gpf_boxlimits (CSW_F xc, CSW_F yc, CSW_F w, CSW_F h, CSW_F ang,
                   CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{
    CSW_F       xr[6], yr[6];

    gpf_makeoriginbox (w, h, xr, yr);
    gpf_rotatepoints (xr, yr, 5, ang);
    gpf_translatepoints (xr, yr, 5, xc, yc);
    gpf_xandylimits (xr, yr, 5,
                     x1, y1, x2, y2);

    return 1;

}  /*  end of function gpf_boxlimits  */


/*
  ****************************************************************

                 g p f _ c a l c b o x p o i n t s

  ****************************************************************

*/

int GPFShape::gpf_calcboxpoints (CSW_F xc, CSW_F yc, CSW_F w, CSW_F h, CSW_F ang,
                       CSW_F *xr, CSW_F *yr)
{

    gpf_makeoriginbox (w, h, xr, yr);
    gpf_rotatepoints (xr, yr, 5, ang);
    gpf_translatepoints (xr, yr, 5, xc, yc);

    return 1;

}  /*  end of function gpf_calcboxpoints  */
