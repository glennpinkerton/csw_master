
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_traverse.cc

    This file has the methods for the CSWPolyTraverse class.
*/

/*
    system header files
*/
#include <math.h>
#include <string.h>

#include "csw/utils/include/csw_.h"

#include "csw/utils/private_include/gpf_utils.h"

#include "csw/utils/private_include/ply_utils.h"

#include "csw/utils/private_include/ply_traverse.h"
#include "csw/utils/private_include/ply_gridvec.h"
#include "csw/utils/private_include/ply_drivers.h"

#include "csw/utils/private_include/csw_scope.h"


/*  define macros for the file  */

#define LOOP                {xx1 = xx2;  yy1 = yy2;  continue;}
#define CONTINUE_TRAVERSE   ply_utils_obj.ply_intop('+', *xint, *yint, ndum); continue



/*
    ****************************************************************

                    p l y _ S e t M a x T r v O u t

    ****************************************************************
*/

int CSWPolyTraverse::ply_SetMaxTrvOut (int val)
{

    MaxTrvOut = val - 2;
    return 1;

}  /*  end of function ply_SetMaxTraverseOut  */




/*

    ***************************************************************

                       p l y _ t r v i n t

    ***************************************************************

  function name:    ply_trvint  (integer)

  call sequence:    ply_trvint (xvply, yvply, nvpt, startv, nextv, cflag,
                                xstart, ystart, iflag,
                                xpoly, ypoly, npoly, npts,
                                xminp, yminp, xmaxp, ymaxp,
                                xint, yint, npint, nsint,
                                xout, yout, nout, outflag)

  synopsis:         Traverse a polygon until a side of the polygon
                    intersects another polygon. 

  return values:    status code

                    0 = intersection found, returned in xint, yint,
                        npint, nsint
                    1 = no intersection found between startv and the
                        end of the polygon
                    3 = intersection found is at the final point
                        of a vector (exactly on a vertex)
                    4 = previous point found was on a vertex and
                        this traversal goes in the wrong direction
                        Usually means need to try again with the
                        opposite polygons as clip, source.
                    5 = the output arrays are exhausted.
                    6 = an intersection is found and returned in 
                        xint, yint but it does not fit the edge
                        rules for the current boolean operation

  usage:            This function is intended to be used as part of
                    the polygon boolean functions, and should not
                    be called outside of this context.  No parameter
                    error reporting is done here.  Any errors in
                    calling parameters will simply cause a status code
                    of 1 to be returned.
 
  algorithm:        The sides of the source polygon are decomposed
                    into separate vectors.  Each vector is tested to
                    see if it intersects with any of the sides of the
                    clipping polygon using a trivial rectangular
                    boundary test and function ply_sidint.  If an
                    intersection is found, it is returned and the
                    status code is returned as 0.  If no intersection
                    is found, the next vector on the source polygon is
                    tested.  If no intersection is found in the
                    remaining sides of the source polygon, the status
                    code is returned as 1.

  description of calling parameters:

    xvply    r   double*    pointer to array of source polygon x
                            coordinates
    yvply    r   double*    pointer to array of source polygon y
                            coordinates
    nvpt     r   int        number of coordinates in source polygon
    startv   r   int        first point in source polygon to use
    nextv    w   int     *  next point in source polygon after
                            intersection
    cflag    r   int        0 = traversing main source component
                            1 = traversing hole source component
    xstart   r   double     x coordinate of first point of the first 
                            vector to check on the source polygon
    ystart   r   double     y coordinate of first point of the first
                            vector to check on the source polygon
    iflag    r   int        flag setting stop of traversal
                            0 = traverse to end of polygon only
                            any other = wrap around end and traverse 
                            back to start point
    xpoly    r   double*[]  array of pointers to the x coordinates of 
                            components of the clipping polygon
    ypoly    r   double*[]  array of pointers to the y coordinates of
                            components of the clipping polygon
    npoly    r   int        number of components in the clipping
                            polygon
    npts     r   int     *  pointer to array which has the number of
                            vertices for each component of the
                            clipping polygon
    xminp    r   double*    pointer to array of minimum x coordinates
                            for each component of clipping polygon
    yminp    r   double*    pointer to array of minimum y coordinates
                            for each component of clipping polygon
    xmaxp    r   double*    pointer to array of maximum x coordinates
                            for each component of clipping polygon
    ymaxp    r   double*    pointer to array of maximum y coordinates
                            for each component of clipping polygon
    xint     w   double*    x coordinate of intersection point
    yint     w   double*    y coordinate of intersection point
    npint    w   int     *  component number of the clipping polygon
                            on which the intersection is found
    nsint    w   int     *  next point on the clipping polygon
                            component after the intersection point
    xout     w   double*    array of x coords for points traversed in 
                            finding the intersection
    yout     w   double*    array of y coords for points traversed in
                            finding the intersection
    nout     w   int     *  pointer to number of points written to 
                            xout and yout
    outflag  r   int        flag for outputing to xout and yout
                            outflag = 1, output to xout and yout
                            any other value, do not output

                 note:  if no intersection is found, the status code 
                        returned is 1 and xint, yint, npint, nsint,
                        xout, yout, and nout are meaningless 
  
*/

