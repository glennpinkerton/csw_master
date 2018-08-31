/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*

    con_xsect.cc

    This file has the member functions of the CSWConXsect class.

*/

/*
    system include files
*/
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <math.h>

/*
    csw include files
*/
#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/utils/private_include/ply_protoP.h"
#include "csw/utils/private_include/gpf_font.h"
#include "csw/utils/include/gpf_graph.h"
#include "csw/utils/private_include/ply_graph.h"
#include "csw/utils/private_include/gpf_graphP.h"

#include "csw/surfaceworks/include/grid_api.h"
#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/con_xsect.h"




/* 
  ****************************************************************************

                  c o n _ c a l c _ c r o s s _ s e c t i o n

  ****************************************************************************

    Calculate lines, polygon fills and text for cross sections through
  multiple grids.  This is the internal function.  The API function for
  this is con_CalcCrossSection, found in con_api.c.

*/

int CSWConXsect::con_calc_cross_section
                           (GRidStruct *input_gridlist, int ngrid,
                            CRossSectionTraceStruct *sectionlist, int nsection,
                            CRossSectionLineStruct **lines, int *nlines,
                            CRossSectionFillStruct **fills, int *nfills,
                            CSW_F page_units_per_inch)

{
    bool                      b_success = false;
    int                       i, j, k, n, n2, isect, npts, maxpts;
    int                       nlineout, nfillout, maxfillout,
                              nfilltmp, istat, offset;
    double                    dxt, dyt, ddt, ddx, ddy, ddmax,
                              *xpts = NULL, *ypts = NULL, *zpts = NULL,
                              *dpts = NULL, *zlast = NULL;
    CSW_F                     xt, yt, xt0, yt0, zdelta, ztiny,
                              *grid = NULL, *gtmp = NULL,
                              dx, dy, dist, danc, dd;
    GRidStruct                *gridlist = NULL, *gptr = NULL, *gptr2 = NULL;
    CRossSectionTraceStruct   *xsptr = NULL;
    CRossSectionLineStruct    *output_lines = NULL, *lptr = NULL, *lptr2 = NULL;
    CRossSectionFillStruct    *output_fills = NULL, *filltmp = NULL;

    auto fscope = [&]()
    {
        FreeGridList (input_gridlist, gridlist, ngrid);
        if (!b_success) {
            FreeOutputLines (output_lines, ngrid * nsection);
            FreeOutputFills (output_fills, nfillout);
        }
        csw_Free (Xwork);
        UNSHIFT
        xpts = NULL;
        ypts = NULL;
        zpts = NULL;
        dpts = NULL;
        zlast = NULL;
        grid = NULL;
        gtmp = NULL;
        Xwork = Ywork = Dwork = Zwork = NULL;
        MaxWork = 0;
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
    Set the output pointers to nulls and zeros in
    case an error occurs.
*/
    *lines = NULL;
    *fills = NULL;
    *nlines = 0;
    *nfills = 0;

/*
    Check for obvious errors
*/
    if (input_gridlist == NULL) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }
    if (ngrid < 1) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }
    if (sectionlist == NULL) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }
    if (nsection < 1) {
        grd_utils_ptr->grd_set_err (5);
        return -1;
    }
    if (lines == NULL  ||  nlines == NULL  ||
        fills == NULL  ||  nfills == NULL) {
        grd_utils_ptr->grd_set_err (6);
        return -1;
    }

/*
    Check for hard null values in any of the grids
*/
    for (i=0; i<ngrid; i++) {
        gptr = input_gridlist + i;
        n = gptr->ncol * gptr->nrow;
        grid = gptr->grid;
        for (k=0; k<n; k++) {
            if (grid[k] < MINUS_NULL  ||
                grid[k] > PLUS_NULL) {
                grd_utils_ptr->grd_set_err (7);
                return -1;
            }
        }
    }

    PageUnitsPerInch = page_units_per_inch;

/*
    Find the min and max z values in all the grids.
*/
    Zmin = 1.e30f;
    Zmax = -1.e30f;
    Xmin = input_gridlist[0].x1;
    Xmax = input_gridlist[0].x2;
    Ymin = input_gridlist[0].y1;
    Ymax = input_gridlist[0].y2;
    for (i=0; i<ngrid; i++) {
        gptr = input_gridlist + i;
        if (gptr->x1 > Xmin) Xmin = gptr->x1;
        if (gptr->x2 < Xmax) Xmax = gptr->x2;
        if (gptr->y1 > Ymin) Ymin = gptr->y1;
        if (gptr->y2 < Ymax) Ymax = gptr->y2;
        n = gptr->ncol * gptr->nrow;
        grid = gptr->grid;
        for (k=0; k<n; k++) {
            if (grid[k] < Zmin) Zmin = grid[k];
            if (grid[k] > Zmax) Zmax = grid[k];
        }
    }

    ztiny = (Zmax - Zmin) / 10000.0f;

/*
    initialize some internal pointers and counters
    to suppress some warnings
*/
    nfillout = 0;
    nlineout = 0;
    output_fills = NULL;
    output_lines = NULL;

/*
    Save the list parameters in private class variables to
    be used in shifting and unshifting the coordinates of
    grids and traces.  The shifting is done to make sure
    that internal float calculations using the coordinates
    is precise enough.
*/
    InputTraceList = sectionlist;
    InputGridList = input_gridlist;
    NumTrace = nsection;
    NumGrid = ngrid;

/*
    Find the bounding box of all cross section traces.
    Also find the maximum distance and maximum number
    of points along any trace.
*/
    Xmin = 1.e30f;
    Ymin = 1.e30f;
    Xmax = -1.e30f;
    Ymax = -1.e30f;
    ddmax = 0.0f;
    maxpts = 0;
    for (i=0; i<nsection; i++) {
        xsptr = sectionlist + i;
        ddt = 0.0f;
        if (xsptr->npts > maxpts) maxpts = xsptr->npts;
        for (j=0; j<xsptr->npts; j++) {
            dxt = xsptr->x[j];
            dyt = xsptr->y[j];
            if (dxt < Xmin) Xmin = dxt;
            if (dyt < Ymin) Ymin = dyt;
            if (dxt > Xmax) Xmax = dxt;
            if (dyt > Ymax) Ymax = dyt;
            if (j > 0) {
                ddx = dxt - xsptr->x[j-1];
                ddy = dyt - xsptr->y[j-1];
                ddx *= ddx;
                ddy *= ddy;
                ddt += sqrt(ddx + ddy);
            }
        }
        if (ddt > ddmax) ddmax = ddt;
    }

    Xspace = 1.e30f;
    Yspace = 1.e30f;

/*
    Use the minimum spacing of any grid to set the distance 
    spacing for sampling along cross section traces.
*/
    for (i=0; i<ngrid; i++) {
        gptr = input_gridlist + i;
        dxt = gptr->x2 - gptr->x1;
        dyt = gptr->y2 - gptr->y1;
        dxt /= (double)(gptr->ncol - 1);
        dyt /= (double)(gptr->nrow - 1);
        if (dxt < Xspace) Xspace = dxt;
        if (dyt < Yspace) Yspace = dyt;
    }

    Xmin -= Xspace;
    Ymin -= Yspace;
    Xmax += Xspace;
    Ymax += Yspace;
    Ncol = (int)((Xmax - Xmin) / Xspace + 1.5f);
    Nrow = (int)((Ymax - Ymin) / Yspace + 1.5f);
    if (Ncol < 2) Ncol = 2;
    if (Nrow < 2) Nrow = 2;
    Xspace = (Xmax - Xmin) / (double)(Ncol - 1);
    Yspace = (Ymax - Ymin) / (double)(Nrow - 1);
    Dspace = (Xspace + Yspace) / 2.0f;

