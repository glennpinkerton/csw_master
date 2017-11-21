
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * System includes.
 */
#include <assert.h>
#include <math.h>

/*
 * CSW includes.
 */
#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"
#include "csw/utils/private_include/ply_protoP.h"


#include "csw/surfaceworks/include/grid_api.h"
#include "csw/surfaceworks/src/SurfaceGroupPlane.h"


/*
 * Local includes.
 */
#include "PadSurfaceForSim.h"


/*-------------------------------------------------------------------------------*/

/*
 * Set an object to be used to convert the xyz coordinates
 * to coordinates relative to a plane.  This is used to make
 * sure that grids for faults in the same group will all be
 * in the same coordinate system and therefore can be used
 * for various grid to grid operations needed to apply shape rules.
 */
void PadSurface::PadSetSurfaceGroupPlane (SurfaceGroupPlane *sptr)
{
    Sgp = sptr;
}


/*-------------------------------------------------------------------------------*/

/*
 * Set the detachment line to use in padding a fault.
 */
void PadSurface::PadSetDetachmentContact (
    double     *xdetach,
    double     *ydetach,
    double     *zdetach,
    int        ndetach)
{
    XDetach = xdetach;
    YDetach = ydetach;
    ZDetach = zdetach;
    NDetach = ndetach;
}


/*-------------------------------------------------------------------------------*/

/*
 * Set a grid that will be used to guide the shape of subsequent
 * surfaces calculated by functions in this file.  This grid must
 * use the same SurfaceGroupPlane object as any subsequent surfaces
 * to be calculated.
 */
void PadSurface::PadSetPadShapeGrid (
    CSW_F       *grid,
    int         ncol,
    int         nrow,
    double      xmin,
    double      ymin,
    double      xmax,
    double      ymax)
{
    ShapeGrid = grid;
    Ncol = ncol;
    Nrow = nrow;
    GXmin = xmin;
    GYmin = ymin;
    GXmax = xmax;
    GYmax = ymax;

    return;
}

/*-------------------------------------------------------------------------------*/

/*
 * If this flag is set to zero, the following actions are taken:
 *
 *   Faults that are steep will have their vertical grid spacing made
 *   smaller so that there is more resolution on the fault surface.
 *
 *   Fault padding attempts to extrapolate the lowest gradients farther
 *   and merge the plane fits less.
 *
 */

void PadSurface::PadSetSimPaddingFlag (int ival)
{
    SimPaddingFlag = ival;
}

/*-------------------------------------------------------------------------------*/

/*
 * Given the original detailed surface, extend it to the specified
 * xyz limits.  The surface is extended by making a coarse grid out
 * to the specified limits.  The grid nodes that are not inside
 * any original triangle and also not close to an original node are
 * appended to the original trimesh nodes and a new trimesh is
 * calculated.
 */
int PadSurface::PadSurfaceForSim(
  NOdeStruct     *nodes_in,
  EDgeStruct     *edges_in,
  TRiangleStruct *tris_in,
  int            num_nodes_in,
  int            num_edges_in,
  int            num_tris_in,
  double         xminin,
  double         xmaxin,
  double         yminin,
  double         ymaxin,
  double         zminin,
  double         zmaxin,
  CSW_F          **grid_out,
  int            *ncol_out,
  int            *nrow_out,
  double         *gxmin_out,
  double         *gymin_out,
  double         *gxmax_out,
  double         *gymax_out,
  NOdeStruct     **nodes_out,
  EDgeStruct     **edges_out,
  TRiangleStruct **tris_out,
  int            *num_nodes_out,
  int            *num_edges_out,
  int            *num_tris_out,
  double         avspace)

{

  int            numnodes;
  double         *xa = NULL, *ya = NULL, *za = NULL;
  CSW_F          *grid = NULL;
  int            ncol, nrow;
  NOdeStruct     *orig_nodes = NULL;
  int            num_orig_nodes;
  NOdeStruct     *nodes = NULL;
  EDgeStruct     *edges = NULL;
  TRiangleStruct *triangles = NULL;
  int            n_nodes, n_edges, n_triangles;
  int            i, size, status;
  double         xmin, ymin, zmin, xmax, ymax, zmax;

  bool     bsuccess = false;

  auto fscope = [&]()
  {
    csw_Free(xa);
    if (bsuccess == false) {
        csw_Free(grid);
    }
  };
  CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of error.
 */
  *grid_out = NULL;
  *ncol_out = 0;
  *nrow_out = 0;
  *gxmin_out = 1.e30;
  *gymin_out = 1.e30;
  *gxmax_out = -1.e30;
  *gymax_out = -1.e30;

  *nodes_out = NULL;
  *edges_out = NULL;
  *tris_out = NULL;
  *num_nodes_out = 0;
  *num_edges_out = 0;
  *num_tris_out = 0;

  xmin = xminin;
  ymin = yminin;
  xmax = xmaxin;
  ymax = ymaxin;
  zmin = zminin;
  zmax = zmaxin;

  zmin = zmin;
  zmax = zmax;

  /*
   * Use the orig_ symbols since they already are used throughout this function.
   * Note that orig_nodes, orig_edges, and orig_triangles should not be modified
   * or csw_Freed in this function.
   */
  orig_nodes = nodes_in;
  edges_in = edges_in;
  tris_in = tris_in;
  num_orig_nodes = num_nodes_in;
  num_edges_in = num_edges_in;
  num_tris_in = num_tris_in;

  numnodes = num_orig_nodes;

  /*
   * Put the node coordinates into xyz arrays.
   */
  size = numnodes + 2000;
  xa = (double *) csw_Malloc(3 * size * sizeof(double));
  if (xa == NULL) {
    return(-1);
  }
  ya = xa + size;
  za = ya + size;

  for (i=0; i<numnodes; i++) {
    xa[i] = orig_nodes[i].x;
    ya[i] = orig_nodes[i].y;
    za[i] = orig_nodes[i].z;
  }

  ncol = (int)((xmax - xmin) / avspace);
  nrow = (int)((ymax - ymin) / avspace);

  /*
   * Allocate space for a rectangular grid to cover the
   * entire padded area.
   */
  grid = (CSW_F *) csw_Malloc(ncol * nrow * sizeof(CSW_F));
  if (grid == NULL) {
    return(-1);
  }

  /*
   * The default size multiplier option tells the internal
   * grid calculation to use more rows and columns in its
   * internal calculation step.  If this is not set, the
   * internal calculation is likely to do a fairly coarse
   * grid and then resample it.  If that were done, the
   * smoothing of the gridded surface would make it not
   * match the input triangles closely.  Setting this option
   * improves the match to the input triangles.
   */
  grd_api_ptr->grd_SetCalcOption(GRD_DEFAULT_SIZE_MULTIPLIER, 4, 0.0f);

  grd_api_ptr->grd_SetConformableSurfaceFromDouble (
    ShapeGrid,
    Ncol, Nrow,
    GXmin, GYmin, GXmax, GYmax);

  /*
   * Calculate a rectangular grid.  The parameters are explained
   * in the function header in the csw/surfaceworks/src/grd_api.c
   * file.  This uses the original points (possibly tilted to be
   * non vertical) only.
   */
  int do_write = csw_GetDoWrite ();
  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad0.xyz");
    grd_api_ptr->grd_WritePoints (xa, ya, za, num_orig_nodes, fname);
  }

  status = grd_api_ptr->grd_CalcGridFromDouble(
    xa,
    ya,
    za,
    NULL /* optional error array */,
    num_orig_nodes,
    grid,
    NULL /* optional grid mask array */,
    NULL /* optional report pointer */,
    ncol,
    nrow,
    xmin,
    ymin,
    xmax,
    ymax,
    NULL /* optional fault cut lines */,
    0 /* number of fault cut lines */,
    NULL /* optional grid options structure */
  );

  if (status == -1) {
    return(-1);
  }

