/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_triangle.cc

    This file has the implementation of methods for the CSWGrdTriangle 
    class.  This refactors code previously located in grd_triangle.c 
    and some from grd_api.c
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/ply_protoP.h"
#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_graph.h"

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/surfaceworks/private_include/grd_triangle_class.h"



void CSWGrdTriangle::grd_set_data_check_func (CLIENT_DATA_CHECK_FUNC func_ptr)
{
    DataCheckFunc = func_ptr;
    return;
}

NOdeStruct*  CSWGrdTriangle::getNodeList (void)
{
    return NodeList;
}



void CSWGrdTriangle::PrintRidgeInfo (void)

{
    int        i;
    RIdgeLineStruct *rptr;

    printf ("\n");
    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        printf ("%d:  %d %d\n", i, rptr->nodes[0], rptr->nodes[rptr->nlist-1]);
    }
    printf ("\n");

    return;
}

void CSWGrdTriangle::PrintRidgeNodes (int i)

{
    RIdgeLineStruct   *rptr;
    int j;

    rptr = RidgeLineList + i;
    for (j=0; j<rptr->nlist; j++) {
        printf ("%d ", rptr->nodes[j]);
    }
    printf ("\n");
    return;
}

void CSWGrdTriangle::null_array (double *ptr, int n)
{
    int              i;

    if (ptr == NULL) return;

    for (i=0; i<n; i++) {
        ptr[i] = 1.e30;
    }

    return;

}



/*
 ***********************************************************************************

                V a l i d a t e E d g e I n t e r s e c t i o n

 ***********************************************************************************

  Perform a sanity check to see if some obvious errors exist in the trimesh
  topology.  This does not insure correctness.  It simply points out some
  known problems if they exist.

*/

void CSWGrdTriangle::ValidateEdgeIntersection (const char *msg)

{
    int                 istat, i, j;
    int                 error_flag;
    char                *cenv;
    EDgeStruct          *ep1, *ep2;
    NOdeStruct          *np;
    double              x1, y1, x2, y2, x3, y3, x4, y4, xint, yint;
    double              z1, z2, z3, z4, pct, zt1, zt2;
    double              dx, dy, zt3, ztiny;
    double              gsav;

    CSWPolyUtils        ply_utils_obj;

    if (ForceValidate == 0) {
        cenv = csw_getenv ("GRD_VALIDATE_TRIMESH_TOPO");
        if (cenv == NULL) return;
    }

    error_flag = 0;

    printf ("\nValidating trimesh edge intersection.\n\n");
    if (msg != NULL) {
        printf ("%s\n", msg);
    }

    error_flag = 0;

/*
 * Make sure that no (non deleted) edges intersect each other except
 * at their end points.
 */
    ply_utils_obj.ply_setgraze (GrazeDistance / 10.0);
    gsav = GrazeDistance;
    GrazeDistance = AreaPerimeter / 200000.0;
    ztiny = AreaPerimeter / 20000.0;
    for (i=0; i<NumEdges; i++) {
        ep1 = EdgeList + i;
        if (ep1->deleted == 1) {
            continue;
        }
        np = NodeList + ep1->node1;
        x1 = np->x;
        y1 = np->y;
        z1 = np->z;
        if (z1 > 1.e20  ||  z1 < -1.e20) {
            z1 = 0.0;
        }
        np = NodeList + ep1->node2;
        x2 = np->x;
        y2 = np->y;
        z2 = np->z;
        if (z2 > 1.e20  ||  z2 < -1.e20) {
            z2 = 0.0;
        }
        for (j=i+1; j<NumEdges; j++) {
            ep2 = EdgeList + j;
            if (ep2->deleted == 1) {
                continue;
            }
            np = NodeList + ep2->node1;
            x3 = np->x;
            y3 = np->y;
            z3 = np->z;
            if (z3 > 1.e20  ||  z3 < -1.e20) {
                z3 = 0.0;
            }
            np = NodeList + ep2->node2;
            x4 = np->x;
            y4 = np->y;
            z4 = np->z;
            if (z4 > 1.e20  ||  z4 < -1.e20) {
                z4 = 0.0;
            }
            istat = ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4, &xint, &yint);
            if (istat == 0) {
                if ((SamePoint (xint, yint, x1, y1)  ||
                     SamePoint (xint, yint, x2, y2)) &&
                    (SamePoint (xint, yint, x3, y3)  ||
                     SamePoint (xint, yint, x4, y4))) {
                    continue;
                }
                else {

                    dx = x2 - x1;
                    dy = y2 - y1;
                    if (dx < 0.0) dx = -dx;
                    if (dy < 0.0) dy = -dy;
                    if (dx > dy) {
                        pct = (x2 - xint) / (x2 - x1);
                    }
                    else {
                        pct = (y2 - yint) / (y2 - y1);
                    }
                    zt1 = z1 + pct * (z2 - z1);

                    dx = x4 - x3;
                    dy = y4 - y3;
                    if (dx < 0.0) dx = -dx;
                    if (dy < 0.0) dy = -dy;
                    if (dx > dy) {
                        pct = (x4 - xint) / (x4 - x3);
                    }
                    else {
                        pct = (y4 - yint) / (y4 - y3);
                    }
                    zt2 = z3 + pct * (z4 - z3);

                    zt3 = zt2 - zt1;
                    if (zt3 < 0.0) zt3 = -zt3;

                    if (zt3 >= ztiny) {
                        continue;
                    }

                    printf ("edges %d and %d intersect and they shouldn't\n", i, j);
                    printf ("nodes are %d %d and %d %d\n",
                            ep1->node1, ep1->node2,
                            ep2->node1, ep2->node2);
                    printf ("   %d: %f %f\n", ep1->node1,
                            NodeList[ep1->node1].x,
                            NodeList[ep1->node1].y);
                    printf ("   %d: %f %f\n", ep1->node2,
                            NodeList[ep1->node2].x,
                            NodeList[ep1->node2].y);
                    printf ("   %d: %f %f\n", ep2->node1,
                            NodeList[ep2->node1].x,
                            NodeList[ep2->node1].y);
                    printf ("   %d: %f %f\n", ep2->node2,
                            NodeList[ep2->node2].x,
                            NodeList[ep2->node2].y);
                    printf ("  xint = %f  yint = %f\n", xint, yint);

                    error_flag = 1;
                }
            }
            else if (istat == 3) {

                xint = x3;
                yint = y3;

                dx = x2 - x1;
                dy = y2 - y1;
                if (dx < 0.0) dx = -dx;
                if (dy < 0.0) dy = -dy;
                if (dx > dy) {
                    pct = (x2 - xint) / (x2 - x1);
                }
                else {
                    pct = (y2 - yint) / (y2 - y1);
                }
                zt1 = z1 + pct * (z2 - z1);

                dx = x4 - x3;
                dy = y4 - y3;
                if (dx < 0.0) dx = -dx;
                if (dy < 0.0) dy = -dy;
                if (dx > dy) {
                    pct = (x4 - xint) / (x4 - x3);
                }
                else {
                    pct = (y4 - yint) / (y4 - y3);
                }
                zt2 = z3 + pct * (z4 - z3);

                zt3 = zt2 - zt1;
                if (zt3 < 0.0) zt3 = -zt3;

                if (zt3 >= ztiny) {
                    continue;
                }

                printf ("edges %d and %d overlap each other.\n", i, j);
                printf ("nodes are %d %d and %d %d\n",
                        ep1->node1, ep1->node2,
                        ep2->node1, ep2->node2);
                printf ("   %d: %f %f\n", ep1->node1,
                        NodeList[ep1->node1].x,
                        NodeList[ep1->node1].y);
                printf ("   %d: %f %f\n", ep1->node2,
                        NodeList[ep1->node2].x,
                        NodeList[ep1->node2].y);
                printf ("   %d: %f %f\n", ep2->node1,
                        NodeList[ep2->node1].x,
                        NodeList[ep2->node1].y);
                printf ("   %d: %f %f\n", ep2->node2,
                        NodeList[ep2->node2].x,
                        NodeList[ep2->node2].y);

                error_flag = 1;
            }
            else if (istat == 4) {
                if (ep1->tri2 >= 0  ||  ep2->tri2 >= 0) {
                    printf ("edges %d and %d are identical\n", i, j);
                    printf ("nodes are %d %d and %d %d\n",
                            ep1->node1, ep1->node2,
                            ep2->node1, ep2->node2);
                    printf ("   %d: %f %f\n", ep1->node1,
                            NodeList[ep1->node1].x,
                            NodeList[ep1->node1].y);
                    printf ("   %d: %f %f\n", ep1->node2,
                            NodeList[ep1->node2].x,
                            NodeList[ep1->node2].y);
                    printf ("   %d: %f %f\n", ep2->node1,
                            NodeList[ep2->node1].x,
                            NodeList[ep2->node1].y);
                    printf ("   %d: %f %f\n", ep2->node2,
                            NodeList[ep2->node2].x,
                            NodeList[ep2->node2].y);

                    error_flag = 1;
                }
            }
        }
    }

    GrazeDistance = gsav;

    if (error_flag == 0) {
        printf ("No incorrectly intersecting edges found in validation\n");
    }

    if (msg != NULL) {
        printf ("%s\n", msg);
    }
    printf ("\nFinished Validating trimesh edge intersection.\n\n\n");

    return;

}  /* end of private ValidateEdgeIntersection function */




/*
 **************************************************************************

           g r d _ c a l c _ t r i a n g l e _ d i p s

 **************************************************************************

  Calculate the direction and magnitude of the dip at each non deleted
  triangle.  Returned values of 1.e30 for both direction and amplitude
  mean the triangle was deleted.

*/

int CSWGrdTriangle::grd_calc_triangle_dips (NOdeStruct *nodes, int numnodes,
                            EDgeStruct *edges, int numedges,
                            TRiangleStruct *triangles, int numtriangles,
                            int degree_flag,
                            double *direction, double *amplitude)
{
    int                     i;
    double                  x[3], y[3], z[3], coef[3], dx, dy, ang, amp;
    TRiangleStruct          *tptr;

    TriangleList = triangles;
    EdgeList = edges;
    NodeList = nodes;
    NumTriangles = numtriangles;
    NumEdges = numedges;
    NumNodes = numnodes;

    for (i=0; i<numtriangles; i++) {
        tptr = triangles + i;
        if (tptr->deleted) {
            if (direction) direction[i] = 1.e30;
            if (amplitude) amplitude[i] = 1.e30;
            continue;
        }
        TrianglePoints (tptr, x, y, z);
        grd_utils_ptr->grd_calc_double_plane (x, y, z, 3, coef);
        dx = coef[1];
        dy = coef[2];
        ang = atan2 (dy, dx);
        if (degree_flag) ang *= 180.0;
        amp = dx * dx + dy * dy;
        amp = sqrt(amp);
        if (direction) direction[i] = ang;
        if (amplitude) amplitude[i] = amp;
    }

    ListNull ();
    FreeMem ();

    return 1;

}  /* end of function grd_calc_triangle_dips */






/*
 ***************************************************************************

       g r d _ d e l e t e _ e d g e s _ f r o m _ t r i m e s h

 ***************************************************************************

*/

int CSWGrdTriangle::grd_delete_edges_from_trimesh (NOdeStruct *nodes, int numnodes,
                                   EDgeStruct *edges, int numedges,
                                   TRiangleStruct *triangles, int numtriangles,
                                   int *edgenumlist, int nlist,
                                   int swapflag)
{
    int                 i;

    NodeList = nodes;
    NumNodes = numnodes;
    EdgeList = edges;
    NumEdges = numedges;
    TriangleList = triangles;
    NumTriangles = numtriangles;

    if (swapflag == 1) {
        for (i=0; i<nlist; i++) {
            SwapEdge (edgenumlist[i]);
        }
    }
    else {
        for (i=0; i<nlist; i++) {
            WhackEdge (edgenumlist[i]);
        }
    }

    ListNull ();
    FreeMem ();

    return 1;

}  /* end of function grd_delete_edges_from_trimesh */




/*
 ***************************************************************************

     g r d _ d e l e t e _ t r i a n g l e s _ f r o m _ t r i m e s h

 ***************************************************************************

*/

int CSWGrdTriangle::grd_delete_triangles_from_trimesh (NOdeStruct *nodes, int numnodes,
                                   EDgeStruct *edges, int numedges,
                                   TRiangleStruct *triangles, int numtriangles,
                                   int *trianglenumlist, int nlist)
{
    int                 i, e1, *list;
    TRiangleStruct      *tptr;
    EDgeStruct          *eptr;

    NodeList = nodes;
    NumNodes = numnodes;
    EdgeList = edges;
    NumEdges = numedges;
    TriangleList = triangles;
    NumTriangles = numtriangles;

    list = trianglenumlist;

    for (i=0; i<nlist; i++) {
        tptr = triangles + list[i];
        e1 = tptr->edge1;
        eptr = edges + e1;
        if (eptr->tri1 == list[i]) {
            eptr->tri1 = -1;
        }
        if (eptr->tri2 == list[i]) {
            eptr->tri2 = -1;
        }
        if (eptr->tri1 == -1) {
            eptr->tri1 = eptr->tri2;
        }
        if (eptr->tri1 == -1) {
            WhackEdge (e1);
        }

        e1 = tptr->edge2;
        eptr = edges + e1;
        if (eptr->tri1 == list[i]) {
            eptr->tri1 = -1;
        }
        if (eptr->tri2 == list[i]) {
            eptr->tri2 = -1;
        }
        if (eptr->tri1 == -1) {
            eptr->tri1 = eptr->tri2;
        }
        if (eptr->tri1 == -1) {
            WhackEdge (e1);
        }

        e1 = tptr->edge3;
        eptr = edges + e1;
        if (eptr->tri1 == list[i]) {
            eptr->tri1 = -1;
        }
        if (eptr->tri2 == list[i]) {
            eptr->tri2 = -1;
        }
        if (eptr->tri1 == -1) {
            eptr->tri1 = eptr->tri2;
        }
        if (eptr->tri1 == -1) {
            WhackEdge (e1);
        }
        tptr->deleted = 1;
    }

    ListNull ();
    FreeMem ();

    return 1;

}  /* end of function grd_delete_triangles_from_trimesh */





/*
 *****************************************************************************

            g r d _ g e t _ b u g _ l o c a t i o n s

 *****************************************************************************

*/

int CSWGrdTriangle::grd_get_bug_locations (double *bx, double *by, int *nbugs, int maxbugs)
{
    int            i, n;

    n = Nbugs;
    if (n > maxbugs) n = maxbugs;

    for (i=0; i<n; i++) {
        bx[i] = BugX[i];
        by[i] = BugY[i];
    }

    *nbugs = n;

    Nbugs = 0;

    return n;

}  /* end of function grd_get_bug_locations */




/*
 *****************************************************************************

        g r d _ s e t _ p o l y _ c o n s t r a i n t _ f l a g

 *****************************************************************************

*/

int CSWGrdTriangle::grd_set_poly_constraint_flag (int val)
{
    val = val;
    PolygonalizeConstraintFlag = 0;
    return 1;
}


void CSWGrdTriangle::ResetGridStaticValues (void)
{
    GridX1 = 0.0;
    GridY1 = 0.0;
    GridX2 = 0.0;
    GridY2 = 0.0;
    GridXspace = 0.0;
    GridYspace = 0.0;
    GridNcol = 0;
    GridNrow = 0;
}



/*
  ****************************************************************************

         g r d _ g r i d _ t o _ e q u i l a t e r a l _ t r i m e s h

  ****************************************************************************

  Convert a grid to a trimesh.  If the grid is faulted, the fault lines
  must be specified in the lines data.  These lines are not actually
  embedded into the trimesh, but they are needed for interpolation of
  z values at points shifted to make the trimesh equilateral.

*/

void CSWGrdTriangle::grd_set_dont_do_eq (int ival)
{
    if (ival != 1) ival = 0;
    DontDoEquilateral = ival;
}

int CSWGrdTriangle::grd_grid_to_equilateral_trimesh
                        (CSW_F *gridin, int nc, int nr,
                         double x1, double y1, double x2, double y2,
                         double *xlinesin, double *ylinesin, double *zlinesin,
                         int *linepointsin, int *linetypes, int nlinesin,
                         NOdeStruct **nodes_out, EDgeStruct **edges_out,
                         TRiangleStruct **triangles_out,
                         int *num_nodes_out, int *num_edges_out,
                         int *num_triangles_out)

{
    int                  i, j, k, n, offset, edges_per_row;
    int                  maxraw, ebase, tbase, ke, kt, triangles_per_row;
    int                  top_ebase, istat;
    CSW_F                *grid = NULL, zinterp;
    CSW_F                *xinterp = NULL, *yinterp = NULL, *zinterp2 = NULL;
    int                  maxinterp, ninterp, *interp = NULL;
    double               xt, yt, xspace, yspace, xyspace;
    NOdeStruct           *nptr = NULL;
    EDgeStruct           *eptr = NULL;
    TRiangleStruct       *tptr = NULL;
    RAwPointStruct       *rptr = NULL;
    double               *xlines = NULL, *ylines = NULL, *zlines = NULL;
    int                  *linepoints = NULL, nlines;
    int                  do_write;
    FAultLineStruct      *faults = NULL;
    int                  nfaults, nrorig, upper_right_edge_num,
                         right_side_start, num_right_side, nright;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        ListNull ();
        FreeMem ();
        ResetGridStaticValues ();
        csw_Free (grid);
        grd_fault_ptr->grd_free_fault_line_structs (faults, nfaults);
        csw_Free (xinterp);
        if (bsuccess == false) {
            csw_Free (NodeList);
            NodeList = NULL;
            csw_Free (EdgeList);
            EdgeList = NULL;
            csw_Free (TriangleList);
            TriangleList = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

    csw_Free (NodeList);
    NodeList = NULL;
    csw_Free (EdgeList);
    EdgeList = NULL;
    csw_Free (TriangleList);
    TriangleList = NULL;

    linetypes = linetypes;

    if (nlinesin <= 0) {
        ChopLinesFlag = 0;
        nlinesin = -nlinesin;
    }
    else {
        ChopLinesFlag = 1;
    }

    ConstraintPointStart = -1;

/*
    Set the output to invalid values in case an error occurs.
*/
    *nodes_out = NULL;
    *num_nodes_out = 0;
    *edges_out = NULL;
    *num_edges_out = 0;
    *triangles_out = NULL;
    *num_triangles_out = 0;

    nrorig = nr;

    xspace = (x2 - x1) / (double)(nc - 1);
    yspace = (y2 - y1) / (double)(nr - 1);
    yspace *= sqrt (3.0);
    yspace *= .5;
    nr = (int) ((y2 - y1) / yspace + .5);
    yspace = (y2 - y1) / (double)(nr - 1);

    xyspace = .25 * xspace * xspace + yspace * yspace;
    xyspace = sqrt (xyspace);

    GridX1 = x1;
    GridY1 = y1;
    GridX2 = x1 + (nc - 1) * xspace;
    GridY2 = y1 + (nr - 1) * yspace;
    GridXspace = xspace;
    GridYspace = yspace;
    GridNcol = nc;
    GridNrow = nr;

    AreaPerimeter = x2 - x1 + y2 - y1;
    GrazeDistance = (x2 - x1 + y2 - y1) / 2000000.0;
    AdjustDistance = (x2 - x1 + y2 - y1) / 200000.0;
    FaultAdjustDistance = AdjustDistance * 10.0;
    ConstraintSegmentNumber = -1;
    CornerBias = 1.0;
    NtryFlag = 0;
    Nbugs = 0;
    MaxNcall = (nr + nc) * 10;

/*
    Clean up the lines to make them usable if needed.
    The private Xline, Yline, Zline Iflag and Iline arrays
    have the cleaned up lines.
*/
    ClipToGridFlag = 1;
    istat = FixLineDefects (xlinesin, ylinesin, zlinesin,
                            linepointsin, NULL, nlinesin);
    ClipToGridFlag = 0;
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    xlines = Xline;
    ylines = Yline;
    zlines = Zline;
    linepoints = Iline;
    nlines = Nline;

    faults = NULL;
    nfaults = 0;
    if (nlines > 0) {
        istat = grd_fault_ptr->grd_fault_arrays_to_structs_2 (
            xlines, ylines, zlines,
            linepoints, NULL, nlines,
            &faults, &nfaults);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

    grid = (CSW_F *)csw_Malloc (nc * nr * sizeof(CSW_F));
    if (grid == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    istat =
      grd_arith_ptr->grd_resample_grid (
        gridin, NULL, nc, nrorig,
        (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
        faults, nfaults,
        grid, NULL, nc, nr,
        (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
        GRD_BICUBIC);

/*
    Allocate space for the raw points, which is the grid nodes
    plus possibly two additional columns.
*/
    n = nc * nr;
    n += nr;
    n += nr;
    RawPoints = (RAwPointStruct *)csw_Calloc (n * sizeof(RAwPointStruct));
    NumRawPoints = 0;
    MaxRawPoints = n;
    if (RawPoints == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    maxraw = n;

    NumRawPoints = nc * nr;

/*
    Allocate space for the initial triangle, edge and node lists.
    The initial sizes are sufficient for a non constrained trimesh.
*/
    n = nc * nr * 4;
    n += nr;
    n += nr;
    n += nc;
    n += nc;
    TriangleList = (TRiangleStruct *)csw_Calloc (n * sizeof(TRiangleStruct));
    if (!TriangleList) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    MaxTriangles = n;
    NumTriangles = 0;

    n = nr * (nc * 4 + 1);
    n += nr;
    n += nr;
    n += nc;
    n += nc;
    EdgeList = (EDgeStruct *)csw_Calloc (n * sizeof(EDgeStruct));
    if (!EdgeList) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    MaxEdges = n;
    NumEdges = 0;

    n = nc * nr * 2;
    if (maxraw > n) n = maxraw;
    n += nr * 2;
    n += nc * 2;
    NodeList = (NOdeStruct *)csw_Calloc (n * sizeof(NOdeStruct));
    if (!NodeList) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    MaxNodes = n;
    NumNodes = 0;

    for (i=0; i<n; i++) {
        NodeList[i].deleted = 1;
    }

/*
    Set the x and y coordinates of the trimesh nodes and raw points to the
    coordinates of the grid nodes.  On the even numbered rows, the actual
    grid nodes are used.  For odd numbered rows, the nodes (except for
    the far left node are moved 1/2 x grid spacing
    to the left.  This provides a more equilateral trimesh.
*/
    for (i=0; i<nr; i++) {
        yt = y1 + i * yspace;
        offset = i * nc;
        for (j=0; j<nc; j++) {
            xt = x1 + j * xspace;
            k = offset + j;
            nptr = NodeList + k;
            if (i % 2 == 0  ||  j == 0) {
                zinterp = grid[k];
                nptr->x = xt;
                nptr->y = yt;
                nptr->z = zinterp;
                nptr->adjusting_node = -1;
                nptr->spillnum = -1;
                nptr->rp = k;
                nptr->crp = -1;
                nptr->deleted = 0;
                rptr = RawPoints + k;
                rptr->x = xt;
                rptr->y = yt;
                rptr->z = grid[k];
                rptr->nodenum = k;
            }
            else {
                zinterp = 1.e30f;
                nptr->x = xt - xspace / 2.0;
                nptr->y = yt;
                nptr->z = zinterp;
                nptr->adjusting_node = -1;
                nptr->spillnum = -1;
                nptr->rp = k;
                nptr->crp = -1;
                nptr->deleted = 0;
                rptr = RawPoints + k;
                rptr->x = xt;
                rptr->y = yt;
                rptr->z = 1.e30;
                rptr->nodenum = k;
            }
        }
    }
    NumNodes = nc * nr;

/*
 * Make an extra node at the right side of each odd
 * row.  It's z value is exactly that of the grid node.
 */
    right_side_start = NumNodes;
    num_right_side = 0;

    for (i=1; i<nr; i+=2) {
        yt = y1 + i * yspace;
        offset = i * nc;
        k = offset + nc - 1;
        xt = x1 + (nc - 1) * xspace;
        nptr = NodeList + right_side_start + num_right_side;
        nptr->x = xt;
        nptr->y = yt;
        nptr->z = grid[k];
        nptr->adjusting_node = -1;
        nptr->spillnum = -1;
        nptr->rp = right_side_start + num_right_side;
        nptr->crp = -1;
        nptr->deleted = 0;
        rptr = RawPoints + right_side_start + num_right_side;
        rptr->x = xt;
        rptr->y = yt;
        rptr->z = grid[k];
        rptr->nodenum = right_side_start + num_right_side;
        num_right_side++;
    }

    NumNodes += num_right_side;

/*
 * Interpolate the null node z values.
 */
    maxinterp = nc * nr;
    xinterp = (CSW_F *)csw_Malloc (maxinterp * 3 * sizeof(CSW_F) + maxinterp * sizeof(int));
    if (xinterp == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    yinterp = xinterp + maxinterp;
    zinterp2 = yinterp + maxinterp;
    interp = (int *)(zinterp2 + maxinterp);
    ninterp = 0;
    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        if (nptr->z < 1.e20) {
            continue;
        }
        xinterp[ninterp] = (CSW_F)(nptr->x);
        yinterp[ninterp] = (CSW_F)(nptr->y);
        interp[ninterp] = i;
        ninterp++;
        if (ninterp >= maxinterp) {
            printf ("overflow on interp arrays ninterp = %d maxinterp = %d\n",
                    ninterp, maxinterp);
            printf ("NumNodes = %d\n", NumNodes);
        }
    }

    istat = grd_arith_ptr->grd_back_interpolate (
        gridin, nc, nrorig,
        (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
        faults, nfaults,
        xinterp, yinterp, zinterp2, ninterp,
        GRD_BICUBIC);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    for (i=0; i<ninterp; i++) {
        if (interp[i] < 0  ||  interp[i] >= NumNodes) {
            printf ("writing outside node list %d\n", interp[i]);
        }
        NodeList[interp[i]].z = zinterp2[i];
    }

    xinterp = yinterp = zinterp2 = NULL;
    interp = NULL;

/*
   Create the edges.  Each grid cell has an edge on its left side, on its
   bottom side and a diagonal edge.  For cells in even numbered rows, the
   diagonal is from lower left to upper right.  For cells in odd number
   rows, the diagonal is from upper left to lower right. This is done to
   connect more equilateral triangles in conjuction with the left shifting
   of the nodes in the odd rows.  The numbers for these edges, within the
   cell are 1, 2 and 3 respectively.  Each cell's numbering starts at 3 *
   the column number of the node at the lower left corner of the cell.
*/
    edges_per_row = (nc - 1) * 3 + 3;
    triangles_per_row = (nc - 1) * 2 + 1;

    for (i=0; i<edges_per_row * nr; i++) {
        EdgeList[i].deleted = 1;
    }
    for (i=0; i<triangles_per_row * nr; i++) {
        TriangleList[i].deleted = 1;
    }

/*
   Do all but the top row.
*/
    nright = 0;
    for (i=0; i<nr-1; i++) {

        offset = i * nc;
        ebase = i * edges_per_row;

      /*
       * Do the even numbered rows from lower left to upper right.
       */
        if (i % 2 == 0) {
        /*
           Do all but the right most column.
        */
            for (j=0; j<nc-1; j++) {
                k = ebase + j * 3;
                eptr = EdgeList + k;
                eptr->deleted = 0;
                eptr->node1 = offset + j;
                eptr->node2 = offset + j + nc;
                eptr->length = yspace;
                eptr->tri1 = -1;
                eptr->tri2 = -1;

                eptr = EdgeList + k + 1;
                eptr->deleted = 0;
                eptr->node1 = offset + j;
                eptr->node2 = offset + j + 1;
                eptr->length = xspace;
                eptr->tri1 = -1;
                eptr->tri2 = -1;

                eptr = EdgeList + k + 2;
                eptr->deleted = 0;
                eptr->node1 = offset + j;
                eptr->node2 = offset + j + nc + 1;
                eptr->length = xyspace;
                eptr->tri1 = -1;
                eptr->tri2 = -1;
            }
        /*
           Do the right most column.
        */
            j = nc - 1;
            k = ebase + edges_per_row - 3;
            eptr = EdgeList + k;
            eptr->deleted = 0;
            eptr->node1 = offset + j;
            eptr->node2 = offset + j + nc;
            eptr->length = xyspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;
            k = ebase + edges_per_row - 2;
            eptr = EdgeList + k;
            eptr->deleted = 0;
            eptr->node1 = offset + j;
            eptr->node2 = right_side_start + nright;
            eptr->length = yspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;
            k = ebase + edges_per_row - 1;
            eptr = EdgeList + k;
            eptr->deleted = 0;
            eptr->node1 = -1;
            eptr->node2 = -1;
            eptr->tri1 = -1;
            eptr->tri2 = -1;
            eptr->deleted = 1;
        }

      /*
       * Do the odd numbered rows from lower left to upper right.
       */
        else {

        /*
           Do all but the right most column.
        */
            for (j=0; j<nc-1; j++) {
                k = ebase + j * 3;
                eptr = EdgeList + k;
                eptr->deleted = 0;
                eptr->node1 = offset + j;
                eptr->node2 = offset + j + nc;
                eptr->length = yspace;
                eptr->tri1 = -1;
                eptr->tri2 = -1;

                eptr = EdgeList + k + 1;
                eptr->deleted = 0;
                eptr->node1 = offset + j;
                eptr->node2 = offset + j + 1;
                eptr->length = xspace;
                eptr->tri1 = -1;
                eptr->tri2 = -1;

                eptr = EdgeList + k + 2;
                eptr->deleted = 0;
                eptr->node1 = offset + j + nc;
                eptr->node2 = offset + j + 1;
                eptr->length = xyspace;
                eptr->tri1 = -1;
                eptr->tri2 = -1;
            }
        /*
           Do the right most column.
        */
            j = nc - 1;
            k = ebase + edges_per_row - 3;
            eptr = EdgeList + k;
            eptr->deleted = 0;
            eptr->node1 = offset + j;
            eptr->node2 = offset + j + nc;
            eptr->length = xyspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;
            k = ebase + edges_per_row - 2;
            eptr = EdgeList + k;
            eptr->deleted = 0;
            eptr->node1 = offset + j;
            eptr->node2 = right_side_start + nright;
            eptr->length = xspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;
            k = ebase + edges_per_row - 1;
            eptr = EdgeList + k;
            eptr->deleted = 0;
            eptr->node1 = right_side_start + nright;
            eptr->node2 = offset + j + nc;
            eptr->length = yspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;
            nright++;
        }
    }
/*
    Do the top row.  Only the horizontal edges are
    added here, so odd or even row doesn't matter.
*/
    offset = (nr - 1) * nc;
    top_ebase = (nr - 1) * edges_per_row;
    for (j=0; j<nc-1; j++) {
        k = top_ebase + j;
        eptr = EdgeList + k;
        eptr->deleted = 0;
        eptr->node1 = offset + j;
        eptr->node2 = offset + j + 1;
        eptr->length = xspace;
        eptr->tri1 = -1;
        eptr->tri2 = -1;
    }

/*
 * If the number of rows is odd, the top right horizontal edge
 * needs to be created.
 */
    upper_right_edge_num = -1;
    if ((nr - 1)%2 == 1) {
        k = top_ebase + nc - 1;
        upper_right_edge_num = k;
        eptr = EdgeList + k;
        eptr->deleted = 0;
        eptr->node1 = offset + nc - 1;
        eptr->node2 = right_side_start + nright;
        eptr->length = xspace;
        eptr->tri1 = -1;
        eptr->tri2 = -1;
    }

    NumEdges = edges_per_row * nr;

/*
    Fill in the triangle list.  Each cell has two triangles.
    In even numbered rows, the first uses the bottom, right
    and diagonal while the second uses the left, top and
    diagonal.  In odd numbered rows, the first triangle uses
    the left, bottom and diagonal while the second triangle
    uses the top, right and diagonal.
*/
    for (i=0; i<nr-1; i++) {
        tbase = i * triangles_per_row;
        ebase = i * edges_per_row;

      /*
       * Even numbered rows.
       */
        if (i % 2 == 0) {
            for (j=0; j<nc-1; j++) {
                kt = tbase + j * 2;
                ke = ebase + j * 3;

                tptr = TriangleList + kt;
                tptr->deleted = 0;

                tptr->edge1 = ke + 1;
                eptr = EdgeList + ke + 1;
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt;
                }
                else {
                    eptr->tri2 = kt;
                }

                tptr->edge2 = ke + 2;
                eptr = EdgeList + ke + 2;
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt;
                }
                else {
                    eptr->tri2 = kt;
                }

                tptr->edge3 = ke + 3;
                eptr = EdgeList + ke + 3;
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt;
                }
                else {
                    eptr->tri2 = kt;
                }

                tptr = TriangleList + kt + 1;
                tptr->deleted = 0;

                tptr->edge1 = ke;
                eptr = EdgeList + ke;
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt + 1;
                }
                else {
                    eptr->tri2 = kt + 1;
                }

                tptr->edge2 = ke + 2;
                eptr = EdgeList + ke + 2;
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt + 1;
                }
                else {
                    eptr->tri2 = kt + 1;
                }

                tptr->edge3 = ke + edges_per_row + 1;
                if (i == nr-2) {
                    tptr->edge3 = top_ebase + j;
                }
                if (i == nr-2) {
                    eptr = EdgeList + top_ebase + j;
                }
                else {
                    eptr = EdgeList + ke + edges_per_row + 1;
                }
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt + 1;
                }
                else {
                    eptr->tri2 = kt + 1;
                }
            }

        /*
         * Rightmost triangle.
         */
            kt = tbase + (nc - 1) * 2;
            ke = ebase + edges_per_row - 3;
            tptr = TriangleList + kt;
            tptr->deleted = 0;
            tptr->edge1 = ke;
            eptr = EdgeList + ke;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }
            tptr->edge2 = ke + 1;
            eptr = EdgeList + ke + 1;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }
            tptr->edge3 = ke + edges_per_row + 1;
            eptr = EdgeList + ke + edges_per_row + 1;
            if (i == nr-2) {
                if (upper_right_edge_num == -1) {
                    assert (0);
                }
                tptr->edge3 = upper_right_edge_num;
                eptr = EdgeList + upper_right_edge_num;
            }
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }
        }

      /*
       * odd numbered rows.
       */
        else {

            for (j=0; j<nc-1; j++) {
                kt = tbase + j * 2;
                ke = ebase + j * 3;

                tptr = TriangleList + kt;
                tptr->deleted = 0;

                tptr->edge1 = ke;
                eptr = EdgeList + ke;
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt;
                }
                else {
                    eptr->tri2 = kt;
                }

                tptr->edge2 = ke + 1;
                eptr = EdgeList + ke + 1;
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt;
                }
                else {
                    eptr->tri2 = kt;
                }

                tptr->edge3 = ke + 2;
                eptr = EdgeList + ke + 2;
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt;
                }
                else {
                    eptr->tri2 = kt;
                }

                tptr = TriangleList + kt + 1;
                tptr->deleted = 0;

                tptr->edge1 = ke + edges_per_row + 1;
                if (i == nr-2) {
                    tptr->edge1 = top_ebase + j;
                }
                eptr = EdgeList + ke + edges_per_row + 1;
                if (i == nr-2) {
                    eptr = EdgeList + top_ebase + j;
                }
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt + 1;
                }
                else {
                    eptr->tri2 = kt + 1;
                }

                tptr->edge2 = ke + 3;
                eptr = EdgeList + ke + 3;
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt + 1;
                }
                else {
                    eptr->tri2 = kt + 1;
                }

                tptr->edge3 = ke + 2;
                eptr = EdgeList + ke + 2;
                if (eptr->tri1 == -1) {
                    eptr->tri1 = kt + 1;
                }
                else {
                    eptr->tri2 = kt + 1;
                }
            }

        /*
         * Rightmost triangle.
         */
            kt = tbase + (nc - 1) * 2;
            ke = ebase + edges_per_row - 3;
            tptr = TriangleList + kt;
            tptr->deleted = 0;
            tptr->edge1 = ke;
            eptr = EdgeList + ke;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }
            tptr->edge2 = ke + 1;
            eptr = EdgeList + ke + 1;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }
            tptr->edge3 = ke + 2;
            eptr = EdgeList + ke + 2;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }

        }
    }
    NumTriangles = (nr - 1) * triangles_per_row;

    RemoveNullsFromTriMesh (1.e20, -1);

    RemoveDeletedElements ();

    ForceValidate = 0;
    ValidateTriangles ((char *)"From calc trimesh from points");
    ForceValidate = 0;

    ForceValidate = 0;
    ValidateEdgeIntersection (NULL);
    ForceValidate = 0;

    do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_WriteTextTriMeshFile (
            0, NULL,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            (char *)"grid_to_eq_tmesh.tri");
    }

/*
    return the results and clean up any work space
*/
    *nodes_out = NodeList;
    *num_nodes_out = NumNodes;
    *edges_out = EdgeList;
    *num_edges_out = NumEdges;
    *triangles_out = TriangleList;
    *num_triangles_out = NumTriangles;

    bsuccess = true;

    return 1;

}  /*  end of function grd_grid_to_equilateral_trimesh  */




/*
  ****************************************************************************

                     g r d _ c a l c _ t r i m e s h

  ****************************************************************************

    This is the top level function used to calculate a triangular mesh given
  a list of x, y, z points and optionally, a list of x, y, z constraint line
  vertices.  The function returns lists of NOdeStructs, EDgeStructs and
  TRiangleStructs.  (These are defined in csw/surfaceworks/private_include/
  grd_triangleP.h).  The number of structures of each type is also returned.

    The x, y, z points are simply separate arrays of double values for each
  coordinate.  The lines are passed in "packed" format.  The vertices for each
  separate line are put into separate xline, yline, and zline arrays.  The
  number of points is put in the linepoints array.  Thus, if the first 3 lines
  have 5, 9, and 3 points respectively, the first 5 xline, yline, zline double
  values are for the first line.  Values 5 through 14 are for line 2 and values
  15 through 17 are for the third line.  The line points array has 5, 9 and 3
  in its 3 values.  The nlines variable is 3 for this example.

*/

int CSWGrdTriangle::grd_calc_trimesh (double *xpts, double *ypts, double *zpts, int npts,
                      double *xlinesin, double *ylinesin, double *zlinesin,
                      int *linepointsin, int *lineflagsin, int nlinesin,
                      NOdeStruct **nodes_out, EDgeStruct **edges_out,
                      TRiangleStruct **triangles_out,
                      int *num_nodes_out, int *num_edges_out, int *num_triangles_out)
{
    int                    istat, i, j, n, rc1, rc2, rc3, rc4;
    int                    nt;
    double                 xmin, ymin, xmax, ymax, dx;
    double                 *xlines, *ylines, *zlines;
    double                 softchk;
    int                    *linepoints, *lineflags, nlines, nltot;
    RAwPointStruct         *rptr, *rpstart;
    NOdeStruct             *nptr;
    int                    exact_flag = 0;
    int                    closed_flag;

    CSWPolyUtils        ply_utils_obj;

/*
 * !!!! debug only
 */

    int    do_write;
    char   filename[200];
    char   fname1[200];
    double v6[6];

    ConvexHullFlag = 0;
    if (npts < 0) {
        npts = -npts;
        ConvexHullFlag = 1;
    }

/*
 * obvious errors
 */
    if (npts < 3) {
        grd_utils_ptr->grd_set_err (2);
        ConvexHullFlag = 0;
        return -1;
    }

    ConstraintPointStart = 0;

    if (nlinesin <= 0) {
        ChopLinesFlag = 0;
        nlinesin = -nlinesin;
    }
    else {
        ChopLinesFlag = 1;
    }

    SplitLongFlag = 0;
    if (*num_edges_out < 0) {
        SplitLongFlag = 1;
        SplitLongLength = -(*num_edges_out);
    }

/*
    Initialize the output pointers to NULL and output counters
    to zero in case of an error.
*/
    RawPoints = NULL;
    RawLines = NULL;
    *nodes_out = NULL;
    *edges_out = NULL;
    *triangles_out = NULL;
    *num_nodes_out = 0;
    *num_edges_out = 0;
    *num_triangles_out = 0;

    if (NodeList != NULL) {
        printf ("non null node list in calc trimesh\n");
    }
    FreeMem();
    ConstraintSegmentNumber = -1;
    NtryFlag = 0;
    Nbugs = 0;

    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;

    for (i=0; i<npts; i++) {
        if (xpts[i] < xmin) xmin = xpts[i];
        if (ypts[i] < ymin) ymin = ypts[i];
        if (xpts[i] > xmax) xmax = xpts[i];
        if (ypts[i] > ymax) ymax = ypts[i];
    }

/*
 * If there are no points or if the points are all in the same location,
 * return an error.
 */
    if (xmax <= xmin  ||  ymax <= ymin) {
        grd_utils_ptr->grd_set_err (3);
        ConvexHullFlag = 0;
        return -1;
    }

/*
    Convert all constaint lines to polygons and throw out
    any lines that self intersect.
*/
    XlineMin = 1.e30;
    YlineMin = 1.e30;
    XlineMax = -1.e30;
    YlineMax = -1.e30;
    FixLineDefects (xlinesin, ylinesin, zlinesin,
                    linepointsin, lineflagsin, nlinesin);

    if (XlineMin < xmin) xmin = XlineMin;
    if (YlineMin < ymin) ymin = YlineMin;
    if (XlineMax > xmax) xmax = XlineMax;
    if (YlineMax > ymax) ymax = YlineMax;

/*
    Chop up the constraint lines into segments that are of
    similar length as the nominal raw point separation.
*/
    ChopLines (Xline, Yline, Zline,
               Iline, Iflag, Nline,
               xmin, ymin, xmax, ymax, npts);
    xlines = Xchop;
    ylines = Ychop;
    zlines = Zchop;
    linepoints = ChopPoints;
    lineflags = ChopFlags;
    nlines = Nchop;

    csw_Free (Xline);
    csw_Free (Iline);
    Xline = NULL;
    Yline = NULL;
    Zline = NULL;
    Iline = NULL;
    Iflag = NULL;
    Nline = 0;

/*
    Allocate space for the raw lines list to be filled in later.
*/
    nltot = 0;
    for (i=0; i<nlines; i++) {
        nltot += linepoints[i];
    }

    n = nltot + nlines*2;
    RawLines = (RAwLineSegStruct *)csw_Calloc (n*sizeof(RAwLineSegStruct));
    NumRawLines = 0;
    if (RawLines == NULL) {
        grd_utils_ptr->grd_set_err(1);
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }

/*
    Allocate space for the raw points, which include both line vertices
    and individual points.
*/
    n += npts;
    n += 100;
    RawPoints = (RAwPointStruct *)csw_Calloc (n * sizeof(RAwPointStruct));
    NumRawPoints = 0;
    MaxRawPoints = n;
    if (RawPoints == NULL) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }

    MaxNcall = n * 10;

/*
    Combine both the points and line vertices into the
    raw point array.
*/
    rptr = RawPoints;
    GrazeDistance = (xmax - xmin + ymax - ymin) / 200000.0;
    for (i=0; i<npts; i++) {
        if (nltot > 0) {
            istat = IsConstraintPoint (xpts[i], ypts[i],
                                       xlines, ylines, nltot);
            if (istat == 1) {
                continue;
            }
        }
        rptr->x = xpts[i];
        rptr->y = ypts[i];
        rptr->z = zpts[i];
        rptr->nodenum = -1;
        rptr->deleted = 0;
        rptr++;
    }

/*
 *
 *  If a raw constraint point coincides with previously defined
 *  raw constraint point, use the previous not the new.
 */
    n = 0;
    for (i=0; i<nlines; i++) {
        rpstart = rptr;
        closed_flag = 0;
        for (j=0; j<linepoints[i]; j++) {
            if (j == linepoints[i] - 1) {
                if (SamePoint (rpstart->x, rpstart->y, xlines[n], ylines[n])) {
                    closed_flag = 1;
                    continue;
                }
            }
            rptr->x = xlines[n];
            rptr->y = ylines[n];
            rptr->z = zlines[n];
            rptr->nodenum = -1;
            rptr->deleted = 0;
            rptr->constraint_class = lineflags[i];
            rptr->exact_flag = exact_flag;
            rptr->endflag = 0;
            if (j == 0) {
                rptr->endflag = 1;
            }
            if (j == linepoints[i]-1) {
                rptr->endflag = 2;
            }

        /*
            Each line segment is saved for the application
            of constraint lines after the unconstrained
            triangulation is finished.
        */
            if (j > 0) {
                AddRawLineSeg(rptr-1, rptr, i);
            }
            n++;
            rptr++;
        }
        if (closed_flag == 1) {
            AddRawLineSeg (rptr, rpstart, i);
            n++;
        }
    }

    NumRawPoints = rptr - RawPoints;

/*
    Add the corners of the area to the point set.
*/
    dx = (xmax - xmin + ymax - ymin) / 100.0;
    AreaPerimeter = dx * 100.0;
    GrazeDistance = AreaPerimeter / 200000.0;
    AdjustDistance = GrazeDistance;
    FaultAdjustDistance = AdjustDistance * 10.0;
    ply_utils_obj.ply_setgraze (GrazeDistance);

    dx *= 5.0;
    xmin -= dx;
    ymin -= dx;
    xmax += dx;
    ymax += dx;

    CreateIndexGrid (xmin, ymin, xmax, ymax);

    rc1 = rptr - RawPoints;
    rptr->flag = CORNER_POINT;
    rptr->x = xmin;
    rptr->y = ymin;
    rptr->z = TRI_NO_VAL;
    rptr++;
    rc2 = rptr - RawPoints;
    rptr->flag = CORNER_POINT;
    rptr->x = xmin;
    rptr->y = ymax;
    rptr->z = TRI_NO_VAL;
    rptr++;
    rc3 = rptr - RawPoints;
    rptr->flag = CORNER_POINT;
    rptr->x = xmax;
    rptr->y = ymax;
    rptr->z = TRI_NO_VAL;
    rptr++;
    rc4 = rptr - RawPoints;
    rptr->flag = CORNER_POINT;
    rptr->x = xmax;
    rptr->y = ymin;
    rptr->z = TRI_NO_VAL;
    rptr++;

/*
    Allocate space for the initial triangle, edge and node lists.
*/
    n = NumRawPoints * 2;
    if (n < 100) n = 100;
    TriangleList = (TRiangleStruct *)csw_Calloc (n * sizeof(TRiangleStruct));
    if (!TriangleList) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    MaxTriangles = n;
    NumTriangles = 0;

    n *= 3;
    EdgeList = (EDgeStruct *)csw_Calloc (n * sizeof(EDgeStruct));
    if (!EdgeList) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    MaxEdges = n;
    NumEdges = 0;

    n = NumRawPoints + 100;
    MaxNodes = NumRawPoints + 100;
    NodeList = (NOdeStruct *)csw_Calloc (n * sizeof(NOdeStruct));
    if (!NodeList) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    NumNodes = 0;

/*
    Create nodes, edges, and triangles for the corner points and
    for two triangles arbitarily chosen to bisect the area.
*/
    rptr = RawPoints + rc1;
    nt = AddNode (rptr->x, rptr->y, rptr->z, rptr->flag);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    nptr = NodeList + nt;
    nptr->rp = rc1;
    rptr->nodenum = nptr - NodeList;

    rptr = RawPoints + rc2;
    nt = AddNode (rptr->x, rptr->y, rptr->z, rptr->flag);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    nptr = NodeList + nt;
    nptr->rp = rc2;
    rptr->nodenum = nptr - NodeList;

    rptr = RawPoints + rc3;
    nt = AddNode (rptr->x, rptr->y, rptr->z, rptr->flag);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    nptr = NodeList + nt;
    nptr->rp = rc3;
    rptr->nodenum = nptr - NodeList;

    rptr = RawPoints + rc4;
    nt = AddNode (rptr->x, rptr->y, rptr->z, rptr->flag);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    nptr = NodeList + nt;
    nptr->rp = rc4;
    rptr->nodenum = nptr - NodeList;

    nt = AddEdge (0, 1, 0, -1, BOUNDARY_EDGE);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    nt = AddEdge (1, 2, 0, -1, BOUNDARY_EDGE);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    nt = AddEdge (2, 3, 1, -1, BOUNDARY_EDGE);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    nt = AddEdge (3, 0, 1, -1, BOUNDARY_EDGE);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    nt = AddEdge (0, 2, 0, 1, 0);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }

    nt = AddTriangle (0, 1, 4, 0);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }
    nt = AddTriangle (2, 3, 4, 0);
    if (nt < 0) {
        FreeMem ();
        ConvexHullFlag = 0;
        return -1;
    }

/*
    Subdivide the starting triangles until all points have been used.
    This is where the work is done for the unconstrained triangulation.
    When this is finished, if there are no constraint lines, the task
    is finished.
*/
    NumCornerNodes = 4;
    istat = SubdivideTriangles ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        ConvexHullFlag = 0;
        NumCornerNodes = 0;
        return -1;
    }

    TriDebugFunc1 ();

/*
    Clean up artifacts from colinear points and points in
    almost exactly the same location.
*/
    RemoveZeroLengthEdges ();
    RemoveZeroAreaTriangles ();

    TriDebugFunc1 ();

/*
    If there are any constraint lines, these are added to the
    unconstrained triangulation here.
*/
    if (NumRawLines > 0) {

        istat = BuildRawPointEdgeLists();
        if (istat == -1) {
            grd_utils_ptr->grd_set_err(1);
            NumCornerNodes = 0;
            return -1;
        }

        istat = ApplyConstraints ();
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            FreeMem ();
            ConvexHullFlag = 0;
            NumCornerNodes = 0;
            return -1;
        }
        TriDebugFunc1 ();

        RemoveZeroLengthEdges ();
        RemoveZeroAreaTriangles ();
        TriDebugFunc1 ();

    }

    do_write = csw_GetDoWrite ();;
    if (do_write) {
        sprintf (fname1, "postconstraint.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

/*
 * If any node z values are GRD_SOFT_NULL_VALUE, remove the node
 * without leaving a tri mesh hole.
 */
    softchk = GRD_SOFT_NULL_VALUE / 100.0;
    FlagEdgeNodes ();
    for (i=0; i<NumNodes; i++) {
        if (NodeList[i].z <= softchk) {
            RemoveNode (i);
        }
    }

    ValidateEdgeIntersection (NULL);

/*
 * Divide long edges to make more regular triangles.
 */
    if (SplitLongFlag == 1) {
        istat = SplitLongEdges ();
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            FreeMem ();
            ConvexHullFlag = 0;
            NumCornerNodes = 0;
            return -1;
        }
    }

    if (do_write) {
        sprintf (filename, "preclip.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            filename);
    }

    if (_RemoveFlag == 1) {
        NodeList[0].deleted = 1;
        NodeList[1].deleted = 1;
        NodeList[2].deleted = 1;
        NodeList[3].deleted = 1;
        RemoveDeletedElements ();
    }

    ForceValidate = 0;
    ValidateTriangles ((char *)"From calc trimesh from points");
    ForceValidate = 0;

    ForceValidate = 0;
    ValidateEdgeIntersection ("From calc trimesh from points");
    ForceValidate = 0;

/*
 * Return the trimesh results.  The returned nodes will not be
 * in the same order as the input xyz points.
 */
    *num_nodes_out = NumNodes;
    *num_edges_out = NumEdges;
    *num_triangles_out = NumTriangles;

    *nodes_out = NodeList;
    *edges_out = EdgeList;
    *triangles_out = TriangleList;

    ListNull ();

    FreeMem ();

    ConvexHullFlag = 0;
    NumCornerNodes = 0;

    return 1;

}  /*  end of function grd_calc_trimesh  */



/*
 *****************************************************************************

         g r d _ r e m o v e _ n o d e s _ f r o m _ t r i m e s h

 *****************************************************************************

*/

int CSWGrdTriangle::grd_remove_nodes_from_trimesh (NOdeStruct **nodelist, int *numnodes,
                                   EDgeStruct **edgelist, int *numedges,
                                   TRiangleStruct **trilist, int *numtriangles,
                                   int *nodes_to_remove, int num_nodes_to_remove)
{
    int               i;
    RAwPointStruct    *rptr;
    int               do_write;
    char              fname[100];
    double            v6[6];

    ConstraintPointStart = 0;

    NodeList = *nodelist;
    EdgeList = *edgelist;
    TriangleList = *trilist;
    NumNodes = *numnodes;
    NumEdges = *numedges;
    NumTriangles = *numtriangles;
    MaxNodes = NumNodes;
    MaxEdges = NumEdges;
    MaxTriangles = NumTriangles;



    do_write = csw_GetDoWrite ();
    if (do_write) {
        sprintf (fname, "preremovefunc.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }



    RawPoints = (RAwPointStruct *) csw_Calloc (*numnodes * sizeof(RAwPointStruct));
    NumRawPoints = 0;
    MaxRawPoints = *numnodes;
    if (RawPoints == NULL) {
        return -1;
    }

    rptr = RawPoints;
    for (i=0; i<*numnodes; i++) {
        rptr->x = NodeList[i].x;
        rptr->y = NodeList[i].y;
        rptr->z = NodeList[i].z;
        rptr->nodenum = i;
        rptr->deleted = 0;
        rptr++;
        NodeList[i].rp = i;
        NodeList[i].crp = -1;
        NodeList[i].adjusting_node = -1;
        NodeList[i].spillnum = -1;
        NodeList[i].deleted = 0;
        NodeList[i].client_data = NULL;
    }

    BuildRawPointEdgeLists ();

    FlagEdgeNodes ();
    for (i=0; i<num_nodes_to_remove; i++) {
        RemoveNode (nodes_to_remove[i]);
    }

    RemoveDeletedElements ();

    *nodelist = NodeList;
    *edgelist = EdgeList;
    *trilist = TriangleList;
    *numnodes = NumNodes;
    *numedges = NumEdges;
    *numtriangles = NumTriangles;

    ListNull ();
    FreeMem ();

    return 1;

}  /* end of function grd_remove_nodes_from_trimesh */






/*
  ****************************************************************************

                       C r e a t e I n d e x G r i d

  ****************************************************************************

  Create a grid of INdexStruct pointers.  Each cell in the grid has a list
  of all the points that lie inside the cell.

*/

int CSWGrdTriangle::CreateIndexGrid (double xmin, double ymin, double xmax, double ymax)
{
    int              ntot, i, j, k, n, nc, nr, istat;
    int              nc2, nr2;
    double           w, h, a, tmp;
    RAwPointStruct   *rptr;
    INdexStruct      *iptr;

/*
    Calculate the grid geometry.
*/
    n = NumRawPoints;
    if (n < 10) n = 10;
    w = xmax - xmin;
    h = ymax - ymin;
    a = h / w;
    tmp = sqrt ((double)n / a);
    nc = (int)(tmp + .5);
    tmp = (double)nc * a;
    nr = (int)(tmp + .5);
    nc++;
    nr++;

/*
 * Bug 9930, make sure the colums and rows are always greater than 1.
 */
    if (nr < 2) nr = 2;
    if (nc < 2) nc = 2;

    if (AverageEdgeLength > 0.0) {
        nc2 = (int)((xmax - xmin) / AverageEdgeLength + 1.5);
        nr2 = (int)((ymax - ymin) / AverageEdgeLength + 1.5);
        if (nr2 < 2) nr2 = 2;
        if (nc2 < 2) nc2 = 2;
        if (nr2 >= nr/2  &&  nr2 <= nr*2  &&
            nc2 >= nc/2  &&  nc2 <= nc*2) {
            nr = nr2;
            nc = nc2;
        }
    }

    IndexXmin = xmin;
    IndexYmin = ymin;
    IndexXmax = xmax;
    IndexYmax = ymax;
    IndexNcol = nc;
    IndexNrow = nr;
    IndexXspace = (xmax - xmin) / (double)(nc - 1);
    IndexYspace = (ymax - ymin) / (double)(nr - 1);

    IndexXmax = IndexXmax;
    IndexYmax = IndexYmax;

/*
    Allocate space, initialized as NULLs.
*/
    if (IndexGrid == NULL) {
        IndexGrid = (INdexStruct **)csw_Calloc (nc*nr*sizeof(INdexStruct*));
        if (!IndexGrid) {
            FreeMem ();
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

/*
    Add each point to its appropriate cell in the index grid.
*/
    ntot = NumRawPoints;
    if (MaxIndexPoint > 0) {
        ntot = MaxIndexPoint;
    }
    NumSame = 0;
    for (n=0; n<ntot; n++) {
        rptr = RawPoints + n;
        i = (int)((rptr->y - ymin) / IndexYspace);
        j = (int)((rptr->x - xmin) / IndexXspace);
        if (i < 0  ||  i >= IndexNrow  ||
            j < 0  ||  j >= IndexNcol) {
            continue;
        }
        k = nc * i + j;
        iptr = IndexGrid[k];
        if (!iptr) {
            iptr = (INdexStruct *)csw_Calloc (sizeof(INdexStruct));
            if (!iptr) {
                FreeMem ();
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            IndexGrid[k] = iptr;
        }
        istat = AddIndexPoint (iptr, n);
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}  /*  end of private CreateIndexGrid function  */




int CSWGrdTriangle::AddToIndexGrid (int start, int end)
{
    int              i, j, k, n, istat;
    RAwPointStruct   *rptr;
    INdexStruct      *iptr;

    if (IndexGrid == NULL) {
        assert (0);
    }

/*
    Add each point to its appropriate cell in the index grid.
*/
    for (n=start; n<end; n++) {
        rptr = RawPoints + n;
        i = (int)((rptr->y - IndexYmin) / IndexYspace);
        j = (int)((rptr->x - IndexXmin) / IndexXspace);
        if (i < 0  ||  i >= IndexNrow  ||
            j < 0  ||  j >= IndexNcol) {
            continue;
        }
        k = IndexNcol * i + j;
        iptr = IndexGrid[k];
        if (!iptr) {
            iptr = (INdexStruct *)csw_Calloc (sizeof(INdexStruct));
            if (!iptr) {
                FreeMem ();
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            IndexGrid[k] = iptr;
        }
        istat = AddIndexPoint (iptr, n);
        if (istat == -1) {
            return -1;
        }
    }

    return 1;
}





/*
  ****************************************************************************

                        A d d I n d e x P o i n t

  ****************************************************************************

    Add a point to the specifed index structure, expanding its list if needed.

*/

int CSWGrdTriangle::AddIndexPoint (INdexStruct *iptr, int np)
{
    int                i, nt, n, m, *list;
    RAwPointStruct     *rp1, *rp2;

    n = iptr->npts;
    m = iptr->max;
    list = iptr->list;
    if (n >= m) {
        m += TRI_INDEX_CHUNK;
        list = (int *)csw_Realloc (list, m * sizeof(int));
        if (!list) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        iptr->max = m;
        iptr->list = list;
    }

    for (i=0; i<n; i++) {
        nt = iptr->list[i];
        if (SameRawPoint (nt, np)) {
            rp1 = RawPoints + nt;
            rp2 = RawPoints + np;
            rp2->nodenum = rp1->nodenum;
            rp2->x = rp1->x;
            rp2->y = rp1->y;
            rp2->z = rp1->z;
            return 1;
        }
    }

    list[n] = np;
    iptr->npts++;

    return 1;

}  /*  end of private AddIndexPoint function  */




/*
  ****************************************************************************

                       G e t I n d e x P o i n t X Y

  ****************************************************************************

  Check the specified cell in the point index for a point closer than the
  specified mindist from the private XCenter, YCenter point.  If a closer
  point is found, its list location is returned.  If no closer point is
  found, -1 is returned.  The point also needs to be inside the specified trinum.
*/

int CSWGrdTriangle::GetIndexPointXY (int index, int trinum, double *distmin)

{
    INdexStruct     *iptr;
    RAwPointStruct  *rptr;
    double          x, y;
    int             istat, n, i, j, k, *list;
    double          dx, dy, dist, dmin;
    int             nclose;

    dmin = *distmin;
    nclose = -1;

    k = index;
    iptr = IndexGrid[k];

    if (!iptr) return -1;

    list = iptr->list;
    n = iptr->npts;
    for (i=0; i<n; i++) {
        j = list[i];
        rptr = RawPoints + j;
        if (rptr->flag == 0  &&  rptr->nodenum < 0) {
            x = rptr->x;
            y = rptr->y;
            istat = PointInTriangle (x, y, trinum);
            if (istat >= 0) {
                dx = x - XCenter;
                dy = y - YCenter;
                dist = dx * dx + dy * dy;
                dist = sqrt (dist);
                if (dist < dmin) {
                    dmin = dist;
                    nclose = j;
                }
            }
        }
    }

    if (nclose >= 0) {
        *distmin = dmin;
        return nclose;
    }

    return -1;

}  /*  end of private GetIndexPointXY function  */








/*
  ****************************************************************************

                    S u b d i v i d e T r i a n g l e s

  ****************************************************************************

    Refine the triangles until all points have been used.  For each triangle,
  if an unused point is found inside it, it is subdivided into 3 smaller
  triangles.  After one point per triangle has been done, the edges are
  swapped to make the most equilateral solution at each stage of the process.

*/

int CSWGrdTriangle::SubdivideTriangles (void)
{
    int   i, istat, idone, ndone, ntri, nedge, np, ndo, ndomax;
    int   jdo, nswap, last_nswap;



/*
 *  !!!! debug only
 */
    char            fname1[200], fname2[200];
    int             do_write;
    double          v6[6];
    FILE            *fptr;
    char            line[200];

    do_write = csw_GetDoWrite ();;
    if (do_write) {
        fptr = fopen ("rawpoints.xyz", "wb");
        if (fptr) {
            for (i=0; i<NumRawPoints; i++) {
                sprintf (line, "%f %f 0.0\n",
                         RawPoints[i].x,
                         RawPoints[i].y);
                fputs (line, fptr);
            }
            fclose (fptr);
            fptr = NULL;
        }
    }




/*
    Run the loop until all the points are done, with a debug check
    for number of iterations also.
*/
    ndo = 0;
    ndomax = NumRawPoints - NumSame - 1;
    if (ndomax < MAX_ITER_TRI) ndomax = MAX_ITER_TRI;
    ndone = 0;
    UseCornerFlag = 1;
    CornerBias = 1.0;
    for (;;) {

        idone = 0;

        nedge = NumEdges + NumTriangles * 3;
        SwapFlags = (char *)csw_Calloc (nedge * sizeof(char));
        if (SwapFlags == NULL) {
            grd_utils_ptr->grd_set_err(1);
            return -1;
        }

    /*
        Try and find a point inside or on the edge of each triangle
        that is currently available.  Use the point to split the
        triangle into 3 parts if a point is found.  Note that the
        private NumTriangles and NumEdges values will get bigger
        during this process, so their values at the start are used
        for the loop counters.
    */
        ntri = NumTriangles;
        nedge = NumEdges;
        for (i=0; i<ntri; i++) {
            np = FindPointInTriangle (i);
            if (np < 0) {
                continue;
            }
            istat = ExpandMem ();
            if (istat == -1) {
                return -1;
            }
            istat = SplitTriangle (i, np);
            if (istat == -2) {
                RawPoints[np].flag = 0;
                continue;
            }
            if (istat == -1) {
                return -1;
            }
            ndone++;
            idone++;
        }

        if (do_write) {
            sprintf (fname1, "preswap%d.tri", ndo);
            grd_WriteTextTriMeshFile (
                0, v6,
                TriangleList, NumTriangles,
                EdgeList, NumEdges,
                NodeList, NumNodes,
                fname1);
        }

    /*
        Swap the edges that were in place prior to this iteration
        if they would make more equilateral triangles by doing so.
    */
        jdo = 0;
        last_nswap = NumEdges * 2;
        memset (SwapFlags, 0, nedge * sizeof(char));
        for (;;) {
            nswap = 0;
            for (i=0; i<NumEdges; i++) {
                if (SwapFlags[i] == 1) continue;
                istat = SwapEdge (i);
                if (istat == 1) {
                    SwapFlags[i] = 1;
                    nswap++;
                }
            }
            if (nswap == 0) {
                break;
            }

        /*
         * Bug 521 fix.
         * If the number of swaps increases or stays the same,
         * get out of the swap loop.
         */
            if (nswap >= last_nswap) {
                break;
            }
            last_nswap = nswap;
            jdo++;
            if (jdo > nedge) {
                break;
            }
            memset (SwapFlags, 0, nedge * sizeof(char));
        }

        if (do_write) {
            sprintf (fname2, "postswap%d.tri", ndo);
            grd_WriteTextTriMeshFile (
                0, v6,
                TriangleList, NumTriangles,
                EdgeList, NumEdges,
                NodeList, NumNodes,
                fname2);
        }

        csw_Free (SwapFlags);
        SwapFlags = NULL;

    /*
        Break the loop if all points are done, or if no points were
        done in this iteration (a bug) or if the max number of iterations
        is exceeded (another bug).
    */
        if (idone < 1) {
            if (UseCornerFlag == 1) {
                break;
            }
            else {
                UseCornerFlag = 1;
                continue;
            }
        }
        if (ndone >= NumRawPoints - NumSame) {
            break;
        }
        ndo++;
        if (ndo > ndomax) {
            FatalMessage ((char *)"Bug in SubdivideTriangles");
            break;
        }
    }

/*
   Swap edges where needed prior to applying constraint edge
*/
    SwapDebug = SwapDebug;

    SwapFlags = (char *)csw_Calloc (NumEdges * sizeof(char));
    FinalSwapFlag = 1;
    CornerBias = CORNER_BIAS;
    ndo = 0;
    for (;;) {
        NumSwapped = 0;
        for (i=0; i<NumEdges; i++) {
            if (SwapFlags[i] >= 1) continue;
            SwapEdge (i);
        }
        if (NumSwapped == 0) {
            break;
        }
        for (i=0; i<NumEdges; i++) {
            if (SwapFlags[i] != 2) SwapFlags[i] = 0;
        }
        if (do_write) {
            sprintf (fname2, "finalswap%d.tri", ndo);
            grd_WriteTextTriMeshFile (
                0, v6,
                TriangleList, NumTriangles,
                EdgeList, NumEdges,
                NodeList, NumNodes,
                fname2);
        }

        ndo++;
    }
    csw_Free (SwapFlags);
    SwapFlags = NULL;
    FinalSwapFlag = 0;

    if (do_write) {
        sprintf (fname2, "precorner%d.tri", ndo);
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname2);
    }

    if (ConvexHullFlag) {
        for (i=0; i<NumEdges; i++) {
            SwapCornerEdge (i);
        }
    }

    if (do_write) {
        sprintf (fname2, "postcorner%d.tri", ndo);
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname2);
    }

    return 1;

}  /*  end of private SubdivideTriangle function  */







/*
  ****************************************************************************

                        S p l i t T r i a n g l e

  ****************************************************************************

    Given a triangle number and the number of a raw point inside the triangle,
  create three new triangles by connecting the raw point to each of the corners
  of the existing triangle.

*/

int CSWGrdTriangle::SplitTriangle (int triangle_num, int point_num)
{
    int                   istat, iszero;
    int                   e1, e2, e3;
    int                   newnode, trinum;
    int                   newe1, newe2, newe3;
    int                   newt1, newt2;
    NOdeStruct            *nptr;
    RAwPointStruct        *rptr;
    EDgeStruct            *newep1, *newep2, *newep3,
                          *oldep1, *oldep2, *oldep3;
    TRiangleStruct        *tptr;


/*
    Get the edges of the specified triangle.
*/
    tptr = TriangleList + triangle_num;
    e1 = tptr->edge1;
    e2 = tptr->edge2;
    e3 = tptr->edge3;
    if (SwapFlags) {
        SwapFlags[e1] = 1;
        SwapFlags[e2] = 1;
        SwapFlags[e3] = 1;
    }

/*
    Add the specified point as a new node.
*/
    rptr = RawPoints + point_num;
    newnode = AddNode (rptr->x, rptr->y, rptr->z, rptr->flag);
    if (newnode < 0) {
        return -1;
    }
    tptr = TriangleList + triangle_num;
    nptr = NodeList + newnode;
    nptr->rp = point_num;
    rptr->nodenum = newnode;

/*
    If the new node is almost exactly on an edge of the triangle,
    use that edge to split each of its two adjoining triangles into
    two smaller triangles.
*/
    SnapToEdge = 0;
    newe1 = NodeOnEdge (triangle_num, newnode);
    SnapToEdge = 1;
    if (newe1 >= 0) {
        istat = SplitFromEdge (newe1, newnode, NULL);
        return istat;
    }

/*
    Create three new edges to connect the specified
    point to the corners of the specified triangle.
*/
    newe1 = AddEdge (0, 0, -1, -1, 0);
    newe2 = AddEdge (0, 0, -1, -1, 0);
    newe3 = AddEdge (0, 0, -1, -1, 0);
    if (newe3 < 0) {
        return -1;
    }

    newep1 = EdgeList + newe1;
    newep2 = EdgeList + newe2;
    newep3 = EdgeList + newe3;
    oldep1 = EdgeList + e1;
    oldep2 = EdgeList + e2;
    oldep3 = EdgeList + e3;

    newep1->node1 = oldep1->node1;
    newep1->node2 = newnode;
    newep2->node1 = oldep1->node2;
    newep2->node2 = newnode;
    if (oldep2->node1 != oldep1->node1  &&  oldep2->node1 != oldep1->node2) {
        newep3->node1 = oldep2->node1;
    }
    else {
        newep3->node1 = oldep2->node2;
    }
    newep3->node2 = newnode;
    newep1->length = NodeDistance (newep1->node1, newep1->node2);
    newep2->length = NodeDistance (newep2->node1, newep2->node2);
    newep3->length = NodeDistance (newep3->node1, newep3->node2);

/*
    The first new triangle will use the same memory as the
    specified input triangle.  The first edge in the input
    triangle is combined with the new edges that use the
    same nodes.
*/
    tptr = TriangleList + triangle_num;
    tptr->edge2 = newep1 - EdgeList;
    newep1->tri1 = tptr - TriangleList;
    tptr->edge3 = newep2 - EdgeList;
    newep2->tri1 = tptr - TriangleList;

    iszero = 0;
    iszero = iszero;

/*
    The second triangle is appended to the triangle list.  It uses the
    second edge of the input triangle and the two new edges that share
    nodes with this second edge.
*/
    newt1 = AddTriangle (0, 0, 0, 0);
    if (newt1 < 0) {
        return -1;
    }
    tptr = TriangleList + newt1;
    newep1 = EdgeList + newe1;
    newep2 = EdgeList + newe2;
    newep3 = EdgeList + newe3;
    oldep1 = EdgeList + e1;
    oldep2 = EdgeList + e2;
    oldep3 = EdgeList + e3;
    trinum = tptr - TriangleList;
    if (oldep2->tri1 == triangle_num) {
        oldep2->tri1 = trinum;
    }
    else {
        oldep2->tri2 = trinum;
    }
    tptr->edge1 = oldep2 - EdgeList;
    tptr->edge2 = -1;
    tptr->edge3 = -1;

    if (newep1->node1 == oldep2->node1  ||  newep1->node1 == oldep2->node2) {
        tptr->edge2 = newep1 - EdgeList;
        newep1->tri2 = trinum;
    }
    if (newep2->node1 == oldep2->node1  ||  newep2->node1 == oldep2->node2) {
        if (tptr->edge2 == -1) {
            tptr->edge2 = newep2 - EdgeList;
        }
        else {
            tptr->edge3 = newep2 - EdgeList;
        }
        newep2->tri2 = trinum;
    }
    if (tptr->edge3 == -1) {
        if (newep3->node1 == oldep2->node1  ||  newep3->node1 == oldep2->node2) {
            tptr->edge3 = newep3 - EdgeList;
        }
        newep3->tri1 = trinum;
    }

/*
    The third triangle is appended to the triangle list.  It uses the
    third edge of the input triangle and the two new edges that share
    nodes with this second edge.
*/
    newt2 = AddTriangle (0, 0, 0, 0);
    if (newt2 < 0) {
        return -1;
    }
    tptr = TriangleList + newt2;
    newep1 = EdgeList + newe1;
    newep2 = EdgeList + newe2;
    newep3 = EdgeList + newe3;
    oldep1 = EdgeList + e1;
    oldep2 = EdgeList + e2;
    oldep3 = EdgeList + e3;
    trinum = tptr - TriangleList;
    if (oldep3->tri1 == triangle_num) {
        oldep3->tri1 = trinum;
    }
    else {
        oldep3->tri2 = trinum;
    }
    tptr->edge1 = oldep3 - EdgeList;
    tptr->edge2 = -1;
    tptr->edge3 = -1;

    if (newep1->node1 == oldep3->node1  ||  newep1->node1 == oldep3->node2) {
        tptr->edge2 = newep1 - EdgeList;
        newep1->tri2 = trinum;
    }
    if (newep2->node1 == oldep3->node1  ||  newep2->node1 == oldep3->node2) {
        if (tptr->edge2 == -1) {
            tptr->edge2 = newep2 - EdgeList;
        }
        else {
            tptr->edge3 = newep2 - EdgeList;
        }
        newep2->tri2 = trinum;
    }
    if (tptr->edge3 == -1) {
        if (newep3->node1 == oldep3->node1  ||  newep3->node1 == oldep3->node2) {
            tptr->edge3 = newep3 - EdgeList;
        }
        newep3->tri2 = trinum;
    }

    return 1;

}  /*  end of private SplitTriangle function  */



void CSWGrdTriangle::ExtendVectors (double *x1, double *y1,
                           double *x2, double *y2,
                           double *x3, double *y3,
                           double *x4, double *y4)
{
    double      dx, dy;
    double      ang, tiny, cang, sang;

    tiny = GrazeDistance * 100.0;

    dx = *x2 - *x1;
    dy = *y2 - *y1;
    ang = atan2 (dy, dx);
    cang = cos (ang);
    sang = sin (ang);
    *x2 += tiny * cang;
    *y2 += tiny * sang;
    *x1 -= tiny * cang;
    *y1 -= tiny * sang;

    dx = *x4 - *x3;
    dy = *y4 - *y3;
    ang = atan2 (dy, dx);
    cang = cos (ang);
    sang = sin (ang);
    *x4 += tiny * cang;
    *y4 += tiny * sang;
    *x3 -= tiny * cang;
    *y3 -= tiny * sang;

    return;

}


void CSWGrdTriangle::ShrinkVector2 (double x1, double y1,
                           double *x2, double *y2)
{
    double      dx, dy;
    double      ang, tiny, cang, sang;

    tiny = GrazeDistance * 100.0;

    dx = *x2 - x1;
    dy = *y2 - y1;
    ang = atan2 (dy, dx);
    cang = cos (ang);
    sang = sin (ang);
    *x2 -= tiny * cang;
    *y2 -= tiny * sang;

}




/*
  ****************************************************************************

                           S w a p E d g e

  ****************************************************************************

    If the edge specified has two triangles that share it, then the
    two triangles form a quadrilateral with the specified edge being
    one of the two possible diagonals of the quadrilateral.  This
    function changes the edge to the other possible diagonal and
    updates the two triangles to reflect the new topology.  The swap
    is only done if the resulting least equilateral triangle is better
    after the swap.

*/

int CSWGrdTriangle::SwapEdge (int edgenum)
{
    TRiangleStruct         *tp1, *tp2;
    EDgeStruct             *ep, *ep3, *ep4, *eptmp;
    int                    e1, e2, e3, e4,
                           n1, n2, n3, n4,
                           tnum1, tnum2, istat, swapit;
    double                 xint, yint;
    double                 eq1, eq2, eq3, eq4, eq11, eq22, badeq;
    double                 worsteq1, worsteq2;
    int                    hull_flag;
    double                 x1, y1, x2, y2, x3, y3, x4, y4;
    double                 xa[5], ya[5], xmid, ymid;
    int                    force;

    CSWPolyUtils        ply_utils_obj;

    hull_flag = 0;
    if (ConvexHullFlag == 1  &&  FinalSwapFlag == 1) {
        hull_flag = 1;
    }

/*
    Do nothing if the edge has only one triangle using it
    or if the edge swap flags are not enabled.
*/
    if (EdgeSwapFlag == GRD_SWAP_NONE) {
        return 0;
    }

    ep = EdgeList + edgenum;
    if (ep->deleted == 1) {
        return 0;
    }
    if (ep->tri2 == -1) {
        return 0;
    }

    if (EdgeSwapFlag == GRD_SWAP_AS_FLAGGED  &&
        ep->flag == GRD_DONT_SWAP_FLAG) {
        return 0;
    }

    if (ep->tri1 == -1) {
        ep->tri1 = -1;
    }

/*
    Find the two edges of the first triangle that are not the specified edge.
*/
    tp1 = TriangleList + ep->tri1;
    tp2 = TriangleList + ep->tri2;

    e1 = e2 = e3 = e4 = -1;

    if (tp1->edge1 != edgenum) {
        e1 = tp1->edge1;
    }
    if (tp1->edge2 != edgenum) {
        if (e1 == -1) {
            e1 = tp1->edge2;
        }
        else {
            e2 = tp1->edge2;
        }
    }
    if (tp1->edge3 != edgenum) {
        e2 = tp1->edge3;
    }

/*
    Find the nodes of the first triangle, in the same order as the edges.
*/
    n1 = CommonNode (edgenum, e1);
    n2 = CommonNode (e1, e2);
    n3 = CommonNode (e2, edgenum);

/*
    Find the edges of the second triangle that are not the diagonal and
    find the remaining node of the quadrilateral.
*/
    if (tp2->edge1 != edgenum) {
        e3 = tp2->edge1;
    }
    if (tp2->edge2 != edgenum) {
        if (e3 == -1) {
            e3 = tp2->edge2;
        }
        else {
            e4 = tp2->edge2;
        }
    }
    if (tp2->edge3 != edgenum) {
        e4 = tp2->edge3;
    }

    ep3 = EdgeList + e3;
    ep4 = EdgeList + e4;

    n4 = CommonNode (e3, e4);

/*
    The edge e3 must be connected to edge e2.  If not, swap
    e3 and e4 edges.  When this is done, the edges e1, e2, e3, to e4
    represent a continuous perimeter of the quadrilateral.  The nodes
    n1, n2, n3, and n4 are the nodes of the perimeter, in order.
*/
    if (CommonNode (e2, e3) == -1) {
        eptmp = ep3;
        ep3 = ep4;
        ep4 = eptmp;
        e3 = ep3 - EdgeList;
        e4 = ep4 - EdgeList;
    }

/*
    If the two possible diagonal segments do not intersect, there
    are two possible scenarios.  If the candidate edge for swapping is
    outside of the quadralateral defined by its adjoining triangles,
    the edge should always be swapped.  If the candidate edge is inside,
    the edge should never be swapped.
*/

  /*
   * Get the endpoints of the two possible diagonals.
   */
    x1 = NodeList[n1].x;
    y1 = NodeList[n1].y;
    x2 = NodeList[n3].x;
    y2 = NodeList[n3].y;
    x3 = NodeList[n2].x;
    y3 = NodeList[n2].y;
    x4 = NodeList[n4].x;
    y4 = NodeList[n4].y;

  /*
   * Make the outline of the quadralateral and check if the
   * mid point of the candidate edge is inside the outline.
   * If it is not inside, and the midpoint of the alternate
   * diagonal is inside, force swapping.  If both midpoints
   * are outside, return without swapping.
   */
    xa[0] = x1;
    ya[0] = y1;
    xa[1] = x3;
    ya[1] = y3;
    xa[2] = x2;
    ya[2] = y2;
    xa[3] = x4;
    ya[3] = y4;
    xa[4] = x1;
    ya[4] = y1;

    xmid = (x1 + x2) / 2.0;
    ymid = (y1 + y2) / 2.0;

    force = 0;
    istat = ply_utils_obj.ply_point (xa, ya, 5, xmid, ymid);
    if (istat != 1) {

    /*
     * Check the alternate diagonal.  Return if its midpoint
     * is also outside the quadralateral.
     */
        xmid = (x3 + x4) / 2.0;
        ymid = (y3 + y4) / 2.0;
        istat = ply_utils_obj.ply_point (xa, ya, 5, xmid, ymid);
        if (istat != 1) {
            if (FinalSwapFlag == 1) {
                if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
                if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
                if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
                if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
                SwapFlags[edgenum] = 2;
                NumSwapped++;
            }
            return 0;
        }

        force = 1;

        goto FORCE_SWAPPING;
    }

  /*
   * The midpoint is inside the quadralateral outline, so check the
   * intersection of the two diagonals.  If they do not intersect,
   * do not swap.
   */
    if (force == 0) {
        if (ExtendDiagonalsFlag) {
            ExtendVectors (&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
        }
        istat = ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4,
                            &xint, &yint);
        if (istat != 0) {
            if (FinalSwapFlag == 1) {
                if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
                if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
                if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
                if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
                SwapFlags[edgenum] = 2;
                NumSwapped++;
            }
            return 0;
        }
    }

/*
    Calculate the "equilateralness" of the two triangles defined
    by the current edge orientation and by the swapped edge
    orientation.  If the new triangles are "more equilateral"
    than the previous triangles, swap the edge.

    If either edge uses a contrived corner point, then the
    equilateralness of the triangles using that edge are
    lowered to make it more difficult for the corner point
    to be used in a final triangle.
*/
    eq1 = CalcEquilateralness (n2, n1, n3);
    eq2 = CalcEquilateralness (n4, n1, n3);
    eq3 = CalcEquilateralness (n1, n2, n4);
    eq4 = CalcEquilateralness (n3, n2, n4);

    badeq = 1.e-20;
    if (eq1 < badeq) eq1 = 0.0;
    if (eq2 < badeq) eq2 = 0.0;
    if (eq3 < badeq) eq3 = 0.0;
    if (eq4 < badeq) eq4 = 0.0;

    if (n1 < NumCornerNodes  ||  n3 < NumCornerNodes) {
        eq1 /= CornerBias;
        eq2 /= CornerBias;
        if (hull_flag == 1) {
            eq1 = 0.0;
            eq2 = 0.0;
        }
    }
    if (n2 < NumCornerNodes  ||  n4 < NumCornerNodes) {
        eq3 /= CornerBias;
        eq4 /= CornerBias;
        if (hull_flag == 1) {
            eq3 = 0.0;
            eq4 = 0.0;
        }
    }

/*
    This section uses the equlateralness of the two
    possible sets of output triangles to decide if
    the edge should be swapped.  If either of the
    current triangles have equlateralness much worse
    than the alternative average equilateralness, then
    the swap is done.  If there is no wildly disparate
    equilateralness  between any triangles, then the
    pair with the largest average equilateralness is
    used.  For the case where both alternatives have
    an undesirable triangle, it doesn't really matter
    which is chosen, so I arbitarily do not swap in
    this case.
*/
    swapit = 0;

    eq11 = eq1 + eq2;
    eq22 = eq3 + eq4;
    worsteq1 = eq1;
    if (eq2 < eq1) worsteq1 = eq2;
    worsteq2 = eq3;
    if (eq4 < eq3) worsteq2 = eq4;

    if (eq3 < eq11 / 20.0  ||
        eq4 < eq11 / 20.0) {
        swapit = 0;
    }
    else if (eq1 < eq22 / 20.0  ||
             eq2 < eq22 / 20.0) {
        swapit = 1;
    }
    else if (worsteq2 > 1.1 * worsteq1) {
        swapit = 1;
    }
    else if (eq22 > 1.01 * eq11) {
        swapit = 1;
    }

/*
    If either current equilateralness (eq1 or eq2) is zero,
    the current alternative has a zero area triangle.  This
    must be swapped out.
*/
    if (swapit == 0) {
        if (eq1 == 0.0  ||  eq2 == 0.0) {
            swapit = 1;
        }
    }

    if (EdgeSwapFlag == GRD_SWAP_AS_FLAGGED) {
        swapit = 1;
    }
    if (EdgeSwapFlag == SWAP_FOR_NULL_REMOVAL) {
        swapit = 1;
        if (NodeList[n2].z > 1.e20  ||
            NodeList[n4].z > 1.e20) {
            swapit = 0;
        }
    }
    if (EdgeSwapFlag == FORCE_SWAP) {
        swapit = 1;
    }

    if (force == 1) {
        swapit = 1;
    }

    if (swapit == 0) {
        if (FinalSwapFlag == 1) {
            if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
            if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
            if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
            if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
            SwapFlags[edgenum] = 2;
            NumSwapped++;
        }
        return 0;
    }

  FORCE_SWAPPING:

/*
    The old diagonal was by definition from n1 to n3, so the
    new diagonal must be from n2 to n4.
*/
    ep->node1 = n2;
    ep->node2 = n4;
    ep->length = NodeDistance (n2, n4);

/*
    Remove the edge from the edge lists of n1 and n3, also
    add the edge to the n2 and n4 node lists.
*/
    if (RawPoints != NULL) {
        RemoveEdgeFromNodeList (n1, edgenum);
        RemoveEdgeFromNodeList (n3, edgenum);
        AddEdgeToNodeList (n2, edgenum);
        AddEdgeToNodeList (n4, edgenum);
    }

/*
    Update the triangles with their new edges.
*/
    tp1->edge1 = e1;
    tp1->edge2 = edgenum;
    tp1->edge3 = e4;

    tp2->edge1 = e2;
    tp2->edge2 = e3;
    tp2->edge3 = edgenum;

/*
    Update the triangles for the edges.
*/
    tnum1 = tp1 - TriangleList;
    tnum2 = tp2 - TriangleList;
    eptmp = EdgeList + e4;
    if (eptmp->tri1 == tnum2) {
        eptmp->tri1 = tnum1;
    }
    else {
        eptmp->tri2 = tnum1;
    }
    eptmp = EdgeList + e2;
    if (eptmp->tri1 == tnum1) {
        eptmp->tri1 = tnum2;
    }
    else {
        eptmp->tri2 = tnum2;
    }

    if (FinalSwapFlag == 1) {
        if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
        if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
        if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
        if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
        SwapFlags[edgenum] = 2;
        NumSwapped++;
    }

    return 1;

}  /*  end of private SwapEdge function  */






/*
  ****************************************************************************

                          C o m m o n N o d e

  ****************************************************************************

    Return the node number that is common to both specified edge numbers.
    If no node is common, return -1.

*/

int CSWGrdTriangle::CommonNode (int e1, int e2)
{
    EDgeStruct         *ep1, *ep2;
    int                result;

    if (e1 < 0  ||  e2 < 0) {
        return -1;
    }

    result = -1;
    ep1 = EdgeList + e1;
    ep2 = EdgeList + e2;
    if (ep1->node1 == ep2->node1  ||
        ep1->node1 == ep2->node2) {
        result = ep1->node1;
    }
    else if (ep1->node2 == ep2->node1  ||
             ep1->node2 == ep2->node2) {
        result = ep1->node2;
    }

    return result;

}  /*  end of private CommonNode function  */



/*
 *****************************************************************************

                      T r i a n g l e P o i n t s

 *****************************************************************************

  Return the x, y, z points for the specified triangle.

*/

void CSWGrdTriangle::TrianglePoints (TRiangleStruct *tptr,
                            double *x,
                            double *y,
                            double *z)
{
    EDgeStruct         *eptr;
    NOdeStruct         *nptr;
    int                n1, n2, n3;

    eptr = EdgeList + tptr->edge1;
    n1 = eptr->node1;
    nptr = NodeList + n1;
    x[0] = nptr->x;
    y[0] = nptr->y;
    z[0] = nptr->z;
    n2 = eptr->node2;
    nptr = NodeList + n2;
    x[1] = nptr->x;
    y[1] = nptr->y;
    z[1] = nptr->z;

    eptr = EdgeList + tptr->edge2;
    if (eptr->node1 == n1  ||  eptr->node1 == n2) {
        n3 = eptr->node2;
    }
    else {
        n3 = eptr->node1;
    }

    nptr = NodeList + n3;
    x[2] = nptr->x;
    y[2] = nptr->y;
    z[2] = nptr->z;

    return;

}  /* end of private TrianglePoints function */



/*
  ****************************************************************************

                     N o d e D i s t a n c e X Y Z

  ****************************************************************************

  Calculate the 3D distance between two nodes.  No checking is done to see
  if a node is in range.  The actual distance, rather than the square of
  the distance is returned.  Use NodeDistanceSquared for the square of the
  distance.

*/

double CSWGrdTriangle::NodeDistanceXYZ (int n1, int n2)

{
    NOdeStruct         *nptr;
    double             x1, y1, x2, y2, z1, z2, dx, dy, dz, dt, dist;

    nptr = NodeList + n1;
    x1 = nptr->x;
    y1 = nptr->y;
    z1 = nptr->z;
    nptr = NodeList + n2;
    x2 = nptr->x;
    y2 = nptr->y;
    z2 = nptr->z;

    dx = x2 - x1;
    dy = y2 - y1;
    if (z1 > 1.e20  ||  z2 > 1.e20) {
        dz = 0.0;
    }
    else if (z1 < -1.e19  ||  z2 < -1.e19) {
        dz = 0.0;
    }
    else {
        dz = z2 - z1;
    }
    dz = 0.0;
    dt = dx * dx + dy * dy + dz * dz;
    dist = sqrt(dt);

    return dist;

}  /*  end of private NodeDistanceXYZ function  */




/*
  ****************************************************************************

                         N o d e D i s t a n c e

  ****************************************************************************

  Calculate the distance between two nodes.  No checking is done to see
  if a node is in range.  The actual distance, rather than the square of
  the distance is returned.  Use NodeDistanceSquared for the square of the
  distance.

*/

double CSWGrdTriangle::NodeDistance (int n1, int n2)
{
    NOdeStruct         *nptr;
    double             x1, y1, x2, y2, dx, dy, dt, dist;

    nptr = NodeList + n1;
    x1 = nptr->x;
    y1 = nptr->y;
    nptr = NodeList + n2;
    x2 = nptr->x;
    y2 = nptr->y;

    dx = x2 - x1;
    dy = y2 - y1;
    dt = dx * dx + dy * dy;
    dist = sqrt(dt);

    return dist;

}  /*  end of private NodeDistance function  */





/*
  ****************************************************************************

                   F i n d P o i n t I n T r i a n g l e

  ****************************************************************************

    Use the point index grid to find a point inside the specified triangle.
    The "center point" of the triangle is used as the center for an indexed
    search for a point.  The search widens until either a point is found or
    until all points in the triangle bounding box have been searched.  If
    no point is found, -1 is returned.  If a point is found, its offset in
    the RawPoints array is returned.

*/

int CSWGrdTriangle::FindPointInTriangle (int trinum)
{
    EDgeStruct        *eptr;
    NOdeStruct        *nptr;
    TRiangleStruct    *tptr;

    double            xc, yc;
    double            x1, y1, x2, y2;
    double            dx, dy;
    int               np, offset, ixc, iyc;
    double            xmid, ymid, distmin;
    int               i1, i2, j1, j2, it1, it2, jt1, jt2;
    int               ixphase, iyphase;
    int               rowanchor, jj, ii, kk, nclose, ncheck;
    int               ndo, maxndo;

    tptr = TriangleList + trinum;

/*
 * Average the x and y coordinates of the mid points of
 * each triangle edge.  This is a pretty close estimate
 * of the center of the triangle.
 */
    xc = 0.0;
    yc = 0.0;

    eptr = EdgeList + tptr->edge1;
    nptr = NodeList + eptr->node1;
    x1 = nptr->x;
    y1 = nptr->y;
    nptr = NodeList + eptr->node2;
    x2 = nptr->x;
    y2 = nptr->y;
    xmid = (x1 + x2) / 2.0;
    ymid = (y1 + y2) / 2.0;
    xc += xmid;
    yc += ymid;

    eptr = EdgeList + tptr->edge2;
    nptr = NodeList + eptr->node1;
    x1 = nptr->x;
    y1 = nptr->y;
    nptr = NodeList + eptr->node2;
    x2 = nptr->x;
    y2 = nptr->y;
    xmid = (x1 + x2) / 2.0;
    ymid = (y1 + y2) / 2.0;
    xc += xmid;
    yc += ymid;

    eptr = EdgeList + tptr->edge3;
    nptr = NodeList + eptr->node1;
    x1 = nptr->x;
    y1 = nptr->y;
    nptr = NodeList + eptr->node2;
    x2 = nptr->x;
    y2 = nptr->y;
    xmid = (x1 + x2) / 2.0;
    ymid = (y1 + y2) / 2.0;
    xc += xmid;
    yc += ymid;

    xc /= 3.0;
    yc /= 3.0;

    XCenter = xc;
    YCenter = yc;

/*
 * Find the row and column containing the triangle center point.
 */
    iyc = (int)((yc - IndexYmin) / IndexYspace);
    ixc = (int)((xc - IndexXmin) / IndexXspace);

/*
 * Find the Bounding box of the triangle.
 */
    TriangleBounds (trinum, &x1, &y1, &x2, &y2);
    x1 -= IndexXspace / 2.0;
    y1 -= IndexYspace / 2.0;
    x2 += IndexXspace / 2.0;
    y2 += IndexYspace / 2.0;

    jt1 = (int)((x1 - IndexXmin) / IndexXspace);
    jt2 = (int)((x2 - IndexXmin) / IndexXspace);
    it1 = (int)((y1 - IndexYmin) / IndexYspace);
    it2 = (int)((y2 - IndexYmin) / IndexYspace);
    jt1--;
    it1--;
    jt2++;
    it2++;
    jt1--;
    it1--;
    jt2++;
    it2++;

/*
 * If the bounding box is completely outside the
 * index, which can happen when adding a constraint line
 * to an existing trimesh, return -1, meaning no point can
 * be found inside the triangle.
 */
    if (jt1 < 0  &&  jt2 < 0) {
        return -1;
    }
    if (it1 < 0  &&  it2 < 0) {
        return -1;
    }
    if (jt1 >= IndexNcol  &&  jt2 >= IndexNcol) {
        return -1;
    }
    if (it1 >= IndexNrow  &&  it2 >= IndexNrow) {
        return -1;
    }

/*
 * If the bounding box is partially inside the index, only use
 * the part that is inside.
 */
    if (jt1 < 0) jt1 = 0;
    if (jt2 > IndexNcol - 1) jt2 = IndexNcol - 1;
    if (it1 < 0) it1 = 0;
    if (it2 > IndexNrow - 1) it2 = IndexNrow - 1;

/*
 * Center the search based on the quadrant that the center point
 * is in inside its index cell.
 */
    ixphase = 0;
    iyphase = 0;

    dx = xc - ixc * IndexXspace - IndexXmin;
    dy = yc - iyc * IndexYspace - IndexYmin;
    if (dx < IndexXspace / 2.0) {
        ixphase = -1;
    }
    if (dy < IndexYspace / 2.0) {
        iyphase = -1;
    }

/*
 * Check for points in levels around the center.
 * Stop when the bounding box has been checked or
 * when a point is found.
 */
    nclose = -1;
    offset = 0;
    distmin = 1.e30;
    maxndo = it2 - it1 + 1;
    if (jt2 - jt1 + 1 > maxndo) {
        maxndo = jt2 - jt1 + 1;
    }

    ndo = 0;

    for (;;) {

        i1 = iyc - offset + iyphase;
        i2 = iyc + offset + iyphase + 1;
        j1 = ixc - offset + ixphase;
        j2 = ixc + offset + ixphase + 1;

        if (i2 < it1) {
            i2 = it1;
        }
        if (i1 > it2) {
            i1 = it2;
        }
        if (j2 < jt1) {
            j2 = jt1;
        }
        if (j1 > jt2) {
            j1 = jt2;
        }

        ncheck = 0;
        if (i1 < it1) {
            i1 = it1;
            ncheck++;
        }
        if (i2 > it2) {
            i2 = it2;
            ncheck++;
        }
        if (j1 < jt1) {
            j1 = jt1;
            ncheck++;
        }
        if (j2 > jt2) {
            j2 = jt2;
            ncheck++;
        }
        if (ncheck == 4) {
            break;
        }

    /*
     * Check the bottom row of the level.
     */
        rowanchor = i1 * IndexNcol;
        for (jj=j1; jj<=j2; jj++) {
            kk = rowanchor + jj;
            np = GetIndexPointXY (kk, trinum, &distmin);
            if (np >= 0) {
                nclose = np;
            }
        }

    /*
     * Check the top row of the level.
     */
        rowanchor = i2 * IndexNcol;
        for (jj=j1; jj<=j2; jj++) {
            kk = rowanchor + jj;
            np = GetIndexPointXY (kk, trinum, &distmin);
            if (np >= 0) {
                nclose = np;
            }
        }

    /*
     * Check the left column of the level.
     */
        for (ii=i1+1; ii<i2; ii++) {
            kk = ii * IndexNcol + j1;
            np = GetIndexPointXY (kk, trinum, &distmin);
            if (np >= 0) {
                nclose = np;
            }
        }

    /*
     * Check the right column of the level.
     */
        for (ii=i1+1; ii<i2; ii++) {
            kk = ii * IndexNcol + j2;
            np = GetIndexPointXY (kk, trinum, &distmin);
            if (np >= 0) {
                nclose = np;
            }
        }

        if (nclose >= 0) {
            RawPoints[nclose].flag = 1;
            break;
        }

        offset++;

        ndo++;
        if (ndo > maxndo) {
            break;
        }

    }

    return nclose;

}  /*  end of private FindPointInTriangle function  */



/*
  ****************************************************************************

                      P o i n t I n T r i a n g l e

  ****************************************************************************

    Return 1 if the specified point is inside the specified triangle.  Return
  -1 if outside.  Return zero if on edge.

*/

int CSWGrdTriangle::PointInTriangle (double x, double y,
                            int trinum)
{
    double                 x1, y1, x2, y2, x3, y3;
    int                    istat, n1, n2, n3;
    EDgeStruct             *eptr;
    TRiangleStruct         *tptr;
    NOdeStruct             *nptr;

    tptr = TriangleList + trinum;
    eptr = EdgeList + tptr->edge1;
    n1 = eptr->node1;
    n2 = eptr->node2;
    eptr = EdgeList + tptr->edge2;
    if (eptr->node1 != n1  &&  eptr->node1 != n2) {
        n3 = eptr->node1;
    }
    else {
        n3 = eptr->node2;
    }

    nptr = NodeList + n1;
    x1 = nptr->x;
    y1 = nptr->y;
    nptr = NodeList + n2;
    x2 = nptr->x;
    y2 = nptr->y;
    nptr = NodeList + n3;
    x3 = nptr->x;
    y3 = nptr->y;

    istat = PointInTriangle3 (x, y,
                              x1, y1, x2, y2, x3, y3);

    return istat;

}  /*  end of private PointInTriangle function  */



/*
  ****************************************************************************

                      P o i n t I n T r i a n g l e 3

  ****************************************************************************

  Convert the triangle vertices into a polygon and see if the point is
  inside the polygon.  Return 1 if the point is inside or on the edge
  of the specified triangle.

*/

int CSWGrdTriangle::PointInTriangle3 (double x, double y,
                             double x1, double y1,
                             double x2, double y2,
                             double x3, double y3)
{
    double      xa[4], ya[4];
    int         istat;

    CSWPolyUtils        ply_utils_obj;

    xa[0] = x1;
    ya[0] = y1;
    xa[1] = x2;
    ya[1] = y2;
    xa[2] = x3;
    ya[2] = y3;
    xa[3] = x1;
    ya[3] = y1;

    istat = ply_utils_obj.ply_point (xa, ya, 4, x, y);

    Ntpt++;

    if (istat == 0) istat = 1;

    return istat;

}  /*  end of private PointInTriangle3 function  */





/*
  ****************************************************************************

                         E x p a n d M e m

  ****************************************************************************

    Grow the work space memory if less than 10 spaces are left in any of
    the arrays.

*/

int CSWGrdTriangle::ExpandMem (void)
{
    EDgeStruct            *enew;
    TRiangleStruct        *tnew;
    NOdeStruct            *nnew;
    int                   size;

    if (ExpandNodeList != 0) {
        if (NumNodes >= MaxNodes - 10) {
            MaxNodes *= 3;
            MaxNodes /= 2;
            nnew = (NOdeStruct *)csw_Realloc (NodeList, MaxNodes * sizeof(NOdeStruct));
            if (!nnew) {
                FreeMem();
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            NodeList = nnew;
            size = (MaxNodes - NumNodes) * sizeof(NOdeStruct);
            memset (NodeList+NumNodes, 0, size);
        }
    }

    if (ExpandEdgeList != 0) {
        if (NumEdges >= MaxEdges - 10) {
            MaxEdges *= 3;
            MaxEdges /= 2;
            enew = (EDgeStruct *)csw_Realloc (EdgeList, MaxEdges * sizeof(EDgeStruct));
            if (!enew) {
                FreeMem();
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            EdgeList = enew;
            size = (MaxEdges - NumEdges) * sizeof(EDgeStruct);
            memset (EdgeList+NumEdges, 0, size);
        }
    }

    if (ExpandTriList != 0) {
        if (NumTriangles >= MaxTriangles - 10) {
            MaxTriangles *= 3;
            MaxTriangles /= 2;
            tnew = (TRiangleStruct *)csw_Realloc
                   (TriangleList, MaxTriangles * sizeof(TRiangleStruct));
            if (!tnew) {
                FreeMem();
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            TriangleList = tnew;
            size = (MaxTriangles - NumTriangles) * sizeof(TRiangleStruct);
            memset (TriangleList+NumTriangles, 0, size);
        }
    }

    return 1;

}  /*  end of private ExpandMem function  */





/*
  ****************************************************************************

                           E x p a n d M e m 2

  ****************************************************************************

    In a worst case scenario, a constraint line segment could intersect all
    of the triangles currently in the trimesh.  For each of these triangle
    intersections, 3 additional edges, 2 additional triangles, and one
    additional node is created.  Thus, there has to be more than twice as
    much triangle space and more than 3 times as much edge space left as
    has been used.  To be safe, I require 4 times as much for edges and triangles
    and 2 times as much for nodes.  A csw_Realloc for each piece added will not
    work because the line segment is processed recursively and therefore
    the private pointers cannot be changed during the recursion.
*/

int CSWGrdTriangle::ExpandMem2 (void)
{
    EDgeStruct            *enew, *eptr;
    TRiangleStruct        *tnew, *tptr;
    NOdeStruct            *nnew, *nptr;
    int                   nsize, oldmax;

    if (ExpandNodeList != 0) {
        oldmax = MaxNodes;
        if (NumNodes >= MaxNodes/2) {
            MaxNodes *= 2;
            nnew = (NOdeStruct *)csw_Realloc (NodeList, MaxNodes * sizeof(NOdeStruct));
            if (!nnew) {
                FreeMem();
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            NodeList = nnew;
            nptr = NodeList + oldmax;
            nsize = (MaxNodes - oldmax) * sizeof(NOdeStruct);
            memset (nptr, 0, nsize);
        }
    }

    if (ExpandEdgeList != 0) {
        oldmax = MaxEdges;
        if (NumEdges >= MaxEdges/2) {
            MaxEdges *= 2;
            enew = (EDgeStruct *)csw_Realloc (EdgeList, MaxEdges * sizeof(EDgeStruct));
            if (!enew) {
                FreeMem();
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            EdgeList = enew;
            eptr = EdgeList + oldmax;
            nsize = (MaxEdges - oldmax) * sizeof(EDgeStruct);
            memset (eptr, 0, nsize);
        }
    }

    if (ExpandTriList != 0) {
        oldmax = MaxTriangles;
        if (NumTriangles >= MaxTriangles/2) {
            MaxTriangles *= 2;
            tnew = (TRiangleStruct *)csw_Realloc
                   (TriangleList, MaxTriangles * sizeof(TRiangleStruct));
            if (!tnew) {
                FreeMem();
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            TriangleList = tnew;
            tptr = TriangleList + oldmax;
            nsize = (MaxTriangles - oldmax) * sizeof(TRiangleStruct);
            memset (tptr, 0, nsize);
        }
    }

    return 1;

}  /*  end of private ExpandMem2 function  */







/*
  ****************************************************************************

                                A d d N o d e

  ****************************************************************************

    Add a node to the list of nodes currently in the triangulation.  There is
    no need to check for sppace in the NodeList array, as it was allocated
    large enough for all possible nodes when it was created.

*/

int CSWGrdTriangle::AddNode (double x, double y, double z, int flag)
{
    NOdeStruct           *nptr;
    int                  istat;

    if (NumNodes >= MaxNodes) {
        istat = ExpandMem2();
        if (istat == -1) {
            return -1;
        }
    }

    nptr = NodeList + NumNodes;
    nptr->x = x;
    nptr->y = y;
    nptr->z = z;
    nptr->xorig = 1.e30;
    nptr->yorig = 1.e30;
    nptr->zorig = 1.e30;
    nptr->flag = flag;
    nptr->rp = -1;
    nptr->crp = -1;
    nptr->spillnum = -1;
    nptr->adjusting_node = -1;
    nptr->deleted = 0;
    nptr->shifted = 0;
    nptr->bflag = 0;
    nptr->on_border = 0;
    nptr->is_locked = 0;

    NumNodes++;

    return nptr - NodeList;

}  /*  end of private AddNode function  */




/*
  ****************************************************************************

                                A d d E d g e

  ****************************************************************************

    Add a new edge the the list of edges in the triangulation.  The list is
    expanded if needed to hold the new edge.

*/

int CSWGrdTriangle::AddEdge (int n1, int n2, int t1, int t2, int flag)
{
    EDgeStruct           *eptr;
    int                  istat;

    if (NumEdges >= MaxEdges) {
        istat = ExpandMem2 ();
        if (istat == -1) {
            return -1;
        }
    }

    eptr = EdgeList + NumEdges;
    eptr->node1 = n1;
    eptr->node2 = n2;
    eptr->tri1 = t1;
    eptr->tri2 = t2;
    eptr->flag = flag;
    eptr->number = -1;
    eptr->deleted = 0;
    eptr->tflag = (char)NewEdgeTflag;
    eptr->on_border = 0;
    eptr->pairid = 0;

    if (n1 != n2) {
        eptr->length = NodeDistance (n1, n2);
    }
    else {
        eptr->length = 0.0;
    }

    NumEdges++;

    return eptr - EdgeList;

}  /*  end of private AddEdge function  */





/*
  ****************************************************************************

                            A d d T r i a n g l e

  ****************************************************************************

    Add a triangle to the list of triangles currently in the triangulation.
    The list is expanded if needed to hold the addition.

*/

int CSWGrdTriangle::AddTriangle (int e1, int e2, int e3, int flag)
{
    TRiangleStruct      *tptr;
    int                 istat;

    if (NumTriangles >= MaxTriangles) {
        istat = ExpandMem2();
        if (istat == -1) {
            return -1;
        }
    }

    tptr = TriangleList + NumTriangles;
    tptr->edge1 = e1;
    tptr->edge2 = e2;
    tptr->edge3 = e3;
    tptr->flag = flag;
    tptr->deleted = 0;

    NumTriangles++;

    return tptr - TriangleList;

}  /*  end of private AddTriangle function  */




/*
  ****************************************************************************

                         T r i a n g l e B o u n d s

  ****************************************************************************

    Return the bounding box of a triangle.

*/

int CSWGrdTriangle::TriangleBounds (int trinum,
                           double *xmin, double *ymin,
                           double *xmax, double *ymax)
{
    TRiangleStruct  *tptr;
    double          x1, y1, x2, y2;
    NOdeStruct      *nptr;
    EDgeStruct      *eptr;

    tptr = TriangleList + trinum;

    x1 = 1.e30;
    y1 = 1.e30;
    x2 = -1.e30;
    y2 = -1.e30;

    eptr = EdgeList + tptr->edge1;
    nptr = NodeList + eptr->node1;
    if (nptr->x < x1) x1 = nptr->x;
    if (nptr->y < y1) y1 = nptr->y;
    if (nptr->x > x2) x2 = nptr->x;
    if (nptr->y > y2) y2 = nptr->y;
    nptr = NodeList + eptr->node2;
    if (nptr->x < x1) x1 = nptr->x;
    if (nptr->y < y1) y1 = nptr->y;
    if (nptr->x > x2) x2 = nptr->x;
    if (nptr->y > y2) y2 = nptr->y;

    eptr = EdgeList + tptr->edge2;
    nptr = NodeList + eptr->node1;
    if (nptr->x < x1) x1 = nptr->x;
    if (nptr->y < y1) y1 = nptr->y;
    if (nptr->x > x2) x2 = nptr->x;
    if (nptr->y > y2) y2 = nptr->y;
    nptr = NodeList + eptr->node2;
    if (nptr->x < x1) x1 = nptr->x;
    if (nptr->y < y1) y1 = nptr->y;
    if (nptr->x > x2) x2 = nptr->x;
    if (nptr->y > y2) y2 = nptr->y;

    eptr = EdgeList + tptr->edge3;
    nptr = NodeList + eptr->node1;
    if (nptr->x < x1) x1 = nptr->x;
    if (nptr->y < y1) y1 = nptr->y;
    if (nptr->x > x2) x2 = nptr->x;
    if (nptr->y > y2) y2 = nptr->y;
    nptr = NodeList + eptr->node2;
    if (nptr->x < x1) x1 = nptr->x;
    if (nptr->y < y1) y1 = nptr->y;
    if (nptr->x > x2) x2 = nptr->x;
    if (nptr->y > y2) y2 = nptr->y;

    *xmin = x1;
    *ymin = y1;
    *xmax = x2;
    *ymax = y2;

    return 1;

}  /*  end of private TriangleBounds function  */






/*
  ****************************************************************************

                               F r e e M e m

  ****************************************************************************

    Free the private array pointers if they are not NULL and reset them to NULL.

*/

int CSWGrdTriangle::FreeMem (void)
{
    int              i, ndo, istat;
    INdexStruct      *iptr;

    if (ListNullNeeded == 1) {
        ListNull ();
        ListNullNeeded = 0;
    }

    csw_Free (CLineFlags);
    CLineFlags = NULL;
    NumCLineFlags = 0;

    if (PolygonList) {
        for (i=0; i<NumPolygons; i++) {
            if (PolygonList[i].x) csw_Free (PolygonList[i].x);
            PolygonList[i].x = NULL;
        }
        csw_Free (PolygonList);
        PolygonList = NULL;
    }

    if (SpillpointList) {
        for (i=0; i<NumSpillpoints; i++) {
            if (SpillpointList[i].polygons_using) {
                csw_Free (SpillpointList[i].polygons_using);
                SpillpointList[i].polygons_using = NULL;
            }
        }
        csw_Free (SpillpointList);
        SpillpointList = NULL;
    }

    if (RidgeLineList) {
        for (i=0; i<NumRidgeLines; i++) {
            if (RidgeLineList[i].nodes) {
                csw_Free (RidgeLineList[i].nodes);
                RidgeLineList[i].nodes = NULL;
            }
        }
        csw_Free (RidgeLineList);
        RidgeLineList = NULL;
    }

    istat = IsNodeEdgeInCache (NodeEdgeList);
    if (istat == 0) {
        if (NodeEdgeList) {
            ndo = NumNodeEdgeList;
            for (i=0; i<ndo; i++) {
                if (NodeEdgeList[i].list != NULL) {
                    csw_Free (NodeEdgeList[i].list);
                    NodeEdgeList[i].list = NULL;
                }
            }
            csw_Free (NodeEdgeList);
        }
    }
    NodeEdgeList = NULL;

    if (NodeList) csw_Free (NodeList);
    if (EdgeList) {
        if (DrainageFlag == 1) {
            for (i=0; i<NumEdges; i++) {
                FreeRidgeFromEdge (EdgeList[i].client_data);
            }
        }
        csw_Free (EdgeList);
        EdgeList = NULL;
    }
    if (TriangleList) {
        csw_Free (TriangleList);
        TriangleList = NULL;
    }
    if (RawPoints) {
        for (i=0; i<MaxRawPoints; i++) {
            if (RawPoints[i].edgelist != NULL) {
                csw_Free (RawPoints[i].edgelist);
                RawPoints[i].edgelist = NULL;
            }
        }
        csw_Free (RawPoints);
        RawPoints = NULL;
    }
    if (RawLines) csw_Free (RawLines);
    RawLines = NULL;
    if (Xline) csw_Free (Xline);
    Xline = NULL;
    if (Iline) csw_Free (Iline);
    Iline = NULL;

    if (ConstraintRawPoints) {
        for (i=0; i<NumConstraintRawPoints; i++) {
            if (ConstraintRawPoints[i].edgelist != NULL) {
                csw_Free (ConstraintRawPoints[i].edgelist);
                ConstraintRawPoints[i].edgelist = NULL;
            }
        }
        csw_Free (ConstraintRawPoints);
        ConstraintRawPoints = NULL;
    }

    if (ForkList) {
        csw_Free (ForkList);
        ForkList = NULL;
    }

    PolygonList = NULL;
    SpillpointList = NULL;
    RidgeLineList = NULL;
    NodeEdgeList = NULL;
    NodeList = NULL;
    EdgeList = NULL;
    TriangleList = NULL;
    RawPoints = NULL;
    RawLines = NULL;
    ConstraintRawPoints = NULL;
    ForkList = NULL;

    NumRawPoints = 0;
    MaxRawPoints = 0;
    NumPolygons = 0;
    NumSpillpoints = 0;
    NumRidgeLines = 0;
    MaxRidgeLines = 0;
    MaxConstraintRawPoints = 0;
    NumConstraintRawPoints = 0;
    Xline = NULL;
    Yline = NULL;
    Zline = NULL;
    Iline = NULL;
    Iflag = NULL;
    Nline = 0;
    NumPolygons = 0;
    MaxPolygons = 0;
    NumFork = 0;
    MaxFork = 0;
    NumNodeEdgeList = 0;

    NumNodes = 0;
    MaxNodes = 0;
    NumEdges = 0;
    MaxEdges = 0;
    NumTriangles = 0;
    MaxTriangles = 0;

    if (IndexGrid) {
        for (i=0; i<IndexNcol*IndexNrow; i++) {
            iptr = IndexGrid[i];
            if (iptr) {
                if (iptr->list) csw_Free (iptr->list);
                csw_Free (iptr);
            }
            IndexGrid[i] = NULL;
        }
        csw_Free (IndexGrid);
    }
    IndexGrid = NULL;
    IndexNcol = 0;
    IndexNrow = 0;

    FreeChopData ();

    return 1;

}  /*  end of private FreeMem function  */





/*
  ****************************************************************************

                         F a t a l M e s s a g e

  ****************************************************************************

    This function prints a message to standard error and then exits the
  program.  It is called if a bug is found in the triangulation logic.

*/

void CSWGrdTriangle::FatalMessage (char *text)
{
    fprintf (stderr, "%s\n", text);
    exit (EXIT_FAILURE);

}  /*  end of private FatalMessage function  */






int CSWGrdTriangle::pr_tri(int tnum)
{

    TRiangleStruct    *tptr;
    EDgeStruct        *ep1, *ep2, *ep3;
    NOdeStruct        *np1, *np2, *np3;

    int               e1, e2, e3, n1, n2, n3;

    tptr = TriangleList + tnum;

    e1 = tptr->edge1;
    e2 = tptr->edge2;
    e3 = tptr->edge3;

    n1 = CommonNode(e1, e2);
    n2 = CommonNode(e2, e3);
    n3 = CommonNode(e3, e1);

    np1 = NodeList + n1;
    np2 = NodeList + n2;
    np3 = NodeList + n3;

    ep1 = EdgeList + e1;
    ep2 = EdgeList + e2;
    ep3 = EdgeList + e3;

    printf ("Detail for triangle number %d:\n", tnum);
    printf ("Edge numbers:  %d %d %d\n", e1, e2, e3);
    printf ("Edge 1: %d %d   %d %d\n", ep1->node1, ep1->node2,
             NodeList[ep1->node1].rp, NodeList[ep1->node2].rp);
    printf ("Edge 2: %d %d   %d %d\n", ep2->node1, ep2->node2,
             NodeList[ep2->node1].rp, NodeList[ep2->node2].rp);
    printf ("Edge 3: %d %d   %d %d\n", ep3->node1, ep3->node2,
             NodeList[ep3->node1].rp, NodeList[ep3->node2].rp);

    printf ("Nodes: %d %d %d\n", n1, n2, n3);
    printf ("Raw Nodes: %d %d %d\n", np1->rp, np2->rp, np3->rp);
    printf ("           %f %f\n", RawPoints[np1->rp].x,
                                  RawPoints[np1->rp].y);
    printf ("           %f %f\n", RawPoints[np2->rp].x,
                                  RawPoints[np2->rp].y);
    printf ("           %f %f\n", RawPoints[np3->rp].x,
                                  RawPoints[np3->rp].y);

    printf ("\n");

    return 1;
}





/*
  ****************************************************************************

                          A d d R a w L i n e S e g

  ****************************************************************************

    Add a segment to the list of raw line segments needed to be processed
  when constraints are added to the triangulation.  The array of raw line
  structures is guaranteed to be large enough to hold all the segments.

*/

int CSWGrdTriangle::AddRawLineSeg (RAwPointStruct *rp1,
                          RAwPointStruct *rp2,
                          int lineid)

{
    RawLines[NumRawLines].rp1 = rp1 - RawPoints;
    RawLines[NumRawLines].rp2 = rp2 - RawPoints;
    if (UseClineFlags == 0) {
        RawLines[NumRawLines].constraint_class = rp1->constraint_class;
    }
    else {
        RawLines[NumRawLines].constraint_class = CLineFlags[lineid];
    }
    RawLines[NumRawLines].lineid = lineid;
    RawLines[NumRawLines].deleted = 0;
    rp1->lineid = lineid + 1;
    rp2->lineid = lineid + 1;
    NumRawLines++;

    return 1;

}  /*  end of private AddRawLineSeg function  */







/*
  ****************************************************************************

                        A p p l y C o n s t r a i n t s

  ****************************************************************************

*/

int CSWGrdTriangle::ApplyConstraints (void)
{
    int            istat, esave;
    int            i, ndo;

    int            do_write;
    char           fname1[200];
    double         v6[6];

    if (ExactFlag == 1) {
        istat =
          PreDeleteNodesCloseToConstraints ();
        if (istat == -1) {
            return -1;
        }
    }

    do_write = csw_GetDoWrite ();;
    if (do_write) {
        sprintf (fname1, "predelete.tri");
        grd_WriteTextTriMeshFile (
            0, NULL,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

/*
 * Attempt to swap edges that intersect constraint segments
 * if the exact constraint flag is set.
 */
    if (ExactFlag == 1) {
        esave = EdgeSwapFlag;
        EdgeSwapFlag = FORCE_SWAP;
        istat =
        SwapExactLines ();
        EdgeSwapFlag = esave;
        if (istat == -1) {
            return -1;
        }
    }

/*
 * Create new nodes if needed to put the constraint segments
 * into the trimesh topology.
 */
    istat = ProcessRawLines();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err(1);
        return -1;
    }

    if (do_write) {
        sprintf (fname1, "postraw.tri");
        grd_WriteTextTriMeshFile (
            0, NULL,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

/*
 * Swap non constraint edges to make triangles as equilateral as possible.
 */
    SwapFlags = (char *)csw_Calloc (NumEdges * sizeof(char));
    FinalSwapFlag = 1;
    CornerBias = CORNER_BIAS;
    ndo = 0;
    for (;;) {
        NumSwapped = 0;
        for (i=0; i<NumEdges; i++) {
            if (EdgeList[i].flag != 0) {
                continue;  /* this is a constraint edge */
            }
            if (SwapFlags[i] >= 1) continue;
            SwapEdge (i);
        }
        if (NumSwapped == 0) {
            break;
        }
        for (i=0; i<NumEdges; i++) {
            if (SwapFlags[i] != 2) SwapFlags[i] = 0;
        }
        ndo++;
        if (ndo > NumEdges) {
            assert (0);
        }
    }
    csw_Free (SwapFlags);
    SwapFlags = NULL;
    FinalSwapFlag = 0;

    if (do_write) {
        sprintf (fname1, "preexact.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

/*
 * Remove the nodes that are in the middle of constraint segments
 * if exact constraint matches are needed.
 */
    if (ExactFlag == 1) {
        esave = EdgeSwapFlag;
        EdgeSwapFlag = FORCE_SWAP;
        istat =
        RemoveNodesOnConstraints ();
        EdgeSwapFlag = esave;
        if (istat == -1) {
            return -1;
        }

    /*
     * Remove nodes that are the opposite point in a triangle shared
     * by the constaint edge, if the point is too close to the edge.
     */
        ndo = 0;
        for (;;) {
            istat =
            RemoveNodesCloseToConstraints ();
            if (istat == -1) {
                return -1;
            }
            if (istat == 0) {
                break;
            }
            ndo++;
            if (ndo > NumRawLines) {
                break;
            }
        }

    /*
     * Remove close points that attach via any edge to the end nodes
     * of the constraint edge.
     */
        ndo = 0;
        for (;;) {
            istat =
            RemoveNodesCloseToConstraints2 ();
            if (istat == -1) {
                return -1;
            }
            if (istat == 0) {
                break;
            }
            ndo++;
            if (ndo > NumRawLines) {
                break;
            }
        }

    /*
     * Final non constraint edge swap to improve equilateralness.
     */
        SwapFlags = (char *)csw_Calloc (NumEdges * sizeof(char));
        FinalSwapFlag = 0;
        CornerBias = CORNER_BIAS;
        ndo = 0;
        for (;;) {
            NumSwapped = 0;
            for (i=0; i<NumEdges; i++) {
                if (EdgeList[i].flag != 0) {
                    continue;  /* this is a constraint edge */
                }
                if (SwapFlags[i] >= 1) continue;
                SwapEdge (i);
            }
            if (NumSwapped == 0) {
                break;
            }
            for (i=0; i<NumEdges; i++) {
                if (SwapFlags[i] != 2) SwapFlags[i] = 0;
            }
            ndo++;
        }
        csw_Free (SwapFlags);
        SwapFlags = NULL;
        FinalSwapFlag = 0;
    }

/*
 * Remove edges inside of fault polygons.
 */
    istat =
    WhackEdgesInsideFaultPolygons ();
    if (istat == -1) {
        return -1;
    }

/*
 * Correct for vertical fault constraints.
 */
    istat =
    grd_constraint_ptr->grd_CorrectVerticalFaults (&NodeList, &NumNodes,
                               &EdgeList, &NumEdges,
                               &TriangleList, &NumTriangles);

    return 1;

}  /*  end of private ApplyConstraints function  */




/*
  ****************************************************************************

               B u i l d R a w P o i n t E d g e L i s t s

  ****************************************************************************

    For each raw point from a constraint line, collect the list of all edges
    connected to the node corresponding to the raw point.  These are used
    to subdivide triangles to constrain to the input lines.

*/

int CSWGrdTriangle::BuildRawPointEdgeLists (void)
{
    int                   i, istat;
    RAwPointStruct        *rp1, *rp2;
    NOdeStruct            *np1, *np2;

/*
 * Free all edge lists associated with raw points
 * before rebuilding.
 */
    for (i=0; i<NumRawPoints; i++) {
        rp1 = RawPoints + i;
        if (rp1->edgelist != NULL) {
            csw_Free (rp1->edgelist);
        }
        rp1->edgelist = NULL;
        rp1->nedge = 0;
        rp1->maxedge = 0;
    }

    for (i=0; i<NumConstraintRawPoints; i++) {
        rp1 = ConstraintRawPoints + i;
        if (rp1->edgelist != NULL) {
            csw_Free (rp1->edgelist);
        }
        rp1->edgelist = NULL;
        rp1->nedge = 0;
        rp1->maxedge = 0;
    }

/*
 * Build the new edge lists.
 */
    for (i=0; i<NumEdges; i++) {
        if (EdgeList[i].deleted == 1  ||
            EdgeList[i].node1 == -1  ||
            EdgeList[i].node2 == -1  ||
            EdgeList[i].tri1 == -1) {
            continue;
        }
        np1 = NodeList + EdgeList[i].node1;
        np2 = NodeList + EdgeList[i].node2;
        if (np1->rp >= 0) {
            rp1 = RawPoints + np1->rp;
            istat = AddEdgeToRawPoint(rp1, i);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err(1);
                return -1;
            }
        }
        else if (np1->crp >= 0) {
            rp1 = ConstraintRawPoints + np1->crp;
            istat = AddEdgeToRawPoint(rp1, i);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err(1);
                return -1;
            }
        }
        if (np2->rp >= 0) {
            rp2 = RawPoints + np2->rp;
            istat = AddEdgeToRawPoint(rp2, i);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err(1);
                return -1;
            }
        }
        else if (np2->crp >= 0) {
            rp2 = ConstraintRawPoints + np2->crp;
            istat = AddEdgeToRawPoint(rp2, i);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err(1);
                return -1;
            }
        }
    }

    return 1;

}  /*  end of private BuildRawPointEdgeLists function  */





/*
  ****************************************************************************

                     A d d E d g e T o R a w p o i n t

  ****************************************************************************

    Add an edge to the list of edges connected to a point that is part
    of a constraint line segment.  The edge is only added once.  This is
    only called from BuildRawPointEdgeLists.

*/

int CSWGrdTriangle::AddEdgeToRawPoint (RAwPointStruct *rptr, int edgenum)
{
    int               *list, n, max, i;

    if (EdgeList[edgenum].deleted == 1) {
        return 1;
    }

    if (rptr->nodenum < 0) {
        return 1;
    }

    if (NodeList[rptr->nodenum].deleted == 1) {
        return 1;
    }

    list = rptr->edgelist;
    n = rptr->nedge;
    max = rptr->maxedge;

/*
    Don't add the edge if it is already in the list.
*/
    for (i=0; i<n; i++) {
        if (list[i] == edgenum) {
            return 1;
        }
    }

/*
    Expand the list space if needed
*/
    if (list == NULL  ||  n >= max) {
        max += TRI_INDEX_CHUNK;
        list = (int *)csw_Realloc(list, max * sizeof(int));
        if (list == NULL) {
            rptr->edgelist = NULL;
            return -1;
        }
    }

/*
    Add the edge to the end of the list.
*/
    list[n] = edgenum;
    n++;

/*
    Put the list parameters back into the raw point structure
*/
    rptr->edgelist = list;
    rptr->nedge = n;
    rptr->maxedge = max;

    return 1;

}  /*  end of private AddEdgeToRawPoint function  */






/*
  ****************************************************************************

                            O p p o s i t e E d g e

  ****************************************************************************

    Given a triangle and a node number, return the edge number that is
    opposite from the node.

*/

int CSWGrdTriangle::OppositeEdge (TRiangleStruct *tptr, int nodenum)
{
    EDgeStruct          *eptr;

    eptr = EdgeList + tptr->edge1;
    if (!(eptr->node1 == nodenum  ||  eptr->node2 == nodenum)) {
        return tptr->edge1;
    }
    eptr = EdgeList + tptr->edge2;
    if (!(eptr->node1 == nodenum  ||  eptr->node2 == nodenum)) {
        return tptr->edge2;
    }
    eptr = EdgeList + tptr->edge3;
    if (!(eptr->node1 == nodenum  ||  eptr->node2 == nodenum)) {
        return tptr->edge3;
    }

    return -1;

}  /*  end of private OppositeEdge function  */






/*
  ****************************************************************************

                        O p p o s i t e N o d e

  ****************************************************************************

    Return the number of the node that is opposite of the specified edge
    on the specified triangle.

*/

int CSWGrdTriangle::OppositeNode (int trinum, int edgenum)
{
    int             istat, e1, e2, e3;
    TRiangleStruct  *tptr;

    tptr = TriangleList + trinum;
    e1 = tptr->edge1;
    e2 = tptr->edge2;
    e3 = tptr->edge3;

    if (e1 == edgenum) {
        istat = CommonNode (e2, e3);
    }
    else if (e2 == edgenum) {
        istat = CommonNode (e1, e3);
    }
    else if (e3 == edgenum) {
        istat = CommonNode (e1, e2);
    }
    else {
        istat = -1;
    }

    return istat;

}  /*  end of private OppositeNode function  */





/*
  ****************************************************************************

                          E d g e I n t e r s e c t

  ****************************************************************************

    Return 1 and the intersection point if the specified edge intersects
    the specified segment.  Return zero otherwise.

*/

int CSWGrdTriangle::EdgeIntersect (int edgenum, int node1, int node2,
                          double *xint, double *yint, double *zint)
{
    int               istat;
    double            x1, y1, x2, y2, x3, y3, x4, y4,
                      xt, yt, zt, z1, z2, pct;
    double            dx, dy, adx, ady;
    NOdeStruct        *np;
    EDgeStruct        *ep;

    CSWPolyUtils        ply_utils_obj;

    *xint = 1.e30;
    *yint = 1.e30;
    *zint = GRD_SOFT_NULL_VALUE;

    np = NodeList + node1;
    x1 = np->x;
    y1 = np->y;
    np = NodeList + node2;
    x2 = np->x;
    y2 = np->y;
    ep = EdgeList + edgenum;
    np = NodeList + ep->node1;
    x3 = np->x;
    y3 = np->y;
    np = NodeList + ep->node2;
    x4 = np->x;
    y4 = np->y;

    istat = ply_utils_obj.ply_segint(x1, y1, x2, y2, x3, y3, x4, y4, &xt, &yt);
    if (istat != 0) {
        return 0;
    }

    z1 = NodeList[node1].z;
    z2 = NodeList[node2].z;
    if (z1 > 1.e20  ||  z2 > 1.e20  ||
        z1 < -1.e15  ||  z2 < -1.e15) {
        zt = GRD_SOFT_NULL_VALUE;
    }
    else {
        dx = x2 - x1;
        dy = y2 - y1;
        adx = dx;
        if (dx < 0.0) adx = -dx;
        ady = dy;
        if (dy < 0.0) ady = -dy;
        if (adx > ady) {
            pct = (xt - x1) / dx;
        }
        else {
            pct = (yt - y1) / dy;
        }
        zt = z1 + (z2 - z1) * pct;
    }

    *xint = xt;
    *yint = yt;
    *zint = zt;

    return 1;

}  /*  end of private EdgeIntersect function  */




/*
  ****************************************************************************

                       P r o c e s s R a w L i n e s

  ****************************************************************************

    Loop through all the constraint segments and modify the trimesh for
    each segment.  Each constraint segment will have its start and end
    node in the unconstrained trimesh.  Any triangles crossed by the segment
    between these nodes are split so that the slit triangles have pieces
    of the raw segment as edges.

*/

int CSWGrdTriangle::ProcessRawLines (void)
{
    int                  istat, i, nt1, nt2;
    RAwLineSegStruct     *rline;
    RAwPointStruct       *rp1, *rp2;
    int                  do_write;
    char                 fname1[100];

    if (ConstraintRawPoints != NULL) {
        csw_Free (ConstraintRawPoints);
        ConstraintRawPoints = NULL;
    }
    MaxConstraintRawPoints = NumRawLines * 10;
    NumConstraintRawPoints = 0;
    ConstraintRawPoints = (RAwPointStruct *)csw_Calloc
        (MaxConstraintRawPoints * sizeof(RAwPointStruct));
    if (ConstraintRawPoints == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        sprintf (fname1, "prerawlines.tri");
        grd_WriteTextTriMeshFile (
            0, NULL,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

    ConstraintZ1 = 1.e30;
    ConstraintZ2 = 1.e30;
    for (i=0; i<NumRawLines; i++) {
        rline = RawLines + i;

        rp1 = RawPoints + rline->rp1;
        rp2 = RawPoints + rline->rp2;
        nt1 = rp1->nodenum;
        nt2 = rp2->nodenum;

        if (nt1 < 0  ||  nt2 < 0) {
            continue;
        }
        ConstraintX1 = rp1->x;
        ConstraintY1 = rp1->y;
        ConstraintZ1 = rp1->z;
        ConstraintX2 = rp2->x;
        ConstraintY2 = rp2->y;
        ConstraintZ2 = rp2->z;
        ConstraintSegmentNumber = i;
        ConstraintSegmentClass = rline->constraint_class;
        if (ConstraintSegmentClass == 0) {
            ConstraintSegmentClass = GRD_UNDEFINED_CONSTRAINT;
        }
        if (ConstraintSegmentClass == GRD_DISCONTINUITY_CONSTRAINT) {
            ConstraintSegmentClass = GRD_TRIMESH_FAULT_CONSTRAINT;
        }
        if (ConstraintSegmentClass == GRD_ZERO_DISCONTINUITY_CONSTRAINT) {
            ConstraintSegmentClass = GRD_TRIMESH_ZERO_FAULT_CONSTRAINT;
        }
        ConstraintLineID = rline->lineid;
        istat = ExpandMem2();
        if (istat == -1) {
            return -1;
        }
        Ncall = 0;
        Orignt1 = nt1;
        Orignt2 = nt2;
        NumNodesToLock = 0;
        istat = ProcessConstraintSegment (nt1, rp1->edgelist, rp1->nedge,
                                          nt2);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err(1);
            if (NtryFlag == 1) {
                grd_utils_ptr->grd_set_err (2);
            }
            NtryFlag = 0;
            return -1;
        }
        LockNodesToLock ();
    }
    ConstraintZ1 = 1.e30;
    ConstraintZ2 = 1.e30;

    return 1;

}  /*  end of private ProcessRawLines function  */


void CSWGrdTriangle::AddNodeToLock (int nodenum)
{
    if (MaxNodesToLock >= NumNodesToLock) {
        MaxNodesToLock += 100;
        NodesToLock = (int *)csw_Realloc (NodesToLock, MaxNodesToLock * sizeof(int));
    }
    if (NodesToLock == NULL) {
        assert (0);
        return;
    }

    NodesToLock[NumNodesToLock] = nodenum;
    NumNodesToLock++;

    return;
}

void CSWGrdTriangle::LockNodesToLock (void)
{
    int             i;
    NOdeStruct      *np;

    if (NodesToLock == NULL  ||  NumNodesToLock < 1) {
        return;
    }

    for (i=0; i<NumNodesToLock; i++) {
        np = NodeList + NodesToLock[i];
        if (np->spillnum == 99) {
            np->spillnum = 999;
        }
    }

    return;

}

/*
  ****************************************************************************

              P r o c e s s C o n s t r a i n t S e g m e n t

  ****************************************************************************

  This is only called from the ProcessRawLines function.

*/

int CSWGrdTriangle::ProcessConstraintSegment (int nt1,
                                     int *list, int nlist,
                                     int nt2)
{
    int               istat, i, n1, n2, et, elist4[4], rp;
    int               ntry, astat, nchk, t1, t2, elock;
    double            xt, yt, zt;
    EDgeStruct        *eptr;
    NOdeStruct        *nptr, *np;
    TRiangleStruct    *tp1, *tp2;
    RAwPointStruct    *rptr, *rp1;

    Ncall++;

    if (Ncall > MaxNcall) return 1;

/*
    These lines are to suppress warnings.
*/
    if (nlist < 1) return 1;
    eptr = EdgeList;
    xt = yt = zt = 1.e30;


    n1 = nt1;
    n2 = nt2;
    et = 0;
    tp1 = NULL;

/*
    If the end point of any edge in the list is at the
    same node as the endpoint of the line segment, then
    set the edge that meets these criteria as a constrained
    edge and return.
*/
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];

        if (eptr->node1 == n2  ||  eptr->node2 == n2) {
            eptr->flag = ConstraintSegmentClass;
            eptr->number = ConstraintSegmentNumber;
            eptr->lineid = ConstraintLineID;
            return 1;
        }
    }

/*
    If the end node of any segment is on the constraint segment,
    then the edge can just be used as a constraint edge.
*/
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        if (eptr->flag == ConstraintSegmentClass  &&
            eptr->number == ConstraintSegmentNumber) {
            continue;
        }
        nchk = eptr->node1;
        if (nchk == n1) {
            nchk = eptr->node2;
        }
        if (NodeOnSegment (nchk, nt1, nt2, NULL)) {
            eptr->flag = ConstraintSegmentClass;
            eptr->number = ConstraintSegmentNumber;
            eptr->lineid = ConstraintLineID;
            rp = NodeList[nchk].rp;
            if (rp < 0) {
                rp = NodeList[nchk].crp;
                rptr = ConstraintRawPoints + rp;
            }
            else {
                rptr = RawPoints + rp;
            }
            istat = ProcessConstraintSegment(nchk,
                                             rptr->edgelist,
                                             rptr->nedge,
                                             nt2);
            return istat;
        }
    }

/*
    Find an opposite triangle edge from the nt1 node that intersects
    the constraint segment.
*/
    ntry = 0;
    et = -1;
    for (;;) {
        astat = 0;
        for (i=0; i<nlist; i++) {
            eptr = EdgeList + list[i];

            if (eptr->tri1 < 0  ||  eptr->tri1 >= NumTriangles) {
                printf ("edge with bad tri1 Orig constraint nodes = %d %d\n",
                        Orignt1, Orignt2);
            }

            tp1 = TriangleList + eptr->tri1;
            et = OppositeEdge (tp1, n1);
            if (et != -1) {
                istat = EdgeIntersect(et, n1, n2,
                                      &xt, &yt, &zt);
                if (istat == 1) {
                    astat = 1;
                    istat = SamePointAsNode (xt, yt, n1);
                    if (istat == 1) {
                        astat = AdjustConstraintNode (n1, et);
                    }
                    break;
                }
            }

            if (eptr->tri2 < 0) {
                continue;
            }
            tp1 = TriangleList + eptr->tri2;
            et = OppositeEdge (tp1, n1);
            if (et != -1) {
                istat = EdgeIntersect(et, n1, n2,
                                      &xt, &yt, &zt);
                if (istat == 1) {
                    astat = 1;
                    istat = SamePointAsNode (xt, yt, n1);
                    if (istat == 1) {
                        astat = AdjustConstraintNode (n1, et);
                    }
                    break;
                }
            }
        }
        if (astat >= 0) {
            break;
        }
        ntry++;
        if (ntry > 10) {
            printf ("Too many retries to find non grazing constraint intersection.\n");
            printf ("orig nt1 = %d  orig nt2 = %d\n", Orignt1, Orignt2);
            NtryFlag = 1;
            return -1;
        }
    /*
        Rebuild the list of edges to check since the failed
        adjustment may have removed a node and changed the topology.
    */
        nptr = NodeList + n1;
        rp = nptr->rp;
        if (rp < 0) {
            rp = nptr->crp;
            if (rp < 0) {
                assert (rp >= 0);
            }
            rptr = ConstraintRawPoints + rp;
        }
        else {
            rptr = RawPoints + rp;
        }
        list = rptr->edgelist;
        nlist = rptr->nedge;

    }

/*
    If no intersection was found.
*/
    if (xt > 1.e15  ||  et < 0  ||  tp1 == NULL) {
        istat = CreateNewTriangleForConstraint (nt1, nt2);
        if (istat == 0) {
            printf ("Error finding intersection in constraint line processing.\n");
            printf ("orig nt1 = %d  orig nt2 = %d\n", Orignt1, Orignt2);
            return 1;
        }
        else if (istat == -1) {
            printf ("Memory allocation error finding intersection"
                    " in constraint line processing.\n");
            printf ("orig nt1 = %d  orig nt2 = %d\n", Orignt1, Orignt2);
            return -1;
        }
        else {
            return 1;
        }
    }

    eptr = EdgeList + et;

/*
    Make a new node from the intersection point and use it to
    split the two triangles defined by the edge into four triangles.
    Note that the various operations here can add to the trimesh
    element lists, possibly growing the lists via csw_Realloc calls.
    Because of this, the offsets into the lists must be used
    to recalculate pointers after each possible csw_Reallocation.
*/
    tp2 = TriangleList + eptr->tri1;
    if (tp1 == tp2) {
        tp2 = TriangleList + eptr->tri2;
    }

/*
    tp1 cannot be NULL here, but pclint complains that it can be.
    The assert above here happens if tp1 is null.  This bit of
    illogic here is to suppress the lint complaint
*/
    t1 = 0;
    if (tp1 != NULL) {
        t1 = tp1 - TriangleList;
    }
    t2 = tp2 - TriangleList;

    zt = 1.e30;
    if (ConstraintSegmentClass == GRD_UNDEFINED_CONSTRAINT) {
        if (ConstraintZ1 < 1.e20  &&  ConstraintZ2 < 1.e20) {
            zt = InterpolateConstraintZvalue (xt, yt);
        }
        else {
            zt = ChooseTrianglePlaneZvalue (t1, t2, xt, yt);
        }
    }
    else if (ConstraintSegmentClass == GRD_BOUNDARY_CONSTRAINT) {
        zt = ChooseTrianglePlaneZvalue (t1, t2, xt, yt);
    }
    else {
        if (ConstraintZ1 < 1.e20  &&  ConstraintZ2 < 1.e20) {
            zt = InterpolateConstraintZvalue (xt, yt);
        }
    }
    if (zt > 1.e15  ||  zt < -1.e15) {
        zt = GRD_SOFT_NULL_VALUE;
    }

/*
 * If xt, yt is close to an original grid corner node, change the grid
 * corner node position to xt, yt, zt rather than creating a new node.
 */
    n1 = FindExistingGridCornerNode (xt, yt, zt, 1);
    if (n1 == -1) {
        n1 = AddNode(xt, yt, zt, ConstraintSegmentClass);
        if (n1 < 0) {
            return -1;
        }
        nptr = NodeList + n1;
        nptr->adjusting_node = nt1;

        /*
        istat = SplitIntoFour (t1, t2, et, n1, elist4);
        */
        istat = SplitFromEdge (et, n1, elist4);
        if (istat == -1) {
            return -1;
        }

        istat = AddConstraintRawPoint (NodeList + n1, elist4, 4);

        if (!(elist4[0] == -1  ||  elist4[1] == -1  ||
              elist4[2] == -1  ||  elist4[3] == -1)) {
            istat = ProcessConstraintSegment(n1,
                                     elist4, 4,
                                     nt2);
        }
    }
    else {
        elock = FindEdgeInList (list, nlist, n1, nt1);
        if (elock < 0) {
            assert (0);
        }
        EdgeList[elock].flag = ConstraintSegmentClass;
        EdgeList[elock].lineid = ConstraintLineID;
        AddNodeToLock (n1);
        np = NodeList + n1;
        np->spillnum = 99;
        rp1 = RawPoints + n1;
        rp1->x = xt;
        rp1->y = yt;
        rp1->z = zt;
        istat = ProcessConstraintSegment (n1,
                                          rp1->edgelist,
                                          rp1->nedge,
                                          nt2);
    }

    if (istat == -1) {
        return -1;
    }
    eptr = EdgeList + et;
    np = NodeList + eptr->node1;
    np->deleted = 0;
    np = NodeList + eptr->node2;
    np->deleted = 0;

    return istat;

}  /*  end of private ProcessConstraintSegment function  */



int CSWGrdTriangle::FindEdgeInList (int *list, int nlist, int n1, int n2)
{
    int        i;
    EDgeStruct *ep;

    if (list == NULL  ||  nlist < 1) {
        return -1;
    }

    for (i=0; i<nlist; i++) {
        ep = EdgeList + list[i];
        if (ep->node1 == n1  &&  ep->node2 == n2) {
            return list[i];
        }
        if (ep->node1 == n2  &&  ep->node2 == n1) {
            return list[i];
        }
    }

    return -1;
}




/*
  ****************************************************************************

                        S p l i t I n t o F o u r

  ****************************************************************************

    Split the two specified triangles with the specified common edge into
    four triangles.  Each of the four will have the specified node as a
    vertex.  This is only called from ProcessConstraintSegment.

*/

#if 0
int CSWGrdTriangle::SplitIntoFour (int t1in, int t2in, int e1in, int n1in,
                          int *elist)
{
    TRiangleStruct     *tp1, *tp2;
    TRiangleStruct     *tp3, *tp4;
    EDgeStruct         *ep;

    int                nc, n1, n2, n3, n4,
                       e1, e2, e3, e4,
                       t1, t2, t3, t4,
                       e13, e23, e24, e14;

    tp1 = TriangleList + t1in;
    tp2 = TriangleList + t2in;
    ep = EdgeList + e1in;

    t1 = t1in;
    t2 = t2in;
    nc = n1in;
    n1 = ep->node1;
    n2 = ep->node2;
    ep->node2 = nc;
    ep->length = NodeDistance(nc, n1);
    n3 = OppositeNode (tp1-TriangleList, ep-EdgeList);
    if (t2in < 0) {
        n4 = -1;
    }
    else {
        n4 = OppositeNode (tp2-TriangleList, ep-EdgeList);
    }

    e1 = ep - EdgeList;
    e2 = AddEdge (n2, nc, -1, -1, 0);
    e3 = AddEdge (n3, nc, -1, -1, ConstraintSegmentClass);
    EdgeList[e3].number = ConstraintSegmentNumber;
    EdgeList[e3].lineid = ConstraintLineID;

    if (n4 >= 0) {
        e4 = AddEdge (n4, nc, -1, -1, 0);
        if (e4 < 0) {
            return -1;
        }
    }
    else {
        e4 = -1;
    }

    e13 = FindTriangleEdge (tp1, n1, n3);
    e23 = FindTriangleEdge (tp1, n2, n3);
    t3 = AddTriangle (e23, e2, e3, 0);
    if (n4 >= 0) {
        e24 = FindTriangleEdge (tp2, n2, n4);
        e14 = FindTriangleEdge (tp2, n1, n4);
        t4 = AddTriangle (e24, e2, e4, 0);
    }
    else {
        t4 = -1;
        e24 = -1;
        e14 = -1;
    }

    tp1 = TriangleList + t1;
    tp3 = TriangleList + t3;

    if (t2 >= 0  &&  t4 >= 0) {
        tp2 = TriangleList + t2;
        tp4 = TriangleList + t4;
    }
    else {
        tp2 = NULL;
        tp4 = NULL;
    }

    tp1->edge1 = e13;
    tp1->edge2 = e1;
    tp1->edge3 = e3;

    if (tp2 != NULL) {
        tp2->edge1 = e14;
        tp2->edge2 = e1;
        tp2->edge3 = e4;
    }

    if (tp2 != NULL  &&  tp4 != NULL) {
        if (EdgeList[e24].tri1 == tp2-TriangleList) {
            EdgeList[e24].tri1 = tp4 - TriangleList;
        }
        else {
            EdgeList[e24].tri2 = tp4 - TriangleList;
        }
    }

    if (EdgeList[e23].tri1 == tp1-TriangleList) {
        EdgeList[e23].tri1 = tp3 - TriangleList;
    }
    else {
        EdgeList[e23].tri2 = tp3 - TriangleList;
    }

    EdgeList[e2].tri1 = tp3 - TriangleList;
    EdgeList[e3].tri1 = tp1 - TriangleList;
    EdgeList[e3].tri2 = tp3 - TriangleList;

    if (tp2 != NULL  &&  tp4 != NULL) {
        EdgeList[e2].tri2 = tp4 - TriangleList;
        EdgeList[e4].tri1 = tp2 - TriangleList;
        EdgeList[e4].tri2 = tp4 - TriangleList;
    }

    elist[0] = e4;
    elist[1] = e2;
    elist[2] = e3;
    elist[3] = e1;

    RemoveEdgeFromNodeList (n2, e1);
    AddEdgeToNodeList (n2, e2);
    AddEdgeToNodeList (n3, e3);

    if (n4 >= 0) {
        AddEdgeToNodeList (n4, e4);
    }

    return 1;

}  /*  end of private SplitIntoFour function  */

#endif






/*
  ****************************************************************************

                     F i n d T r i a n g l e E d g e

  ****************************************************************************

    Return the edge number from the specified triangle where
    the edge uses the specified nodes.  If no edge in the triangle
    uses the nodes, -1 is returned.

*/

int CSWGrdTriangle::FindTriangleEdge (TRiangleStruct *tp1, int n1, int n2)
{
    EDgeStruct            *ep;

    ep = EdgeList + tp1->edge1;
    if (ep->node1 == n1  &&  ep->node2 == n2) {
        return tp1->edge1;
    }
    if (ep->node1 == n2  &&  ep->node2 == n1) {
        return tp1->edge1;
    }

    ep = EdgeList + tp1->edge2;
    if (ep->node1 == n1  &&  ep->node2 == n2) {
        return tp1->edge2;
    }
    if (ep->node1 == n2  &&  ep->node2 == n1) {
        return tp1->edge2;
    }

    ep = EdgeList + tp1->edge3;
    if (ep->node1 == n1  &&  ep->node2 == n2) {
        return tp1->edge3;
    }
    if (ep->node1 == n2  &&  ep->node2 == n1) {
        return tp1->edge3;
    }

    return -1;

}  /*  end of private FindTriangleEdge function  */





/*
  ****************************************************************************

                      S a m e P o i n t A s N o d e

  ****************************************************************************

    Return 1 if the x,y point grazes the node or return zero otherwise.

*/

int CSWGrdTriangle::SamePointAsNode (double x, double y, int nodenum)
{
    double        dx, dy, dist;
    NOdeStruct    *nptr;

    nptr = NodeList + nodenum;

    dx = x - nptr->x;
    dy = y - nptr->y;
    dist = sqrt(dx * dx + dy * dy);

    if (dist <= GrazeDistance) {
        return 1;
    }

    return 0;

}  /*  end of private SamePointAsNode function  */




/*
 *******************************************************************************

                        F r e e C h o p D a t a

 *******************************************************************************

*/

void CSWGrdTriangle::FreeChopData (void)
{
    if (Xchop) csw_Free (Xchop);
    if (ChopPoints) csw_Free (ChopPoints);
    Xchop = NULL;
    Ychop = NULL;
    Zchop = NULL;
    ChopPoints = NULL;
    ChopFlags = NULL;
    Nchop = 0;
    return;

}  /*  end of private FreeChopData function  */


/*
 *********************************************************************************

                          C h o p L i n e s

 *********************************************************************************

  Chop up the constraint lines by adding additional points between vertices.
  The resulting segments will be somewhat the same length as the nominal
  spacing between the rest of the data points used in the triangulation.
  The Z values of the new points are linearly interpolated from the z values
  of the original vertices.

*/

int CSWGrdTriangle::ChopLines (double *xlines, double *ylines, double *zlines,
                      int *linepoints, int *lineflags, int nlines,
                      double xmin, double ymin, double xmax, double ymax,
                      int npts)
{
    double            area, dtest, dx, dy, dz, dtot, dt,
                      x1, y1, x2, y2, z1, z2;
    int               i, j, k, n, n2, j2, jdo, nstart;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            FreeChopData ();
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    FreeChopData ();

    if (xlines == NULL  ||  ylines == NULL  ||  zlines == NULL  ||
        nlines < 1  ||  npts < 1) {
        return 1;
    }

/*
    Use the average area per point as the nominal
    point spacing.
*/
    area = (xmax - xmin) * (ymax - ymin) / (double)npts;
    dtest = sqrt (area);

/*
    Allocate space for chopped coordinates.
*/
    dtot = 0.0;
    k = 0;
    for (i=0; i<nlines; i++) {
        for (j=0; j<linepoints[i]-1; j++) {
            dx = xlines[k+1] - xlines[k];
            dy = ylines[k+1] - ylines[k];
            dt = dx * dx + dy * dy;
            dt = sqrt(dt);
            dtot += dt;
            k++;
        }
        k++;
    }

    if (dtot > 0.0  &&  dtest > 0.0) {
        n = (int)(dtot / dtest) + 100 + nlines;
    }
    else {
        return 1;
    }
    n += k * 4;

    Xchop = (double *)csw_Malloc (n * 3 * sizeof(double));
    if (Xchop == NULL) {
        return -1;
    }
    Ychop = Xchop + n;
    Zchop = Ychop + n;

    n = nlines * 2 + 10;
    ChopPoints = (int *)csw_Malloc (n * 2 * sizeof(int));
    if (ChopPoints == NULL) {
        return -1;
    }
    ChopFlags = ChopPoints + n;

    n = 0;
    k = 0;
    n2 = 0;
    nstart = 0;
    x2 = y2 = z2 = 0.0; /* silence warnings */
    for (i=0; i<nlines; i++) {
        for (j=0; j<linepoints[i]-1; j++) {
            x1 = xlines[k];
            y1 = ylines[k];
            x2 = xlines[k+1];
            y2 = ylines[k+1];
            z1 = zlines[k];
            z2 = zlines[k+1];
            k++;
            dx = x2 - x1;
            dy = y2 - y1;
            dz = z2 - z1;
            dt = dx * dx + dy * dy;
            dt = sqrt(dt);
            jdo = (int)(dt / dtest);
            jdo += 2;
            dx /= (double)(jdo - 1);
            dy /= (double)(jdo - 1);
            dz /= (double)(jdo - 1);
            Xchop[n2] = x1;
            Ychop[n2] = y1;
            Zchop[n2] = z1;
            n2++;
            if (ChopLinesFlag == 1) {
                for (j2=1; j2<jdo-1; j2++) {
                    Xchop[n2] = x1 + j2 * dx;
                    Ychop[n2] = y1 + j2 * dy;
                    Zchop[n2] = z1 + j2 * dz;
                    n2++;
                }
            }
        }
        Xchop[n2] = x2;
        Ychop[n2] = y2;
        Zchop[n2] = z2;
        n2++;
        ChopFlags[i] = lineflags[i];
        ChopPoints[i] = n2 - nstart;
        nstart = n2;
    }

    Nchop = nlines;

    ChopLinesFlag = 1;

    bsuccess = true;

    return 1;

}  /*  end of private ChopLines function  */



/*
 ********************************************************************************

                   S a m e R a w P o i n t

 ********************************************************************************

  If the two specified points are nearly coincident, return 1.  Otherwise,
  return zero.

*/

int CSWGrdTriangle::SameRawPoint (int p1, int p2)
{
    double       dx, dy, dt, tiny;
    RAwPointStruct  *rp1, *rp2;

    if (RawPoints == NULL) {
        return 0;
    }

    tiny = AreaPerimeter / 20000.0;
    tiny *= tiny;

    rp1 = RawPoints + p1;
    rp2 = RawPoints + p2;

    dx = rp1->x - rp2->x;
    dy = rp1->y - rp2->y;
    dt = dx * dx + dy * dy;
    if (dt <= tiny) {
        NumSame++;
        return 1;
    }
    return 0;

}  /*  end of private SameRawPoint function  */



/*
 *************************************************************************************

                      A d d C o n s t r a i n t R a w P o i n t

 *************************************************************************************

  Add a point to the list of points generated when constraint lines cross existing
  triangles.  This is put into the crp member of the node structure.  Returnd 1 on
  success or -1 if a memory allocation failure occurred.

*/

int CSWGrdTriangle::AddConstraintRawPoint (NOdeStruct *nptr, int *elistin, int nlistin)

{
    int               *el = NULL;
    RAwPointStruct    *rp = NULL;
    int               nlist, n, i;
    int               *elist = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            FreeMem ();
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    elist = p_int_1000;

    if (nlistin > 1000) {
        return -1;
    }

/*
 * Remove any -1 list numbers from the edge list.
 */
    n = 0;
    for (i=0; i<nlistin; i++) {
        if (elistin[i] >= 0) {
            elist[n] = elistin[i];
            n++;
        }
    }
    nlist = n;

/*
    Expand the list of constraint raw points if needed.
*/
    if (ConstraintRawPoints == NULL) {
        NumConstraintRawPoints = 0;
        MaxConstraintRawPoints = 0;
    }
    if (NumConstraintRawPoints >= MaxConstraintRawPoints) {
        MaxConstraintRawPoints += 100;
        rp = (RAwPointStruct *)csw_Calloc (MaxConstraintRawPoints * sizeof(RAwPointStruct));
        if (rp == NULL) {
            return -1;
        }
        if (ConstraintRawPoints != NULL) {
            memcpy (rp, ConstraintRawPoints, NumConstraintRawPoints * sizeof(RAwPointStruct));
            csw_Free (ConstraintRawPoints);
        }
        ConstraintRawPoints = rp;
    }

    nptr->crp = NumConstraintRawPoints;

/*
    Append this point to the end of the list.
*/
    rp = ConstraintRawPoints + NumConstraintRawPoints;
    rp->x = nptr->x;
    rp->y = nptr->y;
    rp->z = nptr->z;
    rp->nodenum = nptr - NodeList;
    el = (int *)csw_Malloc (nlist * sizeof(int));
    if (el == NULL) {
        return -1;
    }
    memcpy (el, elist, nlist * sizeof(int));
    rp->edgelist = el;
    el = NULL;
    rp->nedge = nlist;
    rp->maxedge = nlist;

    NumConstraintRawPoints++;

    bsuccess = true;

    return 1;

}  /*  end of private AddConstraintRawPoint function  */



/*
 **********************************************************************************

               A d j u s t C o n s t r a i n t N o d e

 **********************************************************************************

  Move the specified node back toward its adjusting_node.  If there is no adjusting edge
  do nothing.  The node is moved to the mid point or to a point 10 times the grazing
  distance from the specified x,y point, whichever is the smaller move.
*/

int CSWGrdTriangle::AdjustConstraintNode (int n1, int edgenum)
{
    double    x1, y1, x2, y2, xt, yt, d1, dx, dy;
    int       i, nadj, edgen1, onconstraint, swapsav, iswap;
    NOdeStruct *np1, *np2;
    EDgeStruct *eptr;

    np1 = NodeList + n1;
    nadj = np1->adjusting_node;
    if (nadj < 0) {
        return 0;
    }

    eptr = EdgeList + edgenum;
    edgen1 = eptr->node1;

/*
 *  Try to distribute the spacing of the constraint nodes
 *  back along the adjustment chain to get a spacing
 *  that will not graze.
 */
    for (i=0; i<5; i++) {
        RecursiveAdjust (n1, eptr->tri1, eptr->tri2);

        np2 = NodeList + nadj;
        x1 = np1->x;
        y1 = np1->y;
        x2 = np2->x;
        y2 = np2->y;
        xt = (x1 + x2) / 2.0;
        yt = (y1 + y2) / 2.0;

        dx = x1 - xt;
        dy = y1 - yt;
        d1 = dx * dx + dy * dy;
        d1 = sqrt(d1);
        if (d1 > AdjustDistance) {
            return 1;
        }
    }


/*
 * If the spacing cannot be changed because things are too crowded,
 * delete a node and try again.  First, a non constraint node is
 * deleted.  If both candidate nodes are constraint nodes, try to
 * swap the edge.  If that doesn't work, go ahead and delete one
 * of the constraint nodes.
 */
    onconstraint = NodeOnConstraint (edgen1);
    if (onconstraint != 0) {
        edgen1 = eptr->node2;
        onconstraint = NodeOnConstraint (edgen1);
        if (onconstraint != 0) {
            swapsav = EdgeSwapFlag;
            EdgeSwapFlag = FORCE_SWAP;
            iswap = SwapEdge (edgenum);
            EdgeSwapFlag = swapsav;
            if (iswap == 0) {
                RemoveNode (edgen1);
            }
            return -1;
        }
    }
    if (NodeList[edgen1].deleted == 0) {
        RemoveNode (edgen1);
    }
    else {
        edgen1 = eptr->node2;
        RemoveNode (edgen1);
    }

    return -1;

}  /* end of private AdjustConstraintNode function */



/*
 *****************************************************************************

                     R e c u r s i v e A d j u s t

 *****************************************************************************

  Reposition the nodes back along the adjustment chain to alleviate grazing
  problems.  This is only called from AdjustConstraintNode.

*/

void CSWGrdTriangle::RecursiveAdjust (int nodenum, int t1, int t2)
{
    NOdeStruct        *np1, *np2;
    int               n1, n2;
    double            xt, yt, dx, dy, dist;

    np1 = NodeList + nodenum;
    n1 = np1->adjusting_node;
    np2 = NodeList + n1;
    n2 = np2->adjusting_node;
    if (n2 >= 0) {
        RecursiveAdjust (n1, t1, t2);
    }

    xt = (np1->x + np2->x) / 2.0;
    yt = (np1->y + np2->y) / 2.0;

    dx = xt - np2->x;
    dy = yt - np2->y;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (dist < AdjustDistance  &&  dist > 0.0) {
        dx *= AdjustDistance / dist;
        dy *= AdjustDistance / dist;
        xt = np2->x + dx;
        yt = np2->y + dy;
    }
    np1->x = xt;
    np1->y = yt;
    if (ConstraintSegmentClass != GRD_DISCONTINUITY_CONSTRAINT) {
        np1->z = ChooseTrianglePlaneZvalue (t1, t2, xt, yt);
    }
    else {
        if (ConstraintZ1 < 1.e20  &&  ConstraintZ2 < 1.e20) {
            np1->z = InterpolateConstraintZvalue (xt, yt);
        }
        else {
            np1->z = GRD_SOFT_NULL_VALUE;
        }
    }

    return;

}  /* end of private RecursiveAdjust function */




/*
 *****************************************************************************

                   N o d e O n C o n s t r a i n t

 *****************************************************************************

  Return 1 if the node is part of a constraint segment or zero otherwise.

*/

int CSWGrdTriangle::NodeOnConstraint (int nodenum)
{
    NOdeStruct     *nptr;

    if (ConstraintPointStart < 1) return 0;

    nptr = NodeList + nodenum;
    if (nptr->rp < 0) {
        return 1;
    }

    if (nptr->rp >= ConstraintPointStart) return 1;

    return 0;

}  /* end of private NodeOnConstraint function */





/*
  ****************************************************************************

                  S w a p E d g e F o r N o d e R e m o v a l

  ****************************************************************************

  The edge forms one of two diagonals of a quadrilateral.  Always swap
  it if the two diagonals intersect (i.e. the quadrilateral is convex).
  This is used by the node removal code.  If the edge was successfully
  swapped, 1 is returned.  If not successfully swapped, zero is returned.

*/

int CSWGrdTriangle::SwapEdgeForNodeRemoval (int edgenum)
{
    int        fsave, istat;
    double     gsav;

    CSWPolyUtils        ply_utils_obj;

    ply_utils_obj.ply_getgraze (&gsav);
    ply_utils_obj.ply_setgraze (GrazeDistance * 10.0);

    fsave = EdgeSwapFlag;
    EdgeSwapFlag = FORCE_SWAP;
    istat = SwapEdge2 (edgenum);
    EdgeSwapFlag = fsave;

    ply_utils_obj.ply_setgraze (gsav);

    return istat;

}  /*  end of private SwapEdgeForNodeRemoval function  */



/*
 **********************************************************************

                      R e m o v e N o d e

 **********************************************************************

  Remove the specified node from the triangular mesh and rebuild
  the topology without the node present.

*/

int CSWGrdTriangle::RemoveNode (int nodenum)
{
    int                i, istat, *elist = NULL, nlist, ndone,
                       trinum, outside_flag,
                       num_left, max_left;
    int                t1, t2, t3;
    int                et1, et2, et3;
    int                elist3[3];
    int                n1, n2;
    double             x1, y1, x2, y2;
    EDgeStruct         *eptr = NULL;
    NOdeStruct         *nptr = NULL;
    TRiangleStruct     *tptr = NULL;
    RAwPointStruct     *rptr = NULL;
    int                nforce, iforce;
    int                *forced_swap = NULL;


    auto fscope = [&]()
    {
        csw_Free (forced_swap);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    FILE   *fptr;
    double x0, y0;
*/

/*
    Get the edge list attached to the node.
*/
    nptr = NodeList + nodenum;
    if (nptr->rp < 0  &&  nptr->crp < 0) {
        return -1;
    }
    if (nptr->rp < 0) {
        rptr = ConstraintRawPoints + nptr->crp;
    }
    else {
        rptr = RawPoints + nptr->rp;
    }
    elist = rptr->edgelist;
    nlist = rptr->nedge;

    if (elist == NULL  ||  nlist < 1) {
        return -1;
    }

    forced_swap = (int *)csw_Malloc (2 * nlist * sizeof(int));
    if (forced_swap == NULL) {
        return -1;
    }

/*
    set all the edge structure temporary flags to zero,
    indicating that the edges have not been swapped.
*/
    outside_flag = 0;
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + elist[i];
        eptr->tflag = 0;
        if (eptr->tri2 == -1) {
            outside_flag = 1;
        }
    }

/*
    Swap the edges connected to the node if they are a
    diagonal of a convex quadrilateral.  This process
    is repeated until no edge is swapped in an iteration.
*/
    num_left = nlist;
    max_left = 3;
    nforce = 0;
    if (outside_flag) {
        max_left = 2;
    }
    for (;;) {
        ndone = 0;
        nlist = rptr->nedge;
        elist = rptr->edgelist;
        if (nlist <= max_left) {
            break;
        }
        for (i=0; i<nlist; i++) {
            eptr = EdgeList + elist[i];
            if (eptr->deleted == 1) {
                continue;
            }
            if (eptr->tflag == 1  ||
                eptr->tri2 == -1  ||
                eptr->on_border == 1) {
                continue;
            }
            if (EdgeList[elist[i]].flag != 0) {
                continue;
            }
            istat = GetSwapCoordsForNodeRemoval
                                  (elist[i],
                                   &x1, &y1, &x2, &y2,
                                   &n1, &n2);
            if (istat != 1) {
                continue;
            }

            istat = NodeOnSegmentForNodeRemoval
                (nodenum, n1, n2, NULL);
            if (istat == 1) {
                continue;
            }

            iforce = elist[i];
            istat = SwapEdgeForNodeRemoval (elist[i]);
            eptr->tflag = (char)istat;
            if (istat == 1) {
                forced_swap[nforce] = iforce;
                nforce++;
                ndone++;
                num_left--;
            }

            if (rptr->nedge != nlist) {
                break;
            }
            if (num_left <= max_left) {
                ndone = 0;
                break;
            }
        }
        if (ndone == 0) {
            break;
        }
    }

    if (nlist > 3  &&  RemoveNodeForZeroAreaFlag) {
        for (;;) {
            ndone = 0;
            nlist = rptr->nedge;
            elist = rptr->edgelist;
            if (nlist == max_left) {
                break;
            }
            for (i=0; i<nlist; i++) {
                eptr = EdgeList + elist[i];
                if (eptr->deleted == 1) {
                    continue;
                }
                if (eptr->tflag == 1  ||
                    eptr->tri2 == -1  ||
                    eptr->on_border == 1) {
                    continue;
                }
                if (EdgeList[elist[i]].flag != 0) {
                    continue;
                }
                istat = GetSwapCoordsForNodeRemoval
                                      (elist[i],
                                       &x1, &y1, &x2, &y2,
                                       &n1, &n2);
                if (istat != 1) {
                    continue;
                }

                iforce = elist[i];
                istat = SwapEdgeForNodeRemoval (elist[i]);
                eptr->tflag = (char)istat;
                if (istat == 1) {
                    forced_swap[nforce] = iforce;
                    nforce++;
                    ndone++;
                    num_left--;
                }

                if (rptr->nedge != nlist) {
                    break;
                }
                if (num_left <= max_left) {
                    ndone = 0;
                    break;
                }
            }
            if (ndone == 0) {
                break;
            }
        }
    }

    if (nlist > 3) {
        return 0;
    }

/*
 * A local copy of the edge list is needed to
 * complete the node removal operation.
 */
    memcpy (elist3, elist, nlist * sizeof(int));
    elist = elist3;

/*
    There should now be either 2 or 3 edges left attached to the
    node.  If the node is interior to another triangle, there are
    3 attached edges.  If the node is on an edge of the network,
    there may be two or three attached edges.

    These remaining edges must be deleted and the surviving edges
    of a triangle that contained the node must be changed to reference
    the new triangle.
*/

/*
    Special case for node on the edge of the trimesh.
*/
    nptr = NodeList + nodenum;
    if (nptr->on_border == 1) {

    /*
     * Extra special case for border node with 3 edges still attached.
     */
        if (nlist == 3) {
            istat = RemoveBorderNode (nodenum, elist, nlist);

            if (istat == 1) {
                nptr = NodeList + nodenum;
                nptr->deleted = 1;
                for (i=0; i<nforce; i++) {
                    SwapEdge (forced_swap[i]);
                }
                csw_Free (forced_swap);
                forced_swap = NULL;
            }

            return istat;
        }

    /*
     * Two edges attached case.
     */
        for (i=0; i<nlist; i++) {
            RemoveEdgeFromNodeList (nodenum, elist[i]);
            eptr = EdgeList + elist[i];
            if (eptr->node1 == nodenum) {
                RemoveEdgeFromNodeList (eptr->node2, elist[i]);
            }
            else {
                RemoveEdgeFromNodeList (eptr->node1, elist[i]);
            }
        }
        WhackEdge (elist[0]);
        WhackEdge (elist[1]);
        nptr = NodeList + nodenum;
        nptr->deleted = 1;

        for (i=0; i<nforce; i++) {
            SwapEdge (forced_swap[i]);
        }

        return 1;

    }

/*
    Interior node case.

    First, arbitrarily pick a triangle from the triangles still using
    the node.  This will be used for the new triangle.
*/
    trinum = -1;
    eptr = NULL;
    nlist = rptr->nedge;
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + elist[i];
        if (eptr->tflag == 1) {
            continue;
        }
        trinum = eptr->tri1;
        if (trinum < 0) {
            trinum = eptr->tri2;
        }
        break;
    }

/*
    This should never happen, but it seems to aleviate
    compiler warnings.
*/
    if (eptr == NULL) {
        assert (eptr != NULL);
    }

/*
    The node now has 3 edges attached to it.  This means
    there are 3 triangles fanning out from the node and
    that the node is inside another triangle formed by
    the opposite edges of the fanning triangles.  These
    fanning triangles and opposite edges are identified
    here.
*/
    t1 = -1;
    t2 = -1;
    t3 = -1;
    et1 = -1;
    et2 = -1;
    et3 = -1;
    eptr = EdgeList + elist[0];
    t1 = eptr->tri1;
    t2 = eptr->tri2;

    et1 = OppositeEdge (TriangleList + t1, nodenum);
    if (t2 >= 0) {
        et2 = OppositeEdge (TriangleList + t2, nodenum);
    }
    for (i=1; i<nlist; i++) {
        eptr = EdgeList + elist[i];
        if (eptr->tri1 != t1  &&  eptr->tri1 != t2) {
            t3 = eptr->tri1;
            et3 = OppositeEdge (TriangleList + t3, nodenum);
            break;
        }
        if (eptr->tri2 != t1  &&  eptr->tri2 != t2) {
            t3 = eptr->tri2;
            if (t3 >= 0) {
                et3 = OppositeEdge (TriangleList + t3, nodenum);
            }
            break;
        }
    }

    for (i=0; i<nlist; i++) {
        RemoveEdgeFromNodeList (nodenum, elist[i]);
        eptr = EdgeList + elist[i];
        if (eptr->node1 == nodenum) {
            RemoveEdgeFromNodeList (eptr->node2, elist[i]);
        }
        else {
            RemoveEdgeFromNodeList (eptr->node1, elist[i]);
        }
    }

    for (i=0; i<nlist; i++) {
        WhackEdge (elist[i]);
    }

/*
 * If any fanning triangle or opposite edge could not be identified,
 * return without adjusting the topology.  This will leave a "hole"
 * in the trimesh.  This will happen if one of the edges connected
 * to the deleted node has only one triangle associated with it.
 * This edge is by definition on the border of the trimesh.
 */
    if (t1 < 0  ||  t2 < 0  ||  t3 < 0  ||
        et1 < 0  ||  et2 < 0  ||  et3 < 0) {

        for (i=0; i<nforce; i++) {
            SwapEdge (forced_swap[i]);
        }

        return 1;
    }

/*
 * Adjust the triangle and edge relationships for the three
 * opposite edges and for the single surviving triangle.
 */
    trinum = t1;
    tptr = TriangleList + trinum;
    memset (tptr, 0, sizeof(TRiangleStruct));
    tptr->edge1 = et1;
    tptr->edge2 = et2;
    tptr->edge3 = et3;

    eptr = EdgeList + et1;
    if (eptr->tri1 == t2  ||  eptr->tri1 == t3  ||  eptr->tri1 == -1) {
        eptr->tri1 = trinum;
    }
    if (eptr->tri2 == t2  ||  eptr->tri2 == t3  ||  eptr->tri2 == -1) {
        eptr->tri2 = trinum;
    }
    if (eptr->on_border == 1) {
        eptr->tri2 = -1;
    }

    eptr = EdgeList + et2;
    if (eptr->tri1 == t2  ||  eptr->tri1 == t3  ||  eptr->tri1 == -1) {
        eptr->tri1 = trinum;
    }
    if (eptr->tri2 == t2  ||  eptr->tri2 == t3  ||  eptr->tri2 == -1) {
        eptr->tri2 = trinum;
    }
    if (eptr->on_border == 1) {
        eptr->tri2 = -1;
    }

    eptr = EdgeList + et3;
    if (eptr->tri1 == t2  ||  eptr->tri1 == t3  ||  eptr->tri1 == -1) {
        eptr->tri1 = trinum;
    }
    if (eptr->tri2 == t2  ||  eptr->tri2 == t3  ||  eptr->tri2 == -1) {
        eptr->tri2 = trinum;
    }
    if (eptr->on_border == 1) {
        eptr->tri2 = -1;
    }

    nptr = NodeList + nodenum;
    nptr->deleted = 1;

    for (i=0; i<nforce; i++) {
        SwapEdge (forced_swap[i]);
    }

    return 1;

}  /*  end of private RemoveNode function  */



/*
 *************************************************************************

                 A d d E d g e T o N o d e L i s t

 *************************************************************************

  Add an edge to the list of edges attached to a specific node.  On
  success, 1 is returned.  If a memory allocation failure happens,
  -1 is returned.

*/

int CSWGrdTriangle::AddEdgeToNodeList (int nodenum, int edgenum)
{
    NOdeStruct       *nptr;
    RAwPointStruct   *rptr;

    int              rp, istat;

    if (RawPoints == NULL) {
        return -1;
    }

    nptr = NodeList + nodenum;
    rp = nptr->rp;
    if (rp >= 0) {
        rptr = RawPoints + rp;
    }
    else {
        rp = nptr->crp;
        if (rp < 0) {
            return 0;
        }
        rptr = ConstraintRawPoints + rp;
    }

    if (rptr->edgelist == NULL) return 1;

    istat = AddEdgeToRawPoint (rptr, edgenum);

    return istat;

}



/*
 *************************************************************************

              R e m o v e E d g e F r o m N o d e L i s t

 *************************************************************************

  Remove the specified edge from the list of edges attached to the node.
  This should only be called from the RemoveNode function.

*/

void CSWGrdTriangle::RemoveEdgeFromNodeList (int nodenum, int edgenum)
{
    int               *list, nlist, i, rp, n;
    NOdeStruct        *nptr;
    RAwPointStruct    *rptr;

    if (RawPoints == NULL) return;

    nptr = NodeList + nodenum;
    rp = nptr->rp;
    if (rp >= 0) {
        rptr = RawPoints + rp;
    }
    else {
        rp = nptr->crp;
        if (rp < 0) {
            return;
        }
        rptr = ConstraintRawPoints + rp;
    }

    list = rptr->edgelist;
    nlist = rptr->nedge;

    if (list == NULL  ||  nlist < 1) return;

    for (i=0; i<nlist; i++) {
        if (list[i] == edgenum) {
            list[i] = -1;
        }
    }

    n = 0;
    for (i=0; i<nlist; i++) {
        if (list[i] >= 0) {
            list[n] = list[i];
            n++;
        }
    }
    rptr->nedge = n;

    return;

}


/*
 *****************************************************************************************

                      R e m o v e Z e r o L e n g t h E d g e s

 *****************************************************************************************

  Remove any zero length edge and one of its endpoint nodes.

*/

void CSWGrdTriangle::RemoveZeroLengthEdges (void)
{
    int              i, n, ntry;
    double           tiny;
    int              do_write, ilast;

    if (RemoveZeroFlag == 0) {
        return;
    }

    tiny = AreaPerimeter / 20000.0;

    if (tiny < 1.e-40) {
        tiny = 1.e-40;
    }

    FlagEdgeNodes ();
    ntry = 0;

    do_write = csw_GetDoWrite ();

    ilast = -1;
    for (;;) {
        n = 0;
        for (i=0; i<NumEdges; i++) {
            if (EdgeList[i].length <= tiny  &&
                EdgeList[i].deleted == 0) {
                if (i == ilast  &&  do_write) {
                    printf ("stuck on edge %d ntry %d\n", i, ntry);
                }
                RemoveZeroLengthEdge (i);
                n++;
                break;
            }
        }
        if (n == 0) {
            break;
        }

        if (ntry > NumEdges) {
            printf ("Too many retries to remove zero length edges.\n");
            NtryFlag = 1;
            break;
        }
        ntry++;
    }

    return;

}  /* end of RemoveZeroLengthEdges function */


/*
 ************************************************************************************

                 R e m o v e Z e r o A r e a T r i a n g l e s

 ************************************************************************************

  If a triangle has essentially zero area, that means that its corner points are
  colinear.  In this case, the triangle needs to be "removed" from the topology
  to avoid problems when subsequently applying constraints.
*/

void CSWGrdTriangle::RemoveZeroAreaTriangles (void)
{
    int             i, ndone, ntry;
    int             iszero;

/*
 * !!!! Debug only.
 */
    int             do_write;
    int             e1, e2, e3, n1, n2, n3;
    FILE            *fptr;
    char            c200[200];

    if (RemoveZeroFlag == 0) return;

    fptr = NULL;
    do_write = csw_GetDoWrite ();;
    if (do_write) {
        fptr = fopen ("zeroarea.dat", "wb");
    }




/*
    The topology changes from triangle removal may introduce
    a new zero area triangle.  Thus, the removal loops until
    no triangles needed to be removed;
*/
    ExtendDiagonalsFlag = 1;
    RemoveNodeForZeroAreaFlag = 1;
    ntry = 0;
    for (;;) {

        BuildRawPointEdgeLists ();
        ndone = 0;

        for (i=0; i<NumTriangles; i++) {

            iszero = CheckZeroAreaTriangle (i);
            if (iszero == 0) {
                continue;
            }

        /*
         * !!!! debug only
         */
            if (fptr != NULL  &&  ntry == 0) {
                e1 = TriangleList[i].edge1;
                e2 = TriangleList[i].edge2;
                e3 = TriangleList[i].edge3;
                n1 = EdgeList[e1].node1;
                n2 = EdgeList[e1].node2;
                if (EdgeList[e2].node1 == n1  ||  EdgeList[e2].node1 == n2) {
                    n3 = EdgeList[e2].node2;
                }
                else {
                    n3 = EdgeList[e2].node1;
                }
                sprintf (c200,
                         "triangle number %d  Edges: %d %d %d  Nodes: %d %d %d\n",
                         i, e1, e2, e3, n1, n2, n3);
                fputs (c200, fptr);
            }



            RemoveZeroAreaTriangle (i);

            ndone++;

            break;
        }

    /*
     * !!!! debug only
     *
        if (fptr) {
            fclose (fptr);
            fptr = NULL;
        }
     */

        if (ndone == 0) {
            break;
        }

        if (ntry > NumTriangles) {
            printf ("Too many retries to remove zero area triangles.\n");
            NtryFlag = 1;
            break;
        }
        ntry++;
    }
    ExtendDiagonalsFlag = 0;
    RemoveNodeForZeroAreaFlag = 0;

    return;

}  /* end of private RemoveZeroAreaTriangles function */




/*
 ***********************************************************************************

                   R e m o v e Z e r o A r e a T r i a n g l e

 ***********************************************************************************

  This is only called from RemoveZeroAreaTriangles.  The longest edge of the triangle
  is either deleted (if it is not shared with another triangle) or swapped if it is
  shared.

  Do not use this function in any other fashion.  It assumes that all very short edges
  have been deleted prior to this function being called.

*/

void CSWGrdTriangle::RemoveZeroAreaTriangle (int trinum)
{
    TRiangleStruct    *tptr;
    EDgeStruct        *ep1, *ep2, *ep3, *epmax;
    int               t1;
    int               n1, n2, n3, nmin;
    double            dmin, dist;

    tptr = TriangleList + trinum;
    ep1 = EdgeList + tptr->edge1;
    ep2 = EdgeList + tptr->edge2;
    ep3 = EdgeList + tptr->edge3;

    if (ep1->length >= ep2->length  &&
        ep1->length >= ep3->length) {
        epmax = ep1;
    }
    else if (ep2->length >= ep1->length  &&
        ep2->length >= ep3->length) {
        epmax = ep2;
    }
    else {
        epmax = ep3;
    }

    t1 = epmax->tri1;
    if (t1 == trinum) {
        t1 = epmax->tri2;
    }

    if (t1 < 0) {
        RemoveEdgeFromNodeList (epmax->node1, epmax - EdgeList);
        RemoveEdgeFromNodeList (epmax->node2, epmax - EdgeList);
        epmax->deleted = 1;
    }
    else {
        n1 = ep1->node1;
        n2 = ep1->node2;
        if (ep2->node1 == n1  ||  ep2->node1 == n2) {
            n3 = ep2->node2;
        }
        else {
            n3 = ep2->node1;
        }
        dist = NodeDistanceXYZ (n1, n2);
        dmin = dist;
        nmin = n1;
        dist = NodeDistanceXYZ (n1, n3);
        if (dist < dmin) {
            dmin = dist;
            nmin = n1;
        }
        dist = NodeDistanceXYZ (n2, n3);
        if (dist < dmin) {
            nmin = n2;
        }

        if (dist >= AreaPerimeter / 20000.0) {
            nmin = FindMiddleNode (n1, n2, n3);
        }

        if (nmin < 0) {
            return;
        }

        RemoveNode (nmin);

    }

    return;

}  /* end of private RemoveZeroAreaTriangle function */


int CSWGrdTriangle::FindMiddleNode (int n1, int n2, int n3)
{
    double       x1, y1, z1,
                 x2, y2, z2,
                 x3, y3, z3,
                 dx, dy, dz;

    x1 = NodeList[n1].x;
    y1 = NodeList[n1].y;
    z1 = NodeList[n1].z;
    x2 = NodeList[n2].x;
    y2 = NodeList[n2].y;
    z2 = NodeList[n2].z;
    x3 = NodeList[n3].x;
    y3 = NodeList[n3].y;
    z3 = NodeList[n3].z;

    dx = x3 - x1;
    if (dx < 0.0) dx = -dx;
    dy = y3 - y1;
    if (dy < 0.0) dy = -dy;
    dz = z3 - z1;
    if (dz < 0.0) dz = -dz;

    if (dx >= dy  &&  dx >= dz) {
        if ((x1-x2) * (x2-x3) > 0.0) {
            return n2;
        }
    }
    else if (dy >= dx  &&  dy >= dz) {
        if ((y1-y2) * (y2-y3) > 0.0) {
            return n2;
        }
    }
    else {
        if ((z1-z2) * (z2-z3) > 0.0) {
            return n2;
        }
    }

    dx = x3 - x2;
    if (dx < 0.0) dx = -dx;
    dy = y3 - y2;
    if (dy < 0.0) dy = -dy;
    dz = z3 - z2;
    if (dz < 0.0) dz = -dz;

    if (dx >= dy  &&  dx >= dz) {
        if ((x2-x1) * (x1-x3) > 0.0) {
            return n1;
        }
    }
    else if (dy >= dx  &&  dy >= dz) {
        if ((y2-y1) * (y1-y3) > 0.0) {
            return n1;
        }
    }
    else {
        if ((z2-z1) * (z1-z3) > 0.0) {
            return n1;
        }
    }

    dx = x1 - x2;
    if (dx < 0.0) dx = -dx;
    dy = y1 - y2;
    if (dy < 0.0) dy = -dy;
    dz = z1 - z2;
    if (dz < 0.0) dz = -dz;

    if (dx >= dy  &&  dx >= dz) {
        if ((x2-x3) * (x3-x1) > 0.0) {
            return n3;
        }
    }
    else if (dy >= dx  &&  dy >= dz) {
        if ((y2-y3) * (y3-y1) > 0.0) {
            return n3;
        }
    }
    else {
        if ((z2-z3) * (z3-z1) > 0.0) {
            return n3;
        }
    }

    return -1;

}


/*
 **************************************************************************

              C a l c E q u i l a t e r a l n e s s

 **************************************************************************

  Calculate a number that measures how equilateral a
  triangle is.  The number returned is based on the ratio between the
  total length of the two shortest sides to the length of the longest
  side.  For a perfectly equilateral triangle, this value is 2.0.  For
  a zero area triangle, this value is 1.0.  The ratio is scaled so the
  return value is between zero and 1.

*/

double CSWGrdTriangle::CalcEquilateralness (int n1, int n2, int n3)
{
    double     d1, d2, d3, result, dtot, dmax;

    d1 = NodeDistanceXYZ (n1, n2);
    d2 = NodeDistanceXYZ (n2, n3);
    d3 = NodeDistanceXYZ (n3, n1);

    dtot = d1 + d2 + d3;

    dmax = d1;
    if (d2 > dmax) {
        dmax = d2;
    }
    if (d3 > dmax) {
        dmax = d3;
    }

    dtot -= dmax;

    result = dtot / dmax - 1.0;

    return result;

}  /* end of private CalcEquilateralness function */





#if 0
/*
 ********************************************************************************

                          P o i n t O n S e g m e n t

 ********************************************************************************

  Return 1 if the xp, yp point is almost exactly on top of the line segment
  defined by the n1 and n2 nodes.  Return zero otherwise.

*/

int CSWGrdTriangle::PointOnSegment (double xp, double yp,
                           int n1, int n2, double *dperpout)

{
    double             x1, y1, x2, y2, xint, yint,
                       dx, dy, tiny, dist;
    int                istat;
    NOdeStruct         *nptr;

    tiny = GrazeDistance * 10;

    nptr = NodeList + n1;
    x1 = nptr->x;
    y1 = nptr->y;
    nptr = NodeList + n2;
    x2 = nptr->x;
    y2 = nptr->y;

    istat = gpf_perpintpoint2 (x1, y1, x2, y2, xp, yp, &xint, &yint);
    if (istat == 0) {
        return 0;
    }

  /*
   * The perpintpoint can return 1 if the point grazes the segment
   * endpoint.  Check for exact inside segment here.
   */
    if ( !((x1-xint)*(xint-x2) >= 0.0f  ||  (y1-yint)*(yint-y2) >= 0.0f)) {
        return 0;
    }

    dx = xp - xint;
    dy = yp - yint;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (dperpout != NULL) {
        *dperpout = dist;
    }

    if (dist <= tiny) {
        SegmentIntX = xint;
        SegmentIntY = yint;
        return 1;
    }

    return 0;

}  /* end of private PointOnSegment function */
#endif





/*
 ********************************************************************************

                          N o d e O n S e g m e n t

 ********************************************************************************

  Return 1 if the nchk node is almost exactly on top of the line segment
  defined by the n1 and n2 nodes.  Return zero otherwise.

*/

int CSWGrdTriangle::NodeOnSegment (int nchk, int n1, int n2, double *dperpout)

{
    double             x1, y1, x2, y2, xp, yp, xint, yint,
                       dx, dy, tiny, dist;
    int                istat;
    NOdeStruct         *nptr;

    tiny = GrazeDistance * 10;

    nptr = NodeList + nchk;
    xp = nptr->x;
    yp = nptr->y;
    nptr = NodeList + n1;
    x1 = nptr->x;
    y1 = nptr->y;
    nptr = NodeList + n2;
    x2 = nptr->x;
    y2 = nptr->y;

    istat = gpf_perpintpoint2 (x1, y1, x2, y2, xp, yp, &xint, &yint);
    if (istat == 0) {
        return 0;
    }

  /*
   * The perpintpoint can return 1 if the point grazes the segment
   * endpoint.  Check for exact inside segment here.
   */
    if ( !((x1-xint)*(xint-x2) >= 0.0f  ||  (y1-yint)*(yint-y2) >= 0.0f)) {
        return 0;
    }

    dx = xp - xint;
    dy = yp - yint;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (dperpout != NULL) {
        *dperpout = dist;
    }

    if (dist <= tiny) {
        SegmentIntX = xint;
        SegmentIntY = yint;
        return 1;
    }

    return 0;

}  /* end of private NodeOnSegment function */



/*
 ****************************************************************************

                     N o d e O n E d g e

 ****************************************************************************

*/

int CSWGrdTriangle::NodeOnEdge (int tnum, int nodenum)
{
    TRiangleStruct         *tptr;
    EDgeStruct             *eptr;
    int                    istat, edgenum;
    double                 dperp, dpmin;

    tptr = TriangleList + tnum;
    edgenum = -1;
    dpmin = 1.e30;

    eptr = EdgeList + tptr->edge1;
    istat = NodeOnSegment (nodenum, eptr->node1, eptr->node2, &dperp);
    if (istat == 1) {
        if (dperp < dpmin) {
            edgenum = tptr->edge1;
            dpmin = dperp;
            if (SnapToEdge) {
                NodeList[nodenum].x = SegmentIntX;
                NodeList[nodenum].y = SegmentIntY;
            }
        }
    }

    eptr = EdgeList + tptr->edge2;
    istat = NodeOnSegment (nodenum, eptr->node1, eptr->node2, &dperp);
    if (istat == 1) {
        if (dperp < dpmin) {
            edgenum = tptr->edge2;
            dpmin = dperp;
            if (SnapToEdge) {
                NodeList[nodenum].x = SegmentIntX;
                NodeList[nodenum].y = SegmentIntY;
            }
        }
    }

    eptr = EdgeList + tptr->edge3;
    istat = NodeOnSegment (nodenum, eptr->node1, eptr->node2, &dperp);
    if (istat == 1) {
        if (dperp < dpmin) {
            edgenum = tptr->edge3;
            dpmin = dperp;
            if (SnapToEdge) {
                NodeList[nodenum].x = SegmentIntX;
                NodeList[nodenum].y = SegmentIntY;
            }
        }
    }

    return edgenum;

}  /* end of private NodeOnEdge function */





/*
 ******************************************************************************

                          S p l i t F r o m E d g e

 ******************************************************************************

 Split the triangle or triangles shared by the specified edge by
 connecting an edge from the specified node to the opposite node
 on the triangle or triangles.  If the edge only has a single triangle 
 that uses it, thaat triangle will be divided into two triangles.
 If the edge has two triangles that use it, each of the two will
 be divided into two triangles.  The new edges, nodes and triangles 
 are all merged into the topology if this function returns success
 (a return value of 1).  If a memory allocation error occurs, the
 function returns -1 and no changes are made to the topology.
*/

int CSWGrdTriangle::SplitFromEdge (int edgenum, int nodenum, int *elist)
{
    int              told1, told2, tnew1, tnew2;
    int              enew0, enew1, enew2;
    EDgeStruct       *ep, *epnew0, *epnew1, *epnew2;

/*
 * Check for obvious errors.
 */
    if (nodenum < 0  ||  nodenum >= NumNodes  ||
        edgenum < 0  ||  edgenum >= NumEdges) {
        return -1;
    }

    if (NodeList[nodenum].deleted) {
        return -1;
    }

    if (EdgeList[edgenum].deleted) {
        return -1;
    }

/*
 * Create new undefined triangles and edges as needed.
 */
    told1 = EdgeList[edgenum].tri1;
    told2 = EdgeList[edgenum].tri2;

    enew0 = AddEdge (-1, -1, -1, -1, 0);
    enew1 = AddEdge (-1, -1, -1, -1, 0);
    if (told2 >= 0) {
        enew2 = AddEdge (-1, -1, -1, -1, 0);
    }
    else {
        enew2 = -1;
    }

    tnew1 = AddTriangle (-1, -1, -1, 0);
    if (told2 >= 0) {
        tnew2 = AddTriangle (-1, -1, -1, 0);
    }
    else {
        tnew2 = -1;
    }

/*
 * Return error if any needed edge or triangle could not be created.
 */
    if (told2 < 0) {
        if (enew0 < 0  ||  enew1 < 0  ||  tnew1 < 0) {
            return -1;
        }
    }
    else {
        if (enew0 < 0  ||  enew1 < 0  ||  enew2 < 0  ||
            tnew1 < 0  ||  tnew2 < 0) {
            return -1;
        }
    }

/*
 * Fill in the edge node values where possible.
 */
    epnew0 = EdgeList + enew0;
    epnew1 = EdgeList + enew1;
    if (enew2 >= 0) {
        epnew2 = EdgeList + enew2;
    }
    else {
        epnew2 = NULL;
    }
    ep = EdgeList + edgenum;

    RemoveEdgeFromNodeList (ep->node2, edgenum);
    epnew0->node1 = nodenum;
    epnew0->node2 = ep->node2;
    ep->node2 = nodenum;
    AddEdgeToNodeList (nodenum, edgenum);
    AddEdgeToNodeList (nodenum, enew0);
    AddEdgeToNodeList (nodenum, enew1);
    epnew1->node1 = nodenum;
    if (epnew2 != NULL) {
        epnew2->node1 = nodenum;
        AddEdgeToNodeList (nodenum, enew2);
    }

    if (elist != NULL) {
        elist[0] = enew2;
        elist[1] = enew0;
        elist[2] = enew1;
        elist[3] = edgenum;
    }

/*
 * Split the first triangle every time.
 */
    SplitSingleTriFromEdge (told1, edgenum,
                            enew0, enew1,
                            tnew1, 1);

/*
 * Split the second triangle if it exists.
 */
    if (told2 >= 0) {
        SplitSingleTriFromEdge (told2, edgenum,
                                enew0, enew2,
                                tnew2, 2);
    }

    return 1;

}  /* end of private SplitFromEdge function */


/*
 * This function is only used from the SplitFromEdge function.
 * Do not use it in any other context.
 */
void CSWGrdTriangle::SplitSingleTriFromEdge (int told,
                                    int eold,
                                    int enew,
                                    int esplit,
                                    int tnew,
                                    int edgetrinum)
{
    EDgeStruct        *epnew, *epsplit, *epold, *ep;
    TRiangleStruct    *tpold, *tpnew;
    int               e11, e12, e13,
                      e21, e22, e23;
    int               nopp;

/*
 * Get pointers to the edge and triangle structures to be used.
 */
    epold = EdgeList + eold;
    epnew = EdgeList + enew;
    epsplit = EdgeList + esplit;

    tpold = TriangleList + told;
    tpnew = TriangleList + tnew;

/*
 * The split edge needs its node2 value assigned.
 */
    nopp = OppositeNode (told, eold);
    epsplit->node2 = nopp;

/*
 * Figure edges for split triangles, but don't put them in the
 * triangle structure until all edges have been figured.
 */
    e11 = eold;
    e12 = esplit;
    e13 = FindTriangleEdge (tpold, nopp, epold->node1);
    if (e13 < 0) {
        assert (0);
    }

    e21 = enew;
    e22 = esplit;
    e23 = FindTriangleEdge (tpold, nopp, epnew->node2);
    if (e23 < 0) {
        assert (0);
    }

/*
 * Put the edges into the triangle structures.
 */

    tpold->edge1 = e11;
    tpold->edge2 = e12;
    tpold->edge3 = e13;

    tpnew->edge1 = e21;
    tpnew->edge2 = e22;
    tpnew->edge3 = e23;

/*
 * Update the edges to reflect the new triangles.
 */
    epsplit->tri1 = told;
    epsplit->tri2 = tnew;

    ep = EdgeList + e23;
    if (ep->tri1 == told) {
        ep->tri1 = tnew;
    }
    else {
        ep->tri2 = tnew;
    }

    if (edgetrinum == 1) {
        epnew->tri1 = tnew;
    }
    else {
        epnew->tri2 = tnew;
    }

}


/*
 ****************************************************************************

           g r d _ r e m o v e _ n u l l s _ f r o m _ t r i m e s h

 ****************************************************************************

*/

int CSWGrdTriangle::grd_remove_nulls_from_trimesh (TRiangleStruct *triangles, int *numtriangles,
                                   EDgeStruct *edges, int *numedges,
                                   NOdeStruct *nodes, int *numnodes,
                                   double nullval, int edge_swap_flag)
{
    int                 i, i1, i2, n1, n2;
    EDgeStruct          *eptr;
    double              dt;
    int                 do_write;
    double              v6[6];
    char                fname[100];

    FreeMem ();

    TriangleList = triangles;
    NumTriangles = *numtriangles;
    EdgeList = edges;
    NumEdges = *numedges;
    NodeList = nodes;
    NumNodes = *numnodes;

    NullValue = nullval;
    EdgeSwapFlag = edge_swap_flag;
    if (edge_swap_flag <= 0) {
        EdgeSwapFlag = SWAP_FOR_NULL_REMOVAL;
    }
    CheckNullPointers = 0;
    if (nullval == GRD_CHECK_FOR_NULL_POINTER) {
        CheckNullPointers = 1;
    }

/*
    Swap edges that have one null and one non null end point.
*/
    if (edge_swap_flag >= 0) {
        for (i=0; i<*numedges; i++) {
            eptr = EdgeList + i;
            n1 = eptr->node1;
            n2 = eptr->node2;
            i1 = NodeIsNull (n1);
            i2 = NodeIsNull (n2);
            if (i1 != i2) {
                SwapEdge (i);
            }
        }
    }

/*
    Whack the edges that have one or more null nodes.
    The triangles that use the edge in common also need
    to be whacked.  No additional topology is built.
    This process leaves "holes" in the tri mesh.
*/
    for (i=0; i<*numedges; i++) {
        eptr = EdgeList + i;
        n1 = eptr->node1;
        n2 = eptr->node2;
        i1 = NodeIsNull (n1);
        i2 = NodeIsNull (n2);
        if (i1 || i2) {
            WhackEdge (i);
        }
        if (i1) {
            NodeList[n1].deleted = 1;
        }
        if (i2) {
            NodeList[n2].deleted = 1;
        }
    }

    if (DataCheckFunc != NULL) {
        for (i=0; i<*numnodes; i++) {
            dt = DataCheckFunc ((void *)(NodeList[i].client_data));
            dt = dt;
        }
    }

    RemoveDeletedElements ();

    do_write = csw_GetDoWrite ();
    if (do_write) {
        sprintf (fname, "nulls_removed.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    *numtriangles = NumTriangles;
    *numedges = NumEdges;
    *numnodes = NumNodes;

    if (DataCheckFunc != NULL) {
        for (i=0; i<*numnodes; i++) {
            dt = DataCheckFunc ((void *)(NodeList[i].client_data));
            dt = dt;
        }
    }

    EdgeSwapFlag = GRD_SWAP_ANY;
    CheckNullPointers = 0;

    if (DontFree == 0) {
        ListNull ();
        FreeMem();
    }

    return 1;

}  /* end of function grd_remove_nulls_from_trimesh */





/*
 ********************************************************************************

                             N o d e I s N u l l

 ********************************************************************************

  Return 1 if the value of the node is null or zero otherwise.  Generally, if
  the value is pretty close or outside the range of the null value, this
  function returns 1.  An exception is if the null value is zero.  In this
  case the z value must be exactly zero to qualify as a null value.

*/

int CSWGrdTriangle::NodeIsNull (int nodenum)
{
    NOdeStruct         *nptr;
    double             zval;

    nptr = NodeList + nodenum;

    if (CheckNullPointers) {
        if (nptr->client_data == NULL) return 1;
        return 0;
    }
    zval = nptr->z;

    if (zval >= 1.e19) return 1;
    if (zval <= -1.e18) return 1;

    if (NullValue == 0.0) {
        if (zval == 0.0) return 1;
        return 0;
    }

    if (NullValue > -1.e10  &&  NullValue < 1.e10) {
        return 0;
    }

    if (NullValue < 0.0) {
        if (zval <= NullValue / 10.0) return 1;
        return 0;
    }

    if (zval >= NullValue / 10.0) return 1;

    return 0;

} /* end of private NodeIsNull function */




/*
 *********************************************************************************

                            W h a c k E d g e

 *********************************************************************************

  Mark the specified edge as deleted.  Also remove it and its shared triangles
  from the topology.

*/

void CSWGrdTriangle::WhackEdge (int edgenum)
{
    EDgeStruct       *eptr, *ep;
    TRiangleStruct   *tp;
    int              tnum;

    eptr = EdgeList + edgenum;
    if (eptr->deleted) return;
    if (eptr->tri1 == -1  &&  eptr->tri2 == -1) {
        RemoveEdgeFromNodeList (eptr->node1, edgenum);
        RemoveEdgeFromNodeList (eptr->node2, edgenum);
        eptr->deleted = 1;
        return;
    }

    tnum = eptr->tri1;
    if (tnum >= 0) {
        tp = TriangleList + tnum;
        if (tp->edge1 != edgenum) {
            ep = EdgeList + tp->edge1;
            if (ep->tri1 == tnum) {
                ep->tri1 = ep->tri2;
            }
            ep->tri2 = -1;
        }

        if (tp->edge2 != edgenum) {
            ep = EdgeList + tp->edge2;
            if (ep->tri1 == tnum) {
                ep->tri1 = ep->tri2;
            }
            ep->tri2 = -1;
        }

        if (tp->edge3 != edgenum) {
            ep = EdgeList + tp->edge3;
            if (ep->tri1 == tnum) {
                ep->tri1 = ep->tri2;
            }
            ep->tri2 = -1;
        }

        tp->deleted = 1;
    }

    tnum = eptr->tri2;
    if (tnum >= 0) {
        tp = TriangleList + tnum;
        if (tp->edge1 != edgenum) {
            ep = EdgeList + tp->edge1;
            if (ep->tri1 == tnum) {
                ep->tri1 = ep->tri2;
            }
            ep->tri2 = -1;
        }

        if (tp->edge2 != edgenum) {
            ep = EdgeList + tp->edge2;
            if (ep->tri1 == tnum) {
                ep->tri1 = ep->tri2;
            }
            ep->tri2 = -1;
        }

        if (tp->edge3 != edgenum) {
            ep = EdgeList + tp->edge3;
            if (ep->tri1 == tnum) {
                ep->tri1 = ep->tri2;
            }
            ep->tri2 = -1;
        }

        tp->deleted = 1;
    }

    RemoveEdgeFromNodeList (eptr->node1, edgenum);
    RemoveEdgeFromNodeList (eptr->node2, edgenum);

    eptr->tri1 = -1;
    eptr->tri2 = -1;
    eptr->deleted = 1;

    return;

}  /* end of private WhackEdge function */



/*
 ************************************************************************

  g r d _ r e m o v e _ d e l e t e d _ t r i m e s h _ e l e m e n t s

 ************************************************************************

*/

int CSWGrdTriangle::grd_remove_deleted_trimesh_elements (
    NOdeStruct    *nodes,
    int           *num_nodes,
    EDgeStruct    *edges,
    int           *num_edges,
    TRiangleStruct  *tris,
    int           *num_tris)
{
    int           istat;

    NodeList = nodes;
    EdgeList = edges;
    TriangleList = tris;
    NumNodes = *num_nodes;
    NumEdges = *num_edges;
    NumTriangles = *num_tris;

    istat =
      RemoveDeletedElements ();
    if (istat == -1) {
        return -1;
    }

    *num_nodes = NumNodes;
    *num_edges = NumEdges;
    *num_tris = NumTriangles;

    ListNull ();
    FreeMem ();

    return 1;
}



/*
 ************************************************************************

               R e m o v e D e l e t e d E l e m e n t s

 ************************************************************************

  Physically remove the deleted elements from the private tri mesh lists.
  The non deleted elements are repositioned to the start of the lists
  and the pointers that maintain the topology are altered accordingly.

*/

int CSWGrdTriangle::RemoveDeletedElements (void)
{
    int               *nodelookup,
                      *edgelookup,
                      *trilookup,
                      n, i, j, n1, n2, n3;
    int               td1, td2;
    TRiangleStruct    *tp;
    EDgeStruct        *ep;

/*
    Create lookup table space for edges, nodes and triangles.
*/
    n = NumNodes + NumEdges + NumTriangles;
    nodelookup = (int *)csw_Calloc (n * sizeof(int));
    if (nodelookup == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    edgelookup = nodelookup + NumNodes;
    trilookup = edgelookup + NumEdges;

/*
    Move the surviving nodes to the start of the node list
    and use the node lookup list to record the move.
*/
    j = 0;
    for (i=0; i<NumNodes; i++) {
        if (NodeList[i].deleted) {
            nodelookup[i] = -1;
            continue;
        }
        if (j != i) {
            memcpy (NodeList+j, NodeList+i, sizeof(NOdeStruct));
        }
        nodelookup[i] = j;
        j++;
    }
    NumNodes = j;

/*
 *  If either node of an edge is deleted, delete the edge also.
 */
    for (i=0; i<NumEdges; i++) {
        if (EdgeList[i].deleted) {
            continue;
        }
        n1 = EdgeList[i].node1;
        n2 = EdgeList[i].node2;
        if (nodelookup[n1] < 0  ||  nodelookup[n2] < 0) {
            EdgeList[i].deleted = 1;
        }
    }

/*
 *  If both triangles are deleted, delete the edge also.
 */
    for (i=0; i<NumEdges; i++) {
        if (EdgeList[i].deleted) {
            continue;
        }
        td1 = 0;
        td2 = 0;
        n1 = EdgeList[i].tri1;
        n2 = EdgeList[i].tri2;
        if (n1 < 0) {
            td1 = 1;
        }
        else {
            td1 = TriangleList[n1].deleted;
        }
        if (n2 < 0) {
            td2 = 1;
        }
        else {
            td2 = TriangleList[n2].deleted;
        }
        if (td1 == 1  &&  td2 == 1) {
            EdgeList[i].deleted = 1;
        }
    }

/*
    Move the surviving edges to the start of the edge list
    and use the edge lookup list to record the move.
*/
    j = 0;
    for (i=0; i<NumEdges; i++) {
        if (EdgeList[i].deleted) {
            edgelookup[i] = -1;
            continue;
        }
        if (EdgeList[i].tri1 == -1) {
            assert (EdgeList[i].tri2 >= 0);
            EdgeList[i].tri1 = EdgeList[i].tri2;
            EdgeList[i].tri2 = -1;
            continue;
        }
        if (j != i) {
            memcpy (EdgeList+j, EdgeList+i, sizeof(EDgeStruct));
        }
        edgelookup[i] = j;
        j++;
    }
    NumEdges = j;

/*
 * Convert the edge pairid numbers to the shifted edge list.
 */
    for (i=0; i<NumEdges; i++) {
        if (EdgeList[i].pairid > 0) {
            j = EdgeList[i].pairid - 1;
            EdgeList[i].pairid = edgelookup[j] + 1;
        }
    }

    for (i=0; i<NumTriangles; i++) {
        if (TriangleList[i].deleted) {
            continue;
        }
        n1 = TriangleList[i].edge1;
        n2 = TriangleList[i].edge2;
        n3 = TriangleList[i].edge3;
        if (edgelookup[n1] < 0  ||
            edgelookup[n2] < 0  ||
            edgelookup[n3] < 0) {
            TriangleList[i].deleted = 1;
        }
    }

/*
    Move the surviving triangles to the start of the triangle list
    and record the move using the triangle lookup.
*/
    j = 0;
    for (i=0; i<NumTriangles; i++) {
        if (TriangleList[i].deleted) {
            trilookup[i] = -1;
            continue;
        }
        if (j != i) {
            memcpy (TriangleList+j, TriangleList+i, sizeof(TRiangleStruct));
        }
        trilookup[i] = j;
        j++;
    }
    NumTriangles = j;

/*
    Reassign the triangle edges using the edge lookup table.
*/
    for (i=0; i<NumTriangles; i++) {
        tp = TriangleList + i;
        j = tp->edge1;
        tp->edge1 = edgelookup[j];
        j = tp->edge2;
        tp->edge2 = edgelookup[j];
        j = tp->edge3;
        tp->edge3 = edgelookup[j];
    }

/*
    Reassign the edge endpoints using the node lookup table.
    Reassign the edge triangles using the triangle lookup table.
*/
    for (i=0; i<NumEdges; i++) {
        ep = EdgeList + i;
        j = ep->node1;
        ep->node1 = nodelookup[j];
        j = ep->node2;
        ep->node2 = nodelookup[j];
        j = ep->tri1;
        ep->tri1 = trilookup[j];
        j = ep->tri2;
        if (j >= 0) {
            ep->tri2 = trilookup[j];
        }
        else {
            ep->tri2 = -1;
        }
        if (ep->tri1 < 0  &&  ep->tri2 >= 0) {
            ep->tri1 = ep->tri2;
            ep->tri2 = -1;
        }
        if (ep->tri1 < 0  &&  ep->tri2 < 0) {
            RemoveEdgeFromNodeList (ep->node1, ep - EdgeList);
            RemoveEdgeFromNodeList (ep->node2, ep - EdgeList);
            ep->deleted = 1;
        }
    }

    csw_Free (nodelookup);

    return 1;

}  /* end of private RemoveDeletedElements function */






/*
  ****************************************************************************

           g r d _ p o i n t e r _ g r i d _ t o _ t r i m e s h

  ****************************************************************************

    Convert a grid (with or without faults) to a trimesh.

*/

int CSWGrdTriangle::grd_pointer_grid_to_trimesh
                        (void **gridin, int nc, int nr,
                         double x1, double y1, double x2, double y2,
                         double *xlines, double *ylines, double *zlines,
                         int *linepoints, int nlines,
                         int trimesh_style,
                         NOdeStruct **nodes_out, EDgeStruct **edges_out,
                         TRiangleStruct **triangles_out,
                         int *num_nodes_out, int *num_edges_out,
                         int *num_triangles_out)
{
    int                  i, j, k, n, offset, edges_per_row, nlv;
    int                  maxraw, ebase, tbase, ke, kt, triangles_per_row;
    int                  top_ebase, constraint_start;
    int                  rpchk;
    void                 **grid;
    double               xt, yt, xspace, yspace, xyspace;
    NOdeStruct           *nptr;
    EDgeStruct           *eptr;
    TRiangleStruct       *tptr;
    RAwPointStruct       *rptr, *rp_prev, *rp_now, *rpstart;

/*
    No shifting is done cuz no z values are available for interpolation.
*/
    trimesh_style = trimesh_style;
    CheckNullPointers = 1;

/*
    Try and allocate a chunk of memory that is a bit
    larger than what will be needed to do the work.
    If the allocation fails, decimate the grid and
    try again until it succeeds.
*/
    grid = gridin;

/*
    Allocate space for the raw lines list to be filled in later.
*/
    n = 0;
    for (i=0; i<nlines; i++) {
        n += linepoints[i];
    }

    n += nlines * 2;
    RawLines = (RAwLineSegStruct *)csw_Calloc (n*sizeof(RAwLineSegStruct));
    NumRawLines = 0;
    if (RawLines == NULL) {
        grd_utils_ptr->grd_set_err(1);
        FreeMem ();
        if (grid != gridin) csw_Free (grid);
        CheckNullPointers = 0;
        return -1;
    }

/*
    Allocate space for the raw points, which includes only the grid
    nodes and the line points.
*/
    n = nc * nr;
    n += nr;
    nlv = 0;
    for (i=0; i<nlines; i++) {
        n += linepoints[i];
        nlv += linepoints[i];
    }
    RawPoints = (RAwPointStruct *)csw_Calloc (n * sizeof(RAwPointStruct));
    NumRawPoints = 0;
    MaxRawPoints = n;
    if (RawPoints == NULL) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        if (grid != gridin) csw_Free (grid);
        CheckNullPointers = 0;
        return -1;
    }
    maxraw = n;

    rptr = RawPoints + nc * nr + nr;
    ConstraintPointStart = -1;
    if (nlv > 0) {
        ConstraintPointStart = rptr - RawPoints;
    }
    constraint_start = ConstraintPointStart;
    n = 0;
    for (i=0; i<nlines; i++) {
        rpstart = rptr;
        rp_prev = rptr;
        for (j=0; j<linepoints[i]-1; j++) {
            rpchk = SnapToRawPoint (constraint_start, rptr-RawPoints, xlines[n], ylines[n]);
            if (rpchk >= 0) {
                rp_now = RawPoints + rpchk;
            }
            else {
                rptr->x = xlines[n];
                rptr->y = ylines[n];
                rptr->z = zlines[n];
                rptr->nodenum = -1;
                rptr->deleted = 0;
                rp_now = rptr;
                rptr++;
            }

        /*
            Each line segment is saved for the application
            of constraint lines after the unconstrained
            triangulation is finished.
        */
            if (j > 0) {
                AddRawLineSeg(rp_prev, rp_now, i);
            }
            n++;
            rp_prev = rp_now;
        }
        AddRawLineSeg (rp_prev, rpstart, i);
        n++;
    }
    NumRawPoints = rptr - RawPoints;

/*
    Allocate space for the initial triangle, edge and node lists.
    The initial sizes are sufficient for a non constrained trimesh
    but can grow larger if constraints are specified.
*/
    n = nc * nr * 2;
    n += nr;
    TriangleList = (TRiangleStruct *)csw_Calloc (n * sizeof(TRiangleStruct));
    if (!TriangleList) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        if (grid != gridin) csw_Free (grid);
        CheckNullPointers = 0;
        return -1;
    }
    MaxTriangles = n;
    NumTriangles = 0;

    n = nr * (nc * 3 + 1);
    n += nr;
    EdgeList = (EDgeStruct *)csw_Calloc (n * sizeof(EDgeStruct));
    if (!EdgeList) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        if (grid != gridin) csw_Free (grid);
        CheckNullPointers = 0;
        return -1;
    }
    MaxEdges = n;
    NumEdges = 0;

    n = nc * nr;
    if (maxraw > n) n = maxraw;
    n += nr;
    NodeList = (NOdeStruct *)csw_Calloc (n * sizeof(NOdeStruct));
    if (!NodeList) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        if (grid != gridin) csw_Free (grid);
        CheckNullPointers = 0;
        return -1;
    }
    MaxNodes = n;
    NumNodes = 0;

/*
    Set the x and y coordinates of the trimesh nodes and raw points to the
    coordinates of the grid nodes.
*/
    xspace = (x2 - x1) / (double)(nc - 1);
    yspace = (y2 - y1) / (double)(nr - 1);
    xyspace = xspace * xspace + yspace * yspace;
    xyspace = sqrt (xyspace);

    for (i=0; i<nr; i++) {
        yt = y1 + i * yspace;
        offset = i * nc;
        for (j=0; j<nc; j++) {
            xt = x1 + j * xspace;
            k = offset + j;
            nptr = NodeList + k;
            nptr->x = xt;
            nptr->y = yt;
            nptr->z = 1.e30;
            nptr->client_data = grid[k];
            nptr->adjusting_node = -1;
            nptr->spillnum = -1;
            nptr->rp = k;
            nptr->crp = -1;
            rptr = RawPoints + k;
            rptr->x = xt;
            rptr->y = yt;
            rptr->z = 1.e30;
            rptr->nodenum = k;
        }
    }
    NumNodes = nc * nr;

/*
   Create the edges.  Each grid cell has an edge on its left side, on its
   bottom side and a diagonal edge.  For cells in even numbered rows, the
   diagonal is from lower left to upper right.  For cells in odd number
   rows, the diagonal is from upper left to lower right. This is done to
   connect more equilateral triangles in conjuction with the left shifting
   of the nodes in the odd rows.  The numbers for these edges, within the
   cell are 1, 2 and 3 respectively.  Each cell's numbering starts at 3 *
   the column number of the node at the lower left corner of the cell.
*/
    edges_per_row = (nc - 1) * 3 + 1;
    triangles_per_row = (nc - 1) * 2;

/*
   Do all but the top row.
*/
    for (i=0; i<nr-1; i++) {
        offset = i * nc;
        ebase = i * edges_per_row;
    /*
       Do all but the right most column.
    */
        for (j=0; j<nc-1; j++) {
            k = ebase + j * 3;
            eptr = EdgeList + k;
            eptr->node1 = offset + j;
            eptr->node2 = offset + j + nc;
            eptr->length = yspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;

            eptr = EdgeList + k + 1;
            eptr->node1 = offset + j;
            eptr->node2 = offset + j + 1;
            eptr->length = xspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;

            eptr = EdgeList + k + 2;
            eptr->node1 = offset + j;
            eptr->node2 = offset + j + nc + 1;
            eptr->length = xyspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;
        }
    /*
       Do the right most column.
    */
        k = ebase + edges_per_row - 1;
        eptr = EdgeList + k;
        eptr->node1 = offset + j;
        eptr->node2 = offset + j + nc;
        eptr->length = yspace;
        eptr->tri1 = -1;
        eptr->tri2 = -1;
    }
/*
    Do the top row.
*/
    offset = (nr - 1) * nc;
    top_ebase = (nr - 1) * edges_per_row;
    for (j=0; j<nc-1; j++) {
        k = top_ebase + j;
        eptr = EdgeList + k;
        eptr->node1 = offset + j;
        eptr->node2 = offset + j + 1;
        eptr->length = xspace;
        eptr->tri1 = -1;
        eptr->tri2 = -1;
    }

    NumEdges = (nr - 1) * edges_per_row + nc - 1;

/*
    Fill in the triangle list.  Each cell has two triangles.
    In even numbered rows, the first uses the bottom, right
    and diagonal while the second uses the left, top and
    diagonal.  In odd numbered rows, the first triangle uses
    the left, bottom and diagonal while the second triangle
    uses the top, right and diagonal.
*/
    for (i=0; i<nr-1; i++) {
        tbase = i * triangles_per_row;
        ebase = i * edges_per_row;

        for (j=0; j<nc-1; j++) {
            kt = tbase + j * 2;
            ke = ebase + j * 3;

            tptr = TriangleList + kt;

            tptr->edge1 = ke + 1;
            eptr = EdgeList + ke + 1;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }

            tptr->edge2 = ke + 2;
            eptr = EdgeList + ke + 2;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }

            tptr->edge3 = ke + 3;
            eptr = EdgeList + ke + 3;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }

            tptr = TriangleList + kt + 1;

            tptr->edge1 = ke;
            eptr = EdgeList + ke;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt + 1;
            }
            else {
                eptr->tri2 = kt + 1;
            }

            tptr->edge2 = ke + 2;
            eptr = EdgeList + ke + 2;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt + 1;
            }
            else {
                eptr->tri2 = kt + 1;
            }

            tptr->edge3 = ke + edges_per_row + 1;
            if (i == nr-2) {
                tptr->edge3 = top_ebase + j;
            }
            if (i == nr-2) {
                eptr = EdgeList + top_ebase + j;
            }
            else {
                eptr = EdgeList + ke + edges_per_row + 1;
            }
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt + 1;
            }
            else {
                eptr->tri2 = kt + 1;
            }
        }
    }
    NumTriangles = (nr - 1) * triangles_per_row;


/*
    return the results and clean up any work space
*/
    *nodes_out = NodeList;
    *num_nodes_out = NumNodes;
    *edges_out = EdgeList;
    *num_edges_out = NumEdges;
    *triangles_out = TriangleList;
    *num_triangles_out = NumTriangles;

    ListNull ();
    FreeMem ();

    if (grid != gridin) csw_Free (grid);

    CheckNullPointers = 0;

    return 1;

}  /*  end of function grd_pointer_grid_to_trimesh  */



/*
 ************************************************************************

          g r d _ c a l c _ g r i d _ f r o m _ t r i m e s h

 ************************************************************************

  Calculate a rectangular grid from the specified trimesh.  Each triangle
  is linearly interpolated to assign z values to any rectangular grid nodes
  lying inside or on the edge of the triangle.  Any rectangular grid node
  that is not inside a triangle is set to the null value.  Also, if any
  corner of a triangle is the null value, all rectangular grid nodes inside
  that triangle are set to the null value.

*/

int CSWGrdTriangle::grd_calc_grid_from_trimesh (NOdeStruct *nodes, int numnodes,
                                EDgeStruct *edges, int numedges,
                                TRiangleStruct *triangles, int numtriangles,
                                double nullvalue,
                                CSW_F *grid, int ncol, int nrow,
                                double x1, double y1, double x2, double y2)

{
    double              xpts[4], ypts[4], zpts[3], coef[3], znull,
                        xt1, yt1, xt2, yt2, zt, xspace, yspace;
    int                 itri, i, j, k, i1, i2, j1, j2, offset, istat;
    int                 negative_null_flag;
    TRiangleStruct      *tptr;
    double              tiny, ztiny;
    double              ytmax, zmin, zmax;

    CSWPolyUtils        ply_utils_obj;

    if (ncol < 2  ||  nrow < 2) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    tiny = (x2 - x1 + y2 - y1) / 200.0;
    ply_utils_obj.ply_setgraze (tiny / 1000.0);

    if (nullvalue < 1.e10  &&  nullvalue > -1.e10) {
        nullvalue = 1.e30;
    }

    EdgeList = edges;
    NodeList = nodes;
    NumEdges = numedges;
    NumNodes = numnodes;

    negative_null_flag = 0;
    if (nullvalue < 0.0) {
        negative_null_flag = 1;
    }

    xspace = (x2 - x1) / (double)(ncol - 1);
    yspace = (y2 - y1) / (double)(nrow - 1);
    znull = nullvalue / 10.0;

/*
 * Find the z value limits and use 1 part in a million
 * of the limits as the ztiny value.
 */
    zmin = 1.e30;
    zmax = -1.e30;
    for (i=0; i<NumNodes; i++) {
        zt = NodeList[i].z;
        if (zt > 1.e20) {
            continue;
        }
        if (zt < zmin) zmin = zt;
        if (zt > zmax) zmax = zt;
    }

    if (zmax <= zmin) {
        ztiny = 1.e-20;
    }
    else {
        ztiny = (zmax - zmin) / 1000000.0;
    }

/*
    Set all grid nodes to the null value.
*/
    for (i=0; i<ncol*nrow; i++) {
        grid[i] = (CSW_F)nullvalue;
    }

    ytmax = -1.e30;
    for (i=0; i<numtriangles; i++) {
        TrianglePoints (triangles + i, xpts, ypts, zpts);
        for (j=0; j<3; j++) {
            if (ypts[j] > ytmax) {
                ytmax = ypts[j];
            }
        }
    }

/*
    Loop through the triangles.  For each triangle calculate
    its bounding box and scan the grid nodes intersecting
    the box.  Any grid node inside or on the edge of the
    triangle is set to the elevation of the plane defined
    by the triangle.
*/
    for (itri=0; itri<numtriangles; itri++) {

        tptr = triangles + itri;
        if (tptr->deleted) continue;

    /*
     * get the triangle points and skip the triangle
     * if any z values are null.
     */
        TrianglePoints (tptr, xpts, ypts, zpts);
        if (negative_null_flag) {
            if (zpts[0] <= znull  ||  zpts[1] <= znull  ||  zpts[2] <= znull) {
                continue;
            }
        }
        else {
            if (zpts[0] >= znull  ||  zpts[1] >= znull  ||  zpts[2] >= znull) {
                continue;
            }
        }

    /*
     * set up the bounding box
     */
        xt1 = xpts[0];
        yt1 = ypts[0];
        xt2 = xpts[0];
        yt2 = ypts[0];
        for (i=1; i<3; i++) {
            if (xpts[i] < xt1) xt1 = xpts[i];
            if (ypts[i] < yt1) yt1 = ypts[i];
            if (xpts[i] > xt2) xt2 = xpts[i];
            if (ypts[i] > yt2) yt2 = ypts[i];
        }

    /*
     * skip if the triangle is completely outside the grid.
     */
        if (yt2 < y1  ||  yt1 > y2  ||
            xt2 < x1  ||  xt1 > x2) {
            continue;
        }
        i1 = (int)((yt1 - y1) / yspace);
        i1--;
        i2 = (int)((yt2 - y1) / yspace);
        i2++;
        j1 = (int)((xt1 - x1) / xspace);
        j1--;
        j2 = (int)((xt2 - x1) / xspace);
        j2++;
        if (i1 < 0) i1 = 0;
        if (i2 > nrow-1) i2 = nrow - 1;
        if (j1 < 0) j1 = 0;
        if (j2 > ncol-1) j2 = ncol - 1;

    /*
     * Calculate the coefficients of the plane defined by the triangle.
     * If there is an error in this, the triangle has zero area and
     * should therefore not be filled.
     */
        istat = grd_utils_ptr->grd_calc_double_plane (xpts, ypts, zpts, 3, coef);
        if (istat == -1) {
            continue;
        }

        xpts[3] = xpts[0];
        ypts[3] = ypts[0];

    /*
     * For each grid node in the bounding box, check to see
     * if it is inside or on the edge of the triangle.  If
     * it is, use the plane coefs to assign a z value.
     */
        for (i=i1; i<=i2; i++) {
            yt1 = y1 + i * yspace;
            offset = i * ncol;
            for (j=j1; j<=j2; j++) {
                xt1 = x1 + j * xspace;
                istat = ply_utils_obj.ply_point (xpts, ypts, 4, xt1, yt1);
                if (istat >= 0) {
                    k = offset + j;
                    zt = coef[0] + coef[1] * xt1 + coef[2] * yt1;
                    if (zt >= -ztiny  &&  zt <= ztiny) {
                        zt = 0.0;
                    }
                    if (grid[k] >= nullvalue) {
                        grid[k] = (CSW_F)zt;
                    }
                }
            }
        }

    }  /* end of loop through each triangle */

    if (DontFree == 0) {
        ListNull ();
        FreeMem ();
    }

    return 1;

}  /* end of function grd_calc_grid_from_trimesh */




/*
 **************************************************************************

     g r d _ c a l c _ t r i m e s h _ b o u n d i n g _ b o x

 **************************************************************************

*/

int CSWGrdTriangle::grd_calc_trimesh_bounding_box (NOdeStruct *nodes, int numnodes,
                                   EDgeStruct *edges, int numedges,
                                   TRiangleStruct *triangles, int numtriangles,
                                   double *xmin, double *ymin,
                                   double *xmax, double *ymax,
                                   double *zmin, double *zmax)
{
    int                 i, j;
    TRiangleStruct      *tptr;
    double              xpts[3], ypts[3], zpts[3];
    double              x1, y1, x2, y2, z1, z2;

    EdgeList = edges;
    NumEdges = numedges;
    NodeList = nodes;
    NumNodes = numnodes;

    x1 = 1.e30;
    y1 = 1.e30;
    z1 = 1.e30;
    x2 = -1.e30;
    y2 = -1.e30;
    z2 = -1.e30;
    for (i=0; i<numtriangles; i++) {
        tptr = triangles + i;
        if (tptr->deleted) continue;

        TrianglePoints (tptr, xpts, ypts, zpts);

        for (j=0; j<3; j++) {
            if (xpts[j] < x1) x1 = xpts[j];
            if (ypts[j] < y1) y1 = ypts[j];
            if (xpts[j] > x2) x2 = xpts[j];
            if (ypts[j] > y2) y2 = ypts[j];
            if (zpts[j] > -1.e15  &&  zpts[j] < 1.e15) {
                if (zpts[j] < z1) z1 = zpts[j];
                if (zpts[j] > z2) z2 = zpts[j];
            }
        }

    }

    *xmin = x1;
    *ymin = y1;
    *zmin = z1;
    *xmax = x2;
    *ymax = y2;
    *zmax = z2;

    ListNull ();
    FreeMem ();

    return 1;

}  /* end of function grd_calc_trimesh_bounding_box */



/*
 **********************************************************************************

                         W e e d C l o s e P o i n t s

 **********************************************************************************

  Remove points in a line that are very close to each other.

*/

void CSWGrdTriangle::WeedClosePoints (double *x, double *y, void **tags, int npts, double dcrit,
                             double *xout, double *yout, void **tagsout, int *nout)
{
    double             dx, dy, dist, x1, y1, dc2;
    int                i, n;

    dc2 = dcrit * dcrit;

    n = 1;
    x1 = x[0];
    y1 = y[0];
    xout[0] = x1;
    yout[0] = y1;
    if (tags && tagsout) {
        tagsout[0] = tags[0];
    }

    for (i=1; i<npts; i++) {
        dx = x[i] - x1;
        dy = y[i] - y1;
        dist = dx * dx + dy * dy;
        if (dist > dc2) {
            xout[n] = x[i];
            yout[n] = y[i];
            if (tags && tagsout) {
                tagsout[n] = tags[i];
            }
            n++;
        }
        x1 = x[i];
        y1 = y[i];
    }

    *nout = n;

    return;

}  /* end of private WeedClosePoints function */




/*
 **********************************************************************************

                      F i x L i n e D e f e c t s

 **********************************************************************************

  Clean up various defects possibly found in the constraint lines when a grid is
  to be converted to a trimesh.  The cleaned up lines are put into the private
  arrays named Xline, Yline, Zline, with Iline having the number of points per
  cleaned up line and Nline having the number of cleaned up lines.

*/

int CSWGrdTriangle::FixLineDefects (double *x, double *y, double *z,
                           int *linepoints, int *lineflags, int nlines)
{
    int           istat, i, j, n, npmax, nsegmax,
                  nlwork, nout, nt, ncmax, np2, n2;
    double        dcrit, dclip, xt1, yt1, xt2, yt2, dx, dy, dskinny;
    double        dist;
    double        xbox[5], ybox[5], *dptr = NULL;
    double        *xw1 = NULL, *yw1 = NULL, *xw2 = NULL, *yw2 = NULL;
    double        *zwork = NULL, smult, tiny;
    void          **tag1 = NULL, **tag2 = NULL;
    int           *iw1 = NULL, *iw2 = NULL, *iw3 = NULL,
                  *iw4 = NULL, *nlcomp = NULL;
    char          *cenv = NULL;

    CSWPolyGraph  ply_graph_obj;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (xw1);
        csw_Free (iw1);
        csw_Free (tag1);
        if (bsuccess == false) {
            csw_Free (Xline);
            Xline = NULL;
            Yline = NULL;
            Zline = NULL;
            csw_Free (Iline);
            Iline = NULL;
            Iflag = NULL;
            Nline = 0;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    Xline = NULL;
    Yline = NULL;
    Zline = NULL;
    Iline = NULL;
    Iflag = NULL;
    Nline = 0;

    if (nlines < 1  ||  linepoints == NULL) {
        return 1;
    }

/*
    Allocate a bunch of work space for the various
    steps needed to fix possible defects.
*/
    npmax = 0;
    for (i=0; i<nlines; i++) {
        npmax += linepoints[i];
    }

    npmax *= 20;
    if (npmax < 1000) npmax = 1000;

    xw1 = (double *)csw_Malloc (5 * npmax * sizeof(double));
    if (xw1 == NULL) {
        return -1;
    }
    yw1 = xw1 + npmax;
    xw2 = yw1 + npmax;
    yw2 = xw2 + npmax;
    zwork = yw2 + npmax;

    n = nlines * 20;
    if (n < 100) n = 100;
    nsegmax = n;
    iw1 = (int *)csw_Malloc (n * 5 * sizeof(int));
    if (iw1 == NULL) {
        return -1;
    }
    iw2 = iw1 + n;
    iw3 = iw2 + n;
    iw4 = iw3 + n;
    nlcomp = iw4 + n;
    ncmax = n;

    tag1 = (void **)csw_Malloc (npmax * 2 * sizeof(void*));
    if (tag1 == NULL) {
        return -1;
    }
    tag2 = tag1 + npmax;

/*
    Allocate the output arrays.
*/
    Xline = (double *)csw_Malloc (npmax * 3 * sizeof(double));
    if (Xline == NULL) {
        return -1;
    }
    Yline = Xline + npmax;
    Zline = Yline + npmax;

    Iline = (int *)csw_Malloc (2 * ncmax * sizeof(int));
    if (Iline == NULL) {
        return -1;
    }
    Iflag = Iline + ncmax;

/*
    If any of the constraint lines are self intersecting,
    do not use them in the calculations.
*/
    tiny = GrazeDistance;
    n = 0;
    nout = 0;
    nlwork = 0;
    for (i=0; i<nlines; i++) {
        if (linepoints[i] < 1) continue;
        dx = x[n] - x[n+linepoints[i]-1];
        dy = y[n] - y[n+linepoints[i]-1];
        dist = dx * dx + dy * dy;
        dist = sqrt(dist);
        if (dist <= tiny * 10.0) {
            x[n] = x[n+linepoints[i]-1];
            y[n] = y[n+linepoints[i]-1];
            istat = 0;
        }
        else {
            istat = gpf_check_for_self_intersection
                      (x+n, y+n, linepoints[i], iw2, &nt, tiny, nsegmax);
            if (istat == -1) {
                return -1;
            }
        }
        if (istat == 0  ||  PolygonalizeConstraintFlag == 0) {
            memcpy (xw1+nout, x+n, linepoints[i]*sizeof(double));
            memcpy (yw1+nout, y+n, linepoints[i]*sizeof(double));
            if (z != NULL) {
                memcpy (zwork+nout, z+n, linepoints[i]*sizeof(double));
            }
            else {
                null_array (zwork+nout, linepoints[i]);
            }
            nout += linepoints[i];
            iw1[nlwork] = linepoints[i];
            if (lineflags) {
                iw4[nlwork] = lineflags[i];
            }
            nlwork++;
        }
        n += linepoints[i];
    }

    if (nlwork < 1) {
        return 1;
    }

/*
    The z values are associated with the polygons in the various weeding and
    clipping functions via an array of pointers that tags along.  This tag
    array is built here.  It represents pointers to the z values that
    belong to lines that survived the self intersection test.
*/
    for (i=0; i<nout; i++) {
        tag1[i] = (void *)(zwork + i);
    }

/*
    Find the x,y corners of the line points
*/
    for (i=0; i<nout; i++) {
        if (x[i] < XlineMin) XlineMin = x[i];
        if (y[i] < YlineMin) YlineMin = y[i];
        if (x[i] > XlineMax) XlineMax = x[i];
        if (y[i] > YlineMax) YlineMax = y[i];
    }

/*
    Remove points in individual lines that are too close to each other.
*/
    dcrit = GrazeDistance * 100.0;
    n = 0;
    nout = 0;
    for (i=0; i<nlwork; i++) {
        WeedClosePoints (xw1+n, yw1+n, tag1+n, iw1[i], dcrit,
                         xw2+nout, yw2+nout, tag2+nout, &nt);
        iw2[i] = nt;
        nout += nt;
        n += iw1[i];
    }

/*
    If a line is not closed (first and last points essentially
    in the same location) then assume it is a vertical fault
    which needs to be converted into a very skinny non vertical
    fault polygon.
*/
    cenv = getenv ("GRD_SKINNY_MULT");
    if (cenv) {
        sscanf (cenv, "%lf", &smult);
    }
    else {
        smult = 1.0;
    }
    dskinny = dcrit * smult;
    dskinny *= 2.0;
    dcrit /= 10.0;
    n = 0;
    nout = 0;

    if (PolygonalizeConstraintFlag == 1) {
        for (i=0; i<nlwork; i++) {
            xt1 = xw2[n];
            yt1 = yw2[n];
            nt = iw2[i] - 1;
            xt2 = xw2[n+nt];
            yt2 = yw2[n+nt];
            dx = xt2 - xt1;
            dy = yt2 - yt1;
            if (dx < 0.0) dx = -dx;
            if (dy < 0.0) dy = -dy;
            if (dx <= dcrit  &&  dy <= dcrit) {
                xw2[n+nt] = xw2[n];
                yw2[n+nt] = yw2[n];
                memcpy (xw1+nout, xw2+n, iw2[i]*sizeof(double));
                memcpy (yw1+nout, yw2+n, iw2[i]*sizeof(double));
                memcpy (tag1+nout, tag2+n, iw2[i]*sizeof(void*));
                nout += iw2[i];
                n += iw2[i];
                iw1[i] = iw2[i];
                continue;
            }
            istat = gpf_polygonalize_line (xw2+n, yw2+n, tag2+n, iw2[i], dskinny,
                                           xw1+nout, yw1+nout, tag1+nout, &nt,
                                           npmax - nout);
            if (istat == -1) {
                return -1;
            }
            iw1[i] = nt;
            nout += nt;
            n += iw2[i];
        }
    }
    else {
        for (i=0; i<nlwork; i++) {
            memcpy (xw1+nout, xw2+n, iw2[i]*sizeof(double));
            memcpy (yw1+nout, yw2+n, iw2[i]*sizeof(double));
            memcpy (tag1+nout, tag2+n, iw2[i]*sizeof(void*));
            nout += iw2[i];
            n += iw2[i];
            iw1[i] = iw2[i];
        }
    }

    if (ClipToGridFlag == 1  &&  PolygonalizeConstraintFlag == 1) {

    /*
        Some of the polygons may intersect each other.  Clean this up
        by unioning all of the polygons.
    */
        for (i=0; i<nlwork; i++) {
            nlcomp[i] = 1;
        }
        istat = ply_graph_obj.ply_union_components (xw1, yw1, tag1, nlwork, nlcomp, iw1,
                                      xw2, yw2, tag2, &np2, iw3, iw2,
                                      npmax, ncmax);
        if (Nbugs == 0) {
            ply_graph_obj.ply_get_bug_locations (BugX, BugY, &Nbugs, 10);
            for (i=0; i<Nbugs; i++) {
                BugX[i] += Xshift;
                BugY[i] += Yshift;
            }
        }
        if (istat == -1) {
            return -1;
        }

    /*
        Clip the polygons to the rectangular
        extents of the grid.
    */
        dclip = 0.0;
        xbox[0] = GridX1 + dclip;
        ybox[0] = GridY1 + dclip;
        xbox[1] = GridX2 - dclip;
        ybox[1] = GridY1 + dclip;
        xbox[2] = GridX2 - dclip;
        ybox[2] = GridY2 - dclip;
        xbox[3] = GridX1 + dclip;
        ybox[3] = GridY2 - dclip;
        xbox[4] = GridX1 + dclip;
        ybox[4] = GridY1 + dclip;

        n = 1;
        n2 = 5;
        istat = ply_graph_obj.ply_boolean (xw2, yw2, tag2, np2, iw3, iw2,
                             xbox, ybox, NULL, 1, &n, &n2,
                             PLY_INTERSECT,
                             xw1, yw1, tag1, &np2, iw3, iw1,
                             npmax, ncmax);
        if (Nbugs == 0) {
            ply_graph_obj.ply_get_bug_locations (BugX, BugY, &Nbugs, 10);
            for (i=0; i<Nbugs; i++) {
                BugX[i] += Xshift;
                BugY[i] += Yshift;
            }
        }
        if (istat == -1) {
            return -1;
        }

        n = 0;
        for (i=0; i<np2; i++) {
            n += iw3[i];
        }
        nlwork = n;

    }

/*
    No clipping was specified
*/
    else {
        np2 = 1;
        iw3[0] = nlwork;
    }

/*
    put the results into the Iline, Xline, Yline and Zline arrays
*/
    n2 = 0;
    n = 0;
    for (i=0; i<np2; i++) {
        for (j=0; j<iw3[i]; j++) {
            Iline[n] = iw1[n];
            if (lineflags != NULL) {
                Iflag[n] = iw4[n];
            }
            else {
                Iflag[n] = 0;
            }
            n2 += iw1[n];
            n++;
        }
    }
    Nline = n;
    memcpy (Xline, xw1, n2 * sizeof(double));
    memcpy (Yline, yw1, n2 * sizeof(double));
    for (i=0; i<n2; i++) {
        dptr = (double *)tag1[i];
        if (dptr == NULL) {
            Zline[i] = 1.e30;
        }
        else {
            Zline[i] = *dptr;
        }
    }

/*
    Clean up the workspace and return.
*/
    bsuccess = true;

    return 1;

}  /* end of private FixLineDefects function */




/*
 **********************************************************************************

                         S n a p T o R a w P o i n t

 **********************************************************************************

  If the specified position is already occupied, return the point number in
  the position.  Return -1 if not occupied.

*/

int CSWGrdTriangle::SnapToRawPoint (int start, int end, double xin, double yin)
{
    int                  i;
    double               xt, yt, dx, dy, dist, tiny;
    RAwPointStruct       *rptr;

    tiny = GrazeDistance * 10.0;
    if (GridNcol > 0  &&  GridNrow > 0) {
        tiny = (GridXspace + GridYspace) / 200.0;
    }

    for (i=start; i<end; i++) {
        rptr = RawPoints + i;
        xt = rptr->x;
        yt = rptr->y;
        dx = xt - xin;
        dy = yt - yin;
        dist = dx * dx + dy * dy;
        dist = sqrt(dist);
        if (dist <= tiny) {
            return i;
        }
    }

    return -1;

}  /* end of private SnapToRawPoint function */





/*
 ***************************************************************************

        g r d _ i n t e r p o l a t e _ t r i m e s h _ v a l u e s

 ***************************************************************************

  Interpolate the soft nulls in the trimesh by either using the faulted
  back interpolation (for points not on faults) or by fitting a plane
  to the nearest valid triangle.

*/

int CSWGrdTriangle::grd_interpolate_trimesh_values (CSW_F *grid, int ncol, int nrow,
                                    double x1, double y1, double x2, double y2,
                                    NOdeStruct *nodes, int numnodes,
                                    EDgeStruct *edges, int numedges,
                                    TRiangleStruct *triangles, int numtriangles)
{
    int                i, j, n, istat, t1, ndone, n2;
    int                *itmp = NULL, **lists = NULL,
                       *nlist = NULL, *maxlist = NULL;
    CSW_F              *fx = NULL, *fy = NULL, *fz = NULL,
                       fxt, fyt, fzt;
    double             xt, yt, dx, dy, dist, pct;
    NOdeStruct         *np1 = NULL, *np2 = NULL;
    EDgeStruct         *eptr = NULL;
    double             xp[3],
                       yp[3],
                       zp[3],
                       coef[3];
    double             softchk, hardchk;


    auto fscope = [&]()
    {
        if (lists != NULL) {
            for (i=0; i<numnodes; i++) {
                csw_Free (lists[i]);
            }
        }
        ListNull ();
        FreeMem ();
        csw_Free (fx);
        csw_Free (itmp);
        csw_Free (lists);
        csw_Free (nlist);
        csw_Free (maxlist);
    };
    CSWScopeGuard func_scope_guard (fscope);


    NodeList = nodes;
    EdgeList = edges;
    TriangleList = triangles;
    NumNodes = numnodes;
    NumEdges = numedges;
    NumTriangles = numtriangles;

    softchk = GRD_SOFT_NULL_VALUE / 100.0;
    hardchk = GRD_HARD_NULL_VALUE / 100.0;

/*
    Set any trimesh nodes that are grazing the faults to
    the soft null value.
*/
    grd_fault_ptr->grd_set_grazing_grid_nodes ();
    grd_fault_ptr->grd_set_grazing_trimesh_nodes (nodes, numnodes);

/*
    The nodes in the trimesh that are shifted away from the
    original grid nodes need to have z values assigned via the
    faulted back interpolation function.
*/
    n = 0;
    for (i=0; i<numnodes; i++) {
        if (nodes[i].shifted) n++;
    }

    fx = (CSW_F *)csw_Malloc (n * 3 * sizeof(CSW_F));
    if (fx == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    fy = fx + n;
    fz = fy + n;

    itmp = (int *)csw_Malloc (n * sizeof(int));
    if (itmp == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    n = 0;
    for (i=0; i<numnodes; i++) {
        if (nodes[i].shifted) {
            itmp[n] = i;
            fx[n] = (CSW_F)nodes[i].x;
            fy[n] = (CSW_F)nodes[i].y;
            n++;
        }
    }

    if (n > 0) {
        istat = grd_fault_ptr->grd_back_interpolate_faulted (grid, ncol, nrow,
                                              (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
                                              fx, fy, fz, n, GRD_BICUBIC);
        if (istat == -1) {
            return -1;
        }
    }

    for (i=0; i<n; i++) {
        j = itmp[i];
        nodes[j].z = (double)fz[i];
        nodes[j].shifted = 0;
    }
    csw_Free (fx);
    fx = NULL;
    csw_Free (itmp);
    itmp = NULL;

/*
    Allocate space for and build lists of edges at each soft null node.
*/
    lists = (int **)csw_Calloc (numnodes * sizeof(int *));
    nlist = (int *)csw_Calloc (numnodes * sizeof(int));
    maxlist = (int *)csw_Calloc (numnodes * sizeof(int));
    if (lists == NULL  ||  nlist == NULL  ||  maxlist == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    istat = BuildTempNodeEdgeLists (edges, numedges, nodes, softchk,
                                    lists, nlist, maxlist);
    csw_Free (maxlist);
    maxlist = NULL;
    if (istat == -1) {
        return -1;
    }

/*
    First, try to interpolate soft nulls by nudging them slightly
    along a non constraint connecting edge and then back interpolating
    from the faulted grid.
*/
    for (i=0; i<numnodes; i++) {
        np1 = nodes + i;
        if (np1->z > softchk) {
            continue;
        }
        if (lists[i] == NULL  ||  nlist[i] < 1) {
            continue;
        }
        grd_break_for_artifact (np1->x, np1->y);
        for (j=0; j<nlist[i]; j++) {
            eptr = edges + lists[i][j];
            if (eptr->deleted  ||  eptr->flag != 0) {
                continue;
            }
            n2 = eptr->node1;
            if (n2 == i) n2 = eptr->node2;
            dx = nodes[i].x - nodes[n2].x;
            dy = nodes[i].y - nodes[n2].y;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist < FaultAdjustDistance) {
                nodes[i].z = nodes[n2].z;
                continue;
            }
            pct = FaultAdjustDistance / dist;
            dx *= pct;
            dy *= pct;
            xt = nodes[i].x - dx;
            yt = nodes[i].y - dy;
            fxt = (CSW_F)xt;
            fyt = (CSW_F)yt;
            istat = grd_fault_ptr->grd_back_interpolate_faulted (grid, ncol, nrow,
                                                  (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
                                                  &fxt, &fyt, &fzt, 1, GRD_BICUBIC);
            if (istat == -1  ||  fzt > hardchk) {
                nodes[i].z = nodes[n2].z;
                continue;
            }
            nodes[i].z = (double)fzt;
            break;
        }
    }

/*
    Any remaining soft null values should be on spots where
    the fault lines are very crowded together.
    Try to interpolate these by fitting planes to triangles
    close by.   This is repeated until no values are done
    in an iteration.
*/
    for (;;) {
        ndone = 0;
        for (i=0; i<numedges; i++) {
            eptr = edges + i;
            if (eptr->deleted) continue;
            np1 = nodes + eptr->node1;
            np2 = nodes + eptr->node2;
            if (np1->z > softchk  &&
                np2->z > softchk) {
                continue;
            }
            if (np1->z < softchk) {
                t1 = FindInterpolationTriangle (np1 - nodes,
                                                lists[eptr->node1], nlist[eptr->node1],
                                                nodes, edges, triangles,
                                                softchk, hardchk);
                if (t1 >= 0) {
                    LocalTrianglePoints (triangles+t1,
                                         nodes, edges,
                                         xp, yp, zp);
                    grd_utils_ptr->grd_calc_double_plane (xp, yp, zp, 3, coef);
                    np1->z = coef[0] + coef[1] * np1->x + coef[2] * np1->y;
                    np1->shifted = 99;
                    ndone++;
                }
            }

            if (np2->z < softchk) {
                t1 = FindInterpolationTriangle (np2 - nodes,
                                                lists[eptr->node2], nlist[eptr->node2],
                                                nodes, edges, triangles,
                                                softchk, hardchk);
                if (t1 >= 0) {
                    LocalTrianglePoints (triangles+t1,
                                         nodes, edges,
                                         xp, yp, zp);
                    grd_utils_ptr->grd_calc_double_plane (xp, yp, zp, 3, coef);
                    np2->z = coef[0] + coef[1] * np1->x + coef[2] * np1->y;
                    np2->shifted = 99;
                    ndone++;
                }
            }
        }

        for (i=0; i<numnodes; i++) {
            if (nodes[i].shifted == 99) {
                nodes[i].shifted = 0;
            }
        }

        if (ndone == 0) break;

    }

/*
    Reset any remaining soft nulls to hard nulls.
*/
    for (i=0; i<numnodes; i++) {
        if (nodes[i].z < softchk) {
            nodes[i].z = GRD_HARD_NULL_VALUE;
        }
    }

    return 1;

}  /* end of grd_interpolate_trimesh_values function */




/*
 *********************************************************************************

                  F i n d I n t e r p o l a t i o n T r i a n g l e

 *********************************************************************************

  Find a triangle which shares the opposite edge to the specified node on the specified
  edge.  The triangle must also have 3 valid z values at its nodes.

*/

int CSWGrdTriangle::FindInterpolationTriangle (int nodenum,
                                      int *list, int nlist,
                                      NOdeStruct *nodes,
                                      EDgeStruct *edges,
                                      TRiangleStruct *triangles,
                                      double softchk, double hardchk)
{
    int                 istat, i, t1, t2, e1, tnum;
    int                 n1, n2, n3, tmax;
    double              eqmax, eqchk;
    TRiangleStruct      *tptr;
    EDgeStruct          *eptr;

    tmax = -1;
    eqmax = 0.001;

    for (i=0; i<nlist; i++) {
        eptr = edges + list[i];
        if (eptr->flag != 0) continue;
        t1 = eptr->tri1;
        t2 = eptr->tri2;
        tptr = triangles + t1;
        e1 = LocalOppositeEdge (tptr, nodenum, edges);
        if (e1 >= 0) {
            eptr = edges + e1;
            tnum = eptr->tri1;
            if (tnum == t1) {
                tnum = eptr->tri2;
            }
            istat = CheckNullCorners (tnum, softchk, hardchk,
                                      nodes, edges, triangles);
            if (istat == 0) {
                n1 = eptr->node1;
                n2 = eptr->node2;
                n3 = LocalOppositeNode (tnum, e1, edges, triangles);
                eqchk = CalcEquilateralness (n1, n2, n3);
                if (eqchk > eqmax) {
                    tmax = tnum;
                    eqmax = eqchk;
                }
            }
        }
        tptr = triangles + t2;
        e1 = LocalOppositeEdge (tptr, nodenum, edges);
        if (e1 >= 0) {
            eptr = edges + e1;
            tnum = eptr->tri1;
            if (tnum == t2) {
                tnum = eptr->tri2;
            }
            istat = CheckNullCorners (tnum, softchk, hardchk,
                                      nodes, edges, triangles);
            if (istat == 0) {
                n1 = eptr->node1;
                n2 = eptr->node2;
                n3 = LocalOppositeNode (tnum, e1, edges, triangles);
                eqchk = CalcEquilateralness (n1, n2, n3);
                if (eqchk > eqmax) {
                    tmax = tnum;
                    eqmax = eqchk;
                }
            }
        }
    }

    return tmax;

}  /* end of private FindInterpolationTriangle function */




/*
 ************************************************************************************

                   B u i l d T e m p N o d e E d g e L i s t s

 ************************************************************************************

  Build lists of all edges attached to soft null nodes.

*/

int CSWGrdTriangle::BuildTempNodeEdgeLists (EDgeStruct *edges, int nedges,
                                   NOdeStruct *nodes, double softchk,
                                   int **lists, int *nlists, int *maxlists)
{
    int               i, n1;
    EDgeStruct        *eptr;
    NOdeStruct        *np1;

    for (i=0; i<nedges; i++) {
        eptr = edges + i;
        if (eptr->deleted) continue;
        n1 = eptr->node1;
        np1 = nodes + n1;
        if (np1->z < softchk) {
            if (nlists[n1] >= maxlists[n1]) {
                maxlists[n1] += 5;
                lists[n1] = (int *)csw_Realloc (lists[n1], maxlists[n1] * sizeof(int));
                if (lists[n1] == NULL) {
                    return -1;
                }
            }
            lists[n1][nlists[n1]] = i;
            nlists[n1]++;
        }
        n1 = eptr->node2;
        np1 = nodes + n1;
        if (np1->z < softchk) {
            if (nlists[n1] >= maxlists[n1]) {
                maxlists[n1] += 5;
                lists[n1] = (int *)csw_Realloc (lists[n1], maxlists[n1] * sizeof(int));
                if (lists[n1] == NULL) {
                    return -1;
                }
            }
            lists[n1][nlists[n1]] = i;
            nlists[n1]++;
        }
    }

    return 1;

}  /* end of private BuildTempNodeEdgeLists function */



/*
 **************************************************************************************

                        C h e c k N u l l C o r n e r s

 **************************************************************************************

  Return 1 if at least one triangle corner is a hard or soft null.  Return zero if
  no null corners are found.

*/

int CSWGrdTriangle::CheckNullCorners (int tnum, double soft, double hard,
                             NOdeStruct *nodes,
                             EDgeStruct *edges,
                             TRiangleStruct *triangles)
{
    double             xp[3], yp[3], zp[3];
    int                i;

    if (tnum < 0) return 1;

    LocalTrianglePoints (triangles + tnum,
                         nodes, edges,
                         xp, yp, zp);

    for (i=0; i<3; i++) {
        if (zp[i] < soft  ||  zp[i] > hard) {
            return 1;
        }
    }

    return 0;

}  /* end of private CheckNullCorners function */



/*
 *****************************************************************************

                  L o c a l T r i a n g l e P o i n t s

 *****************************************************************************

  Return the x, y, z points for the specified triangle.

*/

void CSWGrdTriangle::LocalTrianglePoints (TRiangleStruct *tptr,
                                 NOdeStruct *nodes,
                                 EDgeStruct *edges,
                                 double *x,
                                 double *y,
                                 double *z)
{
    EDgeStruct         *eptr;
    NOdeStruct         *nptr;
    int                n1, n2, n3;

    eptr = edges + tptr->edge1;
    n1 = eptr->node1;
    nptr = nodes + n1;
    x[0] = nptr->x;
    y[0] = nptr->y;
    z[0] = nptr->z;
    if (nptr->shifted == 99) {
        z[0] = GRD_SOFT_NULL_VALUE;
    }
    n2 = eptr->node2;
    nptr = nodes + n2;
    x[1] = nptr->x;
    y[1] = nptr->y;
    z[1] = nptr->z;
    if (nptr->shifted == 99) {
        z[1] = GRD_SOFT_NULL_VALUE;
    }

    eptr = edges + tptr->edge2;
    if (eptr->node1 == n1  ||  eptr->node1 == n2) {
        n3 = eptr->node2;
    }
    else {
        n3 = eptr->node1;
    }

    nptr = nodes + n3;
    x[2] = nptr->x;
    y[2] = nptr->y;
    z[2] = nptr->z;
    if (nptr->shifted == 99) {
        z[2] = GRD_SOFT_NULL_VALUE;
    }

    return;

}  /* end of private LocalTrianglePoints function */





/*
  ****************************************************************************

                    L o c a l O p p o s i t e E d g e

  ****************************************************************************

    Given a triangle and a node number, return the edge number that is
    opposite from the node.

*/

int CSWGrdTriangle::LocalOppositeEdge (TRiangleStruct *tptr, int nodenum, EDgeStruct *edges)
{
    EDgeStruct          *eptr;

    eptr = edges + tptr->edge1;
    if (!(eptr->node1 == nodenum  ||  eptr->node2 == nodenum)) {
        return tptr->edge1;
    }
    eptr = edges + tptr->edge2;
    if (!(eptr->node1 == nodenum  ||  eptr->node2 == nodenum)) {
        return tptr->edge2;
    }
    eptr = edges + tptr->edge3;
    if (!(eptr->node1 == nodenum  ||  eptr->node2 == nodenum)) {
        return tptr->edge3;
    }

    return -1;

}  /*  end of private LocalOppositeEdge function  */





/*
  ****************************************************************************

                   L o c a l O p p o s i t e N o d e

  ****************************************************************************

    Return the number of the node that is opposite of the specified edge
    on the specified triangle.

*/

int CSWGrdTriangle::LocalOppositeNode (int trinum, int edgenum,
                              EDgeStruct *edges,
                              TRiangleStruct *triangles)
{
    int             istat, e1, e2, e3;
    TRiangleStruct  *tptr;

    tptr = triangles + trinum;
    e1 = tptr->edge1;
    e2 = tptr->edge2;
    e3 = tptr->edge3;

    if (e1 == edgenum) {
        istat = LocalCommonNode (e2, e3, edges);
    }
    else if (e2 == edgenum) {
        istat = LocalCommonNode (e1, e3, edges);
    }
    else if (e3 == edgenum) {
        istat = LocalCommonNode (e1, e2, edges);
    }
    else {
        istat = -1;
    }

    return istat;

}  /*  end of private LocalOppositeNode function  */





/*
  ****************************************************************************

                       L o c a l C o m m o n N o d e

  ****************************************************************************

    Return the node number that is common to both specified edge numbers.
    If no node is common, return -1.

*/

int CSWGrdTriangle::LocalCommonNode (int e1, int e2, EDgeStruct *edges)
{
    EDgeStruct         *ep1, *ep2;
    int                result;

    result = -1;
    ep1 = edges + e1;
    ep2 = edges + e2;
    if (ep1->node1 == ep2->node1  ||
        ep1->node1 == ep2->node2) {
        result = ep1->node1;
    }
    else if (ep1->node2 == ep2->node1  ||
             ep1->node2 == ep2->node2) {
        result = ep1->node2;
    }
    return result;

}  /*  end of private LocalCommonNode function  */



/*
 ********************************************************************************

               g r d _ b r e a k _ f o r _ a r t i f a c t

 ********************************************************************************

*/

int CSWGrdTriangle::grd_break_for_artifact (double x, double y)

{
    static const double      xt = 0.0, yt = 0.0, dt = 0.0;
    double      dx, dy;

    if (artifact_flag == 0) {
        return 1;
    }

    x += Xshift;
    y += Yshift;

    dx = x - xt;
    if (dx < 0.0) dx = -dx;
    dy = y - yt;
    if (dy < 0.0) dy = -dy;

    if (dx <= dt  &&  dy <= dt) {
        artifact_flag = 1;
    }

    return 1;

}



int CSWGrdTriangle::grd_set_shifts_for_debug (double x, double y) {
    Xshift = x;
    Yshift = y;
    return 1;
}





/*
 **************************************************************************************

  This file section has functions for calculating drainage areas from a grid.  The grid
  is converted to a trimesh and the trimesh edges are used to define the polygons.
  This is only used for unfaulted grids.

 **************************************************************************************

*/

/*
 **************************************************************************************

            g r d _ c a l c u l a t e _ d r a i n a g e _ p o l y g o n s

 **************************************************************************************

*/

int CSWGrdTriangle::grd_calculate_drainage_polygons (
            CSW_F *grid, int ncol, int nrow,
            double x1, double y1, double x2, double y2,
            int future_flag,
            SPillpointStruct *spill_filter_list,
            int num_spill_filters,
            DRainagesStruct **drainages_out)
{
    NOdeStruct     *nodes = NULL;
    int            numnodes;
    EDgeStruct     *edges = NULL;
    int            numedges;
    TRiangleStruct *triangles = NULL;
    int            numtriangles;
    POlygonStruct  *polygons = NULL;
    int            npolygons;
    SPillpointStruct *spillpoints = NULL;
    int            nspillpoints;
    DRainagesStruct *drainages = NULL;
    RIdgeLineStruct *rptr = NULL;

    int            istat, i, j, ndo;


    auto fscope = [&]()
    {
        DrainageFlag = 0;
        FreeMem ();
        if (NodeEdgeList) {
            ndo = NumNodeEdgeList;
            for (i=0; i<ndo; i++) {
                csw_Free (NodeEdgeList[i].list);
            }
            csw_Free (NodeEdgeList);
            NodeEdgeList = NULL;
        }
        NumNodeEdgeList = 0;

        OrigNumNodes = -1;
        csw_Free (PolySegs);
        PolySegs = NULL;
        PolygonList = NULL;
        SpillpointList = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);


    *drainages_out = NULL;

    future_flag = future_flag;

    FreeMem ();

    NumRidgeLines = 0;
    MaxRidgeLines = 0;
    NumPolygons = 0;
    MaxPolygons = 0;
    OrigNumNodes = -1;
    DrainageFlag = 1;

    SpillFilterList = spill_filter_list;
    NumSpillFilters = num_spill_filters;
    if (SpillFilterList == NULL) {
        NumSpillFilters = 0;
    }

    SpillFilterDistance = 0.0;
    if (NumSpillFilters > 0) {
        SpillFilterDistance = (x2 - x1) / (double)(ncol - 1) +
                              (y2 - y1) / (double)(nrow - 1);
    }

    istat = grd_grid_to_trimesh (grid, ncol, nrow,
                                 x1, y1, x2, y2,
                                 NULL, NULL, NULL, NULL, NULL, 0,
                                 GRD_CELL_DIAGONALS,
                                 &nodes, &edges, &triangles,
                                 &numnodes, &numedges, &numtriangles);
    if (istat == -1) {
        return -1;
    }

    NcolSaddle = ncol;
    NrowSaddle = nrow;
    GridSaddle = grid;

    X1Saddle = x1;
    Y1Saddle = y1;
    XspaceSaddle = (x2 - x1) / (double)(ncol - 1);
    YspaceSaddle = (y2 - y1) / (double)(nrow - 1);
    X1Saddle -= XspaceSaddle / 100.0;
    Y1Saddle -= YspaceSaddle / 100.0;

    AreaSize = x2 - x1 + y2 - y1;
    GrazeDistance = AreaSize / 20000.0;

    NodeList = nodes;
    NumNodes = numnodes;
    MaxNodes = numnodes;
    EdgeList = edges;
    NumEdges = numedges;
    MaxEdges = numedges;
    TriangleList = triangles;
    NumTriangles = numtriangles;
    MaxTriangles = numtriangles;

    for (i=0; i<NumEdges; i++) {
        EdgeList[i].number = -1;
    }

    BuildInitialDrainageEdgeList ();

    istat = SetPossibleRidgeEdges ();
    if (istat == -1) {
        return -1;
    }

    CleanInitialDrainageEdgeList ();

    i = numedges;
    if (i < 10) i = 10;
    PolySegs = (int *)csw_Malloc (i * sizeof(int));
    if (PolySegs == NULL) {
        return -1;
    }

    istat = BuildPolygonsFromRidgeLines ();
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<NumSpillpoints; i++) {
        if (SpillpointList[i].num_polygons_using < 1) {
            printf ("Spillpoint number %d not used by any polygon.\n", i);
        }
    }

    polygons = PolygonList;
    npolygons = NumPolygons;
    spillpoints = SpillpointList;
    nspillpoints = NumSpillpoints;

    drainages = (DRainagesStruct *)csw_Calloc (sizeof(DRainagesStruct));
    if (drainages == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    drainages->polygon_list = polygons;
    drainages->total_polygons = npolygons;
    drainages->spillpoint_list = spillpoints;
    drainages->total_spillpoints = nspillpoints;
    drainages->nodes = (NOdeStruct *)csw_Malloc (NumNodes * sizeof(NOdeStruct));
    if (drainages->nodes) {
        memcpy (drainages->nodes, NodeList,
                NumNodes * sizeof(NOdeStruct));
    }
    drainages->numnodes = NumNodes;
    drainages->edges = (EDgeStruct *)csw_Malloc (NumEdges * sizeof(EDgeStruct));
    if (drainages->edges) {
        memcpy (drainages->edges, EdgeList,
                NumEdges * sizeof(EDgeStruct));
    }
    drainages->numedges = NumEdges;
    drainages->triangles = (TRiangleStruct *)csw_Malloc (NumTriangles * sizeof(TRiangleStruct));
    if (drainages->triangles) {
        memcpy (drainages->triangles, TriangleList,
                NumTriangles * sizeof(TRiangleStruct));
    }
    drainages->numtriangles = NumTriangles;
    drainages->ridgelist = (POlygonStruct *)csw_Malloc
         (NumRidgeLines * sizeof(POlygonStruct));
    if (drainages->ridgelist) {
        for (i=0; i<NumRidgeLines; i++) {
            rptr = RidgeLineList + i;
            drainages->ridgelist[i].x = (double *)csw_Malloc (2 * rptr->nlist * sizeof(double));
            if (drainages->ridgelist[i].x == NULL) {
                drainages->ridgelist[i].y = NULL;
                drainages->ridgelist[i].z = NULL;
                drainages->ridgelist[i].npts = 0;
                continue;
            }
            drainages->ridgelist[i].y = drainages->ridgelist[i].x + rptr->nlist;
            drainages->ridgelist[i].z = NULL;
            drainages->ridgelist[i].npts = rptr->nlist;
            drainages->ridgelist[i].number = i;
            for (j=0; j<rptr->nlist; j++) {
                drainages->ridgelist[i].x[j] = NodeList[rptr->nodes[j]].x;
                drainages->ridgelist[i].y[j] = NodeList[rptr->nodes[j]].y;
            }
        }
    }
    drainages->numridges = NumRidgeLines;

    *drainages_out = drainages;

    return 1;

}  /* end of function grd_calculate_drainage_polygons */




/*
 *********************************************************************************************

                    S e t P o s s i b l e R i d g e E d g e s

 *********************************************************************************************

  Test each edge in the private EdgeList array to see if it is on a local maximum.  If the
  slope from the edge to the opposite point of each triangle sharing the edge is flat or
  downward, then the edge is a possible ridge edge.

*/

int CSWGrdTriangle::SetPossibleRidgeEdges (void)
{
    int                 i, n, istat, nspill;
    EDgeStruct          *eptr;

/*
    First, set all boundary edges as ridge edges by definition.
*/
    for (i=0; i<NumEdges; i++) {

        eptr = EdgeList + i;
        eptr->flag = 0;
        if (eptr->tri2 == -1) {
            eptr->flag = BOUNDARY_EDGE;
            NodeList[eptr->node1].bflag = 1;
            NodeList[eptr->node2].bflag = 1;
        }
    }

/*
    Find saddle points and local maxima on edges.  These can each
    be start points of ridges.
*/
    nspill = 0;
    for (i=0; i<NumNodes; i++) {
        istat = CheckForSaddle (i);
        if (istat == 1) {
            NodeList[i].adjusting_node = RIDGE_START_NODE;
            NodeList[i].spillnum = nspill;
            nspill++;
            continue;
        }
    }

/*
    Save spill points for eventual association with polygons and output.
*/
    SpillpointList = (SPillpointStruct *)csw_Calloc (nspill * sizeof(SPillpointStruct));
    if (SpillpointList == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    NumSpillpoints = nspill;

    for (i=0; i<NumNodes; i++) {
        if (NodeList[i].spillnum >= 0) {
            nspill = NodeList[i].spillnum;
            SpillpointList[nspill].x = NodeList[i].x;
            SpillpointList[nspill].y = NodeList[i].y;
            SpillpointList[nspill].z = NodeList[i].z;
        }
    }

/*
    Any edge where the two triangles shared by the edge slope
    away from the edge is put into a single segment ridge line.
*/
    for (i=0; i<NumEdges; i++) {
        istat = IsEdgeOnRidge (i);
        if (istat == 1) {
            eptr = EdgeList + i;
            eptr->tflag2 = EDGE_ON_RIDGE;
        }
    }

/*
    Make each triangle edge that is on a boundary a single segment
    ridge line.  If less than two other ridge lines intersect the
    boundary, these will all be deleted later.
*/
    istat = CreateBoundaryRidges ();
    if (istat == -1) {
        return -1;
    }

/*
    Trace uphill from each start point.  The process could create
    more start points, so repeat until nothing was done in an
    iteration.
*/
    for (;;) {
        n = 0;
        for (i=0; i<OrigNumNodes; i++) {
            if (NodeList[i].adjusting_node == RIDGE_START_NODE) {
                istat = TraceByDip (i);
                if (istat == -1) {
                    return -1;
                }
                NodeList[i].adjusting_node = RIDGES_ALREADY_TRACED;
                n++;
            }
        }
        if (n == 0) break;
    }

/*
    Split up any edges that are on a ridge and also crossed by
    other ridges.
*/
    for (i=0; i<NumEdges; i++) {
        istat = SplitRidgeAtEdge (i);
        if (i == -1) {
            return -1;
        }
    }

    return 1;

}  /* end of private SetPossibleRidgeEdges function */



/*
 ************************************************************************************

                   C h e c k F o r E d g e M a x i m u m

 ************************************************************************************

*/

int CSWGrdTriangle::CheckForEdgeMaximum (int nodenum)
{
    NOdeEdgeListStruct     *nptr;
    EDgeStruct             *eptr;
    int                    *list, nlist, i, n1, n2;
    double                 zchk;

    nptr = NodeEdgeList + nodenum;
    if (nptr->done) return 0;

    list = nptr->list;
    nlist = nptr->nlist;
    n1 = -1;
    n2 = -1;

    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        if (eptr->flag != BOUNDARY_EDGE) {
            continue;
        }
        if (n1 == -1) {
            if (eptr->node1 != nodenum) {
                n1 = eptr->node1;
            }
            else {
                n1 = eptr->node2;
            }
        }
        else {
            if (eptr->node1 != nodenum) {
                n2 = eptr->node1;
            }
            else {
                n2 = eptr->node2;
            }
        }
    }

    if (n1 == -1  ||  n2 == -1) {
        return 0;
    }

    zchk = NodeList[nodenum].z;
    if (NodeList[n1].z <= zchk  &&
        NodeList[n2].z <= zchk) {
        return 1;
    }

    return 0;

}  /* end of private CheckForEdgeMaximum function */





/*
 **************************************************************************************

                B u i l d I n i t i a l D r a i n a g e E d g e L i s t

 **************************************************************************************

  Attach a list of any ridge or boundary edges to each node used by the edges.

*/

int CSWGrdTriangle::BuildInitialDrainageEdgeList (void)
{
    int                 i, istat;
    EDgeStruct          *eptr;

    NumNodeEdgeList = NumNodes;
    NodeEdgeList = (NOdeEdgeListStruct *)csw_Calloc (NumNodes * sizeof(NOdeEdgeListStruct));
    if (NodeEdgeList == NULL) {
        NumNodeEdgeList = 0;
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        istat = AddToNodeEdgeList (i, eptr->node1);
        if (istat == -1) return -1;
        istat = AddToNodeEdgeList (i, eptr->node2);
        if (istat == -1) return -1;
    }

    OrigNumNodes = NumNodes;

    return 1;

}  /* end of private BuildInitialDrainageEdgeList function */



/*
 **************************************************************************************

                C l e a n I n i t i a l D r a i n a g e E d g e L i s t

 **************************************************************************************

  Free the lists at each node and reset counters to zero.

*/

int CSWGrdTriangle::CleanInitialDrainageEdgeList (void)
{
    int                 i;
    NOdeEdgeListStruct  *nptr;

    for (i=0; i<OrigNumNodes; i++) {
        nptr = NodeEdgeList + i;
        if (nptr->list) {
            csw_Free (nptr->list);
        }
        nptr->list = NULL;
        nptr->nlist = 0;
        nptr->maxlist = 0;
    }

    return 1;

}  /* end of private CleanInitialDrainageEdgeList function */



/*
 *****************************************************************************************

                        A d d T o N o d e E d g e L i s t

 *****************************************************************************************

  Add an edge number to the list of possible ridge edges at a node.

*/

int CSWGrdTriangle::AddToNodeEdgeList (int edgenum, int nodenum)
{
    int                *list, nlist, maxlist;
    NOdeEdgeListStruct *nptr;

    nptr = NodeEdgeList + nodenum;
    nlist = nptr->nlist;
    maxlist = nptr->maxlist;
    list = nptr->list;

    if (nlist >= maxlist) {
        maxlist += 6;
        list = (int *)csw_Realloc (list, maxlist * sizeof(int));
        if (list == NULL) {
            nptr->list = NULL;
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

    list[nlist] = edgenum;
    nlist++;

    nptr->maxlist = maxlist;
    nptr->list = list;
    nptr->nlist = nlist;

    return 1;

}  /* end of private AddToNodeEdgeList function  */


/*
 *****************************************************************************************

                    R e m o v e F r o m N o d e E d g e L i s t

 *****************************************************************************************

  Remove the specified edge from the specified node's edge list.

*/

int CSWGrdTriangle::RemoveFromNodeEdgeList (int edgenum, int nodenum)

{
    int                *list, nlist, i, n;
    NOdeEdgeListStruct *nptr;

    nptr = NodeEdgeList + nodenum;
    nlist = nptr->nlist;
    list = nptr->list;

    for (i=0; i<nlist; i++) {
        if (list[i] == edgenum) {
            list[i] = -1;
            break;
        }
    }

    n = 0;
    for (i=0; i<nlist; i++) {
        if (list[i] >= 0) {
            list[n] = list[i];
            n++;
        }
    }

    nptr->nlist = n;

    return 1;

}  /* end of private RemoveFromNodeEdgeList function  */




/*
 ******************************************************************************************

                            N e x t R i d g e L i n e

 ******************************************************************************************

  Return a pointer to the next available ridge line structure, or return NULL if
  no more structures are available.

*/

RIdgeLineStruct* CSWGrdTriangle::NextRidgeLine (void)
{
    RIdgeLineStruct        *rptr;

    if (NumRidgeLines >= MaxRidgeLines) {
        MaxRidgeLines += 100;
        RidgeLineList = (RIdgeLineStruct *)csw_Realloc (RidgeLineList,
                                                        MaxRidgeLines * sizeof(RIdgeLineStruct));
        if (RidgeLineList == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return NULL;
        }
    }

    rptr = RidgeLineList + NumRidgeLines;
    NumRidgeLines++;
    memset (rptr, 0, sizeof(RIdgeLineStruct));

    return rptr;

}  /* end of private NextRidgeLine structure */




/*
 ************************************************************************************

                   A d d N o d e T o R i d g e L i n e

 ************************************************************************************

  Append the specified node to the specified ridge line.

*/

int CSWGrdTriangle::AddNodeToRidgeLine (RIdgeLineStruct *rptr, int nodenum)
{
    int                    *nodes;
    int                    nlist, maxlist;
    NOdeStruct             *nptr;

    nodes = rptr->nodes;
    nlist = rptr->nlist;
    maxlist = rptr->maxlist;

    if (nlist >= maxlist) {
        maxlist += 20;
        nodes = (int *)csw_Realloc (nodes, maxlist * sizeof(int));
        if (nodes == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

    nodes[nlist] = nodenum;
    nlist++;

    rptr->nodes = nodes;
    rptr->nlist = nlist;
    rptr->maxlist = maxlist;

    nptr = NodeList + nodenum;

    nptr->flag = NODE_USED_FOR_RIDGE;
    if (nptr->crp == -1) {
        nptr->crp = rptr - RidgeLineList;
    }

    return 1;

}  /* end of private AddNodeToRidgeLine function */



/*
 ****************************************************************************************

                     D e l e t e H a n g i n g R i d g e L i n e s

 ****************************************************************************************

  If a ridge line is not connected to other lines at both ends, it cannot be part of
  a polygon, so mark it as deleted.

*/

void CSWGrdTriangle::DeleteHangingRidgeLines (void)
{
    int                 i, n1, n2, done;
    NOdeEdgeListStruct  *nptr;
    RIdgeLineStruct     *rptr;

    for (;;) {
        done = 0;
        for (i=0; i<NumRidgeLines; i++) {
            rptr = RidgeLineList + i;
            if (rptr->deleted) {
                continue;
            }
            if (rptr->nodes == NULL  ||  rptr->nlist < 2) {
                rptr->deleted = 1;
                continue;
            }
            n1 = rptr->nodes[0];
            n2 = rptr->nodes[rptr->nlist-1];
            if (n1 > OrigNumNodes || n2 > OrigNumNodes) continue;
            nptr = NodeEdgeList + n1;
            if (nptr->nlist < 2) {
                DeleteRidgeLine (rptr);
                done++;
                continue;
            }
            nptr = NodeEdgeList + n2;
            if (nptr->nlist < 2) {
                DeleteRidgeLine (rptr);
                done++;
                continue;
            }
        }
        if (done == 0) {
            break;
        }
    }

    return;

}  /* end of private DeleteHangingRidgeLines function */



/*
 *********************************************************************************************

                            D e l e t e R i d g e L i n e

 *********************************************************************************************

  Mark the ridge line as deleted and also remove it from the lists of its end nodes.

*/

void CSWGrdTriangle::DeleteRidgeLine (RIdgeLineStruct *rptr)
{
    NOdeEdgeListStruct    *np1, *np2;
    int                   n1, n2, i, *list, nlist, n, rnum;

    n1 = rptr->nodes[0];
    n2 = rptr->nodes[rptr->nlist-1];
    rnum = rptr - RidgeLineList;

    np1 = NodeEdgeList + n1;
    np2 = NodeEdgeList + n2;

    list = np1->list;
    nlist = np1->nlist;
    n = 0;
    for (i=0; i<nlist; i++) {
        if (list[i] != rnum) {
            list[n] = list[i];
            n++;
        }
    }
    np1->nlist = n;

    list = np2->list;
    nlist = np2->nlist;
    n = 0;
    for (i=0; i<nlist; i++) {
        if (list[i] != rnum) {
            list[n] = list[i];
            n++;
        }
    }
    np2->nlist = n;

    rptr->deleted = 1;

}  /* end of private DeleteRidgeLine function */





/*
 ************************************************************************************************

                    B u i l d P o l y g o n s F r o m R i d g e L i n e s

 ************************************************************************************************

  Connect ridge lines into polygons.

*/

int CSWGrdTriangle::BuildPolygonsFromRidgeLines (void)
{
    int                 i, n1, n2, nmax, istat;
    NOdeEdgeListStruct  *nptr;
    RIdgeLineStruct     *rptr;
    FOrkInfoStruct      *fptr;

/*
    Split any ridge that has the same first and last node, unless there is
    only one node in the ridge, in which case it is deleted.
*/
    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->deleted) continue;
        n1 = rptr->nodes[0];
        n2 = rptr->nodes[rptr->nlist-1];
        if (n1 == n2) {
            istat = SplitRidge (rptr);
            if (istat == -1) {
                return -1;
            }
        }
    }

/*
    Free the old node edge list and csw_Reallocate a new one large
    enough for the original and new nodes introduced in building
    ridge lines.
*/
    for (i=0; i<OrigNumNodes; i++) {
        nptr = NodeEdgeList + i;
        if (nptr->list) {
            csw_Free (nptr->list);
        }
        nptr->list = NULL;
        nptr->nlist = 0;
        nptr->maxlist = 0;
    }
    csw_Free (NodeEdgeList);

    nmax = -1;
    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->deleted) continue;
        n1 = rptr->nodes[0];
        n2 = rptr->nodes[rptr->nlist-1];
        if (n1 > nmax) nmax = n1;
        if (n2 > nmax) nmax = n2;
    }
    nmax++;

    NumNodeEdgeList = nmax;
    NodeEdgeList = (NOdeEdgeListStruct *)csw_Calloc
                   (nmax * sizeof(NOdeEdgeListStruct));
    if (NodeEdgeList == NULL) {
        NumNodeEdgeList = 0;
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    OrigNumNodes = nmax;

/*
    Rebuild the node edge lists to point to ridge lines
*/
    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->deleted) continue;
        n1 = rptr->nodes[0];
        n2 = rptr->nodes[rptr->nlist-1];
        istat = AddToNodeEdgeList (i, n1);
        if (istat == -1) {
            return -1;
        }
        istat = AddToNodeEdgeList (i, n2);
        if (istat == -1) {
            return -1;
        }
    }

    DeleteHangingRidgeLines ();
    DeleteIdenticalRidges ();

/*
    Process the forked ridge data.
*/
    for (i=0; i<NumFork; i++) {
        fptr = ForkList + i;
        istat = ChooseForkingRidges (fptr->trilist,
                                     fptr->ridgelist,
                                     fptr->nlist,
                                     fptr->nodenum,
                                     fptr->edgelist,
                                     fptr->nedge);
        if (istat == -1) {
            return -1;
        }
    }

    istat = CombineBoundaryRidges ();
    if (istat == -1) {
        return -1;
    }

/*
    The node edge list needs to be rebuilt to account for
    changes made in combining boundary ridges.
*/
    for (i=0; i<OrigNumNodes; i++) {
        nptr = NodeEdgeList + i;
        if (nptr->list) {
            csw_Free (nptr->list);
        }
        nptr->list = NULL;
        nptr->nlist = 0;
        nptr->maxlist = 0;
    }
    csw_Free (NodeEdgeList);

    nmax = -1;
    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->deleted) continue;
        n1 = rptr->nodes[0];
        n2 = rptr->nodes[rptr->nlist-1];
        if (n1 > nmax) nmax = n1;
        if (n2 > nmax) nmax = n2;
    }
    nmax++;

    NumNodeEdgeList = nmax;
    NodeEdgeList = (NOdeEdgeListStruct *)csw_Calloc
                   (nmax * sizeof(NOdeEdgeListStruct));
    if (NodeEdgeList == NULL) {
        NumNodeEdgeList = 0;
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    OrigNumNodes = nmax;

    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->deleted) continue;
        n1 = rptr->nodes[0];
        n2 = rptr->nodes[rptr->nlist-1];
        istat = AddToNodeEdgeList (i, n1);
        if (istat == -1) {
            return -1;
        }
        istat = AddToNodeEdgeList (i, n2);
        if (istat == -1) {
            return -1;
        }
    }

    DeleteHangingRidgeLines ();
    DeleteIdenticalRidges ();

    istat = AddBoundaryLowsToSpillpointList ();
    if (istat == -1) {
        return -1;
    }

/*
    If less than 2 ridges intersect the boundary, do not
    use the boundary in polygon determination.
*/
    BoundaryRidgeCrossingCount = 0;
    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->deleted) continue;
        if (rptr->bflag) continue;
        n1 = rptr->nodes[0];
        n2 = rptr->nodes[rptr->nlist-1];
        if (NodeList[n1].bflag  ||  NodeList[n2].bflag) {
            BoundaryRidgeCrossingCount++;
        }
        if (BoundaryRidgeCrossingCount > 1) {
            break;
        }
    }

    if (BoundaryRidgeCrossingCount < 2) {
        DeleteBoundaryRidges ();
    }

/*
    For each ridge line, trace the two polygons that use it.  Each
    polygon is traced right handed.  One will use the ridge line in
    its forward direction and one will use the ridge line in its
    backward direction.
*/
    for (i=0; i<NumRidgeLines; i++) {

        rptr = RidgeLineList + i;
        if (rptr->deleted) continue;
        if (rptr->forward == 0) {
            TraceSinglePolygon (rptr->nodes[0], rptr);
        }
        if (rptr->backward == 0) {
            TraceSinglePolygon (rptr->nodes[rptr->nlist-1], rptr);
        }

    }

    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->deleted == 1) {
            continue;
        }
        if (rptr->forward <= 0  ||  rptr->backward <= 0) {
            printf ("Ridge number %d not connected correctly.\n", i);
        }
    }

    return 1;

}  /* end of private BuildPolygonsFromRidgeLines function */




/*
 *********************************************************************************

                   T r a c e S i n g l e P o l y g o n

 *********************************************************************************

  Given a starting ridge line, connect it with other ridge lines that form
  a closed polygon.  When more than one alternative is available at a connecting
  point, the alternative that provides the smallest angle with the previous
  ridge line is used.  This insures that the minimum size polygons are traced.

*/

int CSWGrdTriangle::TraceSinglePolygon (int start_node,
                               RIdgeLineStruct *start_line)
{
    int                    nlist, *list, me, n, n2, next, node0,
                           nlist2, *list2, istat, rlflag, j;
    RIdgeLineStruct        *rptr, *rp;

    node0 = start_node;
    me = start_line - RidgeLineList;
    PolySegs[0] = me;
    rptr = start_line;
    rptr->tflag = 1;
    list = rptr->nodes;
    nlist = rptr->nlist;
    if (list[0] == node0) {
        n2 = list[nlist-1];
        rptr->forward = -1;
    }
    else {
        n2 = list[0];
        rptr->backward = -1;
    }
    n = 1;
    rlflag = 1;

    for (;;) {
        list2 = NodeEdgeList[n2].list;
        nlist2 = NodeEdgeList[n2].nlist;
        next = ChooseConnectingRidge (list2, nlist2, me, n2, &rlflag);
        if (next == -1) {
            for (j=0; j<n; j++) {
                rp = RidgeLineList + PolySegs[j];
                if (rp->forward == -1) rp->forward = 0;
                if (rp->backward == -1) rp->backward = 0;
                rp->tflag = 0;
            }
            return -1;
        }

        rptr = RidgeLineList + next;
        rptr->tflag = 1;
        list = rptr->nodes;
        nlist = rptr->nlist;
        if (list[0] == n2) {
            n2 = list[nlist-1];
            rptr->forward = -1;
        }
        else {
            n2 = list[0];
            rptr->backward = -1;
        }
        PolySegs[n] = next;
        n++;
        if (n2 == node0) {
            break;
        }
        me = next;
    }

    istat = CheckPolygonForSpillpoint (n);
    if (istat == 0) {
        printf ("No spillpoint in polygon number %d\n", NumPolygons);
    }

    istat = AddPolygonToOutput (n);
    if (istat == -1) {
        return -1;
    }

    return 1;

}  /* end of private TraceSinglePolygon function */



/*
 ***********************************************************************************

                     C h o o s e C o n n e c t i n g R i d g e

 ***********************************************************************************

  Given a list of ridge lines connected to a node and the identity of the line
  that is entering the node and therefore needs to be connected to, choose the
  outgoing ridge line that will close the tightest polygon.  This is done by finding
  the unused line that creates the smallest angle with the incoming line and that
  also obeys the right or left handedness of the current tracing.

  The ridge line number is returned on success or -1 is returned on failure.

*/

int CSWGrdTriangle::ChooseConnectingRidge (int *list, int nlist, int me,
                                  int node, int *right_left_flag)
{
    int                i, n1, n2, good, rlflag, nsav, goodsav;
    RIdgeLineStruct    *rptr;
    double             x1, y1, x2, y2, x3, y3, dx, dy,
                       dist, asav,
                       dchk, dtmp, ang, a0, a1,
                       dang, maxang, minang;

/*
    If there are 2 or less connections at the node, the angle to find the
    tightest closing polygon is not needed.  Just use whatever ridge line
    is available.
*/

    if (nlist < 3) {
        for (i=0; i<nlist; i++) {
            if (list[i] == me) continue;
            rptr = RidgeLineList + list[i];
            if (rptr->deleted) continue;
            return (list[i]);
        }
        return -1;
    }

    if (nlist >= 2000) return -1;

    rlflag = *right_left_flag;

/*
    More than 2 connections at the node, so find the one that will make the
    tightest polygon.  The connection with the smallest angle to the last
    vector of the previous ridge is used.  It must also be the correct
    handedness (right or left).
*/
    rptr = RidgeLineList + me;
    if (node == rptr->nodes[0]) {
        n1 = rptr->nodes[1];
        n2 = rptr->nodes[0];
    }
    else {
        n1 = rptr->nodes[rptr->nlist-2];
        n2 = rptr->nodes[rptr->nlist-1];
    }
    x1 = NodeList[n1].x;
    y1 = NodeList[n1].y;
    x2 = NodeList[n2].x;
    y2 = NodeList[n2].y;

  /*
   *  x1, y1,  x2, y2 are the segment endpoints for the last segment
   *  of the ridge line entering the node.
   */
    dx = x1 - x2;
    dy = y1 - y2;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    a0 = atan2(dy, dx);
    asav = 1.e30;

  /*
   * dist is the length of the last segment entering the node
   * The available segments at the node are normalized to a
   * length of dist.  The distance between the non common
   * endpoints of the normalized segments is used.  The segments
   * with the smallest distance are the smallest angle also.
   */
    dchk = 1.e30;
    maxang = -1.e30;
    minang = 1.e30;
    good = -1;
    goodsav = -1;
    nsav = 0;

    for (i=0; i<nlist; i++) {
        if (list[i] == me) {
            continue;
        }
        rptr = RidgeLineList + list[i];
        if (rptr->deleted) {
            continue;
        }
        if (rptr->nodes[0] == n2) {
            if (rptr->forward != 0) continue;
            n1 = rptr->nodes[1];
        }
        else {
            if (rptr->backward != 0) continue;
            n1 = rptr->nodes[rptr->nlist-2];
        }
        x3 = NodeList[n1].x;
        y3 = NodeList[n1].y;
        if (x3 - x2 == 0.0  &&  y3 - y2 == 0.0) {
            continue;
        }
        dx = x3 - x2;
        dy = y3 - y2;
        ang = atan2(dy, dx);
        x3 = x2 + dist * cos(ang);
        y3 = y2 + dist * sin(ang);

        dx = x3 - x1;
        dy = y3 - y1;
        dtmp = dx * dx + dy * dy;
        dtmp = sqrt(dtmp);
        a1 = atan2(dy, dx);
        a1 = ang;
        dang = a1 - a0;

    /*
     * no handedness is yet defined, so use the smallest distance
     * which will also be the smallest angle.
     */
        if (rlflag == 0) {
            if (dang > PI) {
                dang -= TWOPI;
            }
            else if (dang < -PI) {
                dang += TWOPI;
            }
            nsav++;
            if (dtmp < dchk) {
                good = i;
                dchk = dtmp;
                asav = dang;
                goodsav = nsav - 1;
            }
        }

    /*
     * Left handedness is needed, so make all angles negative
     * and choose the one closest to zero.
     */
        else if (rlflag == -1) {
            if (dang > 0) dang -= TWOPI;
            if (dang > maxang) {
                maxang = dang;
                good = i;
            }
            nsav++;
        }

    /*
     * Right handedness is needed, so make all angles positive
     * and choose the one closest to zero.
     */
        else {
            if (dang < 0) dang += TWOPI;
            if (dang < minang) {
                minang = dang;
                good = i;
            }
            nsav++;
        }
    }

    if (good == -1) return -1;

    if (rlflag == 0  &&  nsav > 1  &&  goodsav != -1) {
        if (asav < 0.0) {
            rlflag = -1;
        }
        else {
            rlflag = 1;
        }
    }

    *right_left_flag = rlflag;

    return (list[good]);

}



/*
 *******************************************************************************

               A d d P o l y g o n T o O u t p u t

 *******************************************************************************

*/

int CSWGrdTriangle::AddPolygonToOutput (int nseg)
{
    POlygonStruct         *pptr = NULL;
    RIdgeLineStruct       *rptr = NULL, *rp2 = NULL;
    double                *xp = NULL, *yp = NULL, xt, yt;
    int                   i, npts, n, j, *list = NULL, nlist, last;
    int                   istat, np, nspill;

    CSWPolyUtils        ply_utils_obj;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (nseg < 2) {
        return 1;
    }

    npts = 0;
    for (i=0; i<nseg; i++) {
        rptr = RidgeLineList + PolySegs[i];
        npts += rptr->nlist;
        if (rptr->forward == -1) rptr->forward = 1;
        if (rptr->backward == -1) rptr->backward = 1;
    }

    if (NumPolygons >= MaxPolygons) {
        int   mp_sav = MaxPolygons;
        MaxPolygons += 10;
        PolygonList = (POlygonStruct *)csw_Realloc
                      (PolygonList, MaxPolygons * sizeof(POlygonStruct));
        if (PolygonList == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        memset ((void *)(PolygonList + mp_sav), 0, 10 * sizeof(POlygonStruct));
    }

    xp = (double *)csw_Malloc (npts * 2 * sizeof(double));
    if (xp == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    yp = xp + npts;

    rptr = RidgeLineList + PolySegs[0];
    list = rptr->nodes;
    nlist = rptr->nlist;
    rp2 = RidgeLineList + PolySegs[1];
    n = 0;
    nspill = 0;

/*
 *  The first ridge line is output either forward or backward
 *  depending upon the node that connects with the second
 *  ridge line.
 */
    if (list[nlist-1] == rp2->nodes[0]  ||
        list[nlist-1] == rp2->nodes[rp2->nlist-1]) {
        for (j=0; j<nlist; j++) {
            xp[n] = NodeList[list[j]].x;
            yp[n] = NodeList[list[j]].y;
            if (NodeList[list[j]].spillnum >= 0) {
                nspill++;
            }
            n++;
        }
        last = list[nlist-1];
    }
    else {
        for (j=nlist-1; j>=0; j--) {
            xp[n] = NodeList[list[j]].x;
            yp[n] = NodeList[list[j]].y;
            if (NodeList[list[j]].spillnum >= 0) {
                nspill++;
            }
            n++;
        }
        last = list[0];
    }

/*
 *  Output the remaining ridges either forward or reverse depending upon
 *  which end connects with t he last node output.
 */
    for (i=1; i<nseg; i++) {
        rptr = RidgeLineList + PolySegs[i];
        list = rptr->nodes;
        nlist = rptr->nlist;
        if (list[0] == last) {
            for (j=1; j<nlist; j++) {
                xp[n] = NodeList[list[j]].x;
                yp[n] = NodeList[list[j]].y;
                if (NodeList[list[j]].spillnum >= 0) {
                    nspill++;
                }
                n++;
            }
            last = list[nlist-1];
        }
        else {
            for (j=nlist-2; j>=0; j--) {
                xp[n] = NodeList[list[j]].x;
                yp[n] = NodeList[list[j]].y;
                if (NodeList[list[j]].spillnum >= 0) {
                    nspill++;
                }
                n++;
            }
            last = list[0];
        }
    }

    if (nspill == 0) {
        printf ("Polygon number %d has no spill point on it.\n", NumPolygons);
    }

/*
 *  If this polygon encloses a ridge point of a ridge that was not used to
 *  build this polygon, then it is the boundary polygon and it should not
 *  be output as a drainage area.  The exception is if all of the ridge
 *  lines are used by the polygon, in which case the border is the only
 *  drainage area.
 */
    ply_utils_obj.ply_setgraze (GrazeDistance / 10.0);
    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->tflag == 1  ||  rptr->nlist < 2) {
            continue;
        }
        np = rptr->nlist / 2;
        np--;
        if (np < 0) np = 0;
        xt = (NodeList[rptr->nodes[np]].x + NodeList[rptr->nodes[np+1]].x) / 2.0;
        yt = (NodeList[rptr->nodes[np]].y + NodeList[rptr->nodes[np+1]].y) / 2.0;
        istat = ply_utils_obj.ply_point (xp, yp, n, xt, yt);
        if (istat == 1) {
            return 1;
        }
        if (istat == -1) {
            break;
        }
    }

    for (i=0; i<nseg; i++) {
        rptr = RidgeLineList + PolySegs[i];
        rptr->tflag = 0;
        list = rptr->nodes;
        nlist = rptr->nlist;
        for (j=0; j<nlist; j++) {
            istat = AddPolygonToSpillpoint (list[j], NumPolygons);
            if (istat == -1) {
                return -1;
            }
        }
    }

    pptr = PolygonList + NumPolygons;
    pptr->x = xp;
    pptr->y = yp;
    pptr->z = NULL;
    pptr->npts = n;
    NumPolygons++;

    bsuccess = true;

    return 1;

}  /* end of private AddPolygonToOutput function */




/*
 ***************************************************************************

                     O p p o s i t e Z v a l u e

 ***************************************************************************

*/

double CSWGrdTriangle::OppositeZvalue (EDgeStruct *eptr, int node)
{
    double             z;

    if (eptr->node1 != node) {
        z = NodeList[eptr->node1].z;
    }
    else {
        z = NodeList[eptr->node2].z;
    }

    return z;

}  /* end of private OppositeZvalue function */



/*
 *****************************************************************************

                        A d j a c e n t E d g e

 *****************************************************************************

*/

int CSWGrdTriangle::AdjacentEdge (TRiangleStruct *tptr, int e1, int node)
{
    EDgeStruct          *eptr;

    if (tptr->edge1 != e1) {
        eptr = EdgeList + tptr->edge1;
        if (eptr->node1 == node  ||  eptr->node2 == node) {
            return (tptr->edge1);
        }
    }
    if (tptr->edge2 != e1) {
        eptr = EdgeList + tptr->edge2;
        if (eptr->node1 == node  ||  eptr->node2 == node) {
            return (tptr->edge2);
        }
    }
    if (tptr->edge3 != e1) {
        eptr = EdgeList + tptr->edge3;
        if (eptr->node1 == node  ||  eptr->node2 == node) {
            return (tptr->edge3);
        }
    }

    return -1;

}  /* end of private AdjacentEdge function */




/*
 **********************************************************************************

                       C h e c k F o r S a d d l e

 **********************************************************************************

  Check a trimesh node to see if it is a saddle point.  A saddle point has more
  than one transition from elevation less than the point to elevation greater
  than or equal to the point as the edges adjacent to the point are looked
  at in order.  If any edge adjacent to the point has only one triangle
  that uses the edge (i.e. the edge is on the boundary of the trimesh), then
  this function always returns zero, or no saddle point.  If the point is a
  saddle, one is returned.  If the point is not a saddle, zero is returned.

*/

int CSWGrdTriangle::CheckForSaddle (int nodenum)
{
    NOdeEdgeListStruct    *nptr;
    EDgeStruct            *eptr;
    TRiangleStruct        *tptr;
    int                   istat, e1, eanch, count, laststate, tnumlast;
    double                zcenter, znode;


/*
    If the node if close to a point passed in the inactive spill
    point list, do not call it a saddle point.
*/
    SpillTypeForSearch = 1;
    istat = SearchSpillFilters (NodeList[nodenum].x,
                                NodeList[nodenum].y);
    if (istat == 1) {
        return 0;
    }

/*
    If the node is not close to a point passed as an active spill
    point or close to an inactive super spill point then do not call
    it a saddle point.
    SpillTypeForSearch = 0;
    SpillFilterDistance *= 2.0;
    istat = SearchSpillFilters (NodeList[nodenum].x,
                                NodeList[nodenum].y);
    if (istat == 0) {
        SpillTypeForSearch = 2;
        SpillFilterDistance *= 2.0;
        istat = SearchSpillFilters (NodeList[nodenum].x,
                                    NodeList[nodenum].y);
        SpillFilterDistance /= 2.0;
        if (istat == 0) {
            return 0;
        }
    }
    SpillFilterDistance /= 2.0;
*/

    istat = CheckForBoundarySaddle (nodenum);
    if (istat == 1) {
        return 1;
    }

    zcenter = NodeList[nodenum].z;
    nptr = NodeEdgeList + nodenum;
    eanch = nptr->list[0];
    e1 = eanch;
    eptr = EdgeList + e1;
    tptr = TriangleList + eptr->tri1;
    tnumlast = eptr->tri1;
    znode = OppositeZvalue(eptr, nodenum);
    laststate = 1;
    if (znode < zcenter) laststate = -1;
    count = 0;

    for (;;) {
        e1 = AdjacentEdge (tptr, e1, nodenum);
        assert (e1 >= 0);
        eptr = EdgeList + e1;
        znode = OppositeZvalue (eptr, nodenum);
        if (znode >= zcenter) {
            if (laststate == -1) count++;
            laststate = 1;
        }
        else {
            laststate = -1;
        }
        if (e1 == eanch) break;
        tptr = TriangleList + eptr->tri1;
        if (eptr->tri1 == tnumlast) {
            if (eptr->tri2 == -1) {
                return 0;
            }
            else {
                tptr = TriangleList + eptr->tri2;
            }
        }
        tnumlast = tptr - TriangleList;
    }

    if (count > 1) {
        istat = VerifySaddle (nodenum);
        return istat;
    }

    return 0;

}  /* end of private CheckForSaddle function */




/*
 *********************************************************************************************

                  C h e c k F o r B o u n d a r y S a d d l e

 *********************************************************************************************

  Return one if the node is on the model boundary and is also a saddle point.  Return
  zero otherwise.

*/

int CSWGrdTriangle::CheckForBoundarySaddle (int nodenum)
{
    NOdeStruct           *nptr;
    NOdeEdgeListStruct   *neptr;
    EDgeStruct           *eptr;
    int                  istat, n1, *list, nlist, i;
    double               zchk;

    nptr = NodeList + nodenum;
    if (nptr->bflag != 1) {
        return 0;
    }

    istat = CheckForEdgeMinimum (nodenum);
    if (istat == 0) {
        return 0;
    }

    neptr = NodeEdgeList + nodenum;
    list = neptr->list;
    nlist = neptr->nlist;
    if (list == NULL  ||  nlist < 3) {
        return 0;
    }

    zchk = nptr->z;

    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        if (eptr->flag == BOUNDARY_EDGE) {
            continue;
        }
        n1 = eptr->node1;
        if (n1 == nodenum) {
            n1 = eptr->node2;
        }
        nptr = NodeList + n1;
        if (nptr->z < zchk) {
            istat = VerifyEdgeSaddle (nodenum);
            return istat;
        }
    }

    return 0;

}  /* end of private CheckForBoundarySaddle function */





/*
 *********************************************************************************************

                              V e r i f y S a d d l e

 *********************************************************************************************

  This function is called if a trimesh node is determined to be a saddle.  Since the flow lines
  and traps (as of june 2001) use rectilinear grids then the trimesh node saddle is checked to
  see if it also qualifies as being close to a rectilinear saddle.  If it is close to a
  rectilinear saddle, this function returns 1.  If it is not close this function returns zero.

*/

int CSWGrdTriangle::VerifySaddle (int nodenum)
{
    NOdeStruct       *nptr;
    int              col, row, istat;

    nptr = NodeList + nodenum;

    col = (int)((nptr->x - X1Saddle) / XspaceSaddle);
    row = (int)((nptr->y - Y1Saddle) / YspaceSaddle);

    istat = CheckGridInteriorSaddle (col-1, row-1);
    if (istat == 1) {
        return 1;
    }
    istat = CheckGridInteriorSaddle (col, row-1);
    if (istat == 1) {
        return 1;
    }
    istat = CheckGridInteriorSaddle (col-1, row);
    if (istat == 1) {
        return 1;
    }
    istat = CheckGridInteriorSaddle (col, row);
    if (istat == 1) {
        return 1;
    }

    return istat;

}  /* end of private VerifySaddle function */





/*
 *********************************************************************************************

                             V e r i f y E d g e S a d d l e

 *********************************************************************************************

  This function is called if a trimesh node is determined to be a saddle.  Since the flow lines
  and traps (as of june 2001) use rectilinear grids then the trimesh node saddle is checked to
  see if it also qualifies as being close to a rectilinear saddle.  If it is close to a
  rectilinear saddle, this function returns 1.  If it is not close this function returns zero.

*/

int CSWGrdTriangle::VerifyEdgeSaddle (int nodenum)
{
    NOdeStruct       *nptr;
    int              col, row, istat;

    nptr = NodeList + nodenum;

    col = (int)((nptr->x - X1Saddle) / XspaceSaddle);
    row = (int)((nptr->y - Y1Saddle) / YspaceSaddle);

    istat = CheckGridEdgeSaddle (col, row);

    return istat;

}  /* end of private VerifyEdgeSaddle function */





/*
 *********************************************************************************************

                          I s E d g e O n R i d g e

 *********************************************************************************************

  Return one if the specified edge has each triangle it shares sloping away from it.
  Otherwise, return zero.

*/

int CSWGrdTriangle::IsEdgeOnRidge (int edgenum)
{
    int             i, n1, n2;
    EDgeStruct      *eptr;
    double          x1, y1, z1,
                    x2, y2, z2,
                    x3, y3, z3,
                    xint, yint, zint,
                    dx, dy, dz, adx, ady;

    i = edgenum;

    eptr = EdgeList + i;
    if (eptr->tri2 == -1) {
        return 0;
    }
    eptr->flag = 0;

    n1 = OppositeNode (eptr->tri1, i);
    n2 = OppositeNode (eptr->tri2, i);

    x1 = NodeList[eptr->node1].x;
    y1 = NodeList[eptr->node1].y;
    z1 = NodeList[eptr->node1].z;
    x2 = NodeList[eptr->node2].x;
    y2 = NodeList[eptr->node2].y;
    z2 = NodeList[eptr->node2].z;
    if (z1 > 1.e20  ||  z2 > 1.e20) {
        return 0;
    }

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    if (dx == 0.0  &&  dy == 0.0) {
        return 0;
    }
    adx = dx;
    if (dx < 0.0) adx = -dx;
    ady = dy;
    if (dy < 0.0) ady = -dy;

    x3 = NodeList[n1].x;
    y3 = NodeList[n1].y;
    z3 = NodeList[n1].z;

    gpf_perpintpoint2 (x1, y1, x2, y2, x3, y3, &xint, &yint);
    if (adx >= ady) {
        zint = z1 + dz * (xint - x1) / dx;
    }
    else {
        zint = z1 + dz * (yint - y1) / dy;
    }

    if (zint < z3) return 0;

    x3 = NodeList[n2].x;
    y3 = NodeList[n2].y;
    z3 = NodeList[n2].z;

    gpf_perpintpoint2 (x1, y1, x2, y2, x3, y3, &xint, &yint);
    if (adx >= ady) {
        zint = z1 + dz * (xint - x1) / dx;
    }
    else {
        zint = z1 + dz * (yint - y1) / dy;
    }

    if (zint < z3) return 0;

    return 1;

}  /* end of private IsEdgeOnRidge function */





/*
 *******************************************************************************************

                        C a l c T r i a n g l e S l o p e

 *******************************************************************************************

  Return the plane coeficients, the strike and the dip of the specified triangle
  in the current trimesh topology.
*/

int CSWGrdTriangle::CalcTriangleSlope (TRiangleStruct *tptr,
                              double *strike, double *dip, double *coef)
{
    double                    x[3], y[3], z[3], amp, ang, dy, dx;

    *strike = 1.e30;
    *dip = 1.e30;
    coef[0] = 1.e30;

    if (tptr->deleted) {
        return -1;
    }
    TrianglePoints (tptr, x, y, z);
    grd_utils_ptr->grd_calc_double_plane (x, y, z, 3, coef);
    dx = coef[1];
    dy = coef[2];
    ang = atan2 (dy, dx);
    amp = dx * dx + dy * dy;
    amp = sqrt(amp);

    *strike = ang;
    *dip = amp;

    return 1;

}  /* end of private CalcTriangleSlope function */





/*
 ***********************************************************************************

                            T r a c e B y D i p

 ***********************************************************************************

  Follow the uphill direction from saddle points or other possible ridge
  origination points until all ridges are traced.
*/

int CSWGrdTriangle::TraceByDip (int nodenum)
{
    NOdeEdgeListStruct    *nptr;
    EDgeStruct            *eptr;
    TRiangleStruct        *tptr;
    int                   *list, nlist, exitedge, istat, i, done;
    int                   tn1, tracelist[10], ridgelist[10], ntrace;
    int                   nridge, n2, edone, boundarycase;
    double                zcenter;

    nptr = NodeEdgeList + nodenum;
    list = nptr->list;
    nlist = nptr->nlist;
    ntrace = 0;
    done = 0;
    memset (ridgelist, 0, 10 * sizeof(int));

    boundarycase = 0;
    if (NodeList[nodenum].bflag == 1  &&
        NodeList[nodenum].spillnum == -1) {
        boundarycase = 1;
    }

/*
    Count the number of edges attached to the node that are
    also "natural" ridges.
*/
    zcenter = NodeList[nodenum].z;
    nridge = 0;
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        if (eptr->tflag2 == EDGE_ON_RIDGE  ||
            eptr->tflag2 == EDGE_USED_FOR_RIDGE) {
            n2 = eptr->node1;
            if (n2 == nodenum) {
                n2 = eptr->node2;
            }
            if (NodeList[n2].z > zcenter  ||
                eptr->tflag2 == EDGE_USED_FOR_RIDGE) {
                nridge++;
            }
        }
    }

/*
    Find all of the edges surrounding the point that may be crossed
    by ridges.  Trace any ridges that start with these segments.
*/
    for (i=0; i<nlist; i++) {

        LastUpDipRidgeNumber = -1;

        edone = 0;
        eptr = EdgeList + list[i];

        if (eptr->tflag2 == EDGE_USED_FOR_RIDGE) {
            continue;
        }

    /*
     * If both triangles that share the edge slope away from
     * the edge, then the entire edge is part of a ridge and
     * neither triangle sharing the edge can be used for another
     * ridge originating from this node.
     */
        if (eptr->tflag2 == EDGE_ON_RIDGE) {
            n2 = eptr->node2;
            if (n2 == nodenum) {
                n2 = eptr->node1;
            }
            if (NodeList[n2].z < NodeList[nodenum].z) {
                continue;
            }
            eptr->tflag2 = EDGE_USED_FOR_RIDGE;
            istat = TraceRidgeUpDip (nodenum, list[i], 1);
            if (istat == -1) {
                return -1;
            }
            tn1 = eptr->tri1;
            tptr = TriangleList + tn1;
            tptr->flag = TRIANGLE_USED_FOR_RIDGE;
            tn1 = eptr->tri2;
            if (tn1 >= 0) {
                tptr = TriangleList + tn1;
                tptr->flag = TRIANGLE_USED_FOR_RIDGE;
            }
            done = 1;
            edone = 1;
            continue;
        }

    /*
     * If the up slope line through a triangle intersects the opposite edge
     * from the node, then a ridge starts along this vector.
     */
        eptr = EdgeList + list[i];
        tn1 = eptr->tri1;
        assert (tn1 >= 0);
        tptr = TriangleList + tn1;
        if (tptr->flag != TRIANGLE_USED_FOR_RIDGE  &&  boundarycase == 0) {
            exitedge = FindRidgeExit (tptr, nodenum, NULL);
            if (exitedge >= 0) {
                tptr = TriangleList + tn1;
                tptr->flag = TRIANGLE_USED_FOR_RIDGE;
                istat = TraceRidgeUpDip (nodenum, exitedge, 0);
                if (istat == -1) {
                    return -1;
                }
                tracelist[ntrace] = tn1;
                ridgelist[ntrace] = LastUpDipRidgeNumber;
                ntrace++;
                done = 1;
                edone = 1;
            }
        }

        eptr = EdgeList + list[i];
        tn1 = eptr->tri2;
        if (tn1 >= 0) {
            tptr = TriangleList + tn1;
            if (tptr->flag != TRIANGLE_USED_FOR_RIDGE  &&  boundarycase == 0) {
                exitedge = FindRidgeExit (tptr, nodenum, NULL);
                if (exitedge >= 0) {
                    tptr = TriangleList + tn1;
                    tptr->flag = TRIANGLE_USED_FOR_RIDGE;
                    istat = TraceRidgeUpDip (nodenum, exitedge, 0);
                    if (istat == -1) {
                        return -1;
                    }
                    tracelist[ntrace] = tn1;
                    ridgelist[ntrace] = LastUpDipRidgeNumber;
                    ntrace++;
                    done = 1;
                    edone = 1;
                }
            }
        }

    /*
     * If the edge is a boundary edge and no trace has been done using it,
     * trace with it if it goes uphill.
     */
        eptr = EdgeList + list[i];
        if (edone == 0  &&  eptr->flag == BOUNDARY_EDGE) {
            n2 = eptr->node2;
            if (n2 == nodenum) {
                n2 = eptr->node1;
            }
            if (NodeList[n2].z >= NodeList[nodenum].z) {
                eptr->tflag2 = EDGE_USED_FOR_RIDGE;
                istat = TraceRidgeUpDip (nodenum, list[i], 1);
                if (istat == -1) {
                    return -1;
                }
                tn1 = eptr->tri1;
                tptr = TriangleList + tn1;
                tptr->flag = TRIANGLE_USED_FOR_RIDGE;
                tn1 = eptr->tri2;
                if (tn1 >= 0) {
                    tptr = TriangleList + tn1;
                    tptr->flag = TRIANGLE_USED_FOR_RIDGE;
                }
                done = 1;
                continue;
            }
        }
    }

/*
    If the center node is on the boundary of the trimesh and
    no trace was done from it, trace up to any higher adjacent
    node that is also on the edge.
*/
    if (NodeList[nodenum].bflag == 1  &&  done == 0) {
        for (i=0; i<nlist; i++) {
            eptr = EdgeList + list[i];
            if (eptr->tflag2 == EDGE_USED_FOR_RIDGE) {
                continue;
            }
            if (eptr->flag == BOUNDARY_EDGE) {
                n2 = eptr->node1;
                if (n2 == nodenum) {
                    n2 = eptr->node2;
                }
                if (NodeList[n2].z > zcenter) {
                    eptr->tflag2 = EDGE_USED_FOR_RIDGE;
                    istat = TraceRidgeUpDip (nodenum, list[i], 1);
                    if (istat == -1) {
                        return -1;
                    }
                    tn1 = eptr->tri1;
                    tptr = TriangleList + tn1;
                    tptr->flag = TRIANGLE_USED_FOR_RIDGE;
                }
            }
        }
    }

/*
    If there are one updip and two or more natural edges, the
    updip is a "ghost" ridge and should be deleted.
*/
    if (NodeList[nodenum].spillnum == -1  &&  nridge > 1  &&  ntrace == 1) {
        RidgeLineList[ridgelist[0]].deleted = 1;
    }
    else if (ntrace > 1) {
        SaveForkingRidgeInfo (tracelist, ridgelist, ntrace, nodenum,
                              NodeEdgeList[nodenum].list,
                              NodeEdgeList[nodenum].nlist);
    }

    for (i=0; i<NumTriangles; i++) {
        TriangleList[i].flag = 0;
    }

    LastUpDipRidgeNumber = -1;

    return 1;

}  /* end of private TraceByDip function */




/*
 ***************************************************************************

                       F i n d R i d g e E x i t

 ***************************************************************************

  Given a triangle and the node on an edge of that triangle where a ridge
  line intersects the triangle, calculate the node on another edge where
  the ridge line exits the triangle.  This node number is put into the
  edge's number member and the sequence number of the exit edge is returned
  by this function.

  The exit is calculated by extending a ray from the node entering the
  triangle in the updip direction of the triangle.  If that ray interects
  another edge of the triangle, then the intersection point is the exit
  from the triangle and the triangle has a potential ridge line in it.

*/

int CSWGrdTriangle::FindRidgeExit (TRiangleStruct *tptr,
                          int nodenum,
                          EDgeStruct *eptrin)
{
    int                elist[3], et, nt, istat, newnode;
    double             direction, slope, coef[3],
                       x0, y0, xt, yt,
                       x1, y1, z1, x2, y2, z2, xint, yint, zint,
                       dx, dy, dz, adx, ady;
    EDgeStruct         *eptr;

    CSWPolyUtils        ply_utils_obj;

/*
    elist is up to two edges that might possible intersect with
    the updip vector from the start edge (or node).
*/
    elist[0] = 0;
    elist[1] = 0;
    if (eptrin == NULL) {
        elist[0] = OppositeEdge (tptr, nodenum);
        elist[1] = -1;
        nt = 1;
    }
    else {
        et = eptrin - EdgeList;
        nt = 0;
        if (et != tptr->edge1) {
            elist[nt] = tptr->edge1;
            nt++;
        }
        if (et != tptr->edge2) {
            elist[nt] = tptr->edge2;
            nt++;
        }
        if (et != tptr->edge3) {
            elist[nt] = tptr->edge3;
            nt++;
        }
    }

/*
    Find the direction of the updip vector through the triangle.
*/
    CalcTriangleSlope (tptr, &direction, &slope, coef);
    x0 = NodeList[nodenum].x;
    y0 = NodeList[nodenum].y;
    xt = x0 + AreaSize * cos (direction);
    yt = y0 + AreaSize * sin (direction);

/*
    Does the updip vector intersect the first possible edge.
*/
    eptr = EdgeList + elist[0];
    nt = eptr->node1;
    x1 = NodeList[nt].x;
    y1 = NodeList[nt].y;
    z1 = NodeList[nt].z;
    nt = eptr->node2;
    x2 = NodeList[nt].x;
    y2 = NodeList[nt].y;
    z2 = NodeList[nt].z;

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    adx = dx;
    if (dx < 0.0) adx = -dx;
    ady = dy;
    if (dy < 0.0) ady = -dy;

    istat = ply_utils_obj.ply_segint (x0, y0, xt, yt,
                        x1, y1, x2, y2,
                        &xint, &yint);

    if (istat == 0) {

        if (adx > ady) {
            zint = z1 + dz * (xint - x1) / dx;
        }
        else {
            zint = z1 + dz * (yint - y1) / dy;
        }
        newnode = AddNode (xint, yint, zint, 0);
        eptr = EdgeList + elist[0];
        eptr->number = newnode;
        return (eptr - EdgeList);
    }

    if (elist[1] == -1) return -1;

/*
    Does the updip vector intersect the second possible edge?
*/
    eptr = EdgeList + elist[1];
    nt = eptr->node1;
    x1 = NodeList[nt].x;
    y1 = NodeList[nt].y;
    z1 = NodeList[nt].z;
    nt = eptr->node2;
    x2 = NodeList[nt].x;
    y2 = NodeList[nt].y;
    z2 = NodeList[nt].z;

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    adx = dx;
    if (dx < 0.0) adx = -dx;
    ady = dy;
    if (dy < 0.0) ady = -dy;

    istat = ply_utils_obj.ply_segint (x0, y0, xt, yt,
                        x1, y1, x2, y2,
                        &xint, &yint);
    if (istat == 0) {

        if (adx > ady) {
            zint = z1 + dz * (xint - x1) / dx;
        }
        else {
            zint = z1 + dz * (yint - y1) / dy;
        }
        newnode = AddNode (xint, yint, zint, 0);
        eptr = EdgeList + elist[1];
        eptr->number = newnode;
        return (eptr - EdgeList);
    }

    return -1;

}  /* end of private FindRidgeExit function */




/*
 ****************************************************************************

                    T r a c e R i d g e U p D i p

 ****************************************************************************

  Trace from edge to edge in the updip direction of each triangle.

*/

int CSWGrdTriangle::TraceRidgeUpDip (int start_node, int next_edge_num, int eflag)
{
    RIdgeLineStruct       *rptr;
    EDgeStruct            *eptr;
    TRiangleStruct        *tptr;
    int                   nextedge, nextnode, istat, grazeflag;
    int                   n2, tn1;

    rptr = NextRidgeLine ();
    if (rptr == NULL) {
        return -1;
    }

    istat = AddNodeToRidgeLine (rptr, start_node);
    if (istat == -1) {
        return -1;
    }

    LastUpDipRidgeNumber = rptr - RidgeLineList;

    eptr = EdgeList + next_edge_num;

/*
 *  If the starting edge is an original triangle edge, then output a
 *  single segment ridge line and mark the opposite node as a start
 *  point for another ridge.
 */
    if (eflag == 1) {
        istat = AssignRidgeToEdge (next_edge_num, rptr-RidgeLineList);
        if (istat == -1) {
            return -1;
        }
        n2 = eptr->node1;
        if (n2 == start_node) {
            n2 = eptr->node2;
        }
        eptr->tflag2 = EDGE_USED_FOR_RIDGE;
        if (NodeList[n2].adjusting_node == RIDGE_START_NODE) {
            istat = AddNodeToRidgeLine (rptr, n2);
            return 1;
        }
        if (NodeList[n2].flag == NODE_USED_FOR_RIDGE) {
            if (NodeList[n2].adjusting_node != RIDGES_ALREADY_TRACED) {
                NodeList[n2].adjusting_node = RIDGE_START_NODE;
            }
            AddNodeToRidgeLine (rptr, n2);
            SplitRidgeAtNode (n2);
            return 1;
        }
        AddNodeToRidgeLine (rptr, n2);
        if (NodeList[n2].adjusting_node != RIDGES_ALREADY_TRACED) {
            NodeList[n2].adjusting_node = RIDGE_START_NODE;
        }
        return 1;
    }

/*
 *  If the first segment of the ridge ends on an edge that has already
 *  been used for another ridge, then the ridge ends at that crossing
 *  point.  The original ridge that used the edge needs to be split
 *  at the crossing point.
 */
    if (eptr->tflag2 == EDGE_USED_FOR_RIDGE  ||
        eptr->tflag2 == EDGE_ON_RIDGE) {
        istat = AddNodeToRidgeLine (rptr, eptr->number);
        if (istat == -1) {
            return -1;
        }
        istat = AddSplitNodeToEdge (eptr-EdgeList);
        if (istat == -1) {
            return -1;
        }
        eptr->tflag2 = EDGE_USED_FOR_RIDGE;
        if (NodeList[eptr->node1].z > NodeList[eptr->node2].z) {
            if (NodeList[eptr->node1].adjusting_node != RIDGES_ALREADY_TRACED) {
                NodeList[eptr->node1].adjusting_node = RIDGE_START_NODE;
            }
        }
        else {
            if (NodeList[eptr->node2].adjusting_node != RIDGES_ALREADY_TRACED) {
                NodeList[eptr->node2].adjusting_node = RIDGE_START_NODE;
            }
        }

        return 1;
    }

    AddNodeToRidgeLine (rptr, eptr->number);
    nextnode = eptr->number;

/*
 *  Keep going updip until a high point or a used edge or a node
 *  intersection with the updip vector is found.
 */
    for (;;) {
        tptr = TriangleList + eptr->tri1;
        tn1 = eptr->tri1;
        if (tptr->flag == TRIANGLE_USED_FOR_RIDGE) {
            if (eptr->tri2 == -1) break;
            tptr = TriangleList + eptr->tri2;
            tn1 = eptr->tri2;
        }
        if (tptr->flag == TRIANGLE_USED_FOR_RIDGE) {
            break;
        }
        nextedge = FindRidgeExit (tptr, nextnode, eptr);
        tptr = TriangleList + tn1;
        if (nextedge == -1) {
            if (eptr->tflag2 == EDGE_ON_RIDGE) {
                if (NodeList[eptr->node1].z > NodeList[eptr->node2].z) {
                    if (NodeList[eptr->node1].adjusting_node != RIDGES_ALREADY_TRACED) {
                        NodeList[eptr->node1].adjusting_node = RIDGE_START_NODE;
                    }
                }
                else {
                    if (NodeList[eptr->node2].adjusting_node != RIDGES_ALREADY_TRACED) {
                        NodeList[eptr->node2].adjusting_node = RIDGE_START_NODE;
                    }
                }
                istat = AddSplitNodeToEdge (eptr-EdgeList);
                if (istat == -1) {
                    return -1;
                }
            }
            break;
        }
        tptr->flag = TRIANGLE_USED_FOR_RIDGE;
        eptr = EdgeList + nextedge;

        grazeflag = CrossesOnEndPoint (eptr);
        nextnode = eptr->number;
        AddNodeToRidgeLine (rptr, nextnode);
        if (grazeflag) {
            if (NodeList[nextnode].adjusting_node != RIDGES_ALREADY_TRACED) {
                NodeList[nextnode].adjusting_node = RIDGE_START_NODE;
            }
            break;
        }

        if (eptr->tflag2 == EDGE_USED_FOR_RIDGE  ||
            eptr->tflag2 == EDGE_ON_RIDGE) {
            istat = AddSplitNodeToEdge (nextedge);
            if (istat == -1) {
                return -1;
            }
            eptr->tflag2 = EDGE_USED_FOR_RIDGE;
            if (eptr->flag == BOUNDARY_EDGE) {
                NodeList[nextnode].bflag = 1;
            }
            if (NodeList[eptr->node1].z > NodeList[eptr->node2].z) {
                if (NodeList[eptr->node1].adjusting_node != RIDGES_ALREADY_TRACED) {
                    NodeList[eptr->node1].adjusting_node = RIDGE_START_NODE;
                }
            }
            else {
                if (NodeList[eptr->node2].adjusting_node != RIDGES_ALREADY_TRACED) {
                    NodeList[eptr->node2].adjusting_node = RIDGE_START_NODE;
                }
            }
            break;
        }
    }

    return 1;

}  /* end of private TraceRidgeUpDip function */





/*
 ****************************************************************************

                    C r o s s e s O n E n d P o i n t

 ****************************************************************************

  Return 1 if the number node of the edge is essentially coincident with
  either end point.  Also change the number node to the grazing end point
  node.

*/

int CSWGrdTriangle::CrossesOnEndPoint (EDgeStruct *eptr)
{
    int               n1, n2, nt, istat;
    double            xt, yt, gsav;

    n1 = eptr->node1;
    n2 = eptr->node2;
    nt = eptr->number;

    xt = NodeList[nt].x;
    yt = NodeList[nt].y;

    gsav = GrazeDistance;
/*    GrazeDistance = eptr->length / 10.0; */
    GrazeDistance = 0.0;

    istat = SamePointAsNode (xt, yt, n1);
    if (istat == 1) {
        GrazeDistance = gsav;
        eptr->number = n1;
        return 1;
    }

    istat = SamePointAsNode (xt, yt, n2);
    GrazeDistance = gsav;
    if (istat == 1) {
        eptr->number = n2;
        return 1;
    }

    return 0;

} /* end of private CrossesOnEndPoint function */





/*
 ********************************************************************************

                           S p l i t R i d g e

 ********************************************************************************

  Split a ridge into two segments, each having the same number of nodes (give
  or take one).  If the ridge has less than 4 original nodes, mark it as
  deleted.

*/

int CSWGrdTriangle::SplitRidge (RIdgeLineStruct *rptr)
{
    RIdgeLineStruct     *rp2;
    int                 n, nlist, *list, i, istat;

    list = rptr->nodes;
    nlist = rptr->nlist;
    if (nlist < 4) {
        rptr->deleted = 1;
        return 1;
    }

    n = nlist / 2;
    rptr->nlist = n;

    rp2 = NextRidgeLine ();
    if (rp2 == NULL) {
        return -1;
    }

    for (i=n-1; i<nlist; i++) {
        istat = AddNodeToRidgeLine (rp2, list[i]);
        if (istat == 0) {
            return -1;
        }
    }

    return 1;

}  /* end of private SplitRidge function */




/*
 **************************************************************************************

                        S p l i t R i d g e A t N o d e

 **************************************************************************************

  Split the ridge that previously had the specified into two ridges.  The connecting
  point of the two ridges is the specified node.

*/

int CSWGrdTriangle::SplitRidgeAtNode (int nodenum)
{
    NOdeStruct        *nptr, *np;
    RIdgeLineStruct   *rptr, *rp2;
    int               i, n1, istat, *list, nlist;

    nptr = NodeList + nodenum;
    if (nptr->crp < 0) {
        return 1;
    }
    rptr = RidgeLineList + nptr->crp;
    list = rptr->nodes;
    nlist = rptr->nlist;

    n1 = -1;
    for (i=0; i<nlist; i++) {
        if (list[i] == nodenum) {
            n1 = i;
            break;
        }
    }

    if (n1 < 1  ||  n1 == nlist-1) {
        return 1;
    }

    rptr->nlist = n1 + 1;

    rp2 = NextRidgeLine ();
    if (rp2 == NULL) {
        return -1;
    }

    for (i=n1; i<nlist; i++) {
        np = NodeList + list[i];
        np->crp = -1;
        istat = AddNodeToRidgeLine (rp2, list[i]);
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}  /* end of private SplitRidgeAtNode function */



/*
 *************************************************************************************

                         S p l i t R i d g e A t E d g e

 *************************************************************************************

  If the specified edge belongs to a ridge and has a list of crossing nodes, split
  the edge into small ridge lines connecting the crossing nodes and their ridges.

*/

int CSWGrdTriangle::SplitRidgeAtEdge (int edgenum)
{
    EDgeStruct           *eptr = NULL;
    CRossNodeStruct      *cptr = NULL;
    RIdgeLineStruct      *rptr = NULL, *rp = NULL;
    int                  n1, n2, i, istat,
                         *list = NULL, nlist, *iptr = NULL;
    double               *dlist = NULL;
    void                 **vlist = NULL;

    auto fscope = [&]()
    {
        csw_Free (vlist);
        csw_Free (dlist);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Check if the edge has a ridge and crossing nodes.
*/
    eptr = EdgeList + edgenum;
    if (eptr->client_data == NULL) {
        return 1;
    }
    cptr = (CRossNodeStruct *)eptr->client_data;
    if (cptr->list == NULL  ||  cptr->nlist < 1  ||  cptr->ridgenum < 0) {
        return 1;
    }

    list = cptr->list;
    nlist = cptr->nlist;

/*
    Sort the crossing nodes in distance order from the
    edge node1.
*/
    vlist = (void**)csw_Malloc (nlist * sizeof(void*));
    if (vlist == NULL) {
        return -1;
    }
    dlist = (double *)csw_Malloc (nlist * sizeof(double));
    if (dlist == NULL) {
        return -1;
    }

    n1 = eptr->node1;
    for (i=0; i<nlist; i++) {
        n2 = list[i];
        dlist[i] = NodeDistance (n1, n2);
        vlist[i] = (void *)(list + i);
    }

    if (nlist > 1) {
        csw_HeapSortDouble2 (dlist, vlist, nlist);
    }

/*
    Create ridge lines between the sorted nodes.
*/
    n1 = eptr->node1;
    for (i=0; i<nlist; i++) {
        iptr = (int *)(vlist[i]);
        n2 = *iptr;
        rptr = NextRidgeLine ();
        if (rptr == NULL) {
            return -1;
        }
        istat = AddNodeToRidgeLine (rptr, n1);
        if (istat == -1) {
            return -1;
        }
        istat = AddNodeToRidgeLine (rptr, n2);
        if (istat == -1) {
            return -1;
        }
        n1 = n2;
    }

    n2 = eptr->node2;
    rptr = NextRidgeLine ();
    if (rptr == NULL) {
        return -1;
    }
    istat = AddNodeToRidgeLine (rptr, n1);
    if (istat == -1) {
        return -1;
    }
    istat = AddNodeToRidgeLine (rptr, n2);
    if (istat == -1) {
        return -1;
    }

/*
    All ridge segments using original triangle edges have exactly one
    edge in them.  The original ridge segment using the edge that has
    been split must be deleted and the new split segments used instead.
*/
    if (cptr->ridgenum >= 0) {
        rp = RidgeLineList + cptr->ridgenum;
        rp->deleted = 1;
    }

    return 1;

}  /* end of private SplitRidgeAtEdge function */





/*
 *************************************************************************************

                        A d d S p l i t N o d e T o E d g e

 *************************************************************************************

  Add the eptr->number node to the list of nodes that terminate ridges on this edge.
  The edge may not have actually been traversed itself as part of a ridge line when
  this function is called, so the edges ridge number may not be available.  Because
  of this, all this function does is add the node to the list of nodes falling upon
  the edge.  After all the ridges are done, the edges with additional nodes on them
  are broken up into smaller edges.

*/

int CSWGrdTriangle::AddSplitNodeToEdge (int edgenum)
{
    EDgeStruct        *eptr;
    CRossNodeStruct   *cptr;
    RIdgeLineStruct   *rptr;
    int               *list, nlist, maxlist, istat;

    eptr = EdgeList + edgenum;
    if (eptr->client_data == NULL) {
        cptr = (CRossNodeStruct *)csw_Calloc (sizeof(CRossNodeStruct));
        if (!cptr) {
            return -1;
        }
        eptr->client_data = (void *)cptr;
        cptr->ridgenum = -1;
    }
    else {
        cptr = (CRossNodeStruct *)eptr->client_data;
    }

    if (cptr->ridgenum == -1) {
        rptr = NextRidgeLine ();
        if (rptr == NULL) {
            return -1;
        }
        istat = AddNodeToRidgeLine (rptr, eptr->node1);
        if (istat == -1) {
            return -1;
        }
        istat = AddNodeToRidgeLine (rptr, eptr->node2);
        if (istat == -1) {
            return -1;
        }
        cptr->ridgenum = rptr - RidgeLineList;
        if (NodeList[eptr->node1].z > NodeList[eptr->node2].z) {
            if (NodeList[eptr->node1].adjusting_node != RIDGES_ALREADY_TRACED) {
                NodeList[eptr->node1].adjusting_node = RIDGE_START_NODE;
            }
        }
        else {
            if (NodeList[eptr->node2].adjusting_node != RIDGES_ALREADY_TRACED) {
                NodeList[eptr->node2].adjusting_node = RIDGE_START_NODE;
            }
        }
    }

    list = cptr->list;
    nlist = cptr->nlist;
    maxlist = cptr->maxlist;

    if (nlist >= maxlist) {
        maxlist += 2;
        list = (int *)csw_Realloc (list, maxlist * sizeof(int));
        if (list == NULL) {
            return -1;
        }
    }

    list[nlist] = eptr->number;
    nlist++;
    cptr->list = list;
    cptr->maxlist = maxlist;
    cptr->nlist = nlist;

    return 1;

}  /* end of private AddSplitNodeToEdge function */





/*
 **************************************************************************************

                        A s s i g n R i d g e T o E d g e

 **************************************************************************************

  The specified ridge number is associated with the specified triangle edge.  This
  is done via a CRossNodeStruct allocation that is used as the edge's client data
  pointer.  When the edge is csw_Freed, the FreeRidgeFromEdge function is called to
  clean up the CRossNodeStruct.

*/

int CSWGrdTriangle::AssignRidgeToEdge (int edgenum, int ridgenum)
{
    EDgeStruct        *eptr;
    CRossNodeStruct   *cptr;

    eptr = EdgeList + edgenum;
    if (eptr->client_data == NULL) {
        cptr = (CRossNodeStruct *)csw_Calloc (sizeof(CRossNodeStruct));
        if (!cptr) {
            return -1;
        }
        eptr->client_data = (void *)cptr;
    }
    else {
        cptr = (CRossNodeStruct *)eptr->client_data;
    }
    cptr->ridgenum = ridgenum;

    return 1;

}  /* end of private AssignRidgeToEdge function */



/*
 *****************************************************************************************

                       F r e e R i d g e F r o m E d g e

 *****************************************************************************************

  Clean up the CRossNodeStruct associated with the edge.

*/

void CSWGrdTriangle::FreeRidgeFromEdge (void *vptr)
{
    CRossNodeStruct    *cptr;

    if (vptr == NULL) {
        return;
    }

    cptr = (CRossNodeStruct *)vptr;

    if (cptr->list) csw_Free (cptr->list);
    csw_Free (cptr);

    return;

}  /* end of private FreeRidgeFromEdge function */





/*
 *************************************************************************************

                    A d d P o l y g o n T o S p i l l p o i n t

 *************************************************************************************

*/

int CSWGrdTriangle::AddPolygonToSpillpoint (int nodenum, int polynum)
{
    SPillpointStruct     *sptr;
    NOdeStruct           *nptr;
    int                  i, *list, nlist, maxlist;

    nptr = NodeList + nodenum;
    if (nptr->spillnum == -1) {
        return 1;
    }

    sptr = SpillpointList + nptr->spillnum;
    list = sptr->polygons_using;
    nlist = sptr->num_polygons_using;
    maxlist = sptr->max_polygons_using;

    if (nlist >= maxlist) {
        maxlist += 2;
        list = (int *)csw_Realloc (list, maxlist * sizeof(int));
        if (list == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

    sptr->polygons_using = list;
    sptr->max_polygons_using = maxlist;

    for (i=0; i<nlist; i++) {
        if (list[i] == polynum) {
            return 1;
        }
    }

    list[nlist] = polynum;
    nlist++;

    sptr->polygons_using = list;
    sptr->num_polygons_using = nlist;
    sptr->max_polygons_using = maxlist;

    return 1;

}  /* end of private AddPolygonToSpillpoint function */




/*
 ************************************************************************************

                   C h e c k F o r E d g e M i n i m u m

 ************************************************************************************

*/

int CSWGrdTriangle::CheckForEdgeMinimum (int nodenum)
{
    NOdeEdgeListStruct     *nptr;
    EDgeStruct             *eptr;
    int                    *list, nlist, i, n1, n2;
    double                 zchk;

    nptr = NodeEdgeList + nodenum;
    if (nptr->done) return 0;

    list = nptr->list;
    nlist = nptr->nlist;
    n1 = -1;
    n2 = -1;

    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        if (eptr->flag != BOUNDARY_EDGE) {
            continue;
        }
        if (n1 == -1) {
            if (eptr->node1 != nodenum) {
                n1 = eptr->node1;
            }
            else {
                n1 = eptr->node2;
            }
        }
        else {
            if (eptr->node1 != nodenum) {
                n2 = eptr->node1;
            }
            else {
                n2 = eptr->node2;
            }
        }
    }

    if (n1 == -1  ||  n2 == -1) {
        return 0;
    }

    zchk = NodeList[nodenum].z;
    if (NodeList[n1].z >= zchk  &&
        NodeList[n2].z >= zchk) {
        return 1;
    }

    return 0;

}  /* end of private CheckForEdgeMinimum function */





/*
 ****************************************************************************************

                 I n t e r p o l a t e C o n s t r a i n t Z v a l u e

 ****************************************************************************************

  Use the ConstraintX1, Y1, X2, Y2, Z1, Z2 values to interpolate a z value at x, y

*/

double CSWGrdTriangle::InterpolateConstraintZvalue (double x, double y)
{
    double               zt, pct, adx, ady;

    adx = ConstraintX2 - ConstraintX1;
    if (adx < 0.0) adx = -adx;
    ady = ConstraintY2 - ConstraintY1;
    if (ady < 0.0) ady = -ady;
    if (adx == 0.0  &&  ady == 0.0) return 1.e30;

    if (adx > ady) {
        pct = (x - ConstraintX1) / (ConstraintX2 - ConstraintX1);
        zt = ConstraintZ1 + pct * (ConstraintZ2 - ConstraintZ1);
    }
    else {
        pct = (y - ConstraintY1) / (ConstraintY2 - ConstraintY1);
        zt = ConstraintZ1 + pct * (ConstraintZ2 - ConstraintZ1);
    }

/*
 * bug 9157
 *  Make sure the result is null if either end point is null
 */
    if (ConstraintZ1 >= NullValue  ||  ConstraintZ2 >= NullValue  ||
        ConstraintZ1 <= -1.e15  ||  ConstraintZ2 <= -1.e15) {
        zt = 1.e30;
    }

    return zt;

}  /* end of private InterpolateConstraintZvalue function */




/*
 ******************************************************************************************

           A d d B o u n d a r y L o w s T o S p i l l p o i n t L i s t

 ******************************************************************************************

  Find the lowest node on each boundary ridge and flag it as a spill point if not
  already flagged as a spill point.

*/

int CSWGrdTriangle::AddBoundaryLowsToSpillpointList (void)
{
    int                i, j, n, *nodes, nlist, jmin;
    double             zmin;
    NOdeStruct         *np1, *np2;
    RIdgeLineStruct    *rptr;

/*
    First, count how many more spill points will be needed.
*/
    n = 0;
    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->deleted) {
            continue;
        }
        nodes = rptr->nodes;
        nlist = rptr->nlist;
        if (nodes == NULL  ||  nlist < 2) continue;
        if (rptr->bflag == 0) continue;
        np1 = NodeList + nodes[0];
        np2 = NodeList + nodes[1];
        if (np1 == np2) continue;
        jmin = -1;
        zmin = 1.e30;
        for (j=0; j<nlist; j++) {
            np1 = NodeList + nodes[j];
            if (np1->spillnum >= 0) {
                jmin = -1;
                break;
            }
            if (np1->z < zmin) {
                zmin = np1->z;
                jmin = nodes[j];
            }
        }
        if (jmin >= 0) {
            if (NodeList[jmin].spillnum == -1) {
                n++;
            }
        }
    }

/*
    grow the list to be big enough
*/
    n += NumSpillpoints;
    n++;

    SpillpointList = (SPillpointStruct *)csw_Realloc
                     (SpillpointList, n * sizeof(SPillpointStruct));
    if (SpillpointList == NULL) {
        NumSpillpoints = 0;
        return -1;
    }

/*
    Add the new spillpoints to the list
*/
    n = NumSpillpoints;
    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->deleted) {
            continue;
        }
        if (rptr->bflag == 0) {
            continue;
        }
        nodes = rptr->nodes;
        nlist = rptr->nlist;
        if (nodes == NULL  ||  nlist < 2) {
            continue;
        }
        np1 = NodeList + nodes[0];
        np2 = NodeList + nodes[1];
        if (np1 == np2) continue;
        jmin = -1;
        zmin = 1.e30;
        for (j=0; j<nlist; j++) {
            np1 = NodeList + nodes[j];
            if (np1->spillnum >= 0) {
                jmin = -1;
                break;
            }
            if (np1->z < zmin) {
                zmin = np1->z;
                jmin = nodes[j];
            }
        }
        if (jmin >= 0) {
            if (NodeList[jmin].spillnum == -1) {
                np1 = NodeList + jmin;
                SpillpointList[n].polygons_using = NULL;
                SpillpointList[n].num_polygons_using = 0;
                SpillpointList[n].max_polygons_using = 0;
                SpillpointList[n].x = np1->x;
                SpillpointList[n].y = np1->y;
                SpillpointList[n].z = np1->z;
                np1->spillnum = n;
                n++;
            }
        }
    }

    NumSpillpoints = n;

    return 1;

}  /* end of private AddBoundaryLowsToSpillpointList */



/*
 ***************************************************************************************

                     D e l e t e I d e n t i c a l R i d g e s

 ***************************************************************************************

*/

void CSWGrdTriangle::DeleteIdenticalRidges (void)
{
    int                 i, j, istat;
    RIdgeLineStruct     *rp1, *rp2;

    for (i=0; i<NumRidgeLines; i++) {
        rp1 = RidgeLineList + i;
        if (rp1->deleted) continue;
        for (j=i+1; j<NumRidgeLines; j++) {
            rp2 = RidgeLineList + j;
            if (rp2->deleted) continue;
            istat = CheckForIdenticalRidges (rp1, rp2);
            if (istat == 1) {
                rp2->deleted = 1;
            }
        }
    }

    return;

}  /* end of private DeleteIdenticalRidges function */





/*
 ***************************************************************************************

                   C h e c k F o r I d e n t i c a l R i d g e s

 ***************************************************************************************

*/

int CSWGrdTriangle::CheckForIdenticalRidges (RIdgeLineStruct *rp1, RIdgeLineStruct *rp2)
{
    int              istat, i, j, nlist, *list1, *list2;

    if (rp1->nlist != rp2->nlist) {
        return 0;
    }
    nlist = rp1->nlist;
    list1 = rp1->nodes;
    list2 = rp2->nodes;

    istat = 1;
    for (i=0; i<nlist; i++) {
        if (list1[i] != list2[i]) {
            istat = 0;
            break;
        }
    }

    if (istat == 1) {
        return 1;
    }

    for (i=0; i<nlist; i++) {
        j = nlist - i - 1;
        if (list1[i] != list2[j]) {
            return 0;
        }
    }

    return 1;

}  /* end of private CheckForIdenticalRidges function */




/*
 ******************************************************************************************

             C h e c k P o l y g o n F o r S p i l l p o i n t

 ******************************************************************************************

  Return 1 if at least one node on the ridge lines makig up the current polygon is a
  spill point.  Return zero if not.

*/

int CSWGrdTriangle::CheckPolygonForSpillpoint (int nseg)
{
    int                   i, j, *list, nlist;
    NOdeStruct            *nptr;
    RIdgeLineStruct       *rptr;

    for (i=0; i<nseg; i++) {
        rptr = RidgeLineList + PolySegs[i];
        list = rptr->nodes;
        nlist = rptr->nlist;
        for (j=0; j<nlist; j++) {
            nptr = NodeList + list[j];
            if (nptr->spillnum >= 0) {
                return 1;
            }
        }
    }

    return 0;

}  /* end of private CheckPolygonForSpillpoint function */




/*
 ****************************************************************************************************

                                  F i n d D o w n E x i t

 ****************************************************************************************************

*/

int CSWGrdTriangle::FindDownExit (TRiangleStruct *tptr,
                         int nodenum)
{
    int                en1, nt;
    double             zc, z1, z2;
    EDgeStruct         *eptr;

/*
    en1 is the edge opposite of the specified node
    in the specified triangle.  If either end point of
    this edge is lower than the center z, output the edge
    as a downhill, with its lowest endpoint used as the number
    member of the edge.
*/
    en1 = OppositeEdge (tptr, nodenum);

    zc = NodeList[nodenum].z;

    eptr = EdgeList + en1;
    eptr->number = -1;
    nt = eptr->node1;
    z1 = NodeList[nt].z;
    if (z1 < zc) {
        zc = z1;
        eptr->number = nt;
    }
    nt = eptr->node2;
    z2 = NodeList[nt].z;
    if (z2 < zc) {
        eptr->number = nt;
    }

    if (eptr->number == -1) {
        return -1;
    }

    return en1;

}  /* end of private FindDownExit function */





/*
 ****************************************************************************************

                          C o m m o n E d g e

 ****************************************************************************************

  Return the edge number that is shared by the two specified triangles.  If
  no edge is shared, return -1.

*/

int CSWGrdTriangle::CommonEdge (int tn1, int tn2)
{
    TRiangleStruct       *tp1, *tp2;
    int                  e1, e2, e3;

    tp1 = TriangleList + tn1;
    tp2 = TriangleList + tn2;
    e1 = tp1->edge1;
    e2 = tp1->edge2;
    e3 = tp1->edge3;

    if (tp2->edge1 == e1  ||
        tp2->edge2 == e1  ||
        tp2->edge3 == e1) {
        return e1;
    }

    if (tp2->edge1 == e2  ||
        tp2->edge2 == e2  ||
        tp2->edge3 == e2) {
        return e2;
    }

    if (tp2->edge1 == e3  ||
        tp2->edge2 == e3  ||
        tp2->edge3 == e3) {
        return e3;
    }

    return -1;

}  /* end of private CommonEdge function */




/*
 **********************************************************************************

                                C o r n e r N o d e

 **********************************************************************************

  Return the node that is on the common edge between the two triangles but is not
  at the specified node number.

*/

int CSWGrdTriangle::CornerNode (TRiangleStruct *tp1,
                       TRiangleStruct *tp2,
                       int nodenum)
{
    int                  en1, en2, n;

    en1 = OppositeEdge (tp1, nodenum);
    en2 = OppositeEdge (tp2, nodenum);
    n = CommonNode (en1, en2);

    return n;

}  /* end of private CornerNode function */




/*
 *******************************************************************************************

                    O r d e r T r i a n g l e s A r o u n d N o d e

 *******************************************************************************************

  Return a list of triangles that but up against each other around the node.

*/

int CSWGrdTriangle::OrderTrianglesAroundNode (int nodenum,
                                     int *ordered_list,
                                     int *num_ordered,
                                     int *edgelist,
                                     int nedge)
{
    int               i, tri0, tn1, *list, nlist, bflag, n, e1;
    EDgeStruct        *eptr;

    list = edgelist;
    nlist = nedge;

    bflag = 0;
    eptr = NULL;
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        if (eptr->tri2 == -1) {
            bflag = 1;
            break;
        }
    }

    if (bflag == 1  &&  eptr != NULL) {
        tn1 = eptr->tri1;
        ordered_list[0] = tn1;
        n = 1;
        for (;;) {
            e1 = AdjacentEdge (TriangleList + tn1,
                               eptr - EdgeList,
                               nodenum);
            eptr = EdgeList + e1;
            tn1 = eptr->tri1;
            if (tn1 == ordered_list[n-1]) {
                tn1 = eptr->tri2;
            }
            if (tn1 == -1) {
                break;
            }
            ordered_list[n] = tn1;
            n++;
        }
        *num_ordered = n;
    }

    else {
        eptr = EdgeList + list[0];
        tn1 = eptr->tri1;
        ordered_list[0] = tn1;
        tri0 = tn1;
        n = 1;
        for (;;) {
            e1 = AdjacentEdge (TriangleList + tn1,
                               eptr - EdgeList,
                               nodenum);
            eptr = EdgeList + e1;
            tn1 = eptr->tri1;
            if (tn1 == ordered_list[n-1]) {
                tn1 = eptr->tri2;
            }
            if (tn1 == tri0) {
                break;
            }
            ordered_list[n] = tn1;
            n++;
        }
        *num_ordered = n;
    }

    return 1;

}  /* end of private OrderTrianglesAroundNode function */




/*
 ************************************************************************************

                 S a v e F o r k i n g R i d g e I n f o

 ************************************************************************************

  Save the data needed to choose forking ridges in a FOrkInfoStruct record.  After
  all ridges are done and hanging ridges eliminated, these will be processed.

*/

int CSWGrdTriangle::SaveForkingRidgeInfo (int *trilist, int *ridgelist, int nlist,
                                 int nodenum, int *edgelist, int nedge)
{
    FOrkInfoStruct        *fptr;

    if (NumFork >= MaxFork) {
        MaxFork += 10;
        ForkList = (FOrkInfoStruct *)csw_Realloc (ForkList,
                                                  MaxFork * sizeof(FOrkInfoStruct));
        if (ForkList == NULL) {
            return -1;
        }
    }

    fptr = ForkList + NumFork;
    memcpy (fptr->trilist, trilist, nlist * sizeof(int));
    memcpy (fptr->ridgelist, ridgelist, nlist * sizeof(int));
    memcpy (fptr->edgelist, edgelist, nedge * sizeof(int));
    fptr->nodenum = nodenum;
    fptr->nlist = nlist;
    fptr->nedge = nedge;

    NumFork++;

    return 1;

}  /* end of private SaveForkingRidgeInfo function */




/*
 ************************************************************************************

                   C h o o s e F o r k i n g R i d g e s

 ************************************************************************************

  When more than one ridge emanates from a node, it may be necessary to delete one
  because its drainage area really is a subset of one of the other ridges from the
  node.  If ridge lines are separated by nodes lower than the center elevation,
  both ridges are always preserved.  If the steepest downhill direction from the
  central node is connected to a ridge without going through a "natural" ridge
  edge, then the ridge is deleted.  This is because there is no barrier between
  the ridge and the downhill direction taken when fluid funnels into and out of the
  central node.

*/

int CSWGrdTriangle::ChooseForkingRidges (int *trilist, int *ridgelist,
                                int nlist, int nodenum,
                                int *edgelist, int nedge)
{
    int              i, j, k, n, n2, nlist1, tlist1[20], rlist[20],
                     imin, exitedge, emin, ibad, tn1, tn2,
                     jmin, tlist2[20], rlist2[20];
    double           zcenter, znode, zmin, zt;
    TRiangleStruct   *tptr, *tp1, *tp2;
    EDgeStruct       *eptr;
    RIdgeLineStruct  *rptr;

/*
    Sort the triangles attached to the node in connective order either
    clockwise or ccw.
*/
    OrderTrianglesAroundNode (nodenum,
                              tlist1,
                              &nlist1,
                              edgelist,
                              nedge);

/*
    Associate the input ridge list with the ordered triangles.
*/
    n = 0;
    for (i=0; i<nlist1; i++) {
        rlist[i] = -1;
        tptr = TriangleList + tlist1[i];
        for (j=0; j<nlist; j++) {
            if (trilist[j] == tlist1[i]) {
                rptr = RidgeLineList + ridgelist[j];
                if (rptr->deleted == 0) {
                    rlist[i] = ridgelist[j];
                    tptr->deleted = 1;
                    n++;
                }
                break;
            }
        }
    }

/*
    If all but one of the original forking ridges has been deleted,
    do not delete the remaining ridge.
*/
    if (n < 2) {
        for (i=0; i<nlist1; i++) {
            TriangleList[tlist1[i]].deleted = 0;
        }
        return 1;
    }

/*
    Find the steepest downhill slope from the center node
*/
    emin = -1;
    imin = -1;
    zmin = 1.e30;
    for (i=0; i<nlist1; i++) {
        tptr = TriangleList + tlist1[i];
        if (tptr->deleted == 0) {
            exitedge = FindDownExit (tptr, nodenum);
            if (exitedge != -1) {
                eptr = EdgeList + exitedge;
                n = eptr->number;
                eptr->number = -1;
                if (n >= 0) {
                    zt = NodeList[n].z;
                    if (zt < zmin) {
                        zmin = zt;
                        imin = i;
                        emin = exitedge;
                    }
                }
            }
        }
    }

    if (imin == -1) {
        for (k=0; k<nlist1; k++) {
            TriangleList[tlist1[k]].deleted = 0;
        }
        return 1;
    }

    tptr = TriangleList + tlist1[imin];
    tptr->deleted = 3;

/*
    These two lines are to suppress some lint info messages,
    otherwise they are useless.
*/
    memset (tlist2, 0, 20 * sizeof(int));
    memset (rlist2, 0, 20 * sizeof(int));

/*
    Shift the ordered list so the steepest downhill is first
*/
    n = 0;
    for (i=imin; i<nlist1; i++) {
        tlist2[n] = tlist1[i];
        rlist2[n] = rlist[i];
        n++;
    }
    for (i=0; i<imin; i++) {
        tlist2[n] = tlist1[i];
        rlist2[n] = rlist[i];
        n++;
    }

    memcpy (tlist1, tlist2, n * sizeof(int));
    memcpy (rlist, rlist2, n * sizeof(int));

/*
    Check for lower spots separating ridge highs.  Since the steepest downhill is the first
    triangle, then any lower spots will be from saddles not associated with the steepest
    downhill.
*/
    zcenter = NodeList[nodenum].z;
    for (i=0; i<nlist1; i++) {
        tptr = TriangleList + tlist1[i];
        if (tptr->deleted == 1) {
            znode = 1.e30;
            tp1 = tptr;
            for (j=i+1; j<nlist1; j++) {
                tp2 = TriangleList + tlist1[j];
                if (tp2->deleted == 0) {
                    n2 = CornerNode (tp1, tp2, nodenum);
                    if (NodeList[n2].z < znode) {
                        znode = NodeList[n2].z;
                    }
                }
                else {
                    if (znode <= zcenter) {
                        tptr->deleted = 2;
                        tp2->deleted = 2;
                    }
                    break;
                }
                tp1 = tp2;
            }
        }
    }

/*
    If no questionable nodes remain, return.
*/
    n = 0;
    for (i=0; i<nlist1; i++) {
        tptr = TriangleList + tlist1[i];
        if (tptr->deleted == 1) {
            if (n == 0) imin = i;
            n++;
        }
    }

    if (n == 0) {
        for (k=0; k<nlist1; k++) {
            TriangleList[tlist1[k]].deleted = 0;
        }
        return 1;
    }

/*
    Only gets to here if there are one or two triangles left to be considered.
*/
    tn1 = -1;
    tn2 = -1;

    for (i=0; i<nlist1; i++) {
        tptr = TriangleList + tlist1[i];
        if (tptr->deleted == 1) {
            if (tn1 == -1) {
                tn1 = i;
            }
            else {
                tn2 = i;
                break;
            }
        }
    }

/*
    If a ridge edge does not exist between the steepest
    downhill and the ridge being checked, then the ridge being
    checked is deleted.
*/
    jmin = -1;
    n = 0;
    for (i=1; i<nlist1; i++) {
        emin = CommonEdge (tlist1[i-1], tlist1[i]);
        eptr = EdgeList + emin;
        if (eptr->tflag2 == EDGE_USED_FOR_RIDGE) {
            n = 1;
        }
        tptr = TriangleList + tlist1[i];
        if (tptr->deleted == 2) {
            n = 1;
        }
        if (tptr->deleted == 1) {
            jmin = i;
            break;
        }
    }

    if (jmin == -1) {
        assert (jmin >= 0);
    }

    if (n == 1) {
        if (jmin == tn1) {
            ibad = tn2;
        }
        else {
            ibad = tn1;
        }
    }
    else {
        if (jmin == tn1) {
            ibad = tn1;
        }
        else {
            ibad = tn2;
        }
    }

    if (ibad >= 0) {
        rptr = RidgeLineList + rlist[ibad];
        rptr->deleted = 1;
    }

    for (k=0; k<nlist1; k++) {
        TriangleList[tlist1[k]].deleted = 0;
    }

    return 1;

}  /* end of private ChooseForkingRidges function */



#if 0

/*
 **********************************************************************************

                    S e p a r a t e E q u a l A n g l e s

 **********************************************************************************

  Return, in the ar1 and ar2 parameters, angles associated with the two specified
  ridges that are not the same.  In other words, if the first segments of the ridges
  are overlaying, find a combination on the ridges that is not overlaying.  If no
  such combination can be found, zero is returned and the output angles are each
  set to 1.e30.  If a memory allocation error occurs, -1 is returned.  On success,
  1 is returned.

*/

int CSWGrdTriangle::SeparateEqualAngles (RIdgeLineStruct *ridge1,
                                RIdgeLineStruct *ridge2,
                                double *ar1,
                                double *ar2)
{
    double                 *xp1 = NULL, *yp1 = NULL,
                           *xp2 = NULL, *yp2 = NULL, ang1, ang2,
                           dx1, dy1, dx2, dy2;
    int                    np1, np2, i;


    auto fscope = [&]()
    {
        csw_Free (xp1);
        csw_Free (xp2);
    };
    CSWScopeGuard func_scope_guard (fscope);


    *ar1 = 1.e30;
    *ar2 = 1.e30;

    np1 = ridge1->nlist;
    xp1 = (double *)csw_Malloc (np1 * 2 * sizeof(double));
    if (xp1 == NULL) {
        return -1;
    }
    yp1 = xp1 + np1;

    np2 = ridge2->nlist;
    xp2 = (double *)csw_Malloc (np2 * 2 * sizeof(double));
    if (xp2 == NULL) {
        return -1;
    }
    yp2 = xp2 + np2;

    dx2 = xp2[1] - xp2[0];
    dy2 = yp2[1] - yp2[0];
    ang2 = atan2(dy2, dx2);

    for (i=2; i<np1; i++) {
        dx1 = xp1[i] - xp1[i-1];
        dy1 = yp1[i] - yp1[i-1];
        ang1 = atan2 (dy1, dx1);
        if (ang1 != ang2) {
            *ar1 = ang1;
            *ar2 = ang2;
            return 1;
        }
    }

    dx1 = xp1[1] - xp1[0];
    dy1 = yp1[1] - yp1[0];
    ang1 = atan2(dy1, dx1);

    for (i=2; i<np2; i++) {
        dx2 = xp2[i] - xp2[i-1];
        dy2 = yp2[i] - yp2[i-1];
        ang1 = atan2 (dy2, dx2);
        if (ang1 != ang2) {
            *ar1 = ang1;
            *ar2 = ang2;
            return 1;
        }
    }

    return -1;

}  /* end of private SeparateEqualAngles function */

#endif




/*
 ******************************************************************************

                   C o m b i n e B o u n d a r y R i d g e s

 ******************************************************************************

  Find non deleted ridges that are entirely on the boundary and connect them
  via there end points until an endpoint has no boundary ridge attached
  to it.

*/

int CSWGrdTriangle::CombineBoundaryRidges (void)
{
    int               i, j, nedge, nlist, *nodes, done, istat;
    RIdgeLineStruct   *rptr;
    NOdeStruct        *np1;

/*
    First, flag all ridges on the boundary.
*/
    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->deleted) {
            continue;
        }
        nodes = rptr->nodes;
        nlist = rptr->nlist;
        if (nodes == NULL  ||  nlist < 2) continue;
        nedge = 0;
        for (j=0; j<nlist; j++) {
            np1 = NodeList + nodes[j];
            if (np1->bflag != 1) {
                break;
            }
            nedge++;
        }
        if (nedge < nlist) {
            continue;
        }
        rptr->bflag = 1;
    }

/*
    Find a ridge on the boundary and connect it to other boundary ridges.
*/
    for (;;) {
        done = 0;
        for (i=0; i<NumRidgeLines; i++) {
            rptr = RidgeLineList + i;
            if (rptr->deleted) {
                continue;
            }
            istat = ExtendBoundaryRidge (rptr);
            if (istat == -1) {
                return -1;
            }
            if (istat == 1) {
                done++;
            }
        }
        if (done == 0) {
            break;
        }
    }

    return 1;

}  /* end of private CombineBoundaryRidges function */




/*
 ****************************************************************************************

                       E x t e n d B o u n d a r y R i d g e

 ****************************************************************************************

  Extend the specified ridge by adding the nodes connected to its last node and
  which are also on a boundary ridge.  If the ridge cannot be extended, zero is
  returned.  If a memory allocation error occurs, -1 is returned.  If the ridge
  is extended, 1 is returned.

*/

int CSWGrdTriangle::ExtendBoundaryRidge (RIdgeLineStruct *rptr)
{
    int                 i, j, ndone, done, istat,
                        n2, *nodes, nlist2, *list, nlist;
    NOdeStruct          *nptr;
    RIdgeLineStruct     *rp2;

    n2 = rptr->nodes[rptr->nlist-1];
    ndone = 0;

/*
    Try to append other ridges to the last node.
*/
    for (;;) {
        list = NodeEdgeList[n2].list;
        nlist = NodeEdgeList[n2].nlist;
        done = 0;
        if (nlist > 2) break;
        for (i=0; i<nlist; i++) {
            rp2 = RidgeLineList + list[i];
            if (rp2 == rptr  ||  rp2->deleted == 1) {
                continue;
            }

            nodes = rp2->nodes;
            nlist2 = rp2->nlist;
            if (nodes[0] == n2) {
                for (j=1; j<nlist2; j++) {
                    nptr = NodeList + nodes[j];
                    nptr->crp = -1;
                    istat = AddNodeToRidgeLine (rptr, nodes[j]);
                    if (istat == -1) {
                        return -1;
                    }
                }
                n2 = nodes[nlist2-1];
                rp2->deleted = 1;
                done++;
                break;
            }
            else if (nodes[nlist2-1] == n2) {
                for (j=nlist2-2; j>=0; j--) {
                    nptr = NodeList + nodes[j];
                    nptr->crp = -1;
                    istat = AddNodeToRidgeLine (rptr, nodes[j]);
                    if (istat == -1) {
                        return -1;
                    }
                }
                n2 = nodes[0];
                rp2->deleted = 1;
                done++;
                break;
            }
        }
        if (done == 0) {
            break;
        }
        ndone++;
    }

/*
    Try to prepend to the first node.
*/
    n2 = rptr->nodes[0];

    for (;;) {
        list = NodeEdgeList[n2].list;
        nlist = NodeEdgeList[n2].nlist;
        done = 0;
        if (nlist > 2) break;
        for (i=0; i<nlist; i++) {
            rp2 = RidgeLineList + list[i];
            if (rp2 == rptr  ||  rp2->deleted == 1) {
                continue;
            }

            nodes = rp2->nodes;
            nlist2 = rp2->nlist;
            if (nodes[0] == n2) {
                for (j=1; j<nlist2; j++) {
                    nptr = NodeList + nodes[j];
                    nptr->crp = -1;
                    istat = PrependNodeToRidgeLine (rptr, nodes[j]);
                    if (istat == -1) {
                        return -1;
                    }
                }
                n2 = nodes[nlist2-1];
                rp2->deleted = 1;
                done++;
                break;
            }
            else if (nodes[nlist2-1] == n2) {
                for (j=nlist2-2; j>=0; j--) {
                    nptr = NodeList + nodes[j];
                    nptr->crp = -1;
                    istat = PrependNodeToRidgeLine (rptr, nodes[j]);
                    if (istat == -1) {
                        return -1;
                    }
                }
                n2 = nodes[0];
                rp2->deleted = 1;
                done++;
                break;
            }
        }
        if (done == 0) {
            break;
        }
        ndone++;
    }

    if (ndone == 0) {
        return 0;
    }

    return 1;

}  /* end of private CombineBoundaryRidges function */




/*
 ************************************************************************************

                   P r e p e n d N o d e T o R i d g e L i n e

 ************************************************************************************

  Insert the specified node at the start of the ridge line.

*/

int CSWGrdTriangle::PrependNodeToRidgeLine (RIdgeLineStruct *rptr, int nodenum)
{
    int                    *nodes = NULL, *ntmp = NULL;
    int                    nlist, maxlist;
    NOdeStruct             *nptr = NULL;


    auto fscope = [&]()
    {
        csw_Free (ntmp);
    };
    CSWScopeGuard func_scope_guard (fscope);


    nodes = rptr->nodes;
    nlist = rptr->nlist;
    maxlist = rptr->maxlist;

    if (nlist >= maxlist) {
        maxlist += 20;
        nodes = (int *)csw_Realloc (nodes, maxlist * sizeof(int));
        if (nodes == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

    ntmp = (int *)csw_Malloc (maxlist * sizeof(int));
    if (ntmp == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    memcpy (ntmp, nodes, nlist * sizeof(int));

    nodes[0] = nodenum;
    memcpy (nodes+1, ntmp, nlist * sizeof(int));
    nlist++;

    rptr->nodes = nodes;
    rptr->nlist = nlist;
    rptr->maxlist = maxlist;

    nptr = NodeList + nodenum;

    nptr->flag = NODE_USED_FOR_RIDGE;
    if (nptr->crp == -1) {
        nptr->crp = rptr - RidgeLineList;
    }

    return 1;

}  /* end of private PrependNodeToRidgeLine function */




/*
 *************************************************************************************

                   C r e a t e B o u n d a r y R i d g e s

 *************************************************************************************

*/

int CSWGrdTriangle::CreateBoundaryRidges (void)
{
    int                  i, istat;
    EDgeStruct           *eptr;
    RIdgeLineStruct      *rptr;
    NOdeStruct           *nptr;

    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->flag != BOUNDARY_EDGE) {
            continue;
        }
        rptr = NextRidgeLine ();
        if (rptr == NULL) {
            return -1;
        }
        istat = AddNodeToRidgeLine (rptr, eptr->node1);
        if (istat == -1) {
            return -1;
        }
        istat = AddNodeToRidgeLine (rptr, eptr->node2);
        if (istat == -1) {
            return -1;
        }
        rptr->bflag = 1;
        eptr->tflag2 = EDGE_USED_FOR_RIDGE;
        istat = AssignRidgeToEdge (i, rptr-RidgeLineList);
        if (istat == -1) {
            return -1;
        }
        nptr = NodeList + eptr->node1;
        istat = CheckForEdgeMaximum (eptr->node1);
        if (istat == 1  &&  nptr->adjusting_node != RIDGES_ALREADY_TRACED) {
            nptr->adjusting_node = RIDGE_START_NODE;
        }
        nptr = NodeList + eptr->node2;
        istat = CheckForEdgeMaximum (eptr->node2);
        if (istat == 1  &&  nptr->adjusting_node != RIDGES_ALREADY_TRACED) {
            nptr->adjusting_node = RIDGE_START_NODE;
        }
    }

    return 1;

}  /* end of private CreateBoundaryRidges function */




/*
 **************************************************************************************

                    D e l e t e B o u n d a r y R i d g e s

 **************************************************************************************

*/

void CSWGrdTriangle::DeleteBoundaryRidges (void)
{
    int                i;
    RIdgeLineStruct    *rptr;

    for (i=0; i<NumRidgeLines; i++) {
        rptr = RidgeLineList + i;
        if (rptr->bflag == 1) {
            rptr->deleted = 1;
        }
    }

    return;

}  /* end of private DeleteBoundaryRidges function */


/*
 *****************************************************************************************

                         S e a r c h S p i l l F i l t e r s

 *****************************************************************************************

*/

int CSWGrdTriangle::SearchSpillFilters (double x, double y)
{
    int              i;
    double           dx, dy, dist;

    for (i=0; i<NumSpillFilters; i++) {
        if (SpillFilterList[i].max_polygons_using == SpillTypeForSearch) {
            dx = x - SpillFilterList[i].x;
            dy = y - SpillFilterList[i].y;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist < SpillFilterDistance) {
                return 1;
            }
        }
    }

    return 0;

}  /* end of private SearchSpillFilters function */



/*
 ***********************************************************************************

                     C h e c k G r i d I n t e r i o r S a d d l e

 ***********************************************************************************

*/

int CSWGrdTriangle::CheckGridInteriorSaddle (int col, int row)
{
    CSW_F           head_ne, head_sw, head_se, head_nw;
    CSW_F           head_n, head_s, head_e, head_w;
    CSW_F           *head, head_center;
    int             offset, test_offset, test_row, test_col;

    if (col < 1  ||  col >= NcolSaddle-1) {
        return 0;
    }
    if (row < 1  ||  row >= NrowSaddle-1) {
        return 0;
    }

    head = GridSaddle;
    offset = NcolSaddle * row;

    head_ne = head[offset + NcolSaddle + col + 1];
    head_nw = head[offset + NcolSaddle + col];
    head_se = head[offset + col + 1];
    head_sw = head[offset + col];

    if (head_ne > 1.e20  ||
        head_se > 1.e20  ||
        head_nw > 1.e20  ||
        head_sw > 1.e20) {
        return 0;
    }
/*
    NW-SE is high, NE-SW is low
*/
    if (
      head_ne < head_nw &&
      head_ne < head_se &&
      head_sw < head_nw &&
      head_sw < head_se
    ) {
        return 1;

/*
    NE-SW is high, NW-SE is low
*/
    } else if (
      head_ne > head_nw &&
      head_ne > head_se &&
      head_sw > head_nw &&
      head_sw > head_se
    ) {
        return 1;
    }

/*
    PLUS saddle point.
*/
    head_center = head[offset + col];
    head_n = head[offset + NcolSaddle + col];
    head_s = head[offset - NcolSaddle + col];
    head_e = head[offset + col + 1];
    head_w = head[offset + col - 1];

    if (head_center > 1.e20  ||
        head_n > 1.e20  ||
        head_s > 1.e20  ||
        head_e > 1.e20  ||
        head_w > 1.e20) {
        return 0;
    }

/*
 * Note that we use >= instead of >.  Required for this scenario:
 * ? 0 0 ?
 * 2 1 1 2
 * ? 0 0 ?
 */
    if (
        head_n >= head_center &&
        head_s >= head_center &&
        head_e <= head_center &&
        head_w <= head_center
    ) {

    /*
     * check to make sure at least one point around the center has
     * a lower head than the center
     */
        for (test_row = row - 1; test_row <= row + 1; test_row++) {
            test_offset = NcolSaddle * test_row;
            for (test_col = col - 1; test_col <= col + 1; test_col++) {
                if (head[test_offset+test_col] < head_center) {
                    return 1;
                }
            }
        }
    }

    if (
        head_n <= head_center &&
        head_s <= head_center &&
        head_e >= head_center &&
        head_w >= head_center
    ) {

    /*
     * check to make sure at least one point around the center has
     * a lower head than the center
     */
        for (test_row = row - 1; test_row <= row + 1; test_row++) {
            test_offset = NcolSaddle * test_row;
            for (test_col = col - 1; test_col <= col + 1; test_col++) {
                if (head[test_offset+test_col] < head_center) {
                      return 1;
                }
            }
        }
    }

    return 0;

}  /* end of private CheckGridInteriorSaddle function */





/*
 *******************************************************************************

                    C h e c k G r i d E d g e S a d d l e

 *******************************************************************************

*/

int CSWGrdTriangle::CheckGridEdgeSaddle (int col, int row)
{
    int         offset, n_cols, n_rows;
    CSW_F       head_center, *head;

    head = GridSaddle;

    offset = row * NcolSaddle;
    n_cols = NcolSaddle;
    n_rows = NrowSaddle;
    head_center = head[offset + col];

/*
    Search the north edge of the grid for minima
*/
    if (row == n_rows-1) {
        if (col < 1  ||  col >= n_cols-1) {
            return 0;
        }
        if (head[offset + col] > 1.e20  ||
            head[offset + col -1] > 1.e20  ||
            head[offset + col + 1] > 1.e20) {
            return 0;
        }
        if (
            head[offset+col+1] >= head_center &&
            head[offset+col-1] >= head_center &&
            head[offset-n_cols+col] < head_center) {
            return 1;
        }
        return 0;
    }

/*
    Search the south edge of the grid for minima
*/
    if (row == 0) {
        if (col < 1  ||  col >= n_cols-1) {
            return 0;
        }
        if (head[offset + col] > 1.e20  ||
            head[offset + col -1] > 1.e20  ||
            head[offset + col + 1] > 1.e20) {
            return 0;
        }
        if (
            head[offset+col+1] >= head_center &&
            head[offset+col-1] >= head_center &&
            head[offset+n_cols+col] < head_center) {
            return 1;
        }
        return 0;
    }

/*
    Search the east edge of the grid for minima
*/
    if (col == n_cols-1) {
        if (row < 1  ||  row >= n_rows-1) {
            return 0;
        }
        if (head[offset+col] > 1.e20  ||
            head[offset+n_cols+col] > 1.e20  ||
            head[offset-n_cols+col] > 1.e20) {
            return 0;
        }
        if (
            head[offset+n_cols+col] >= head_center &&
            head[offset-n_cols+col] >= head_center &&
            head[offset+col-1] < head_center) {
            return 1;
        }
        return 0;
    }

/*
    Search the west edge of the grid for minima
*/
    if (col == 0) {
        if (row < 1  ||  row >= n_rows-1) {
            return 0;
        }
        if (head[offset+col] > 1.e20  ||
            head[offset+n_cols+col] > 1.e20  ||
            head[offset-n_cols+col] > 1.e20) {
            return 0;
        }
        if (
            head[offset+n_cols+col] >= head_center &&
            head[offset-n_cols+col] >= head_center &&
            head[offset+col+1] < head_center) {
            return 1;
        }
        return 0;
    }

    return 0;

}  /* end of private CheckGridEdgeSaddle function */



/*
 *****************************************************************************

            g r d _ g e t _ n o d e s _ f o r _ t r i a n g l e

 *****************************************************************************

*/

int CSWGrdTriangle::grd_get_nodes_for_triangle (TRiangleStruct *triangle,
                                EDgeStruct *edgelist,
                                int *n1, int *n2, int *n3)
{
    EDgeStruct           *eptr;

    *n1 = -1;
    *n2 = -1;
    *n3 = -1;

    if (triangle == NULL  ||  edgelist == NULL) {
        return -1;
    }

    eptr = edgelist + triangle->edge1;

    *n1 = eptr->node1;
    *n2 = eptr->node2;
    eptr = edgelist + triangle->edge2;
    if (eptr->node1 == *n1  ||  eptr->node1 == *n2) {
        *n3 = eptr->node2;
    }
    else {
        *n3 = eptr->node1;
    }

    return 1;

}  /* end of function grd_get_nodes_for_triangle */



/*
 **************************************************************************************

             g r d _ b a c k _ i n t e r p o l a t e _ t r i _ m e s h

 **************************************************************************************

  Interpolate the z values at the specified x,y points by using the plane defined
  by the triangle in the trimesh that contains the x,y point.  If no triangle
  contains a particular x,y then its z is set to 1.e30.

*/

int CSWGrdTriangle::grd_back_interpolate_tri_mesh (TRiangleStruct *triangles, int ntriangles,
                                   EDgeStruct *edges, int nedges,
                                   NOdeStruct *nodes, int numnodes,
                                   double *x, double *y, double *z,
                                   int npts)
{
    int                       ido, i, j, istat;
    double                    xt, yt, zt, x1, y1, x2, y2,
                              xtri[4], ytri[4], ztri[4], coef[3];
    TRiangleStruct            *tsav;
    EDgeStruct                *esav;
    NOdeStruct                *nsav;

    CSWPolyUtils             ply_utils_obj;

    tsav = TriangleList;
    esav = EdgeList;
    nsav = NodeList;

    TriangleList = triangles;
    EdgeList = edges;
    NodeList = nodes;

/*
 * Loop through the points.
 */
    for (ido=0; ido<npts; ido++) {

        xt = x[ido];
        yt = y[ido];
        zt = 1.e30;

    /*
     * Loop through each triangle to find one that
     * contains the current point.
     */
        for (i=0; i<ntriangles; i++) {

        /*
         * Check if the point is inside this triangle's bounding box.
         */
            x1 = 1.e30;
            y1 = 1.e30;
            x2 = -1.e30;
            y2 = -1.e30;
            TrianglePoints (triangles+i,
                            xtri, ytri, ztri);
            for (j=0; j<3; j++) {
                if (xtri[j] < x1) x1 = xtri[j];
                if (ytri[j] < y1) y1 = ytri[j];
                if (xtri[j] > x2) x2 = xtri[j];
                if (ytri[j] > y2) y2 = ytri[j];
            }

            if (xt < x1 || yt < y1  ||
                xt > x2 || yt > y2) {
                continue;
            }

         /*
          * The point is inside the bounding box so check
          * if it is inside the triangle.
          */
            xtri[3] = xtri[0];
            ytri[3] = ytri[0];
            istat = ply_utils_obj.ply_point (xtri, ytri, 4, xt, yt);
            if (istat == -1) {
                continue;
            }

         /*
          * The point is inside the triangle to interpolate z from the
          * plane of the triangle and go to the next point.
          */
            if (ztri[0] > 1.e20  ||  ztri[1] > 1.e20  ||  ztri[2] > 1.e20  ||
                ztri[0] < -1.e20  ||  ztri[1] < -1.e20  ||  ztri[2] < -1.e20) {
                zt = 1.e30;
            }
            else {
                grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);
                zt = coef[0] + xt * coef[1] + yt * coef[2];
            }

            break;

        }

        z[ido] = zt;

    }

    TriangleList = tsav;
    EdgeList = esav;
    NodeList = nsav;

    nedges = nedges;
    numnodes = numnodes;

    return 1;

}  /* end of function grd_back_interpolate_tri_mesh */



/*
 *****************************************************************************************

             g r d _ t r i _ m e s h _ f r o m _ n o d e _ t r i a n g l e s

 *****************************************************************************************

  Given a list of nodes and a list of triangle structures that reference the nodes,
  return a triangle. edge. node topology.  The original node list does not change,
  but an edge list and a new triangle list (based on edges) are each produced.

*/

int CSWGrdTriangle::grd_tri_mesh_from_node_triangles (NOdeTriangleStruct *node_triangles,
                                     int num_node_triangles,
                                     NOdeStruct *nodes,
                                     int numnodes,
                                     EDgeStruct **edges,
                                     int *numedges,
                                     TRiangleStruct **triangles,
                                     int *numtriangles)
{
    NOdeEdgeListStruct     *nelist;
    NOdeTriangleStruct     *ntptr;
    EDgeStruct             *eptr;
    TRiangleStruct         *tptr;
    int                    i, bad, n1, n2, n3, istat,
                           trinum, e1, e2, e3;

/*
 * initialize output in case of error
 */
    *triangles = NULL;
    *numtriangles = 0;
    *edges = NULL;
    *numedges = 0;

/*
 * allocate memeory for the tasks at hand
 */
    nelist = (NOdeEdgeListStruct *)csw_Calloc (numnodes * sizeof(NOdeEdgeListStruct));
    if (nelist == NULL) {
        return -1;
    }

    EdgeList = (EDgeStruct *)csw_Calloc (2 * numnodes * sizeof(EDgeStruct));
    if (EdgeList == NULL) {
        csw_Free (nelist);
        return -1;
    }

    TriangleList = (TRiangleStruct *)csw_Calloc
                   (num_node_triangles * sizeof(TRiangleStruct));
    if (TriangleList == NULL) {
        csw_Free (nelist);
        csw_Free (EdgeList);
        EdgeList = NULL;
        return -1;
    }

    NumNodeEdgeList = numnodes;
    NodeEdgeList = nelist;
    NodeList = nodes;
    NumNodes = numnodes;
    MaxEdges = 2 * numnodes;
    MaxTriangles = num_node_triangles;
    MaxNodes = numnodes;
    NumTriangles = 0;
    NumEdges = 0;
    ExpandNodeList = 0;

/*
 * loop through the input node triangles and create the
 * node, edge, triangle topology
 */
    bad = 0;
    for (i=0; i<num_node_triangles; i++) {
        ntptr = node_triangles + i;
        n1 = ntptr->node1;
        n2 = ntptr->node2;
        n3 = ntptr->node3;
        trinum = AddTriangle (-1, -1, -1, 0);
        if (trinum == -1) {
            bad = 1;
            break;
        }
        e1 = FindEdgeAtNode (n1, n2);
        if (e1 == -1) {
            e1 = AddEdge (n1, n2, trinum, -1, 0);
            if (e1 == -1) {
                bad = 1;
                break;
            }
            istat = AddToNodeEdgeList (e1, n1);
            if (istat == -1) {
                bad = 1;
                break;
            }
            istat = AddToNodeEdgeList (e1, n2);
            if (istat == -1) {
                bad = 1;
                break;
            }
        }
        else {
            eptr = EdgeList + e1;
            eptr->tri2 = trinum;
        }
        e2 = FindEdgeAtNode (n2, n3);
        if (e2 == -1) {
            e2 = AddEdge (n2, n3, trinum, -1, 0);
            if (e2 == -1) {
                bad = 1;
                break;
            }
            istat = AddToNodeEdgeList (e2, n2);
            if (istat == -1) {
                bad = 1;
                break;
            }
            istat = AddToNodeEdgeList (e2, n3);
            if (istat == -1) {
                bad = 1;
                break;
            }
        }
        else {
            eptr = EdgeList + e2;
            eptr->tri2 = trinum;
        }
        e3 = FindEdgeAtNode (n3, n1);
        if (e3 == -1) {
            e3 = AddEdge (n3, n1, trinum, -1, 0);
            if (e1 == -1) {
                bad = 1;
                break;
            }
            istat = AddToNodeEdgeList (e3, n3);
            if (istat == -1) {
                bad = 1;
                break;
            }
            istat = AddToNodeEdgeList (e3, n1);
            if (istat == -1) {
                bad = 1;
                break;
            }
        }
        else {
            eptr = EdgeList + e3;
            eptr->tri2 = trinum;
        }

        tptr = TriangleList + trinum;
        tptr->edge1 = e1;
        tptr->edge2 = e2;
        tptr->edge3 = e3;

    }  /* end of loop through input node triangles */

    if (bad == 1) {
        NodeList = NULL;
        FreeMem ();
        ExpandNodeList = 1;
        return -1;
    }

    *triangles = TriangleList;
    *numtriangles = NumTriangles;
    *edges = EdgeList;
    *numedges = NumEdges;

    ListNull ();
    FreeMem ();

    ExpandNodeList = 1;

    return 1;

} /* end of function grd_trimesh_from_node_triangles */



/*
 ******************************************************************************

    Functions for getting setting and csw_Freeing the triangle index grid data.
    The TriangleIndexGrid is created in this file but the index most often
    needs to be cached by the calling function.  Therefore, there are no
    provisions in this file to csw_Free the triangle index grid.  This must be
    done by the calling function.

 ******************************************************************************
*/


void CSWGrdTriangle::grd_get_triangle_index (
    void    **index,
    double  *x1,
    double  *y1,
    double  *x2,
    double  *y2,
    int     *nc,
    int     *nr,
    double  *xs,
    double  *ys)
{
    *index = (void *)TriangleIndexGrid;
    *x1 = TriangleIndexXmin;
    *y1 = TriangleIndexYmin;
    *x2 = TriangleIndexXmax;
    *y2 = TriangleIndexYmax;
    *nc = TriangleIndexNcol;
    *nr = TriangleIndexNrow;
    *xs = TriangleIndexXspace;
    *ys = TriangleIndexYspace;
}


void CSWGrdTriangle::grd_set_triangle_index (
    void    *index,
    double  x1,
    double  y1,
    double  x2,
    double  y2,
    int     nc,
    int     nr,
    double  xs,
    double  ys)
{
    TriangleIndexGrid = (INdexStruct **)index;
    TriangleIndexXmin = x1;
    TriangleIndexYmin = y1;
    TriangleIndexXmax = x2;
    TriangleIndexYmax = y2;
    TriangleIndexNcol = nc;
    TriangleIndexNrow = nr;
    TriangleIndexXspace = xs;
    TriangleIndexYspace = ys;
}

void CSWGrdTriangle::grd_free_triangle_index (
    void    *index,
    int     ncells)
{
    int              i;
    INdexStruct      **tindex, *iptr;

    if (index == NULL  ||  ncells < 1) {
        return;
    }

    tindex = (INdexStruct **)index;

    for (i=0; i<ncells; i++) {
        iptr = tindex[i];
        if (iptr != NULL) {
            csw_Free (iptr->list);
            csw_Free (iptr);
        }
    }

    csw_Free (tindex);

    return;

}


/*
 *******************************************************************************

           g r d _ d r a p e _ l i n e s _ o n _ t r i _ m e s h

 *******************************************************************************

  Calculate the 3d points where the specified x,y lines lie on the specified
  tri mesh surface.  The z values of the points are calculated from the plane
  of the triangle where the point is located.  There will be more points in
  the output (probably many more points) than in the input because a point
  is generated whenever a line crosses a triangle edge as well as at the
  original vertices of the lines.

  The xout, yout, zout and iout arrays are each allocated here and must be csw_Freed by
  the calling function.

*/

int CSWGrdTriangle::grd_drape_lines_on_tri_mesh (int id1, int id2,
                                 NOdeStruct *nodes,
                                 int numnodes,
                                 EDgeStruct *edges,
                                 int numedges,
                                 TRiangleStruct *triangles,
                                 int numtriangles,
                                 double *xlines,
                                 double *ylines,
                                 int *ilines,
                                 int nlines,
                                 double **xout,
                                 double **yout,
                                 double **zout,
                                 int **iout,
                                 int *nout)

{
    int         istat, i, n, npts;
    TRiangleIndexListStruct    *tptr = NULL;
    NOdeEdgeCacheStruct        *nptr = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        ListNull ();
        FreeMem ();
        if (bsuccess == false) {
            csw_Free (Xout);
            Xout = NULL;
            csw_Free (Yout);
            Yout = NULL;
            csw_Free (Zout);
            Zout = NULL;
            csw_Free (Iout);
            Iout = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * first, clean up anything in private pointers that might be hanging around.
 * and initialize output in case an error occurs
 */
    FreeMem ();

    *xout = NULL;
    *yout = NULL;
    *zout = NULL;
    *iout = NULL;
    *nout = 0;

    NumBoundaryEdges = 0;

    Nsearch = 0;
    Ntpt = 0;
    Nsearch = Nsearch;
    Ntpt = Ntpt;

/*
 * transfer the specified trimesh to private variables.
 */
    NodeList = nodes;
    EdgeList = edges;
    TriangleList = triangles;
    NumNodes = numnodes;
    NumEdges = numedges;
    NumTriangles = numtriangles;

/*
 * Create an index for quick triangle spatial lookup and a list
 * of edges connected to each node.
 */
    tptr = FindTriIndex (id1, id2);
    if (tptr == NULL) {
        istat = CreateTriangleIndexGrid ();
        if (istat == -1) {
            return -1;
        }

        AddStaticTriIndex (id1, id2);
    }
    else {
        grd_set_triangle_index (
            tptr->index,
            tptr->xmin,
            tptr->ymin,
            tptr->xmax,
            tptr->ymax,
            tptr->ncol,
            tptr->nrow,
            tptr->xspace,
            tptr->yspace);
    }

    nptr = FindNodeEdge (id1, id2);
    if (nptr == NULL) {
        istat = BuildNodeEdgeLists ();
        if (istat == -1) {
            return -1;
        }
        AddStaticNodeEdge (id1, id2);
    }
    else {
        NodeEdgeList = nptr->list;
        NumNodeEdgeList = nptr->nlist;
    }

    XoutTiny = (TriangleIndexXspace + TriangleIndexYspace) / 2000.0;

/*
 * allocate the initial output arrays using the private variables
 * these all can grow if needed.
 */
    Maxpout = nlines * numtriangles / 100;
    if (Maxpout < nlines * 10) Maxpout = nlines * 10;
    if (Maxpout > nlines * 100) Maxpout = nlines * 100;
    if (Maxpout < 1000) Maxpout = 1000;
    Maxlout = nlines * 2;
    if (Maxlout < 10) Maxlout = 10;
    Npout = 0;
    Npsav = 0;
    Nlout = 0;
    Xout = (double *)csw_Malloc (Maxpout * sizeof(double));
    if (Xout == NULL) {
        return -1;
    }

    Yout = (double *)csw_Malloc (Maxpout * sizeof(double));
    if (Yout == NULL) {
        return -1;
    }

    Zout = (double *)csw_Malloc (Maxpout * sizeof(double));
    if (Zout == NULL) {
        return -1;
    }

    Iout = (int *)csw_Calloc (Maxlout * sizeof(int));
    if (Iout == NULL) {
        return -1;
    }

/*
 * for each line, find a point that is inside a triangle
 * and trace the line on the surface from there.
 */
    n = 0;
    for (i=0; i<nlines; i++) {
        npts = ilines[i];
        istat = TraceLineOnTriMesh (xlines + n,
                                    ylines + n,
                                    npts);
        if (istat == -1) {
            return -1;
        }
        n += npts;
    }

/*
 * Successful completion.
 */
    *xout = Xout;
    *yout = Yout;
    *zout = Zout;
    *iout = Iout;
    *nout = Nlout;

    Xout = NULL;
    Yout = NULL;
    Zout = NULL;
    Iout = NULL;

    bsuccess = true;

    return 1;

}  /* end of function grd_drape_lines_on_tri_mesh */



/*
 *******************************************************************************

                       F i n d E d g e A t N o d e

 *******************************************************************************

  Return the edge number that connects the two specified nodes.  If none exists
  yet, return -1.

*/

int CSWGrdTriangle::FindEdgeAtNode (int n1, int n2)
{
    NOdeEdgeListStruct    *neptr;
    int                   i, nt1, nt2;

    neptr = NodeEdgeList + n1;
    for (i=0; i<neptr->nlist; i++) {
        nt1 = EdgeList[neptr->list[i]].node1;
        nt2 = EdgeList[neptr->list[i]].node2;
        if (nt1 == n1  &&  nt2 == n2) {
            return neptr->list[i];
        }
        if (nt2 == n1  &&  nt1 == n2) {
            return neptr->list[i];
        }
    }

    return -1;

}  /* end of private FindEdgeAtNode function */



/*
  ****************************************************************************

                     B u i l d N o d e E d g e L i s t s

  ****************************************************************************

    Build lists at each node of the edges connected to the node.
*/

int CSWGrdTriangle::BuildNodeEdgeLists (void)
{
    int                   i, istat, ndo;

/*
 * Clean up any existing list first.
 */
    if (NodeEdgeList) {
        ndo = NumNodeEdgeList;
        for (i=0; i<ndo; i++) {
            if (NodeEdgeList[i].list != NULL) {
                csw_Free (NodeEdgeList[i].list);
            }
        }
        csw_Free (NodeEdgeList);
    }
    NodeEdgeList = NULL;

    NumNodeEdgeList = NumNodes;
    NodeEdgeList = (NOdeEdgeListStruct *)csw_Calloc
                   (NumNodes * sizeof(NOdeEdgeListStruct));
    if (NodeEdgeList == NULL) {
        NumNodeEdgeList = 0;
        return -1;
    }
    for (i=0; i<NumEdges; i++) {
        istat = AddToNodeEdgeList (i, EdgeList[i].node1);
        istat = AddToNodeEdgeList (i, EdgeList[i].node2);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err(1);
            return -1;
        }
    }

    for (i=0; i<NumNodes; i++) {
        if (NodeEdgeList[i].list == NULL  ||
            NodeEdgeList[i].nlist < 1) {
            NodeList[i].deleted = 1;
        }
    }

    return 1;

}  /*  end of private BuildNodeEdgeLists function  */



/*
 **********************************************************************************

            F i n d T r i a n g l e C o n t a i n i n g P o i n t

 **********************************************************************************

  This function tries to use the triangle index grid to determine which triangle
  has the specified point in it or on its edge.  If there is no index grid, the
  function returns -1.  If the index grid exists, but no triangle is found using
  it, -1 is returned.

*/

int CSWGrdTriangle::FindTriangleContainingPoint (double x, double y)
{
    int           irow, jcol, k, itri;

/*
 * both the triangle index grid and the node edge list must have
 * been set up prior to calling this function.
 */
    if (TriangleIndexGrid == NULL) {
        return -1;
    }

    if (NodeEdgeList == NULL) {
        return -1;
    }

/*
 * If the point is outside the surface bounds, it cannot
 * be in any triangle.
 */
    if (x < TriangleIndexXmin || x > TriangleIndexXmax) {
        return -1;
    }
    if (y < TriangleIndexYmin || y > TriangleIndexYmax) {
        return -1;
    }

/*
 * find the index cell that has the point.
 */
    jcol = (int)((x - TriangleIndexXmin) / TriangleIndexXspace);
    irow = (int)((y - TriangleIndexYmin) / TriangleIndexYspace);
    k = irow * TriangleIndexNcol + jcol;

/*
 * look in that index cell.
 */
    if (TriangleIndexGrid[k] != NULL) {
        itri = SearchIndexForTriangle (x, y, k);
        if (itri >= 0) {
            return itri;
        }
    }

    return -1;

}  /* end of private FindTriangleContainingPoint function */




/*
 ******************************************************************************

                S e a r c h I n d e x F o r T r i a n g l e

 ******************************************************************************

  Check all the triangles connected to all the nodes in the specified index
  cell to see if any contain the specified point.  If a triangle containing
  the specified point is found, return its triangle number.  If no triangle
  is found, return -1.

*/

int CSWGrdTriangle::SearchIndexForTriangle (double x, double y, int k)
{
    INdexStruct      *iptr;
    int              i, n, *list, trinum;
    int              istat;

    iptr = TriangleIndexGrid[k];
    if (iptr == NULL) {
        return -1;
    }

    list = iptr->list;
    n = iptr->npts;

    for (i=0; i<n; i++) {
        Nsearch++;
        trinum = list[i];
        istat = PointInTriangle (x, y, trinum);
        if (istat == 1) {
            return trinum;
        }
    }

    return -1;

}  /* end of private SearchIndexForTriangle function */




/*
  ****************************************************************************

                   E d g e I n t e r s e c t X Y

  ****************************************************************************

    Return 1 and the intersection point if the specified edge intersects
    the specified segment.  Return zero otherwise.

    This is only called when processing a line segment intersection with a
    surface which it is draping over.  A special condition of this usage is
    that an intersection essentially identical to the first point of the
    segment specified is not considered to be a valid intersection.  The
    first point passed will always already be in the output arrays so we
    do not want to use it again.

*/

int CSWGrdTriangle::EdgeIntersectXY (int edgenum, int firstflag, int allow_graze,
                            double x1, double y1, double x2, double y2,
                            double *xint, double *yint, double *zint)
{
    int               istat;
    double            x3, y3, x4, y4,
                      xt, yt, zt;
    double            dx, dy, dxt, dyt, dnorm, dist, indexav;
    NOdeStruct        *np;
    EDgeStruct        *ep;

    CSWPolyUtils             ply_utils_obj;

    *xint = 1.e30;
    *yint = 1.e30;
    *zint = GRD_SOFT_NULL_VALUE;

    ep = EdgeList + edgenum;
    np = NodeList + ep->node1;

    x3 = np->x;
    y3 = np->y;
    np = NodeList + ep->node2;
    x4 = np->x;
    y4 = np->y;

    istat = ply_utils_obj.ply_segint(x1, y1, x2, y2, x3, y3, x4, y4, &xt, &yt);
    if (istat != 0) {
        return 0;
    }

  /*
   * If this grazes with the x1,y1 point, do not consider it an intersection
   * with this edge.
   */

  /*
   * BUG 8906
   *
   * In some cases its ok to graze with the first point,
   * so the allow_graze parameter decides.
   */
    if (allow_graze == 0) {
        if (firstflag == 0) {
            istat = OutPointGraze (xt, yt, x1, y1);
            if (istat == 1) {
                return 0;
            }
        }
    }

  /*
   * If the intersect point is very close to either end point,
   * move it slightly towards the middle so there is no question
   * about which edge is being crossed.
   */
    GrazingIntersection = 0;
    GrazingXt = 1.e30;
    GrazingYt = 1.e30;
    indexav = (TriangleIndexXspace + TriangleIndexYspace) / 200.0;
    istat = OutPointGraze (xt, yt, x3, y3);
    if (istat == 1) {
        GrazingXt = xt;
        GrazingYt = yt;
        dx = xt - x4;
        dy = yt - y4;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        dnorm = dist / indexav;
        if (dnorm < 10.0) dnorm = 10.0;
        dxt = dx / dnorm;
        dyt = dy / dnorm;
        xt -= dxt;
        yt -= dyt;
        GrazingIntersection = 1;
    }
    else {
        istat = OutPointGraze (xt, yt, x4, y4);
        if (istat == 1) {
            GrazingXt = xt;
            GrazingYt = yt;
            dx = xt - x3;
            dy = yt - y3;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            dnorm = dist / indexav;
            if (dnorm < 10.0) dnorm = 10.0;
            dxt = dx / dnorm;
            dyt = dy / dnorm;
            xt -= dxt;
            yt -= dyt;
            GrazingIntersection = 1;
        }
    }

    zt = TrianglePlaneZvalue (ep->tri1, xt, yt);

    *xint = xt;
    *yint = yt;
    *zint = zt;

    return 1;

}  /*  end of private EdgeIntersectXY function  */



/*
 ***********************************************************************************

                    T r a c e L i n e O n T r i M e s h

 ***********************************************************************************

  Break up the line into individual segments and trace each segment across
  the surface of the tri mesh.

*/

int CSWGrdTriangle::TraceLineOnTriMesh (double *xlines,
                               double *ylines,
                               int npts)
{
    int               i, istat;
    int               ido, nclip, *iclip;
    double            *xclip, *yclip, *xp, *yp;
    double            x1, y1, x2, y2, tiny;

/*
 * First, clip the line to the tri mesh.
 */
    istat = ClipLineToTriMesh (xlines, ylines, npts,
                               &xclip, &yclip,
                               &iclip, &nclip);
    if (istat == -1) {
        return -1;
    }

    if (nclip < 1) {
        return 1;
    }

/*
 * Drape each segment of each clipped polyline over the tri mesh.
 */
    xp = xclip;
    yp = yclip;
    for (ido=0; ido<nclip; ido++) {
        for (i=0; i<iclip[ido]-1; i++) {
            x1 = xp[i];
            y1 = yp[i];
            x2 = xp[i+1];
            y2 = yp[i+1];
            if (nclip > 1) {
                tiny = (x2 - x1) / 1000.0;
                x1 += tiny;
                x2 -= tiny;
                tiny = (y2 - y1) / 1000.0;
                y1 += tiny;
                y2 -= tiny;
            }
            istat = ProcessDrapedSegment (x1, y1, x2, y2);
            if (istat == -1) {
                return -1;
            }
        }
        xp += iclip[ido];
        yp += iclip[ido];
        if (Nlout >= Maxlout) {
            Maxlout += 20;
            Iout = (int *)csw_Realloc (Iout, Maxlout * sizeof(int));
            if (Iout == NULL) {
                return -1;
            }
        }
        Iout[Nlout] = Npout - Npsav;
        Nlout++;
        Npsav = Npout;
    }

    csw_Free (xclip);
    csw_Free (iclip);

    return 1;

}  /* end of private TraceLineOnTriMesh function */




/*
 **************************************************************************************

                      P r o c e s s D r a p e d S e g m e n t

 **************************************************************************************

  Calculate the 3D intersections between the segment and the current tri mesh.
  On success, the triangle number of the last triangle traversed is returned.
  If a memory allocation failure occurs, -1 is returned.

*/

int CSWGrdTriangle::ProcessDrapedSegment (double x1, double y1,
                                 double x2, double y2)
{
    TRiangleStruct   *tptr;
    EDgeStruct       *eptr;
    int              e1, elast, tlast, istat;
    double           xt, yt, zt, xtri[3], ytri[3], ztri[3], coef[3];
    double           xlast, ylast, dx, dy, dxt, dyt, dist, dnorm;
    int              n, ndo, itri, itri2, edgenum, same_triangle;

/*
 * If the segment is essentially of zero length, just append
 * the start and end points to the output and return.
 */
    dx = x2 - x1;
    dy = y2 - y1;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);

    if (dist < XoutTiny * 5.0) {

        itri = FindTriangleContainingPoint (x1, y1);
        if (itri < 0) {
            itri = FindTriangleContainingPoint (x2, y2);
        }
        if (itri < 0) return 1;

        tptr = TriangleList + itri;
        TrianglePoints (tptr, xtri, ytri, ztri);
        grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);

        zt = coef[0] + x1 * coef[1] + y1 * coef[2];
        istat = AppendOutPoint (x1, y1, zt);
        if (istat == -1) {
            return -1;
        }

        zt = coef[0] + x2 * coef[1] + y2 * coef[2];
        istat = AppendOutPoint (x2, y2, zt);
        return istat;
    }

/*
 * Find the triangle containing the first point of the
 * segment.  This point should always be inside a triangle
 * since the segment was clipped before being passed to this
 * function, but grazing can still happen.  If the point is
 * not inside a triangle, adjust it slightly toward the
 * center of the line segment and try again.
 *
 * It turns out that when internal baundary edges exist in the
 * trimesh, as is common in gocad data, the grazing problem
 * can be much more severe.  The code needs to seach much further
 * along the segment to find a good starting point.
 */
    dxt = XoutTiny * 10.0;
    dnorm = dist / dxt;
    if (dnorm < 1.0) dnorm = 1.0;
    dxt = dx / dnorm;
    dyt = dy / dnorm;
    itri = FindTriangleContainingPoint (x1, y1);
    ndo = 0;
    while (itri < 0) {
        itri = FindTriangleContainingPoint (x1+dxt, y1+dyt);
        if (itri == -1  &&  ndo > 12) {
            return 1;
        }
        ndo++;
        dxt *= 1.5;
        dyt *= 1.5;
    }
    istat = CheckTriangleNodeGraze (itri, x1, y1);

/*
 * BUG 8906
 *
 * Make sure the first point in the segment is not coincident
 * with a triangle node.
 */
    n = 1;
    while (istat == 1) {
        x1 += dxt * n;
        y1 += dyt * n;
        itri = FindTriangleContainingPoint (x1, y1);
        if (itri < 0) {
            n++;
            if (n > 10) break;
            continue;
        }
        istat = CheckTriangleNodeGraze (itri, x1, y1);
        n++;
        if (n > 10) break;
    }

    if (istat == 1) {
        return 1;
    }

/*
 * BUG 8906
 *
 * Explicitly determine if both endpoints are in the same triangle.
 */
    itri2 = FindTriangleContainingPoint (x2, y2);
    if (itri == itri2) {
        same_triangle = 1;
    }
    else {
        same_triangle = 0;
    }
    edgenum = FindEdgeContainingPoint (itri, x1, y1);

    if (edgenum >= 0) {
        if (EdgeList[edgenum].tri1 == itri) {
            itri2 = EdgeList[edgenum].tri2;
        }
        else {
            itri2 = EdgeList[edgenum].tri1;
        }
    }
    else {
        itri2 = -1;
    }

/*
 * Get the plane for the initial triangle and
 * use it to calculate the initial z value.
 */
    tptr = TriangleList + itri;
    tlast = itri;
    TrianglePoints (tptr,
                    xtri, ytri, ztri);
    grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);
    zt = coef[0] + x1 * coef[1] + y1 * coef[2];

/*
 * Add the initial segment point to the output arrays.
 */
    istat = AppendOutPoint (x1, y1, zt);
    if (istat == -1) {
        return -1;
    }

    if (same_triangle) {
        zt = coef[0] + x2 * coef[1] + y2 * coef[2];
        istat = AppendOutPoint (x2, y2, zt);
        return istat;
    }

/*
 * Find the triangle edge that the segment intersects in the
 * initial triangle.
 */
    xt = 1.e30;
    yt = 1.e30;
    e1 = tptr->edge1;
    istat = 0;
    if (e1 != edgenum) {
        istat = EdgeIntersectXY (e1, 1, 1,
                                 x1, y1, x2, y2,
                                 &xt, &yt, &zt);
    }
    if (istat == 0) {
        e1 = tptr->edge2;
        istat = 0;
        if (e1 != edgenum) {
            istat = EdgeIntersectXY (e1, 1, 1,
                                     x1, y1, x2, y2,
                                     &xt, &yt, &zt);
        }
        if (istat == 0) {
            e1 = tptr->edge3;
            istat = 0;
            if (e1 != edgenum) {
                istat = EdgeIntersectXY (e1, 1, 1,
                                         x1, y1, x2, y2,
                                         &xt, &yt, &zt);
            }
        }
    }

/*
 * BUG 8906
 *
 * If there is no intersection, it may be because the initial triangle was wrong.
 * Try the itri2, which is the other triangle sharing the initial edge.
 */
    if (itri2 >= 0) {
        if (istat == 0  ||  xt > 1.e20) {
            itri = itri2;
            tptr = TriangleList + itri;
            tlast = itri;
            TrianglePoints (tptr,
                            xtri, ytri, ztri);
            grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);
            zt = coef[0] + x1 * coef[1] + y1 * coef[2];

            xt = 1.e30;
            yt = 1.e30;
            e1 = tptr->edge1;
            istat = 0;
            if (e1 != edgenum) {
                istat = EdgeIntersectXY (e1, 1, 1,
                                         x1, y1, x2, y2,
                                         &xt, &yt, &zt);
            }
            if (istat == 0) {
                e1 = tptr->edge2;
                istat = 0;
                if (e1 != edgenum) {
                    istat = EdgeIntersectXY (e1, 1, 1,
                                             x1, y1, x2, y2,
                                             &xt, &yt, &zt);
                }
                if (istat == 0) {
                    e1 = tptr->edge3;
                    istat = 0;
                    if (e1 != edgenum) {
                        istat = EdgeIntersectXY (e1, 1, 1,
                                                 x1, y1, x2, y2,
                                                 &xt, &yt, &zt);
                    }
                }
            }
        }
    }

/*
 * If there was still no intersection, the segment is entirely inside
 * the initial triangle.  Since each end point of the segment could
 * graze on the triangle edge, I use the segment center point to
 * determine which triangle the point is inside.
 *
 * Another possibility is that the segment starts on a boundary edge
 * of the trimesh and extends into a null area.  In this case, the
 * triangle containing the mid point doesn't exist and no draped
 * segment should be output.
 */
    if (istat == 0  ||  xt > 1.e20) {
        itri = FindTriangleContainingPoint ((x1 + x2) / 2.0, (y1 + y2) / 2.0);
        if (itri == -1) {
            Npout--;
            return 1;
        }
        tptr = TriangleList + itri;
        tlast = itri;
        TrianglePoints (tptr,
                        xtri, ytri, ztri);
        grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);
        zt = coef[0] + x2 * coef[1] + y2 * coef[2];
        if (LastSegmentFlag == 1) LastSegmentFlag = 2;
        istat = AppendOutPoint (x2, y2, zt);
        if (istat == -1) {
            return -1;
        }
        return itri;
    }

/*
 * Add the first edge intersection to the output.
 */
    if (OutPointGraze (x1, y1, xt, yt) == 0) {
        istat = AppendOutPoint (xt, yt, zt);
        if (istat == -1) {
            return -1;
        }
    }

    if (GrazingXt < 1.e20  &&  GrazingYt < 1.e20) {
        if (OutPointGraze (GrazingXt, GrazingYt, x2, y2) == 1) {
            if (LastSegmentFlag == 1) LastSegmentFlag = 2;
            return itri;
        }
    }
    else {
        if (OutPointGraze (xt, yt, x2, y2) == 1) {
            if (LastSegmentFlag == 1) LastSegmentFlag = 2;
            return itri;
        }
    }
    xlast = xt;
    ylast = yt;

/*
 * Now that we have an edge intersection, follow from triangle to
 * triangle computing other edge intersections.
 */
    elast = e1;
    for (;;) {

    /*
     * Find the triangle that shares the edge that was not
     * the last triangle traversed.  If none is found, the
     * draped line terminates on the edge of the surface.
     */
        eptr = EdgeList + elast;
        if (eptr->tri1 == tlast) {
            tlast = eptr->tri2;
        }
        else {
            tlast = eptr->tri1;
        }
        if (tlast == -1) {
            break;
        }
        tptr = TriangleList + tlast;

    /*
     * Check the two edges that were not the last edge
     * for intersections with the remaining segment.
     */
        e1 = tptr->edge1;
        if (e1 != elast) {
            istat = EdgeIntersectXY (e1, 0, 1,
                                     xlast, ylast, x2, y2,
                                     &xt, &yt, &zt);
            if (istat != 0) {
                elast = e1;
                xlast = xt;
                ylast = yt;
                istat = AppendOutPoint (xt, yt, zt);
                if (istat == -1) {
                    return -1;
                }
                if (GrazingXt < 1.e20  &&  GrazingYt < 1.e20) {
                    if (OutPointGraze (GrazingXt, GrazingYt, x2, y2) == 1) {
                        if (LastSegmentFlag == 1) LastSegmentFlag = 2;
                        return itri;
                    }
                }
                else {
                    if (OutPointGraze (xt, yt, x2, y2) == 1) {
                        if (LastSegmentFlag == 1) LastSegmentFlag = 2;
                        return itri;
                    }
                }
                continue;
            }
        }

        e1 = tptr->edge2;
        if (e1 != elast) {
            istat = EdgeIntersectXY (e1, 0, 1,
                                     xlast, ylast, x2, y2,
                                     &xt, &yt, &zt);
            if (istat != 0) {
                elast = e1;
                xlast = xt;
                ylast = yt;
                istat = AppendOutPoint (xt, yt, zt);
                if (istat == -1) {
                    return -1;
                }
                if (GrazingXt < 1.e20  &&  GrazingYt < 1.e20) {
                    if (OutPointGraze (GrazingXt, GrazingYt, x2, y2) == 1) {
                        if (LastSegmentFlag == 1) LastSegmentFlag = 2;
                        return itri;
                    }
                }
                else {
                    if (OutPointGraze (xt, yt, x2, y2) == 1) {
                        if (LastSegmentFlag == 1) LastSegmentFlag = 2;
                        return itri;
                    }
                }
                continue;
            }
        }

        e1 = tptr->edge3;
        if (e1 != elast) {
            istat = EdgeIntersectXY (e1, 0, 1,
                                     xlast, ylast, x2, y2,
                                     &xt, &yt, &zt);
            if (istat != 0) {
                elast = e1;
                xlast = xt;
                ylast = yt;
                istat = AppendOutPoint (xt, yt, zt);
                if (istat == -1) {
                    return -1;
                }
                if (GrazingXt < 1.e20  &&  GrazingYt < 1.e20) {
                    if (OutPointGraze (GrazingXt, GrazingYt, x2, y2) == 1) {
                        if (LastSegmentFlag == 1) LastSegmentFlag = 2;
                        return itri;
                    }
                }
                else {
                    if (OutPointGraze (xt, yt, x2, y2) == 1) {
                        if (LastSegmentFlag == 1) LastSegmentFlag = 2;
                        return itri;
                    }
                }
                continue;
            }
        }

    /*
     * None of the edges of the triangle intersect, so the
     * remaining segment must end inside the triangle.
     */
        break;
    }

/*
 * Add the end segment point to the output arrays.
 */
    TrianglePoints (tptr,
                    xtri, ytri, ztri);
    grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);
    zt = coef[0] + x2 * coef[1] + y2 * coef[2];

    if (LastSegmentFlag == 1) LastSegmentFlag = 2;
    istat = AppendOutPoint (x2, y2, zt);
    if (istat == -1) {
        return -1;
    }

    return (tptr - TriangleList);

}  /* end of private ProcessDrapedSegment function */




/*
 *******************************************************************************

                 A p p e n d O u t P o i n t

 *******************************************************************************

  Append a point to the Xout, Yout, Zout arrays, csw_Reallocing as needed.

*/

int CSWGrdTriangle::AppendOutPoint (double x, double y, double z)
{
    if (x > 1.e20  ||  y > 1.e20) {
        return 1;
    }

    if (OutPointGraze (x, y, XoutLast, YoutLast)) {
        return 1;
    }

    if (Npout >= Maxpout) {
        Maxpout += OUTCHUNK;
        Xout = (double *)csw_Realloc (Xout, Maxpout * sizeof(double));
        if (Xout == NULL) {
            return -1;
        }
        Yout = (double *)csw_Realloc (Yout, Maxpout * sizeof(double));
        if (Yout == NULL) {
            return -1;
        }
        Zout = (double *)csw_Realloc (Zout, Maxpout * sizeof(double));
        if (Zout == NULL) {
            return -1;
        }
    }

    Xout[Npout] = x;
    Yout[Npout] = y;
    Zout[Npout] = z;

    XoutLast = x;
    YoutLast = y;

    Npout++;

    return 1;

}  /* end of private AppendOutPoint function */



/*
 *************************************************************************************

                           O u t P o i n t G r a z e

 *************************************************************************************

  Return 1 if the two points are withing XoutTiny of each other or return zero
  if they are not.

*/

int CSWGrdTriangle::OutPointGraze (double x1, double y1, double x2, double y2)
{
    double             dx, dy, dist;

    dx = x1 - x2;
    dy = y1 - y2;
    dist = dx * dx + dy * dy;
    dist = sqrt(dist);

    if (dist <= XoutTiny) {
        return 1;
    }

    return 0;

}  /* end of private OutPointGraze function */




/*
 *******************************************************************************************

                g r d _ c a l c _ h o l e s _ i n _ t r i m e s h

 *******************************************************************************************

  Whack edges that are:

    1: not constraint edges
    2: have all nodes in the shared triangles of the edge with z values
       between the specified min and max (inclusive)

  Any edges, nodes and triangles deleted in the process are removed from the
  topology and thus the number of (nodes, edges, triangles) returned may
  differ from the numbers passed.

*/

int CSWGrdTriangle::grd_calc_holes_in_tri_mesh (NOdeStruct *nodes,
                                int *numnodes,
                                EDgeStruct *edges,
                                int *numedges,
                                TRiangleStruct *triangles,
                                int *numtriangles,
                                double zmin,
                                double zmax)
{
    int                   i, j, n, np, n1, n2, n3;
    double                xtri[4], ytri[4], ztri[4];
    EDgeStruct            *eptr;

    TriangleList = triangles;
    EdgeList = edges;
    NodeList = nodes;
    NumTriangles = *numtriangles;
    NumEdges = *numedges;
    NumNodes = *numnodes;

    for (i=0; i<*numedges; i++) {
        eptr = edges + i;
        if (eptr->flag != 0) {
            continue;
        }
        if (eptr->deleted) {
            continue;
        }
        if (eptr->tri1 < 0  &&  eptr->tri2 < 0) {
            RemoveEdgeFromNodeList (eptr->node1, eptr - EdgeList);
            RemoveEdgeFromNodeList (eptr->node2, eptr - EdgeList);
            eptr->deleted = 1;
            continue;
        }
        n1 = eptr->tri1;
        TrianglePoints (triangles+n1, xtri, ytri, ztri);
        n2 = eptr->tri2;
        if (n2 >= 0) {
            n3 = OppositeNode (n2, i);
            if (n3 >= 0) {
                np = 4;
                xtri[3] = nodes[n3].x;
                ytri[3] = nodes[n3].y;
                ztri[3] = nodes[n3].z;
            }
            else {
                np = 3;
            }
        }
        else {
            np = 3;
        }

        n = 0;
        for (j=0; j<np; j++) {
            if (ztri[j] >= zmin  &&  ztri[j] <= zmax) {
                n++;
            }
        }

        if (n == np) {
            WhackEdge (i);
        }

    }

    RemoveDeletedElements ();

    *numtriangles = NumTriangles;
    *numedges = NumEdges;
    *numnodes = NumNodes;

    ListNull ();
    FreeMem();

    return 1;

}  /* end of grd_calc_holes_in_tri_mesh function */



/*
 *****************************************************************************************

              T r i a n g l e P l a n e Z v a l u e

 *****************************************************************************************

  Return the z value for the specified point on the plane determined from
  the specified triangle.

*/

double CSWGrdTriangle::TrianglePlaneZvalue (int tnum, double x, double y)
{
    double          xtri[3], ytri[3], ztri[3], coef[3], z;
    double          zchk;
    TRiangleStruct  *tptr;

    tptr = TriangleList + tnum;
    TrianglePoints (tptr, xtri, ytri, ztri);

/*
 * bug 9139
 *  If any triangle z point is NULL, make the output NULL
 */
    zchk = NullValue;
    if (zchk > 1.e20) zchk = 1.e20;
    if (ztri[0] > zchk  ||  ztri[1] > zchk  ||  ztri[2] > zchk) {
        z = 1.e30;
    }
    else if (ztri[0] < -zchk  ||  ztri[1] < -zchk  ||  ztri[2] < -zchk) {
        z = 1.e30;
    }
    else {
        grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);
        z = coef[0] + coef[1] * x + coef[2] * y;
    }

    return z;

}





/*
 *****************************************************************************************

              C h o o s e T r i a n g l e P l a n e Z v a l u e

 *****************************************************************************************

  One of the two triangles should enclose the point.  Return the z value from the
  plane of the enclosing triangle.
*/

double CSWGrdTriangle::ChooseTrianglePlaneZvalue (int t1, int t2, double x, double y)
{
    double          xtri[4], ytri[4], ztri[4], coef[3], z;
    TRiangleStruct  *tptr;
    int             istat;

    CSWPolyUtils             ply_utils_obj;

    assert (t1 >= 0);

    z = GRD_SOFT_NULL_VALUE;
    tptr = TriangleList + t1;
    TrianglePoints (tptr, xtri, ytri, ztri);
    xtri[3] = xtri[0];
    ytri[3] = ytri[0];
    istat = ply_utils_obj.ply_point (xtri, ytri, 4, x, y);
    if (istat >= 0) {
        if (ztri[0] >= NullValue  ||  ztri[1] >= NullValue  ||  ztri[2] >= NullValue) {
            z = 1.e30;
        }
        else if (ztri[0] <= -1.e15  ||  ztri[1] <= -1.e15  ||  ztri[2] <= -1.e15) {
            z = 1.e30;
        }
        else {
            grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);
            z = coef[0] + coef[1] * x + coef[2] * y;
        }
    }
    else if (t2 >= 0) {
        tptr = TriangleList + t2;
        TrianglePoints (tptr, xtri, ytri, ztri);
        xtri[3] = xtri[0];
        ytri[3] = ytri[0];
        istat = ply_utils_obj.ply_point (xtri, ytri, 4, x, y);
        if (istat >= 0) {
            if (ztri[0] >= NullValue  ||  ztri[1] >= NullValue  ||  ztri[2] >= NullValue) {
                z = 1.e30;
            }
            else if (ztri[0] <= -1.e15  ||  ztri[1] <= -1.e15  ||  ztri[2] <= -1.e15) {
                z = 1.e30;
            }
            else {
                grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);
                z = coef[0] + coef[1] * x + coef[2] * y;
            }
        }
    }

    return z;

}


/*
 ***********************************************************************************

                     C l i p S e g m e n t T o T r i M e s h

 ***********************************************************************************

  Clip the specified segment to the trimesh.  Any edge that has only one triangle
  associated with it is considered to be on the tri mesh boundary.  The segment
  is intersected with these edges and the parts of the input segment between
  intersections, and also inside the trimesh are output as clipped segments.

*/

int CSWGrdTriangle::ClipSegmentToTriMesh (double x1, double y1,
                                 double x2, double y2,
                                 CLippedSegment **results,
                                 int *nresults)
{
    CLippedSegment        *clipseg = NULL;
    CLippedPoint          *cpoint = NULL, *cp = NULL,
                          **cplist = NULL, **cplist2 = NULL;
    int                   ii, i, j, istat, n, it1, it2, modchek, bad;
    int                   ncpoint, maxcpoint, nstart, ido;
    double                xint, yint, zint, dint, d1, d2, dx, dy;
    double                tinysav;

/*
 * !!!! debug only
 */
    int do_write, iflag, n1, n2;
    char fline[200];
    FILE *fptr;
    double   xt1, yt1, xt2, yt2;
    double   vbase[6];

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (cpoint);
        csw_Free (cplist);
        if (bsuccess == false) {
            csw_Free (clipseg);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of error.
 */
    *results = NULL;
    *nresults = 0;

/*
 * Check if one or both endpoints is inside a triangle.
 */
    it1 = FindTriangleContainingPoint (x1, y1);
    it2 = FindTriangleContainingPoint (x2, y2);

/*
 * Calculate the boundary intersections and put them in the
 * cpoint array.  The original end points are also put into the
 * array.
 */
    maxcpoint = 20;
    cpoint = (CLippedPoint *)csw_Malloc (maxcpoint * sizeof(CLippedPoint));
    if (cpoint == NULL) {
        return -1;
    }
    cpoint[0].x = x1;
    cpoint[0].y = y1;
    cpoint[0].tnum = it1;
    cpoint[0].edgenum = -1;
    cpoint[0].dist = 0.0;
    cpoint[0].graze = 0;
    cpoint[1].x = x2;
    cpoint[1].y = y2;
    cpoint[1].tnum = it2;
    cpoint[1].edgenum = -1;
    cpoint[1].dist = 1.e30;
    cpoint[1].graze = 0;

    ncpoint = 2;




/*
 *  !!!! debug only
 */
    do_write = csw_GetDoWrite ();;
    if (do_write) {
        fptr = fopen ("edges.xy", "wb");
        if (fptr) {
            sprintf (fline, "%f %f %f %f\n", x1, y1, x2, y2);
            fputs (fline, fptr);
            for (i=0; i<NumEdges; i++) {
                if (EdgeList[i].deleted) continue;
                iflag = 0;
                if (EdgeList[i].tri2 < 0) iflag = 1;
                n1 = EdgeList[i].node1;
                n2 = EdgeList[i].node2;
                xt1 = NodeList[n1].x;
                yt1 = NodeList[n1].y;
                xt2 = NodeList[n2].x;
                yt2 = NodeList[n2].y;
                sprintf (fline, "%.3f %.3f %.3f %.3f %d %d\n",
                         xt1, yt1, xt2, yt2, i, iflag);
                fputs (fline, fptr);
            }
            fclose (fptr);
            fptr = NULL;
        }
    }

    if (do_write) {
        sprintf (fline, "drapeclip.tri");
        vbase[0] = 1.e30;
        vbase[1] = 1.e30;
        vbase[2] = 1.e30;
        vbase[3] = 1.e30;
        vbase[4] = 1.e30;
        vbase[5] = 1.e30;
        grd_WriteTextTriMeshFile (
            0, vbase,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fline);
    }




/*
 * Boundary edges are edges with one good triangle attached.
 */
    if (NumBoundaryEdges < 1) {
        NumBoundaryEdges = 0;
        for (i=0; i<NumEdges; i++) {
            if (EdgeList[i].deleted) continue;
            if (EdgeList[i].tri2 >= 0) continue;
            if (EdgeList[i].tri1 < 0) continue;
            BoundaryEdges[NumBoundaryEdges] = i;
            NumBoundaryEdges++;
            if (NumBoundaryEdges > MAX_BOUNDARY_EDGES-1) {
                NumBoundaryEdges = MAX_BOUNDARY_EDGES - 1;
            }
        }
    }

    for (ii=0; ii<NumBoundaryEdges; ii++) {
        i = BoundaryEdges[ii];
        istat = EdgeIntersectXY (i, 1, 1,
                                 x1, y1, x2, y2,
                                 &xint, &yint, &zint);
        if (istat != 1) {
            continue;
        }
        dx = xint - x1;
        dy = yint - y1;
        dint = dx * dx + dy * dy;
        dint = sqrt (dint);
        if (maxcpoint >= ncpoint) {
            maxcpoint += 20;
            cpoint = (CLippedPoint *)csw_Realloc (cpoint, maxcpoint * sizeof(CLippedPoint));
            if (cpoint == NULL) {
                return -1;
            }
        }
        cpoint[ncpoint].x = xint;
        cpoint[ncpoint].y = yint;
        cpoint[ncpoint].tnum = EdgeList[i].tri1;
        cpoint[ncpoint].edgenum = i;
        cpoint[ncpoint].dist = dint;
        cpoint[ncpoint].graze = GrazingIntersection;
        ncpoint++;
    }

/*
 * If there are only 2 points, the original segment is entirely
 * inside or entirely outside the tri mesh.
 */
    if (ncpoint == 2) {
        if (it1 < 0  ||  it2 < 0) {
            return 1;
        }
        clipseg = (CLippedSegment *)csw_Malloc (sizeof(CLippedSegment));
        if (clipseg == NULL) {
            return -1;
        }
        clipseg[0].x1 = x1;
        clipseg[0].y1 = y1;
        clipseg[0].x2 = x2;
        clipseg[0].y2 = y2;
        clipseg[0].tri1 = it1;
        clipseg[0].tri2 = it2;
        clipseg[0].edge1 = -1;
        clipseg[0].edge2 = -1;
        *results = clipseg;
        *nresults = 1;
        return 2;
    }

/*
 * sort in ascending distance from x1, y1
 */
    cplist = (CLippedPoint **)csw_Malloc (2 * ncpoint * sizeof(CLippedPoint*));
    if (cplist == NULL) {
        return -1;
    }
    cplist2 = cplist + ncpoint;

    for (i=0; i<ncpoint; i++) {
        cplist[i] = cpoint + i;
    }

    for (i=1; i<ncpoint; i++) {
        d1 = cplist[i]->dist;
        for (j=i+1; j<ncpoint; j++) {
            d2 = cplist[j]->dist;
            if (d2 < d1) {
                cp = cplist[i];
                cplist[i] = cplist[j];
                cplist[j] = cp;
            }
        }
    }

/*
 * Remove duplicate points.  This is attempted up to 6 times
 * increasing the grazing distance each time.  After each
 * try a check for the approriate number of points is done.
 * If all of this fails, no clipped segments are output.
 */
    tinysav = XoutTiny;
    bad = 0;
    for (ido=0; ido<6; ido++) {
        cplist2[0] = cplist[0];
        n = 1;
        i = 1;
        for (;;) {
            istat = 0;
            if (cplist[i-1]->graze == 1  &&
                cplist[i]->graze == 1) {
                istat = OutPointGraze (cplist[i-1]->x, cplist[i-1]->y,
                                       cplist[i]->x, cplist[i]->y);
            }
            while (istat && i < ncpoint) {
                if (cplist2[n-1]->edgenum == -1) {
                    cplist2[n-1]->edgenum = cplist[i]->edgenum;
                }
                i++;
                if (i < ncpoint) {
                    istat = OutPointGraze (cplist[i-1]->x, cplist[i-1]->y,
                                           cplist[i]->x, cplist[i]->y);
                }
            }
            cplist2[n] = cplist[i];
            n++;
            i++;
            if (i >= ncpoint) break;
        }
        ncpoint = n;

    /*
     * If both original segment end points were inside triangles
     * or both were outside triangles, there should be an even
     * number of unique intersections.  If one was inside a
     * triangle and the other outside all triangles, there should
     * be an odd number of unique intersection points.
     *
     * If this is not the case, increment the XoutTiny value and
     * try again.
     */
        bad = 0;
        modchek = n % 2;
        if (it1 >= 0  &&  it2 >= 0) {
            if (modchek == 1) bad = 1;
        }
        if (it1 < 0  &&  it2 < 0) {
            if (modchek == 1) bad = 1;
        }
        if (it1 >= 0  &&  it2 < 0) {
            if (modchek == 0) bad = 1;
        }
        if (it1 < 0  &&  it2 >= 0) {
            if (modchek == 0) bad = 1;
        }
        if (bad == 0) break;
        for (i=0; i<ncpoint; i++) {
            cplist[i] = cplist2[i];
        }
        XoutTiny *= 2;
    }
    XoutTiny = tinysav;

    if (bad == 1) {
        return 1;
    }
/*
 * Allocate space for output clipped segments.
 */
    clipseg = (CLippedSegment *)csw_Malloc (ncpoint * sizeof(CLippedSegment));
    if (clipseg == NULL) {
        return -1;
    }

/*
 * If the first point of the input segment is in a triangle, start pairing
 * up points from that point.  If it was not in a triangle, start pairing
 * points from the second point in the sorted point list.
 */
    nstart = 0;
    if (it1 < 0) nstart = 1;
    n = 0;
    for (i=nstart; i<ncpoint-1; i+=2) {
        clipseg[n].x1 = cplist2[i]->x;
        clipseg[n].y1 = cplist2[i]->y;
        clipseg[n].x2 = cplist2[i+1]->x;
        clipseg[n].y2 = cplist2[i+1]->y;
        clipseg[n].tri1 = cplist2[i]->tnum;
        clipseg[n].tri2 = cplist2[i+1]->tnum;
        clipseg[n].edge1 = cplist2[i]->edgenum;
        clipseg[n].edge2 = cplist2[i+1]->edgenum;
        n++;
    }

    *results = clipseg;
    *nresults = n;

    bsuccess = true;

    return 1;

}  /* end of private ClipSegmentToTriMesh function */




/*
 **************************************************************************************

                 C l i p L i n e T o T r i M e s h

 **************************************************************************************

*/

int CSWGrdTriangle::ClipLineToTriMesh (double *xline, double *yline, int npts,
                              double **xout, double **yout,
                              int **iout, int *nout)
{
    CLippedSegment   *clipseg = NULL, *seglist = NULL;
    int              i, j, nseg, np, nline, nplast, *ip = NULL,
                     istat, maxseg, chunksize, nclipseg;
    double           x1, y1, x2, y2, *xp = NULL, *yp = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (seglist);
        if (bsuccess == false) {
            csw_Free (xp);
            csw_Free (ip);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of error.
 */
    *xout = NULL;
    *yout = NULL;
    *iout = NULL;
    *nout = 0;

/*
 * Allocate initial space for clipped segments.
 */
    chunksize = npts;
    if (chunksize < 50) chunksize = 50;
    maxseg = chunksize * 2;
    seglist = (CLippedSegment *)csw_Malloc (maxseg * sizeof(CLippedSegment));
    if (seglist == NULL) {
        return -1;
    }

/*
 * Clip each segment to the tri mesh.  Expand the clipped segment
 * space if needed.
 */
    nseg = 0;
    clipseg = NULL;
    nclipseg = 0;
    for (i=0; i<npts-1; i++) {
        istat = ClipSegmentToTriMesh (xline[i], yline[i],
                                      xline[i+1], yline[i+1],
                                      &clipseg, &nclipseg);
        if (istat == -1) {
            return -1;
        }
        if (nseg + nclipseg >= maxseg) {
            while (nseg + nclipseg >= maxseg) {
                maxseg += chunksize;
            }
            seglist = (CLippedSegment *)csw_Realloc (seglist,
                                                 maxseg * sizeof(CLippedSegment));
            if (seglist == NULL) {
                return -1;
            }
        }

        for (j=0; j<nclipseg; j++) {
            memcpy (seglist + nseg,
                    clipseg + j,
                    sizeof(CLippedSegment));
            nseg++;
        }
        csw_Free (clipseg);
        clipseg = NULL;
        nclipseg = 0;
    }

    if (nseg == 0) {
        return 1;
    }

/*
 * Allocate space for the output lines.
 */
    xp = (double *)csw_Malloc (nseg * 4 * sizeof(double));
    if (xp == NULL) {
        return -1;
    }
    yp = xp + nseg * 2;

    ip = (int *)csw_Malloc (nseg * sizeof(int));
    if (ip == NULL) {
        return -1;
    }

/*
 * If segment endpoints graze each other, compress them into
 * the same line.  Put in line breaks where the endpoints do
 * not graze.
 */
    nplast = 0;
    xp[0] = seglist[0].x1;
    yp[0] = seglist[0].y1;
    np = 1;
    nline = 0;

    for (i=0; i<nseg-1; i++) {
        x1 = seglist[i].x2;
        y1 = seglist[i].y2;
        x2 = seglist[i+1].x1;
        y2 = seglist[i+1].y1;
        istat = OutPointGraze (x1, y1, x2, y2);
        if (istat == 1) {
            xp[np] = x1;
            yp[np] = y1;
            np++;
            continue;
        }

        xp[np] = x1;
        yp[np] = y1;
        np++;
        ip[nline] = np - nplast;
        nline++;
        nplast = np;
        xp[np] = x2;
        yp[np] = y2;
        np++;
    }

/*
 * Get the last point of the last segment.
 */
    xp[np] = seglist[nseg-1].x2;
    yp[np] = seglist[nseg-1].y2;
    np++;
    ip[nline] = np - nplast;
    nline++;

    *xout = xp;
    *yout = yp;
    *iout = ip;
    *nout = nline;

    bsuccess = true;

    return 1;

}  /* end of private ClipLineToTriMesh function */



/*
 ************************************************************************************

                  F i n d E d g e C o n t a i n i n g P o i n t

 ************************************************************************************

  Given a triangle number and a point location, return the number of the edge of
  the specified triangle that has the point very nearly on top of it.  If none of
  the triangles edges is very near the point, return -1.

*/


int CSWGrdTriangle::FindEdgeContainingPoint (int trinum, double x, double y)
{
    TRiangleStruct  *tptr;
    EDgeStruct      *eptr;
    double          x1, y1, x2, y2;
    double          xint, yint, dx, dy, dist;
    int             istat;

    tptr = TriangleList + trinum;

    eptr = EdgeList + tptr->edge1;
    x1 = NodeList[eptr->node1].x;
    y1 = NodeList[eptr->node1].y;
    x2 = NodeList[eptr->node2].x;
    y2 = NodeList[eptr->node2].y;
    istat = gpf_perpintpoint2 (x1, y1, x2, y2, x, y, &xint, &yint);
    if (istat == 1) {
        dx = x - xint;
        dy = y - yint;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        if (dist <= XoutTiny) {
            return tptr->edge1;
        }
    }

    eptr = EdgeList + tptr->edge2;
    x1 = NodeList[eptr->node1].x;
    y1 = NodeList[eptr->node1].y;
    x2 = NodeList[eptr->node2].x;
    y2 = NodeList[eptr->node2].y;
    istat = gpf_perpintpoint2 (x1, y1, x2, y2, x, y, &xint, &yint);
    if (istat == 1) {
        dx = x - xint;
        dy = y - yint;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        if (dist <= XoutTiny) {
            return tptr->edge2;
        }
    }

    eptr = EdgeList + tptr->edge3;
    x1 = NodeList[eptr->node1].x;
    y1 = NodeList[eptr->node1].y;
    x2 = NodeList[eptr->node2].x;
    y2 = NodeList[eptr->node2].y;
    istat = gpf_perpintpoint2 (x1, y1, x2, y2, x, y, &xint, &yint);
    if (istat == 1) {
        dx = x - xint;
        dy = y - yint;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        if (dist <= XoutTiny) {
            return tptr->edge3;
        }
    }

    return -1;

}  /* end of private FindEdgeContainingPoint function */





/*
 **************************************************************************************

           g r d _ o u t l i n e _ t r i a n g l e s

 **************************************************************************************

*/

int CSWGrdTriangle::grd_outline_triangles (NOdeStruct *nodes, int num_nodes,
                           EDgeStruct *edges, int num_edges,
                           TRiangleStruct *triangles, int num_triangles,
                           double *xout, double *yout,
                           int *npout, int *ncout, int *nvout,
                           int maxpts, int maxcomps)
{
    int                istat, i, n, *nc1 = NULL, *nv1 = NULL, np1;
    double             *xp1 = NULL, *yp1 = NULL, zdum[3];

    CSWPolyGraph  ply_graph_obj;


    auto fscope = [&]()
    {
        csw_Free (xp1);
        csw_Free (nc1);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Obvious errors in parameters.
 */
    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (num_nodes < 3  ||  num_edges < 3  ||  num_triangles < 1) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (xout == NULL  ||  yout == NULL) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    if (npout == NULL  ||  ncout == NULL  ||  nvout == NULL) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

    if (maxpts < 1  ||  maxcomps < 1) {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

/*
 * Allocate work space.  There will be 4 points in the input polygon array
 * per triangle.  Each triangle is a separate input polygon with one component
 * and 4 vertices (closed triangle) in that single component.
 */
    xp1 = (double *)csw_Malloc (num_triangles * 8 * sizeof(double));
    if (xp1 == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return 1;
    }
    yp1 = xp1 + num_triangles * 4;

    nc1 = (int *)csw_Malloc (num_triangles * 2 * sizeof(int));
    if (nc1 == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    nv1 = nc1 + num_triangles;
    np1 = num_triangles;

    for (i=0; i<np1; i++) {
        nc1[i] = 1;
        nv1[i] = 4;
    }

/*
 * Set up the private file variables for triangle point extraction.
 */
    TriangleList = triangles;
    NumTriangles = num_triangles;
    EdgeList = edges;
    NumEdges = num_edges;
    NodeList = nodes;
    NumNodes = num_nodes;

/*
 * Extract triangle points and put them into the
 * polygon arrays.
 */
    n = 0;
    for (i=0; i<num_triangles; i++) {
        TrianglePoints (TriangleList + i,
                        xp1+n, yp1+n, zdum);
        xp1[n+3] = xp1[n];
        yp1[n+3] = yp1[n];
        n += 4;
    }

/*
 * Combine all the triangles into outline polygons.
 */
    istat = ply_graph_obj.ply_union_components (xp1, yp1, NULL,
                                  np1, nc1, nv1,
                                  xout, yout, NULL,
                                  npout, ncout, nvout,
                                  maxpts, maxcomps);
    return istat;
}





/*
 ***************************************************************************************

              g r d _ c l i p _ t r i _ m e s h _ t o _ p o l y g o n

 ***************************************************************************************

  Delete any nodes, edges or triangles outside the specified polygon.

*/

int CSWGrdTriangle::grd_clip_tri_mesh_to_polygon (NOdeStruct **nodes, int *num_nodes,
                                  EDgeStruct **edges, int *num_edges,
                                  TRiangleStruct **triangles, int *num_triangles,
                                  double *xpoly, double *ypoly,
                                  int *polypoints, int npoly, int flag)

{
    NOdeStruct      *np1 = NULL, *np2 = NULL, *np = NULL;
    double          *xa = NULL, *ya = NULL;
    double          xmid, ymid, tiny;
    double          xmin, ymin, xmax, ymax, zmin, zmax;
    int             i, n, istat;
    int             *inside = NULL, *lookup = NULL, aflag;
    int             do_write;
    double          v6[6];
    char            fname[200];

    CSWPolyUtils             ply_utils_obj;


    auto fscope = [&]()
    {
        ListNull ();
        FreeMem ();
        csw_Free (xa);
        csw_Free (inside);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (npoly < 1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (xpoly == NULL  ||  ypoly == NULL  ||  polypoints == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (*nodes == NULL  ||  *edges == NULL  ||  *triangles == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (*num_nodes < 1  ||  *num_edges < 1  ||  *num_triangles < 1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    aflag = flag;
    if (aflag < 0) aflag = -aflag;
    aflag /= 10;
    if (aflag < 1) aflag = 1;

    if (flag < 0) flag = -1;
    if (flag >= 0) flag = 1;

    xa = (double *)csw_Malloc (2 * *num_edges * sizeof(double));
    if (xa == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    ya = xa + *num_edges;

    inside = (int *)csw_Malloc (2 * *num_edges * sizeof(int));
    if (inside == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    lookup = inside + *num_edges;

    grd_calc_trimesh_bounding_box (*nodes, *num_nodes,
                                   *edges, *num_edges,
                                   *triangles, *num_triangles,
                                   &xmin, &ymin,
                                   &xmax, &ymax,
                                   &zmin, &zmax);
    tiny = (xmax - xmin + ymax - ymin) / 20000.0;
    TriangleList = *triangles;
    NumTriangles = *num_triangles;
    EdgeList = *edges;
    NumEdges = *num_edges;
    NodeList = *nodes;
    NumNodes = *num_nodes;

/*
 * Get the mid points of each edge and remove the edge
 * if the mid point is on the wrong side of the polygon.
 * If the edge is removed, one or both of its end point
 * nodes will also be outside and must be removed also.
 */
    n = 0;
    for (i=0; i<NumEdges; i++) {
        if (EdgeList[i].deleted == 1) {
            continue;
        }
        if (EdgeList[i].node1 < 0  ||  EdgeList[i].node2 < 0) {
            continue;
        }
        np1 = NodeList + EdgeList[i].node1;
        np2 = NodeList + EdgeList[i].node2;
        xmid = (np1->x + np2->x) / 2.0;
        ymid = (np1->y + np2->y) / 2.0;
        xa[n] = xmid;
        ya[n] = ymid;
        lookup[n] = i;
        n++;
    }

    ply_utils_obj.ply_setgraze (tiny * (double)aflag);
    istat = ply_utils_obj.ply_points (xpoly, ypoly, polypoints, npoly,
                        xa, ya, n,
                        inside);
    if (istat != 0) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    for (i=0; i<n; i++) {

        if (inside[i] < 0  &&  flag > 0) {
            WhackEdge (lookup[i]);
            np = NodeList + EdgeList[lookup[i]].node1;
            if (np->deleted == 0) {
                istat = ply_utils_obj.ply_point (xpoly, ypoly, polypoints[0],
                                   np->x, np->y);
                if (istat == -1) {
                    np->deleted = 1;
                }
            }

            np = NodeList + EdgeList[lookup[i]].node2;
            if (np->deleted == 0) {
                istat = ply_utils_obj.ply_point (xpoly, ypoly, polypoints[0],
                                   np->x, np->y);
                if (istat == -1) {
                    np->deleted = 1;
                }
            }
        }
        else if (inside[i] > 0  &&  flag < 0) {
            WhackEdge (lookup[i]);
            np = NodeList + EdgeList[lookup[i]].node1;
            if (np->deleted == 0) {
                istat = ply_utils_obj.ply_point (xpoly, ypoly, polypoints[0],
                                   np->x, np->y);
                if (istat == 1) {
                    np->deleted = 1;
                }
            }
            np = NodeList + EdgeList[lookup[i]].node2;
            if (np->deleted == 0) {
                istat = ply_utils_obj.ply_point (xpoly, ypoly, polypoints[0],
                                   np->x, np->y);
                if (istat == 1) {
                    np->deleted = 1;
                }
            }
        }
    }

    ply_utils_obj.ply_setgraze (0.0);

    do_write = csw_GetDoWrite ();
    if (do_write) {
        sprintf (fname, "preremoveclip.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    RemoveDeletedElements ();

    *num_nodes = NumNodes;
    *num_edges = NumEdges;
    *num_triangles = NumTriangles;
    *nodes = NodeList;
    *edges = EdgeList;
    *triangles = TriangleList;

    return 1;

}  /* end of grd_clip_tri_mesh_to_polygon function */





/*
 ***************************************************************************************

              g r d _ t r i _ m e s h _ f r o m _ g r i d _ p o i n t s

 ***************************************************************************************

*/

int CSWGrdTriangle::grd_tri_mesh_from_grid_points (
            double *x, double *y, double *z, int npts,
            double *xlines, double *ylines, double *zlines,
            int *line_pts, int *line_types, int nlines,
            CSW_F *grid, int ncol, int nrow,
            double xmin, double ymin, double xmax, double ymax,
            NOdeStruct **nodes, int *num_nodes,
            EDgeStruct **edges, int *num_edges,
            TRiangleStruct **triangles, int *num_triangles)
{
    int     i, j, k, n, n1, istat;
    char    *cindex = NULL;
    double  xt, yt, xspace, yspace;
    double  *xa = NULL, *ya = NULL, *za = NULL;


    auto fscope = [&]()
    {
        csw_Free (cindex);
        csw_Free (xa);
    };
    CSWScopeGuard func_scope_guard (fscope);


    cindex = (char *)csw_Calloc (ncol * nrow * sizeof(char));
    if (cindex == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    xspace = (xmax - xmin) / (double)(ncol - 1);
    yspace = (ymax - ymin) / (double)(nrow - 1);

    for (n=0; n<npts; n++) {

        xt = x[n] - xmin;
        if (xt < xmin  ||  xt > xmax) continue;
        yt = y[n] - ymin;
        if (yt < ymin  ||  yt > ymax) continue;
        i = (int)((yt - ymin) / yspace + 0.5);
        if (i < 0) i = 0;
        if (i > nrow-1) i = nrow - 1;
        j = (int)((xt - xmin) / xspace + 0.5);
        if (j < 0) j = 0;
        if (j > ncol-1) j = ncol - 1;

        k = i * ncol + j;
        cindex[k] = 1;

    }

    n = npts + ncol * nrow;

    xa = (double *)csw_Malloc (n * 3 * sizeof(double));
    if (xa == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    ya = xa + n;
    za = ya + n;

    n = 0;
    for (i=0; i<npts; i++) {
        xa[n] = x[i];
        ya[n] = y[i];
        za[n] = z[i];
        n++;
    }

    for (i=0; i<nrow; i++) {
        n1 = i * ncol;
        yt = ymin + i * yspace;
        for (j=0; j<ncol; j++) {
            k = n1 + j;
            if (cindex[k] == 1) continue;
            xt = xmin + j * xspace;
            xa[n] = xt;
            ya[n] = yt;
            za[n] = grid[k];
            n++;
        }
    }

    istat = grd_calc_trimesh (xa, ya, za, n,
                              xlines, ylines, zlines,
                              line_pts, line_types, nlines,
                              nodes, edges, triangles,
                              num_nodes, num_edges, num_triangles);
    return istat;

}  /* end of function grd_tri_mesh_from_grid_points */




/*
 ********************************************************************************

                           S a m e P o i n t

 ********************************************************************************

  If the two specified points are nearly coincident, return 1.  Otherwise,
  return zero.

*/

int CSWGrdTriangle::SamePoint (double x1, double y1, double x2, double y2)
{
    double       dx, dy, dt, tiny;

    tiny = AreaPerimeter / 20000.0;
    dx = x1 - x2;
    dy = y1 - y2;
    dt = dx * dx + dy * dy;
    dt = sqrt (dt);
    if (dt <= tiny) {
        return 1;
    }
    return 0;

}  /*  end of private SamePoint function  */


/*
 ********************************************************************************

                    S a m e P o i n t T i n y

 ********************************************************************************

  If the two specified points are nearly coincident, return 1.  Otherwise,
  return zero.

*/

int CSWGrdTriangle::SamePointTiny (double x1, double y1, double x2, double y2, double tiny)

{
    double       dx, dy, dt;

    dx = x1 - x2;
    dy = y1 - y2;
    dt = dx * dx + dy * dy;
    dt = sqrt (dt);
    if (dt <= tiny) {
        return 1;
    }
    return 0;

}  /*  end of private SamePoint function  */



/*
 **************************************************************************************

          C h e c k F o r C o i n c i d e n t C o n s t r a i n t P o i n t

 **************************************************************************************

  Return the index of a raw constraint point if one is "exactly" at the specified
  location.  If there is no coincident point, -1 is returned.

*/

int CSWGrdTriangle::CheckForCoincidentConstraintPoint (double x, double y)
{

    RAwPointStruct          *rptr;
    int                     i, istat;

    for (i=MaxIndexPoint; i<NumRawCheck; i++) {
        rptr = RawPoints + i;
        istat = SamePoint (x, y, rptr->x, rptr->y);
        if (istat == 1) {
            return i;
        }
    }

    return -1;

}






/*
 **************************************************************************************

                                  L i s t N u l l

 **************************************************************************************

*/

void CSWGrdTriangle::ListNull (void)
{
    if (NodeList) {
        NlistReturn = NodeList;
    }
    if (EdgeList) {
        ElistReturn = EdgeList;
    }
    if (TriangleList) {
        TlistReturn = TriangleList;
    }

    NodeList = NULL;
    EdgeList = NULL;
    TriangleList = NULL;
    NumNodes = 0;
    NumEdges = 0;
    NumTriangles = 0;

    return;

}  /* end of private ListNull function */



void CSWGrdTriangle::TriDebugFunc1 (void)
{
    return;
}



/*
 ***********************************************************************************

    g r d _ c l i p _ t r i _ m e s h _ t o _ m u l t i p l e _ p o l y g o n s

 ***********************************************************************************

*/

int CSWGrdTriangle::grd_clip_tri_mesh_to_multiple_polygons
                                 (NOdeStruct *nodes, int *num_nodes,
                                  EDgeStruct *edges, int *num_edges,
                                  TRiangleStruct *triangles, int *num_triangles,
                                  double *xpoly, double *ypoly,
                                  int *polycomps, int *polypoints, int npoly,
                                  int flag)
{
    NOdeStruct      *np1 = NULL, *np2 = NULL, *np = NULL;
    double          *xa = NULL, *ya = NULL;
    double          xmid, ymid, tiny;
    double          xmin, ymin, xmax, ymax, zmin, zmax;
    int             i, j, k, n, nc, npts, istat, inside1, outsum1;
    int             *inside = NULL, *outside_sum = NULL, *lookup = NULL;

    CSWPolyUtils             ply_utils_obj;


    auto fscope = [&]()
    {
        ListNull ();
        FreeMem ();
        csw_Free (inside);
        csw_Free (xa);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (npoly < 1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (xpoly == NULL  ||  ypoly == NULL  ||  polypoints == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (*num_nodes < 1  ||  *num_edges < 1  ||  *num_triangles < 1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (flag < 0) flag = -1;
    if (flag >= 0) flag = 1;

    xa = (double *)csw_Malloc (2 * *num_edges * sizeof(double));
    if (xa == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    ya = xa + *num_edges;

    inside = (int *)csw_Calloc (3 * *num_edges * sizeof(int));
    if (inside == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    lookup = inside + *num_edges;
    outside_sum = lookup + *num_edges;

    grd_calc_trimesh_bounding_box (nodes, *num_nodes,
                                   edges, *num_edges,
                                   triangles, *num_triangles,
                                   &xmin, &ymin,
                                   &xmax, &ymax,
                                   &zmin, &zmax);
    tiny = (xmax - xmin + ymax - ymin) / 20000.0;
    TriangleList = triangles;
    NumTriangles = *num_triangles;
    EdgeList = edges;
    NumEdges = *num_edges;
    NodeList = nodes;
    NumNodes = *num_nodes;

/*
 * Get the mid points of each edge.
 */
    n = 0;
    for (i=0; i<*num_edges; i++) {
        if (edges[i].deleted == 1) {
            continue;
        }
        if (edges[i].node1 < 0  ||  edges[i].node2 < 0) {
            continue;
        }
        np1 = nodes + edges[i].node1;
        np2 = nodes + edges[i].node2;
        xmid = (np1->x + np2->x) / 2.0;
        ymid = (np1->y + np2->y) / 2.0;
        xa[n] = xmid;
        ya[n] = ymid;
        lookup[n] = i;
        n++;
    }
    npts = n;

/*
 * Check if the midpoint is outside of all polygons.
 */
    ply_utils_obj.ply_setgraze (tiny);
    n = 0;
    nc = 0;
    for (i=0; i<npoly; i++) {
        istat = ply_utils_obj.ply_points (xpoly+n, ypoly+n,
                            polypoints+nc, polycomps[i],
                            xa, ya, npts,
                            inside);
        if (istat != 0) {
            ply_utils_obj.ply_setgraze (0.0);
            grd_utils_ptr->grd_set_err (3);
            return -1;
        }
        for (j=0; j<polycomps[i]; j++) {
            n += polypoints[nc];
            nc++;
        }
        for (j=0; j<npts; j++) {
            if (inside[j] == -1) {
                outside_sum[j]++;
            }
        }
    }

/*
 * Delete the edge and one or more nodes if the midpoint
 * was outside all (for flag = 1) or inside at least one
 * polygon (if flag = -1).
 */
    for (i=0; i<npts; i++) {
        if (outside_sum[i] >= npoly  &&  flag > 0) {
            WhackEdge (lookup[i]);
            np = NodeList + EdgeList[lookup[i]].node1;
            n = 0;
            nc = 0;
            outsum1 = 0;
            for (j=0; j<npoly; j++) {
                istat = ply_utils_obj.ply_points (xpoly+n, ypoly+n,
                                    polypoints+nc, polycomps[j],
                                    &np->x, &np->y, 1,
                                    &inside1);
                if (istat != 0) {
                    grd_utils_ptr->grd_set_err (3);
                    ply_utils_obj.ply_setgraze (0.0);
                    return -1;
                }
                for (k=0; k<polycomps[j]; k++) {
                    n += polypoints[nc];
                    nc++;
                }
                if (inside1 == -1) {
                    outsum1++;
                }
            }
            if (outsum1 >= npoly) {
                np->deleted = 1;
            }
            np = NodeList + EdgeList[lookup[i]].node2;
            n = 0;
            nc = 0;
            outsum1 = 0;
            for (j=0; j<npoly; j++) {
                istat = ply_utils_obj.ply_points (xpoly+n, ypoly+n,
                                    polypoints+nc, polycomps[j],
                                    &np->x, &np->y, 1,
                                    &inside1);
                if (istat != 0) {
                    grd_utils_ptr->grd_set_err (3);
                    ply_utils_obj.ply_setgraze (0.0);
                    return -1;
                }
                for (k=0; k<polycomps[j]; k++) {
                    n += polypoints[nc];
                    nc++;
                }
                if (inside1 == -1) {
                    outsum1++;
                }
            }
            if (outsum1 >= npoly) {
                np->deleted = 1;
            }
        }
        else if (outside_sum[i] < npoly  &&  flag < 0) {
            WhackEdge (lookup[i]);
            np = NodeList + EdgeList[lookup[i]].node1;
            n = 0;
            nc = 0;
            for (j=0; j<npoly; j++) {
                istat = ply_utils_obj.ply_points (xpoly+n, ypoly+n,
                                    polypoints+nc, polycomps[j],
                                    &np->x, &np->y, 1,
                                    &inside1);
                if (istat != 0) {
                    grd_utils_ptr->grd_set_err (3);
                    ply_utils_obj.ply_setgraze (0.0);
                    return -1;
                }
                if (inside1 == 1) {
                    np->deleted = 1;
                    break;
                }
                for (j=0; j<polycomps[j]; j++) {
                    n += polypoints[nc];
                    nc++;
                }
            }
            np = NodeList + EdgeList[lookup[i]].node2;
            n = 0;
            nc = 0;
            for (j=0; j<npoly; j++) {
                istat = ply_utils_obj.ply_points (xpoly+n, ypoly+n,
                                    polypoints+nc, polycomps[j],
                                    &np->x, &np->y, 1,
                                    &inside1);
                if (istat != 0) {
                    grd_utils_ptr->grd_set_err (3);
                    ply_utils_obj.ply_setgraze (0.0);
                    return -1;
                }
                if (inside1 == 1) {
                    np->deleted = 1;
                    break;
                }
                for (j=0; j<polycomps[j]; j++) {
                    n += polypoints[nc];
                    nc++;
                }
            }
        }
    }

    ply_utils_obj.ply_setgraze (0.0);

    RemoveDeletedElements ();

    *num_nodes = NumNodes;
    *num_edges = NumEdges;
    *num_triangles = NumTriangles;

    return 1;

}  /* end of function grd_clip_tri_mesh_to_multiple_polygons */




int CSWGrdTriangle::IsConstraintPoint (double x, double y,
                              double *xlines, double *ylines, int nltot)
{
    int            i, istat;

    for (i=0; i<nltot; i++) {
        istat = SamePoint (x, y, xlines[i], ylines[i]);
        if (istat == 1) {
            return 1;
        }
    }

    return 0;

}  /* end of private IsConstraintPoint function */




/*
 *************************************************************************

              g r d _ f i l t e r _ d a t a _ s p i k e s

 *************************************************************************

*/

int CSWGrdTriangle::grd_filter_data_spikes (double *xin, double *yin, double *zin,
                            int *ibad, int nin,
                            SPikeFilterOptions *options)
{
    POintCollection     *plist, *ptr;
    NOdeStruct          *nodes;
    EDgeStruct          *edges, *eptr;
    TRiangleStruct      *triangles;
    double              xloc[MAX_POINTS_IN_COLLECTION],
                        yloc[MAX_POINTS_IN_COLLECTION],
                        zloc[MAX_POINTS_IN_COLLECTION];
    double              sum, avgerr, zt, errmult, coef[3];
    double              max_dist, fact, maybemult,
                        lfact, sfact;
    int                 i, j, jj, istat, n1, n2,
                        *ilist, n, nloc, nlong,
                        num_nodes, num_edges, num_triangles;

/*
 * Triangulate the points without any post triangulation
 * removal of discarded edges.
 */
    _RemoveFlag = 0;
    num_nodes = num_edges = num_triangles = 0;
    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    istat = grd_calc_trimesh (xin, yin, zin, nin,
                              NULL, NULL, NULL,
                              NULL, NULL, 0,
                              &nodes, &edges, &triangles,
                              &num_nodes, &num_edges, &num_triangles);
    _RemoveFlag = 1;
    if (istat == -1) {
        return -1;
    }

    NodeList = nodes;
    EdgeList = edges;
    TriangleList = triangles;
    NumNodes = num_nodes;
    NumEdges = num_edges;
    NumTriangles = num_triangles;

/*
 * Create a list of every edge connected at each node.
 */
    istat = BuildNodeEdgeLists ();
    if (istat == -1) {
        FreeMem ();
        return -1;
    }

/*
 * Allocate space for the local point collections.
 */
    plist = (POintCollection *)csw_Calloc
        (NumNodes * sizeof(POintCollection));
    if (plist == NULL) {
        FreeMem ();
        return -1;
    }

    max_dist = -1.0;
    if (options != NULL) {
        max_dist = options->max_dist;
    }
    if (max_dist < 0.0) {
        max_dist = AreaPerimeter;
    }

/*
 * Populate the local point collections with the node
 * from the edge that is not the ith node.  If neither
 * node on the edge is the ith node, this is a program
 * bug, so assert is done.
 */
    for (i=0; i<NumNodes; i++) {
        ptr = plist + i;
        ilist = NodeEdgeList[i].list;
        nloc = NodeEdgeList[i].nlist;
        if (nloc > MAX_POINTS_IN_COLLECTION) {
            nloc = MAX_POINTS_IN_COLLECTION;
        }
        n = 0;
        nlong = 0;
        for (j=0; j<nloc; j++) {
            eptr = EdgeList + ilist[j];
            if (eptr->deleted == 1) {
                continue;
            }
            n1 = eptr->node1;
            n2 = eptr->node2;
            if (n1 == i  ||  n2 == i) {
                if (n1 != i) {
                    ptr->points[n] = n1;
                }
                else {
                    ptr->points[n] = n2;
                }
            }
            else {
                assert (0);
            }
            n++;
            if (eptr->length > max_dist) {
                nlong++;
            }
        }
        ptr->npts = n;
        ptr->nlong = nlong;

    }

/*
 * The errmult variable determines how much worse than the
 * average error that the point must be to be deemed bad.
 * Smaller errmult will flag more bad points.
 */
    fact = ((double)NumNodes) / 5000.0;
    errmult = 5.0;
    if (fact > 1.0) {
        errmult = 5.0 / fact;
    }
    if (errmult < 3.0) errmult = 3.0;
    if (options != NULL) {
        sfact = options->smoothing_factor;
        if (sfact < 1.0  ||  sfact > 5.0) sfact = 3.0;
        errmult = 15.0 / sfact;
    }
    maybemult = errmult / 1.5;
    if (maybemult < 2.0) maybemult = 2.0;

/*
 * Initialize to all good points.
 */
    memset (ibad, 0, NumNodes * sizeof(int));

/*
 * Fit plane to each local collection.  Find the average
 * error of the points used to calculate the plane.
 * If the target point has an error much larger than that
 * then the target point is flagged as bad.
 */
    for (i=0; i<NumNodes; i++) {
        ptr = plist + i;
        if (ptr->npts < 4) {
            continue;
        }
        nloc = ptr->npts;
        ilist = ptr->points;
        for (j=0; j<nloc; j++) {
            jj = ilist[j];
            xloc[j] = xin[jj];
            yloc[j] = yin[jj];
            zloc[j] = zin[jj];
        }
        istat = grd_CalcPreciseDoublePlane (xloc, yloc, zloc, nloc, coef);
        if (istat != 1) {
            continue;
        }

    /*
     * evaluate plane at local points.
     */
        sum = 0.0;
        for (j=0; j<nloc; j++) {
            zt = coef[0] + coef[1] * xloc[j] + coef[2] * yloc[j];
            zt -= zloc[j];
            if (zt < 0.0) {
                zt = -zt;
            }
            sum += zt;
        }
        avgerr = sum / (double)nloc;

    /*
     * evaluate plane at the target (ith) point.
     */
        zt = coef[0] + coef[1] * xin[i] + coef[2] * yin[i];
        zt -= zin[i];
        if (zt < 0.0) {
            zt = -zt;
        }
        lfact = (double)ptr->nlong / (double)(ptr->npts);
        lfact *= 2.0;
        if (lfact < 1.0) lfact = 1.0;
        if (zt > avgerr * errmult * lfact) {
            ibad[i] = 1;
        }
        else if (zt > avgerr * maybemult * lfact) {
            ibad[i] = 2;
        }
    }

    FreeMem ();
    csw_Free (plist);

    return 1;

}  /* end of function grd_filter_data_spikes */




/*
 **************************************************************************

                    S p l i t L o n g E d g e s

 **************************************************************************

*/

int CSWGrdTriangle::SplitLongEdges (void)
{
    EDgeStruct         *eptr;
    NOdeStruct         *np1, *np2;
    double             x1, y1, z1, x2, y2, z2;
    double             xt, yt, zt, dist, mindist;
    double             sum, dmin, dmax, elen, split;
    int                i, n1, n2, flag1, flag2, flag,
                       ndone, ndo, istat, nodenum;
    int                nedge;

    if (NumEdges < 3) {
        return 1;
    }

/*
 * Find the average edge length of all original edges.
 */
    sum = 0.0;
    dmin = 1.e30;
    dmax = -1.e30;
    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->node1 < 4  ||  eptr->node2 < 4) {
            continue;
        }
        elen = eptr->length;
        sum += elen;
        if (elen < dmin) dmin = elen;
        if (elen > dmax) dmax = elen;
    }
    sum /= (double)NumEdges;
    elen = (dmin + dmax) / 2.0;
    if (elen < sum) {
        sum = elen;
    }

    split = (double)SplitLongLength;
    if (split < 1.0) split = 1.0;
    mindist = sum / split;
    if (mindist > AreaPerimeter / 8.0) {
        mindist = AreaPerimeter / 8.0;
    }

/*
 * Split edges until no edge is longer than mindist.
 */
    ndo = 0;
    for (;;) {

        ndone = 0;
        nedge = NumEdges;

    /*
     * First, split constraint edges.
     */
        for (i=0; i<nedge; i++) {

            eptr = EdgeList + i;
            if (eptr->flag == 0) {
                continue;
            }
            if (eptr->deleted == 1) {
                continue;
            }
            dist = eptr->length;
            if (dist <= mindist) {
                continue;
            }

            n1 = eptr->node1;
            n2 = eptr->node2;

        /*
         * Corner edges have a node numbered
         * less than 4 and they should not be split.
         */
            if (n1 < 4  ||  n2 < 4) {
                continue;
            }

            np1 = NodeList + n1;
            np2 = NodeList + n2;

            flag1 = np1->flag;
            flag2 = np2->flag;

            x1 = np1->x;
            y1 = np1->y;
            z1 = np1->z;
            x2 = np2->x;
            y2 = np2->y;
            z2 = np2->z;

            xt = (x1 + x2) / 2.0;
            yt = (y1 + y2) / 2.0;
            zt = (z1 + z2) / 2.0;

            flag = 0;
            if (flag1 == flag2) {
                flag = flag1;
            }

            nodenum = AddNode (xt, yt, zt, flag);
            if (nodenum == -1) {
                return -1;
            }

            istat = AddConstraintRawPoint (NodeList + nodenum,
                                           &i, 1);
            if (istat == -1) {
                return -1;
            }

            UseCornerFlag = 1;
            istat = SplitFromEdge (i, nodenum, NULL);
            UseCornerFlag = 0;
            if (istat == -1) {
                return -1;
            }

            ndone++;
        }

    /*
     * Next, split all original non constraint edges.
     */
        for (i=0; i<nedge; i++) {

            eptr = EdgeList + i;
            if (eptr->flag == 1) {
                continue;
            }
            if (eptr->deleted == 1) {
                continue;
            }
            dist = eptr->length;
            if (dist <= mindist) {
                continue;
            }

            n1 = eptr->node1;
            n2 = eptr->node2;

        /*
         * Corner edges have a node numbered
         * less than 4 and they should not be split.
         */
            if (n1 < 4  ||  n2 < 4) {
                continue;
            }

            np1 = NodeList + n1;
            np2 = NodeList + n2;

            flag1 = np1->flag;
            flag2 = np2->flag;

            x1 = np1->x;
            y1 = np1->y;
            z1 = np1->z;
            x2 = np2->x;
            y2 = np2->y;
            z2 = np2->z;

            xt = (x1 + x2) / 2.0;
            yt = (y1 + y2) / 2.0;
            zt = (z1 + z2) / 2.0;

            flag = 0;
            if (flag1 == flag2) {
                flag = flag1;
            }

            nodenum = AddNode (xt, yt, zt, flag);
            if (nodenum == -1) {
                return -1;
            }

            istat = AddConstraintRawPoint (NodeList + nodenum,
                                           &i, 1);
            if (istat == -1) {
                return -1;
            }

            UseCornerFlag = 1;
            istat = SplitFromEdge (i, nodenum, NULL);
            UseCornerFlag = 0;
            if (istat == -1) {
                return -1;
            }

            ndone++;
        }

    /*
     * swap edges to make more equilateral.
     */
        for (i=0; i<nedge; i++) {
            eptr = EdgeList + i;
            if (eptr->deleted == 1) {
                continue;
            }
            if (eptr->flag != 0) {
                continue;
            }
            SwapEdge (i);
        }
        if (ndone < 1) {
            break;
        }
        if (ndo > 4) {
            break;
        }

        ndo++;

    }

    return 1;

}  /* end of private SplitLongEdges function */



/*
 ******************************************************************************

                           R e m o v e E d g e

 ******************************************************************************

  Remove the specified edge by deleting one of its nodes and connecting
  the rest of the edges from the deleted node to the other node that wasn't
  deleted.

*/

#if 0
int CSWGrdTriangle::RemoveEdge (int edgenum)

{
    EDgeStruct       *epin;

    if (edgenum < 0  ||  edgenum >= NumEdges) {
        return -1;
    }

    epin = EdgeList + edgenum;
    if (epin->deleted == 1) {
        return 1;
    }

    RemoveNode (epin->node1);
    epin->deleted = 1;

    return 1;

}
#endif



/*
 *************************************************************************************

                  C h e c k T r i a n g l e N o d e G r a z e

 *************************************************************************************

*/

int CSWGrdTriangle::CheckTriangleNodeGraze (int itri, double x1, double y1)

{
    TRiangleStruct    *tptr;
    double            xtri[3], ytri[3], ztri[3];

    if (TriangleList == NULL) {
        return 0;
    }

    if (itri < 0) {
        return 0;
    }

    tptr = TriangleList + itri;
    TrianglePoints (tptr,
                    xtri, ytri, ztri);

    if (OutPointGraze (x1, y1, xtri[0], ytri[0]) == 1) {
        return 1;
    }

    if (OutPointGraze (x1, y1, xtri[1], ytri[1]) == 1) {
        return 1;
    }

    if (OutPointGraze (x1, y1, xtri[2], ytri[2]) == 1) {
        return 1;
    }

    return 0;

}

/*
  ****************************************************************************

                    C r e a t e T r i a n g l e I n d e x G r i d

  ****************************************************************************

  Create a grid of INdexStruct pointers.  Each cell in the grid has a list
  of all the points that lie inside the cell.

*/

int CSWGrdTriangle::CreateTriangleIndexGrid (void)
{
    int              i, n, nc, nr, istat;
    int              i1, i2, j1, j2, ii, jj, kk, iioffset;
    double           w, h, a, tmp;
    double           xmin, ymin, xmax, ymax, xt, yt;
    double           x1, y1, x2, y2;
    INdexStruct      *iptr;

/*
 *  Calculate the bounding box of the nodes.
 *  This is the same as the triangle bounding box.
 */
    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;

    for (i=0; i<NumNodes; i++) {
        xt = NodeList[i].x;
        yt = NodeList[i].y;
        if (xt < xmin) xmin = xt;
        if (yt < ymin) ymin = yt;
        if (xt > xmax) xmax = xt;
        if (yt > ymax) ymax = yt;
    }

    xt = (xmax - xmin + ymax - ymin) / 200.0;
    if (xt <= 0.0) {
        xt = xmin / 1000.0;
        if (xt <= 0.0) xt = 1.0;
    }
    xmin -= xt;
    ymin -= xt;
    xmax += xt;
    ymax += xt;

/*
    Calculate the grid geometry.
*/
    n = NumNodes * 2;
    if (n < 100) n = 100;
    w = xmax - xmin;
    h = ymax - ymin;
    a = h / w;
    tmp = sqrt ((double)n / a);
    nc = (int)(tmp + .5);
    tmp = (double)nc * a;
    nr = (int)(tmp + .5);
    nc++;
    nr++;

    TriangleIndexXmin = xmin;
    TriangleIndexYmin = ymin;
    TriangleIndexXmax = xmax;
    TriangleIndexYmax = ymax;
    TriangleIndexNcol = nc;
    TriangleIndexNrow = nr;
    TriangleIndexXspace = (xmax - xmin) / (double)(nc - 1);
    TriangleIndexYspace = (ymax - ymin) / (double)(nr - 1);

/*
    Allocate space, initialized as NULLs.
*/
    TriangleIndexGrid = (INdexStruct **)csw_Calloc (nc*nr*sizeof(INdexStruct*));
    if (!TriangleIndexGrid) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Add each triangle to its appropriate cell in the index grid.
    This is based on the bounding box of each triangle.
*/
    for (n=0; n<NumTriangles; n++) {

        TriangleBounds (n, &x1, &y1, &x2, &y2);
        if (x1 >= x2  ||  y1 >= y2) {
            continue;
        }

        j1 = (int)((x1 - TriangleIndexXmin) / TriangleIndexXspace);
        j2 = (int)((x2 - TriangleIndexXmin) / TriangleIndexXspace);
        if (j1 < 0) j1 = 0;
        if (j1 >= TriangleIndexNcol) j1 = TriangleIndexNcol - 1;
        if (j2 < 0) j2 = 0;
        if (j2 >= TriangleIndexNcol) j2 = TriangleIndexNcol - 1;

        i1 = (int)((y1 - TriangleIndexYmin) / TriangleIndexYspace);
        i2 = (int)((y2 - TriangleIndexYmin) / TriangleIndexYspace);
        if (i1 < 0) i1 = 0;
        if (i1 >= TriangleIndexNrow) i1 = TriangleIndexNrow - 1;
        if (i2 < 0) i2 = 0;
        if (i2 >= TriangleIndexNrow) i2 = TriangleIndexNrow - 1;

        for (ii=i1; ii<=i2; ii++) {
            iioffset = ii * TriangleIndexNcol;
            for (jj=j1; jj<=j2; jj++) {
                kk = iioffset + jj;
                iptr = TriangleIndexGrid[kk];
                if (!iptr) {
                    iptr = (INdexStruct *)csw_Calloc (sizeof(INdexStruct));
                    if (!iptr) {
                        grd_utils_ptr->grd_set_err (1);
                        return -1;
                    }
                    TriangleIndexGrid[kk] = iptr;
                }
                istat = AddIndexTriangle (iptr, n);
                if (istat == -1) {
                    return -1;
                }
            }
        }

    }

    return 1;

}  /*  end of private CreateTriangleIndexGrid function  */




/*
  ****************************************************************************

                        A d d I n d e x T r i a n g l e

  ****************************************************************************

    Add a triangle number to the specifed index structure, expanding its list if needed.

*/

int CSWGrdTriangle::AddIndexTriangle (INdexStruct *iptr, int np)
{
    int                i, nt, n, m, *list;

    n = iptr->npts;
    m = iptr->max;
    list = iptr->list;
    if (n >= m) {
        m += TRI_INDEX_CHUNK;
        list = (int *)csw_Realloc (list, m * sizeof(int));
        if (!list) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        iptr->max = m;
        iptr->list = list;
    }

    for (i=0; i<n; i++) {
        nt = iptr->list[i];
        if (nt == np) {
            return 1;
        }
    }

    list[n] = np;
    iptr->npts++;

    return 1;

}  /*  end of private AddIndexTriangle function  */





/*
 **************************************************************************************

           g r d _ o u t l i n e _ t r i _ m e s h _ b o u n d a r y

 **************************************************************************************

*/

int CSWGrdTriangle::grd_outline_tri_mesh_boundary (
                           NOdeStruct *nodes, int num_nodes,
                           EDgeStruct *edges, int num_edges,
                           TRiangleStruct *triangles, int num_triangles,
                           double *xout, double *yout, double *zout,
                           int *nodeout,
                           int *npout, int *ncout, int *nvout,
                           int maxpts, int maxcomps)

{
    NOdeEdgeListStruct         *nptr = NULL;
    EDgeStruct                 *eptr = NULL;
    NOdeStruct                 *nodeptr = NULL;
    int                        *list = NULL, nlist, firstedge,
                               prevedge, nextedge, i, n;
    int                        n1, n2, nodenum, ifirst;
    int                        *boundary_node_list = NULL, num_boundary_node_list;
    double                     xt, yt, zt;
    int                        ncomp;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (boundary_node_list);
        if (bsuccess == false) {
            ListNull ();
            FreeMem ();
        }
        else {
            if (DontFree == 0) {
                ListNull ();
                FreeMem ();
            }
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Obvious errors in parameters.
 */
    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (num_nodes < 3  ||  num_edges < 3  ||  num_triangles < 1) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    if (xout == NULL  ||  yout == NULL) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    if (npout == NULL  ||  ncout == NULL  ||  nvout == NULL) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

    if (maxpts < 1  ||  maxcomps < 1) {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

    *npout = 0;

    TriangleList = triangles;
    EdgeList = edges;
    NodeList = nodes;
    NumTriangles = num_triangles;
    NumEdges = num_edges;
    NumNodes = num_nodes;

    BuildNodeEdgeLists ();

/*
 * Its possible for a boundary to have one more node than the total
 * number of nodes in the tri mesh, because the first and last nodes
 * of the boundary are repeated to close the polygon.
 */
    boundary_node_list = (int *)csw_Malloc ((NumNodes + 1) * sizeof(int));
    if (boundary_node_list == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    num_boundary_node_list = 0;

/*
 * Initialize edge used flags to zero (not used)
 */
    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        eptr->tflag = 0;
    }

/*
 * There can be several boundary lines for the tri mesh if there are
 * holes in the mesh.  Each boundary line is found by locating an
 * edge that only has one triangle and then tracing the boundary from
 * node to node starting with that edge.  When a boundary is made, the
 * edges used in it are flagged so they won't be used in a subsequent
 * boundary.  The process is repeated until no unflagged boundary
 * edges remain.
 */

    ncomp = 0;
    n = 0;
    ifirst = 0;
    for (;;) {

    /*
     * Find an edge that has only one triangle.
     */
        eptr = NULL;
        firstedge = -1;
        for (i=ifirst; i<NumEdges; i++) {
            eptr = EdgeList + i;
            if (eptr->tflag == 1) {
                continue;
            }
            if (eptr->tri1 >= 0  &&  eptr->tri2 < 0) {
                firstedge = i;
                break;
            }
        }

    /*
     * If no suitable edge was found, we are done.
     */
        if (firstedge == -1  ||  eptr == NULL) {
            break;
        }

        ifirst = i + 1;

        eptr->tflag = 1;
        nextedge = firstedge;
        prevedge = firstedge;
        nodenum = eptr->node1;
        boundary_node_list[0] = nodenum;
        num_boundary_node_list = 1;

    /*
     * Starting arbitrarily with the first node of the edge found,
     * find another edge connected to the node that also only has
     * one triangle.  Repeat until we get back to the original edge.
     */
        for (;;) {

            eptr = EdgeList + nextedge;
            n1 = eptr->node1;
            n2 = eptr->node2;
            if (nodenum == n1) {
                nodenum = n2;
            }
            else {
                nodenum = n1;
            }

            boundary_node_list[num_boundary_node_list] = nodenum;
            num_boundary_node_list++;

            assert (num_boundary_node_list <= NumNodes + 1);

            nptr = NodeEdgeList + nodenum;
            list = nptr->list;
            nlist = nptr->nlist;

            nextedge = -1;
            for (i=0; i<nlist; i++) {
                if (list[i] == prevedge) {
                    continue;
                }
                eptr = EdgeList + list[i];
                if (eptr->tri1 >= 0  &&  eptr->tri2 < 0) {
                    if (list[i] == firstedge) {
                        nextedge = firstedge;
                        break;
                    }
                    if (eptr->tflag == 0) {
                        nextedge = list[i];
                        eptr->tflag = 1;
                        break;
                    }
                }
            }

            if (nextedge == firstedge) {
                break;
            }

        /*
         * If no next edge was found, there is a problem with
         * the tri mesh topology and an error is returned.
         */
            if (nextedge == -1) {
                grd_utils_ptr->grd_set_err (7);
                return -1;
            }

            prevedge = nextedge;

        }  /* end of single boundary loop */

    /*
     * Append this boundary to the output.
     */
        for (i=0; i<num_boundary_node_list; i++) {
            nodeptr = NodeList + boundary_node_list[i];
            xt = nodeptr->x;
            yt = nodeptr->y;
            zt = nodeptr->z;
            xout[n] = xt;
            yout[n] = yt;
            zout[n] = zt;
            nodeout[n] = boundary_node_list[i];
            n++;
            if (n >= maxpts) {
                break;
            }
        }
        nvout[ncomp] = num_boundary_node_list;
        ncomp++;
        if (ncomp >= maxcomps) {
            break;
        }
        if (n >= maxpts) {
            break;
        }

    }  /* end of multiple boundary loop */

    *npout = 1;
    ncout[0] = ncomp;

    bsuccess = true;

    return 1;
}





/*
  ****************************************************************************

                     S w a p C o r n e r E d g e

  ****************************************************************************

*/

int CSWGrdTriangle::SwapCornerEdge (int edgenum)
{
    TRiangleStruct         *tp1, *tp2;
    EDgeStruct             *ep, *ep3, *ep4, *eptmp;
    int                    e1, e2, e3, e4,
                           n1, n2, n3, n4,
                           tnum1, tnum2, istat, swapit;
    double                 xint, yint;

    CSWPolyUtils             ply_utils_obj;

/*
    Do nothing if the edge has only one triangle using it
*/
    ep = EdgeList + edgenum;
    if (ep->deleted == 1) {
        return 1;
    }
    if (ep->tri2 == -1) {
        return 1;
    }

/*
    Find the two edges of the first triangle that are not the specified edge.
*/
    tp1 = TriangleList + ep->tri1;
    tp2 = TriangleList + ep->tri2;

    e1 = e2 = e3 = e4 = -1;

    if (tp1->edge1 != edgenum) {
        e1 = tp1->edge1;
    }
    if (tp1->edge2 != edgenum) {
        if (e1 == -1) {
            e1 = tp1->edge2;
        }
        else {
            e2 = tp1->edge2;
        }
    }
    if (tp1->edge3 != edgenum) {
        e2 = tp1->edge3;
    }

/*
    Find the nodes of the first triangle, in the same order as the edges.
*/
    n1 = CommonNode (edgenum, e1);
    n2 = CommonNode (e1, e2);
    n3 = CommonNode (e2, edgenum);

/*
    Find the edges of the second triangle that are not the diagonal and
    find the remaining node of the quadrilateral.
*/
    if (tp2->edge1 != edgenum) {
        e3 = tp2->edge1;
    }
    if (tp2->edge2 != edgenum) {
        if (e3 == -1) {
            e3 = tp2->edge2;
        }
        else {
            e4 = tp2->edge2;
        }
    }
    if (tp2->edge3 != edgenum) {
        e4 = tp2->edge3;
    }

    if (e1 == -1 ||  e2 == -1  ||  e3 == -1  ||  e4 == -1) {
        printf ("\nbad edges in swap corner edge\n");
        dumpEdgeInfo (edgenum);
        assert (0);
    }

    ep3 = EdgeList + e3;
    ep4 = EdgeList + e4;

    n4 = CommonNode (e3, e4);

    if (n1 < 0  ||  n2 < 0  ||  n3 < 0  ||  n4 < 0) {
        return 0;
    }

/*
    The edge e3 must be connected to edge e2.  If not, swap
    e3 and e4 edges.  When this is done, the edges e1, e2, e3, to e4
    represent a continuous perimeter of the quadrilateral.  The nodes
    n1, n2, n3, and n4 are the nodes of the perimeter, in order.
*/
    if (CommonNode (e2, e3) == -1) {
        eptmp = ep3;
        ep3 = ep4;
        ep4 = eptmp;
        e3 = ep3 - EdgeList;
        e4 = ep4 - EdgeList;
    }


/*
    If the two possible diagonal segments do not intersect,
    they cannot be swapped.
*/
    istat = ply_utils_obj.ply_segint (NodeList[n1].x, NodeList[n1].y,
                        NodeList[n3].x, NodeList[n3].y,
                        NodeList[n2].x, NodeList[n2].y,
                        NodeList[n4].x, NodeList[n4].y,
                        &xint, &yint);
    if (istat != 0) {
        return 0;
    }

/*
 * Only check the current diagonal of the quadralateral to see
 * if it uses a corner point.  If it does, and the alternate
 * diagonal does not use a corner point, swap to the alternate.
 */
    swapit = 0;
    if (n1 < 4  ||  n3 < 4) {
        if (n2 > 3  &&  n4 > 3) {
            swapit = 1;
        }
    }

    if (swapit == 0) {
        return 0;
    }

/*
    The old diagonal was by definition from n1 to n3, so the
    new diagonal must be from n2 to n4.
*/
    ep->node1 = n2;
    ep->node2 = n4;
    ep->length = NodeDistance (n2, n4);

/*
    Remove the edge from the edge lists of n1 and n3, also
    add the edge to the n2 and n4 node lists.
*/
    if (RawPoints != NULL) {
        RemoveEdgeFromNodeList (n1, edgenum);
        RemoveEdgeFromNodeList (n3, edgenum);
        AddEdgeToNodeList (n2, edgenum);
        AddEdgeToNodeList (n4, edgenum);
    }

/*
    Update the triangles with their new edges.
*/
    tp1->edge1 = e1;
    tp1->edge2 = edgenum;
    tp1->edge3 = e4;

    tp2->edge1 = e2;
    tp2->edge2 = e3;
    tp2->edge3 = edgenum;

/*
    Update the triangles for the edges.
*/
    tnum1 = tp1 - TriangleList;
    tnum2 = tp2 - TriangleList;
    eptmp = EdgeList + e4;
    if (eptmp->tri1 == tnum2) {
        eptmp->tri1 = tnum1;
    }
    else {
        eptmp->tri2 = tnum1;
    }
    eptmp = EdgeList + e2;
    if (eptmp->tri1 == tnum1) {
        eptmp->tri1 = tnum2;
    }
    else {
        eptmp->tri2 = tnum2;
    }

    if (FinalSwapFlag == 1) {
        if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
        if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
        if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
        if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
        SwapFlags[edgenum] = 2;
        NumSwapped++;
    }

    return 1;

}  /*  end of private SwapCornerEdge function  */





/*
 ************************************************************************************

 Find the closest grid corner node that is within 1 average grid spacing
 of the specified point.  If any grid corner nodes have already been locked
 into place by previous constraint processing, that node is only returned
 if it is within 1/1000th grid spacing of the specified point, or essentially
 in the same location as the specified point.
*/

int CSWGrdTriangle::FindExistingGridCornerNode (double xt, double yt, double zt, int checkFan)
{
    int         istat, irow, jcol, k, kmin;
    double      xn, ynn, dx, dy, dist, dmin, dcrit;
    NOdeStruct  *np;
    double      tiny;

    if (PolygonalizeConstraintFlag == 1) {
        return -1;
    }

/*
 * If not in grid to trimesh mode, always return -1.
 */
    if (GridNcol < 2  ||  GridNrow < 2) {
        istat = FindExistingTriangleNode (xt, yt, zt, checkFan);
        return istat;
    }

    dmin = 1.e30;
    dcrit = GridXspace / 2.1;
    if (GridYspace < GridXspace) {
        dcrit = GridYspace / 2.1;
    }
    if (StaticCriticalDistance > 0.0) {
        dcrit = StaticCriticalDistance;
    }
    kmin = -1;
    tiny = dcrit / 1000.0;

    irow = (int)((yt - GridY1) / GridYspace);
    jcol = (int)((xt - GridX1) / GridXspace);

    if (irow < 0  ||  jcol < 0) {
        return -1;
    }

    if (irow > GridNrow - 2  ||  jcol > GridNcol - 2) {
        return -1;
    }

    k = irow * GridNcol + jcol;

    np = NodeList + k;
    xn = np->x;
    ynn = np->y;
    dx = xt - xn;
    dy = yt - ynn;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (np->spillnum == 999) {
        if (dist < tiny) {
            dmin = dist;
            kmin = k;
        }
    }
    else {
        if (dist < dmin) {
            dmin = dist;
            kmin = k;
        }
    }

    np = NodeList + k + 1;
    xn = np->x;
    ynn = np->y;
    dx = xt - xn;
    dy = yt - ynn;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (np->spillnum == 999) {
        if (dist < tiny) {
            dmin = dist;
            kmin = k + 1;
        }
    }
    else {
        if (dist < dmin) {
            dmin = dist;
            kmin = k + 1;
        }
    }

    np = NodeList + k + GridNcol;
    xn = np->x;
    ynn = np->y;
    dx = xt - xn;
    dy = yt - ynn;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (np->spillnum == 999) {
        if (dist < tiny) {
            dmin = dist;
            kmin = k + GridNcol;
        }
    }
    else {
        if (dist < dmin) {
            dmin = dist;
            kmin = k + GridNcol;
        }
    }

    np = NodeList + k + GridNcol + 1;
    xn = np->x;
    ynn = np->y;
    dx = xt - xn;
    dy = yt - ynn;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (np->spillnum == 999) {
        if (dist < tiny) {
            dmin = dist;
            kmin = k + GridNcol + 1;
        }
    }
    else {
        if (dist < dmin) {
            dmin = dist;
            kmin = k + GridNcol + 1;
        }
    }

  /*
   * If the closest node is more than a grid cell spacing away,
   * don't move the previous grid corner node.
   */
    if (dmin > dcrit  ||  kmin == -1) {
        return -1;
    }

  /*
   * If the closest node is on the grid border, do not move it.
   */
    if (kmin / GridNcol == 0  ||  kmin / GridNcol == GridNrow - 1  ||
        kmin % GridNcol == 0  ||  kmin % GridNcol == GridNcol - 1) {
        return - 1;
    }

    if (checkFan == 1) {
        istat = CheckTriangleFanForGridCornerNode (kmin, xt, yt);
        if (istat != 1) {
            return -1;
        }
    }

    np = NodeList + kmin;
    np->x = xt;
    np->y = yt;
    np->z = zt;

    return kmin;

}



/*
 *********************************************************************************

   functions for checking a triangle fan around a node to see if
   a point is inside the fan.

   These are only called from FindExistingGridCornerNode.

 *********************************************************************************
*/

int CSWGrdTriangle::_CheckTcheck (int *list, int tnum)
{
    int    i;

    if (tnum < 0) {
        return -1;
    }

    for (i=0; i<16; i++) {
        if (list[i] == tnum) {
            return -1;
        }
        if (list[i] == -999) {
            list[i] = tnum;
            return 1;
        }
    }

    return -1;

}

int CSWGrdTriangle::CheckTriangleFanForGridCornerNode (int node, double x, double y)
{
    NOdeStruct    *np;
    EDgeStruct    *ep;

    int           rp, t1, t2;
    int           i, j, istat, inside;
    int           *list, nlist;
    int           tcheck[16];
    int           nzero = 0;
    int           good = 0;
    int           n2, n3, n4, opedge;

    double        x1, y1, x2, y2, x3, y3, x4, y4, xint, yint;

    CSWPolyUtils             ply_utils_obj;

    for (i=0; i<16; i++) {
        tcheck[i] = -999;
    }

    np = NodeList + node;
    rp = np->rp;
    if (rp < 0) {
        return -1;
    }

    list = RawPoints[rp].edgelist;
    nlist = RawPoints[rp].nedge;

    if (list == NULL  ||  nlist < 2) {
        return -1;
    }
    if (nlist > 16) {
        return -1;
    }

  /*
   * Check if the point is inside the triangle fan
   * attached to the node.
   */
    for (i=0; i<nlist; i++) {
        ep = EdgeList + list[i];
        t1 = ep->tri1;
        t2 = ep->tri2;
        istat = _CheckTcheck (tcheck, t1);
        if (istat == 1) {
            inside = PointInTriangle (x, y, t1);
            if (inside == 1) {
                good = 1;
            }
            if (inside == 0) {
                if (nzero > 0) {
                    good = 1;
                }
                nzero++;
            }
        }
        istat = _CheckTcheck (tcheck, t2);
        if (istat == 1) {
            inside = PointInTriangle (x, y, t2);
            if (inside == 1) {
                good = 1;
            }
            if (inside == 0) {
                if (nzero > 0) {
                    good = 1;
                }
                nzero++;
            }
        }
    }

  /*
   * If the point was outside the triangle fan, or if a non
   * small number of edges are attached to the node, return
   * -1, which means don't move the grid node corner to this spot.
   */
    if (OutsidePointAdjust == 0) {
        if (good == 0  ||  nlist > 10) {
            return -1;
        }
    }

  /*
   * If the outline of the triangle fan is concave, it is not enough
   * to insure that the new point in inside the outline.  I also have
   * to make sure that any edges connected to the new point location
   * do not intersect any of the outside edges of the triangle fan.
   */
    x1 = x;
    y1 = y;
    good = 1;

    for (i=0; i<nlist; i++) {

    /*
     * Set up the vector from the new point to the
     * opposite point on the ith edge in the list.
     * The vector is shrunk to keep it from intersecting
     * right at the x2, y2 point.
     */
        ep = EdgeList + list[i];
        n2 = ep->node1;
        if (n2 == node) {
            n2 = ep->node2;
        }
        x2 = NodeList[n2].x;
        y2 = NodeList[n2].y;
        ShrinkVector2 (x1, y1, &x2, &y2);

    /*
     * Loop through the triangles in the tcheck list,
     * getting the edge opposite from the node and calculating
     * an intersection between that opposite edge and the vector
     * set up in the outer loop.  The loop breaks when a -999
     * triangle is found.
     */
        for (j=0; j<10; j++) {

            if (tcheck[j] == -999) {
                break;
            }
            opedge = OppositeEdge (TriangleList+tcheck[j], node);
            if (opedge < 0  ||  opedge > NumEdges) {
                assert (0);
            }

            n3 = EdgeList[opedge].node1;
            n4 = EdgeList[opedge].node2;

            x3 = NodeList[n3].x;
            y3 = NodeList[n3].y;
            x4 = NodeList[n4].x;
            y4 = NodeList[n4].y;

        /*
         * The only acceptable status values from the segment intersection function
         * are 1 and 5, which means no intersection between the segments.  If any other
         * value is returned, bail out with the good flag set to 0.
         */
            istat = ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4, &xint, &yint);
            if (!(istat == 1  ||  istat == 5)) {
                good = 0;
                break;
            }
        }

        if (good == 0) {
            break;
        }

    }

    if (good == 0) {
        return -1;
    }

    return 1;

}



void CSWGrdTriangle::dumpEdgeInfo (int edgenum)

{
    TRiangleStruct         *tp1, *tp2;
    EDgeStruct             *ep;
    int                    e1, e2, e3, e4;

/*
    Do nothing if the edge has only one triangle using it
*/
    ep = EdgeList + edgenum;
    if (ep->deleted == 1) {
        return;
    }
    if (ep->tri2 == -1) {
        return;
    }

/*
    Find the two edges of the first triangle that are not the specified edge.
*/
    tp1 = TriangleList + ep->tri1;
    tp2 = TriangleList + ep->tri2;

    e1 = e2 = e3 = e4 = -1;

    if (tp1->edge1 != edgenum) {
        e1 = tp1->edge1;
    }
    if (tp1->edge2 != edgenum) {
        if (e1 == -1) {
            e1 = tp1->edge2;
        }
        else {
            e2 = tp1->edge2;
        }
    }
    if (tp1->edge3 != edgenum) {
        e2 = tp1->edge3;
    }

/*
    Find the edges of the second triangle that are not the diagonal and
    find the remaining node of the quadrilateral.
*/
    if (tp2->edge1 != edgenum) {
        e3 = tp2->edge1;
    }
    if (tp2->edge2 != edgenum) {
        if (e3 == -1) {
            e3 = tp2->edge2;
        }
        else {
            e4 = tp2->edge2;
        }
    }
    if (tp2->edge3 != edgenum) {
        e4 = tp2->edge3;
    }

    printf ("info for edge %d\n", edgenum);
    printf ("from node %d to %d\n", EdgeList[edgenum].node1, EdgeList[edgenum].node2);
    printf ("surrounding quad info\n");
    printf ("e1 = %d e2 = %d e3 = %d e4 = %d\n", e1, e2, e3, e4);
    printf ("triangle 1 = %d  triangle 2 = %d\n", ep->tri1, ep->tri2);
    printf ("tp1 deleted = %d  tp2 deleted = %d\n", tp1->deleted, tp2->deleted);
    printf ("tp1 edges = %d %d %d\n", tp1->edge1, tp1->edge2, tp1->edge3);
    printf ("tp2 edges = %d %d %d\n", tp2->edge1, tp2->edge2, tp2->edge3);

    printf ("tp1 edge 1 nodes = %d %d\n",
      EdgeList[tp1->edge1].node1, EdgeList[tp1->edge1].node2);

    printf ("tp1 edge 2 nodes = %d %d\n",
      EdgeList[tp1->edge2].node1, EdgeList[tp1->edge2].node2);

    printf ("tp1 edge 3 nodes = %d %d\n",
      EdgeList[tp1->edge3].node1, EdgeList[tp1->edge3].node2);

    printf ("tp2 edge 1 nodes = %d %d\n",
      EdgeList[tp2->edge1].node1, EdgeList[tp2->edge1].node2);

    printf ("tp2 edge 2 nodes = %d %d\n",
      EdgeList[tp2->edge2].node1, EdgeList[tp2->edge2].node2);

    printf ("tp2 edge 3 nodes = %d %d\n",
      EdgeList[tp2->edge3].node1, EdgeList[tp2->edge3].node2);

}

#if 0
void CSWGrdTriangle::checkBadEdges (void)
{
    int          i;
    EDgeStruct   *ep;

    for (i=0; i<NumEdges; i++) {
        ep = EdgeList + i;
        if (ep->deleted == 1) {
            continue;
        }
        if (ep->node1 < 0  ||  ep->node2 < 0) {
            continue;
        }
        if (ep->node1 == ep->node2) {
            printf ("\nSame start and end nodes for edge %d\n", i);
            printf ("swapflag for this node = %d\n", SwapFlags[i]);
            dumpEdgeInfo (i);
        }
    }
}
#endif


/*
 ***************************************************************************

 g r d _ i n t e r p o l a t e _ u n f a u l t e d _ t r i m e s h _ v a l u e s

 ***************************************************************************

  Interpolate the soft nulls in the trimesh by back interpolation from the grid.
  Since there are no faults, the triangle extrapolation is not needed.

  The flag should be either GRD_BILINEAR or GRD_BICUBIC.

*/

int CSWGrdTriangle::grd_interpolate_unfaulted_trimesh_values
                                   (CSW_F *grid, int ncol, int nrow,
                                    double x1, double y1, double x2, double y2,
                                    NOdeStruct *nodes, int numnodes,
                                    int flag)

{
    int                i, istat;
    CSW_F              fxt, fyt, fzt;
    double             softchk, hardchk;

    softchk = GRD_SOFT_NULL_VALUE / 100.0;
    hardchk = GRD_HARD_NULL_VALUE / 100.0;

    for (i=0; i<numnodes; i++) {
        if (nodes[i].z < softchk  ||  nodes[i].z > hardchk) {
            fxt = (CSW_F)nodes[i].x;
            fyt = (CSW_F)nodes[i].y;
            istat =
                grd_arith_ptr->grd_back_interpolate (grid, ncol, nrow,
                                      (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
                                      NULL, 0,
                                      &fxt, &fyt, &fzt, 1, flag);
            if (istat == -1) {
                return -1;
            }
            nodes[i].z = fzt;
        }
    }

    return 1;

}  /* end of grd_interpolate_unfaulted_trimesh_values function */



/*
 *************************************************************************************

                 g r d _ a d d _ l i n e s _ t o _ t r i m e s h

 *************************************************************************************

  Add a set of constraint lines to a pre existing trimesh.

*/

int CSWGrdTriangle::grd_add_lines_to_trimesh
                     (
                      double *xlinesin, double *ylinesin, double *zlinesin,
                      int *linepointsin, int *lineflagsin, int nlinesin,
                      int exact_flag,
                      NOdeStruct **nodes_out, EDgeStruct **edges_out,
                      TRiangleStruct **triangles_out,
                      int *num_nodes_out, int *num_edges_out, int *num_triangles_out)
{
    int               istat, i, j, n, closed_flag, ntot;
    double            xmin, ymin, xmax, ymax, dx;
    double            *xlines = NULL, *ylines = NULL, *zlines = NULL;
    double            softchk;
    int               *linepoints = NULL, *lineflags = NULL, nlines, nltot;
    RAwPointStruct    *rptr = NULL, *rpstart = NULL;
    NOdeStruct        *nptr = NULL;
    EDgeStruct        *eptr = NULL;
    int               coincident_index;
    RAwPointStruct    *coincident_rptr = NULL, *last_rptr = NULL;
    int               n1, n2;
    NOdeStruct        *np1 = NULL, *np2 = NULL;
    RAwPointStruct    *rp1 = NULL, *rp2 = NULL;

    CSWPolyUtils             ply_utils_obj;

/*
 * !!!! debug only
 */
    char                   fname1[200];
    double                 v6[6];
    int                    do_write;
    int                    ncout[100];

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        csw_Free (xlines);
        csw_Free (linepoints);
        ListNull ();
        FreeMem ();
        if (bsuccess == false) {
            ListNullNeeded = 0;
            ConvexHullFlag = 0;
            AverageEdgeLength = -1.0;
            MaxIndexPoint = -1;
            ExactFlag = 0;
            *nodes_out = NlistReturn;
            *edges_out = ElistReturn;
            *triangles_out = TlistReturn;
            NlistReturn = NULL;
            ElistReturn = NULL;
            TlistReturn = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Obvious errors.
 */
    if (nlinesin < 1) {
        return 0;
    }

    if (xlinesin == NULL  ||  ylinesin == NULL  ||  zlinesin == NULL  ||
        linepointsin == NULL  ||  nodes_out == NULL  ||  edges_out == NULL  ||
        triangles_out == NULL  ||  *num_nodes_out < 3  ||  *num_edges_out < 3  ||
        *num_triangles_out < 1)
    {
        return 0;
    }

    ExactFlag = exact_flag;
    if (ExactFlag == 1) {
        PolygonalizeConstraintFlag = 0;
    }

/*
 * Make sure the private arrays start out clean.
 */
    FreeMem ();
    ListNullNeeded = 1;

    NodeList = *nodes_out;
    EdgeList = *edges_out;
    TriangleList = *triangles_out;
    NumNodes = *num_nodes_out;
    NumEdges = *num_edges_out;
    NumTriangles = *num_triangles_out;
    MaxNodes = NumNodes;
    MaxEdges = NumEdges;
    MaxTriangles = NumTriangles;

/*
 * Use the mean of the input trimesh edge lengths as the index cell
 * size and as a basis for how far a node can be moved to coincide
 * with a constraint node.
 */
    AverageEdgeLength =
    grd_calc_average_edge_length (NodeList, EdgeList, NumEdges,
                                  0);

    XYTiny = AverageEdgeLength / 1000.0;

    ExpandNodeList = 1;
    ExpandEdgeList = 1;
    ExpandTriList = 1;

/*
 * Make copies of the constraint line arrays
 * and filter them to remove very small lines.
 */
    ntot = 0;
    for (i=0; i<nlinesin; i++) {
        ntot += linepointsin[i];
    }
    xlines = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (xlines == NULL) {
        return -1;
    }
    ylines = xlines + ntot;
    zlines = ylines + ntot;
    linepoints = (int *)csw_Malloc (nlinesin * 2 * sizeof(int));
    if (linepoints == NULL) {
        return -1;
    }
    memcpy (xlines, xlinesin, ntot * sizeof(double));
    memcpy (ylines, ylinesin, ntot * sizeof(double));
    memcpy (zlines, zlinesin, ntot * sizeof(double));
    memcpy (linepoints, linepointsin, nlinesin * sizeof(int));
    if (lineflagsin) {
        lineflags = linepoints + nlinesin;
        memcpy (lineflags, lineflagsin, nlinesin * sizeof(int));
    }
    else {
        lineflags = NULL;
    }
    nlines = nlinesin;

    istat =
    FilterConstraintLines (xlines, ylines, zlines,
                           linepoints, lineflags, &nlines);
    if (istat == -1) {
        return -1;
    }

    istat =
    OrganizeConstraintLines (&xlines, &ylines, &zlines,
                             &linepoints, &lineflags, &nlines);
    if (istat == -1) {
        return -1;
    }

/*
    Allocate space for the raw lines list to be filled in later.
*/
    nltot = 0;
    for (i=0; i<nlines; i++) {
        nltot += linepoints[i];
        ncout[i] = 1;
    }

/*
 * !!!! debug only
 */
    do_write = csw_GetDoWrite ();;
    if (do_write) {
        strcpy (fname1, "rawlines.xyz");
        grd_WriteLines (
            xlines, ylines, zlines,
            nlines, ncout, linepoints, fname1);
    }

    if (do_write) {
        sprintf (fname1, "input.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

/*
 * Allocate space for the constraint line segments.
 */
    n = nltot + nlines*2;
    RawLines = (RAwLineSegStruct *)csw_Calloc (n*sizeof(RAwLineSegStruct));
    NumRawLines = 0;
    if (RawLines == NULL) {
        grd_utils_ptr->grd_set_err(1);
        return -1;
    }

/*
    Allocate space for the line points plus the original nodes.
*/
    n += NumNodes;
    n += 100;
    RawPoints = (RAwPointStruct *)csw_Calloc (n * sizeof(RAwPointStruct));
    NumRawPoints = 0;
    MaxRawPoints = n;
    if (RawPoints == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    MaxNcall = n * 10;

    CLineFlags = (int *)csw_Calloc (nlines * 2 * sizeof(int));
    if (CLineFlags == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    NumCLineFlags = nlines;

/*
 * Put the original unconstrained nodes into the raw points array.
 */
    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;

    rptr = RawPoints;
    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        rptr->x = nptr->x;
        rptr->y = nptr->y;
        rptr->z = nptr->z;
        rptr->nodenum = i;
        nptr->rp = rptr - RawPoints;
        nptr->crp = -1;
        if (rptr->x < xmin) xmin = rptr->x;
        if (rptr->y < ymin) ymin = rptr->y;
        if (rptr->x > xmax) xmax = rptr->x;
        if (rptr->y > ymax) ymax = rptr->y;
        rptr++;
    }

    MaxIndexPoint = rptr - RawPoints;

/*
    Add data from the specified lines to the raw points arrays.
*/
    n = 0;

/*
 *  If a raw constraint point coincides with a previously defined
 *  raw constraint point, use the previous not the new.
 */
    UseClineFlags = 1;
    NumRawCheck = rptr - RawPoints;
    for (i=0; i<nlines; i++) {
        rpstart = rptr;
        last_rptr = rptr;
        closed_flag = 0;
        if (lineflags) {
            CLineFlags[i] = lineflags[i];
            if (lineflags[i] == LIMIT_LINE_FLAG) lineflags[i] = 0;
        }
        for (j=0; j<linepoints[i]; j++) {
            if (j == linepoints[i] - 1) {
                if (SamePoint (rpstart->x, rpstart->y, xlines[n], ylines[n])) {
                    closed_flag = 1;
                    continue;
                }
            }
            coincident_index =
            CheckForCoincidentConstraintPoint (xlines[n], ylines[n]);
            if (coincident_index >= 0) {
                coincident_rptr = RawPoints + coincident_index;
                if (j > 0) {
                    AddRawLineSeg (last_rptr, coincident_rptr, i);
                }
                last_rptr = coincident_rptr;
                n++;
                continue;
            }

            rptr->x = xlines[n];
            rptr->y = ylines[n];
            if (rptr->x < xmin) xmin = rptr->x;
            if (rptr->y < ymin) ymin = rptr->y;
            if (rptr->x > xmax) xmax = rptr->x;
            if (rptr->y > ymax) ymax = rptr->y;
            rptr->z = zlines[n];
            rptr->nodenum = -1;
            rptr->deleted = 0;
            rptr->exact_flag = exact_flag;
            rptr->endflag = 0;
            if (j == 0) {
                rptr->endflag = 1;
            }
            if (j == linepoints[i]-1) {
                rptr->endflag = 2;
            }
            rptr->constraint_class = 0;
            if (lineflags) {
                rptr->constraint_class = lineflags[i];
            }

        /*
            Each line segment is saved for the application
            of constraint lines after the unconstrained
            triangulation is finished.
        */
            if (j > 0) {
                AddRawLineSeg(last_rptr, rptr, i);
            }
            last_rptr = rptr;
            n++;
            rptr++;
            NumRawCheck = rptr - RawPoints;
        }
        if (closed_flag == 1) {
            AddRawLineSeg (rptr-1, rpstart, i);
            n++;
        }
    }
    UseClineFlags = 0;
    NumLinePoints = rptr - RawPoints - MaxIndexPoint;

    NumRawPoints = rptr - RawPoints;

/*
    Build an index grid of the points.
*/
    dx = (xmax - xmin + ymax - ymin) / 100.0;
    AreaPerimeter = dx * 100.0;
    GrazeDistance = AreaPerimeter / 200000.0;
    ply_utils_obj.ply_setgraze (GrazeDistance);

    dx *= 5.0;
    xmin -= dx;
    ymin -= dx;
    xmax += dx;
    ymax += dx;

    CreateIndexGrid (xmin, ymin, xmax, ymax);

/*
 * Flag the original nodes that are on an outside edge of the trimesh.
 */
    istat = FlagEdgeNodes ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err(1);
        return -1;
    }

/*
 * Since there already is an input trimesh, the raw point edge lists for it
 * need to be built prior to adjusting the original node locations to the
 * constraint points.
 */
    istat = BuildRawPointEdgeLists();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err(1);
        return -1;
    }

    RemoveZeroLengthEdges ();
    RemoveZeroAreaTriangles ();

/*
 * If any constriant point is almost exactly on an original node location,
 * change the original node to reflect the constraint point.
 */
    AdjustForConstraintPoints ();

    do_write = csw_GetDoWrite ();
    if (do_write) {
        sprintf (fname1, "adjustconstraint.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

/*
 * The raw points that were not assigned to existing nodes
 * need to be indexed so they can be inserted into triangles
 * by the SubdivideTriangles function.
 */
    AddToIndexGrid (MaxIndexPoint, NumRawPoints);

/*
    Subdivide the existing triangles until all points have been used.
    This is where the endpoints of the constraints are added if needed.
*/
    istat = SubdivideTriangles ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
 * If any of the raw constraint points does not have a trimesh node
 * assigned to it, the constraint node is outside of the original
 * trimesh.  Such a node needs to be tied into the trimesh in order
 * to have all constraint nodes a part of the trimesh topology.
 * The CorrectOutsideConstraintPoints function does this work if
 * possible.
 */
    istat =
    CorrectOutsideConstraintPoints ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
 * If there are still constraint points without nodes, the complexity of the
 * constraints relative to the original trimesh is probably too great.  In
 * this case, try to remove constraints without nodes.
 */
    istat = CompressRawLines ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    TriDebugFunc1 ();

/*
 * The list relating nodes and edges needs to be rebuilt
 * to include all the edges created by SubdivideTriangles
 */
    istat = BuildRawPointEdgeLists();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err(1);
        return -1;
    }

/*
    Clean up artifacts from colinear points and points in
    almost exactly the same location.
*/
    RemoveZeroLengthEdges ();
    RemoveZeroAreaTriangles ();

    TriDebugFunc1 ();

    if (do_write) {
        sprintf (fname1, "preconstraint.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

    ForceValidate = 0;
    ValidateEdgeIntersection (NULL);
    ForceValidate = 0;

/*
    Apply the new constraints to the triangulation.
*/
    istat = ApplyConstraints ();
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    TriDebugFunc1 ();

    RemoveZeroLengthEdges ();
    RemoveZeroAreaTriangles ();
    TriDebugFunc1 ();

/*
 * If any node z values are GRD_SOFT_NULL_VALUE, remove the node
 * without leaving a tri mesh hole.
 */
    softchk = GRD_SOFT_NULL_VALUE / 100.0;
    for (i=0; i<NumNodes; i++) {
        if (NodeList[i].z <= softchk) {
            RemoveNode (i);
        }
    }

    if (do_write) {
        sprintf (fname1, "preremove.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

    ForceValidate = 0;
    ValidateConstraints ();
    ForceValidate = 0;

    ForceValidate = 0;
    ValidateExactConstraints ();
    ForceValidate = 0;

    ForceValidate = 0;
    ValidateEdgeIntersection ("\nValidation before remove deleted\n");
    ForceValidate = 0;

    ForceValidate = 0;
    ValidateTriangleShapes ();
    ForceValidate = 0;

    RemoveDeletedElements ();

    for (i=0; i<NumEdges; i++) {
        EdgeList[i].isconstraint = 0;
        if (EdgeList[i].flag != 0) {
            EdgeList[i].isconstraint = (char)1;
        }
    }

    if (do_write) {
        sprintf (fname1, "postconstraint.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

/*
 * Any edges with both endpoints on a constraint line need to
 * have their lineid values the same as the constraint line id.
 */
    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->deleted == 1) {
            continue;
        }
        n1 = eptr->node1;
        n2 = eptr->node2;
        np1 = NodeList + n1;
        np2 = NodeList + n2;
        if (np1->rp >= 0) {
            rp1 = RawPoints + np1->rp;
        }
        else if (np1->crp >= 0) {
            rp1 = ConstraintRawPoints + np1->crp;
        }
        else {
            continue;
        }
        if (np2->rp >= 0) {
            rp2 = RawPoints + np2->rp;
        }
        else if (np2->crp >= 0) {
            rp2 = ConstraintRawPoints + np2->crp;
        }
        else {
            continue;
        }

        if (rp1->lineid > 0  &&  rp2->lineid > 0  &&
            rp1->lineid == rp2->lineid) {
            eptr->lineid = rp1->lineid - 1;
        }
    }


/*
 * Any edges belonging to a limit line need to have
 * their flag set to LIMIT_LINE_FLAG
 */
    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->deleted == 1) {
            continue;
        }
        j = eptr->lineid;
        if (j >= 0  &&  j < NumCLineFlags) {
            if (CLineFlags[j] == LIMIT_LINE_FLAG) {
                eptr->flag = LIMIT_LINE_FLAG;
            }
        }
    }

/*
    Transfer the results to the output arrays.
*/
    *num_nodes_out = NumNodes;
    *num_edges_out = NumEdges;
    *num_triangles_out = NumTriangles;
    *nodes_out = NodeList;
    *edges_out = EdgeList;
    *triangles_out = TriangleList;

    bsuccess = true;

    return 1;

}  /*  end of function add_lines_to_trimesh  */


/*----------------------------------------------------------------------------*/

/*
 * Move existing nodes that are close to a constraint node
 * to the actual location of the constraint node.
 */
int CSWGrdTriangle::AdjustForConstraintPoints (void)
{
    int                i, ncp;
    NOdeStruct         *nptr;
    RAwPointStruct     *rptr;

/*
 * Move existing nodes to nearby constraint nodes if the move
 * is less than half the average edge length and if the move
 * does not introduce any crossing edges.
 */
    rptr = RawPoints + MaxIndexPoint;
    for (i=0; i<NumLinePoints; i++) {
        if (rptr - RawPoints >= NumRawCheck) {
            break;
        }
        ncp =
        SetupExistingTriangleNode (rptr->x, rptr->y, rptr->z);
        if (ncp == -1) {
            rptr++;
            continue;
        }
        if (ncp < -1) {
            ncp = -ncp;
            ncp -= 2;
            nptr = NodeList + ncp;
            rptr->x = nptr->x;
            rptr->y = nptr->y;
            rptr->z = nptr->z;
            rptr->nodenum = ncp;
            nptr->rp = rptr - RawPoints;
            nptr->crp = -1;
            rptr++;
        }
        else {
            nptr = NodeList + ncp;
            if (nptr->rp >= 0  &&  ncp >= MaxIndexPoint) {
                RawPoints[nptr->rp].nodenum = -1;
            }
            rptr->nodenum = ncp;
            nptr->rp = rptr - RawPoints;
            nptr->crp = -1;
            nptr->x = rptr->x;
            nptr->y = rptr->y;
            nptr->z = rptr->z;
            rptr++;
        }
    }

    return 1;

}

/*--------------------------------------------------------------------*/

double CSWGrdTriangle::grd_calc_average_edge_length (
    NOdeStruct          *nodes,
    EDgeStruct          *edges,
    int                 num_edges,
    int                 long_short_flag)
{
    int           i, n, histo[100];
    double        dmin, dmax, dspace, dmean, d1, d2;
    int           n1, n2, nsum;
    double        sum, dx, dy, dist, dcut;
    EDgeStruct    *eptr;
    NOdeStruct    *np1, *np2;

/*
 * Find the minimum length, maximum length and average length
 * of all non deleted edges.
 */
    dmin = 1.e30;
    dmax = -1.e30;

    nsum = 0;
    sum = 0.0;

    for (i=0; i<num_edges; i++) {
        eptr = edges + i;
        if (eptr->deleted) {
            continue;
        }
        n1 = eptr->node1;
        n2 = eptr->node2;
        np1 = nodes + n1;
        np2 = nodes + n2;
        dx = np1->x - np2->x;
        dy = np1->y - np2->y;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        if (dist < dmin) {
            dmin = dist;
        }
        if (dist > dmax) {
            dmax = dist;
        }
        eptr->length = dist;
        sum += dist;
        nsum++;
    }

/*
 * Return the average if that is needed.
 */
    if (long_short_flag == 0  ||
        dmin >= dmax  ||
        nsum < 1) {
        if (nsum < 1) {
            return 1.e30;
        }
        dmean = sum / nsum;
        return dmean;
    }

/*
 * Save the average in case there is a problem with the
 * long or short ignore below.
 */
    dmean = sum / nsum;

/*
 * Calculate the min, max and cell size for the histogram.
 */
    dx = (dmax - dmin) / 1000.0;
    d1 = dmin + dx;
    d2 = dmax - dx;
    dmin -= dx;
    dmax += dx;
    dspace = (dmax - dmin) / 100.0;

/*
 * Load up the histogram.
 */
    memset (histo, 0, 100 * sizeof(int));
    for (i=0; i<num_edges; i++) {
        eptr = edges + i;
        if (eptr->deleted) {
            continue;
        }
        dist = eptr->length;
        n = (int)((dist - dmin) / dspace);
        if (n < 0  ||  n > 99) {
            assert (0);
        }
        histo[n]++;
    }

/*
 * Find the histogram cell that has the median value in it.
 */
    nsum = 0;
    n = -1;
    for (i=0; i<100; i++) {
        nsum += histo[i];
        if (nsum > num_edges / 2) {
            n = i;
            break;
        }
    }

    if (n == -1) {
        assert (0);
    }

/*
 * Get the distance cutoff depending on the long short flag.
 */
    if (long_short_flag == GRD_IGNORE_LONG_EDGES) {
        dcut = dmin + (n + 1) * dspace;
    }
    else {
        dcut = dmin + n * dspace;
    }

/*
 * If the dcut value is very near the min or max,
 * just return the mean.
 */
    if (dcut > d2  ||  dcut < d1) {
        return dmean;
    }

/*
 * Average greater than dcut for ignore short or less than
 * dcut for ignore long.
 */
    sum = 0.0;
    nsum = 0;
    for (i=0; i<num_edges; i++) {
        eptr = edges + i;
        if (eptr->deleted) {
            continue;
        }
        dist = eptr->length;
        if (long_short_flag == GRD_IGNORE_LONG_EDGES) {
            if (dist > dcut) {
                continue;
            }
        }
        else {
            if (dist < dcut) {
                continue;
            }
        }
        sum += dist;
        nsum++;
    }

    if (nsum > 0) {
        dmean = sum / nsum;
    }

    return dmean;

}



/*-----------------------------------------------------------------------*/

int CSWGrdTriangle::grd_decimate_constraint (
    double              *xin,
    double              *yin,
    double              *zin,
    int                 *npts,
    double              decimation_distance)
{
    int             i, np, istat;
    double          *zp;
    void            **tag, **tagout;
    double          *x, *y, *z;
    double          xmin, ymin, zmin, xmax, ymax, zmax;

/*
 * Use the xy, xz or yz as the 2d line, depending on the largest
 * coordinate range.
 */
    xmin = 1.e30;
    ymin = 1.e30;
    zmin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;
    zmax = -1.e30;
    for (i=0; i<*npts; i++) {
        if (xin[i] < xmin) xmin = xin[i];
        if (yin[i] < ymin) ymin = yin[i];
        if (zin[i] < zmin) zmin = zin[i];
        if (xin[i] > xmax) xmax = xin[i];
        if (yin[i] > ymax) ymax = yin[i];
        if (zin[i] > zmax) zmax = zin[i];
    }

    xmax -= xmin;
    ymax -= ymin;
    zmax -= zmin;

    x = xin;
    y = yin;
    z = zin;
    if (zmax > xmax  ||  zmax > ymax) {
        if (ymax > xmax) {
            x = zin;
            y = yin;
            z = xin;
        }
        else {
            x = xin;
            y = zin;
            z = yin;
        }
    }


    np = *npts;

/*
 * Allocate void pointers for the z array.
 */
    tag = (void **)csw_Calloc (np * 2 * sizeof(void*));
    if (tag == NULL) {
        return -1;
    }
    tagout = tag + np;

/*
 * Set the void pointer tags to the addresses of each
 * element in the input z array.
 */
    for (i=0; i<np; i++) {
        tag[i] = (void *)(z + i);
    }

/*
 * Thin the points using the distance only.
 */
    istat =
    gpf_xythin2 (
        x, y, tag, np,
        decimation_distance, -1.0,
        x, y, tagout, npts);

    if (istat != 1) {
        return istat;
    }

/*
 * Retrieve the output tags and put their values back into the z array.
 */
    np = *npts;
    for (i=0; i<np; i++) {
        zp = (double *)(tagout[i]);
        z[i] = *zp;
    }

    csw_Free (tag);

    return 1;

}

/*
 ***********************************************************************************

                   V a l i d a t e C o n s t r a i n t s

 ***********************************************************************************

  Make sure all the raw constraint lines have edges associated with them.
*/

void CSWGrdTriangle::ValidateConstraints (void)
{
    int                 istat, i, j;
    int                 good, jmin, n1min, n2min;
    EDgeStruct          *ep;
    NOdeStruct          *np1, *np2;
    double              x1, y1, x2, y2, x3, y3, xint, yint;
    double              tiny, dx, dy, dist, mindist;
    RAwPointStruct      *rp1, *rp2;
    RAwLineSegStruct    *rline;
    char                *cenv;

    if (ForceValidate == 0) {
        cenv = csw_getenv ("GRD_VALIDATE_TRIMESH_TOPO");
        if (cenv == NULL) return;
    }

    printf ("\nValidating trimesh constraints.\n\n");

/*
 * Make sure that each raw segment in the constraints lies on an edge.
 */
    tiny = GrazeDistance * 10.0;

    for (i=0; i<NumRawLines; i++) {

        rline = RawLines + i;

        rp1 = RawPoints + rline->rp1;
        rp2 = RawPoints + rline->rp2;

        x1 = rp1->x;
        y1 = rp1->y;
        x2 = rp2->x;
        y2 = rp2->y;
        x3 = (x1 + x2) / 2.0;
        y3 = (y1 + y2) / 2.0;

        good = 0;
        mindist = 1.e30;
        jmin = -1;
        n1min = -1;
        n2min = -1;

        for (j=0; j<NumEdges; j++) {

            ep = EdgeList + j;
            if (ep->deleted == 1) {
                continue;
            }

            np1 = NodeList + ep->node1;
            np2 = NodeList + ep->node2;
            x1 = np1->x;
            y1 = np1->y;
            x2 = np2->x;
            y2 = np2->y;

            istat = gpf_perpintpoint2 (x1, y1, x2, y2, x3, y3, &xint, &yint);
            if (istat == 0) {
                continue;
            }

          /*
           * The perpintpoint can return 1 if the point grazes the segment
           * endpoint.  Check for exact inside segment here.
           */
            if ( !((x1-xint)*(xint-x2) >= 0.0f  ||  (y1-yint)*(yint-y2) >= 0.0f)) {
                continue;
            }

            dx = x3 - xint;
            dy = y3 - yint;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);

            if (dist < mindist) {
                mindist = dist;
                jmin = j;
                n1min = ep->node1;
                n2min = ep->node2;
            }

            if (dist <= tiny) {
                good = 1;
                break;
            }

        }

        if (good == 0) {
            printf ("Raw constraint number %d from raw point %d to %d\n",
                     i, rline->rp1, rline->rp2);
            printf ("does not have its midpoint on a triangle edge\n");
            printf ("distance from edge number %d from node %d to %d = %g\n",
                     jmin, n1min, n2min, mindist);
        }

    }

    printf ("Finished checking constraints.\n");

    return;

}  /* end of private ValidateConstraint function */



/*
 ************************************************************************************

  Find the closest original trimesh node to the specified constraint point
  location.  If the closest node is within a critical distance, use that node
  for the constraint point by moving it to the constraint location.  The move
  is only done if no crossing edges are produced.
*/

int CSWGrdTriangle::FindExistingTriangleNode (double xt, double yt, double zt, int checkFan)
{
    int         istat, irow, jcol, i, k, kmin;
    int         i1, i2, j1, j2, ii, jj, offset;
    double      dx, dy, dist, dmin, dcrit;
    NOdeStruct  *np;
    int         *list, nlist;
    INdexStruct *iptr;

    if (PolygonalizeConstraintFlag == 1) {
        return -1;
    }

    if (AverageEdgeLength < 0.0) {
        assert (0);
    }

    dmin = 1.e30;
    dcrit = AverageEdgeLength / 2.5;

    kmin = -1;

/*
 * Find the index cell containing the target location.
 */
    irow = (int)((yt - IndexYmin) / IndexYspace);
    jcol = (int)((xt - IndexXmin) / IndexXspace);

/*
 * Search the cell and all adjacent cells.
 */
    i1 = irow - 1;
    i2 = irow + 1;
    j1 = jcol - 1;
    j2 = jcol + 1;

    if (i1 < 0) i1 = 0;
    if (i2 < 0) i2 = 0;
    if (j1 < 0) j1 = 0;
    if (j2 < 0) j2 = 0;
    if (i1 >= IndexNrow) i1 = IndexNrow - 1;
    if (i2 >= IndexNrow) i2 = IndexNrow - 1;
    if (j1 >= IndexNcol) j1 = IndexNcol - 1;
    if (j2 >= IndexNcol) j2 = IndexNcol - 1;

/*
 * Find the closest node to the specified target point.
 */
    dmin = 1.e30;
    kmin = -1;
    for (ii=i1; ii<=i2; ii++) {
        offset = ii * IndexNcol;
        for (jj=j1; jj<=j2; jj++) {
            k = offset + jj;
            iptr = IndexGrid[k];
            if (iptr == NULL) {
                continue;
            }
            list = iptr->list;
            nlist = iptr->npts;
            if (list == NULL  ||  nlist < 1) {
                continue;
            }
            for (i=0; i<nlist; i++) {
                np = NodeList + list[i];
                if (np->deleted == 1) {
                    continue;
                }
                if (np->is_locked) {
                    dx = xt - np->xorig;
                    dy = yt - np->yorig;
                }
                else {
                    dx = xt - np->x;
                    dy = yt - np->y;
                }
                dist = dx * dx + dy * dy;
                dist = sqrt (dist);
                if (dist < dmin) {
                    dmin = dist;
                    kmin = list[i];
                }
            }
        }
    }

  /*
   * If the closest node is more than the critical spacing away,
   * don't move the previous triangle node.
   */
    if (dmin > dcrit  ||  kmin == -1) {
        return -1;
    }

  /*
   * If the closest node is on the trimesh border, do not move it.
   */
    np = NodeList + kmin;
    if (np->on_border) {
        return -1;
    }

    if (checkFan == 1) {
        istat = CheckTriangleFanForGridCornerNode (kmin, xt, yt);
        if (istat != 1) {
            return -1;
        }
    }

    np = NodeList + kmin;
    np->x = xt;
    np->y = yt;
    np->z = zt;

    return kmin;

}





int CSWGrdTriangle::SetupExistingTriangleNode
    (double xt, double yt, double zt)
{
    int         istat, irow, jcol, k, kmin, *list, nlist;
    int         i, i1, i2, j1, j2, ii, jj, offset;
    double      xn, ynn, dx, dy, dist, dmin;
    double      xorig, yorig, dorig, dcrit, dcrit1000, dcrit100;
    NOdeStruct  *np;
    INdexStruct *iptr;
    RAwPointStruct    *rptr;

    if (PolygonalizeConstraintFlag == 1) {
        return -1;
    }

    dmin = 1.e30;
    kmin = -1;

    dcrit = AverageEdgeLength / 2.5;
    dcrit1000 = dcrit / 3.0;
    dcrit100 = dcrit / 3.0;  /*TODO!!!! change back to /10 */
    if (ExactFlag == 0) {
        dcrit1000 /= 10000.0;
    }

    irow = (int)((yt - IndexYmin) / IndexYspace);
    jcol = (int)((xt - IndexXmin) / IndexXspace);

/*
 * Search the cell and all adjacent cells.
 */
    i1 = irow - 1;
    i2 = irow + 1;
    j1 = jcol - 1;
    j2 = jcol + 1;

    if (i1 < 0) i1 = 0;
    if (i2 < 0) i2 = 0;
    if (j1 < 0) j1 = 0;
    if (j2 < 0) j2 = 0;
    if (i1 >= IndexNrow) i1 = IndexNrow - 1;
    if (i2 >= IndexNrow) i2 = IndexNrow - 1;
    if (j1 >= IndexNcol) j1 = IndexNcol - 1;
    if (j2 >= IndexNcol) j2 = IndexNcol - 1;

/*
 * Find the closest node to the specified target point.
 */
    dmin = 1.e30;
    kmin = -1;
    np = NULL;
    for (ii=i1; ii<=i2; ii++) {
        offset = ii * IndexNcol;
        for (jj=j1; jj<=j2; jj++) {
            k = offset + jj;
            iptr = IndexGrid[k];
            if (iptr == NULL) {
                continue;
            }
            list = iptr->list;
            nlist = iptr->npts;
            if (list == NULL  ||  nlist < 1) {
                continue;
            }
            for (i=0; i<nlist; i++) {
                if (list[i] >= NumRawPoints) {
                    continue;
                }
                rptr = RawPoints + list[i];
                if (rptr->nodenum < 0) {
                    continue;
                }
                np = NodeList + rptr->nodenum;
                if (np->deleted == 1) {
                    continue;
                }
                dx = xt - np->x;
                dy = yt - np->y;
                dist = dx * dx + dy * dy;
                dist = sqrt (dist);
                if (dist < dmin) {
                    dmin = dist;
                    kmin = np - NodeList;
                }
            }
        }
    }

    if (kmin == -1  ||  np == NULL) {
        return -1;
    }

    if (dmin > dcrit) {
        return -1;
    }

    np = NodeList + kmin;

    if (np->on_border == 1) {
        if (dmin > dcrit100) {
            return -1;
        }
    }

/*
 * If the closest node has already been "locked" then compare the distance
 * between the current target point and the original node location to
 * the distance between the previously "locked" location and the original
 * node location.  If the target point is closer to the original node
 * location than the previous "locked" location, change the node's position
 * to the target point.
 */
    if (np->is_locked == 1) {

    /*
     * If the raw point location (xt, yt) is "exactly" on the
     * current locked node location, use the locked node as the
     * raw point's node.
     */
        xn = np->x;
        ynn = np->y;
        dx = xn - xt;
        dy = ynn - yt;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        if (dist < dcrit1000) {
            return kmin;
        }

        xorig = np->xorig;
        yorig = np->yorig;
        dx = xt - xorig;
        dy = yt - yorig;
        dorig = dx * dx + dy * dy;
        dorig = sqrt (dorig);

        xn = np->x;
        ynn = np->y;
        dx = xn - xorig;
        dy = ynn - yorig;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);

        if (dorig < dist) {
            istat = CheckTriangleFanForGridCornerNode (kmin, xt, yt);
            if (istat != 1) {
                return -1;
            }
            np->x = xt;
            np->y = yt;
            np->z = zt;
            if (np->rp >= 0) {
                RawPoints[np->rp].nodenum = -1;
            }
            return kmin;
        }
        return -1;
    }

  /*
   * Move the node into its first "locked" position.
   */
    istat = CheckTriangleFanForGridCornerNode (kmin, xt, yt);
    if (istat != 1) {
        return -1;
    }
    np->xorig = np->x;
    np->yorig = np->y;
    np->zorig = np->z;
    np->x = xt;
    np->y = yt;
    np->z = zt;
    np->is_locked = 1;

    return kmin;

}



#if 0
/*
 *********************************************************************************

                 U s e E d g e N o d e F o r C o n s t r a i n t

 *********************************************************************************

  If the specified x, y point lies very close to an edge segment attached
  to the specified nodenum, return 1.  Otherwise, return zero.

*/
int CSWGrdTriangle::UseEdgeNodeForConstraint
  (int nodenum, double xp, double yp, double tiny)
{
    int          nlist, *list, i, istat;
    int          n1, n2;
    EDgeStruct   *eptr;
    double       pdist;

    list = NULL;
    nlist = GetNodeEdgeList (nodenum, &list);
    if (nlist < 2  ||  list == NULL) {
        return 0;
    }

/*
 * The nodenum and the two adjacent nodes that are also
 * on the trimesh border need to be essentially colinear
 * to pass this test.
 */
    n1 = -1;
    n2 = -1;

    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        if (eptr->on_border == 0) {
            continue;
        }
        if (n1 == -1) {
            n1 = eptr->node1;
            if (n1 == nodenum) {
                n1 = eptr->node2;
            }
        }
        else if (n2 == -1) {
            n2 = eptr->node1;
            if (n2 == nodenum) {
                n2 = eptr->node2;
            }
        }
        else {
            return 0;
        }
    }

    if (n1 == -1  ||  n2 == -1) {
        return 0;
    }

    istat = NodeOnSegment (nodenum, n1, n2, &pdist);
    if (istat != 1  ||  pdist > tiny) {
        return 0;
    }

    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        if (eptr->on_border == 0) {
            continue;
        }
        istat = PointOnSegment (
                  xp, yp,
                  eptr->node1, eptr->node2, &pdist);
        if (istat == 1  &&  pdist <= tiny) {
            return 1;
        }
    }

    return 0;

}
#endif






/*
 *********************************************************************************

             F l a g E d g e N o n C o n s t r a i n t N o d e s

 *********************************************************************************

  Set the on_border flag for all nodes that are endpoints of edges where the
  edge has only one triangle using it.

*/

int CSWGrdTriangle::FlagEdgeNonConstraintNodes (void)
{
    int           i;
    EDgeStruct    *eptr;
    NOdeStruct    *np1, *np2;

    if (EdgeList == NULL  ||  NodeList == NULL  ||
        NumEdges < 3  ||  NumNodes < 3) {
        return 1;
    }

    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->deleted == 1) {
            continue;
        }
        if (eptr->tri2 >= 0) {
            continue;
        }
        if (eptr->flag != 0) {
            continue;
        }
        eptr->on_border = 1;
        np1 = NodeList + eptr->node1;
        np1->on_border = 1;
        np2 = NodeList + eptr->node2;
        np2->on_border = 1;
    }

    return 1;

}



/*
 *********************************************************************************

                           F l a g E d g e N o d e s

 *********************************************************************************

  Set the on_border flag for all nodes that are endpoints of edges where the
  edge has only one triangle using it.

*/

int CSWGrdTriangle::FlagEdgeNodes (void)
{
    int           i;
    EDgeStruct    *eptr;
    NOdeStruct    *np1, *np2;

    if (EdgeList == NULL  ||  NodeList == NULL  ||
        NumEdges < 3  ||  NumNodes < 3) {
        return 1;
    }

    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->deleted == 1) {
            continue;
        }
        if (eptr-> tri2 >= 0) {
            continue;
        }
        eptr->on_border = 1;
        np1 = NodeList + eptr->node1;
        np1->on_border = 1;
        np2 = NodeList + eptr->node2;
        np2->on_border = 1;
    }

    return 1;

}

/*
  ****************************************************************************

          S w a p F o r E x a c t C o n s t r a i n t S e g m e n t

  ****************************************************************************

  If a constraint segment intersects another edge then try to swap the other
  edge so it does not intersect the constraint segment.

*/

int CSWGrdTriangle::SwapForExactConstraintSegment (int nt1,
                                     int *list, int nlist,
                                     int nt2)
{
    int               istat, i, n1, n2, et, rp;
    int               nchk, sameflag, ns1, ns2;
    double            xt, yt, zt, x1, y1, x2, y2, apsave;
    double            xc1, yc1, xc2, yc2, xint, yint;
    EDgeStruct        *eptr;
    NOdeStruct        *nptr;
    TRiangleStruct    *tp1;
    RAwPointStruct    *rptr;

    CSWPolyUtils             ply_utils_obj;

    if (nlist < 1) return 1;

    Ncall++;
    if (Ncall > MaxNcall) {
        assert (0);
    }

/*
    These lines are to suppress warnings.
*/
    eptr = EdgeList;
    xt = yt = zt = 1.e30;


    n1 = nt1;
    n2 = nt2;
    et = 0;
    tp1 = NULL;

    xc1 = NodeList[n1].x;
    yc1 = NodeList[n1].y;
    xc2 = NodeList[n2].x;
    yc2 = NodeList[n2].y;

/*
    If the end node of any edge in the list is the
    same node as the endpoint of the line segment, then
    don't do any swapping.
*/
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];

        if (eptr->node1 == n2  ||  eptr->node2 == n2) {
            return 1;
        }
    }

/*
    If the end node of any edge connected to the start node of the
    constraint segment is on the constraint segment, don't do any
    swapping.
*/
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        nchk = eptr->node1;
        if (nchk == n1) {
            nchk = eptr->node2;
        }
        if (NodeOnSegment (nchk, nt1, nt2, NULL)) {
            return 1;
        }
    }

/*
    Find an opposite triangle edge from the nt1 node that intersects
    the constraint segment.
*/
    et = -1;

    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];

        if (eptr->tri1 < 0  ||  eptr->tri1 >= NumTriangles) {
            printf ("edge with bad tri1 Orig constraint nodes = %d %d\n",
                    Orignt1, Orignt2);
        }

        tp1 = TriangleList + eptr->tri1;
        et = OppositeEdge (tp1, n1);
        if (et != -1  &&  EdgeList[et].tflag == 0) {
            istat = EdgeIntersect(et, n1, n2,
                                  &xt, &yt, &zt);
            if (istat == 1) {
                istat = SamePointAsNode (xt, yt, n1);
                if (istat == 1) {
                    return 1;
                }
                break;
            }
        }

        if (eptr->tri2 < 0) {
            continue;
        }
        tp1 = TriangleList + eptr->tri2;
        et = OppositeEdge (tp1, n1);
        if (et != -1  &&  EdgeList[et].tflag == 0) {
            istat = EdgeIntersect(et, n1, n2,
                                  &xt, &yt, &zt);
            if (istat == 1) {
                istat = SamePointAsNode (xt, yt, n1);
                if (istat == 1) {
                    return 1;
                }
                break;
            }
        }
    }

/*
    If no intersection was found, return without swapping.
*/
    if (xt > 1.e15  ||  et < 0  ||  tp1 == NULL) {
        return 1;
    }

/*
 * Try to swap the edge that intersects the constraint segment.
 */
    istat = GetSwapCoords (et, &x1, &y1, &x2, &y2, &ns1, &ns2);
    if (istat != 1) {
        return 1;
    }

/*
 * If the swap creates an edge from the constraint nt1 to nt2,
 * do the swap and return.
 */
    if ((ns1 == nt1  &&  ns2 == nt2) ||
        (ns1 == nt2  &&  ns2 == nt1)) {
        SwapEdge (et);
        EdgeList[et].tflag = 1;
        return 1;
    }

/*
 * Check if the swapped edge intersects the constraint segment.
 */
    istat = ply_utils_obj.ply_segint (x1, y1, x2, y2,
                        xc1, yc1, xc2, yc2,
                        &xint, &yint);

/*
 * If the intersection is almost exactly at the xc1, yc1 point,
 * reset as if there is no intersection.  The normal "same point"
 * distance is 1/20000th of the area perimeter.  This is made
 * 100 times smaller here.
 */
    if (istat == 0) {
       apsave = AreaPerimeter;
       AreaPerimeter /= 100.0;
       sameflag = SamePoint (xc1, yc1, xint, yint);
       AreaPerimeter = apsave;
       if (sameflag == 1) {
            istat = 1;
       }
    }

/*
 * If the swapped edge does not intersect the constraint segment,
 * swap the edge and recursively call this function with the new
 * edge list from the start node.
 */
    if (istat == 1) {
        SwapEdge (et);
        EdgeList[et].tflag = 1;
        nptr = NodeList + n1;
        rp = nptr->rp;
        if (rp < 0) {
            rp = nptr->crp;
            if (rp < 0) {
                assert (rp >= 0);
            }
            rptr = ConstraintRawPoints + rp;
        }
        else {
            rptr = RawPoints + rp;
        }
        list = rptr->edgelist;
        nlist = rptr->nedge;

        istat = SwapForExactConstraintSegment(nt1,
                                         list,
                                         nlist,
                                         nt2);
    }

/*
 * If the swapped edge will intersect the constraint segment,
 * return from this function.
 */
    return istat;

}  /*  end of private SwapForExactConstraintSegment function  */



/*
 ******************************************************************

                  G e t S w a p C o o r d s

 ******************************************************************

    Return the coordinates of the swapped edge without actually
    doing the swapping.  If the edge cannot be swapped, zero is
    returned and the coordinates are set to 1.e30.
*/

int CSWGrdTriangle::GetSwapCoords (int edgenum,
                          double *x1out, double *y1out,
                          double *x2out, double *y2out,
                          int *node1out, int *node2out)

{
    TRiangleStruct         *tp1, *tp2;
    EDgeStruct             *ep, *ep3, *ep4, *eptmp;
    int                    e1, e2, e3, e4,
                           n1, n2, n3, n4, istat;
    double                 xint, yint;
    double                 x1, y1, x2, y2, x3, y3, x4, y4;
    double                 xa[5], ya[5], xmid, ymid;
    int                    force;

    CSWPolyUtils             ply_utils_obj;

    *x1out = 1.e30;
    *y1out = 1.e30;
    *x2out = 1.e30;
    *y2out = 1.e30;
    *node1out = -1;
    *node2out = -1;

/*
    Do nothing if the edge has only one triangle using it
    or if the edge swap flags are not enabled.
*/
    if (EdgeSwapFlag == GRD_SWAP_NONE) {
        return 0;
    }

    ep = EdgeList + edgenum;
    if (ep->deleted == 1) {
        return 0;
    }
    if (ep->tri2 == -1) {
        return 0;
    }

    if (EdgeSwapFlag == GRD_SWAP_AS_FLAGGED  &&
        ep->flag == GRD_DONT_SWAP_FLAG) {
        return 0;
    }

/*
    Find the two edges of the first triangle that are not the specified edge.
*/
    tp1 = TriangleList + ep->tri1;
    tp2 = TriangleList + ep->tri2;

    e1 = e2 = e3 = e4 = -1;

    if (tp1->edge1 != edgenum) {
        e1 = tp1->edge1;
    }
    if (tp1->edge2 != edgenum) {
        if (e1 == -1) {
            e1 = tp1->edge2;
        }
        else {
            e2 = tp1->edge2;
        }
    }
    if (tp1->edge3 != edgenum) {
        e2 = tp1->edge3;
    }

/*
    Find the nodes of the first triangle, in the same order as the edges.
*/
    n1 = CommonNode (edgenum, e1);
    n2 = CommonNode (e1, e2);
    n3 = CommonNode (e2, edgenum);

/*
    Find the edges of the second triangle that are not the diagonal and
    find the remaining node of the quadrilateral.
*/
    if (tp2->edge1 != edgenum) {
        e3 = tp2->edge1;
    }
    if (tp2->edge2 != edgenum) {
        if (e3 == -1) {
            e3 = tp2->edge2;
        }
        else {
            e4 = tp2->edge2;
        }
    }
    if (tp2->edge3 != edgenum) {
        e4 = tp2->edge3;
    }

    ep3 = EdgeList + e3;
    ep4 = EdgeList + e4;

    n4 = CommonNode (e3, e4);

/*
    The edge e3 must be connected to edge e2.  If not, swap
    e3 and e4 edges.  When this is done, the edges e1, e2, e3, to e4
    represent a continuous perimeter of the quadrilateral.  The nodes
    n1, n2, n3, and n4 are the nodes of the perimeter, in order.
*/
    if (CommonNode (e2, e3) == -1) {
        eptmp = ep3;
        ep3 = ep4;
        ep4 = eptmp;
        e3 = ep3 - EdgeList;
        e4 = ep4 - EdgeList;
    }

/*
    If the two possible diagonal segments do not intersect, there
    are two possible scenarios.  If the candidate edge for swapping is
    outside of the quadralateral defined by its adjoining triangles,
    the edge should always be swapped.  If the candidate edge is inside,
    the edge should never be swapped.
*/

  /*
   * Get the endpoints of the two possible diagonals.
   */
    x1 = NodeList[n1].x;
    y1 = NodeList[n1].y;
    x2 = NodeList[n3].x;
    y2 = NodeList[n3].y;
    x3 = NodeList[n2].x;
    y3 = NodeList[n2].y;
    x4 = NodeList[n4].x;
    y4 = NodeList[n4].y;

  /*
   * Make the outline of the quadralateral and check if the
   * mid point of the candidate edge is inside the outline.
   * If it is not inside, and the midpoint of the alternate
   * diagonal is inside, force swapping.  If both midpoints
   * are outside, return without swapping.
   */
    xa[0] = x1;
    ya[0] = y1;
    xa[1] = x3;
    ya[1] = y3;
    xa[2] = x2;
    ya[2] = y2;
    xa[3] = x4;
    ya[3] = y4;
    xa[4] = x1;
    ya[4] = y1;

    xmid = (x1 + x2) / 2.0;
    ymid = (y1 + y2) / 2.0;

    force = 0;
    istat = ply_utils_obj.ply_point (xa, ya, 5, xmid, ymid);
    if (istat != 1) {

    /*
     * Check the alternate diagonal.  Return if its midpoint
     * is also outside the quadralateral.
     */
        xmid = (x3 + x4) / 2.0;
        ymid = (y3 + y4) / 2.0;
        istat = ply_utils_obj.ply_point (xa, ya, 5, xmid, ymid);
        if (istat != 1) {
            if (FinalSwapFlag == 1) {
                if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
                if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
                if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
                if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
                SwapFlags[edgenum] = 2;
                NumSwapped++;
            }
            return 0;
        }

        force = 1;

        goto FORCE_SWAPPING;
    }

  /*
   * The midpoint is inside the quadralateral outline, so check the
   * intersection of the two diagonals.  If they do not intersect,
   * do not swap.
   */
    if (force == 0) {
        if (ExtendDiagonalsFlag) {
            ExtendVectors (&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
        }
        istat = ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4,
                            &xint, &yint);
        if (istat != 0) {
            if (FinalSwapFlag == 1) {
                if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
                if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
                if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
                if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
                SwapFlags[edgenum] = 2;
                NumSwapped++;
            }
            return 0;
        }
    }

  FORCE_SWAPPING:

/*
    The old diagonal was by definition from n1 to n3, so the
    new diagonal must be from n2 to n4.  Return the coordinates
    for n2 and n4.
*/
    *x1out = NodeList[n2].x;
    *y1out = NodeList[n2].y;
    *x2out = NodeList[n4].x;
    *y2out = NodeList[n4].y;
    *node1out = n2;
    *node2out = n4;

    return 1;

}  /*  end of private GetSwapCoords function  */




/*
  ****************************************************************************

                       S w a p E x a c t L i n e s

  ****************************************************************************

  Loop through all the constraint segments and attempt to swap existing edges
  that intersect with the segments.

*/

int CSWGrdTriangle::SwapExactLines (void)

{
    int                  istat, i, nt1, nt2;
    RAwLineSegStruct     *rline;
    RAwPointStruct       *rp1, *rp2;

    for (i=0; i<NumRawLines; i++) {
        rline = RawLines + i;
        rp1 = RawPoints + rline->rp1;
        rp2 = RawPoints + rline->rp2;
        nt1 = rp1->nodenum;
        nt2 = rp2->nodenum;
        Ncall = 0;
        Orignt1 = nt1;
        Orignt2 = nt2;
        NumNodesToLock = 0;
        istat = SwapForExactConstraintSegment (nt1, rp1->edgelist, rp1->nedge,
                                               nt2);
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}  /*  end of private SwapExactLines function  */




/*
  ****************************************************************************

              R e m o v e N o d e s O n C o n s t r a i n t s

  ****************************************************************************

  If a constraint segment is broken up by another node or nodes, remove those
  nodes so that the consteraint segment coincides with a single edge in the
  trimesh.

*/

int CSWGrdTriangle::RemoveNodesOnConstraints (void)

{
    int                  i, n, istat, ndo, maxdo;
    RAwLineSegStruct     *rline;
    RAwPointStruct       *rp1, *rp2;

    if (NumRawLines < 1) {
        return 1;
    }

    ndo = 0;
    maxdo = NumRawLines;
    if (maxdo < 10) {
        maxdo = 10;
    }

    for (;;) {
        n = 0;
        for (i=0; i<NumRawLines; i++) {
            rline = RawLines + i;
            rp1 = RawPoints + rline->rp1;
            rp2 = RawPoints + rline->rp2;
            istat =
            CheckForNodeOnConstraint (rp1, rp2);
            if (istat == 1) {
                n++;
            }
        }
        if (n == 0) {
            break;
        }
        ndo++;
        if (ndo > maxdo) {
            printf
            ("\n\n!!!!!!!! Too many iterations in RemoveNodesOnConstraints !!!!!!!!\n\n");
            break;
        }
    }

    return 1;

}  /*  end of private RemoveNodesOnConstraints function  */




/*
 ***********************************************************************************

                 V a l i d a t e E x a c t C o n s t r a i n t s

 ***********************************************************************************

  Make sure all the raw constraint lines have edges associated with them.
*/

void CSWGrdTriangle::ValidateExactConstraints (void)
{
    int                 i, j, n1, n2;
    int                 good, en1, en2;
    EDgeStruct          *ep;
    RAwPointStruct      *rp1, *rp2;
    RAwLineSegStruct    *rline;
    char                *cenv;

    if (ForceValidate == 0) {
        cenv = csw_getenv ("GRD_VALIDATE_TRIMESH_TOPO");
        if (cenv == NULL) return;
    }

    printf ("\nValidating trimesh constraint exactness.\n\n");

/*
 * Make sure that each raw constraint segment has an edge that
 * has the same start and end nodes.
 */
    for (i=0; i<NumRawLines; i++) {

        rline = RawLines + i;

        rp1 = RawPoints + rline->rp1;
        rp2 = RawPoints + rline->rp2;

        n1 = rp1->nodenum;
        n2 = rp2->nodenum;
        if (n1 < 0  ||  n2 < 0) {
            printf ("Negative node number for raw constraint point number %d\n", i);
            continue;
        }

        good = 0;
        for (j=0; j<NumEdges; j++) {

            ep = EdgeList + j;
            if (ep->deleted == 1) {
                continue;
            }

            en1 = ep->node1;
            en2 = ep->node2;

            if ((en1 == n1  &&  en2 == n2)  ||
                (en1 == n2  &&  en2 == n1)) {
                good = 1;
                break;
            }

        }

        if (good == 0) {
            printf ("Raw constraint number %d from raw point %d to %d\n",
                     i, rline->rp1, rline->rp2);
            printf ("and node %d to %d does not have an exact edge match\n",
                     rp1->nodenum, rp2->nodenum);
        }

    }

    printf ("Finished checking constraint exactness.\n");

    return;

}  /* end of private ValidateExactConstraint function */





/*
 *************************************************************************

                   G e t S h e l l A r o u n d N o d e

 *************************************************************************

  Return the points one edge away from a node and the edges that
  connect those points around the node.  The returned nodes and edges
  are not in any particular order.

*/

int CSWGrdTriangle::GetShellAroundNode (
    int center_node_num,
    int **nodes_out,
    int *num_nodes_out,
    int **edges_out,
    int *num_edges_out)
{
    int               i, j, k, n1, nt, en, found,
                      rp, npout, neout;
    int               *list = NULL, nlist, *nodelist = NULL, *edgelist = NULL;
    RAwPointStruct    *rptr = NULL;
    NOdeStruct        *nptr = NULL;
    EDgeStruct        *eptr = NULL, *ep2 = NULL;
    int               estat;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodelist);
            csw_Free (edgelist);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of an error.
 */
    *nodes_out = NULL;
    *num_nodes_out = 0;
    *edges_out = NULL;
    *num_edges_out = 0;

/*
 * Get the edges attached to the center node.
 */
    nptr = NodeList + center_node_num;
    rp = nptr->rp;
    if (rp == -1) {
        rp = nptr->crp;
        if (rp == -1) {
            return 0;
        }
        rptr = ConstraintRawPoints + rp;
    }
    else {
        rptr = RawPoints + rp;
    }

    list = rptr->edgelist;
    nlist = rptr->nedge;

    if (list == NULL  ||  nlist < 3) {
        return 0;
    }

/*
 * Allocate space for the output nodes and edges.
 */
    nodelist = (int *)csw_Malloc (nlist * sizeof(int));
    if (nodelist == NULL) {
        return -1;
    }

    edgelist = (int *)csw_Malloc (nlist * sizeof(int));
    if (edgelist == NULL) {
        return -1;
    }

/*
 * Populate the node list from the edges attached to the center node.
 */
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        n1 = eptr->node1;
        if (n1 == center_node_num) {
            n1 = eptr->node2;
        }
        nodelist[i] = n1;
    }
    npout = nlist;

/*
 * Populate the output edge list.
 * Get the list of edges attached to each node in the output
 * node list.  If the opposite endpoint of the edge is in the
 * output node list, add the edge to the output edge list.
 * The edge is added only once to the output edge list.
 */
    neout = 0;
    for (i=0; i<npout; i++) {
        nptr = NodeList + nodelist[i];

    /*
     * Get the list of edges attached to this node.
     */
        rp = nptr->rp;
        if (rp == -1) {
            rp = nptr->crp;
            if (rp == -1) {
                return 0;
            }
            rptr = ConstraintRawPoints + rp;
        }
        else {
            rptr = RawPoints + rp;
        }
        list = rptr->edgelist;
        nlist = rptr->nedge;

    /*
     * Check each edge for an opposite node in the set
     * of output nodes.
     */
        for (j=0; j<nlist; j++) {
            en = list[j];
            ep2 = EdgeList + en;
            nt = ep2->node1;
            if (nt == nodelist[i]) {
                nt = ep2->node2;
            }
            found = 0;
            for (k=0; k<npout; k++) {
                if (nt == nodelist[k]) {
                    found = 1;
                    break;
                }
            }

        /*
         * If the opposite node is in the output node list, check if
         * this edge has already been added to the output edges.
         */
            if (found == 1) {
                found = 0;
                for (k=0; k<neout; k++) {
                    if (en == edgelist[k]) {
                        found = 1;
                        break;
                    }
                }

            /*
             * This edge is not in the output list yet, so add it.
             * Only add it if the opposite node of either triangle
             * sharing the edge is the center node of the shell.
             */
                if (found == 0) {
                    estat = CheckForOppositeNodeMatch (en, center_node_num);
                    if (estat == 1) {
                        edgelist[neout] = en;
                        neout++;
                    }
                }
            }
        }
    }

/*
 * Set the output variables and return.
 */
    *nodes_out = nodelist;
    *num_nodes_out = npout;
    *edges_out = edgelist;
    *num_edges_out = neout;

    bsuccess = true;

    return 1;

}



/*
 * Return 1 if the specified node is an opposite node of one
 * triangle shared by the edge.  Return zero if not.
 */
int CSWGrdTriangle::CheckForOppositeNodeMatch (int edgenum, int nodenum)
{
    int         tri1, ntest;
    EDgeStruct  *eptr;

    if (edgenum < 0  ||  nodenum < 0) {
        return 0;
    }

    eptr = EdgeList + edgenum;
    if (eptr->deleted == 1) {
        return 0;
    }

    tri1 = eptr->tri1;
    if (tri1 == -1) {
        return 0;
    }

    ntest = OppositeNode (tri1, edgenum);
    if (ntest == nodenum) {
        return 1;
    }

    tri1 = eptr->tri2;
    if (tri1 == -1) {
        return 0;
    }

    ntest = OppositeNode (tri1, edgenum);
    if (ntest == nodenum) {
        return 1;
    }

    return 0;

}




/*
 *******************************************************************************************

                      R e m o v e N o d e O n C o n s t r a i n t

 *******************************************************************************************

  Remove the specified center node and stitch together the two specified edges.

*/

int CSWGrdTriangle::RemoveNodeOnConstraint (
    int    center_node,
    int    edge1,
    int    edge2)
{
    int    istat;

    istat =
    SwapCentralNodeEdges (center_node, edge1, edge2);
    if (istat == -1) {
        return -1;
    }

    istat =
    UnsplitFromCentralNode (center_node, edge1, edge2);

    if (istat == 1) {
        NodeList[center_node].deleted = 1;
    }

    return istat;

}


/*
 *******************************************************************************************

                     S w a p C e n t r a l N o d e E d g e s

 *******************************************************************************************

  Simplify the connected edges to the central node by swapping edges where possible.
  This should end up with 4 edges connected to the central node and the nodes at the
  far end of these edges form a quadralateral with the constraint edge as its
  diagonal.  If the central node is on the trimesh border, there will be 3 edges and
  3 points at the end of this swapping.

*/

int CSWGrdTriangle::SwapCentralNodeEdges (
    int    center_node,
    int    edge1,
    int    edge2)
{
    int    i, ndo, nswap, istat;
    int    *list, nlist;
    int    rp, maxdo;
    RAwPointStruct    *rptr;
    NOdeStruct        *nptr;
    EDgeStruct        *ep1, *ep2;
    double            x1, y1, x2, y2;
    int               n1, n2, n3, n4, nc1, nc2;
    int               minlist = 4;

    ep1 = EdgeList + edge1;
    ep2 = EdgeList + edge2;

    n1 = ep1->node1;
    n2 = ep1->node2;
    n3 = ep2->node1;
    n4 = ep2->node2;
    if (n1 == n3  ||  n1 == n4) {
        nc1 = n2;
    }
    else {
        nc1 = n1;
    }
    if (n3 == n1  ||  n3 == n2) {
        nc2 = n4;
    }
    else {
        nc2 = n3;
    }

    nptr = NodeList + center_node;
    if (nptr->on_border != 0) {
        minlist = 3;
    }

    rp = nptr->rp;
    if (rp < 0) {
        rp = nptr->crp;
        if (rp < 0) {
            printf
            ("\n\n!!!!!!!!  no raw point for central node in SwapCentralNode  !!!!!!!!\n");
            printf
            ("    center node = %d  edge1 = %d  edge2 = %d\n", center_node, edge1, edge2);
            assert (0);
        }
        rptr = ConstraintRawPoints + rp;
    }
    else {
        rptr = RawPoints + rp;
    }

    list = rptr->edgelist;
    nlist = rptr->nedge;

    maxdo = nlist * nlist + 1;
    ndo = 0;

    for (;;) {

        nswap = 0;
        for (i=0; i<nlist; i++) {
            if (list[i] == edge1  ||  list[i] == edge2) {
                continue;
            }
            if (EdgeList[list[i]].flag != 0) {
                continue;
            }
            istat =
            GetSwapCoords (list[i],
                           &x1, &y1, &x2, &y2,
                           &n1, &n2);
            if (n1 == -1  ||  n2 == -1  ||
                x1 > 1.e20  ||  y1 > 1.e20  ||
                x2 > 1.e20  ||  y2 > 1.e20) {
                continue;
            }
            if (n1 == nc1  &&  n2 == nc2) {
                continue;
            }
            if (n2 == nc1  &&  n1 == nc2) {
                continue;
            }

            istat = SwapEdge (list[i]);
            if (istat == 1) {
                nswap++;
                break;
            }
        }

        if (nswap == 0) {
            break;
        }

        list = rptr->edgelist;
        nlist = rptr->nedge;

        ndo++;

        if (ndo > maxdo) {
            printf
            ("\n\n  !!!!!!!! Looping too many times in SwapCentralNodeEdges !!!!!!!!\n\n");
            assert (0);
        }

    }

    nlist = rptr->nedge;
    if (nlist != minlist) {
        printf ("\n\n  !!!!!!!! The number of edges left after SwapCentralNodeEdges \n");
        printf ("             should be three for a border node or four otherwise.\n");
        printf
        ("  central node = %d  edge1 = %d  edge2 = %d\n\n", center_node, edge1, edge2);
        fflush (stdout);
        nlist = nlist;
    }

    return 1;

}



/*
 *********************************************************************

          C h e c k F o r N o d e O n C o n s t r a i n t

 *********************************************************************

  If the constraint line connecting the two specified raw points
  has another node on it between the two raw points, remove the node
  closest to point rp1.  If there are several intermediate nodes,
  they are removed one at a time by multiple calls to this function.

  If a node is removed, 1 is returned.  If no node needs to be
  removed, zero is returned.  On an error, -1 is returned.

*/

int CSWGrdTriangle::CheckForNodeOnConstraint (
    RAwPointStruct    *rp1,
    RAwPointStruct    *rp2)
{
    int              n1, n2, i, onseg, nchk, nchk2, edge2;
    int              *list, nlist, istat;
    EDgeStruct       *eptr;

    n1 = rp1->nodenum;
    n2 = rp2->nodenum;

    if (n1 < 0  ||  n2 < 0) {
        assert (0);
    }

    list = rp1->edgelist;
    nlist = rp1->nedge;

/*
 * If the constraint line is an exact edge, return zero.
 */
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        if (eptr->node1 == n2  ||
            eptr->node2 == n2) {
            return 0;
        }
    }

/*
 * Find a node on the constraint segment
 * and attempt to remove it.
 */
    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        nchk = eptr->node1;
        if (nchk == n1) {
            nchk = eptr->node2;
        }
        onseg = NodeOnSegment (nchk, n1, n2, NULL);
        if (onseg == 1) {
            nchk2 = CheckEdgeListForOppositeNodeOnSegment (
                nchk, n2, &edge2);
            if (nchk2 == -1) {
                return -1;
            }
            istat = RemoveNodeOnConstraint (
                nchk, list[i], edge2);
            if (istat == -1) {
                return -1;
            }
            return 1;
        }
    }

    return 0;

}




int CSWGrdTriangle::CheckEdgeListForOppositeNodeOnSegment (
    int        n1,
    int        n2,
    int        *edgenum)
{
    int        i, onseg, rp, nchk;
    int        *list, nlist;
    RAwPointStruct   *rptr;
    NOdeStruct       *nptr;
    EDgeStruct       *eptr;

/*
 * Get the list of edges attached to the n1 node.
 */
    nptr = NodeList + n1;
    rp = nptr->rp;
    if (rp == -1) {
        rp = nptr->crp;
        if (rp == -1) {
            return -1;
        }
        rptr = ConstraintRawPoints + rp;
    }
    else {
        rptr = RawPoints + rp;
    }

    list = rptr->edgelist;
    nlist = rptr->nedge;

/*
 * If the final node of the constraint segment is an
 * opposite node of an attached edge, return that node
 * and edge.
 */
    for (i=0; i<nlist; i++) {
        if (list[i] > NumEdges  ||  list[i] < 0) {
            assert (0);
        }
        eptr = EdgeList + list[i];
        if (eptr->node1 == n2  ||
            eptr->node2 == n2) {
            *edgenum = list[i];
            return n2;
        }
    }

/*
 * If an opposite node of an attached edge is on the constraint segment,
 * return that node and edge.
 */
    for (i=0; i<nlist; i++) {
        if (list[i] > NumEdges  ||  list[i] < 0) {
            assert (0);
        }
        eptr = EdgeList + list[i];
        nchk = eptr->node1;
        if (nchk == n1) {
            nchk = eptr->node2;
        }
        onseg = NodeOnSegment (nchk, n1, n2, NULL);
        if (onseg == 1) {
            *edgenum = list[i];
            return nchk;
        }
    }

    return -1;

}




/*
 *******************************************************************************************

                  U n s p l i t F r o m C e n t r a l N o d e

 *******************************************************************************************

  This is called after central node edge swapping has reduced the number of
  attached edges at the central node to 4 (or 3 if the central node is on the
  border of the trimesh.  This function creates two (or 1 in the on border case)
  triangles having the common edge connecting the opposite endpoints of edge1
  and edge2.

*/

int CSWGrdTriangle::UnsplitFromCentralNode (
    int    center_node,
    int    edge1,
    int    edge2)
{
    int               i, j, cflag, istat, ndone, done;
    int               nlist, newedge, ncommon;
    int               rp;
    RAwPointStruct    *rptr;
    NOdeStruct        *nptr;
    int               *nodes, *edges;
    int               nnodes;
    EDgeStruct        *ep1, *ep2, *eptr;
    int               nedges;
    int               n1, n2, n3, n4, nc1, nc2;
    int               tc1;
    int               *list;

    list = p_int_1000;

/*
 * Get the edges surrounding the center node.
 */
    istat = GetShellAroundNode (
        center_node,
        &nodes,
        &nnodes,
        &edges,
        &nedges);

    if (istat == -1) {
        return -1;
    }

  /* silence lint warnings */
    nodes = nodes;
    nnodes = nnodes;

/*
 * If the number of edges is not either 2 (for a border center node)
 * or 4 (for an interior center node), then something is wrong,
 * probably a program bug.
 */
    if (!(nedges == 2  ||  nedges == 4)) {
        printf
        ("there must be 2 or 4 edges in the shell around the center node\n");
        printf
        ("in UnsplitFromCentralNode center_node = %d  edge1 = %d, edge2 = %d\n",
         center_node, edge1, edge2);
        csw_Free (edges);
        csw_Free (nodes);
        return -1;
    }

/*
 * Get the nodes for the new spliced together edge along the
 * constraint segment.
 */
    ep1 = EdgeList + edge1;
    ep2 = EdgeList + edge2;

    cflag = ep1->flag;

    n1 = ep1->node1;
    n2 = ep1->node2;
    n3 = ep2->node1;
    n4 = ep2->node2;
    if (n1 == n3  ||  n1 == n4) {
        nc1 = n2;
    }
    else {
        nc1 = n1;
    }
    if (n3 == n1  ||  n3 == n2) {
        nc2 = n4;
    }
    else {
        nc2 = n3;
    }

/*
 * Get the list of nodes attached to the center node.
 */
    nptr = NodeList + center_node;

    rp = nptr->rp;
    if (rp < 0) {
        rp = nptr->crp;
        if (rp < 0) {
            printf
            ("\n\n!!!!!!!!  no raw point for central node in UnsplitFromCentralNode  !!!!!!!!\n");
            printf
            ("    center node = %d  edge1 = %d  edge2 = %d\n", center_node, edge1, edge2);
            assert (0);
        }
        rptr = ConstraintRawPoints + rp;
    }
    else {
        rptr = RawPoints + rp;
    }

    nlist = rptr->nedge;
    if (nlist > 1000) {
        csw_Free (edges);
        csw_Free (nodes);
        return -1;
    }

    for (i=0; i<nlist; i++) {
        list[i] = rptr->edgelist[i];
    }

/*
 * Remove the edges attached to the center node.  This will leave
 * a hole in the trimesh.
 */
    for (i=0; i<nlist; i++) {
        WhackEdge (list[i]);
    }

/*
 * Create a new edge connecting the constraint segment end nodes.
 */
    newedge = AddEdge (nc1, nc2, -1, -1, cflag);
    if (newedge < 0) {
        csw_Free (edges);
        csw_Free (nodes);
        return -1;
    }
    AddEdgeToNodeList (nc1, newedge);
    AddEdgeToNodeList (nc2, newedge);
    eptr = EdgeList + newedge;

/*
 * If the center node is on the trimesh border, then there are only
 * two edges in the shell around the center node.  These two edges,
 * new edge from the constraint segment, form a single triangle that
 * is added to the trimesh;
 */
    if (nedges == 2) {
        tc1 =
        AddTriangle (
            edges[0],
            edges[1],
            newedge,
            0);
        if (tc1 < 0) {
            csw_Free (edges);
            csw_Free (nodes);
            WhackEdge (newedge);
            return -1;
        }
        eptr->tri1 = tc1;
        ep1 = EdgeList + edges[0];
        ep1->tri2 = tc1;
        ep2 = EdgeList + edges[1];
        ep2->tri2 = tc1;
        csw_Free (edges);
        csw_Free (nodes);
        return 1;
    }

/*
 * If there are 4 edges in the shell around the center node, pair edges with
 * a common node that is not either of the constraint end nodes.  These pairs
 * of edges, along with the constraint edge, form a triangle that is added
 * to the trimesh topology.  There should be exactly 2 triangles added.
 */
    for (i=0; i<nedges; i++) {
        EdgeList[edges[i]].tflag2 = 0;
    }
    ndone = 0;
    for (i=0; i<nedges; i++) {
        ep1 = EdgeList + edges[i];
        if (ep1->tflag2 == 1) {
            continue;
        }
        done = 0;
        for (j=i+1; j<nedges; j++) {
            ep2 = EdgeList + edges[j];
            ncommon = CommonNode (edges[i], edges[j]);
            if (ncommon == -1) {
                continue;
            }
            if (ncommon == nc1  ||  ncommon == nc2) {
                continue;
            }
            tc1 = AddTriangle (edges[i], edges[j], newedge, 0);
            if (tc1 == -1) {
                csw_Free (edges);
                csw_Free (nodes);
                return -1;
            }
            if (ep1->tri1 == -1) {
                ep1->tri1 = tc1;
            }
            else {
                ep1->tri2 = tc1;
            }
            if (ep2->tri1 == -1) {
                ep2->tri1 = tc1;
            }
            else {
                ep2->tri2 = tc1;
            }
            if (eptr->tri1 == -1) {
                eptr->tri1 = tc1;
            }
            else {
                eptr->tri2 = tc1;
            }
            ep1->tflag2 = 1;
            ep2->tflag2 = 1;
            done = 1;
            ndone++;
        }

        if (done == 0) {
            printf ("\n!!!!!!!! No pair of edges found for unsplit !!!!!!!!\n");
            printf ("         center node = %d, edge1 = %d, edge2 = %d\n\n",
                    center_node, edge1, edge2);
            done = done;
        }

    }

    if (ndone != 2) {
        printf ("\n!!!!!!!! In Unsplit, ndone is not 2\n");
        printf ("         center node = %d, edge1 = %d, edge2 = %d\n\n",
                center_node, edge1, edge2);
        ndone = ndone;
    }

    csw_Free (edges);
    csw_Free (nodes);

    return 1;

}



/*-------------------------------------------------------------------*/

/*
 * Find nodes close to the constraints but not on them
 * and delete the nodes to get better shaped triangles.
 * This function only cjecks for opposite nodes in triangles
 * shared by the constraint edge.
 */
int CSWGrdTriangle::RemoveNodesCloseToConstraints (void)
{
    int             i, j, k, good, istat;
    int             *close_nodes = NULL, nclose;
    int             rp, *list = NULL, nlist;
    int             ndone;
    RAwPointStruct  *rptr = NULL;

    EDgeStruct      *eptr = NULL;
    int             t1, n1, nc1, nc2;
    int             maxclose;
    double          length, x1, y1, x2, y2, x3, y3, dist;


    auto fscope = [&]()
    {
        csw_Free (close_nodes);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Allocate space for the close node list.
 */
    maxclose = NumNodes * 2;
    if (maxclose < 1000) maxclose = 1000;
    close_nodes = (int *)csw_Malloc (maxclose * sizeof(int));
    if (close_nodes == NULL) {
        return -1;
    }

    nclose = 0;

/*
 * The spillnum member of the node class is set to 1 to flag that
 * the node is close to a constraint and thus needs to be removed.
 * Make sure these are all zero to start.
 */
    for (i=0; i<NumNodes; i++) {
        NodeList[i].spillnum = 0;
    }

/*
 * For each constraint edge, find the one or two opposite nodes
 * to the constraint edge on the triangles shared by the edge.
 * If a node is less than .25 * the length of the constraint
 * edge distant from the constraint edge, put the node number
 * into the remove node list.
 */
    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->deleted == 1) {
            continue;
        }
        if (eptr->flag == 0) {
            continue;
        }
        nc1 = eptr->node1;
        nc2 = eptr->node2;
        length = eptr->length / 2.5;
        x1 = NodeList[nc1].x;
        y1 = NodeList[nc1].y;
        x2 = NodeList[nc2].x;
        y2 = NodeList[nc2].y;

    /*
     * Check the opposite node on the first triangle.
     */
        t1 = eptr->tri1;
        n1 = OppositeNode (t1, i);
        if (n1 >= 0) {
            if (NodeList[n1].spillnum == 0) {
                x3 = NodeList[n1].x;
                y3 = NodeList[n1].y;
                istat =
                gpf_perpdistance2 (x1, y1, x2, y2, x3, y3, &dist);
                if (istat == 1) {
                    if (dist < length) {
                        close_nodes[nclose] = n1;
                        nclose++;
                        if (nclose >= NumNodes) {
                            assert (0);
                        }
                    }
                }
                NodeList[n1].spillnum = 1;
            }
        }

    /*
     * If there is a second triangle, check its opposite node.
     */
        t1 = eptr->tri2;
        if (t1 >= 0) {
            n1 = OppositeNode (t1, i);
            if (n1 >= 0) {
                if (NodeList[n1].spillnum == 0) {
                    x3 = NodeList[n1].x;
                    y3 = NodeList[n1].y;
                    istat =
                    gpf_perpdistance2 (x1, y1, x2, y2, x3, y3, &dist);
                    if (istat == 1) {
                        if (dist < length) {
                            close_nodes[nclose] = n1;
                            nclose++;
                            if (nclose >= NumNodes) {
                                assert (0);
                            }
                        }
                    }
                    NodeList[n1].spillnum = 1;
                }
            }
        }
    }

/*
 * Actually remove the close nodes.
 */
    ndone = 0;
    for (i=0; i<nclose; i++) {

        j = close_nodes[i];

    /*
     * Make sure that none of the edges attached to the node
     * are constraint edges.
     */
        if (NodeList[j].deleted == 1) {
            continue;
        }
        rp = NodeList[j].rp;
        if (rp == -1) {
            rp = NodeList[j].crp;
            if (rp == -1) {
                continue;
            }
            rptr = ConstraintRawPoints + rp;
        }
        else {
            rptr = RawPoints + rp;
        }

        list = rptr->edgelist;
        nlist = rptr->nedge;
        good = 1;
        for (k=0; k<nlist; k++) {
            eptr = EdgeList + list[k];
            if (eptr->flag != 0) {
                good = 0;
                break;
            }
        }

    /*
     * Only remove if there are no constraint edges attached.
     */
        if (good == 1) {
            istat = RemoveNode (j);
            if (istat == 1) {
                ndone++;
            }
        }
    }

    return ndone;

}


/*
 ******************************************************************************

            g r d _ r e m o v e _ s l i v e r _ t r i a n g l e s

 ******************************************************************************

*/

int CSWGrdTriangle::grd_remove_sliver_triangles (
    NOdeStruct         **nodes,
    EDgeStruct         **edges,
    TRiangleStruct     **triangles,
    int                *num_nodes,
    int                *num_edges,
    int                *num_triangles,
    double             edge_length_ratio)
{
    int                i, n1, n2, n3, istat;
    int                nchk, *nodelist = NULL, n;
    int                ndo, maxdo, jdo;
    TRiangleStruct     *tptr = NULL;
    EDgeStruct         *eptr = NULL;
    NOdeStruct         *nptr = NULL;
    RAwPointStruct     *rptr = NULL;
    double             xmin, ymin, xmax, ymax;

    double             d1, d2, d3, result, dtot, dmax;

    char               fname1[200];
    int                do_write;
    double             v6[6];


    auto fscope = [&]()
    {
        ListNull ();
        FreeMem ();
        ConvexHullFlag = 0;
        AverageEdgeLength = -1.0;
        ExactFlag = 0;
        MaxIndexPoint = -1;
        csw_Free (nodelist);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (edge_length_ratio <= 1.0  ||  edge_length_ratio >= 2.0) {
        edge_length_ratio = 1.25;
    }

    TriangleList = *triangles;
    EdgeList = *edges;
    NodeList = *nodes;
    NumTriangles = *num_triangles;
    NumEdges = *num_edges;
    NumNodes = *num_nodes;
    MaxNodes = NumNodes;
    MaxEdges = NumEdges;
    MaxTriangles = NumTriangles;

    nodelist = (int *)csw_Malloc (NumNodes * sizeof(int));
    if (nodelist == NULL) {
        return -1;
    }

/*
    Allocate space for the line points plus the original nodes.
*/
    n = NumNodes;
    RawPoints = (RAwPointStruct *)csw_Calloc (n * sizeof(RAwPointStruct));
    NumRawPoints = 0;
    MaxRawPoints = n;
    if (RawPoints == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
 * Put the original nodes into the raw points array.
 */
    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;

    rptr = RawPoints;
    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        rptr->x = nptr->x;
        rptr->y = nptr->y;
        rptr->z = nptr->z;
        rptr->nodenum = i;
        nptr->rp = rptr - RawPoints;
        nptr->crp = -1;
        if (rptr->x < xmin) xmin = rptr->x;
        if (rptr->y < ymin) ymin = rptr->y;
        if (rptr->x > xmax) xmax = rptr->x;
        if (rptr->y > ymax) ymax = rptr->y;
        rptr++;
    }

    FlagEdgeNodes ();
    BuildRawPointEdgeLists ();
    EdgeSwapFlag = 0;

/*
 * Check for slivers and remove nodes that produce the slivers.
 * This is done repeatedly because removing a node can introduce
 * a new sliver.  Edge swapping is done between repeats.
 */
    ndo = 0;
    maxdo = 10;
    for (;;) {
        n = 0;

        for (i=0; i<NumNodes; i++) {
            NodeList[i].spillnum = 0;
        }

    /*
     * Check each triangle for being a sliver.
     */
        for (i=0; i<NumTriangles; i++) {

            tptr = TriangleList + i;
            if (tptr->deleted == 1) {
                continue;
            }
            eptr = EdgeList + tptr->edge1;
            n1 = eptr->node1;
            n2 = eptr->node2;
            n3 = OppositeNode (i, tptr->edge1);

            d1 = NodeDistance (n1, n2);
            d2 = NodeDistance (n2, n3);
            d3 = NodeDistance (n3, n1);

            dtot = d1 + d2 + d3;

            dmax = d1;
            nchk = n3;
            if (d2 > dmax) {
                dmax = d2;
                nchk = n1;
            }
            if (d3 > dmax) {
                dmax = d3;
                nchk = n2;
            }

            dtot -= dmax;
            result = dtot / dmax;

            if (result < edge_length_ratio) {
                istat = CheckNeighborRemove (nchk);
                if (istat == 0) {
                    NodeList[nchk].spillnum = 1;
                    nodelist[n] = nchk;
                    n++;
                }
            }

        }

    /*
     * If no slivers were found, break out of the repeat loop.
     */
        if (n == 0) {
            break;
        }

        for (i=0; i<n; i++) {
            RemoveNode (nodelist[i]);
        }

        ndo++;
        if (ndo > maxdo) {
            break;
        }

        SwapFlags = (char *)csw_Calloc (NumEdges * sizeof(char));
        FinalSwapFlag = 1;
        CornerBias = CORNER_BIAS;
        jdo = 0;
        for (;;) {
            NumSwapped = 0;
            for (i=0; i<NumEdges; i++) {
                if (EdgeList[i].flag != 0) {
                    continue;  /* this is a constraint edge */
                }
                if (EdgeList[i].on_border == 1) {
                    continue;
                }
                if (SwapFlags[i] >= 1) continue;
                SwapEdge (i);
            }
            if (NumSwapped == 0) {
                break;
            }
            for (i=0; i<NumEdges; i++) {
                if (SwapFlags[i] != 2) SwapFlags[i] = 0;
            }
            jdo++;
            if (jdo > maxdo) {
                break;
            }
        }
        csw_Free (SwapFlags);
        SwapFlags = NULL;
        FinalSwapFlag = 0;

    }

    csw_Free (nodelist);
    nodelist = NULL;

    RemoveDeletedElements ();

    do_write = csw_GetDoWrite ();;
    if (do_write) {
        sprintf (fname1, "nosliver.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

/*
    Transfer the results to the output arrays.
*/
    *num_nodes = NumNodes;
    *num_edges = NumEdges;
    *num_triangles = NumTriangles;
    *nodes = NodeList;
    *edges = EdgeList;
    *triangles = TriangleList;


    return 1;

}



int CSWGrdTriangle::CheckNeighborRemove (int nodenum)
{
    NOdeStruct        *nptr;
    EDgeStruct        *eptr;
    int               n1, n2, nchk;
    int               rp, i, nlist, *list;
    RAwPointStruct    *rptr;

    nptr = NodeList + nodenum;
    rp = nptr->rp;
    if (rp < 0) {
        rp = nptr->crp;
        if (rp < 0) {
            assert (rp >= 0);
        }
        rptr = ConstraintRawPoints + rp;
    }
    else {
        rptr = RawPoints + rp;
    }
    list = rptr->edgelist;
    nlist = rptr->nedge;

    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        n1 = eptr->node1;
        n2 = eptr->node2;
        if (n1 == nodenum) {
            nchk = n2;
        }
        else {
            nchk = n1;
        }
        if (NodeList[nchk].spillnum == 1) {
            return 1;
        }
    }

    return 0;

}



/*
 ******************************************************************************

               V a l i d a t e T r i a n g l e S h a p e s

 ******************************************************************************

*/

void CSWGrdTriangle::ValidateTriangleShapes (void)
{
    int                i, n1, n2, n3;
    TRiangleStruct     *tptr;
    EDgeStruct         *eptr;
    int                n14, n13, n12, n11;
    int                iworst;
    double             d1, d2, d3, result, dtot, dmax;
    double             sum, sum2, best, worst;
    char               *cenv;

    if (ForceValidate == 0) {
        cenv = csw_getenv ("GRD_VALIDATE_TRIMESH_TOPO");
        if (cenv == NULL) return;
    }

/*
 * Check each triangle for being a sliver.
 */
    sum = 0.0;
    sum2 = 0.0;
    n14 = 0;
    n13 = 0;
    n12 = 0;
    n11 = 0;
    best = 0.0;
    worst = 3.0;
    iworst = -1;
    for (i=0; i<NumTriangles; i++) {

        tptr = TriangleList + i;
        if (tptr->deleted == 1) {
            continue;
        }
        eptr = EdgeList + tptr->edge1;
        n1 = eptr->node1;
        n2 = eptr->node2;
        n3 = OppositeNode (i, tptr->edge1);

        d1 = NodeDistanceXYZ (n1, n2);
        d2 = NodeDistanceXYZ (n2, n3);
        d3 = NodeDistanceXYZ (n3, n1);

        dtot = d1 + d2 + d3;

        dmax = d1;
        if (d2 > dmax) {
            dmax = d2;
        }
        if (d3 > dmax) {
            dmax = d3;
        }

        dtot -= dmax;
        result = dtot / dmax;

        sum += result;
        sum2++;

        if (result < 1.4) {
            n14++;
        }
        if (result < 1.3) {
            n13++;
        }
        if (result < 1.2) {
            n12++;
        }
        if (result < 1.1) {
            n11++;
        }

        if (result > best) {
            best = result;
        }
        if (result < worst) {
            worst = result;
            iworst = i;
        }

    }

    if (sum2 <= 0.0) {
        return;
    }

    sum /= sum2;

    printf ("\nTriangle shape validation:\n");
    printf ("  Total number of triangles = %d\n", (int)sum2);
    printf ("  Average distance ratio = %.2f\n", sum);
    printf ("  Best distance ratio = %.2f\n", best);

    printf ("  Worst distance ratio = %.2f\n", worst);
    if (iworst >= 0) {
        printf ("  located at triangle %d  edges %d %d %d\n", iworst,
                TriangleList[iworst].edge1,
                TriangleList[iworst].edge2,
                TriangleList[iworst].edge3);
    }

    printf ("  Number less than 1.4 = %d\n", n14);
    printf ("  Number less than 1.3 = %d\n", n13);
    printf ("  Number less than 1.2 = %d\n", n12);
    printf ("  Number less than 1.1 = %d\n", n11);
    printf ("\n");

    printf ("\nFinished Triangle shape validation:\n\n\n");

    return;

}

/*-------------------------------------------------------------------*/

/*
 * Find nodes close to the constraints but not on them
 * and delete the nodes to get better shaped triangles.
 * This function removes any node connected to either
 * end point of a constraint if it is close enough.
 */
int CSWGrdTriangle::RemoveNodesCloseToConstraints2 (void)
{
    int             i, j, k, good, istat;
    int             *close_nodes = NULL, nclose;
    int             rp, *list = NULL, nlist;
    int             ndone;
    RAwPointStruct  *rptr = NULL;

    EDgeStruct      *eptr = NULL;
    int             n1, nc1, nc2;
    int             maxclose;
    double          length, x1, y1, x2, y2, x3, y3, dist;


    auto fscope = [&]()
    {
        csw_Free (close_nodes);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Allocate space for the close node list.
 */
    maxclose = NumNodes * 2;
    if (maxclose < 1000) maxclose = 1000;
    close_nodes = (int *)csw_Malloc (maxclose * sizeof(int));
    if (close_nodes == NULL) {
        return -1;
    }

    nclose = 0;

/*
 * The spillnum member of the node class is set to 1 to flag that
 * the node is close to a constraint and thus needs to be removed.
 * Make sure these are all zero to start.
 */
    for (i=0; i<NumNodes; i++) {
        NodeList[i].spillnum = 0;
    }

/*
 * For each constraint edge, find the one or two opposite nodes
 * to the constraint edge on the triangles shared by the edge.
 * If a node is less than .25 * the length of the constraint
 * edge distant from the constraint edge, put the node number
 * into the remove node list.
 */
    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->deleted == 1) {
            continue;
        }
        if (eptr->flag == 0) {
            continue;
        }
        nc1 = eptr->node1;
        nc2 = eptr->node2;
        length = eptr->length / 2.5;
        x1 = NodeList[nc1].x;
        y1 = NodeList[nc1].y;
        x2 = NodeList[nc2].x;
        y2 = NodeList[nc2].y;

    /*
     * Check nodes attached to the first end point.
     */
        rp = NodeList[nc1].rp;
        if (rp == -1) {
            rp = NodeList[nc1].crp;
            if (rp == -1) {
                continue;
            }
            rptr = ConstraintRawPoints + rp;
        }
        else {
            rptr = RawPoints + rp;
        }

        list = rptr->edgelist;
        nlist = rptr->nedge;
        for (j=0; j<nlist; j++) {
            n1 = EdgeList[list[j]].node1;
            if (n1 == nc1) {
                n1 = EdgeList[list[j]].node2;
            }
            if (n1 >= 0) {
                if (NodeList[n1].spillnum == 1) {
                    continue;
                }
                x3 = NodeList[n1].x;
                y3 = NodeList[n1].y;
                istat =
                gpf_perpdistance2 (x1, y1, x2, y2, x3, y3, &dist);
                if (istat == 1) {
                    if (dist < length) {
                        close_nodes[nclose] = n1;
                        NodeList[n1].spillnum = 1;
                        nclose++;
                        if (nclose > NumNodes) {
                            assert (0);
                        }
                    }
                }
            }
        }

    /*
     * Check nodes attached to the second end point.
     */
        rp = NodeList[nc2].rp;
        if (rp == -1) {
            rp = NodeList[nc2].crp;
            if (rp == -1) {
                continue;
            }
            rptr = ConstraintRawPoints + rp;
        }
        else {
            rptr = RawPoints + rp;
        }

        list = rptr->edgelist;
        nlist = rptr->nedge;
        for (j=0; j<nlist; j++) {
            if (list[j] > NumEdges  ||  list[j] < 0) {
                assert (0);
            }
            n1 = EdgeList[list[j]].node1;
            if (n1 == nc2) {
                n1 = EdgeList[list[j]].node2;
            }
            if (n1 >= 0) {
                if (NodeList[n1].spillnum == 1) {
                    continue;
                }
                x3 = NodeList[n1].x;
                y3 = NodeList[n1].y;
                istat =
                gpf_perpdistance2 (x1, y1, x2, y2, x3, y3, &dist);
                if (istat == 1) {
                    if (dist < length) {
                        close_nodes[nclose] = n1;
                        NodeList[n1].spillnum = 1;
                        nclose++;
                        if (nclose >= NumNodes) {
                            assert (0);
                        }
                    }
                }
            }
        }

    }

/*
 * reset the spillnums to zero.
 */
    for (i=0; i<NumNodes; i++) {
        NodeList[i].spillnum = 0;
    }

/*
 * Actually remove the close nodes.
 */
    ndone = 0;
    for (i=0; i<nclose; i++) {

        j = close_nodes[i];

    /*
     * Make sure that none of the edges attached to the node
     * are constraint edges.
     */
        if (NodeList[j].deleted == 1) {
            continue;
        }
        rp = NodeList[j].rp;
        if (rp == -1) {
            rp = NodeList[j].crp;
            if (rp == -1) {
                continue;
            }
            rptr = ConstraintRawPoints + rp;
        }
        else {
            rptr = RawPoints + rp;
        }

        list = rptr->edgelist;
        nlist = rptr->nedge;
        good = 1;
        for (k=0; k<nlist; k++) {
            eptr = EdgeList + list[k];
            if (eptr->flag != 0) {
                good = 0;
                break;
            }
        }

    /*
     * Only remove if there are no constraint edges attached.
     */
        if (good == 1) {
            istat = RemoveNode (j);
            if (istat == 1) {
                ndone++;
            }
        }
    }

    return ndone;

}

/*
 ******************************************************************************

                     V a l i d a t e T r i a n g l e s

 ******************************************************************************

*/

void CSWGrdTriangle::ValidateTriangles (char *msg)
{
    int                i, n1, n2;
    TRiangleStruct     *tptr;
    EDgeStruct         *ep1, *ep2, *ep3;
    char               *cenv;

    if (ForceValidate == 0) {
        cenv = csw_getenv ("GRD_VALIDATE_TRIMESH_TOPO");
        if (cenv == NULL) return;
    }

    printf ("\nValidating triangle edge connectiveness\n");
    if (msg != NULL) {
        printf ("%s\n", msg);
    }

/*
 * Make sure the edges of each non deleted triangle
 * connect with each other.
 */
    for (i=0; i<NumTriangles; i++) {

        tptr = TriangleList + i;
        if (tptr->deleted == 1) {
            continue;
        }

        ep1 = EdgeList + tptr->edge1;
        ep2 = EdgeList + tptr->edge2;
        ep3 = EdgeList + tptr->edge3;

        n1 = ep1->node1;
        n2 = ep1->node2;

        if (!(ep2->node1 == n1  ||  ep2->node2 == n1  ||
              ep2->node1 == n2  ||  ep2->node2 == n2)) {
            printf ("Triangle number %d has bad edges\n", i);
            printf ("%d %d %d\n", tptr->edge1, tptr->edge2, tptr->edge3);

        }

        else if (!(ep3->node1 == n1  ||  ep3->node2 == n1  ||
              ep3->node1 == n2  ||  ep3->node2 == n2)) {
            printf ("Triangle number %d has bad edges\n", i);
            printf ("%d %d %d\n", tptr->edge1, tptr->edge2, tptr->edge3);

        }

    }

    if (msg != NULL) {
        printf ("%s\n", msg);
    }
    printf ("\nFinished Validating triangle edge connectiveness\n\n\n");

    return;

}


/*------------------------------------------------------------------------------*/

int CSWGrdTriangle::CorrectOutsideConstraintPoints (void)
{
    int               i, j, istat;
    int               n1, n2, newnode;
    int               nedges, ndone, jmin;
    double            xr, yr, zr, x1, y1, x2, y2, dist, dmin;
    EDgeStruct        *eptr;
    RAwPointStruct    *rptr;

/*
 * If there are no constraint points, do nothing.
 */
    if (MaxIndexPoint < 0) {
        return 1;
    }

    AdjustForOutsideConstraintPoints ();

/*
 * Loop through all the raw constraint points.  If a raw point doesn't have
 * a node associated with it, extend the trimesh by splitting the closest
 * border edge.
 */
    ndone = 0;

    for (i=MaxIndexPoint; i<NumRawPoints; i++) {

        rptr = RawPoints + i;
        if (rptr->nodenum >= 0) {
            continue;
        }
        xr = rptr->x;
        yr = rptr->y;
        zr = rptr->z;
        dmin = 1.e30;
        jmin = -1;
        nedges = NumEdges;

        for (j=0; j<nedges; j++) {
            eptr = EdgeList + j;
            if (eptr->on_border == 0) {
                continue;
            }
            n1 = eptr->node1;
            n2 = eptr->node2;
            x1 = NodeList[n1].x;
            y1 = NodeList[n1].y;
            x2 = NodeList[n2].x;
            y2 = NodeList[n2].y;

            istat = gpf_perpdistance2 (
                x1, y1, x2, y2, xr, yr, &dist);
            if (istat == 0) {
                continue;
            }

            if (dist < dmin) {
                dmin = dist;
                jmin = j;
            }
        }

        if (jmin >= 0) {
            if (dmin > EdgeList[jmin].length) {
                if (dmin < 1.5 * EdgeList[jmin].length) {
                    CorrectBorderEdgeForClippedConstraint (i);
                }
                continue;
            }
        }

        if (jmin >= 0) {
            newnode = AddNode (xr, yr, zr, 0);
            if (newnode == -1) {
                return -1;
            }

            istat = SplitFromEdge (jmin, newnode, NULL);
            if (istat == -1) {
                return -1;
            }

            rptr->nodenum = newnode;
            NodeList[newnode].rp = i;

            ndone++;
        }

        else {
            printf ("Cannot extend trimesh to include raw point number %d\n", i);
        }

    }

    return ndone;

}

/*
 ************************************************************************************

                 C h e c k Z e r o A r e a T r i a n g l e

 ************************************************************************************

  Return 1 if the triangle has essentially zero area.
*/

int CSWGrdTriangle::CheckZeroAreaTriangle (int index)
{
    double          x1, y1, z1, x2, y2, z2, x3, y3, z3, area, tiny;
    double          a, b, c, p, dx, dy, dz;
    int             i, n1, n2, n3;
    NOdeStruct      *np;
    EDgeStruct      *ep;



    tiny = AreaPerimeter / 200000.0;
    tiny *= tiny;

    i = index;

    if (TriangleList[i].deleted == 1) {
        return 0;
    }

/*
 * Calculate the area of the 3d polygon from the
 * lengths of its sides.
 */
    ep = EdgeList + TriangleList[i].edge1;
    n1 = ep->node1;
    n2 = ep->node2;
    ep = EdgeList + TriangleList[i].edge2;
    if (ep->node1 != n1  &&  ep->node1 != n2) {
        n3 = ep->node1;
    }
    else {
        n3 = ep->node2;
    }

    np = NodeList + n1;
    x1 = np->x;
    y1 = np->y;
    z1 = np->z;
    if (z1 > 1.e20  ||  z1 < -1.e20) {
        z1 = 0.0;
    }

    np = NodeList + n2;
    x2 = np->x;
    y2 = np->y;
    z2 = np->z;
    if (z2 > 1.e20  ||  z2 < -1.e20) {
        z2 = 0.0;
    }

    np = NodeList + n3;
    x3 = np->x;
    y3 = np->y;
    z3 = np->z;
    if (z3 > 1.e20  ||  z3 < -1.e20) {
        z3 = 0.0;
    }

    dx = x2 - x1;
    dy = y2 - y1;
    dz = 0.0;
    a = dx * dx + dy * dy + dz * dz;
    a = sqrt (a);

    dx = x3 - x2;
    dy = y3 - y2;
    dz = 0.0;
    b = dx * dx + dy * dy + dz * dz;
    b = sqrt (b);

    dx = x1 - x3;
    dy = y1 - y3;
    dz = 0.0;
    c = dx * dx + dy * dy + dz * dz;
    c = sqrt (c);

    p = a + b + c;
    p /= 2.0;
    if (a >= p  ||  b >= p  ||  c >= p) {
        return 0;
    }

    area = p * (p - a) * (p - b) * (p - c);
    area = sqrt (area);
    area /= 4.0;

/*
 * If the area is very near zero, return 1, otherwise return zero.
 */
    if (area > tiny) {
        return 0;
    }

    return 1;

}  /* end of private CheckZeroAreaTriangle function */



/*------------------------------------------------------------------------------*/

int CSWGrdTriangle::grd_insert_crossing_points (
    double      *x1in,
    double      *y1in,
    double      *z1in,
    int         *tag1in,
    int         n1in,
    double      *x2in,
    double      *y2in,
    double      *z2in,
    int         *tag2in,
    int         n2in,
    double      *x1out,
    double      *y1out,
    double      *z1out,
    int         *tag1out,
    int         *n1out,
    double      *x2out,
    double      *y2out,
    double      *z2out,
    int         *tag2out,
    int         *n2out,
    int         maxout,
    double      *xwork,
    double      *ywork,
    double      *zwork,
    int         *tagwork,
    int         maxwork)

{
    double      *xw, *yw, *zw;
    int         *tagw;
    int         n, i, j, ntot;
    double      x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
    double      xint, yint, zt1, zt2;
    int         istat;
    double      *xout, *yout, *zout;
    int         *tagout;
    int         same;
    double      tiny;

    CSWPolyUtils             ply_utils_obj;

/*
 * Initialize output in case of error.
 */
    *n1out = *n2out = 0;

/*
 * Use the work space specified in the calling parameters.
 */
    ntot = maxwork;
    if (maxout < maxwork) ntot = maxout;
    xw = xwork;
    yw = ywork;
    zw = zwork;
    tagw = tagwork;

    tiny = 1.e-6;

/*
 * Insert the crossings into the x1, y1, z1 line.
 */
    xw[0] = x1in[0];
    yw[0] = y1in[0];
    zw[0] = z1in[0];
    tagw[0] = 0;
    n = 1;
    for (i=1; i<n1in; i++) {

        x1 = x1in[i-1];
        y1 = y1in[i-1];
        z1 = z1in[i-1];
        x2 = x1in[i];
        y2 = y1in[i];
        z2 = z1in[i];

    /*
     * BUGFIX !!!!
     */
        same = 0;
        for (j=1; j<n2in; j++) {

            x3 = x2in[j-1];
            y3 = y2in[j-1];
            z3 = z2in[j-1];
            x4 = x2in[j];
            y4 = y2in[j];
            z4 = z2in[j];

            istat =
            ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4, &xint, &yint);

            if (istat != 0) {
                continue;
            }

        /*
         * Bug 9426
         * If the intersection point is essentially the same as either
         * ith segment endpoint, do not use it as a crossing.
         */
            same = SamePointTiny (x1, y1, xint, yint, tiny);
            if (same) continue;
            same = SamePointTiny (x2, y2, xint, yint, tiny);
            if (same) continue;

            xw[n] = xint;
            yw[n] = yint;
            tagw[n] = 1;
            zt1 = InterpolateZ (
                x1, y1, z1,
                x2, y2, z2,
                xint, yint);
            zt2 = InterpolateZ (
                x3, y3, z3,
                x4, y4, z4,
                xint, yint);
            zw[n] = (zt1 + zt2) / 2.0;
            n++;
            if (n >= ntot) {
                csw_Free (xw);
                return -1;
            }

        }

        xw[n] = x2;
        yw[n] = y2;
        zw[n] = z2;
        if (tag1in != NULL) {
            tagw[n] = tag1in[i];
        }
        else {
            tagw[n] = 0;
        /*
         * BUGFIX !!!!
         */
            if (same) tagw[n] = 1;
        }
        n++;

        if (n >= ntot) {
            csw_Free (xw);
            return -1;
        }

    }

/*
 * Allocate space for the first line's output and
 * copy the work arrays into the output.
 */
    xout = x1out;
    yout = y1out;
    zout = z1out;
    tagout = tag1out;

    memcpy (xout, xw, n * sizeof(double));
    memcpy (yout, yw, n * sizeof(double));
    memcpy (zout, zw, n * sizeof(double));
    memcpy (tagout, tagw, n * sizeof(int));

    *n1out = n;

    xout = NULL;
    yout = NULL;
    zout = NULL;
    tagout = NULL;

/*
 * Insert the crossings into the x2, y2, z2 line.
 */
    xw[0] = x2in[0];
    yw[0] = y2in[0];
    zw[0] = z2in[0];
    tagw[n] = 0;
    n = 1;
    for (i=1; i<n2in; i++) {

        x1 = x2in[i-1];
        y1 = y2in[i-1];
        z1 = z2in[i-1];
        x2 = x2in[i];
        y2 = y2in[i];
        z2 = z2in[i];

        for (j=1; j<n1in; j++) {

            x3 = x1in[j-1];
            y3 = y1in[j-1];
            z3 = z1in[j-1];
            x4 = x1in[j];
            y4 = y1in[j];
            z4 = z1in[j];

            istat =
            ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4, &xint, &yint);

            if (istat != 0) {
                continue;
            }

        /*
         * Bug 9426
         * If the intersection point is essentially the same as either
         * ith segment endpoint, do not use it as a crossing.
         */
            same = SamePointTiny (x1, y1, xint, yint, tiny);
            if (same) continue;
            same = SamePointTiny (x2, y2, xint, yint, tiny);
            if (same) continue;

            xw[n] = xint;
            yw[n] = yint;
            tagw[n] = 1;
            zt1 = InterpolateZ (
                x1, y1, z1,
                x2, y2, z2,
                xint, yint);
            zt2 = InterpolateZ (
                x3, y3, z3,
                x4, y4, z4,
                xint, yint);
            zw[n] = (zt1 + zt2) / 2.0;
            n++;
            if (n >= ntot) {
                return -1;
            }

        }

        xw[n] = x2;
        yw[n] = y2;
        zw[n] = z2;
        if (tag2in != NULL) {
            tagw[n] = tag2in[i];
        }
        else {
            tagw[n] = 0;
        }
        n++;

        if (n >= ntot) {
            return -1;
        }

    }

/*
 * Allocate space for the second line's output and
 * copy the work arrays into the output.
 */
    xout = x2out;
    yout = y2out;
    zout = z2out;
    tagout = tag2out;

    memcpy (xout, xw, n * sizeof(double));
    memcpy (yout, yw, n * sizeof(double));
    memcpy (zout, zw, n * sizeof(double));
    memcpy (tagout, tagw, n * sizeof(int));

    *n2out = n;

    return 1;

}


/*-----------------------------------------------------------*/

/*
 * Return a linearly interpolated z value at the
 * point xint, yint, which is on the line defined
 * by x1, y1, z1 and x2, y2, z2.
 */
double CSWGrdTriangle::InterpolateZ (
                double x1, double y1, double z1,
                double x2, double y2, double z2,
                double xint, double yint)
{
    double         dx, dy, adx, ady, pct;
    double         zt;

    dx = x2 - x1;
    dy = y2 - y1;

/*
 * If the end points are coincident, return
 * the average of the two z values.
 */
    if (dx == 0.0  &&  dy == 0.0) {
        zt = (z1 + z2) / 2.0;
        return zt;
    }

/*
 * Use the largest dimension (x or y) to interpolate
 * the percent of the segment where xint, yint lies.
 */
    adx = dx;
    ady = dy;
    if (adx < 0.0) adx = -adx;
    if (ady < 0.0) ady = -ady;

    if (adx > ady) {
        pct = (xint - x1) / dx;
    }
    else {
        pct = (yint - y1) / dy;
    }

    zt = z1 + pct * (z2 - z1);

    return zt;

}



/*--------------------------------------------------------------*/

/*
 * Decimate the specified line, without removing any point with
 * a non zero tag.
 */
int CSWGrdTriangle::grd_decimate_tagged_line (
    double      *x,
    double      *y,
    double      *z,
    int         *tag,
    int         *npts)
{
    x = x;
    y = y;
    z = z;
    tag = tag;
    npts = npts;

    return 1;
}




/*
 *******************************************************************************

           g r d _ d r a p e _ p o i n t s _ o n _ t r i _ m e s h

 *******************************************************************************

  Calculate the 3d points where the specified x,y points lie on the specified
  tri mesh surface.  The z values of the points are calculated from the plane
  of the triangle where the point is located.

  The xout, yout and zout arrays are each allocated here and must be csw_Freed by
  the calling function.

*/

int CSWGrdTriangle::grd_drape_points_on_tri_mesh (int id1, int id2,
                                 NOdeStruct *nodes,
                                 int numnodes,
                                 EDgeStruct *edges,
                                 int numedges,
                                 TRiangleStruct *triangles,
                                 int numtriangles,
                                 double *xpts,
                                 double *ypts,
                                 int npts,
                                 double **xout,
                                 double **yout,
                                 double **zout,
                                 int *nout)

{
    int             istat, i, n, itri;

    TRiangleIndexListStruct   *tiptr = NULL;
    NOdeEdgeCacheStruct       *nptr = NULL;
    TRiangleStruct            *tptr = NULL;
    double                    xtri[4], ytri[4], ztri[4];
    double                    xt, yt, zt, coef[3];

    bool       bsuccess = false;

    auto fscope = [&]()
    {
        ListNull ();
        FreeMem ();
        if (bsuccess == false) {
            csw_Free (Xout);
            Xout = NULL;
            csw_Free (Yout);
            Yout = NULL;
            csw_Free (Zout);
            Zout = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * first, clean up anything in private pointers that might be hanging around.
 * and initialize output in case an error occurs
 */
    FreeMem ();

    *xout = NULL;
    *yout = NULL;
    *zout = NULL;
    *nout = 0;

    NumBoundaryEdges = 0;

    Nsearch = 0;
    Ntpt = 0;
    Nsearch = Nsearch;
    Ntpt = Ntpt;

/*
 * transfer the specified trimesh to private variables.
 */
    NodeList = nodes;
    EdgeList = edges;
    TriangleList = triangles;
    NumNodes = numnodes;
    NumEdges = numedges;
    NumTriangles = numtriangles;

/*
 * Create an index for quick triangle spatial lookup and a list
 * of edges connected to each node.
 */
    tiptr = FindTriIndex (id1, id2);
    if (tiptr == NULL) {
        istat = CreateTriangleIndexGrid ();
        if (istat == -1) {
            return -1;
        }

        AddStaticTriIndex (id1, id2);
    }
    else {
        grd_set_triangle_index (
            tiptr->index,
            tiptr->xmin,
            tiptr->ymin,
            tiptr->xmax,
            tiptr->ymax,
            tiptr->ncol,
            tiptr->nrow,
            tiptr->xspace,
            tiptr->yspace);
    }

    nptr = FindNodeEdge (id1, id2);
    if (nptr == NULL) {
        istat = BuildNodeEdgeLists ();
        if (istat == -1) {
            return -1;
        }
        AddStaticNodeEdge (id1, id2);
    }
    else {
        NodeEdgeList = nptr->list;
        NumNodeEdgeList = nptr->nlist;
    }

    XoutTiny = (TriangleIndexXspace + TriangleIndexYspace) / 2000.0;

/*
 * allocate the initial output arrays using the private variables
 * these all can grow if needed.
 */
    Maxpout = npts;
    Xout = (double *)csw_Malloc (Maxpout * sizeof(double));
    if (Xout == NULL) {
        return -1;
    }

    Yout = (double *)csw_Malloc (Maxpout * sizeof(double));
    if (Yout == NULL) {
        return -1;
    }

    Zout = (double *)csw_Malloc (Maxpout * sizeof(double));
    if (Zout == NULL) {
        return -1;
    }

/*
 * Find the triangle that contains each point and interpolate
 * the z value from the plane of the triangle.
 */
    n = 0;
    for (i=0; i<npts; i++) {
        xt = xpts[i];
        yt = ypts[i];
        itri = FindTriangleContainingPoint (xt, yt);
        if (itri < 0) continue;

        tptr = TriangleList + itri;
        TrianglePoints (tptr, xtri, ytri, ztri);
        grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);

        zt = coef[0] + xt * coef[1] + yt * coef[2];

        Xout[n] = xt;
        Yout[n] = yt;
        Zout[n] = zt;
        n++;
    }

/*
 * Successful completion.
 */
    *xout = Xout;
    *yout = Yout;
    *zout = Zout;
    *nout = n;

    Xout = NULL;
    Yout = NULL;
    Zout = NULL;

    return 1;

}  /* end of function grd_drape_points_on_tri_mesh */




/*----------------------------------------------------------------------------*/

/*
 * Find fault edges that connect into closed polygons
 * and whack any trimesh edges which are inside the
 * fault polygons.  This will leave the fault edges that
 * define the polygon with only a single triangle
 * that uses the edge.
 */
int CSWGrdTriangle::WhackEdgesInsideFaultPolygons (void)
{
    int          i, istat;
    EDgeStruct   *ep;

    BuildNodeEdgeLists ();

/*
 * Initialize the edge tflag2 members so they
 * can be used as flags for edges that have
 * already been processed.
 */
    for (i=0; i<NumEdges; i++) {
        EdgeList[i].tflag2 = 0;
    }

/*
 * Loop through all the edges and check fault edges
 * for being part of a polygon.
 */
    for (i=0; i<NumEdges; i++) {

        ep = EdgeList + i;
        if (ep->deleted == 1) {
            continue;
        }
        if (!(ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
              ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT)) {
            continue;
        }
        if (ep->tflag2 == 1) {
            continue;
        }

        istat = ConnectFaultEdges (i);
        if (istat == -1) {
            return -1;
        }

    }

/*
 * Reset the tflag2 members to zero.
 */
    for (i=0; i<NumEdges; i++) {
        EdgeList[i].tflag2 = 0;
    }

    return 1;

}


/*-----------------------------------------------------------------------------*/

/*
 * If the specified edge can connect via other fault
 * edges back to itself, a fault polygon is built and
 * trimesh edges inside the polygon are whacked.
 */
int CSWGrdTriangle::ConnectFaultEdges (int edgenum)
{
    int             i, found, istat, n1, n2, n1start, n2start;
    int             *list, nlist, enow, lineid;
    int             nodenow, n;
    double          xmid, ymid, xmin, ymin, xmax, ymax;
    EDgeStruct      *ep;
    NOdeStruct      *np;

    int      *poly_nodes;
    double   *xpoly;
    double   *ypoly;

    poly_nodes = p_int_mce;
    xpoly = p_double_mce1;
    ypoly = p_double_mce2;

    CSWPolyUtils             ply_utils_obj;

/*
 * Set the grazing distance for the point in polygon operation.
 */
    ply_utils_obj.ply_setgraze (GrazeDistance / 10.0);

/*
 * Set the start nodes to the edgenum's nodes.
 */
    ep = EdgeList + edgenum;
    ep->tflag2 = 1;
    n1start = ep->node1;
    n2start = ep->node2;
    lineid = ep->lineid;

    nodenow = n1start;
    enow = edgenum;
    poly_nodes[0] = nodenow;
    n = 1;

/*
 * Traverse from node to node using fault edges until
 * a dead end is found or until the fault closes on itself.
 */
    for (;;) {

        list = NodeEdgeList[nodenow].list;
        if (list == NULL) {
            return 0;
        }
        nlist = NodeEdgeList[nodenow].nlist;
        found = 0;
        for (i=0; i<nlist; i++) {
            if (list[i] == enow) {
                continue;
            }
            ep = EdgeList + list[i];
            if (ep->deleted == 1) {
                continue;
            }
            if (!(ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
                  ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT)) {
                continue;
            }
            if (ep->lineid != lineid) {
                continue;
            }
            ep->tflag2 = 1;
            if (ep->node1 == nodenow) {
                nodenow = ep->node2;
            }
            else {
                nodenow = ep->node1;
            }
            enow = list[i];
            poly_nodes[n] = nodenow;
            n++;
            found = 1;
            break;
        }

        if (found == 0  ||  n >= MAX_CONNECT_EDGES_TRI) {
            return 0;
        }

        if (nodenow == n1start) {
            break;
        }
        if (nodenow == n2start) {
            poly_nodes[n] = n1start;
            n++;
            break;
        }

    }

/*
 * Make a polygon from the nodes and find its x,y limits.
 */
    xmin = ymin = 1.e30;
    xmax = ymax = -1.e30;

    for (i=0; i<n; i++) {

        np = NodeList + poly_nodes[i];
        xpoly[i] = np->x;
        ypoly[i] = np->y;

        if (np->x < xmin) xmin = np->x;
        if (np->y < ymin) ymin = np->y;
        if (np->x > xmax) xmax = np->x;
        if (np->y > ymax) ymax = np->y;

    }

/*
 * Whack the edges whose mid points are inside the polygon.
 */
    for (i=0; i<NumEdges; i++) {

        ep = EdgeList + i;
        if (ep->deleted == 1  ||
            ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
            ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT) {
            continue;
        }
        n1 = ep->node1;
        n2 = ep->node2;
        xmid = (NodeList[n1].x + NodeList[n2].x) / 2.0;
        ymid = (NodeList[n1].y + NodeList[n2].y) / 2.0;

        if (xmid < xmin  ||  xmid > xmax  ||
            ymid < ymin  ||  ymid > ymax) {
            continue;
        }

        istat =
        ply_utils_obj.ply_point (xpoly, ypoly, n, xmid, ymid);
        if (istat == 1) {
            WhackEdge (i);
        }

    }

    return 1;

}



/*----------------------------------------------------------------------------*/

int CSWGrdTriangle::GetNodeEdgeList (int nodenum, int **elistout)
{
    NOdeStruct        *nptr;
    RAwPointStruct    *rptr;
    int               nlist, *elist;

    nptr = NodeList + nodenum;
    if (nptr->rp < 0  &&  nptr->crp < 0) {
        return -1;
    }

    if (nptr->rp < 0) {
        rptr = ConstraintRawPoints + nptr->crp;
    }
    else {
        rptr = RawPoints + nptr->rp;
    }

    elist = rptr->edgelist;
    nlist = rptr->nedge;

    if (elist == NULL  ||  nlist < 1) {
        return -1;
    }

    *elistout = elist;
    return nlist;

}



/*-----------------------------------------------------------------------------*/

/*
 * Remove a border node when 3 edges are still attached to the node
 * after edge swapping.  This is only called from the RemoveNode
 * function.
 */
int CSWGrdTriangle::RemoveBorderNode (int nodenum,
                             int *elist,
                             int nlist)
{
    int               i, itri, nb,
                      nb1, nb2, eb1, eb2,
                      ntmp, nlist2, *elist2;
    int               e1, e2, e3;
    int               n1, n2, n3;
    EDgeStruct        *ep;

/*
 * e1, e2, and e3 are 3 edges that will eventually
 * form a new triangle around the deleted node.
 * These are initialized to -1 in case of an error or
 * bug in the algorithm.  n1, n2 and n3 are nodes of
 * the new triangle.
 */
    e1 = e2 = e3 = -1;
    n1 = n2 = n3 = -1;

/*
 * Find the 2 border edges that connect to the border node.
 * There should be exactly two or there is a bug in the
 * previous trimesh calculation code.
 *
 * nb1 and nb2 are the opposite nodes of these two edges.
 * eb1 and eb2 are the edge numbers of the border edges
 */
    nb1 = nb2 = eb1 = eb2 = -1;
    for (i=0; i<nlist; i++) {
        ep = EdgeList + elist[i];
        if (ep->on_border == 1  ||
            ep->tri2 < 0) {
            nb = ep->node1;
            if (nb == nodenum) {
                nb = ep->node2;
            }
            if (nb1 < 0) {
                nb1 = nb;
                eb1 = elist[i];
            }
            else if (nb2 < 0) {
                nb2 = nb;
                eb2 = elist[i];
            }
            else {
                assert (0);
            }
        }
    }

    if (nb1 == -1  ||  nb2 == -1) {
        printf ("assert attempting to remove border node %d\n", nodenum);
        assert (0);
    }

    if (eb1 == -1  ||  eb2 == -1) {
        printf ("assert attempting to remove border node %d\n", nodenum);
        assert (0);
    }

/*
 * Create a new edge from nb1 to nb2.  This is the first edge
 * of the final triangle (e1).
 */
    e1 = AddEdge (nb1, nb2, -1, -1, EdgeList[eb1].flag);
    if (e1 < 0) {
        return -1;
    }
    EdgeList[e1].on_border = 1;
    n1 = nb1;
    n2 = nb2;

/*
 * Find the opposite node of the connected edge that is not
 * a border edge.  There should be exactly one of these.
 */
    for (i=0; i<nlist; i++) {
        ep = EdgeList + elist[i];
        if (ep->on_border == 0  &&
            ep->tri2 >= 0) {
            nb = ep->node1;
            if (nb == nodenum) {
                nb = ep->node2;
            }
            if (n3 < 0) {
                n3 = nb;
            }
            else {
                assert (0);
            }
        }
    }

    if (n3 == -1) {
        assert (0);
    }

/*
 * Find the edge connecting to n1 that has n3 as its opposite node.
 * This becomes the second edge of the final triangle (e2).
 */
    nlist2 = GetNodeEdgeList (n1, &elist2);

    for (i=0; i<nlist2; i++) {
        ep = EdgeList + elist2[i];
        ntmp = ep->node1;
        if (ntmp == n1) {
            ntmp = ep->node2;
        }
        if (ntmp == n3) {
            e2 = elist2[i];
            break;
        }
    }

    if (e2 == -1) {
        assert (0);
    }

/*
 * Find the edge connecting to n1 that has n3 as its opposite node.
 * This becomes the second edge of the final triangle (e2).
 */
    nlist2 = GetNodeEdgeList (n2, &elist2);

    for (i=0; i<nlist2; i++) {
        ep = EdgeList + elist2[i];
        ntmp = ep->node1;
        if (ntmp == n2) {
            ntmp = ep->node2;
        }
        if (ntmp == n3) {
            e3 = elist2[i];
            break;
        }
    }

    if (e3 == -1) {
        assert (0);
    }

/*
 * Whack the edges connected to the removed node.
 */
    for (i=0; i<nlist; i++) {
        WhackEdge (elist[i]);
    }

/*
 * Build a new triangle using e1, e2 and e3.
 */
    itri = AddTriangle (e1, e2, e3, 0);
    if (itri < 0) {
        return -1;
    }
    EdgeList[e1].tri1 = itri;
    EdgeList[e2].tri2 = itri;
    EdgeList[e3].tri2 = itri;

    return 1;

}


/*---------------------------------------------------------------------------------*/

/*
 * Group of functions to maintain the private triangle index cache list.
 */

/*---------------------------------------------------------------------------------*/

int CSWGrdTriangle::AddStaticTriIndex (int id1, int id2)
{
    int          istat;

    istat = AddTriIndex (id1, id2,
                         TriangleIndexGrid,
                         TriangleIndexXmin,
                         TriangleIndexYmin,
                         TriangleIndexXmax,
                         TriangleIndexYmax,
                         TriangleIndexNcol,
                         TriangleIndexNrow,
                         TriangleIndexXspace,
                         TriangleIndexYspace);
    return istat;
}



int CSWGrdTriangle::AddTriIndex (int id1, int id2,
                        INdexStruct  **index,
                        double xmin, double ymin, double xmax, double ymax,
                        int ncol, int nrow,
                        double xspace, double yspace)
{
    TRiangleIndexListStruct   *tptr;

    if (TriIndexList == NULL  ||  NumTriIndexList >= MaxTriIndexList) {
        MaxTriIndexList += 10;
        TriIndexList = (TRiangleIndexListStruct *)
            csw_Realloc (TriIndexList, MaxTriIndexList * sizeof(TRiangleIndexListStruct));
    }

    if (TriIndexList == NULL) {
        return -1;
    }

    tptr = TriIndexList + NumTriIndexList;

    tptr->id1 = id1;
    tptr->id2 = id2;
    tptr->index = index;
    tptr->xmin = xmin;
    tptr->ymin = ymin;
    tptr->xmax = xmax;
    tptr->ymax = ymax;
    tptr->ncol = ncol;
    tptr->nrow = nrow;
    tptr->xspace = xspace;
    tptr->yspace = yspace;

    NumTriIndexList++;

    return 1;
}



int CSWGrdTriangle::RemoveTriIndex (int id1, int id2)
{
    TRiangleIndexListStruct     *tptr;
    int                         i, match;

    if (NumTriIndexList < 1) {
        return 0;
    }

    match = -1;
    for (i=0; i<NumTriIndexList; i++) {

        tptr = TriIndexList + i;

        if (tptr->id1 == id1  &&  tptr->id2 == id2) {
            match = i;
            break;
        }

    }

    if (match >= 0) {
        for (i=match+1; i<NumTriIndexList; i++) {
            memcpy (TriIndexList + i - 1,
                    TriIndexList + i,
                    sizeof(TRiangleIndexListStruct));
        }
        NumTriIndexList--;
        return 1;
    }

    return 0;

}



TRiangleIndexListStruct* CSWGrdTriangle::FindTriIndex (int id1, int id2)
{

    TRiangleIndexListStruct     *tptr;
    int                         i;

    if (NumTriIndexList < 1) {
        return NULL;
    }

    for (i=0; i<NumTriIndexList; i++) {

        tptr = TriIndexList + i;

        if (tptr->id1 == id1  &&  tptr->id2 == id2) {
            return tptr;
        }

    }

    return NULL;

}




void CSWGrdTriangle::FreeTriIndexList (void)
{

    int                        i, j;
    TRiangleIndexListStruct    *tptr;
    INdexStruct                *iptr, **iplist;

    for (i=0; i<NumTriIndexList; i++) {
        tptr = TriIndexList + i;
        iplist = tptr->index;
        if (iplist != NULL) {
            for (j=0; j<tptr->ncol * tptr->nrow; j++) {
                iptr = iplist[j];
                if (iptr != NULL) {
                    csw_Free (iptr->list);
                    csw_Free (iptr);
                }
            }
            csw_Free (iplist);
        }
        tptr->index = NULL;
    }

    csw_Free (TriIndexList);
    TriIndexList = NULL;
    NumTriIndexList = 0;
    MaxTriIndexList = 0;

}


int CSWGrdTriangle::grd_remove_tri_index (int id1, int id2)
{
    int               istat;

    istat = RemoveTriIndex (id1, id2);
    istat = RemoveNodeEdge (id1, id2);

    return istat;
}


void CSWGrdTriangle::grd_free_tri_index_list (void)
{
    FreeTriIndexList ();
    FreeNodeEdgeCache ();
}

/*---------------------------------------------------------------------------------*/

/*
 * Group of functions to maintain the private node edge cache list.
 */

/*---------------------------------------------------------------------------------*/

int CSWGrdTriangle::AddStaticNodeEdge (int id1, int id2)
{
    int          istat;

    istat = AddNodeEdge (id1, id2,
                         NodeEdgeList,
                         NumNodeEdgeList);
    return istat;
}



int CSWGrdTriangle::AddNodeEdge (int id1, int id2,
                        NOdeEdgeListStruct   *list,
                        int                  nlist)
{
    NOdeEdgeCacheStruct       *nptr;

    if (NodeEdgeCache == NULL  ||  NumNodeEdgeCache >= MaxNodeEdgeCache) {
        MaxNodeEdgeCache += 10;
        NodeEdgeCache = (NOdeEdgeCacheStruct *)
            csw_Realloc (NodeEdgeCache, MaxNodeEdgeCache * sizeof(NOdeEdgeCacheStruct));
    }

    if (NodeEdgeCache == NULL) {
        return -1;
    }

    nptr = NodeEdgeCache + NumNodeEdgeCache;

    nptr->id1 = id1;
    nptr->id2 = id2;
    nptr->list = list;
    nptr->nlist = nlist;

    NumNodeEdgeCache++;

    return 1;
}



int CSWGrdTriangle::RemoveNodeEdge (int id1, int id2)
{
    NOdeEdgeCacheStruct         *nptr;
    int                         i, match;

    if (NumNodeEdgeCache < 1) {
        return 0;
    }

    match = -1;
    for (i=0; i<NumNodeEdgeCache; i++) {

        nptr = NodeEdgeCache + i;

        if (nptr->id1 == id1  &&  nptr->id2 == id2) {
            match = i;
            break;
        }

    }

    if (match >= 0) {
        for (i=match+1; i<NumNodeEdgeCache; i++) {
            memcpy (NodeEdgeCache + i - 1,
                    NodeEdgeCache + i,
                    sizeof(NOdeEdgeCacheStruct));
        }
        NumNodeEdgeCache--;
        return 1;
    }

    return 0;

}



NOdeEdgeCacheStruct *CSWGrdTriangle::FindNodeEdge (int id1, int id2)
{

    NOdeEdgeCacheStruct         *nptr;
    int                         i;

    if (NumNodeEdgeCache < 1) {
        return NULL;
    }

    for (i=0; i<NumNodeEdgeCache; i++) {

        nptr = NodeEdgeCache + i;

        if (nptr->id1 == id1  &&  nptr->id2 == id2) {
            return nptr;
        }

    }

    return NULL;

}




int CSWGrdTriangle::IsNodeEdgeInCache (NOdeEdgeListStruct *list)
{

    NOdeEdgeCacheStruct         *nptr;
    int                         i;

    if (NumNodeEdgeCache < 1) {
        return 0;
    }

    for (i=0; i<NumNodeEdgeCache; i++) {

        nptr = NodeEdgeCache + i;

        if (nptr->list == list) {
            return 1;
        }

    }

    return 0;

}




void CSWGrdTriangle::FreeNodeEdgeCache (void)
{

    int                        i, j, ndo;
    NOdeEdgeCacheStruct        *nptr;
    NOdeEdgeListStruct         *neptr;

    for (i=0; i<NumNodeEdgeCache; i++) {
        nptr = NodeEdgeCache + i;
        ndo = nptr->nlist;
        neptr = nptr->list;
        for (j=0; j<ndo; j++) {
            csw_Free (neptr[j].list);
        }
        csw_Free (neptr);
    }

    csw_Free (NodeEdgeCache);
    NodeEdgeCache = NULL;
    NumNodeEdgeCache = 0;
    MaxNodeEdgeCache = 0;

}



/*------------------------------------------------------------------------*/

/*
 * Move existing nodes that are close to a constraint node
 * to the actual location of the constraint node.
 */
int CSWGrdTriangle::AdjustForOutsideConstraintPoints (void)
{
    int                i, ncp;
    NOdeStruct         *nptr;
    RAwPointStruct     *rptr;

    OutsidePointAdjust = 1;

/*
 * Move existing nodes to nearby constraint nodes if the move
 * is less than half the average edge length and if the move
 * does not introduce any crossing edges.
 */
    rptr = RawPoints + MaxIndexPoint;
    for (i=0; i<NumLinePoints; i++) {
        if (rptr->nodenum >= 0) {
            rptr++;
            continue;
        }
        ncp =
        SetupExistingTriangleNode (rptr->x, rptr->y, rptr->z);
        if (ncp == -1) {
            rptr++;
            continue;
        }
        if (ncp < -1) {
            ncp = -ncp;
            ncp -= 2;
            nptr = NodeList + ncp;
            rptr->x = nptr->x;
            rptr->y = nptr->y;
            rptr->z = nptr->z;
            rptr->nodenum = ncp;
            nptr->rp = rptr - RawPoints;
            nptr->crp = -1;
            rptr++;
        }
        else {
            nptr = NodeList + ncp;
            rptr->nodenum = ncp;
            nptr->rp = rptr - RawPoints;
            nptr->crp = -1;
            nptr->x = rptr->x;
            nptr->y = rptr->y;
            nptr->z = rptr->z;
            rptr++;
        }
    }

    OutsidePointAdjust = 0;

    return 1;

}

/*
 ******************************************************************

      G e t S w a p C o o r d s F o r N o d e R e m o v a l

 ******************************************************************

    Return the coordinates of the swapped edge without actually
    doing the swapping.  If the edge cannot be swapped, zero is
    returned and the coordinates are set to 1.e30.
*/

int CSWGrdTriangle::GetSwapCoordsForNodeRemoval
                         (int edgenum,
                          double *x1out, double *y1out,
                          double *x2out, double *y2out,
                          int *node1out, int *node2out)

{
    TRiangleStruct         *tp1, *tp2;
    EDgeStruct             *ep, *ep3, *ep4, *eptmp;
    int                    e1, e2, e3, e4,
                           n1, n2, n3, n4, istat;
    double                 xint, yint;
    double                 x1, y1, x2, y2, x3, y3, x4, y4;
    double                 xa[5], ya[5], xmid, ymid;
    int                    force;

    CSWPolyUtils             ply_utils_obj;

    *x1out = 1.e30;
    *y1out = 1.e30;
    *x2out = 1.e30;
    *y2out = 1.e30;
    *node1out = -1;
    *node2out = -1;

/*
    Do nothing if the edge has only one triangle using it
    or if the edge swap flags are not enabled.
*/
    if (EdgeSwapFlag == GRD_SWAP_NONE) {
        return 0;
    }

    ep = EdgeList + edgenum;
    if (ep->deleted == 1) {
        return 0;
    }
    if (ep->tri2 == -1) {
        return 0;
    }

    if (EdgeSwapFlag == GRD_SWAP_AS_FLAGGED  &&
        ep->flag == GRD_DONT_SWAP_FLAG) {
        return 0;
    }

/*
    Find the two edges of the first triangle that are not the specified edge.
*/
    tp1 = TriangleList + ep->tri1;
    tp2 = TriangleList + ep->tri2;

    e1 = e2 = e3 = e4 = -1;

    if (tp1->edge1 != edgenum) {
        e1 = tp1->edge1;
    }
    if (tp1->edge2 != edgenum) {
        if (e1 == -1) {
            e1 = tp1->edge2;
        }
        else {
            e2 = tp1->edge2;
        }
    }
    if (tp1->edge3 != edgenum) {
        e2 = tp1->edge3;
    }

/*
    Find the nodes of the first triangle, in the same order as the edges.
*/
    n1 = CommonNode (edgenum, e1);
    n2 = CommonNode (e1, e2);
    n3 = CommonNode (e2, edgenum);

/*
    Find the edges of the second triangle that are not the diagonal and
    find the remaining node of the quadrilateral.
*/
    if (tp2->edge1 != edgenum) {
        e3 = tp2->edge1;
    }
    if (tp2->edge2 != edgenum) {
        if (e3 == -1) {
            e3 = tp2->edge2;
        }
        else {
            e4 = tp2->edge2;
        }
    }
    if (tp2->edge3 != edgenum) {
        e4 = tp2->edge3;
    }

    ep3 = EdgeList + e3;
    ep4 = EdgeList + e4;

    n4 = CommonNode (e3, e4);

/*
    The edge e3 must be connected to edge e2.  If not, swap
    e3 and e4 edges.  When this is done, the edges e1, e2, e3, to e4
    represent a continuous perimeter of the quadrilateral.  The nodes
    n1, n2, n3, and n4 are the nodes of the perimeter, in order.
*/
    if (CommonNode (e2, e3) == -1) {
        eptmp = ep3;
        ep3 = ep4;
        ep4 = eptmp;
        e3 = ep3 - EdgeList;
        e4 = ep4 - EdgeList;
    }

/*
    If the two possible diagonal segments do not intersect, there
    are two possible scenarios.  If the candidate edge for swapping is
    outside of the quadralateral defined by its adjoining triangles,
    the edge should always be swapped.  If the candidate edge is inside,
    the edge should never be swapped.
*/

  /*
   * Get the endpoints of the two possible diagonals.
   */
    x1 = NodeList[n1].x;
    y1 = NodeList[n1].y;
    x2 = NodeList[n3].x;
    y2 = NodeList[n3].y;
    x3 = NodeList[n2].x;
    y3 = NodeList[n2].y;
    x4 = NodeList[n4].x;
    y4 = NodeList[n4].y;

  /*
   * Make the outline of the quadralateral and check if the
   * mid point of the candidate edge is inside the outline.
   * If it is not inside, and the midpoint of the alternate
   * diagonal is inside, force swapping.  If both midpoints
   * are outside, return without swapping.
   */
    xa[0] = x1;
    ya[0] = y1;
    xa[1] = x3;
    ya[1] = y3;
    xa[2] = x2;
    ya[2] = y2;
    xa[3] = x4;
    ya[3] = y4;
    xa[4] = x1;
    ya[4] = y1;

    xmid = (x1 + x2) / 2.0;
    ymid = (y1 + y2) / 2.0;

    force = 0;
    istat = ply_utils_obj.ply_point (xa, ya, 5, xmid, ymid);
    if (istat == -1) {

    /*
     * Check the alternate diagonal.  Return if its midpoint
     * is also outside the quadralateral.
     */
        xmid = (x3 + x4) / 2.0;
        ymid = (y3 + y4) / 2.0;
        istat = ply_utils_obj.ply_point (xa, ya, 5, xmid, ymid);
        if (istat == -1) {
            if (FinalSwapFlag == 1) {
                if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
                if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
                if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
                if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
                SwapFlags[edgenum] = 2;
                NumSwapped++;
            }
            return 0;
        }

        force = 1;

        goto FORCE_SWAPPING;
    }

  /*
   * The midpoint is inside the quadralateral outline, so check the
   * intersection of the two diagonals.  If they do not intersect,
   * do not swap.
   */
    if (force == 0) {
        if (ExtendDiagonalsFlag) {
            ExtendVectors (&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
        }
        istat = ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4,
                            &xint, &yint);
        if (istat != 0) {
            if (FinalSwapFlag == 1) {
                if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
                if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
                if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
                if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
                SwapFlags[edgenum] = 2;
                NumSwapped++;
            }
            return 0;
        }
    }

  FORCE_SWAPPING:

/*
    The old diagonal was by definition from n1 to n3, so the
    new diagonal must be from n2 to n4.  Return the coordinates
    for n2 and n4.
*/
    *x1out = NodeList[n2].x;
    *y1out = NodeList[n2].y;
    *x2out = NodeList[n4].x;
    *y2out = NodeList[n4].y;
    *node1out = n2;
    *node2out = n4;

    return 1;

}  /*  end of private GetSwapCoordsForNodeRemoval function  */



/*
 ********************************************************************************

             N o d e O n S e g m e n t F o r N o d e R e m o v a l

 ********************************************************************************

  Return 1 if the nchk node is almost exactly on top of the line segment
  defined by the n1 and n2 nodes.  Return zero otherwise.

*/

int CSWGrdTriangle::NodeOnSegmentForNodeRemoval (int nchk, int n1, int n2, double *dperpout)

{
    double             x1, y1, x2, y2, xp, yp, xint, yint,
                       dx, dy, tiny, dist;
    int                istat;
    NOdeStruct         *nptr;

    tiny = GrazeDistance * 10;

    nptr = NodeList + nchk;
    xp = nptr->x;
    yp = nptr->y;
    nptr = NodeList + n1;
    x1 = nptr->x;
    y1 = nptr->y;
    nptr = NodeList + n2;
    x2 = nptr->x;
    y2 = nptr->y;

    istat = gpf_perpintpoint2 (x1, y1, x2, y2, xp, yp, &xint, &yint);
    if (istat == 0) {
        return 0;
    }

  /*
   * The perpintpoint can return 1 if the point grazes the segment
   * endpoint.  Check for exact inside segment here.
    if ( !((x1-xint)*(xint-x2) >= 0.0f  ||  (y1-yint)*(yint-y2) >= 0.0f)) {
        return 0;
    }
   */

    dx = xp - xint;
    dy = yp - yint;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (dperpout != NULL) {
        *dperpout = dist;
    }

    if (dist <= tiny) {
        SegmentIntX = xint;
        SegmentIntY = yint;
        return 1;
    }

    return 0;

}  /* end of private NodeOnSegmentForNodeRemoval function */




/*-------------------------------------------------------------------------*/

void CSWGrdTriangle::RemoveZeroLengthEdge (int edgenum)
{
    int               t1, t2, e1, e2,
                      t1prime, t2prime,
                      e1opp, e2opp,
                      n1, n2, et;
    int               *list, nlist, i;
    TRiangleStruct    *tptr;
    EDgeStruct        *eptr, *e1ptr, *e2ptr;

    int               *nadd, *nrem,
                      *eadd, *erem;
    int               numadd, numrem;

    nadd = p_int_mar1;
    nrem = p_int_mar2;
    eadd = p_int_mar3;
    erem = p_int_mar4;

    if (RemoveZeroFlag == 0) return;

    numadd = 0;
    numrem = 0;

/*
 * Get the nodes and triangles used by the edge.
 */
    eptr = EdgeList + edgenum;
    if (eptr->deleted == 1) {
        return;
    }

    n1 = eptr->node1;
    n2 = eptr->node2;
    t1 = eptr->tri1;
    t2 = eptr->tri2;
    if (t1 < 0  ||  n1 < 0  ||  n2 < 0) {
        return;
    }

    nlist = GetNodeEdgeList (n1, &list);

/*
 * Find an edge connected to n1, which is not the edge to be
 * deleted, and which is a part of the t1 triangle.  This is
 * the e1 edge.
 */
    e1 = -1;
    tptr = TriangleList + t1;
    for (i=0; i<nlist; i++) {
        et = list[i];
        if (et == edgenum) continue;
        if (et == tptr->edge1  ||
            et == tptr->edge2  ||
            et == tptr->edge3) {
            e1 = et;
            break;
        }
    }

    if (e1 == -1) {
        return;
    }

/*
 * Find the triangle shared by the e1 edge that is not the t1 triangle.
 * This is the t1prime triangle.
 */
    e1ptr = EdgeList + e1;
    t1prime = e1ptr->tri1;
    if (t1prime == t1) {
        t1prime = e1ptr->tri2;
    }

/*
 * Find the edge of the t1 triangle that is opposite of node n1.
 * This is the e1opp edge.
 */
    tptr = TriangleList + t1;
    e1opp = OppositeEdge (tptr, n1);
    if (e1opp == -1) {
        return;
    }

/*
 * If the t2 triangle is -1, then the edge to be removed is on the
 * border of the trimesh.  In this case, no operations based on the t2
 * triangle can done.
 */
    if (t2 < 0) {
        e2 = -1;
        t2prime = -1;
        e2opp = -1;
    }

/*
 * The t2 triangle is valid, so find e2, t2prime and e2opp
 * in the same fashion as was done for the t1 triangle.
 */
    else {

    /*
     * Find an edge connected to n1, which is not the edge to be
     * deleted, and which is a part of the t2 triangle.  This is
     * the e2 edge.
     */
        e2 = -1;
        tptr = TriangleList + t2;
        for (i=0; i<nlist; i++) {
            et = list[i];
            if (et == edgenum) continue;
            if (et == tptr->edge1  ||
                et == tptr->edge2  ||
                et == tptr->edge3) {
                e2 = et;
                break;
            }
        }

        if (e2 == -1) {
            return;
        }

    /*
     * Find the triangle shared by the e2 edge that is not the t2 triangle.
     * This is the t2prime triangle.
     */
        e2ptr = EdgeList + e2;
        t2prime = e2ptr->tri1;
        if (t2prime == t2) {
            t2prime = e2ptr->tri2;
        }

    /*
     * Find the edge of the t2 triangle that is opposite of node n1.
     * This is the e1opp edge.
     */
        tptr = TriangleList + t2;
        e2opp = OppositeEdge (tptr, n1);
        if (e2opp == -1) {
            return;
        }

    }

/*
 * All of the edges connected to n1 must have references to n1 changed to n2.
 */
    for (i=0; i<nlist; i++) {
        if (list[i] == edgenum  ||  list[i] == e1  ||  list[i] == e2) {
            continue;
        }
        eptr = EdgeList + list[i];
        if (eptr->node1 == n1) {
            eptr->node1 = n2;
        }
        else {
            eptr->node2 = n2;
        }
        eptr->length = NodeDistanceXYZ (eptr->node1, eptr->node2);
        nadd[numadd] = n2;
        eadd[numadd] = list[i];
        numadd++;
        nrem[numrem] = n1;
        erem[numrem] = list[i];
        numrem++;
    }

/*
 * Adjust for the removal of t1 and e1.
 */
    if (e1 == 11934 || e1opp == 11934) {
        e1 = e1;
    }

    eptr = EdgeList + e1opp;
    if (eptr->tri1 == t1) {
        eptr->tri1 = t1prime;
    }
    else {
        eptr->tri2 = t1prime;
    }

    if (eptr->tri1 < 0) {
        eptr->tri1 = eptr->tri2;
        eptr->tri2 = -1;
    }

    if (t1prime >= 0) {
        tptr = TriangleList + t1prime;
        if (tptr->edge1 == e1) {
            tptr->edge1 = e1opp;
        }
        else if (tptr->edge2 == e1) {
            tptr->edge2 = e1opp;
        }
        else {
            tptr->edge3 = e1opp;
        }
    }

    eptr = EdgeList + e1;
    nrem[numrem] = eptr->node1;
    erem[numrem] = e1;
    numrem++;
    nrem[numrem] = eptr->node2;
    erem[numrem] = e1;
    numrem++;
    eptr->deleted = 1;
    tptr = TriangleList + t1;
    tptr->deleted = 1;

/*
 * Adjust for removal of t2 and e2.
 */
    if (t2 >= 0) {

        if (e2 == 11934 || e2opp == 11934) {
            e2 = e2;
        }

        eptr = EdgeList + e2opp;
        if (eptr->tri1 == t2) {
            eptr->tri1 = t2prime;
        }
        else {
            eptr->tri2 = t2prime;
        }

        if (eptr->tri1 < 0) {
            eptr->tri1 = eptr->tri2;
            eptr->tri2 = -1;
        }

        if (t2prime >= 0) {
            tptr = TriangleList + t2prime;
            if (tptr->edge1 == e2) {
                tptr->edge1 = e2opp;
            }
            else if (tptr->edge2 == e2) {
                tptr->edge2 = e2opp;
            }
            else {
                tptr->edge3 = e2opp;
            }
        }

        eptr = EdgeList + e2;
        nrem[numrem] = eptr->node1;
        erem[numrem] = e2;
        numrem++;
        nrem[numrem] = eptr->node2;
        erem[numrem] = e2;
        numrem++;
        eptr->deleted = 1;
        tptr = TriangleList + t2;
        tptr->deleted = 1;
    }

    if (numrem > _MAX_ADD_REM_) {
        assert (0);
    }
    if (numadd > _MAX_ADD_REM_) {
        assert (0);
    }

    for (i=0; i<numrem; i++) {
        RemoveEdgeFromNodeList (nrem[i], erem[i]);
    }
    for (i=0; i<numadd; i++) {
        AddEdgeToNodeList (nadd[i], eadd[i]);
    }

    numrem = numadd = 0;

/*
 * Mark the edgenum and n1 as deleted.
 */
    eptr = EdgeList + edgenum;
    RemoveEdgeFromNodeList (eptr->node1, edgenum);
    RemoveEdgeFromNodeList (eptr->node2, edgenum);
    EdgeList[edgenum].deleted = 1;
    NodeList[n1].deleted = 1;

    return ;

}



/*-------------------------------------------------------------------------*/

int CSWGrdTriangle::grd_set_remove_zero_flag (int ival)
{
    if (ival != 0) ival = 1;
    RemoveZeroFlag = ival;
    return 1;
}


/*
  ****************************************************************************

                           S w a p E d g e 2

  ****************************************************************************

    If the edge specified has two triangles that share it, then the
    two triangles form a quadrilateral with the specified edge being
    one of the two possible diagonals of the quadrilateral.  This
    function changes the edge to the other possible diagonal and
    updates the two triangles to reflect the new topology.  The swap
    is only done if the resulting least equilateral triangle is better
    after the swap.

    This differs from SwapEdge only in the fact that a point is considered
    inside a polygon here if it grazes the border of the polygon.  This
    is used for swapping associated with removing nodes.

*/

int CSWGrdTriangle::SwapEdge2 (int edgenum)

{
    TRiangleStruct         *tp1, *tp2;
    EDgeStruct             *ep, *ep3, *ep4, *eptmp;
    int                    e1, e2, e3, e4,
                           n1, n2, n3, n4,
                           tnum1, tnum2, istat, swapit;
    double                 xint, yint;
    double                 eq1, eq2, eq3, eq4, eq11, eq22, badeq;
    double                 worsteq1, worsteq2;
    int                    hull_flag;
    double                 x1, y1, x2, y2, x3, y3, x4, y4;
    double                 xa[5], ya[5], xmid, ymid;
    int                    force;

    CSWPolyUtils             ply_utils_obj;

    hull_flag = 0;
    if (ConvexHullFlag == 1  &&  FinalSwapFlag == 1) {
        hull_flag = 1;
    }

/*
    Do nothing if the edge has only one triangle using it
    or if the edge swap flags are not enabled.
*/
    if (EdgeSwapFlag == GRD_SWAP_NONE) {
        return 0;
    }

    ep = EdgeList + edgenum;
    if (ep->deleted == 1) {
        return 0;
    }
    if (ep->tri2 == -1) {
        return 0;
    }

    if (EdgeSwapFlag == GRD_SWAP_AS_FLAGGED  &&
        ep->flag == GRD_DONT_SWAP_FLAG) {
        return 0;
    }

/*
    Find the two edges of the first triangle that are not the specified edge.
*/
    tp1 = TriangleList + ep->tri1;
    tp2 = TriangleList + ep->tri2;

    e1 = e2 = e3 = e4 = -1;

    if (tp1->edge1 != edgenum) {
        e1 = tp1->edge1;
    }
    if (tp1->edge2 != edgenum) {
        if (e1 == -1) {
            e1 = tp1->edge2;
        }
        else {
            e2 = tp1->edge2;
        }
    }
    if (tp1->edge3 != edgenum) {
        e2 = tp1->edge3;
    }

/*
    Find the nodes of the first triangle, in the same order as the edges.
*/
    n1 = CommonNode (edgenum, e1);
    n2 = CommonNode (e1, e2);
    n3 = CommonNode (e2, edgenum);

/*
    Find the edges of the second triangle that are not the diagonal and
    find the remaining node of the quadrilateral.
*/
    if (tp2->edge1 != edgenum) {
        e3 = tp2->edge1;
    }
    if (tp2->edge2 != edgenum) {
        if (e3 == -1) {
            e3 = tp2->edge2;
        }
        else {
            e4 = tp2->edge2;
        }
    }
    if (tp2->edge3 != edgenum) {
        e4 = tp2->edge3;
    }

    ep3 = EdgeList + e3;
    ep4 = EdgeList + e4;

    n4 = CommonNode (e3, e4);

/*
    The edge e3 must be connected to edge e2.  If not, swap
    e3 and e4 edges.  When this is done, the edges e1, e2, e3, to e4
    represent a continuous perimeter of the quadrilateral.  The nodes
    n1, n2, n3, and n4 are the nodes of the perimeter, in order.
*/
    if (CommonNode (e2, e3) == -1) {
        eptmp = ep3;
        ep3 = ep4;
        ep4 = eptmp;
        e3 = ep3 - EdgeList;
        e4 = ep4 - EdgeList;
    }

/*
    If the two possible diagonal segments do not intersect, there
    are two possible scenarios.  If the candidate edge for swapping is
    outside of the quadralateral defined by its adjoining triangles,
    the edge should always be swapped.  If the candidate edge is inside,
    the edge should never be swapped.
*/

  /*
   * Get the endpoints of the two possible diagonals.
   */
    x1 = NodeList[n1].x;
    y1 = NodeList[n1].y;
    x2 = NodeList[n3].x;
    y2 = NodeList[n3].y;
    x3 = NodeList[n2].x;
    y3 = NodeList[n2].y;
    x4 = NodeList[n4].x;
    y4 = NodeList[n4].y;

  /*
   * Make the outline of the quadralateral and check if the
   * mid point of the candidate edge is inside the outline.
   * If it is not inside, and the midpoint of the alternate
   * diagonal is inside, force swapping.  If both midpoints
   * are outside, return without swapping.
   */
    xa[0] = x1;
    ya[0] = y1;
    xa[1] = x3;
    ya[1] = y3;
    xa[2] = x2;
    ya[2] = y2;
    xa[3] = x4;
    ya[3] = y4;
    xa[4] = x1;
    ya[4] = y1;

    xmid = (x1 + x2) / 2.0;
    ymid = (y1 + y2) / 2.0;

    force = 0;
    istat = ply_utils_obj.ply_point (xa, ya, 5, xmid, ymid);
    if (istat == -1) {

    /*
     * Check the alternate diagonal.  Return if its midpoint
     * is also outside the quadralateral.
     */
        xmid = (x3 + x4) / 2.0;
        ymid = (y3 + y4) / 2.0;
        istat = ply_utils_obj.ply_point (xa, ya, 5, xmid, ymid);
        if (istat == -1) {
            if (FinalSwapFlag == 1) {
                if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
                if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
                if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
                if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
                SwapFlags[edgenum] = 2;
                NumSwapped++;
            }
            return 0;
        }

        force = 1;

        goto FORCE_SWAPPING;
    }

  /*
   * The midpoint is inside the quadralateral outline, so check the
   * intersection of the two diagonals.  If they do not intersect,
   * do not swap.
   */
    if (force == 0) {
        if (ExtendDiagonalsFlag) {
            ExtendVectors (&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4);
        }
        istat = ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4,
                            &xint, &yint);
        if (istat != 0) {
            if (FinalSwapFlag == 1) {
                if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
                if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
                if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
                if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
                SwapFlags[edgenum] = 2;
                NumSwapped++;
            }
            return 0;
        }

    }

/*
    Calculate the "equilateralness" of the two triangles defined
    by the current edge orientation and by the swapped edge
    orientation.  If the new triangles are "more equilateral"
    than the previous triangles, swap the edge.

    If either edge uses a contrived corner point, then the
    equilateralness of the triangles using that edge are
    lowered to make it more difficult for the corner point
    to be used in a final triangle.
*/
    eq1 = CalcEquilateralness (n2, n1, n3);
    eq2 = CalcEquilateralness (n4, n1, n3);
    eq3 = CalcEquilateralness (n1, n2, n4);
    eq4 = CalcEquilateralness (n3, n2, n4);

    badeq = 1.e-20;
    if (eq1 < badeq) eq1 = 0.0;
    if (eq2 < badeq) eq2 = 0.0;
    if (eq3 < badeq) eq3 = 0.0;
    if (eq4 < badeq) eq4 = 0.0;

    if (n1 < NumCornerNodes  ||  n3 < NumCornerNodes) {
        eq1 /= CornerBias;
        eq2 /= CornerBias;
        if (hull_flag == 1) {
            eq1 = 0.0;
            eq2 = 0.0;
        }
    }
    if (n2 < NumCornerNodes  ||  n4 < NumCornerNodes) {
        eq3 /= CornerBias;
        eq4 /= CornerBias;
        if (hull_flag == 1) {
            eq3 = 0.0;
            eq4 = 0.0;
        }
    }

/*
    This section uses the equlateralness of the two
    possible sets of output triangles to decide if
    the edge should be swapped.  If either of the
    current triangles have equlateralness much worse
    than the alternative average equilateralness, then
    the swap is done.  If there is no wildly disparate
    equilateralness  between any triangles, then the
    pair with the largest average equilateralness is
    used.  For the case where both alternatives have
    an undesirable triangle, it doesn't really matter
    which is chosen, so I arbitarily do not swap in
    this case.
*/
    swapit = 0;

    eq11 = eq1 + eq2;
    eq22 = eq3 + eq4;
    worsteq1 = eq1;
    if (eq2 < eq1) worsteq1 = eq2;
    worsteq2 = eq3;
    if (eq4 < eq3) worsteq2 = eq4;

    if (eq3 < eq11 / 20.0  ||
        eq4 < eq11 / 20.0) {
        swapit = 0;
    }
    else if (eq1 < eq22 / 20.0  ||
             eq2 < eq22 / 20.0) {
        swapit = 1;
    }
    else if (worsteq2 > 1.1 * worsteq1) {
        swapit = 1;
    }
    else if (eq22 > 1.01 * eq11) {
        swapit = 1;
    }

/*
    If either current equilateralness (eq1 or eq2) is zero,
    the current alternative has a zero area triangle.  This
    must be swapped out.
*/
    if (swapit == 0) {
        if (eq1 == 0.0  ||  eq2 == 0.0) {
            swapit = 1;
        }
    }

    if (EdgeSwapFlag == GRD_SWAP_AS_FLAGGED) {
        swapit = 1;
    }
    if (EdgeSwapFlag == SWAP_FOR_NULL_REMOVAL) {
        swapit = 1;
        if (NodeList[n2].z > 1.e20  ||
            NodeList[n4].z > 1.e20) {
            swapit = 0;
        }
    }
    if (EdgeSwapFlag == FORCE_SWAP) {
        swapit = 1;
    }

    if (force == 1) {
        swapit = 1;
    }

    if (swapit == 0) {
        if (FinalSwapFlag == 1) {
            if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
            if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
            if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
            if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
            SwapFlags[edgenum] = 2;
            NumSwapped++;
        }
        return 0;
    }

  FORCE_SWAPPING:

/*
    The old diagonal was by definition from n1 to n3, so the
    new diagonal must be from n2 to n4.
*/
    ep->node1 = n2;
    ep->node2 = n4;
    ep->length = NodeDistance (n2, n4);

/*
    Remove the edge from the edge lists of n1 and n3, also
    add the edge to the n2 and n4 node lists.
*/
    if (RawPoints != NULL) {
        RemoveEdgeFromNodeList (n1, edgenum);
        RemoveEdgeFromNodeList (n3, edgenum);
        AddEdgeToNodeList (n2, edgenum);
        AddEdgeToNodeList (n4, edgenum);
    }

/*
    Update the triangles with their new edges.
*/
    tp1->edge1 = e1;
    tp1->edge2 = edgenum;
    tp1->edge3 = e4;

    tp2->edge1 = e2;
    tp2->edge2 = e3;
    tp2->edge3 = edgenum;

/*
    Update the triangles for the edges.
*/
    tnum1 = tp1 - TriangleList;
    tnum2 = tp2 - TriangleList;
    eptmp = EdgeList + e4;
    if (eptmp->tri1 == tnum2) {
        eptmp->tri1 = tnum1;
    }
    else {
        eptmp->tri2 = tnum1;
    }
    eptmp = EdgeList + e2;
    if (eptmp->tri1 == tnum1) {
        eptmp->tri1 = tnum2;
    }
    else {
        eptmp->tri2 = tnum2;
    }

    if (FinalSwapFlag == 1) {
        if (SwapFlags[e1] < 1) SwapFlags[e1] = 1;
        if (SwapFlags[e2] < 1) SwapFlags[e2] = 1;
        if (SwapFlags[e3] < 1) SwapFlags[e3] = 1;
        if (SwapFlags[e4] < 1) SwapFlags[e4] = 1;
        SwapFlags[edgenum] = 2;
        NumSwapped++;
    }

    return 1;

}  /*  end of private SwapEdge2 function  */



/*-----------------------------------------------------------------------------*/

/*
 * Remove any raw line segments that do not have nodes assigned
 * to both end points.  An attempt is made to connect the good
 * nodes of two segments sharing such a nodeless raw point.
 */
int CSWGrdTriangle::CompressRawLines (void)
{
    RAwLineSegStruct    *rline = NULL, *rline2 = NULL,
                        *rlnew = NULL, *rltmp = NULL;
    RAwPointStruct      *rp1 = NULL, *rp2 = NULL;
    int                 jrp, ndo, maxdo, i, j, n, rsize;
    int                 ndone;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
           if (rlnew != RawLines) csw_Free (rlnew);
           csw_Free (RawLines);
           RawLines = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    rsize = sizeof(RAwLineSegStruct);
    maxdo = NumRawLines;
    ndo = 0;

    for (;;) {

        rlnew = (RAwLineSegStruct *)csw_Malloc (NumRawLines * rsize);
        if (rlnew == NULL) {
            return -1;
        }

        n = 0;
        ndone = 0;

        for (i=0; i<NumRawLines; i++) {
            rline = RawLines + i;
            rp1 = RawPoints + rline->rp1;
            rp2 = RawPoints + rline->rp2;
            if (rp1->nodenum >= 0  &&
                rp2->nodenum >= 0) {
                memcpy (rlnew+n, rline, rsize);
                n++;
                continue;
            }

            jrp = rline->rp1;
            if (rp1->nodenum >= 0) {
                jrp = rline->rp2;
            }

            for (j=i+1; j<NumRawLines; j++) {

                rline2 = RawLines + j;

                if (rline2->rp1 == jrp  ||
                    rline2->rp2 == jrp) {
                    rltmp = rlnew + n;
                    if (rline->rp1 == jrp) {
                        rltmp->rp1 = rline->rp2;
                    }
                    else {
                        rltmp->rp1 = rline->rp1;
                    }
                    if (rline2->rp1 == jrp) {
                        rltmp->rp2 = rline2->rp2;
                    }
                    else {
                        rltmp->rp2 = rline2->rp1;
                    }
                    n++;
                    ndone++;
                    break;
                }
            }
        }

        csw_Free (RawLines);
        RawLines = rlnew;
        NumRawLines = n;

        if (ndone < 1) {
            break;
        }

        ndo++;
        if (ndo > maxdo) {
            break;
        }

    }

    for (i=0; i<NumRawLines; i++) {
        rline = RawLines + i;
        rp1 = RawPoints + rline->rp1;
        rp2 = RawPoints + rline->rp2;
        if (rp1->nodenum < 0  &&
            rp2->nodenum < 0) {
            printf ("bad raw point still\n");
        }
    }

    bsuccess = true;

    return 1;

}


/*------------------------------------------------------------------------------*/

int CSWGrdTriangle::grd_chew_up_triangles (
    int              startnode,
    NOdeStruct       *nodes,
    int              *numnodes,
    EDgeStruct       *edges,
    int              *numedges,
    TRiangleStruct   *tris,
    int              *numtris)
{
    int              i, ndo, start_edge, start_tri;
    EDgeStruct       *eptr;
    int              ncon;
    int              *work1, *work2, nwork;

/*
 * Check for obvious errors.
 */
    if (startnode < 0) {
        return -1;
    }

    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        return -1;
    }

    if (*numnodes < 3  ||  *numedges < 3  ||  *numtris < 1) {
        return -1;
    }

    if (startnode >= *numnodes) {
        return -1;
    }

/*
 * Make sure there is at least one constraint edge.
 */
    ncon = 0;
    for (i=0; i<*numedges; i++) {
        eptr = edges + i;
        if (eptr->isconstraint) {
            ncon++;
        }
    }
    if (ncon < 1) {
        return 0;
    }

/*
 * Assign private pointers to nodes, edges and triangles.
 */
    NodeList = nodes;
    EdgeList = edges;
    TriangleList = tris;
    NumNodes = *numnodes;
    NumEdges = *numedges;
    NumTriangles = *numtris;

/*
 * Build a node edge list.  When edges are whacked they need to
 * also be removed from their node edge lists.  Any nodes that
 * have no edges at the end also need to be whacked.
 */
    BuildNodeEdgeLists ();

/*
 * Create a new array of flags to mark which edges need to be deleted.
 */
    if (ChewFlags != NULL) {
        csw_Free (ChewFlags);
        ChewFlags = NULL;
    }

    ChewFlags = (char *)csw_Calloc (*numedges * sizeof(char));
    if (ChewFlags == NULL) {
        ListNull ();
        return -1;
    }

/*
 * Create 2 work arrays to keep track of which
 * triangles have been traversed in the triangle chewing.
 */
    work1 = (int *)csw_Calloc (2 * NumTriangles * sizeof(int));
    if (work1 == NULL) {
        csw_Free (ChewFlags);
        ListNull ();
        return -1;
    }
    work2 = work1 + NumTriangles;

/*
 * Find a border edge that uses the start node.
 */
    start_edge = -1;
    for (i=0; i<*numedges; i++) {
        eptr = edges + i;
        if (eptr->deleted) continue;
        if (eptr->tri2 >= 0) continue;
        if (eptr->node1 == startnode  ||
            eptr->node2 == startnode) {
            start_edge = i;
            break;
        }
    }

/*
 * If no border edge uses the node, find an interior edge
 * that uses the start node.
 */
    if (start_edge == -1) {
      for (i=0; i<*numedges; i++) {
        eptr = edges + i;
        if (eptr->deleted) continue;
        if (eptr->node1 == startnode  ||
            eptr->node2 == startnode) {
            start_edge = i;
            break;
        }
      }
    }

    if (start_edge == -1) {
        ListNull ();
        csw_Free (work1);
        return -1;
    }

    start_tri = edges[start_edge].tri1;
    if (start_tri == -1) {
        ListNull ();
        csw_Free (work1);
        return -1;
    }

    if (edges[start_edge].isconstraint == 0) {
        ChewFlags[start_edge] = 1;
    }

    work1[0] = start_tri;
    nwork = 1;

    for (i=0; i<nwork; i++) {
        RecurseChew (i, &nwork, work1, work2);
    }

/*
 * Whack all the marked edges.
 */
    for (i=0; i<NumEdges; i++) {
        if (ChewFlags[i] == 1) {
            RemoveFromNodeEdgeList (i, EdgeList[i].node1);
            RemoveFromNodeEdgeList (i, EdgeList[i].node2);
            WhackEdge (i);
        }
    }

/*
 * Any edges with a -1 tri1 need to be deleted also.
 */
    for (i=0; i<NumEdges; i++) {
        if (edges[i].tri1 == -1) {
            RemoveFromNodeEdgeList (i, EdgeList[i].node1);
            RemoveFromNodeEdgeList (i, EdgeList[i].node2);
            WhackEdge (i);
        }
    }

/*
 * Any nodes with zero edges attached need to be deleted.
 */
    if (NodeEdgeList) {
        ndo = NumNodeEdgeList;
        for (i=0; i<ndo; i++) {
            if (NodeEdgeList[i].list == NULL  ||
                NodeEdgeList[i].nlist < 1) {
                NodeList[i].deleted = 1;
            }
        }
    }

    RemoveDeletedElements ();

    *numnodes = NumNodes;
    *numedges = NumEdges;
    *numtris = NumTriangles;

    if (NodeEdgeList) {
        ndo = NumNodeEdgeList;
        for (i=0; i<ndo; i++) {
            if (NodeEdgeList[i].list != NULL) {
                csw_Free (NodeEdgeList[i].list);
            }
        }
        csw_Free (NodeEdgeList);
    }
    NodeEdgeList = NULL;

    ListNull ();

    csw_Free (work1);

    return 1;
}


void CSWGrdTriangle::RecurseChew (int index, int *nwork, int *work1, int *work2)
{
    TRiangleStruct       *tptr;
    EDgeStruct           *eptr;
    int                  e1, e2, e3, tnext, tid, nw;

    tid = work1[index];

    if (tid < 0) {
        return;
    }

    tptr = TriangleList + tid;
    e1 = tptr->edge1;
    e2 = tptr->edge2;
    e3 = tptr->edge3;

    nw = *nwork;

    eptr = EdgeList + e1;
    if (eptr->isconstraint == 0  &&  ChewFlags[e1] == 0) {
        tnext = eptr->tri1;
        if (tnext == tid) {
            tnext = eptr->tri2;
        }
        ChewFlags[e1] = 1;
        if (tnext >= 0  &&  work2[tnext] == 0) {
            work1[nw] = tnext;
            work2[tnext] = 1;
            nw++;
        }
    }

    eptr = EdgeList + e2;
    if (eptr->isconstraint == 0  &&  ChewFlags[e2] == 0) {
        tnext = eptr->tri1;
        if (tnext == tid) {
            tnext = eptr->tri2;
        }
        ChewFlags[e2] = 1;
        if (tnext >= 0  &&  work2[tnext] == 0) {
            work1[nw] = tnext;
            work2[tnext] = 1;
            nw++;
        }
    }

    eptr = EdgeList + e3;
    if (eptr->isconstraint == 0  &&  ChewFlags[e3] == 0) {
        tnext = eptr->tri1;
        if (tnext == tid) {
            tnext = eptr->tri2;
        }
        ChewFlags[e3] = 1;
        if (tnext >= 0  &&  work2[tnext] == 0) {
            work1[nw] = tnext;
            work2[tnext] = 1;
            nw++;
        }
    }

    *nwork = nw;

    return;

}



/*----------------------------------------------------------------------------*/

int CSWGrdTriangle::grd_reshape_triangles (
    NOdeStruct       *nodes,
    int              *numnodes,
    EDgeStruct       *edges,
    int              *numedges,
    TRiangleStruct   *tris,
    int              *numtris)
{

    int       do_write;

    NodeList = nodes;
    EdgeList = edges;
    TriangleList = tris;
    NumNodes = *numnodes;
    NumEdges = *numedges;
    NumTriangles = *numtris;

    RemoveZeroLengthEdges ();
    RemoveZeroAreaTriangles ();

    RemoveDeletedElements ();

    do_write = csw_GetDoWrite ();
    if (do_write) {
        char fname1[100];
        double v6[6];
        sprintf (fname1, "postreshape.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname1);
    }

    *numnodes = NumNodes;
    *numedges = NumEdges;
    *numtris = NumTriangles;

    ListNull ();
    FreeMem ();

    return 1;
}


/*-------------------------------------------------------------------*/

/*
 * This is called prior to adding new points where constraints
 * cross edges in the current trimesh.  Nodes within a close
 * perpendicular distance of a constraint segment are deleted
 * from the pre constraint trimesh prior to add ing the constraint
 * segments.  This simplifys the constraint segment addition and
 * the nodes would be removed in any case after the constraint
 * segment addition.
 */
int CSWGrdTriangle::PreDeleteNodesCloseToConstraints (void)
{
    int             i, j, istat, nlist, nt1, nt2;
    int             *close_nodes = NULL, nclose, ndone;
    RAwLineSegStruct *rline = NULL;
    RAwPointStruct  *rp1 = NULL, *rp2 = NULL, *rptr = NULL;
    NOdeStruct      *np = NULL;
    double          x1, y1, x2, y2, dx, dy, dist;
    double          xmid, ymid, xmin, ymin, xmax, ymax;
    double          pdist, dcrit, xt, yt;

    int             *ilist = NULL;


    auto fscope = [&]()
    {
        csw_Free (close_nodes);
    };
    CSWScopeGuard func_scope_guard (fscope);


    ilist = p_int_1000;

/*
 * Allocate a list for potential deleted node numbers.
 */
    close_nodes = (int *)csw_Malloc (NumNodes * sizeof(int));
    if (close_nodes == NULL) {
        return -1;
    }
    nclose = 0;

/*
 * The spillnum member of the node class is set to 1 to flag that
 * the node is close to a constraint and thus needs to be removed.
 * Make sure these are all zero to start.
 */
    for (i=0; i<NumNodes; i++) {
        NodeList[i].spillnum = 0;
    }

/*
 * For each constraint edge, calculate a box centered on the
 * edge and extending twice the length of the edge in width and
 * height.  This box is used in conjunction with the point index
 * to retrieve the nodes near the edge.  This set of nodes is
 * tested for perpendicular distance to the edge and any within
 * .25 times the length of the edge are put into the removal list.
 */
    for (i=0; i<NumRawLines; i++) {

        rline = RawLines + i;
        rp1 = RawPoints + rline->rp1;
        rp2 = RawPoints + rline->rp2;
        nt1 = rp1->nodenum;
        nt2 = rp2->nodenum;
        if (nt1 < 0  ||  nt2 < 0) {
            continue;
        }

    /*
     * Find the mid point of the segment and a square defining
     * the segment's local region.
     */
        x1 = NodeList[nt1].x;
        y1 = NodeList[nt1].y;
        x2 = NodeList[nt2].x;
        y2 = NodeList[nt2].y;

        xmid = (x1 + x2) / 2.0;
        ymid = (y1 + y2) / 2.0;

        dx = x2 - x1;
        dy = y2 - y1;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);

        xmin = xmid - dist;
        ymin = ymid - dist;
        xmax = xmid + dist;
        ymax = ymid + dist;

        dcrit = dist * .25;

    /*
     * Retrieve the nodes from the local area.
     */
        nlist =
        GetIndexPoints (xmin, ymin, xmax, ymax, ilist, 1000);

        if (nlist < 1) {
            continue;
        }

    /*
     * Check each node.  If the node is not a constraint node,
     * and is not locked in place and also has not already
     * been added to the delete list, then chek the perpendicular
     * distance from the node to the segment.  If that distance
     * is less than .25 times the length of the segment, add the
     * node to the delete list.
     */
        for (j=0; j<nlist; j++) {

            rptr = RawPoints + ilist[j];
            nt1 = rptr->nodenum;
            if (nt1 < 0  ||  nt1 >= MaxIndexPoint) {
                continue;
            }
            np = NodeList + nt1;
            if (np->is_locked != 0  ||  np->spillnum != 0) {
                continue;
            }

            xt = np->x;
            yt = np->y;
            istat =
            gpf_perpdistance2 (x1, y1, x2, y2, xt, yt, &pdist);
            if (istat == 1) {
                if (pdist <= dcrit) {
                    np->spillnum = 1;
                    close_nodes[nclose] = nt1;
                    nclose++;
                }
            }
        }
    }

/*
 * Actually remove the close nodes.
 */
    ndone = 0;
    for (i=0; i<nclose; i++) {

        j = close_nodes[i];

        istat = RemoveNode (j);
        if (istat == 1) {
            ndone++;
        }
    }

    for (i=0; i<NumEdges; i++) {
        if (EdgeList[i].deleted == 1) continue;
        EdgeList[i].tflag = 0;
    }

    return ndone;

}



/*
  ****************************************************************************

                       G e t I n d e x P o i n t s

  ****************************************************************************

  Return a list of raw point numbers for the points inside the specified rectangle.

*/

int CSWGrdTriangle::GetIndexPoints (double xmin, double ymin, double xmax, double ymax,
                           int *listout, int maxlistout)

{
    INdexStruct     *iptr;
    int             n, i, j, k, kk, *list, nlist, offset;
    int             i1, i2, j1, j2;

    j1 = (int)((xmin - IndexXmin) / IndexXspace);
    j2 = (int)((xmax - IndexXmin) / IndexXspace);
    i1 = (int)((ymin - IndexYmin) / IndexYspace);
    i2 = (int)((ymax - IndexYmin) / IndexYspace);
    if (j1 < 0) j1 = 0;
    if (j2 >= IndexNcol) j2 = IndexNcol;
    if (i1 < 0) i1 = 0;
    if (i2 >= IndexNrow) i2 = IndexNrow;

    n = 0;
    for (i=i1; i<=i2; i++) {
        offset = i * IndexNcol;
        for (j=j1; j<=j2; j++) {
            k = offset + j;
            iptr = IndexGrid[k];
            if (!iptr) continue;

            list = iptr->list;
            nlist = iptr->npts;
            for (kk=0; kk<nlist; kk++) {
                if (n >= maxlistout) {
                    return -1;
                }
                listout[n] = list[kk];
                n++;
            }
        }
    }

    return n;

}  /*  end of private GetIndexPoints function  */



/*-----------------------------------------------------------------------*/

/*
 * Remove tiny constraint lines from the specified line list.
 */

int CSWGrdTriangle::FilterConstraintLines (
    double    *xlines,
    double    *ylines,
    double    *zlines,
    int       *nv,
    int       *lineflags,
    int       *nlinesio)
{
    int       i, j, n, npts, nout, ntot, istat, flag,
              nlines, nlineout, *nvout = NULL, *lfout = NULL;
    double    *xout = NULL, *yout = NULL, *zout = NULL;


    auto fscope = [&]()
    {
        csw_Free (xout);
        csw_Free (nvout);
    };
    CSWScopeGuard func_scope_guard (fscope);


    nlines = *nlinesio;
    ntot = 0;

    for (i=0; i<nlines; i++) {
        ntot += nv[i];
    }

    xout = (double *)csw_Malloc (3 * ntot * sizeof(double));
    if (xout == NULL) {
        return -1;
    }
    yout = xout + ntot;
    zout = yout + ntot;
    nvout = (int *)csw_Malloc (2 * nlines * sizeof(int));
    if (nvout == NULL) {
        return -1;
    }
    lfout = nvout + nlines;

    nlineout = 0;
    n = 0;
    nout = 0;
    flag = 0;

    for (i=0; i<nlines; i++) {
        npts = nv[i];
        if (lineflags) {
            flag = lineflags[i];
        }
        istat =
          FilterSingleConstraintLine (
            xlines+n,
            ylines+n,
            npts);
        if (istat == 1) {
            n += npts;
            continue;
        }
        for (j=0; j<npts; j++) {
            xout[nout] = xlines[n];
            yout[nout] = ylines[n];
            zout[nout] = zlines[n];
            nout++;
            n++;
        }
        nvout[nlineout] = npts;
        lfout[nlineout] = flag;
        nlineout++;
    }

    memcpy (nv, nvout, nlineout * sizeof(int));
    if (lineflags) {
        memcpy (lineflags, lfout, nlineout * sizeof(int));
    }
    memcpy (xlines, xout, nout * sizeof(double));
    memcpy (ylines, yout, nout * sizeof(double));
    memcpy (zlines, zout, nout * sizeof(double));

    *nlinesio = nlineout;

    return 1;

}





int CSWGrdTriangle::FilterSingleConstraintLine (
    double    *xline,
    double    *yline,
    int       npts)
{
    int       istat, isclosed;
    double    xc, yc, area, dist,
              dcrit, acrit, tiny;

    if (npts < 2) {
        return 1;
    }

    if (AverageEdgeLength > 0.0) {
        dcrit = AverageEdgeLength / 1.5;
    }
    else {
        dcrit = .001;
    }
    acrit = dcrit * dcrit;
    tiny = dcrit / 100.0;

    isclosed = 0;
    istat =
      SamePointTiny (
        xline[0], yline[0],
        xline[npts-1], yline[npts-1],
        tiny);
    if (istat == 1) {
        isclosed = 1;
    }

    if (isclosed  &&  npts < 4) {
        return 1;
    }

    if (isclosed) {
        istat =
          gpf_CalcPolygonAreaAndCentroid (xline, yline, npts,
                                          &xc, &yc, &area);
        if (istat != 1) {
            return 1;
        }
        if (area <= acrit) {
            return 1;
        }
        return 0;
    }

    istat =
      gpf_CalcXYDistanceAlongLine (
        xline, yline, npts, &dist);
    if (istat != 1) {
        return 1;
    }
    if (dist <= dcrit) {
        return 1;
    }

    return 0;

}

void CSWGrdTriangle::setZisAttribute (int ival)
{
    ZisAttribute = ival;
}

int CSWGrdTriangle::grd_reset_trimesh_zvalues_from_grid (
    NOdeStruct     *nodes,
    int            num_nodes,
    EDgeStruct     *edges,
    int            num_edges,
    TRiangleStruct *tris,
    int            num_tris,
    CSW_F          *grid,
    int            ncol,
    int            nrow,
    double         gxmin,
    double         gymin,
    double         gxmax,
    double         gymax,
    FAultLineStruct *faults,
    int            nfaults,
    int            interp_flag,
    int            *lineids
    )
{
    EDgeStruct     *eptr = NULL;
    TRiangleStruct *tptr = NULL;
    NOdeStruct     *nptr = NULL;
    double         *xa = NULL, *ya = NULL, *za = NULL;
    int            *ia = NULL, *list = NULL;
    int            istat, i, j, n, n1, n2, n3, nlist, ndone;
    double         xt, yt, zt, xloc[100], yloc[100], zloc[100];
    int            nloc, ndo;
    int            do_write;


    auto fscope = [&]()
    {
        csw_Free (xa);
        EdgeList = NULL;
        NumEdges = 0;
        NodeList = NULL;
        NumNodes = 0;
    };
    CSWScopeGuard func_scope_guard (fscope);


    faults = faults;
    nfaults = nfaults;

    XYTiny = (gxmax - gxmin + gymax - gymin) / 200000.0;

    num_tris = num_tris;

    lineids = lineids;

    do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_WriteTextTriMeshFile (
            0, NULL,
            tris, num_tris,
            edges, num_edges,
            nodes, num_nodes,
            (char *)"pre_reset.tri");
    }

/*
 * Set the on_border and tflag members of the nodes to zero.
 * Then, set any node either on or adjacent to a constraint
 * to tflag = 1.  In addition, nodes on a constraint have
 * on_border set to 1.
 */
    for (i=0; i<num_nodes; i++) {
        nptr = nodes + i;
        nptr->on_border = 0;
        nptr->bflag = 0;
        nptr->z = 1.e30;
    }

    for (i=0; i<num_edges; i++) {
        eptr = edges + i;

        n1 = eptr->node1;
        n2 = eptr->node2;

        if (eptr->deleted == 1) {
            continue;
        }
        if (eptr->isconstraint == 0) {
            continue;
        }
        if (eptr->tri2 >= 0) {
            continue;
        }
        if (eptr->tri1 < 0) {
            continue;
        }
        tptr = tris + eptr->tri1;
        n1 = eptr->node1;
        nodes[n1].on_border = 1;
        n1 = eptr->node2;
        nodes[n1].on_border = 1;

        istat = grd_get_nodes_for_triangle (
            tptr, edges, &n1, &n2, &n3);
        if (istat != 1) {
            continue;
        }
        nodes[n1].bflag = 1;
        nodes[n2].bflag = 1;
        nodes[n3].bflag = 1;

    }

/*
 * Allocate space for x, y and z to use in back interpolation.
 */
    xa = (double *)csw_Malloc (num_nodes * 4 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + num_nodes;
    za = ya + num_nodes;
    ia = (int *)(za + num_nodes);

/*
 * Put nodes that are not close to faults into the arrays.
 */
    n = 0;
    for (i=0; i<num_nodes; i++) {
        nptr = nodes + i;
        if (ZisAttribute == 0) {
            if (nptr->bflag == 1  ||  nptr->deleted == 1) {
                continue;
            }
        }
        xa[n] = nptr->x;
        ya[n] = nptr->y;
        za[n] = nptr->z;
        ia[n] = i;
        n++;
    }

/*
 * If no nodes were found, try again using adjacent nodes but
 * not border nodes.
 */
    if (n == 0) {
        for (i=0; i<num_nodes; i++) {
            nptr = nodes + i;
            if (nptr->bflag == 1) {
                continue;
            }
            xa[n] = nptr->x;
            ya[n] = nptr->y;
            za[n] = nptr->z;
            ia[n] = i;
            n++;
        }
    }

/*
 * If there are still no nodes, use the first node as a seed node.
 */
    if (n == 0) {
        xa[0] = nodes[0].x;
        ya[0] = nodes[0].y;
        za[0] = nodes[0].z;
        ia[0] = 0;
        n = 1;
    }

/*
 * Interpolate at these nodes from the grid.
 */
    istat =
      grd_arith_ptr->grd_back_interpolate (
        grid, ncol, nrow,
        gxmin, gymin, gxmax, gymax,
        NULL, 0,
        xa, ya, za, n,
        interp_flag);
    if (istat == -1) {
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
      grd_WritePoints (xa, ya, za, n,
                       (char *)"backint.xyz");
    }

/*
 * If there is only one node and it is interpolated as null,
 * try interpolating it without faults.
 */
    if (n == 1) {
        if (za[0] > 1.e20) {
            istat =
              grd_arith_ptr->grd_back_interpolate (
                grid, ncol, nrow,
                gxmin, gymin, gxmax, gymax,
                NULL, 0,
                xa, ya, za, n,
                interp_flag);
            if (istat == -1) {
                return -1;
            }
        }
    }

/*
 * If there was only one node, and it is still null in z,
 * I can't interpolate z values for this patch.
 */
    if (n == 1) {
        if (za[0] > 1.e20) {
            return 0;
        }
    }

/*
 * Put the interpolated values into the trimesh nodes.
 */
    for (i=0; i<n; i++) {
        j = ia[i];
        nodes[j].z = za[i];
    }

/*
 * Build a list of edges connected to nodes.
 */
    EdgeList = edges;
    NodeList = nodes;
    NumEdges = num_edges;
    NumNodes = num_nodes;
    istat =
      BuildNodeEdgeLists ();
    if (istat == -1) {
        return -1;
    }

/*
 * Calculate z values at each trimesh node that still has
 * a null value.  Nodes adjacent to constraints but not
 * on constraints are done here.
 */
    ndo = 0;
    for (;;) {
        ndone = 0;
        for (i=0; i<num_nodes; i++) {
            nptr = nodes + i;
            if (nptr->deleted == 1) {
                continue;
            }
            if (nptr->z < 1.e20) {
                continue;
            }
            if (nptr->on_border == 1) {
                continue;
            }
            list = NodeEdgeList[i].list;
            nlist = NodeEdgeList[i].nlist;
            if (list == NULL  ||  nlist < 1) {
                assert (0);
            }
            nloc = 0;
            xt = nodes[i].x;
            yt = nodes[i].y;
            for (j=0; j<nlist; j++) {
                eptr = edges + list[j];
                n1 = eptr->node1;
                if (n1 == i) {
                    n1 = eptr->node2;
                }
                if (nodes[n1].z < 1.e20) {
                    xloc[nloc] = nodes[n1].x - xt;
                    yloc[nloc] = nodes[n1].y - yt;
                    zloc[nloc] = nodes[n1].z;
                    nloc++;
                    if (nloc > 99) nloc = 99;
                }
            }
            if (nloc == 0) {
                continue;
            }
            zt = CalcLocalZ (xloc, yloc, zloc, nloc);

            if (zt < 1.e20) {
                ia[ndone] = i;
                za[ndone] = zt;
                ndone++;
            }
        }
        if (ndone == 0) {
            break;
        }
        for (i=0; i<ndone; i++) {
            j = ia[i];
            nodes[j].z = za[i];
        }
        ndo++;
        if (ndo >= num_nodes) {
            printf ("infinite loop resetting z values\n");
            assert (0);
        }
    }


/*
 * Calculate z values at each trimesh node that still has
 * a null value.  All nodes still NULL are done here.
 */
    ndo = 0;
    for (;;) {
        ndone = 0;
        for (i=0; i<num_nodes; i++) {
            nptr = nodes + i;
            if (nptr->deleted == 1) {
                continue;
            }
            if (nptr->z < 1.e20) {
                continue;
            }
            list = NodeEdgeList[i].list;
            nlist = NodeEdgeList[i].nlist;
            if (list == NULL  ||  nlist < 1) {
                assert (0);
            }
            nloc = 0;
            xt = nodes[i].x;
            yt = nodes[i].y;
            for (j=0; j<nlist; j++) {
                eptr = edges + list[j];
                n1 = eptr->node1;
                if (n1 == i) {
                    n1 = eptr->node2;
                }
                if (nodes[n1].z < 1.e20) {
                    xloc[nloc] = nodes[n1].x - xt;
                    yloc[nloc] = nodes[n1].y - yt;
                    zloc[nloc] = nodes[n1].z;
                    nloc++;
                    if (nloc > 99) nloc = 99;
                }
            }
            if (nloc == 0) {
                continue;
            }
            zt = CalcLocalZ (xloc, yloc, zloc, nloc);

            if (zt < 1.e20) {
                ia[ndone] = i;
                za[ndone] = zt;
                ndone++;
            }
        }
        if (ndone == 0) {
            break;
        }
        for (i=0; i<ndone; i++) {
            j = ia[i];
            nodes[j].z = za[i];
        }
        ndo++;
        if (ndo >= num_nodes) {
            printf ("infinite loop resetting z values\n");
            assert (0);
        }
    }

/*
 * For any ZERO offset edges, average the z values for the paired edges.
 */
    AveragePairedZValues ();

/*
 * Debug code to check if any null z values still exist.
 */
    for (i=0; i<num_nodes; i++) {
        if (nodes[i].z > 1.e20  &&  nodes[i].deleted == 0) {
            printf ("Node %d still has NULL z value.\n", i);
        }
    }

/*
 * Reset the node flags back to zeros.
 */
    for (i=0; i<num_nodes; i++) {
        nptr = nodes + i;
        nptr->on_border = 0;
        nptr->bflag = 0;
    }

    return 1;
}




/*------------------------------------------------------------------------*/

void CSWGrdTriangle::AveragePairedZValues (void)
{
    int           pairid, i, n1, n2, n3, n4;
    EDgeStruct    *ep1, *ep2;
    NOdeStruct    *np1, *np2;
    double        zavg;
    int           ndone;

    for (i=0; i<NumEdges; i++) {
        ep1 = EdgeList + i;
        if (!(ep1->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT  ||
              ep1->flag == GRD_ZERO_DISCONTINUITY_CONSTRAINT)) {
            continue;
        }
        if (ep1->pairid == 0) continue;
        pairid = ep1->pairid - 1;
        ep2 = EdgeList + pairid;
        n1 = ep1->node1;
        n2 = ep1->node2;
        n3 = ep2->node1;
        n4 = ep2->node2;
        ndone = 0;
        if (SameNodePosition (n1, n3)) {
            np1 = NodeList + n1;
            np2 = NodeList + n3;
            zavg = (np1->z + np2->z) / 2.0;
            np1->z = zavg;
            np2->z = zavg;
            ndone++;
        }
        if (SameNodePosition (n1, n4)) {
            np1 = NodeList + n1;
            np2 = NodeList + n4;
            zavg = (np1->z + np2->z) / 2.0;
            np1->z = zavg;
            np2->z = zavg;
            ndone++;
        }
        if (SameNodePosition (n2, n3)) {
            np1 = NodeList + n2;
            np2 = NodeList + n3;
            zavg = (np1->z + np2->z) / 2.0;
            np1->z = zavg;
            np2->z = zavg;
            ndone++;
        }
        if (SameNodePosition (n2, n4)) {
            np1 = NodeList + n2;
            np2 = NodeList + n4;
            zavg = (np1->z + np2->z) / 2.0;
            np1->z = zavg;
            np2->z = zavg;
            ndone++;
        }
        if (ndone == 0) {
            ndone = ndone;
        }
    }

    return;

}


/*------------------------------------------------------------------------*/

int CSWGrdTriangle::SameNodePosition (int n1, int n2)
{
    NOdeStruct    *np1, *np2;
    double        dx, dy, dist;

    np1 = NodeList + n1;
    np2 = NodeList + n2;

    dx = np1->x - np2->x;
    dy = np1->y - np2->y;

    dist = dx * dx + dy * dy;
    dist = sqrt (dist);

    if (dist <= XYTiny) {
        return 1;
    }

    return 0;
}

/*------------------------------------------------------------------------*/


double CSWGrdTriangle::CalcLocalZ (double *x, double *y, double *z, int npts)
{
    int            i, istat;
    double         dist, dsqval, sum1, sum2, wgt, dsum;
    double         coef[3], zval;
    CSW_F          fx[100], fy[100], ftiny, fdist;
    int            nf;

    if (npts <= 1) {
        return z[0];
    }

    sum1 = 0.0;
    sum2 = 0.0;
    dsum = 0.0;

    for (i=0; i<npts; i++) {
        dist = x[i] * x[i] + y[i] * y[i];
        if (dist < 1.e-10) {
            return z[i];
        }
        dsum += sqrt (dist);
        dist = 1.0 / dist;
        sum1 += z[i] * dist;
        sum2 += dist;
    }

    if (sum2 < 1.e-10) {
        return z[0];
    }

    dsqval = sum1 / sum2;

    if (npts < 3) {
        return dsqval;
    }

    dsum /= npts;
    ftiny = (CSW_F)(dsum / 100.0);
    nf = npts;
    if (nf > 100) {
        nf = 100;
    }
    for (i=0; i<nf; i++) {
        fx[i] = (CSW_F)x[i];
        fy[i] = (CSW_F)y[i];
    }

    istat =
      grd_utils_ptr->grd_colinear_check (fx, fy, nf, ftiny, &fdist);
    if (istat == 1) {
        return dsqval;
    }

    wgt = (double)(npts - 1);
    wgt /= 3.0;
    dsum /= 2.0;
    if (fdist < dsum) {
        wgt *= fdist / dsum;
    }
    wgt *= wgt;
    if (wgt > 3.0) wgt = 3.0;

    istat =
      grd_utils_ptr->grd_calc_double_plane (
        x, y, z, npts, coef);

    if (istat != 1) {
        return dsqval;
    }

    zval = (dsqval +  wgt * coef[0]) / (1.0 + wgt);

    return zval;

}


/*
  ****************************************************************************

                  g r d _ g r i d _ t o _ t r i m e s h

  ****************************************************************************

    Convert a grid (with or without faults) to a trimesh.

*/

int CSWGrdTriangle::grd_grid_to_trimesh
                        (CSW_F *gridin, int nc, int nr,
                         double x1, double y1, double x2, double y2,
                         double *xlinesin, double *ylinesin, double *zlinesin,
                         int *linepointsin, int *linetypes, int nlinesin,
                         int trimesh_style,
                         NOdeStruct **nodes_out, EDgeStruct **edges_out,
                         TRiangleStruct **triangles_out,
                         int *num_nodes_out, int *num_edges_out,
                         int *num_triangles_out)

{
    int                  i, j, k, n, offset, edges_per_row;
    int                  maxraw, ebase, tbase, ke, kt, triangles_per_row;
    int                  top_ebase, istat;
    CSW_F                *grid, zinterp;
    double               xt, yt, xspace, yspace, xyspace;
    NOdeStruct           *nptr;
    EDgeStruct           *eptr;
    TRiangleStruct       *tptr;
    RAwPointStruct       *rptr;

    FreeMem ();

/*
 * If the trimesh flag is anything other than 1 (GRD_CELL_DIAGONALS)
 * then calculate a very nearly equilateral trimesh from the grid.
 */
    if (trimesh_style != GRD_CELL_DIAGONALS  &&  DontDoEquilateral == 0) {
        istat = grd_grid_to_equilateral_trimesh (
            gridin, nc, nr,
            x1, y1, x2, y2,
            xlinesin, ylinesin, zlinesin,
            linepointsin, linetypes, nlinesin,
            nodes_out, edges_out, triangles_out,
            num_nodes_out, num_edges_out, num_triangles_out);
        return istat;
    }

/*
 * The lines are no longer applied to the trimesh in this function.
 * After the unconstrained trimesh is returned, the lines should
 * be applied with grd_add_lines_to_trimesh.
 */
    nlinesin = 0;
    xlinesin = NULL;
    ylinesin = NULL;
    zlinesin = NULL;
    linepointsin = NULL;
    linetypes = NULL;

    if (nlinesin <= 0) {
        ChopLinesFlag = 0;
        nlinesin = -nlinesin;
    }
    else {
        ChopLinesFlag = 1;
    }

    ConstraintPointStart = 0;

/*
    Set the output to invalid values in case an error occurs.
*/
    *nodes_out = NULL;
    *num_nodes_out = 0;
    *edges_out = NULL;
    *num_edges_out = 0;
    *triangles_out = NULL;
    *num_triangles_out = 0;

    xspace = (x2 - x1) / (double)(nc - 1);
    yspace = (y2 - y1) / (double)(nr - 1);
    xyspace = xspace * xspace + yspace * yspace;
    xyspace = sqrt (xyspace);

    GridX1 = x1;
    GridY1 = y1;
    GridX2 = x1 + (nc - 1) * xspace;
    GridY2 = y1 + (nr - 1) * yspace;
    GridXspace = xspace;
    GridYspace = yspace;
    GridNcol = nc;
    GridNrow = nr;

    AreaPerimeter = x2 - x1 + y2 - y1;
    GrazeDistance = (x2 - x1 + y2 - y1) / 2000000.0;
    AdjustDistance = (x2 - x1 + y2 - y1) / 200000.0;
    FaultAdjustDistance = AdjustDistance * 10.0;
    ConstraintSegmentNumber = -1;
    CornerBias = 1.0;
    NtryFlag = 0;
    Nbugs = 0;
    MaxNcall = (nr + nc) * 10;

  /*
   * Only the cell diagonal style is now supported.
   * The GRD_EQUILATERAL style is done by grd_grid_to_equilateral_trimesh
   */
    trimesh_style = trimesh_style;

    grid = gridin;

  /*
   * Allocate space for the raw points.
   */
    n = nc * nr;
    n += nr;
    RawPoints = (RAwPointStruct *)csw_Calloc (n * sizeof(RAwPointStruct));
    NumRawPoints = 0;
    MaxRawPoints = n;
    if (RawPoints == NULL) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        ResetGridStaticValues ();
        return -1;
    }
    maxraw = n;

    NumRawPoints = nc * nr;

/*
    Allocate space for the initial triangle, edge and node lists.
    The initial sizes are sufficient for a non constrained trimesh
    but can grow larger if constraints are specified.
*/
    n = nc * nr * 4;
    n += nr;
    TriangleList = (TRiangleStruct *)csw_Calloc (n * sizeof(TRiangleStruct));
    if (!TriangleList) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        ResetGridStaticValues ();
        return -1;
    }
    MaxTriangles = n;
    NumTriangles = 0;

    n = nr * (nc * 4 + 1);
    n += nr;
    EdgeList = (EDgeStruct *)csw_Calloc (n * sizeof(EDgeStruct));
    if (!EdgeList) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        ResetGridStaticValues ();
        return -1;
    }
    MaxEdges = n;
    NumEdges = 0;

    n = nc * nr * 2;
    if (maxraw > n) n = maxraw;
    n += nr * 2;
    NodeList = (NOdeStruct *)csw_Calloc (n * sizeof(NOdeStruct));
    if (!NodeList) {
        grd_utils_ptr->grd_set_err (1);
        FreeMem ();
        ResetGridStaticValues ();
        return -1;
    }
    MaxNodes = n;
    NumNodes = 0;

/*
    Set the x and y coordinates of the trimesh nodes and raw points to the
    coordinates of the grid nodes.
*/
    for (i=0; i<nr; i++) {
        yt = y1 + i * yspace;
        offset = i * nc;
        for (j=0; j<nc; j++) {
            xt = x1 + j * xspace;
            k = offset + j;
            nptr = NodeList + k;
            zinterp = grid[k];
            nptr->x = xt;
            nptr->y = yt;
            nptr->z = zinterp;
            nptr->adjusting_node = -1;
            nptr->spillnum = -1;
            nptr->rp = k;
            nptr->crp = -1;
            rptr = RawPoints + k;
            rptr->x = xt;
            rptr->y = yt;
            rptr->z = grid[k];
            rptr->nodenum = k;
        }
    }
    NumNodes = nc * nr;

/*
   Create the edges.  Each grid cell has an edge on its left side, on its
   bottom side and a diagonal edge.  For cells in even numbered rows, the
   diagonal is from lower left to upper right.  For cells in odd number
   rows, the diagonal is from upper left to lower right. This is done to
   connect more equilateral triangles in conjuction with the left shifting
   of the nodes in the odd rows.  The numbers for these edges, within the
   cell are 1, 2 and 3 respectively.  Each cell's numbering starts at 3 *
   the column number of the node at the lower left corner of the cell.
*/
    edges_per_row = (nc - 1) * 3 + 1;
    triangles_per_row = (nc - 1) * 2;

/*
   Do all but the top row.
*/
    for (i=0; i<nr-1; i++) {
        offset = i * nc;
        ebase = i * edges_per_row;
    /*
       Do all but the right most column.
    */
        for (j=0; j<nc-1; j++) {
            k = ebase + j * 3;
            eptr = EdgeList + k;
            eptr->node1 = offset + j;
            eptr->node2 = offset + j + nc;
            eptr->length = yspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;

            eptr = EdgeList + k + 1;
            eptr->node1 = offset + j;
            eptr->node2 = offset + j + 1;
            eptr->length = xspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;

            eptr = EdgeList + k + 2;
            eptr->node1 = offset + j;
            eptr->node2 = offset + j + nc + 1;
            eptr->length = xyspace;
            eptr->tri1 = -1;
            eptr->tri2 = -1;
        }
    /*
       Do the right most column.
    */
        k = ebase + edges_per_row - 1;
        eptr = EdgeList + k;
        eptr->node1 = offset + j;
        eptr->node2 = offset + j + nc;
        eptr->length = yspace;
        eptr->tri1 = -1;
        eptr->tri2 = -1;
    }
/*
    Do the top row.
*/
    offset = (nr - 1) * nc;
    top_ebase = (nr - 1) * edges_per_row;
    for (j=0; j<nc-1; j++) {
        k = top_ebase + j;
        eptr = EdgeList + k;
        eptr->node1 = offset + j;
        eptr->node2 = offset + j + 1;
        eptr->length = xspace;
        eptr->tri1 = -1;
        eptr->tri2 = -1;
    }

    NumEdges = (nr - 1) * edges_per_row + nc - 1;

/*
    Fill in the triangle list.  Each cell has two triangles.
    In even numbered rows, the first uses the bottom, right
    and diagonal while the second uses the left, top and
    diagonal.  In odd numbered rows, the first triangle uses
    the left, bottom and diagonal while the second triangle
    uses the top, right and diagonal.
*/
    for (i=0; i<nr-1; i++) {
        tbase = i * triangles_per_row;
        ebase = i * edges_per_row;

        for (j=0; j<nc-1; j++) {
            kt = tbase + j * 2;
            ke = ebase + j * 3;

            tptr = TriangleList + kt;

            tptr->edge1 = ke + 1;
            eptr = EdgeList + ke + 1;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }

            tptr->edge2 = ke + 2;
            eptr = EdgeList + ke + 2;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }

            tptr->edge3 = ke + 3;
            eptr = EdgeList + ke + 3;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt;
            }
            else {
                eptr->tri2 = kt;
            }

            tptr = TriangleList + kt + 1;

            tptr->edge1 = ke;
            eptr = EdgeList + ke;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt + 1;
            }
            else {
                eptr->tri2 = kt + 1;
            }

            tptr->edge2 = ke + 2;
            eptr = EdgeList + ke + 2;
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt + 1;
            }
            else {
                eptr->tri2 = kt + 1;
            }

            tptr->edge3 = ke + edges_per_row + 1;
            if (i == nr-2) {
                tptr->edge3 = top_ebase + j;
            }
            if (i == nr-2) {
                eptr = EdgeList + top_ebase + j;
            }
            else {
                eptr = EdgeList + ke + edges_per_row + 1;
            }
            if (eptr->tri1 == -1) {
                eptr->tri1 = kt + 1;
            }
            else {
                eptr->tri2 = kt + 1;
            }
        }
    }
    NumTriangles = (nr - 1) * triangles_per_row;

/*
    Clean up artifacts from colinear points and points in
    almost exactly the same location.
    RemoveZeroLengthEdges ();
    RemoveZeroAreaTriangles ();
*/

    ForceValidate = 0;
    ValidateEdgeIntersection (NULL);
    ForceValidate = 0;

/*
    return the results and clean up any work space
*/
    *nodes_out = NodeList;
    *num_nodes_out = NumNodes;
    *edges_out = EdgeList;
    *num_edges_out = NumEdges;
    *triangles_out = TriangleList;
    *num_triangles_out = NumTriangles;

    ListNull ();
    FreeMem ();

    ResetGridStaticValues ();
    return 1;

}  /*  end of function grd_grid_to_trimesh  */

/*
 ****************************************************************************

               R e m o v e N u l l s F r o m T r i M e s h

 ****************************************************************************

*/

int CSWGrdTriangle::RemoveNullsFromTriMesh (double nullval, int edge_swap_flag)

{
    int                 i, i1, i2, n1, n2;
    EDgeStruct          *eptr;
    int                 do_write;
    double              v6[6];
    char                fname[100];

    NullValue = nullval;
    EdgeSwapFlag = edge_swap_flag;
    if (edge_swap_flag <= 0) {
        EdgeSwapFlag = SWAP_FOR_NULL_REMOVAL;
        edge_swap_flag = 1;
    }
    CheckNullPointers = 0;
    if (nullval == GRD_CHECK_FOR_NULL_POINTER) {
        CheckNullPointers = 1;
    }

/*
    Swap edges that have one null and one non null end point.
*/
    if (edge_swap_flag >= 0) {
        for (i=0; i<NumEdges; i++) {
            eptr = EdgeList + i;
            if (eptr->deleted == 1) {
                continue;
            }
            n1 = eptr->node1;
            n2 = eptr->node2;
            if (n1 < 0  ||  n2 < 0) {
                continue;
            }
            i1 = NodeIsNull (n1);
            i2 = NodeIsNull (n2);
            if (i1 != i2) {
                SwapEdge (i);
            }
        }
    }

/*
    Whack the edges that have one or more null nodes.
    The triangles that use the edge in common also need
    to be whacked.  No additional topology is built.
    This process leaves "holes" in the tri mesh.
*/
    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->deleted == 1) {
            continue;
        }
        n1 = eptr->node1;
        n2 = eptr->node2;
        if (n1 < 0  ||  n2 < 0) {
            continue;
        }
        i1 = NodeIsNull (n1);
        i2 = NodeIsNull (n2);
        if (i1 || i2) {
            WhackEdge (i);
        }
        if (i1) {
            NodeList[n1].deleted = 1;
        }
        if (i2) {
            NodeList[n2].deleted = 1;
        }
    }

    RemoveDeletedElements ();

    do_write = csw_GetDoWrite ();
    if (do_write) {
        sprintf (fname, "nulls_removed.tri");
        grd_WriteTextTriMeshFile (
            0, v6,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    EdgeSwapFlag = GRD_SWAP_ANY;
    CheckNullPointers = 0;

    return 1;

}  /* end of private RemoveNullsFromTriMesh */



/*
 *****************************************************************************

     g r d _ S u b d i v i d e T r i a n g l e s F r o m C e n t e r s

 *****************************************************************************
*/

int CSWGrdTriangle::grd_SubdivideTrianglesFromCenters (
    int               *trinums,
    double            *xcenters,
    double            *ycenters,
    double            *zcenters,
    int               ncenters,
    NOdeStruct        **nodes,
    int               *numnodesout,
    EDgeStruct        **edges,
    int               *numedgesout,
    TRiangleStruct    **tris,
    int               *numtrisout)
{
    int               i, istat;
    TRiangleStruct    *tp;

    FreeMem ();

    NodeList = *nodes;
    EdgeList = *edges;
    TriangleList = *tris;
    NumNodes = *numnodesout;
    NumEdges = *numedgesout;
    NumTriangles = *numtrisout;
    MaxNodes = NumNodes;
    MaxEdges = NumEdges;
    MaxTriangles = NumTriangles;

/*
 * Use the edge tflag member to record if the edge may
 * need swapping.  All edges are initialized as if they
 * will not need swapping by setting tflag to zero.
 */
    for (i=0; i<NumEdges; i++) {
        EdgeList[i].tflag = 0;
    }

/*
 * Split each triangle and flag each edge as possibly needing
 * swapping.
 */
    NewEdgeTflag = 1;
    for (i=0; i<ncenters; i++) {
        tp = TriangleList + trinums[i];
        EdgeList[tp->edge1].tflag = 1;
        EdgeList[tp->edge2].tflag = 1;
        EdgeList[tp->edge3].tflag = 1;
        istat =
          SplitTriangleXYZ (
            trinums[i],
            xcenters[i],
            ycenters[i],
            zcenters[i] );
        if (istat == -1) {
            return -1;
        }
    }
    NewEdgeTflag = 0;

/*
 * Swap edges if needed.
 */
    for (i=0; i<NumEdges; i++) {
        if (EdgeList[i].tflag == 0) continue;
        SwapEdge (i);
    }

    *nodes = NodeList;
    *edges = EdgeList;
    *tris = TriangleList;
    *numnodesout = NumNodes;
    *numedgesout = NumEdges;
    *numtrisout = NumTriangles;
    ListNull ();
    FreeMem ();

    return 1;
}




/*
  ****************************************************************************

                 S p l i t T r i a n g l e X Y Z

  ****************************************************************************

    Given a triangle number and the number of a raw point inside the triangle,
  create three new triangles by connecting the raw point to each of the corners
  of the existing triangle.

*/

int CSWGrdTriangle::SplitTriangleXYZ (int triangle_num,
                             double xp, double yp, double zp)

{
    int                   istat, iszero;
    int                   e1, e2, e3;
    int                   newnode, trinum;
    int                   newe1, newe2, newe3;
    int                   newt1, newt2;
    NOdeStruct            *nptr;
    EDgeStruct            *newep1, *newep2, *newep3,
                          *oldep1, *oldep2, *oldep3;
    TRiangleStruct        *tptr;


/*
    Get the edges of the specified triangle.
*/
    tptr = TriangleList + triangle_num;
    e1 = tptr->edge1;
    e2 = tptr->edge2;
    e3 = tptr->edge3;
    if (SwapFlags) {
        SwapFlags[e1] = 1;
        SwapFlags[e2] = 1;
        SwapFlags[e3] = 1;
    }

/*
    Add the specified point as a new node.
*/
    newnode = AddNode (xp, yp, zp, 0);
    if (newnode < 0) {
        return -1;
    }
    tptr = TriangleList + triangle_num;
    nptr = NodeList + newnode;
    nptr->rp = -1;

/*
    If the new node is almost exactly on an edge of the triangle,
    use that edge to split each of its two adjoining triangles into
    two smaller triangles.
*/
    SnapToEdge = 0;
    newe1 = NodeOnEdge (triangle_num, newnode);
    SnapToEdge = 1;
    if (newe1 >= 0) {
        istat = SplitFromEdge (newe1, newnode, NULL);
        return istat;
    }

/*
    Create three new edges to connect the specified
    point to the corners of the specified triangle.
*/
    newe1 = AddEdge (0, 0, -1, -1, 0);
    newe2 = AddEdge (0, 0, -1, -1, 0);
    newe3 = AddEdge (0, 0, -1, -1, 0);
    if (newe3 < 0) {
        return -1;
    }

    newep1 = EdgeList + newe1;
    newep2 = EdgeList + newe2;
    newep3 = EdgeList + newe3;
    oldep1 = EdgeList + e1;
    oldep2 = EdgeList + e2;
    oldep3 = EdgeList + e3;

    newep1->node1 = oldep1->node1;
    newep1->node2 = newnode;
    newep2->node1 = oldep1->node2;
    newep2->node2 = newnode;
    if (oldep2->node1 != oldep1->node1  &&  oldep2->node1 != oldep1->node2) {
        newep3->node1 = oldep2->node1;
    }
    else {
        newep3->node1 = oldep2->node2;
    }
    newep3->node2 = newnode;
    newep1->length = NodeDistance (newep1->node1, newep1->node2);
    newep2->length = NodeDistance (newep2->node1, newep2->node2);
    newep3->length = NodeDistance (newep3->node1, newep3->node2);

/*
    The first new triangle will use the same memory as the
    specified input triangle.  The first edge in the input
    triangle is combined with the new edges that use the
    same nodes.
*/
    tptr = TriangleList + triangle_num;
    tptr->edge2 = newep1 - EdgeList;
    newep1->tri1 = tptr - TriangleList;
    tptr->edge3 = newep2 - EdgeList;
    newep2->tri1 = tptr - TriangleList;

    iszero = 0;
    iszero = iszero;

/*
    The second triangle is appended to the triangle list.  It uses the
    second edge of the input triangle and the two new edges that share
    nodes with this second edge.
*/
    newt1 = AddTriangle (0, 0, 0, 0);
    if (newt1 < 0) {
        return -1;
    }
    tptr = TriangleList + newt1;
    newep1 = EdgeList + newe1;
    newep2 = EdgeList + newe2;
    newep3 = EdgeList + newe3;
    oldep1 = EdgeList + e1;
    oldep2 = EdgeList + e2;
    oldep3 = EdgeList + e3;

    trinum = tptr - TriangleList;
    if (oldep2->tri1 == triangle_num) {
        oldep2->tri1 = trinum;
    }
    else {
        oldep2->tri2 = trinum;
    }
    tptr->edge1 = oldep2 - EdgeList;
    tptr->edge2 = -1;
    tptr->edge3 = -1;

    if (newep1->node1 == oldep2->node1  ||  newep1->node1 == oldep2->node2) {
        tptr->edge2 = newep1 - EdgeList;
        newep1->tri2 = trinum;
    }
    if (newep2->node1 == oldep2->node1  ||  newep2->node1 == oldep2->node2) {
        if (tptr->edge2 == -1) {
            tptr->edge2 = newep2 - EdgeList;
        }
        else {
            tptr->edge3 = newep2 - EdgeList;
        }
        newep2->tri2 = trinum;
    }
    if (tptr->edge3 == -1) {
        if (newep3->node1 == oldep2->node1  ||  newep3->node1 == oldep2->node2) {
            tptr->edge3 = newep3 - EdgeList;
        }
        newep3->tri1 = trinum;
    }

/*
    The third triangle is appended to the triangle list.  It uses the
    third edge of the input triangle and the two new edges that share
    nodes with this second edge.
*/
    newt2 = AddTriangle (0, 0, 0, 0);
    newep1 = EdgeList + newe1;
    newep2 = EdgeList + newe2;
    newep3 = EdgeList + newe3;
    oldep1 = EdgeList + e1;
    oldep2 = EdgeList + e2;
    oldep3 = EdgeList + e3;
    if (newt2 < 0) {
        return -1;
    }
    tptr = TriangleList + newt2;
    trinum = tptr - TriangleList;
    if (oldep3->tri1 == triangle_num) {
        oldep3->tri1 = trinum;
    }
    else {
        oldep3->tri2 = trinum;
    }
    tptr->edge1 = oldep3 - EdgeList;
    tptr->edge2 = -1;
    tptr->edge3 = -1;

    if (newep1->node1 == oldep3->node1  ||  newep1->node1 == oldep3->node2) {
        tptr->edge2 = newep1 - EdgeList;
        newep1->tri2 = trinum;
    }
    if (newep2->node1 == oldep3->node1  ||  newep2->node1 == oldep3->node2) {
        if (tptr->edge2 == -1) {
            tptr->edge2 = newep2 - EdgeList;
        }
        else {
            tptr->edge3 = newep2 - EdgeList;
        }
        newep2->tri2 = trinum;
    }
    if (tptr->edge3 == -1) {
        if (newep3->node1 == oldep3->node1  ||  newep3->node1 == oldep3->node2) {
            tptr->edge3 = newep3 - EdgeList;
        }
        newep3->tri2 = trinum;
    }

    return 1;

}  /*  end of private SplitTriangleXYZ function  */


/*--------------------------------------------------------------------------*/

/*
 * Collection of functions to smooth trimeshes.
 */

/*--------------------------------------------------------------------------*/

void CSWGrdTriangle::grd_set_trimesh (
    NOdeStruct     *nodes,
    int            num_nodes,
    EDgeStruct     *edges,
    int            num_edges,
    TRiangleStruct *tris,
    int            num_tris)
{
    FreeMem ();

    NodeList = nodes;
    NumNodes = num_nodes;
    EdgeList = edges;
    NumEdges = num_edges;
    TriangleList = tris;
    NumTriangles = num_tris;

    return;
}


void CSWGrdTriangle::grd_unset_trimesh (void)
{
    ListNull ();
}



int CSWGrdTriangle::grd_smooth_trimesh_nodes (
    NOdeStruct     *nodes,
    int            numnodes,
    EDgeStruct     *edges,
    int            numedges,
    TRiangleStruct *tris,
    int            numtris,
    int            sfactin,
    double         avlen)

{
    int            i, istat, sfsave;
    double         sfact;

    sfsave = sfactin;
    if (sfactin > 100) sfactin -= 100;
    sfact = (double)sfactin / 6.0;
    sfact *= sfact;
    if (sfactin > 6) {
        sfact *= sfact;
    }
    sfactin = sfsave;

    FreeMem ();

    NodeList = nodes;
    NumNodes = numnodes;
    EdgeList = edges;
    NumEdges = numedges;
    TriangleList = tris;
    NumTriangles = numtris;

/*
 * Build lists of edges connected to each node.
 */
    istat =
      BuildNodeEdgeLists ();
    if (istat == -1) {
        ListNull ();
        FreeMem ();
        return -1;
    }

    if (NodeEdgeList == NULL) {
        ListNull ();
        FreeMem ();
        return -1;
    }

/*
 * Put the node z values for input to smoothing into the zorig
 * members, the output will be put into the z members.
 */
    for (i=0; i<NumNodes; i++) {
        NodeList[i].zorig = NodeList[i].z;
    }

    istat =
      SmoothTrimeshUsingGrid (sfactin, avlen);

    ListNull ();
    FreeMem ();

    return istat;

}


#if 0
/*----------------------------------------------------------------------*/

int CSWGrdTriangle::BuildTriangleFan (int nodenum,
                             int *tfan,
                             int *efan,
                             int maxfan,
                             int *nfanout)
{
    NOdeEdgeListStruct    *neptr;
    EDgeStruct            *eptr;
    int                   *list, nlist, i, nfan;

    *nfanout = 0;
    if (nodenum < 0) return -1;

    neptr = NodeEdgeList + nodenum;
    list = neptr->list;
    nlist = neptr->nlist;
    nfan = 0;

    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        AddToTriangleFan (
                  nodenum,
                  eptr->tri1,
                  tfan, efan, maxfan,
                  &nfan);
    }

    *nfanout = nfan;

    return 1;

}

/*--------------------------------------------------------------------------*/

void CSWGrdTriangle::AddToTriangleFan
                     (int nodenum,
                      int trinum,
                      int *tfan,
                      int *efan,
                      int maxfan,
                      int *nfanout)
{
    int           i, nfan;

    if (*nfanout >= maxfan) {
        return;
    }

    nfan = *nfanout;

    for (i=0; i<nfan; i++) {
        if (tfan[i] == trinum) {
            return;
        }
    }

    tfan[nfan] = trinum;
    if (trinum >= 0) {
        efan[nfan] = OppositeEdge (TriangleList+trinum, nodenum);
    }
    else {
        efan[nfan] = -1;
    }

    *nfanout = nfan + 1;

    return;

}

#endif


/*------------------------------------------------------------------------------*/

int CSWGrdTriangle::grd_bezier_smooth_triangle (
    TRiangleStruct      *tptr,
    EDgeStruct          *edges,
    NOdeStruct          *nodes,
    double              xin,
    double              yin,
    double              *zout)
{
    double              bpoints[10];
    int                 istat;

    istat =
      CalcBezierControlPoints (tptr, bpoints);
    if (istat == -1) {
        return -1;
    }

    edges = edges;
    nodes = nodes;
    xin = xin;
    yin = yin;
    zout = zout;

    return 1;

}

/*------------------------------------------------------------------------------*/

int CSWGrdTriangle::grd_bezier_smooth_triangle_center (
    TRiangleStruct      *tptr,
    double              *zout)
{
    int                 istat, i, l, m, r;
    double              u[3], b[10];

    istat =
      CalcBezierControlPoints (tptr, b);
    if (istat == -1) {
        return -1;
    }

    u[0] = 1.0 / 3.0;
    u[1] = 1.0 / 3.0;
    u[2] = 1.0 - (u[0] + u[1]);

    for (r=1; r<=3; r++) {
        m = -1;
        for (i=0; i<=3-r; i++) {
            for (l=0; l<=i; l++) {
                m++;
                b[m] = u[0] * b[m] +
                       u[1] * b[m+1+i] +
                       u[2] * b[m+2+i];
            }
        }
    }

    *zout = b[0];

    return 1;

}

/*------------------------------------------------------------------------------*/

/*
 * Use the node normal and the location of the node to calculate
 * z at the specified x,y location.  This is used to fill in the
 * bezier triangle interpolation control points.
 */

double CSWGrdTriangle::CalcZForNodeNormal (double x, double y,
                                  NOdeStruct *nptr)
{
    double               x0, y0, z0, z, zout, nx, ny, nz;

    zout = 1.e30;

    if (nptr == NULL) {
        return zout;
    }

    if (nptr->norm == NULL) {
        return zout;
    }

    if (nptr->deleted == 1) {
        return zout;
    }

    x0 = nptr->x;
    y0 = nptr->y;
    z0 = nptr->z;

    nx = nptr->norm->nx;
    ny = nptr->norm->ny;
    nz = nptr->norm->nz;

    if (nz < 1.e-8) {
        return zout;
    }

    z = (nx * (x - x0) + ny * (y - y0)) / -nz;
    zout = z + z0;

    return zout;

}

/*----------------------------------------------------------------------------*/

double CSWGrdTriangle::CalcZForTriangleNormal (
    double          x,
    double          y,
    TRiangleStruct  *tptr)
{
    double          zt, nzt, zt0;
    NOdeStruct      *nptr;
    EDgeStruct      *eptr;
    int             e1, n1, n2, n3;

    e1 = tptr->edge1;
    eptr = EdgeList + e1;
    n1 = eptr->node1;
    n2 = eptr->node2;
    n3 = OppositeNode (tptr-TriangleList, e1);

    zt = nzt = 0.0;

    nptr = NodeList + n1;
    zt0 = CalcZForNodeNormal (x, y, nptr);
    if (zt0 < 1.e20) {
        zt += zt0;
        nzt++;
    }
    nptr = NodeList + n2;
    zt0 = CalcZForNodeNormal (x, y, nptr);
    if (zt0 < 1.e20) {
        zt += zt0;
        nzt++;
    }
    nptr = NodeList + n3;
    zt0 = CalcZForNodeNormal (x, y, nptr);
    if (zt0 < 1.e20) {
        zt += zt0;
        nzt++;
    }

    if (nzt < 1) {
        return 1.e30;
    }

    zt /= nzt;

    return zt;

}


/*----------------------------------------------------------------------------*/

double CSWGrdTriangle::CalcZForEdgeNormal (
    double          x,
    double          y,
    EDgeStruct      *eptr)
{
    double          zt, nzt, zt0;
    NOdeStruct      *nptr;
    int             n1, n2;

    n1 = eptr->node1;
    n2 = eptr->node2;

    zt = nzt = 0.0;

    nptr = NodeList + n1;
    zt0 = CalcZForNodeNormal (x, y, nptr);
    if (zt0 < 1.e20) {
        zt += zt0;
        nzt++;
    }
    nptr = NodeList + n2;
    zt0 = CalcZForNodeNormal (x, y, nptr);
    if (zt0 < 1.e20) {
        zt += zt0;
        nzt++;
    }

    if (nzt < 1) {
        return 1.e30;
    }

    zt /= nzt;

    return zt;

}



/*----------------------------------------------------------------------*/

/*
 * Calculate the 10 bezier triangle control points.
 */

int CSWGrdTriangle::CalcBezierControlPoints (
    TRiangleStruct  *tptr,
    double          *bpoints)
{
    int             istat, n1, n2, n3, e1;
    EDgeStruct      *eptr;

    e1 = tptr->edge1;
    eptr = EdgeList + e1;
    n1 = eptr->node1;
    n2 = eptr->node2;
    n3 = OppositeNode (tptr-TriangleList, e1);

    istat = CalcBezierControlPointsFromNodes (
        n1, n2, n3, bpoints);

    return istat;

}


int CSWGrdTriangle::CalcBezierControlPointsFromNodes (
    int n1,
    int n2,
    int n3,
    double *bpoints)
{

    NOdeStruct      *np003, *np300, *np030;
    double          x003, x102, x201, x300,
                    x012, x210,
                    x021, x120,
                    x030;
    double          y003, y102, y201, y300,
                    y012, y210,
                    y021, y120,
                    y030;
    double          z003, z102, z201, z300,
                    z012, z111, z210,
                    z021, z120,
                    z030;
    double          dx, dy;

    if (n1 < 0  ||  n2 < 0  ||  n3 < 0) {
        n1 = n1;
    }

    np003 = NodeList + n1;
    x003 = np003->x;
    y003 = np003->y;
    z003 = np003->z;

    np300 = NodeList + n2;
    x300 = np300->x;
    y300 = np300->y;
    z300 = np300->z;

    np030 = NodeList + n3;
    x030 = np030->x;
    y030 = np030->y;
    z030 = np030->z;

/*
 * Points 1/3rd of the way along the 003 to 300 edge.
 */
    dx = (x300 - x003) / 3.0;
    dy = (y300 - y003) / 3.0;
    x102 = x003 + dx;
    y102 = y003 + dy;
    x201 = x102 + dx;
    y201 = y102 + dy;

    dx = (x030 - x003) / 3.0;
    dy = (y030 - y003) / 3.0;
    x012 = x003 + dx;
    y012 = y003 + dy;
    x021 = x012 + dx;
    y021 = y012 + dy;

    dx = (x030 - x300) / 3.0;
    dy = (y030 - y300) / 3.0;
    x210 = x300 + dx;
    y210 = y300 + dy;
    x120 = x210 + dx;
    y120 = y210 + dy;

/*
 * Find the z values at the edge control points using
 * the x,y locations and the normals at the triangle
 * corner nodes.
 */
    z102 = CalcZForNodeNormal (x102, y102, np003);
    z012 = CalcZForNodeNormal (x012, y012, np003);
    z201 = CalcZForNodeNormal (x201, y201, np300);
    z210 = CalcZForNodeNormal (x210, y210, np300);
    z021 = CalcZForNodeNormal (x021, y021, np030);
    z120 = CalcZForNodeNormal (x120, y120, np030);

/*
 * Use a weighted average of the control node z values
 * for the center control node z value.
 */
    z111 = (z102 + z201 + z012 + z021 + z120 + z210) / 4.0 -
           (z003 + z300 + z030) / 6.0;

/*
 * Put the points into the bpoints array in reverse order
 * of the uvw coordinates.
 */
    bpoints[0] = z300;
    bpoints[1] = z210;
    bpoints[2] = z201;
    bpoints[3] = z120;
    bpoints[4] = z111;
    bpoints[5] = z102;
    bpoints[6] = z030;
    bpoints[7] = z021;
    bpoints[8] = z012;
    bpoints[9] = z003;

    return 1;

}


/*--------------------------------------------------------------------*/

int CSWGrdTriangle::SmoothTrimeshUsingGrid (int sfact, double avlen)
{
    int                   istat, i, navg, ncol, nrow;
    CSW_F                 *grid = NULL;
    double                xmin, ymin, xmax, ymax, tiny, zavg;
    NOdeStruct            *nptr = NULL, *np2 = NULL;
    EDgeStruct            *eptr = NULL, *ep2 = NULL;
    int                   nfedge;
    NOdeEdgeListStruct    *neptr = NULL;
    int                   *list = NULL, nlist, j;
    int                   ndone;
    CSW_F                 fx, fy;
    CSW_F                 *fxa = NULL, *fya = NULL, *fza = NULL;
    int                   *ia = NULL, na;
    int                   dbnodelist[100];
    FAultLineStruct       *faults = NULL;
    int                   nfaults = 0;


    auto fscope = [&]()
    {
        CleanupTriNormals ();
        CleanupNodeNormals ();
        csw_Free (grid);
        csw_Free (fxa);
        csw_Free (ia);
        grd_fault_ptr->grd_free_fault_line_structs (faults, nfaults);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (avlen <= 0.000001) {
        return -1;
    }

/*
 * Calculate a grid from the trimesh.  This grid will honor
 * faults if the trimesh honors faults, so no fault vectors
 * need to be specified here.
 */
    xmin = ymin = 1.e30;
    xmax = ymax = -1.e30;
    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        if (nptr->deleted == 1) continue;
        if (nptr->x < xmin) xmin = nptr->x;
        if (nptr->y < ymin) ymin = nptr->y;
        if (nptr->x > xmax) xmax = nptr->x;
        if (nptr->y > ymax) ymax = nptr->y;
    }

    if (xmax <= xmin  ||  ymax <= ymin) {
        return -1;
    }

    if (xmax > 1.e20  ||  ymax > 1.e20) {
        return -1;
    }

    tiny = (xmax - xmin + ymax - ymin) / 200.0;
    xmin -= tiny;
    ymin -= tiny;
    xmax += tiny;
    ymax += tiny;

    ncol = (int)((xmax - xmin) / avlen + 1.5);
    nrow = (int)((ymax - ymin) / avlen + 1.5);

    grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (grid == NULL) {
        return -1;
    }

    DontFree = 1;
    istat =
      grd_calc_grid_from_trimesh (
        NodeList, NumNodes,
        EdgeList, NumEdges,
        TriangleList, NumTriangles,
        1.e30,
        grid, ncol, nrow,
        xmin, ymin, xmax, ymax);
    DontFree = 0;
    if (istat == -1) {
        return -1;
    }

    int do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        grd_WriteXYZGridFile ("GridFromTri.xyz",
                              grid, ncol, nrow,
                              1,
                              xmin, ymin, xmax, ymax);
    }        

  /*
   * The grid may have and probably will have several null nodes.
   * When these are filled in, the faults are needed.  A list
   * of FAultLineStruct structures is created as needed for the 
   * null filling.
   */
    faults = NULL;
    nfaults = 0;
    istat = CreateTriMeshFaults (&faults, &nfaults);
    if (istat == -1) {
        return -1;
    }

    istat =
        grd_FillNullValues (grid, ncol, nrow,
                            (CSW_F)xmin, (CSW_F)ymin, (CSW_F)xmax, (CSW_F)ymax,
                            faults, nfaults,
                            1.e19, NULL, NULL);

    if (istat == -1) {
        grd_fault_ptr->grd_free_fault_line_structs (faults, nfaults);
        return -1;
    }

    if (do_write == 1) {
        grd_WriteXYZGridFile ("AfterNullFill.xyz",
                              grid, ncol, nrow,
                              1,
                              xmin, ymin, xmax, ymax);
    }        

/*
 * Smooth the grid, honoring faults if needed.
 */
    istat =
      grd_calc_ptr->grd_smooth_grid (
        grid, ncol, nrow, sfact,
        faults, nfaults,
        (CSW_F)xmin, (CSW_F)ymin, (CSW_F)xmax, (CSW_F)ymax,
        -1.e30f, 1.e30f, NULL);
    if (istat == -1) {
        grd_fault_ptr->grd_free_fault_line_structs (faults, nfaults);
        return -1;
    }

    if (do_write == 1) {
        grd_WriteXYZGridFile ("AfterSmooth.xyz",
                              grid, ncol, nrow,
                              1,
                              xmin, ymin, xmax, ymax);
    }        

/*
 * Interpolate the smoothed grid at the trimesh nodes.
 */
    fxa = (CSW_F *)csw_Malloc (3 * NumNodes * sizeof(CSW_F));
    if (fxa == NULL) {
        return -1;
    }
    fya = fxa + NumNodes;
    fza = fya + NumNodes;
    ia = (int *)csw_Malloc (NumNodes * sizeof(int));
    if (ia == NULL) {
        return -1;
    }

    zavg = 0.0;
    navg = 0;
    na = 0;
    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        if (nptr->deleted == 1) continue;
        nptr->zorig = nptr->z;
        fx = (CSW_F)nptr->x;
        fy = (CSW_F)nptr->y;
        fxa[na] = fx;
        fya[na] = fy;
        ia[na] = i;
        na++;
    }

    istat =
      grd_arith_ptr->grd_back_interpolate (
        grid, ncol, nrow,
        (CSW_F)xmin, (CSW_F)ymin, (CSW_F)xmax, (CSW_F)ymax,
        faults, nfaults,
        fxa, fya, fza, na,
        GRD_BILINEAR);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<na; i++) {
        nptr = NodeList + ia[i];
        nptr->z = fza[i];
        zavg += fza[i];
        navg++;
    }
    
    if (navg > 0) {
        zavg /= navg;
    }

    grd_fault_ptr->grd_free_fault_line_structs (faults, nfaults);
    faults = NULL;
    nfaults = 0;

    FlagEdgeNonConstraintNodes ();

/*
 * Set the z values of the trimesh nodes on fault edges to
 * 1.e30 and also those nodes topologically adjacent.
 */
    nfedge = 0;
    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->deleted == 1) continue;
        if (eptr->tri2 >= 0) continue;
        if (eptr->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT  ||
            eptr->flag == GRD_ZERO_DISCONTINUITY_CONSTRAINT  ||
            eptr->flag == 0) {
            continue;
        }
        nptr = NodeList + eptr->node1;
        nptr->z = 1.e30;
        neptr = NodeEdgeList + eptr->node1;
        list = neptr->list;
        nlist = neptr->nlist;
        for (j=0; j<nlist; j++) {
            ep2 = EdgeList + list[j];
            if (ep2->deleted == 1) continue;
            nptr = NodeList + ep2->node1;
            if (nptr->on_border == 0) {
                nptr->z = 1.e30;
            }
            nptr = NodeList + ep2->node2;
            if (nptr->on_border == 0) {
                nptr->z = 1.e30;
            }
        }
        nptr = NodeList + eptr->node2;
        nptr->z = 1.e30;
        neptr = NodeEdgeList + eptr->node2;
        list = neptr->list;
        nlist = neptr->nlist;
        for (j=0; j<nlist; j++) {
            ep2 = EdgeList + list[j];
            if (ep2->deleted == 1) continue;
            nptr = NodeList + ep2->node1;
            if (nptr->on_border == 0) {
                nptr->z = 1.e30;
            }
            nptr = NodeList + ep2->node2;
            if (nptr->on_border == 0) {
                nptr->z = 1.e30;
            }
        }
        nfedge++;
    }

    CleanupTriNormals ();
    CleanupNodeNormals ();

    do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_WriteTextTriMeshFile (
            0, NULL,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            (char *)"NullNearFault.tri");
    }

/*
 * Fill in the null node z values.  This is done by using
 * the normals at topologically adjacent nodes with valid
 * z values.
 */
    for (;;) {

        double xtri[3], ytri[3], ztri[3];
        TRiangleStruct    *tptr;
        double            zt, nzt, zt0;

        if (nfedge == 0) break;

    /*
     * Make sure all triangles with non null corner elevations
     * have valid normals.
     */
        for (i=0; i<NumTriangles; i++) {
            tptr = TriangleList + i;
            if (tptr->deleted == 1) continue;
            TrianglePoints2 (
                tptr, NodeList, EdgeList,
                xtri, ytri, ztri);
            if (ztri[0] > 1.e20  ||
                ztri[1] > 1.e20  ||
                ztri[2] > 1.e20) {
                continue;
            }
            if (tptr->norm == NULL) {
                TriangleNormal (xtri, ytri, ztri);
                tptr->norm = (TRiNormStruct *)csw_Calloc (sizeof(TRiNormStruct));
                if (tptr->norm == NULL) {
                    return -1;
                }
                tptr->norm->nx = tnxNorm;
                tptr->norm->ny = tnyNorm;
                tptr->norm->nz = tnzNorm;
                tptr->norm->count = 1;
            }
        }

    /*
     * Make sure the node normals reflect the current triangle normals.
     */
        istat =
          CalcNodeNormals ();
        if (istat == -1) {
            return -1;
        }

    /*
     * Calculate the z values where they are null.
     */
        ndone = 0;
        for (i=0; i<NumNodes; i++) {
            nptr = NodeList + i;
            if (nptr->z < 1.e20) continue;
            neptr = NodeEdgeList + i;
            list = neptr->list;
            nlist = neptr->nlist;
            zt = 0.0;
            nzt = 0.0;
            for (j=0; j<nlist; j++) {
                ep2 = EdgeList + list[j];
                if (ep2->deleted == 1) continue;
                if (ep2->node1 == i) {
                    np2 = NodeList + ep2->node2;
                    dbnodelist[j] = ep2->node2;
                }
                else {
                    np2 = NodeList + ep2->node1;
                    dbnodelist[j] = ep2->node1;
                }
                if (np2->norm == NULL) continue;
                if (np2->norm->count < 1) continue;
                zt0 = CalcZForNodeNormal (
                    nptr->x, nptr->y, np2);
                if (zt0 > 1.e20) continue;
                zt += zt0;
                nzt += 1;
            }
            if (nzt > 0) {
                nptr->z = zt / nzt;
                ndone++;
            }
            else {
                dbnodelist[0] = dbnodelist[0];
            }
        }

        if (ndone == 0) {
            break;
        }
    }

/*
 * If there are still any NULL z values, try to fill them
 * using the z values of topologically adjacent nodes.
 */
    for (;;) {

        double            zt, nzt, zt0;

        if (nfedge == 0) break;

    /*
     * Calculate the z values where they are null.
     */
        ndone = 0;
        for (i=0; i<NumNodes; i++) {
            nptr = NodeList + i;
            if (nptr->z < 1.e20) continue;
            neptr = NodeEdgeList + i;
            list = neptr->list;
            nlist = neptr->nlist;
            zt = 0.0;
            nzt = 0.0;
            for (j=0; j<nlist; j++) {
                ep2 = EdgeList + list[j];
                if (ep2->deleted == 1) continue;
                if (ep2->node1 == i) {
                    np2 = NodeList + ep2->node2;
                    dbnodelist[j] = ep2->node2;
                }
                else {
                    np2 = NodeList + ep2->node1;
                    dbnodelist[j] = ep2->node1;
                }
                zt0 = np2->z;
                if (zt0 > 1.e20) continue;
                zt += zt0;
                nzt += 1;
            }
            if (nzt > 0) {
                nptr->z = zt / nzt;
                ndone++;
            }
            else {
                dbnodelist[0] = dbnodelist[0];
            }
        }

        if (ndone == 0) {
            break;
        }
    }

/*
 * If there are still null nodes, assert.
 */
    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        if (nptr->deleted == 1) continue;
        if (nptr->z > 1.e20) {
            nptr->z = zavg;
        }
    }

    return 1;

}


/*------------------------------------------------------------------------------*/

/*
 * Calculate the normal vector to the specified triangle.
 * The normal will always have zero or positive z.  The
 * normal is scaled so that the length is 1.0.
 *
 * The components of the normal are put into the private
 * instance variables tnxNorm, tnyNorm and tnzNorm.
 */
void CSWGrdTriangle::TriangleNormal (double *x, double *y, double *z)
{
    double    x1, y1t, z1, x2, y2, z2,
              px, py, pz;
    double    dist;

    x1 = x[1] - x[0];
    y1t = y[1] - y[0];
    z1 = z[1] - z[0];
    x2 = x[2] - x[0];
    y2 = y[2] - y[0];
    z2 = z[2] - z[0];

    px = y1t * z2 - z1 * y2;
    py = z1 * x2 - x1 * z2;
    pz = x1 * y2 - y1t * x2;

    dist = px * px + py * py + pz * pz;
    dist = sqrt (dist);

    if (dist <= 1.e-30) {
        tnxNorm = 0.0;
        tnyNorm = 0.0;
        tnzNorm = 1.0;
    }
    else {
        tnxNorm = px / dist;
        tnyNorm = py / dist;
        tnzNorm = pz / dist;
    }

    if (tnzNorm < 0.0) {
        tnxNorm = -tnxNorm;
        tnyNorm = -tnyNorm;
        tnzNorm = -tnzNorm;
    }

    return;

}

/*----------------------------------------------------------------------------*/

/*
 * Return the x, y, z points for the specified triangle.
 */

void CSWGrdTriangle::TrianglePoints2 (TRiangleStruct *tptr,
                             NOdeStruct *nodes,
                             EDgeStruct *edges,
                             double *x,
                             double *y,
                             double *z)

{
    EDgeStruct         *eptr;
    NOdeStruct         *nptr;
    int                n1, n2, n3;

    eptr = edges + tptr->edge1;
    n1 = eptr->node1;
    nptr = nodes + n1;
    x[0] = nptr->x;
    y[0] = nptr->y;
    z[0] = nptr->z;
    n2 = eptr->node2;
    nptr = nodes + n2;
    x[1] = nptr->x;
    y[1] = nptr->y;
    z[1] = nptr->z;

    eptr = edges + tptr->edge2;
    if (eptr->node1 == n1  ||  eptr->node1 == n2) {
        n3 = eptr->node2;
    }
    else {
        n3 = eptr->node1;
    }

    nptr = nodes + n3;
    x[2] = nptr->x;
    y[2] = nptr->y;
    z[2] = nptr->z;

    return;

}  /* end of private TrianglePoints2 function */

/*------------------------------------------------------------------------------*/

void CSWGrdTriangle::CleanupTriNormals (void)
{
    int             i;
    TRiangleStruct  *tptr;

    for (i=0; i<NumTriangles; i++) {
        tptr = TriangleList + i;
        csw_Free (tptr->norm);
        tptr->norm = NULL;
    }
}

/*------------------------------------------------------------------------------*/

void CSWGrdTriangle::CleanupNodeNormals (void)
{
    int             i;
    NOdeStruct      *nptr;

    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        csw_Free (nptr->norm);
        nptr->norm = NULL;
    }
}


/*--------------------------------------------------------------------------------*/

/*
 * Calculate the average normal at each node.  The average is just
 * the arithmetic mean of the normals of triangle using the node.
 */

int CSWGrdTriangle::CalcNodeNormals (void)
{
    int              i, n1, n2, n3,
                     count;
    TRiNormStruct    *norm, *norm2;
    TRiangleStruct   *tptr;
    NOdeStruct       *nptr;

/*
 * Make sure the node normals are cleaned up.
 */
    CleanupNodeNormals ();

/*
 * Sum the count and normals arrays whenever a node
 * is used in a triangle.
 */
    for (i=0; i<NumTriangles; i++) {

        tptr = TriangleList + i;
        if (tptr->deleted == 1) continue;
        norm = tptr->norm;
        if (norm == NULL) continue;

        grd_get_nodes_for_triangle (
            tptr, EdgeList,
            &n1, &n2, &n3);
        if (n1 < 0  ||  n2 < 0  ||  n3 < 0) {
            assert (0);
        }

        nptr = NodeList + n1;
        norm2 = nptr->norm;
        if (norm2 == NULL) {
            norm2 = (TRiNormStruct *)csw_Calloc (sizeof(TRiNormStruct));
        }
        if (norm2 == NULL) {
            CleanupNodeNormals ();
            return -1;
        }
        nptr->norm = norm2;
        norm2->nx += norm->nx;
        norm2->ny += norm->ny;
        norm2->nz += norm->nz;
        norm2->count++;

        nptr = NodeList + n2;
        norm2 = nptr->norm;
        if (norm2 == NULL) {
            norm2 = (TRiNormStruct *)csw_Calloc (sizeof(TRiNormStruct));
        }
        if (norm2 == NULL) {
            CleanupNodeNormals ();
            return -1;
        }
        nptr->norm = norm2;
        norm2->nx += norm->nx;
        norm2->ny += norm->ny;
        norm2->nz += norm->nz;
        norm2->count++;

        nptr = NodeList + n3;
        norm2 = nptr->norm;
        if (norm2 == NULL) {
            norm2 = (TRiNormStruct *)csw_Calloc (sizeof(TRiNormStruct));
        }
        if (norm2 == NULL) {
            CleanupNodeNormals ();
            return -1;
        }
        nptr->norm = norm2;
        norm2->nx += norm->nx;
        norm2->ny += norm->ny;
        norm2->nz += norm->nz;
        norm2->count++;

    }

/*
 * Calculate averages at each node that is used in
 * at least one triangle.
 */
    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        if (nptr->deleted == 1) continue;
        if (nptr->norm == NULL) {
            nptr->norm = (TRiNormStruct *)csw_Calloc (sizeof(TRiNormStruct));
            if (nptr->norm == NULL) {
                CleanupNodeNormals ();
                return -1;
            }
            nptr->norm->nz = 1.0;
        }
        norm = nptr->norm;
        if (norm->count > 0) {
            count = norm->count;
            norm->nx /= count;
            norm->ny /= count;
            norm->nz /= count;
        }
        else {
            norm->nx = 0.0;
            norm->ny = 0.0;
            norm->nz = 1.0;
        }
    }

    return 1;

}



/*-----------------------------------------------------------------------------*/

int CSWGrdTriangle::grd_calc_trimesh_node_normals (
    NOdeStruct      *nodes,
    int             num_nodes,
    EDgeStruct      *edges,
    int             num_edges,
    TRiangleStruct  *tris,
    int             num_tris)
{
    int             i, istat;
    double          xtri[3], ytri[3], ztri[3];
    TRiangleStruct  *tptr;

    NodeList = nodes;
    EdgeList = edges;
    TriangleList = tris;
    NumNodes = num_nodes;
    NumEdges = num_edges;
    NumTriangles = num_tris;

/*
 * Make sure all triangles with non null corner elevations
 * have valid normals.
 */
    for (i=0; i<NumTriangles; i++) {
        tptr = TriangleList + i;
        if (tptr->deleted == 1) continue;
        TrianglePoints2 (
            tptr, NodeList, EdgeList,
            xtri, ytri, ztri);
        if (ztri[0] > 1.e20  ||
            ztri[1] > 1.e20  ||
            ztri[2] > 1.e20) {
            continue;
        }
        if (tptr->norm == NULL) {
            TriangleNormal (xtri, ytri, ztri);
            tptr->norm = (TRiNormStruct *)csw_Calloc (sizeof(TRiNormStruct));
            if (tptr->norm == NULL) {
                CleanupTriNormals ();
                CleanupNodeNormals ();
                return -1;
            }
            tptr->norm->nx = tnxNorm;
            tptr->norm->ny = tnyNorm;
            tptr->norm->nz = tnzNorm;
            tptr->norm->count = 1;
        }
    }

/*
 * Make sure the node normals reflect the current triangle normals.
 */
    istat =
      CalcNodeNormals ();
    if (istat == -1) {
        CleanupTriNormals ();
        CleanupNodeNormals ();
        return -1;
    }

    ListNull ();
    FreeMem ();

    return 1;

}


/*--------------------------------------------------------------------------*/

int CSWGrdTriangle::grd_free_trimesh_normals (
    NOdeStruct      *nodes,
    int             num_nodes,
    EDgeStruct      *edges,
    int             num_edges,
    TRiangleStruct  *tris,
    int             num_tris)
{

    NodeList = nodes;
    EdgeList = edges;
    TriangleList = tris;
    NumNodes = num_nodes;
    NumEdges = num_edges;
    NumTriangles = num_tris;

    CleanupTriNormals ();
    CleanupNodeNormals ();

    ListNull ();
    FreeMem ();

    return 1;

}

/*-----------------------------------------------------------------------------*/

double CSWGrdTriangle::grd_calc_z_for_triangle_normal (
    double          x,
    double          y,
    TRiangleStruct  *tptr)
{
    double          zval;

    if (NodeList == NULL  ||  EdgeList == NULL  ||
        TriangleList == NULL) {
        return 1.e30;
    }

    zval = CalcZForTriangleNormal (x, y, tptr);

    return zval;
}


/*-----------------------------------------------------------------------------*/

double CSWGrdTriangle::grd_calc_z_for_edge_normal (
    double          x,
    double          y,
    EDgeStruct      *eptr)
{
    double          zval;

    if (NodeList == NULL  ||  EdgeList == NULL  ||
        TriangleList == NULL) {
        return 1.e30;
    }

    zval = CalcZForEdgeNormal (x, y, eptr);

    return zval;
}


/*-----------------------------------------------------------------------------*/

double CSWGrdTriangle::grd_calc_z_for_node_normal (
    double          x,
    double          y,
    NOdeStruct      *nptr)
{
    double          zval;

    if (NodeList == NULL  ||  EdgeList == NULL  ||
        TriangleList == NULL) {
        return 1.e30;
    }

    zval = CalcZForNodeNormal (x, y, nptr);

    return zval;
}


/*-----------------------------------------------------------------------------*/


void CSWGrdTriangle::CorrectBorderEdgeForClippedConstraint (int rpnum)
{
    int              i, jmin, n1, n2, istat, edgenum;
    RAwPointStruct   *rptr;
    double           xr, yr, xint, yint;
    double           dmin, dist, x1, y1, x2, y2;
    EDgeStruct       *eptr;

    edgenum = RawPointBorderEdgeIntersect (rpnum, &xint, &yint);
    if (edgenum == -1) {
        return;
    }

    dmin = 1.e30;
    eptr = EdgeList + edgenum;
    jmin = -1;

    for (i=MaxIndexPoint; i<NumRawPoints; i++) {

        if (i == rpnum) continue;

        rptr = RawPoints + i;
        if (rptr->nodenum < 0) {
            continue;
        }
        xr = rptr->x;
        yr = rptr->y;

        n1 = eptr->node1;
        n2 = eptr->node2;
        x1 = NodeList[n1].x;
        y1 = NodeList[n1].y;
        x2 = NodeList[n2].x;
        y2 = NodeList[n2].y;

        istat = gpf_perpdistance2 (
            x1, y1, x2, y2, xr, yr, &dist);
        if (istat == 0) {
            continue;
        }

        if (dist < dmin) {
            dmin = dist;
            jmin = i;
        }

    }

    if (jmin == -1) {
        return;
    }

    if (RawPoints[jmin].endflag != 0) {
        return;
    }

    n1 = RawPoints[jmin].nodenum;
    if (NodeList[n1].on_border == 1) {
        return;
    }

    WhackEdge (edgenum);

    return;

}


int CSWGrdTriangle::RawPointBorderEdgeIntersect (
    int         rpnum,
    double      *xintout,
    double      *yintout)
{
    int         i, j, jedge, istat;
    double      dx, dy, dist, dmin,
                x0, y0, x1, y1, x2, y2, x3, y3, x4, y4,
                xint, yint, xintsav, yintsav;
    RAwPointStruct    *rp1, *rp2;
    RAwLineSegStruct  *rline;
    EDgeStruct        *eptr;
    NOdeStruct        *nptr;

    CSWPolyUtils             ply_utils_obj;

    x0 = RawPoints[rpnum].x;
    y0 = RawPoints[rpnum].y;

    jedge = -1;
    dmin = 1.e30;
    *xintout = 1.e30;
    *yintout = 1.e30;
    xintsav = 1.e30;
    yintsav = 1.e30;

/*
 * In the outer loop, find constraint segments with rpnum as one of the
 * segment endpoints.
 */
    for (i=0; i<NumRawLines; i++) {

        rline = RawLines + i;
        if (!(rline->rp1 == rpnum  ||  rline->rp2 == rpnum)) {
            continue;
        }

        rp1 = RawPoints + rline->rp1;
        rp2 = RawPoints + rline->rp2;
        x1 = rp1->x;
        y1 = rp1->y;
        x2 = rp2->x;
        y2 = rp2->y;

    /*
     * Inner loop, find the border node that has the closest
     * intersection point with the outer loops constraint segment.
     */
        for (j=0; j<NumEdges; j++) {

            eptr = EdgeList + j;
            if (eptr->on_border == 0) {
                continue;
            }

            nptr = NodeList + eptr->node1;
            x3 = nptr->x;
            y3 = nptr->y;
            nptr = NodeList + eptr->node2;
            x4 = nptr->x;
            y4 = nptr->y;

            istat =
              ply_utils_obj.ply_segint (x1, y1, x2, y2, x3, y3, x4, y4, &xint, &yint);
            if (istat == 0) {
                dx = x0 - xint;
                dy = y0 - yint;
                dist = dx * dx + dy * dy;
                dist = sqrt (dist);
                if (dist < dmin) {
                    dmin = dist;
                    jedge = j;
                    xintsav = xint;
                    yintsav = yint;
                }
            }
        }

    }

    if (jedge >= 0) {
        *xintout = xintsav;
        *yintout = yintsav;
    }

    return jedge;

}


/*-----------------------------------------------------------------------------------------*/

/*
 * Group of methods dealing with organizing crossing discontinuity constraints.
 * The constraints of this type are unsplit if needed, and then they are resplit
 * so that two split line endpoints do not coincide.  When the surface discontinuties
 * are calculated, all unsplit lines are done first.  After that, lines split by
 * these unsplit lines are done, and the process is repeated for whatever levels
 * of split line nesting are needed.
 */

void CSWGrdTriangle::grd_set_organize_lines_flag (int ival)
{
    OrganizeLinesFlag = ival;
}


/*------------------------------------------------------------------------------------------*/

int CSWGrdTriangle::OrganizeConstraintLines (double **xlinesio,
                                    double **ylinesio,
                                    double **zlinesio,
                                    int    **lplinesio,
                                    int    **lflinesio,
                                    int    *nlinesio)
{
    double            *xw = NULL, *yw = NULL, *zw = NULL,
                      *xw2 = NULL, *yw2 = NULL, *zw2 = NULL;
    double            *xtp = NULL, *ytp = NULL, *ztp = NULL,
                      *xtp2 = NULL, *ytp2 = NULL, *ztp2 = NULL;
    int               *lpw = NULL, *lfw = NULL, nlw,
                      *lpw2 = NULL, *lfw2 = NULL, nlw2;
    int               istat, i, ntot, npmax, nlmax, iflag, npts;

    double            *xlines = NULL, *ylines = NULL, *zlines = NULL;
    int               *lplines = NULL, *lflines = NULL, nlines;

    double            *xout = NULL, *yout = NULL, *zout = NULL;
    int               *npout = NULL, *nfout = NULL, n, nout, nlout;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (xw);
        csw_Free (lpw);
        csw_Free (lplines);

        if (bsuccess == false) {
            csw_Free (xout);
            csw_Free (npout);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (OrganizeLinesFlag == 0) {
        return 1;
    }

    xlines = *xlinesio;
    ylines = *ylinesio;
    zlines = *zlinesio;
    lplines = *lplinesio;
    lflines = *lflinesio;
    nlines = *nlinesio;

/*
 * Do nothing if there is only one constraint line.
 */
    if (nlines < 2) {
        return 1;
    }

/*
 * Allocate space for separate discontinuity constraints
 * and non discontinuity constraints.
 */
    ntot = 0;
    for (i=0; i<nlines; i++) {
        ntot += lplines[0];
    }

    xw = (double *)csw_Malloc (ntot * 12 * sizeof(double));
    if (xw == NULL) {
        return -1;
    }
    yw = xw + ntot * 3;
    zw = yw + ntot * 3;
    xw2 = zw + ntot * 3;
    yw2 = xw2 + ntot;
    zw2 = yw2 + ntot;

    npmax = ntot * 3;
    nlmax = nlines * nlines;
    if (nlmax > nlines * 100) nlmax = nlines * 100;
    if (nlmax < 100) nlmax = 100;

    lpw = (int *)csw_Malloc (nlmax * 4 * sizeof(int));
    if (lpw == NULL) {
        return -1;
    }
    lfw = lpw + nlmax;
    lpw2 = lfw + nlmax;
    lfw2 = lpw2 + nlmax;

/*
 * Separate the constraints into discontinuity constraints (xw, yw, zw)
 * and non discontinuity constraints (xw2, yw2, zw2).
 */
    nlw = nlw2 = 0;
    xtp = xw;
    ytp = yw;
    ztp = zw;
    xtp2 = xw2;
    ytp2 = yw2;
    ztp2 = zw2;
    ntot = 0;

    for (i=0; i<nlines; i++) {
        iflag = lflines[i];
        npts = lplines[i];
        if (iflag == GRD_DISCONTINUITY_CONSTRAINT  ||
            iflag == GRD_ZERO_DISCONTINUITY_CONSTRAINT  ||
            iflag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
            iflag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT) {
            lpw[nlw] = npts;
            lfw[nlw] = iflag;
            memcpy (xtp, xlines+ntot, npts*sizeof(double));
            memcpy (ytp, ylines+ntot, npts*sizeof(double));
            memcpy (ztp, zlines+ntot, npts*sizeof(double));
            xtp += npts;
            ytp += npts;
            ztp += npts;
            nlw++;
        }
        else {
            lpw2[nlw2] = npts;
            lfw2[nlw2] = iflag;
            memcpy (xtp2, xlines+ntot, npts*sizeof(double));
            memcpy (ytp2, ylines+ntot, npts*sizeof(double));
            memcpy (ztp2, zlines+ntot, npts*sizeof(double));
            xtp2 += npts;
            ytp2 += npts;
            ztp2 += npts;
            nlw2++;
        }
        ntot += npts;
    }

/*
 * Concatenate the discontinuity lines back together if needed.
 */
    istat =
      ConcatLines (xw, yw, zw, lpw, lfw, &nlw);
    if (istat == -1) {
      return -1;
    }

/*
 * Organize the crossing lines so that the topology can be split
 * along them.
 */
    istat =
      OrganizeCrossingLines (xw, yw, zw, lpw, lfw, &nlw, npmax, nlmax);
    if (istat == -1) {
      return -1;
    }

/*
 * Combine non discontinuity and organized discontinuity lines
 * back into the complete constraint lines arrays.
 */
    npmax += ntot;
    nlmax += nlines;

    xout = (double *)csw_Malloc (3 * npmax * sizeof(double));
    if (xout == NULL) {
        return -1;
    }
    yout = xout + npmax;
    zout = yout + npmax;

    npout = (int *)csw_Calloc (2 * nlmax * sizeof(int));
    if (npout == NULL) {
        return -1;
    }
    nfout = npout + nlmax;

    nout = 0;
    nlout = 0;
    n = 0;

    for (i=0; i<nlw2; i++) {
        npout[nlout] = lpw2[i];
        nfout[nlout] = lfw2[i];
        memcpy (xout+nout, xw2+n, lpw2[i] * sizeof(double));
        memcpy (yout+nout, yw2+n, lpw2[i] * sizeof(double));
        memcpy (zout+nout, zw2+n, lpw2[i] * sizeof(double));
        nout += lpw2[i];
        nlout++;
        n += lpw2[i];
    }

    n = 0;
    for (i=0; i<nlw; i++) {
        npout[nlout] = lpw[i];
        nfout[nlout] = lfw[i];
        memcpy (xout+nout, xw+n, lpw[i] * sizeof(double));
        memcpy (yout+nout, yw+n, lpw[i] * sizeof(double));
        memcpy (zout+nout, zw+n, lpw[i] * sizeof(double));
        nout += lpw[i];
        nlout++;
        n += lpw[i];
    }

    *xlinesio = xout;
    *ylinesio = yout;
    *zlinesio = zout;
    *lplinesio = npout;
    *lflinesio = nfout;
    *nlinesio = nlout;

    bsuccess = true;

    return 1;

}

/*------------------------------------------------------------------------------------------*/

int CSWGrdTriangle::ConcatLines (double *xlines,
                        double *ylines,
                        double *zlines,
                        int    *lplines,
                        int    *lflines,
                        int    *nlines)
{
    int istat;

    istat =
      gpf_concat_lines_xy (
        xlines, ylines, zlines,
        lplines, lflines, nlines,
        XYTiny);

    return istat;
}

/*------------------------------------------------------------------------------------------*/

int CSWGrdTriangle::OrganizeCrossingLines (double *xlines,
                                  double *ylines,
                                  double *zlines,
                                  int    *lplines,
                                  int    *lflines,
                                  int    *nlines,
                                  int    npmax,
                                  int    nlmax)
{
    int    istat;

    istat =
      gpf_organize_crossing_lines_xy (
        xlines, ylines, zlines,
        lplines, lflines, nlines,
        npmax, nlmax,
        XYTiny);

    return istat;

}


/*-------------------------------------------------------------------------------------------*/

void CSWGrdTriangle::grd_validate_topology (
    NOdeStruct      *nodes,
    int             numnodes,
    EDgeStruct      *edges,
    int             numedges,
    TRiangleStruct  *tris,
    int             numtris,
    char            *msg)
{
    NOdeStruct      *nsav;
    EDgeStruct      *esav;
    TRiangleStruct  *tsav;
    int             nnsav, nesav, ntsav;

    nsav = NodeList;
    esav = EdgeList;
    tsav = TriangleList;
    nnsav = NumNodes;
    nesav = NumEdges;
    ntsav = NumTriangles;

    NodeList = nodes;
    EdgeList = edges;
    TriangleList = tris;
    NumNodes = numnodes;
    NumEdges = numedges;
    NumTriangles = numtris;

    ForceValidate = 1;
    ValidateTriangles (msg);
    ValidateEdgeIntersection (msg);
    ValidateTriangleShapes ();
    ForceValidate = 0;

    NodeList = nsav;
    EdgeList = esav;
    TriangleList = tsav;
    NumNodes = nnsav;
    NumEdges = nesav;
    NumTriangles = ntsav;

    return;

}


/*
 ***********************************************************************************

         C r e a t e T r i M e s h F a u l t s

 ***********************************************************************************

 Scan the current trimesh (NodeList, EdgeList) for edges that are fault
 constraints.  Each edge is made into a separate FAultLineStruct if any is
 found.

*/

int CSWGrdTriangle::CreateTriMeshFaults (FAultLineStruct **faultsout,
                                 int             *nfaultsout)
{
    int          i, j, maxfault, nfault, n1, n2;
    EDgeStruct   *ep = NULL, *epair = NULL;
    FAultLineStruct   *faults = NULL;
    double       *xfa = NULL, *yfa = NULL;
    int          *ifa = NULL;


    auto fscope = [&]()
    {
        csw_Free (xfa);
        csw_Free (ifa);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of error.
 */
    *faultsout = NULL;
    *nfaultsout = 0;

/*
 * tflag2 is used to keep a paired edge from being added twice.
 * initialize it to zero.
 */
    for (i=0; i<NumEdges; i++) {
        EdgeList[i].tflag2 = 0;
    }

/*
 * Count the maximum number of possible faults.
 */
    maxfault = 0;
    nfault = 0;
    for (i=0; i<NumEdges; i++) {
        if (EdgeList[i].flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
            EdgeList[i].flag == GRD_DISCONTINUITY_CONSTRAINT) {
            maxfault++;
        }
    }

/*
 * If no edges are fault constraints, this is not a faulted trimesh.
 */
    if (maxfault < 1) {
        return 1;
    }

/*
 * Allocate arrays for fault lines.
 */
    xfa = (double *)csw_Malloc (maxfault * 4 * sizeof(double));
    if (xfa == NULL) {
        return -1;
    }
    yfa = xfa + maxfault * 2;

    ifa = (int *)csw_Malloc (maxfault * sizeof(int));
    if (ifa == NULL) {
        return -1;
    }

/*
 * Add the endpoints of each constraint edge to the fault point arrays.
 */
    int n = 0;
    for (i=0; i<NumEdges; i++) {
        ep = EdgeList + i;
        if (ep->deleted) {
            continue;
        }
        if (ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
            ep->flag == GRD_DISCONTINUITY_CONSTRAINT) {
            j = ep->pairid;
            if (j > 0) {
                epair = EdgeList + (j - 1);
                if (epair->tflag2 == 1) {
                    continue;
                }
                epair->tflag2 = 1;
            }

            n1 = ep->node1;
            n2 = ep->node2;

            ifa[nfault] = 2;
            xfa[n] = NodeList[n1].x;
            yfa[n] = NodeList[n1].y;
            n++;
            xfa[n] = NodeList[n2].x;
            yfa[n] = NodeList[n2].y;
            n++;
            nfault++;
        }
    }

    if (nfault < 1) {
        return 1;
    }

/*
 * Convert the fault point arrays to fault line structs.
 */
    int nflist = 0;
    int istat = grd_fault_ptr->grd_fault_arrays_to_structs_2 (xfa, yfa, NULL,
                                               ifa, NULL,
                                               nfault,
                                               &faults, &nflist);

    if (istat == -1) {
        return -1;
    }

/*
 * If no fault lines are available from the conversion, this is
 * not a valid faulted trimesh.
 */
    if (nflist < 1  ||  faults == NULL) {
        return 1;
    }

    *faultsout = faults;
    *nfaultsout = nflist;
            
    return 1;
}



int CSWGrdTriangle::grd_smooth_trimesh_nodes_directly (
    NOdeStruct     *nodes,
    int            numnodes,
    EDgeStruct     *edges,
    int            numedges,
    TRiangleStruct *tris,
    int            numtris,
    int            sfactin,
    double         avlen)

{
    int            i, istat, sfsave;
    double         sfact;

    sfsave = sfactin;
    if (sfactin > 100) sfactin -= 100;
    sfact = (double)sfactin / 6.0;
    sfact *= sfact;
    if (sfactin > 6) {
        sfact *= sfact;
    }
    sfactin = sfsave;

    FreeMem ();

    NodeList = nodes;
    NumNodes = numnodes;
    EdgeList = edges;
    NumEdges = numedges;
    TriangleList = tris;
    NumTriangles = numtris;

/*
 * Build lists of edges connected to each node.
 */
    istat =
      BuildNodeEdgeLists ();
    if (istat == -1) {
        ListNull ();
        FreeMem ();
        return -1;
    }

    if (NodeEdgeList == NULL) {
        ListNull ();
        FreeMem ();
        return -1;
    }

/*
 * Put the node z values for input to smoothing into the zorig
 * members, the output will be put into the z members.
 */
    for (i=0; i<NumNodes; i++) {
        NodeList[i].zorig = NodeList[i].z;
    }

    /*
    istat =
      SmoothTrimeshDirectly (sfactin, avlen);
      */
    istat =
      SmoothTrimeshUsingGrid (sfactin, avlen);

    ListNull ();
    FreeMem ();

    return istat;

}

#if 0
int CSWGrdTriangle::SmoothTrimeshDirectly (int sfact, double avlen)
{
    double               sum1, sum2, x0, y0, z0, xt, yt, zt,
                         dx, dy, dist, tiny, wgt, dsfact;
    int                  i, j, n1;
    NOdeEdgeListStruct   *neptr;
    EDgeStruct           *eptr;
    int                  *list, nlist;

    avlen *= 1.5;
    avlen *= avlen;
    tiny = avlen / 1000.0;

    dsfact = (double)sfact;
    if (sfact >= 100) {
        sfact -= 100;
        dsfact = (double)sfact / 3.0;
    }

    for (i=0; i<NumNodes; i++) {

        if (NodeList[i].deleted) {
            continue;
        }

        neptr = NodeEdgeList + i;
        list = neptr->list;
        nlist = neptr->nlist;
        x0 = NodeList[i].x;
        y0 = NodeList[i].y;
        z0 = NodeList[i].zorig;
        sum1 = 0.0;
        sum2 = 0.0;

        for (j=0; j<nlist; j++) {

            eptr = EdgeList + list[j];
            if (eptr->deleted) {
                continue;
            }

            n1 = eptr->node1;
            if (n1 == i) {
                n1 = eptr->node2;
            }
            xt = NodeList[n1].x;
            yt = NodeList[n1].y;
            zt = NodeList[n1].zorig;
            dx = xt - x0;
            dy = yt - y0;
            dist = dx * dx + dy * dy;
            if (dist < avlen) {
                sum1 += zt;
                sum2 += 1.0;
            }
            else {
                wgt = avlen / dist;
                sum1 += zt * wgt;
                sum2 += wgt;
            }
        }

        if (sum2 > tiny) {
            zt = sum1 / sum2;
            zt = (zt * dsfact + z0) / (dsfact + 1.0);
            NodeList[i].z = zt;
        }

    }

    return 1;
}
#endif


/*
 * If an intersection between a constraint segment and another triangle
 * edge cannot be found, it may be because the constraint segment is
 * outside of the original unconstrained trimesh.  If the specified nodes 
 * have exactly one connected edge which shares the same end node, then
 * the edge between node1 and node2 can be created and a new triangle 
 * can be created.  If there is no "common opposite node" then no new 
 * triangle can be created.
 *
 * The return status is 1 if a new triangle has been created.  The return
 * status is zero if no new triangle can be created.  If a memory allocation
 * error occurs while attempting to add hte new triangle, a return status
 * of -1 is returned.
 */
int CSWGrdTriangle::CreateNewTriangleForConstraint (int node1, int node2)
{
    int            list1[MAXLIST], list2[MAXLIST];
    int            nlist1, nlist2, nodecommon;
    int            *redge, nedge, maxedge;
    int            i, j, n1, n2, ne1, ne2, ncommon, enew, tnew;
    EDgeStruct     *ep1, *ep2;
    NOdeStruct     *np1, *np2;
    RAwPointStruct *rp1, *rp2;

/*
 * Find the raw points for the nodes.
 */
    np1 = NodeList + node1;
    np2 = NodeList + node2;
    if (np1->rp >= 0) {
        rp1 = RawPoints + np1->rp;
    }
    else if (np1->crp >= 0) {
        rp1 = ConstraintRawPoints + np1->crp;
    }
    else {
        return 0;
    }
    if (np2->rp >= 0) {
        rp2 = RawPoints + np2->rp;
    }
    else if (np2->crp >= 0) {
        rp2 = ConstraintRawPoints + np2->crp;
    }
    else {
        return 0;
    }

/*
 * Find edges attached to node1 and node2.
 */
    nlist1 = 0;
    nlist2 = 0;
    for (i=0; i<NumEdges; i++) {
        ep1 = EdgeList + i;
        if (ep1->deleted) {
            continue;
        }
        if (ep1->node1 == node1  ||  ep1->node2 == node1) {
            list1[nlist1] = i;
            nlist1++;
            if (nlist1 >= MAXLIST) {
                return 0;
            }
        }
        if (ep1->node1 == node2  ||  ep1->node2 == node2) {
            list2[nlist2] = i;
            nlist2++;
            if (nlist2 >= MAXLIST) {
                return 0;
            }
        }
    }

    if (nlist1 < 1  ||  nlist2 < 1) {
        return 0;
    }

/*
 * Find pairs of edges, one attached to each node, that have
 * the same opposite node.  If exactly one such pair is found,
 * a new triangle can be created.
 */
    ncommon = 0;
    nodecommon = -1;
    for (i=0; i<nlist1; i++) {
        ep1 = EdgeList + list1[i];
        n1 = ep1->node1;
        if (n1 == node1) {
            n1 = ep1->node2;
        }
        for (j=0; j<nlist2; j++) {
            ep2 = EdgeList + list2[j];
            n2 = ep2->node1;
            if (n2 == node2) {
                n2 = ep2->node2;
            }
            if (n1 == n2) {
                ne1 = list1[i];
                ne2 = list2[j];
                ncommon++;
                nodecommon = n1;
            }
        }
    }

    if (ncommon != 1  ||  ne2 == ne1  ||  nodecommon == -1) {
        return 0;
    }

    NodeList[nodecommon].on_border = 0;

/*
 * Create a new edge and triangle.
 */
    enew = AddEdge (node1, node2, -1, -1, BOUNDARY_EDGE);
    if (enew < 0) {
        return -1;
    }

    tnew = AddTriangle (ne1, ne2, enew, 0);
    if (tnew < 0) {
        return 0;
    }

    ep1 = EdgeList + ne1;
    ep1->tri2 = tnew;
    ep1->flag = 0;
    ep1->on_border = 0;
    ep1 = EdgeList + ne2;
    ep1->tri2 = tnew;
    ep1->flag = 0;
    ep1->on_border = 0;
    ep1 = EdgeList + enew;
    ep1->tri1 = tnew;
    ep1->flag = GRD_TRIMESH_FAULT_CONSTRAINT;
    ep1->on_border = 1;

/*
 * Add the new edge to the raw point edge lists.
 */
    redge = rp1->edgelist;
    nedge = rp1->nedge;
    maxedge = rp1->maxedge;
    if (nedge >= maxedge) {
        maxedge += 2;
        redge = (int *)csw_Realloc (redge, maxedge * sizeof(int));
    }
    redge[nedge] = enew;
    nedge++;
    rp1->edgelist = redge;
    rp1->nedge = nedge;
    rp1->maxedge = maxedge;

    redge = rp2->edgelist;
    nedge = rp2->nedge;
    maxedge = rp2->maxedge;
    if (nedge >= maxedge) {
        maxedge += 2;
        redge = (int *)csw_Realloc (redge, maxedge * sizeof(int));
    }
    redge[nedge] = enew;
    nedge++;
    rp2->edgelist = redge;
    rp2->nedge = nedge;
    rp2->maxedge = maxedge;

    return 1;
}



/*
 ************************************************************************************

                g r d _ W r i t e T e x t T r i M e s h F i l e

 ************************************************************************************

*/

#define TEXT_TMESH_VERSION            "1.00"

int CSWGrdTriangle::grd_WriteTextTriMeshFile
                 (int vused,
                  double *vbasein,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename)

{
    FILE         *fptr;
    int          i;
    char         line[500];
    double       vb[6], *vbase;

    fptr = fopen (filename, "w");
    if (fptr == NULL) {
        printf ("Cannot open the new tri mesh file\n");
        return -1;
    }

    vb[0] = 1.e30;
    vb[1] = 1.e30;
    vb[2] = 1.e30;
    vb[3] = 1.e30;
    vb[4] = 1.e30;
    vb[5] = 1.e30;

    vbase = vbasein;
    if (vbase == NULL) {
        vused = 0;
        vbase = vb;
    }

    sprintf (line, "!TXT_TMESH %s\n", TEXT_TMESH_VERSION);
    fputs (line, fptr);

    sprintf (line, "%d %g %g %g %g %g %g\n",
             vused,
             vbase[0],
             vbase[1],
             vbase[2],
             vbase[3],
             vbase[4],
             vbase[5]);
    fputs (line, fptr);

    sprintf (line, "%d %d %d\n", numnodes, nedges, ntriangles);
    fputs (line, fptr);

    for (i=0; i<numnodes; i++) {
        sprintf (line, "%g %g %g %d\n",
            nodes[i].x, nodes[i].y,
            nodes[i].z, nodes[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<nedges; i++) {
        sprintf (line, "%d %d %d %d %d\n",
            edges[i].node1, edges[i].node2, edges[i].tri1,
            edges[i].tri2, edges[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<ntriangles; i++) {
        sprintf (line, "%d %d %d %d\n",
            triangles[i].edge1, triangles[i].edge2,
            triangles[i].edge3, triangles[i].flag);
        fputs (line, fptr);
    }

    fclose (fptr);

    return 1;

}

/*
 * Returns position in file where we wrote the data, or -1 on i/o error.
 */
long CSWGrdTriangle::grd_AppendTextTriMeshFile
                 (int vused,
                  double *vbase,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename)

{
    FILE         *fptr;
    int          i;
    char         line[500];
    long position = 0L;

    fptr = fopen(filename, "a");
    if (fptr == NULL) {
        printf ("Cannot open the new tri mesh file\n");
        return -1;
    }
    /* get position in file where we will be appending */
    fseek(fptr, 0L, SEEK_END);
    position = ftell(fptr);

    sprintf (line, "!TXT_TMESH %s\n", TEXT_TMESH_VERSION);
    fputs (line, fptr);

    sprintf (line, "%d %g %g %g %g %g %g\n",
             vused,
             vbase[0],
             vbase[1],
             vbase[2],
             vbase[3],
             vbase[4],
             vbase[5]);
    fputs (line, fptr);

    sprintf (line, "%d %d %d\n", numnodes, nedges, ntriangles);
    fputs (line, fptr);

    for (i=0; i<numnodes; i++) {
        sprintf (line, "%g %g %g %d\n",
            nodes[i].x, nodes[i].y,
            nodes[i].z, nodes[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<nedges; i++) {
        sprintf (line, "%d %d %d %d %d\n",
            edges[i].node1, edges[i].node2, edges[i].tri1,
            edges[i].tri2, edges[i].flag);
        fputs (line, fptr);
    }

    for (i=0; i<ntriangles; i++) {
        sprintf (line, "%d %d %d %d\n",
            triangles[i].edge1, triangles[i].edge2,
            triangles[i].edge3, triangles[i].flag);
        fputs (line, fptr);
    }

    fclose (fptr);

    return(position);
}




/*
 *************************************************************************

           g r d _ C a l c P r e c i s e D o u b l e P l a n e

 *************************************************************************

  Calculate the coefs of a plane in a "precise" manner.  The function uses
  singular value decomposition, which will handle a lot more "degenerate"
  cases than other methods.  The algorithm is quite a bit slower also,
  so only call this when degenerate cases may need to be handled.

*/

int CSWGrdTriangle::grd_CalcPreciseDoublePlane (double *x, double *y, double *z,
                         int npts, double *coef)
{
    int     istat;
    double  coef1[3];

    if (npts < 3) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

/*
 * Calculate a plane with the original coordinates.
 */
    istat = grd_tsurf_ptr->grd_calc_trend_surface (x, y, z, npts, 1, coef1);
    if (istat == -1) {
        return istat;
    }
    memcpy (coef, coef1, 3 * sizeof(double));

    return 1;

} /* end of function grd_CalcPreciseDoublePlane */



/*
 *********************************************************************************

                     g r d _ W r i t e L i n e s

 *********************************************************************************

  Write a set of x, y and (optional) z coordinates to a file as individual
  polylines.

*/

int CSWGrdTriangle::grd_WriteLines (double *x,
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
 *********************************************************************************

                     g r d _ W r i t e P o i n t s

 *********************************************************************************

  Write a set of xyz points out to the specified ascii file.  This is used
  almost exclusively for debugging purposes.

*/

int CSWGrdTriangle::grd_WritePoints (double *x,
                     double *y,
                     double *z,
                     int npts,
                     char *fname)
{
    FILE         *fptr;
    int          i;
    char         c200[200];

    if (x == NULL  ||  y == NULL  ||  z == NULL  ||
        npts < 1  ||  fname == NULL) {
        return -1;
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    for (i=0; i<npts; i++) {
        sprintf (c200, "%g %g %g\n", x[i], y[i], z[i]);
        fputs (c200, fptr);
    }

    fclose (fptr);

    return 1;

}



int CSWGrdTriangle::grd_WriteXYZGridFile (
    const char *fname,
    CSW_F   *grid,
    int     ncol,
    int     nrow,
    int     ncoarse,
    double x1, double y1, double x2, double y2)
{
    FILE     *fptr;
    int      i, j, offset, k;
    char     c50[200];
    double   dx, dy, xt, yt;

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

    dx = (x2 - x1) / (ncol - 1);
    dy = (y2 - y1) / (nrow - 1);

    for (i=0; i<nrow; i+=ncoarse) {
        fputs ("\n", fptr);
        offset = i * ncol;
        yt = y1 + i * dy;
        for (j=0; j<ncol; j+=ncoarse) {
            xt = x1 + j * dx;
            k = offset + j;
            if (grid[k] > 1.e20  ||  grid[k] < -1.e20) {
                sprintf (c50, "%f %f %7.2g\n", xt, yt, grid[k]);
            }
            else {
                sprintf (c50, "%f %f %7.2f\n", xt, yt, grid[k]);
            }
            fputs (c50, fptr);
        }
    }

    fclose (fptr);

    return 1;

}


/*
  ****************************************************************

               g r d _ F i l l N u l l V a l u e s

  ****************************************************************

  function name:    grd_FillNullValues             (int)

  call sequence:    grd_FillNullValues (grid, ncol, nrow,
                                        x1, y1, x2, y2,
                                        faults, nfaults,
                                        nullval, gridout, maskout)

  purpose:          Expand the non null values in a grid out to the
                    edge of the grid to replace the nulls.  The expansion
                    is done by extrapolating gradients in nearest non null
                    nodes where that is possible.  If not possible, or if
                    the null node is far from non null nodes, a simple
                    average of non null nodes is also used.  For most
                    filled nodes, the values will be a combination of the
                    gradient extrapolation and the simple average.

  return value:     status code

                    1 = success
                   -1 = error

  errors:           1 = memory allocation error
                    2 = original grid is all null values
                    3 = grid pointer is null
                    4 = ncol or nrow less than 2

  calling parameters:

    grid      r/w  CSW_F*       Grid array with null values in it.
    ncol      r    int          Number of columns in grid.
    nrow      r    int          Number of rows in grid.
    x1        r    CSW_F        Minimum x of the grid.
    y1        r    CSW_F        Minimum y of the grid.
    x2        r    CSW_F        Maximum x of the grid.
    y2        r    CSW_F        Maximum y of the grid.
    faults    r    FAultLineStruct*   Array of fault lines used to calculate
                                the original grid.
    nfaults   r    int          Number of fault lines.
    nullval   r    CSW_F        Null value for the grid.
    gridout   w    CSW_F*       Optional output grid, or NULL to
                                overwrite the input grid.
    maskout   w    char*        Optional mask for output grid.  Each
                                hard null value in the input grid will
                                have its mask node set to 1, meaning
                                uncontrolled outside the data area.
                                If this is NULL, it is ignored.

*/

int CSWGrdTriangle::grd_FillNullValues (CSW_F *grid, int ncol, int nrow,
                        CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                        FAultLineStruct *faults, int nfaults,
                        CSW_F nullval, CSW_F *gridout, char *maskout)
{
    int          istat;

    grd_fault_ptr->grd_free_faults ();
    if (faults  &&  nfaults > 0) {
        istat = grd_fault_ptr->grd_define_fault_vectors (faults, nfaults);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        istat = grd_fault_ptr->grd_build_fault_indices (grid, ncol, nrow,
                                         x1, y1, x2, y2);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        istat = grd_fault_ptr->grd_fill_faulted_nulls (nullval);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }
    else {
        istat = grd_utils_ptr->grd_fill_nulls_new (grid, ncol, nrow, nullval, gridout, maskout, 0);
    }

    return istat;

}  /*  end of function grd_FillNullValues  */





/*
 *******************************************************************************

               g r d _ C a l c T r i M e s h F r o m G r i d

 *******************************************************************************

  Calculate a topologically correct mesh of triangles from a rectangular grid.

*/

int CSWGrdTriangle::grd_calc_tri_mesh_from_grid (CSW_F *grid, int nc, int nr,
                             double x1, double y1, double x2, double y2,
                             double *xlines, double *ylines, double *zlines,
                             int *linepoints, int *linetypesin, int nlines,
                             int trimesh_style,
                             NOdeStruct **nodes_out, EDgeStruct **edges_out,
                             TRiangleStruct **triangles_out,
                             int *num_nodes_out, int *num_edges_out,
                             int *num_triangles_out)
{
    int                 istat, i, *ltypes = NULL, nl;
    int                 icomp[500], do_write;


    auto fscope = [&]()
    {
        csw_Free (ltypes);
    };
    CSWScopeGuard func_scope_guard (fscope);


    nl = nlines;
    if (nl < 0) nl = -nl;

    ltypes = NULL;
    if (nl > 0) {
        ltypes = (int *)csw_Malloc (nl * sizeof(int));
        if (ltypes == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

        for (i=0; i<nl; i++) {
            if (linetypesin != NULL) {
                if (linetypesin[i] < -1000  ||
                    linetypesin[i] > 1000) {
                    linetypesin[i] = 0;
                }
                ltypes[i] = linetypesin[i];
            }
            else {
                ltypes[i] = 0;
            }
        }
    }

/*
 * Calculate an unconstrained trimesh from the grid.
 */
    if (trimesh_style != GRD_EQUILATERAL  &&
        trimesh_style != GRD_CELL_DIAGONALS) {
        trimesh_style = GRD_EQUILATERAL;
    }
    grd_set_shifts_for_debug (0.0, 0.0);
    istat = grd_grid_to_trimesh (grid, nc, nr,
                                 x1, y1, x2, y2,
                                 xlines, ylines, zlines,
                                 linepoints, ltypes, nlines,
                                 trimesh_style,
                                 nodes_out, edges_out, triangles_out,
                                 num_nodes_out, num_edges_out, num_triangles_out);
    if (istat == -1) {
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write  &&  nlines <= 500) {
        for (i=0; i<nlines; i++) {
            icomp[i] = 1;
        }
        grd_WriteLines (xlines, ylines, zlines,
                        nlines, icomp, linepoints,
                        (char *)"fromgrid.xyz");
    }
    if (do_write) {
        grd_WriteTextTriMeshFile (
          0, NULL,
          *triangles_out, *num_triangles_out,
          *edges_out, *num_edges_out,
          *nodes_out, *num_nodes_out,
          (char *)"fromgrid.tri");
    }

    return istat;

}  /* end of grd_CalcTriMeshFromGrid function */
