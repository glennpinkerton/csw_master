
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_drivers.cc

    This file has the methods for the CSWPolyDrivers class.  This is
    a refactor of the old ply_drivers.c code.
*/


/*
    system header files
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>


/*
    application header files
*/
#include "csw/utils/include/csw_errnum.h"
#include "csw/utils/include/csw_.h"

#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_traverse.h"
#include "csw/utils/private_include/ply_gridvec.h"
#include "csw/utils/private_include/ply_drivers.h"
#include "csw/utils/private_include/ply_compdata.h"
#include "csw/utils/private_include/csw_memmgt.h"
#include "csw/utils/private_include/csw_scope.h"



/*
  ****************************************************************

                   p l y _ t o g g l e f l a g

  ****************************************************************

*/

int CSWPolyDrivers::ply_toggleflag (void)
{
    return ToggleFlag;

}  /*  end of function ply_toggleflag  */





/*

    ***************************************************************

                     p l y _ i n t c o m p s

    ***************************************************************

  function name:    ply_intcomps   (integer)

  call sequence:    ply_intcomps (xpolys, ypolys, ncomps, icomps,
                                  xps, yps, xmins, ymins, xmaxs, ymaxs,
                                  xpolyc, ypolyc, ncompc, icompc,
                                  xpc, ypc, xminc, yminc, xmaxc, ymaxc, 
                                  xpoutr, ypoutr, npcout, ipcout,
                                  extrax, extray, 
                                  nextra, extraholes, maxextra, maxextraholes)

  synopsis:         find the intersecting components of two polygons

  return values:    status code

                    -1 = error with dynamic memory allocation
                     0 = normal successful completion
                     1 = output exceeds size of arrays defined 
                     2 = source does not intersect the clip polygon
                     3 = bad polygon, either self intersecting, zero area
                         or an unsupported grazing case

  usage:            called only by the polygon boolean operation functions
                    to intersect two polygons.

  description of calling parameters:

    err_obj    w    CSWErrNum&   Reference to object that may pass back
                                an error number.
    ply_gridvec_obj r CSWPolyGridvec&   reference to grid vec object which
                                        has been set up for the source and
                                        clip polygons by the calling function
    xpolys     r    double*     array of source polygon x coordinates
    ypolys     r    double*     array of source polygon y coordinates
    ncomps     r    int         number of source components
    icomps     r    int     *   number of points per source component
    xps        r    double*[]   array of pointers to x coordinates for each
                                source component
    yps        r    double*[]   array of pointers to y coordinates for each 
                                source component
    xmins      r    double*     array of minimum x for source components
    ymins      r    double*     array of minimum y for source components
    xmaxs      r    double*     array of maximum x for source components
    ymaxs      r    double*     array of maximum y for source components
    xpolyc     r    double*     array of x coordinates for clip polygon
    ypolyc     r    double*     array of y coordinates for clip polygon
    ncompc     r    int         number of components in clip polygon
    icompc     r    int     *   number of points in each clip component
    xpc        r    double*[]   array of pointers to x coordinates for each
                                clip component
    ypc        r    double*[]   array of pointers to y coordinates for each 
                                clip component
    xminc      r    double*     array of minimum x for clip components
    yminc      r    double*     array of minimum y for clip components
    xmaxc      r    double*     array of maximum x for clip components
    ymaxc      r    double*     array of maximum y for clip components
    xpoutr     w    double*     array of output components x coordinates
    ypoutr     w    double*     array of output components y coordinates
    npcout     w    int     *   number of polygons in output
    nhcout     w    int     *   array with number of components in each output
                                polygon
    ipcout     w    int     *   array of number of points in each output
                                component
    extrax     w    double*     array of points in extra polygons.  These polygons
                                are potential output components but they need to
                                be intersected with the original source and clip
                                first.
    extray     w    double*     y points of extra polygons
    nextra     w    int*        number of extra polygons
    extraholes w    int*        array with number of points per extra polygon
    maxextra   r    int         maximum number of points in extra polygons
    maxextraholes r int         maximum number of extra polygons

*/

