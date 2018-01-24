
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 *  DLSurf.cc
 *
 *  This is the implementation of the DLSurf class.
 *  It is used to store grid data and manage the visualization
 *  of the grid data.
 *
 *  This class stores both grid and trimesh data.  Since the
 *  visualization process can switch from a grid to a trimesh
 *  representation depending on how the surface is being
 *  displayed, I combine both representations into the same class.
 *
 *  This class is only used from the display list class.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "csw/jeasyx/private_include/DisplayList.h"
#include "csw/jeasyx/private_include/DLContour.h"
#include "csw/jeasyx/private_include/DLSurf.h"

#include "csw/utils/include/csw_.h"
#include "csw/surfaceworks/include/grid_api.h"
#include "csw/surfaceworks/include/contour_api.h"
#include "csw/utils/private_include/ply_protoP.h"


/*  constants for the file  */


/*------------------------------------------------------------------*/

/*
 * Constructor with no parameters.
 */
DLSurf::DLSurf ()
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

    tmesh_num = -1;

    turn_off_reclip = 0;
    needs_reclip = 1;
    needs_contour_reclip = 1;
    needs_recalc = 1;
    needs_rotation = 0;

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
    data = NULL;
    trigrid = NULL;

    ncol = 0;
    nrow = 0;
    tncol = 0;
    tnrow = 0;

    xpoly = NULL;
    ypoly = NULL;
    zpoly = NULL;
    ncpoly = NULL;
    nvpoly = NULL;
    npoly = 0;

    xmin = 1.e30;
    ymin = 1.e30;
    width = 0.0;
    height = 0.0;
    angle = 0.0;

    faults = NULL;
    nfaults = 0;

    memset (zmin_band, 0, 1000 * sizeof(double));
    memset (zmax_band, 0, 1000 * sizeof(double));
    memset (red_band, 0, 1000 * sizeof(int));
    memset (green_band, 0, 1000 * sizeof(int));
    memset (blue_band, 0, 1000 * sizeof(int));
    memset (alpha_band, 0, 1000 * sizeof(int));
    nband = 0;

    last_image_xmin = 1.e30;
    last_image_ymin = 1.e30;
    last_image_xmax = -1.e30;
    last_image_ymax = -1.e30;
    last_image_ncol = -1;
    last_image_nrow = -1;

    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    num_nodes = 0;
    num_edges = 0;
    num_triangles = 0;

    cnodes = NULL;
    cedges = NULL;
    ctriangles = NULL;
    cnum_nodes = 0;
    cnum_edges = 0;
    cnum_triangles = 0;

    smooth_needed = 1;

    memset (&conprop, 0, sizeof(DLContourProperties));

    zscale = 1.0;
    last_zscale = 1.0;
    grid_zscale_needed = 0;
    tmesh_zscale_needed = 0;
    band_zscale_needed = 0;

}

DLSurf::~DLSurf ()
{
    csw_Free (data);
    data = NULL;
    csw_Free (trigrid);
    trigrid = NULL;
    grdapi_obj.grd_FreeFaultLineStructs (faults, nfaults);
    faults = NULL;
    csw_Free (nodes);
    nodes = NULL;
    csw_Free (edges);
    edges = NULL;
    csw_Free (triangles);
    triangles = NULL;
    csw_Free (cnodes);
    cnodes = NULL;
    csw_Free (cedges);
    cedges = NULL;
    csw_Free (ctriangles);
    ctriangles = NULL;
    csw_Free (xpoly);
    xpoly = NULL;
    ypoly = NULL;
    zpoly = NULL;
    nvpoly = NULL;
    ncpoly = NULL;
}


/*--------------------------------------------------------------------------*/
void DLSurf::SetSurfaceID (int id)
{
    image_id = id;
    saved_image_id = id;
}

/*--------------------------------------------------------------------------*/

/*
 * Set the data and geometry for the grid.  A copy of the data and a copy of
 * the faults are made and used in the object.
 */
int DLSurf::SetGridData (
    double *data_in,
    int ncol_in,
    int nrow_in,
    double xmin_in,
    double ymin_in,
    double width_in,
    double height_in,
    double angle_in,
    FAultLineStruct *faults_in,
    int nfaults_in
)
{
    int          i;

  /*
   * obvious errors.
   */
    if (ncol_in < 2  ||  nrow_in < 2) {
        return -1;
    }

    if (width_in <= 0.0  ||  height_in <= 0.0) {
        return -1;
    }

    if (data_in == NULL) {
        return -1;
    }

    if (angle_in < -360.1  ||  angle_in > 360.1) {
        return -1;
    }

    if (xmin_in > 1.e20  ||  ymin_in > 1.e20) {
        return -1;
    }

    if (xmin_in < -1.e20  ||  ymin_in < -1.e20) {
        return -1;
    }

  /*
   * Transfer to private instance variables.
   */
    ncol = ncol_in;
    nrow = nrow_in;

    xmin = xmin_in;
    ymin = ymin_in;
    width = width_in;
    height = height_in;
    angle = angle_in;

    if (faults_in != NULL  &&  nfaults_in > 0) {
        faults = grdapi_obj.grd_CopyFaultLineStructs (faults_in, nfaults_in);
        if (faults == NULL) {
            return -1;
        }
        nfaults = nfaults_in;
    }

    data = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (data == NULL) {
        grdapi_obj.grd_FreeFaultLineStructs (faults, nfaults);
        return -1;
    }

    for (i=0; i<ncol * nrow; i++) {
        data[i] = (CSW_F)data_in[i];
    }

  /*
   * If the angle is zero or very close to zero, set the
   * grid bounds now.  If the grid needs to be rotated,
   * do not do that until the first actual use of the
   * grid occurs (i.e. contouring, color filling or posting).
   */
    if (angle > -.01  &&  angle < .01) {
        angle = 0.0;
        gxmin = xmin;
        gymin = ymin;
        gxmax = xmin + width;
        gymax = ymin + height;
        needs_rotation = 0;
    }
    else {
        needs_rotation = 1;
    }

    needs_recalc = 1;

/*
 * The grid data is the master surface now, so csw_Free any trimesh
 * data that may have been set prior to this function call.
 */
    csw_Free (trigrid);
    trigrid = NULL;
    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (triangles);
    csw_Free (cnodes);
    csw_Free (cedges);
    csw_Free (ctriangles);
    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    num_nodes = 0;
    num_edges = 0;
    num_triangles = 0;

    cnodes = NULL;
    cedges = NULL;
    ctriangles = NULL;
    cnum_nodes = 0;
    cnum_edges = 0;
    cnum_triangles = 0;

    return 1;

}