/*
    Shift all x,y coordinates to be relative to the Xmin, Ymin
    calculated above.  This insures that subsequent single 
    precision calculations are precise enough.
*/
    ShiftGrids ();
    ShiftXsects ();
    XminSav = Xmin;
    YminSav = Ymin;
    Xmax -= Xmin;
    Ymax -= Ymin;
    Xmin = 0.0f;
    Ymin = 0.0f;

/*
    Make an internal grid list.  If any input grid does not completely
    enclose the bounding box of the cross sections, resample it and
    put its data into the internal grid list.  If the input grid does
    enclose the cross section bounding box, just copy it verbatim into
    the internal grid list.

    Calloc is used here because any new resampled grids need to be csw_Freed
    by FreeGridList, and it needs NULL pointers in any structures that 
    have not been processed.
*/
    gridlist = (GRidStruct *)csw_Calloc (ngrid * sizeof(GRidStruct));
    if (gridlist == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    for (i=0; i<ngrid; i++) {
        gptr = input_gridlist + i;
        gptr2 = gridlist + i;
        if (Xmin >= gptr->x1  &&  Ymin >= gptr->y1  &&
            Xmax <= gptr->x2  &&  Ymax <= gptr->y2) {
            memcpy (gptr2, gptr, sizeof(GRidStruct));
        }
        else {
            gtmp = (CSW_F *)csw_Malloc (Ncol * Nrow * sizeof(CSW_F));
            if (gtmp == NULL) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            istat = grd_arith_ptr->grd_resample_grid
                                      (gptr->grid, NULL,
                                       gptr->ncol, gptr->nrow,
                                       (CSW_F)gptr->x1, (CSW_F)gptr->y1,
                                       (CSW_F)gptr->x2, (CSW_F)gptr->y2,
                                       gptr->faults, gptr->nfaults,
                                       gtmp, NULL,
                                       Ncol, Nrow,
                                       (CSW_F)Xmin, (CSW_F)Ymin,
                                       (CSW_F)Xmax, (CSW_F)Ymax,
                                       GRD_BICUBIC);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            gptr2->grid = gtmp;
            gptr2->ncol = Ncol;
            gptr2->nrow = Nrow;
            gptr2->x1 = Xmin;
            gptr2->y1 = Ymin;
            gptr2->x2 = Xmax;
            gptr2->y2 = Ymax;
            gptr2->type = gptr->type;
            gptr2->id = gptr->id;
            strncpy (gptr2->name, gptr->name, CON_NAME_LENGTH-1);
        }
    }

/*
    Allocate work space for enough sampling points to fill the
    longest cross section trace.
*/
    n = (int)(ddmax / Dspace);
    n += maxpts;
    n += 10;

    if (n > MaxWork) {
        Xwork = (CSW_F *)csw_Realloc (Xwork, n*4*sizeof(CSW_F));
        if (Xwork == NULL) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        else {
            Ywork = Xwork + n;
            Zwork = Ywork + n;
            Dwork = Zwork + n;
            MaxWork = n;
        }
    }

/*
    Allocate as many output line structures as there are grids * sections.
*/
    output_lines = (CRossSectionLineStruct *)csw_Calloc
           (ngrid * nsection * sizeof(CRossSectionLineStruct));
    if (output_lines == NULL) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    Process one cross section at a time.
*/
    nfillout = 0;
    maxfillout = 0;
    nlineout = 0;
    offset = 0;
    for (isect=0; isect<nsection; isect++) {

        xsptr = sectionlist + isect;

    /*
        Generate x,y points along the cross section trace.
    */
        npts = xsptr->npts;
        n2 = 0;
        danc = 0.0f;
        dist = 0.0f;

        xt0 = (CSW_F)xsptr->x[0];
        yt0 = (CSW_F)xsptr->y[0];
        for (i=1; i<npts; i++) {
            xt = (CSW_F)xsptr->x[i];
            yt = (CSW_F)xsptr->y[i];
            dx = xt - xt0;
            dy = yt - yt0;
            dist = dx * dx + dy * dy;
            dist = (CSW_F)sqrt ((double)dist);
            n = (int)(dist / Dspace + .5f);
            dx /= (CSW_F)n;
            dy /= (CSW_F)n;
            dd = dist / (CSW_F)n;
            for (j=0; j<n; j++) {
                Xwork[n2] = xt0 + j * dx;
                Ywork[n2] = yt0 + j * dy;
                Dwork[n2] = danc + j * dd;
                n2++;
            }
            danc += dist;
            xt0 = xt;
            yt0 = yt;
        }
        Xwork[n2] = xt0;
        Ywork[n2] = yt0;
        Dwork[n2] = danc;
        n2++;

    /*
        Back interpolate each grid at each point generated 
        along the cross section trace.  When a particular
        grid is done, put it into the next available output
        line structure.
    */
        zlast = NULL;
        for (i=0; i<ngrid; i++) {
            gptr = gridlist + i;
            grd_arith_ptr->grd_back_interpolate (gptr->grid, 
                                  gptr->ncol,
                                  gptr->nrow,
                                  (CSW_F)gptr->x1,
                                  (CSW_F)gptr->y1,
                                  (CSW_F)gptr->x2,
                                  (CSW_F)gptr->y2,
                                  gptr->faults,
                                  gptr->nfaults,
                                  Xwork, Ywork, Zwork, n2,
                                  GRD_BICUBIC);
            xpts = (double *)csw_Malloc (4 * n2 * sizeof(double));
            if (xpts == NULL) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            ypts = xpts + n2;
            zpts = ypts + n2;
            dpts = zpts + n2;
            MovePoints (xpts, ypts, zpts, dpts, n2);

        /*
            If any point in the line is higher than the point in the
            last line, set it to the value of the last line point.
            This is needed because differing grid geometries on
            input can produce crossing horizons.
        */
            if (zlast) {
                for (j=0; j<n2; j++) {
                    if (zpts[j] > zlast[j]) {
                        zpts[j] = zlast[j];
                    }
                    else {
                        zdelta = (CSW_F)(zpts[j] - zlast[j]);
                        if (zdelta < 0.0f) zdelta = -zdelta;
                        if (zdelta < ztiny) zpts[j] = zlast[j];
                    }
                }
            }
            zlast = zpts;
            
            lptr = output_lines + nlineout;
            lptr->x = xpts;
            lptr->y = ypts;
            lptr->z = zpts;
            lptr->dist = dpts;
            lptr->npts = n2;
            lptr->traceid = xsptr->id;
            lptr->gridid = gptr->id;
            lptr->gridtype = gptr->type;
            strncpy (lptr->tracename, xsptr->name, CON_NAME_LENGTH-1);
            strncpy (lptr->gridname, gptr->name, CON_NAME_LENGTH-1);
            nlineout++;
            lptr = output_lines + nlineout;
        }

    /*
        Calculate polygons between adjacent surfaces and put
        them into the output_fills array.
    */
        for (i=0; i<ngrid-1; i++) {
            lptr = output_lines + offset + i;
            lptr2 = lptr + 1;
            istat = con_calc_cross_section_polygons (lptr, lptr2,
                                                     &filltmp, &nfilltmp, -1.0f);
            if (istat == -1) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            while (nfillout + nfilltmp > maxfillout) {
                maxfillout += _FILL_CHUNK;
                output_fills = (CRossSectionFillStruct *) csw_Realloc
                               (output_fills, maxfillout * sizeof(CRossSectionFillStruct));
            }
            if (!output_fills) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            memcpy (output_fills + nfillout, filltmp,
                    nfilltmp * sizeof(CRossSectionFillStruct));
            nfillout += nfilltmp;
            csw_Free (filltmp);
            filltmp = NULL;
            nfilltmp = 0;
        }
        offset = nlineout;
    }

/*
    Clean up and return success.
*/
    *lines = output_lines;
    *fills = output_fills;
    *nlines = nlineout;
    *nfills = nfillout;

    b_success = true;

    return 1;

}  /*  end of function con_calc_cross_section  */




