
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif


#include <csw/jsurfaceworks/private_include/PatchSplit.h>
#include <csw/jsurfaceworks/private_include/SWCalc.h>

#include <csw/surfaceworks/src/FaultConnect.h>
#include <csw/surfaceworks/include/grid_api.h>
#include <csw/surfaceworks/private_include/Vert.h>
#include <csw/surfaceworks/include/grid_api.h>

#include <csw/utils/private_include/ply_protoP.h>
#include <csw/utils/include/csw_.h>


// Seems easier to have the functions communicating with
// java be extern C functions.  I could not get the java
// jni headers (that come with java) to compile with the g++
// compiler.

extern "C" {
#include <csw/jsurfaceworks/private_include/SurfaceWorksJNI.h>
}




/*-------------------------------------------------------------------------------*/

void PATCHSplit::csw_Free_clist (FAultCenterline **list, int n)
{
    int      i;

    if (list == NULL  ||  n < 1) return;

    for (i=0; i<n; i++) {
        csw_Free (list[i]->x);
        csw_Free (list[i]);
    }

    csw_Free (list);

    return;
}


/*-------------------------------------------------------------------------------*/

int PATCHSplit::_graze (double x1, double y1, double x2, double y2)
{
    double dx, dy;

    if (GrazeTiny < 0.0) {
        return 0;
    }

    dx = x2 - x1;
    if (dx < 0.0) dx = -dx;
    dy = y2 - y1;
    if (dy < 0.0) dy = -dy;

    if (dx <= GrazeTiny  &&  dy <= GrazeTiny) {
        return 1;
    }

    return 0;
}

/*-------------------------------------------------------------------------------*/

int PATCHSplit::_graze2 (double x1, double y1, double x2, double y2)
{
    double dx, dy;

    if (GrazeTiny < 0.0) {
        return 0;
    }

    dx = x2 - x1;
    if (dx < 0.0) dx = -dx;
    dy = y2 - y1;
    if (dy < 0.0) dy = -dy;

    if (dx <= GrazeTiny * 2.0  &&  dy <= GrazeTiny * 2.0) {
        return 1;
    }

    return 0;
}

/*-------------------------------------------------------------------------------*/

int PATCHSplit::OnSegment (double x1, double y1, double x2, double y2, double x, double y)
{
    double      dx, dy;
    int         istat;

    dx = x2 - x1;
    if (dx < 0.0) dx = -dx;
    dy = y2 - y1;
    if (dy < 0.0) dy = -dy;

    istat = 0;
    if (dx > dy) {
        if ((x - x1) * (x2 - x) >= 0.0) {
            istat = 1;
        }
    }
    else {
        if ((y - y1) * (y2 - y) >= 0.0) {
            istat = 1;
        }
    }

    return istat;

}

/*-------------------------------------------------------------------------------*/

void PATCHSplit::ps_SetDebugState (int ival)
  {
    DebugState = ival;
  }


/*-------------------------------------------------------------------------------*/

void PATCHSplit::ps_SetAverageEdgeLength (double avlen)
  {
    AverageEdgeLength = avlen;
  }

/*-------------------------------------------------------------------------------*/

void PATCHSplit::ps_ClearHorizonData (void)
  {
    FreeSingleHorizonData ();
    DebugState = 0;
  }

/*
 * Free everything except the fault surface list.
 */
void PATCHSplit::FreeSingleHorizonData (void)
{
    int                 i;
    BOrderSegment       *bp;
    FAultCenterline     *fp;
    BAseLine            *lp;

    csw_Free_work_polys ();

    if (BorderIn != NULL) {
        for (i=0; i<NumBorderIn; i++) {
            bp = BorderIn + i;
            csw_Free (bp->x);
        }
    }
    csw_Free (BorderIn);
    BorderIn = NULL;
    NumBorderIn = 0;
    MaxBorderIn = 0;

    if (Fcenter != NULL) {
        for (i=0; i<NumFcenter; i++) {
            fp = Fcenter + i;
            csw_Free (fp->x);
        }
    }
    csw_Free (Fcenter);
    Fcenter = NULL;
    NumFcenter = 0;
    MaxFcenter = 0;

    csw_Free (XPointsIn);
    XPointsIn = NULL;
    YPointsIn = NULL;
    ZPointsIn = NULL;
    NumPointsIn = 0;
    MeshFlag = 0;

    if (LinesIn != NULL) {
        for (i=0; i<NumLinesIn; i++) {
            lp = LinesIn + i;
            csw_Free (lp->x);
            csw_Free (lp->y);
            csw_Free (lp->z);
        }
    }
    csw_Free (LinesIn);
    LinesIn = NULL;
    NumLinesIn = 0;
    MaxLinesIn = 0;
    NumLinePoints = 0;

    if (WorkLines != NULL) {
        for (i=0; i<NumWorkLines; i++) {
            lp = WorkLines + i;
            csw_Free (lp->x);
            csw_Free (lp->y);
            csw_Free (lp->z);
        }
    }
    csw_Free (WorkLines);
    WorkLines = NULL;
    NumWorkLines = 0;
    MaxWorkLines = 0;

    NumPointsIn = NumPointsIn;
    MaxWorkLines = MaxWorkLines;

    XYTiny = -1.e30;

    return;

}


void PATCHSplit::ps_ClearAllData (void)
  {
    FreeData ();
    DebugState = 0;
  }

void PATCHSplit::FreeData (void)
{
    int                 i;
    BOrderSegment       *bp;
    FAultCenterline     *fp;
    BAseLine            *lp;

    csw_Free_work_polys ();

    if (BorderIn != NULL) {
        for (i=0; i<NumBorderIn; i++) {
            bp = BorderIn + i;
            csw_Free (bp->x);
        }
    }
    csw_Free (BorderIn);
    BorderIn = NULL;
    NumBorderIn = 0;
    MaxBorderIn = 0;

    if (Fcenter != NULL) {
        for (i=0; i<NumFcenter; i++) {
            fp = Fcenter + i;
            csw_Free (fp->x);
        }
    }
    csw_Free (Fcenter);
    Fcenter = NULL;
    NumFcenter = 0;
    MaxFcenter = 0;

    csw_Free (XPointsIn);
    XPointsIn = NULL;
    YPointsIn = NULL;
    ZPointsIn = NULL;
    NumPointsIn = 0;

    if (LinesIn != NULL) {
        for (i=0; i<NumLinesIn; i++) {
            lp = LinesIn + i;
            csw_Free (lp->x);
            csw_Free (lp->y);
            csw_Free (lp->z);
        }
    }
    csw_Free (LinesIn);
    LinesIn = NULL;
    NumLinesIn = 0;
    MaxLinesIn = 0;
    NumLinePoints = 0;

    if (WorkLines != NULL) {
        for (i=0; i<NumWorkLines; i++) {
            lp = WorkLines + i;
            csw_Free (lp->x);
            csw_Free (lp->y);
            csw_Free (lp->z);
        }
    }
    csw_Free (WorkLines);
    WorkLines = NULL;
    NumWorkLines = 0;
    MaxWorkLines = 0;

    NumPointsIn = NumPointsIn;
    MaxWorkLines = MaxWorkLines;

    XYTiny = -1.e30;

    Xmin = 1.e30,
    Ymin = 1.e30,
    Xmax = -1.e30,
    Ymax = -1.e30,
    Zmin = 1.e30,
    Zmax = -1.e30;

    FreeOrigPointsList ();
    FreeOrigLinesList ();
    FreePatchList ();
    FreeProtoContactLines ();
    FreeSplitLines ();

    if (SModel != NULL) {
        delete SModel;
        SModel = NULL;
    }

}


/*-------------------------------------------------------------------------------*/

void PATCHSplit::ps_SetLineTrimFraction (double val)
  {
    if (val < 0.01) val = 0.01;
    if (val > 0.2) val = 0.2;
    LineTrimFraction = val;
  }

/*-------------------------------------------------------------------------------*/

/*
 * Copy a border segment to the list of border segments associated with
 * the current horizon being defined for splitting.  The ownership of the
 * data remains with the calling function.
 */
int PATCHSplit::ps_AddBorderSegment (
    double              *x,
    double              *y,
    double              *z,
    int                 npts,
    int                 type,
    int                 flag)
  {
    BOrderSegment       *bp;

    if (x == NULL  ||  y == NULL  ||  z == NULL) {
        return 0;
    }
    if (npts < 2) {
        return 0;
    }

    if (type < 1  ||  type > 3) {
        type = 0;
    }

    if (NumBorderIn >= MaxBorderIn) {
        MaxBorderIn += 10;
        BorderIn = (BOrderSegment *)csw_Realloc (
            BorderIn,
            MaxBorderIn * sizeof(BOrderSegment)
        );
    }

    if (BorderIn == NULL) {
        return -1;
    }

    bp = BorderIn + NumBorderIn;
    bp->x = (double *)csw_Malloc (3 * npts * sizeof(double));
    if (bp->x == NULL) {
        bp->x = NULL;
        bp->y = NULL;
        bp->z = NULL;
        return -1;
    }
    bp->y = bp->x + npts;
    bp->z = bp->y + npts;
    memcpy (bp->x, x, npts*sizeof(double));
    memcpy (bp->y, y, npts*sizeof(double));
    memcpy (bp->z, z, npts*sizeof(double));
    bp->npts = npts;
    bp->type = type;
    bp->flag = flag;

    NumBorderIn++;

    UpdateLimits (x, y, z, npts);

    return 1;

  }


/*-------------------------------------------------------------------------*/

/*
 * Copy a Fault Cut center line to the list of center lines
 * to be used to split up the surface.  The data pointers
 * stay owned by the calling function.
 */
int PATCHSplit::ps_AddFaultCenterline (
    double       *x,
    double       *y,
    double       *z,
    int          npts,
    int          fault_id,
    int          flag)
  {
    FAultCenterline    *fp;

    if (x == NULL  ||  y == NULL  ||  z == NULL) {
        return 0;
    }

    if (npts < 2) {
        return 0;
    }

    if (NumFcenter >= MaxFcenter) {
        MaxFcenter += 10;
        Fcenter = (FAultCenterline *)csw_Realloc
            (Fcenter,
             MaxFcenter * sizeof(FAultCenterline));
    }

    if (Fcenter == NULL) {
        return -1;
    }

    fp = Fcenter + NumFcenter;

    fp->x = (double *)csw_Malloc (npts * 6 * sizeof(double));
    if (fp->x == NULL) {
        fp->y = NULL;
        fp->z = NULL;
        return -1;
    }
    fp->y = fp->x + npts;
    fp->z = fp->y + npts;
    memcpy (fp->x, x, npts * sizeof(double));
    memcpy (fp->y, y, npts * sizeof(double));
    memcpy (fp->z, z, npts * sizeof(double));
    fp->npts = npts;
    fp->flag = flag;
    fp->fault_id = fault_id;
    fp->j1 = 0;
    fp->j2 = npts;
    fp->origcline = NumFcenter;

    fp->jleft = -1;
    fp->jright = -1;
    fp->left_contact = -1;
    fp->right_contact = -1;

    NumFcenter++;

    UpdateLimits (x, y, z, npts);

    return 1;
  }

/*----------------------------------------------------------------------------*/

/*
 * Set the points currently being used for the input patch.  The data are copied
 * to local arrays so the calling function retains ownership and should csw_Free the
 * data when needed.
 */
int PATCHSplit::ps_SetPoints (
    double       *x,
    double       *y,
    double       *z,
    int          npts,
    int          meshflag)
  {
    csw_Free (XPointsIn);
    XPointsIn = NULL;
    YPointsIn = NULL;
    ZPointsIn = NULL;
    NumPointsIn = 0;
    MeshFlag = 0;

    if (x == NULL  ||  y == NULL  ||  z == NULL) {
        return 1;
    }

    if (npts < 1) {
        return 1;
    }

    XPointsIn = (double *)csw_Malloc (3 * npts * sizeof(double));
    if (XPointsIn == NULL) {
        return -1;
    }
    YPointsIn = XPointsIn + npts;
    ZPointsIn = YPointsIn + npts;

    memcpy (XPointsIn, x, npts * sizeof(double));
    memcpy (YPointsIn, y, npts * sizeof(double));
    memcpy (ZPointsIn, z, npts * sizeof(double));

    NumPointsIn = npts;
    MeshFlag = meshflag;

    UpdateLimits (x, y, z, npts);

    return 1;

  }


/*-----------------------------------------------------------------------------*/

/*
 * Copy a line currently associated with the input patch.  These
 * are lines other than fault cut center lines.  For instance, a
 * lithology boundary might qualify as one of these lines.  The
 * calling function retains ownership of the data.
 */
int PATCHSplit::ps_AddLine (
    double       *xin,
    double       *yin,
    double       *zin,
    int          npts,
    int          flag)
  {
    BAseLine           *bp;
    double             *x, *y, *z;

    if (NumLinesIn >= MaxLinesIn) {
        MaxLinesIn += 10;
        LinesIn = (BAseLine *)csw_Realloc
            (LinesIn,
             MaxLinesIn * sizeof(BAseLine));
    }

    if (LinesIn == NULL) {
        return -1;
    }

    x = (double *)csw_Malloc (npts * 3 * sizeof(double));
    if (x == NULL) {
        return -1;
    }
    y = x + npts;
    z = y + npts;
    memcpy (x, xin, npts * sizeof(double));
    memcpy (y, yin, npts * sizeof(double));
    memcpy (z, zin, npts * sizeof(double));

    bp = LinesIn + NumLinesIn;
    bp->x = x;
    bp->y = y;
    bp->z = z;
    bp->npts = npts;
    bp->flag = flag;

    NumLinePoints += npts;

    UpdateLimits (x, y, z, npts);

    return 1;

  }


/*------------------------------------------------------------------------------*/

void PATCHSplit::ps_SetModelBounds (
    double xmin,
    double ymin,
    double zmin,
    double xmax,
    double ymax,
    double zmax
  )
  {
    Xmin = xmin;
    Ymin = ymin;
    Zmin = zmin;
    Xmax = xmax;
    Ymax = ymax;
    Zmax = zmax;
  }



/*----------------------------------------------------------------------------*/

/*
 * Add a horizon patch that does not need splitting
 * directly to the sealed model object.
 */

