
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_thinxy.cc

    This file contains utilities for rearranging xy coordinate
    arrays into packed, unpacked, thinned out, etc. modes.

*/

#include <string.h>
#include <ctype.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/gpf_utils.h"



/*
  ****************************************************************

                     g p f _ x y t h i n 1

  ****************************************************************

  function name:    gpf_xythin1          (int)

  call sequence:    gpf_xythin1 (x, y, nin, dist1, dist2, qflag,
                                 xout, yout, nout)

  purpose:          remove points from a polyline that do not 
                    contribute to the shape of the line

  return value:     status code

                    0 = bad parameter passed
                    1 = normal successful completion

  calling parameters:

    x           r    CSW_F*     x coordinates of polyline
    y           r    CSW_F*     y coordinates of polyline
    nin         r    int        number of points in input polyline
                                nin must be > 2
    dist1       r    CSW_F      minimum interpoint distance
    dist2       r    CSW_F      minimum out of line distance
                                dist1 and dist2 must be > 0.0
    qflag       r    int        flag for quick thin only
                                0 = do not do quick thin
                                1 = do quick thin
    xout        w    CSW_F*     output x coordinates
    yout        w    CSW_F*     output y coordinates
    nout        w    int*       number of output points

        note:  xout and yout may be exactly the same pointers 
               as x and y, but they cannot otherwise overlap 
               the x and y arrays.

*/

int gpf_xythin1 (CSW_F *x, CSW_F *y, int nin, CSW_F dist1, CSW_F dist2, 
                 int qflag, CSW_F *xout, CSW_F *yout, int *nout)
{
    int           i, n, istat;
    CSW_F         dx, dy, slope, yint, xt1, yt1, eps;
    double        xdum, ydum;
    int           ntot, ndo;

    CSWPolyUtils  ply_utils_obj;
    
/*  return for obvious errors  */

    if (nin < 3  ||  dist1 <= 0.0f) {
        for (i=0; i<nin; i++) {
            xout[i] = x[i];
            yout[i] = y[i];
        }
        *nout = nin;
        return 0;
    }

/*  remove close adjacent points  */

    n = 0;
    xout[0] = x[0];
    yout[0] = y[0];
    for (i=1; i<nin-1; i++) {
        dx = x[i] - xout[n];
        if (dx < -dist1  ||  dx > dist1) {
            n++;
            xout[n] = x[i];
            yout[n] = y[i];
        }    
        else {
            dy = y[i] - yout[n];
            if (dy < -dist1  ||  dy > dist1) {
                n++;
                xout[n] = x[i];
                yout[n] = y[i];
            }    
        }
    }

    n++;
    xout[n] = x[nin-1];
    yout[n] = y[nin-1];
    ntot = n + 1;

    if (ntot < 3) {
        *nout = ntot;
        return 1;
    }

    if (qflag) {
        if (ntot > nin / 2) {
            *nout = ntot;
            return 1;
        }
    }

    if (dist2 <= 0.0f) {
        *nout = ntot;
        return 1;
    }

/*  remove colinear points  */

    ndo = 0;
    eps = dist2 / 100.f;
    while (ndo < 20) {
        i = 1;
        n = 1;

        for (;;) {
            if (i == ntot - 1) {
                xout[n] = xout[i];
                yout[n] = yout[i];
                n++;
                break;
            }
            xt1 = xout[i-1];
            yt1 = yout[i-1];
            dx = xout[i+1] - xt1;
            dy = yout[i+1] - yt1;
            if (dx > -eps  &&  dx < eps) {
                dx = xout[i] - xt1;
                if (dx < 0.0f) dx = -dx;
            }
            else {
                slope = dy / dx;
                yint = yt1 - slope * xt1;
                if (slope < 1.0f  &&  slope > -1.0f) {
                    dx = yout[i] - xout[i] * slope - yint;
                    if (dx < 0.0f) dx = -dx;
                }
                else {
                    dx = xout[i] - (yout[i] - yint) / slope;
                    if (dx < 0.0f) dx = -dx;
                }    
            }
           
        /*
            Output if the segments are not colinear.
        */ 
            if (dx > dist2) {
                xout[n] = xout[i];
                yout[n] = yout[i];
                n++;
            }

        /*
            Check for overlapping segments if
            the segments appear to be colinear.
            If the segments overlap, output them.
        */
            else {
                istat = ply_utils_obj.ply_segint (
                                    (double)xout[i-1], (double)yout[i-1],
                                    (double)xout[i], (double)yout[i],
                                    (double)xout[i], (double)yout[i],
                                    (double)xout[i+1], (double)yout[i+1],
                                    &xdum, &ydum);
                if (istat == 3  ||  istat == 4) {
                    xout[n] = xout[i];
                    yout[n] = yout[i];
                    n++;
                }
            }
            i++;
            xout[n] = xout[i];
            yout[n] = yout[i];
            n++;
            i++;
            if (i > ntot-1) {
                break;
            }
        }
        if (ntot == n) {
            break;
        }
        ntot = n;
        if (ntot < 3) {
            break;
        }
    }

    *nout = ntot;
    return 1;

}  /*  end of function gpf_xythin1  */



