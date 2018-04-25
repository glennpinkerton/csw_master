
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_cutline.cc

    Refactor of old ply_cutline.c file.  This version uses the
    CSWPolyCutline class rather than csw_Free standing C functions.
    The biggesat reason for the refactor is to give me more C++
    experience.  The second reason is to make the csw code 
    thread safe, or at least a lot closer to thread safe.
*/


/*
    system header files
*/

#include <stdio.h>


/*
    application header files
*/

#include "csw/utils/include/csw_.h"

#include "csw/utils/private_include/gpf_calcdraw.h"
#include "csw/utils/private_include/gpf_utils.h"

#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_traverse.h"
#include "csw/utils/private_include/ply_cutline.h"
#include "csw/utils/private_include/csw_scope.h"

/*
    define constants
*/

#define MAXHOLES               1000





/*
  ****************************************************************

                p l y _ A d d C u t L i n e s

  ****************************************************************

    Add lines connecting holes in a complex polygon so that all of
  the connecting lines lie entirely inside the polygon.  This seems
  to be the most certain way of getting filled polygons rendered
  properly on a variety of output devices.

    This function normally returns 1, and if there is a memory 
  allocation error, -1 is returned.

    It is possible for more than one polygon to be returned by
  this function if there are holes in the input polygon which 
  are nested more than one level deep.

*/

int CSWPolyCutline::ply_AddCutLines
         (double *xin, double *yin, int ncompin, int *nptsin,
          double *xout, double *yout, int *npolyout, int *nptsout)
{
    int           ncomp, nmem1, nmem2, i, ii, jj,
                  *iw1 = NULL, *iw2 = NULL, nestpoly, istat,
                  *inest1 = NULL, *inest2 = NULL,
                  nn, nnout, npt, nc, nbad, ntimes, nmemholes,
                  nmempoints;
    double        *xw1 = NULL, *yw1 = NULL, *xnest = NULL, *ynest = NULL,
                  *xp = NULL, *yp = NULL, xt;

    CSWPolyUtils  ply_utils_obj;

    auto fscope = [&]()
    {
        csw_Free (xw1);
        csw_Free (iw1);
        csw_Free (XHoles);

        XHoles = NULL;
        YHoles = NULL;
        Tmp = NULL;
        IHoles = NULL;
        NHoles = 0;

    };
    CSWScopeGuard func_scope_guard (fscope);

    ncomp = ncompin;

/*
    allocate workspace memory, including private memory
    used by other functions in the file.  I allocate
    more space than should be needed because I am the
    suspicious type.
*/
    nmem1 = 0;
    for (i=0; i<ncomp; i++) {
        nmem1 += nptsin[i];
        nmem1 += 20;
    }
    nmem1 += 10;
    nmem2 = nmem1 * 3 + 10 * ncomp;
    
    xw1 = (double *)csw_Malloc (nmem2 * sizeof(double));
    if (!xw1) {
        return -1;
    }

    yw1 = xw1 + nmem1;
    Tmp = yw1 + nmem1;

    nmempoints = nmem1;


    nmem1 = ncomp * 2 + 10;
    nmem2 = nmem1 * 3 + 10;
    
    iw1 = (int *)csw_Malloc (nmem2 * sizeof(int));
    if (!iw1) {
        return -1;
    }

    iw2 = iw1 + nmem1;
    IHoles = iw2 + nmem1;
    nmemholes = nmem1;

    nmem2 = nmem1 * 2 + 10;
    XHoles = (double **)csw_Malloc (nmem2 * sizeof(double *));
    if (!XHoles) {
        return -1;
    }
    YHoles = XHoles + nmem1;

/*
    set memory size parameters for hole nest function
*/

    xt = (double)nmempoints;
    ply_utils_obj.ply_parms ('w', "maxpout", &xt);
    xt = (double)nmemholes;
    ply_utils_obj.ply_parms ('w', "maxhout", &xt);

/*
    unnest the polygon so that each sub polygon has only
    one level of holes
*/

    istat = ply_utils_obj.ply_holnest (xin, yin, ncompin, nptsin,
                         xw1, yw1, &nestpoly, iw1, iw2);

    if (istat == -1  ||  istat == 1  ||  istat == 3) {
        return -1;
    }

/*
    no unnesting was needed, so use the original input
*/

    if (istat == 2) {
        xnest = xin;
        ynest = yin;
        inest2 = nptsin;
        inest1 = &ncomp;
        nestpoly = 1;
    }

/*
    use the output of ply_holnest
*/

    else {
        xnest = xw1;
        ynest = yw1;
        inest1 = iw1;
        inest2 = iw2;
    }

/*
    loop through the separated polygons and for each polygon,
    connect the holes with cut lines that do not intersect
    any other edge of the polygon
*/

    xp = xout;
    yp = yout;
    npt = 0;
    nn = 0;
    jj = 0;

    for (ii=0; ii<nestpoly; ii++) {

/*
        first copy the main component as is into the output
*/
        nc = inest1[ii];
        npt = inest2[jj];
        nn = npt;
        csw_memcpy (xp, xnest, npt * sizeof(double));
        csw_memcpy (yp, ynest, npt * sizeof(double));

        xnest += npt;
        ynest += npt;

/*
        special case for no holes in this sub polygon
*/
        if (nc == 1) {
            xp += npt;
            yp += npt;
            nptsout[ii] = npt;
            jj++;
            continue;
        }

/*
        there are holes, so process them here
*/
        nbad = 0;
        NHoles = nc;
        for (i=1; i<nc; i++) {
            IHoles[i] = inest2[jj+i];
            XHoles[i] = xnest;
            YHoles[i] = ynest;
            npt = IHoles[i];
            xnest += npt;
            ynest += npt;
        }

        for (i=1; i<nc; i++) {
            istat = InsertAHole (xp, yp, nn,
                                 XHoles[i], YHoles[i], IHoles[i], &nnout);
            if (istat == 1) {
                XHoles[i] = NULL;
            }
            else {
                nbad++;
            }
            nn = nnout;
        }

/*
        reprocess the "bad" holes until there are no bad holes left
        or until the loop has been run as many times as there are
        number of holes.  If there are still bad holes after this
        many tries, there is something strange about the polygon or
        there is a bug in the InsertAHole function.
*/

        ntimes = 0;
        while (nbad > 0) {
            nbad = 0;
            for (i=1; i<nc; i++) {
                if (XHoles[i]) {
                    istat = InsertAHole (xp, yp, nn,
                                         XHoles[i], YHoles[i], IHoles[i], &nnout);
                    if (istat == 1) {
                        XHoles[i] = NULL;
                    }
                    else {
                        nbad++;
                    }
                }
            }
            if (ntimes > nc) {
                return -1;
            }
            ntimes++;
        }

/*
        update the input and putput pointers for the
        next sub polygon and continue the loop
*/

        xp += nn;
        yp += nn;
        jj += nc;

        nptsout[ii] = nn;

    }

/*
    csw_Free the workspace memory and return success
*/

    *npolyout = nestpoly;

    return 1;

}  /*  end of function ply_AddCutLines  */



