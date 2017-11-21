
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 *  NDPSurf.cc
 *
 *  This is the implementation of the NDPSurf class.
 *
 *  Blended grids are stored here and various methods
 *  of the class are used to visualize the blended
 *  grids in various fashions.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/ply_protoP.h"

#include "csw/surfaceworks/include/grid_api.h"
#include "csw/surfaceworks/include/contour_api.h"
#include "csw/surfaceworks/private_include/grd_fault.h"

#include "csw/jeasyx/private_include/DisplayList.h"
#include "csw/jeasyx/private_include/DLContour.h"
#include "csw/jeasyx/private_include/NDPSurf.h"


/*  constants for the file  */


/*------------------------------------------------------------------*/

/*
 * Empty constructor
 */
NDPSurf::NDPSurf ()
{

  /*
   * Initialize pubic data.
   */
    image_id = -1;
    saved_image_id = image_id;
    index_num = -1;
    frame_num = -1;
    layer_num = -1;
    deleted_flag = 0;
    visible_flag = 1;

    needs_reclip = 1;
    needs_recalc = 1;

    name[0] = '\0';

    gxmin = 1.e30;
    gymin = 1.e30;
    gxmax = -1.e30;
    gymax = -1.e30;

    conapi_obj.con_DefaultCalcOptions (&calc_options);
    conapi_obj.con_DefaultDrawOptions (&draw_options);

  /*
   * Initialize private data.
   */
    v1data = NULL;
    v2data = NULL;
    v3data = NULL;
    v4data = NULL;
    f1data = NULL;
    f2data = NULL;
    f3data = NULL;
    f4data = NULL;

    nvals = 0;
    vals = NULL;
    red = NULL;
    green = NULL;
    blue = NULL;
    alpha = NULL;

    rgrid = NULL;
    ggrid = NULL;
    bgrid = NULL;
    agrid = NULL;

    ncol = 0;
    nrow = 0;

    xpoly = NULL;
    ypoly = NULL;
    nvpoly = NULL;
    npoly = 0;

    xmin = 1.e30;
    ymin = 1.e30;
    width = 0.0;
    height = 0.0;

    faults = NULL;
    nfaults = 0;

    last_image_xmin = 1.e30;
    last_image_ymin = 1.e30;
    last_image_xmax = -1.e30;
    last_image_ymax = -1.e30;
    last_image_ncol = -1;
    last_image_nrow = -1;

    memset (&conprop, 0, sizeof(DLContourProperties));
    conprop.showColorFills = 1;

}

NDPSurf::~NDPSurf ()
{
    csw_Free (v1data);
    v1data = NULL;
    v2data = NULL;
    v3data = NULL;
    v4data = NULL;
    csw_Free (f1data);
    f1data = NULL;
    f2data = NULL;
    f3data = NULL;
    f4data = NULL;

    csw_Free (rgrid);
    rgrid = NULL;
    ggrid = NULL;
    bgrid = NULL;
    agrid = NULL;

    csw_Free (vals);
    vals = NULL;
    red = NULL;
    green = NULL;
    blue = NULL;
    alpha = NULL;

    csw_Free (xpoly);
    xpoly = NULL;
    ypoly = NULL;
    csw_Free (nvpoly);

    grdapi_obj.grd_FreeFaultLineStructs (faults, nfaults);
    faults = NULL;

}


/*--------------------------------------------------------------------------*/
void NDPSurf::SetSurfaceID (int id)
{
    image_id = id;
    saved_image_id = id;
}

void NDPSurf::SetName (char *str)
{
    if (str == NULL) {
        name[0] = '\0';
        return;
    }

    strncpy (name, str, 99);
    name[99] = '\0';

    return;
}



/*--------------------------------------------------------------------------*/

/*
 * Set the data and geometry for the grid.  A copy of the data and a copy of
 * the faults are made and used in the object.
 */
