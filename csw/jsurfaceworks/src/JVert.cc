
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#include <math.h>
#include <stdlib.h>

#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

#include <csw/jsurfaceworks/private_include/SurfaceWorksJNI.h>
#include <csw/surfaceworks/private_include/Vert.h>

/*----------------------------------------------------------------------------*/

/*
 * The functions that refer to jni methods are isolated here and removed
 * from the Vert.cc file to allow linking of Vert.cc with non java platforms.
 */

/*
 * Calculate the baseline and send the results back to the Java side.
 */
int vert_CalcBaselineFromJava (
    double    *xpts,
    double    *ypts,
    double    *zpts,
    int       npts,
    int       fault_flag
)
{
    double    c1, c2, c3, x0, y0, z0;
    int       istat;

    c1 = c2 = c3 = x0 = y0 = z0 = 0.0;

    istat = vert_CalcBaseline (
        xpts,
        ypts,
        zpts,
        npts,
        fault_flag,
        &c1,
        &c2,
        &c3,
        &x0,
        &y0,
        &z0
    );
    if (istat == -1) {
        return -1;
    }

    jni_call_set_vert_baseline_method (
        c1, c2, c3, x0, y0, z0, istat);

    return 1;

}


void vert_SendBackBaseline (GRDVert &gvert)
{
    double         c1, c2, c3, x0, y0, z0;
    int            use;

    c1 = c2 = c3 = x0 = y0 = z0 = 0.0;
    use = 0;

    vert_GetBaseline (&c1, &c2, &c3, &x0, &y0, &z0, &use, gvert);

    jni_call_set_vert_baseline_method (
        c1, c2, c3, x0, y0, z0, use);

    return;

}