/*
  ****************************************************************

                   I n s e r t A H o l e

  ****************************************************************

    Insert a single hole at a spot where its cut lines are all inside
  the polygon and all outside the hole.  This is called only from
  function ply_AddCutLines.

*/

int CSWPolyCutline::InsertAHole (double *xp, double *yp, int nnin,
                        double *xhole, double *yhole, int nholein, int *nnout)
{
    double          x1, y1, x2, y2, xout, yout,
                    *xt1 = NULL, *yt1 = NULL, *xt2 = NULL,
                    *yt2 = NULL, *xt3 = NULL, *yt3 = NULL;
    int             i, j, istat, k,
                    nn, jsav, isav, nhole, ncopy;

    CSWPolyTraverse  ply_traverse_obj;

/*
    make local copies of parameters that need to be
    passed by address to other functions.
*/

    nn = nnin;
    nhole = nholein;

/*
    initialize the nuber of output points to the number
    of input points in case the hole can't be added yet
*/

    *nnout = nn;

/*
    for each point on the hole, try connecting the hole point
    with each point on the polygon.  If the line segment from
    a connection does not intersect the polygon or any hole,
    other than at its endpoints, then this line segment can
    be used as the cutline.  It is possible that no line segment
    meets these criteria if holes that have not been processed 
    yet block access from the current hole being processed to all
    points in the polygon.  In this case a -1 is returned and the
    calling function handles the condition.
*/

    jsav = -1;
    isav = -1;

    for (i=0; i<nhole; i++) {
        x1 = xhole[i];
        y1 = yhole[i];
        isav = i;

        for (j=0; j<nn; j++) {
            x2 = xp[j];
            y2 = yp[j];

/*
            check for intersection with the already processed polygon
*/

            istat = ply_traverse_obj.ply_ClipVecToArea (&xp, &yp, 1, &nn,
                                       x1, y1, x2, y2,
                                       &xout, &yout);

/*
            check for intersection with unprocessed holes
*/

            if (istat != 0) {

                for (k=1; k<NHoles; k++) {
                    xt2 = XHoles[k];
                    yt2 = YHoles[k];
                    istat = 3;
                    if (xt2  != NULL) {
                        istat = ply_traverse_obj.ply_ClipVecToArea 
                                                  (&xt2, &yt2, 1, IHoles+k,
                                                   x1, y1, x2, y2, &xout, &yout);
                        if (istat == 0) {
                            break;                    
                        }
                    }
                }

                if (istat != 0) {
                    jsav = j;
                    if (jsav == 0) {
                        jsav = nn - 1;
                    }
                    xt1 = xp + jsav;
                    yt1 = yp + jsav;
                    goto FOUND_CUT_LINE;
                }
            }
        }
    }

/*
    no suitable cut line was found, return -1 and let the
    calling function handle the condition
*/

    return -1;


/*
    this label is the destination for having found a suitable cut line 
    to use for connecting the hole and the main polygon
*/

FOUND_CUT_LINE:

/*
    if the polygon end of the cut line is not at the last point 
    of the polygon, I have to move the polygon data to make room
    for the hole and its connecting line.
*/

    if (jsav != nn-1) {
        xt3 = xt1 + nhole + 2;
        yt3 = yt1 + nhole + 2;
        ncopy = (nn - jsav - 1) * sizeof(double);
        csw_memcpy (Tmp, xt1 + 1, ncopy);
        csw_memcpy (xt3, Tmp, ncopy);
        csw_memcpy (Tmp, yt1 + 1, ncopy);
        csw_memcpy (yt3, Tmp, ncopy);
    }

/*
    insert the hole points at the appropriate spots 
    in the polygon
*/

    for (i = isav; i < nhole; i++) {
        xt1++;
        yt1++;
        *xt1 = xhole[i];
        *yt1 = yhole[i];
    }

    for (i=0; i<isav; i++) {
        xt1++;
        yt1++;
        *xt1 = xhole[i];
        *yt1 = yhole[i];
    }

    xt1++;
    yt1++;
    *xt1 = xp[jsav];
    *yt1 = yp[jsav];

    *nnout = nn + nhole + 1;

    return 1;

}  /*  end of InsertAHole function  */