/*
 * Convert the grid to a trimesh.
 */
  n_nodes = 0;
  n_edges = 0;
  n_triangles = 0;
  status = grd_api_ptr->grd_CalcTriMeshFromGrid (
    grid, ncol, nrow,
    xmin, ymin, xmax, ymax,
    NULL, NULL, NULL, NULL, NULL, 0, // no constraint lines
    GRD_EQUILATERAL,
    &nodes,
    &edges,
    &triangles,
    &n_nodes,
    &n_edges,
    &n_triangles
  );
  if (status == -1) {
    return(-1);
  }

  *grid_out = grid;
  *ncol_out = ncol;
  *nrow_out = nrow;
  *gxmin_out = xmin;
  *gymin_out = ymin;
  *gxmax_out = xmax;
  *gymax_out = ymax;

  *nodes_out = nodes;
  *edges_out = edges;
  *tris_out = triangles;
  *num_nodes_out = n_nodes;
  *num_edges_out = n_edges;
  *num_tris_out = n_triangles;

  bsuccess = true;

  return 1;

}




/*--------------------------------------------------------------------*/


/*
 * Given the original detailed surface, extend it to the specified
 * xyz limits.  The surface is extended by making a coarse grid out
 * to the specified limits.  The grid nodes that are not inside
 * any original triangle and also not close to an original node are
 * appended to the original trimesh nodes and a new trimesh is
 * calculated.
 */
int PadSurface::PadFaultSurfaceForSim(
  NOdeStruct     *nodes_in,
  EDgeStruct     *edges_in,
  TRiangleStruct *tris_in,
  int            num_nodes_in,
  int            num_edges_in,
  int            num_tris_in,
  double         xminin,
  double         xmaxin,
  double         yminin,
  double         ymaxin,
  double         zminin,
  double         zmaxin,
  CSW_F          **grid_out,
  int            *ncol_out,
  int            *nrow_out,
  double         *gxmin_out,
  double         *gymin_out,
  double         *gxmax_out,
  double         *gymax_out,
  NOdeStruct     **nodes_out,
  EDgeStruct     **edges_out,
  TRiangleStruct **tris_out,
  int            *num_nodes_out,
  int            *num_edges_out,
  int            *num_tris_out,
  double         avspace,
  double         minage,
  double         maxage)