/* 
  ****************************************************************************

                          F r e e G r i d L i s t

  ****************************************************************************

    Free the internal grid list and any resampled arrays in it.  The first
    grid list is the input grid list.  If any grid pointer in the second
    grid list (the internal list) does not match the first list's pointer,
    it needs to be csw_Freed.  This is all put into this function because it
    needs to be done as part of error returns from several places in 
    con_calc_cross_section.

    This must only be used just before returning from the calling function,
    because the list pointer is csw_Freed here.

*/

int CSWConXsect::FreeGridList
    (GRidStruct *sorted_list, GRidStruct *list, int ngrid)
{
    int                 i;

    for (i=0; i<ngrid; i++) {
        if (list[i].grid == NULL) break;
        if (sorted_list[i].grid != list[i].grid) {
            csw_Free (list[i].grid);
            list[i].grid = NULL;
        }
    }

    csw_Free (list);

    return 1;

}  /*  end of private FreeGridList function  */





/* 
  ****************************************************************************

                        F r e e O u t p u t L i n e s

  ****************************************************************************

    Free the output line list and all the data inside it.  This is called 
  if an error return happens in calculating cross sections and also from
  application command via con_FreeCrossSectionData.

*/

int CSWConXsect::FreeOutputLines (CRossSectionLineStruct *list, int nlist)
{
    int                          i;

    if (list == NULL) return 1;

    for (i=0; i<nlist; i++) {
       if (list[i].x) csw_Free (list[i].x);
       list[i].x = NULL;
    }
    csw_Free (list);

    return 0; 

}  /*  end of private FreeOutputLines function  */




/* 
  ****************************************************************************

                        F r e e O u t p u t F i l l s

  ****************************************************************************

    Free the output fill list and all the data inside it.  This is called 
  if an error return happens in calculating cross sections and also from
  application command via con_FreeCrossSectionData.

*/

int CSWConXsect::FreeOutputFills (CRossSectionFillStruct *list, int nlist)
{
    int                          i;

    if (list == NULL) return 1;

    for (i=0; i<nlist; i++) {
       if (list[i].dist) csw_Free (list[i].dist);
       list[i].dist = NULL;
    }
    csw_Free (list);

    return 0; 

}  /*  end of private FreeOutputFills function  */







/* 
  ****************************************************************************

                          M o v e P o i n t s

  ****************************************************************************

    Copy from the float Xwork, Ywork, Dwork and Zwork arrays into double
    arrays specified in the parameter list.  In some cases, only part of
    the work arrays need to be copied.  In these cases, the calling function
    should pass NULL pointers for inactive arrays.  

*/

int CSWConXsect::MovePoints
                      (double *x, double *y,
                       double *z, double *d, int n)
{
    int           i;

    if (x) {
        for (i=0; i<n; i++) {
            x[i] = (double)Xwork[i] + XminSav;
        }
    }

    if (y) {
        for (i=0; i<n; i++) {
            y[i] = (double)Ywork[i] + YminSav;
        }
    }

    if (z) {
        for (i=0; i<n; i++) {
            z[i] = (double)Zwork[i];
        }
    }

    if (d) {
        for (i=0; i<n; i++) {
            d[i] = (double)Dwork[i];
        }
    }

    return 1;

}  /*  end of private MovePoints function  */




/* 
  ****************************************************************************

                            S h i f t G r i d s

  ****************************************************************************

    Shift the x and y limits of all the input grids by subtracting Xmin and
  Ymin from them.  This puts all coordinates into a range that can be handled
  by single precision library routines.

*/

int CSWConXsect::ShiftGrids (void)
{
    int            i;

    for (i=0; i<NumGrid; i++) {
        InputGridList[i].x1 -= Xmin;
        InputGridList[i].y1 -= Ymin;
        InputGridList[i].x2 -= Xmin;
        InputGridList[i].y2 -= Ymin;
    }

    return 1;

}  /*  end of private ShiftGrids function  */





/* 
  ****************************************************************************

                            U n s h i f t G r i d s

  ****************************************************************************

    Shift the grid limits in the input grid list back to their original values
    by adding XminSav and YminSav to them.
*/

int CSWConXsect::UnshiftGrids (void)
{
    int            i;

    for (i=0; i<NumGrid; i++) {
        InputGridList[i].x1 += XminSav;
        InputGridList[i].y1 += YminSav;
        InputGridList[i].x2 += XminSav;
        InputGridList[i].y2 += YminSav;
    }

    return 1;

}  /*  end of private UnshiftGrids function  */






/* 
  ****************************************************************************

                          S h i f t X s e c t s

  ****************************************************************************

    Shift all the x and y coordinates of the cross section list by subtracting
  Xmin and Ymin from them.  This is done to support single precision arithmetic
  in library routines used to calculate the cross section.

*/

int CSWConXsect::ShiftXsects (void)
{
    int                          i, j;
    CRossSectionTraceStruct      *xsptr;

    for (i=0; i<NumTrace; i++) {
        xsptr = InputTraceList + i;
        for (j=0; j<xsptr->npts; j++) {
            xsptr->x[j] -= Xmin;
            xsptr->y[j] -= Ymin;
        }
    }

    return 1;

}  /*  end of ShiftXsects function  */







/* 
  ****************************************************************************

                          U n s h i f t X s e c t s

  ****************************************************************************

    Shift the cross section coordinates back to their original values by adding
  XminSav and YminSav to the x and y coordinates respectively.
*/

int CSWConXsect::UnshiftXsects (void)
{
    int                          i, j;
    CRossSectionTraceStruct      *xsptr;

    for (i=0; i<NumTrace; i++) {
        xsptr = InputTraceList + i;
        for (j=0; j<xsptr->npts; j++) {
            xsptr->x[j] += XminSav;
            xsptr->y[j] += YminSav;
        }
    }

    return 1;

}  /*  end of UnshiftXsects function  */