int PATCHSplit::ps_AddHorizonTriMeshPatch (
    int          id,
    double       age,
    double       *xnodes,
    double       *ynodes,
    double       *znodes,
    int          num_nodes,
    int          *n1edge,
    int          *n2edge,
    int          *t1edge,
    int          *t2edge,
    int          num_edges,
    int          *e1tri,
    int          *e2tri,
    int          *e3tri,
    int          num_tris)
  {
    NOdeStruct   *nodes;
    EDgeStruct   *edges;
    TRiangleStruct   *tris;
    int          i, istat;

    if (SModel == NULL) {
        return -1;
    }

    nodes = (NOdeStruct *)csw_Calloc (num_nodes * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc (num_edges * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    tris = (TRiangleStruct *)csw_Calloc (num_tris * sizeof(TRiangleStruct));
    if (tris == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        return -1;
    }

    for (i=0; i<num_nodes; i++) {
        nodes[i].x = xnodes[i];
        nodes[i].y = ynodes[i];
        nodes[i].z = znodes[i];
    }

    for (i=0; i<num_edges; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<num_tris; i++) {
        tris[i].edge1 = e1tri[i];
        tris[i].edge2 = e2tri[i];
        tris[i].edge3 = e3tri[i];
    }

    SModel->addInputHorizon (
        id, age,
        nodes, num_nodes,
        edges, num_edges,
        tris, num_tris);

    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (tris);
    nodes = NULL;
    edges = NULL;
    tris = NULL;

    istat =
    AddToOrigPointsList (
        XPointsIn, YPointsIn, ZPointsIn,
        NumPointsIn, id);
    if (istat == -1) {
        return -1;
    }

    istat =
    AddToOrigLinesList (
        LinesIn,
        NumLinesIn, id);
    if (istat == -1) {
        return -1;
    }

    return 1;

  }


/*
 * Add a fault surface trimesh directly to the SealedModel object.
 */
int PATCHSplit::ps_AddFaultSurface (
    int          id,
    int          vused,
    double       *vbase,
    double       *x,
    double       *y,
    double       *z,
    int          num_nodes,
    int          *n1,
    int          *n2,
    int          *t1,
    int          *t2,
    int          num_edges,
    int          *e1,
    int          *e2,
    int          *e3,
    int          num_triangles,
    double       minage,
    double       maxage)
  {
    int              i;
    NOdeStruct       *nodes;
    EDgeStruct       *edges;
    TRiangleStruct   *triangles;

    if (SModel == NULL) {
        return -1;
    }

    if (x == NULL  ||  y == NULL  ||  z == NULL  ||
        n1 == NULL  ||  n2 == NULL  ||  t1 == NULL  ||  t2 == NULL  ||
        e1 == NULL  ||  e2 == NULL  ||  e3 == NULL  ||
        num_nodes < 3  ||  num_edges < 3  ||  num_triangles < 1) {
        return 0;
    }

    nodes = (NOdeStruct *)csw_Calloc (num_nodes * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc (num_edges * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    triangles = (TRiangleStruct *)csw_Calloc (num_triangles * sizeof(TRiangleStruct));
    if (triangles == NULL) {
        csw_Free (edges);
        csw_Free (nodes);
        return -1;
    }

    for (i=0; i<num_nodes; i++) {
        nodes[i].x = x[i];
        nodes[i].y = y[i];
        nodes[i].z = z[i];
    }

    for (i=0; i<num_edges; i++) {
        edges[i].node1 = n1[i];
        edges[i].node2 = n2[i];
        edges[i].tri1 = t1[i];
        edges[i].tri2 = t2[i];
    }

    for (i=0; i<num_triangles; i++) {
        triangles[i].edge1 = e1[i];
        triangles[i].edge2 = e2[i];
        triangles[i].edge3 = e3[i];
    }

    SModel->addInputFault (
        id,
        vused, vbase,
        nodes, num_nodes,
        edges, num_edges,
        triangles, num_triangles,
        minage, maxage);

    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (triangles);
    nodes = NULL;
    edges = NULL;
    triangles = NULL;

    AddToOrigPointsList (
        XPointsIn, YPointsIn, ZPointsIn,
        NumPointsIn, id);

    AddToOrigLinesList (
        LinesIn,
        NumLinesIn, id);

    return 1;

  }


/*----------------------------------------------------------------------------*/

int PATCHSplit::BuildBorderPoints (
    BOrderSegment *blist,
    int           nlist,
    double        **xborder,
    double        **yborder,
    double        **zborder,
    int           *nborder
)
{
    BOrderSegment *bp;
    double        *xb, *yb, *zb;
    double        x1, y1, x2, y2;
    int           i, j, n;
    int           nb, same, ndone;

  /*
   * Allocate space for border points.
   */
    n = 0;
    for (i=0; i<nlist; i++) {
        n += blist[i].npts;
        n += 2;
        blist[i].used = 0;
    }

    if (n < 4) {
        return -1;
    }

    xb = (double *)csw_Malloc (n * 3 * sizeof(double));
    if (xb == NULL) {
        return -1;
    }
    yb = xb + n;
    zb = yb + n;

  /*
   * Put the first border segment's points into the point arrays.
   */
    nb = 0;
    bp = blist;
    for (i=0; i<bp->npts; i++) {
        xb[nb] = bp->x[i];
        yb[nb] = bp->y[i];
        zb[nb] = bp->z[i];
        nb++;
    }

    bp->used = 1;

  /*
   * find border segments that have not been used that connect to
   * the end of the point list.  Add each segment as found.
   */
    for (;;) {
        ndone = 0;
        for (i=1; i<nlist; i++) {
            bp = blist + i;
            if (bp->used == 1) {
                continue;
            }
            x1 = bp->x[0];
            y1 = bp->y[0];
            x2 = bp->x[bp->npts-1];
            y2 = bp->y[bp->npts-1];

          /*
           * If first point connects, append in forward order.
           */
            same = SamePoint (x1, y1, xb[nb-1], yb[nb-1]);
            if (same == 1) {
                for (j=0; j<bp->npts; j++) {
                    xb[nb] = bp->x[j];
                    yb[nb] = bp->y[j];
                    zb[nb] = bp->z[j];
                    nb++;
                }
                bp->used = 1;
                same = SamePoint (xb[0], yb[0], xb[nb-1], yb[nb-1]);
                if (same == 1) {
                    ndone = 0;
                    break;
                }
                ndone++;
                continue;
            }

          /*
           * If first point connects, append in forward order.
           */
            same = SamePoint (x2, y2, xb[nb-1], yb[nb-1]);
            if (same == 1) {
                for (j=bp->npts-1; j>=0; j--) {
                    xb[nb] = bp->x[j];
                    yb[nb] = bp->y[j];
                    zb[nb] = bp->z[j];
                    nb++;
                }
                bp->used = 1;
                same = SamePoint (xb[0], yb[0], xb[nb-1], yb[nb-1]);
                if (same == 1) {
                    ndone = 0;
                    break;
                }
                ndone++;
                continue;
            }

        }
        if (ndone == 0) {
            break;
        }

    }

    same = SamePoint (xb[0], yb[0], xb[nb-1], yb[nb-1]);
    if (same == 0) {
        xb[nb] = xb[0];
        yb[nb] = yb[0];
        nb++;
    }

    for (i=0; i<nlist; i++) {
        blist[i].used = 0;
    }

    *xborder = xb;
    *yborder = yb;
    *zborder = zb;
    *nborder = nb;

    return 1;

}  /* end of private BuildBorderPoints function */


/*----------------------------------------------------------------------------*/

int PATCHSplit::SamePoint (
    double          x1,
    double          y1,
    double          x2,
    double          y2
)
{
    double          dx, dy, dist;

    if (XYTiny < 0.0) {
        return 0;
    }

    dx = x2 - x1;
    dy = y2 - y1;
    dist = dx * dx + dy * dy;

    if (dist <= XYTiny * XYTiny) {
        return 1;
    }

    return 0;

}  /* end of private SamePoint function */

/*----------------------------------------------------------------------------*/

int PATCHSplit::SamePointXYZ (
    double          x1,
    double          y1,
    double          z1,
    double          x2,
    double          y2,
    double          z2
)
{
    double          dx, dy, dz, dist;

    if (XYTiny < 0.0) {
        return 0;
    }

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    dist = dx * dx + dy * dy + dz * dz;

    if (dist <= XYTiny * XYTiny) {
        return 1;
    }

    return 0;

}  /* end of private SamePointXYZ function */

/*--------------------------------------------------------------------------------*/

void PATCHSplit::UpdateLimits (
    double     *x,
    double     *y,
    double     *z,
    int        npts
)
{
    int        i;

    for (i=0; i<npts; i++) {
        if (x[i] < Xmin) Xmin = x[i];
        if (y[i] < Ymin) Ymin = y[i];
        if (z[i] < Zmin) Zmin = z[i];
        if (x[i] > Xmax) Xmax = x[i];
        if (y[i] > Ymax) Ymax = y[i];
        if (z[i] > Zmax) Zmax = z[i];
    }

    XYTiny = (Xmax - Xmin + Ymax - Ymin) / 10000.0;
    ZTiny = (Zmax - Zmin) / 10000.0;

    XYTiny = XYTiny;
    ZTiny = ZTiny;

    return;

}


/*-------------------------------------------------------------------------------------*/


int PATCHSplit::ps_CalcLinesForSplitting (void)
  {
    int        istat;

    if (SModel == NULL) {
        return -1;
    }

    istat =
    SModel->padFaultsForSplitLines ();
    if (istat == -1) {
        return -1;
    }

    istat =
    SModel->calcFaultHorizonIntersections ();
    if (istat == -1) {
        return -1;
    }

    istat =
    BuildSplitLines ();
    if (istat == -1) {
        return -1;
    }

    SendBackSplitLines ();

    return 1;
  }

/*-------------------------------------------------------------------------------------*/

/*
 * If there are currently defined fault cut centerlines, the
 * currently defined surface is split according to these
 * centerlines.  If no centerlines are defined, nothing is
 * done.  The split surfaces are added to the InputHorizonList
 */
int PATCHSplit::ps_CalcSplit (int id, double age)
  {
    BAseLine        *lp;
    int             i, j, n, istat;
    int             do_write;
    int             ftypes[10000];

    if (SModel == NULL) {
        return -1;
    }

  /*
   * Initialize the number of fault component arrays once.
   */
    if (local_calc_first) {
        for (i=0; i<10000; i++) {
            ftypes[i] = GRD_DISCONTINUITY_CONSTRAINT;
        }
        local_calc_first = 0;
    }

  /*
   * Make sure the data have been setup correctly.
   */
    if (DebugState == 0) {
        if (BorderIn == NULL  ||  NumBorderIn < 1) {
            return 0;
        }
        if (Fcenter == NULL  ||  NumFcenter < 1) {
            return 0;
        }
        if (XPointsIn == NULL  &&  LinesIn == NULL) {
            return 0;
        }
        if (NumPointsIn + NumLinesIn < 1) {
            return 0;
        }
    }

  /*
   * There need to be at least 3 valid z values in the points
   * and lines combined.
   */
    if (DebugState == 0) {
        n = 0;
        if (ZPointsIn != NULL) {
            for (i=0; i<NumPointsIn; i++) {
                if (ZPointsIn[i] < 1.e20) {
                    n++;
                }
            }
        }

        if (n < 4) {
            for (i=0; i<NumLinesIn; i++) {
                lp = LinesIn + i;
                for (j=0; j<lp->npts; j++) {
                    if (lp->z[j] < 1.e20) {
                        n++;
                    }
                }
                if (n > 3) {
                    break;
                }
            }
        }

        if (n < 4) {
            return 0;
        }
    }

    BorderXmin = 1.e30;
    BorderYmin = 1.e30;
    BorderXmax = -1.e30;
    BorderYmax = -1.e30;

/*
 * Build the border for the original unsplit patch.
 */
    csw_Free (OrigXBorder);
    OrigXBorder = NULL;
    OrigYBorder = NULL;
    OrigZBorder = NULL;
    OrigNBorder = 0;

    istat = BuildBorderPoints (BorderIn, NumBorderIn,
                               &OrigXBorder,
                               &OrigYBorder,
                               &OrigZBorder,
                               &OrigNBorder);
    if (istat == -1) {
        FreeData ();
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        WriteCenterlinesToFile ((char *)"raw_clines.xyz");
    }

/*
 * If any centerline endpoint is very close to the border,
 * extend it to cross the border.
 */
    istat =
    CorrectCloseToBorderLines ();
    if (istat == -1) {
        FreeData ();
        return -1;
    }

    if (DebugState != 0) {
        for (i=0; i<NumFcenter; i++) {
            jni_call_add_corrected_centerline_method (
                Fcenter[i].x,
                Fcenter[i].y,
                Fcenter[i].z,
                Fcenter[i].npts);
        }
    }

/*
 * Cleanup crossing centerlines.
 */
    istat = FixCrossingCenterlines (1);
    if (istat == -1) {
        FreeData ();
        return -1;
    }

    if (NumFcenter < 1) {
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        WriteCenterlinesToFile ((char *)"cross_clines.xyz");
    }

/*
 * Extend the fault cut center lines to past the border
 * or to where the extension intersects another center
 * line.
 */
    istat = ExtendCenterlines ();
    if (istat == -1) {
        FreeData ();
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        WriteCenterlinesToFile ((char *)"extend_clines.xyz");
    }

    if (BorderXmin >= BorderXmax  ||
        BorderYmin >= BorderYmax) {
        FreeData ();
        return -1;
    }

/*
 * Clip the centerlines to the inside of the original
 * surface border.
 */
    if (OrigXBorder != NULL  &&  OrigNBorder > 2) {
        istat =
        ClipCenterlinesToBorder ();
        if (istat == -1) {
            FreeData ();
            return -1;
        }
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        WriteCenterlinesToFile ((char *)"clip_clines.xyz");
    }

/*
 * Cleanup crossing centerlines again after extension.
 */
    istat = FixCrossingCenterlines (0);
    if (istat == -1) {
        FreeData ();
        return -1;
    }

    if (NumFcenter < 1) {
        return -1;
    }

    if (DebugState != 0) {
        for (i=0; i<NumFcenter; i++) {
            jni_call_add_extended_centerline_method (
                Fcenter[i].x,
                Fcenter[i].y,
                Fcenter[i].z,
                Fcenter[i].npts);
        }
    }

/*
 * Build polygons from the temporary extended lines.
 */
    istat = BuildWorkPolygons ();
    if (istat == -1) {
        FreeData ();
        return -1;
    }

/*
 * Calculate grids for all work polygons based on the
 * points inside each polygon.
 */
    istat = CalcWorkPolygonSurfaces ();
    if (istat == -1) {
        FreeData ();
        return -1;
    }

    WOrkPoly    *wp;
    for (i=0; i<NumWorkPolyList; i++) {

        wp = WorkPolyList + i;

        if (wp->grid == NULL  ||
            wp->xp == NULL  ||
            wp->yp == NULL  ||
            wp->np < 3  ||
            wp->xmin >= wp->xmax  ||
            wp->ymin >= wp->ymax  ||
            wp->ncol < 2  ||
            wp->nrow < 2) {
            continue;
        }

        if (wp->minor_lines == NULL  ||  wp->num_minor_lines < 1) {
            istat =
              SModel->addInputGridHorizon (
                id,
                age,
                wp->grid,
                wp->ncol,
                wp->nrow,
                wp->xmin,
                wp->ymin,
                wp->xmax,
                wp->ymax,
                wp->xp,
                wp->yp,
                wp->np,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                0);
        }
        else {
            double  *xat, *yat, *zat;
            int     *nat;
            istat = BuildArraysFromBaseline (
              wp->minor_lines, wp->num_minor_lines,
              &xat, &yat, &zat, &nat);
            if (istat == -1) {
                FreeData ();
                return -1;
            }
            istat =
              SModel->addInputGridHorizon (
                id,
                age,
                wp->grid,
                wp->ncol,
                wp->nrow,
                wp->xmin,
                wp->ymin,
                wp->xmax,
                wp->ymax,
                wp->xp,
                wp->yp,
                wp->np,
                xat,
                yat,
                zat,
                nat,
                ftypes,
                wp->num_minor_lines
                );
             csw_Free (xat);
             csw_Free (nat);
             xat = yat = zat = NULL;
             nat = NULL;
        }

        if (istat == -1) {
            FreeData ();
            return -1;
        }
        istat =
        AddToOrigPointsList (
            wp->x,
            wp->y,
            wp->z,
            wp->npts,
            id);
        if (istat == -1) {
            FreeData ();
            return -1;
        }

        istat =
        AddToOrigLinesList (
            LinesIn,
            NumLinesIn, id);
        if (istat == -1) {
            FreeData ();
            return -1;
        }

    }

    OrigXBorder = OrigXBorder;
    OrigYBorder = OrigYBorder;
    OrigZBorder = OrigZBorder;
    OrigNBorder = OrigNBorder;

    FreeSingleHorizonData ();

    return 1;

  }


/*----------------------------------------------------------------------*/

/*
 * Given a bounding box and a line segment find the point where the ray
 * defined by the line segment intersects the box.  The point is the closest
 * intersection point to the x2in, y2in point on the segment.
 */

void PATCHSplit::ClosestBoxCrossing (
    double    bxmin,
    double    bymin,
    double    bxmax,
    double    bymax,
    double    x1in,
    double    y1in,
    double    x2in,
    double    y2in,
    double    otherx,
    double    othery,
    double    *xpoint,
    double    *ypoint)
{
    double    dx, dy, dist, dmin, xpmin, ypmin,
              x1, y1, x2, y2, xt, yt, dmult;
    double    other_dist;
    int       istat;

/*
 * Extend the segment far enough to make sure it intersects
 * the box.
 */
    xt = bxmax - bxmin;
    yt = bymax - bymin;
    dx = x2in - x1in;
    dy = y2in - y1in;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (dist < 0.00001) dist = 0.00001;
    dmult = (xt + yt) / dist;

    x1 = x1in - dx * dmult;
    y1 = y1in - dy * dmult;
    x2 = x2in + dx * dmult;
    y2 = y2in + dy * dmult;

/*
 * Check intersections with each side and return the closest.
 * Note that the ply_seging function returns zero when the
 * intersection point is within the limits of both segments.
 */
    dmin = 1.e30;
    xpmin = 1.e30;
    ypmin = 1.e30;

  /*
   * Check intersection with bottom side of box.
   */
    istat = ply_utils_obj.ply_segint (x1, y1, x2, y2,
                        bxmin, bymin, bxmax, bymin,
                        &xt, &yt);
    if (istat == 0) {
        dx = xt - x2in;
        dy = yt - y2in;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        dx = xt - otherx;
        dy = yt - othery;
        other_dist = dx * dx + dy * dy;
        other_dist = sqrt (other_dist);
        if (dist < other_dist) {
            if (dist < dmin) {
                dmin = dist;
                xpmin = xt;
                ypmin = yt;
            }
        }
    }

  /*
   * Check intersection with top side of box.
   */
    istat = ply_utils_obj.ply_segint (x1, y1, x2, y2,
                        bxmin, bymax, bxmax, bymax,
                        &xt, &yt);
    if (istat == 0) {
        dx = xt - x2in;
        dy = yt - y2in;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        dx = xt - otherx;
        dy = yt - othery;
        other_dist = dx * dx + dy * dy;
        other_dist = sqrt (other_dist);
        if (dist < other_dist) {
            if (dist < dmin) {
                dmin = dist;
                xpmin = xt;
                ypmin = yt;
            }
        }
    }

  /*
   * Check intersection with left side of box.
   */
    istat = ply_utils_obj.ply_segint (x1, y1, x2, y2,
                        bxmin, bymin, bxmin, bymax,
                        &xt, &yt);
    if (istat == 0) {
        dx = xt - x2in;
        dy = yt - y2in;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        dx = xt - otherx;
        dy = yt - othery;
        other_dist = dx * dx + dy * dy;
        other_dist = sqrt (other_dist);
        if (dist < other_dist) {
            if (dist < dmin) {
                dmin = dist;
                xpmin = xt;
                ypmin = yt;
            }
        }
    }

  /*
   * Check intersection with right side of box.
   */
    istat = ply_utils_obj.ply_segint (x1, y1, x2, y2,
                        bxmax, bymin, bxmax, bymax,
                        &xt, &yt);
    if (istat == 0) {
        dx = xt - x2in;
        dy = yt - y2in;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        dx = xt - otherx;
        dy = yt - othery;
        other_dist = dx * dx + dy * dy;
        other_dist = sqrt (other_dist);
        if (dist < other_dist) {
            if (dist < dmin) {
                dmin = dist;
                xpmin = xt;
                ypmin = yt;
            }
        }
    }

    *xpoint = xpmin;
    *ypoint = ypmin;

    return;

}


/*------------------------------------------------------------------------------*/

int PATCHSplit::ExtendCenterlines (void)
{
    double          *xw, *yw, *zw;
    int             nwmax, wsize, istat, line_fit_status,
                    i, j, n, nout, j1, j2;
    FAultCenterline   *fp;
    double          slope, yint, x1, y1, x2, y2,
                    xtiny, ytiny, dtiny, xp, yp,
                    firstx, firsty, firstz, lastx, lasty, lastz;
    double          bx1, by1, bx2, by2;
    double          xfit1, yfit1, xfit2, yfit2;
    double          dx, dy, dist, dtot, pct;
    double          otherx, othery;
    double          maximum_extension, dchk, dang;
    int             first_is_inside, last_is_inside;
    int             j1orig, j2orig;

/*
 * Allocate work space large enough for extending the largest centerline.
 */
    n = 0;
    for (i=0; i<NumFcenter; i++) {
        if (Fcenter[i].npts > n) {
            n = Fcenter[i].npts;
        }
        Fcenter[i].major = 1;
    }

    nwmax = n + 5;
    wsize = nwmax * 3 * sizeof(double);
    xw = (double *)csw_Calloc (wsize);
    if (xw == NULL) {
        return -1;
    }
    yw = xw + nwmax;
    zw = yw + nwmax;

/*
 * Calculate the bounding box of the original border.
 */
    bx1 = 1.e30;
    by1 = 1.e30;
    bx2 = -1.e30;
    by2 = -1.e30;

    for (i=0; i<OrigNBorder; i++) {
        if (OrigXBorder[i] < bx1) bx1 = OrigXBorder[i];
        if (OrigYBorder[i] < by1) by1 = OrigYBorder[i];
        if (OrigXBorder[i] > bx2) bx2 = OrigXBorder[i];
        if (OrigYBorder[i] > by2) by2 = OrigYBorder[i];
    }

    xtiny = (bx2 - bx1) / 100.0;
    ytiny = (by2 - by1) / 100.0;

    dtiny = (xtiny + ytiny) / 2000.0;

    bx1 -= xtiny;
    by1 -= ytiny;
    bx2 += xtiny;
    by2 += ytiny;

/*
 * The border used for connecting polygons is slightly larger than
 * the actual border point limits and also slightly smaller than the
 * extended line limits.
 */
    BorderXmin = bx1;
    BorderYmin = by1;
    BorderXmax = bx2;
    BorderYmax = by2;

    bx1 -= xtiny;
    by1 -= ytiny;
    bx2 += xtiny;
    by2 += ytiny;

/*
 * For each fault centerline, extend it to the bounding box or to the closest
 * other centerline if the extension crosses another centerline.
 */

/*
 * j1 and j2 are the location boundaries in the work arrays for the
 * original centerline points.
 */
    for (i=0; i<NumFcenter; i++) {

        fp = Fcenter + i;

      /*
       * If both endpoints are locked in place due to intersecting
       * faults, do not extend anything on this centerline.
       */
        if (fp->lock1 == 1  &&  fp->lock2 == 1) {
            if (DebugState != 0) {
                jni_call_add_extended_centerline_method (
                    fp->x, fp->y, fp->z, fp->npts);
            }

            continue;
        }

        j1 = 2;
        j2 = j1 + fp->npts;
        j1orig = j1;
        j2orig = j2;

      /*
       * Check if the first and last points of the centerline are
       * inside the bounding box.
       */
        first_is_inside = 1;
        last_is_inside = 1;
        x1 = fp->x[0];
        y1 = fp->y[0];
        x2 = fp->x[fp->npts-1];
        y2 = fp->y[fp->npts-1];
        if (x1 < bx1  ||  x1 > bx2  ||
            y1 < by1  ||  y1 > by2) {
            first_is_inside = 0;
        }
        if (x2 < bx1  ||  x2 > bx2  ||
            y2 < by1  ||  y2 > by2) {
            last_is_inside = 0;
        }

      /*
       * If the first or last point is locked, treat it as if it is outside.
       */
        if (fp->lock1) first_is_inside = 0;
        if (fp->lock2) last_is_inside = 0;

      /*
       * Transfer the centerline points to the j1 to j2-1 elements
       * of the work array.  This leaves room for extension points
       * on each end if they are needed.  Calculate the total length
       * of the line in xy here also.
       */
        dtot = 0.0;
        for (j=0; j<fp->npts; j++) {
            xw[j1+j] = fp->x[j];
            yw[j1+j] = fp->y[j];
            zw[j1+j] = fp->z[j];
            if (j > 0) {
                dx = fp->x[j] - fp->x[j-1];
                dy = fp->y[j] - fp->y[j-1];
                dist = dx * dx + dy * dy;
                dist = sqrt (dist);
                dtot += dist;
            }
        }
        maximum_extension = dtot * ExtendFraction;
        firstz = zw[j1];
        lastz = zw[j2-1];

      /*
       * Find the intercepts of the best fit line with the box.
       * xfit1, yfit1 is the closest to the first original point and
       * xfit2, yfit2 is the closest to the last original point.
       */
        int    np4 = fp->npts / 8;
        if (np4 < 2) np4 = 2;

        line_fit_status =
        csw_FitDoubleLine (xw+j1, yw+j1, np4,
                           &slope, &yint);

        if (line_fit_status == 1) {
            if (slope >= -1.0  &&  slope <= 1.0) {
                x1 = bx1;
                y1 = x1 * slope + yint;
                x2 = bx2;
                y2 = x2 * slope + yint;
            }
            else {
                y1 = by1;
                x1 = (y1 - yint) / slope;
                y2 = by2;
                x2 = (y2 - yint) / slope;
            }
            otherx = x2;
            othery = y2;
            ClosestBoxCrossing (bx1, by1, bx2, by2,
                                x2, y2, x1, y1, otherx, othery,
                                &xfit1, &yfit1);
        }
        else {
            xfit1 = 1.e30;
            yfit1 = 1.e30;
        }

        line_fit_status =
        csw_FitDoubleLine (xw+j2-np4, yw+j2-np4, np4,
                           &slope, &yint);

        if (line_fit_status == 1) {
            if (slope >= -1.0  &&  slope <= 1.0) {
                x1 = bx1;
                y1 = x1 * slope + yint;
                x2 = bx2;
                y2 = x2 * slope + yint;
            }
            else {
                y1 = by1;
                x1 = (y1 - yint) / slope;
                y2 = by2;
                x2 = (y2 - yint) / slope;
            }
            otherx = x1;
            othery = y1;
            ClosestBoxCrossing (bx1, by1, bx2, by2,
                                x1, y1, x2, y2, otherx, othery,
                                &xfit2, &yfit2);
        }
        else {
            xfit2 = 1.e30;
            yfit2 = 1.e30;
        }

      /*
       * If the first point of the center line is inside the bounding box
       * extend the first segment and combine it with the best fit extension.
       */
        if (first_is_inside == 1) {
            x1 = xw[j1+1];
            y1 = yw[j1+1];
            x2 = xw[j1];
            y2 = yw[j1];
            otherx = xw[j2-1];
            othery = yw[j2-1];
            ClosestBoxCrossing (bx1, by1, bx2, by2,
                                x1, y1, x2, y2, otherx, othery,
                                &xp, &yp);
            if (line_fit_status == 1) {
                dx = x2 - x1;
                dy = y2 - y1;
                dist = dx * dx + dy * dy;
                dist = sqrt (dist);
                pct = dist / dtot;
                pct *= 10.0;
                x2 = (xfit1 + pct * xp) / (1.0 + pct);
                y2 = (yfit1 + pct * yp) / (1.0 + pct);
                ClosestBoxCrossing (bx1, by1, bx2, by2,
                                    x1, y1, x2, y2, otherx, othery,
                                    &firstx, &firsty);
            }
            else {
                firstx = xp;
                firsty = yp;
            }
        }
        else {
            firstx = xw[j1];
            firsty = yw[j1];
        }

      /*
       * Make sure that the firstx, firsty point is no farther then
       * maximum_extension from the j1th point on the original line.
       */
        dx = firstx - xw[j1];
        dy = firsty - yw[j1];
        dchk = dx * dx + dy * dy;
        dchk = sqrt (dchk);
        if (dchk > maximum_extension) {
            dang = atan2 (dy, dx);
            firstx = xw[j1] + maximum_extension * cos (dang);
            firsty = yw[j1] + maximum_extension * sin (dang);
        }

      /*
       * If the last point is inside the bounding box
       * extend the last segment and combine it with the best fit extension.
       */
        if (last_is_inside == 1) {
            x1 = xw[j2-2];
            y1 = yw[j2-2];
            x2 = xw[j2-1];
            y2 = yw[j2-1];
            otherx = xw[j1];
            othery = yw[j1];
            ClosestBoxCrossing (bx1, by1, bx2, by2,
                                x1, y1, x2, y2, otherx, othery,
                                &xp, &yp);
            if (line_fit_status == 1) {
                dx = x2 - x1;
                dy = y2 - y1;
                dist = dx * dx + dy * dy;
                dist = sqrt (dist);
                pct = dist / dtot;
                pct *= 10.0;
                x2 = (xfit2 + pct * xp) / (1.0 + pct);
                y2 = (yfit2 + pct * yp) / (1.0 + pct);
                ClosestBoxCrossing (bx1, by1, bx2, by2,
                                    x1, y1, x2, y2, otherx, othery,
                                    &lastx, &lasty);
            }
            else {
                lastx = xp;
                lasty = yp;
            }
        }
        else {
            lastx = xw[j2-1];
            lasty = yw[j2-1];
        }

      /*
       * Make sure that the lastx, lasty point is no farther then
       * maximum_extension from the j2-1th point on the original line.
       */
        dx = lastx - xw[j2-1];
        dy = lasty - yw[j2-1];
        dchk = dx * dx + dy * dy;
        dchk = sqrt (dchk);
        if (dchk > maximum_extension) {
            dang = atan2 (dy, dx);
            lastx = xw[j2-1] + maximum_extension * cos (dang);
            lasty = yw[j2-1] + maximum_extension * sin (dang);
        }

      /*
       * Check if the extensions cross any other center lines.
       * Also, add the extended endpoints to the work arrays.
       */
        if (first_is_inside == 1) {
            istat = CheckFcenterCrossing (
                i,
                0,
                &xw[j1],
                &yw[j1],
                &firstx,
                &firsty,
                dtiny,
                &xp,
                &yp
            );
            if (istat == 1) {
                firstx = xp;
                firsty = yp;
            }
            xw[j1-1] = firstx;
            yw[j1-1] = firsty;
            zw[j1-1] = firstz;
            j1--;
        }

        if (last_is_inside) {
            istat = CheckFcenterCrossing (
                i,
                0,
                &xw[j2-1],
                &yw[j2-1],
                &lastx,
                &lasty,
                dtiny,
                &xp,
                &yp
            );
            if (istat == 1) {
                lastx = xp;
                lasty = yp;
            }

            xw[j2] = lastx;
            yw[j2] = lasty;
            zw[j2] = lastz;
            j2++;
        }

      /*
       * Update the centerline object with the work arrays.
       */
        n = j2;
        csw_Free (fp->x);
        fp->x = NULL;
        fp->y = NULL;
        fp->z = NULL;
        fp->x = (double *)csw_Malloc (3 * n * sizeof(double));
        if (fp->x == NULL) {
            csw_Free (xw);
            return -1;
        }
        fp->y = fp->x + n;
        fp->z = fp->y + n;
        nout = n - j1;

        memcpy (fp->x, xw + j1, nout * sizeof(double));
        memcpy (fp->y, yw + j1, nout * sizeof(double));
        memcpy (fp->z, zw + j1, nout * sizeof(double));

        fp->npts = nout;
        fp->j1 = 0;
        fp->j2 = nout;
        fp->j1orig = j1orig - j1;
        fp->j2orig = j2orig - j1;

    }  /* end of loop extending each centerline */

    csw_Free (xw);

    return 1;

}  /* end of private ExtendCenterlines function */



/*--------------------------------------------------------------------*/

/*
 * Check if the specified vector intersects any other fault center line
 * being used to split the patch.  If no intersection is found, zero is
 * returned.  If an intersection is found, 1 is returned and the intersection
 * point is returned in xpoint, ypoint.  If more than one intersection is
 * found, the closest intersection to the x1, y1 point is returned.
 */

int PATCHSplit::CheckFcenterCrossing (
    int                source_index,
    int                end_point_flag,
    double             *x1p,
    double             *y1p,
    double             *x2p,
    double             *y2p,
    double             tiny,
    double             *xpoint,
    double             *ypoint
)
{
    int        j, istat, ido, done, origcline;
    double     x1, y1, x2, y2;
    double     x1out, y1out, x2out, y2out;
    double     *x, *y;
    double     xt, yt;
    double     dist, dx, dy, dmin;
    double     xpmin, ypmin;
    FAultCenterline    *fp;

    x1 = *x1p;
    y1 = *y1p;
    x2 = *x2p;
    y2 = *y2p;
    x1out = x1;
    y1out = y1;
    x2out = x2;
    y2out = y2;

/*
 * Initialize output in case no intersection is found.
 */
    dmin = 1.e30;
    *xpoint = 1.e30;
    *ypoint = 1.e30;
    xpmin = x2;
    ypmin = y2;

/*
 * No intersection is possible with a "zero" length vector.
 */
    dx = x2 - x1;
    dy = y2 - y1;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);

    if (dist <= tiny) {
        return 0;
    }

    origcline = Fcenter[source_index].origcline;

/*
 * Check all centerlines except the one that the vector
 * belongs to (the source_index centerline)
 */
    for (ido=0; ido<NumFcenter; ido++) {

        if (ido == source_index) {
            continue;
        }

        fp = Fcenter + ido;

        if (fp->npts < 2) {
            continue;
        }

        if (fp->origcline == origcline) {
            continue;
        }

        x = fp->x;
        y = fp->y;

        for (j=fp->j1; j<fp->j2-1; j++) {

          /*
           * Make sure the same intersection point isn't found many times.
           */
            if (ido == LastCrossingLine  &&  j == LastCrossingSegment) {
                continue;
            }

          /*
           * ply_segint returns zero if the intersection is
           * inside the limits of each input segment.
           */
            istat = ply_utils_obj.ply_segint (x1, y1, x2, y2,
                                x[j], y[j], x[j+1], y[j+1],
                                &xt, &yt);
            if (_graze2 (xt, yt, LastXint, LastYint) == 1) {
                continue;
            }

            if (istat == 0) {

                if (end_point_flag == 1  ||  end_point_flag == 3) {
                    istat = _graze (x1, y1, xt, yt);
                    if (istat == 1) {
                        continue;
                    }
                }

                if (end_point_flag == 2  ||  end_point_flag == 3) {
                    istat = _graze (x2, y2, xt, yt);
                    if (istat == 1) {
                        continue;
                    }
                }

                done = 0;

            /*
             * If the intersection grazes an end point of the line
             * being split in the calling function, use it.
             */
                if (end_point_flag == -1) {
                    istat = _graze (x1, y1, xt, yt);
                    if (istat == 1) {
                        done = 1;
                        xt = (x1 + xt) / 2.0;
                        yt = (y1 + yt) / 2.0;
                        x1out = xt;
                        y1out = yt;
                        dx = xt - x1;
                        dy = yt - y1;
                        dist = dx * dx + dy * dy;
                        dist = sqrt (dist);
                        if (dist < dmin) {
                            dmin = dist;
                            xpmin = xt;
                            ypmin = yt;
                            LastCrossingLine = ido;
                            LastCrossingSegment = j;
                        }
                    }
                }
                if (end_point_flag == -2) {
                    istat = _graze (x2, y2, xt, yt);
                    if (istat == 1) {
                        done = 1;
                        xt = (x2 + xt) / 2.0;
                        yt = (y2 + yt) / 2.0;
                        x2out = xt;
                        y2out = yt;
                        dx = xt - x1;
                        dy = yt - y1;
                        dist = dx * dx + dy * dy;
                        dist = sqrt (dist);
                        if (dist < dmin) {
                            dmin = dist;
                            xpmin = xt;
                            ypmin = yt;
                            LastCrossingLine = ido;
                            LastCrossingSegment = j;
                        }
                    }
                }

             /*
              * If the intersection point grazes an end point of the
              * line being checked, use it.
              */
                if (j == fp->j1) {
                    istat = _graze (x[j], y[j], xt, yt);
                    if (istat == 1) {
                        done = 1;
                        xt = (x[j] + xt) / 2.0;
                        yt = (y[j] + yt) / 2.0;
                        x[j] = xt;
                        y[j] = yt;
                        dx = xt - x1;
                        dy = yt - y1;
                        dist = dx * dx + dy * dy;
                        dist = sqrt (dist);
                        if (dist < dmin) {
                            dmin = dist;
                            xpmin = xt;
                            ypmin = yt;
                            LastCrossingLine = ido;
                            LastCrossingSegment = j;
                        }
                    }
                }
                if (j == fp->j2-2) {
                    istat = _graze (x[j+1], y[j+1], xt, yt);
                    if (istat == 1) {
                        done = 1;
                        xt = (x[j+1] + xt) / 2.0;
                        yt = (y[j+1] + yt) / 2.0;
                        x[j+1] = xt;
                        y[j+1] = yt;
                        dx = xt - x1;
                        dy = yt - y1;
                        dist = dx * dx + dy * dy;
                        dist = sqrt (dist);
                        if (dist < dmin) {
                            dmin = dist;
                            xpmin = xt;
                            ypmin = yt;
                            LastCrossingLine = ido;
                            LastCrossingSegment = j;
                        }
                    }
                }

              /*
               * Check if this is the closest intersection to x1, y1.
               */
                if (done == 0) {
                    dx = xt - x1;
                    dy = yt - y1;
                    dist = dx * dx + dy * dy;
                    dist = sqrt (dist);
                    if (dist < dmin) {
                        dmin = dist;
                        xpmin = xt;
                        ypmin = yt;
                        LastCrossingLine = ido;
                        LastCrossingSegment = j;
                    }
                }
            }

          /*
           * If the intersection point is on the x1,y1 to x2,y2 segment
           * and it grazes the end point of the candidate line, accept
           * it as an intersection.
           */
            else if (OnSegment (x1, y1, x2, y2, xt, yt) == 1) {

            /*
             * The intersection also needs to graze the segment from the
             * line being scanned for crossing.
             */
                istat = _graze (x[j], y[j], xt, yt);
                if (istat != 1) continue;
                istat = _graze (x[j+1], y[j+1], xt, yt);
                if (istat != 1) continue;

            /*
             * If the intersection grazes an end point of the line
             * being split in the calling function, use it.
             */
                if (end_point_flag == -1) {
                    istat = _graze (x1, y1, xt, yt);
                    if (istat == 1) {
                        xt = (x1 + xt) / 2.0;
                        yt = (y1 + yt) / 2.0;
                        x1out = xt;
                        y1out = yt;
                        dx = xt - x1;
                        dy = yt - y1;
                        dist = dx * dx + dy * dy;
                        dist = sqrt (dist);
                        if (dist < dmin) {
                            dmin = dist;
                            xpmin = xt;
                            ypmin = yt;
                            LastCrossingLine = ido;
                            LastCrossingSegment = j;
                        }
                    }
                }
                if (end_point_flag == -2) {
                    istat = _graze (x2, y2, xt, yt);
                    if (istat == 1) {
                        xt = (x2 + xt) / 2.0;
                        yt = (y2 + yt) / 2.0;
                        x2out = xt;
                        y2out = yt;
                        dx = xt - x1;
                        dy = yt - y1;
                        dist = dx * dx + dy * dy;
                        dist = sqrt (dist);
                        if (dist < dmin) {
                            dmin = dist;
                            xpmin = xt;
                            ypmin = yt;
                            LastCrossingLine = ido;
                            LastCrossingSegment = j;
                        }
                    }
                }

             /*
              * If the intersection point grazes an end point of the
              * line being checked, use it.
              */
                if (j == fp->j1) {
                    istat = _graze (x[j], y[j], xt, yt);
                    if (istat == 1) {
                        xt = (x[j] + xt) / 2.0;
                        yt = (y[j] + yt) / 2.0;
                        x[j] = xt;
                        y[j] = yt;
                        dx = xt - x1;
                        dy = yt - y1;
                        dist = dx * dx + dy * dy;
                        dist = sqrt (dist);
                        if (dist < dmin) {
                            dmin = dist;
                            xpmin = xt;
                            ypmin = yt;
                            LastCrossingLine = ido;
                            LastCrossingSegment = j;
                        }
                    }
                }
                if (j == fp->j2-2) {
                    istat = _graze (x[j+1], y[j+1], xt, yt);
                    if (istat == 1) {
                        xt = (x[j+1] + xt) / 2.0;
                        yt = (y[j+1] + yt) / 2.0;
                        x[j+1] = xt;
                        y[j+1] = yt;
                        dx = xt - x1;
                        dy = yt - y1;
                        dist = dx * dx + dy * dy;
                        dist = sqrt (dist);
                        if (dist < dmin) {
                            dmin = dist;
                            xpmin = xt;
                            ypmin = yt;
                            LastCrossingLine = ido;
                            LastCrossingSegment = j;
                        }
                    }
                }
            }

        }

    }

    if (dmin < 1.e20) {
        *xpoint = xpmin;
        *ypoint = ypmin;
        LastXint = xpmin;
        LastYint = ypmin;
        *x1p = x1out;
        *y1p = y1out;
        *x2p = x2out;
        *y2p = y2out;
        return 1;
    }

    return 0;

}  /* end of private CheckFcenterCrossing function */



/*----------------------------------------------------------------------------------*/

/*
 * This is only called from internal functions where the first and last end
 * points have been set to valid values.  It returns with no work if either
 * endpoint is invalid or if any part of the input structure is NULL.
 */
void PATCHSplit::InterpolateClineZValues (FAultCenterline *fp)
{
    int             i, j, npts, j1, j2;
    double          *x, *y, *z, dx, dy, dz, dist, dtot, z1, z2;

    if (fp == NULL) {
        return;
    }

    npts = fp->npts;
    if (npts < 3) {
        return;
    }

    x = fp->x;
    y = fp->y;
    z = fp->z;

    if (x == NULL  ||  y == NULL  ||  z == NULL) {
        return;
    }

    if (z[0] > 1.e20  ||  z[npts-1] > 1.e20) {
        return;
    }

/*
 * Set the first valid interval endpoint to the first point.
 */
    i = 1;
    j1 = 1;

/*
 * Find other valid points.  If they are not adjacent to the first valid
 * point, interpolate the invalid points in the gap.
 */
    while (i < npts) {

    /*
     * If this point is valid, mark it as the interval end point.
     * If not valid, increment the index and continue.
     */
        if (z[i] < 1.e20) {
            j2 = i;
        }
        else {
            i++;
            continue;
        }

    /*
     * If the interval end point is adjacent to the interval start point,
     * make this point the start point for the next interval and increment the
     * index.
     */
        if (j2 == j1+1) {
            j1 = j2;
            i++;
            continue;
        }

    /*
     * If there was a gap, interpolate inside it linearly using distance
     * from the interval start point.
     */
        z1 = z[j1];
        z2 = z[j2];

    /*
     * Find the total in line distance across the gap.
     */
        dtot = 0.0;
        for (j=j1; j<j2; j++) {
            dx = x[j+1] - x[j];
            dy = y[j+1] - y[j];
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            dtot += dist;
        }

        if (dtot <= 0.0) {
            dz = 0.0;
        }
        else {
            dz = (z2 - z1) / dtot;
        }

    /*
     * Interpolate based on total distance from the start point.
     */
        for (j=j1; j<j2; j++) {
            dx = x[j+1] - x[j];
            dy = y[j+1] - y[j];
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            dtot += dist;
            z[j] = z1 + dtot * dz;
        }

    /*
     * Make the end point the next interval start point and increment the index.
     */
        j1 = j2;
        i++;

    }  /* end of while loop */

    return;

}


/*----------------------------------------------------------------------------------*/

/*
  Generate a new set of centerlines where none of the lines cross each other.  Very
  short lines are deleted at the end of the process.
*/

int PATCHSplit::FixCrossingCenterlines (int shiftflag)
{
    int             i, j, n, n1, n2, n3, nwork, ntot, ido, istat;
    int             nflist, end_point_flag;
    int             j1, j2, npts, nseg;
    double          dt1, dt2, dtiny, zmid, pct, dtot;
    double          x1, y1, z1,
                    x2, y2, z2,
                    dx, dy;
    double          xint, yint, dist, tiny;
    double          xt1, yt1, xt2, yt2;
    double          *xw, *yw, *zw, *altzw;
    int             *iw;
    FAultCenterline *fp, **flist, *fpt;
    int             maxlist, iw1, iw2;

    int             seg1[1000], seg2[1000];

    if (Xmin >= Xmax) {
        return -1;
    }

    tiny = (Xmax - Xmin + Ymax - Ymin) / 400.0;
    dtiny = tiny;

    GrazeTiny = tiny * 1.0;

    ntot = 0;

    for (ido=0; ido<NumFcenter; ido++) {
        fp = Fcenter + ido;
        if (fp->npts > ntot) {
            ntot = fp->npts;
        }
    }

    ntot *= 100;
    if (ntot < NumFcenter) ntot = NumFcenter;
    if (ntot < 1000) ntot = 1000;

    xw = (double *)csw_Malloc (ntot * 5 * sizeof(double));
    if (xw == NULL) {
        return -1;
    }
    yw = xw + ntot;
    zw = yw + ntot;
    altzw = zw + ntot;
    iw = (int *) (altzw + ntot);

    n = NumFcenter * NumFcenter;
    if (n < 100) {
        n = 100;
    }
    maxlist = n;
    flist = (FAultCenterline **)csw_Calloc (n * sizeof(FAultCenterline *));
    if (flist == NULL) {
        csw_Free (xw);
        return -1;
    }

/*
 * Check each centerline for intersections with other centerlines.  If the
 * line intersects, break it into segments added to the new centerline list.
 */
    nflist = 0;
    for (ido=0; ido<NumFcenter; ido++) {

        LastXint = 1.e30;
        LastYint = 1.e30;

        fp = Fcenter + ido;
        if (fp->npts < 2) {
            continue;
        }
        n = 0;

        xw[n] = fp->x[0];
        yw[n] = fp->y[0];
        zw[n] = fp->z[0];
        altzw[n] = zw[n];
        iw[n] = 1;
        n++;

      /*
       * Check each segment for crossing another centerline.  Insert
       * the intersection points along with the original points into
       * the work arrays.
       */
        for (i=0; i<fp->npts-1; i++) {

            x1 = fp->x[i];
            y1 = fp->y[i];
            x2 = fp->x[i+1];
            y2 = fp->y[i+1];
            zmid = (fp->z[i] + fp->z[i+1]) / 2.0;
            end_point_flag = 0;
            if (i == 0) {
                end_point_flag = -1;
            }
            if (i == fp->npts-2) {
                end_point_flag = -2;
            }

            LastCrossingLine = -1;
            LastCrossingSegment = -1;
            istat = CheckFcenterCrossing (
                ido,
                end_point_flag,
                &x1,
                &y1,
                &x2,
                &y2,
                tiny,
                &xint,
                &yint
            );

            fp->x[i] = x1;
            fp->y[i] = y1;
            fp->x[i+1] = x2;
            fp->y[i+1] = y2;

            while (istat == 1) {
                xw[n] = xint;
                yw[n] = yint;
                zw[n] = 1.e30;
                altzw[n] = zmid;
                iw[n] = 2;
                n++;
                x1 = xint;
                y1 = yint;
                istat = CheckFcenterCrossing (
                    ido,
                    1,
                    &x1,
                    &y1,
                    &x2,
                    &y2,
                    tiny,
                    &xint,
                    &yint
                );
                fp->x[i+1] = x2;
                fp->y[i+1] = y2;
            }

            if (_graze2 (x2, y2, xw[n-1], yw[n-1]) == 0  ||
                end_point_flag == -2) {
                iw[n] = 0;
                xw[n] = x2;
                yw[n] = y2;
                zw[n] = fp->z[i+1];
                altzw[n] = zw[n];
                iw[n] = 0;
                n++;
            }

            LastCrossingLine = -1;
            LastCrossingSegment = -1;

        }

        LastXint = 1.e30;
        LastYint = 1.e30;

        nwork = n;
        if (iw[n-1] == 0) {
            iw[n-1] = 1;
        }

        iw1 = iw[0];
        iw2 = iw[n-1];

    /*
     * Move the work arrays back into the centerline struct so any
     * new points are incorporated into subsequent intersection checks
     * for other lines.
     */
        fp->j1 = 0;
        fp->j2 = nwork;
        fp->npts = nwork;
        csw_Free (fp->x);
        fp->x = (double *)csw_Malloc (nwork * 6 * sizeof(double));
        if (fp->x == NULL) {
            csw_Free (xw);
            csw_Free_clist (flist, nflist);
            return -1;
        }
        fp->y = fp->x + nwork;
        fp->z = fp->y + nwork;

        memcpy (fp->x, xw, nwork * sizeof(double));
        memcpy (fp->y, yw, nwork * sizeof(double));
        memcpy (fp->z, zw, nwork * sizeof(double));

    /*
     * The work arrays are now filled with point and intersect data
     * for this centerline.  Separate this into shorter centerline
     * structures which do not intersect any other centerlines.
     */

    /*
     * Count the number of segments.  If more than 1000, bail out.
     */
        nseg = 0;
        for (i=0; i<nwork; i++) {
            if (iw[i] != 0) {
                nseg++;
            }
        }
        if (nseg >= 1000) {
            csw_Free (xw);
            csw_Free_clist (flist, nflist);
            return -1;
        }

    /*
     * Set up the start and end work indices for each segment.
     */
        nseg = 0;
        seg1[0] = 0;
        for (i=1; i<nwork; i++) {
            if (iw[i] != 0) {
                seg2[nseg] = i;
                nseg++;
                seg1[nseg] = i;
            }
        }
        seg2[nseg] = n - 1;

    /*
     * Calculate the total length of all segments.  This is used
     * to help discard very short segments.
     */
        dtot = 0.0;
        for (i=0; i<nseg; i++) {
            dist = 0.0;
            j1 = seg1[i];
            j2 = seg2[i];
            for (j=j1; j<j2; j++) {
                dx = xw[j+1] - xw[j];
                dy = yw[j+1] - yw[j];
                dt1 = dx * dx + dy * dy;
                dt1 = sqrt (dt1);
                dist += dt1;
            }

            dtot += dist;
        }

    /*
     * Put each segment that merits it into the new centerline list.
     */
        for (i=0; i<nseg; i++) {

        /*
         * If the total segment length is very short, do not use it.
         */
            dist = 0.0;
            dt2 = 0.0;
            if (i == 0  || i == nseg-1) {
                dt2 = dtiny * 5.0;
                if (dt2 < LineTrimFraction * dtot) dt2 = LineTrimFraction * dtot;
            }
            if (NumFcenter == 1  ||  shiftflag == 0) {
                dt2 /= 10.0;
            }
            j1 = seg1[i];
            j2 = seg2[i];
            for (j=j1; j<j2; j++) {
                dx = xw[j+1] - xw[j];
                dy = yw[j+1] - yw[j];
                dt1 = dx * dx + dy * dy;
                dt1 = sqrt (dt1);
                dist += dt1;
            }

            if (dist <= dt2) {
                continue;
            }

        /*
         * If there are too many centerlines, bail out.
         */
            if (nflist >= maxlist) {
                break;
            }

        /*
         * Allocate and fill in a new centerline.
         */
            fpt = (FAultCenterline *)csw_Calloc (sizeof(FAultCenterline));
            if (fpt == NULL) {
                csw_Free (xw);
                csw_Free_clist (flist, n);
                return -1;
            }

            npts = j2 - j1 + 1;

            fpt->x = (double *)csw_Malloc (npts * 3 * sizeof(double));
            if (fpt->x == NULL) {
                csw_Free (xw);
                csw_Free (fpt);
                csw_Free_clist (flist, n);
                return -1;
            }
            fpt->y = fpt->x + npts;
            fpt->z = fpt->y + npts;

            memcpy (fpt->x, xw+j1, npts * sizeof(double));
            memcpy (fpt->y, yw+j1, npts * sizeof(double));
            memcpy (fpt->z, zw+j1, npts * sizeof(double));

            fpt->npts = npts;
            fpt->flag = fp->flag;
            fpt->fault_id = fp->fault_id;
            fpt->j1 = 0;
            fpt->j2 = npts;

            fpt->lock1 = 0;
            fpt->lock2 = 0;

            fpt->left_contact = -1;
            fpt->right_contact = -1;

            fpt->origcline = ido;

            if (nseg > 1) {
                if (i == 0) {
                    fpt->lock1 = 0;
                    fpt->lock2 = 1;
                    if (iw[j1] == 2) fpt->lock1 = 1;
                }
                else if (i == nseg - 1) {
                    fpt->lock1 = 1;
                    fpt->lock2 = 0;
                    if (iw[j2] == 2) fpt->lock2 = 1;
                }
                else {
                    fpt->lock1 = 1;
                    fpt->lock2 = 1;
                }
            }

            if (iw1 == 2) {
                fpt->lock1 = 1;
            }
            if (iw2 == 2) {
                fpt->lock2 = 1;
            }

        /*
         * Interpolate the first z value if needed.
         */
            if (fpt->z[0] > 1.e20) {
                z1 = fpt->z[1];
                if (npts < 3) {
                    if (z1 < 1.e20) {
                        fpt->z[0] = z1;
                    }
                    else {
                        fpt->z[0] = altzw[j1];
                    }
                }
                else {
                    z2 = fpt->z[2];
                    if (z1 > 1.e20  &&  z2 > 1.e20) {
                        fpt->z[0] = altzw[j1];
                    }
                    else if (z1 > 1.e20) {
                        fpt->z[0] = z2;
                    }
                    else if (z2 > 1.e20) {
                        fpt->z[0] = z1;
                    }
                    else {
                        xt1 = fpt->x[1];
                        yt1 = fpt->y[1];
                        xt2 = fpt->x[2];
                        yt2 = fpt->y[2];
                        dx = xt2 - xt1;
                        dy = yt2 - yt1;
                        dt1 = dx * dx + dy * dy;
                        dt1 = sqrt (dt1);
                        if (dt1 <= 0.0) {
                            fpt->z[0] = z1;
                        }
                        else {
                            dx = xt1 - fpt->x[0];
                            dy = yt1 - fpt->y[0];
                            dt2 = dx * dx + dy * dy;
                            dt2 = sqrt (dt2);
                            pct = dt2 / dt1;
                            if (pct > 10.0) pct = 10.0;
                            fpt->z[0] = z1 - (z2 - z1) * pct;
                        }
                    }
                }
            }

        /*
         * Interpolate the last z value if needed.
         */
            n1 = npts - 1;
            n2 = npts - 2;
            n3 = npts - 3;
            if (fpt->z[n1] > 1.e20) {
                z1 = fpt->z[n2];
                if (npts < 3) {
                    if (z1 < 1.e20) {
                        fpt->z[n1] = z1;
                    }
                    else {
                        fpt->z[n1] = altzw[j2];
                    }
                }
                else {
                    z2 = fpt->z[n3];
                    if (z1 > 1.e20  &&  z2 > 1.e20) {
                        fpt->z[n1] = altzw[j2];
                    }
                    else if (z1 > 1.e20) {
                        fpt->z[n1] = z2;
                    }
                    else if (z2 > 1.e20) {
                        fpt->z[n1] = z1;
                    }
                    else {
                        xt1 = fpt->x[n2];
                        yt1 = fpt->y[n2];
                        xt2 = fpt->x[n3];
                        yt2 = fpt->y[n3];
                        dx = xt2 - xt1;
                        dy = yt2 - yt1;
                        dt1 = dx * dx + dy * dy;
                        dt1 = sqrt (dt1);
                        if (dt1 <= 0.0) {
                            fpt->z[n1] = z1;
                        }
                        else {
                            dx = xt1 - fpt->x[n1];
                            dy = yt1 - fpt->y[n1];
                            dt2 = dx * dx + dy * dy;
                            dt2 = sqrt (dt2);
                            pct = dt2 / dt1;
                            if (pct > 10.0) pct = 10.0;
                            fpt->z[n1] = z1 - (z2 - z1) * pct;
                        }
                    }
                }
            }

            InterpolateClineZValues (fpt);

            flist[nflist] = fpt;
            nflist++;
            fpt = NULL;

        }  /* end of loop that puts segments into the new list */

    }  /* end of ido loop */

    csw_Free (xw);

/*
 * csw_Free the old centerline array and build a new one using
 * the new list populated here.
 */
    if (Fcenter != NULL) {
        for (i=0; i<NumFcenter; i++) {
            fp = Fcenter + i;
            csw_Free (fp->x);
        }
    }
    csw_Free (Fcenter);
    Fcenter = NULL;
    NumFcenter = 0;
    MaxFcenter = 0;

    Fcenter = (FAultCenterline *)csw_Calloc (nflist * sizeof(FAultCenterline));
    if (Fcenter == NULL) {
        csw_Free_clist (flist, nflist);
        return -1;
    }

    for (i=0; i<nflist; i++) {
        memcpy (Fcenter+i, flist[i], sizeof(FAultCenterline));
    }

    NumFcenter = nflist;
    MaxFcenter = nflist;

    for (i=0; i<nflist; i++) {
        csw_Free (flist[i]);
    }

    csw_Free (flist);

    return 1;

}



/*--------------------------------------------------------------------------------------*/

int PATCHSplit::BuildWorkPolygons (void)
{
    double            *xw, *yw, *xlines, *ylines;
    double            *xp, dx, dy, ang, sang, cang, tiny;
    double            xt2, yt2;
    int               *ilines, *iwout, npoly;
    int               i, j, n1, n2, istat, nlines, maxpts, maxpoly;
    int               nlast;
    FAultCenterline   *fp;
    WOrkPoly          *wp1;

/*
 * Allocate workspace for lines and polygon output.
 */
    n1 = 5;
    for (i=0; i<NumFcenter; i++) {
        n1 += Fcenter[i].j2 - Fcenter[i].j1;
    }
    n1 += OrigNBorder;
    n1 *= 2;
    n2 = n1 * 10;
    if (n2 < 1000) n2 = 1000;

    xlines = (double *)csw_Malloc ((n1 * 2 + n2 * 2) * sizeof(double));
    if (xlines == NULL) {
        return -1;
    }
    ylines = xlines + n1;
    xw = ylines + n1;
    yw = ylines + n2;

    maxpts = n2;

    n1 = NumFcenter * 2;
    ilines = (int *)csw_Malloc (n1 * 2 * sizeof(int));
    if (ilines == NULL) {
        csw_Free (xlines);
        return -1;
    }
    iwout = ilines + n1;
    maxpoly = n1;

/*
 * Copy centerline coordinates into the line workspace.
 */
    tiny = (Xmax - Xmin + Ymax - Ymin) / 4000.0;
    n1 = 0;
    n2 = 0;
    for (i=0; i<NumFcenter; i++) {
        fp = Fcenter + i;
        ilines[i] = fp->j2 - fp->j1;
        nlast = n2;
        for (j=fp->j1; j<fp->j2; j++) {
            xlines[n2] = fp->x[j];
            ylines[n2] = fp->y[j];
            n2++;
        }
        dx = xlines[nlast] - xlines[nlast+1];
        dy = ylines[nlast] - ylines[nlast+1];
        ang = atan2 (dy, dx);
        cang = cos (ang);
        sang = sin (ang);
        xt2 = xlines[nlast] + tiny * cang;
        yt2 = ylines[nlast] + tiny * sang;

        xlines[nlast] = xt2;
        ylines[nlast] = yt2;
        nlast = n2 - 1;
        dx = xlines[nlast] - xlines[nlast-1];
        dy = ylines[nlast] - ylines[nlast-1];
        ang = atan2 (dy, dx);
        cang = cos (ang);
        sang = sin (ang);
        xt2 = xlines[nlast] + tiny * cang;
        yt2 = ylines[nlast] + tiny * sang;
        xlines[nlast] = xt2;
        ylines[nlast] = yt2;
    }

    nlines = NumFcenter;

/*
 * Copy the original surface border to the line workspace.
 */
    if (OrigXBorder  &&  OrigNBorder > 1) {
        memcpy (xlines+n2, OrigXBorder, OrigNBorder * sizeof(double));
        memcpy (ylines+n2, OrigYBorder, OrigNBorder * sizeof(double));
        ilines[nlines] = OrigNBorder;
        nlines++;
    }

    else {
/*
 * If no original border is available,
 * copy the border box into the line workspace.
 */
        ilines[nlines] = 5;
        nlines++;
        xlines[n2] = BorderXmin;
        ylines[n2] = BorderYmin;
        n2++;
        xlines[n2] = BorderXmax;
        ylines[n2] = BorderYmin;
        n2++;
        xlines[n2] = BorderXmax;
        ylines[n2] = BorderYmax;
        n2++;
        xlines[n2] = BorderXmin;
        ylines[n2] = BorderYmax;
        n2++;
        xlines[n2] = BorderXmin;
        ylines[n2] = BorderYmin;
        n2++;
    }

    int do_write;
    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        char fname[200];
        int     icomp[200];
        for (i=0; i<nlines; i++) {
            icomp[i] = 1;
        }
        sprintf (fname, "pre_build_poly.xyz");
        grdapi_ptr->grd_WriteLines (
          xlines, ylines, NULL,
          nlines, icomp, ilines,
          fname);
    }

/*
 * Calculate polygons for the set of lines.
 */
    istat = grdapi_ptr->ply_BuildPolygonsFromLines (
        xlines,
        ylines,
        nlines,
        ilines,
        xw,
        yw,
        &npoly,
        iwout,
        maxpts,
        maxpoly
    );
    if (istat == -1) {
        csw_Free (xlines);
        csw_Free (ilines);
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        char fname[200];
        int     icomp[200];
        for (i=0; i<npoly; i++) {
            icomp[i] = 1;
        }
        sprintf (fname, "post_build_poly.xyz");
        grdapi_ptr->grd_WriteLines (
          xw, yw, NULL,
          npoly, icomp, iwout,
          fname);
    }

/*
 * Allocate space for the private WorkPoly array.
 */
    csw_Free_work_polys ();
    WorkPolyList = NULL;

    WorkPolyList = (WOrkPoly *)csw_Calloc (npoly * sizeof(WOrkPoly));
    if (WorkPolyList == NULL) {
        csw_Free (xlines);
        csw_Free (ilines);
        return -1;
    }
    NumWorkPolyList = npoly;
    MaxWorkPolyList = npoly;

/*
 * copy work data into individual work polygon structures.
 */
    n1 = 0;
    for (i=0; i<npoly; i++) {
        n2 = iwout[i];
        wp1 = WorkPolyList + i;
        wp1->np = n2;
        xp = (double *)csw_Malloc (n2 * 3 * sizeof(double));
        if (xp == NULL) {
            csw_Free_work_polys();
            csw_Free (xlines);
            csw_Free (ilines);
            return -1;
        }
        wp1->xp = xp;
        wp1->yp = xp + n2;
        wp1->zp = wp1->yp + n2;

        memcpy (wp1->xp, xw+n1, n2 * sizeof(double));
        memcpy (wp1->yp, yw+n1, n2 * sizeof(double));
        for (j=0; j<n2; j++) {
            wp1->zp[j] = 1.e30;
        }

        n1 += n2;
    }

/*
 * Any centerlines that are inside a work polygon
 * should be added to the work polygon's minor line list.
 */
    istat = AssignMinorLines (xlines, ylines, nlines, ilines);
    if (istat == -1) {
        csw_Free_work_polys ();
        return -1;
    }

    if (DebugState != 0) {
        for (i=0; i<npoly; i++) {
            wp1 = WorkPolyList + i;
            jni_call_add_work_poly_method (
                wp1->x, wp1->y, wp1->z, wp1->npts);
        }
    }

    NumWorkPolyList = NumWorkPolyList;
    MaxWorkPolyList = MaxWorkPolyList;

    return 1;

}

void PATCHSplit::csw_Free_work_polys (void)
{
    int           i;

    if (WorkPolyList == NULL) {
        return;
    }

    if (NumWorkPolyList < 1) {
        return;
    }

    for (i=0; i<NumWorkPolyList; i++) {
        csw_Free (WorkPolyList[i].x);
        csw_Free (WorkPolyList[i].grid);
        csw_Free (WorkPolyList[i].xp);
    }

    csw_Free (WorkPolyList);
    WorkPolyList = NULL;
    NumWorkPolyList = 0;
    MaxWorkPolyList = 0;

    return;
}


/*----------------------------------------------------------------------------*/

/*
 * Calculate the perpendicular distance from the point (xt, yt) to the
 * closest point on the specified closed line.
 */
double PATCHSplit::BorderDistance (
    double *x,
    double *y,
    int npts,
    double xt,
    double yt)

{
    double         xp, yp, dist, dmin;
    int            i, istat;

  /*
   * Too few points, return zero.
   */
    if (npts < 4) {
        return 0.0;
    }

    dmin = 1.e30;

    for (i=0; i<npts-1; i++) {

        istat = gpf_perpintpoint2 (
            x[i],
            y[i],
            x[i+1],
            y[i+1],
            xt,
            yt,
            &xp,
            &yp);

      /*
       * Only use if the point is inside the border segment.
       */
        if (istat != 1) {
            continue;
        }

        gpf_calcdistance2 (
            xt,
            yt,
            xp,
            yp,
            &dist);

        if (dist < dmin) {
            BorderXint = xp;
            BorderYint = yp;
            dmin = dist;
        }

    }

  /*
   * return zero if no points were used.
   */
    if (dmin > 1.e20) {
        return 0.0;
    }

    return dmin;

}


/*---------------------------------------------------------------------------*/

/*
 * Determine which points fall into each work polygon, but not very close
 * to the polygon boundary.  This set of points is copied into arrays that are
 * part of the work polygon structure.  These points are then used to calculate
 * a coarse smooth grid that extends about 10 percent further on each side
 * than the polygon boundary.
 */
int PATCHSplit::CalcWorkPolygonSurfaces (void)
{
    int        ido, i, j, n, npoly, npts, istat;
    int        maxpts, ncol, nrow;
    CSW_F      *grid;
    double     xt, yt, zt, xmin, ymin, xmax, ymax,
               dist, dmin;
    double     *xpoly, *ypoly, *xpts, *ypts, *zpts;
    double     *xdec, *ydec, *zdec, *zdum, gspace;
    int        ndec, maxdec;
    WOrkPoly   *wp;
    BAseLine   *bp;

    double     x1, y1, x2, y2;


    /*
     * !!!! debug only
     */
    FILE       *fptr;
    char       line[200];
    int        do_write;

    if (NumPointsIn + NumLinePoints < 3) {
        return -1;
    }

    maxpts = NumPointsIn + NumLinePoints;

/*
 * Loop through each work polygon, creating a grid surface for each.
 */
    for (ido=0; ido<NumWorkPolyList; ido++) {

        wp = WorkPolyList + ido;

        xpoly = wp->xp;
        ypoly = wp->yp;
        npoly = wp->np;

    /*
     * Find the bounding box of this polygon.
     */
        xmin = 1.e30;
        ymin = 1.e30;
        xmax = -1.e30;
        ymax = -1.e30;
        for (i=0; i<npoly; i++) {
            if (xpoly[i] < xmin) xmin = xpoly[i];
            if (xpoly[i] > xmax) xmax = xpoly[i];
            if (ypoly[i] < ymin) ymin = ypoly[i];
            if (ypoly[i] > ymax) ymax = ypoly[i];
        }

    /*
     * If the bounding box is screwed up, skip this polygon.
     */
        if (xmin >= xmax  ||  ymin >= ymax) {
            continue;
        }

    /*
     * Do not use points close to a centerline in calculating the
     * grid for the work surface.
     *
     * If the points were from a mesh (set via the meshflag
     * parameter passed to ps_SetPoints), then ignore points
     * closer than 2 percent of the average dimension.  If
     * the points are not from a mesh, ignore points within
     * 1 percent.
     */
        dmin = (xmax - xmin + ymax - ymin) / 200.0;
        if (MeshFlag == 1) {
            dmin *= 4.0;
        }

        if (AverageEdgeLength > 0.0) {
            if (dmin < AverageEdgeLength * 2.0) {
                dmin = AverageEdgeLength * 2.0;
            }
        }

        if (dmin > (xmax - xmin + ymax - ymin) / 10.0) {
            dmin = (xmax - xmin + ymax - ymin) / 10.0;
        }

    /*
     * Allocate space for the points that are inside the polygon.
     */
        xpts = (double *)csw_Malloc (maxpts * 3 * sizeof(double));
        if (xpts == NULL) {
            csw_Free_work_polys ();
            return -1;
        }
        ypts = xpts + maxpts;
        zpts = ypts + maxpts;

    /*
     * Collect the points inside the polygon and not close to a centerline
     */
        n = 0;

        for (i=0; i<NumPointsIn; i++) {

            xt = XPointsIn[i];
            yt = YPointsIn[i];
            zt = ZPointsIn[i];
            if (zt > 1.e20  ||  zt < -1.e20) {
                continue;
            }
            if (xt < xmin  ||  xt > xmax  ||
                yt < ymin  ||  yt > ymax) {
                continue;
            }

            istat = ply_utils_obj.ply_point (xpoly, ypoly, npoly, xt, yt);
            if (istat != 1) {
                continue;
            }

            dist = ClineDistance (ido, xt, yt);
            if (dist < dmin) {
                continue;
            }

            dist = MinorLineDistance (wp, xt, yt);
            if (dist < dmin) {
                continue;
            }

            xpts[n] = xt;
            ypts[n] = yt;
            zpts[n] = zt;
            n++;
        }

    /*
     * Collect the line points inside the polygon but not close to the border.
     */
        for (i=0; i<NumLinesIn; i++) {

            bp = LinesIn + i;

            for (j=0; j<bp->npts; j++) {

                xt = bp->x[j];
                yt = bp->y[j];
                zt = bp->z[j];

                if (zt > 1.e20  ||  zt < -1.e20) {
                    continue;
                }
                if (xt < xmin  ||  xt > xmax  ||
                    yt < ymin  ||  yt > ymax) {
                    continue;
                }

                istat = ply_utils_obj.ply_point (xpoly, ypoly, npoly, xt, yt);
                if (istat != 1) {
                    continue;
                }

                dist = ClineDistance (ido, xt, yt);
                if (dist < dmin) {
                    continue;
                }

                dist = MinorLineDistance (wp, xt, yt);
                if (dist < dmin) {
                    continue;
                }

                xpts[n] = xt;
                ypts[n] = yt;
                zpts[n] = zt;
                n++;

            }

        }

    /*
     * !!!! debug only
     */
        do_write = csw_GetDoWrite ();
        if (do_write == 1) {
            fptr = fopen ("work_poly.xy", "wb");
            if (fptr != NULL) {
                for (i=0; i<npoly; i++) {
                    sprintf (line, "%g %g\n",
                             xpoly[i], ypoly[i]);
                    fputs (line, fptr);
                }
                fclose (fptr);
                fptr = NULL;
            }
            fptr = fopen ("work_pts.xyz", "wb");
            if (fptr != NULL) {
                for (i=0; i<n; i++) {
                    sprintf (line, "%g %g %g\n",
                             xpts[i], ypts[i], zpts[i]);
                    fputs (line, fptr);
                }
                fclose (fptr);
                fptr = NULL;
            }
        }



        npts = n;

        x1 = xmin;
        y1 = ymin;
        x2 = xmax;
        y2 = ymax;
        istat =
        grdapi_ptr->grd_RecommendedSizeFromDouble (
            xpts, ypts, npts,
            &x1, &y1, &x2, &y2,
            &ncol, &nrow);
        if (MeshFlag) {
            ncol /= 2;
            nrow /= 2;
            if (ncol < 2) ncol = 2;
            if (nrow < 2) nrow = 2;
        }

        double dxy = (Xmax - Xmin + Ymax - Ymin) / 20.0;
        x1 -= dxy;
        y1 -= dxy;
        x2 += dxy;
        y2 += dxy;

    /*
     * Save the polygon points arrays.
     */
        wp->x = xpts;
        wp->y = ypts;
        wp->z = zpts;
        wp->npts = npts;

    /*
     * If the work polygon has minor fault lines,
     * make fault structures from them for use in gridding.
     */
        FAultLineStruct    *faults, *fp;
        int                num_faults, ftype, nfp;
        faults = NULL;
        num_faults = 0;
        ftype = GRD_VERTICAL_FAULT;
        if (wp->num_minor_lines > 0  &&  wp->minor_lines != NULL) {
            faults = (FAultLineStruct *)csw_Calloc
              (wp->num_minor_lines * sizeof(FAultLineStruct));
            if (faults == NULL) {
                csw_Free_work_polys ();
                return -1;
            }
            BAseLine *bptr, *blist;
            blist = wp->minor_lines;
            for (i=0; i<wp->num_minor_lines; i++) {
                bptr = blist + i;
                istat =
                  grdapi_ptr->grd_DoubleFaultArraysToStructs (
                    bptr->x,
                    bptr->y,
                    bptr->z,
                    &bptr->npts,
                    &ftype,
                    1,
                    &fp,
                    &nfp);
                if (istat == -1) {
                    csw_Free_work_polys ();
                    grdapi_ptr->grd_FreeFaultLineStructs (faults, wp->num_minor_lines);
                    return -1;
                }
                if (nfp > 1) {
                    assert (0);
                }
                memcpy (faults+num_faults, fp, sizeof(FAultLineStruct));
                csw_Free (fp);
                fp = NULL;
                num_faults++;
            }
        }

        gspace = (y2 - y1 + x2 - x1) / (ncol + nrow - 2);
        if (AverageEdgeLength > 0.0) {
            gspace = AverageEdgeLength;
        }
        ncol = (int)((x2 - x1) / gspace + 1.5);
        nrow = (int)((y2 - y1) / gspace + 1.5);

    /*
     * Allocate the grid array.
     */
        grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (grid == NULL) {
            csw_Free_work_polys ();
            grdapi_ptr->grd_FreeFaultLineStructs (faults, wp->num_minor_lines);
            return -1;
        }

    /*
     * Calculate a grid for the points in this polygon.
     */
        istat =
        grdapi_ptr->grd_CalcGridFromDouble (
            xpts, ypts, zpts, NULL, npts,
            grid, NULL, NULL,
            ncol, nrow,
            x1, y1, x2, y2,
            faults, num_faults, NULL);
        grdapi_ptr->grd_FreeFaultLineStructs (faults, num_faults);
        faults = NULL;
        num_faults = 0;
        if (istat == -1) {
            csw_Free (grid);
            csw_Free_work_polys ();
            return -1;
        }

    /*
     * Resample the polygon at close to the grid spacing.
     */
        maxdec = (ncol + nrow) * 4;
        if (maxdec < npoly * 2) maxdec = npoly * 2;
        xdec = (double *)csw_Malloc (maxdec * 2 * sizeof(double));
        zdec = (double *)csw_Malloc (maxdec * 2 * sizeof(double));
        if (xdec == NULL  ||  zdec == NULL) {
            csw_Free (xdec);
            csw_Free (zdec);
            csw_Free (grid);
            csw_Free_work_polys ();
            return -1;
        }
        ydec = xdec + maxdec;
        zdum = zdec + maxdec;

        memset (zdec, 0, 2 * maxdec * sizeof(double));

        gspace = (x2 - x1 + y2 - y1) / (ncol + nrow - 2);

        istat =
        grdapi_ptr->grd_ResampleXYZLine (xpoly, ypoly, zdum, npoly,
                             gspace,
                             xdec, ydec, zdec, &ndec,
                             maxdec);
        csw_Free (xpoly);
        csw_Free (zdec);
        zdec = zdum = xpoly = ypoly = NULL;
        wp->xp = xdec;
        wp->yp = ydec;
        wp->np = ndec;

    /*
     * Save the grid data and geometry in the work polygon structure.
     */
        wp->grid = grid;
        wp->ncol = ncol;
        wp->nrow = nrow;
        wp->xmin = x1;
        wp->ymin = y1;
        wp->xmax = x2;
        wp->ymax = y2;

    } /* end of ido loop through work polygons */

    return 1;

}

/*------------------------------------------------------------------------------*/

int PATCHSplit::CorrectCloseToBorderLines (void)
{
    FAultCenterline   *fp;
    int               i, j, istat;
    double            bdist, dtest, dx, dy, dext;
    double            x1, y1, x2, y2, xt1, yt1;
    double            dt, pct, gsave;
    double            dist, d10, dtot, dtmax;
    double            xa1, ya1, xa2, ya2;
    double            ang, cang, sang;

    ply_utils_obj.ply_getgraze (&gsave);

    for (i=0; i<NumFcenter; i++) {

        fp = Fcenter + i;

        xa1 = fp->x[1];
        ya1 = fp->y[1];
        xa2 = fp->x[fp->npts-2];
        ya2 = fp->y[fp->npts-2];

        dtot = 0.0;
        for (j=1; j<fp->npts; j++) {
           dx = fp->x[j] - fp->x[j-1];
           dy = fp->y[j] - fp->y[j-1];
           dist = dx * dx + dy * dy;
           dist = sqrt (dist);
           dtot += dist;
        }
        dtmax = dtot;

        d10 = dtot / 10.0;

        dtot = 0.0;
        for (j=1; j<fp->npts; j++) {
           dx = fp->x[j] - fp->x[j-1];
           dy = fp->y[j] - fp->y[j-1];
           dist = dx * dx + dy * dy;
           dist = sqrt (dist);
           dtot += dist;
           if (dtot >= d10) {
               dt = dtot - d10;
               pct = dt / dist;
               pct = 1.0 - pct;
               xa1 = fp->x[j-1] + dx * pct;
               ya1 = fp->y[j-1] + dy * pct;
               break;
           }
        }

        dtot = 0.0;
        for (j=fp->npts-2; j>=0; j--) {
           dx = fp->x[j] - fp->x[j+1];
           dy = fp->y[j] - fp->y[j+1];
           dist = dx * dx + dy * dy;
           dist = sqrt (dist);
           dtot += dist;
           if (dtot >= d10) {
               dt = dtot - d10;
               pct = dt / dist;
               pct = 1.0 - pct;
               xa2 = fp->x[j+1] + dx * pct;
               ya2 = fp->y[j+1] + dy * pct;
               break;
           }
        }

        x1 = fp->x[0];
        y1 = fp->y[0];
        x2 = fp->x[fp->npts-1];
        y2 = fp->y[fp->npts-1];

        dext = dtmax / 10.0;
        dext *= 2.5;
        dtest = dtmax / 20.0;

        ply_utils_obj.ply_setgraze (dtest);

      /*
       * If the first point is close to the boundary, extend the
       * first vector to place the first point outside the boundary.
       */
        bdist = BorderDistance (
            OrigXBorder, OrigYBorder, OrigNBorder, x1, y1);
        if (bdist <= dtest) {
            dx = xa1 - x1;
            dy = ya1 - y1;
            ang = atan2 (dy, dx);
            cang = cos (ang);
            sang = sin(ang);
            xt1 = x1 - dext * cang;
            yt1 = y1 - dext * sang;
            istat = InsideBorder (xt1, yt1);
            if (istat == 1) {
                fp->x[0] = BorderXint;
                fp->y[0] = BorderYint;
            }
            else {
                fp->x[0] = xt1;
                fp->y[0] = yt1;
            }
        }

      /*
       * If the last point is close to the boundary, extend the
       * last vector to place the last point outside the boundary.
       */
        bdist = BorderDistance (
            OrigXBorder, OrigYBorder, OrigNBorder, x2, y2);
        if (bdist <= dtest) {
            dx = xa2 - x2;
            dy = ya2 - y2;
            ang = atan2 (dy, dx);
            cang = cos (ang);
            sang = sin(ang);
            xt1 = x2 - dext * cang;
            yt1 = y2 - dext * sang;
            istat = InsideBorder (xt1, yt1);
            if (istat == 1) {
                fp->x[fp->npts-1] = BorderXint;
                fp->y[fp->npts-1] = BorderYint;
            }
            else {
                fp->x[fp->npts-1] = xt1;
                fp->y[fp->npts-1] = yt1;
            }
        }

        UpdateLimits (fp->x, fp->y, fp->z, fp->npts);

    }  /* end of loop correcting close centerline endpoints */

    ply_utils_obj.ply_setgraze (gsave);

    return 1;

}  /* end of private CorrectCloseToBorderLines function */


int PATCHSplit::InsideBorder (double x, double y)
{
    int         istat;

    if (OrigXBorder == NULL  ||  OrigYBorder == NULL  ||  OrigNBorder < 3) {
        return -1;
    }

    istat = ply_utils_obj.ply_point (
        OrigXBorder,
        OrigYBorder,
        OrigNBorder,
        x,
        y);

    return istat;
}




/*----------------------------------------------------------------------------*/

/*
 * Calculate the perpendicular distance from the point (xt, yt) to the
 * closest point to a centerline that uses the specified work polygon index.
 */
double PATCHSplit::ClineDistance (
    int    index,
    double xt,
    double yt)

{
    FAultCenterline   *fcl;
    double            xp, yp, dist, dmin;
    int               ido, i, istat, npts;
    double            *x, *y;

    dmin = 1.e30;

    for (ido=0; ido<NumFcenter; ido++) {

        fcl = Fcenter + ido;

    /*
     * Skip this centerline if it doesn't use the work polygon
     * index specified.
        if (fcl->jleft < 0  ||  fcl->jright < 0) {
            continue;
        }
        if (!(fcl->jleft == index  ||  fcl->jright == index)) {
            continue;
        }
     */

    /*
     * Skip if the centerline has been processed into contact lines.
     */
        if (fcl->left_contact >= 0  ||  fcl->right_contact >= 0) {
            continue;
        }

        x = fcl->x;
        y = fcl->y;
        npts = fcl->npts;

        if (npts < 2) {
            continue;
        }

    /*
     * Find a point on the line that, when connected to the
     * target point, forms a segment perpendicular to the
     * segment of the line.  The distance between this point
     * and the target point is the distance to the line.
     */
        for (i=0; i<npts-1; i++) {

            istat = gpf_perpintpoint2 (
                x[i],
                y[i],
                x[i+1],
                y[i+1],
                xt,
                yt,
                &xp,
                &yp);

          /*
           * Ignore if the perpendicular point is outside the segment.
           */
            if (istat == 0) {
                continue;
            }

            gpf_calcdistance2 (
                xt,
                yt,
                xp,
                yp,
                &dist);

            if (dist < dmin) {
                dmin = dist;
            }
        }
    }

  /*
   * If no perpendicular point was found, check for the closest
   * segment endpoint to the target point.
   */
    if (dmin < 1.e20) {
        return dmin;
    }

    for (ido=0; ido<NumFcenter; ido++) {

        fcl = Fcenter + ido;

    /*
     * Skip this centerline if it doesn't use the work polygon
     * index specified.
     */
        if (fcl->jleft < 0  ||  fcl->jright < 0) {
            continue;
        }
        if (!(fcl->jleft == index  ||  fcl->jright == index)) {
            continue;
        }

        x = fcl->x;
        y = fcl->y;
        npts = fcl->npts;

        if (npts < 2) {
            continue;
        }

    /*
     * Find the closest centerline endpoint.
     */
        for (i=0; i<npts; i++) {

            xp = x[i];
            yp = y[i];

            gpf_calcdistance2 (
                xt,
                yt,
                xp,
                yp,
                &dist);

            if (dist < dmin) {
                dmin = dist;
            }
        }
    }

    return dmin;

}



/*-------------------------------------------------------------------*/

/*
 * Create an empty sealed model object to be filled in with input
 * faults and horizons.  On success, 1 is returned.  On failure,
 * -1 is returned.
 */
int PATCHSplit::ps_StartSealedModelDefinition (int marginPct)
  {
    double marginFrac;

    if (SModel != NULL) {
        delete SModel;
        SModel = NULL;
    }

    try {
        SModel = new SealedModel ();
    }
    catch (...) {
        SModel = NULL;
    }
    if (SModel == NULL) {
        return -1;
    }

    marginFrac = marginPct / 1000.0;
    SModel->setMarginFraction (marginFrac);

    return 1;

  }



/*-----------------------------------------------------------------*/

/*
 * Call the SealedModel methods to pad out to the model extents and
 * then seal the model without vertical boundaries.  On success, 1 is
 * returned.  On error, -1 is returned.
 */
int PATCHSplit::ps_CalcSealedModel (double avspace)
  {
    int        istat;

    if (SModel == NULL) {
        return -1;
    }

    istat =
    SModel->padModel (Xmin, Ymin, Zmin, Xmax, Ymax, Zmax, avspace);
    if (istat == -1) {
        return istat;
    }

    istat =
    SModel->sealPaddedModel ();

    SModel->writeTetgenSmeshFile ((char *)"test.smesh");

    SendBackSealedModel ();

    return istat;

  }



/*-----------------------------------------------------------------------------------*/

/*
 * Retrieve the sealed horizons, faults and intersect lines from the
 * sealed model object.  Convert them to proto patches and contact lines.
 * Then, send the proto patches back to the java side.
 */
int PATCHSplit::SendBackSealedModel (void)
{
    int         istat;

    if (SModel == NULL) {
        return -1;
    }

/*
 * Build the PatchList and ProtoContactLines list from the
 * sealed model's lists of sealed faults, sealed horizons and
 * sealed intersect lines.
 */
    istat =
      BuildProtoPatchesFromSealedModel ();
    if (istat == -1) {
        return -1;
    }

    SendBackProtoPatches ();

    return 1;
}


/*-----------------------------------------------------------------------------------*/

/*
 * Retrieve the input horizons sealed model object.  Convert them to
 * proto patches and contact lines. Then, send the proto patches back
 * to the java side.
 */
int PATCHSplit::SendBackInputModel (void)
{
    int         istat;

    if (SModel == NULL) {
        return -1;
    }

/*
 * Build the PatchList and ProtoContactLines list from the
 * sealed model's lists of input horizons.
 */
    istat =
      BuildProtoPatchesFromInputModel ();
    if (istat == -1) {
        return -1;
    }

    SendBackProtoPatches ();

    return 1;
}

/*-----------------------------------------------------------------------------------*/

/*
 * Send the proto patches and contact lines back to the java side.
 */
void PATCHSplit::SendBackProtoPatches (void)
{
    int              i, j;

    GRDVert          gvert;

    PRotoPatch       *patch;
    BOrderSegment    *bseg;
    BAseLine         *bline;

    for (i=0; i<NumPatchList; i++) {

        patch = PatchList + i;

        jni_call_start_proto_patch_method (patch->patchid);

        vert_SetBaseline (
            patch->sgpdata[0],
            patch->sgpdata[1],
            patch->sgpdata[2],
            patch->sgpdata[3],
            patch->sgpdata[4],
            patch->sgpdata[5],
            patch->sgpflag,
            gvert
        );
        vert_SendBackBaseline (gvert);
        SendBackProtoTriMesh (
                         patch->nodes,
                         patch->num_nodes,
                         patch->edges,
                         patch->num_edges,
                         patch->triangles,
                         patch->num_triangles);

        for (j=0; j<patch->numBorderList; j++) {
            bseg = patch->borderList + j;
            jni_call_add_border_segment_method (
                bseg->x,
                bseg->y,
                bseg->z,
                bseg->npts,
                bseg->type,
                bseg->direction);
        }

        for (j=0; j<patch->numLineList; j++) {
            bline = patch->lineList + j;
            jni_call_add_patch_line_method (
                bline->x,
                bline->y,
                bline->z,
                bline->npts,
                bline->flag);
        }

        jni_call_add_patch_points_method (
            patch->patchid,
            patch->x,
            patch->y,
            patch->z,
            patch->npts);

        jni_call_end_proto_patch_method (patch->patchid);

    }

    SendBackProtoPatchContactLines ();

    return;

}


/*----------------------------------------------------------------------*/

int PATCHSplit::SendBackFaultMajorMinor (void)
{
    FAultCenterline    *fp;
    int                i, id, major;

    for (i=0; i<NumFcenter; i++) {
        fp = Fcenter + i;
        id = fp->fault_id;
        major = fp->major;

        jni_call_add_fault_major_minor_method (id, major);
    }

    return 1;

}




/*----------------------------------------------------------------------*/

int PATCHSplit::SendBackProtoTriMesh
    (NOdeStruct *nodes, int num_nodes,
     EDgeStruct *edges, int num_edges,
     TRiangleStruct *triangles, int num_triangles)
{
    double         *xnode,
                   *ynode,
                   *znode;
    int            *flagnode;
    int            *n1edge,
                   *n2edge,
                   *t1edge,
                   *t2edge,
                   *flagedge;
    int            *e1tri,
                   *e2tri,
                   *e3tri,
                   *flagtri;

    int            i;
    NOdeStruct     *np;
    EDgeStruct     *ep;
    TRiangleStruct *tp;

    int            do_write;

/*
 * Check for obvious errors.
 */
    if (nodes == NULL  ||  num_nodes < 3  ||
        edges == NULL  ||  num_edges < 3  ||
        triangles == NULL  ||  num_triangles < 1) {
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        char    fname[100];
        strcpy (fname, "sendback.tri");
        grdapi_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            triangles, num_triangles,
            edges, num_edges,
            nodes, num_nodes,
            fname);
    }

/*
 * Allocate space for node arrays.
 */
    xnode = (double *)csw_Malloc (3 * num_nodes * sizeof(double));
    if (xnode == NULL) {
        return -1;
    }
    ynode = xnode + num_nodes;
    znode = ynode + num_nodes;

    flagnode = (int *)csw_Calloc (num_nodes * sizeof(int));
    if (flagnode == NULL) {
        csw_Free (xnode);
        return -1;
    }

/*
 * Allocate space for edge arrays.
 */
    n1edge = (int *)csw_Calloc (5 * num_edges * sizeof(int));
    if (n1edge == NULL) {
        csw_Free (xnode);
        csw_Free (flagnode);
        return -1;
    }
    n2edge = n1edge + num_edges;
    t1edge = n2edge + num_edges;
    t2edge = t1edge + num_edges;
    flagedge = t2edge + num_edges;

/*
 * Allocate space for triangle arrays.
 */
    e1tri = (int *)csw_Calloc (num_triangles * 4 * sizeof(int));
    if (e1tri == NULL) {
        csw_Free (xnode);
        csw_Free (flagnode);
        csw_Free (n1edge);
        return -1;
    }
    e2tri = e1tri + num_triangles;
    e3tri = e2tri + num_triangles;
    flagtri = e3tri + num_triangles;

/*
 * Copy node data to node arrays.
 */
    for (i=0; i<num_nodes; i++) {
        np = nodes + i;
        xnode[i] = np->x;
        ynode[i] = np->y;
        znode[i] = np->z;
        flagnode[i] = np->flag;
    }

/*
 * Copy edge data to edge arrays.
 */
    for (i=0; i<num_edges; i++) {
        ep = edges + i;
        n1edge[i] = ep->node1;
        n2edge[i] = ep->node2;
        t1edge[i] = ep->tri1;
        t2edge[i] = ep->tri2;
        flagedge[i] = ep->flag;
    }

/*
 * Copy triangle data to arrays.
 */
    for (i=0; i<num_triangles; i++) {
        tp = triangles + i;
        e1tri[i] = tp->edge1;
        e2tri[i] = tp->edge2;
        e3tri[i] = tp->edge3;
        flagtri[i] = tp->flag;
    }

/*
 * Send the arrays to the java side where a java TriMesh
 * object will be constructed using these data.
 */
    jni_call_add_tri_mesh_method
    (
        xnode,
        ynode,
        znode,
        flagnode,
        num_nodes,
        n1edge,
        n2edge,
        t1edge,
        t2edge,
        flagedge,
        num_edges,
        e1tri,
        e2tri,
        e3tri,
        flagtri,
        num_triangles
    );

/*
 * Free the arrays.
 */
    csw_Free (xnode);
    csw_Free (flagnode);
    csw_Free (n1edge);
    csw_Free (e1tri);

    return 1;

}

/*--------------------------------------------------------------------------------*/

void PATCHSplit::SendBackProtoPatchContactLines (void)
{
    COntactLine           *cl;
    int                   ido;

    if (ProtoContactLines == NULL  ||  NumProtoContactLines < 1) {
        return;
    }

    for (ido=0; ido<NumProtoContactLines; ido++) {
        cl = ProtoContactLines + ido;
        jni_call_add_proto_patch_contact_line_method (
            cl->x,
            cl->y,
            cl->z,
            cl->npts,
            cl->patchid1,
            cl->patchid2);
    }

    return;

}


/*--------------------------------------------------------------------------------*/

void PATCHSplit::SendBackSplitLines (void)
{
    COntactLine           *cl;
    int                   ido;

    if (SplitLines == NULL  ||  NumSplitLines < 1) {
        return;
    }

    for (ido=0; ido<NumSplitLines; ido++) {
        cl = SplitLines + ido;
        jni_call_add_split_line_method (
            cl->x,
            cl->y,
            cl->z,
            cl->npts,
            cl->patchid1,
            cl->patchid2);
    }

    return;

}



/*------------------------------------------------------------------------------------*/

int PATCHSplit::AddToProtoContactLines (
    double   *x,
    double   *y,
    double   *z,
    int      npts,
    int      id1,
    int      id2)
{
    COntactLine     *clptr;
    double          *xa, *ya, *za;

    if (npts < 2  ||  x == NULL  ||  y == NULL  ||  z == NULL) {
        return -1;
    }

/*
 * Grow the ProtoContactLines list if needed.
 */
    if (ProtoContactLines == NULL  ||
        NumProtoContactLines >= MaxProtoContactLines) {
        MaxProtoContactLines += 20;
        ProtoContactLines = (COntactLine *)csw_Realloc
            (ProtoContactLines, MaxProtoContactLines * sizeof(COntactLine));
    }

    if (ProtoContactLines == NULL) {
        return -1;
    }

    clptr = ProtoContactLines + NumProtoContactLines;
    xa = (double *)csw_Malloc (npts * 3 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + npts;
    za = ya + npts;

    memcpy (xa, x, npts * sizeof(double));
    memcpy (ya, y, npts * sizeof(double));
    memcpy (za, z, npts * sizeof(double));

    clptr->x = xa;
    clptr->y = ya;
    clptr->z = za;
    clptr->npts = npts;
    clptr->patchid1 = id1;
    clptr->patchid2 = id2;

    NumProtoContactLines++;

    return 1;

}


/*------------------------------------------------------------------------------------*/

int PATCHSplit::AddToSplitLines (
    double   *x,
    double   *y,
    double   *z,
    int      npts,
    int      id1,
    int      id2)
{
    COntactLine     *clptr;
    double          *xa, *ya, *za;

    if (npts < 2  ||  x == NULL  ||  y == NULL  ||  z == NULL) {
        return -1;
    }

/*
 * Grow the SplitLines list if needed.
 */
    if (SplitLines == NULL  ||
        NumSplitLines >= MaxSplitLines) {
        MaxSplitLines += 20;
        SplitLines = (COntactLine *)csw_Realloc
            (SplitLines, MaxSplitLines * sizeof(COntactLine));
    }

    if (SplitLines == NULL) {
        return -1;
    }

    clptr = SplitLines + NumSplitLines;
    xa = (double *)csw_Malloc (npts * 3 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + npts;
    za = ya + npts;

    memcpy (xa, x, npts * sizeof(double));
    memcpy (ya, y, npts * sizeof(double));
    memcpy (za, z, npts * sizeof(double));

    clptr->x = xa;
    clptr->y = ya;
    clptr->z = za;
    clptr->npts = npts;
    clptr->patchid1 = id1;
    clptr->patchid2 = id2;

    NumSplitLines++;

    return 1;

}



/*------------------------------------------------------------------------------------*/

void PATCHSplit::FreeProtoContactLines (void)
{
    int          i;
    COntactLine  *clptr;

    if (ProtoContactLines == NULL) {
        return;
    }

    for (i=0; i<NumProtoContactLines; i++) {
        clptr = ProtoContactLines + i;
        csw_Free (clptr->x);
    }

    csw_Free (ProtoContactLines);
    ProtoContactLines = NULL;
    NumProtoContactLines = 0;
    MaxProtoContactLines = 0;

    return;
}



/*------------------------------------------------------------------------------------*/

void PATCHSplit::FreeSplitLines (void)
{
    int          i;
    COntactLine  *clptr;

    if (SplitLines == NULL) {
        return;
    }

    for (i=0; i<NumSplitLines; i++) {
        clptr = SplitLines + i;
        csw_Free (clptr->x);
    }

    csw_Free (SplitLines);
    SplitLines = NULL;
    NumSplitLines = 0;
    MaxSplitLines = 0;

    return;
}



/*------------------------------------------------------------------------------------*/

int PATCHSplit::AddToOrigPointsList (
    double const *xin, double const *yin, double const *zin,
    int npts, int id)
{
    int             origmax;
    double          *x, *y, *z;
    ORiginalPoints  *optr;

    if (OrigPointsList == NULL  ||
        NumOrigPointsList >= MaxOrigPointsList) {
        origmax = MaxOrigPointsList;
        MaxOrigPointsList += 20;
        OrigPointsList = (ORiginalPoints *)csw_Realloc
            (OrigPointsList, MaxOrigPointsList * sizeof(ORiginalPoints));
        if (OrigPointsList != NULL) {
            memset (OrigPointsList + origmax, 0, 20 * sizeof(ORiginalPoints));
        }
    }

    if (OrigPointsList == NULL) {
        return -1;
    }

    if (xin == NULL  ||  yin == NULL  ||  zin == NULL  ||
        npts < 1) {
        x = y = z = NULL;
    }
    else {
        x = (double *)csw_Malloc (npts * 3 * sizeof(double));
        if (x == NULL) {
            return -1;
        }
        y = x + npts;
        z = y + npts;

        memcpy (x, xin, npts * sizeof(double));
        memcpy (y, yin, npts * sizeof(double));
        memcpy (z, zin, npts * sizeof(double));
    }

    optr = OrigPointsList + NumOrigPointsList;
    optr->x = x;
    optr->y = y;
    optr->z = z;
    optr->npts = npts;
    optr->id = id;

    NumOrigPointsList++;

    return 1;

}


void PATCHSplit::FreeOrigPointsList (void)
{
    int              i;
    ORiginalPoints   *optr;

    if (OrigPointsList == NULL  ||
        NumOrigPointsList < 1) {
        return;
    }

    for (i=0; i<NumOrigPointsList; i++) {
        optr = OrigPointsList + i;
        csw_Free (optr->x);
    }

    csw_Free (OrigPointsList);
    OrigPointsList = NULL;
    NumOrigPointsList = 0;
    MaxOrigPointsList = 0;

    return;
}


/*--------------------------------------------------------------------------*/

int PATCHSplit::AddPointsToProtoPatch (PRotoPatch *prptr)
{
    int          i, id, npts, dsize;
    ORiginalPoints   *optr;
    double       *x, *y, *z;

    if (prptr == NULL) {
        assert (0);
    }

    prptr->x = NULL;
    prptr->y = NULL;
    prptr->z = NULL;
    prptr->npts = 0;

    id = prptr->patchid;

    for (i=0; i<NumOrigPointsList; i++) {
        optr = OrigPointsList + i;
        if (optr == NULL) {
            continue;
        }
        if (optr->id != id) {
            continue;
        }
        npts = optr->npts;
        dsize = npts * sizeof(double);
        x = (double *)csw_Malloc (dsize * 3);
        if (x == NULL) {
            return -1;
        }
        y = x + npts;
        z = y + npts;

        memcpy (x, optr->x, dsize);
        memcpy (y, optr->y, dsize);
        memcpy (z, optr->z, dsize);
        prptr->x = x;
        prptr->y = y;
        prptr->z = z;
        prptr->npts = npts;

        return 1;
    }

    return 0;

}



/*---------------------------------------------------------------------*/

/*
 * Get the sealed horizons, faults, sediment surface and model bottom
 * from the sealed model.  Also get the sealed horizon intersects, fault
 * intersects, top and bottom intersects.  Build proto patches for
 * each sealed fault and horizon from these data.
 */
int PATCHSplit::BuildProtoPatchesFromSealedModel (void)
{
    CSWTriMeshStruct        *tblist[4];
    CSWTriMeshStruct        *sealed_horizons,
                            *sealed_faults,
                            *sealed_top,
                            *sealed_bottom,
                            *sealed_calc_bottom;
    int                     num_sealed_horizons,
                            num_sealed_faults;
    _INtersectionLineList_  *ilist;

    const
      _INtersectionLine_    *horizon_intersects,
                            *top_and_bottom_intersects;
    int                     num_horizon_intersects,
                            num_top_and_bottom_intersects;
    int                     istat;

/*
 * Retrieve horizon, fault, sediment surface
 * and model bottom data from the sealed model.
 */
    if (SModel == NULL) {
        return -1;
    }

    if (XYTiny < 0.0) {
        XYTiny = (Xmax - Xmin + Ymax - Ymin) / 200000.0;
    }

    sealed_horizons = NULL;
    sealed_faults = NULL;
    sealed_top = NULL;
    sealed_bottom = NULL;
    sealed_calc_bottom = NULL;
    num_sealed_horizons = 0;
    num_sealed_faults = 0;

    SModel->getSealedHorizons (&sealed_horizons, &num_sealed_horizons);
    SModel->getSealedFaults (&sealed_faults, &num_sealed_faults);
    tblist[0] = NULL;
    tblist[1] = NULL;
    tblist[2] = NULL;
    SModel->getSealedTopAndBottom (tblist);
    sealed_top = tblist[0];
    sealed_bottom = tblist[1];
    sealed_calc_bottom = tblist[2];

    if (sealed_horizons == NULL  ||
        sealed_faults == NULL  ||
        num_sealed_horizons < 1  ||
        num_sealed_faults < 1) {
        return -1;
    }

/*
 * Retrieve the various intersection lists from the sealed model.
 */
    ilist = NULL;
    horizon_intersects = NULL;
    top_and_bottom_intersects = NULL;
    num_horizon_intersects = 0;
    num_top_and_bottom_intersects = 0;

    ilist = SModel->getHorizonIntersectionLines ();
    if (ilist) {
        horizon_intersects = ilist->list;
        num_horizon_intersects = ilist->nlist;
    }
    csw_Free (ilist);
    ilist = NULL;

    csw_Free (ilist);
    ilist = NULL;

    ilist = SModel->getTopAndBottomIntersectionLines ();
    if (ilist) {
        top_and_bottom_intersects = ilist->list;
        num_top_and_bottom_intersects = ilist->nlist;
    }
    csw_Free (ilist);
    ilist = NULL;

    top_and_bottom_intersects = top_and_bottom_intersects;
    num_top_and_bottom_intersects = num_top_and_bottom_intersects;

    if (horizon_intersects == NULL) {
        return -1;
    }

    if (num_horizon_intersects < 1) {
        return -1;
    }

    int do_write;
    do_write = csw_GetDoWrite ();
    if (do_write) {
        char      fname[100];
        double    v6[6];
        int       i, npad;
        CSWTriMeshStruct  *tmp, *padlist;

        SModel->getInputHorizons (&padlist, &npad);
        for (i=0; i<npad; i++) {
          tmp = padlist + i;
          sprintf (fname, "mb_input_hor_%d.tri", i+1);
          grdapi_ptr->grd_WriteTextTriMeshFile (
            0, v6,
            tmp->tris, tmp->num_tris,
            tmp->edges, tmp->num_edges,
            tmp->nodes, tmp->num_nodes,
            fname);
        }
        SModel->getInputFaults (&padlist, &npad);
        for (i=0; i<npad; i++) {
          tmp = padlist + i;
          sprintf (fname, "mb_input_flt_%d.tri", i+1);
          grdapi_ptr->grd_WriteTextTriMeshFile (
            0, v6,
            tmp->tris, tmp->num_tris,
            tmp->edges, tmp->num_edges,
            tmp->nodes, tmp->num_nodes,
            fname);
        }

        SModel->getPaddedHorizons (&padlist, &npad);
        for (i=0; i<npad; i++) {
          tmp = padlist + i;
          sprintf (fname, "mb_pad_hor_%d.tri", i+1);
          grdapi_ptr->grd_WriteTextTriMeshFile (
            0, v6,
            tmp->tris, tmp->num_tris,
            tmp->edges, tmp->num_edges,
            tmp->nodes, tmp->num_nodes,
            fname);
        }
        SModel->getPaddedFaults (&padlist, &npad);
        for (i=0; i<npad; i++) {
          tmp = padlist + i;
          sprintf (fname, "mb_pad_flt_%d.tri", i+1);
          grdapi_ptr->grd_WriteTextTriMeshFile (
            0, v6,
            tmp->tris, tmp->num_tris,
            tmp->edges, tmp->num_edges,
            tmp->nodes, tmp->num_nodes,
            fname);
        }

        for (i=0; i<num_sealed_horizons; i++) {
          tmp = sealed_horizons + i;
          sprintf (fname, "mb_seal_hor_%d.tri", i+1);
          grdapi_ptr->grd_WriteTextTriMeshFile (
            0, v6,
            tmp->tris, tmp->num_tris,
            tmp->edges, tmp->num_edges,
            tmp->nodes, tmp->num_nodes,
            fname);
        }
        for (i=0; i<num_sealed_faults; i++) {
          tmp = sealed_faults + i;
          sprintf (fname, "mb_seal_flt_%d.tri", i+1);
          grdapi_ptr->grd_WriteTextTriMeshFile (
            0, v6,
            tmp->tris, tmp->num_tris,
            tmp->edges, tmp->num_edges,
            tmp->nodes, tmp->num_nodes,
            fname);
        }
    }

/*
 * Declare variables needed for filling in the proto patches.
 */
    int                      i, j, ntot, nprptr, sid,
                             nbtot, nb, type, dsize;
    double                   *xb, *yb, *zb;
    CSWTriMeshStruct         *tmesh;
    PRotoPatch               *prptr;
    NOdeStruct               *nodes;
    EDgeStruct               *edges;
    TRiangleStruct           *tris;
    _INtersectionLine_       const *iptr;
    BOrderSegment            *blist, *bptr;

    FreePatchList ();

/*
 * Allocate space for the unset proto patch list.
 */
    ntot = num_sealed_horizons + num_sealed_faults + 4;
    PatchList = (PRotoPatch *)csw_Calloc (ntot * sizeof (PRotoPatch));
    if (PatchList == NULL) {
        return -1;
    }

    nprptr = 0;

/*
 * Build proto patches from the sealed horizons.
 */
    for (i=0; i<num_sealed_horizons; i++) {

        tmesh = sealed_horizons + i;
        prptr = PatchList + nprptr;
        nprptr++;

    /*
     * Transfer the sealed trimesh to the proto patch.
     */
        nodes = (NOdeStruct *)csw_Malloc (tmesh->num_nodes * sizeof(NOdeStruct));
        edges = (EDgeStruct *)csw_Malloc (tmesh->num_edges * sizeof(EDgeStruct));
        tris = (TRiangleStruct *)csw_Malloc (tmesh->num_tris * sizeof(TRiangleStruct));
        if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
            FreePatchList ();
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (tris);
            return -1;
        }

        memcpy (nodes, tmesh->nodes, tmesh->num_nodes * sizeof(NOdeStruct));
        memcpy (edges, tmesh->edges, tmesh->num_edges * sizeof(EDgeStruct));
        memcpy (tris, tmesh->tris, tmesh->num_tris * sizeof(TRiangleStruct));

        prptr->nodes = nodes;
        prptr->edges = edges;
        prptr->triangles = tris;
        prptr->num_nodes = tmesh->num_nodes;
        prptr->num_edges = tmesh->num_edges;
        prptr->num_triangles = tmesh->num_tris;

        sid = tmesh->id;
        prptr->patchid = tmesh->external_id;
        prptr->sgpflag = tmesh->vflag;
        memcpy (prptr->sgpdata, tmesh->vbase, 6*sizeof(double));

        AddPointsToProtoPatch (prptr);
        AddLinesToProtoPatch (prptr);

    /*
     * Allocate space for the proto patch border list.
     */
        nbtot = 0;
        for (j=0; j<num_horizon_intersects; j++) {
            iptr = horizon_intersects + j;
            if (iptr->surf1 == sid  ||  iptr->surf2 == sid) {
                nbtot++;
            }
        }

        if (nbtot < 1) {
            continue;
        }

        blist = (BOrderSegment *)csw_Calloc (nbtot * sizeof(BOrderSegment));
        if (blist == NULL) {
            FreePatchList ();
            return -1;
        }

        prptr->borderList = blist;
        prptr->numBorderList = nbtot;
        prptr->maxBorderList = nbtot;

    /*
     * Find the horizon intersects that use this horizon and
     * create BOrderSegments for the proto patch from them.
     */
        nb = 0;
        for (j=0; j<num_horizon_intersects; j++) {

            iptr = horizon_intersects + j;
            if (!(iptr->surf1 == sid  ||  iptr->surf2 == sid)) {
                continue;
            }

            if (nb > nbtot) {
                assert (0);
            }
            bptr = blist + nb;
            nb++;

            type = 0;
            if (iptr->surf1 >= _FAULT_ID_BASE_  &&  iptr->surf1 < _BOUNDARY_ID_BASE_) {
                type = 1;
            }
            if (iptr->surf2 >= _FAULT_ID_BASE_  &&  iptr->surf2 < _BOUNDARY_ID_BASE_) {
                type = 1;
            }

            xb = (double *)csw_Malloc (3 * iptr->npts * sizeof(double));
            if (xb == NULL) {
                FreePatchList ();
                return -1;
            }
            yb = xb + iptr->npts;
            zb = yb + iptr->npts;
            dsize = iptr->npts * sizeof(double);
            memcpy (xb, iptr->x, dsize);
            memcpy (yb, iptr->y, dsize);
            memcpy (zb, iptr->z, dsize);
            bptr->x = xb;
            bptr->y = yb;
            bptr->z = zb;
            bptr->npts = iptr->npts;
            bptr->type = type;
            bptr->direction = 0;
            bptr->used = 0;

            if (type == 1) {
                CSWTriMeshStruct   *tm1, *tm2;
                tm1 = NULL;
                tm2 = NULL;
                if (iptr->surf1 < _FAULT_ID_BASE_) {
                    tm1 = sealed_horizons + iptr->surf1;
                }
                else if (iptr->surf1 < _BOUNDARY_ID_BASE_) {
                    tm1 = sealed_faults + iptr->surf1 - _FAULT_ID_BASE_;
                }
                if (iptr->surf2 < _FAULT_ID_BASE_) {
                    tm2 = sealed_horizons + iptr->surf2;
                }
                else if (iptr->surf2 < _BOUNDARY_ID_BASE_) {
                    tm2 = sealed_faults + iptr->surf2 - _FAULT_ID_BASE_;
                }
                if (tm1 != NULL  &&  tm2 != NULL) {
                    istat =
                      AddToProtoContactLines (
                        iptr->x, iptr->y, iptr->z, iptr->npts,
                        tm1->external_id, tm2->external_id);
                    if (istat == -1) {
                        FreePatchList ();
                        FreeProtoContactLines ();
                        return -1;
                    }
                }
            }
        }

    /*
     * Figure out the direction flags for the border segments.  The
     * first is arbitatily forward and the remainder are assigned
     * relative to the first.
     */
        double            x1, y1, z1, x2, y2, z2;
        double            xt1, yt1, zt1, xt2, yt2, zt2;
        int               ndone, npts, ido;

        bptr = blist;
        bptr->direction = 1;
        x1 = bptr->x[0];
        y1 = bptr->y[0];
        z1 = bptr->z[0];
        npts = bptr->npts;
        x2 = bptr->x[npts-1];
        y2 = bptr->y[npts-1];
        z2 = bptr->z[npts-1];
        bptr->used = 1;

        ido = 0;
        for (;;) {
            ndone = 0;
            for (j=1; j<nbtot; j++) {
                bptr = blist + j;
                if (bptr->used == 1) {
                    continue;
                }
                npts = bptr->npts;
                xt1 = bptr->x[0];
                yt1 = bptr->y[0];
                zt1 = bptr->z[0];
                xt2 = bptr->x[npts-1];
                yt2 = bptr->y[npts-1];
                zt2 = bptr->z[npts-1];
                istat = SamePointXYZ (x1, y1, z1, xt1, yt1, zt1);
                if (istat == 1) {
                    bptr->used = 1;
                    bptr->direction = -1;
                    ndone++;
                    x1 = xt2;
                    y1 = yt2;
                    z1 = zt2;
                    continue;
                }
                istat = SamePointXYZ (x1, y1, z1, xt2, yt2, zt2);
                if (istat == 1) {
                    bptr->used = 1;
                    bptr->direction = 1;
                    ndone++;
                    x1 = xt1;
                    y1 = yt1;
                    z1 = zt1;
                    continue;
                }
                istat = SamePointXYZ (x2, y2, z2, xt1, yt1, zt1);
                if (istat == 1) {
                    bptr->used = 1;
                    bptr->direction = 1;
                    ndone++;
                    x1 = xt2;
                    y1 = yt2;
                    z1 = zt2;
                    continue;
                }
                istat = SamePointXYZ (x2, y2, z2, xt2, yt2, zt2);
                if (istat == 1) {
                    bptr->used = 1;
                    bptr->direction = -1;
                    ndone++;
                    x1 = xt1;
                    y1 = yt1;
                    z1 = zt1;
                    continue;
                }
            }
            if (ndone == 0) {
                break;
            }
            ido++;
            if (ido > nbtot) {
                break;
            }
        }

        for (j=0; j<nbtot; j++) {
            bptr = blist + j;
            bptr->used = 0;
        }

    }  /* end of loop through sealed horizons */

/*
 * Build proto patches from the sealed faults.
 */
    for (i=0; i<num_sealed_faults; i++) {

        tmesh = sealed_faults + i;
        prptr = PatchList + nprptr;
        nprptr++;

    /*
     * Transfer the sealed trimesh to the proto patch.
     */
        nodes = (NOdeStruct *)csw_Malloc (tmesh->num_nodes * sizeof(NOdeStruct));
        edges = (EDgeStruct *)csw_Malloc (tmesh->num_edges * sizeof(EDgeStruct));
        tris = (TRiangleStruct *)csw_Malloc (tmesh->num_tris * sizeof(TRiangleStruct));
        if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
            FreePatchList ();
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (tris);
            return -1;
        }

        memcpy (nodes, tmesh->nodes, tmesh->num_nodes * sizeof(NOdeStruct));
        memcpy (edges, tmesh->edges, tmesh->num_edges * sizeof(EDgeStruct));
        memcpy (tris, tmesh->tris, tmesh->num_tris * sizeof(TRiangleStruct));

        prptr->nodes = nodes;
        prptr->edges = edges;
        prptr->triangles = tris;
        prptr->num_nodes = tmesh->num_nodes;
        prptr->num_edges = tmesh->num_edges;
        prptr->num_triangles = tmesh->num_tris;

        sid = tmesh->id;
        prptr->patchid = tmesh->external_id;

        AddPointsToProtoPatch (prptr);
        AddLinesToProtoPatch (prptr);

    }  /* end of loop through sealed faults */

/*
 * Build proto patches from the sealed sediment surface
 * and model bottom.
 */
    for (i=0; i<3; i++) {

        tmesh = sealed_top;
        if (i == 1) {
            tmesh = sealed_bottom;
        }
        if (i == 2) {
            tmesh = sealed_calc_bottom;
        }
        if (tmesh == NULL) {
            continue;
        }
        prptr = PatchList + nprptr;
        nprptr++;

    /*
     * Transfer the sealed trimesh to the proto patch.
     */
        nodes = (NOdeStruct *)csw_Malloc (tmesh->num_nodes * sizeof(NOdeStruct));
        edges = (EDgeStruct *)csw_Malloc (tmesh->num_edges * sizeof(EDgeStruct));
        tris = (TRiangleStruct *)csw_Malloc (tmesh->num_tris * sizeof(TRiangleStruct));
        if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
            FreePatchList ();
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (tris);
            return -1;
        }

        memcpy (nodes, tmesh->nodes, tmesh->num_nodes * sizeof(NOdeStruct));
        memcpy (edges, tmesh->edges, tmesh->num_edges * sizeof(EDgeStruct));
        memcpy (tris, tmesh->tris, tmesh->num_tris * sizeof(TRiangleStruct));

        prptr->nodes = nodes;
        prptr->edges = edges;
        prptr->triangles = tris;
        prptr->num_nodes = tmesh->num_nodes;
        prptr->num_edges = tmesh->num_edges;
        prptr->num_triangles = tmesh->num_tris;

        sid = tmesh->id;
        prptr->patchid = tmesh->external_id;

        AddPointsToProtoPatch (prptr);
        AddLinesToProtoPatch (prptr);

    /*
     * Allocate space for the proto patch border list.
     */
        nbtot = 0;
        for (j=0; j<num_horizon_intersects; j++) {
            iptr = horizon_intersects + j;
            if (iptr->surf1 == sid  ||  iptr->surf2 == sid) {
                nbtot++;
            }
        }

        if (nbtot < 1) {
            continue;
        }

        blist = (BOrderSegment *)csw_Calloc (nbtot * sizeof(BOrderSegment));
        if (blist == NULL) {
            FreePatchList ();
            return -1;
        }

        prptr->borderList = blist;
        prptr->numBorderList = nbtot;
        prptr->maxBorderList = nbtot;

    /*
     * Find the horizon intersects that use this horizon and
     * create BOrderSegments for the proto patch from them.
     */
        nb = 0;
        for (j=0; j<num_horizon_intersects; j++) {

            iptr = horizon_intersects + j;
            if (!(iptr->surf1 == sid  ||  iptr->surf2 == sid)) {
                continue;
            }

            if (nb > nbtot) {
                assert (0);
            }
            bptr = blist + nb;
            nb++;

            type = 0;
            if (iptr->surf1 >= _FAULT_ID_BASE_  &&  iptr->surf1 < _BOUNDARY_ID_BASE_) {
                type = 1;
            }
            if (iptr->surf2 >= _FAULT_ID_BASE_  &&  iptr->surf2 < _BOUNDARY_ID_BASE_) {
                type = 1;
            }

            xb = (double *)csw_Malloc (3 * iptr->npts * sizeof(double));
            if (xb == NULL) {
                FreePatchList ();
                return -1;
            }
            yb = xb + iptr->npts;
            zb = yb + iptr->npts;
            dsize = iptr->npts * sizeof(double);
            memcpy (xb, iptr->x, dsize);
            memcpy (yb, iptr->y, dsize);
            memcpy (zb, iptr->z, dsize);
            bptr->x = xb;
            bptr->y = yb;
            bptr->z = zb;
            bptr->npts = iptr->npts;
            bptr->type = type;
            bptr->direction = 0;

        }

    /*
     * Figure out the direction flags for the border segments.  The
     * first is arbitatily forward and the remainder are assigned
     * relative to the first.
     */
        double            x1, y1, z1, x2, y2, z2;
        double            xt1, yt1, zt1, xt2, yt2, zt2;
        int               ndone, npts, ido;

        bptr = blist;
        bptr->direction = 1;
        x1 = bptr->x[0];
        y1 = bptr->y[0];
        z1 = bptr->z[0];
        npts = bptr->npts;
        x2 = bptr->x[npts-1];
        y2 = bptr->y[npts-1];
        z2 = bptr->z[npts-1];
        bptr->used = 1;

        ido = 0;
        for (;;) {
            ndone = 0;
            for (j=1; j<nbtot; j++) {
                bptr = blist + j;
                if (bptr->used == 1) {
                    continue;
                }
                npts = bptr->npts;
                xt1 = bptr->x[0];
                yt1 = bptr->y[0];
                zt1 = bptr->z[0];
                xt2 = bptr->x[npts-1];
                yt2 = bptr->y[npts-1];
                zt2 = bptr->z[npts-1];
                istat = SamePointXYZ (x1, y1, z1, xt1, yt1, zt1);
                if (istat == 1) {
                    bptr->used = 1;
                    bptr->direction = -1;
                    ndone++;
                    x1 = xt2;
                    y1 = yt2;
                    z1 = zt2;
                    continue;
                }
                istat = SamePointXYZ (x1, y1, z1, xt2, yt2, zt2);
                if (istat == 1) {
                    bptr->used = 1;
                    bptr->direction = 1;
                    ndone++;
                    x1 = xt1;
                    y1 = yt1;
                    z1 = zt1;
                    continue;
                }
                istat = SamePointXYZ (x2, y2, z2, xt1, yt1, zt1);
                if (istat == 1) {
                    bptr->used = 1;
                    bptr->direction = 1;
                    ndone++;
                    x1 = xt2;
                    y1 = yt2;
                    z1 = zt2;
                    continue;
                }
                istat = SamePointXYZ (x2, y2, z2, xt2, yt2, zt2);
                if (istat == 1) {
                    bptr->used = 1;
                    bptr->direction = -1;
                    ndone++;
                    x1 = xt1;
                    y1 = yt1;
                    z1 = zt1;
                    continue;
                }
            }
            if (ndone == 0) {
                break;
            }
            ido++;
            if (ido > nbtot) {
                break;
            }
        }

        for (j=0; j<nbtot; j++) {
            bptr = blist + j;
            bptr->used = 0;
        }

    }  /* end of loop through sealed top and bottom */

    NumPatchList = nprptr;

    return 1;

}



