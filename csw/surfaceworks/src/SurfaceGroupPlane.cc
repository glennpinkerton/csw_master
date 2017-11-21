
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * Include system headers.
 */
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
#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "SurfaceGroupPlane.h"

#define _MAX_Z_NORMAL_       .75




/*-----------------------------------------------------------------------*/

/**
 * This is an empty constructor for a SurfaceGroupPlane object.  The object is
 * only initialized to an empty state when this is used.  To make the object
 * useful, you need to add horizons and faults with the various versions of
 * the addInputHorizon and addInputFault methods.
 */
SurfaceGroupPlane::SurfaceGroupPlane ()
{
    init ();
}


SurfaceGroupPlane::SurfaceGroupPlane (SurfaceGroupPlane *sptr)
{
    copy_from_pointer (sptr);
}

SurfaceGroupPlane::SurfaceGroupPlane (SurfaceGroupPlane const & src)
{
    copy_from_reference (src);
}

SurfaceGroupPlane const & SurfaceGroupPlane::operator = (SurfaceGroupPlane const & src)
{
    if (&src == this) {
        return (*this);
    }

    copy_from_reference (src);

    return (*this);
}

void SurfaceGroupPlane::copy_from_pointer (SurfaceGroupPlane *sptr)
{
    init ();
    coef[0] = sptr->coef[0];
    coef[1] = sptr->coef[1];
    coef[2] = sptr->coef[2];
    xorigin = sptr->xorigin;
    yorigin = sptr->yorigin;
    zorigin = sptr->zorigin;
    horizontal_flag = sptr->horizontal_flag;

    zerodist = sptr->zerodist;

    return;
}

void SurfaceGroupPlane::copy_from_reference (SurfaceGroupPlane const & src)
{
    init ();
    coef[0] = src.coef[0];
    coef[1] = src.coef[1];
    coef[2] = src.coef[2];
    xorigin = src.xorigin;
    yorigin = src.yorigin;
    zorigin = src.zorigin;
    horizontal_flag = src.horizontal_flag;

    zerodist = src.zerodist;

    return;
}


/*-----------------------------------------------------------------------*/

/**
 * The destructor csw_Frees all memory that was allocated in using the object
 * and sets the object to its empty state.
 */
SurfaceGroupPlane::~SurfaceGroupPlane()
{
    free_mem ();
}



/*-----------------------------------------------------------------------*/

/*
 * Initialize the private variables appropriately.  This should only
 * be called from constructors or the csw_Free_mem method.
 *
 * This is a private method.
 */
void SurfaceGroupPlane::init (void)
{
    xfit = yfit = zfit = NULL;
    numfit = 0;
    maxfit = 0;

    coef[0] = 1.e30;
    coef[1] = 1.e30;
    coef[2] = 1.e30;
    coef_flag = 0;

    xorigin = 1.e30;
    yorigin = 1.e30;
    zorigin = 1.e30;

    horizontal_flag = 0;

    tnxNorm = 0.0;
    tnyNorm = 0.0;
    tnzNorm = 1.0;

    numNorm = 0;
    avxNorm = 0.0;
    avyNorm = 0.0;
    avzNorm = 0.0;

    zerodist = 1.e30;
    zrange = -1.0;

    plane_fit_flag = 0;

    fault_flag = -1;

    return;
}


/*----------------------------------------------------------------------*/

void SurfaceGroupPlane::setFaultFlag (int ival)
{
    fault_flag = ival;
}

/*----------------------------------------------------------------------*/

int SurfaceGroupPlane::getPlaneFitFlag (void)
{
    return plane_fit_flag;
}

/*----------------------------------------------------------------------*/

/*
 * Free the memory for the private data members and re initialize them
 * all to their empty values.
 *
 * This is a private method.
 */
void SurfaceGroupPlane::free_mem (void)
{
    csw_Free (xfit);
    csw_Free (yfit);
    csw_Free (zfit);

    init ();

    return;
}

/*-----------------------------------------------------------------------*/

/*
 * Set the plane coefficients from an external source.  This could be
 * used, for instance, if a plane is calculated and then saved for
 * reuse later on.  This is a public method.
 */
void SurfaceGroupPlane::setPlaneCoefs (double *c)
{
    coef[0] = c[0];
    coef[1] = c[1];
    coef[2] = c[2];
    coef_flag = 1;

    return;

}


/*-----------------------------------------------------------------------*/

int SurfaceGroupPlane::getPlaneCoefs (double *c)
{
    if (c == NULL) {
        return -1;
    }

    if (coef_flag == 0) {
        c[0] = 1.e30;
        c[1] = 1.e30;
        c[2] = 1.e30;
        return -1;
    }

    c[0] = coef[0];
    c[1] = coef[1];
    c[2] = coef[2];

    return 1;

}


int SurfaceGroupPlane::getOrigin (double *xorig,
                                  double *yorig,
                                  double *zorig)
{
    if (xorig == NULL  ||  yorig == NULL  ||  zorig == NULL) {
        return -1;
    }

    *xorig = xorigin;
    *yorig = yorigin;
    *zorig = zorigin;

    if (coef_flag == 0) {
        return -1;
    }

    return 1;
}


