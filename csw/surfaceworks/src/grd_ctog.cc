
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_ctog.cc

    Implementation of the CSWGrdCtog class methods.
*/

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/ply_utils.h"

#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_ctog.h"




/*
 *****************************************************************************

                         g r d _ c o n t o u r s _ o n l y

 *****************************************************************************

  This is only called from grd_contour_to_grid (in grd_utils.c) if there are
  no individual points (i.e. only contour lines).  Faults are supported and
  they must have been set up to be recoverable via grd_get_current_fault_vectors
  by the calling function.

*/

int CSWGrdCtog::grd_contours_only (COntourDataStruct *clist,
                       int nclist,
                       double xmin, double ymin, double xmax, double ymax,
                       int ncol, int nrow,
                       CSW_F *grid, char *mask)
{
    int                i, j, n, ntot, npts, istat;
    double             *xp = NULL, *yp = NULL, zlev;
    double             x1, y1, x2, y2, z1, z2;
    COntourDataStruct  *cptr = NULL;

    double             *xfault = NULL, *yfault = NULL, *zfault = NULL;
    int                *ifault = NULL, nfault;

    bool          bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            FreeData ();
        }
        csw_Free (xfault);
        csw_Free (ifault);
    };
    CSWScopeGuard func_scope_guard (fscope);


    FaultFlag = 0;

/*
 * Since this only gets called from grd_contour_to_grid,
 * assume that the input parameters are valid.
 */
    
/*
 * Allocate work grid and crossing arrays.
 */
    ntot = ncol * nrow;
    Wgrid = (CSW_F *)csw_Malloc (ntot * sizeof(CSW_F));
    if (Wgrid == NULL) {
        return -1;
    }

    Hcross = (_CRossing *)csw_Malloc (2 * ntot * sizeof(_CRossing));
    if (Hcross == NULL) {
        return -1;
    }
    Vcross = Hcross + ntot;

    ClosestCrossing = (int *)csw_Malloc (ntot * sizeof(int));
    if (ClosestCrossing == NULL) {
    }

    for (i=0; i<ntot; i++) {
        Wgrid[i] = 1.e30f;
        Hcross[i].pct1 = -1.0;
        Vcross[i].pct1 = -1.0;
        Hcross[i].pct2 = -1.0;
        Vcross[i].pct2 = -1.0;
        Hcross[i].ncross = 0;
        Vcross[i].ncross = 0;
        ClosestCrossing[i] = -1;
    }

    Xmin = xmin;
    Ymin = ymin;
    Xmax = xmax;
    Ymax = ymax;
    Ncol = ncol;
    Nrow = nrow;
    Xspace = (Xmax - Xmin) / (double)(ncol - 1);
    Yspace = (Ymax - Ymin) / (double)(nrow - 1);

    Zmin = 1.e30;
    Zmax = -1.e30;

    for (i=0; i<nclist; i++) {
        cptr = clist + i;
        xp = cptr->x;
        yp = cptr->y;
        npts = cptr->npts;
        zlev = cptr->zlev;
        if (zlev < Zmin) zlev = Zmin;
        if (zlev > Zmax) zlev = Zmax;
        for (j=0; j<npts-1; j++) {
            x1 = xp[j];
            y1 = yp[j];
            x2 = xp[j+1];
            y2 = yp[j+1];
            SetVectorCrossings (x1, y1, zlev, x2, y2, zlev);
        }
    }

    xfault = NULL;
    yfault = NULL;
    zfault = NULL;
    ifault = NULL;
    nfault = 0;

    grd_fault_ptr->grd_get_current_fault_vectors (&xfault, &yfault, &zfault,
                                   &ifault, &nfault);

    if (nfault > 0) {
        FaultFlag = 1;
        n = 0;
        for (i=0; i<nfault; i++) {
            for (j=0; j<ifault[i]-1; j++) {
                x1 = xfault[n];
                y1 = yfault[n];
                z1 = zfault[n];
                x2 = xfault[n+1];
                y2 = yfault[n+1];
                z2 = zfault[n+1];
                SetVectorCrossings (x1, y1, z1, x2, y2, z2);
                istat = AddFaultVector (x1, y1, z1,
                                        x2, y2, z2);
                if (istat == -1) {
                    return -1;
                }
                n++;
            }
            n++;
        }
        nfault = 0;
    }

    Ztiny = (Zmax - Zmin) / 1000.0;
    if (Ztiny < 0.0) Ztiny = 0.0;

    istat = CalculateCornerPoints ();
    if (istat == 1) {
        return -1;
    }

    istat = InterpolateOnBoundary ();
    if (istat == 1) {
        return -1;
    }

    bsuccess = true;

    return 1;

}


/*
 **************************************************************************

                     S e t V e c t o r C r o s s i n g s

 **************************************************************************

  Set the crossing location on each segment of the horizontal and vertical 
  crossing arrays for the specified segment.  Where the segment crosses a
  row or column of the grid, the cell side corresponding to the location
  of the intersection with the particular row or column is set to record
  the crossing location and its z value.  If more than one contour line
  cross the same cell side, the subsequent crossing is averaged with the
  average of all previous crossings.  I know this is not accurate, but 
  if there are 3 or more crossings of the same cell side, a finer grid is
  needed anyway for good results.

*/

