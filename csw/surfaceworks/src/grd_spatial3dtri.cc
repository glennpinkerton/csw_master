
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * This file has the actual implementation of the
 * Spatial3DTriangleIndex class.  This class has the purpose of
 * maintaining a "3d grid" where each cell in the grid has a
 * list of the triangles that intersect or are close to the
 * grid cell.
 */

#include <assert.h>
#include <stdlib.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/surfaceworks/include/grd_spatial3dtri.h"

/*-------------------------------------------------------------------------*/

/*
 * Constructor to make a new triangle index object.  The triangle index object
 * must have its x,y,z bounds at a minimum to do anything useful with it, so
 * I force these to be defined in the constructor.  If they are defined badly
 * (i.e. the mins greater than max) then I consider this a programming error
 * and I assert.
 *
 * This constructor allocates an index grid that has equal spacing in x, y and z
 * with 50 grid cells in the largest dimension.  This will make the bounding box
 * slightly larger internally.
 *
 * This is a public method.
 */
Spatial3DTriangleIndex::Spatial3DTriangleIndex (
    double    xmin,
    double    ymin,
    double    zmin,
    double    xmax,
    double    ymax,
    double    zmax
)

{

    if (xmin >= xmax  ||
        ymin >= ymax  ||
        zmin >= zmax) {
        assert (0);
    }

    IndexXmin = xmin;
    IndexYmin = ymin;
    IndexZmin = zmin;
    IndexXmax = xmax;
    IndexYmax = ymax;
    IndexZmax = zmax;

    IndexXspace = -1.0;
    IndexYspace = -1.0;
    IndexZspace = -1.0;

    IndexGrid = NULL;

    CreateIndexGrid ();

    TriangleList = NULL;
    NumTriangleList = 0;
    MaxTriangleList = 0;

    geometryAllowed = 1;

}



/*--------------------------------------------------------------------*/

/*
 * Free the various memory in the destructor.
 */
Spatial3DTriangleIndex::~Spatial3DTriangleIndex ()
{
    int  i, ntot;

    csw_Free (TriangleList);

    ntot = IndexNcol * IndexNrow * IndexNlevel;

    if (IndexGrid != NULL) {
        for (i=0; i<ntot; i++) {
            csw_Free (IndexGrid[i]);
        }
    }
    csw_Free (IndexGrid);
}

/*--------------------------------------------------------------------*/

/*
 * Free all the memory that was added via AddTriMesh calls,
 * which empties the index.  You can use this if you want to
 * create a completely new index in the object, with the same
 * geometry as was originally used.
 *
 * This is a public method.
 */
void Spatial3DTriangleIndex::Clear (void)
{
    int  i, ntot;

    csw_Free (TriangleList);
    TriangleList = NULL;

    ntot = IndexNcol * IndexNrow * IndexNlevel;

    if (IndexGrid != NULL) {
        for (i=0; i<ntot; i++) {
            csw_Free (IndexGrid[i]);
        }
    }
    csw_Free (IndexGrid);
    IndexGrid = NULL;

    return;
}

/*--------------------------------------------------------------------*/

/*
 * Set the bounds and cell spacings for the 3d index grid.
 * This can only be done if no triangles have been added to the
 * object yet.  In other words, the AddTriMesh method has not
 * been called yet.  If the AddTriMesh method was called prior
 * to calling this method, the return status is zero.  If a
 * memory allocation error occurs, the return status is -1.  On
 * success, the return status is 1.  If the specified parameters
 * are not valid (i.e min > max, space < 0) then zero is returned.
 *
 * This is a public method.
 */
int Spatial3DTriangleIndex::SetGeometry (
    double xmin,
    double ymin,
    double zmin,
    double xmax,
    double ymax,
    double zmax,
    double xspace,
    double yspace,
    double zspace)
{
    int    istat;

    if (geometryAllowed == 0) {
        return 0;
    }

    if (xmin >= xmax  ||
        ymin >= ymax  ||
        zmin >= zmax) {
        return 0;
    }

    if (xspace <= 0.0  ||
        yspace <= 0.0  ||
        zspace <= 0.0) {
        return 0;
    }

    IndexXmin = xmin;
    IndexYmin = ymin;
    IndexZmin = zmin;
    IndexXmax = xmax;
    IndexYmax = ymax;
    IndexZmax = zmax;

    IndexXspace = xspace;
    IndexYspace = yspace;
    IndexZspace = zspace;

    istat = CreateIndexGrid ();

    return istat;

}


/*-------------------------------------------------------------------------*/

