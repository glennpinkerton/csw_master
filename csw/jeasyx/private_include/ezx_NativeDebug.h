
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#ifndef _EZX_NATIVE_DEBUG_H_
#define _EZX_NATIVE_DEBUG_H_

#include <stdio.h>

namespace NativeDebug {

void SimplePolyInt ();
int poly_intersect (double *xs, double *ys, int *isc, int *vs, int nsc,
                    double *xc, double *yc, int *icc, int *vc, int ncc);

} // end of NativeDebug namespace

#endif // end of header file
