
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_tsurf.cc

    This file has functions used to generate and evaluate trend
    surface fits to x, y, z data points.

      The following public functions are provided in this file.

            grd_calc_trend_surface
                Calculate the coefficients of a trend surface through
                a set of x, y, z points.

            grd_eval_trend_surface
                Evaluate a trend surface (given its coefficients) at
                an array of x, y points.

            grd_calc_trend_grid
                Return a rectangular grid of a trend surface through
                an array of x, y, z points.

            grd_calc_perp_plane_fit
                Return the coefficients of a plane that fits the points
                by minimizing the perpendicular distance to the plane.

    These are the only global functions in this file.  The rest of the
    functions are private class functions used in the internal calculation of
    the surface.

    An application should not call these functions directly.  Call the
    functions located in grd_api.c instead of these functions.  These functions
    may be put into a separate server process in the future.
*/

#include <float.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"
#include "csw/utils/private_include/simulP.h"
#include "csw/utils/private_include/gpf_utils.h"

#include "csw/surfaceworks/include/grd_shared_structs.h"
#include "csw/surfaceworks/private_include/grd_tsurf.h"


void CSWGrdTsurf::grd_get_saved_svd_results (
    double *w, double *v, double *pn, int size)
{
    if (size < 3) {
        return;
    }
    if (size > 10) {
        return;
    }

    memcpy (w, Svd_w_save, size * sizeof(double));
    memcpy (v, Svd_v_save, size * size * sizeof(double));
    memcpy (pn, Svd_pn_save, size * sizeof(double));

    return;
}





/*
  ****************************************************************

           g r d _ c a l c _ t r e n d _ s u r f a c e

  ****************************************************************

  function name:  grd_calc_trend_surface        (int)

  call sequence:  grd_calc_trend_surface (x, y, z, npts, iorder,
                                          coef)

  purpose:        Calculate the coefficients for an iorder trend surface
                  with a least squares best fit through the specified
                  points.

  return value:   1 = success
                 -1 = failure

  errors:         1 = error allocating workspace
                  2 = too few points for the order or
                      the order is out of the 1-8 range
                  3 = the data cannot be fit, probably
                      because they are colinear or all
                      in the same location


  calling parameters:

    x       CSW_F*    r       array of x coordinates
    y       CSW_F*    r       array of y coordinates
    z       CSW_F*    r       array of z coordinates
    npts    int           r       number of points in the arrays
    iorder  int           r       order of the polynomial to calculate
                                  this must be 1 - 8
    coef    CSW_F*    w       returned coefficients

        Note:  You must supply a coef array large enough for the
               number of coefficients returned.  For the highest
               order polynomial supported, 45 coefficients are
               returned.

*/

