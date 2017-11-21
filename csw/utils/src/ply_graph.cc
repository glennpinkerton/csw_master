/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_graph.cc

    This file has been refactored to provide the bodies of the
    methods defined in the CSWPolyGraph class.

    This file has functions used to do polygon boolean operations
    by using connected graph representations of the polygon edges.
    Each segment of the two sets of polygons is put into a graph.
    Each intersection between segments is used to split the original
    segments into two other segments of the graph.  Once all segments
    have been split, various combinations of checking midpoint of segments
    for inside/outside/edge of the polygons produces a set of segments
    for the results of the polygon boolean operation.  The final
    polygons are assembled from the output set of segments.
*/

#include <assert.h>
#include <math.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/ply_protoP.h"
#include "csw/utils/private_include/gpf_utils.h"

#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_graph.h"

#include "csw/utils/private_include/csw_scope.h"


int CSWPolyGraph::_test_inside_outside (double *xp, double *yp, int np, int *nv,
                          double x, double y)
{
    int             ix, iy, istat;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);


    SetupRawVectors (xp, yp, NULL, 1, &np, nv,
                     xp, yp, NULL, 1, &np, nv);
    SetupEdgeGrids ();

    SnapNodes ();

    ix = (int)((x - TestXmin) / TestScale + .5) - IntegerRange;
    iy = (int)((y - TestYmin) / TestScale + .5) - IntegerRange;

    istat = InsideOutside (ix, iy, 1);

    FreeAllMem ();

    return istat;

}



int CSWPolyGraph::_test_hole_nesting (int **xa, int**ya, int*na, int ncomp,
                        double *xout, double *yout,
                        int *npout, int *ncout, int *nvout)
{
    int             i, k, xmin, ymin, xmax, ymax;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    CompList = (COmponentStruct *)csw_Malloc (ncomp * sizeof(COmponentStruct));
    if (!CompList) {
        return -1;
    }

    TestScale = 3.0 / 1000000.0;
    TestScale *= INTEGER_MULTIPLIER;
    TestXmin = 0.0;
    TestYmin = 0.0;
    IntegerRange = 0;

    Pxout = xout;
    Pyout = yout;
    Ptagout = NULL;
    Pnpout = npout;
    Pncout = ncout;
    Pnvout = nvout;
    Pmaxcomp = 2000;
    Pmaxnpts = 20000;

    Ncomp = ncomp;

    for (i=0; i<ncomp; i++) {

        xmin = xa[i][0];
        ymin = ya[i][0];
        xmax = xmin;
        ymax = ymin;
        for (k=1; k<na[i]; k++) {
            if (xa[i][k] < xmin) xmin = xa[i][k];
            if (ya[i][k] < ymin) ymin = ya[i][k];
            if (xa[i][k] > xmax) xmax = xa[i][k];
            if (ya[i][k] > ymax) ymax = ya[i][k];
        }

        CompList[i].x = xa[i];
        CompList[i].y = ya[i];
        CompList[i].npts = na[i];
        CompList[i].holeflag = 0;
        CompList[i].xmin = xmin;
        CompList[i].ymin = ymin;
        CompList[i].xmax = xmax;
        CompList[i].ymax = ymax;

    }

    NestHoles ();

    if (xout == NULL) {
        return 1;
    }

    BuildOutputPolygons ();

    return 1;

}



void CSWPolyGraph::plr (char *grid, int i1, int i2, int ncol) {

    int     i, j1, j2;

    for (i=i1; i<=i2; i++) {
        j1 = i * ncol;
        j2 = j1 + ncol - 1;
        printf (" left = %d   right = %d\n",
                (int)grid[j1], (int)grid[j2]);
    }

}

void CSWPolyGraph::prow (char *grid, int j1, int j2, int i1, int ncol) {

    int i, j, k;

    i = i1 * ncol;

    for (j=j1; j<=j2; j++) {
        k = i + j;
        printf  ("j = %d  val = %d\n", j, grid[k]);
    }
}





/*
  ****************************************************************************

                          p l y _ b o o l e a n

  ****************************************************************************

  Do the specified boolean operation on the specified input polygon sets.

*/

int CSWPolyGraph::ply_boolean (double *xp1, double *yp1, void **tag1,
                 int np1, int *nc1, int *nv1,
                 double *xp2, double *yp2, void **tag2,
                 int np2, int *nc2, int *nv2,
                 int operation,
                 double *xout, double *yout, void **tagout,
                 int *npout, int *ncout, int *nvout,
                 int maxpts, int maxcomp)
{
    int             istat, i;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (np1 < 1  ||  np2 < 1) {
        return -1;
    }

    FragmentFlag = 0;
    if (operation == PLY_FRAGMENT) {
        FragmentFlag = 1;
    }

    istat = IdenticalInput (xp1, yp1, np1, nc1, nv1,
                            xp2, yp2, np2, nc2, nv2);
    if (istat == 1111) {
        if (operation == PLY_XOR) {
            *npout = 0;
            return 1;
        }
        else {
            CopyPolygons (xp1, yp1, np1, nc1, nv1,
                          xout, yout, npout, ncout, nvout);
            return 1;
        }
    }

    Xp1 = xp1;
    Yp1 = yp1;
    Xp2 = xp2;
    Yp2 = yp2;
    Tag1 = tag1;
    Tag2 = tag2;
    Np1 = np1;
    Nc1 = nc1;
    Nv1 = nv1;
    Np2 = np2;
    Nc2 = nc2;
    Nv2 = nv2;

    InputCompIDFlag1 = (int *)csw_Calloc (2 * maxcomp * sizeof(int));
    if (InputCompIDFlag1 == NULL) {
        return -1;
    }
    InputCompIDFlag2 = InputCompIDFlag1 + maxcomp;
    NumInputCompIDFlag1 = maxcomp;
    NumInputCompIDFlag2 = maxcomp;

    Pxout = xout;
    Pyout = yout;
    Ptagout = tagout;

    Pnpout = npout;
    Pncout = ncout;
    Pnvout = nvout;

    Pmaxnpts = maxpts;
    Pmaxcomp = maxcomp;

    IntegerRange = INTEGER_RANGE * INTEGER_MULTIPLIER;

    Xwork = NULL;
    CompList = NULL;

    Xwork = (int *)csw_Malloc (maxpts * 2 * sizeof(int));
    if (!Xwork) {
        FreeAllMem ();
        return -1;
    }
    Ywork = Xwork + maxpts;

    Tagwork = (void **)csw_Calloc (maxpts * sizeof(void*));
    if (!Tagwork) {
        FreeAllMem ();
        return -1;
    }

    CompList = (COmponentStruct *)csw_Calloc (maxcomp * sizeof(COmponentStruct));
    if (!CompList) {
        FreeAllMem ();
        return -1;
    }

    Nwork = 0;
    Ncomp = 0;

    istat = SetupRawVectors (xp1, yp1, tag1, np1, nc1, nv1,
                             xp2, yp2, tag2, np2, nc2, nv2);
    if (istat == -1) {
        FreeAllMem ();
        return -1;
    }

    istat = SetupEdgeGrids ();
    if (istat == -1) {
        FreeAllMem ();
        return -1;
    }

    istat = BuildInitialLists ();
    if (istat == -1) {
        FreeAllMem ();
        return -1;
    }

    SnapNodes ();

    DiscardFlag = -1;
    istat = CalculateSegmentIntersections ();
    if (istat == -1) {
        FreeAllMem ();
        return -1;
    }

    DiscardFlag = 1;

    istat = RemoveTemporaryNodes ();
    if (istat == -1) {
        FreeAllMem ();
        return -1;
    }

    istat = ResetRawVectors ();
    if (istat == -1) {
        FreeAllMem ();
        return -1;
    }

    istat = SetupEdgeGrids ();
    if (istat == -1) {
        FreeAllMem ();
        return -1;
    }

    istat = RemoveDuplicateSegments ();
    if (istat == -1) {
        FreeAllMem ();
        return -1;
    }

    RemoveOverlaps ();

    if (operation == PLY_INTERSECT) {
        IntersectGraphs ();
    }
    else if (operation == PLY_UNION) {
        UnionGraphs ();
    }
    else if (operation == PLY_XOR) {
        XorGraphs ();
    }
    else if (operation == PLY_FRAGMENT) {
        FragmentGraphs ();
    }

    if (operation == PLY_FRAGMENT) {
        for (i=0; i<Ncomp; i++) {
            CompList[i].holeflag = -1;
        }
    }
    else {
        istat = NestHoles ();
        if (istat == -1) {
            FreeAllMem ();
            return -1;
        }
    }

    istat = BuildOutputPolygons ();
    if (istat == -1) {
        FreeAllMem ();
        return -1;
    }

    FreeAllMem ();

    return 1;

}  /*  end of ply_boolean function  */





/*
  ****************************************************************************

                      S e t u p R a w V e c t o r s

  ****************************************************************************

    Given the two sets of input polygons, create arrays of endpoints of all
  the edges.  The endpoint arrays are scaled to integers.  All integers in
  the arrays are evenly divisible by INTEGER_MULTIPLIER.  This is critical
  for several grazing avoidance solutions throughout the algorithm.

*/

int CSWPolyGraph::SetupRawVectors (double *xp1, double *yp1, void **tag1,
                            int np1, int *nc1, int *nv1,
                            double *xp2, double *yp2, void **tag2,
			    int np2, int *nc2, int *nv2)
{
    int              i, j, k, n, n2, n3, nbase, ii, *iptr;
    int              ix1, iy1, ix2, iy2, ix0, iy0, nbb;
    void             *it1, *it2, *it0;
    int              bbx1, bby1, bbx2, bby2;
    double           scale, tiny, dx1, dy1, dx2, dy2;
    double           p1x1, p1y1, p1x2, p1y2;
    double           p2x1, p2y1, p2x2, p2y2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    Free any previous bounding box list in the Raw structures.
*/
    if (first == 0) {
        FreeBBLists ();
    }

/*
    Set the Raw1 and Raw2 structures to zeros initially.
*/
    memset ((char *)&Raw1, 0, sizeof(RAwEdgeStruct));
    memset ((char *)&Raw2, 0, sizeof(RAwEdgeStruct));
    first = 0;

/*
    Find the x,y limits of the first set of polygons.
*/
    p1x1 = 1.e30;
    p1y1 = 1.e30;
    p1x2 = -1.e30;
    p1y2 = -1.e30;
    n = 0;
    n2 = 0;
    for (i=0; i<np1; i++) {
        for (j=0; j<nc1[i]; j++) {
            for (k=0; k<nv1[n2]; k++) {
                if (xp1[n] < p1x1) p1x1 = xp1[n];
                if (xp1[n] > p1x2) p1x2 = xp1[n];
                if (yp1[n] < p1y1) p1y1 = yp1[n];
                if (yp1[n] > p1y2) p1y2 = yp1[n];
                n++;
            }
            n2++;
        }
    }
    tiny = (p1x2 - p1x1 + p1y2 - p1y1) / 100.0;
    p1x1 -= tiny;
    p1y1 -= tiny;
    p1x2 += tiny;
    p1y2 += tiny;

    XYTiny = tiny / 100.0;

/*
    Find the x,y limits of the second set of polygons.
*/
    p2x1 = 1.e30;
    p2y1 = 1.e30;
    p2x2 = -1.e30;
    p2y2 = -1.e30;
    n = 0;
    n2 = 0;
    for (i=0; i<np2; i++) {
        for (j=0; j<nc2[i]; j++) {
            for (k=0; k<nv2[n2]; k++) {
                if (xp2[n] < p2x1) p2x1 = xp2[n];
                if (xp2[n] > p2x2) p2x2 = xp2[n];
                if (yp2[n] < p2y1) p2y1 = yp2[n];
                if (yp2[n] > p2y2) p2y2 = yp2[n];
                n++;
            }
            n2++;
        }
    }
    tiny = (p2x2 - p2x1 + p2y2 - p2y1) / 100.0;
    p2x1 -= tiny;
    p2y1 -= tiny;
    p2x2 += tiny;
    p2y2 += tiny;

/*
    Find the cumulative limits and calculate the scale for
    conversion to integer coordinates.
*/
    dx1 = p1x1;
    if (p2x1 < dx1) dx1 = p2x1;
    dy1 = p1y1;
    if (p2y1 < dy1) dy1 = p2y1;
    dx2 = p1x2;
    if (p2x2 > dx2) dx2 = p2x2;
    dy2 = p1y2;
    if (p2y2 > dy2) dy2 = p2y2;

    scale = (dx2 - dx1) / (double)IntegerRange2;
    if (dy2 - dy1 > dx2 - dx1) {
        scale = (dy2 - dy1) / (double)IntegerRange2;
    }

    TestScale = scale;
    TestXmin = dx1;
    TestYmin = dy1;

    Raw1.xmin = (int)((p1x1 - dx1) / scale + .5) - IntegerRange;
    Raw1.ymin = (int)((p1y1 - dy1) / scale + .5) - IntegerRange;
    Raw1.xmax = (int)((p1x2 - dx1) / scale + .5) - IntegerRange;
    Raw1.ymax = (int)((p1y2 - dy1) / scale + .5) - IntegerRange;

    Raw2.xmin = (int)((p2x1 - dx1) / scale + .5) - IntegerRange;
    Raw2.ymin = (int)((p2y1 - dy1) / scale + .5) - IntegerRange;
    Raw2.xmax = (int)((p2x2 - dx1) / scale + .5) - IntegerRange;
    Raw2.ymax = (int)((p2y2 - dy1) / scale + .5) - IntegerRange;

/*
    Allocate memory for the first set of polygon edges.  The number
    of edges should be at most the number of polygon vertex points,
    but I add a couple just to make sure.
*/
    n = 0;
    n2 = 0;
    for (i=0; i<np1; i++) {
        for (j=0; j<nc1[i]; j++) {
            n += nv1[n2];
            n2++;
        }
    }
    n += 2;

    iptr = (int *)csw_Malloc (6 * n * sizeof(int));
    if (!iptr) {
        return -1;
    }

    Raw1.x1 = iptr;
    Raw1.y1 = iptr + n;
    Raw1.x2 = iptr + n * 2;
    Raw1.y2 = iptr + n * 3;
    Raw1.bbid = iptr + n * 4;
    Raw1.compid = iptr + n * 5;

    Raw1.bblist = (BOxStruct *)csw_Calloc (np1 * sizeof(BOxStruct));
    if (!Raw1.bblist) {
        csw_Free (iptr);
        return -1;
    }
    Raw1.nbblist = np1;

    Raw1.tag1 = (void **)csw_Calloc (2 * n * sizeof(void*));
    if (!Raw1.tag1) {
        csw_Free (iptr);
        csw_Free (Raw1.bblist);
        return -1;
    }
    Raw1.tag2 = Raw1.tag1 + n;

/*
    These 4 lines are needed to suppress warnings.
*/
    bbx1 = 2000000000;
    bbx2 = -2000000000;
    bby1 = 2000000000;
    bby2 = -2000000000;

/*
    Set up the raw edge arrays for the first polygon set.
*/
    n = 0;
    n2 = 0;
    n3 = 0;
    nbb = 0;
    for (ii=0; ii<np1; ii++) {
        Raw1.bblist[nbb].start = n2;
        for (i=0; i<nc1[ii]; i++) {
            ix1 = (int)((xp1[n] - dx1) / scale + .5) - IntegerRange;
            iy1 = (int)((yp1[n] - dy1) / scale + .5) - IntegerRange;
            if (tag1 == NULL) {
                it1 = NULL;
            }
            else {
                it1 = tag1[n];
            }
            SnapPoint (&ix1, &iy1);
            ix1 += RAW_OFFSET;
            ix0 = ix1;
            iy0 = iy1;
            it0 = it1;
            if (i == 0) {
                bbx1 = ix0;
                bbx2 = ix0;
                bby1 = iy0;
                bby2 = iy0;
            }
            else {
                if (ix0 < bbx1) bbx1 = ix0;
                if (iy0 < bby1) bby1 = iy0;
                if (ix0 > bbx2) bbx2 = ix0;
                if (iy0 > bby2) bby2 = iy0;
            }
            n++;
            nbase = n2;
            for (j=1; j<nv1[n3]; j++) {
                ix2 = (int)((xp1[n] - dx1) / scale + .5) - IntegerRange;
                iy2 = (int)((yp1[n] - dy1) / scale + .5) - IntegerRange;
                if (tag1 == NULL) {
                    it2 = NULL;
                }
                else {
                    it2 = tag1[n];
                }
                n++;
                if (TooClose (ix1, iy1, ix2, iy2)) {
                    continue;
                }
                SnapPoint (&ix2, &iy2);
                ix2 += RAW_OFFSET;
                Raw1.x1[n2] = ix1;
                Raw1.y1[n2] = iy1;
                Raw1.tag1[n2] = it1;
                Raw1.x2[n2] = ix2;
                Raw1.y2[n2] = iy2;
                Raw1.tag2[n2] = it2;
                Raw1.bbid[n2] = nbb;
                Raw1.compid[n2] = n3;
                n2++;
                ix1 = ix2;
                iy1 = iy2;
                it1 = it2;
                if (ix1 < bbx1) bbx1 = ix1;
                if (iy1 < bby1) bby1 = iy1;
                if (ix1 > bbx2) bbx2 = ix1;
                if (iy1 > bby2) bby2 = iy1;
            }
            n3++;
        /*
            Close the component if it is not already closed
        */

        /*
         * If the operation is fragmentation, then the input
         * components do not have to be closed.
         */
            if (FragmentFlag == 0) {
                if (!(ix1 == ix0  &&  iy1 == iy0)) {
                    if (TooClose (ix0, iy0, ix1, iy1)) {
                        n2--;
                        if (n2 >= 0) {
                            Raw1.x2[n2] = ix0;
                            Raw1.y2[n2] = iy0;
                            n2++;
                        }
                    }
                    else {
                        Raw1.x1[n2] = ix1;
                        Raw1.y1[n2] = iy1;
                        Raw1.tag1[n2] = it1;
                        Raw1.x2[n2] = ix0;
                        Raw1.y2[n2] = iy0;
                        Raw1.tag2[n2] = it0;
                        Raw1.bbid[n2] = nbb;
                        Raw1.compid[n2] = n3;
                        n2++;
                    }
                }
                if (n2 - nbase < 3) n2 = nbase;
            }
        }
        Raw1.bblist[nbb].size = n2 - Raw1.bblist[nbb].start;
        Raw1.bblist[nbb].x1 = bbx1 - BBMARGIN;
        Raw1.bblist[nbb].y1 = bby1 - BBMARGIN;
        Raw1.bblist[nbb].x2 = bbx2 + BBMARGIN;
        Raw1.bblist[nbb].y2 = bby2 + BBMARGIN;
        nbb++;
    }

    Raw1.nvec = n2;

/*
    Allocate memory for the second set of polygon edges.  The number
    of edges should be at most the number of polygon vertex points,
    but I add a couple just to make sure.
*/
    n = 0;
    n2 = 0;
    for (i=0; i<np2; i++) {
        for (j=0; j<nc2[i]; j++) {
            n += nv2[n2];
            n2++;
        }
    }
    n += 2;

    iptr = (int *)csw_Malloc (6 * n * sizeof(int));
    if (!iptr) {
        csw_Free (Raw1.x1);
        csw_Free (Raw1.bblist);
        csw_Free (Raw1.tag1);
        memset ((char *)&Raw1, 0, sizeof(RAwEdgeStruct));
        return -1;
    }

    Raw2.x1 = iptr;
    Raw2.y1 = iptr + n;
    Raw2.x2 = iptr + n * 2;
    Raw2.y2 = iptr + n * 3;
    Raw2.bbid = iptr + n * 4;
    Raw2.compid = iptr + n * 5;

    Raw2.bblist = (BOxStruct *)csw_Calloc (np2 * sizeof(BOxStruct));
    if (!Raw2.bblist) {
        csw_Free (iptr);
        csw_Free (Raw1.x1);
        csw_Free (Raw1.bblist);
        csw_Free (Raw1.tag1);
        return -1;
    }
    Raw2.nbblist = np2;

    Raw2.tag1 = (void **)csw_Calloc (2 * n * sizeof(void*));
    if (!Raw2.tag1) {
        csw_Free (iptr);
        csw_Free (Raw2.bblist);
        csw_Free (Raw1.x1);
        csw_Free (Raw1.bblist);
        csw_Free (Raw1.tag1);
        return -1;
    }
    Raw2.tag2 = Raw2.tag1 + n;

/*
    Set up the raw edge arrays for the second polygon set.
*/
    n = 0;
    n2 = 0;
    n3 = 0;
    nbb = 0;
    for (ii=0; ii<np2; ii++) {
        Raw2.bblist[nbb].start = n2;
        for (i=0; i<nc2[ii]; i++) {
            ix1 = (int)((xp2[n] - dx1) / scale + .5) - IntegerRange;
            iy1 = (int)((yp2[n] - dy1) / scale + .5) - IntegerRange;
            if (tag2 == NULL) {
                it1 = NULL;
            }
            else {
                it1 = tag2[n];
            }
            SnapPoint (&ix1, &iy1);
            ix1 += RAW_OFFSET;
            ix0 = ix1;
            iy0 = iy1;
            it0 = it1;
            if (i == 0) {
                bbx1 = ix0;
                bbx2 = ix0;
                bby1 = iy0;
                bby2 = iy0;
            }
            else {
                if (ix0 < bbx1) bbx1 = ix0;
                if (iy0 < bby1) bby1 = iy0;
                if (ix0 > bbx2) bbx2 = ix0;
                if (iy0 > bby2) bby2 = iy0;
            }
            n++;
            nbase = n2;
            for (j=1; j<nv2[n3]; j++) {
                ix2 = (int)((xp2[n] - dx1) / scale + .5) - IntegerRange;
                iy2 = (int)((yp2[n] - dy1) / scale + .5) - IntegerRange;
                if (tag2 == NULL) {
                    it2 = NULL;
                }
                else {
                    it2 = tag2[n];
                }
                n++;
                if (TooClose (ix1, iy1, ix2, iy2)) {
                    continue;
                }
                SnapPoint (&ix2, &iy2);
                ix2 += RAW_OFFSET;
                Raw2.x1[n2] = ix1;
                Raw2.y1[n2] = iy1;
                Raw2.tag1[n2] = it1;
                Raw2.x2[n2] = ix2;
                Raw2.y2[n2] = iy2;
                Raw2.tag2[n2] = it2;
                Raw2.bbid[n2] = nbb;
                Raw2.compid[n2] = n3;
                n2++;
                ix1 = ix2;
                iy1 = iy2;
                it1 = it2;
                if (ix1 < bbx1) bbx1 = ix1;
                if (iy1 < bby1) bby1 = iy1;
                if (ix1 > bbx2) bbx2 = ix1;
                if (iy1 > bby2) bby2 = iy1;
            }
            n3++;
        /*
            Close the component if it is not already closed
        */

        /*
         * If the operation is fragmentation, then the input
         * components do not have to be closed.
         */
            if (FragmentFlag == 0) {
                if (!(ix1 == ix0  &&  iy1 == iy0)) {
                    if (TooClose (ix0, iy0, ix1, iy1)) {
                        n2--;
                        if (n2 >= 0) {
                            Raw2.x2[n2] = ix0;
                            Raw2.y2[n2] = iy0;
                            n2++;
                        }
                    }
                    else {
                        Raw2.x1[n2] = ix1;
                        Raw2.y1[n2] = iy1;
                        Raw2.tag1[n2] = it1;
                        Raw2.x2[n2] = ix0;
                        Raw2.y2[n2] = iy0;
                        Raw2.tag2[n2] = it0;
                        Raw2.bbid[n2] = nbb;
                        Raw2.compid[n2] = n3;
                        n2++;
                    }
                }
                if (n2 - nbase < 3) n2 = nbase;
            }
        }
        Raw2.bblist[nbb].size = n2 - Raw2.bblist[nbb].start;
        Raw2.bblist[nbb].x1 = bbx1 - BBMARGIN;
        Raw2.bblist[nbb].y1 = bby1 - BBMARGIN;
        Raw2.bblist[nbb].x2 = bbx2 + BBMARGIN;
        Raw2.bblist[nbb].y2 = bby2 + BBMARGIN;
        nbb++;
    }

    Raw2.nvec = n2;

    return 1;

}  /*  end of private function SetupRawVectors  */




/*

    ***************************************************************

                     S e g i n t I n t e g e r

    ***************************************************************

  function name:    SegintInteger  (integer)

  call sequence:    SegintInteger (x1, y1, x2, y2, x3, y3, x4, y4, &x, &y)

  synopsis:         Calculates the intersection between two vectors, and
                    returns the intersecting point in x and y.

  return values:    Status code

                    0 = normal successful return (x and y are the unique
                        intersection point inside the vectors passed)
                    1 = the lines intersect, but not inside the limits of
                        the vectors.  X and y are still the intersection
                        point.
                    2 = the segments are parallel and touch at one point.
                        this point is returned in x and y.
                    3 = The lines are parallel and overlap.  x and y are
                        not returned in this case since there are an
                        infinite number of solutions.
                    4 = The two vectors are identical.  No intersection
                        point is calculated.
                    5 = The lines are parallel and do not intersect at all.
                    6 = At least one vector has zero length, and no
                        intersection can be found.

  usage:            This is intended as a general utility function to be
                    used in all of the polygon operations functions.  In
                    the case of parallel overlaping lines, the calling
                    function will have to decide what to do.

  algorithm:        Special cases are used if either or both of the vectors
                    are vertical.  After the intersection point has been
                    calculated, it is determined whether the point is
                    inside of both vectors.

  description of calling parameters:

    x1    r    int       x coordinate of 1st endpoint of 1st vector
    y1    r    int       y coordinate of 1st endpoint of 1st vector
    x2    r    int       x coordinate of 2nd endpoint of 1st vector
    y2    r    int       y coordinate of 2nd endpoint of 1st vector
    x3    r    int       x coordinate of 1st endpoint of 2nd vector
    y3    r    int       y coordinate of 1st endpoint of 2nd vector
    x4    r    int       x coordinate of 2nd endpoint of 2nd vector
    y4    r    int       y coordinate of 2nd endpoint of 2nd vector
    x     w    int*      pointer to x coordinate of intersection point
    y     w    int*      pointer to y coordinate of intersection point

*/