int CSWPolyDrivers::ply_intcomps (CSWErrNum &err_obj,
                  CSWPolyGridvec  &ply_gridvec_obj,
                  double *xpolys, double *ypolys, int ncomps, int *icomps,
                  double **xps, double **yps, 
                  double *xmins, double *ymins, double *xmaxs, double *ymaxs,
                  double *xpolyc, double *ypolyc, int ncompc, int *icompc,
                  double **xpc, double **ypc, 
                  double *xminc, double *yminc, double *xmaxc, double *ymaxc,
                  double *xpoutr, double *ypoutr, 
                  int *npcout, int *nhcout, int *ipcout,
                  double *extrax, double *extray,
                  int *nextra, int *extraholes, int maxextra, int maxextraholes)
{
    double    **xcomp = NULL, **ycomp = NULL, *xr = NULL, *yr = NULL, ddum, pdum,
              xnext, ynext, xfirst, yfirst, **xhic = NULL, **yhic = NULL,
              *xhr = NULL, *yhr = NULL,
              *xhrsav = NULL, *yhrsav = NULL, *xminh = NULL,
              *yminh = NULL, *xmaxh = NULL, *ymaxh = NULL,
              **xcc = NULL, **ycc = NULL, yfirstsav, xfirstsav, graze, xfsav, yfsav,
              tmpgrz;
    int       *ncomp = NULL, multmem, istat, ntot, i, numtotcomp, idum,
              npcomp, startv, iflag, firstv, nowcomp, clipv, firstin,
              nextvs, j, nctot, nptapnd, jh, *nhic = NULL,
              *ihr = NULL, nhr, *ihr2 = NULL, newhol, k, kk, *ncc = NULL, firstvsav,
              ndo, maxndo;

    CSWPolyUtils     ply_utils_obj;
    CSWPolyCompdata  ply_compdata_obj;
    CSWMemmgt        csw_mem_obj;

    ddum = 0;
    idum = 0;

/*  allocate memory for output component pointers and coordinate arrays  */

    ntot = 0;
    for (i=0; i<ncomps; i++) {
        ntot += icomps[i];
    }
    for (i=0; i<ncompc; i++) {
        ntot += icompc[i];
    }

    istat = ply_utils_obj.ply_intop ('i', ddum, ddum, ntot);
    maxndo = ntot;

    istat = ply_utils_obj.ply_parms ('r', "multmem", &ddum);
    multmem = (int) (ddum + .01f);
    ntot = ntot * multmem;
    if (ntot < 1000) {
        ntot = 1000;
    }

    nctot = (ncompc + ncomps) * multmem * 2;
    if (nctot < 100) {
        nctot = 100;
    }
    
    pdum = (double) ntot;
    istat = ply_utils_obj.ply_parms ('w', "maxpmem", &pdum);
    pdum = (double) nctot;
    istat = ply_utils_obj.ply_parms ('w', "maxhmem", &pdum);

    xcomp = (double **) csw_mem_obj.csw_StackMalloc (nctot * 4 * sizeof(double *));
    if(!xcomp) {
        return -1;
    }

    ycomp = xcomp + nctot;
    xcc = ycomp + nctot;
    ycc = xcc + nctot;

    ncomp = (int *) csw_mem_obj.csw_StackMalloc (nctot * sizeof(int));
    if(!ncomp) {
        return -1;
    }

    xr = (double *) csw_mem_obj.csw_StackMalloc ( ntot * 2 * sizeof (double));
    if(!xr) {
        return -1;
    }

    yr = xr + ntot;

    xhic = (double **)csw_mem_obj.csw_StackMalloc (nctot * 2 * sizeof (double *));
    if (!xhic) {
        return -1;
    }

    yhic = xhic + nctot;

    nhic = (int *)csw_mem_obj.csw_StackMalloc (nctot * 2 * sizeof (int));
    if (!nhic) {
        return -1;
    }

    ncc = nhic + nctot;

    xhr = (double *)csw_mem_obj.csw_StackMalloc (2 * ntot * sizeof (double));
    if (!xhr) {
        return -1;
    }

    yhr = xhr + ntot;
    Ntotnr = ntot;
    xhrsav = xhr;
    yhrsav = yhr;

    ihr = (int *)csw_mem_obj.csw_StackMalloc (2 * nctot * sizeof (int));
    if (!ihr) {
        return -1;
    }
    
    ihr2 = ihr + nctot;

    xminh = (double *)csw_mem_obj.csw_StackMalloc (nctot * 4 * sizeof (double));
    if (!xminh) {
        return -1;
    }
    
    yminh = xminh + nctot;
    xmaxh = yminh + nctot;
    ymaxh = xmaxh + nctot;  

    istat = ply_compdata_obj.ply_compinit (ncomps, ncompc);
    if (istat == -1) {
        return -1;
    }

    ply_utils_obj.ply_getgraze (&graze);

/* 
----------------------------------------------------------------------------

    loop through source polygon components extracting one output
    component at a time and writing it into the temporary output array  */

/*  find components created by intersection with main source component  */

    xfirst = xpolys[0];
    yfirst = ypolys[0];
    xfirstsav = xfirst;
    yfirstsav = yfirst;
    xfsav = xfirst;
    yfsav = yfirst;
    startv = 0;
    iflag = 0;
    firstvsav = 0;
    firstin = 0;

    numtotcomp = 0;
    ndo = 0;
    npcomp = 0;

//    int  first_inside = ply_utils_obj.ply_pointpa (xpolyc, ypolyc, ncompc, icompc,
//                                                   xfirst, yfirst);

    ply_traverse_obj.ply_SetPrevInOut (1);


    for (;;) {

        ply_traverse_obj.ply_setclosingpoint (1.e30, 1.e30);
        istat = ply_traverse_obj.ply_trvintinit ("lastx2", 0.0);
        istat = ply_traverse_obj.ply_trvintinit ("orflag", 0.0);
        ply_gridvec_obj.ply_SetActiveGrid (1);
        ply_traverse_obj.ply_SetMaxTrvOut (Ntotnr);
        istat = ply_traverse_obj.ply_trvint 
                           (xpolys, ypolys, icomps[0], startv, &firstv, 0,
                            xfirst, yfirst, iflag,
                            xpc, ypc, ncompc, icompc,
                            xminc, yminc, xmaxc, ymaxc,
                            &xfirst, &yfirst, &nowcomp, &clipv,
                            xr, yr, &npcomp, 0);
        if (istat == 5) {
            return 1;
        }
        ply_utils_obj.ply_getgraze (&tmpgrz);
        ply_utils_obj.ply_setgraze (tmpgrz * 5.f);
        if (ply_utils_obj.ply_graze (xfirst, yfirst, xfsav, yfsav)) {
            if (numtotcomp > 0) {
                ply_utils_obj.ply_setgraze (tmpgrz);
                break;
            }
        }
        ply_utils_obj.ply_setgraze (tmpgrz);

        if(istat == 1) {
            istat = ply_utils_obj.ply_pointpa (xpc, ypc, ncompc, icompc,
                                 xpolys[0], ypolys[0]);
            if (istat <= 0) {
                firstin = 0;
            }
            else {
                firstin = 1;
            }
            break;
        }

        istat = ply_traverse_obj.ply_GetPrevInOut ();
        if (istat > 0) {
            firstin = 1;
        }
        else if (istat < 0) {
            firstin = 0;
        }
        else {
            if (nowcomp > 0) {
                firstin = 0;
            }
            else {
                firstin = 1;
            }
        }

        if (ply_utils_obj.ply_intop ('c', xfirst, yfirst, idum) ) {
            startv = firstv;
            continue;
        }

        if (firstin) {
            firstvsav = firstv;
            xfirstsav = xfirst;
            yfirstsav = yfirst;
            firstv = clipv;
        }
        else {
            nowcomp = 0;
        }

        istat = ply_iandcomp (ply_gridvec_obj,
                              xfirst, yfirst, firstin, nowcomp, firstv,
                              ncomps, icomps,
                              xmins, ymins, xmaxs, ymaxs,
                              ncompc, icompc,
                              xminc, yminc, xmaxc, ymaxc,
                              xps, yps, xpc, ypc,
                              xr, yr, &npcomp, &nextvs,
                              &xnext, &ynext);

        if (istat == 666) {
            if (numtotcomp > 0) {
                ply_utils_obj.ply_setgraze (tmpgrz);
                break;
            }
            return 1;
        }

/*      eliminate consecutive coincident points  */

        ply_utils_obj.ply_thinoutdups (xr, yr, &npcomp);

        if (npcomp < 4) {
            continue;
        }

/*      make sure the component is inside both polygons  */

        istat = ply_utils_obj.ply_PolyInsidePoly2 (
                                xpolyc, ypolyc, icompc, ncompc, graze,
                                xr, yr, npcomp);
        if (istat != 1) {
            continue;
        }

        istat = ply_utils_obj.ply_PolyInsidePoly2 (
                               xpolys, ypolys, icomps, ncomps, graze,
                               xr, yr, npcomp);
        if (istat != 1) {
            continue;
        }

        xcomp[numtotcomp] = xr;
        ycomp[numtotcomp] = yr;
        ncomp[numtotcomp] = npcomp;

        bool b_comp_done = false;
        if (ply_utils_obj.ply_graze (xr[0], yr[0],
                                     xr[npcomp-1], yr[npcomp-1])) {
            b_comp_done = true;
        }

        xr += npcomp;
        yr += npcomp;
        Ntotnr -= npcomp;

        startv = nextvs;
        xfirst = xnext;
        yfirst = ynext;
        if (firstin) {
            startv = firstvsav;
            xfirst = xfirstsav;
            yfirst = yfirstsav;
        }
        numtotcomp++;

        if (numtotcomp >= nctot) {
            ply_compdata_obj.ply_compfree ();
            return 1;
        }

        if (b_comp_done) {
            break;
        }

        ndo++;
        if (ndo > maxndo) {
            ply_compdata_obj.ply_compfree ();
            return 3;
        }    
    }

/*--------------------------------------------------------------------------

    add holes to the output components if needed

*/

    newhol = 0;
    *npcout = 0;
    nptapnd = 0;

    if (numtotcomp < 1) {
        istat = ply_utils_obj.ply_pointpa (xpc, ypc, ncompc, icompc,
                             xpolys[0], ypolys[0]);
        if (istat != 1) {
            return 2;
        }
        if (firstin) {
            ply_utils_obj.ply_copy (xpolys, ypolys, icomps[0], xr, yr);
            xcomp[0] = xr;
            ycomp[0] = yr;
            ncomp[0] = icomps[0];
        }
        else {
            ply_utils_obj.ply_copy (xpolyc, ypolyc, icompc[0], xr, yr);
            xcomp[0] = xr;
            ycomp[0] = yr;
            ncomp[0] = icompc[0];
        }
        numtotcomp = 1;
    }    

    for (i=0; i<numtotcomp; i++) {
        
/*      first append component to output  */

        istat = ply_utils_obj.ply_apndhol (xpoutr, ypoutr, npcout, ipcout, &nptapnd,
                             xcomp[i], ycomp[i], ncomp[i], -1);
        if (istat == 1) {
            ply_compdata_obj.ply_compfree ();
            *npcout = numtotcomp;
            return 1;
        }
        nhcout[i] = 1;

/*      union any intersecting holes inside the component  */

        istat = ply_compdata_obj.ply_compflag ('s');
        jh = 0;

        for (j=1; j<ncomps; j++) {
            if (ply_compdata_obj.ply_compchk(j)) continue;
            istat = ply_utils_obj.ply_point (xcomp[i], ycomp[i], ncomp[i],
                               *(xps[j]), *(yps[j]) );
            if (istat == 1) {
                xhic[jh] = xps[j];
                yhic[jh] = yps[j];
                nhic[jh] = j;
                xminh[jh] = xmins[j];
                yminh[jh] = ymins[j];
                xmaxh[jh] = xmaxs[j];
                ymaxh[jh] = ymaxs[j];
                jh++;
            }
        }

        istat = ply_compdata_obj.ply_compflag ('c');
        for (j=1; j<ncompc; j++) {
            if (ply_compdata_obj.ply_compchk(j)) continue;
            istat = ply_utils_obj.ply_point (xcomp[i], ycomp[i], ncomp[i],
                               *(xpc[j]), *(ypc[j]) );
            if (istat == 1) {
                xhic[jh] = xpc[j];
                yhic[jh] = ypc[j];
                nhic[jh] = -j;
                xminh[jh] = xminc[j];
                yminh[jh] = yminc[j];
                xmaxh[jh] = xmaxc[j];
                ymaxh[jh] = ymaxc[j];
                jh++;
            }
        }

        if (jh > 1) {
            istat = ply_holunion (xhic, yhic, nhic, jh,
                                  icomps, icompc,
                                  xminh, yminh, xmaxh, ymaxh,
                                  xhr, yhr, &nhr, ihr, ihr2,
                                  extrax, extray, nextra,
                                  extraholes, maxextra, maxextraholes);
            if (istat == -1) {
                err_obj.csw_SetErrNum (1);
                ply_compdata_obj.ply_compfree ();
                return -1;
            }
        }
        else {
            nhr = 0;
        }

/*      reset limits for array sizes since the holunion process may have 
        changed them  */

        pdum = (double) ntot;
        istat = ply_utils_obj.ply_parms ('w', "maxpmem", &pdum);
        pdum = (double) nctot;
        istat = ply_utils_obj.ply_parms ('w', "maxhmem", &pdum);

        kk = 0;
        for (j=0; j<nhr; j++) {
            for (k=0; k<ihr[j]; k++) {
                if (k == 0) {
                    istat = ply_utils_obj.ply_apndhol (xpoutr, ypoutr, npcout, ipcout, &nptapnd,
                                         xhr, yhr, ihr2[kk], 1);
                    if (istat == 1) {
                        ply_compdata_obj.ply_compfree ();
                        return 1;
                    }
                    nhcout[i]++;
                }
                else {
                    xcc[newhol] = xhr;
                    ycc[newhol] = yhr;
                    ncc[newhol] = ihr2[kk];
                    newhol++;
                }

                xhr += ihr2[kk];
                yhr += ihr2[kk];
                kk++;
            }
        }

        xhr = xhrsav;
        yhr = yhrsav;


/*      set used flag for source holes that lie entirely
        inside any of the clip holes  */

        ply_compdata_obj.ply_compflag ('s');
        for (j=1; j<ncomps; j++) {
            if (ply_compdata_obj.ply_compchk (j)) {
                continue;
            }
            for (k=1; k<ncompc; k++) {
                istat = ply_utils_obj.ply_point (xpc[k], ypc[k], icompc[k], 
                                   *(xps[j]), *(yps[j]));
                if (istat == 1) {
                    ply_compdata_obj.ply_compset (j);
                    break;
                }
            }
        }

/*      clip holes which lie entirely inside source holes  */

        ply_compdata_obj.ply_compflag ('c');
        for (j=1; j<ncompc; j++) {
            if (ply_compdata_obj.ply_compchk (j)) {
                continue;
            }
            for (k=1; k<ncomps; k++) {
                istat = ply_utils_obj.ply_point (xps[k], yps[k], icomps[k], 
                                   *(xpc[j]), *(ypc[j]));
                if (istat == 1) {
                    ply_compdata_obj.ply_compset (j);
                    break;
                }
            }
        }

/*      append holes which are still unused inside the component  */

        istat = ply_compdata_obj.ply_compflag ('s');
        for (j=1; j<ncomps; j++) {
            if (ply_compdata_obj.ply_compchk(j)) continue;
            istat = ply_utils_obj.ply_point (xcomp[i], ycomp[i], ncomp[i],
                               *(xps[j]), *(yps[j]) );
            if (istat == 1) {
                istat = ply_utils_obj.ply_apndhol (xpoutr, ypoutr, npcout, ipcout, &nptapnd,
                                     xps[j], yps[j], icomps[j], 1);
                if(istat == 1) {
                    ply_compdata_obj.ply_compfree ();
                    return 1;
                }
                nhcout[i]++;
            }
        }

        istat = ply_compdata_obj.ply_compflag ('c');
        for (j=1; j<ncompc; j++) {
            if (ply_compdata_obj.ply_compchk(j)) continue;
            istat = ply_utils_obj.ply_point (xcomp[i], ycomp[i], ncomp[i],
                               *(xpc[j]), *(ypc[j]) );
            if (istat == 1) {
                istat = ply_utils_obj.ply_apndhol (xpoutr, ypoutr, npcout, ipcout, &nptapnd,
                                     xpc[j], ypc[j], icompc[j], 1);
                if (istat == 1) {
                    ply_compdata_obj.ply_compfree ();
                    return 1;
                }
                nhcout[i]++;
            }
        }
    }

    for (i=0; i<newhol; i++) {
        istat = ply_utils_obj.ply_apndhol(xpoutr, ypoutr, npcout, ipcout, &nptapnd,
                            xcc[i], ycc[i], ncc[i], -1);
        if (istat == 1) {
            ply_compdata_obj.ply_compfree ();
            return 1;
        }
        nhcout[numtotcomp] = 1;
        numtotcomp++;
    }

    *npcout = numtotcomp;
    
    istat = ply_compdata_obj.ply_compfree();

    return 0;

}  /*  end of ply_intcomps function  */






