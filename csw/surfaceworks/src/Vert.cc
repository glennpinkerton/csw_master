
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

#include <csw/surfaceworks/src/SurfaceGroupPlane.h>
#include <csw/surfaceworks/private_include/Vert.h>
#include <csw/surfaceworks/include/grid_api.h>

#include <csw/surfaceworks/private_include/grd_fileio.h>


/*----------------------------------------------------------------------------*/

/*
 * This file has functions callable from C to use the functionality of the
 * C++ SurfaceGroupPlane class.  This class overrides the previous functionality
 * for converting "vertical" coordinates.  The function names and calling
 * sequences in this file have not changed.  However, they now use completely
 * different underlying algorithms.  There are still six double values that define
 * the conversion to and from "steep" coordinates.  The values are interpreted quite
 * differently internally.   The first three returned values are now the coefficients
 * of the best fitting plane (in a least squares sense) to the data points.  The
 * 4th 5th and 6th return values are respectively the x, y and z origin used to rotate the
 * plane to horizontal.  Conversion of points now consists of rotation to horizontal
 * and unconversion is reverse rotation back to the original plane.
 */

/*----------------------------------------------------------------------------*/

/*
 * Fit a plane to the points.  If the x and y coefficients are both less than .05,
 * then the points are essentially horizontal and don't need any rotation.  In this
 * case, the return status is zero and all returned values are 1.e30.  If the
 * plane is relatively steep, the coefs and origin are returned, with a status of 1.
 *
 * The calling function really doesn't have to know any of this.  It only has to
 * record the double values in the order they are returned and send them back
 * to these functions in the same order, which it had to do with the old implementation
 * as well.
 *
 * Input parameters:
 *
 *  xpts        Array of x coordinates for the surface, including any constraint lines
 *  ypts        Array of y coordinates for the surface, including any constraint lines
 *  zpts        Array of z coordinates for the surface, including any constraint lines
 *  npts        Number of points.
 *  fault_flag  Set to 1 if the surface is for a fault or set to zero otherwise.
 *              If this is set to -1, it is almost impossible to get a steep surface
 *              returned from this function.
 *  c1_out      First coefficient of the plane
 *  c2_out      Second coefficient of the plane.
 *  c3_out      Third coefficient of the plane.
 *  x0_out      X origin of rotation.
 *  y0_out      Y origin of rotation.
 *  z0_out      Z origin or rotation.
 *
 */
extern "C" {
  int vert_CalcBaseline (
    double *xpts,
    double *ypts,
    double *zpts,
    int    npts,
    int    fault_flag,
    double *c1_out,
    double *c2_out,
    double *c3_out,
    double *x0_out,
    double *y0_out,
    double *z0_out)
  {
    int              istat;
    SurfaceGroupPlane  *sgp = NULL;

    CSWGrdFileio     grd_fileio_obj;

    int do_write = csw_GetDoWrite ();
    if (do_write) {
        char    fname[100];
        sprintf (fname, "vertpoints.xyz");
        grd_fileio_obj.grd_write_points (xpts, ypts, zpts, npts, fname);
    }

  /*
   * Set output in case an error occurs.
   */
    *c1_out = 1.e30;
    *c2_out = 1.e30;
    *c3_out = 1.e30;
    *x0_out = 1.e30;
    *y0_out = 1.e30;
    *z0_out = 1.e30;

    try {
        sgp = new SurfaceGroupPlane ();
    }
    catch (...) {
        sgp = NULL;
    }

    if (sgp == NULL) {
        return -1;
    }
    sgp->setFaultFlag (fault_flag);

    sgp->addPointsForFit (xpts, ypts, zpts, npts);

    double  coef[3];

    istat =
      sgp->calcPlaneCoefs (coef);
    if (istat != 1) {
        delete (sgp);
        return 0;
    }

    *c1_out = coef[0];
    *c2_out = coef[1];
    *c3_out = coef[2];
    istat = sgp->getOrigin (x0_out, y0_out, z0_out);
    if (istat != 1) {
        *c1_out = 1.e30;
        *c2_out = 1.e30;
        *c3_out = 1.e30;
        *x0_out = 1.e30;
        *y0_out = 1.e30;
        *z0_out = 1.e30;
        delete (sgp);
        return 0;
    }

    int hflag = sgp->getHorizontalFlag ();
    if (hflag == 1) {
        istat = 0;
    }
    else {
        istat = 1;
    }

    delete (sgp);

    return istat;

  }
} // end of extern "C" block

/*--------------------------------------------------------------------------------*/

/*
 * Set baseline endpoint values that have been previously calculated.
 * The baseline points are the points originally calculated from the
 * vert_CalcBaseline function.  If you are absolutely certain of the
 * extents of the "vertical" data, you can set these manually.  Make
 * sure that all of the data can project onto the baseline between the
 * specified endpoints.
 */