int NDPSurf::SetGridData (
    int *v1, int *v2, int *v3, int *v4,
    double *f1, double *f2, double *f3, double *f4,
    int ncol_in,
    int nrow_in,
    double xmin_in,
    double ymin_in,
    double width_in,
    double height_in
)
{
    int          ntot;

  /*
   * obvious errors.
   */
    if (ncol_in < 2  ||  nrow_in < 2) {
        return -1;
    }

    if (width_in <= 0.0  ||  height_in <= 0.0) {
        return -1;
    }

    if (xmin_in > 1.e20  ||  ymin_in > 1.e20) {
        return -1;
    }

    if (xmin_in < -1.e20  ||  ymin_in < -1.e20) {
        return -1;
    }

  /*
   * Allocate space and transfer values and fractions.
   */
    ntot = ncol_in * nrow_in;
    v1data = (int *)csw_Malloc (4 * ntot * sizeof(int));
    if (v1data == NULL) {
        return -1;
    }
    v2data = v1data + ntot;
    v3data = v2data + ntot;
    v4data = v3data + ntot;

    f1data = (double *)csw_Malloc (4 * ntot * sizeof(double));
    if (f1data == NULL) {
        csw_Free (v1data);
        v1data = NULL;
        v2data = NULL;
        v3data = NULL;
        v4data = NULL;
        return -1;
    }
    f2data = f1data + ntot;
    f3data = f2data + ntot;
    f4data = f3data + ntot;

    memcpy (v1data, v1, ntot * sizeof(int));
    memcpy (v2data, v2, ntot * sizeof(int));
    memcpy (v3data, v3, ntot * sizeof(int));
    memcpy (v4data, v4, ntot * sizeof(int));

    memcpy (f1data, f1, ntot * sizeof(double));
    memcpy (f2data, f2, ntot * sizeof(double));
    memcpy (f3data, f3, ntot * sizeof(double));
    memcpy (f4data, f4, ntot * sizeof(double));

  /*
   * Transfer to private instance variables.
   */
    ncol = ncol_in;
    nrow = nrow_in;

    xmin = xmin_in;
    ymin = ymin_in;
    width = width_in;
    height = height_in;

    gxmin = xmin;
    gymin = ymin;
    gxmax = xmin + width;
    gymax = ymin + height;

    needs_recalc = 1;

    return 1;

}

/*--------------------------------------------------------------------------*/

int NDPSurf::SetColorData (
    int       nv,
    int       *v,
    int       *r,
    int       *g,
    int       *b,
    int       *a)
{
    nvals = 0;
    csw_Free (vals);
    vals = NULL;
    red = NULL;
    green = NULL;
    blue = NULL;
    alpha = NULL;

    if (nv < 1) {
        return 1;
    }

    vals = (int *)csw_Malloc (nv * 5 * sizeof(int));
    if (vals == NULL) {
        return -1;
    }

    red = vals + nv;
    green = red + nv;
    blue = green + nv;
    alpha = blue + nv;

    memcpy (vals, v, nv * sizeof(int));
    memcpy (red, r, nv * sizeof(int));
    memcpy (green, g, nv * sizeof(int));
    memcpy (blue, b, nv * sizeof(int));
    memcpy (alpha, a, nv * sizeof(int));

    nvals = nv;

    return 1;

}

/*--------------------------------------------------------------------------*/

int NDPSurf::SetFaultData (
    double *xf,
    double *yf,
    int    *npf,
    int    nf)
{
    int istat;

    grdapi_obj.grd_FreeFaultLineStructs (faults, nfaults);
    faults = NULL;
    nfaults = 0;

    if (xf == NULL  ||  yf == NULL  ||
        npf == NULL  ||  nf < 1) {
        return 1;
    }

    istat =
      grdapi_obj.grd_DoubleFaultArraysToStructs (
        xf, yf, NULL,
        npf, NULL, nf,
        &faults, &nfaults);

    return istat;
}


/*--------------------------------------------------------------------------*/