int CSWPolyTraverse::ply_trvint (double *xvply, double *yvply, 
                int nvpt, int startv, int *nextv, int cflag,
                double xstart, double ystart, int iflag,
                double **xpoly, double **ypoly, int npoly, int *npts,
                double *xminp, double *yminp, double *xmaxp, double *ymaxp,
                double *xint, double *yint, int *npint, int *nsint,
                double *xout, double *yout, int *nout, int outflag)
{
    int          i, j, i1, i2, i3, ifirst, jtest, istat, int_found,
                 ncomp, nvtot, nvploc, istatp, istatn, ndum, nc,
                 mult, jstatn, kstatp, startloopflag;
    double       x0, y0, x1, y1, x2, y2, xb1, yb1, xb2, yb2, x3, y3;
    double       xprv, yprv, xnxt, ynxt, xleft, yleft;
    double       savegraze;
    CSWPolyUtils  ply_utils_obj;

/*  check obvious errors  */

    if (nvpt < 3) return 1;
    if (startv < 0  ||  startv > nvpt) return 1;
    if (outflag != 1) outflag = 0;

    ndum = 0;
    istatp = -1;
    int_found = 0;

/*  save pointers and size of source polygon in class variables  */

    startloopflag = 0;

/*  open the polygon if it is closed  */

    nvploc = nvpt;
    while (ply_utils_obj.ply_graze(xvply[0], yvply[0],
            xvply[nvploc-1], yvply[nvploc-1])  &&
            nvploc > 1 ) {
        nvploc--;
    }
    if(nvploc < 2) {
        return 1;
    }

/*  check if this is the first point in the polygon  */

    if(startv > 0) {
        startv--;
        ifirst = 1;
    }
    else {
        ifirst = 0;
    }

/*  loop through the polygon sides remaining from startv to nvpt
    check each source polygon side for intersection with the
    clipping polygon  */

    ncomp = 0;
    nvtot = nvploc;
    if(iflag) {
        nvtot = nvploc + startv + 1;
    }

    kstatp = 0;
    for(i=startv; i < nvtot; i++) {

/*      generate vector from polygon side  */

        i1 = i;
        i2 = i+1;
        if(i2 == nvploc) {
            i2 = 0;
        }
        else if(i2 > nvploc) {
            i2 = i2 - nvploc;
            i1 = i2 -1;
        }
        while (i2 >= nvploc) {
            i2 = i2 - nvploc;
            if (i2 > 0) i1 = i2 - 1;
        }
        i3 = i2 + 1;
        if(i3 == nvploc) {
            i3 = 0;
        }
        x1 = xvply[i1];
        y1 = yvply[i1];
        x2 = xvply[i2];
        y2 = yvply[i2];
        x3 = xvply[i3];
        y3 = yvply[i3];
        if (i1 > 0) {
            x0 = xvply[i1-1];
            y0 = yvply[i1-1];
        }
        else {
            x0 = xvply[nvploc-1];
            y0 = yvply[nvploc-1];
        }

        if(outflag) {
            if (ncomp >= MaxTrvOut) {
                return 5;
            }
            if (PrevInOut == 1) {
                xout[ncomp] = x1;
                yout[ncomp] = y1;
                ncomp++;
            }
            if (ncomp >= MaxTrvOut) {
                return 5;
            }
            xout[ncomp] = x2;
            yout[ncomp] = y2;
            ncomp++;
            if (ncomp >= MaxTrvOut) {
                return 5;
            }
        }

/*      if not the first point in the source polygon, use xstart and
        ystart as the first point in the vector  */

        if (ifirst) {
            x1 = xstart;
            y1 = ystart;
            ifirst =0;
        }

/*      check if the vector is inside any clipping polygon component
        limits    */

        xb1 = x1;
        if(x2 < x1) xb1 = x2;
        yb1 = y1;
        if(y2 < y1) yb1 = y2;
        xb2 = x1;
        if(x1 < x2) xb2 = x2;
        yb2 = y1;
        if(y1 < y2) yb2 = y2;

        jtest = 0;
        
        for (j = 0; j < npoly; j++) {
            
            if(xb1 > xmaxp[j]  ||  yb1 > ymaxp[j]  ||
               xb2 < xminp[j]  ||  yb2 < yminp[j])    {
                continue;
            }
            else {
                jtest = 1;
                break;
            }
        }

/*      check the next vector if this one is not inside any bounds  */

        if(!jtest) {
            if(lastx2) {
                lastx2 = 0;
                return 4;
            }
            continue;
        }

/*      get vector intersection with the polygon if it exists  */

        istat = ply_sidint (xpoly, ypoly, npoly, npts,
                            x1, y1, x2, y2,
                            xint, yint, npint, nsint);

/*      continue the loop if no intersection is found  */

        if (istat == 1) {
            continue;
        }

/*
        if intersection is on the start point, continue the loop
*/
        if (istat == 2) {
            if(ply_utils_obj.ply_graze(*xint, *yint, xstart, ystart)) {
                if (!kstatp) {
                    kstatp = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, x2, y2);
                }
                continue;
            }
        }

/*
        if intersection other than x1, y1 is on start point, exit the loop
*/
        if (istat == 0  ||  istat == 3) {
            if(ply_utils_obj.ply_graze(*xint, *yint, xstart, ystart)) {
                startloopflag = 1;
                goto OUTPUT_AND_RETURN;
            }
        }

/*
        return if this intersection closes the component
*/
        if (ply_utils_obj.ply_graze (*xint, *yint, CloseX, CloseY)) {
            if (istat == 2) {
                ply_utils_obj.ply_CalcSmallOffset (x1, y1, x0, y0, &xprv, &yprv);
                ply_utils_obj.ply_CalcSmallOffset (x1, y1, x2, y2, &xnxt, &ynxt);
            }
            else if (istat == 0) {
                if (ply_utils_obj.ply_graze (x1, y1, *xint, *yint)) {
                    ply_utils_obj.ply_CalcSmallOffset (*xint, *yint, x0, y0, &xprv, &yprv);
                }
                else {
                    ply_utils_obj.ply_CalcSmallOffset (*xint, *yint, x1, y1, &xprv, &yprv);
                }
                if (ply_utils_obj.ply_graze (x2, y2, *xint, *yint)) {
                    ply_utils_obj.ply_CalcSmallOffset (*xint, *yint, x3, y3, &xnxt, &ynxt);
                }
                else {
                    ply_utils_obj.ply_CalcSmallOffset (*xint, *yint, x2, y2, &xnxt, &ynxt);
                }
            }
            else {
                ply_utils_obj.ply_CalcSmallOffset (x2, y2, x1, y1, &xprv, &yprv);
                ply_utils_obj.ply_CalcSmallOffset (x2, y2, x3, y3, &xnxt, &ynxt);
            }
            ply_utils_obj.ply_getgraze (&savegraze);
            ply_utils_obj.ply_setgraze (savegraze / 1000.f);
            if (kstatp) {
                istatp = kstatp;
            }
            else {
                istatp = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, xprv, yprv);
            }
            istatn = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, xnxt, ynxt);
            if (!istatn) {
                istatn = ply_utils_obj.ply_checkedge (xnxt, ynxt, x2, y2, 
                                        xpoly, ypoly, npoly, npts);
            }
            ply_utils_obj.ply_setgraze (savegraze);
            nc = *npint;

            goto OUTPUT_AND_RETURN;
        }

        if (orflag == 1) int_found++;