int Spatial3DTriangleIndex::AddTriMesh (
    int             trimeshid,
    NOdeStruct      *nodes,
    EDgeStruct      *edges,
    TRiangleStruct  *triangles,
    int             numtriangles)
{
    int             i, n, istat;
    int             n1, n2, n3;
    double          x[3], y[3], z[3];
    TRiangleStruct  *tptr;
    SPatial3DTriangleStruct *stptr;

  /*
   * Make the TriangleList big enough to hold all of the
   * triangles plus the triangles the list already has in it.
   */
    n = NumTriangleList + numtriangles;
    if (TriangleList == NULL  ||
        n >= MaxTriangleList) {
        MaxTriangleList = n;
        TriangleList =
          (SPatial3DTriangleStruct *)csw_Realloc
            (TriangleList,
             MaxTriangleList * sizeof(SPatial3DTriangleStruct));
    }

    if (TriangleList == NULL) {
        return -1;
    }

  /*
   * Find the node coordinates for each triangle.  Add the triangle
   * to both the TriangleList and the IndexGrid.
   */
    for (i=0; i<numtriangles; i++) {
        tptr = triangles + i;

      /*
       * Get the x,y,z of the corner points.
       */
        istat =
        grd_triangle_ptr->grd_get_nodes_for_triangle (
            tptr, edges,
            &n1, &n2, &n3);
        if (istat != 1) {
            continue;
        }
        x[0] = nodes[n1].x;
        y[0] = nodes[n1].y;
        z[0] = nodes[n1].z;
        x[1] = nodes[n2].x;
        y[1] = nodes[n2].y;
        z[1] = nodes[n2].z;
        x[2] = nodes[n3].x;
        y[2] = nodes[n3].y;
        z[2] = nodes[n3].z;

      /*
       * Add the triangle to the TriangleList array.
       */
        stptr = TriangleList + NumTriangleList;
        stptr->tmeshid = trimeshid;
        stptr->trinum = i;
        stptr->used = 0;

      /*
       * Put the triangle into the index grid.
       */
        istat =
        AddTriangle (
            x, y, z, NumTriangleList);
        if (istat == -1) {
            return -1;
        }

        NumTriangleList++;

    }

    return 1;

}


/*-------------------------------------------------------------------------*/

/*
 * Add a triangle to the index given the x,y,z locations of its
 * corner points and the position of the triangle in the
 * TriangleList array.
 *
 * This is a private method.
 */

int Spatial3DTriangleIndex::AddTriangle (
    double *x,
    double *y,
    double *z,
    int    triangleListNum)
{
    double      xmin, ymin, zmin,
                xmax, ymax, zmax;
    int         i1, i2, j1, j2, k1, k2,
                ii, jj, kk, index, istat;
    int         i, koffset, ioffset;


  /*
   * Find the 3d bounding box of the triangle.
   */
    xmin = ymin = zmin = 1.e30;
    xmax = ymax = zmax = -1.e30;

    for (i=0; i<3; i++) {
        if (x[i] < xmin) xmin = x[i];
        if (y[i] < ymin) ymin = y[i];
        if (z[i] < zmin) zmin = z[i];
        if (x[i] > xmax) xmax = x[i];
        if (y[i] > ymax) ymax = y[i];
        if (z[i] > zmax) zmax = z[i];
    }

    if (xmax < xmin  ||
        ymax < ymin  ||
        zmax < zmin) {
        return 0;
    }

  /*
   * Find the grid cell corners of the bounding box.
   */
    j1 = (int)((xmin - IndexXmin) / IndexXspace);
    i1 = (int)((ymin - IndexYmin) / IndexYspace);
    k1 = (int)((zmin - IndexZmin) / IndexZspace);
    j2 = (int)((xmax - IndexXmin) / IndexXspace);
    i2 = (int)((ymax - IndexYmin) / IndexYspace);
    k2 = (int)((zmax - IndexZmin) / IndexZspace);

    if (i1 < 0) i1 = 0;
    if (i1 > IndexNrow - 1) i1 = IndexNrow - 1;
    if (j1 < 0) j1 = 0;
    if (j1 > IndexNcol - 1) j1 = IndexNcol - 1;
    if (k1 < 0) k1 = 0;
    if (k1 > IndexNlevel - 1) k1 = IndexNlevel - 1;

    if (i2 < 0) i2 = 0;
    if (i2 > IndexNrow - 1) i2 = IndexNrow - 1;
    if (j2 < 0) j2 = 0;
    if (j2 > IndexNcol - 1) j2 = IndexNcol - 1;
    if (k2 < 0) k2 = 0;
    if (k2 > IndexNlevel - 1) k2 = IndexNlevel - 1;

  /*
   * Loop through the cells that are in the bounding box and
   * add the triangleListNum to each cell.
   */
    for (kk=k1; kk<=k2; kk++) {
        koffset = kk * IndexNcol * IndexNrow;
        for (ii=i1; ii<=i2; ii++) {
            ioffset = koffset + ii * IndexNcol;
            for (jj=j1; jj<=j2; jj++) {
                index = ioffset + jj;
                istat = AddToIndexGrid (index, triangleListNum);
                if (istat == -1) {
                    return -1;
                }
            }
        }
    }

    return 1;

}