int NDPSurf::SetBoundaryData (
    double *xp,
    double *yp,
    int    *npts,
    int    np)
{
    csw_Free (xpoly);
    csw_Free (nvpoly);
    npoly = 0;
    xpoly = NULL;
    ypoly = NULL;
    nvpoly = NULL;

    if (xp == NULL  ||  yp == NULL  ||
        npts == NULL  ||  np < 1) {
        return 1;
    }

    int ntot = 0;
    for (int i=0; i<np; i++) {
        ntot += npts[i];
    }

    xpoly = (double *)csw_Malloc (ntot * 2 * sizeof(double));
    if (xpoly == NULL) {
        return -1;
    }
    ypoly = xpoly + ntot;

    nvpoly = (int *)csw_Malloc (np * sizeof(int));
    if (nvpoly == NULL) {
        csw_Free (xpoly);
        return -1;
    }

    memcpy (xpoly, xp, ntot * sizeof(double));
    memcpy (ypoly, yp, ntot * sizeof(double));
    memcpy (nvpoly, npts, np * sizeof(int));

    npoly = np;

    return 1;

}



/*--------------------------------------------------------------------------*/

/*
 * Separate the properties into calc and draw options.
 */
void NDPSurf::SetContourProperties (void *vp)
{
    DLContourProperties    *p;

    conapi_obj.con_DefaultCalcOptions (&calc_options);
    conapi_obj.con_DefaultDrawOptions (&draw_options);
    if (vp == NULL) {
        return;
    }

    p = (DLContourProperties *) vp;

    calc_options.thickness_flag = p->isThickness;
    calc_options.log_base = (CSW_F)p->dataLogBase;
    calc_options.contour_interval = (CSW_F)p->contourInterval;
    calc_options.major_spacing = p->majorInterval;
    calc_options.first_contour = (CSW_F)p->contourMinValue;
    calc_options.last_contour = (CSW_F)p->contourMaxValue;
    calc_options.base_grid_value = (CSW_F)p->hardMin;
    calc_options.top_grid_value = (CSW_F)p->hardMax;

    draw_options.major_tick_spacing = (CSW_F)p->majorTickSpacing;
    draw_options.major_text_spacing = (CSW_F)p->majorLabelSpacing;
    draw_options.major_tick_len = (CSW_F)p->majorTickLength;
    draw_options.major_text_size = (CSW_F)p->majorLabelSize;
    draw_options.minor_tick_spacing = (CSW_F)p->minorTickSpacing;
    draw_options.minor_text_spacing = (CSW_F)p->minorLabelSpacing;
    draw_options.minor_tick_len = (CSW_F)p->minorTickLength;
    draw_options.minor_text_size = (CSW_F)p->minorLabelSize;
    draw_options.tick_direction = p->tickMajor;
    draw_options.minor_text_font = p->contourFont;
    draw_options.major_text_font = p->contourFont;

    memcpy (&conprop, p, sizeof(DLContourProperties));
    image_id = saved_image_id;

    needs_recalc = 1;

    return;
}

void *NDPSurf::GetContourProperties (void)
{
    return &conprop;
}

COntourCalcOptions const *NDPSurf::GetCalcOptions (void)
{
    return &calc_options;
}

COntourDrawOptions const *NDPSurf::GetDrawOptions (void)
{
    return &draw_options;
}

/*--------------------------------------------------------------------------*/

int NDPSurf::CalcContours (void *vptr, int inum, int val)
{
    int istat;

    inum = inum;
    istat = CalcContours (vptr, val);
    return istat;
}

/*--------------------------------------------------------------------------*/


int NDPSurf::CalcContours (void *vptr, int val)
{
/*
    CDisplayList        *dlist;
    COntourOutputRec    *contours;
    int                 ncontours;
    int                 i, istat;

    if (conprop.showContours == 0) {
        return 1;
    }

    if (conprop.showColorFills == 0) {
        image_id = -1;
    }

    if (vptr == NULL) {
        return -1;
    }
    dlist = (CDisplayList *)vptr;

    contours = NULL;
    ncontours = 0;
*/
    vptr = vptr;
    val = val;


    return 1;

}

/*--------------------------------------------------------------------------*/