int CSWPolyGraph::SegintInteger (int x1, int y1, int x2, int y2,
                          int x3, int y3, int x4, int y4,
                          int *x, int *y)
{
    double       s1, s2, b1, b2, xt, xint, yint;
    int          sameline=0, ixint, iyint;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  check if either vector has zero length  */

    if (x1 == x2  &&  y1 == y2) return 6;
    if (x3 == x4  &&  y3 == y4) return 6;

/*  both segments are identical   */

    if(x1 == x3  &&  y1 == y3  &&  x2 == x4  &&  y2 == y4) return 4;
    if(x2 == x3  &&  y2 == y3  &&  x1 == x4  &&  y1 == y4) return 4;

/*  calculate slopes and intercepts of the vectors */

    if(x1 != x2) {
        s1 = (double)(y2-y1) / (double)(x2-x1);
        b1 = (double)y1 - s1 * (double)x1;
    }
    else {
        s1 = 1.e30f;
        b1 = 1.e30f;
    }
    if(x3 != x4) {
        s2 = (double)(y4-y3) / (double)(x4-x3);
        b2 = (double)y3 - s2 * (double)x3;
    }
    else {
        s2 = 1.e30f;
        b2 = 1.e30f;
    }

/*  special cases 1st vector is vertical or both are vertical */

    xint = 0.0;
    yint = 0.0;
    if(x1 == x2) {

/*      both vectors vertical  */

        if(x3 == x4) {
            if(x3 == x1) {
                sameline = 1;
            }
            else {
                return 5;
            }
        }

/*      only 1st vector is vertical  */

        else {
            yint = (s2 * (double)x1 + b2 + .5);
            xint = (double)x1;
        }
    }

/*  special case, second vector is vertical  */

    else if(x3 == x4) {
        yint = (s1 * (double)x3 + b1 + .5);
        xint = (double)x3;
    }

/*  general case, neither vector is vertical  */

    else {

/*      same slope means that the vectors are parallel or overlapping  */

        if(s1 - s2 == 0.0) {
            if (b1 - b2 == 0.0) {
                sameline = 1;
            }
            else {
                return 5;
            }
        }

/*      the slopes are not equal, so the intersection can be calculated  */

        else {
            xt = (b1-b2) / (s2-s1);
            xint = xt;
            yint = (s1 * xt + b1 + .5);
        }
    }

    if (y1 == y2) {
        yint = y1;
    }
    else if (y3 == y4) {
        yint = y3;
    }

    *x = (int)(xint + .5);
    *y = (int)(yint + .5);

    SnapPoint (x, y);

/*
    If the intersection point is very close to a vector endpoint,
    make it the vector endpoint.
*/
    if (VeryClose (*x, *y, x1, y1)) {
        *x = x1;
        *y = y1;
    }
    else if (VeryClose (*x, *y, x2, y2)) {
        *x = x2;
        *y = y2;
    }
    else if (VeryClose (*x, *y, x3, y3)) {
        *x = x3;
        *y = y3;
    }
    else if (VeryClose (*x, *y, x4, y4)) {
        *x = x4;
        *y = y4;
    }

    ixint = *x;
    iyint = *y;

/*  check if the intersection point is on the vector  */

/*  first for non overlapping lines  */

    if(!sameline) {

        if( ((double)(x1 - ixint) * (double)(ixint - x2)) >= CSW_DZERO  &&
            ((double)(y1 - iyint) * (double)(iyint - y2)) >= CSW_DZERO  &&
            ((double)(x3 - ixint) * (double)(ixint - x4)) >= CSW_DZERO  &&
            ((double)(y3 - iyint) * (double)(iyint - y4)) >= CSW_DZERO ) {
            return 0;
        }
        else {

/*          check if one endpoint is coincident  */

            if (x1 == x3  &&  y1 == y3) {
                *x = x1;
                *y = y1;
                return 0;
            }
            if (x1 == x4  &&  y1 == y4) {
                *x = x1;
                *y = y1;
                return 0;
            }
            if (x2 == x3  &&  y2 == y3) {
                *x = x2;
                *y = y2;
                return 0;
            }
            if (x2 == x4  &&  y2 == y4) {
                *x = x2;
                *y = y2;
                return 0;
            }

/*          segments do not intersect  */

            return 1;
        }
    }

/*  check if segments of overlapping line overlap or touch at one point */

    else {
        if((((double)(x1-x3) * (double)(x3-x2)) >= CSW_DZERO  ||
            ((double)(x1-x4) * (double)(x4-x2)) >= CSW_DZERO  ||
            ((double)(x3-x1) * (double)(x1-x4)) >= CSW_DZERO  ||
            ((double)(x3-x2) * (double)(x2-x4)) >= CSW_DZERO)
            &&
           (((double)(y1-y3) * (double)(y3-y2)) >= CSW_DZERO  ||
            ((double)(y1-y4) * (double)(y4-y2)) >= CSW_DZERO  ||
            ((double)(y3-y1) * (double)(y1-y4)) >= CSW_DZERO  ||
            ((double)(y3-y2) * (double)(y2-y4)) >= CSW_DZERO)) {

/*          do segments touch at one point  */

            if( (x1==x3 && y1==y3 && (x4-x1)*(x1-x2)>=0 && (y4-y1)*(y1-y2)>=0 ) ||
                (x1==x4 && y1==y4 && (x3-x1)*(x1-x2)>=0 && (y3-y1)*(y1-y2)>=0 )) {
                *x = x1;
                *y = y1;
                return 2;
            }

            if( (x2==x3 && y2==y3 && ((double)(x1-x2)*(double)(x2-x4)) >= CSW_DZERO &&
                                     ((double)(y1-y2)*(double)(y2-y4)) >= CSW_DZERO ) ||
                (x2==x4 && y2==y4 && ((double)(x1-x2)*(double)(x2-x3)) >= CSW_DZERO &&
                                     ((double)(y1-y2)*(double)(y2-y3)) >= CSW_DZERO )) {
                *x = x2;
                *y = y2;
                return 2;
            }

/*          segments overlap an arbitrary amount  */

            return 3;
        }

/*      sameline segments do not overlap or touch  */

        else {
            return 5;
        }
    }

}   /*  end of private SegintInteger function  */





/*

    ***************************************************************

                     P o i n t I n t e g e r

    ***************************************************************

  function name:  PointInteger  (integer)

  call sequence:  PointInteger (x1a, y1a, x2a, y2a, nvec, p, q,
                                bblist, nbblist)

  synopsis:       Function to return the location of a point
                  relative to a complex set of polygon edges.

  return values:  -1  point is outside polygon
                   0  point is on polygon boundary
                   1  point is inside polygon

  usage:          This function is called from the polygon boolean
                  operations.  The calling function is responsible
                  for preconditioning the point to avoid grazing
                  of any vector endpoint.  As such, do not call
                  this function on your own.

  algorithm:      The algorithm involves calculation of intersections
                  between the polygon sides and a ray extending
                  vertically (+ y) from the target point.  If there
                  are an odd number of intersections, the target point
                  is inside the polygon.  For an even number of
                  intersections, the point is outside of the polygon.
                  If any of the intersection points is coincident with
                  the target point, the target point lies on the
                  boundary of the polygon.

  description of calling parameters:

    x1a          r   int*      array of x for first vector endpoint
    y1a          r   int*      array of y for first vector endpoint
    x2a          r   int*      array of x for second vector endpoint
    y2a          r   int*      array of y for second vector endpoint
    nvec         r   int       number of vectors
    p            r   int       x coordinate of target point
    q            r   int       y coordinate of target point
    bblist       r   BOxStruct*   list of component bounding boxes and
                                  locations in the vector list
    nbblist      r   int       number of items in bblist

*/

int  CSWPolyGraph::PointInteger (int *x1a, int *y1a, int *x2a, int *y2a,
                          int nvec, int p, int q, BOxStruct *bblist,
                          int nbblist)
{
    int        x1, y1, x2, y2, y;
    int        i, kount, itmp;
    double     slope, dy;
    int        i1, i2, ii, graze, fudgeflag;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    kount = 0;
    fudgeflag = 0;
    graze = GrazeDistance;
    if (p % RAW_OFFSET  ==  0) p++;

    if (bblist == NULL) nbblist = 1;

    for (ii=0; ii<nbblist; ii++) {

        if (bblist != NULL) {
            if (p < bblist[ii].x1  ||  p > bblist[ii].x2  ||
                q < bblist[ii].y1  ||  q > bblist[ii].y2) {
                    continue;
            }
            else if (UnionFlag  &&  ii == BBnum) {
                continue;
            }
            else {
                i1 = bblist[ii].start;
                i2 = i1 + bblist[ii].size;
            }
        }
        else {
            i1 = 0;
            i2 = nvec;
        }

    /*
        If there is any vertical segment that has the same x coordinate
        as the target point, move the target point FUDGE units in x.  If the
        target point actually lies on the vertical segment, return 0.
    */
        for (i=i1; i<i2; i++) {
            x1 = x1a[i];
            itmp = x1 - p;
            if (x1 == x2a[i] && itmp >= -GRAZE  &&  itmp <= GRAZE) {
                if (((double)(y1a[i]-q) * (double)(q-y2a[i])) >= CSW_DZERO) {
                    return 0;
                }
                else {
                    fudgeflag = 1;
                }
            }
        }

        if (fudgeflag == 1) {
            p += FUDGE;
        }

    /*
        Check intersections between the target ray and all segments.
    */
        for (i=i1; i<i2; i++) {

            x1 = x1a[i];
            y1 = y1a[i];
            x2 = x2a[i];
            y2 = y2a[i];

        /*
            If the side is non vertical, then if the target point is
            between the x endpoints and the y of the intersection
            with the ray is greater than the target y coordinate,
            increment the intersection count.
        */
            if ((double)(x1-p) * (double)(p-x2) > 0.0) {
                slope = (double)(y2 - y1) / (double)(x2 - x1);
                dy = (double)((double)y1 + slope * (double)(p - x1));
                y = (int)(dy + 0.5);
                itmp = y - q;
                if (itmp >= -graze  &&  itmp <= graze) {
                    return 0;
                }
                if (y > q) {
                    kount++;
                }
            }
        }
    }

/*
    return inside or outside based on an odd
    or even number of intersections
*/
    kount = kount % 2;
    if ( kount == 0) return -1;
    return 1;

}    /* end of private PointInteger function */






/*
  ****************************************************************

                S e t u p E d g e G r i d s

  ****************************************************************

    Set up grids for locations of polygon edges for clip and
  source polygons.  The calling function has already set up
  the segment endpoint arrays in the Raw1 and Raw2 structures
  prior to calling this function.

    The edges for each polygon set are indexed by a
  grid specifying if each cell is all inside, all
  outside, or part inside and outside.  The grid
  geometry for each edge set is determined separately.

*/

int CSWPolyGraph::SetupEdgeGrids (void)
{
    int            flag, n2, n3, n4, n5, isum;
    int            i, j, n, nrow, ncol;
    int            ix1, iy1, ix2, iy2, spac;
    int            *bcell;
    double         x1, y1, x2, y2, ratio, xt, yt;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    initialize and zero the grids
*/
    memset (hgrid, 0, LOCAL_MAX_GRID_SIZE * 2 * sizeof(int));
    memset (pgrid, 0, LOCAL_MAX_GRID_SIZE * 2 * sizeof(int));
    memset (vgrid, 0, LOCAL_MAX_GRID_SIZE * 2 * sizeof(int));

/*
    Setup the grid geometry for the first set of polygons.
*/
    x1 = (double)Raw1.xmin;
    y1 = (double)Raw1.ymin;
    x2 = (double)Raw1.xmax;
    y2 = (double)Raw1.ymax;
    ratio = (x2 - x1) / (y2 - y1);
    if (ratio < .1) ratio = .1;
    if (ratio > 10.) ratio = 10.;

    xt = (double)LOCAL_MAX_GRID_SIZE / ratio;
    nrow = (int)sqrt(xt) - 1;
    ncol = LOCAL_MAX_GRID_SIZE / nrow - 1;

    xt = (x2 - x1) / (ncol - 1);
    yt = (y2 - y1) / (nrow - 1);
    i = (int)(xt + .5);
    if (yt > xt) i = (int)(yt + .5);

    i /= INTEGER_MULTIPLIER;
    i++;
    i *= INTEGER_MULTIPLIER;
    Raw1.gspace = i;

    ix1 = (int)x1;
    ix1 /= Raw1.gspace;
    ix1 -= 2;
    ix1 *= Raw1.gspace;
    Raw1.gxmin = ix1;

    iy1 = (int)y1;
    iy1 /= Raw1.gspace;
    iy1 -= 2;
    iy1 *= Raw1.gspace;
    Raw1.gymin = iy1;

    ix2 = (int)x2;
    ix2 /= Raw1.gspace;
    ix2 += 3;
    ix2 *= Raw1.gspace;
    Raw1.gxmax = ix2;

    iy2 = (int)y2;
    iy2 /= Raw1.gspace;
    iy2 += 3;
    iy2 *= Raw1.gspace;
    Raw1.gymax = iy2;

    Raw1.ncol = (ix2 - ix1) / Raw1.gspace + 1;
    Raw1.nrow = (iy2 - iy1) / Raw1.gspace + 1;

/*
    Set the vector crossings for the first polygon set.
*/
    for (i=0; i<Raw1.nvec; i++) {
        SetHorizontalCrossingsForVector
          (Raw1.x1[i], Raw1.y1[i], Raw1.x2[i], Raw1.y2[i], hgrid,
           Raw1.gxmin, Raw1.gymin, Raw1.gspace, Raw1.ncol, Raw1.nrow);
        SetVerticalCrossingsForVector
          (Raw1.x1[i], Raw1.y1[i], Raw1.x2[i], Raw1.y2[i], vgrid,
           Raw1.gxmin, Raw1.gymin, Raw1.gspace, Raw1.ncol, Raw1.nrow);
    }

/*
    The horizontal crossings are used to guide a scan of the polygon set.
    At the start of each row, the point is always outside.  If a horizontal
    side which is crossed by a polygon edge is encountered, the next node will
    be inside if an odd number of edges crossed the cell side, or it will be
    outside if an even number of vectors crossed.  The pgrid array will be
    1 for inside nodes, -1 for outside and zero for close to an edge.
*/
    nrow = Raw1.nrow;
    ncol = Raw1.ncol;

    for (i=0; i<nrow; i++) {
        flag = -1;
        n = i * ncol;
        for (j=0; j<ncol-1; j++) {
            n2 = n + j;
            if (hgrid[n2] > 0  ||  flag == 0) {
                pgrid[n2] = 0;
                pgrid[n2+1] = 2;
                if (hgrid[n2] % 2 != 0) {
                    flag = -flag;
                }
            }
            else {
                if (pgrid[n2] == 2) {
                    pgrid[n2] = 0;
                }
                else {
                    pgrid[n2] = flag;
                }
            }
        }
        if (pgrid[n+ncol-1] == 2) {
            pgrid[n+ncol-1] = 0;
        }
        else {
            pgrid[n+ncol-1] = -1;
        }
    }

/*
    Any non zero pgrid nodes adjacent to a vertical crossing
    need to be set to zero.
*/
    for (i=0; i<nrow-1; i++) {
        n = i * ncol;
        for (j=0; j<ncol; j++) {
            n2 = n + j;
            if (vgrid[n2] != 0) {
                pgrid[n2] = 0;
                pgrid[n2+ncol] = 0;
            }
        }
    }

/*
    If any endpoint of an edge vector is still next to non zero
    nodes, the component fits entirely inside a cell.  Set all
    4 corners of the cell to zero in this case.
*/
    ix1 = Raw1.gxmin;
    iy1 = Raw1.gymin;
    spac = Raw1.gspace;
    for (n=0; n<Raw1.nvec; n++) {
        j = (Raw1.x1[n] - ix1) / spac;
        i = (Raw1.y1[n] - iy1) / spac;
        n2 = i * ncol + j;
        if (pgrid[n2] != 0) {
            pgrid[n2] = 0;
            pgrid[n2+1] = 0;
            pgrid[n2+ncol] = 0;
            pgrid[n2+ncol+1] = 0;
        }
    }

/*
    If 3 or more corners of a cell are -1, any point in the cell is
    outside.  If 3 or more are 1, any point is inside.  In all other
    cases, a more exhaustive test will need to be done.  The
    inside_grid array is set to -1 for outside, 1 for inside or
    zero if a more exhaustive test is needed.
*/
    bcell = Raw1.inside_grid;
    for (i=0; i<nrow-1; i++) {
        n = i * ncol;
        for (j=0; j<ncol-1; j++) {
            n2 = n+j;
            n3 = n2+1;
            n4 = n2+ncol;
            n5 = n4+1;
            isum = pgrid[n2] + pgrid[n3] + pgrid[n4] + pgrid[n5];
            if (isum <= -3) {
                bcell[n2] = -1;
            }
            else if (isum >= 3) {
                bcell[n2] = 1;
            }
            else {
                bcell[n2] = 0;
            }
        }
        bcell[n+ncol-1] = -1;
    }

/*
    Done with the first polygon set, do the same stuff using
    the second set.
*/


/*
    reinitialize work grids for second polygon set.
*/
    memset (hgrid, 0, LOCAL_MAX_GRID_SIZE * 2 * sizeof(int));
    memset (pgrid, 0, LOCAL_MAX_GRID_SIZE * 2 * sizeof(int));
    memset (vgrid, 0, LOCAL_MAX_GRID_SIZE * 2 * sizeof(int));

/*
    Setup the grid geometry for the second set of polygons.
*/
    x1 = (double)Raw2.xmin;
    y1 = (double)Raw2.ymin;
    x2 = (double)Raw2.xmax;
    y2 = (double)Raw2.ymax;
    ratio = (x2 - x1) / (y2 - y1);
    if (ratio < .1f) ratio = .1f;
    if (ratio > 10.f) ratio = 10.f;

    xt = (double)LOCAL_MAX_GRID_SIZE / ratio;
    nrow = (int)sqrt(xt) - 1;
    ncol = LOCAL_MAX_GRID_SIZE / nrow - 1;

    xt = (x2 - x1) / (ncol  -  1);
    yt = (y2 - y1) / (nrow  -  1);
    i = (int)(xt + .5);
    if (yt > xt) i = (int)(yt + .5);;

    i /= INTEGER_MULTIPLIER;
    i++;
    i *= INTEGER_MULTIPLIER;
    Raw2.gspace = i;

    ix1 = (int)x1;
    ix1 /= Raw2.gspace;
    ix1 -= 2;
    ix1 *= Raw2.gspace;
    Raw2.gxmin = ix1;

    iy1 = (int)y1;
    iy1 /= Raw2.gspace;
    iy1 -= 2;
    iy1 *= Raw2.gspace;
    Raw2.gymin = iy1;

    ix2 = (int)x2;
    ix2 /= Raw2.gspace;
    ix2 += 3;
    ix2 *= Raw2.gspace;
    Raw2.gxmax = ix2;

    iy2 = (int)y2;
    iy2 /= Raw2.gspace;
    iy2 += 3;
    iy2 *= Raw2.gspace;
    Raw2.gymax = iy2;

    Raw2.ncol = (ix2 - ix1) / Raw2.gspace + 1;
    Raw2.nrow = (iy2 - iy1) / Raw2.gspace + 1;

/*
    set vector crossings for second polygon set
*/
    for (i=0; i<Raw2.nvec; i++) {
        SetHorizontalCrossingsForVector
          (Raw2.x1[i], Raw2.y1[i], Raw2.x2[i], Raw2.y2[i], hgrid,
           Raw2.gxmin, Raw2.gymin, Raw2.gspace, Raw2.ncol, Raw2.nrow);
        SetVerticalCrossingsForVector
          (Raw2.x1[i], Raw2.y1[i], Raw2.x2[i], Raw2.y2[i], vgrid,
           Raw2.gxmin, Raw2.gymin, Raw2.gspace, Raw2.ncol, Raw2.nrow);
    }

/*
    The horizontal crossings are used to guide a scan of the polygon set.
    At the start of each row, the point is always outside.  If a horizontal
    side which is crossed by a polygon edge is encountered, the next node will
    be inside if an odd number of edges crossed the cell side, or it will be
    outside if an even number of vectors crossed.  The pgrid array will be
    1 for inside nodes, -1 for outside and zero for close to an edge.
*/
    nrow = Raw2.nrow;
    ncol = Raw2.ncol;

    for (i=0; i<nrow; i++) {
        flag = -1;
        n = i * ncol;
        for (j=0; j<ncol-1; j++) {
            n2 = n + j;
            if (hgrid[n2] > 0  ||  flag == 0) {
                pgrid[n2] = 0;
                pgrid[n2+1] = 2;
                if (hgrid[n2] % 2 != 0) {
                    flag = -flag;
                }
            }
            else {
                if (pgrid[n2] == 2) {
                    pgrid[n2] = 0;
                }
                else {
                    pgrid[n2] = flag;
                }
            }
        }
        if (pgrid[n+ncol-1] == 2) {
            pgrid[n+ncol-1] = 0;
        }
    }

/*
    Any non zero pgrid nodes adjacent to a vertical crossing
    need to be set to zero.
*/
    for (i=0; i<nrow-1; i++) {
        n = i * ncol;
        for (j=0; j<ncol; j++) {
            n2 = n + j;
            if (vgrid[n2] != 0) {
                pgrid[n2] = 0;
                pgrid[n2+ncol] = 0;
            }
        }
    }

/*
    If any endpoint of an edge vector is still next to non zero
    nodes, the component fits entirely inside a cell.  Set all
    4 corners of the cell to zero in this case.
*/
    ix1 = Raw2.gxmin;
    iy1 = Raw2.gymin;
    spac = Raw2.gspace;
    for (n=0; n<Raw2.nvec; n++) {
        j = (Raw2.x1[n] - ix1) / spac;
        i = (Raw2.y1[n] - iy1) / spac;
        n2 = i * ncol + j;
        if (pgrid[n2] != 0) {
            pgrid[n2] = 0;
            pgrid[n2+1] = 0;
            pgrid[n2+ncol] = 0;
            pgrid[n2+ncol+1] = 0;
        }
    }

/*
    If 3 or more corners of a cell are -1, any point in the cell is
    outside.  If 3 or more are 1, any point is inside.  In all other
    cases, a more exhaustive test will need to be done.  The
    inside_grid array is set to -1 for outside, 1 for inside or
    zero if a more exhaustive test is needed.
*/
    bcell = Raw2.inside_grid;
    for (i=0; i<nrow-1; i++) {
        n = i * ncol;
        for (j=0; j<ncol-1; j++) {
            n2 = n+j;
            n3 = n2+1;
            n4 = n2+ncol;
            n5 = n4+1;
            isum = pgrid[n2] + pgrid[n3] + pgrid[n4] + pgrid[n5];
            if (isum <= -3) {
                bcell[n2] = -1;
            }
            else if (isum >= 3) {
                bcell[n2] = 1;
            }
            else {
                bcell[n2] = 0;
            }
        }
        bcell[n+ncol-1] = -1;
    }

    return 1;

}  /*  end of private function SetupEdgeGrids  */




/*
  ******************************************************************

    S e t H o r i z o n t a l C r o s s i n g s F o r V e c t o r

  ******************************************************************

    Calculate which horizontal grid cell sides a vector crosses.  The
  horizontal sides of the crossed cells are indexed in the hgrid
  array specified.  If the vector crosses one of these sides,
  increment the count of the total number of vectors that cross
  the grid cell side.

*/

int CSWPolyGraph::SetHorizontalCrossingsForVector
                                 (int x1, int y1, int x2, int y2, int *hgrid,
                                  int gx1, int gy1, int gspace, int ncol, int nrow)
{
    double            bint, slope, x, y;
    int               row1, row2, col1, col2, itmp, i, j;
    int               *ctmp;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  calculate slope and intercept if non vertical  */

    if (x1 != x2) {
        slope = (double)(y1-y2) / (double)(x1-x2);
        bint = y1 - slope * x1;
    }
    else {
        slope = 1.e30f;
        bint = 1.e30f;
    }

/*  grid start and end rows and columns  */

    row1 = (y1-gy1) / gspace;
    row2 = (y2-gy1) / gspace;
    col1 = (x1-gx1) / gspace;
    col2 = (x2-gx1) / gspace;

/*
    do nothing if the entire vector is in a single row.
*/
    if (row1 == row2) {
        return 0;
    }

/*  special case for near vertical vector in only one column  */

    else if (col1 == col2) {

        if (row1 > row2) {
            itmp = row1;
            row1 = row2;
            row2 = itmp;
        }

        for (i=row1+1; i<=row2; i++) {
            ctmp = hgrid + i*ncol + col1;
            *ctmp += 1;
        }
    }

/*  general case where the vector crosses rows and columns  */

    else {

        if (row1 > row2) {
            itmp = row1;
            row1 = row2;
            row2 = itmp;
        }

        if (row2 < 0) {
            return 0;
        }
        if (row1 > nrow) {
            return 0;
        }
        if (col2 < 0) {
            return 0;
        }
        if (col1 > ncol) {
            return 0;
        }

        for (i=row1+1; i<=row2; i++) {
            y = gy1 + i * gspace;
            x = (y - bint) / slope;
            j = (int)((x - (double)gx1) / (double)gspace);
            if (j < 0  ||  j > ncol-1) {
                continue;
            }
            ctmp = hgrid + i*ncol + j;
            *ctmp += 1;
        }
    }

    return 0 ;

}  /*  end of private function SetHorizontalCrossingsForVector  */




/*
  ******************************************************************

      S e t V e r t i c a l C r o s s i n g s F o r V e c t o r

  ******************************************************************

    Calculate which vertical grid cell sides a vector crosses.  The
  array is set to 1 if any number of vectors cross a vertical side.

*/

int CSWPolyGraph::SetVerticalCrossingsForVector
                                 (int x1, int y1, int x2, int y2, int *vgrid,
                                  int gx1, int gy1, int gspace, int ncol, int nrow)
{
    double            bint, slope, x, y;
    int               row1, row2, col1, col2, itmp, offset, i, j;
    int               *ctmp;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  calculate slope and intercept if non vertical  */

    if (x1 != x2) {
        slope = (double)(y1-y2) / (double)(x1-x2);
        bint = y1 - slope * x1;
    }
    else {
        slope = 1.e30f;
        bint = 1.e30f;
    }

/*  grid start and end rows and columns  */

    row1 = (y1-gy1) / gspace;
    row2 = (y2-gy1) / gspace;
    col1 = (x1-gx1) / gspace;
    col2 = (x2-gx1) / gspace;

/*
    do nothing if the entire vector is in a single column.
*/
    if (col1 == col2) {
        return 0;
    }

/*  special case for near horizontal vector in only one row  */

    else if (row1 == row2) {

        if (col1 > col2) {
            itmp = col1;
            col1 = col2;
            col2 = itmp;
        }

        offset = row1 * ncol;
        for (i=col1+1; i<=col2; i++) {
            ctmp = vgrid + offset + i;
            *ctmp = 1;
        }
    }

/*  general case where the vector crosses rows and columns  */

    else {

        if (col1 > col2) {
            itmp = col1;
            col1 = col2;
            col2 = itmp;
        }

        if (row2 < 0) {
            return 0;
        }
        if (row1 > nrow) {
            return 0;
        }
        if (col2 < 0) {
            return 0;
        }
        if (col1 > ncol) {
            return 0;
        }

        for (j=col1+1; j<=col2; j++) {
            x = gx1 + j * gspace;
            y = slope * x + bint;
            i = (int)((double)(y-gy1) / (double)gspace);
            if (i < 0  ||  i > nrow - 1) {
                continue;
            }
            ctmp = vgrid + i*ncol + j;
            *ctmp = 1;
        }
    }

    return 0 ;

}  /*  end of private function SetVerticalCrossingsForVector  */



/*
  ****************************************************************************

                        I n s i d e O u t s i d e

  ****************************************************************************

    Return -1 if the point is outside the polygon set, 1 if inside or
    zero if on the edge.

*/

int CSWPolyGraph::InsideOutside (int ix, int iy, int numpoly)
{
    int          i, j, ix0, iy0, gspace, ncol, n, istat,
                 *x1, *y1, *x2, *y2, nvec;
    int          gx1, gy1, gx2, gy2;
    BOxStruct    *bblist;
    int          nbblist;
    int          *iogrid;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (numpoly == 1) {
        ix0 = Raw1.gxmin;
        iy0 = Raw1.gymin;
        gspace = Raw1.gspace;
        ncol = Raw1.ncol;
        x1 = Raw1.x1;
        y1 = Raw1.y1;
        x2 = Raw1.x2;
        y2 = Raw1.y2;
        iogrid = Raw1.inside_grid;
        nvec = Raw1.nvec;
        gx1 = Raw1.gxmin;
        gy1 = Raw1.gymin;
        gx2 = Raw1.gxmax;
        gy2 = Raw1.gymax;
        bblist = Raw1.bblist;
        nbblist = Raw1.nbblist;
    }
    else {
        ix0 = Raw2.gxmin;
        iy0 = Raw2.gymin;
        gspace = Raw2.gspace;
        ncol = Raw2.ncol;
        x1 = Raw2.x1;
        y1 = Raw2.y1;
        x2 = Raw2.x2;
        y2 = Raw2.y2;
        iogrid = Raw2.inside_grid;
        nvec = Raw2.nvec;
        gx1 = Raw2.gxmin;
        gy1 = Raw2.gymin;
        gx2 = Raw2.gxmax;
        gy2 = Raw2.gymax;
        bblist = Raw2.bblist;
        nbblist = Raw2.nbblist;
    }

/*
    If the point is outside of the grid bounds, it must
    also be outside the polygon.
*/
    if (ix <= gx1  ||  ix >= gx2  ||  iy <= gy1  ||  iy >= gy2) {
        return -1;
    }

    i = (iy - iy0) / gspace;
    j = (ix - ix0) / gspace;

    n = i * ncol + j;
    if (iogrid[n] != 0) {
        return iogrid[n];
    }

    istat = PointInteger (x1, y1, x2, y2,
                          nvec, ix, iy, bblist, nbblist);
    return istat;

}  /*  end of private InsideOutside function  */





/*
  ****************************************************************************

                    B u i l d I n i t i a l L i s t s

  ****************************************************************************

    Build the initial segment and node lists (prior to intersecting segments).
  The Raw1 and Raw2 vectors must have been populated before calling this
  function.

*/

