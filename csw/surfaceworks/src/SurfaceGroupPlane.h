
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * This class supports the transformation of a group of points belonging
 * to a group of surfaces into a coordinate system based on a plane that
 * fits the surface "best".  Best is defined as the plane that will produce
 * a surface without both near horizontal and near vertical areas.
 *
 * The general idea for using this class is to first create an empty
 * instance.  Then, add any points and trimeshes that you want grouped
 * into a set of related surfaces.  When all of these have been added,
 * call the calcPlaneCoefs method.  This will return the coefs and also
 * keep them in the object for future use.  Finally, you can at any time
 * after the coefs have been calculated convert to the plane system or
 * from the plane system.  Once the coefs have been calculated you can
 * also free the local copies of trimesh and point data to save memory.
 */

#ifndef _SURFACE_GROUP_PLANE_H_
#define _SURFACE_GROUP_PLANE_H_

/*
 * System includes.
 */
#include <stdlib.h>

class SurfaceGroupPlane;

/*
 * CSW includes.
 */
#include "csw/surfaceworks/private_include/grd_triangle_class.h"
#include "csw/surfaceworks/private_include/grd_tsurf.h"

class SurfaceGroupPlane {

  private:

    CSWGrdTriangle  *grd_triangle_ptr = NULL;
    CSWGrdTsurf     *grd_tsurf_ptr = NULL;

  public:

    SurfaceGroupPlane ();
    SurfaceGroupPlane (SurfaceGroupPlane *sgp);
    virtual ~SurfaceGroupPlane ();

// Do not allow copy or move

  private:

    SurfaceGroupPlane (SurfaceGroupPlane const &src) {};
    SurfaceGroupPlane const &operator=
        (SurfaceGroupPlane const &src) {return *this;};
    SurfaceGroupPlane (SurfaceGroupPlane const &&src) {};
    SurfaceGroupPlane const &operator=
        (SurfaceGroupPlane const &&src) {return *this;};

  public:

    void  SetGrdTrianglePtr (CSWGrdTriangle *p) {grd_triangle_ptr = p;};
    void  SetGrdTsurfPtr (CSWGrdTsurf *p) {grd_tsurf_ptr = p;};

    int addTriMeshForFit (
        NOdeStruct      *nodes,
        int             numnodes,
        EDgeStruct      *edges,
        int             numedges,
        TRiangleStruct  *triangles,
        int             numtriangles);

    int addPointsForFit (
        double          *x,
        double          *y,
        double          *z,
        int             npts);

    void freeFitPoints (void);

    int convertToPlane (
        NOdeStruct      *nodes,
        int             numnodes);

    int convertToPlane (
        double          *x,
        double          *y,
        double          *z,
        int             npts);

    int convertFromPlane (
        NOdeStruct      *nodes,
        int             numnodes);

    int convertFromPlane (
        double          *x,
        double          *y,
        double          *z,
        int             npts);

    int calcPlaneCoefs (double *coef);
    int calcPlaneCoefs (void);

    void setPlaneCoefs (double *coefs);
    int getPlaneCoefs (double *coefs);

    void setOrigin (double x, double y, double z);
    int  getOrigin (double *x, double *y, double *z);

    void setCoefsAndOrigin (
        double    c1,
        double    c2,
        double    c3,
        double    x0,
        double    y0,
        double    z0);
    int getCoefsAndOrigin (
        double    *c1,
        double    *c2,
        double    *c3,
        double    *x0,
        double    *y0,
        double    *z0);

    int getHorizontalFlag (void) {return horizontal_flag;};

    int getPlaneFitFlag (void);


    void setFaultFlag (int ival);

  private:

    double              *xfit,
                        *yfit,
                        *zfit;
    int                 numfit,
                        maxfit;

    double              coef[3];
    int                 coef_flag;

    double              xorigin,
                        yorigin,
                        zorigin;
    int                 horizontal_flag;

    double              tnxNorm,
                        tnyNorm,
                        tnzNorm;

    double              avxNorm,
                        avyNorm,
                        avzNorm;
    int                 numNorm;

    double              zerodist;
    double              zrange;

    int                 plane_fit_flag;

    int                 fault_flag;

    void init (void);
    void free_mem (void);

    int  add_fit_point (double x, double y, double z);

    void copy_from_pointer (SurfaceGroupPlane * sptr);
    void copy_from_reference (SurfaceGroupPlane const & src);

    void TriangleNormal (double *tx, double *ty, double *tz);
    void UpdateAverageNormals (
        NOdeStruct    *nodes,
        EDgeStruct    *edges,
        TRiangleStruct *tris,
        int           numtris);
    void CalcAverageNormalsFromPoints (
        double *xmid, double *ymid, double *zmid);
    int CalcCoefsFromNormal (double *xmid, double *ymid, double *zmid);

    int NearZeroDist (double val);

    void OutputSVDResults (
        double *x, double *y, double *z, int npts,
        double *coef, char *fname);

};

#endif