/*--------------------------------------------------------------------------*/

int DLSurf::SetTriMesh (
                    double *xnode,
                    double *ynode,
                    double *znode,
                    int *nodeflag,
                    int numnode,
                    int *n1edge,
                    int *n2edge,
                    int *t1edge,
                    int *t2edge,
                    int *edgeflag,
                    int numedge,
                    int *e1tri,
                    int *e2tri,
                    int *e3tri,
                    int *triflag,
                    int numtri)
{
    int               i;
    NOdeStruct        *np;
    EDgeStruct        *ep;
    TRiangleStruct    *tp;
    double            z1, z2;

/*
 * Check obvious errors.  Note that the flag arrays can be
 * NULL.  They are optional.
 */
    if (xnode == NULL  ||
        ynode == NULL  ||
        znode == NULL  ||
        numnode < 3  ||
        n1edge == NULL  ||
        n2edge == NULL  ||
        t1edge == NULL  ||
        t2edge == NULL  ||
        numedge < 3  ||
        e1tri == NULL  ||
        e2tri == NULL  ||
        e3tri == NULL  ||
        numtri < 1)
    {
        return 0;
    }

/*
 * Cleanup old trimesh data.
 */
    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (triangles);
    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    num_nodes = 0;
    num_edges = 0;
    num_triangles = 0;

    csw_Free (cnodes);
    csw_Free (cedges);
    csw_Free (ctriangles);
    cnodes = NULL;
    cedges = NULL;
    ctriangles = NULL;
    cnum_nodes = 0;
    cnum_edges = 0;
    cnum_triangles = 0;

    smooth_needed = 1;

/*
 * Allocate space for new trimesh.
 */
    nodes = (NOdeStruct *)csw_Calloc (numnode * sizeof(NOdeStruct));
    edges = (EDgeStruct *)csw_Calloc (numedge * sizeof(EDgeStruct));
    triangles = (TRiangleStruct *)csw_Calloc
                 (numtri * sizeof(TRiangleStruct));
    if (nodes == NULL  ||
        edges == NULL  ||
        triangles == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
        nodes = NULL;
        edges = NULL;
        triangles = NULL;
        return -1;
    }

/*
 * Fill in node data.
 */
    for (i=0; i<numnode; i++) {
        np = nodes+i;
        np->x = xnode[i];
        np->y = ynode[i];
        np->z = znode[i];
        if (nodeflag != NULL) {
            np->flag = nodeflag[i];
        }
    }
    num_nodes = numnode;

/*
 * Fill in edge data.
 */
    for (i=0; i<numedge; i++) {
        ep = edges + i;
        ep->node1 = n1edge[i];
        ep->node2 = n2edge[i];
        ep->tri1 = t1edge[i];
        ep->tri2 = t2edge[i];
        if (edgeflag != NULL) {
            ep->flag = edgeflag[i];
        }
    }
    num_edges = numedge;

/*
 * Fill in triangle data.
 */
    for (i=0; i<numtri; i++) {
        tp = triangles + i;
        tp->edge1 = e1tri[i];
        tp->edge2 = e2tri[i];
        tp->edge3 = e3tri[i];
        if (triflag) {
            tp->flag = triflag[i];
        }
    }
    num_triangles = numtri;

/*
 * The triangle data set from the outside is now the master
 * data for this surface.  Any grid master data must be
 * csw_Freed now.
 */
    csw_Free (data);
    data = NULL;
    ncol = 0;
    nrow = 0;
    gxmin = 1.e30;
    gymin = 1.e30;
    gxmax = -1.e30;
    gymax = -1.e30;

    needs_reclip = 1;
    needs_recalc = 1;
    needs_rotation = 0;

/*
 * reset the surface xy limits to the trimesh bounding box
 */
    grdapi_obj.grd_CalcTriMeshBoundingBox (
        nodes, num_nodes,
        edges, num_edges,
        triangles, num_triangles,
        &gxmin, &gymin,
        &gxmax, &gymax,
        &z1, &z2);

    return 1;

}

/*--------------------------------------------------------------------------*/

/*
 * Separate the properties into calc and draw options.
 */
void DLSurf::SetContourProperties (void *vp)
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
    calc_options.smoothing = (int)p->contourSmoothing;

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

    last_zscale = zscale;
    zscale = p->zUnitsConversionFactor;

    csw_Free (cnodes);
    csw_Free (cedges);
    csw_Free (ctriangles);
    cnodes = NULL;
    cedges = NULL;
    ctriangles = NULL;
    cnum_nodes = 0;
    cnum_edges = 0;
    cnum_triangles = 0;

    smooth_needed = 1;

    return;
}

void *DLSurf::GetContourProperties (void)
{
    return &conprop;
}

COntourCalcOptions const *DLSurf::GetCalcOptions (void)
{
    return &calc_options;
}

COntourDrawOptions const *DLSurf::GetDrawOptions (void)
{
    return &draw_options;
}

/*--------------------------------------------------------------------------*/

int DLSurf::SetImageBands (
    double *zmin,
    double *zmax,
    int *red,
    int *green,
    int *blue,
    int *alpha,
    int nband_in
)
{

  /*
   * Change the image bands to an unset state.
   */
    memset (zmin_band, 0, 1000 * sizeof(double));
    memset (zmax_band, 0, 1000 * sizeof(double));
    memset (red_band, 0, 1000 * sizeof(int));
    memset (green_band, 0, 1000 * sizeof(int));
    memset (blue_band, 0, 1000 * sizeof(int));
    memset (alpha_band, 0, 1000 * sizeof(int));
    nband = 0;

    needs_recalc = 1;

  /*
   * Any null pointers leave the image bands in an unset state.
   */
    if (nband_in < 1) {
        return 1;
    }

    if (zmin == NULL  ||  zmax == NULL  ||
        red == NULL  ||  green == NULL  ||  blue == NULL  ||  alpha == NULL) {
        return 1;
    }

  /*
   * Copy new band data into the instance variables.
   */
    if (nband_in > 1000) {
        nband_in = 1000;
    }

    nband = nband_in;

    memcpy (zmin_band, zmin, nband * sizeof(double));
    memcpy (zmax_band, zmax, nband * sizeof(double));
    memcpy (red_band, red, nband * sizeof(int));
    memcpy (green_band, green, nband * sizeof(int));
    memcpy (blue_band, blue, nband * sizeof(int));
    memcpy (alpha_band, alpha, nband * sizeof(int));

    return 1;

}

