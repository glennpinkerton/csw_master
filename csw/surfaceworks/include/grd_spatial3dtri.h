
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * Define the interface for the Spatial3DTriangleIndex class.  The
 * body of the class is in grd_spatial3dtri.cc, in the ..\src
 * directory.
 */

#ifndef GRD_SPATIAL3DTRI_H
#define GRD_SPATIAL3DTRI_H

#include "csw/surfaceworks/include/grd_shared_structs.h"

typedef struct {
    int tmeshid;
    int trinum;
    char used;
}  SPatial3DTriangleStruct;

typedef struct {
    SPatial3DTriangleStruct *list;
    int                     nlist;
} SPatial3DTriangleStructList;


class Spatial3DTriangleIndex;

#include "csw/surfaceworks/private_include/grd_triangle_class.h"

class Spatial3DTriangleIndex
{

  private:

    CSWGrdTriangle  *grd_triangle_ptr = NULL;


  public:

    Spatial3DTriangleIndex (double xmin,
                            double ymin,
                            double zmin,
                            double xmax,
                            double ymax,
                            double zmax);

    virtual ~Spatial3DTriangleIndex ();

// Objects of this class not meant to be copied or moved.

    Spatial3DTriangleIndex (const Spatial3DTriangleIndex &other) = delete;
    const Spatial3DTriangleIndex &operator=
        (const Spatial3DTriangleIndex &other) = delete;
    Spatial3DTriangleIndex (Spatial3DTriangleIndex &&other) = delete;
    const Spatial3DTriangleIndex &operator=
        (Spatial3DTriangleIndex &&other) = delete;


    void  SetGrdTrianglePtr (CSWGrdTriangle *p) {grd_triangle_ptr = p;};


    int SetGeometry (double xmin,
                     double ymin,
                     double zmin,
                     double xmax,
                     double ymax,
                     double zmax,
                     double xspace,
                     double yspace,
                     double zspace);

    int AddTriMesh (int trimeshid,
                    NOdeStruct *nodes,
                    EDgeStruct *edges,
                    TRiangleStruct *triangles, int numtriangles);

    void Clear (void);

    SPatial3DTriangleStructList *
      GetTriangles (
        int tmeshid,
        double xmin,
        double ymin,
        double zmin,
        double xmax,
        double ymax,
        double zmax);


  private:

  /*
   * Private methods.
   */
    int AddTriangle (double *x,
                     double *y,
                     double *z,
                     int spatialTriangleNumber);

    int AddToIndexGrid (int index,
                        int spatialTriangleNumber);

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

    SPatial3DTriangleStruct
                  *TriangleList;
    int           NumTriangleList,
                  MaxTriangleList;

    int           geometryAllowed;

};


#endif
