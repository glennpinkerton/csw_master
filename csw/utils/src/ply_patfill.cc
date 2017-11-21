
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_patfill.cc

    This file has functions for getting vectors, arcs, and
    polygons to draw in pattern fills.  The primitives are 
    clipped to the polygon and can be drawn to a device after
    being retrieved from this file.  

    The sequence of steps is to call ply_SetupPattFill
    with the polygon data, pattern number and pattern scale.
    Upon a successful completion of the setup, the primitives
    can be retrieved by calling ply_GetPatVecs, ply_GetPatPolys,
    ply_GetPatArcs or ply_GetPatDots.
*/


/*
    system header files
*/

#include <string.h>
#include <stdio.h>


/*
    application header files
*/

#include "csw/utils/include/csw_errnum.h"
#include "csw/utils/include/csw_timer.h"
#include "csw/utils/include/csw_.h"

#include "csw/utils/private_include/ply_fpatP.h"

#include "csw/utils/private_include/gpf_utils.h"

#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_tiledata.h"
#include "csw/utils/private_include/ply_cutline.h"
#include "csw/utils/private_include/ply_patfill.h"
#include "csw/utils/private_include/ply_calc.h"

#include "csw/utils/private_include/csw_memmgt.h"
#include "csw/utils/private_include/csw_scope.h"




/*
  ****************************************************************

    p l y _ S e t P a t t e r n A l l O r N o t h i n g F l a g

  ****************************************************************

*/

int CSWPolyPatfill::ply_SetPatternAllOrNothingFlag (int val)
{
    if (val != 1) val = 0;
    AllOrNothingFlag = val;
    return 1;

}  /*  end of function ply_SetPatternAllOrNothingFlag  */



/*
  ****************************************************************

          p l y _ G e t A l l O r N o t h i n g F l a g

  ****************************************************************

*/

int CSWPolyPatfill::ply_GetAllOrNothingFlag (void)
{
    return AllOrNothingFlag;

}  /*  end of function ply_GetAllOrNothingFlag  */



/*
  ****************************************************************

                  p l y _ S e t u p P a t F i l l

  ****************************************************************

  function name:    ply_SetupPatFill            (int)

  call sequence:    ply_SetupPatFill (xp, yp, ic, nc,
                                      patnum, patscale)

  purpose:          Calculate a set of line, arc, fill and filled
                    arc primitives that can be drawn to fill the
                    specified polygon with a pattern.  This is the
                    single precision version.

  return value:     status code

                    1 = normal successful completion
                   -1 = memory allocation error
                   -2 = pattern number out of valid range
                   -3 = bad polygon component (less than 3 points)
                        or nc less than 1

  calling parameters: 

    xp          r     CSW_F*     array of polygon x coordinates
    yp          r     CSW_F*     array of polygon y coordinates
    ic          r     int*       number of vertices per component
    nc          r     int        number of components
    patnum      r     int        polygon fill pattern number
    patscale    r     CSW_F      fill pattern scale factor

*/

int CSWPolyPatfill::ply_SetupPatFill (CSW_F *xp, CSW_F *yp, int *ic, int nc,
                      int patnum, CSW_F patscale)
{
    int            i, n, npts, istat;
    double         *xwork = NULL, *ywork = NULL, dscale;

    CSWMemmgt      csw_mem_obj;

/*  count points  */

    npts = 0;
    for (i=0; i<nc; i++) {
        npts += ic[i];
    }
    n = npts;
    npts += 100;

/*  allocate work space  */

    xwork = (double *)csw_mem_obj.csw_StackMalloc (npts * 2 * sizeof(double));
    if (!xwork) {
        return -1;
    }
    ywork = xwork + npts;

/*  transfer points to work space  */

    for (i=0; i<n; i++) {
        xwork[i] = (double)xp[i];
        ywork[i] = (double)yp[i];
    }

/*  call the double version of the function  */

    dscale = patscale;
    istat = ply_SetupPatFill2 (xwork, ywork, ic, nc,
                               patnum, dscale);

    return istat;

}  /*  end of function ply_SetupPatFill  */



/*
  ****************************************************************

                p l y _ S e t u p P a t F i l l 2

  ****************************************************************

  function name:    ply_SetupPatFill2           (int)

  call sequence:    ply_SetupPatFill2 (xp, yp, ic, nc,
                                       patnum, patscale)

  purpose:          Calculate a set of line, arc, fill and filled
                    arc primitives that can be drawn to fill the
                    specified polygon with a pattern.  This is the
                    double precision version.

  return value:     status code

                    1 = normal successful completion
                   -1 = memory allocation error
                   -2 = pattern number out of valid range
                   -3 = bad polygon component (less than 3 points)
                        or nc less than 1

  calling parameters: 

    xp          r     double*    array of polygon x coordinates
    yp          r     double*    array of polygon y coordinates
    ic          r     int*       number of vertices per component
    nc          r     int        number of components
    patnum      r     int        polygon fill pattern number
    patscale    r     double     fill pattern scale factor

*/