{

  int            numnodes;
  double         *xa = NULL, *ya = NULL, *za = NULL;
  CSW_F          *grid = NULL;
  int            ncol, nrow;
  NOdeStruct     *orig_nodes = NULL;
  int            num_orig_nodes;
  NOdeStruct     *nodes = NULL;
  EDgeStruct     *edges = NULL;
  TRiangleStruct *triangles = NULL;
  int            n_nodes, n_edges, n_triangles;
  int            i, size, status;
  double         xmin, ymin, xmax, ymax, zmin, zmax;
  double         xcorners[8], ycorners[8], zcorners[8];
  double         xdet[2], ydet[2], zdet[2];

  bool     bsuccess = false;

  auto fscope = [&]()
  {
    csw_Free(xa);
    if (bsuccess == false) {
      csw_Free(grid);
    }
  };
  CSWScopeGuard func_scope_guard (fscope);


  minage = minage;
  maxage = maxage;

/*
 * Initialize output in case of error.
 */
  *grid_out = NULL;
  *ncol_out = 0;
  *nrow_out = 0;
  *gxmin_out = 1.e30;
  *gymin_out = 1.e30;
  *gxmax_out = -1.e30;
  *gymax_out = -1.e30;

  *nodes_out = NULL;
  *edges_out = NULL;
  *tris_out = NULL;
  *num_nodes_out = 0;
  *num_edges_out = 0;
  *num_tris_out = 0;

  xmin = xminin;
  ymin = yminin;
  zmin = zminin;
  xmax = xmaxin;
  ymax = ymaxin;
  zmax = zmaxin;

/*
 * If the detachment contact line exists, use it to
 * make a new zmin slightly below the lowest point in
 * the detachment.
 */
  xdet[0] = 0.0;
  xdet[1] = 0.0;
  ydet[0] = 0.0;
  ydet[1] = 0.0;
  zdet[0] = 0.0;
  zdet[1] = 0.0;
  if (XDetach != NULL) {
    double z1, z2, z3, z4;

    z4 = (zmax - zmin) / 1000.0;

    z1 = 1.e30;
    z2 = -1.e30;

    for (i=0; i<NDetach; i++) {
      if (ZDetach[i] < z1) z1 = ZDetach[i];
      if (ZDetach[i] > z2) z2 = ZDetach[i];
    }
    if (z2 >= z1) {
      z3 = (z2 - z1) / 100.0;
      if (z3 < z4) z3 = z4;
      zmin = z1 - z4;
    }

    xdet[0] = XDetach[0];
    ydet[0] = YDetach[0];
    zdet[0] = ZDetach[0];
    xdet[1] = XDetach[NDetach-1];
    ydet[1] = YDetach[NDetach-1];
    zdet[1] = ZDetach[NDetach-1];

  }

  xcorners[0] = xmin;
  ycorners[0] = ymin;
  zcorners[0] = zmin;
  xcorners[1] = xmax;
  ycorners[1] = ymin;
  zcorners[1] = zmin;
  xcorners[2] = xmax;
  ycorners[2] = ymax;
  zcorners[2] = zmin;
  xcorners[3] = xmin;
  ycorners[3] = ymax;
  zcorners[3] = zmin;
  xcorners[4] = xmin;
  ycorners[4] = ymin;
  zcorners[4] = zmax;
  xcorners[5] = xmax;
  ycorners[5] = ymin;
  zcorners[5] = zmax;
  xcorners[6] = xmax;
  ycorners[6] = ymax;
  zcorners[6] = zmax;
  xcorners[7] = xmin;
  ycorners[7] = ymax;
  zcorners[7] = zmax;

  /*
   * Use the orig_ symbols since they already are used throughout this function.
   * Note that orig_nodes, orig_edges, and orig_triangles should not be modified
   * or csw_Freed in this function.
   */
  orig_nodes = nodes_in;
  edges_in = edges_in;
  tris_in = tris_in;
  num_orig_nodes = num_nodes_in;
  num_edges_in = num_edges_in;
  num_tris_in = num_tris_in;

  numnodes = num_orig_nodes;

  /*
   * Put the node coordinates into xyz arrays.
   */
  size = numnodes + 2000;
  xa = (double *) csw_Malloc(3 * size * sizeof(double));
  if (xa == NULL) {
    return(-1);
  }
  ya = xa + size;
  za = ya + size;

  for (i=0; i<numnodes; i++) {
    xa[i] = orig_nodes[i].x;
    ya[i] = orig_nodes[i].y;
    za[i] = orig_nodes[i].z;
  }

  int do_write;
  do_write = csw_GetDoWrite ();
  if (do_write == 1) {
    char      fname[100];
    sprintf (fname, "preplane_pts.xyz");
    grd_api_ptr->grd_WritePoints (xa, ya, za, num_orig_nodes, fname);
  }

  /*
   * If there is a surface group plane object, the points need to be
   * converted to it's coordinate system before grid calculation.
   */
  if (Sgp != NULL) {
    Sgp->convertToPlane (xa, ya, za, num_orig_nodes);
    Sgp->convertToPlane (xcorners, ycorners, zcorners, 8);
    if (XDetach != NULL) {
      Sgp->convertToPlane (xdet, ydet, zdet, 2);
    }
  }

  if (do_write == 1) {
    char      fname[100];
    sprintf (fname, "flat.xyz");
    grd_api_ptr->grd_WritePoints (xa, ya, za, num_orig_nodes, fname);
  }

  double      xp1, yp1, xp2, yp2;

  xp1 = 1.e30;
  yp1 = 1.e30;
  xp2 = -1.e30;
  yp2 = -1.e30;

  if (ShapeGrid == NULL) {
    for (i=0; i<numnodes; i++) {
      if (xa[i] < xp1) xp1 = xa[i];
      if (ya[i] < yp1) yp1 = ya[i];
      if (xa[i] > xp2) xp2 = xa[i];
      if (ya[i] > yp2) yp2 = ya[i];
    }
    for (i=0; i<8; i++) {
      if (xcorners[i] < xp1) xp1 = xcorners[i];
      if (ycorners[i] < yp1) yp1 = ycorners[i];
      if (xcorners[i] > xp2) xp2 = xcorners[i];
      if (ycorners[i] > yp2) yp2 = ycorners[i];
    }
    double dxx, dyy;
    dxx = (xp2 - xp1) / 20.0;
    dyy = (yp2 - yp1) / 20.0;
    xp1 -= dxx;
    xp2 += dxx;
    yp1 -= dyy;
    yp2 += dyy;
  }
  else {
    xp1 = GXmin;
    yp1 = GYmin;
    xp2 = GXmax;
    yp2 = GYmax;
  }

  if (xp1 >= xp2  ||  yp1 >= yp2) {
    assert (0);
  }

  xmin = xp1;
  ymin = yp1;
  xmax = xp2;
  ymax = yp2;

/*
 * If the detachment line is available, set the
 * x padding limits to its end point values.
 */
  if (XDetach != NULL) {
    if (xdet[0] < xdet[1]) {
      xmin = xdet[0];
      xmax = xdet[1];
    }
    else {
      xmin = xdet[1];
      xmax = xdet[0];
    }
  }

  ncol = (int)((xmax - xmin) / avspace);
  nrow = (int)((ymax - ymin) / avspace);
  if (SimPaddingFlag == 0) {
    double aspect = 2.0 * (xmax - xmin) / (ymax - ymin);
    if (aspect > 1.0) {
        aspect = sqrt (aspect);
        nrow = (int)(nrow * aspect);
    }
  }
  if (ncol < 2) ncol = 2;
  if (nrow < 2) nrow = 2;

  /*
   * Allocate space for a rectangular grid to cover the
   * entire padded area.
   */
  grid = (CSW_F *) csw_Malloc(ncol * nrow * sizeof(CSW_F));
  if (grid == NULL) {
    return(-1);
  }

  /*
   * The default size multiplier option tells the internal
   * grid calculation to use more rows and columns in its
   * internal calculation step.  If this is not set, the
   * internal calculation is likely to do a fairly coarse
   * grid and then resample it.  If that were done, the
   * smoothing of the gridded surface would make it not
   * match the input triangles closely.  Setting this option
   * improves the match to the input triangles.
   */
  int   pflag, tflag;
  pflag = 0;
  tflag = 103;
  if (Sgp != NULL) {
    pflag = Sgp->getPlaneFitFlag ();
    if (pflag == 0) {
      tflag += 1000;
    }
  }
  grd_api_ptr->grd_SetCalcOption(GRD_DEFAULT_SIZE_MULTIPLIER, 4, 0.0f);
  grd_api_ptr->grd_SetCalcOption(GRD_TRIANGULATE_FLAG, tflag, 0.0f);
  grd_api_ptr->grd_SetCalcOption(GRD_ANISOTROPY_FLAG, 1, 0.0f);

  grd_api_ptr->grd_SetConformableSurfaceFromDouble (
    ShapeGrid,
    Ncol, Nrow,
    GXmin, GYmin, GXmax, GYmax);

  /*
   * Calculate a rectangular grid.  The parameters are explained
   * in the function header in the csw/surfaceworks/src/grd_api.c
   * file.  This uses the original points (possibly tilted to be
   * non vertical) only.
   */
  do_write = csw_GetDoWrite ();
  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad0.xyz");
    grd_api_ptr->grd_WritePoints (xa, ya, za, num_orig_nodes, fname);
  }

  status = grd_api_ptr->grd_CalcGridFromDouble(
    xa,
    ya,
    za,
    NULL /* optional error array */,
    num_orig_nodes,
    grid,
    NULL /* optional grid mask array */,
    NULL /* optional report pointer */,
    ncol,
    nrow,
    xmin,
    ymin,
    xmax,
    ymax,
    NULL /* optional fault cut lines */,
    0 /* number of fault cut lines */,
    NULL /* optional grid options structure */
  );

  if (status == -1) {
    return(-1);
  }

  int    ismooth, ispike;
  ismooth = 0;
  ispike = 1;
  if (num_orig_nodes > 100) {
    ismooth = 1;
  }
  if (num_orig_nodes > 500) {
    ispike = 2;
  }

  status = grd_api_ptr->grd_FilterGridSpikes (
    grid, ncol, nrow, ispike);

  if (status == -1) {
    return(-1);
  }

  if (ismooth > 0) {
      status =
        grd_api_ptr->grd_SmoothGrid (grid, ncol, nrow, ismooth,
                        NULL, 0,
                        (CSW_F)xmin, (CSW_F)ymin, (CSW_F)xmax, (CSW_F)ymax,
                        -1.e30f, 1.e30f, NULL);
      if (status == -1) {
        return(-1);
      }
  }

  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad0.grd");
    grd_api_ptr->grd_WriteFile (
      fname, NULL,
      grid, NULL, NULL,
      ncol, nrow,
      xmin, ymin, xmax, ymax,
      GRD_NORMAL_GRID_FILE,
      NULL, 0);
  }

