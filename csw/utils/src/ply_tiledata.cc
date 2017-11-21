
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_tiledata.c

      This file has functions for getting vectors, arcs, and
    polygons to draw in pattern fills.  The primitives returned
    are translated and scaled to the same coordinate system as
    the polygon.

*/

#include <string.h>
#include <stdio.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/ply_fpatP.h"
#include "csw/utils/private_include/ply_patdatP.h"

#include "csw/utils/private_include/ply_tiledata.h"
#include "csw/utils/private_include/csw_scope.h"


/*
  ****************************************************************

              p l y _ S e t T i l e N u m

  ****************************************************************

    Set the pattern number for subsequent data fetches of tile
    primitives.  If this is not called, no tile fill data will
    be returned by any of the functions.

*/

int CSWPolyTiledata::ply_SetTileNum (int num)
{
    PatNum = num;
    return 1;

}  /*  end of function ply_SetTileNum  */


/*
  ****************************************************************

              p l y _ S e t T i l e S c a l e

  ****************************************************************

    Set the pattern scale factor for scaling of subsequent primitives.
    If this is not called a scale factor of 1.0 is used.

*/

int CSWPolyTiledata::ply_SetTileScale (CSW_F num)
{
    PatScale = num;
    return 1;

}  /*  end of function ply_SetTileScale  */



/*
  ****************************************************************

              p l y _ S e t T i l e O r i g i n

  ****************************************************************

    Set the coordinates of the lower left corner of the tile.
    These coordinates are added to all scaled primitive coords
    that are returned from subsequent calls to get data.

*/

int CSWPolyTiledata::ply_SetTileOrigin (CSW_F x, CSW_F y)
{
    TileX0 = x;
    TileY0 = y;

    return 1;

}  /*  end of function ply_SetTileOrigin  */





/*
  ****************************************************************

                p l y _ G e t T i l e V e c s

  ****************************************************************

    Get the vector strokes for the current PatNum and scale and
    translate them according to PatScale, TileX0, and TileY0.
    The resulting vectors are in plot scale units, as are the
    polygon vertices but the vectors are not clipped to the polygon.    

    The vectors are returned in x and y, and they are packed
    in contiguous groups according to ic.  The number of groups 
    is returned in nc.  The return status is always 1.  If no 
    vectors are available, both nc and ic[0] will be zero.

*/

int CSWPolyTiledata::ply_GetTileVecs (CSW_F *x, CSW_F *y, int *ic, int *nc)
{
    int             i, j, n, nvec, *itp = NULL;
    FIllVecRec      *vtmp = NULL, *vt0 = NULL;
    FILLPATINT      *ptmp = NULL, pt;
    CSW_F           *xtp = NULL, *ytp = NULL, ft;

    *nc = 0;
    ic[0] = 0;

    n = PatNum;
    if (n < 1  ||  n > PLYMAXPAT) {
        ic[0] = 0;
        *nc = 0;
        return 1;
    }

    n--;
    nvec = TileRecs[n].nvec;
    if (nvec < 1) {
        ic[0] = 0;
        *nc = 0;
        return 1;
    }

    xtp = x;
    ytp = y;
    itp = ic;
    vt0 = TileVectors + TileRecs[n].vec1;

    for (i=0; i<nvec; i++) {
        vtmp = vt0 + i;
        itp[i] = vtmp->npt / 2;
        ptmp = TilePoints + vtmp->pt1;
        for (j=0; j<vtmp->npt; j++) {
            pt = *(ptmp + j);
            if (j%2 == 0) {
                ft = (CSW_F)pt;
                *xtp = ft / 200.f * PatScale + TileX0;
                xtp++;
            }
            else {
                ft = (CSW_F)pt;
                *ytp = ft / 200.f * PatScale + TileY0;
                ytp++;
            }
        }
    }

    *nc = nvec;

    return 1;

}  /*  end of function ply_GetTileVecs  */