/*------------------------------------------------------------------------------------*/

void PATCHSplit::FreePatchList (void)
{
    int                i, j;
    PRotoPatch         *patch;

    if (PatchList == NULL) {
        return;
    }

    if (NumPatchList < 1) {
        return;
    }

    for (i=0; i<NumPatchList; i++) {
        patch = PatchList + i;
        csw_Free (patch->x);
        csw_Free (patch->nodes);
        csw_Free (patch->edges);
        csw_Free (patch->triangles);
        if (patch->borderList) {
            for (j=0; j<patch->numBorderList; j++) {
                csw_Free (patch->borderList[j].x);
            }
            csw_Free (patch->borderList);
        }
        if (patch->lineList) {
            for (j=0; j<patch->numLineList; j++) {
                csw_Free (patch->lineList[j].x);
            }
            csw_Free (patch->lineList);
        }
    }
    csw_Free (PatchList);
    PatchList = NULL;

    return;

}



/*------------------------------------------------------------------------------------*/

int PATCHSplit::AddToOrigLinesList (
    BAseLine const *linesin,
    int nlines, int id)
{
    int             origmax, i, npts;
    double          *x, *y, *z;
    ORiginalLines   *optr;
    BAseLine        *lines, *bp2;
    BAseLine const  *bp1;

    if (OrigLinesList == NULL  ||
        NumOrigLinesList >= MaxOrigLinesList) {
        origmax = MaxOrigLinesList;
        MaxOrigLinesList += 20;
        OrigLinesList = (ORiginalLines **)csw_Realloc
            (OrigLinesList, MaxOrigLinesList * sizeof(ORiginalLines*));
        if (OrigLinesList != NULL) {
            memset (OrigLinesList + origmax, 0, 20 * sizeof(ORiginalLines*));
        }
    }

    if (OrigLinesList == NULL) {
        return -1;
    }

    if (linesin == NULL || nlines < 1) {
        optr = NULL;
    }
    else {
        optr = (ORiginalLines *)csw_Calloc (sizeof(ORiginalLines));
        if (optr == NULL) {
            return -1;
        }
        lines = (BAseLine *)csw_Calloc (nlines * sizeof(BAseLine));
        if (lines == NULL) {
            csw_Free (optr);
            return -1;
        }
        for (i=0; i<nlines; i++) {
            bp1 = linesin + i;
            bp2 = lines + i;
            npts = bp1->npts;
            x = (double *)csw_Malloc (3 * npts * sizeof(double));
            if (x == NULL) {
                csw_Free (lines);
                csw_Free (optr);
                return -1;
            }
            y = x + npts;
            z = y + npts;
            memcpy (x, bp1->x, npts * sizeof(double));
            memcpy (y, bp1->y, npts * sizeof(double));
            memcpy (z, bp1->z, npts * sizeof(double));
            bp2->x = x;
            bp2->y = y;
            bp2->z = z;
            bp2->npts = npts;
            bp2->flag = bp1->flag;
        }
        optr->list = lines;
        optr->nlist = nlines;
        optr->id = id;
    }

    OrigLinesList[NumOrigLinesList] = optr;

    NumOrigLinesList++;

    return 1;

}