/*--------------------------------------------------------------------------*/

int DLSurf::CalcContours (void *vptr, int inum)
{
    int istat;

    tmesh_num = inum;
    istat = CalcContours (vptr);
    return istat;
}

/*--------------------------------------------------------------------------*/


int DLSurf::CalcContours (void *vptr)
{
    CDisplayList        *dlist;
    int                 i, istat;
    COntourOutputRec    *contours;
    int                 ncontours;

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
    dlist->SetFrameNum (frame_num);

    contours = NULL;
    ncontours = 0;

/*
 * If this surface has a grid as its master data, calculate
 * the contours from the grid.
 */
    if (data != NULL) {

        if (grid_zscale_needed == 1) {
            double sfact = zscale / last_zscale;
            if (sfact < 0.9999  || sfact > 1.0001) {
                for (i=0; i<ncol*nrow; i++) {
                    if (data[i] >= 1.e20) continue;
                    data[i] *= (CSW_F)sfact;
                }
            }
            grid_zscale_needed = 0;
        }

        if (needs_rotation) {
            istat = rotate_grid ();
            if (istat == -1) {
                return -1;
            }
            needs_rotation = 0;
        }

        istat = conapi_obj.con_CalcContoursFromDouble (
            data, ncol, nrow,
            gxmin, gymin, gxmax, gymax,
            0.0,  /* unknown scale */
            &contours, &ncontours,
            faults, nfaults,
            &calc_options);
        if (istat == -1) {
            return -1;
        }
    }

/*
 * If this surface has a trimesh as its master data, calculate
 * the contours from the trimesh.
 */
    else if (nodes != NULL  &&  edges != NULL  &&  triangles != NULL) {

        if (cnodes == NULL) {
            cnodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
            cedges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
            ctriangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));
            if (cnodes == NULL  ||  cedges == NULL  ||  ctriangles == NULL) {
                csw_Free (cnodes);
                csw_Free (cedges);
                csw_Free (ctriangles);
                return -1;
            }

            memcpy (cnodes, nodes, num_nodes * sizeof(NOdeStruct));
            memcpy (cedges, edges, num_edges * sizeof(EDgeStruct));
            memcpy (ctriangles, triangles, num_triangles * sizeof(TRiangleStruct));
            cnum_nodes = num_nodes;
            cnum_edges = num_edges;
            cnum_triangles = num_triangles;
        }

        if (tmesh_zscale_needed == 1) {
            double sfact = zscale / last_zscale;
            if (sfact < 0.9999  ||  sfact > 1.0001) {
                for (i=0; i<num_nodes; i++) {
                    if (cnodes[i].z >= 1.e20) continue;
                    cnodes[i].z *= sfact;
                }
            }
            tmesh_zscale_needed = 0;
        }

        int do_write = csw_GetDoWrite ();
        if (do_write) {
            printf ("writing trimesh file for debug\n");
            char fname[100];
            sprintf (fname, "preconsmooth.tri");
            grdapi_obj.grd_WriteTextTriMeshFile (
                0, NULL,
                ctriangles, cnum_triangles,
                cedges, cnum_edges,
                cnodes, cnum_nodes,
                fname);
        }

        if (smooth_needed) {
            istat =
              conapi_obj.con_SmoothTriMesh (
                &cnodes, &cnum_nodes,
                &cedges, &cnum_edges,
                &ctriangles, &cnum_triangles,
                calc_options.smoothing);
            if (istat == -1) {
                return -1;
            }
            smooth_needed = 0;
        }

        istat = conapi_obj.con_CalcTriMeshContours (
            cnodes, cnum_nodes,
            cedges, cnum_edges,
            ctriangles, cnum_triangles,
            1.e20,
            &contours, &ncontours,
            &calc_options);
        if (istat == -1) {
            return -1;
        }

    }

    if (contours == NULL  ||  ncontours < 1) {
        return 0;
    }

    for (i=0; i<ncontours; i++) {
        dlist->AddContour (contours+i,
                           index_num,
                           image_id);
    }

/*
 * Copies of the contours are added to the display list,
 * so the originals need to be csw_Freed here.
 */
    conapi_obj.con_FreeContours (contours, ncontours);

    return 1;

}

/*--------------------------------------------------------------------------*/