extern "C" {

  void vert_SetBaseline (
    double    c1,
    double    c2,
    double    c3,
    double    x0,
    double    y0,
    double    z0,
    int       flag,
    GRDVert   &gvert)
  {
    SurfaceGroupPlane  *Sgp = &gvert.sgp;
    int    UseFlag = gvert.useflag;
    if (flag == 0) {
        UseFlag = 0;
    }
    else {
        Sgp->setCoefsAndOrigin (c1, c2, c3, x0, y0, z0);
        UseFlag = flag;
    }
    gvert.useflag = UseFlag;
  }

  void vert_GetBaseline (
    double *c1,
    double *c2,
    double *c3,
    double *x0,
    double *y0,
    double *z0,
    int    *use,
    GRDVert  &gvert
  )
  {
    *c1 = 1.e30;
    *c2 = 1.e30;
    *c3 = 1.e30;
    *x0 = 1.e30;
    *y0 = 1.e30;
    *z0 = 1.e30;
    *use = 0;

    SurfaceGroupPlane  *Sgp = &gvert.sgp;

    Sgp->getCoefsAndOrigin (c1, c2, c3, x0, y0, z0);
    if (*c1 < 1.e30) {
        *use = 1;
    }

    return;

  }

  int vert_GetFlag (GRDVert &gvert)
  {
    return gvert.useflag;
  }



/*----------------------------------------------------------------------------------*/

/*
 * Unset the baseline, turning off conversion and unconversion.
 */
  void vert_UnsetBaseline (void)
  {
  }

}  // end of extern C

/*----------------------------------------------------------------------------------*/

/*
 * Convert an array of points from xyz to steep surface coords.
 * The conversion replaces the current values in the array with
 * the new values.
 */

extern "C" {
  int vert_ConvertPoints (
    double     *xpts,
    double     *ypts,
    double     *zpts,
    int        npts,
    GRDVert    &gvert
  )
  {

    SurfaceGroupPlane    *Sgp = &gvert.sgp;

    Sgp->convertToPlane (xpts, ypts, zpts, npts);

    return 1;

  }

/*----------------------------------------------------------------------------------*/

/*
 * Convert an array of points from steep surface coords to xyz coords.
 * The conversion is done in place.
 */
  int vert_UnconvertPoints (
    double     *xpts,
    double     *ypts,
    double     *zpts,
    int        npts,
    GRDVert    &gvert
  )
  {
    SurfaceGroupPlane  *Sgp = &gvert.sgp;
    Sgp->convertFromPlane (xpts, ypts, zpts, npts);

    return 1;

  }

}  // end of extern "C"


/*----------------------------------------------------------------------------------*/

/*
 * Convert an array of trimesh nodes from xyz to steep surface coords.
 * The conversion is done in place.
 */

extern "C" {
  int vert_ConvertTriMeshNodes (
    NOdeStruct *nodes,
    int        npts,
    GRDVert    &gvert
  )
  {

    SurfaceGroupPlane   *Sgp = &gvert.sgp;

    Sgp->convertToPlane (nodes, npts);

    return 1;

  }

/*----------------------------------------------------------------------------------*/

/*
 * Convert an array of trimesh nodes from steep surface coords to xyz coords.
 * The conversion is done in place.
 */
  int vert_UnconvertTriMeshNodes (
    NOdeStruct *nodes,
    int        npts,
    GRDVert    &gvert
  )
  {

    SurfaceGroupPlane   *Sgp = &gvert.sgp;

    Sgp->convertFromPlane (nodes, npts);

    return 1;

  }

}  // end of extern "C"

/*------------------------------------------------------------------------*/

/*
 * Decide if the specified cloud of points is best represented
 * by rotating its best fit plane to horizontal.
 *
 * The baseline points need to be saved so you can set them via the vert_SetBaseline
 * function before actually converting or unconverting points.
 *
 *
 * Input parameters:
 *
 *  nodes       Array of NOdeStructs with point locations.
 *  npts        Number of nodes in the array.
 *  fault_flag  Set to 1 if the surface is for a fault or set to zero otherwise.
 *  c1_out      First coefficient of plane fit.
 *  c2_out      Second coefficient of plane fit.
 *  c3_out      Third coefficient of plane fit.
 *  x0_out      X origin of plane rotation.
 *  y0_out      Y origin of plane rotation.
 *  z0_out      Z origin of plane rotation.
 */
extern "C" {
  int vert_CalcBaselineFromNodes (
    NOdeStruct    *nodes,
    int    npts,
    int    fault_flag,
    double *c1_out,
    double *c2_out,
    double *c3_out,
    double *x0_out,
    double *y0_out,
    double *z0_out
  )

  {
    double            *xpts, *ypts, *zpts;
    int               i, istat;

    if (nodes == NULL  ||  npts < 3) {
        return -1;
    }

    xpts = (double *)csw_Malloc (3 * npts * sizeof(double));
    if (xpts == NULL) {
        return -1;
    }
    ypts = xpts + npts;
    zpts = ypts + npts;

    for (i=0; i<npts; i++) {
        xpts[i] = nodes[i].x;
        ypts[i] = nodes[i].y;
        zpts[i] = nodes[i].z;
    }

    istat = vert_CalcBaseline (
        xpts,
        ypts,
        zpts,
        npts,
        fault_flag,
        c1_out,
        c2_out,
        c3_out,
        x0_out,
        y0_out,
        z0_out);

    csw_Free (xpts);

    return istat;

  }

}  // end of extern C