/*

    ***************************************************************

                      p l y _ o r c o m p s

    ***************************************************************

  function name:    ply_orcomps   (integer)

  call sequence:    ply_orcomps (xpolys, ypolys, ncomps, icomps,
                                  xps, yps, xmins, ymins, xmaxs, ymaxs,
                                  xpolyc, ypolyc, ncompc, icompc,
                                  xpc, ypc, xminc, yminc, xmaxc, ymaxc, 
                                  xpoutr, ypoutr, npcout, ipcout)

  synopsis:         find the union components of two polygons

  return values:    status code

                    -1 = error with dynamic memory allocation
                     0 = normal successful completion
                     1 = output exceeds size of arrays defined 

  usage:            called only by the polygon boolean operation functions
                    to union two polygons.

  description of calling parameters:

    xpolys     r    double*     array of source polygon x coordinates
    ypolys     r    double*     array of source polygon y coordinates
    ncomps     r    int         number of source components
    icomps     r    int     *   number of points per source component
    xps        r    double*[]   array of pointers to x coordinates for each
                                source component
    yps        r    double*[]   array of pointers to y coordinates for each 
                                source component
    xmins      r    double*     array of minimum x for source components
    ymins      r    double*     array of minimum y for source components
    xmaxs      r    double*     array of maximum x for source components
    ymaxs      r    double*     array of maximum y for source components
    xpolyc     r    double*     array of x coordinates for clip polygon
    ypolyc     r    double*     array of y coordinates for clip polygon
    ncompc     r    int         number of components in clip polygon
    icompc     r    int     *   number of points in each clip component
    xpc        r    double*[]   array of pointers to x coordinates for each
                                clip component
    ypc        r    double*[]   array of pointers to y coordinates for each 
                                clip component
    xminc      r    double*     array of minimum x for clip components
    yminc      r    double*     array of minimum y for clip components
    xmaxc      r    double*     array of maximum x for clip components
    ymaxc      r    double*     array of maximum y for clip components
    xpoutr     w    double*     array of output components x coordinates
    ypoutr     w    double*     array of output components y coordinates
    npcout     w    int     *   number of polygons in output
    nhcout     w    int     *   array with number of components in each output
                                polygon
    ipcout     w    int     *   array of number of points in each output
                                component
*/

