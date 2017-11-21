
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_gridvec.c

      This file has functions used for recording and retrieving polygon
    edge locations which are used as part of the polygon boolean functions.

*/

#include <math.h>

#include "csw/utils/include/csw_.h"

#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_gridvec.h"

#include "csw/utils/private_include/csw_scope.h"

/*
    define constants for the file
*/
#define LOCAL_MAX_GRID_SIZE         10000



/*
  ****************************************************************

                 p l y _ i n i t v e c g r i d

  ****************************************************************

    Initialize structures for polygon edge crossing grids.

*/

int CSWPolyGridvec::ply_initvecgrid (void)
{
    char          *ctmp;
    int           ndo;

/*  allocate memory for grids  */

    if (first) {
        first = 0;
        gstruct1.grid = NULL;
        gstruct2.grid = NULL;
        clipg = NULL;
        srcg = NULL;
        ctmp = (char *)csw_Malloc ((2 * LOCAL_MAX_GRID_SIZE + 20) * sizeof(char));
        if (!ctmp) {
            return -1;
        }
        gstruct1.grid = ctmp;
        gstruct2.grid = ctmp + LOCAL_MAX_GRID_SIZE + 2;
    }

/*
    zero the grids
*/
    ctmp = (char *)gstruct1.grid;
    ndo = 2 * LOCAL_MAX_GRID_SIZE + 20;
    memset (ctmp, 0, ndo);

    return 1;

}  /*  end of function ply_initvecgrid  */




/*
  ****************************************************************

                p l y _ s e t u p v e c g r i d s

  ****************************************************************

  Set up grids for locations of polygon edges for clip and
  source polygons.  This is called from ply_CalcIntersect or
  ply_CalcUnion only.  This function will return -1 if an
  error occurs or 1 on success.

*/

int CSWPolyGridvec::ply_setupvecgrids (double *xc, double *yc, int nc, int *ic,
                       double *xs, double *ys, int ns, int *is)
{
    int            istat, i, j, n, n1;
    double         x1, y1, x2, y2,
                   xg1, yg1, spac, ratio, xt, yt;
    CSWPolyUtils   ply_utils_obj;

/*  initialize and zero the grids  */

    istat = ply_initvecgrid ();
    if (istat != 1) {
        return -1;
    }

/*  set for clip in grid1 and src in grid2  */

    ply_SetSrcClipGrids (1);

/*  find limits of clip polygon  */

    istat = ply_utils_obj.ply_glimits (xc, yc, ic, nc,
                         &x1, &y1, &x2, &y2);
    if (istat) {
        return -1;
    }

    if (x2 <= x1  ||  y2 <= y1) {
        return -1;
    }
 
    spac = (x2 - x1 + y2 - y1) /1000.f;

    clipg->x1 = x1 - spac;
    clipg->y1 = y1 - spac;
    clipg->x2 = x2 + spac;
    clipg->y2 = y2 + spac;

/*  find limits of source polygon  */

    istat = ply_utils_obj.ply_glimits (xs, ys, is, ns,
                         &x1, &y1, &x2, &y2);
    if (istat) {
        return -1;
    }

    if (x2 <= x1  ||  y2 <= y1) {
        return -1;
    }

    spac = (x2 - x1 + y2 - y1) /1000.f;

    srcg->x1 = x1 - spac;
    srcg->y1 = y1 - spac;
    srcg->x2 = x2 + spac;
    srcg->y2 = y2 + spac;

/*  setup the same grid geometry for both source and clip grids  */    

    if (clipg->x1 < x1) x1 = clipg->x1;
    if (clipg->y1 < y1) y1 = clipg->y1;
    if (clipg->x2 > x2) x2 = clipg->x2;
    if (clipg->y2 > y2) y2 = clipg->y2;

    spac = (x2 - x1 + y2 - y1) / 100.f;
    x2 += spac;
    y2 += spac;
    x1 -= spac;
    y1 -= spac;

    ratio = (x2 - x1) / (y2 - y1);
    if (ratio < .1f) ratio = .1f;
    if (ratio > 10.f) ratio = 10.f;

    xt = (double)LOCAL_MAX_GRID_SIZE / ratio;
    Nrow = (int)sqrt(xt) - 1;
    Ncol = LOCAL_MAX_GRID_SIZE / Nrow - 1;

    xt = (x2 - x1) / Ncol;
    yt = (y2 - y1) / Nrow;
    spac = xt;
    if (yt > xt) spac = yt;
    
    xg1 = x1;
    yg1 = y1;

    clipg->xg1 = xg1;
    clipg->yg1 = yg1;
    clipg->spac = spac;
    clipg->nc = Ncol;
    clipg->nr = Nrow;

    srcg->xg1 = xg1;
    srcg->yg1 = yg1;
    srcg->spac = spac;
    srcg->nc = Ncol;
    srcg->nr = Nrow;

/*  set grid for clip polygon  */

    xgmin = xg1;
    ygmin = yg1;
    xspac = spac;
    yspac = spac;

    ActiveGrid = clipg->grid;

    n = 0;
    for (i=0; i<nc; i++) {
        n1 = n;
        for (j=0; j<ic[i]-1; j++) {
            ply_setgridforvec (xc[n], yc[n], xc[n+1], yc[n+1]);
            n++;
        }
        ply_setgridforvec (xc[n], yc[n], xc[n1], yc[n1]);
        n++;
    }

/*  set grid for source polygon  */

    ActiveGrid = srcg->grid;
    
    n = 0;
    for (i=0; i<ns; i++) {
        n1 = n;
        for (j=0; j<is[i]-1; j++) {
            ply_setgridforvec (xs[n], ys[n], xs[n+1], ys[n+1]);
            n++;
        }
        ply_setgridforvec (xs[n], ys[n], xs[n1], ys[n1]);
        n++;
    }

    return 1;

}  /*  end of function ply_setupvecgrids  */