void SurfaceGroupPlane::setOrigin (double xorig, double yorig, double zorig)
{
    xorigin = xorig;
    yorigin = yorig;
    zorigin = zorig;
}

void SurfaceGroupPlane::setCoefsAndOrigin (
        double    c1,
        double    c2,
        double    c3,
        double    x0,
        double    y0,
        double    z0)
{
    coef[0] = c1;
    coef[1] = c2;
    coef[2] = c3;
    xorigin = x0;
    yorigin = y0;
    zorigin = z0;
    return;
}

int SurfaceGroupPlane::getCoefsAndOrigin (
        double    *c1,
        double    *c2,
        double    *c3,
        double    *x0,
        double    *y0,
        double    *z0)
{
    *c1 = 1.e30;
    *c2 = 1.e30;
    *c3 = 1.e30;
    *x0 = 1.e30;
    *y0 = 1.e30;
    *z0 = 1.e30;

    if (horizontal_flag == 1) {
        return 0;
    }

    *c1 = coef[0];
    *c2 = coef[1];
    *c3 = coef[2];
    *x0 = xorigin;
    *y0 = yorigin;
    *z0 = zorigin;

    return 1;
}




/*-----------------------------------------------------------------------*/

/*
 * Add the nodes from the specified trimesh to the fit point arrays.
 * This is a public method.
 */
int SurfaceGroupPlane::addTriMeshForFit (
    NOdeStruct       *nodes,
    int              numnodes,
    EDgeStruct       *edges,
    int              numedges,
    TRiangleStruct   *tris,
    int              numtris)
{
    int              i, istat;
    NOdeStruct       *nptr;

    UpdateAverageNormals (
        nodes, edges,
        tris, numtris);

/*
 * Loop through the nodes and add each non deleted node
 * to the fit arrays.
 */
    for (i=0; i<numnodes; i++) {
        nptr = nodes + i;
        if (nptr->deleted == 0) {
            istat =
              add_fit_point (nptr->x, nptr->y, nptr->z);
            if (istat == -1) {
                return -1;
            }
        }
    }

    numedges = numedges;

    return 1;

}

/*-----------------------------------------------------------------------*/

/*
 * Add the specified points to the fit point arrays.  This is
 * a public method.
 */
int SurfaceGroupPlane::addPointsForFit (
    double      *x,
    double      *y,
    double      *z,
    int         npts)
{
    int         i, istat;

    for (i=0; i<npts; i++) {
        istat =
          add_fit_point (x[i], y[i], z[i]);
        if (istat == -1) {
            return -1;
        }
    }

    return 1;

}


/*-----------------------------------------------------------------------*/

/*
 * Add a point at a time to the fit arrays, growing the arrays as needed.
 * This is a private method.
 */