/*
        Check previous and next paths for in/out/edge positions
        and either continue or return depending on the case
*/
        if (istat >= 0) {
            if (istat == 2) {
                ply_utils_obj.ply_CalcSmallOffset (x1, y1, x0, y0, &xprv, &yprv);
                ply_utils_obj.ply_CalcSmallOffset (x1, y1, x2, y2, &xnxt, &ynxt);
            }
            else if (istat == 0) {
                if (ply_utils_obj.ply_graze (x1, y1, *xint, *yint)) {
                    ply_utils_obj.ply_CalcSmallOffset (*xint, *yint, x0, y0, &xprv, &yprv);
                }
                else {
                    ply_utils_obj.ply_CalcSmallOffset (*xint, *yint, x1, y1, &xprv, &yprv);
                }
                if (ply_utils_obj.ply_graze (x2, y2, *xint, *yint)) {
                    ply_utils_obj.ply_CalcSmallOffset (*xint, *yint, x3, y3, &xnxt, &ynxt);
                }
                else {
                    ply_utils_obj.ply_CalcSmallOffset (*xint, *yint, x2, y2, &xnxt, &ynxt);
                }
            }
            else {
                ply_utils_obj.ply_CalcSmallOffset (x2, y2, x1, y1, &xprv, &yprv);
                ply_utils_obj.ply_CalcSmallOffset (x2, y2, x3, y3, &xnxt, &ynxt);
            }
            ply_utils_obj.ply_getgraze (&savegraze);
            ply_utils_obj.ply_setgraze (savegraze / 1000.f);
            if (kstatp) {
                istatp = kstatp;
            }
            else {
                istatp = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, xprv, yprv);
            }
            istatn = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, xnxt, ynxt);
            if (!istatn) {
                istatn = ply_utils_obj.ply_checkedge (xnxt, ynxt, x2, y2, 
                                        xpoly, ypoly, npoly, npts);
            }
            ply_utils_obj.ply_setgraze (savegraze);
            nc = *npint;

/*          apply union rules for edge intersections  */

            if (orflag == 1) {
                
/*                previous point outside  */

                if (istatp == -1) {
                    if (istatn == -1) {
                        CONTINUE_TRAVERSE;
                    }
                    else {
                        goto OUTPUT_AND_RETURN;
                    }
                }

/*              previous point on edge  */

                else if (istatp == 0) {
                    if (istatn == 1) {
                        goto OUTPUT_AND_RETURN;
                    }
                    else {
                        CONTINUE_TRAVERSE;
                    }
                }
        
/*              previous point inside  */

                else {
                    if (istatn != 1) {
                        CONTINUE_TRAVERSE;
                    }
                    else {
                        goto OUTPUT_AND_RETURN;
                    }
                }
            }

/*          apply hole/main union rules for edge intersections  */

            if (orflag == 2) {
                
/*              previous point outside  */

                if (istatp == -1) {
                    if (istatn < 0) {
                        goto OUTPUT_AND_RETURN;
                    }
                    else {
                        if (ply_drivers_ptr->ply_toggleflag () == -1) {
                            CONTINUE_TRAVERSE;
                        }
                        else {
                            goto OUTPUT_AND_RETURN;
                        }
                    }
                }

/*              previous point on edge  */

                else if (istatp == 0) {
                    CONTINUE_TRAVERSE;
                }
        
/*              previous point inside  */

                else {
                    if (ply_drivers_ptr->ply_toggleflag () == -1) {
                        goto OUTPUT_AND_RETURN;
                    }
                    else {
                        CONTINUE_TRAVERSE;
                    }
                }
            }
            
            
/*          edge intersection rules for polygon intersection case  */            
           
/*          previous point outside  */

            if (istatp == -1) {
                if (istatn == 1) {
                    goto OUTPUT_AND_RETURN;
                }
                else if (istatn == 0) {
                    if (nc == 0) {
                        mult = 2;
                        ply_utils_obj.ply_CalcLeftOffset (mult, *xint, *yint, xnxt, ynxt, 
                                            &xleft, &yleft);
                        jstatn = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, 
                                              xleft, yleft);
                        while (jstatn == 0) {
                            mult *= 2;
                            if (mult < 2) {
                                mult = mult;
                            }
                            ply_utils_obj.ply_CalcLeftOffset (mult, *xint, *yint, xnxt, ynxt, 
                                                &xleft, &yleft);
                            jstatn = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, 
                                                  xleft, yleft);
                        }
                        if (jstatn == -1) {
                            goto OUTPUT_AND_RETURN;
                        }
                        else {
                            CONTINUE_TRAVERSE;
                        }
                    }
                    else {
                        CONTINUE_TRAVERSE;
                    }
                }
                else {
                    CONTINUE_TRAVERSE;
                }
            }

/*          previous point on edge  */

            else if (istatp == 0) {
                if (istatn == 1) {
                    if (nc == 0) {
                        CONTINUE_TRAVERSE;
                    }
                    else {
                        goto OUTPUT_AND_RETURN;
                    }
                }
                else if (istatn == 0) {
                    CONTINUE_TRAVERSE;
                }
                else {
                    if (nc == 0) {
                        mult = 2;
                        ply_utils_obj.ply_CalcLeftOffset (mult, *xint, *yint, xprv, yprv, 
                                            &xleft, &yleft);
                        jstatn = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, 
                                              xleft, yleft);
                        while (jstatn == 0) {
                            mult *= 2;
                            ply_utils_obj.ply_CalcLeftOffset (mult, *xint, *yint, xprv, yprv, 
                                                &xleft, &yleft);
                            jstatn = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, 
                                                  xleft, yleft);
                        }
                        if (jstatn == 1) {
                            goto OUTPUT_AND_RETURN;
                        }
                        else {
                            CONTINUE_TRAVERSE;
                        }
                    }
                    else {
                        mult = 2;
                        ply_utils_obj.ply_CalcLeftOffset (mult, *xint, *yint, xprv, yprv, 
                                            &xleft, &yleft);
                        jstatn = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, 
                                              xleft, yleft);
                        while (jstatn == 0) {
                            mult *= 2;
                            ply_utils_obj.ply_CalcLeftOffset (mult, *xint, *yint, xprv, yprv, 
                                                &xleft, &yleft);
                            jstatn = ply_utils_obj.ply_pointpa (xpoly, ypoly, npoly, npts, 
                                                  xleft, yleft);
                        }
                        if (jstatn == 1) {
                            goto OUTPUT_AND_RETURN;
                        }
                        else {
                            CONTINUE_TRAVERSE;
                        }
                    }
                }
            }

/*          previous point inside  */

            else {
                if (istatn == 1) {
                    if (nc > 0  ||  cflag > 0) {
                        goto OUTPUT_AND_RETURN;
                    }
                    else {
                        CONTINUE_TRAVERSE;
                    }
                }
                else if (istatn == -1) {
                    goto OUTPUT_AND_RETURN;
                }
                else {
                    if (nc > 0) {
                        goto OUTPUT_AND_RETURN;
                    }
                    else {
                        goto OUTPUT_AND_RETURN;
                    }
                }
            }
        }
    }

/*  no intersection found at all  */

    if (int_found == 0) {
        return 1;
    }
    *nextv = startv;
    return 6;

/*  
    branch to this label via goto when the traverse 
    has been successfully completed
*/

    OUTPUT_AND_RETURN:


    if (startloopflag == 0) {
        PrevInOut = istatp;
    }

    *nextv = i2;
    if (i2 == 0) {
        *nextv = nvploc;
    }
    if(outflag) {
        ncomp--;
        xout[ncomp] = *xint;
        yout[ncomp] = *yint;
        *nout = ncomp+1;
    }