/*
 * Convert the grid to a trimesh.
 */
  n_nodes = 0;
  n_edges = 0;
  n_triangles = 0;
  status = grd_api_ptr->grd_CalcTriMeshFromGrid (
    grid, ncol, nrow,
    xmin, ymin, xmax, ymax,
    NULL, NULL, NULL, NULL, NULL, 0, // no constraint lines
    GRD_EQUILATERAL,
    &nodes,
    &edges,
    &triangles,
    &n_nodes,
    &n_edges,
    &n_triangles
  );
  if (status == -1) {
    return(-1);
  }

  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad1.tri");
    grd_api_ptr->grd_WriteTextTriMeshFile (
        0, NULL,
        triangles, n_triangles,
        edges, n_edges,
        nodes, n_nodes,
        fname);
  }

  /*
   * If there is a surface group plane object, the nodes need to be
   * converted back to the model coordinate system.
   */
  if (Sgp != NULL) {
    Sgp->convertFromPlane (nodes, n_nodes);
  }

  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad.tri");
    grd_api_ptr->grd_WriteTextTriMeshFile (
        0, NULL,
        triangles, n_triangles,
        edges, n_edges,
        nodes, n_nodes,
        fname);
  }

  *grid_out = grid;
  *ncol_out = ncol;
  *nrow_out = nrow;
  *gxmin_out = xmin;
  *gymin_out = ymin;
  *gxmax_out = xmax;
  *gymax_out = ymax;

  *nodes_out = nodes;
  *edges_out = edges;
  *tris_out = triangles;
  *num_nodes_out = n_nodes;
  *num_edges_out = n_edges;
  *num_tris_out = n_triangles;

  bsuccess = true;

  return 1;

}



/*----------------------------------------------------------------------------------------*/


void PadSurface::ProcessLowList (
    double *ya,
    int *lowlist, int nlow,
    double xmin, double xmax,
    double *ymin_io,
    double *ymax_io)
{
    int    nc1, nc2, n, i;
    double yt, dy1, dy2, ymin, ymax, dy, dx;

    ymin = *ymin_io;
    ymax = *ymax_io;
    nc1 = 0;
    nc2 = 0;

    for (i=0; i<nlow; i++) {
        n = lowlist[i];
        yt = ya[n];
        dy1 = ymax - yt;
        dy2 = ymin - yt;
        if (dy1 < 0.0) dy1 = -dy1;
        if (dy2 < 0.0) dy2 = -dy2;
        if (dy1 < dy2) {
            nc1++;
        }
        if (dy2 < dy1) {
            nc2++;
        }
    }

    dy = (ymax - ymin);
    dx = xmax - xmin;
    if (dy < dx) {
        dy *= sqrt (dx / dy);
    }

    if (nc1 > nc2) {
        ymax += dy;
    }
    if (nc2 > nc1) {
        ymin -= dy;
    }

    *ymin_io = ymin;
    *ymax_io = ymax;

    return;

}



/*--------------------------------------------------------------------*/