/* 
  ****************************************************************************

       c o n _ c a l c _ c r o s s _ s e c t i o n _ p o l y g o n s

  ****************************************************************************

    Calculate polygons formed between two surface lines in a cross section.

    Each surface line is made into a polygon by squaring off the line
    endpoints in a fashion that will cause the two polygons to intersect.
    For example, if line 1 is generally greater than line 2, line 1 is squared
    to a value less than the minimum of line 2 and line 2 is squared to a
    value greater than the maximum of line 1.  Three points are added for the
    "squaring off".  The first point is at the same x ans the last line point
    and an appropriate y value to produce intersecting polygons.  The second
    point is at the same x as the first line point, and the same y as the 
    previously added point.  The third point added is at the same location
    as the first line point, thus closing the polygon.

    The polygons are intersected, and the results are the polygon set "between" 
    the two surface lines.

*/

int CSWConXsect::con_calc_cross_section_polygons
                                    (CRossSectionLineStruct *line1,
                                     CRossSectionLineStruct *line2,
                                     CRossSectionFillStruct **fills,
                                     int *nfills, 
                                     CSW_F page_units_per_inch)

{
    int                       istat, i, j, k, nc, nc2, n, start, n1, n2, nmax;
    double                    *xp1 = NULL, *yp1 = NULL, *xp2 = NULL, *yp2 = NULL,
                              zgap, *xptr = NULL, *yptr = NULL,
                              minval, maxval, minval1, maxval1,
                              minval2, maxval2, *xout = NULL, *yout = NULL, tiny;
    int                       *compout = NULL, *holeout = NULL, ncomp;
    double                    area, areacheck, xt;
    CRossSectionFillStruct    *ftmp = NULL;

    std::unique_ptr<CSWPolyGraph>
        ply_graph_obj {new CSWPolyGraph ()};


    auto fscope = [&]()
    {
        csw_Free (xp1);
        csw_Free (xout);
        csw_Free (compout);
        FreeOutputFills (ftmp, ncomp);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Check obvious errors.
*/
    if (line1 == NULL  ||  line2 == NULL) {
        grd_utils_ptr->grd_set_err (2);;
        return -1;
    }
    if (fills == NULL  ||  nfills == NULL) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

/*
    allocate work space for the polygons used for clipping
*/
    nmax = line1->npts;
    if (line2->npts > nmax) nmax = line2->npts;
    nmax += 10;

    xp1 = (double *)csw_Malloc (nmax * 4 * sizeof(double));
    if (!xp1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    yp1 = xp1 + nmax;
    xp2 = yp1 + nmax;
    yp2 = xp2 + nmax;

/*
    allocate space for the clipping results.
*/
    nmax *= 10;
    xout = (double*)csw_Malloc (nmax * 4 * sizeof(double));
    if (!xout) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }
    yout = xout + nmax * 2;

    compout = (int *)csw_Malloc (nmax * 2 * sizeof(int));
    if (!compout) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    if (page_units_per_inch > 0.0) {
        PageUnitsPerInch = page_units_per_inch;
    }

    holeout = compout + nmax;

/*
    find the minimum and maximum z values of both lines
*/
    minval1 = 1.e30f;
    minval2 = 1.e30f;
    maxval1 = -1.e30f;
    maxval2 = -1.e30f;

    for (i=0; i<line1->npts; i++) {
        if (line1->z[i] < minval1) {
            minval1 = line1->z[i];
        }
        if (line1->z[i] > maxval1) {
            maxval1 = line1->z[i];
        }
    }
    
    for (i=0; i<line2->npts; i++) {
        if (line2->z[i] < minval2) {
            minval2 = line2->z[i];
        }
        if (line2->z[i] > maxval2) {
            maxval2 = line2->z[i];
        }
    }

/*
    If line1 is greater than line 2 numerically, use the overall minimum
    to square off line 1 and the overall maximum to square off line 2.
    If line 1 is numerically less than line 2, use the overall maximum
    to square off line 1 and the overall minimum to square off line 2.
*/
    tiny = (double)(Zmax - Zmin) / 10000.0f;
    zgap = minval1 - maxval2;
    if (zgap < 0.0f) zgap = -zgap;
    zgap /= 10.0f;
    if (minval1 < minval2 - tiny  ||  maxval1 < maxval2 - tiny) {
        minval = maxval2 + zgap;
        maxval = minval1 - zgap;
    }
    else {
        minval = minval2 - zgap;
        maxval = maxval1 + zgap;
    }

    areacheck = (maxval - minval) * line1->dist[line1->npts-1];
    areacheck /= 10000.0f;

/*
    Generate the polygons from line1 and line2.
*/
    memcpy (xp1, line1->dist, line1->npts*sizeof(double));
    memcpy (yp1, line1->z, line1->npts*sizeof(double));
    n1 = line1->npts;
    xp1[n1] = xp1[n1-1];
    yp1[n1] = minval;
    n1++;
    xp1[n1] = xp1[0];
    yp1[n1] = minval;
    n1++;
    xp1[n1] = xp1[0];
    yp1[n1] = yp1[0];
    n1++;

    memcpy (xp2, line2->dist, line2->npts*sizeof(double));
    memcpy (yp2, line2->z, line2->npts*sizeof(double));
    n2 = line2->npts;
    xp2[n2] = xp2[n2-1];
    yp2[n2] = maxval;
    n2++;
    xp2[n2] = xp2[0];
    yp2[n2] = maxval;
    n2++;
    xp2[n2] = xp2[0];
    yp2[n2] = yp2[0];
    n2++;

    nc = 1;
    nc2 = 1;
    istat = ply_graph_obj->ply_boolean (xp1, yp1, NULL, 1, &nc, &n1,
                         xp2, yp2, NULL, 1, &nc2, &n2,
                         PLY_INTERSECT, 
                         xout, yout, NULL, &ncomp, compout, holeout,
                         nmax*2, nmax);
    csw_Free (xp1);
    if (istat == -1) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

/*
    allocate the output fill arrays and stuff the results
    into them.
*/
    ftmp = (CRossSectionFillStruct *)csw_Calloc 
              (ncomp * sizeof(CRossSectionFillStruct));
    if (!ftmp) {
        grd_utils_ptr->grd_set_err (1);
        return -1;
    }

    start = 0;
    i = 0;
    for (j=0; j<ncomp; j++) {
        n = holeout[j];
        xptr = (double *)csw_Malloc (n * 2 * sizeof(double));
        if (!xptr) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        yptr = xptr + n;
        memcpy (xptr, xout+start, n*sizeof(double));
        memcpy (yptr, yout+start, n*sizeof(double));
        start += n;
    
    /*
        If the area of this component is much smaller than the
        area of the bounding box to the input lines, do not
        output the component.
    */
        area = 0.0f;
        for (k=0; k<n-1; k++) {
            xt = xptr[k] * yptr[k+1] - xptr[k+1] * yptr[k];
            area += xt;
        }
        area /= 2.0f;
        if (area < 0.0f) area = -area;
        if (area < areacheck) {
            csw_Free (xptr);
            xptr = yptr = NULL;
            continue;
        }

        ftmp[i].dist = xptr;
        ftmp[i].z = yptr;
        ftmp[i].npts = n;
        ftmp[i].traceid = line1->traceid;
        strncpy (ftmp[i].tracename, line1->tracename, CON_NAME_LENGTH-1);
        ftmp[i].topgridid = line1->gridid;
        ftmp[i].topgridtype = line1->gridtype;
        strncpy (ftmp[i].topgridname, line1->gridname, CON_NAME_LENGTH-1);
        ftmp[i].bottomgridid = line2->gridid;
        ftmp[i].bottomgridtype = line2->gridtype;
        strncpy (ftmp[i].bottomgridname, line2->gridname, CON_NAME_LENGTH-1);
        i++;
    }

    *fills = ftmp;
    *nfills = i;

    return 1;

}  /*  end of function con_calc_cross_section_polygons  */



/* 
  ****************************************************************************

               c o n _ f r e e _ c r o s s _ s e c t i o n

  ****************************************************************************

    Free lines and fills returned from con_calc_cross_section.  If
  a pointer is null or the number of items is zero, ignore the pointer.

*/

int CSWConXsect::con_free_cross_section
                           (CRossSectionLineStruct *lines, int nlines,
                            CRossSectionFillStruct *fills, int nfills)
{
    int                        i;

    if (lines  &&  nlines > 0) {
        for (i=0; i<nlines; i++) {
            if (lines[i].x) csw_Free (lines[i].x);
            lines[i].x = NULL;
        }
        csw_Free (lines);
    }

    if (fills  &&  nfills > 0) {
        for (i=0; i<nfills; i++) {
            if (fills[i].dist) csw_Free (fills[i].dist);
            fills[i].dist = NULL;
        }
        csw_Free (fills);
    }

    return 1;

}  /*  end of function con_free_cross_section  */





/* 
  ****************************************************************************

         c o n _ f r e e _ c r o s s _ s e c t i o n _ d r a w i n g

  ****************************************************************************

    Free the line and fill primitives returned from con_draw_cross_section.

*/

int CSWConXsect::con_free_cross_section_drawing
                           (CRossSectionLinePrimitive *lines, int nlines,
                            CRossSectionFillPrimitive *fills, int nfills)
{
    int                          i;

    if (lines  &&  nlines > 0) {
        for (i=0; i<nlines; i++) {
            if (lines[i].x) csw_Free (lines[i].x);
            lines[i].x = NULL;
        }
        csw_Free (lines);
    }

    if (fills  &&  nfills > 0) {
        for (i=0; i<nfills; i++) {
            if (fills[i].x) csw_Free (fills[i].x);
            fills[i].x = NULL;
        }
        csw_Free (fills);
    }

    return 1;

}  /*  end of function con_free_cross_section_drawing  */







/**********************************************************************

    The functions below this point all are concerned with drawing the
    cross section via a call to the con_DrawCrossSection api function.

***********************************************************************/





/* 
  ****************************************************************************

                c o n _ d r a w _ c r o s s _ s e c t i o n

  ****************************************************************************

    Return line and fill primitives that can be drawn to make a cross section.
  The input is the lines, text and fills returned by con_CalcCrossSection.

*/

int CSWConXsect::con_draw_cross_section
                          (CRossSectionLineStruct *lines, int nlines,
                           CRossSectionFillStruct *fills, int nfills,
                           int zflag, int traceid,
                           char *xcaption, char *zcaption,
                           CSW_F hscale, CSW_F zscale,
                           CRossSectionLinePrimitive **lprimin, int *nlprimin,
                           CRossSectionFillPrimitive **fprimin, int *nfprimin)
{
    GRaphLineRec               *glines = NULL;
    GRaphTextRec               *gtext = NULL;
    GRaphUnion                 grunion;
    int                        nglines, ngtext;
    CRossSectionLinePrimitive  *lprim = NULL;
    CRossSectionFillPrimitive  *fprim = NULL;
    int                        nlprim, nfprim, maxlprim, maxfprim;
    CSW_F                      *xprim = NULL, *yprim = NULL;
    double                     *xptr = NULL, *yptr = NULL, dmin, dmax, zmin, zmax;
    CSW_F                      tsize1, tsize2, xt, yt,
                               pxmin, pymin, pxmax, pymax;
    CSW_F                      axmin, axmax, aymin, aymax,
                               axmajor, axminor, aymajor, ayminor;
    char                       c100[100], depthcaption[500], distancecaption[500];
    int                        lpsize;
    int                        i, j, n, npts, istat, len;

    GPFGraph     gpf_graph_obj;

    auto fscope = [&]()
    {
        FreeLinePrims (lprim, nlprim);
        FreeFillPrims (fprim, nfprim);
        gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
        csw_Free (xprim);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
    Check obvious errors.
*/
    if (lines == NULL  ||  fills == NULL  ||
        nlines == 0  ||  nfills == 0) {
        grd_utils_ptr->grd_set_err (2);
        return -1;
    }
    if (lprimin == NULL  ||  nlprimin == NULL  ||
        fprimin == NULL  ||  nfprimin == NULL) {
        grd_utils_ptr->grd_set_err (3);
        return -1;
    }

    lpsize = sizeof(CRossSectionLinePrimitive);

/*
    Copy the captions to local space if not null.  If a caption
    is NULL, use a default instead.  Make sure the copy doesn't
    overflow the 500 character local buffers.
*/
    memset (depthcaption, 0, 500);
    memset (distancecaption, 0, 500);
    if (zcaption) {
        strncpy (depthcaption, zcaption, 499);
    }
    else {
        if (zflag == 1) {
            strcpy (depthcaption, "Elevation");
        }
        else {
            strcpy (depthcaption, "Depth");
        }
    }

    if (xcaption) {
        strncpy (distancecaption, xcaption, 499);
    }
    else {
        strcpy (distancecaption, "Distance Along Cross Section");
    }

/*
    Initialize the output to NULL and zero in case an error occurs.
*/
    *lprimin = NULL;
    *fprimin = NULL;
    *nlprimin = 0;
    *nfprimin = 0;

    lprim = NULL;
    nlprim = 0;
    maxlprim = 0;
    fprim = NULL;
    nfprim = 0;
    maxfprim = 0;

/*
    Find the limits of the distance and z arrays
*/
    dmin = 1.e30f;
    dmax = -1.e30f;
    zmin = 1.e30f;
    zmax = -1.e30f;

    for (i=0; i<nlines; i++) {
        if (traceid != -1) {
            if (lines[i].traceid != traceid) {
                continue;
            }
        }
        n = lines[i].npts;
        xptr = lines[i].dist;
        yptr = lines[i].z;
        for (j=0; j<n; j++) {
            if (xptr[j] < dmin) dmin = xptr[j];
            if (xptr[j] > dmax) dmax = xptr[j];
            if (yptr[j] < zmin) zmin = yptr[j];
            if (yptr[j] > zmax) zmax = yptr[j];
        }
    }
                
    for (i=0; i<nfills; i++) {
        if (traceid != -1) {
            if (fills[i].traceid != traceid) {
                continue;
            }
        }
        n = fills[i].npts;
        xptr = fills[i].dist;
        yptr = fills[i].z;
        for (j=0; j<n; j++) {
            if (xptr[j] < dmin) dmin = xptr[j];
            if (xptr[j] > dmax) dmax = xptr[j];
            if (yptr[j] < zmin) zmin = yptr[j];
            if (yptr[j] > zmax) zmax = yptr[j];
        }
    }

/*
    If dmin is still a huge number, no trace in the input 
    data matched the specified traceid.
*/
    if (dmin > 1.e20f) {
        grd_utils_ptr->grd_set_err (4);
        return -1;
    }

    dmin -= (dmax - dmin) / 100.0f;
    dmax += (dmax - dmin) / 100.0f;

/*
    Make the text sizes larger if the cross section is larger.
*/
    tsize1 = 0.15f;
    tsize2 = 0.10f;

    xt = (CSW_F)(dmax - dmin) / hscale;
    yt = (CSW_F)(zmax - zmin) / zscale;
    xt += yt;

    if (xt > 32) {
        tsize1 = 0.18f;
        tsize2 = 0.12f;
    }
    if (xt > 42) {
        tsize1 = 0.20f;
        tsize2 = 0.15f;
    }
    if (xt > 52) {
        tsize1 = 0.25f;
        tsize2 = 0.18f;
    }
    if (xt > 70) {
        tsize1 = 0.30f;
        tsize2 = 0.20f;
    }

/*
    Find the lower left corner of the graph axes, leaving
    enough room for captions and labels on each axis.
*/
    sprintf (c100, "%f", (CSW_F)zmin);
    len = (int)strlen (c100);
    xt = (CSW_F)len * tsize2;
    sprintf (c100, "%f", (CSW_F)zmax);
    len = (int)strlen (c100);
    yt = (CSW_F)len * tsize2;
    if (yt > xt) xt = yt;
    sprintf (c100, "%f", (CSW_F)(zmax-zmin));
    len = (int)strlen (c100);
    yt = (CSW_F)len * tsize2;
    if (yt > xt) xt = yt;
    xt += tsize1;
    xt *= 1.5f;

    pxmin = xt;
    pymin = (tsize1 + tsize2) * 2.0f;
    
/*
    Calculate axis limits and intervals.  The results
    are "pretty" numbers that lie outside of the actual
    data limits by a small amount.  This has to be done now 
    in order to calculate the upper right corner of the
    cross section axes and pass it on to gpf_StartGraph.
*/
    CalcAxisIntervals (10, (CSW_F)dmin, (CSW_F)dmax, 
                       &axmin, &axmax, &axmajor, &axminor);
    CalcAxisIntervals (7, (CSW_F)zmin, (CSW_F)zmax,
                       &aymin, &aymax, &aymajor, &ayminor);
    pxmax = pxmin + (axmax - axmin) / hscale;
    pymax = pymin + (aymax - aymin) / zscale;

/*
    Set some graph parameters that affect the whole graph.
*/
    grunion.ival = 102;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_FONT, grunion);
    grunion.ival = 102;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_FONT, grunion);
    grunion.fval = tsize2;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_SIZE, grunion);
    grunion.fval = tsize1;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_SIZE, grunion);
    grunion.fval = tsize2 / 10.0f;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_LABEL_THICK, grunion);
    grunion.fval = tsize1 / 10.0f;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_CAPTION_THICK, grunion);

    gpf_graph_obj.gpf_StartGraph (pxmin, pymin, pxmax, pymax, 1.0f, 0, PageUnitsPerInch);