int SurfaceGroupPlane::add_fit_point (double x, double y, double z)
{

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        if (bsuccess == false) {
            freeFitPoints ();
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Expand the arrays if needed.
 */
    if (numfit >= maxfit) {
        maxfit += 1000;
        xfit = (double *)csw_Realloc (xfit, maxfit * sizeof(double));
        yfit = (double *)csw_Realloc (yfit, maxfit * sizeof(double));
        zfit = (double *)csw_Realloc (zfit, maxfit * sizeof(double));
    }

    if (xfit == NULL  ||  yfit == NULL  ||  zfit == NULL) {
        return -1;
    }

    xfit[numfit] = x;
    yfit[numfit] = y;
    zfit[numfit] = z;
    numfit++;

    bsuccess = true;

    return 1;

}

void SurfaceGroupPlane::freeFitPoints (void)
{
    csw_Free (xfit);
    csw_Free (yfit);
    csw_Free (zfit);

    xfit = yfit = zfit = NULL;
    numfit = 0;
    maxfit = 0;
}


/*-----------------------------------------------------------------------*/

/*
 * Calculate the coefficents of a plane that is best for projecting
 * the surface points to.  If there are less than 3 surface points
 * currently defined, this method returns -1.  If the specified c
 * pointer is NULL, -1 is returned.  On success, 1 is returned and
 * the c array is filled with the 3 coefficients.  If the return is
 * -1 and c is not a NULL pointer, c is filled with 1.e30 values.
 */
int SurfaceGroupPlane::calcPlaneCoefs (double *c)
{

    double       xmid, ymid, zmid;
    double       zscale;

    if (c == NULL) {
        return -1;
    }

    if (numfit < 3) {
        return -1;
    }

/*
 * Initialize output in case of error.
 */
    c[0] = 1.e30;
    c[1] = 1.e30;
    c[2] = 1.e30;

    plane_fit_flag = 0;

    if (xfit == NULL  ||
        yfit == NULL  ||
        zfit == NULL) {
        return -1;
    }

/*
 * Find the bounding box of the fit data.
 */
    double       xmin, ymin, zmin, xmax, ymax, zmax;
    int          i;

    xmin = ymin = zmin = 1.e30;
    xmax = ymax = zmax = -1.e30;

    for (i=0; i<numfit; i++) {
        if (xfit[i] < xmin) xmin = xfit[i];
        if (yfit[i] < ymin) ymin = yfit[i];
        if (zfit[i] < zmin) zmin = zfit[i];
        if (xfit[i] > xmax) xmax = xfit[i];
        if (yfit[i] > ymax) ymax = yfit[i];
        if (zfit[i] > zmax) zmax = zfit[i];
    }

    if (xmin >= xmax && ymin >= ymax) {
        return -1;
    }

    if (zmax <= zmin) {
        zscale = 1.0;
    }
    else {
        zscale = (xmax - xmin + ymax - ymin) / (zmax - zmin);
        zscale /= 2.0;
    }

/*
 * If this is definately not a fault, do not scale the z value.
 */
    if (fault_flag == 0) {
        zscale = 1.0;
    }

/*
 * If the fault flag has not been set, use a scaling that
 * exagerates the z value range to the average of the x and y ranges.
 */
    if (fault_flag == -1) {
        zscale /= 4.0;
    }

    if (fault_flag != 1) {
        plane_fit_flag = 999;
        return 1;
    }

    if (zscale < 1.0) zscale = 1.0;

    zerodist = (xmax - xmin + ymax - ymin) / 20000.0;

    xmid = (xmin + xmax) / 2.0;
    ymid = (ymin + ymax) / 2.0;
    zmid = (zmin + zmax) / 2.0;

/*
 * Transform all fit coordinates to an origin at xmid, ymid, zmid.
 */
    for (i=0; i<numfit; i++) {
        xfit[i] -= xmid;
        yfit[i] -= ymid;
        zfit[i] -= zmid;
        zfit[i] *= zscale;
    }

/*
 * Fit a plane through the transformed points.
 */
    double     ctmp[3];
    int        istat;

    istat =
      grd_tsurf_ptr->grd_calc_perp_plane_fit (xfit, yfit, zfit, numfit, ctmp);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<numfit; i++) {
        zfit[i] /= zscale;
        xfit[i] += xmid;
        yfit[i] += ymid;
        zfit[i] += zmid;
    }

    memcpy (coef, ctmp, 3 * sizeof(double));
    coef[0] /= zscale;
    coef[1] /= zscale;
    coef[2] /= zscale;

    int do_write = csw_GetDoWrite ();
    if (do_write) {
        OutputSVDResults (xfit, yfit, zfit, numfit,
                          coef, (char *)"sgp.out");
    }

/*
 * Shift the origin so the plane goes through it.  Translate by the
 * x or y axis intercept, whichever is less, to accomplish this.
 */
    double        xint, yint, axint, ayint;

    xint = 1.e30;
    yint = 1.e30;

    if (coef[2] < -0.01  ||  coef[2] > .01) {
        yint = -coef[0] / coef[2];
    }
    if (coef[1] < -0.01  ||  coef[1] > .01) {
        xint = -coef[0] / coef[1];
    }

    horizontal_flag = 0;
    if (xint > 1.e20  &&  yint > 1.e20) {
        horizontal_flag = 1;
        xorigin = xmid;
        yorigin = ymid;
        zorigin = zmid;
        return 1;
    }

    axint = xint;
    if (xint < 0.0) axint = -xint;
    ayint = yint;
    if (yint < 0.0) ayint = -yint;
    if (axint < ayint) {
        xorigin = xmid + xint;
        yorigin = ymid;
        zorigin = zmid;
    }
    else {
        xorigin = xmid;
        yorigin = ymid + yint;
        zorigin = zmid;
    }

    coef_flag = 1;

    c[0] = coef[0];
    c[1] = coef[1];
    c[2] = coef[2];

    return 1;

}


/*-----------------------------------------------------------------------*/