void PadSurface::ProcessHighList (
    double *ya,
    int *highlist, int nhigh,
    double xmin, double xmax,
    double *ymin_io,
    double *ymax_io)
{
    int    nc1, nc2, n, i;
    double yt, dy1, dy2, ymin, ymax, dy, dx;

    ymin = *ymin_io;
    ymax = *ymax_io;
    nc1 = 0;
    nc2 = 0;

    for (i=0; i<nhigh; i++) {
        n = highlist[i];
        yt = ya[n];
        dy1 = ymax - yt;
        dy2 = ymin - yt;
        if (dy1 < 0.0) dy1 = -dy1;
        if (dy2 < 0.0) dy2 = -dy2;
        if (dy1 < dy2) {
            nc1++;
        }
        if (dy2 < dy1) {
            nc2++;
        }
    }

    dy = (ymax - ymin);
    dx = xmax - xmin;
    if (dy < dx) {
        dy *= sqrt (dx / dy);
    }

    if (nc1 > nc2) {
        ymax += dy;
    }
    if (nc2 > nc1) {
        ymin -= dy;
    }

    *ymin_io = ymin;
    *ymax_io = ymax;

    return;

}




/*----------------------------------------------------------------------------------------*/

/*
 * Given the original detailed surface, extend it to the specified
 * xyz limits.  The surface is extended by making a grid at avspace
 * spacing from the input points and then converting the grid back
 * to triangles.  The Sgp object currently set (if it is not NULL)
 * is used to convert the xyz points prior to gridding and to
 * unconvert the triangle node locations after grid to trimesh.
 */
int PadSurface::PadFaultSurfaceForSim(
  double         *xin,
  double         *yin,
  double         *zin,
  int            nptsin,
  int            *lowlist,
  int            nlow,
  double         xminin,
  double         xmaxin,
  double         yminin,
  double         ymaxin,
  double         zminin,
  double         zmaxin,
  CSW_F          **grid_out,
  int            *ncol_out,
  int            *nrow_out,
  double         *gxmin_out,
  double         *gymin_out,
  double         *gxmax_out,
  double         *gymax_out,
  NOdeStruct     **nodes_out,
  EDgeStruct     **edges_out,
  TRiangleStruct **tris_out,
  int            *num_nodes_out,
  int            *num_edges_out,
  int            *num_tris_out,
  double         avspace
) {

  double         *xa = NULL, *ya = NULL, *za = NULL;
  CSW_Float      *grid;
  int            ncol, nrow;
  int            npts;
  NOdeStruct     *nodes;
  EDgeStruct     *edges;
  TRiangleStruct *triangles;
  int            n_nodes, n_edges, n_triangles;
  int            i, istat;
  double         xmin, ymin, xmax, ymax, zmin, zmax;

/*
 * Initialize output in case of error.
 */
  *grid_out = NULL;
  *ncol_out = 0;
  *nrow_out = 0;
  *gxmin_out = 1.e30;
  *gymin_out = 1.e30;
  *gxmax_out = -1.e30;
  *gymax_out = -1.e30;

  *nodes_out = NULL;
  *edges_out = NULL;
  *tris_out = NULL;
  *num_nodes_out = 0;
  *num_edges_out = 0;
  *num_tris_out = 0;

  xmin = xminin;
  ymin = yminin;
  zmin = zminin;
  xmax = xmaxin;
  ymax = ymaxin;
  zmax = zmaxin;

  zmin = zmin;
  zmax = zmax;

  xa = xin;
  ya = yin;
  za = zin;
  npts = nptsin;

  int do_write;
  do_write = csw_GetDoWrite ();
  if (do_write == 1) {
    char      fname[100];
    sprintf (fname, "preplane_pts.xyz");
    grd_api_ptr->grd_WritePoints (xa, ya, za, npts, fname);
  }

  /*
   * If there is a surface group plane object, the points need to be
   * converted to it's coordinate system before grid calculation.
   */
  if (Sgp != NULL) {
    Sgp->convertToPlane (xa, ya, za, npts);
  }

  if (do_write == 1) {
    char      fname[100];
    sprintf (fname, "flat.xyz");
    grd_api_ptr->grd_WritePoints (xa, ya, za, npts, fname);
  }

  double      xp1, yp1, xp2, yp2;

  xp1 = 1.e30;
  yp1 = 1.e30;
  xp2 = -1.e30;
  yp2 = -1.e30;

  if (ShapeGrid == NULL) {
    for (i=0; i<npts; i++) {
      if (xa[i] < xp1) xp1 = xa[i];
      if (ya[i] < yp1) yp1 = ya[i];
      if (xa[i] > xp2) xp2 = xa[i];
      if (ya[i] > yp2) yp2 = ya[i];
    }
    double dxx, dyy;
    dxx = (xp2 - xp1) / 2.0;
    dyy = (yp2 - yp1) / 2.0;
    if (lowlist != NULL  &&  nlow > 0) {
      dxx = dyy = 0.0;
    }
    xp1 -= dxx;
    xp2 += dxx;
    yp1 -= dyy;
    yp2 += dyy;
  }
  else {
    xp1 = GXmin;
    yp1 = GYmin;
    xp2 = GXmax;
    yp2 = GYmax;
  }

  if (xp1 >= xp2  ||  yp1 >= yp2) {
    assert (0);
  }

  xmin = xp1;
  ymin = yp1;
  xmax = xp2;
  ymax = yp2;

/*
 * If the lowlist is not NULL, extend only in the
 * direction of the lowlist points.
 */
  if (lowlist != NULL  &&  nlow > 0) {
    ProcessLowList (ya, lowlist, nlow,
                    xmin, xmax,
                    &ymin, &ymax);
  }

  if (avspace <= 0.0) {
    avspace = (xmax - xmin + ymax - ymin) / 100.0;
  }
  ncol = (int)((xmax - xmin) / avspace);
  nrow = (int)((ymax - ymin) / avspace);
  if (SimPaddingFlag == 0) {
    double aspect = 2.0 * (xmax - xmin) / (ymax - ymin);
    if (aspect > 1.0) {
        aspect = sqrt (aspect);
        nrow = (int)(nrow * aspect);
    }
  }
  if (ncol < 2) ncol = 2;
  if (nrow < 2) nrow = 2;

  /*
   * Allocate space for a rectangular grid to cover the
   * entire padded area.
   */
  grid = (CSW_Float *) malloc(ncol * nrow * sizeof(CSW_Float));
  if (grid == NULL) {
    free(xa);
    free(grid);
    return(-1);
  }

  /*
   * The default size multiplier option tells the internal
   * grid calculation to use more rows and columns in its
   * internal calculation step.  If this is not set, the
   * internal calculation is likely to do a fairly coarse
   * grid and then resample it.  If that were done, the
   * smoothing of the gridded surface would make it not
   * match the input triangles closely.  Setting this option
   * improves the match to the input triangles.
   */
  int   pflag, tflag;
  pflag = 0;
  tflag = 103;
  if (Sgp != NULL) {
    pflag = Sgp->getPlaneFitFlag ();
    if (pflag == 0) {
      tflag += 1000;
    }
  }
  grd_api_ptr->grd_SetCalcOption(GRD_DEFAULT_SIZE_MULTIPLIER, 4, 0.0f);
  grd_api_ptr->grd_SetCalcOption(GRD_TRIANGULATE_FLAG, tflag, 0.0f);
  grd_api_ptr->grd_SetCalcOption(GRD_ANISOTROPY_FLAG, 1, 0.0f);

  grd_api_ptr->grd_SetConformableSurfaceFromDouble (
    ShapeGrid,
    Ncol, Nrow,
    GXmin, GYmin, GXmax, GYmax);

  /*
   * Calculate a rectangular grid.  The parameters are explained
   * in the function header in the csw/surfaceworks/src/grd_api.c
   * file.  This uses the original points (possibly tilted to be
   * non vertical) only.
   */
  do_write = csw_GetDoWrite ();
  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad0.xyz");
    grd_api_ptr->grd_WritePoints (xa, ya, za, npts, fname);
  }

  istat = grd_api_ptr->grd_CalcGridFromDouble(
    xa,
    ya,
    za,
    NULL /* optional error array */,
    npts,
    grid,
    NULL /* optional grid mask array */,
    NULL /* optional report pointer */,
    ncol,
    nrow,
    xmin,
    ymin,
    xmax,
    ymax,
    NULL /* optional fault cut lines */,
    0 /* number of fault cut lines */,
    NULL /* optional grid options structure */
  );

  free (xa);
  xa = ya = za = NULL;

  if (istat == -1) {
    free(grid);
    return(-1);
  }

  int    ismooth, ispike;
  ismooth = 0;
  ispike = 1;
  if (npts > 200) {
    ismooth = 1;
  }
  if (npts > 500) {
    ispike = 2;
  }

  istat = grd_api_ptr->grd_FilterGridSpikes (
    grid, ncol, nrow, ispike);

  if (istat == -1) {
    free(grid);
    return(-1);
  }

  if (ismooth > 0) {
      istat =
        grd_api_ptr->grd_SmoothGrid (grid, ncol, nrow, 1,
                        NULL, 0,
                        (CSW_F)xmin, (CSW_F)ymin, (CSW_F)xmax, (CSW_F)ymax,
                        -1.e30f, 1.e30f, NULL);
      if (istat == -1) {
        free(grid);
        return(-1);
      }
  }

  if (do_write) {
    char      fname[100];
    sprintf (fname, "flat.grd");
    grd_api_ptr->grd_WriteFile (
      fname, NULL,
      grid, NULL, NULL,
      ncol, nrow,
      xmin, ymin, xmax, ymax,
      GRD_NORMAL_GRID_FILE,
      NULL, 0);
  }