void CSWGrdCtog::SetVectorCrossings (double x1, double y1, double z1,
                                double x2, double y2, double z2)
{
    int              irow, jcol, i, i1, i2;
    double           pct, x0, y0, xint, yint;
    double           xbig, ybig, zlev, dist, dint, tiny,
                     dx, dy, xpct;
    _CRossing        *crptr;

    CSWPolyUtils    ply_utils_obj;

/*
 * If the vector is entirely outside the grid, do nothing.
 */
    if (x1 < Xmin  &&  x2 < Xmin) return;
    if (x1 > Xmax  &&  x2 > Xmax) return;
    if (y1 < Ymin  &&  y2 < Ymin) return;
    if (y1 > Ymax  &&  y2 > Ymax) return;

    xbig = Xspace * 1000.0;
    ybig = Yspace * 1000.0;
    tiny = (Xspace + Yspace) / 2000.0;

/*
 * Find the bottom and top row intersected by the vector.
 */
    if (y2 > y1) {
        i1 = (int)((y1 - Ymin) / Yspace);
        if (y1 > Ymin) i1++;
        i2 = (int)((y2 - Ymin) / Yspace);
    }
    else {
        i1 = (int)((y2 - Ymin) / Yspace);
        if (y2 > Ymin) i1++;
        i2 = (int)((y1 - Ymin) / Yspace);
    }

    if (i1 < 0) i1 = 0;
    if (i2 > Nrow-1) i2 = Nrow - 1;

    dx = x2 - x1;
    dy = y2 - y1;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);

/*
 * For each row in the range, find the intersection point
 * with the vector and set the cell crossing at that point.
 */
    for (i=i1; i<=i2; i++) {
        y0 = Ymin + i * Yspace;
        ply_utils_obj.ply_segint (x1, y1, x2, y2,
                    Xmin - xbig, y0, Xmax + xbig, y0, 
                    &xint, &yint);
        if (xint < Xmin  ||  xint > Xmax) {
            continue;
        }
        dx = xint - x1;
        dy = yint - y1;
        dint = dx * dx + dy * dy;
        dint = sqrt (dint);
        if (dist < tiny) {
            zlev = z1;
        }
        else {
            xpct = dint / dist;
            zlev = z1 + dx * xpct;
        }
        jcol = (int)((xint - Xmin) / Xspace + 0.01);
        crptr = Hcross + Ncol * i + jcol;
        x0 = Xmin + jcol * Xspace;
        pct = (xint - x0) / Xspace;
        if (pct < 0.01) pct = 0.01;
        if (pct > .99) pct = .99;
        SetCrossingStruct (crptr, pct, zlev);
    }

/*
 * Find the left and right columns crossed by the vector.
 */
    if (x2 > x1) {
        i1 = (int)((x1 - Xmin) / Xspace);
        if (x1 > Xmin) i1++;
        i2 = (int)((x2 - Xmin) / Xspace);
    }
    else {
        i1 = (int)((x2 - Xmin) / Xspace);
        if (x2 > Xmin) i1++;
        i2 = (int)((x1 - Xmin) / Xspace);
    }

    if (i1 < 0) i1 = 0;
    if (i2 > Ncol-1) i2 = Ncol - 1;

/*
 * For each column in the range, find the intersection point
 * with the vector and set the cell crossing at that point.
 */
    for (i=i1; i<=i2; i++) {
        x0 = Xmin + i * Xspace;
        ply_utils_obj.ply_segint (x1, y1, x2, y2,
                    x0, Ymin - ybig, x0, Ymax + ybig, 
                    &xint, &yint);
        if (yint < Ymin  ||  yint > Ymax) {
            continue;
        }
        dx = xint - x1;
        dy = yint - y1;
        dint = dx * dx + dy * dy;
        dint = sqrt (dint);
        if (dist < tiny) {
            zlev = z1;
        }
        else {
            xpct = dint / dist;
            zlev = z1 + dx * xpct;
        }
        irow = (int)((yint - Ymin) / Yspace + 0.01);
        crptr = Vcross + Ncol * irow + i;
        y0 = Ymin + irow * Yspace;
        pct = (xint - x0) / Xspace;
        if (pct < 0.01) pct = 0.01;
        if (pct > .99) pct = .99;
        SetCrossingStruct (crptr, pct, zlev);
    }

    return;

}


/*
 *******************************************************************************

                  C a l c u l a t e C o r n e r P o i n t s

 *******************************************************************************

  Calculate the 4 corner node elevations and each elevation on the grid edge.
  Do linear interpolation or extrapolation of contour crossings to get the
  corner values if possible.  If not possible, look for the closest crossings
  and try to average them.

*/

int CSWGrdCtog::CalculateCornerPoints (void)