int CSWPolyDrivers::ply_orcomps (
                 double *xpolys, double *ypolys, int ncomps, int *icomps,
                 double **xps, double **yps, 
                 double *xmins, double *ymins, double *xmaxs, double *ymaxs,
                 double *xpolyc, double *ypolyc, int ncompc, int *icompc,
                 double **xpc, double **ypc, 
                 double *xminc, double *yminc, double *xmaxc, double *ymaxc,
                 double *xpoutr, double *ypoutr, int *npcout, int *nhcout, 
                 int *ipcout)
{
    double    **xcomp = NULL, **ycomp = NULL, *xr = NULL, *yr = NULL,
              ddum, pdum, *swap = NULL,
              xnext, ynext, xfirst, yfirst, xf, yf, x0, y0;
    double    *xpss[1], *ypss[1], *xdone = NULL, *ydone = NULL;
    int       *ncomp = NULL, multmem, istat, ntot, i, numtotcomp, idum,
              npcomp, startv, iflag, firstv, nowcomp, clipv, firstin,
              nextvs, j, jj, nctot, nptapnd, ndone, k, doneflag,
              iicomp, k2, allinside;

    CSWPolyUtils     ply_utils_obj;
    CSWPolyCompdata  ply_compdata_obj;
    CSWMemmgt        csw_mem_obj;

/*
    auto fscope = [&]()
      {
      };
    CSWScopeGuard  func_scope_guard (fscope);
*/


    idum = 0;
    iflag = 0;
    xnext = 0.0;
    ynext = 0.0;

/*  allocate memory for output component pointers and coordinate arrays  */

    ntot = 0;
    for (i=0; i<ncomps; i++) {
        ntot += icomps[i];
    }
    for (i=0; i<ncompc; i++) {
        ntot += icompc[i];
    }

    ddum = 0.0f;
    istat = ply_utils_obj.ply_intop ('i', ddum, ddum, ntot);

    istat = ply_utils_obj.ply_parms ('r', "multmem", &ddum);
    multmem = (int) (ddum + .01f);
    ntot = ntot * multmem * 2;
    if (ntot < 1000) {
        ntot = 1000;
    }

    nctot = (ncompc + ncomps) * multmem * 2;
    if (nctot < 100) {
        nctot = 100;
    }
    
    pdum = (double) ntot;
    istat = ply_utils_obj.ply_parms ('w', "maxpmem", &pdum);
    pdum = (double) nctot;
    istat = ply_utils_obj.ply_parms ('w', "maxhmem", &pdum);

    xcomp = (double **) csw_mem_obj.csw_StackMalloc (nctot * sizeof(double *));
    if(!xcomp) {
        return -1;
    }

    ycomp = (double **) csw_mem_obj.csw_StackMalloc (nctot * sizeof(double *));
    if(!ycomp) {
        return -1;
    }

    ncomp = (int *) csw_mem_obj.csw_StackMalloc (nctot * sizeof(int));
    if(!ncomp) {
        return -1;
    }

    xr = (double *) csw_mem_obj.csw_StackMalloc (2 * ntot * sizeof (double));
    if(!xr) {
        return -1;
    }

    yr = (double *) csw_mem_obj.csw_StackMalloc (2 * ntot * sizeof (double));
    if(!yr) {
        return -1;
    }
    Ntotnr = ntot;

    istat = ply_compdata_obj.ply_compinit (ncomps, ncompc);
    if (istat == -1) {
        return -1;
    }

    numtotcomp = 0;
    ply_traverse_obj.ply_setclosingpoint (1.e30, 1.e30);

/*
    find a point on the source main component that is outside of the clip main
    component, then rotate the source points so the outside point is the first
    point.
*/
    jj =0;
    for (i=0; i<icomps[0]; i++) {
        istat = ply_utils_obj.ply_point (xpolyc, ypolyc, icompc[0], 
                           xpolys[i], ypolys[i]);

        if (istat < 0) {
            if (i > 0) {
                istat = ply_utils_obj.ply_rotpts (xpolys, ypolys, &icomps[0], i);
                if (istat) {
                    return (istat);
                }
            }
            jj = 1;
            break;
        }
    }

/*  
    if no source points are outside use the clip polygon main component as the
    output main component
*/
    if (!jj) {

        ply_utils_obj.ply_copy (xpolyc, ypolyc, icompc[0], xr, yr);
        xcomp[numtotcomp] = xr;
        ycomp[numtotcomp] = yr;
        ncomp[numtotcomp] = icompc[0];

        xr += icompc[0];
        yr += icompc[0];
        numtotcomp++;
    }
        
/*  
    merge the main components of each polygon to form the main component
    of the output polygon
*/
    else {

        xfirst = xpolys[0];
        yfirst = ypolys[0];
        x0 = xfirst;
        y0 = yfirst;
        startv = 0;
        iflag = 0;

        istat = ply_traverse_obj.ply_trvintinit ("orflag", 1.1);
        ply_traverse_obj.ply_setclosingpoint (1.e30, 1.e30);
        ply_traverse_obj.ply_SetMaxTrvOut (Ntotnr);

    /*
        The union will produce a single main component and an arbitrary
        number of holes in the main component.  The loop here exits when 
        no points for another component can be found.  This means that 
        all of the holes have been found also.
    */
        xdone = xr;
        ydone = yr;
        ndone = 0;
        for (;;) {

            xf = xfirst;
            yf = yfirst;
            istat = ply_traverse_obj.ply_trvint 
                               (xpolys, ypolys, icomps[0], startv, &firstv, 0,
                                xfirst, yfirst, iflag,
                                xpc, ypc, 1, icompc,
                                xminc, yminc, xmaxc, ymaxc,
                                &xfirst, &yfirst, &nowcomp, &clipv,
                                xr, yr, &npcomp, 0);
            if (istat == 5) {
                return 1;
            }
            if (istat == 6) {
                if (firstv < startv) firstv = startv;
                if ((!ply_utils_obj.ply_graze(xf,yf,xfirst,yfirst))  &&
                    (!ply_utils_obj.ply_graze(xfirst,yfirst,x0,y0))) {
                    startv = firstv;
                    continue;
                }
                break;
            }
            if (istat == 1) {
                break;
            }

        /*
            If this is not the first output component, check if the xfirst, yfirst
            point has already been included in a previous component.  If so, then
            retraverse to the next intersection from xfirst, yfirst.
        */
            doneflag = 0;
            for (k=0; k<ndone; k++) {
                if (ply_utils_obj.ply_graze(xfirst, yfirst, xdone[k], ydone[k])) {
                    doneflag = 1;
                    break;
                }
            }

            if (doneflag) {
                startv = firstv;
                continue;
            }

            firstv = clipv;
            nowcomp = 0;
            firstin = 0;

            ply_iorcompinit ("orflag", 1.1);
            ply_traverse_obj.ply_setclosingpoint (xfirst, yfirst);

            nextvs = startv;
            istat = ply_iorcomp (xfirst, yfirst, firstin, nowcomp, firstv,
                                 1, icomps,
                                 xmins, ymins, xmaxs, ymaxs,
                                 1, icompc,
                                 xminc, yminc, xmaxc, ymaxc,
                                 xps, yps, xpc, ypc,
                                 xr, yr, &npcomp, &nextvs,
                                 &xnext, &ynext);
            if (istat == 666) {
                return 1;
            }
            if (npcomp < 3) {
                break;
            }

            xcomp[numtotcomp] = xr;
            ycomp[numtotcomp] = yr;
            ncomp[numtotcomp] = npcomp;

            xr += npcomp;
            yr += npcomp;
            ndone += npcomp;
            Ntotnr -= npcomp;

            numtotcomp++;

            firstv = nextvs;
            startv = firstv + 1;
            xfirst = xnext;
            yfirst = ynext;

            if (numtotcomp >= nctot) {
                return 1;
            }

        }  /*  end of for(;;) that finds all components  */

    }

/*
    Make sure the first component is the main component, not a hole.
*/
    if (numtotcomp > 1) {
        for (k=0; k<numtotcomp; k++) {
            allinside = 1;

            for (k2=0; k2<numtotcomp; k2++) {
                if (k == k2) continue;
                istat = ply_utils_obj.ply_point (xcomp[k], ycomp[k], ncomp[k],
                                   *(xcomp[k2]), *(ycomp[k2]));
                if (istat != 1) {
                    allinside = 0;
                    break;
                }
            }

            if (allinside == 1) {
                swap = xcomp[k];
                xcomp[k] = xcomp[0];
                xcomp[0] = swap;
                swap = ycomp[k];
                ycomp[k] = ycomp[0];
                ycomp[0] = swap;
                istat = ncomp[k];
                ncomp[k] = ncomp[0];
                ncomp[0] = istat;
                break;
            }
        }
    }
    
/* 
----------------------------------------------------------------------------

    Calculate source holes that lie outside the clip main component.

*/

    xfirst = xnext;
    yfirst = ynext;
    ply_traverse_obj.ply_trvintinit ("orflag", 2.1);

    for (iicomp=1; iicomp<ncomps; iicomp++) {

        startv = 0;
        xpss[0] = xps[i];
        ypss[0] = yps[i];

        ply_traverse_obj.ply_SetPrevInOut (0);
        for (;;) {

            ply_traverse_obj.ply_setclosingpoint (1.e30, 1.e30);
            ply_traverse_obj.ply_SetMaxTrvOut (Ntotnr);
            istat = ply_traverse_obj.ply_trvint
                               (xps[iicomp], yps[iicomp], icomps[iicomp], 
                                startv, &firstv, iicomp,
                                xfirst, yfirst, iflag,
                                xpc, ypc, 1, icompc,
                                &(xminc[iicomp]), &(yminc[iicomp]), 
                                &(xmaxc[iicomp]), &(ymaxc[iicomp]),
                                &xfirst, &yfirst, &nowcomp, &clipv,
                                xr, yr, &npcomp, 0);
            if (istat == 5) {
                return 1;
            }

            if(istat == 1) {
                break;
            }

            idum = 0;
            if (ply_utils_obj.ply_intop ('c', xfirst, yfirst, idum) ) {
                startv = firstv;
                continue;
            }

            istat = ply_traverse_obj.ply_GetPrevInOut ();

            if (istat == -1) {
                firstin = 0;
            }
            else {
                firstin = 1;
                nowcomp = iicomp;
            }

            ply_traverse_obj.ply_setclosingpoint (xfirst, yfirst);
            istat = ply_iorcomp (xfirst, yfirst, firstin, nowcomp, firstv,
                                 1, &icomps[iicomp],
                                 &(xmins[iicomp]), &(ymins[iicomp]), 
                                 &(xmaxs[iicomp]), &(ymaxs[iicomp]),
                                 1, icompc,
                                 xminc, yminc, xmaxc, ymaxc,
                                 xpss, ypss, xpc, ypc,
                                 xr, yr, &npcomp, &nextvs,
                                 &xnext, &ynext);
            if (istat == 666) {
                return 1;
            }

            if (npcomp < 4) {
                continue;
            }

            xcomp[numtotcomp] = xr;
            ycomp[numtotcomp] = yr;
            ncomp[numtotcomp] = npcomp;

            xr += npcomp;
            yr += npcomp;
            Ntotnr -= npcomp;

            startv = nextvs;
            xfirst = xnext;
            yfirst = ynext;
            numtotcomp++;

            if (numtotcomp >= nctot) {
                return 1;
            }

        }

    }

/* 
----------------------------------------------------------------------------

    Calculate clip holes that lie outside the source main component.

*/

    xfirst = xnext;
    yfirst = ynext;
    ply_traverse_obj.ply_trvintinit ("orflag", 2.1);

    for (iicomp=1; iicomp<ncompc; iicomp++) {

        startv = 0;
        xpss[0] = xpc[i];
        ypss[0] = ypc[i];

        ply_traverse_obj.ply_SetPrevInOut (0);
        for (;;) {

            ply_traverse_obj.ply_setclosingpoint (1.e30, 1.e30);
            ply_traverse_obj.ply_SetMaxTrvOut (Ntotnr);
            istat = ply_traverse_obj.ply_trvint
                               (xpc[iicomp], ypc[iicomp], icompc[iicomp], 
                                startv, &firstv, iicomp,
                                xfirst, yfirst, iflag,
                                xps, yps, 1, icomps,
                                &(xmins[iicomp]), &(ymins[iicomp]), 
                                &(xmaxs[iicomp]), &(ymaxs[iicomp]),
                                &xfirst, &yfirst, &nowcomp, &clipv,
                                xr, yr, &npcomp, 0);
            if (istat == 5) {
                return 1;
            }

            if(istat == 1) {
                break;
            }

            if (ply_utils_obj.ply_intop ('c', xfirst, yfirst, idum) ) {
                startv = firstv;
                continue;
            }

            istat = ply_traverse_obj.ply_GetPrevInOut ();

            if (istat == -1) {
                firstin = 0;
            }
            else {
                firstin = 1;
                nowcomp = iicomp;
            }

            ply_traverse_obj.ply_setclosingpoint (xfirst, yfirst);
            istat = ply_iorcomp (xfirst, yfirst, firstin, nowcomp, firstv,
                                 1, &icompc[iicomp],
                                 &(xminc[iicomp]), &(yminc[iicomp]), 
                                 &(xmaxc[iicomp]), &(ymaxc[iicomp]),
                                 1, icomps,
                                 xmins, ymins, xmaxs, ymaxs,
                                 xpss, ypss, xps, yps,
                                 xr, yr, &npcomp, &nextvs,
                                 &xnext, &ynext);

            if (istat == 666) {
                return 1;
            }
            if (npcomp < 4) {
                continue;
            }

            xcomp[numtotcomp] = xr;
            ycomp[numtotcomp] = yr;
            ncomp[numtotcomp] = npcomp;

            xr += npcomp;
            yr += npcomp;
            Ntotnr -= npcomp;

            startv = nextvs;
            xfirst = xnext;
            yfirst = ynext;
            numtotcomp++;

            if (numtotcomp >= nctot) {
                return 1;
            }

        }

    }

/*--------------------------------------------------------------------------

    add holes to the output components if needed

*/

    *nhcout = 0;
    jj = 0;
    nptapnd = 0;

        
/*  first append main component to output  */

    istat = ply_utils_obj.ply_apndhol (xpoutr, ypoutr, nhcout, ipcout, &nptapnd,
                             xcomp[0], ycomp[0], ncomp[0], -1);
    if (istat == 1) {
        return 1;
    }
    nhcout[i] = 1;
    jj++;

/*  append any other components as holes  */

    for (j=1; j<numtotcomp; j++) {
        istat = ply_utils_obj.ply_apndhol (xpoutr, ypoutr, nhcout, ipcout, &nptapnd,
                             xcomp[j], ycomp[j], ncomp[j], 1);
        if (istat == 1) {
            return 1;
        }
    }

/*  set used flag for untouched source holes that have any
    clip holes inside of them  */

    ply_compdata_obj.ply_compflag ('s');
    for (j=1; j<ncomps; j++) {
        if (ply_compdata_obj.ply_compchk (j)) {
            continue;
        }
        for (k=1; k<ncompc; k++) {
            istat = ply_utils_obj.ply_point (xpc[k], ypc[k], icompc[k], 
                               *(xps[j]), *(yps[j]));
            if (istat == -1) {
                ply_compdata_obj.ply_compset (j);
                break;
            }
        }
    }

/*  untouched clip holes with any interior source holes  */

    ply_compdata_obj.ply_compflag ('c');
    for (j=1; j<ncompc; j++) {
        if (ply_compdata_obj.ply_compchk (j)) {
            continue;
        }
        for (k=1; k<ncomps; k++) {
            istat = ply_utils_obj.ply_point (xps[k], yps[k], icomps[k], 
                               *(xpc[j]), *(ypc[j]));
            if (istat == -1) {
                ply_compdata_obj.ply_compset (j);
                break;
            }
        }
    }

    
/*  untouched source holes  */

    ply_compdata_obj.ply_compflag ('s');
    
    for (j=1; j<ncomps; j++) {
        if (!ply_compdata_obj.ply_compchk (j) ) {
            istat = ply_utils_obj.ply_pointpa (xpc, ypc, ncompc, icompc,
                                 *(xps[j]), *(yps[j]) );
            if (istat == -1) {
                istat = ply_utils_obj.ply_apndhol (xpoutr, ypoutr, nhcout, ipcout,
                                     &nptapnd,
                                     xps[j], yps[j], icomps[j], 1);
                if (istat == 1) {
                    return 1;
                }
            }
        }
    }

/*  untouched clip holes  */

    ply_compdata_obj.ply_compflag ('c');

    for (j=1; j<ncompc; j++) {
        if (!ply_compdata_obj.ply_compchk (j) ) {
            istat = ply_utils_obj.ply_pointpa (xps, yps, ncomps, icomps,
                                 *(xpc[j]), *(ypc[j]) );
            if (istat == -1) {
                istat = ply_utils_obj.ply_apndhol (xpoutr, ypoutr, nhcout, ipcout,
                                     &nptapnd,
                                     xpc[j], ypc[j], icompc[j], 1);
                if (istat == 1) {
                    return 1;
                }
            }
        }
    }
            
    istat = ply_compdata_obj.ply_compfree();

    *npcout = 1;

    return 0;

}  /*  end of ply_orcomps function  */