int CSWPolyGraph::BuildInitialLists (void)
{
    int          istat, i, xmin, ymin, xmax, ymax, ncol, nrow, nlist;
    int          max_seg_index;
    double       ratio, xt, yt;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    Find the limits of both raw data sets.  These will be used to
    calculate the geometry of both the segment index grid and the
    node index grid.
*/
    xmin = Raw1.gxmin;
    ymin = Raw1.gymin;
    xmax = Raw1.gxmax;
    ymax = Raw1.gymax;
    if (Raw2.gxmin < xmin) xmin = Raw2.gxmin;
    if (Raw2.gymin < ymin) ymin = Raw2.gymin;
    if (Raw2.gxmax > xmax) xmax = Raw2.gxmax;
    if (Raw2.gymax > ymax) ymax = Raw2.gymax;

/*
    For very large data sets, make the segment index grid larger.
*/
    max_seg_index = MAX_SEG_INDEX / 16;

    i = Raw1.nvec + Raw2.nvec;
    if (i > 10) max_seg_index = MAX_SEG_INDEX / 8;
    if (i > 25) max_seg_index = MAX_SEG_INDEX / 4;
    if (i > 50) max_seg_index = MAX_SEG_INDEX / 2;
    if (i > 200) max_seg_index = MAX_SEG_INDEX;
    if (i > 1000) max_seg_index = MAX_SEG_INDEX * 2;
    if (i > 5000) max_seg_index = MAX_SEG_INDEX * 4;
    if (i > 10000) max_seg_index = MAX_SEG_INDEX * 8;
    if (i > 20000) max_seg_index = MAX_SEG_INDEX * 12;

/*
    Setup the grid geometry for the index grids
*/
    xmin /= INTEGER_MULTIPLIER;
    xmin *= INTEGER_MULTIPLIER;
    ymin /= INTEGER_MULTIPLIER;
    ymin *= INTEGER_MULTIPLIER;
    IndexXmin = xmin;
    IndexYmin = ymin;

    ratio = (double)(xmax - xmin) / (double)(ymax - ymin);
    if (ratio < .1) ratio = .1;
    if (ratio > 10.) ratio = 10.;

    xt = (double)max_seg_index / ratio;
    xt = sqrt (xt);
    nrow = (int)xt;
    ncol = (int)((double)max_seg_index / xt);
    if (nrow < 3) nrow = 3;
    if (ncol < 3) ncol = 3;

    xt = (double)(xmax - xmin) / (double)(ncol - 1);
    yt = (double)(ymax - ymin) / (double)(nrow - 1);
    i = (int)(xt + .5);
    if (yt > xt) i = (int)(yt + .5);
    i /= INTEGER_MULTIPLIER;
    i *= INTEGER_MULTIPLIER;
    i += INTEGER_MULTIPLIER;
    i += INTEGER_MULTIPLIER;

    SegIndexSpace = i;
    SegIndexNcol = (xmax - xmin) / i + 2;
    SegIndexNrow = (ymax - ymin) / i + 2;
    SegIndexNrow = SegIndexNrow;

    xt = (double)MAX_NODE_INDEX / ratio;
    nrow = (int)sqrt(xt) - 1;
    ncol = MAX_NODE_INDEX / nrow - 1;

    xt = (double)(xmax - xmin) / (double)(ncol - 1);
    yt = (double)(ymax - ymin) / (double)(nrow - 1);
    i = (int)(xt + .5);
    if (yt > xt) i = (int)(yt + .5);

    NodeIndexSpace = i;
    NodeIndexNcol = (xmax - xmin) / i + 2;
    NodeIndexNrow = (ymax - ymin) / i + 2;
    NodeIndexNrow = NodeIndexNrow;

/*
    Initialize the index grids.
*/
    for (i=0; i<MAX_SEG_INDEX * 16; i++) {
        SegIndex[i] = NULL;
    }
    for (i=0; i<MAX_NODE_INDEX * 2; i++) {
        NodeIndex[i] = NULL;
    }

/*
    Allocate the initial space for the segment and node lists.
    This can grow if needed, but an attempt is made here to
    allocate a reasonable number based on the number of raw
    segments available in both polygon sets.  If the number
    of input edges is very large, normally each edge will be
    relatively short and therefore relatively less likely to
    intersect with any other edge.  Thus, for large input
    data sets, I allocate relatively smaller lists.
*/
    nlist = Raw1.nvec + Raw2.nvec;
    if (nlist < 100) {
        nlist *= 20;
    }
    else if (nlist < 200) {
        nlist *= 15;
    }
    else if (nlist < 500) {
        nlist *= 10;
    }
    else if (nlist < 1000) {
        nlist *= 7;
    }
    else if (nlist < 5000) {
        nlist *= 5;
    }
    else {
        nlist *= 3;
    }

    SegList = (SEgmentStruct *)csw_Calloc (nlist * sizeof(SEgmentStruct));
    if (!SegList) {
        return -1;
    }
    MaxSegs = nlist;

    NodeList = (PLY_NOdeStruct *)csw_Calloc (nlist * sizeof(PLY_NOdeStruct));
    if (!NodeList) {
        return -1;
    }
    MaxNodes = nlist;

    IsegList = (int *)csw_Malloc (MaxSegs * sizeof(int));
    if (IsegList == NULL) {
        return -1;
    }

/*
    Split each raw segment into pieces where each piece is clipped
    by a cell of the segment index grid.  The nodes that are created
    to link these pieces are flagged as temporary nodes in the node
    structure for special treatment after all of the segment intersections
    have been calculated.
*/
    DoNotCheckTempNodes = 0;
    for (i=0; i<Raw1.nvec; i++) {
        istat = InsertSegmentPieces (Raw1.x1[i], Raw1.y1[i],
                                     Raw1.x2[i], Raw1.y2[i], 1,
                                     Raw1.bbid[i], Raw1.compid[i]);
        if (istat == -1) {
            return -1;
        }
        DoNotCheckTempNodes = 1;
        istat = CheckExistingNode (Raw1.x1[i], Raw1.y1[i]);
        if (istat >= 0) {
            NodeList[istat].tag = Raw1.tag1[i];
        }
        istat = CheckExistingNode (Raw1.x2[i], Raw1.y2[i]);
        if (istat >= 0) {
            NodeList[istat].tag = Raw1.tag2[i];
        }
        DoNotCheckTempNodes = 0;
    }

    for (i=0; i<Raw2.nvec; i++) {
        istat = InsertSegmentPieces (Raw2.x1[i], Raw2.y1[i],
                                     Raw2.x2[i], Raw2.y2[i], 2,
                                     Raw2.bbid[i], Raw2.compid[i]);
        if (istat == -1) {
            return -1;
        }
        DoNotCheckTempNodes = 1;
        istat = CheckExistingNode (Raw2.x1[i], Raw2.y1[i]);
        if (istat >= 0) {
            NodeList[istat].tag = Raw2.tag1[i];
        }
        istat = CheckExistingNode (Raw2.x2[i], Raw2.y2[i]);
        if (istat >= 0) {
            NodeList[istat].tag = Raw2.tag2[i];
        }
        DoNotCheckTempNodes = 0;
    }

    return 1;

}  /*  end of private BuildInitialLists function  */





/*
  ****************************************************************************

                  I n s e r t S e g m e n t P i e c e s

  ****************************************************************************

    Split a segment into the subsections that are inside cells of the
  segment index grid.  These subsections are then indexed.  The nodes
  connecting the temporary sub segments are flagged as temporary nodes
  so they can be ignored when eventually constructing polygons from the
  results of the graph boolean operation.

*/

int CSWPolyGraph::InsertSegmentPieces (int px1, int py1, int px2, int py2,
                                int setid, int bbid, int compid)

{
    int          i, j, k, i1, j1, i2, j2, offset,
                 x1, y1, x2, y2, x2sav, istat;
    double       slope, bint;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    BBInsert = bbid;
    CPInsert = compid;

/*
    Special case for a horizontal input segment.
*/
    if (py1 == py2) {
        i1 = (py1 - IndexYmin) / SegIndexSpace;
        offset = i1 * SegIndexNcol;
        j1 = (px1 - IndexXmin) / SegIndexSpace;
        j2 = (px2 - IndexXmin) / SegIndexSpace;
        x1 = px1;
        y1 = py1;
        y2 = py1;

    /*
        Left to right segment.
    */
        if (j1 <= j2) {
            for (j=j1; j<=j2; j++) {
                x2 = (j + 1) * SegIndexSpace + IndexXmin;
                if (j == j2) {
                    x2 = px2;
                }
                k = offset + j;
                if (j == j1  &&  j == j2) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 1);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else if (j == j1) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 0);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else if (j == j2) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 1);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 0);
                    if (istat == -1) {
                        return -1;
                    }
                }
                x1 = x2;
            }
        }

    /*
        Right to left segment.
    */
        else {
            for (j=j1; j>=j2; j--) {
                x2 = j * SegIndexSpace + IndexXmin;
                if (j == j2) {
                    x2 = px2;
                }
                k = offset + j;
                if (j == j1  &&  j == j2) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 1);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else if (j == j1) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 0);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else if (j == j2) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 1);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 0);
                    if (istat == -1) {
                        return -1;
                    }
                }
                x1 = x2;
            }
        }
    }

/*
    Special case for a vertical input segment.
*/
    else if (px1 == px2) {
        j1 = (px1 - IndexXmin) / SegIndexSpace;
        i1 = (py1 - IndexYmin) / SegIndexSpace;
        i2 = (py2 - IndexYmin) / SegIndexSpace;
        y1 = py1;
        x1 = px1;
        x2 = px1;

    /*
        Bottom to top direction.
    */
        if (i1 <= i2) {
            for (i=i1; i<=i2; i++) {
                y2 = (i + 1) * SegIndexSpace + IndexYmin;
                if (i == i2) {
                    y2 = py2;
                }
                k = i * SegIndexNcol + j1;
                if (i == i1  &&  i == i2) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 1);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else if (i == i1) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 0);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else if (i == i2) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 1);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 0);
                    if (istat == -1) {
                        return -1;
                    }
                }
                y1 = y2;
            }
        }

    /*
        Top to bottom direction.
    */
        else {
            for (i=i1; i>=i2; i--) {
                y2 = i * SegIndexSpace + IndexYmin;
                if (i == i2) {
                    y2 = py2;
                }
                k = i * SegIndexNcol + j1;
                if (i == i1  &&  i == i2) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 1);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else if (i == i1) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 0);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else if (i == i2) {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 1);
                    if (istat == -1) {
                        return -1;
                    }
                }
                else {
                    istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 0);
                    if (istat == -1) {
                        return -1;
                    }
                }
                y1 = y2;
            }
        }
    }

/*
    General case for a segment of non zero and non infinite slope.
*/
    else {

        slope = (double)(py2 - py1) / (double)(px2 - px1);
        bint = (double)py1 - slope * (double)px1;

        i1 = (py1 - IndexYmin) / SegIndexSpace;
        i2 = (py2 - IndexYmin) / SegIndexSpace;

    /*
        Bottom to top direction.
    */
        if (i1 <= i2) {
            y1 = py1;
            x2sav = px1;
            for (i=i1; i<=i2; i++) {
                x1 = x2sav;
                y2 = (i + 1) * SegIndexSpace + IndexYmin;
                if (y2 > py2) y2 = py2;
                x2 = (int)(((double)y2 - bint) / slope);
                j1 = (x1 - IndexXmin) / SegIndexSpace;
                j2 = (x2 - IndexXmin) / SegIndexSpace;
                offset = i * SegIndexNcol;
                x2sav = x2;

            /*
                Left to right as well as bottom to top.
            */
                if (j1 <= j2) {
                    for (j=j1; j<=j2; j++) {
                        k = offset + j;
                        x2 = (j + 1) * SegIndexSpace + IndexXmin;
                        if (px2 < px1) {
                            x2 = j * SegIndexSpace + IndexXmin;
                        }
                        if (j == j2) x2 = x2sav;
                        y2 = (int)((double)slope * (double)x2 + bint);
                        if (i == i2  &&  j == j2) {
                            x2 = px2;
                            y2 = py2;
                        }
                        if (i == i1  &&  i == i2  &&  j == j1  &&  j == j2) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 1);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else if (i == i1  &&  j == j1) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 0);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else if (i == i2  &&  j == j2) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 1);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 0);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        x1 = x2;
                        y1 = y2;
                    }
                }

            /*
                Right to left as well as bottom to top.
            */
                else {
                    for (j=j1; j>=j2; j--) {
                        k = offset + j;
                        x2 = j * SegIndexSpace + IndexXmin;
                        if (j == j2) x2 = x2sav;
                        y2 = (int)((double)slope * (double)x2 + bint);
                        if (i == i2  &&  j == j2) {
                            x2 = px2;
                            y2 = py2;
                        }
                        if (i == i1  &&  i == i2  &&  j == j1  &&  j == j2) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 1);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else if (i == i1  &&  j == j1) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 0);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else if (i == i2  &&  j == j2) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 1);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 0);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        x1 = x2;
                        y1 = y2;
                    }
                }
            }
        }

    /*
        Top to bottom direction.
    */
        else {
            y1 = py1;
            x2sav = px1;
            for (i=i1; i>=i2; i--) {
                x1 = x2sav;
                y2 = i * SegIndexSpace + IndexYmin;
                if (y2 < py2) y2 = py2;
                x2 = (int)(((double)y2 - bint) / slope);
                j1 = (x1 - IndexXmin) / SegIndexSpace;
                j2 = (x2 - IndexXmin) / SegIndexSpace;
                offset = i * SegIndexNcol;
                x2sav = x2;

            /*
                Left to right as well as top to bottom.
            */
                if (j1 <= j2) {
                    for (j=j1; j<=j2; j++) {
                        k = offset + j;
                        x2 = (j + 1) * SegIndexSpace + IndexXmin;
                        if (px2 < px1) {
                            x2 = j * SegIndexSpace + IndexXmin;
                        }
                        if (j == j2) x2 = x2sav;
                        y2 = (int)((double)slope * (double)x2 + bint);
                        if (i == i2  &&  j == j2) {
                            x2 = px2;
                            y2 = py2;
                        }
                        if (i == i1  &&  i == i2  &&  j == j1  &&  j == j2) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 1);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else if (i == i1  &&  j == j1) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 0);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else if (i == i2  &&  j == j2) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 1);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 0);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        x1 = x2;
                        y1 = y2;
                    }
                }

            /*
                Right to left as well as top to bottom.
            */
                else {
                    for (j=j1; j>=j2; j--) {
                        k = offset + j;
                        x2 = j * SegIndexSpace + IndexXmin;
                        if (j == j2) x2 = x2sav;
                        y2 = (int)((double)slope * (double)x2 + bint);
                        if (i == i2  &&  j == j2) {
                            x2 = px2;
                            y2 = py2;
                        }
                        if (i == i1  &&  i == i2  &&  j == j1  &&  j == j2) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 1);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else if (i == i1  &&  j == j1) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 1, 0);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else if (i == i2  &&  j == j2) {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 1);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        else {
                            istat = InsertPiece (x1, y1, x2, y2, setid, k, 0, 0);
                            if (istat == -1) {
                                return -1;
                            }
                        }
                        x1 = x2;
                        y1 = y2;
                    }
                }
            }
        }  /*  end of general case  */

/*
    End of if logic separating horizontal, vertical and general cases.
*/
    }

    return 1;

}  /*  end of private InsertSegmentPieces function  */






/*
  ****************************************************************************

                           I n s e r t P i e c e

  ****************************************************************************

    Add the specified segment to the segment and node lists.

*/

int CSWPolyGraph::InsertPiece (int x1in, int y1in, int x2in, int y2in,
                        int setid, int kcell, int node1flag, int node2flag)
{
    SEgmentStruct       *seg;
    PLY_NOdeStruct          *node,
                        *lastnode;
    int                 i, j, k, n1, n2, new1, new2, *list, istat;
    int                 x1, y1, x2, y2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    x1 = x1in;
    y1 = y1in;
    x2 = x2in;
    y2 = y2in;

    SnapPoint (&x1, &y1);
    SnapPoint (&x2, &y2);

/*
    Ignore a zero length segment.
*/
    if (x1 == x2  &&  y1 == y2) {
        return 1;
    }

/*
    Grow the segment and node lists if needed.
*/
    if (NumSegs >= MaxSegs) {
        MaxSegs += LIST_CHUNK;
        SegList = (SEgmentStruct *)csw_Realloc
                  (SegList, MaxSegs * sizeof(SEgmentStruct));
        if (!SegList) {
            return -1;
        }
        memset (SegList + NumSegs, 0,
                (MaxSegs - NumSegs) * sizeof(SEgmentStruct));
    }

    if (NumNodes >= MaxNodes - 1) {
        MaxNodes += LIST_CHUNK;
        NodeList = (PLY_NOdeStruct *)csw_Realloc
                   (NodeList, MaxNodes * sizeof(PLY_NOdeStruct));
        if (!NodeList) {
            return -1;
        }
        memset (NodeList + NumNodes, 0,
                (MaxNodes - NumNodes) * sizeof(PLY_NOdeStruct));
    }

/*
    If the calling function knows that an endpoint is already occupied, then
    the node number for the endpoint is passed in the node1flag and node2flag
    parameters as negative numbers.  In this case, use these node numbers
    rather than creating a new node or searching for a previous node in the
    same location.
*/
    new1 = -1;
    new2 = -1;
    if (node1flag < 0  &&  node2flag < 0) {
        n1 = -node1flag;
        n2 = -node2flag;
        n1--;
        n2--;
    }

    else {

    /*
        See if either endpoint is already occupied by a node.
    */
        n1 = -1;
        n2 = -1;
        if (lastnodenum >= 0) {
            lastnode = NodeList + lastnodenum;
        }
        else {
            lastnode = NULL;
        }

    /*
        The large majority of the cases will be stringing together
        nodes from the pieces separation process.  In these cases,
        the x1,y1 should be the same as the previous x2,y2 and the
        node for the previous x2,y2 should be used.
    */
        if (lastnode) {
            if (x1 == lastnode->x  &&  y1 == lastnode->y) {
                n1 = lastnodenum;
            }
            else if (x2 == lastnode->x  &&  y2 == lastnode->y) {
                n2 = lastnodenum;
            }
        }

    /*
        Use the node index array to check for coincident nodes
        if the lastnode pointer is not coincident.
    */
        if (n1 == -1) {
            n1 = CheckExistingNode (x1, y1);
        }
        if (n2 == -1) {
            n2 = CheckExistingNode (x2, y2);
        }

    /*
        If either node is still unfound, create a new node.
    */
        if (n1 == -1) {
            n1 = NumNodes;
            new1 = n1;
            NumNodes++;
        }
        if (n2 == -1) {
            n2 = NumNodes;
            new2 = n2;
            NumNodes++;
        }
    }

    if (n1 == n2) {
        if (node2flag == 1  &&  node1flag == 0) {
            NodeList[n1].tempflag = 0;
            NodeList[n1].tag = NULL;
        }
        return 1;
    }

/*
    Fill in the segment structure.
*/
    seg = SegList + NumSegs;
    seg->node1 = n1;
    seg->node2 = n2;
    seg->discarded = (char)0;
    seg->setid = (char)setid;
    seg->bbid = BBInsert;
    seg->compid = CPInsert;

    istat = AddToBBList (NumSegs, BBInsert, setid);
    if (istat == -1) {
        return -1;
    }

/*
    Fill in the node structures.
*/
    node = NodeList + n1;
    node->x = x1;
    node->y = y1;
    node->xorig = (x1 + IntegerRange) * TestScale + TestXmin;
    node->yorig = (y1 + IntegerRange) * TestScale + TestYmin;
    if (node1flag >= 0  &&  new1 != -1) {
        if (node1flag == 0) {
            node->tempflag = 1;
        }
        else {
            node->tempflag = 0;
        }
    }
    node->seglist[node->nseg] = NumSegs;
    node->nseg++;
    if (node->nseg >= MAXSEG) {
        return -1;
    }

    node = NodeList + n2;
    node->x = x2;
    node->y = y2;
    node->xorig = (x2 + IntegerRange) * TestScale + TestXmin;
    node->yorig = (y2 + IntegerRange) * TestScale + TestYmin;
    if (node2flag >= 0  &&  new2 != -1) {
        if (node2flag == 0) {
            node->tempflag = 1;
        }
        else {
            node->tempflag = 0;
        }
    }
    node->seglist[node->nseg] = NumSegs;
    node->nseg++;
    if (node->nseg >= MAXSEG) {
        return -1;
    }

/*
    Index the segment.
*/
    list = SegIndex[kcell];
    if (!list) {
        list = (int *)csw_Calloc (12 * sizeof(int));
        if (!list) {
            return -1;
        }
        list[0] = 10;
    }

    else if (list[1] >= list[0]) {
        list[0] += 10;
        list = (int *)csw_Realloc (list, (list[0]+2) * sizeof(int));
        if (!list) {
            return -1;
        }
    }

    i = list[1] + 2;
    list[i] = NumSegs;
    list[1]++;
    NumSegs++;
    SegIndex[kcell] = list;

/*
    Index any new nodes.
*/
    if (new1 >= 0  ||  NewNode1 >= 0) {
        if (new1 < 0) {
            new1 = NewNode1;
        }
        node = NodeList + new1;
        i = (node->y - IndexYmin) / NodeIndexSpace;
        j = (node->x - IndexXmin) / NodeIndexSpace;
        k = i * NodeIndexNcol + j;
        list = NodeIndex[k];
        if (!list) {
            list = (int *)csw_Calloc (12 * sizeof(int));
            if (!list) {
                return -1;
            }
            list[0] = 10;
        }

        else if (list[1] >= list[0]) {
            list[0] += 10;
            list = (int *)csw_Realloc (list, (list[0]+2) * sizeof(int));
            if (!list) {
                return -1;
            }
        }

        i = list[1] + 2;
        list[i] = new1;
        list[1]++;
        NodeIndex[k] = list;
    }

    if (new2 >= 0  ||  NewNode2 >= 0) {
        if (new2 < 0) {
            new2 = NewNode2;
        }
        node = NodeList + new2;
        i = (node->y - IndexYmin) / NodeIndexSpace;
        j = (node->x - IndexXmin) / NodeIndexSpace;
        k = i * NodeIndexNcol + j;
        list = NodeIndex[k];
        if (!list) {
            list = (int *)csw_Calloc (12 * sizeof(int));
            if (!list) {
                return -1;
            }
            list[0] = 10;
        }

        else if (list[1] >= list[0]) {
            list[0] += 10;
            list = (int *)csw_Realloc (list, (list[0]+2) * sizeof(int));
            if (!list) {
                return -1;
            }
        }

        i = list[1] + 2;
        list[i] = new2;
        list[1]++;
        NodeIndex[k] = list;
    }

    return 1;

}  /*  end of private InsertPiece function  */






/*
  ****************************************************************************

                      C h e c k E x i s t i n g N o d e

  ****************************************************************************

    Check the specified x,y coordinate for the presence of another node.  This
  should only be called from the InsertPiece function.

*/

int CSWPolyGraph::CheckExistingNode (int x, int y)
{
    int               i, j, n, *list, ixtmp, iytmp, range;
    PLY_NOdeStruct        *node;

    i = (y - IndexYmin) / NodeIndexSpace;
    j = (x - IndexXmin) / NodeIndexSpace;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);


    list = NodeIndex[i*NodeIndexNcol + j];
    if (!list) {
        return -1;
    }

    n = list[1];
    for (i=0; i<n; i++) {
        j = list[i+2];
        node = NodeList + j;
        if (DoNotCheckTempNodes == 1) {
            if (node->tempflag == 1) continue;
        }
        if (x == node->x  &&  y == node->y) {
            return j;
        }
    }

    range = INTEGER_MULTIPLIER / 2;
    for (i=0; i<n; i++) {
        j = list[i+2];
        node = NodeList + j;
        if (DoNotCheckTempNodes == 1) {
            if (node->tempflag == 1) continue;
        }
        ixtmp = node->x - x;
        iytmp = node->y - y;
        if (ixtmp > -range  &&  ixtmp < range  &&
            iytmp > -range  &&  iytmp < range) {
            return j;
        }
    }

    range = INTEGER_MULTIPLIER * 2;
    for (i=0; i<n; i++) {
        j = list[i+2];
        node = NodeList + j;
        if (DoNotCheckTempNodes == 1) {
            if (node->tempflag == 1) continue;
        }
        ixtmp = node->x - x;
        iytmp = node->y - y;
        if (ixtmp > -range  &&  ixtmp < range  &&
            iytmp > -range  &&  iytmp < range) {
            return j;
        }
    }

    return -1;

}  /*  end of private CheckExistingNode function  */




/*
  ****************************************************************************

          C a l c u l a t e S e g m e n t I n t e r s e c t i o n s

  ****************************************************************************

    Calculate intersections between all segments in the segment list.  When
  an intersection is found, a new node is created and each intersecting
  segment is split into two shorter segments.

*/

int CSWPolyGraph::CalculateSegmentIntersections (void)
{
    int             istat, i, j, k, i0, j0,
                    x1, y1, x2, y2, x21, y21, x22, y22, xt, yt,
                    xint, yint, *list, ndo;
    PLY_NOdeStruct      *node;
    SEgmentStruct   *seg1, *seg2, *ovseg;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    Loop through the entire segment list to check intersections.
    Note that when a segment is split, the new segments are appended
    to the SegList array, and NumSegs is incremented.  These new
    segments will be checked for intersections also since the loop
    does not stop until the NumSegs variable is exceeded.  This
    insures that all segment intersections are found when one segment
    is intersected by more than one other segment.
*/
    i = 0;
    while (i < NumSegs) {

    /*
        Get the endpoints for the first segment.
    */
        seg1 = SegList + i;
        if (seg1->discarded) {
            i++;
            continue;
        }
        node = NodeList + seg1->node1;
        x1 = node->x;
        y1 = node->y;
        node = NodeList + seg1->node2;
        x2 = node->x;
        y2 = node->y;

    /*
        Get the index cell of the first segment.
    */
        xt = (x1 + x2) / 2;
        yt = (y1 + y2) / 2;
        i0 = (yt - IndexYmin) / SegIndexSpace;
        j0 = (xt - IndexXmin) / SegIndexSpace;
        k = i0 * SegIndexNcol + j0;
        if (SegIndex[k] == NULL) {
            i++;
            continue;
        }

    /*
        The second element in the index cell's list is the number
        of segments crossing the cell.  The actual segment numbers
        start with list[2].
    */
        list = SegIndex[k];
        if (list[1] < 2) {
            i++;
            continue;
        }

    /*
        Check for an intersection with each segment that crosses the same index cell.
    */
        ndo = list[1];
        for (j=0; j<ndo; j++) {

        /*
            If the segment in the list has already been the base segment
            in a set of intersection calculations (i.e. it has already
            been or is now the ith segment in the outer loop) then all
            of the intersections with the current segment have already
            been calculated.
        */
            list = SegIndex[k];
            if (list[j+2] <= i) {
                continue;
            }

            seg2 = SegList + list[j+2];
            if (seg2->discarded) continue;
            node = NodeList + seg2->node1;
            x21 = node->x;
            y21 = node->y;
            node = NodeList + seg2->node2;
            x22 = node->x;
            y22 = node->y;

            istat = SegintInteger (x1, y1, x2, y2, x21, y21, x22, y22,
                                   &xint, &yint);
        /*
            If the two segments overlap each other and they are in the
            same polygon set, then an intersection needs to be found.
            This is the endpoint that is completely inside the other
            segment.  The split of the overlapping segments may produce
            duplicate results.  If this happens, the longer of the two
            overlapping segments needs to be discarded.
        */
            ovseg = NULL;
            if (istat == 3 || istat == 2) {
                istat = 0;
                if (IntInside (x1, x21, x22)  &&  IntInside (y1, y21, y22)) {
                    xint = x1;
                    yint = y1;
                    ovseg = seg2;
                }
                else if (IntInside (x2, x21, x22)  &&  IntInside (y2, y21, y22)) {
                    xint = x2;
                    yint = y2;
                    ovseg = seg2;
                }
                else if (IntInside (x21, x1, x2)  &&  IntInside (y21, y1, y2)) {
                    xint = x21;
                    yint = y21;
                    ovseg = seg1;
                }
                else if (IntInside (x22, x1, x2)  &&  IntInside (y22, y1, y2)) {
                    xint = x22;
                    yint = y22;
                    ovseg = seg1;
                }
                else {
                    istat = 2;
                }
            }

        /*
            Attempt to split the segments using this intersection point.
        */
            if (istat == 0) {
                istat = SplitSegments (seg1, seg2,
                                       xint, yint, k);
                if (istat == -1) {
                    return -1;
                }
                if (istat == 0) {
                    if (ovseg) {
                        ovseg->discarded = (char)DiscardFlag;
                    }
                    continue;
                }
            /*
                Update the endpoints for the first segment.
            */
                seg1 = SegList + i;
                node = NodeList + seg1->node1;
                x1 = node->x;
                y1 = node->y;
                node = NodeList + seg1->node2;
                x2 = node->x;
                y2 = node->y;
            }
        }

        i++;

    }  /*  end of while i < NumSegs loop  */

    return 1;

}  /*  end of private CalculateSegmentIntersections function  */





/*
  ****************************************************************************

                        S p l i t S e g m e n t s

  ****************************************************************************

    Split the specified segments at the specified intersection point.  A new
  node is created along with two new segments.  The current segment endpoints
  are adjusted to agree with the spliting.

*/

