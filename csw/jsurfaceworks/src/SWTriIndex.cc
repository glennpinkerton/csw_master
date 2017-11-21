
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#include <math.h>
#include <stdlib.h>

#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

#include <csw/surfaceworks/include/grd_spatial3dtri.h>
#include <csw/jsurfaceworks/private_include/SurfaceWorksJNI.h>
#include <csw/jsurfaceworks/private_include/SWTriIndex.h>
#include <csw/surfaceworks/include/grid_api.h>

/*
 * This file has stand alone functions to implement the
 * Spatial3dTriangleIndex class methods.  The java objects
 * do their work on C++ "peer" objects via the SWCommand
 * interface and the functions in this file.  Basically,
 * the C++ equivalent to the java object is set as "active"
 * and its methods are called.
 *
 * As of Oct 2017, the SWTriIndex class is used rather than
 * the stand alone functions.  The 2017 refactor is, among
 * many things, trying to make tghe entire csw code thread safe.
 * Eliminating the static variables here is one step in the 
 * thread safety process.
 */

/*-------------------------------------------------------------------*/

#define MAX_3D_INDEX_LIST 1000

typedef struct {
    Spatial3DTriangleIndex  *index;
    int                     id1, id2;
} _INdex3DStruct_;


/*-------------------------------------------------------------------*/

/*
 * Set the active 3d index which will be used for subsequent
 * function calls to manipulate the index.
 */
int SWTriIndex::sw_Set3DTriangleIndex (int id1, int id2)
{
    int               i;
    _INdex3DStruct_   *iptr;

    if (id1 == 0  &&  id2 == 0) {
        ActiveIndex = NULL;
        return 0;
    }

    for (i=0; i<NumIndexList; i++) {
        iptr = IndexList + i;
        if (iptr->id1 == id1  &&  iptr->id2 == id2) {
            ActiveIndex = iptr->index;
            return 1;
        }
    }

    ActiveIndex = NULL;

    return 0;

}


/*-----------------------------------------------------------------------*/

/*
 * Create a new 3D triangble index and identify it with the
 * two id values.  The new index is also made the active index.
 * This function returns 1 on success or -1 if there is not
 * enough memory for the new index.
 */
int SWTriIndex::sw_Create3DTriangleIndex (int id1, int id2,
                              double xmin, double ymin, double zmin,
                              double xmax, double ymax, double zmax)
{
    int                 i;
    _INdex3DStruct_     *iptr;

  /*
   * Make a new index object.
   */
    ActiveIndex =
      new Spatial3DTriangleIndex (xmin, ymin, zmin,
                                  xmax, ymax, zmax);
    if (ActiveIndex == NULL) {
        return -1;
    }

  /*
   * Put the new object into a previously deleted slot if possible.
   */
    for (i=0; i<NumIndexList; i++) {
        iptr = IndexList + i;
        if (iptr->index == NULL) {
            iptr->index = ActiveIndex;
            iptr->id1 = id1;
            iptr->id2 = id2;
            return 1;
        }
    }

  /*
   * Append to the end of the index list if there is room.
   */
    if (NumIndexList >= MAX_3D_INDEX_LIST) {
        return -1;
    }

    iptr = IndexList + NumIndexList;
    iptr->index = ActiveIndex;
    iptr->id1 = id1;
    iptr->id2 = id2;

    return 1;

}


/*-----------------------------------------------------------------------*/

/*
 * Set the geometry of the active 3d triangle index.  This returns 1 on success
 * or zero if there is no active index at the moment.
 */
int SWTriIndex::sw_Set3DTriangleIndexGeometry (double xmin, double ymin, double zmin,
                                   double xmax, double ymax, double zmax,
                                   double xspace, double yspace, double zspace)
{
    int            istat;

    if (ActiveIndex == NULL) {
        return 0;
    }

    istat =
      ActiveIndex->SetGeometry (xmin, ymin, zmin,
                                xmax, ymax, zmax,
                                xspace, yspace, zspace);

    return istat;

}

/*-------------------------------------------------------------------------*/

/*
 * Add a trimesh to the active index.  On success, 1 is returned.
 * If a memory allocation error occurs, -1 is returned.  If there
 * is no valid active index, zero is returned.
 */
