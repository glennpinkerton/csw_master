
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_stats.cc

    This file has some geostatistical functions used in support
    of the grid calculations.  The functions in this file are built
    into the libsurf.a library.

    These are not intended to be part of the gridding API.  These
    are only called from other grid calculation functions.
*/

#include <float.h>
#include <math.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/simulP.h"
#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/gpf_spline.h"

#include "csw/surfaceworks/private_include/grd_stats.h"


/*
  ****************************************************************

            g r d _ g l o b a l _ a n i s o t r o p y

  ****************************************************************

  Calculate a single preferred strike and strike power for the
  entire data set.  If no preferred direction is found, the strike
  is set to -1000 and the strike power is set to 2.

  Generally, you need 50 or more points to be able to calculate
  a preferred direction.  If there are too few point pairs to
  divide into angular bands, no preferred direction is calculated.
  If there are less than 10 points, I don't even make an attempt.

*/

int CSWGrdStats::grd_global_anisotropy (CSW_F *xin, CSW_F *yin, CSW_F *zin, int nin, int nbins,
                           CSW_F *strike, int *power, CSW_F *aratio, int thickflag)
{
    CSW_F            *vardata = NULL, *varang[NDIR], knees[NDIR],
                     varmax[NDIR];
    CSW_F            x0, y0, z0, dx, dy, dz, dd, dmax, zmax, sum,
                     x1, y1, x2, y2, ang, dist, dang, dmax2;
    int              ndir, ndir1, i, j, k, ivar, ibin,
                     istat, n, nw;
    int              *countdata = NULL, *countang[NDIR];
    int              *count = NULL;
    CSW_F            *var = NULL, xdum, ydum;
    CSW_F            *x = NULL, *y = NULL, *z = NULL,
                     *xw = NULL, *yw = NULL, *zw = NULL;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (xw);
        csw_Free (vardata);
        csw_Free (countdata);
        if (bsuccess == false) {
            *strike = -1000.f;
            *power = 2;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Initialize some stuff to suppress lint messages
*/
    memset ((char *)knees, 0, NDIR * sizeof(CSW_F));
    memset ((char *)varmax, 0, NDIR * sizeof(CSW_F));
    memset ((char *)varang, 0, NDIR * sizeof(CSW_F));
    memset ((char *)countang, 0, NDIR * sizeof(int));

    n = nin;

/*
    obvious errors.
*/
    *strike = -1000.f;
    *power = 2;
    if (n < 10) return 1;

    if (nbins > MAX_STATS_BINS) nbins = MAX_STATS_BINS;
    if (n < 100) {
        if (nbins > n / 4) {
            nbins = n / 4;
        }
    }
    if (nbins < 10) nbins = 10;

/*
    Compress the number of points if there are a large number.
*/
    x = xin;
    y = yin;
    z = zin;

    xw = NULL;
    yw = NULL;
    zw = NULL;
    nw = 0;

    if (n > MAX_GLOBAL_POINTS) {
MSL
        xw = (CSW_F *)csw_Malloc (3 * MAX_GLOBAL_POINTS * sizeof(CSW_F));
        if (!xw) {
            x = xin;
            y = yin;
            z = zin;
            n = nin;
        }
        else {
            yw = xw + MAX_GLOBAL_POINTS;
            zw = yw + MAX_GLOBAL_POINTS;
            istat = grd_utils_ptr->grd_collect_points
                     (xin, yin, zin, nin, MAX_GLOBAL_POINTS,
                      xw, yw, zw, &nw);
            if (istat == -1) {
                x = xin;
                y = yin;
                z = zin;
                n = nin;
            }
            else {
                x = xw;
                y = yw;
                z = zw;
                n = nw;
            }
        }
    }

/*
    The number of directional wedges is a function of
    the number of points available.  More points allow
    for more directions.  If any wedges are not sufficiently
    populated to make a good variogram, no anisotropy will be
    calculated.
*/
    i = nbins * 5;
    ndir = n * n / i;
    if (ndir < 4) ndir = 4;
    if (ndir > NDIR) ndir = NDIR;
    ndir1 = ndir - 1;

/*
    Allocate data for the directional variograms.
*/
MSL
    vardata = (CSW_F *)csw_Malloc (ndir * nbins * sizeof(CSW_F));
    if (!vardata) {
        return -1;
    }
    for (i=0; i<ndir; i++) {
        varang[i] = vardata + i * nbins;
    }

MSL
    countdata = (int *)csw_Calloc (ndir * nbins * sizeof(int));
    if (!countdata) {
        return -1;
    }
    for (i=0; i<ndir; i++) {
        countang[i] = countdata + i * nbins;
    }

/*
    Initialize variograms to a "null" value.
*/
    for (i=0; i<ndir*nbins; i++) {
        vardata[i] = 1.e30f;
    }

/*
    Find the x,y limits and use them to get the width
    of each bin in the variograms.
*/
    gpf_xandylimits (x, y, n, &x1, &y1, &x2, &y2);

    dx = x2 - x1;
    dy = y2 - y1;

    if (dx <= ABSOLUTE_TINY  ||  dy <= ABSOLUTE_TINY) {
        return 1;
    }

    dmax = dx * dx + dy * dy;
    dmax = (CSW_F)sqrt ((double)dmax);

    dd = dmax / (CSW_F)nbins;
    dd *= 1.01f;

/*
    dang is the angular width of each wedge for
    accumulating directional variogram points.
*/
    dang = PI / (CSW_F)ndir;

/*
    Calculate the variance (delta z squared) vs distance pairs
    and put them into the appropriate directional variograms.
*/
    for (i=0; i<n; i++) {

        x0 = x[i];
        y0 = y[i];
        z0 = z[i];

        if (x0 < x1  ||  x0 > x2  ||
            y0 < y1  ||  y0 > y2) {
            continue;
        }

        if (thickflag == 1  &&  z0 <= 0.0f) continue;

        for (j=0; j<n; j++) {

            if (i == j) continue;
            if (thickflag == 1  &&  z[j] <= 0.0f) continue;

            if (x[j] < x1  ||  x[j] > x2  ||
                y[j] < y1  ||  y[j] > y2) {
                continue;
            }

        /*
         * If the line connecting the points crosses a fault,
         * do not use it in the calculations.
         */
            istat = grd_fault_ptr->con_find_fault_intersection
                (x0, y0, x[j], y[j], -1, 1, &xdum, &ydum);
            if (istat > 0) continue;

            dx = x[j] - x0;
            dy = y[j] - y0;
            dz = z[j] - z0;
            if (dx >= -ABSOLUTE_TINY  &&  dx <= ABSOLUTE_TINY) {
                dx = 0.0f;
            }
            if (dy >= -ABSOLUTE_TINY  &&  dy <= ABSOLUTE_TINY) {
                dy = 0.0f;
            }

            if (dx == 0.0  &&  dy == 0.0) {
                continue;
            }

            if (dx == 0.0f) {
                ang = HALFPI;
            }
            else {
                ang = (CSW_F)atan ((double)(dy/dx));
            }
            if (ang < 0.0f) ang += PI;
            ivar = (int) (ang / dang);
            if (ivar > ndir1) ivar = ndir1;
            if (ivar < 0) ivar = 0;

            dist = dx * dx + dy * dy;
            dist = (CSW_F)sqrt((double)dist);
            if (dz < 0.0f)
                dz = -dz;

            count = countang[ivar];
            var = varang[ivar];

            ibin = (int) (dist / dd);
            if (count[ibin] == 0) {
                var[ibin] = dz;
            }
            else {
                var[ibin] += dz;
            }
            count[ibin]++;

        }

    }

/*
    Process each directional variogram.
*/
    for (i=0; i<ndir; i++) {

        count = countang[i];
        var = varang[i];
        varmax[i] = 0.0f;

    /*
        Get averages for non empty bins.
    */
        for (j = 0; j<nbins; j++) {
            k = count[j];
            if (k > 1) {
                var[j] /= (CSW_F)k;
            }
        }

    /*
        Fill in empty bins by linear interpolation from non empty neighbors.
    */
        istat = FillInVariogram (var, nbins);
        if (istat == -1) {
            return -2;
        }

    /*
        Find the "knee" for the directional variogram.
    */
        FindKnee (var, nbins, 0.0f, dmax, varmax+i, knees+i);

    /*
        If there are no positive variances in any particular direction,
        the geometry of the points is almost certainly
        inappropriate for anisotropy calculations, so I
        bail out and return to the caller.
    */
        if (varmax[i] <= ABSOLUTE_TINY) {
            return 1;
        }

    }

/*
    Use the average maximum variance in all directions as the
    plateau value and repeat the knee calculations.
*/
    zmax = 0.0f;
    sum = 0.0f;
    for (i=0; i<ndir; i++) {
        zmax += varmax[i];
        sum++;
    }
    zmax /= sum;

    dmax2 = 0.0f;
    sum = 0.0f;
    for (i=0; i<ndir; i++) {
        if (knees[i] < 0.9 * dmax) {
            dmax2 += knees[i];
            sum++;
        }
    }

    if (sum == 0.0) {
        dmax2 = dmax / 2.0f;
    }
    else {
        dmax2 /= sum;
    }

    for (i=0; i<ndir; i++) {
        var = varang[i];
        FindKnee (var, nbins, 0.0f, dmax2, &zmax, knees+i);
/*
        sum = zmax / varmax[i];
        knees[i] *= sum;
*/
        if (knees[i] > dmax2) knees[i] = dmax2;
    }

/*
    Calculate the anisotropy parameters (strike and power) from the
    array of directional knee values.
*/
    AnalyseEllipse (knees, ndir, strike, power, aratio);

/*
    That's all folks.
*/
    bsuccess = true;

    return 1;

}  /*  end of function grd_global_anisotropy  */







/*
  ****************************************************************

                F i l l I n V a r i o g r a m

  ****************************************************************

    Linearly interpolate a variogram to fill in empty bins.  If the
  first bin is empty, make it zero.

*/

int CSWGrdStats::FillInVariogram (CSW_F *bins, int nbins)
{
    CSW_F            bwork[MAX_STATS_BINS], z1, z2, t1;
    int              i, j, j1, j2;

/*
    If less than 1/5 of the bins are occupied,
    do not attempt to fill in the rest.
*/
    j = 0;
    for (i=0; i<nbins; i++) {
        if (bins[i] < 1.e29f) {
            j++;
        }
    }

    if (nbins < 1)
        return -1;

    j1 = nbins / 5;
    if (j1 < 2) j1 = 2;
    if (j < j1) {
        return -1;
    }

/*
    A copy of the original data is needed to find the nearest
    defined neighbors.  The bins array will be filled in as
    the function progresses, so the originally undefined points
    would become defined points.  Therefore, a copy of the original
    data is needed to maintain the original nearest defined
    neighbors.
*/
    memcpy ((char *)bwork, (char *)bins, nbins*sizeof(CSW_F));

/*
    Make sure the first and last bins are not empty.  If the first bin is
    empty, replace it with zero.  If the last is empty, replace it with
    about 3/4 of the variogram maximum value.
*/
    if (bwork[0] > 1.e29f) {
        bwork[0] = 0.0f;
        bins[0] = 0.0f;
    }
    if (bwork[nbins-1] > 1.e29f) {
        z2 = -1.e30f;
        for (i=0; i<nbins; i++) {
            if (bins[i] < 1.e29f) {
                if (bins[i] > z2) z2 = bins[i];
            }
        }
        if (z2 > 0.0f  &&  nbins > 0) {
            z2 *= .75f;
            bwork[nbins-1] = z2;
            bins[nbins-1] = z2;
        }
    }

/*
    Fill in empty bins via linear interpolation
    from the nearest nonempty neighbors.
*/
    j1 = 0;
    j2 = 0;
    for (i=1; i<nbins-1; i++) {

        if (bwork[i] > 1.e29f) {
            z1 = 1.e30f;
            z2 = 1.e30f;
            for (j=i-1; j>=0; j--) {
                if (bwork[j] < 1.e29f) {
                    z1 = bwork[j];
                    j1 = j;
                    break;
                }
            }
            for (j=i+1; j<nbins; j++) {
                if (bwork[j] < 1.e29f) {
                    z2 = bwork[j];
                    j2 = j;
                    break;
                }
            }

        /*
            If only one nonempty neighbor could be found, use only that one value.
            This should not happen because of forcing values in the first and last
            bins, but since I am inherently suspicious, I'm keeping it in.
        */
            if (z2 > 1.e29f) {
                bins[i] = z1;
            }
            else if (z1 > 1.e29f) {
                bins[i] = z2;
            }
            else {
                t1 = (z2 - z1) / (j2 - j1);
                bins[i] = z1 + t1 * (i - j1);
            }
        }

    }

    return 1;

}  /*  end of private FillInVariograms function  */





/*
  *****************************************************************************

                            F i n d K n e e

  *****************************************************************************

  Search a variagram for the point where the maximum value starts to
  decrease.  This is the "feature width" at the point associated with
  the variogram.

*/

int CSWGrdStats::FindKnee (CSW_F *bins, int nbins, CSW_F d1, CSW_F d2,
                     CSW_F *zmaxptr, CSW_F *knee)
{
    int      i, j, k, i1, i2, nb1, jmin, jmin2;
    CSW_F    dd, zt, ztlast, zmax, sum, sum2,
             bwork[MAX_STATS_BINS];

    memset ((char *)bwork, 0, MAX_STATS_BINS * sizeof(CSW_F));

    zmax = *zmaxptr;
    nb1 = nbins - 1;

/*
    smooth the variogram with a simple moving average.
*/
    for (i=0; i<nbins; i++) {

        i1 = i - 1;
        if (i1 < 0) i1 = 0;
        i2 = i + 1;
        if (i2 > nb1) i2 = nb1;

        sum = 0.f;
        sum2 = 0.f;
        for (j=i1; j<=i2; j++) {
            sum += bins[j];
            sum2++;
        }
        bwork[i] = sum / sum2;

    }

/*
    If the zmax parameter was not specified,
    find an appropriate zmax here.
*/
    jmin = nbins / 10;
    if (jmin < 2) jmin = 2;
    jmin2 = jmin * 2;

    if (zmax <= 0.0f  ||  zmax > 1.e20f) {

        zt = -1.e30f;
        for (i=0; i<nbins; i++) {
            if (bins[i] > zt) zt = bins[i];
        }

        zmax = zt;
        ztlast = zt;

        for (;;) {

            j = 0;
            zt *= .95f;

            if (zt >= 0.99f * ztlast) {
                break;
            }
            ztlast = zt;

            for (i=0; i<nbins; i++) {
                if (bins[i] > zt) j++;
            }

            if (j > jmin2) {
                break;
            }

            if (zt < zmax / 4.0f) {
                zt = zmax * .95f;
                break;
            }

        }

        zmax = zt;

    }

/*
    zmax was specified, so first point over zmax is the knee
*/
    else {
        jmin = 0;
    }

/*
    Find the spot where several values
    in a row are greater than zmax.
*/
    j = 0;
    k = -1;
    for (i=0; i<nbins; i++) {

        if (bwork[i] > zmax  &&  j == 0) {
            k = i;
            j++;
            if (j > jmin) break;
            continue;
        }

        if (bwork[i] < zmax  &&  j > 0) {
            j = 0;
            continue;
        }

        if (bwork[i] > zmax  &&  j > 0) {
            j++;
            if (j > jmin) break;
            continue;
        }

    }
    if (k == 0) {
        k = -1;
    }

    if (k > 0) {
        j = 0;
        for (i=k+1; i<nbins; i++) {
            if (bwork[i] < zmax) j++;
        }
        j /= k;
        k += j;
    }

/*
    If no bin could be identified as the knee,
    set the knee value to d2 and return -1.
*/
    *zmaxptr = zmax;
    if (k == -1) {
        *knee = d2;
        return -1;
    }

/*
    Otherwise, the "knee" is the spot where the background maximum starts.
*/
    dd = (d2 - d1) / (CSW_F)nbins;
    *knee = k * dd + d1;

    return 1;

}  /*  end of function FindKnee  */






/*
  ****************************************************************

                  A n a l y s e E l l i p s e

  ****************************************************************

    Given the raw knee values, construct an anisotopy ellipse
  if possible.  If the raw data is too noisey, no anisotropy
  can be calculated.  In this case, strike is set to -1000 and
  power is set to 2.

*/

int CSWGrdStats::AnalyseEllipse (CSW_F *knees, int ndir, CSW_F *strike,
                           int *power, CSW_F *aratio)
{
    int          i, j, imax, i1, i2, i3, ns, pmax;
    CSW_F        work[NDIR],
                 x[MAX_ELLIPSE], y[MAX_ELLIPSE],
                 sx[MAX_ELLIPSE], sy[MAX_ELLIPSE],
                 dang, dang2, tmp;
    CSW_F        kmax, kmin, st2, xmax, ymax,
                 sum, sum2, pfact, local_tiny;

    *power = 0;
    *strike = -1000.0f;
    *aratio = 1.0f;

    memset ((char *)x, 0, MAX_ELLIPSE * sizeof(CSW_F));
    memset ((char *)y, 0, MAX_ELLIPSE * sizeof(CSW_F));
    dang = PI / (CSW_F)ndir;
    dang2 = dang / 2.0f;

/*
    Find the min and max raw knee values.
*/
    imax = 0;
    kmax = 0.0f;
    kmin = 1.e30f;
    sum = 0.0f;
    for (i=0; i<ndir; i++) {
        if (knees[i] > kmax) {
            kmax = knees[i];
            imax = i;
        }
        if (knees[i] < kmin) {
            kmin = knees[i];
        }
        sum += knees[i];
    }
    if (ndir > 0) {
        sum /= (CSW_F)ndir;
    }
    else {
        return 1;
    }

/*
 *  Bug 7957
 *    If any knee is very much smaller than the average, that
 *    means no knee could be found in the variogram.  I assign
 *    these a value the same as the max knee from the directional
 *    variograms.  If there are 3 or more of these, the logic
 *    below will reject the data set because it cannot determine
 *    a direction.
 */
    for (i=0; i<ndir; i++) {
        if (knees[i] < 0.0001 * sum) {
            knees[i] = kmax;
        }
    }

/*
 *  Bug 7957
 *    If any knees are less than 10 percent of the average,
 *    make them the average.  Then, redo min/max with new
 *    values.
 */
    for (i=0; i<ndir; i++) {
        if (knees[i] < 0.1*sum) knees[i] = sum;
    }
    kmax = 0.0f;
    kmin = 1.e30f;
    sum = 0.0f;
    for (i=0; i<ndir; i++) {
        if (knees[i] > kmax) {
            kmax = knees[i];
            imax = i;
        }
        if (knees[i] < kmin) {
            kmin = knees[i];
        }
    }

    local_tiny = (kmax - kmin) / 1000.0f;

    i1 = imax;

/*
    If more than 4 knees are greater than 80 percent
    of the maximum, no reliable anisotropy can be calculated.
*/
    kmax *= .8f;
    ns = 0;
    for (i=0; i<ndir; i++) {
        if (knees[i] > kmax) ns++;
    }

    pmax = 6;
    pfact = 1.0f;
    if (ns > 3) {
        pmax = 2;
    }
    else if (ns > 2) {
        pmax = 4;
    }

/*
    Smooth the knee values.
*/
    if (ndir < 2)
        ndir = 2;
    work[0] = (knees[ndir-1] + knees[0] + knees[1]) / 3.0f;
    work[ndir-1] = (knees[ndir-2] + knees[ndir-1] + knees[0]) / 3.0f;
    for (i=1; i<ndir-1; i++) {
        work[i] = (knees[i-1] + knees[i] + knees[i+1]) / 3.0f;
    }

/*
    Find the maximum smoothed knee value.
*/
    kmax = 0.0f;
    for (i=0; i<ndir; i++) {
        if (work[i] > kmax) {
            kmax = work[i];
            imax = i;
        }
    }

/*
    If more than 2 smoothed knees are greater than 80 percent
    of the maximum, set an adjustment for the eccentricity.
*/
    kmax *= .8f;
    ns = 0;
    for (i=0; i<ndir; i++) {
        if (work[i] > kmax) ns++;
    }
    kmax /= .8f;

    pmax = 6;
    pfact = 1.0f;
    if (ns > 3) {
        pfact = 3.0f / (CSW_F)ns;
        pfact *= pfact;
    }

/*
    Find average of knees not close to the maximum.
*/
    sum = 0.0f;
    sum2 = 0.0f;
    kmax *= .8f;
    for (i=0; i<ndir; i++) {
        if (work[i] > kmax) continue;
        sum += work[i];
        sum2++;
    }
    kmax /= .8f;

    if (sum2 < 1.0f) {
        *strike = -1000.f;
        *power = 2;
        *aratio = 1.0f;
        return 1;
    }
    sum /= sum2;

    st2 = 1.e30f;
    if (sum > 0.0f) st2 = kmax / sum;
    i2 = imax;

    if (st2 < 1.e29f) {
        st2 *= pfact;
    }

    *aratio = st2;

    if (st2 < 1.2f) {
        *strike = -1000.f;
        *power = 2;
        *aratio = 1.0f;
        return 1;
    }

/*
    If the raw and smoothed angles are not quite close,
    the knee values are too noisy for a reliable anisotropy
    estimate.
*/
    i3 = i2 - i1;
    if (i3 > 1  ||  i3 < -1) {
        *strike = -1000.f;
        *power = 2;
        *aratio = 1.0f;
        return 1;
    }

/*
    Convert to cartesian coordinates.
*/
    j = 0;
    for (i=0; i<ndir; i++) {
        tmp = i * dang + dang2;
        x[j] = (CSW_F)cos ((double)tmp);
        y[j] = (CSW_F)sin ((double)tmp);
        x[j] *= work[i];
        y[j] *= work[i];
        j++;
    }

    for (i=ndir-1; i<=0; i--) {
        x[j] = x[i];
        y[j] = -y[i];
        j++;
    }

    x[j] = x[0];
    y[j] = y[0];
    j++;

/*
    Fit a spline to the points.
*/
    GPFSpline    gpf_spline_obj;
    gpf_spline_obj.gpf_SplineFit (x, y, j, 1,
                   sx, sy, MAX_ELLIPSE, 0.0f,
                   &ns);

/*
    Find the point on the spline that is farthest from the origin
    and use it for the major axis of anisotropy.
*/
    kmax = -1.e30f;
    xmax = 1.e30f;
    ymax = 1.e30f;

    for (i=0; i<ns; i++) {
        if (sx[i] > -local_tiny  &&  sx[i] < local_tiny) {
            sx[i] = 0.0f;
        }
        if (sy[i] > -local_tiny  &&  sy[i] < local_tiny) {
            sy[i] = 0.0f;
        }
        dang = sx[i] * sx[i] + sy[i] * sy[i];
        if (dang > kmax) {
            xmax = sx[i];
            ymax = sy[i];
            kmax = dang;
        }
    }

    *power = 2;
    if (st2 > 3.0f) *power = 4;
    if (st2 > 5.0f) *power = 6;
    if (*power > pmax) *power = pmax;

    *strike = (CSW_F)atan2 ((double)ymax, (double)xmax);
    *strike *= DEGREES;

    return 1;

}  /*  end of static AnalyseEllipse function  */






/*
  ****************************************************************

            g r d _ l o c a l _ a n i s o t r o p y

  ****************************************************************

    Calculate a preferred strike direction in a local subset of points.
  The distance vs. variance pairs are done between the local points
  and the total data set.  Requirements for statistical integrety of
  the data are less stringent than for the global anisotropy calculations.

    If less than 8 directional variograms are calculable, the local
  anisotropy is considered undefined.  In this case, the strike is
  set to -1000.

*/

int CSWGrdStats::grd_local_anisotropy (int *local, int nlocal,
                          CSW_F *x, CSW_F *y, CSW_F *z, int n,
                          CSW_F *strike, int *power, CSW_F *aratio,
                          int thickflag)
{
    CSW_F            *vardata = NULL, *varang[NLOCALDIR], knees[NLOCALDIR],
                     varmax[NLOCALDIR];
    CSW_F            x0, y0, z0, dx, dy, dz, dd, dmax, zmax, sum,
                     x1, y1, x2, y2, ang, dist, dang, dxmax, dmaxsq;
    int              ndir, ndir1, i, j, k, ivar, ibin,
                     istat, nbins, ii;
    int              *countdata = NULL, *countang[NLOCALDIR];
    int              *count = NULL;
    CSW_F            *var = NULL, maxpower;

    bool     bsuccess = false;

    auto fscope = [&]()
    {
        csw_Free (vardata);
        csw_Free (countdata);
        if (bsuccess == false) {
            *strike = -1000.f;
            *power = 2;
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Initialize some stuff to suppress lint messages
*/
    memset ((char *)varmax, 0, NLOCALDIR * sizeof(CSW_F));
    memset ((char *)varang, 0, NLOCALDIR * sizeof(int));
    memset ((char *)knees, 0, NLOCALDIR * sizeof(int));
    memset ((char *)countang, 0, NLOCALDIR * sizeof(int));

/*
    obvious errors.
*/
    *strike = -1000.f;
    *power = 2;
    *aratio = 1.0f;
    if (n < 10) return 1;

    nbins = nlocal;
    if (nbins > MAX_STATS_BINS) nbins = MAX_STATS_BINS;
    if (n < 100) {
        if (nbins > n / 8) {
            nbins = n / 8;
        }
    }
    if (nbins < 10) nbins = 10;

    nbins = 10;
    ndir = NLOCALDIR;
    ndir1 = ndir - 1;

    maxpower = (CSW_F)(nlocal*n) / 10000.0f;
    if (maxpower > 6.0f) maxpower = 6.0f;

/*
    Allocate data for the directional variograms.
*/
MSL
    vardata = (CSW_F *)csw_Malloc (ndir * nbins * sizeof(CSW_F));
    if (!vardata) {
        return -1;
    }
    for (i=0; i<ndir; i++) {
        varang[i] = vardata + i * nbins;
    }

MSL
    countdata = (int *)csw_Calloc (ndir * nbins * sizeof(int));
    if (!countdata) {
        return -1;
    }
    for (i=0; i<ndir; i++) {
        countang[i] = countdata + i * nbins;
    }

/*
    Initialize variograms to a "null" value.
*/
    for (i=0; i<ndir*nbins; i++) {
        vardata[i] = 1.e30f;
    }

/*
    Find the x,y limits and use them to get the width
    of each bin in the variograms.
*/
    gpf_xandylimits (x, y, n, &x1, &y1, &x2, &y2);

    dx = (x2 - x1 + y2 - y1) / 6.0f;
    dxmax = dx;
    dmaxsq = 2.0f * dx * dx;
    dmax = (CSW_F)sqrt ((double)dmaxsq);

    dd = dmax / (CSW_F)nbins;

/*
    dang is the angular width of each wedge for
    accumulating directional variogram points.
*/
    dang = PI / (CSW_F)ndir;

/*
    Calculate the variance (delta z squared) vs distance pairs
    and put them into the appropriate directional variograms.
*/
    for (ii=0; ii<nlocal; ii++) {

        i = local[ii];

        x0 = x[i];
        y0 = y[i];
        z0 = z[i];

        if (thickflag == 1  &&  z0 <= 0.0f) continue;

        for (j=0; j<n; j++) {

            if (i == j) continue;
            if (thickflag == 1  &&  z[j] <= 0.0f) continue;

            dx = x[j] - x0;
            if (dx > dxmax) continue;
            dy = y[j] - y0;
            if (dy > dxmax) continue;
            dz = z[j] - z0;

            dist = dx * dx + dy * dy;
            if (dist > dmaxsq) continue;

            dist = (CSW_F)sqrt((double)dist);
            dz *= dz;

            if (dx == 0.0f) {
                ang = HALFPI;
            }
            else {
                ang = (CSW_F)atan ((double)(dy/dx));
            }
            if (ang < 0.0f) ang += PI;
            ivar = (int) (ang / dang);
            if (ivar > ndir1) ivar = ndir1;

            count = countang[ivar];
            var = varang[ivar];

            ibin = (int) (dist / dd);
            if (count[ibin] == 0) {
                var[ibin] = dz;
            }
            else {
                var[ibin] += dz;
            }
            count[ibin]++;

        }

    }

/*
    Process each directional variogram.
*/
    for (i=0; i<ndir; i++) {

        count = countang[i];
        var = varang[i];
        varmax[i] = 0.0f;

    /*
        Get averages for non empty bins.
    */
        for (j = 0; j<nbins; j++) {
            k = count[j];
            if (k > 1) {
                var[j] /= (CSW_F)k;
            }
        }

    /*
        Fill in empty bins by linear interpolation from non empty neighbors.
    */
        istat = FillInVariogram (var, nbins);
        if (istat == -1) {
            return -2;
        }

    /*
        Find the "knee" for the directional variogram.
    */
        FindKnee (var, nbins, 0.0f, dmax, varmax+i, knees+i);

    /*
        If there are no positive variances in this direction,
        the geometry of the points is almost certainly
        inappropriate for anisotropy calculations, so I
        bail out and return to the caller.
    */
        if (varmax[i] <= ABSOLUTE_TINY) {
            return 1;
        }

    }

/*
    Use the average maximum variance in all directions as the
    plateau value and repeat the knee calculations.
*/
    zmax = 0.0f;
    sum = 0.0f;
    for (i=0; i<ndir; i++) {
        zmax += varmax[i];
        sum++;
    }
    zmax /= sum;

    for (i=0; i<ndir; i++) {
        var = varang[i];
        FindKnee (var, nbins, 0.0f, dmax, &zmax, knees+i);
        sum = zmax / varmax[i];
        knees[i] *= sum;
    }

/*
    Calculate the anisotropy parameters (strike and power) from the
    array of directional knee values.
*/
    AnalyseEllipse (knees, ndir, strike, power, aratio);
    if (*strike < 0.0f  &&  *strike > -900.0f) *strike += 180.0f;
    if ((CSW_F)(*power) > maxpower) *power = (int) (maxpower + .5);

/*
    That's all folks.
*/
    bsuccess = true;

    return 1;

}  /*  end of function grd_local_anisotropy  */