/*
  ****************************************************************

                    g p f _ x y x y t h i n 1

  ****************************************************************

  function name:    gpf_xyxythin1     (int)

  call sequence:    gpf_xyxythin1 (xy, nin, dist1, dist2, qflag,
                                   xyout, nout)

  purpose:          Thin an array of x,y points that are packed
                    together.  The function unpacks the points
                    and calls gpf_xythin1 to do the thinning.

  return value:     status code

                    0 = bad parameter passed
                    1 = normal successful completion

  calling parameters:

    xy          r    CSW_F*     array of packed x,y coordinates
    nin         r    int        number of points in xy
    dist1       r    CSW_F      minimum interpoint spacing
    dist2       r    CSW_F      minimum colinear offset
    qflag       r    int        quick thin flag
                                0 = no quick thin
                                anything else, do quick thin
    xyout       w    CSW_F*     output packed x,y coordinates
    nout        w    int*       number of output points

*/

int gpf_xyxythin1 (CSW_F *xy, int nin, CSW_F dist1, CSW_F dist2, 
                   int qflag, CSW_F *xyout, int *nout)
{
    int           i, j;
    int           memflg = 0, jstat, istat;
    CSW_F         *xy1[1], *xy2[2],
                  *xt = NULL, *yt = NULL, *xyt = NULL;

    xy1[0] = NULL;
    xy2[0] = NULL;
    xy2[1] = NULL;

    auto fscope = [&]()
    {
        csw_Free (xt);
        if (memflg) {
            csw_Free (xy1[0]);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    memflg = 0;

    istat = gpf_xyseparate (xy, nin, xy2, &memflg);

    if (istat != 1) {
        j = 0;
        for (i=0; i<nin; i++) {
            xyout[j] = xy[j];
            j++;
            xyout[j] = xy[j];
            j++;
        }
        *nout = nin;
        return 1;
    }

    xt = (CSW_F *)csw_Malloc (nin * 2 * sizeof(CSW_F));
    if (!xt) {
        j = 0;
        for (i=0; i<nin; i++) {
            xyout[j] = xy[j];
            j++;
            xyout[j] = xy[j];
            j++;
        }
        *nout = nin;
        return 1;
    }
    yt = xt + nin;

    istat = gpf_xythin1 (xy2[0], xy2[1], nin, dist1, dist2, qflag,
                         xt, yt, nout);
    if (memflg) {
        csw_Free (xy2[0]);
        csw_Free (xy2[1]);
    }

    jstat = gpf_packxy (xt, yt, *nout, xy1, &memflg);
    if (jstat != 1) {
        j = 0;
        for (i=0; i<nin; i++) {
            xyout[j] = xy[j];
            j++;
            xyout[j] = xy[j];
            j++;
        }
        *nout = nin;
        return 1;
    }

    j = 0;
    xyt = xy1[0];
    for (i=0; i<*nout; i++) {
        xyout[j] = xyt[j];
        j++;
        xyout[j] = xyt[j];
        j++;
    }

    return istat;

}  /*  end of function gpf_xyxythin1  */




/*
******************************************************************

                   g p f _ x y s e p a r a t e

******************************************************************

  function name:    gpf_xyseparate       (int)

  call sequence:    gpf_xyseparate (xy, npt, xy2, memflg)

  purpose:          separate xy packed coordinates into x and y arrays

  return value:     status code

                    -1 = memory allocation error
                     0 = npt is less than 1
                     1 = normal successful completion

  calling parameters:

    xy       r    CSW_F*       array of x,y packed coordinates
    npt      r    int          number of input points
    xy2      w    CSW_F**      array of two CSW_F pointers
                               The first is the x, second is y
    memflg   w    int*         returns zero if static memory used
                               returns 1 if csw_Malloc used and the
                               calling function is responsible for
                               csw_Freeing the x and y pointers

*/

int gpf_xyseparate (CSW_F *xy, int npt, CSW_F **xy2, int *memflg)
{
    CSW_F       *x = NULL, *y = NULL;
    int         i;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (x);
            csw_Free (y);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    *memflg = 1;
 
    if (npt < 1) {
        return 0;
    }

    x = (CSW_F *)csw_Malloc (npt * sizeof(CSW_F));
    if (!x) {
        return -1;
    }
    y = (CSW_F *)csw_Malloc (npt * sizeof(CSW_F));
    if (!y) {
        return -1;
    }

    xy2[0] = x;
    xy2[1] = y;

    for (i=0; i<npt; i++) {
        x[i] = *xy++;
        y[i] = *xy++;
    }

    bsuccess = true;

    return 1;

}  /*  end of function gpf_xyseparate  */





/*
******************************************************************

                     g p f _ p a c k x y

******************************************************************

  function name:    gpf_packxy      (int)

  call sequence:    gpf_packxy (x, y, npts, xy, memflg)

  purpose:          Pack separate x and y arrays into a single x,y array.

  return value:     status code

                    -1 = memory allocation error
                     0 = npts is less than 1
                     1 = normal successful completion

  calling parameters:

    x       r    CSW_F*     x coordinate array
    y       r    CSW_F*     y coordinate array
    npts    r    int        number of points in x and y
    xy      w    CSW_F**    pointer to x,y packed coordinates
    memflg  w    int*       flag for memory csw_Freeing
                            0 = do not csw_Free xy after use
                            1 = csw_Free xy after use

*/

int gpf_packxy (CSW_F *x, CSW_F *y, int npts, CSW_F **xy, int *memflg)
{
    register int       i;
    CSW_F              *xyt = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xyt);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    *memflg = 0;

    if (npts < 1) {
        return 0;
    }

/*  allocate memory for output  */

    xyt = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
    if (!xyt) {
        return -1;
    }
    *memflg = 1;

    xy[0] = xyt;

/*  pack output  */

    for (i=0; i<npts; i++) {
        *xyt++ = x[i];
        *xyt++ = y[i];
    }

    bsuccess = true;

    return 1;

}  /*  end of function gpf_packxy  */



/*
******************************************************************

         g p f _ c o m p r e s s f i l l p o i n t s

******************************************************************

  function name:    gpf_compressfillpoints         (int)

  call sequence:    gpf_compressfillpoints (xyin, npt, xy, np2, memflag)

  purpose:          copy points into array without the hole flags

  return value:     status code

                    -1 = memory allocation error
                     1 = normal successful completion

  calling parameters:

    xyin      r    CSW_F*       input x,y points
    npt       r    int          number of input points
    xy        w    CSW_F**      output points
    np2       w    int*         number of output points
    memflag   w    int*         0 = no csw_Malloc,  1 = csw_Malloc

*/

int gpf_compressfillpoints (CSW_F *xyin, int npt, CSW_F **xy, 
                            int *np2, int *memflag)
{
    int         i, n;
    CSW_F       *xyt = NULL, big, *x = NULL, *y = NULL,
                *x2 = NULL, *y2 = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xyt);
            xy[0] = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    xy[0] = NULL;
    big = 1.e15f;

/*  allocate memory for output  */

    xyt = (CSW_F *)csw_Malloc (npt * 2 * sizeof(CSW_F));
    if (!xyt) {
        return -1;
    }
    *memflag = 1;

/*  copy all points except hole flag points to the output array  */

    n = 0;
    x = xyin;
    y = xyin + 1;
    x2 = xyt;
    y2 = xyt + 1;
    for (i=0; i<npt; i++) {
        if (*x < big  &&  *x > -big  &&  *y < big  &&  *y > -big) {
            *x2 = *x;
            *y2 = *y;
            if (i < npt-1) {
                x2 += 2;
                y2 += 2;
            }
            n++;
        }
        else {
            *np2 = n;
        }
        if (i < npt-1) {
            x += 2;
            y += 2;
        }
    }

    xy[0] = xyt;
    *np2 = n;

    bsuccess = true;

    return 1;

}  /*  end of function gpf_compressfillpoints  */