/*
 * Convert the grid to a trimesh.
 */
  n_nodes = 0;
  n_edges = 0;
  n_triangles = 0;
  istat = grd_api_ptr->grd_CalcTriMeshFromGrid (
    grid, ncol, nrow,
    xmin, ymin, xmax, ymax,
    NULL, NULL, NULL, NULL, NULL, 0, // no constraint lines
    GRD_EQUILATERAL,
    &nodes,
    &edges,
    &triangles,
    &n_nodes,
    &n_edges,
    &n_triangles
  );
  if (istat == -1) {
    return(-1);
  }

  if (do_write) {
    char      fname[100];
    sprintf (fname, "flat.tri");
    grd_api_ptr->grd_WriteTextTriMeshFile (
        0, NULL,
        triangles, n_triangles,
        edges, n_edges,
        nodes, n_nodes,
        fname);
  }

  /*
   * If there is a surface group plane object, the nodes need to be
   * converted back to the model coordinate system.
   */
  if (Sgp != NULL) {
    Sgp->convertFromPlane (nodes, n_nodes);
  }

  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad.tri");
    grd_api_ptr->grd_WriteTextTriMeshFile (
        0, NULL,
        triangles, n_triangles,
        edges, n_edges,
        nodes, n_nodes,
        fname);
  }

  *grid_out = grid;
  *ncol_out = ncol;
  *nrow_out = nrow;
  *gxmin_out = xmin;
  *gymin_out = ymin;
  *gxmax_out = xmax;
  *gymax_out = ymax;

  *nodes_out = nodes;
  *edges_out = edges;
  *tris_out = triangles;
  *num_nodes_out = n_nodes;
  *num_edges_out = n_edges;
  *num_tris_out = n_triangles;

  return 1;

}



/*--------------------------------------------------------------------*/


/*
 * Given the original detailed surface, extend it to the specified
 * xyz limits.  The surface is extended by making a coarse grid out
 * to the specified limits.  The grid nodes that are not inside
 * any original triangle and also not close to an original node are
 * appended to the original trimesh nodes and a new trimesh is
 * calculated.
 */
