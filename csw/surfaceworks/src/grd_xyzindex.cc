
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * This file has the actual implementation of the
 * XYZIndex3D class.  This class has the purpose of
 * maintaining a "3d grid" where each cell in the grid has a
 * list of the triangles that intersect or are close to the
 * grid cell.
 */

#include <assert.h>
#include <stdlib.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/surfaceworks/include/grd_xyzindex.h"

/*-------------------------------------------------------------------------*/

/*
 * Constructor to make a new xyz point index object.  The xyz point index object
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
XYZIndex3D::XYZIndex3D (
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

    IndexNcol = 0;
    IndexNrow = 0;
    IndexNlevel = 0;

    IndexGrid = NULL;

    CreateIndexGrid ();

    xPointList = NULL;
    yPointList = NULL;
    zPointList = NULL;
    NumPointList = 0;
    MaxPointList = 0;

    geometryAllowed = 1;

}



/*--------------------------------------------------------------------*/

/*
 * Free the various memory in the destructor.
 */
XYZIndex3D::~XYZIndex3D ()
{
    int  i, ntot;

    csw_Free (xPointList);
    csw_Free (yPointList);
    csw_Free (zPointList);

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
 * Free all the memory that was added via Add... calls,
 * which empties the index.  You can use this if you want to
 * create a completely new index in the object, with the same
 * geometry as was originally used.
 *
 * This is a public method.
 */
void XYZIndex3D::Clear (void)
{
    int  i, ntot;

    csw_Free (xPointList);
    csw_Free (yPointList);
    csw_Free (zPointList);
    xPointList = yPointList = zPointList = NULL;

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
 * This can only be done if no points have been added to the
 * object yet.  In other words, none of the Add... methods has
 * been called yet.  If any Add... method was called prior
 * to calling this method, the return status is zero.  If a
 * memory allocation error occurs, the return status is -1.  On
 * success, the return status is 1.  If the specified parameters
 * are not valid (i.e min > max, space < 0) then zero is returned.
 *
 * This is a public method.
 */
int XYZIndex3D::SetGeometry (
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

/*
 * Add an array of trimesh nodes to the index and to the x, y, z
 * point lists.
 */
int XYZIndex3D::AddTriMeshNodes (
    NOdeStruct      *nodes,
    int             numnodes)
{
    int             i, istat;
    double          xt, yt, zt;
    NOdeStruct      *nptr;

  /*
   * Find the node coordinates for each triangle.  Add the triangle
   * to both the TriangleList and the IndexGrid.
   */
    for (i=0; i<numnodes; i++) {
        nptr = nodes + i;
        xt = nptr->x;
        yt = nptr->y;
        zt = nptr->z;
        istat = AddPoint (xt, yt, zt);
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}


/*-------------------------------------------------------------------------*/

/*
 * Add a list of points to the index and to the x, y, z lists.
 * If an input parameter is bad (NULL xyz or less than 1 npts), zero
 * is returned.  If a memory allocation error occurs, -1 is returned.
 * On success, 1 is returned.
 *
 * This is a public method.
 */

int XYZIndex3D::AddPoints (
    double *x,
    double *y,
    double *z,
    int    npts)
{
    int         i, istat;

    if (x == NULL  ||  y == NULL  ||  z == NULL) {
        return 0;
    }

    if (npts < 1) {
        return 0;
    }

    for (i=0; i<npts; i++) {
        istat = AddPoint (x[i], y[i], z[i]);
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}



/*--------------------------------------------------------------------------------*/

/*
 * Add a single point to the index and to the x, y, z lists.  On success
 * this method returns 1.  On a memory allocation failure, -1 is returned.
 * If the specified point is outside of the bounds of the index, zero
 * is returned.
 */
int XYZIndex3D::AddPoint (double x, double y, double z)
{
    int         irow, jcol, klevel;
    int         index, koffset, ioffset;
    int         n, istat;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            csw_Free (xPointList);
            csw_Free (yPointList);
            csw_Free (zPointList);
            xPointList = yPointList = zPointList = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


  /*
   * Grow the point lists if needed.
   */
    if (xPointList == NULL  ||
        NumPointList >= MaxPointList) {
        MaxPointList += 200;
        xPointList = (double *)csw_Realloc (xPointList,
                                        MaxPointList * sizeof(double));
        yPointList = (double *)csw_Realloc (yPointList,
                                        MaxPointList * sizeof(double));
        zPointList = (double *)csw_Realloc (zPointList,
                                        MaxPointList * sizeof(double));
    }

    if (xPointList == NULL  ||
        yPointList == NULL  ||
        zPointList == NULL) {
        csw_Free (xPointList);
        csw_Free (yPointList);
        csw_Free (zPointList);
        xPointList = yPointList = zPointList = NULL;
        return -1;
    }

    n = NumPointList;

    xPointList[n] = x;
    yPointList[n] = y;
    zPointList[n] = z;

  /*
   * Find the grid cell holding the point.
   */
    irow = (int)((y - IndexYmin) / IndexYspace);
    jcol = (int)((x - IndexXmin) / IndexXspace);
    klevel = (int)((z - IndexZmin) / IndexZspace);

    if (irow < 0  ||  jcol < 0  ||  klevel < 0) {
        return 0;
    }
    if (irow >= IndexNrow  ||
        jcol >= IndexNcol  ||
        klevel >= IndexNlevel) {
        return 0;
    }

    koffset = klevel * IndexNcol * IndexNrow;
    ioffset = koffset + irow * IndexNcol;
    index = ioffset + jcol;

    istat = AddToIndexGrid (index, n);
    if (istat == -1) {
        return -1;
    }

    NumPointList++;

    bsuccess = true;

    return 1;

}


/*-----------------------------------------------------------------------------*/

/*
 * Add the specified point number to the indexth cell of the grid.
 *
 * This is a private method.
 */
int XYZIndex3D::AddToIndexGrid (int index, int pointNum)
{
    int           *list = NULL, size, n;

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
    list[n] = pointNum;
    list[1]++;
    IndexGrid[index] = list;

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
int XYZIndex3D::CreateIndexGrid (void)
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
 *  Return a list of point numbers close to the specified location
 *
 *  This is a public method.
 */
int XYZIndex3D::GetClosePoints (
    double x, double y, double z,
    int *listout,
    int *nlistout,
    int maxlist)
{
    int          i, j, k, irow, index, n;
    int          *list, nlist;
    int          nlev;

    *nlistout = 0;
    if (IndexGrid == NULL) {
        return 0;
    }

    k = (int)((z - IndexZmin) / IndexZspace + .5);
    j = (int)((x - IndexXmin) / IndexXspace + .5);
    i = (int)((y - IndexYmin) / IndexYspace + .5);
    if (i < 0  ||  j < 0  ||  k < 0) {
        return 0;
    }
    if (i > IndexNrow-1  ||  j > IndexNcol-1  ||  k > IndexNlevel-1) {
        return 0;
    }
    k--;
    j--;
    i--;
    if (i < 0) i = 0;
    if (j < 0) j = 0;
    if (k < 0) k = 0;
    if (i > IndexNrow - 2) i = IndexNrow - 2;
    if (j > IndexNcol - 2) j = IndexNcol - 2;
    if (k > IndexNlevel - 2) k = IndexNlevel - 2;

    irow = i;

    nlev = IndexNcol * IndexNrow;
    index = k * nlev + irow * IndexNcol + j;

    n = 0;

    list = IndexGrid[index];
    if (list) {
        nlist = list[1];
        for (i=0; i<nlist; i++) {
            if (n < maxlist) {
                listout[n] = list[i+2];
                n++;
            }
        }
    }
    list = IndexGrid[index+1];
    if (list) {
        nlist = list[1];
        for (i=0; i<nlist; i++) {
            if (n < maxlist) {
                listout[n] = list[i+2];
                n++;
            }
        }
    }
    list = IndexGrid[index+IndexNcol];
    if (list) {
        nlist = list[1];
        for (i=0; i<nlist; i++) {
            if (n < maxlist) {
                listout[n] = list[i+2];
                n++;
            }
        }
    }
    list = IndexGrid[index+IndexNcol+1];
    if (list) {
        nlist = list[1];
        for (i=0; i<nlist; i++) {
            if (n < maxlist) {
                listout[n] = list[i+2];
                n++;
            }
        }
    }

    index = (k + 1) * nlev + irow * IndexNcol + j;

    list = IndexGrid[index];
    if (list) {
        nlist = list[1];
        for (i=0; i<nlist; i++) {
            if (n < maxlist) {
                listout[n] = list[i+2];
                n++;
            }
        }
    }
    list = IndexGrid[index+1];
    if (list) {
        nlist = list[1];
        for (i=0; i<nlist; i++) {
            if (n < maxlist) {
                listout[n] = list[i+2];
                n++;
            }
        }
    }
    list = IndexGrid[index+IndexNcol];
    if (list) {
        nlist = list[1];
        for (i=0; i<nlist; i++) {
            if (n < maxlist) {
                listout[n] = list[i+2];
                n++;
            }
        }
    }
    list = IndexGrid[index+IndexNcol+1];
    if (list) {
        nlist = list[1];
        for (i=0; i<nlist; i++) {
            if (n < maxlist) {
                listout[n] = list[i+2];
                n++;
            }
        }
    }

    *nlistout = n;

    return 1;

}


/*--------------------------------------------------------------------------*/

/*
 * Return the x, y, z coordinates for a point number.
 */
int XYZIndex3D::GetPointXYZ (int pointnum,
                             double *x, double *y, double *z)
{
    *x = *y = *z = 1.e30;
    if (pointnum < 0  ||  pointnum >= NumPointList) {
        return 0;
    }

    if (xPointList == NULL  ||
        yPointList == NULL  ||
        zPointList == NULL) {
        return 0;
    }

    *x = xPointList[pointnum];
    *y = yPointList[pointnum];
    *z = zPointList[pointnum];

    return 1;

}