{
    int                 i, j, i1, i2, j1, j2, offset;
    double              x0, y0, x1, y1, x2, y2, z1, z2;
    double              xpct, ypct, zt;
    _CRossing           *cp1, *cp2, *crptr;

/*
 * Bottom left corner.
 */

/*
 * Try to find 2 crossings on the bottom row.
 */
    j1 = -1;
    j2 = -1;
    i1 = -1;
    i2 = -1;
    cp1 = NULL;
    cp2 = NULL;
    for (j=0; j<Ncol; j++) {
        crptr = Hcross + j;
        if (crptr->pct1 >= 0.0) {
            if (cp1) {
                cp2 = crptr;
                j2 = j;
                break;
            }
            else {
                cp1 = crptr;
                j1 = j;
            }
        }
    }

    if (j1 >= 0  &&  j2 >= 0  &&  j1 != j2
        &&  cp1 != NULL  && cp2 != NULL) {
        x0 = Xmin + j1 * Xspace;
        x1 = x0 + cp1->pct2 * Xspace;
        x0 = Xmin + j2 * Xspace;
        x2 = x0 + cp2->pct1 * Xspace;
        z1 = cp1->zlev2;
        z2 = cp2->zlev1;
        xpct = (Xmin - x1) / (x2 - x1);
        zt = z1 + xpct * (z2 - z1);
        Wgrid[0] = (CSW_F)zt;
    }

/*
 * Look for 2 crossings on left column.
 */
    for (i=0; i<Nrow; i++) {
        crptr = Vcross + Ncol * i;
        if (crptr->pct1 >= 0.0) {
            if (cp1) {
                cp2 = crptr;
                i2 = i;
                break;
            }
            else {
                cp1 = crptr;
                i1 = i;
            }
        }
    }
    if (i1 >= 0  &&  i2 >= 0  &&  i1 != i2
        &&  cp1 != NULL  && cp2 != NULL) {
        y0 = Ymin + i1 * Yspace;
        y1 = y0 + cp1->pct2 * Yspace;
        y0 = Ymin + i2 * Yspace;
        y2 = y0 + cp2->pct1 * Yspace;
        z1 = cp1->zlev2;
        z2 = cp2->zlev1;
        ypct = (Ymin - y1) / (y2 - y1);
        zt = z1 + ypct * (z2 - z1);
        if (Wgrid[0] > 1.e20f) {
            Wgrid[0] = (CSW_F)zt;
        }
        else {
            Wgrid[0] = (CSW_F)((Wgrid[0] + zt) / 2.0);
        }
    }

/*
 * Bottom right corner
 */

/*
 * Try to find 2 crossings on the bottom row.
 */
    j1 = -1;
    j2 = -1;
    i1 = -1;
    i2 = -1;
    cp1 = NULL;
    cp2 = NULL;
    for (j=Ncol-1; j>=0; j--) {
        crptr = Hcross + j;
        if (crptr->pct1 >= 0.0) {
            if (cp1) {
                cp2 = crptr;
                j2 = j;
                break;
            }
            else {
                cp1 = crptr;
                j1 = j;
            }
        }
    }

    if (j1 >= 0  &&  j2 >= 0  &&  j1 != j2
        &&  cp1 != NULL  && cp2 != NULL) {
        x0 = Xmin + j1 * Xspace;
        x1 = x0 + cp1->pct1 * Xspace;
        x0 = Xmin + j2 * Xspace;
        x2 = x0 + cp2->pct2 * Xspace;
        z1 = cp1->zlev1;
        z2 = cp2->zlev2;
        xpct = (Xmin - x1) / (x2 - x1);
        zt = z1 + xpct * (z2 - z1);
        Wgrid[Ncol-1] = (CSW_F)zt;
    }

/*
 * Look for 2 crossings on right column.
 */
    for (i=0; i<Nrow; i++) {
        crptr = Vcross + (i + 1) * Ncol - 1;
        if (crptr->pct1 >= 0.0) {
            if (cp1) {
                cp2 = crptr;
                i2 = i;
                break;
            }
            else {
                cp1 = crptr;
                i1 = i;
            }
        }
    }
    if (i1 >= 0  &&  i2 >= 0  &&  i1 != i2
        &&  cp1 != NULL  && cp2 != NULL) {
        y0 = Ymin + i1 * Yspace;
        y1 = y0 + cp1->pct2 * Yspace;
        y0 = Ymin + i2 * Yspace;
        y2 = y0 + cp2->pct1 * Yspace;
        z1 = cp1->zlev2;
        z2 = cp2->zlev1;
        ypct = (Ymin - y1) / (y2 - y1);
        zt = z1 + ypct * (z2 - z1);
        if (Wgrid[Ncol-1] > 1.e20f) {
            Wgrid[Ncol-1] = (CSW_F)zt;
        }
        else {
            Wgrid[Ncol-1] = (CSW_F)((Wgrid[0] + zt) / 2.0);
        }
    }

/*
 * Top left corner.
 */

/*
 * Try to find 2 crossings on the top row.
 */
    j1 = -1;
    j2 = -1;
    i1 = -1;
    i2 = -1;
    cp1 = NULL;
    cp2 = NULL;
    offset = (Nrow - 1) * Ncol;
    for (j=0; j<Ncol; j++) {
        crptr = Hcross + offset + j;
        if (crptr->pct1 >= 0.0) {
            if (cp1) {
                cp2 = crptr;
                j2 = j;
                break;
            }
            else {
                cp1 = crptr;
                j1 = j;
            }
        }
    }

    if (j1 >= 0  &&  j2 >= 0  &&  j1 != j2
        &&  cp1 != NULL  && cp2 != NULL) {
        x0 = Xmin + j1 * Xspace;
        x1 = x0 + cp1->pct2 * Xspace;
        x0 = Xmin + j2 * Xspace;
        x2 = x0 + cp2->pct1 * Xspace;
        z1 = cp1->zlev2;
        z2 = cp2->zlev1;
        xpct = (Xmin - x1) / (x2 - x1);
        zt = z1 + xpct * (z2 - z1);
        Wgrid[offset] = (CSW_F)zt;
    }

/*
 * Look for 2 crossings on left column.
 */
    for (i=Nrow-1; i>=0; i--) {
        crptr = Vcross + Ncol * i;
        if (crptr->pct1 >= 0.0) {
            if (cp1) {
                cp2 = crptr;
                i2 = i;
                break;
            }
            else {
                cp1 = crptr;
                i1 = i;
            }
        }
    }
    if (i1 >= 0  &&  i2 >= 0  &&  i1 != i2
        &&  cp1 != NULL  && cp2 != NULL) {
        y0 = Ymin + i1 * Yspace;
        y1 = y0 + cp1->pct1 * Yspace;
        y0 = Ymin + i2 * Yspace;
        y2 = y0 + cp2->pct2 * Yspace;
        z1 = cp1->zlev1;
        z2 = cp2->zlev2;
        ypct = (Ymin - y1) / (y2 - y1);
        zt = z1 + ypct * (z2 - z1);
        if (Wgrid[offset] > 1.e20f) {
            Wgrid[offset] = (CSW_F)zt;
        }
        else {
            Wgrid[offset] = (CSW_F)((Wgrid[0] + zt) / 2.0);
        }
    }

/*
 * Top right corner.
 */

/*
 * Try to find 2 crossings on the top row.
 */
    j1 = -1;
    j2 = -1;
    i1 = -1;
    i2 = -1;
    cp1 = NULL;
    cp2 = NULL;
    offset = (Nrow - 1) * Ncol;
    for (j=Ncol-1; j>=0; j--) {
        crptr = Hcross + offset + j;
        if (crptr->pct1 >= 0.0) {
            if (cp1) {
                cp2 = crptr;
                j2 = j;
                break;
            }
            else {
                cp1 = crptr;
                j1 = j;
            }
        }
    }

    if (j1 >= 0  &&  j2 >= 0  &&  j1 != j2
        &&  cp1 != NULL  && cp2 != NULL) {
        x0 = Xmin + j1 * Xspace;
        x1 = x0 + cp1->pct1 * Xspace;
        x0 = Xmin + j2 * Xspace;
        x2 = x0 + cp2->pct2 * Xspace;
        z1 = cp1->zlev1;
        z2 = cp2->zlev2;
        xpct = (Xmin - x1) / (x2 - x1);
        zt = z1 + xpct * (z2 - z1);
        Wgrid[offset+Ncol-1] = (CSW_F)zt;
    }

/*
 * Look for 2 crossings on right column.
 */
    for (i=Nrow-1; i>=0; i--) {
        crptr = Vcross + Ncol * i + Ncol - 1;
        if (crptr->pct1 >= 0.0) {
            if (cp1) {
                cp2 = crptr;
                i2 = i;
                break;
            }
            else {
                cp1 = crptr;
                i1 = i;
            }
        }
    }
    if (i1 >= 0  &&  i2 >= 0  &&  i1 != i2
        &&  cp1 != NULL  && cp2 != NULL) {
        y0 = Ymin + i1 * Yspace;
        y1 = y0 + cp1->pct1 * Yspace;
        y0 = Ymin + i2 * Yspace;
        y2 = y0 + cp2->pct2 * Yspace;
        z1 = cp1->zlev1;
        z2 = cp2->zlev2;
        ypct = (Ymin - y1) / (y2 - y1);
        zt = z1 + ypct * (z2 - z1);
        if (Wgrid[offset+Ncol-1] > 1.e20f) {
            Wgrid[offset+Ncol-1] = (CSW_F)zt;
        }
        else {
            Wgrid[offset+Ncol-1] = (CSW_F)((Wgrid[0] + zt) / 2.0);
        }
    }

/*
 * If any corner has not been calculated, search for
 * nearby points and plane fit to calculate.
 */
    if (Wgrid[0] > 1.e20f) {
        PlaneFit (0);
    }
    if (Wgrid[Ncol-1] > 1.e20f) {
        PlaneFit (Ncol - 1);
    }
    offset = (Nrow - 1) * Ncol;
    if (Wgrid[offset] > 1.e20f) {
        PlaneFit (offset);
    }
    if (Wgrid[Nrow * Ncol-1] > 1.e20f) {
        PlaneFit (Nrow * Ncol - 1);
    }

    return 1;

}