int PadSurface::PadFaultSurfaceForSim(
  NOdeStruct     *nodes_in,
  EDgeStruct     *edges_in,
  TRiangleStruct *tris_in,
  int            num_nodes_in,
  int            num_edges_in,
  int            num_tris_in,
  int            *lowlist,
  int            nlow,
  int            *highlist,
  int            nhigh,
  double         low_extend_pct,
  double         high_extend_pct,
  double         xminin,
  double         xmaxin,
  double         yminin,
  double         ymaxin,
  double         zminin,
  double         zmaxin,
  CSW_F          **grid_out,
  int            *ncol_out,
  int            *nrow_out,
  double         *gxmin_out,
  double         *gymin_out,
  double         *gxmax_out,
  double         *gymax_out,
  NOdeStruct     **nodes_out,
  EDgeStruct     **edges_out,
  TRiangleStruct **tris_out,
  int            *num_nodes_out,
  int            *num_edges_out,
  int            *num_tris_out,
  double         avspace,
  double         minage,
  double         maxage
) {

  int            numnodes;
  double         *xa = NULL, *ya = NULL, *za = NULL;
  CSW_Float      *grid;
  int            ncol, nrow;
  NOdeStruct     *orig_nodes;
  int            num_orig_nodes;
  NOdeStruct     *nodes;
  EDgeStruct     *edges;
  TRiangleStruct *triangles;
  int            n_nodes, n_edges, n_triangles;
  int            i, size, status;
  double         xmin, ymin, xmax, ymax, zmin, zmax;
  double         xcorners[8], ycorners[8], zcorners[8];

  minage = minage;
  maxage = maxage;

/*
 * Initialize output in case of error.
 */
  *grid_out = NULL;
  *ncol_out = 0;
  *nrow_out = 0;
  *gxmin_out = 1.e30;
  *gymin_out = 1.e30;
  *gxmax_out = -1.e30;
  *gymax_out = -1.e30;

  *nodes_out = NULL;
  *edges_out = NULL;
  *tris_out = NULL;
  *num_nodes_out = 0;
  *num_edges_out = 0;
  *num_tris_out = 0;

  xmin = xminin;
  ymin = yminin;
  zmin = zminin;
  xmax = xmaxin;
  ymax = ymaxin;
  zmax = zmaxin;

  xcorners[0] = xmin;
  ycorners[0] = ymin;
  zcorners[0] = zmin;
  xcorners[1] = xmax;
  ycorners[1] = ymin;
  zcorners[1] = zmin;
  xcorners[2] = xmax;
  ycorners[2] = ymax;
  zcorners[2] = zmin;
  xcorners[3] = xmin;
  ycorners[3] = ymax;
  zcorners[3] = zmin;
  xcorners[4] = xmin;
  ycorners[4] = ymin;
  zcorners[4] = zmax;
  xcorners[5] = xmax;
  ycorners[5] = ymin;
  zcorners[5] = zmax;
  xcorners[6] = xmax;
  ycorners[6] = ymax;
  zcorners[6] = zmax;
  xcorners[7] = xmin;
  ycorners[7] = ymax;
  zcorners[7] = zmax;

  /*
   * Use the orig_ symbols since they already are used throughout this function.
   * Note that orig_nodes, orig_edges, and orig_triangles should not be modified
   * or freed in this function.
   */
  orig_nodes = nodes_in;
  edges_in = edges_in;
  tris_in = tris_in;
  num_orig_nodes = num_nodes_in;
  num_edges_in = num_edges_in;
  num_tris_in = num_tris_in;

  numnodes = num_orig_nodes;

  /*
   * Put the node coordinates into xyz arrays.
   */
  size = numnodes + 2000;
  xa = (double *) malloc(3 * size * sizeof(double));
  if (xa == NULL) {
    return(-1);
  }
  ya = xa + size;
  za = ya + size;

  for (i=0; i<numnodes; i++) {
    xa[i] = orig_nodes[i].x;
    ya[i] = orig_nodes[i].y;
    za[i] = orig_nodes[i].z;
  }

  int do_write;
  do_write = csw_GetDoWrite ();
  if (do_write == 1) {
    char      fname[100];
    sprintf (fname, "preplane_pts.xyz");
    grd_api_ptr->grd_WritePoints (xa, ya, za, num_orig_nodes, fname);
  }

  /*
   * If there is a surface group plane object, the points need to be
   * converted to it's coordinate system before grid calculation.
   */
  if (Sgp != NULL) {
    Sgp->convertToPlane (xa, ya, za, num_orig_nodes);
    Sgp->convertToPlane (xcorners, ycorners, zcorners, 8);
  }

  if (do_write == 1) {
    char      fname[100];
    sprintf (fname, "flat.xyz");
    grd_api_ptr->grd_WritePoints (xa, ya, za, num_orig_nodes, fname);
  }

  double      xp1, yp1, xp2, yp2;

  xp1 = 1.e30;
  yp1 = 1.e30;
  xp2 = -1.e30;
  yp2 = -1.e30;

  if (ShapeGrid == NULL) {
    for (i=0; i<numnodes; i++) {
      if (xa[i] < xp1) xp1 = xa[i];
      if (ya[i] < yp1) yp1 = ya[i];
      if (xa[i] > xp2) xp2 = xa[i];
      if (ya[i] > yp2) yp2 = ya[i];
    }
    for (i=0; i<8; i++) {
      if (xcorners[i] < xp1) xp1 = xcorners[i];
      if (ycorners[i] < yp1) yp1 = ycorners[i];
      if (xcorners[i] > xp2) xp2 = xcorners[i];
      if (ycorners[i] > yp2) yp2 = ycorners[i];
    }
    double dxx, dyy;
    dxx = (xp2 - xp1) / 20.0;
    dyy = (yp2 - yp1) / 20.0;
    if (lowlist != NULL  &&  nlow > 0) {
      dxx = dyy = 0.0;
    }
    if (highlist != NULL  &&  nhigh > 0) {
      dxx = dyy = 0.0;
    }
    xp1 -= dxx;
    xp2 += dxx;
    yp1 -= dyy;
    yp2 += dyy;
  }
  else {
    xp1 = GXmin;
    yp1 = GYmin;
    xp2 = GXmax;
    yp2 = GYmax;
  }

  if (xp1 >= xp2  ||  yp1 >= yp2) {
    assert (0);
  }

  xmin = xp1;
  ymin = yp1;
  xmax = xp2;
  ymax = yp2;

/*
 * If the lowlist is not NULL, extend only in the
 * direction of the lowlist points.
 */
  double orig_ymin, orig_ymax, max_y_extend, min_y_extend;
  orig_ymin = ymin;
  orig_ymax = ymax;
  high_extend_pct /= 100.0;
  low_extend_pct /= 100.0;
  max_y_extend = (orig_ymax - orig_ymin) * high_extend_pct;
  min_y_extend = (orig_ymax - orig_ymin) * low_extend_pct;
  if (lowlist != NULL  &&  nlow > 0) {
    ProcessLowList (ya, lowlist, nlow,
                    xmin, xmax,
                    &ymin, &ymax);
  }

  if (highlist != NULL  &&  nhigh > 0) {
    ProcessHighList (ya, highlist, nhigh,
                     xmin, xmax,
                     &ymin, &ymax);
  }

  if (ymax > orig_ymax + max_y_extend) {
    ymax = orig_ymax + max_y_extend;
  }
  if (ymin < orig_ymin - min_y_extend) {
    ymin = orig_ymin - min_y_extend;
  }

  if (avspace <= 0.0) {
    avspace = (xmax - xmin + ymax - ymin) / 100.0;
  }
  ncol = (int)((xmax - xmin) / avspace);
  nrow = (int)((ymax - ymin) / avspace);
  if (SimPaddingFlag == 0) {
    double aspect = 2.0 * (xmax - xmin) / (ymax - ymin);
    if (aspect > 1.0) {
        aspect = sqrt (aspect);
        nrow = (int)(nrow * aspect);
    }
  }
  if (ncol < 2) ncol = 2;
  if (nrow < 2) nrow = 2;

  /*
   * Allocate space for a rectangular grid to cover the
   * entire padded area.
   */
  grid = (CSW_Float *) malloc(ncol * nrow * sizeof(CSW_Float));
  if (grid == NULL) {
    free(xa);
    free(grid);
    return(-1);
  }

  /*
   * The default size multiplier option tells the internal
   * grid calculation to use more rows and columns in its
   * internal calculation step.  If this is not set, the
   * internal calculation is likely to do a fairly coarse
   * grid and then resample it.  If that were done, the
   * smoothing of the gridded surface would make it not
   * match the input triangles closely.  Setting this option
   * improves the match to the input triangles.
   */
  int   pflag, tflag;
  pflag = 0;
  tflag = 103;
  if (Sgp != NULL) {
    pflag = Sgp->getPlaneFitFlag ();
    if (pflag == 0) {
      tflag += 1000;
    }
  }
  grd_api_ptr->grd_SetCalcOption(GRD_DEFAULT_SIZE_MULTIPLIER, 4, 0.0f);
  grd_api_ptr->grd_SetCalcOption(GRD_TRIANGULATE_FLAG, tflag, 0.0f);
  grd_api_ptr->grd_SetCalcOption(GRD_ANISOTROPY_FLAG, 1, 0.0f);

  grd_api_ptr->grd_SetConformableSurfaceFromDouble (
    ShapeGrid,
    Ncol, Nrow,
    GXmin, GYmin, GXmax, GYmax);

  /*
   * Calculate a rectangular grid.  The parameters are explained
   * in the function header in the csw/surfaceworks/src/grd_api.c
   * file.  This uses the original points (possibly tilted to be
   * non vertical) only.
   */
  do_write = csw_GetDoWrite ();
  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad0.xyz");
    grd_api_ptr->grd_WritePoints (xa, ya, za, num_orig_nodes, fname);
  }

  status = grd_api_ptr->grd_CalcGridFromDouble(
    xa,
    ya,
    za,
    NULL /* optional error array */,
    num_orig_nodes,
    grid,
    NULL /* optional grid mask array */,
    NULL /* optional report pointer */,
    ncol,
    nrow,
    xmin,
    ymin,
    xmax,
    ymax,
    NULL /* optional fault cut lines */,
    0 /* number of fault cut lines */,
    NULL /* optional grid options structure */
  );

  free (xa);
  xa = ya = za = NULL;

  if (status == -1) {
    free(grid);
    return(-1);
  }

  int    ismooth, ispike;
  ismooth = 0;
  ispike = 1;
  if (num_orig_nodes > 100) {
    ismooth = 1;
  }
  if (num_orig_nodes > 500) {
    ispike = 2;
  }

  status = grd_api_ptr->grd_FilterGridSpikes (
    grid, ncol, nrow, ispike);

  if (status == -1) {
    free(grid);
    return(-1);
  }

  if (ismooth > 0) {
      status =
        grd_api_ptr->grd_SmoothGrid (grid, ncol, nrow, ismooth,
                        NULL, 0,
                        (CSW_F)xmin, (CSW_F)ymin, (CSW_F)xmax, (CSW_F)ymax,
                        -1.e30f, 1.e30f, NULL);
      if (status == -1) {
        free(grid);
        return(-1);
      }
  }

  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad0.grd");
    grd_api_ptr->grd_WriteFile (
      fname, NULL,
      grid, NULL, NULL,
      ncol, nrow,
      xmin, ymin, xmax, ymax,
      GRD_NORMAL_GRID_FILE,
      NULL, 0);
  }