int CSWPolyGraph::SplitSegments (SEgmentStruct *seg1, SEgmentStruct *seg2,
                          int xint, int yint, int kcell)
{
    int                  istat, nodenum, *list, nlist, i, n1, n2;
    int                  nstat1, nstat2, nstat3, nstat4;
    PLY_NOdeStruct           *node, *node1, *node2;
    SEgmentStruct        *seg;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    If there is already a node at the intersection location, use it.
    Otherwise, create a new node and use it.
*/
    NewNode1 = -1;
    NewNode2 = -1;
    nodenum = CheckExistingNode (xint, yint);
    if (nodenum < 0) {
        nodenum = NumNodes;
        NumNodes++;
        NewNode1 = nodenum;
        if (NumNodes >= MaxNodes) {
            MaxNodes += LIST_CHUNK;
            NodeList = (PLY_NOdeStruct *)csw_Realloc
                       (NodeList, MaxNodes * sizeof(PLY_NOdeStruct));
            if (!NodeList) {
                return -1;
            }
            memset (NodeList + NumNodes, 0,
                    (MaxNodes - NumNodes) * sizeof(PLY_NOdeStruct));
        }
    }
    else {
        if (NodeList[nodenum].tempflag == 1) {
            if (seg1->node2 == seg2->node1 &&  seg1->node2 == nodenum) {
                return 0;
            }
            if (seg1->node1 == seg2->node2 &&  seg1->node1 == nodenum) {
                return 0;
            }
        }
        xint = NodeList[nodenum].x;
        yint = NodeList[nodenum].y;
    }
    NodeList[nodenum].tempflag = 0;

/*
    Check if the new segments produced by the split are
    duplicated in the segment list (same nodes at endpoints).
*/
    nstat1 = 0;
    nstat2 = 0;
    nstat3 = 0;
    nstat4 = 0;
    if (NewNode1 < 0) {
        list = SegIndex[kcell];
        if (list) {
            n1 = nodenum;
            n2 = seg1->node2;
            if (n1 == n2) {
                nstat1 = 1;
            }
            else {
                nlist = list[1];
                for (i=2; i<nlist+2; i++) {
                    seg = SegList + list[i];
                    if (seg->discarded) continue;
                    if (seg->node1 == n1  &&  seg->node2 == n2) {
                        nstat1 = 1;
                        break;
                    }
                    if (seg->node1 == n2  &&  seg->node2 == n1) {
                        nstat1 = 1;
                        break;
                    }
                }
            }
            n2 = seg2->node2;
            if (n1 == n2) {
                nstat2 = 1;
            }
            else {
                nlist = list[1];
                for (i=2; i<nlist+2; i++) {
                    seg = SegList + list[i];
                    if (seg->discarded) continue;
                    if (seg->node1 == n1  &&  seg->node2 == n2) {
                        nstat2 = 1;
                        break;
                    }
                    if (seg->node1 == n2  &&  seg->node2 == n1) {
                        nstat2 = 1;
                        break;
                    }
                }
            }
            n2 = seg1->node1;
            if (n1 != n2) {
                nlist = list[1];
                for (i=2; i<nlist+2; i++) {
                    seg = SegList + list[i];
                    if (seg->discarded) continue;
                    if (seg->node1 == n1  &&  seg->node2 == n2) {
                        nstat3 = 1;
                        break;
                    }
                    if (seg->node1 == n2  &&  seg->node2 == n1) {
                        nstat3 = 1;
                        break;
                    }
                }
            }
            else {
                nstat3 = 1;
            }
            n2 = seg2->node1;
            if (n1 != n2) {
                nlist = list[1];
                for (i=2; i<nlist+2; i++) {
                    seg = SegList + list[i];
                    if (seg->discarded) continue;
                    if (seg->node1 == n1  &&  seg->node2 == n2) {
                        nstat4 = 1;
                        break;
                    }
                    if (seg->node1 == n2  &&  seg->node2 == n1) {
                        nstat4 = 1;
                        break;
                    }
                }
            }
            else {
                nstat4 = 1;
            }
        }
    }

/*
    If there are duplicates of all segments ignore this split.
*/
    if (nstat1 == 1  &&  nstat2 == 1  &&  nstat3 == 1  &&  nstat4 == 1) {
        return 0;
    }

/*
    At least one segment is not duplicated, so prepare the node at
    the intersection for receiving the segments.
*/
    node = NodeList + nodenum;
    node->x = xint;
    node->y = yint;
    node->xorig = (node->x + IntegerRange) * TestScale + TestXmin;
    node->yorig = (node->y + IntegerRange) * TestScale + TestYmin;
    node->tempflag = 0;

/*
    Insert the segments into the SegList array if they are not duplicates.
*/
    if (seg1->node2 != nodenum  &&  seg1->node1 != nodenum) {
        node1 = NodeList + seg1->node2;
        RemoveSegmentFromNode ((int)(seg1 - SegList), node1);
        BBInsert = seg1->bbid;
        CPInsert = seg1->compid;
        istat = InsertPiece (xint, yint, node1->x, node1->y,
                             seg1->setid, kcell, -(nodenum + 1),
                             -(seg1->node2 + 1));
        if (istat == -1) {
            NewNode1 = -1;
            NewNode2 = -1;
            return -1;
        }
        if (seg1->node1 != nodenum) seg1->node2 = nodenum;
        i = seg1 - SegList;
        if (node->nseg >= MAXSEG) {
            NewNode1 = -1;
            NewNode2 = -1;
            return -1;
        }
        node->seglist[node->nseg] = i;
        node->nseg++;
    }
    NewNode1 = -1;

    if (seg2->node2 != nodenum  &&  seg2->node1 != nodenum) {
        node2 = NodeList + seg2->node2;
        RemoveSegmentFromNode ((int)(seg2 - SegList), node2);
        BBInsert = seg2->bbid;
        CPInsert = seg2->bbid;
        istat = InsertPiece (xint, yint, node2->x, node2->y,
                             seg2->setid, kcell, -(nodenum + 1),
                             -(seg2->node2 + 1));
        if (istat == -1) {
            NewNode2 = -1;
            return -1;
        }
        if (seg2->node1 != nodenum) seg2->node2 = nodenum;
        i = seg2 - SegList;
        if (node->nseg >= MAXSEG) {
            NewNode1 = -1;
            NewNode2 = -1;
            return -1;
        }
        node->seglist[node->nseg] = i;
        node->nseg++;
    }
    NewNode2 = -1;

    return 1;

}  /*  end of private SplitSegments function  */






/*
  ****************************************************************************

                R e m o v e S e g m e n t F r o m N o d e

  ****************************************************************************

*/

int CSWPolyGraph::RemoveSegmentFromNode (int segnum, PLY_NOdeStruct *node)
{
    int                     i, j, *list, nseg;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    list = node->seglist;
    nseg = node->nseg;

    if (list == NULL  ||  nseg < 1) {
        return 1;
    }

    for (i=0; i<nseg; i++) {
        if (list[i] == segnum) {
            for (j=i+1; j<nseg; j++) {
                list[j-1] = list[j];
            }
            node->nseg--;
            return 1;
        }
    }

    return 1;

}  /*  end of private RemoveSegmentFromNode function  */






/*
  ****************************************************************************

               R e m o v e D u p l i c a t e S e g m e n t s

  ****************************************************************************

    Remove segments that are exactly on top of other segments by setting
  their discard flags to 1.  Also, any segments that have one or both end
  nodes temporary are discarded.

*/

int CSWPolyGraph::RemoveDuplicateSegments (void)
{
    int              i, j, k, n1, n2, n3, n4, nlist, *list,
                     nfixed, nlist2, k1, k2;
    PLY_NOdeStruct       *node;
    SEgmentStruct    *seg1, *seg2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    Duplicated segments must be attached to the same node, so look
    through each node's segment list for duplicates.  Temporary nodes
    initially introduced in spliting up the segments for indexing
    are no longer needed, so skip them.
*/
    for (i=0; i<NumNodes; i++) {

        node = NodeList + i;
        if (node->tempflag == 1  ||  node->nseg < 3) {
            continue;
        }

        nlist = node->nseg;
        list = node->seglist;

    /*
        There are more than 2 segments attached to the node,
        so check if any segments in the list of attached
        segments are identical with each other.  If identical
        segments are found, arbitrarily flag the last one in
        the list as discarded.
    */
        for (j=0; j<nlist; j++) {
            seg1 = SegList + list[j];
            if (seg1->discarded) {
                continue;
            }
            n1 = seg1->node1;
            n2 = seg1->node2;
            if (!(n1 == i  ||  n2 == i)) {
                continue;
            }

            for (k=j+1; k<nlist; k++) {
                seg2 = SegList + list[k];
                if (seg2->discarded  ||  list[k] == list[j]) {
                    continue;
                }
                n3 = seg2->node1;
                n4 = seg2->node2;
                if (n1 == n3  &&  n2 == n4) {
                    seg2->discarded = (char)DiscardFlag;
                }
                else if (n1 == n4  &&  n2 == n3) {
                    seg2->discarded = (char)DiscardFlag;
                }
            }
        }
    }

/*
    Any segments still left with a temporary node at either end
    must be discarded.  Also, any segments that do not separate an area
    need to be discarded.
*/
    for (i=0; i<NumSegs; i++) {
        seg1 = SegList + i;
        if (seg1->discarded) {
            continue;
        }
        if (NodeList[seg1->node1].tempflag  ||  NodeList[seg1->node2].tempflag) {
            seg1->discarded = (char)DiscardFlag;
        }
    }

/*
    There still can be problem segmemts if several segments retrace back on
    themselves.  Iteratively remove offshoots and spikes until none are
    removed in an iteration.  An offshoot is a node with only one segment
    attached or two segments, but both segments are identical.
*/
    for (;;) {

        nfixed = 0;

        for (i=0; i<NumNodes; i++) {

            if (NodeList[i].tempflag == 1) continue;
            nlist = NodeList[i].nseg;
            list = NodeList[i].seglist;
            nlist2 = 0;
            k1 = -1;
            k2 = -1;
            for (k=0; k<nlist; k++) {
                if (SegList[list[k]].discarded == 0) {
                    if (nlist2 > 0) {
                        k2 = list[k];
                    }
                    else {
                        k1 = list[k];
                    }
                    nlist2++;
                }
            }
            if (nlist2 > 2  ||  nlist2 < 1) continue;

            if (nlist2 == 1  &&  k1 >= 0) {
                seg1 = SegList + k1;
                seg1->discarded = (char)DiscardFlag;
                nfixed++;
                continue;
            }

            if (k1 == -1  ||  k2 == -1) continue;
            seg1 = SegList + k1;
            seg2 = SegList + k2;
            n1 = seg1->node1;
            n2 = seg1->node2;
            n3 = seg2->node1;
            n4 = seg2->node2;
            if (n1 == n3  &&  n2 == n4) {
                seg1->discarded = (char)DiscardFlag;
                seg2->discarded = (char)DiscardFlag;
                nfixed++;
            }
            else if (n1 == n4  &&  n2 == n3) {
                seg1->discarded = (char)DiscardFlag;
                seg2->discarded = (char)DiscardFlag;
                nfixed++;
            }
        }

        if (nfixed == 0) {
            break;
        }

    }

    return 1;

}  /*  end of private RemoveDuplicateSegments function  */





/*
  ****************************************************************************

                R e m o v e T e m p o r a r y N o d e s

  ****************************************************************************

    The temporary nodes used to speed up intersection calculation are removed.
  No segment will have a temporary node at either endpoint when this function
  is finished.

*/

int CSWPolyGraph::RemoveTemporaryNodes (void)
{
    int                 i, j, k, ntemp, n3, jlast, s1, s2;
    int                 sid;
    PLY_NOdeStruct          *node1, *node2;
    SEgmentStruct       *seg1, *seg2, *stmp;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    i = 0;
    while (i < NumSegs) {

        seg1 = SegList + i;
        if (seg1->discarded) {
            i++;
            continue;
        }

        sid = seg1->setid;

        node1 = NodeList + seg1->node1;
        node2 = NodeList + seg1->node2;
        n3 = seg1->node2;

        if (node1->tempflag == 0  &&  node2->tempflag == 1) {

            ntemp = seg1->node2;
            jlast = i;

            for (;;) {

            /*
                Find two segments with the same set id as the starting segment.
                The two segments must both have their discarded flags still
                set to zero or -1.
            */
                s1 = -1;
                s2 = -1;
                for (k=0; k<node2->nseg; k++) {
                    j = node2->seglist[k];
                    stmp = SegList + j;
                    if (stmp->setid == sid  &&  stmp->discarded < 1) {
                        if (s1 < 0) {
                            s1 = j;
                        }
                        else {
                            s2 = j;
                            break;
                        }
                    }
                }

                if (s1 == -1  &&  s2 == -1) {
                    seg1->discarded = (char)DiscardFlag;
                    break;
                }

                j = s1;
                if (j == jlast) {
                    if (s2 == -1) {
                        seg1->discarded = (char)DiscardFlag;
                        break;
                    }
                    j = s2;
                }
                jlast = j;

                seg2 = SegList + j;
                n3 = seg2->node2;
                if (n3 == ntemp) {
                    n3 = seg2->node1;
                }

                ntemp = n3;
                seg2->discarded = (char)DiscardFlag;
                node2 = NodeList + n3;
                if (node2->tempflag == 0) {
                    if (node2->nseg >= MAXSEG) {
                        return -1;
                    }
                    node2->seglist[node2->nseg] = i;
                    node2->nseg++;
                    break;
                }
            }

            if (seg1->discarded == 1) {
                continue;
            }

            seg1->node2 = n3;
            seg1->discarded = 99;
            i++;

        }

        else {
            i++;
        }

    }

    for (i=0; i<NumSegs; i++) {
        if (SegList[i].discarded == 99) {
            SegList[i].discarded = 0;
        }
    }

    return 1;

}  /*  end of private RemoveTemporaryNodes function  */




/*
  ****************************************************************************

                           S n a p N o d e s

  ****************************************************************************

    Move node locations to the nearest multiple of INTEGER_MULTIPLIER in
  x and y.

*/

int CSWPolyGraph::SnapNodes (void)
{
    int                 i, ix, iy, sign;
    PLY_NOdeStruct          *node;
    double              dt;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    for (i=0; i<NumNodes; i++) {
        node = NodeList + i;
        ix = node->x;
        sign = 1;
        if (ix < 0) {
            ix = -ix;
            sign = -1;
        }
        dt = (double)ix / (double)INTEGER_MULTIPLIER + .5;
        ix = (int)dt;
        node->x = sign * ix * INTEGER_MULTIPLIER;
        iy = node->y;
        sign = 1;
        if (iy < 0) {
            iy = -iy;
            sign = -1;
        }
        dt = (double)iy / (double)INTEGER_MULTIPLIER + .5;
        iy = (int)dt;
        node->y = sign * iy * INTEGER_MULTIPLIER;
        node->xorig = (node->x + IntegerRange) * TestScale + TestXmin;
        node->yorig = (node->y + IntegerRange) * TestScale + TestYmin;
    }

    return 1;

}  /*  end of private SnapNodes function  */





/*
  ****************************************************************************

                       I n t e r s e c t G r a p h s

  ****************************************************************************

    Select the segments that are inside the opposite polygon set for output.
  After the selection is finished, build the polygons from the selected segments.

*/

int CSWPolyGraph::IntersectGraphs (void)
{
    int               i, xmid, ymid, istat, tiny, xlen, ylen;
    SEgmentStruct     *seg;
    PLY_NOdeStruct        *node1, *node2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    tiny = INTEGER_MULTIPLIER / 2;

    for (i=0; i<NumSegs; i++) {
        seg = SegList + i;
        if (seg->discarded) {
            continue;
        }
        node1 = NodeList + seg->node1;
        node2 = NodeList + seg->node2;
        PerpX1 = node1->x;
        PerpY1 = node1->y;
        PerpX2 = node2->x;
        PerpY2 = node2->y;
        xmid = (node1->x + node2->x) / 2;
        ymid = (node1->y + node2->y) / 2;
        xlen = node1->x - node2->x;
        if (xlen < 0) xlen = -xlen;
        ylen = node1->y - node2->y;
        if (ylen < 0) ylen = -ylen;
        if (ylen > xlen) xlen = ylen;

        if (seg->setid == 1) {
            istat = InsideOutside (xmid, ymid, 2);
            if (istat == 0) {
                istat = InsideEdgeCheck (xmid, ymid, tiny, 2, xlen);
            }
        }
        else {
            istat = InsideOutside (xmid, ymid, 1);
            if (istat == 0) {
                istat = InsideEdgeCheck (xmid, ymid, tiny, 1, xlen);
            }
        }

        if (istat == -1) {
            seg->discarded = (char)DiscardFlag;
        }
        else {
/*
printf("\nSegment number %d used  from set %d\n", i, seg->setid);
intxy(node1->x, node1->y);
intxy(node2->x, node2->y);
*/
        }
    }


    RemoveProblemSegments ();
    istat = BuildPolygonComponents ();

    return istat;

}  /*  end of private IntersectGraphs function  */





/*
  ****************************************************************************

                         U n i o n G r a p h s

  ****************************************************************************

    Select the segments that are outside of the other polygon and create
  the output polygons by connecting these segments.

*/

int CSWPolyGraph::UnionGraphs (void)
{
    int               i, xmid, ymid, istat, tiny, xlen, ylen;
    SEgmentStruct     *seg;
    PLY_NOdeStruct        *node1, *node2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    tiny = INTEGER_MULTIPLIER / 2;

    for (i=0; i<NumSegs; i++) {
        seg = SegList + i;
        if (seg->discarded) {
            continue;
        }
        node1 = NodeList + seg->node1;
        node2 = NodeList + seg->node2;
        PerpX1 = node1->x;
        PerpY1 = node1->y;
        PerpX2 = node2->x;
        PerpY2 = node2->y;
        xmid = (node1->x + node2->x) / 2;
        ymid = (node1->y + node2->y) / 2;
        xlen = node1->x - node2->x;
        if (xlen < 0) xlen = -xlen;
        ylen = node1->y - node2->y;
        if (ylen < 0) ylen = -ylen;
        if (ylen > xlen) xlen = ylen;

        BBnum = seg->bbid;

        if (seg->setid == 1) {
            istat = InsideOutside (xmid, ymid, 2);
            if (istat == 0) {
                istat = OutsideEdgeCheck (xmid, ymid, tiny, 2, xlen);
            }
            else if (istat == -1) {
                UnionFlag = 1;
                istat = InsideOutside (xmid, ymid, 1);
                UnionFlag = 0;
            }
        }
        else {
            istat = InsideOutside (xmid, ymid, 1);
            if (istat == 0) {
                istat = OutsideEdgeCheck (xmid, ymid, tiny, 1, xlen);
            }
            else if (istat == -1) {
                UnionFlag = 1;
                istat = InsideOutside (xmid, ymid, 2);
                UnionFlag = 0;
            }
        }

        if (istat == 1) {
            seg->discarded = (char)DiscardFlag;
        }
    }

    RemoveProblemSegments ();
    istat = BuildPolygonComponents ();

    return istat;

}  /*  end of private UnionGraphs function  */





/*
  ****************************************************************************

                          X o r G r a p h s

  ****************************************************************************

    Select the segments that are on poly 1 and outside poly 2 and that are on
  poly2 and inside poly 1.  A set of output polygons from this pass is written.
  The second pass selects segments on poly 2 and outside poly 1 and segments
  on poly 1 and inside poly 2.  The polygons from the second pass are appended
  to the polygons written in the first pass.

*/

int CSWPolyGraph::XorGraphs (void)
{
    int               i, xmid, ymid, istat;
    SEgmentStruct     *seg;
    PLY_NOdeStruct        *node1, *node2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    For the first pass, get components from segments outside
    set 1 and inside set 2.
*/
    GrazeDistance = 1;
    for (i=0; i<NumSegs; i++) {
        seg = SegList + i;
        if (seg->discarded == 1) {
            continue;
        }

        node1 = NodeList + seg->node1;
        node2 = NodeList + seg->node2;
        xmid = (node1->x + node2->x) / 2;
        ymid = (node1->y + node2->y) / 2;

        if (seg->setid == 1) {
            istat = InsideOutside (xmid, ymid, 2);
            if (istat == 0) {
                seg->discarded = 1;
                continue;
            }
            if (istat != -1) {
                seg->discarded = 102;
            }
        }
        else {
            istat = InsideOutside (xmid, ymid, 1);
            if (istat == 0) {
                seg->discarded = 1;
                continue;
            }
            if (istat != 1) {
                seg->discarded = 102;
            }
        }
        if (seg->discarded == 0) seg->discarded = 101;
    }
    GrazeDistance = GRAZE;

    RemoveProblemSegments ();
    istat = BuildXorComponents ();

    return istat;

}  /*  end of private XorGraphs function  */





/*
  ****************************************************************************

                B u i l d P o l y g o n C o m p o n e n t s

  ****************************************************************************

    Build polygon components from undiscarded segments.  At each node, there
  should be at most two undiscarded segments when this is called.

*/

int CSWPolyGraph::BuildPolygonComponents (void)
{
    int               i, j, k, istat, n0, nlast, n1, *xcomp, *ycomp;
    int               xmin, ymin, xmax, ymax, nd, senter, ns;
    void              **tagcomp;
    PLY_NOdeStruct        *node;
    SEgmentStruct     *seg1, *seg2;
    int               badflag;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    This is only to suppress a compiler warning.
*/
    seg2 = NULL;

/*
    Find the first non discarded segment and trace from node to node
    until getting back to the starting node.
*/
    i = 0;
    while (i < NumSegs) {

        seg1 = SegList + i;
        if (seg1->discarded) {
            i++;
            continue;
        }

        ns = 0;

        n0 = seg1->node1;
        node = NodeList + n0;
        Xwork[0] = node->x;
        Ywork[0] = node->y;
        Tagwork[0] = node->tag;
        Nwork++;
        if (Nwork >= Pmaxnpts) {
            return -2;
        }

        node = NodeList + seg1->node2;
        j = 1;
        seg1->discarded = -99;
        IsegList[ns] = seg1 - SegList;
        ns++;
        senter = i;
        nlast = seg1->node2;

    /*
        Trace from node to node using the non discarded
        segment at each node.  The segment used to get
        to the node is discarded.  In the absence of
        self intersecting input polygons, only one exit
        segment will exist per node.  If more than one
        exit segment exists, the output polygons will
        probably be self intersecting, and I choose an
        exit segment that will hopefully be the least
        problematic for the self intersecting polygon
        output.
    */
        badflag = 0;
        for (;;) {

            Xwork[j] = node->x;
            Ywork[j] = node->y;
            Tagwork[j] = node->tag;
            j++;
            Nwork++;
            if (Nwork >= Pmaxnpts) {
                return -2;
            }

            istat = -1;
            nd = 0;
            for (k=0; k<node->nseg; k++) {
                seg2 = SegList + node->seglist[k];
                if (seg2->discarded == 0) {
                    istat = k;
                    nd++;
                }
            }

            if (nd > 1) {
                istat = ChooseExitSegment (node, senter,
                                           node->seglist, node->nseg);
            }

            if (istat == -1) {

                if (ns <= 1) {
                    SegList[IsegList[0]].discarded = (char)DiscardFlag;

                    ns = 0;
                    break;
                }

        /*
            This error condition should not happen, but just in case.
        */
/*
                fprintf (stderr,
"Unexpected termination of a polygon building loop.\n\
This indicates a bug in the polygon boolean algorithm.\n");
*/
                SetBugLocation (node->x, node->y);
                badflag = 1;
                break;

            }

            seg2 = SegList + node->seglist[istat];

            n1 = seg2->node2;
            if (n1 == nlast) {
                n1 = seg2->node1;
            }

            nlast = n1;
            node = NodeList + n1;
            seg2->discarded = -99;
            IsegList[ns] = seg2 - SegList;
            ns++;
            senter = seg2 - SegList;

        /*
            Exit the loop if we are back at the starting node.
        */
            if (n1 == n0) {
                break;
            }

        }  /* end of for (;;) loop */

        if (ns == 0  ||  badflag == 1) {
            i++;
            continue;
        }

        for (k=0; k<ns; k++) {
            SegList[IsegList[k]].discarded = (char)DiscardFlag;
        }

        Xwork[j] = Xwork[0];
        Ywork[j] = Ywork[0];
        Tagwork[j] = Tagwork[0];
        j++;
        Nwork++;
        if (Nwork >= Pmaxnpts) {
            return -2;
        }

    /*
        Transfer the component into the CompList array.
    */
        if (Ncomp >= Pmaxcomp) {
            return -3;
        }

        xcomp = (int *)csw_Malloc (2 * j * sizeof(int));
        if (!xcomp) {
            return -1;
        }
        ycomp = xcomp + j;

        tagcomp = (void **)csw_Malloc (j * sizeof(void *));
        if (!tagcomp) {
            csw_Free (xcomp);
            return -1;
        }

        memcpy (xcomp, Xwork, j * sizeof(int));
        memcpy (ycomp, Ywork, j * sizeof(int));
        memcpy (tagcomp, Tagwork, j * sizeof(void*));

        xmin = xcomp[0];
        ymin = ycomp[0];
        xmax = xmin;
        ymax = ymin;
        for (k=1; k<j; k++) {
            if (xcomp[k] < xmin) xmin = xcomp[k];
            if (xcomp[k] > xmax) xmax = xcomp[k];
            if (ycomp[k] < ymin) ymin = ycomp[k];
            if (ycomp[k] > ymax) ymax = ycomp[k];
        }

        CompList[Ncomp].x = xcomp;
        CompList[Ncomp].y = ycomp;
        CompList[Ncomp].tag = tagcomp;
        CompList[Ncomp].npts = j;
        CompList[Ncomp].holeflag = 0;
        CompList[Ncomp].xmin = xmin;
        CompList[Ncomp].ymin = ymin;
        CompList[Ncomp].xmax = xmax;
        CompList[Ncomp].ymax = ymax;
        Ncomp++;

        i++;

    }  /*  end of while i < NumSegs loop  */

    return 1;

}  /*  end of private BuildPolygonComponents function  */





/*
  ****************************************************************************

                          N e s t H o l e s

  ****************************************************************************

  Flag the component list as either a main polygon component or a hole
  of one of the main components.

*/

int CSWPolyGraph::NestHoles (void)
{
    int               i, j, k, xt, yt, istat, nvec, nvec2, maxwork, doneflag;
    int               *x1, *y1, *x2, *y2, *ix1, *iy1, *ix2, *iy2,
                      lastndone, ndone, ii, imain;
    COmponentStruct   *comp, *comp2, *comp3;
    char              *cenv;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);


/*
    Suppress warnings.
*/
    istat = 1;

    if (Ncomp < 2) {
        CompList[0].holeflag = -1;
        return 1;
    }

/*
    If the number of components is quite large, use a version of this
    function that indexes components to speed up the calculation.
*/
    cenv = csw_getenv ("SKIP_INDEXED_NESTING");
    if (Ncomp > NCOMP_INDEXED  &&  cenv == NULL) {
        istat = IndexedNestHoles ();
        if (istat != -99) {
            return istat;
        }
    }

/*
    Allocate arrays for vector endpoints that will be used
    in polygon inside/outside determination.  Two sets of
    arrays are used so that the main component set does not
    need to be rebuilt as often.
*/
    maxwork = 0;
    for (i=0; i<Ncomp; i++) {
        if (CompList[i].npts > maxwork) {
            maxwork = CompList[i].npts;
        }
    }

    x1 = (int *)csw_Malloc (8 * maxwork * sizeof(int));
    if (!x1) {
        return -1;
    }
    y1 = x1 + maxwork;
    x2 = y1 + maxwork;
    y2 = x2 + maxwork;
    ix1 = y2 + maxwork;
    iy1 = ix1 + maxwork;
    ix2 = iy1 + maxwork;
    iy2 = ix2 + maxwork;

