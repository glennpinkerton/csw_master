
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
 * XYIndex2D class.  This class has the purpose of
 * maintaining a "3d grid" where each cell in the grid has a
 * list of the triangles that intersect or are close to the
 * grid cell.
 */

#include <assert.h>
#include <stdlib.h>


#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/surfaceworks/include/grd_xyindex.h"

/*-------------------------------------------------------------------------*/

/*
 * Constructor to make a new xy point index object.  The index object
 * must have its x,y bounds at a minimum to do anything useful with it, so
 * I force these to be defined in the constructor.  If they are defined badly
 * (i.e. the mins greater than max) then I consider this a programming error
 * and I assert.
 *
 * This constructor allocates an index grid that has equal spacing in x and y
 * with 50 grid cells in the largest dimension.  This will make the bounding box
 * slightly larger internally.
 *
 * This is a public method.
 */
XYIndex2D::XYIndex2D (
    double    xmin,
    double    ymin,
    double    xmax,
    double    ymax
)

{

    if (xmin >= xmax  ||
        ymin >= ymax) {
        assert (0);
    }

    IndexXmin = xmin;
    IndexYmin = ymin;
    IndexXmax = xmax;
    IndexYmax = ymax;

    IndexXspace = -1.0;
    IndexYspace = -1.0;

    IndexGrid = NULL;

    CreateIndexGrid ();

    geometryAllowed = 1;

}



/*--------------------------------------------------------------------*/

/*
 * Free the various memory in the destructor.
 */
XYIndex2D::~XYIndex2D ()
{
    int  i, ntot;

    ntot = IndexNcol * IndexNrow;

    if (IndexGrid != NULL) {
        for (i=0; i<ntot; i++) {
            csw_Free (IndexGrid[i]);
        }
    }
    csw_Free (IndexGrid);
}

/*--------------------------------------------------------------------*/

/*
 * Free all the memory that was added via AddPoints calls,
 * which empties the index.  You can use this if you want to
 * create a completely new index in the object, with the same
 * geometry as was originally used.
 *
 * This is a public method.
 */
void XYIndex2D::Clear (void)
{
    int  i, ntot;

    ntot = IndexNcol * IndexNrow;

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
 * Set the bounds and cell spacings for the xy index grid.
 * This can only be done if no points have been added to the
 * object yet.  In other words, the AddPoints method has not
 * been called yet.  If the AddPoints method was called prior
 * to calling this method, the return status is zero.  If a
 * memory allocation error occurs, the return status is -1.  On
 * success, the return status is 1.  If the specified parameters
 * are not valid (i.e min > max, space < 0) then zero is returned.
 *
 * This is a public method.
 */
int XYIndex2D::SetGeometry (
    double xmin,
    double ymin,
    double xmax,
    double ymax,
    double xspace,
    double yspace)
{
    int    istat;

    if (geometryAllowed == 0) {
        return 0;
    }

    if (xmin >= xmax  ||
        ymin >= ymax) {
        return 0;
    }

    if (xspace <= 0.0  ||
        yspace <= 0.0) {
        return 0;
    }

    IndexXmin = xmin;
    IndexYmin = ymin;
    IndexXmax = xmax;
    IndexYmax = ymax;

    IndexXspace = xspace;
    IndexYspace = yspace;

    istat = CreateIndexGrid ();

    return istat;

}


/*-------------------------------------------------------------------------*/

int XYIndex2D::AddPoints (double *x, double *y, int npts)
{
    int             i, istat;

    for (i=0; i<npts; i++) {
        istat = AddPoint (x[i], y[i], i);
        if (istat != 1) {
            return istat;
        }
    }

    return 1;

}


/*-------------------------------------------------------------------------*/

int XYIndex2D::AddPoint (
    double x,
    double y,
    int    pointNum)
{
    int         i1, j1,
                index, istat;

  /*
   * Find the grid cell containing the point.
   */
    j1 = (int)((x - IndexXmin) / IndexXspace);
    i1 = (int)((y - IndexYmin) / IndexYspace);

    if (i1 < 0) i1 = 0;
    if (i1 > IndexNrow - 1) i1 = IndexNrow - 1;
    if (j1 < 0) j1 = 0;
    if (j1 > IndexNcol - 1) j1 = IndexNcol - 1;

    index = i1 * IndexNcol + j1;

  /*
   * Add the point number to the list for the cell.
   */
    istat = AddToIndexGrid (index, pointNum);

    return istat;

}


/*-----------------------------------------------------------------------------*/

/*
 * Add the specified point number to the list for the
 * index'th grid cell.
 *
 * This is a private method.
 */
int XYIndex2D::AddToIndexGrid (int index, int pointNum)
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
        int *tlist = NULL;
        tlist = (int *)csw_Realloc (list, size * sizeof(int));
        if (tlist == NULL) {
            return -1;
        }
        list = tlist;
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
int XYIndex2D::CreateIndexGrid (void)
{

    double dx, dy, space, tmp;
    int    itmp, ntot;

    dx = (IndexXmax - IndexXmin);
    dy = (IndexYmax - IndexYmin);

    space = dx;
    if (dy > space) space = dy;

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

    if (IndexXspace <= 0.0) {
        IndexXspace = space;
        IndexYspace = space;
    }

    csw_Free (IndexGrid);
    IndexGrid = NULL;

    ntot = IndexNcol * IndexNrow;

    IndexGrid = (int **)csw_Calloc (ntot * sizeof(int *));

    if (IndexGrid == NULL) {
        IndexNcol = 0;
        IndexNrow = 0;
        return -1;
    }

    return 1;

}


/*---------------------------------------------------------------------------*/

/*
 *  Return a list of point numbers close to the specified location
 */
int XYIndex2D::GetClosePoints (
    double x, double y,
    int *listout,
    int *nlistout,
    int maxlist)
{
    int          i, j, index, n;
    int          *list, nlist;

    *nlistout = 0;
    if (IndexGrid == NULL) {
        return 0;
    }

    j = (int)((x - IndexXmin) / IndexXspace + .5);
    i = (int)((y - IndexYmin) / IndexYspace + .5);
    j--;
    i--;
    if (i < 0  ||  j < 0) {
        return 0;
    }
    if (i > IndexNrow-2  ||  j > IndexNcol-2) {
        return 0;
    }

    index = i * IndexNcol + j;

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

    *nlistout = n;

    return 1;

}

/*-----------------------------------------------------------------------*/

void XYIndex2D::GetIndexGrid (
    int         const ***igrid,
    int         *ncol,
    int         *nrow,
    double      *xmin,
    double      *ymin,
    double      *xmax,
    double      *ymax)
{
    *igrid = (int const **)IndexGrid;
    *ncol = IndexNcol;
    *nrow = IndexNrow;
    *xmin = IndexXmin;
    *ymin = IndexYmin;
    *xmax = IndexXmax;
    *ymax = IndexYmax;
}