/*

    ***************************************************************

                    p l y _ i a n d c o m p

    ***************************************************************

  function name:    ply_iandcomp    (integer)

  call sequence:    ply_iandcomp(xfirst, yfirst, firstinside, nowcomp, firstv,
                                 ncomps, icomps,
                                 xminps, yminps, xmaxps, ymaxps,
                                 ncompc, icompc,
                                 xminpc, yminpc, xmaxpc, ymaxpc,
                                 xps, yps, xpc, ypc,
                                 xcomp, ycomp, npcomp, nextvs, 
                                 xnext, ynext)

  synopsis:          find an individual component in polygon intersection

  return values:     status code

                     0 = normal successful completion
                     1 = error in routine
                     2 = first intersection already used
                   666 = bad polygon can't be traversed
                     -1 = not enough memory

  usage:             This function is called from ply_iand only.  The
                     calling function is responsible for error checking 
                     and preparing the data for this function.

  algorithm:         The source polygon and clipping polygon are alternately
                     traversed from intersection to intersection until the
                     original point is reoccupied.

  description of calling parameters:

    xfirst    r    double      X coordinate of first point in the component
    yfirst    r    double      Y coordinate of first point in component
    firstin   r    int         flag for whether the preceding point in the 
                               source is inside or outside of the clip polygon.
                               0 = outside, 1 = inside
    nowcomp   r    int         component number of first source point
    firstv    r    int         side number of first source point
    ncomps    r    int         number of components in source polygon
    icomps    r    int     *   array with number of points in each source
                               component
    xminps    r    double*     array of minimum x values for source components
    yminps    r    double*     array of minimum y values for source components
    xmaxps    r    double*     array of maximum x values for source components
    ymaxps    r    double*     array of maximum y values for source components
    ncompc    r    int         number of components in clipping polygon
    icompc    r    int     *   array with number of points in each clipping
                               component
    xminpc    r    double*     array of minimum x values for clipping components
    yminpc    r    double*     array of minimum y values for clipping components
    xmaxpc    r    double*     array of maximum x values for clipping components
    ymaxpc    r    double*     array of maximum y values for clipping components
    xps       r    double**    array of pointers to x source components
    yps       r    double**    array of pointers to y source components
    xpc       r    double**    array of pointers to x clipping components
    ypc       r    double**    array of pointers to y clipping components
    xcomp     w    double*     x coordinates of output component
    ycomp     w    double*     y coordinates of output component
    npcomp    w    int     *   number of points in the output component
    nextvs    w    int     *   number of next vector on the source to scan
    xnext     w    double*     x coordinate of next source scan start
    ynext     w    double*     y coordinate of next source scan start

*/

int CSWPolyDrivers::ply_iandcomp (
                 CSWPolyGridvec   &ply_gridvec_obj,
                 double xfirst, double yfirst, int firstin, int nowcomp, int firstv,
                 int ncomps, int *icomps,
                 double *xminps, double *yminps, double *xmaxps, double *ymaxps,
                 int ncompc, int *icompc,
                 double *xminpc, double *yminpc, double *xmaxpc, double *ymaxpc,
                 double **xps, double **yps, double **xpc, double **ypc,
                 double *xcomp, double *ycomp, int *npcomp, int *nextvs, 
                 double *xnext, double *ynext)
{
    double     xend, yend, xstart, ystart;
    int        toggle, istat, startv, nextv, outflag, ndum, nc,
               nstart, npts, iflag1, iflag2, iflag, nctmp,
               cflag, ntotnrloc;
    CSWPolyUtils     ply_utils_obj;
    CSWPolyCompdata  ply_compdata_obj;

    istat = ply_compdata_obj.ply_compinit (ncomps, ncompc);
    if (istat == -1) {
        return -1;
    }

    ndum = 0;

/*  set up for first traverse of polygon  */

    istat = ply_traverse_obj.ply_trvintinit ("orflag", 0.0);
    istat = ply_traverse_obj.ply_sidintinit ("orflag", 0.0);

    startv = firstv;
    if(startv < 1) startv = 1;
    xend = xfirst;
    yend = yfirst;
    xstart = xfirst;
    ystart = yfirst;

    if(firstin) {
        toggle = -1;
        nstart = 1;
        *xnext = xend;
        *ynext = yend;
        *nextvs = startv;
        iflag1 = 1;
        iflag2 = 1;
    }
    else {
        toggle = 1;
        nstart = 0;
        iflag1 = 1;
        iflag2 = 1;
    }

    *xcomp = xfirst;
    *ycomp = yfirst;
    xcomp++;
    ycomp++;
    *npcomp = 1;
    outflag = 1;
    nc = nowcomp;
    nextv = startv;
    cflag = 0;

    ply_traverse_obj.ply_setclosingpoint (xfirst, yfirst);

/*  set intersection array for start point if not already set  */

    istat = ply_utils_obj.ply_intop('+', xfirst, yfirst, ndum);

/*  scan until intersection is found and then toggle to scan the other
    polygon.  Continue until the polygon closes  */

    ntotnrloc = Ntotnr;
    for(;;) {

/*      traverse along source  */

        npts = 0;
        ToggleFlag = toggle;
        ply_traverse_obj.ply_SetMaxTrvOut (ntotnrloc);
        if(toggle == 1) {
            iflag = iflag1;
            if(nc != 0) iflag = 1;
            nctmp = nc;
            ply_gridvec_obj.ply_SetActiveGrid (1);
            cflag = nc;
            istat = ply_traverse_obj.ply_trvint
                              (xps[nc], yps[nc], icomps[nc], startv, &nextv, cflag,
                               xstart, ystart, iflag,
                               xpc, ypc, ncompc, icompc,
                               xminpc, yminpc, xmaxpc, ymaxpc,
                               &xstart, &ystart, &nc, &startv,
                               xcomp, ycomp, &npts, outflag);
        }

/*      traverse along clip  */

        else {
            iflag = iflag2;
            if(nc != 0) iflag = 1;
            nctmp = nc;
            ply_gridvec_obj.ply_SetActiveGrid (-1);
            cflag = nc;
            istat = ply_traverse_obj.ply_trvint
                              (xpc[nc], ypc[nc], icompc[nc], startv, &nextv, cflag,
                               xstart, ystart, iflag,
                               xps, yps, ncomps, icomps,
                               xminps, yminps, xmaxps, ymaxps,
                               &xstart, &ystart, &nc, &startv,
                               xcomp, ycomp, &npts, outflag);
        }
        if (istat == 5) {
            *npcomp = 0;
            return 666;
        }

/*      if no intersection was found, there is a problem 
        return 1 to indicate this problem  */

        if(istat == 1) {
            *npcomp = 0;
            return 1;
        }

/*      a return status of -1 indicates not enough space for intersections  */

        if (istat == -1) {
            *npcomp = 0;
            return -1;
        }        

/*      set hole component intersection flags */

        if (toggle == 1) {
            ply_compdata_obj.ply_compflag ('s');
            ply_compdata_obj.ply_compset (nctmp);
            ply_compdata_obj.ply_compflag ('c');
            ply_compdata_obj.ply_compset (nc);
        }
        else {
            ply_compdata_obj.ply_compflag ('c');
            ply_compdata_obj.ply_compset (nctmp);
            ply_compdata_obj.ply_compflag ('s');
            ply_compdata_obj.ply_compset (nc);
        }

/*      save as xnext, ynext if needed  */

        if(!nstart  &&  toggle == 1) {
            *xnext = xstart;
            *ynext = ystart;
            *nextvs = nextv;
            nstart = 1;
        }

        xcomp += npts;
        ycomp += npts;
        *npcomp += npts;
        ntotnrloc -= npts;

/*      toggle to opposite polygon  */

        toggle = -toggle;

/*      add to intersection list  */

        istat = ply_utils_obj.ply_intop('+', xstart, ystart, ndum);

/*      memory overflow in intersection list  */

        if(istat == -1) {
            *npcomp = 0;
            return -1;
        }

/*      check if this closes the component  */

        if(istat) {
            if(ply_utils_obj.ply_graze (xstart, ystart, xfirst, yfirst)) {
                 break;
            }
        }

    }

/*  make sure the polygon closed properly and return  */

/*
    if(nc != nowcomp  ||  !nstart) {
        *npcomp = 0;
        return 1;
    }
*/

    return 0;

}  /*  end of ply_iandcomp function  */