/*
******************************************************************

                 p l y _ s e t g r i d f o r v e c

******************************************************************

  function name:    ply_setgridforvec    (integer)

  call sequence:    ply_setgridforvec (x1, y1, x2, y2)

  purpose:          set up grid data structure for a primitive vector

  return value:     status code

                    0 = normal successful completion
                    -1 = error allocating memory 

  calling parameters:

    x1    double          x coordinate of first endpoint of vector
    y1    double          y coordinate of first endpoint of vector
    x2    double          x coordinate of last endpoint
    y2    double          y coordinate of last endpoint

*/

int CSWPolyGridvec::ply_setgridforvec (double x1, double y1, double x2, double y2)
{
    double            bint, slope, x, y;
    int               row1, row2, col1, col2, itmp, offset, c1, c2, c2sav, i, j;
    char              *ctmp = NULL;

/*  calculate slope and intercept if non vertical  */

    if (x1 - x2 == 0.0) {
        slope = (y1-y2) / (x1-x2);
        bint = y1 - slope * x1;
    }
    else {
        slope = 1.e30f;
        bint = 1.e30f;
    }

/*  grid start and end rows and columns  */

    row1 = (int)((y1-ygmin) / yspac);
    row2 = (int)((y2-ygmin) / yspac);
    col1 = (int)((x1-xgmin) / xspac);
    col2 = (int)((x2-xgmin) / xspac);

/*  special case for near horizontal vector with endpoints in the same row  */

    if (row1 == row2 ) {

        if (col1 > col2) {
            itmp = col1;
            col1 = col2;
            col2 = itmp;
        }

        offset = row1 * Ncol + col1;

        ctmp = ActiveGrid + offset;

        for (i=col1; i<=col2; i++) {
            *ctmp = 1;
            ctmp++;
        }
    }

/*  special case for near vertical vector in only one column  */

    else if (col1 == col2) {

        if (row1 > row2) {
            itmp = row1;
            row1 = row2;
            row2 = itmp;
        }

        for (i=row1; i<=row2; i++) {
            offset = i * Ncol;
            ctmp = ActiveGrid + offset + col1;
            *ctmp = 1;
        }
    }

/*  general case where the vector crosses rows and columns  */

    else {

        if (row1 > row2) {
            itmp = row1;
            row1 = row2;
            row2 = itmp;
            itmp = col1;
            col1 = col2;
            col2 = itmp;
        }

        if (row2 < 0) {
            return 0;
        }
        if (row1 > Nrow) {
            return 0;
        }
        
        c1 = col1;

        for (i=row1; i<=row2; i++) {
            y = ygmin + (i+1) * yspac;
            x = (y - bint) / slope;
            c2 = (int)((x - xgmin) / xspac);
            if (i == row2) {
                c2 = col2;
            }
            c2sav = c2;
            if (c1 > c2) {
                itmp = c1;
                c1 = c2;
                c2 = itmp;
            }

            offset = i * Ncol;

            for (j=c1; j<=c2; j++) {
                ctmp = ActiveGrid + offset + j;
                *ctmp = 1;
            }
            c1 = c2sav;
        }
    }

    return 0 ;

}  /*  end of function ply_setgridforvec  */


/*
  ****************************************************************

                 p l y _ S e t A c t i v e G r i d

  ****************************************************************

    Set the the vector grid to either the clip or source grid.
    For val = -1, set to source  for val = 1, set to clip.

*/