int SurfaceGroupPlane::convertToPlane
    (double    *x,
     double    *y,
     double    *z,
     int       npts)
{
    int        i;
    double     xt, yt, zt, xt2, yt2, z100, zt1, zt2;
    double     xang, zang, xcos, xsin, zcos, zsin;
    double     x1, y1, z1, x2, y2, z2, dx, dy, dz;
    double     pi = 3.1415926;
    double     halfpi = 3.1415926 / 2.0;
    double     dipang, x3, y3, dist;

    if (plane_fit_flag == 999  ||
        fault_flag != 1) {
        return 1;
    }

/*
 * Check for obvious errors.
 */
    if (x == NULL  ||  y == NULL  ||  z == NULL) {
        return 1;
    }

    if (npts < 1) {
        return 1;
    }

/*
 * Find the points where the z = 0 contour crosses the
 * x and y axes respectively.  These points define the
 * strike of the plane.
 */
    if (coef[1] > -0.0001  &&  coef[1] < 0.0001) {
        x1 = 0.0;
        x2 = 0.0;
        if (coef[2] < -.0001 ||  coef[2] > .0001) {
            x2 = 10000.0;
        }
    }
    else {
        x1 = coef[0] / coef[1];
        x2 = 0.0;
    }

    if (coef[2] > -0.0001  &&  coef[2] < 0.0001) {
        y1 = 0.0;
        y2 = 0.0;
        if (coef[1] < -.0001 ||  coef[1] > .0001) {
            y2 = 10000.0;
        }
    }
    else {
        y1 = 0.0;
        y2 = coef[0] / coef[2];
    }

    dx = x2 - x1;
    dy = y2 - y1;

/*
 * It is possible that the z = 0 contour crosses exactly or very nearly
 * exactly through the origin, and the strike angle has no meaning.
 * If this is the case, try a z contour appropriate for the plane coefs.
 */
    if (NearZeroDist (dx)  &&  NearZeroDist (dy)) {

        zt1 = coef[1];
        zt2 = coef[2];
        if (zt1 < 0.0) zt1 = -zt1;
        if (zt2 < 0.0) zt2 = -zt2;
        z100 = (zt1 + zt2) / 2.0;

        if (coef[1] > -0.0001  &&  coef[1] < 0.0001) {
            x1 = 0.0;
            x2 = 0.0;
        }
        else {
            x1 = (z100 - coef[0]) / coef[1];
            x2 = 0.0;
        }

        if (coef[2] > -0.0001  &&  coef[2] < 0.0001) {
            y1 = 0.0;
            y2 = 0.0;
        }
        else {
            y1 = 0.0;
            y2 = (z100 - coef[0]) / coef[2];
        }

        dx = x2 - x1;
        dy = y2 - y1;

    /*
     * If the contour is still zero length, the plane is essentially
     * horizontal and no rotation is needed.
     */
        if (NearZeroDist (dx)  &&  NearZeroDist (dy)) {
            return 1;
        }
    }

    zang = atan2 (dy, dx);
    if (zang < -halfpi) zang += pi;
    if (zang > halfpi) zang -= pi;

    dipang = zang + halfpi;
    x3 = x1 + cos (dipang);
    y3 = y1 + sin (dipang);
    dx = x3 - x1;
    dy = y3 - y1;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    z1 = coef[0] + coef[1] * x1 + coef[2] * y1;
    z2 = coef[0] + coef[1] * x3 + coef[2] * y3;
    dz = z2 - z1;
    xang = atan2 (dz, dist);

    xang = -xang;
    zang = -zang;

    xcos = cos (xang);
    zcos = cos (zang);
    xsin = sin (xang);
    zsin = sin (zang);

    for (i=0; i<npts; i++) {
        x[i] -= xorigin;
        y[i] -= yorigin;
        z[i] -= zorigin;
    }

    for (i=0; i<npts; i++) {
        xt = x[i] * zcos - y[i] * zsin;
        yt = x[i] * zsin + y[i] * zcos;
        zt = z[i];
        xt2 = xt;
        yt2 = yt * xcos - zt * xsin;
        zt2 = yt * xsin + zt * xcos;
        x[i] = xt2;
        y[i] = yt2;
        z[i] = zt2;
    }

    return 1;

}


/*-----------------------------------------------------------------------*/

int SurfaceGroupPlane::convertFromPlane
    (double    *x,
     double    *y,
     double    *z,
     int       npts)
{
    int        i;
    double     xt, yt, zt, xt2, yt2, z100, zt1, zt2;
    double     xang, zang, xcos, xsin, zcos, zsin;
    double     x1, y1, z1, x2, y2, z2, dx, dy, dz;
    double     pi = 3.1415926;
    double     halfpi = 3.1415926 / 2.0;
    double     dipang, x3, y3, dist;

    if (plane_fit_flag == 999  ||
        fault_flag != 1) {
        return 1;
    }


/*
 * Check for obvious errors.
 */
    if (x == NULL  ||  y == NULL  ||  z == NULL) {
        return 1;
    }

    if (npts < 1) {
        return 1;
    }

/*
 * Find the points where the z = 0 contour crosses the
 * x and y axes respectively.  These points define the
 * strike of the plane.
 */
    if (coef[1] > -0.0001  &&  coef[1] < 0.0001) {
        x1 = 0.0;
        x2 = 0.0;
        if (coef[2] < -.0001 ||  coef[2] > .0001) {
            x2 = 10000.0;
        }
    }
    else {
        x1 = coef[0] / coef[1];
        x2 = 0.0;
    }

    if (coef[2] > -0.0001  &&  coef[2] < 0.0001) {
        y1 = 0.0;
        y2 = 0.0;
        if (coef[1] < -.0001 ||  coef[1] > .0001) {
            y2 = 10000.0;
        }
    }
    else {
        y1 = 0.0;
        y2 = coef[0] / coef[2];
    }

    dx = x2 - x1;
    dy = y2 - y1;

/*
 * It is possible that the z = 0 contour crosses exactly or very nearly
 * exactly through the origin, and the strike angle has no meaning.
 * If this is the case, try the z = 1 contour.
 */
    if (NearZeroDist (dx)  &&  NearZeroDist (dy)) {

        zt1 = coef[1];
        zt2 = coef[2];
        if (zt1 < 0.0) zt1 = -zt1;
        if (zt2 < 0.0) zt2 = -zt2;
        z100 = (zt1 + zt2) / 2.0;

        if (coef[1] > -0.0001  &&  coef[1] < 0.0001) {
            x1 = 0.0;
            x2 = 0.0;
        }
        else {
            x1 = (z100 - coef[0]) / coef[1];
            x2 = 0.0;
        }

        if (coef[2] > -0.0001  &&  coef[2] < 0.0001) {
            y1 = 0.0;
            y2 = 0.0;
        }
        else {
            y1 = 0.0;
            y2 = (z100 - coef[0]) / coef[2];
        }

        dx = x2 - x1;
        dy = y2 - y1;

    /*
     * If the contour is still zero length, the plane is essentially
     * horizontal and no rotation is needed.
     */
        if (NearZeroDist (dx)  &&  NearZeroDist (dy)) {
            return 1;
        }
    }

    zang = atan2 (dy, dx);
    if (zang < -halfpi) zang += pi;
    if (zang > halfpi) zang -= pi;

    dipang = zang + halfpi;
    x3 = x1 + cos (dipang);
    y3 = y1 + sin (dipang);
    dx = x3 - x1;
    dy = y3 - y1;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    z1 = coef[0] + coef[1] * x1 + coef[2] * y1;
    z2 = coef[0] + coef[1] * x3 + coef[2] * y3;
    dz = z2 - z1;
    xang = atan2 (dz, dist);

    xcos = cos (xang);
    zcos = cos (zang);
    xsin = sin (xang);
    zsin = sin (zang);

    for (i=0; i<npts; i++) {
        xt = x[i];
        yt = y[i] * xcos - z[i] * xsin;
        zt = y[i] * xsin + z[i] * xcos;
        xt2 = xt * zcos - yt * zsin;
        yt2 = xt * zsin + yt * zcos;
        zt2 = zt;
        x[i] = xt2;
        y[i] = yt2;
        z[i] = zt2;
    }

    for (i=0; i<npts; i++) {
        x[i] += xorigin;
        y[i] += yorigin;
        z[i] += zorigin;
    }

    return 1;

}


