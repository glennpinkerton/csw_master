
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_util.c

    This file has utility functions for polygon clipping operations.
*/


/*
    system header files
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
    application header files
*/

#include "csw/utils/include/csw_.h"

#include "csw/utils/private_include/gpf_utils.h"

#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_traverse.h"

#include "csw/utils/private_include/csw_scope.h"



/*

    ***************************************************************

                     p l y _ a p n d h o l

    ***************************************************************

  function name:    ply_apndhol  (integer)

  call sequence:    ply_apndhol (xpoly, ypoly, ncomp, icomp, npt,
                                 xhole, yhole, holpts, holev)

  synopsis:         Add points from a hole or additional component to
                    the result polygon array for polygon clipping
                    operations.

  return values:    status code

                    0 = hole added successfully
                    1 = memory overflow would happen if the hole is
                        added.  The hole was not added in this case.

  usage:            Called only from polygon clipping operations.  This
                    function assumes that polygon parameters for available
                    memory have been set properly in ply_parms.  This
                    function should not be used independent of the rest of
                    the polygon clipping functions.

  algorithm:        The available memory is checked to insure that the
                    added points will not overflow.  The holflg is
                    appended to the xpoly array and the hole nesting level
                    is appended to the ypoly array.  These signal the
                    start of a new hole or component.  The points for the
                    hole are then added.  Finally, ncomp and icomp are
                    updated to reflect the current state of the result
                    polygon.

  description of calling parameters:

    xpoly     r/w  double*    array of result x coordinates
    ypoly     r/w  double*    array of result y coordinates
    ncomp     r/w  int     *  number of components in result
    icomp     r/w  int     *  array with number of points in each
                              component of the result
    npt       r/w  int     *  total number of points in result
    xhole     r    double*    x coordinates of hole
    yhole     r    double*    y coordinates of hole
    holpts    r    int        number of points in hole
    holev     r    int        nesting level of hole

*/

int CSWPolyUtils::ply_apndhol (
                 double *xpoly, double *ypoly, int *ncomp, int *icomp, int *npt,
                 double *xhole, double *yhole, int holpts, int holev)
{
    int          i, npmax, nhmax;

/*  suppress warning  */

    holev = holev;

/*  retrieve hole flag and maximum array sizes  */

    npmax = (int)MaxPout;
    nhmax = (int)MaxHout;

/*  check if this hole will overflow memory  */

    if (*ncomp + 1  >= nhmax) return 1;
    if (*npt + holpts + 1  >=  npmax) return 1;

/*  add points for hole to result  */

    for (i=0; i<holpts; i++) {
        xpoly[*npt] = xhole[i];
        ypoly[*npt] = yhole[i];
        (*npt)++;
    }

/*  update ncomp and icomp  */

    icomp[*ncomp] = holpts;
    (*ncomp)++;

    return 0;

}  /*  end of ply_apndhol function  */






/*

    ***************************************************************

                         p l y _ c o p y

    ***************************************************************

  function name:    ply_copy   (integer)

  call sequence:    ply_copy(xpoly, ypoly, npts, xpout, ypout)

  synopsis:         copy coordinates from one polygon to another

  return values:    always returns 0

  usage:            This function is called from polygon clipping
                    functions or can be used as a general utility
                    to copy any string of x,y coordinates.

  algorithm:        Each element of the input arrays is copied to the
                    output arrays, irregardless of hole flags or any
                    other flags in the data.  No check is made for
                    the size of the output arrays.

  description of calling parameters:

    xpoly    r    double*    array of input x coordinates
    ypoly    r    double*    array of input y coordinates
    npts     r    int        number of points in arrays
    xpout    w    double*    array of output x coordinates
    ypout    w    double*    array of output y coordinates

*/

int CSWPolyUtils::ply_copy (
    double *xpoly, double *ypoly, int npts, double *xpout, double *ypout)
{
    int         i;

    for (i=0; i< npts; i++) {
        xpout[i] = xpoly[i];
        ypout[i] = ypoly[i];
    }

    return 0;

}   /*  end of ply_copy function  */






/*

    ***************************************************************

                          p l y _ g l o b a l i m

    ***************************************************************

  function name:    ply_globalim

  call sequence:    ply_globalim (xmin, ymin, xmax, ymax,
                                  gxmin, gymin, gxmax, gymax)

  synopsis:         Calculate global xy limits for several polygon
                    components.

  return values:    status code

                    0 = successful execution
                    1 = no valid points input
                        (either npts is less than 1 or all the
                         xmin...ymax values are absurdly large)

  usage:            Called by the polygon clipping functions and can
                    be used generically to get minmax values of a set
                    of coordinates.

  algorithm:        Initialize global limits to absurd values and then
                    use if logic to replace with values from the
                    component arrays.

  description of calling parameters:

    xmin     r    double*   pointer to array of minimum x coordinates
                            for polygon components
    ymin     r    double*   pointer to array of minimum y coordinates
    xmax     r    double*   pointer to array of maximum x coordinates
    ymax     r    double*   pointer to array of maximum y coordinates
    npts     r    int       number of points in xmin...ymax
    gxmin    w    double*   pointer to minimum x for all components
    gymin    w    double*   pointer to minimum y for all components
    gxmax    w    double*   pointer to maximum x for all components
    gymax    w    double*   pointer to maximum y for all components

*/

int CSWPolyUtils::ply_globalim (
    double *xmin, double *ymin, double *xmax, double *ymax, int npts,
    double *gxmin, double *gymin, double *gxmax, double *gymax)
{
    int         i;


/*  check for positive npts  */

    if(npts < 1) return 1;

/*  initialize global limits to absurd values  */

    *gxmin = 1.e30f;
    *gymin = 1.e30f;
    *gxmax = 1.e30f;
    *gymax = 1.e30f;

/*  loop through component limits to find global minmax  */

    for (i=0; i<npts; i++) {
        if(xmin[i] < *gxmin)  *gxmin = xmin[i];
        if(ymin[i] < *gymin)  *gymin = ymin[i];
        if(xmax[i] > *gxmax)  *gxmax = xmax[i];
        if(ymax[i] > *gymax)  *gymax = ymax[i];
    }

    return 0;

}  /*  end of ply_globalim function  */




/*
******************************************************************

                     p l y _ g r a z e

******************************************************************

    return 1 if points are within grazing distance
    return 0 if not
*/

int CSWPolyUtils::ply_graze(
    double x, double y, double xc, double yc)
{
    double    dx, dy;

    dx = x-xc;
    if (dx < MinusEpsilon  ||  dx > Epsilon) {
        return 0;
    }

    dy = y-yc;
    if (dy < MinusEpsilon  ||  dy > Epsilon) {
        return 0;
    }

    return 1;

}  /*  end of function ply_graze  */



/*
******************************************************************

                     p l y _ g r a z e 1

******************************************************************

    return 1 if points are within grazing distance
    return 0 if not

    all coordinates are passed as CSW_Fs

*/


int CSWPolyUtils::ply_graze1 (
    CSW_F x, CSW_F y, CSW_F xc, CSW_F yc)
{
    double    dx, dy;

    dx = x-xc;
    if (dx < MinusEpsilon  ||  dx > Epsilon) {
        return 0;
    }

    dy = y-yc;
    if (dy < MinusEpsilon  ||  dy > Epsilon) {
        return 0;
    }

    return 0;

}  /*  end of function ply_graze1  */



/*
******************************************************************

                  p l y _ s e t g r a z e

******************************************************************

    set a double precision grazing value

*/

int CSWPolyUtils::ply_setgraze(double x)
{
    double     xmin;

    xmin = 1.e-8;
    if(x < xmin) {
        Epsilon = xmin;
    }
    else {
        Epsilon = x;
    }
    MinusEpsilon = -Epsilon;

    return 0;

}  /*  end of function ply_setgraze  */



/*
******************************************************************

                  p l y _ g e t g r a z e

******************************************************************

    get the current grazing value

*/

int CSWPolyUtils::ply_getgraze(double *x)
{
    *x = Epsilon;

    return 0;

}  /*  end of function ply_getgraze  */







/*

    ***************************************************************

                      p l y _ g r i d _ s e t

    ***************************************************************

  function name:    ply_grid_set  (integer)

  call sequence:    ply_grid_set (xpmin, ypmin, xpmax, ypmax, numpoly,
                                  xgmin, ygmin, xgmax, ygmax,
                                  xspace, yspace)

  synopsis:         Set up grid spacings for polygon operations.


  return values:    The return value is a status code.

                      0 = Successful execution
                      1 = numpoly passed is less than 1
                      2 = could not allocate memory for work space
                          (numpoly*8 bytes are needed for work space)
                      3 = could not resolve the spacings within the
                          amount of memory available for the grid.
                          This probably indicates that the polygon
                          limits passed to the routine are very small
                          while the global limits are very large.  If
                          one polygon has absurd coordinates while the
                          rest of the set has reasonable limits this can
                          happen.
                      4 = Global min max of the polygon set defines
                          zero or negative area (minimum and maximum
                          are equal or maximum is less than minimum)

  usage:            This routine is used whenever polygon operations
                    (points inside polygons, polygon intersection etc.)
                    are to be done.  The function calculates reasonable
                    values for the spacings in x and y of the raster
                    grid which will index the polygon structure.  The
                    function is intimately tied to the polygon operation
                    procedures and should not be used in any other context.
                    Function poly_limits should be used prior to this call
                    to set up the polygon limits values needed for input.

  algorithm:        The average and smallest areas for rectangles around
                    polygons are found.  The average absolute deviation
                    from the mean value is also found.  The size of each
                    grid cell is based on the smallest polygon in the
                    set or upon a size 2 absolute deviations smaller than
                    average, whichever size is larger.  The sizes are then
                    adjusted so that there are a maximum of maxmem nodes
                    in the grid. (maxmem is found from the system_parms
                    function).

  description of calling parameters:

    xpmin     r    double *    Pointer to array of minimum x coordinates
                               for each individual polygon.
    ypmin     r    double *    Pointer to array of minimum y coordinates.
    xpmax     r    double *    Pointer to array of maximum x coordinates.
    ypmax     r    double *    Pointer to array of maximum y coordinates.
    numpoly   r    int         Number of polygons in the set. (Number of
                               values in xpmin ... ypmax)
    xgmin     r    double      Minimum x coordinate of the entire set of
                               polygons.
    ygmin     r    double      Minimum y coordinate of entire set.
    xgmax     r    double      Maximum x coordinate of entire set.
    ygmax     r    double      Maximum y coordinate of entire set.
    xspace    w    double *    Pointer to returned x spacing of the grid.
    yspace    w    double *    Pointer to returned y spacing of the grid.

*/