int CSWPolyGridvec::ply_SetActiveGrid (int val)
{
    if (val == -1) {
        ActiveStruct = srcg;
    }
    else {
        ActiveStruct = clipg;
    }

    ActiveGrid = ActiveStruct->grid;

    xgmin = ActiveStruct->xg1;
    ygmin = ActiveStruct->yg1;
    Ncol = ActiveStruct->nc;
    Nrow = ActiveStruct->nr;
    xspac = ActiveStruct->spac;
    yspac = xspac;
    
    return 1;

}  /*  end of function ply_SetActiveGrid  */



/*
  ****************************************************************

               p l y _ S e t S r c C l i p G r i d s

  ****************************************************************

  Set the source and clip grid structure pointers to the proper
  structures.  If flag = 1, the original source and clip are
  used.  If flag != 1, the opposite is used.  This is called if
  the source and clip need to be switched in the intersection or
  union operations.

*/

int CSWPolyGridvec::ply_SetSrcClipGrids (int flag)
{
    
    if (flag == 1) {
        clipg = &gstruct1;
        srcg = &gstruct2;
    }
    else {
        clipg = &gstruct2;
        srcg = &gstruct1;
    }

    return 1;

}  /*  end of function ply_SetSrcClipGrids  */




/*
******************************************************************

                 p l y _ c h e c k g r i d f o r v e c

******************************************************************

  function name:    ply_checkgridforvec    (integer)

  call sequence:    ply_checkgridforvec (x1, y1, x2, y2)

  purpose:          check if the current active grid has any occupied
                    cells which are traversed by a specified vector.
                    This is called by ply_sidint to see if it is 
                    possible for a vector to intersect a polygon edge.

  return value:     status code

                    0 = no occupied cells
                    1 = at least 1 occupied cell

  calling parameters:

    x1    double          x coordinate of first endpoint of vector
    y1    double          y coordinate of first endpoint of vector
    x2    double          x coordinate of last endpoint
    y2    double          y coordinate of last endpoint

*/

int CSWPolyGridvec::ply_checkgridforvec (double x1, double y1, double x2, double y2)
{
    double            bint, slope, x, y;
    int               row1, row2, col1, col2, itmp, offset, c1, c2, c2sav, i, j;
    char              *ctmp = NULL;

/*  calculate slope and intercept if non vertical  */

    if (x1 - x2 == 0.0) {
        slope = (y1-y2) / (x1-x2);
        bint = y1 - slope * x1;
    }
    else {
        slope = 1.e30f;
        bint = 1.e30f;
    }

/*  grid start and end rows and columns  */

    row1 = (int)((y1-ygmin) / yspac);
    row2 = (int)((y2-ygmin) / yspac);
    col1 = (int)((x1-xgmin) / xspac);
    col2 = (int)((x2-xgmin) / xspac);

/*  special case for near horizontal vector with endpoints in the same row  */

    if (row1 == row2 ) {

        if (col1 > col2) {
            itmp = col1;
            col1 = col2;
            col2 = itmp;
        }

        offset = row1 * Ncol + col1;

        ctmp = ActiveGrid + offset;

        for (i=col1; i<=col2; i++) {
            if (*ctmp) {
                return 1;
            }
            ctmp++;
        }
    }

/*  special case for near vertical vector in only one column  */

    else if (col1 == col2) {

        if (row1 > row2) {
            itmp = row1;
            row1 = row2;
            row2 = itmp;
        }

        for (i=row1; i<=row2; i++) {
            offset = i * Ncol;
            ctmp = ActiveGrid + offset + col1;
            if (*ctmp) {
                return 1;
            }
        }
    }

/*  general case where the vector crosses rows and columns  */

    else {

        if (row1 > row2) {
            itmp = row1;
            row1 = row2;
            row2 = itmp;
            itmp = col1;
            col1 = col2;
            col2 = itmp;
        }

        if (row2 < 0) {
            return 0;
        }
        if (row1 > Nrow) {
            return 0;
        }
        
        c1 = col1;

        for (i=row1; i<=row2; i++) {
            y = ygmin + (i+1) * yspac;
            x = (y - bint) / slope;
            c2 = (int)((x - xgmin) / xspac);
            if (i == row2) {
                c2 = col2;
            }
            c2sav = c2;
            if (c1 > c2) {
                itmp = c1;
                c1 = c2;
                c2 = itmp;
            }

            offset = i * Ncol;

            for (j=c1; j<=c2; j++) {
                ctmp = ActiveGrid + offset + j;
                if (*ctmp) {
                    return 1;
                }
            }
            c1 = c2sav;
        }
    }

    return 0 ;

}  /*  end of function ply_checkgridforvec  */