/*
 ******************************************************************************

                          P l a n e F i t

 ******************************************************************************

  Fit a plane to contour crossing points found near the specified node.  
  The Wgrid value at the node is set to the plane fit value.

*/

void CSWGrdCtog::PlaneFit (int nodenum)
{
    int                irow, jcol, idelta, jdelta,
                       ii, jj, level, maxlevel, offset, k,
                       n, i1, i2, j1, j2, npts, istat;
    double             xplane[100],
                       yplane[100],
                       zplane[100],
                       coef[3],
                       x0, y0, xt, yt, zt, zmin, zmax;
    _CRossing          *crptr;

    maxlevel = Nrow;
    if (Ncol > Nrow) {
        maxlevel = Ncol;
    }

    int num_needed = 3;
    if (FaultFlag) num_needed = 10;

    zmin = 1.e30;
    zmax = -1.e30;

    n = 0;
    irow = nodenum / Ncol;
    jcol = nodenum % Ncol;
    x0 = Xmin + jcol * Xspace;
    y0 = Ymin + irow * Yspace;

/*
 * Search around the node for crossings.  If 4 are found
 * and they are not all the same z value, that is enough
 * for a plane fit.  If 99 are found of all the same z value,
 * I just use that z value.
 */
    for (level=0; level<maxlevel; level++) {

        if (n > 98) break;
        i1 = irow - level - 1;
        i2 = irow + level;
        j1 = jcol - level - 1;
        j2 = jcol + level;

        for (ii=i1; ii<=i2; ii++) {
            if (ii < 0  ||  ii >= Nrow) {
                continue;
            }
            offset = ii * Ncol;
            idelta = ii - irow;
            if (idelta < 0) idelta = -idelta;
            for (jj=j1; jj<=j2; jj++) {
                if (jj < 0  ||  jj >= Ncol) {
                    continue;
                }
                jdelta = jj - jcol;
                if (jdelta < 0) jdelta = -jdelta;
                if (idelta < level  &&  jdelta < level) {
                    continue;
                }
                k = offset + jj;
                crptr = Hcross + k;
                if (crptr->pct1 >= 0.0) {
                    xt = Xmin + jj * Xspace;
                    xt += crptr->pct1 * Xspace;
                    yt = Ymin + ii * Yspace;
                    zt = crptr->zlev1;
                    if (zt < zmin) zmin = zt;
                    if (zt > zmax) zmax = zt;
                    xt -= x0;
                    yt -= y0;
                    xplane[n] = xt;
                    yplane[n] = yt;
                    zplane[n] = zt;
                    n++;
                    if (n > 99) n = 99;
                }
                else {
                    crptr = Vcross + k;
                    if (crptr->pct1 >= 0.0) {
                        yt = Ymin + ii * Yspace;
                        yt += crptr->pct1 * Yspace;
                        xt = Xmin + jj * Xspace;
                        zt = crptr->zlev1;
                        if (zt < zmin) zmin = zt;
                        if (zt > zmax) zmax = zt;
                        xt -= x0;
                        yt -= y0;
                        xplane[n] = xt;
                        yplane[n] = yt;
                        zplane[n] = zt;
                        n++;
                        if (n > 99) n = 99;
                    }
                }
            }
        }

    /*
     * Break out of the level loop if 4 or more points are available,
     * and they are not all the same z value.
     */
        if (n > num_needed) {
            if (zmax > zmin + Ztiny) {
                break;
            }
        }

    }  /* end of level loop  */

/*
 * If less than 1 point was found, assert.
 * This should not happen !!
 */
    if (n < 1) {
        assert (0);
        return;
    }

    if (zmax - zmin < Ztiny) {
        Wgrid[nodenum] = (CSW_F)zmin;
        return;
    }

    if (FaultFlag) {
        FilterPointsForFaults (nodenum,
                               xplane, yplane, zplane, &n);
    }

/*
 * If less than 3 points are available, use the first point 
 * z value for the node.
 */
    if (n < 3) {
        Wgrid[nodenum] = (CSW_F)zplane[0];
        return;
    }

/*
 * Use the plane value at the node.
 */
    npts = n;
    istat = grd_utils_ptr->grd_calc_double_plane (xplane, yplane, zplane,
                                   npts, coef);
    if (istat == -1) {
        Wgrid[nodenum] = (CSW_F)zplane[0];
        return;
    }

/*
 * Since the node is at the origin, only the first coefficient is needed.
 */
    zt = coef[0];
        
    Wgrid[nodenum] = (CSW_F)zt;

    return;

}