int DLSurf::CalcImage (void *vptr)
{
    CDisplayList        *dlist = NULL;
    double              xspace, yspace, x1, y1t, x2, y2;
    double              units;
    int                 i, istat, nc, nr;
    CSW_F               *newgrid = NULL;

    if (conprop.showColorFills == 0) {
        image_id = -1;
        return 1;
    }

    if (nband < 1) {
        return 1;
    }

    if (data != NULL) {
        if (ncol < 2  ||  nrow < 2) {
            return -1;
        }
    }

    if (vptr == NULL) {
        return -1;
    }

    dlist = (CDisplayList *)vptr;
    dlist->SetFrameNum (frame_num);

    if (data != NULL) {
        if (needs_rotation) {
            istat = rotate_grid ();
            if (istat == -1) {
                return -1;
            }
            needs_rotation = 0;
        }
    }

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
   * The lower left corner of the image (x1, y1t) must be completely outside
   * of the image if the entire image is to be displayed.  When a trimesh created
   * from a regular grid is color filled, the x1, y1t is exactly on the trimesh
   * border, which confuses the inside/outside determination.  If the lower left
   * corner is moved slightly to the outside in this case, things work better.
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

    newgrid = (CSW_F *)csw_Malloc (nc * nr * sizeof(CSW_F));
    if (newgrid == NULL) {
        return -1;
    }

  /*
   * If the master data is a grid, resample it at the image resolution.
   */
    istat = -1;
    if (data != NULL) {
        if (grid_zscale_needed == 1) {
            double sfact = zscale / last_zscale;
            if (sfact < 0.9999  || sfact > 1.0001) {
                for (i=0; i<ncol*nrow; i++) {
                    if (data[i] >= 1.e20) continue;
                    data[i] *= (CSW_F)sfact;
                }
            }
            grid_zscale_needed = 0;
        }

        istat = grdapi_obj.grd_ResampleGridFromDouble
                                 (data, NULL, ncol, nrow,
                                  gxmin, gymin, gxmax, gymax,
                                  faults, nfaults,
                                  newgrid, NULL,
                                  nc, nr,
                                  x1, y1t, x2, y2,
                                  GRD_BICUBIC);
        if (istat == -1) {
            csw_Free (newgrid);
            return -1;
        }
    }

  /*
   * TriMesh data is being displayed.
   */
    else {

      /*
       * A grid was calculated for contour lines, so use it.
       */
        if (trigrid != NULL) {
            if (grid_zscale_needed == 1) {
                double sfact = zscale / last_zscale;
                if (sfact < 0.9999  || sfact > 1.0001) {
                    for (i=0; i<ncol*nrow; i++) {
                        if (trigrid[i] >= 1.e20) continue;
                        trigrid[i] *= (CSW_F)sfact;
                    }
                }
                grid_zscale_needed = 0;
            }

            istat = grdapi_obj.grd_ResampleGridFromDouble
                                     (trigrid, NULL, tncol, tnrow,
                                      gxmin, gymin, gxmax, gymax,
                                      faults, nfaults,
                                      newgrid, NULL,
                                      nc, nr,
                                      x1, y1t, x2, y2,
                                      GRD_BICUBIC);
            if (istat == -1) {
                csw_Free (newgrid);
                return -1;
            }
        }

      /*
       * If the master data is a trimesh, convert it to a grid at the
       * image resolution.
       */
        else if (nodes != NULL  &&  edges != NULL  &&  triangles != NULL) {

            if (cnodes == NULL) {
                cnodes = (NOdeStruct *)csw_Malloc (num_nodes * sizeof(NOdeStruct));
                cedges = (EDgeStruct *)csw_Malloc (num_edges * sizeof(EDgeStruct));
                ctriangles = (TRiangleStruct *)csw_Malloc (num_triangles * sizeof(TRiangleStruct));
                if (cnodes == NULL  ||  cedges == NULL  ||  ctriangles == NULL) {
                    csw_Free (cnodes);
                    csw_Free (cedges);
                    csw_Free (ctriangles);
                    csw_Free (newgrid);
                    return -1;
                }

                memcpy (cnodes, nodes, num_nodes * sizeof(NOdeStruct));
                memcpy (cedges, edges, num_edges * sizeof(EDgeStruct));
                memcpy (ctriangles, triangles, num_triangles * sizeof(TRiangleStruct));
                cnum_nodes = num_nodes;
                cnum_edges = num_edges;
                cnum_triangles = num_triangles;
            }

            if (tmesh_zscale_needed == 1) {
                double sfact = zscale / last_zscale;
                if (sfact < 0.9999  || sfact > 1.0001) {
                    for (i=0; i<num_nodes; i++) {
                        if (cnodes[i].z >= 1.e20) continue;
                        cnodes[i].z *= sfact;
                    }
                }
                tmesh_zscale_needed = 0;
            }

            if (smooth_needed) {
                istat =
                  conapi_obj.con_SmoothTriMesh (
                    &cnodes, &cnum_nodes,
                    &cedges, &cnum_edges,
                    &ctriangles, &cnum_triangles,
                    calc_options.smoothing);
                if (istat == -1) {
                    csw_Free (newgrid);
                    return -1;
                }
                smooth_needed = 0;
            }

            istat =
            grdapi_obj.grd_CalcGridFromTriMesh (
                cnodes, cnum_nodes,
                cedges, cnum_edges,
                ctriangles, cnum_triangles,
                1.e30,
                newgrid, nc, nr,
                x1, y1t, x2, y2);
            if (istat == -1) {
                csw_Free (newgrid);
                return -1;
            }
        }

      /*
       * Calculate the boundary polygon if needed.
       */
        if (xpoly == NULL) {
            istat = calc_outline_polygon ();
            if (istat == -1) {
                csw_Free (newgrid);
                return -1;
            }
        }

      /*
       * Calculate an image mask and set the image nodes outside
       * the trimesh boundary to null values.
       */
        if (xpoly != NULL) {

            GRdImage    *gimage;
            unsigned char *ucdata;

            gimage =
            grdapi_obj.grd_CreateClipMask (
                xpoly, ypoly,
                npoly, ncpoly, nvpoly,
                x1, y1t, x2, y2,
                nc, nr);
            if (gimage != NULL) {
                ucdata = gimage->data;
                for (i=0; i<nc*nr; i++) {
                    if (ucdata[i] == 0) {
                        newgrid[i] = 1.e30f;
                    }
                }
                grdapi_obj.grd_FreeImageData (gimage);
                gimage = NULL;
            }
        }
    }

    if (istat == -1) {
        csw_Free (newgrid);
        return 1;
    }

    if (band_zscale_needed == 1) {
        double       sfact = zscale / last_zscale;
        for (i=0; i<nband; i++) {
            zmin_band[i] *= sfact;
            zmax_band[i] *= sfact;
        }
        band_zscale_needed = 0;
    }

    dlist->SetImageColors (zmin_band,
                           zmax_band,
                           red_band,
                           green_band,
                           blue_band,
                           alpha_band,
                           nband);

    istat = dlist->AddGridImage (frame_num,
                                 image_id,
                                 newgrid,
                                 nc, nr,
                                 1.e20f,
                                 x1, y1t, x2, y2);
    csw_Free (newgrid);
    newgrid = NULL;
    if (istat == -1) {
        return -1;
    }

    return 1;

}

/*--------------------------------------------------------------------------*/