/*  x2,y2 on source side is intersection point  */

    if(istat == 3) {
        (*nextv)++;
        return 3;
    }
/*  
    if the path came back and intersected the start point without
    any other intersections, return 1 for an error
*/

    if (startloopflag) {
        if (int_found == 0) {
            return 1;
        }
        *nextv = startv;
        return 6;
    }

    return 0;

}  /*  end of ply_trvint function  */


/*
  ****************************************************************

                  p l y _ G e t P r e v I n O u t

  ****************************************************************

*/

int CSWPolyTraverse::ply_GetPrevInOut (void)
{
    return PrevInOut;

}  /*  end of function ply_GetPrevInOut  */



/*
  ****************************************************************

                  p l y _ S e t P r e v I n O u t

  ****************************************************************

*/

int CSWPolyTraverse::ply_SetPrevInOut (int val)
{
    PrevInOut = val;
    return 1;

}  /*  end of function ply_SetPrevInOut  */





/*
******************************************************************

                p l y _ t r v i n t i n i t

******************************************************************

    initialize class variables used by ply_trvint
*/

int CSWPolyTraverse::ply_trvintinit (const char *string, double val)
{
    if(!strcmp(string, "lastx2")) {
        lastx2 = (long)val;
    }
    else if (!strcmp(string, "orflag")) {
        orflag = (long)val;
    }
    else {
        return 1;
    }

    return 0;

}





/*
    ***************************************************************

                      p l y _ s i d i n t

    ***************************************************************

  function name:    ply_sidint  (integer)

  call sequence:    ply_sidint(xpoly, ypoly, npoly, npts,
                               x1, y1, x2, y2,
                               &xout, &yout, &nhole, &npoint)

  synopsis:         Calculate the intersection between a vector and a
                    polygon. 
 
  return values:    status code

                    0 = intersection found and returned in xout,yout
                    1 = no intersection found
                    2 = intersection at the x1,y1 point only
                    3 = best intersection is at x2,y2

  usage:            This function is used as part of the polygon boolean
                    operations and should not be used in general (i.e. to
                    clip vectors to a polygon.)  This function assumes 
                    that any trivial rejection (i.e. bounds of vector
                    outside of the polygon bounds) has already been
                    performed.  The trivial rejection procedures can
                    eliminate alot of vectors from consideration and save
                    alot of time.  Special handling of overlapping vectors
                    are set up by the calling function. 

  algorithm:        Each component of the polygon (main body and holes)
                    is traversed with each polygon side separated and 
                    checked for intersection.  If more than one
                    intersection is found, the closest to x1,y1 is used.

  description of calling parameters:

    xpoly     r   double*[]  array of pointers to x coordinates of the 
                             components of the polygon
    ypoly     r   double*[]  array of pointers to y coordinates of the
                             components of the polygon
    npoly     r   int        number of components in the polygon
    npts      r   int     *  array with number of points in each component
    x1        r   double     x coordinate of first endpoint of vector
    y1        r   double     y coordinate of first endpoint of vector
    x2        r   double     x coordinate of second endpoint of vector
    y2        r   double     y coordinate of second endpoint of vector
    xout      w   double*    x coordinate of closest intersection to x1,y1
    yout      w   double*    y coordinate of closest intersection to x1,y1
    nhole     w   int     *  component number where intersection is found
    npoint    w   int     *  next point on the component subsequent to the
                             intersection

*/