/*-----------------------------------------------------------------------*/

int SurfaceGroupPlane::convertToPlane
    (NOdeStruct     *nodes,
     int            npts)
{
    int        i;
    double     xt, yt, zt, xt2, yt2, z100, zt1, zt2;
    double     xang, zang, xcos, xsin, zcos, zsin;
    double     x1, y1, z1, x2, y2, z2, dx, dy, dz;
    double     pi = 3.1415926;
    double     halfpi = 3.1415926 / 2.0;
    double     dipang, x3, y3, dist;

    if (plane_fit_flag == 999) {
        return 1;
    }

/*
 * Check for obvious errors.
 */
    if (nodes == NULL) {
        return 1;
    }

    if (npts < 1) {
        return 1;
    }

/*
 * Find the points where the z = 0 contour crosses the
 * x and y axes respectively.  These points define the
 * strike of the plane.
 */
    if (coef[1] > -0.0001  &&  coef[1] < 0.0001) {
        x1 = 0.0;
        x2 = 0.0;
        if (coef[2] < -.0001 ||  coef[2] > .0001) {
            x2 = 10000.0;
        }
    }
    else {
        x1 = coef[0] / coef[1];
        x2 = 0.0;
    }

    if (coef[2] > -0.0001  &&  coef[2] < 0.0001) {
        y1 = 0.0;
        y2 = 0.0;
        if (coef[1] < -.0001 ||  coef[1] > .0001) {
            y2 = 10000.0;
        }
    }
    else {
        y1 = 0.0;
        y2 = coef[0] / coef[2];
    }

    dx = x2 - x1;
    dy = y2 - y1;

/*
 * It is possible that the z = 0 contour crosses exactly or very nearly
 * exactly through the origin, and the strike angle has no meaning.
 * If this is the case, try the z = 1 contour.
 */
    if (NearZeroDist (dx)  &&  NearZeroDist (dy)) {

        zt1 = coef[1];
        zt2 = coef[2];
        if (zt1 < 0.0) zt1 = -zt1;
        if (zt2 < 0.0) zt2 = -zt2;
        z100 = (zt1 + zt2) / 2.0;

        if (coef[1] > -0.0001  &&  coef[1] < 0.0001) {
            x1 = 0.0;
            x2 = 0.0;
        }
        else {
            x1 = (z100 - coef[0]) / coef[1];
            x2 = 0.0;
        }

        if (coef[2] > -0.0001  &&  coef[2] < 0.0001) {
            y1 = 0.0;
            y2 = 0.0;
        }
        else {
            y1 = 0.0;
            y2 = (z100 - coef[0]) / coef[2];
        }

        dx = x2 - x1;
        dy = y2 - y1;

    /*
     * If the contour is still zero length, the plane is essentially
     * horizontal and no rotation is needed.
     */
        if (NearZeroDist (dx)  &&  NearZeroDist (dy)) {
            return 1;
        }
    }

    zang = atan2 (dy, dx);
    if (zang < -halfpi) zang += pi;
    if (zang > halfpi) zang -= pi;

    dipang = zang + halfpi;
    x3 = x1 + cos (dipang);
    y3 = y1 + sin (dipang);
    dx = x3 - x1;
    dy = y3 - y1;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    z1 = coef[0] + coef[1] * x1 + coef[2] * y1;
    z2 = coef[0] + coef[1] * x3 + coef[2] * y3;
    dz = z2 - z1;
    xang = atan2 (dz, dist);

    xang = -xang;
    zang = -zang;

    xcos = cos (xang);
    zcos = cos (zang);
    xsin = sin (xang);
    zsin = sin (zang);

    for (i=0; i<npts; i++) {
        nodes[i].x -= xorigin;
        nodes[i].y -= yorigin;
        nodes[i].z -= zorigin;
    }

    for (i=0; i<npts; i++) {
        xt = nodes[i].x * zcos - nodes[i].y * zsin;
        yt = nodes[i].x * zsin + nodes[i].y * zcos;
        zt = nodes[i].z;
        xt2 = xt;
        yt2 = yt * xcos - zt * xsin;
        zt2 = yt * xsin + zt * xcos;
        nodes[i].x = xt2;
        nodes[i].y = yt2;
        nodes[i].z = zt2;
    }

    return 1;

}