void PATCHSplit::FreeOrigLinesList (void)
{
    int             i, j, nlist;
    ORiginalLines   *optr;
    BAseLine        *bptr, *bp;

    if (OrigLinesList == NULL  ||
        NumOrigLinesList < 1) {
        return;
    }

    for (i=0; i<NumOrigLinesList; i++) {
        optr = OrigLinesList[i];
        if (optr == NULL) {
            continue;
        }
        bptr = optr->list;
        nlist = optr->nlist;
        if (bptr == NULL  ||  nlist < 1) {
            continue;
        }
        for (j=0; j<nlist; j++) {
            bp = bptr + j;
            csw_Free (bp->x);
        }
        csw_Free (bptr);
        csw_Free (optr);
    }

    csw_Free (OrigLinesList);
    OrigLinesList = NULL;
    NumOrigLinesList = 0;
    MaxOrigLinesList = 0;

    return;

}


/*--------------------------------------------------------------------------*/

int PATCHSplit::AddLinesToProtoPatch (PRotoPatch *prptr)
{
    int          i, j, id, npts, dsize;
    ORiginalLines   *optr;
    double       *x, *y, *z;
    BAseLine     *blist, *bp1, *bp2;
    int          nlist;

    if (prptr == NULL) {
        assert (0);
    }

    prptr->lineList = NULL;
    prptr->numLineList = 0;
    prptr->maxLineList = 0;

    id = prptr->patchid;

    for (i=0; i<NumOrigLinesList; i++) {
        optr = OrigLinesList[i];
        if (optr == NULL) {
            continue;
        }
        if (optr->id != id) {
            continue;
        }
        nlist = optr->nlist;
        blist = (BAseLine *)csw_Calloc (nlist * sizeof(BAseLine));
        if (blist == NULL) {
            return -1;
        }
        for (j=0; j<nlist; j++) {
            bp1 = optr->list + j;
            bp2 = blist + j;
            npts = bp1->npts;
            dsize = npts * sizeof(double);
            x = (double *)csw_Malloc (dsize * 3);
            if (x == NULL) {
                FreeBaseLineArray (blist, nlist);
                return -1;
            }
            y = x + npts;
            z = y + npts;
            memcpy (x, bp1->x, dsize);
            memcpy (y, bp1->y, dsize);
            memcpy (z, bp1->z, dsize);
            bp2->x = x;
            bp2->y = y;
            bp2->z = z;
            bp2->npts = npts;
        }

        prptr->lineList = blist;
        prptr->numLineList = nlist;
        prptr->maxLineList = nlist;

        return 1;
    }

    return 0;

}