int CSWPolyPatfill::ply_SetupPatFill2 (double *xp, double *yp, int *ic, int nc,
                       int patnum, double patscale)
{
    int            i, j, i1, i2, sum, n, n2, istat,
                   ix1, iy1, ix2, iy2, *idum = NULL;
    int            nv1, nf1, nc1, nd1, iv2, if2;
    int            ncol, nrow, nodes;
    double         *xp1 = NULL, *yp1 = NULL, *xp2 = NULL, *yp2 = NULL;
    double         gx1, gy1, gx2, gy2, fx0, fy0, tiny;
    char           *hgrid = NULL, *vgrid = NULL, *iogrid = NULL, *bcell = NULL;
    int            t1, t2, t3, t4;
    CSWPolyUtils   ply_utils_obj;
    CSWPolyTiledata  ply_tiledata_obj;
    CSWMemmgt      csw_mem_obj;

    auto fscope = [&]()
    {
        if (CellInside) {
            csw_Free (CellInside);
            CellInside = NULL;
        }
        ply_PattWorkFree ();
        csw_Free (IvecPoly);
        IvecPoly = NULL;
        csw_Free (CprimW);
        CprimW = NULL;
        csw_Free (XcprimW);
        XcprimW = NULL;
        YcprimW = NULL;
        NcprimW = 0;
        NxcprimW = 0;
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  obvious errors  */

    if (patnum < 1  ||  patnum > PLYMAXPAT) {
        return -2;
    }

    if (nc < 1) {
        return -3;
    }

    for (i=0; i<nc; i++) {
        if (ic[i] < 3) return -3;
    }

    if (patscale < 0.1f) patscale = 0.1f;
    if (patscale > 100.f) patscale = 100.f;

    PatNum = patnum;
    PatScale = (CSW_F)patscale;

/*  set pattern number and scale for tile data functions  */

    ply_tiledata_obj.ply_SetTileNum (patnum);
    ply_tiledata_obj.ply_SetTileScale ((CSW_F)patscale);

/*  allocate work space  */

    idum = (int *)csw_mem_obj.csw_StackMalloc (sizeof (int));
    if (!idum) {
        return -1;
    }

    n = nc + 1;
    xp1 = (double *)csw_mem_obj.csw_StackMalloc (n * 4 * sizeof(double));
    if (!xp1) {
        return -1;
    }
    yp1 = xp1 + n;
    xp2 = yp1 + n;
    yp2 = xp2 + n;

/*  allocate space for polyline clip vector subsets  */

    n = 0;
    for (i=0; i<nc; i++) {
        n += ic[i];
    }
    n *= 4;

    Xvc = (double *)csw_mem_obj.csw_StackMalloc (n * 2 * sizeof(double));
    if (!Xvc) {
        return -1;
    }
    Yvc = Xvc + n;

    n /= 2;
    Ivc = (int *) csw_mem_obj.csw_StackMalloc (2 * n * sizeof(int));
    if (!Ivc) {
        return -1;
    }
    Ivc2 = Ivc + n;

    MaxIvc = n;
    MaxXvc = n * 2;

/*  get polygon limits  */

    istat = ply_utils_obj.ply_limits (xp, yp, ic, nc,
                        xp1, yp1, xp2, yp2,
                        &gx1, &gy1, &gx2, &gy2);

/*  setup grid geometry based on limits and pattern size  */

    tiny = (gx2 - gx1 + gy2 - gy1) / 200.f;
    gx1 -= tiny;
    gy1 -= tiny;
    gx2 += tiny;
    gy2 += tiny;
    gx1 -= patscale * 2.0;
    gy1 -= patscale * 2.0;
    gx2 += patscale * 2.0;
    gy2 += patscale * 2.0;

    ix1 = (int)(gx1 / patscale);
    if (gx1 < 0.0f) ix1--;
    gx1 = ix1 * patscale;
    iy1 = (int)(gy1 / patscale);
    if (gy1 < 0.0f) iy1--;
    gy1 = iy1 * patscale;
    ix2 = (int)(gx2 / patscale);
    if (gx2 >= 0.0f) ix2++;
    iy2 = (int)(gy2 / patscale);
    if (gy2 >= 0.0f) iy2++;
   
    Gx1 = gx1;
    Gy1 = gy1;
    Gx2 = ix2 * patscale;
    Gy2 = iy2 * patscale;

    ncol = ix2 - ix1 + 1;
    nrow = iy2 - iy1 + 1;
    nodes = ncol * nrow;
    nodes += 10;

    hgrid = (char *)csw_mem_obj.csw_StackMalloc (4 * nodes * sizeof(char));
    if (!hgrid) {
        return -1;
    }
    vgrid = hgrid + nodes;
    iogrid = vgrid + nodes;
    bcell = iogrid + nodes;

    nodes -= 10;

/*  set grids for side crossings at primitive resolution  */

    if (!(PatNum <= 10  || (PatNum >= 13 && PatNum <= 15)  ||
        (PatNum >= 17 && PatNum <= 19))) {
        ply_setprimscangrids (xp, yp, ic, nc);
    }

/*  set grids for polygon side crossings at tile resolution  */

    ply_utils_obj.ply_scan (hgrid, vgrid, iogrid, bcell,
              ncol, nrow, ncol, nrow,
              xp, yp, ic, nc,
              Gx1, Gy1, patscale, patscale,
              xp1, yp1, xp2, yp2);

/*  count the cells not completely outside  */

    n = 0;
    n2 = 0;
    for (i=0; i<nrow-1; i++) {
        i1 = i * ncol;
        i2 = i1 + ncol;
        for (j=0; j<ncol-1; j++) {
            if (*(bcell + i1 + j) == 1) {
                n++;
                continue;
            }
            t1 = iogrid[i1+j] - 1;
            t2 = iogrid[i1+j+1] - 1;
            t3 = iogrid[i2+j] - 1;
            t4 = iogrid[i2+j+1] - 1;
            sum = t1 + t2 + t3 + t4;
            if (sum > 0) {
                n++;
                n2++;
            }
        }
    }

    n2 = n - n2;
    if (n2 < 4) n2 = 4;
    if (n < 4) n = 4;

/*  allocate memory for output primitives  */

    ply_tiledata_obj.ply_CountTilePrims (n, n2, &nv1, &nf1, &nc1, &nd1, &iv2, &if2);

    if (patscale > 1.0f) {
        nf1 = (int)(nf1 * patscale);
        nv1 = (int)(nv1 * patscale);
    }

    if (iv2 > 0) {
        iv2 = iv2 * (nc + 4);
    }
    if (if2 > 0) {
        if2 = if2 * (nc + 4);
    }
    if (nv1 > 0) {
        nv1 = nv1 * (nc + 4);
    }
    if (nf1 > 0) {
        nf1 = nf1 * (nc + 4);
    }
    if (nc1 > 0) {
        nc1 = nc1 * (nc + 4);
    }
    if (nd1 > 0) {
        nd1 = nd1 * (nc + 4);
    }

    if (IvecPoly) {
        csw_Free (IvecPoly);
        IvecPoly = NULL;
    }
    if (CprimW) {
        csw_Free (CprimW);
        CprimW = NULL;
    }
    if (XcprimW) {
        csw_Free (XcprimW);
        XcprimW = NULL;
        YcprimW = NULL;
    }
    NcprimW = 0;
    NxcprimW = 0;

    i = nv1*2 + nf1*2 + nc1*3 + nd1*3;

    MaxVptPoly = nv1;
    MaxFptPoly = nf1;
    MaxVecPoly = iv2;
    MaxArcPoly = nc1;
    MaxFvecPoly = if2;
    MaxFarcPoly = nd1;

    istat = ply_PattWorkAlloc (nv1, nf1, nc1, nd1);

    if (istat != 1) {
        return -1;
    }

    MaxCprimW = 0;
    MaxXcprimW = 0;
    if (ClipPattPrimFlag) {
        i = nv1 + nf1 + nc1*2 + nd1*2;
        i *= 2;
        MaxXcprimW = i;
        if (i < 1) i = 1;
        XcprimW = (CSW_F *)csw_Malloc (i * 2 * sizeof(CSW_F));
        if (!XcprimW) {
            return -1;
        }
        YcprimW = XcprimW + i;
        i = iv2 + if2 + 
            nc1*MAXCPRIMPARTS2 + nd1*MAXCPRIMPARTS2;
        i *= 2;
        i = i * (nc + 4) / 4;
        if (i < 1) i = 1;

        CprimW = (POlygonClipPrimRec *)csw_Malloc (i * sizeof(POlygonClipPrimRec));
        MaxCprimW = i;
        if (!CprimW) {
            return -1;
        }
    }
    
    i = iv2 + if2;
    if (i < 1) i = 1;
    IvecPoly = (int *)csw_Malloc (i * sizeof(int));
    if (!IvecPoly) {
        return -1;
    }
    
    IfvecPoly = IvecPoly + iv2;

/*  initialize output primitive private count variables  */

    NvecPoly = 0;
    NfvecPoly = 0;
    NarcPoly = 0;
    NfarcPoly = 0;

    NptPoly = 0;
    NfptPoly = 0;

    NvecOut = 0;
    NarcOut = 0;
    NfvecOut = 0;
    NfarcOut = 0;

    NptOut = 0;
    NfptOut = 0;

/*  
    For each tile that is not completely outside of the polygon,
    output it to the primitive arrays with clipping to the 
    polygon if needed.
*/

    FirstClip = 1;
    for (i=0; i<nrow-1; i++) {
        i1 = i * ncol;
        i2 = i1 + ncol;
        fy0 = gy1 + i * patscale;
        for (j=0; j<ncol-1; j++) {
            t1 = iogrid[i1+j] - 1;
            t2 = iogrid[i1+j+1] - 1;
            t3 = iogrid[i2+j] - 1;
            t4 = iogrid[i2+j+1] - 1;
            sum = t1 + t2 + t3 + t4;
            fx0 = gx1 + j * patscale;
            if (sum < 0  &&  *(bcell + i1 + j) == 0) {
                continue;
            }
            if (sum > 0  &&  *(bcell + i1 + j) == 0) {
                istat = ply_OutputTile (fx0, fy0);
                if (istat == -1) {
                    return -1;
                }
            }
            else {
                if (ClipPattPrimFlag) {
                    Fx0 = fx0;
                    Fy0 = fy0;
                    istat = ply_SetupTileClipVecs (xp, yp, ic, nc);
                    if (istat == -1) {
                        break;
                    }
                    ply_tiledata_obj.ply_SetTileOrigin ((CSW_F)fx0, (CSW_F)fy0);
                    istat = ply_ClipPrimToPoly (xp, yp, 1, &nc, ic);
                    if (istat == -1) {
                        break;
                    }
                }
                else {
                    if (FirstClip) {
                        ply_AddSavedClipPrimData ();
                        FirstClip = 0;
                    }
                }
            }
        }
    }


/*  transfer data to "permanent" clip primitive storage  */

    if (ClipPattPrimFlag) {
        ply_TransferClipPrimData ();
    }

    return 1;
                
}  /*  end of function ply_SetupPatFill2  */


/*
  ****************************************************************

                   p l y _ O u t p u t T i l e

  ****************************************************************

    Copy the translated tile primitives to the polygon primitive
  arrays without clipping to the polygon.  Returns 1 on success or
  -1 if memory would overflow.

*/

int CSWPolyPatfill::ply_OutputTile (double x0, double y0)
{
    int            i, j, n, nc;
    CSWPolyTiledata  ply_tiledata_obj;

    ply_tiledata_obj.ply_SetTileOrigin ((CSW_F)x0, (CSW_F)y0);

/*  vector primitives  */

    ply_tiledata_obj.ply_GetTileVecs (XvecTile, YvecTile, IvecTile, &nc);
    n = 0;
    for (i=0; i<nc; i++) {
        IvecPoly[NvecPoly] = IvecTile[i];
        for (j=0; j<IvecTile[i]; j++) {
            XvecPoly[NptPoly] = XvecTile[n];
            YvecPoly[NptPoly] = YvecTile[n];
            n++;
            NptPoly++;
            if (NptPoly >= MaxVptPoly) return -1;
        }
        NvecPoly++;
        if (NvecPoly >= MaxVecPoly) return -1;
    }

/*  filled vector primitives  */

    ply_tiledata_obj.ply_GetTilePolys (XvecTile, YvecTile, IvecTile, &nc);
    n = 0;
    for (i=0; i<nc; i++) {
        IfvecPoly[NfvecPoly] = IvecTile[i];
        for (j=0; j<IvecTile[i]; j++) {
            XfvecPoly[NfptPoly] = XvecTile[n];
            YfvecPoly[NfptPoly] = YvecTile[n];
            n++;
            NfptPoly++;
            if (NfptPoly >= MaxFptPoly) return -1;
        }
        NfvecPoly++;
        if (NfvecPoly >= MaxFvecPoly) return -1;
    }

/*  arc primitives  */

    ply_tiledata_obj.ply_GetTileArcs (XarcTile, YarcTile, RarcTile, &nc);
    for (i=0; i<nc; i++) {
        XarcPoly[NarcPoly] = XarcTile[i];
        YarcPoly[NarcPoly] = YarcTile[i];
        RarcPoly[NarcPoly] = RarcTile[i];
        NarcPoly++;
        if (NarcPoly >= MaxArcPoly) return -1;
    }

/*  filled arc primitives  */

    ply_tiledata_obj.ply_GetTileDots (XarcTile, YarcTile, RarcTile, &nc);
    for (i=0; i<nc; i++) {
        XfarcPoly[NfarcPoly] = XarcTile[i];
        YfarcPoly[NfarcPoly] = YarcTile[i];
        RfarcPoly[NfarcPoly] = RarcTile[i];
        NfarcPoly++;
        if (NfarcPoly >= MaxFarcPoly) return -1;
    }

    return 1;

}  /*  end of function ply_OutputTile   */




/*
  ****************************************************************

                   p l y _ N e x t P a t V e c

  ****************************************************************

    Get the next pattern fill vector points.  Call this after 
  ply_SetupPatFill succeeds to get each polyline to draw.

    A return value of 1 means the call succeeded.  A return value
  of zero means there are no more vectors to plot, and the data
  returned is undefined.  Do not draw the line if zero is returned.

*/

int CSWPolyPatfill::ply_NextPatVec (CSW_F *x, CSW_F *y, int *npts)
{
    int          i;

    if (NptOut >= NptPoly) {
        *npts = 0;
        return 0;
    }
    if (NvecOut >= NvecPoly) {
        *npts = 0;
        return 0;
    }

    for (i=0; i<IvecPoly[NvecOut]; i++) {
        x[i] = XvecPoly[NptOut];
        y[i] = YvecPoly[NptOut];
        NptOut++;
        if (NptOut >= NptPoly) {
            *npts = i + 1;
            NvecOut++;
            return 1;
        }
    }

    *npts = IvecPoly[NvecOut];
    NvecOut++;
    
    return 1;

}  /*  end of function ply_NextPatVec  */




/*
  ****************************************************************

               p l y _ N e x t P a t V e c F i l l

  ****************************************************************

    Get the next pattern fill polygon points.  Call this after 
  ply_SetupPatFill succeeds to get each polygon to draw.

    A return value of 1 means the call succeeded.  A return value
  of zero means there are no more polygons to plot, and the data
  returned is undefined.  Do not draw the polygon if zero is returned.

*/

int CSWPolyPatfill::ply_NextPatVecFill (CSW_F *x, CSW_F *y, int *npts)
{
    int          i;

    if (NfptOut >= NfptPoly) {
        *npts = 0;
        return 0;
    }
    if (NfvecOut >= NfvecPoly) {
        *npts = 0;
        return 0;
    }

    for (i=0; i<IfvecPoly[NfvecOut]; i++) {
        x[i] = XfvecPoly[NfptOut];
        y[i] = YfvecPoly[NfptOut];
        NfptOut++;
        if (NfptOut >= NfptPoly) {
            *npts = i + 1;
            NfvecOut++;
            return 1;
        }
    }

    *npts = IfvecPoly[NfvecOut];
    NfvecOut++;
    
    return 1;

}  /*  end of function ply_NextPatVecFill  */


/*
  ****************************************************************

                   p l y _ N e x t P a t A r c

  ****************************************************************

    Get the next circle to draw in the fill pattern.  If zero is
  returned, no more circles are available.  Do not draw the circle
  if zero is returned.

    x and y are the center point,  r is the radius.

*/

int CSWPolyPatfill::ply_NextPatArc (CSW_F *x, CSW_F *y, CSW_F *r)
{
    if (NarcOut >= NarcPoly) {
        return 0;
    }

    *x = XarcPoly[NarcOut];
    *y = YarcPoly[NarcOut];
    *r = RarcPoly[NarcOut];

    NarcOut++;

    return 1;

}  /*  end of function ply_NextPatArc  */


/*
  ****************************************************************

               p l y _ N e x t P a t A r c F i l l

  ****************************************************************

    Get the next filled circle to draw in the pattern.  If zero is
  returned, no more circles are available.  Do not draw the circle
  if zero is returned.

    x and y are the center point,  r is the radius.

*/

int CSWPolyPatfill::ply_NextPatArcFill (CSW_F *x, CSW_F *y, CSW_F *r)
{
    if (NfarcOut >= NfarcPoly) {
        return 0;
    }

    *x = XfarcPoly[NfarcOut];
    *y = YfarcPoly[NfarcOut];
    *r = RfarcPoly[NfarcOut];

    NfarcOut++;

    return 1;

}  /*  end of function ply_NextPatArcFill  */


/*
  ****************************************************************

               p l y _ C l i p P r i m T o P o l y

  ****************************************************************

    Clip the fill pattern primitives in the current tile to 
  the polygon.  This is called from ply_SetupPatFill2 for tiles
  that are not entirely inside the polygon.

*/

int CSWPolyPatfill::ply_ClipPrimToPoly (double *xp, double *yp, int np, int *ic, int *ih)
{
    int               istat, n, i;

/*  
    do not scan for long line patterns
*/

    if (PatNum <= 10  || (PatNum >= 13 && PatNum <= 15)  ||
        (PatNum >= 17 && PatNum <= 19)) {

        n = 0;
        for (i=0; i<np; i++) {
            istat = ply_ClipLongTileLines (xp, yp, ih+n, ic[i]);
            n += ic[i];
            if (istat == -1) {
                return -1;
            }
        }
        return 1;
    }

/*
    use a scanned grid to speed up clipping of short line
    and arc patterns
*/

    n = 0;
    for (i=0; i<np; i++) {
        istat = ply_ClipPrimToPoly1 (xp, yp, ih+n, ic[i]);
        n += ic[i];
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}  /*  end of function ply_ClipPrimToPoly  */


/*
  ****************************************************************

              p l y _ C l i p P r i m T o P o l y 1

  ****************************************************************

    Clip small tile primitives to a single component of the clipped
    tile.  This is only called from ply_ClipPrimToPoly.  If a pattern
    has long lines it is clipped with ply_ClipLongTileLines.

*/

int CSWPolyPatfill::ply_ClipPrimToPoly1 (double *xp, double *yp, int *ic, int nc)
{
    int             np, istat;
    CSWPolyTiledata  ply_tiledata_obj;

/*  
    get tile vectors and output each vector, clipping
    when needed
*/

    ply_tiledata_obj.ply_GetTileVecs (XvecTile, YvecTile, IvecTile, &np);
    if (np > 0) {
        istat = ply_ClipVecsForTile (xp, yp, ic, nc, np);
        if (istat == -1) return -1;
    }

/*
    get tile polygons and output, clipping where needed
*/

    ply_tiledata_obj.ply_GetTilePolys (XvecTile, YvecTile, IvecTile, &np);
    if (np > 0) {
        istat = ply_ClipPolysForTile (xp, yp, ic, nc, np);
        if (istat == -1) return -1;
    }

/*
    get tile arcs and output, clipping where needed
*/

    ply_tiledata_obj.ply_GetTileArcs (XarcTile, YarcTile, RarcTile, &np);
    if (np > 0) {
        istat = ply_ClipArcsForTile (xp, yp, ic, nc, np);
        if (istat == -1) return -1;
    }

/*
    get filled arcs for tile and output, clipping where needed
*/

    ply_tiledata_obj.ply_GetTileDots (XarcTile, YarcTile, RarcTile, &np);
    if (np > 0) {
        istat = ply_ClipDotsForTile (xp, yp, ic, nc, np);
        if (istat == -1) return -1;
    }

    return 1;

}  /*  end of function ply_ClipPrimToPoly1  */


/*
  ****************************************************************

      p l y _ D o e s F i l l P r i m N e e d C l i p p i n g

  ****************************************************************

    Based on the bounding box of the primitive, check the CellInside
    array and return -1 if the bounding box is outside, 1 if the 
    bounding box is completely inside, or 0 if it is intersected
    by the boundary.

*/

int CSWPolyPatfill::ply_DoesFillPrimNeedClipping (double x1, double y1, double x2, double y2)
{
    int              i1, j1, i2, j2, i, j, base, sum;
    double           tiny;
    int              t1;

    tiny = (x2 - x1 + y2 - y1) / 2000.f;

    x2 -= tiny;
    x1 += tiny;
    y2 -= tiny;
    y1 += tiny;

    j1 = (int)((x1 - PatX0) / CellSize);
    j2 = (int)((x2 - PatX0) / CellSize);
    i1 = (int)((y1 - PatY0) / CellSize);
    i2 = (int)((y2 - PatY0) / CellSize);

    if (i1 < 0  ||  i2 >= NrowMax) {
        return -1;
    }
    if (j1 < 0  ||  j2 >= NcolMax) {
        return -1;
    }

    sum = 0;
    for (i=i1; i<=i2; i++) {
        base = i * NcolMax;
        for (j=j1; j<=j2; j++) {
            t1 = CellInside[base+j] - 1;
            if (t1 == 0) {
                return 0;
            }
            sum += t1;
        }
    }

    if (sum > 0) return 1;
    if (sum < 0) return -1;

    return 0;

}  /*  end of function ply_DoesFillPrimNeedClipping  */


/*
  ****************************************************************

              p l y _ C l i p V e c s F o r T i l e

  ****************************************************************

    Output vector pattern primitives, clipping as needed.
    Returns -1 if a memory overflow occurs.

*/

int CSWPolyPatfill::ply_ClipVecsForTile (double *xp, double *yp, int *ic, int nc, int np)
{
    int            istat, i, j, k, n, n2, npout, npts;
    double         x1, y1, x2, y2;
    CSW_F          *xt = NULL, *yt = NULL, *xft = NULL, *yft = NULL;
    int            maxvecs;

    CSWPolyCalc    ply_calc_obj;

/*  
    Loop through each vector primitive in the private tile vector
    data arrays.
*/

    n = 0;
    for (i=0; i<np; i++) {
        npts = IvecTile[i];

        xt = XvecTile + n;
        yt = YvecTile + n;

        ply_PatPrimLimits (xt, yt, npts,
                           &x1, &y1, &x2, &y2);

        n += npts;

        istat = ply_DoesFillPrimNeedClipping (x1, y1, x2, y2);

/*      completely outside  */

        if (istat == -1) {
            continue;
        }
        
/*      completely inside  */

        if (istat == 1) {
            IvecPoly[NvecPoly] = npts;
            if (NxcprimW + npts >= MaxXcprimW) return -1;
            xft = XcprimW + NxcprimW;
            yft = YcprimW + NxcprimW;
            for (j=0; j<npts; j++) {
                XvecPoly[NptPoly] = xt[j];
                YvecPoly[NptPoly] = yt[j];
                xft[j] = (CSW_F)xt[j];
                yft[j] = (CSW_F)yt[j];
                NptPoly++;
                if (NptPoly >= MaxVptPoly) return -1;
            }
            NvecPoly++;
            if (NvecPoly >= MaxVecPoly) return -1;
            CprimW[NcprimW].start = NxcprimW;
            CprimW[NcprimW].npts = npts;
            CprimW[NcprimW].flag = 0;
            NxcprimW += npts;
            NcprimW++;
            if (NcprimW >= MaxCprimW) return -1;
            continue;
        }

/*      possibly intersects boundary  */

        for (j=0; j<npts; j++) {
            XvecD[j] = xt[j];
            YvecD[j] = yt[j];
        }

        maxvecs = npts * 2;
        if (maxvecs < MINWORKVECS) maxvecs = MINWORKVECS;

        ply_calc_obj.ply_ClipPlineToArea (1, xp, yp, ic, nc,
                             Xvc, Yvc, Ivc, Nvc,
                             XvecD, YvecD, npts,
                             Xvec2, Yvec2, Ivec2, &npout,
                             maxvecs, MAXWORKCOMPS);

        n2 = 0;
        if (npout > MAXCPRIMPARTS) npout = MAXCPRIMPARTS;
        for (j=0; j<npout; j++) {
            IvecPoly[NvecPoly] = Ivec2[j];
            if (NxcprimW + Ivec2[j] >= MaxXcprimW) return -1;
            xft = XcprimW + NxcprimW;
            yft = YcprimW + NxcprimW;
            for (k=0; k<Ivec2[j]; k++) {
                XvecPoly[NptPoly] = (CSW_F)Xvec2[n2];
                YvecPoly[NptPoly] = (CSW_F)Yvec2[n2];
                xft[k] = (CSW_F)Xvec2[n2];
                yft[k] = (CSW_F)Yvec2[n2];
                n2++;
                NptPoly++;
                if (NptPoly >= MaxVptPoly) return -1;
            }
            NvecPoly++;
            if (NvecPoly >= MaxVecPoly) return -1;
            CprimW[NcprimW].start = NxcprimW;
            CprimW[NcprimW].npts = Ivec2[j];
            CprimW[NcprimW].flag = 0;
            NxcprimW += Ivec2[j];
            NcprimW++;
            if (NcprimW >= MaxCprimW) return -1;
        }

    }  /*  end of loop through vector primitives  */

    return 1;

}  /*  end of function ply_ClipVecsForTile  */


/*
  ****************************************************************

               p l y _ P a t P r i m L i m i t s

  ****************************************************************

    Find x,y limits of an array of coordinates.

*/

int CSWPolyPatfill::ply_PatPrimLimits (CSW_F *x, CSW_F *y, int npts, 
                       double *x1, double *y1, double *x2, double *y2)
{
    int        i;

    *x1 = 1.e30f;
    *y1 = 1.e30f;
    *x2 = -1.e30f;
    *y2 = -1.e30f;

    for (i=0; i<npts; i++) {
        if (x[i] < *x1) *x1 = x[i];
        if (x[i] > *x2) *x2 = x[i];
        if (y[i] < *y1) *y1 = y[i];
        if (y[i] > *y2) *y2 = y[i];
    }

    return 1;

}  /*  end of function ply_PatPrimLimits  */


/*
  ****************************************************************

              p l y _ C l i p P o l y s F o r T i l e

  ****************************************************************

    Output polygon pattern primitives, clipping as needed.

*/

int CSWPolyPatfill::ply_ClipPolysForTile
    (double *xp, double *yp, int *ic, int nc, int np)
{
    int            istat, i, j, k, n, n2, n3, npout, npts, npts2;
    double         x1, y1, x2, y2;
    CSW_F          *xt = NULL, *yt = NULL, *xft = NULL, *yft = NULL;
    int            maxcomps, maxvecs;

    CSWErrNum      err_obj;
    CSWPolyCalc    ply_calc_obj;

/*  
    Loop through each polygon primitives in the private tile 
    polygon data arrays.
*/

    n = 0;
    for (i=0; i<np; i++) {
        npts = IvecTile[i];

        xt = XvecTile + n;
        yt = YvecTile + n;

        ply_PatPrimLimits (xt, yt, npts,
                           &x1, &y1, &x2, &y2);

        n += npts;

        istat = ply_DoesFillPrimNeedClipping (x1, y1, x2, y2);

/*      completely outside  */

        if (istat == -1) {
            continue;
        }
        
/*      completely inside  */

        if (istat == 1) {
            IfvecPoly[NfvecPoly] = npts;
            if (NxcprimW + npts >= MaxXcprimW) return -1;
            xft = XcprimW + NxcprimW;
            yft = YcprimW + NxcprimW;
            for (j=0; j<npts; j++) {
                XfvecPoly[NfptPoly] = xt[j];
                YfvecPoly[NfptPoly] = yt[j];
                xft[j] = (CSW_F)xt[j];
                yft[j] = (CSW_F)yt[j];
                NfptPoly++;
                if (NfptPoly >= MaxFptPoly) return -1;
            }
            NfvecPoly++;
            if (NfvecPoly >= MaxFvecPoly) return -1;
            CprimW[NcprimW].start = NxcprimW;
            CprimW[NcprimW].npts = npts;
            CprimW[NcprimW].flag = 1;
            NxcprimW += npts;
            NcprimW++;
            if (NcprimW >= MaxCprimW) return -1;
            continue;
        }

/*      possibly intersects boundary  */

        if (AllOrNothingFlag) {
            continue;
        }

        for (j=0; j<npts; j++) {
            XvecD[j] = xt[j];
            YvecD[j] = yt[j];
        }

        maxcomps = MAXPRIMCOMPS + nc * 2;
        maxvecs = npts * 2;
        if (maxvecs > MAXWORKVECS) {
            maxvecs = MAXWORKVECS;
        }

        for (j=0; j<nc; j++) {
            maxvecs += ic[j];
            maxvecs += ic[j];
        }

        istat = ply_calc_obj.ply_CalcIntersect (err_obj,
                                   xp, yp, ic, nc,
                                   XvecD, YvecD, &npts, 1,
                                   Xvec2, Yvec2, &npout, Ivec2, Ivec3,
                                   maxcomps, maxcomps, maxvecs);
        if (istat == -1) {
            continue;
        }

        n2 = 0;
        n3 = 0;
        if (npout > MAXCPRIMPARTS) npout = MAXCPRIMPARTS;
        for (j=0; j<npout; j++) {
            npts2 = 0;
            for (k=0; k<Ivec2[j]; k++) {
                npts2 += Ivec3[n3];
                n3++;
            }
            if (NxcprimW + npts2 >= MaxXcprimW) return -1;
            IfvecPoly[NfvecPoly] = npts2;
            xft = XcprimW + NxcprimW;
            yft = YcprimW + NxcprimW;
            for (k=0; k<npts2; k++) {
                XfvecPoly[NfptPoly] = (CSW_F)Xvec2[n2];
                YfvecPoly[NfptPoly] = (CSW_F)Yvec2[n2];
                xft[k] = (CSW_F)Xvec2[n2];
                yft[k] = (CSW_F)Yvec2[n2];
                n2++;
                NfptPoly++;
                if (NfptPoly >= MaxFptPoly) return -1;
            }
            NfvecPoly++;
            if (NfvecPoly >= MaxFvecPoly) return -1;
            CprimW[NcprimW].start = NxcprimW;
            CprimW[NcprimW].npts = npts2;
            CprimW[NcprimW].flag = 1;
            NcprimW++;
            if (NcprimW >= MaxCprimW) return -1;
            NxcprimW += npts2;
        }

    }  /*  end of loop through polygon primitives  */

    return 1;

}  /*  end of function ply_ClipPolysForTile  */


/*
  ****************************************************************

              p l y _ C l i p A r c s F o r T i l e

  ****************************************************************

    Output arc pattern primitives, clipping as needed.

*/

int CSWPolyPatfill::ply_ClipArcsForTile (double *xp, double *yp, int *ic, int nc, int np)
{
    int            istat, i, j, k, n2, npout, npts;
    double         x1, y1, x2, y2;
    CSW_F          xt, yt, rt;
    CSW_F          fmult, *xft = NULL, *yft = NULL;
    CSWPolyCalc    ply_calc_obj;

/*  
    Loop through each arc primitives in the private tile 
    polygon data arrays.
*/

    for (i=0; i<np; i++) {

        xt = XarcTile[i];
        yt = YarcTile[i];
        rt = RarcTile[i];
        x1 = xt - rt;
        y1 = yt - rt;
        x2 = xt + rt;
        y2 = yt + rt;

        istat = ply_DoesFillPrimNeedClipping (x1, y1, x2, y2);

/*      completely outside  */

        if (istat == -1) {
            continue;
        }
        
/*      completely inside  */

        if (istat == 1) {
            XarcPoly[NarcPoly] = xt;
            YarcPoly[NarcPoly] = yt;
            RarcPoly[NarcPoly] = rt;
            NarcPoly++;
            if (NarcPoly >= MaxArcPoly) return -1;
            if (NxcprimW + 2 >= MaxXcprimW) return -1;
            xft = XcprimW + NxcprimW;
            yft = YcprimW + NxcprimW;
            xft[0] = xt;
            yft[0] = yt;
            xft[1] = rt;
            yft[1] = rt;
            CprimW[NcprimW].start = NxcprimW;
            CprimW[NcprimW].npts = 2;
            CprimW[NcprimW].flag = 2;
            NxcprimW += 2;
            NcprimW++;
            if (NcprimW >= MaxCprimW) return -1;
            continue;
        }

/*      possibly intersects boundary  */

/*
        if (AllOrNothingFlag) {
            continue;
        }
*/

        fmult = PatScale;
        if (fmult < 0.5f) fmult = 0.5f;
        if (fmult > (CSW_F)MAXWORKVECS / (CSW_F)POINTS_PER_ARC) {
            fmult = (CSW_F)(MAXWORKVECS / (CSW_F)POINTS_PER_ARC);
        }
        
        npts = (int)(POINTS_PER_ARC * fmult);
        gpf_CalcCirclePoints (xt, yt, rt,
                              XvecTile, YvecTile, npts);

        for (j=0; j<npts; j++) {
            XvecD[j] = XvecTile[j];
            YvecD[j] = YvecTile[j];
        }

        ply_calc_obj.ply_ClipPlineToArea (1, xp, yp, ic, nc,
                             Xvc, Yvc, Ivc, Nvc,
                             XvecD, YvecD, npts,
                             Xvec2, Yvec2, Ivec2, &npout,
                             MAXWORKVECS, MAXWORKCOMPS);

        n2 = 0;
        if (npout > MAXCPRIMPARTS) npout = MAXCPRIMPARTS;
        for (j=0; j<npout; j++) {
            IvecPoly[NvecPoly] = Ivec2[j];
            if (NxcprimW + Ivec2[j] >= MaxXcprimW) return -1;
            xft = XcprimW + NxcprimW;
            yft = YcprimW + NxcprimW;
            for (k=0; k<Ivec2[j]; k++) {
                XvecPoly[NptPoly] = (CSW_F)Xvec2[n2];
                YvecPoly[NptPoly] = (CSW_F)Yvec2[n2];
                xft[k] = (CSW_F)Xvec2[n2];
                yft[k] = (CSW_F)Yvec2[n2];
                n2++;
                NptPoly++;
                if (NptPoly >= MaxVptPoly) return -1;
            }
            NvecPoly++;
            if (NvecPoly >= MaxVecPoly) return -1;
            CprimW[NcprimW].start = NxcprimW;
            CprimW[NcprimW].npts = Ivec2[j];
            CprimW[NcprimW].flag = 0;
            NxcprimW += Ivec2[j];
            NcprimW++;
            if (NcprimW >= MaxCprimW) return -1;
        }

    }  /*  end of loop through arc primitives  */

    return 1;

}  /*  end of function ply_ClipArcsForTile  */


/*
  ****************************************************************

              p l y _ C l i p D o t s F o r T i l e

  ****************************************************************

    Output filled arc pattern primitives, clipping as needed.

*/

int CSWPolyPatfill::ply_ClipDotsForTile (double *xp, double *yp, int *ic, int nc, int np)
{
    int            istat, i, j, k, n2, n3, npout, npts, npts2;
    double         x1, y1, x2, y2;
    CSW_F          xt, yt, rt;
    CSW_F          fmult, *xft = NULL, *yft = NULL;
    int            maxcomps, maxvecs;

    CSWErrNum      err_obj;
    CSWPolyCalc    ply_calc_obj;

/*  
    Loop through the dot primitives in the private tile 
    polygon data arrays.
*/

    for (i=0; i<np; i++) {

        xt = XarcTile[i];
        yt = YarcTile[i];
        rt = RarcTile[i];
        x1 = xt - rt;
        y1 = yt - rt;
        x2 = xt + rt;
        y2 = yt + rt;

        istat = ply_DoesFillPrimNeedClipping (x1, y1, x2, y2);

/*      completely outside  */

        if (istat == -1) {
            continue;
        }
        
/*      completely inside  */

        if (istat == 1) {
            XfarcPoly[NfarcPoly] = xt;
            YfarcPoly[NfarcPoly] = yt;
            RfarcPoly[NfarcPoly] = rt;
            NfarcPoly++;
            if (NfarcPoly >= MaxFarcPoly) return -1;
            if (NxcprimW + 2 >= MaxXcprimW) return -1;
            xft = XcprimW + NxcprimW;
            yft = YcprimW + NxcprimW;
            xft[0] = xt;
            yft[0] = yt;
            xft[1] = rt;
            yft[1] = rt;
            CprimW[NcprimW].start = NxcprimW;
            CprimW[NcprimW].npts = 2;
            CprimW[NcprimW].flag = 3;
            NxcprimW += 2;
            NcprimW++;
            if (NcprimW >= MaxCprimW) return -1;
            continue;
        }

/*      possibly intersects boundary  */

        if (AllOrNothingFlag) {
            continue;
        }

        fmult = PatScale;
        if (fmult < 0.5f) fmult = 0.5f;
        if (fmult > (CSW_F)MAXTILEVECS / (CSW_F)POINTS_PER_ARC) {
            fmult = (CSW_F)MAXTILEVECS / (CSW_F)POINTS_PER_ARC;
        }
        
        npts = (int)(POINTS_PER_ARC * fmult);
        gpf_CalcCirclePoints (xt, yt, rt,
                              XvecTile, YvecTile, npts);

        for (j=0; j<npts; j++) {
            XvecD[j] = XvecTile[j];
            YvecD[j] = YvecTile[j];
        }

        maxcomps = MAXPRIMCOMPS;
        maxvecs = npts * 2;
        if (maxvecs > MAXWORKVECS) {
            maxvecs = MAXWORKVECS;
        }

        for (j=0; j<nc; j++) {
            maxvecs += ic[j];
            maxvecs += ic[j];
        }

        istat = ply_calc_obj.ply_CalcIntersect (err_obj,
                                   xp, yp, ic, nc,
                                   XvecD, YvecD, &npts, 1,
                                   Xvec2, Yvec2, &npout, Ivec2, Ivec3,
                                   maxcomps, maxcomps, maxvecs);
        if (istat == -1) {
            continue;
        }

        n2 = 0;
        n3 = 0;
        if (npout > MAXCPRIMPARTS) npout = MAXCPRIMPARTS;
        for (j=0; j<npout; j++) {
            npts2 = 0;
            for (k=0; k<Ivec2[j]; k++) {
                npts2 += Ivec3[n3];
                n3++;
            }
            if (NxcprimW + npts2 >= MaxXcprimW) return -1;
            IfvecPoly[NfvecPoly] = npts2;
            xft = XcprimW + NxcprimW;
            yft = YcprimW + NxcprimW;
            for (k=0; k<npts2; k++) {
                XfvecPoly[NfptPoly] = (CSW_F)Xvec2[n2];
                YfvecPoly[NfptPoly] = (CSW_F)Yvec2[n2];
                xft[k] = (CSW_F)Xvec2[n2];
                yft[k] = (CSW_F)Yvec2[n2];
                n2++;
                NfptPoly++;
                if (NfptPoly >= MaxFptPoly) return -1;
            }
            NfvecPoly++;
            if (NfvecPoly >= MaxFvecPoly) return -1;
            CprimW[NcprimW].start = NxcprimW;
            CprimW[NcprimW].npts = npts2;
            CprimW[NcprimW].flag = 1;
            NxcprimW += npts2;
            NcprimW++;
            if (NcprimW >= MaxCprimW) return -1;
        }

    }  /*  end of loop through dot primitives  */

    return 1;

}  /*  end of function ply_ClipDotsForTile  */


/*
  ****************************************************************

           p l y _ C l i p L o n g T i l e L i n e s

  ****************************************************************

    Retrieve the "long" polylines for the pattern and clip them 
    to the polygon fragment.

*/

int CSWPolyPatfill::ply_ClipLongTileLines (double *xp, double *yp, int *ic, int nc)
{
    int              i, j, k, kk, n, np, nout, n2, *iout;
    double           *xw = NULL, *yw = NULL, *xout = NULL, *yout = NULL;
    int              maxvecs;
    CSW_F            *xft = NULL, *yft = NULL;
    CSWPolyTiledata  ply_tiledata_obj;
    CSWPolyCalc      ply_calc_obj;

/*  get the pattern vectors  */

    ply_tiledata_obj.ply_GetTileVecs (XvecTile, YvecTile, IvecTile, &np);
    if (np < 1) {
        return 1;
    }

/*  allocate work space  */

    xw = XvecD;
    yw = YvecD;
    xout = Xvec2;
    yout = Yvec2;
    iout = Ivec2;

/*  clip each polyline and output as needed  */

    k = 0;
    for (i=0; i<np; i++) {
        n = IvecTile[i];
        for (j=0; j<n; j++) {
            xw[j] = XvecTile[k];
            yw[j] = YvecTile[k];
            k++;
        }
      
        maxvecs = n * 2;
        if (maxvecs < MINWORKVECS) maxvecs = MINWORKVECS;

        ply_calc_obj.ply_ClipPlineToArea (1, xp, yp, ic, nc,
                             Xvc, Yvc, Ivc, Nvc,
                             xw, yw, n,
                             xout, yout, iout, &nout,
                             maxvecs, MAXWORKCOMPS);
        
        n2 = 0;
        for (j=0; j<nout; j++) {
            IvecPoly[NvecPoly] = iout[j];
            if (NxcprimW + iout[j] >= MaxXcprimW) return -1;
            xft = XcprimW + NxcprimW;
            yft = YcprimW + NxcprimW;
            for (kk=0; kk<iout[j]; kk++) {
                XvecPoly[NptPoly] = (CSW_F)xout[n2];
                YvecPoly[NptPoly] = (CSW_F)yout[n2];
                xft[kk] = (CSW_F)xout[n2];
                yft[kk] = (CSW_F)yout[n2];
                n2++;
                NptPoly++;
                if (NptPoly >= MaxVptPoly) return -1;
            }
            NvecPoly++;
            if (NvecPoly >= MaxVecPoly) return -1;
            CprimW[NcprimW].start = NxcprimW;
            CprimW[NcprimW].npts = iout[j];
            CprimW[NcprimW].flag = 0;
            NxcprimW += iout[j];
            NcprimW++;
            if (NcprimW >= MaxCprimW) return -1;
        }
    }

    return 1;

}  /*  end of function ply_ClipLongTileLines  */



/*
  ****************************************************************

           p l y _ s e t p r i m s c a n g r i d s

  ****************************************************************

    Set up the CellInside grid at the resolution of the tile primitive
  for the entire polygon that is to be filled.  Returns 1 on success 
  or -1 on memory allocation error.

*/

int CSWPolyPatfill::ply_setprimscangrids (double *xp, double *yp, int *ic, int nc)
{
    int             ix1, iy1, ix2, iy2, i, j, i1, i2,
                    n, ncol, nrow, nodes, *idum = NULL;
    double          csize, *xp1 = NULL, *yp1 = NULL,
                    *xp2 = NULL, *yp2 = NULL,
                    d1, d2, d3, d4;
    int             t1, t2, t3, t4;
    CSWPolyUtils    ply_utils_obj;
    CSWMemmgt       csw_mem_obj;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
        HgridP = NULL;
        VgridP = NULL;
        IogridP = NULL;
        BcellP = NULL;
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  allocate marker pointer for function  */

    idum = (int *)csw_mem_obj.csw_StackMalloc (sizeof(int));
    if (!idum) {
        return -1;
    }

/*  get limits of input polygon  */

    n = nc+1;
    xp1 = (double *)csw_mem_obj.csw_StackMalloc (n * 4 * sizeof(double));
    if (!xp1) {
        return -1;
    }
    yp1 = xp1 + n;
    xp2 = yp1 + n;
    yp2 = xp2 + n;

    ply_utils_obj.ply_limits (xp, yp, ic, nc,
                xp1, yp1, xp2, yp2,
                &d1, &d2, &d3, &d4);

/*  calculate grid geometry  */

    csize = PatScale / 10.f;
    ix1 = (int)(Gx1 / csize + .01f);
    iy1 = (int)(Gy1 / csize + .01f);
    ix2 = (int)(Gx2 / csize + .01f);
    iy2 = (int)(Gy2 / csize + .01f);
    if (Gx1 < 0.0f) ix1--;
    if (Gx2 < 0.0f) ix2--;
    if (Gy1 < 0.0f) iy1--;
    if (Gy2 < 0.0f) iy2--;

    PatX0 = Gx1;
    PatY0 = Gy1;

    ncol = ix2 - ix1 + 1;
    nrow = iy2 - iy1 + 1;
    nodes = ncol * nrow + 10;

    while (nodes >= MAXPRIMNODES) {
        csize *= 2.0f;
        ncol = ncol / 2 + 1;
        nrow = nrow / 2 + 1;
        nodes = ncol * nrow + 10;
    }

/*  allocate work memory  */

    HgridP = (char *)csw_mem_obj.csw_StackMalloc (4 * nodes * sizeof(char));
    if (!HgridP) {
        return -1;
    }
    VgridP = HgridP + nodes;
    IogridP = VgridP + nodes;
    BcellP = IogridP + nodes;

/*  allocate cell inside grid space  */

    CellSize = csize;
    if (CellInside) {
        csw_Free (CellInside);
        CellInside = NULL;
    }

    CellInside = (char *)csw_Malloc (nodes * sizeof(char));
    if (!CellInside) {
        return -1;
    }

/*  set iogrid and boundary grid arrays  */

    ply_utils_obj.ply_scan (HgridP, VgridP, IogridP, BcellP,
              ncol, nrow, ncol, nrow,
              xp, yp, ic, nc,
              PatX0, PatY0, CellSize, CellSize,
              xp1, yp1, xp2, yp2);

/*  calculate CellInside grid  */

    NcolMax = ncol;
    NrowMax = nrow;
    for (i=0; i<nrow-1; i++) {
        i1 = i * ncol;
        i2 = i1 + ncol;
        for (j=0; j<ncol-1; j++) {
            *(CellInside + i1 + j) = 1;
            if (*(BcellP + i1 + j) == 1) {
                continue;
            }
            t1 = *(IogridP + i1 + j) - 1;
            t2 = *(IogridP + i1 + j + 1) - 1;
            t3 = *(IogridP + i2 + j) - 1;
            t4 = *(IogridP + i2 + j + 1) - 1;
            n = t1 + t2 + t3 + t4;
            if (n > 0) {
                *(CellInside + i1 + j) = 2;
            }
            else if (n < 0) {
                *(CellInside + i1 + j) = 0;
            }
        }
    }

    return 1;

}  /*  end of function ply_setprimscangrids  */


/*
  ****************************************************************

              p l y _ S e t u p T i l e C l i p V e c s

  ****************************************************************

    Clip each component of the polygon to the tile as if each
    component is a polyline.  The vectors that lie inside the
    tile will be used for all vector clipping when the tile
    primitives are clipped to the polygon.

    If the clipped vector set will overflow memory, -1 is returned.
    On success, 1 is returned.

*/

int CSWPolyPatfill::ply_SetupTileClipVecs (double *xp, double *yp, int *ic, int nc)
{
    double          xtile[5], ytile[5], tiny;
    int             istat, nt1, i, ntile;
    CSWPolyCalc     ply_calc_obj;

/*
    calculate tile polygon vertices
*/
    tiny = PatScale / 1000.f;
    xtile[0] = Fx0 - tiny;
    ytile[0] = Fy0 - tiny;
    xtile[1] = xtile[0];
    ytile[1] = Fy0 + PatScale + tiny;
    xtile[2] = Fx0 + PatScale + tiny;
    ytile[2] = ytile[1];
    xtile[3] = xtile[2];
    ytile[3] = ytile[0];
    xtile[4] = xtile[0];
    ytile[4] = ytile[0];
    ntile = 5;

/*
    Find the intersection between the tile and the polygon.
    This is used to clip the lines in the tile.
*/
    nt1 = 0;
    CSWErrNum   err_obj;
    istat = ply_calc_obj.ply_CalcIntersect (err_obj,
                               xtile, ytile, &ntile, 1,
                               xp, yp, ic, nc,
                               Xvc, Yvc, &nt1, Ivc2, Ivc,
                               MaxIvc, MaxIvc, MaxXvc);

    Nvc = 0;
    for (i=0; i<nt1; i++) {
        Nvc += Ivc2[i]; 
    }

    return istat;

}  /*  end of function ply_SetupTileClipVecs  */



/*
  ****************************************************************

           p l y _ S e t P a t t C l i p S a v e F l a g

  ****************************************************************

*/

int CSWPolyPatfill::ply_SetPattClipSaveFlag (int val)
{
    ClipPattPrimFlag = val;
    return 1;

}  /*  end of function ply_SetPattClipSaveFlag  */



/*
  ****************************************************************

           p l y _ G e t P a t t C l i p S a v e F l a g

  ****************************************************************

*/

int CSWPolyPatfill::ply_GetPattClipSaveFlag (void)
{
    return ClipPattPrimFlag;

}  /*  end of function ply_GetPattClipSaveFlag  */





/*
  ****************************************************************

          p l y _ T r a n s f e r C l i p P r i m D a t a

  ****************************************************************

    Transfer data from the pattern clip work area to permanent
  allocated memory which will be attached to the polygon structure.
  Returns 1 on success or -1 on allocation failure.  If there are
  no clipped pattern primitives to save, success is returned, but
  a subsequent call to ply_GetPattClipSavePtrs will return NULLs.

    The permanent storage pointers must be retrieved with the Get
  function before the next call to this function or the storage
  will be lost for the rest of time.

*/

int CSWPolyPatfill::ply_TransferClipPrimData (void)
{
    int             i;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
        csw_Free (CprimW);
        CprimW = NULL;
        csw_Free (XcprimW);
        XcprimW = NULL;
        csw_Free (CprimPtr);
        CprimPtr = NULL;
        NcprimPtr = 0;
        NxcprimW = 0;
        YcprimW = NULL;
        NcprimW = 0;
    };
    CSWScopeGuard  func_scope_guard (fscope);

    if (CprimPtr) csw_Free (CprimPtr);
    if (XcprimPtr) csw_Free (XcprimPtr);

/*  allocate memory for permanent storage  */

    CprimPtr = NULL;
    XcprimPtr = NULL;
    YcprimPtr = NULL;

    NcprimPtr = NcprimW;
    NcprimW = 0;
    
    if (NcprimPtr > 0) {
        CprimPtr = (POlygonClipPrimRec *)csw_Malloc (NcprimPtr * sizeof(POlygonClipPrimRec));
        if (!CprimPtr) {
            return -1;
        }
        XcprimPtr = (CSW_F *)csw_Malloc (NxcprimW * 2 * sizeof(CSW_F));
        if (!XcprimPtr) {
            return -1;
        }
        YcprimPtr = XcprimPtr + NxcprimW;

/*      copy coordinate and structure data to permanent storage  */

        for (i=0; i<NxcprimW; i++) {
            XcprimPtr[i] = XcprimW[i];
            YcprimPtr[i] = YcprimW[i];
        }

        for (i=0; i<NcprimPtr; i++) {
            csw_memcpy (&(CprimPtr[i]), &(CprimW[i]), sizeof(POlygonClipPrimRec));
        }
    }

/*  no clipped pattern primitives  */
    
    else {
        NcprimPtr = 0;
    }

    return 1;

}  /*  end of function ply_TransferClipPrimData  */


/*
  ****************************************************************

         p l y _ A d d S a v e d C l i p P r i m D a t a

  ****************************************************************

    Add the previously saved clipped vectors and polygons to the
  polygon pattern data.  Returns 1 on success or -1 if the data
  storage would overflow from the addition.

*/

int CSWPolyPatfill::ply_AddSavedClipPrimData (void)
{
    int            i, j, npts;
    CSW_F          *xt = NULL, *yt = NULL;

/*  return with no action if NcprimPtr is less than 1  */

    if (NcprimPtr < 1  ||  CprimPtr == NULL  ||  XcprimPtr == NULL  ||
        YcprimPtr == NULL) {
        return 1;
    }

/*  loop through CprimPtr array and set appropriate line or polygon data  */

    for (i=0; i<NcprimPtr; i++) {
        xt = XcprimPtr + CprimPtr[i].start;
        yt = YcprimPtr + CprimPtr[i].start;
        npts = CprimPtr[i].npts;

/*      line primitive  */
    
        if (CprimPtr[i].flag == 0  &&  NvecPoly < MaxVecPoly  &&
            NptPoly + npts < MaxVptPoly) {

            IvecPoly[NvecPoly] = npts;
            for (j=0; j<npts; j++) {
                XvecPoly[NptPoly] = xt[j];
                YvecPoly[NptPoly] = yt[j];
                NptPoly++;
            }
            NvecPoly++;
        }

/*      polygon primitive  */

        else if (CprimPtr[i].flag == 1  &&  NfvecPoly < MaxFvecPoly  &&
            NfptPoly + npts < MaxFptPoly) {
            IfvecPoly[NfvecPoly] = npts;
            for (j=0; j<npts; j++) {
                XfvecPoly[NfptPoly] = xt[j];
                YfvecPoly[NfptPoly] = yt[j];
                NfptPoly++;
            }
            NfvecPoly++;
        }

/*      arc primitive  */

        else if (CprimPtr[i].flag == 2  &&  NarcPoly < MaxArcPoly) {
            XarcPoly[NarcPoly] = xt[0];
            YarcPoly[NarcPoly] = yt[0];
            RarcPoly[NarcPoly] = xt[1];
            NarcPoly++;
        }

/*      filled arc primitive  */

        else if (CprimPtr[i].flag == 3  &&  NfarcPoly < MaxFarcPoly) {
            XfarcPoly[NfarcPoly] = xt[0];
            YfarcPoly[NfarcPoly] = yt[0];
            RfarcPoly[NfarcPoly] = xt[1];
            NfarcPoly++;
        }
    }

    return 1;

}  /*  end of function ply_AddSavedClipPrimData  */



/*
  ****************************************************************

            p l y _ P a t t W o r k A l l o c

  ****************************************************************

    Allocate space for polygon setup arrays.

*/

int CSWPolyPatfill::ply_PattWorkAlloc (int nv1, int nf1, int nc1, int nd1)
{

    bool     bsuccess = false;
 
    auto fscope = [&]()
    {
        if (bsuccess == false) {
            ply_PattWorkFree ();
        }
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  
    first csw_Free any non null pointers to setup data
*/

    ply_PattWorkFree ();

/*  vector primitive space  */

    if (nv1 > 0) {
        XvecPoly = (CSW_F *)csw_Malloc (nv1 * sizeof(CSW_F));
        if (!XvecPoly) {
            return -1;
        }
        YvecPoly = (CSW_F *)csw_Malloc (nv1 * sizeof(CSW_F));
        if (!YvecPoly) {
            return -1;
        }
    }

/*  fill primitive space  */

    if (nf1 > 0) {
        XfvecPoly = (CSW_F *)csw_Malloc (nf1 * sizeof(CSW_F));
        if (!XfvecPoly) {
            return -1;
        }
        YfvecPoly = (CSW_F *)csw_Malloc (nf1 * sizeof(CSW_F));
        if (!YfvecPoly) {
            return -1;
        }
    }

/*  arc primitive space  */

    if (nc1 > 0) {
        XarcPoly = (CSW_F *)csw_Malloc (nc1 * sizeof(CSW_F));
        if (!XarcPoly) {
            return -1;
        }
        YarcPoly = (CSW_F *)csw_Malloc (nc1 * sizeof(CSW_F));
        if (!YarcPoly) {
            return -1;
        }
        RarcPoly = (CSW_F *)csw_Malloc (nc1 * sizeof(CSW_F));
        if (!RarcPoly) {
            return -1;
        }
        if (nv1 < nc1 * POINTS_PER_ARC * 2) {
            XvecPoly = (CSW_F *)csw_Realloc (XvecPoly,
                                             nc1 * POINTS_PER_ARC * 2 * sizeof(CSW_F));
            if (!XvecPoly) {
                return -1;
            }
            YvecPoly = (CSW_F *)csw_Realloc (YvecPoly,
                                             nc1 * POINTS_PER_ARC * 2 * sizeof(CSW_F));
            if (!YvecPoly) {
                return -1;
            }
            MaxVptPoly = nc1 * POINTS_PER_ARC * 2;
        }
    }
            
/*  filled arc primitive space  */

    if (nd1 > 0) {
        XfarcPoly = (CSW_F *)csw_Malloc (nd1 * sizeof(CSW_F));
        if (!XfarcPoly) {
            return -1;
        }
        YfarcPoly = (CSW_F *)csw_Malloc (nd1 * sizeof(CSW_F));
        if (!YfarcPoly) {
            return -1;
        }
        RfarcPoly = (CSW_F *)csw_Malloc (nd1 * sizeof(CSW_F));
        if (!RfarcPoly) {
            return -1;
        }
        if (nf1 < nd1 * POINTS_PER_ARC * 2) {
            XfvecPoly = (CSW_F *)csw_Realloc (XfvecPoly,
                                             nd1 * POINTS_PER_ARC * 2 * sizeof(CSW_F));
            if (!XfvecPoly) {
                return -1;
            }
            YfvecPoly = (CSW_F *)csw_Realloc (YfvecPoly,
                                             nd1 * POINTS_PER_ARC * 2 * sizeof(CSW_F));
            if (!YfvecPoly) {
                return -1;
            }
            MaxFptPoly = nd1 * POINTS_PER_ARC * 2;
        }
    }

    bsuccess = true;
            
    return 1;

}  /*  end of function ply_PattWorkAlloc  */



/*
  ****************************************************************

               p l y _ P a t t W o r k F r e e

  ****************************************************************

    Free all non null pattern setup pointers.

*/

int CSWPolyPatfill::ply_PattWorkFree (void)
{

    if (XvecPoly) {
        csw_Free (XvecPoly);
        XvecPoly = NULL;
    }

    if (YvecPoly) {
        csw_Free (YvecPoly);
        YvecPoly = NULL;
    }

    if (XfvecPoly) {
        csw_Free (XfvecPoly);
        XfvecPoly = NULL;
    }

    if (YfvecPoly) {
        csw_Free (YfvecPoly);
        YfvecPoly = NULL;
    }

    if (XarcPoly) {
        csw_Free (XarcPoly);
        XarcPoly = NULL;
    }

    if (YarcPoly) {
        csw_Free (YarcPoly);
        YarcPoly = NULL;
    }

    if (RarcPoly) {
        csw_Free (RarcPoly);
        RarcPoly = NULL;
    }

    if (XfarcPoly) {
        csw_Free (XfarcPoly);
        XfarcPoly = NULL;
    }

    if (YfarcPoly) {
        csw_Free (YfarcPoly);
        YfarcPoly = NULL;
    }

    if (RfarcPoly) {
        csw_Free (RfarcPoly);
        RfarcPoly = NULL;
    }

    return 1;

}  /*  end of function ply_PattWorkFree  */