int DLSurf::CalcNodes (void *vptr)
{
    CDisplayList        *dlist;
    double              xspace, yspace, x, y, z;
    double              zmin, zmax;
    int                 i, j, jstart, k, snum;
    double              size, tsize;
    int                 ndec;

    if (vptr == NULL) {
        return 1;
    }

    if (data != NULL) {
        if (ncol < 2  ||  nrow < 2) {
            return 1;
        }
    }

    if (conprop.showNodes == 0  &&
        conprop.showNodeValues == 0) {
        return 1;
    }

    if (conprop.showColorFills == 0) {
        image_id = -1;
    }

    dlist = (CDisplayList *)vptr;

    yspace = (gymax - gymin) / (nrow - 1);
    xspace = (gxmax - gxmin) / (ncol - 1);

/*
 * Draw node symbols.
 */
    size = 0.0;
    if (conprop.showNodes) {
        dlist->SetFrameNum (frame_num);
        dlist->SetGridNum (index_num);
        dlist->SetImageID (image_id);
        size = conprop.nodeSymbolSize;
        snum = conprop.nodeSymbol;
        dlist->SetSymbolColor (conprop.redNode,
                               conprop.greenNode,
                               conprop.blueNode,
                               conprop.alphaNode);
        if (data != NULL) {
            if (grid_zscale_needed == 1) {
                double sfact = zscale / last_zscale;
                if (sfact < 0.9999  || sfact > 1.0001) {
                    for (i=0; i<ncol*nrow; i++) {
                        if (data[i] >= 1.e20) continue;
                        data[i] *= (CSW_F)sfact;
                    }
                }
                grid_zscale_needed = 0;
            }

            for (i=0; i<nrow; i++) {
                y = gymin + i * yspace;
                jstart = i * ncol;
                for (j=0; j<ncol; j++) {
                    k = jstart + j;
                    z = data[k];
                    if (z < -1.e20  ||  z > 1.e20) {
                        continue;
                    }
                    x = gxmin + xspace * j;
                    dlist->AddSymb (x, y, size, 0.0, snum);
                }
            }
        }
        else if (nodes != NULL) {
            if (tmesh_zscale_needed == 1) {
                double sfact = zscale / last_zscale;
                if (sfact < 0.9999  || sfact > 1.0001) {
                    for (i=0; i<num_nodes; i++) {
                        if (nodes[i].z >= 1.e20) continue;
                        nodes[i].z *= sfact;
                    }
                }
                tmesh_zscale_needed = 0;
            }

            for (i=0; i<num_nodes; i++) {
                x = nodes[i].x;
                y = nodes[i].y;
                dlist->AddSymb (x, y, size, 0.0, snum);
            }
        }
    }

/*
 * Draw node values centered below the node if symbols
 * were drawn or centered at the node if no symbols were drawn.
 */
    tsize = 0.0;
    if (conprop.showNodeValues) {
        dlist->SetGridNum (index_num);
        dlist->SetImageID (image_id);
        zmin = 1.e30;
        zmax = -1.e30;
        if (data != NULL) {
            for (i=0; i<ncol * nrow; i++) {
                z = data[i];
                if (z < -1.e20  ||  z > 1.e20) {
                    continue;
                }
                if (z < zmin) zmin = z;
                if (z > zmax) zmax = z;
            }
        }
        else if (nodes != NULL) {
            for (i=0; i<num_nodes; i++) {
                z = nodes[i].z;
                if (z < -1.e20  ||  z > 1.e20) {
                    continue;
                }
                if (z < zmin) zmin = z;
                if (z > zmax) zmax = z;
            }
        }
        ndec = 0;
        zmax -= zmin;
        if (zmax < 10000.0) {
            ndec = 1;
        }
        if (zmax < 1000.0) {
            ndec = 2;
        }
        if (zmax < 100.0) {
            ndec = 3;
        }
        if (zmax < 10.0) {
            ndec = 4;
        }
        if (zmax > 1.e10) {
            ndec = 4;
        }

        tsize = conprop.nodeValueSize;
        if (conprop.showNodes) {
            dlist->SetTextOffsets (0.0, -(tsize + size * 2.0));
            dlist->SetTextAnchor (8);
        }
        else {
            dlist->SetTextOffsets (0.0, 0.0);
            dlist->SetTextAnchor (5);
        }
        dlist->SetTextColor (conprop.redNodeValue,
                             conprop.greenNodeValue,
                             conprop.blueNodeValue,
                             conprop.alphaNodeValue);
        dlist->SetTextFont (conprop.contourFont);

        if (data != NULL) {
            for (i=0; i<nrow; i++) {
                y = gymin + i * yspace;
                jstart = i * ncol;
                for (j=0; j<ncol; j++) {
                    k = jstart + j;
                    z = data[k];
                    if (z < -1.e20  ||  z > 1.e20) {
                        continue;
                    }
                    x = gxmin + xspace * j;
                    dlist->AddNumber (x, y, size, 0.0, z, ndec, 0);
                }
            }
        }
        else if (nodes != NULL) {
            for (i=0; i<num_nodes; i++) {
                z = nodes[i].z;
                if (z > 1.e20  ||  z < -1.e20) {
                    continue;
                }
                x = nodes[i].x;
                y = nodes[i].y;
                dlist->AddNumber (x, y, size, 0.0, z, ndec, 0);
            }
        }
    }

    dlist->SetGridNum (-1);
    dlist->SetImageID (-1);

    return 1;
}


/*--------------------------------------------------------------------------*/