/*
 ********************************************************************************

                              F r e e D a t a

 ********************************************************************************

*/

void CSWGrdCtog::FreeData (void)
{
    csw_Free (Wgrid);
    csw_Free (Hcross);
    csw_Free (ClosestCrossing);
    csw_Free (FaultVectorList);

    Wgrid = NULL;
    Hcross = NULL;
    Vcross = NULL;
    ClosestCrossing = NULL;

    FaultVectorList = NULL;
    NumFaultVectors = 0;
    MaxFaultVectors = 0;

    return;
}


/*
 ********************************************************************************

                   I n t e r p o l a t e O n B o u n d a r y

 ********************************************************************************

  This is called after all 4 corner points have been calculated.  For each
  edge of the grid, node values are interpolated linearly from the corner
  points and any crossings.  If there are no crossings, nodes are calculated
  by a plane fit to the closest interior crossings.

*/

int CSWGrdCtog::InterpolateOnBoundary (void)

{
    double          x1, y1, z1, x2, y2, z2,
                    xt, yt, zt, dx, dy, dz, xpct, ypct;
    int             i1, i2, j1, j2, i, j, k, ncross, offset;
    _CRossing       *crptr;

/*
 * Fill in the bottom row.
 *
 * First, check to see if there are any crossings.
 */
    ncross = 0;
    for (j=0; j<Ncol; j++) {
        if (Hcross[j].pct1 >= 0.0) {
            ncross = 1;
            break;
        }
    }

/*
 * There are no crossings, so do plane fits.
 */
    if (ncross == 0) {
        for (j=1; j<Ncol-1; j++) {
            PlaneFit (j);
        }
    }

/*
 * There are crossings, so interpolate between them.
 */
    else {
        x1 = Xmin;
        z1 = Wgrid[0];
        j1 = 1;
        offset = 0;
        crptr = Vcross;
        for (j=1; j<Ncol-1; j++) {
            crptr = Hcross + j;
            if (crptr->pct1 < 0.0) {
                continue;
            }
            j2 = j + offset;
            x2 = Xmin + j * Xspace;
            x2 += crptr->pct2 * Xspace;
            z2 = crptr->zlev2;
            dx = x2 - x1;
            dz = z2 - z1;
            for (k=j1; k<=j2; k++) {
                xt = Xmin + k * Xspace;
                xt -= x1;
                xpct = xt / dx;
                zt = z1 + xpct * dz;
                Wgrid[k] = (CSW_F)zt;
            }
            j1 = j2 + 1;
            x1 = x2;
            z1 = z2;
        }
        x2 = Xmax;
        j2 = Ncol - 1;
        z2 = Wgrid[Ncol-1];
        dx = x2 - x1;
        dz = z2 - z1;
        for (k=j1; k<j2; k++) {
            xt = Xmin + k * Xspace;
            xt -= x1;
            xpct = xt / dx;
            zt = z1 + xpct * dz;
            Wgrid[k] = (CSW_F)zt;
        }
    }

/*
 * Fill in the top row in a similar fashion as the bottom row.
 *
 * First, check to see if there are any crossings.
 */
    ncross = 0;
    offset = Ncol * (Nrow - 1);
    for (j=0; j<Ncol; j++) {
        if (Hcross[j+offset].pct1 >= 0.0) {
            ncross = 1;
            break;
        }
    }

/*
 * There are no crossings, so do plane fits.
 */
    if (ncross == 0) {
        for (j=1; j<Ncol-1; j++) {
            PlaneFit (j+offset);
        }
    }

/*
 * There are crossings, so interpolate between them.
 */
    else {
        x1 = Xmin;
        z1 = Wgrid[offset];
        j1 = 1;
        crptr = Vcross;
        for (j=1; j<Ncol-1; j++) {
            crptr = Hcross + j + offset;
            if (crptr->pct1 < 0.0) {
                continue;
            }
            j2 = j;
            x2 = Xmin + j * Xspace;
            x2 += crptr->pct2 * Xspace;
            z2 = crptr->zlev2;
            dx = x2 - x1;
            dz = z2 - z1;
            for (k=j1; k<=j2; k++) {
                xt = Xmin + k * Xspace;
                xt -= x1;
                xpct = xt / dx;
                zt = z1 + xpct * dz;
                Wgrid[k+offset] = (CSW_F)zt;
            }
            j1 = j2 + 1;
            x1 = x2;
            z1 = z2;
        }
        x2 = Xmax;
        j2 = Ncol - 1;
        z2 = Wgrid[Ncol*Nrow-1];
        dx = x2 - x1;
        dz = z2 - z1;
        for (k=j1; k<j2; k++) {
            xt = Xmin + k * Xspace;
            xt -= x1;
            xpct = xt / dx;
            zt = z1 + xpct * dz;
            Wgrid[k+offset] = (CSW_F)zt;
        }
    }

/*
 * Do the left side.
 *
 * First, check to see if there are any crossings.
 */
    ncross = 0;
    for (i=0; i<Nrow; i++) {
        if (Vcross[i*Ncol].pct1 >= 0.0) {
            ncross = 1;
            break;
        }
    }

/*
 * There are no crossings, so do plane fits.
 */
    if (ncross == 0) {
        for (i=1; i<Nrow-1; i++) {
            PlaneFit (i*Ncol);
        }
    }

/*
 * There are crossings, so interpolate between them.
 */
    else {
        y1 = Ymin;
        z1 = Wgrid[0];
        i1 = 1;
        crptr = Vcross;
        for (i=1; i<Nrow-1; i++) {
            crptr = Vcross + i * Ncol;
            if (crptr->pct1 < 0.0) {
                continue;
            }
            i2 = i;
            y2 = Ymin + i * Yspace;
            y2 += crptr->pct2 * Yspace;
            dy = y2 - y1;
            z2 = crptr->zlev2;
            dz = z2 - z1;
            for (k=i1; k<=i2; k++) {
                yt = Ymin + k * Yspace;
                yt -= y1;
                ypct = yt / dy;
                zt = z1 + ypct * dz;
                Wgrid[k*Ncol] = (CSW_F)zt;
            }
            i1 = i2;
            y1 = y2;
            z1 = z2;
        }
        y2 = Ymax;
        z2 = Wgrid[Ncol * (Nrow - 1)];
        i2 = Ncol - 1;
        dy = y2 - y1;
        dz = z2 - z1;
        for (k=i1; k<i2; k++) {
            yt = Ymin + k * Yspace;
            yt -= y1;
            ypct = yt / dy;
            zt = z1 + ypct * dz;
            Wgrid[k*Ncol] = (CSW_F)zt;
        }
    }

/*
 * Do the right side.
 *
 * First, check to see if there are any crossings.
 */
    ncross = 0;
    offset = Ncol - 1;
    for (i=0; i<Nrow; i++) {
        if (Vcross[i*Ncol+offset].pct1 >= 0.0) {
            ncross = 1;
            break;
        }
    }

/*
 * There are no crossings, so do plane fits.
 */
    if (ncross == 0) {
        for (i=1; i<Nrow-1; i++) {
            PlaneFit ((i+1)*Ncol-1);
        }
    }

/*
 * There are crossings, so interpolate between them.
 */
    else {
        y1 = Ymin;
        z1 = Wgrid[0];
        i1 = 1;
        offset = Ncol - 1;
        crptr = Vcross + offset;
        for (i=1; i<Nrow-1; i++) {
            crptr = Vcross + i * Ncol + offset;
            if (crptr->pct1 < 0.0) {
                continue;
            }
            if (crptr->pct2 < 0.0) {
                continue;
            }
            i2 = i;
            y2 = Ymin + i * Yspace;
            y2 += crptr->pct2 * Yspace;
            dy = y2 - y1;
            z2 = crptr->zlev2;
            dz = z2 - z1;
            for (k=i1; k<=i2; k++) {
                yt = Ymin + k * Yspace;
                yt -= y1;
                ypct = yt / dy;
                zt = z1 + ypct * dz;
                Wgrid[k*Ncol+offset] = (CSW_F)zt;
            }
            i1 = i2;
            y1 = y2;
            z1 = z2;
        }
        y2 = Ymax;
        z2 = Wgrid[Ncol * Nrow - 1];
        i2 = Ncol - 1;
        dy = y2 - y1;
        dz = z2 - z1;
        for (k=i1; k<i2; k++) {
            yt = Ymin + k * Yspace;
            yt -= y1;
            ypct = yt / dy;
            zt = z1 + ypct * dz;
            Wgrid[k*Ncol+offset] = (CSW_F)zt;
        }
    }

/*
 * If there are still any uncalculated nodes on the
 * perimeter, use plane fits to calculate them.
 */
    offset = Ncol * (Nrow - 1);
    for (j=1; j<Ncol; j++) {
        if (Wgrid[j] > 1.e20f) {
            PlaneFit (j);
        }
        if (Wgrid[j+offset] > 1.e20f) {
            PlaneFit (j+offset);
        }
    }

    offset = Ncol - 1;
    for (i=0; i<Nrow; i++) {
        if (Wgrid[i*Ncol] > 1.e20f) {
            PlaneFit (i*Ncol);
        }
        if (Wgrid[i*Ncol+offset] > 1.e20f) {
            PlaneFit (i*Ncol+offset);
        }
    }

    return 1;

}