int NDPSurf::CalcImage (void *vptr)
{
    CDisplayList        *dlist;
    double              xspace, yspace, x1, y1t, x2, y2;
    double              units;
    int                 istat, nc, nr, ntot;

    if (conprop.showColorFills == 0) {
        image_id = -1;
        return 1;
    }

    if (vptr == NULL) {
        return -1;
    }

    dlist = (CDisplayList *)vptr;

    dlist->GetFrameClipLimits (frame_num, &x1, &y1t, &x2, &y2);

    if (x1 > 1.e20  ||  y1t > 1.e20  ||  x2 > 1.e20  ||  y2 > 1.e20) {
        return -1;
    }

    double  tiny;

  /*
   * If the image is completely outside the frame clip limits,
   * do not attempt to draw it.
   */
    if (x1 >= gxmax  ||  y1t >= gymax  ||
        x2 <= gxmin  ||  y2 <= gymin) {
        return 1;
    }

    units = dlist->GetFrameUnitsPerPixel (frame_num);
    if (units < 1.e20) {
        xspace = units * 2.0;
        yspace = units * 2.0;
    }
    else {
        xspace = (gxmax - gxmin) / (ncol - 1);
        yspace = (gymax - gymin) / (nrow - 1);
    }

  /*
   * Bug 541
   *
   * The lower left corner of the image (x1, y1t) must be completely outside
   * of the image if the entire image is to be displayed.  When a trimesh created
   * from a regular grid is color filled, the x1, y1t is exactly on the trimesh
   * border, which confuses the inside/outside determination.  If the lower left
   * corner is moved slightly to the outside in this case, things work better.
   *
   * Glenn Pinkerton
   */
    tiny = (gxmax - gxmin + gymax - gymin) / 200.0;
    tiny += xspace + yspace;
    if (x1 < gxmin + xspace) x1 = gxmin - tiny;
    if (y1t < gymin + yspace) y1t = gymin - tiny;
    if (x2 > gxmax - xspace) x2 = gxmax + tiny;
    if (y2 > gymax - yspace) y2 = gymax + tiny;

    nc = (int)((x2 - x1) / xspace + 1.0);
    nr = (int)((y2 - y1t) / yspace + 1.0);

    if (nc * nr < 0) {
        tiny = gxmin;
    }

  /*
   * Calculate color component grids at the original spacing and
   * then resample them to the image spacing.
   */
    CSW_F    *rgrid2, *ggrid2, *bgrid2, *agrid2;

    if (rgrid == NULL) {
        istat =
          grdimage_obj.grd_create_ndp_colors (
            v1data, v2data, v3data, v4data,
            f1data, f2data, f3data, f4data,
            ncol, nrow,
            nvals, vals,
            red, green, blue, alpha,
            &rgrid, &ggrid, &bgrid, &agrid);
        if (istat == -1) {
            return -1;
        }
        csw_Free (v1data);
        csw_Free (f1data);
        v1data = v2data = v3data = v4data = NULL;
        f1data = f2data = f3data = f4data = NULL;
    }

    int        do_write;
    do_write = csw_GetDoWrite ();
    if (do_write) {
        grdapi_obj.grd_WriteFaultLines (faults, nfaults,
                             (char *)"ndppoly.xyz");
        grdapi_obj.grd_WriteTextFile (rgrid, ncol, nrow,
                           gxmin, gymin, gxmax, gymax,
                           (char *)"ndpred.xyz");
        grdapi_obj.grd_WriteTextFile (ggrid, ncol, nrow,
                           gxmin, gymin, gxmax, gymax,
                           (char *)"ndpgreen.xyz");
        grdapi_obj.grd_WriteTextFile (bgrid, ncol, nrow,
                           gxmin, gymin, gxmax, gymax,
                           (char *)"ndpblue.xyz");
        grdapi_obj.grd_WriteTextFile (agrid, ncol, nrow,
                           gxmin, gymin, gxmax, gymax,
                           (char *)"ndpalpha.xyz");
    }


    ntot = nc * nr;
    rgrid2 = (CSW_F *)csw_Malloc (ntot * 4 * sizeof(CSW_F));
    if (rgrid2 == NULL) {
        return -1;
    }
    ggrid2 = rgrid2 + ntot;
    bgrid2 = ggrid2 + ntot;
    agrid2 = bgrid2 + ntot;

    istat = grdapi_obj.grd_ResampleGridFromDouble
                             (rgrid, NULL, ncol, nrow,
                              gxmin, gymin, gxmax, gymax,
                              faults, nfaults,
                              rgrid2, NULL,
                              nc, nr,
                              x1, y1t, x2, y2,
                              GRD_BILINEAR);
    if (istat == -1) {
        csw_Free (rgrid2);
        return -1;
    }

    istat = grdapi_obj.grd_ResampleGridFromDouble
                             (ggrid, NULL, ncol, nrow,
                              gxmin, gymin, gxmax, gymax,
                              faults, nfaults,
                              ggrid2, NULL,
                              nc, nr,
                              x1, y1t, x2, y2,
                              GRD_BILINEAR);
    if (istat == -1) {
        csw_Free (rgrid2);
        return -1;
    }

    istat = grdapi_obj.grd_ResampleGridFromDouble
                             (bgrid, NULL, ncol, nrow,
                              gxmin, gymin, gxmax, gymax,
                              faults, nfaults,
                              bgrid2, NULL,
                              nc, nr,
                              x1, y1t, x2, y2,
                              GRD_BILINEAR);
    if (istat == -1) {
        csw_Free (rgrid2);
        return -1;
    }

    istat = grdapi_obj.grd_ResampleGridFromDouble
                             (agrid, NULL, ncol, nrow,
                              gxmin, gymin, gxmax, gymax,
                              faults, nfaults,
                              agrid2, NULL,
                              nc, nr,
                              x1, y1t, x2, y2,
                              GRD_BILINEAR);
    if (istat == -1) {
        csw_Free (rgrid2);
        return -1;
    }

    GRdImage         *maskimage;
    unsigned char    *clip_mask;
    int              *ncomp, i;

    clip_mask = NULL;
    maskimage = NULL;

    if (npoly > 0) {
        ncomp = (int *)csw_Malloc (npoly * sizeof(int));
        if (ncomp == NULL) {
            csw_Free (rgrid2);
            return -1;
        }
        for (i=0; i<npoly; i++) {
            ncomp[i] = 1;
        }
        if (do_write) {
            grdapi_obj.grd_WriteLines (xpoly, ypoly, NULL,
                            npoly, ncomp, nvpoly,
                            (char *)"fromgrid.xyz");
        }
        maskimage =
          grdimage_obj.grd_create_clip_mask (
            xpoly, ypoly,
            npoly, ncomp, nvpoly,
            x1, y1t, x2, y2,
            nc, nr);
        csw_Free (ncomp);
        ncomp = NULL;
        if (maskimage != NULL) {
            clip_mask = maskimage->data;
        }
    }

    unsigned char    *ur, *ug, *ub, *ua;
    CSW_F            zt;

    ur = (unsigned char *)csw_Malloc (ntot * 4 * sizeof(unsigned char));
    if (ur == NULL) {
        csw_Free (rgrid2);
        csw_Free (clip_mask);
        csw_Free (maskimage);
        return -1;
    }
    ug = ur + ntot;
    ub = ug + ntot;
    ua = ub + ntot;

    for (i=0; i<ntot; i++) {
        zt = rgrid2[i];
        if (zt < 0) zt = 0;
        if (zt > 255) zt = 255;
        ur[i] = (unsigned char)zt;
        zt = ggrid2[i];
        if (zt < 0) zt = 0;
        if (zt > 255) zt = 255;
        ug[i] = (unsigned char)zt;
        zt = bgrid2[i];
        if (zt < 0) zt = 0;
        if (zt > 255) zt = 255;
        ub[i] = (unsigned char)zt;
        zt = agrid2[i];
        if (zt < 0) zt = 0;
        if (zt > 255) zt = 255;
        ua[i] = (unsigned char)zt;
        if (clip_mask) {
            if (clip_mask[i] == 0) {
                ua[i] = 0;
            }
        }
    }

    csw_Free (rgrid2);
    rgrid2 = ggrid2 = bgrid2 = agrid2 = NULL;

    dlist->SetGridNum (index_num);

    istat =
        dlist->AddColorImage (ur, ug, ub, ua,
                              nc, nr,
                              x1, y1t, x2, y2);
    if (istat == -1) {
        csw_Free (clip_mask);
        csw_Free (maskimage);
        csw_Free (ur);
        return -1;
    }

    csw_Free (ur);
    csw_Free (clip_mask);
    csw_Free (maskimage);

    return 1;

}
