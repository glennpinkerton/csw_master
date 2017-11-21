
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * Define the interface for the XYZIndex3D class.  The
 * body of the class is in grd_xyzindex.cc, in the ..\src
 * directory.
 */

#ifndef GRD_XYZINDEX3D_H
#define GRD_XYZINDEX3D_H

#include "csw/surfaceworks/include/grd_shared_structs.h"


class XYZIndex3D
{

  public:

    XYZIndex3D (double xmin,
                double ymin,
                double zmin,
                double xmax,
                double ymax,
                double zmax);

    virtual ~XYZIndex3D ();

    int SetGeometry (double xmin,
                     double ymin,
                     double zmin,
                     double xmax,
                     double ymax,
                     double zmax,
                     double xspace,
                     double yspace,
                     double zspace);

    int AddTriMeshNodes
                    (NOdeStruct *nodes,
                    int         numnodes);

    int AddPoint (double x, double y, double z);

    int AddPoints (double *x, double *y, double *z, int npts);

    void Clear (void);

    int GetClosePoints (
        double x,
        double y,
        double z,
        int    *list,
        int    *nlist,
        int    maxlist);

    int GetPointXYZ (int pointnum, double *x, double *y, double *z);

  private:

  /*
   * Private methods.
   */
    int AddToIndexGrid (int index,
                        int pointNumber);

    int CreateIndexGrid (void);

  /*
   * Private data members.
   */
    int           **IndexGrid;
    int           IndexNcol,
                  IndexNrow,
                  IndexNlevel;
    double        IndexXmin,
                  IndexYmin,
                  IndexZmin,
                  IndexXspace,
                  IndexYspace,
                  IndexZspace,
                  IndexXmax,
                  IndexYmax,
                  IndexZmax;

    double        *xPointList,
                  *yPointList,
                  *zPointList;
    int           NumPointList,
                  MaxPointList;

    int           geometryAllowed;

};


#endif