/*
    Loop finding mains and their holes until all components have
    been assigned as one or the other.
*/
    lastndone = -1;
    for (;;) {

    /*
        The loop that uses i as a counter checks to see if a component is outside
        all other components.  If that is the case, the component is a main
        component and its holeflag is set to -1.
    */
        for (i=0; i<Ncomp; i++) {

            comp = CompList + i;
            if (comp->holeflag == -2) {
                goto MAIN_COMPONENT_FOUND;
            }
            if (comp->holeflag != 0) {
                continue;
            }

            imain = 0;
            for (ii=0; ii<comp->npts-1; ii++) {

                xt = (comp->x[ii] + comp->x[ii+1]) / 2;
                yt = (comp->y[ii] + comp->y[ii+1]) / 2;

            /*
                Loop through all components but the ith component to see if
                the target point (xt,yt) is inside any other component.  If
                all of the comp points are inside another component, then
                comp cannot be a main polygon.
            */
                istat = -1;
                for (j=0; j<Ncomp; j++) {

                    if (j == i) {
                        continue;
                    }

                    comp2 = CompList + j;
                    if (comp2->holeflag != 0) {
                        continue;
                    }
                    if (comp2->xmin > xt  ||  comp2->ymin > yt  ||
                        comp2->xmax < xt  ||  comp2->ymax < yt) {
                        continue;
                    }
                    BuildCompVecs (x1, y1, x2, y2, comp2);
                    nvec = comp2->npts - 1;

                    istat = PointInteger (x1, y1, x2, y2, nvec, xt, yt, NULL, 0);
                    if (istat == 1) {
                        imain++;
                        break;
                    }
                }
            }

        /*
            All of the points for the comp were inside another component,
            so continue the i loop to find a main component.
        */
            if (imain >= comp->npts-1) {
                continue;
            }

            MAIN_COMPONENT_FOUND:

        /*
            The ith component is outside all other components.  Flag it
            as a main component (holeflag = -1) and find first level
            holes of the ith component.
        */
            comp->holeflag = -1;
            BuildCompVecs (ix1, iy1, ix2, iy2, comp);
            nvec = comp->npts - 1;

            for (j=0; j<Ncomp; j++) {

                if (j == i) {
                    continue;
                }
                comp2 = CompList + j;
                if (comp2->holeflag != 0) {
                    continue;
                }

            /*
             * If all of the comp2 sides are indise the ith comp,
             * then the comp2 comp is inside the ith comp and is
             * a potential level 1 hole.
             */
                imain = 0;
                for (ii=0; ii<comp2->npts-1; ii++) {
                    xt = (comp2->x[ii] + comp2->x[ii+1]) / 2;
                    yt = (comp2->y[ii] + comp2->y[ii+1]) / 2;

                    if (comp->xmin > xt  ||  comp->ymin > yt  ||
                        comp->xmax < xt  ||  comp->ymax < yt) {
                        continue;
                    }

                    istat = PointInteger (ix1, iy1, ix2, iy2, nvec, xt, yt, NULL, 0);
                    if (istat == 1) {
                        imain++;
                    }
                }
                if (imain < comp->npts-1) {
                    continue;
                }

            /*
                If the jth component is inside the ith component, check if
                it is also inside some other component.
            */
                xt = (comp2->x[0] + comp2->x[1]) / 2;
                yt = (comp2->y[0] + comp2->y[1]) / 2;
                istat = -1;
                for (k=0; k<Ncomp; k++) {
                    if (k == i  ||  k == j) {
                        continue;
                    }
                    comp3 = CompList + k;
                    if (comp3->holeflag == -1  ||  comp3->holeflag > 0) {
                        continue;
                    }
                    if (comp3->xmin > xt  ||  comp3->ymin > yt  ||
                        comp3->xmax < xt  ||  comp3->ymax < yt) {
                        continue;
                    }
                    nvec2 = comp3->npts - 1;
                    BuildCompVecs (x1, y1, x2, y2, comp3);
                    istat = PointInteger (x1, y1, x2, y2, nvec2, xt, yt, NULL, 0);
                    if (istat == 1) {
                        break;
                    }
                }

            /*
                If the k loop above never found a component outside the xt,yt point
                from the jth component, then the jth component (comp2) is a first
                level hole of the ith component.
            */
                if (istat == -1) {
                    comp2->holeflag = -(i + 100);
                }

            }  /*  continue the jth component loop  */

            for (j=0; j<Ncomp; j++) {
                if (CompList[j].holeflag < -90) {
                    CompList[j].holeflag = -CompList[j].holeflag - 99;
                }
            }

        }  /*  continue the ith component loop  */

    /*
        If all components have been assigned, exit the loop.
    */
        doneflag = 1;
        ndone = 0;
        for (i=0; i<Ncomp; i++) {
            if (CompList[i].holeflag == 0) {
                doneflag = 0;
            }
            else {
                ndone++;
            }
        }

        if (doneflag == 1  ||  ndone == lastndone) {
            break;
        }

        lastndone = ndone;

    }  /*  end of loop that continues until all components are assigned  */

    for (i=0; i<Ncomp; i++) {
        if (CompList[i].holeflag == 0) {
            CompList[i].holeflag = -1;
        }
    }

    if (x1) csw_Free (x1);

    return 1;

}  /*  end of private NestHoles function  */





/*
  ****************************************************************************

                        B u i l d C o m p V e c s

  ****************************************************************************

    This is called only from the NestHoles function.  It converts a continuous
  point line into an array of vectors.

*/

int CSWPolyGraph::BuildCompVecs (int *x1, int *y1, int *x2, int *y2,
                          COmponentStruct *comp)
{
    int                 i;

    for (i=0; i<comp->npts-1; i++) {

        x1[i] = comp->x[i] + RAW_OFFSET;
        y1[i] = comp->y[i];
        x2[i] = comp->x[i+1] + RAW_OFFSET;
        y2[i] = comp->y[i+1];

    }

    return 1;

}  /*  end of private BuildCompVecs function  */





/*
  ****************************************************************************

                  B u i l d O u t p u t P o l y g o n s

  ****************************************************************************

    Populate the output point and component arrays from the internal
  component structure array.  All main components will be immediately
  followed by their hole components in the output.

*/

int CSWPolyGraph::BuildOutputPolygons (void)
{
    int            i, j, n, nc, nv, nmain, tmp, *ix, *iy, npts;
    int            istat;
    void           **tagloc;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    *Pnpout = 0;
    n = 0;
    nv = 0;
    nc = 0;

    for (;;) {

    /*
        Search for a main component in the component list.
    */
        nmain = -1;
        for (i=0; i<Ncomp; i++) {
            if (CompList[i].holeflag == -1) {
                nmain = i;
                break;
            }
        }
        if (nmain == -1) {
            break;
        }

        Pncout[nc] = 0;

    /*
        Output the main component.
    */
        ix = CompList[nmain].x;
        iy = CompList[nmain].y;
        tagloc = static_cast<void **> (CompList[nmain].tag);
        npts = CompList[nmain].npts;
        for (j=0; j<npts; j++) {
            tmp = ix[j];
            Pxout[n] = (tmp + IntegerRange) * TestScale + TestXmin;
            tmp = iy[j];
            Pyout[n] = (tmp + IntegerRange) * TestScale + TestYmin;
            if (Ptagout != NULL) {
                Ptagout[n] = tagloc[j];
            }
            n++;
            if (n >= Pmaxnpts) {
                return -2;
            }
        }

        Pnvout[nv] = npts;
        nv++;
        (Pncout[nc])++;

    /*
        Done with the main component, find its holes.  Note that the holeflag
        member of the component structure is actually the sequence number of the
        main parent plus one.
    */
        CompList[nmain].holeflag = -2;

        for (i=0; i<Ncomp; i++) {

            if (CompList[i].holeflag != nmain + 1) {
                continue;
            }

        /*
            Output the points for the hole.
        */
            ix = CompList[i].x;
            iy = CompList[i].y;
            tagloc = static_cast <void **> (CompList[i].tag);
            npts = CompList[i].npts;
            for (j=0; j<npts; j++) {
                tmp = ix[j];
                Pxout[n] = (tmp + IntegerRange) * TestScale + TestXmin;
                tmp = iy[j];
                Pyout[n] = (tmp + IntegerRange) * TestScale + TestYmin;
                if (Ptagout != NULL) {
                    Ptagout[n] = tagloc[j];
                }
                n++;
                if (n >= Pmaxnpts) {
                    return -2;
                }
            }

            Pnvout[nv] = npts;
            (Pncout[nc])++;
            nv++;
            CompList[i].holeflag = -2;
        }

    /*
        Done with this main/holes combination, find the next.
    */
        nc++;

    }

    if (FragmentFlag == 1) {
        istat = AddUntouchedInputPolygons (&nc, &nv);
        if (istat != 1) {
            return istat;
        }
    }

    *Pnpout = nc;

    return 1;

}  /*  end of private BuildOutputPolygons function  */



/*
  ****************************************************************************

                          F r e e A l l M e m

  ****************************************************************************

    Free the memory used in the boolean operation.

*/

int CSWPolyGraph::FreeAllMem (void)
{
    int              i;

    FreeBBLists ();

    if (Raw1.x1) {
        csw_Free (Raw1.x1);
        Raw1.x1 = NULL;
    }

    if (Raw2.x1) {
        csw_Free (Raw2.x1);
        Raw2.x1 = NULL;
    }

    if (Raw1.tag1) {
        csw_Free (Raw1.tag1);
        Raw1.tag1 = NULL;
    }

    if (Raw2.tag1) {
        csw_Free (Raw2.tag1);
        Raw1.tag1 = NULL;
    }

    if (IsegList) {
        csw_Free (IsegList);
        IsegList = NULL;
    }

    if (SegList) {
        csw_Free (SegList);
        SegList = NULL;
    }

    if (NodeList) {
        csw_Free (NodeList);
        NodeList = NULL;
    }

    for (i=0; i<MAX_SEG_INDEX * 16; i++) {
        if (SegIndex[i]) {
            csw_Free (SegIndex[i]);
            SegIndex[i] = NULL;
        }
    }

    for (i=0; i<MAX_NODE_INDEX * 2; i++) {
        if (NodeIndex[i]) {
            csw_Free (NodeIndex[i]);
            NodeIndex[i] = NULL;
        }
    }

    if (CompList) {
        for (i=0; i<Pmaxcomp; i++) {
            if (CompList[i].x) csw_Free (CompList[i].x);
            if (CompList[i].tag) csw_Free (CompList[i].tag);
        }
        csw_Free (CompList);
        CompList = NULL;
    }

    if (Xwork) {
        csw_Free (Xwork);
        Xwork = NULL;
        Ywork = NULL;
    }

    if (Tagwork) {
        csw_Free (Tagwork);
        Tagwork= NULL;
    }

    NumSegs = 0;
    NumNodes = 0;

    csw_Free (InputCompIDFlag1);
    InputCompIDFlag1 = NULL;
    InputCompIDFlag2 = NULL;

    XYTiny = 0.0;

    return 1;

}  /*  end of private FreeAllMem function  */




int CSWPolyGraph::segxy (int segnum)

{
    SEgmentStruct         *seg;
    PLY_NOdeStruct            *node1, *node2;
    double                x1, y1, x2, y2;
    int                   tmp;

    seg = SegList + segnum;
    node1 = NodeList + seg->node1;
    node2 = NodeList + seg->node2;

    tmp = node1->x;
    x1 = (tmp + IntegerRange) * TestScale + TestXmin;
    tmp = node1->y;
    y1 = (tmp + IntegerRange) * TestScale + TestYmin;
    tmp = node2->x;
    x2 = (tmp + IntegerRange) * TestScale + TestXmin;
    tmp = node2->y;
    y2 = (tmp + IntegerRange) * TestScale + TestYmin;

    fprintf (stderr, "%6.3f %6.3f    %6.3f %6.3f\n",
             x1, y1, x2, y2);

    return 1;
}


int CSWPolyGraph::rawxy (int *x1, int *y1, int *x2, int *y2, int n)

{
    int                   tmp, i;
    CSW_F                 fx1, fy1, fx2, fy2;

    fprintf (stderr, "\n");
    for (i=0; i<n; i++) {
        tmp = x1[i];
        fx1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
        tmp = y1[i];
        fy1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);
        tmp = x2[i];
        fx2 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
        tmp = y2[i];
        fy2 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);

        fprintf (stderr, "%9.5f %9.5f    %9.5f %9.5f\n",
                 fx1, fy1, fx2, fy2);
    }
    fprintf (stderr, "\n");

    return 1;
}



int CSWPolyGraph::nodexy (int n1, int n2)

{
    PLY_NOdeStruct            *node1, *node2;
    double                x1, y1, x2, y2;
    int                   tmp;

    node1 = NodeList + n1;
    node2 = NodeList + n2;

    tmp = node1->x;
    x1 = (tmp + IntegerRange) * TestScale + TestXmin;
    tmp = node1->y;
    y1 = (tmp + IntegerRange) * TestScale + TestYmin;
    tmp = node2->x;
    x2 = (tmp + IntegerRange) * TestScale + TestXmin;
    tmp = node2->y;
    y2 = (tmp + IntegerRange) * TestScale + TestYmin;

    fprintf (stderr, "%6.2f %6.2f    %6.2f %6.2f\n",
             x1, y1, x2, y2);

    return 1;
}



int CSWPolyGraph::intxy (int x, int y)

{
    double                x1, y1;

    x1 = (x + IntegerRange) * TestScale + TestXmin;
    y1 = (y + IntegerRange) * TestScale + TestYmin;

    fprintf (stderr, "%9.5f %9.5f\n", x1, y1);

    return 1;
}






/*
  ****************************************************************************

                     I n s i d e E d g e C h e c k

  ****************************************************************************

    This is called if the target point is on an edge of the opposite polygon
  set.  The target point is moved perpendicular to the line it is on until
  it is inside its own polygon set and then that point is checked against the
  target polygon set.

*/

int CSWPolyGraph::InsideEdgeCheck (int p, int q, int tiny, int setid, int ilen)
{
    int           set2, istat;
    int           xt1, yt1, xt2, yt2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    set2 = 1;
    if (setid == 1) set2 = 2;

/*
    First, check inside/outside with a smaller edge grazing value
    in case the problem is from a very short segment.
*/
    if (ilen <= EDGE_CHECK_LENGTH) {
        GrazeDistance = 2;
        istat = InsideOutside (p, q, set2);
        if (istat != 0) {
            GrazeDistance = GRAZE;
            return istat;
        }
    }

/*
    Next, check for an offset as described above in the function header.
*/
    while (tiny <= INTEGER_MULTIPLIER * 6) {
        PerpPoints (tiny, &xt1, &yt1, &xt2, &yt2);
        istat = InsideOutside (xt1, yt1, set2);
        if (istat == 1) {
            istat = InsideOutside (xt1, yt1, setid);
            GrazeDistance = GRAZE;
            return istat;
        }

        istat = InsideOutside (xt2, yt2, set2);
        if (istat == 1) {
            istat = InsideOutside (xt2, yt2, setid);
            GrazeDistance = GRAZE;
            return istat;
        }

        tiny += INTEGER_MULTIPLIER / 2;
    }

    GrazeDistance = GRAZE;

    return 0;

}  /*  end of private InsideEdgeCheck function  */



/*
  ****************************************************************************

                                T o o C l o s e

  ****************************************************************************

    Return 1 if the specified points are closer than the TOO_CLOSE symbol.

*/

int CSWPolyGraph::TooClose (int x1, int y1, int x2, int y2)
{
    int             dx, dy;

    dx = x2 - x1;
    if (dx < 0) dx = -dx;
    dy = y2 - y1;
    if (dy < 0) dy = -dy;

    if (dx <= TOO_CLOSE  &&  dy <= TOO_CLOSE) {
        return 1;
    }

    return 0;

}  /*  end of private TooClose function  */





/*
  ****************************************************************************

                           I n t I n s i d e

  ****************************************************************************

    Check if an integer number is between two other integer numbers.  If
    the bounding numbers are the same and the target is the same, 1 is
    returned.  In all other cases, 0 is returned if the target is on or
    outside the endpoints, otherwise 1 is returned.

*/

int CSWPolyGraph::IntInside (int i1, int i2, int i3)
{

    if (i2 == i3) {
        if (i1 == i2) return 1;
        return 0;
    }

    if ((double)(i2-i1) * (double)(i1-i3) > 0) {
        return 1;
    }

    return 0;

}  /*  end of private IntInside function  */



/*
  ****************************************************************************

                                V e r y C l o s e

  ****************************************************************************

    Return 1 if the specified points are closer than VERY_CLOSE

*/

int CSWPolyGraph::VeryClose (int x1, int y1, int x2, int y2)
{
    int             dx, dy;

    dx = x2 - x1;
    if (dx < 0) dx = -dx;
    dy = y2 - y1;
    if (dy < 0) dy = -dy;

    if (dx <= VERY_CLOSE  &&  dy <= VERY_CLOSE) {
        return 1;
    }

    return 0;

}  /*  end of private VeryClose function  */





/*
  ****************************************************************************

                 R e m o v e P r o b l e m S e g m e n t s

  ****************************************************************************

    This is called just before BuildPolygonComponents.  If any nodes have
  other than 0 or 2 non discarded segments, some of the segments may be
  problems.  This function will remove any of these problem segments that
  are completely interior to their own polygon.

*/

int CSWPolyGraph::RemoveProblemSegments (void)
{
    int          i, j, k, *list, nlist, nd, n1, n2;
    PLY_NOdeStruct   *node;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    for (i=0; i<NumSegs; i++) {
        if (SegList[i].node1 == SegList[i].node2) {
            SegList[i].discarded = (char)DiscardFlag;
        }
    }

    for (i=0; i<NumNodes; i++) {

        node = NodeList + i;
        if (node->tempflag == 1) {
            continue;
        }
        nlist = node->nseg;
        if (nlist == 2  ||  nlist == 0) {
            continue;
        }
        list = node->seglist;

        nd = 0;
        for (j=0; j<nlist; j++) {
            k = list[j];
            if (SegList[k].discarded == DiscardFlag) {
                continue;
            }
            n1 = SegList[k].node1;
            n2 = SegList[k].node2;
            if (!(n1 == i  ||  n2 == i)) {
                continue;
            }
            nd++;
        }

        if (nd == 2  ||  nd == 0) {
            continue;
        }

        for (j=0; j<nlist; j++) {
            k = list[j];
            if (SegList[k].discarded == DiscardFlag) {
                continue;
            }
            n1 = SegList[k].node1;
            n2 = SegList[k].node2;
            if (!(n1 == i  ||  n2 == i)) {
                continue;
            }
            DiscardInteriorSegment (k);
        }

    }

    return 1;

}  /*  end of private RemoveProblemSegments function  */






/*
  ****************************************************************************

               D i s c a r d I n t e r i o r S e g m e n t

  ****************************************************************************

    If a segment is completely inside its own polygon, discard it.  This
  function returns 1 if the segment needs to be discarded or zero if not.

*/

int CSWPolyGraph::DiscardInteriorSegment (int segnum)
{
    SEgmentStruct  *seg;
    int            xt1, yt1, xt2, yt2, i1, i2;
    int            x1, y1, x2, y2, offset;
    double         slope, dang, cosang, sinang, dxm, dym;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    offset = INTEGER_MULTIPLIER * 2;

    seg = SegList + segnum;
    x1 = NodeList[seg->node1].x;
    y1 = NodeList[seg->node1].y;
    x2 = NodeList[seg->node2].x;
    y2 = NodeList[seg->node2].y;

    if (x1 == x2  &&  y1 == y2) {
        seg->discarded = (char)DiscardFlag;
        return 1;
    }

    if (x1 == x2) {
        yt1 = (y1 + y2) / 2;
        yt2 = yt1;
        xt1 = x1 + offset;
        xt2 = x1 - offset;
    }
    else if (y1 == y2) {
        xt1 = (x1 + x2) / 2;
        xt2 = xt1;
        yt1 = y1 - offset;
        yt2 = y1 + offset;
    }
    else {
        slope = (double)(y2 - y1) / (double)(x2 - x1);
        slope = -(1.0 / slope);
        dang = atan(slope);
        cosang = cos (dang);
        sinang = sin (dang);
        dxm = (x1 + x2) / 2.0;
        dym = (y1 + y2) / 2.0;
        xt1 = (int)(dxm + (double)offset * cosang);
        yt1 = (int)(dym + (double)offset * sinang);
        xt2 = (int)(dxm - (double)offset * cosang);
        yt2 = (int)(dym - (double)offset * sinang);
    }

    i1 = InsideOutside (xt1, yt1, seg->setid);
    i2 = InsideOutside (xt2, yt2, seg->setid);

    if (i1 == i2) {
        seg->discarded = (char)DiscardFlag;
        return 1;
    }

    return 0;

}  /*  end of private DiscardInteriorSegment function  */






/*
  ****************************************************************************

                     I n d e x e d N e s t H o l e s

  ****************************************************************************

    Flag the component list as either a main polygon component or a hole
  of one of the main components.  This version indexes the components to
  speed up the calculations.

*/

int CSWPolyGraph::IndexedNestHoles (void)
{
    int               i, j, k, xt, yt, istat, nvec, nvec2, maxwork, doneflag;
    int               *x1, *y1, *x2, *y2, *ix1, *iy1, *ix2, *iy2;
    int               *index[1000], *list, xmin, ymin, xmax, ymax,
                      xspace, yspace, i1, j1, i2, j2, ii, jj, kk, offset,
                      isize, nlist, *list2, *ltmp, maxlist2, chunk;
    int               jlast, il, kl, lt, good, jdo, *dolist;
    COmponentStruct   *comp, *comp2, *comp3;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    Suppress warnings.
*/
    istat = 1;
    chunk = 1000;
    maxlist2 = chunk;

    if (Ncomp < 2) {
        CompList[0].holeflag = -1;
        return 1;
    }

/*
    Set up the index grid geometry for the polygon components.
*/
    for (i=0; i<1000; i++) {
        index[i] = NULL;
    }

    xmin = CompList[0].xmin;
    ymin = CompList[0].ymin;
    xmax = CompList[0].xmax;
    ymax = CompList[0].ymax;
    for (i=1; i<Ncomp; i++) {
        if (CompList[i].xmin < xmin) xmin = CompList[i].xmin;
        if (CompList[i].ymin < ymin) ymin = CompList[i].ymin;
        if (CompList[i].xmax > xmax) xmax = CompList[i].xmax;
        if (CompList[i].ymax > ymax) ymax = CompList[i].ymax;
    }
    xmin--;
    ymin--;
    xmax++;
    ymax++;
    xspace = (xmax - xmin) / 30;
    yspace = (ymax - ymin) / 30;
    xspace++;
    yspace++;

/*
    Fill in the index grid.
*/
    for (i=0; i<Ncomp; i++) {

        i1 = (CompList[i].ymin - ymin) / yspace;
        i2 = (CompList[i].ymax - ymin) / yspace;
        j1 = (CompList[i].xmin - xmin) / xspace;
        j2 = (CompList[i].xmax - xmin) / xspace;

        for (ii=i1; ii<=i2; ii++) {
            offset = ii * 32;
            for (jj=j1; jj<=j2; jj++) {
                kk = offset + jj;
                list = index[kk];
                if (index[kk] == NULL) {
                    index[kk] = (int *)csw_Malloc (12 * sizeof(int));
                    if (index[kk] == NULL) {
                        FreeIndex (index, 1000);
                        return -99;
                    }
                    list = index[kk];
                    list[0] = 10;
                    list[1] = 0;
                }
                if (list[0] >= list[1]) {
                    isize = list[0] + 12;
                    list = static_cast<int *>
                        (csw_Realloc ((void *)list, isize * sizeof(int)));
                    if (!list) {
                        FreeIndex (index, 1000);
                        return -99;
                    }
                    list[0] = isize - 2;
                    index[kk] = list;
                }
                list[list[1]+2] = i;
                list[1]++;
            }
        }
    }

/*
    Allocate arrays for vector endpoints that will be used
    in polygon inside/outside determination.  Two sets of
    arrays are used so that the main component set does not
    need to be rebuilt as often.
*/
    maxwork = 0;
    for (i=0; i<Ncomp; i++) {
        if (CompList[i].npts > maxwork) {
            maxwork = CompList[i].npts;
        }
    }

    x1 = (int *)csw_Malloc (8 * maxwork * sizeof(int));
    if (!x1) {
        FreeIndex (index, 1000);
        return -1;
    }
    y1 = x1 + maxwork;
    x2 = y1 + maxwork;
    y2 = x2 + maxwork;
    ix1 = y2 + maxwork;
    iy1 = ix1 + maxwork;
    ix2 = iy1 + maxwork;
    iy2 = ix2 + maxwork;

/*
    Loop finding mains and their holes until all components have
    been assigned as one or the other.
*/
    for (;;) {

    /*
        The loop that uses i as a counter checks to see if a component is outside
        all other components.  If that is the case, the component is a main
        component and its holeflag is set to -1.
    */
        for (i=0; i<Ncomp; i++) {

            comp = CompList + i;
            if (comp->holeflag == -2) {
                goto MAIN_COMPONENT_FOUND;
            }
            if (comp->holeflag != 0) {
                continue;
            }

            xt = (comp->x[0] + comp->x[1]) / 2;
            yt = (comp->y[0] + comp->y[1]) / 2;

            ii = (yt - ymin) / yspace;
            jj = (xt - xmin) / xspace;
            kk = ii * 32 + jj;
            list = index[kk];
            if (list == NULL) {
                goto MAIN_COMPONENT_FOUND;
            }
            if (list[1] < 2) {
                goto MAIN_COMPONENT_FOUND;
            }
            nlist = list[1];
            list += 2;

        /*
            Loop through all components but the ith component to see if
            the target point (xt,yt) is inside any other component.  If
            this is the case, then the ith component cannot be a main
            component.
        */
            istat = -1;
            for (j=0; j<nlist; j++) {

                if (list[j] == i) {
                    continue;
                }

                comp2 = CompList + list[j];
                if (comp2->holeflag != 0) {
                    continue;
                }
                if (comp2->xmin > xt  ||  comp2->ymin > yt  ||
                    comp2->xmax < xt  ||  comp2->ymax < yt) {
                    continue;
                }
                BuildCompVecs (x1, y1, x2, y2, comp2);
                nvec = comp2->npts - 1;

                istat = PointInteger (x1, y1, x2, y2, nvec, xt, yt, NULL, 0);
                if (istat == 1) {
                    break;
                }

            }

        /*
            The point was inside another component, so continue the i loop
            to find a main component.
        */
            if (istat == 1) {
                continue;
            }

            MAIN_COMPONENT_FOUND:

        /*
            The ith component is outside all other components.  Flag it
            as a main component (holeflag = -1) and find first level
            holes of the ith component.
        */
            comp->holeflag = -1;
            BuildCompVecs (ix1, iy1, ix2, iy2, comp);
            nvec = comp->npts - 1;

        /*
            Only the components that are inside the bounding box of the main component
            can possible be holes of the main component, so use the index to generate
            a list of these components to check for hole status.
        */
            i1 = (comp->ymin - ymin) / yspace;
            i2 = (comp->ymax - ymin) / yspace;
            j1 = (comp->xmin - xmin) / xspace;
            j2 = (comp->xmax - xmin) / xspace;
            jlast = 0;
            list2 = (int *)csw_Malloc (maxlist2 * sizeof(int));
            if (list2 != NULL) {
                for (ii=i1; ii<=i2; ii++) {
                    offset = ii * 32;
                    for (jj=j1; jj<=j2; jj++) {
                        kk = offset + jj;
                        ltmp = index[kk];
                        nlist = ltmp[1];
                        ltmp += 2;
                        for (kl=0; kl<nlist; kl++) {
                            lt = ltmp[kl];
                            good = 1;
                            for (il=0; il<jlast; il++) {
                                if (list2[il] == lt) {
                                    good = 0;
                                    break;
                                }
                            }
                            if (good == 1) {
                                list2[jlast] = lt;
                                jlast++;
                                if (jlast > maxlist2) {
                                    maxlist2 += chunk;
                                    list2 = (int *)csw_Realloc (list2, maxlist2 * sizeof(int));
                                    if (list2 == NULL) {
                                        goto LIST2_ERROR;
                                    }
                                }
                            }
                        }
                    }
                }

                LIST2_ERROR:

                if (list2 == NULL) {
                    dolist = NULL;
                    jlast = Ncomp;
                }
                else {
                    dolist = list2;
                }
            }
            else {
                dolist = NULL;
                jlast = Ncomp;
            }

            for (jdo=0; jdo<jlast; jdo++) {

                if (dolist) {
                    j = dolist[jdo];
                }
                else {
                    j = jdo;
                }

                if (j == i) {
                    continue;
                }
                comp2 = CompList + j;
                if (comp2->holeflag != 0) {
                    continue;
                }

                xt = (comp2->x[0] + comp2->x[1]) / 2;
                yt = (comp2->y[0] + comp2->y[1]) / 2;

                ii = (yt - ymin) / yspace;
                jj = (xt - xmin) / xspace;
                kk = ii * 32 + jj;
                list = index[kk];
                if (comp->xmin > xt  ||  comp->ymin > yt  ||
                    comp->xmax < xt  ||  comp->ymax < yt) {
                    continue;
                }

                istat = PointInteger (ix1, iy1, ix2, iy2, nvec, xt, yt, NULL, 0);
                if (istat == -1) {
                    continue;
                }

            /*
                If the index cell has less than 3 polygons in it, then one
                of the polygons must be the main polygon (component i) and
                the other must be the current hole candidate (component j).
                Since no other polygons exist, j must be a first level hole
                of i.
            */
                if (list == NULL) {
                    comp2->holeflag = -(i + 100);
                    continue;
                }
                if (list[1] < 3) {
                    comp2->holeflag = -(i + 100);
                    continue;
                }
                nlist = list[1];
                list += 2;

            /*
                If the jth component is inside the ith component, check if
                it is also inside some other component.
            */
                istat = -1;
                for (k=0; k<nlist; k++) {
                    if (list[k] == i  ||  list[k] == j) {
                        continue;
                    }
                    comp3 = CompList + list[k];
                    if (comp3->holeflag == -1  ||  comp3->holeflag > 0) {
                        continue;
                    }
                    if (comp3->xmin > xt  ||  comp3->ymin > yt  ||
                        comp3->xmax < xt  ||  comp3->ymax < yt) {
                        continue;
                    }
                    nvec2 = comp3->npts - 1;
                    BuildCompVecs (x1, y1, x2, y2, comp3);
                    istat = PointInteger (x1, y1, x2, y2, nvec2, xt, yt, NULL, 0);
                    if (istat == 1) {
                        break;
                    }
                }

            /*
                If the k loop above never found a component outside the xt,yt point
                from the jth component, then the jth component (comp2) is a first
                level hole of the ith component.
            */
                if (istat == -1) {
                    comp2->holeflag = -(i + 100);
                }

            }  /*  continue the jth component loop  */

            for (jdo=0; jdo<jlast; jdo++) {
                j = jdo;
                if (dolist) {
                    j = dolist[jdo];
                }
                if (CompList[j].holeflag < -90) {
                    CompList[j].holeflag = -CompList[j].holeflag - 99;
                }
            }
            if (list2) {
                csw_Free (list2);
            }
            list2 = NULL;
            dolist = NULL;

        }  /*  continue the ith component loop  */

    /*
        If all components have been assigned, exit the loop.
    */
        doneflag = 1;
        for (i=0; i<Ncomp; i++) {
            if (CompList[i].holeflag == 0) {
                doneflag = 0;
                break;
            }
        }

        if (doneflag == 1) {
            break;
        }

    }  /*  end of loop that continues until all components are assigned  */

    if (x1) csw_Free (x1);
    FreeIndex (index, 1000);

    return 1;

}  /*  end of private IndexedNestHoles function  */