/*
******************************************************************

                     g p f _ p a c k x y 2

******************************************************************

  function name:    gpf_packxy2      (int)

  call sequence:    gpf_packxy2 (x, y, npts, xy)

  purpose:          Pack separate x and y arrays into a single x,y array.
                    The output array is allocated by the calling function.

  return value:     status code

                    -1 = memory allocation error
                     0 = npts is less than 1
                     1 = normal successful completion

  calling parameters:

    x       r    CSW_F*     x coordinate array
    y       r    CSW_F*     y coordinate array
    npts    r    int        number of points in x and y
    xy      w    CSW_F**    pointer to x,y packed coordinates

*/

int gpf_packxy2 (CSW_F *x, CSW_F *y, int npts, CSW_F *xy)
{
    register int       i;
    CSW_F              *xyt;

    xyt = xy;

/*  pack output  */

    for (i=0; i<npts; i++) {
        *xyt++ = x[i];
        *xyt++ = y[i];
    }

    return 1;

}  /*  end of function gpf_packxy2  */






/*
******************************************************************

                g p f _ p a c k x y _ d o u b l e

******************************************************************

  function name:    gpf_packxy2      (int)

  call sequence:    gpf_packxy2 (x, y, npts, xy)

  purpose:          Pack separate x and y arrays into a single x,y array.
                    The output array is allocated by the calling function.

  return value:     status code

                    -1 = memory allocation error
                     0 = npts is less than 1
                     1 = normal successful completion

  calling parameters:

    x       r    double*    x coordinate array
    y       r    double*    y coordinate array
    npts    r    int        number of points in x and y
    xy      w    CSW_F*     array with x,y packed coordinates

*/