int DLSurf::rotate_grid (void)
{
    double        cang, sang, xt, yt, xt2, yt2,
                  x1, y1t, x2, y2, xspace, yspace,
                  xp[5], yp[5];
    double        *xpts, *ypts, *zpts;
    int           i, n, nc2, nr2, npts, ncomp;
    int           istat, irow, jcol;
    GRidCalcOptions  options;
    char          *mask;

    if (data == NULL  ||  ncol < 2  ||  nrow < 2) {
        return -1;
    }

    cang = cos (angle * 3.1415926 / 180.0);
    sang = sin (angle * 3.1415926 / 180.0);

  /*
   * Allocate space for xyz points in the grid.
   */
    xpts = (double *)csw_Malloc (3 * ncol * nrow * sizeof(double));
    if (xpts == NULL) {
        return -1;
    }
    ypts = xpts + ncol * nrow;
    zpts = ypts + ncol * nrow;

  /*
   * Get the polygon boundary of the rotated grid.
   */
    xp[0] = xmin;
    yp[0] = ymin;
    xp[1] = xmin + width * cang;
    yp[1] = ymin + width * sang;
    xp[3] = xmin - height * sang;
    yp[3] = ymin + height * cang;
    xp[2] = xp[3] + width * cang;
    yp[2] = yp[3] + width * sang;
    xp[4] = xp[0];
    yp[4] = yp[0];

  /*
   * Get the geometry for the rotated grid.
   */
    x1 = 1.e30;
    y1t = 1.e30;
    x2 = -1.e30;
    y2 = -1.e30;
    for (i=0; i<4; i++) {
        if (xp[i] < x1) x1 = xp[i];
        if (yp[i] < y1t) y1t = yp[i];
        if (xp[i] > x2) x2 = xp[i];
        if (yp[i] > y2) y2 = yp[i];
    }

    gxmin = x1;
    gymin = y1t;
    gxmax = x2;
    gymax = y2;

    xspace = width / (ncol - 1);
    yspace = height / (nrow - 1);

    nc2 = (int)((x2 - x1) / xspace + 1);
    nr2 = (int)((y2 - y1t) / yspace + 1);

    if (nc2 < 2) nc2 = 2;
    if (nr2 < 2) nr2 = 2;

  /*
   * Fill in the xyz points.
   */
    n = 0;
    for (i=0; i<ncol*nrow; i++) {
        if (data[i] < -1.e20  ||  data[i] > 1.e20) {
            continue;
        }
        jcol = i % ncol;
        irow = i / ncol;
        xt = jcol * xspace;
        yt = irow * yspace;

        xt2 = xt * cang - yt * sang;
        yt2 = xt * sang + yt * cang;

        xpts[n] = xmin + xt2;
        ypts[n] = ymin + yt2;
        zpts[n] = data[i];

        n++;
    }

  /*
   * Free the old grid and allocate the new grid.
   */
    csw_Free (data);
    data = NULL;
    ncol = 0;
    nrow = 0;
    data = (CSW_F *)csw_Malloc (nc2 * nr2 * sizeof(CSW_F));
    if (data == NULL) {
        csw_Free (xpts);
        return -1;
    }

    grdapi_obj.grd_DefaultCalcOptions (&options);

    istat = grdapi_obj.grd_CalcGridFromDouble
       (xpts, ypts, zpts, NULL, n,
        data, NULL, NULL, nc2, nr2,
        x1, y1t, x2, y2,
        faults, nfaults,
        &options);
    csw_Free (xpts);
    xpts = NULL;
    ypts = NULL;
    zpts = NULL;
    if (istat == -1) {
        csw_Free (data);
        data = NULL;
        return -1;
    }

    ncol = nc2;
    nrow = nr2;
    needs_rotation = 0;
    angle = 0.0;

  /*
   * Set nodes outside of the original polygon to hard nulls.
   * If this fails, its ok.  The whole grid without nulls
   * will be used.
   */
    mask = (char *)csw_Calloc (ncol * nrow * sizeof(char));
    if (mask != NULL) {
        npts = 5;
        ncomp = 1;
        istat = grdapi_obj.grd_MaskWithPolygon (mask, ncol, nrow,
                                     x1, y1t, x2, y2,
                                     GRD_OUTSIDE_POLYGON,
                                     xp, yp, 1, &npts, &ncomp);
        for (i=0; i<ncol*nrow; i++) {
            if (mask[i] != 0) {
                data[i] = 1.e30f;
            }
        }
        csw_Free (mask);
        mask = NULL;
    }

    return 1;

}

/*--------------------------------------------------------------------------*/

