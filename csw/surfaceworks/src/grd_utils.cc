
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_utils.cc

    This file implements the methods of the CSWGrdUtils class.
    These methods are general utilities used by many different
    parts of the surface code.
*/

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "csw/utils/include/csw_.h"

#include "csw/utils/private_include/simulP.h"
#include "csw/utils/private_include/csw_scope.h"
#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_graph.h"
#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/ply_protoP.h"

#include "csw/surfaceworks/private_include/grd_utils.h"



/*
    Define some constants for the file
*/

#define MAX_BINS              2000
#define MAX_PLANE             100

#define _VALLEY_              1
#define _PLATEAU_             2

#define SOFT_NULL_VALUE       -1.e15f






/*
  ****************************************************************

          g r d _ s e t _ h a r d _ n u l l _ v a l u e s

  ****************************************************************

    Set masked nodes in a grid to null values depending on the
  flag.  If flag = 1, both outside and inside bad nodes are set.
  For flag = 999, any non zero mask is set to a hard null value.
  For any other flag value, only outside are set.

    This is an internally called function.  The application should
  call grd_SetHardNullValues instead of this function.

*/

int CSWGrdUtils::grd_set_hard_null_values
                             (CSW_F *grid, char *mask, int ncol, int nrow,
                              CSW_F nullvalue, int flag, CSW_F **gridout)
{
    int        i;
    CSW_F      *g2 = NULL;
    *gridout = NULL;

    auto fscope = [&]()
    {
        if (g2 != NULL  &&  g2 != grid) {
            csw_Free (g2);
            g2 = NULL;
            *gridout = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    check obvious errors
*/
    if (ncol < -WildInteger  ||  ncol > WildInteger  ||
        nrow < -WildInteger  ||  nrow > WildInteger  ||
        flag < -WildInteger  ||  flag > WildInteger) {
        grd_set_err (3);
        return -1;
    }

    if (grid == NULL  ||  mask == NULL) {
        grd_set_err (2);
        return -1;
    }

    if (ncol < 2  ||  nrow < 2) {
        grd_set_err (4);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return success (which is not expected) here.
*/
    if (grd_simulation()) {
        return 1;
    }

/*
    If the gridout is not null, allocate space.
    Otherwise, use the input grid for output.
*/
    if (gridout != NULL) {
MSL
        g2 = (CSW_F *)csw_Malloc (ncol*nrow*sizeof(CSW_F));
        if (g2 == NULL) {
            grd_set_err (1);
            return -1;
        }
        *gridout = g2;
    }
    else {
        g2 = grid;
    }

/*
    loop through grid and set nulls
*/
    for (i=0; i<ncol*nrow; i++) {

        g2[i] = grid[i];
        if (mask[i] == GRD_MASK_USER_EDIT_RESET) {
            continue;
        }
        if (mask[i] == GRD_MASK_ENABLED_BY_RECALC) {
            continue;
        }
        if (mask[i] == 1) {
            g2[i] = nullvalue;
        }
        if (mask[i] == 2  &&  flag == 1) {
            g2[i] = nullvalue;
        }
        if (flag == 999) {
            if (mask[i] != 0) {
                g2[i] = nullvalue;
            }
        }

    }

    return 1;

}  /*  end of function grd_set_hard_null_values  */





/*
  ****************************************************************

            g r d _ r e c o m m e n d e d _ s i z e

  ****************************************************************

  function name: grd_recommended_size              (int)

  call sequence: grd_recommended_size (x, y, npts,
                                       x1, y1, x2, y2,
                                       ncol, nrow)

  purpose:       Return the recommended number of columns and rows
                 for a specified set of points.  If the bounds data
                 are specified, only points within the bounds are
                 used to calculate the recommended size.  If the
                 bounds are specified as any of the following, the
                 limits of the specified data are used for the bounds.

                   If x1, y1, x2 or y2 is greater than 1.e20f or less
                   than -1.e20f.

                   If x1 greater than or equal to x2 or if y1 greater
                   than or equal to y2.

  return value:    status code

                   -1 = error
                    1 = success

  errors:          1 = Memory allocation error.
                   2 = You specified less than 1 point.
                   3 = Either x or y is NULL.
                   4 = Either ncol or nrow is null.
                   5 = One of the boundary pointers is NULL.
                   7 = All points are in the same location.

  calling parameters:

    x        r    CSW_F*     Array of x coordinates.
    y        r    CSW_F*     Array of y coordinates.
    npts     r    int            Number of points in x,y.
    x1       r/w  *CSW_F     Minimum x value
    y1       r/w  *CSW_F     Minimum y value
    x2       r/w  *CSW_F     Maximum x value
    y2       r/w  *CSW_F     Maximum y value
    ncol     w    int*           Recommended number of columns.
    nrow     w    int*           Recommended number of rows.

*/

int CSWGrdUtils::grd_recommended_size
                         (CSW_F *x, CSW_F *y, int npts,
                          CSW_F *x1p, CSW_F *y1p, CSW_F *x2p, CSW_F *y2p,
                          int *ncol, int *nrow)
{
    int       i, j, k, n, bflag, ntot, n1, n2, cutoff,
              ngrid, dtype, nx, ny, nxsav, nysav, percell;
    char      *countgrid = NULL;
    CSW_F     *xx, *yy, x1, y1, x2, y2, datapct,
              xt, yt, xspace, yspace, aspect;
    CSW_F     x1sav, y1sav, x2sav, y2sav;
    double    dmult;

    auto fscope = [&]()
    {
        csw_Free (countgrid);
        countgrid = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    check for obvious errors
*/
    if (npts < 1) {
        grd_set_err (2);
        return -1;
    }

    if (x == NULL  ||  y == NULL) {
        grd_set_err (3);
        return -1;
    }

    if (ncol == NULL  ||  nrow == NULL) {
        grd_set_err (4);
        return -1;
    }

    if (x1p == NULL  ||  y1p == NULL  ||  x2p == NULL  ||  y2p == NULL) {
        grd_set_err (5);
        return -1;
    }

/*
    Transfer limits to local variables.
*/
    x1 = *x1p;
    y1 = *y1p;
    x2 = *x2p;
    y2 = *y2p;

/*
    If any of the bounds values is > 1.e20f or < -1.e20f,
    calculate bounds from data.
*/
    bflag = 0;
    if (x1 < -1.e20f || x1 > 1.e20f) bflag = 1;
    if (y1 < -1.e20f || y1 > 1.e20f) bflag = 1;
    if (x2 < -1.e20f || x2 > 1.e20f) bflag = 1;
    if (y2 < -1.e20f || y2 > 1.e20f) bflag = 1;

/*
    if any min is >= the corresponding max,
    calculate bounds from data
*/
    if (x1 >= x2) bflag = 1;
    if (y1 >= y2) bflag = 1;

/*
    If the boundary has been specified, save the
    specified points and calculate internal limits
    based on the extent of the data.
*/
    if (bflag == 0) {
        ntot = npts;
        xx = x;
        yy = y;
        x1sav = x1;
        y1sav = y1;
        x2sav = x2;
        y2sav = y2;
        x1 = 1.e30f;
        y1 = 1.e30f;
        x2 = -1.e30f;
        y2 = -1.e30f;
        for (i=0; i<npts; i++) {
            if (x[i] < x1) x1 = x[i];
            if (y[i] < y1) y1 = y[i];
            if (x[i] > x2) x2 = x[i];
            if (y[i] > y2) y2 = y[i];
        }
    }

/*
    No boundary was specified, so the extent of the data plus
    a small margin is used internally and is returned to the
    calling function.
*/
    else {
        x1 = 1.e30f;
        y1 = 1.e30f;
        x2 = -1.e30f;
        y2 = -1.e30f;
        for (i=0; i<npts; i++) {
            if (x[i] < x1) x1 = x[i];
            if (y[i] < y1) y1 = y[i];
            if (x[i] > x2) x2 = x[i];
            if (y[i] > y2) y2 = y[i];
        }
        ntot = npts;
        xx = x;
        yy = y;
        xt = (x2 - x1) / 40.0f;
        yt = (y2 - y1) / 40.0f;
        if (xt <= 0.0f  &&  yt <= 0.0f) {
            grd_set_err (7);
            return -1;
        }
        if (xt <= 0.0f) xt = yt;
        if (yt <= 0.0f) yt = xt;
        x1 -= xt;
        x2 += xt;
        y1 -= yt;
        y2 += yt;
        x1sav = x1;
        y1sav = y1;
        x2sav = x2;
        y2sav = y2;
    }

    datapct = ((x2 - x1) * (y2 - y1)) / ((x2sav - x1sav) * (y2sav - y1sav));
    datapct = (CSW_F)sqrt ((double)datapct);
    datapct = (CSW_F)sqrt ((double)datapct);
    if (datapct > 1.0f)
        datapct = 1.0f;
    if (datapct < 0.1)
        datapct = 0.1f;

/*
    Note that x1sav, y1sav, x2sav and y2sav have the limits
    of the area of interest that will be used for the final
    grid size determination and also will be returned to the
    calling function as the area of interest corners.
*/
    x1 = x1sav;
    y1 = y1sav;
    x2 = x2sav;
    y2 = y2sav;

/*
    If the program is in parameter error simulation mode,
    return successs (which is not expected) here.
*/
    if (grd_simulation()) {
        return 1;
    }

/*
    The initial guess assumes fairly evenly distributed
    data and for larger data sets, less nodes per data
    point are specified.
*/
    if (ntot < 50) {
        ngrid = 50 * ntot;
    }
    else if (ntot < 100) {
        ngrid = 25 * ntot;
    }
    else if (ntot < 500) {
        ngrid = 15 * ntot;
        i = 500 / ntot;
        if (i > 3) i = 3;
        ngrid *= i;
    }
    else if (ntot < 2000) {
        ngrid = 10 * ntot;
    }
    else if (ntot < 5000) {
        ngrid = 9 * ntot;
    }
    else if (ntot < 20000) {
        ngrid = 7 * ntot;
    }
    else if (ntot < 100000) {
        ngrid = 5 * ntot;
    }
    else if (ntot < 300000) {
        ngrid = 3 * ntot;
    }
    else {
        ngrid = 1 * ntot;
    }

    ngrid = (int)(ngrid * datapct);

    if (ngrid > 1000000) {
        ngrid = 1000000;
    }

/*
    Make a guess with x and y node spacing nearly equal
    and the number of columns and rows in the same ratio
    as the width to height of the data area.
*/
    if (ngrid < 25) ngrid = 25;
    aspect = (y2 - y1) / (x2 - x1);
    xt = (CSW_F)ngrid / aspect;
    xt = (CSW_F)sqrt ((double)xt);
    yt = (CSW_F)ngrid / xt;

    nx = (int) (xt + 1.5f);
    ny = (int) (yt + 1.5f);
    if (nx < 2) nx = 2;
    if (ny < 2) ny = 2;

    xspace = (x2 - x1) / (nx - 1);
    yspace = (y2 - y1) / (ny - 1);

/*
    If several consecutive points in the input arrays
    are quite close to each other, then the data are
    probably either profile or contour line data.
*/
    dtype = 0;
    if (ntot > 100) {

        n = 0;
        for (i=0; i<6; i++) {

            j = ntot / 7 * (i + 1);

            if (x[j+1] - x[j] < xspace  &&  x[j+2] - x[j+1] < xspace  &&
                x[j+3] - x[j+2] < xspace   &&
                y[j+1] - y[j] < yspace  &&  y[j+2] - y[j+1] < yspace  &&
                y[j+3] - y[j+2] < yspace) {
                n++;
            }

        }

    /*
        If the data are in profile or contour
        configuration, increase the total number
        of grid nodes recommended.
    */
        if (n > 3) {
            if (nx * ny > 15000  &&  nx * ny < 30000) {
                nx = nx * 11 / 10;
                ny = ny * 11 / 10;
            }
            else {
                nx = nx * 6 / 5;
                ny = ny * 6 / 5;
            }
            dtype = 1;
        }

    }

/*
    Allocate space for counting data points per grid cell.
*/
MSL
    countgrid = (char *)csw_Calloc (nx*ny*sizeof(char));
    if (!countgrid) {
        grd_set_err (1);
        return -1;
    }

/*
    Count grid cells with more than percell points in them.  In case
    of problem data where several z values are assigned to each point,
    the percell number is incremented.
*/
    percell = 1;
    for (;;) {
        memset (countgrid, 0, nx*ny*sizeof(char));
        for (n = 0; n<ntot; n++) {
            j = (int)((xx[n] - x1) / xspace);
            i = (int)((yy[n] - y1) / yspace);
            if (j < 0) continue;
            if (j > nx-1) continue;
            if (i < 0) continue;
            if (i > ny-1) continue;
            k = i * nx + j;
            countgrid[k]++;
        }
        n1 = 0;
        n2 = 0;
        for (n = 0; n < nx*ny; n++) {
            if (countgrid[n] != 0) {
                n1++;
                if (countgrid[n] > percell) {
                    n2++;
                }
            }
        }
        if (n2 < n1*4/5) {
            break;
        }
        percell++;
        if (percell == 9) break;
    }

/*
    The percell number is now established.  Do one more count
    to use for grid size adjustment.
*/
    memset (countgrid, 0, nx*ny*sizeof(char));
    n2 = 0;
    for (n = 0; n<ntot; n++) {
        j = (int)((xx[n] - x1) / xspace);
        i = (int)((yy[n] - y1) / yspace);
        if (j < 0) continue;
        if (j > nx-1) continue;
        if (i < 0) continue;
        if (i > ny-1) continue;
        k = i * nx + j;
        if (countgrid[k] <= percell) {
            countgrid[k]++;
        }
        if (countgrid[k] > percell) {
            n2++;
        }
    }

/*
    For contour or profile data, only increment
    the grid size if over half of the points
    are doubled up in cells.
*/
    if (percell > 1) {
        nx = (int)(nx / sqrt ((double)percell));
        ny = (int)(ny / sqrt ((double)percell));
        if (nx < 2) nx = 2;
        if (ny < 2) ny = 2;
    }
    ngrid = nx * ny;
    if (dtype == 1) {
        cutoff = ngrid / 2;
    }

/*
    For other data styles, increase the grid size
    if a lot less cells are multiply occupied.
    For large data sets, do not expand as much for
    multiply occupied grid cells.
*/
    else {
        cutoff = ngrid / 10;
        xt = (CSW_F)ntot / 10000.f;
        if (xt < 0.01f) xt = 0.01f;
        if (xt > 4.0f) xt = 4.0f;
        cutoff = (int)((CSW_F)cutoff *  xt);
        if (cutoff < 2) cutoff = 2;
    }

    if (ntot < 10)
        cutoff = 2;

    if (n2 > cutoff) {
        dmult = (double)n2 / (double)cutoff;
        dmult = sqrt (dmult);
        if (dmult > 3) dmult = 3;
        nx = (int) (nx * dmult);
        ny = (int) (ny * dmult);
    }

    if (ntot < 50  &&  n2 > 0) {
        nx = nx * 4 / 3;
        ny = ny * 4 / 3;
    }
    else if (ntot < 100  &&  n2 > 0) {
        nx = nx * 5 / 4;
        ny = ny * 5 / 4;
    }

/*
    The number of columns and rows is currently based on the
    internal area of interest.  These need to be recalculated
    to reflect the user specified area of interest.
*/
    xspace = (x2 - x1) / (nx - 1);
    yspace = (y2 - y1) / (ny - 1);
    x1 = x1sav;
    y1 = y1sav;
    x2 = x2sav;
    y2 = y2sav;

    if (bflag) {
        x1 -= xspace;
        y1 -= yspace;
        x2 += xspace;
        y2 += yspace;
    }

    nxsav = nx;
    nysav = ny;
    nx = (int)((x2 - x1) / xspace + 0.5f);
    ny = (int)((y2 - y1) / yspace + 0.5f);
    if (nx > 20 * nxsav)
        nx = 20 * nxsav;
    if (ny > 20 * nysav)
        ny = 20 * nysav;

    if (nx < 5) nx = 5;
    if (ny < 5) ny = 5;

/*
 * Bug 9489
 *
 *  If a bad point location is in the input, a huge grid
 *  may be recommended.  The maximum number of grid cells
 *  from a recommended grid is set to about 1 million.  This
 *  mitigates the problem.
 */
    n = 0;
    while (n < 20  &&  nx * ny > 1000000) {
        nx *= 4;
        nx /= 5;
        ny *= 4;
        ny /= 5;
        n++;
    }

    if (n >= 20) {
        nx = 1000;
        ny = 1000;
    }

    *ncol = nx;
    *nrow = ny;

    if (bflag) {
        *x1p = x1;
        *y1p = y1;
        *x2p = x2;
        *y2p = y2;
    }

    return 1;

}  /*  end of function grd_recommend_size  */




/*
  ****************************************************************

               g r d _ x y z _ f r o m _ g r i d

  ****************************************************************

    Return x, y, z points for grid nodes.

*/

int CSWGrdUtils::grd_xyz_from_grid
                      (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                       CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                       CSW_F *x, CSW_F *y, CSW_F *z, int *npts, int maxpts)
{
    int          i, j, k, i1, n, nskip;
    CSW_F        xs, ys, yt, nval;

/*
    check obvious errors
*/
    if (!x  ||  !y  ||  !z  ||  !npts) {
        grd_set_err (4);
        return -1;
    }

    *npts = 0;
    if (ncol < 2  ||  nrow < 2) {
        grd_set_err (3);
        return -1;
    }

    if (maxpts < 1) {
        grd_set_err (5);
        return -1;
    }

    if (!grid) {
        grd_set_err (2);
        return -1;
    }

    if (x1 > WildFloat  ||  x1 < -WildFloat  ||
       y1 > WildFloat  ||  y1 < -WildFloat  ||
       x2 > WildFloat  ||  x2 < -WildFloat  ||
       y2 > WildFloat  ||  y2 < -WildFloat) {
        grd_set_err (7);
        return -1;
    }

    if (x1 >= x2  ||  y1 >= y2) {
        grd_set_err (6);
        return -1;
    }

/*
    If the program is in parameter error simulation mode,
    return success (which is not expected) here.
*/
    if (grd_simulation()) {
        return 1;
    }

/*
    make null values positive if needed
*/
    nval = nullval / 10.0f;
    if (nullval < 0.0) {
        for (i=0; i<ncol*nrow; i++) {
            if (grid[i] <= nval) {
                grid[i] = -nullval;
            }
        }
        nval = -nval;
    }

/*
    If maxpts is large enough, output every non null node.
*/
    if (maxpts >= ncol * nrow) {
        nskip = 1;
    }

/*
    Otherwise, determine the number of nodes to skip in order to nearly
    fill the x, y and z arrays without overflowing them.
*/
    else {

        nskip = 1;
        for (;;) {

            n = 0;
            for (i=0; i<nrow; i+=nskip) {
                i1 = i * ncol;
                for (j=0; j<ncol; j+=nskip) {
                    k = i1 + j;
                    if (grid[k] < nval) {
                        n++;
                        if (n > maxpts) break;
                    }
                }
                if (n > maxpts) break;
            }

            if (n <= maxpts) break;
            nskip++;

        }

    }

/*
    Calculate the grid spacings in x and y.
*/
    xs = (x2 - x1) / (CSW_F)(ncol - 1);
    ys = (y2 - y1) / (CSW_F)(nrow - 1);

/*
    Build the x, y and z arrays.  There is no need to check for
    overflow of these arrays here, because the nskip calculated
    above guarantees no overflow.
*/
    n = 0;
    for (i=0; i<nrow; i+=nskip) {
        i1 = i * ncol;
        yt = y1 + i * ys;
        for (j=0; j<ncol; j+=nskip) {
            k = i1 + j;
            if (grid[k] < nval) {
                x[n] = x1 + j * xs;
                y[n] = yt;
                z[n] = grid[k];
                n++;
            }
        }
    }

    *npts = n;

/*
    reset original null values in grid if they were negative
*/
    if (nullval < 0.0) {
        for (i=0; i<ncol*nrow; i++) {
            if (grid[i] > nval) grid[i] = nullval;
        }
    }

    return 1;

}  /*  end of function grd_xyz_from_grid  */




/*
  ****************************************************************

              g r d _ c o l l e c t _ p o i n t s

  ****************************************************************

    Reduce the number of points in a data set by averaging points that
  are near each other.

*/

int CSWGrdUtils::grd_collect_points
                       (CSW_F *x, CSW_F *y, CSW_F *z, int npts, int maxp,
                        CSW_F *xw, CSW_F *yw, CSW_F *zw, int *nw)
{
    int            icol, irow, nout, n, i, ncol, nrow, ndo, offset;
    CSW_F          xmin, ymin, xmax, ymax, aspect, xspac, yspac;
    CSW_F          fxbins[MAX_BINS*2], fybins[MAX_BINS*2], fzbins[MAX_BINS*2];
    int            ncsav, nrsav;
    int            bins[MAX_BINS*2];
    double         dcol;

/*
    Find the data limits and set up a grid
    with nearly square cells that covers the
    data x and y limits.
*/
    grd_limits_xy (x, y, npts,
                   &xmin, &ymin, &xmax, &ymax);
    if (xmin >= xmax  ||  ymin >= ymax) {
        return -1;
    }

    if (maxp > MAX_BINS) maxp = MAX_BINS;

    aspect = (ymax - ymin) / (xmax - xmin);
    dcol = (double)maxp / (double)aspect;
    ncol = (int) sqrt (dcol);
    if (ncol < 5) ncol = 5;
    nrow = maxp / ncol;
    if (nrow < 2) {
        nrow = 2;
        ncol = maxp / nrow;
    }

/*
    Average the x, y, and z values in each grid cell.  If
    the number of populated cells is too low, refine the
    grid and try again.
*/
    ndo = 0;
    for (;;) {

    /*
        Calculate spacings for cells.
    */
        xspac = (xmax - xmin) / (CSW_F)(ncol - 1);
        yspac = (ymax - ymin) / (CSW_F)(nrow - 1);
        memset (bins, 0, MAX_BINS*sizeof(int));
        memset (fxbins, 0, MAX_BINS*sizeof(CSW_F));
        memset (fybins, 0, MAX_BINS*sizeof(CSW_F));
        memset (fzbins, 0, MAX_BINS*sizeof(CSW_F));

    /*
        Loop through the data points.  Each point is located
        in a particular bin, and the point's x, y, z values are
        averaged with the values previously found in the bin.
    */
        for (i=0; i<npts; i++) {
            irow = (int) ((y[i] - ymin) / yspac);
            icol = (int) ((x[i] - xmin) / xspac);
            if (irow < 0  ||  irow > nrow-2) continue;
            if (icol < 0  ||  icol > ncol-2) continue;
            offset = irow * ncol + icol;
            n = bins[offset];
            bins[offset]++;
            fxbins[offset] = (fxbins[offset] * (CSW_F)n + x[i]) / (CSW_F)(n + 1);
            fybins[offset] = (fybins[offset] * (CSW_F)n + y[i]) / (CSW_F)(n + 1);
            fzbins[offset] = (fzbins[offset] * (CSW_F)n + z[i]) / (CSW_F)(n + 1);
        }

    /*
        Count the number of bins with values and break the
        loop if there are enough of them.
    */
        nout = 0;
        for (i=0; i<ncol*nrow; i++) {
            if (bins[i] > 0) {
                nout++;
            }
        }

        if (nout < maxp &&  nout > maxp * 3 / 4) {
            break;
        }

    /*
        Refine the grid and try again.
    */
        ncsav = ncol;
        nrsav = nrow;
        if (nout > maxp) {
            ncol -= 2;
            nrow -= 2;
            continue;
        }
        else {
            ncol = ncol * 5 / 4;
            nrow = nrow * 5 / 4;
        }

    /*
        Call the present results good enough if the
        grid has become too large or if more than 4 tries
        have been made.
    */
        if (ncol*nrow >= MAX_BINS) {
            ncol = ncsav;
            nrow = nrsav;
            break;
        }

        ndo++;
        if (ndo > 4) {
            ncol = ncsav;
            nrow = nrsav;
            break;
        }

    }

/*
    Stuff each averaged value into the output arrays.
*/
    nout = 0;
    *nw = 0;
    for (i=0; i<ncol*nrow; i++) {
        if (bins[i] > 0) {
            if (nout >= maxp) {
                return -1;
            }
            xw[nout] = fxbins[i];
            yw[nout] = fybins[i];
            zw[nout] = fzbins[i];
            nout++;
        }
    }

/*
    Return the number of points and the success status.
*/
    *nw = nout;

    return 1;

}  /*  end of function grd_collect_points  */




/*
  ****************************************************************

                 g r d _ l i m i t s _ x y

  ****************************************************************

    Return the x and y limits of a set of CSW_F points.

*/

int CSWGrdUtils::grd_limits_xy
                  (CSW_F *x, CSW_F *y, int n,
                   CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2)
{
    int       i;
    CSW_F     xmin, ymin, xmax, ymax;

    xmin = 1.e30f;
    ymin = 1.e30f;
    xmax = -1.e30f;
    ymax = -1.e30f;

    for (i=0; i<n; i++) {
        if (x[i] < xmin) xmin = x[i];
        if (x[i] > xmax) xmax = x[i];
        if (y[i] < ymin) ymin = y[i];
        if (y[i] > ymax) ymax = y[i];
    }

    *x1 = xmin;
    *y1 = ymin;
    *x2 = xmax;
    *y2 = ymax;

    return 1;

}  /*  end of function grd_limits_xy  */






/*
  ****************************************************************

                     g r d _ s e t _ e r r

  ****************************************************************

    Set the private error number.

*/

int CSWGrdUtils::grd_set_err (int val)
{

    GridErrNum = val;
    return 1;

}  /*  end of function grd_set_err  */




/*
  ****************************************************************

                 g r d _ g e t _ e r r

  ****************************************************************

    Return the private error number.

*/

int CSWGrdUtils::grd_get_err (void)
{

    return GridErrNum;

}  /*  end of function grd_GetErr  */




/*
  ****************************************************************

               g r d _ b i l i n _ i n t e r p

  ****************************************************************

    Use bilinear interpolation on surrounding grid nodes to
  estimate the elevation at a set of x, y points. If a point is
  outside of the grid, it is set to 1.e30f.

    On success, 1 is returned.  If npts is less than 1 or if the
  minimum x or y is greater than or equal to the corresponding
  maximum, -1 is returned.

*/

int CSWGrdUtils::grd_bilin_interp
                     (CSW_F *x, CSW_F *y, CSW_F *z, int npts,
                      CSW_F *gridin, int ncol, int nrow, int nskip,
                      CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax)
{
    int      i, j, k, i1, i2, onnode;
    double   x0, y0, xsp, ysp, z1, z2, z3, z4, tiny,
             zt1, zt2, xpct, ypct, xsp2, ysp2, double_z;
    CSW_F    *grid = NULL;

/*
    check for obvious errors
*/
    if (npts < 1) return -1;
    if (xmax <= xmin  ||  ymax <= ymin) return -1;

    xsp = (xmax - xmin) / (CSW_F)(ncol - 1);
    ysp = (ymax - ymin) / (CSW_F)(nrow - 1);
    xsp2 = xsp * (CSW_F)nskip;
    ysp2 = ysp * (CSW_F)nskip;

    tiny = (xsp + ysp) / 100.0f;

    grid = gridin;

/*
    loop through the points and interpolate.
    any points outside the grid are set to
    1.e30f.
*/
    for (i=0; i<npts; i++) {

    /*
        set to null value if outside the grid.
    */
        if (x[i] < xmin - tiny  ||  x[i] > xmax + tiny  ||
            y[i] < ymin - tiny  ||  y[i] > ymax + tiny) {
            z[i] = 1.e30f;
            continue;
        }

    /*
        calculate column (j) and row (k) of the lower
        left corner of the grid cell that contains the point.
    */
        onnode = FindBestColumnAndRow (
            (double)x[i], (double)y[i],
            grid, ncol, nrow, nskip,
            xmin, ymin, xsp2, ysp2,
            &j, &k);

        if (onnode) {
            z[i] = *(grid + ncol * k + j);
            continue;
        }

    /*
        get the z values at the cell corners
        z1 is lower left, z2 lower right, z3 upper left,
        and z4 upper right.
    */
        i1 = ncol * k + j;
        i2 = i1 + ncol*nskip;
        z1 = *(grid + i1);
        z2 = *(grid + i1 + nskip);
        z3 = *(grid + i2);
        z4 = *(grid + i2 + nskip);

        if (z1 > 1.e19  ||  z2 > 1.e19  ||  z3 > 1.e19  ||  z4 > 1.e19) {
            z[i] = 1.e30f;
            continue;
        }

        if (z1 < -1.e19  ||  z2 < -1.e19  ||  z3 < -1.e19  ||  z4 < -1.e19) {
            z[i] = 1.e30f;
            continue;
        }

    /*
        get percent of cell size for x and y
    */
        x0 = xmin + j * xsp;
        y0 = ymin + k * ysp;
        xpct = (x[i] - x0) / xsp2;
        ypct = (y[i] - y0) / ysp2;

    /*
        interpolate based on x and y percents
        and the corner values
    */
        zt1 = z1 + (z2 - z1) * xpct;
        zt2 = z3 + (z4 - z3) * xpct;
        double_z = zt1 + (zt2 - zt1) * ypct;

        /*
		 * Prevent floating-point invalid operation when putting a small
         * number back into the float variable.
         */
        if (-Z_ABSOLUTE_TINY < double_z && double_z < Z_ABSOLUTE_TINY) {
            double_z = 0.0;
		}
        z[i] = (CSW_F) double_z;
    }

    return 1;

}  /*  end of function grd_bilin_interp  */






/*
  ****************************************************************

                 g r d _ b i c u b _ i n t e r p

  ****************************************************************

  Interpolate a grid at a specified set of points using bicubic
  interpolation of the grid nodes in the immediate vicinity of
  each point in the set.

  This is not a public function.  The correct behavior of this
  function depends upon the grid geometry previously set up.

*/

int CSWGrdUtils::grd_bicub_interp
                     (CSW_F *xpts, CSW_F *ypts, CSW_F *zpts, int npts, CSW_F nullval,
                      CSW_F *gridin, int ncol, int nrow, int nskip,
                      CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax,
                      int irforce, int icforce)
{
    const static double  wt[] =
       {
        1.0, 0.0, -3.0, 2.0, 0.0, 0.0, 0.0, 0.0, -3.0, 0.0, 9.0, -6.0, 2.0, 0.0, -6.0, 4.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, 0.0, -9.0, 6.0, -2.0, 0.0, 6.0, -4.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 9.0, -6.0, 0.0, 0.0, -6.0, 4.0,
        0.0, 0.0, 3.0, -2.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -9.0, 6.0, 0.0, 0.0, 6.0, -4.0,
        0.0, 0.0, 0.0, 0.0, 1.0, 0.0, -3.0, 2.0, -2.0, 0.0, 6.0, -4.0, 1.0, 0.0, -3.0, 2.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 3.0, -2.0, 1.0, 0.0, -3.0, 2.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -3.0, 2.0, 0.0, 0.0, 3.0, -2.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, -2.0, 0.0, 0.0, -6.0, 4.0, 0.0, 0.0, 3.0, -2.0,
        0.0, 1.0, -2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, -3.0, 6.0, -3.0, 0.0, 2.0, -4.0, 2.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, -6.0, 3.0, 0.0, -2.0, 4.0, -2.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -3.0, 3.0, 0.0, 0.0, 2.0, -2.0,
        0.0, 0.0, -1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 3.0, -3.0, 0.0, 0.0, -2.0, 2.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 1.0, -2.0, 1.0, 0.0, -2.0, 4.0, -2.0, 0.0, 1.0, -2.0, 1.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 2.0, -1.0, 0.0, 1.0, -2.0, 1.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, -1.0, 0.0, 0.0, -1.0, 1.0,
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 1.0, 0.0, 0.0, 2.0, -2.0, 0.0, 0.0, -1.0, 1.0
       };

    int               i, j, irow, irow1, irow2, iclast, irlast,
                      jcol, ido, nr, ns2, onnode;
    double            y[4], y1[4], y2[4], y12[4];
    double            yt11, yt21, yt31, yt41,
                      xt11, xt21, xt31, xt41,
                      yc1, yc2, yc3, yc4;
    double            x[16], xx, c[16], tiny,
                      x0, y0, ansy, t, u, xspac1, yspac1, xspac2, yspac2;
    CSW_F             *grid = NULL;

    xspac1 = (double)(xmax - xmin) / (double)(ncol - 1);
    yspac1 = (double)(ymax - ymin) / (double)(nrow - 1);
    xspac2 = xspac1 * (double)nskip;
    yspac2 = yspac1 * (double)nskip;

    if (nullval < 1.e18f) nullval = 1.e18f;


/*
 * Any point that is inside two grid spacings from the edge
 * of the grid will be interpolated.  Points outside of this
 * margin will be set to 1.e30f
 */
    tiny = xspac1 + yspac1;

    nr = nskip * ncol;
    ns2 = nskip * 2;

    iclast = -1000;
    irlast = -1000;

/*
 * Special vftest debug stuff.
 */
    if (ncol == 4  &&  nrow == 4) {
        CSW_F             z1, z2;
        z1 = 1.e30f;
        z2 = -1.e30f;
        for (i=0; i<16; i++) {
            if (gridin[i] < z1) z1 = gridin[i];
            if (gridin[i] > z2) z2 = gridin[i];
        }
        if (z2 - z1 > 800) {
            z2 = z1;
        }
    }

/*
 * suppress warning about the c array possibly not initialized.
 */
    memset (c, 0, 16 * sizeof(double));

    grid = gridin;

    for (ido = 0; ido < npts; ido++) {

    /*
        set to null value if outside the grid.
    */
        if (xpts[ido] < xmin - tiny  ||  xpts[ido] > xmax + tiny  ||
            ypts[ido] < ymin - tiny  ||  ypts[ido] > ymax + tiny) {
            zpts[ido] = 1.e30f;
            continue;
        }

        if (ncol < 4  ||  nrow < 4) {
            goto DO_BILIN_INSTEAD;
        }

    /*
        find the column and row of the grid
        that is an integer multiple of nskip
        jcol = (int) ((xpts[ido] - xmin) / xspac2);
        jcol *= nskip;
        irow = (int) ((ypts[ido] - ymin) / yspac2);
        irow *= nskip;
    */

    /*
        use specified row and col if needed
    */
        if (irforce >= 0  &&  icforce >= 0) {
            irow = irforce;
            jcol = icforce;

        } else {
            onnode = FindBestColumnAndRow (
                (double)xpts[ido], (double)ypts[ido],
                grid, ncol, nrow, nskip,
                xmin, ymin, xspac2, yspac2,
                &jcol, &irow);

            if (onnode) {
                zpts[ido] = *(grid + irow * ncol + jcol);
                continue;
            }
        }

        if (jcol == iclast  &&  irow == irlast) {
            goto EVAL_AT_POINT;
        }

        irow1 = irow * ncol + jcol;
        irow2 = irow1 + nr;

    /*
        save corner values of skipped grid in array y
    */
        y[0] = (double)*(grid + irow1);
        y[1] = (double)*(grid + irow1 + nskip);
        y[2] = (double)*(grid + irow2 + nskip);
        y[3] = (double)*(grid + irow2);

        if (y[0] > (double)nullval  ||  y[1] > (double)nullval  ||
            y[2] > (double)nullval  ||  y[3] > (double)nullval) {
            zpts[ido] = 1.e30f;
            continue;
        }

        if (y[0] < (double)-nullval  ||  y[1] < (double)-nullval  ||
            y[2] < (double)-nullval  ||  y[3] < (double)-nullval) {
            zpts[ido] = 1.e30f;
            continue;
        }

    /*
        points for the y partial derivatives
    */
        if (irow >= nskip   &&  irow < nrow - ns2) {
            yt11 = (double)*(grid + irow1 - nr);
            yt21 = (double)*(grid + irow1 - nr + nskip);
            yt31 = (double)*(grid + irow2 + nr + nskip);
            yt41 = (double)*(grid + irow2 + nr);
        }

        else if (irow < nskip) {
            yt11 = 2.0 * y[0] - y[3];
            yt21 = 2.0 * y[1] - y[2];
            yt31 = (double)*(grid + irow2 + nr + nskip);
            yt41 = (double)*(grid + irow2 + nr);
        }

        else {
            yt11 = (double)*(grid + irow1 - nr);
            yt21 = (double)*(grid + irow1 - nr + nskip);
            yt31 = 2.0 * y[2] - y[1];
            yt41 = 2.0 * y[3] - y[0];
        }

        if (yt11 > (double)nullval  ||  yt21 > (double)nullval  ||
            yt31 > (double)nullval  ||  yt41 > (double)nullval) {
            goto DO_BILIN_INSTEAD;
        }

        if (yt11 < (double)-nullval  ||  yt21 < (double)-nullval  ||
            yt31 < (double)-nullval  ||  yt41 < (double)-nullval) {
            goto DO_BILIN_INSTEAD;
        }

    /*
        points for the x partial derivatives
    */
        if (jcol >= nskip  &&  jcol < ncol - ns2) {
            xt11 = (double)*(grid + irow1 - nskip);
            xt21 = (double)*(grid + irow1 + ns2);
            xt31 = (double)*(grid + irow2 + ns2);
            xt41 = (double)*(grid + irow2 - nskip);
        }

        else if (jcol < nskip) {
            xt11 = 2.0 * y[0] - y[1];
            xt21 = (double)*(grid + irow1 + ns2);
            xt31 = (double)*(grid + irow2 + ns2);
            xt41 = 2.0 * y[3] - y[2];
        }

        else {
            xt11 = (double)*(grid + irow1 - nskip);
            xt21 = 2.0 * y[1] - y[0];
            xt31 = 2.0 * y[2] - y[3];
            xt41 = (double)*(grid + irow2 - nskip);
        }

        if (xt11 > (double)nullval  ||  xt21 > (double)nullval  ||
            xt31 > (double)nullval  ||  xt41 > (double)nullval) {
            goto DO_BILIN_INSTEAD;
        }

        if (xt11 < (double)-nullval  ||  xt21 < (double)-nullval  ||
            xt31 < (double)-nullval  ||  xt41 < (double)-nullval) {
            goto DO_BILIN_INSTEAD;
        }

    /*
        points for the cross derivatives
    */
        if (irow >= nskip  &&  irow < nrow - ns2) {
            if (jcol >= nskip) {
                yc1 = *(grid + irow1 - nr - nskip);
                yc4 = *(grid + irow2 + nr - nskip);
            }
            else {
                yc1 = 2.f * y[0] - y[2];
                yc4 = 2.f * y[3] - y[1];
            }
            if (jcol < ncol - ns2) {
                yc2 = *(grid + irow1 - nr + ns2);
                yc3 = *(grid + irow2 + nr + ns2);
            }
            else {
                yc2 = 2.f * y[1] - y[3];
                yc3 = 2.f * y[2] - y[0];
            }
        }

        else if (irow < nskip) {
            yc1 = 2.f * y[0] - y[2];
            yc2 = 2.f * y[1] - y[3];
            if (jcol >= nskip) {
                yc4 = *(grid + irow2 + nr - nskip);
            }
            else {
                yc4 = 2.f * y[3] - y[1];
            }
            if (jcol < ncol - ns2) {
                yc3 = *(grid + irow2 + nr + ns2);
            }
            else {
                yc3 = 2.f * y[2] - y[0];
            }
        }

        else {
            yc4 = 2.f * y[3] - y[1];
            yc3 = 2.f * y[2] - y[0];
            if (jcol >= nskip) {
                yc1 = *(grid + irow1 - nr - nskip);
            }
            else {
                yc1 = 2.f * y[0] - y[2];
            }
            if (jcol < ncol - ns2) {
                yc2 = *(grid + irow1 - nr + ns2);
            }
            else {
                yc2 = 2.f * y[1] - y[3];
            }
        }

        if (yc1 > nullval  ||  yc2 > nullval  ||
            yc3 > nullval  ||  yc4 > nullval) {
            goto DO_BILIN_INSTEAD;
        }

        if (yc1 < -nullval  ||  yc2 < -nullval  ||
            yc3 < -nullval  ||  yc4 < -nullval) {
            goto DO_BILIN_INSTEAD;
        }

    /*
        Calculate the partial derivatives.
    */
        y1[0] = (y[1] - xt11) / 2.0;
        y1[1] = (xt21 - y[0]) / 2.0;
        y1[2] = (xt31 - y[3]) / 2.0;
        y1[3] = (y[2] - xt41) / 2.0;

        y2[0] = (y[3] - yt11) / 2.0;
        y2[1] = (y[2] - yt21) / 2.0;
        y2[2] = (yt31 - y[1]) / 2.0;
        y2[3] = (yt41 - y[0]) / 2.0;

        y12[0] = (y[2] - yt21 - xt41 + yc1) / 2.0;
        y12[1] = (xt31 - yc2 - y[3] + yt11) / 2.0;
        y12[2] = (yc3 - xt21 - yt41 + y[0]) / 2.0;
        y12[3] = (yt31 - y[1] - yc4 + xt11) / 2.0;

    /*
        Calculate the coefficients.
    */
        for (i=0; i<4; i++) {
            x[i] = y[i];
            x[i+4] = y1[i];
            x[i+8] = y2[i];
            x[i+12] = y12[i];
        }

        for (i=0; i<16; i++) {
            xx = 0.0f;
            for (j=0; j<16; j++) {
                xx += *(wt + 16*j + i) * x[j];
            }
            c[i] = xx;
        }

        goto EVAL_AT_POINT;

    /*
        evaluate the coefficients at this point
    */
        EVAL_AT_POINT:

        x0 = jcol * xspac1 + xmin;
        y0 = irow * yspac1 + ymin;
        u = (xpts[ido] - x0) / xspac2;
        t = (ypts[ido] - y0) / yspac2;
        ansy = 0.0f;

        for (i=3; i>= 0; i--) {
            ansy = t * ansy + ((c[i+12] * u + c[i+8]) * u + c[i+4]) * u + c[i];
        }

        if (-Z_ABSOLUTE_TINY < ansy && ansy < Z_ABSOLUTE_TINY) {
            ansy = 0.0;
        }

        zpts[ido] = (CSW_F)ansy;

        iclast = jcol;
        irlast = irow;

        continue;

    /*
        There is a null node inside one of the operators, so
        do a bilinear interpolation instead of a bicubic.
    */
        DO_BILIN_INSTEAD:


        grd_bilin_interp (xpts+ido, ypts+ido, zpts+ido, 1,
                          grid, ncol, nrow, nskip,
                          xmin, ymin, xmax, ymax);
        continue;

    }

    return 1;


}  /*  end of function grd_bicub_interp  */






/*
  ****************************************************************

               g r d _ c a l c _ p l a n e

  ****************************************************************

    Return the coefficients of the best fitting plane to a small
  set of points.

*/

int CSWGrdUtils::grd_calc_plane
           (CSW_F *x, CSW_F *y, CSW_F *z, int nptsin, CSW_F *coef)
{
    double          a[9], b[3], big, pivinv, dum, z1, z2,
                    xbar[MAX_PLANE],
                    ybar[MAX_PLANE],
                    zbar[MAX_PLANE],
                    x1m, x2m, y1m, y2m,
                    s1, s2, s3, s4, s5, s6, s7, s8,
                    temp, dachk;
    int             ipiv[3], i, j, k,
                    irow, icol, npts, n;

    if (nptsin < 3) {
        return -1;
    }

/*
    Find data limits and convert to local double arrays.
*/
    npts = nptsin;
    if (npts > MAX_PLANE) npts = MAX_PLANE;

    x1m = 1.e30;
    y1m = 1.e30;
    x2m = -1.e30;
    y2m = -1.e30;
    z1 = 1.e30;
    z2 = -1.e30;

    for (i=0; i<npts; i++) {
        if ((double)x[i] < x1m) x1m = (double)x[i];
        if ((double)x[i] > x2m) x2m = (double)x[i];
        if ((double)y[i] < y1m) y1m = (double)y[i];
        if ((double)y[i] > y2m) y2m = (double)y[i];
        if ((double)z[i] < z1) z1 = (double)z[i];
        if ((double)z[i] > z2) z2 = (double)z[i];
        xbar[i] = (double)x[i];
        ybar[i] = (double)y[i];
        zbar[i] = (double)z[i];
    }

/*
    Special case for an essentially flat horizontal data input.
*/
    coef[0] = (CSW_F)((z1 + z2) / 2.0);
    coef[1] = 0.0;
    coef[2] = 0.0;
    if (z2 - z1 < Z_ABSOLUTE_TINY) {
        return 1;
    }

/*
    check if data is obviously bad
*/
    temp = x2m - x1m;
    if (y2m - y1m < temp) temp = y2m - y1m;
    dachk = 1.e-9 * temp;
    if (dachk < 1.e-18) dachk = 1.e-18;

    s1 = x2m - x1m;
    s2 = y2m - y1m;
    if (s2 > s1) s1 = s2;
    if (s2 < dachk) {
        return -1;
    }

    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = 0.0;

/*
    set up for gaussian elimination
*/
    for (i=0; i<npts; i++) {
        s1 += xbar[i];
        s2 += ybar[i];
        s3 += xbar[i] * xbar[i];
        s4 += xbar[i] * ybar[i];
        s5 += ybar[i] * ybar[i];
        s6 += zbar[i];
        s7 += xbar[i] * zbar[i];
        s8 += ybar[i] * zbar[i];
    }

    a[0] = (double) npts;
    a[1] = s1;
    a[2] = s2;
    a[3] = s1;
    a[4] = s3;
    a[5] = s4;
    a[6] = s2;
    a[7] = s4;
    a[8] = s5;

    b[0] = s6;
    b[1] = s7;
    b[2] = s8;

    n = 3;

    for (i=0; i<n; i++) {
        ipiv[i] = 0;
    }

    icol = 0;
    irow = 0;

/*
    gaussian elimination
*/
    for (i=0; i<n; i++) {

        big = 0.0;

        for (j=0; j<n; j++) {

            if (ipiv[j] != 1) {
                for (k=0; k<n; k++) {
                    if (ipiv[k] == 0) {
                        temp = *(a + 3*k + j);
                        if (temp < 0.0) temp = -temp;
                        if (temp >= big) {
                            big = *(a + 3*k + j);
                            if (big < 0.0) big = -big;
                            irow = j;
                            icol = k;
                        }
                    }
                    else if (ipiv[k] > 1) {
                        grd_set_err (99);
                        return -1;
                    }
                }
            }
        }

        ipiv[icol]++;

        if (irow != icol) {
            for (j=0; j<n; j++) {
                dum = *(a + irow + 3*j);
                *(a + irow + 3*j) = *(a + icol + 3*j);
                *(a + icol + 3*j) = dum;
            }
            dum = b[irow];
            b[irow] = b[icol];
            b[icol] = dum;
        }

        dum = *(a + 4*icol);
        if (dum < 0.0) dum = -dum;
        if (dum < dachk) {
            grd_set_err (99);
            return -1;
        }

        pivinv = 1.0 / (*(a + 4*icol));
        *(a + 4*icol) = 1.0;

        for (j=0; j<n; j++) {
            *(a + icol + 3*j) *= pivinv;
        }
        b[icol] *= pivinv;

        for (j=0; j<n; j++) {
            if (j != icol) {
                dum = *(a + 3*icol + j);
                *(a + 3*icol + j) = 0.;
                for (k=0; k<n; k++) {
                    *(a + 3*k + j) -= *(a + icol + 3*k) * dum;
                }
                b[j] -= b[icol] * dum;
            }
        }

    }  /*  end of i loop and gaussian elimination  */

/*
    convert coefs to CSW_F and return
*/
    for (i=0; i<3; i++) {
        coef[i] = (CSW_F)b[i];
    }

    return 1;

}  /*  end of function grd_calc_plane  */






/*
  ****************************************************************

              g r d _ e x p a n d _ l i m i t s

  ****************************************************************

    Expand a grid in all four directions to make the number of columns
  and rows an even multiple of the coarse grid interval.

*/

int CSWGrdUtils::grd_expand_limits
                      (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                       int ncol, int nrow, int ncoarse, int margin,
                       CSW_F *xx1, CSW_F *yy1, CSW_F *xx2, CSW_F *yy2,
                       int *ncol2, int *nrow2, int *col0, int *row0)
{
    CSW_F         xsp, ysp;
    int           n, nc0, nr0, nn;

    nc0 = ncol;
    nr0 = nrow;

    xsp = (x2 - x1) / (ncol - 1);
    ysp = (y2 - y1) / (nrow - 1);

    nn = margin / ncoarse + 1;
    if (nn < 2) nn = 2;
    if (margin < 0) nn = 5;

    n = ncol / ncoarse;
    ncol = (n + nn) * ncoarse + 1;
    n = nrow / ncoarse;
    nrow = (n + nn) * ncoarse + 1;

    *col0 = (ncol - nc0) / 2;
    *row0 = (nrow - nr0) / 2;

    *xx1 = x1 - xsp * *col0;
    *yy1 = y1 - ysp * *row0;

    *xx2 = *xx1 + (ncol - 1) * xsp;
    *yy2 = *yy1 + (nrow - 1) * ysp;

    *ncol2 = ncol;
    *nrow2 = nrow;

    return 1;

}  /*  end of function grd_expand_limits  */




/*
  ****************************************************************

                    g r d _ s i m u l a t i o n

  ****************************************************************

    If the _ERROR_SIM_MODE environment variable is set to 1, return 1.
  Otherwise return zero.  This is used when simulating bad parameters
  passed to functions.

*/

int CSWGrdUtils::grd_simulation (void)
{
    char  *cenv;
    int   first = 1;

    if (first) {
        cenv = (char *)csw_getenv ("_ERROR_SIM_MODE");
        first = 0;
    }
    if (cenv) {
        if (cenv[0] == '1') return 1;
    }

    return 0;

}  /*  end of function grd_simulation  */




/*
  ****************************************************************

                 g r d _ s t e p _ i n t e r p

  ****************************************************************

    Interpolate a grid at a set of x,y points using the value of
  the closest grid node to each point.

*/

int CSWGrdUtils::grd_step_interp
                    (CSW_F *xloc, CSW_F *yloc, CSW_F *zloc, int nloc,
                     CSW_F *grid, int ncol, int nrow,
                     CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2)
{
    int           i, k, irow, jcol;
    CSW_F         x, y, xsp, ysp, z1, z2, z3, z4, dist, dt, x0, y0;

/*
    Calculate grid spacing.
*/
    xsp = (x2 - x1) / (CSW_F)(ncol - 1);
    ysp = (y2 - y1) / (CSW_F)(nrow - 1);

/*
    Loop through the points.  For each point determine what
    grid node is closest and use the value at that node as
    the value at the point.
*/
    for (i=0; i<nloc; i++) {

        x = xloc[i];
        y = yloc[i];

        jcol = (int)((x - x1) / xsp);
        irow = (int)((y - y1) / ysp);
        if (jcol < 0) jcol = 0;
        if (jcol > ncol - 2) jcol = ncol - 2;
        if (irow < 0) irow = 0;
        if (irow > nrow - 2) irow = nrow - 2;

        x0 = x1 + xsp * jcol;
        y0 = y1 + ysp * irow;

        k = ncol * irow + jcol;
        z1 = grid[k];
        z2 = grid[k+1];
        z3 = grid[k+ncol+1];
        z4 = grid[k+ncol];

        dist = 1.e30f;
        x -= x0;
        y -= y0;
        dt = x * x + y * y;
        if (dt < dist) {
            zloc[i] = z1;
            dist = dt;
        }

        x -= xsp;
        dt = x * x + y * y;
        if (dt < dist) {
            zloc[i] = z2;
            dist = dt;
        }

        y -= ysp;
        dt = x * x + y * y;
        if (dt < dist) {
            zloc[i] = z3;
            dist = dt;
        }

        x += xsp;
        dt = x * x + y * y;
        if (dt < dist) {
            zloc[i] = z4;
            dist = dt;
        }

    }

    return 1;

}  /*  end of function grd_step_interp  */







/*
  ****************************************************************

      g r d _ i n v e r s e _ d i s t a n c e _ a v e r a g e

  ****************************************************************

    Calculate an inverse distance to the dpower weighted average of the
  points specified.  The points must be origined at the grid node.
  If the dsq pointer is not NULL, return the individual squared
  distances in it.  Most times the status returned is 1, but if
  one of the data points is almost directly on top of the node,
  a return of 999 is made.

*/

int CSWGrdUtils::grd_inverse_distance_average
                          (CSW_F *x, CSW_F *y, CSW_F *z, int n, int dpower,
                           CSW_F tiny, CSW_F *dsq, CSW_F *avg)
{
    int         i, retval;
    CSW_F       sum1, sum2, dt, dt0, dt02, tiny2;

    retval = 1;

    tiny /= 20.0f;
    tiny2 = tiny / 5.0f;
    sum1 = sum2 = 0.0f;
    tiny *= tiny;
    tiny2 *= tiny2;
    dt0 = tiny;
    dt02 = tiny2;
    if (dpower > 2) tiny *= tiny;
    if (dpower > 4) tiny *= dt0;
    if (dpower > 2) tiny2 *= tiny2;
    if (dpower > 4) tiny *= dt02;

    if (tiny2 < 0.0) {
        tiny2 = 0.0;
    }

    for (i=0; i<n; i++) {
        dt = x[i] * x[i] + y[i] * y[i];
        dt0 = dt;
        if (dpower > 2) {
            if (dt0 < 1.e15f)
                dt *= dt;
            else
                dt = 1.e30f;
        }
        if (dpower > 4) {
            if (dt0 < 1.e10f)
                dt *= dt0;
            else
                dt = 1.e30f;
        }
        if (dt <= tiny2) {
            *avg = z[i];
            return 999;
        }
        if (dt < tiny) {
            dt = tiny;
            retval = 999;
        }
        if (dsq) dsq[i] = dt;
        dt = 1.0f / dt;
        sum1 += dt;
        sum2 += dt * z[i];
    }

    *avg = sum2 / sum1;

    return retval;

}  /*  end of function grd_inverse_distance_average  */





/*
  *****************************************************************

                g r d _ c o l i n e a r _ c h e c k

  *****************************************************************

    Return 1 if the specified points are nearly colinear or return
  zero if not.

*/

int CSWGrdUtils::grd_colinear_check
               (CSW_F *x, CSW_F *y, int npts, CSW_F tiny, CSW_F *distmax)
{
    int           i, good;
    CSW_F         xt, yt, x1, y1, x2, y2, dt, dmax;
    double        d1;

    if (distmax) {
        *distmax = 0.0f;
    }

/*
    Check obvious errors.
*/
    if (npts < 3) return 1;
    if (tiny <= 0.0f) return 0;

/*
    Find two points further than tiny apart to define a trial line.
    Each point is then checked to see if it lies on or very
    close to the line.
*/
    x1 = x[0];
    y1 = y[0];
    x2 = x1;
    y2 = y1;

    good = 0;
    for (i=1; i<npts; i++) {
        xt = x[i] - x1;
        yt = y[i] - y1;
        dt = xt * xt + yt * yt;
        dt = (CSW_F)sqrt ((double)dt);
        if (dt > tiny) {
            x2 = x[i];
            y2 = y[i];
            good = 1;
            break;
        }
    }

    if (good == 0) {
        return 1;
    }


/*
    If any points have a perpendicular distance greater
    than the tiny tolerance, the set is not colinear.
*/
    dmax = 0.0f;
    for (i=1; i<npts; i++) {
        gpf_perpdsquare (x1, y1, x2, y2, x[i], y[i], &dt);
        if (dt > tiny  &&  distmax == NULL) {
            return 0;
        }
        if (dt > dmax) dmax = dt;
    }

    if (distmax) {
        d1 = (double)dmax;
        d1 = sqrt(d1);
        if (d1 < Z_ABSOLUTE_TINY) {
            d1 = 0.0;
        }
        *distmax = (CSW_F) d1;
    }

    if (dmax > tiny / 4.0) return 0;

    return 1;

}  /*  end of grd_colinear_check function  */



/*
  ****************************************************************

                   g r d _ f l a t _ c h e c k

  ****************************************************************

    Check if the z data are nearly all at the same value.

*/

int CSWGrdUtils::grd_flat_check
                   (CSW_F *x, CSW_F *y, CSW_F *z, int n,
                    CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax,
                    CSW_F zrange)
{
    CSW_F        x1, y1, x2, y2, z1, z2, a1, a2, zcrit;
    int          i;

/*
    Find the data limits in x, y, and z.
*/
    x1 = 1.e30f;
    y1 = 1.e30f;
    x2 = -1.e30f;
    y2 = -1.e30f;
    z1 = 1.e30f;
    z2 = -1.e30f;
    for (i=0; i<n; i++) {
        if (x[i] < x1) x1 = x[i];
        if (x[i] > x2) x2 = x[i];
        if (y[i] < y1) y1 = y[i];
        if (y[i] > y2) y2 = y[i];
        if (z[i] < z1) z1 = z[i];
        if (z[i] > z2) z2 = z[i];
    }

    if (z1 - z2 == 0.0) return 1;

/*
    The permitted z range for a flat set is bigger
    if the local area is large relative to the entire data area.
*/
    z1 = z2 - z1;
    a1 = (x2 - x1) * (y2 - y1);
    a2 = (xmax - xmin) * (ymax - ymin);

/*
    If the local area is nearly zero, the local z range
    must be extremely small to flag as flat.
*/
    if (a1 <= a2 / 1000.0f) {
        if (z1 < zrange / 100000.0f) {
            return 1;
        }
        else {
            return 0;
        }
    }

/*
    For a local area as large as the whole area, a local
    delta z of 1/100th of the total range is considered flat.
    For smaller areas, smaller delta z's are needed.
*/
    zcrit = zrange / 100.0f;
    a1 /= a2;
    zcrit *= a1;
    if (z1 <= zcrit) {
        return 1;
    }

    return 0;


}  /*  end of private FlatCheck function  */




/*
  ****************************************************************

           g r d _ c a l c _ c o l i n e a r _ p l a n e

  ****************************************************************

    This function is called when the complete local data set is
  colinear and therefore cannot be fit with a plane.  This condition
  is nearly always caused by the data being in straight profiles.
  I make the assumption that the data are in profiles in this function.
  Every point is projected in x and y to a point perpendicular
  to the common line at a distance equal to the average spacing
  between adjacent points.  The new points are appended to the arrays
  and a new plane is calculated.

    This algorithm assumes that the surface strikes perpendicular to
  the colinear data at each point.

*/

int CSWGrdUtils::grd_calc_colinear_plane
                            (CSW_F *x, CSW_F *y, CSW_F *z, int n,
                             CSW_F xspace, CSW_F yspace, CSW_F *value)
{
    int          i, istat;
    CSW_F        s1, s2, dx, dy, coef[10];
    CSW_F        tiny;

    xspace *= 2.f;
    yspace *= 2.f;

    tiny = (xspace + yspace) / 80.0f;

/*
    Get the slope of the line.
*/
    dx = x[n-1] - x[0];
    dy = y[n-1] - y[0];

/*
    Return error if all points are essentially coincident.
*/
    if (dx < tiny  &&  dx > -tiny  &&  dy < tiny  &&  dy < -tiny) {
        *value = 1.e30f;
        return -1;
    }

/*
    Set slopes for horizontal and vertical lines.
*/
    if (dx < tiny  &&  dx > -tiny) {
        s1 = 100000.f;
    }
    else if (dy < tiny  &&  dy > -tiny) {
        s1 = 0.0f;
    }
    else {
        s1 = dy / dx;
    }

    dx /= 2.0f;
    dy /= 2.0f;

/*
    near vertical line
*/
    if (s1 > 10000.f  ||  s1 < -10000.f) {
        for (i=0; i<n; i++) {
            x[n+i] = x[i] + xspace;
            y[n+i] = y[i];
        }
    }

/*
    near horizontal line
*/
    else if (s1 < .0001f  &&  s1 > -.0001f) {
        for (i=0; i<n; i++) {
            y[n+i] = y[i] + yspace;
            x[n+i] = x[i];
        }
    }

/*
    general line
*/
    else {
        s2 = -1.0f / s1;
        if (s2 > 1.0f  ||  s2 < -1.0f) {
            dx = dy / s2;
        }
        else {
            dy = s2 * dx;
        }
        if (dx > dy) {
            s1 = xspace / dx;
            dx = xspace;
            dy *= s1;
        }
        else {
            s1 = yspace / dy;
            dy = yspace;
            dx *= s1;
        }
        for (i=0; i<n; i++) {
            x[n+i] = x[i] + dx;
            y[n+i] = y[i] + dy;
        }
    }

    for (i=0; i<n; i++) {
        z[n+i] = z[i];
    }

/*
    calculate a plane using the new x and y locations
*/
    istat = grd_calc_plane (x, y, z, n*2, coef);
    if (istat == -1) {
        *value = 1.e30f;
        return -1;
    }

    *value = coef[0];

    return 1;

}  /*  end of grd_calc_colinear_plane function  */





/*
  ****************************************************************

                g r d _ a v e r a g e _ a n g l e s

  ****************************************************************

    Average angle values that lie between 0 and 180 degrees.  The
  method corrects for angles near 180 and zero in the same set of
  values.

*/

int CSWGrdUtils::grd_average_angles
                       (CSW_F *angles, int nang,
                        CSW_F *avgout, CSW_F *spread)
{
    int         n1, n2, i;
    CSW_F       avg, min1, max1, min2, max2,
                ang, d3;

/*
    handle trivial cases of zero and 1 point.
*/
    if (nang < 1) {
        *avgout = 0.0f;
        return -1;
    }

    if (nang == 1) {
        *avgout = angles[0];
        return 1;
    }

/*
    find the limits of values less than 90 and greater than 90.
*/
    min1 = 1.e30f;
    max1 = -1.e30f;
    min2 = 1.e30f;
    max2 = -1.e30f;
    avg = 0.0f;
    n1 = 0;
    n2 = 0;

    for (i=0; i<nang; i++) {

        ang = angles[i];
        avg += ang;
        if (ang < 90.0f) {
            if (ang < min1) min1 = ang;
            if (ang > max2) max2 = ang;
            n1++;
        }
        else {
            if (ang < min2) min2 = ang;
            if (ang > max2) max2 = ang;
            n2++;
        }

    }

/*
    If all angles are less than or all angles are greater than
    90, then the simple arithmetic mean is output.
*/
    avg /= (CSW_F)nang;
    if (n1 < 1  ||  n2 < 1) {
        *avgout = avg;
        if (n1 < 1) {
            *spread = (max2 - min2) / 30.0f;
        }
        else {
            *spread = (max1 - min1) / 30.0f;
        }
        if (*spread < 1.0f) *spread = 1.0f;
        return 1;
    }

/*
    If the gap between the two sets of data is less than 100,
    then output the arithmetic mean.
*/
    d3 = min2 - max1;
    if (d3 < 100) {
        *avgout = avg;
        if (n1 < 1) {
            *spread = (max2 - min2) / 30.0f;
        }
        else {
            *spread = (max1 - min1) / 30.0f;
        }
        if (*spread < 1.0f) *spread = 1.0f;
        return 1;
    }

/*
    If the gap is greater than 100, add 180 to angles
    less than 90 and then average all angles.
*/
    avg = 0.0f;
    min1 = 1.e30f;
    max1 = 1.e30f;
    for (i=0; i<nang; i++) {
        ang = angles[i];
        if (ang < 90) {
            ang += 180.0f;
        }
        avg += ang;
        if (ang < min1) min1 = ang;
        if (ang > max1) max1 = ang;
    }

    avg /= (CSW_F)nang;
    if (avg > 180.0f) {
        avg -= 180.0f;
    }
    if (avg < 0.0f) {
        avg += 180.0f;
    }

    *avgout = avg;
    *spread = (max1 - min1) / 30.0f;
    if (*spread < 1.0f) *spread = 1.0f;

    return 1;

}  /*  end of grd_average_angles function  */




/*
  ****************************************************************

       g r d _ g e t _ c l o s e s t _ p o i n t _ v a l u e

  ****************************************************************

    Return the value of the closest point in the local data set.
    The x and y point locations are relative to the local origin
    hat you are searching around.

*/

int CSWGrdUtils::grd_get_closest_point_value
                                (CSW_F *x, CSW_F *y, CSW_F *z, int n,
                                 CSW_F *val)
{
    int         i;
    CSW_F       dt, tiny;

    tiny = 1.e30f;
    *val = 1.e30f;
    for (i=0; i<n; i++) {
        dt = x[i] * x[i] + y[i] * y[i];
        if (dt < tiny) {
            tiny = dt;
            *val = z[i];
        }
    }

    if (*val > 1.e20f) {
        return -1;
    }

    return 1;

}  /*  end of function grd_get_closest_point_value  */





/*
  ****************************************************************

                g r d _ c o n s t a n t _ g r i d

  ****************************************************************

    Fill in the grid with a constant and make all mask values zero.

*/

int CSWGrdUtils::grd_constant_grid
                             (CSW_F *grid, char *mask, int ncol, int nrow,
                              CSW_F val)
{
    int          i;

    for (i=0; i<ncol*nrow; i++) {
        grid[i] = val;
    }

    if (mask) {
        memset (mask, 0, ncol*nrow*sizeof(char));
    }

    return 1;

}  /*  end of function grd_constant_grid  */





/*
  ****************************************************************************

               g r d _ t r i a n g l e s _ f r o m _ g r i d

  ****************************************************************************

    Calculate 3D triangles from a grid.

*/

int CSWGrdUtils::grd_triangles_from_grid
                            (CSW_F *gridin, int ncol, int nrow,
                             CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                             POint3D **points, int *num_points,
                             TRiangleIndex   **triangles, int *num_triangles)
{
    int              i, j, k, offset, n, ntot, nc,
                     npts, i1, i2, i3, i4, *igrid;
    CSW_F            yg1, zt, xspace, yspace, *grid;
    TRiangleIndex    *tri;
    POint3D          *pts;

    if (triangles == NULL  ||  num_triangles == NULL  ||
        points == NULL  ||  num_points == NULL) {
        grd_set_err (5);
        return -1;
    }

    *points = NULL;
    *num_points = 0;
    *triangles = NULL;
    *num_triangles = 0;

/*
    Check obvious errors.
*/
    if (gridin == NULL) {
        grd_set_err (2);
        return -1;
    }

    if (ncol < 2  ||  nrow < 2  ||
        ncol > WildInteger  ||  nrow > WildInteger) {
        grd_set_err (3);
        return -1;
    }

    if (x1 >= x2  ||  y1 >= y2) {
        grd_set_err (4);
        return -1;
    }

/*
    Since null values in the grid may need to be modified,
    a copy of the grid is allocated and used for the
    triangle calculation.
*/
    ntot = ncol * nrow;
    grid = (CSW_F*)csw_Malloc (ntot * sizeof(CSW_F));
    if (!grid) {
        grd_set_err (1);
        return -1;
    }
    memcpy (grid, gridin, ntot * sizeof(CSW_F));

/*
    An array of ints as large as the grid is used to
    store the point number of each grid node.  If the
    grid node is null, its point number is -1.
*/
    igrid = (int *)csw_Malloc (ntot * sizeof(int));
    if (!igrid) {
        csw_Free (grid);
        grd_set_err (1);
        return -1;
    }

/*
    Count the non null grid nodes.  Allocate twice as many
    triangle structures as non null grid nodes and exactly
    as many point structures as non null grid nodes.  If any
    null value is less than zero, negate it.
*/
    n = 0;
    for (i=0; i<ntot; i++) {
        zt = grid[i];
        if (zt >= 1.e20f  ||  zt <= -1.e20f) {
            if (zt <= -1.e20f) grid[i] = -zt;
            continue;
        }
        n++;
    }

    tri = (TRiangleIndex   *)csw_Malloc (n * 2 * sizeof(TRiangleIndex  ));
    if (!tri) {
        csw_Free (grid);
        csw_Free (igrid);
        grd_set_err (1);
        return -1;
    }

    pts = (POint3D *)csw_Malloc (n * sizeof(POint3D));
    if (!pts) {
        csw_Free (grid);
        csw_Free (igrid);
        csw_Free (tri);
        grd_set_err (1);
        return -1;
    }

    xspace = (x2 - x1) / (CSW_F)(ncol - 1);
    yspace = (y2 - y1) / (CSW_F)(nrow - 1);

/*
    Fill in the pts array with all non null node locations and
    set the igrid index array.
*/
    n = 0;
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        yg1 = y1 + i * yspace;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            if (grid[k] > 1.e19f) {
                igrid[k] = -1;
                continue;
            }
            igrid[k] = n;
            pts[n].x = x1 + j * xspace;
            pts[n].y = yg1;
            pts[n].z = grid[k];
            n++;
        }
    }
    npts = n;

/*
    Create triangles by dividing each grid cell from the upper left
    to the lower right.  As long as all three corners in the igrid
    array are not -1, then the triangle is output.
*/
    n = 0;
    for (i=0; i<nrow-1; i++) {
        offset = i * ncol;
        for (j=0; j<ncol-1; j++) {
            k = offset + j;
            i1 = igrid[k];
            i2 = igrid[k+1];
            i3 = igrid[k+ncol];
            i4 = igrid[k+ncol+1];
            nc = 0;
            if (i1 >= 0  &&  i2 >= 0  &&  i3 >= 0) {
                tri[n].v1 = i1;
                tri[n].v2 = i2;
                tri[n].v3 = i3;
                n++;
                nc++;
            }
            if (i4 >= 0  &&  i2 >= 0  &&  i3 >= 0) {
                tri[n].v1 = i3;
                tri[n].v2 = i2;
                tri[n].v3 = i4;
                n++;
                nc++;
            }
        /*
            If no triangles were output, try again with a lower left
            to upper right diagonal.
        */
            if (nc == 0) {
                if (i1 >= 0  &&  i2 >= 0  &&  i4 >= 0) {
                    tri[n].v1 = i1;
                    tri[n].v2 = i2;
                    tri[n].v3 = i4;
                    n++;
                }
                if (i1 >= 0  &&  i4 >= 0  &&  i3 >= 0) {
                    tri[n].v1 = i1;
                    tri[n].v2 = i4;
                    tri[n].v3 = i3;
                    n++;
                }
            }
        }
    }

    csw_Free (grid);
    csw_Free (igrid);

    *triangles = tri;
    *points = pts;
    *num_triangles = n;
    *num_points = npts;

    return 1;

}  /*  end of grd_triangles_from_grid function  */




/*
  ****************************************************************************

              g r d _ t r i a n g l e s _ f r o m _ i n d e x

  ****************************************************************************

    Fill in an array of separate distinct triangles given an array of
  points and an array of triangle indices.

*/

int CSWGrdUtils::grd_triangles_from_index
                             (POint3D *points,
                              TRiangleIndex   *tri_index, int ntriangles,
                              TRiangle3D *triangles)
{
    int               i, i1, i2, i3;

    if (points == NULL  ||
        tri_index == NULL  ||  ntriangles < 1) {
        grd_set_err (2);
        return -1;
    }

    if (triangles == NULL) {
        grd_set_err (3);
        return -1;
    }

    for (i=0; i<ntriangles; i++) {
        i1 = tri_index[i].v1;
        i2 = tri_index[i].v2;
        i3 = tri_index[i].v3;
        triangles[i].vertices[0].x = points[i1].x;
        triangles[i].vertices[0].y = points[i1].y;
        triangles[i].vertices[0].z = points[i1].z;
        triangles[i].vertices[1].x = points[i2].x;
        triangles[i].vertices[1].y = points[i2].y;
        triangles[i].vertices[1].z = points[i2].z;
        triangles[i].vertices[2].x = points[i3].x;
        triangles[i].vertices[2].y = points[i3].y;
        triangles[i].vertices[2].z = points[i3].z;
    }

    return 1;

}  /*  end of function grd_triangles_from_index  */





/*
  ****************************************************************************

                 g r d _ h e x a g o n s _ f r o m _ g r i d

  ****************************************************************************

    Generate a network of connected hexagons overlaying a grid.  The hexagons
  are all regular.  Null grid values are ignored.  If a grid value is null,
  then the z value of a hexagon point near it will also be null.  The hexagons
  are regular hexagons (equal sides, equal angles).

*/

int CSWGrdUtils::grd_hexagons_from_grid
                           (CSW_F *grid, int ncol, int nrow,
                            CSW_F xmin, CSW_F ymin, CSW_F xmax, CSW_F ymax,
                            POint3D **pointout, int *npointout,
                            SIdeIndex **sideout, int *nsideout,
                            HExagonIndex **hexout, int *nhexout)
{
    int             i, j, k, ncw, nrw, np, ns, nh, ir,
                    p1, p2, maxp, istat;
    int             *sideindex, *pointindex;
    CSW_F           x1, y1, x2, y2, xt, yt, xspace, yspace, y0;
    CSW_F           *gwork;

    POint3D         *pwork;
    SIdeIndex       *swork;
    HExagonIndex    *hwork;

/*
    Check obvious errors.
*/
    if (grid == NULL) {
        grd_set_err (2);
        return -1;
    }
    if (ncol < 2  ||  ncol > 1000000  ||
        nrow < 2  ||  nrow > 1000000) {
        grd_set_err (3);
        return -1;
    }
    if (xmin >= xmax  ||  ymin >= ymax) {
        grd_set_err (4);
        return -1;
    }
    if (pointout == NULL  ||  npointout == NULL  ||
        sideout == NULL  ||  nsideout == NULL  ||
        hexout == NULL  ||  nhexout == NULL) {
        grd_set_err (5);
        return -1;
    }

/*
    Set output to NULL in case of errors.
*/
    *pointout = NULL;
    *npointout = 0;
    *sideout = NULL;
    *nsideout = 0;
    *hexout = NULL;
    *nhexout = 0;

/*
    Change the grid geometry so that each row is .5 of the average
    input spacing and each column is sqrt(3) of the average spacing.
    When the grid has these spacings, the corner points for regular
    hexagons will be at grid nodes.
*/
    yspace = (ymax - ymin) / (CSW_F)(nrow - 1);
    xspace = (xmax - xmin) / (CSW_F)(ncol - 1);
    yspace = (yspace + xspace) / 2.0f;
    xspace = 0.866025f * yspace;
    yspace /= 2.0f;

/*
    Make sure the number of cell rows is an even multiple of 6 and the
    number of cell columns is an even multiple of 3.  This insures that
    an integral number of hexagons can be fitted in any horizontal
    or vertical part of the grid.  For cell rows and columns to be correct,
    node rows and columns need to be 1 more than the even multiple.  In addition,
    an extra row is needed for the top most point of the top most
    hexagons.
*/
    ncw = (int)((xmax - xmin) / xspace) + 1;
    nrw = (int)((ymax - ymin) / yspace) + 1;
    if (ncw%3 != 0) {
        ncw -= ncw%3;
        ncw += 3;
    }
    if (nrw%6 != 0) {
        nrw -= nrw%6;
        nrw += 6;
    }
    ncw++;
    nrw += 2;

/*
    Expand the grid limits enough to fit in the number of rows and
    columns with the calculated grid spacings.
*/
    xt = (CSW_F)(ncw - 1) * xspace - xmax + xmin;
    xt /= 2.0f;
    x1 = xmin - xt;
    x2 = x1 + (CSW_F)(ncw - 1) * xspace;
    yt = (CSW_F)(nrw - 1) * yspace - ymax + ymin;
    yt /= 2.0f;
    y1 = ymin - yt;
    y2 = y1 + (CSW_F)(nrw - 1) * yspace;

/*
    Allocate a work grid for the resampled geometry and
    also index grids for points and sides of hexagons.
*/
    gwork = (CSW_F *)csw_Malloc (ncw * nrw * sizeof(CSW_F));
    if (gwork == NULL) {
        grd_set_err (1);
        return -1;
    }

    pointindex = (int *)csw_Malloc (ncw * nrw * 2 * sizeof(int));
    if (pointindex == NULL) {
        csw_Free (gwork);
        grd_set_err (1);
        return -1;
    }
    sideindex = pointindex + ncw * nrw;

/*
    Fill the index grids with -1 to flag that no point or side
    exists at the location in the respective index grid.
*/
    for (i=0; i<ncw*nrw; i++) {
        pointindex[i] = -1;
        sideindex[i] = -1;
    }

/*
    Allocate point, side and polygon lists.
*/
    maxp = ncw * nrw / 2;
    pwork = (POint3D *)csw_Malloc (maxp * sizeof(POint3D));
    if (pwork == NULL) {
        csw_Free (gwork);
        csw_Free (pointindex);
        grd_set_err (1);
        return -1;
    }

    maxp = ncw * nrw;
    swork = (SIdeIndex *)csw_Malloc (maxp * sizeof(SIdeIndex));
    if (swork == NULL) {
        csw_Free (gwork);
        csw_Free (pointindex);
        csw_Free (pwork);
        grd_set_err (1);
        return -1;
    }

    maxp = ncw * nrw / 3;
    hwork = (HExagonIndex *)csw_Malloc (maxp * sizeof(HExagonIndex));
    if (hwork == NULL) {
        csw_Free (gwork);
        csw_Free (pointindex);
        csw_Free (pwork);
        csw_Free (swork);
        grd_set_err (1);
        return -1;
    }

    np = 0;
    ns = 0;
    nh = 0;

/*
    Resample the input grid into the gwork geometry.
*/
    istat = grd_arith_ptr->grd_resample_grid (grid, NULL, ncol, nrow,
                               xmin, ymin, xmax, ymax,
                               NULL, 0,
                               gwork, NULL, ncw, nrw,
                               x1, y1, x2, y2,
                               GRD_BILINEAR);
    if (istat == -1) {
        csw_Free (gwork);
        csw_Free (pointindex);
        csw_Free (pwork);
        csw_Free (swork);
        csw_Free (hwork);
        grd_set_err (1);
        return -1;
    }

/*
    Calculate the points list.
*/
    ir = 0;
    while (ir < nrw-1) {
        y0 = (CSW_F)ir * yspace + y1;
        for (j=0; j<ncw; j++) {
            xt = x1 + xspace * (CSW_F)j;
            if (j%2 == 0) {
                yt = y0 + yspace;
                k = (ir + 1) * ncw + j;
            }
            else {
                yt = y0;
                k = ir * ncw + j;
            }
            pwork[np].x = xt;
            pwork[np].y = yt;
            pwork[np].z = gwork[k];
            pointindex[k] = np;
            np++;
        }
        ir += 3;
        if (ir >= nrw) break;
        y0 = (CSW_F)ir * yspace + y1;
        for (j=0; j<ncw; j++) {
            xt = x1 + xspace * (CSW_F)j;
            if (j%2 == 1) {
                yt = y0 + yspace;
                k = (ir + 1) * ncw + j;
            }
            else {
                yt = y0;
                k = ir * ncw + j;
            }
            pwork[np].x = xt;
            pwork[np].y = yt;
            pwork[np].z = gwork[k];
            pointindex[k] = np;
            np++;
        }
        ir += 3;
    }

/*
    Calculate the sides list.
*/
    ir = 0;
    while (ir < nrw-1) {
        for (j=0; j<ncw-1; j++) {
            if (j%2 == 0) {
                p1 = pointindex[(ir+1)*ncw+j];
                p2 = pointindex[ir*ncw+j+1];
            }
            else {
                p1 = pointindex[ir*ncw+j];
                p2 = pointindex[(ir+1)*ncw+j+1];
            }
            swork[ns].p1 = p1;
            swork[ns].p2 = p2;
            k = ir * ncw + j;
            sideindex[k] = ns;
            ns++;
        }
        ir++;
        if (ir >= nrw-1) break;
        for (j=0; j<ncw; j+=2) {
            k = ir * ncw + j;
            p1 = pointindex[k];
            p2 = pointindex[k+ncw*2];
            swork[ns].p1 = p1;
            swork[ns].p2 = p2;
            sideindex[k] = ns;
            sideindex[k+ncw] = ns;
            ns++;
        }
        ir += 2;
        if (ir >= nrw-1) break;
        for (j=0; j<ncw-1; j++) {
            if (j%2 == 1) {
                p1 = pointindex[(ir+1)*ncw+j];
                p2 = pointindex[ir*ncw+j+1];
            }
            else {
                p1 = pointindex[ir*ncw+j];
                p2 = pointindex[(ir+1)*ncw+j+1];
            }
            swork[ns].p1 = p1;
            swork[ns].p2 = p2;
            k = ir * ncw + j;
            sideindex[k] = ns;
            ns++;
        }
        ir++;
        if (ir >= nrw-1) break;
        for (j=1; j<ncw; j+=2) {
            k = ir * ncw + j;
            p1 = pointindex[k];
            p2 = pointindex[k+ncw*2];
            swork[ns].p1 = p1;
            swork[ns].p2 = p2;
            sideindex[k] = ns;
            sideindex[k+ncw] = ns;
            ns++;
        }
        ir += 2;
    }

/*
    Calculate the hexagon list.
*/
    ir = 0;
    while (ir < nrw-3) {
        for (j=0; j<ncw-2; j+=2) {
            k = ir * ncw + j;
            hwork[nh].s1 = sideindex[k];
            hwork[nh].s2 = sideindex[k+1];
            hwork[nh].s3 = sideindex[k+ncw+2];
            hwork[nh].s4 = sideindex[k+ncw*3+1];
            hwork[nh].s5 = sideindex[k+ncw*3];
            hwork[nh].s6 = sideindex[k+ncw];
            nh++;
        }
        ir += 3;
        for (j=1; j<ncw-2; j+=2) {
            k = ir * ncw + j;
            hwork[nh].s1 = sideindex[k];
            hwork[nh].s2 = sideindex[k+1];
            hwork[nh].s3 = sideindex[k+ncw+2];
            hwork[nh].s4 = sideindex[k+ncw*3+1];
            hwork[nh].s5 = sideindex[k+ncw*3];
            hwork[nh].s6 = sideindex[k+ncw];
            nh++;
        }
        ir += 3;
    }

    csw_Free (gwork);
    csw_Free (pointindex);

    *pointout = pwork;
    *sideout = swork;
    *hexout = hwork;
    *npointout = np;
    *nsideout = ns;
    *nhexout = nh;

    return 1;

}  /*  end of function grd_hexagons_from_grid  */





/*
  ****************************************************************************

               g r d _ h e x a g o n s _ f r o m _ i n d e x

  ****************************************************************************

    Calculate a set of stand alone hexagons from the specified points, sides,
  and hexagon indices.

*/

int CSWGrdUtils::grd_hexagons_from_index
                            (POint3D *points,
                             SIdeIndex *sides,
                             HExagonIndex *hex_index, int nhex,
                             HExagon3D *hexagons)
{
    int               i, i1, i2, i3, i4, i5, i6;

    if (points == NULL  ||  sides == NULL  ||
        hex_index == NULL  ||  nhex < 1) {
        grd_set_err (2);
        return -1;
    }

    if (hexagons == NULL) {
        grd_set_err (3);
        return -1;
    }

    for (i=0; i<nhex; i++) {
        i1 = hex_index[i].s1;
        i2 = hex_index[i].s2;
        i3 = hex_index[i].s3;
        i4 = hex_index[i].s4;
        i5 = hex_index[i].s5;
        i6 = hex_index[i].s6;
        hexagons[i].corners[0].x = points[sides[i1].p1].x;
        hexagons[i].corners[0].y = points[sides[i1].p1].y;
        hexagons[i].corners[0].z = points[sides[i1].p1].z;
        hexagons[i].corners[1].x = points[sides[i2].p1].x;
        hexagons[i].corners[1].y = points[sides[i2].p1].y;
        hexagons[i].corners[1].z = points[sides[i2].p1].z;
        hexagons[i].corners[2].x = points[sides[i3].p1].x;
        hexagons[i].corners[2].y = points[sides[i3].p1].y;
        hexagons[i].corners[2].z = points[sides[i3].p1].z;
        hexagons[i].corners[3].x = points[sides[i4].p2].x;
        hexagons[i].corners[3].y = points[sides[i4].p2].y;
        hexagons[i].corners[3].z = points[sides[i4].p2].z;
        hexagons[i].corners[4].x = points[sides[i5].p2].x;
        hexagons[i].corners[4].y = points[sides[i5].p2].y;
        hexagons[i].corners[4].z = points[sides[i5].p2].z;
        hexagons[i].corners[5].x = points[sides[i6].p2].x;
        hexagons[i].corners[5].y = points[sides[i6].p2].y;
        hexagons[i].corners[5].z = points[sides[i6].p2].z;
    }

    return 1;

}  /*  end of function grd_hexagons_from_index  */



/*
  ****************************************************************

                   E x p a n d F o r Z e r o

  ****************************************************************

    This is only called from grd_expand_one_level_new for the case
  of expanding near zero's in thickness grids.

*/

int CSWGrdUtils::ExpandForZero
                         (CSW_F *grid1, CSW_F *grid2, int ncol, int nrow,
                          CSW_F nval, int eflag, int flag)
{
    int          i, j, ii, jj, i1, i2, k, k2,
                 nt, n2, ndone, im, jm, km;
    CSW_F        sum, sum2, zt, zt2;

    ndone = 0;
    sum = 0.0f;
    sum2 = 0.0f;

    if (eflag > 4) eflag = 0;

    for (i=0; i<nrow; i++) {

        i1 = ncol * i;

        for (j=0; j<ncol; j++) {

            k = i1 + j;
            nt = 0;
            n2 = 0;
            if (grid1[k] != 0.0) {
                grid2[k] = grid1[k];
                continue;
            }
            else {
                sum = 0.0f;
                sum2 = 0.0f;
                for (ii=i-1; ii<=i+1; ii++) {
                    if (ii < 0  ||  ii >= nrow) continue;
                    i2 = ii * ncol;
                    for (jj=j-1; jj<=j+1; jj++) {
                        if (jj < 0  ||  jj >= ncol) continue;
                        k2 = i2 + jj;
                        if (grid1[k2] < nval  &&  grid1[k] != 0.0) {
                            nt++;
                            sum += grid1[k2];
                        }
                    }
                }
                if (eflag) {
                    for (ii=i-2; ii<=i+2; ii+=2) {
                        if (ii < 0  ||  ii >= nrow) continue;
                        i2 = ii * ncol;
                        im = (ii + i) / 2;
                        im *= ncol;
                        for (jj=j-2; jj<=j+2; jj+=2) {
                            if (jj < 0  ||  jj >= ncol) continue;
                            k2 = i2 + jj;
                            jm = (jj + j) / 2;
                            km = im + jm;
                            if (grid1[k2] < nval  &&  grid1[km] < nval  &&
                                grid1[k2] != 0.0  &&  grid1[km] != 0.0) {
                                n2++;
                                zt = 2.0f * grid1[km] - grid1[k2];
                                if (flag == 1  &&  zt < 0.0f) {
                                    zt = -zt;
                                }
                                else if (flag == -1  &&  zt > 0.0f) {
                                    zt = -zt;
                                }
                                sum2 += zt;
                            }
                        }
                    }
                }
            }

            if (flag == 1  &&  sum < 0.0f) {
                sum = -sum;
            }
            else if (flag == -1  &&  sum > 0.0f) {
                sum = -sum;
            }
            if (n2 > 0) {
                if (nt > 0) {
                    zt = sum / (CSW_F)nt;
                    zt2 = sum2 / (CSW_F)n2;
                    if (eflag != 0) {
                        grid2[k] = zt2;
                    }
                    else {
                        grid2[k] = (zt + zt2) / 2.0f;
                    }
                }
                else {
                    grid2[k] = sum2 / (CSW_F)n2;
                }
                ndone++;
            }

            else {
                if (nt > 0) {
                    grid2[k] = sum / (CSW_F)nt;
                    ndone++;
                }
                else {
                    grid2[k] = grid1[k];
                }
            }

        }
    }

    return ndone;

}  /*  end of private function ExpandForZero  */





/*
  ****************************************************************

            g r d _ c a l c _ d o u b l e _ p l a n e

  ****************************************************************

    Return the coefficients of the best fitting plane to a small
  set of points.

*/

int CSWGrdUtils::grd_calc_double_plane
                          (double *xbar, double *ybar, double *zbar, int nptsin,
                           double *coef)
{
    double          a[9], b[3], big, pivinv, dum, z1, z2,
                    s1, s2, s3, s4, s5, s6, s7, s8,
                    dachk, temp;
    int             ipiv[3], i, j, k,
                    irow, icol, npts, n;

    if (nptsin < 3) {
        return -1;
    }

/*
    Find data limits and convert to local double arrays.
*/
    npts = nptsin;
    if (npts > MAX_PLANE) npts = MAX_PLANE;

    z1 = 1.e30;
    z2 = -1.e30;

    for (i=0; i<npts; i++) {
        if (zbar[i] < z1) z1 = zbar[i];
        if (zbar[i] > z2) z2 = zbar[i];
    }

/*
    Special case for an essentially flat horizontal data input.
*/
    coef[0] = (z1 + z2) / 2.0;
    coef[1] = 0.0;
    coef[2] = 0.0;
    if (z2 - z1 < Z_ABSOLUTE_TINY) {
        return 1;
    }

/*
    set up for gaussian elimination
*/
    s1 = s2 = s3 = s4 = s5 = s6 = s7 = s8 = 0;
    for (i=0; i<npts; i++) {
        s1 += xbar[i];
        s2 += ybar[i];
        s3 += xbar[i] * xbar[i];
        s4 += xbar[i] * ybar[i];
        s5 += ybar[i] * ybar[i];
        s6 += zbar[i];
        s7 += xbar[i] * zbar[i];
        s8 += ybar[i] * zbar[i];
    }

    a[0] = npts;
    a[1] = s1;
    a[2] = s2;
    a[3] = s1;
    a[4] = s3;
    a[5] = s4;
    a[6] = s2;
    a[7] = s4;
    a[8] = s5;

    b[0] = s6;
    b[1] = s7;
    b[2] = s8;

    n = 3;

    for (i=0; i<n; i++) {
        ipiv[i] = 0;
    }

    icol = 0;
    irow = 0;

    dachk = 1.e-20;

/*
    gaussian elimination
*/
    for (i=0; i<n; i++) {

        big = 0.0;

        for (j=0; j<n; j++) {

            if (ipiv[j] != 1) {
                for (k=0; k<n; k++) {
                    if (ipiv[k] == 0) {
                        temp = *(a + 3*k + j);
                        if (temp < 0.0) temp = -temp;
                        if (temp >= big) {
                            big = *(a + 3*k + j);
                            if (big < 0.0) big = -big;
                            irow = j;
                            icol = k;
                        }
                    }
                    else if (ipiv[k] > 1) {
                        return -1;
                    }
                }
            }
        }

        ipiv[icol]++;

        if (irow != icol) {
            for (j=0; j<n; j++) {
                dum = *(a + irow + 3*j);
                *(a + irow + 3*j) = *(a + icol + 3*j);
                *(a + icol + 3*j) = dum;
            }
            dum = b[irow];
            b[irow] = b[icol];
            b[icol] = dum;
        }

        dum = *(a + 4*icol);
        if (dum < 0.0) dum = -dum;
        if (dum < dachk) {
            return -1;
        }

        pivinv = 1.0 / (*(a + 4*icol));
        *(a + 4*icol) = 1.0;

        for (j=0; j<n; j++) {
            *(a + icol + 3*j) *= pivinv;
        }
        b[icol] *= pivinv;

        for (j=0; j<n; j++) {
            if (j != icol) {
                dum = *(a + 3*icol + j);
                *(a + 3*icol + j) = 0.;
                for (k=0; k<n; k++) {
                    *(a + 3*k + j) -= *(a + icol + 3*k) * dum;
                }
                b[j] -= b[icol] * dum;
            }
        }

    }  /*  end of i loop and gaussian elimination  */

/*
    return the coefs
*/
    for (i=0; i<3; i++) {
        coef[i] = b[i];
    }

    return 1;

}  /*  end of function grd_calc_double_plane  */





/*
  ****************************************************************

                  g r d _ f i l l _ p l a t e a u

  ****************************************************************

    Replace the z value for the nodes having the plateau value.
    Gradients are extrapolated from nearby non null nodes to do this.
    Note that if nearby nodes are a valley that will be filled in
    later, the valley values are considered valid for gradient
    extrapolation.
*/

int CSWGrdUtils::grd_fill_plateau
                     (CSW_F *grid, int ncol, int nrow,
                      CSW_F value, CSW_F nullval, CSW_F zrange)
{
    int           istat, i;
    CSW_F         nval, fudge, zmin, zmax;

    fudge = 0.0f;
    zrange = zrange;

    GridZmax = value;
    GridZmin = -1.e30f;

    nval = nullval / 10.0f;

    zmin = 1.e30f;
    zmax = -1.e30f;

    FlatStyle = _PLATEAU_;
    for (i=0; i<ncol*nrow; i++) {
        if (grid[i] < nval) {
            if (grid[i] >= value) {
                grid[i] = SOFT_NULL_VALUE;
            }
            else {
                if (grid[i] > value - fudge) {
                    grid[i] = value - fudge;
                }
            }
            if (grid[i] < zmin) zmin = grid[i];
            if (grid[i] > zmax) zmax = grid[i];
        }
    }

    TinySum = 0.0;
    if (zmax > zmin) {
        TinySum = (zmax - zmin) / 100000.0f;
        if (TinySum > 1.e-10) TinySum = 1.e-10f;
    }
    istat = FillFlatSpot (grid, ncol, nrow, value, nullval);

    TinySum = 0.0;

    return istat;
}



/*
 *********************************************************************

                   g r d _ f i l l _ v a l l e y

 *********************************************************************
*/
int CSWGrdUtils::grd_fill_valley
                    (CSW_F *grid, int ncol, int nrow,
                     CSW_F value, CSW_F nullval, CSW_F zrange)
{
    int           istat, i;
    CSW_F         nval, fudge;

    zrange = zrange;
    fudge = 0.0f;

    GridZmin = value;
    GridZmax = 1.e30f;

    nval = nullval / 10.0f;

    for (i=0; i<ncol*nrow; i++) {
        if (grid[i] < nval) {
            if (grid[i] <= value) {
                grid[i] = SOFT_NULL_VALUE;
            }
            else {
                if (grid[i] < value + fudge) {
                    grid[i] = value + fudge;
                }
            }
        }
    }

    FlatStyle = _VALLEY_;
    istat = FillFlatSpot (grid, ncol, nrow, value, nullval);
    return istat;
}




/*
 ************************************************************************

                       F i l l F l a t S p o t

 ************************************************************************
*/
int CSWGrdUtils::FillFlatSpot
                        (CSW_F *grid, int ncol, int nrow,
                         CSW_F value, CSW_F nullval)
{
    int           i, istat, nstat, icoarse;
    int           j, k, ki, kj, offset, off2, eflag;
    CSW_F         sum, sum2, tiny, z1, z2;
    CSW_F         fudge;
    CSW_F         *grid1 = NULL, *grid2 = NULL, nval, sval;
    CSW_F         value_to_set;

    auto fscope = [&]()
    {
        if (grid1 != NULL) {
            csw_Free (grid1);
            grid1 = NULL;
            grid2 = NULL;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    allocate work space memory
*/
MSL
    grid1 = (CSW_F *)csw_Malloc (ncol * nrow * 5 * sizeof(CSW_F));
    if (!grid1) {
        grd_set_err (1);
        return -1;
    }
    grid2 = grid1 + ncol * nrow;

/*
    copy input grid to workspace
*/
    csw_memcpy ((char *)grid1, (char *)grid, ncol*nrow*sizeof(CSW_F));

/*
    Make sure at least one valid number is available in the grid.
*/
    nval = nullval;
    sval = SOFT_NULL_VALUE / 10.0f;
    nstat = 0;
    z1 = 1.e30f;
    z2 = -1.e30f;
    for (i=0; i< ncol*nrow; i++) {
        if (grid1[i] < nval  &&  grid1[i] > sval) {
            if (grid1[i] < z1) z1 = grid1[i];
            if (grid1[i] > z2) z2 = grid1[i];
            nstat++;
        }
    }

    if (nstat == 0) {
        grd_set_err (12);
        csw_Free (grid1);
        return -1;
    }

    tiny = (z2 - z1) / 50000.0f;
    fudge = tiny * 500.0f;

    icoarse = ncol + nrow;
    icoarse /= 20;
    if (icoarse < 4) icoarse = 4;

    ExpandZmin = (CSW_F)(GridZmin - (z2 - z1) / 20.0);
    ExpandZmax = (CSW_F)(GridZmax + (z2 - z1) / 20.0);

/*
    Fill in the nulls by extending gradients from the non null nearest
    neighbors.  Do this by expanding one level at a time.
*/
    nstat = 0;
    eflag = 1;
    istat = 1;
    for (i=0; i<=icoarse+1; i++) {

        istat = ExpandForValue
            (grid1, grid2, ncol, nrow, nval, eflag, value, fudge);
        if (istat == 0) {
            nstat = 1;
            break;
        }

        eflag++;

        istat = ExpandForValue
            (grid2, grid1, ncol, nrow, nval, eflag, value, fudge);

        if (istat == 0) {
            nstat = 2;
            break;
        }
        eflag++;

    }

    if (nstat == 1) {
        csw_memcpy ((char *)grid1, (char *)grid2, ncol*nrow*sizeof(CSW_F));
    }

/*
 * If there are still soft null values, set them to the current
 * appropriate limit of the grid.
 */
    value_to_set = ExpandZmin;
    if (FlatStyle == _PLATEAU_) {
        value_to_set = ExpandZmax;
    }

    ExpandZmin = -1.e30f;
    ExpandZmax = 1.e30f;

    for (i=0; i<ncol*nrow; i++) {
        if (grid1[i] <= sval) {
            grid1[i] = value_to_set;
        }
    }

/*
    Smooth grid1 and then combine it with original grid, using
    smoothed grid1 values where the original grid was null.
*/
    for (i=0; i<nrow; i++) {

        offset = i * ncol;

        for (j=0; j<ncol; j++) {

            k = offset + j;

            if (grid1[k] >= nval) {
                grid2[k] = 1.e30f;
                continue;
            }
            sum = 0.0f;
            sum2 = 0.0f;
            for (ki=i-1; ki<=i+1; ki++) {
                if (ki<0  ||  ki>=nrow) {
                    continue;
                }
                off2 = ki*ncol;
                for (kj=j-1; kj<=j+1; kj++) {
                    if (kj>=0  &&  kj<ncol) {
                        if (grid1[off2+kj] < nval) {
                            sum += grid1[off2+kj];
                            sum2 += 1.0f;
                        }
                    }
                }
            }

            if (sum2 > 0.0) {
                if (sum < TinySum  &&  sum > -TinySum) {
                    sum = 0.0;
                }
                grid2[k] = sum / sum2;
            }
            else {
                grid2[k] = grid1[k];
            }

        }

    }

    /*
     * bug 8122   Make the plateau values a bit higher to get a better
     * contour at the edge of the plateau.  Make the valleys values a little lower.
     */

    if (FlatStyle == _PLATEAU_) {
        for (i=0; i<ncol*nrow; i++) {
            if (grid[i] <= sval) {
                if (grid2[i] < value + fudge) {
                    grid2[i] = value + fudge;
                }
                grid[i] = grid2[i];
            }
        }
    }
    else {
        for (i=0; i<ncol*nrow; i++) {
            if (grid[i] <= sval) {
                if (grid2[i] > value - fudge) {
                    grid2[i] = value - fudge;
                }
                grid[i] = grid2[i];
            }
        }
    }

    csw_Free (grid1);

    return 1;

}  /*  end of private FillFlatSpot function  */





/*
  ****************************************************************

                   E x p a n d F o r V a l u e

  ****************************************************************

    This is called from FillFlatSpot to etrapolate one cell width
    out from the current good nodes.
*/

int CSWGrdUtils::ExpandForValue
                          (CSW_F *grid1, CSW_F *grid2, int ncol, int nrow,
                           CSW_F nval, int eflag, CSW_F value, CSW_F fudge)
{
    int          i, j, ii, jj, i1, i2, k, k2,
                 nt, n2, ndone, im, jm, km;
    CSW_F        sum, sum2, zt, zt2, sval, zcorr;

    ndone = 0;
    sum = 0.0f;
    sum2 = 0.0f;
    sval = SOFT_NULL_VALUE / 100.0f;
    if (eflag > 0) {
        fudge *= eflag;
    }

    for (i=0; i<nrow; i++) {

        i1 = ncol * i;

        for (j=0; j<ncol; j++) {

            k = i1 + j;
            if (grid1[k] > sval) {
                grid2[k] = grid1[k];
                continue;
            }
            nt = 0;
            n2 = 0;

            sum = 0.0f;
            sum2 = 0.0f;

        /*
         * The first estimate is an average of the valid grid nodes
         * immediately adjacent to the node being filled.
         */
            for (ii=i-1; ii<=i+1; ii++) {
                if (ii < 0  ||  ii >= nrow) continue;
                i2 = ii * ncol;
                for (jj=j-1; jj<=j+1; jj++) {
                    if (jj < 0  ||  jj >= ncol) continue;
                    k2 = i2 + jj;
                    if (FlatStyle == _PLATEAU_) {
                        if (grid1[k2] < nval  &&  grid1[k2] > sval) {
                            zt = grid1[k2];
                            if (zt < value + fudge) {
                                zcorr = value - zt;
                                zt = value + zcorr + fudge;
                            }
                            nt++;
                            sum += zt;
                        }
                    }
                    else {
                        if (grid1[k2] < nval  &&  grid1[k2] > sval) {
                            zt = grid1[k2];
                            if (zt > value - fudge) {
                                zcorr = zt - value;
                                zt = value - zcorr - fudge;
                            }
                            nt++;
                            sum += zt;
                        }
                    }
                }
            }

        /*
         * The second estimate uses gradients from two valid nodes
         * near the node being filled.  This is only done when the
         * filling procedure is near the original valid area of the
         * grid (signalled by the eflag parameter).  Extending
         * gradients far from the original data can introduce large
         * artifacts into the filled in area.
         */
            if (eflag) {
                for (ii=i-2; ii<=i+2; ii+=2) {
                    if (ii < 0  ||  ii >= nrow) continue;
                    i2 = ii * ncol;
                    im = (ii + i) / 2;
                    im *= ncol;
                    for (jj=j-2; jj<=j+2; jj+=2) {
                        if (jj < 0  ||  jj >= ncol) continue;
                        k2 = i2 + jj;
                        jm = (jj + j) / 2;
                        km = im + jm;
                        if (FlatStyle == _PLATEAU_) {
                            if (grid1[k2] < nval  &&  grid1[km] < nval  &&
                                grid1[k2] > sval  &&  grid1[km] > sval) {
                                n2++;
                                zt = 2.0f * grid1[km] - grid1[k2];
                                if (zt <= value + fudge) {
                                    zcorr = value - zt;
                                    zt = value + zcorr + fudge;
                                }
                                sum2 += zt;
                            }
                        }
                        else {
                            if (grid1[k2] < nval  &&  grid1[km] < nval  &&
                                grid1[k2] > sval  &&  grid1[km] > sval) {
                                n2++;
                                zt = 2.0f * grid1[km] - grid1[k2];
                                if (zt >= value - fudge) {
                                    zcorr = zt - value;
                                    zt = value - zcorr - fudge;
                                }
                                sum2 += zt;
                            }
                        }
                    }
                }
            }

            if (n2 > 0) {
                if (nt > 0) {
                    zt = sum / (CSW_F)nt;
                    zt2 = sum2 / (CSW_F)n2;
                    if (eflag != 0) {
                        grid2[k] = zt2;
                    }
                    else {
                        grid2[k] = (zt + zt2) / 2.0f;
                    }
                }
                else {
                    grid2[k] = sum2 / (CSW_F)n2;
                }
                ndone++;
                if (grid2[k] > ExpandZmax) grid2[k] = ExpandZmax;
                if (grid2[k] < ExpandZmin) grid2[k] = ExpandZmin;
            }

            else {
                if (nt > 0) {
                    grid2[k] = sum / (CSW_F)nt;
                    ndone++;
                    if (grid2[k] > ExpandZmax) grid2[k] = ExpandZmax;
                    if (grid2[k] < ExpandZmin) grid2[k] = ExpandZmin;
                }
                else {
                    grid2[k] = grid1[k];
                }
            }

        }
    }

    return ndone;

}  /*  end of private function ExpandForValue  */






/*
 ************************************************************************

      g r d _ m a s k _ w i t h _ p o l y g o n

 ************************************************************************

*/

int CSWGrdUtils::grd_mask_with_polygon
                          (char *mask, int ncol, int nrow,
                           double xmin, double ymin,
                           double xmax, double ymax,
                           int flag,
                           double *xpin, double *ypin,
                           int npin, int *icin, int *ipin)

{
    double           *xp, *yp, *xpts, *ypts;
    double           xspace, yspace, yt;
    int              *ic, *ip, *inside;
    int              ido, i, j, k, n, nc, npts, npout;
    int              istat, maxpoints, maxcomps, offset;

    CSWPolyUtils     ply_utils_obj;
    CSWPolyGraph     ply_graph_obj;

/*
 * check for obvious errors
 */
    if (mask == NULL  ||  xpin == NULL  ||  ypin == NULL) {
        grd_set_err (2);
        return -1;
    }
    if (ncol < 2  ||  nrow < 2) {
        grd_set_err (2);
        return -1;
    }
    if (xmin >= xmax  ||  ymin >= ymax) {
        grd_set_err (2);
        return -1;
    }
    if (npin < 1  ||  icin == NULL  ||  ipin == NULL) {
        grd_set_err (2);
        return -1;
    }

/*
 * allocate space for internal polygon representation
 */
    nc = 0;
    n = 0;
    for (i=0; i<npin; i++) {
        for (j=0; j<icin[i]; j++) {
            n += ipin[nc];
            nc++;
        }
    }

    maxpoints = n * 4;
    maxcomps = nc * 4;

    xp = (double *)csw_Malloc (maxpoints * 2 * sizeof(double));
    if (xp == NULL) {
        grd_set_err (1);
        return -1;
    }
    yp = xp + maxpoints;

    ic = (int *)csw_Malloc (maxcomps * 2 * sizeof(int));
    if (ic == NULL) {
        csw_Free (xp);
        grd_set_err (1);
        return -1;
    }
    ip = ic + maxcomps;

/*
 * Union the input polygons.
 */
    istat = ply_graph_obj.ply_union_components (xpin, ypin, NULL,
                                  npin, icin, ipin,
                                  xp, yp, NULL,
                                  &npout, ic, ip,
                                  maxpoints, maxcomps);
    if (istat == -1) {
        csw_Free (xp);
        csw_Free (ic);
        grd_set_err (2);
        return -1;
    }

/*
 * generate x,y points for each grid node.
 */
    npts = ncol * nrow;
    xpts = (double *)csw_Malloc (npts * 2 * sizeof(double));
    if (xpts == NULL) {
        csw_Free (xp);
        csw_Free (ic);
        grd_set_err (1);
        return -1;
    }
    ypts = xpts + npts;

    xspace = (xmax - xmin) / (double)(ncol - 1);
    yspace = (ymax - ymin) / (double)(nrow - 1);

    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        yt = ymin + i * yspace;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            xpts[k] = xmin + j * xspace;
            ypts[k] = yt;
        }
    }

/*
 * allocate space for the array that gets the in/out status
 * of each point.
 */
    inside = (int *)csw_Calloc (npts * sizeof(int));
    if (inside == NULL) {
        csw_Free (xp);
        csw_Free (ic);
        csw_Free (xpts);
        grd_set_err (1);
        return -1;
    }

/*
 * For each polygon output by the union operation, test all
 * points for inside, on edge or outside that polygon.  The
 * mask is updated after each polygon is tested.
 */
    n = 0;
    nc = 0;
    for (ido=0; ido<npout; ido++) {
        istat = ply_utils_obj.ply_points (xp+n, yp+n, ip+nc, ic[ido],
                            xpts, ypts, npts,
                            inside);
        if (istat == -1) {
            csw_Free (xp);
            csw_Free (ic);
            csw_Free (xpts);
            csw_Free (inside);
            grd_set_err (1);
            return -1;
        }

        if (flag == GRD_OUTSIDE_POLYGON) {
            for (i=0; i<npts; i++) {
                if (inside[i] <= 0) {
                    mask[i] = 1;
                }
            }
        }
        else {
            for (i=0; i<npts; i++) {
                if (inside[i] >= 0) {
                    mask[i] = 1;
                }
            }
        }

        for (i=0; i<ic[ido]; i++) {
            n += ip[nc];
            nc++;
        }

    }  /* end of ido loop */

    csw_Free (xp);
    csw_Free (ic);
    csw_Free (xpts);
    csw_Free (inside);

    return 1;

}  /* end of function grd_mask_with_polygon */




/*
 *****************************************************************************

              g r d _ r e f i n e _ c o n t o u r _ d a t a

 *****************************************************************************

 Insert points in the specified contour lines so that no adjacent points are
 more than the specified spacing distance apart in the output.  If the specified
 spacing would produce a huge amount of output points (aproximately 100 times
 the number of input points or more) then the spacing is adjusted to produce
 a reasonable number of output points.

 QWER The spline flag is ignored for now.  The interpolation is always linear.
 (7/10/02)

*/

int CSWGrdUtils::grd_refine_contour_data
                            (COntourDataStruct *clines,
                             int nclines, int spline_flag,
                             double spacing,
                             COntourDataStruct **clout)
{
    COntourDataStruct        *cptr, *cout, *cpout;
    double                   *x, *y, *xt, *yt;
    double                   dx, dy, dist, dtot, dtest, kdx, kdy;
    int                      i, j, k, n, npts, ntot, npmax, kdo;

    spline_flag = spline_flag;

    cout = (COntourDataStruct *)csw_Calloc (nclines * sizeof(COntourDataStruct));
    if (cout == NULL) {
        return -1;
    }

/*
 * Adjust the spacing if it is too small.
 */
    dtot = 0.0;
    ntot = 0;
    for (i=0; i<nclines; i++) {
        cptr = clines + i;
        x = cptr->x;
        y = cptr->y;
        npts = cptr->npts;
        ntot += npts;
        for (j=0; j<npts-1; j++) {
            dx = x[j] - x[j+1];
            dy = y[j] - y[j+1];
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            dtot += dist;
        }
    }

    npmax = ntot * 100;
    if (npmax < 1000) npmax = 1000;
    if (spacing < dtot / npmax) {
        spacing = dtot / npmax;
    }

/*
 * Fill in extra points on each individual contour data line.
 */
    for (i=0; i<nclines; i++) {

        cptr = clines + i;
        cpout = cout + i;
        x = cptr->x;
        y = cptr->y;
        npts = cptr->npts;

    /*
     * Allocate space for this output line.
     */
        dtot = 0.0;
        for (j=0; j<npts-1; j++) {
            dx = x[j] - x[j+1];
            dy = y[j] - y[j+1];
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            dtot += dist;
        }
        n = (int)(dtot / spacing);
        n *= 4;
        if (n < 10) {
            n = 10;
        }
        npmax = npts + n + 10;
        if (npmax < 50) {
            npmax = 50;
        }
        xt = (double *) csw_Malloc (npmax * sizeof(double));
        if (xt == NULL) {
            grd_cleanup_contour_data (cout, nclines);
            csw_Free (cout);
            return -1;
        }
        yt = (double *)csw_Malloc (npmax * sizeof(double));
        if (yt == NULL) {
            grd_cleanup_contour_data (cout, nclines);
            csw_Free (xt);
            csw_Free (cout);
            return -1;
        }

    /*
     * Put original and interpolated points in the output line.
     */
        dtest = spacing * 1.25;
        n = 0;
        for (j=0; j<npts-1; j++) {
            xt[n] = x[j];
            yt[n] = y[j];
            n++;
            dx = x[j+1] - x[j];
            dy = y[j+1] - y[j];
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist < dtest) {
                continue;
            }
            kdo = (int)(dist / spacing + .5);
            kdx = dx / kdo;
            kdy = dy / kdo;
            for (k=1; k<kdo; k++) {
                xt[n] = x[j] + k * kdx;
                yt[n] = y[j] + k * kdy;
                n++;
            }
        }
        xt[n] = x[npts-1];
        yt[n] = y[npts-1];
        n++;

        cpout->x = xt;
        cpout->y = yt;
        cpout->npts = n;
        cpout->zlev = cptr->zlev;

    }

    *clout = cout;

    return 1;

}



/*
 *********************************************************************************

          g r d _ c l e a n u p _ c o n t o u r _ d a t a

 *********************************************************************************

  Free the x and y pointers in each contour data structure.

*/

void CSWGrdUtils::grd_cleanup_contour_data (COntourDataStruct *clist, int nlist)
{
    int                  i;
    COntourDataStruct    *cp;

    if (clist == NULL ||  nlist < 1) {
        return;
    }

    for (i=0; i<nlist; i++) {
        cp = clist + i;
        csw_Free (cp->x);
        csw_Free (cp->y);
    }

    return;
}



/*
 *****************************************************************************

                      g r d _ c o n t o u r _ t o _ g r i d

 *****************************************************************************

  This function uses a combination of triangulation and gridding to calculate
  a surface from a combination of points and contour lines or from contour
  lines alone.  If faults have been set up, they are honored.

  Errors:

    1 = memory allocation failure
    2 = NULL clines or nclines < 2
    3 = grid limits (xmin >= xmax or ymin >= ymax)
    4 = ncol or nrow < 2
    5 = NULL output grid

*/

#define FREE_LOCAL_DATA \
    csw_Free(nodes); nodes = NULL; \
    csw_Free(edges); edges = NULL; \
    csw_Free(triangles); triangles = NULL; \
    csw_Free(xfaults); xfaults = NULL; \
    csw_Free(faultpts); faultpts = NULL; \
    csw_Free(conpts); conpts = NULL; \
    csw_Free(xlines); xlines = NULL; \
    csw_Free(linepts); linepts = NULL; \
    csw_Free(xdat); xdat = NULL; \
    csw_Free(dxdat); dxdat = NULL; \
    grd_cleanup_contour_data (clines, nclines);


int CSWGrdUtils::grd_contour_to_grid
                        (COntourDataStruct *clinesin, int nclines,
                         double *xin, double *yin, double *zin, int nin,
                         double x1, double y1, double x2, double y2,
                         int ncol, int nrow,
                         CSW_F *grid, char *mask,
                         GRidCalcOptions *options)
{
    COntourDataStruct    *clines, *cptr;

    NOdeStruct           *nodes;
    EDgeStruct           *edges, *eptr;
    TRiangleStruct       *triangles;

    int                  numnodes, numedges, numtriangles;

    double               *xfaults, *yfaults, *zfaults;
    int                  *faultpts, nfaults;
    int                  *conpts, ncons, numconpts;

    double               *xlines, *ylines, *zlines;
    int                  *linepts, *linetypes, nlines;

    int                  i, j, n, np, nt, nf, dsize;
    int                  done, istat, ido;
    double               dx, dy, dz, ztiny, dist, dtest, wgt;
    double               zmin, zmax;
    double               dx1, dy1, dz1, dx2, dy2, dz2;
    int                  n1, n2;

    double               *dxdat, *dydat, *dzdat;
    CSW_F                *xdat, *ydat, *zdat, *edat;
    CSW_F                xt, yt, zt, zt2;
    int                  ndata;

/*
 * Check for obvious errors.
 */
    if (clinesin == NULL  ||  nclines < 2) {
        grd_set_err (2);
        return -1;
    }

    if (x1 >= x2  ||  y1 >= y2) {
        grd_set_err (3);
        return -1;
    }

    if (ncol < 2  ||  nrow < 2) {
        grd_set_err (4);
        return -1;
    }

    if (grid == NULL) {
        grd_set_err (5);
        return -1;
    }

/*
 * Set all local pointers that may be allocated to NULL
 * so the csw_Frees in case of allocation errors will not crash.
 */
    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    xfaults = NULL;
    faultpts = NULL;
    conpts = NULL;
    xlines = NULL;
    linepts = NULL;
    xdat = NULL;
    dxdat = NULL;
    clines = NULL;

/*
 * Introduce points in the contour lines so that no
 * points are much more than the grid spacing apart.
 */
    dtest = (x2 - x1 + y2 - y1) / (ncol + nrow - 2);
    dtest *= 1.25;

    istat = grd_refine_contour_data (clinesin, nclines,
                                     0, dtest,
                                     &clines);
    if (istat == -1) {
        FREE_LOCAL_DATA
        grd_set_err (1);
        return -1;
    }

/*
 * Count the number of points in the contour lines.
 */
    ncons = nclines;
    conpts = (int *)csw_Malloc (ncons * sizeof(int));
    if (conpts == NULL) {
        FREE_LOCAL_DATA
        grd_set_err (1);
        return -1;
    }

    n = 0;
    for (i=0; i<ncons; i++) {
        cptr = clines + i;
        n += cptr->npts;
        conpts[i] = cptr->npts;
    }
    numconpts = n;

/*
 * Get the faults if there are any.
 */
    nfaults = 0;
    grd_fault_ptr->grd_get_current_fault_vectors
        (&xfaults, &yfaults, &zfaults,
         &faultpts, &nfaults);

/*
 * Allocate enough space for both the contour lines and faults
 * in the constraint arrays.
 */
    nlines = ncons + nfaults;
    linepts = (int *)csw_Malloc (nlines * 2 * sizeof(int));
    if (linepts == NULL) {
        FREE_LOCAL_DATA
        grd_set_err (1);
        return -1;
    }
    linetypes = linepts + nlines;

    n = 0;
    for (i=0; i<nlines; i++) {
        if (i < ncons) {
            linetypes[i] = GRD_CONTOUR_CONSTRAINT;
            linepts[i] = conpts[i];
            n += conpts[i];
        }
        else {
            linetypes[i] = GRD_DISCONTINUITY_CONSTRAINT;
            np = faultpts[i-ncons];
            linepts[i] = np;
            n += np;
        }
    }

    xlines = (double *)csw_Malloc (n * 3 * sizeof(double));
    if (xlines == NULL) {
        FREE_LOCAL_DATA
        grd_set_err (1);
        return -1;
    }
    ylines = xlines + n;
    zlines = ylines + n;

/*
 * Fill the constraint line arrays with the contours
 * and the faults.
 */
    nf = 0;
    n = 0;
    for (i=0; i<nlines; i++) {

        np = linepts[i];
        dsize = np * sizeof(double);

        if (i < ncons) {
            cptr = clines + i;
            memcpy (xlines+n, cptr->x, dsize);
            memcpy (ylines+n, cptr->y, dsize);
            for (j=0; j<np; j++) {
                zlines[n] = cptr->zlev;
                n++;
            }
        }

        else {
            memcpy (xlines+n, xfaults+nf, dsize);
            memcpy (ylines+n, yfaults+nf, dsize);
            memcpy (zlines+n, zfaults+nf, dsize);
            n += np;
            nf += np;
        }
    }

/*
 * Allocate space for grid input points, which consist of the
 * original contour points, the original data points and enough
 * space to grow by a factor of 10.
 */
    n = nin + numconpts;
    n *= 11;
    xdat = (CSW_F *)csw_Malloc (n * 4 * sizeof(CSW_F));
    if (xdat == NULL) {
        FREE_LOCAL_DATA
        grd_set_err (1);
        return -1;
    }
    ydat = xdat + n;
    zdat = ydat + n;
    edat = zdat + n;

    dxdat = (double *)csw_Malloc (n * 3 * sizeof(double));
    if (dxdat == NULL) {
        FREE_LOCAL_DATA
        grd_set_err (1);
        return -1;
    }
    dydat = dxdat + n;
    dzdat = dydat + n;

/*
 * Put the data points and contour line points
 * into the grid data arrays.
 */
    zmin = 1.e30;
    zmax = -1.e30;
    n = 0;
    for (i=0; i<nin; i++) {
        xdat[n] = (CSW_F)xin[i];
        ydat[n] = (CSW_F)yin[i];
        zdat[n] = (CSW_F)zin[i];
        if (zin[i] < zmin) zmin = zin[i];
        if (zin[i] > zmax) zmax = zin[i];
        n++;
    }

    for (i=0; i<numconpts; i++) {
        xdat[n] = (CSW_F)xlines[i];
        ydat[n] = (CSW_F)ylines[i];
        zdat[n] = (CSW_F)zlines[i];
        if (zlines[i] < zmin) zmin = zlines[i];
        if (zlines[i] > zmax) zmax = zlines[i];
        n++;
    }

    ndata = n;
    if (zmax <= zmin) {
        ztiny = 0.0;
    }
    else {
        ztiny = (zmax - zmin) / 1000.0;
    }

/*
 * Put the original points into the expandable trimesh
 * data point arrays.
 */
    nt = 0;
    for (i=0; i<nin; i++) {
        dxdat[nt] = xin[i];
        dydat[nt] = yin[i];
        dzdat[nt] = zin[i];
        nt++;
    }

/*
 * Calculate trimesh and grid.  For each iteration, evaluate
 * the mid points of non constraint edges of the trimesh via
 * back interpolation from the grid.  The grid value is combined
 * with the linear interpolation along the trimesh edge and the
 * combined result is used as an additional point in the next
 * grid calculation.  If no trimesh edges are long enough to
 * be split, the loop exits.
 */
    for (ido = 0; ido<3; ido++) {

    /*
     * Calculate a grid.
     */
        istat = grd_calc_ptr->grd_calc_grid
                  (xdat, ydat, zdat, edat, ndata,
                   grid, mask, NULL, ncol, nrow,
                   (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2, options);
        if (istat == -1) {
            FREE_LOCAL_DATA
            grd_set_err (1);
            return -1;
        }

    /*
     * Calculate a constrained trimesh.
     */
        istat = grd_triangle_ptr->grd_calc_trimesh (dxdat, dydat, dzdat, nt,
                                  xlines, ylines, zlines,
                                  linepts, linetypes, -nlines,
                                  &nodes, &edges, &triangles,
                                  &numnodes, &numedges, &numtriangles);
        if (istat == -1) {
            FREE_LOCAL_DATA
            grd_set_err (1);
            return -1;
        }

    /*
     * Split any non constraint edges in the trimesh
     * to create new points for the next grid calc pass.
     */
        done = 0;
        n = ndata;
        for (i=0; i<numedges; i++) {

            eptr = edges + i;
            if (eptr->deleted ||  eptr->flag) {
                continue;
            }
            n1 = eptr->node1;
            n2 = eptr->node2;
            if (n1 < 0  ||  n2 < 0  ||
                n1 >= numnodes  ||  n2 >= numnodes) {
                continue;
            }

            dx1 = nodes[n1].x;
            dy1 = nodes[n1].y;
            dz1 = nodes[n1].z;
            dx2 = nodes[n2].x;
            dy2 = nodes[n2].y;
            dz2 = nodes[n2].z;

            dx = dx2 - dx1;
            dy = dy2 - dy1;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);

            if (dist < dtest) {
                continue;
            }
            dz = dz2 - dz1;
            if (dx < 0.0) dz = -dz;

            wgt = dtest / dist;

            xt = (CSW_F)((dx1 + dx2) / 2.0);
            yt = (CSW_F)((dy1 + dy2) / 2.0);
            zt = 1.e30f;

            if (nfaults > 0) {
                grd_fault_ptr->grd_back_interpolate_faulted
                                     (grid, ncol, nrow,
                                      (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
                                      &xt, &yt, &zt, 1, GRD_BICUBIC);
            }
            else {
                grd_arith_ptr->grd_back_interpolate (grid, ncol, nrow,
                                      (CSW_F)x1, (CSW_F)y1, (CSW_F)x2, (CSW_F)y2,
                                      NULL, 0,
                                      &xt, &yt, &zt, 1, GRD_BICUBIC);
            }

            if (zt < 1.e20) {

            /*
             * If the end points of the edge are at the same
             * z value, use the grid only for the mid point.
             * This addresses the problem of edges connecting
             * different points in the same contour in a
             * chord like fashion.
             */
                if (dz > ztiny) {
                    zt2 = (CSW_F) ((dz1 + dz2) / 2.0);
                    zt = (CSW_F) ((zt2 * wgt + zt) / (1.0 + wgt));
                }
                xdat[n] = xt;
                ydat[n] = yt;
                zdat[n] = zt;
                n++;
                dxdat[nt] = (double)xt;
                dydat[nt] = (double)yt;
                dzdat[nt] = (double)zt;
                nt++;
                done++;
            }
        }

        if (done == 0) {
            break;
        }

        ndata = n;

    } /* end of ido loop */

    FREE_LOCAL_DATA

    return 1;

}
#undef FREE_LOCAL_DATA




/*
 **************************************************************************************

  If a point is on the edge of a grid cell (or very close to on the edge) use the cell
  that has no null valued corners, if there is one, for the cell containing the point.

  Returns 1 if the point is very close to a grid node, otherwise returns 0.

 **************************************************************************************
*/

int CSWGrdUtils::FindBestColumnAndRow
                                (double x, double y,
                                 CSW_F *grid, int ncol, int nrow, int nskip,
                                 double x1, double y1,
                                 double xsp, double ysp,
                                 int *jcol, int *irow)
{
    int           i, j, k, colmax, rowmax;
    double        xt, yt, dx, dy, tiny;
    int           onleft, onright, onbottom, ontop;

    colmax = (ncol - 1) - (ncol - 1) % nskip;
    rowmax = (nrow - 1) - (nrow - 1) % nskip;

    tiny = (xsp + ysp) / 1000.0;

/*
 * Calculate the closest column and row.
 */
    j = (int) ((x - x1) / xsp + 0.5);
    j *= nskip;
    i = (int) ((y - y1) / ysp + 0.5);
    i *= nskip;

    if (j < 0) j = 0;
    if (i < 0) i = 0;
    if (j > colmax) j = colmax;
    if (i > rowmax) i = rowmax;

    xt = j * xsp / nskip + x1;
    yt = i * ysp / nskip + y1;

    dx = x - xt;
    dy = y - yt;

    if (-tiny < dx && dx < tiny && -tiny < dy && dy < tiny) {
        *jcol = j;
        *irow = i;
        return 1; /* point is on a grid node. */
    }

/*
 * Calculate the lower left column and row.
 */
    j = (int) ((x - x1) / xsp);
    j *= nskip;
    i = (int) ((y - y1) / ysp);
    i *= nskip;

    if (j > colmax - nskip) j = colmax - nskip;
    if (i > rowmax - nskip) i = rowmax - nskip;
    if (j < 0) j = 0;
    if (i < 0) i = 0;

    xt = j * xsp / nskip + x1;
    yt = i * ysp / nskip + y1;

    dx = x - xt;
    dy = y - yt;

    onleft = -tiny < dx && dx < tiny;
    onright = xsp - tiny < dx && dx < xsp + tiny;
    onbottom = -tiny < dy && dy < tiny;
    ontop = ysp - tiny < dy && dy < ysp + tiny;

/*
 * If the point is not on an edge, return the raw column and row.
 */
    if (!onleft && !onright && !onbottom && !ontop) {
        *jcol = j;
        *irow = i;
        return 0;
    }

/*
 * If the cell has no null values, return the raw column and row.
 */
    k = i * ncol + j;
    if (grid[k] < 1.e20  &&
        grid[k+nskip] < 1.e20  &&
        grid[k+ncol*nskip] < 1.e20  &&
        grid[k+ncol*nskip + nskip] < 1.e20) {
        *jcol = j;
        *irow = i;
        return 0;
    }

    if (onleft) {
        j -= nskip;
        if (j < 0) j = 0;
    }

    else if (onright) {
        j += nskip;
        if (j > colmax - nskip) j = colmax - nskip;
    }

    if (onbottom) {
        i -= nskip;
        if (i < 0) i = 0;
    }

    else if (ontop) {
        i += nskip;
        if (i > rowmax - nskip) i = rowmax - nskip;
    }

    if (j < 0) j = 0;
    if (i < 0) i = 0;

    *jcol = j;
    *irow = i;

    return 0;

}




/*
 ************************************************************************************

                        g r d _ v e c t o r _ a n g l e

 ************************************************************************************

  Calculate the angle between two vectors with a common endpoint at x0, y0, z0.
  The result is returned in degrees.

*/
#define RAD_TO_DEG        (180.0 / 3.145926)
double CSWGrdUtils::grd_vector_angle (double x0, double y0, double z0,
                         double x1, double y1, double z1,
                         double x2, double y2, double z2)
{
    double          dx1, dy1, dz1, dx2, dy2, dz2;
    double          dotprod, distprod, dist1, dist2, cosang, ang;

    dx1 = x1 - x0;
    dy1 = y1 - y0;
    dz1 = z1 - z0;
    dx2 = x2 - x0;
    dy2 = y2 - y0;
    dz2 = z2 - z0;

    dotprod = dx1*dx2 + dy1*dy2 + dz1*dz2;

    dist1 = dx1*dx1 + dy1*dy1 + dz1*dz1;
    dist1 = sqrt (dist1);
    dist2 = dx2*dx2 + dy2*dy2 + dz2*dz2;
    dist2 = sqrt (dist2);

    distprod = dist1 * dist2;
    if (distprod <= 0.0) {
        return 0.0;
    }

    cosang = dotprod / distprod;
    if (cosang > 0.999999) cosang = 0.999999;
    if (cosang < -0.999999) cosang = -0.999999;

    ang = acos (cosang);

    ang *= RAD_TO_DEG;

    return ang;

}



/*
 *************************************************************************************

               g r d _ c o m p a r e _ g e o m s

 *************************************************************************************

  Return 1 if the two specified geometries are very nearly identical or
  return zero if they are not.

*/

int CSWGrdUtils::grd_compare_geoms (
    double x1, double y1, double x2, double y2, int nc1, int nr1,
    double x3, double y3, double x4, double y4, int nc2, int nr2)
{
    double    tiny, dx;

/*
 * If the number of rows and columns are not the same for
 * each grid, the geometries are different.
 */
    if (nc1 != nc2  ||  nr1 != nr2) {
        return 0;
    }

/*
 * Use a value of about 1 part in 10000 of the average grid dimension
 * as the "zero" distance.  If any corner coordinates differ by more
 * than the zero distance, the geometries are different.
 */
    tiny = x2 - x1 + y2 - y1 + x4 - x3 + y4 - y3;
    tiny /= 40000.0;

    dx = x3 - x1;
    if (dx < -tiny  ||  dx > tiny) {
        return 0;
    }
    dx = x4 - x2;
    if (dx < -tiny  ||  dx > tiny) {
        return 0;
    }
    dx = y3 - y1;
    if (dx < -tiny  ||  dx > tiny) {
        return 0;
    }
    dx = y4 - y2;
    if (dx < -tiny  ||  dx > tiny) {
        return 0;
    }

    return 1;

}



/*
  ****************************************************************

               g r d _ f i l l _ n u l l s _ n e w

  ****************************************************************

    Fill in nulls in a grid by averaging the nearest non null values.
    If the flag is not zero, then the zero values are filled in and
    the hard nulls are not changed.  If the flag  is zero, hard nulls
    are filled in.  If you want both, call this twice, first with a
    flag of -1 for negative thickness or 1 for positive thickness.
    The second call should have flag = 0 and the output from the first
    call should be used as the input for the second call.

*/

int CSWGrdUtils::grd_fill_nulls_new
                       (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
                        CSW_F *gridout, char *maskout, int flag)

{
    int           i, istat, nstat, nc2, nr2, icoarse;
    int           j, k, ki, kj, offset, off2, icmin, eflag;
    CSW_F         sum, sum2, tiny, z1, z2, hard_null_val;
    CSW_F         positive_hard_null_val, fudge;
    CSW_F         *grid1 = NULL, *grid2 = NULL,
                  *grid3 = NULL, *grid4 = NULL, nval, nval2;
    CSW_F         rat, x1, y1, x2, y2;

    char          *null_process_flags = NULL;
    int           i1, i2, j1, j2, ii, jj, kk;
    int           offset2;

    auto fscope = [&]()
    {
        TinySum = 0.0f;
        csw_Free (grid1);
        grid1 = NULL;
        grid2 = NULL;
        grid3 = NULL;
        grid4 = NULL;
        csw_Free (null_process_flags);
        null_process_flags = NULL;
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    obvious errors
*/
    if (!grid) {
        grd_set_err (3);
        return -1;
    }

    if (ncol < 2  ||  nrow < 2) {
        grd_set_err (4);
        return -1;
    }

/*
    If the specified null value is a "reasonable" number,
    then set it to 1.e20.
*/
    if (nullval > -1.e10  &&  nullval < 1.e10) {
        nullval = WildFloat;
    }

/*
    make nulls positive if needed
*/
    if (nullval < 0.0) {
        nval = -nullval / 10.0f;
        hard_null_val =  -1.e30f;
    }
    else {
        nval = nullval / 10.0f;
        hard_null_val =  1.e30f;
    }
    positive_hard_null_val = hard_null_val;
    if (hard_null_val < 0.0f)
        positive_hard_null_val = -hard_null_val;

/*
    Count how many nulls are in the input grid.
*/
    nstat = 0;
    for (i=0; i< ncol*nrow; i++) {
        if (-nval < grid[i] && grid[i] < nval) {
            nstat++;
        }
    }

    if (nstat == 0) {
    /*
      There must be at least one non null in the input grid.
    */
        grd_set_err (2);
        return -1;
    }

    if (nstat == ncol*nrow) {
    /*
        No null values found.
        Copy the input grid to gridout if needed.
    */
        if (gridout) {
            csw_memcpy (
                (char *)gridout,
                (char *)grid,
                ncol*nrow*sizeof(CSW_F)
            );
        }

        return 1;
    }

    nval2 = nval / 100.0f;

/*
 * Allocate a char mask for recording if a node needs
 * to be processed or not.
 */
    null_process_flags = (char *)csw_Calloc (ncol*nrow*sizeof(char));
    if (null_process_flags == NULL) {
        return -1;
    }

/*
 * All non null values are set to 2. All neighbors of non nulls set to 1.
 * All other nodes set to zero.
 */
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        i1 = i - 1;
        i2 = i + 1;
        if (i1 < 0) i1 = 0;
        if (i2 > nrow - 1) i2 = nrow - 1;
        for (j=0; j<ncol; j++) {
            k = offset + j;
            if (grid[k] > nval2) {
                continue;
            }
            null_process_flags[k] = 2;
            j1 = j - 1;
            j2 = j + 1;
            if (j1 < 0) j1 = 0;
            if (j2 > ncol - 1) j2 = ncol - 1;
            for (ii=i1; ii<=i2; ii++) {
                offset2 = ii * ncol;
                for (jj=j1; jj<=j2; jj++) {
                    kk = offset2+jj;
                    if (grid[kk] > nval2) {
                        null_process_flags[kk] = 1;
                    }
                }
            }
        }
    }

/*
    allocate work space memory
*/
MSL
    grid1 = (CSW_F *)csw_Malloc (ncol * nrow * 4 * sizeof(CSW_F));
    if (!grid1) {
        grd_set_err (1);
        return -1;
    }
    grid2 = grid1 + ncol * nrow;

/*
    copy input grid to workspace
*/
    csw_memcpy ((char *)grid1, (char *)grid, ncol*nrow*sizeof(CSW_F));

/*
    make nulls positive if needed
    Also get range of non-null values.
*/
    z1 = 1.e30f;
    z2 = -1.e30f;
    for (i=0; i<ncol*nrow; i++) {
        if (grid1[i] < -nval) {
            grid1[i] = -nullval;

        }
        else if (grid1[i] < nval) {
            if (grid1[i] < z1) z1 = grid1[i];
            if (grid1[i] > z2) z2 = grid1[i];
        }
    }

/*
    If the program is in parameter error simulation mode,
    return success (which is not expected) here.
*/
    if (grd_simulation()) {
        return 1;
    }

    tiny = (z2 - z1) / 50000.0f;
    fudge = tiny * 100.0f;
    TinySum = tiny / 2.0f;
    if (TinySum > 1.e-10) TinySum = 1.e-10f;

    nstat = ncol * nrow - nstat;
    icmin = 2;
    if (ncol*nrow > 30000)
        icmin = 4;
    if (ncol*nrow > 200000)
        icmin = 8;

/*
    The coarse grid geometry is determined by the ratio of
    null values to nodes in the original grid.
*/
    rat = (CSW_F)nstat / (CSW_F)(ncol*nrow);
    rat += 0.2f;
    icoarse = (int)(rat * 5.0f);
    icoarse++;
    if (icoarse < 4) {
        icoarse = 2;
    }
    else if (icoarse < 7) {
        icoarse = 4;
    }
    else if (icoarse < 13) {
        icoarse = 8;
    }
    else {
        icoarse = 16;
    }

    if (icoarse < icmin) {
        icoarse = icmin;
    }

    if (flag != 0) {
        icoarse = 2;
    }

/*
    build the maskout array if needed
    initially these are set to "inside uncontrolled"
*/
    if (maskout) {
        for (i=0; i<ncol*nrow; i++) {
            if (grid1[i] > nval) {
                maskout[i] = 1;
            }
        }

    /*
        Scan the maskout array from the edges to set
        "outside uncontrolled" nodes.

        First scan a row at a time.
    */
        for (i=0; i<nrow; i++) {
            offset = i * ncol;
            for (j=0; j<ncol; j++) {
                k = offset + j;
                if (maskout[k] == 2) {
                    maskout[k] = 1;
                }
                else {
                    break;
                }
            }
            for (j=ncol-1; j>=0; j--) {
                k = offset + j;
                if (maskout[k] == 2) {
                    maskout[k] = 1;
                }
                else {
                    break;
                }
            }
        }

    /*
        Next scan a column at a time.
    */
        for (j=0; j<ncol; j++) {
            for (i=0; i<nrow; i++) {
                k = i * ncol + j;
                if (maskout[k] == 2) {
                    maskout[k] = 1;
                }
                else {
                    break;
                }
            }
            for (i=nrow-1; i>=0; i--) {
                k = i * ncol + j;
                if (maskout[k] == 2) {
                    maskout[k] = 1;
                }
                else {
                    break;
                }
            }
        }
    }

/*
    Fill in the nulls by extending gradients from the non null nearest
    neighbors.  Do this by expanding one level at a time.
*/
    nstat = 0;
    eflag = 1;
    for (i=0; i<=icoarse; i++) {

        istat = grd_expand_one_level_new (
            grid1, grid2, ncol, nrow, nval, eflag, flag, null_process_flags);
        if (istat == 0) {
            nstat = 1;
            break;
        }
        eflag++;

        istat = grd_expand_one_level_new (
            grid2, grid1, ncol, nrow, nval, eflag, flag, null_process_flags);
        if (istat == 0) {
            nstat = 2;
            break;
        }
        eflag++;

    }

    if (nstat == 1) {
        csw_memcpy ((char *)grid1, (char *)grid2, ncol*nrow*sizeof(CSW_F));
    }

/*
    resample at coarser interval and fill in the rest of
    the null values at that coarse interval
*/
    if (nstat == 0  &&  flag == 0) {

    /*
        This line is only to avoid a bogus lint warning.
    */
        if (icoarse < 2) icoarse = 2;

        nc2 = ncol / icoarse + 1;
        nr2 = nrow / icoarse + 1;
        x1 = 0.0f;
        y1 = 0.0f;
        x2 = (CSW_F)ncol;
        y2 = (CSW_F)nrow;

        grid3 = grid2 + ncol * nrow;
        grid4 = grid3 + nc2 * nr2;

        grd_arith_ptr->grd_resample_grid (grid1, NULL, ncol, nrow,
                           x1, y1, x2, y2,
                           NULL, 0,
                           grid3, NULL, nc2, nr2,
                           x1, y1, x2, y2, GRD_BILINEAR);

    /*
     * Calculate a null_process flags array for the coarse grid.
     */
        memset (null_process_flags, 0, ncol * nrow * sizeof(char));
        for (i=0; i<nr2; i++) {
            offset = i * nc2;
            i1 = i - 1;
            i2 = i + 1;
            if (i1 < 0) i1 = 0;
            if (i2 > nr2 - 1) i2 = nr2 - 1;
            for (j=0; j<nc2; j++) {
                k = offset + j;
                if (grid3[k] > nval) {
                    continue;
                }
                null_process_flags[k] = 2;
                j1 = j - 1;
                j2 = j + 1;
                if (j1 < 0) j1 = 0;
                if (j2 > nc2 - 1) j2 = nc2 - 1;
                for (ii=i1; ii<=i2; ii++) {
                    offset2 = ii * nc2;
                    for (jj=j1; jj<=j2; jj++) {
                        kk = offset2+jj;
                        if (grid3[kk] > nval2) {
                            null_process_flags[kk] = 1;
                        }
                    }
                }
            }
        }

        nstat = 0;
        eflag = 0;
        for (i=0; i<nc2+nr2; i++) {

            istat = grd_expand_one_level_new (
                grid3, grid4, nc2, nr2, nval, eflag, flag, null_process_flags);
            if (istat == 0) {
                nstat = 1;
                break;
            }

            istat = grd_expand_one_level_new (
                grid4, grid3, nc2, nr2, nval, eflag, flag, null_process_flags);
            if (istat == 0) {
                nstat = 2;
                break;
            }

            eflag = 0;

        }

        if (nstat == 0) {
            grd_set_err (2);
            TinySum = 0.0f;
            return -1;
        }

        if (nstat == 1) {
            csw_memcpy ((char *)grid3, (char *)grid4, nc2*nr2*sizeof(CSW_F));
        }

        grd_arith_ptr->grd_resample_grid (grid3, NULL, nc2, nr2,
                           x1, y1, x2, y2,
                           NULL, 0,
                           grid2, NULL, ncol, nrow,
                           x1, y1, x2, y2, GRD_BILINEAR);

        for (i=0; i<ncol*nrow; i++) {
            if (grid1[i] >= nval) {
                grid1[i] = grid2[i];
            }
        }

    }

/*
    Smooth grid1 and then combine it with original grid, using
    smoothed grid1 values where the original grid was null.
*/
    for (i=0; i<nrow; i++) {

        offset = i * ncol;

        for (j=0; j<ncol; j++) {

            k = offset + j;

            if (grid1[k] >= nval) {
                grid2[k] = nval * 100.0f;
                continue;
            }
            sum = 0.0f;
            sum2 = 0.0f;
            for (ki=i-1; ki<=i+1; ki++) {
                if (ki<0  ||  ki>=nrow) {
                    continue;
                }
                off2 = ki*ncol;
                for (kj=j-1; kj<=j+1; kj++) {
                    if (kj>=0  &&  kj<ncol) {
                        if (grid1[off2+kj] < nval) {
                            sum += grid1[off2+kj];
                            sum2 += 1.0f;
                        }
                    }
                }
            }

            if (sum2 > 0.0) {
                if (sum < TinySum  &&  sum > -TinySum) {
                    sum = 0.0;
                }
                grid2[k] = sum / sum2;
            }
            else {
                grid2[k] = positive_hard_null_val;
            }

        }

    }

    if (flag == 1) {
        for (i=0; i<ncol*nrow; i++) {
            if (grid[i] == 0.0f) {
                if (grid2[i] <= fudge) {
                    grid2[i] = fudge;
                }
            }
        }
    }
    else if (flag == -1) {
        for (i=0; i<ncol*nrow; i++) {
            if (grid[i] == 0.0f) {
                if (grid2[i] >= -fudge) {
                    grid2[i] = -fudge;
                }
            }
        }
    }

    for (i=0; i<nrow; i++) {

        offset = i * ncol;

        for (j=0; j<ncol; j++) {

            k = offset + j;
            if (grid2[k] >= nval) {
                grid1[k] = nval * 100.0f;
                continue;
            }
            sum = 0.0f;
            sum2 = 0.0f;
            for (ki=i-1; ki<=i+1; ki++) {
                if (ki<0  ||  ki>=nrow) {
                    continue;
                }
                off2 = ki*ncol;
                for (kj=j-1; kj<=j+1; kj++) {
                    if (kj>=0  &&  kj<ncol) {
                        if (grid2[off2+kj] < nval) {
                            sum += grid2[off2+kj];
                            sum2 += 1.0f;
                        }
                    }
                }
            }

            if (sum2 > 0.0) {
                if (sum < TinySum  &&  sum > -TinySum) {
                    sum = 0.0;
                }
                grid1[k] = sum / sum2;
            }
            else {
                grid1[k] = positive_hard_null_val;
            }

        }

    }

    if (flag == 1) {
        for (i=0; i<ncol*nrow; i++) {
            if (grid[i] == 0.0f) {
                if (grid1[i] <= 0.0f) {
                    grid1[i] = fudge;
                }
            }
        }
    }
    else if (flag == -1) {
        for (i=0; i<ncol*nrow; i++) {
            if (grid[i] == 0.0f) {
                if (grid1[i] >= 0.0f) {
                    grid1[i] = -fudge;
                }
            }
        }
    }

    if (flag == 0) {
        for (i=0; i<ncol*nrow; i++) {
            if (grid[i] < nullval  &&  grid[i] > -nullval) {
                grid1[i] = grid[i];
            }
        }
    }

    else {
        for (i=0; i<ncol*nrow; i++) {
        /*  negative thickness  */
            if (flag == 1) {
                if (grid[i] < 0.0f  ||  grid[i] > tiny) {
                    grid1[i] = grid[i];
                }
            }
        /*  positive thickness  */
            else {
                if (grid[i] < -tiny  ||  grid[i] > 0.0f) {
                    grid1[i] = grid[i];
                }
            }
        }
    }

/*
    copy the grid1 work grid to either grid or gridout as needed
*/
    if (gridout) {
        csw_memcpy ((char *)gridout, (char *)grid1, ncol*nrow*sizeof(CSW_F));
    }
    else {
        csw_memcpy ((char *)grid, (char *)grid1, ncol*nrow*sizeof(CSW_F));
    }

    return 1;

}  /*  end of function grd_fill_nulls_new  */






/*
  ****************************************************************

        g r d _ e x p a n d _ o n e _ l e v e l _ n e w

  ****************************************************************

    Average the neighbors of null nodes to assign a value to the
  null nodes.  If the eflag parameter is non zero, then an extrapolative
  average is combined with the simple average.

*/

int CSWGrdUtils::grd_expand_one_level_new
                         (CSW_F *grid1, CSW_F *grid2, int ncol, int nrow,
                          CSW_F nval, int eflag, int flag, char *process_flags)

{
    int          i, j, ii, jj, i1, i2, j1, j2, kk, k, k2, istat,
                 nt, n2, ndone, im, jm, km, offset, offset2;
    double       sum, sum2, zt, zt2, double_z;

    if (flag != 0) {
        istat = ExpandForZero
            (grid1, grid2, ncol, nrow, nval, eflag, flag);
        return istat;
    }

    ndone = 0;
    sum = 0.0f;
    sum2 = 0.0f;

    if (eflag > 10) eflag = 0;

/*
 * All nodes with process_flags of 1 should be interpolated from
 * neighbors.
 */
    for (i=0; i<nrow; i++) {

        offset = i * ncol;

        i1 = i - 1;
        i2 = i + 1;
        if (i1 < 0) i1 = 0;
        if (i2 > nrow-1) i2 = nrow - 1;

        for (j=0; j<ncol; j++) {

            k = offset + j;

            if (process_flags[k] == 0) {
                grid2[k] = (CSW_F)(nval * 100.0);
                continue;
            }
            if (process_flags[k] == 2) {
                grid2[k] = grid1[k];
                continue;
            }

            nt = 0;
            n2 = 0;
            if (grid1[k] > nval) {
                sum = 0.0f;
                sum2 = 0.0f;
                for (ii=i-1; ii<=i+1; ii++) {
                    if (ii < 0  ||  ii >= nrow) continue;
                    offset2 = ii * ncol;
                    for (jj=j-1; jj<=j+1; jj++) {
                        if (jj < 0  ||  jj >= ncol) continue;
                        k2 = offset2 + jj;
                        if (grid1[k2] < nval) {
                            nt++;
                            sum += grid1[k2];
                        }
                    }
                }
                if (eflag) {
                    for (ii=i-2; ii<=i+2; ii+=2) {
                        if (ii < 0  ||  ii >= nrow) continue;
                        offset2 = ii * ncol;
                        im = (ii + i) / 2;
                        im *= ncol;
                        for (jj=j-2; jj<=j+2; jj+=2) {
                            if (jj < 0  ||  jj >= ncol) continue;
                            k2 = offset2 + jj;
                            jm = (jj + j) / 2;
                            km = im + jm;
                            if (grid1[k2] < nval  &&  grid1[km] < nval) {
                                n2++;
                                zt = 2.0f * grid1[km] - grid1[k2];
                                if (flag == 1  &&  zt < 0.0f) {
                                    zt = -zt;
                                }
                                else if (flag == -1  &&  zt > 0.0f) {
                                    zt = -zt;
                                }
                                sum2 += zt;
                            }
                        }
                    }
                }
            }

            if (flag == 1  &&  sum < 0.0f) {
                sum = -sum;
            }
            else if (flag == -1  &&  sum > 0.0f) {
                sum = -sum;
            }
            if (n2 > 0) {
                if (nt > 0) {
                    zt = sum / nt;
                    zt2 = sum2 / n2;
                    double_z = (zt2 + eflag * zt) / (1.0 + eflag);
                }
                else {
                    double_z = sum2 / n2;
                }
                ndone++;
            }

            else {
                if (nt > 0) {
                    double_z = sum / nt;
                    ndone++;
                }
                else {
                    double_z = nval * 100.0;
                }
            }

          /*
           * bug 8383   if output grid value is very near zero, set it
           *            to exactly zero.
           *            also bug 9254, bug 9256, bug 9178
           */
            if (-Z_ABSOLUTE_TINY < double_z && double_z < Z_ABSOLUTE_TINY) {
                double_z = 0.0;
            }

            grid2[k] = (CSW_F) double_z;

        }
    }

/*
 * Reset the process_flags to 2 for successfully interpolated nodes
 * that were previously 1 nodes.  Reset any zero flagged neighbors
 * of the new 2 nodes to 1 nodes.
 */
    for (i=0; i<nrow; i++) {
        offset = i * ncol;
        i1 = i - 1;
        i2 = i + 1;
        if (i1 < 0) i1 = 0;
        if (i2 > nrow-1) i2 = nrow - 1;
        for (j=0; j<ncol; j++) {
            k = offset + j;

            if (process_flags[k] != 1) {
                continue;
            }

            if (grid2[k] > nval) {
                continue;
            }

            process_flags[k] = 2;

            j1 = j - 1;
            j2 = j + 1;
            if (j1 < 0) j1 = 0;
            if (j2 > ncol-1) j2 = ncol - 1;

            for (ii=i1; ii<=i2; ii++) {
                offset2 = ii * ncol;
                for (jj=j1; jj<=j2; jj++) {
                    kk = offset2 + jj;
                    if (process_flags[kk] == 0) {
                        process_flags[kk] = 1;
                    }
                }
            }
        }
    }

    return ndone;

}  /*  end of function grd_expand_one_level_new  */