void PATCHSplit::FreeBaseLineArray (BAseLine *blist, int nlist)
{
    int        i;
    BAseLine   *bp;

    for (i=0; i<nlist; i++) {
        bp = blist + i;
        csw_Free (bp->x);
    }
    csw_Free (blist);

    return;
}



/*----------------------------------------------------------------------------*/

/*
 * Set the sediment surface, which will not be split,
 * in the sealed model object.
 */

int PATCHSplit::ps_SetSedimentSurface (
    int          id,
    double       age,
    double       *xnodes,
    double       *ynodes,
    double       *znodes,
    int          num_nodes,
    int          *n1edge,
    int          *n2edge,
    int          *t1edge,
    int          *t2edge,
    int          num_edges,
    int          *e1tri,
    int          *e2tri,
    int          *e3tri,
    int          num_tris)
  {
    NOdeStruct   *nodes;
    EDgeStruct   *edges;
    TRiangleStruct   *tris;
    int          i, istat;

    if (SModel == NULL) {
        return -1;
    }

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    tris = (TRiangleStruct *)csw_Malloc (num_tris * sizeof(TRiangleStruct));
    if (tris == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        return -1;
    }

    for (i=0; i<num_nodes; i++) {
        nodes[i].x = xnodes[i];
        nodes[i].y = ynodes[i];
        nodes[i].z = znodes[i];
    }

    for (i=0; i<num_edges; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<num_tris; i++) {
        tris[i].edge1 = e1tri[i];
        tris[i].edge2 = e2tri[i];
        tris[i].edge3 = e3tri[i];
    }

    SModel->setSedimentSurface (
        id, age,
        nodes, num_nodes,
        edges, num_edges,
        tris, num_tris);

    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (tris);
    nodes = NULL;
    edges = NULL;
    tris = NULL;

    istat =
    AddToOrigPointsList (
        XPointsIn, YPointsIn, ZPointsIn,
        NumPointsIn, id);
    if (istat == -1) {
        return -1;
    }

    istat =
    AddToOrigLinesList (
        LinesIn,
        NumLinesIn, id);
    if (istat == -1) {
        return -1;
    }

    return 1;

  }