/*

    ***************************************************************

                       p l y _ i o r c o m p

    ***************************************************************

  function name:    ply_iorcomp    (integer)

  call sequence:    ply_iorcomp(xfirst, yfirst, firstinside, nowcomp, firstv,
                                ncomps, icomps,
                                xminps, yminps, xmaxps, ymaxps,
                                ncompc, icompc,
                                xminpc, yminpc, xmaxpc, ymaxpc,
                                xps, yps, xpc, ypc,
                                xcomp, ycomp, npcomp, nextvs, 
                                xnext, ynext)

  synopsis:          find an individual component in polygon union

  return values:     status code

                     0 = normal successful completion
                     1 = error in routine
                   666 = bad polygon, can't be traversed
                     -1 = not enough memory

  usage:             This function is called from other polygon functions
                     only.  The calling function is responsible for error 
                     checking and preparing the data for this function.

  algorithm:         The source polygon and clipping polygon are alternately
                     traversed from intersection to intersection until the
                     original point is reoccupied.

  description of calling parameters:

    xfirst    r    double      X coordinate of first point in the component
    yfirst    r    double      Y coordinate of first point in component
    firstin   r    int         flag for whether the preceding point in the 
                               source is inside or outside of the clip polygon.
                               0 = outside, 1 = inside
    nowcomp   r    int         component number of first source point
    firstv    r    int         side number of first source point
    ncomps    r    int         number of components in source polygon
    icomps    r    int     *   array with number of points in each source
                               component
    xminps    r    double*     array of minimum x values for source components
    yminps    r    double*     array of minimum y values for source components
    xmaxps    r    double*     array of maximum x values for source components
    ymaxps    r    double*     array of maximum y values for source components
    ncompc    r    int         number of components in clipping polygon
    icompc    r    int     *   array with number of points in each clipping
                               component
    xminpc    r    double*     array of minimum x values for clipping components
    yminpc    r    double*     array of minimum y values for clipping components
    xmaxpc    r    double*     array of maximum x values for clipping components
    ymaxpc    r    double*     array of maximum y values for clipping components
    xps       r    double**    array of pointers to x source components
    yps       r    double**    array of pointers to y source components
    xpc       r    double**    array of pointers to x clipping components
    ypc       r    double**    array of pointers to y clipping components
    xcomp     w    double*     x coordinates of output component
    ycomp     w    double*     y coordinates of output component
    npcomp    w    int     *   number of points in the output component
    nextvs    w    int     *   number of next vector on the source to scan
    xnext     w    double*     x coordinate of next source scan start
    ynext     w    double*     y coordinate of next source scan start
    
*/

int CSWPolyDrivers::ply_iorcomp(
                double xfirst, double yfirst, int firstin, int nowcomp, int firstv,
                int ncomps, int *icomps,
                double *xminps, double *yminps, double *xmaxps, double *ymaxps,
                int ncompc, int *icompc,
                double *xminpc, double *yminpc, double *xmaxpc, double *ymaxpc,
                double **xps, double **yps, double **xpc, double **ypc,
                double *xcomp, double *ycomp, int *npcomp, int *nextvs, 
                double *xnext, double *ynext)
{
    double     xend, yend, xstart, ystart;
    int        toggle, istat, startv, nextv, tnextv, outflag, ndum, nc,
               nstart, npts, iflag1, iflag2, iflag, nctmp,
               ntotnrloc;
    CSWPolyUtils     ply_utils_obj;
    CSWPolyCompdata  ply_compdata_obj;

/*  set up for first traverse of polygon  */

    istat = ply_traverse_obj.ply_trvintinit ("lastx2", 0.0);
    istat = ply_traverse_obj.ply_trvintinit ("orflag", (double)orflag+.1);
    istat = ply_traverse_obj.ply_sidintinit ("orflag", (double)orflag+.1);

    startv = firstv;
    if(startv < 1) startv = 1;
    xend = xfirst;
    yend = yfirst;
    xstart = xfirst;
    ystart = yfirst;

    if(!firstin) {
        toggle = -1;
        nstart = -1;
        *xnext = xend;
        *ynext = yend;
        iflag1 = 1;
        iflag2 = 1;
    }
    else {
        toggle = 1;
        nstart = 0;
        iflag1 = 1;
        iflag2 = 1;
    }

    *xcomp = xfirst;
    *ycomp = yfirst;
    xcomp++;
    ycomp++;
    *npcomp = 1;
    outflag = 1;
    nc = nowcomp;
    nextv = *nextvs;

/*  set intersection array for start point if not already set  */

    ndum = 0;
    istat = ply_utils_obj.ply_intop('+', xfirst, yfirst, ndum);

/*  scan until intersection is found and then toggle to scan the other
    polygon.  Continue until the polygon closes  */

    ntotnrloc = Ntotnr;
    for(;;) {

/*      traverse along source  */

        npts = 0;
        ToggleFlag = toggle;
        ply_traverse_obj.ply_SetMaxTrvOut (ntotnrloc);
        if(toggle == 1) {
            iflag = iflag1;
            if(nc != 0) iflag = 1;
            nctmp = nc;
            istat = ply_traverse_obj.ply_trvint
                              (xps[nc], yps[nc], icomps[nc], startv, &tnextv, nctmp,
                               xstart, ystart, iflag,
                               xpc, ypc, ncompc, icompc,
                               xminpc, yminpc, xmaxpc, ymaxpc,
                               &xstart, &ystart, &nc, &startv,
                               xcomp, ycomp, &npts, outflag);
            nextv = tnextv;
            if (nstart == -1) {
                *nextvs = nextv - 1;
                nstart = 1;
            }
            iflag1 = 1;
        }

/*      traverse along clip  */

        else {
            iflag = iflag2;
            if(nc != 0) iflag = 1;
            nctmp = nc;
            istat = ply_traverse_obj.ply_trvint
                              (xpc[nc], ypc[nc], icompc[nc], startv, &tnextv, nctmp,
                               xstart, ystart, iflag,
                               xps, yps, ncomps, icomps,
                               xminps, yminps, xmaxps, ymaxps,
                               &xstart, &ystart, &nc, &startv,
                               xcomp, ycomp, &npts, outflag);
        }

        if (istat == 5) {
            *npcomp = 0;
            return 666;
        }

/*      check if the traverse is to be added to the component  */

/*      if no intersection was found, there is a problem 
        return 1 to indicate this problem  */

        if(istat == 1) {
            *npcomp = 0;
            return 1;
        }

/*      set hole component intersection flags */

        if (toggle == 1) {
            ply_compdata_obj.ply_compflag ('s');
            ply_compdata_obj.ply_compset (nctmp);
            ply_compdata_obj.ply_compflag ('c');
            ply_compdata_obj.ply_compset (nc);
        }
        else {
            ply_compdata_obj.ply_compflag ('c');
            ply_compdata_obj.ply_compset (nctmp);
            ply_compdata_obj.ply_compflag ('s');
            ply_compdata_obj.ply_compset (nc);
        }

/*      save as xnext, ynext if needed  */

        if(!nstart) {
            *xnext = xstart;
            *ynext = ystart;
            *nextvs = nextv;
            nstart = 1;
        }

        xcomp += npts;
        ycomp += npts;
        *npcomp += npts;
        ntotnrloc -= npts;

/*      toggle to opposite polygon if needed  */

        toggle = -toggle;

/*      add to intersection list  */

        istat = ply_utils_obj.ply_intop('+', xstart, ystart, ndum);

/*      memory overflow in intersection list  */

        if(istat == -1) {
            *npcomp = 0;
            return -1;
        }

/*      check if this closes the component  */

        if(istat) {
            if(ply_utils_obj.ply_graze (xstart, ystart, xfirst, yfirst)) {
                 break;
            }
        }

    }

    return 0;

}  /*  end of ply_iorcomp function  */