/*
 *********************************************************************************

               I n t e r p o l a t e B e t w e e n C r o s s i n g s

 *********************************************************************************

  The Wgrid array must have all of its boundary nodes calculated prior to
  calling this function.  Also, both the horizontal and vertical crossing
  arrays (Hcross and Vcross respectively) must have been set.

  This function does a linear interpolation based on the Hcross array,
  followed by a linear interpolation based on the Vcross array.  The 
  Vcross values are blended with the Hcross values with weighting higher
  for the values closer to a crossing point.

  If both the Hcross and Vcross data are based on equal z values, the
  node being interpolated is on a plateau.  Special handling involving
  extrapolation from nearby non plateau nodes is used to make the plateau
  slightly different from the contour defining it.

  If a csw_Malloc failure occurs, -1 is returned.  Otherwise, 1 is returned.
*/

int CSWGrdCtog::InterpolateBetweenCrossings (void)
{
    int           i, j, k, i1, i2, j1, j2, hdist, vdist, 
                  kk, offset;
    int           *idist = NULL;
    CSW_F         *data = NULL;
    double        x1, y1, z1, x2, y2, z2, xt, yt, zt,
                  zhoriz, zvert, whoriz, wvert,
                  dx, dy, dz, xpct, ypct, tiny;
    _CRossing     *crptr = NULL;


    auto fscope = [&]()
    {
        csw_Free (data);
    };
    CSWScopeGuard func_scope_guard (fscope);


    idist = ClosestCrossing;
    tiny = (Xspace + Yspace) / 200.0;

/*
 * Allocate local work space in the data array.
 */
    data = (CSW_F *)csw_Malloc (Ncol * Nrow * sizeof(CSW_F));
    if (data == NULL) {
        return -1;
    }

/*
 * Initialize the interior of the work space as not yet
 * calculated.  Transfer the boundary from the Wgrid
 * array.
 */
    for (i=0; i<Ncol*Nrow; i++) {
        data[i] = 1.e30f;
    }

/*
 * Bottom and top rows.
 */
    offset = Ncol * (Nrow - 1);
    for (j=0; j<Ncol; j++) {
        data[j] = Wgrid[j];
        data[j+offset] = Wgrid[j+offset];
    }

/*
 * Left and right columns.
 */
    offset = Ncol - 1;
    for (i=0; i<Nrow; i++) {
        data[i*Ncol] = Wgrid[i*Ncol];
        data[i*Ncol+offset] = Wgrid[i*Ncol+offset];
    }

/*
 * Interpolate the horizontal crossings a row at a time.
 * Note that since the outer edge of the Wgrid array is
 * already calculated, the rows are scanned from 1 to
 * Ncol - 2 rather than from 0 to Ncol -1.  Also, the 
 * bottom and top rows do not need to be interpolated.
 */
    for (i=1; i<Nrow-1; i++) {

    /*
     * For each row, use the left most node as a pseudo
     * crossing for interpolation purposes.
     */
        offset = i * Ncol;
        x1 = Xmin;
        j1 = 1;
        z1 = Wgrid[offset];

        for (j=1; j<Ncol-1; j++) {

        /*
         * Find an occupied horizontal crossing in the row.
         */
            crptr = Hcross + offset + j;
            if (crptr->pct1 < 0.0) {
                continue;
            }

        /*
         * Use the previous and current crossings as
         * the endpoints for the linear interpolation.
         */
            j2 = j;
            x2 = j * Xspace + Xmin;
            x2 += crptr->pct1 * Xspace;
            z2 = crptr->zlev1;
            dx = x2 - x1;
            if (dx < tiny) {
                dx = tiny;
            }
            dz = z2 - z1;

        /*
         * loop through the nodes inside the bracketing
         * crossings and set them by linear interpolation.
         */
            for (k=j1; k<=j2; k++) {
                hdist = j2 - k;
                if (j1 != 1) {
                    if (k - j1 < hdist) hdist = k - j1;
                }
                idist[offset + k] = hdist;
                xt = k * Xspace + Xmin;
                xt -= x1;
                xpct = xt / dx;
                zt = z1 + xpct * dz;
                data[offset+k] = zt;
            }

         /*
          * The first crossing for the next bracket is set
          * to the last crossing for this bracket.
          */
            j1 = j2 + 1;
            x1 = j * Xspace + Xmin;
            x1 += crptr->pct2 * Xspace;
            z1 = crptr->zlev2;

        } /* end of j loop through columns of a single row */

    /*
     * A final loop to the end of the row is needed.  If no
     * crossings were found in the row, the interpolation is
     * done between the end nodes, but the distance to nearest
     * crossing is set very high so these node values will be
     * weighted very little when blended with the value 
     * from a closer vertical crossing.
     */
        j2 = Ncol - 1;
        x2 = Xmax;
        z2 = Wgrid[offset+Ncol-1];
        dx = x2 - x1;
        if (dx < tiny) dx = tiny;
        dz = z2 - z1;
        for (k=j1; k<j2; k++) {
            hdist = k - j1;
            if (j1 == 1) {
                hdist = Ncol;
            }
            idist[offset+k] = hdist;
            xt = k * Xspace + Xmin;
            xt -= x1;
            xpct = xt / dx;
            zt = z1 + xpct * dz;
            data[offset+k] = zt;
        }

    } /* end of i loop through rows of the Hcross array */ 

/*
 * Do the Vcross array in a similar fashion, with the exception
 * of blending the z value based on closest crossing distance.
 * The idist array (same as the ClosestCrossing array) will be
 * set to the smallest of the horizontal or vertical crossing
 * distance for use later.
 */
 
/*
 * Loop through the columns in the Vcross array.
 */
    for (j=1; j<Ncol-1; j++) {
        
        y1 = Ymin;
        i1 = 1;
        z1 = Wgrid[j];
        
        for (i=1; i<Nrow-1; i++) {

            offset = i * Ncol;
            crptr = Vcross + offset + j;
            if (crptr->pct1 < 0.0) {
                continue;
            }

            i2 = i;
            y2 = i * Yspace + Ymin;
            y2 += crptr->pct1 * Yspace;
            z2 = crptr->zlev1;
            dy = y2 - y1;
            if (dy < tiny) dy = tiny;
            dz = z2 - z1;

        /*
         * Scan through the nodes bracketed by the
         * vertical crossings.  Use this blended
         * with any previous horizontally derived
         * values for the final data array value.
         */
            for (k=i1; k<=i2; k++) {

                kk = Ncol * k + j;
                vdist = i2 - k;
                if (i1 != 1) {
                    if (k-i1 < vdist) vdist = k - i1;
                }
                hdist = idist[kk];
                if (vdist < hdist) idist[kk] = vdist;

                yt = k * Yspace + Ymin;
                yt -= y1;
                ypct = yt / dy;
                zt = z1 + ypct * dz;
                zvert = zt;
               
            /*
             * weights for the horizontal and vertical values
             */
                if (hdist >= 0) {
                    whoriz = 1.0 / (double)(hdist + 1);
                }
                else {
                    whoriz = 0.001;
                }
                if (vdist >= 0) {
                    wvert = 1.0 / (double)(vdist + 1);
                }
                else {
                    wvert = 0.001;
                }

                whoriz *= whoriz;
                wvert *= wvert;

            /*
             * Blend the vertical and horizontal values.
             */
                zhoriz = data[kk];
                if (zhoriz > 1.e20) {
                    data[kk] = zvert;
                }
                else {
                    data[kk] = (zvert * wvert + zhoriz * whoriz) / 
                               (wvert + whoriz);
                }

            }

            i1 = i2 + 1;
            y1 = i * Yspace + Ymin;
            y1 += crptr->pct2 * Yspace;
            z1 = crptr->zlev2;

        /* end of i loop through an individual column of the Vcross array */
        }

    /*
     * Fill in to the top row of the column.
     */
        i2 = Nrow - 1;
        y2 = Ymax;
        z2 = Wgrid[Ncol*(Nrow-1)+j];
        dy = y2 - y1;
        if (dy < tiny) dy = tiny;
        dz = z2 - z1;

        for (k=i1; k<i2; k++) {

            kk = Ncol * k + j;
            vdist = k - i1;
            if (i1 == 1) {
                vdist = Nrow;
            }
            hdist = idist[kk];
            if (vdist < hdist) idist[kk] = vdist;

            yt = k * Yspace + Ymin;
            yt -= y1;
            ypct = yt / dy;
            zt = z1 + ypct * dz;
               
        /*
         * weights for the horizontal and vertical values
         */
            if (hdist >= 0) {
                whoriz = 1.0 / (double)(hdist + 1);
            }
            else {
                whoriz = 0.001;
            }
            if (vdist >= 0) {
                wvert = 1.0 / (double)(vdist + 1);
            }
            else {
                wvert = 0.001;
            }

            whoriz *= whoriz;
            wvert *= wvert;

        /*
         * Blend the vertical and horizontal values.
         */
            zhoriz = data[kk];
            if (zhoriz > 1.e20) {
                data[kk] = zvert;
            }
            else {
                data[kk] = (zvert * wvert + zhoriz * whoriz) / 
                           (wvert + whoriz);
            }

        }

    } /* end of i loop through rows of the Hcross array */ 

    return 1;

}  /* end of InterpolateBetweenCrossings function */



