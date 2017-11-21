
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
  con_triangle.c

  This file has functions used to trace contours through
  a tri mesh.

*/

/*#include <stdio.h>*/
#include <string.h>
#include <math.h>
#include <assert.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_triangle_class.h"
#include "csw/surfaceworks/private_include/grd_utils.h"

#include "csw/surfaceworks/private_include/con_calc.h"

#include "csw/surfaceworks/private_include/con_triangle_class.h"


/*
 *************************************************************************

        c o n _ c o n t o u r _ g r i d _ u s i n g _ t r i m e s h

 *************************************************************************

  This is called to contour faulted grids.  The grid is resampled at
  a finer interval according to the contour smoothing factor.  The
  resampled faulted grid is converted to a trimesh and all of the
  nodes in the trimesh are assigned z values if needed.  Then, the
  contours are traced through the trimesh.

  This is only called from con_CalcContours, in con_api.c.  This
  calling function sets up some fault structures needed here.

*/

int CSWConTriangle::con_contour_grid_using_trimesh
    (CSW_F *gridin, int ncol, int nrow, double nullval,
     double x1, double y1, double x2, double y2,
     FAultLineStruct *faults, int nfaults,
     COntourOutputRec **contours, int *ncontours,
     COntourCalcOptions *input_options)
{
    int                  ncolout, nrowout, *linepoints, *linetypes, nlines,
                         i, j, istat, n, maxdiv, sfact, maxnodes, npts;
    CSW_F                *fx = NULL, *fy = NULL, *gout = NULL;
    double               aspect, *xline = NULL, *yline = NULL, *zline = NULL;
    NOdeStruct           *nodes = NULL, *np1 = NULL, *np2 = NULL;
    int                  numnodes;
    TRiangleStruct       *triangles = NULL;
    int                  numtriangles;
    EDgeStruct           *edges = NULL;
    int                  numedges;
    COntourOutputRec     *local_contours = NULL,
                         *local_faults = NULL,
                         *conout = NULL;
    int                  num_local_contours,
                         num_local_faults;
    COntourCalcOptions   *options;
    COntourCalcOptions   local_options;
    double               xshift, yshift;
    char                 fname[100];
    double               top_grid_value,
                         base_grid_value;
    CSW_F                *grid = NULL;
    double               zmin, zmax, cnull;
    double               dx, dy, avspace;

    int                  do_write;
    double               v6[6];
    int                  nc[500];

/*
    Set the output to nulls in case an error occurs
*/
    *contours = NULL;
    *ncontours = 0;
    gout = NULL;

    auto fscope = [&]()
    {
        grd_fault_ptr->grd_free_faults();
        con_calc_ptr->con_free_contours (local_contours, num_local_contours);
        con_calc_ptr->con_free_contours (local_faults, num_local_faults);
        csw_Free (gout);
        csw_Free (grid);
        csw_Free (linepoints);
        csw_Free (xline);
        csw_Free (linetypes);
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
        csw_Free (fx);
        fx = NULL;
        fy = NULL;
        grid = NULL;
        gout = NULL;
        linepoints = NULL;
        xline = NULL;
        yline = NULL;
        zline = NULL;
        linetypes = NULL;
        nodes = NULL;
        edges = NULL;
        triangles = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    If the options pointer is NULL, use default options.  Otherwise
    make a local copy of the options since it may be modified for
    plateau or valley handling.
*/
    options = &local_options;
    if (input_options == NULL) {
        con_calc_ptr->con_default_calc_options (options);
        top_grid_value = 1.e30;
        base_grid_value = -1.e30;
    }
    else {
        memcpy (options, input_options, sizeof(COntourCalcOptions));
        top_grid_value = options->top_grid_value;
        base_grid_value = options->base_grid_value;
    }

/*
 *  If the grid has an upper and/or lower clip limit, the
 *  temporary grid for contouring must be extrapolated
 *  above or below to make a good first and last contour.
 */
    grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (grid == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    memcpy (grid, gridin, ncol * nrow * sizeof(CSW_F));

    cnull = nullval / 10.0;
    zmin = 1.e30;
    zmax = -1.e30;
    for (i=0; i<ncol*nrow; i++) {
        if (grid[i] < cnull) {
            if (grid[i] < zmin) zmin = grid[i];
            if (grid[i] > zmax) zmax = grid[i];
        }
    }

    if (base_grid_value > -1.e20  ||  top_grid_value < 1.e20) {

        if (top_grid_value < 1.e20f) {
            istat = grd_utils_ptr->grd_fill_plateau (grid, ncol, nrow,
                                      (CSW_F)top_grid_value,
                                      (CSW_F)cnull,
                                      (CSW_F)(zmax - zmin));
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }

        if (base_grid_value > -1.e20f) {
            istat = grd_utils_ptr->grd_fill_valley (grid, ncol, nrow,
                                     (CSW_F)base_grid_value,
                                     (CSW_F)cnull,
                                     (CSW_F)(zmax - zmin));
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }

        if (options->first_contour >= options->last_contour) {
            if (base_grid_value > -1.e20) options->first_contour = (CSW_F)base_grid_value;
            if (top_grid_value < 1.e20) options->last_contour = (CSW_F)top_grid_value;
        }
    }

/*
    Shift the corners of the grid so that the lower left is
    at 0, 0.  This improves the precision of geometric
    calculations.  The contour line coordinates are shifted
    back at the end.
*/
    xshift = x1;
    yshift = y1;

    x1 = 0.0;
    y1 = 0.0;
    x2 -= xshift;
    y2 -= yshift;

    istat = grd_fault_ptr->grd_define_and_shift_fault_vectors (faults, nfaults,
                                                xshift, yshift);
    if (istat == -1) {
        return -1;
    }
    grd_fault_ptr->grd_build_fault_indices (grid, ncol, nrow,
                             (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2);

/*
    Calculate the rows and columns for the resampled grid
    depending upon the smoothing factor for the contours
    and the aspect ratio of the input grid.
*/
    aspect = (x2 - x1) / (y2 - y1);
    sfact = 3;
    if (options) {
        sfact = options->smoothing;
    }

    maxdiv = sfact / 2 + 1;
    if (maxdiv > 4) maxdiv = 4;

    if (aspect < 0.01) aspect = 0.01;

    if (aspect >= 1.0) {
        ncolout = 30 + sfact * 10;
        nrowout = (int)((double)ncolout / aspect);
    }
    else {
        nrowout = 30 + sfact * 10;
        ncolout = (int)((double)nrowout * aspect);
    }

    maxnodes = 36000 + (sfact - 3) * 24000;
    aspect = (double)(ncolout * nrowout) / (double)maxnodes;
    if (aspect > 1.0) {
        nrowout = (int)((double)nrowout / aspect);
        ncolout = (int)((double)ncolout / aspect);
    }

    ncolout = ncol;
    nrowout = nrow;

    do_write = csw_GetDoWrite ();
    if (do_write) {
      sprintf (fname, "confault_in.grd");
      grd_fileio_ptr->grd_write_text_file (grid, ncol, nrow,
                         x1, y1, x2, y2,
                         fname);
    }

    gout = (CSW_F *)csw_Malloc (ncolout * nrowout * sizeof(CSW_F));
    if (gout == NULL) {
        return -1;
    }
    grd_fault_ptr->grd_set_fault_resample_null_fill_flag (0);
    istat = grd_fault_ptr->grd_resample_faulted_grid (gout, ncolout, nrowout,
                                       (CSW_F)x1, (CSW_F)y1,
                                       (CSW_F)x2, (CSW_F)y2,
                                       GRD_BICUBIC, 1);
    grd_fault_ptr->grd_set_fault_resample_null_fill_flag (1);
    if (istat == -1) {
        return -1;
    }
    istat = grd_fault_ptr->grd_build_fault_indices (gout, ncolout, nrowout,
                        (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2);
    if (istat == -1) {
        return -1;
    }

/*
 *  Get the constraints from the faulted grid functions.  The original
 *  input faults may have been moved slightly to avoid coincidence with
 *  grid columns or rows, so the only accurate constraints available
 *  must come from the grid fault functions.
 */

/*
 * This code previously used ncol and nrow for calculating avspace.
 * The corrected code uses ncolout and nrowout.
 */
    dx = (x2 - x1) / (ncolout - 1);
    dy = (y2 - y1) / (nrowout - 1);
    avspace = (dx + dy) / 2.0;
    grd_fault_ptr->grd_get_current_fault_lines (&xline, &yline, &zline,
                                 &linepoints, &nlines, avspace);

    linetypes = (int *)csw_Malloc (nlines * sizeof(int));
    if (linetypes == NULL) {
        return -1;
    }

    for (i=0; i<nlines; i++) {
        linetypes[i] = GRD_DISCONTINUITY_CONSTRAINT;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
      sprintf (fname, "confault.xyz");
      for (i=0; i<nlines; i++) {
        nc[i] = 1;
      }
      grd_triangle_ptr->grd_WriteLines (
        xline, yline, zline,
        nlines, nc, linepoints,
        fname);
      sprintf (fname, "confault.grd");
      grd_fileio_ptr->grd_write_text_file (gout, ncolout, nrowout,
                         x1, y1, x2, y2,
                         fname);
    }

/*
 * First convert the grid to an unconstrained tri mesh.
 */
    istat = grd_triangle_ptr->grd_grid_to_trimesh (
        gout, ncolout, nrowout,
        x1, y1, x2, y2,
        xline, yline, zline,
        linepoints, linetypes, nlines,
        GRD_EQUILATERAL,
        &nodes,
        &edges,
        &triangles,
        &numnodes,
        &numedges,
        &numtriangles);
    if (istat == -1) {
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_fileio_ptr->grd_write_tri_mesh_file (triangles, numtriangles,
                              edges, numedges,
                              nodes, numnodes,
                              0.0, 0.0, 
                              "preconstraint.tri");
    }

/*
 * Add the exact constraint lines if needed.
 */
    if (xline != NULL) {
        istat =
        grd_triangle_ptr->grd_add_lines_to_trimesh (
            xline, yline, zline,
            linepoints, linetypes, nlines,
            1,
            &nodes, &edges, &triangles,
            &numnodes, &numedges, &numtriangles);
        if (istat == -1) {
            return -1;
        }
    }

    if (do_write) {
        grd_fileio_ptr->grd_write_tri_mesh_file (triangles, numtriangles,
                              edges, numedges,
                              nodes, numnodes,
                              0.0, 0.0, 
                              "postconstraint.tri");
    }


/*
    Fill in any z values that need it.
*/
    istat = grd_triangle_ptr->grd_interpolate_trimesh_values (
        gout, ncolout, nrowout,
        x1, y1, x2, y2,
        nodes, numnodes,
        edges, numedges,
        triangles, numtriangles);
    if (istat == -1) {
        return -1;
    }

    if (do_write) {
        grd_fileio_ptr->grd_write_tri_mesh_file (triangles, numtriangles,
                              edges, numedges,
                              nodes, numnodes,
                              0.0, 0.0, 
                              "postzinterp.tri");
    }

/*
 * Subdivide triangles where needed and assign smooth z values
 * to the new nodes.
 */
    NodeList = nodes;
    EdgeList = edges;
    TriangleList = triangles;
    NumNodes = numnodes;
    NumEdges = numedges;
    NumTriangles = numtriangles;

    istat =
      SmoothTriangles (sfact, maxdiv);

    if (istat == -1) {
        return -1;
    }

    nodes = NodeList;
    edges = EdgeList;
    triangles = TriangleList;
    numnodes = NumNodes;
    numedges = NumEdges;
    numtriangles = NumTriangles;

    NodeList = NULL;
    EdgeList = NULL;
    TriangleList = NULL;
    NumNodes = 0;
    NumEdges = 0;
    NumTriangles = 0;

/*
 *  If debugging is on for faulted contours, write out a trimesh file
 */
    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        sprintf (fname, "con_debug.tri");
        grd_fileio_ptr->grd_write_text_tri_mesh_file (
            0, v6,
            triangles, numtriangles,
            edges, numedges,
            nodes, numnodes,
            fname);
    }

/*
    Trace the contours through the trimesh.
*/
    istat = con_contours_from_trimesh (
        triangles, numtriangles,
        edges, numedges,
        nodes, numnodes,
        nullval,
        &local_contours, &num_local_contours,
        options);
    if (istat == -1) {
        return -1;
    }

/*
    Add the triangle edges that are also faults into
    the output contours as fault segments.  First,
    the number of fault edges are counted, then space
    is allocated and the fault edges are added to the
    output contours.
*/
    n = 0;
    for (i=0; i<numedges; i++) {
        if (edges[i].deleted) {
            continue;
        }
        if (edges[i].flag != GRD_TRIMESH_FAULT_CONSTRAINT) {
            continue;
        }
        n++;
    }

    num_local_faults = n;
    local_faults = (COntourOutputRec *)csw_Calloc
        (n * sizeof(COntourOutputRec));
    if (local_faults == NULL) {
        return -1;
    }

    n = 0;
    for (i=0; i<numedges; i++) {
        if (edges[i].deleted) {
            continue;
        }
        if (edges[i].flag != GRD_TRIMESH_FAULT_CONSTRAINT) {
            continue;
        }
        np1 = nodes + edges[i].node1;
        np2 = nodes + edges[i].node2;
        local_faults [n].faultflag = 1;
        fx = (CSW_F *)csw_Malloc (4 * sizeof(CSW_F));
        if (fx == NULL) {
            return -1;
        }
        fy = fx + 2;
        fx[0] = (CSW_F)np1->x;
        fy[0] = (CSW_F)np1->y;
        fx[1] = (CSW_F)np2->x;
        fy[1] = (CSW_F)np2->y;
        local_faults[n].x = fx;
        local_faults[n].y = fy;
        local_faults[n].npts = 2;
        n++;
    }

    csw_Free (triangles);
    triangles = NULL;
    csw_Free (edges);
    edges = NULL;
    csw_Free (nodes);
    nodes = NULL;

    n = num_local_contours + num_local_faults;
    conout = (COntourOutputRec *) csw_Realloc (local_contours,
                                               n * sizeof(COntourOutputRec));
    memcpy (conout+num_local_contours,
            local_faults,
            num_local_faults * sizeof(COntourOutputRec));
    csw_Free (local_faults);
    local_faults = NULL;

/*
    Shift the x and y coordinates of the output contours
    and faults back to the original system.
*/
    for (i=0; i<n; i++) {
        npts = conout[i].npts;
        fx = conout[i].x;
        fy = conout[i].y;
        for (j=0; j<npts; j++) {
            fx[j] += (CSW_F)xshift;
            fy[j] += (CSW_F)yshift;
        }
    }

    *contours = conout;
    *ncontours = n;

    return 1;

}  /* end of con_contour_grid_using_trimesh function */










/*
 *************************************************************************

           c o n _ c o n t o u r s _ f r o m _ t r i m e s h

 *************************************************************************

  Return an array of COntourLineStruct structures that have contours for
  the specified trimesh.  The contour interval (or separate contour levels)
  should be specified as part of the contour calculation options structure.

*/

int CSWConTriangle::con_contours_from_trimesh
                      (TRiangleStruct *triangles, int numtriangles,
                       EDgeStruct *edges, int numedges,
                       NOdeStruct *nodes, int numnodes,
                       double nullvalue,
                       COntourOutputRec **contours,
                       int *numcontours,
                       COntourCalcOptions *options)
{
    int                i, istat, ival;

    *contours = NULL;
    *numcontours = 0;

    auto fscope = [&]()
    {
        FreeMem ();
    };
    CSWScopeGuard func_scope_guard (fscope);

    XconWork = (double *)csw_Malloc (numedges * 2 * sizeof(double));
    if (!XconWork) {
        return -1;
    }
    YconWork = XconWork + numedges;
    NumConWork = 0;
    ZvalSave = (double *)csw_Malloc (numnodes * sizeof(double));
    if (!ZvalSave) {
        return -1;
    }

    con_calc_ptr->con_set_calc_options (options);

    for (i=0; i<numnodes; i++) {
        ZvalSave[i] = nodes[i].z;
    }

    NodeList = nodes;
    NumNodes = numnodes;
    EdgeList = edges;
    NumEdges = numedges;
    TriangleList = triangles;
    NumTriangles = numtriangles;

    NullValue = nullvalue;
    if (NullValue > -1.e10  &&  NullValue < 1.e10) {
        NullValue = 1.e30;
    }
    NullValue /= 100.0;

    PointLimits ();
    if (Zmin >= Zmax) {
        return -1;
    }

    if (options->nminor + options->nmajor <= 0) {
        ContourInterval = options->contour_interval;
        Nminor = 0;
        Nmajor = 0;
        MajorList = NULL;
        MinorList = NULL;
    }
    else {
        ContourInterval = -1.0;
        Nminor = options->nminor;
        Nmajor = options->nmajor;
        MajorList = options->major_contours;
        MinorList = options->minor_contours;
    }

    LogBase = options->log_base;
    ContourMin = options->first_contour;
    ContourMax = options->last_contour;
    MajorInterval = options->major_spacing;
    if (ContourInterval < 0.0  &&  Nminor + Nmajor < 1) {
        istat = SetContourLimits ();
        if (istat == -1) {
            return -1;
        }
    }

    if (ContourInterval <= 0.0) {
        grd_utils_ptr->grd_set_err (9);
        return -1;
    }

    if (ContourMin >= ContourMax) {
        ival = (int)(Zmin / ContourInterval);
        ContourMin = ival * ContourInterval;
        ContourMin -= ContourInterval;
        ival = (int)(Zmax / ContourInterval);
        ContourMax = ival * ContourInterval;
        ContourMax += ContourInterval;
    }

    ContourBase = options->base_value;

    ContourLines = NULL;
    NumContourLines = 0;
    MaxContourLines = 0;

    CalcContours ();

    *contours = ContourLines;
    *numcontours = NumContourLines;

    return 1;

} /* end of con_contours_from_trimesh function */



/*
 *******************************************************************************

                              F r e e M e m

 *******************************************************************************

  Free the workspace memory used in calculating contours from the trimesh.

*/

void CSWConTriangle::FreeMem (void)
{
    if (XconWork != NULL) {
        csw_Free (XconWork);
    }
    if (ZvalSave != NULL) {
        csw_Free (ZvalSave);
    }

    XconWork = NULL;
    YconWork = NULL;
    ZvalSave = NULL;

    return;

}  /* end of private FreeMem function */




/*
 *****************************************************************************

                         C a l c C o n t o u r s

 *****************************************************************************

    Trace all contour levels from the min to the max.

*/

int CSWConTriangle::CalcContours (void)
{
    int           i, istat, ival;
    double        tiny;

/*
    Do specifically designated contour levels.
*/
    if (ContourInterval <= 0.0) {
        MajorFlag = 1;
        for (i=0; i<Nmajor; i++) {
            ContourLevel = MajorList[i];
            AdjustNodeValues ();
            istat = TraceContourLevel ();
            if (istat == -1) {
                return -1;
            }
        }
        MajorFlag = 0;
        for (i=0; i<Nminor; i++) {
            ContourLevel = MinorList[i];
            AdjustNodeValues ();
            istat = TraceContourLevel ();
            if (istat == -1) {
                return -1;
            }
        }
    }

/*
    Or, do a contour interval.
*/
    else {
        tiny = ContourInterval / 10.0;
        AdjustNodesForInterval ();
        ContourLevel = ContourMin;
        for (;;) {
            if (ContourLevel > ContourMax) {
                break;
            }
            if (ContourLevel < 0.0) {
                ival = (int)((ContourLevel - ContourBase - tiny) / ContourInterval);
            }
            else {
                ival = (int)((ContourLevel - ContourBase + tiny) / ContourInterval);
            }
            MajorFlag = 0;
            if (ival % MajorInterval == 0) {
                MajorFlag = 1;
            }
            istat = TraceContourLevel ();
            if (istat == -1) {
                return -1;
            }
            ContourLevel += ContourInterval;
        }
    }

    return 1;
}






/*
 *********************************************************************************

                      T r a c e C o n t o u r L e v e l

 *********************************************************************************

    Trace all contours for a single contour level through a triangular mesh.
*/

int CSWConTriangle::TraceContourLevel (void)
{
    int               i, istat;
    EDgeStruct        *etmp;

    for (i=0; i<NumEdges; i++) {
        EdgeList[i].tflag = 0;
    }

/*
    First, trace all contours that originate with an edge
    which only has a single triangle attached to it.
    These will be unclosed contours.
*/
    for (i=0; i<NumEdges; i++) {
        etmp = EdgeList + i;
        if (etmp->tflag) {
            continue;
        }
        if (etmp->tri2 >= 0) {
            continue;
        }
        istat = EdgeZrange (etmp);
        if (istat == 0) {
            continue;
        }

        TraceSingleContour (etmp);
    }

/*
    Trace any contours that start or end on faults.
*/
    TracingFaultFlag = 1;
    for (i=0; i<NumEdges; i++) {
        etmp = EdgeList + i;
        if (etmp->tflag) {
            continue;
        }
        if (etmp->flag == 0) {
            continue;
        }
        istat = EdgeZrange (etmp);
        if (istat == 0) {
            continue;
        }

        TraceSingleContour (etmp);
    }
    TracingFaultFlag = 0;

/*
    Next, trace any closed contours for this level.
    In this pass, any edge not already used is eligible
    as the start of a contour.
*/
    for (i=0; i<NumEdges; i++) {
        etmp = EdgeList + i;
        if (etmp->tflag) {
            continue;
        }
        istat = EdgeZrange (etmp);
        if (istat == 0) {
            continue;
        }

        TraceSingleContour (etmp);
    }

    return 1;
}


/*
 *****************************************************************************

             T r a c e S i n g l e C o n t o u r

 *****************************************************************************

    Trace one continuous contour through the triangular mesh.

*/

int CSWConTriangle::TraceSingleContour (EDgeStruct *estart)
{
    int                next, t1, t2, first;
    EDgeStruct         *enow, *exitedge;

    NumConWork = 0;
    next = estart->tri1;
    enow = estart;
    AddEdgePoint (estart);

    first = 1;
    for (;;) {
        if (next < 0) break;
        exitedge = FindExitEdge (enow, TriangleList + next);
        if (exitedge == NULL) {
            if (TracingFaultFlag  &&  first == 1  &&  next == estart->tri1) {
                next = estart->tri2;
                continue;
            }
            break;
        }
        if (TracingFaultFlag == 1  &&
            exitedge->flag == GRD_TRIMESH_INSIDE_FAULT) {
            if (next == estart->tri2) break;
            if (first == 0) break;
            if (NumConWork > 1) break;
            next = estart->tri2;
            continue;
        }
        AddEdgePoint (exitedge);
        if (exitedge == estart) break;
        if (first) {
            SetDownhill (estart, exitedge);
            first = 0;
        }
        if (exitedge->flag != 0) break;
        t1 = exitedge->tri1;
        t2 = exitedge->tri2;
        if (t1 != next) {
            next = t1;
        }
        else {
            next = t2;
        }
        if (next < 0) break;
        enow = exitedge;
    }

    OutputContourLine ();

    return 1;
}




/*
 ***************************************************************************

                       F i n d E x i t E d g e

 ***************************************************************************

    Find the edge where the contour level exits from
    the specified triangle with entry on the specified
    edge.

*/

EDgeStruct *CSWConTriangle::FindExitEdge
                                (EDgeStruct *enow,
                                 TRiangleStruct *tnow)
{
    EDgeStruct        *etmp;

    etmp = EdgeList + tnow->edge1;
    if (etmp != enow) {
        if (enow->flag != 0  &&  etmp->flag != 0) {
        }
        else {
            if (EdgeZrange(etmp) == 1) {
                return etmp;
            }
        }
    }

    etmp = EdgeList + tnow->edge2;
    if (etmp != enow) {
        if (enow->flag != 0  &&  etmp->flag != 0) {
        }
        else {
            if (EdgeZrange(etmp) == 1) {
                return etmp;
            }
        }
    }

    etmp = EdgeList + tnow->edge3;
    if (etmp != enow) {
        if (enow->flag != 0  &&  etmp->flag != 0) {
        }
        else {
            if (EdgeZrange(etmp) == 1) {
                return etmp;
            }
        }
    }

    return NULL;
}





/*
 ***************************************************************************

                       A d d E d g e P o i n t

 ***************************************************************************

    Add the x,y coordinates of the zlevel intersection with
    the specified edge to the contour work arrays.

*/

void CSWConTriangle::AddEdgePoint (EDgeStruct *enow)
{
    NOdeStruct        *nptr;
    double            x1, y1, z1, x2, y2, z2,
                      xt, yt, pct;

    nptr = NodeList + enow->node1;
    x1 = nptr->x;
    y1 = nptr->y;
    z1 = nptr->z;
    nptr = NodeList + enow->node2;
    x2 = nptr->x;
    y2 = nptr->y;
    z2 = nptr->z;

    pct = (ContourLevel - z1) / (z2 - z1);

    xt = x1 + pct * (x2 - x1);
    yt = y1 + pct * (y2 - y1);

    XconWork[NumConWork] = xt;
    YconWork[NumConWork] = yt;

    NumConWork++;

    enow->tflag = 1;

    return;
}




/*
 *************************************************************************

                       P o i n t L i m i t s

 *************************************************************************

    Calculate the z limits of the nodes.

*/

int CSWConTriangle::PointLimits (void)
{
    double           z1, z2, zt;
    int              i;

    z1 = 1.e30;
    z2 = -1.e30;

    for (i=0; i<NumNodes; i++) {
        zt = NodeList[i].z;
        if (zt >= NullValue) continue;
        if (zt < z1) z1 = zt;
        if (zt > z2) z2 = zt;
    }

    ZvalAdjust = (z2 - z1) / TINY_DIVISOR;
    if (ZvalAdjust < 1.e-24) {
        ZvalAdjust = 1.e-24;
    }
    Zmin = z1;
    Zmax = z2;
    Ztiny = (Zmax - Zmin) / 1000000.0;

    return 1;
}





/*
 ***************************************************************************

                     O u t p u t C o n t o u r L i n e

 ***************************************************************************

  Add a contour line to the output array of contour lines.

*/

int CSWConTriangle::OutputContourLine (void)
{
    int              i;
    CSW_F            *fx, *fy;
    COntourOutputRec *crec;

    int              do_write;
    double           v6[6];

    if (NumConWork < 2) return 1;

    if (NumContourLines >= MaxContourLines) {
        MaxContourLines += CONTOUR_LINE_CHUNK;
        ContourLines = (COntourOutputRec *)csw_Realloc
                 (ContourLines, MaxContourLines*sizeof(COntourOutputRec));
        if (!ContourLines) {
            return -1;
        }
    }

    crec = ContourLines + NumContourLines;
    NumContourLines++;

    crec->zvalue = (CSW_F)ContourLevel;
    con_calc_ptr->con_format_number (crec->zvalue, (CSW_F)LogBase, crec->text);
    crec->npts = NumConWork;

    fx = (CSW_F *)csw_Malloc (NumConWork * 2 * sizeof(CSW_F));
    if (fx == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    fy = fx + NumConWork;

    for (i=0; i<NumConWork; i++) {
        fx[i] = (CSW_F)XconWork[i];
        fy[i] = (CSW_F)YconWork[i];
    }

    crec->x = fx;
    crec->y = fy;
    crec->major = (char)MajorFlag;
    crec->downhill = (char)DownhillFlag;
    crec->closure = (char)ClosureFlag;
    crec->local_min_max = 0;
    crec->expect_double = 0;
    crec->faultflag = 0;



    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        grd_fileio_ptr->grd_write_points (XconWork,
                         YconWork,
                         YconWork,
                         NumConWork,
                         (char *)"contour.xyz");
        grd_fileio_ptr->grd_write_text_tri_mesh_file (
                         0, v6,
                         TriangleList, NumTriangles,
                         EdgeList, NumEdges,
                         NodeList, NumNodes,
                         (char *)"contour.tri");
    }





    return 1;

}



/*
 *****************************************************************************

                     A d j u s t N o d e V a l u e s

 *****************************************************************************

    Adjust any node z values that are almost exactly on a contour level.

*/

int CSWConTriangle::AdjustNodeValues (void)
{
    int                       i;
    double                    z1, zt;
    double                    tiny, fudge;

    tiny = ZvalAdjust;
    fudge = tiny * 5.0;

    for (i=0; i<NumNodes; i++) {
        NodeList[i].z = ZvalSave[i];
        z1 = NodeList[i].z;
        if (z1 > NullValue) continue;
        zt = z1 - ContourLevel;
        if (zt > -tiny  &&  zt < tiny) {
            NodeList[i].z = z1 + fudge;
        }
    }

    return 1;
}



/*
 ****************************************************************************

                          E d g e Z r a n g e

 ****************************************************************************

  Return 1 if the specified edge has a z range which bounds the current
  contour level.  Return zero if not.

*/

int CSWConTriangle::EdgeZrange (EDgeStruct *e)
{
    NOdeStruct     *nptr;
    double         z1, z2, dz;

    nptr = NodeList + e->node1;
    z1 = nptr->z;
    nptr = NodeList + e->node2;
    z2 = nptr->z;

    if (NullValue > 0.0) {
        if (z1 >= NullValue  ||  z2 >= NullValue) return 0;
    }
    else {
        if (z1 <= NullValue  ||  z2 <= NullValue) return 0;
    }

    if ((z1 - ContourLevel) * (ContourLevel - z2) <= 0.0) return 0;

    dz = z1 - z2;
    if (dz < 0.0) dz = -dz;
    if (dz <= Ztiny) return 0;

    return 1;

}




/*
 *********************************************************************************

                            S e t D o w n h i l l

 *********************************************************************************

  This is called when the first segment of a contour is calculated.  The two
  edges connected by the segment are used to determine whether the downhill
  direction is to the left (1) or right (-1).

  The segment connecting the two edges forms a triangle along with the
  common node of the two edges.  The orientation of that triangle when
  travelling from the first contour point to the second contour point
  and then to the common node is used to determine if the common node
  is to the left or right of the contour segment.  The value of the
  common node determines downhill or uphill.

*/

void CSWConTriangle::SetDownhill (EDgeStruct *e1, EDgeStruct *e2)
{
    int                 common_node, orientation;
    double              x1, y1, x2, y2, x3, y3, zt, area;

/*
    First, get the segment points and the common node point.
*/
    x1 = XconWork[0];
    y1 = YconWork[0];
    x2 = XconWork[1];
    y2 = YconWork[1];
    if (e1->node1 == e2->node1) {
        common_node = e1->node1;
    }
    else if (e1->node1 == e2->node2) {
        common_node = e1->node1;
    }
    else {
        common_node = e1->node2;
    }
    x3 = NodeList[common_node].x;
    y3 = NodeList[common_node].y;
    zt = NodeList[common_node].z;

/*
    If the triangle area is positive, the orientation is
    clockwise.  If negative, counter clockwise.
*/
    area = x1 * y2 - x2 * y1 +
           x2 * y3 - x3 * y2 +
           x3 * y1 - x1 * y3;
    orientation = -1;
    if (area < 0.0) {
        orientation = 1;
    }

    if (zt < ContourLevel) {
        DownhillFlag = -orientation;
    }
    else {
        DownhillFlag = orientation;
    }

    return;
}





/*
 ********************************************************************************

                       S e t C o n t o u r L i m i t s

 ********************************************************************************

  This is called to set contour limits based on a contour interval and the
  data limits.

*/

int CSWConTriangle::SetContourLimits (void)
{
    int               istat, i, nlist, major;
    CSW_F             conint, conmax, conmin, *zlist;

    auto fscope = [&]()
    {
        csw_Free (zlist);
    };
    CSWScopeGuard func_scope_guard (fscope);

    zlist = (CSW_F *)csw_Malloc (NumNodes * sizeof(CSW_F));
    if (zlist == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    nlist = 0;
    for (i=0; i<NumNodes; i++) {
        if (NodeList[i].deleted) continue;
        zlist[nlist] = (CSW_F)NodeList[i].z;
        nlist++;
    }

    istat = con_calc_ptr->con_calc_default_contour_interval (zlist, nlist,
                                       &conint,
                                       &conmin, &conmax,
                                       &major);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    ContourMin = conmin;
    ContourMax = conmax;
    ContourInterval = conint;
    MajorInterval = major;

    return 1;

}




/*
 **************************************************************************************

                       A d j u s t N o d e s F o r I n t e r v a l

 **************************************************************************************

  Any node that is very close to a contour level, based on the current ContourBase
  and contour interval, has its z value tweeked to make the node different from
  the contour level.

*/

void CSWConTriangle::AdjustNodesForInterval (void)
{
    int               i, ival;
    double            z, zt1, zt2, zt3, dz1, dz2, dz3;
    double            fudge;

    fudge = ZvalAdjust * 5.0;

    for (i=0; i<NumNodes; i++) {
        z = NodeList[i].z;
        if (z > NullValue) continue;
        ival = (int)((z - ContourBase) / ContourInterval);
        zt1 = ival * ContourInterval;
        zt2 = zt1 + ContourInterval;
        zt3 = zt1 - ContourInterval;
        dz1 = z - zt1;
        if (dz1 < 0.0) dz1 = -dz1;
        dz2 = z - zt2;
        if (dz2 < 0.0) dz2 = -dz2;
        dz3 = z - zt3;
        if (dz3 < 0.0) dz3 = -dz3;
        if (dz1 <= ZvalAdjust  ||  dz2 <= ZvalAdjust  ||  dz3 <= ZvalAdjust) {
            NodeList[i].z += fudge;
        }
    }

    return;

}


/*------------------------------------------------------------------------------*/

/*
 * This section has a collection of functions used to calculate a smooth,
 * subdivided trimesh.
 */

/*------------------------------------------------------------------------------*/

int CSWConTriangle::con_smooth_trimesh (
    NOdeStruct    **nodes,
    int           *numnodes,
    EDgeStruct    **edges,
    int           *numedges,
    TRiangleStruct  **tris,
    int           *numtris,
    int           sfact)
{
    int           maxdiv, istat;

    NodeList = *nodes;
    NumNodes = *numnodes;
    EdgeList = *edges;
    NumEdges = *numedges;
    TriangleList = *tris;
    NumTriangles = *numtris;

    if (NodeList == NULL  ||
        EdgeList == NULL  ||
        TriangleList == NULL) {
        return -1;
    }

    if (NumNodes < 3  ||  NumEdges < 3  ||  NumTriangles < 1) {
        return -1;
    }

    maxdiv = sfact / 2 + 1;
    if (maxdiv < 2) maxdiv = 2;
    if (maxdiv > 4) maxdiv = 4;

    istat =
      SmoothTriangles (sfact, maxdiv);

    *nodes = NodeList;
    *numnodes = NumNodes;
    *edges = EdgeList;
    *numedges = NumEdges;
    *tris = TriangleList;
    *numtris = NumTriangles;

    return istat;

}

/*------------------------------------------------------------------------------*/

int CSWConTriangle::SmoothTriangles (int sfact, int maxdiv)
{
    int               istat, ido, i, nav;
    TRiangleStruct    *tptr;
    EDgeStruct        *eptr;
    NOdeStruct        *nptr;
    double            xmin, ymin, zmin, xmax, ymax, zmax,
                      zt, dx, dy, avlen;
    double            xtri[3], ytri[3], ztri[3];
    int               do_write;
    char              fname[100];
    double            xwave, ywave;

    auto fscope = [&]()
    {
        CleanupTriNormals ();
        CleanupNodeNormals ();
        csw_Free (TriNums);
        csw_Free (xCenters);
        grd_triangle_ptr->grd_unset_trimesh ();
        NumCenters = MaxCenters = 0;
    };
    CSWScopeGuard func_scope_guard (fscope);


    tnxNorm = 0.0;
    tnyNorm = 0.0;
    tnzNorm = 0.0;

    if (maxdiv > MAXDIV) maxdiv = MAXDIV;
    if (sfact < 0) sfact = 0;
    if (sfact > 9) sfact = 9;

    if (sfact == 0) {
        return 1;
    }

    sfact /= 2;
    if (sfact < 1) sfact = 1;

    xmin = ymin = zmin = 1.e30;
    xmax = ymax = zmax = -1.e30;
    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        if (nptr->deleted == 1) continue;
        if (nptr->z > 1.e20) continue;
        if (nptr->x < xmin) xmin = nptr->x;
        if (nptr->y < ymin) ymin = nptr->y;
        if (nptr->z < zmin) zmin = nptr->z;
        if (nptr->x > xmax) xmax = nptr->x;
        if (nptr->y > ymax) ymax = nptr->y;
        if (nptr->z > zmax) zmax = nptr->z;
    }
    if (xmin >= xmax  ||  ymin >= ymax) {
        return -1;
    }

    if (zmin > 1.e20) {
        return -1;
    }

    DeltaZCrit = (zmax - zmin) / 1000.0;
    if (DeltaZCrit < 1.e-8) DeltaZCrit = 1.e-8;

    xwave = (xmax - xmin) / 2.0;
    ywave = (ymax - ymin) / 2.0;
    xwave /= 3.1415926;
    ywave /= 3.1415926;

/*
 * Make sure the triangle normal structures start out clean.
 */
    CleanupTriNormals ();
    csw_Free (TriNums);
    csw_Free (xCenters);
    TriNums = NULL;
    xCenters = yCenters = zCenters = NULL;
    NumCenters = MaxCenters = 0;

    do_write = csw_GetDoWrite ();
    if (do_write) {
      sprintf (fname, "presmooth.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        nptr->zorig = nptr->z;
        nptr->z = .1 * (nptr->x - xmin);
      }
      sprintf (fname, "xplane.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        nptr->z = .1 * (nptr->y - ymin);
      }
      sprintf (fname, "yplane.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        nptr->z = .1 * (nptr->y - ymin) +
                  .1 * (nptr->x - xmin);
      }
      sprintf (fname, "xyplane.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        nptr->z = .2 * (nptr->y - ymin) +
                  .1 * (nptr->x - xmin);
      }
      sprintf (fname, "xy2plane.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        nptr->z = .1 * (nptr->y - ymin) +
                  .2 * (nptr->x - xmin);
      }
      sprintf (fname, "x2yplane.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        dx = nptr->x - xmin;
        dy = nptr->y - ymin;
        zt = dx * dx + dy * dy;
        zt = sqrt (zt);
        nptr->z = .1 * zt;
      }
      sprintf (fname, "xysphere.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        dx = nptr->x - xmin;
        dx *= 2;
        dy = nptr->y - ymin;
        zt = dx * dx + dy * dy;
        zt = sqrt (zt);
        nptr->z = .1 * zt;
      }
      sprintf (fname, "x2ysphere.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        dx = nptr->x - xmin;
        dy = nptr->y - ymin;
        dy *= 2;
        zt = dx * dx + dy * dy;
        zt = sqrt (zt);
        nptr->z = .1 * zt;
      }
      sprintf (fname, "xy2sphere.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        dx = nptr->x - xmin;
        dy = nptr->y - ymin;
        zt = sin (dx / xwave) + sin (dy / ywave);
        nptr->z = 100 * zt;
      }
      sprintf (fname, "xywave.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        dx = nptr->x - xmin;
        dx *= 2.0;
        dy = nptr->y - ymin;
        zt = sin (dx / xwave) + sin (dy / ywave);
        nptr->z = 100 * zt;
      }
      sprintf (fname, "x2ywave.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        dx = nptr->x - xmin;
        dy = nptr->y - ymin;
        dy *= 2.0;
        zt = sin (dx / xwave) + sin (dy / ywave);
        nptr->z = 100 * zt;
      }
      sprintf (fname, "xy2wave.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);

      for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        nptr->z = nptr->zorig;
      }
    }

/*
 * If the smoothing factor is greater than zero, calculate the
 * average length of non deleted edges to use as part of the
 * node smoothing algorithm.
 */
    avlen = 0.0;
    nav = 0;
    if (sfact > 0) {
        for (i=0; i<NumEdges; i++) {
            if (EdgeList[i].deleted == 1) continue;
            avlen += EdgeLength (i);
            nav++;
        }
    }
    if (nav > 0) {
        avlen /= nav;
    }

/*
 * Make sure normals are available for all triangles.
 */
    for (i=0; i<NumTriangles; i++) {

        tptr = TriangleList + i;
        if (tptr->deleted == 1) continue;
        if (tptr->norm == NULL) {
            tptr->norm = (TRiNormStruct *)csw_Calloc (sizeof(TRiNormStruct));
            if (tptr->norm == NULL) {
                return -1;
            }
            TrianglePoints (
                tptr, NodeList, EdgeList,
                xtri, ytri, ztri);
            TriangleNormal (
                xtri, ytri, ztri);
            tptr->norm->nx = tnxNorm;
            tptr->norm->ny = tnyNorm;
            tptr->norm->nz = tnzNorm;
        }

    }

/*
 * Smooth the original triangle nodes by converting to a grid, smoothing
 * the grid, and interpolating back to the trimesh nodes.
 */
    if (sfact > 0  &&  avlen > 0.0) {
      istat =
        grd_triangle_ptr->grd_smooth_trimesh_nodes_directly (
          NodeList, NumNodes,
          EdgeList, NumEdges,
          TriangleList, NumTriangles,
          100 + sfact, avlen);
      if (istat == -1) {
        return -1;
      }
    }

    if (do_write) {
      sprintf (fname, "presmooth2.tri");
      grd_fileio_ptr->grd_write_text_tri_mesh_file (
        0, NULL,
        TriangleList, NumTriangles,
        EdgeList, NumEdges,
        NodeList, NumNodes,
        fname);
    }

/*
 * Loop through all the triangles maxdiv times.  In each
 * iteration, subdivide and recalc z values where needed
 * to support higher surface curvatures.
 */
    for (ido=0; ido<maxdiv; ido++) {

        grd_triangle_ptr->grd_set_trimesh (
            NodeList, NumNodes,
            EdgeList, NumEdges,
            TriangleList, NumTriangles);

    /*
     * Make sure normals are available for all triangles and nodes.
     */
        for (i=0; i<NumTriangles; i++) {

            tptr = TriangleList + i;
            if (tptr->deleted == 1) continue;
            if (tptr->norm == NULL) {
                tptr->norm = (TRiNormStruct *)csw_Calloc (sizeof(TRiNormStruct));
                if (tptr->norm == NULL) {
                    return -1;
                }
                TrianglePoints (
                    tptr, NodeList, EdgeList,
                    xtri, ytri, ztri);
                TriangleNormal (
                    xtri, ytri, ztri);
                tptr->norm->nx = tnxNorm;
                tptr->norm->ny = tnyNorm;
                tptr->norm->nz = tnzNorm;
            }

        }

        istat =
          CalcNodeNormals ();
        if (istat == -1) {
            return -1;
        }

    /*
     * Check the curvature in the vicinity of each triangle and decide
     * whether to subdivide the triangle or not.  All triangles start with
     * their subdivide flags set to zero, meaning subdivision may be needed.
     * Once it is determined that subdivision is not needed, the flag is
     * set to 1, and no subdivision will be needed for the rest of the
     * triangle smoothing process.
     */
        for (i=0; i<NumTriangles; i++) {

            tptr = TriangleList + i;
            if (tptr->deleted == 1) continue;
            if (tptr->norm == NULL) {
                assert (0);
            }

        /*
         * If the triangle has already been deemed to not require
         * subdivision, there is no need to check the triangle again.
         */
            if (tptr->norm->subflag == 1) {
                continue;
            }

            SetTriangleSubdivideFlag (tptr, ido);

        }

    /*
     * Set all edge flags to not be swapped.  Any edges from
     * subdivided triangles will have their flags set to allow
     * swapping when the triangle is subdivided.  I use the
     * edge struct tflag member for this.
     */
        for (i=0; i<NumEdges; i++) {
            eptr = EdgeList + i;
            eptr->tflag = 0;
        }

    /*
     * Subdivide triangles using the centroid point.  This will also
     * estimate z values at the new points for a smooth surface.
     */
        TriNums = (int *)csw_Malloc (NumTriangles * sizeof(int));
        xCenters = (double *)csw_Malloc (3 * NumTriangles * sizeof(double));
        if (TriNums == NULL  ||  xCenters == NULL) {
            return -1;
        }
        yCenters = xCenters + NumTriangles;
        zCenters = yCenters + NumTriangles;
        NumCenters = 0;
        MaxCenters = NumTriangles;
        for (i=0; i<NumTriangles; i++) {

            tptr = TriangleList + i;
            if (tptr->deleted == 1) continue;
            if (tptr->norm == NULL) {
                assert (0);
            }
            if (tptr->norm->subflag == 1) {
                continue;
            }
            SubdivideTriangleFromCenter (tptr);

        }

    /*
     * Adjust the topology for the new points.
     */
        grd_triangle_ptr->grd_unset_trimesh ();

        istat =
        grd_triangle_ptr->grd_SubdivideTrianglesFromCenters (
            TriNums,
            xCenters,
            yCenters,
            zCenters,
            NumCenters,
            &NodeList, &NumNodes,
            &EdgeList, &NumEdges,
            &TriangleList, &NumTriangles);
        if (istat == -1) {
            return -1;
        }

        if (do_write) {
          sprintf (fname, "smooth_%d.tri", ido);
          grd_fileio_ptr->grd_write_text_tri_mesh_file (
            0, NULL,
            TriangleList, NumTriangles,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
        }

        csw_Free (TriNums);
        csw_Free (xCenters);
        TriNums = NULL;
        xCenters = yCenters = zCenters = NULL;
        NumCenters = MaxCenters = 0;

    } /* end of ido loop through subdivision of triangles */


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
void CSWConTriangle::TriangleNormal (double *x, double *y, double *z)
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

void CSWConTriangle::TrianglePoints
                           (TRiangleStruct *tptr,
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

}  /* end of private TrianglePoints function */

/*------------------------------------------------------------------------------*/

/*
 * The decision to subdivide is currently based on the range of normal
 * components for the triangle and any triangles edge connected to the
 * triangle.
 */
void CSWConTriangle::SetTriangleSubdivideFlag
         (TRiangleStruct *tptr, int iter_num)
{
    double      zt1, zt2, dz, zcrit;
    double      xtri[3], ytri[3], ztri[3], xt, yt;

    TrianglePoints (tptr, NodeList, EdgeList, xtri, ytri, ztri);
    xt = (xtri[0] + xtri[1] + xtri[2]) / 3.0;
    yt = (ytri[0] + ytri[1] + ytri[2]) / 3.0;

    zt1 = CalcZForTriangle (xt, yt, tptr);

    grd_triangle_ptr->grd_bezier_smooth_triangle_center (tptr, &zt2);

    if (zt2 > 1.e20) {
        tptr->norm->subflag = 1;
        tptr->norm->zcenter = 1.e30;
        return;
    }

    if (iter_num <= 0) {
        zcrit = DeltaZCrit;
    }
    else {
        zcrit = DeltaZCrit * iter_num;
    }

    dz = zt2 - zt1;
    if (dz < 0.0) dz = -dz;

    if (dz <= zcrit) {
        tptr->norm->subflag = 1;
        tptr->norm->zcenter = 1.e30;
    }
    else {
        tptr->norm->zcenter = zt2;
    }

    return;

}

/*------------------------------------------------------------------------------*/

/*
 * Subdivide the triangle into three parts by adding a point at
 * the "center" of the triangle.  I simply use the average x, y
 * of the 3 corners for the center.
 */
void CSWConTriangle::SubdivideTriangleFromCenter
         (TRiangleStruct *tptr)
{
    double          xtri[3], ytri[3], ztri[3];
    double          xc, yc, zc;

    if (tptr == NULL) return;

/*
 * Calculate the center point z.
 */
    if (tptr->norm->zcenter < 1.e20) {
        zc = tptr->norm->zcenter;
    }
    else {
        grd_triangle_ptr->grd_bezier_smooth_triangle_center (
            tptr, &zc);
    }

    if (zc >= 1.e20) {
        return;
    }

/*
 * Calculate the center point x and y.
 */
    TrianglePoints (tptr,
                    NodeList,
                    EdgeList,
                    xtri, ytri, ztri);

    xc = (xtri[0] + xtri[1] + xtri[2]) / 3.0;
    yc = (ytri[0] + ytri[1] + ytri[2]) / 3.0;

/*
 * Add the point to the list of points to add to the topology.
 */
    if (NumCenters >= MaxCenters) {
        return;
    }
    TriNums[NumCenters] = tptr - TriangleList;
    xCenters[NumCenters] = xc;
    yCenters[NumCenters] = yc;
    zCenters[NumCenters] = zc;
    NumCenters++;

    return;
}

/*------------------------------------------------------------------------------*/

/*
 * Use the triangle normal and a corner point of the triangle
 * to calculate the z value at the specified x, y point.  If
 * the triangle is very nearly vertical, 1.e30 is returned.
 * If there is a problem with the input triangle, 1.e30 is returned.
 */

double CSWConTriangle::CalcZForTriangle
                               (double x, double y,
                                TRiangleStruct *tptr)
{
    double               x0, y0, z0, z, zout, nx, ny, nz;
    EDgeStruct           *eptr;
    NOdeStruct           *nptr;

    zout = 1.e30;

    if (tptr == NULL) {
        return zout;
    }

    if (tptr->norm == NULL) {
        return zout;
    }

    if (tptr->deleted == 1) {
        return zout;
    }

    eptr = EdgeList + tptr->edge1;
    nptr = NodeList + eptr->node1;
    x0 = nptr->x;
    y0 = nptr->y;
    z0 = nptr->z;

    nx = tptr->norm->nx;
    ny = tptr->norm->ny;
    nz = tptr->norm->nz;

    if (nz < 1.e-8) {
        return zout;
    }

    z = (nx * (x - x0) + ny * (y - y0)) / -nz;
    zout = z + z0;

    return zout;

}


/*------------------------------------------------------------------------------*/

void CSWConTriangle::CleanupTriNormals (void)
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

double CSWConTriangle::EdgeLength (int iedge)
{
    EDgeStruct       *eptr;
    NOdeStruct       *np1, *np2;
    double           dx, dy, dist;

    eptr = EdgeList + iedge;
    np1 = NodeList + eptr->node1;
    np2 = NodeList + eptr->node2;
    dx = np1->x - np2->x;
    dy = np1->y - np2->y;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);

    return dist;

}


/*--------------------------------------------------------------------------------*/

/*
 * Calculate the average normal at each node.  The average is just
 * the arithmetic mean of the normals of triangle using the node.
 */

int CSWConTriangle::CalcNodeNormals (void)
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

        grd_triangle_ptr->grd_get_nodes_for_triangle (
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

/*------------------------------------------------------------------------------*/

void CSWConTriangle::CleanupNodeNormals (void)
{
    int             i;
    NOdeStruct      *nptr;

    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        csw_Free (nptr->norm);
        nptr->norm = NULL;
    }
}