/*
  ****************************************************************

                p l y _ i o r c o m p i n i t

  ****************************************************************

*/

int CSWPolyDrivers::ply_iorcompinit (const char *string, double val)
{
    if (!strcmp (string, "orflag")) {
        orflag = (int) val;
    }

    return 0;

}  /*  end of function ply_iorcompinit  */







/*

    ***************************************************************

                     p l y _ h o l i n t

    ***************************************************************

  function name:    ply_holint

  call sequence:    ply_holint
                        (xpolys, ypolys, ncomps, icomps, nst,
                         xpolyc, ypolyc, ncompc, icompc, nct,
                         xpolyr, ypolyr, ncompr, icompr, nrt)

  synopsis:         Add holes to results of polygon clipping. 

  return values:    status code

                    -1  error in dynamic memory allocation
                     0  successful completion
                     1  error appending hole to result polygon
                        this is probably because the result polygon
                        is not large enough to hold all of the holes

  usage:            Called by ply_intsect to add holes according to the 
                    hole rules for polygon intersection.  Only one output
                    component at a time is treated here.  Only one level
                    of hole nesting is allowed.  The polygons must be
                    preprocessed with ply_holnest to remove higher levels
                    of nesting before calling this routine.

  algorithm:        Holes which are still intact are checked.  Clip holes
                    outside of the source polygon and source holes outside
                    clip polygon are removed.  The remaining holes are
                    apended to the result polygon.
                    
  description of calling parameters:

    xpolys    r    double*      array of source polygon x coordinates
    ypolys    r    double*      array of source polygon y coordinates
    ncomps    r    int          number of components in source polygon
    icomps    r    int     *    array with number of points in each source
                                polygon component
    nst       r    int          total number of source points
    xpolyc    r    double*      array of clipping polygon x coordinates
    ypolyc    r    double*      array of clipping polygon y coordinates
    ncompc    r    int          number of components in clipping polygon
    icompc    r    int     *    array with number of points in each
                                clipping polygon component
    nct       r    int          total number of clip polygon points
    xpolyr    r/w  double*      array of x coordinates for result polygon
    ypolyr    r/w  double*      array of y coordinates for result polygon
    ncompr    r/w  int     *    number of components in result polygon
    icompr    w    int     *    array with number of points for each 
                                component of the result polygon
    nrt       r/w  int     *    total number of result points
  
*/

int CSWPolyDrivers::ply_holint  (
                 double *xpolys, double *ypolys, int ncomps, int *icomps, 
                 int *jcomps, int nst,
                 double *xpolyc, double *ypolyc, int ncompc, int *icompc, 
                 int *jcompc, int nct,
                 double *xpolyr, double *ypolyr, int *ncompr, int *icompr, 
                 int *nrt)
{
    int           *jsloc, *jcloc, nclist, nslist;
    int           i, j, offset, istat, itemp;
    CList         *iclist, *islist, *iclsav, *islsav;
    CSWPolyUtils  ply_utils_obj;
    CSWMemmgt     csw_mem_obj;


/*  allocate memory for local pointers  */

    jsloc = (int *) csw_mem_obj.csw_StackMalloc (ncomps * sizeof(int));
    if(!jsloc) {
        return -1;
    }

    jcloc = (int  *) csw_mem_obj.csw_StackMalloc (ncompc * sizeof(int ));
    if(!jcloc) {
        return -1;
    }

    islist = (CList *) csw_mem_obj.csw_StackMalloc (ncomps * sizeof(CList));
    if(!islist) {
        return -1;
    }

    iclist = (CList *) csw_mem_obj.csw_StackMalloc (ncompc * sizeof(CList));
    if(!iclist) {
        return -1;
    }

    nslist = 0;
    nclist = 0;

/*  remove source holes lying outside clip polygon  */

    islsav = islist;
    offset = 0;
    for (j=1; j<ncomps; j++) {
        itemp = icomps[j-1];
        offset += itemp;
        if(jcomps[j] < 0) continue;
        istat = ply_utils_obj.ply_point(xpolyc, ypolyc, nct,
                          xpolys[offset+1], ypolys[offset+1]);
        if(istat == -1) {
            islist->nps = icomps[j];
            islist->off = offset+1;
            islist++;
            nslist++;
        }
    }
    islist = islsav;

/*  remove clipping holes lying outside source polygon  */

    iclsav = iclist;
    offset = 0;
    for (j=1; j<ncompc; j++) {
        itemp = icompc[j-1];
        offset += itemp;
        if(jcompc[j] < 0) continue;
        istat = ply_utils_obj.ply_point(xpolys, ypolys, nst,
                          xpolyc[offset+1], ypolyc[offset+1]);
        if(istat == -1) {
            iclist->nps = icompc[j];
            iclist->off = offset+1;
            iclist++;
            nclist++;
        }
    }
    iclist = iclsav;

/*  free memory and return if no holes are left to process  */

    if (nslist + nclist  <=  0) {
        return 0;
    }
            
/*  append remaining clip holes  */

    for (i=0; i<nclist; i++) {
        istat = ply_utils_obj.ply_apndhol 
            (xpolyr, ypolyr, ncompr, icompr, nrt,
             xpolyc, ypolyc, iclist->nps, 1);
        if(istat == 1) {
            return 1;
        }
        iclist++;
    }

/*  append remaining source holes  */

    for (i=0; i<nslist; i++) {
        istat = ply_utils_obj.ply_apndhol
            (xpolyr, ypolyr, ncompr, icompr, nrt,
             xpolys, ypolys, islist->nps, 1);
        if (istat == 1) {
            return 1;
        }
        islist++;
    }


    return 0;

}   /*  end of ply_holint function  */







/*

    ***************************************************************

                      p l y _ h o l u n i o n

    ***************************************************************

  function name:    ply_holunion    (integer)

  call sequence:    ply_holunion (xhic, yhic, nhic, nh,
                                  icomps, icompc, 
                                  xmin, ymin, xmax, ymax,
                                  xhr, yhr, nhr, ihr, iphr,
                                  extrax, extray,
                                  nextra, extraholes, maxextra, maxextraholes)

  synopsis:         union polygon holes 

  return values:    status code

                    -1 = error allocating memory 
                     0 = normal successful execution

  usage:
  
    This function is called from ply_intcomps only.  It is used to union
    polygon holes as part of the polygon intersection operation.

  algorithm:

    Check which holes intersect each other, and union those holes.
    Holes unioned will have their used flag set, all other holes will
    still be "untouched".

  description of calling parameters:

    xhic     r   double**    array of pointers to hole polygon x coordinates
    yhic     r   double**    array of pointers to hole polygon y coordinates
    nhic     r   int     *   array with indexes into icomp arrays for holes
    nh       r   int         number of holes in xhic...nhic
    icomps   r   int     *   array with number of points in each source component
    icompc   r   int     *   array with number of points in each clip component
    xmin     r   double*     array with hole minimum x coordinates
    ymin     r   double*     array with hole minimum y coordinates
    xmax     r   double*     array with hole maximum x's
    ymax     r   double*     array with hole maximum y's
    xhr      w   double*     output array with unioned hole x coords
    yhr      w   double*     output array with unioned hole y coords
    nhr      w   int     *   number of output polygons
    ihr      w   int     *   array with number of components per output polygon
    iphr     w   int     *   array with number of points per output component
    extrax   w   double*     array of points in extra polygons.  These polygons
                             are potential output components but they need to
                             be intersected with the original source and clip
                             first.
    extray   w   double*     y points of extra polygons
    nextra   w   int*        number of extra polygons
    extraholes w int*        array with number of points per extra polygon
    maxextra r   int         maximum number of points in extra polygons
    maxextraholes r  int     maximum number of extra polygons
  
*/