/*
    Calculate line and text primitives for each axis.  After
    each calculation, the graph primitive structures are
    converted to CRossSection primitive structures which are
    then appended to the list of output lines and fills.
    The min, max, major and minor values calculated above need
    to be set prior to the distance axes and prior to the depth
    axes calculations.
*/
    grunion.fval = axmin;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MIN_VALUE, grunion);
    grunion.fval = axmax;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAX_VALUE, grunion);
    grunion.fval = axmajor;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_INTERVAL, grunion);
    grunion.fval = axminor;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_INTERVAL, grunion);
    istat = gpf_graph_obj.gpf_CalcGraphAxis (GPF_TOP_AXIS, distancecaption,
                               axmin, axmax, 0.0f,
                               1, NULL, NULL, 0,
                               &axmin, &axmax, &axmajor,
                               &glines, &nglines, &gtext, &ngtext);
    if (istat == -1) {
        return -1;
    }
    istat = AppendLinesToOutput (glines, nglines, &lprim, &nlprim, &maxlprim);
    gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
    glines = NULL;
    nglines = 0;
    if (istat == -1) {
        return -1;
    }
    istat = AppendTextToOutput (gtext, ngtext, &lprim, &nlprim, &maxlprim);
    gpf_graph_obj.gpf_FreeGraphText (gtext, ngtext);
    if (istat == -1) {
        return -1;
    }
                               
    istat = gpf_graph_obj.gpf_CalcGraphAxis (GPF_BOTTOM_AXIS, NULL,
                               axmin, axmax, 0.0f,
                               0, NULL, NULL, 0,
                               &axmin, &axmax, &axmajor,
                               &glines, &nglines, &gtext, &ngtext);
    if (istat == -1) {
        return -1;
    }
    istat = AppendLinesToOutput (glines, nglines, &lprim, &nlprim, &maxlprim);
    gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
    if (istat == -1) {
        return -1;
    }
    istat = AppendTextToOutput (gtext, ngtext, &lprim, &nlprim, &maxlprim);
    gpf_graph_obj.gpf_FreeGraphText (gtext, ngtext);
    if (istat == -1) {
        return -1;
    }
                               
    grunion.fval = aymin;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MIN_VALUE, grunion);
    grunion.fval = aymax;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAX_VALUE, grunion);
    grunion.fval = aymajor;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MAJOR_INTERVAL, grunion);
    grunion.fval = ayminor;
    gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_MINOR_INTERVAL, grunion);
    if (zflag == -1) {
        grunion.ival = 2;
        gpf_graph_obj.gpf_SetGraphParameter (GPF_AXIS_DIRECTION_FLAG, grunion);
    }
    istat = gpf_graph_obj.gpf_CalcGraphAxis (GPF_LEFT_AXIS, depthcaption,
                               aymin, aymax, 0.0f,
                               1, NULL, NULL, 0,
                               &aymin, &aymax, &aymajor,
                               &glines, &nglines, &gtext, &ngtext);
    if (istat == -1) {
        return -1;
    }
    istat = AppendLinesToOutput (glines, nglines, &lprim, &nlprim, &maxlprim);
    gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
    if (istat == -1) {
        return -1;
    }
    istat = AppendTextToOutput (gtext, ngtext, &lprim, &nlprim, &maxlprim);
    gpf_graph_obj.gpf_FreeGraphText (gtext, ngtext);
    if (istat == -1) {
        return -1;
    }
                               
    istat = gpf_graph_obj.gpf_CalcGraphAxis (GPF_RIGHT_AXIS, NULL,
                               aymin, aymax, 0.0f,
                               0, NULL, NULL, 0,
                               &aymin, &aymax, &aymajor,
                               &glines, &nglines, &gtext, &ngtext);
    if (istat == -1) {
        return -1;
    }
    istat = AppendLinesToOutput (glines, nglines, &lprim, &nlprim, &maxlprim);
    gpf_graph_obj.gpf_FreeGraphLines (glines, nglines, 1);
    if (istat == -1) {
        return -1;
    }
    istat = AppendTextToOutput (gtext, ngtext, &lprim, &nlprim, &maxlprim);
    gpf_graph_obj.gpf_FreeGraphText (gtext, ngtext);
    if (istat == -1) {
        return -1;
    }