int SWTriIndex::sw_Add3DTriangleIndexTriMesh (
    int       tmeshid,
    double    *xnode,
    double    *ynode,
    double    *znode,
    int       num_node,
    int       *n1edge,
    int       *n2edge,
    int       *t1edge,
    int       *t2edge,
    int       num_edge,
    int       *e1tri,
    int       *e2tri,
    int       *e3tri,
    int       num_tri)
{
    int           i, istat;
    NOdeStruct    *drapeNodes;
    EDgeStruct    *drapeEdges;
    TRiangleStruct   *drapeTriangles;

    if (ActiveIndex == NULL) {
        return 0;
    }

  /*
   * Put the data into node, edge and triangle lists.
   */
    drapeNodes = (NOdeStruct *)calloc
                 (1, num_node * sizeof(NOdeStruct));
    if (drapeNodes == NULL) {
        return -1;
    }
    drapeEdges = (EDgeStruct *)calloc
                 (1, num_edge * sizeof(EDgeStruct));
    if (drapeEdges == NULL) {
        free (drapeNodes);
        return -1;
    }
    drapeTriangles = (TRiangleStruct *)calloc
                     (1, num_tri * sizeof(TRiangleStruct));
    if (drapeTriangles == NULL) {
        free (drapeNodes);
        free (drapeEdges);
        return -1;
    }

    for (i=0; i<num_node; i++) {
        drapeNodes[i].x = xnode[i];
        drapeNodes[i].y = ynode[i];
        drapeNodes[i].z = znode[i];
    }

    for (i=0; i<num_edge; i++) {
        drapeEdges[i].node1 = n1edge[i];
        drapeEdges[i].node2 = n2edge[i];
        drapeEdges[i].tri1 = t1edge[i];
        drapeEdges[i].tri2 = t2edge[i];
    }

    for (i=0; i<num_tri; i++) {
        drapeTriangles[i].edge1 = e1tri[i];
        drapeTriangles[i].edge2 = e2tri[i];
        drapeTriangles[i].edge3 = e3tri[i];
    }

    istat =
      ActiveIndex->AddTriMesh (tmeshid,
                               drapeNodes,
                               drapeEdges,
                               drapeTriangles,
                               num_tri);

    free (drapeNodes);
    free (drapeEdges);
    free (drapeTriangles);

    return istat;

}


/*--------------------------------------------------------------------------*/

/*
 * Clear all the triangles from the active index, leaving the geometry
 * in place.
 */
void SWTriIndex::sw_Clear3DTriangleIndex (void)
{
    if (ActiveIndex == NULL) {
        return;
    }

    ActiveIndex->Clear ();

    return;
}



/*--------------------------------------------------------------------------*/

/*
 * Delete the active index.  This also sets the active index to NULL.
 * On success, 1 is returned.  If the ActiveIndex is invalid zero is
 * returned.
 */
int SWTriIndex::sw_Delete3DTriangleIndex (void)
{
    int              i;
    _INdex3DStruct_  *iptr;

    if (ActiveIndex == NULL) {
        return 0;
    }

  /*
   * Find the slot for the active index and delete it.
   */
    for (i=0; i<NumIndexList; i++) {
        iptr = IndexList + i;
        if (iptr->index == ActiveIndex) {
            delete ActiveIndex;
            iptr->index = NULL;
            iptr->id1 = 0;
            iptr->id2 = 0;
            return 1;
        }
    }

    return 0;

}


/*--------------------------------------------------------------------------*/

/*
 * Delete the index for the specified id's.  If this is also the active index,
 * the active index is set to NULL.  On success, 1 is returned.  If the index
 * cannot be found, zero is returned.
 */
int SWTriIndex::sw_Delete3DTriangleIndexID (int id1, int id2)
{
    int               i;
    _INdex3DStruct_   *iptr;

  /*
   * Find the slot for the specified index and delete it.
   */
    for (i=0; i<NumIndexList; i++) {
        iptr = IndexList + i;
        if (iptr->id1 == id1  &&  iptr->id2 == id2) {
            if (iptr->index == ActiveIndex) {
                ActiveIndex = NULL;
            }
            delete iptr->index;
            iptr->index = NULL;
            iptr->id1 = 0;
            iptr->id2 = 0;
            return 1;
        }
    }

    return 0;

}


/*----------------------------------------------------------------------------*/

/*
 * Get the triangles in the specified bounding box and send them back
 * to the java side.  If the active index is invalid, zero is returned.
 * If there is a memory allocation error, -1 is returned.  On success
 * 1 is returned.  Note that success includes an empty set of triangles
 * found in the area.  The java side needs to determine if any triangles
 * were found and sent back.
 */
int SWTriIndex::sw_Get3DTriangles (int tmeshid,
                       double xmin, double ymin, double zmin,
                       double xmax, double ymax, double zmax)
{
    int             istat;
    SPatial3DTriangleStructList    *stlist;

    if (ActiveIndex == NULL) {
        return 0;
    }

    stlist =
      ActiveIndex->GetTriangles (tmeshid,
                                 xmin, ymin, zmin,
                                 xmax, ymax, zmax);

    if (stlist == NULL) {
        return -1;
    }

    istat =
    SendBackTriangles (stlist);

    return istat;

}


/*---------------------------------------------------------------------------------*/

/*
 * Send the triangle numbers and trimesh numbers for the found
 * triangles back to the java side.  Returns 1 on success or
 * -1 on a memory allocation error.
 */
int SWTriIndex::SendBackTriangles (SPatial3DTriangleStructList *stlist)
{
    int        n, i, ntot;
    int        *tmeshid, *trinum;
    SPatial3DTriangleStruct   *sptr, *list;

    if (stlist == NULL) {
        return 1;
    }

    n = stlist->nlist;
    list = stlist->list;
    ntot = n;
    if (ntot < 1) ntot = 1;

    tmeshid = (int *)calloc (1, 2 * ntot * sizeof(int));
    if (tmeshid == NULL) {
        return -1;
    }
    trinum = tmeshid + ntot;

    for (i=0; i<n; i++) {
        sptr = list + i;
        tmeshid[i] = sptr->tmeshid;
        trinum[i] = sptr->trinum;
    }

    jni_call_add_native_tindex_result_method (tmeshid, trinum, n);

    return 1;
}