/*
  ****************************************************************************

                           F r e e I n d e x

  ****************************************************************************

    Free the lists in an index grid.  This is only called from function
  IndexedNestHoles.

*/

int CSWPolyGraph::FreeIndex (int **index, int n)
{
    int         i;

    for (i=0; i<n; i++) {
        if (index[i] != NULL) {
            csw_Free (index[i]);
        }
    }

    return 1;

}  /*  end of private FreeIndex function  */




/*
  ****************************************************************************

                           S n a p P o i n t

  ****************************************************************************

    Snap a single point in the same fashion as nodes.

*/

int CSWPolyGraph::SnapPoint (int *x, int *y)
{
    int                 ix, iy, sign;
    double              dt;

    ix = *x;
    sign = 1;
    if (ix < 0) {
        ix = -ix;
        sign = -1;
    }
    dt = (double)ix / (double)INTEGER_MULTIPLIER + .5;
    ix = (int)dt;
    *x = sign * ix * INTEGER_MULTIPLIER;
    iy = *y;
    sign = 1;
    if (iy < 0) {
        iy = -iy;
        sign = -1;
    }
    dt = (double)iy / (double)INTEGER_MULTIPLIER + .5;
    iy = (int)dt;
    *y = sign * iy * INTEGER_MULTIPLIER;

    return 1;

}  /*  end of private SnapPoint function  */






/*
  ****************************************************************************

                                  p d s

  ****************************************************************************

    Used for debugging, prints a summary of all segments attached to a node.

*/

int CSWPolyGraph::pds (PLY_NOdeStruct *node)
{
    int               i, tmp;
    CSW_F             x1, y1, x2, y2;
    SEgmentStruct     *seg;
    PLY_NOdeStruct        *node1, *node2;

    for (i=0; i<node->nseg; i++) {

        seg = SegList + node->seglist[i];
        node1 = NodeList + seg->node1;
        node2 = NodeList + seg->node2;

        tmp = node1->x;
        x1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
        tmp = node1->y;
        y1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);
        tmp = node2->x;
        x2 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
        tmp = node2->y;
        y2 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);

        printf ("%6d %6d %6d %2d   %f %f %f %f\n",
                node->seglist[i], seg->node1, seg->node2,
                seg->discarded, x1, y1, x2, y2);

    }
    printf ("\n");

    return 1;

}  /*  end of private pds function  */






/*
  ****************************************************************************

                     O u t s i d e E d g e C h e c k

  ****************************************************************************

    This is called if the target point is on an edge of the opposite polygon
  set.  The target point is moved slightly until it is outside its own
  polygon set, and then that point is checked against the target polygon set.
  If the new point is outside both polygon sets, the segment needs to be
  kept.  If no point outside both sets is found, the segment is discarded.

    This function returns -1 if a close point outside both polygon sets is
  found or it returns 1 if no such point is found.

*/

int CSWPolyGraph::OutsideEdgeCheck (int p, int q, int tiny, int setid, int ilen)
{
    int           set2, istat, xt1, yt1, xt2, yt2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    set2 = 1;
    if (setid == 1) set2 = 2;

/*
    First, check inside/outside with a smaller edge grazing value
    in case the problem is from a very short segment.
*/
    if (ilen <= EDGE_CHECK_LENGTH) {
        GrazeDistance = 2;
        istat = InsideOutside (p, q, set2);
        if (istat != 0) {
            GrazeDistance = GRAZE;
            return istat;
        }
    }

/*
    Next, check for an offset as described above in the function header.
*/
    while (tiny <= INTEGER_MULTIPLIER * 3) {
        PerpPoints (tiny, &xt1, &yt1, &xt2, &yt2);
        istat = InsideOutside (xt1, yt1, set2);
        if (istat == -1) {
            istat = InsideOutside (xt1, yt1, setid);
            if (istat == -1) {
                GrazeDistance = GRAZE;
                return -1;
            }
        }

        istat = InsideOutside (xt2, yt2, set2);
        if (istat == -1) {
            istat = InsideOutside (xt2, yt2, setid);
            if (istat == -1) {
                GrazeDistance = GRAZE;
                return -1;
            }
        }

        tiny += INTEGER_MULTIPLIER / 2;
    }

    GrazeDistance = GRAZE;

/*
    No close point outside both can be found, so return a status that
    will discard the segment being tested.
*/
    return 1;

}  /*  end of private OutsideEdgeCheck function  */




#if 0

/*
  ****************************************************************************

                         X o r E d g e C h e c k

  ****************************************************************************

    If a close point is inside of the opposite polygon set and inside of
  the polygon current polygon set, do not discard the segment.  If the
  segment is to be discarded, 0 is returned.  If it should be kept, 2
  is returned.

*/

CSWPolyGraph::int XorEdgeCheck (int p, int q, int tiny, int setid)
{
    int           set2, istat, xt, yt;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    set2 = 1;
    if (setid == 1) set2 = 2;

/*
    First, check inside/outside with a smaller edge grazing value
    in case the problem is from a very short segment.
*/
    GrazeDistance = 2;
    istat = InsideOutside (p, q, set2);
    if (istat != 0) {
        GrazeDistance = GRAZE;
        return istat;
    }

/*
    Next, check for an offset as described above in the function header.
*/
    while (tiny <= INTEGER_MULTIPLIER * 3) {
        xt = p + tiny;
        yt = q + tiny;
        istat = InsideOutside (xt, yt, set2);
        if (istat == -1) {
            istat = InsideOutside (xt, yt, setid);
            if (istat == 1) {
                GrazeDistance = GRAZE;
                return 2;
            }
        }

        xt = p - tiny;
        yt = q + tiny;
        istat = InsideOutside (xt, yt, set2);
        if (istat == -1) {
            istat = InsideOutside (xt, yt, setid);
            if (istat == 1) {
                GrazeDistance = GRAZE;
                return 2;
            }
        }

        xt = p - tiny;
        yt = q - tiny;
        istat = InsideOutside (xt, yt, set2);
        if (istat == -1) {
            istat = InsideOutside (xt, yt, setid);
            if (istat == 1) {
                GrazeDistance = GRAZE;
                return 2;
            }
        }

        xt = p + tiny;
        yt = q - tiny;
        istat = InsideOutside (xt, yt, set2);
        if (istat == -1) {
            istat = InsideOutside (xt, yt, setid);
            if (istat == 1) {
                GrazeDistance = GRAZE;
                return 2;
            }
        }

        tiny += INTEGER_MULTIPLIER / 2;
        GrazeDistance += 2;
        if (GrazeDistance > GRAZE) GrazeDistance = GRAZE;
    }

    GrazeDistance = GRAZE;

    return 0;

}  /*  end of private XorEdgeCheck function  */

#endif





/*
  ****************************************************************************

                B u i l d X o r C o m p o n e n t s

  ****************************************************************************

    Build polygon components after the xor operation on the graphs.  The xor
  differentuiates between two types of segments.  When possible, the exit segment
  from a node will use the same type of segment as the entrance segment.

*/

int CSWPolyGraph::BuildXorComponents (void)
{
    int               i, j, k, istat, n0, nlast, n1, *xcomp, *ycomp;
    int               xmin, ymin, xmax, ymax, nd, senter;
    PLY_NOdeStruct        *node;
    SEgmentStruct     *seg1, *seg2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    This is only to suppress a compiler warning.
*/
    seg2 = NULL;

/*
    Find the first non discarded segment and trace from node to node
    until getting back to the starting node.
*/
    i = 0;
    while (i < NumSegs) {

        seg1 = SegList + i;
        if (seg1->discarded == 1) {
            i++;
            continue;
        }

        n0 = seg1->node1;
        node = NodeList + n0;
        Xwork[0] = node->x;
        Ywork[0] = node->y;
        Nwork++;
        if (Nwork >= Pmaxnpts) {
            return -2;
        }

        node = NodeList + seg1->node2;
        j = 1;
        nlast = seg1->node2;
        senter = seg1 - SegList;

    /*
        Trace from node to node using the non discarded
        segment at each node.  If there is more than one
        segment, use the one with the same discarded value
        as the entry segment.
    */
        for (;;) {

            Xwork[j] = node->x;
            Ywork[j] = node->y;
            j++;
            Nwork++;
            if (Nwork >= Pmaxnpts) {
                return -2;
            }

            istat = -1;
            nd = 0;
            for (k=0; k<node->nseg; k++) {
                seg2 = SegList + node->seglist[k];
                if (seg1 == seg2) continue;
                if (seg2->discarded == seg1->discarded) {
                    istat = k;
                    nd++;
                }
            }

            if (nd > 1) {
                istat = ChooseXorExitSegment (node, senter, seg1->discarded,
                                              node->seglist, node->nseg);
            }

            if (istat == -1) {
                nd = 0;
                for (k=0; k<node->nseg; k++) {
                    seg2 = SegList + node->seglist[k];
                    if (seg1 == seg2) continue;
                    if (seg2->discarded != 1) {
                        istat = k;
                        nd++;
                    }
                }
                if (nd > 1) {
                    istat = ChooseXorExitSegment (node, senter, 0,
                                                  node->seglist, node->nseg);
                }
            }

        /*
            This error condition should not happen, but just in case.
        */
            if (istat == -1) {
                fprintf (stderr,
"Unexpected termination of a xor polygon building loop.\n\
This indicates a bug in the polygon boolean algorithm.\n");
                return -1;
            }

            seg2 = SegList + node->seglist[istat];

            n1 = seg2->node2;
            if (n1 == nlast) {
                n1 = seg2->node1;
            }

            nlast = n1;
            node = NodeList + n1;
            seg1->discarded = 1;
            seg1 = seg2;
            senter = seg1 - SegList;

        /*
            Exit the loop if we are back at the starting node.
        */
            if (n1 == n0) {
                break;
            }

        }

        seg2->discarded = 1;
        Xwork[j] = Xwork[0];
        Ywork[j] = Ywork[0];
        j++;
        Nwork++;
        if (Nwork >= Pmaxnpts) {
            return -2;
        }

    /*
        Transfer the component into the CompList array.
    */
        if (Ncomp >= Pmaxcomp) {
            return -3;
        }

        xcomp = (int *)csw_Malloc (2 * j * sizeof(int));
        if (!xcomp) {
            return -1;
        }

        ycomp = xcomp + j;

        memcpy (xcomp, Xwork, j * sizeof(int));
        memcpy (ycomp, Ywork, j * sizeof(int));

        xmin = xcomp[0];
        ymin = ycomp[0];
        xmax = xmin;
        ymax = ymin;
        for (k=1; k<j; k++) {
            if (xcomp[k] < xmin) xmin = xcomp[k];
            if (xcomp[k] > xmax) xmax = xcomp[k];
            if (ycomp[k] < ymin) ymin = ycomp[k];
            if (ycomp[k] > ymax) ymax = ycomp[k];
        }

        CompList[Ncomp].x = xcomp;
        CompList[Ncomp].y = ycomp;
        CompList[Ncomp].npts = j;
        CompList[Ncomp].holeflag = 0;
        CompList[Ncomp].xmin = xmin;
        CompList[Ncomp].ymin = ymin;
        CompList[Ncomp].xmax = xmax;
        CompList[Ncomp].ymax = ymax;
        Ncomp++;

        i++;

    }  /*  end of while i < NumSegs loop  */

    return 1;

}  /*  end of private BuildXorComponents function  */




int CSWPolyGraph::rawxy2 (int p, int q,
            int *x1, int *y1, int *x2, int *y2, int n)

{
    int                   tmp, i;
    CSW_F                 fp, fq, fx1, fy1, fx2, fy2;
    CSW_F                 dist, mindist;

    mindist = (CSW_F)(10000.0 * TestScale);

    fp = (CSW_F)((p + IntegerRange) * TestScale + TestXmin);
    fq = (CSW_F)((q + IntegerRange) * TestScale + TestYmin);
    fprintf (stderr, "\n");
    for (i=0; i<n; i++) {
        tmp = x1[i];
        fx1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
        tmp = y1[i];
        fy1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);
        tmp = x2[i];
        fx2 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
        tmp = y2[i];
        fy2 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);

        gpf_perpdistance1 (fx1, fy1, fx2, fy2, fp, fq, &dist);
        if (dist <= mindist) {
            fprintf (stderr, "%d %9.5f %9.5f    %9.5f %9.5f\n",
                 i, fx1, fy1, fx2, fy2);
        }
    }
    fprintf (stderr, "\n");

    return 1;
}



int CSWPolyGraph::prcomp (COmponentStruct *comp, int nmax)

{
    int            i, ndo, tmp;
    CSW_F          x1, y1;

    ndo = nmax;
    if (nmax < 1) ndo = comp->npts;

    fprintf (stderr, "\n");
    for (i=0; i<ndo; i++) {
        tmp = comp->x[i];
        x1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
        tmp = comp->y[i];
        y1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);
        fprintf (stderr, "%9.4f %9.4f\n", x1, y1);
    }
    fprintf (stderr, "\n");

    return 1;
}





/*
  ****************************************************************************

                     C h o o s e E x i t S e g m e n t

  ****************************************************************************

  If more than one non discarded segment exists at a node, choose one that
  is not colinear with the entry segment.  The index within the specified
  list of the chosen segment is returned.

*/

int CSWPolyGraph::ChooseExitSegment (PLY_NOdeStruct *node,
                              int sentry, int *list, int nlist)
{
    int            i, j, k, nd;
    int            nodenum, tmp, outseg;
    CSW_F          x1, y1, x2, y2, xn, yn, dist, dmax;
    SEgmentStruct  *seg, *stmp;
    PLY_NOdeStruct     *node2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    tmp = node->x;
    xn = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
    tmp = node->y;
    yn = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);

    seg = SegList + sentry;
    nodenum = node - NodeList;
    if (seg->node1 != nodenum) {
        node2 = NodeList + seg->node1;
    }
    else {
        node2 = NodeList + seg->node2;
    }

    tmp = node2->x;
    x1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
    tmp = node2->y;
    y1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);

    outseg = -1;
    dmax = -1.e30f;
    for (i=0; i<nlist; i++) {
        j = list[i];
        if (j == sentry) {
            continue;
        }
        seg = SegList + j;
        if (seg->discarded == 1) {
            continue;
        }
        if (seg->node1 != nodenum) {
            node2 = NodeList + seg->node1;
        }
        else {
            node2 = NodeList + seg->node2;
        }

    /*
        Make sure there is at least one segment other than
        this segment that is available from the node at the
        far end of the segment.
    */
        nd = 0;
        for (k=0; k<node2->nseg; k++) {
            if (node2->seglist[k] == j) continue;
            stmp = SegList + node2->seglist[k];
            if (stmp->discarded == 0) {
                nd = 1;
                break;
            }
        }

        if (nd == 0) continue;

        tmp = node2->x;
        x2 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
        tmp = node2->y;
        y2 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);

        gpf_perpdsquare (x1, y1, x2, y2, xn, yn, &dist);
        if (dist > dmax) {
            dmax = dist;
            outseg = i;
        }
    }

    return outseg;

}  /*  end of private ChooseExitSegment function  */






/*
  ****************************************************************************

                 C h o o s e X o r E x i t S e g m e n t

  ****************************************************************************

    If more than one non discarded segment exists at a node, choose one that
  is not colinear with the entry segment.

*/

int CSWPolyGraph::ChooseXorExitSegment (PLY_NOdeStruct *node,
                              int sentry, int discardflag,
                              int *list, int nlist)
{
    int            i, j;
    int            nodenum, tmp, outseg;
    CSW_F          x1, y1, x2, y2, xn, yn, dist, dmax;
    SEgmentStruct  *seg;
    PLY_NOdeStruct     *node2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    tmp = node->x;
    xn = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
    tmp = node->y;
    yn = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);

    seg = SegList + sentry;
    nodenum = node - NodeList;
    if (seg->node1 != nodenum) {
        node2 = NodeList + seg->node1;
    }
    else {
        node2 = NodeList + seg->node2;
    }

    tmp = node2->x;
    x1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
    tmp = node2->y;
    y1 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);

    outseg = -1;
    dmax = -1.e30f;
    for (i=0; i<nlist; i++) {
        j = list[i];
        if (j == sentry) {
            continue;
        }
        seg = SegList + j;
        if (seg->discarded == 1) {
            continue;
        }
        if (discardflag != 0) {
            if (seg->discarded != discardflag) {
                continue;
            }
        }
        if (seg->node1 != nodenum) {
            node2 = NodeList + seg->node1;
        }
        else {
            node2 = NodeList + seg->node2;
        }
        tmp = node2->x;
        x2 = (CSW_F)((tmp + IntegerRange) * TestScale + TestXmin);
        tmp = node2->y;
        y2 = (CSW_F)((tmp + IntegerRange) * TestScale + TestYmin);

        gpf_perpdsquare (x1, y1, x2, y2, xn, yn, &dist);
        if (dist > dmax) {
            dmax = dist;
            outseg = i;
        }
    }

    return outseg;

}  /*  end of private ChooseXorExitSegment function  */




/*
  ****************************************************************************

                       P e r p P o i n t s

  ****************************************************************************

    This function returns two points, perpendicular to the current Perp
    vector and dist away from it.  This is tightly coupled to the InsideEdgeCheck
    and OutsideEdgeCheck functions.

*/

int CSWPolyGraph::PerpPoints (int dist, int *x1, int *y1, int *x2, int *y2)
{
    double          slope, ang, cang, sang;
    double          xmid, ymid;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    xmid = (double)(PerpX1 + PerpX2) / 2.0;
    ymid = (double)(PerpY1 + PerpY2) / 2.0;

/*
    Vertical line case.
*/
    if (PerpX2 == PerpX1) {
        *y1 = (int)ymid;
        *y2 = (int)ymid;
        *x1 = (int)xmid - dist;
        *x2 = (int)xmid + dist;
        return 1;
    }

/*
    Horizontal line case.
*/
    if (PerpY1 == PerpY2) {
        *x1 = (int)xmid;
        *x2 = (int)xmid;
        *y1 = (int)ymid - dist;
        *y2 = (int)ymid + dist;
        return 1;
    }

/*
    General case.
*/
    slope = (double)(PerpY2 - PerpY1) / (double)(PerpX2 - PerpX1);
    slope = -1.0 / slope;
    ang = atan(slope);
    cang = cos(ang) * (double)dist;
    sang = sin(ang) * (double)dist;

    *x1 = (int)(xmid + cang);
    *y1 = (int)(ymid + sang);
    *x2 = (int)(xmid - cang);
    *y2 = (int)(ymid - sang);

    return 1;

}  /*  end of private PerpPoints function  */






/*
  ****************************************************************************

                      R e s e t R a w V e c t o r s

  ****************************************************************************

    Repopulate the raw vector lists with the coordinates of the segments.  This
  is needed because the segments have been snapped to node locations and in
  order to do proper inside/outside/edge calculations the raw polygon vectors
  must be exactly the same.

*/

int CSWPolyGraph::ResetRawVectors (void)
{
    int             i, j, n1, n2, nraw;
    int             *list, nlist;
    SEgmentStruct   *seg;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    First, count the non discarded segments in each set.
*/
    n1 = 0;
    n2 = 0;
    for (i=0; i<NumSegs; i++) {
        seg = SegList + i;
        if (seg->discarded) continue;

        if (seg->setid == 1) {
            n1++;
        }
        else {
            n2++;
        }
    }

    n1 += 10;
    n2 += 10;

/*
    Repopulate the Raw1 segments.  This has to be done in bounding
    box id order so that the InsideOutside polygon stuff will work.
*/
    csw_Free (Raw1.x1);
    Raw1.x1 = (int *)csw_Malloc (n1 * 4 * sizeof(int));
    if (!Raw1.x1) {
        return -1;
    }
    Raw1.y1 = Raw1.x1 + n1;
    Raw1.x2 = Raw1.x1 + n1 * 2;
    Raw1.y2 = Raw1.x1 + n1 * 3;
    Raw1.bbid = NULL;
    Raw1.compid = NULL;

    nraw = 0;
    for (i=0; i<Raw1.nbblist; i++) {
        list = Raw1.bblist[i].seglist;
        nlist = Raw1.bblist[i].nseg;
        Raw1.bblist[i].start = nraw;
        for (j=0; j<nlist; j++) {
            seg = SegList + list[j];
            if (seg->discarded > 0) continue;
            Raw1.x1[nraw] = NodeList[seg->node1].x;
            Raw1.y1[nraw] = NodeList[seg->node1].y;
            Raw1.x2[nraw] = NodeList[seg->node2].x;
            Raw1.y2[nraw] = NodeList[seg->node2].y;
            nraw++;
            if (seg->discarded == -1) seg->discarded = 1;
        }
        Raw1.bblist[i].size = nraw - Raw1.bblist[i].start;
    }
    Raw1.nvec = nraw;

/*
    Repopulate the Raw2 segments in the same manner.
*/
    csw_Free (Raw2.x1);
    Raw2.x1 = (int *)csw_Malloc (n2 * 4 * sizeof(int));
    if (!Raw2.x1) {
        return -1;
    }
    Raw2.y1 = Raw2.x1 + n2;
    Raw2.x2 = Raw2.x1 + n2 * 2;
    Raw2.y2 = Raw2.x1 + n2 * 3;
    Raw2.bbid = NULL;
    Raw2.compid = NULL;

    nraw = 0;
    for (i=0; i<Raw2.nbblist; i++) {
        list = Raw2.bblist[i].seglist;
        nlist = Raw2.bblist[i].nseg;
        Raw2.bblist[i].start = nraw;
        for (j=0; j<nlist; j++) {
            seg = SegList + list[j];
            if (seg->discarded > 0) continue;
            Raw2.x1[nraw] = NodeList[seg->node1].x;
            Raw2.y1[nraw] = NodeList[seg->node1].y;
            Raw2.x2[nraw] = NodeList[seg->node2].x;
            Raw2.y2[nraw] = NodeList[seg->node2].y;
            nraw++;
            if (seg->discarded == -1) seg->discarded = 1;
        }
        Raw2.bblist[i].size = nraw - Raw2.bblist[i].start;
    }
    Raw2.nvec = nraw;

    return 1;

}  /*  end of private ResetRawVectors function  */




/*
  ****************************************************************************

                           A d d T o B B L i s t

  ****************************************************************************

    Add the specifed segment number to the bblist associated with setid
  and bbid.  On success, 1 is returned.  If a csw_Malloc fails, -1 is returned.

*/

int CSWPolyGraph::AddToBBList (int segnum, int bbid, int setid)
{
    int         *list, max, n;
    BOxStruct   *bbptr;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (setid == 1) {
        bbptr = Raw1.bblist;
    }
    else {
        bbptr = Raw2.bblist;
    }

    max = bbptr[bbid].maxseg;
    n = bbptr[bbid].nseg;
    list = bbptr[bbid].seglist;

    if (n >= max) {
        max += 100;
        list = (int *)csw_Realloc (list, max * sizeof(int));
        if (!list) {
            bbptr[bbid].seglist = NULL;
            return -1;
        }
    }

    list[n] = segnum;
    n++;

    bbptr[bbid].nseg = n;
    bbptr[bbid].maxseg = max;
    bbptr[bbid].seglist = list;

    return 1;

}  /*  end of private AddToBBList function  */





/*
  ****************************************************************************

                          F r e e B B L i s t s

  ****************************************************************************

    Free all memory associated with the BOxStruct lists in Raw1 and Raw2.

*/

int CSWPolyGraph::FreeBBLists (void)
{
    int          i;

    if (Raw1.bblist) {
        for (i=0; i<Raw1.nbblist; i++) {
            if (Raw1.bblist[i].seglist) csw_Free (Raw1.bblist[i].seglist);
        }
        csw_Free (Raw1.bblist);
        Raw1.bblist = NULL;
        Raw1.nbblist = 0;
    }

    if (Raw2.bblist) {
        for (i=0; i<Raw2.nbblist; i++) {
            if (Raw2.bblist[i].seglist) csw_Free (Raw2.bblist[i].seglist);
        }
        csw_Free (Raw2.bblist);
        Raw2.bblist = NULL;
        Raw2.nbblist = 0;
    }

    return 1;

}  /*  end of private FreeBBLists function  */





/*
  ****************************************************************************

                      R e m o v e O v e r l a p s

  ****************************************************************************

    If a segment from a node contains the end point of another segment from
  the node, discard the longer segment.  This is called just after duplicates
  have been discarded.

*/

int CSWPolyGraph::RemoveOverlaps (void)
{
    int                 i, j, k, istat, nseg, n, *list;
    int                 p, q, x1, y1, x2, y2;
    PLY_NOdeStruct          *node;
    SEgmentStruct       *seg;
    int                 opposite_node, middle_node;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    for (i=0; i<NumNodes; i++) {

        node = NodeList + i;
        if (node->tempflag) continue;
        nseg = node->nseg;
        if (nseg < 3) continue;
        list = node->seglist;

        n = 0;
        for (j=0; j<nseg; j++) {
            seg = SegList + list[j];
            if (seg->discarded) continue;
            n++;
        }

        if (n < 3) continue;

        x1 = node->x;
        y1 = node->y;

        for (j=0; j<nseg; j++) {

            seg = SegList + list[j];
            if (seg->discarded) continue;

            if (seg->node1 == i) {
                x2 = NodeList[seg->node2].x;
                y2 = NodeList[seg->node2].y;
                opposite_node = seg->node2;
            }
            else {
                x2 = NodeList[seg->node1].x;
                y2 = NodeList[seg->node1].y;
                opposite_node = seg->node1;
            }

            for (k=0; k<nseg; k++) {
                if (k == j) continue;
                seg = SegList + list[k];
                if (seg->discarded) continue;
                if (seg->node1 == i) {
                    p = NodeList[seg->node2].x;
                    q = NodeList[seg->node2].y;
                    middle_node = seg->node2;
                }
                else {
                    p = NodeList[seg->node1].x;
                    q = NodeList[seg->node1].y;
                    middle_node = seg->node1;
                }

                istat = PointOnLine (p, q, x1, y1, x2, y2);
                if (istat == 1) {
                    ConnectForOverlap (list[j], middle_node, opposite_node);
                    SegList[list[j]].discarded = 1;
                    break;
                }
            }
        }
    }

    return 1;

}  /*  end of private RemoveOverlaps function  */


int CSWPolyGraph::ConnectForOverlap (int ioldseg, int middle_node, int opposite_node)
{
    int          *list, nlist, i;
    PLY_NOdeStruct   *node;
    SEgmentStruct *seg, *newseg, *oldseg;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);


/*
 * If the opposite node is already connected to the
 * middle node, nothing needs to be done.
 */
    node = NodeList + middle_node;
    list = node->seglist;
    nlist = node->nseg;
    for (i=0; i<nlist; i++) {
        seg = SegList + list[i];
        if (seg->node1 == opposite_node  ||
            seg->node2 == opposite_node) {
            return 1;
        }
    }

/*
    Grow the segment list if needed.
*/
    if (NumSegs >= MaxSegs) {
        MaxSegs += LIST_CHUNK;
        SegList = (SEgmentStruct *)csw_Realloc
                  (SegList, MaxSegs * sizeof(SEgmentStruct));
        if (!SegList) {
            return -1;
        }
        memset (SegList + NumSegs, 0,
                (MaxSegs - NumSegs) * sizeof(SEgmentStruct));
    }

/*
 * Make a new segment from middle to opposite.
 */
    oldseg = SegList + ioldseg;
    newseg = SegList + NumSegs;

    newseg->node1 = middle_node;
    newseg->node2 = opposite_node;
    newseg->discarded = (char)0;
    newseg->setid = oldseg->setid;
    newseg->bbid = oldseg->bbid;
    newseg->compid = oldseg->compid;

/*
 * Add the new segment to the node segment lists.
 */
    node = NodeList + middle_node;
    list = node->seglist;
    nlist = node->nseg;
    if (nlist > MAXSEG - 1) {
        return -1;
    }
    list[nlist] = NumSegs;
    node->nseg++;

    node = NodeList + opposite_node;
    list = node->seglist;
    nlist = node->nseg;
    if (nlist > MAXSEG - 1) {
        return -1;
    }
    list[nlist] = NumSegs;
    node->nseg++;

    NumSegs++;

    return 1;

}