/*-----------------------------------------------------------------------------*/

/*
 * Add the specified triangle list number to the list for the
 * index'th grid cell.
 *
 * This is a private method.
 */
int Spatial3DTriangleIndex::AddToIndexGrid (int index, int triangleListNum)
{
    int           *list = NULL, size, n;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (list);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (IndexGrid == NULL) {
        return -1;
    }

    list = IndexGrid[index];
    if (list == NULL) {
        list = (int *)csw_Malloc (7 * sizeof(int));
        if (list == NULL) {
            return -1;
        }
        list[0] = 5;
        list[1] = 0;
    }

    if (list[1] >= list[0]) {
        size = list[0] + 7;
        list = (int *)csw_Realloc (list, size * sizeof(int));
        if (list == NULL) {
            return -1;
        }
        list[0] = size - 2;
    }

    n = list[1] + 2;
    list[n] = triangleListNum;
    list[1]++;
    IndexGrid[index] = list;

    bsuccess = true;

    return 1;

}



/*--------------------------------------------------------------------*/

/*
 * Create an index grid based on the current bounds and cell
 * spacings of this object.  If the cell spacings are not valid,
 * use a default that will create about 50 cells in the longest
 * dimension.
 *
 * This is a private method.
 */
int Spatial3DTriangleIndex::CreateIndexGrid (void)
{

    double dx, dy, dz, space, tmp;
    int    itmp, ntot;

    dx = (IndexXmax - IndexXmin);
    dy = (IndexYmax - IndexYmin);
    dz = (IndexZmax - IndexZmin);

    space = dx;
    if (dy > space) space = dy;
    if (dz > space) space = dz;

    space /= 50.0;

    if (IndexXspace > 0.0) {
        space = IndexXspace;
    }
    itmp = (int)(dx / space) + 1;
    if (itmp < 1) itmp = 1;
    IndexNcol = itmp + 1;
    tmp = itmp * space;
    tmp -= dx;
    tmp /= 2.0;
    if (tmp < 0.0) tmp = 0.0;
    IndexXmin -= tmp;
    IndexXmax += tmp;

    if (IndexYspace > 0.0) {
        space = IndexYspace;
    }
    itmp = (int)(dy / space) + 1;
    if (itmp < 1) itmp = 1;
    IndexNrow = itmp + 1;
    tmp = itmp * space;
    tmp -= dy;
    tmp /= 2.0;
    if (tmp < 0.0) tmp = 0.0;
    IndexYmin -= tmp;
    IndexYmax += tmp;

    if (IndexZspace > 0.0) {
        space = IndexZspace;
    }
    itmp = (int)(dz / space) + 1;
    if (itmp < 1) itmp = 1;
    IndexNlevel = itmp + 1;
    tmp = itmp * space;
    tmp -= dz;
    tmp /= 2.0;
    if (tmp < 0.0) tmp = 0.0;
    IndexZmin -= tmp;
    IndexZmax += tmp;

    if (IndexXspace <= 0.0) {
        IndexXspace = space;
        IndexYspace = space;
        IndexZspace = space;
    }

    csw_Free (IndexGrid);
    IndexGrid = NULL;

    ntot = IndexNcol * IndexNrow * IndexNlevel;

    IndexGrid = (int **)csw_Calloc (ntot * sizeof(int *));

    if (IndexGrid == NULL) {
        IndexNcol = 0;
        IndexNrow = 0;
        IndexNlevel = 0;
        return -1;
    }

    return 1;

}


/*---------------------------------------------------------------------------*/

/*
 * Return a list of triangles that might possibly intersect the specified
 * 3d bounds.  The list may have more triangles than actually intersect the
 * bounds, but it will never leave out a triangle that intersects the bounds.
 *
 * If the tmeshid is greater than or equal to zero, it acts as a filter for
 * the results.  No triangles having that tmeshid will be returned.
 *
 * On success a pointer to a SPatial3DTriangleStructList structure is returned.
 * On failure Null is returned.  If no triangles intersect the area, a valid
 * structure is still returned, and its list member is NULL, and its nlist
 * member is zero.
 *
 * The only reason a NULL pointer is returned is a memory allocation failure.
 *
 * The return structure is allocated here, but it is the responsibility of
 * the calling function to csw_Free it when needed.
 */

SPatial3DTriangleStructList *Spatial3DTriangleIndex::GetTriangles (
    int tmeshid,
    double xmin, double ymin, double zmin,
    double xmax, double ymax, double zmax)

