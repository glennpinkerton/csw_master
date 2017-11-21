/*
 ***************************************************************************

  This program exercises the perpendicular fit to plane algorithm.

 ***************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>

#include <csw/surfaceworks/include/grid_api.h>
#include <csw/surfaceworks/private_include/grd_apiP.h>
#include <csw/utils/include/csw_.h>

static void ProcessPoints
  (double *x, double *y, double *z, int npts, FILE *fpout);

static int SetNum = 0;

int main (int argc, char *argv[])
{
    FILE       *fpin, *fpout;
    char       line[200], *ctmp;
    int        npts, n, endfile;
    static double    x[1000], y[1000], z[1000];

    argc = argc;
    argv = argv;

    fpin = fopen ((char *)"fptest.dat", "rb");
    if (fpin == NULL) {
        printf ("Input file cannot be opened\n");
        return 1;
    }

    fpout = fopen ((char *)"fptest.out", "wb");
    if (fpout == NULL) {
        printf ("Output file cannot be opened\n");
        fclose (fpin);
        return 1;
    }
        
    SetNum = 0;
    for (;;) {
        
        ctmp = csw_fgets (line, 200, fpin);
        if (ctmp == NULL) {
            break;
        }
        if (line[0] == '\0') continue;

        sscanf (line, "%d", &npts);
        
        n = 0;
        endfile = 0;
        for (;;) {
            if (n == npts) break;
            ctmp = csw_fgets (line, 200, fpin);
            if (ctmp == NULL) {
                endfile = 1;
                break;
            }
            if (line[0] == '\0') {
                continue;
            }
            sscanf (line, "%lf %lf %lf", x+n, y+n, z+n);
            n++;
        }

        if (endfile == 1) {
            break;
        }

        SetNum++;
        ProcessPoints (x, y, z, npts, fpout);

    }
            
    fclose (fpin);
    fclose (fpout);

    return 1;

}



static void ProcessPoints (
    double *x, double *y, double *z, int npts, FILE *fptr)
{
    double coef[3], w[3], v[9], pn[3];
    int    istat, i, nout;
    char   line[200];

    istat =
        grd_CalcPerpPlaneFit (x, y, z, npts, coef);
    if (istat == -1) {
        return;
    }

    grd_get_saved_svd_results (w, v, pn, 3);

    sprintf (line, "\nResults for set number %d\n", SetNum);
    fputs (line, fptr);
    
    nout = npts;
    if (nout > 40) nout = 40;

    for (i=0; i<nout; i++) {
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

    fflush (fptr);

    return;

}


    
