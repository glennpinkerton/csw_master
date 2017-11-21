/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*

    gpf_spline.cc

    This file has functions used for smoothing polylines via cubic
    splines.  

*/

#include <math.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/gpf_spline.h"



/*
  ****************************************************************

                  g p f _ S p l i n e F i t 2

  ****************************************************************

    Get coordinates in packed form, separate and call gpf_SplineFit.
  Pack the return coordinates when done.

*/

int GPFSpline::gpf_SplineFit2 (CSW_F *xy, int nin, int flag,
                    CSW_F *xyout, int maxout, CSW_F dres, int *nout)
{
    int              memflag1 = 0, memflag2 = 0, n2, istat;
    CSW_F            *xy2[2], *xout = NULL, *yout = NULL, *xytmp = NULL;

    xy2[0] = NULL;
    xy2[1] = NULL;

    auto fscope = [&]()
    {
        if (memflag1) {
            csw_Free (xy2[0]);
            csw_Free (xy2[1]);
        }
        if (memflag2) {
            csw_Free (xytmp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

    istat = gpf_xyseparate (xy, nin, xy2, &memflag1);
    if (istat != 1) {
        return -1;
    }

    xout = xyout;
    yout = xout + maxout;

    istat = gpf_SplineFit (xy2[0], xy2[1], nin, flag,
                           xout, yout, maxout, dres, &n2);
    if (istat != 1) {
        return -1;
    }

    istat = gpf_packxy (xout, yout, n2, &xytmp, &memflag2);
    if (istat != 1) {
        return -1;
    }

    csw_memcpy (xyout, xytmp, n2 * 2 * sizeof(CSW_F));

    *nout = n2;

    return 1;
        
}  /*  end of function gpf_SplineFit2  */



/*
******************************************************************

                   g p f _ S p l i n e F i t 

******************************************************************

  function name:    gpf_SplineFit        (int)

  call sequence:    gpf_SplineFit (x, y, nin, flag,
                                   xout, yout, maxout, dres,
                                   nout)

  purpose:          Smooth through a sequence of x,y points using a
                    cubic spline.  The spline is evaluated at dres
                    distance intervals along the original polyline
                    to produce the smoothed line.  If maxout is too
                    small for dres resolution, the resolution is
                    decreased until less than maxout points will be
                    calculated.  The function must have at least 4
                    input points to interpolate.  If less than 4
                    points are input, the input is copied as is to 
                    the output and no interpolation is done.

  return value:     status code

                    -4 = all points are in the same location
                    -3 = nin less than 1
                    -2 = maxout is less than 2 * nin
                    -1 = memory allocation error
                     1 = normal successful completion

  calling parameters:

    x        r      CSW_F*        array of input x coordinates
    y        r      CSW_F*        array of input y coordinates
    nin      r      int           number of input points
    flag     r      int           flag for whether the polyline is closed.
                                  0 = let function decide
                                  1 = always treat as closed
                                  2 = always treat as unclosed
    xout     w      CSW_F*        output interpolated x coordinates
    yout     w      CSW_F*        output interpolated y coordinates
    maxout   r      int           maximum number of output points
                                  this must be at least 2 * nin and should
                                  be at least 4 to 5 times nin for good results.
    dres     r      CSW_F         resolution of interpolation in units
                                  of distance along the input polyline
                                  If this is less than or equal to zero, a
                                  resolution is determined based on maxout.
    nout     w      int*          number of interpolated points output

*/

int GPFSpline::gpf_SplineFit (CSW_F *x, CSW_F *y, int nin, int flag,
                   CSW_F *xout, CSW_F *yout, int maxout, CSW_F dres,
                   int *nout)
{
    int            i, nmax, ndo, n;
    CSW_F          *xwork = NULL, *ywork = NULL, *distwork = NULL,
                   *uwork = NULL, *y2work = NULL;
    double         dchk, dtot, xt, yt, tiny,
                   gx2, gy2;
    CSW_F          x1, y1, x2, y2;
    

    auto fscope = [&]()
    {
        csw_Free (distwork);   
    };
    CSWScopeGuard func_scope_guard (fscope);


/*  check obvious errors  */

    if (maxout < 1  ||  maxout < 2 * nin) {
        return -2;
    }
  
    if (nin < 1) {
        return -3;
    }

/*  return trivial solution if less than 4 points input  */

    if (nin < 4) {
        for (i=0; i<nin; i++) {
            xout[i] = x[i];
            yout[i] = y[i];
        }
        *nout = nin;
        return 1;
    }

/*  allocate workspace memory  */

    nmax = maxout + 2;
    distwork = (CSW_F *)csw_Malloc (nmax * 5 * sizeof (CSW_F));
    if (!distwork) {
        return -1;
    }
    
    uwork = distwork + nmax;
    y2work = uwork + nmax;
    xwork = y2work + nmax;
    ywork = xwork + nmax;

/*  find bounds of the input line  */

    x1 = 1.e30f;
    y1 = 1.e30f;
    x2 = -1.e30f;
    y2 = -1.e30f;
    gx2 = 1.e30f;
    gy2 = 1.e30f;
    for (i=0; i<nin; i++) {
        if (x[i] < x1)  x1 = x[i];
        if (x[i] > x2)  x2 = x[i];
        if (y[i] < y1)  y1 = y[i];
        if (y[i] > y2)  y2 = y[i];
    }
    tiny = (x2 - x1 + y2 - y1) / 50.f;

    if (tiny < 0.000001f) {
        return -4;
    }

/*  calculate "ghost" end points if the line is closed  */

    xt = x[0] - x[nin-1];
    xt = xt * xt;
    yt = y[0] - y[nin-1];
    yt = yt * yt;
    n = 0;
    if (sqrt(xt + yt) < tiny  ||  flag == 1) {
        if (flag == 2) {
            GhostFlag = 0;
        }    
        else {
            xwork[0] = x[nin-2];
            ywork[0] = y[nin-2];
            gx2 = x[1];
            gy2 = y[1];
            n++;
            GhostFlag = 1;
        }
    }
    else {
        GhostFlag = 0;
    }

/*  move input points and ghost points (if needed) into work arrays  */

    for (i=0; i<nin; i++) {
        xwork[n] = x[i];
        ywork[n] = y[i];
        n++;
    }

    if (gx2 < 1.e20f) {
        xwork[n] = (CSW_F)gx2;
        ywork[n] = (CSW_F)gy2;
        n++;
    }

    tiny /= 2000.f;

/*  calculate distances at each point and total distance along polyline  */

    dtot = 0.0f;
    distwork[0] = 0.0f;
    for (i=1; i<n; i++) {        
        xt = xwork[i] - xwork[i-1];
        xt = xt * xt;
        yt = ywork[i] - ywork[i-1];
        yt = yt * yt;
        dchk = sqrt (xt + yt);
        if (dchk < tiny) {
            dchk = tiny;
        }
        dtot += dchk;
        distwork[i] = (CSW_F)dtot;
    }

/*  adjust resolution of interpolation if needed  */

    if (dres <= 0.0f) {
        dres = (CSW_F)(dtot / (maxout - nin));
    }

    ndo = (int)(dtot / dres);
    while (ndo >= maxout - nin) {
        dres *= 1.25f;
        ndo = (int)(dtot / dres);
    }

/*  calculate the second derivatives of the x spline  */

    gpf_CalcSplineDeriv (distwork, xwork, n, y2work, uwork);

/*  evaluate the spline for x coordinates  */

    gpf_EvalSpline (distwork, xwork, n, y2work, dres, xout, nout);

/*  calculate the second derivatives of the y spline  */

    gpf_CalcSplineDeriv (distwork, ywork, n, y2work, uwork);

/*  evaluate the spline for y coordinates  */

    gpf_EvalSpline (distwork, ywork, n, y2work, dres, yout, nout);

    return 1;

}  /*  end of function gpf_SplineFit  */




/*
******************************************************************

              g p f _ C a l c S p l i n e D e r i v

******************************************************************

  function name:    gpf_CalcSplineDeriv        (int)

  call sequence:    gpf_CalcSplineDeriv (x, y, nin, yp1, ypn,
                                         y2, u)

  purpose:          Calculate the second derivatives for a cubic spline 
                    through a set of x,y points.  The x values must all
                    be increasing.  This function should only be called
                    from gpf_SplineFit for real polyline smoothing.
                    The gpf_SplineFit function sets up the parame-
                    tric forms for x and y as functions of distance and 
                    calls this function.

  return value:     always returns 1

  calling parameters:

    x          r      CSW_F*     array of input distance coordinates
    y          r      CSW_F*     array of input x or y values
    nin        r      int        number of points input
    y2         w      CSW_F*     array of nin 2nd derivatives
    u          r      CSW_F*     work array as big as x and y

*/

int GPFSpline::gpf_CalcSplineDeriv (CSW_F *x, CSW_F *y, int nin, 
                         CSW_F *y2, CSW_F *u)
{
    CSW_F          p, sig, un, qn;
    int            n;
    register int   i;

    n = nin;

/*  first point  */

    y2[0] = 0.f;
    u[0] = 0.f;

/*  interior points  */

    for (i=1; i<n-1; i++) {
        sig = (x[i]-x[i-1]) / (x[i+1]-x[i-1]);
        p = sig * y2[i-1] + 2;
        y2[i] = (sig-1.f) / p;
        u[i] = (6.f * ((y[i+1]-y[i]) / (x[i+1]-x[i]) - (y[i]-y[i-1]) / (x[i]-x[i-1]))
                     / (x[i+1]-x[i-1]) - sig * u[i-1]) / p;
    }

/*  last point  */

    qn = 0.f;
    un = 0.f;

    y2[n-1] = (un - qn*u[n-2]) / (qn * y2[n-2] + 1.f);

/*  backsubstitution loop  */

    for (i=n-2; i>=0; i--) {
        y2[i] = y2[i] * y2[i+1] + u[i];
    }

    return 1;

}  /*  end of function gpf_CalcSplineDeriv  */



/*
******************************************************************

                  g p f _ E v a l S p l i n e

******************************************************************

  function name:    gpf_EvalSpline        (int)

  call sequence:    gpf_EvalSpline (x, y, nin, y2, dxin,
                                    yout, nout)

  purpose:          Given the points and derivatives from gpf_CalcSplineDeriv,
                    evaluate the spline at aproximately dxin intervals along
                    the line.  This should only be called from gpf_SplineFit.
                    The output will contain all of the input points, and in 
                    addition, if the distance between two successive input
                    points is greater than 1.5 * dxin, the interval is divided
                    into equal length segments and evaluated at the intermediate
                    points.

  return value:     always returns 1

  calling parameters:

    x        r      CSW_F*    array of distances along polyline
    y        r      CSW_F*    array of x or y coordinates
    nin      r      int       number of points in polyline
    y2       r      CSW_F*    array of second derivatives as returned
                              by gpf_CalcSplineDeriv
    dxin     r      CSW_F     approximate evaluation interval in distance
                              units
    yout     w      CSW_F*    output array of x or y points
    nout     w      int*      number of output points.

*/

int GPFSpline::gpf_EvalSpline (CSW_F *x, CSW_F *y, int nin, 
                    CSW_F *y2, CSW_F dxin, CSW_F *yout, int *nout)
{
    register int     i, j;
    int              start, end, n, ndo;
    CSW_F            dx, dchk, dmin;
    CSW_F            a, b, xt, yt;

    start = 0;
    end = nin;
    if (GhostFlag) {
        start = 1;
        end = nin-1;
    }

    yout[0] = y[start];
    dmin = dxin * 1.5f;
    n = 1;

/*  loop through input points  */

    for (i=start+1; i<end; i++) {
        dchk = x[i] - x[i-1];

/*      interpolate the interval if needed  */

        if (dchk > dmin) {
            ndo = (int)(dchk / dxin + .5f);
            dx = dchk / ndo;
            for (j=1; j<ndo; j++) {
                xt = x[i-1] + j*dx;
                a = (x[i]-xt) / dchk;
                b = (xt - x[i-1]) / dchk;
                yt = a * y[i-1] + b * y[i] +
                     ((a*a*a - a) * y2[i-1] + (b*b*b - b) * y2[i]) * dchk * dchk / 6.f;
                yout[n] = yt;
                n++;
            }
        }

/*      include original endpoint in output  */

        yout[n] = y[i];
        n++;
    }

    *nout = n;

    return 1;

}  /*  end of function gpf_EvalSpline  */