/*-----------------------------------------------------------------------*/

int SurfaceGroupPlane::convertFromPlane
    (NOdeStruct     *nodes,
     int            npts)
{
    int        i;
    double     xt, yt, zt, xt2, yt2, z100, zt1, zt2;
    double     xang, zang, xcos, xsin, zcos, zsin;
    double     x1, y1, z1, x2, y2, z2, dx, dy, dz;
    double     pi = 3.1415926;
    double     halfpi = 3.1415926 / 2.0;
    double     dipang, x3, y3, dist;

    if (plane_fit_flag == 999  ||
        fault_flag != 1) {
        return 1;
    }

/*
 * Check for obvious errors.
 */
    if (nodes == NULL) {
        return 1;
    }

    if (npts < 1) {
        return 1;
    }

/*
 * Find the points where the z = 0 contour crosses the
 * x and y axes respectively.  These points define the
 * strike of the plane.
 */
    if (coef[1] > -0.0001  &&  coef[1] < 0.0001) {
        x1 = 0.0;
        x2 = 0.0;
        if (coef[2] < -.0001 ||  coef[2] > .0001) {
            x2 = 10000.0;
        }
    }
    else {
        x1 = coef[0] / coef[1];
        x2 = 0.0;
    }

    if (coef[2] > -0.0001  &&  coef[2] < 0.0001) {
        y1 = 0.0;
        y2 = 0.0;
        if (coef[1] < -.0001 ||  coef[1] > .0001) {
            y2 = 10000.0;
        }
    }
    else {
        y1 = 0.0;
        y2 = coef[0] / coef[2];
    }

    dx = x2 - x1;
    dy = y2 - y1;

/*
 * It is possible that the z = 0 contour crosses exactly or very nearly
 * exactly through the origin, and the strike angle has no meaning.
 * If this is the case, try the z = 1 contour.
 */
    if (NearZeroDist (dx)  &&  NearZeroDist (dy)) {

        zt1 = coef[1];
        zt2 = coef[2];
        if (zt1 < 0.0) zt1 = -zt1;
        if (zt2 < 0.0) zt2 = -zt2;
        z100 = (zt1 + zt2) / 2.0;

        if (coef[1] > -0.0001  &&  coef[1] < 0.0001) {
            x1 = 0.0;
            x2 = 0.0;
        }
        else {
            x1 = (z100 - coef[0]) / coef[1];
            x2 = 0.0;
        }

        if (coef[2] > -0.0001  &&  coef[2] < 0.0001) {
            y1 = 0.0;
            y2 = 0.0;
        }
        else {
            y1 = 0.0;
            y2 = (z100 - coef[0]) / coef[2];
        }

        dx = x2 - x1;
        dy = y2 - y1;

    /*
     * If the contour is still zero length, the plane is essentially
     * horizontal and no rotation is needed.
     */
        if (NearZeroDist (dx)  &&  NearZeroDist (dy)) {
            return 1;
        }
    }

    zang = atan2 (dy, dx);
    if (zang < -halfpi) zang += pi;
    if (zang > halfpi) zang -= pi;

    dipang = zang + halfpi;
    x3 = x1 + cos (dipang);
    y3 = y1 + sin (dipang);
    dx = x3 - x1;
    dy = y3 - y1;
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    z1 = coef[0] + coef[1] * x1 + coef[2] * y1;
    z2 = coef[0] + coef[1] * x3 + coef[2] * y3;
    dz = z2 - z1;
    xang = atan2 (dz, dist);

    xcos = cos (xang);
    zcos = cos (zang);
    xsin = sin (xang);
    zsin = sin (zang);

    for (i=0; i<npts; i++) {
        xt = nodes[i].x;
        yt = nodes[i].y * xcos - nodes[i].z * xsin;
        zt = nodes[i].y * xsin + nodes[i].z * xcos;
        xt2 = xt * zcos - yt * zsin;
        yt2 = xt * zsin + yt * zcos;
        zt2 = zt;
        nodes[i].x = xt2;
        nodes[i].y = yt2;
        nodes[i].z = zt2;
    }

    for (i=0; i<npts; i++) {
        nodes[i].x += xorigin;
        nodes[i].y += yorigin;
        nodes[i].z += zorigin;
    }

    return 1;

}