int gpf_packxy_double (double *x, double *y, int npts, CSW_F *xy)
{
    int                i;
    CSW_F              *xyt;

    xyt = xy;

/*  pack output  */

    for (i=0; i<npts; i++) {
        *xyt++ = (CSW_F)x[i];
        *xyt++ = (CSW_F)y[i];
    }

    return 1;

}  /*  end of function gpf_packxy_double  */


/*
  ****************************************************************

                     g p f _ x y t h i n 2

  ****************************************************************

  function name:    gpf_xythin2          (int)

  call sequence:    gpf_xythin2 (x, y, tag, nin, dist1, dist2,
                                 xout, yout, tagout, nout)

  purpose:          remove points from a polyline that do not 
                    contribute to the shape of the line

  return value:     status code

                    0 = bad parameter passed
                    1 = normal successful completion

  calling parameters:

    x           r    double*    x coordinates of polyline
    y           r    double*    y coordinates of polyline
    tag         r    void**     array of void pointers to keep connected with the xy points
    nin         r    int        number of points in input polyline
                                nin must be > 2
    dist1       r    double     minimum interpoint distance
    dist2       r    double     minimum out of line distance
                                dist1 must be > zero
                                if dist2 <= 0, then only distance thinning is done
    xout        w    double*    output x coordinates
    yout        w    double*    output y coordinates
    tagout      w    void**     array of pointers connecred with output points
    nout        w    int*       number of output points

        note:  xout and yout may be exactly the same pointers 
               as x and y, but they cannot otherwise overlap 
               the x and y arrays.

*/