int CSWPolyUtils::ply_grid_set (
    double *xpmin, double *ypmin, double *xpmax, double *ypmax,
    int numpoly,
    double xgmin, double ygmin, double xgmax, double ygmax,
    double *xspace, double *yspace)
{
    double        amin, sum, sum2, dirt,
                  *work = NULL, dx, dy, tinyarea;
    int           maxmem, i, n1, n2;

    auto fscope = [&]()
    {
        csw_Free (work);
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  check for obvious errors  */

    if (numpoly < 1) return 1;
    if ( (xgmax - xgmin) * (ygmax - ygmin)  <=  0.0f )  return 4;

    tinyarea = (xgmax - xgmin) * (ygmax - ygmin) / 10000.0;

/*  get memory for work array, return with status = 2 if unsuccessful  */

    work = (double *) csw_Malloc(numpoly*sizeof(double));
    if (! work)  {
        return 2;
    }

/*  find the smallest and average polygon areas  */

    amin = 1.e30f;
    sum=0.0f;

    for (i=0; i < numpoly; i++) {
        dx = xpmax[i] - xpmin[i];
        dy = ypmax[i] - ypmin[i];
        if (dx < .01 * dy) {
            dx = .01 * dy;
        }
        if (dy < .01 * dx) {
            dy = .01 * dx;
        }
        dirt = dx * dy;
        if (dirt < tinyarea) dirt = tinyarea;
        work[i] = dirt;
        if (dirt < amin) amin = dirt;
        sum += dirt;
    }
    sum = sum/numpoly;

/*  calculate average absolute deviation  */

    sum2 = 0.0f;

    for (i=0; i < numpoly; i++) {
        sum2 += fabs(work[i] - sum);
    }
    sum2 = sum2/numpoly;
    dirt = sum - 2.0f * sum2;
    if(dirt > amin) amin = dirt;

/*  csw_Free work space memory  */



/*  get maximum number of grid cells permitted  */

    maxmem = MAX_GRID_SIZE * 4;

/*  calculate spacings and adjust so that no more than maxmem bytes
    are needed for storage of each raster grid  */

    *xspace = sqrt(amin);

    if(sum2 < 0.1f * sum) {
        *xspace = .1f * *xspace;
    }
    else {
        *xspace = sum2 / sum * *xspace;
    }

    dirt = (xgmax - xgmin) / 40000.0f;
    if (dirt < (ygmax - ygmin) / 40000.0f) {
         dirt = (ygmax - ygmin) / 40000.0f;
    }
    if ( *xspace < dirt )  *xspace = dirt;

    n1 = (int)((xgmax - xgmin) / *xspace);
    n2 = (int)((ygmax - ygmin) / *xspace);

    i = 0;

    while ( n1*n2 > maxmem ) {
        n1 = n1 * 3 / 4;
        n2 = n2 * 3 / 4;
        if (n1 < 20) n1 = 20;
        if (n2 < 20) n2 = 20;
        if(i++ > 10) return 3;    /*  to keep from infinite loop  */
    }

    if (n1 > 0  &&  n2 > 0) {
        *xspace = (xgmax - xgmin) / n1;
        *yspace = (ygmax - ygmin) / n2;
    }
    else if (n1 > 0) {
        *xspace = (xgmax - xgmin) / n1;
        *yspace = (ygmax - ygmin) / n1;
    }
    else if (n2 > 0) {
        *xspace = (xgmax - xgmin) / n2;
        *yspace = (ygmax - ygmin) / n2;
    }
    else {
        return 4;
    }

    return 0;

}   /*  end of ply_grid_set function  */






/*
  **************************************************************

                       p l y _ h o l d e l

  **************************************************************

  function name:    ply_holdel  (integer)

  call sequence:    ply_holdel(xpoly, ypoly, ncomp, icomp, nhole)

  synopsis:         Delete a hole from a polygon

  return values:    always returns 0

  usage:            Used by polygon clipping to delete a hole that has no
                    determinable orientation.  Can also be used as a
                    general utility to delete a hole.

  algorithm:        The x and y coordinates for the polygon are pushed up
                    to overwrite the hole.  The icomp array is also pushed
                    up and ncomp is decremented by 1.

  description of calling parameters:

    xpoly    r/w  double*     array of polygon x coordinates
    ypoly    r/w  double*     array of polygon y coordinates
    ncomp    r/w  int     *   number of components in polygon
    icomp    r/w  int     *   array with number of points in each
                              polygon component
    nhole    r    int         hole number to delete
                              (first hole is 0, last hole is ncomp-1)

*/

int CSWPolyUtils::ply_holdel (
    double *xpoly, double *ypoly,
    int *ncomp, int *icomp, int nhole)
{
    int           offset, ioff, i, j, npts;

/*  do nothing if nhole is out of range  */

    if(nhole < 0  ||  nhole >= *ncomp) {
        return 0;
    }

/*  push up data if needed  */

    if(nhole < *ncomp-1) {

/*      offset of hole to delete  */

        offset = 0;
        for (i=0; i<nhole; i++) {
            offset += icomp[i];
        }
        ioff = icomp[nhole];

/*      find total number of points in input polygon  */

        npts = offset;
        for (i=nhole; i<*ncomp; i++) {
            npts += icomp[i];
        }

/*      push up x and y coordinates of remaining holes  */

        for (i=offset+ioff; i<npts; i++)  {
            j = i-ioff;
            xpoly[j] = xpoly[i];
            ypoly[j] = ypoly[i];
        }

/*      push up icomp array  */

        for (i=nhole+1; i<*ncomp; i++) {
            j = i-1;
            icomp[j] = icomp[i];
        }
    }

/*  decrement the total number of components  */

    (*ncomp)--;

    return 0;

}   /*  end of ply_holdel function  */






/*

    ***************************************************************

                        p l y _ h o l n e s t

    ***************************************************************

  function name:    ply_holnest   (integer)

  call sequence:    ply_holnest (xpoly, ypoly, ncomp, icomp,
                                 xpout, ypout, npout, ncout, icout)

  synopsis:         Separate multiple nesting levels of polygon holes.

  return values:    status code

                    -1 = could not allocate memory for workspace
                     0 = successful completion
                     1 = could not apend hole to output polygon set
                         this is probably because not enough memory
                         was allocated for the output polygon
                     2 = No unnesting was needed.  The input polygon
                         should be used as is.  No data are written
                         to the output polygon in this case.
                     3 = No component could be found that is not inside
                         another component.  This is either because two
                         components of the polygon are identical or
                         because of a program bug.

  usage:            This function is an integral part of the polygon
                    boolean operations functions.  It is not designed
                    or documented to be used outside of this context.
                    The polygon operations use this function to insure
                    that the intersection, union etc functions are only
                    dealing with holes at a single nesting level.

  algorithm:        First, a component not inside any other component is
                    found and used as the root of a polygon.  Next, any
                    components inside the root, but not inside any other
                    components, are output as holes of the root polygon.
                    The root and its holes are removed from the processing
                    list, and the process is repeated until all components
                    have been accounted for.  This will not work for self
                    intersecting polygons.

  description of calling parameters:

    xpoly      r    double*    array of input x coordinates
    ypoly      r    double*    array of input y coordinates
    ncomp      r    int        number of components in input polygon
    icomp      r    int     *  array with number of points in each
                               input polygon component
    xpout      w    double*    array of output polygon x coordinates
    ypout      w    double*    array of output polygon y coordinates
    npout      w    int     *  number of polygons in output
    ncout      w    int     *  array with number of components for
                               each output polygon
    icout      w    int     *  array with number of points in each
                               component of each output polygon
*/

int CSWPolyUtils::ply_holnest (
    double *xpoly, double *ypoly, int ncomp, int *icomp,
    double *xpout, double *ypout, int *npout, int *ncout,
    int *icout)
{
    HList        *holist = NULL, *holorg = NULL,
                 *holsav = NULL, *holtmp = NULL;
    PList        *plylist = NULL;
    int          i, j, offset, istat, ioff, joff, koff,
                 numout, nstack, rootoff, rootnps, iroot, nstacksav;
    int          nctemp, nout, npts, inps, jnps;
    int          nptemp, nclast;


    auto fscope = [&]()
    {
        csw_Free (holist);
        csw_Free (plylist);
    };
    CSWScopeGuard  func_scope_guard (fscope);


/*  if only one component, return a flag of 2  */

    if (ncomp == 1) return 2;


/*  set some stuff to make compiler warnings go away  */

    iroot = 0;
    istat = 0;
    rootoff = 0;
    rootnps = 0;
    npts = 0;

/*  allocate memory for workspace  */

    holist = (HList *) csw_Malloc(ncomp * sizeof(HList));
    if(!holist) {
        return -1;
    }
    holorg = holist;

    plylist = (PList *) csw_Malloc(ncomp * sizeof(PList));
    if(!plylist) {
        return -1;
    }

/*  build local structure array in holist which contains offset, size
    and flag for each polygon component  */

    offset = 0;
    for (i=0; i<ncomp; i++) {

        holist->off = offset;
        holist->flg = 0;
        holist->nps = icomp[i];
        offset += icomp[i];
        holist++;

    }

    nstack = ncomp;
    holist = holorg;


/*  unnest levels of holes by finding components of the polygon which are
    not inside any other components, and then finding all of the holes
    inside these components which are also not inside any other component
    This produces a set of polygons with only one level of hole nesting
    per polygon  */


    nctemp = 0;
    numout = 0;
    nout = 0;
    nptemp = 0;
    nclast = 0;

    while (nstack > 0) {

        holist = holorg;

        for (i=0; i<nstack; i++) {
            holist->flg = 0;
        }

/*      find a component not inside any other component  */

        for (i=0; i<nstack; i++) {

            ioff = holist->off;
            npts = holist->nps;
            holsav = holist;
            holist = holorg;

            for (j=0; j<nstack; j++) {

                if(j != i) {
                    joff = holist->off;
                    inps = holist->nps;
                    istat = 0;
                    koff = 0;
                    while (istat == 0  &&  koff < npts) {
                        istat = ply_point(xpoly+joff, ypoly+joff, inps,
                                          *(xpoly+ioff+koff),
                                          *(ypoly+ioff+koff));
                        koff++;
                    }
                    if(istat >= 0) {
                        break;
                    }

                    holist++;
                }
                else {
                    holist++;
                }
            }

            if(istat < 0  ||  nstack == 1) {
                rootoff = ioff;
                rootnps = npts;
                iroot = i;
                holsav->flg = -1;
                istat = -1;
                break;
            }

            holist = holsav;
            holist++;

        }

/*      istat should not be gt or equal to 0 unless all components
        are identical or there is a bug in the code  */

        if(istat >= 0) {
            return 4;
        }

/*      add main component (root polygon) to the output  */

        if (nptemp > 0) {
            ncout[nptemp-1] = nctemp - nclast;
        }
        nclast = nctemp;
        icout[nctemp] = npts;
        nctemp++;
        nptemp++;
        plylist->ply = numout;
        plylist->hol = 0;
        plylist++;
        ioff = rootoff;

        for (i=0; i<npts; i++) {
            *(xpout+nout) = *(xpoly+ioff);
            *(ypout+nout) = *(ypoly+ioff);
            ioff++;
            nout++;
            if (nout >= MaxPts) {
                holist = holorg;
            }
        }

/*      find holes inside the root and outside everything else  */

        holist = holorg;

        for (i=0; i<nstack; i++) {

            if(i == iroot) {
                holist++;
            }

            else {

                ioff = holist->off;
                npts = holist->nps;
                istat = 0;
                koff = 0;

                while (istat >= 0  &&  koff < npts) {

                    istat = ply_point(xpoly+rootoff, ypoly+rootoff,
                                      rootnps,
                                      *(xpoly+ioff+koff),
                                      *(ypoly+ioff+koff));
                    koff++;
                }

/*              if the hole is outside the root polygon  */

                if(istat < 0) {
                    holist++;
                }

/*              the hole is inside the root component of the polygon,
                make sure this hole is not inside another component     */

                else {

                    holsav = holist;
                    holist = holorg;

                    for (j=0; j<nstack; j++) {

                        if(j == iroot  ||  j == i) {
                            holist++;
                        }

                        else {

                            joff = holist->off;
                            jnps = holist->nps;
                            istat = -1;
                            koff = 0;

                            while (istat < 0  &&  koff < npts) {
                                istat = ply_point
                                        (xpoly+joff, ypoly+joff, jnps,
                                         *(xpoly+ioff+koff),
                                         *(ypoly+ioff+koff));
                                koff++;
                            }
                            if(istat >= 0) {
                                break;
                            }
                            holist++;
                        }
                    }

/*                  the hole is a level 1 hole of the root, output and
                    flag the hole  */

                    if(istat < 0  ||  nstack <= 2) {
                        plylist->ply = numout;
                        plylist->hol = 1;
                        plylist++;
                        istat = ply_apndhol(xpout, ypout, &nctemp,
                                            icout, &nout,
                                            xpoly+ioff, ypoly+ioff,
                                            npts, 1);

                        if(istat != 0) {
                            return 1;
                        }
                        holist = holsav;
                        holist->flg = -1;
                    }
                    else {
                        holist = holsav;
                    }

/*                  increment the holist structure to check the next component
                    for inside/outside of the root component    */

                    holist++;

                }
            }
        }

/*      compress the stack in preparation for the next root component  */

        holist = holorg;
        nstacksav = nstack;
        i=0;

        while(i<nstack) {

            if(holist->flg >= 0) {
                holist++;
                i++;
            }
            else {
                holsav = holist;
                for(j=i+1; j<nstack; j++) {
                    holtmp = holist + 1;
                    holist->off = holtmp->off;
                    holist->flg = holtmp->flg;
                    holist->nps = holtmp->nps;
                    holist++;
                }
                nstack--;
                holist = holsav;
            }
        }

        if(nstack >= nstacksav) {
            return 3;
        }

/*  unnest the next root component  */

        numout++;

    }


/*  last component number for last polygon  */

    ncout[nptemp-1] = nctemp - nclast;
    *npout = nptemp;

/*  csw_Free workspace memory and return as successful completion  */


    return 0;

}    /*  end of ply_holnest function  */







/*

    ***************************************************************

                       p l y _ i n t o p

    ***************************************************************

  function name:    ply_intop    (integer)

  call sequence:    ply_intop(flag, x, y, nsize)

  synopsis:         initialize, csw_Free, add, compare polygon side
                    intersections

  return values:    dependent on flag

                    for any flag, if -1 is returned, the function
                    cannot get enough memory to work

                    for flag = 'c', 0 means the point does not exist
                                    1 means the point already exists

                    for flag = 'a' or '+', 2 means that not enough memory
                                           was allocated in the 'i' call

                    for all other flags, a 0 is returned

  usage:            This function is intimately tied to the polygon
                    boolean operations.  Error checking is minimal.
                    It is assumed that the calling function knows
                    what it is doing and passes valid data to this
                    function.  The function is used to keep track of
                    intersection points calculated in polygon boolean
                    operations.  A point can be added or compared to
                    points previously calculated.  In addition, the
                    data arrays can be initialized or csw_Freed up, and
                    the fudge factor for "identical" points can be
                    set up.

  algorithm:        If logic keys on the flag.  For the c flag, the
                    current contents of the intersection arrays is
                    searched sequentially for a previous point that
                    is almost identical to x,y passed.  If the point
                    is coincident, 1 is returned.

  description of calling parameters:

    flag    r    char     single character flag telling the function
                          what to do
                          'i' = initialize 2 arrays for nsize number of
                                polygon sides.  The memory is allocated
                                here but the calling function or the class
                                destructor must csw_Free the xint pointer when
                                appropriate.
                          'a' = append x,y to the end of the arrays
                          'e' = use the value passed in x as the
                                coincident point fudge factor.  If 2
                                points are this close in x and y they
                                are considered coincident
                          'c' = compare x and y with previous points
                                return 0 if no previous point is coin-
                                cident.  return 1 if a previous point
                                is coincident.
                          '+' = same as 'c', but if the point is not
                                coincident it is added to the data arrays

*/

int CSWPolyUtils::ply_intop (
    char flag, double x, double y, int nsize)
{
    int         i;
    double      dx, dy;

/*  convert flag to lower case  */

    if(isupper((int)flag)) {
        flag = (char)tolower(flag);
    }

/*  check if x,y was previously occupied  */

    if(flag == 'c'  ||  flag == '+') {

        if (xint == NULL  ||  yint == NULL)
            return 2;

        for (i=0; i<numint; i++) {
            dx = xint[i] - x;
            dy = yint[i] - y;
            if(dx < 0.0f) dx = -dx;
            if(dy < 0.0f) dy = -dy;
            if(dx <= Epsilon  &&  dy <= Epsilon) {
                return 1;
            }
        }

        if(flag == '+') {
            if(numint >= maxint) return 2;
            xint[numint] = x;
            yint[numint] = y;
            numint++;
        }

        return 0;

    }

/*  allocate memory for intersection arrays  */

    else if(flag == 'i') {

        i = (long)MultMem;;
        if(i<1) i=1;
        nsize = nsize * i;
        if (nsize < 100) {
            nsize = 100;
        }

        xint = (double *)csw_Malloc(nsize * sizeof(double));
        if(!xint) {
            return -1;
        }

        yint = (double *)csw_Malloc(nsize * sizeof(double));
        if(!yint) {
            csw_Free(xint);
            return -1;
        }

        maxint = nsize;
        numint = 0;
        return 0;

    }

/*  csw_Free memory previously allocated  */

    else if(flag == 'f') {

        return 0;

    }

/*  add intersection coordinate pair  */

    else if(flag == 'a') {

        if(numint > maxint) {
            return 2;
        }

        xint[numint] = x;
        yint[numint] = y;
        numint++;
        return 0;

    }

/*  set Epsilon value for intersection check  */

    else if(flag == 'e') {

        if (x < 1.e-8) x = 1.e-8;
        Epsilon = x;
        MinusEpsilon = -Epsilon;
        return 0;

    }

    return 0;

}   /*  end of ply_intop function  */






/*

    ***************************************************************

                       p l y _ l i m i t s

    ***************************************************************

  function name:    ply_limits  (integer)

  call sequence:    ply_limits (xpoly, ypoly, packpoly, numpoly,
                                xpmin, ypmin, xpmax, ypmax,
                                xgmin, ygmin, xgmax, ygmax)

  synopsis:         Find the x,y limits for a set of polygons.

  return values:    This function returns 0 if executed successfully.
                    Any return value other than 0 indicates that at
                    least one polygon had 0 vertices in it.  If the
                    return value is not 0, DO NOT USE THE LIMITS
                    CALCULATED.

  usage:            This is used in conjunction with the many in many
                    points within polygons code and also with the polygon
                    clipping code.  The calling routine must have set
                    up the required pointers and memory prior to this
                    call.

  algorithm:        The polygons are unpacked and if logic compares
                    the coordinates against limits values that are
                    initialized at absurd values.

  description of calling parameters:

    xpoly      r   double *  Pointer to an array of x coordinates
                             for the set of polygons.
    ypoly      r   double *  Pointer to an array of y coordinates
                             for the set of polygons.
    packpoly   r   int      *  Pointer to an array with the number
                               of points in each polygon.
    numpoly    r   int       Number of individual polygons in the set.
                             I.E.  if numpoly = 3, and the first three
                             elements of packpoly are 8,13,6 then the
                             coordinates of the first polygon are in
                             addresses 0-7 of xpoly and ypoly, the 2nd
                             polygon is in 8-20, and the 3rd polygon is
                             in 21-26.
    xpmin      w   double    pointer to array with minimum x coordinates
                             for each separate polygon.
    ypmin      w   double    pointer to array of minimum y for each
                             separate polygon.
    xpmax      w   double    pointer to array of maximum x for each
                             separate polygon.
    ypmax      w   double    pointer to array of maximum y for each
                             separate polygon.
    xgmin      w   double    pointer to minimum x of the polygon set
    ygmin      w   double    pointer to minimum y of the polygon set
    xgmax      w   double    pointer to maximum x of the polygon set
    ygmax      w   double    pointer to maximum y of the polygon set

*/

int CSWPolyUtils::ply_limits (
    double *xpoly, double *ypoly, int *packpoly, int numpoly,
    double *xpmin, double *ypmin, double *xpmax, double *ypmax,
    double *xgmin, double *ygmin, double *xgmax, double *ygmax)
{
    int        i=0, j=0, jtot=0;
    double     holeflag, tiny;

/*  obvious error condition, numpoly lt 1  */

    if (numpoly < 1) return 1;

/*  get holeflag from ply_parms  */

    holeflag = HoleFlag;

/*  initialize global limits to absurd values  */

    *xgmin = 1.e30f;
    *ygmin = 1.e30f;
    *xgmax = -1.e30f;
    *ygmax = -1.e30f;

/*  start loop through all polygons  */

    for ( i=0; i < numpoly; i++) {

        jtot = packpoly[i];
        if(jtot < 1) return 1;      /* no points in this polygon */

/*      absurd limits for this polygon to start     */

        xpmin[i] = 1.e30f;
        ypmin[i] = 1.e30f;
        xpmax[i] = -1.e30f;
        ypmax[i] = -1.e30f;

/*      get limits for separate polygon              */

        for ( j=0;  j < jtot;  j++) {

            if (*xpoly < holeflag) {
                if (*xpoly < xpmin[i]) xpmin[i] = *xpoly;
                if (*ypoly < ypmin[i]) ypmin[i] = *ypoly;
                if (*xpoly > xpmax[i]) xpmax[i] = *xpoly;
                if (*ypoly > ypmax[i]) ypmax[i] = *ypoly;
            }
            xpoly++;
            ypoly++;

        }

        tiny = (xpmax[i] - xpmin[i] + ypmax[i] - ypmin[i]) / 200000.0;
        if (tiny < 1.e-10) tiny = 1.e-10;
        xpmin[i] -= tiny;
        xpmax[i] += tiny;
        ypmin[i] -= tiny;
        ypmax[i] += tiny;

/*      end of individual polygon loop, update global limits  */

        if (xpmin[i] < *xgmin) *xgmin = xpmin[i];
        if (ypmin[i] < *ygmin) *ygmin = ypmin[i];
        if (xpmax[i] > *xgmax) *xgmax = xpmax[i];
        if (ypmax[i] > *ygmax) *ygmax = ypmax[i];

    }

/*  end of loop through all polygons  */

    return 0;

}  /*  end of ply_limits function  */



/*

    ***************************************************************

                       p l y _ g l i m i t s

    ***************************************************************

  function name:    ply_glimits  (integer)

  call sequence:    ply_glimits (xpoly, ypoly, packpoly, numpoly,
                                 xgmin, ygmin, xgmax, ygmax)

  synopsis:         Find the x,y limits for a set of polygons.

  return values:    This function returns 0 if executed successfully.
                    Any return value other than 0 indicates that at
                    least one polygon had 0 vertices in it.  If the
                    return value is not 0, DO NOT USE THE LIMITS
                    CALCULATED.

  usage:            This is used in conjunction with the many in many
                    points within polygons code and also with the polygon
                    clipping code.  The calling routine must have set
                    up the required pointers and memory prior to this
                    call.

  algorithm:        The polygons are unpacked and if logic compares
                    the coordinates against limits values that are
                    initialized at absurd values.

  description of calling parameters:

    xpoly      r   double *  Pointer to an array of x coordinates
                             for the set of polygons.
    ypoly      r   double *  Pointer to an array of y coordinates
                             for the set of polygons.
    packpoly   r   int      *  Pointer to an array with the number
                               of points in each polygon.
    numpoly    r   int       Number of individual polygons in the set.
                             I.E.  if numpoly = 3, and the first three
                             elements of packpoly are 8,13,6 then the
                             coordinates of the first polygon are in
                             addresses 0-7 of xpoly and ypoly, the 2nd
                             polygon is in 8-20, and the 3rd polygon is
                             in 21-26.
    xgmin      w   double    pointer to minimum x of the polygon set
    ygmin      w   double    pointer to minimum y of the polygon set
    xgmax      w   double    pointer to maximum x of the polygon set
    ygmax      w   double    pointer to maximum y of the polygon set

*/

int CSWPolyUtils::ply_glimits (
    double *xpoly, double *ypoly, int *packpoly, int numpoly,
    double *xgmin, double *ygmin, double *xgmax, double *ygmax)
{
    int        i=0, j=0, jtot=0;
    double     holeflag;

/*  obvious error condition, numpoly lt 1  */

    if (numpoly < 1) return 1;

/*  get holeflag from ply_parms  */

    holeflag = HoleFlag;

/*  initialize global limits to absurd values  */

    *xgmin = 1.e30f;
    *ygmin = 1.e30f;
    *xgmax = -1.e30f;
    *ygmax = -1.e30f;

/*  start loop through all polygons  */

    for ( i=0; i < numpoly; i++) {

        jtot = packpoly[i];
        if(jtot < 1) return 1;      /* no points in this polygon */

/*      get limits for separate polygon              */

        for ( j=0;  j < jtot;  j++) {

            if (*xpoly < holeflag) {
                if (*xpoly < *xgmin) *xgmin = *xpoly;
                if (*ypoly < *ygmin) *ygmin = *ypoly;
                if (*xpoly > *xgmax) *xgmax = *xpoly;
                if (*ypoly > *ygmax) *ygmax = *ypoly;
            }
            xpoly++;
            ypoly++;

        }
    }

/*  end of loop through all polygons  */

    return 0;

}  /*  end of ply_glimits function  */






/*

    ***************************************************************

                     p l y _ o r i e n t

    ***************************************************************

  function name:    ply_orient  (integer)

  call sequence:    ply_orient (flag, xpoly, ypoly, npts,
                                xpmin, ypmin, xpmax, ypmax)

  synopsis:         Check if polygon needs to be reoriented and
                    reorient (reverse the point order) if needed

  return values:    status code

                    0  = no reorientation was needed
                    -1 = polygon was reoriented
                    1  = could not determine polygon orientation
                         do not use this polygon for clipping in this
                         case.
                    2  = Bad min/max values passed.
                    3  = Bad flag passed.

  usage:            This function is an integral part of the polygon
                    clipping functions and is not designed for general
                    use.

  algorithm:        The current orientation of the polygon is
                    determined by projecting points a tiny distance
                    from the midpoint of the polygon side at -90
                    degrees (clockwise) and +90 degrees (counter
                    clockwise).  If only one of these points is inside
                    the polygon, the polygon has the orientation of
                    that point.  If neither or both points are inside
                    the process is repeated for the next polygon side.
                    If all sides are checked and no solution is found,
                    there is effectively no area inside the polygon
                    and the polygon has no use in terms of polygon
                    clipping.  After the orientation has been
                    determined, it is compared with the desired
                    orientation.  If needed, the order of the points
                    is reversed using function ply_revers to reorient
                    the polygon.

  description of calling parameters:

    flag      r    int         flag for desired orientation
                               1  = clockwise
                               -1 = counter clockwise
    xpoly     r/w  double*     pointer to x coordinates of the polygon
    ypoly     r/w  double*     pointer yo y coordinates of the polygon
    npts      r    int         number of points in xpoly and ypoly
    xpmin     r    double      minimum x of the polygon
    ypmin     r    double      minimum y of the polygon
    xpmax     r    double      maximum x of the polygon
    ypmax     r    double      maximum y of the polygon

*/

int CSWPolyUtils::ply_orient (
    int flag, double *xpoly, double *ypoly, int npts,
    double xpmin, double ypmin, double xpmax, double ypmax)
{
    double     pi2=1.5707963f;
    double     dx, dy, ang, xanc, yanc, x1, y1, x2, y2;
    double     eps;
    int        i, i1, i2, icw, iccw, itest;

/*  check obvious errors  */

    if(!(flag == 1  ||  flag == -1)) return 3;
    if(xpmax <= xpmin  ||  ypmax <= ypmin) return 2;

    eps = (xpmax - xpmin + ypmax - ypmin) / 10000.f;
    if (eps < Epsilon * 2.f) {
        eps = Epsilon * 2.f;
    }

    itest = 0;
    icw = 0;
    iccw = 0;

/*  loop through sides of the polygon, checking points perpendicular
    to the polygon side at the midpoint of the side.  One point is -90
    degrees (clockwise) from the polygon side and the other point is
    +90 degrees (counter clockwise) from the polygon side  */

    for (i = 0; i < npts; i++) {

        i1 = i;
        i2 = i+1;
        if(i2 >= npts) i2 = 0;
        dx = xpoly[i2] - xpoly[i1];
        dy = ypoly[i2] - ypoly[i1];

/*      if points are coincident, check the next pair  */

        if(dx == 0  &&  dy == 0) continue;

/*      calculate points for inside/outside checks  */

        ang = atan2(dy, dx);
        xanc = (xpoly[i1] + xpoly[i2]) / 2.0f;
        yanc = (ypoly[i1] + ypoly[i2]) / 2.0f;

        x1 = xanc + eps * cos(ang - pi2);
        y1 = yanc + eps * sin(ang - pi2);
        x2 = xanc + eps * cos(ang + pi2);
        y2 = yanc + eps * sin(ang + pi2);

/*      check if clockwise point is inside  */

        icw = ply_point (xpoly, ypoly, npts, x1, y1);
        if(icw == 0) continue;

/*      check if counter clockwise point is inside  */

        iccw = ply_point (xpoly, ypoly, npts, x2, y2);
        if(iccw == 0) continue;

/*      if exactly one point is inside, the solution is good  */

        itest = icw * iccw;
        if(itest < 0) break;

/*      check next side  */

    }

/*  make sure a good solution to the current orientation was found  */

    if (itest >= 0) return 1;

/*  check if reorientation is needed  */

    if(flag == -1) {
        if(iccw == 1) return 0;
    }
    else {
        if(icw == 1) return 0;
    }

/*  reorient the polygon (reverse the order of vertices)  */

    i2 = ply_revers (xpoly, ypoly, npts);
    return -1;

}  /*  end of ply_orient function  */






/*

    ***************************************************************

                          p l y _ p a r m s

    ***************************************************************

  function name:    ply_parms   (integer)

  call sequence:    ply_parms(flag, string, val)

  synopsis:         Read and write parameters for the polygon operations
                    functions.

  return values:    status code

                    0 = successful execution
                    1 = no match for the string passed

  usage:            This function is called by the polygon operations
                    low level routines to get parameters which are
                    changeable with configuration.  The user will not
                    usually need to read parameters.  The user interface
                    will need to write parameters if they differ from
                    the defaults.

  algorithm:        The parameters are stored as private class variables
                    to limit their scope and avoid possible accidental
                    modification.  The value in address val is transfered
                    to and from the proper private class variable based
                    on the keyword passed in string.

  description of calling parameters:

    flag    r    char      Single character flag for reading or writing
                           the parameter.  If flag is 'w' or 'W', then
                           the value pointed to by val is transferred to
                           the private class variable.  For any other
                           value of flag, the private class variable value
                           is assigned to the value of val.
    string  r    char *    Keyword specifying which variable to use.
                           The keywords are not case sensitive.
    val     r/w  double *  Value of the parameter.

        Summary of valid keywords

     "holeflag"   The value defining the start of a polygon hole.
                  When a polygon x coordinate has an absolute value
                  greater than or equal to holeflag, it is assumed
                  that the next coordinate defines the start of a hole in
                  the polygon.  The previous coordinate is assumed to be
                  the end coordinate of the polygon or the end coordinate
                  of a previously defined hole.  The number of points in
                  the polygon must include all points in the polygon,
                  all points in the holes and all hole flags.

     "scanflag"   This is a flag which can be used to force scanning
                  mode or explicit vector calculation mode in the
                  polygon operations.  For instance, in the points
                  inside of polygons operations, normally the mode of
                  operation is determined automatically by the program.
                  (By default, scanflag is set for automatic determination)
                  If you want to override this, pass a 1 to force scanning
                  at all times or pass a -1 to inhibit scanning.  A zero
                  means to automatically select the mode.

      "maxpmem"   Maximum number of points in scratch work arrays.

      "maxhmem"   Maximum number of components in scratch arrays.

      "maxpout"   This sets the maximum array size for points in an output
                  polygon from polygon clipping.  In complicated
                  situations, the output can be much larger than the
                  input.  To insure that memory is not overflowed, the
                  routines to add holes and components to polygon clipping
                  results will check against this parameter.

      "maxhout"   This sets the maximum number of holes that a single
                  component in the polygon clipping results can have.
                  The routines to append to the polygon clipping results
                  will check this parameter to insure no memory overflow.

      "multmem"   This parameter is used to instruct the polygon clipping
                  routines how to set maxpmem and maxhmem.  The amount of
                  memory needed to handle the worst case in polygon
                  clipping is prohibitive.  Rarely does any problem come
                  close to the worst case.  This parameter can be set to
                  a value sufficient for almost all cases, and if a case
                  arises that cannot be handled, the parameter can be
                  increased.
*/

int CSWPolyUtils::ply_parms(
    char flag, const char *string, double *val)
{
    int      i;
    char     tmpstr[11];

/*
     static constant local variables  (This should be thread safe)
*/
    static const char
                   *hflag   = "holeflag",
                   *sflag   = "scanflag",
                   *opflag  = "maxpout",
                   *ohflag  = "maxhout",
                   *mpflag  = "maxpmem",
                   *mhflag  = "maxhmem",
                   *mmflag  = "multmem";

/*  copy string to tmpstr and convert to lower case  */

    strcpy(tmpstr, string);

    for(i=0; i < (int)strlen(tmpstr); i++) {
        if( isupper ( (int)tmpstr[i] ) )    tmpstr[i] = (char)tolower ( (int)tmpstr[i] );
    }

    if ( isupper ( (int)flag ) )  flag = (char)tolower ( (int)flag );

/* ------------------- maxpmem keyword  -------------------------*/

    if(!strncmp(tmpstr, mpflag, 7)) {
        if(flag == 'w') {
            maxpmem  = *val;
        }
        else {
            *val = maxpmem ;
        }
        return 0;
    }

/* ------------------- maxhmem keyword  -------------------------*/

    if(!strncmp(tmpstr, mhflag, 8)) {
        if(flag == 'w') {
            maxhmem  = *val;
        }
        else {
            *val = maxhmem ;
        }
        return 0;
    }

/* ------------------- multmem keyword  -------------------------*/

    if(!strncmp(tmpstr, mmflag, 7)) {
        if(flag == 'w') {
            multmem  = *val;
            MultMem = multmem;
        }
        else {
            *val = multmem ;
        }
        return 0;
    }


/* ------------------- holeflag keyword  ------------------------*/

    if(!strncmp(tmpstr, hflag, 8)) {
        if(flag == 'w') {
            holeflag = *val;
            HoleFlag = holeflag;
        }
        else {
            *val = holeflag;
        }
        return 0;
    }


/* ------------------- scanflag keyword  ------------------------*/

    if(!strncmp(tmpstr, sflag, 8)) {
        if(flag == 'w') {
            scanflag = *val;
            ScanFlag = scanflag;
        }
        else {
            *val = scanflag;
        }
        return 0;
    }

/* ------------------- maxpout keyword  -------------------------*/

    if (!strncmp(tmpstr, opflag, 7)) {
        if (flag == 'w') {
            maxpout = *val;
            MaxPout = maxpout;
        }
        else {
            *val = maxpout;
        }
        return 0;
    }

/* ------------------- maxhout keyword  -------------------------*/

    if (!strncmp(tmpstr, ohflag, 7)) {
        if (flag == 'w') {
            maxhout = *val;
            MaxHout = maxhout;
        }
        else {
            *val = maxhout;
        }
        return 0;
    }

/* ------------------- no match with a keyword ------------------*/

    return 1;

}   /*  end of ply_parms function  */







/*

    ***************************************************************

                         p l y _ r e o r g

    ***************************************************************

  function name:    ply_reorg   (integer)

  call sequence:    ply_reorg(xpoly, ypoly, npts,
                              xmin, ymin, flag)

  synopsis:         Reorigin the polygon coordinates.

  return values:    status code returned

                    0 = successful execution

  usage:            Called by polygon operations functions to
                    make the range of the coordinates smaller
                    and thus increase the precision of line
                    intersection calculations.  It is important
                    that the calling routine call this function
                    in pairs to shift coordinates into and out of
                    the new origin.

  algorithm:        The function first gets the polygon hole flag,
                    and for every point less than the hole flag,
                    xmin and ymin are subtracted or added, depending
                    on the value of flag.

  description of calling parameters:

    xpoly      r    double *    pointer to array of x coordinates
    ypoly      r    double *    pointer to array of y coordinates
    npts       r    int         number of points in xpoly and ypoly
    xmin       r    double      x value to add or subtract
    ymin       r    double      y value to add or subtract
    flag       r    char        flag for adding or subtracting
                                '+' means add
                                '-' means subtract

*/

int CSWPolyUtils::ply_reorg(
    double *xpoly, double *ypoly, int npts,
    double xmin, double ymin, char flag)
{
    int      i;
    double hole;

/*  get polygon hole flag  */

    hole = HoleFlag;

/*  loop through data, adding or subtracting  */

    if(flag == '+') {
        for(i=0; i < npts; i++) {
            if(xpoly[i] < hole) {
                xpoly[i] += xmin;
                ypoly[i] += ymin;
            }
        }
    }

    if(flag == '-') {
        for(i=0; i < npts; i++) {
            if(xpoly[i] < hole) {
                xpoly[i] -= xmin;
                ypoly[i] -= ymin;
            }
        }
    }

    return 0;

}  /*  end of ply_reorg function  */







/*

    ***************************************************************

                       p l y _ r e v e r s

    ***************************************************************

  function name:    ply_revers  (integer)

  call sequence:    ply_revers (xpoly, ypoly, npts)

  synopsis:         Reverse the order of points in a single component
                    a polygon

  return values:    Always returns 0

  usage:            Used by polygon orient function and also as a
                    general utility for reversing the order of x and y
                    coordinates in a string.

  algorithm:        Loop through the first half of the string, exchanging
                    elements of the string with corresponding elements
                    offset from the end of the string.

  description of calling parameters:

    xpoly      double*   pointer to array of polygon x coordinates
    ypoly      double*   pointer to array of polygon y coordinates
    npts       int       number of points in xpoly and ypoly

*/

int CSWPolyUtils::ply_revers (
    double *xpoly, double *ypoly, int npts)
{
    int         i, i2;
    double temp;

/*  reverse order of x and y coordinates  */

    for (i = 0; i < npts/2; i++) {

        i2 = npts - i - 1;

        temp = xpoly[i];
        xpoly[i] = xpoly[i2];
        xpoly[i2] = temp;

        temp = ypoly[i];
        ypoly[i] = ypoly[i2];
        ypoly[i2] = temp;

    }

    return 0;

}  /*  end of ply_revers function  */







/*

    ***************************************************************

                      p l y _ r o t p t s

    ***************************************************************

  function name:    ply_rotpts   (integer)

  call sequence:    ply_rotpts (x, y, nptsin, nrot)

  synopsis:         rotate points in polygon go that point nrot is in
                    the first position in the polygon array

  return values:    status code

                    -1   error allocating memory
                     0   normal successful conclusion
                     1   bad input polygon, probably too few points or all
                         points in the same spot

  description of calling parameters:

    x       r/w  double*    array of polygon x coordinates
    y       r/w  double*    array of polygon y coordinates
    nptsin  r/w  int     *  number of points in x,y on input and output
    nrot    r    int        point number of x,y to reposition as the first
                            point in the polygon

*/

int CSWPolyUtils::ply_rotpts (
    double *x, double *y, int *nptsin, int nrot)
{
    double    *xw = NULL, *yw = NULL;
    int       i, iclose, j, npts;


    auto fscope = [&]()
    {
        csw_Free (xw);
        csw_Free (yw);
    };
    CSWScopeGuard  func_scope_guard (fscope);


    npts = *nptsin;

/*  allocate work space  */

    xw = (double *)csw_Malloc (npts * sizeof (double));
    if (!xw) {
        return -1;
    }

    yw = (double *)csw_Malloc (npts * sizeof (double));
    if (!yw) {
        return -1;
    }

/*  if polygon is closed, open it  */

    iclose = 0;
    while (npts > 2  &&  ply_graze(x[0], y[0], x[npts-1], y[npts-1])) {
        iclose = 1;
        npts--;
    }

    if (npts < 3  ||  nrot >= npts) {
        return 1;
    }

/*  copy to work arrays  */

    for (i=0; i<npts; i++) {
        xw[i] = x[i];
        yw[i] = y[i];
    }

/*  copy back into x,y  */

    j = 0;
    for (i=nrot; i<npts; i++) {
        x[j] = xw[i];
        y[j] = yw[i];
        j++;
    }
    for (i=0; i<nrot; i++) {
        x[j] = xw[i];
        y[j] = yw[i];
        j++;
    }

/*  close if original was closed  */

    if (iclose) {
        x[j] = x[0];
        y[j] = y[0];
        j++;
    }

    *nptsin = j;

    return 0;

}  /*  end of function ply_rotpts  */






/*

    ***************************************************************

                        p l y _ s c a n

    ***************************************************************

  function name:    ply_scan    (integer)

  call sequence:    ply_scan(hgrid, vgrid, iogrid, bcell,
                             nd1, nd2, ncol, nrow,
                             xpoly, ypoly, packpoly, numpoly,
                             xgmin, ygmin, xspace, yspace,
                             xpmin, ypmin, xpmax, ypmax)

  synopsis:         Set inside/outside flags in a grid covering a set
                    of polygons.

  return values:    status code returned

                    0 = successful execution
                    -1 = memory allocation error

  usage:            This function is called by the points in polygon
                    function (ply_points) and also by polygon intersection
                    functions.  The grids must be allocated by the calling
                    function.  This function initializes all three grids
                    to zero, so the calling function doesn't need to
                    initialize the grids.

  algorithm:        This function initializes all grids to zero and then
                    separates the set of polygons into separate polygons.
                    Function ply_scan1 is called to set the iogrid flags
                    for each separate polygon.  The final iogrid will be
                    a union of the inside flags for each separate polygon.

  description of calling parameters:

    hgrid     r   char *      Pointer to grid used for horizontal crossing
                              flags.
    vgrid     r   char *      Pointer to grid for vertical crossing flags
    iogrid    w   char *      Pointer to grid of inside/outside flags
    bcell     w   char *      Pointer to grid for boundary crossing of grid cell.
    nd1       r   int         first physical dimension of grids
    nd2       r   int         second physical dimension of grids
    ncol      r   int         number of columns in the grids
    nrow      r   int         number of rows in the grids
    xpoly     r   double *    pointer to array of polygon x coordinates
    ypoly     r   double *    pointer to array of polygon y coordinates
    packpoly  r   int      *  pointer to array with the number of vertices
                              in each polygon component
    numpoly   r   int         number of polygons in the set
    xgmin     r   double      minimum x coordinate of the grids
    ygmin     r   double      minimum y coordinate of the grids
    xspace    r   double      spacing between columns of the grid
    yspace    r   double      spacing between rows of the grid
    xpmin     r   double *    pointer to array of minimum x coordinates
                              for each individual polygon
    ypmin     r   double *    pointer to array of individual y minimums
    xpmax     r   double *    pointer to array of individual x maximums
    ypmax     r   double *    pointer to array of individual y maximums

*/

int CSWPolyUtils::ply_scan(
    char *hgrid, char *vgrid, char *iogrid, char *bcell,
    int nd1, int nd2, int ncol, int nrow,
    double *xpoly, double *ypoly, int *packpoly, int numpoly,
    double xgmin, double ygmin, double xspace, double yspace,
    double *xpmin, double *ypmin, double *xpmax, double *ypmax)
{
    int        n, npts, i, istat, numpolyloc, ido, ndo, ntot;
    int        npnest, *ncnest = NULL, *icnest = NULL, npscan,
               *ncscan = NULL, *icscan = NULL;
    double     **xpw = NULL, **ypw = NULL;
    double     *xnest = NULL, *ynest = NULL, *xscan = NULL, *yscan = NULL;
    double     x1, y1, x2, y2, dval;

// lambda expression captures all local variables by reference.
// Any cleanup needed upon this function going out of scope
// should be done in the body (between curly braces) of the
// expression.
    auto fscope = [&]()
    {
        csw_Free (xnest);
        csw_Free (ncnest);
        csw_Free (xpw);
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*
    The packed polygon needs to be unnested prior
    to scanning each individual polygon.  The results of
    unnesting produce polygons with at most one level
    of holes.
*/
    n = 0;
    for (i=0; i<numpoly; i++) {
        n += packpoly[i];
    }
    n *= 2;
    xnest = (double *)csw_Malloc (n * 2 * sizeof(double));
    if (!xnest) {
        return -1;
    }
    ynest = xnest + n;
    dval = n;
    ply_parms ('w', "maxpout", &dval);
    n = numpoly * 2;
    ncnest = (int *)csw_Malloc (n * 2 * sizeof(int));
    if (ncnest == NULL) {
        return -1;
    }
    icnest = ncnest + n;
    dval = n;
    ply_parms ('w', "maxhout", &dval);

    numpolyloc = numpoly;

    istat = ply_holnest (xpoly, ypoly, numpoly, packpoly,
                         xnest, ynest, &npnest, ncnest, icnest);

    if (istat == 2) {
        npscan = 1;
        ncscan = &numpolyloc;
        icscan = packpoly;
        xscan = xpoly;
        yscan = ypoly;
    }
    else if (istat == 0) {
        npscan = npnest;
        ncscan = ncnest;
        icscan = icnest;
        xscan = xnest;
        yscan = ynest;
    }
    else {
        return -1;
    }

/*
    initialize hgrid and vgrid arrays with zeros
*/
    npts = nd1 * nd2;

    csw_GridZero (hgrid,npts);
    csw_GridZero (vgrid,npts);
    csw_GridZero (bcell,npts);

/*
    initialize iogrid array with 0 flags for adjacent
*/
    for (i=0; i<npts; i++) {
        iogrid[i] = 0;
    }

/*
    get cumulative limits of all polygons
*/
    x1 = 1.e30f;
    y1 = 1.e30f;
    x2 = -1.e30f;
    y2 = -1.e30f;
    for (i=0; i<numpoly; i++) {
        if (xpmin[i] < x1) x1 = xpmin[i];
        if (ypmin[i] < y1) y1 = ypmin[i];
        if (xpmax[i] > x2) x2 = xpmax[i];
        if (ypmax[i] > y2) y2 = ypmax[i];
    }

/*
    Loop through each unnested polygon and scan it.
*/
    ndo = 0;
    ntot = 0;
    for (ido = 0; ido<npscan; ido++) {

        numpoly = ncscan[ido];
        packpoly = icscan + ndo;
        xpoly = xscan + ntot;
        ypoly = yscan + ntot;

        ndo += numpoly;

    /*
        allocate memory for component pointers
    */
        i = numpoly + 1;
        xpw = (double **)csw_Malloc (i * 2 * sizeof(double *));
        if (!xpw) {
            return -1;
        }
        ypw = xpw + i;

        n = 0;
        for (i=0; i<numpoly; i++) {
            xpw[i] = xpoly + n;
            ypw[i] = ypoly + n;
            n += packpoly[i];
            ntot += packpoly[i];
        }

    /*  separate the individual polygons and scan them one at a time  */

        ply_scan1(xpw, ypw, packpoly, numpoly,
                  hgrid, vgrid, iogrid, bcell,
                  ncol, nrow, nd1, nd2,
                  xspace, yspace, xgmin, ygmin,
                  x1, y1, x2, y2);

        CorrectForSmallHoles (xpmin, ypmin, xpmax, ypmax, numpoly,
                              xgmin, ygmin, xspace, yspace,
                              nd1, ncol, nrow,
                              iogrid, bcell);
        csw_Free (xpw);
        xpw = NULL;

    }

    return 0;

}  /*  end of ply_scan function  */







/*

    ***************************************************************

                        p l y _ s c a n 1

    ***************************************************************

  function name:    ply_scan1    (integer)

  call sequence:    ply_scan1(xpoly, ypoly, npts, ncomp,
                              hgrid, vgrid, iogrid, bcell,
                              ncol, nrow, nd1, nd2,
                              xspace, yspace, xgmin, ygmin,
                              xmin, ymin, xmax, ymax)

  synopsis:         Scan for grid nodes inside a single polygon.

  return values:    status code returned

                    0 = successful execution
                    1 = illegal polygon, either less than 3
                        points or all the points are coincident.

  usage:            This function is called from the ply_points function
                    and also from the polygon intersection functions.
                    It is assumed that the grid geometry has been set up
                    so that a margin of at least one complete grid cell
                    exists on all sides of the polygon set.  This is
                    essential for proper execution of this function.
                    No checks are done here, because speed is of the
                    essence in this routine.  The proper setup must be done
                    by the calling function.

  algorithm:        The crossings of the polygon side with the grid lines
                    are flagged.  Then, starting at a grid node 1 row down
                    and 1 column left of the lower left corner of a
                    rectangle circumscribing the polygon, the grid nodes
                    are deemed outside.  As one scans along each row, if
                    a polygon side crossing is encountered, the inside/
                    outside flag is toggled.  After all inside/outside
                    nodes are assigned, another pass is made flagging the
                    nodes which are adjacnt to crossings.

  description of calling parameters:

    xpoly     r   double**   Array of pointers to x coordinates of each component
    ypoly     r   double**   Array of pointers to y coordinates of each component
    npts      r   int*       Number of points in each polygon component
    ncomp     r   int        Number of polygon components
    hgrid     r   char *     Pointer to array of flags for horizontal grid
                             line crossings.
    vgrid     r   char *     Pointer to array of flags for vertical grid
                             line crossings.
                             hgrid and vgrid are reset to zero prior to
                             returning from this function.
    iogrid    w   char *     Pointer to array of flags for inside outside
                             or adjacent condition at each grid node.
                             inside = 1, adjacent = 0, outside = -1
    bcell     w   char*      Pointer to boundary crossings for grid cells
                             0 = no boundary crossing  1 = boundary crossing
    ncol      r   int        number of columns in the grid
    nrow      r   int        number of rows in the grid
    nd1       r   int        first physical dimension of the grid
    nd2       r   int        second physical dimension of the grid
    xspace    r   double     spacing between columns
    yspace    r   double     spacing between rows
    xgmin     r   double     minimum x coordinate of the grid
    ygmin     r   double     minimum y coordinate of the grid
    xmin      r   double     minimum x coordinate of the polygon
    ymin      r   double     minimum y coordinate of the polygon
    xmax      r   double     maximum x coordinate of the polygon
    ymax      r   double     maximum y coordinate of the polygon

*/

int CSWPolyUtils::ply_scan1(
    double **xpoly, double **ypoly, int *npts, int ncomp,
    char *hgrid, char *vgrid, char *iogrid, char *bcell,
    int ncol, int nrow, int nd1, int nd2,
    double xspace, double yspace, double xgmin, double ygmin,
    double xmin, double ymin, double xmax, double ymax)
{
    double     xt, yt;
    int        imin, jmin, imax, jmax;
    int        outer, index, idirt, i, j;

/*  calculate row and column limits of the polygon  */

    jmin = (int)((xmin-xgmin) / xspace) - 2;
    jmax = (int)((xmax-xgmin) / xspace) + 3;
    imin = (int)((ymin-ygmin) / yspace) - 2;
    imax = (int)((ymax-ygmin) / yspace) + 3;

    if(jmin < 1)    jmin = 1;
    if(jmax > ncol-2) jmax = ncol-2;
    if(imin < 1)    imin = 1;
    if(imax > nrow-2) imax = nrow-2;

/*  special case for polygon enclosed by a single grid cell
    set each corner of the grid cell to zero (adjacent flag)
    and return  */

    if (jmax-jmin <= 1  &&  imax-imin <= 1) {
        outer = imin * nd1;
        idirt = outer + jmin;
        *(iogrid+idirt) = 0;
        *(iogrid+idirt+1) = 0;
        *(iogrid+idirt+nd1) = 0;
        *(iogrid+idirt+nd1+1) = 0;
        *(bcell+idirt) = 1;
        return 0;
    }

/*
    in all other circumstances, the polygon spans more than one grid cell,
    scan and fill the polygon
*/

/*  set crossings in hgrid and vgrid  */

    idirt = ply_side_set(hgrid, vgrid, nd1, nd2, ncol, nrow,
                         xspace, yspace, xgmin, ygmin,
                         xpoly[0], ypoly[0], npts, ncomp);
    if(idirt) {
        return 1;
    }

/*  scan for inside/outside starting at outside on each row */

    for (i=imin; i <= imax; i++) {
        outer = i * nd1;
        index = -1;
        yt = ygmin + yspace * i;
        for (j=jmin; j < jmax; j++) {
            idirt = outer + j;
            iogrid[idirt] = (char)index;
/*
            if(*(iogrid+idirt) != 2) {
                *(iogrid+idirt) = (char)(index + 1);
            }
*/
            if(*(hgrid+idirt) != 0  ||  *(hgrid+idirt-1) != 0) {
                xt = xgmin + xspace * (j + 1);
                index = ply_pointpa (xpoly, ypoly, ncomp, npts, xt, yt);
                if (index == 0) index = 1;
            }
        }
    }

/*  scan for nodes adjacent to polygon sides and flag them with zeros  */

    for (i=imin; i <= imax; i++) {
        outer = i * nd1;
        for (j=jmin; j <= jmax; j++) {
            idirt = outer + j;
            if(*(hgrid+idirt) != 0  ||  hgrid[idirt-1] != 0) {
                if (i < imax  &&  j < jmax) {
                    *(bcell+idirt) = 1;
                    if (i > 0) {
                        *(bcell + idirt - nd1) = 1;
                    }
                }
            }
            if(*(vgrid+idirt) != 0  ||  vgrid[idirt-1] != 0) {
                if (i < imax  &&  j < jmax) {
                    *(bcell+idirt) = 1;
                    if (j > 0) {
                        *(bcell + idirt - 1) = 1;
                    }
                }
            }
        }
    }

/*  erase hgrid and vgrid for the rectangle circumscribing the polygon */

    for (i=imin; i <= imax; i++) {
        outer = i * nd1;
        for (j=jmin; j <= jmax; j++) {
            idirt = outer + j;
            *(hgrid+idirt) = 0;
            *(vgrid+idirt) = 0;
        }
    }

    return 0;

}   /*   end of ply_scan1 function   */



/*
 * This is only used by ply_segint below
 */
double CSWPolyUtils::_minDist (
    double x1, double y1, double x2, double y2,
    double x3, double y3, double x4, double y4)
{
    double        dx, dy, dist, dmin;

    dmin = 1.e40;

    dx = x1 - x3;
    dy = y1 - y3;
    dist = dx * dx + dy * dy;
    if (dist < dmin) {
        dmin = dist;
    }

    dx = x1 - x4;
    dy = y1 - y4;
    dist = dx * dx + dy * dy;
    if (dist < dmin) {
        dmin = dist;
    }

    dx = x2 - x3;
    dy = y2 - y3;
    dist = dx * dx + dy * dy;
    if (dist < dmin) {
        dmin = dist;
    }

    dx = x2 - x4;
    dy = y2 - y4;
    dist = dx * dx + dy * dy;
    if (dist < dmin) {
        dmin = dist;
    }

    dist = sqrt (dmin);

    return dist;

}

/*

    ***************************************************************

                      p l y _ s e g i n t

    ***************************************************************

  function name:    ply_segint  (integer)

  call sequence:    ply_segint (x1, y1, x2, y2, x3, y3, x4, y4, &x, &y)

  synopsis:         Calculates the intersection between two vectors, and
                    returns the intersecting point in x and y.

  return values:    Status code

                    0 = normal successful return (x and y are the unique
                        intersection point inside the vectors passed)
                    1 = the lines intersect, but not inside the limits of
                        the vectors.  X and y are still the intersection
                        point.
                    2 = the segments are parallel and touch at one point.
                        this point is returned in x and y.
                    3 = The lines are parallel and overlap.  x and y are
                        not returned in this case since there are an
                        infinite number of solutions.
                    4 = The two vectors are identical.  No intersection
                        point is calculated.
                    5 = The lines are parallel and do not intersect at all.
                    6 = At least one vector has zero length, and no
                        intersection can be found.

  usage:            This is intended as a general utility function to be
                    used in all of the polygon operations functions.  In
                    the case of parallel overlaping lines, the calling
                    function will have to decide what to do.

  algorithm:        Special cases are used if either or both of the vectors
                    are vertical.  After the intersection point has been
                    calculated, it is determined whether the point is
                    inside of both vectors.

  description of calling parameters:

    x1    r    double    x coordinate of 1st endpoint of 1st vector
    y1    r    double    y coordinate of 1st endpoint of 1st vector
    x2    r    double    x coordinate of 2nd endpoint of 1st vector
    y2    r    double    y coordinate of 2nd endpoint of 1st vector
    x3    r    double    x coordinate of 1st endpoint of 2nd vector
    y3    r    double    y coordinate of 1st endpoint of 2nd vector
    x4    r    double    x coordinate of 2nd endpoint of 2nd vector
    y4    r    double    y coordinate of 2nd endpoint of 2nd vector
    x     w    double*   pointer to x coordinate of intersection point
    y     w    double*   pointer to y coordinate of intersection point

*/

int CSWPolyUtils::ply_segint(
    double x1, double y1, double x2, double y2,
    double x3, double y3, double x4, double y4,
    double *x, double *y)
{
    double       s1, s2, b1, b2, tiny, mindist;
    int          sameline=0, i1, i2, i3, i4;
    double       dtmp, dtiny;

    i1 = 0;
    i2 = 0;
    i3 = 0;
    i4 = 0;

/*
 * If a segment is very close to vertical,
 * make it exactly vertical.
 */
    dtiny = Epsilon / 10.0;
    if (dtiny < 1.e-9) {
        dtiny = 1.e-9;
    }
    dtmp = x2 - x1;
    if (dtmp < 0.0) dtmp = -dtmp;
    if (dtmp < dtiny) {
        x2 = x1;
    }
    dtmp = x4 - x3;
    if (dtmp < 0.0) dtmp = -dtmp;
    if (dtmp < dtiny) {
        x4 = x3;
    }

/*  check if either vector has zero length  */

    if (ply_graze (x1, y1, x2, y2)) {
        return 6;
    }
    if (ply_graze (x3, y3, x4, y4)) {
        return 6;
    }

/*  both segments are identical   */

    if(x1 - x3 == 0.0  &&  y1 - y3 == 0.0  &&
       x2 - x4 == 0.0  &&  y2 - y4 == 0.0) return 4;
    if(x2 - x3 == 0.0  &&  y2 - y3 == 0.0  &&
       x1 - x4 == 0.0  &&  y1 - y4 == 0.0) return 4;

/*  calculate slopes and intercepts of the vectors */

    if(x1 - x2 != 0.0) {
        s1 = (y2-y1) / (x2-x1);
        b1 = y1 - s1 * x1;
    }
    else {
        s1 = 1.e30f;
        b1 = 1.e30f;
    }
    if(x3 - x4 != 0.0) {
        s2 = (y4-y3) / (x4-x3);
        b2 = y3 - s2 * x3;
    }
    else {
        s2 = 1.e30f;
        b2 = 1.e30f;
    }

/*  special cases 1st vector is vertical or both are vertical */

    if(x1 - x2 == 0.0) {

/*      both vectors vertical  */

        if(x3 - x4 == 0.0) {
            if(x3 - x1 == 0.0) {
                sameline = 1;
            }
            else {
                return 5;
            }
        }

/*      only 1st vector is vertical  */

        else {
            *y = s2 * x1 + b2;
            *x = x1;
        }
    }

/*  special case, second vector is vertical  */

    else if(x3 - x4 == 0.0) {
        *y = s1 * x3 + b1;
        *x = x3;
    }

/*  general case, neither vector is vertical  */

    else {

/*      same slope means that the vectors are parallel or overlapping  */

        tiny = s1 - s2;
        if(tiny > -0.00001f  &&  tiny < 0.00001f) {
            if (ply_graze ((double)0.0, b1, (double)0.0, b2)) {
                sameline = 1;
            }
            else {
                return 5;
            }
        }

/*      the slopes are not equal, so the intersection can be calculated  */

        else {
            *x = (b1-b2) / (s2-s1);
            *y = s1 * (*x) + b1;
        }
    }

/*  check if the intersection point is on the vector  */

/*  first for non overlapping lines  */

    if(!sameline) {

    /*
     * If the slope of a segment is greater than 1, make sure the
     * intersection point is inside its y coordinates.  If the
     * slope is less than 1, make sure the intersection point is
     * inside its x coordinates.
     */
        if (s1 > 1.0) {
            i1 = 1;
            if ((y1 - *y) * (*y - y2) >= 0) {
                i2 = 1;
            }
        }
        else {
            i2 = 1;
            if ((x1 - *x) * (*x - x2) >= 0) {
                i1 = 1;
            }
        }

        if (s2 > 1.0) {
            i3 = 1;
            if ((y3 - *y) * (*y - y4) >= 0) {
                i4 = 1;
            }
        }
        else {
            i4 = 1;
            if ((x3 - *x) * (*x - x4) >= 0) {
                i3 = 1;
            }
        }

        if (i1 + i2 + i3 + i4 == 4) {
            return 0;
        }

/*      check if one endpoint is coincident  */

        if (x1 - x3 == 0.0  &&  y1 - y3 == 0.0) {
            *x = x1;
            *y = y1;
            return 0;
        }
        if (x1 - x4 == 0.0  &&  y1 - y4 == 0.0) {
            *x = x1;
            *y = y1;
            return 0;
        }
        if (x2 - x3 == 0.0  &&  y2 - y3 == 0.0) {
            *x = x2;
            *y = y2;
            return 0;
        }
        if (x2 - x4 == 0.0  &&  y2 - y4 == 0.0) {
            *x = x2;
            *y = y2;
            return 0;
        }

/*      segments do not intersect  */

        return 1;
    }

/*  check if segments of overlapping line overlap or touch at one point */

    else {
        if(((x1-x3) * (x3-x2) >= 0  ||
            (x1-x4) * (x4-x2) >= 0  ||
            (x3-x1) * (x1-x4) >= 0  ||
            (x3-x2) * (x2-x4) >= 0)
            &&
           ((y1-y3) * (y3-y2) >= 0  ||
            (y1-y4) * (y4-y2) >= 0  ||
            (y3-y1) * (y1-y4) >= 0  ||
            (y3-y2) * (y2-y4) >= 0)) {

/*          do segments touch at one point  */

            if( (x1-x3==0.0 && y1-y3==0.0 &&
                (x4-x1)*(x1-x2)>=0 && (y4-y1)*(y1-y2)>=0 ) ||
                (x1-x4==0.0 && y1-y4==0.0 &&
                (x3-x1)*(x1-x2)>=0 && (y3-y1)*(y1-y2)>=0 )) {
                *x = x1;
                *y = y1;
                return 2;
            }

            if( (x2-x3==0.0 && y2-y3==0.0 &&
                (x1-x2)*(x2-x4)>=0 && (y1-y2)*(y2-y4)>=0 ) ||
                (x2-x4==0.0 && y2-y4==0.0 &&
                (x1-x2)*(x2-x3)>=0 && (y1-y2)*(y2-y3)>=0 )) {
                *x = x2;
                *y = y2;
                return 2;
            }

/*          segments overlap an arbitrary amount  */

            mindist = _minDist (x1, y1, x2, y2, x3, y3, x4, y4);
            if (mindist < Epsilon * 2.0) {
                return 2;
            }

            return 3;
        }

/*      sameline segments do not overlap or touch  */

        else {
            return 5;
        }
    }

}   /*  end of ply_segint function  */






/*

    ***************************************************************

                    p l y _ s i d e _ s e t

    ***************************************************************

  function name:    ply_side_set   (integer)

  call sequence:    ply_side_set (hgrid, vgrid, nd1, nd2, ncol, nrow,
                                  xspac, yspac, xmin, ymin,
                                  xpoly, ypoly, packpoly, npoly)

  synopsis:         Set up raster grid crossing tables for a set of
                    polygons.

  return values:    Returns a status code

                    0 = No errors in execution
                    1 = npoly passed is less than 1
                    2 = A polygon in the set has less then
                        three points or all points are
                        coincident for a polygon in the set.

  usage:            This function is called from the polygon operation
                    routines as part of points in polygon or polygon
                    boolean operations.  The calling routine must have
                    allocated memory for hgrid and vgrid.  The size of
                    the underlying grid, and the spacing of the grid
                    shoild have been calculated by the calling routine,
                    probably via function ply_grid_set.  The calling
                    routine must insure that hgrid and vgrid are
                    initialized to zero before calling this function.

  algorithm:        Unpack the sides of each polygon and pass the vector
                    to ply_xvec to calculate the crossings.  The vectors
                    are shifted to a local coordinate system with xmin
                    and ymin at 0,0 in order to increase the intersection
                    calculation accuracy.

  description of calling parameters:

    hgrid      w    char *    Pointer to grid of horizontal line
                              crossings.
    vgrid      w    char *    Pointer to grid of vertical line
                              crossings.
    nd1        r    long      First physical dimension of the grids
    nd2        r    long      Second physical dimension of the grids
    ncol       r    long      Number of columns of data in each grid
    nrow       r    long      Number of rows of data in each grid
    xspac      r    double    Spacing between adjacent columns
    yspac      r    double    Spacing between adjacent rows
    xmin       r    double    Minimum x coordinate of the grid
    ymin       r    double    Minimum y coordinate of the grid
    xpoly      r    double *  Pointer to array of polygon x coordinates
    ypoly      r    double *  Pointer to array of polygon y coordinates
    packpoly   r    long *    Pointer to array with the number of vertices
                              in each polygon.
    npoly      r    long      Number of polygons in the set.

*/

int CSWPolyUtils::ply_side_set (
    char *hgrid, char *vgrid,
    int nd1, int nd2, int ncol, int nrow,
    double xspac, double yspac, double xmin, double ymin,
    double *xpoly, double *ypoly, int *packpoly, int npoly )
{
    int          i, j, k, idirt, i1, i2, i3, i1sav, i2sav;
    double       hole;

/*  check for obvious errors in calling parameters  */

    if (npoly < 1) return 1;

/*  read the polygon hole flag  */

    hole = HoleFlag;

/*    separate into individual polygons, separate vectors and
      calculate crossings    */

    i1=0;
    for (i=0; i < npoly; i++) {
        i1sav = i1;
/*      i3 is the last possible point in the polygon-hole combination  */

        i3 = packpoly[i] + i1;
        k = i1;

/*      find hole flags and treat the segment prior to the hole flag as if
        it is a polygon.  repeat until the last (i3) point is encountered  */

        while (k < i3) {

            while (k < i3) {
                if (xpoly[k] >= hole) {
                    break;
                }
                k++;
            }

            i2 = k-1;
            i2sav = i2;

/*          sides for separate polygon or hole
            make sure the polygon or hole is not closed    */

            while (xpoly[i1] - xpoly[i2] == 0.0  &&
                   ypoly[i1] - ypoly[i2] == 0.0) {
                i2--;
                if(i2 == i1) return 2;
            }

/*          loop through sides and set grid  */

            for (j=i1; j < i2; j++) {
                idirt = ply_xvec (xpoly[j], ypoly[j],
                                  xpoly[j+1], ypoly[j+1],
                                  hgrid, vgrid, nd1, nd2, ncol, nrow,
                                  xspac, yspac, xmin, ymin);
                if(idirt) return 2;
            }

/*          last side connecting with first point  */

            idirt = ply_xvec (xpoly[i1], ypoly[i1],
                              xpoly[i2], ypoly[i2],
                              hgrid, vgrid, nd1, nd2, ncol, nrow,
                              xspac, yspac, xmin, ymin);
            if(idirt) return 2;

/*          increment first point index for next hole  */

            i1 = i2sav + 2;
            k = i1;

        }    /*  end of while loop through holes */


/*  increment first point index for next separate polygon   */

        i1 = i1sav + packpoly[i];

    }    /*  end of loop through polygons */

    return 0;

}  /*  end of ply_side_set function  */





/*

    ***************************************************************

                        p l y _ x v e c

    ***************************************************************

  function name:    ply_xvec   (integer)

  call sequence:    ply_xvec (x1, y1, x2, y2,
                              hgrid, vgrid, nd1, nd2, ncol, nrow,
                              xspac, yspac, xmin, ymin)

  synopsis:         set horizontal and vertical grid crossing
                    flags for a vector.

  return values:    returns a status code

                    0 = Execution completed successfully.

  usage:            Called from polygon operations, grid masking and
                    eventually from faulted grid and contour calculations.
                    This is a low level function requiring memory
                    allocation and other preprocessing steps by
                    the calling routine.

  algorithm:        The end points of the vector define a locus of
                    horizontal and vertical lines which may intersect
                    the vector.  The intersections are calculated and
                    the the grid point is set to 1 if a vector crosses the
                    grid cell side.

  description of calling parameters:

    x1,y1     r    double    x and y coordinates of the first vector
                             endpoint
    x2,y2     r    double    x and y coordinates of second endpoint
    hgrid     w    char *    pointer to grid of horizontal crossings
    vgrid     w    char *    pointer to grid of vertical crossings
    nd1       r    int       first physical dimension of grids
    nd2       r    int       second physical dimension of grids
    ncol      r    int       number of columns of data in each grid
    nrow      r    int       number of rows of data in each grid
    xspac     r    double    spacing between adjacent columns
    yspac     r    double    spacing between adjacent rows
    xmin      r    double    minimum x value in the grid
    ymin      r    double    minimum y value in the grid

*/

int CSWPolyUtils::ply_xvec (
    double x1, double y1, double x2, double y2,
    char *hgrid, char *vgrid,
    int nd1, int nd2, int ncol, int nrow,
    double xspac, double yspac, double xmin, double ymin)
{
    double     slope, yint, dirt;
    int        nr1, nr2, nc1, nc2, offset, i, j;

/*  suppress warning  */

    nd2 = nd2;

/*  translate coordinates to be relative to xmin, ymin  */

    x1 -= xmin;
    y1 -= ymin;
    x2 -= xmin;
    y2 -= ymin;

/*  calculate first and last horizontal lines to check  */

    if (y1 < y2) {
        nr1 = (int)(y1 / yspac) + 2;
        nr2 = (int)(y2 / yspac) + 1;
    }
    else {
        nr1 = (int)(y2 / yspac) + 2;
        nr2 = (int)(y1 / yspac) + 1;
    }

/*  make sure the vector is inside the grid  */

    if (nr2 < 1) return 0;
    if (nr1 > nrow) return 0;

/*  calculate first and last vertical lines to check  */

    if (x1 < x2) {
        nc1 = (int)(x1 / xspac) + 2;
        nc2 = (int)(x2 / xspac) + 1;
    }
    else {
        nc1 = (int)(x2 / xspac) + 2;
        nc2 = (int)(x1 / xspac) + 1;
    }

/*  make sure the vector is inside the grid  */

    if(nc2 < 1) return 0;
    if(nc1 > ncol) return 0;

/*  adjust ranges of lines so that all are inside the grid  */

    if(nr1 < 1) nr1=1;
    if(nr2 > nrow) nr2=nrow;
    if(nc1 < 1) nc1=1;
    if(nc2 > ncol) nc2=ncol;

/*  calculate the slope and intercept of the vector  */

    if (x1 - x2 == 0.0) {
        slope = 1.e10f;
    }
    else {
        slope = (y2-y1) / (x2-x1);
    }
    yint = y1 - slope * x1;
    if(fabs(slope) <= 1.e-10f) slope = 1.e-10f;

/*  loop through intersections with horizontal lines  */

    for (i=nr1-1; i<nr2; i++) {
        dirt = i * yspac;
        dirt = (dirt-yint) / slope;
        j = (int)(dirt / xspac);
        if(j >= 0  &&  j < ncol) {
            offset = i*nd1 + j;
            *(hgrid+offset) = (char)(*(hgrid+offset) + 1);
            if (*(hgrid+offset) > 10) *(hgrid+offset) = (char)10;
        }
    }

/*  loop through intersections with vertical lines  */

    for (i=nc1-1; i<nc2; i++) {
        dirt = i * xspac;
        dirt = slope * dirt + yint;
        j = (int)(dirt / yspac);
        if ( j >= 0  &&  j < nrow ) {
            offset = j * nd1 + i;
            *(vgrid+offset) = 1;
        }
    }

    return 0;

}  /*   end of ply_xvec function  */


/*

    ***************************************************************

                      p l y _ p o i n t

    ***************************************************************

  function name:  ply_point  (integer)

  call sequence:  ply_point (xpoly,ypoly,numvert,p,q)

  synopsis:       Function to return the location of a point
                  relative to a single polygon component.

  return values:  -1  point is outside polygon
                   0  point is on polygon boundary
                   1  point is inside polygon

  usage:          This function is a kernal function, in that it
                  does not call any other functions.  Higher level
                  functions are used to separate single polygons
                  from a group and for other preprocessing steps.
                  For maximum accuracy, shift the coordinates of
                  the polygon vertices and the target point to be
                  relative to a local origin prior to calling this
                  function.  This will make the line intersection
                  and grazing calculations more accurate.

  algorithm:      The algorithm involves calculation of intersections
                  between the polygon sides and a ray extending
                  vertically (+ y) from the target point.  If there
                  are an odd number of intersections, the target point
                  is inside the polygon.  For an even number of
                  intersections, the point is outside of the polygon.
                  If any of the intersection points is coincident with
                  the target point, the target point lies on the
                  boundary of the polygon.

  description of calling parameters:

    xpoly        r   double *  pointer to array of x coordinates
                               for the polygon vertices
    ypoly        r   double *  pointer to array of y coordinates
                               for the polygon vertices
    numvert      r   int       number of vertices in xpoly and ypoly
    p            r   double    x coordinate of target point
    q            r   double    y coordinate of target point

*/

int  CSWPolyUtils::ply_point (
    double *xpoly, double *ypoly,
    int numvert,
    double p, double q)
{
    double     eps, x1, y1, x2, y2, dist;
    double     x1save = 1.e30f;
    double     y, zero=0.0f;
    int        i, kount=(int)zero, graz=(int)zero, graz1=0;
    int        istat, i2;

/*      If the polygon is closed, unclose it.    If this is not
        done, there is a tiny chance that, if the first point grazes,
        it will be counted more than once in the intersection counter
        and thus mess up the inside/outside rules */

        i2 = numvert - 1;
        while (xpoly[0] - xpoly[i2] == 0.0  &&
               ypoly[0] - ypoly[i2] == 0.0) {
            i2--;
            if(i2 < 2) {
                return -1;
            }
        }

/*      make sure at least 3 points are left after opening polygon  */

        if(i2 < 2) {
            return -1;
        }

/*
 * Check if the point is almost exactly on an edge of the triangle.
 */
        for (i=0; i<=i2; i++) {
            x1 = xpoly[i];
            y1 = ypoly[i];
            if (ply_graze (x1, y1, p, q)) {
                return 0;
            }
            if(i == i2) {
                x2 = xpoly[0];
                y2 = ypoly[0];
            }
            else {
                x2 = xpoly[i+1];
                y2 = ypoly[i+1];
            }
            istat = gpf_perpdistance2 (x1, y1, x2, y2, p, q, &dist);
            if (istat == 1  &&  dist <= Epsilon) {
                return 0;
            }
        }

/*      Main loop of the function.  Loop through polygon or hole sides and
        check each side of the polygon for an intersection with a vertical
        ray extending upward from the target point
        If grazing (intersection with a vertex) occurs, make sure only one
        or in some cases zero, intersections are counted  */

        graz = 0;
        graz1 = 0;
        y2 = 1.e30f;
        for (i=0; i <= i2; i++) {
            x1 = xpoly[i];
            y1 = ypoly[i];
            if (ply_graze (x1, y1, p, q)) {
                return 0;
            }
            if (i == 0 &&  x1 - p == 0.0) {
                eps = Epsilon / 10.f;
                p += eps;
            }
            if(i == i2) {
                x2 = xpoly[0];
                y2 = ypoly[0];
            }
            else {
                x2 = xpoly[i+1];
                y2 = ypoly[i+1];
            }

/*         if the polygon side is vertical, it only intersects if the target
           point lies exactly on the polygon boundary or if the target point
           is at the same x coordinate and below the bottom point of the
           vertical polygon side  */

            if (x1 - x2 == 0.0) {
                istat = (p-x1 >= -Epsilon  &&  p-x1 <= Epsilon);
                if (istat && (y1-q)*(q-y2) >= zero) return 0;
                if (p - x1 == 0.0  &&  i == i2  &&  kount > 0) {
                    if (q < y1  &&  q < y2) kount++;
                }
            }

/*         the side is non vertical, check for intersection with the
           vertical ray from the target point    */

            else {

                if ((x1-p) * (p-x2) > zero) {
                    y = y1 + (y2 - y1) * (p - x1) / (x2 - x1);
                    if (q-y >= -Epsilon  &&  q-y <= Epsilon) return 0;;
                    if (y > q) kount++;
                }

/*              check for grazing at the initial point on this side  */

                else if (p - x1 == 0.0) {
                    if (q-y1 >= -Epsilon  &&  q-y1 <= Epsilon) return 0;
                    if (graz) {
                        graz = 0;
                        if ((x1save-p) * (p-x2) > zero && y1 > q) kount++ ;
                    }

                    else {
                        graz1 = 1;
                    }
                 }

/*               check for terminal point grazing  */

                 else if ( p - x2 == 0.0) {
                     if (q-y2 >= -Epsilon  &&  q-y2 <= Epsilon) return 0;
                     graz = 1;
                     x1save = x1;
                 }

            }      /*   end of non vertical else block  */

        }        /*   end of loop through all sides of polygon or hole  */

/*      handle extreme case of first vertex grazing    */

        if (graz1) {
            if (y2 > q ) kount++;
        }

/*    are number of intersections odd or even    */

    kount = kount % 2;
    if ( kount == 0) return -1;
    return 1;

}    /*   ******** end of ply_point function  ************ */



/*

    ***************************************************************

                      p l y _ p o i n t 1

    ***************************************************************

  function name:  ply_point1  (integer)

  call sequence:  ply_point1 (xpoly,ypoly,numvert,p,q)

  synopsis:       Function to return the location of a point
                  relative to a single polygon.   All coordinates
                  are passed as CSW_Fs.  Use ply_point to pass
                  double precision coordinates.

  return values:  -1  point is outside polygon
                   0  point is on polygon boundary
                   1  point is inside polygon

  usage:          This function is a kernal function, in that it
                  does not call any other functions.  Higher level
                  functions are used to separate single polygons
                  from a group and for other preprocessing steps.
                  For maximum accuracy, shift the coordinates of
                  the polygon vertices and the target point to be
                  relative to a local origin prior to calling this
                  function.  This will make the line intersection
                  and grazing calculations more accurate.

  algorithm:      The algorithm involves calculation of intersections
                  between the polygon sides and a ray extending
                  vertically (+ y) from the target point.  If there
                  are an odd number of intersections, the target point
                  is inside the polygon.  For an even number of
                  intersections, the point is outside of the polygon.
                  If any of the intersection points is coincident with
                  the target point, the target point lies on the
                  boundary of the polygon.

  description of calling parameters:

    xpoly        r   CSW_F *   pointer to array of x coordinates
                               for the polygon vertices
    ypoly        r   CSW_F *   pointer to array of y coordinates
                               for the polygon vertices
    numvert      r   int       number of vertices in xpoly and ypoly
    p            r   CSW_F     x coordinate of target point
    q            r   CSW_F     y coordinate of target point

*/

int CSWPolyUtils::ply_point1 (
    CSW_F *xpoly, CSW_F *ypoly,
    int numvert,
    CSW_F p, CSW_F q)
{
    CSW_F      x1, y1, x2, y2;
    CSW_F      x1save = 1.e30f;
    CSW_F      y, zero=0.0f;
    int        i, kount=(int)zero, graz=(int)zero, graz1=0;
    int        i1, i2, i3, k, i2sav;
    CSW_F      hole;

/*  get polygon hole flag   */

    hole = (CSW_F)HoleFlag;

/*  loop through polygons and holes  */

    i3 = numvert;
    i1 = 0;
    k = 0;
    y2 = 1.e30f;

    while (k < i3) {

        while (k < i3) {
            if (xpoly[k] >= hole) {
                break;
            }
            k++;
        }

        i2 = k-1;
        i2sav = i2;

/*      If the polygon is closed, unclose it.    If this is not
        done, there is a tiny chance that, if the first point grazes,
        it will be counted more than once in the intersection counter
        and thus mess up the inside/outside rules */

        while (xpoly[i1] - xpoly[i2] == 0.0  &&
               ypoly[i1] - ypoly[i2] == 0.0) {
            i2--;
            if(i2 == i1) {
                if(i1 == 0) return -1;
                i1 = i2sav + 2;
                k = i1;
                break;
            }
        }

/*      make sure at least 3 points are left after opening polygon  */

        if(i2-i1 < 2) {
            i1 = i2sav + 2;
            k = i1;
            break;
        }

/*      Main loop of the function.  Loop through polygon or hole sides and
        check each side of the polygon for an intersection with a vertical
        ray extending upward from the target point
        If grazing (intersection with a vertex) occurs, make sure only one
        or in some cases zero, intersections are counted  */

        graz = 0;
        graz1 = 0;
        for (i=i1; i <= i2; i++) {
            x1 = xpoly[i];
            y1 = ypoly[i];
            if (ply_graze1 (x1, y1, p, q)) {
                return 0;
            }
            if(i == i2) {
                x2 = xpoly[i1];
                y2 = ypoly[i1];
            }
            else {
                x2 = xpoly[i+1];
                y2 = ypoly[i+1];
            }

/*         if the polygon side is vertical, it only intersects if the target
           point lies exactly on the polygon boundary  */

            if (x1 - x2 == 0.0) {
                if (ply_graze1(p, zero, x1, zero) && (y1-q)*(q-y2) >= zero) return 0;
            }

/*         the side is non vertical, check for intersection with the
           vertical ray from the target point    */

            else {

                if ((x1-p) * (p-x2) > zero) {
                    y = (y1 * (x2-p) + (p-x1) * y2) / (x2-x1);
                    if (ply_graze1(zero, y, zero, q)) return 0;
                    if (y > q) kount++;
                }

/*              check for grazing at the initial point on this side  */

                else if (p - x1 == 0.0) {
                    if (ply_graze1(zero, q, zero, y1)) return 0;
                    if (graz) {
                        graz = 0;
                        if ((x1save-p) * (p-x2) > zero && y1 > q) kount++ ;
                    }

                    else {
                        graz1 = 1;
                    }
                 }

/*               check for terminal point grazing  */

                 else if (p - x2 == 0.0) {
                     if (ply_graze1(zero, q, zero, y2)) return 0;
                     graz = 1;
                     x1save = x1;
                 }

            }      /*   end of non vertical else block  */

        }        /*   end of loop through all sides of polygon or hole  */

/*      handle extreme case of first vertex grazing    */

        if (graz1) {
            if (y2 > q ) kount++;
        }

        i1 = i2sav + 2;
        k = i1;

    }  /*  end of where loop through holes  */


/*    are number of intersections odd or even    */

    kount = kount % 2;
    if ( kount == 0) return -1;
    return 1;

}    /*   ******** end of ply_point  function  ************ */






/*

    ***************************************************************

                      p l y _ p o i n t p a

    ***************************************************************

  function name:    ply_pointpa    (integer)

  call sequence:    ply_pointpa (xv, yv, nc, ic, p, q)

  synopsis:         Check if a point is inside, outside or on the
                    edge of a polygon.  The polygon is passed as
                    arrays of pointers to components.

  return values:    inside/outside flag

                    -1 = point is outside polygon
                     0 = point is on the edge of the polygon
                     1 = point is inside of the polygon

  usage:             Called when the polygon components are organized
                     as arrays of pointers to individual components.  This
                     is the way the polygon boolean operations store polygon
                     components, so they use this function rather than direct
                     ply_point calls.

  algorithm:         The relationship of the point to the main component
                     (first component) is determined.  If the point is
                     outside the outside flag is returned.  If the point is
                     inside, the holes are checked.  If the point is inside any
                     holes, the outside flag is returned.

  description of calling parameters:

    xv      r    double*[]  array of pointers to component x coordinates
    yv      r    double*[]  array of pointers to component y coordinates
    nc      r    int        number of components
    ic      r    int     *  array with number of points in each component
    p       r    double     x coordinate of target point
    q       r    double     y coordinate of target point


*/

int CSWPolyUtils::ply_pointpa (
    double **xv, double **yv, int nc,
    int *ic, double p, double q)
{
    int           istat, i;

/*  check if the target point is inside the main component  */

    istat = ply_point (xv[0], yv[0], ic[0], p, q);

    if(istat <= 0  ||  nc <= 1) {
        return istat;
    }

/*  check if target point is inside any holes  */

    for (i=1; i<nc; i++) {
        istat = ply_point(xv[i], yv[i], ic[i], p, q);
        if(istat >= 0) {
            istat = -istat;
            return istat;
        }
    }

/*  point is inside the polygon  */

    return 1;

}




/*

    ***************************************************************

                      p l y _ p o i n t p a 1

    ***************************************************************

  function name:    ply_pointpa1    (integer)

  call sequence:    ply_pointpa1 (xv, yv, nc, ic, p, q)

  synopsis:         Check if a point is inside, outside or on the
                    edge of a polygon.  The polygon is passed as
                    arrays of pointers to components.  All coordinates
                    are passed as CSW_Fs.

  return values:    inside/outside flag

                    -1 = point is outside polygon
                     0 = point is on the edge of the polygon
                     1 = point is inside of the polygon

  usage:             Called when the polygon components are organized
                     as arrays of pointers to individual components.  This
                     is the way the polygon boolean operations store polygon
                     components, so they use this function rather than direct
                     ply_point calls.

  algorithm:         The relationship of the point to the main component
                     (first component) is determined.  If the point is
                     outside the outside flag is returned.  If the point is
                     inside, the holes are checked.  If the point is inside any
                     holes, the outside flag is returned.

  description of calling parameters:

    xv      r    CSW_F*[]   array of pointers to component x coordinates
    yv      r    CSW_F*[]   array of pointers to component y coordinates
    nc      r    int        number of components
    ic      r    int     *  array with number of points in each component
    p       r    CSW_F      x coordinate of target point
    q       r    CSW_F      y coordinate of target point


*/

int CSWPolyUtils::ply_pointpa1 (
    CSW_F **xv, CSW_F **yv, int nc,
    int *ic, CSW_F p, CSW_F q)
{
    int           istat, i;

/*  check if the target point is inside the main component  */

    istat = ply_point1 (xv[0], yv[0], ic[0], p, q);

    if(istat <= 0  ||  nc <= 1) {
        return istat;
    }

/*  check if target point is inside any holes  */

    for (i=1; i<nc; i++) {
        istat = ply_point1 (xv[i], yv[i], ic[i], p, q);
        if(istat >= 0) {
            istat = -istat;
            return istat;
        }
    }

/*  point is inside the polygon  */

    return 1;

}  /*  end of function ply_pointpa1  */




/*

    ***************************************************************

                      p l y _ p o i n t s

    ***************************************************************

  function name:    ply_points   (integer)

  call sequence:    ply_points(xpoly, ypoly, packpoly, numpoly,
                               xpts, ypts, npts,
                               inside)

  synopsis:         Report whether points are inside or outside a
                    set of polygons.

  return values:    Returns a status code.

                    0 = Successful execution
                    1 = Unable to allocate memory for work space.
                    2 = A polygon with less than 3 vertices was
                        passed to the function.
                    3 = Numpoly value passed is less than 1
                    4 = Resolution problem in the polygon limits.
                        This usually means that a single polygon
                        in a large set has absurdly large limits
                        compared to the rest of the polygons in
                        the set.  As such, the grids produced as
                        workspace cannot support both the very
                        large polygon and the rest of the polygons
                        which have normal limits.
                    5 = All of the coordinates in the polygon set
                        are equal, defining a zero area polygon set.
                        This would produce outside flags for all points
                        but I assume this condition is an error.

  usage:            The ply_points function is intended to be a high
                    level function called by the user or application.
                    The functions used by ply_points are all in the
                    polygon operations library or the map utilities
                    library.  Polygon arrays, point arrays and the
                    resulting inside array must be allocated by the
                    calling routine.  All work space is allocated
                    dynamically in ply_points.

  algorithm:        There are two algorithms present here to determine
                    which elements of a set of points are inside or
                    outside of a set of polygons.  For small numbers
                    of points the calculations are done directly by
                    multiple calls to ply_spoint.  For larger sets of
                    points, this can be very time consuming.  In this
                    case, a grid of inside/outside flags is built using
                    raster scanning techniques, and the in/out status
                    of the points is determined by the status of the
                    grid cell in which they lie.

  description of calling parameters:

    xpoly      r   double *   Pointer to array of polygon x coordinates.
    ypoly      r   double *   Pointer to array of polygon y coordinates.
    packpoly   r   int      * Pointer to array of number of vertices in
                              each individual polygon.
    numpoly    r   int        Number of individual polygons in the set.
                              The coordinates in xpoly and ypoly must be
                              in the order defined by packpoly.
                              I.E., if packpoly [1] = 25 and packpoly [2]
                              = 15, xpoly and ypoly [0...24] will have
                              the coordinates for the first polygon, and
                              [25...39] will have the coordinates of the
                              second polygon.
    xpts       r   double *   Pointer to array of x coordinates of points
                              to test.
    ypts       r   double *   Pointer to y coordinates of points.
    npts       r   int        Number of points in xpts and ypts.
    inside     w   int *      Pointer to array of flags for whether the
                              corresponding point is inside, outside or
                              on the boundary of the set of polygons.
                              If inside [i] = -1, then xpts[i],ypts[i] is
                              outside.  If inside = 0, the point is on the
                              boundary.  If inside = 1, the point is inside
                              the set of polygons.
*/

int CSWPolyUtils::ply_points(
    double *xpoly, double *ypoly, int *packpoly, int numpoly,
    double *xpts, double *ypts, int npts,
    int *inside)
{
    double     *xpmin = NULL, *ypmin = NULL, *xpmax = NULL, *ypmax = NULL;
    double     xgmin, ygmin, xgmax, ygmax;
    double     xspace, yspace;
    int        ncol, nrow, nd1, nd2;
    char       *hgrid = NULL, *vgrid = NULL, *iogrid = NULL,
               *index = NULL, *indexb = NULL, *bcell = NULL;
    int        i, j, k, idirt;
    double     dirt, dirt2, avsize;
    int        outer;
    double     flagd;

    auto fscope = [&]()
    {
        csw_Free(xpmin);
        csw_Free(hgrid);

    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  allocate memory for individual polygon limits  */

    xpmin = (double *)csw_Calloc((numpoly + 1) * 4 * sizeof(double));
    if(!xpmin) return 1;

    ypmin = xpmin + numpoly;
    xpmax = ypmin + numpoly;
    ypmax = xpmax + numpoly;

/*  calculate polygon limits  */

    idirt = ply_limits(xpoly, ypoly, packpoly, numpoly,
                       xpmin, ypmin, xpmax, ypmax,
                       &xgmin, &ygmin, &xgmax, &ygmax);
    if(idirt) {
        return 2;
    }

/*  set up underlying grid geometry for polygon representation  */

    idirt = ply_grid_set(xpmin, ypmin, xpmax, ypmax, numpoly,
                         xgmin, ygmin, xgmax, ygmax,
                         &xspace, &yspace);

    if(idirt) {
        if(idirt == 2) return 1;
        if(idirt == 1) return 3;
        if(idirt == 3) return 4;
        return 5;
    }

/*  calculate number of rows and columns, then expand limits
    so that a margin of at least one grid cell exists around the
    polygon set  */

    ncol = (int)((xgmax-xgmin) / xspace) + 5;
    nrow = (int)((ygmax-ygmin) / yspace) + 5;

    dirt = ((xspace * ncol) - (xgmax-xgmin)) / 2.0f;
    xgmin -= dirt;
    xgmax += dirt;

    dirt = ((yspace * nrow) - (ygmax-ygmin)) / 2.0f;
    ygmin -= dirt;
    ygmax += dirt;

    ncol += 1;
    nrow += 1;

/*  check if direct calculation is probably more efficient then
    raster scan techniques.  If the average number of nodes per
    polygon scan is high and the product of the number of target
    points * number of polygon sides is low, the direct calculations
    are nearly as fast or faster than the raster scan method  */

/*  calculate the average number of nodes to scan for each polygon  */

    dirt = 0.0f;
    for (i=0; i < numpoly; i++) {
        dirt += ((xpmax[i]-xpmin[i]) * (ypmax[i]-ypmin[i]));
    }
    avsize = dirt / numpoly;
    avsize = avsize / (xspace * yspace);

/*  calculate total number of polygon sides  */

    dirt = 0.0f;
    for (i=0; i < numpoly; i++) {
        dirt += packpoly[i];
    }

/*  scale the number of polygon sides by a factor estimating how
    many polygons will actually be calculated the hard way (if the
    point is outside the limits of a polygon it does not need to
    be checked the hard way)  */

    dirt2 = 2.0f * avsize / ((xgmax-xgmin) * (ygmax-ygmin));

/*  if probably more efficient the direct way, do the calculations
    using ply_point, csw_Free memory and return  */

/*  check scanflag parameter to see if direct calculation is
    forced or inhibited  */

    flagd = ScanFlag;

    if(flagd <= 0) {
        if(avsize * numpoly > dirt * dirt2 * npts  ||  flagd < 0) {
            for (i=0; i < npts; i++) {
                inside[i] = ply_spoint(xpoly, ypoly, packpoly, numpoly,
                                       xpts[i], ypts[i],
                                       xpmin, ypmin, xpmax, ypmax);
            }
            return 0;
        }
    }

/*  the scanning method is probably more efficient.  The rest of the
    function executes the scanning techniques for points in polygons
    calculations  */

/*  allocate memory for grids  */

    nd1 = ncol+5;
    nd2 = nrow+5;
    idirt = (nd1 * nd2);
    idirt = (idirt/4+1) * 4;
    hgrid = (char *)csw_Malloc(idirt*4);
    if(!hgrid) {
        return 1;
    }

    vgrid = hgrid +idirt;
    iogrid = vgrid + idirt;
    bcell = iogrid + idirt;

/*  scan hgrid and vgrid.  assign in/out flags at each node  */

    idirt = ply_scan(hgrid, vgrid, iogrid, bcell,
                     nd1, nd2, ncol, nrow,
                     xpoly, ypoly, packpoly, numpoly,
                     xgmin, ygmin, xspace, yspace,
                     xpmin, ypmin, xpmax, ypmax);

/*  check each target point by locating the grid cell it is in.
    If 3 or 4 corners of the grid cell are 0 (adjacent) use the
    ply_spoint function.  Otherwise, the target point has the
    same status as the non zero corners of the grid cell      */

    for (k=0;  k < npts;  k++) {
        j = (int)((xpts[k]-xgmin) / xspace);
        i = (int)((ypts[k]-ygmin) / yspace);
        if(j < 0  ||  j >= ncol  ||  i < 0  ||  i >= nrow) {
            inside[k] = -1;
            continue;
        }
        outer = i * nd1 + j ;
        index = iogrid + outer;
        indexb = bcell + outer;
        if (*indexb == 1) {
            inside[k] = ply_spoint(xpoly, ypoly, packpoly, numpoly,
                                   xpts[k], ypts[k],
                                   xpmin, ypmin, xpmax, ypmax);
            continue;
        }

        idirt = *(index) + *(index+1) + *(index+nd1) + *(index+nd1+1);

        if(idirt > 1) {
            inside[k] = 1;
        }
        else if(idirt < -1) {
            inside[k] = -1;
        }
        else {
            inside[k] = ply_spoint(xpoly, ypoly, packpoly, numpoly,
                                   xpts[k], ypts[k],
                                   xpmin, ypmin, xpmax, ypmax);
        }

    }

/*  csw_Free allocated memory and return  */

    return 0;

}   /*  end of ply_points function  */





/*

    ***************************************************************

                         p l y _ s p o i n t

    ***************************************************************

  function name:    ply_spoint   (integer)

  call sequence:    ply_spoint (xpoly, ypoly, polypack, npoly,
                                p, q,
                                xpmin, ypmin, xpmax, ypmax)

  synopsis:         This function decides whether a point is inside,
                    outside, or on the boundary of a set of polygons.

  return values:    -1  point is outside of polygon set
                     0  point is on a polygon boundary
                     1  point is inside of one of the polygons
                        in the set

  usage:            This function is called from the multiple points
                    inside polygon routines to establish the inside/
                    outside criteria for initial points and points
                    near polygon boundaries.  For highest accuracy,
                    the polygon points and target points should be
                    translated to local coordinates prior to calling
                    this routine.

  algorithm:        This routine unpacks the set of polygons and
                    uses point_in_poly to establish inside/outside
                    for each polygon in the set.  If the target point
                    is inside any single polygon, the point is inside
                    the set of polygons.  If the target lies on any
                    single polygon boundary, the point is on the
                    boundary of the set of polygons.  The target point
                    must be outside of all individual polygons in the
                    set to be outside of the set.  The individual
                    polygon limits are used to skip polygons in the
                    set where the target point is outside of the limits.

  description of calling parameters:

    xpoly      r   double *    pointer to array of packed x polygon
                               vertex coordinates
    ypoly      r   double *    pointer to array of packed y polygon
                               vertex coordinates
    polypack   r   int      *  pointer to array with numbers of vertices
                               in each individual polygon of the set.
                               i.e.  if *polypack =5 and *(polypack+1) =10
                               locations 0-4 of xpoly and ypoly have the
                               vertices for the first polygon and locations
                               5-14 have the vertices of the second
                               polygon.
    npoly      r   int         number of individual polygons packed
    p          r   double      x coordinate of target point
    q          r   double      y coordinate of target point
    xpmin      r   double *    pointer to array of minimum x coordinates
                               for individual polygons.
    ypmin      r   double *    pointer to array of minimum y coordinates
                               for individual polygons.
    xpmax      r   double *    pointer to array of maximum x coordinates
                               for individual polygons
    ypmax      r   double *    pointer to array of maximum y coordinates
                               for individual polygons

*/

int CSWPolyUtils::ply_spoint (
    double *xpoly, double *ypoly,
    int *polypack, int npoly,
    double p, double q,
    double *xpmin, double *ypmin, double *xpmax, double *ypmax)
{
    int        index = -1;
    int        i = 0, first = 0, temp;
    int        iflag = 1;

/*   check some obvious parameter errors  */

    if (npoly < 1) return -1;

/*   make sure xpmin pointer is not null.  if it is null, do not
     use the polygon limit checks below  */

    if(!xpmin) iflag = 0;

/*  loop through all of the individual polygons.  If the target point
    is inside any individual polygon or on its boundary, return the
    appropriate condition  */

  for (i=0; i<npoly; i++) {

      temp = polypack[i];

/*  check against limits of this polygon first */

      if(iflag  &&  xpmin != NULL) {
          if(p >= xpmin[i]  &&  p <= xpmax[i]  &&
             q >= ypmin[i]  &&  q <= ypmax[i])  {
                if( temp > 2) {
                    ply_reorg(xpoly+first, ypoly+first, temp,
                              xpmin[i], ypmin[i], '-');
                    p -= xpmin[i];
                    q -= ypmin[i];
                    index = ply_point(xpoly+first, ypoly+first,
                                      temp,
                                      p, q);
                    p += xpmin[i];
                    q += ypmin[i];
                    ply_reorg(xpoly+first, ypoly+first, temp,
                              xpmin[i], ypmin[i], '+');
                    if(index >= 0) return index;
                }
           }
       }

/*  force call to ply_point since limits are not available  */

       else {
           if( temp > 2) {
               index = ply_point(xpoly+first, ypoly+first,
                                 temp,
                                 p, q);
               if(index >= 0) return index;
           }
       }
       first += temp;
  }

/*       outside entire set of polygons      */

  return -1;

}     /*   end of ply_spoint function  */



/*
  ****************************************************************

               p l y _ P o l y I n s i d e P o l y 1

  ****************************************************************

  function name:     ply_PolyInsidePoly1        (int)

  call sequence:     ply_PolyInsidePoly1 (x1, y1, icomp1, ncomp1, graze,
                                          x2, y2, npts)

  purpose:           Determine if the polygon component designated by
                     x2, y2 is inside or outside of the x1,y1 test polygon.
                     This function assumes that the two polygons do not
                     intersect, but they may have common edges.

  return value:      in/out flag

                     -2 = memory allocation error
                     -1 = component is outside of polygon
                      0 = all component points lie on the edge
                          of the test polygon
                      1 = component is inside the test polygon.

  calling parameters:

    x1        r    CSW_F*    x coordinates of test polygon
    y1        r    CSW_F*    y coordinates of test polygon
    icomp1    r    int*      array with points per hole in test polygon
    ncomp1    r    int       number of holes in test polygon
    graze     r    CSW_F     grazing distance for determining if
                             a point is "on" the test polygon edge
    x2        r    CSW_F*    x coordinates of component
    y2        r    CSW_F*    y coordinates of component
    npts      r    int       number of points in component

*/

int CSWPolyUtils::ply_PolyInsidePoly1 (
    CSW_F *x1, CSW_F *y1, int *icomp1, int ncomp1,
    CSW_F graze, CSW_F *x2, CSW_F *y2, int npts)
{
    int           i, istat, n1, n1sav;
    double        dgraz;
    double        *xv = NULL, *yv = NULL, *xv2 = NULL, *yv2 = NULL;

    auto fscope = [&]()
    {
        csw_Free (xv);
        csw_Free (xv2);
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  set grazing value  */

    dgraz = (double)graze;

/*  allocate memory for double coordinate arrays  */

    n1 = 0;
    for (i=0; i<ncomp1; i++) {
        n1 += icomp1[i];
    }
    n1sav = n1;
    n1 += 10;

    xv = (double *)csw_Malloc (n1 * 2 * sizeof(double));
    if (!xv) {
        return -2;
    }
    yv = xv + n1;
    xv2 = (double *)csw_Malloc ((npts+10) * 2 * sizeof(double));
    if (!xv2) {
        return -2;
    }
    yv2 = xv2 + npts;

/*  convert input points to double arrays  */

    for (i=0; i<n1sav; i++) {
        xv[i] = x1[i];
        yv[i] = y1[i];
    }
    for (i=0; i<npts; i++) {
        xv2[i] = x2[i];
        yv2[i] = y2[i];
    }

/*  call the double version of the function  */

    istat = ply_PolyInsidePoly2 (xv, yv, icomp1, ncomp1, dgraz,
                                 xv2, yv2, npts);

    return istat;

}  /*  end of function ply_PolyInsidePoly1  */




/*
  ****************************************************************

               p l y _ P o l y I n s i d e P o l y 2

  ****************************************************************

  function name:     ply_PolyInsidePoly2        (int)

  call sequence:     ply_PolyInsidePoly1 (x1, y1, icomp1, ncomp1, graze,
                                          x2, y2, npts)

  purpose:           Determine if the polygon component designated by
                     x2, y2 is inside or outside of the x1,y1 test polygon.
                     This function assumes that the two polygons do not
                     intersect, but they may have common edges.  All coordinates
                     passed must be double precision.

  return value:      in/out flag

                     -2 = memory allocation error
                     -1 = component is outside of polygon
                      0 = all component points lie on the edge
                          of the test polygon
                      1 = component is inside the test polygon.

  calling parameters:

    x1        r    double*   x coordinates of test polygon
    y1        r    double*   y coordinates of test polygon
    icomp1    r    int*      array with points per hole in test polygon
    ncomp1    r    int       number of holes in test polygon
    graze     r    double    grazing distance for determining if
                             a point is "on" the test polygon edge
    x2        r    double*   x coordinates of component
    y2        r    double*   y coordinates of component
    npts      r    int       number of points in component

*/

int CSWPolyUtils::ply_PolyInsidePoly2 (
    double *x1, double *y1, int *icomp1, int ncomp1,
    double graze, double *x2, double *y2, int npts)
{
    int           i, j, istat, n1;
    double        dgraz;
    double        **xv = NULL, **yv = NULL;
    double        xt, yt, xt1, yt1, xt2, yt2, dx, dy, eps,
                  ang, xanc, yanc, pi2;
    int           icw, iccw, itest, i1, i2;

    auto fscope = [&]()
    {
        csw_Free (xv);
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  set grazing value  */

    dgraz = (double)graze;
    ply_setgraze (dgraz);

    eps = graze * 2.f;
    pi2 = 1.5707963f;

/*  initialize some variables  */

    xt1 = 1.e30f;
    yt1 = 1.e30f;
    xt2 = 1.e30f;
    yt2 = 1.e30f;
    itest = 0;
    icw = 0;

/*  set up arrays of pointers  */

    j = ncomp1 + 2;
    xv = (double **)csw_Malloc (j * 2 * sizeof(double *));
    if (!xv) {
        return -2;
    }
    yv = xv + j;

    n1 = 0;
    for (i=0; i<ncomp1; i++) {
        xv[i] = x1 + n1;
        yv[i] = y1 + n1;
        n1 += icomp1[i];
    }

/*
    find a point on the component that is not on an edge
    of the test polygon and return the points in/out status
*/

    for (i=0; i<npts; i++) {
        istat = ply_pointpa (xv, yv, ncomp1, icomp1,
                             x2[i], y2[i]);
        if (istat) {
            return istat;
        }
    }

/*
    all vertices are on the test polygon edge,
    check the mid points of each side
*/

    for (i=0; i<npts; i++) {
        if (i < npts-1) {
            xt = (x2[i] + x2[i+1]) / 2.f;
            yt = (y2[i] + y2[i+1]) / 2.f;
        }
        else {
            xt = (x2[i] + x2[0]) / 2.f;
            yt = (y2[i] + y2[0]) / 2.f;
        }
        istat = ply_pointpa (xv, yv, ncomp1, icomp1,
                             xt, yt);
        if (istat) {
            return istat;
        }
    }

/*
    all midpoints are also on the test polygon,
    find a point inside the component and check
    if it is inside or outside the test polygon
*/

    for (i = 0; i < npts; i++) {

        i1 = i;
        i2 = i+1;
        if(i2 >= npts) i2 = 0;
        dx = x2[i2] - x2[i1];
        dy = y2[i2] - y2[i1];

/*      if points are coincident, check the next pair  */

        if(dx == 0  &&  dy == 0) continue;

/*      calculate points for inside/outside checks  */

        ang = atan2(dy, dx);
        xanc = (x2[i1] + x2[i2]) / 2.0f;
        yanc = (y2[i1] + y2[i2]) / 2.0f;

        xt1 = xanc + eps * cos(ang - pi2);
        yt1 = yanc + eps * sin(ang - pi2);
        xt2 = xanc + eps * cos(ang + pi2);
        yt2 = yanc + eps * sin(ang + pi2);

/*      check if clockwise point is inside  */

        icw = ply_point (x2, y2, npts, xt1, yt1);
        if(icw == 0) continue;

/*      check if counter clockwise point is inside  */

        iccw = ply_point (x2, y2, npts, xt2, yt2);
        if(iccw == 0) continue;

/*      if exactly one point is inside, the solution is good  */

        itest = icw * iccw;
        if(itest < 0) break;

/*      check next side  */

    }

/*
    if a point has been found that is inside the component,
    check if it is also inside the test polygon
*/

    if (itest >= 0) {
        return 0;
    }

    if (icw > 0) {
        xt = xt1;
        yt = yt1;
    }
    else {
        xt = xt2;
        yt = yt2;
    }

    istat = ply_pointpa (xv, yv, ncomp1, icomp1,
                         xt, yt);
    return istat;

}  /*  end of function ply_PolyInsidePoly2  */



/*
  ****************************************************************

              p l y _ p o i n t o n l i n e s e g

  ****************************************************************

    Determine if point x3,y3 is within grazing distance of line
  segment x1,y1 x2,y2.  Return 1 if x3,y3 is on the line segment
  Return zero if x3,y3 is not on the line segment.

*/

int CSWPolyUtils::ply_pointonlineseg (
    double x1, double y1, double x2, double y2,
    double x3, double y3)
{
    double            xt, yt;
    int               istat;

/*  case where x3,y3 grazes an end point  */

    if (ply_graze (x1, y1, x3, y3)) return 1;
    if (ply_graze (x2, y2, x3, y3)) return 1;

/*  vertical line segment case  */

    if (ply_graze ((double)0.0, x1, (double)0.0, x2)) {
        if (!ply_graze ((double)0.0, x1, (double)0.0, x3)) {
            return 0;
        }
        if ((y1-y3)*(y3-y2) >= 0.0f) {
            return 1;
        }
        else {
            return 0;
        }
    }

/*  horizontal line segment case  */

    if (ply_graze ((double)0.0, y1, (double)0.0, y2)) {
        if (!ply_graze ((double)0.0, y1, (double)0.0, y3)) {
            return 0;
        }
        if ((x1-x3)*(x3-x2) >= 0.0f) {
            return 1;
        }
        else {
            return 0;
        }
    }

/*
    reject if x3,y3 is outside the bounds of the line segment
*/

    if ((x1-x3)*(x3-x2) < 0.0f) {
        return 0;
    }
    if ((y1-y3)*(y3-y2) < 0.0f) {
        return 0;
    }

/*
    general case, find point on segment that perpendicular intersects
    the segment and goes through x3, y3.  If this point (xt,yt) is
    within grazing distance of x3,y3, then x3,y3 is effectively on
    the line segment.
*/

    istat = gpf_perpintpoint2 (x1, y1, x2, y2, x3, y3, &xt, &yt);
    if (!istat) {
        return 0;
    }
    if (ply_graze (x3, y3, xt, yt)) {
        return 1;
    }
    else {
        return 0;
    }

}  /*  end of function ply_pointonlinesegment  */



/*
  ****************************************************************

               p l y _ C a l c S m a l l O f f s e t

  ****************************************************************

    Calculate a point on the line between x1,y1 and x2,y2 that is
    twice the grazing distance from x1,y1 in the direction of x2,y2.

*/

int CSWPolyUtils::ply_CalcSmallOffset (
    double x1, double y1, double x2, double y2,
    double *x3, double *y3)
{
    double         ang, eps, dx, dy, d2;

    ply_getgraze (&eps);
    d2 = eps * 10.f;
    eps /= 10.f;
    dx = x2 - x1;
    dy = y2 - y1;

    if (dx > -eps  &&  dx < eps  &&  dy > -eps  &&  dy < eps) {
        *x3 = (x1 + x2) / 2.f;
        *y3 = (y1 + y2) / 2.f;
        return 1;
    }

    ang = atan2 (dy,dx);
    *x3 = x1 + d2 * cos (ang);
    *y3 = y1 + d2 * sin (ang);

    return 1;

}  /*  end of function ply_CalcSmallOffset  */


/*
  ****************************************************************

               p l y _ e x t e n d l i n e s e g

  ****************************************************************

    extend the line segment the grazing distance past the second
    endpoint and modify the second endpoint to the new position

*/

int CSWPolyUtils::ply_extendlineseg (
    double x1, double y1, double *x2, double *y2)
{
    double         ang, eps, dx, dy;

    ply_getgraze (&eps);

    dx = *x2 - x1;
    dy = *y2 - y1;

    ang = atan2 (dy,dx);
    *x2 +=  eps * cos (ang);
    *y2 +=  eps * sin (ang);

    return 1;

}  /*  end of function ply_extendlineseg  */




/*
  ****************************************************************

               p l y _ C a l c L e f t O f f s e t

  ****************************************************************

    Calculate a point on the line between x1,y1 and x2,y2 that is
    twice the grazing distance from x1,y1 in the direction of x2,y2.
    Then travel perpendicular from that point to the left (ccw) an
    equal distance to get the final point.

*/

int CSWPolyUtils::ply_CalcLeftOffset (
    int mult,
    double x1, double y1, double x2, double y2,
    double *x3, double *y3)
{
    double         ang, eps, dx, dy, d2, d3, cosang, sinang;

    if (mult < 2) mult = 2;

    ply_getgraze (&eps);
    d2 = eps * 2.f;
    d3 = eps * (double)mult;
    eps /= 10.f;
    dx = x2 - x1;
    dy = y2 - y1;

    if (dx > -eps  &&  dx < eps  &&  dy > -eps  &&  dy < eps) {
        *x3 = (x1 + x2) / 2.f;
        *y3 = (y1 + y2) / 2.f;
        return 1;
    }

    ang = atan2 (dy,dx);
    cosang = cos (ang);
    sinang = sin (ang);
    *x3 = x1 + d3 * cosang - d2 * sinang;
    *y3 = y1 + d3 * sinang + d2 * cosang;

    return 1;

}  /*  end of function ply_CalcLeftOffset  */


/*
  ****************************************************************

              p l y _ t h i n o u t d u p s

  ****************************************************************

    Eliminate adjacent points that are nearly coincident from a
    polygon component.

*/

int CSWPolyUtils::ply_thinoutdups (
    double *x, double *y, int *npts)
{
    int               i, n;
    double            dx, dy;

    if (*npts < 1) {
        return 1;
    }

    n = 0;
    for (i=0; i<*npts-1; i++) {
        dx = x[i] - x[i+1];
        dy = y[i] - y[i+1];
        if (dx < 0.0f) dx = -dx;
        if (dy < 0.0f) dy = -dy;
        if (dx > Epsilon  ||  dy > Epsilon) {
            x[n] = x[i];
            y[n] = y[i];
            n++;
        }
    }
    x[n] = x[*npts-1];
    y[n] = y[*npts-1];
    n++;

    *npts = n;

    return 1;

}  /*  end of function ply_thinoutdups  */


/*
  ****************************************************************

                 p l y _ c h e c k e d g e

  ****************************************************************

    Double check that a point is actually on the edge of the
  polygon.  The segment is checked for intersection and either
  the midpoint or second endpoint is checked for in/out/edge.

*/

int CSWPolyUtils::ply_checkedge (
    double x1, double y1, double x2, double y2,
    double **xp, double **yp, int np, int *ip)
{
    int            istat, nh, npt;
    double         xt, yt;

    CSWPolyTraverse  ply_traverse_obj;

    istat = ply_traverse_obj.ply_sidint (xp, yp, np, ip,
                        x1, y1, x2, y2,
                        &xt, &yt, &nh, &npt);

    if (istat == 0  ||  istat == 3) {
        xt = (xt + x1) / 2.f;
        yt = (yt + y1) / 2.f;
    }
    else {
        xt = x2;
        yt = y2;
    }

    istat = ply_pointpa (xp, yp, np, ip, xt, yt);

    return istat;

}  /*  end of function ply_checkedge  */


/*
  ****************************************************************

                   p l y _ P l i n e I n O u t

  ****************************************************************

    Check if a polyline is inside or outside of a polygon.  This
  is used by the line clip to polygon function only.

*/

int CSWPolyUtils::ply_PlineInOut (
    double *xp1, double *yp1,
    int np1, int *ip1, int *jp1,
    double *x1, double *y1, int n1)
{
    double        **xp = NULL, **yp = NULL;
    int           ido, j, n, nt, n0;
    int           i, istat;
    double        xt, yt;

    auto fscope = [&]()
    {
        csw_Free (xp);
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  if too few points, return outside  */

    if (n1 < 2) {
        return -1;
    }

/*
    If the end points of a single vector are very close, return outside.
*/
    if (np1 == 2) {
        if (ply_graze (xp1[0], yp1[0], xp1[1], yp1[1])) {
            return -1;
        }
    }

/*
    allocate space for hole pointers
*/

    n = 0;
    for (i=0; i<np1; i++) {
        for (j=0; j<ip1[i]; j++) {
            n++;
        }
    }
    n += 2;

    xp = (double **)csw_Malloc (n * 2 * sizeof(double *));
    if (!xp) {
        return -1;
    }
    yp = xp + n;

/*
    loop through each part of the multiple polygon and
    if the line is inside any part return as inside
    If the loop completes without any lines inside
    then return as outside
*/

    n = 0;
    nt = 0;

    for (ido=0; ido<np1; ido++) {

/*
        setup holes for the component
*/

        n0 = n;
        for (j=0; j<ip1[ido]; j++) {
            xp[j] = xp1 + nt;
            yp[j] = yp1 + nt;
            nt += jp1[n];
            n++;
        }

/*      check each endpoint for in/out  */

        for (i=1; i<n1-1; i++) {
            istat = ply_pointpa (xp, yp, ip1[ido], &jp1[n0], x1[i], y1[i]);
            if (istat) {
                if (istat == 1  ||  np1 == 1) {
                    return istat;
                }
            }
        }

/*      if all endpoints were on boundary, check mid points of vectors  */

        for (i=0; i<n1-1; i++) {
            xt = (x1[i] + x1[i+1]) / 2.f;
            yt = (y1[i] + y1[i+1]) / 2.f;
            istat = ply_pointpa (xp, yp, ip1[ido], &jp1[n0], xt, yt);
            if (istat) {
                if (istat == 1  ||  np1 == 1) {
                    return istat;
                }
            }
        }

        if (n1 > 2) continue;

/*      if a single vector, check each .1 of vector length  */

/*
        for (i=0; i<11; i++) {
            xt = x1[0]*.1f*i + x1[1]*(1.f-.1f*i);
            yt = y1[0]*.1f*i + y1[1]*(1.f-.1f*i);
            istat = ply_pointpa (xp, yp, ip1[ido], &jp1[n0], xt, yt);
            if (istat) {
                if (istat == 1  ||  np1 == 1) {
                    return istat;
                }
            }
        }
*/

    }

    return -1;

}  /*  end of function ply_PlineInOut  */


int CSWPolyUtils::ply_UtilMaxPts (int val)
{
    MaxPts = val;
    return 1;
}






/*
  ****************************************************************

              C o r r e c t F o r S m a l l H o l e s

  ****************************************************************

    If a hole lies entirely inside of a grid cell when a polygon
  is scanned, set the 4 corners of the grid cell to zero flagging
  them as adjacent to the polygon and set the border crossing
  array to 1.

*/

int CSWPolyUtils::CorrectForSmallHoles (
    double *xmin, double *ymin, double *xmax, double *ymax,
    int ncomp,
    double xgmin, double ygmin, double xspace, double yspace,
    int nd1, int ncol, int nrow,
    char *iogrid, char *bcell)
{
    int            i, outer, idirt, jmin, jmax, imin, imax;

/*
    loop through each component and check if the component
    is entirely inside of a grid cell
*/

    for (i=0; i<ncomp; i++) {

        jmin = (int)((xmin[i]-xgmin) / xspace);
        jmax = (int)((xmax[i]-xgmin) / xspace) + 1;
        imin = (int)((ymin[i]-ygmin) / yspace) ;
        imax = (int)((ymax[i]-ygmin) / yspace) + 1;

        if(jmin < 0)    jmin = 0;
        if(jmax > ncol-1) jmax = ncol-1;
        if(imin < 0)    imin = 0;
        if(imax > nrow-1) imax = nrow-1;

        if (jmax-jmin <= 1  &&  imax-imin <= 1) {
            outer = imin * nd1;
            idirt = outer + jmin;
            *(iogrid+idirt) = 0;
            *(iogrid+idirt+1) = 0;
            *(iogrid+idirt+nd1) = 0;
            *(iogrid+idirt+nd1+1) = 0;
            *(bcell+idirt) = 1;
            return 0;
        }
    }

    return 1;

}  /*  end of CorrectForSmallHoles function  */



/*

    ***************************************************************

                p l y _ h o l n e s t x y z

    ***************************************************************

  function name:    ply_holnest   (integer)

  call sequence:    ply_holnest (xpoly, ypoly, zpoly, ncomp, icomp,
                                 xpout, ypout, zpoly, npout, ncout, icout)

  synopsis:         Separate multiple nesting levels of polygon holes.

  return values:    status code

                    -1 = could not allocate memory for workspace
                     0 = successful completion
                     1 = could not apend hole to output polygon set
                         this is probably because not enough memory
                         was allocated for the output polygon
                     2 = No unnesting was needed.  The input polygon
                         should be used as is.  No data are written
                         to the output polygon in this case.
                     3 = No component could be found that is not inside
                         another component.  This is either because two
                         components of the polygon are identical or
                         because of a program bug.

  usage:            This function is an integral part of the polygon
                    boolean operations functions.  It is not designed
                    or documented to be used outside of this context.
                    The polygon operations use this function to insure
                    that the intersection, union etc functions are only
                    dealing with holes at a single nesting level.

  algorithm:        First, a component not inside any other component is
                    found and used as the root of a polygon.  Next, any
                    components inside the root, but not inside any other
                    components, are output as holes of the root polygon.
                    The root and its holes are removed from the processing
                    list, and the process is repeated until all components
                    have been accounted for.  This will not work for self
                    intersecting polygons.

  description of calling parameters:

    xpoly      r    double*    array of input x coordinates
    ypoly      r    double*    array of input y coordinates
    zpoly      r    double*    array of input z coordinates
    ncomp      r    int        number of components in input polygon
    icomp      r    int     *  array with number of points in each
                               input polygon component
    xpout      w    double*    array of output polygon x coordinates
    ypout      w    double*    array of output polygon y coordinates
    zpout      w    double*    array of output polygon z coordinates
    npout      w    int     *  number of polygons in output
    ncout      w    int     *  array with number of components for
                               each output polygon
    icout      w    int     *  array with number of points in each
                               component of each output polygon
    maxpts     r    int        maximum number of output points
    maxcomp    r    int        maximum number of output components
*/

int CSWPolyUtils::ply_holnestxyz (
    double *xpoly, double *ypoly, double *zpoly,
    int ncomp, int *icomp,
    double *xpout, double *ypout, double *zpout,
    int *npout, int *ncout, int *icout,
    int maxpts, int maxcomp)
{
    HList        *holist = NULL, *holorg = NULL, *holsav = NULL, *holtmp = NULL;
    PList        *plylist = NULL;
    int          i, j, offset, istat, ioff, joff, koff,
                 numout, nstack, rootoff, rootnps, iroot, nstacksav;
    int          nctemp, nout, npts, inps, jnps;
    int          nptemp, nclast;

    auto fscope = [&]()
    {
        csw_Free (holist);
        csw_Free (plylist);
        MaxPout = 0;
        MaxHout = 0;
    };
    CSWScopeGuard  func_scope_guard (fscope);

/*  if only one component, return a flag of 2  */

    if (ncomp == 1) {
        *npout = 1;
        ncout[0] = 1;
        icout[0] = icomp[0];
        memcpy (xpout, xpoly, icomp[0] * sizeof(double));
        memcpy (ypout, ypoly, icomp[0] * sizeof(double));
        memcpy (zpout, zpoly, icomp[0] * sizeof(double));
        return 0;
    }


/*  set some stuff to make compiler warnings go away  */

    iroot = 0;
    istat = 0;
    rootoff = 0;
    rootnps = 0;
    npts = 0;

/*  allocate memory for workspace  */

    holist = (HList *) csw_Malloc(ncomp * sizeof(HList));
    if(!holist) {
        return -1;
    }
    holorg = holist;

    plylist = (PList *) csw_Malloc(ncomp * sizeof(PList));
    if(!plylist) {
        return -1;
    }

    MaxPout = maxpts;
    MaxHout = maxcomp;

/*  build local structure array in holist which contains offset, size
    and flag for each polygon component  */

    offset = 0;
    for (i=0; i<ncomp; i++) {

        holist->off = offset;
        holist->flg = 0;
        holist->nps = icomp[i];
        offset += icomp[i];
        holist++;

    }

    nstack = ncomp;
    holist = holorg;


/*  unnest levels of holes by finding components of the polygon which are
    not inside any other components, and then finding all of the holes
    inside these components which are also not inside any other component
    This produces a set of polygons with only one level of hole nesting
    per polygon  */


    nctemp = 0;
    numout = 0;
    nout = 0;
    nptemp = 0;
    nclast = 0;

    while (nstack > 0) {

        holist = holorg;

        for (i=0; i<nstack; i++) {
            holist->flg = 0;
        }

/*      find a component not inside any other component  */

        for (i=0; i<nstack; i++) {

            ioff = holist->off;
            npts = holist->nps;
            holsav = holist;
            holist = holorg;

            for (j=0; j<nstack; j++) {

                if(j != i) {
                    joff = holist->off;
                    inps = holist->nps;
                    istat = 0;
                    koff = 0;
                    while (istat == 0  &&  koff < npts) {
                        istat = ply_point(xpoly+joff, ypoly+joff, inps,
                                          *(xpoly+ioff+koff),
                                          *(ypoly+ioff+koff));
                        koff++;
                    }
                    if(istat >= 0) {
                        break;
                    }

                    holist++;
                }
                else {
                    holist++;
                }
            }

            if(istat < 0  ||  nstack == 1) {
                rootoff = ioff;
                rootnps = npts;
                iroot = i;
                holsav->flg = -1;
                istat = -1;
                break;
            }

            holist = holsav;
            holist++;

        }

/*      istat should not be gt or equal to 0 unless all components
        are identical or there is a bug in the code  */

        if(istat >= 0) {
            return 4;
        }

/*      add main component (root polygon) to the output  */

        if (nptemp > 0) {
            ncout[nptemp-1] = nctemp - nclast;
        }
        nclast = nctemp;
        icout[nctemp] = npts;
        nctemp++;
        nptemp++;
        plylist->ply = numout;
        plylist->hol = 0;
        plylist++;
        ioff = rootoff;

        for (i=0; i<npts; i++) {
            *(xpout+nout) = *(xpoly+ioff);
            *(ypout+nout) = *(ypoly+ioff);
            *(zpout+nout) = *(zpoly+ioff);
            ioff++;
            nout++;
            if (nout >= MaxPts) {
                holist = holorg;
            }
        }

/*      find holes inside the root and outside everything else  */

        holist = holorg;

        for (i=0; i<nstack; i++) {

            if(i == iroot) {
                holist++;
            }

            else {

                ioff = holist->off;
                npts = holist->nps;
                istat = 0;
                koff = 0;

                while (istat >= 0  &&  koff < npts) {

                    istat = ply_point(xpoly+rootoff, ypoly+rootoff,
                                      rootnps,
                                      *(xpoly+ioff+koff),
                                      *(ypoly+ioff+koff));
                    koff++;
                }

/*              if the hole is outside the root polygon  */

                if(istat < 0) {
                    holist++;
                }

/*              the hole is inside the root component of the polygon,
                make sure this hole is not inside another component     */

                else {

                    holsav = holist;
                    holist = holorg;

                    for (j=0; j<nstack; j++) {

                        if(j == iroot  ||  j == i) {
                            holist++;
                        }

                        else {

                            joff = holist->off;
                            jnps = holist->nps;
                            istat = -1;
                            koff = 0;

                            while (istat < 0  &&  koff < npts) {
                                istat = ply_point
                                        (xpoly+joff, ypoly+joff, jnps,
                                         *(xpoly+ioff+koff),
                                         *(ypoly+ioff+koff));
                                koff++;
                            }
                            if(istat >= 0) {
                                break;
                            }
                            holist++;
                        }
                    }

/*                  the hole is a level 1 hole of the root, output and
                    flag the hole  */

                    if(istat < 0  ||  nstack <= 2) {
                        plylist->ply = numout;
                        plylist->hol = 1;
                        plylist++;
                        istat = ply_apndholxyz(xpout, ypout, zpout, &nctemp,
                                            icout, &nout,
                                            xpoly+ioff, ypoly+ioff, zpoly+ioff,
                                            npts, 1);

                        if(istat != 0) {
                            return 1;
                        }
                        holist = holsav;
                        holist->flg = -1;
                    }
                    else {
                        holist = holsav;
                    }

/*                  increment the holist structure to check the next component
                    for inside/outside of the root component    */

                    holist++;

                }
            }
        }

/*      compress the stack in preparation for the next root component  */

        holist = holorg;
        nstacksav = nstack;
        i=0;

        while(i<nstack) {

            if(holist->flg >= 0) {
                holist++;
                i++;
            }
            else {
                holsav = holist;
                for(j=i+1; j<nstack; j++) {
                    holtmp = holist + 1;
                    holist->off = holtmp->off;
                    holist->flg = holtmp->flg;
                    holist->nps = holtmp->nps;
                    holist++;
                }
                nstack--;
                holist = holsav;
            }
        }

        if(nstack >= nstacksav) {
            return 3;
        }

/*  unnest the next root component  */

        numout++;

    }


/*  last component number for last polygon  */

    ncout[nptemp-1] = nctemp - nclast;
    *npout = nptemp;

    return 0;

}    /*  end of ply_holnestxyz function  */



/*

    ***************************************************************

                     p l y _ a p n d h o l x y z

    ***************************************************************

  function name:    ply_apndhol  (integer)

  call sequence:    ply_apndhol (xpoly, ypoly, zpoly,
                                 ncomp, icomp, npt,
                                 xhole, yhole, zhole,
                                 holpts, holev)

  synopsis:         Add points from a hole or additional component to
                    the result polygon array for polygon clipping
                    operations.

  return values:    status code

                    0 = hole added successfully
                    1 = memory overflow would happen if the hole is
                        added.  The hole was not added in this case.

  usage:            Called only from polygon clipping operations.  This
                    function assumes that polygon parameters for available
                    memory have been set properly in ply_parms.  This
                    function should not be used independent of the rest of
                    the polygon clipping functions.

  algorithm:        The available memory is checked to insure that the
                    added points will not overflow.  The holflg is
                    appended to the xpoly array and the hole nesting level
                    is appended to the ypoly array.  These signal the
                    start of a new hole or component.  The points for the
                    hole are then added.  Finally, ncomp and icomp are
                    updated to reflect the current state of the result
                    polygon.

  description of calling parameters:

    xpoly     r/w  double*    array of result x coordinates
    ypoly     r/w  double*    array of result y coordinates
    zpoly     r/w  double*    array of result z coordinates
    ncomp     r/w  int     *  number of components in result
    icomp     r/w  int     *  array with number of points in each
                              component of the result
    npt       r/w  int     *  total number of points in result
    xhole     r    double*    x coordinates of hole
    yhole     r    double*    y coordinates of hole
    zhole     r    double*    z coordinates of hole
    holpts    r    int        number of points in hole
    holev     r    int        nesting level of hole

*/

int CSWPolyUtils::ply_apndholxyz
                (double *xpoly, double *ypoly, double *zpoly,
                 int *ncomp, int *icomp, int *npt,
                 double *xhole, double *yhole, double *zhole,
                 int holpts, int holev)

{
    int          i, npmax, nhmax;

/*  suppress warning  */

    holev = holev;

/*  retrieve hole flag and maximum array sizes  */

    npmax = (int)MaxPout;
    nhmax = (int)MaxHout;

/*  check if this hole will overflow memory  */

    if (*ncomp + 1  >= nhmax) return 1;
    if (*npt + holpts + 1  >=  npmax) return 1;

/*  add points for hole to result  */

    for (i=0; i<holpts; i++) {
        xpoly[*npt] = xhole[i];
        ypoly[*npt] = yhole[i];
        zpoly[*npt] = zhole[i];
        (*npt)++;
    }

/*  update ncomp and icomp  */

    icomp[*ncomp] = holpts;
    (*ncomp)++;

    return 0;

}  /*  end of ply_apndhol function  */