int CSWPolyTraverse::ply_sidint(
               CSWPolyGridvec  &ply_gridvec_obj,
               double **xpoly, double **ypoly, int npoly, int *npts,
               double x1, double y1, double x2, double y2,
               double *xout, double *yout, int *nhole, int *npoint)
{   
    double    xx1, yy1, xx2, yy2, xb1, yb1, xb2, yb2, 
              x2save, y2save, xt, yt, dirt, dirt2, eps;
    int       i, j, istat, first, npm1, tinyflag, nptloc, jstat;
    double    *xpoint = NULL, *ypoint = NULL, dist, x0, y0,
              dist1, dist2, dist3;
    CSWPolyUtils  ply_utils_obj;
    //CSWPolyGridvec  ply_gridvec_obj;

/*  check for obvious errors  */

    if(npoly < 1) return 1;
    if(x1 - x2 == 0.0  &&  y1 - y2 == 0.0) return 1;

    dist = 1.e30f;

/*  check vector grid to see if intersection is possible  */

    istat = ply_gridvec_obj.ply_checkgridforvec (x1, y1, x2, y2);
    if (!istat) {
        return 1;
    }

/*  extend second endpoint by grazing distance  */

    x2save = x2;
    y2save = y2;
    xt = x2;
    yt = y2;
    ply_utils_obj.ply_extendlineseg (x1,y1, &xt, &yt);
    x2 = xt;
    y2 = yt;

/*  calculate bounding limits of the vector  */

    if(x1 < x2) {
        xb1 = x1;
        xb2 = x2;
    }
    else {
        xb1 = x2;
        xb2 = x1;
    }

    if(y1 < y2) {
        yb1 = y1;
        yb2 = y2;
    }
    else {
        yb1 = y2;
        yb2 = y1;
    }

    ply_utils_obj.ply_getgraze (&eps);
    eps *= 10.f;
    xb1 -= eps;
    xb2 += eps;
    yb1 -= eps;
    yb2 += eps;

/*  loop through each polygon component, checking each side in each
    component for intersection with the vector  */

    first = 1;
    tinyflag = 0;

    for (i=0; i < npoly; i++) {
        xpoint = xpoly[i];
        ypoint = ypoly[i];

/*      open polygon component if first and last points are coincident  */

        nptloc = npts[i];
        while(xpoint[0] - xpoint[nptloc-1] == 0.0  &&
              ypoint[0] - ypoint[nptloc-1] == 0.0  &&
              nptloc > 1) {
            nptloc--;
        }
        if(nptloc < 2) {
            continue;
        }
        xx1 = *xpoint;
        yy1 = *ypoint;
        x0 = xx1;
        y0 = yy1;
        xpoint++;
        ypoint++;

/*      check each side of this component  */

        npm1 = nptloc-1;
        for (j=0; j < nptloc; j++) {
            if(j == npm1) {
                xx2 = x0;
                yy2 = y0;
            }
            else {
                xx2 = *xpoint;
                yy2 = *ypoint;
                xpoint++;
                ypoint++;
            }

/*          if this side is outside of the vector bounds, it cannot intersect, 
            reset xx1,yy1 and continue the loop through this component using
            the LOOP macro  */

            if(xx1 < xb1  &&  xx2 < xb1)  LOOP
            if(xx1 > xb2  &&  xx2 > xb2)  LOOP
            if(yy1 < yb1  &&  yy2 < yb1)  LOOP
            if(yy1 > yb2  &&  yy2 > yb2)  LOOP

/*          if the endpoints of the polygon side are coincident, the side
            cannot intersect with the vector  */

            if(xx1 - xx2 == 0.0  &&  yy1 - yy2 == 0.0)  LOOP

/*          solve for intersection between the polygon side and the vector  */

            istat = ply_utils_obj.ply_segint (xx1, yy1, xx2, yy2,
                                x1, y1, x2, y2,
                                &xt, &yt);

/*
            if the Close point lines on the segment between x1, y1 and x2, y2
            and on the segment between xx1,yy1 and xx2,yy2, 
            use the close point as the output point
*/

            if (istat == 0  ||  istat == 2) {
                if (!ply_utils_obj.ply_graze (xt, yt, x1, y1)) {
                    if (ply_utils_obj.ply_pointonlineseg (x1, y1, x2, y2, CloseX, CloseY)  &&
                        ply_utils_obj.ply_pointonlineseg (xx1, yy1, xx2, yy2, CloseX, CloseY)) {
                        xt = CloseX;
                        yt = CloseY;
                        istat = 2;
                    }
                }
            }

/*          check if the polygon vertex grazes the vector  */

            if (istat == 1) {
                jstat = gpf_perpintpoint2 (x1, y1, x2, y2, xx1, yy1, &xt, &yt);
                if (jstat) {
                    if (ply_utils_obj.ply_graze (xx1, yy1, xt, yt)) {
                        xt = xx1;
                        yt = yy1;
                        istat = 0;
                    }
                }
            }

/*          check if first point on vector grazes the polygon edge  */

            if (istat == 1) {
                jstat = gpf_perpintpoint2 (xx1, yy1, xx2, yy2, x1, y1, &xt, &yt);
                if (jstat) {
                    if (ply_utils_obj.ply_graze (x1, y1, xt, yt)) {
                        istat = 0;
                    }
                }
            }

/*          if the line segments are overlapping, choose the closest
            endpoint still in the overlap region.  */

            if(istat == 3) {
                if((xx1-x1)*(x1-xx2) >= 0 &&
                   (yy1-y1)*(y1-yy2) >= 0 &&
                   (xx1-x2)*(x2-xx2) >= 0 &&
                   (yy1-y2)*(y2-yy2) >= 0 )  {
                        xt = x2save;
                        yt = y2save;
                        istat = 2;
                }
            }

            if(istat == 3) {
                if ((x1-xx1)*(xx1-x2) >= 0.0f) {
                   dirt = xx1 - x1;
                   dirt2 = yy1 - y1;
                   dist1 = dirt*dirt + dirt2*dirt2;
                }
                else {
                    dist1 = 1.e30f;
                }
                if ((x1-xx2)*(xx2-x2) >= 0.0f) {
                    dirt = xx2 - x1;
                    dirt2 = yy2 - y1;
                    dist2 = dirt*dirt + dirt2*dirt2;
                }
                else {
                    dist2 = 1.e30f;
                }
                dirt = x2save - x1;
                dirt2 = y2save - y1;
                dist3 = dirt*dirt + dirt2*dirt2;
                if(dist3 > dist2  &&  dist3 > dist1) {
                    if(dist2 > dist1) {
                        xt = xx1;
                        yt = yy1;
                    }
                    else {
                        xt = xx2;
                        yt = yy2;
                    }
                }
                else if(dist2 > dist1  &&  dist2 > dist3) {
                    if(dist3 > dist1) {
                        xt = xx1;
                        yt = yy1;
                    }
                    else {
                        xt = x2save;
                        yt = y2save;
                    }
                }
                else {
                    if(dist2 > dist3) {
                        xt = x2save;
                        yt = y2save;
                    }
                    else {
                        xt = xx2;
                        yt = yy2;
                    }
                }
                istat = 2;
            }

/*          sides are identical, use x2, y2 as intersection */

            if(istat == 4) {
                xt = x2save;
                yt = y2save;
                istat = 2;
            }

/*          if a valid intersection is found, check against the previous
            closest intersection to x1,y1.  reset the intersection to the 
            current point if the current point is closer  */

            if(istat == 0  ||  istat == 2) {

                dirt2 = x1 - xt;
                dirt = y1 - yt;
                dirt = dirt*dirt + dirt2*dirt2;

/*              if the intersection is the same as x1, y1, special
                case, otherwise, check normally  */

                if(!ply_utils_obj.ply_graze(xt, yt, x1, y1)) {

/*                  initialize output point to the first intersection found  */

                    if(first) {
                        first = 0;
                        *xout = xt;
                        *yout = yt;
                        dist = dirt;
                        *nhole = i;
                        *npoint = j+1;
                        if(ply_utils_obj.ply_graze(xx2, yy2, xt, yt)) {
                            (*npoint)++;
                        }       
                    }

/*                  reset if closer than current output point  */

                    if(dirt < dist) { 
                        *xout = xt;
                        *yout = yt;
                        dist = dirt;
                        *nhole = i;
                        *npoint = j+1;
                        if(ply_utils_obj.ply_graze(xx2, yy2, xt, yt)) {
                            (*npoint)++;
                        }       
                    }
                    tinyflag = 0;
                }
                else if(first) {
                    *xout = xt;
                    *yout = yt;
                    *nhole = i;
                    *npoint = j+1;
                    tinyflag = 1;
                    if(ply_utils_obj.ply_graze(xx2, yy2, xt, yt)) {
                        (*npoint)++;
                    }       
                }
            }

            xx1 = xx2;
            yy1 = yy2;

/*      end of loop through a single component  */

        }

/*  end of loop through components of the polygon  */
    
    }

/*  return with no intersection flag  */

    if(first) {
        if(!tinyflag  ||  orflag2) {
            return 1;
        }
    }

/*  return with success flag  */

    if (ply_utils_obj.ply_graze(x1, y1, *xout, *yout)) {
        if (orflag2){
            return 1;
        }
        return 2;
    }

    if (ply_utils_obj.ply_graze(x2, y2, *xout, *yout)) {
        return 3;
    }

    return 0;

}  /*  end of ply_sidint function  */




/*
******************************************************************

                p l y _ s i d i n t i n i t

******************************************************************

    initialize flags for ply_sidint  

*/


int CSWPolyTraverse::ply_sidintinit (const char *string, double val)
{
    if (!strcmp(string, "orflag")) {
        orflag2 = (long) val;
    }
    else {
        return 1;
    }
    return 1;
}



/*
  ****************************************************************

                p l y _ s e t c l o s i n g p o i n t

  ****************************************************************

*/

int CSWPolyTraverse::ply_setclosingpoint (double x, double y)
{
    CloseX = x;
    CloseY = y;
    return 1;

}  /*  end of function ply_setclosingpoint  */