/*----------------------------------------------------------------------------*/

/*
 * Set the sediment surface, which will not be split,
 * in the sealed model object.
 */

int PATCHSplit::ps_SetModelBottom (
    int          id,
    double       age,
    double       *xnodes,
    double       *ynodes,
    double       *znodes,
    int          num_nodes,
    int          *n1edge,
    int          *n2edge,
    int          *t1edge,
    int          *t2edge,
    int          num_edges,
    int          *e1tri,
    int          *e2tri,
    int          *e3tri,
    int          num_tris)
  {
    NOdeStruct   *nodes;
    EDgeStruct   *edges;
    TRiangleStruct   *tris;
    int          i, istat;

    if (SModel == NULL) {
        return -1;
    }

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    tris = (TRiangleStruct *)csw_Malloc (num_tris * sizeof(TRiangleStruct));
    if (tris == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        return -1;
    }

    for (i=0; i<num_nodes; i++) {
        nodes[i].x = xnodes[i];
        nodes[i].y = ynodes[i];
        nodes[i].z = znodes[i];
    }

    for (i=0; i<num_edges; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<num_tris; i++) {
        tris[i].edge1 = e1tri[i];
        tris[i].edge2 = e2tri[i];
        tris[i].edge3 = e3tri[i];
    }

    SModel->setModelBottom (
        id, age,
        nodes, num_nodes,
        edges, num_edges,
        tris, num_tris);

    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (tris);
    nodes = NULL;
    edges = NULL;
    tris = NULL;

    istat =
    AddToOrigPointsList (
        XPointsIn, YPointsIn, ZPointsIn,
        NumPointsIn, id);
    if (istat == -1) {
        return -1;
    }

    istat =
    AddToOrigLinesList (
        LinesIn,
        NumLinesIn, id);
    if (istat == -1) {
        return -1;
    }

    return 1;

  }