/*
  ****************************************************************************

                         P o i n t O n L i n e

  ****************************************************************************

    Return 1 if point p,q is on the line segment.  Return 0 otherwise.

*/

int CSWPolyGraph::PointOnLine (int p, int q, int x1, int y1, int x2, int y2)
{
    double         slope, dx, dy;
    int            ty;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (x1 == x2  &&  y1 == y2) return 0;

    if (x1 == x2) {
        if (p != x1) return 0;
        if (y2 > y1) {
            if (q > y1  &&  q < y2) {
                return 1;
            }
            else {
                return 0;
            }
        }
        else {
            if (q > y2  &&  q < y1) {
                return 1;
            }
            else {
                return 0;
            }
        }
    }

    if (y1 == y2) {
        if (q != y1) return 0;
        if (x2 > x1) {
            if (p > x1  &&  p < x2) {
                return 1;
            }
            else {
                return 0;
            }
        }
        else {
            if (p > x2  &&  p < x1) {
                return 1;
            }
            else {
                return 0;
            }
        }
    }

    slope = (double)(y2-y1) / (double)(x2-x1);
    dx = (double)(p-x1);
    dy = slope * dx;
    ty = y1 + (int)dy;
    ty -= q;
    if (ty < 0) ty = -ty;
    if (ty > 2) return 0;

    if (x2 > x1) {
        if (p > x1  &&  p < x2) {
            return 1;
        }
        else {
            return 0;
        }
    }

    if (p > x2  &&  p < x1) {
        return 1;
    }

    return 0;

}  /*  end of private PointOnLine function  */








int CSWPolyGraph::praw (void)

{

    rawxy (Raw1.x1, Raw1.y1, Raw1.x2, Raw1.y2, Raw1.nvec);
    printf ("\n");

    rawxy (Raw2.x1, Raw2.y1, Raw2.x2, Raw2.y2, Raw2.nvec);
    printf ("\n");

    return 1;

}




/*
  ****************************************************************************

                     I d e n t i c a l I n p u t

  ****************************************************************************

    Return 1 if the two polygon sets are essentially identical or zero otherwise.

*/

int CSWPolyGraph::IdenticalInput (double *xp1, double *yp1, int np1, int *nc1, int *nv1,
                           double *xp2, double *yp2, int np2, int *nc2, int *nv2)
{
    int              istat, i, n, np;
    double           tiny, x1, y1, x2, y2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (np1 != np2) {
        return 0;
    }

    n = 0;
    for (i=0; i<np1; i++) {
        if (nc1[i] != nc2[i]) {
            return 0;
        }
        n += nc1[i];
    }

    np = 0;
    for (i=0; i<n; i++) {
        if (nv1[i] != nv2[i]) {
            return 0;
        }
        np += nv1[i];
    }

    x1 = 1.e30;
    y1 = 1.e30;
    x2 = -1.e30;
    y2 = -1.e30;

    for (i=0; i<np; i++) {
        if (xp1[i] < x1) x1 = xp1[i];
        if (yp1[i] < y1) y1 = yp1[i];
        if (xp2[i] > x2) x2 = xp2[i];
        if (yp2[i] > y2) y2 = yp2[i];
    }

    tiny = (x2 - x1 + y2 - y1) / IDENTICAL_GRAZE;

    for (i=0; i<np; i++) {
        istat = csw_NearEqual ((CSW_F)xp1[i], (CSW_F)xp2[i], (CSW_F)tiny);
        if (istat == 0) return 0;
        istat = csw_NearEqual ((CSW_F)yp1[i], (CSW_F)yp2[i], (CSW_F)tiny);
        if (istat == 0) return 0;
    }

    return 1;

}  /*  end of private IdenticalInput function  */





/*
  ****************************************************************************

                         C o p y P o l y g o n s

  ****************************************************************************

    Copy points from one polygon array to another.

*/

int CSWPolyGraph::CopyPolygons (double *xp1, double *yp1, int np1, int *nc1, int *nv1,
                         double *xp2, double *yp2, int *np2, int *nc2, int *nv2)
{
    int              i, n, np;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    *np2 = np1;

    n = 0;
    for (i=0; i<np1; i++) {
        nc2[i] = nc1[i];
        n++;
    }

    np = 0;
    for (i=0; i<n; i++) {
        nv2[i] = nv1[i];
        np += nv1[i];
    }

    memcpy (xp2, xp1, np * sizeof(double));
    memcpy (yp2, yp1, np * sizeof(double));

    return 1;

}  /*  end of private CopyPolygons function  */




/*
 ******************************************************************************

                 p l y _ u n i o n _ c o m p o n e n t s

 ******************************************************************************

  Union all the specified polygons into a simplified polygon set where
  no edges cross each other.
*/

int CSWPolyGraph::ply_union_components (double *xin, double *yin, void **tagin,
                          int numin, int *compin, int *nptsin,
                          double *xout, double *yout, void **tagout,
                          int *numout, int *compout, int *nptsout,
                          int npmax, int ncmax)
{
    double       *xw1, *yw1, *xw2, *yw2;
    int          *iw1, *iw2, *iw3, *iw4, *ncpts;
    void         **tw1, **tw2;
    int          istat, i, j, n, n1, n2, nt, nt2;
    double       *xt1, *yt1, *xt2, *yt2, *xt3, *yt3;
    int          *it1, *it2, *it3, *ic1, *ic2, *ic3;
    void         **tt1, **tt2, **tt3;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    Check for obvious errors.
*/
    if (xin == NULL  ||  yin == NULL  ||
        numin < 1  ||  compin == NULL  ||
        nptsin == NULL) {
        printf ("Bad parameters for ply_union_components\n");
        assert (0);
        return -1;
    }
    if (xout == NULL  ||  yout == NULL  ||
        numout == NULL  ||  compout == NULL  ||
        nptsout == NULL  ||  npmax < 1  ||  ncmax < 1) {
        printf ("Bad parameters for ply_union_components\n");
        assert (0);
        return -1;
    }
    n = 0;
    n1 = 0;
    n2 = 0;
    for (i=0; i<numin; i++) {
        n1 += compin[i];
        for (j=0; j<compin[i]; j++) {
            n2 += nptsin[n];
            n++;
        }
    }
    if (ncmax < n1  ||  npmax < n2) {
        printf ("Output sizes too small for ply_union_components\n");
        assert (0);
        return -1;
    }

/*
    If there is only a single polygon in the input,
    just copy it to the output and return.
*/
    n = n2;
    if (numin < 2) {
        memcpy (xout, xin, n * sizeof(double));
        memcpy (yout, yin, n * sizeof(double));
        if (tagout != NULL  &&  tagin != NULL) {
            memcpy (tagout, tagin, n * sizeof(void *));
        }
        memcpy (nptsout, nptsin, compin[0] * sizeof(int));
        compout[0] = compin[0];
        *numout = 1;
        return 1;
    }

/*
    Allocate work space memory
*/
    xw1 = (double *)csw_Malloc (npmax * 4 * sizeof(double));
    if (xw1 == NULL) {
        return -1;
    }
    yw1 = xw1 + npmax;
    xw2 = yw1 + npmax;
    yw2 = xw2 + npmax;

    iw1 = (int *)csw_Malloc (ncmax * 5 * sizeof(int));
    if (iw1 == NULL) {
        csw_Free (xw1);
        return -1;
    }
    iw2 = iw1 + ncmax;
    iw3 = iw2 + ncmax;
    iw4 = iw3 + ncmax;
    ncpts = iw4 + ncmax;

    tw1 = (void **)csw_Calloc (npmax * 2 * sizeof(void*));
    if (tw1 == NULL) {
        csw_Free (xw1);
        csw_Free (iw1);
        return -1;
    }
    tw2 = tw1 + npmax;

/*
    Set up points per polygon list.
*/
    n = 0;
    for (i=0; i<numin; i++) {
        ncpts[i] = 0;
        for (j=0; j<compin[i]; j++) {
            ncpts[i] += nptsin[n];
            n++;
        }
    }

/*
    The following block of code is to make lint happy.
*/
    xt3 = xin;
    yt3 = yin;
    tt3 = tagin;
    ic3 = compin;
    it3 = nptsin;
    nt2 = numin;

/*
    Union the first two polygons, then union the results
    with subsequent polygons.  The xt1 set of pointers
    will always point to the subsequent input polygon.
    The xt2 set of pointers will point to either the
    first input polygon or to the results from the
    previous union.  The xt3 set is for the output
    of the union.  It will alternate between xw1 and xw2
    because of the need for the previous results to
    be used in the next union operation.
*/
    xt2 = xin;
    yt2 = yin;
    tt2 = tagin;
    ic2 = compin;
    it2 = nptsin;
    n = 0;
    nt = 1;
    n1 = ncpts[0];
    n2 = compin[0];

    for (i=1; i<numin; i++) {

        xt1 = xin + n1;
        yt1 = yin + n1;
        tt1 = tagin + n1;
        if (tagin == NULL) {
            tt1 = NULL;
        }
        ic1 = compin + i;
        it1 = nptsin + n2;

        if (i % 2 == 1) {
            xt3 = xw1;
            yt3 = yw1;
            tt3 = tw1;
            ic3 = iw1;
            it3 = iw2;
        }
        else {
            xt3 = xw2;
            yt3 = yw2;
            tt3 = tw2;
            ic3 = iw3;
            it3 = iw4;
        }

        istat = ply_boolean (
            xt1, yt1, tt1, 1, ic1, it1,
            xt2, yt2, tt2, nt, ic2, it2,
            PLY_UNION,
            xt3, yt3, tt3, &nt2, ic3, it3,
            npmax, ncmax);

        if (istat == -1) {
            csw_Free (xw1);
            csw_Free (iw1);
            csw_Free (tw1);
            return -1;
        }

        if (nt2 < 1) {
            csw_Free (xw1);
            csw_Free (iw1);
            csw_Free (tw1);
            return -1;
        }

    /*
        Use previous output for second input in next union.
    */
        xt2 = xt3;
        yt2 = yt3;
        tt2 = tt3;
        ic2 = ic3;
        it2 = it3;
        nt = nt2;

        n1 += ncpts[i];
        n2 += compin[i];

    }

/*
    Copy the work output to the specified output and return.
*/
    n = 0;
    n1 = 0;
    for (i=0; i<nt2; i++) {
        compout[i] = ic3[i];
        for (j=0; j<ic3[i]; j++) {
            n1 += it3[n];
            nptsout[n] = it3[n];
            n++;
        }
    }
    memcpy (xout, xt3, n1 * sizeof(double));
    memcpy (yout, yt3, n1 * sizeof(double));
    if (tagout != NULL) {
        memcpy (tagout, tt3, n1 * sizeof(void*));
    }
    *numout = nt2;

    csw_Free (xw1);
    csw_Free (iw1);
    csw_Free (tw1);

    return 1;

}  /* end of ply_union_components function */



/*
 *************************************************************************

                    S e t B u g L o c a t i o n

 *************************************************************************

  Record up to 10 closing component bug locations to feed back to the user.

*/

int CSWPolyGraph::SetBugLocation (int ix, int iy)
{
    if (Nbug >= 10) return 1;

    BugX[Nbug] = (ix + IntegerRange) * TestScale + TestXmin;
    BugY[Nbug] = (iy + IntegerRange) * TestScale + TestYmin;
    Nbug++;

    return 1;

}  /* end of private SetBugLocation function */




/*
 ***************************************************************************

          p l y _ g e t _ b u g _ l o c a t i o n s

 ***************************************************************************

*/

int CSWPolyGraph::ply_get_bug_locations (double *bx, double *by, int *nbug, int maxbug)
{
    int              n, i;

    n = Nbug;
    if (n > maxbug) n = maxbug;

    for (i=0; i<n; i++) {
        bx[i] = BugX[i];
        by[i] = BugY[i];
    }

    *nbug = n;

    return 1;

}  /* end of function ply_get_bug_locations */



/*
  ****************************************************************************

                        F r a g m e n t G r a p h s

  ****************************************************************************

    All segments that connect to at least two other segments are selected
    for this process.  The idea is to build polygons out of complex
    intersecting polygons and lines.
*/

int CSWPolyGraph::FragmentGraphs (void)
{
    int               i, iseg, istat;
    SEgmentStruct     *seg;
    PLY_NOdeStruct        *node1, *node2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    CompressSegNodeLists ();

/*
 * Discard any segments that have a hanging node.  Once the
 * segment is discarded, march back along the topology until
 * a node with more than 2 segments is found, discarding as
 * we march back.
 */
    for (i=0; i<NumSegs; i++) {
        seg = SegList + i;
        if (seg->discarded == 1) {
            continue;
        }

        node1 = NodeList + seg->node1;
        node2 = NodeList + seg->node2;

        iseg = i;
        while (node1->nseg < 2  ||  node2->nseg < 2) {
            seg = SegList + iseg;
            seg->discarded = 1;
            RemoveSegmentFromNode (iseg, node1);
            RemoveSegmentFromNode (iseg, node2);
            if (node1->nseg > 1  ||  node2->nseg > 1) {
                break;
            }
            if (node1->nseg > 0) {
                iseg = node1->seglist[0];
            }
            else if (node2->nseg > 0) {
                iseg = node2->seglist[0];
            }
            else {
                break;
            }
        }
    }

    istat = BuildPolygonComponentsForFragments ();

    return istat;

}  /*  end of private FragmentGraphs function  */




/*
 ***************************************************************************

        p l y _ b u i l d _ p o l y g o n s _ f r o m _ l i n e s

 ***************************************************************************

  Separate the first line from the remaining lines and call the ply_boolean
  function with the operation set to PLY_FRAGMENT.

*/

int CSWPolyGraph::ply_build_polygons_from_lines (double *xline, double *yline,
                                   int nline, int *line_pts,
                                   double *xpout, double *ypout,
                                   int *npolyout, int *nptsout,
                                   int maxpts, int maxpoly)
{
    int            i, nc1, *nc2, *nc3, istat;
    double         *xp2, *yp2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (xline == NULL  ||  yline == NULL  ||  line_pts == NULL  ||
        xpout == NULL  ||  ypout == NULL  ||
        npolyout == NULL  ||  nptsout == NULL)
    {
        return -1;
    }

    if (nline < 2) {
        return -1;
    }

    if (maxpts < 1  ||  maxpoly < 1) {
        return -1;
    }

    nc1 = 1;
    nc2 =  (int *)csw_Malloc (2 * maxpoly * sizeof(int));
    if (nc2 == NULL) {
        return -1;
    }
    nc3 = nc2 + maxpoly;

    for (i=0; i<nline; i++) {
        nc2[i] = 1;
    }

    nc3 = nc2 + maxpoly;

    xp2 = xline + line_pts[0];
    yp2 = yline + line_pts[0];

    istat = ply_boolean (xline, yline, NULL,
                         1, &nc1, line_pts,
                         xp2, yp2, NULL,
                         nline-1, nc2, line_pts+1,
                         PLY_FRAGMENT,
                         xpout, ypout, NULL,
                         npolyout, nc3, nptsout,
                         maxpts, maxpoly);
    csw_Free (nc2);

    return istat;
}



/*
 ***************************************************************************

   p l y _ b u i l d _ p o l y g o n s _ f r o m _ t a g g e d _ l i n e s

 ***************************************************************************

  Separate the first line from the remaining lines and call the ply_boolean
  function with the operation set to PLY_FRAGMENT.

*/

int CSWPolyGraph::ply_build_polygons_from_tagged_lines
                                  (double *xline, double *yline, void **tags,
                                   int nline, int *line_pts,
                                   double *xpout, double *ypout, void **tagout,
                                   int *npolyout, int *nptsout,
                                   int maxpts, int maxpoly)
{
    int            i, nc1, *nc2, *nc3, istat;
    double         *xp2, *yp2;
    void           **tag2;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (xline == NULL  ||  yline == NULL  ||  line_pts == NULL  ||
        xpout == NULL  ||  ypout == NULL  ||
        npolyout == NULL  ||  nptsout == NULL)
    {
        return -1;
    }

    if (nline < 2) {
        return -1;
    }

    if (maxpts < 1  ||  maxpoly < 1) {
        return -1;
    }

    nc1 = 1;
    nc2 =  (int *)csw_Malloc (2 * maxpoly * sizeof(int));
    if (nc2 == NULL) {
        return -1;
    }
    nc3 = nc2 + maxpoly;

    for (i=0; i<nline; i++) {
        nc2[i] = 1;
    }

    nc3 = nc2 + maxpoly;

    xp2 = xline + line_pts[0];
    yp2 = yline + line_pts[0];
    tag2 = tags + line_pts[0];

    istat = ply_boolean (xline, yline, tags,
                         1, &nc1, line_pts,
                         xp2, yp2, tag2,
                         nline-1, nc2, line_pts+1,
                         PLY_FRAGMENT,
                         xpout, ypout, tagout,
                         npolyout, nc3, nptsout,
                         maxpts, maxpoly);
    csw_Free (nc2);

    return istat;
}




/*
  ****************************************************************************

      B u i l d P o l y g o n C o m p o n e n t s F o r F r a g m e n t s

  ****************************************************************************

  When connecting segments for fragments, there will be many cases where
  a node has more than 2 "good" segments attached to it.  The choice of
  which segment to use must pass some self intersection (or lack of self
  intersection) tests.  Also, we should loop from node to node, not from
  segment to segment.  We first trace the polygon components that stop
  and start on nodes with more than 2 segments.  Once these are done, there
  may be additinal components from lines that exactly touch on endpoints.

*/

int CSWPolyGraph::BuildPolygonComponentsForFragments (void)
{
    int            istat, iseg, nloops, npts,
                   *looplist, nseg, ixmin, iymin, ixmax, iymax,
                   i, j, iido, jjdo, ido, n1, n2, success;
    int            *ix, *iy;
    void           **tagcomp;
    int            first_exit, nsegmin, count3;

    SEgmentStruct  *sptr;
    PLY_NOdeStruct     *nptr, *npbase;
    COmponentStruct   *cptr;
    LOopStruct     *lptr;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    FreeLoopList ();
    csw_Free (LoopNodes);
    csw_Free (LoopSegments);
    LoopNodes = NULL;
    LoopSegments = NULL;
    NumLoopNodes = 0;
    NumLoopSegments = 0;
    MaxLoopNodes = 0;
    MaxLoopSegments = 0;

    for (i=0; i<NumSegs; i++) {
        sptr = SegList + i;
        sptr->dir12 = 0;
        sptr->dir21 = 0;
    }

    count3 = 0;
    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        memset (nptr->flags, 0, MAXSEG * sizeof(char));
        if (nptr->nseg > 2) count3++;
    }
    nsegmin = 3;
    if (count3 == 0) {
        nsegmin = 2;
    }

    for (iido=0; iido<NumNodes; iido++) {

        if (nsegmin == 2  &&  NumLoopList > 0) {
            break;
        }

        npbase = NodeList + iido;
        if (npbase->nseg < nsegmin) {
            continue;
        }

        write_current_output ();

        for (jjdo=0; jjdo<npbase->nseg; jjdo++) {

            if (npbase->flags[jjdo] == 1) {
                continue;
            }

            ido = npbase->seglist[jjdo];
            npbase->flags[jjdo] = 1;

            sptr = SegList + ido;
            sptr->tmpdir = 0;
            if (sptr->discarded) continue;
            if (sptr->nused > 1) continue;

            NumLoopNodes = 0;
            NumLoopSegments = 0;

            istat = AddLoopSegment (ido);
            if (istat == -1) {
                return -1;
            }
            n1 = sptr->node1;
            n2 = sptr->node2;
            sptr->tmpdir = 1;
            if (sptr->node1 != iido) {
                n1 = sptr->node2;
                n2 = sptr->node1;
                sptr->tmpdir = -1;
            }

            istat = AddLoopNode (n1);
            if (istat == -1) {
                return -1;
            }
            istat = AddLoopNode (n2);
            if (istat == -1) {
                return -1;
            }

            nptr = NodeList + n2;
            iseg = ido;

            first_exit = 0;
            success = 0;
            for (;;) {
                istat = ChooseExitSegmentForFragments
                        (&first_exit, nptr, iseg, nptr->seglist, nptr->nseg);
                if (istat == -1) {
                    break;
                }
                if (istat == -999) {
                    success = 1;
                    break;
                }
                iseg = nptr->seglist[istat];
                istat = AddLoopSegment (iseg);
                if (istat == -1) {
                    return -1;
                }

             /*
              * Add the next node and record the temporary
              * direction in which the segment is traversed.
              * If from node1 to node2, tmpdir is 1.  If from
              * node2 to node1, tmpdir is -1.
              */
                sptr = SegList + iseg;
                n1 = sptr->node1;
                sptr->tmpdir = -1;
                if (n1 == nptr - NodeList) {
                    n1 = sptr->node2;
                    sptr->tmpdir = 1;
                }
                istat = AddLoopNode (n1);
                if (istat == -1) {
                    return -1;
                }
                nptr = NodeList + n1;
            }

            if (success == 1) {
                istat = AddToLoopList ();
                if (istat == -1) {
                   return -1;
                }
                if (istat == 1) {
                    iseg = LoopSegments[NumLoopSegments-1];
                    for (j=0; j<npbase->nseg; j++) {
                        if (npbase->seglist[j] == iseg) {
                            npbase->flags[j] = 1;
                            break;
                        }
                    }
                }
            }

        }  /* end of loop thru segments attached to the node */

    }  /* end of outer loop for all nodes */


/*
 * debug print stuff
    fptr = fopen ("loop_list.dat", "wb+");
    if (fptr) {
        for (i=0; i<NumLoopList; i++) {
            lptr = LoopList + i;
            npts = lptr->nloop-1;
            for (j=0; j<npts; j++) {
                sprintf (line, "%d ", lptr->segs[j]);
                fputs (line, fptr);
                if (j > 0  &&  j % 16 == 0  &&  j != npts-1) {
                    fputs ("\n", fptr);
                }
            }
            fputs ("\n", fptr);
        }

        fclose (fptr);
    }
 */

/*
 * Build components from the loop list.
 */
    nloops = NumLoopList;
    if (nloops > Pmaxcomp) {
        nloops = Pmaxcomp;
    }

    for (i=0; i<nloops; i++) {
        cptr = CompList + i;
        lptr = LoopList + i;
        npts = lptr->nloop;
        ix = (int *)csw_Malloc (2 * npts * sizeof(int));
        if (ix == NULL) {
            FreeLoopList ();
            FreeAllMem ();
            return -1;
        }
        iy = ix + npts;

        tagcomp = (void **)csw_Calloc (npts * sizeof(void *));
        if (!tagcomp) {
            csw_Free (ix);
            return -1;
        }

        ixmin = 2000000000;
        iymin = 2000000000;
        ixmax = -2000000000;
        iymax = -2000000000;
        for (j=0; j<npts; j++) {
            looplist = lptr->nodes;
            nseg = looplist[j];
            nptr = NodeList + nseg;
            ix[j] = nptr->x;
            iy[j] = nptr->y;
            tagcomp[j] = nptr->tag;
            if (ix[j] < ixmin) ixmin = ix[j];
            if (iy[j] < iymin) iymin = iy[j];
            if (ix[j] > ixmax) ixmax = ix[j];
            if (iy[j] > iymax) iymax = iy[j];
        }
        cptr->x = ix;
        cptr->y = iy;
        cptr->tag = tagcomp;
        cptr->npts = npts;
        cptr->holeflag = 0;
        cptr->xmin = ixmin;
        cptr->ymin = iymin;
        cptr->xmax = ixmax;
        cptr->ymax = iymax;
    }
    Ncomp = nloops;

    type_current_loop ();

    return 1;

}  /*  end of private BuildPolygonComponentsForFragments function  */





/*
  ****************************************************************************

        C h o o s e E x i t S e g m e n t F o r F r a g m e n t s

  ****************************************************************************

  Choose the best exit segment from a node.  The best is determined
  by checking available segments as follows:

    1) If there is only one valid segment, choose it.

    2) If any valid segment closes the current loop, choose it.

    3) Choose the smallest angle in the same handedness sense as the
       first angle chosen for the first junction encountered in the
       loop.

    Segments are invalid because:

    1) The segment is marked as deleted.

    2) The segment has been used twice already.

    3) The segment has already been used in the desired direction,
       and the entry edge to the junction has already been used once
       in either direction.

  A segment is invalid if it has been used in two previous polygons,
  if it has been discarded by the pre filtering, or if it has already
  been used in the direction that it currently would be used in.

  The index within the specified list of the chosen segment is returned.

*/

int CSWPolyGraph::ChooseExitSegmentForFragments (int *first_exit, PLY_NOdeStruct *node,
                                          int sentry, int *list, int nlist)

{
    int            iseg, iseg2, n, n1, nseg;
    int            i, j, node_num, nlast;
    double         xlast, ylast, xnow, ynow, xnext, ynext;
    double         dx1, dy1, ang1;
    double         dx2, dy2, ang2;
    double         dang, mindang, maxdang;
    SEgmentStruct  *sptr, *splast;
    int            jclose, iclose;
    double         ax, bx, ay, by, sinang;
    double         amag, bmag, minsinang, cproduct;
    int            entry_used;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    node_num = node - NodeList;

/*
 * If the next point of any segment closes with any point
 * already in the loop, choose that segment.  All of the
 * segments connected to the current node need to be checked
 * for closure, and the segment which closes the closest to
 * the current node is used.
 */
    j = -1;
    jclose = -1;
    iclose = -1;
    j = j;
    iclose = iclose;
    jclose = jclose;

    entry_used = 0;
/*
    sptr = SegList + sentry;
    if (sptr->nused > 0) {
        entry_used = 1;
    }
*/

    for (i=0; i<nlist; i++) {

        nseg = list[i];

        if (sentry == nseg) {
            continue;
        }
        sptr = SegList + nseg;
        if (sptr->discarded) {
            continue;
        }
        if (sptr->nused > 1) {
            continue;
        }

      /*
       * Get the next node and skip this segment if it has
       * already been used in this direction.
       */
        n1 = sptr->node1;
        if (n1 == node_num) {
            if (entry_used) {
                if (sptr->dir12 == 1) continue;
            }
            n1 = sptr->node2;
        }
        else {
            if (entry_used) {
                if (sptr->dir21 == 1) continue;
            }
        }

        if (n1 == LoopNodes[0]) {
            LoopSegments[NumLoopSegments] = nseg;
            LoopNodes[NumLoopNodes] = n1;
            NumLoopSegments++;
            NumLoopNodes++;
            return -999;
        }
    }

/*
 * Debug code to type the current loop.
 */
    if (nlist > 2) {
        type_current_loop ();
    }

/*
 * Find the best exit out of the exits that are valid.
 */
    iseg = -1;
    n = 0;
    for (i=0; i<nlist; i++) {
        nseg = list[i];
        if (sentry == nseg) {
            continue;
        }
        sptr = SegList + nseg;
        if (sptr->discarded) {
            continue;
        }
        if (sptr->nused > 1) {
            continue;
        }

      /*
       * Get the next node and skip this segment if it has
       * already been used in this direction.
       */
        n1 = sptr->node1;
        if (n1 == node_num) {
            if (entry_used) {
                if (sptr->dir12 == 1) continue;
            }
            n1 = sptr->node2;
        }
        else {
            if (entry_used) {
                if (sptr->dir21 == 1) continue;
            }
        }

        iseg = i;
        n++;
    }

/*
 * If there are no edges to choose from its an error.
 */
    if (n < 1) {
        return -1;
    }

/*
 * Get coordinates for previous and current nodes.
 */
    splast = SegList + sentry;
    nlast = splast->node1;
    if (nlast == node_num) {
        nlast = splast->node2;
    }

    xlast = (double)NodeList[nlast].x;
    ylast = (double)NodeList[nlast].y;

    xnow = (double)node->x;
    ynow = (double)node->y;

    dx1 = xlast - xnow;
    dy1 = ylast - ynow;
    ang1 = atan2 (dy1, dx1);
    ax = cos(ang1);
    ay = sin(ang1);

/*
 * If more than one candidate segment still exists, choose the
 * segment that makes the smallest angle with the entry segment.
 */
    iseg = -1;
    iseg2 = -1;
    mindang = 1.e30;
    maxdang = -1.e30;
    minsinang = 0.0;
    xnext = 1.e30;
    ynext = 1.e30;
    for (i=0; i<nlist; i++) {

        nseg = list[i];

        if (sentry == nseg) {
            continue;
        }
        sptr = SegList + nseg;
        if (sptr->discarded) {
            continue;
        }
        if (sptr->nused > 1) {
            continue;
        }

      /*
       * Get the next node and skip this segment if it has
       * already been used in this direction.
       */
        n1 = sptr->node1;
        if (n1 == node_num) {
            if (entry_used) {
                if (sptr->dir12 == 1) continue;
            }
            n1 = sptr->node2;
        }
        else {
            if (entry_used) {
                if (sptr->dir21 == 1) continue;
            }
        }

        xnext = (double)NodeList[n1].x;
        ynext = (double)NodeList[n1].y;
        dx2 = xnext - xnow;
        dy2 = ynext - ynow;
        ang2 = atan2 (dy2, dx2);
        bx = cos(ang2);
        by = sin(ang2);

        cproduct = ax * by - ay * bx;
        amag = ax * ax + ay * ay;
        amag = sqrt (amag);
        bmag = bx * bx + by * by;
        bmag = sqrt (bmag);
        if (amag * bmag < 0.00000001) {
            sinang = 0.0;
        }
        else {
            sinang = cproduct / (amag * bmag);
        }

        dang = ang1 - ang2;
        while (dang < 0.0) {
            dang += TWOPI;
        }
        if (dang > PI) {
            dang = TWOPI - dang;
        }

        if (*first_exit == 0) {
            if (dang < mindang) {
                mindang = dang;
                minsinang = sinang;
                iseg = i;
            }
        }
        else {

      /*
       * Record the segment with the minimum angle if there is
       * a segment with the same handedness as the initial segment
       * (iseg).  Also record the maximum angle of opposite
       * handedness (iseg2).
       */
            if (*first_exit * cproduct < 0.0) {
                if (dang > maxdang) {
                    maxdang = dang;
                    iseg2 = i;
                }
            }
            else {
                if (dang < mindang) {
                    mindang = dang;
                    iseg = i;
                }
            }
        }

    }

/*
 * If there are no segments with the correct handedness, use the
 * largest angle of opposite handedness.
 */
    if (iseg == -1) {
        iseg = iseg2;
    }

    if (xnext > 1.e20  ||  ynext > 1.e20) {
        return -1;
    }

    if (iseg == -1) {
        return -1;
    }

    if (nlist > 2  &&  *first_exit == 0) {
        if (minsinang < 0.0) {
            *first_exit = -1;
        }
        else {
            *first_exit = 1;
        }
    }

    return iseg;

}  /*  end of private ChooseExitSegmentForFragments function  */