/*
 * Convert the grid to a trimesh.
 */
  n_nodes = 0;
  n_edges = 0;
  n_triangles = 0;
  status = grd_api_ptr->grd_CalcTriMeshFromGrid (
    grid, ncol, nrow,
    xmin, ymin, xmax, ymax,
    NULL, NULL, NULL, NULL, NULL, 0, // no constraint lines
    GRD_EQUILATERAL,
    &nodes,
    &edges,
    &triangles,
    &n_nodes,
    &n_edges,
    &n_triangles
  );
  if (status == -1) {
    return(-1);
  }

  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad1.tri");
    grd_api_ptr->grd_WriteTextTriMeshFile (
        0, NULL,
        triangles, n_triangles,
        edges, n_edges,
        nodes, n_nodes,
        fname);
  }

  /*
   * If there is a surface group plane object, the nodes need to be
   * converted back to the model coordinate system.
   */
  if (Sgp != NULL) {
    Sgp->convertFromPlane (nodes, n_nodes);
  }

  if (do_write) {
    char      fname[100];
    sprintf (fname, "pad.tri");
    grd_api_ptr->grd_WriteTextTriMeshFile (
        0, NULL,
        triangles, n_triangles,
        edges, n_edges,
        nodes, n_nodes,
        fname);
  }

  *grid_out = grid;
  *ncol_out = ncol;
  *nrow_out = nrow;
  *gxmin_out = xmin;
  *gymin_out = ymin;
  *gxmax_out = xmax;
  *gymax_out = ymax;

  *nodes_out = nodes;
  *edges_out = edges;
  *tris_out = triangles;
  *num_nodes_out = n_nodes;
  *num_edges_out = n_edges;
  *num_tris_out = n_triangles;

  return 1;

}