/*
    Scale the surface lines into the plot coordinates and 
    output them as part of the surface line layer.
*/
    for (i=0; i<nlines; i++) {
        if (traceid != -1) {
            if (lines[i].traceid != traceid) {
                continue;
            }
        }
        npts = lines[i].npts;
        xptr = lines[i].dist;
        yptr = lines[i].z;
        xprim = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
        if (!xprim) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        yprim = xprim + npts;
        for (j=0; j<npts; j++) {
            xprim[j] = (CSW_F)((xptr[j] - axmin) / hscale + pxmin);
            yprim[j] = (CSW_F)((yptr[j] - aymin) / zscale + pymin);
        }

        if (nlprim >= maxlprim) {
            maxlprim += _LINE_CHUNK;
            lprim = (CRossSectionLinePrimitive *)csw_Realloc (lprim, 
                                                              maxlprim * lpsize);
            if (!lprim) {
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
        }

        lprim[nlprim].x = xprim;
        xprim = NULL;
        lprim[nlprim].y = yprim;
        yprim = NULL;
        lprim[nlprim].npts = npts;
        lprim[nlprim].thick = 0.01f;
        lprim[nlprim].layer = CON_XSECT_SURFACE_LAYER;
        lprim[nlprim].textflag = 0;
        lprim[nlprim].id = lines[i].gridid;

        nlprim++;
    }

/*
    Scale the polygons to plotter units and output.
*/
    for (i=0; i<nfills; i++) {
        if (traceid != -1) {
            if (fills[i].traceid != traceid) {
                continue;
            }
        }
        npts = fills[i].npts;
        xptr = fills[i].dist;
        yptr = fills[i].z;
        xprim = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
        if (!xprim) {
            yprim = NULL;
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        yprim = xprim + npts;
        for (j=0; j<npts; j++) {
            xprim[j] = (CSW_F)((xptr[j] - axmin) / hscale + pxmin);
            yprim[j] = (CSW_F)((yptr[j] - aymin) / zscale + pymin);
        }

        if (nfprim >= maxfprim) {
            maxfprim += _LINE_CHUNK;
            fprim = (CRossSectionFillPrimitive *)csw_Realloc (fprim, 
                                                              maxfprim * lpsize);
        }
        if (!fprim) {
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }

        fprim[nfprim].x = xprim;
        fprim[nfprim].y = yprim;
        fprim[nfprim].npts = npts;
        fprim[nfprim].layer = CON_XSECT_FILL_LAYER;
        fprim[nfprim].topid = fills[i].topgridid;
        fprim[nfprim].bottomid = fills[i].bottomgridid;

        nfprim++;
    }

/*
    All done, return primitives to calling function.
*/
    *lprimin = lprim;
    *nlprimin = nlprim;
    *fprimin = fprim;
    *nfprimin = nfprim;

    return 1;

}  /*  end of function con_draw_cross_section  */






/*
  ****************************************************************

               C a l c A x i s I n t e r v a l s

  ****************************************************************

    Calculate good "even" values for major tick spacing, minor
    tick spacing and the end points of a graph axis.  This is only
    used by con_draw_cross_section.

*/

int CSWConXsect::CalcAxisIntervals
                             (int nmajor,
                              CSW_F datamin,
                              CSW_F datamax,
                              CSW_F *axismin,
                              CSW_F *axismax,
                              CSW_F *axismajor,
                              CSW_F *axisminor)
{
    CSW_F      major, minor, min, max, origdelta, ftmp,
               delta, logdelta, base, mantissa;

    origdelta = (datamax - datamin) / nmajor;
    delta = origdelta;
    if (origdelta < 0.0f) {
        delta = -delta;
        ftmp = datamin;
        datamin = datamax;
        datamax = ftmp;
    }

/*
    get the largest value that is a power of 10 and less than
    the previously calculated delta
*/
    logdelta = (CSW_F)log10 ((double)delta);
    logdelta = (CSW_F)floor ((double)logdelta);
    base = (CSW_F)pow ((double)10.0f, (double)logdelta);
    mantissa = delta / base;

/*
    Use a "round" value fairly near the actual delta value
    for the major interval and adjust the minor interval
    according to the major interval.
*/
    if (mantissa < 1.5f) {
        major = base;
        minor = major / 5.0f;
    }
    else if (mantissa < 3.0f) {
        major = 2.0f * base;
        minor = major / 4.0f;
    }
    else if (mantissa < 4.5f) {
        major = 4.0f * base;
        minor = major / 4.0f;
    }
    else if (mantissa < 7.5f) {
        major = 5.0f * base;
        minor = major / 5.0f;
    }
    else {
        major = 10.0f * base;
        minor = major / 5.0f;
    }

/*
    The axis minimum is the largest value evenly divisible 
    by major and less than the datamin.  Axis max is smallest
    number evenly divisible by major and larger than datamax.
*/
    min = (CSW_F)floor ((double)(datamin/major)) * major;
    max = (CSW_F)ceil ((double)(datamax/major)) * major;

/*
    If this is a reversed direction axis, adjust for that.
*/
    if (origdelta < 0.0f) {
        major = -major;
        minor = -minor;
        ftmp = min;
        min = max;
        max = ftmp;
    }

/*
    Return the calculated values
*/
    *axismin = min;
    *axismax = max;
    *axismajor = major;
    *axisminor = minor;

    return 1;

}  /*  end of private CalcAxisIntervals function  */





/* 
  ****************************************************************************

                  A p p e n d L i n e s T o O u t p u t

  ****************************************************************************

  Append line primitives returned from the graph axis functions to the
  cross section output primitive list.

*/

int CSWConXsect::AppendLinesToOutput
                           (GRaphLineRec *grlines, int ngrlines,
                            CRossSectionLinePrimitive **linesin,
                            int *nlinesin, int *maxlinesin)
{
    int                         i, j, nlines, maxlines, npts;
    CRossSectionLinePrimitive   *lprim = NULL, *lpsav = NULL;
    GRaphLineRec                *glptr = NULL;
    CSW_F                       *xptr = NULL, *yptr = NULL,
                                *xprim = NULL, *yprim = NULL;

    bool    b_success = false;

    auto fscope = [&]()
    {
        if (!b_success) {
            *linesin = lprim;
            *nlinesin = nlines;
            *maxlinesin = maxlines;
        }
        if (lpsav != NULL) {
            for (int ii=0; ii<nlines; ii++) {
                csw_Free (lpsav[ii].x);
                lpsav[ii].x = NULL;
                lpsav[ii].y = NULL;
            }
        }
        csw_Free (xprim);
    };
    CSWScopeGuard func_scope_guard (fscope);

    lprim = *linesin;
    nlines = *nlinesin;
    maxlines = *maxlinesin;

    for (i=0; i<ngrlines; i++) {

        glptr = grlines + i;
        npts = glptr->npts;
        xptr = glptr->x;
        yptr = glptr->y;

        xprim = (CSW_F *)csw_Malloc (npts * 2 * sizeof(CSW_F));
        if (!xprim) {
            lpsav = lprim;
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
        yprim = xprim + npts;

        for (j=0; j<npts; j++) {
            xprim[j] = xptr[j];
            yprim[j] = yptr[j];
        }

        if (nlines >= maxlines) {
            maxlines += _LINE_CHUNK;
            lpsav = lprim;
            lprim = (CRossSectionLinePrimitive *)csw_Realloc
                    (lprim, maxlines * sizeof(CRossSectionLinePrimitive));
            if (!lprim) {
                maxlines -= _LINE_CHUNK;
                grd_utils_ptr->grd_set_err (1);
                return -1;
            }
            lpsav = NULL;
        }

        lprim[nlines].npts = npts;
        lprim[nlines].x = xprim;
        xprim = NULL;
        lprim[nlines].y = yprim;
        yprim = NULL;
        lprim[nlines].thick = glptr->linethick;
        lprim[nlines].layer = CON_XSECT_AXIS_LAYER;
        lprim[nlines].textflag = 0;
        lprim[nlines].id = -1;

        nlines++;

    }

    *nlinesin = nlines;
    *maxlinesin = maxlines;
    *linesin = lprim;

    b_success = true;
    lpsav = NULL;

    return 1;

}  /*  end of private AppendLinesToOutput function  */






/* 
  ****************************************************************************

                    A p p e n d T e x t T o O u t p u t

  ****************************************************************************

    Stroke the specified text primitives and append the lines generated
    to the line primitive output for the cross section.
*/

int CSWConXsect::AppendTextToOutput
                          (GRaphTextRec *grtext, int ngrtext,
                           CRossSectionLinePrimitive **linesin,
                           int *nlinesin, int *maxlinesin)
{
    int                         i, j, k, jj, kk, n,
                                nlines, maxlines;
    CRossSectionLinePrimitive   *lprim = NULL, *lpsav = NULL;
    GRaphTextRec                *gtptr = NULL;
    CSW_F                       *xprim = NULL, *yprim = NULL;
    int                         font, nchar, nparts, parts[100],
                                ntot, istat;
    char                        *text = NULL;
    CSW_F                       sinang, cosang, angle, size,
                                textlen, charwidth,
                                xtext[1000], ytext[1000],
                                xanchor, yanchor;

    bool                        b_success = false;

    GPFFont      gpf_font_obj;

    auto fscope = [&]()
    {
        if (!b_success) {
            *linesin = lprim;
            *nlinesin = nlines;
            *maxlinesin = maxlines;
        }
        if (lpsav != NULL) {
            for (int ii=0; ii<nlines; ii++) {
                csw_Free (lpsav[ii].x);
                lpsav[ii].x = NULL;
                lpsav[ii].y = NULL;
            }
        }
        csw_Free (xprim);
    };
    CSWScopeGuard func_scope_guard (fscope);


    lprim = *linesin;
    nlines = *nlinesin;
    maxlines = *maxlinesin;

    font = 102;
    gpf_font_obj.gpf_setfont (font);

    for (i=0; i<ngrtext; i++) {

        gtptr = grtext + i;
        angle = gtptr->angle * .0174532f;
        sinang = (CSW_F)sin ((double)angle);
        cosang = (CSW_F)cos ((double)angle);
        nchar = gtptr->nchar;
        text = gtptr->text;
        size = gtptr->size;

        xanchor = gtptr->x;
        yanchor = gtptr->y;
        textlen = 0.0f;

        for (j=0; j<nchar; j++) {
            istat = gpf_font_obj.gpf_getfontstrokes (text[j], size, xtext, ytext,
                                        parts, &nparts, 1000, 100);
            if (!istat) {
                gpf_font_obj.gpf_CharWidth (text[j], size, &charwidth);
                textlen += charwidth;
                xanchor = textlen * cosang + gtptr->x;
                yanchor = textlen * sinang + gtptr->y;
                continue;
            }
            ntot = 0;
            for (k=0; k<nparts; k++) {
                ntot += parts[k];
            }

            gpf_font_obj.gpf_rotatechar (xtext, ytext, ntot, cosang, sinang);

            for (k=0; k<ntot; k++) {
                xtext[k] += xanchor;
                ytext[k] += yanchor;
            }

            jj = 0;
            for (k=0; k<nparts; k++) {
                n = parts[k];
                xprim = (CSW_F *)csw_Malloc (n * 2 * sizeof(CSW_F));
                if (!xprim) {
                    lpsav = lprim;
                    grd_utils_ptr->grd_set_err (1);
                    return -1;
                }
                yprim = xprim + n;

                for (kk=0; kk<n; kk++) {
                    xprim[kk] = xtext[jj];
                    yprim[kk] = ytext[jj];
                    jj++;
                }
    
                if (nlines >= maxlines) {
                    maxlines += _LINE_CHUNK;
                    lpsav = lprim;
                    lprim = (CRossSectionLinePrimitive *)csw_Realloc
                            (lprim, maxlines * sizeof(CRossSectionLinePrimitive));
                    if (!lprim) {
                        grd_utils_ptr->grd_set_err (1);
                        maxlines -= _LINE_CHUNK;
                        return -1;
                    }
                    lpsav = NULL;
                }

                lprim[nlines].npts = n;
                lprim[nlines].x = xprim;
                xprim = NULL;
                lprim[nlines].y = yprim;
                yprim = NULL;
                lprim[nlines].thick = gtptr->thick;
                lprim[nlines].layer = CON_XSECT_AXIS_LAYER;
                lprim[nlines].textflag = 1;
                lprim[nlines].id = -1;

                nlines++;
            }

            gpf_font_obj.gpf_CharWidth (text[j], size, &charwidth);
            textlen += charwidth;
            xanchor = textlen * cosang + gtptr->x;
            yanchor = textlen * sinang + gtptr->y;

        }

    }

    *nlinesin = nlines;
    *maxlinesin = maxlines;
    *linesin = lprim;

    lpsav = NULL;
    b_success = true;

    return 1;

}  /*  end of private AppendTextToOutput function  */





/* 
  ****************************************************************************

                        F r e e L i n e P r i m s

  ****************************************************************************

    Free the memory associated with a list of cross section line primitives.

*/

int CSWConXsect::FreeLinePrims (CRossSectionLinePrimitive *list, int nlist)
{
    int                              i;

    for (i=0; i<nlist; i++) {
        if (list[i].x) csw_Free (list[i].x);
    }

    csw_Free (list);

    return 1;

}  /*  end of private FreeLinePrims function  */





/* 
  ****************************************************************************

                        F r e e F i l l P r i m s

  ****************************************************************************

    Free the memory associated with a list of cross section fill primitives.

*/

int CSWConXsect::FreeFillPrims (CRossSectionFillPrimitive *list, int nlist)
{
    int                              i;

    for (i=0; i<nlist; i++) {
        if (list[i].x) csw_Free (list[i].x);
    }

    csw_Free (list);

    return 1;

}  /*  end of private FreeFillPrims function  */