/*
 ***************************************************************************

                        C l o s e F r a g L o o p

 ***************************************************************************


int CSWPolyGraph::CloseFragLoop (int worknum, int segnum)

{
    int              i, n, nseg;
    SEgmentStruct    *sptr;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (worknum > 0) {
        n = 0;
        for (i=worknum; i<NumLoopNodes; i++) {
            LoopNodes[n] = LoopNodes[i];
            n++;
        }
        NumLoopNodes = n;
        n = 0;
        for (i=0; i<NumLoopSegments; i++) {
            if (i < worknum) {
                nseg = LoopSegments[i];
                SegList[nseg].tmpdir = 0;
            }
            else {
                LoopSegments[n] = LoopSegments[i];
                n++;
            }
        }
        NumLoopSegments = n;
    }

    LoopSegments[NumLoopSegments] = segnum;
    NumLoopSegments++;
    sptr = SegList + segnum;
    n = sptr->node1;
    sptr->tmpdir = -1;
    if (n == LoopNodes[NumLoopNodes-1]) {
        n = sptr->node2;
        sptr->tmpdir = 1;
    }
    LoopNodes[NumLoopNodes] = n;
    NumLoopNodes++;

    return 1;
}
*/




/*
 ****************************************************************************

                        A d d T o L o o p L i s t

 ****************************************************************************

*/

int CSWPolyGraph::AddToLoopList (void)
{
    int            i, j, k, n, match, jmatch, nseg;
    int            n1, found, istat;
    int            *nlist, *slist;
    LOopStruct     *lptr;
    SEgmentStruct  *sptr;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
 * Check obvious errors.
 */
    if (LoopSegments == NULL  ||  LoopNodes == NULL) {
        return 0;
    }
    if (NumLoopNodes < 4  ||  NumLoopSegments < 3) {
        return 0;
    }
    if (NumLoopNodes != NumLoopSegments+1) {
        return 0;
    }

/*
 * If the current loop is a super polygon, i.e. one which
 * has other segments internal to it, do not add it to the
 * output. The only possible super polygons are those which
 * trace boundaries of regions in the polygon set.  The
 * superflag member is set in this case.
 */
    istat = CheckForSuperPolygon ();
    if (istat == 1) {
        for (i=0; i<NumLoopSegments; i++) {
            nseg = LoopSegments[i];
            sptr = SegList + nseg;
            sptr->tmpdir = 0;
        }
        return 0;
    }

/*
 * If any segment in the current work loop has not
 * been used previously, the loop cannot be a duplicate.
 */
    match = 1;
    for (i=0; i<NumLoopSegments; i++) {
        nseg = LoopSegments[i];
        if (SegList[nseg].nused == 0) {
            match = 0;
            break;
        }
    }

/*
 * If the current working loop already exists do not
 * add it again.  The loop exists if the number of nodes
 * is the same as in a previous loop and all of the
 * segments in the candidate loop also exist in the previous
 * loop.
 */
    if (LoopList != NULL  &&  match == 1) {

        match = 0;
        for (i=0; i<NumLoopList; i++) {

            lptr = LoopList + i;
            slist = lptr->segs;
            n = lptr->nloop;
            if (n != NumLoopNodes) {
                continue;
            }

            jmatch = 1;
            for (j=0; j<n-1; j++) {
                found = 0;
                n1 = slist[j];
                for (k=0; k<NumLoopSegments; k++) {
                    if (n1 == LoopSegments[k]) {
                        found = 1;
                        break;
                    }
                }
                if (found == 0) {
                    jmatch = 0;
                    break;
                }
            }
            if (jmatch == 1) {
                match = 1;
                break;
            }
        }
    }

/*
 * If there is a match with a previous loop, reset the
 * tmpdir members of the candidate segments to zero
 * and then return.
 */
    if (match == 1) {
        for (i=0; i<NumLoopSegments; i++) {
            nseg = LoopSegments[i];
            SegList[nseg].tmpdir = 0;
        }
        return 0;
    }

/*
 * Grow the loop list if needed.
 */
    if (NumLoopList >= MaxLoopList  ||
        LoopList == NULL) {
        MaxLoopList += 100;
        LoopList = (LOopStruct *)csw_Realloc
                   (LoopList, MaxLoopList * sizeof(LOopStruct));
        memset (LoopList + NumLoopList, 0,
                (MaxLoopList - NumLoopList) * sizeof(LOopStruct));
    }
    if (LoopList == NULL) {
        return -1;
    }

/*
 * Allocate space for the current loops segments and nodes.
 */
    nlist = (int *)csw_Malloc (NumLoopNodes * sizeof(int));
    if (nlist == NULL) {
        FreeLoopList ();
        return -1;
    }
    slist = (int *)csw_Malloc (NumLoopSegments * sizeof(int));
    if (slist == NULL) {
        csw_Free (nlist);
        FreeLoopList ();
        return -1;
    }

/*
 * Increment the used count for each segment in the loop.
 * Also set the direction flag.
 */
    for (i=0; i<NumLoopSegments; i++) {

        nseg = LoopSegments[i];
        sptr = SegList + nseg;
        sptr->nused++;
        sptr->used_in_last_loop = 1;
        if (sptr->tmpdir == -1) {
            sptr->dir21 = 1;
        }
        else if (sptr->tmpdir == 1) {
            sptr->dir12 = 1;
        }
        sptr->tmpdir = 0;

    /*
     * Set the Input comp id flag for the setid and bbid combination
     * to 1.  This will be checked after all fragments
     * have been built.  Any closed input lines that have not
     * been used will be output as result polygons.
     */
        if (sptr->setid == 1) {
            if (sptr->compid >= 0  &&  sptr->compid < NumInputCompIDFlag1) {
                InputCompIDFlag1[sptr->compid] = 1;
            }
        }
        else {
            if (sptr->compid >= 0  &&  sptr->compid < NumInputCompIDFlag2) {
                InputCompIDFlag2[sptr->compid] = 1;
            }
        }

    }

/*
 * Copy the current loop nodes into the loop list
 */
    memcpy (nlist, LoopNodes, NumLoopNodes * sizeof(int));
    memcpy (slist, LoopSegments, NumLoopSegments * sizeof(int));

    LoopList[NumLoopList].nodes = nlist;
    LoopList[NumLoopList].segs = slist;
    LoopList[NumLoopList].nloop = NumLoopNodes;
    NumLoopList++;

    return 1;

}


/*
 *****************************************************************************

                            F r e e L o o p L i s t

 *****************************************************************************

*/

void CSWPolyGraph::FreeLoopList (void)
{
    int            i;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (LoopList == NULL) {
        return;
    }

    for (i=0; i<MaxLoopList; i++) {
        csw_Free (LoopList[i].nodes);
        LoopList[i].nodes = NULL;
        csw_Free (LoopList[i].segs);
        LoopList[i].segs = NULL;
    }

    csw_Free (LoopList);
    LoopList = NULL;

    NumLoopList = 0;
    MaxLoopList = 0;

    return;
}


/*
 ********************************************************************************

                         A d d L o o p N o d e

 ********************************************************************************

*/

int CSWPolyGraph::AddLoopNode (int nodenum)
{

    if (LoopNodes == NULL  ||  NumLoopNodes >= MaxLoopNodes - 3) {
        MaxLoopNodes += 100;
        LoopNodes = (int *)csw_Realloc (LoopNodes, MaxLoopNodes * sizeof(int));
    }
    if (LoopNodes == NULL) {
        return -1;
    }

    LoopNodes[NumLoopNodes] = nodenum;
    NumLoopNodes++;

    return (NumLoopNodes - 1);

}


/*
 ********************************************************************************

                       A d d L o o p S e g m e n t

 ********************************************************************************

*/

int CSWPolyGraph::AddLoopSegment (int segnum)
{

    if (LoopSegments == NULL  ||  NumLoopSegments >= MaxLoopSegments - 3) {
        MaxLoopSegments += 100;
        LoopSegments = (int *)csw_Realloc (LoopSegments, MaxLoopSegments * sizeof(int));
    }
    if (LoopSegments == NULL) {
        return -1;
    }

    LoopSegments[NumLoopSegments] = segnum;
    NumLoopSegments++;

    return (NumLoopSegments - 1);

}



/*
 ************************************************************************************

                     C h e c k F o r S u p e r P o l y g o n

 ************************************************************************************

  Check the current loop and see if any of the edges attached to nodes in the
  loop are actually inside the loop.  If that is so, return 1.  If not so,
  return zero.

*/

int CSWPolyGraph::CheckForSuperPolygon (void)
{
    int              i, j, k, n, good, istat;
    double           *xp, *yp, xt, yt;
    char             *seg_used;
    PLY_NOdeStruct       *np1, *np2;
    SEgmentStruct    *sptr;
    CSWPolyUtils     ply_utils_obj;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
 * If the current loop is a triangle, it cannot have
 * any other edges inside it.
 */
    if (NumLoopSegments < 4) {
        return 0;
    }

/*
 * Allocate space for the polygon and a list of which
 * segments are connected to the current loop.  If an
 * allocation fails, -1 is returned.
 */
    seg_used = (char *)csw_Calloc (NumSegs * sizeof(char));
    if (seg_used == NULL) {
        return -1;
    }
    xp = (double *)csw_Malloc (NumLoopNodes * 2 * sizeof(double));
    if (xp == NULL) {
        csw_Free (seg_used);
        return -1;
    }
    yp = xp + NumLoopNodes;

/*
 * Build the polygon and set the flags for the segments
 * attached to but not part of the polygon.
 */
    for (i=0; i<NumLoopNodes; i++) {
        n = LoopNodes[i];
        np1 = NodeList + n;
        xp[i] = np1->x;
        yp[i] = np1->y;
        for (j=0; j<np1->nseg; j++) {
            n = np1->seglist[j];
            good = 1;
            for (k=0; k<NumLoopSegments; k++) {
                if (n == LoopSegments[k]) {
                    good = 0;
                    break;
                }
            }
            if (good == 1) {
                seg_used[n] = 1;
            }
        }
    }

/*
 * Check if any of the flagged segments has its mid point
 * inside the polygon.  The grazing value is set to about
 * 1 part in 1 million given that the x and y coordinates
 * extend from -100 million to 100 million, aproximately.
 */
    ply_utils_obj.ply_setgraze (200.0);
    istat = -1;
    for (i=0; i<NumSegs; i++) {
        if (seg_used[i] == 0) continue;
        sptr = SegList + i;
        np1 = NodeList + sptr->node1;
        np2 = NodeList + sptr->node2;
        xt = (np1->x + np2->x) / 2.0;
        yt = (np1->y + np2->y) / 2.0;
        istat = ply_utils_obj.ply_point (xp, yp, NumLoopNodes,
                           xt, yt);
        if (istat == 1) {
            break;
        }
    }

    ply_utils_obj.ply_setgraze (0.0);

    csw_Free (xp);
    csw_Free (seg_used);

    if (istat == 1) {
        return 1;
    }

    return 0;

}



/*
 ***********************************************************************

               C o m p r e s s S e g N o d e L i s t s

 ***********************************************************************

*/

void CSWPolyGraph::CompressSegNodeLists (void)
{
    int               i, j, n1, n2, n, ns, nn, nlist;
    int               *slook, *nlook, *slist;
    char              *nflag;
    SEgmentStruct     *sptr;
    PLY_NOdeStruct        *nptr;
    int               do_write;
    char              fname[100];
    double            *x, *y, *z;
    int               *icomp, *ivec;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    do_write = csw_GetDoWrite ();
    if (do_write) {
        sprintf (fname, "precompress.seg");
        x = (double *)csw_Malloc (NumSegs * 6 * sizeof(double));
        if (x == NULL) {
            return;
        }
        y = x + NumSegs * 2;
        z = y + NumSegs * 2;
        icomp = (int *)csw_Malloc (2 * NumSegs * sizeof(int));
        if (icomp == NULL) {
            csw_Free (x);
            return;
        }
        ivec = icomp + NumSegs;
        n = 0;
        for (i=0; i<NumSegs; i++) {
            icomp[i] = 1;
            ivec[i] = 2;
            n1 = SegList[i].node1;
            n2 = SegList[i].node2;
            x[n] = NodeList[n1].xorig;
            y[n] = NodeList[n1].yorig;
            z[n] = 0.0;
            n++;
            x[n] = NodeList[n2].xorig;
            y[n] = NodeList[n2].yorig;
            z[n] = 0.0;
            n++;
        }
        WriteLines (x, y, z, NumSegs,
                        icomp, ivec, fname);
        csw_Free (x);
        csw_Free (icomp);
        x = y = z = NULL;
        icomp = ivec = NULL;
    }

    slook = (int *)csw_Malloc (NumSegs * sizeof(int));
    if (slook == NULL) {
        return;
    }
    nlook = (int *)csw_Malloc (NumNodes * sizeof(int));
    if (nlook == NULL) {
        csw_Free (slook);
        return;
    }
    nflag = (char *)csw_Calloc (NumNodes * sizeof(char));
    if (nflag == NULL) {
        csw_Free (slook);
        csw_Free (nlook);
        return;
    }

    n = 0;
    for (i=0; i<NumSegs; i++) {
        sptr = SegList + i;
        if (sptr->discarded == 1) {
            slook[i] = -1;
            continue;
        }
        slook[i] = n;
        nflag[sptr->node1] = 1;
        nflag[sptr->node2] = 1;
        if (i == n) {
            n++;
            continue;
        }
        memcpy (SegList+n, sptr, sizeof(SEgmentStruct));
        n++;
    }

    ns = n;

    n = 0;
    for (i=0; i<NumNodes; i++) {
        if (nflag[i] == 0) {
            nlook[i] = -1;
            continue;
        }
        nptr = NodeList + i;
        nlook[i] = n;
        if (i == n) {
            n++;
            continue;
        }

        memcpy (NodeList+n, nptr, sizeof(PLY_NOdeStruct));
        n++;
    }

    nn = n;

    for (i=0; i<ns; i++) {
        sptr = SegList + i;
        n1 = sptr->node1;
        n2 = sptr->node2;
        sptr->node1 = nlook[n1];
        sptr->node2 = nlook[n2];
    }

    for (i=0; i<nn; i++) {
        nptr = NodeList + i;
        slist = nptr->seglist;
        nlist = nptr->nseg;
        n = 0;
        for (j=0; j<nlist; j++) {
            n1 = slist[j];
            if (slook[n1] == -1) {
                continue;
            }
            slist[n] = slook[n1];
            n++;
        }
        nptr->nseg = n;
    }

    csw_Free (slook);
    csw_Free (nlook);
    csw_Free (nflag);

    NumSegs = ns;
    NumNodes = nn;

    if (do_write) {
        sprintf (fname, "postcompress.seg");
        x = (double *)csw_Malloc (NumSegs * 6 * sizeof(double));
        if (x == NULL) {
            return;
        }
        y = x + NumSegs * 2;
        z = y + NumSegs * 2;
        icomp = (int *)csw_Malloc (2 * NumSegs * sizeof(int));
        if (icomp == NULL) {
            csw_Free (x);
            return;
        }
        ivec = icomp + NumSegs;
        n = 0;
        for (i=0; i<NumSegs; i++) {
            icomp[i] = 1;
            ivec[i] = 2;
            n1 = SegList[i].node1;
            n2 = SegList[i].node2;
            x[n] = NodeList[n1].xorig;
            y[n] = NodeList[n1].yorig;
            z[n] = 0.0;
            n++;
            x[n] = NodeList[n2].xorig;
            y[n] = NodeList[n2].yorig;
            z[n] = 0.0;
            n++;
        }
        WriteLines (x, y, z, NumSegs,
                        icomp, ivec, fname);
        csw_Free (x);
        csw_Free (icomp);
        x = y = z = NULL;
        icomp = ivec = NULL;
    }

    return;

}




/*
 ******************************************************************************

                          p l y _ n e s t _ h o l e s

 ******************************************************************************

  Given a list of closed polygon components, return a list of polygons with
  the components properly nested.

*/

int CSWPolyGraph::ply_nest_holes (double *xpoly, double *ypoly,
                    int npoly, int *polypoints,
                    double *xpout, double *ypout,
                    int *npout, int *icout, int *ipout,
                    int max_points, int max_comps)
{
    int             i, j, n, ntotal, npts,
                    ix1, iy1, ix2, iy2;
    int             *ix, *iy;
    double          xmin, ymin, xmax, ymax,
                    dx, dy, tiny, scale;
    double          xt, yt;
    int             ixt, iyt;
    COmponentStruct *cptr;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
 * Check for obvious errors.
 */
    if (xpoly == NULL  ||  ypoly == NULL  ||
        npoly < 1  ||  polypoints == NULL  ||
        xpout == NULL  ||  ypout == NULL  ||
        npout == NULL  ||  icout == NULL  ||  ipout == NULL  ||
        max_points < 1  ||  max_comps < 1) {
        return -1;
    }

/*
 * Find the limits of the input components to use
 * for scaling to integer coordinates.
 */
    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;
    n = 0;
    for (i=0; i<npoly; i++) {
        npts = polypoints[i];
        for (j=0; j<npts; j++) {
            if (xpoly[n] < xmin) xmin = xpoly[n];
            if (ypoly[n] < ymin) ymin = ypoly[n];
            if (xpoly[n] > xmax) xmax = xpoly[n];
            if (ypoly[n] > ymax) ymax = ypoly[n];
            n++;
        }
    }

    ntotal = n;

    if (xmin > xmax ||  ntotal < 3) {
        return -1;
    }

    dx = xmax - xmin;
    dy = ymax - ymin;
    tiny = (dx + dy) / 200.0;
    xmin -= tiny;
    ymin -= tiny;
    xmax += tiny;
    ymax += tiny;

    scale = (xmax - xmin) / (double)IntegerRange2;

/*
 * Set the private class variables as needed.
 */
    Pxout = xpout;
    Pyout = ypout;
    Ptagout = NULL;
    Pnpout = npout;
    Pncout = icout;
    Pnvout = ipout;
    Pmaxcomp = max_comps;
    Pmaxnpts = max_points;

    Ncomp = npoly;

    TestScale = scale;
    TestXmin = xmin;
    TestYmin = ymin;

/*
 * Allocate a list of component structures.
 */
    CompList = (COmponentStruct *)csw_Calloc (npoly * sizeof(COmponentStruct));
    if (!CompList) {
        return -1;
    }

    n = 0;
    for (i=0; i<npoly; i++) {
        npts = polypoints[i];
        cptr = CompList + i;
        ix = (int *)csw_Malloc (2 * npts * sizeof(int));
        if (ix == NULL) {
            FreeAllMem ();
            return -1;
        }
        iy = ix + npts;
        ix1 = 1000000000;
        iy1 = ix1;
        ix2 = -1000000000;
        iy2 = ix2;
        for (j=0; j<npts; j++) {
            xt = xpoly[n];
            yt = ypoly[n];
            ixt = (int)((xt - xmin) / scale);
            ixt -= IntegerRange;
            iyt = (int)((yt - ymin) / scale);
            iyt -= IntegerRange;
            if (ixt < ix1) ix1 = ixt;
            if (iyt < iy1) iy1 = iyt;
            if (ixt > ix2) ix2 = ixt;
            if (iyt > iy2) iy2 = iyt;
            ix[j] = ixt;
            iy[j] = iyt;
            n++;
        }
        cptr->x = ix;
        cptr->y = iy;
        cptr->npts = npts;
        cptr->xmin = ix1;
        cptr->ymin = iy1;
        cptr->xmax = ix2;
        cptr->ymax = iy2;
    }

    NestHoles ();

    BuildOutputPolygons ();

    return 1;
}



/*----------------------------------------------------------*/

void CSWPolyGraph::write_current_output (void)
{
    int        i, j, do_write;
    LOopStruct *lp;
    FILE       *fptr;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    do_write = csw_GetDoWrite ();
    if (do_write) {
        fptr = fopen ("currentloops.xyz", "w");
        if (fptr == NULL) {
            return;
        }
        fprintf (fptr, "%d\n", NumLoopList);
        for (i=0; i<NumLoopList; i++) {
            lp = LoopList + i;
            fprintf (fptr, "%d\n", lp->nloop);
            for (j=0; j<lp->nloop; j++) {
                fprintf (fptr, "%.3f %.3f %d\n",
                         NodeList[lp->nodes[j]].xorig,
                         NodeList[lp->nodes[j]].yorig,
                         lp->nodes[j]);
            }
        }
        fclose (fptr);
    }

    return;

}

void CSWPolyGraph::type_current_loop (void)
{
    int          i, j, do_write;
    int          nn, *nodes;
    FILE         *fptr;

    do_write = csw_GetDoWrite ();
    if (do_write) {
        fprintf (stdout, "\n%d\n", NumLoopNodes);
        for (j=0; j<NumLoopNodes; j++) {
            fprintf (stdout, "%.3f %.3f %d\n",
                     NodeList[LoopNodes[j]].xorig,
                     NodeList[LoopNodes[j]].yorig,
                     LoopNodes[j]);
        }
        fflush (stdout);

        fptr = fopen ("looplist.xy", "wb");
        if (fptr) {
            for (i=0; i<NumLoopList; i++) {
                nodes = LoopList[i].nodes;
                nn = LoopList[i].nloop;
                for (j=0; j<nn; j++) {
                    fprintf (fptr, "%.2f %.2f\n",
                             NodeList[nodes[j]].xorig,
                             NodeList[nodes[j]].yorig
                    );
                }
                fprintf (fptr, "\n");
            }
            fclose (fptr);
        }
    }
}

int CSWPolyGraph::SamePoint (double x1, double y1, double x2, double y2)
{
    double           dx, dy, dist;

    dx = x2 - x1;
    dy = y2 - y1;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (dist <= XYTiny) {
        return 1;
    }
    return 0;
}

/*
  ****************************************************************************

              A d d U n t o u c h e d I n p u t P o l y g o n

  ****************************************************************************

  Add any input polygons that were not used at all in the boolean operation
  to the output polygon set.  This is currently only used for the fragment
  operation.
*/

int CSWPolyGraph::AddUntouchedInputPolygons (int *ncio, int *nvio)

{
    int           i, j, nc, nv, n2, n, nout;
    int           nfirst;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
    };
    CSWScopeGuard  func_scope_guard (fscope);

    nc = *ncio;
    nv = *nvio;

    nout = 0;
    for (i=0; i<nv; i++) {
        nout += Pnvout[i];
    }

/*
 * Check for closed components in the set 1 input polygons
 * that have not been used in the output.
 */
    n2 = 0;
    n = 0;
    for (i=0; i<Np1; i++) {
        for (j=0; j<Nc1[i]; j++) {
            if (InputCompIDFlag1[n2] == 1) {
                n += Nv1[n2];
                n2++;
                continue;
            }
            if (SamePoint (Xp1[n], Yp1[n],
                           Xp1[n+Nv1[n2]-1],
                           Yp1[n+Nv1[n2]-1]) == 1) {

                nfirst = nout;
                for (j=0; j<Nv1[n2]; j++) {
                    Pxout[nout] = Xp1[n];
                    Pyout[nout] = Yp1[n];
                    if (Ptagout != NULL) {
                        Ptagout[nout] = Tag1[n];
                    }
                    n++;
                    nout++;
                    if (nout >= Pmaxnpts) {
                        return -2;
                    }
                }
                Pxout[nout-1] = Pxout[nfirst];
                Pyout[nout-1] = Pyout[nfirst];
                Pncout[nc] = 1;
                Pnvout[nv] = Nv1[n2];
                nc++;
                nv++;
            }
            else {
                n += Nv2[n2];
            }
            n2++;
        }
    }

/*
 * Check for closed components in the set 2 input polygons
 * that have not been used in the output.
 */
    n2 = 0;
    n = 0;
    for (i=0; i<Np2; i++) {
        for (j=0; j<Nc2[i]; j++) {
            if (InputCompIDFlag2[n2] == 1) {
                n += Nv2[n2];
                n2++;
                continue;
            }
            if (SamePoint (Xp2[n], Yp2[n],
                           Xp2[n+Nv2[n2]-1],
                           Yp2[n+Nv2[n2]-1]) == 1) {

                nfirst = nout;
                for (j=0; j<Nv2[n2]; j++) {
                    Pxout[nout] = Xp2[n];
                    Pyout[nout] = Yp2[n];
                    if (Ptagout != NULL) {
                        Ptagout[nout] = Tag2[n];
                    }
                    n++;
                    nout++;
                    if (nout >= Pmaxnpts) {
                        return -2;
                    }
                }
                Pxout[nout-1] = Pxout[nfirst];
                Pyout[nout-1] = Pyout[nfirst];
                Pncout[nc] = 1;
                Pnvout[nv] = Nv2[n2];
                nc++;
                nv++;
            }
            else {
                n += Nv2[n2];
            }
            n2++;
        }
    }

    *nvio = nv;
    *ncio = nc;

    return 1;

}

/*
 *********************************************************************************

                     g r d _ W r i t e L i n e s

 *********************************************************************************

  Write a set of x, y and (optional) z coordinates to a file as individual
  polylines.

*/

int CSWPolyGraph::WriteLines (double *x,
                   double *y,
                   double *z,
                   int    np,
                   int    *nc,
                   int    *nv,
                   char *fname)
{
    FILE         *fptr;
    int          i, j, k, n1, n2;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||
        nc == NULL  ||  nv == NULL  ||
        np < 1  ||  fname == NULL) {
        return -1;
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    n1 = 0;
    for (i=0; i<np; i++) {
        n1 += nc[i];
    }
    sprintf (c200, "%d\n", n1);
    fputs (c200, fptr);

    n1 = 0;
    n2 = 0;
    for (i=0; i<np; i++) {
        for (j=0; j<nc[i]; j++) {
            sprintf (c200, "%d\n", nv[n1]);
            fputs (c200, fptr);
            for (k=0; k<nv[n1]; k++) {
                if (z != NULL) {
                    sprintf (c200, "%g %g %g\n", x[n2], y[n2], z[n2]);
                    fputs (c200, fptr);
                }
                else {
                    sprintf (c200, "%g %g 0.0\n", x[n2], y[n2]);
                    fputs (c200, fptr);
                }
                n2++;
            }
            n1++;
        }
    }

    fclose (fptr);

    return 1;

}


/*
  ****************************************************************************

                          I n i t A l l M e m

  ****************************************************************************

    Init the memory used in the boolean operation.

*/

int CSWPolyGraph::InitAllMem (void)
{
    int              i;

    memset ((void *)&Raw1, 0, sizeof(Raw1));
    memset ((void *)&Raw2, 0, sizeof(Raw2));

    for (i=0; i<MAX_SEG_INDEX * 16; i++) {
        if (SegIndex[i]) {
            SegIndex[i] = NULL;
        }
    }

    for (i=0; i<MAX_NODE_INDEX * 2; i++) {
        if (NodeIndex[i]) {
            NodeIndex[i] = NULL;
        }
    }

    for (i=0; i<LOCAL_MAX_GRID_SIZE * 2; i++) {
        hgrid[i] = 0;
        pgrid[i] = 0;
        vgrid[i] = 0;
    }

    return 1;

}  /*  end of private InitAllMem function  */