/*-----------------------------------------------------------------------*/

/*
 * Calculate the coefficents of a plane that is best for projecting
 * the surface points to.  If there are less than 3 surface points
 * currently defined, this method returns -1.  On success, 1 is returned.
 */
int SurfaceGroupPlane::calcPlaneCoefs (void)
{
    double      xmid, ymid, zmid;
    double      zscale;

    if (numfit < 3) {
        return -1;
    }

    if (xfit == NULL  ||
        yfit == NULL  ||
        zfit == NULL) {
        return -1;
    }

/*
 * Find the bounding box of the fit data.
 */
    double       xmin, ymin, zmin, xmax, ymax, zmax;
    int          i;

    xmin = ymin = zmin = 1.e30;
    xmax = ymax = zmax = -1.e30;

    for (i=0; i<numfit; i++) {
        if (xfit[i] < xmin) xmin = xfit[i];
        if (yfit[i] < ymin) ymin = yfit[i];
        if (zfit[i] < zmin) zmin = zfit[i];
        if (xfit[i] > xmax) xmax = xfit[i];
        if (yfit[i] > ymax) ymax = yfit[i];
        if (zfit[i] > zmax) zmax = zfit[i];
    }

    if (xmin >= xmax && ymin >= ymax) {
        return 0;
    }

    if (zmax <= zmin) {
        zscale = 1.0;
    }
    else {
        zscale = (xmax - xmin + ymax - ymin) / (zmax - zmin) * 2.0;
    }

/*
 * If this is definately not a fault, do not scale the z value.
 */
    if (fault_flag == 0) {
        zscale = 1.0;
    }

/*
 * If the fault flag has not been set, use a scaling that
 * exagerates the z value range to the average of the x and y ranges.
 */
    if (fault_flag == -1) {
        zscale /= 4.0;
    }

    if (zscale < 1.0) zscale = 1.0;

    zerodist = (xmax - xmin + ymax - ymin) / 20000.0;

/*
 * Find the mid point of each coordinate range.  The points
 * are then shifted to this origin to make the numbers smaller
 * and therefore improve precision in the plane fitting.
 */
    xmid = (xmin + xmax) / 2.0;
    ymid = (ymin + ymax) / 2.0;
    zmid = (zmin + zmax) / 2.0;

    for (i=0; i<numfit; i++) {
        xfit[i] -= xmid;
        yfit[i] -= ymid;
        zfit[i] -= zmid;
        zfit[i] *= zscale;
    }

/*
 * Fit a plane through the shifted points.
 */
    double     ctmp[3];
    int        istat;
    istat =
      grd_tsurf_ptr->grd_calc_perp_plane_fit (xfit, yfit, zfit, numfit, ctmp);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<numfit; i++) {
        zfit[i] /= zscale;
        xfit[i] += xmid;
        yfit[i] += ymid;
        zfit[i] += zmid;
    }

    memcpy (coef, ctmp, 3 * sizeof(double));
    coef[0] /= zscale;
    coef[1] /= zscale;
    coef[2] /= zscale;

    int do_write = csw_GetDoWrite ();
    if (do_write) {
        OutputSVDResults (xfit, yfit, zfit, numfit,
                          coef, (char *)"sgp.out");
    }

/*
 * Shift the origin so the plane goes through it.  Translate by the
 * x or y axis intercept, whichever is less, to accomplish this.
 */
    double        xint, yint, axint, ayint;

    xint = 1.e30;
    yint = 1.e30;

    if (coef[2] < -0.0001  ||  coef[2] > .0001) {
        yint = -coef[0] / coef[2];
    }
    if (coef[1] < -0.0001  ||  coef[1] > .0001) {
        xint = -coef[0] / coef[1];
    }

    horizontal_flag = 0;
    if (xint > 1.e20  &&  yint > 1.e20) {
        horizontal_flag = 1;
        xorigin = xmid;
        yorigin = ymid;
        zorigin = zmid;
        return 1;
    }

    axint = xint;
    if (xint < 0.0) axint = -xint;
    ayint = yint;
    if (yint < 0.0) ayint = -yint;
    if (axint < ayint) {
        xorigin = xmid + xint;
        yorigin = ymid;
        zorigin = zmid;
    }
    else {
        xorigin = xmid;
        yorigin = ymid + yint;
        zorigin = zmid;
    }

    coef_flag = 1;

    return 1;

}


/*--------------------------------------------------------------------------------*/

/*
 * Calculate the normal vector to the specified triangle.
 * The normal will always have zero or positive z.  The
 * normal is scaled so that the length is 1.0.
 *
 * The components of the normal are put into the private
 * instance variables tnxNorm, tnyNorm and tnzNorm.
 */
