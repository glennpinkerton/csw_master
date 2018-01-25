
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * Include system headers.
 */
#include <assert.h>
#include <math.h>
#include <stdlib.h>


/*
 * This define allows private csw functions to be used.
 */
#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

/*
 * General csw includes.
 */
#include <csw/hlevutils/src/simulate_new.h>

#include <csw/utils/include/csw_.h>
#include <csw/utils/private_include/csw_scope.h>

#include <csw/utils/private_include/gpf_utils.h>
#include <csw/utils/private_include/ply_utils.h>

#include <csw/surfaceworks/private_include/Vert.h>
#include <csw/surfaceworks/private_include/grd_utils.h>
#include <csw/surfaceworks/include/grd_xyzindex.h>

/*
 * Local headers.
 */
#include "moller.h"
#include "PadSurfaceForSim.h"
#include "SealedModel.h"


#define LIMIT_LINE_FLAG    137
#define TETGEN_INSIDE_EDGE_FLAG   1000000



void SealedModel::freeMemory (void)
{
    free_mem ();
}



/*-----------------------------------------------------------------------*/

/*
 * Initialize the private variables appropriately.  This should only
 * be called from constructors.
 *
 * This is a protected method.
 */
void SealedModel::init (void)
{

    sideFaultStart = -1;
    sideFaultEnd = -1;

    InputHorizonList = NULL;
    NumInputHorizonList = 0;
    MaxInputHorizonList = 0;
    InputFaultList = NULL;
    NumInputFaultList = 0;
    MaxInputFaultList = 0;
    InputDetachmentList = NULL;
    NumInputDetachmentList = 0;
    MaxInputDetachmentList = 0;

    FaultList = NULL;
    HorizonList = NULL;
    NumFaultList = 0;
    NumHorizonList = 0;
    MaxFaultList = 0;
    MaxHorizonList = 0;

    NorthBoundarySurface = NULL;
    SouthBoundarySurface = NULL;
    EastBoundarySurface = NULL;
    WestBoundarySurface = NULL;

    EmbeddedNorthBoundarySurface = NULL;
    EmbeddedSouthBoundarySurface = NULL;
    EmbeddedEastBoundarySurface = NULL;
    EmbeddedWestBoundarySurface = NULL;

    InputSedimentSurface = NULL;
    InputModelBottom = NULL;
    PaddedSedimentSurface = NULL;
    PaddedModelBottom = NULL;
    SealedSedimentSurface = NULL;
    SealedModelBottom = NULL;
    PaddedTop = NULL;
    PaddedBottom = NULL;
    SealedTop = NULL;
    SealedBottom = NULL;

    PaddedDetachment = NULL;
    SealedDetachment = NULL;

    PaddedHorizonList = NULL;
    NumPaddedHorizonList = 0;
    MaxPaddedHorizonList = 0;
    PaddedFaultList = NULL;
    NumPaddedFaultList = 0;
    MaxPaddedFaultList = 0;
    PaddedDetachmentList = NULL;
    NumPaddedDetachmentList = 0;
    MaxPaddedDetachmentList = 0;

    IndexPaddedFaults = 0;

    SealedHorizonList = NULL;
    NumSealedHorizonList = 0;
    MaxSealedHorizonList = 0;
    SealedFaultList = NULL;
    NumSealedFaultList = 0;
    MaxSealedFaultList = 0;

    averageSpacing = -1.0;
    verticalBoundarySpacing = -1.0;
    modelGrazeDistance = -1.0;

    maxRadialDist = -1.0;

    PaddedTriangle3DIndex = NULL;
    InputTriangle3DIndex = NULL;

    WorkIntersectionSegments = NULL;
    NumWorkIntersectionSegments = 0;
    MaxWorkIntersectionSegments = 0;

    WorkIntersectionLines = NULL;
    NumWorkIntersectionLines = 0;
    MaxWorkIntersectionLines = 0;

    IntersectionLines = NULL;
    NumIntersectionLines = 0;

    HorizonOutlineList = NULL;

    SealedHorizonIntersects = NULL;
    NumSealedHorizonIntersects = 0;
    MaxSealedHorizonIntersects = 0;

    SealedTopAndBottomIntersects = NULL;
    NumSealedTopAndBottomIntersects = 0;
    MaxSealedTopAndBottomIntersects = 0;

    SealedFaultIntersects = NULL;
    NumSealedFaultIntersects = 0;
    MaxSealedFaultIntersects = 0;

    WorkFaultIntersects = NULL;
    NumWorkFaultIntersects = 0;
    MaxWorkFaultIntersects = 0;

    modelXmin = modelYmin = modelZmin = 1.e30;
    modelXmax = modelYmax = modelZmax = -1.e30;
    origModelZmin = 1.e30;
    origModelZmax = -1.e30;
    modelZScale = 1.e30;
    padXmin = padYmin = padZmin = 1.e30;
    padXmax = padYmax = padZmax = -1.e30;

    padXEast = padXWest = padYNorth = padYSouth = 1.e30;

    modelTiny = 0.0001;

    TetgenX = NULL;
    TetgenY = NULL;
    TetgenZ = NULL;
    TetgenNodeMark = NULL;
    TetgenNode1 = NULL;
    TetgenNode2 = NULL;
    TetgenNode3 = NULL;
    TetgenFacetMark = NULL;
    NumTetgenNodes = 0;
    MaxTetgenNodes = 0;
    NumTetgenFacets = 0;
    MaxTetgenFacets = 0;

    simSealFlag = 1;
    simOutputFlag = 0;
    sealFlagLock = 0;
    marginFraction = 0.0;

    int    i;
    memset (AttribNameBuffer, 0, 100 * _MAX_ATTRIBS_);
    for (i=0; i<_MAX_ATTRIBS_; i++) {
        AttribNames[i] = AttribNameBuffer + 100 * i;
    }
    NumAttribs = 0;

    NodeIndexGrid = NULL;
    IndexXmin = 1.e30;
    IndexYmin = 1.e30;
    IndexXspace = -1.0;
    IndexYspace = -1.0;
    IndexNcol = -1;
    IndexNrow = -1;

    return;

}


/*----------------------------------------------------------------------*/

/*
 * Free the memory for the private data members and re initialize them
 * all to their empty values.
 *
 * This is a protected method.
 */
void SealedModel::free_mem (void)
{
    int                 i;
    _INtersectionLine_  *ilptr;

    FreeTmesh (InputHorizonList, NumInputHorizonList);
    FreeTmesh (InputFaultList, NumInputFaultList);

    FreeTmesh (NorthBoundarySurface, 1);
    FreeTmesh (SouthBoundarySurface, 1);
    FreeTmesh (EastBoundarySurface, 1);
    FreeTmesh (WestBoundarySurface, 1);

    FreeTmesh (InputSedimentSurface, 1);
    FreeTmesh (InputModelBottom, 1);
    FreeTmesh (PaddedSedimentSurface, 1);
    FreeTmesh (PaddedModelBottom, 1);
    FreeTmesh (SealedSedimentSurface, 1);
    FreeTmesh (SealedModelBottom, 1);

    FreeTmesh (PaddedDetachment, 1);

    FreeTmesh (PaddedBottom, 1);
    FreeTmesh (PaddedTop, 1);
    FreeTmesh (SealedBottom, 1);
    FreeTmesh (SealedTop, 1);

    InputHorizonList = NULL;
    InputFaultList = NULL;
    NumInputHorizonList = 0;
    NumInputFaultList = 0;
    MaxInputHorizonList = 0;
    MaxInputFaultList = 0;

    NorthBoundarySurface = NULL;
    SouthBoundarySurface = NULL;
    EastBoundarySurface = NULL;
    WestBoundarySurface = NULL;

    InputSedimentSurface = NULL;
    InputModelBottom = NULL;
    PaddedSedimentSurface = NULL;
    PaddedModelBottom = NULL;
    SealedSedimentSurface = NULL;
    SealedModelBottom = NULL;
    PaddedBottom = NULL;
    PaddedTop = NULL;
    SealedBottom = NULL;
    SealedTop = NULL;

  /*
   * The embedded boundary surfaces are just pointers into
   * the sealed fault list array, so they should not be
   * csw_Freed, but rather should just be set to NULL.
   */
    EmbeddedNorthBoundarySurface = NULL;
    EmbeddedSouthBoundarySurface = NULL;
    EmbeddedEastBoundarySurface = NULL;
    EmbeddedWestBoundarySurface = NULL;

    FreeTmesh (PaddedHorizonList, NumPaddedHorizonList);
    FreeTmesh (PaddedFaultList, NumPaddedFaultList);
    PaddedHorizonList = NULL;
    PaddedFaultList = NULL;
    NumPaddedHorizonList = 0;
    NumPaddedFaultList = 0;
    MaxPaddedHorizonList = 0;
    MaxPaddedFaultList = 0;

    FreeTmesh (SealedHorizonList, NumSealedHorizonList);
    FreeTmesh (SealedFaultList, NumSealedFaultList);
    SealedHorizonList = NULL;
    SealedFaultList = NULL;
    NumSealedHorizonList = 0;
    NumSealedFaultList = 0;
    MaxSealedHorizonList = 0;
    MaxSealedFaultList = 0;

    if (PaddedTriangle3DIndex != NULL) {
        delete (PaddedTriangle3DIndex);
    }
    PaddedTriangle3DIndex = NULL;

    if (InputTriangle3DIndex != NULL) {
        delete (InputTriangle3DIndex);
    }
    InputTriangle3DIndex = NULL;

    csw_Free (WorkIntersectionSegments);
    WorkIntersectionSegments = NULL;
    NumWorkIntersectionSegments = 0;
    MaxWorkIntersectionSegments = 0;

    if (WorkIntersectionLines != NULL) {
        for (i=0; i<NumWorkIntersectionLines; i++) {
            ilptr = WorkIntersectionLines + i;
            csw_Free (ilptr->x);
        }
        csw_Free (WorkIntersectionLines);
    }
    NumWorkIntersectionLines = 0;
    MaxWorkIntersectionLines = 0;
    WorkIntersectionLines = NULL;

    if (IntersectionLines != NULL) {
        for (i=0; i<NumIntersectionLines; i++) {
            ilptr = IntersectionLines + i;
            csw_Free (ilptr->x);
        }
        csw_Free (IntersectionLines);
    }
    NumIntersectionLines = 0;
    IntersectionLines = NULL;

    if (SealedHorizonIntersects != NULL) {
        for (i=0; i<NumSealedHorizonIntersects; i++) {
            ilptr = SealedHorizonIntersects + i;
            csw_Free (ilptr->x);
        }
        csw_Free (SealedHorizonIntersects);
    }
    NumSealedHorizonIntersects = 0;
    MaxSealedHorizonIntersects = 0;
    SealedHorizonIntersects = NULL;

    if (SealedTopAndBottomIntersects != NULL) {
        for (i=0; i<NumSealedTopAndBottomIntersects; i++) {
            ilptr = SealedTopAndBottomIntersects + i;
            csw_Free (ilptr->x);
        }
        csw_Free (SealedTopAndBottomIntersects);
    }
    NumSealedTopAndBottomIntersects = 0;
    MaxSealedTopAndBottomIntersects = 0;
    SealedTopAndBottomIntersects = NULL;

    if (SealedFaultIntersects != NULL) {
        for (i=0; i<NumSealedFaultIntersects; i++) {
            ilptr = SealedFaultIntersects + i;
            csw_Free (ilptr->x);
        }
        csw_Free (SealedFaultIntersects);
    }
    NumSealedFaultIntersects = 0;
    MaxSealedFaultIntersects = 0;
    SealedFaultIntersects = NULL;

    FreeTetgenData ();

    return;
}




/*--------------------------------------------------------------------*/

void SealedModel::setSimSealFlag (int ival)
{
    if (sealFlagLock) {
        return;
    }

    if (ival == LOCK_SEAL_FLAG) {
        sealFlagLock = 1;
        ival = 1;
    }

    simSealFlag = ival;
}

int SealedModel::getSimSealFlag (void)
{
    return simSealFlag;
}


/*--------------------------------------------------------------------*/
void SealedModel::setMarginFraction (double val)
{
    marginFraction = val;
    if (marginFraction < 0.001) marginFraction = 0.001;
    if (marginFraction > 100.0) marginFraction = 100.0;
    if (marginFraction > .05) {
        simSealFlag = 1;
        simOutputFlag = 1;
    }
    else {
        simSealFlag = 1;
        simOutputFlag = 0;
    }
}

double SealedModel::getMarginFraction (void)
{
    return marginFraction;
}

/*--------------------------------------------------------------------*/

int SealedModel::padModel (
    double        fractionXY,
    double        fractionZ,
    double        avspace)
{
    double        dx, dy, dz,
                  xmin, ymin, zmin,
                  xmax, ymax, zmax;

    CalcModelBounds ();

    dx = modelXmax - modelXmin;
    dy = modelYmax - modelYmin;
    dz = modelZmax - modelZmin;

    dx *= fractionXY;
    dy *= fractionXY;
    dz *= fractionZ;

    xmin = modelXmin - dx;
    ymin = modelYmin - dy;
    zmin = modelZmin - dz;
    xmax = modelXmax + dx;
    ymax = modelYmax + dy;
    zmax = modelZmax + dz;

    int istat =
      padModel (xmin, ymin, zmin,
                xmax, ymax, zmax,
                avspace);
    return istat;
}



/*--------------------------------------------------------------------*/

/**
 *
 * Extend the horizons and faults to the specified limits.
 */

int SealedModel::padModel (
    double        xmin,
    double        ymin,
    double        zmin,
    double        xmax,
    double        ymax,
    double        zmax,
    double        avspace)
{
    int              i, istat;
    CSWTriMeshStruct *tmesh = NULL;
    NOdeStruct       *nodes = NULL;
    EDgeStruct       *edges = NULL;
    TRiangleStruct   *triangles = NULL;
    int              num_nodes_out,
                     num_edges_out,
                     num_tris_out;
    double           tiny, ztiny;
    double           xcenter, ycenter;

    CSW_F            *grid;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
            FreePaddedLists ();
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (InputHorizonList == NULL) {
        return -1;
    }

    FreePaddedLists ();

    if (marginFraction > 0.0) {
        double dx, dy, dz;
        dx = (xmax - xmin) * marginFraction;
        dy = (ymax - ymin) * marginFraction;
        dz = (zmax - zmin) * marginFraction;
        if (marginFraction < 0.05) {
            dz = (zmax - zmin) * .05;
        }
        xmin -= dx;
        xmax += dx;
        ymin -= dy;
        ymax += dy;
        zmin -= dz;
        zmax += dz;
    }

    padXmin = xmin;
    padYmin = ymin;
    padZmin = zmin;
    padXmax = xmax;
    padYmax = ymax;
    padZmax = zmax;

/*
 * Calculate the average spacing for all the padded horizons.
 */
    averageSpacing = avspace;
    if (averageSpacing <= 0.0) {
        double         dsum;
        int            nedges, j, ntot;
        EDgeStruct     *eptr;
        dsum = 0.0;
        ntot = 0;
        for (i=0; i<NumInputHorizonList; i++) {
            edges = InputHorizonList[i].edges;
            nedges = InputHorizonList[i].num_edges;
            nodes = InputHorizonList[i].nodes;
            for (j=0; j<nedges; j++) {
                eptr = edges + j;
                if (eptr->deleted) continue;
                if (eptr->length > 0.0  &&  eptr->length < 1.e20) {
                    dsum += eptr->length;
                    ntot++;
                }
                else {
                    double      dx, dy, dist;
                    NOdeStruct  *np1, *np2;
                    np1 = nodes + eptr->node1;
                    np2 = nodes + eptr->node2;
                    dx = np1->x - np2->x;
                    dy = np1->y - np2->y;
                    dist = dx * dx + dy * dy;
                    dist = sqrt (dist);
                    eptr->length = dist;
                    dsum += dist;
                    ntot++;
                }
            }
        }
        if (ntot > 0) {
            averageSpacing = dsum / ntot;
        }
        else {
            averageSpacing = (xmax - xmin + ymax - ymin) / 100.0;
        }
    }

    CalcModelBounds ();

    origModelZmin = modelZmin;
    origModelZmax = modelZmax;

  /*
   * Extend the z limits so the vertical side boundaries will
   * intersect with each padded surface with plenty of room
   * to spare.
   */
    ztiny = (padZmax - padZmin) / 10.0;
    padZmax += ztiny;
    if (padZmax < modelZmax + ztiny) {
        padZmax = modelZmax + ztiny;
    }
    if (padZmin > modelZmin - ztiny) {
        padZmin = modelZmin - ztiny;
    }
    zmin = padZmin;
    zmax = padZmax;

  /*
   * Make vertical boundary surfaces at the pad limits.
   */
    istat =
    CreateVerticalBoundaries ();
    if (istat == -1) {
        FreePaddedLists ();
        FreeVerticalBoundaries ();
        return -1;
    }

  /*
   * Extend the x and y extents slightly to insure that the
   * padded surfaces and faults intersect the vertical boundaries
   * with plenty of room to spare.
   */
    tiny = (xmax - xmin + ymax - ymin) / 20.0;
    padXmin -= tiny;
    padYmin -= tiny;
    padXmax += tiny;
    padYmax += tiny;
    xmin -= tiny;
    ymin -= tiny;
    xmax += tiny;
    ymax += tiny;

    int      ncol, nrow;
    double   gxmin, gymin, gxmax, gymax;

  /*
   * Pad the sediment surface and model bottom.
   */
    pad_surface_obj.PadSetPadShapeGrid (NULL, 0, 0,
                        1.e30, 1.e30, -1.e30, -1.e30);

    tmesh = InputSedimentSurface;
    if (tmesh) {
        istat =
          pad_surface_obj.PadSurfaceForSim (
              tmesh->nodes,
              tmesh->edges,
              tmesh->tris,
              tmesh->num_nodes,
              tmesh->num_edges,
              tmesh->num_tris,
              xmin,
              xmax,
              ymin,
              ymax,
              zmin,
              zmax,
              &grid,
              &ncol,
              &nrow,
              &gxmin,
              &gymin,
              &gxmax,
              &gymax,
              &nodes,
              &edges,
              &triangles,
              &num_nodes_out,
              &num_edges_out,
              &num_tris_out,
              averageSpacing);
        if (istat == -1) {
            return -1;
        }
        FreeTmesh (PaddedSedimentSurface, 1);
        PaddedSedimentSurface = (CSWTriMeshStruct *)csw_Calloc (sizeof(CSWTriMeshStruct));
        if (PaddedSedimentSurface == NULL) {
            return -1;
        }
        PaddedSedimentSurface->nodes = nodes;
        PaddedSedimentSurface->edges = edges;
        PaddedSedimentSurface->tris = triangles;
        PaddedSedimentSurface->num_nodes = num_nodes_out;
        PaddedSedimentSurface->num_edges = num_edges_out;
        PaddedSedimentSurface->num_tris = num_tris_out;
        PaddedSedimentSurface->id = _SED_SURF_ID_;
        xcenter = (xmin + xmax) / 2.0;
        ycenter = (ymin + ymax) / 2.0;
        PaddedSedimentSurface->xcenter = xcenter;
        PaddedSedimentSurface->ycenter = ycenter;
        PaddedSedimentSurface->external_id = tmesh->external_id;
        PaddedSedimentSurface->age = tmesh->age;

        PaddedSedimentSurface->grid = grid;
        PaddedSedimentSurface->ncol = ncol;
        PaddedSedimentSurface->nrow = nrow;
        PaddedSedimentSurface->gxmin = gxmin;
        PaddedSedimentSurface->gymin = gymin;
        PaddedSedimentSurface->gxmax = gxmax;
        PaddedSedimentSurface->gymax = gymax;

    }

  /*
   * Pad the horizons.
   */
    if (InputHorizonList != NULL) {
        for (i=0; i<NumInputHorizonList; i++) {
            tmesh = InputHorizonList + i;
            istat =
              CalcTriMeshCentroid (tmesh, &xcenter, &ycenter);
            if (istat == -1) {
                return -1;
            }

            istat =
              pad_surface_obj.PadSurfaceForSim (
                  tmesh->nodes,
                  tmesh->edges,
                  tmesh->tris,
                  tmesh->num_nodes,
                  tmesh->num_edges,
                  tmesh->num_tris,
                  xmin,
                  xmax,
                  ymin,
                  ymax,
                  zmin,
                  zmax,
                  &grid,
                  &ncol,
                  &nrow,
                  &gxmin,
                  &gymin,
                  &gxmax,
                  &gymax,
                  &nodes,
                  &edges,
                  &triangles,
                  &num_nodes_out,
                  &num_edges_out,
                  &num_tris_out,
                  averageSpacing);
            if (istat == -1) {
                return -1;
            }
            istat =
              AddPaddedHorizon (
                  tmesh->external_id,
                  tmesh->age,
                  nodes, num_nodes_out,
                  edges, num_edges_out,
                  triangles, num_tris_out,
                  xcenter, ycenter);
            if (istat == -1) {
                return -1;
            }
            tmesh->grid = grid;
            tmesh->ncol = ncol;
            tmesh->nrow = nrow;
            tmesh->gxmin = gxmin;
            tmesh->gymin = gymin;
            tmesh->gxmax = gxmax;
            tmesh->gymax = gymax;
        }
    }

    tmesh = InputModelBottom;
    if (tmesh) {
        istat =
          pad_surface_obj.PadSurfaceForSim (
              tmesh->nodes,
              tmesh->edges,
              tmesh->tris,
              tmesh->num_nodes,
              tmesh->num_edges,
              tmesh->num_tris,
              xmin,
              xmax,
              ymin,
              ymax,
              zmin,
              zmax,
              &grid,
              &ncol,
              &nrow,
              &gxmin,
              &gymin,
              &gxmax,
              &gymax,
              &nodes,
              &edges,
              &triangles,
              &num_nodes_out,
              &num_edges_out,
              &num_tris_out,
              averageSpacing);
        if (istat == -1) {
            return -1;
        }
        FreeTmesh (PaddedModelBottom, 1);
        PaddedModelBottom = (CSWTriMeshStruct *)csw_Calloc (sizeof(CSWTriMeshStruct));
        if (PaddedModelBottom == NULL) {
            return -1;
        }
        PaddedModelBottom->nodes = nodes;
        PaddedModelBottom->edges = edges;
        PaddedModelBottom->tris = triangles;
        PaddedModelBottom->num_nodes = num_nodes_out;
        PaddedModelBottom->num_edges = num_edges_out;
        PaddedModelBottom->num_tris = num_tris_out;
        PaddedModelBottom->id = _MODEL_BOTTOM_ID_;
        xcenter = (xmin + xmax) / 2.0;
        ycenter = (ymin + ymax) / 2.0;
        PaddedModelBottom->xcenter = xcenter;
        PaddedModelBottom->ycenter = ycenter;
        PaddedModelBottom->external_id = tmesh->external_id;
        PaddedModelBottom->age = tmesh->age;

        PaddedModelBottom->grid = grid;
        PaddedModelBottom->ncol = ncol;
        PaddedModelBottom->nrow = nrow;
        PaddedModelBottom->gxmin = gxmin;
        PaddedModelBottom->gymin = gymin;
        PaddedModelBottom->gxmax = gxmax;
        PaddedModelBottom->gymax = gymax;

    }

  /*
   * Calculate a plane fit to each fault and pad it.
   */
    if (InputFaultList != NULL) {

        SurfaceGroupPlane   *sgp = NULL;
        double       pcoef[3];
        double       xorigin, yorigin, zorigin;

        bool     lsuccess = false;

        auto lscope = [&]()
        {
            if (lsuccess == false) {
                delete (sgp);
                FreePaddedLists ();
            }
        };
        CSWScopeGuard loc_scope_guard (lscope);

        try {
            SNF;
            sgp = new SurfaceGroupPlane ();
        }
        catch (...) {
            printf ("\n***** Exception from new *****\n\n");
            sgp = NULL;
            return -1;
        }

        for (i=0; i<NumInputFaultList; i++) {
            tmesh = InputFaultList + i;

            istat =
              sgp->addTriMeshForFit (
                tmesh->nodes, tmesh->num_nodes,
                tmesh->edges, tmesh->num_edges,
                tmesh->tris, tmesh->num_tris);
            if (istat == -1) {
                return -1;
            }

            istat =
              sgp->calcPlaneCoefs ();
            sgp->freeFitPoints ();
            if (istat == -1) {
                return -1;
            }

            istat =
              sgp->getCoefsAndOrigin (
                pcoef, pcoef+1, pcoef+2,
                &xorigin, &yorigin, &zorigin);
            if (istat == -1) {
                return -1;
            }

            tmesh->vbase[0] = pcoef[0];
            tmesh->vbase[1] = pcoef[1];
            tmesh->vbase[2] = pcoef[2];
            tmesh->vbase[3] = xorigin;
            tmesh->vbase[4] = yorigin;
            tmesh->vbase[5] = zorigin;
            tmesh->vflag = 1;

            pad_surface_obj.PadSetSurfaceGroupPlane (sgp);
            pad_surface_obj.PadSetDetachmentContact (
                tmesh->xdetach,
                tmesh->ydetach,
                tmesh->zdetach,
                tmesh->ndetach);

            istat =
              pad_surface_obj.PadFaultSurfaceForSim (
                  tmesh->nodes,
                  tmesh->edges,
                  tmesh->tris,
                  tmesh->num_nodes,
                  tmesh->num_edges,
                  tmesh->num_tris,
                  xmin,
                  xmax,
                  ymin,
                  ymax,
                  zmin,
                  zmax,
                  &grid,
                  &ncol,
                  &nrow,
                  &gxmin,
                  &gymin,
                  &gxmax,
                  &gymax,
                  &nodes,
                  &edges,
                  &triangles,
                  &num_nodes_out,
                  &num_edges_out,
                  &num_tris_out,
                  averageSpacing,
                  tmesh->minage,
                  tmesh->maxage);
            if (istat == -1) {
                return -1;
            }
            istat =
              AddPaddedFault (
                  tmesh->external_id,
                  nodes, num_nodes_out,
                  edges, num_edges_out,
                  triangles, num_tris_out,
                  tmesh->vflag, tmesh->vbase,
                  tmesh->minage, tmesh->maxage,
                  tmesh->xdetach, tmesh->ydetach, tmesh->zdetach,
                  tmesh->ndetach, tmesh->detach_id);
            if (istat == -1) {
                return -1;
            }
            tmesh->grid = grid;
            tmesh->ncol = ncol;
            tmesh->nrow = nrow;
            tmesh->gxmin = gxmin;
            tmesh->gymin = gymin;
            tmesh->gxmax = gxmax;
            tmesh->gymax = gymax;
            tmesh->is_padded = 1;
        }

        delete sgp;

        pad_surface_obj.PadSetSurfaceGroupPlane (NULL);
        pad_surface_obj.PadSetDetachmentContact (NULL, NULL, NULL, 0);

        lsuccess = true;

    }  // end of if block for fault list

    WriteDebugFiles ();

    bsuccess = true;

    return 1;

}




/*--------------------------------------------------------------------*/

/**
 * Calculate new fault and horizon patches that are sealed with each
 * other and with the sides and top of the model.  On success, 1 is
 * returned.  If there are no padded surfaces to work with, zero is
 * returned.  On a memory allocation failure, -1 is returned.
 */
int SealedModel::sealPaddedModel (void)
{
    int           i, j, istat;

    double        maxrdist, dtot, dist;
    double        xcenter, ycenter, zcenter,
                  xcenter3d, ycenter3d, zcenter3d;
    double        dx, dy, dz;

    _INtersectionLine_ *iptr;
    double    *xresamp, *yresamp, *zresamp;
    int       nresamp, maxresamp;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        FaultList = NULL;
        HorizonList = NULL;
        NumFaultList = 0;
        NumHorizonList = 0;
        MaxFaultList = 0;
        MaxHorizonList = 0;
        if (bsuccess == false) {
            csw_Free (xresamp);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (NumPaddedHorizonList + NumPaddedFaultList < 2) {
        return 0;
    }

    if (PaddedHorizonList == NULL  ||  PaddedFaultList == NULL) {
        return 0;
    }

/*
 * If the padded sediment surface is missing, use a horizontal
 * surface at padZmax.  If the padded model bottom is missing,
 * use a horizontal surface at padZmin.
 */
    if (simSealFlag == 1) {
        CreateDefaultPaddedTop ();
        CreateDefaultPaddedBottom ();
    }

    if (PaddedTriangle3DIndex != NULL) {
        delete (PaddedTriangle3DIndex);
    }
    PaddedTriangle3DIndex = NULL;

/*
 * Create a new triangle index for padded surfaces.
 */
    istat = CreatePadded3DTriangleIndex ();
    if (istat == -1  ||  PaddedTriangle3DIndex == NULL) {
        return -1;
    }

    if (modelGrazeDistance <= 0.0) {
        CalcModelGrazeDistance ();
    }

/*
 * The maximum radial distance for variable decimation is the
 * average distance from the center of the model box to the
 * mid point of each vertical side of the padded model.
 */
    dtot = 0.0;

    xcenter3d = (modelXmin + modelXmax) / 2.0;
    ycenter3d = (modelYmin + modelYmax) / 2.0;
    zcenter3d = (modelZmin + modelZmax) / 2.0;

    xcenter = (padXmin + padXmax) / 2.0;
    ycenter = padYmin;
    zcenter = (padZmin + padZmax) / 2.0;
    dz = zcenter - zcenter3d;

    dx = xcenter - xcenter3d;
    dy = ycenter - ycenter3d;
    dist = dx * dx + dy * dy + dz * dz;
    dist = sqrt (dist);
    dtot += dist;

    ycenter = padYmax;
    dy = ycenter - ycenter3d;
    dist = dx * dx + dy * dy + dz * dz;
    dist = sqrt (dist);
    dtot += dist;

    xcenter = padXmin;
    ycenter = (padYmin + padYmax) / 2.0;
    dx = xcenter - xcenter3d;
    dy = ycenter - ycenter3d;
    dist = dx * dx + dy * dy + dz * dz;
    dist = sqrt (dist);
    dtot += dist;

    xcenter = padXmax;
    dx = xcenter - xcenter3d;
    dist = dx * dx + dy * dy + dz * dz;
    dist = sqrt (dist);
    dtot += dist;

    maxrdist = dtot / 4.0;

    maxRadialDist = maxrdist;

    FaultList = PaddedFaultList;
    HorizonList = PaddedHorizonList;
    NumFaultList = NumPaddedFaultList;
    NumHorizonList = NumPaddedHorizonList;
    MaxFaultList = MaxPaddedFaultList;
    MaxHorizonList = MaxPaddedHorizonList;

/*
 * Calculate intersections between horizons and the boundaries.
 */
    istat =
    CalcPaddedSurfaceBoundaryIntersections ();

/*
 * Calculate horizon to fault intersections.
 */
    double    hage, fage1, fage2;

    for (i=0; i<NumPaddedHorizonList; i++) {
        hage = PaddedHorizonList[i].age;
        for (j=0; j<NumPaddedFaultList; j++) {
            fage1 = PaddedFaultList[j].minage;
            fage2 = PaddedFaultList[j].maxage;
            if (hage < fage1  ||  hage > fage2) {
                continue;
            }
            istat =
              CalcPaddedSurfaceIntersectionLines (
                  i, _HORIZON_TMESH_,
                  j, _FAULT_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
    }

    if (IntersectionLines == NULL  ||  NumIntersectionLines < 1) {
        return 0;
    }

    WriteIntersectionLines ((char *)"rawint.xyz");

    WritePartialIntersectionLines (
        (char *)"rawside.xyz",
        sideFaultStart, sideFaultEnd);

    CalcModelBounds ();

    if (modelXmin > modelXmax) {
        return 0;
    }

/*
 * Decimate and resample the intersection lines.
 */
    for (i=0; i<NumIntersectionLines; i++) {
        iptr = IntersectionLines + i;
        maxresamp = iptr->npts * 10;
        if (maxresamp < 1000) maxresamp = 1000;
        xresamp = (double *)csw_Malloc (3 * maxresamp * sizeof(double));
        if (xresamp == NULL) {
            return -1;
        }
        yresamp = xresamp + maxresamp;
        zresamp = yresamp + maxresamp;
        istat =
          grd_api_obj.grd_ResampleXYZLine (
            iptr->x, iptr->y, iptr->z, iptr->npts,
            averageSpacing,
            xresamp, yresamp, zresamp, &nresamp,
            maxresamp);
        if (istat == -1) {
            return -1;
        }
        csw_Free (iptr->x);
        iptr->x = xresamp;
        iptr->y = yresamp;
        iptr->z = zresamp;
        iptr->npts = nresamp;
        xresamp = NULL;
        yresamp = NULL;
        zresamp = NULL;
    }

    WriteIntersectionLines ((char *)"decint.xyz");
    WritePartialIntersectionLines (
        (char *)"decside.xyz",
        sideFaultStart, sideFaultEnd);

    xresamp = NULL;
    yresamp = NULL;
    zresamp = NULL;
    nresamp = 0;

    WriteDebugFiles ();

/*
 * Use the intersection lines and the padded surface
 * original borders to build the new sealed horizon
 * surfaces.  This will build the SealedHorizonList.
 */
    FreeSealedLists ();
    istat =
      OutlineAndCropHorizons ();
    if (istat == -1) {
        return -1;
    }

    WriteSealedHorizonIntersects ((char *)"postoutline.xyz");

    WriteDebugFiles ();

/*
 * Apply the intersection lines to faults as exact constraints.
 */
    for (i=0; i<NumFaultList; i++) {
        istat =
          EmbedEdgesInFault (_FAULT_ID_BASE_ + i);
        WriteDebugFiles ();
        if (istat == -1) {
            return -1;
        }
    }

    FixSealedFaultBoundaryIntersects ();

    WriteDebugFiles ();

/*
 * Do not embed edges in the vertical boundaries unless the
 * simSealFlag and simOutputFlag are true.
 */
    if (simSealFlag  &&  simOutputFlag) {
        istat =
          EmbedEdgesInBoundary (_BOUNDARY_ID_BASE_ + 1);
        if (istat == -1) {
            return -1;
        }
        if (SealedFaultList) {
            EmbeddedNorthBoundarySurface = SealedFaultList + NumSealedFaultList - 1;
        }

        WriteDebugFiles ();

        istat =
          EmbedEdgesInBoundary (_BOUNDARY_ID_BASE_ + 2);
        if (istat == -1) {
            return -1;
        }
        if (SealedFaultList) {
            EmbeddedSouthBoundarySurface = SealedFaultList + NumSealedFaultList - 1;
        }

        WriteDebugFiles ();

        istat =
          EmbedEdgesInBoundary (_BOUNDARY_ID_BASE_ + 3);
        if (istat == -1) {
            return -1;
        }
        if (SealedFaultList) {
            EmbeddedEastBoundarySurface = SealedFaultList + NumSealedFaultList - 1;
        }

        WriteDebugFiles ();

        istat =
          EmbedEdgesInBoundary (_BOUNDARY_ID_BASE_ + 4);
        if (istat == -1) {
            return -1;
        }
        if (SealedFaultList) {
            EmbeddedWestBoundarySurface = SealedFaultList + NumSealedFaultList - 1;
        }

        WriteDebugFiles ();

    /*
     * TEMP
     */
        istat = FixBoundaryCorners ();
        if (istat == -1) {
            return -1;
        }

    }

    WriteDebugFiles ();

    bsuccess = true;

    return 1;

}


/*--------------------------------------------------------------------*/

/*
 * Return the unsealed intersections.  This is meant for getting the
 * results of calcFaultHorizonIntersects back.
 */
_INtersectionLineList_ * SealedModel::getRawIntersectionLines (void)
{
    _INtersectionLineList_    *result = NULL;

    result = (_INtersectionLineList_ *)csw_Calloc (sizeof(_INtersectionLineList_));
    if (result == NULL) {
        return NULL;
    }

    result->list = (const _INtersectionLine_ *)IntersectionLines;
    result->nlist = NumIntersectionLines;

    return result;

}

/*--------------------------------------------------------------------*/

/**
 * Retrieve the intersection lines calculated via the most recent call
 * to sealInputModel or sealPaddedModel.  This is primarily used for
 * debugging.  This does not return a copy of the intersection lines.
 * The list pointer in the result is the actual pointer from the object.
 * This list pointer is const in the result structure.  The calling function
 * cannot modify the list.
 */
_INtersectionLineList_ * SealedModel::getHorizonIntersectionLines (void)
{
    _INtersectionLineList_    *result = NULL;

    result = (_INtersectionLineList_ *)csw_Calloc (sizeof(_INtersectionLineList_));
    if (result == NULL) {
        return NULL;
    }

    result->list = (const _INtersectionLine_ *)SealedHorizonIntersects;
    result->nlist = NumSealedHorizonIntersects;

    return result;

}

_INtersectionLineList_ * SealedModel::getFaultIntersectionLines (void)
{
    _INtersectionLineList_    *result = NULL;

    result = (_INtersectionLineList_ *)csw_Calloc (sizeof(_INtersectionLineList_));
    if (result == NULL) {
        return NULL;
    }

    result->list = (const _INtersectionLine_ *)SealedFaultIntersects;
    result->nlist = NumSealedFaultIntersects;

    return result;

}

_INtersectionLineList_ * SealedModel::getTopAndBottomIntersectionLines (void)
{
    _INtersectionLineList_    *result = NULL;

    result = (_INtersectionLineList_ *)csw_Calloc (sizeof(_INtersectionLineList_));
    if (result == NULL) {
        return NULL;
    }

    result->list = (const _INtersectionLine_ *)SealedTopAndBottomIntersects;
    result->nlist = NumSealedTopAndBottomIntersects;

    return result;

}


/*--------------------------------------------------------------------*/

void SealedModel::getInputHorizons (CSWTriMeshStruct **list, int *nlist)
{
    *list = InputHorizonList;
    *nlist = NumInputHorizonList;
}


/*--------------------------------------------------------------------*/

void SealedModel::getInputFaults (CSWTriMeshStruct **list, int *nlist)
{
    *list = InputFaultList;
    *nlist = NumInputFaultList;
}



/*--------------------------------------------------------------------*/

void SealedModel::getPaddedHorizons (CSWTriMeshStruct **list, int *nlist)
{
    *list = PaddedHorizonList;
    *nlist = NumPaddedHorizonList;
}


/*--------------------------------------------------------------------*/

void SealedModel::getPaddedFaults (CSWTriMeshStruct **list, int *nlist)
{
    *list = PaddedFaultList;
    *nlist = NumPaddedFaultList;
}

/*--------------------------------------------------------------------*/

void SealedModel::getPaddedTopAndBottom (CSWTriMeshStruct **list)
{
    list[0] = PaddedSedimentSurface;
    list[1] = PaddedModelBottom;
}


/*--------------------------------------------------------------------*/

void SealedModel::getSealedHorizons (CSWTriMeshStruct **list, int *nlist)
{
    *list = SealedHorizonList;
    *nlist = NumSealedHorizonList;
}


/*--------------------------------------------------------------------*/

void SealedModel::getSealedFaults (CSWTriMeshStruct **list, int *nlist)
{
    *list = SealedFaultList;
    *nlist = NumSealedFaultList;
}


/*--------------------------------------------------------------------*/

void SealedModel::getSealedTopAndBottom (CSWTriMeshStruct **list)
{
    list[0] = SealedSedimentSurface;
    list[1] = SealedModelBottom;
    list[2] = SealedBottom;
    list[3] = SealedTop;
    if (simOutputFlag == 0) {
        list[2] = NULL;
        list[3] = NULL;
    }
}

/*--------------------------------------------------------------------*/

CSWTriMeshStruct *SealedModel::getSealedDetachment (void)
{
    return SealedDetachment;
}


/*--------------------------------------------------------------------*/

/*
 * This returns the north, south, east and west boundry trimesh
 * struct in the specified list.  The calling function should csw_Free
 * the list, but not the trimesh nodes, edges and triangles inside
 * the list.  This is intended for debug use.
 */
void SealedModel::getShallowBoundaryCopies (CSWTriMeshStruct **list, int *nlist)
{
    CSWTriMeshStruct    *tlist = NULL;
    int                 size;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (tlist);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    *list = NULL;
    *nlist = 0;

    if (NorthBoundarySurface == NULL  ||
        SouthBoundarySurface == NULL  ||
        EastBoundarySurface == NULL  ||
        WestBoundarySurface == NULL) {
        return;
    }

    if (simOutputFlag == 0) {
        return;
    }

    size = sizeof(CSWTriMeshStruct);

    tlist = (CSWTriMeshStruct *)csw_Calloc (4 * size);
    if (tlist == NULL) {
        return;
    }

    memcpy (tlist, NorthBoundarySurface, size);
    memcpy (tlist+1, SouthBoundarySurface, size);
    memcpy (tlist+2, EastBoundarySurface, size);
    memcpy (tlist+3, WestBoundarySurface, size);

    *list = tlist;
    *nlist = 4;

    bsuccess = true;

    return;

}




/*--------------------------------------------------------------------*/

/*
 * Calculate the intersection lines between two surfaces which already exist
 * in either the horizon list or the fault list.  The results are appended to
 * the current WorkIntersectionLines array.
 *
 * This method returns 1 on success or -1 on an error.
 * Success does not mean that any intersections were found.  You need to
 * check the NumWorkIntersectionLines variable to see if any intersections
 * were found.
 *
 * If both input surface numbers and types point to the same trimesh object,
 * no intersection is attempted and zero is returned.
 *
 * This is a protected method.
 */
int SealedModel::CalcInputSurfaceIntersectionLines (
    int    surf1_num,
    int    surf1_type,
    int    surf2_num,
    int    surf2_type)
{
    CSWTriMeshStruct    *surf1,
                        *surf2,
                        *stmp;
    TRiangleStruct      *tp1, *tp2;

    int                 istat, i, j, ntri, tmeshid1, tmeshid2;
    double              txmin, tymin, tzmin, txmax, tymax, tzmax;

/*
 * Make sure the triangles are indexed.
 */
    if (InputTriangle3DIndex == NULL) {
        istat = CreateInput3DTriangleIndex ();
        if (istat == -1  ||  InputTriangle3DIndex == NULL) {
            return -1;
        }
    }

    if (modelGrazeDistance <= 0.0) {
        CalcModelGrazeDistance ();
    }

/*
 * Get the trimesh objects for each surface.
 */
    if (surf1_type == _HORIZON_TMESH_) {
        if (InputHorizonList == NULL) {
            return -1;
        }
        if (surf1_num >= NumInputHorizonList) {
            return -1;
        }
        surf1 = InputHorizonList + surf1_num;
        tmeshid1 = surf1_num + _HORIZON_ID_BASE_;
    }
    else {
        if (InputFaultList == NULL) {
            return -1;
        }
        if (surf1_num >= NumInputFaultList) {
            return -1;
        }
        surf1 = InputFaultList + surf1_num;
        tmeshid1 = surf1_num + _FAULT_ID_BASE_;
    }

    if (surf2_type == _HORIZON_TMESH_) {
        if (InputHorizonList == NULL) {
            return -1;
        }
        if (surf2_num >= NumInputHorizonList) {
            return -1;
        }
        surf2 = InputHorizonList + surf2_num;
        tmeshid2 = surf2_num + _HORIZON_ID_BASE_;
    }
    else {
        if (InputFaultList == NULL) {
            return -1;
        }
        if (surf2_num >= NumInputFaultList) {
            return -1;
        }
        surf2 = InputFaultList + surf2_num;
        tmeshid2 = surf2_num + _FAULT_ID_BASE_;
    }

/*
 * Swap surf1 and surf2 if surf2 has less triangles.
 */
    if (surf2->num_tris < surf1->num_tris) {
        stmp = surf1;
        surf1 = surf2;
        surf2 = stmp;
        i = tmeshid1;
        tmeshid1 = tmeshid2;
        tmeshid2 = i;
    }

    ntri = surf1->num_tris;

/*
 * For each triangle in surf1, find any surf2 triangles close to
 * it (using the 3d triangle index).  Calculate intersection segments
 * between the surf1 triangle and each possible surf2 triangle.
 * The WorkIntersectionSegments list is updated with these intersections.
 */
    SPatial3DTriangleStructList *stout;
    SPatial3DTriangleStruct     *stlist, *stptr;
    int                         nstlist;
    double                      tiny;

    NumWorkIntersectionSegments = 0;

    for (i=0; i<ntri; i++) {

      /*
       * Find the 3d bounding box of the triangle.
       */
        tp1 = surf1->tris + i;
        Calc3DBox (tp1, surf1->edges, surf1->nodes,
                   &txmin, &tymin, &tzmin,
                   &txmax, &tymax, &tzmax,
                   &tiny);

      /*
       * Get all triangles within the bounding box that do not
       * belong to the surf1 trimesh.
       */
        stout =
            InputTriangle3DIndex->GetTriangles (
                tmeshid1,
                txmin, tymin, tzmin,
                txmax, tymax, tzmax);
        if (stout == NULL) {
            return -1;
        }

        stlist = stout->list;
        nstlist = stout->nlist;

        if (stlist == NULL  ||  nstlist < 1) {
            csw_Free (stout);
            csw_Free (stlist);
            stout = NULL;
            stlist = NULL;
            continue;
        }

      /*
       * Check the triangle intersections between the current
       * surf1 triangle and all the surf2 triangle candidates
       * returned from the indexing.  The CalcTriangleIntersect
       * method adds segments to the WorkIntersectionSegments
       * list as needed.
       */
        for (j=0; j<nstlist; j++) {
            stptr = stlist + j;
            if (stptr->tmeshid != tmeshid2) {
                continue;
            }
            tp2 = surf2->tris + stptr->trinum;
            CalcTriangleIntersection (tp1, tp2, modelGrazeDistance / 10.0,
                                      surf1->edges,
                                      surf1->nodes,
                                      surf2->edges,
                                      surf2->nodes);
        }

      /*
       * Free the results returned from GetTriangles and check the next triangle.
       */
        csw_Free (stlist);
        csw_Free (stout);
        stlist = NULL;
        stout = NULL;
        nstlist = 0;

    }  // end of loop through the surf1 triangles

/*
 * Connect the segments in the WorkIntersectionSegment list and
 * add the intersection lines to the WorkIntersectionLines list.
 * The intersection lines are marked as shared by the two specified
 * trimesh id's.
 */
    FindOverlapWorkSegments ();
    ConnectIntersectionSegments (tmeshid1, tmeshid2);

    surf1->numIntersects += NumWorkIntersectionLines;
    surf2->numIntersects += NumWorkIntersectionLines;

/*
 * Move the work intersectionlines to the results intersection line list.
 */
    istat =
    AddWorkLinesToResults ();

    return istat;

}


/*---------------------------------------------------------------------------*/

/*
 * Return the corner points of a 3d box slightly larger than the bounds of
 * the specified triangle.
 */
void SealedModel::Calc3DBox (
    TRiangleStruct    *tptr,
    EDgeStruct        *edges,
    NOdeStruct        *nodes,
    double            *txmin,
    double            *tymin,
    double            *tzmin,
    double            *txmax,
    double            *tymax,
    double            *tzmax,
    double            *tinyout)
{
    int               istat, n1, n2, n3;
    double            xt, yt, zt,
                      xmin, ymin, zmin,
                      xmax, ymax, zmax,
                      dx, dy, dz, tiny;

/*
 * Initialize output in case of error.
 */
    *txmin = *tymin = *tzmin = 1.e30;
    *txmax = *tymax = *tzmax = -1.e30;

    istat =
      grd_api_obj.grd_GetNodesForTriangle (
          tptr, edges,
          &n1, &n2, &n3);
    if (istat == -1) {
        return;
    }

    xmin = ymin = zmin = 1.e30;
    xmax = ymax = zmax = -1.e30;

    xt = nodes[n1].x;
    yt = nodes[n1].y;
    zt = nodes[n1].z;
    if (xt < xmin) xmin = xt;
    if (yt < ymin) ymin = yt;
    if (zt < zmin) zmin = zt;
    if (xt > xmax) xmax = xt;
    if (yt > ymax) ymax = yt;
    if (zt > zmax) zmax = zt;

    xt = nodes[n2].x;
    yt = nodes[n2].y;
    zt = nodes[n2].z;
    if (xt < xmin) xmin = xt;
    if (yt < ymin) ymin = yt;
    if (zt < zmin) zmin = zt;
    if (xt > xmax) xmax = xt;
    if (yt > ymax) ymax = yt;
    if (zt > zmax) zmax = zt;

    xt = nodes[n3].x;
    yt = nodes[n3].y;
    zt = nodes[n3].z;
    if (xt < xmin) xmin = xt;
    if (yt < ymin) ymin = yt;
    if (zt < zmin) zmin = zt;
    if (xt > xmax) xmax = xt;
    if (yt > ymax) ymax = yt;
    if (zt > zmax) zmax = zt;

    dx = (xmax - xmin);
    dy = (ymax - ymin);
    dz = (zmax - zmin);

    tiny = (dx + dy + dz) / 300.0;
    if (tiny < 0.0) tiny = 0.0;

    xmin -= tiny;
    ymin -= tiny;
    zmin -= tiny;
    xmax += tiny;
    ymax += tiny;
    zmax += tiny;

    *txmin = xmin;
    *tymin = ymin;
    *tzmin = zmin;
    *txmax = xmax;
    *tymax = ymax;
    *tzmax = zmax;
    *tinyout = tiny;

    return;

}



/*------------------------------------------------------------------------*/

/*
 * Calculate the intersection line segment, if there is one, between
 * two triangles.  If an intersection segment is found, it is added
 * to the WorkIntersectionSegments list.  On success, 1 is returned.
 * If the triangles do not intersect, zero is returned.  If there is
 * a memory allocation error adding the segment to the WorkIntersectionSegments
 * list, -1 is returned.
 *
 * This is a protected method.
 */
int SealedModel::CalcTriangleIntersection (
    TRiangleStruct   *tp1,
    TRiangleStruct   *tp2,
    double           tiny,
    EDgeStruct       *s1edges,
    NOdeStruct       *s1nodes,
    EDgeStruct       *s2edges,
    NOdeStruct       *s2nodes)
{
    double           t1_xyz1[3], t1_xyz2[3], t1_xyz3[3];
    double           t2_xyz1[3], t2_xyz2[3], t2_xyz3[3];
    double           seg_xyz1[3], seg_xyz2[3];
    double           dx, dy, dz, dist;
    int              istat, coplanar, n1, n2, n3;
    _INtersectionSegment_    *sptr;

    double           pxmin, pymin, pzmin;

    if (padXmin < padXmax  &&  padXmin < 1.e30) {
        pxmin = padXmin;
        pymin = padYmin;
        pzmin = padZmin;
    }
    else {
        pxmin = 0.0;
        pymin = 0.0;
        pzmin = 0.0;
    }

/*
 * Populate the xyz arrays for the two triangles.
 */
    istat =
      grd_api_obj.grd_GetNodesForTriangle (
        tp1, s1edges,
        &n1, &n2, &n3);
    if (istat == -1) {
        return -1;
    }
    t1_xyz1[0] = s1nodes[n1].x - pxmin;
    t1_xyz1[1] = s1nodes[n1].y - pymin;
    t1_xyz1[2] = s1nodes[n1].z - pzmin;
    t1_xyz2[0] = s1nodes[n2].x - pxmin;
    t1_xyz2[1] = s1nodes[n2].y - pymin;
    t1_xyz2[2] = s1nodes[n2].z - pzmin;
    t1_xyz3[0] = s1nodes[n3].x - pxmin;
    t1_xyz3[1] = s1nodes[n3].y - pymin;
    t1_xyz3[2] = s1nodes[n3].z - pzmin;

    istat =
      grd_api_obj.grd_GetNodesForTriangle (
        tp2, s2edges,
        &n1, &n2, &n3);
    if (istat == -1) {
        return -1;
    }
    t2_xyz1[0] = s2nodes[n1].x - pxmin;
    t2_xyz1[1] = s2nodes[n1].y - pymin;
    t2_xyz1[2] = s2nodes[n1].z - pzmin;
    t2_xyz2[0] = s2nodes[n2].x - pxmin;
    t2_xyz2[1] = s2nodes[n2].y - pymin;
    t2_xyz2[2] = s2nodes[n2].z - pzmin;
    t2_xyz3[0] = s2nodes[n3].x - pxmin;
    t2_xyz3[1] = s2nodes[n3].y - pymin;
    t2_xyz3[2] = s2nodes[n3].z - pzmin;

    int do_write;
    do_write = csw_GetDoWrite ();
    if (do_write) {
        char fname[200], c200[200];
        sprintf (fname, "tripoints.xyz");
        FILE *fptr;
        fptr = fopen (fname, "wb");
        if (fptr) {
            sprintf (c200, "triangle 1\n");
            fputs (c200, fptr);
            sprintf (c200, "%f %f %f\n", t1_xyz1[0], t1_xyz1[1], t1_xyz1[2]);
            fputs (c200, fptr);
            sprintf (c200, "%f %f %f\n", t1_xyz2[0], t1_xyz2[1], t1_xyz2[2]);
            fputs (c200, fptr);
            sprintf (c200, "%f %f %f\n", t1_xyz3[0], t1_xyz3[1], t1_xyz3[2]);
            fputs (c200, fptr);
            sprintf (c200, "triangle 2\n");
            fputs (c200, fptr);
            sprintf (c200, "%f %f %f\n", t2_xyz1[0], t2_xyz1[1], t2_xyz1[2]);
            fputs (c200, fptr);
            sprintf (c200, "%f %f %f\n", t2_xyz2[0], t2_xyz2[1], t2_xyz2[2]);
            fputs (c200, fptr);
            sprintf (c200, "%f %f %f\n", t2_xyz3[0], t2_xyz3[1], t2_xyz3[2]);
            fputs (c200, fptr);
            fclose (fptr);
            fptr = NULL;
        }
    }

/*
 * Initialize the segment in case of error.
 */
    seg_xyz1[0] = 1.e30;
    seg_xyz2[0] = 1.e30;

/*
 * Use the triangle intersection function from moller.cc.
 */
    istat =
      tri_tri_intersect_with_isectline (
         t1_xyz1, t1_xyz2, t1_xyz3,
         t2_xyz1, t2_xyz2, t2_xyz3,
         &coplanar,
         seg_xyz1, seg_xyz2);
    if (istat == 0) {
        return 0;
    }
    if (coplanar == 1) {
        return 0;
    }
/*
 * If the intersection is extremely short, do not put it
 * into the segment list.  The tiny distance is passed
 * from the calling method.
 */
    dx = seg_xyz1[0] - seg_xyz2[0];
    dy = seg_xyz1[1] - seg_xyz2[1];
    dz = seg_xyz1[2] - seg_xyz2[2];
    dist = dx * dx + dy * dy + dz * dz;
    dist = sqrt (dist);
    if (dist < tiny) {
        return 0;
    }

    seg_xyz1[0] += pxmin;
    seg_xyz1[1] += pymin;
    seg_xyz1[2] += pzmin;
    seg_xyz2[0] += pxmin;
    seg_xyz2[1] += pymin;
    seg_xyz2[2] += pzmin;

/*
 * Add the segment to the WorkIntersectionSegments list
 */
    if (WorkIntersectionSegments == NULL  ||
        NumWorkIntersectionSegments >= MaxWorkIntersectionSegments) {
        MaxWorkIntersectionSegments += 100;
        WorkIntersectionSegments = (_INtersectionSegment_ *) csw_Realloc
          (WorkIntersectionSegments,
           MaxWorkIntersectionSegments * sizeof(_INtersectionSegment_));
    }

    if (WorkIntersectionSegments == NULL) {
        return -1;
    }

    sptr = WorkIntersectionSegments + NumWorkIntersectionSegments;
    sptr->x1 = seg_xyz1[0];
    sptr->y1 = seg_xyz1[1];
    sptr->z1 = seg_xyz1[2];
    sptr->x2 = seg_xyz2[0];
    sptr->y2 = seg_xyz2[1];
    sptr->z2 = seg_xyz2[2];
    sptr->used = 0;

    do_write = csw_GetDoWrite ();
    if (do_write) {
        printf ("\n");
        printf ("triangle 1 points\n");
        printf ("%f %f %f\n",
                t1_xyz1[0]+pxmin, t1_xyz1[1]+pymin, t1_xyz1[2]+pzmin);
        printf ("%f %f %f\n",
                t1_xyz2[0]+pxmin, t1_xyz2[1]+pymin, t1_xyz2[2]+pzmin);
        printf ("%f %f %f\n",
                t1_xyz3[0]+pxmin, t1_xyz3[1]+pymin, t1_xyz3[2]+pzmin);
        printf ("triangle 2 points\n");
        printf ("%f %f %f\n",
                t2_xyz1[0]+pxmin, t2_xyz1[1]+pymin, t2_xyz1[2]+pzmin);
        printf ("%f %f %f\n",
                t2_xyz2[0]+pxmin, t2_xyz2[1]+pymin, t2_xyz2[2]+pzmin);
        printf ("%f %f %f\n",
                t2_xyz3[0]+pxmin, t2_xyz3[1]+pymin, t2_xyz3[2]+pzmin);
        printf ("segment points:\n");
        printf ("%f %f %f\n",sptr->x1, sptr->y1, sptr->z1);
        printf ("%f %f %f\n",sptr->x2, sptr->y2, sptr->z2);
        printf ("\n");
    }

    NumWorkIntersectionSegments++;

    return 1;

}


/*------------------------------------------------------------------------*/

/*
 * Connect the current WorkIntersectionSegments list end to end to form
 * intersection lines.  The intersection lines are appended to the
 * WorkIntersectionLines list.  The intersectionlines are all identified
 * as being shared by tmeshid1 and tmeshid2.
 *
 * On success, 1 is returned.  On a memory allocation error, -1 is returned.
 *
 * This is a protected method.
 */
int SealedModel::ConnectIntersectionSegments (int tmeshid1, int tmeshid2)
{
    double      xfirst, yfirst, zfirst,
                xlast, ylast, zlast,
                xmin, ymin, zmin,
                xmax, ymax, zmax, tiny;
    int         istat, i, j, n, n1, n2, nwtot, ndone;
    int         *iwork1 = NULL, *idir1 = NULL,
                *iwork2 = NULL, *idir2 = NULL;
    double      *xline = NULL, *yline = NULL, *zline = NULL,
                xt, yt, zt;
    int         nmax, nline;

    _INtersectionSegment_   *sp1, *sp2;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        csw_Free (iwork1);

        if (bsuccess == false) {
            FreeWorkIntersectionLines ();
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (WorkIntersectionSegments == NULL) {
        return 1;
    }

/*
 * Allocate space for the work arrays.
 */
    nwtot = NumWorkIntersectionSegments + 1;
    if (nwtot < 100) nwtot = 100;

    iwork1 = (int *)csw_Malloc (4 * nwtot * sizeof(int));
    if (iwork1 == NULL) {
        return -1;
    }
    iwork2 = iwork1 + nwtot;
    idir1 = iwork2 + nwtot;
    idir2 = idir1 + nwtot;

/*
 * Calculate a grazing distance which defines "identically located" points.
 */
    if (modelGrazeDistance > 0.0) {
        tiny = modelGrazeDistance;
    }
    else {
        if (averageSpacing > 0.0) {
            tiny = averageSpacing / 100.0;
        }
        else {
            CalcWorkSegmentBounds (&xmin, &ymin, &zmin,
                                   &xmax, &ymax, &zmax);
            if (xmin >= xmax  &&  ymin >= ymax  &&  zmin >= zmax) {
                return 1;
            }
            tiny = (xmax - xmin) + (ymax - ymin) + (zmax - zmin);
            tiny /= 30000.0;
        }
    }

/*
 * Start with the first work segment.  Find any segments that attach to it
 * at either end point.  Repeat until no attaching segments are found.
 * As each segment is used it is marked as used.  Once a particular line
 * has been completely constructed, the next unused segment is the start for
 * a new line.
 */
    for (i=0; i<NumWorkIntersectionSegments; i++) {
        sp1 = WorkIntersectionSegments + i;
        if (sp1->used == 1) {
            continue;
        }

    /*
     * The segment has not been used in another line so it is a
     * seed segment for another line.  The initial first and last
     * points of the line being built are the first and last points
     * of this segment.
     */
        xfirst = sp1->x1;
        yfirst = sp1->y1;
        zfirst = sp1->z1;
        xlast = sp1->x2;
        ylast = sp1->y2;
        zlast = sp1->z2;
        n1 = 0;
        n2 = 0;

     /*
      * Loop looking for attached segments, building the line outward
      * from the seed segment until no attached segment is found.
      */
        for (;;) {

            ndone = 0;

        /*
         * Find unused segments that can be appended or prepended to
         * the current first and last points.  The indexes in the
         * WorkIntersectionSegments list are added to the iwork1 list
         * when prepended and to the iwork2 list when appended.  If
         * a prepended segment should go from point 1 to point 2 in
         * the final line, idir1 is set to 1.  If from point 2 to point1
         * idir1 is set to -1.  Idir2 records the direction for appended
         * segments.
         */
            for (j=i+1; j<NumWorkIntersectionSegments; j++) {

                sp2 = WorkIntersectionSegments + j;
                if (sp2->used == 1) {
                    continue;
                }

            /*
             * Check if either end point of this segment "coincides"
             * with the current first or last point.
             */
                istat = SamePointXYZ (xfirst, yfirst, zfirst,
                                   sp2->x1, sp2->y1, sp2->z1, tiny);
                if (istat == 1) {
                    sp2->used = 1;
                    xfirst = sp2->x2;
                    yfirst = sp2->y2;
                    zfirst = sp2->z2;
                    iwork1[n1] = j;
                    idir1[n1] = -1;
                    n1++;
                    ndone++;
                    continue;
                }

                istat = SamePointXYZ (xfirst, yfirst, zfirst,
                                   sp2->x2, sp2->y2, sp2->z2, tiny);
                if (istat == 1) {
                    sp2->used = 1;
                    xfirst = sp2->x1;
                    yfirst = sp2->y1;
                    zfirst = sp2->z1;
                    iwork1[n1] = j;
                    idir1[n1] = 1;
                    n1++;
                    ndone++;
                    continue;
                }

                istat = SamePointXYZ (xlast, ylast, zlast,
                                   sp2->x1, sp2->y1, sp2->z1, tiny);
                if (istat == 1) {
                    sp2->used = 1;
                    xlast = sp2->x2;
                    ylast = sp2->y2;
                    zlast = sp2->z2;
                    iwork2[n2] = j;
                    idir2[n2] = 1;
                    n2++;
                    ndone++;
                    continue;
                }

                istat = SamePointXYZ (xlast, ylast, zlast,
                                   sp2->x2, sp2->y2, sp2->z2, tiny);
                if (istat == 1) {
                    sp2->used = 1;
                    xlast = sp2->x1;
                    ylast = sp2->y1;
                    zlast = sp2->z1;
                    iwork2[n2] = j;
                    idir2[n2] = -1;
                    n2++;
                    ndone++;
                    continue;
                }

            }  // end of j loop

          /*
           * If no segments were appended or prepended,
           * break out of the infinite loop.
           */
            if (ndone == 0) {
                break;
            }

        } // end of the infinite loop

      /*
       * Build a line from the connected segment numbers.
       */
        nmax = n1 + n2 + 3;
        xline = (double *)csw_Malloc (nmax * 3 * sizeof(double));
        if (xline == NULL) {
            return -1;
        }
        yline = xline + nmax;
        zline = yline + nmax;

      /*
       * Start with the prepended segments.
       */
        nline = 0;
        for (j=n1-1; j>=0; j--) {
            n = iwork1[j];
            if (idir1[j] == -1) {
                xt = WorkIntersectionSegments[n].x2;
                yt = WorkIntersectionSegments[n].y2;
                zt = WorkIntersectionSegments[n].z2;
            }
            else {
                xt = WorkIntersectionSegments[n].x1;
                yt = WorkIntersectionSegments[n].y1;
                zt = WorkIntersectionSegments[n].z1;
            }
            xline[nline] = xt;
            yline[nline] = yt;
            zline[nline] = zt;
            nline++;
        }

     /*
      * Put in the original "seed" segment.
      */
        xline[nline] = WorkIntersectionSegments[i].x1;
        yline[nline] = WorkIntersectionSegments[i].y1;
        zline[nline] = WorkIntersectionSegments[i].z1;
        nline++;
        xline[nline] = WorkIntersectionSegments[i].x2;
        yline[nline] = WorkIntersectionSegments[i].y2;
        zline[nline] = WorkIntersectionSegments[i].z2;
        nline++;

     /*
      * Put in the appended segments.
      */
        for (j=0; j<n2; j++) {
            n = iwork2[j];
            if (idir2[j] == 1) {
                xt = WorkIntersectionSegments[n].x2;
                yt = WorkIntersectionSegments[n].y2;
                zt = WorkIntersectionSegments[n].z2;
            }
            else {
                xt = WorkIntersectionSegments[n].x1;
                yt = WorkIntersectionSegments[n].y1;
                zt = WorkIntersectionSegments[n].z1;
            }
            xline[nline] = xt;
            yline[nline] = yt;
            zline[nline] = zt;
            nline++;
        }

        istat =
          AddWorkIntersectionLine (xline, yline, zline, nline,
                                   tmeshid1, tmeshid2);

        if (istat == 999) {
            csw_Free (xline);
            xline = NULL;
            yline = NULL;
            zline = NULL;
        }

    }  // end of i loop

    istat =
      ConnectCloseWorkIntersectionLines ();
    if (istat != 1) {
        return -1;
    }

    bsuccess = true;

    return 1;

}



/*---------------------------------------------------------------------------*/

/*
 * Free the coordinate arrays in the WorkIntersectionLines list.  This does
 * not csw_Free the list itself.  Each structure in the list is set to an "empty"
 * state that can be reused.
 *
 * This is a protected method.
 */
void SealedModel::FreeWorkIntersectionLines (void)
{
    int                  i;
    _INtersectionLine_   *lptr;

    if (WorkIntersectionLines == NULL) {
        return;
    }

    for (i=0; i<NumWorkIntersectionLines; i++) {
        lptr = WorkIntersectionLines + i;
        csw_Free (lptr->x);
        lptr->x = NULL;
        lptr->y = NULL;
        lptr->z = NULL;
        lptr->npts = 0;
        lptr->surf1 = 0;
        lptr->surf2 = 0;
    }

    NumWorkIntersectionLines = 0;
}


/*---------------------------------------------------------------------------*/

/*
 * Return a bounding box slightly larger than the min/max of the
 * current WorkIntersectionSegments list.
 *
 * This is a protected method.
 */
void SealedModel::CalcWorkSegmentBounds (
    double  *sxmin,
    double  *symin,
    double  *szmin,
    double  *sxmax,
    double  *symax,
    double  *szmax)
{
    double  xmin, ymin, zmin, xmax, ymax, zmax;
    double  tiny;
    int     i;
    _INtersectionSegment_  *sptr;

    *sxmin = 1.e30;
    *symin = 1.e30;
    *szmin = 1.e30;
    *sxmax = -1.e30;
    *symax = -1.e30;
    *szmax = -1.e30;

    if (WorkIntersectionSegments == NULL) {
        return;
    }

    xmin = ymin = zmin = 1.e30;
    xmax = ymax = zmax = -1.e30;

    for (i=0; i<NumWorkIntersectionSegments; i++) {
        sptr = WorkIntersectionSegments + i;
        if (sptr->x1 < xmin) xmin = sptr->x1;
        if (sptr->y1 < ymin) ymin = sptr->y1;
        if (sptr->z1 < zmin) zmin = sptr->z1;
        if (sptr->x1 > xmax) xmax = sptr->x1;
        if (sptr->y1 > ymax) ymax = sptr->y1;
        if (sptr->z1 > zmax) zmax = sptr->z1;
        if (sptr->x2 < xmin) xmin = sptr->x2;
        if (sptr->y2 < ymin) ymin = sptr->y2;
        if (sptr->z2 < zmin) zmin = sptr->z2;
        if (sptr->x2 > xmax) xmax = sptr->x2;
        if (sptr->y2 > ymax) ymax = sptr->y2;
        if (sptr->z2 > zmax) zmax = sptr->z2;
    }

    if (xmin > xmax) {
        return;
    }

    tiny = (xmax - xmin) + (ymax - ymin) + (zmax - zmin);
    tiny /= 30000.0;

    xmin -= tiny;
    ymin -= tiny;
    zmin -= tiny;
    xmax += tiny;
    ymax += tiny;
    zmax += tiny;

    *sxmin = xmin;
    *symin = ymin;
    *szmin = zmin;
    *sxmax = xmax;
    *symax = ymax;
    *szmax = zmax;

    return;

}


/*----------------------------------------------------------------------------*/

/*
 * Return 1 if the two xyz points are less than or eual to tiny distance
 * from each other.  Return zero if the two points are greater than tiny
 * distance from each other.
 */
int SealedModel::SamePointXYZ (double x1, double y1, double z1,
                               double x2, double y2, double z2,
                               double tiny)
{
    double        dx, dy, dz, dist;

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;

    dist = dx * dx + dy * dy + dz * dz;

    if (dist <= tiny * tiny) {
        return 1;
    }

    return 0;
}

int SealedModel::SamePointXYZ (double x1, double y1, double z1,
                               double x2, double y2, double z2)
{
    double        dx, dy, dz, dist, tiny;

    tiny = modelTiny;

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;

    dist = dx * dx + dy * dy + dz * dz;

    if (dist <= tiny * tiny) {
        return 1;
    }

    return 0;
}

int SealedModel::SameValue (double val1, double val2,
                            double tiny)
{
    double      dz;

    dz = val1 - val2;
    if (dz < 0.0) dz = -dz;

    if (dz <= tiny) {
        return 1;
    }

    return 0;

}



/*----------------------------------------------------------------------------*/

/*
 * Update the modelXmin etc. members to the current limits of
 * the input horizons and faults.  Also include the input sediment
 * surface and input model bottom in the limits determination.
 */
void SealedModel::CalcModelBounds (void)
{
    double             xmin, ymin, zmin,
                       xmax, ymax, zmax;
    int                i, j;
    CSWTriMeshStruct   *tmesh;
    NOdeStruct         *nptr;

    if (InputHorizonList == NULL ||
        InputFaultList == NULL) {
        return;
    }

    xmin = 1.e30;
    ymin = 1.e30;
    zmin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;
    zmax = -1.e30;

    for (i=0; i<NumInputHorizonList; i++) {
        tmesh = InputHorizonList + i;
        for (j=0; j<tmesh->num_nodes; j++) {
            nptr = tmesh->nodes + j;
            if (nptr->x < xmin) xmin = nptr->x;
            if (nptr->y < ymin) ymin = nptr->y;
            if (nptr->z < zmin) zmin = nptr->z;
            if (nptr->x > xmax) xmax = nptr->x;
            if (nptr->y > ymax) ymax = nptr->y;
            if (nptr->z > zmax) zmax = nptr->z;
        }
    }

    for (i=0; i<NumInputFaultList; i++) {
        tmesh = InputFaultList + i;
        for (j=0; j<tmesh->num_nodes; j++) {
            nptr = tmesh->nodes + j;
            if (nptr->x < xmin) xmin = nptr->x;
            if (nptr->y < ymin) ymin = nptr->y;
            if (nptr->z < zmin) zmin = nptr->z;
            if (nptr->x > xmax) xmax = nptr->x;
            if (nptr->y > ymax) ymax = nptr->y;
            if (nptr->z > zmax) zmax = nptr->z;
        }
    }

    if (InputSedimentSurface) {
        tmesh = InputSedimentSurface;
        for (j=0; j<tmesh->num_nodes; j++) {
            nptr = tmesh->nodes + j;
            if (nptr->x < xmin) xmin = nptr->x;
            if (nptr->y < ymin) ymin = nptr->y;
            if (nptr->z < zmin) zmin = nptr->z;
            if (nptr->x > xmax) xmax = nptr->x;
            if (nptr->y > ymax) ymax = nptr->y;
            if (nptr->z > zmax) zmax = nptr->z;
        }
    }

    if (InputModelBottom) {
        tmesh = InputModelBottom;
        for (j=0; j<tmesh->num_nodes; j++) {
            nptr = tmesh->nodes + j;
            if (nptr->x < xmin) xmin = nptr->x;
            if (nptr->y < ymin) ymin = nptr->y;
            if (nptr->z < zmin) zmin = nptr->z;
            if (nptr->x > xmax) xmax = nptr->x;
            if (nptr->y > ymax) ymax = nptr->y;
            if (nptr->z > zmax) zmax = nptr->z;
        }
    }

    modelXmin = xmin;
    modelYmin = ymin;
    modelZmin = zmin;
    modelXmax = xmax;
    modelYmax = ymax;
    modelZmax = zmax;

    modelTiny = (modelXmax - modelXmin + modelYmax - modelYmin) / 200000.0;

    return;

}


/*--------------------------------------------------------------------*/

/*
 * Set the grazing distance to 1 part in 100000 of the average model dimension.
 */
void SealedModel::CalcModelGrazeDistance (void)
{
    if (modelXmin > modelXmax  ||
        modelYmin > modelYmax  ||
        modelZmin > modelZmax) {
        modelGrazeDistance = .0001;
        return;
    }

    double dt = (modelXmax - modelXmin +
                 modelYmax - modelYmin +
                 modelZmax - modelZmin)  / 300000.0;
    modelGrazeDistance = dt;
}


/*----------------------------------------------------------------------------*/

/*
 * Create a triangle index and add all the input horizon and fault surfaces to it.
 * On success, 1 is returned.  If the model bounds cannot be calculated, zero
 * is returned.  On a memory allocation failure, -1 is returned.
 */
int SealedModel::CreateInput3DTriangleIndex (void)
{
    CSWTriMeshStruct      *tmesh;
    int                   i, istat, tmeshid;

    if (InputHorizonList == NULL) {
        return 0;
    }

    CalcModelBounds ();

    if (modelXmin > modelXmax) {
        return 0;
    }

    if (InputTriangle3DIndex != NULL) {
        delete InputTriangle3DIndex;
        InputTriangle3DIndex = NULL;
    }

    try {
        SNF;
        InputTriangle3DIndex = new Spatial3DTriangleIndex (
                modelXmin, modelYmin, modelZmin,
                modelXmax, modelYmax, modelZmax);
    }
    catch (...) {
        printf ("\n***** Exception from new *****\n\n");
        InputTriangle3DIndex = NULL;
        return 0;
    }

    if (averageSpacing > 0.0) {
        InputTriangle3DIndex->SetGeometry (
        modelXmin, modelYmin, modelZmin,
        modelXmax, modelYmax, modelZmax,
        averageSpacing, averageSpacing, averageSpacing);
    }

  /*
   * Put the horizons into the index.
   */
    for (i=0; i<NumInputHorizonList; i++) {
        tmesh = InputHorizonList + i;
        istat =
        InputTriangle3DIndex->AddTriMesh (
            i,
            tmesh->nodes,
            tmesh->edges,
            tmesh->tris,
            tmesh->num_tris);
        if (istat == -1) {
            return -1;
        }
    }

  /*
   * Put the faults into the index.
   */
    if (IndexPaddedFaults == 0) {
        if (InputFaultList == NULL) {
            return 0;
        }
        for (i=0; i<NumInputFaultList; i++) {
            tmesh = InputFaultList + i;
            tmeshid = _FAULT_ID_BASE_ + i;
            istat =
            InputTriangle3DIndex->AddTriMesh (
                tmeshid,
                tmesh->nodes,
                tmesh->edges,
                tmesh->tris,
                tmesh->num_tris);
            if (istat == -1) {
                return -1;
            }
        }
    }

    else {
        if (PaddedFaultList == NULL) {
            return 0;
        }
        for (i=0; i<NumPaddedFaultList; i++) {
            tmesh = PaddedFaultList + i;
            tmeshid = _FAULT_ID_BASE_ + i;
            istat =
            InputTriangle3DIndex->AddTriMesh (
                tmeshid,
                tmesh->nodes,
                tmesh->edges,
                tmesh->tris,
                tmesh->num_tris);
            if (istat == -1) {
                return -1;
            }
        }
    }

    return 1;

}



/*-----------------------------------------------------------------------------*/

/*
 * Free the memory associated with a CSWTriMeshStruct and set the struct
 * to an "empty" state.
 */
void SealedModel::FreeTmesh (CSWTriMeshStruct *tmeshlist, int nt)
{
    CSWTriMeshStruct    *tmesh;
    int                 i;

    if (tmeshlist == NULL) {
        return;
    }

    for (i=0; i<nt; i++) {
        tmesh = tmeshlist + i;

        csw_Free (tmesh->nodes);
        csw_Free (tmesh->edges);
        csw_Free (tmesh->tris);
        csw_Free (tmesh->grid);
        csw_Free (tmesh->xdetach);

        tmesh->nodes = NULL;
        tmesh->edges = NULL;
        tmesh->tris = NULL;
        tmesh->grid = NULL;

        tmesh->num_nodes = 0;
        tmesh->num_edges = 0;
        tmesh->num_tris = 0;

        tmesh->xdetach = NULL;
        tmesh->ydetach = NULL;
        tmesh->zdetach = NULL;
        tmesh->ndetach = 0;

    }

    csw_Free (tmeshlist);

    return;

}


void SealedModel::FreeOutlineList (_OUtline_ **list, int nlist)
{
    int          i;
    _OUtline_    *optr;

    if (list == NULL) {
        return;
    }

    for (i=0; i<nlist; i++) {
        optr = list[i];
        if (optr != NULL) {
            csw_Free (optr->x);
            csw_Free (optr->itags);
            csw_Free (optr);
        }
    }

    csw_Free (list);

    return;

}



/*-----------------------------------------------------------------------------*/

/*
 * Free the padded horizon and padded fault trimesh lists.
 */
void SealedModel::FreePaddedLists (void)
{

    if (PaddedHorizonList != NULL) {
        FreeTmesh (PaddedHorizonList, NumPaddedHorizonList);
    }
    PaddedHorizonList = NULL;
    NumPaddedHorizonList = 0;
    MaxPaddedHorizonList = 0;

    if (PaddedFaultList != NULL) {
        FreeTmesh (PaddedFaultList, NumPaddedFaultList);
    }
    PaddedFaultList = NULL;
    NumPaddedFaultList = 0;
    MaxPaddedFaultList = 0;

    return;

}



/*-----------------------------------------------------------------------------*/

/*
 * Free the sealed horizon and sealed fault trimesh lists.
 */
void SealedModel::FreeSealedLists (void)
{

    if (SealedHorizonList != NULL) {
        FreeTmesh (SealedHorizonList, NumSealedHorizonList);
    }
    SealedHorizonList = NULL;
    NumSealedHorizonList = 0;
    MaxSealedHorizonList = 0;

    if (SealedFaultList != NULL) {
        FreeTmesh (SealedFaultList, NumSealedFaultList);
    }
    SealedFaultList = NULL;
    NumSealedFaultList = 0;
    MaxSealedFaultList = 0;

    return;

}


/*-----------------------------------------------------------------------------*/

/*
 * Add a line to the WorkIntersectionLines list.  On success, 1 is
 * returned.  On a memory allocation error, -1 is returned.  If the
 * line is very short (less than averageSpacing / 10.0) then do not add it.
 *
 * This is a protected method.
 */
int SealedModel::AddWorkIntersectionLine (
                      double *xline, double *yline, double *zline, int nline,
                      int tmeshid1, int tmeshid2)
{
    _INtersectionLine_   *lptr;
    double               dx, dy, dz, dist, dtot, tiny;
    int                  i;

    tiny = averageSpacing / 10.0;
    dtot = 0.0;
    for (i=0; i<nline-1; i++) {
        dx = xline[i] - xline[i+1];
        dy = yline[i] - yline[i+1];
        dz = zline[i] - zline[i+1];
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);
        dtot += dist;
        if (dtot > tiny) break;
    }

    if (dtot <= tiny) {
        return 999;
    }

    if (WorkIntersectionLines == NULL  ||
        NumWorkIntersectionLines >= MaxWorkIntersectionLines) {
        MaxWorkIntersectionLines += 10;
        WorkIntersectionLines = (_INtersectionLine_ *)csw_Realloc
          (WorkIntersectionLines,
           MaxWorkIntersectionLines * sizeof(_INtersectionLine_));
    }

    if (WorkIntersectionLines == NULL) {
        return -1;
    }

    lptr = WorkIntersectionLines + NumWorkIntersectionLines;
    memset (lptr, 0, sizeof(_INtersectionLine_));
    lptr->x = xline;
    lptr->y = yline;
    lptr->z = zline;
    lptr->npts = nline;
    lptr->surf1 = tmeshid1;
    lptr->surf2 = tmeshid2;
    lptr->embed_flag = 0;
    lptr->splice_partner_1 = -1;
    lptr->splice_partner_2 = -1;
    lptr->external_id1 = FindFaultExternalID (tmeshid1);
    lptr->external_id2 = FindFaultExternalID (tmeshid2);

    NumWorkIntersectionLines++;

    return 1;

}


int SealedModel::FindFaultExternalID (int id)
{
    int        i, idout;

    if (id == _DETACHMENT_TMESH_) {
        return id;
    }

    if (id >= _FAULT_ID_BASE_  &&  id < _BOUNDARY_ID_BASE_) {
        if (FaultList) {
            i = id - _FAULT_ID_BASE_;
            if (i < 0  ||  i >= NumFaultList) {
                return -1;
            }
            idout = FaultList[i].external_id;
            return idout;
        }
        return -1;
    }
    else {
        return -1;
    }

}


/*------------------------------------------------------------------------------------*/

/*
 * Append the current WorkIntersectionLines list to the IntersectionLines list.
 *
 * This is a protected method.
 */
int SealedModel::AddWorkLinesToResults (void)
{
    int          size1, size2, size3;
    int          ncp[1000], ncv[1000];

    if (WorkIntersectionLines == NULL  ||
        NumWorkIntersectionLines < 1) {
        return 1;
    }

    int do_write;
    do_write = csw_GetDoWrite ();
    if (do_write) {

        double   *x = NULL, *y = NULL, *z = NULL;
        int      i, j, ntot, n;
        char     fname[100];

        auto lscope = [&]()
        {
            csw_Free (x);
        };
        CSWScopeGuard loc_scope_guard (lscope);

        ntot = 0;
        for (i=0; i<NumWorkIntersectionLines; i++) {
            ntot += WorkIntersectionLines[i].npts;
        }
        x = (double *)csw_Malloc (3 * ntot * sizeof(double));
        if (x == NULL) {
            return -1;
        }
        y = x + ntot;
        z = y + ntot;
        n = 0;
        for (i=0; i<NumWorkIntersectionLines; i++) {
            ncp[i] = 1;
            ncv[i] = WorkIntersectionLines[i].npts;
            for (j=0; j<WorkIntersectionLines[i].npts; j++) {
                x[n] = WorkIntersectionLines[i].x[j];
                y[n] = WorkIntersectionLines[i].y[j];
                z[n] = WorkIntersectionLines[i].z[j];
                n++;
            }
        }
        sprintf (fname, "worklines.xyz");
        grd_api_obj.grd_WriteLines (
            x, y, z,
            NumWorkIntersectionLines, ncp, ncv,
            fname);
    }

    size1 = NumWorkIntersectionLines * sizeof (_INtersectionLine_);
    size2 = NumIntersectionLines * sizeof (_INtersectionLine_);
    size3 = size1 + size2;

    IntersectionLines = (_INtersectionLine_ *)csw_Realloc (
        IntersectionLines, size3);
    if (IntersectionLines == NULL) {
        return -1;
    }

    memcpy (IntersectionLines + NumIntersectionLines,
            WorkIntersectionLines,
            size1);
    NumIntersectionLines += NumWorkIntersectionLines;

    int istat;
    istat = NumWorkIntersectionLines;

    NumWorkIntersectionLines = 0;

    return istat;

}





/*-----------------------------------------------------------------------------------*/

void SealedModel::setAverageSpacing (double val)
{
    averageSpacing = val;
}

/*-----------------------------------------------------------------------------------*/

double SealedModel::getAverageSpacing (void)
{
    return averageSpacing;
}


/*-------------------------------------------------------------------------------------*/

int SealedModel::addInputHorizon (
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    int               istat;

    istat = addInputHorizon (
        0, -1.0,
        nodes_in, num_nodes,
        edges_in, num_edges,
        triangles_in, num_triangles);
    return istat;
}

int SealedModel::addInputHorizon (
    double            age,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    int               istat;

    istat = addInputHorizon (
        0, age,
        nodes_in, num_nodes,
        edges_in, num_edges,
        triangles_in, num_triangles);
    return istat;
}


/**
 * Add a horizon to the list of horizons to be sealed.  The horizon is specified as a
 * "CSW trimesh", i.e. as lists of nodes, edges, and triangles.  This method creates
 * copies of the specified node, edge and triangle arrays.  So, the calling function
 * should csw_Free these when it no longer directly needs them.  On success, 1 is returned.
 * On a memory allocation failure, -1 is returned.
 */
int SealedModel::addInputHorizon (
    int               id,
    double            age,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;

    CSWTriMeshStruct   *tmesh = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (InputHorizonList == NULL  ||
        NumInputHorizonList >= MaxInputHorizonList) {
        MaxInputHorizonList += 10;
        InputHorizonList = (CSWTriMeshStruct *)csw_Realloc
            (InputHorizonList,
             MaxInputHorizonList * sizeof(CSWTriMeshStruct));
    }

    if (InputHorizonList == NULL) {
        return -1;
    }

    tmesh = InputHorizonList + NumInputHorizonList;
    memset (tmesh, 0, sizeof(CSWTriMeshStruct));

    nodes = (NOdeStruct *)csw_Calloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Calloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Calloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;
    tmesh->external_id = id;
    tmesh->age = age;
    tmesh->minage = -1.0;
    tmesh->maxage = -1.0;
    tmesh->zmin = 1.0;
    tmesh->zmax = 0.0;

    NumInputHorizonList++;

    return 1;

}





/*-------------------------------------------------------------------------------------*/

/*
 * Add an input horizon defined by a grid and a polygon.  These are
 * converted to a trimesh and added as a trimesh to the model.
 */
int SealedModel::addInputGridHorizon (
    int       id,
    double    age,
    CSW_F     *grid,
    int       ncol,
    int       nrow,
    double    gxmin,
    double    gymin,
    double    gxmax,
    double    gymax,
    double    *xpoly,
    double    *ypoly,
    int       npoly,
    double    *xfault_in,
    double    *yfault_in,
    double    *zfault_in,
    int       *ifcomp_in,
    int       *ifault,
    int       nfault)
{
    double          *zpoly = NULL;
    int             istat;
    NOdeStruct      *nodes = NULL;
    EDgeStruct      *edges = NULL;
    TRiangleStruct  *tris = NULL;
    int             numnodes,
                    numedges,
                    numtris;
    int             lineflags;
    double          *xfault = NULL, *yfault = NULL, *zfault = NULL;

    double    *xresamp = NULL, *yresamp = NULL, *zresamp = NULL, avspace;
    double    *xa = NULL, *ya = NULL, *za = NULL;
    int       na, offset;
    int       i, nresamp, maxresamp, ntot, nmax;
    int       *ifcomp = NULL;

//    bool     bsuccess = false;


    auto fscope = [&]()
    {
        csw_Free (zpoly);
        csw_Free (ifcomp);
        csw_Free (xresamp);
        csw_Free (xfault);
        csw_Free (yfault);
        csw_Free (zfault);
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (tris);
    };
    CSWScopeGuard func_scope_guard (fscope);


    zpoly = (double *)csw_Malloc (npoly * sizeof(double));
    if (zpoly == NULL) {
        return -1;
    }

    istat =
      grd_api_obj.grd_BackInterpolateFromDouble (
        grid, ncol, nrow,
        gxmin, gymin, gxmax, gymax,
        NULL, 0,
        xpoly, ypoly, zpoly, npoly,
        GRD_BICUBIC);
    if (istat == -1) {
        return -1;
    }

/*
 * Resample the minor fault lines at the grid spacing.
 */
    ifcomp = (int *)csw_Malloc (nfault * sizeof(int));
    if (ifcomp == NULL) {
        return -1;
    }

    avspace = (gxmax - gxmin) / (ncol - 1);
    avspace += (gymax - gymin) / (nrow - 1);
    avspace /= 2.0;

    xfault = NULL;
    yfault = NULL;
    zfault = NULL;
    offset = 0;
    ntot = 0;
    nmax = 0;
    for (i=0; i<nfault; i++) {
        xa = xfault_in + offset;
        ya = yfault_in + offset;
        za = zfault_in + offset;
        na = ifcomp_in[i];
        offset += na;
        maxresamp = na * 10;
        if (maxresamp < 1000) maxresamp = 1000;
        xresamp = (double *)csw_Malloc (3 * maxresamp * sizeof(double));
        if (xresamp == NULL) {
            return -1;
        }
        yresamp = xresamp + maxresamp;
        zresamp = yresamp + maxresamp;
        istat =
          grd_api_obj.grd_ResampleXYZLine (
            xa, ya, za, na,
            avspace,
            xresamp, yresamp, zresamp, &nresamp,
            maxresamp);
        if (istat == -1) {
            return -1;
        }
        if (ntot + nresamp >= nmax) {
            nmax += nresamp + 1000;
            xfault = (double *)csw_Realloc (xfault, nmax * sizeof(double));
            yfault = (double *)csw_Realloc (yfault, nmax * sizeof(double));
            zfault = (double *)csw_Realloc (zfault, nmax * sizeof(double));
            if (xfault == NULL  ||  yfault == NULL  ||  zfault == NULL) {
                return -1;
            }
        }
        if (xfault  &&  yfault  &&  zfault  && nresamp > 0) {
            memcpy (xfault + ntot, xresamp, nresamp * sizeof(double));
            memcpy (yfault + ntot, yresamp, nresamp * sizeof(double));
            memcpy (zfault + ntot, zresamp, nresamp * sizeof(double));
        }
        ntot += nresamp;
        ifcomp[i] = nresamp;
    }

/*
 * Calculate a trimesh with no embedded minor faults.  However, the fault
 * lines are needed to calculate the elevations at the equilateral points.
 */
    istat =
      grd_api_obj.grd_CalcTriMeshFromGrid (
        grid, ncol, nrow,
        gxmin, gymin, gxmax, gymax,
        xfault, yfault, zfault,
        ifcomp, ifault, nfault,
        GRD_EQUILATERAL,
        &nodes, &edges, &tris,
        &numnodes, &numedges, &numtris);
    if (istat == -1) {
        return -1;
    }

    int do_write = csw_GetDoWrite ();
    if (do_write) {
      char    fname[200];
      int     nc[500];
      sprintf (fname, "minorfault.xyz");
      for (i=0; i<nfault; i++) {
        nc[i] = 1;
      }
      grd_api_obj.grd_WriteLines (
        xfault, yfault, zfault,
        nfault, nc, ifcomp,
        fname);
      sprintf (fname, "preminorfault.tri");
      grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tris, numtris,
        edges, numedges,
        nodes, numnodes,
        fname);
    }

/*
 * Add the minor faults as exact constraints.
 */
    istat =
      grd_api_obj.grd_AddLinesToTriMesh (
        xfault, yfault, zfault,
        ifcomp, ifault, nfault,
        1,
        &nodes, &edges, &tris,
        &numnodes, &numedges, &numtris);
    if (istat == -1) {
        return -1;
    }

    if (do_write) {
      char    fname[200];
      int     nwork = 1;
      sprintf (fname, "outline_poly.xyz");
      grd_api_obj.grd_WriteLines (
        xpoly, ypoly, zpoly,
        1, &nwork, &npoly,
        fname);
      sprintf (fname, "nopolyfault.tri");
      grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tris, numtris,
        edges, numedges,
        nodes, numnodes,
        fname);
    }

/*
 * Add the polygon as an exact constraint.
 */
    lineflags = 0;
    istat =
      grd_api_obj.grd_AddLinesToTriMesh (
        xpoly, ypoly, zpoly,
        &npoly, &lineflags, 1,
        1,
        &nodes, &edges, &tris,
        &numnodes, &numedges, &numtris);
    if (istat == -1) {
        return -1;
    }

    if (do_write) {
      char    fname[200];
      sprintf (fname, "noclippolyfault.tri");
      grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tris, numtris,
        edges, numedges,
        nodes, numnodes,
        fname);
    }

    istat =
      grd_api_obj.grd_ClipTriMeshToPolygon (
        &nodes, &numnodes,
        &edges, &numedges,
        &tris, &numtris,
        xpoly, ypoly, &npoly, 1, 1);
    if (istat == -1) {
        return -1;
    }

    if (do_write) {
      char    fname[200];
      sprintf (fname, "clippolyfault.tri");
      grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tris, numtris,
        edges, numedges,
        nodes, numnodes,
        fname);
    }

    istat =
      addInputHorizon (
        id,
        age,
        nodes,
        numnodes,
        edges,
        numedges,
        tris,
        numtris);

//    bsuccess = true;

    return istat;

}





/*-------------------------------------------------------------------------------------*/

int SealedModel::addInputFault (
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    int               istat;

    istat = addInputFault (
        0,
        nodes_in, num_nodes,
        edges_in, num_edges,
        triangles_in, num_triangles);
    return istat;
}

/*
 * Add a fault to the list of faults to be sealed.  The fault is specified as a
 * "CSW trimesh", i.e. as lists of nodes, edges, and triangles.  This method creates
 * copies of the specified node, edge and triangle arrays.  So, the calling function
 * should csw_Free these when it no longer directly needs them.  On success, 1 is returned.
 * On a memory allocation failure, -1 is returned.
 *
 */
int SealedModel::addInputFault (
    int               id,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;

    CSWTriMeshStruct   *tmesh = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (InputFaultList == NULL  ||
        NumInputFaultList >= MaxInputFaultList) {
        MaxInputFaultList += 10;
        InputFaultList = (CSWTriMeshStruct *)csw_Realloc
            (InputFaultList,
             MaxInputFaultList * sizeof(CSWTriMeshStruct));
    }

    if (InputFaultList == NULL) {
        return -1;
    }

    tmesh = InputFaultList + NumInputFaultList;
    memset (tmesh, 0, sizeof(CSWTriMeshStruct));

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;
    tmesh->external_id = id;
    tmesh->age = -1.0;
    tmesh->minage = -1.0;
    tmesh->maxage = -1.0;
    tmesh->zmin = 1.0;
    tmesh->zmax = 0.0;

    NumInputFaultList++;

    bsuccess = true;

    return 1;

}





/*-------------------------------------------------------------------------------------*/

/*
 * Add a padded horizon to the list of padded horizons.  This is a protected
 * method only called from padModel.
 */
int SealedModel::AddPaddedHorizon (
    int               id,
    double            age,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles,
    double            xcenter,
    double            ycenter)
{
    CSWTriMeshStruct  *tmesh;

    if (PaddedHorizonList == NULL  ||
        NumPaddedHorizonList >= MaxPaddedHorizonList) {
        MaxPaddedHorizonList += 10;
        PaddedHorizonList = (CSWTriMeshStruct *)csw_Realloc
            (PaddedHorizonList,
             MaxPaddedHorizonList * sizeof(CSWTriMeshStruct));
    }

    if (PaddedHorizonList == NULL) {
        return -1;
    }

    tmesh = PaddedHorizonList + NumPaddedHorizonList;
    memset (tmesh, 0, sizeof(CSWTriMeshStruct));

    tmesh->nodes = nodes_in;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges_in;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles_in;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 1;
    tmesh->is_sealed = 0;
    tmesh->id = _HORIZON_ID_BASE_ + NumPaddedHorizonList;
    tmesh->vflag = 0;
    tmesh->xcenter = xcenter;
    tmesh->ycenter = ycenter;
    tmesh->external_id = id;
    tmesh->age = age;

    int do_write;
    do_write = csw_GetDoWrite ();;
    if (do_write) {
        char fname1[200];
        sprintf (fname1, "pad_hor_%d.tri", NumPaddedHorizonList);
        grd_api_obj.grd_WriteTextTriMeshFile (
            0, NULL,
            triangles_in, num_triangles,
            edges_in, num_edges,
            nodes_in, num_nodes,
            fname1);
    }

    NumPaddedHorizonList++;

    return 1;

}




/*--------------------------------------------------------------------------*/

/*
 * Add a sealed horizon to the list of sealed horizons.  This is a protected
 * method only called from OutlineAndCropHorizons.
 */
int SealedModel::AddSealedHorizon (CSWTriMeshStruct *tmesh_in)
{
    CSWTriMeshStruct   *tmesh;

    if (SealedHorizonList == NULL  ||
        NumSealedHorizonList >= MaxSealedHorizonList) {
        MaxSealedHorizonList += 10;
        SealedHorizonList = (CSWTriMeshStruct *)csw_Realloc
            (SealedHorizonList,
             MaxSealedHorizonList * sizeof(CSWTriMeshStruct));
    }

    if (SealedHorizonList == NULL) {
        return -1;
    }

    tmesh = SealedHorizonList + NumSealedHorizonList;
    memset (tmesh, 0, sizeof(CSWTriMeshStruct));

    tmesh->nodes = tmesh_in->nodes;
    tmesh->num_nodes = tmesh_in->num_nodes;
    tmesh->edges = tmesh_in->edges;
    tmesh->num_edges = tmesh_in->num_edges;
    tmesh->tris = tmesh_in->tris;
    tmesh->num_tris = tmesh_in->num_tris;
    tmesh->is_padded = 1;
    tmesh->is_sealed = 1;
    tmesh->id = tmesh_in->id;
    tmesh->vflag = tmesh_in->vflag;
    memcpy (tmesh->vbase, tmesh_in->vbase, 6 * sizeof(double));
    tmesh->xcenter = tmesh_in->xcenter;
    tmesh->ycenter = tmesh_in->ycenter;
    tmesh->external_id = tmesh_in->external_id;
    tmesh->age = tmesh_in->age;

    NumSealedHorizonList++;

    return 1;

}




/*--------------------------------------------------------------------------*/

/*
 * Add a sealed fault to the list of sealed faults.  This is a protected
 * method.
 */
int SealedModel::AddSealedFault (CSWTriMeshStruct *tmesh_in)
{
    CSWTriMeshStruct   *tmesh;

    if (SealedFaultList == NULL  ||
        NumSealedFaultList >= MaxSealedFaultList) {
        MaxSealedFaultList += 10;
        SealedFaultList = (CSWTriMeshStruct *)csw_Realloc
            (SealedFaultList,
             MaxSealedFaultList * sizeof(CSWTriMeshStruct));
    }

    if (SealedFaultList == NULL) {
        return -1;
    }

    tmesh = SealedFaultList + NumSealedFaultList;
    memset (tmesh, 0, sizeof(CSWTriMeshStruct));

    tmesh->nodes = tmesh_in->nodes;
    tmesh->num_nodes = tmesh_in->num_nodes;
    tmesh->edges = tmesh_in->edges;
    tmesh->num_edges = tmesh_in->num_edges;
    tmesh->tris = tmesh_in->tris;
    tmesh->num_tris = tmesh_in->num_tris;
    tmesh->is_padded = 1;
    tmesh->is_sealed = 1;
    tmesh->id = tmesh_in->id;
    tmesh->vflag = tmesh_in->vflag;
    memcpy (tmesh->vbase, tmesh_in->vbase, 6 * sizeof(double));
    tmesh->xcenter = tmesh_in->xcenter;
    tmesh->ycenter = tmesh_in->ycenter;
    tmesh->external_id = tmesh_in->external_id;
    tmesh->age = tmesh_in->age;

    if (tmesh->id >= _BOUNDARY_ID_BASE_) {
        tmesh->external_id = tmesh->id;
    }

    NumSealedFaultList++;

    return 1;

}





/*-------------------------------------------------------------------------------------*/

/*
 * Public method to set the detachment surface, which is used by the
 * sealFaultsToDetachment method.
 */
int SealedModel::setPaddedDetachment (
    NOdeStruct       *nodes_in,
    int              num_nodes,
    EDgeStruct       *edges_in,
    int              num_edges,
    TRiangleStruct   *tris_in,
    int              num_tris)
{
    CSWTriMeshStruct *tmesh = NULL;

    NOdeStruct       *nodes = NULL;
    EDgeStruct       *edges = NULL;
    TRiangleStruct   *tris = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (tris);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (nodes_in == NULL  ||  edges_in == NULL  ||  tris_in == NULL) {
        return -1;
    }
    if (num_tris < 1  ||  num_edges < 3  ||  num_nodes < 3) {
        return -1;
    }

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    tris = (TRiangleStruct *)csw_Malloc (num_tris * sizeof(TRiangleStruct));
    if (nodes == NULL  ||  edges == NULL  ||  tris == NULL) {
        return -1;
    }

    if (PaddedDetachment != NULL) {
        FreeTmesh (PaddedDetachment, 1);
        PaddedDetachment = NULL;
    }
    PaddedDetachment = (CSWTriMeshStruct *)csw_Calloc (sizeof(CSWTriMeshStruct));
    if (PaddedDetachment == NULL) {
        return -1;
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (tris, tris_in, num_tris * sizeof(TRiangleStruct));

    tmesh = PaddedDetachment;

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = tris;
    tmesh->num_tris = num_tris;
    tmesh->is_padded = 1;
    tmesh->is_sealed = 0;
    tmesh->xcenter = 1.e30;
    tmesh->ycenter = 1.e30;

    bsuccess = true;

    return 1;

}




/*-------------------------------------------------------------------------------------*/

/*
 * Public method to add a padded fault.  This is used by the FaultConnect class
 * to put it's downward padded fault into the surface intersection calculations.
 */
int SealedModel::addPaddedFault (
    int              id,
    int              vflag,
    double           *vbase,
    NOdeStruct       *nodes,
    int              num_nodes,
    EDgeStruct       *edges,
    int              num_edges,
    TRiangleStruct   *tris,
    int              num_tris)
{
    int              istat;

    NOdeStruct       *n2 = NULL;
    EDgeStruct       *e2 = NULL;
    TRiangleStruct   *t2 = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (n2);
            csw_Free (e2);
            csw_Free (t2);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    n2 = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    e2 = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    t2 = (TRiangleStruct *)csw_Malloc (num_tris * sizeof(TRiangleStruct));
    if (n2 == NULL  ||  e2 == NULL  ||  t2 == NULL) {
        return -1;
    }

    memcpy (n2, nodes, num_nodes * sizeof(NOdeStruct));
    memcpy (e2, edges, num_edges * sizeof(EDgeStruct));
    memcpy (t2, tris, num_tris * sizeof(TRiangleStruct));

    istat = AddPaddedFault (
        id,
        n2, num_nodes,
        e2, num_edges,
        t2, num_tris,
        vflag, vbase,
        -1.0, -1.0,
        NULL, NULL, NULL, 0, 0);
    if (istat == -1) {
        return -1;
    }

    bsuccess = true;

    return 1;
}


/*-------------------------------------------------------------------------------------*/

/*
 * Add a padded fault to the list of padded faults.  This is a
 * private method.
 */
int SealedModel::AddPaddedFault (
    int               id,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles,
    int               vflag,
    double            *vbase,
    double            minage,
    double            maxage,
    double            *xline,
    double            *yline,
    double            *zline,
    int               nline,
    int               detach_id
)
{
    CSWTriMeshStruct   *tmesh = NULL;
    double             *xdetach = NULL, *ydetach = NULL, *zdetach = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xdetach);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (PaddedFaultList == NULL  ||
        NumPaddedFaultList >= MaxPaddedFaultList) {
        MaxPaddedFaultList += 10;
        PaddedFaultList = (CSWTriMeshStruct *)csw_Realloc
            (PaddedFaultList,
             MaxPaddedFaultList * sizeof(CSWTriMeshStruct));
    }

    if (PaddedFaultList == NULL) {
        return -1;
    }

    tmesh = PaddedFaultList + NumPaddedFaultList;
    memset (tmesh, 0, sizeof(CSWTriMeshStruct)); /*lint !e669*/

    tmesh->nodes = nodes_in;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges_in;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles_in;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 1;
    tmesh->is_sealed = 0;
    tmesh->id = _FAULT_ID_BASE_ + NumPaddedFaultList;
    tmesh->vflag = vflag;
    memcpy (tmesh->vbase, vbase, 6 * sizeof(double));
    tmesh->xcenter = 1.e30;
    tmesh->ycenter = 1.e30;
    tmesh->external_id = id;
    tmesh->age = -1.0;
    tmesh->minage = minage;
    tmesh->maxage = maxage;
    tmesh->detach_id = detach_id;
    tmesh->zmin = 1.0;
    tmesh->zmax = 0.0;

    if (xline &&  yline  &&  zline  &&  nline > 1) {
        xdetach = (double *)csw_Malloc (nline * 3 * sizeof(double));
        if (xdetach == NULL) {
            return -1;
        }
        ydetach = xdetach + nline;
        zdetach = ydetach + nline;
        memcpy (xdetach, xline, nline * sizeof(double));
        memcpy (ydetach, yline, nline * sizeof(double));
        memcpy (zdetach, zline, nline * sizeof(double));
        tmesh->xdetach = xdetach;
        tmesh->ydetach = ydetach;
        tmesh->zdetach = zdetach;
        tmesh->ndetach = nline;
    }

    int do_write;
    do_write = csw_GetDoWrite ();;
    if (do_write) {
        char fname1[200];
        sprintf (fname1, "pad_flt_%d.tri", NumPaddedFaultList);
        grd_api_obj.grd_WriteTextTriMeshFile (
            0, NULL,
            triangles_in, num_triangles,
            edges_in, num_edges,
            nodes_in, num_nodes,
            fname1);
    }

    NumPaddedFaultList++;

    bsuccess = true;

    return 1;

}

/*
 * If any segments in the work intersection list are identical,
 * which can happen if triangles intersect on a mutual edge of the
 * two triangles, only keep the first segment encountered.  Set the
 * used member of the intersection to 1 for all but the first
 * overlapping segment.
 */
int SealedModel::FindOverlapWorkSegments (void)
{
    _INtersectionSegment_   *sp1, *sp2;

    double        x1, y1, z1,
                  x2, y2, z2,
                  x3, y3, z3,
                  x4, y4, z4;
    double        xmin, ymin, zmin,
                  xmax, ymax, zmax,
                  tiny;
    int           same1, same2, i, j;

    if (WorkIntersectionSegments == NULL) {
        return 1;
    }

/*
 * Calculate a grazing distance which defines "identically located" points.
 */
    if (modelGrazeDistance > 0.0) {
        tiny = modelGrazeDistance / 10.0;
    }
    else {
        if (averageSpacing > 0.0) {
            tiny = averageSpacing / 1000.0;
        }
        else {
            CalcWorkSegmentBounds (&xmin, &ymin, &zmin,
                                   &xmax, &ymax, &zmax);
            if (xmin >= xmax  &&  ymin >= ymax  &&  zmin >= zmax) {
                return 1;
            }
            tiny = (xmax - xmin) + (ymax - ymin) + (zmax - zmin);
            tiny /= 3000000.0;
        }
    }

/*
 * Check all pairs of work segments for identical segments.
 */
    for (i=0; i<NumWorkIntersectionSegments; i++) {

        sp1 = WorkIntersectionSegments + i;
        if (sp1->used == 1) {
            continue;
        }
        x1 = sp1->x1;
        y1 = sp1->y1;
        z1 = sp1->z1;
        x2 = sp1->x2;
        y2 = sp1->y2;
        z2 = sp1->z2;

        for (j=i+1; j<NumWorkIntersectionSegments; j++) {

            sp2 = WorkIntersectionSegments + j;
            if (sp2->used == 1) {
                continue;
            }
            x3 = sp2->x1;
            y3 = sp2->y1;
            z3 = sp2->z1;
            x4 = sp2->x2;
            y4 = sp2->y2;
            z4 = sp2->z2;

            same1 = SamePointXYZ (x1, y1, z1,
                               x3, y3, z3,
                               tiny);
            same2 = SamePointXYZ (x2, y2, z2,
                               x4, y4, z4,
                               tiny);
            if (same1 == 1  &&  same2 == 1) {
                sp2->used = 1;
                continue;
            }

            same1 = SamePointXYZ (x1, y1, z1,
                               x4, y4, z4,
                               tiny);
            same2 = SamePointXYZ (x2, y2, z2,
                               x3, y3, z3,
                               tiny);
            if (same1 == 1  &&  same2 == 1) {
                sp2->used = 1;
                continue;
            }

        }  // end of j loop

    }  // end of i loop

    return 1;

}

/*--------------------------------------------------------------------*/

/*
 * Calculate the intersection lines between two surfaces which already exist
 * in either the horizon list or the fault list.  The results are appended to
 * the current WorkIntersectionLines array.
 *
 * This method returns 1 on success or -1 on an error.
 * Success does not mean that any intersections were found.  You need to
 * check the NumWorkIntersectionLines variable to see if any intersections
 * were found.
 *
 * If both padded surface numbers and types point to the same trimesh object,
 * no intersection is attempted and zero is returned.
 *
 * This is a protected method.
 */
int SealedModel::CalcPaddedSurfaceIntersectionLines (
    int    surf1_num,
    int    surf1_type,
    int    surf2_num,
    int    surf2_type)
{
    CSWTriMeshStruct    *surf1,
                        *surf2,
                        *stmp;
    TRiangleStruct      *tp1, *tp2;

    int                 istat, i, j, ntri, tmeshid1, tmeshid2;
    double              txmin, tymin, tzmin, txmax, tymax, tzmax;

/*
 * Make sure the triangles are indexed.
 */
    if (PaddedTriangle3DIndex == NULL) {
        istat = CreatePadded3DTriangleIndex ();
        if (istat == -1  ||  PaddedTriangle3DIndex == NULL) {
            return -1;
        }
    }

    if (modelGrazeDistance <= 0.0) {
        CalcModelGrazeDistance ();
    }

    surf1 = NULL;
    surf2 = NULL;

/*
 * Get the trimesh objects for each surface.
 */
    if (surf1_type == _HORIZON_TMESH_) {
        if (PaddedHorizonList == NULL) {
            return -1;
        }
        if (surf1_num >= NumPaddedHorizonList) {
            return -1;
        }
        surf1 = PaddedHorizonList + surf1_num;
        tmeshid1 = surf1_num + _HORIZON_ID_BASE_;
    }
    else if (surf1_type == _DETACHMENT_TMESH_) {
        surf1 = PaddedDetachment;
        tmeshid1 = _DETACHMENT_TMESH_;
    }
    else if (surf1_type == _FAULT_TMESH_) {
        if (PaddedFaultList == NULL) {
            return -1;
        }
        if (surf1_num >= NumPaddedFaultList) {
            return -1;
        }
        surf1 = PaddedFaultList + surf1_num;
        tmeshid1 = surf1_num + _FAULT_ID_BASE_;
    }
    else if (surf1_type == _BOUNDARY_TMESH_) {
        if (surf1_num == _NORTH_ID_) {
            surf1 = NorthBoundarySurface;
        }
        else if (surf1_num == _SOUTH_ID_) {
            surf1 = SouthBoundarySurface;
        }
        else if (surf1_num == _EAST_ID_) {
            surf1 = EastBoundarySurface;
        }
        else if (surf1_num == _WEST_ID_) {
            surf1 = WestBoundarySurface;
        }
        else {
            return 0;
        }
        tmeshid1 = surf1_num + _BOUNDARY_ID_BASE_;
    }
    else if (surf1_type == _SED_SURF_ID_) {
        surf1 = PaddedSedimentSurface;
        tmeshid1 = _SED_SURF_ID_;
    }
    else if (surf1_type == _MODEL_BOTTOM_ID_) {
        surf1 = PaddedModelBottom;
        tmeshid1 = _MODEL_BOTTOM_ID_;
    }
    else if (surf1_type == _BOTTOM_ID_) {
        surf1 = PaddedBottom;
        tmeshid1 = _BOTTOM_ID_;
    }
    else if (surf1_type == _TOP_ID_) {
        surf1 = PaddedTop;
        tmeshid1 = _TOP_ID_;
    }
    else {
        return 0;
    }

    if (surf2_type == _HORIZON_TMESH_) {
        if (PaddedHorizonList == NULL) {
            return -1;
        }
        if (surf2_num >= NumPaddedHorizonList) {
            return -1;
        }
        surf2 = PaddedHorizonList + surf2_num;
        tmeshid2 = surf2_num + _HORIZON_ID_BASE_;
    }
    else if (surf2_type == _DETACHMENT_TMESH_) {
        surf2 = PaddedDetachment;
        tmeshid2 = _DETACHMENT_TMESH_;
    }
    else if (surf2_type == _FAULT_TMESH_) {
        if (PaddedFaultList == NULL) {
            return -1;
        }
        if (surf2_num >= NumPaddedFaultList) {
            return -1;
        }
        surf2 = PaddedFaultList + surf2_num;
        tmeshid2 = surf2_num + _FAULT_ID_BASE_;
    }
    else if (surf2_type == _BOUNDARY_TMESH_) {
        if (surf2_num == _NORTH_ID_) {
            surf2 = NorthBoundarySurface;
        }
        else if (surf2_num == _SOUTH_ID_) {
            surf2 = SouthBoundarySurface;
        }
        else if (surf2_num == _EAST_ID_) {
            surf2 = EastBoundarySurface;
        }
        else if (surf2_num == _WEST_ID_) {
            surf2 = WestBoundarySurface;
        }
        else {
            return 0;
        }
        tmeshid2 = surf2_num + _BOUNDARY_ID_BASE_;
    }
    else if (surf2_type == _SED_SURF_ID_) {
        surf2 = PaddedSedimentSurface;
        tmeshid2 = _SED_SURF_ID_;
    }
    else if (surf2_type == _MODEL_BOTTOM_ID_) {
        surf2 = PaddedModelBottom;
        tmeshid2 = _MODEL_BOTTOM_ID_;
    }
    else if (surf2_type == _BOTTOM_ID_) {
        surf2 = PaddedBottom;
        tmeshid2 = _BOTTOM_ID_;
    }
    else if (surf2_type == _TOP_ID_) {
        surf2 = PaddedTop;
        tmeshid2 = _TOP_ID_;
    }
    else {
        return 0;
    }

    if (surf1 == NULL  ||  surf2 == NULL) {
        return 0;
    }

    int do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_api_obj.grd_WriteTextTriMeshFile (
            0, NULL,
            surf1->tris, surf1->num_tris,
            surf1->edges, surf1->num_edges,
            surf1->nodes, surf1->num_nodes,
            (char *)"surf1.tri");
        grd_api_obj.grd_WriteTextTriMeshFile (
            0, NULL,
            surf2->tris, surf2->num_tris,
            surf2->edges, surf2->num_edges,
            surf2->nodes, surf2->num_nodes,
            (char *)"surf2.tri");
    }

/*
 * Swap surf1 and surf2 if surf2 has less triangles.
 */
    if (surf2->num_tris < surf1->num_tris) {
        stmp = surf1;
        surf1 = surf2;
        surf2 = stmp;
        i = tmeshid1;
        tmeshid1 = tmeshid2;
        tmeshid2 = i;
    }

    ntri = surf1->num_tris;

/*
 * For each triangle in surf1, find any surf2 triangles close to
 * it (using the 3d triangle index).  Calculate intersection segments
 * between the surf1 triangle and each possible surf2 triangle.
 * The WorkIntersectionSegments list is updated with these intersections.
 */
    SPatial3DTriangleStructList *stout;
    SPatial3DTriangleStruct     *stlist, *stptr;
    int                         nstlist;
    double                      tiny;

    NumWorkIntersectionSegments = 0;

    for (i=0; i<ntri; i++) {

      /*
       * Find the 3d bounding box of the triangle.
       */
        tp1 = surf1->tris + i;
        Calc3DBox (tp1, surf1->edges, surf1->nodes,
                   &txmin, &tymin, &tzmin,
                   &txmax, &tymax, &tzmax,
                   &tiny);

      /*
       * Get all triangles within the bounding box that do not
       * belong to the surf1 trimesh.
       */
        stout =
            PaddedTriangle3DIndex->GetTriangles (
                tmeshid1,
                txmin, tymin, tzmin,
                txmax, tymax, tzmax);
        if (stout == NULL) {
            continue;
        }

        stlist = stout->list;
        nstlist = stout->nlist;

        if (stlist == NULL  ||  nstlist < 1) {
            csw_Free (stout);
            csw_Free (stlist);
            stout = NULL;
            stlist = NULL;
            continue;
        }

      /*
       * Check the triangle intersections between the current
       * surf1 triangle and all the surf2 triangle candidates
       * returned from the indexing.  The CalcTriangleIntersect
       * method adds segments to the WorkIntersectionSegments
       * list as needed.
       */
        for (j=0; j<nstlist; j++) {
            stptr = stlist + j;
            if (stptr->tmeshid != tmeshid2) {
                continue;
            }
            tp2 = surf2->tris + stptr->trinum;
            CalcTriangleIntersection (tp1, tp2, modelGrazeDistance / 10.0,
                                      surf1->edges,
                                      surf1->nodes,
                                      surf2->edges,
                                      surf2->nodes);
        }

      /*
       * Free the results returned from GetTriangles and check the next triangle.
       */
        csw_Free (stlist);
        csw_Free (stout);
        stlist = NULL;
        stout = NULL;
        nstlist = 0;

    }  // end of loop through the surf1 triangles

/*
 * Connect the segments in the WorkIntersectionSegment list and
 * add the intersection lines to the WorkIntersectionLines list.
 * The intersection lines are marked as shared by the two specified
 * trimesh id's.
 */
    FindOverlapWorkSegments ();
    ConnectIntersectionSegments (tmeshid1, tmeshid2);

    surf1->numIntersects += NumWorkIntersectionLines;
    surf2->numIntersects += NumWorkIntersectionLines;

/*
 * Move the work intersectionlines to the results intersection line list.
 */
    istat =
    AddWorkLinesToResults ();

    return istat;

}


/*----------------------------------------------------------------------------*/

/*
 * Create a triangle index and add all the padded horizon and fault surfaces to it.
 * On success, 1 is returned.  If the model bounds cannot be calculated, zero
 * is returned.  On a memory allocation failure, -1 is returned.
 */
int SealedModel::CreatePadded3DTriangleIndex (void)
{
    CSWTriMeshStruct      *tmesh;
    int                   i, istat, tmeshid;

    if (PaddedHorizonList == NULL  &&
        PaddedFaultList == NULL) {
        return 0;
    }

    if (padXmin > padXmax) {
        return 0;
    }

    if (PaddedTriangle3DIndex != NULL) {
        delete PaddedTriangle3DIndex;
        PaddedTriangle3DIndex = NULL;
    }

    try {
        SNF;
        PaddedTriangle3DIndex = new Spatial3DTriangleIndex (
                padXmin, padYmin, padZmin,
                padXmax, padYmax, padZmax);
    }
    catch (...) {
        printf ("\n***** Exception from new *****\n\n");
        PaddedTriangle3DIndex = NULL;
        return 0;
    }

    if (averageSpacing > 0.0) {
        PaddedTriangle3DIndex->SetGeometry (
        padXmin, padYmin, padZmin,
        padXmax, padYmax, padZmax,
        averageSpacing, averageSpacing, averageSpacing);
    }

  /*
   * Put the sediment surface and model bottom into the index.
   */
    if (PaddedSedimentSurface != NULL) {
        tmesh = PaddedSedimentSurface;
        istat =
        PaddedTriangle3DIndex->AddTriMesh (
            _SED_SURF_ID_,
            tmesh->nodes,
            tmesh->edges,
            tmesh->tris,
            tmesh->num_tris);
        if (istat == -1) {
            return -1;
        }
    }
    if (PaddedModelBottom != NULL) {
        tmesh = PaddedModelBottom;
        istat =
        PaddedTriangle3DIndex->AddTriMesh (
            _MODEL_BOTTOM_ID_,
            tmesh->nodes,
            tmesh->edges,
            tmesh->tris,
            tmesh->num_tris);
        if (istat == -1) {
            return -1;
        }
    }

/*
 * Put the top and bottom into the index.
 */
    if (PaddedTop != NULL) {
        tmesh = PaddedTop;
        istat =
        PaddedTriangle3DIndex->AddTriMesh (
            _TOP_ID_,
            tmesh->nodes,
            tmesh->edges,
            tmesh->tris,
            tmesh->num_tris);
        if (istat == -1) {
            return -1;
        }
    }
    if (PaddedBottom != NULL) {
        tmesh = PaddedBottom;
        istat =
        PaddedTriangle3DIndex->AddTriMesh (
            _BOTTOM_ID_,
            tmesh->nodes,
            tmesh->edges,
            tmesh->tris,
            tmesh->num_tris);
        if (istat == -1) {
            return -1;
        }
    }

  /*
   * Put the horizons into the index.
   */
    if (PaddedHorizonList != NULL) {
        for (i=0; i<NumPaddedHorizonList; i++) {
            tmesh = PaddedHorizonList + i;
            istat =
            PaddedTriangle3DIndex->AddTriMesh (
                i,
                tmesh->nodes,
                tmesh->edges,
                tmesh->tris,
                tmesh->num_tris);
            if (istat == -1) {
                return -1;
            }
        }
    }

  /*
   * Put the faults into the index.
   */
    if (PaddedFaultList != NULL) {
        for (i=0; i<NumPaddedFaultList; i++) {
            tmesh = PaddedFaultList + i;
            tmeshid = _FAULT_ID_BASE_ + i;
            istat =
            PaddedTriangle3DIndex->AddTriMesh (
                tmeshid,
                tmesh->nodes,
                tmesh->edges,
                tmesh->tris,
                tmesh->num_tris);
            if (istat == -1) {
                return -1;
            }
        }
    }

  /*
   * Put the boundary surfaces into the index.
   */
    if (NorthBoundarySurface != NULL) {
        tmeshid = _BOUNDARY_ID_BASE_ + _NORTH_ID_;
        tmesh = NorthBoundarySurface;
        istat =
        PaddedTriangle3DIndex->AddTriMesh (
            tmeshid,
            tmesh->nodes,
            tmesh->edges,
            tmesh->tris,
            tmesh->num_tris);
        if (istat == -1) {
            return -1;
        }
    }

    if (SouthBoundarySurface != NULL) {
        tmeshid = _BOUNDARY_ID_BASE_ + _SOUTH_ID_;
        tmesh = SouthBoundarySurface;
        istat =
        PaddedTriangle3DIndex->AddTriMesh (
            tmeshid,
            tmesh->nodes,
            tmesh->edges,
            tmesh->tris,
            tmesh->num_tris);
        if (istat == -1) {
            return -1;
        }
    }

    if (EastBoundarySurface != NULL) {
        tmeshid = _BOUNDARY_ID_BASE_ + _EAST_ID_;
        tmesh = EastBoundarySurface;
        istat =
        PaddedTriangle3DIndex->AddTriMesh (
            tmeshid,
            tmesh->nodes,
            tmesh->edges,
            tmesh->tris,
            tmesh->num_tris);
        if (istat == -1) {
            return -1;
        }
    }

    if (WestBoundarySurface != NULL) {
        tmeshid = _BOUNDARY_ID_BASE_ + _WEST_ID_;
        tmesh = WestBoundarySurface;
        istat =
        PaddedTriangle3DIndex->AddTriMesh (
            tmeshid,
            tmesh->nodes,
            tmesh->edges,
            tmesh->tris,
            tmesh->num_tris);
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}


/*-----------------------------------------------------------------*/

/*
 * Free the north, south, east and west boundary surfaces.
 * This is a protected method.
 */
void SealedModel::FreeVerticalBoundaries (void)
{
    FreeTmesh (NorthBoundarySurface, 1);
    FreeTmesh (SouthBoundarySurface, 1);
    FreeTmesh (EastBoundarySurface, 1);
    FreeTmesh (WestBoundarySurface, 1);
    NorthBoundarySurface = NULL;
    SouthBoundarySurface = NULL;
    EastBoundarySurface = NULL;
    WestBoundarySurface = NULL;
}


/*-----------------------------------------------------------------*/

/*
 * Create vertical boundaries at the north, south, east and west limits
 * of the model.  The current padXmin, padYmin, padZmin, padXmax, padYmax,
 * and padZmax are used for the model extents.
 */
int SealedModel::CreateVerticalBoundaries (void)
{
    CSW_F        *grid = NULL;
    NOdeStruct   *nodes = NULL;
    EDgeStruct   *edges = NULL;
    TRiangleStruct  *tris = NULL;
    int          numnodes, numedges, numtris;
    int          ncol, nrow, istat, i;
    double       xyspace, zspace;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            FreeVerticalBoundaries ();
            csw_Free (grid);
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (tris);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


  /*
   * Make sure any old boundaries have been csw_Freed.
   */
    FreeVerticalBoundaries ();

    verticalBoundarySpacing = averageSpacing;

  /*
   * Use the average spacing as a basis for the number of columns
   * in each boundary grid.  Modify the number of rows (z direction)
   * to simulate exagerating the z values to roughly the same extents
   * as the xy values.
   */
    xyspace = verticalBoundarySpacing;
    zspace = xyspace;

  /*
   * Make a grid for the north and south boundaries.
   */
    ncol = (int)((padXmax - padXmin) / xyspace + 1.5);
    nrow = (int)((padZmax - padZmin) / zspace + 1.5);
    nrow += 2;

    grid = (CSW_F *)csw_Calloc (ncol * nrow * sizeof(CSW_F));
    if (grid == NULL) {
        return -1;
    }

  /*
   * Create the North trimesh.
   */
    for (i=0; i<ncol*nrow; i++) {
        grid[i] = (CSW_F)padYmax;
    }

    numnodes = numedges = numtris = 0;
    istat =
      grd_api_obj.grd_CalcTriMeshFromGrid (
        grid, ncol, nrow,
        padXmin, padZmin-2.0*zspace, padXmax, padZmax,
        NULL, NULL, NULL, NULL, NULL, 0,
        GRD_EQUILATERAL,
        &nodes, &edges, &tris,
        &numnodes, &numedges, &numtris);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<numnodes; i++) {
        nodes[i].z = nodes[i].y;
        nodes[i].y = padYmax;
    }

    NorthBoundarySurface = (CSWTriMeshStruct *)csw_Calloc (
        sizeof(CSWTriMeshStruct));
    if (NorthBoundarySurface == NULL) {
        return -1;
    }

    double    zmid;

    NorthBoundarySurface->nodes = nodes;
    NorthBoundarySurface->num_nodes = numnodes;
    NorthBoundarySurface->edges = edges;
    NorthBoundarySurface->num_edges = numedges;
    NorthBoundarySurface->tris = tris;
    NorthBoundarySurface->num_tris = numtris;
    NorthBoundarySurface->is_padded = 1;
    NorthBoundarySurface->is_sealed = 0;
    NorthBoundarySurface->id = _BOUNDARY_ID_BASE_ + 1;
    NorthBoundarySurface->vflag = 1;
    zmid = (padZmin + padZmax) / 2.0;
    NorthBoundarySurface->vbase[0] = 0.0;
    NorthBoundarySurface->vbase[1] = 0.0;
    NorthBoundarySurface->vbase[2] = 100.0;
    NorthBoundarySurface->vbase[3] = (padXmin + padXmax) / 2.0;
    NorthBoundarySurface->vbase[4] = padYmax;
    NorthBoundarySurface->vbase[5] = zmid;

  /*
   * Create the South trimesh.
   */
    for (i=0; i<ncol*nrow; i++) {
        grid[i] = (CSW_F)padYmin;
    }

    numnodes = numedges = numtris = 0;
    istat =
      grd_api_obj.grd_CalcTriMeshFromGrid (
        grid, ncol, nrow,
        padXmin, padZmin-2.0*zspace, padXmax, padZmax,
        NULL, NULL, NULL, NULL, NULL, 0,
        GRD_EQUILATERAL,
        &nodes, &edges, &tris,
        &numnodes, &numedges, &numtris);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<numnodes; i++) {
        nodes[i].z = nodes[i].y;
        nodes[i].y = padYmin;
    }

    SouthBoundarySurface = (CSWTriMeshStruct *)csw_Calloc (
        sizeof(CSWTriMeshStruct));
    if (SouthBoundarySurface == NULL) {
        return -1;
    }

    SouthBoundarySurface->nodes = nodes;
    SouthBoundarySurface->num_nodes = numnodes;
    SouthBoundarySurface->edges = edges;
    SouthBoundarySurface->num_edges = numedges;
    SouthBoundarySurface->tris = tris;
    SouthBoundarySurface->num_tris = numtris;
    SouthBoundarySurface->is_padded = 1;
    SouthBoundarySurface->is_sealed = 0;
    SouthBoundarySurface->id = _BOUNDARY_ID_BASE_ + 2;
    SouthBoundarySurface->vflag = 1;
    zmid = (padZmin + padZmax) / 2.0;
    SouthBoundarySurface->vbase[0] = 0.0;
    SouthBoundarySurface->vbase[1] = 0.0;
    SouthBoundarySurface->vbase[2] = 100.0;
    SouthBoundarySurface->vbase[3] = (padXmin + padXmax) / 2.0;
    SouthBoundarySurface->vbase[4] = padYmin;
    SouthBoundarySurface->vbase[5] = zmid;

  /*
   * Finished with the north/south grid so csw_Free it.
   */
    csw_Free (grid);
    grid = NULL;

  /*
   * Make a grid for the east and west boundaries.
   * Note that it will have the same number of rows as the north
   * and south boundary grids.
   */
    ncol = (int)((padYmax - padYmin) / xyspace + 1.5);

    grid = (CSW_F *)csw_Calloc (ncol * nrow * sizeof(CSW_F));
    if (grid == NULL) {
        return -1;
    }

  /*
   * Create the East trimesh.
   */
    for (i=0; i<ncol*nrow; i++) {
        grid[i] = (CSW_F)padXmax;
    }

    numnodes = numedges = numtris = 0;
    istat =
      grd_api_obj.grd_CalcTriMeshFromGrid (
        grid, ncol, nrow,
        padYmin, padZmin-2.0*zspace, padYmax, padZmax,
        NULL, NULL, NULL, NULL, NULL, 0,
        GRD_EQUILATERAL,
        &nodes, &edges, &tris,
        &numnodes, &numedges, &numtris);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<numnodes; i++) {
        nodes[i].z = nodes[i].y;
        nodes[i].y = nodes[i].x;
        nodes[i].x = padXmax;
    }

    EastBoundarySurface = (CSWTriMeshStruct *)csw_Calloc (
        sizeof(CSWTriMeshStruct));
    if (EastBoundarySurface == NULL) {
        return -1;
    }

    EastBoundarySurface->nodes = nodes;
    EastBoundarySurface->num_nodes = numnodes;
    EastBoundarySurface->edges = edges;
    EastBoundarySurface->num_edges = numedges;
    EastBoundarySurface->tris = tris;
    EastBoundarySurface->num_tris = numtris;
    EastBoundarySurface->is_padded = 1;
    EastBoundarySurface->is_sealed = 0;
    EastBoundarySurface->id = _BOUNDARY_ID_BASE_ + 3;
    EastBoundarySurface->vflag = 1;
    zmid = (padZmin + padZmax) / 2.0;
    EastBoundarySurface->vbase[0] = 0.0;
    EastBoundarySurface->vbase[1] = 100.0;
    EastBoundarySurface->vbase[2] = 0.0;
    EastBoundarySurface->vbase[3] = padXmax;
    EastBoundarySurface->vbase[4] = (padYmin + padYmax) / 2.0;
    EastBoundarySurface->vbase[5] = zmid;

  /*
   * Create the West trimesh.
   */
    for (i=0; i<ncol*nrow; i++) {
        grid[i] = (CSW_F)padXmin;
    }
    istat =
      grd_api_obj.grd_CalcTriMeshFromGrid (
        grid, ncol, nrow,
        padYmin, padZmin-2.0*zspace, padYmax, padZmax,
        NULL, NULL, NULL, NULL, NULL, 0,
        GRD_EQUILATERAL,
        &nodes, &edges, &tris,
        &numnodes, &numedges, &numtris);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<numnodes; i++) {
        nodes[i].z = nodes[i].y;
        nodes[i].y = nodes[i].x;
        nodes[i].x = padXmin;
    }

    WestBoundarySurface = (CSWTriMeshStruct *)csw_Calloc (
        sizeof(CSWTriMeshStruct));
    if (WestBoundarySurface == NULL) {
        return -1;
    }

    WestBoundarySurface->nodes = nodes;
    WestBoundarySurface->num_nodes = numnodes;
    WestBoundarySurface->edges = edges;
    WestBoundarySurface->num_edges = numedges;
    WestBoundarySurface->tris = tris;
    WestBoundarySurface->num_tris = numtris;
    WestBoundarySurface->is_padded = 1;
    WestBoundarySurface->is_sealed = 0;
    WestBoundarySurface->id = _BOUNDARY_ID_BASE_ + 4;
    WestBoundarySurface->vflag = 1;
    zmid = (padZmin + padZmax) / 2.0;
    WestBoundarySurface->vbase[0] = 0.0;
    WestBoundarySurface->vbase[1] = 100.0;
    WestBoundarySurface->vbase[2] = 0.0;
    WestBoundarySurface->vbase[3] = padXmin;
    WestBoundarySurface->vbase[4] = (padYmin + padYmax) / 2.0;
    WestBoundarySurface->vbase[5] = zmid;

    padYNorth = padYmax;
    padYSouth = padYmin;
    padXEast = padXmax;
    padXWest = padXmin;

  /*
   * Finished with the east/west grid so csw_Free it.
   */
    csw_Free (grid);
    grid = NULL;

    bsuccess = true;

    return 1;

}




/*-------------------------------------------------------------------------------*/

/*
 * Calculate intersections between the surfaces and the vertical boundaries
 * of the model (north, south, east, west).
 */
int SealedModel::CalcPaddedSurfaceBoundaryIntersections (void)
{
    int            i, istat, start;

    for (i=0; i<NumPaddedHorizonList; i++) {
        if (NorthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                i, _HORIZON_TMESH_,
                _NORTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (SouthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                i, _HORIZON_TMESH_,
                _SOUTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (EastBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                i, _HORIZON_TMESH_,
                _EAST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (WestBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                i, _HORIZON_TMESH_,
                _WEST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
    }

    start = NumIntersectionLines;

    for (i=0; i<NumPaddedFaultList; i++) {
        if (NorthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                i, _FAULT_TMESH_,
                _NORTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (SouthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                i, _FAULT_TMESH_,
                _SOUTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (EastBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                i, _FAULT_TMESH_,
                _EAST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (WestBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                i, _FAULT_TMESH_,
                _WEST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
    }

    WritePartialIntersectionLines ((char *)"sidefault.xyz", start);

    sideFaultStart = start;
    sideFaultEnd = NumIntersectionLines;

    if (PaddedSedimentSurface != NULL) {
        if (NorthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _SED_SURF_ID_,
                _NORTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (SouthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _SED_SURF_ID_,
                _SOUTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (EastBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _SED_SURF_ID_,
                _EAST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (WestBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _SED_SURF_ID_,
                _WEST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }

    }

    if (PaddedModelBottom != NULL) {
        if (NorthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _MODEL_BOTTOM_ID_,
                _NORTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (SouthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _MODEL_BOTTOM_ID_,
                _SOUTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (EastBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _MODEL_BOTTOM_ID_,
                _EAST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
        if (WestBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _MODEL_BOTTOM_ID_,
                _WEST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }

    }

    if (IntersectionLines == NULL) {
        return 1;
    }

/*
 * Any fault or horizon intersection with a vertical boundary should
 * always be embedded into the fault.
 *
 * Lint gets confused here and warns about referencing past
 * the end of the IntersectionLines array, so the warning is
 * silenced.
 */
    for (i=start; i<NumIntersectionLines; i++) {
        IntersectionLines[i].embed_flag = 1; /*lint !e661*/
    }


    _INtersectionLine_    *iptr;
    int                   nlast;

    nlast = NumIntersectionLines;

/*
 * Intersect the top and bottom of the padded model extents
 * with the vertical sides.  The top and bottom were created
 * slightly inside the z extents of the vertical sides in
 * order to insure triangle intersections.  Once the intersect
 * lines are calculated, they are snapped to exactly the padded
 * boundary values.  The surfaces themselves are also snapped
 * to the exact padded boundary values after the intersects have
 * been calculated.
 *
 * The lint suppressions are for "out of bounds pointer" warnings.
 * PClint seems to be confused here.  The asserts prior to each
 * suppressed line will insure that no out of boounds pointer is
 * generated.
 */
    if (PaddedTop != NULL) {
        if (NorthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _TOP_ID_,
                _NORTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
            if (NumIntersectionLines != nlast + 1) {
                assert (0);
            }
            iptr = IntersectionLines + nlast; /*lint !e662*/
            SnapTopLine (iptr);
            nlast++;
        }
        if (SouthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _TOP_ID_,
                _SOUTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
            if (NumIntersectionLines != nlast + 1) {
                assert (0);
            }
            iptr = IntersectionLines + nlast; /*lint !e662*/
            SnapTopLine (iptr);
            nlast++;
        }
        if (EastBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _TOP_ID_,
                _EAST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
            if (NumIntersectionLines != nlast + 1) {
                assert (0);
            }
            iptr = IntersectionLines + nlast; /*lint !e662*/
            SnapTopLine (iptr);
            nlast++;
        }
        if (WestBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _TOP_ID_,
                _WEST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
            if (NumIntersectionLines != nlast + 1) {
                assert (0);
            }
            iptr = IntersectionLines + nlast; /*lint !e662*/
            SnapTopLine (iptr);
            nlast++;
        }
        //SnapTopSurface ();
    }


    if (PaddedBottom != NULL) {
        if (NorthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _BOTTOM_ID_,
                _NORTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
            if (NumIntersectionLines != nlast + 1) {
                assert (0);
            }
            iptr = IntersectionLines + nlast; /*lint !e662*/
            SnapBottomLine (iptr);
            nlast++;
        }
        if (SouthBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _BOTTOM_ID_,
                _SOUTH_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
            if (NumIntersectionLines != nlast + 1) {
                assert (0);
            }
            iptr = IntersectionLines + nlast; /*lint !e662*/
            SnapBottomLine (iptr);
            nlast++;
        }
        if (EastBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _BOTTOM_ID_,
                _EAST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
            if (NumIntersectionLines != nlast + 1) {
                assert (0);
            }
            iptr = IntersectionLines + nlast; /*lint !e662*/
            SnapBottomLine (iptr);
            nlast++;
        }
        if (WestBoundarySurface != NULL) {
            istat =
              CalcPaddedSurfaceIntersectionLines (
                0, _BOTTOM_ID_,
                _WEST_ID_, _BOUNDARY_TMESH_);
            if (istat == -1) {
                return -1;
            }
            if (NumIntersectionLines != nlast + 1) {
                assert (0);
            }
            iptr = IntersectionLines + nlast; /*lint !e662*/
            SnapBottomLine (iptr);
            nlast++;
        }
        //SnapBottomSurface ();
    }

    return 1;

}


/*----------------------------------------------------------------------------------*/

int SealedModel::UpdateIntersectionsForCrossing (CSWTriMeshStruct *surf)
{
  int            numIntersects, i, j;
  double         *xdec1, *ydec1, *zdec1,
                 *xdec2, *ydec2, *zdec2;
  int            ndec1, ndec2;
  int            is_boundary;
  _INtersectionLine_  *iptr, *jptr;

  GRDVert        gvert;

  if (SealedHorizonIntersects == NULL) {
    return 1;
  }

  if (NumSealedHorizonIntersects < 2) {
    return 1;
  }

/*
 * There have to be at least 2 intersections associated with the
 * surface to have a crossing.
 */
  numIntersects = surf->numIntersects;
  if (numIntersects < 2) {
    return 1;
  }

  is_boundary = 0;
  if (surf->id >= _BOUNDARY_ID_BASE_) {
    is_boundary = 1;
  }

  vert_SetBaseline (
    surf->vbase[0],
    surf->vbase[1],
    surf->vbase[2],
    surf->vbase[3],
    surf->vbase[4],
    surf->vbase[5],
    surf->vflag,
    gvert);

/*
 * Find the intersection lines that use this surface.
 * If any points come too close to one another, move
 * the close point from the jth surface to the nearest
 * point on the ith surface.
 */
  for (i=0; i<NumSealedHorizonIntersects-1; i++) {

    iptr = SealedHorizonIntersects + i;
    if (iptr->surf1 != surf->id  &&  iptr->surf2 != surf->id) {
      continue;
    }

    if (is_boundary == 0) {
      if (iptr->surf1 >= _BOUNDARY_ID_BASE_  ||
          iptr->surf2 >= _BOUNDARY_ID_BASE_) {
        continue;
      }
    }

    xdec1 = iptr->x;
    ydec1 = iptr->y;
    zdec1 = iptr->z;
    ndec1 = iptr->npts;

/*
 * If the surface needs to be rotated to nearly horizontal,
 * rotate the line before checking for intersects.
 */
    vert_ConvertPoints (xdec1, ydec1, zdec1, ndec1, gvert);

    int do_write = csw_GetDoWrite ();
    if (do_write) {
      char    fname[200];
      int     nwork = 1;
      sprintf (fname, "seal_hor_%d.xyz", i);
      grd_api_obj.grd_WriteLines (
        xdec1, ydec1, zdec1,
        1, &nwork, &ndec1,
        fname);
    }

    for (j=i+1; j<NumSealedHorizonIntersects; j++) {

      jptr = SealedHorizonIntersects + j;
      if (jptr->surf1 != surf->id  &&  jptr->surf2 != surf->id) {
        continue;
      }

      xdec2 = jptr->x;
      ydec2 = jptr->y;
      zdec2 = jptr->z;
      ndec2 = jptr->npts;

      vert_ConvertPoints (xdec2, ydec2, zdec2, ndec2, gvert);

      SnapLineToMaster (xdec1, ydec1, zdec1, ndec1,
                        xdec2, ydec2, zdec2, &ndec2);

      vert_UnconvertPoints (xdec2, ydec2, zdec2, ndec2, gvert);

      jptr->npts = ndec2;

    }

    vert_UnconvertPoints (xdec1, ydec1, zdec1, ndec1, gvert);

  }

  vert_UnsetBaseline ();

  return 1;
}




/*------------------------------------------------------------------------------------*/

/*
 * Calculate outlines of horizon patches and then clip the padded horizons
 * to the outlines.  The results are put into the sealedHorizons list.
 */
int SealedModel::OutlineAndCropHorizons (void)
{
    _OUtline_           *outline = NULL;
    CSWTriMeshStruct    *tmesh = NULL, *stmesh = NULL;
    int                 i, istat;

    auto fscope = [&]()
    {
        FreeOutlineList (HorizonOutlineList, NumHorizonList);
        HorizonOutlineList = NULL;
        csw_Free (stmesh);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (HorizonList == NULL) {
        return 0;
    }

    FreeOutlineList (HorizonOutlineList, NumHorizonList);
    HorizonOutlineList = (_OUtline_**)csw_Malloc (NumHorizonList * sizeof(_OUtline_ *));
    if (HorizonOutlineList == NULL) {
        return -1;
    }

    for (i=0; i<NumHorizonList; i++) {
        tmesh = HorizonList + i;

        outline = CalcSealedHorizonOutline (tmesh);
        HorizonOutlineList[i] = outline;
    }

    istat =
      CreateSealedHorizonIntersects ();
    if (istat == -1) {
        return -1;
    }

    istat =
      CreateSealedTopAndBottomIntersects ();
    if (istat == -1) {
        return -1;
    }

    istat =
      CreateSealedFaultIntersects ();
    if (istat == -1) {
        return -1;
    }

    istat = ResampleSealedIntersects ();
    if (istat == -1) {
        return -1;
    }

    istat = FixupCrossingIntersects ();
    if (istat == -1) {
        return -1;
    }

    istat = CorrectToFixedFaultLines (averageSpacing);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<NumHorizonList; i++) {

        tmesh = HorizonList + i;
        stmesh = CropSurface (tmesh);
        if (stmesh == NULL) {
            continue;
        }

        stmesh->external_id = tmesh->external_id;
        stmesh->age = tmesh->age;
        AddSealedHorizon (stmesh);
        csw_Free (stmesh);
        stmesh = NULL;
    }

    if (PaddedSedimentSurface != NULL) {
        SealedSedimentSurface =
          CropSurface (PaddedSedimentSurface);
        SealedSedimentSurface->id = PaddedSedimentSurface->id;
        SealedSedimentSurface->external_id = PaddedSedimentSurface->external_id;
        SealedSedimentSurface->age = PaddedSedimentSurface->age;
    }
    else if (PaddedTop != NULL) {
        SealedTop =
          CropSurface (PaddedTop);
        SealedTop->id = PaddedTop->id;
        SealedTop->external_id = PaddedTop->external_id;
        SealedTop->age = 0.0;
    }

    if (PaddedModelBottom != NULL) {
        SealedModelBottom =
          CropSurface (PaddedModelBottom);
        SealedModelBottom->id = PaddedModelBottom->id;
        SealedModelBottom->external_id = PaddedModelBottom->external_id;
        SealedModelBottom->age = PaddedModelBottom->age;
    }

    if (PaddedBottom != NULL) {
        SealedBottom =
          CropSurface (PaddedBottom);
        SealedBottom->id = PaddedBottom->id;
        SealedBottom->external_id = _BOUNDARY_ID_BASE_ + 5;
        SealedBottom->age = 10000;
    }

    if (simOutputFlag) {
        istat =
          SnapSealedFaultPointsToSealedHorizonPoints ();
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}


/*------------------------------------------------------------------------------------*/

/*
 * This method uses the polygon boolean operations to
 * create polygons from the padded horizon outline
 * and the (extended a bit) intersection lines.  The polygon
 * which has an original prepadded surface point inside
 * of it is the sealed surface outline.
 */
_OUtline_* SealedModel::CalcSealedHorizonOutline (CSWTriMeshStruct *tmesh)
{

  double          *xdec = NULL, *ydec = NULL, *zdec = NULL;
  double          *xborder = NULL, *yborder = NULL, *zborder = NULL;
  int             *nodeout = NULL;
  double          *xwork = NULL, *ywork = NULL, *zwork = NULL;
  int             nwork, maxcomp, maxpts, nwtot;
  int             npwork[1000];
  int             nborder, ndec, istat;
  int             ncborder[100], nvborder[100];
  NOdeStruct      *nodes = NULL;
  EDgeStruct      *edges = NULL;
  TRiangleStruct  *triangles = NULL;
  int             num_nodes, num_edges, num_triangles;
  int             i, j;
  int             line_nums[1000];
  int             nline_nums;
  void            **tag1 = NULL, **tag2 = NULL;
  _ITag_          *itag1 = NULL;

  _INtersectionLine_   *iptr, int_border;

  CSWPolyUtils    ply_utils_obj;

  double          *xpoly = NULL, *ypoly = NULL;
  int             maxpoly, maxpolypts;
  int             npolyout, *nptsout = NULL;

  double          xOrig, yOrig;
  int             nptot = 0;
  _OUtline_       *newOutline = NULL;


  auto fscope = [&]()
  {
    csw_Free (itag1);
    csw_Free (tag1);
    csw_Free (nptsout);
    csw_Free (xpoly);
    csw_Free (int_border.x);
    csw_Free (xwork);
    csw_Free (ywork);
    csw_Free (zwork);
    csw_Free (xborder);
    csw_Free (nptsout);
  };
  CSWScopeGuard func_scope_guard (fscope);


  if (IntersectionLines == NULL) {
    return NULL;
  }

  memset (&int_border, 0, sizeof(_INtersectionLine_));

/*
 * Find the intersections that use this horizon surface.
 * For each, save the points in the work arrays.
 */
  xwork = ywork = zwork = NULL;
  itag1 = NULL;
  maxcomp = 1000;
  maxpts = 0;
  nwtot = 0;
  nwork = 0;

  nline_nums = 0;
  for (i=0; i<NumIntersectionLines; i++) {

    iptr = IntersectionLines + i;

    if (iptr->surf1 != tmesh->id  &&  iptr->surf2 != tmesh->id) {
      continue;
    }

  /*
   * Get the array of xyz points for the decimated intersection line.
   */
    xdec = iptr->x;
    ydec = iptr->y;
    zdec = iptr->z;
    ndec = iptr->npts;

  /*
   * Add this line to the work arrays.
   */
    if (nwtot >= maxpts) {
      maxpts += ndec;
      xwork = (double *)csw_Realloc (xwork, maxpts * sizeof(double));
      ywork = (double *)csw_Realloc (ywork, maxpts * sizeof(double));
      zwork = (double *)csw_Realloc (zwork, maxpts * sizeof(double));
      itag1 = (_ITag_ *)csw_Realloc (itag1, maxpts * sizeof(_ITag_));
    }
    if (xwork == NULL  ||  ywork == NULL  ||  zwork == NULL) {
      return NULL;
    }
    memcpy (xwork+nwtot, xdec, ndec * sizeof(double));
    memcpy (ywork+nwtot, ydec, ndec * sizeof(double));
    memcpy (zwork+nwtot, zdec, ndec * sizeof(double));

    if (itag1 != NULL) {
      for (j=0; j<ndec; j++) {
        itag1[nwtot+j].lineid = i;
        itag1[nwtot+j].pointid = j;
        itag1[nwtot+j].lineid2 = -1;
        itag1[nwtot+j].pointid2 = -1;
      }
    }

    nwtot += ndec;
    npwork[nwork] = ndec;
    nwork++;
    if (nwork >= maxcomp) {
      return NULL;
    }

    line_nums[nline_nums] = i;
    nline_nums++;

  }

/*
 * Get the csw triangle, edge, node trimesh topology.
 */
  nodes = tmesh->nodes;
  edges = tmesh->edges;
  triangles = tmesh->tris;
  num_nodes = tmesh->num_nodes;
  num_edges = tmesh->num_edges;
  num_triangles = tmesh->num_tris;

/*
 * If any of the vertical boundary surfaces are NULL,
 * use the outline of the original trimesh as well for
 * the sealed trimesh border calculation.
 */
  if (NorthBoundarySurface == NULL  ||
      SouthBoundarySurface == NULL  ||
      EastBoundarySurface == NULL  ||
      WestBoundarySurface == NULL) {

    xborder = (double *)csw_Malloc (4 * num_nodes * sizeof(double));
    if (xborder == NULL) {
      return NULL;
    }
    yborder = xborder + num_nodes;
    zborder = yborder + num_nodes;
    nodeout = (int *)zborder + num_nodes;

    istat =
    grd_api_obj.grd_OutlineTriMeshBoundary (
      nodes, num_nodes,
      edges, num_edges,
      triangles, num_triangles,
      xborder, yborder, zborder,
      nodeout,
      &nborder, ncborder, nvborder,
      num_nodes, 100);
    if (istat == -1) {
      return NULL;
    }

    if (nborder != 1  ||  ncborder[0] != 1) {
      return NULL;
    }

  /*
   * Add the trimesh border to the work arrays.
   */
    if (nwtot + nvborder[0] >= maxpts) {
      maxpts = nwtot + nvborder[0] + 1;
      xwork = (double *)csw_Realloc (xwork, maxpts * sizeof(double));
      ywork = (double *)csw_Realloc (ywork, maxpts * sizeof(double));
      zwork = (double *)csw_Realloc (zwork, maxpts * sizeof(double));
      itag1 = (_ITag_ *)csw_Realloc (itag1, maxpts * sizeof(_ITag_));
    }

    if (xwork == NULL  ||  ywork == NULL  ||  zwork == NULL) {
      return NULL;
    }

    memcpy (xwork+nwtot, xborder, nvborder[0] * sizeof(double));
    memcpy (ywork+nwtot, yborder, nvborder[0] * sizeof(double));
    memcpy (zwork+nwtot, zborder, nvborder[0] * sizeof(double));

    if (itag1 != NULL) {
      for (j=0; j<nvborder[0]; j++) {
        itag1[nwtot+j].lineid = 1000001;
        itag1[nwtot+j].pointid = j;
        itag1[nwtot+j].lineid2 = -1;
        itag1[nwtot+j].pointid2 = -1;
      }
    }

    int_border.x = xborder;
    int_border.y = yborder;
    int_border.z = zborder;
    int_border.npts = nvborder[0];
    xborder = yborder = zborder = NULL;

    nwtot += nvborder[0];
    npwork[nwork] = nvborder[0];
    nwork++;
    if (nwork >= maxcomp) {
      return NULL;
    }
  }

/*
 * Calculate the polygons from the lines.
 */
  maxpoly = nwork * nwork;
  if (maxpoly < 100) maxpoly = 100;
  nptsout = (int *)csw_Malloc (maxpoly * sizeof(int));
  if (nptsout == NULL) {
    return NULL;
  }

  maxpolypts = nwtot * 10;
  xpoly = (double *)csw_Malloc (maxpolypts * 2 * sizeof(double));
  if (xpoly == NULL) {
    return NULL;
  }
  ypoly = xpoly + maxpolypts;




/*
 * !!!! debug stuff
 */
  int do_write = csw_GetDoWrite ();
  if (do_write) {
    char    fname[200];
    sprintf (fname, "inputlines.xyz");
    grd_api_obj.grd_WriteLines (
      xwork, ywork, NULL,
      1, &nwork, npwork,
      fname);
  }


  tag1 = (void **)csw_Calloc (2 * maxpolypts * sizeof(void *));
  if (tag1 == NULL) {
    return NULL;
  }
  tag2 = tag1 + maxpolypts;
  int ntot = 0;
  for (i=0; i<nwork; i++) {
    ntot += npwork[i];
  }

  if (itag1 != NULL) {
    for (i=0; i<ntot; i++) {
      tag1[i] = (void *)(itag1 + i);
    }
  }

  npolyout = 0;
  istat =
  grd_api_obj.ply_BuildPolygonsFromTaggedLines (
    xwork, ywork, tag1,
    nwork, npwork,
    xpoly, ypoly, tag2,
    &npolyout, nptsout,
    maxpolypts, maxpoly);


/*
 * !!!! debug stuff
 */
  do_write = csw_GetDoWrite ();
  if (do_write) {
    char fname[200];
    sprintf (fname, "polylines.xyz");
    grd_api_obj.grd_WriteLines (
      xpoly, ypoly, NULL,
      1, &npolyout, nptsout,
      fname);
  }





  if (istat == -1) {
    return NULL;
  }

  if (npolyout < 1) {
    return NULL;
  }

/*
 * Determine which polygon has the original surface point inside of it.
 */
  xOrig = tmesh->xcenter;
  yOrig = tmesh->ycenter;
  if (xOrig > 1.e20  ||  yOrig > 1.e20) {
    return NULL;
  }

  for (i=0; i<npolyout; i++) {

    istat =
    ply_utils_obj.ply_point (xpoly+nptot, ypoly+nptot, nptsout[i],
               xOrig, yOrig);

  /*
   * If the ith polygon contains the original surface point,
   * then build an outline object using the xy polygon points
   * and the z values from the original intersections or from
   * the original padded surface if a polygon point is not
   * the "same point" as an original intersection point.
   */
    if (istat == 1) {
      SetSharedOutlinePoints (
        xpoly+nptot, ypoly+nptot, tag2+nptot, nptsout[i], &int_border);
      newOutline =
      BuildHorizonOutline (xpoly+nptot, ypoly+nptot, tag2+nptot, nptsout[i],
                           nodes, num_nodes, tmesh);
      MarkIntersectionLinesToEmbed (
                        newOutline->x,
                        newOutline->y,
                        newOutline->z,
                        newOutline->npts,
                        line_nums, nline_nums);
      return newOutline;
    }
    nptot += nptsout[i];
  }

  return NULL;

}


/*
 * When the lineid of the tag array changes, determine if the
 * intersection point is the point with the new lineid or the
 * last point with the old lineid.  Change the tags so that
 * the intersection point is always the point with the new id.
 * This is only needed when either of the lineid's is the
 * original polygon border (id = 1000001).
 */
void SealedModel::SetSharedOutlinePoints (
  double        *xpoly,
  double        *ypoly,
  void          **vtags,
  int           npoly,
  _INtersectionLine_  *bptr
)
{
  int           i, id1, id2;
  _INtersectionLine_  *iptr1 = NULL, *iptr2 = NULL;
  double        xt1, yt1, xt2, yt2, dist1, dist2,
                dist11, dist12, dist21, dist22;
  _ITag_        *itptr = NULL;
  _ITag_        *tags = NULL;


  auto fscope = [&]()
  {
    csw_Free (tags);
  };
  CSWScopeGuard func_scope_guard (fscope);


  if (IntersectionLines == NULL) {
    return;
  }

  tags = (_ITag_ *)csw_Malloc (npoly * sizeof (_ITag_));
  if (tags == NULL) {
    return;
  }

  for (i=0; i<npoly; i++) {
    if (vtags[i] == NULL) {
      tags[i].lineid = -1;
      tags[i].pointid = -1;
      tags[i].lineid2 = -1;
      tags[i].pointid2 = -1;
    }
    else {
      itptr = (_ITag_ *)vtags[i];
      tags[i].lineid = itptr->lineid;
      tags[i].pointid = itptr->pointid;
      tags[i].lineid2 = -1;
      tags[i].pointid2 = -1;
    }
  }

  if (tags[0].lineid == -1) {
    for (i=1; i<npoly; i++) {
      if (tags[i].lineid >= 0) {
        tags[0].lineid = tags[i].lineid;
        break;
      }
    }
  }

  if (tags[0].lineid < 0) {
    return;
  }

  int lstart = tags[0].lineid;
  for (i=1; i<npoly; i++) {
    if (tags[i].lineid < 0) {
      tags[i].lineid = lstart;
    }
    else {
      lstart = tags[i].lineid;
    }
  }

  for (i=0; i<npoly-1; i++) {
    id1 = tags[i].lineid;
    id2 = tags[i+1].lineid;
    if (id1 == id2) {
      continue;
    }
    if (id1 < NumIntersectionLines  &&  id2 < NumIntersectionLines) {
      continue;
    }
    if (id1 < NumIntersectionLines) {
      iptr1 = IntersectionLines + id1; /*lint !e662*/
    }
    else {
      iptr1 = bptr;
    }
    if (id2 < NumIntersectionLines) {
      iptr2 = IntersectionLines + id2; /*lint !e662*/
    }
    else {
      iptr2 = bptr;
    }

    xt1 = xpoly[i];
    yt1 = ypoly[i];
    xt2 = xpoly[i+1];
    yt2 = ypoly[i+1];

    grd_api_obj.grd_DistanceToLine (iptr1->x, iptr1->y, iptr1->npts,
                        xt1, yt1, &dist11);
    grd_api_obj.grd_DistanceToLine (iptr1->x, iptr1->y, iptr1->npts,
                        xt2, yt2, &dist12);
    grd_api_obj.grd_DistanceToLine (iptr2->x, iptr2->y, iptr2->npts,
                        xt1, yt1, &dist21);
    grd_api_obj.grd_DistanceToLine (iptr2->x, iptr2->y, iptr2->npts,
                        xt2, yt2, &dist22);

    dist1 = dist11 + dist21;
    dist2 = dist12 + dist22;

    if (dist1 < dist2) {
      tags[i].lineid = id2;
    }
    else if (i == 0) {
      tags[i].lineid = id2;
      tags[npoly-1].lineid = id2;
    }

  }

  for (i=0; i<npoly; i++) {
    if (vtags[i] == NULL) {
      continue;
    }
    else {
      itptr = (_ITag_ *)vtags[i];
      itptr->lineid = tags[i].lineid;
    }
  }

  return;

}







/*---------------------------------------------------------------------*/

/*
 * Clip the specified surface to the specified boundary.
 *
 *  I have made a number of changes to this method to make use of
 *  new triangulation functions in the surface modeling library.
 *  I commented out sections of the previous code and I also added some debug
 *  code which is currently commented out.
 */
CSWTriMeshStruct* SealedModel::CropSurface(
  CSWTriMeshStruct *tmesh)
{
  NOdeStruct *nodes = NULL;
  EDgeStruct *edges = NULL;
  TRiangleStruct *triangles = NULL;
  _OUtline_  *outline = NULL;
  int num_nodes, num_edges, num_triangles;
  int i, istat;

  double* xOutline = NULL;
  double* yOutline = NULL;
  double* zOutline = NULL;

  bool     bsuccess = false;

  auto fscope = [&]()
  {
    csw_Free(xOutline);
    csw_Free(yOutline);
    csw_Free(zOutline);
    if (bsuccess == false) {
      csw_Free (nodes);
      csw_Free (edges);
      csw_Free (triangles);
    }
  };
  CSWScopeGuard func_scope_guard (fscope);


/*
 * Allocate space for a new cropped trimesh.  Copy the original
 * trimesh into it to start.
 */
  nodes = (NOdeStruct *)csw_Malloc (tmesh->num_nodes * sizeof(NOdeStruct));
  edges = (EDgeStruct *)csw_Malloc (tmesh->num_edges * sizeof(EDgeStruct));
  triangles = (TRiangleStruct *)csw_Malloc (tmesh->num_tris * sizeof(TRiangleStruct));
  if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
    return NULL;
  }
  memcpy (nodes, tmesh->nodes, tmesh->num_nodes * sizeof(NOdeStruct));
  memcpy (edges, tmesh->edges, tmesh->num_edges * sizeof(EDgeStruct));
  memcpy (triangles, tmesh->tris, tmesh->num_tris * sizeof(TRiangleStruct));
  num_nodes = tmesh->num_nodes;
  num_edges = tmesh->num_edges;
  num_triangles = tmesh->num_tris;

  if (tmesh->id == _SED_SURF_ID_  ||
      tmesh->id == _MODEL_BOTTOM_ID_  ||
      tmesh->id == _TOP_ID_  ||
      tmesh->id == _BOTTOM_ID_)  {
    outline = BuildOutlineFromSealedTopAndBottomBorder (&(tmesh->sealed_border));
  }
  else {
    outline = BuildOutlineFromSealedBorder (&(tmesh->sealed_border));
  }
  if (outline == NULL) {
    return NULL;
  }

/*
 * Put the points from the outline object into simple C style arrays
 * so they can be passed to the csw library functions.
 */
  int numOutlineNodes = outline->npts;
  int maxOutlineNodes = outline->npts + 1;
  xOutline = (double *) csw_Malloc(maxOutlineNodes * sizeof(double));
  yOutline = (double *) csw_Malloc(maxOutlineNodes * sizeof(double));
  zOutline = (double *) csw_Malloc(maxOutlineNodes * sizeof(double));
  if (xOutline == NULL  ||  yOutline == NULL  ||  zOutline == NULL) {
    return NULL;
  }

  int osize = outline->npts * sizeof(double);
  memcpy (xOutline, outline->x, osize);
  memcpy (yOutline, outline->y, osize);
  memcpy (zOutline, outline->z, osize);

  csw_Free (outline->x);
  csw_Free (outline);
  outline = NULL;

  double    dx, dy, dz, dist;
  dx = xOutline[0] - xOutline[numOutlineNodes-1];
  dy = yOutline[0] - yOutline[numOutlineNodes-1];
  dz = zOutline[0] - zOutline[numOutlineNodes-1];
  dist = dx * dx + dy * dy + dz * dz;
  if (dist > averageSpacing * .6) {
    xOutline[numOutlineNodes] = xOutline[0];
    yOutline[numOutlineNodes] = yOutline[0];
    zOutline[numOutlineNodes] = zOutline[0];
    numOutlineNodes++;
  }
  else if (dist > 0.0) {
    xOutline[numOutlineNodes-1] = xOutline[0];
    yOutline[numOutlineNodes-1] = yOutline[0];
    zOutline[numOutlineNodes-1] = zOutline[0];
  }

  int linepoints[1];               //number of points in
  linepoints[0] = numOutlineNodes; //each constrained line

  int lineflags[1];
  lineflags[0] = 0;

  int numLines = 1;

  int    ntot = 0;
  for (i=0; i<numLines; i++) {
    ntot += linepoints[i];
  }

  int do_write = csw_GetDoWrite ();
  if (do_write == 1) {
    for (i=0; i<num_nodes; i++) {
      if (nodes[i].z < -4.0) {
          printf ("bad node number %d\n", i);
      }
    }

    for (i=0; i<ntot; i++) {
      if (zOutline[i] < -4) {
          printf ("bad outline z at %d\n", i);
      }
    }
  }

/*
 * Add the decimated line to the trimesh as a constraint.
 * If this decimated line needs to be used by another trimesh
 * as its constraint, the exact same line must be used if the
 * two trimeshes are to seal to one another.  You need to save
 * the decimated line if this is the case.
 */
  istat = grd_api_obj.grd_AddLinesToTriMesh(
    xOutline,
    yOutline,
    zOutline,
    linepoints,
    lineflags,
    numLines,
    1,  // require exact adherance to lines
    &nodes,
    &edges,
    &triangles,
    &num_nodes,
    &num_edges,
    &num_triangles);
  if (istat == -1) {
    return NULL;
  }

/*
 * !!!! debug only
 */
  do_write = csw_GetDoWrite ();;
  if (do_write) {
    char fname1[200];
    sprintf (fname1, "preclip.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);

    sprintf (fname1, "polyborder.xyz");
    FILE *fptr = fopen (fname1, "wb");
    char    c200[200];
    if (fptr) {
        sprintf (c200, "1\n%d\n", numOutlineNodes);
        fputs (c200, fptr);
        for (i=0; i<numOutlineNodes; i++) {
            sprintf (c200, "%7.2f %7.2f %7.2f\n",
                     xOutline[i], yOutline[i], zOutline[i]);
            fputs (c200, fptr);
        }
        fclose (fptr);
    }
  }



/*
 * Since the outline may have been decimated, set the number of outline nodes
 * to the linepoints as returned from the decimation function.  Then, clip
 * the trimesh to the outline.
 */
  istat = grd_api_obj.grd_ClipTriMeshToPolygon(&nodes, &num_nodes,
                                   &edges, &num_edges,
                                   &triangles, &num_triangles,
                                   xOutline, yOutline,
                                   &numOutlineNodes, 1, 1);

  if (istat == -1) {
    return NULL;
  }

/*
 * !!!! debug only
 */
  do_write = csw_GetDoWrite ();
  if (do_write) {
    char fname1[200];
    sprintf (fname1, "postclip.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);
  }



/*
 * Create a new trimesh with the clipped results and return it.
 */
  CSWTriMeshStruct  *tmeshout = NULL;

  tmeshout = (CSWTriMeshStruct *)csw_Calloc (sizeof(CSWTriMeshStruct));
  if (tmeshout == NULL) {
    return NULL;
  }

  tmeshout->nodes = nodes;
  tmeshout->edges = edges;
  tmeshout->tris = triangles;
  tmeshout->num_nodes = num_nodes;
  tmeshout->num_edges = num_edges;
  tmeshout->num_tris = num_triangles;

  bsuccess = true;

  return tmeshout;

}


/*------------------------------------------------------------------------*/

/*
 * Find all the intersection objects that use the specified
 * surface id and put them into the surface as constraints.
 * This is used for faults and vertical model boundaries.  It
 * is not used for horizons, and will assert is a horizon id
 * is specified.
 */
int SealedModel::EmbedEdgesInFault(int id)
{

  CSWTriMeshStruct   *fault = NULL, *newfault = NULL;
  _INtersectionLine_ *iptr = NULL;
  double    *xdec = NULL, *ydec = NULL, *zdec = NULL;
  int       ndec, maxlines;
  double    *xc = NULL, *yc = NULL, *zc = NULL;
  int       *npc = NULL, nc, ntot, new_ntot, istat;
  int       index;
  double    minage, maxage;

  bool     bsuccess = false;

  auto fscope = [&]()
  {
    if (newfault != NULL) {
      csw_Free (newfault->nodes);
      csw_Free (newfault->edges);
      csw_Free (newfault->tris);
    }
    csw_Free (newfault);
    csw_Free (xc);
    csw_Free (yc);
    csw_Free (zc);
    csw_Free (npc);
    if (bsuccess == false) {
    }
  };
  CSWScopeGuard func_scope_guard (fscope);


  if (FaultList == NULL) {
    return 0;
  }

/*
 * Assert if a horizon id or boundary id is specified.
 */
  if (id < _FAULT_ID_BASE_  ||  id >= _BOUNDARY_ID_BASE_) {
    assert (0);
  }

  if (SealedHorizonIntersects == NULL) {
    return 0;
  }
  if (simSealFlag  &&  SealedFaultIntersects == NULL) {
    return 0;
  }

/*
 * Get the fault trimesh.
 */
  index = id - _FAULT_ID_BASE_;
  fault = FaultList + index;

  minage = fault->minage;
  maxage = fault->maxage;
  if (minage < 0.0  || maxage < 0.0) {
    return -1;
  }

/*
 * If the fault's min and max z values have not been
 * calculated yet, do that now.
 */
  if (fault->zmin >= fault->zmax) {
    calcFaultZLimits (fault);
  }

  xc = NULL;
  yc = NULL;
  zc = NULL;
  nc = 0;
  ntot = 0;

  maxlines = NumIntersectionLines + 1;
  if (maxlines < 100) maxlines = 100;
  npc = (int *)csw_Malloc (maxlines * sizeof(int));
  if (npc == NULL) {
    return -1;
  }

/*
 * If the fault has a detachment contact, make it an exact
 * constraint of the fault.
 */
  if (fault->xdetach != NULL) {
    new_ntot =
    add_constraint_for_embed (
      fault->xdetach, fault->ydetach, fault->zdetach, fault->ndetach,
      &xc, &yc, &zc,
      ntot);
    if (new_ntot == -1) {
      return -1;
    }
    ntot = new_ntot;
    faultLineFlags[nc] = GRD_UNDEFINED_CONSTRAINT;
    nc++;
    npc[0] = fault->ndetach;
  }

/*
 * Loop through the sealed fault intersects and use
 * any that share this fault.
 */
  int      i, j, otherid, nc1, nbad;

  nc1 = nc;

  if (SealedFaultIntersects) {
    for (i=0; i<NumSealedFaultIntersects; i++) {

      iptr = SealedFaultIntersects + i;

    /*
     * If neither shared surface id is this fault's id,
     * do not use this line.
     */
      if (iptr->surf1 != fault->id  &&  iptr->surf2 != fault->id) {
        continue;
      }

      xdec = iptr->x;
      ydec = iptr->y;
      zdec = iptr->z;
      ndec = iptr->npts;

      nbad = 0;
      if (fault->zmax > fault->zmin) {
        for (j=0; j<ndec; j++) {
          if (zdec[j] < fault->zmin  ||  zdec[j] > fault->zmax) {
            nbad++;
          }
        }
      }

      if (nbad == ndec) {
        continue;
      }

      new_ntot =
      add_constraint_for_embed (xdec, ydec, zdec, ndec,
                                &xc, &yc, &zc,
                                ntot);
      npc[nc] = ndec;
      faultLineFlags[nc] = GRD_MODEL_SIDE_CONSTRAINT;
      nc++;
      if (new_ntot == -1  ||  nc >= 2000) {
        return -1;
      }
      ntot = new_ntot;
    }
  }

  if (nc >= nc1 + 2) {
    fault->sealed_to_sides = 1;
  }
  else {
    fault->sealed_to_sides = 0;
  }

  double line_age;

  for (i=0; i<NumSealedHorizonIntersects; i++) {
    iptr = SealedHorizonIntersects + i;
    if (iptr->embed_flag == 0) {
      continue;
    }

    if (iptr->surf1 != fault->id  &&  iptr->surf2 != fault->id) {
      continue;
    }

  /*
   * Make sure the other, non fault surface that shares the intersection
   * is inside the age range of the fault.
   */
    if (iptr->surf1 == fault->id) {
      otherid = iptr->surf2;
    }
    else {
      otherid = iptr->surf1;
    }

    line_age = GetPaddedSurfaceAge (otherid);
    if (line_age < minage  ||  line_age > maxage) {
      continue;
    }

    xdec = iptr->x;
    ydec = iptr->y;
    zdec = iptr->z;
    ndec = iptr->npts;

    new_ntot =
    add_constraint_for_embed (xdec, ydec, zdec, ndec,
                              &xc, &yc, &zc,
                              ntot);
    npc[nc] = ndec;
    faultLineFlags[nc] = GRD_UNDEFINED_CONSTRAINT;
    nc++;
    if (new_ntot == -1) {
      return -1;
    }
    ntot = new_ntot;
  }

  if (nc < 1) {
    return 1;
  }

  WriteDebugFiles ();

/*
 * Copy the embedding surface so it can be changed without
 * disturbing the original.
 */
  newfault = (CSWTriMeshStruct *)csw_Calloc (sizeof(CSWTriMeshStruct));
  if (newfault == NULL) {
    return 1;
  }

/*
 * Copy all the scalar data and allocate new space for the arrays.
 */
  memcpy (newfault, fault, sizeof(CSWTriMeshStruct));
  newfault->nodes = (NOdeStruct *)csw_Malloc (fault->num_nodes * sizeof(NOdeStruct));
  newfault->edges = (EDgeStruct *)csw_Malloc (fault->num_edges * sizeof(EDgeStruct));
  newfault->tris = (TRiangleStruct *)csw_Malloc (fault->num_tris * sizeof(TRiangleStruct));
  if (newfault->nodes == NULL  ||
      newfault->edges == NULL  ||
      newfault->tris == NULL) {
    return 1;
  }

  WriteDebugFiles ();

/*
 * Copy the old arrays into the new fault.
 */
  memcpy (newfault->nodes, fault->nodes, fault->num_nodes * sizeof(NOdeStruct));
  memcpy (newfault->edges, fault->edges, fault->num_edges * sizeof(EDgeStruct));
  memcpy (newfault->tris, fault->tris, fault->num_tris * sizeof(TRiangleStruct));

/*
 * Apply the lines as exact constraints to the new fault.
 */
  istat =
    EmbedPointsInFaultSurface (newfault,
                               xc, yc, zc, npc, nc);
  if (istat == -1) {
    return 1;
  }

  WriteDebugFiles ();

  int do_write = csw_GetDoWrite ();
  if (do_write) {
    char   fname1[100];
    double v6[6];

    sprintf (fname1, "embed.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
      0, v6,
      newfault->tris, newfault->num_tris,
      newfault->edges, newfault->num_edges,
      newfault->nodes, newfault->num_nodes,
      fname1);

  }

/*
 * Remove the padded fault triangles that are outside of the
 * final sealed fault.
 */
  istat =
    ClipFaultToIntersections (newfault);
  if (istat == -1) {
    return 1;
  }

  if (do_write) {
    char   fname1[100];
    double v6[6];

    sprintf (fname1, "embedclip.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
      0, v6,
      newfault->tris, newfault->num_tris,
      newfault->edges, newfault->num_edges,
      newfault->nodes, newfault->num_nodes,
      fname1);

  }

/*
 * Adding a sealed fault makes a shallow copy of newfault, so
 * we have to csw_Free newfault, but not its nodes, edges, or tris.
 */
  AddSealedFault (newfault);

  bsuccess = true;

  return istat;

}






int SealedModel::add_constraint_for_embed (
    double        *xadd,
    double        *yadd,
    double        *zadd,
    int           nadd,
    double        **xcout,
    double        **ycout,
    double        **zcout,
    int           ntot)
{
  int        size;
  double     *xc, *yc, *zc;

  xc = *xcout;
  yc = *ycout;
  zc = *zcout;

  size = ntot + nadd;
  xc = (double *)csw_Realloc (xc, size * sizeof(double));
  if (xc == NULL) {
    return -1;
  }
  yc = (double *)csw_Realloc (yc, size * sizeof(double));
  if (yc == NULL) {
    csw_Free (xc);
    return -1;
  }
  zc = (double *)csw_Realloc (zc, size * sizeof(double));
  if (zc == NULL) {
    csw_Free (xc);
    csw_Free (yc);
    return -1;
  }

  memcpy (xc + ntot, xadd, nadd * sizeof(double));
  memcpy (yc + ntot, yadd, nadd * sizeof(double));
  memcpy (zc + ntot, zadd, nadd * sizeof(double));

/*
 * If the constraint is not a closed polygon, extend each end point slightly
 * along the end vector direction.
 */
  double      dx, dy, dz;
  double      dist;

  dx = xc[ntot] - xc[ntot+nadd-1];
  dy = yc[ntot] - yc[ntot+nadd-1];
  dz = zc[ntot] - zc[ntot+nadd-1];
  dist = dx * dx + dy * dy + dz * dz;
  dist = sqrt (dist);

/*
 * The line is very nearly closed so make it exactly closed.
 */
  if (dist < modelTiny) {
    xc[ntot+nadd-1] = xc[ntot];
    yc[ntot+nadd-1] = yc[ntot];
    zc[ntot+nadd-1] = zc[ntot];
  }

  *xcout = xc;
  *ycout = yc;
  *zcout = zc;

  return size;


}




/*--------------------------------------------------------------------------*/


/*
 * Add the specified constraint lines exactly to the specified trimesh.
 */
int SealedModel::EmbedPointsInSurface(
  CSWTriMeshStruct *tmesh,
  double    *xlines,
  double    *ylines,
  double    *zlines,
  int       *nplines,
  int       nlines)
{

/*
 * Get the current trimesh for the surface.
 */
  NOdeStruct *nodes;
  EDgeStruct *edges;
  TRiangleStruct *triangles;
  int num_nodes, num_edges, num_triangles;

  GRDVert     gvert;

  nodes = tmesh->nodes;
  edges = tmesh->edges;
  triangles = tmesh->tris;
  num_nodes = tmesh->num_nodes;
  num_edges = tmesh->num_edges;
  num_triangles = tmesh->num_tris;

  int    i, ntot;

  int    do_write, ncout[1000];
  char   fname1[200];
  double v6[6];
  do_write = csw_GetDoWrite ();;

  ntot = 0;
  for (i=0; i<nlines; i++) {
    ntot += nplines[i];
  }

  if (do_write) {
    for (i=0; i<nlines; i++) {
      ncout[i] = 1;
    }
    strcpy (fname1, "preconvert.xyz");
    grd_api_obj.grd_WriteLines (
        xlines, ylines, zlines,
        nlines, ncout, nplines, fname1);
  }

  if (do_write) {
    sprintf (fname1, "preconvert.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, v6,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);
  }

/*
 * Convert the trimesh nodes and lines to steep coordinates if needed.
 */
  vert_SetBaseline (
    tmesh->vbase[0],
    tmesh->vbase[1],
    tmesh->vbase[2],
    tmesh->vbase[3],
    tmesh->vbase[4],
    tmesh->vbase[5],
    tmesh->vflag,
    gvert);
  vert_ConvertPoints (xlines, ylines, zlines, ntot, gvert);
  vert_ConvertTriMeshNodes (nodes, num_nodes, gvert);

  if (do_write) {
    for (i=0; i<nlines; i++) {
      ncout[i] = 1;
    }
    strcpy (fname1, "embedlines.xyz");
    grd_api_obj.grd_WriteLines (
        xlines, ylines, zlines,
        nlines, ncout, nplines, fname1);
  }

  if (do_write) {
    sprintf (fname1, "embedinput.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, v6,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);
  }

  WriteDebugFiles ();

/*
 * Constrain the trimesh exactly to the lines.
 */
  int    istat;

  istat = grd_api_obj.grd_AddLinesToTriMesh(
    xlines,
    ylines,
    zlines,
    nplines,
    NULL,  // no line flags
    nlines,
    1,     // require exact adherance to lines
    &nodes,
    &edges,
    &triangles,
    &num_nodes,
    &num_edges,
    &num_triangles);

  if (istat == -1) {
    return -1;
  }

  if (do_write) {
    sprintf (fname1, "embedoutput.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, v6,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);
  }

  vert_UnconvertPoints (xlines, ylines, zlines, ntot, gvert);
  vert_UnconvertTriMeshNodes (nodes, num_nodes, gvert);

  tmesh->nodes = nodes;
  tmesh->num_nodes = num_nodes;
  tmesh->edges = edges;
  tmesh->num_edges = num_edges;
  tmesh->tris = triangles;
  tmesh->num_tris = num_triangles;

  WriteDebugFiles ();

  return 1;
}



/*--------------------------------------------------------------------------*/

/*
 * Build an outline object from the specified polygon.
 * The z values need to be assigned to the polygon and
 * then the x, y and z values are put into an outline object.
 */
_OUtline_ * SealedModel::BuildHorizonOutline (
  double       *xpoly,
  double       *ypoly,
  void         **vtags,
  int          npoly,
  NOdeStruct   *nodes,
  int          numnodes,
  CSWTriMeshStruct  *tmesh)
{
  double       dx, dy, dz, dxt, dyt, adx, ady,
               xt1, yt1, zt1, xt2, yt2, zt2;
  double       *zpoly = NULL, xt, yt, zt;
  double       xmin, ymin, xmax, ymax, xytiny;
  double       *xpoly2 = NULL, *ypoly2 = NULL, *zpoly2 = NULL;

  _OUtline_    *newOutline = NULL;

  _ITag_       *itptr = NULL;
  _ITag_       *itag = NULL;

  bool     bsuccess = false;

  auto fscope = [&]()
  {
    csw_Free (zpoly);
    CleanupNodeIndex ();
    if (bsuccess == false) {
      csw_Free (xpoly2);
      csw_Free (newOutline);
    }
  };
  CSWScopeGuard func_scope_guard (fscope);


  int          i, j, istat;
  int          jt1, j2;
  double       pct;

/*
 * Allocate space for the z values of the polygon.
 */
  zpoly = (double *)csw_Malloc (npoly * sizeof(double));
  if (zpoly == NULL) {
    return NULL;
  }

  xmin = 1.e30;
  ymin = 1.e30;
  xmax = -1.e30;
  ymax = -1.e30;
  for (i=0; i<npoly; i++) {
    if (xpoly[i] < xmin) xmin = xpoly[i];
    if (ypoly[i] < ymin) ymin = ypoly[i];
    if (xpoly[i] > xmax) xmax = xpoly[i];
    if (ypoly[i] > ymax) ymax = ypoly[i];
    zpoly[i] = 1.e30;
  }

  if (xmax <= xmin  ||  ymax <= ymin) {
    return NULL;
  }

/*
 * Make the "same point" distance about 1 part in 10000 of the polygon
 * average dimension.
 */
  xytiny = (xmax - xmin + ymax - ymin) / 20000.0;

/*
 * Index the nodes to speed up FromNodes
 */
  istat =
  CreateNodeIndex (
    nodes,
    numnodes,
    xmin,
    ymin,
    xmax,
    ymax);
  if (istat == -1) {
    return NULL;
  }

/*
 * Use a close intersection point or node if possible
 * for the z value.
 */
  for (i=0; i<npoly; i++) {
    xt = xpoly[i];
    yt = ypoly[i];
    zt = ZFromIntList (
      xt,
      yt,
      xytiny,
      tmesh->id);
    if (zt > 1.e20) {
      zt = ZFromNodes (
        xt,
        yt,
        nodes,
        xytiny);
    }
    zpoly[i] = zt;
  }

/*
 * Make sure the first and last polygon points have
 * valid z values.  If this is not the case, use the
 * closest (in point order) valid z for the endpoint.
 */
  if (zpoly[0] > 1.e20) {
    zpoly[0] = zpoly[npoly-1];
  }
  if (zpoly[npoly-1] > 1.e20) {
    zpoly[npoly-1] = zpoly[0];
  }
  if (zpoly[0] > 1.e20) {
    for (j=1; j<npoly; j++) {
      if (zpoly[j] < 1.e20) {
        zpoly[0] = zpoly[j];
        zpoly[npoly-1] = zpoly[j];
        break;
      }
    }
  }

/*
 * If the first point is still invalid, all the z values
 * must be invalid, and a NULL result is returned.
 */
  if (zpoly[0] > 1.e20) {
    return NULL;
  }

  xt1 = xpoly[0];
  yt1 = ypoly[0];
  zt1 = zpoly[0];

  i = 1;

/*
 * Silence lint warnings with the next 4 lines.
 */
  xt2 = 1.e30;
  yt2 = 1.e30;
  zt2 = 1.e30;
  j2 = -1;

/*
 * Fill in missing values by interpolation along the line.
 */
  for (;;) {

  /*
   * If the ith point has an invalid z, interpolate it.
   */
    if (zpoly[i] > 1.e20) {
      jt1 = i;
      for (j=i+1; j<npoly; j++) {
        if (zpoly[j] < 1.e20) {
          j2 = j - 1;
          xt2 = xpoly[j];
          yt2 = ypoly[j];
          zt2 = zpoly[j];
          i = j;
          break;
        }
      }

      dx = xt2 - xt1;
      dy = yt2 - yt1;
      dz = zt2 - zt1;
      for (j=jt1; j<=j2; j++) {
        dxt = xpoly[j] - xt1;
        dyt = ypoly[j] - yt1;
        if (dxt == 0.0  &&  dyt == 0.0) {
          zt = zt1;
        }
        else {
          adx = dxt;
          if (adx < 0) adx = -adx;
          ady = dyt;
          if (ady < 0) ady = -ady;
          if (adx > ady) {
            pct = dxt / dx;
          }
          else {
            pct = dyt / dy;
          }
          zt = zt1 + dz * pct;
        }
        zpoly[j] = zt;
      }
    }

  /*
   * If the ith point has a valid z value, use its xyz as the
   * start value for interpolation.
   */
    else {
      xt1 = xpoly[i];
      yt1 = ypoly[i];
      zt1 = zpoly[i];
    }

    i++;
    if (i >= npoly) {
      break;
    }

  }

/*
 * Create an _OUtline_ structure and fill it with the xyz polygon points.
 */

  newOutline = (_OUtline_ *)csw_Malloc (sizeof(_OUtline_));
  if (newOutline == NULL) {
    return NULL;
  }

  xpoly2 = (double *)csw_Malloc (npoly * 3 * sizeof(double));
  if (xpoly2 == NULL) {
    return NULL;
  }
  ypoly2 = xpoly2 + npoly;
  zpoly2 = ypoly2 + npoly;

/*
 * Transfer points to the outline object.
 */
  memcpy (xpoly2, xpoly, npoly * sizeof(double));
  memcpy (ypoly2, ypoly, npoly * sizeof(double));
  memcpy (zpoly2, zpoly, npoly * sizeof(double));

  itag = (_ITag_ *)csw_Malloc (npoly * sizeof (_ITag_));
  if (itag == NULL) {
    return NULL;
  }

  for (i=0; i<npoly; i++) {
    if (vtags[i] == NULL) {
      itag[i].lineid = -1;
      itag[i].pointid = -1;
      itag[i].lineid2 = -1;
      itag[i].pointid2 = -1;
    }
    else {
      itptr = (_ITag_ *)vtags[i];
      itag[i].lineid = itptr->lineid;
      itag[i].pointid = itptr->pointid;
      itag[i].lineid2 = -1;
      itag[i].pointid2 = -1;
    }
  }

  newOutline->x = xpoly2;
  xpoly2 = NULL;
  newOutline->y = ypoly2;
  ypoly2 = NULL;
  newOutline->z = zpoly2;
  zpoly2 = NULL;
  newOutline->npts = npoly;
  newOutline->surfid = tmesh->id;
  newOutline->itags = itag;

  bsuccess = true;

  return newOutline;

}



/*----------------------------------------------------------------------*/

/*
 * Search the object's IntersectionLines list for a point
 * close to the specified x and y.
 */
double SealedModel::ZFromIntList (
  double           xt,
  double           yt,
  double           tiny,
  int              tmeshid)
{
  int              i, j;
  _INtersectionLine_  *iptr;
  double           *xdec, *ydec, *zdec;
  int              ndec;
  double           dx, dy, dist;

  if (IntersectionLines == NULL) {
    return 1.e30;
  }

  for (i=0; i<NumIntersectionLines; i++) {

    iptr = IntersectionLines + i;
    if (iptr->surf1 != tmeshid  &&  iptr->surf2 != tmeshid) {
      continue;
    }

    xdec = iptr->x;
    ydec = iptr->y;
    zdec = iptr->z;
    ndec = iptr->npts;

    for (j=0; j<ndec; j++) {
      dx = xt - xdec[j];
      dy = yt - ydec[j];
      dist = dx * dx + dy * dy;
      dist = sqrt (dist);
      if (dist <= tiny) {
        return zdec[j];
      }
    }

  }

  return 1.e30;

}


/*----------------------------------------------------------------------------*/

/*
 * Build a spatial index of the specified nodes.  The index is a grid of
 * pointers to integers.  Each pointer points to an ineger list, where the
 * first element in the integer list is the maximum size of the list, the
 * second element is the number of node indices in the list, and the remainder
 * are the node indices for nodes lying within a cell of the index grid.
 * The NodeIndexGrid member has the grid.  The IndexNcol and IndexNrow pointers
 * have the columns and rows of the grid, respectively.  The IndexXmin and
 * IndexYmin members have the coordinates of the lower left corner of the grid.
 * The IndexXspace and IndexYspace have the width and height of each grid cell,
 * respectively.
 */
int SealedModel::CreateNodeIndex (
  NOdeStruct      *nodes,
  int             numnodes,
  double          xmin,
  double          ymin,
  double          xmax,
  double          ymax)
{
  double          aspect, margin, xspace, yspace, xt, yt;
  int             nc, nr, ntot;

  int     i, icol, irow, *list = NULL, k, n;


  auto fscope = [&]()
  {
    CleanupNodeIndex ();
  };
  CSWScopeGuard func_scope_guard (fscope);


/*
 * Try to make about numnodes * 4 cells, or 10000 at the most.
 */
  ntot = numnodes * 4;
  if (ntot > 10000) {
    ntot = 10000;
  }

/*
 * Try to make the width and height of cells about the same.
 */
  aspect = (ymax - ymin) / (xmax - xmin);
  nc = (int)(sqrt(ntot / aspect));
  if (nc < 2) {
    nc = 2;
  }
  nr = ntot / nc;
  if (nr < 2) {
    nr = 2;
  }

  IndexNcol = nc;
  IndexNrow = nr;

/*
 * Make the index grid areal extents
 * slightly larger than the specified limits.
 */
  margin = (xmax - xmin + ymax - ymin) / 200.0;
  xmin -= margin;
  ymin -= margin;
  xmax += margin;
  ymax += margin;

  xspace = (xmax - xmin) / (nc - 1);
  yspace = (ymax - ymin) / (nr - 1);

  IndexXspace = xspace;
  IndexYspace = yspace;
  IndexXmin = xmin;
  IndexYmin = ymin;

/*
 * Allocate space for the index grid.
 */
  CleanupNodeIndex ();
  NodeIndexGrid = (int **)csw_Calloc (nc * nr * sizeof(int *));
  if (NodeIndexGrid == NULL) {
    return -1;
  }

/*
 * Fill in the index grid.
 */
  for (i=0; i<numnodes; i++) {

  /*
   * Get the list for the cell containing this node.
   */
    xt = nodes[i].x;
    yt = nodes[i].y;
    icol = (int)((xt - xmin) / xspace);
    irow = (int)((yt - ymin) / yspace);
    if (icol < 0  ||  irow < 0  ||
        icol >= nc  ||  irow >= nr) {
      continue;
    }
    k = irow * nc + icol;
    list = NodeIndexGrid[k];
    if (list == NULL) {
      list = (int *)csw_Malloc (10 * sizeof(int));
      if (list == NULL) {
        return -1;
      }
      list[0] = 10;
      list[1] = 0;
      NodeIndexGrid[k] = list;
    }

  /*
   * Grow the list if needed.
   */
    n = list[1] + 2;
    if (n >= list[0]) {
      list[0] += 10;
      list = (int *)csw_Realloc (list, list[0] * sizeof(int));
      if (list == NULL) {
        return -1;
      }
      NodeIndexGrid[k] = list;
    }

  /*
   * Add the current node index (i) to this list.
   */
    list[n] = i;
    list[1]++;

  }

  return 1;

}


void SealedModel::CleanupNodeIndex (void)
{
  int      i, *list;

  if (NodeIndexGrid != NULL) {
    for (i=0; i<IndexNcol * IndexNrow; i++) {
      list = NodeIndexGrid[i];
      csw_Free (list);
    }
  }
  csw_Free (NodeIndexGrid);
  NodeIndexGrid = NULL;
  IndexXmin = 1.e30;
  IndexYmin = 1.e30;
  IndexXspace = -1.0;
  IndexYspace = -1.0;
  IndexNcol = -1;
  IndexNrow = -1;

  return;
}

/*-------------------------------------------------------------------------*/

/*
 * Return the z value of the closest node to the specified x, y point.
 * If the closest node is greater than tiny from x, y a value of 1.e30 is
 * returned.  The node index must have been set up prior to this call.
 */
double SealedModel::ZFromNodes (
  double            xt,
  double            yt,
  NOdeStruct        *nodes,
  double            tiny)
{
  int               index, jcol, irow, i1, i2, jt1, j2, i, j,
                    k, offset, *list, nlist, kk;
  double            dx, dy, dist, dmin, zmin;

  if (NodeIndexGrid == NULL) {
    return 1.e30;
  }

  zmin = 1.e30;
  dmin = 1.e30;

/*
 * Find the index grid cell containing xt, yt
 */
  jcol = (int)((xt - IndexXmin) / IndexXspace);
  irow = (int)((yt - IndexYmin) / IndexYspace);

  i1 = irow - 1;
  i2 = irow + 1;
  jt1 = jcol - 1;
  j2 = jcol + 1;

  if (jt1 >= IndexNcol  ||  i1 >= IndexNrow) {
    return zmin;
  }
  if (j2 < 0  ||  i2 < 0) {
    return zmin;
  }

  if (i1 < 0) i1 = 0;
  if (jt1 < 0) jt1 = 0;
  if (i2 > IndexNrow - 1) i2 = IndexNrow - 1;
  if (j2 > IndexNcol - 1) j2 = IndexNcol - 1;

/*
 * Check the cell containing xt, yt and the surrounding cells.
 * If there are any points in a cell, the list pointer is non
 * NULL and the number of points is in list[1].  The actual
 * point indices start in list[2].  (list[0] has the maximum
 * size of the list, which is not pertinent here);
 */
  for (i=i1; i<=i2; i++) {
    offset = i * IndexNcol;
    for (j=jt1; j<=j2; j++) {
      k = offset + j;
      list = NodeIndexGrid[k];
      if (list == NULL) {
        continue;
      }
      nlist = list[1];
      for (kk=0; kk<nlist; kk++) {
        index = list[kk+2];
        dx = xt - nodes[index].x;
        dy = yt - nodes[index].y;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        if (dist < dmin) {
          dmin = dist;
          if (dist <= tiny) {
            zmin = nodes[index].z;
          }
        }
      }
    }
  }

  return zmin;

}


/*-------------------------------------------------------------------*/

/*
 * Get the polygon border of the specified trimesh and then get
 * the centroid of that border polygon.
 */
int SealedModel::CalcTriMeshCentroid (
    CSWTriMeshStruct    *tmesh,
    double              *xcenter,
    double              *ycenter)
{
    int     npout, *ncout = NULL, *nvout = NULL;
    int     maxpts, maxcomp, *nodeout = NULL;
    int     istat;
    double  *xout = NULL, *yout = NULL, *zout = NULL, area;

    CSWPolyUtils   ply_utils_obj;
 

    auto fscope = [&]()
    {
      csw_Free (xout);
      csw_Free (ncout);
    };
    CSWScopeGuard func_scope_guard (fscope);


    maxpts = tmesh->num_edges;
    maxcomp = 100;

    xout = (double *)csw_Malloc (maxpts * 4 * sizeof(double));
    if (xout == NULL) {
        return -1;
    }
    yout = xout + maxpts;
    zout = yout + maxpts;
    nodeout = (int *)(zout + maxpts);

    ncout = (int *)csw_Malloc (maxcomp * 2 * sizeof(int));
    if (ncout == NULL) {
        return -1;
    }
    nvout = ncout + maxcomp;

    istat =
      grd_api_obj.grd_OutlineTriMeshBoundary (
          tmesh->nodes, tmesh->num_nodes,
          tmesh->edges, tmesh->num_edges,
          tmesh->tris, tmesh->num_tris,
          xout, yout, zout, nodeout,
          &npout, ncout, nvout,
          maxpts, maxcomp);
    if (istat == -1) {
        return -1;
    }

    istat =
      gpf_CalcPolygonAreaAndCentroid (
          xout, yout, nvout[0],
          xcenter, ycenter, &area);
    if (istat == -1) {
        return -1;
    }

/*
 * Make sure the centroid point is also inside the polygon.
 */
    istat = ply_utils_obj.ply_point (xout, yout, nvout[0],
                       *xcenter, *ycenter);
    if (istat == 1) {
        return 1;
    }

/*
 * The centroid is not inside the polygon, which can happen
 * with an irregularly shaped concave polygon.  In this case,
 * I find an interior point that is not really close to the
 * polygon boundary and use it as the "centroid".
 */
    double       xp, yp, xc, yc, dx, dy, dist, dmax;
    double       xmin, ymin, xmax, ymax;
    int          i, j, ido, jdo, ndo;

    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;

    for (i=0; i<nvout[0]; i++) {
        if (xout[i] < xmin) xmin = xout[i];
        if (yout[i] < ymin) ymin = yout[i];
        if (xout[i] > xmax) xmax = xout[i];
        if (yout[i] > ymax) ymax = yout[i];
    }

    if (xmin >= xmax  ||  ymin >= ymax) {
        return -1;
    }

/*
 * Superimpose a coarse rectangular grid over the polygon
 * bounding box.  For each grid node inside the polygon,
 * calculate its distance to the polygon border.  The node
 * with the largest distance is used as the "centroid".
 */
    dx = (xmax - xmin + ymax - ymin) / 16.0;
    jdo = (int)((xmax - xmin) / dx + .5);
    ido = (int)((ymax - ymin) / dx + .5);
    dmax = 0.0;
    xc = 1.e30;
    yc = 1.e30;
    ndo = 0;

    for (;;) {

        dx = (xmax - xmin) / (jdo - 1);
        dy = (ymax - ymin) / (ido - 1);
        for (i=1; i<ido; i++) {
            yp = ymin + i * dy;
            for (j=1; j<jdo; j++) {
                xp = xmin + j * dx;
                istat = ply_utils_obj.ply_point (xout, yout, nvout[0], xp, yp);
                if (istat == 1) {
                    istat = gpf_calclinedist2xy (
                        xout, yout, nvout[0], xp, yp, &dist);
                    if (istat == 0) {
                        if (dist > dmax) {
                            xc = xp;
                            yc = yp;
                            dmax = dist;
                        }
                    }
                }
            }
        }

    /*
     * If an interior point was found, use it.  If none was found,
     * make the grid cells smaller and try again.
     */
        if (xc < 1.e20  &&  yc < 1.e20) {
            break;
        }

        ido *= 2;
        jdo *= 2;
        ndo++;

    /*
     * If this loop has repeated more than 8 times, there is a problem.
     * After 8 times, the grid is 2000 by 2000.  Any even close too reasonable
     * data would not require this to get a point inside a polygon, so the
     * more likely explanation if the grid gets this big is a program bug.
     */
        if (ndo > 8) {
            printf ("The centroid calculation loop has repeated more than 8 times.\n");
            assert (0);
        }

    }

    *xcenter = xc;
    *ycenter = yc;

    return 1;

}



/*---------------------------------------------------------------------*/

/*
 * Go through the specified IntersectionLines indices and find the
 * lines that are very close to lying on the specified polygon.
 * These lines have their embed_flag member set to 1.
 */
void SealedModel::MarkIntersectionLinesToEmbed (
    double      *xpoly,
    double      *ypoly,
    double      *zpoly,
    int         npoly,
    int         *lines,
    int         nlines)
{
    _INtersectionLine_    *iptr;
    double      x, y, z, dx, dy, dz, dist, tiny;
    int         start, end, nclose, i, j, ipoly, maxclose;

    if (IntersectionLines == NULL) {
        return;
    }

    tiny = averageSpacing / 2.0;

    for (i=0; i<nlines; i++) {
        iptr = IntersectionLines + lines[i];
        iptr->nclose = 0;
    }

    for (ipoly=0; ipoly<npoly; ipoly++) {
        x = xpoly[ipoly];
        y = ypoly[ipoly];
        z = zpoly[ipoly];
        for (i=0; i<nlines; i++) {
            iptr = IntersectionLines + lines[i];
            if (iptr->embed_flag == 1) {
                continue;
            }
            start = 1;
            end = iptr->npts - 1;
            if (iptr->npts < 4) {
                start = 0;
                end = iptr->npts;
            }
            nclose = iptr->nclose;
            maxclose = iptr->npts / 4;
            if (maxclose < 2) maxclose = 2;
            if (iptr->npts < 3) {
                maxclose = 1;
            }
            for (j=start; j<end; j++) {
                dx = iptr->x[j] - x;
                dy = iptr->y[j] - y;
                dz = iptr->z[j] - z;
                dist = dx * dx + dy * dy + dz * dz;
                dist = sqrt (dist);
                if (dist <= tiny) {
                    nclose++;
                }
                if (nclose > maxclose) {
                    iptr->embed_flag = 1;
                    break;
                }
            }
            iptr->nclose = nclose;
        }
    }

    return;

}

/*---------------------------------------------------------------------*/
/*
 * Find all the intersection objects that use the specified
 * surface id and put them into the surface as constraints.
 * This is used for vertical boundaries, but the results are
 * added to the sealed fault list.
 */
int SealedModel::EmbedEdgesInBoundary(int id)
{

  CSWTriMeshStruct   *bound = NULL, *newbound = NULL, *fault = NULL;
  _INtersectionLine_ *iptr = NULL;
  double    *xdec = NULL, *ydec = NULL, *zdec = NULL;
  int       ndec, maxlines;
  double    *xc = NULL, *yc = NULL, *zc = NULL;
  int       *npc = NULL, *nfc = NULL, nc, ntot, new_ntot, istat;
  int       index, nbad, j;

  bool     bsuccess = false;

  auto fscope = [&]()
  {
    if (bsuccess == false) {
      csw_Free (newbound->nodes);
      csw_Free (newbound->edges);
      csw_Free (newbound->tris);
    }
    csw_Free (newbound);
    csw_Free (xc);
    csw_Free (yc);
    csw_Free (zc);
    csw_Free (npc);
  };
  CSWScopeGuard func_scope_guard (fscope);


/*
 * Assert if a horizon id or fault id is specified.
 */
  if (id < _BOUNDARY_ID_BASE_) {
    assert (0);
  }

  if (SealedFaultIntersects == NULL  ||  SealedHorizonIntersects == NULL) {
    return 0;
  }

/*
 * Get the boundary trimesh.
 */
  bound = NULL;
  index = id - _BOUNDARY_ID_BASE_;
  if (index == _NORTH_ID_) {
    bound = NorthBoundarySurface;
  }
  else if (index == _SOUTH_ID_) {
    bound = SouthBoundarySurface;
  }
  else if (index == _EAST_ID_) {
    bound = EastBoundarySurface;
  }
  else if (index == _WEST_ID_) {
    bound = WestBoundarySurface;
  }
  else {
    return -1;
  }

  if (bound == NULL) {
    return -1;
  }

  xc = NULL;
  yc = NULL;
  zc = NULL;
  nc = 0;
  ntot = 0;

  maxlines = NumIntersectionLines;
  if (maxlines < 100) maxlines = 100;
  npc = (int *)csw_Malloc (2 * maxlines * sizeof(int));
  if (npc == NULL) {
    return -1;
  }
  nfc = npc + maxlines;

/*
 * Loop through the sealed fault intersects and find intersections where
 * one of the shared surfaces is this boundary.
 */
  int      i;
  for (i=0; i<NumSealedFaultIntersects; i++) {

    iptr = SealedFaultIntersects + i;
    if (iptr->embed_flag == 0) {
      continue;
    }

  /*
   * If neither shared surface is this boundary
   * do not use this line.
   */
    if (iptr->surf1 != bound->id  &&  iptr->surf2 != bound->id) {
      continue;
    }

    if (iptr->npts < 2) {
      continue;
    }

    xdec = iptr->x;
    ydec = iptr->y;
    zdec = iptr->z;
    ndec = iptr->npts;

    int faultid;
    faultid = iptr->surf1;
    if (faultid == bound->id) {
      faultid = iptr->surf2;
    }

    nbad = 0;
    if (faultid >= _FAULT_ID_BASE_  &&
        faultid < _BOUNDARY_ID_BASE_  &&
        FaultList != NULL) {
      fault = FaultList + (faultid - _FAULT_ID_BASE_);
      if (fault->zmax > fault->zmin) {
        for (j=0; j<ndec; j++) {
          if (zdec[j] < fault->zmin  ||  zdec[j] > fault->zmax) {
            nbad++;
          }
        }
      }
    }

    if (nbad == ndec) {
      continue;
    }

    new_ntot =
    add_constraint_for_embed (xdec, ydec, zdec, ndec,
                              &xc, &yc, &zc,
                              ntot);
    npc[nc] = ndec;
    nfc[nc] = 0;
    nc++;
    if (new_ntot == -1) {
      return -1;
    }
    ntot = new_ntot;
  }


  for (i=0; i<NumSealedHorizonIntersects; i++) {

    iptr = SealedHorizonIntersects + i;
    if (iptr->embed_flag == 0) {
      continue;
    }

    if (iptr->surf1 != bound->id  &&  iptr->surf2 != bound->id) {
      continue;
    }

    xdec = iptr->x;
    ydec = iptr->y;
    zdec = iptr->z;
    ndec = iptr->npts;

    new_ntot =
    add_constraint_for_embed (xdec, ydec, zdec, ndec,
                              &xc, &yc, &zc,
                              ntot);
    npc[nc] = ndec;
    nfc[nc] = 0;
    nc++;
    if (new_ntot == -1) {
      return -1;
    }
    ntot = new_ntot;
  }

  if (SealedTopAndBottomIntersects != NULL) {
    for (i=0; i<NumSealedTopAndBottomIntersects; i++) {

      iptr = SealedTopAndBottomIntersects + i;
      if (iptr->embed_flag == 0) {
        continue;
      }

      if (iptr->surf1 != bound->id  &&  iptr->surf2 != bound->id) {
        continue;
      }

      xdec = iptr->x;
      ydec = iptr->y;
      zdec = iptr->z;
      ndec = iptr->npts;

      new_ntot =
      add_constraint_for_embed (xdec, ydec, zdec, ndec,
                                &xc, &yc, &zc,
                                ntot);
      npc[nc] = ndec;
      nfc[nc] = LIMIT_LINE_FLAG;
      nc++;
      if (new_ntot == -1) {
        return -1;
      }
      ntot = new_ntot;
    }
  }

  if (nc < 1) {
    return 1;
  }

/*
 * Copy the embedding surface so it can be changed without
 * disturbing the original.
 */
  newbound = (CSWTriMeshStruct *)csw_Calloc (sizeof(CSWTriMeshStruct));
  if (newbound == NULL) {
    return 1;
  }

/*
 * Copy all the scalar data and allocate new space for the arrays.
 */
  memcpy (newbound, bound, sizeof(CSWTriMeshStruct));
  newbound->nodes = (NOdeStruct *)csw_Malloc (bound->num_nodes * sizeof(NOdeStruct));
  newbound->edges = (EDgeStruct *)csw_Malloc (bound->num_edges * sizeof(EDgeStruct));
  newbound->tris = (TRiangleStruct *)csw_Malloc (bound->num_tris * sizeof(TRiangleStruct));
  if (newbound->nodes == NULL  ||
      newbound->edges == NULL  ||
      newbound->tris == NULL) {
    return 1;
  }

/*
 * Copy the old arrays into the new bound.
 */
  memcpy (newbound->nodes, bound->nodes, bound->num_nodes * sizeof(NOdeStruct));
  memcpy (newbound->edges, bound->edges, bound->num_edges * sizeof(EDgeStruct));
  memcpy (newbound->tris, bound->tris, bound->num_tris * sizeof(TRiangleStruct));

/*
 * Apply the lines as exact constraints to the new bound.
 */
  istat =
    EmbedPointsInBoundarySurface (newbound,
                                  xc, yc, zc, npc, nfc, nc);
  if (istat == -1) {
    return 1;
  }

/*
 * Crop boundary to model surface and model bottom.
 * TEMP
 */
  istat =
    ClipBoundaryToIntersections (newbound);
  if (istat == -1) {
    return 1;
  }

/*
 * Adding a sealed bound makes a shallow copy of newbound, so
 * we have to csw_Free newbound, but not its nodes, edges, or tris.
 */
  AddSealedFault (newbound);

  bsuccess = true;

  return istat;

}

/*---------------------------------------------------------------------*/

int SealedModel::ClipFaultToIntersections (
  CSWTriMeshStruct *tmesh)
{
  int             i, istat;

/*
 * Find the border node with the highest z value.
 */
  int      startnode = -1;
  double   zmax = -1.e30, zmin = 1.e30;
  int      n1, n2;
  EDgeStruct  *eptr;

  for (i=0; i<tmesh->num_edges; i++) {

    eptr = tmesh->edges + i;
    if (eptr->tri2 >= 0) {
      continue;
    }

    n1 = eptr->node1;
    n2 = eptr->node2;

    if (tmesh->nodes[n1].z > zmax) {
      zmax = tmesh->nodes[n1].z;
      startnode = n1;
    }

    if (tmesh->nodes[n2].z > zmax) {
      zmax = tmesh->nodes[n2].z;
      startnode = n2;
    }

  }

  if (startnode == -1) {
    return -1;
  }

/*
 * Starting at this highest node, delete all the triangles
 * topologically between that node and the nearest constraint
 * edges in the trimesh.  This will trim the top of the
 * topmost constraint lines.
 */
  WriteDebugFiles ();

  int do_write = csw_GetDoWrite ();
  if (do_write) {
    char    fname[100];
    sprintf (fname, "prechew1.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tmesh->tris, tmesh->num_tris,
        tmesh->edges, tmesh->num_edges,
        tmesh->nodes, tmesh->num_nodes,
        fname);
  }

  istat =
    grd_api_obj.grd_ChewUpTriangles (
      startnode,
      tmesh->nodes, &tmesh->num_nodes,
      tmesh->edges, &tmesh->num_edges,
      tmesh->tris, &tmesh->num_tris);
  if (istat == -1) {
    return -1;
  }

  WriteDebugFiles ();

  if (do_write) {
    char    fname[100];
    sprintf (fname, "postchew1.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tmesh->tris, tmesh->num_tris,
        tmesh->edges, tmesh->num_edges,
        tmesh->nodes, tmesh->num_nodes,
        fname);
  }

  if (tmesh->sealed_to_sides == 1  &&  simOutputFlag == 1) {
    return 1;
  }

/*
 * Find a border node of the fault that is on the model top.
 * This node is used to remove triangles topologically connected
 * to it but still topologically "above" the highest horizon
 * intersection lines on the fault.  The result of this operation
 * is the sealed fault trimesh.  If the simSealFlag is not set,
 * there will be no model top, so this is not needed.
 */
  if (simOutputFlag) {
    double ztiny = (padZmax - padZmin) / 1000000.0;
    startnode = -1;
    for (i=0; i<tmesh->num_edges; i++) {

      eptr = tmesh->edges + i;
      if (eptr->tri2 >= 0) {
        continue;
      }

      n1 = eptr->node1;
      n2 = eptr->node2;

      istat = SameValue (padZmax,
                         tmesh->nodes[n1].z,
                         ztiny);
      if (istat == 1) {
        startnode = n1;
        break;
      }

      istat = SameValue (padZmax,
                         tmesh->nodes[n2].z,
                         ztiny);
      if (istat == 1) {
        startnode = n2;
        break;
      }

    }

  /*
   * If no node on the fault is on the top, use the highest
   * border node.
   */
    if (startnode == -1) {
      zmax = -1.e30;
      for (i=0; i<tmesh->num_edges; i++) {

        eptr = tmesh->edges + i;
        if (eptr->tri2 >= 0) {
          continue;
        }

        n1 = eptr->node1;
        n2 = eptr->node2;

        if (tmesh->nodes[n1].z > zmax) {
          zmax = tmesh->nodes[n1].z;
          startnode = n1;
        }

        if (tmesh->nodes[n2].z > zmax) {
          zmax = tmesh->nodes[n2].z;
          startnode = n2;
        }
      }
    }

  /*
   * If there still isn't a startnode, something is very wrong.
   */
    if (startnode == -1) {
      printf ("Could not find a start node for trimesh clipping.\n");
      assert (0);
    }

    if (do_write) {
      char    fname[100];
      sprintf (fname, "prechew2.tri");
      grd_api_obj.grd_WriteTextTriMeshFile (
          0, NULL,
          tmesh->tris, tmesh->num_tris,
          tmesh->edges, tmesh->num_edges,
          tmesh->nodes, tmesh->num_nodes,
          fname);
    }

    istat =
      grd_api_obj.grd_ChewUpTriangles (
        startnode,
        tmesh->nodes, &tmesh->num_nodes,
        tmesh->edges, &tmesh->num_edges,
        tmesh->tris, &tmesh->num_tris);
    if (istat == -1) {
      return -1;
    }

    if (do_write) {
      char    fname[100];
      sprintf (fname, "postchew2.tri");
      grd_api_obj.grd_WriteTextTriMeshFile (
          0, NULL,
          tmesh->tris, tmesh->num_tris,
          tmesh->edges, tmesh->num_edges,
          tmesh->nodes, tmesh->num_nodes,
          fname);
    }

  }

/*
 * The lower part of the fault may also need to be trimmed,
 * if constraint lines completely intersect the fault.
 * Only check nodes that are on the trimesh border and that
 * are not on a constraint edge.
 */
  int startnodeflag = -1;
  startnode = -1;
  for (i=0; i<tmesh->num_edges; i++) {

    eptr = tmesh->edges + i;
    if (eptr->tri2 >= 0) {
      continue;
    }

    n1 = eptr->node1;
    n2 = eptr->node2;

    if (tmesh->nodes[n1].z < zmin) {
      zmin = tmesh->nodes[n1].z;
      startnodeflag = eptr->flag;
      startnode = n1;
    }

    if (tmesh->nodes[n2].z < zmin) {
      zmin = tmesh->nodes[n2].z;
      startnodeflag = eptr->flag;
      startnode = n2;
    }

  }

/*
 * There may not be any lower clipping needed.  If this is the case,
 * return success.
 */
  if (startnode == -1) {
    WriteDebugFiles ();
    return 1;
  }

  if (startnodeflag == -1) {
    WriteDebugFiles ();
    return 1;
  }

  if (!(startnodeflag == 0  ||
        startnodeflag == GRD_MODEL_SIDE_CONSTRAINT)) {
    WriteDebugFiles ();
    return 1;
  }

  for (i=0; i<tmesh->num_edges; i++) {

    eptr = tmesh->edges + i;
    if (eptr->deleted) {
        continue;
    }

    if (eptr->tri2 >= 0) {
      continue;
    }

    n1 = eptr->node1;
    n2 = eptr->node2;

    if (!(n1 == startnode  ||  n2 == startnode)) {
      continue;
    }

    if (!(eptr->flag == 0  ||
          eptr->flag == GRD_MODEL_SIDE_CONSTRAINT)) {
      WriteDebugFiles ();
      return 1;
    }

  }

/*
 * Starting at this lowest node, delete all the triangles
 * topologically between that node and the nearest constraint
 * edges in the trimesh.  This will trim the bottom of the
 * trimesh to the lowest constraint lines.
 */
  WriteDebugFiles ();

  do_write = csw_GetDoWrite ();
  if (do_write) {
    char    fname[100];
    sprintf (fname, "prechew3.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tmesh->tris, tmesh->num_tris,
        tmesh->edges, tmesh->num_edges,
        tmesh->nodes, tmesh->num_nodes,
        fname);
  }

  istat =
    grd_api_obj.grd_ChewUpTriangles (
      startnode,
      tmesh->nodes, &tmesh->num_nodes,
      tmesh->edges, &tmesh->num_edges,
      tmesh->tris, &tmesh->num_tris);
  if (istat == -1) {
    return -1;
  }

  if (do_write) {
    char    fname[100];
    sprintf (fname, "postchew3.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tmesh->tris, tmesh->num_tris,
        tmesh->edges, tmesh->num_edges,
        tmesh->nodes, tmesh->num_nodes,
        fname);
  }

  WriteDebugFiles ();

  return 1;

}

int SealedModel::CheckForClosedPath (_INtersectionLine_ **list, int nlist)
{
    int         i, isame, ndo, found;
    double      xfirst, yfirst, zfirst, xlast, ylast, zlast,
                x1, y1, z1, x2, y2, z2;
    _INtersectionLine_  *iptr;

    for (i=0; i<nlist; i++) {
        iptr = list[i];
        iptr->nclose = 0;
    }

    int do_write = csw_GetDoWrite ();
    if (do_write) {
        char fname[100];
        sprintf (fname, "closepath.xyz");
        WriteIlinesToFile (
            list, nlist,
            fname);
    }

/*
 * The initial first and last points are the endpoints of the
 * first intersection line in the list.
 */
    iptr = list[0];
    xfirst = iptr->x[0];
    yfirst = iptr->y[0];
    zfirst = iptr->z[0];
    xlast = iptr->x[iptr->npts-1];
    ylast = iptr->y[iptr->npts-1];
    zlast = iptr->z[iptr->npts-1];
    iptr->nclose = 1;

/*
 * Search for other lines in the list that have not been
 * used and that have an endpoint at xlast, ylast, zlast.
 * As each such line is found, update the last coordinates
 * to the lines opposite endpoint.  When the updated last
 * coordinates are the same as the xfirst, yfirst, zfirst
 * point, the path is closed.  If no line is found to
 * attach to the current xlast, ylast, zlast then the path
 * is not closed.  If more than nlist iterations are done,
 * the path is not closed.
 */
    ndo = 0;

    for (;;) {

        found = 0;

        for (i=0; i<nlist; i++) {

            iptr = list[i];
            if (iptr->nclose == 1) {
                continue;
            }

            x1 = iptr->x[0];
            y1 = iptr->y[0];
            z1 = iptr->z[0];
            x2 = iptr->x[iptr->npts-1];
            y2 = iptr->y[iptr->npts-1];
            z2 = iptr->z[iptr->npts-1];

            isame = SamePointXYZ (x1, y1, z1,
                                  xlast, ylast, zlast);
            if (isame == 1) {
                xlast = x2;
                ylast = y2;
                zlast = z2;
                found = 1;
                iptr->nclose = 1;
                break;
            }

            isame = SamePointXYZ (x2, y2, z2,
                                  xlast, ylast, zlast);
            if (isame == 1) {
                xlast = x1;
                ylast = y1;
                zlast = z1;
                found = 1;
                iptr->nclose = 1;
                break;
            }

        }

        isame = SamePointXYZ (xfirst, yfirst, zfirst,
                              xlast, ylast, zlast);
        if (isame == 1) {
            return 1;
        }

        if (found == 0) {
            return 0;
        }

        ndo++;
        if (ndo > nlist) {
            break;
        }

    }

    return 0;

}



/*-------------------------------------------------------------------------*/

/*
 * Create a new set of intersection lines that exactly match the points in the
 * sealed horizon outlines.
 */
int SealedModel::CreateSealedHorizonIntersects (void)
{
    int          i, j, k, npts;
    _ITag_       *it1 = NULL, *itags = NULL;
    _OUtline_    *outline = NULL;
    _SEaledBorder_  *sbord = NULL;
    CSWTriMeshStruct  *tmesh = NULL;
    double       *x = NULL, *y = NULL, *z = NULL;
    int          n, kk, nseg, npts2, start, istat, nout;
    double       *x2 = NULL, *y2 = NULL, *z2 = NULL;
    int          n1[1000], n2[1000], olines[1000], iborder[1000];


    auto fscope = [&]()
    {
        csw_Free (x2);
    };
    CSWScopeGuard func_scope_guard (fscope);

    
    if (HorizonOutlineList == NULL  ||
        HorizonList == NULL) {
        assert (0);
    }

/*
 * Free existing sealed horizon intersects if needed.
 */
    if (SealedHorizonIntersects != NULL) {
        for (i=0; i<NumSealedHorizonIntersects; i++) {
            csw_Free (SealedHorizonIntersects[i].x);
        }
        csw_Free (SealedHorizonIntersects);
    }
    SealedHorizonIntersects = NULL;
    NumSealedHorizonIntersects = 0;
    MaxSealedHorizonIntersects = 0;

/*
 * Loop through all the sealed horizon outlines and
 * separate them into intersect lines based on the
 * tags associated with each outline point.
 */
    for (i=0; i<NumHorizonList; i++) {

        outline = HorizonOutlineList[i];
        npts = outline->npts;
        itags = outline->itags;
        x = outline->x;
        y = outline->y;
        z = outline->z;

    /*
     * Make sure the first and last outline points have
     * valid line ids.
     */
        if (itags[0].lineid == -1) {
            for (j=1; j<npts-1; j++) {
                if (itags[j].lineid >= 0) {
                    itags[0].lineid = itags[j].lineid;
                    break;
                }
            }
            if (itags[npts-1].lineid >= 0) {
                itags[0].lineid2 = itags[npts-1].lineid;
            }
            itags[0].pointid = -1;
            itags[0].pointid2 = -1;
        }

        if (itags[npts-1].lineid == -1) {
            for (j=npts-2; j>0; j--) {
                if (itags[j].lineid >= 0) {
                    itags[npts-1].lineid = itags[j].lineid;
                    break;
                }
            }
            if (itags[0].lineid >= 0) {
                itags[npts-1].lineid2 = itags[0].lineid;
            }
            itags[npts-1].pointid = -1;
            itags[npts-1].pointid2 = -1;
        }

        if (itags[0].lineid == -1) {
            printf ("Error getting first point for sealed horizon intersects\n");
            assert (0);
        }
        if (itags[npts-1].lineid == -1) {
            printf ("Error getting last point for sealed horizon intersects\n");
            assert (0);
        }

    /*
     * Fill in line ids for invalid intermediate points.
     */
        for (j=1; j<npts-1; j++) {

            if (itags[j].lineid >= 0) {
                continue;
            }

            it1 = itags + j;
            it1->lineid = itags[j-1].lineid;
            it1->lineid2 = -1;
            for (k=j+1; k<npts; k++) {
                if (itags[k].lineid >= 0) {
                    it1->lineid2 = itags[k].lineid;
                    break;
                }
            }

            if (it1->lineid2 == -1) {
                printf
                ("Error getting lineid2 for intermediate sealed horizon"
                 " intersect point\n");
                assert (0);
            }

            it1->pointid = -1;
            it1->pointid2 = -1;

        }

        if (itags[0].lineid == itags[npts-1].lineid) {
            if (itags[1].lineid != itags[0].lineid) {
                if (itags[1].pointid != 0) {
                    itags[0].lineid2 = itags[0].lineid;
                    itags[0].lineid = itags[1].lineid;
                    itags[npts-1].lineid2 = itags[0].lineid;
                }
                else {
                    itags[1].lineid2 = itags[0].lineid;
                    itags[0].lineid2 = -1;
                    itags[npts-1].lineid2 = -1;
                }
            }
        }

    /*
     * Break up this outline into individual intersect lines
     * based on a change in the tag lineid between 2 points.
     */
        start = -1;
        if (itags[0].lineid != itags[npts-1].lineid) {
            start = 0;
        }
        else {
            for (j=1;j<npts-1; j++) {
                if (itags[j].lineid != itags[0].lineid) {
                    start = j;
                    break;
                }
                if (itags[j].lineid2 != -1 &&
                    itags[j].lineid2 != itags[0].lineid) {
                    start = j;
                    break;
                }
            }
        }

        if (start == -1) {
            istat =
              AddSealedHorizonIntersect (
                  x, y, z, npts, itags[0].lineid);
            if (istat == -1) {
                return -1;
            }
            continue;
        }

    /*
     * Find the start and stop indices of each separate intersection
     * line within the outline.
     */
        n=0;
        n1[n] = start;
        olines[n] = itags[start+1].lineid;

        for (j=start+1; j<start+npts-1; j++) {
            k = j;
            if (j >= npts) {
                k = j - npts;
            }
            if (itags[k].lineid2 >= 0) {
                n2[n] = j;
                n++;
                n1[n] = j;
                olines[n] = itags[k].lineid2;
            }
            else if (itags[k].lineid != olines[n]) {
                if (itags[k].lineid2 == itags[n1[n]].lineid) {
                    n2[n] = j;
                }
                else {
                    int ichoice;
                    ichoice = ChooseSharedPoint (
                        x[k-1], y[k-1], z[k-1],
                        x[k], y[k], z[k],
                        olines[n], itags[k].lineid);
                    if (ichoice == 1) {
                        n2[n] = j - 1;
                    }
                    else {
                        n2[n] = j;
                    }
                }
                n++;
                if (n > 999) {
                    n = 999;
                }
                n1[n] = n2[n-1];
                olines[n] = itags[k].lineid;
            }
        }
        n2[n] = start + npts;
        n++;

    /*
     * Put the points between each start and stop into intersection
     * structures and add the structures to the sealed list.
     */
        nseg = 0;
        for (j=0; j<n; j++) {
            npts2 = n2[j] - n1[j] + 1;
            x2 = (double *)csw_Malloc (npts2 * 3 * sizeof(double));
            if (x2 == NULL) {
                return -1;
            }
            y2 = x2 + npts2;
            z2 = y2 + npts2;
            nout = 0;
            for (k=n1[j]; k<=n2[j]; k++) {
                kk = k;
                if (k >= npts) {
                    kk = k - npts;
                }
                if (kk == 0  &&  nout > 0) {
                    continue;
                }
                x2[nout] = x[kk];
                y2[nout] = y[kk];
                z2[nout] = z[kk];
                nout++;
            }
            istat =
              AddSealedHorizonIntersect (
                  x2, y2, z2, nout, olines[j]);
            csw_Free (x2);
            x2 = y2 = z2 = NULL;
            npts2 = 0;
            if (istat == -1) {
                return -1;
            }
            iborder[nseg] = NumSealedHorizonIntersects - 1;
            nseg++;
        }

    /*
     * Make the sealed border structure for this outline.
     */
        tmesh = HorizonList + i;
        sbord = &(tmesh->sealed_border);
        sbord->nlist = nseg;
        sbord->surfid = tmesh->id;
        for (j=0; j<nseg; j++) {
            sbord->intersection_id_list[j] = iborder[j];
            sbord->direction_list[j] = 1;
        }

    }  /* end of i loop through surface outlines */

    return 1;

}





/*-----------------------------------------------------------------------*/

/*
 * Add a new sealed horizon intersection line to the list.
 */
int SealedModel::AddSealedHorizonIntersect (
    double         *x,
    double         *y,
    double         *z,
    int            npts,
    int            orig_lineid)
{
    _INtersectionLine_   *iptr = NULL;
    double         *x2 = NULL, *y2 = NULL, *z2 = NULL;

    double    *xresamp = NULL, *yresamp = NULL, *zresamp = NULL;
    int       nresamp, maxresamp, istat;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (xresamp);
        if (bsuccess == false) {
            FaultList = NULL;
            HorizonList = NULL;
            NumFaultList = 0;
            NumHorizonList = 0;
            MaxFaultList = 0;
            MaxHorizonList = 0;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (IntersectionLines == NULL) {
        assert (0);
    }

    if (SealedHorizonIntersects == NULL  ||
        NumSealedHorizonIntersects >= MaxSealedHorizonIntersects) {
        MaxSealedHorizonIntersects += 20;
        SealedHorizonIntersects = (_INtersectionLine_ *)csw_Realloc
            (SealedHorizonIntersects,
             MaxSealedHorizonIntersects * sizeof(_INtersectionLine_));
    }

    if (SealedHorizonIntersects == NULL) {
        return -1;
    }

/*
 * The sealed horizon intersect line needs resampling because small
 * segments may have been created near the endpoints when the
 * unsealed lines were connected into a polygon.
 */
    maxresamp = npts * 10;
    if (maxresamp < 1000) maxresamp = 1000;
    xresamp = (double *)csw_Malloc (3 * maxresamp * sizeof(double));
    if (xresamp == NULL) {
        return -1;
    }
    yresamp = xresamp + maxresamp;
    zresamp = yresamp + maxresamp;
    istat =
      grd_api_obj.grd_ResampleXYZLine (
        x, y, z, npts,
        averageSpacing,
        xresamp, yresamp, zresamp, &nresamp,
        maxresamp);
    if (istat == -1) {
        return -1;
    }

    npts = nresamp;

    iptr = SealedHorizonIntersects + NumSealedHorizonIntersects;
    memset (iptr, 0, sizeof(_INtersectionLine_));

    x2 = (double *)csw_Malloc (npts * 3 * sizeof(double));
    if (x2 == NULL) {
        return -1;
    }
    y2 = x2 + npts;
    z2 = y2 + npts;

    memcpy (x2, xresamp, npts * sizeof(double));
    memcpy (y2, yresamp, npts * sizeof(double));
    memcpy (z2, zresamp, npts * sizeof(double));

    iptr->x = x2;
    iptr->y = y2;
    iptr->z = z2;
    iptr->npts = npts;

    int do_write = csw_GetDoWrite ();
    if (do_write) {
        char    fname[100];
        sprintf (fname, "seal_hor_int_%d.xyz", NumSealedHorizonIntersects);
        int nc = 1;
        grd_api_obj.grd_WriteLines (
          x2, y2, z2,
          1, &nc, &npts,
          fname);
    }

    iptr->embed_flag = 1;
    iptr->nclose = 0;

    if (orig_lineid >= NumIntersectionLines) {
        iptr->surf1 = orig_lineid;
        iptr->surf2 = orig_lineid;
    }
    else {
        iptr->surf1 = IntersectionLines[orig_lineid].surf1; /*lint !e662*/
        iptr->surf2 = IntersectionLines[orig_lineid].surf2; /*lint !e662*/
    }

    NumSealedHorizonIntersects++;

    bsuccess = true;

    return 1;

}



/*-----------------------------------------------------------------------*/

/*
 * Add a new sealed top or bottom intersection line to the list.
 */
int SealedModel::AddSealedTopAndBottomIntersect (
    double         *x,
    double         *y,
    double         *z,
    int            npts,
    int            lineid)
{
    _INtersectionLine_   *iptr = NULL;
    double         *x2 = NULL, *y2 = NULL, *z2 = NULL;

    double    *xresamp = NULL, *yresamp = NULL, *zresamp = NULL;
    int       nresamp, maxresamp, istat;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (xresamp);
        if (bsuccess == false) {
            FaultList = NULL;
            HorizonList = NULL;
            NumFaultList = 0;
            NumHorizonList = 0;
            MaxFaultList = 0;
            MaxHorizonList = 0;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (IntersectionLines == NULL) {
        assert (0);
    }

    if (SealedTopAndBottomIntersects == NULL  ||
        NumSealedTopAndBottomIntersects >= MaxSealedTopAndBottomIntersects) {
        MaxSealedTopAndBottomIntersects += 20;
        SealedTopAndBottomIntersects = (_INtersectionLine_ *)csw_Realloc
            (SealedTopAndBottomIntersects,
             MaxSealedTopAndBottomIntersects * sizeof(_INtersectionLine_));
    }

    if (SealedTopAndBottomIntersects == NULL) {
        return -1;
    }

/*
 * The sealed top or bottom line needs resampling because small
 * segments may have been created near the endpoints when the
 * unsealed lines were connected into a polygon.
 */
    maxresamp = npts * 10;
    if (maxresamp < 1000) maxresamp = 1000;
    xresamp = (double *)csw_Malloc (3 * maxresamp * sizeof(double));
    if (xresamp == NULL) {
        return -1;
    }
    yresamp = xresamp + maxresamp;
    zresamp = yresamp + maxresamp;
    istat =
      grd_api_obj.grd_ResampleXYZLine (
        x, y, z, npts,
        averageSpacing,
        xresamp, yresamp, zresamp, &nresamp,
        maxresamp);
    if (istat == -1) {
        return -1;
    }

    npts = nresamp;

    iptr = SealedTopAndBottomIntersects + NumSealedTopAndBottomIntersects;
    memset (iptr, 0, sizeof(_INtersectionLine_));

    x2 = (double *)csw_Malloc (npts * 3 * sizeof(double));
    if (x2 == NULL) {
        return -1;
    }
    y2 = x2 + npts;
    z2 = y2 + npts;

    memcpy (x2, xresamp, npts * sizeof(double));
    memcpy (y2, yresamp, npts * sizeof(double));
    memcpy (z2, zresamp, npts * sizeof(double));

    iptr->x = x2;
    iptr->y = y2;
    iptr->z = z2;
    iptr->npts = npts;

    iptr->embed_flag = 1;
    iptr->nclose = 0;

    iptr->surf1 = IntersectionLines[lineid].surf1;
    iptr->surf2 = IntersectionLines[lineid].surf2;

    NumSealedTopAndBottomIntersects++;

    bsuccess = true;

    return 1;

}




/*-----------------------------------------------------------------------------------*/

_OUtline_ *SealedModel::BuildOutlineFromSealedBorder (_SEaledBorder_ *sbord)
{
    int             i, j, idir, start;
    int             npts, npts2, ntot, last = -1;
    double          xlast, ylast, zlast;
    double          *x = NULL, *y = NULL, *z = NULL,
                    *x2 = NULL, *y2 = NULL, *z2 = NULL;
    _INtersectionLine_  *iptr = NULL;
    _OUtline_           *outline = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (x2);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (SealedHorizonIntersects == NULL) {
        return NULL;
    }

    ntot = 0;
    for (i=0; i<sbord->nlist; i++) {
        iptr = SealedHorizonIntersects + sbord->intersection_id_list[i];
        ntot += iptr->npts;
    }

    x2 = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (x2 == NULL) {
        return NULL;
    }
    y2 = x2 + ntot;
    z2 = y2 + ntot;

    npts2 = 0;
    xlast = ylast = zlast = 1.e30;
    for (i=0; i<sbord->nlist; i++) {
        iptr = SealedHorizonIntersects + sbord->intersection_id_list[i];
        x = iptr->x;
        y = iptr->y;
        z = iptr->z;
        npts = iptr->npts;
        idir = sbord->direction_list[i];

        if (idir == 1) {
            start = 0;
            if (i > 0) {
                start = 1;
                if (xlast > 1.e20) {
                    assert (0);
                }
                x[0] = xlast;
                y[0] = ylast;
                z[0] = zlast;
            }
            for (j=start; j<npts; j++) {
                x2[npts2] = x[j];
                y2[npts2] = y[j];
                z2[npts2] = z[j];
                npts2++;
            }
            last = npts-1;
            xlast = x[npts-1];
            ylast = y[npts-1];
            zlast = z[npts-1];
        }
        else {
            start = npts - 1;
            if (i > 0) {
                start = npts - 2;
                if (xlast > 1.e20) {
                    assert (0);
                }
                x[npts-1] = xlast;
                y[npts-1] = ylast;
                z[npts-1] = zlast;
            }
            for (j=start; j>=0; j--) {
                x2[npts2] = x[j];
                y2[npts2] = y[j];
                z2[npts2] = z[j];
                npts2++;
            }
            last = 0;
            xlast = x[0];
            ylast = y[0];
            zlast = z[0];
        }
    }

    if (npts2 < 2) {
        return NULL;
    }

  /*
   * Make sure the outline is exactly closed and the last
   * sealed horizon intersect point is also exactly closed.
   */
    x2[npts2-1] = x2[0];
    y2[npts2-1] = y2[0];
    z2[npts2-1] = z2[0];

    if (x != NULL  &&  y != NULL  &&  z != NULL) {
        x[last] = x2[0];
        y[last] = y2[0];
        z[last] = z2[0];
    }

  /*
   * Allocate and fill in an outline structure.
   */
    outline = (_OUtline_ *)csw_Calloc (sizeof(_OUtline_));
    if (outline == NULL) {
        return NULL;
    }

    outline->x = x2;
    outline->y = y2;
    outline->z = z2;
    outline->npts = npts2;
    outline->surfid = sbord->surfid;

    bsuccess = true;

    return outline;

}


/*----------------------------------------------------------------*/

/*
 * Fix up crossing intersection lines on each surface.
 */
int SealedModel::FixupCrossingIntersects (void)
{
    int                i, istat;
    CSWTriMeshStruct   *tmesh;

    if (FaultList != NULL) {
        for (i=0; i<NumFaultList; i++) {
            tmesh = FaultList + i;
            istat =
              UpdateIntersectionsForCrossing (tmesh);
            if (istat == -1) {
                FaultList = NULL;
                HorizonList = NULL;
                NumFaultList = 0;
                NumHorizonList = 0;
                MaxFaultList = 0;
                MaxHorizonList = 0;
                return -1;
            }
        }
    }

    if (simOutputFlag) {
        istat =
          UpdateIntersectionsForCrossing (NorthBoundarySurface);
        if (istat == -1) {
            FaultList = NULL;
            HorizonList = NULL;
            NumFaultList = 0;
            NumHorizonList = 0;
            MaxFaultList = 0;
            MaxHorizonList = 0;
            return -1;
        }
        istat =
          UpdateIntersectionsForCrossing (SouthBoundarySurface);
        if (istat == -1) {
            FaultList = NULL;
            HorizonList = NULL;
            NumFaultList = 0;
            NumHorizonList = 0;
            MaxFaultList = 0;
            MaxHorizonList = 0;
            return -1;
        }
        istat =
          UpdateIntersectionsForCrossing (EastBoundarySurface);
        if (istat == -1) {
            FaultList = NULL;
            HorizonList = NULL;
            NumFaultList = 0;
            NumHorizonList = 0;
            MaxFaultList = 0;
            MaxHorizonList = 0;
            return -1;
        }
        istat =
          UpdateIntersectionsForCrossing (WestBoundarySurface);
        if (istat == -1) {
            FaultList = NULL;
            HorizonList = NULL;
            NumFaultList = 0;
            NumHorizonList = 0;
            MaxFaultList = 0;
            MaxHorizonList = 0;
            return -1;
        }
    }

    return 1;

}




/*----------------------------------------------------------------------------*/

/*
 * Return 1 if the specified point is nearly identical to a point in
 * the specified point lists.  Return zero if the specified point is
 * not nearly identical.  The modelTiny value is used to determine
 * identical locations.  This is set to 1 part in 100000 of the average
 * xy model extents by default.
 */
int SealedModel::CheckForPointInList (
    double     x,
    double     y,
    double     *xc,
    double     *yc,
    XYIndex2D  *xyindex,
    double     *xnew,
    double     *ynew)
{
    int        list[100], nlist, istat;

    *xnew = 1.e30;
    *ynew = 1.e30;

    istat =
      xyindex->GetClosePoints (
        x, y,
        list, &nlist,
        100);

    if (istat != 1  ||  nlist < 1) {
        return 0;
    }

    double    dx, dy, dist, xt, yt;
    int       i;

    for (i=0; i<nlist; i++) {
        xt = xc[list[i]];
        yt = yc[list[i]];
        dx = xt - x;
        dy = yt - y;
        dist = dx * dx + dy * dy;
        dist = sqrt (dist);
        if (dist <= modelTiny) {
            *xnew = xt;
            *ynew = yt;
            return 1;
        }
    }

    return 0;

}


/*-----------------------------------------------------------------*/

/*
 * Choose one of two points as the point shared by
 * the two intersection lines.
 */
int SealedModel::ChooseSharedPoint (
    double x1,
    double y1,
    double z1,
    double x2,
    double y2,
    double z2,
    int    iline1,
    int    iline2)
{
    _INtersectionLine_    *ip1, *ip2, *iptr, *ipcommon;
    int                   isurf1, isurf2, i1, i2;

    if (IntersectionLines == NULL) {
        return -1;
    }

/*
 * If either lineid is the origional polygon border,
 * always use the second point.  This was set up in
 * an earlier function.
 */
    if (iline1 >= NumIntersectionLines  ||  iline2 >= NumIntersectionLines) {
      return 2;
    }

    ip1 = IntersectionLines + iline1; /*lint !e662*/
    ip2 = IntersectionLines + iline2; /*lint !e662*/

    isurf1 = -1;
    isurf2 = -1;

    i1 = ip1->surf1;
    i2 = ip1->surf2;
    if (ip2->surf1 == i1) {
        isurf1 = i2;
        isurf2 = ip2->surf2;
    }
    else if (ip2->surf1 == i2) {
        isurf1 = i1;
        isurf2 = ip2->surf2;
    }
    else if (ip2->surf2 == i1) {
        isurf1 = i2;
        isurf2 = ip2->surf1;
    }
    else if (ip2->surf2 == i2) {
        isurf1 = i1;
        isurf2 = ip2->surf1;
    }

    if (isurf1 == -1  ||  isurf2 == -1) {
        assert (0);
    }

/*
 * The intersection lines come together at a boundary corner.
 */
    if (isurf1 >= _BOUNDARY_ID_BASE_  &&
        isurf2 >= _BOUNDARY_ID_BASE_) {

        double    xcorner, ycorner;
        double    dx, dy, dist1, dist2;
        int       iclose;

        xcorner = 1.e30;
        ycorner = 1.e30;
        iclose = -1;

        if (isurf1 == _BOUNDARY_ID_BASE_ + _NORTH_ID_) {
            ycorner = padYNorth;
        }
        else if (isurf1 == _BOUNDARY_ID_BASE_ + _SOUTH_ID_) {
            ycorner = padYSouth;
        }
        else if (isurf1 == _BOUNDARY_ID_BASE_ + _EAST_ID_) {
            xcorner = padXEast;
        }
        else if (isurf1 == _BOUNDARY_ID_BASE_ + _WEST_ID_) {
            xcorner = padXWest;
        }

        if (isurf2 == _BOUNDARY_ID_BASE_ + _NORTH_ID_) {
            ycorner = padYNorth;
        }
        else if (isurf2 == _BOUNDARY_ID_BASE_ + _SOUTH_ID_) {
            ycorner = padYSouth;
        }
        else if (isurf2 == _BOUNDARY_ID_BASE_ + _EAST_ID_) {
            xcorner = padXEast;
        }
        else if (isurf2 == _BOUNDARY_ID_BASE_ + _WEST_ID_) {
            xcorner = padXWest;
        }

        if (xcorner > 1.e20  ||  ycorner > 1.e20) {
            assert (0);
        }

        dx = x1 - xcorner;
        dy = y1 - ycorner;
        dist1 = dx * dx + dy * dy;
        dist1 = sqrt (dist1);
        iclose = 1;

        dx = x2 - xcorner;
        dy = y2 - ycorner;
        dist2 = dx * dx + dy * dy;
        dist2 = sqrt (dist2);
        if (dist2 < dist1) {
            iclose = 2;
        }

        return iclose;
    }

/*
 * If the intersection lines do not come together at a boundary
 * corner they must come together at a third intersection line.
 * This third line is shared by sisurf1 and isurf2.
 */
    int        i;

    ipcommon = NULL;
    for (i=0; i<NumIntersectionLines; i++) {
        iptr = IntersectionLines + i;
        if (iptr->surf1 == isurf1  &&
            iptr->surf2 == isurf2) {
            ipcommon = iptr;
            break;
        }
        if (iptr->surf1 == isurf2  &&
            iptr->surf2 == isurf1) {
            ipcommon = iptr;
            break;
        }
    }

    if (ipcommon == NULL) {
        assert (0);
    }

/*
 * Find the shortest distance between each point and the
 * ipcommon line.
 */
    double      dist1, dist2;
    int         iclose;

    dist1 = IlineDistance (ipcommon, x1, y1, z1);
    iclose = 1;
    dist2 = IlineDistance (ipcommon, x2, y2, z2);
    if (dist2 < dist1) {
        iclose = 2;
    }

    return iclose;

}



/*----------------------------------------------------------------------*/

/*
 * Return the distance from the specified point to the closest
 * point on the specified intersection line.
 */
double SealedModel::IlineDistance (
    _INtersectionLine_ *iptr,
    double x, double y, double z)
{
    int        i, npts;
    double     dx, dy, dz, dist, dmin;
    double     *xa, *ya, *za;

    xa = iptr->x;
    ya = iptr->y;
    za = iptr->z;
    npts = iptr->npts;

    dmin = 1.e30;
    for (i=0; i<npts; i++) {
        dx = xa[i] - x;
        dy = ya[i] - y;
        dz = za[i] - z;
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);
        if (dist < dmin) {
            dmin = dist;
        }
    }

    return dmin;

}


/*-----------------------------------------------------------------*/

/*
 * Find the intersection line that is not a sealed horizon intersect
 * and that goes through the specified end point number on the specified
 * sealed horizon intersect.
 *
 * This is private.
 */
int SealedModel::FindThirdIntersectLine (
    int    ipoint1,
    int    iline1)
{
    _INtersectionLine_    *ip1, *ip2, *iptr;
    int                   isurf1, isurf2, i1, i2;
    int                   iline2, ipoint2, i, npts;
    double                xt, yt, zt, dx, dy, dz, dist;
    int                   is1, is2;

    if (IntersectionLines == NULL) {
        return -1;
    }
    if (SealedHorizonIntersects == NULL) {
        return -1;
    }

    ip1 = SealedHorizonIntersects + iline1;

    xt = ip1->x[ipoint1];
    yt = ip1->y[ipoint1];
    zt = ip1->z[ipoint1];

    is1 = ip1->surf1;
    if (is1 >= _FAULT_ID_BASE_) {
        is1 = ip1->surf2;
    }

    iline2 = -1;
    ipoint2 = -1;
    for (i=0; i<NumSealedHorizonIntersects; i++) {
        if (i == iline1) {
            continue;
        }
        ip2 = SealedHorizonIntersects + i;
        is2 = ip2->surf1;
        if (is2 >= _FAULT_ID_BASE_) {
            is2 = ip2->surf2;
        }
        if (is1 != is2) {
            continue;
        }
        dx = ip2->x[0] - xt;
        dy = ip2->y[0] - yt;
        dz = ip2->z[0] - zt;
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);
        if (dist <= modelTiny) {
            ipoint2 = 0;
            iline2 = i;
            break;
        }
        npts = ip2->npts;
        dx = ip2->x[npts-1] - xt;
        dy = ip2->y[npts-1] - yt;
        dz = ip2->z[npts-1] - zt;
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);
        if (dist <= modelTiny) {
            ipoint2 = npts-1;
            iline2 = i;
            break;
        }
    }

    if (iline2 == -1  ||  ipoint2 == -1) {
        return -1;
    }

    ip2 = SealedHorizonIntersects + iline2;

    isurf1 = -1;
    isurf2 = -1;

    i1 = ip1->surf1;
    i2 = ip1->surf2;
    if (ip2->surf1 == i1) {
        isurf1 = i2;
        isurf2 = ip2->surf2;
    }
    else if (ip2->surf1 == i2) {
        isurf1 = i1;
        isurf2 = ip2->surf2;
    }
    else if (ip2->surf2 == i1) {
        isurf1 = i2;
        isurf2 = ip2->surf1;
    }
    else if (ip2->surf2 == i2) {
        isurf1 = i1;
        isurf2 = ip2->surf1;
    }

    if (isurf1 == -1  ||  isurf2 == -1) {
        return -1;
    }

/*
 * The intersection lines come together at a boundary corner.
 */
    if (isurf1 >= _BOUNDARY_ID_BASE_  &&
        isurf2 >= _BOUNDARY_ID_BASE_) {

        return -1;

    }

/*
 * If the intersection lines do not come together at a boundary
 * corner they must come together at a third intersection line.
 * This third line is shared by isurf1 and isurf2.
 */
    int    lineout;
    lineout = -1;
    for (i=0; i<NumIntersectionLines; i++) {
        iptr = IntersectionLines + i;
        if (iptr->surf1 == isurf1  &&
            iptr->surf2 == isurf2) {
            lineout = i;
            break;
        }
        if (iptr->surf1 == isurf2  &&
            iptr->surf2 == isurf1) {
            lineout = i;
            break;
        }
    }

    return lineout;

}



/*-------------------------------------------------------------------------*/

/*
 * Create a set of fault to fault and fault to boundary intersections
 * that exactly tie in to the sealed horizon intersects.  These are put
 * into the WorkFaultIntersects list temporarily.  Once the tie in points
 * have been put in to this work list, the work list is processed and
 * the SealedFaultIntersects list is created.
 */
int SealedModel::CreateSealedFaultIntersects (void)
{
    int          i, iline, istat, npts;
    _INtersectionLine_    *iptr;

    if (SealedHorizonIntersects == NULL) {
        return -1;
    }

    istat = SpliceFaultIntersectionLines ();
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<NumSealedHorizonIntersects; i++) {
        iptr = SealedHorizonIntersects + i;
        if (iptr->surf1 == 1000001) {
            continue;
        }
        iline =
          FindThirdIntersectLine (0, i);
        if (iline >= 0) {
            istat =
              ProcessThirdLine (0, i, iline);
            if (istat == -1) {
                return -1;
            }
        }

        npts = iptr->npts;
        iline =
          FindThirdIntersectLine (npts-1, i);
        if (iline >= 0) {
            istat =
              ProcessThirdLine (npts-1, i, iline);
            if (istat == -1) {
                return -1;
            }
        }
    }

    ProcessFaultWorkList ();

    return 1;

}



/*--------------------------------------------------------------------------------*/

//   Study this function carefully.  I think there are many demons here that need fixing.

/*--------------------------------------------------------------------------------*/

int SealedModel::ProcessThirdLine (int ipoint1,
                                   int iline1,
                                   int iline3)
{
    _INtersectionLine_  *ip1 = NULL, *ip3 = NULL, *iptr = NULL, *ipw = NULL;
    int                 isurf1, isurf2, i, n, istat;
    double              xt, yt, zt, dx, dy, dz;
    double              dist;


    if (SealedHorizonIntersects == NULL) {
        return -1;
    }

    if (IntersectionLines == NULL) {
        return -1;
    }

/*
 * Get the coordinates of the point of the sealed horizon line.
 */
    ip1 = SealedHorizonIntersects + iline1;
    xt = ip1->x[ipoint1];
    yt = ip1->y[ipoint1];
    zt = ip1->z[ipoint1];

/*
 * Get the horizons shared by the raw third intersection line.
 */
    ip3 = IntersectionLines + iline3;
    isurf1 = ip3->surf1;
    isurf2 = ip3->surf2;

    ipw = NULL;

/*
 * See if a work line sharing these surfaces already exists.
 */
    if (WorkFaultIntersects != NULL) {
        for (i=0; i<NumWorkFaultIntersects; i++) {
            iptr = WorkFaultIntersects[i];
            if (iptr->surf1 == isurf1  &&
                iptr->surf2 == isurf2) {
                ipw = iptr;
                break;
            }
            if (iptr->surf1 == isurf2  &&
                iptr->surf2 == isurf1) {
                ipw = iptr;
                break;
            }
        }
    }

/*
 * If no work line sharing these surfaces exists, find a line from the IntersectionLines
 * list and copy it into a new work line.
 */
    _INtersectionLine_ *iptmp;
    int                *flags;

    iptmp = NULL;
    if (ipw == NULL) {
        for (i=0; i<NumIntersectionLines; i++) {
            iptr = IntersectionLines + i;
            if ((iptr->surf1 == isurf1  &&  iptr->surf2 == isurf2)  ||
                (iptr->surf1 == isurf2  &&  iptr->surf2 == isurf1)) {
                ipw = (_INtersectionLine_ *)csw_Calloc (sizeof(_INtersectionLine_));
                if (ipw == NULL) {
                    return -1;
                }
                CopyIline (iptr, ipw);
                istat = AddWorkFaultIntersect (ipw);
                if (istat == -1) {
                    return -1;
                }
                iptmp = iptr;
            /*
             * Make the xyz arrays larger by 1000 points each.  This means
             * that 1000 horizons can intersect a fault, which should be more
             * than enough.
             */
                double      *xa = NULL, *ya = NULL, *za = NULL;
                int         nmax;

                nmax = ipw->npts + 1000;
                xa = (double *)csw_Malloc (nmax * 4 * sizeof(double));
                if (xa == NULL) {
                    return -1;
                }
                ya = xa + nmax;
                za = ya + nmax;
                flags = (int *)(za + nmax);
                memset (flags, 0, nmax * sizeof(int));
                memcpy (xa, ipw->x, ipw->npts * sizeof(double));
                memcpy (ya, ipw->y, ipw->npts * sizeof(double));
                memcpy (za, ipw->z, ipw->npts * sizeof(double));

                csw_Free (ipw->x);
                ipw->x = xa;
                ipw->y = ya;
                ipw->z = za;
                ipw->flags = flags;
                ipw->surf1 = isurf1;
                ipw->surf2 = isurf2;

                break;
            }
        }
    }
    else {
        iptmp = ipw;
    }

    if (iptmp == NULL  ||  ipw == NULL) {
        return -1;
    }

/*
 * See if the point already is in the work line.
 */
    for (i=0; i<ipw->npts; i++) {
        dx = xt - ipw->x[i];
        dy = yt - ipw->y[i];
        dz = zt - ipw->z[i];
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);
        if (dist <= modelTiny) {
            ipw->x[i] = xt;
            ipw->y[i] = yt;
            ipw->z[i] = zt;
            return 1;
        }
    }

/*
 * If the point is not on a work line segment, then
 * do nothing.
 */
    double    x1, y1, z1, x2, y2, z2, mtiny, atmp;
    int       good = 0;

    mtiny = modelTiny / 10.0;

    for (i=0; i<ipw->npts-1; i++) {

        x1 = ipw->x[i];
        y1 = ipw->y[i];
        z1 = ipw->z[i];
        x2 = ipw->x[i+1];
        y2 = ipw->y[i+1];
        z2 = ipw->z[i+1];

        dx = x2 - x1;
        if (dx < 0.0) dx = -dx;
        if (dx <= mtiny) {
           atmp = (x1 + x2) / 2.0;
           x1 = atmp - mtiny / 2.0;
           x2 = atmp + mtiny / 2.0;
        }
        dy = y2 - y1;
        if (dy < 0.0) dy = -dy;
        if (dy <= mtiny) {
           atmp = (y1 + y2) / 2.0;
           y1 = atmp - mtiny / 2.0;
           y2 = atmp + mtiny / 2.0;
        }
        dz = z2 - z1;
        if (dz < 0.0) dz = -dz;
        if (dz <= mtiny) {
           atmp = (z1 + z2) / 2.0;
           z1 = atmp - mtiny / 2.0;
           z2 = atmp + mtiny / 2.0;
        }

        if ((x1 - xt) * (xt - x2) < 0.0) {
            continue;
        }
        if ((y1 - yt) * (yt - y2) < 0.0) {
            continue;
        }
        if ((z1 - zt) * (zt - z2) < 0.0) {
            continue;
        }

        good = 1;
        break;

    }

    if (good == 0) {
        return 1;
    }


/*
 * Insert the point into the work line.  This is done by moving the closest
 * point (if it has not already been moved) to the exact xt, yt, zt coordinates.
 * The flag value is then set to show that the point cannot be moved again.
 * Only the interior points can be moved.  If an end point is moved, it will
 * or at least may, introduce a gap in the fault outline.
 */
    double           dmin;
    int              imin;

    dmin = 1.e30;
    imin = -1;
    for (i=1; i<ipw->npts-1; i++) {
        dx = xt - ipw->x[i];
        dy = yt - ipw->y[i];
        dz = zt - ipw->z[i];
        dist = dx * dx + dy * dy + dz * dz;
        if (dist < dmin) {
            dmin = dist;
            imin = i;
        }
    }

    if (imin < 0) {
        return -1;
    }

/*
 * If the closest point has not been used, move it to
 * the xt, yt, zt location and lock it in place.
 */
    if (ipw->flags[imin] == 0) {
        ipw->x[imin] = xt;
        ipw->y[imin] = yt;
        ipw->z[imin] = zt;
        ipw->flags[imin] = (char)1;
        return 1;
    }

/*
 * Move a neighboring point to the xt, yt, zt location if possible.
 */
    if (imin > 0  &&  ipw->flags[imin-1] == 0) {
        istat = BetweenPointsXYZ (xt, yt, zt,
                                  ipw->x[imin-1], ipw->y[imin-1], ipw->z[imin-1],
                                  ipw->x[imin], ipw->y[imin], ipw->z[imin]);
        if (istat == 1) {
            ipw->x[imin-1] = xt;
            ipw->y[imin-1] = yt;
            ipw->z[imin-1] = zt;
            ipw->flags[imin-1] = 1;
            return 1;
        }
    }
    else if (imin < ipw->npts-1  &&  ipw->flags[imin+1] == 0) {
        istat = BetweenPointsXYZ (xt, yt, zt,
                                  ipw->x[imin+1], ipw->y[imin+1], ipw->z[imin+1],
                                  ipw->x[imin], ipw->y[imin], ipw->z[imin]);
        if (istat == 1) {
            ipw->x[imin+1] = xt;
            ipw->y[imin+1] = yt;
            ipw->z[imin+1] = zt;
            ipw->flags[imin+1] = 1;
            return 1;
        }
    }

/*
 * Insert the xy, yt, zt point if no close point can be moved.
 */
    if (imin > 0) {
        istat = BetweenPointsXYZ (xt, yt, zt,
                                  ipw->x[imin-1], ipw->y[imin-1], ipw->z[imin-1],
                                  ipw->x[imin], ipw->y[imin], ipw->z[imin]);
        if (istat == 1) {
            istat =
            InsertPointInLine (ipw->x, ipw->y, ipw->z, ipw->flags, ipw->npts,
                               imin-1, 1, xt, yt, zt);
            if (istat == 1) {
                ipw->npts++;
                return 1;
            }
        }
    }

    n = ipw->npts - 1;
    if (imin < n) {
        istat = BetweenPointsXYZ (xt, yt, zt,
                                  ipw->x[imin+1], ipw->y[imin+1], ipw->z[imin+1],
                                  ipw->x[imin], ipw->y[imin], ipw->z[imin]);
        if (istat == 1) {
            istat =
            InsertPointInLine (ipw->x, ipw->y, ipw->z, ipw->flags, ipw->npts,
                               imin, 1, xt, yt, zt);
            if (istat == 1) {
                ipw->npts++;
                return 1;
            }
        }
    }

    return 1;

}


/*--------------------------------------------------------------------------------*/

/*
 * If the any of the 3 coordinates is inside its specified
 * range, return 1.  If none are inside return zero.
 */

int SealedModel::BetweenPointsXYZ (
    double x, double y, double z,
    double x1, double y1, double z1,
    double x2, double y2, double z2)
{
    double          dx, dy, dz, davg;
    int             count, total;

    dx = x2 - x1;
    if (dx < 0) dx = -dx;
    dy = y2 - y1;
    if (dy < 0) dy = -dy;
    dz = z2 - z1;
    if (dz < 0) dz = -dz;
    davg = (dx + dy + dz) / 3.0;

    count = 0;
    total = 3;

    if (dx > .1 * davg) {
        if ((x1-x)*(x-x2) >= 0.0) {
            return 1;
        }
    }
    else {
        total--;
    }

    if (dy > .1 * davg) {
        if ((y1-y)*(y-y2) >= 0.0) {
            return 1;
        }
    }
    else {
        total--;
    }

    if (dz > .1 * davg) {
        if ((z1-z)*(z-z2) >= 0.0) {
            return 1;
        }
    }
    else {
        total--;
    }

    if (total == 0) {
        return 0;
    }

    if (count >= total) {
        return 1;
    }

    return 0;

}


/*--------------------------------------------------------------------------------*/

int SealedModel::CopyIline (
    _INtersectionLine_ *old_line,
    _INtersectionLine_ *new_line)
{
    double    *xa = NULL, *ya = NULL, *za = NULL;
    int       nmax;

    memcpy (new_line, old_line, sizeof(_INtersectionLine_));
    nmax = old_line->npts;

    xa = (double *)csw_Malloc (nmax * 3 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + nmax;
    za = ya + nmax;

    memcpy (xa, old_line->x, nmax * sizeof(double));
    memcpy (ya, old_line->y, nmax * sizeof(double));
    memcpy (za, old_line->z, nmax * sizeof(double));

    new_line->x = xa;
    new_line->y = ya;
    new_line->z = za;

    return 1;

}



/*--------------------------------------------------------------------------------*/

int SealedModel::AddWorkFaultIntersect (_INtersectionLine_ *ipw)
{
    int               ilast;

    if (WorkFaultIntersects == NULL) {
        NumWorkFaultIntersects = 0;
        MaxWorkFaultIntersects = 0;
    }

    if (NumWorkFaultIntersects >= MaxWorkFaultIntersects  ||
        WorkFaultIntersects == NULL) {
        ilast = MaxWorkFaultIntersects;
        MaxWorkFaultIntersects += 20;
        WorkFaultIntersects = (_INtersectionLine_ **)csw_Realloc (
            WorkFaultIntersects,
            MaxWorkFaultIntersects * sizeof(_INtersectionLine_ *));
        if (WorkFaultIntersects != NULL) {
            memset (WorkFaultIntersects + ilast,
                    0,
                    20 * sizeof(_INtersectionLine_ *));
        }
    }

    if (WorkFaultIntersects == NULL) {
        return -1;
    }

    WorkFaultIntersects[NumWorkFaultIntersects] = ipw;
    NumWorkFaultIntersects++;

    return 1;

}



/*--------------------------------------------------------------------------------*/
void SealedModel::FreeWorkFaultIntersects (void)
{
    int                 i;
    _INtersectionLine_  *iptr;

    if (WorkFaultIntersects == NULL) {
        return;
    }

    for (i=0; i<NumWorkFaultIntersects; i++) {
        iptr = WorkFaultIntersects[i];
        if (iptr != NULL) {
            csw_Free (iptr->x);
        }
        csw_Free (iptr);
    }

    csw_Free (WorkFaultIntersects);
    WorkFaultIntersects = NULL;
    NumWorkFaultIntersects = 0;
    MaxWorkFaultIntersects = 0;

    return;

}


/*--------------------------------------------------------------------------------*/

int SealedModel::ProcessFaultWorkList (void)
{
    int                 i, j, istat;
    _INtersectionLine_  *iptr, *jptr;
    _INtersectionLine_  *ilist[1000];

    if (WorkFaultIntersects == NULL) {
        return -1;
    }

    if (IntersectionLines == NULL) {
        return -1;
    }

    csw_Free (SealedFaultIntersects);
    SealedFaultIntersects = NULL;
    NumSealedFaultIntersects = 0;
    MaxSealedFaultIntersects = 0;

    int do_write = csw_GetDoWrite ();
    if (do_write) {
        char   fname[200];
        sprintf (fname, "workfaultint.xyz");
        WriteIlinesToFile (
            WorkFaultIntersects,
            NumWorkFaultIntersects,
            fname);
    }

  /*
   * Add all the work fault intersects as sealed fault intersects.
   * These are intersections between faults and vertical boundaries,
   * that have had the end points of sealed horizon intersects exactly
   * inserted into them.
   */
    for (i=0; i<NumWorkFaultIntersects; i++) {
        iptr = WorkFaultIntersects[i];
        istat =
          AddSealedFaultIntersect (
              iptr->x, iptr->y, iptr->z, iptr->npts,
              iptr->surf1, iptr->surf2);
        if (istat == -1) {
            FreeWorkFaultIntersects ();
            return -1;
        }
    }

  /*
   * Add any raw intersect lines that use faults and boundaries, which
   * were not added from the work fault intersects.
   */
    int         id1, id2;
    int         isame, isfault, otherid;

    for (i=0; i<NumIntersectionLines; i++) {

        iptr = IntersectionLines + i;
        isfault = 0;
        otherid = 0;
        if (iptr->surf1 >= _FAULT_ID_BASE_  &&
            iptr->surf1 < _BOUNDARY_ID_BASE_) {
            isfault = 1;
            otherid = iptr->surf2;
        }
        if (iptr->surf2 >= _FAULT_ID_BASE_  &&
            iptr->surf2 < _BOUNDARY_ID_BASE_) {
            isfault = 1;
            otherid = iptr->surf1;
        }
        if (isfault == 0) {
            continue;
        }
        if (otherid >= 0  &&  otherid < _BOUNDARY_ID_BASE_) {
            continue;
        }

        isame = 0;
        id1 = iptr->surf1;
        id2 = iptr->surf2;
        for (j=0; j<NumWorkFaultIntersects; j++) {
            jptr = WorkFaultIntersects[j];
            if (jptr->surf1 == id1  &&  jptr->surf2 == id2) {
                isame = 1;
                break;
            }
            if (jptr->surf1 == id2  &&  jptr->surf2 == id1) {
                isame = 1;
                break;
            }
        }
        if (isame == 0) {
            istat =
              AddSealedFaultIntersect (
                  iptr->x, iptr->y, iptr->z, iptr->npts,
                  iptr->surf1, iptr->surf2);
            if (istat == -1) {
                FreeWorkFaultIntersects ();
                return -1;
            }
        }
    }

    if (SealedFaultIntersects != NULL) {
        do_write = csw_GetDoWrite ();
        if (do_write) {
            char   fname[200];
            sprintf (fname, "sealedfaultint.xyz");
            for (i=0; i<NumSealedFaultIntersects; i++) {
                ilist[i] = SealedFaultIntersects + i;
            }
            WriteIlinesToFile (
                ilist,
                NumSealedFaultIntersects,
                fname);
        }
    }

    FreeWorkFaultIntersects ();

    return 1;

}


/*-------------------------------------------------------------------------------*/

void SealedModel::WriteIlinesToFile (
    _INtersectionLine_   **ilist,
    int                  nlist,
    char                 *fname)
{

    int      nlines[1000], ilines[1000];
    _INtersectionLine_  *iptr = NULL;
    double   *xw = NULL, *yw = NULL, *zw = NULL;
    int      j, n, nmax, i;


    auto fscope = [&]()
    {
        csw_Free (xw);
    };
    CSWScopeGuard func_scope_guard (fscope);


    nmax = 0;
    for (i=0; i<nlist; i++) {
        nmax += ilist[i]->npts;
    }
    xw = (double *)csw_Malloc (nmax * 3 * sizeof(double));
    if (xw == NULL) {
        return;
    }
    yw = xw + nmax;
    zw = yw + nmax;
    n = 0;
    for (i=0; i<nlist; i++) {
        iptr = ilist[i];
        for (j=0; j<iptr->npts; j++) {
            xw[n] = iptr->x[j];
            yw[n] = iptr->y[j];
            zw[n] = iptr->z[j];
            n++;
        }
        ilines[i] = iptr->npts;
        nlines[i] = 1;
    }
    grd_api_obj.grd_WriteLines (
      xw, yw, zw,
      nlist, nlines, ilines,
      fname);

    return;

}


/*-----------------------------------------------------------------------*/

/*
 * Add a new sealed fault intersection line to the list.
 */
int SealedModel::AddSealedFaultIntersect (
    double         *x,
    double         *y,
    double         *z,
    int            npts,
    int            isurf1,
    int            isurf2)
{
    _INtersectionLine_   *iptr = NULL;
    double         *x2 = NULL, *y2 = NULL, *z2 = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (x2);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (IntersectionLines == NULL) {
        assert (0);
    }

    if (npts < 2) {
        return 1;
    }

    if (SealedFaultIntersects == NULL  ||
        NumSealedFaultIntersects >= MaxSealedFaultIntersects) {
        MaxSealedFaultIntersects += 20;
        SealedFaultIntersects = (_INtersectionLine_ *)csw_Realloc
            (SealedFaultIntersects,
             MaxSealedFaultIntersects * sizeof(_INtersectionLine_));
    }

    if (SealedFaultIntersects == NULL) {
        return -1;
    }

    iptr = SealedFaultIntersects + NumSealedFaultIntersects;
    memset (iptr, 0, sizeof(_INtersectionLine_));

    x2 = (double *)csw_Malloc (npts * 3 * sizeof(double));
    if (x2 == NULL) {
        return -1;
    }
    y2 = x2 + npts;
    z2 = y2 + npts;

    memcpy (x2, x, npts * sizeof(double));
    memcpy (y2, y, npts * sizeof(double));
    memcpy (z2, z, npts * sizeof(double));

    iptr->x = x2;
    iptr->y = y2;
    iptr->z = z2;
    iptr->npts = npts;

    iptr->embed_flag = 1;
    iptr->nclose = 0;

    iptr->surf1 = isurf1;
    iptr->surf2 = isurf2;

    NumSealedFaultIntersects++;

    bsuccess = true;

    return 1;

}



/*-----------------------------------------------------------------------------*/

/*
 * Use the sealed intersect lines along with the outline of the
 * specified trimesh to create a collection of polygons.  The
 * polygon that also has the minimum z values is chosen as the
 * fault clip polygon, effectively removing the padding at the
 * top of the fault but keeping the "listric" fault surface at
 * the bottom.
 */
int SealedModel::BuildFaultOutlineFromSealedIntersects (
    _INtersectionLine_        **sfplist,
    int                       nsfp,
    _INtersectionLine_        **shplist,
    int                       nshp,
    CSWTriMeshStruct          *tmesh,
    double                    **xcout,
    double                    **ycout,
    double                    **zcout,
    int                       *ncout,
    int                       **npcout)
{
    double                    *xc = NULL, *yc = NULL, *zc = NULL;
    int                       n, nc, *npc = NULL;
    int                       istat;

    double            xfirst, yfirst, zfirst,
                      xlast, ylast, zlast,
                      x1, y1, z1, x2, y2, z2;
    _INtersectionLine_  *iptr = NULL;
    int               i, j, ndo, maxdo, nmax, ndone, idone;
    double            *xappend = NULL, *yappend = NULL, *zappend = NULL,
                      *xprepend = NULL, *yprepend = NULL, *zprepend = NULL,
                      *xbord = NULL, *ybord = NULL, *zbord = NULL;
    int               nappend, nprepend, nbord;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        csw_Free (xappend);
        if (bsuccess == false) {
            csw_Free (xc);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    n = nc = 0;
    nshp = nshp;

/*
 * Initialize output in case of error.
 */
    *xcout = NULL;
    *ycout = NULL;
    *zcout = NULL;
    *ncout = 0;
    *npcout = NULL;

/*
 * Allocate space for append, prepend and border arrays.
 */
    nmax = shplist[0]->npts;
    nmax += 2;
    for (i=0; i<nsfp; i++) {
        nmax += sfplist[i]->npts;
        nmax += 2;
        sfplist[i]->nclose = 0;
    }

    nmax += tmesh->num_edges;
    nmax += 2;
    if (nmax < 1000) nmax = 1000;
    xappend = (double *)csw_Malloc (nmax * 9 * sizeof(double));
    if (xappend == NULL) {
        return -1;
    }
    yappend = xappend + nmax;
    zappend = yappend + nmax;
    xprepend = zappend + nmax;
    yprepend = xprepend + nmax;
    zprepend = yprepend + nmax;
    xbord = zprepend + nmax;
    ybord = xbord + nmax;
    zbord = ybord + nmax;

    nappend = 0;
    nprepend = 0;
    nbord = 0;

/*
 * Put the single horizon intersect line into the
 * append array as a starting point.
 */
    iptr = shplist[0];
    n = iptr->npts;
    xfirst = iptr->x[0];
    yfirst = iptr->y[0];
    zfirst = iptr->z[0];
    xlast = iptr->x[n-1];
    ylast = iptr->y[n-1];
    zlast = iptr->z[n-1];

    memcpy (xappend, iptr->x, n * sizeof(double));
    memcpy (yappend, iptr->y, n * sizeof(double));
    memcpy (zappend, iptr->z, n * sizeof(double));

    nappend = n;

    int       first_done = 0;
    int       last_done = 0;
    int       i1, i2;

/*
 * Trim the fault intersect lines to the portion intersecting
 * with and lower than (in z value) the top sealed horizon
 * intersect line.
 */
    ndone = 0;
    for (i=0; i<nsfp; i++) {
        iptr = sfplist[i];
        idone = -1;
        if (first_done == 0) {
            for (j=0; j<iptr->npts; j++) {
                istat = SamePointXYZ (xfirst, yfirst, zfirst,
                                      iptr->x[j], iptr->y[j], iptr->z[j]);
                if (istat == 1) {
                    idone = j;
                    first_done = 1;
                    break;
                }
            }
        }
        if (idone == -1  &&  last_done == 0) {
            for (j=0; j<iptr->npts; j++) {
                istat = SamePointXYZ (xlast, ylast, zlast,
                                      iptr->x[j], iptr->y[j], iptr->z[j]);
                if (istat == 1) {
                    idone = j;
                    last_done = 1;
                    break;
                }
            }
        }

        if (idone == -1) {
            continue;
        }

        if (iptr->z[0] < iptr->z[iptr->npts-1]) {
            i1 = 0;
            i2 = idone;
        }
        else {
            i1 = idone;
            i2 = iptr->npts-1;
        }

        n = 0;
        for (j=i1; j<=i2; j++) {
            xbord[n] = iptr->x[j];
            ybord[n] = iptr->y[j];
            zbord[n] = iptr->z[j];
            n++;
        }
        iptr->npts = n;
        memcpy (iptr->x, xbord, n * sizeof(double));
        memcpy (iptr->y, ybord, n * sizeof(double));
        memcpy (iptr->z, zbord, n * sizeof(double));
        ndone++;
    }

    if (ndone != 2) {
        printf ("There should be exactly 2 fault intersect lines\n"
                "trimmed to the top horizon intersect line\n");
        assert (0);
    }

/*
 * Loop through each scaled fault intersect and prepend
 * or append as needed.  This is repeated until no append
 * or prepend is needed.
 */
    ndo = 0;
    maxdo = nsfp + 1;
    for (;;) {
        ndone = 0;
        for (i=0; i<nsfp; i++) {
            iptr = sfplist[i];
            if (iptr->nclose == 1) {
                continue;
            }
            x1 = iptr->x[0];
            y1 = iptr->y[0];
            z1 = iptr->z[0];
            x2 = iptr->x[iptr->npts-1];
            y2 = iptr->y[iptr->npts-1];
            z2 = iptr->z[iptr->npts-1];

            istat = SamePointXYZ (xfirst, yfirst, zfirst,
                                  x1, y1, z1);
            if (istat == 1) {
                for (j=1; j<iptr->npts; j++) {
                    xprepend[nprepend] = iptr->x[j];
                    yprepend[nprepend] = iptr->y[j];
                    zprepend[nprepend] = iptr->z[j];
                    nprepend++;
                }
                ndone++;
                iptr->nclose = 1;
                xfirst = iptr->x[iptr->npts-1];
                yfirst = iptr->y[iptr->npts-1];
                zfirst = iptr->z[iptr->npts-1];
                continue;
            }

            istat = SamePointXYZ (xlast, ylast, zlast,
                                  x1, y1, z1);
            if (istat == 1) {
                for (j=1; j<iptr->npts; j++) {
                    xappend[nappend] = iptr->x[j];
                    yappend[nappend] = iptr->y[j];
                    zappend[nappend] = iptr->z[j];
                    nappend++;
                }
                ndone++;
                iptr->nclose = 1;
                xlast = iptr->x[iptr->npts-1];
                ylast = iptr->y[iptr->npts-1];
                zlast = iptr->z[iptr->npts-1];
                continue;
            }

            istat = SamePointXYZ (xfirst, yfirst, zfirst,
                                  x2, y2, z2);
            if (istat == 1) {
                for (j=iptr->npts-2; j>=0; j--) {
                    xprepend[nprepend] = iptr->x[j];
                    yprepend[nprepend] = iptr->y[j];
                    zprepend[nprepend] = iptr->z[j];
                    nprepend++;
                }
                ndone++;
                iptr->nclose = 1;
                xfirst = iptr->x[0];
                yfirst = iptr->y[0];
                zfirst = iptr->z[0];
                continue;
            }

            istat = SamePointXYZ (xlast, ylast, zlast,
                                  x2, y2, z2);
            if (istat == 1) {
                for (j=iptr->npts-2; j>=0; j--) {
                    xappend[nappend] = iptr->x[j];
                    yappend[nappend] = iptr->y[j];
                    zappend[nappend] = iptr->z[j];
                    nappend++;
                }
                ndone++;
                iptr->nclose = 1;
                xlast = iptr->x[0];
                ylast = iptr->y[0];
                zlast = iptr->z[0];
                continue;
            }

        }

        if (ndone == 0) {
            break;
        }
        if (ndo > maxdo) {
            printf ("Too many iterations splicing fault outline\n");
            assert (0);
        }

        ndo++;

    }

/*
 * Put the prepend and append lists into the bord arrays.
 */
    for (i=nprepend-1; i>=0; i--) {
        xbord[nbord] = xprepend[i];
        ybord[nbord] = yprepend[i];
        zbord[nbord] = zprepend[i];
        nbord++;
    }

    for (i=0; i<nappend; i++) {
        xbord[nbord] = xappend[i];
        ybord[nbord] = yappend[i];
        zbord[nbord] = zappend[i];
        nbord++;
    }

    int do_write;
    do_write = csw_GetDoWrite ();
    if (do_write) {
        char   fname[100];
        sprintf (fname, "part_stitch.xyz");
        n = 1;
        grd_api_obj.grd_WriteLines (xbord, ybord, zbord,
                        1, &n, &nbord, fname);
    }

/*
 * Fill the gap by linear interpolation.
 *
 *  TODO  This is a quick fix for veit's Cancun data.  The
 *        code should drape the xy line onto the padded fault
 *        for its z values.
 */
    double      space, sp1, sp2, dtot;
    double      dx, dy, dz;
    int         nsamp, nbord_orig;

    dx = xbord[0] - xbord[1];
    dy = ybord[0] - ybord[1];
    dz = zbord[0] - zbord[1];
    sp1 = dx * dx + dy * dy + dz * dz;
    sp1 = sqrt (sp1);

    dx = xbord[nbord-1] - xbord[nbord-2];
    dy = ybord[nbord-1] - ybord[nbord-2];
    dz = zbord[nbord-1] - zbord[nbord-2];
    sp2 = dx * dx + dy * dy + dz * dz;
    sp2 = sqrt (sp2);

    nbord_orig = nbord;
    space = (sp1 + sp2) / 2.0;

    dx = xbord[0] - xbord[nbord-1];
    dy = ybord[0] - ybord[nbord-1];
    dz = zbord[0] - zbord[nbord-1];
    dtot = dx * dx + dy * dy + dz * dz;
    dtot = sqrt (dtot);

    if (dtot < space * 1.5) {
        xbord[nbord] = xbord[0];
        ybord[nbord] = ybord[0];
        zbord[nbord] = zbord[0];
        nbord++;
    }
    else {
        nsamp = (int)(dtot/space + .5);
        dx = xbord[0] - xbord[nbord-1];
        dy = ybord[0] - ybord[nbord-1];
        dz = zbord[0] - zbord[nbord-1];
        dx /= nsamp;
        dy /= nsamp;
        dz /= nsamp;
        for (i=0; i<nsamp-1; i++) {
            if (nbord >= nmax - 1) {
                assert (0);
            }
            xbord[nbord] = xbord[nbord-1] + dx;
            ybord[nbord] = ybord[nbord-1] + dy;
          /* TODO the following line needs to be replaced by an
             interpolation from the fault at the xy point  */
            zbord[nbord] = zbord[nbord-1] + dz;
            nbord++;
        }
        xbord[nbord-1] = xbord[0];
        ybord[nbord-1] = ybord[0];
        zbord[nbord-1] = zbord[0];
    }

    xc = (double *)csw_Malloc (nbord * 3 * sizeof(double));
    if (xc == NULL) {
        return -1;
    }
    yc = xc + nbord;
    zc = yc + nbord;
    memcpy (xc, xbord, nbord * sizeof(double));
    memcpy (yc, ybord, nbord * sizeof(double));
    memcpy (zc, zbord, nbord * sizeof(double));

/*
 * If the gap fill is entirely on a boundary plane,
 * embed it into that boundary plane.
 */
    int nw, ne, nn, ns, nt;

    nw = ne = nn = ns = 0;
    nt = nbord - nbord_orig;

    double tiny = (padXmax - padXmin + padYmax - padYmin) / 20000.0;
    for (i=nbord_orig; i<nbord; i++) {
        istat = SameValue (xbord[i], padXWest, tiny);
        if (istat == 1) {
            nw++;
            continue;
        }
        istat = SameValue (xbord[i], padXEast, tiny);
        if (istat == 1) {
            ne++;
            continue;
        }
        istat = SameValue (ybord[i], padYSouth, tiny);
        if (istat == 1) {
            ns++;
            continue;
        }
        istat = SameValue (ybord[i], padYNorth, tiny);
        if (istat == 1) {
            nn++;
            continue;
        }
    }

    if (nn == nt) {
        istat = EmbedLineInBoundary (
            _BOUNDARY_ID_BASE_ + _NORTH_ID_,
            xbord + nbord_orig,
            ybord + nbord_orig,
            zbord + nbord_orig,
            nt);
    }
    else if (ns == nt) {
        istat = EmbedLineInBoundary (
            _BOUNDARY_ID_BASE_ + _SOUTH_ID_,
            xbord + nbord_orig,
            ybord + nbord_orig,
            zbord + nbord_orig,
            nt);
    }
    else if (ne == nt) {
        istat = EmbedLineInBoundary (
            _BOUNDARY_ID_BASE_ + _EAST_ID_,
            xbord + nbord_orig,
            ybord + nbord_orig,
            zbord + nbord_orig,
            nt);
    }
    else if (nw == nt) {
        istat = EmbedLineInBoundary (
            _BOUNDARY_ID_BASE_ + _WEST_ID_,
            xbord + nbord_orig,
            ybord + nbord_orig,
            zbord + nbord_orig,
            nt);
    }

    nc = 1;
    n = nbord;
    npc = (int *)csw_Malloc (1 * sizeof(int));
    if (npc == NULL) {
        return -1;
    }

    nc = 1;
    *xcout = xc;
    *ycout = yc;
    *zcout = zc;
    *ncout = nc;
    npc[0] = n;
    *npcout = npc;

    bsuccess = true;

    return 1;
}


/*-------------------------------------------------------------------------*/

/*
 * Return the nodes from the specified boundary surface that are at
 * or very close to the specified x or y value.  If you want to use
 * xval as the corner value, specify yval as 1.e30.  If you want to
 * use yval, specify xval as 1.e30.  If neither is set to 1.e30,
 * that is an programming error and the function asserts.  If an
 * invalid boundary id is specified, the function asserts.  If the
 * maxout value is too small, the function returns -1.  On success,
 * nout is greater than zero and 1 is returned.
 *
 * This is a protected method.
 */

int SealedModel::GetBoundaryCornerNodes (
    int          boundary_id,
    double       xval,
    double       yval,
    double       *zout,
    int          *nodeout,
    int          *nout,
    int          maxout)
{
    CSWTriMeshStruct        *tmesh;
    NOdeStruct              *nodes;
    int                     i, numnodes, n;
    double                  dx;

    *nout = 0;

    if (xval < 1.e20  &&  yval < 1.e20) {
        assert (0);
    }

    if (boundary_id < _BOUNDARY_ID_BASE_) {
        assert (0);
    }

    tmesh = NULL;
    if (boundary_id == _BOUNDARY_ID_BASE_ + _NORTH_ID_) {
        tmesh = EmbeddedNorthBoundarySurface;
    }
    else if (boundary_id == _BOUNDARY_ID_BASE_ + _SOUTH_ID_) {
        tmesh = EmbeddedSouthBoundarySurface;
    }
    else if (boundary_id == _BOUNDARY_ID_BASE_ + _EAST_ID_) {
        tmesh = EmbeddedEastBoundarySurface;
    }
    else if (boundary_id == _BOUNDARY_ID_BASE_ + _WEST_ID_) {
        tmesh = EmbeddedWestBoundarySurface;
    }

    if (tmesh == NULL) {
        assert (0);
    }

    nodes = tmesh->nodes;
    numnodes = tmesh->num_nodes;

    n = 0;
    if (xval < 1.e20) {
        for (i=0; i<numnodes; i++) {
            dx = nodes[i].x - xval;
            if (dx < 0.0) dx = -dx;
            if (dx <= modelTiny) {
                zout[n] = nodes[i].z;
                nodeout[n] = i;
                n++;
                if (n >= maxout) {
                    return -1;
                }
            }
        }
    }

    else if (yval < 1.e20) {
        for (i=0; i<numnodes; i++) {
            dx = nodes[i].y - yval;
            if (dx < 0.0) dx = -dx;
            if (dx <= modelTiny) {
                zout[n] = nodes[i].z;
                nodeout[n] = i;
                n++;
                if (n >= maxout) {
                    return -1;
                }
            }
        }
    }

    *nout = n;

    return 1;

}


/*--------------------------------------------------------------------------*/

/*
 * Remove a nodes from the boundary specified by boundary where
 * the shared node (shared with another boundary) exists on this
 * boundary but not on the other boundary.
 */
int SealedModel::FixBoundaryForSharedCorner (
    int         boundary_id,
    double      *myzvals,
    int         *mynodes,
    int         mynval,
    double      *otherzvals,
    int         othernval)
{
    int         i, j, n, good, istat, swapx, swapy;
    int         nodes_to_delete[2000];
    double      zt, dz, tiny, xt;
    CSWTriMeshStruct    *tmesh;

    tiny = modelTiny * 10.0;

/*
 * Find the boundary trimesh specified by the boundary_id, or
 * assert if none is found.
 */
    tmesh = NULL;
    swapx = 0;
    swapy = 0;
    if (boundary_id == _BOUNDARY_ID_BASE_ + _NORTH_ID_) {
        tmesh = EmbeddedNorthBoundarySurface;
        swapx = 0;
        swapy = 1;
    }
    else if (boundary_id == _BOUNDARY_ID_BASE_ + _SOUTH_ID_) {
        tmesh = EmbeddedSouthBoundarySurface;
        swapx = 0;
        swapy = 1;
    }
    else if (boundary_id == _BOUNDARY_ID_BASE_ + _EAST_ID_) {
        tmesh = EmbeddedEastBoundarySurface;
        swapx = 1;
        swapy = 0;
    }
    else if (boundary_id == _BOUNDARY_ID_BASE_ + _WEST_ID_) {
        tmesh = EmbeddedWestBoundarySurface;
        swapx = 1;
        swapy = 0;
    }

    if (tmesh == NULL) {
        assert (0);
    }

/*
 * For each node in the my list, see if there is a coincident
 * node in the other list.  If none is found, the node number
 * from the my list is added to the list of nodes to remove
 * from the trimesh.
 */
    n = 0;
    for (i=0; i<mynval; i++) {
        zt = myzvals[i];
        good = 0;
        for (j=0; j<othernval; j++) {
            dz = otherzvals[j] - zt;
            if (dz < 0) dz = -dz;
            if (dz <= tiny) {
                good = 1;
                break;
            }
        }
        if (good == 0) {
            nodes_to_delete[n] = mynodes[i];
            n++;
            if (n >= 2000) {
                assert (0);
            }
        }
    }

    if (n > 0) {

    /*
     * Temporarily swap the y and z coords or the x and z
     * coords so the remove node stuff works.
     */
        if (swapx) {
            for (i=0; i<tmesh->num_nodes; i++) {
                xt = tmesh->nodes[i].x;
                tmesh->nodes[i].x = tmesh->nodes[i].z;
                tmesh->nodes[i].z = xt;
            }
        }
        else if (swapy) {
            for (i=0; i<tmesh->num_nodes; i++) {
                xt = tmesh->nodes[i].y;
                tmesh->nodes[i].y = tmesh->nodes[i].z;
                tmesh->nodes[i].z = xt;
            }
        }

    /*
     * Remove the nodes while preserving the topology.
     */
        istat =
          grd_api_obj.grd_RemoveNodesFromTriMesh (
              &(tmesh->nodes), &(tmesh->num_nodes),
              &(tmesh->edges), &(tmesh->num_edges),
              &(tmesh->tris), &(tmesh->num_tris),
              nodes_to_delete, n);

    /*
     * Unswap the node coordinates.
     */
        if (swapx) {
            for (i=0; i<tmesh->num_nodes; i++) {
                xt = tmesh->nodes[i].x;
                tmesh->nodes[i].x = tmesh->nodes[i].z;
                tmesh->nodes[i].z = xt;
            }
        }
        else if (swapy) {
            for (i=0; i<tmesh->num_nodes; i++) {
                xt = tmesh->nodes[i].y;
                tmesh->nodes[i].y = tmesh->nodes[i].z;
                tmesh->nodes[i].z = xt;
            }
        }

        return istat;
    }

    return 1;

}



/*-----------------------------------------------------------------------*/

#define MAX_CORNER_SIZE 2000

/*
 * Make sure the nodes for each surface sharing a corner are
 * in identical locations on the corner.
 */
int SealedModel::FixBoundaryCorners (void)
{
    //double         zout1[MAX_CORNER_SIZE], zout2[MAX_CORNER_SIZE];
    //int            nodeout1[MAX_CORNER_SIZE], nodeout2[MAX_CORNER_SIZE];

    double         *zout1 = dmcs_1;
    double         *zout2 = dmcs_2;
    int            *nodeout1 = imcs_1;
    int            *nodeout2 = imcs_2;

    int            istat, nout1, nout2;

/*
 * Northeast corner.
 */
    istat =
      GetBoundaryCornerNodes (
        _BOUNDARY_ID_BASE_ + _NORTH_ID_,
        padXEast, 1.e30,
        zout1, nodeout1,
        &nout1, MAX_CORNER_SIZE);
    if (istat == -1) {
        return -1;
    }

    istat =
      GetBoundaryCornerNodes (
        _BOUNDARY_ID_BASE_ + _EAST_ID_,
        1.e30, padYNorth,
        zout2, nodeout2,
        &nout2, MAX_CORNER_SIZE);
    if (istat == -1) {
        return -1;
    }

    istat =
      FixBoundaryForSharedCorner (
        _BOUNDARY_ID_BASE_ + _NORTH_ID_,
        zout1, nodeout1, nout1,
        zout2, nout2);
    if (istat == -1) {
        return -1;
    }

    istat =
      FixBoundaryForSharedCorner (
        _BOUNDARY_ID_BASE_ + _EAST_ID_,
        zout2, nodeout2, nout2,
        zout1, nout1);
    if (istat == -1) {
        return -1;
    }

/*
 * Northwest corner.
 */
    istat =
      GetBoundaryCornerNodes (
        _BOUNDARY_ID_BASE_ + _NORTH_ID_,
        padXWest, 1.e30,
        zout1, nodeout1,
        &nout1, MAX_CORNER_SIZE);
    if (istat == -1) {
        return -1;
    }

    istat =
      GetBoundaryCornerNodes (
        _BOUNDARY_ID_BASE_ + _WEST_ID_,
        1.e30, padYNorth,
        zout2, nodeout2,
        &nout2, MAX_CORNER_SIZE);
    if (istat == -1) {
        return -1;
    }

    istat =
      FixBoundaryForSharedCorner (
        _BOUNDARY_ID_BASE_ + _NORTH_ID_,
        zout1, nodeout1, nout1,
        zout2, nout2);
    if (istat == -1) {
        return -1;
    }

    istat =
      FixBoundaryForSharedCorner (
        _BOUNDARY_ID_BASE_ + _WEST_ID_,
        zout2, nodeout2, nout2,
        zout1, nout1);
    if (istat == -1) {
        return -1;
    }

/*
 * Southeast corner.
 */
    istat =
      GetBoundaryCornerNodes (
        _BOUNDARY_ID_BASE_ + _SOUTH_ID_,
        padXEast, 1.e30,
        zout1, nodeout1,
        &nout1, MAX_CORNER_SIZE);
    if (istat == -1) {
        return -1;
    }

    istat =
      GetBoundaryCornerNodes (
        _BOUNDARY_ID_BASE_ + _EAST_ID_,
        1.e30, padYSouth,
        zout2, nodeout2,
        &nout2, MAX_CORNER_SIZE);
    if (istat == -1) {
        return -1;
    }

    istat =
      FixBoundaryForSharedCorner (
        _BOUNDARY_ID_BASE_ + _SOUTH_ID_,
        zout1, nodeout1, nout1,
        zout2, nout2);
    if (istat == -1) {
        return -1;
    }

    istat =
      FixBoundaryForSharedCorner (
        _BOUNDARY_ID_BASE_ + _EAST_ID_,
        zout2, nodeout2, nout2,
        zout1, nout1);
    if (istat == -1) {
        return -1;
    }

/*
 * Southwest corner.
 */
    istat =
      GetBoundaryCornerNodes (
        _BOUNDARY_ID_BASE_ + _SOUTH_ID_,
        padXWest, 1.e30,
        zout1, nodeout1,
        &nout1, MAX_CORNER_SIZE);
    if (istat == -1) {
        return -1;
    }

    istat =
      GetBoundaryCornerNodes (
        _BOUNDARY_ID_BASE_ + _WEST_ID_,
        1.e30, padYSouth,
        zout2, nodeout2,
        &nout2, MAX_CORNER_SIZE);
    if (istat == -1) {
        return -1;
    }

    istat =
      FixBoundaryForSharedCorner (
        _BOUNDARY_ID_BASE_ + _SOUTH_ID_,
        zout1, nodeout1, nout1,
        zout2, nout2);
    if (istat == -1) {
        return -1;
    }

    istat =
      FixBoundaryForSharedCorner (
        _BOUNDARY_ID_BASE_ + _WEST_ID_,
        zout2, nodeout2, nout2,
        zout1, nout1);
    if (istat == -1) {
        return -1;
    }

    return 1;

}



/*---------------------------------------------------------------------*/

/*
 * Resample each sealed intersect line to distribute the points
 * evenly between the end points of each line.
 */
int SealedModel::ResampleSealedIntersects (void)
{
    _INtersectionLine_  *iptr = NULL;
    double              *xout = NULL, *yout = NULL, *zout = NULL;
    int                 nout, npts, maxout, i, istat;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xout);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (SealedHorizonIntersects == NULL) {
        assert (0);
    }
    if (simSealFlag  &&  SealedFaultIntersects == NULL) {
        assert (0);
    }

    for (i=0; i<NumSealedHorizonIntersects; i++) {
        iptr = SealedHorizonIntersects + i;
        npts = iptr->npts;
        maxout = npts * 2;
        xout = (double *)csw_Malloc (maxout * 3 * sizeof(double));
        if (xout == NULL) {
            return -1;
        }
        yout = xout + maxout;
        zout = yout + maxout;
        nout = 0;

        istat = CorrectForEndPointSpacing (
            iptr->x, iptr->y, iptr->z, npts,
            xout, yout, zout, &nout, maxout);
        if (istat == -1  ||  nout < 2) {
            return -1;
        }
        csw_Free (iptr->x);
        iptr->x = xout;
        iptr->y = yout;
        iptr->z = zout;
        iptr->npts = nout;
        xout = yout = zout = NULL;
        nout = 0;
    }

    if (SealedFaultIntersects != NULL) {
      for (i=0; i<NumSealedFaultIntersects; i++) {
        iptr = SealedFaultIntersects + i;
        npts = iptr->npts;
        maxout = npts * 4;
        if (maxout < 100) maxout = 100;
        xout = (double *)csw_Malloc (maxout * 3 * sizeof(double));
        if (xout == NULL) {
            return -1;
        }
        yout = xout + maxout;
        zout = yout + maxout;
        nout = 0;

        istat = CorrectForEndPointSpacing (
            iptr->x, iptr->y, iptr->z, npts,
            xout, yout, zout, &nout, maxout);
        if (istat == -1  ||  nout < 2) {
            return -1;
        }

        istat = SplitLongSegments (
            xout, yout, zout, &nout, maxout);
        if (istat == -1  ||  nout < 2) {
            return -1;
        }

        csw_Free (iptr->x);
        iptr->x = xout;
        iptr->y = yout;
        iptr->z = zout;
        iptr->npts = nout;
        xout = yout = zout = NULL;
        nout = 0;
      }
    }

    bsuccess = true;

    return 1;
}


/*----------------------------------------------------------------------*/

int SealedModel::SplitLongSegments (
    double      *xout,
    double      *yout,
    double      *zout,
    int         *nout,
    int         maxout)
{
    double      *xw = NULL, *yw = NULL, *zw = NULL;
    int         n, i, npts;
    double      dx, dy, dz, dist, dcrit;


    auto fscope = [&]()
    {
        csw_Free (xw);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (averageSpacing <= 0.0) {
        return 1;
    }

    npts = *nout;
    dcrit = averageSpacing * 1.4;

    xw = (double *)csw_Malloc (3 * maxout * sizeof(double));
    if (xw == NULL) {
        return -1;
    }
    yw = xw + maxout;
    zw = yw + maxout;

    xw[0] = xout[0];
    yw[0] = yout[0];
    zw[0] = zout[0];
    n = 1;

    for (i=1; i<npts; i++) {
        dx = xout[i] - xout[i-1];
        dy = yout[i] - yout[i-1];
        dz = zout[i] - zout[i-1];
        dist = dx * dx + dy * dy + dz * dz;
        dist = sqrt (dist);
        if (dist <= dcrit) {
            xw[n] = xout[i];
            yw[n] = yout[i];
            zw[n] = zout[i];
            n++;
            continue;
        }
        xw[n] = xw[n-1] + dx / 2.0;
        yw[n] = yw[n-1] + dy / 2.0;
        zw[n] = zw[n-1] + dz / 2.0;
        n++;
        xw[n] = xout[i];
        yw[n] = yout[i];
        zw[n] = zout[i];
        n++;
        if (n >= maxout) {
            return -1;
        }
    }

    *nout = n;

    memcpy (xout, xw, n * sizeof(double));
    memcpy (yout, yw, n * sizeof(double));
    memcpy (zout, zw, n * sizeof(double));

    return 1;

}


/*----------------------------------------------------------------------*/

/*
 * If the spacing between an end point of the line and its neighbor is
 * significantly less than the spacing between the neif=ghbor and its
 * neighbor away from the end point, then the point mext to the end point
 * is removed or moved firther away from the end point.
 */
int SealedModel::CorrectForEndPointSpacing (
    double    *xin,
    double    *yin,
    double    *zin,
    int       nin,
    double    *xout,
    double    *yout,
    double    *zout,
    int       *nout,
    int       maxout)
{
    int       i, n;
    double    dx, dy, dz, dist1, dist2;
    int       dflag1, dflag2;

    *nout = 0;
    if (nin < 2) {
        return -1;
    }

    if (nin == 2) {
        memcpy (xout, xin, 2 * sizeof(double));
        memcpy (yout, yin, 2 * sizeof(double));
        memcpy (zout, zin, 2 * sizeof(double));
        *nout = 2;
        return 1;
    }

/*
 * Check if point 1 in the line needs to be deleted or moved.
 */
    dx = xin[0] - xin[1];
    dy = yin[0] - yin[1];
    dz = zin[0] - zin[1];
    dist1 = dx * dx + dy * dy + dz * dz;
    dist1 = sqrt (dist1);
    dx = xin[2] - xin[1];
    dy = yin[2] - yin[1];
    dz = zin[2] - zin[1];
    dist2 = dx * dx + dy * dy + dz * dz;
    dist2 = sqrt (dist1);

    if (dist1 < 0.45 * dist2) {
        dflag1 = 1;
    }
    else if (dist1 < 0.75 * dist2) {
        dflag1 = 2;
    }
    else {
        dflag1 = 0;
    }

/*
 * Check if point nin-2 in the line needs to be deleted or moved.
 */
    dx = xin[nin-1] - xin[nin-2];
    dy = yin[nin-1] - yin[nin-2];
    dz = zin[nin-1] - zin[nin-2];
    dist1 = dx * dx + dy * dy + dz * dz;
    dist1 = sqrt (dist1);
    dx = xin[nin-3] - xin[nin-2];
    dy = yin[nin-3] - yin[nin-2];
    dz = zin[nin-3] - zin[nin-2];
    dist2 = dx * dx + dy * dy + dz * dz;
    dist2 = sqrt (dist1);

    if (dist1 < 0.45 * dist2) {
        dflag2 = 1;
    }
    else if (dist1 < 0.75 * dist2) {
        dflag2 = 2;
    }
    else {
        dflag2 = 0;
    }

/*
 * Assemble the output line.
 */

  /*
   * The first point is always used as is.
   */
    xout[0] = xin[0];
    yout[0] = yin[0];
    zout[0] = zin[0];
    n = 1;

  /*
   * If dflag1 is 1, the second input point is too close
   * to the first input point and it is not used in the
   * output line.
   */

  /*
   * The second output point is the second input point if
   * dflag1 is zero.
   */
    if (dflag1 == 0) {
        xout[1] = xin[1];
        yout[1] = yin[1];
        zout[1] = zin[1];
        n = 2;
    }

  /*
   * The second output point is the mid point between the
   * first and third input points if dflag1 is 2.
   */
    else if (dflag1 == 2) {
        dx = (xin[0] + xin[2]) / 2.0;
        dy = (yin[0] + yin[2]) / 2.0;
        dz = (zin[0] + zin[2]) / 2.0;
        xout[1] = dx;
        yout[1] = dy;
        zout[1] = dz;
        n = 2;
    }

  /*
   * The third through nin - 3 input points are used as is in the output.
   */
    for (i=2; i<nin - 2; i++) {
        xout[n] = xin[i];
        yout[n] = yin[i];
        zout[n] = zin[i];
        n++;
        if (n >= maxout) {
            return -1;
        }
    }

  /*
   * If there are 3 or less input points, the next to last
   * point is the same as the next to first point and it
   * has already been taken care of.
   */
    if (nin > 3) {

      /*
       * The next to last output point is the same as the next to last
       * input point if dflag2 is zero.
       */
        if (dflag2 == 0) {
            xout[n] = xin[nin - 2];
            yout[n] = yin[nin - 2];
            zout[n] = zin[nin - 2];
            n++;
            if (n >= maxout) {
                return -1;
            }
        }

      /*
       * The next to last output point is the mid point between the
       * the last input point and the nin - 3 input point if dflag2
       * is 2.
       */
        else if (dflag2 == 2) {
            dx = (xin[nin-1] + xin[nin-3]) / 2.0;
            dy = (yin[nin-1] + yin[nin-3]) / 2.0;
            dz = (zin[nin-1] + zin[nin-3]) / 2.0;
            xout[n] = dx;
            yout[n] = dy;
            zout[n] = dz;
            n++;
            if (n >= maxout) {
                return -1;
            }
        }
    }

/*
 * The last input point is always used as is.
 */
    xout[n] = xin[nin-1];
    yout[n] = yin[nin-1];
    zout[n] = zin[nin-1];
    n++;

    *nout = n;

    return 1;
}


/*--------------------------------------------------------------------------------*/

/*
 * Write a report about the surfaces and triangles in the
 * sealed model.  If the filename is not NULL, the report is
 * written to that file.  If the filename is NULL, the report
 * is written to stdout.
 */
int SealedModel::analyzeSealedModel (char *filename)
{
    FILE         *fptr;

    if (filename) {
        fptr = fopen (filename, "wb");
    }
    else {
        fptr = stdout;
    }

    if (fptr == NULL) {
        return -1;
    }

    int          i, j, ntot;
    char         c200[200];
    double       minang, mintriang, pct;
    int          histang[10], ibin, n1, n2, n3;
    double       ang1, ang2, ang3;
    CSWTriMeshStruct    *tmesh;
    NOdeStruct   *nodes;
    EDgeStruct   *edges;
    TRiangleStruct  *tris;
    int          numtris;
    int          minclass, minsurf, mintriangle;
    double       xmin, ymin, zmin;
    int          nodemin, min_node;

    if (SealedHorizonList == NULL  ||
        SealedFaultList == NULL) {
        return -1;
    }

    minang = 1.e30;
    memset (histang, 0, 10 * sizeof(int));
    xmin = ymin = zmin = 1.e30;

    ntot = 0;
    minclass = min_node = minsurf = mintriangle = -1;

    for (i=0; i<NumSealedHorizonList; i++) {

        tmesh = SealedHorizonList + i;
        nodes = tmesh->nodes;
        edges = tmesh->edges;
        tris = tmesh->tris;
        numtris = tmesh->num_tris;

        for (j=0; j<numtris; j++) {

            grd_api_obj.grd_GetNodesForTriangle (tris+j, edges, &n1, &n2, &n3);
            ang1 =
              grd_utils_obj.grd_vector_angle (
                nodes[n1].x, nodes[n1].y, nodes[n1].z,
                nodes[n2].x, nodes[n2].y, nodes[n2].z,
                nodes[n3].x, nodes[n3].y, nodes[n3].z);
            ang2 =
              grd_utils_obj.grd_vector_angle (
                nodes[n2].x, nodes[n2].y, nodes[n2].z,
                nodes[n1].x, nodes[n1].y, nodes[n1].z,
                nodes[n3].x, nodes[n3].y, nodes[n3].z);
            ang3 = 180. - (ang1 + ang2);
            mintriang = ang1;
            nodemin = n1;
            if (ang2 < mintriang) {
                mintriang = ang2;
                nodemin = n2;
            }
            if (ang3 < mintriang) {
                mintriang = ang3;
                nodemin = n3;
            }

            if (mintriang < minang) {
                minang = mintriang;
                minclass = 1;
                minsurf = i;
                mintriangle = j;
                xmin = nodes[nodemin].x;
                ymin = nodes[nodemin].y;
                zmin = nodes[nodemin].z;
                min_node = nodemin;
            }

            ibin = (int)(mintriang / 10.0);
            histang[ibin]++;
            ntot++;
        }

    }

    for (i=0; i<NumSealedFaultList; i++) {

        tmesh = SealedFaultList + i;
        nodes = tmesh->nodes;
        edges = tmesh->edges;
        tris = tmesh->tris;
        numtris = tmesh->num_tris;

        for (j=0; j<numtris; j++) {

            grd_api_obj.grd_GetNodesForTriangle (tris+j, edges, &n1, &n2, &n3);
            ang1 =
              grd_utils_obj.grd_vector_angle (
                nodes[n1].x, nodes[n1].y, nodes[n1].z,
                nodes[n2].x, nodes[n2].y, nodes[n2].z,
                nodes[n3].x, nodes[n3].y, nodes[n3].z);
            ang2 =
              grd_utils_obj.grd_vector_angle (
                nodes[n2].x, nodes[n2].y, nodes[n2].z,
                nodes[n1].x, nodes[n1].y, nodes[n1].z,
                nodes[n3].x, nodes[n3].y, nodes[n3].z);
            ang3 = 180. - (ang1 + ang2);
            mintriang = ang1;
            nodemin = n1;
            if (ang2 < mintriang) {
                mintriang = ang2;
                nodemin = n2;
            }
            if (ang3 < mintriang) {
                mintriang = ang3;
                nodemin = n3;
            }

            if (mintriang < minang) {
                minang = mintriang;
                minclass = 2;
                minsurf = i;
                mintriangle = j;
                xmin = nodes[nodemin].x;
                ymin = nodes[nodemin].y;
                zmin = nodes[nodemin].z;
                min_node = nodemin;
            }

            ibin = (int)(mintriang / 10.0);
            histang[ibin]++;
            ntot++;
        }

    }

    if (SealedSedimentSurface != NULL) {
        tmesh = SealedSedimentSurface;
        nodes = tmesh->nodes;
        edges = tmesh->edges;
        tris = tmesh->tris;
        numtris = tmesh->num_tris;

        for (j=0; j<numtris; j++) {

            grd_api_obj.grd_GetNodesForTriangle (tris+j, edges, &n1, &n2, &n3);
            ang1 =
              grd_utils_obj.grd_vector_angle (
                nodes[n1].x, nodes[n1].y, nodes[n1].z,
                nodes[n2].x, nodes[n2].y, nodes[n2].z,
                nodes[n3].x, nodes[n3].y, nodes[n3].z);
            ang2 =
              grd_utils_obj.grd_vector_angle (
                nodes[n2].x, nodes[n2].y, nodes[n2].z,
                nodes[n1].x, nodes[n1].y, nodes[n1].z,
                nodes[n3].x, nodes[n3].y, nodes[n3].z);
            ang3 = 180. - (ang1 + ang2);
            mintriang = ang1;
            nodemin = n1;
            if (ang2 < mintriang) {
                mintriang = ang2;
                nodemin = n2;
            }
            if (ang3 < mintriang) {
                mintriang = ang3;
                nodemin = n3;
            }

            if (mintriang < minang) {
                minang = mintriang;
                minclass = 2;
                minsurf = i;
                mintriangle = j;
                xmin = nodes[nodemin].x;
                ymin = nodes[nodemin].y;
                zmin = nodes[nodemin].z;
                min_node = nodemin;
            }

            ibin = (int)(mintriang / 10.0);
            histang[ibin]++;
            ntot++;
        }
    }

    if (SealedModelBottom != NULL) {
        tmesh = SealedModelBottom;
        nodes = tmesh->nodes;
        edges = tmesh->edges;
        tris = tmesh->tris;
        numtris = tmesh->num_tris;

        for (j=0; j<numtris; j++) {

            grd_api_obj.grd_GetNodesForTriangle (tris+j, edges, &n1, &n2, &n3);
            ang1 =
              grd_utils_obj.grd_vector_angle (
                nodes[n1].x, nodes[n1].y, nodes[n1].z,
                nodes[n2].x, nodes[n2].y, nodes[n2].z,
                nodes[n3].x, nodes[n3].y, nodes[n3].z);
            ang2 =
              grd_utils_obj.grd_vector_angle (
                nodes[n2].x, nodes[n2].y, nodes[n2].z,
                nodes[n1].x, nodes[n1].y, nodes[n1].z,
                nodes[n3].x, nodes[n3].y, nodes[n3].z);
            ang3 = 180. - (ang1 + ang2);
            mintriang = ang1;
            nodemin = n1;
            if (ang2 < mintriang) {
                mintriang = ang2;
                nodemin = n2;
            }
            if (ang3 < mintriang) {
                mintriang = ang3;
                nodemin = n3;
            }

            if (mintriang < minang) {
                minang = mintriang;
                minclass = 2;
                minsurf = i;
                mintriangle = j;
                xmin = nodes[nodemin].x;
                ymin = nodes[nodemin].y;
                zmin = nodes[nodemin].z;
                min_node = nodemin;
            }

            ibin = (int)(mintriang / 10.0);
            histang[ibin]++;
            ntot++;
        }
    }

    sprintf (c200,
             "\n\nNumber of sealed horizons: %d\n",
             NumSealedHorizonList);
    fputs (c200, fptr);
    sprintf (c200,
             "Number of sealed faults: %d\n\n",
             NumSealedFaultList);
    fputs (c200, fptr);

    sprintf (c200,
             "Minimum angle in any triangle: %d\n",
             (int)(minang + .5));
    fputs (c200, fptr);
    if (minclass == 1) {
        sprintf (c200,
                 "found on horizon number %d, triangle number %d\n\n",
                  minsurf+1, mintriangle);
    }
    else {
        sprintf (c200,
                 "found on fault number %d, triangle number %d\n",
                  minsurf+1, mintriangle);
    }
    fputs (c200, fptr);
    sprintf (c200,
             "Near node %d at %f %f %f\n\n",
              min_node, xmin, ymin, zmin);
    fputs (c200, fptr);

    int    i1, i2;
    for (i=0; i<7; i++) {
        i1 = i * 10;
        i2 = i1 + 10;
        pct = (double)histang[i] / (double)ntot;
        pct *= 100.0;
        sprintf (c200,
                 "Angles from %d to %d degrees: %d (%.1f percent)\n",
                 i1, i2, histang[i], pct);
        fputs (c200, fptr);
    }

    if (fptr != stdout) {
        fclose (fptr);
    }

    return 1;

}


/*-------------------------------------------------------------------------------------*/

/**
 * Set the sediment surface of the model.  The sediment surface is specified as a
 * "CSW trimesh", i.e. as lists of nodes, edges, and triangles.  This method creates
 * copies of the specified node, edge and triangle arrays.  So, the calling function
 * should csw_Free these when it no longer directly needs them.  On success, 1 is returned.
 * On a memory allocation failure, -1 is returned.
 *
 * The sediment surface specified here will replace any existing sediment surface.
 */
int SealedModel::setSedimentSurface (
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    int               istat;

    istat = setSedimentSurface (
        0, 0.0,
        nodes_in, num_nodes,
        edges_in, num_edges,
        triangles_in, num_triangles);
    return istat;

}


int SealedModel::setSedimentSurface (
    int               id,
    double            age,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;

    CSWTriMeshStruct   *tmesh = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
            csw_Free (tmesh);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    FreeTmesh (InputSedimentSurface, 1);
    InputSedimentSurface = NULL;

    tmesh = (CSWTriMeshStruct *)csw_Calloc (sizeof(CSWTriMeshStruct));
    if (tmesh == NULL) {
        return -1;
    }

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;
    tmesh->external_id = id;
    tmesh->age = age;

    InputSedimentSurface = tmesh;

    bsuccess = true;

    return 1;

}





int SealedModel::addToSedimentSurface (
    int               id,
    double            age,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    AddToSedimentSurface
                     (nodes_in, num_nodes,
                      edges_in, num_edges,
                      triangles_in, num_triangles);
    if (InputSedimentSurface != NULL) {
        InputSedimentSurface->external_id = id;
        InputSedimentSurface->age = age;
    }

    return 1;

}


/*-----------------------------------------------------------------------------------*/

/*
 * Add the specified nodes, edges and triangles to the current sediment surface.
 * This is needed because several struct3d "surfaces" may need to be combined
 * into a single sediment surface.
 *
 * This is a protected method.
 */
int SealedModel::AddToSedimentSurface (
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    int               istat;

    if (InputSedimentSurface == NULL) {
        istat =
        setSedimentSurface (
            nodes_in, num_nodes,
            edges_in, num_edges,
            triangles_in, num_triangles);
        return istat;
    }

    NOdeStruct        *nodes, *nodes2;
    EDgeStruct        *edges, *edges2;
    TRiangleStruct    *tris, *tris2;
    int               nn1, nn2, ne1, ne2, nt1, nt2;

    nodes = InputSedimentSurface->nodes;
    edges = InputSedimentSurface->edges;
    tris = InputSedimentSurface->tris;
    nn1 = InputSedimentSurface->num_nodes;
    ne1 = InputSedimentSurface->num_edges;
    nt1 = InputSedimentSurface->num_tris;
    nn2 = nn1 + num_nodes;
    ne2 = ne1 + num_edges;
    nt2 = nt1 + num_triangles;

    nodes2 = (NOdeStruct *)csw_Realloc (nodes, nn2 * sizeof(NOdeStruct));
    edges2 = (EDgeStruct *)csw_Realloc (edges, ne2 * sizeof(EDgeStruct));
    tris2 = (TRiangleStruct *)csw_Realloc (tris, nt2 * sizeof(TRiangleStruct));
    if (nodes2 == NULL  ||  edges2 == NULL  ||  tris2 == NULL) {
        csw_Free (nodes2);
        csw_Free (edges2);
        csw_Free (tris2);
        return -1;
    }
    memcpy (nodes2+nn1, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges2+ne1, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (tris2+nt1, triangles_in, num_triangles * sizeof(TRiangleStruct));

    InputSedimentSurface->nodes = nodes2;
    InputSedimentSurface->edges = edges2;
    InputSedimentSurface->tris = tris2;
    InputSedimentSurface->num_nodes = nn2;
    InputSedimentSurface->num_edges = ne2;
    InputSedimentSurface->num_tris = nt2;

    return 1;

}



/*-----------------------------------------------------------------------------------*/

/*
 * Add the specified nodes, edges and triangles to the current model bottom.
 * This is needed because several struct3d "surfaces" may need to be combined
 * into a single sediment surface.
 *
 * This is a protected method.
 */
int SealedModel::AddToModelBottom (
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    int               istat;

    if (InputModelBottom == NULL) {
        istat =
        setModelBottom (
            nodes_in, num_nodes,
            edges_in, num_edges,
            triangles_in, num_triangles);
        return istat;
    }

    NOdeStruct        *nodes, *nodes2;
    EDgeStruct        *edges, *edges2;
    TRiangleStruct    *tris, *tris2;
    int               nn1, nn2, ne1, ne2, nt1, nt2;

    nodes = InputModelBottom->nodes;
    edges = InputModelBottom->edges;
    tris = InputModelBottom->tris;
    nn1 = InputModelBottom->num_nodes;
    ne1 = InputModelBottom->num_edges;
    nt1 = InputModelBottom->num_tris;
    nn2 = nn1 + num_nodes;
    ne2 = ne1 + num_edges;
    nt2 = nt1 + num_triangles;

    nodes2 = (NOdeStruct *)csw_Realloc (nodes, nn2 * sizeof(NOdeStruct));
    edges2 = (EDgeStruct *)csw_Realloc (edges, ne2 * sizeof(EDgeStruct));
    tris2 = (TRiangleStruct *)csw_Realloc (tris, nt2 * sizeof(TRiangleStruct));
    if (nodes2 == NULL  ||  edges2 == NULL  ||  tris2 == NULL) {
        csw_Free (nodes2);
        csw_Free (edges2);
        csw_Free (tris2);
        return -1;
    }
    memcpy (nodes2+nn1, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges2+ne1, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (tris2+nt1, triangles_in, num_triangles * sizeof(TRiangleStruct));

    InputModelBottom->nodes = nodes2;
    InputModelBottom->edges = edges2;
    InputModelBottom->tris = tris2;
    InputModelBottom->num_nodes = nn2;
    InputModelBottom->num_edges = ne2;
    InputModelBottom->num_tris = nt2;

    return 1;

}


/*-------------------------------------------------------------------------------------*/

/**
 * Set the model bottom surface.  The model bottom surface is specified as a
 * "CSW trimesh", i.e. as lists of nodes, edges, and triangles.  This method creates
 * copies of the specified node, edge and triangle arrays.  So, the calling function
 * should csw_Free these when it no longer directly needs them.  On success, 1 is returned.
 * On a memory allocation failure, -1 is returned.
 *
 * The sediment surface specified here will replace any existing sediment surface.
 */
int SealedModel::setModelBottom (
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    int               istat;
    istat = setModelBottom (
        0, -1.0,
        nodes_in, num_nodes,
        edges_in, num_edges,
        triangles_in, num_triangles);
    return istat;
}


int SealedModel::setModelBottom (
    int               id,
    double            age,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;

    CSWTriMeshStruct   *tmesh = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
            csw_Free (tmesh);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    FreeTmesh (InputModelBottom, 1);
    InputModelBottom = NULL;

    tmesh = (CSWTriMeshStruct *)csw_Calloc (sizeof(CSWTriMeshStruct));
    if (tmesh == NULL) {
        return -1;
    }

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
        csw_Free (tmesh);
        return -1;
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;
    tmesh->external_id = id;
    tmesh->age = age;

    InputModelBottom = tmesh;

    bsuccess = true;

    return 1;

}

int SealedModel::addToModelBottom (
    int               id,
    double            age,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    AddToModelBottom (nodes_in, num_nodes,
                      edges_in, num_edges,
                      triangles_in, num_triangles);
    if (InputModelBottom != NULL) {
        InputModelBottom->external_id = id;
        InputModelBottom->age = age;
    }

    return 1;

}


/*--------------------------------------------------------------------------------*/

/*
 * Create a new set of intersection lines that exactly match the points in the
 * sealed top, sediment surface, bottom and model bottom outlines.
 */
int SealedModel::CreateSealedTopAndBottomIntersects (void)
{
    int          j, k, npts;
    _ITag_       *it1 = NULL, *itags = NULL;
    _OUtline_    *outline = NULL;
    _SEaledBorder_  *sbord = NULL;
    CSWTriMeshStruct  *tmesh = NULL;
    double       *x = NULL, *y = NULL, *z = NULL;
    int          n, kk, nseg, npts2, start, istat, nout;
    double       *x2 = NULL, *y2 = NULL, *z2 = NULL;
    int          n1[1000], n2[1000], olines[1000], iborder[1000];


    auto fscope = [&]()
    {
        csw_Free (x2);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (PaddedSedimentSurface != NULL) {
        tmesh = PaddedSedimentSurface;
        outline = CalcSealedHorizonOutline (tmesh);
        if (outline == NULL) {
            return -1;
        }
        npts = outline->npts;
        itags = outline->itags;
        x = outline->x;
        y = outline->y;
        z = outline->z;

    /*
     * Make sure the first and last outline points have
     * valid line ids.
     */
        if (itags[0].lineid == -1) {
            for (j=1; j<npts-1; j++) {
                if (itags[j].lineid >= 0) {
                    itags[0].lineid = itags[j].lineid;
                    break;
                }
            }
            if (itags[npts-1].lineid >= 0) {
                itags[0].lineid2 = itags[npts-1].lineid;
            }
            itags[0].pointid = -1;
            itags[0].pointid2 = -1;
        }

        if (itags[npts-1].lineid == -1) {
            for (j=npts-2; j>0; j--) {
                if (itags[j].lineid >= 0) {
                    itags[npts-1].lineid = itags[j].lineid;
                    break;
                }
            }
            if (itags[0].lineid >= 0) {
                itags[npts-1].lineid2 = itags[0].lineid;
            }
            itags[npts-1].pointid = -1;
            itags[npts-1].pointid2 = -1;
        }

        if (itags[0].lineid == -1) {
            printf ("Error getting first point for sealed horizon intersects\n");
            assert (0);
        }
        if (itags[npts-1].lineid == -1) {
            printf ("Error getting last point for sealed horizon intersects\n");
            assert (0);
        }

    /*
     * Fill in line ids for invalid intermediate points.
     */
        for (j=1; j<npts-1; j++) {

            if (itags[j].lineid >= 0) {
                continue;
            }

            it1 = itags + j;
            it1->lineid = itags[j-1].lineid;
            it1->lineid2 = -1;
            for (k=j+1; k<npts; k++) {
                if (itags[k].lineid >= 0) {
                    it1->lineid2 = itags[k].lineid;
                    break;
                }
            }

            if (it1->lineid2 == -1) {
                printf
                ("Error getting lineid2 for intermediate sealed horizon"
                 " intersect point\n");
                assert (0);
            }

            it1->pointid = -1;
            it1->pointid2 = -1;

        }

        if (itags[0].lineid == itags[npts-1].lineid) {
            if (itags[1].lineid != itags[0].lineid) {
                if (itags[1].pointid != 0) {
                    itags[0].lineid2 = itags[0].lineid;
                    itags[0].lineid = itags[1].lineid;
                    itags[npts-1].lineid2 = itags[0].lineid;
                }
                else {
                    itags[1].lineid2 = itags[0].lineid;
                    itags[0].lineid2 = -1;
                    itags[npts-1].lineid2 = -1;
                }
            }
        }

    /*
     * Break up this outline into individual intersect lines
     * based on a change in the tag lineid between 2 points.
     */
        start = -1;
        if (itags[0].lineid != itags[npts-1].lineid) {
            start = 0;
        }
        else {
            for (j=1;j<npts-1; j++) {
                if (itags[j].lineid != itags[0].lineid) {
                    start = j;
                    break;
                }
                if (itags[j].lineid2 != -1 &&
                    itags[j].lineid2 != itags[0].lineid) {
                    start = j;
                    break;
                }
            }
        }

        if (start == -1) {
            assert (0);
        }

    /*
     * Find the start and stop indices of each separate intersection
     * line within the outline.
     */
        n=0;
        n1[n] = start;
        olines[n] = itags[start+1].lineid;

        for (j=start+1; j<start+npts-1; j++) {
            k = j;
            if (j >= npts) {
                k = j - npts;
            }
            if (itags[k].lineid2 >= 0) {
                n2[n] = j;
                n++;
                n1[n] = j;
                olines[n] = itags[k].lineid2;
            }
            else if (itags[k].lineid != olines[n]) {
                if (itags[k].lineid2 == itags[n1[n]].lineid) {
                    n2[n] = j;
                }
                else {
                    int ichoice;
                    ichoice = ChooseSharedPoint (
                        x[k-1], y[k-1], z[k-1],
                        x[k], y[k], z[k],
                        olines[n], itags[k].lineid);
                    if (ichoice == 1) {
                        n2[n] = j - 1;
                    }
                    else {
                        n2[n] = j;
                    }
                }
                n++;
                if (n > 999) {
                    n = 999;
                }
                n1[n] = n2[n-1];
                olines[n] = itags[k].lineid;
            }
        }
        n2[n] = start + npts;
        n++;

    /*
     * Put the points between each start and stop into intersection
     * structures and add the structures to the sealed list.
     */
        nseg = 0;
        for (j=0; j<n; j++) {
            npts2 = n2[j] - n1[j] + 1;
            x2 = (double *)csw_Malloc (npts2 * 3 * sizeof(double));
            if (x2 == NULL) {
                return -1;
            }
            y2 = x2 + npts2;
            z2 = y2 + npts2;
            nout = 0;
            for (k=n1[j]; k<=n2[j]; k++) {
                kk = k;
                if (k >= npts) {
                    kk = k - npts;
                }
                if (kk == 0  &&  nout > 0) {
                    continue;
                }
                x2[nout] = x[kk];
                y2[nout] = y[kk];
                z2[nout] = z[kk];
                nout++;
            }
            istat =
              AddSealedTopAndBottomIntersect (
                  x2, y2, z2, nout, olines[j]);
            csw_Free (x2);
            x2 = y2 = z2 = NULL;
            npts2 = 0;
            if (istat == -1) {
                return -1;
            }
            iborder[nseg] = NumSealedTopAndBottomIntersects - 1;
            nseg++;
        }

    /*
     * Make the sealed border structure for this outline.
     */
        tmesh = PaddedSedimentSurface;
        sbord = &(tmesh->sealed_border);
        sbord->nlist = nseg;
        sbord->surfid = tmesh->id;
        for (j=0; j<nseg; j++) {
            sbord->intersection_id_list[j] = iborder[j];
            sbord->direction_list[j] = 1;
        }

        csw_Free (outline->x);
        csw_Free (outline->itags);
        csw_Free (outline);
        outline = NULL;

    }  /* end of padded sediment surface if block */


    if (PaddedModelBottom != NULL) {
        tmesh = PaddedModelBottom;
        outline = CalcSealedHorizonOutline (tmesh);
        if (outline == NULL) {
            return -1;
        }
        npts = outline->npts;
        itags = outline->itags;
        x = outline->x;
        y = outline->y;
        z = outline->z;

    /*
     * Make sure the first and last outline points have
     * valid line ids.
     */
        if (itags[0].lineid == -1) {
            for (j=1; j<npts-1; j++) {
                if (itags[j].lineid >= 0) {
                    itags[0].lineid = itags[j].lineid;
                    break;
                }
            }
            if (itags[npts-1].lineid >= 0) {
                itags[0].lineid2 = itags[npts-1].lineid;
            }
            itags[0].pointid = -1;
            itags[0].pointid2 = -1;
        }

        if (itags[npts-1].lineid == -1) {
            for (j=npts-2; j>0; j--) {
                if (itags[j].lineid >= 0) {
                    itags[npts-1].lineid = itags[j].lineid;
                    break;
                }
            }
            if (itags[0].lineid >= 0) {
                itags[npts-1].lineid2 = itags[0].lineid;
            }
            itags[npts-1].pointid = -1;
            itags[npts-1].pointid2 = -1;
        }

        if (itags[0].lineid == -1) {
            printf ("Error getting first point for sealed horizon intersects\n");
            assert (0);
        }
        if (itags[npts-1].lineid == -1) {
            printf ("Error getting last point for sealed horizon intersects\n");
            assert (0);
        }

    /*
     * Fill in line ids for invalid intermediate points.
     */
        for (j=1; j<npts-1; j++) {

            if (itags[j].lineid >= 0) {
                continue;
            }

            it1 = itags + j;
            it1->lineid = itags[j-1].lineid;
            it1->lineid2 = -1;
            for (k=j+1; k<npts; k++) {
                if (itags[k].lineid >= 0) {
                    it1->lineid2 = itags[k].lineid;
                    break;
                }
            }

            if (it1->lineid2 == -1) {
                printf
                ("Error getting lineid2 for intermediate sealed horizon"
                 " intersect point\n");
                assert (0);
            }

            it1->pointid = -1;
            it1->pointid2 = -1;

        }

        if (itags[0].lineid == itags[npts-1].lineid) {
            if (itags[1].lineid != itags[0].lineid) {
                if (itags[1].pointid != 0) {
                    itags[0].lineid2 = itags[0].lineid;
                    itags[0].lineid = itags[1].lineid;
                    itags[npts-1].lineid2 = itags[0].lineid;
                }
                else {
                    itags[1].lineid2 = itags[0].lineid;
                    itags[0].lineid2 = -1;
                    itags[npts-1].lineid2 = -1;
                }
            }
        }

    /*
     * Break up this outline into individual intersect lines
     * based on a change in the tag lineid between 2 points.
     */
        start = -1;
        if (itags[0].lineid != itags[npts-1].lineid) {
            start = 0;
        }
        else {
            for (j=1;j<npts-1; j++) {
                if (itags[j].lineid != itags[0].lineid) {
                    start = j;
                    break;
                }
                if (itags[j].lineid2 != -1 &&
                    itags[j].lineid2 != itags[0].lineid) {
                    start = j;
                    break;
                }
            }
        }

        if (start == -1) {
            assert (0);
        }

    /*
     * Find the start and stop indices of each separate intersection
     * line within the outline.
     */
        n=0;
        n1[n] = start;
        olines[n] = itags[start+1].lineid;

        for (j=start+1; j<start+npts-1; j++) {
            k = j;
            if (j >= npts) {
                k = j - npts;
            }
            if (itags[k].lineid2 >= 0) {
                n2[n] = j;
                n++;
                n1[n] = j;
                olines[n] = itags[k].lineid2;
            }
            else if (itags[k].lineid != olines[n]) {
                if (itags[k].lineid2 == itags[n1[n]].lineid) {
                    n2[n] = j;
                }
                else {
                    int ichoice;
                    ichoice = ChooseSharedPoint (
                        x[k-1], y[k-1], z[k-1],
                        x[k], y[k], z[k],
                        olines[n], itags[k].lineid);
                    if (ichoice == 1) {
                        n2[n] = j - 1;
                    }
                    else {
                        n2[n] = j;
                    }
                }
                n++;
                if (n > 999) {
                    n = 999;
                }
                n1[n] = n2[n-1];
                olines[n] = itags[k].lineid;
            }
        }
        n2[n] = start + npts;
        n++;

    /*
     * Put the points between each start and stop into intersection
     * structures and add the structures to the sealed list.
     */
        nseg = 0;
        for (j=0; j<n; j++) {
            npts2 = n2[j] - n1[j] + 1;
            x2 = (double *)csw_Malloc (npts2 * 3 * sizeof(double));
            if (x2 == NULL) {
                return -1;
            }
            y2 = x2 + npts2;
            z2 = y2 + npts2;
            nout = 0;
            for (k=n1[j]; k<=n2[j]; k++) {
                kk = k;
                if (k >= npts) {
                    kk = k - npts;
                }
                if (kk == 0  &&  nout > 0) {
                    continue;
                }
                x2[nout] = x[kk];
                y2[nout] = y[kk];
                z2[nout] = z[kk];
                nout++;
            }
            istat =
              AddSealedTopAndBottomIntersect (
                  x2, y2, z2, nout, olines[j]);
            csw_Free (x2);
            x2 = y2 = z2 = NULL;
            npts2 = 0;
            if (istat == -1) {
                return -1;
            }
            iborder[nseg] = NumSealedTopAndBottomIntersects - 1;
            nseg++;
        }

    /*
     * Make the sealed border structure for this outline.
     */
        tmesh = PaddedModelBottom;
        sbord = &(tmesh->sealed_border);
        sbord->nlist = nseg;
        sbord->surfid = tmesh->id;
        for (j=0; j<nseg; j++) {
            sbord->intersection_id_list[j] = iborder[j];
            sbord->direction_list[j] = 1;
        }

        csw_Free (outline->x);
        csw_Free (outline->itags);
        csw_Free (outline);
        outline = NULL;

    }  /* end of padded model bottom if block */

/*
 * If there is no sediment surface, calculate sealed intersect
 * lines for the default model top.
 */
    if (PaddedSedimentSurface == NULL  &&  PaddedTop != NULL) {
        tmesh = PaddedTop;
        outline = CalcSealedHorizonOutline (tmesh);
        if (outline == NULL) {
            return -1;
        }
        npts = outline->npts;
        itags = outline->itags;
        x = outline->x;
        y = outline->y;
        z = outline->z;

    /*
     * Make sure the first and last outline points have
     * valid line ids.
     */
        if (itags[0].lineid == -1) {
            for (j=1; j<npts-1; j++) {
                if (itags[j].lineid >= 0) {
                    itags[0].lineid = itags[j].lineid;
                    break;
                }
            }
            if (itags[npts-1].lineid >= 0) {
                itags[0].lineid2 = itags[npts-1].lineid;
            }
            itags[0].pointid = -1;
            itags[0].pointid2 = -1;
        }

        if (itags[npts-1].lineid == -1) {
            for (j=npts-2; j>0; j--) {
                if (itags[j].lineid >= 0) {
                    itags[npts-1].lineid = itags[j].lineid;
                    break;
                }
            }
            if (itags[0].lineid >= 0) {
                itags[npts-1].lineid2 = itags[0].lineid;
            }
            itags[npts-1].pointid = -1;
            itags[npts-1].pointid2 = -1;
        }

        if (itags[0].lineid == -1) {
            printf ("Error getting first point for sealed horizon intersects\n");
            assert (0);
        }
        if (itags[npts-1].lineid == -1) {
            printf ("Error getting last point for sealed horizon intersects\n");
            assert (0);
        }

    /*
     * Fill in line ids for invalid intermediate points.
     */
        for (j=1; j<npts-1; j++) {

            if (itags[j].lineid >= 0) {
                continue;
            }

            it1 = itags + j;
            it1->lineid = itags[j-1].lineid;
            it1->lineid2 = -1;
            for (k=j+1; k<npts; k++) {
                if (itags[k].lineid >= 0) {
                    it1->lineid2 = itags[k].lineid;
                    break;
                }
            }

            if (it1->lineid2 == -1) {
                printf
                ("Error getting lineid2 for intermediate sealed horizon"
                 " intersect point\n");
                assert (0);
            }

            it1->pointid = -1;
            it1->pointid2 = -1;

        }

        if (itags[0].lineid == itags[npts-1].lineid) {
            if (itags[1].lineid != itags[0].lineid) {
                if (itags[1].pointid != 0) {
                    itags[0].lineid2 = itags[0].lineid;
                    itags[0].lineid = itags[1].lineid;
                    itags[npts-1].lineid2 = itags[0].lineid;
                }
                else {
                    itags[1].lineid2 = itags[0].lineid;
                    itags[0].lineid2 = -1;
                    itags[npts-1].lineid2 = -1;
                }
            }
        }

    /*
     * Break up this outline into individual intersect lines
     * based on a change in the tag lineid between 2 points.
     */
        start = -1;
        if (itags[0].lineid != itags[npts-1].lineid) {
            start = 0;
        }
        else {
            for (j=1;j<npts-1; j++) {
                if (itags[j].lineid != itags[0].lineid) {
                    start = j;
                    break;
                }
                if (itags[j].lineid2 != -1 &&
                    itags[j].lineid2 != itags[0].lineid) {
                    start = j;
                    break;
                }
            }
        }

        if (start == -1) {
            assert (0);
        }

    /*
     * Find the start and stop indices of each separate intersection
     * line within the outline.
     */
        n=0;
        n1[n] = start;
        olines[n] = itags[start+1].lineid;

        for (j=start+1; j<start+npts-1; j++) {
            k = j;
            if (j >= npts) {
                k = j - npts;
            }
            if (itags[k].lineid2 >= 0) {
                n2[n] = j;
                n++;
                n1[n] = j;
                olines[n] = itags[k].lineid2;
            }
            else if (itags[k].lineid != olines[n]) {
                if (itags[k].lineid2 == itags[n1[n]].lineid) {
                    n2[n] = j;
                }
                else {
                    int ichoice;
                    ichoice = ChooseSharedPoint (
                        x[k-1], y[k-1], z[k-1],
                        x[k], y[k], z[k],
                        olines[n], itags[k].lineid);
                    if (ichoice == 1) {
                        n2[n] = j - 1;
                    }
                    else {
                        n2[n] = j;
                    }
                }
                n++;
                if (n > 999) {
                    n = 999;
                }
                n1[n] = n2[n-1];
                olines[n] = itags[k].lineid;
            }
        }
        n2[n] = start + npts;
        n++;

    /*
     * Put the points between each start and stop into intersection
     * structures and add the structures to the sealed list.
     */
        nseg = 0;
        for (j=0; j<n; j++) {
            npts2 = n2[j] - n1[j] + 1;
            x2 = (double *)csw_Malloc (npts2 * 3 * sizeof(double));
            if (x2 == NULL) {
                return -1;
            }
            y2 = x2 + npts2;
            z2 = y2 + npts2;
            nout = 0;
            for (k=n1[j]; k<=n2[j]; k++) {
                kk = k;
                if (k >= npts) {
                    kk = k - npts;
                }
                if (kk == 0  &&  nout > 0) {
                    continue;
                }
                x2[nout] = x[kk];
                y2[nout] = y[kk];
                z2[nout] = z[kk];
                nout++;
            }
            istat =
              AddSealedTopAndBottomIntersect (
                  x2, y2, z2, nout, olines[j]);
            csw_Free (x2);
            x2 = y2 = z2 = NULL;
            npts2 = 0;
            if (istat == -1) {
                return -1;
            }
            iborder[nseg] = NumSealedTopAndBottomIntersects - 1;
            nseg++;
        }

    /*
     * Make the sealed border structure for this outline.
     */
        tmesh = PaddedTop;
        sbord = &(tmesh->sealed_border);
        sbord->nlist = nseg;
        sbord->surfid = tmesh->id;
        for (j=0; j<nseg; j++) {
            sbord->intersection_id_list[j] = iborder[j];
            sbord->direction_list[j] = 1;
        }

        csw_Free (outline->x);
        csw_Free (outline->itags);
        csw_Free (outline);
        outline = NULL;

    }  /* end of padded sediment surface if block */


    if (PaddedBottom != NULL) {
        tmesh = PaddedBottom;
        outline = CalcSealedHorizonOutline (tmesh);
        if (outline == NULL) {
            return -1;
        }
        npts = outline->npts;
        itags = outline->itags;
        x = outline->x;
        y = outline->y;
        z = outline->z;

    /*
     * Make sure the first and last outline points have
     * valid line ids.
     */
        if (itags[0].lineid == -1) {
            for (j=1; j<npts-1; j++) {
                if (itags[j].lineid >= 0) {
                    itags[0].lineid = itags[j].lineid;
                    break;
                }
            }
            if (itags[npts-1].lineid >= 0) {
                itags[0].lineid2 = itags[npts-1].lineid;
            }
            itags[0].pointid = -1;
            itags[0].pointid2 = -1;
        }

        if (itags[npts-1].lineid == -1) {
            for (j=npts-2; j>0; j--) {
                if (itags[j].lineid >= 0) {
                    itags[npts-1].lineid = itags[j].lineid;
                    break;
                }
            }
            if (itags[0].lineid >= 0) {
                itags[npts-1].lineid2 = itags[0].lineid;
            }
            itags[npts-1].pointid = -1;
            itags[npts-1].pointid2 = -1;
        }

        if (itags[0].lineid == -1) {
            printf ("Error getting first point for sealed horizon intersects\n");
            assert (0);
        }
        if (itags[npts-1].lineid == -1) {
            printf ("Error getting last point for sealed horizon intersects\n");
            assert (0);
        }

    /*
     * Fill in line ids for invalid intermediate points.
     */
        for (j=1; j<npts-1; j++) {

            if (itags[j].lineid >= 0) {
                continue;
            }

            it1 = itags + j;
            it1->lineid = itags[j-1].lineid;
            it1->lineid2 = -1;
            for (k=j+1; k<npts; k++) {
                if (itags[k].lineid >= 0) {
                    it1->lineid2 = itags[k].lineid;
                    break;
                }
            }

            if (it1->lineid2 == -1) {
                printf
                ("Error getting lineid2 for intermediate sealed horizon"
                 " intersect point\n");
                assert (0);
            }

            it1->pointid = -1;
            it1->pointid2 = -1;

        }

        if (itags[0].lineid == itags[npts-1].lineid) {
            if (itags[1].lineid != itags[0].lineid) {
                if (itags[1].pointid != 0) {
                    itags[0].lineid2 = itags[0].lineid;
                    itags[0].lineid = itags[1].lineid;
                    itags[npts-1].lineid2 = itags[0].lineid;
                }
                else {
                    itags[1].lineid2 = itags[0].lineid;
                    itags[0].lineid2 = -1;
                    itags[npts-1].lineid2 = -1;
                }
            }
        }

    /*
     * Break up this outline into individual intersect lines
     * based on a change in the tag lineid between 2 points.
     */
        start = -1;
        if (itags[0].lineid != itags[npts-1].lineid) {
            start = 0;
        }
        else {
            for (j=1;j<npts-1; j++) {
                if (itags[j].lineid != itags[0].lineid) {
                    start = j;
                    break;
                }
                if (itags[j].lineid2 != -1 &&
                    itags[j].lineid2 != itags[0].lineid) {
                    start = j;
                    break;
                }
            }
        }

        if (start == -1) {
            assert (0);
        }

    /*
     * Find the start and stop indices of each separate intersection
     * line within the outline.
     */
        n=0;
        n1[n] = start;
        olines[n] = itags[start+1].lineid;

        for (j=start+1; j<start+npts-1; j++) {
            k = j;
            if (j >= npts) {
                k = j - npts;
            }
            if (itags[k].lineid2 >= 0) {
                n2[n] = j;
                n++;
                n1[n] = j;
                olines[n] = itags[k].lineid2;
            }
            else if (itags[k].lineid != olines[n]) {
                if (itags[k].lineid2 == itags[n1[n]].lineid) {
                    n2[n] = j;
                }
                else {
                    int ichoice;
                    ichoice = ChooseSharedPoint (
                        x[k-1], y[k-1], z[k-1],
                        x[k], y[k], z[k],
                        olines[n], itags[k].lineid);
                    if (ichoice == 1) {
                        n2[n] = j - 1;
                    }
                    else {
                        n2[n] = j;
                    }
                }
                n++;
                if (n > 999) {
                    n = 999;
                }
                n1[n] = n2[n-1];
                olines[n] = itags[k].lineid;
            }
        }
        n2[n] = start + npts;
        n++;

    /*
     * Put the points between each start and stop into intersection
     * structures and add the structures to the sealed list.
     */
        nseg = 0;
        for (j=0; j<n; j++) {
            npts2 = n2[j] - n1[j] + 1;
            x2 = (double *)csw_Malloc (npts2 * 3 * sizeof(double));
            if (x2 == NULL) {
                return -1;
            }
            y2 = x2 + npts2;
            z2 = y2 + npts2;
            nout = 0;
            for (k=n1[j]; k<=n2[j]; k++) {
                kk = k;
                if (k >= npts) {
                    kk = k - npts;
                }
                if (kk == 0  &&  nout > 0) {
                    continue;
                }
                x2[nout] = x[kk];
                y2[nout] = y[kk];
                z2[nout] = z[kk];
                nout++;
            }
            istat =
              AddSealedTopAndBottomIntersect (
                  x2, y2, z2, nout, olines[j]);
            csw_Free (x2);
            x2 = y2 = z2 = NULL;
            npts2 = 0;
            if (istat == -1) {
                return -1;
            }
            iborder[nseg] = NumSealedTopAndBottomIntersects - 1;
            nseg++;
        }

    /*
     * Make the sealed border structure for this outline.
     */
        tmesh = PaddedBottom;
        sbord = &(tmesh->sealed_border);
        sbord->nlist = nseg;
        sbord->surfid = tmesh->id;
        for (j=0; j<nseg; j++) {
            sbord->intersection_id_list[j] = iborder[j];
            sbord->direction_list[j] = 1;
        }

        csw_Free (outline->x);
        csw_Free (outline->itags);
        csw_Free (outline);
        outline = NULL;

    }  /* end of padded bottom if block */

    return 1;

}




/*-----------------------------------------------------------------------------------*/

_OUtline_ *SealedModel::BuildOutlineFromSealedTopAndBottomBorder (_SEaledBorder_ *sbord)
{
    int             i, j, idir, start;
    int             npts, npts2, ntot;
    double          *x = NULL, *y = NULL, *z = NULL,
                    *x2 = NULL, *y2 = NULL, *z2 = NULL;
    _INtersectionLine_  *iptr = NULL;
    _OUtline_           *outline = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (x2);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (SealedTopAndBottomIntersects == NULL) {
        return NULL;
    }

    ntot = 0;
    for (i=0; i<sbord->nlist; i++) {
        iptr = SealedTopAndBottomIntersects + sbord->intersection_id_list[i];
        ntot += iptr->npts;
    }

    x2 = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (x2 == NULL) {
        return NULL;
    }
    y2 = x2 + ntot;
    z2 = y2 + ntot;

    npts2 = 0;
    for (i=0; i<sbord->nlist; i++) {
        iptr = SealedTopAndBottomIntersects + sbord->intersection_id_list[i];
        x = iptr->x;
        y = iptr->y;
        z = iptr->z;
        npts = iptr->npts;
        idir = sbord->direction_list[i];

        if (idir == 1) {
            start = 0;
            if (i > 0) {
                start = 1;
            }
            for (j=start; j<npts; j++) {
                x2[npts2] = x[j];
                y2[npts2] = y[j];
                z2[npts2] = z[j];
                npts2++;
            }
        }
        else {
            start = npts - 1;
            if (i > 0) {
                start = npts - 2;
            }
            for (j=start; j>=0; j--) {
                x2[npts2] = x[j];
                y2[npts2] = y[j];
                z2[npts2] = z[j];
                npts2++;
            }
        }
    }

    outline = (_OUtline_ *)csw_Calloc (sizeof(_OUtline_));
    if (outline == NULL) {
        return NULL;
    }

    outline->x = x2;
    outline->y = y2;
    outline->z = z2;
    outline->npts = npts2;
    outline->surfid = sbord->surfid;

    bsuccess = true;

    return outline;

}


/*---------------------------------------------------------------------------*/

/*
 * Free the memory used to create tetgen input data.  This
 * is called from the destructor and also to reset everything
 * at the start of the createTetgenInput method.
 */
void SealedModel::FreeTetgenData (void)
{

    csw_Free (TetgenX);
    csw_Free (TetgenY);
    csw_Free (TetgenZ);
    csw_Free (TetgenNodeMark);
    TetgenX = NULL;
    TetgenY = NULL;
    TetgenZ = NULL;
    TetgenNodeMark = NULL;
    NumTetgenNodes = 0;
    MaxTetgenNodes = 0;

    csw_Free (TetgenNode1);
    csw_Free (TetgenNode2);
    csw_Free (TetgenNode3);
    csw_Free (TetgenFacetMark);
    TetgenNode1 = NULL;
    TetgenNode2 = NULL;
    TetgenNode3 = NULL;
    TetgenFacetMark = NULL;
    NumTetgenFacets = 0;
    MaxTetgenFacets = 0;

    return;

}


/*------------------------------------------------------------------------*/

/*
 * Add a node to the tetgen input arrays, growing the arrays
 * as needed.  If this succeeds, the index of the added node
 * is returned.  On a memory allocation failure, -1 is returned.
 */
int SealedModel::AddTetgenNode (
    double     x,
    double     y,
    double     z,
    int        mark)
{

/*
 * grow the arrays if needed.
 */
    if (TetgenX == NULL  ||
        NumTetgenNodes >= MaxTetgenNodes) {
        MaxTetgenNodes += 200;
        TetgenX = (double *)csw_Realloc (
            TetgenX, MaxTetgenNodes * sizeof(double));
        TetgenY = (double *)csw_Realloc (
            TetgenY, MaxTetgenNodes * sizeof(double));
        TetgenZ = (double *)csw_Realloc (
            TetgenZ, MaxTetgenNodes * sizeof(double));
        TetgenNodeMark = (int *)csw_Realloc (
            TetgenNodeMark, MaxTetgenNodes * sizeof(int));
    }

    if (TetgenX == NULL  ||
        TetgenY == NULL  ||
        TetgenZ == NULL  ||
        TetgenNodeMark == NULL) {
        FreeTetgenData ();
        return -1;
    }

/*
 * Add the new point.
 */
    TetgenX[NumTetgenNodes] = x;
    TetgenY[NumTetgenNodes] = y;
    TetgenZ[NumTetgenNodes] = z;
    TetgenNodeMark[NumTetgenNodes] = mark;

    NumTetgenNodes++;

    return (NumTetgenNodes - 1);

}

/*--------------------------------------------------------------------*/

#define FREE_TETGEN_RETURN_DATA \
    csw_Free (*nodex); *nodex = NULL;\
    csw_Free (*nodey); *nodey = NULL;\
    csw_Free (*nodez); *nodez = NULL;\
    csw_Free (*nodemarks); *nodemarks = NULL;\
    csw_Free (*facetnode1); *facetnode1 = NULL;\
    csw_Free (*facetnode2); *facetnode2 = NULL;\
    csw_Free (*facetnode3); *facetnode3 = NULL;\
    csw_Free (*facetmarks); *facetmarks = NULL;

/*
 * Create node and facet lists for tetgen input from the
 * sealed model surfaces.  If the model hasn't been sealed
 * via sealPaddedModel, this method fails and returns zero.
 *
 * If the model does not have sealed horizons, sealed faults
 * (which include the vertical boundaries), a sealed sediment
 * surface and a sealed model bottom, zero is returned.
 *
 * On memory allocation failure, the return value is -1.
 * In any failure case, the return data pointers are NUll.
 *
 * On success, the node and facet data are allocated here and
 * filled in.  Ownership of these data is relinquished to the
 * calling function, which must csw_Free the data when appropriate.
 */
int SealedModel::createTetgenInput (
    double   **nodex,
    double   **nodey,
    double   **nodez,
    int      **nodemarks,
    int      *numnodes,
    int      **facetnode1,
    int      **facetnode2,
    int      **facetnode3,
    int      **facetmarks,
    int      *numfacets)
{
    XYZIndex3D    *index3d = NULL;
    CSWTriMeshStruct   *tmesh = NULL;
    NOdeStruct    *nodes = NULL;
    EDgeStruct    *edges = NULL;
    TRiangleStruct  *tris = NULL, *tptr = NULL;
    double        xt, yt, zt, xt2, yt2, zt2,
                  dx, dy, dz, dist, tiny;
    int           n1, n2, n3, found, nlist, istat,
                  i, j, k, nnodes, ntris;
    int           *nodelookup = NULL;
    int           list[1000];

    EDgeStruct    *ep1 = NULL, *ep2 = NULL, *ep3 = NULL, *eptr = NULL;

    double     *xp = NULL, *yp = NULL, *zp = NULL;
    int        *ip = NULL, isize, dsize;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        FreeTetgenData ();
        csw_Free (nodelookup);
        csw_Free (xp);
        csw_Free (yp);
        csw_Free (zp);
        csw_Free (ip);
        delete index3d;
        if (bsuccess == false) {
            FREE_TETGEN_RETURN_DATA
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of error.
 */
    *nodex = NULL;
    *nodey = NULL;
    *nodez = NULL;
    *nodemarks = NULL;
    *numnodes = 0;
    *facetnode1 = NULL;
    *facetnode2 = NULL;
    *facetnode3 = NULL;
    *facetmarks = NULL;
    *numfacets = 0;

    if (simSealFlag == 0) {
        return 0;
    }

    if (SealedHorizonList == NULL  ||
        SealedFaultList == NULL  ||
        SealedSedimentSurface == NULL  ||
        SealedBottom == NULL) {
        return 0;
    }

/*
 * Create a 3d spatial index for the nodes.
 */
    double      xytiny, ztiny;
    xytiny = (padXmax - padXmin + padYmax - padYmin) / 200.0;
    ztiny = (padZmax - padZmin) / 100.0;
    index3d =
      new XYZIndex3D (
        padXmin - xytiny,
        padYmin - xytiny,
        padZmin - ztiny,
        padXmax + xytiny,
        padYmax + xytiny,
        padZmax + ztiny);

/*
 * Clean up tetgen data if needed.
 */
    FreeTetgenData ();

    tiny = modelTiny;

/*
 * Put the nodes and triangles from the sealed horizons
 * into the tetgen arrays.  The sealed sediment surface
 * and sealed model bottom are also done here.
 */
    int    boundaryFlag = 0;
    for (i=0; i<NumSealedHorizonList + 3; i++) {

        if (i < NumSealedHorizonList) {
            tmesh = SealedHorizonList + i;
        }
        else if (i == NumSealedHorizonList) {
            tmesh = SealedSedimentSurface;
        }
        else if (i == NumSealedHorizonList + 1) {
            tmesh = SealedModelBottom;
        }
        else {
            tmesh = SealedBottom;
        }

        if (tmesh == NULL) {
            continue;
        }

        nodes = tmesh->nodes;
        nnodes = tmesh->num_nodes;
        nodelookup = (int *)csw_Malloc (nnodes * sizeof(int));
        if (nodelookup == NULL) {
            return -1;
        }

    /*
     * For each node in the horizon, see if another coincident
     * node has already been added to the tetgen data.  If no
     * previous coincident node exists, add the node to the tetgen
     * data and set the nodelookup to the new node.  If a previous
     * coincident node does exist, use the index of the previous
     * tetgen node in the nodelookup.
     */
        for (j=0; j<nnodes; j++) {
            xt = nodes[j].x;
            yt = nodes[j].y;
            zt = nodes[j].z;
            istat = index3d->GetClosePoints (
                    xt, yt, zt,
                    list, &nlist, 1000);
            if (istat != 1  ||  nlist < 1) {
                istat = index3d->AddPoint (xt, yt, zt);
                if (istat == -1) {
                    return -1;
                }
                istat = AddTetgenNode (xt, yt, zt, i);
                if (istat == -1) {
                    return -1;
                }
                nodelookup[j] = istat;
                continue;
            }
            else {
                found = -1;
                for (k=0; k<nlist; k++) {
                    istat = index3d->GetPointXYZ (
                        list[k], &xt2, &yt2, &zt2);
                    if (istat != 1) {
                        continue;
                    }
                    dx = xt - xt2;
                    dy = yt - yt2;
                    dz = zt - zt2;
                    dist = dx * dx + dy * dy + dz * dz;
                    dist = sqrt (dist);
                    if (dist <= tiny) {
                        found = list[k];
                        break;
                    }
                }
                if (found >= 0) {
                    nodelookup[j] = found;
                    continue;
                }
                istat = index3d->AddPoint (xt, yt, zt);
                if (istat == -1) {
                    return -1;
                }
                istat = AddTetgenNode (xt, yt, zt, i);
                if (istat == -1) {
                    return -1;
                }
                nodelookup[j] = istat;
            }
        }

    /*
     * Loop through the horizon's triangles and put the tetgen
     * node numbers corresponding to the triangle corners into
     * the tetgen facet arrays.
     */
        ntris = tmesh->num_tris;
        tris = tmesh->tris;
        edges = tmesh->edges;

        for (j=0; j<ntris; j++) {

            tptr = tris + j;
            istat = grd_api_obj.grd_GetNodesForTriangle (
                tptr, edges,
                &n1, &n2, &n3);
            if (istat == -1) {
                return -1;
            }

            n1 = nodelookup[n1];
            n2 = nodelookup[n2];
            n3 = nodelookup[n3];

            istat = AddTetgenFacet (n1, n2, n3, boundaryFlag);
            if (istat == -1) {
                return -1;
            }

        }

        csw_Free (nodelookup);
        nodelookup = NULL;

    }  /* end of tetgen data from horizons */

/*
 * Put the nodes and triangles from the sealed faults
 * and vertical boundaries into the tetgen arrays.
 */
    int           marker_base;
    for (i=0; i<NumSealedFaultList; i++) {

        tmesh = SealedFaultList + i;

        for (j=0; j<tmesh->num_edges; j++) {
          eptr = tmesh->edges + j;
          if (eptr->isconstraint == 1) {
            if (eptr->flag != LIMIT_LINE_FLAG) {
              eptr->isconstraint = 0;
            }
          }
        }

        nodes = tmesh->nodes;
        nnodes = tmesh->num_nodes;
        nodelookup = (int *)csw_Malloc (nnodes * sizeof(int));
        if (nodelookup == NULL) {
            return -1;
        }

        marker_base = 0;
        if (i < NumSealedFaultList - 4) {
            marker_base = 10000;
        }

    /*
     * For each node in the fault, see if another coincident
     * node has already been added to the tetgen data.  If no
     * previous coincident node exists, add the node to the tetgen
     * data and set the nodelookup to the new node.  If a previous
     * coincident node does exist, use the index of the previous
     * tetgen node in the nodelookup.
     */
        for (j=0; j<nnodes; j++) {
            xt = nodes[j].x;
            yt = nodes[j].y;
            zt = nodes[j].z;
            istat = index3d->GetClosePoints (
                    xt, yt, zt,
                    list, &nlist, 1000);
            if (istat != 1  ||  nlist < 1) {
                istat = index3d->AddPoint (xt, yt, zt);
                if (istat == -1) {
                    return -1;
                }
                istat = AddTetgenNode (xt, yt, zt, 0);
                if (istat == -1) {
                    return -1;
                }
                nodelookup[j] = istat;
                continue;
            }
            else {
                found = -1;
                for (k=0; k<nlist; k++) {
                    istat = index3d->GetPointXYZ (
                        list[k], &xt2, &yt2, &zt2);
                    if (istat != 1) {
                        continue;
                    }
                    dx = xt - xt2;
                    dy = yt - yt2;
                    dz = zt - zt2;
                    dist = dx * dx + dy * dy + dz * dz;
                    dist = sqrt (dist);
                    if (dist <= tiny) {
                        found = list[k];
                        break;
                    }
                }
                if (found >= 0) {
                    nodelookup[j] = found;
                    continue;
                }
                istat = index3d->AddPoint (xt, yt, zt);
                if (istat == -1) {
                    return -1;
                }
                istat = AddTetgenNode (xt, yt, zt, 0);
                if (istat == -1) {
                    return -1;
                }
                nodelookup[j] = istat;
            }
        }

    /*
     * Loop through the fault's triangles and put the tetgen
     * node numbers corresponding to the triangle corners into
     * the tetgen facet arrays.
     */
        ntris = tmesh->num_tris;
        tris = tmesh->tris;
        edges = tmesh->edges;

        for (j=0; j<ntris; j++) {

            tptr = tris + j;
            istat = grd_api_obj.grd_GetNodesForTriangle (
                tptr, edges,
                &n1, &n2, &n3);
            if (istat == -1) {
                return -1;
            }

            boundaryFlag = 0;
            if (tmesh->sealed_to_sides == 0  &&  marker_base > 0) {
                ep1 = edges + tptr->edge1;
                ep2 = edges + tptr->edge2;
                ep3 = edges + tptr->edge3;
                if ((ep1->tri2 == -1  &&  ep1->isconstraint == 0)  ||
                    (ep2->tri2 == -1  &&  ep2->isconstraint == 0)  ||
                    (ep3->tri2 == -1  &&  ep3->isconstraint == 0)) {
                    boundaryFlag = TETGEN_INSIDE_EDGE_FLAG;
                }
            }

            n1 = nodelookup[n1];
            n2 = nodelookup[n2];
            n3 = nodelookup[n3];

            istat = AddTetgenFacet (n1, n2, n3, boundaryFlag);
            if (istat == -1) {
                return -1;
            }

        }

        csw_Free (nodelookup);
        nodelookup = NULL;

        for (j=0; j<tmesh->num_edges; j++) {
          eptr = tmesh->edges + j;
          if (eptr->flag != 0) {
            eptr->isconstraint = 1;
          }
        }

    }  /* end of tetgen data from faults */

    delete index3d;
    index3d = NULL;

    if (NumTetgenNodes < 4  ||  NumTetgenFacets < 4) {
        return -1;
    }

    if (TetgenX == NULL  ||
        TetgenY == NULL  ||
        TetgenZ == NULL  ||
        TetgenNodeMark == NULL  ||
        TetgenNode1 == NULL  ||
        TetgenNode2 == NULL  ||
        TetgenNode3 == NULL  ||
        TetgenFacetMark == NULL) {
        return -1;
    }

/*
 * Allocate the output arrays and copy the tetgen data
 * into them.
 */

/*
 * Output node data.
 */
    isize = NumTetgenNodes * sizeof(int);
    dsize = NumTetgenNodes * sizeof(double);

    xp = (double *)csw_Malloc (dsize);
    if (xp == NULL) {
        return -1;
    }
    memcpy (xp, TetgenX, dsize);
    *nodex = xp;
    xp = NULL;

    yp = (double *)csw_Malloc (dsize);
    if (yp == NULL) {
        return -1;
    }
    memcpy (yp, TetgenY, dsize);
    *nodey = yp;
    yp = NULL;

    zp = (double *)csw_Malloc (dsize);
    if (zp == NULL) {
        return -1;
    }
    memcpy (zp, TetgenZ, dsize);
    *nodez = zp;
    zp = NULL;

    ip = (int *)csw_Malloc (isize);
    if (ip == NULL) {
        return -1;
    }
    memcpy (ip, TetgenNodeMark, isize);
    *nodemarks = ip;
    ip = NULL;

/*
 * Output facet data.
 */
    isize = NumTetgenFacets * sizeof(int);

    ip = (int *)csw_Malloc (isize);
    if (ip == NULL) {
        return -1;
    }
    memcpy (ip, TetgenNode1, isize);
    *facetnode1 = ip;
    ip = NULL;

    ip = (int *)csw_Malloc (isize);
    if (ip == NULL) {
        return -1;
    }
    memcpy (ip, TetgenNode2, isize);
    *facetnode2 = ip;
    ip = NULL;

    ip = (int *)csw_Malloc (isize);
    if (ip == NULL) {
        return -1;
    }
    memcpy (ip, TetgenNode3, isize);
    *facetnode3 = ip;
    ip = NULL;

    ip = (int *)csw_Malloc (isize);
    if (ip == NULL) {
        return -1;
    }
    memcpy (ip, TetgenFacetMark, isize);
    *facetmarks = ip;
    ip = NULL;

    *numnodes = NumTetgenNodes;
    *numfacets = NumTetgenFacets;

    bsuccess = true;

    return 1;

}

#undef FREE_TETGEN_RETURN_DATA




/*------------------------------------------------------------------------*/

/*
 * Add a facet to the tetgen input arrays, growing the arrays
 * as needed.  If this succeeds, the index of the added facet
 * is returned.  On a memory allocation failure, -1 is returned.
 */
int SealedModel::AddTetgenFacet (
    int        n1,
    int        n2,
    int        n3,
    int        mark)
{

/*
 * grow the arrays if needed.facet
 */
    if (TetgenNode1 == NULL  ||
        NumTetgenFacets >= MaxTetgenFacets) {
        MaxTetgenFacets += 200;
        TetgenNode1 = (int *)csw_Realloc (
            TetgenNode1, MaxTetgenFacets * sizeof(int));
        TetgenNode2 = (int *)csw_Realloc (
            TetgenNode2, MaxTetgenFacets * sizeof(int));
        TetgenNode3 = (int *)csw_Realloc (
            TetgenNode3, MaxTetgenFacets * sizeof(int));
        TetgenFacetMark = (int *)csw_Realloc (
            TetgenFacetMark, MaxTetgenFacets * sizeof(int));
    }

    if (TetgenNode1 == NULL  ||
        TetgenNode2 == NULL  ||
        TetgenNode3 == NULL  ||
        TetgenFacetMark == NULL) {
        FreeTetgenData ();
        return -1;
    }

/*
 * Add the new point.
 */
    TetgenNode1[NumTetgenFacets] = n1;
    TetgenNode2[NumTetgenFacets] = n2;
    TetgenNode3[NumTetgenFacets] = n3;
    TetgenFacetMark[NumTetgenFacets] = mark;

    NumTetgenFacets++;

    return (NumTetgenFacets - 1);

}



/*---------------------------------------------------------------------*/

/*
 * Write the tetgen nodes and facets to a .smesh file.
 */
int SealedModel::writeTetgenSmeshFile (char const *pathname)
{
    int         istat;
    double      *nodex, *nodey, *nodez;
    int         *nodemarks, numnodes;
    int         *facetnode1, *facetnode2, *facetnode3;
    int         *facetmarks;
    int         numfacets;

    if (pathname == NULL) {
        return -1;
    }

    istat =
      createTetgenInput (
        &nodex, &nodey, &nodez,
        &nodemarks, &numnodes,
        &facetnode1, &facetnode2, &facetnode3,
        &facetmarks, &numfacets);

    if (istat != 1) {
        return -1;
    }

    FILE        *fptr;
    int         i, len;
    char        fname[500], c500[500], *ctmp;

    len = strlen (pathname);
    if (len >= 494) {
        return -1;
    }

    strcpy (fname, pathname);
    csw_StrLeftJust (fname);

    ctmp = strrchr (fname, '.');
    if (ctmp == NULL) {
        strcat (fname, ".smesh");
    }
    else {
        *ctmp = '\0';
        strcat (fname, ".smesh");
    }

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return -1;
    }

/*
 * Write the nodes to the file.
 */
    sprintf (c500, "#\n#Start of node list.\n#\n");
    fputs (c500, fptr);
    sprintf (c500, "%d 3 1 0\n", numnodes);
    fputs (c500, fptr);

    for (i=0; i<numnodes; i++) {
        sprintf (c500, "%d %f %f %f %d\n",
                 i, nodex[i], nodey[i], nodez[i],
                 nodemarks[i]);
        fputs (c500, fptr);
    }

/*
 * Write the facets to the file.
 */
    sprintf (c500, "#\n#Start of facet list.\n#\n");
    fputs (c500, fptr);
    sprintf (c500, "%d 1\n", numfacets);
    fputs (c500, fptr);

    for (i=0; i<numfacets; i++) {
        sprintf (c500, "3 %d %d %d %d\n",
                 facetnode1[i],
                 facetnode2[i],
                 facetnode3[i],
                 facetmarks[i]);
        fputs (c500, fptr);
    }

/*
 * There are no holes in the model.
 */
    sprintf (c500, "#\n#Start of hole list.\n#\n");
    fputs (c500, fptr);
    sprintf (c500, "0\n");
    fputs (c500, fptr);

/*
 * Write the region points to the file.
 */
    sprintf (c500, "#\n#Start of region list.\n#\n");
    fputs (c500, fptr);
    sprintf (c500, "0\n");
    fputs (c500, fptr);

    fclose (fptr);
    fptr = NULL;

    return 1;

}


/*------------------------------------------------------------------------*/

/*
 * Move point locations on the second line if they are relatively
 * close to the master line so that they are exactly on a master
 * line point.
 */
int SealedModel::SnapLineToMaster (
    double    *xmaster,
    double    *ymaster,
    double    *zmaster,
    int       nmaster,
    double    *xline,
    double    *yline,
    double    *zline,
    int       *nlineio)
{
    int       i, j, jmin, n, nline, nmax;
    double    xt, yt, zt, xt2, yt2,
              dx, dy, dist, dcrit, pdist, dmin;
    double    d1, d2, dcrit2;
    double    *xwork = NULL, *ywork = NULL, *zwork = NULL;
    int       *iwork = NULL;


    auto fscope = [&]()
    {
        csw_Free (xwork);
        csw_Free (iwork);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Allocate work space.
 */
    nmax = *nlineio;
    if (nmaster > nmax) nmax = nmaster;

    xwork = (double *)csw_Malloc (nmax * 3 * sizeof(double));
    if (xwork == NULL) {
        return -1;
    }
    ywork = xwork + nmax;
    zwork = ywork + nmax;

    iwork = (int *)csw_Calloc (nmax * sizeof(int));
    if (iwork == NULL) {
        return -1;
    }

/*
 * For each point on the line, check if it is close to the master line
 * and if it is make it exactly on the nearest master line point.
 * No master line point is used more than once.  If movement to the
 * nearest unused master point is too far, the line point is discarded.
 */
    n = 0;
    nline = *nlineio;

    for (i=0; i<nline; i++) {

        xt = xline[i];
        yt = yline[i];
        zt = zline[i];

      /*
       * The critical distance to the master line is based
       * on the average distance to neighbor points along the line.
       */
        d1 = 1.e30;
        d2 = 1.e30;
        if (i > 0) {
            dx = xt - xline[i-1];
            dy = yt - yline[i-1];
            d1 = dx * dx + dy * dy;
            d1 = sqrt (d1);
        }
        if (i < nline-1) {
            dx = xt - xline[i+1];
            dy = yt - yline[i+1];
            d2 = dx * dx + dy * dy;
            d2 = sqrt (d2);
        }
        if (d1 > 1.e20  &&  d2 > 1.e20) {
            assert (0);
        }
        if (d1 > 1.e20) {
            dcrit = d2;
        }
        else if (d2 > 1.e20) {
            dcrit = d1;
        }
        else {
            dcrit = (d1 + d2) / 2.0;
        }
        dcrit2 = dcrit;
        dcrit /= 4.0;

        if (simOutputFlag == 0) {
            dcrit /= 10.0;
        }

      /*
       * Calculate the distance to the master line.
       */
        gpf_calclinedist2xy (
            xmaster, ymaster, nmaster,
            xt, yt, &pdist);

      /*
       * Line point is far enough away, so don't change it.
       */
        if (pdist >= dcrit) {
            xwork[n] = xt;
            ywork[n] = yt;
            zwork[n] = zt;
            n++;
            continue;
        }

      /*
       * Find the closest unused master line point.
       */
        dmin = 1.e30;
        jmin = -1;
        for (j=0; j<nmaster; j++) {
            if (iwork[j] == 1) {
                continue;
            }
            xt2 = xmaster[j];
            yt2 = ymaster[j];
            dx = xt - xt2;
            dy = yt - yt2;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist < dmin) {
                dmin = dist;
                jmin = j;
            }
        }

      /*
       * Discard the line point if no master point was found
       * or if the closest unused master point is too far to move to.
       */
        if (jmin == -1) {
            continue;
        }
        if (dmin > dcrit2) {
            continue;
        }

      /*
       * Put the close master point into the work arrays
       * and flag the master point as used.
       */
        xwork[n] = xmaster[jmin];
        ywork[n] = ymaster[jmin];
        zwork[n] = zmaster[jmin];
        n++;
        iwork[jmin] = 1;

    }

    memcpy (xline, xwork, n * sizeof(double));
    memcpy (yline, ywork, n * sizeof(double));
    memcpy (zline, zwork, n * sizeof(double));
    *nlineio = n;

    return 1;

}


/*---------------------------------------------------------------------------------*/

/*
 * If a sealed horizon intersect point is very close to a sealed fault
 * intersect point, move the fault point to the horizon point.
 *
 * This is a protected method.
 */
int SealedModel::SnapSealedFaultPointsToSealedHorizonPoints (void)
{
    double       xt, yt, zt;
    double       dx, dy, dz, dcrit, d1, d2;
    double       *xa1, *ya1, *za1;
    int          i, j, na1, istat;
    _INtersectionLine_  *fptr;

    if (SealedFaultIntersects == NULL) {
        return -1;
    }

    for (i=0; i<NumSealedFaultIntersects; i++) {
        fptr = SealedFaultIntersects + i;
        xa1 = fptr->x;
        ya1 = fptr->y;
        za1 = fptr->z;
        na1 = fptr->npts;
        for (j=0; j<na1; j++) {
            if (j == 0  &&  fptr->splice_partner_1 >= 0) {
                continue;
            }
            if (j == na1 - 1  &&  fptr->splice_partner_2 >= 0) {
                continue;
            }
            xt = xa1[j];
            yt = ya1[j];
            zt = za1[j];
          /*
           * The critical distance to a horizon intersect point
           * is based on the average distance to neighbor points
           * on the fault intersect line.
           */
            d1 = 1.e30;
            d2 = 1.e30;
            if (j > 0) {
                dx = xt - xa1[j-1];
                dy = yt - ya1[j-1];
                dz = zt - za1[j-1];
                d1 = dx * dx + dy * dy + dz * dz;
                d1 = sqrt (d1);
            }
            if (j < na1-1) {
                dx = xt - xa1[j+1];
                dy = yt - ya1[j+1];
                dz = zt - za1[j+1];
                d2 = dx * dx + dy * dy + dz * dz;
                d2 = sqrt (d2);
            }
            if (d1 > 1.e20  &&  d2 > 1.e20) {
                assert (0);
            }
            if (d1 > 1.e20) {
                dcrit = d2;
            }
            else if (d2 > 1.e20) {
                dcrit = d1;
            }
            else {
                dcrit = (d1 + d2) / 2.0;
            }
            dcrit /= 2.0;

            if (simOutputFlag == 0) {
                dcrit /= 5.0;
            }

            istat =
              FindCloseSealedHorizonPoint (&xt, &yt, &zt, dcrit);
            if (istat == 1) {
                xa1[j] = xt;
                ya1[j] = yt;
                za1[j] = zt;
            }
        }
    }

    return 1;

}


/*----------------------------------------------------------------------------*/

/*
 * Find a sealed horizon intersect point within dcrit distance
 * of the specified x, y, z location.  Return 1 if a point is
 * found and set its location in the x, y, z.  If no close point
 * is found, return zero.
 *
 * This is a protected method.
 */
int SealedModel::FindCloseSealedHorizonPoint (
    double *xio, double *yio, double *zio, double dcrit)
{
    double         xt, yt, zt, dx, dy, dz, dist;
    double         xmin, ymin, zmin, dmin;
    double         x, y, z;
    int            i, j;
    _INtersectionLine_  *hptr;

    if (SealedHorizonIntersects == NULL) {
        return 0;
    }
    if (SealedTopAndBottomIntersects == NULL) {
        return 0;
    }

    x = *xio;
    y = *yio;
    z = *zio;

/*
 * Find the closest sealed horizon intersect point.
 */
    xmin = ymin = zmin = 1.e30;
    dmin = 1.e30;
    for (i=0; i<NumSealedHorizonIntersects; i++) {
        hptr = SealedHorizonIntersects + i;
        for (j=0; j<hptr->npts; j++) {
            xt = hptr->x[j];
            yt = hptr->y[j];
            zt = hptr->z[j];
            dx = xt - x;
            dy = yt - y;
            dz = zt - z;
            dist = dx * dx + dy * dy + dz * dz;
            dist = sqrt (dist);
            if (dist < dmin) {
                dmin = dist;
                xmin = xt;
                ymin = yt;
                zmin = zt;
            }
        }
    }

    for (i=0; i<NumSealedTopAndBottomIntersects; i++) {
        hptr = SealedTopAndBottomIntersects + i;
        for (j=0; j<hptr->npts; j++) {
            xt = hptr->x[j];
            yt = hptr->y[j];
            zt = hptr->z[j];
            dx = xt - x;
            dy = yt - y;
            dz = zt - z;
            dist = dx * dx + dy * dy + dz * dz;
            dist = sqrt (dist);
            if (dist < dmin) {
                dmin = dist;
                xmin = xt;
                ymin = yt;
                zmin = zt;
            }
        }
    }

/*
 * If the closest point is within dcrit, return it's coordinates.
 */
    if (dmin <= dcrit) {
        *xio = xmin;
        *yio = ymin;
        *zio = zmin;
        return 1;
    }

    return 0;

}


/*-------------------------------------------------------------------------*/

/*
 * Add an attribute name to the "global" attribute list for
 * the sealed model object.  If the name already exists,
 * return its index.  If a new name is added, return the new index.
 *
 * If the maximum number of attributes is exhausted, assert so the
 * max number can be made larger.
 */
int SealedModel::AddNewAttributeName (char const *atname)
{
    int          i, cval;
    char         c100[100];

    strncpy (c100, atname, 99);

    csw_StrLeftJust (c100);

    for (i=0; i<NumAttribs; i++) {
        cval = strcmp (c100, AttribNames[i]);
        if (cval == 0) {
            return i;
        }
    }

    strcpy (AttribNames[NumAttribs], c100);
    NumAttribs++;

    if (NumAttribs >= _MAX_ATTRIBS_) {
        printf ("Too many global attribute names.");
        assert (0);
    }

    return (NumAttribs - 1);

}



/*---------------------------------------------------------------------------*/

/*
 * If two intersection lines shared by the same fault also share
 * endpoints, record the splice information in their structures.
 */
int SealedModel::SpliceFaultIntersectionLines (void)
{
    _INtersectionLine_  *iptr, *jptr;
    int                 i, j, npts, istat, faultid;
    double              xt1, yt1, zt1,
                        xt2, yt2, zt2,
                        xt3, yt3, zt3,
                        xt4, yt4, zt4;

    if (IntersectionLines == NULL) {
        return -1;
    }

    for (i=0; i<NumIntersectionLines; i++) {

    /*
     * Check if this intersection line is shared by a fault.
     */
        iptr = IntersectionLines + i;

        faultid = iptr->surf1;
        if (faultid < _FAULT_ID_BASE_  ||  faultid >= _BOUNDARY_ID_BASE_) {
            faultid = iptr->surf2;
        }
        if (faultid < _FAULT_ID_BASE_  ||  faultid >= _BOUNDARY_ID_BASE_) {
            continue;
        }

    /*
     * Get the end point coordinates of the line.
     */
        npts = iptr->npts;
        xt1 = iptr->x[0];
        yt1 = iptr->y[0];
        zt1 = iptr->z[0];
        xt2 = iptr->x[npts-1];
        yt2 = iptr->y[npts-1];
        zt2 = iptr->z[npts-1];

        for (j=i+1; j<NumIntersectionLines; j++) {

        /*
         * Find other intersection lines shared by the same fault
         * that have coincident end points.
         */
            jptr = IntersectionLines + j;

            if (jptr->surf1 != faultid  &&  jptr->surf2 != faultid) {
                continue;
            }

            npts = jptr->npts;
            xt3 = jptr->x[0];
            yt3 = jptr->y[0];
            zt3 = jptr->z[0];
            xt4 = jptr->x[npts-1];
            yt4 = jptr->y[npts-1];
            zt4 = jptr->z[npts-1];

            istat = SamePointXYZ (xt1, yt1, zt1,
                                  xt3, yt3, zt3);
            if (istat == 1) {
                iptr->splice_partner_1 = j;
                jptr->splice_partner_1 = i;
            }

            istat = SamePointXYZ (xt1, yt1, zt1,
                                  xt4, yt4, zt4);
            if (istat == 1) {
                iptr->splice_partner_1 = j + _LAST_POINT_FLAG_;
                jptr->splice_partner_2 = i;
            }

            istat = SamePointXYZ (xt2, yt2, zt2,
                                  xt3, yt3, zt3);
            if (istat == 1) {
                iptr->splice_partner_2 = j;
                jptr->splice_partner_1 = i + _LAST_POINT_FLAG_;
            }

            istat = SamePointXYZ (xt2, yt2, zt2,
                                  xt4, yt4, zt4);
            if (istat == 1) {
                iptr->splice_partner_1 = j + _LAST_POINT_FLAG_;
                jptr->splice_partner_1 = i + _LAST_POINT_FLAG_;
            }

        }

    }

    return 1;

}


/*--------------------------------------------------------------------------------*/

/*
 * Move the point specified by the spliceid to the specified
 * xyz location.  The spliceid is the index into the IntersectionLines list
 * with a flag multiplexed to specify the first or last point of
 * the intersection line.
 */
int SealedModel::MoveSplicePoint (
    int          spliceid,
    double       xt,
    double       yt,
    double       zt)
{
    _INtersectionLine_  *iptr;
    int                 index, last, npts;

    if (IntersectionLines == NULL) {
        return -1;
    }

    index = spliceid % _LAST_POINT_FLAG_;
    if (index < 0  ||  index >= NumIntersectionLines) {
        return -1;
    }

    iptr = IntersectionLines + index; /*lint !e662*/
    last = spliceid / _LAST_POINT_FLAG_;

    if (last == 0) {
        iptr->x[0] = xt;
        iptr->y[0] = yt;
        iptr->z[0] = zt;
    }
    else {
        npts = iptr->npts;
        iptr->x[npts-1] = xt;
        iptr->y[npts-1] = yt;
        iptr->z[npts-1] = zt;
    }

    return 1;

}



/*-----------------------------------------------------------------------------------*/

/*
 * Insert the specified point either after or before
 * the specified index depending on which points the
 * new point lies between.
 */
int SealedModel::InsertPointInLine (
    double    *xa,
    double    *ya,
    double    *za,
    int       *iflags,
    int       npts,
    int       index,
    int       iflagval,
    double    xt,
    double    yt,
    double    zt)
{
    double    xt1, yt1, zt1, xt2, yt2, zt2;
    double    dx, dy, dz;
    int       i, j, istat;

    if (npts < 2) {
        return -1;
    }

    i = index;

    if (i < npts-1) {
        xt1 = xa[i];
        yt1 = ya[i];
        zt1 = za[i];
        xt2 = xa[i+1];
        yt2 = ya[i+1];
        zt2 = za[i+1];
        dx = xt2 - xt1;
        if (dx < 0) dx = -dx;
        dy = yt2 - yt1;
        if (dy < 0) dy = -dy;
        dz = zt2 - zt1;
        if (dz < 0) dz = -dz;
        istat = 0;
        if (dx > dy  &&  dx > dz) {
            if ((xt1-xt)*(xt-xt2) >= 0) {
                istat = 1;
            }
        }
        else if (dy > dx  &&  dy > dz) {
            if ((yt1-yt)*(yt-yt2) >= 0) {
                istat = 1;
            }
        }
        else {
            if ((zt1-zt)*(zt-zt2) >= 0) {
                istat = 1;
            }
        }
        if (istat == 1) {
            for (j=npts-1; j>i; j--) {
                xa[j+1] = xa[j];
                ya[j+1] = ya[j];
                za[j+1] = za[j];
                iflags[j+1] = iflags[j];
            }
            xa[i+1] = xt;
            ya[i+1] = yt;
            za[i+1] = zt;
            iflags[i+1] = iflagval;
            return 1;
        }
    }

    if (i > 0) {
        xt1 = xa[i];
        yt1 = ya[i];
        zt1 = za[i];
        xt2 = xa[i-1];
        yt2 = ya[i-1];
        zt2 = za[i-1];
        dx = xt2 - xt1;
        if (dx < 0) dx = -dx;
        dy = yt2 - yt1;
        if (dy < 0) dy = -dy;
        dz = zt2 - zt1;
        if (dz < 0) dz = -dz;
        istat = 0;
        if (dx > dy  &&  dx > dz) {
            if ((xt1-xt)*(xt-xt2) >= 0) {
                istat = 1;
            }
        }
        else if (dy > dx  &&  dy > dz) {
            if ((yt1-yt)*(yt-yt2) >= 0) {
                istat = 1;
            }
        }
        else {
            if ((zt1-zt)*(zt-zt2) >= 0) {
                istat = 1;
            }
        }
        if (istat == 1) {
            for (j=npts-1; j>=i; j--) {
                xa[j+1] = xa[j];
                ya[j+1] = ya[j];
                za[j+1] = za[j];
                iflags[j+1] = iflags[j];
            }
            xa[i] = xt;
            ya[i] = yt;
            za[i] = zt;
            iflags[i] = iflagval;
            return 1;
        }
    }

    return -1;

}

/*-----------------------------------------------------------------------------------*/

/**
 * Add a fault to the list of faults to be sealed.  This version specifies
 * nodes, edges and triangles and it also specifies the steep coordinate
 * reference frame.  All of the data are copied into internal structures
 * so the calling function may csw_Free its data when appropriate.
 */
int SealedModel::addInputFault (
    int               id,
    int               vflag,
    double            *vbase,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;

    CSWTriMeshStruct   *tmesh = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (InputFaultList == NULL  ||
        NumInputFaultList >= MaxInputFaultList) {
        MaxInputFaultList += 10;
        InputFaultList = (CSWTriMeshStruct *)csw_Realloc
            (InputFaultList,
             MaxInputFaultList * sizeof(CSWTriMeshStruct));
    }

    if (InputFaultList == NULL) {
        return -1;
    }

    tmesh = InputFaultList + NumInputFaultList;
    memset (tmesh, 0, sizeof(CSWTriMeshStruct)); /*lint !e669*/

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;

    if (vflag == 1  &&  vbase != NULL) {
        tmesh->vflag = vflag;
        memcpy (tmesh->vbase, vbase, 6 * sizeof(double));
    }

    tmesh->external_id = id;
    tmesh->age = -1.0;
    tmesh->minage = -1.0;
    tmesh->maxage = -1.0;

    NumInputFaultList++;

    bsuccess = true;

    return 1;

}




/*---------------------------------------------------------------------*/

/*
 * Return the padded trimesh with specified id.  The search uses
 * the padded horizons and the padded sed surface.
 */

CSWTriMeshStruct *SealedModel::FindPaddedTriMesh (int id)
{
    int                i;
    CSWTriMeshStruct   *tmesh;

    if (id == _SED_SURF_ID_) {
        return PaddedSedimentSurface;
    }

    if (id < 0) {
        return NULL;
    }
    if (id >= _FAULT_ID_BASE_) {
        return NULL;
    }

    if (PaddedHorizonList == NULL) {
        return NULL;
    }

    for (i=0; i<NumPaddedHorizonList; i++) {
        tmesh = PaddedHorizonList + i;
        if (tmesh->id == id) {
            return tmesh;
        }
    }

    return NULL;

}


/*-------------------------------------------------------------------------*/

void SealedModel::WriteDebugFiles (void)
{
    int do_write;
    do_write = csw_GetDoWrite ();
    if (do_write) {

        char      fname[100];
        double    v6[6];
        int       i, npad;
        CSWTriMeshStruct  *tmp = NULL, *padlist = NULL;

        double    *xline = NULL, *yline = NULL, *zline = NULL;
        int       ntot, n, nv, nvtot, *ivec = NULL, *icomp = NULL;
        _INtersectionLine_  *iptr = NULL;


        auto lscope = [&]()
        {
            csw_Free (xline);
            csw_Free (ivec);
        };
        CSWScopeGuard loc_scope_guard (lscope);


        if (PaddedTop) {
          sprintf (fname, "top.tri");
          grd_api_obj.grd_WriteTextTriMeshFile (
            0, v6,
            PaddedTop->tris, PaddedTop->num_tris,
            PaddedTop->edges, PaddedTop->num_edges,
            PaddedTop->nodes, PaddedTop->num_nodes,
            fname);
        }

        if (PaddedBottom) {
          sprintf (fname, "bottom.tri");
          grd_api_obj.grd_WriteTextTriMeshFile (
            0, v6,
            PaddedBottom->tris, PaddedBottom->num_tris,
            PaddedBottom->edges, PaddedBottom->num_edges,
            PaddedBottom->nodes, PaddedBottom->num_nodes,
            fname);
        }

        if (NorthBoundarySurface) {
          sprintf (fname, "north.tri");
          grd_api_obj.grd_WriteTextTriMeshFile (
            0, v6,
            NorthBoundarySurface->tris, NorthBoundarySurface->num_tris,
            NorthBoundarySurface->edges, NorthBoundarySurface->num_edges,
            NorthBoundarySurface->nodes, NorthBoundarySurface->num_nodes,
            fname);
        }

        if (SouthBoundarySurface) {
          sprintf (fname, "south.tri");
            grd_api_obj.grd_WriteTextTriMeshFile (
            0, v6,
            SouthBoundarySurface->tris, SouthBoundarySurface->num_tris,
            SouthBoundarySurface->edges, SouthBoundarySurface->num_edges,
            SouthBoundarySurface->nodes, SouthBoundarySurface->num_nodes,
            fname);
        }

        if (EastBoundarySurface) {
          sprintf (fname, "east.tri");
          grd_api_obj.grd_WriteTextTriMeshFile (
            0, v6,
            EastBoundarySurface->tris, EastBoundarySurface->num_tris,
            EastBoundarySurface->edges, EastBoundarySurface->num_edges,
            EastBoundarySurface->nodes, EastBoundarySurface->num_nodes,
            fname);
        }

        if (WestBoundarySurface) {
          sprintf (fname, "west.tri");
          grd_api_obj.grd_WriteTextTriMeshFile (
            0, v6,
            WestBoundarySurface->tris, WestBoundarySurface->num_tris,
            WestBoundarySurface->edges, WestBoundarySurface->num_edges,
            WestBoundarySurface->nodes, WestBoundarySurface->num_nodes,
            fname);
        }

        getInputHorizons (&padlist, &npad);
        for (i=0; i<npad; i++) {
          tmp = padlist + i;
          sprintf (fname, "mb_input_hor_%d.tri", i+1);
          grd_api_obj.grd_WriteTextTriMeshFile (
            0, v6,
            tmp->tris, tmp->num_tris,
            tmp->edges, tmp->num_edges,
            tmp->nodes, tmp->num_nodes,
            fname);
        }
        getInputFaults (&padlist, &npad);
        for (i=0; i<npad; i++) {
          tmp = padlist + i;
          sprintf (fname, "mb_input_flt_%d.tri", i+1);
          grd_api_obj.grd_WriteTextTriMeshFile (
            0, v6,
            tmp->tris, tmp->num_tris,
            tmp->edges, tmp->num_edges,
            tmp->nodes, tmp->num_nodes,
            fname);
        }

        getPaddedHorizons (&padlist, &npad);
        for (i=0; i<npad; i++) {
          tmp = padlist + i;
          sprintf (fname, "mb_pad_hor_%d.tri", i+1);
          grd_api_obj.grd_WriteTextTriMeshFile (
            0, v6,
            tmp->tris, tmp->num_tris,
            tmp->edges, tmp->num_edges,
            tmp->nodes, tmp->num_nodes,
            fname);
        }
        getPaddedFaults (&padlist, &npad);
        for (i=0; i<npad; i++) {
          tmp = padlist + i;
          sprintf (fname, "mb_pad_flt_%d.tri", i+1);
          grd_api_obj.grd_WriteTextTriMeshFile (
            0, v6,
            tmp->tris, tmp->num_tris,
            tmp->edges, tmp->num_edges,
            tmp->nodes, tmp->num_nodes,
            fname);
        }

        if (SealedHorizonList != NULL) {
            for (i=0; i<NumSealedHorizonList; i++) {
              tmp = SealedHorizonList + i;
              sprintf (fname, "mb_seal_hor_%d.tri", i+1);
              grd_api_obj.grd_WriteTextTriMeshFile (
                0, v6,
                tmp->tris, tmp->num_tris,
                tmp->edges, tmp->num_edges,
                tmp->nodes, tmp->num_nodes,
                fname);
            }
        }

        if (SealedFaultList != NULL) {
            for (i=0; i<NumSealedFaultList; i++) {
              tmp = SealedFaultList + i;
              sprintf (fname, "mb_seal_flt_%d.tri", i+1);
              grd_api_obj.grd_WriteTextTriMeshFile (
                0, v6,
                tmp->tris, tmp->num_tris,
                tmp->edges, tmp->num_edges,
                tmp->nodes, tmp->num_nodes,
                fname);
            }
        }


        ntot = 0;
        nvtot = 0;

        if (SealedHorizonIntersects) {
            for (i=0; i<NumSealedHorizonIntersects; i++) {
                iptr = SealedHorizonIntersects + i;
                ntot += iptr->npts;
                ntot++;
                nvtot++;
            }
        }

        if (SealedFaultIntersects) {
            for (i=0; i<NumSealedFaultIntersects; i++) {
                iptr = SealedFaultIntersects + i;
                ntot += iptr->npts;
                ntot++;
                nvtot++;
            }
        }

        nvtot++;

        xline = (double *)csw_Malloc (ntot * 3 * sizeof(double));
        if (xline == NULL) {
            return;
        }
        yline = xline + ntot;
        zline = yline + ntot;

        ivec = (int *)csw_Calloc (2 * nvtot * sizeof(int));
        if (ivec == NULL) {
            return;
        }
        icomp = ivec + nvtot;

        n = 0;
        nv = 0;
        if (SealedHorizonIntersects) {
          for (i=0; i<NumSealedHorizonIntersects; i++) {
            iptr = SealedHorizonIntersects + i;
            memcpy (xline+n, iptr->x, iptr->npts * sizeof(double));
            memcpy (yline+n, iptr->y, iptr->npts * sizeof(double));
            memcpy (zline+n, iptr->z, iptr->npts * sizeof(double));
            n += iptr->npts;
            ivec[nv] = iptr->npts;
            icomp[nv] = 1;
            nv++;
          }
        }

        if (SealedFaultIntersects) {
          for (i=0; i<NumSealedFaultIntersects; i++) {
            iptr = SealedFaultIntersects + i;
            memcpy (xline+n, iptr->x, iptr->npts * sizeof(double));
            memcpy (yline+n, iptr->y, iptr->npts * sizeof(double));
            memcpy (zline+n, iptr->z, iptr->npts * sizeof(double));
            n += iptr->npts;
            ivec[nv] = iptr->npts;
            icomp[nv] = 1;
            nv++;
          }
        }

        sprintf (fname, "seal_lines.xyz");
        grd_api_obj.grd_WriteLines (
          xline, yline, zline,
          nv, icomp, ivec,
          fname);

    }

    return;

}



/*------------------------------------------------------------------------------------*/


int SealedModel::calcFaultHorizonIntersections (void)
{
    int            i, j, istat;
    _INtersectionLine_  *ilptr;

/*
 * If intersection lines already exist, csw_Free them.
 */
    if (IntersectionLines != NULL) {
        for (i=0; i<NumIntersectionLines; i++) {
            ilptr = IntersectionLines + i;
            csw_Free (ilptr->x);
        }
        csw_Free (IntersectionLines);
    }
    NumIntersectionLines = 0;
    IntersectionLines = NULL;

    WriteDebugFiles ();

    if (InputTriangle3DIndex != NULL) {
        delete (InputTriangle3DIndex);
        InputTriangle3DIndex = NULL;
    }

/*
 * Calculate horizon to fault intersections.
 */
    for (i=0; i<NumInputHorizonList; i++) {
        for (j=0; j<NumPaddedFaultList; j++) {
            istat =
              CalcInputSurfacePaddedFaultIntersectionLines (
                  i, _HORIZON_TMESH_,
                  j, _FAULT_TMESH_);
            if (istat == -1) {
                return -1;
            }
        }
    }

    if (InputTriangle3DIndex != NULL) {
        delete (InputTriangle3DIndex);
        InputTriangle3DIndex = NULL;
    }

    if (IntersectionLines == NULL  ||  NumIntersectionLines < 1) {
        return 0;
    }

    return 1;

}


/*-------------------------------------------------------------------------------*/

/*
 * If there are small gaps in a fault surface, the intersection between an
 * input fault and an input surface may have many lines with small separations
 * between them.  This method combines multiple lines with small separations
 * into longer continuous lines.
 */
int SealedModel::ConnectCloseWorkIntersectionLines (void)
{
    _INtersectionLine_      *lp1 = NULL, *lp2 = NULL;
    int                     i, j, n, nmax, istat, done;
    double                  *xw = NULL, *yw = NULL, *zw = NULL;
    double                  xfirst, yfirst, xlast, ylast,
                            x1, y1, x2, y2, dx, dy, dist, dcrit;
    _INtersectionLine_      *work2;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xw);
            csw_Free (work2);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (NumWorkIntersectionLines < 2) {
        return 1;
    }

    if (WorkIntersectionLines == NULL) {
        return 1;
    }

    if (padXmin < padXmax) {
        dcrit = (padXmax - padXmin + padYmax - padYmin) / 40.0;
    }
    else {
        x1 = 1.e30;
        y1 = 1.e30;
        x2 = -1.e30;
        y2 = -1.e30;
        for (i=0; i<NumWorkIntersectionLines; i++) {
            lp1 = WorkIntersectionLines + i;
            for (j=0; j<lp1->npts; j++) {
                if (lp1->x[j] < x1) x1 = lp1->x[j];
                if (lp1->x[j] > x2) x2 = lp1->x[j];
                if (lp1->y[j] < y1) y1 = lp1->y[j];
                if (lp1->y[j] > y2) y2 = lp1->y[j];
            }
        }
        if (x1 >= x2  ||  y1 >= y2) {
            return 1;
        }
        dcrit = (x2 - x1 + y2 - y1) / 20.0;
    }

    nmax = 0;
    for (i=0; i<NumWorkIntersectionLines; i++) {
        WorkIntersectionLines[i].nclose = 0;
        nmax += WorkIntersectionLines[i].npts;
    }
    nmax += NumWorkIntersectionLines * 3;
    if (nmax < 1000) nmax = 1000;

    xw = (double *)csw_Malloc (nmax * 3 * sizeof(double));
    if (xw == NULL) {
        return -1;
    }
    yw = xw + nmax;
    zw = yw + nmax;

    for (i=0; i<NumWorkIntersectionLines; i++) {
        lp1 = WorkIntersectionLines + i;
        if (lp1->nclose == 1) {
            continue;
        }
        n = lp1->npts;
        memcpy (xw, lp1->x, n * sizeof(double));
        memcpy (yw, lp1->y, n * sizeof(double));
        memcpy (zw, lp1->z, n * sizeof(double));
        done = 0;
        for (j=i+1; j<NumWorkIntersectionLines; j++) {
            lp2 = WorkIntersectionLines + j;
            if (lp2->nclose == 1) {
                continue;
            }
            xfirst = xw[0];
            yfirst = yw[0];
            xlast = xw[n-1];
            ylast = yw[n-1];
            x1 = lp2->x[0];
            y1 = lp2->y[0];
            x2 = lp2->x[lp2->npts-1];
            y2 = lp2->y[lp2->npts-1];
            dx = x1 - xfirst;
            dy = y1 - yfirst;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist <= dcrit) {
                istat =
                PrependIline (xw, yw, zw, &n, lp2, -1);
                if (istat == -1) {
                    return -1;
                }
                lp2->nclose = 1;
                done = 1;
                continue;
            }
            dx = x1 - xlast;
            dy = y1 - ylast;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist <= dcrit) {
                istat =
                AppendIline (xw, yw, zw, &n, lp2, 1);
                if (istat == -1) {
                    return -1;
                }
                lp2->nclose = 1;
                done = 1;
                continue;
            }
            dx = x2 - xfirst;
            dy = y2 - yfirst;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist <= dcrit) {
                istat =
                PrependIline (xw, yw, zw, &n, lp2, 1);
                if (istat == -1) {
                    return -1;
                }
                lp2->nclose = 1;
                done = 1;
                continue;
            }
            dx = x2 - xlast;
            dy = y2 - ylast;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist <= dcrit) {
                istat =
                AppendIline (xw, yw, zw, &n, lp2, -1);
                if (istat == -1) {
                    return -1;
                }
                lp2->nclose = 1;
                done = 1;
                continue;
            }
        }
        if (done == 1) {
            csw_Free (lp1->x);
            lp1->x = (double *)csw_Malloc (n * 3 * sizeof(double));
            if (lp1->x == NULL) {
                return -1;
            }
            lp1->y = lp1->x + n;
            lp1->z = lp1->y + n;
            memcpy (lp1->x, xw, n * sizeof(double));
            memcpy (lp1->y, yw, n * sizeof(double));
            memcpy (lp1->z, zw, n * sizeof(double));
            lp1->npts = n;
        }
    }

/*
 * Remove work intersection lines that have nclose = 1.
 * These lines have been combined with others and are
 * therefore redundant.
 */
    work2 = (_INtersectionLine_ *)csw_Calloc
      (NumWorkIntersectionLines * sizeof(_INtersectionLine_));
    if (work2 == NULL) {
        return -1;
    }

    n = 0;
    for (i=0; i<NumWorkIntersectionLines; i++) {
        lp1 = WorkIntersectionLines + i;
        if (lp1->nclose == 1) {
            csw_Free (lp1->x);
            continue;
        }
        memcpy (work2+n, lp1, sizeof(_INtersectionLine_));
        n++;
    }

    csw_Free (WorkIntersectionLines);
    WorkIntersectionLines = work2;
    NumWorkIntersectionLines = n;

    bsuccess = true;

    return 1;
}


int SealedModel::AppendIline (double *xw, double *yw, double*zw,
                              int *nw,
                              _INtersectionLine_ *lp, int idir)
{
    int       i, n, size;

    size = lp->npts * sizeof(double);
    n = *nw;

    if (idir == 1) {
        memcpy (xw+n, lp->x, size);
        memcpy (yw+n, lp->y, size);
        memcpy (zw+n, lp->z, size);
    }
    else {
        for (i=lp->npts-1; i>=0; i--) {
            xw[n] = lp->x[i];
            yw[n] = lp->y[i];
            zw[n] = lp->z[i];
            n++;
        }
    }

    *nw = n;

    return 1;
}



int SealedModel::PrependIline (double *xw, double *yw, double*zw,
                              int *nw,
                              _INtersectionLine_ *lp, int idir)
{
    int       i, n, size;
    double    *xa = NULL, *ya = NULL, *za = NULL;


    auto fscope = [&]()
    {
        csw_Free (xa);
    };
    CSWScopeGuard func_scope_guard (fscope);


    size = *nw + lp->npts + 2;
    xa = (double *)csw_Malloc (size * 3 * sizeof(double));
    if (xa == NULL) {
        return -1;
    }
    ya = xa + size;
    za = ya + size;

    n = 0;
    if (idir == 1) {
        for (i=0; i<lp->npts; i++) {
            xa[n] = lp->x[i];
            ya[n] = lp->y[i];
            za[n] = lp->z[i];
            n++;
        }
    }
    else {
        for (i=lp->npts-1; i>=0; i--) {
            xa[n] = lp->x[i];
            ya[n] = lp->y[i];
            za[n] = lp->z[i];
            n++;
        }
    }

    size = *nw * sizeof(double);
    memcpy (xa+n, xw, size);
    memcpy (ya+n, yw, size);
    memcpy (za+n, zw, size);

    n += *nw;
    size = n * sizeof(double);
    memcpy (xw, xa, size);
    memcpy (yw, ya, size);
    memcpy (zw, za, size);

    *nw = n;

    return 1;

}

/*--------------------------------------------------------------------*/

/*
 *  Add a fault that has min and max ages.
 */
int SealedModel::addInputFault (
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles,
    double            minage,
    double            maxage)
{
    int    istat;
    istat = addInputFault (0, nodes_in, num_nodes,
                           edges_in, num_edges,
                           triangles_in, num_triangles,
                           minage, maxage);
    return istat;
}

int SealedModel::addInputFault (
    int               id,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles,
    double            minage,
    double            maxage)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;

    CSWTriMeshStruct   *tmesh = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (InputFaultList == NULL  ||
        NumInputFaultList >= MaxInputFaultList) {
        MaxInputFaultList += 10;
        InputFaultList = (CSWTriMeshStruct *)csw_Realloc
            (InputFaultList,
             MaxInputFaultList * sizeof(CSWTriMeshStruct));
    }

    if (InputFaultList == NULL) {
        return -1;
    }

    tmesh = InputFaultList + NumInputFaultList;  /*lint !e662*/
    memset (tmesh, 0, sizeof(CSWTriMeshStruct)); /*lint !e669*/

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;
    tmesh->external_id = id;
    tmesh->age = -1.0;
    tmesh->minage = minage;
    tmesh->maxage = maxage;

    NumInputFaultList++;

    bsuccess = true;

    return 1;

}


int SealedModel::addInputFault (
    int               id,
    int               vflag,
    double            *vbase,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles,
    double            minage,
    double            maxage,
    double            *xline,
    double            *yline,
    double            *zline,
    int               nline)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;
    double            *xdetach = NULL, *ydetach = NULL, *zdetach = NULL;

    CSWTriMeshStruct   *tmesh = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
            csw_Free (xdetach);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (InputFaultList == NULL  ||
        NumInputFaultList >= MaxInputFaultList) {
        MaxInputFaultList += 10;
        InputFaultList = (CSWTriMeshStruct *)csw_Realloc
            (InputFaultList,
             MaxInputFaultList * sizeof(CSWTriMeshStruct));
    }

    if (InputFaultList == NULL) {
        return -1;
    }

    tmesh = InputFaultList + NumInputFaultList;  /*lint !e662*/
    memset (tmesh, 0, sizeof(CSWTriMeshStruct)); /*lint !e669*/

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    if (xline &&  yline  &&  zline  &&  nline > 1) {
        xdetach = (double *)csw_Malloc (nline * 3 * sizeof(double));
        if (xdetach == NULL) {
            return -1;
        }
        ydetach = xdetach + nline;
        zdetach = ydetach + nline;
        memcpy (xdetach, xline, nline * sizeof(double));
        memcpy (ydetach, yline, nline * sizeof(double));
        memcpy (zdetach, zline, nline * sizeof(double));
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;
    tmesh->external_id = id;
    tmesh->age = -1.0;
    tmesh->minage = minage;
    tmesh->maxage = maxage;

    tmesh->xdetach = xdetach;
    tmesh->ydetach = ydetach;
    tmesh->zdetach = zdetach;
    tmesh->ndetach = nline;

    if (vflag == 1  &&  vbase != NULL) {
        tmesh->vflag = vflag;
        memcpy (tmesh->vbase, vbase, 6 * sizeof(double));
    }

    NumInputFaultList++;

    bsuccess = true;

    return 1;

}




/*-----------------------------------------------------------------------------------*/

/**
 * Add a fault to the list of faults to be sealed.  This version specifies
 * nodes, edges and triangles and it also specifies the steep coordinate
 * reference frame.  All of the data are copied into internal structures
 * so the calling function may csw_Free its data when appropriate.
 */
int SealedModel::addInputFault (
    int               id,
    int               vflag,
    double            *vbase,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles,
    double            minage,
    double            maxage)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;

    CSWTriMeshStruct   *tmesh = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (InputFaultList == NULL  ||
        NumInputFaultList >= MaxInputFaultList) {
        MaxInputFaultList += 10;
        InputFaultList = (CSWTriMeshStruct *)csw_Realloc
            (InputFaultList,
             MaxInputFaultList * sizeof(CSWTriMeshStruct));
    }

    if (InputFaultList == NULL) {
        return -1;
    }

    tmesh = InputFaultList + NumInputFaultList; 
    memset (tmesh, 0, sizeof(CSWTriMeshStruct));

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;

    if (vflag == 1  &&  vbase != NULL) {
        tmesh->vflag = vflag;
        memcpy (tmesh->vbase, vbase, 6 * sizeof(double));
    }

    tmesh->external_id = id;
    tmesh->age = -1.0;
    tmesh->minage = minage;
    tmesh->maxage = maxage;

    NumInputFaultList++;

    bsuccess = true;

    return 1;

}





/*---------------------------------------------------------------------------------*/

/*
 * Find all the intersection objects that use the specified
 * surface id and put them into the surface as constraints.
 * This is used for vertical boundaries, but the results are
 * added to the sealed fault list.
 */
int SealedModel::EmbedLineInBoundary(
  int id,
  double *xc, double *yc, double *zc, int nc) {

  CSWTriMeshStruct   *bound;
  int       istat;
  int       index;

/*
 * Assert if a horizon id or fault id is specified.
 */
  if (id < _BOUNDARY_ID_BASE_) {
    assert (0);
  }

/*
 * Get the boundary trimesh.
 */
  bound = NULL;
  index = id - _BOUNDARY_ID_BASE_;
  if (index == _NORTH_ID_) {
    bound = NorthBoundarySurface;
  }
  else if (index == _SOUTH_ID_) {
    bound = SouthBoundarySurface;
  }
  else if (index == _EAST_ID_) {
    bound = EastBoundarySurface;
  }
  else if (index == _WEST_ID_) {
    bound = WestBoundarySurface;
  }
  else {
    return -1;
  }

  if (bound == NULL) {
    return -1;
  }

/*
 * Apply the lines as exact constraints to the new bound.
 */
  istat =
    EmbedPointsInSurface (bound,
                          xc, yc, zc, &nc, 1);
  if (istat == -1) {
    return -1;
  }

  return istat;

}


/*--------------------------------------------------------------------------*/

/*
 * Create a horizontal surface at the padZmax elevation, extending
 * to the x and y padded limits.
 */
int SealedModel::CreateDefaultPaddedTop (void)
{
    int          i, ncol, nrow, istat;
    double       xyspace, x1, y1, x2, y2, tiny, zval;
    CSW_F        *grid = NULL;
    NOdeStruct   *nodes = NULL;
    EDgeStruct   *edges = NULL;
    TRiangleStruct  *tris = NULL;
    int          numnodes, numedges, numtris;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        csw_Free (grid);
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (tris);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    x1 = padXmin;
    y1 = padYmin;
    x2 = padXmax;
    y2 = padYmax;

    tiny = (x2 - x1 + y2 - y1) / 50.0;
    x1 -= tiny;
    y1 -= tiny;
    x2 += tiny;
    y2 += tiny;

    tiny = (padZmax - padZmin) / 100000.0;
    zval = padZmax - tiny;

    xyspace = verticalBoundarySpacing;
    if (xyspace <= 0.0) {
        xyspace = (x2 - x1 + y2 - y1) / 50.0;
    }

    if (x1 + 2 * xyspace >= padXWest) {
        x1 -= xyspace;
    }
    if (y1 + 2 * xyspace >= padYSouth) {
        y1 -= xyspace;
    }
    if (x2 - 2 * xyspace <= padXEast) {
        x2 += xyspace;
    }
    if (y2 - 2 * xyspace <= padYNorth) {
        y2 += xyspace;
    }

    ncol = (int)((x2 - x1) / xyspace + .5);
    nrow = (int)((y2 - y1) / xyspace + .5);
    if (ncol < 2) ncol = 2;
    if (nrow < 2) nrow = 2;

    grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (grid == NULL) {
        return -1;
    }

    for (i=0; i<ncol * nrow; i++) {
        grid[i] = (CSW_F)zval;
    }

    istat =
      grd_api_obj.grd_CalcTriMeshFromGrid (
        grid, ncol, nrow,
        x1, y1, x2, y2,
        NULL, NULL, NULL,
        NULL, NULL, 0,
        GRD_EQUILATERAL,
        &nodes, &edges, &tris,
        &numnodes, &numedges, &numtris);
    if (istat == -1) {
        return -1;
    }

    if (PaddedTop != NULL) {
        FreeTmesh (PaddedTop, 1);
        PaddedTop = NULL;
    }

    PaddedTop = (CSWTriMeshStruct *)csw_Calloc
        (sizeof(CSWTriMeshStruct));
    if (PaddedTop == NULL) {
        return -1;
    }

    PaddedTop->nodes = nodes;
    PaddedTop->edges = edges;
    PaddedTop->tris = tris;
    PaddedTop->num_nodes = numnodes;
    PaddedTop->num_edges = numedges;
    PaddedTop->num_tris = numtris;
    PaddedTop->id = _TOP_ID_;
    PaddedTop->external_id = _BOUNDARY_ID_BASE_ + 6;
    PaddedTop->xcenter = (x1 + x2) / 2.0;
    PaddedTop->ycenter = (y1 + y2) / 2.0;
    PaddedTop->age = 0.0;

    bsuccess = true;

    return 1;
}


/*--------------------------------------------------------------------------*/

/*
 * Create a horizontal surface at the padZmin elevation, extending
 * to the x and y padded limits.
 */
int SealedModel::CreateDefaultPaddedBottom (void)
{
    int          i, ncol, nrow, istat;
    double       xyspace, x1, y1, x2, y2, tiny, zval;
    CSW_F        *grid = NULL;
    NOdeStruct   *nodes = NULL;
    EDgeStruct   *edges = NULL;
    TRiangleStruct  *tris = NULL;
    int          numnodes, numedges, numtris;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        csw_Free (grid);
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (tris);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    x1 = padXmin;
    y1 = padYmin;
    x2 = padXmax;
    y2 = padYmax;

    tiny = (x2 - x1 + y2 - y1) / 50.0;
    x1 -= tiny;
    y1 -= tiny;
    x2 += tiny;
    y2 += tiny;

    tiny = (padZmax - padZmin) / 1000000.0;
    zval = padZmin + tiny;

    xyspace = verticalBoundarySpacing;
    if (xyspace <= 0.0) {
        xyspace = (x2 - x1 + y2 - y1) / 50.0;
    }

    if (x1 + 2 * xyspace >= padXWest) {
        x1 -= xyspace;
    }
    if (y1 + 2 * xyspace >= padYSouth) {
        y1 -= xyspace;
    }
    if (x2 - 2 * xyspace <= padXEast) {
        x2 += xyspace;
    }
    if (y2 - 2 * xyspace <= padYNorth) {
        y2 += xyspace;
    }

    ncol = (int)((x2 - x1) / xyspace + .5);
    nrow = (int)((y2 - y1) / xyspace + .5);
    if (ncol < 2) ncol = 2;
    if (nrow < 2) nrow = 2;

    grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (grid == NULL) {
        return -1;
    }

    for (i=0; i<ncol * nrow; i++) {
        grid[i] = (CSW_F)zval;
    }

    istat =
      grd_api_obj.grd_CalcTriMeshFromGrid (
        grid, ncol, nrow,
        x1, y1, x2, y2,
        NULL, NULL, NULL,
        NULL, NULL, 0,
        GRD_EQUILATERAL,
        &nodes, &edges, &tris,
        &numnodes, &numedges, &numtris);
    if (istat == -1) {
        return -1;
    }

    if (PaddedBottom != NULL) {
        FreeTmesh (PaddedBottom, 1);
        PaddedBottom = NULL;
    }

    PaddedBottom = (CSWTriMeshStruct *)csw_Calloc
        (sizeof(CSWTriMeshStruct));
    if (PaddedBottom == NULL) {
        return -1;
    }

    PaddedBottom->nodes = nodes;
    PaddedBottom->edges = edges;
    PaddedBottom->tris = tris;
    PaddedBottom->num_nodes = numnodes;
    PaddedBottom->num_edges = numedges;
    PaddedBottom->num_tris = numtris;
    PaddedBottom->id = _BOTTOM_ID_;
    PaddedBottom->external_id = _BOTTOM_ID_;
    PaddedBottom->xcenter = (x1 + x2) / 2.0;
    PaddedBottom->ycenter = (y1 + y2) / 2.0;
    PaddedBottom->age = 0.0;

    bsuccess = true;

    return 1;
}




/*---------------------------------------------------------------------------------*/

void SealedModel::SnapTopLine (_INtersectionLine_ *iptr)
{
    int         i;

    for (i=0; i<iptr->npts; i++) {
        if (iptr->x[i] < padXWest) iptr->x[i] = padXWest;
        if (iptr->x[i] > padXEast) iptr->x[i] = padXEast;
        if (iptr->y[i] > padYNorth) iptr->y[i] = padYNorth;
        if (iptr->y[i] < padYSouth) iptr->y[i] = padYSouth;
        iptr->z[i] = padZmax;
    }

}

void SealedModel::SnapBottomLine (_INtersectionLine_ *iptr)
{
    int         i;

    for (i=0; i<iptr->npts; i++) {
        if (iptr->x[i] < padXWest) iptr->x[i] = padXWest;
        if (iptr->x[i] > padXEast) iptr->x[i] = padXEast;
        if (iptr->y[i] > padYNorth) iptr->y[i] = padYNorth;
        if (iptr->y[i] < padYSouth) iptr->y[i] = padYSouth;
        iptr->z[i] = padZmin;
    }

}


/*---------------------------------------------------------------------------------*/

void SealedModel::SnapTopSurface (void)
{

    if (PaddedTop == NULL) {
        return;
    }

    int            i, numnodes;
    NOdeStruct     *nodes;

    nodes = PaddedTop->nodes;
    numnodes = PaddedTop->num_nodes;

    for (i=0; i<numnodes; i++) {
        if (nodes[i].x < padXWest) nodes[i].x = padXWest;
        if (nodes[i].x > padXEast) nodes[i].x = padXEast;
        if (nodes[i].y < padYSouth) nodes[i].y = padYSouth;
        if (nodes[i].y > padYNorth) nodes[i].y = padYNorth;
        nodes[i].z = padZmax;
    }

}

void SealedModel::SnapBottomSurface (void)
{

    if (PaddedBottom == NULL) {
        return;
    }

    int            i, numnodes;
    NOdeStruct     *nodes;

    nodes = PaddedBottom->nodes;
    numnodes = PaddedBottom->num_nodes;

    for (i=0; i<numnodes; i++) {
        if (nodes[i].x < padXWest) nodes[i].x = padXWest;
        if (nodes[i].x > padXEast) nodes[i].x = padXEast;
        if (nodes[i].y < padYSouth) nodes[i].y = padYSouth;
        if (nodes[i].y > padYNorth) nodes[i].y = padYNorth;
        nodes[i].z = padZmin;
    }

}


/*------------------------------------------------------------------------------*/

CSWTriMeshStruct *SealedModel::FindPaddedFaultForID (int id)
{
    int                 i;
    CSWTriMeshStruct    *tmesh;

    if (PaddedFaultList == NULL) {
        return NULL;
    }

    for (i=0; i<NumPaddedFaultList; i++) {
        tmesh = PaddedFaultList + i;
        if (tmesh->external_id == id  &&  tmesh->is_padded == 1) {
            return tmesh;
        }
    }

    return NULL;

}


/*-------------------------------------------------------------------------------*/

int SealedModel::ReshapePaddedFault (
    CSWTriMeshStruct   *tmesh,
    CSWTriMeshStruct   *stmesh)
{
    tmesh = tmesh;
    stmesh = stmesh;
    return 1;
}


/*--------------------------------------------------------------------*/

/**
 Seal the surfaces in the padded fault list to the padded detachment surface.
 The results are put into the sealed fault list and the sealed detachment
 surface.
 */
int SealedModel::sealFaultsToDetachment (void)
{
    int           i, j, istat;
    int           num_nodes, num_edges;
    NOdeStruct    *nodes = NULL;
    EDgeStruct    *edges = NULL, *eptr = NULL;

    double    *xresamp = NULL, *yresamp = NULL, *zresamp = NULL;
    int       nresamp, maxresamp;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xresamp);
            FaultList = NULL;
            HorizonList = NULL;
            NumFaultList = 0;
            NumHorizonList = 0;
            MaxFaultList = 0;
            MaxHorizonList = 0;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (NumPaddedFaultList < 1) {
        return 0;
    }

    if (PaddedDetachment == NULL  ||  PaddedFaultList == NULL) {
        return 0;
    }

    if (PaddedTriangle3DIndex != NULL) {
        delete (PaddedTriangle3DIndex);
    }
    PaddedTriangle3DIndex = NULL;

    int do_write;
    do_write = csw_GetDoWrite ();;
    if (do_write) {
        char fname1[200];
        sprintf (fname1, "pad_detach.tri");
        grd_api_obj.grd_WriteTextTriMeshFile (
            0, NULL,
            PaddedDetachment->tris, PaddedDetachment->num_tris,
            PaddedDetachment->edges, PaddedDetachment->num_edges,
            PaddedDetachment->nodes, PaddedDetachment->num_nodes,
            fname1);
    }

/*
 * Calculate the boundaries for the padded faults and the padded detachment.
 */
    padXmin = 1.e30;
    padYmin = 1.e30;
    padZmin = 1.e30;
    padXmax = -1.e30;
    padYmax = -1.e30;
    padZmax = -1.e30;

    nodes = PaddedDetachment->nodes;
    num_nodes = PaddedDetachment->num_nodes;
    for (j=0; j<num_nodes; j++) {
        if (nodes[j].deleted == 1) continue;
        if (nodes[j].x < padXmin) padXmin = nodes[j].x;
        if (nodes[j].y < padYmin) padYmin = nodes[j].y;
        if (nodes[j].z < padZmin) padZmin = nodes[j].z;
        if (nodes[j].x > padXmax) padXmax = nodes[j].x;
        if (nodes[j].y > padYmax) padYmax = nodes[j].y;
        if (nodes[j].z > padZmax) padZmax = nodes[j].z;
    }

    for (i=0; i<NumPaddedFaultList; i++) {
        nodes = PaddedFaultList[i].nodes;
        num_nodes = PaddedFaultList[i].num_nodes;
        for (j=0; j<num_nodes; j++) {
            if (nodes[j].deleted == 1) continue;
            if (nodes[j].x < padXmin) padXmin = nodes[j].x;
            if (nodes[j].y < padYmin) padYmin = nodes[j].y;
            if (nodes[j].z < padZmin) padZmin = nodes[j].z;
            if (nodes[j].x > padXmax) padXmax = nodes[j].x;
            if (nodes[j].y > padYmax) padYmax = nodes[j].y;
            if (nodes[j].z > padZmax) padZmax = nodes[j].z;
        }
    }

    if (padXmin >= padXmax  ||
        padYmin >= padYmax  ||
        padZmin > padZmax) // all z values can be identical, do I don't use >= here
    {
        return 0;
    }

    double tiny;

    tiny = (padXmax - padXmin + padYmax - padYmin + padZmax - padZmin) / 300.0;
    padXmin -= tiny;
    padXmax += tiny;
    padYmin -= tiny;
    padYmax += tiny;
    padZmin -= tiny;
    padZmax += tiny;

    int       navg = 0;
    double    dist;

    averageSpacing = 0.0;
    edges = PaddedDetachment->edges;
    nodes = PaddedDetachment->nodes;
    num_edges = PaddedDetachment->num_edges;
    for (i=0; i<num_edges; i++) {
        eptr = edges + i;
        if (eptr->deleted == 0) {
            dist = CalcEdgeLength (eptr, nodes);
            averageSpacing += dist;
            navg++;
        }
    }

    if (navg < 1) {
        return 0;
    }

    averageSpacing /= navg;

/*
 * Create new triangle index for padded and detachment surfaces.
 */
    istat = CreateDetachment3DTriangleIndex ();
    if (istat == -1  ||  PaddedTriangle3DIndex == NULL) {
        return -1;
    }

    modelGrazeDistance = tiny / 10.0;

    FaultList = PaddedFaultList;
    NumFaultList = NumPaddedFaultList;
    MaxFaultList = MaxPaddedFaultList;

/*
 * Calculate detachment to fault intersections.
 */
    for (j=0; j<NumPaddedFaultList; j++) {

        if (do_write) {
            char fname1[200];
            sprintf (fname1, "pad_fault_%d.tri", j);
            grd_api_obj.grd_WriteTextTriMeshFile (
                0, NULL,
                FaultList[j].tris, FaultList[j].num_tris,
                FaultList[j].edges, FaultList[j].num_edges,
                FaultList[j].nodes, FaultList[j].num_nodes,
                fname1);
        }

        istat =
          CalcPaddedSurfaceIntersectionLines (
              0, _DETACHMENT_TMESH_,
              j, _FAULT_TMESH_);

        if (istat == -1) {
            return -1;
        }
    }

    if (IntersectionLines == NULL  ||  NumIntersectionLines < 1) {
        return 0;
    }


    WriteIntersectionLines ((char *)"rawint.xyz");

    _INtersectionLine_ *iptr = NULL;
/*
 * Decimate and resample the intersection lines.
 */
    for (i=0; i<NumIntersectionLines; i++) {
        iptr = IntersectionLines + i;
        maxresamp = iptr->npts * 10;
        if (maxresamp < 1000) maxresamp = 1000;
        xresamp = (double *)csw_Malloc (3 * maxresamp * sizeof(double));
        if (xresamp == NULL) {
            return -1;
        }
        yresamp = xresamp + maxresamp;
        zresamp = yresamp + maxresamp;
        istat =
          grd_api_obj.grd_ResampleXYZLine (
            iptr->x, iptr->y, iptr->z, iptr->npts,
            averageSpacing,
            xresamp, yresamp, zresamp, &nresamp,
            maxresamp);
        if (istat == -1) {
            return -1;
        }
        csw_Free (iptr->x);
        iptr->x = xresamp;
        xresamp = NULL;
        iptr->y = yresamp;
        iptr->z = zresamp;
        iptr->npts = nresamp;
    }
    xresamp = NULL;
    yresamp = NULL;
    zresamp = NULL;
    nresamp = 0;

    WriteIntersectionLines ((char *)"decint.xyz");

/*
 * Use the decimated intersection lines to build the new
 * sealed detachment surface.
 */
    istat =
      EmbedEdgesInDetachment ();
    if (istat == -1  ||  SealedDetachment == NULL) {
        return -1;
    }

    if (do_write) {
        char fname1[200];
        sprintf (fname1, "seal_detach.tri");
        grd_api_obj.grd_WriteTextTriMeshFile (
            0, NULL,
            SealedDetachment->tris, SealedDetachment->num_tris,
            SealedDetachment->edges, SealedDetachment->num_edges,
            SealedDetachment->nodes, SealedDetachment->num_nodes,
            fname1);
    }

/*
 * Apply the intersection lines to faults as exact constraints.
 */
    for (i=0; i<NumFaultList; i++) {
        istat =
          EmbedDetachmentEdgesInFault (i);
        if (istat == -1) {
            return -1;
        }

        if (do_write  &&  SealedFaultList != NULL) {
            char fname1[200];
            sprintf (fname1, "seal_fault_%d.tri", i);
            grd_api_obj.grd_WriteTextTriMeshFile (
                0, NULL,
                SealedFaultList[i].tris, SealedFaultList[i].num_tris,
                SealedFaultList[i].edges, SealedFaultList[i].num_edges,
                SealedFaultList[i].nodes, SealedFaultList[i].num_nodes,
                fname1);
        }

    }

    bsuccess = true;

    return 1;

}




/*----------------------------------------------------------------------------*/

/*
 * Create a triangle index and add the padded detachment surface and all the
 * padded fault surfaces to it.
 * On success, 1 is returned.  If the model bounds cannot be calculated, zero
 * is returned.  On a memory allocation failure, -1 is returned.
 */
int SealedModel::CreateDetachment3DTriangleIndex (void)
{
    CSWTriMeshStruct      *tmesh;
    int                   i, istat, tmeshid;

    if (PaddedDetachment == NULL  ||
        PaddedFaultList == NULL) {
        return 0;
    }

    if (padXmin > padXmax) {
        return 0;
    }

    if (PaddedTriangle3DIndex != NULL) {
        delete PaddedTriangle3DIndex;
        PaddedTriangle3DIndex = NULL;
    }

    try {
        SNF;
        PaddedTriangle3DIndex = new Spatial3DTriangleIndex (
                padXmin, padYmin, padZmin,
                padXmax, padYmax, padZmax);
    }
    catch (...) {
        printf ("\n***** Exception from new *****\n\n");
        PaddedTriangle3DIndex = NULL;
        return 0;
    }

    if (averageSpacing > 0.0) {
        PaddedTriangle3DIndex->SetGeometry (
        padXmin, padYmin, padZmin,
        padXmax, padYmax, padZmax,
        averageSpacing, averageSpacing, averageSpacing);
    }

  /*
   * Put the detachment into the index.
   */
    tmesh = PaddedDetachment;
    istat =
    PaddedTriangle3DIndex->AddTriMesh (
        _DETACHMENT_TMESH_,
        tmesh->nodes,
        tmesh->edges,
        tmesh->tris,
        tmesh->num_tris);
    if (istat == -1) {
        return -1;
    }

  /*
   * Put the faults into the index.
   */
    for (i=0; i<NumPaddedFaultList; i++) {
        tmesh = PaddedFaultList + i;
        tmeshid = _FAULT_ID_BASE_ + i;
        istat =
        PaddedTriangle3DIndex->AddTriMesh (
            tmeshid,
            tmesh->nodes,
            tmesh->edges,
            tmesh->tris,
            tmesh->num_tris);
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}

/*------------------------------------------------------------------------*/

/*
 * Add the current Intersection Lines to the detachment as exact constraints.
 */
int SealedModel::EmbedEdgesInDetachment (void)
{

  CSWTriMeshStruct   *detach = NULL, *newdetach = NULL;
  _INtersectionLine_ *iptr = NULL;
  double    *xdec = NULL, *ydec = NULL, *zdec = NULL;
  int       ndec, i;
  double    *xc = NULL, *yc = NULL, *zc = NULL;
  int       *npc = NULL, nc, ntot, new_ntot, istat;

  bool     bsuccess = false;


  auto fscope = [&]()
  {
    csw_Free (xc);
    csw_Free (yc);
    csw_Free (zc);
    csw_Free (npc);
    FreeTmesh (SealedDetachment, 1);
    if (bsuccess == false) {
      csw_Free (newdetach->nodes);
      csw_Free (newdetach->edges);
      csw_Free (newdetach->tris);
      csw_Free (newdetach);
    }
  };
  CSWScopeGuard func_scope_guard (fscope);


  if (PaddedDetachment == NULL  ||  IntersectionLines == NULL) {
    return 0;
  }

  npc = (int *)csw_Calloc (NumIntersectionLines * sizeof(int));
  if (npc == NULL) {
    return -1;
  }

  xc = NULL;
  yc = NULL;
  zc = NULL;
  nc = 0;
  ntot = 0;

  for (i=0; i<NumIntersectionLines; i++) {

    iptr = IntersectionLines + i;

    xdec = iptr->x;
    ydec = iptr->y;
    zdec = iptr->z;
    ndec = iptr->npts;

    new_ntot =
    add_constraint_for_embed (xdec, ydec, zdec, ndec,
                              &xc, &yc, &zc,
                              ntot);
    npc[nc] = ndec;
    nc++;
    if (new_ntot == -1) {
      return -1;
    }
    ntot = new_ntot;
  }

  if (nc < 1) {
    return 1;
  }


/*
 * Copy the embedding surface so it can be changed without
 * disturbing the original.
 */
  newdetach = (CSWTriMeshStruct *)csw_Calloc (sizeof(CSWTriMeshStruct));
  if (newdetach == NULL) {
    return -1;
  }
  detach = PaddedDetachment;

/*
 * Copy all the scalar data and allocate new space for the arrays.
 */
  memcpy (newdetach, detach, sizeof(CSWTriMeshStruct));
  newdetach->nodes = (NOdeStruct *)csw_Malloc (detach->num_nodes * sizeof(NOdeStruct));
  newdetach->edges = (EDgeStruct *)csw_Malloc (detach->num_edges * sizeof(EDgeStruct));
  newdetach->tris = (TRiangleStruct *)csw_Malloc (detach->num_tris * sizeof(TRiangleStruct));
  if (newdetach->nodes == NULL  ||
      newdetach->edges == NULL  ||
      newdetach->tris == NULL) {
    return -1;
  }

/*
 * Copy the old arrays into the new detach.
 */
  memcpy (newdetach->nodes, detach->nodes, detach->num_nodes * sizeof(NOdeStruct));
  memcpy (newdetach->edges, detach->edges, detach->num_edges * sizeof(EDgeStruct));
  memcpy (newdetach->tris, detach->tris, detach->num_tris * sizeof(TRiangleStruct));

/*
 * Apply the lines as exact constraints to the new detach.
 */
  istat =
    EmbedPointsInSurface (newdetach,
                          xc, yc, zc, npc, nc);
  if (istat == -1) {
    return 1;
  }

  int do_write = csw_GetDoWrite ();
  if (do_write) {
    char   fname1[100];

    sprintf (fname1, "embed.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
      0, NULL,
      newdetach->tris, newdetach->num_tris,
      newdetach->edges, newdetach->num_edges,
      newdetach->nodes, newdetach->num_nodes,
      fname1);

  }


  SealedDetachment = newdetach;

  bsuccess = true;

  return istat;

}



/*-----------------------------------------------------------------------------*/

double SealedModel::CalcEdgeLength (EDgeStruct *eptr, NOdeStruct *nodes)
{
    NOdeStruct     *np1, *np2;
    double         dx, dy, dist;

    np1 = nodes + eptr->node1;
    np2 = nodes + eptr->node2;

    dx = np1->x - np2->x;
    dy = np1->y - np2->y;

    dist = dx * dx + dy * dy;
    dist = sqrt (dist);

    return dist;

}

/*------------------------------------------------------------------------*/

/*
 * Find all the intersection objects that use the specified
 * surface id and put them into the surface as constraints.
 * This is used for faults and vertical model boundaries.  It
 * is not used for horizons, and will assert is a horizon id
 * is specified.
 */
int SealedModel::EmbedDetachmentEdgesInFault(int id)
{

  CSWTriMeshStruct   *fault = NULL, *newfault = NULL;
  _INtersectionLine_ *iptr = NULL;
  double    *xdec = NULL, *ydec = NULL, *zdec = NULL;
  int       ndec, maxlines;
  double    *xc = NULL, *yc = NULL, *zc = NULL;
  int       *npc = NULL, nc, ntot, new_ntot, istat;
  int       index;

  bool     bsuccess = false;

  auto fscope = [&]()
  {
      if (bsuccess == false) {
        csw_Free (xc);
        csw_Free (yc);
        csw_Free (zc);
        csw_Free (npc);
        csw_Free (newfault->nodes);
        csw_Free (newfault->edges);
        csw_Free (newfault->tris);
      }
      csw_Free (newfault);
  };
  CSWScopeGuard func_scope_guard (fscope);


  if (FaultList == NULL) {
    return 0;
  }

  if (IntersectionLines == NULL) {
    return 0;
  }

/*
 * Get the fault trimesh.
 */
  index = id;
  fault = FaultList + index;
  id += _FAULT_ID_BASE_;

  xc = NULL;
  yc = NULL;
  zc = NULL;
  nc = 0;
  ntot = 0;

  maxlines = NumIntersectionLines;
  if (maxlines < 100) maxlines = 100;
  npc = (int *)csw_Malloc (maxlines * sizeof(int));
  if (npc == NULL) {
    return -1;
  }

/*
 * Loop through the sealed fault intersects and use
 * any that share this fault.
 */
  int      i;

  for (i=0; i<NumIntersectionLines; i++) {

    iptr = IntersectionLines + i;

    if (!(iptr->surf1 == id  ||  iptr->surf2 == id)) {
        continue;
    }

    xdec = iptr->x;
    ydec = iptr->y;
    zdec = iptr->z;
    ndec = iptr->npts;

    new_ntot =
    add_constraint_for_embed (xdec, ydec, zdec, ndec,
                              &xc, &yc, &zc,
                              ntot);
    npc[nc] = ndec;
    nc++;
    if (new_ntot == -1) {
      return -1;
    }
    ntot = new_ntot;
  }

  if (nc < 1) {
    return 1;
  }

  WriteDebugFiles ();

/*
 * Copy the embedding surface so it can be changed without
 * disturbing the original.
 */
  newfault = (CSWTriMeshStruct *)csw_Calloc (sizeof(CSWTriMeshStruct));
  if (newfault == NULL) {
    return 1;
  }

/*
 * Copy all the scalar data and allocate new space for the arrays.
 */
  memcpy (newfault, fault, sizeof(CSWTriMeshStruct));
  newfault->nodes = (NOdeStruct *)csw_Malloc (fault->num_nodes * sizeof(NOdeStruct));
  newfault->edges = (EDgeStruct *)csw_Malloc (fault->num_edges * sizeof(EDgeStruct));
  newfault->tris = (TRiangleStruct *)csw_Malloc (fault->num_tris * sizeof(TRiangleStruct));
  if (newfault->nodes == NULL  ||
      newfault->edges == NULL  ||
      newfault->tris == NULL) {
    return 1;
  }

  WriteDebugFiles ();

/*
 * Copy the old arrays into the new fault.
 */
  memcpy (newfault->nodes, fault->nodes, fault->num_nodes * sizeof(NOdeStruct));
  memcpy (newfault->edges, fault->edges, fault->num_edges * sizeof(EDgeStruct));
  memcpy (newfault->tris, fault->tris, fault->num_tris * sizeof(TRiangleStruct));

/*
 * Apply the lines as exact constraints to the new fault.
 */
  istat =
    EmbedPointsInSurface (newfault,
                          xc, yc, zc, npc, nc);
  if (istat == -1) {
    return 1;
  }

  WriteDebugFiles ();

  int do_write = csw_GetDoWrite ();
  if (do_write) {
    char   fname1[100];
    double v6[6];

    sprintf (fname1, "embed.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
      0, v6,
      newfault->tris, newfault->num_tris,
      newfault->edges, newfault->num_edges,
      newfault->nodes, newfault->num_nodes,
      fname1);

  }

/*
 * Remove the padded fault triangles that are outside of the
 * final sealed fault.
 */
  istat =
    ClipDetachmentFaultToIntersections (newfault);
  if (istat == -1) {
    return -1;
  }

/*
 * Adding a sealed fault makes a shallow copy of newfault, so
 * we have to csw_Free newfault, but not its nodes, edges, or tris.
 */
  AddSealedFault (newfault);

  bsuccess = true;

  return istat;

}




/*------------------------------------------------------------------------*/

int SealedModel::ClipDetachmentFaultToIntersections (
  CSWTriMeshStruct *tmesh)
{
  int             i, istat;

/*
 * Find the trimesh border node with the lowest z value.
 */
  int      startnode = -1;
  double   zmin = 1.e30;
  int      n1, n2;
  EDgeStruct  *eptr;

  for (i=0; i<tmesh->num_edges; i++) {

    eptr = tmesh->edges + i;
    if (eptr->tri2 >= 0) {
      continue;
    }

    n1 = eptr->node1;
    n2 = eptr->node2;

    if (tmesh->nodes[n1].z < zmin) {
      zmin = tmesh->nodes[n1].z;
      startnode = n1;
    }

    if (tmesh->nodes[n2].z < zmin) {
      zmin = tmesh->nodes[n2].z;
      startnode = n2;
    }

  }

  if (startnode == -1) {
    return -1;
  }

/*
 * Starting at this lowest node, delete all the triangles
 * topologically between that node and the nearest constraint
 * edges in the trimesh.  This will trim the bottom of the
 * padded fault to the detachment surface.
 */
  WriteDebugFiles ();

  istat =
    grd_api_obj.grd_ChewUpTriangles (
      startnode,
      tmesh->nodes, &tmesh->num_nodes,
      tmesh->edges, &tmesh->num_edges,
      tmesh->tris, &tmesh->num_tris);
  if (istat == -1) {
    return -1;
  }

  WriteDebugFiles ();

  return 1;

}


/*--------------------------------------------------------------------------*/

void SealedModel::WriteIntersectionLines (char *fname)
{
    int   ncv[1000], ncp[1000];

    if (IntersectionLines == NULL) {
        return;
    }

    int do_write;

    do_write = csw_GetDoWrite ();

    if (do_write) {
        double   *x = NULL, *y = NULL, *z = NULL;
        int      i, j, ntot, n;


        auto lscope = [&]()
        {
            csw_Free (x);
        };
        CSWScopeGuard loc_scope_guard (lscope);


        ntot = 0;
        for (i=0; i<NumIntersectionLines; i++) {
            ntot += IntersectionLines[i].npts;
        }
        x = (double *)csw_Malloc (3 * ntot * sizeof(double));
        if (x == NULL) {
            return;
        }
        y = x + ntot;
        z = y + ntot;
        n = 0;
        for (i=0; i<NumIntersectionLines; i++) {
            ncp[i] = 1;
            ncv[i] = IntersectionLines[i].npts;
            for (j=0; j<IntersectionLines[i].npts; j++) {
                x[n] = IntersectionLines[i].x[j];
                y[n] = IntersectionLines[i].y[j];
                z[n] = IntersectionLines[i].z[j];
                n++;
            }
        }
        grd_api_obj.grd_WriteLines (
            x, y, z,
            NumIntersectionLines, ncp, ncv,
            fname);
    }

}


/*--------------------------------------------------------------------------*/

void SealedModel::WriteSealedHorizonIntersects (char *fname)
{
    int         ncv[1000], ncp[1000];

    if (SealedHorizonIntersects == NULL) {
        return;
    }

    int do_write;

    do_write = csw_GetDoWrite ();

    if (do_write) {
        double   *x = NULL, *y = NULL, *z = NULL;
        int      i, j, ntot, n;


        auto lscope = [&]()
        {
            csw_Free (x);
        };
        CSWScopeGuard loc_scope_guard (lscope);


        ntot = 0;
        for (i=0; i<NumSealedHorizonIntersects; i++) {
            ntot += SealedHorizonIntersects[i].npts;
        }
        x = (double *)csw_Malloc (3 * ntot * sizeof(double));
        if (x == NULL) {
            return;
        }
        y = x + ntot;
        z = y + ntot;
        n = 0;
        for (i=0; i<NumSealedHorizonIntersects; i++) {
            ncp[i] = 1;
            ncv[i] = SealedHorizonIntersects[i].npts;
            for (j=0; j<SealedHorizonIntersects[i].npts; j++) {
                x[n] = SealedHorizonIntersects[i].x[j];
                y[n] = SealedHorizonIntersects[i].y[j];
                z[n] = SealedHorizonIntersects[i].z[j];
                n++;
            }
        }
        grd_api_obj.grd_WriteLines (
            x, y, z,
            NumSealedHorizonIntersects, ncp, ncv,
            fname);
        csw_Free (x);
    }

}

/*---------------------------------------------------------------*/

double SealedModel::GetPaddedSurfaceAge (int id)
{
  int                i;
  CSWTriMeshStruct   *tmesh = NULL;
  double             age;

  age = -1.0;

  if (PaddedHorizonList == NULL) {
    return age;
  }

  for (i=0; i<NumPaddedHorizonList; i++) {
    tmesh = PaddedHorizonList + i;
    if (tmesh->id == id) {
      age = tmesh->age;
      break;
    }
  }

  return age;

}

/*---------------------------------------------------------------*/

int SealedModel::padFaultsForSplitLines (void)
{
    int               i, j, istat;
    double            xmin, ymin, zmin, xmax, ymax, zmax,
                      gxmin, gymin, gxmax, gymax;
    CSWTriMeshStruct  *tmesh = NULL;
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL, *eptr = NULL;
    TRiangleStruct    *triangles = NULL;
    int               num_nodes_out, num_edges_out, num_tris_out;
    CSW_F             *grid = NULL;
    int               ncol, nrow;
    double            average_spacing, sum1, sum2;



  /*
   * Calculate a plane fit to each fault and pad it.
   */
    if (InputFaultList != NULL) {

        double       pcoef[3];
        double       xorigin, yorigin, zorigin;
        int          lowlist[20], highlist[20], nlow, nhigh;
        double       zlow, zhigh;
        NOdeStruct   *nptr = NULL;

        SurfaceGroupPlane   *sgp = NULL;

        bool     bsuccess = false;

        auto lscope = [&]()
        {
            if (bsuccess == false) {
                delete (sgp);
                FreePaddedLists ();
            }
        };
        CSWScopeGuard loc_scope_guard (lscope);

        try {
            SNF;
            sgp = new SurfaceGroupPlane ();
        }
        catch (...) {
            printf ("\n***** Exception from new *****\n\n");
            sgp = NULL;
            return -1;
        }

        for (i=0; i<NumInputFaultList; i++) {
            tmesh = InputFaultList + i;

            sum1 = 0.0;
            sum2 = 0.0;
            for (j=0; j<tmesh->num_edges; j++) {
                eptr = tmesh->edges + j;
                if (eptr->deleted) continue;
                if (eptr->length > 0.0) {
                    sum1 += eptr->length;
                    sum2 ++;
                }
                else {
                    double      dx, dy, dist;
                    NOdeStruct  *np1, *np2;
                    np1 = tmesh->nodes + eptr->node1;
                    np2 = tmesh->nodes + eptr->node2;
                    dx = np1->x - np2->x;
                    dy = np1->y - np2->y;
                    dist = dx * dx + dy * dy;
                    dist = sqrt (dist);
                    eptr->length = dist;
                    sum1 += dist;
                    sum2++;
                }
            }
            if (sum2 > 0.0) {
                average_spacing = sum1 / sum2;
            }
            else {
                continue;
            }

            istat =
              sgp->addTriMeshForFit (
                tmesh->nodes, tmesh->num_nodes,
                tmesh->edges, tmesh->num_edges,
                tmesh->tris, tmesh->num_tris);
            if (istat == -1) {
                return -1;
            }

            istat =
              sgp->calcPlaneCoefs ();
            sgp->freeFitPoints ();
            if (istat == -1) {
                return -1;
            }

            istat =
              sgp->getCoefsAndOrigin (
                pcoef, pcoef+1, pcoef+2,
                &xorigin, &yorigin, &zorigin);
            if (istat == -1) {
                return -1;
            }

            tmesh->vbase[0] = pcoef[0];
            tmesh->vbase[1] = pcoef[1];
            tmesh->vbase[2] = pcoef[2];
            tmesh->vbase[3] = xorigin;
            tmesh->vbase[4] = yorigin;
            tmesh->vbase[5] = zorigin;

            pad_surface_obj.PadSetSurfaceGroupPlane (sgp);

        /*
         * Find the xyz limits of the input trimesh and
         * build the lowlist and highlist arrays.
         */
            xmin = ymin = zmin = 1.e30;
            xmax = ymax = zmax = -1.e30;
            for (j=0; j<tmesh->num_nodes; j++) {
                nptr = tmesh->nodes + j;
                if (nptr->deleted == 1) continue;
                if (nptr->x < xmin) xmin = nptr->x;
                if (nptr->y < ymin) ymin = nptr->y;
                if (nptr->z < zmin) zmin = nptr->z;
                if (nptr->x > xmax) xmax = nptr->x;
                if (nptr->y > ymax) ymax = nptr->y;
                if (nptr->z > zmax) zmax = nptr->z;
            }
            if (xmin >= xmax  ||  ymin >= ymax  ||  zmin >= zmax) {
                assert (0);
            }
            zlow = zmin + (zmax - zmin) / 10.0;
            zhigh = zmax - (zmax - zmin) / 10.0;

            nlow = nhigh = 0;
            for (j=0; j<tmesh->num_nodes; j++) {
                nptr = tmesh->nodes + j;
                if (nptr->deleted == 1) continue;
                if (nptr->z <= zlow  &&  nlow < 20) {
                    lowlist[nlow] = j;
                    nlow++;
                }
                if (nptr->z >= zhigh  &&  nhigh < 20) {
                    highlist[nhigh] = j;
                    nhigh++;
                }
            }

            istat =
              pad_surface_obj.PadFaultSurfaceForSim (
                  tmesh->nodes,
                  tmesh->edges,
                  tmesh->tris,
                  tmesh->num_nodes,
                  tmesh->num_edges,
                  tmesh->num_tris,
                  lowlist, nlow,
                  highlist, nhigh,
                  10, 10,
                  xmin,
                  xmax,
                  ymin,
                  ymax,
                  zmin,
                  zmax,
                  &grid,
                  &ncol,
                  &nrow,
                  &gxmin,
                  &gymin,
                  &gxmax,
                  &gymax,
                  &nodes,
                  &edges,
                  &triangles,
                  &num_nodes_out,
                  &num_edges_out,
                  &num_tris_out,
                  average_spacing,
                  tmesh->minage,
                  tmesh->maxage);
            if (istat == -1) {
                return -1;
            }
            istat =
              AddPaddedFault (
                  tmesh->external_id,
                  nodes, num_nodes_out,
                  edges, num_edges_out,
                  triangles, num_tris_out,
                  tmesh->vflag, tmesh->vbase,
                  tmesh->minage, tmesh->maxage,
                  NULL, NULL, NULL, 0, 0);
            if (istat == -1) {
                return -1;
            }
            tmesh->grid = grid;
            tmesh->ncol = ncol;
            tmesh->nrow = nrow;
            tmesh->gxmin = gxmin;
            tmesh->gymin = gymin;
            tmesh->gxmax = gxmax;
            tmesh->gymax = gymax;
            tmesh->is_padded = 1;
        }

        pad_surface_obj.PadSetSurfaceGroupPlane (NULL);

        bsuccess = true;

    }  // end of if block for fault list

    WriteDebugFiles ();

    return 1;

}





/*--------------------------------------------------------------------*/

/*
 * Calculate the intersection lines between two surfaces which already exist
 * in either the horizon list or the fault list.  The results are appended to
 * the current WorkIntersectionLines array.  This method uses horizons from
 * the input horizon list and faults from the padded fault list.
 *
 * This method returns 1 on success or -1 on an error.
 * Success does not mean that any intersections were found.  You need to
 * check the NumWorkIntersectionLines variable to see if any intersections
 * were found.
 *
 * If both input surface numbers and types point to the same trimesh object,
 * no intersection is attempted and zero is returned.
 *
 * This is a protected method.
 */
int SealedModel::CalcInputSurfacePaddedFaultIntersectionLines (
    int    surf1_num,
    int    surf1_type,
    int    surf2_num,
    int    surf2_type)
{
    CSWTriMeshStruct    *surf1,
                        *surf2,
                        *stmp;
    TRiangleStruct      *tp1, *tp2;

    int                 istat, i, j, ntri, tmeshid1, tmeshid2;
    double              txmin, tymin, tzmin, txmax, tymax, tzmax;

/*
 * Make sure the triangles are indexed.
 */
    if (InputTriangle3DIndex == NULL) {
        IndexPaddedFaults = 1;
        istat = CreateInput3DTriangleIndex ();
        IndexPaddedFaults = 0;
        if (istat == -1  ||  InputTriangle3DIndex == NULL) {
            return -1;
        }
    }

    if (modelGrazeDistance <= 0.0) {
        CalcModelGrazeDistance ();
    }

/*
 * Get the trimesh objects for each surface.
 */
    if (surf1_type == _HORIZON_TMESH_) {
        if (InputHorizonList == NULL) {
            return -1;
        }
        if (surf1_num >= NumInputHorizonList) {
            return -1;
        }
        surf1 = InputHorizonList + surf1_num;
        tmeshid1 = surf1_num + _HORIZON_ID_BASE_;
    }
    else {
        if (PaddedFaultList == NULL) {
            return -1;
        }
        if (surf1_num >= NumPaddedFaultList) {
            return -1;
        }
        surf1 = PaddedFaultList + surf1_num;
        tmeshid1 = surf1_num + _FAULT_ID_BASE_;
    }

    if (surf2_type == _HORIZON_TMESH_) {
        if (InputHorizonList == NULL) {
            return -1;
        }
        if (surf2_num >= NumInputHorizonList) {
            return -1;
        }
        surf2 = InputHorizonList + surf2_num;
        tmeshid2 = surf2_num + _HORIZON_ID_BASE_;
    }
    else {
        if (PaddedFaultList == NULL) {
            return -1;
        }
        if (surf2_num >= NumPaddedFaultList) {
            return -1;
        }
        surf2 = PaddedFaultList + surf2_num;
        tmeshid2 = surf2_num + _FAULT_ID_BASE_;
    }

/*
 * Swap surf1 and surf2 if surf2 has less triangles.
 */
    if (surf2->num_tris < surf1->num_tris) {
        stmp = surf1;
        surf1 = surf2;
        surf2 = stmp;
        i = tmeshid1;
        tmeshid1 = tmeshid2;
        tmeshid2 = i;
    }

    ntri = surf1->num_tris;

/*
 * For each triangle in surf1, find any surf2 triangles close to
 * it (using the 3d triangle index).  Calculate intersection segments
 * between the surf1 triangle and each possible surf2 triangle.
 * The WorkIntersectionSegments list is updated with these intersections.
 */
    SPatial3DTriangleStructList *stout;
    SPatial3DTriangleStruct     *stlist, *stptr;
    int                         nstlist;
    double                      tiny;

    NumWorkIntersectionSegments = 0;

    for (i=0; i<ntri; i++) {

      /*
       * Find the 3d bounding box of the triangle.
       */
        tp1 = surf1->tris + i;
        Calc3DBox (tp1, surf1->edges, surf1->nodes,
                   &txmin, &tymin, &tzmin,
                   &txmax, &tymax, &tzmax,
                   &tiny);

      /*
       * Get all triangles within the bounding box that do not
       * belong to the surf1 trimesh.
       */
        stout =
            InputTriangle3DIndex->GetTriangles (
                tmeshid1,
                txmin, tymin, tzmin,
                txmax, tymax, tzmax);
        if (stout == NULL) {
            return -1;
        }

        stlist = stout->list;
        nstlist = stout->nlist;

        if (stlist == NULL  ||  nstlist < 1) {
            csw_Free (stout);
            csw_Free (stlist);
            stout = NULL;
            stlist = NULL;
            continue;
        }

      /*
       * Check the triangle intersections between the current
       * surf1 triangle and all the surf2 triangle candidates
       * returned from the indexing.  The CalcTriangleIntersect
       * method adds segments to the WorkIntersectionSegments
       * list as needed.
       */
        for (j=0; j<nstlist; j++) {
            stptr = stlist + j;
            if (stptr->tmeshid != tmeshid2) {
                continue;
            }
            tp2 = surf2->tris + stptr->trinum;
            CalcTriangleIntersection (tp1, tp2, modelGrazeDistance / 10.0,
                                      surf1->edges,
                                      surf1->nodes,
                                      surf2->edges,
                                      surf2->nodes);
        }

      /*
       * Free the results returned from GetTriangles and check the next triangle.
       */
        csw_Free (stlist);
        csw_Free (stout);
        stlist = NULL;
        stout = NULL;
        nstlist = 0;

    }  // end of loop through the surf1 triangles

/*
 * Connect the segments in the WorkIntersectionSegment list and
 * add the intersection lines to the WorkIntersectionLines list.
 * The intersection lines are marked as shared by the two specified
 * trimesh id's.
 */
    FindOverlapWorkSegments ();
    ConnectIntersectionSegments (tmeshid1, tmeshid2);

    surf1->numIntersects += NumWorkIntersectionLines;
    surf2->numIntersects += NumWorkIntersectionLines;

/*
 * Move the work intersectionlines to the results intersection line list.
 */
    istat =
    AddWorkLinesToResults ();

    return istat;

}

/*--------------------------------------------------------------------------*/


/*
 * Add the specified constraint lines exactly to the specified trimesh.
 * In addition, make one or two other exact constraints that cross the
 * trimesh just above the maximum constraint elevation and (if no detachment
 * surface is defined) just below the minimum constraint elevation.
 */
int SealedModel::EmbedPointsInFaultSurface(
  CSWTriMeshStruct *tmesh,
  double    *xlines,
  double    *ylines,
  double    *zlines,
  int       *nplines,
  int       nlines)
{

/*
 * Get the current trimesh for the surface.
 */
  NOdeStruct *nodes = NULL;
  EDgeStruct *edges = NULL;
  TRiangleStruct *triangles = NULL;
  int num_nodes, num_edges, num_triangles;

  double      avspace;

  int         *lineflags = faultLineFlags;

  double      *xe1 = NULL, *ye1 = NULL, *xe2 = NULL, *ye2 = NULL,
              *xe3 = NULL, *ye3 = NULL, *xe4 = NULL, *ye4 = NULL;
  double      *ze1 = NULL, *ze2 = NULL, *ze3 = NULL, *ze4 = NULL;
  int         ne1, ne2, ne3, ne4;
  double      x1, x2, y1, y2, xtmp, ytmp;
  double      x1max, y1max, x1min, y1min, x2max, y2max, x2min, y2min;

  double      *xline2 = NULL, *yline2 = NULL, *zline2 = NULL;
  int         npline2[4], nline2;

  GRDVert     gvert;

  bool     bsuccess = false;

  auto fscope = [&]()
  {
    csw_Free (xline2);
    csw_Free (xe1);
    csw_Free (xe2);
    csw_Free (xe3);
    csw_Free (xe4);
    if (bsuccess == false) {
    }
  };
  CSWScopeGuard func_scope_guard (fscope);


  nodes = tmesh->nodes;
  edges = tmesh->edges;
  triangles = tmesh->tris;
  num_nodes = tmesh->num_nodes;
  num_edges = tmesh->num_edges;
  num_triangles = tmesh->num_tris;

  int    i, ntot;

  int    do_write, ncout[1000];
  char   fname1[200];
  double v6[6];
  do_write = csw_GetDoWrite ();;

  ntot = 0;
  for (i=0; i<nlines; i++) {
    ntot += nplines[i];
  }

  if (do_write) {
    for (i=0; i<nlines; i++) {
      ncout[i] = 1;
    }
    strcpy (fname1, "preconvert.xyz");
    grd_api_obj.grd_WriteLines (
        xlines, ylines, zlines,
        nlines, ncout, nplines, fname1);
  }

  if (do_write) {
    sprintf (fname1, "preconvert.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, v6,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);
  }

/*
 * Convert the trimesh nodes and lines to steep coordinates if needed.
 */
  vert_SetBaseline (
    tmesh->vbase[0],
    tmesh->vbase[1],
    tmesh->vbase[2],
    tmesh->vbase[3],
    tmesh->vbase[4],
    tmesh->vbase[5],
    tmesh->vflag,
    gvert);
  vert_ConvertPoints (xlines, ylines, zlines, ntot, gvert);
  vert_ConvertTriMeshNodes (nodes, num_nodes, gvert);

  if (do_write) {
    for (i=0; i<nlines; i++) {
      ncout[i] = 1;
    }
    strcpy (fname1, "embedlines.xyz");
    grd_api_obj.grd_WriteLines (
        xlines, ylines, zlines,
        nlines, ncout, nplines, fname1);
  }

  if (do_write) {
    sprintf (fname1, "embedinput.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, v6,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);
  }

/*
 * Find the average spacing of all constraint lines.
 */
  double    sum1, sum2;
  double    dx, dy, dist, xmin, xmax;
  int       n, j;

  sum1 = 0.0;
  sum2 = 0.0;
  n = 0;

  for (i=0; i<nlines; i++) {
    for (j=0; j<nplines[i]-1; j++) {
      dx = xlines[n] - xlines[n+1];
      dy = ylines[n] - ylines[n+1];
      dist = dx * dx + dy * dy;
      dist = sqrt (dist);
      sum1 += dist;
      sum2 ++;
      n++;
    }
    n++;
  }

  if (sum2 > 0.0) {
    avspace = sum1 / sum2;
  }
  else {
    return -1;
  }

/*
 * Find the x limits of the rotated trimesh nodes.
 */
  xmin = 1.e30;
  xmax = -1.e30;
  for (i=0; i<num_nodes; i++) {
    if (nodes[i].x < xmin) xmin = nodes[i].x;
    if (nodes[i].x > xmax) xmax = nodes[i].x;
  }
  xmin += avspace / 1000.0;
  xmax -= avspace / 1000.0;

/*
 * Find the constraint line endpoints that are the most northwest, northeast,
 * southwest and southeast.  This is only done if there are no sealed fault
 * intersects, meaning no crossing faults and no faults crossing vertical
 * boundaries.
 */
  xe1 = ye1 = ze1 = NULL;
  xe2 = ye2 = ze2 = NULL;
  xe3 = ye3 = ze3 = NULL;
  xe4 = ye4 = ze4 = NULL;
  ne1 = ne2 = ne3 = ne4 = 0;

  n = 0;
  x1max = y1max = -1.e30;
  x2max = y2max = -1.e30;
  x1min = y1min = 1.e30;
  x2min = y2min = 1.e30;

  if (SealedFaultIntersects == NULL  ||  NumSealedFaultIntersects < 1) {
    for (i=0; i<nlines; i++) {
      x1 = xlines[n];
      y1 = ylines[n];
      x2 = xlines[n+nplines[i]-1];
      y2 = ylines[n+nplines[i]-1];
      n += nplines[i];
      if (x1 > x2) {
        xtmp = x1;
        x1 = x2;
        x2 = xtmp;
        ytmp = y1;
        y1 = y2;
        y2 = ytmp;
      }
      if (y1 < y1min) {
        y1min = y1;
        x1min = x1;
      }
      if (y1 > y1max) {
        y1max = y1;
        x1max = x1;
      }
      if (y2 < y2min) {
        y2min = y2;
        x2min = x2;
      }
      if (y2 > y2max) {
        y2max = y2;
        x2max = x2;
      }
    }

  /*
   * If the x coordinates of the endpoints are inside the trimesh
   * x extents, extend a horizontal line outside to the appropriate
   * x extent.  xe1 is for the lower left extension.  xe2 for the
   * upper left.  xe3 for lower right and xe4 for upper right.
   */
    if (x1min > xmin) {
      MakeExtensionLine (xmin, y1min, x1min, avspace, &xe1, &ye1, &ze1, &ne1);
      if (xe1 == NULL) {
        return -1;
      }
    }
    if (x1max > xmin) {
      MakeExtensionLine (xmin, y1max, x1max, avspace, &xe2, &ye2, &ze2, &ne2);
      if (xe2 == NULL) {
        return -1;
      }
    }
    if (x2min < xmax) {
      MakeExtensionLine (xmax, y2min, x2min, avspace, &xe3, &ye3, &ze3, &ne3);
      if (xe3 == NULL) {
        return -1;
      }
    }
    if (x2max < xmax) {
      MakeExtensionLine (xmax, y2max, x2max, avspace, &xe4, &ye4, &ze4, &ne4);
      if (xe4 == NULL) {
        return -1;
      }
    }

  }  // end of endpoint extension block

  WriteDebugFiles ();

/*
 * Constrain the trimesh exactly to the input lines.
 */
  int    istat;

  istat = grd_api_obj.grd_AddLinesToTriMesh(
    xlines,
    ylines,
    zlines,
    nplines,
    lineflags,
    nlines,
    1,     // require exact adherance to lines
    &nodes,
    &edges,
    &triangles,
    &num_nodes,
    &num_edges,
    &num_triangles);

  if (istat == -1) {
    return -1;
  }

/*
 * If extensions were needed, constrain exactly to these also.
 */
  int netot = ne1 + ne2 + ne3 + ne4;
  if (netot > 1) {
    memset (npline2, 0, 4 * sizeof(int));
    xline2 = (double *)csw_Malloc (netot * 3 * sizeof(double));
    if (xline2 == NULL) {
      return -1;
    }
    yline2 = xline2 + netot;
    zline2 = yline2 + netot;
    n = 0;
    nline2 = 0;
    if (xe1 != NULL  &&  ye1 != NULL  &&  ze1 != NULL) {
      memcpy (xline2, xe1, ne1 * sizeof(double));
      memcpy (yline2, ye1, ne1 * sizeof(double));
      memcpy (zline2, ze1, ne1 * sizeof(double));
      n += ne1;
      npline2[nline2] = ne1;
      nline2++;
    }
    if (xe2 != NULL  &&  ye2 != NULL  &&  ze2 != NULL) {
      memcpy (xline2+n, xe2, ne2 * sizeof(double));
      memcpy (yline2+n, ye2, ne2 * sizeof(double));
      memcpy (zline2+n, ze2, ne2 * sizeof(double));
      n += ne2;
      npline2[nline2] = ne2;
      nline2++;
    }
    if (xe3 != NULL  &&  ye3 != NULL  &&  ze3 != NULL) {
      memcpy (xline2+n, xe3, ne3 * sizeof(double));
      memcpy (yline2+n, ye3, ne3 * sizeof(double));
      memcpy (zline2+n, ze3, ne3 * sizeof(double));
      n += ne3;
      npline2[nline2] = ne3;
      nline2++;
    }
    if (xe4 != NULL  &&  ye4 != NULL  &&  ze4 != NULL) {
      memcpy (xline2+n, xe4, ne4 * sizeof(double));
      memcpy (yline2+n, ye4, ne4 * sizeof(double));
      memcpy (zline2+n, ze4, ne4 * sizeof(double));
      npline2[nline2] = ne4;
      nline2++;
    }

    istat =
      grd_api_obj.grd_BackInterpolateTriMesh (
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        xline2, yline2, zline2, netot);
    if (istat == -1) {
      return -1;
    }

    int    start;
    start = 0;
    for (i=0; i<nline2; i++) {
      sum1 = 0.0;
      sum2 = 0.0;
      for (j=0; j<npline2[i]; j++) {
        if (zline2[start+j] < 1.e20) {
          sum1 += zline2[start+j];
          sum2++;
        }
      }
      if (sum2 <= 0.0) {
        printf ("No inside point for extend line in Fault Embedding\n");
        assert (0);
      }
      for (j=0; j<npline2[i]; j++) {
        if (zline2[start+j] > 1.e20) {
          zline2[start+j] = sum1 / sum2;
        }
      }
      start += npline2[i];
    }

    istat = grd_api_obj.grd_AddLinesToTriMesh(
      xline2,
      yline2,
      zline2,
      npline2,
      NULL,  // no line flags
      nline2,
      1,     // require exact adherance to lines
      &nodes,
      &edges,
      &triangles,
      &num_nodes,
      &num_edges,
      &num_triangles);

    if (istat == -1) {
      return -1;
    }

  }

  if (do_write) {
    sprintf (fname1, "embedoutput.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, v6,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);
  }

  vert_UnconvertPoints (xlines, ylines, zlines, ntot, gvert);
  vert_UnconvertTriMeshNodes (nodes, num_nodes, gvert);

  tmesh->nodes = nodes;
  tmesh->num_nodes = num_nodes;
  tmesh->edges = edges;
  tmesh->num_edges = num_edges;
  tmesh->tris = triangles;
  tmesh->num_tris = num_triangles;

  WriteDebugFiles ();

  bsuccess = true;

  return 1;

}





/*-----------------------------------------------------------------------------*/

void SealedModel::MakeExtensionLine (
  double       x1,
  double       y1,
  double       x2,
  double       space,
  double       **xa,
  double       **ya,
  double       **za,
  int          *na)
{
  int          nline, i;
  double       dx, *x = NULL, *y = NULL, *z = NULL;

  bool     bsuccess = false;


  auto fscope = [&]()
  {
    if (bsuccess == false) {
      csw_Free (x);
    }
  };
  CSWScopeGuard func_scope_guard (fscope);


  *xa = NULL;
  *ya = NULL;
  *za = NULL;

  if (x2 > x1) {
    nline = (int)((x2 - x1) / space + 1.5);
  }
  else {
    nline = (int)((x1 - x2) / space + 1.5);
  }

  if (nline < 2) {
    nline = 2;
  }
  x = (double *)csw_Malloc (nline * 3 * sizeof(double));
  if (x == NULL) {
    return;
  }
  y = x + nline;
  z = y + nline;

  dx = (x2 - x1) / (nline - 1);

  for (i=0; i<nline; i++) {
    x[i] = x1 + dx * i;
    y[i] = y1;
    z[i] = 1.e30;
  }

  *xa = x;
  *ya = y;
  *za = z;

  *na = nline;

  bsuccess = true;

  return;

}



/*--------------------------------------------------------------------*/

void SealedModel::ExtendIntersectLines (void)
{
    _INtersectionLine_      *iptr;
    double                  *x, *y;
    double                  dxline, dyline,
                            dx, dy, dist, tiny, tiny2,
                            ang, cang, sang;
    int                     npts, np1, nlast, i;

    if (IntersectionLines == NULL) {
        return;
    }

    for (i=0; i<NumIntersectionLines; i++) {
        iptr = IntersectionLines + i;

    /*
     * Do not extend intersections that do not involve an horizon.
     */
        if (iptr->surf1 >= _BOUNDARY_ID_BASE_  &&
            iptr->surf2 >= _FAULT_ID_BASE_) {
            continue;
        }
        if (iptr->surf2 >= _BOUNDARY_ID_BASE_  &&
            iptr->surf1 >= _FAULT_ID_BASE_) {
            continue;
        }
        if (iptr->surf2 >= _BOUNDARY_ID_BASE_  &&
            iptr->surf1 >= _BOUNDARY_ID_BASE_) {
            continue;
        }

        npts = iptr->npts;
        np1 = npts - 1;
        x = iptr->x;
        y = iptr->y;
        dxline = x[0] - x[npts-1];
        dyline = y[0] - y[npts-1];
        dist = dxline * dxline + dyline * dyline;
        dist = sqrt (dist);
        tiny = dist / 10.0;
        tiny2 = tiny / 2000.0;

        nlast = 1;
        dx = x[0] - x[nlast];
        dy = y[0] - y[nlast];
        while (NearZero (dx, tiny2)  &&
               NearZero (dy, tiny2)) {
            nlast++;
            if (nlast >= npts) {
                dx = dxline;
                dy = dyline;
                break;
            }
            dx = x[0] - x[nlast];
            dy = y[0] - y[nlast];
        }

        ang = atan2 (dy, dx);
        cang = cos (ang);
        sang = sin (ang);
        x[0] += tiny * cang;
        y[0] += tiny * sang;

        nlast = npts - 2;
        dx = x[np1] - x[nlast];
        dy = y[np1] - y[nlast];
        while (NearZero (dx, tiny2)  &&
               NearZero (dy, tiny2)) {
            nlast--;
            if (nlast <= 0) {
                dx = dxline;
                dy = dyline;
                break;
            }
            dx = x[np1] - x[nlast];
            dy = y[np1] - y[nlast];
        }

        ang = atan2 (dy, dx);
        cang = cos (ang);
        sang = sin (ang);
        x[np1] += tiny * cang;
        y[np1] += tiny * sang;

    }

    return;

}

int SealedModel::addInputFault (
    int               id,
    int               vflag,
    double            *vbase,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles,
    double            minage,
    double            maxage,
    double            *xline,
    double            *yline,
    double            *zline,
    int               nline,
    int               detachment_id)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;
    double            *xdetach = NULL, *ydetach = NULL, *zdetach = NULL;

    CSWTriMeshStruct   *tmesh;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
            csw_Free (xdetach);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (InputFaultList == NULL  ||
        NumInputFaultList >= MaxInputFaultList) {
        MaxInputFaultList += 10;
        InputFaultList = (CSWTriMeshStruct *)csw_Realloc
            (InputFaultList,
             MaxInputFaultList * sizeof(CSWTriMeshStruct));
    }

    if (InputFaultList == NULL) {
        return -1;
    }

    tmesh = InputFaultList + NumInputFaultList;  /*lint !e662*/
    memset (tmesh, 0, sizeof(CSWTriMeshStruct)); /*lint !e669*/

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    xdetach = ydetach = zdetach = NULL;
    if (xline &&  yline  &&  zline  &&  nline > 1) {
        xdetach = (double *)csw_Malloc (nline * 3 * sizeof(double));
        if (xdetach == NULL) {
            return -1;
        }
        ydetach = xdetach + nline;
        zdetach = ydetach + nline;
        memcpy (xdetach, xline, nline * sizeof(double));
        memcpy (ydetach, yline, nline * sizeof(double));
        memcpy (zdetach, zline, nline * sizeof(double));
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;
    tmesh->external_id = id;
    tmesh->age = -1.0;
    tmesh->minage = minage;
    tmesh->maxage = maxage;

    tmesh->xdetach = xdetach;
    tmesh->ydetach = ydetach;
    tmesh->zdetach = zdetach;
    tmesh->ndetach = nline;

    tmesh->detach_id = detachment_id;

    if (vflag == 1  &&  vbase != NULL) {
        tmesh->vflag = vflag;
        memcpy (tmesh->vbase, vbase, 6 * sizeof(double));
    }

    NumInputFaultList++;

    bsuccess = true;

    return 1;

}





/*----------------------------------------------------------------*/

/*
 * Snap the endpoints of sealed horizon outlines to exactly agree
 * with the fault intersection line endpoints.  This is needed
 * because uncrossing fault intersection lines may have modified
 * the fault intersection line endpoints.
 */
int SealedModel::CorrectToFixedFaultLines (double avspace)
{
    int                  ido, i, j, n, s1, s2, nlist, ipts, jpts;
    _INtersectionLine_   *iptr, *jptr;
    double               xt1, yt1, zt1, xt2, yt2, zt2,
                         dx, dy, dz, dist, dcrit;
    int                  ilist[1000];

    if (SealedHorizonIntersects == NULL) {
        return -1;
    }

    if (avspace <= 0.0) {
        return -1;
    }

    dcrit = avspace / 5.0;
    if (simOutputFlag) {
        dcrit = avspace / 2.0;
    }

/*
 * Find the sealed horizon intersects that share a
 * common horizon.
 */
    for (ido=0; ido<NumHorizonList; ido++) {

        n = 0;
        for (i=0; i<NumSealedHorizonIntersects; i++) {
            iptr = SealedHorizonIntersects + i;
            if (iptr->surf1 == ido  ||  iptr->surf2 == ido) {
                ilist[n] = i;
                n++;
                if (n > 1000) {
                    return -1;
                }
            }
        }

        nlist = n;

    /*
     * Compare the endpoints of fault and non fault intersection
     * lines.  In the nested loops, iptr is always a fault and
     * jptr is always a non fault intersection line.
     */
        for (i=0; i<nlist; i++) {

            iptr = SealedHorizonIntersects + ilist[i];
            s1 = iptr->surf1;
            s2 = iptr->surf2;
            if ((s1 < _FAULT_ID_BASE_  ||  s1 >= _BOUNDARY_ID_BASE_)  &&
                (s2 < _FAULT_ID_BASE_  ||  s2 >= _BOUNDARY_ID_BASE_)) {
                continue;
            }

            ipts = iptr->npts - 1;

            for (j=0; j<nlist; j++) {

                if (i == j) {
                    continue;
                }

                jptr = SealedHorizonIntersects + ilist[j];
                s1 = jptr->surf1;
                s2 = jptr->surf2;
                if ((s1 >= _FAULT_ID_BASE_  &&  s1 < _BOUNDARY_ID_BASE_)  ||
                    (s2 >= _FAULT_ID_BASE_  &&  s2 < _BOUNDARY_ID_BASE_)) {
                    continue;
                }

                jpts = jptr->npts - 1;

            /*
             * Check for coincidence with first iptr location.
             */
                xt1 = iptr->x[0];
                yt1 = iptr->y[0];
                zt1 = iptr->z[0];

                xt2 = jptr->x[0];
                yt2 = jptr->y[0];
                zt2 = jptr->z[0];

                dx = xt1 - xt2;
                dy = yt1 - yt2;
                dz = zt1 - zt2;
                dist = dx * dx + dy * dy + dz * dz;
                dist = sqrt (dist);
                if (dist <= dcrit) {
                    jptr->x[0] = xt1;
                    jptr->y[0] = yt1;
                    jptr->z[0] = zt1;
                }

                xt2 = jptr->x[jpts];
                yt2 = jptr->y[jpts];
                zt2 = jptr->z[jpts];

                dx = xt1 - xt2;
                dy = yt1 - yt2;
                dz = zt1 - zt2;
                dist = dx * dx + dy * dy + dz * dz;
                dist = sqrt (dist);
                if (dist <= dcrit) {
                    jptr->x[jpts] = xt1;
                    jptr->y[jpts] = yt1;
                    jptr->z[jpts] = zt1;
                }

            /*
             * Check for coincidence with last iptr location.
             */
                xt1 = iptr->x[ipts];
                yt1 = iptr->y[ipts];
                zt1 = iptr->z[ipts];

                xt2 = jptr->x[0];
                yt2 = jptr->y[0];
                zt2 = jptr->z[0];

                dx = xt1 - xt2;
                dy = yt1 - yt2;
                dz = zt1 - zt2;
                dist = dx * dx + dy * dy + dz * dz;
                dist = sqrt (dist);
                if (dist <= dcrit) {
                    jptr->x[0] = xt1;
                    jptr->y[0] = yt1;
                    jptr->z[0] = zt1;
                }

                xt2 = jptr->x[jpts];
                yt2 = jptr->y[jpts];
                zt2 = jptr->z[jpts];

                dx = xt1 - xt2;
                dy = yt1 - yt2;
                dz = zt1 - zt2;
                dist = dx * dx + dy * dy + dz * dz;
                dist = sqrt (dist);
                if (dist <= dcrit) {
                    jptr->x[jpts] = xt1;
                    jptr->y[jpts] = yt1;
                    jptr->z[jpts] = zt1;
                }

            }  // end of j loop

        }  // end of i loop

    }  // end of ido loop

    return 1;

}

/*---------------------------------------------------------------------------------------*/

/*
 * Add a detachment to the list of detachments to be sealed.  The detachment is specified as a
 * "CSW trimesh", i.e. as lists of nodes, edges, and triangles.  This method creates
 * copies of the specified node, edge and triangle arrays.  So, the calling function
 * should csw_Free these when it no longer directly needs them.  On success, 1 is returned.
 * On a memory allocation failure, -1 is returned.
 *
 */
int SealedModel::addInputDetachment (
    int               id,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;

    CSWTriMeshStruct   *tmesh = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (InputDetachmentList == NULL  ||
        NumInputDetachmentList >= MaxInputDetachmentList) {
        MaxInputDetachmentList += 10;
        InputDetachmentList = (CSWTriMeshStruct *)csw_Realloc
            (InputDetachmentList,
             MaxInputDetachmentList * sizeof(CSWTriMeshStruct));
    }

    if (InputDetachmentList == NULL) {
        return -1;
    }

    tmesh = InputDetachmentList + NumInputDetachmentList;
    memset (tmesh, 0, sizeof(CSWTriMeshStruct));

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;
    tmesh->external_id = id;
    tmesh->age = -1.0;
    tmesh->minage = -1.0;
    tmesh->maxage = -1.0;

    NumInputDetachmentList++;

    bsuccess = true;

    return 1;

}



/*---------------------------------------------------------------------------------------*/

/*
 * Protected method to add a detachment to the list of padded detachments.
 * The list is used for sealing an entire padded model.  The temporary
 * sealing of an individual detachment to its faults is done without
 * using this list.  See the setPaddedDetachment method.
 */
int SealedModel::AddPaddedDetachment (
    int               id,
    NOdeStruct        *nodes_in,
    int               num_nodes,
    EDgeStruct        *edges_in,
    int               num_edges,
    TRiangleStruct    *triangles_in,
    int               num_triangles)
{
    NOdeStruct        *nodes = NULL;
    EDgeStruct        *edges = NULL;
    TRiangleStruct    *triangles = NULL;

    CSWTriMeshStruct   *tmesh = NULL;

    bool     bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (PaddedDetachmentList == NULL  ||
        NumPaddedDetachmentList >= MaxPaddedDetachmentList) {
        MaxPaddedDetachmentList += 10;
        PaddedDetachmentList = (CSWTriMeshStruct *)csw_Realloc
            (PaddedDetachmentList,
             MaxPaddedDetachmentList * sizeof(CSWTriMeshStruct));
    }

    if (PaddedDetachmentList == NULL) {
        return -1;
    }

    tmesh = PaddedDetachmentList + NumPaddedDetachmentList;
    memset (tmesh, 0, sizeof(CSWTriMeshStruct));

    nodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));

    if (nodes == NULL  ||  edges == NULL  ||  triangles == NULL) {
        return -1;
    }

    memcpy (nodes, nodes_in, num_nodes * sizeof(NOdeStruct));
    memcpy (edges, edges_in, num_edges * sizeof(EDgeStruct));
    memcpy (triangles, triangles_in, num_triangles * sizeof(TRiangleStruct));

    tmesh->nodes = nodes;
    tmesh->num_nodes = num_nodes;
    tmesh->edges = edges;
    tmesh->num_edges = num_edges;
    tmesh->tris = triangles;
    tmesh->num_tris = num_triangles;
    tmesh->is_padded = 0;
    tmesh->is_sealed = 0;
    tmesh->external_id = id;
    tmesh->age = -1.0;
    tmesh->minage = -1.0;
    tmesh->maxage = -1.0;

    NumPaddedDetachmentList++;

    bsuccess = true;

    return 1;

}

/*------------------------------------------------------------------------------*/

int SealedModel::ClipBoundaryToIntersections (
  CSWTriMeshStruct *tmesh)
{
  int             i, istat;

/*
 * Find the border node with the highest z value.
 */
  int      startnode = -1;
  double   zmax = -1.e30, zmin = 1.e30;
  int      n1, n2;
  EDgeStruct  *eptr;

  for (i=0; i<tmesh->num_edges; i++) {

    eptr = tmesh->edges + i;
    if (eptr->isconstraint == 1) {
      if (eptr->flag != LIMIT_LINE_FLAG) {
        eptr->isconstraint = 0;
      }
    }

    if (eptr->tri2 >= 0) {
      continue;
    }

    n1 = eptr->node1;
    n2 = eptr->node2;

    if (tmesh->nodes[n1].z > zmax) {
      zmax = tmesh->nodes[n1].z;
      startnode = n1;
    }

    if (tmesh->nodes[n2].z > zmax) {
      zmax = tmesh->nodes[n2].z;
      startnode = n2;
    }

  }

  if (startnode == -1) {
    return -1;
  }

/*
 * Starting at this highest node, delete all the triangles
 * topologically between that node and the nearest constraint
 * edges in the trimesh.  This will trim the top of the boundary
 * to the topmost constraint lines.
 */
  WriteDebugFiles ();

  int do_write = csw_GetDoWrite ();
  if (do_write) {
    char    fname[100];
    sprintf (fname, "prechew1.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tmesh->tris, tmesh->num_tris,
        tmesh->edges, tmesh->num_edges,
        tmesh->nodes, tmesh->num_nodes,
        fname);
  }

  istat =
    grd_api_obj.grd_ChewUpTriangles (
      startnode,
      tmesh->nodes, &tmesh->num_nodes,
      tmesh->edges, &tmesh->num_edges,
      tmesh->tris, &tmesh->num_tris);
  if (istat == -1) {
    return -1;
  }

  WriteDebugFiles ();

/*
 * Find the lowest point on the fault and use it to
 * chew up triangles at the bottom of the boundary.
 */
  startnode = -1;
  for (i=0; i<tmesh->num_edges; i++) {

    eptr = tmesh->edges + i;
    if (eptr->tri2 >= 0) {
      continue;
    }

    if (eptr->flag != 0) {
      continue;
    }

    n1 = eptr->node1;
    n2 = eptr->node2;

    if (tmesh->nodes[n1].z < zmin) {
      zmin = tmesh->nodes[n1].z;
      startnode = n1;
    }

    if (tmesh->nodes[n2].z < zmin) {
      zmin = tmesh->nodes[n2].z;
      startnode = n2;
    }

  }

/*
 * Starting at this lowest node, delete all the triangles
 * topologically between that node and the nearest constraint
 * edges in the trimesh.  This will trim the bottom of the
 * trimesh to the lowest constraint lines.
 */
  WriteDebugFiles ();

  do_write = csw_GetDoWrite ();
  if (do_write) {
    char    fname[100];
    sprintf (fname, "prechew3.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tmesh->tris, tmesh->num_tris,
        tmesh->edges, tmesh->num_edges,
        tmesh->nodes, tmesh->num_nodes,
        fname);
  }

  istat =
    grd_api_obj.grd_ChewUpTriangles (
      startnode,
      tmesh->nodes, &tmesh->num_nodes,
      tmesh->edges, &tmesh->num_edges,
      tmesh->tris, &tmesh->num_tris);
  if (istat == -1) {
    return -1;
  }

  if (do_write) {
    char    fname[100];
    sprintf (fname, "postchew3.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, NULL,
        tmesh->tris, tmesh->num_tris,
        tmesh->edges, tmesh->num_edges,
        tmesh->nodes, tmesh->num_nodes,
        fname);
  }

  for (i=0; i<tmesh->num_edges; i++) {
    eptr = tmesh->edges + i;
    if (eptr->flag != 0) {
      eptr->isconstraint = 1;
    }
  }

  WriteDebugFiles ();

  return 1;

}


/*--------------------------------------------------------------------------*/


/*
 * Add the specified constraint lines exactly to the specified trimesh.
 * This has a flag array for the lines and is only used when embedding
 * into the vertical boundaries.
 */
int SealedModel::EmbedPointsInBoundarySurface(
  CSWTriMeshStruct *tmesh,
  double    *xlines,
  double    *ylines,
  double    *zlines,
  int       *nplines,
  int       *lineflags,
  int       nlines)
{

/*
 * Get the current trimesh for the surface.
 */
  NOdeStruct *nodes = NULL;
  EDgeStruct *edges = NULL;
  TRiangleStruct *triangles = NULL;
  int num_nodes, num_edges, num_triangles;

  GRDVert  gvert;

  nodes = tmesh->nodes;
  edges = tmesh->edges;
  triangles = tmesh->tris;
  num_nodes = tmesh->num_nodes;
  num_edges = tmesh->num_edges;
  num_triangles = tmesh->num_tris;

  int    i, ntot;

  int    do_write, ncout[1000];
  char   fname1[200];
  double v6[6];
  do_write = csw_GetDoWrite ();;

  ntot = 0;
  for (i=0; i<nlines; i++) {
    ntot += nplines[i];
  }

  if (do_write) {
    for (i=0; i<nlines; i++) {
      ncout[i] = 1;
    }
    strcpy (fname1, "preconvert.xyz");
    grd_api_obj.grd_WriteLines (
        xlines, ylines, zlines,
        nlines, ncout, nplines, fname1);
  }

  if (do_write) {
    sprintf (fname1, "preconvert.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, v6,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);
  }

/*
 * Convert the trimesh nodes and lines to steep coordinates if needed.
 */
  vert_SetBaseline (
    tmesh->vbase[0],
    tmesh->vbase[1],
    tmesh->vbase[2],
    tmesh->vbase[3],
    tmesh->vbase[4],
    tmesh->vbase[5],
    tmesh->vflag, gvert);
  vert_ConvertPoints (xlines, ylines, zlines, ntot, gvert);
  vert_ConvertTriMeshNodes (nodes, num_nodes, gvert);

  if (do_write) {
    for (i=0; i<nlines; i++) {
      ncout[i] = 1;
    }
    strcpy (fname1, "embedlines.xyz");
    grd_api_obj.grd_WriteLines (
        xlines, ylines, zlines,
        nlines, ncout, nplines, fname1);
  }

  if (do_write) {
    sprintf (fname1, "embedinput.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, v6,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);
  }

  WriteDebugFiles ();

/*
 * Constrain the trimesh exactly to the lines.
 */
  int    istat;

  istat = grd_api_obj.grd_AddLinesToTriMesh(
    xlines,
    ylines,
    zlines,
    nplines,
    lineflags,
    nlines,
    1,     // require exact adherance to lines
    &nodes,
    &edges,
    &triangles,
    &num_nodes,
    &num_edges,
    &num_triangles);

  if (istat == -1) {
    return -1;
  }

  if (do_write) {
    sprintf (fname1, "embedoutput.tri");
    grd_api_obj.grd_WriteTextTriMeshFile (
        0, v6,
        triangles, num_triangles,
        edges, num_edges,
        nodes, num_nodes,
        fname1);
  }

  vert_UnconvertPoints (xlines, ylines, zlines, ntot, gvert);
  vert_UnconvertTriMeshNodes (nodes, num_nodes, gvert);

  tmesh->nodes = nodes;
  tmesh->num_nodes = num_nodes;
  tmesh->edges = edges;
  tmesh->num_edges = num_edges;
  tmesh->tris = triangles;
  tmesh->num_tris = num_triangles;

  WriteDebugFiles ();

  return 1;
}


/*----------------------------------------------------------------------------*/

/*
 * This is called after the fault surfaces have been sealed.
 * The SealedFaultIntersect lines, which are between the fault
 * and the boundary, need to be clipped to the fault surface.
 */
int SealedModel::FixSealedFaultBoundaryIntersects (void)
{
    int                 istat, ido, i, id, otherid;
    CSWTriMeshStruct    *tmesh = NULL;
    _INtersectionLine_  *iptr = NULL;
    double              *xpoly = NULL, *ypoly = NULL, *zpoly = NULL;
    int                 npoly;
    int                 npout, ncout[100], nvout[100];
    int                 maxpts, maxcomp;
    int                 *nodeout = NULL;

    GRDVert     gvert;


    auto fscope = [&]()
    {
        csw_Free (xpoly);
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (SealedFaultList == NULL  ||
        SealedFaultIntersects == NULL) {
        return -1;
    }

/*
 * Allocate a polygon array big enough for any boundary
 * of any fault trimesh.
 */
    maxcomp = 100;
    maxpts = 0;
    for (ido=0; ido<NumSealedFaultList; ido++) {
        tmesh = SealedFaultList + ido;
        if (tmesh->num_edges > maxpts) {
            maxpts = tmesh->num_edges;
        }
    }

    maxpts += 1;
    if (maxpts < 4) {
        return -1;
    }

    xpoly = (double *)csw_Malloc (maxpts * 4 * sizeof(double));
    if (xpoly == NULL) {
        return -1;
    }
    ypoly = xpoly + maxpts;
    zpoly = ypoly + maxpts;
    nodeout = (int *)(zpoly + maxpts);

/*
 * Calculate the outline of each sealed fault trimesh.
 * Clip all intersection lines that use the fault to
 * the outline.
 */
    for (ido=0; ido<NumSealedFaultList; ido++) {

        tmesh = SealedFaultList + ido;
        vert_SetBaseline (
            tmesh->vbase[0],
            tmesh->vbase[1],
            tmesh->vbase[2],
            tmesh->vbase[3],
            tmesh->vbase[4],
            tmesh->vbase[5],
            tmesh->vflag, gvert);
        id = tmesh->id;

        istat =
          grd_api_obj.grd_OutlineTriMeshBoundary (
            tmesh->nodes, tmesh->num_nodes,
            tmesh->edges, tmesh->num_edges,
            tmesh->tris, tmesh->num_tris,
            xpoly, ypoly, zpoly, nodeout,
            &npout, ncout, nvout,
            maxpts, maxcomp);
        if (istat == -1) {
            return -1;
        }

        if (npout > 1) {
            continue;
        }
        if (ncout[0] > 1) {
            continue;
        }

        npoly = nvout[0];

        for (i=0; i<NumSealedFaultIntersects; i++) {

            iptr = SealedFaultIntersects + i;
            if (iptr->surf1 < _BOUNDARY_ID_BASE_  &&
                iptr->surf2 < _BOUNDARY_ID_BASE_) {
                continue;
            }
            if (iptr->surf1 >= _BOUNDARY_ID_BASE_) {
                otherid = iptr->surf2;
            }
            else {
                otherid = iptr->surf1;
            }

            if (otherid != id) {
                continue;
            }

            istat =
              ClipILineToPoly (iptr, xpoly, ypoly, zpoly, npoly, gvert);
            if (istat == -1) {
                return -1;
            }
        }

        vert_UnsetBaseline ();

    }

    return 1;

}


int SealedModel::ClipILineToPoly (
    _INtersectionLine_    *iptr,
    double                *xp,
    double                *yp,
    double                *zp,
    int                   np,
    GRDVert               &gvert)
{
    double          *xw = NULL, *yw = NULL, *zw = NULL;
    int             nw, nline;
    double          *xline = NULL, *yline = NULL, *zline = NULL;
    int             i, inside, ilast;
    double          xmid, ymid;
    int             do_write;

    CSWPolyUtils   ply_utils_obj;


    auto fscope = [&]()
    {
        csw_Free (xline);
    };
    CSWScopeGuard func_scope_guard (fscope);

 
    xline = iptr->x;
    yline = iptr->y;
    zline = iptr->z;
    nline = iptr->npts;

    xw = (double *)csw_Malloc (nline * 3 * sizeof(double));
    if (xw == NULL) {
        return -1;
    }
    yw = xw + nline;
    zw = yw + nline;

    vert_ConvertPoints (xline, yline, zline, nline, gvert);
    vert_ConvertPoints (xp, yp, zp, np, gvert);

    do_write = csw_GetDoWrite ();
    if (do_write) {
        char fname[100];
        int nc;
        sprintf (fname, "clip_iline_input.xyz");
        nc = 1;
        grd_api_obj.grd_WriteLines (
            xline, yline, zline,
            1, &nc, &nline,
            fname);
    }

    ilast = -1;
    nw = 0;

    for (i=0; i<nline-1; i++) {
        xmid = (xline[i] + xline[i+1]) / 2.0;
        ymid = (yline[i] + yline[i+1]) / 2.0;
        inside = ply_utils_obj.ply_point (xp, yp, np, xmid, ymid);
        if (inside >= 0) {
            xw[nw] = xline[i];
            yw[nw] = yline[i];
            zw[nw] = zline[i];
            nw++;
            ilast = i + 1;
        }
    }

    if (ilast >= 0) {
        xw[nw] = xline[ilast];
        yw[nw] = yline[ilast];
        zw[nw] = zline[ilast];
        nw++;
    }

    if (do_write) {
        char fname[100];
        int nc;
        sprintf (fname, "clip_iline_output.xyz");
        nc = 1;
        grd_api_obj.grd_WriteLines (
            xw, yw, zw,
            1, &nc, &nw,
            fname);
    }

    vert_UnconvertPoints (xp, yp, zp, np, gvert);
    vert_UnconvertPoints (xw, yw, zw, nw, gvert);

    iptr->x = xw;
    iptr->y = yw;
    iptr->z = zw;
    iptr->npts = nw;

    return 1;

}


/*-------------------------------------------------------------------------*/

void SealedModel::calcFaultZLimits (CSWTriMeshStruct *fault)
{
    int                 i, j;
    double              zmin, zmax, minage, maxage, zt;
    CSWTriMeshStruct    *surf;

    if (HorizonList == NULL) {
        return;
    }

    minage = fault->minage;
    maxage = fault->maxage;
    if (minage < 0  ||  maxage < 0) {
        return;
    }

    zmin = 1.e30;
    zmax = -1.e30;

    for (i=0; i<NumHorizonList; i++) {
        surf = HorizonList + i;
        if (surf->age < minage  ||  surf->age > maxage) {
            continue;
        }
        for (j=0; j<surf->num_nodes; j++) {
            zt = surf->nodes[j].z;
            if (zt < zmin) zmin = zt;
            if (zt > zmax) zmax = zt;
        }
    }

    if (zmax >= zmin) {
        fault->zmin = zmin;
        fault->zmax = zmax;
    }

    return;

}


/*--------------------------------------------------------------------------*/

void SealedModel::WritePartialIntersectionLines (char *fname, int start)
{
    int         ncv[1000], ncp[1000];

    if (IntersectionLines == NULL) {
        return;
    }
    if (start >= NumIntersectionLines) {
        return;
    }

    int do_write;

    do_write = csw_GetDoWrite ();

    if (do_write) {
        double   *x = NULL, *y = NULL, *z = NULL;
        int      i, j, ntot, n, nc;


        auto lscope = [&]()
        {
            csw_Free (x);
        };
        CSWScopeGuard loc_scope_guard (lscope);


        ntot = 0;
        for (i=start; i<NumIntersectionLines; i++) {
            ntot += IntersectionLines[i].npts;  /*lint !e662*/
        }
        x = (double *)csw_Malloc (3 * ntot * sizeof(double));
        if (x == NULL) {
            return;
        }
        y = x + ntot;
        z = y + ntot;
        n = 0;
        nc = 0;
        for (i=start; i<NumIntersectionLines; i++) {
            ncp[nc] = 1;
            ncv[nc] = IntersectionLines[i].npts;  /*lint !e662*/
            nc++;
            for (j=0; j<IntersectionLines[i].npts; j++) { /*lint !e662*/
                x[n] = IntersectionLines[i].x[j];  /*lint !e662*/
                y[n] = IntersectionLines[i].y[j];  /*lint !e662*/
                z[n] = IntersectionLines[i].z[j];  /*lint !e662*/
                n++;
            }
        } /*lint !e662*/
        grd_api_obj.grd_WriteLines (
            x, y, z,
            nc, ncp, ncv,
            fname);
    }

    return;

}


/*--------------------------------------------------------------------------*/

void SealedModel::WritePartialIntersectionLines (char *fname,
                                                 int start,
                                                 int end)
{
    int        ncv[1000], ncp[1000];

    if (IntersectionLines == NULL) {
        return;
    }
    if (start >= NumIntersectionLines) {
        return;
    }
    if (start >= end) {
        return;
    }
    if (end > NumIntersectionLines) {
        end = NumIntersectionLines;
    }

    int do_write;

    do_write = csw_GetDoWrite ();

    if (do_write) {
        double   *x = NULL, *y = NULL, *z = NULL;
        int      i, j, ntot, n, nc;


        auto lscope = [&]()
        {
            csw_Free (x);
        };
        CSWScopeGuard loc_scope_guard (lscope);


        ntot = 0;
        for (i=start; i<end; i++) {
            ntot += IntersectionLines[i].npts;  /*lint !e662*/
        }
        x = (double *)csw_Malloc (3 * ntot * sizeof(double));
        if (x == NULL) {
            return;
        }
        y = x + ntot;
        z = y + ntot;
        n = 0;
        nc = 0;
        for (i=start; i<end; i++) {
            ncp[nc] = 1;
            ncv[nc] = IntersectionLines[i].npts;  /*lint !e662*/
            nc++;
            for (j=0; j<IntersectionLines[i].npts; j++) { /*lint !e662*/
                x[n] = IntersectionLines[i].x[j];  /*lint !e662*/
                y[n] = IntersectionLines[i].y[j];  /*lint !e662*/
                z[n] = IntersectionLines[i].z[j];  /*lint !e662*/
                n++;
            }
        } /*lint !e662*/
        grd_api_obj.grd_WriteLines (
            x, y, z,
            nc, ncp, ncv,
            fname);
    }

}