/*
  ****************************************************************

                p l y _ G e t T i l e A r c s

  ****************************************************************

    Get the circles for the current PatNum and scale and
    translate them according to PatScale, TileX0, and TileY0.
    The resulting circles are in plot scale units, as are the
    polygon vertices but the circles are not clipped to the polygon.    

    The circles are returned in x, y, and r.  The number of circles 
    is returned in nc.  The return status is always 1.  If no 
    circles are available, nc will be zero.

*/

int CSWPolyTiledata::ply_GetTileArcs (CSW_F *x, CSW_F *y, CSW_F *r, int *nc)
{
    int             i, n, narc;
    FIllArcRec      *atmp = NULL, *at0 = NULL;
    CSW_F           *xtp = NULL, *ytp = NULL, *rtp = NULL, ft;

    *nc = 0;

    n = PatNum;
    if (n < 1  ||  n > PLYMAXPAT) {
        *nc = 0;
        return 1;
    }

    n--;
    narc = TileRecs[n].narc;
    if (narc < 1) {
        *nc = 0;
        return 1;
    }

    xtp = x;
    ytp = y;
    rtp = r;
    at0 = TileArcs + TileRecs[n].arc1;

    for (i=0; i<narc; i++) {
        atmp = at0 + i;
        ft = (CSW_F)atmp->x;
        *xtp = ft / 200.f * PatScale + TileX0;
        ft = (CSW_F)atmp->y;
        *ytp = ft / 200.f * PatScale + TileY0;
        ft = (CSW_F)atmp->r;
        *rtp = (CSW_F)ft / 200.f * PatScale;
        xtp++;
        ytp++;
        rtp++;
    }

    *nc = narc;

    return 1;

}  /*  end of function ply_GetTileArcs  */


/*
  ****************************************************************

                p l y _ G e t T i l e P o l y s

  ****************************************************************

    Get the polygon vertices for the current PatNum and scale and
    translate them according to PatScale, TileX0, and TileY0.
    The resulting vectors are in plot scale units, as are the
    polygon vertices but the vectors are not clipped to the polygon.    

    The vectors are returned in x and y, and they are packed
    in contiguous groups according to ic.  The number of groups 
    is returned in nc.  The return status is always 1.  If no 
    vectors are available, both nc and ic[0] will be zero.

*/

int CSWPolyTiledata::ply_GetTilePolys (CSW_F *x, CSW_F *y, int *ic, int *nc)
{
    int             i, j, n, nvec, *itp = NULL;
    FIllVecRec      *vtmp = NULL, *vt0 = NULL;
    FILLPATINT      *ptmp = NULL, pt;
    CSW_F           *xtp = NULL, *ytp = NULL, ft;

    *nc = 0;
    ic[0] = 0;

    n = PatNum;
    if (n < 1  ||  n > PLYMAXPAT) {
        ic[0] = 0;
        *nc = 0;
        return 1;
    }

    n--;
    nvec = TileRecs[n].nfvec;
    if (nvec < 1) {
        ic[0] = 0;
        *nc = 0;
        return 1;
    }

    xtp = x;
    ytp = y;
    itp = ic;
    vt0 = TilePolygons + TileRecs[n].fvec1;

    for (i=0; i<nvec; i++) {
        vtmp = vt0 + i;
        itp[i] = vtmp->npt / 2;
        ptmp = TilePoints + vtmp->pt1;
        for (j=0; j<vtmp->npt; j++) {
            pt = *(ptmp + j);
            if (j%2 == 0) {
                ft = (CSW_F)pt;
                *xtp = ft / 200.f * PatScale + TileX0;
                xtp++;
            }
            else {
                ft = (CSW_F)pt;
                *ytp = ft / 200.f * PatScale + TileY0;
                ytp++;
            }
        }
    }

    *nc = nvec;

    return 1;

}  /*  end of function ply_GetTilePolys  */


