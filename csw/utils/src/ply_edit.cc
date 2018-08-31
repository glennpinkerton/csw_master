
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*  
    ply_edit.c

      This file has utility functions for modifying a polygon
    boundary by adding extra points.
*/


/*
    system header files
*/
#include <assert.h>
#include <stdlib.h>
#include <memory>

/*
    application header files
*/

#include "csw/utils/include/csw_.h"

#include "csw/utils/private_include/gpf_utils.h"

#include "csw/utils/private_include/ply_edit.h"
#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_graph.h"

#include "csw/utils/private_include/csw_scope.h"



/*
 **************************************************************************

         p l y _ a d d _ p o i n t s _ t o _ b o u n d a r y

 **************************************************************************

  Add the specified points by splitting the polygon edge closest in 
  perpendicular distance to each point.  

*/

int CSWPolyEdit::ply_add_points_to_boundary (double *xp, double *yp, void **vp,
                                int *icomp, int *ipts, int npoly,
                                double *xpts, double *ypts,
                                void **vpts, int npts,
                                double *xout, double *yout, void **vpout,
                                int *icout, int *ipout, int *nout,
                                int npmax, int ncmax)
{
    int             i, j, k, n, nc, istat, last;
    double          xmin, ymin, xmax, ymax;

    FreeMem ();    

    auto fscope = [&]()
    {
        FreeMem ();
    };
    CSWScopeGuard  func_scope_guard (fscope);


    if (npoly < MAX_STATIC_POLYS) {
        StartPolyList = StaticStartPolyList;
    }
    else {
        StartPolyList = (int *)csw_Malloc (npoly * sizeof(int));
    }

    if (StartPolyList == NULL) {
        FreeMem ();
        return -1;
    }

    nc = 0;
    for (i=0; i<npoly; i++) {
        for (j=0; j<icomp[i]; j++) {
            nc++;
        }
    }

    if (nc < MAX_STATIC_POLYS) {
        StartCompList = StaticStartCompList;
    }
    else {
        StartCompList = (int *)csw_Malloc (nc * sizeof(int));
    }

    if (StartCompList == NULL) {
        FreeMem ();
        return -1;
    }

    Npoly = npoly;
    Ncomp = nc;
    Icomp = icomp;

    Xout = xout;
    Yout = yout;
    Vpout = vpout;
    Icout = icout;
    Ipout = ipout;
    Nout = 0;
    Npmax = npmax;
    Ncmax = ncmax;

/*
 *  Fill in the boundary point link list.
 */
    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;

    n = 0;
    nc = 0;
    for (i=0; i<npoly; i++) {
        for (j=0; j<icomp[i]; j++) {
            last = -1;
            StartCompList[nc] = NumBPList;
            for (k=0; k<ipts[nc]; k++) {
                if (xp[n] < xmin) xmin = xp[n];
                if (yp[n] < ymin) ymin = yp[n];
                if (xp[n] > xmax) xmax = xp[n];
                if (yp[n] > ymax) ymax = yp[n];
                istat = AppendToBPList (xp[n], yp[n], vp[n], last);
                if (istat == -1) {
                    FreeMem ();
                    return -1;
                }
                last = n;
                n++;
            }
            nc++;
        }
    }

    SamePointDistance = (xmax - xmin + ymax - ymin) / 200000.0;

/*
 * Split the edges as needed.
 */
    for (i=0; i<npts; i++) {
        istat = SplitClosestEdge (xpts[i], ypts[i], vpts[i]);
        if (istat == -1) {
            FreeMem ();
            return -1;
        }
    }

/*
 * The new points may have changed the topology of the polygon,
 * so union all the components together to output a new polygon.
 */
    istat = UnionNewComponents ();
    if (istat == -1) {
        FreeMem ();
        return -1;
    }

    *nout = Nout;

    FreeMem ();

    return 1;

}  /* end of function ply_add_points_to_boundary */
        
                