/*
  ****************************************************************

                p l y _ g e t c l o s i n g p o i n t

  ****************************************************************

*/

int CSWPolyTraverse::ply_getclosingpoint (double *x, double *y)
{
    *x = CloseX;
    *y = CloseY;
    return 1;

}  /*  end of function ply_getclosingpoint  */



/*
    ***************************************************************

                  p l y _ C l i p V e c T o A r e a

    ***************************************************************

  function name:    ply_ClipVecToArea  (integer)

  call sequence:    ply_ClipVecToArea(xpoly, ypoly, npoly, npts,
                                      x1, y1, x2, y2,
                                      &xout, &yout)

  synopsis:         Calculate the intersection between a vector and a
                    polygon. 
 
  return values:    status code

                    0 = intersection found and returned in xout,yout
                    1 = no intersection found
                    2 = intersection at the x1,y1 point only
                    3 = best intersection is at x2,y2

  algorithm:        Each component of the polygon (main body and holes)
                    is traversed with each polygon side separated and 
                    checked for intersection.  If more than one
                    intersection is found, the closest to x1,y1 is used.

  description of calling parameters:

    xpoly     r   double*[]  array of pointers to x coordinates of the 
                             components of the polygon
    ypoly     r   double*[]  array of pointers to y coordinates of the
                             components of the polygon
    npoly     r   int        number of components in the polygon
    npts      r   int     *  array with number of points in each component
    x1        r   double     x coordinate of first endpoint of vector
    y1        r   double     y coordinate of first endpoint of vector
    x2        r   double     x coordinate of second endpoint of vector
    y2        r   double     y coordinate of second endpoint of vector
    xout      w   double*    x coordinate of closest intersection to x1,y1
    yout      w   double*    y coordinate of closest intersection to x1,y1

*/

int CSWPolyTraverse::ply_ClipVecToArea
                     (double **xpoly, double **ypoly,
                      int npoly, int *npts,
                      double x1, double y1, double x2, double y2,
                      double *xout, double *yout)
{   
    double    xx1, yy1, xx2, yy2, xb1, yb1, xb2, yb2, 
              x2save, y2save, xt, yt, dirt, dirt2, eps;
    int       i, j, istat, first, tinyflag, nptloc;
    double    *xpoint = NULL, *ypoint = NULL, dist, dist1, dist2, dist3;
    CSWPolyUtils  ply_utils_obj;

/*  check for obvious errors  */

    if(npoly < 1) return 1;
    if(x1 - x2 == 0.0  &&  y1 - y2 == 0.0) return 1;

    dist = 1.e30f;

/*  extend line segment by grazing distance  */

    x2save = x2;
    y2save = y2;
    xt = x2;
    yt = y2;
    ply_utils_obj.ply_extendlineseg (x1, y1, &xt, &yt);
    x2 = xt;
    y2 = yt;

/*  calculate bounding limits of the vector  */

    if(x1 < x2) {
        xb1 = x1;
        xb2 = x2;
    }
    else {
        xb1 = x2;
        xb2 = x1;
    }

    if(y1 < y2) {
        yb1 = y1;
        yb2 = y2;
    }
    else {
        yb1 = y2;
        yb2 = y1;
    }

    ply_utils_obj.ply_getgraze (&eps);
    eps *= 10.f;
    xb1 -= eps;
    xb2 += eps;
    yb1 -= eps;
    yb2 += eps;

/*  loop through each polygon component, checking each side in each
    component for intersection with the vector  */

    first = 1;
    tinyflag = 0;

    for (i=0; i < npoly; i++) {
        xpoint = xpoly[i];
        ypoint = ypoly[i];

        nptloc = npts[i] - 1;
        xx1 = *xpoint;
        yy1 = *ypoint;
        xpoint++;
        ypoint++;

/*      check each side of this component  */

        for (j=0; j < nptloc; j++) {

            xx2 = *xpoint;
            yy2 = *ypoint;
            xpoint++;
            ypoint++;

/*          if this side is outside of the vector bounds, it cannot intersect, 
            reset xx1,yy1 and continue the loop through this component using
            the LOOP macro  */

            if(xx1 < xb1  &&  xx2 < xb1)  LOOP
            if(xx1 > xb2  &&  xx2 > xb2)  LOOP
            if(yy1 < yb1  &&  yy2 < yb1)  LOOP
            if(yy1 > yb2  &&  yy2 > yb2)  LOOP

/*          if the endpoints of the polygon side are coincident, the side
            cannot intersect with the vector  */

            if(xx1 - xx2 == 0.0  &&  yy1 - yy2 == 0.0)  LOOP

/*          solve for intersection between the polygon side and the vector  */

            istat = ply_utils_obj.ply_segint (xx1, yy1, xx2, yy2,
                                x1, y1, x2, y2,
                                &xt, &yt);

            if (istat == 1) {
                if (ply_utils_obj.ply_graze(xt, yt, x2, y2)  ||  ply_utils_obj.ply_graze(xt, yt, x2save, y2save)) {
                    istat = 0;
                    xt = x2save;
                    yt = y2save;
                }
                if (ply_utils_obj.ply_graze(xt, yt, x1, y1)) {
                    istat = 0;
                }
            }

/*          if the line segments are overlapping, choose the closest
            endpoint still in the overlap region.  */

            if(istat == 3) {
                if((xx1-x1)*(x1-xx2) >= 0 &&
                   (yy1-y1)*(y1-yy2) >= 0 &&
                   (xx1-x2)*(x2-xx2) >= 0 &&
                   (yy1-y2)*(y2-yy2) >= 0 )  {
                        xt = x2save;
                        yt = y2save;
                        istat = 2;
                }
            }

            if(istat == 3) {
                if ((x1-xx1)*(xx1-x2) >= 0.0f) {
                   dirt = xx1 - x1;
                   dirt2 = yy1 - y1;
                   dist1 = dirt*dirt + dirt2*dirt2;
                }
                else {
                    dist1 = 1.e30f;
                }
                if ((x1-xx2)*(xx2-x2) >= 0.0f) {
                    dirt = xx2 - x1;
                    dirt2 = yy2 - y1;
                    dist2 = dirt*dirt + dirt2*dirt2;
                }
                else {
                    dist2 = 1.e30f;
                }
                dirt = x2save - x1;
                dirt2 = y2save - y1;
                dist3 = dirt*dirt + dirt2*dirt2;
                if(dist3 > dist2  &&  dist3 > dist1) {
                    if(dist2 > dist1) {
                        xt = xx1;
                        yt = yy1;
                    }
                    else {
                        xt = xx2;
                        yt = yy2;
                    }
                }
                else if(dist2 > dist1  &&  dist2 > dist3) {
                    if(dist3 > dist1) {
                        xt = xx1;
                        yt = yy1;
                    }
                    else {
                        xt = x2save;
                        yt = y2save;
                    }
                }
                else {
                    if(dist2 > dist3) {
                        xt = x2save;
                        yt = y2save;
                    }
                    else {
                        xt = xx2;
                        yt = yy2;
                    }
                }
                istat = 2;
            }

/*          sides are identical, use x2, y2 as intersection */

            if(istat == 4) {
                xt = x2save;
                yt = y2save;
                istat = 2;
            }

/*          if a valid intersection is found, check against the previous
            closest intersection to x1,y1.  reset the intersection to the 
            current point if the current point is closer  */

            if(istat == 0  ||  istat == 2) {

                dirt2 = x1 - xt;
                dirt = y1 - yt;
                dirt = dirt*dirt + dirt2*dirt2;

/*              if the intersection is the same as x1, y1, special
                case, otherwise, check normally  */


                if(!ply_utils_obj.ply_graze(xt, yt, x1, y1)) {

/*                  initialize output point to the first intersection found  */

                    if(first) {
                        first = 0;
                        *xout = xt;
                        *yout = yt;
                        dist = dirt;
                    }

/*                  reset if closer than current output point  */

                    if(dirt < dist) { 
                        *xout = xt;
                        *yout = yt;
                        dist = dirt;
                    }
                    tinyflag = 0;
                }
                else if(first) {
                    *xout = xt;
                    *yout = yt;
                    tinyflag = 1;
                }
            }

            xx1 = xx2;
            yy1 = yy2;

/*      end of loop through a single component  */

        }

/*  end of loop through components of the polygon  */
    
    }

/*  return with no intersection flag  */

    if(first) {
        if(!tinyflag) {
            return 1;
        }
    }

/*  return with success flag  */

    ply_utils_obj.ply_getgraze (&eps);
    ply_utils_obj.ply_setgraze (eps * 1.2f);

    if (ply_utils_obj.ply_graze(x1, y1, *xout, *yout)) {
        ply_utils_obj.ply_setgraze (eps);
        return 2;
    }

    if (ply_utils_obj.ply_graze(x2, y2, *xout, *yout)) {
        ply_utils_obj.ply_setgraze (eps);
        return 3;
    }

    if (ply_utils_obj.ply_graze(x2save, y2save, *xout, *yout)) {
        ply_utils_obj.ply_setgraze (eps);
        return 3;
    }

    return 0;

}  /*  end of ply_ClipVecToArea function  */