/*------------------------------------------------------------------------------*/

void PATCHSplit::WriteCenterlinesToFile (char *fname)
{
    FAultCenterline    *fp;
    double             *x, *y;
    int                icomp[200], ivec[200];
    int                i, n, ntot;

    ntot = 0;
    for (i=0; i<NumFcenter; i++) {
        ntot += Fcenter[i].npts;
    }
    ntot += OrigNBorder;

    x = (double *)csw_Malloc (2 * ntot * sizeof(double));
    if (x == NULL) {
        return;
    }
    y = x + ntot;

    n = 0;
    for (i=0; i<NumFcenter; i++) {
        fp = Fcenter + i;
        ivec[i] = fp->npts;
        icomp[i] = 1;
        memcpy (x+n, fp->x, fp->npts * sizeof(double));
        memcpy (y+n, fp->y, fp->npts * sizeof(double));
        n += fp->npts;
    }
    memcpy (x+n, OrigXBorder, OrigNBorder * sizeof(double));
    memcpy (y+n, OrigYBorder, OrigNBorder * sizeof(double));
    ivec[NumFcenter] = OrigNBorder;
    icomp[NumFcenter] = 1;

    grdapi_ptr->grd_WriteLines (x, y, NULL,
                    NumFcenter + 1, icomp, ivec,
                    fname);

    return;

}

/*-------------------------------------------------------------------*/

int PATCHSplit::ClipCenterlinesToBorder (void)
{
    int             i, maxout, maxcomp;
    int             iout[100], nout, istat;
    FAultCenterline *fp;
    double          *xout, *yout, *zout;

    maxcomp = 100;
    for (i=0; i<NumFcenter; i++) {
        fp = Fcenter + i;

        maxout = fp->npts + 2;
        maxout *= 2;
        xout = (double *)csw_Malloc (maxout * 3 * sizeof(double));
        if (xout == NULL) {
            return -1;
        }
        yout = xout + maxout;
        zout = yout + maxout;

        istat =
          ply_calc_obj.ply_ClipPlineToArea (
            1,
            OrigXBorder,
            OrigYBorder,
            &OrigNBorder,
            1,
            OrigXBorder,
            OrigYBorder,
            &OrigNBorder,
            1,
            fp->x,
            fp->y,
            fp->npts,
            xout,
            yout,
            iout,
            &nout,
            maxout,
            maxcomp);
        if (istat == -1) {
            csw_Free (xout);
            return -1;
        }

        csw_Free (fp->x);
        fp->x = xout;
        fp->y = yout;
        fp->z = zout;
        fp->npts = iout[0];
        memset (zout, 0, maxout * sizeof(double));

        xout = yout = zout = NULL;

    }

    return 1;
}

/*-----------------------------------------------------------------*/

/*
 * Return the input surface patches in the sealed model to the
 * java code.  This is used to get the results of splitting surfaces.
 */
int PATCHSplit::ps_GetSealedInput (void)
  {
    int        istat;

    if (SModel == NULL) {
        return -1;
    }

    istat =
      SendBackInputModel ();

    if (istat == -1) {
        return -1;
    }

    istat =
      SendBackFaultMajorMinor ();

    return istat;

  }



/*---------------------------------------------------------------------*/

/*
 * Get the input horizons from the sealed model.  Build proto patches for
 * each input horizon from these data.
 */
int PATCHSplit::BuildProtoPatchesFromInputModel (void)
{
    CSWTriMeshStruct        *input_horizons;
    int                     num_input_horizons;


/*
 * Retrieve horizon data from the input model.
 */
    if (SModel == NULL) {
        return -1;
    }

    if (XYTiny < 0.0) {
        XYTiny = (Xmax - Xmin + Ymax - Ymin) / 200000.0;
    }

    input_horizons = NULL;
    num_input_horizons = 0;

    SModel->getInputHorizons (&input_horizons, &num_input_horizons);

    if (input_horizons == NULL  ||
        num_input_horizons < 1) {
        return -1;
    }

/*
 * Declare variables needed for filling in the proto patches.
 */
    int                      i, ntot, nprptr;
    CSWTriMeshStruct         *tmesh;
    PRotoPatch               *prptr;
    NOdeStruct               *nodes;
    EDgeStruct               *edges;
    TRiangleStruct           *tris;

    FreePatchList ();

/*
 * Allocate space for the unset proto patch list.
 */
    ntot = num_input_horizons + 2;
    PatchList = (PRotoPatch *)csw_Calloc (ntot * sizeof (PRotoPatch));
    if (PatchList == NULL) {
        return -1;
    }

    nprptr = 0;

/*
 * Build proto patches from the input horizons.
 */
    for (i=0; i<num_input_horizons; i++) {

        tmesh = input_horizons + i;
        prptr = PatchList + nprptr;
        nprptr++;

    /*
     * Transfer the input trimesh to the proto patch.
     */
        nodes = (NOdeStruct *)csw_Malloc (tmesh->num_nodes * sizeof(NOdeStruct));
        edges = (EDgeStruct *)csw_Malloc (tmesh->num_edges * sizeof(EDgeStruct));
        tris = (TRiangleStruct *)csw_Malloc (tmesh->num_tris * sizeof(TRiangleStruct));
        if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
            FreePatchList ();
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (tris);
            return -1;
        }

        memcpy (nodes, tmesh->nodes, tmesh->num_nodes * sizeof(NOdeStruct));
        memcpy (edges, tmesh->edges, tmesh->num_edges * sizeof(EDgeStruct));
        memcpy (tris, tmesh->tris, tmesh->num_tris * sizeof(TRiangleStruct));

        prptr->nodes = nodes;
        prptr->edges = edges;
        prptr->triangles = tris;
        prptr->num_nodes = tmesh->num_nodes;
        prptr->num_edges = tmesh->num_edges;
        prptr->num_triangles = tmesh->num_tris;

        prptr->patchid = tmesh->external_id;

        prptr->sgpflag = tmesh->vflag;
        memcpy (prptr->sgpdata, tmesh->vbase, 6 * sizeof(double));

        AddPointsToProtoPatch (prptr);
        AddLinesToProtoPatch (prptr);

    }  /* end of loop through input horizons */

    NumPatchList = nprptr;

    return 1;

}


/*------------------------------------------------------------------------------*/

int PATCHSplit::BuildSplitLines (void)
{
    CSWTriMeshStruct   *tm1, *tm2;
    CSWTriMeshStruct   *hlist, *flist;
    int                nhlist, nflist;
    _INtersectionLineList_  *list;
    const
      _INtersectionLine_    *ilist, *iptr;
    int                i, nlist, istat;

    if (SModel == NULL) {
        return -1;
    }

    list = SModel->getRawIntersectionLines ();
    if (list == NULL) {
        return 0;
    }

    nlist = list->nlist;
    ilist = list->list;

    if (ilist == NULL  ||  nlist < 1) {
        return 0;
    }

    SModel->getInputHorizons (&hlist, &nhlist);
    SModel->getInputFaults (&flist, &nflist);

    if (hlist == NULL  ||  flist == NULL  ||
        nhlist < 1  ||  nflist < 1) {
        return 0;
    }

    for (i=0; i<nlist; i++) {
        iptr = ilist + i;
        tm1 = NULL;
        tm2 = NULL;
        if (iptr->surf1 < _FAULT_ID_BASE_) {
            tm1 = hlist + iptr->surf1;
        }
        else if (iptr->surf1 < _BOUNDARY_ID_BASE_) {
            tm1 = flist + iptr->surf1 - _FAULT_ID_BASE_;
        }
        if (iptr->surf2 < _FAULT_ID_BASE_) {
            tm2 = hlist + iptr->surf2;
        }
        else if (iptr->surf2 < _BOUNDARY_ID_BASE_) {
            tm2 = flist + iptr->surf2 - _FAULT_ID_BASE_;
        }
        if (tm1 != NULL  &&  tm2 != NULL) {
            istat =
              AddToSplitLines (
                iptr->x, iptr->y, iptr->z, iptr->npts,
                tm1->external_id, tm2->external_id);
            if (istat == -1) {
                FreeSplitLines ();
                return -1;
            }
        }
    }

    return 1;

}


/*
 ***********************************************************

    functions supporting the fault connect groups

 ***********************************************************
*/

int PATCHSplit:: ps_StartFaultConnect (double avspace)
{

    if (FConnect != NULL) {
        delete (FConnect);
        FConnect = NULL;
    }

    try {
        FConnect = new FaultConnect ();
    }
    catch (...) {
        FConnect = NULL;
        return 1;
    }
    FConnect->setAverageSpacing (avspace);

    return 1;
}


int PATCHSplit:: ps_EndFaultConnect (void)
{

    if (FConnect != NULL) {
        delete (FConnect);
        FConnect = NULL;
    }

    return 1;
}


/*----------------------------------------------------------------*/

int PATCHSplit:: ps_ConnectFaults (void)
{
    if (FConnect == NULL) {
        return -1;
    }

    int istat;
    istat = FConnect->connectFaults ();
    if (istat != 1) {
        return istat;
    }

    istat =
      BuildProtoPatchesFromFaultConnect ();
    if (istat != 1) {
        return istat;
    }

    SendBackProtoPatches ();

    return 1;

}


/*----------------------------------------------------------------*/