/*
 **********************************************************************

                           F r e e M e m

 **********************************************************************

*/

void CSWPolyEdit::FreeMem (void)
{
    if (StartCompList != StaticStartCompList) {
        csw_Free (StartCompList);
    }
    if (StartPolyList != StaticStartPolyList) {
        csw_Free (StartPolyList);
    }

    csw_Free (BPList);
    BPList = NULL;
    NumBPList = 0;
    MaxBPList = 0;

    Xout = NULL;
    Yout = NULL;
    Vpout = NULL;
    Icout = NULL;
    Ipout = NULL;
    Nout = 0;
    Npoly = 0;
    Ncomp = 0;

    return;

}  /* end of FreeMem function */



/*
 **************************************************************************

                    A p p e n d T o B P L i s t

 **************************************************************************

  Add an item to the link list of boundary points.

*/

int CSWPolyEdit::AppendToBPList (double x, double y,
                           void *vp, int iprev)
{
    _BOundaryPoint     *bptr = NULL;
    int                inow;

/*
 * Grow the boundary point list if needed.
 */
    if (NumBPList >= MaxBPList) {
        MaxBPList += BPChunk;
        BPList = (_BOundaryPoint *)csw_Realloc
                 (BPList, MaxBPList * sizeof(_BOundaryPoint));
    }

    if (BPList == NULL) {
        return -1;
    }

/*
 * Add to the end and link to the previous.
 */
    bptr = BPList + NumBPList;
    bptr->x = x;
    bptr->y = y;
    bptr->vp = vp;
    bptr->flag = 0;
    bptr->iprev = iprev;
    bptr->inext = -1;
    if (iprev >= 0) {
        bptr = BPList + iprev;
        bptr->inext = NumBPList;
    }
    inow = NumBPList;

    NumBPList++;

    return inow;

} /* end of AppendToBPList function */




/*
 *************************************************************************

                   S p l i t C l o s e s t E d g e

 *************************************************************************

  Find the polygon edge closest (in perpendicular distance) to the 
  specified point.  Split that edge into two edges with the specified
  point common to each new edge.

*/

int CSWPolyEdit::SplitClosestEdge (double xpt, double ypt, void *vpt)
{
    int               i, j, istat, iprev, inext, inow;
    double            x1, y1, x2, y2;
    double            mindist, dist;
    _BOundaryPoint    *bprev = NULL, *bnext = NULL,
                      *bpmin = NULL, *bpnow = NULL;

/*
 * If the point is essentially coincident with a point
 * currently on the polygon, do not add it.
 */
    for (i=0; i<Ncomp; i++) {
        j = StartCompList[i];
        bprev = BPList + j;
        for (;;) {
            if (bprev->inext < 0) {
                break;
            }
            x1 = bprev->x;
            y1 = bprev->y;
            istat = SamePoint (xpt, ypt, x1, y1);
            if (istat == 1) {
                return 0;
            }
            bprev = BPList + bprev->inext;
        }
    }

/*
 * Search all of the polygon components for the segment
 * closest perpendicularly to the specified point.
 */
    bpmin = NULL;
    mindist = 1.e30;
    for (i=0; i<Ncomp; i++) {
        j = StartCompList[i];
        bprev = BPList + j;
        for (;;) {
            if (bprev->inext < 0) {
                break;
            }
            x1 = bprev->x;
            y1 = bprev->y;
            bnext = BPList + bprev->inext;
            x2 = bnext->x;
            y2 = bnext->y;
            istat = gpf_perpdistance2 (x1, y1, x2, y2,
                                       xpt, ypt, &dist);
            if (istat == 1  &&  dist < mindist) {
                bpmin = bprev;
                mindist = dist;
            }
            bprev = bnext;
        }
    }

/*
 * If no segment was found, find the closest point.
 */
    if (bpmin == NULL) {
        return 0;
    }

/*
 * If the inext point for the segment start point is
 * invalid, that is a program bug, so assert.
 */
    assert (bpmin->inext >= 0);

/*
 * Split the segment by introducing the new point in the
 * link list between the segments previous end points.
 */
    iprev = bpmin - BPList;
    inext = bpmin->inext;
    inow = AppendToBPList (xpt, ypt, vpt, iprev);
    bpnow = BPList + inow;
    bpnow->inext = inext;
    bnext = BPList + inext;
    bnext->iprev = inow;

    return 1;

}  /* end of SplitClosestEdge function */   