{
    int         i1, i2, j1, j2, k1, k2,
                ii, jj, kk, index;
    int         koffset, ioffset;
    int         *local_list = NULL, nlocal, maxlocal, nout;
    int         ndo, n, *list = NULL;
    SPatial3DTriangleStructList    *stlist = NULL;
    SPatial3DTriangleStruct        *stptr = NULL, *stout = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (stlist);
            csw_Free (local_list);
            csw_Free (stout);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (IndexGrid == NULL  ||  TriangleList == NULL) {
        return NULL;
    }

  /*
   * Allocate space for the output structure.  Initialize it
   * as empty in case of an error or in case no triangles are found.
   */
    stlist = (SPatial3DTriangleStructList *)csw_Calloc
        (sizeof(SPatial3DTriangleStructList));
    if (stlist == NULL) {
        return NULL;
    }

    if (xmax <= xmin  ||
        ymax <= ymin  ||
        zmax <= zmin) {
        bsuccess = true;
        return stlist;
    }

  /*
   * Find the grid cell corners of the bounding box.
   */
    j1 = (int)((xmin - IndexXmin) / IndexXspace);
    i1 = (int)((ymin - IndexYmin) / IndexYspace);
    k1 = (int)((zmin - IndexZmin) / IndexZspace);
    j2 = (int)((xmax - IndexXmin) / IndexXspace);
    i2 = (int)((ymax - IndexYmin) / IndexYspace);
    k2 = (int)((zmax - IndexZmin) / IndexZspace);

    if (i2 < 0  ||  j2 < 0  ||  k2 < 0) {
        return NULL;
    }

    if (i1 >= IndexNrow  ||  j1 >= IndexNcol  ||  k1 >= IndexNlevel) {
        return NULL;
    }

    if (i1 < 0) i1 = 0;
    if (i1 > IndexNrow - 1) i1 = IndexNrow - 1;
    if (j1 < 0) j1 = 0;
    if (j1 > IndexNcol - 1) j1 = IndexNcol - 1;
    if (k1 < 0) k1 = 0;
    if (k1 > IndexNlevel - 1) k1 = IndexNlevel - 1;

    if (i2 < 0) i2 = 0;
    if (i2 > IndexNrow - 1) i2 = IndexNrow - 1;
    if (j2 < 0) j2 = 0;
    if (j2 > IndexNcol - 1) j2 = IndexNcol - 1;
    if (k2 < 0) k2 = 0;
    if (k2 > IndexNlevel - 1) k2 = IndexNlevel - 1;

  /*
   * Loop through the cells that are in the bounding box and
   * add the triangles one time each to the local triangle number list.
   */
    local_list = (int *)csw_Malloc (100 * sizeof(int));
    if (local_list == NULL) {
        return NULL;
    }
    nlocal = 0;
    maxlocal = 100;
    for (kk=k1; kk<=k2; kk++) {
        koffset = kk * IndexNcol * IndexNrow;
        for (ii=i1; ii<=i2; ii++) {
            ioffset = koffset + ii * IndexNcol;
            for (jj=j1; jj<=j2; jj++) {
                index = ioffset + jj;
                list = IndexGrid[index];
                if (list != NULL) {
                    ndo = list[1];
                    for (n=0; n<ndo; n++) {
                        stptr = TriangleList + list[n+2];
                        if (stptr->used == 0) {
                            if (nlocal >= maxlocal) {
                                maxlocal += 100;
                                local_list = (int *)csw_Realloc
                                  (local_list, maxlocal * sizeof(int));
                                if (local_list == NULL) {
                                    return NULL;
                                }
                            }
                            local_list[nlocal] = list[n+2];
                            stptr->used = 1;
                            nlocal++;
                        }
                    }
                }
            }
        }
    }

  /*
   * If no triangles were found, return the empty structure.
   */
    if (nlocal == 0) {
        return stlist;
    }

  /*
   * Reset the used flags in case an error occurs in allocating the output array.
   */
    for (n=0; n<nlocal; n++) {
        stptr = TriangleList + local_list[n];
        stptr->used = 0;
    }

  /*
   * Allocate space for the output triangles.
   */
    stout = (SPatial3DTriangleStruct *)csw_Calloc
      (nlocal * sizeof(SPatial3DTriangleStruct));
    if (stout == NULL) {
        return NULL;
    }

  /*
   * Fill in the output triangle list and return it.
   */
    nout = 0;
    for (n=0; n<nlocal; n++) {
        stptr = TriangleList + local_list[n];
        if (stptr->tmeshid == tmeshid) {
            continue;
        }
        stout[nout].tmeshid = stptr->tmeshid;
        stout[nout].trinum = stptr->trinum;
        nout++;
    }
    stlist->list = stout;
    stlist->nlist = nout;

    bsuccess = true;

    return stlist;

}