int DLSurf::calc_contours_via_grid (
    NOdeStruct            *local_nodes,
    int                   numnodes,
    EDgeStruct            *local_edges,
    int                   numedges,
    TRiangleStruct        *local_triangles,
    int                   numtriangles,
    COntourOutputRec      **contoursout,
    int                   *ncontoursout,
    COntourCalcOptions    *local_calc_options)
{

    int                   istat, nc, nr, ntot;
    CSW_F                 *gdata;
    double                aspect;

/*
 * Create a grid with about 4 times the number of nodes
 * as there are triangles in the trimesh.  A minimum of
 * 1000 nodes is also applied to the grid.
 */
    if (trigrid == NULL) {
        ntot = numnodes * 4;
        if (ntot < 1000) ntot = 1000;
        aspect = (gymax - gymin) / (gxmax - gxmin);
        nc = (int)(sqrt (ntot / aspect) + .5);
        if (nc < 2) nc = 2;
        nr = ntot / nc;
        if (nr < 2) nr = 2;

        gdata = (CSW_F *)csw_Malloc (nc * nr * sizeof(CSW_F));
        if (gdata == NULL) {
            return -1;
        }

        istat =
        grdapi_obj.grd_CalcGridFromTriMeshNodes (
            local_nodes, numnodes,
            gdata, nc, nr,
            gxmin, gymin, gxmax, gymax);
        if (istat == -1) {
            csw_Free (gdata);
            return -1;
        }
        trigrid = gdata;
        tncol = nc;
        tnrow = nr;
    }
    else {
        gdata = trigrid;
        nc = tncol;
        nr = tnrow;
    }

/*
 * Calculate contours from the grid.
 */
    COntourOutputRec    *contours;
    int                 ncontours;

    istat =
    conapi_obj.con_CalcContoursFromDouble (
        gdata, nc, nr,
        gxmin, gymin, gxmax, gymax, 0.0,
        &contours, &ncontours,
        NULL, 0,
        local_calc_options);
    gdata = NULL;
    if (istat == -1) {
        return -1;
    }

/*
 * Get the outlines polygon of the trimesh.
 */
    double        *xout, *yout, *zout;
    int           npout, *nodeout, *ncout, *nvout;
    int           maxout;

    maxout = numnodes;
    if (maxout < 1000) maxout = 1000;

    xout = (double *)csw_Malloc (maxout * 5 * sizeof(double));
    if (xout == NULL) {
        return -1;
    }
    yout = xout + maxout;
    zout = yout + maxout;
    nodeout = (int *)(zout + maxout);
    ncout = nodeout + maxout;
    nvout = (int *)csw_Malloc (maxout * sizeof(int));
    if (nvout == NULL) {
        csw_Free (xout);
        return -1;
    }

    istat =
    grdapi_obj.grd_OutlineTriMeshBoundary (
        local_nodes,
        numnodes,
        local_edges,
        numedges,
        local_triangles,
        numtriangles,
        xout, yout, zout,
        nodeout,
        &npout, ncout, nvout,
        maxout, maxout);
    if (istat == -1) {
        csw_Free (xout);
        csw_Free (nvout);
        conapi_obj.con_FreeContours (contours, ncontours);
        return -1;
    }

/*
 * Make sure the boundary components are nested with the outside
 * border being the first component.
 */
    double        *xout_out, *yout_out, *zout_out;
    int           npout_out, *ncout_out, *nvout_out;

    xout_out = (double *)csw_Malloc (maxout * 4 * sizeof(double));
    if (xout_out == NULL) {
        csw_Free (xout);
        csw_Free (nvout);
        return -1;
    }
    yout_out = xout_out + maxout;
    zout_out = yout_out + maxout;
    ncout_out = (int *)(zout_out + maxout);
    nvout_out = (int *)csw_Malloc (maxout * sizeof(int));
    if (nvout_out == NULL) {
        csw_Free (xout_out);
        csw_Free (xout);
        csw_Free (nvout);
        return -1;
    }
    npout_out = 0;

    istat =
    ply_utils_obj.ply_holnestxyz (
        xout, yout, zout,
        ncout[0], nvout,
        xout_out, yout_out, zout_out,
        &npout_out, ncout_out, nvout_out,
        maxout, maxout);
    if (istat != 0  ||  npout_out <= 0) {
        conapi_obj.con_FreeContours (contours, ncontours);
        csw_Free (xout);
        csw_Free (nvout);
        csw_Free (xout_out);
        csw_Free (nvout_out);
        return -1;
    }

/*
 * If the number of output polygons is not 1, the trimesh
 * does not have edge connectivity.  In this case, the
 * polygon with the most points is used for clipping.
 * The code below finds the polygon with the most points and
 * copies it to the start of the x, y and z output arrays.
 */
    if (npout_out > 1) {
        int       ibig, nbig, i, j, k, n, nt1, nntot, ntbig;
        int       nvbig, nv1;
        ibig = -1;
        nbig = -1;
        ntbig = -1;
        nvbig = -1;
        k = 0;
        nntot = 0;
        for (i=0; i<npout_out; i++) {
            n = 0;
            nt1 = nntot;
            nv1 = k;
            for (j=0; j<ncout_out[i]; j++) {
                n += nvout_out[k];
                nntot += nvout_out[k];
                k++;
            }
            if (n > nbig) {
                nbig = n;
                ibig = i;
                ntbig = nt1;
                nvbig = nv1;
            }
        }
        if (ibig > 0  &&  ntbig > 0  &&  nbig > 0  &&  nvbig > 0) {
            double *dwork = (double *)csw_Malloc (maxout * sizeof(double));
            if (dwork == NULL) {
                conapi_obj.con_FreeContours (contours, ncontours);
                csw_Free (xout);
                csw_Free (nvout);
                csw_Free (xout_out);
                csw_Free (nvout_out);
                return -1;
            }
            memcpy (dwork, xout_out + ntbig, nbig * sizeof(double));
            memcpy (xout_out, dwork, nbig * sizeof(double));
            memcpy (dwork, yout_out + ntbig, nbig * sizeof(double));
            memcpy (yout_out, dwork, nbig * sizeof(double));
            memcpy (dwork, zout_out + ntbig, nbig * sizeof(double));
            memcpy (zout_out, dwork, nbig * sizeof(double));
            csw_Free (dwork);
            dwork = NULL;
            ncout_out[0] = ncout_out[ibig];
            for (j=0; j<ncout_out[ibig]; j++) {
                nvout_out[j] = nvout_out[j+nvbig];
            }
        }
    }

/*
 * Save the trimesh outline in member variables.
 */
    xpoly = xout_out;
    ypoly = yout_out;
    zpoly = zout_out;
    npoly = ncout_out[0];
    nvpoly = nvout_out;

/*
 * Clip the contours to the border of the trimesh.
 */
    COntourOutputRec        *clipcon;
    int                     nclipcon;

    istat =
    conapi_obj.con_ClipContoursToPolygon (
        1, // inside
        contours,
        ncontours,
        xout, yout, nvout, ncout[0],
        &clipcon, &nclipcon);
    conapi_obj.con_FreeContours (contours, ncontours);

    csw_Free (xout);
    csw_Free (nvout);
    xout = yout = zout = NULL;
    nvout = ncout = NULL;

    if (istat == -1) {
        return -1;
    }

/*
 * Return the clipped contours.
 */
    *contoursout = clipcon;
    *ncontoursout = nclipcon;

    return 1;

}

/*--------------------------------------------------------------------------*/