void SurfaceGroupPlane::TriangleNormal (double *x, double *y, double *z)
{
    double    x1, y1t, z1, x2, y2, z2,
              px, py, pz;
    double    dist;

    x1 = x[1] - x[0];
    y1t = y[1] - y[0];
    z1 = z[1] - z[0];
    x2 = x[2] - x[0];
    y2 = y[2] - y[0];
    z2 = z[2] - z[0];

    px = y1t * z2 - z1 * y2;
    py = z1 * x2 - x1 * z2;
    pz = x1 * y2 - y1t * x2;

  /*
   * If the z component of the normal points down, the triangle
   * is clockwise when viewed from above, and it needs to be
   * changed to ccw.
   */
    if (pz < 0.0) {
        px = -px;
        py = -py;
        pz = -pz;
    }

    dist = px * px + py * py + pz * pz;
    dist = sqrt (dist);

    if (dist <= 1.e-10) {
        tnxNorm = 0.0;
        tnyNorm = 0.0;
        tnzNorm = 1.0;
    }
    else {
        tnxNorm = px / dist;
        tnyNorm = py / dist;
        tnzNorm = pz / dist;
    }

    return;

}



/*------------------------------------------------------------------------------*/

/*
 * Find the average normals of the triangles specified and add them to
 * the avxNorm, avyNorm and avzNorm values.  Increment the numNorm value
 * as well.
 */

void SurfaceGroupPlane::UpdateAverageNormals (
    NOdeStruct    *nodes,
    EDgeStruct    *edges,
    TRiangleStruct *tris,
    int           numtris)
{
    double        tx[3], ty[3], tz[3];
    int           i, n, ndo, istat;
    int           n1, n2, n3;
    double        max_z_normal = _MAX_Z_NORMAL_;
    double        xsum, ysum, zsum;
    TRiangleStruct  *tptr;

    for (ndo=0; ndo<10; ndo++) {

        xsum = 0.0;
        ysum = 0.0;
        zsum = 0.0;
        n = 0;
        for (i=0; i<numtris; i++) {
            tptr = tris + i;
            istat =
              grd_triangle_ptr->grd_get_nodes_for_triangle (
                tptr, edges,
                &n1, &n2, &n3);
            if (istat != 1) {
                continue;
            }
            tx[0] = nodes[n1].x;
            ty[0] = nodes[n1].y;
            tz[0] = nodes[n1].z;
            tx[1] = nodes[n2].x;
            ty[1] = nodes[n2].y;
            tz[1] = nodes[n2].z;
            tx[2] = nodes[n3].x;
            ty[2] = nodes[n3].y;
            tz[2] = nodes[n3].z;
            TriangleNormal (tx, ty, tz);
            if (tnzNorm > max_z_normal) {
                continue;
            }
            xsum += tnxNorm;
            ysum += tnyNorm;
            zsum += tnzNorm;
            n++;
        }

        if (n < 3) {
            max_z_normal *= 1.05;
            if (max_z_normal > .95) max_z_normal = .95;
            continue;
        }

        break;

    }

    if (n < 1) {
        return;
    }

    xsum /= n;
    ysum /= n;
    zsum /= n;

    avxNorm += xsum;
    avyNorm += ysum;
    avzNorm += zsum;

    numNorm++;

    return;

}



/*----------------------------------------------------------------------*/

int SurfaceGroupPlane::NearZeroDist (double val)
{
    if (zerodist < 1.e20) {
        if (val >= -zerodist  &&  val <= zerodist) {
            return 1;
        }
    }
    else {
        if (val >= -0.001  &&  val <= 0.001) {
            return 1;
        }
    }

    return 0;
}


void SurfaceGroupPlane::OutputSVDResults (
        double *x, double *y, double *z, int npts,
        double *coef, char *fname)
{
    double w[3], v[9], pn[3];
    int    i, nout;
    char   line[200];
    FILE   *fptr;

    fptr = fopen (fname, "wb");
    if (fptr == NULL) {
        return;
    }

    grd_tsurf_ptr->grd_get_saved_svd_results (w, v, pn, 3);

    nout = npts / 75;
    if (nout < 1) nout = 1;

    for (i=0; i<npts; i+=nout) {
        sprintf (line, "%.3f %.3f %.3f\n", x[i], y[i], z[i]);
        fputs (line, fptr);
    }

    sprintf (line, "Singular values: %.4f %.4f %.4f\n",
             w[0], w[1], w[2]);
    fputs (line, fptr);

    sprintf (line, "Singular Vector Matrix\n");
    fputs (line, fptr);

    sprintf (line, "%.4f %.4f %.4f\n", v[0], v[1], v[2]);
    fputs (line, fptr);
    sprintf (line, "%.4f %.4f %.4f\n", v[3], v[4], v[5]);
    fputs (line, fptr);
    sprintf (line, "%.4f %.4f %.4f\n", v[6], v[7], v[8]);
    fputs (line, fptr);

    sprintf (line, "Plane normals: %.4f %.4f %.4f\n",
             pn[0], pn[1], pn[2]);
    fputs (line, fptr);

    sprintf (line, "Plane coefs: %.4f %.4f %.4f\n",
              coef[0], coef[1], coef[2]);
    fputs (line, fptr);

    fclose (fptr);

    return;

}