/*
    ***************************************************************

                      p l y _ s i d i n t

    ***************************************************************

  function name:    ply_sidint  (integer)

  call sequence:    ply_sidint(xpoly, ypoly, npoly, npts,
                               x1, y1, x2, y2,
                               &xout, &yout, &nhole, &npoint)

  synopsis:         Calculate the intersection between a vector and a
                    polygon. 
 
  return values:    status code

                    0 = intersection found and returned in xout,yout
                    1 = no intersection found
                    2 = intersection at the x1,y1 point only
                    3 = best intersection is at x2,y2

  usage:            This function is used as part of the polygon boolean
                    operations and should not be used in general (i.e. to
                    clip vectors to a polygon.)  This function assumes 
                    that any trivial rejection (i.e. bounds of vector
                    outside of the polygon bounds) has already been
                    performed.  The trivial rejection procedures can
                    eliminate alot of vectors from consideration and save
                    alot of time.  Special handling of overlapping vectors
                    are set up by the calling function. 

  algorithm:        Each component of the polygon (main body and holes)
                    is traversed with each polygon side separated and 
                    checked for intersection.  If more than one
                    intersection is found, the closest to x1,y1 is used.

  description of calling parameters:

    xpoly     r   double*[]  array of pointers to x coordinates of the 
                             components of the polygon
    ypoly     r   double*[]  array of pointers to y coordinates of the
                             components of the polygon
    npoly     r   int        number of components in the polygon
    npts      r   int     *  array with number of points in each component
    x1        r   double     x coordinate of first endpoint of vector
    y1        r   double     y coordinate of first endpoint of vector
    x2        r   double     x coordinate of second endpoint of vector
    y2        r   double     y coordinate of second endpoint of vector
    xout      w   double*    x coordinate of closest intersection to x1,y1
    yout      w   double*    y coordinate of closest intersection to x1,y1
    nhole     w   int     *  component number where intersection is found
    npoint    w   int     *  next point on the component subsequent to the
                             intersection

*/