/*
 ******************************************************************************

                    S e t C r o s s i n g S t r u c t

 ******************************************************************************

  The crossing structure percent and zlevel is set if needed.  Only the outer
  most crossings are saved.  Thus, if there are already 2 crossings of the
  side, if the 3rd percent is between the first two, it will not be used.

  If there is only one crossing, both the percenta and both the z levels
  are set identically to that crossing value.

  This guarantees that the crossings closest to the nodes are always saved.

*/

void CSWGrdCtog::SetCrossingStruct (_CRossing *crptr,
                               double pct,
                               double zlev)
{

    if (crptr->ncross == 0) {
        crptr->pct1 = pct;
        crptr->zlev1 = zlev;
        crptr->pct2 = pct;
        crptr->zlev2 = zlev;
        crptr->ncross = 1;
    }
    else {
        if (pct < crptr->pct1) {
            crptr->pct1 = pct;
            crptr->zlev1 = zlev;
        }
        else if (pct > crptr->pct2) {
            crptr->pct2 = pct;
            crptr->zlev2 = zlev;
        }
    }
        
    return; 

}


/*
 *****************************************************************************

                          A d d F a u l t V e c t o r

 *****************************************************************************

  Add a vector to the fault vector list, growing the list as needed.  This list
  is used by FilterPointsForFaults to determine if points are on the same side 
  of faults.

*/