int PATCHSplit:: ps_SetDetachment (
    double          *xnodes,
    double          *ynodes,
    double          *znodes,
    int             num_nodes,
    int             *n1edge,
    int             *n2edge,
    int             *t1edge,
    int             *t2edge,
    int             num_edges,
    int             *e1tri,
    int             *e2tri,
    int             *e3tri,
    int             num_tris)
{
    if (FConnect == NULL) {
        return -1;
    }

    NOdeStruct      *nodes;
    EDgeStruct      *edges;
    TRiangleStruct  *tris;

    nodes = (NOdeStruct *)csw_Calloc (num_nodes * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc (num_edges * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    tris = (TRiangleStruct *)csw_Calloc (num_tris * sizeof(TRiangleStruct));
    if (tris == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        return -1;
    }

    int    i;

    for (i=0; i<num_nodes; i++) {
        nodes[i].x = xnodes[i];
        nodes[i].y = ynodes[i];
        nodes[i].z = znodes[i];
    }

    for (i=0; i<num_edges; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<num_tris; i++) {
        tris[i].edge1 = e1tri[i];
        tris[i].edge2 = e2tri[i];
        tris[i].edge3 = e3tri[i];
    }

    int istat;

    istat =
      FConnect->setDetachment (
        nodes, num_nodes,
        edges, num_edges,
        tris, num_tris);

    return istat;
}


/*----------------------------------------------------------------*/

int PATCHSplit:: ps_AddToDetachment (
    double          *xnodes,
    double          *ynodes,
    double          *znodes,
    int             num_nodes,
    int             *n1edge,
    int             *n2edge,
    int             *t1edge,
    int             *t2edge,
    int             num_edges,
    int             *e1tri,
    int             *e2tri,
    int             *e3tri,
    int             num_tris)
{
    if (FConnect == NULL) {
        return -1;
    }

    NOdeStruct      *nodes;
    EDgeStruct      *edges;
    TRiangleStruct  *tris;

    nodes = (NOdeStruct *)csw_Calloc (num_nodes * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc (num_edges * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    tris = (TRiangleStruct *)csw_Calloc (num_tris * sizeof(TRiangleStruct));
    if (tris == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        return -1;
    }

    int    i;

    for (i=0; i<num_nodes; i++) {
        nodes[i].x = xnodes[i];
        nodes[i].y = ynodes[i];
        nodes[i].z = znodes[i];
    }

    for (i=0; i<num_edges; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<num_tris; i++) {
        tris[i].edge1 = e1tri[i];
        tris[i].edge2 = e2tri[i];
        tris[i].edge3 = e3tri[i];
    }

    int istat;

    istat =
      FConnect->addToDetachment (
        nodes, num_nodes,
        edges, num_edges,
        tris, num_tris);

    return istat;
}


/*----------------------------------------------------------------*/

int PATCHSplit:: ps_AddConnectingFault (
    int             id,
    int             flag,
    int             sgpflag,
    double          *sgpdata,
    double          *xnodes,
    double          *ynodes,
    double          *znodes,
    int             num_nodes,
    int             *n1edge,
    int             *n2edge,
    int             *t1edge,
    int             *t2edge,
    int             num_edges,
    int             *e1tri,
    int             *e2tri,
    int             *e3tri,
    int             num_tris)
{
    if (FConnect == NULL) {
        return -1;
    }

    NOdeStruct      *nodes;
    EDgeStruct      *edges;
    TRiangleStruct  *tris;

    nodes = (NOdeStruct *)csw_Calloc (num_nodes * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc (num_edges * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    tris = (TRiangleStruct *)csw_Calloc (num_tris * sizeof(TRiangleStruct));
    if (tris == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        return -1;
    }

    int    i;

    for (i=0; i<num_nodes; i++) {
        nodes[i].x = xnodes[i];
        nodes[i].y = ynodes[i];
        nodes[i].z = znodes[i];
    }

    for (i=0; i<num_edges; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<num_tris; i++) {
        tris[i].edge1 = e1tri[i];
        tris[i].edge2 = e2tri[i];
        tris[i].edge3 = e3tri[i];
    }

    int istat;

    istat =
       FConnect->setFault (
         id,
         sgpflag, sgpdata,
         nodes, num_nodes,
         edges, num_edges,
         tris, num_tris);

    flag = flag;

    return istat;
}



/*---------------------------------------------------------------------*/

/*
 * Get the sealed detachment and sealed faults from the
 * FConnect object and build proto patches from them.
 */
int PATCHSplit::BuildProtoPatchesFromFaultConnect (void)
{
/*
 * Retrieve the sealed detachment from the FaultConnect object.
 */
    if (FConnect == NULL) {
        return -1;
    }

    if (XYTiny < 0.0) {
        XYTiny = (Xmax - Xmin + Ymax - Ymin) / 200000.0;
    }

/*
 * Declare variables needed for filling in the proto patches.
 */
    int                      i, ntot, nprptr;
    PRotoPatch               *prptr;
    NOdeStruct               *nodes;
    EDgeStruct               *edges;
    TRiangleStruct           *tris;
    int                      nn, ne, nt;
    int                      sgpflag;
    double                   sgpdata[6];
    int                      num_sealed_faults;
    int                      istat;

    FreePatchList ();

    num_sealed_faults = FConnect->getNumberOfConnectedFaults ();


/*
 * Allocate space for the unset proto patch list.
 */
    ntot = num_sealed_faults + 1;
    PatchList = (PRotoPatch *)csw_Calloc (ntot * sizeof (PRotoPatch));
    if (PatchList == NULL) {
        return -1;
    }

/*
 * Transfer the sealed detachment data to the first
 * proto patch.
 */
    istat =
      FConnect->getConnectedDetachment (
        &nodes, &nn,
        &edges, &ne,
        &tris, &nt,
        &sgpflag,
        sgpdata);
    if (istat != 1) {
        FreePatchList ();
        return -1;
    }

    int    detachid = -1000;

    prptr = PatchList;
    prptr->nodes = nodes;
    prptr->edges = edges;
    prptr->triangles = tris;
    prptr->num_nodes = nn;
    prptr->num_edges = ne;
    prptr->num_triangles = nt;
    prptr->patchid = detachid;
    prptr->sgpflag = sgpflag;
    memcpy (prptr->sgpdata, sgpdata, 6 * sizeof(double));

    nprptr = 1;

/*
 * Build proto patches from the sealed faults.
 */
    int    fid, fid2, npts;
    double *x, *y, *z;

    for (i=0; i<num_sealed_faults; i++) {

        prptr = PatchList + nprptr;
        nprptr++;

        istat =
          FConnect->getConnectedFault (
            i, &fid,
            &nodes, &nn,
            &edges, &ne,
            &tris, &nt,
            &sgpflag,
            sgpdata);
        if (istat != 1) {
            FreePatchList ();
            return -1;
        }

        istat =
          FConnect->getFaultContactLine (
            i, &fid2,
            &x, &y, &z, &npts);
        if (istat != 1) {
            FreePatchList ();
            return -1;
        }

        if (fid != fid2) {
            assert (0);
        }

        prptr->nodes = nodes;
        prptr->edges = edges;
        prptr->triangles = tris;
        prptr->num_nodes = nn;
        prptr->num_edges = ne;
        prptr->num_triangles = nt;
        prptr->patchid = fid;
        prptr->sgpflag = sgpflag;
        memcpy (prptr->sgpdata, sgpdata, 6 * sizeof(double));

        istat =
          AddToProtoContactLines (
            x, y, z, npts,
            fid, detachid);
        if (istat != 1) {
            FreePatchList ();
            return -1;
        }

    }  /* end of loop through sealed horizons */

    NumPatchList = nprptr;

    return 1;

}


/*----------------------------------------------------------------------*/

int PATCHSplit::AssignMinorLines (
    double  *xlines,
    double  *ylines,
    int     nlines,
    int     *ilines)
{
    int         i, j, ido, npts, istat, numinside, numgraze,
                nborder, maxborder, inside, offset;
    double      *x, *y, tiny, grsav;
    WOrkPoly    *wp;
    FAultCenterline   *fp;
    char        used[10000];

    if (nlines > 10000) {
        return -1;
    }

    if (Xmax > Xmin  &&  Ymax > Ymin) {
        tiny = (Xmax - Xmin + Ymax - Ymin) / 2000.0;
    }
    else {
        tiny = .001;
    }
    ply_utils_obj.ply_getgraze (&grsav);
    ply_utils_obj.ply_setgraze (tiny);

    memset (used, 0, nlines * sizeof(char));

/*
 * For each work polygon, find lines that are inside the polygon.
 */
    for (ido=0; ido<NumWorkPolyList; ido++) {

        wp = WorkPolyList + ido;

        offset = 0;
        for (i=0; i<nlines; i++) {
            if (used[i] == 1) {
                offset += ilines[i];
                continue;
            }
            x = xlines + offset;
            y = ylines + offset;
            npts = ilines[i];
            offset += npts;

            nborder = 0;
            maxborder = 3;
            if (npts < 3) maxborder = npts - 1;
            if (maxborder < 1) maxborder = 1;

            fp = Fcenter + i;

            numinside = 0;
            numgraze = 0;
            for (j=0; j<npts; j++) {
                inside = ply_utils_obj.ply_point (wp->xp, wp->yp, wp->np,
                                    x[j], y[j]);
              /*
               * If a line point is inside, add the line as
               * a minor fault line of the polygon.
               */
                if (inside == 1) {
                    numinside++;
                }
                else if (inside == 0) {
                    numgraze++;
                }

              /*
               * If the line point is outside, check the next line.
               */
                else if (inside == -1) {
                    break;
                }

              /*
               * If 3 points are on the work poly border, the line
               * is part of the work poly border and thus it cannot
               * be inside any work polygon.
               */
                else {
                    nborder++;
                    if (nborder >= maxborder) {
                        used[i] = 1;
                        break;
                    }
                }
            }

            if (numinside + numgraze == npts  &&  numgraze < 2) {
                istat =
                  AddMinorLineToWorkPoly (wp, x, y, npts,
                                          fp->lock1, fp->lock2);
                if (istat == -1) {
                    ply_utils_obj.ply_setgraze (grsav);
                    return -1;
                }
                fp->major = 0;
                used[i] = 1;
            }
        }
    }

    ply_utils_obj.ply_setgraze (grsav);

    return 1;
}

/*
 * Copy the line and add it to the work poly lines.  This is
 * only called from AssignMinorLines.
 */
int PATCHSplit::AddMinorLineToWorkPoly (
    WOrkPoly    *wp,
    double      *x,
    double      *y,
    int         npts,
    int         lock1,
    int         lock2)
{
    BAseLine    *blist, *bptr;
    int         ntot, next;
    double      *xa, *ya, *za;

    blist = wp->minor_lines;
    next = wp->num_minor_lines;
    ntot = wp->max_minor_lines;
    if (next >= ntot) {
        ntot += 10;
        blist = (BAseLine *)csw_Realloc (blist, ntot * sizeof(BAseLine));
        if (blist == NULL) {
            return -1;
        }
        memset (blist+next, 0, 10 * sizeof(BAseLine));
    }

    wp->minor_lines = blist;
    wp->max_minor_lines = ntot;

    bptr = blist + next;
    xa = (double *)csw_Calloc (npts * 3 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + npts;
    za = ya + npts;

    int    istart = 0;

    if (lock1 == 0) {
        istart = 1;
        npts--;
    }
    if (lock2 == 0) npts--;

    memcpy (xa, x+istart, npts * sizeof(double));
    memcpy (ya, y+istart, npts * sizeof(double));
    bptr->x = xa;
    bptr->y = ya;
    bptr->z = za;
    bptr->npts = npts;

    next++;
    wp->num_minor_lines = next;

    return 1;

}


/*---------------------------------------------------------------------*/

int PATCHSplit::BuildArraysFromBaseline (
  BAseLine    *blist, int nlist,
  double **xline, double **yline, double **zline,
  int **iline)
{
    int        i, j, n, ntot, *nplist;
    double     *x, *y, *z;
    BAseLine   *bptr;

/*
 * Initialize output in case of error.
 */
    *xline = NULL;
    *yline = NULL;
    *zline = NULL;
    *iline = NULL;

/*
 * Allocate space for the output lines.
 */
    ntot = 0;
    for (i=0; i<nlist; i++) {
        bptr = blist + i;
        ntot += bptr->npts;
    }

    if (ntot < 1) {
        return 0;
    }

    x = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (x == NULL) {
        return -1;
    }
    y = x + ntot;
    z = y + ntot;

    nplist = (int *)csw_Malloc (nlist * sizeof(int));
    if (nplist == NULL) {
        csw_Free (x);
        return -1;
    }

/*
 * Transfer from BAseLine structs to output lines.
 */
    n = 0;
    for (i=0; i<nlist; i++) {
        bptr = blist + i;
        nplist[i] = bptr->npts;
        for (j=0; j<bptr->npts; j++) {
            x[n] = bptr->x[j];
            y[n] = bptr->y[j];
            z[n] = bptr->z[j];
            n++;
        }
    }

    *xline = x;
    *yline = y;
    *zline = z;
    *iline = nplist;

    return 1;

}

/*----------------------------------------------------------------------------*/

/*
 * Calculate the perpendicular distance from the point (xt, yt) to the
 * closest point to a centerline that uses the specified work polygon index.
 */
double PATCHSplit::MinorLineDistance (
    WOrkPoly   *wp,
    double     xt,
    double     yt)

{
    BAseLine          *blist, *bptr;
    int               nlist;
    double            xp, yp, dist, dmin;
    int               ido, i, istat, npts;
    double            *x, *y;

    dmin = 1.e30;

    blist = wp->minor_lines;
    nlist = wp->num_minor_lines;
    if (blist == NULL  ||  nlist < 1) {
        return 1.e30;
    }

    dmin = 1.e30;

    for (ido=0; ido<nlist; ido++) {

        bptr = blist + ido;

        x = bptr->x;
        y = bptr->y;
        npts = bptr->npts;

    /*
     * Find a point on the line that, when connected to the
     * target point, forms a segment perpendicular to the
     * segment of the line.  The distance between this point
     * and the target point is the distance to the line.
     */
        for (i=0; i<npts-1; i++) {

            istat = gpf_perpintpoint2 (
                x[i],
                y[i],
                x[i+1],
                y[i+1],
                xt,
                yt,
                &xp,
                &yp);

          /*
           * Ignore if the perpendicular point is outside the segment.
           */
            if (istat == 0) {
                continue;
            }

            gpf_calcdistance2 (
                xt,
                yt,
                xp,
                yp,
                &dist);

            if (dist < dmin) {
                dmin = dist;
            }
        }
    }

  /*
   * If no perpendicular point was found, check for the closest
   * segment endpoint to the target point.
   */
    if (dmin < 1.e20) {
        return dmin;
    }

    for (ido=0; ido<nlist; ido++) {

        bptr = blist + ido;

        x = bptr->x;
        y = bptr->y;
        npts = bptr->npts;

        if (npts < 2) {
            continue;
        }

    /*
     * Find the closest line segment endpoint.
     */
        for (i=0; i<npts; i++) {

            xp = x[i];
            yp = y[i];

            gpf_calcdistance2 (
                xt,
                yt,
                xp,
                yp,
                &dist);

            if (dist < dmin) {
                dmin = dist;
            }
        }
    }

    return dmin;

}


/*---------------------------------------------------------------*/
void  PATCHSplit::ps_SetExtendFraction (double val)
{
    if (val < 0.0 || val > .5) {
        return;
    }

    ExtendFraction = val;
}



/*----------------------------------------------------------------*/

int PATCHSplit::ps_SetLowerSurface (
    double          *xnodes,
    double          *ynodes,
    double          *znodes,
    int             num_nodes,
    int             *n1edge,
    int             *n2edge,
    int             *t1edge,
    int             *t2edge,
    int             num_edges,
    int             *e1tri,
    int             *e2tri,
    int             *e3tri,
    int             num_tris)
  {
    if (FConnect == NULL) {
        return -1;
    }

    NOdeStruct      *nodes;
    EDgeStruct      *edges;
    TRiangleStruct  *tris;

    nodes = (NOdeStruct *)csw_Calloc (num_nodes * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc (num_edges * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    tris = (TRiangleStruct *)csw_Calloc (num_tris * sizeof(TRiangleStruct));
    if (tris == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        return -1;
    }

    int    i;

    for (i=0; i<num_nodes; i++) {
        nodes[i].x = xnodes[i];
        nodes[i].y = ynodes[i];
        nodes[i].z = znodes[i];
    }

    for (i=0; i<num_edges; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<num_tris; i++) {
        tris[i].edge1 = e1tri[i];
        tris[i].edge2 = e2tri[i];
        tris[i].edge3 = e3tri[i];
    }

    int istat;

    istat =
      FConnect->setDetachmentLowerSurface (
        nodes, num_nodes,
        edges, num_edges,
        tris, num_tris);

    return istat;
  }

/*----------------------------------------------------------------*/
int PATCHSplit::ps_SetUpperSurface (
    double          *xnodes,
    double          *ynodes,
    double          *znodes,
    int             num_nodes,
    int             *n1edge,
    int             *n2edge,
    int             *t1edge,
    int             *t2edge,
    int             num_edges,
    int             *e1tri,
    int             *e2tri,
    int             *e3tri,
    int             num_tris)
  {
    if (FConnect == NULL) {
        return -1;
    }

    NOdeStruct      *nodes;
    EDgeStruct      *edges;
    TRiangleStruct  *tris;

    nodes = (NOdeStruct *)csw_Calloc (num_nodes * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc (num_edges * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    tris = (TRiangleStruct *)csw_Calloc (num_tris * sizeof(TRiangleStruct));
    if (tris == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        return -1;
    }

    int    i;

    for (i=0; i<num_nodes; i++) {
        nodes[i].x = xnodes[i];
        nodes[i].y = ynodes[i];
        nodes[i].z = znodes[i];
    }

    for (i=0; i<num_edges; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<num_tris; i++) {
        tris[i].edge1 = e1tri[i];
        tris[i].edge2 = e2tri[i];
        tris[i].edge3 = e3tri[i];
    }

    int istat;

    istat =
      FConnect->setDetachmentUpperSurface (
        nodes, num_nodes,
        edges, num_edges,
        tris, num_tris);

    return istat;
  }

/*-----------------------------------------------------------------------*/


int PATCHSplit::ps_CalcDetachment (double lower_age,
                         double upper_age,
                         double age)
  {

    if (FConnect == NULL) {
        return -1;
    }

    int istat = FConnect->calcDetachment (lower_age, upper_age, age);
    if (istat == -1) {
        return -1;
    }

    istat =
      BuildProtoPatchesFromFaultConnectDetachment ();
    if (istat != 1) {
        return istat;
    }

    SendBackProtoPatches ();

    return istat;
  }

/*---------------------------------------------------------------------*/

/*
 * Get the calculated detachment from the FConnect object and
 * build a proto patch from it.
 */
int PATCHSplit::BuildProtoPatchesFromFaultConnectDetachment (void)
{
/*
 * Retrieve the sealed detachment from the FaultConnect object.
 */
    if (FConnect == NULL) {
        return -1;
    }

    if (XYTiny < 0.0) {
        XYTiny = (Xmax - Xmin + Ymax - Ymin) / 200000.0;
    }

/*
 * Declare variables needed for filling in the proto patches.
 */
    int                      ntot, nprptr;
    PRotoPatch               *prptr;
    NOdeStruct               *nodes;
    EDgeStruct               *edges;
    TRiangleStruct           *tris;
    int                      nn, ne, nt;
    int                      sgpflag;
    double                   sgpdata[6];
    int                      num_sealed_faults;
    int                      istat;

    FreePatchList ();

    num_sealed_faults = FConnect->getNumberOfConnectedFaults ();


/*
 * Allocate space for the unset proto patch list.
 */
    ntot = num_sealed_faults + 1;
    PatchList = (PRotoPatch *)csw_Calloc (ntot * sizeof (PRotoPatch));
    if (PatchList == NULL) {
        return -1;
    }

/*
 * Transfer the calculated detachment data to the
 * proto patch.
 */
    istat =
      FConnect->getCalculatedDetachment (
        &nodes, &nn,
        &edges, &ne,
        &tris, &nt,
        &sgpflag,
        sgpdata);
    if (istat != 1) {
        FreePatchList ();
        return -1;
    }

    int    detachid = -1000;

    prptr = PatchList;
    prptr->nodes = nodes;
    prptr->edges = edges;
    prptr->triangles = tris;
    prptr->num_nodes = nn;
    prptr->num_edges = ne;
    prptr->num_triangles = nt;
    prptr->patchid = detachid;
    prptr->sgpflag = sgpflag;
    memcpy (prptr->sgpdata, sgpdata, 6 * sizeof(double));

    nprptr = 1;

    NumPatchList = nprptr;

    return 1;

}

/*
 * Add a fault surface trimesh directly to the SealedModel object.
 * The fault in his version has been sealed to a detachment and
 * the contact line for that detachment seal is specified.
 */
int PATCHSplit::ps_AddFaultSurfaceWithDetachmentContact (
    int          id,
    int          vused,
    double       *vbase,
    double       *x,
    double       *y,
    double       *z,
    int          num_nodes,
    int          *n1,
    int          *n2,
    int          *t1,
    int          *t2,
    int          num_edges,
    int          *e1,
    int          *e2,
    int          *e3,
    int          num_triangles,
    double       minage,
    double       maxage,
    double       *xline,
    double       *yline,
    double       *zline,
    int          nline,
    int          detach_id)
  {
    int              i;
    NOdeStruct       *nodes;
    EDgeStruct       *edges;
    TRiangleStruct   *triangles;

    if (SModel == NULL) {
        return -1;
    }

    if (x == NULL  ||  y == NULL  ||  z == NULL  ||
        n1 == NULL  ||  n2 == NULL  ||  t1 == NULL  ||  t2 == NULL  ||
        e1 == NULL  ||  e2 == NULL  ||  e3 == NULL  ||
        num_nodes < 3  ||  num_edges < 3  ||  num_triangles < 1) {
        return 0;
    }

    nodes = (NOdeStruct *)csw_Calloc (num_nodes * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc (num_edges * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    triangles = (TRiangleStruct *)csw_Calloc (num_triangles * sizeof(TRiangleStruct));
    if (triangles == NULL) {
        csw_Free (edges);
        csw_Free (nodes);
        return -1;
    }

    for (i=0; i<num_nodes; i++) {
        nodes[i].x = x[i];
        nodes[i].y = y[i];
        nodes[i].z = z[i];
    }

    for (i=0; i<num_edges; i++) {
        edges[i].node1 = n1[i];
        edges[i].node2 = n2[i];
        edges[i].tri1 = t1[i];
        edges[i].tri2 = t2[i];
    }

    for (i=0; i<num_triangles; i++) {
        triangles[i].edge1 = e1[i];
        triangles[i].edge2 = e2[i];
        triangles[i].edge3 = e3[i];
    }

    SModel->addInputFault (
        id,
        vused, vbase,
        nodes, num_nodes,
        edges, num_edges,
        triangles, num_triangles,
        minage, maxage,
        xline, yline, zline, nline,
        detach_id);

    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (triangles);
    nodes = NULL;
    edges = NULL;
    triangles = NULL;

    AddToOrigPointsList (
        XPointsIn, YPointsIn, ZPointsIn,
        NumPointsIn, id);

    AddToOrigLinesList (
        LinesIn,
        NumLinesIn, id);

    return 1;

  }
