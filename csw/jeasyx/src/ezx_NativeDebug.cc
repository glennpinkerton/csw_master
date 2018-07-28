#include <stdio.h>

#include "csw/jeasyx/private_include/ezx_NativeDebug.h"

#include "csw/utils/private_include/ply_graph.h"


void NativeDebug::SimplePolyInt () {
    double    xs[50], ys[50], xc[50], yc[50];
    int       is[10], ic[10], ns, nc, vs[10], vc[10];

    ns = 1;
    nc = 1;

    is[0] = 1;
    ic[0] = 1;
    vs[0] = 5;
    vc[0] = 5;

    xs[0] = 1.0;
    xs[1] = 1.0;
    xs[2] = 2.0;
    xs[3] = 2.0;
    xs[4] = 1.0;
    ys[0] = 0.0;
    ys[1] = 3.0;
    ys[2] = 3.0;
    ys[3] = 0.0;
    ys[4] = 0.0;

    xc[0] = 0.0;
    xc[1] = 0.0;
    xc[2] = 3.0;
    xc[3] = 3.0;
    xc[4] = 0.0;
    yc[0] = 1.0;
    yc[1] = 2.0;
    yc[2] = 2.0;
    yc[3] = 1.0;
    yc[4] = 1.0;

    NativeDebug::poly_intersect (xs, ys, is, vs, ns,
                                 xc, yc, ic, vc, nc);

    xs[2] = 5.0;
    xs[3] = 5.0;
    
    NativeDebug::poly_intersect (xs, ys, is, vs, ns,
                                 xc, yc, ic, vc, nc);

    xs[0] = -2;
    xs[1] = -2;
    xs[4] = -2;

    NativeDebug::poly_intersect (xs, ys, is, vs, ns,
                                 xc, yc, ic, vc, nc);

    return;
}

int NativeDebug::poly_intersect
                   (double *xs, double *ys, int *isc, int *isv, int nsc,
                    double *xc, double *yc, int *icc, int *icv, int ncc)
{
    CSWPolyGraph   pcalc;

    double         xout[1000], yout[1000];
    int            icout[1000], ihout[1000], npout;
    void           *tag1[1000], *tag2[1000], *tagout[1000];

    for (int i=0; i<1000; i++) {
        tag1[i] = NULL;
        tag2[i] = NULL;
        ihout[i] = 0;
        icout[i] = 0;
    }

    int   istat = pcalc.ply_boolean (
      xs, ys, tag1, nsc, isc, isv,
      xc, yc, tag2, ncc, icc, icv,
      PLY_GRAPH_INTERSECT,
      xout, yout, tagout, &npout, icout, ihout,
      100, 100);

printf ("\nistat from CalcIntersect = %d\n", istat);
printf ("    npout = %d    icout = %d    ihout = %d\n",
  npout, icout[0], ihout[0]);
printf ("\n");

int    i, j, n1;

printf ("Source points:\n");
n1 = 0;
for (i=0; i<nsc; i++) {
  for (j=0; j<isv[i]; j++) {
    printf ("  %.2f, %.2f\n", xs[n1], ys[n1]);
    n1++;
  }
  printf ("\n");
}

printf ("Clip points:\n");
n1 = 0;
for (i=0; i<ncc; i++) {
  for (j=0; j<icv[i]; j++) {
    printf ("  %.2f, %.2f\n", xc[n1], yc[n1]);
    n1++;
  }
  printf ("\n");
}

printf ("Intersection points:\n");
n1 = 0;
for (i=0; i<npout; i++) {
  for (j=0; j<ihout[i]; j++) {
    printf ("  %.2f, %.2f\n", xout[n1], yout[n1]);
    n1++;
  }
  printf ("\n");
}

    return istat;

}