int CSWGrdCtog::AddFaultVector (double x1, double y1, double z1,
                           double x2, double y2, double z2)
{
    _FAultVector     *ptr;

    ptr = FaultVectorList;
    if (NumFaultVectors >= MaxFaultVectors) {
        MaxFaultVectors += 1000;
        ptr = (_FAultVector *)csw_Realloc
                          (FaultVectorList, MaxFaultVectors * sizeof (_FAultVector));
    }
    if (ptr == NULL) {
        FreeData ();
        return -1;
    }
    FaultVectorList = ptr;

    ptr = FaultVectorList + NumFaultVectors;
    ptr->x1 = x1;
    ptr->y1 = y1;
    ptr->z1 = z1;
    ptr->x2 = x2;
    ptr->y2 = y2;
    ptr->z2 = z2;

    NumFaultVectors++;

    return 1;

}


/*
 **********************************************************************************

                      F i l t e r P o i n t s F o r F a u l t s

 **********************************************************************************

  If any of the list of points is across any fault line from the specified
  node, do not include it in the output point list.

*/

void CSWGrdCtog::FilterPointsForFaults (
                                   int nodenum,
                                   double *x,
                                   double *y,
                                   double *z,
                                   int *nptsin)
{
    double                x1, y1, x2, y2, x3, y3, x4, y4, xint, yint;
    _FAultVector          *fptr;
    int                   i, n, j, npts, irow, jcol, intflag;
    int                   istat = -1;
    CSWPolyUtils    ply_utils_obj;
    
    if (NumFaultVectors < 1  ||  FaultVectorList == NULL) {
        return;
    }

    irow = nodenum / Ncol;
    jcol = nodenum % Ncol;
    x1 = Xmin + jcol * Xspace;
    y1 = Ymin + irow * Yspace;

    npts = *nptsin;
    n = 0;

    for (i=0; i<npts; i++) {

        x2 = x[i];
        y2 = y[i];

        for (j=0; j<NumFaultVectors; j++) {

            fptr = FaultVectorList + j;
            x3 = fptr->x1;
            y3 = fptr->y1;
            x4 = fptr->x2;
            y4 = fptr->y2;
            istat = ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4, 
                                &xint, &yint);
            if (istat == 0) {
                intflag = 1;
                break;
            }
        }

        if (intflag == 0) {
            x[n] = x[i];
            y[n] = y[i];
            n++;
        }
    }

    *nptsin = n;

    return;

}
        
        