int CSWPolyDrivers::ply_holunion (
                  double **xhic, double **yhic, int *nhic, int nh,
                  int *icomps, int *icompc,
                  double *xmin, double *ymin, double *xmax, double *ymax,
                  double *xhr, double *yhr, int *nhr, int *ihr, int *iphr,
                  double *extrax, double *extray,
                  int *nextra, int *extraholes, int maxextra, int maxextraholes)
{
    POlyrec     *prec = NULL, *precsav = NULL, **preca = NULL;

    int         i, istat, ndum, ndum2, ndum3, *itmp = NULL, nextv, j, jj, noffset;
    double      xt1, yt1, xt2, yt2, xdum, ydum, xdum2, ydum2,
                xdum3, ydum3;
    char        sctmp;
    double      *xtmp = NULL, *ytmp = NULL, **xst = NULL,
                **yst = NULL, **xct = NULL, **yct = NULL,
                *xtmpsav = NULL, *ytmpsav = NULL, *xhrsav = NULL,
                *yhrsav = NULL, *xmint = NULL, *ymint = NULL,
                *xmaxt = NULL, *ymaxt = NULL;
    int         itemp, *iphrsav = NULL, ntpt, nthol, 
                nbyte, multmem;
    int         nex, extot;
    double      *ex = NULL, *ey = NULL;
    CSWPolyUtils     ply_utils_obj;
    CSWPolyCompdata  ply_compdata_obj;
    CSWMemmgt        csw_mem_obj;

/*  allocate memory for polygon records  */

    prec = (POlyrec *)csw_mem_obj.csw_StackMalloc (nh * sizeof (POlyrec));
    if (!prec) {
        return -1;
    }

    preca = (POlyrec **)csw_mem_obj.csw_StackMalloc (nh * sizeof (POlyrec *));
    if (!preca) {
        return -1;
    }

    precsav = prec;

/*  initial setup of polygon record array  */

    for (i=0; i<nh; i++) {
        itemp = nhic[i];
        preca[i] = prec;
        prec++;
        if (itemp < 0) {
            itemp = -itemp;
            preca[i]->npt = icompc[itemp];
            preca[i]->scflag = 'c';
        }
        else {
            preca[i]->npt = icomps[itemp];
            preca[i]->scflag = 's';
        }
        preca[i]->scf2 = preca[i]->scflag;
        preca[i]->icomp = itemp;
        preca[i]->nc = 1;
        preca[i]->xpoly = xhic[i];
        preca[i]->ypoly = yhic[i];
        preca[i]->iflag = 0;
        preca[i]->ic = &(preca[i]->npt);
        preca[i]->xpa[0] = preca[i]->xpoly;
        preca[i]->ypa[0] = preca[i]->ypoly;
    }

    prec = precsav;

/*  reverse the orientation of holes */

    for (i=0; i<nh; i++) {
        ply_utils_obj.ply_revers (preca[i]->xpoly, preca[i]->ypoly,
                    preca[i]->npt);
    }


/*------------------------------------------------------------------*/

/*  union the holes which intersect  */

/*  add up number of points and number of holes  */

    istat = ply_utils_obj.ply_parms ('r', "multmem", &xdum);
    ydum = xdum;
    multmem = (int)(xdum+.01f);

    ntpt = 0;
    nthol = 0;
    for (i=0; i<nh; i++) {
        ntpt += preca[i]->npt;
        nthol += 1;
    }

/*  allocate memory for temporary union result arrays  */

    if (ntpt < 1000) ntpt = 1000;
    if (nthol < 20) nthol = 20;
    nbyte = multmem * ntpt * sizeof (double) * 2;

    xtmp = (double *)csw_mem_obj.csw_StackMalloc (nbyte);
    if (!xtmp) {
        return -1;
    }

    noffset = multmem * ntpt;
    ytmp = xtmp + noffset;

    itmp = (int *)csw_mem_obj.csw_StackMalloc (multmem * nthol * sizeof(int *));
    if (!itmp) {
        return -1;
    }

    nbyte = multmem * nthol * sizeof(double *) * 4;

    xst = (double **)csw_mem_obj.csw_StackMalloc (nbyte);
    if (!xst) {
        return -1;
    }

    noffset = multmem * nthol;
    yst = xst + noffset;
    xct = yst + noffset;
    yct = xct + noffset;

    nbyte = multmem * nthol * sizeof(double) * 4;
    xmint = (double *)csw_mem_obj.csw_StackMalloc (nbyte);
    if (!xmint) {
        return -1;
    }

    noffset = multmem * nthol;
    ymint = xmint + noffset;
    xmaxt = ymint + noffset;
    ymaxt = xmaxt + noffset;

/*  set component functions to do nothing while holes are oring  */

    ply_compdata_obj.ply_compnoop ('y');

/*  check each hole to see if it intersects any other hole  */

    *nhr = 0;
    nex = 0;
    ex = extrax;
    ey = extray;
    extot = 0;
    if (nextra) {
        *nextra = 0;
    }
    
    for (i=0; i<nh; i++) {

        if (preca[i]->iflag) {
            continue;
        }

        sctmp = preca[i]->scflag;
        xmint[0] = xmin[i];
        ymint[0] = ymin[i];
        xmaxt[0] = xmax[i];
        ymaxt[0] = ymax[i];
        xt1 = xmin[i];
        yt1 = ymin[i];
        xt2 = xmax[i];
        yt2 = ymax[i];

        xst[0] = preca[i]->xpoly;
        yst[0] = preca[i]->ypoly;

        j = -1;
        for (;;) {

            j++;

            if (j == i) {
                continue;
            }

            if (j >= nh) {
                break;
            }

            if (preca[j]->iflag) {
                continue;
            }

            if (sctmp == preca[j]->scflag) {
                continue;
            }

            if (xmin[j] > xt2  ||
                ymin[j] > yt2  ||
                xmax[j] < xt1  ||
                ymax[j] < yt1 ) {
                continue;
            }

            istat = ply_traverse_obj.ply_trvint
                               (preca[i]->xpoly, preca[i]->ypoly,
                                preca[i]->npt, 0, &nextv, i,
                                xdum, ydum, 0,
                                preca[j]->xpa, preca[j]->ypa,
                                1, preca[j]->ic,
                                &xmin[j], &ymin[j], &xmax[j], &ymax[j],
                                &xdum2, &ydum2, &ndum, &ndum2,
                                &xdum3, &ydum3, &ndum3, 0);

            if (istat == 1) {
                continue;
            }
            else {

                xct[0] = preca[j]->xpoly;
                yct[0] = preca[j]->ypoly;
        
                istat = ply_orcomps (preca[i]->xpoly, preca[i]->ypoly, 
                                     preca[i]->nc, preca[i]->ic,
                                     xst, yst, xmint, ymint, xmaxt, ymaxt,
                                     preca[j]->xpoly, preca[j]->ypoly,
                                     1, preca[j]->ic,
                                     xct, yct, &xmin[j], &ymin[j], 
                                     &xmax[j], &ymax[j],
                                     xhr, yhr, &itemp, ihr, iphr);

        /*
            Put the first component of the or calculation into
            the intermediate output arrays.
        */
                xtmpsav = xtmp;
                ytmpsav = ytmp;
                iphrsav = iphr;
                xhrsav = xhr;
                yhrsav = yhr;

                ply_utils_obj.ply_copy (xhr, yhr, *iphr, xtmp, ytmp);
                xhr += *iphr;
                yhr += *iphr;
                xtmp += *iphr;
                ytmp += *iphr;
                iphr++;

        /*
            Put any other components into the extra arrays.  These
            need to be intersected with the original source and
            clip components before including them in the final output.
        */
                if (extrax != NULL) {
                    for (jj=1; jj<*ihr; jj++) {
                        if (extot + *iphr >= maxextra  ||
                            nex >= maxextraholes) {
                            return -1;
                        }
                        ply_utils_obj.ply_copy (xhr, yhr, *iphr, ex, ey);
                        xhr += *iphr;
                        yhr += *iphr;
                        ex += *iphr;
                        ey += *iphr;
                        extot += *iphr;
                        extraholes[nex] = *iphr;
                        iphr++;
                        nex++;
                        if (nextra)
                            *nextra = nex;
                    }
                    *ihr = 1;
                }

                xtmp = xtmpsav;
                ytmp = ytmpsav;
                xhr = xhrsav;
                yhr = yhrsav;
                iphr = iphrsav;

                preca[i]->xpoly = xtmp;
                preca[i]->ypoly = ytmp;
                preca[i]->nc = 1;
                preca[i]->ic = iphr;
                xst[0] = preca[i]->xpoly;
                yst[0] = preca[i]->ypoly;


/*              new xy limits for new source polygon  */

                istat = ply_utils_obj.ply_limits (preca[i]->xpoly, preca[i]->ypoly,
                                    preca[i]->ic, preca[i]->nc,
                                    xmint, ymint, xmaxt, ymaxt,
                                    &xt1, &yt1, &xt2, &yt2);

                preca[j]->iflag = 1;
                preca[i]->iflag = 1;
                sctmp = 'm';
                preca[i]->scflag = 'm';
                preca[j]->scflag = 'm';
                j = -1;
            }
        }

/*      increment the output pointers  */

        if (preca[i]->iflag) {
            for (jj=0; jj<*ihr; jj++) {
                xhr += *iphr;
                yhr += *iphr;
                iphr++;
            }
            ihr++;
            (*nhr)++;
        }

    }  /*  end of loop that checks each hole for intersection  */

/*  set the components touched by union operation  */

    ply_compdata_obj.ply_compnoop ('n');

    for (i=0; i<nh; i++) {
        
        if (preca[i]->iflag) {
            ply_compdata_obj.ply_compflag (preca[i]->scf2);
            ply_compdata_obj.ply_compset  (preca[i]->icomp);
        }
    }

/*  reverse the orientation of input holes back to original  */

    for (i=0; i<nh; i++) {
        ply_utils_obj.ply_revers (preca[i]->xpoly, preca[i]->ypoly,
                    preca[i]->npt);
    }


    return 0;

}  /*  end of function ply_holunion  */