int CSWGrdTsurf::grd_calc_trend_surface (CSW_F *x, CSW_F *y, CSW_F *z, int npts,
                            int iorder, CSW_F *coef)
{
    double       *dx = NULL, *dy = NULL, *dz = NULL, dcoef[MAX_COEFS];
    int          i, istat, nc;


    auto fscope = [&]()
    {
        csw_Free (dx);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    check obvious parameter errors
*/
    if (x == NULL  ||  y == NULL  ||
        z == NULL  ||  coef == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (iorder < 1  ||  iorder > 8) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    i = (iorder +1) * (iorder + 2) / 2;
    if (npts < i) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    allocate workspace
*/
MSL
    dx = (double *)csw_Malloc (npts * 3 * sizeof(double));
    if (!dx) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    dy = dx + npts;
    dz = dy + npts;

/*
    convert input data to double precision
*/
    for (i=0; i<npts; i++) {
        dx[i] = (double)x[i];
        dy[i] = (double)y[i];
        dz[i] = (double)z[i];
    }

/*
    Calculate the coefficients.
*/
    istat = SurfaceFit (dx, dy, dz, npts, iorder,
                        dcoef, &nc);
    if (istat == -1) {
        return -1;
    }

/*
    Convert coefficients back to CSW_F and return success.
*/
    for (i=0; i<nc; i++) {
        if (dcoef[i] < Z_ABSOLUTE_TINY  &&
            dcoef[i] > -Z_ABSOLUTE_TINY) {
            dcoef[i] = 0.0;
        }
        coef[i] = (CSW_F)dcoef[i];
    }

    return 1;

}  /*  end of function grd_calc_trend_surface  */







/*
  ****************************************************************

             g r d _ e v a l _ t r e n d _ s u r f a c e

  ****************************************************************

  function name:  grd_eval_trend_surface            (int)

  call sequence:  grd_eval_trend_surface (x, y, z, npts, iorder, coef)

  purpose:        Evaluate the specified trend surface at the set of
                  x and y points.  The trend surface coefficients
                  should have been calculated by grd_calc_trend_surface.

  return value:   1 = success
                 -1 = error

  errors:        1        memory allocation failure
                 3        order must be 1-8

  calling parameters:

    x        CSW_F* r        Array of x coordinates.
    y        CSW_F* r        Array of y coordinates.
    z        CSW_F* w        Array for calculated z values.
    npts     int        r        number of points
    iorder   int        r        order of the polynomial
    coef     CSW_F* r        coefficients as calculated by
                                 grd_calc_trend_surface

        Note:  The coefficients should be the same as returned from
               grd_calc_trend_surface.  If you rearrange the order of
               the coefficient array, the evaluation results will
               be bad.

*/

int CSWGrdTsurf::grd_eval_trend_surface (CSW_F *x, CSW_F *y, CSW_F *z, int npts,
                            int iorder, CSW_F *coef)
{
    int             i, j, ncoef;
    double          *dx = NULL, *dy = NULL, dcoef[MAX_COEFS], val;


    auto fscope = [&]()
    {
        csw_Free (dx);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    check for obvious parameter errors
*/
    if (x == NULL  ||  y == NULL  ||
        z == NULL  ||  coef == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (iorder < 1  ||  iorder > 8) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    allocate workspace
*/
MSL
    dx = (double *)csw_Malloc (2 * npts * sizeof(double));
    if (!dx) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    dy = dx + npts;

/*
    convert to double precision
*/
    for (i=0; i<npts; i++) {
        dx[i] = (double)x[i];
        dy[i] = (double)y[i];
    }

    ncoef = (iorder + 1) * (iorder + 2) / 2;

/*
    evaluate the points
*/
    for (i=0; i<npts; i++) {
        memset (dcoef, 0, MAX_COEFS*sizeof(double));
        PolyCoef (dx[i], dy[i], dcoef, iorder);
        val = 0.0;
        for (j=0; j<ncoef; j++) {
            val += dcoef[j] * coef[j];
        }
        z[i] = (CSW_F)val;
    }

/*
    clean up and return
*/
    return 1;

}  /*  end of function grd_eval_trend_surface  */






/*
  ****************************************************************

              g r d _ c a l c _ t r e n d _ g r i d

  ****************************************************************

  function name:  grd_calc_trend_grid              (int)

  call sequence:  grd_calc_trend_grid (x, y, z, npts, iorder,
                                       grid, ncol, nrow,
                                       xmin, ymin, xmax, ymax)

  purpose:        Calculate a grid for a trend surface.  The
                  surface coefficients are calculated and then
                  evaluated at the nodes of the specified grid.

  return value:   1 = success
                 -1 = error

  errors:        1        memory allocation error
                 3        iorder is outside of 1-8
                          range or npts is too low
                          for the order specified.
                 4        The data cannot be fit, probably
                          because they are colinear or all
                          at the same location
                 5        ncol, nrow is less than 8 or
                          xmin >= xmax or ymin >= ymax

  calling parameters:

    x         CSW_F*  r    array of x coordinates
    y         CSW_F*  r    array of y coordinates
    z         CSW_F*  r    array of z coordinates
    npts      int         r    number of points
    iorder    int         r    order of polynomial
    grid      CSW_F*  w    output grid
    ncol      int         r    number of columns in grid
    nrow      int         r    number of rows in grid
    xmin      CSW_F   r    minimum x coordinate of grid
    ymin      CSW_F   r    minimum y coordinate of grid
    xmax      CSW_F   r    maximum x or grid
    ymax      CSW_F   r    maximum y of grid

        Note:  grid must be large enough for ncol * nrow CSW_F numbers.

*/

int CSWGrdTsurf::grd_calc_trend_grid (CSW_F *x, CSW_F *y, CSW_F *z, int npts, int iorder,
                         CSW_F *grid, int ncol, int nrow,
                         CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax)
{
    int              irow, i, j, np2, nw, maxp, minp, istat, ndcoef;
    CSW_F            *x2 = NULL, *y2 = NULL, *z2 = NULL,
                     *xw = NULL, *yw = NULL, *zw = NULL,
                     xspac, yspac;
    double           *dx = NULL, *dy = NULL, *dz = NULL,
                     dx1, dy1, dx2, dy2,
                     dcoef[MAX_COEFS];
    double           dxt, dyt, dzt;

    int              do_write;
    char             fname[100];

    static const int       minpts [] = {3, 6, 10, 15, 21, 28, 36, 45};
    static const int       maxpts [] = {200, 200, 200, 300, 400, 500, 800, 950};


    auto fscope = [&]()
    {
        csw_Free (xw);
        csw_Free (dx);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    check obvious parameter errors
*/
    if (x == NULL  ||  y == NULL  ||
        z == NULL  ||  grid == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (ncol < MIN_COLS_TSURF ||  nrow < MIN_ROWS_TSURF  ||
        xmin >= xmax  ||  ymin >= ymax) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }

    if (iorder < 1  ||  iorder > 8) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    minp = minpts[iorder-1];
    maxp = maxpts[iorder-1];

    if (npts < minp) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    If there are a lot of points, thin them out
    before calculating the trend surface.
*/
    xw = NULL;
    if (npts > maxp) {

MSL
        xw = (CSW_F *)csw_Malloc (maxp * 3 * sizeof(CSW_F));
        if (!xw) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        yw = xw + maxp;
        zw = yw + maxp;

        istat = grd_utils_ptr->grd_collect_points (x, y, z, npts, maxp,
                                    xw, yw, zw, &nw);
        if (istat == -1  ||  nw < minp) {
            grd_utils_ptr->grd_set_err (4);
            return -1;
        }

        x2 = xw;
        y2 = yw;
        z2 = zw;
        np2 = nw;

    }
    else {

        x2 = x;
        y2 = y;
        z2 = z;
        np2 = npts;

    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        strcpy (fname, "tsurf_calc.xyz");
        grd_fileio_ptr->grd_write_float_points (x2, y2, z2, np2, fname);
    }


/*
    allocate work space.
*/
MSL
    dx = (double *)csw_Malloc (np2 * 3 * sizeof(double));
    if (!dx) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    dy = dx + np2;
    dz = dy + np2;

/*
    convert data to double
*/
    dx1 = 1.e30;
    dy1 = 1.e30;
    dx2 = -1.e30;
    dy2 = -1.e30;
    for (i=0; i<np2; i++) {
        dx[i] = (double)x2[i];
        dy[i] = (double)y2[i];
        dz[i] = (double)z2[i];
        dz[i] *= 100000.0;
        if (dx[i] < dx1) dx1 = dx[i];
        if (dx[i] > dx2) dx2 = dx[i];
        if (dy[i] < dy1) dy1 = dy[i];
        if (dy[i] > dy2) dy2 = dy[i];
    }

    xspac = (xmax - xmin) / (CSW_F)(ncol - 1);
    yspac = (ymax - ymin) / (CSW_F)(nrow - 1);

/*
    calculate the surface coefficients
*/
    istat = SurfaceFit (dx, dy, dz, np2, iorder,
                        dcoef, &ndcoef);
    if (istat == -1) {
        return -1;
    }

/*
    convert dcoefs into CSW_F coefs for evaluation
*/
    for (i=0; i<ndcoef; i++) {
        if (dcoef[i] < Z_ABSOLUTE_TINY  &&
            dcoef[i] > -Z_ABSOLUTE_TINY) {
            dcoef[i] = 0.0;
        }
    }

/*
    evaluate the grid nodes
*/
    for (i=0; i<nrow; i++) {
        irow = i * ncol;
        dyt = i * yspac + ymin;
        for (j=0; j<ncol; j++) {
            dxt = j * xspac + xmin;
            istat = eval_trend_surface2 (&dxt, &dyt, &dzt, 1, iorder, dcoef);
            if (istat == -1) {
                return -1;
            }
            *(grid + irow + j) = (CSW_F)(dzt / 100000.0);;
        }
    }

/*
    clean up and return success
*/
    return 1;


}  /*  end of function grd_calc_trend_grid  */




/*
  ****************************************************************

                        P o l y C o e f

  ****************************************************************

  Calculate the matrix coefficients for x and y coordinates in
  a trend surface.  The coordinates are expanded according to the
  order of the polynomial.

  This is only called from the private SurfaceFit function.  The
  calling function is responsible for supplying a large enough p
  array for all the coefficients needed given the order of the fit.

    The return status is always 1.

*/

int CSWGrdTsurf::PolyCoef (double x, double y, double *p, int iord)
{
    double          x2, y2, x3, y3, x4, y4;

    if (iord < 1) iord = 1;
    if (iord > 8) iord = 8;

    p[0] = 1.0;
    p[1] = x;
    p[2] = y;

    if (iord == 1) return 1;

    x2 = x * x;
    y2 = y * y;
    p[3] = x2;
    p[4] = x * y;
    p[5] = y2;

    if (iord == 2) return 1;

    x3 = x2 * x;
    y3 = y2 * y;
    p[6] = x3;
    p[7] = x2 * y;
    p[8] = x * y2;
    p[9] = y3;

    if (iord == 3) return 1;

    x4 = x2 * x2;
    y4 = y2 * y2;
    p[10] = x4;
    p[11] = x3 * y;
    p[12] = x2 * y2;
    p[13] = x * y3;
    p[14] = y4;

    if (iord == 4) return 1;

    p[15] = x3 * x2;
    p[16] = x4 * y;
    p[17] = x3 * y2;
    p[18] = x2 * y3;
    p[19] = x * y4;
    p[20] = y3 * y2;

    if (iord == 5) return 1;

    p[21] = x3 * x3;
    p[22] = x3 * x2 * y;
    p[23] = x4 * y2;
    p[24] = x3 * y3;
    p[25] = x2 * y4;
    p[26] = x * y2 * y3;
    p[27] = y3 * y3;

    if (iord == 6) return 1;

    p[28] = x3 * x4;
    p[29] = x3 * x3 * y;
    p[30] = x3 * x2 * y2;
    p[31] = x4 * y3;
    p[32] = x3 * y4;
    p[33] = x2 * y2 * y3;
    p[34] = x * y3 * y3;
    p[35] = y3 * y4;

    if (iord == 7) return 1;

    p[36] = x4 * x4;
    p[37] = x3 * x4 * y;
    p[38] = x3 * x3 * y2;
    p[39] = x2 * x3 * y3;
    p[40] = x4 * y4;
    p[41] = x3 * y2 * y3;
    p[42] = x2 * y3 * y3;
    p[43] = x * y3 * y4;
    p[44] = y4 * y4;

    return 1;

}  /*  end of private PolyCoef function  */






/*
  ****************************************************************

                        S u r f a c e F i t

  ****************************************************************

    This function returns the coefficients for a trend surface
  of the specified order.  The function is only called from the
  grd_calc_trend_surface function.

    On success, 1 is returned.  On failure -1 is returned and the
  grid error number is appropriately set.

*/

int CSWGrdTsurf::SurfaceFit (double *x, double *y, double *z, int ndata, int iord,
                       double *a, int *ma)
{
    double          *dt = NULL, *dt2 = NULL, **u = NULL,
                    **v = NULL, *w = NULL, *b = NULL;
    double          tol, wmax, thresh, afunc[MAX_COEFS];
    int             istat, mp, np, i, j;


    auto fscope = [&]()
    {
        csw_Free (u);
        csw_Free (dt);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Allocate space for work arrays.
*/
    np = (iord + 1) * (iord + 2) / 2;
    mp = ndata;
    *ma = np;

    mp+=2;
    np+=2;

MSL
    u = (double **)csw_Calloc ((np + mp) * sizeof(double *));
    if (!u) {
        return -1;
    }
    v = u + mp;

    i = mp * np + np * np + np + mp;
MSL
    dt = (double *)csw_Calloc (i * sizeof(double));
    if (!dt) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    for (i=0; i<mp; i++) {
        u[i] = (double *)(dt + i * np);
    }
    dt2 = dt + mp * np;
    for (i=0; i<np; i++) {
        v[i] = (double *)(dt2 + i * np);
    }

    w = (double *)(dt + mp*np + np*np);
    b = (double *)(w + np);

    mp-=2;
    np-=2;

/*
    Set the tolerance for editing the singular values and
    some other needed constants.  The Numerical Recipes text
    uses 1.e-06 for this value.  Since this function is
    double and the Numerical Recipes is single precision,
    changing thie tolerance to 1.e-12 is appropriate.

    The change is part of the fix for bug_9851.  In this bug,
    the x and y extents of the data were each about 0 to 1000000.
    The range of the independent variables seems to control
    the range of singular values calculated.  In order to
    calculate for xy data ranges possible in mapping, changing
    the tol to 1.e-10 is needed.  I added a couple of orders of
    magnitude for good measure.
*/
    tol = 1.0e-12;

/*
    Accumulate coefficients for the fitting matrix.
    The SvdCmp function uses index 1 as the start
    of the arrays, so adjust accordingly.
*/
    for (i=0; i<ndata; i++) {
        PolyCoef (x[i], y[i], afunc, iord);
        for (j=0; j<np; j++) {
            u[i+1][j+1] = afunc[j];
        }
        b[i+1] = z[i];
    }

/*
    Do the singular value decomposition.
*/
    istat = SvdCmp (u, ndata, np, w, v);
    if (istat == -1) {
        return -1;
    }

/*
    Edit the singular values.
*/
    wmax = 0.0;
    for (i=0; i<np; i++) {
        if (w[i+1] > wmax) wmax = w[i+1];
    }
    thresh = tol * wmax;
    for (i=0; i<np; i++) {
        if (w[i+1] < thresh) w[i+1] = 0.0;
    }

/*
    Do the back substitution.
*/
    istat = SvdBackSub (u, w, v, ndata, np, b, afunc);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<np; i++) {
        a[i] = afunc[i+1];
    }

    return 1;

}  /*  end of private SurfaceFit function  */



/*
  ****************************************************************

                          S v d C m p

  ****************************************************************

    Do the singular value decomposition of a matrix.

*/

int CSWGrdTsurf::SvdCmp (double **a, int m, int n, double *w, double **v)
{
    int       flag,i,its,j,jj,k,l,nm;
    double    anorm,c,f,g,h,s,scale,x,y,z,*rv1=NULL;


    auto fscope = [&]()
    {
        csw_Free (rv1);
    };
    CSWScopeGuard func_scope_guard (fscope);


    l = nm = 0;

MSL
    rv1= (double *)csw_Calloc ((n+2) * sizeof(double));
    if (!rv1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    g=scale=anorm=0.0;
    for (i=1;i<=n;i++) {
        l=i+1;
        rv1[i]=scale*g;
        g=s=scale=0.0;
        if (i <= m) {
            for (k=i;k<=m;k++) scale += fabs(a[k][i]);
            if (scale != 0.0) {
                for (k=i;k<=m;k++) {
                    a[k][i] /= scale;
                    s += a[k][i]*a[k][i];
                }
                f=a[i][i];
                g = -SIGN(sqrt(s),f); /*lint !e666*/
                h=f*g-s;
                a[i][i]=f-g;
                for (j=l;j<=n;j++) {
                    for (s=0.0,k=i;k<=m;k++) s += a[k][i]*a[k][j];
                    f=s/h;
                    for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
                }
                for (k=i;k<=m;k++) a[k][i] *= scale;
            }
        }
        w[i]=scale *g;
        g=s=scale=0.0;
        if (i <= m && i != n) {
            for (k=l;k<=n;k++) scale += fabs(a[i][k]);
            if (scale != 0.0) {
                for (k=l;k<=n;k++) {
                    a[i][k] /= scale;
                    s += a[i][k]*a[i][k];
                }
                f=a[i][l];
                g = -SIGN(sqrt(s),f); /*lint !e666*/
                h=f*g-s;
                a[i][l]=f-g;
                for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
                for (j=l;j<=m;j++) {
                    for (s=0.0,k=l;k<=n;k++) s += a[j][k]*a[i][k];
                    for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
                }
                for (k=l;k<=n;k++) a[i][k] *= scale;
            }
        }
        anorm=DMAX(anorm,(fabs(w[i])+fabs(rv1[i])));
    }
    for (i=n;i>=1;i--) {
        if (i < n) {
            if (g != 0.0) {
                for (j=l;j<=n;j++)
                    v[j][i]=(a[i][j]/a[i][l])/g;
                for (j=l;j<=n;j++) {
                    for (s=0.0,k=l;k<=n;k++) s += a[i][k]*v[k][j];
                    for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
                }
            }
            for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
        }
        v[i][i]=1.0;
        g=rv1[i];
        l=i;
    }
    for (i=IMIN(m,n);i>=1;i--) {
        l=i+1;
        g=w[i];
        for (j=l;j<=n;j++) a[i][j]=0.0;
        if (g != 0.0) {
            g=1.0/g;
            for (j=l;j<=n;j++) {
                for (s=0.0,k=l;k<=m;k++) s += a[k][i]*a[k][j];
                f=(s/a[i][i])*g;
                for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
            }
            for (j=i;j<=m;j++) a[j][i] *= g;
        } else for (j=i;j<=m;j++) a[j][i]=0.0;
        ++a[i][i];
    }
    for (k=n;k>=1;k--) {
        for (its=1;its<=30;its++) {
            flag=1;
            for (l=k;l>=1;l--) {
                nm=l-1;
                if ((double)(fabs(rv1[l])+anorm) - anorm  ==  0.0) {
                    flag=0;
                    break;
                }
                if ((double)(fabs(w[nm])+anorm) - anorm  ==  0.0) break;
            }
            if (flag) {
                c=0.0;
                s=1.0;
                for (i=l;i<=k;i++) {
                    f=s*rv1[i];
                    rv1[i]=c*rv1[i];
                    if ((double)(fabs(f)+anorm) - anorm == 0.0) break;
                    g=w[i];
                    h=sqrt(f*f+g*g);
                    w[i]=h;
                    h=1.0/h;
                    c=g*h;
                    s = -f*h;
                    for (j=1;j<=m;j++) {
                        y=a[j][nm];
                        z=a[j][i];
                        a[j][nm]=y*c+z*s;
                        a[j][i]=z*c-y*s;
                    }
                }
            }
            z=w[k];
            if (l == k) {
                if (z < 0.0) {
                    w[k] = -z;
                    for (j=1;j<=n;j++) v[j][k] = -v[j][k];
                }
                break;
            }
            if (its == 30) {
                grd_utils_ptr->grd_set_err (3);
                return -1;
            }
            x=w[l];
            nm=k-1;
            y=w[nm];
            g=rv1[nm];
            h=rv1[k];
            f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
            g=sqrt(f*f+1.0);
            f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
            c=s=1.0;
            for (j=l;j<=nm;j++) {
                i=j+1;
                g=rv1[i];
                y=w[i];
                h=s*g;
                g=c*g;
                z=sqrt(f*f+h*h);
                rv1[j]=z;
                c=f/z;
                s=h/z;
                f=x*c+g*s;
                g = g*c-x*s;
                h=y*s;
                y *= c;
                for (jj=1;jj<=n;jj++) {
                    x=v[jj][j];
                    z=v[jj][i];
                    v[jj][j]=x*c+z*s;
                    v[jj][i]=z*c-x*s;
                }
                z=sqrt(f*f+h*h);
                w[j]=z;
                if (z != 0.0) {
                    z=1.0/z;
                    c=f*z;
                    s=h*z;
                }
                f=c*g+s*y;
                x=c*y-s*g;
                for (jj=1;jj<=m;jj++) {
                    y=a[jj][j];
                    z=a[jj][i];
                    a[jj][j]=y*c+z*s;
                    a[jj][i]=z*c-y*s;
                }
            }
            rv1[l]=0.0;
            rv1[k]=f;
            w[k]=x;
        }
    }

    return 1;

}  /*  end of private SvdCmp function  */



/*
  ****************************************************************

                         S v d B a c k S u b

  ****************************************************************

    Calculate the solution vector via back substitution.

*/

int CSWGrdTsurf::SvdBackSub (double **u, double *w, double **v,
                       int m, int n, double *b, double *x)
{
    int               jj,j,i;
    double            s,*tmp = NULL;


    auto fscope = [&]()
    {
        csw_Free (tmp);
    };
    CSWScopeGuard func_scope_guard (fscope);


MSL
    tmp = (double *)csw_Malloc ((n+2) * sizeof(double));
    if (!tmp) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    for (j=1;j<=n;j++) {
        s=0.0;
        if (w[j] != 0.0) {
            for (i=1;i<=m;i++) s += u[i][j]*b[i];
            s /= w[j];
        }
        tmp[j]=s;
    }
    for (j=1;j<=n;j++) {
        s=0.0;
        for (jj=1;jj<=n;jj++) s += v[j][jj]*tmp[jj];
        x[j]=s;
    }

    return 1;

}  /*  end of private SvdBackSub function  */






/*
  ****************************************************************

           g r d _ c a l c _ d o u b l e _ t r e n d _ s u r f a c e

  ****************************************************************

  function name:  grd_calc_trend_surface        (int)

  call sequence:  grd_calc_trend_surface (x, y, z, npts, iorder,
                                          coef)

  purpose:        Calculate the coefficients for an iorder trend surface
                  with a least squares best fit through the specified
                  points.

  return value:   1 = success
                 -1 = failure

  errors:         1 = error allocating workspace
                  2 = too few points for the order or
                      the order is out of the 1-8 range
                  3 = the data cannot be fit, probably
                      because they are colinear or all
                      in the same location


  calling parameters:

    x       double *      r       array of x coordinates
    y       double*       r       array of y coordinates
    z       double*       r       array of z coordinates
    npts    int           r       number of points in the arrays
    iorder  int           r       order of the polynomial to calculate
                                  this must be 1 - 8
    coef    double*       w       returned coefficients

        Note:  You must supply a coef array large enough for the
               number of coefficients returned.  For the highest
               order polynomial supported, 45 coefficients are
               returned.

*/

int CSWGrdTsurf::grd_calc_double_trend_surface
             (double *x, double *y, double *z, int npts, int iorder,
              double *coef)
{
    double       *dx = NULL, *dy = NULL, *dz = NULL, dcoef[MAX_COEFS];
    int          i, istat, nc;

/*
    check obvious parameter errors
*/
    if (x == NULL  ||  y == NULL  ||
        z == NULL  ||  coef == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (iorder < 1  ||  iorder > 8) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    i = (iorder +1) * (iorder + 2) / 2;
    if (npts < i) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

/*
    allocate workspace
*/
    dx = x;
    dy = y;
    dz = z;

/*
    Calculate the coefficients.
*/
    istat = SurfaceFit (dx, dy, dz, npts, iorder,
                        dcoef, &nc);
    if (istat == -1) {
        return -1;
    }

    for (i=0; i<nc; i++) {
        coef[i] = dcoef[i];
    }

    return 1;

}  /*  end of function grd_calc_trend_surface  */






/*---------------------------------------------------------------------------*/

int CSWGrdTsurf::eval_trend_surface2 (double *dx, double *dy, double *dz, int npts,
                            int iorder, double *dcoef)

{
    int             i, j, ncoef;
    double          val, dcoef2[100];

/*
    check for obvious parameter errors
*/
    if (dx == NULL  ||  dy == NULL  ||
        dz == NULL  ||  dcoef == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (iorder < 1  ||  iorder > 8) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_utils_ptr->grd_simulation()) {
        return 1;
    }

    ncoef = (iorder + 1) * (iorder + 2) / 2;

/*
    evaluate the points
*/
    for (i=0; i<npts; i++) {
        memset (dcoef2, 0, MAX_COEFS*sizeof(double));
        PolyCoef (dx[i], dy[i], dcoef2, iorder);
        val = 0.0;
        for (j=0; j<ncoef; j++) {
            val += dcoef[j] * dcoef2[j];
        }
        dz[i] = (CSW_F)val;
    }

    return 1;

}  /*  end of function eval_trend_surface2  */

/*
  ****************************************************************

           g r d _ c a l c _ p e r p _ p l a n e _ f i t

  ****************************************************************

  function name:  grd_calc_perp_plane_fit        (int)

  call sequence:  grd_calc_perp_plane_fit (x, y, z, npts,
                                           coef)

  purpose:        Calculate the coefficients of a plane that minimizes
                  the perpendicular distance to the plane.

  return value:   1 = success
                 -1 = failure

  errors:         1 = error allocating workspace
                  2 = too few points for the order or
                      the order is out of the 1-8 range
                  3 = the data cannot be fit, probably
                      because they are colinear or all
                      in the same location


  calling parameters:

    x       double*    r       array of x coordinates
    y       double*    r       array of y coordinates
    z       double*    r       array of z coordinates
    npts    int        r       number of points in the arrays
    coef    double*    w       returned coefficients (at least [3])

*/

int CSWGrdTsurf::grd_calc_perp_plane_fit (
    double    *x,
    double    *y,
    double    *z,
    int       npts,
    double    *coef)
{
    int          i, istat;

/*
    check obvious parameter errors
*/
    if (x == NULL  ||  y == NULL  ||
        z == NULL  ||  coef == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }

    if (npts < 3) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    Calculate the coefficients.
*/
    istat = PerpPlaneFit (x, y, z, npts,
                          coef);
    if (istat == -1) {
        return -1;
    }

/*
 *  If a coefficient is essentially zero, make it zero.  This
 *  is done to avoid float underflow errors.
 */
    for (i=0; i<3; i++) {
        if (coef[i] < Z_ABSOLUTE_TINY  &&
            coef[i] > -Z_ABSOLUTE_TINY) {
            coef[i] = 0.0;
        }
    }

    return 1;

}  /*  end of function grd_calc_perp_plane_fit  */




/*
 ******************************************************************************

  The algorithm here is from the following url:

    http://mathforum.org/library/drmath/view/63765.htmp

  The specified points are used to calculate the mean of x, y and z.  After
  the mean is available, the signed difference of each point from the mean is
  used to populate a matix suitable for input into the SvdCmp function.  As
  explained in the above reference, the output of the SvdCmp function can be
  used to get the normal vector of the plane which minimizes the perpendicular
  distance from each point to the plane.

  SvdCmp calculates 3 singular values (in the array designated w here) and it
  calculates 3 singular vectors (in the 2d array designated v here).  The 
  normal vector of the best fitting plane corresponds to the column in the
  singular vector array that has the same index as the minimum singular value.

  The returned coefficients are calculated assuming that the normal vector 
  originates at the mean x, y, z point.

 ******************************************************************************
*/

int CSWGrdTsurf::PerpPlaneFit (double *x, double *y, double *z, int ndata,
                               double *coef)

{
    double          *dt = NULL, *dt2 = NULL,
                    **u = NULL, **v = NULL, *w = NULL;
    int             istat, mp, np, i;
    double          x0, y0, z0;
    int             imin;
    double          st, smin, pnx, pny, pnz;
    double          xyabs, dist;

    int             do_write;


    auto fscope = [&]()
    {
        csw_Free (u);
        csw_Free (dt);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Initialize output in case of error.
 */
    coef[0] = 1.e30;
    coef[1] = 1.e30;
    coef[2] = 1.e30;

    if (ndata < 3) {
        return -1;
    }

/*
    Allocate space for work arrays.  Because of the 1 base indexing in svdcmp,
    The work arrays are a bit larger than you might expect.
*/
    np = 3;
    mp = ndata;

    mp+=2;
    np+=2;

    u = (double **)csw_Calloc ((np + mp) * sizeof(double *));
    if (!u) {
        return -1;
    }
    v = u + mp;

    i = mp * np + np * np + np + mp;
    dt = (double *)csw_Calloc (i * sizeof(double));
    if (!dt) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    for (i=0; i<mp; i++) {
        u[i] = (double *)(dt + i * np);
    }
    dt2 = dt + mp * np;
    for (i=0; i<np; i++) {
        v[i] = (double *)(dt2 + i * np);
    }

    w = (double *)(dt + mp*np + np*np);

    mp-=2;
    np-=2;

/*
 *  Find the "centroid" of the points.  In this case I
 *  just use the mean values of x, y and z as the centroid.
 */
    x0 = y0 = z0 = 0.0;
    for (i=0; i<ndata; i++) {
        x0 += x[i];
        y0 += y[i];
        z0 += z[i];
    }
    x0 /= ndata;
    y0 /= ndata;
    z0 /= ndata;

/*
 *  The matrix to be decomposed consists of the variation from the
 *  centroid at each point.  Note that the SvdComp requires 1 based
 *  indexing for input and also returns 1 based indexing for output.
 */
    for (i=0; i<ndata; i++) {
        u[i+1][1] = x[i] - x0;
        u[i+1][2] = y[i] - y0;
        u[i+1][3] = z[i] - z0;
    }

/*
    Do the singular value decomposition.
*/
    istat = SvdCmp (u, ndata, np, w, v);
    if (istat == -1) {
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        FILE    *fptr;
        char    line[200];
        int     nout;
        fptr = fopen ((char *)"svd.out", "wb");
        if (fptr != NULL) {
            nout = ndata;
            if (nout > 40) nout = 40;
            for (i=0; i<nout; i++) {
                sprintf (line, "%.2f %.2f %.2f\n", x[i], y[i], z[i]);
                fputs (line, fptr);
            }
            sprintf (line, "\nw = %f %f %f\n", w[1], w[2], w[3]);
            fputs (line, fptr);
            for (i=0; i<3; i++) {
                sprintf (line, "v row %d = %f %f %f\n",
                         i, v[i+1][1], v[i+1][2], v[i+1][3]);
                fputs (line, fptr);
            }
            fclose (fptr);
        }
    }

/*
 * Save the w and v values for a test program to retrieve.
 */
    Svd_w_save[0] = w[1];
    Svd_w_save[1] = w[2];
    Svd_w_save[2] = w[3];

    Svd_v_save[0] = v[1][1];
    Svd_v_save[1] = v[1][2];
    Svd_v_save[2] = v[1][3];
    Svd_v_save[3] = v[2][1];
    Svd_v_save[4] = v[2][2];
    Svd_v_save[5] = v[2][3];
    Svd_v_save[6] = v[3][1];
    Svd_v_save[7] = v[3][2];
    Svd_v_save[8] = v[3][3];

/*
 * Find the minimum squared value of w (the singular values).  The index of
 * this value points to the column in v (the singular vectors) that corresponds
 * to the normal of the best fitting plane.
 */
    smin = 1.e40;
    imin = -1;
    for (i=0; i<3; i++) {
        st = w[i+1] * w[i+1];
        if (st < smin) {
            smin = st;
            imin = i;
        }
    }

    if (imin < 0  ||  smin > 1.e20) {
        return -1;
    }

    pnx = v[1][imin+1];
    pny = v[2][imin+1];
    pnz = v[3][imin+1];

/*
 * Use the standard of positive z normal.
 */
    if (pnz < 0.0) {
        pnz = -pnz;
        pnx = -pnx;
        pny = -pny;
    }

    xyabs = pnx * pnx + pny * pny;
    xyabs = sqrt (xyabs);

/*
 * "Normalize" the normal to a magnitude of 1.0
 */
    if (pnz < 0.0000001 * xyabs) {
        pnz = 0.0000001 * xyabs;
        dist = pnx * pnx + pny * pny + pnz * pnz;
        dist = sqrt (dist);
        pnx /= dist;
        pny /= dist;
        pnz /= dist;
    }

    Svd_pn_save[0] = pnx;
    Svd_pn_save[1] = pny;
    Svd_pn_save[2] = pnz;

    coef[1] = -pnx / pnz;
    coef[2] = -pny / pnz;
    coef[0] = z0 - (x0 * coef[1] + y0 * coef[2]);

    return 1;

}  /*  end of private PerpPlaneFit function  */