/*
 *****************************************************************************

                    U n i o n N e w C o m p o n e n t s

 *****************************************************************************

*/

int CSWPolyEdit::UnionNewComponents (void)
{
    _BOundaryPoint    *bprev = NULL;
    double            *xp = NULL, *yp = NULL;
    void              **vp = NULL;
    int               *ipts = NULL;
    int               i, j, n, nc, n2,
                      istat, ifirst;
    std::unique_ptr <CSWPolyGraph>
        ply_graph_obj {new CSWPolyGraph ()};

    auto fscope = [&]()
    {
        csw_Free (xp);
        csw_Free (vp);
        csw_Free (ipts);
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
 * Allocate space for input to polygon union operation.
 */
    n = NumBPList;
    xp = (double *)csw_Malloc (n * 2 * sizeof(double));
    if (xp == NULL) {
        return -1;
    }
    yp = xp + n;

    vp = (void **)csw_Calloc (n * sizeof(void *));
    if (vp == NULL) {
        return -1;
    }

    ipts = (int *)csw_Malloc (Ncomp * sizeof(int));
    if (ipts == NULL) {
        return -1;
    }

/*
 * Convert the link list of boundary points to packed
 * polygon arrays.
 */
    n = 0;
    nc = 0;
    for (i=0; i<Npoly; i++) {
        for (j=0; j<Icomp[i]; j++) {
            n2 = 0;
            ifirst = StartCompList[nc];
            bprev = BPList + ifirst;
            for (;;) {
                xp[n] = bprev->x;
                yp[n] = bprev->y;
                vp[n] = bprev->vp;
                n2++;
                n++;
                if (bprev->inext < 0) {
                    break;
                }
                bprev = BPList + bprev->inext;
            }
            if (n2 > 0) {
                ipts[nc] = n2;
                nc++;
            }
        }
    }
        
/*
 * Union the polygons.
 */
    istat = ply_graph_obj->ply_union_components (xp, yp, vp,
                                  Npoly, Icomp, ipts,
                                  Xout, Yout, Vpout,
                                  &Nout, Icout, Ipout,
                                  Npmax, Ncmax);
    if (istat == -1) {
        return -1;
    }

    return 1;

}  /* end of UnionNewComponents function */



/*
 *******************************************************************

                          S a m e P o i n t

 *******************************************************************

*/

int CSWPolyEdit::SamePoint (double x1, double y1, double x2, double y2)
{
    double            dx, dy, dist, tiny;

    tiny = SamePointDistance * SamePointDistance;

    dx = x2 - x1;
    dy = y2 - y1;
    dist = dx * dx + dy * dy;

    if (dist <= tiny) {
        return 1;
    }

    return 0;

}  /* end of SamePoint function */





/*
 **************************************************************************

         p l y _ a d d _ t i e _ l i n e _ t o _ b o u n d a r y

 **************************************************************************

  Add the specified points by splitting the polygon edge closest in 
  perpendicular distance to each point.  

*/

int CSWPolyEdit::ply_add_tie_line_to_boundary (double *xp, double *yp, void **vp,
                                  int *icomp, int *ipts, int npoly,
                                  double x1, double y1, void *vp1,
                                  double x2, double y2, void *vp2,
                                  double *xout, double *yout, void **vpout,
                                  int *icout, int *ipout, int *nout,
                                  int npmax, int ncmax)
{
    int             i, j, k, n, nc, istat, last;
    int             inside1, inside2, intmp, ntot, nctot;
    double          xmin, ymin, xmax, ymax;
    _BOundaryPoint  *bpmin = NULL, *bnext = NULL, *bpnow = NULL;
    int             iprev, inext, inow;
    CSWPolyUtils    ply_utils_obj;

    FreeMem ();    

    auto fscope = [&]()
    {
        FreeMem ();
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
 * Determine whether each end point is inside
 * or outside of the input polygon.
 */
    inside1 = 0;
    inside2 = 0;

    n = 0;
    nc = 0;
    for (i=0; i<npoly; i++) {
        ply_utils_obj.ply_points (xp+n, yp+n, ipts+nc, icomp[i],
                    &x1, &y1, 1,
                    &intmp);
        if (intmp == 1) {
            inside1 = 1;
            break;
        }
        for (j=0; j<icomp[i]; j++) {
            n += ipts[nc];
            nc++;
        }
    }
    ntot = n;
    nctot = nc;

    n = 0;
    nc = 0;
    for (i=0; i<npoly; i++) {
        ply_utils_obj.ply_points (xp+n, yp+n, ipts+nc, icomp[i],
                    &x2, &y2, 1,
                    &intmp);
        if (intmp == 1) {
            inside2 = 1;
            break;
        }
    }

/*
 * If both end points are inside the current polygon,
 * the input polygon is copied to the output.
 */
    if (inside1 == 1  &&  inside2 == 1) {
        memcpy (xout, xp, ntot * sizeof(double));
        memcpy (yout, yp, ntot * sizeof(double));
        memcpy (vpout, vp, ntot * sizeof(void *));
        memcpy (icout, icomp, npoly * sizeof(int));
        memcpy (ipout, ipts, nctot * sizeof(int));
        *nout = npoly;
        return 1;
    }

/*
 * Allocate lists for start of components.
 */
    if (npoly < MAX_STATIC_POLYS) {
        StartPolyList = StaticStartPolyList;
    }
    else {
        StartPolyList = (int *)csw_Malloc (npoly * sizeof(int));
    }

    if (StartPolyList == NULL) {
        return -1;
    }

    nc = 0;
    for (i=0; i<npoly; i++) {
        for (j=0; j<icomp[i]; j++) {
            nc++;
        }
    }

    if (nc < MAX_STATIC_POLYS) {
        StartCompList = StaticStartCompList;
    }
    else {
        StartCompList = (int *)csw_Malloc (nc * sizeof(int));
    }

    if (StartCompList == NULL) {
        return -1;
    }

    Npoly = npoly;
    Ncomp = nc;
    Icomp = icomp;

    Xout = xout;
    Yout = yout;
    Vpout = vpout;
    Icout = icout;
    Ipout = ipout;
    Nout = 0;
    Npmax = npmax;
    Ncmax = ncmax;

/*
 *  Fill in the boundary point link list.
 */
    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;

    n = 0;
    nc = 0;
    for (i=0; i<npoly; i++) {
        for (j=0; j<icomp[i]; j++) {
            last = -1;
            StartCompList[nc] = NumBPList;
            for (k=0; k<ipts[nc]; k++) {
                if (xp[n] < xmin) xmin = xp[n];
                if (yp[n] < ymin) ymin = yp[n];
                if (xp[n] > xmax) xmax = xp[n];
                if (yp[n] > ymax) ymax = yp[n];
                istat = AppendToBPList (xp[n], yp[n], vp[n], last);
                if (istat == -1) {
                    return -1;
                }
                last = n;
                n++;
            }
            nc++;
        }
    }

    SamePointDistance = (xmax - xmin + ymax - ymin) / 200000.0;

/*
 * Find the edges that intersect the line.  The flag
 * member is set to 1 for intersecting polygon edges.
 */
    FindEdgeIntersections (x1, y1, x2, y2);

/*
 * If the first point (x1, y1) is outside the polygon,
 * split the edge that has the closest intersection
 * with the first point.
 */
    if (inside1 == 0) {
        bpmin = FindClosestEdgeIntersection (x1, y1);
        if (bpmin) {

        /*
         * Split the segment by introducing the new point in the
         * link list between the segments previous end points.
         */
            iprev = bpmin - BPList;
            inext = bpmin->inext;
            inow = AppendToBPList (x1, y1, vp1, iprev);
            bpnow = BPList + inow;
            bpnow->inext = inext;
            bnext = BPList + inext;
            bnext->iprev = inow;
        }
    }

/*
 * If the second point (x2, y2) is outside the polygon,
 * split the edge that has the closest intersection
 * with the second point.
 */
    if (inside2 == 0) {
        bpmin = FindClosestEdgeIntersection (x2, y2);
        if (bpmin) {

        /*
         * Split the segment by introducing the new point in the
         * link list between the segments previous end points.
         */
            iprev = bpmin - BPList;
            inext = bpmin->inext;
            inow = AppendToBPList (x2, y2, vp2, iprev);
            bpnow = BPList + inow;
            bpnow->inext = inext;
            bnext = BPList + inext;
            bnext->iprev = inow;
        }
    }

/*
 * The new points may have changed the topology of the polygon,
 * so union all the components together to output a new polygon.
 */
    istat = UnionNewComponents ();
    if (istat == -1) {
        return -1;
    }

    *nout = Nout;

    return 1;

}  /* end of function ply_add_tie_line_to_boundary */




/*
 **************************************************************************

                  F i n d E d g e I n t e r s e c t i o n s

 **************************************************************************

*/

void CSWPolyEdit::FindEdgeIntersections (double x1, double y1,
                                  double x2, double y2)
{
    int             i, j, istat;
    double          xt1, yt1, xt2, yt2, xint, yint;
    _BOundaryPoint  *bprev = NULL, *bnext = NULL;
    CSWPolyUtils    ply_utils_obj;
    
    for (i=0; i<Ncomp; i++) {
        j = StartCompList[i];
        bprev = BPList + j;
        for (;;) {
            if (bprev->inext < 0) {
                break;
            }
            xt1 = bprev->x;
            yt1 = bprev->y;
            bnext = BPList + bprev->inext;
            xt2 = bnext->x;
            yt2 = bnext->y;
            istat = ply_utils_obj.ply_segint (x1, y1, x2, y2,
                                xt1, yt1, xt2, yt2,
                                &xint, &yint);
            if (istat == 0) {
                istat = SamePoint (xint, yint, xt2, yt2);
                if (istat == 0) {
                    bprev->flag = 1;
                    bprev->xint = xint;
                    bprev->yint = yint;
                }
                else {
                    bnext->flag = 1;
                    bnext->xint = xint;
                    bnext->yint = yint;
                }
            }
            bprev = bnext;
        }
    }

    return;

}



/*
 ******************************************************************************

            F i n d C l o s e s t E d g e I n t e r s e c t i o n

 ******************************************************************************

*/

_BOundaryPoint *CSWPolyEdit::FindClosestEdgeIntersection (double x, double y)
{
    double            dx, dy, dist, dmin;
    _BOundaryPoint    *bmin = NULL;
    int               i, j;
    double            xint, yint;
    _BOundaryPoint    *bprev = NULL, *bnext = NULL;
    
    bmin = NULL;
    dmin = 1.e30;

    for (i=0; i<Ncomp; i++) {
        j = StartCompList[i];
        bprev = BPList + j;
        for (;;) {
            if (bprev->inext < 0) {
                break;
            }
            if (bprev->flag == 1) {
                bnext = BPList + bprev->inext;
                xint = bprev->xint;
                yint = bprev->yint;
                dx = x - xint;
                dy = y - yint;
                dist = dx * dx + dy * dy;
                if (dist < dmin) {
                    dmin = dist;
                    bmin = bprev;
                }
                bprev = bnext;
            }
            else {
                bnext = BPList + bprev->inext;
                bprev = bnext;
            }
        }
    }

    return bmin;

}
