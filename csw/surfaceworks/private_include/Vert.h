
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#ifndef _VERT_H_
#define _VERT_H_


#include  "csw/surfaceworks/src/SurfaceGroupPlane.h"


class GRDVert
{
  public:

    SurfaceGroupPlane    sgp;
    int                  useflag = 0;
};


/*
    If a C++ compiler is running, all of the prototypes
    must be declared extern "C".
*/
# if defined(__cplusplus)  ||  defined(c_plusplus)
extern "C"
{
# endif

#include <csw/surfaceworks/include/grid_api.h>


int vert_CalcBaselineFromJava (
    double       *xpts,
    double       *ypts,
    double       *zpts,
    int          npts,
    int          fault_flag
);

int vert_CalcBaseline (
    double       *xpts,
    double       *ypts,
    double       *zpts,
    int          npts,
    int          fault_flag,
    double       *c1_out,
    double       *c2_out,
    double       *c3_out,
    double       *x0_out,
    double       *y0_out,
    double       *z0_out
);

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
);

void vert_SetBaseline (
    double       c1,
    double       c2,
    double       c3,
    double       x0,
    double       y0,
    double       z0,
    int          use,
    GRDVert      &gvert
);

void vert_SendBackBaseline (GRDVert &gvert);

int vert_GetFlag (GRDVert &gvert);

void vert_GetBaseline (
    double     *c1,
    double     *c2,
    double     *c3,
    double     *x0,
    double     *y0,
    double     *z0,
    int        *use,
    GRDVert    &gvert
);

void vert_UnsetBaseline (void);

int vert_ConvertPoints (
    double       *xpts,
    double       *ypts,
    double       *zpts,
    int          npts,
    GRDVert      &gvert
);

int vert_UnconvertPoints (
    double       *xpts,
    double       *ypts,
    double       *zpts,
    int          npts,
    GRDVert      &gvert
);

int vert_ConvertTriMeshNodes (
    NOdeStruct   *nodes,
    int          num_nodes,
    GRDVert      &gvert
);

int vert_UnconvertTriMeshNodes (
    NOdeStruct   *nodes,
    int          num_nodes,
    GRDVert      &gvert
);

/*
  The extern "C" block needs to be closed if this
  is being run through a C++ compiler.
*/
# if defined (__cplusplus)  ||  defined (c_plusplus)
}
# endif

#endif