int gpf_xythin2 (double *x, double *y, void **tag, int nin,
                 double dist1, double dist2, 
                 double *xout, double *yout, void **tagout, int *nout)

{
    int           i, n, istat;
    double        dx, dy, slope, yint, xt1, yt1, eps;
    double        xdum, ydum;
    int           ntot, ndo;

    CSWPolyUtils  ply_utils_obj;
    
/*  return for obvious errors  */

    if (nin < 3  ||  dist1 <= 0.0f) {
        for (i=0; i<nin; i++) {
            xout[i] = x[i];
            yout[i] = y[i];
            tagout[i] = tag[i];
        }
        *nout = nin;
        return 0;
    }

/*  remove close adjacent points  */

    n = 0;
    xout[0] = x[0];
    yout[0] = y[0];
    tagout[0] = tag[0];

    for (i=1; i<nin-1; i++) {
        dx = x[i] - xout[n];
        if (dx < -dist1  ||  dx > dist1) {
            n++;
            xout[n] = x[i];
            yout[n] = y[i];
            tagout[n] = tag[i];
        }    
        else {
            dy = y[i] - yout[n];
            if (dy < -dist1  ||  dy > dist1) {
                n++;
                xout[n] = x[i];
                yout[n] = y[i];
                tagout[n] = tag[i];
            }    
        }
    }

    n++;
    xout[n] = x[nin-1];
    yout[n] = y[nin-1];
    tagout[n] = tag[nin-1];
    ntot = n + 1;

    if (ntot < 3) {
        *nout = ntot;
        return 1;
    }

    if (dist2 <= 0.0f) {
        *nout = ntot;
        return 1;
    }

/*  remove colinear points  */

    ndo = 0;
    eps = dist2 / 100.f;
    while (ndo < 20) {
        i = 1;
        n = 1;

        for (;;) {
            if (i == ntot - 1) {
                xout[n] = xout[i];
                yout[n] = yout[i];
                tagout[n] = tag[i];
                n++;
                break;
            }
            xt1 = xout[i-1];
            yt1 = yout[i-1];
            dx = xout[i+1] - xt1;
            dy = yout[i+1] - yt1;
            if (dx > -eps  &&  dx < eps) {
                dx = xout[i] - xt1;
                if (dx < 0.0f) dx = -dx;
            }
            else {
                slope = dy / dx;
                yint = yt1 - slope * xt1;
                if (slope < 1.0f  &&  slope > -1.0f) {
                    dx = yout[i] - xout[i] * slope - yint;
                    if (dx < 0.0f) dx = -dx;
                }
                else {
                    dx = xout[i] - (yout[i] - yint) / slope;
                    if (dx < 0.0f) dx = -dx;
                }    
            }
           
        /*
            Output if the segments are not colinear.
        */ 
            if (dx > dist2) {
                xout[n] = xout[i];
                yout[n] = yout[i];
                tagout[n] = tag[i];
                n++;
            }

        /*
            Check for overlapping segments if
            the segments appear to be colinear.
            If the segments overlap, output them.
        */
            else {
                istat = ply_utils_obj.ply_segint (
                                    (double)xout[i-1], (double)yout[i-1],
                                    (double)xout[i], (double)yout[i],
                                    (double)xout[i], (double)yout[i],
                                    (double)xout[i+1], (double)yout[i+1],
                                    &xdum, &ydum);
                if (istat == 3  ||  istat == 4) {
                    xout[n] = xout[i];
                    yout[n] = yout[i];
                    tagout[n] = tag[i];
                    n++;
                }
            }
            i++;
            xout[n] = xout[i];
            yout[n] = yout[i];
            tagout[n] = tag[i];
            n++;
            i++;
            if (i > ntot-1) {
                break;
            }
        }
        if (ntot == n) {
            break;
        }
        ntot = n;
        if (ntot < 3) {
            break;
        }
    }

    *nout = ntot;
    return 1;

}  /*  end of function gpf_xythin2  */