/*
  ****************************************************************

                p l y _ G e t T i l e D o t s

  ****************************************************************

    Get the filled circles for the current PatNum and scale and
    translate them according to PatScale, TileX0, and TileY0.
    The resulting circles are in plot scale units, as are the
    polygon vertices but the circles are not clipped to the polygon.    

    The circles are returned in x, y, and r.  The number of circles 
    is returned in nc.  The return status is always 1.  If no 
    circles are available, nc will be zero.

*/

int CSWPolyTiledata::ply_GetTileDots (CSW_F *x, CSW_F *y, CSW_F *r, int *nc)
{
    int             i, n, narc;
    FIllArcRec      *atmp, *at0;
    CSW_F           *xtp, *ytp, *rtp, ft;

    *nc = 0;

    n = PatNum;
    if (n < 1  ||  n > PLYMAXPAT) {
        *nc = 0;
        return 1;
    }

    n--;
    narc = TileRecs[n].nfarc;
    if (narc < 1) {
        *nc = 0;
        return 1;
    }

    xtp = x;
    ytp = y;
    rtp = r;
    at0 = TileFarcs + TileRecs[n].farc1;

    for (i=0; i<narc; i++) {
        atmp = at0 + i;
        ft = (CSW_F)atmp->x;
        *xtp = ft / 200.f * PatScale + TileX0;
        ft = (CSW_F)atmp->y;
        *ytp = ft / 200.f * PatScale + TileY0;
        ft = (CSW_F)atmp->r;
        *rtp = ft / 200.f * PatScale;
        xtp++;
        ytp++;
        rtp++;
    }

    *nc = narc;

    return 1;

}  /*  end of function ply_GetTileDots  */



/*
  ****************************************************************

                p l y _ C o u n t T i l e P r i m s

  ****************************************************************

      Count the approximate number of vectors, polygons, arcs, and 
  dots in a tile and multiply by the specified multiplier.  This is 
  used to allocate memory for the output arrays in pattern filling.

*/

int CSWPolyTiledata::ply_CountTilePrims (int mult, int mult2, 
                        int *nv1, int *nf1, int *nc1, int *nd1, 
                        int *iv1, int *if1)
{
    int           n1, n, imult;
    CSW_F         fmult;
    FIllVecRec    *vtmp = NULL;

    *nv1 = 0;
    *nf1 = 0;
    *nc1 = 0;
    *nd1 = 0;
    *iv1 = 0;
    *if1 = 0;

    n = PatNum - 1;
    fmult = PatScale;
    if (fmult < 0.5f) fmult = 0.5f;
    if (mult2 < 4) mult2 = 4;
   
/*  count vectors  */

    if (TileRecs[n].vec1 >= 0) {
        n1 = TileRecs[n].nvec + 1;
        *iv1 = n1 * mult;
        vtmp = TileVectors + TileRecs[n].vec1;
        *nv1 = (vtmp->npt+2) * mult * n1;
    }
    else {
        *iv1 = 0;
        *nv1 = 0;
    }

/*  count filled polygons  */

    if (TileRecs[n].fvec1 >= 0) {
        n1 = TileRecs[n].nfvec + 1;
        *if1 = n1 * mult;
        vtmp = TileVectors + TileRecs[n].fvec1;
        *nf1 = (vtmp->npt+2) * mult * n1;
    }
    else {
        *if1 = 0;
        *nf1 = 0;
    }

/*  count arcs and filled arcs  */

    *nc1 = TileRecs[n].narc * mult;
    *nd1 = TileRecs[n].nfarc * mult;

/*  add for arcs drawn as vectors in clipping  */

    n1 = TileRecs[n].narc * mult2 / 2;
    *iv1 = *iv1 + n1;

    n1 = TileRecs[n].nfarc * mult2 / 2;
    *if1 = *if1 + n1;

    imult = (int)(fmult / 2.0f);

    n1 = TileRecs[n].narc * mult2 * POINTS_PER_ARC * imult;
    *nv1 = *nv1 + n1;

    n1 = TileRecs[n].nfarc * mult2 * POINTS_PER_ARC * imult;
    *nf1 = *nf1 + n1;

    return 1;

}  /*  end of function ply_CountTilePrims  */