int DLSurf::CalcEdges (void *vptr)
{
    CDisplayList        *dlist;
    double              xspace, yspace, x, y, z;
    double              xa[2], ya[2];
    int                 i, j, jstart, k;
    int                 n1, n2;

    if (vptr == NULL) {
        return 1;
    }

    if (data != NULL) {
        if (ncol < 2  ||  nrow < 2) {
            return 1;
        }
    }

    if (conprop.showCellEdges == 0) {
        return 1;
    }

    if (conprop.showColorFills == 0) {
        image_id = -1;
    }

    dlist = (CDisplayList *)vptr;

    yspace = (gymax - gymin) / (nrow - 1);
    xspace = (gxmax - gxmin) / (ncol - 1);

    if (conprop.showCellEdges) {
        dlist->SetFrameNum (frame_num);
        dlist->SetGridNum (10000 + index_num );
        dlist->SetLineColor (conprop.redCellEdge,
                             conprop.greenCellEdge,
                             conprop.blueCellEdge,
                             conprop.alphaCellEdge);
        dlist->SetLineThickness (0.001);
    /*
     * Draw grid cell edges.
     */
        if (data != NULL) {
            for (i=0; i<nrow; i++) {
                y = gymin + i * yspace;
                jstart = i * ncol;
                ya[0] = y;
                ya[1] = y;
                for (j=0; j<ncol-1; j++) {
                    k = jstart + j;
                    z = data[k];
                    if (z < -1.e20  ||  z > 1.e20) {
                        continue;
                    }
                    z = data[k+1];
                    if (z < -1.e20  ||  z > 1.e20) {
                        continue;
                    }
                    xa[0] = gxmin + xspace * j;
                    xa[1] = xa[0] + xspace;
                    dlist->AddLine (xa, ya, 2);
                }
            }
            for (j=0; j<ncol; j++) {
                x = gxmin + j * xspace;
                xa[0] = x;
                xa[1] = x;
                for (i=0; i<nrow-1; i++) {
                    k = i * ncol + j;
                    z = data[k];
                    if (z < -1.e20  ||  z > 1.e20) {
                        continue;
                    }
                    z = data[k+ncol];
                    if (z < -1.e20  ||  z > 1.e20) {
                        continue;
                    }
                    ya[0] = gymin + yspace * i;
                    ya[1] = ya[0] + yspace;
                    dlist->AddLine (xa, ya, 2);
                }
            }
        }
        else if (edges != NULL  &&  nodes != NULL) {
            for (i=0; i<num_edges; i++) {
                if (edges[i].deleted == 1) {
                    continue;
                }
                n1 = edges[i].node1;
                n2 = edges[i].node2;
                if (n1 < 0  ||  n2 < 0) {
                    continue;
                }
                xa[0] = nodes[n1].x;
                ya[0] = nodes[n1].y;
                xa[1] = nodes[n2].x;
                ya[1] = nodes[n2].y;
                dlist->AddLine (xa, ya, 2);
            }
        }
    }

    dlist->SetGridNum (-1);

    return 1;
}




int DLSurf::calc_outline_polygon (void)
{

/*
 * Get the outlines polygon of the trimesh.
 */
    double        *xout, *yout, *zout;
    int           npout, *nodeout, *ncout, *nvout;
    int           maxout, istat;

    maxout = num_nodes;
    if (maxout < 1000) maxout = 1000;

    xout = (double *)csw_Malloc (maxout * 5 * sizeof(double));
    if (xout == NULL) {
        return -1;
    }
    yout = xout + maxout;
    zout = yout + maxout;
    nodeout = (int *)(zout + maxout);
    ncout = nodeout + maxout;
    nvout = (int *)csw_Malloc (maxout * sizeof(int));
    if (nvout == NULL) {
        csw_Free (xout);
        return -1;
    }

    istat =
    grdapi_obj.grd_OutlineTriMeshBoundary (
        nodes,
        num_nodes,
        edges,
        num_edges,
        triangles,
        num_triangles,
        xout, yout, zout,
        nodeout,
        &npout, ncout, nvout,
        maxout, maxout);
    if (istat == -1) {
        csw_Free (xout);
        csw_Free (nvout);
        return -1;
    }

/*
 * Make sure the boundary components are nested with the outside
 * border being the first component.
 */
    double        *xout_out, *yout_out, *zout_out;
    int           npout_out, *ncout_out, *nvout_out;

    xout_out = (double *)csw_Malloc (maxout * 5 * sizeof(double));
    if (xout_out == NULL) {
        csw_Free (xout);
        csw_Free (nvout);
        return -1;
    }
    yout_out = xout_out + maxout;
    zout_out = yout_out + maxout;
    ncout_out = (int *)(zout_out + maxout);
    nvout_out = ncout_out + maxout;
    npout_out = 0;

    istat =
    ply_utils_obj.ply_holnestxyz (
        xout, yout, zout,
        ncout[0], nvout,
        xout_out, yout_out, zout_out,
        &npout_out, ncout_out, nvout_out,
        maxout, maxout);
    csw_Free (xout);
    csw_Free (nvout);
    xout = yout = zout = NULL;
    nvout = ncout = nodeout = NULL;
    if (istat != 0  ||  npout_out <= 0) {
        csw_Free (xout_out);
        return -1;
    }

/*
 * Save the trimesh outline in member variables.
 */
    xpoly = xout_out;
    ypoly = yout_out;
    zpoly = zout_out;
    npoly = npout_out;
    ncpoly = ncout_out;
    nvpoly = nvout_out;

    return 1;

}

/*--------------------------------------------------------------------------*/

int DLSurf::CalcFaultLines (void *vptr)
{
    CDisplayList        *dlist;
    double              xa[2], ya[2];
    int                 i;
    int                 n1, n2;

    if (vptr == NULL) {
        return 1;
    }

    if (data != NULL) {
        if (ncol < 2  ||  nrow < 2) {
            return 1;
        }
    }

    if (conprop.showFaultLines == 0) {
        return 1;
    }

    if (conprop.showColorFills == 0) {
        image_id = -1;
    }

    dlist = (CDisplayList *)vptr;
    dlist->SetFrameNum (frame_num);

    dlist->SetGridNum (10000 + index_num );
    dlist->SetLineColor (conprop.redFaultLine,
                         conprop.greenFaultLine,
                         conprop.blueFaultLine,
                         conprop.alphaFaultLine);
    dlist->SetLineColor (0, 0, 0, 255);
    dlist->SetLineThickness (conprop.faultThickness);

    if (edges != NULL  &&  nodes != NULL) {
        for (i=0; i<num_edges; i++) {
            if (edges[i].deleted == 1) {
                continue;
            }
            if (edges[i].tri2 >= 0  ||  edges[i].flag == 0) {
                continue;
            }
            if (edges[i].flag == GRD_ZERO_DISCONTINUITY_CONSTRAINT  ||
                edges[i].flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT) {
                continue;
            }
            n1 = edges[i].node1;
            n2 = edges[i].node2;
            if (n1 < 0  ||  n2 < 0) {
                continue;
            }
            xa[0] = nodes[n1].x;
            ya[0] = nodes[n1].y;
            xa[1] = nodes[n2].x;
            ya[1] = nodes[n2].y;
            dlist->AddLine (xa, ya, 2);
        }
    }

    dlist->SetGridNum (-1);

    return 1;
}