int CSWPolyTraverse::ply_sidint(
               double **xpoly, double **ypoly, int npoly, int *npts,
               double x1, double y1, double x2, double y2,
               double *xout, double *yout, int *nhole, int *npoint)
{   
    double    xx1, yy1, xx2, yy2, xb1, yb1, xb2, yb2, 
              x2save, y2save, xt, yt, dirt, dirt2, eps;
    int       i, j, istat, first, npm1, tinyflag, nptloc, jstat;
    double    *xpoint = NULL, *ypoint = NULL, dist, x0, y0,
              dist1, dist2, dist3;
    CSWPolyUtils  ply_utils_obj;

/*  check for obvious errors  */

    if(npoly < 1) return 1;
    if(x1 - x2 == 0.0  &&  y1 - y2 == 0.0) return 1;

    dist = 1.e30f;

/*  extend second endpoint by grazing distance  */

    x2save = x2;
    y2save = y2;
    xt = x2;
    yt = y2;
    ply_utils_obj.ply_extendlineseg (x1,y1, &xt, &yt);
    x2 = xt;
    y2 = yt;

/*  calculate bounding limits of the vector  */

    if(x1 < x2) {
        xb1 = x1;
        xb2 = x2;
    }
    else {
        xb1 = x2;
        xb2 = x1;
    }

    if(y1 < y2) {
        yb1 = y1;
        yb2 = y2;
    }
    else {
        yb1 = y2;
        yb2 = y1;
    }

    ply_utils_obj.ply_getgraze (&eps);
    eps *= 10.f;
    xb1 -= eps;
    xb2 += eps;
    yb1 -= eps;
    yb2 += eps;

/*  loop through each polygon component, checking each side in each
    component for intersection with the vector  */

    first = 1;
    tinyflag = 0;

    for (i=0; i < npoly; i++) {
        xpoint = xpoly[i];
        ypoint = ypoly[i];

/*      open polygon component if first and last points are coincident  */

        nptloc = npts[i];
        while(xpoint[0] - xpoint[nptloc-1] == 0.0  &&
              ypoint[0] - ypoint[nptloc-1] == 0.0  &&
              nptloc > 1) {
            nptloc--;
        }
        if(nptloc < 2) {
            continue;
        }
        xx1 = *xpoint;
        yy1 = *ypoint;
        x0 = xx1;
        y0 = yy1;
        xpoint++;
        ypoint++;

/*      check each side of this component  */

        npm1 = nptloc-1;
        for (j=0; j < nptloc; j++) {
            if(j == npm1) {
                xx2 = x0;
                yy2 = y0;
            }
            else {
                xx2 = *xpoint;
                yy2 = *ypoint;
                xpoint++;
                ypoint++;
            }

/*          if this side is outside of the vector bounds, it cannot intersect, 
            reset xx1,yy1 and continue the loop through this component using
            the LOOP macro  */

            if(xx1 < xb1  &&  xx2 < xb1)  LOOP
            if(xx1 > xb2  &&  xx2 > xb2)  LOOP
            if(yy1 < yb1  &&  yy2 < yb1)  LOOP
            if(yy1 > yb2  &&  yy2 > yb2)  LOOP

/*          if the endpoints of the polygon side are coincident, the side
            cannot intersect with the vector  */

            if(xx1 - xx2 == 0.0  &&  yy1 - yy2 == 0.0)  LOOP

/*          solve for intersection between the polygon side and the vector  */

            istat = ply_utils_obj.ply_segint (xx1, yy1, xx2, yy2,
                                x1, y1, x2, y2,
                                &xt, &yt);

/*
            if the Close point lines on the segment between x1, y1 and x2, y2
            and on the segment between xx1,yy1 and xx2,yy2, 
            use the close point as the output point
*/

            if (istat == 0  ||  istat == 2) {
                if (!ply_utils_obj.ply_graze (xt, yt, x1, y1)) {
                    if (ply_utils_obj.ply_pointonlineseg (x1, y1, x2, y2, CloseX, CloseY)  &&
                        ply_utils_obj.ply_pointonlineseg (xx1, yy1, xx2, yy2, CloseX, CloseY)) {
                        xt = CloseX;
                        yt = CloseY;
                        istat = 2;
                    }
                }
            }

/*          check if the polygon vertex grazes the vector  */

            if (istat == 1) {
                jstat = gpf_perpintpoint2 (x1, y1, x2, y2, xx1, yy1, &xt, &yt);
                if (jstat) {
                    if (ply_utils_obj.ply_graze (xx1, yy1, xt, yt)) {
                        xt = xx1;
                        yt = yy1;
                        istat = 0;
                    }
                }
            }

/*          check if first point on vector grazes the polygon edge  */

            if (istat == 1) {
                jstat = gpf_perpintpoint2 (xx1, yy1, xx2, yy2, x1, y1, &xt, &yt);
                if (jstat) {
                    if (ply_utils_obj.ply_graze (x1, y1, xt, yt)) {
                        istat = 0;
                    }
                }
            }

/*          if the line segments are overlapping, choose the closest
            endpoint still in the overlap region.  */

            if(istat == 3) {
                if((xx1-x1)*(x1-xx2) >= 0 &&
                   (yy1-y1)*(y1-yy2) >= 0 &&
                   (xx1-x2)*(x2-xx2) >= 0 &&
                   (yy1-y2)*(y2-yy2) >= 0 )  {
                        xt = x2save;
                        yt = y2save;
                        istat = 2;
                }
            }

            if(istat == 3) {
                if ((x1-xx1)*(xx1-x2) >= 0.0f) {
                   dirt = xx1 - x1;
                   dirt2 = yy1 - y1;
                   dist1 = dirt*dirt + dirt2*dirt2;
                }
                else {
                    dist1 = 1.e30f;
                }
                if ((x1-xx2)*(xx2-x2) >= 0.0f) {
                    dirt = xx2 - x1;
                    dirt2 = yy2 - y1;
                    dist2 = dirt*dirt + dirt2*dirt2;
                }
                else {
                    dist2 = 1.e30f;
                }
                dirt = x2save - x1;
                dirt2 = y2save - y1;
                dist3 = dirt*dirt + dirt2*dirt2;
                if(dist3 > dist2  &&  dist3 > dist1) {
                    if(dist2 > dist1) {
                        xt = xx1;
                        yt = yy1;
                    }
                    else {
                        xt = xx2;
                        yt = yy2;
                    }
                }
                else if(dist2 > dist1  &&  dist2 > dist3) {
                    if(dist3 > dist1) {
                        xt = xx1;
                        yt = yy1;
                    }
                    else {
                        xt = x2save;
                        yt = y2save;
                    }
                }
                else {
                    if(dist2 > dist3) {
                        xt = x2save;
                        yt = y2save;
                    }
                    else {
                        xt = xx2;
                        yt = yy2;
                    }
                }
                istat = 2;
            }

/*          sides are identical, use x2, y2 as intersection */

            if(istat == 4) {
                xt = x2save;
                yt = y2save;
                istat = 2;
            }

/*          if a valid intersection is found, check against the previous
            closest intersection to x1,y1.  reset the intersection to the 
            current point if the current point is closer  */

            if(istat == 0  ||  istat == 2) {

                dirt2 = x1 - xt;
                dirt = y1 - yt;
                dirt = dirt*dirt + dirt2*dirt2;

/*              if the intersection is the same as x1, y1, special
                case, otherwise, check normally  */

                if(!ply_utils_obj.ply_graze(xt, yt, x1, y1)) {

/*                  initialize output point to the first intersection found  */

                    if(first) {
                        first = 0;
                        *xout = xt;
                        *yout = yt;
                        dist = dirt;
                        *nhole = i;
                        *npoint = j+1;
                        if(ply_utils_obj.ply_graze(xx2, yy2, xt, yt)) {
                            (*npoint)++;
                        }       
                    }

/*                  reset if closer than current output point  */

                    if(dirt < dist) { 
                        *xout = xt;
                        *yout = yt;
                        dist = dirt;
                        *nhole = i;
                        *npoint = j+1;
                        if(ply_utils_obj.ply_graze(xx2, yy2, xt, yt)) {
                            (*npoint)++;
                        }       
                    }
                    tinyflag = 0;
                }
                else if(first) {
                    *xout = xt;
                    *yout = yt;
                    *nhole = i;
                    *npoint = j+1;
                    tinyflag = 1;
                    if(ply_utils_obj.ply_graze(xx2, yy2, xt, yt)) {
                        (*npoint)++;
                    }       
                }
            }

            xx1 = xx2;
            yy1 = yy2;

/*      end of loop through a single component  */

        }

/*  end of loop through components of the polygon  */
    
    }

/*  return with no intersection flag  */

    if(first) {
        if(!tinyflag  ||  orflag2) {
            return 1;
        }
    }

/*  return with success flag  */

    if (ply_utils_obj.ply_graze(x1, y1, *xout, *yout)) {
        if (orflag2){
            return 1;
        }
        return 2;
    }

    if (ply_utils_obj.ply_graze(x2, y2, *xout, *yout)) {
        return 3;
    }

    return 0;

}  /*  end of ply_sidint function  */
