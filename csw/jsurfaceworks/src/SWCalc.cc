
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#include <math.h>
#include <stdlib.h>
#include <iostream>

#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

#define DEBUG_DRAPED_LINES 0

#include <assert.h>

#include "csw/jsurfaceworks/private_include/SurfaceWorksJNI.h"
#include "csw/jsurfaceworks/private_include/SWCalc.h"
#include "csw/surfaceworks/private_include/Vert.h"
#include "csw/surfaceworks/include/grid_api.h"
#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"
#include "csw/utils/private_include/gpf_utils.h"
#include "csw/utils/private_include/ply_protoP.h"



/*-----------------------------------------------------------------*/


/*
 * Constructor has an int parametger to use in opening a unique (for the
 * java virtual machine calling this) file name for playback testing.
 */

SWCalc::SWCalc (int ifile)
{

// Open the playback log file if needed.  This log is meant to stay
// open for the lifetime of the SWCalc object, so it is not manually
// closed.  The ofstream destructor will close it when the SWCalc
// object is deleted.

#ifdef _SW_DEBUG_LOG_FILE_
    char *pbenv = getenv ("CSW_DONT_WRITE_PLAYBACK");
    if (pbenv == NULL) {
        std::string pbf_name = "csw/jtest/swfile_" + std::to_string(ifile)+".txt";
        try {
            pbfile.open (pbf_name);
            if (pbfile.is_open() == false) {
                std::cout << "pbfile open failed" << std::endl;
            }
        }
        catch (std::exception &e) {
            std::cout << "Exception opening playback log file " << pbf_name << std::endl;
            std::cout << e.what() << std::endl;
        }
    }
#endif

}


void SWCalc::OutputForPlayback (const char *lfline) {

#ifdef _SW_DEBUG_LOG_FILE_
        try {
            if (pbfile.is_open()) {
                if (lfline != NULL) {
                    std::string pbstr;
                    pbstr.append (lfline);
                    pbfile << pbstr;
                }
                else {
                    pbfile.flush ();
                }
            }
        }
        catch (std::exception &e) {
            std::cout << "Exception in OutputForPlayback" << std::endl;
            std::cout << e.what() << std::endl;
        }
#endif

    };




/*
 * Specify the bounds of the model.  This must be done prior to
 * calculating any trimeshes, because the z values need to be
 * scaled to about the same range as the x and y values to get
 * reasonable triangles calculated.  If this is not called,
 * the calculation functions will return a failure.
 */
void SWCalc::sw_SetModelBounds (
    double     xmin,
    double     ymin,
    double     zmin,
    double     xmax,
    double     ymax,
    double     zmax)
{
    double     dx, dy, dz, scale;

    ModelXmin = 1.e30;
    ModelXmax = -1.e30;

    if (xmin >= xmax  ||
        ymin >= ymax  ||
        zmin >= zmax) {
        return;
    }

    ModelXmin = xmin;
    ModelXmax = xmax;

    dx = xmax - xmin;
    dy = ymax - ymin;
    dz = zmax - zmin;
    dx = (dx + dy) / 2.0;
    scale = dx / dz;

    if (scale < 0.001) scale = 0.001;


    return;

}



/*-----------------------------------------------------------------*/

/*
 * Calculate a tri mesh and return its nodes, edges and triangles
 * to the java side.  This function takes an array of points,
 * optional constraint line arrays, and an optional boundary polygon.
 * It calculates a trimesh clipped to the boundary.  The triangles will
 * be fairly regular except very near the boundary, where slivers may occur.
 *
 *  Input:
 *
 *    xpts         An array of the x coordinates of the points.
 *    ypts         An array of the y coordinates of the points.
 *    zpts         An array of the z coordinates of the points.
 *    npts         The number of points.
 *    xline        Optional array of constraint line x coordinates
 *    yline        Optional array of constraint line y coordinates
 *    zline        Optional array of constraint line z coordinates
 *    npline       Optional array specifying the number of points
 *                 in each separate constraint line.
 *    linetypes    Optional array specifying a type for each separate line.
 *    nline        Number of separate constraint lines.
 *    xbounds_in   Optional array of closed border x coordinates.
 *    ybounds_in   Optional array of closed border y coordinates.
 *    zbounds_in   Optional array of closed border z coordinates.
 *    nbounds_in   Number of points in the closed border.
 *    gflag        Currently ignored.
 *    grid_geometry   Grid geometry object or NULL to use default.
 *    grid_options    Grid options object or NULL to use default.
 *    fault_surface_flag
 *                 Set this to 2 if the points are on a vertical fault.
 *                 Set this to 1 if the points are on a fault surface or
 *                 to zero if on another type of surface.
 *    bad_bounds_flag
 *                 Flag telling what to do if the bounds arrays are NULL
 *                 1 = Use the bounding box of the points.
 *                 2 = Use the convex hull of the points.
 *
 */
int SWCalc::sw_CalcTriMesh (
    double *xpts,
    double *ypts,
    double *zpts,
    int    npts,
    double *xline,
    double *yline,
    double *zline,
    int    *npline,
    int    *linetypes,
    int    nline,
    double *xbounds_in,
    double *ybounds_in,
    double *zbounds_in,
    int    nbounds_in,
    int    gflag,
    SWGridGeometryStruct  *grid_geometry,
    SWGridOptionsStruct   *grid_options,
    int    fault_surface_flag,
    int    bad_bounds_flag
    )
{
    int              istat;
    double           gxmin, gymin, gxmax, gymax;
    double           dx, dy, xyspace;
    int              ncol, nrow;
    CSW_F            *gdata;
    FAultLineStruct  *flist;
    int              nflist;

    GRidCalcOptions  options;

    NOdeStruct       *nodes;
    EDgeStruct       *edges;
    TRiangleStruct   *triangles;
    int              num_nodes;
    int              num_edges;
    int              num_triangles;

    int              i, j, n, ntot, ntotvert;
    double           *xline2,
                     *yline2,
                     *zline2;
    int              *npline2,
                     *linetypes2,
                     nline2;

    double           *xbounds, *ybounds, *zbounds;
    int              nbounds;

    double           *xsav, *ysav, *zsav;
    double           avspace;

    double           *xpg, *ypg, *zpg;
    int              npg;
    int              vert_flag;
    int              convex_hull_flag;
    int              vused;
    double           vbase[6];

    double           zmin, zmax;

    int              uncrossFlag;

    int              do_write;
    char             fname[200];


/*
 *  !!!! Uncomment this when you need to write specific debugging files.
 */
    char             fcname[100];

    FaultSurfaceFlag = fault_surface_flag;

/*
 * The constraints should never be converted to skinny polygons
 * any more (May 2004).  I hard code the poly constraint flag
 * to zero to achieve this.
 */
    grdapi_ptr->grd_SetPolyConstraintFlag (0);

    if (bad_bounds_flag < 1  ||  bad_bounds_flag > 2) {
        bad_bounds_flag = 1;
    }

/*
 * Calculate the trimesh using an intermediate grid.
 * This is all that is currently supported.
 */
    if (gflag != 0) {

        xpg = NULL;
        ypg = NULL;
        zpg = NULL;
        ntot = 0;
        if (npline != NULL) {
            for (i=0; i<nline; i++) {
                ntot += npline[i];
            }
        }

        xpg = (double *)csw_Malloc ((npts + ntot) * 3 * sizeof(double));
        if (xpg == NULL) {
            return -1;
        }
        ypg = xpg + npts + ntot;
        zpg = ypg + npts + ntot;

        n = 0;
        for (i=0; i<npts; i++) {
            if (zpts[i] < 1.e20) {
                xpg[n] = xpts[i];
                ypg[n] = ypts[i];
                zpg[n] = zpts[i];
                n++;
            }
        }

        if (zline != NULL) {
            for (i=0; i<ntot; i++) {
                if (zline[i] < 1.e20) {
                    xpg[n] = xline[i];
                    ypg[n] = yline[i];
                    zpg[n] = zline[i];
                    n++;
                }
            }
        }

        npg = n;

        vert_flag = vert_GetFlag (gvert);

        vert_ConvertPoints (xpg, ypg, zpg, npg, gvert);
        vert_ConvertPoints (xline, yline, zline, ntot, gvert);
        ntotvert = ntot;
        vert_ConvertPoints (xbounds_in, ybounds_in, zbounds_in, nbounds_in, gvert);

    /*
     * If the removeCrossingConstraint flag in the grid options is set to
     * 1, then any constraint line that crosses a previous constraint is
     * not used.
     */
        uncrossFlag = 0;
        if (grid_options != NULL) {
            if (grid_options->removeCrossingConstraintFlag == 1) {
                uncrossFlag = 1;
            }
        }
        if (nline < 1  || xline == NULL  ||  yline == NULL  || npline == NULL) {
            uncrossFlag = 0;
        }
        if (uncrossFlag == 1) {
            istat =
            UncrossConstraints (xline, yline, zline, npline, &nline);
            if (istat == -1) {
                return -1;
            }
            ntot = 0;
            if (npline != NULL) {
                for (i=0; i<nline; i++) {
                    ntot += npline[i];
                }
            }
            ntotvert = ntot;
        }

    /*
     * Get or calculate the grid geometry.
     */
        gxmin = 1.e30;
        gymin = 1.e30;
        gxmax = -1.e30;
        gymax = -1.e30;

        if (grid_geometry != NULL  &&  vert_flag == 0) {
            ncol = grid_geometry->ncol;
            nrow = grid_geometry->nrow;
            gxmin = grid_geometry->xmin;
            gymin = grid_geometry->ymin;
            gxmax = grid_geometry->xmax;
            gymax = grid_geometry->ymax;
        }
        else {
            istat = grdapi_ptr->grd_RecommendedSizeFromDouble
                (xpg, ypg, npg, 0,
                 &gxmin, &gymin, &gxmax, &gymax,
                 &ncol, &nrow);
            if (istat == -1) {
                csw_Free (xpg);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
            if (fault_surface_flag) {
                ncol /= 2;
                nrow /= 2;
            }
/*
            else {
                ncol /= 2;
                nrow /= 2;
            }
*/
        }

        if (grid_geometry != NULL) {
            ncol = grid_geometry->ncol;
            nrow = grid_geometry->nrow;
        }

        while (ncol * nrow > MAX_DEFAULT_NODES) {
            ncol *= 4;
            nrow *= 4;
            ncol /= 5;
            nrow /= 5;
        }

        if (ncol < 2  ||  nrow < 2  ||
            gxmin >= gxmax  ||  gymin >= gymax) {
            istat = grdapi_ptr->grd_RecommendedSizeFromDouble
                (xpg, ypg, npg, 0,
                 &gxmin, &gymin, &gxmax, &gymax,
                 &ncol, &nrow);
            if (istat == -1) {
                csw_Free (xpg);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
            if (fault_surface_flag) {
                ncol /= 4;
                nrow /= 4;
            }
/*
            else {
                ncol /= 2;
                nrow /= 2;
            }
*/
            while (ncol * nrow > MAX_DEFAULT_NODES) {
                ncol *= 4;
                nrow *= 4;
                ncol /= 5;
                nrow /= 5;
            }
        }

        dx = (gxmax - gxmin) / (ncol - 1);
        dy = (gymax - gymin) / (nrow - 1);
        xyspace = (dx + dy) / 2.0;

        if (gridtri.getNodeList () != NULL) {
            printf ("non null node list just after xyspace =\n");
        }

    /*
     * If the boundary is not specified, use the bad bounds flag action
     * to get the boundary.
     */
        xbounds = NULL;
        ybounds = NULL;
        zbounds = NULL;
        nbounds = 0;
        convex_hull_flag = 0;

        if (xbounds_in != NULL  &&
            ybounds_in != NULL  &&
            zbounds_in != NULL  &&
            nbounds_in > 3) {
            xbounds = xbounds_in;
            ybounds = ybounds_in;
            zbounds = zbounds_in;
            nbounds = nbounds_in;

            if (xbounds[0] > 1.e20) {
                if (bad_bounds_flag == 1) {
                    xbounds = NULL;
                    ybounds = NULL;
                    zbounds = NULL;
                    nbounds = 0;
                }
                else {
                    zbounds = NULL;
                    istat =
                    grdapi_ptr->grd_CalcConvexHull (
                        xyspace,
                        xpg, ypg, npg,
                        &xbounds, &ybounds, &nbounds);
                    if (istat <= 0) {
                        xbounds = NULL;
                        ybounds = NULL;
                        zbounds = NULL;
                        nbounds = 0;
                    }
                    else {
                        convex_hull_flag = 1;
                    }
                }
            }
        }
        else {
            if (bad_bounds_flag == 1) {
                xbounds = NULL;
                ybounds = NULL;
                zbounds = NULL;
                nbounds = 0;
            }
            else {
                zbounds = NULL;
                istat =
                grdapi_ptr->grd_CalcConvexHull (
                    xyspace,
                    xpg, ypg, npg,
                    &xbounds, &ybounds, &nbounds);
                if (istat <= 0) {
                    xbounds = NULL;
                    ybounds = NULL;
                    zbounds = NULL;
                    nbounds = 0;
                }
                else {
                    convex_hull_flag = 1;
                }
            }
        }

        if (gridtri.getNodeList () != NULL) {
            printf ("non null node list just before gdata =\n");
        }

    /*
     * Allocate space for the grid.
     */
        gdata = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (gdata == NULL) {
            if (xbounds != xbounds_in) {
                csw_Free (xbounds);
                csw_Free (ybounds);
            }
            csw_Free (xpg);
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * Build the fault arrays if needed.  The gflag must be set to
     * 2 to get faulted grid calculation.
     */
        flist = NULL;
        nflist = 0;
        if (nline > 0  &&  gflag == 2) {
            istat = grdapi_ptr->grd_DoubleFaultArraysToStructs
                (xline, yline, zline,
                 npline, linetypes, nline,
                 &flist, &nflist);
            if (istat == -1) {
                if (xbounds != xbounds_in) {
                    csw_Free (xbounds);
                    csw_Free (ybounds);
                }
                csw_Free (gdata);
                csw_Free (xpg);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
        }

        if (gridtri.getNodeList () != NULL) {
            printf ("non null node list just after fault arrays to structs =\n");
        }

    /*
     * Transfer grid options.
     */
        ConvertGridCalcOptions (grid_options,
                                &options);
        options.triangulate_flag = 1;
        if (FaultSurfaceFlag != 0) {
            options.triangulate_flag = 3;
            options.anisotropy_flag = 0;
            if (FaultSurfaceFlag == 2) {
                options.triangulate_flag = 6;
            }
        }

    /*
     * Calculate the grid.
     */
        if (gridtri.getNodeList () != NULL) {
            printf ("non null node list just before calc grid from double\n");
        }
        istat = grdapi_ptr->grd_CalcGridFromDouble
            (xpg, ypg, zpg, NULL, npg,
             gdata, NULL, NULL,
             ncol, nrow,
             gxmin, gymin, gxmax, gymax,
             flist, nflist,
             &options);
        csw_Free (xpg);
        xpg = NULL;
        ypg = NULL;
        zpg = NULL;
        grdapi_ptr->grd_FreeFaultLineStructs (flist, nflist);
        flist = NULL;
        nflist = 0;
        if (istat == -1) {
            if (xbounds != xbounds_in) {
                csw_Free (xbounds);
                csw_Free (ybounds);
            }
            csw_Free (gdata);
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * Make a temporary set of constraint lines that includes
     * the lines passed to this function and the boundaries
     * passed to this function.
     */
        xline2 = NULL;
        yline2 = NULL;
        zline2 = NULL;
        npline2 = NULL;
        linetypes2 = NULL;
        nline2 = 0;
        if (fault_surface_flag == 2  &&  nbounds > 0) {
            nline = 0;
        }
        if (nline > 0  ||  nbounds > 0) {
            ntot = 0;
            if (nline > 0  &&  npline != NULL) {
                for (i=0; i<nline; i++) {
                    ntot += npline[i];
                }
            }
            ntot += nbounds;
            nline2 = nline;
            xline2 = (double *)csw_Malloc (ntot * sizeof(double));
            yline2 = (double *)csw_Malloc (ntot * sizeof(double));
            zline2 = (double *)csw_Malloc (ntot * sizeof(double));
            npline2 = (int *)csw_Malloc ((nline2 + 1) * sizeof(int));
            linetypes2 = (int *)csw_Calloc ((nline2 + 1) * sizeof(int));
            if (xline2 == NULL  ||
                yline2 == NULL  ||
                zline2 == NULL  ||
                npline2 == NULL  ||
                linetypes2 == NULL) {
                csw_Free (xline2);
                csw_Free (yline2);
                csw_Free (zline2);
                csw_Free (npline2);
                csw_Free (linetypes2);
                if (xbounds != xbounds_in) {
                    csw_Free (xbounds);
                    csw_Free (ybounds);
                }
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }

            n = 0;
            if (nline > 0  &&  npline != NULL  &&  xline != NULL  &&  yline != NULL) {
                ntot -= nbounds;
                memcpy (xline2, xline, ntot * sizeof(double));
                memcpy (yline2, yline, ntot * sizeof(double));
                if (zline != NULL) {
                    memcpy (zline2, zline, ntot * sizeof(double));
                }
                else {
                    for (j=0; j<ntot; j++) {
                        zline2[j] = 1.e30;
                    }
                }
                n += ntot;
                memcpy (npline2, npline, nline * sizeof(int));
                if (linetypes != NULL) {
                    memcpy (linetypes2, linetypes, nline * sizeof(int));
                }
            }
            else {
                ntot = 0;
            }

            if (nbounds > 0  &&  xbounds != NULL  &&  ybounds != NULL) {
                memcpy (xline2+n, xbounds, nbounds * sizeof(double));
                memcpy (yline2+n, ybounds, nbounds * sizeof(double));
                if (zbounds != NULL) {
                    memcpy (zline2+n, zbounds, nbounds * sizeof(double));
                }
                else {
                    for (j=0; j<nbounds; j++) {
                        zline2[n+j] = 1.e30;
                    }
                }
                nline2 = nline;
                npline2[nline2] = nbounds;
                linetypes2[nline2] = 0;
                nline2++;

            }


        /*
         * If any constraint points are undefined in z, attempt
         * to interpolate them from the grid.
         */
            for (i=0; i<ntot+nbounds; i++) {
                if (zline2[i] > 1.e20  ||  zline2[i] < -1.e20) {
                    grdapi_ptr->grd_BackInterpolateFromDouble (
                        gdata, ncol, nrow,
                        gxmin, gymin, gxmax, gymax,
                        NULL, 0,
                        xline2+i, yline2+i, zline2+i, 1,
                        GRD_BILINEAR);
                }
            }
        }

    /*
     * Resample the constraint lines at aproximately the grid spacing.
     */
        dx = (gxmax - gxmin) / (ncol - 1);
        dy = (gymax - gymin) / (nrow - 1);
        avspace = (dx + dy) / 2.0;
        xsav = xline2;
        ysav = yline2;
        zsav = zline2;

        if (xline2 != NULL  &&  npline2 != NULL  &&
            nline2 > 0) {
          istat =
          ResampleConstraintLines (
              &xline2, &yline2, &zline2,
              npline2, NULL, nline2, avspace);
          csw_Free (xsav);
          csw_Free (ysav);
          csw_Free (zsav);
          xsav = ysav = zsav = NULL;

          sprintf (fcname, "constraint_%d.xyz", FileID);
          WriteConstraintLines (fcname,
                                xline2, yline2, zline2,
                                npline2, nline2);
          FileID++;
        }

    /*
     * Calculate the trimesh from the grid.
     */
        nodes = NULL;
        edges = NULL;
        triangles = NULL;
        num_nodes = 0;
        num_edges = 0;
        num_triangles = 0;
        istat = grdapi_ptr->grd_CalcFaultedTriMeshFromGrid
                (gdata, ncol, nrow,
                 gxmin, gymin, gxmax, gymax,
                 xline2, yline2, zline2,
                 npline2, linetypes2, nline2,
                 GRD_EQUILATERAL,
                 &nodes, &edges, &triangles,
                 &num_nodes, &num_edges, &num_triangles);
        csw_Free (gdata);
        gdata = NULL;
        if (istat == -1) {
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            if (xbounds != xbounds_in) {
                csw_Free (xbounds);
                csw_Free (ybounds);
            }
            return -1;
        }

    /*
     * Free the temporary constraints.
     */
        csw_Free (xline2);
        csw_Free (yline2);
        csw_Free (zline2);
        csw_Free (npline2);
        csw_Free (linetypes2);
        nline2 = 0;
        xline2 = NULL;
        yline2 = NULL;
        zline2 = NULL;
        npline2 = NULL;
        linetypes2 = NULL;

        do_write = csw_GetDoWrite ();
        if (do_write) {
            strcpy (fname, "preclip.tri");
            grdapi_ptr->grd_WriteTextTriMeshFile (
                0, NULL,
                triangles, num_triangles,
                edges, num_edges,
                nodes, num_nodes,
                fname);
        }

    /*
     * Clip the trimesh to the boundary if needed.
     */
        if (nbounds > 0) {
            istat = grdapi_ptr->grd_ClipTriMeshToPolygon
                (&nodes, &num_nodes,
                 &edges, &num_edges,
                 &triangles, &num_triangles,
                 xbounds, ybounds, &nbounds, 1, 1);
            if (istat == -1) {
                csw_Free (nodes);
                csw_Free (edges);
                csw_Free (triangles);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                if (xbounds != xbounds_in) {
                    csw_Free (xbounds);
                    csw_Free (ybounds);
                }
                return -1;
            }
        }

        vert_GetBaseline (
            vbase,
            vbase+1,
            vbase+2,
            vbase+3,
            vbase+4,
            vbase+5,
            &vused, gvert);

        if (xbounds != xbounds_in  ||  convex_hull_flag == 1) {
            csw_Free (xbounds);
            csw_Free (ybounds);
            xbounds = NULL;
            ybounds = NULL;
        }

        do_write = csw_GetDoWrite ();
        if (do_write) {
            strcpy (fname, "convert.tri");
            grdapi_ptr->grd_WriteTextTriMeshFile (
                0, NULL,
                triangles, num_triangles,
                edges, num_edges,
                nodes, num_nodes,
                fname);
        }

    /*
     * Convert the trimesh nodes from steep surface coords to xyz if needed.
     */
        vert_UnconvertTriMeshNodes (nodes, num_nodes, gvert);

    /*
     * Convert the input line and bounds data back if needed.
     */
        vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
        vert_UnconvertPoints (xbounds, ybounds, zbounds, nbounds, gvert);

        vert_SendBackBaseline (v_jenv, v_jobj, gvert);

    /*
     * Convert the trimesh to arrays and send them back to the java side.
     */
        zmin = 1.e30;
        zmax = -1.e30;
        for (i=0; i<num_nodes; i++) {
            if (nodes[i].z < zmin) zmin = nodes[i].z;
            if (nodes[i].z > zmax) zmax = nodes[i].z;
        }

        if (zmin < -100000  ||  zmax < -100000  ||
            zmin > 100000  ||  zmax > 100000) {
            printf ("bad trimesh limits in SWCalc::sw_CalcTrimesh\n");
        }

        do_write = csw_GetDoWrite ();
        if (do_write) {
            strcpy (fname, "calc.tri");
            grdapi_ptr->grd_WriteTextTriMeshFile (
                0, NULL,
                triangles, num_triangles,
                edges, num_edges,
                nodes, num_nodes,
                fname);
        }

        istat =
        SendBackTriMesh (nodes, num_nodes,
                         edges, num_edges,
                         triangles, num_triangles);

        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
        nodes = NULL;
        edges = NULL;
        triangles = NULL;
        num_nodes = 0;
        num_edges = 0;
        num_triangles = 0;

        if (istat == -1) {
            return -1;
        }

    }  /* end of trimesh from grid block */

    return 1;
}



/*----------------------------------------------------------------------*/

int SWCalc::SendBackVbase (int vused, double *vbase)
{
    jni_call_set_vert_baseline_method
    (
        v_jenv,
        v_jobj,
        vbase[0],
        vbase[1],
        vbase[2],
        vbase[3],
        vbase[4],
        vbase[5],
        vused
    );

    return 1;
}

/*----------------------------------------------------------------------*/

int SWCalc::SendBackTriMesh
    (NOdeStruct *nodes, int num_nodes,
     EDgeStruct *edges, int num_edges,
     TRiangleStruct *triangles, int num_triangles)
{
    double         *xnode,
                   *ynode,
                   *znode;
    int            *flagnode;
    int            *n1edge,
                   *n2edge,
                   *t1edge,
                   *t2edge,
                   *flagedge;
    int            *e1tri,
                   *e2tri,
                   *e3tri,
                   *flagtri;

    int            i;
    NOdeStruct     *np;
    EDgeStruct     *ep;
    TRiangleStruct *tp;

/*
 * Check for obvious errors.
 */
    if (nodes == NULL  ||  num_nodes < 3  ||
        edges == NULL  ||  num_edges < 3  ||
        triangles == NULL  ||  num_triangles < 1) {
        return -1;
    }

/*
 * Allocate space for node arrays.
 */
    xnode = (double *)csw_Malloc (3 * num_nodes * sizeof(double));
    if (xnode == NULL) {
        return -1;
    }
    ynode = xnode + num_nodes;
    znode = ynode + num_nodes;

    flagnode = (int *)csw_Calloc (num_nodes * sizeof(int));
    if (flagnode == NULL) {
        csw_Free (xnode);
        return -1;
    }

/*
 * Allocate space for edge arrays.
 */
    n1edge = (int *)csw_Calloc (5 * num_edges * sizeof(int));
    if (n1edge == NULL) {
        csw_Free (xnode);
        csw_Free (flagnode);
        return -1;
    }
    n2edge = n1edge + num_edges;
    t1edge = n2edge + num_edges;
    t2edge = t1edge + num_edges;
    flagedge = t2edge + num_edges;

/*
 * Allocate space for triangle arrays.
 */
    e1tri = (int *)csw_Calloc (num_triangles * 4 * sizeof(int));
    if (e1tri == NULL) {
        csw_Free (xnode);
        csw_Free (flagnode);
        csw_Free (n1edge);
        return -1;
    }
    e2tri = e1tri + num_triangles;
    e3tri = e2tri + num_triangles;
    flagtri = e3tri + num_triangles;

/*
 * Copy node data to node arrays.
 */
    for (i=0; i<num_nodes; i++) {
        np = nodes + i;
        xnode[i] = np->x;
        ynode[i] = np->y;
        znode[i] = np->z;
        flagnode[i] = np->flag;
    }

/*
 * Copy edge data to edge arrays.
 */
    for (i=0; i<num_edges; i++) {
        ep = edges + i;
        n1edge[i] = ep->node1;
        n2edge[i] = ep->node2;
        t1edge[i] = ep->tri1;
        t2edge[i] = ep->tri2;
        flagedge[i] = ep->flag;
    }

/*
 * Copy triangle data to arrays.
 */
    for (i=0; i<num_triangles; i++) {
        tp = triangles + i;
        e1tri[i] = tp->edge1;
        e2tri[i] = tp->edge2;
        e3tri[i] = tp->edge3;
        flagtri[i] = tp->flag;
    }

/*
 * Send the arrays to the java side where a java TriMesh
 * object will be constructed using these data.
 */
    jni_call_add_tri_mesh_method
    (
        v_jenv,
        v_jobj,
        xnode,
        ynode,
        znode,
        flagnode,
        num_nodes,
        n1edge,
        n2edge,
        t1edge,
        t2edge,
        flagedge,
        num_edges,
        e1tri,
        e2tri,
        e3tri,
        flagtri,
        num_triangles
    );

/*
 * Free the arrays.
 */
    csw_Free (xnode);
    csw_Free (flagnode);
    csw_Free (n1edge);
    csw_Free (e1tri);

    return 1;

}

/*-----------------------------------------------------------------*/

void SWCalc::ConvertGridCalcOptions (
    SWGridOptionsStruct    *swop,
    GRidCalcOptions        *gop)
{
    if (gop == NULL) {
        return;
    }
    grdapi_ptr->grd_DefaultCalcOptions (gop);
    if (swop == NULL) {
        return;
    }

    gop->preferred_strike = swop->preferredStrike;
    gop->outside_margin = swop->outsideMargin;
    gop->inside_margin = swop->insideMargin;
    gop->fast_flag = swop->fastFlag;
    gop->min_value = (CSW_F)swop->minValue;
    gop->max_value = (CSW_F)swop->maxValue;
    gop->hard_min = (CSW_F)swop->hardMin;
    gop->hard_max = (CSW_F)swop->hardMax;
    gop->anisotropy_flag = swop->anisotropyFlag;
    gop->distance_power = swop->distancePower;
    gop->strike_power = swop->strikePower;
    gop->num_local_points = swop->numLocalPoints;
    gop->local_search_pattern = swop->localSearchPattern;
    gop->max_search_distance = (CSW_F)swop->maxSearchDistance;
    gop->thickness_flag = swop->thicknessFlag;
    gop->step_flag = swop->stepFlag;

    return;

}

/*-----------------------------------------------------------------*/

/*
 * Calculate a grid and return its nodes, and point errors
 * to the java side.
 */
int SWCalc::sw_CalcGrid (
    int    grsmooth,
    double *xpts,
    double *ypts,
    double *zpts,
    int    npts,
    double *xline,
    double *yline,
    double *zline,
    int    *npline,
    int    *linetypes,
    int    nline,
    double *xbounds,
    double *ybounds,
    int    nbounds,
    SWGridGeometryStruct  *grid_geometry,
    SWGridOptionsStruct   *grid_options
    )
{
    int              istat, ncomp;
    double           gxmin, gymin, gxmax, gymax;
    int              ncol, nrow;
    CSW_F            *gdata = NULL;
    CSW_F            *zerr = NULL;
    CSW_F            *rsgrid = NULL;
    char             *rsmask = NULL;
    char             *mask = NULL;

    FAultLineStruct  *flist = NULL;
    int              nflist;

    GRidCalcOptions  options;

    auto fscope = [&]()
    {
        csw_Free (gdata);
        csw_Free (mask);
        csw_Free (zerr);
        csw_Free (rsgrid);
        csw_Free (rsmask);
        csw_Free (flist);
    };
    CSWScopeGuard func_scope_guard (fscope);

    CSW_F    global_zdelta = 1.e30;
    CSW_F    local_zdelta_avg = 1.e30;

    int  nugget = grdapi_ptr->grd_EdgeNuggetEffect
          (xpts, ypts, zpts, npts,
           &global_zdelta, &local_zdelta_avg);
    int      noisy_edge = 0;

    if (nugget == 1  &&  fabs (global_zdelta) < 1.e20  &&
        fabs (local_zdelta_avg) < 1.e20) {
        if (local_zdelta_avg > global_zdelta * .1) {
            noisy_edge = 1;
        }
    }

/*
 * Get or calculate the grid geometry.  In the code below, I
 * limit the total number of grid nodes to 1000000 (for default
 * size) and to 2000000 for explicitly specified size. 
 */
    if (grid_geometry != NULL) {
        ncol = grid_geometry->ncol;
        nrow = grid_geometry->nrow;
        gxmin = grid_geometry->xmin;
        gymin = grid_geometry->ymin;
        gxmax = grid_geometry->xmax;
        gymax = grid_geometry->ymax;
        while (ncol * nrow > 2 * MAX_DEFAULT_NODES) {
            ncol *= 9;
            nrow *= 9;
            ncol /= 10;
            nrow /= 10;
        }
    }
    else {
        gxmin = 1.e30;
        gymin = 1.e30;
        gxmax = -1.e30;
        gymax = -1.e30;
        istat = grdapi_ptr->grd_RecommendedSizeFromDouble
            (xpts, ypts, npts, noisy_edge,
             &gxmin, &gymin, &gxmax, &gymax,
             &ncol, &nrow);
        if (istat == -1) {
            return -1;
        }
        while (ncol * nrow > MAX_DEFAULT_NODES) {
            ncol *= 9;
            nrow *= 9;
            ncol /= 10;
            nrow /= 10;
        }

/*
if (noisy_edge == 0) {
  CSW_F  ddx = (gxmax - gxmin) / 2.0;
  CSW_F  ddy = (gymax - gymin) / 2.0;
  gxmin -= ddx;
  gxmax += ddx;
  gymin -= ddy;
  gymax += ddy;
  ncol += ncol / 2;
  nrow += nrow / 2;
}
*/

    }

    if (ncol < 2  ||  nrow < 2  ||
        gxmin >= gxmax  ||  gymin >= gymax) {
        istat = grdapi_ptr->grd_RecommendedSizeFromDouble
            (xpts, ypts, npts, noisy_edge,
             &gxmin, &gymin, &gxmax, &gymax,
             &ncol, &nrow);
        if (istat == -1) {
            return -1;
        }
        while (ncol * nrow > MAX_DEFAULT_NODES) {
            ncol *= 9;
            nrow *= 9;
            ncol /= 10;
            nrow /= 10;
        }
    }

    if (ncol < 2) ncol = 2;
    if (nrow < 2) nrow = 2;

printf ("\nncol = %d   nrow = %d\n", ncol, nrow);
fflush (stdout);

/*
 * Allocate space for the grid, mask and error array.
 */
    gdata = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (gdata == NULL) {
        return -1;
    }

    mask = (char *)csw_Calloc (ncol * nrow * sizeof(char));
    if (mask == NULL) {
        return -1;
    }

    zerr = (CSW_F *)csw_Calloc (npts * sizeof(CSW_F));
    if (zerr == NULL) {
        return -1;
    }

/*
 * Build the fault arrays if needed.  Only lines with
 * flags of FAULT_CONSTRAINT (as defined in the java side)
 * will be used as faults.
 */
    flist = NULL;
    nflist = 0;
    if (nline > 0) {
        istat = grdapi_ptr->grd_DoubleFaultArraysToStructs
            (xline, yline, zline,
             npline, linetypes, nline,
             &flist, &nflist);
        if (istat == -1) {
            return -1;
        }
    }

/*
 * Transfer grid options.
 */
    ConvertGridCalcOptions (grid_options,
                            &options);

    CSW_F *gdc = gdata;
    char  *gdm = mask;

    int  n20 = 0;
    n20 = (ncol + nrow) / 40;
    if (n20 < 8) n20 = 8;
    double  gxmarg = 0.0;
    double  gymarg = 0.0;
    gxmarg = (gxmax - gxmin) / 40.0;
    gymarg = (gymax - gymin) / 40.0;

    if (grsmooth > 0) {
        n20 = (ncol + nrow) / 20;
        if (n20 < 16) n20 = 16;
        gxmarg = (gxmax - gxmin) / 20.0;
        gymarg = (gymax - gymin) / 20.0;
    }

    int  nrs = (ncol + n20) * (nrow + n20);
    rsgrid = (CSW_F *)csw_Malloc (nrs * sizeof(CSW_F));
    if (rsgrid == NULL) return -1;
    rsmask = (char *)csw_Malloc (nrs * sizeof(char));
    if (rsmask == NULL) return -1;
    gdc = rsgrid;
    gdm = rsmask;

    if (nugget == 1  &&  fabs (global_zdelta) < 1.e20  &&
        fabs (local_zdelta_avg) < 1.e20) {
        if (local_zdelta_avg > global_zdelta * .1) {
            grsmooth = 1000;
            options.moving_avg_only = 1;
        }
    }

/*
 * Calculate the grid.
 */
    istat = grdapi_ptr->grd_CalcGridFromDouble
        (xpts, ypts, zpts, NULL, npts,
         gdc, gdm, NULL,
         ncol + n20, nrow + n20,
         gxmin - gxmarg, gymin - gymarg,
         gxmax + gxmarg, gymax + gymarg,
         flist, nflist,
         &options);

    int  rstat = 0;
    if (grsmooth > 0) {
        grsmooth++;
        if (grsmooth > 9  &&  grsmooth < 100) grsmooth = 9;
        rstat =
        grdapi_ptr->grd_SmoothGrid 
            (gdc, ncol + n20, nrow + n20, grsmooth,
             flist, nflist,
             gxmin - gxmarg, gymin - gymarg,
             gxmax + gxmarg, gymax + gymarg,
             -1.e30, 1.e30, NULL);
        if (rstat == -1) return -1;
    }

// Resample to the "final" grid geometry.

/*
    CSW_F    ddxx = 0.0, ddyy = 0.0;
    if (nugget == 1  &&  grid_geometry == NULL) {
        ddxx = (gxmax - gxmin) / 100.0;
        ddyy = (gymax - gymin) / 100.0;
    }
*/

    rstat =
    grdapi_ptr->grd_ResampleGrid
        (gdc, gdm, ncol + n20, nrow + n20,
         gxmin - gxmarg, gymin - gymarg,
         gxmax + gxmarg, gymax + gymarg,
         flist, nflist,
         gdata, mask, ncol, nrow,
         gxmin, gymin, gxmax, gymax,
         GRD_BILINEAR);
    if (rstat == -1) return -1;

    grdapi_ptr->grd_FreeFaultLineStructs (flist, nflist);
    flist = NULL;
    nflist = 0;
    if (istat == -1) {
        return -1;
    }

/*
 * If there is a boundary, redo the mask accordingly.
 */
    if (xbounds != NULL  &&
        ybounds != NULL  &&
        nbounds > 0) {
        ncomp = 1;
        istat = grdapi_ptr->grd_MaskWithPolygon (mask, ncol, nrow,
                                     gxmin, gymin, gxmax, gymax,
                                     GRD_OUTSIDE_POLYGON,
                                     xbounds, ybounds, 1, &nbounds, &ncomp);
        if (istat == -1) {
            return -1;
        }
    }

    istat = SendBackGrid (
        gdata,
        ncol,
        nrow,
        gxmin,
        gymin,
        gxmax,
        gymax,
        zerr,
        npts,
        mask);

    return istat;
}



int SWCalc::SendBackGrid (
    CSW_F *gdata,
    int ncol,
    int nrow,
    double xmin,
    double ymin,
    double xmax,
    double ymax,
    CSW_F *point_errors,
    int npts,
    char *mask
)
{
    int           i, ntot;
    double        *ddata = NULL, *derror = NULL;

    auto fscope = [&]()
    {
        csw_Free (ddata);
        csw_Free (derror);
    };
    CSWScopeGuard func_scope_guard (fscope);

/*
 * If the double data cannot be allocated, it is an error.
 */
    ntot = ncol * nrow;
    ddata = (double *)csw_Malloc (ntot * sizeof(double));
    if (ddata == NULL) {
        return -1;
    }

/*
 * If the double point error cannot be allocated, just don't
 * send back any point error data.
 */
    derror = NULL;
    if (point_errors != NULL  &&  npts > 0) {
        derror = (double *)csw_Malloc (npts * sizeof(double));
    }
    if (derror == NULL) {
        npts = 0;
    }

/*
 * Transfer data and errors to double as needed.
 */
    for (i=0; i<ntot; i++) {
        ddata[i] = gdata[i];
    }

    if (derror != NULL  &&  point_errors != NULL) {
        for (i=0; i<npts; i++) {
            derror[i] = point_errors[i];
        }
    }

/*
 * Send the variables and arrays to the java side where a java Grid
 * object will be constructed using these data.
 */
    jni_call_add_grid_method
    (
        v_jenv,
        v_jobj,
        ddata,
        derror,
        mask,
        ncol,
        nrow,
        (double)xmin,
        (double)ymin,
        (double)xmax,
        (double)ymax,
        npts
    );

    return 1;

}


/*
 ************************************************************************************

    Functions for calculating draped lines over a trimesh.  These are only called by
    the SWCommand.cc command processor function.

 ************************************************************************************
*/

void SWCalc::free_drape_lines (void)
{
    csw_Free (drapeXline);
    csw_Free (drapeNpline);
    drapeNline = 0;
    drapeXline = NULL;
    drapeYline = NULL;
    drapeZline = NULL;
    drapeNpline = NULL;
}

void SWCalc::free_drape_points (void)
{
    csw_Free (drapeXpoint);
    drapeNpoint = 0;
    drapeXpoint = NULL;
    drapeYpoint = NULL;
    drapeZpoint = NULL;
}

void SWCalc::free_drape_trimesh (void)
{
    csw_Free (drapeNodes);
    csw_Free (drapeEdges);
    csw_Free (drapeTriangles);
    drapeNodes = NULL;
    drapeEdges = NULL;
    drapeTriangles = NULL;
    numDrapeNodes = 0;
    numDrapeEdges = 0;
    numDrapeTriangles = 0;

    numDrapeEdges = numDrapeEdges;
    numDrapeNodes = numDrapeNodes;
    numDrapeTriangles = numDrapeTriangles;
    drapeNline = drapeNline;

    drapeId1 = 0;
    drapeId2 = 0;
}

/*
 * Clean up all cached data for draped line calculation.
 */
void SWCalc::sw_ClearDrapeCache (void)
{
    free_drape_lines();
    free_drape_points();
    free_drape_trimesh ();
}

/*
 * Set the raw input lines.  These are allocated by the
 * calling function, but their ownership is relinquished
 * to this function, which is responsible for csw_Freeing
 * the line data.
 */
void SWCalc::sw_SetDrapeLineCache (

    double       *x,
    double       *y,
    double       *z,
    int          *np,
    int          nline
)
{
    int          i, ntot;

    free_drape_lines ();

    ntot = 0;
    for (i=0; i<nline; i++) {
        ntot += np[i];
    }

    drapeXline = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (drapeXline == NULL) {
        return;
    }
    drapeYline = drapeXline + ntot;
    drapeZline = drapeYline + ntot;

    drapeNpline = (int *)csw_Malloc (nline * sizeof(int));
    if (drapeNpline == NULL) {
        free_drape_lines ();
        return;
    }
    drapeNline = nline;

    memcpy (drapeXline, x, ntot * sizeof(double));
    memcpy (drapeYline, y, ntot * sizeof(double));
    memcpy (drapeZline, z, ntot * sizeof(double));
    memcpy (drapeNpline, np, nline * sizeof(int));

    vert_ConvertPoints (drapeXline, drapeYline, drapeZline, ntot, gvert);

}

/*
 * Set the raw input points.  These are allocated by the
 * calling function, but their ownership is relinquished
 * to this function, which is responsible for csw_Freeing
 * the point data.
 */
void SWCalc::sw_SetDrapePointCache (

    double       *x,
    double       *y,
    double       *z,
    int          ntot
)
{
    free_drape_points ();

    drapeXpoint = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (drapeXpoint == NULL) {
        return;
    }
    drapeYpoint = drapeXpoint + ntot;
    drapeZpoint = drapeYpoint + ntot;

    memcpy (drapeXpoint, x, ntot * sizeof(double));
    memcpy (drapeYpoint, y, ntot * sizeof(double));
    memcpy (drapeZpoint, z, ntot * sizeof(double));
    drapeNpoint = ntot;

    vert_ConvertPoints (x, y, z, ntot, gvert);

}

/*
 * Set up the trimesh data to be used in subsequent draped line
 * calculations.  The raw arrays are used to populate trimesh
 * objects.  The calling function maintains ownership of the
 * raw arrays.
 */
int SWCalc::sw_SetDrapeTriMeshCache (
    int        id1,
    int        id2,
    double     *xnode,
    double     *ynode,
    double     *znode,
    int        num_node,
    int        *n1edge,
    int        *n2edge,
    int        *t1edge,
    int        *t2edge,
    int        num_edge,
    int        *e1tri,
    int        *e2tri,
    int        *e3tri,
    int        num_tri
)
{
    int        i;

    free_drape_trimesh ();

    drapeId1 = id1;
    drapeId2 = id2;

    vert_ConvertPoints (xnode, ynode, znode, num_node, gvert);

    drapeNodes = (NOdeStruct *)csw_Calloc
                 (num_node * sizeof(NOdeStruct));
    if (drapeNodes == NULL) {
        return -1;
    }
    drapeEdges = (EDgeStruct *)csw_Calloc
                 (num_edge * sizeof(EDgeStruct));
    if (drapeEdges == NULL) {
        free_drape_trimesh ();
        return -1;
    }
    drapeTriangles = (TRiangleStruct *)csw_Calloc
                     (num_tri * sizeof(TRiangleStruct));
    if (drapeTriangles == NULL) {
        free_drape_trimesh ();
        return -1;
    }

    for (i=0; i<num_node; i++) {
        drapeNodes[i].x = xnode[i];
        drapeNodes[i].y = ynode[i];
        drapeNodes[i].z = znode[i];
    }
    numDrapeNodes = num_node;

    for (i=0; i<num_edge; i++) {
        drapeEdges[i].node1 = n1edge[i];
        drapeEdges[i].node2 = n2edge[i];
        drapeEdges[i].tri1 = t1edge[i];
        drapeEdges[i].tri2 = t2edge[i];
    }
    numDrapeEdges = num_edge;

    for (i=0; i<num_tri; i++) {
        drapeTriangles[i].edge1 = e1tri[i];
        drapeTriangles[i].edge2 = e2tri[i];
        drapeTriangles[i].edge3 = e3tri[i];
    }
    numDrapeTriangles = num_tri;

    return 1;

}

/*
 * Calculate the draped lines over the current cached trimesh.
 * Return the calculated lines back to the java side.
 */
int SWCalc::sw_CalcDrapedLines (void)
{
    double            *xout, *yout, *zout;
    int               *iout, nout;
    int               istat, i, n;


   /*
    *  !!!! uncomment for debug
    */
#if DEBUG_DRAPED_LINES
    double            vbase[6];
    char              line[200];
#endif

  /*
   *  !!!! uncomment for debug
   */
#if DEBUG_DRAPED_LINES
    sprintf (line, "dtnative_%d.tri", fileid);
    fileid++;
    vbase[0] = 1.e30;
    vbase[1] = 1.e30;
    vbase[2] = 1.e30;
    vbase[3] = 1.e30;
    vbase[4] = 1.e30;
    vbase[5] = 1.e30;
    grdapi_ptr->grd_WriteTextTriMeshFile (
        0, vbase,
        drapeTriangles, numDrapeTriangles,
        drapeEdges, numDrapeEdges,
        drapeNodes, numDrapeNodes,
        line);
#endif

    istat = grdapi_ptr->grd_DrapeLinesOnTriMesh (
        drapeId1, drapeId2,
        drapeNodes, numDrapeNodes,
        drapeEdges, numDrapeEdges,
        drapeTriangles, numDrapeTriangles,
        drapeXline, drapeYline, drapeNpline, drapeNline,
        &xout, &yout, &zout,
        &iout, &nout);

    if (istat == -1) {
        return -1;
    }

    n = 0;
    for (i=0; i<nout; i++) {
        n += iout[i];
    }

    vert_UnconvertPoints (xout, yout, zout, n, gvert);

    n = 0;
    for (i=0; i<nout; i++) {
        jni_call_add_draped_line_method (
            v_jenv,
            v_jobj,
            xout+n,
            yout+n,
            zout+n,
            iout[i]
        );
        n += iout[i];
    }

    csw_Free (xout);
    csw_Free (yout);
    csw_Free (zout);
    csw_Free (iout);

    return 1;

}


/*---------------------------------------------------------------------------*/

int SWCalc::sw_ConvertNodeTrimesh (
    double      *xnodes,
    double      *ynodes,
    double      *znodes,
    int         nnodes,
    int         *n1tri,
    int         *n2tri,
    int         *n3tri,
    int         ntri)
{
    NOdeStruct      *nodes;
    EDgeStruct      *edges;
    TRiangleStruct  *triangles;
    int             num_nodes;
    int             num_edges;
    int             num_triangles;
    NOdeTriangleStruct   *node_tris;

    int             i, istat;

/*
 * If there is no data, return an error.
 */
    if (ntri < 1  ||  nnodes < 1) {
        return -1;
    }

/*
 * Allocate space for the structure lists needed by
 * the csw library function and fill in the space.
 */
    node_tris = (NOdeTriangleStruct *)csw_Calloc
        (ntri * sizeof(NOdeTriangleStruct)
    );
    if (node_tris == NULL) {
        return -1;
    }

    for (i=0; i<ntri; i++) {
        node_tris[i].node1 = n1tri[i];
        node_tris[i].node2 = n2tri[i];
        node_tris[i].node3 = n3tri[i];
    }

    nodes = (NOdeStruct *)csw_Calloc
        (nnodes * sizeof (NOdeStruct)
    );
    if (nodes == NULL) {
        csw_Free (node_tris);
        return -1;
    }

    for (i=0; i<nnodes; i++) {
        nodes[i].x = xnodes[i];
        nodes[i].y = ynodes[i];
        nodes[i].z = znodes[i];
    }
    num_nodes = nnodes;

/*
 * Convert to a node, edge, triangle trimesh
 */
    istat = grdapi_ptr->grd_TriMeshFromNodeTriangles (
        node_tris,
        ntri,
        nodes,
        num_nodes,
        &edges, &num_edges,
        &triangles, &num_triangles);

    csw_Free (node_tris);
    node_tris = NULL;
    if (istat == -1) {
        csw_Free (nodes);
        return -1;
    }

/*
 * Check if the trimesh needs to use "steep" coordinates.
 */
    istat =
    check_for_steep (
        nodes, num_nodes,
        edges,
        triangles, num_triangles);
    if (istat == 1) {
        jni_call_set_vert_baseline_method (
            v_jenv,
            v_jobj,
            VBC1, VBC2, VBC3, VBX0,
            VBY0, VBZ0, VBFlag);
    }
    else {
        jni_call_set_vert_baseline_method (
            v_jenv,
            v_jobj,
            1.e30, 1.e30, 1.e30, 1.e30,
            1.e30, 1.e30, 0);
    }

/*
 * Convert the trimesh to arrays and send them back to the java side.
 */
    istat =
    SendBackTriMesh (nodes, num_nodes,
                     edges, num_edges,
                     triangles, num_triangles);

    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (triangles);

    return istat;

}


/*---------------------------------------------------------------------------*/

int SWCalc::sw_ReadGrid (char *fname)
{
    CSW_F      *gdata;
    char       *mask;
    int        ncol, nrow, gtype;
    double     gxmin, gymin, gxmax, gymax;

    int        istat;

    gdata = NULL;
    mask = NULL;
    istat = grdapi_ptr->grd_ReadFile (
        fname, NULL,
        &gdata, &mask, NULL,
        &ncol, &nrow,
        &gxmin, &gymin, &gxmax, &gymax,
        &gtype, NULL, NULL);
    if (istat == -1) {
        csw_Free (gdata);
        csw_Free (mask);
        return -1;
    }

    istat = SendBackGrid (
        gdata,
        ncol,
        nrow,
        gxmin,
        gymin,
        gxmax,
        gymax,
        NULL,
        0,
        mask);

    csw_Free (gdata);
    csw_Free (mask);

    return istat;
}

/*---------------------------------------------------------------------------*/

/*
 * Read a trimesh from the specified file and send it back to the java side.
 */

int SWCalc::sw_ReadTriMesh (char *fname)
{
    int            istat;
    int            vused;
    double         *vbase;
    TRiangleStruct *triangles;
    EDgeStruct     *edges;
    NOdeStruct     *nodes;
    int            ntri, nedge, nnode;

    istat =
    grdapi_ptr->grd_ReadTriMeshFile (
        fname,
        &vused, &vbase,
        &triangles, &ntri,
        &edges, &nedge,
        &nodes, &nnode);

    if (istat == -1) {
        return -1;
    }

    SendBackTriMesh (nodes, nnode,
                     edges, nedge,
                     triangles, ntri);
    SendBackVbase (vused, vbase);

    return 1;
}


/*---------------------------------------------------------------------------*/

/*
 * Read a trimesh from the specified file and send it back to the java side.
 */

int SWCalc::sw_ReadTriMeshFromMultiFile (char *fname, long position)
{
    int            istat;
    int            vused;
    double         *vbase;
    TRiangleStruct *triangles;
    EDgeStruct     *edges;
    NOdeStruct     *nodes;
    int            ntri, nedge, nnode;

    istat =
    grdapi_ptr->grd_ReadTriMeshFromMultiFile (
        fname, position,
        &vused, &vbase,
        &triangles, &ntri,
        &edges, &nedge,
        &nodes, &nnode);

    if (istat == -1) {
        return -1;
    }

    SendBackTriMesh (nodes, nnode,
                     edges, nedge,
                     triangles, ntri);
    SendBackVbase (vused, vbase);

    return 1;
}



/*---------------------------------------------------------------------------*/


/*
 * Write the trimesh data to the specified file in the specified format.
 */

int SWCalc::sw_WriteTriMesh (
    char      *fname,
    int       bflag,
    int       vused,
    double    *vbase,
    double    *xn,
    double    *yn,
    double    *zn,
    int       nnode,
    int       *n1edge,
    int       *n2edge,
    int       *t1edge,
    int       *t2edge,
    int       nedge,
    int       *e1tri,
    int       *e2tri,
    int       *e3tri,
    int       ntri)
{
    NOdeStruct         *nodes;
    EDgeStruct         *edges;
    TRiangleStruct     *triangles;

    int                i, istat;

/*
 * Allocate space for nodes, edges and triangles.
 */
    nodes = (NOdeStruct *)csw_Calloc
        (nnode * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc
        (nedge * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }

    triangles = (TRiangleStruct *)csw_Calloc
        (ntri * sizeof(TRiangleStruct));
    if (triangles == NULL) {
        csw_Free (edges);
        csw_Free (nodes);
        return -1;
    }

/*
 * fill in nodes, edges and triangles.
 */
    for (i=0; i<nnode; i++) {
        nodes[i].x = xn[i];
        nodes[i].y = yn[i];
        nodes[i].z = zn[i];
    }

    for (i=0; i<nedge; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<ntri; i++) {
        triangles[i].edge1 = e1tri[i];
        triangles[i].edge2 = e2tri[i];
        triangles[i].edge3 = e3tri[i];
    }

/*
 * write the data to text or binary.
 */
    istat = -1;
    if (bflag == 0) {
        istat =
        grdapi_ptr->grd_WriteTextTriMeshFile (
            vused, vbase,
            triangles, ntri,
            edges, nedge,
            nodes, nnode,
            fname);
    }

    else {
        istat =
        grdapi_ptr->grd_WriteBinaryTriMeshFile (
            vused, vbase,
            triangles, ntri,
            edges, nedge,
            nodes, nnode,
            fname);
    }

    return istat;

}

/*
 * Write the trimesh data to the specified file in the specified format.
 */
long SWCalc::sw_AppendTriMesh (
    char      *fname,
    int       bflag,
    int       vused,
    double    *vbase,
    double    *xn,
    double    *yn,
    double    *zn,
    int       nnode,
    int       *n1edge,
    int       *n2edge,
    int       *t1edge,
    int       *t2edge,
    int       nedge,
    int       *e1tri,
    int       *e2tri,
    int       *e3tri,
    int       ntri)
{
    NOdeStruct         *nodes;
    EDgeStruct         *edges;
    TRiangleStruct     *triangles;

    int                i;
    long istat;


    auto fscope = [&]()
    {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Allocate space for nodes, edges and triangles.
 */
    nodes = (NOdeStruct *)csw_Calloc (
        nnode * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc
        (nedge * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }

    triangles = (TRiangleStruct *)csw_Calloc
        (ntri * sizeof(TRiangleStruct));
    if (triangles == NULL) {
        csw_Free (edges);
        csw_Free (nodes);
        return -1;
    }

/*
 * fill in nodes, edges and triangles.
 */
    for (i=0; i<nnode; i++) {
        nodes[i].x = xn[i];
        nodes[i].y = yn[i];
        nodes[i].z = zn[i];
    }

    for (i=0; i<nedge; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<ntri; i++) {
        triangles[i].edge1 = e1tri[i];
        triangles[i].edge2 = e2tri[i];
        triangles[i].edge3 = e3tri[i];
    }

/*
 * write the data to text or binary.
 */
    istat = -1;
    if (bflag == 0) {
        istat =
        grdapi_ptr->grd_AppendTextTriMeshFile (
            vused, vbase,
            triangles, ntri,
            edges, nedge,
            nodes, nnode,
            fname);
    }

    else {
        istat =
        grdapi_ptr->grd_AppendBinaryTriMeshFile (
            vused, vbase,
            triangles, ntri,
            edges, nedge,
            nodes, nnode,
            fname);
    }
    return istat;
}


/*-------------------------------------------------------------------------------*/

/*
 * Extend a fault surface in its strike and dip directions, more or less.
 * This function does not return anything back to java.  It is used as a
 * utility by the patch splitting algorithm.
 */
int SWCalc::sw_ExtendFault (
    double           x_extension_fraction,
    double           y_extension_fraction,
    double           *xin,
    double           *yin,
    double           *zin,
    int              nin,
    TRiangleStruct   **triangles_out,
    int              *ntri_out,
    EDgeStruct       **edges_out,
    int              *nedge_out,
    NOdeStruct       **nodes_out,
    int              *nnode_out)
{
    double           *x, *y, *z;
    int              i, istat, nnode;
    double           xmin, ymin, xmax, ymax, dist;
    double           aspect;
    CSW_F            *gdata;
    int              ncol, nrow;
    GRidCalcOptions  options;
    TRiangleStruct   *triangles;
    EDgeStruct       *edges;
    NOdeStruct       *nodes;
    int              num_nodes, num_edges, num_triangles;
    int              maxnode;



/*
 * !!!! debug only
    FILE             *fptr;
    double           vbase[6];
    char             filename[100], filename2[100], line[200];
    strcpy (filename, "extend_fault.tri");
    strcpy (filename2, "extend_fault.xyz");
 */



/*
 * Initialize output in case of an error.
 */
    *triangles_out = NULL;
    *edges_out = NULL;
    *nodes_out = NULL;
    *ntri_out = 0;
    *nedge_out = 0;
    *nnode_out = 0;

    nnode = nin;

/*
 * Allocate space for old node xyz arrays.
 */
    x = (double *)csw_Malloc (3 * nnode * sizeof(double));
    if (x == NULL) {
        return -1;
    }
    y = x + nnode;
    z = y + nnode;

/*
 * Fill in points and convert to steep surface coordinates if needed.
 */
    for (i=0; i<nnode; i++) {
        x[i] = xin[i];
        y[i] = yin[i];
        z[i] = zin[i];
    }




/*
 * !!!! debug only
    fptr = fopen ("extend_fault.orig", "wb");
    if (fptr != NULL) {
        sprintf (line, "%d\n", nnode);
        fputs (line, fptr);
        for (i=0; i<nnode; i++) {
            sprintf (line, "%7.2f %7.2f %7.2f\n", x[i], y[i], z[i]);
            fputs (line, fptr);
        }
        fclose (fptr);
    }
    fptr = NULL;
 */




    vert_ConvertPoints (x, y, z, nnode, gvert);




/*
 * !!!! debug only
    fptr = fopen (filename2, "wb");
    if (fptr != NULL) {
        sprintf (line, "%d\n", nnode);
        fputs (line, fptr);
        for (i=0; i<nnode; i++) {
            sprintf (line, "%7.2f %7.2f %7.2f\n", x[i], y[i], z[i]);
            fputs (line, fptr);
        }
        fclose (fptr);
    }
    fptr = NULL;
 */






/*
 * Find the xy limits in the converted coordinates.
 */
    xmin = 1.e30;
    ymin = 1.e30;
    xmax = -1.e30;
    ymax = -1.e30;
    for (i=0; i<nnode; i++) {
        if (x[i] < xmin) xmin = x[i];
        if (y[i] < ymin) ymin = y[i];
        if (x[i] > xmax) xmax = x[i];
        if (y[i] > ymax) ymax = y[i];
    }

    if (xmax <= xmin  ||  ymax <= ymin) {
        csw_Free (x);
        return -1;
    }

/*
 * Calculate the grid size in rows and columns.
 */
    maxnode = nnode * 2;
    if (maxnode < 100) maxnode = 100;
    i = (int)(sqrt((double)nnode));
    aspect = (xmax - xmin) / (ymax - ymin);
    if (aspect >= 1.0) {
        nrow = i;
        ncol = (int)(aspect * nrow);
    }
    else {
        ncol = i;
        nrow = (int)(ncol / aspect);
    }

    while (ncol * nrow > maxnode) {
        ncol *= 4;
        ncol /= 5;
        nrow *= 4;
        nrow /= 5;
    }

    if (ncol < 2) ncol = 2;
    if (nrow < 2) nrow = 2;

    gdata = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
    if (gdata == NULL) {
        csw_Free (x);
        return -1;
    }

/*
 * Extend the xy limits.
 */
    dist = xmax - xmin;
    dist *= x_extension_fraction;
    xmin -= dist;
    xmax += dist;
    dist = ymax - ymin;
    dist *= y_extension_fraction;
    ymin -= dist;
    ymax += dist;

    grdapi_ptr->grd_DefaultCalcOptions (&options);
    options.triangulate_flag = 4;

/*
 * Calculate a grid to the extension limits.
 */
    istat =
    grdapi_ptr->grd_CalcGridFromDouble (
        x, y, z, NULL, nnode,
        gdata, NULL, NULL,
        ncol, nrow,
        xmin, ymin,
        xmax, ymax,
        NULL, 0, &options);

    csw_Free (x);
    x = NULL;
    y = NULL;
    z = NULL;

    if (istat == -1) {
        csw_Free (gdata);
        return -1;
    }

/*
 * Calculate the trimesh from the grid.
 */
    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    num_nodes = 0;
    num_edges = 0;
    num_triangles = 0;
    istat = grdapi_ptr->grd_CalcFaultedTriMeshFromGrid
        (gdata, ncol, nrow,
         xmin, ymin, xmax, ymax,
         NULL, NULL, NULL,
         NULL, NULL, 0,
         GRD_EQUILATERAL,
         &nodes, &edges, &triangles,
         &num_nodes, &num_edges, &num_triangles);
    csw_Free (gdata);
    gdata = NULL;
    if (istat == -1) {
        return -1;
    }


/*
 * !!!! debug only
    vbase[0] = 1.e30;
    vbase[1] = 1.e30;
    vbase[2] = 1.e30;
    vbase[3] = 1.e30;
    vbase[4] = 1.e30;
    vbase[5] = 1.e30;

    if (extension_fraction < 0.5) {
        grdapi_ptr->grd_WriteTextTriMeshFile (
            0,
            vbase,
            triangles, num_triangles,
            edges, num_edges,
            nodes, num_nodes,
            filename);
    }
 */



    vert_UnconvertTriMeshNodes (
        nodes,
        num_nodes, gvert);

/*
 * Return the extended trimesh.
 */
    *nodes_out = nodes;
    *edges_out = edges;
    *triangles_out = triangles;
    *ntri_out = num_triangles;
    *nedge_out = num_edges;
    *nnode_out = num_nodes;

    return 1;

}


/*-------------------------------------------------------------------------------*/

int SWCalc::sw_ExtendFaultFromJava (
    double         fraction,
    double         *xnode,
    double         *ynode,
    double         *znode,
    int            nnode)
{
    TRiangleStruct     *triangles;
    EDgeStruct         *edges;
    NOdeStruct         *nodes;
    int                num_nodes, num_edges, num_triangles;
    int                istat;

    if (nnode < 3) {
        return -1;
    }

    istat =
    sw_ExtendFault (
        fraction,
        fraction,
        xnode,
        ynode,
        znode,
        nnode,
        &triangles,
        &num_triangles,
        &edges,
        &num_edges,
        &nodes,
        &num_nodes);

    if (istat == -1) {
        return -1;
    }

/*
 * Convert the trimesh to arrays and send them back to the java side.
 */
    vert_SendBackBaseline (v_jenv, v_jobj, gvert);
    istat =
    SendBackTriMesh (nodes, num_nodes,
                     edges, num_edges,
                     triangles, num_triangles);

    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (triangles);
    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    num_nodes = 0;
    num_edges = 0;
    num_triangles = 0;

    if (istat == -1) {
        return -1;
    }

    return 1;

}


/*-------------------------------------------------------------------------*/

int SWCalc::sw_GridToTriMesh (
    int             ncol,
    int             nrow,
    double          xmin,
    double          ymin,
    double          xmax,
    double          ymax,
    double          rang,
    double          *gdata,
    double          *xline,
    double          *yline,
    double          *zline,
    int             *npline,
    int             *lineflags,
    int             nline)
{
    int             i, ntot, istat;
    CSW_F           *grid;
    double          zt;
    TRiangleStruct  *triangles;
    EDgeStruct      *edges;
    NOdeStruct      *nodes;
    int             num_nodes,
                    num_edges,
                    num_triangles;

    if (ncol < 2  ||  nrow < 2) {
        return -1;
    }

    if (xmin >= xmax  ||  ymin >= ymax) {
        return -1;
    }

    grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof (CSW_F));
    if (grid == NULL) {
        return -1;
    }

    ntot = ncol * nrow;
    for (i=0; i<ntot; i++) {
        zt = gdata[i];
        if (zt < -1.e20  ||  zt > 1.e20) {
            zt = 1.e30;
        }
        grid[i] = (CSW_F)zt;
    }

    istat = grdapi_ptr->grd_CalcTriMeshFromGrid (
        grid,
        ncol,
        nrow,
        xmin,
        ymin,
        xmax,
        ymax,
        xline, yline, zline,
        npline, lineflags, nline,
        GRD_EQUILATERAL,
        &nodes,
        &edges,
        &triangles,
        &num_nodes,
        &num_edges,
        &num_triangles);

    csw_Free (grid);

    if (istat == -1) {
        return -1;
    }

    istat =
    grdapi_ptr->grd_RemoveNullsFromTriMesh (
        nodes, &num_nodes,
        edges, &num_edges,
        triangles, &num_triangles,
        1.e20, -1);

    if (istat == -1) {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
        return -1;
    }

/*
 * Add the fault lines as constraints.
 */
    istat = grdapi_ptr->grd_AddLinesToTriMesh (
        xline, yline, zline,
        npline, lineflags, nline,
        0,
        &nodes, &edges, &triangles,
        &num_nodes, &num_edges, &num_triangles);

/*
 * TODO
 * If rang is not very nearly zero, rotate the node locations here.
 */
    rang = rang;

    istat =
    SendBackTriMesh (nodes, num_nodes,
                     edges, num_edges,
                     triangles, num_triangles);

    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (triangles);
    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    num_nodes = 0;
    num_edges = 0;
    num_triangles = 0;

    if (istat == -1) {
        return -1;
    }

    return 1;

}


/*------------------------------------------------------------------------------*/

/*
 * !!!! Function to write constraint lines to text file,
 * used for debuging only.  
 */
void SWCalc::WriteConstraintLines
(
    const char *fname,
    double *xline2,
    double *yline2,
    double *zline2,
    int *npline2,
    int nline2
)
{
    int    do_write, i;
    int    nv[1000];

    for (i=0; i<1000; i++) {
        nv[i] = 1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write) {
        if (nline2 > 1000) nline2 = 1000;
        grdapi_ptr->grd_WriteLines (xline2, yline2, zline2,
                        nline2, nv, npline2,
                        (char *)fname);
    }

    return;

}





/*-----------------------------------------------------------------*/

/*
 * Calculate a tri mesh and return its nodes, edges and triangles
 * to the calling function.  This function takes an array of points,
 * optional constraint line arrays, and an optional boundary polygon.
 * It calculates a trimesh clipped to the boundary.  The triangles will
 * be fairly regular except very near the boundary, where slivers may occur.
 *
 *  Input:
 *
 *    xpts         An array of the x coordinates of the points.
 *    ypts         An array of the y coordinates of the points.
 *    zpts         An array of the z coordinates of the points.
 *    npts         The number of points.
 *    xline        Optional array of constraint line x coordinates
 *    yline        Optional array of constraint line y coordinates
 *    zline        Optional array of constraint line z coordinates
 *    npline       Optional array specifying the number of points
 *                 in each separate constraint line.
 *    linetypes    Optional array specifying a type for each separate line.
 *    nline        Number of separate constraint lines.
 *    xbounds_in   Optional array of closed border x coordinates.
 *    ybounds_in   Optional array of closed border y coordinates.
 *    zbounds_in   Optional array of closed border z coordinates.
 *    nbounds_in   Number of points in the closed border.
 *    gflag        Currently ignored.
 *    grid_geometry   Grid geometry object or NULL to use default.
 *    grid_options    Grid options object or NULL to use default.
 *    fault_surface_flag
 *                 Set this to 1 if the points are on a fault surface or
 *                 to zero if on another type of surface.
 *    bad_bounds_flag
 *                 Flag telling what to do if the bounds arrays are NULL
 *                 1 = Use the bounding box of the points.
 *                 2 = Use the convex hull of the points.
 *
 *      Note: All the output arrays are allocated here in this function,
 *            but the ownership is relinquished to the calling function.
 *            The calling function should csw_Free the arrays when appropriate.
 *
 *    nodes_out           Returned NOdeStruct array.
 *    num_nodes_out       Number of nodes returned.
 *    edges_out           Returned EDgeStruct array.
 *    num_edges_out       Number of edges returned.
 *    triangles_out       Returned triangles array.
 *    num_triangles_out   Number of triangles returned.
 *
 */
int SWCalc::sw_CalcTriMeshLocally
(
    double *xpts,
    double *ypts,
    double *zpts,
    int    npts,
    double *xline,
    double *yline,
    double *zline,
    int    *npline,
    int    *linetypes,
    int    nline,
    double *xbounds_in,
    double *ybounds_in,
    double *zbounds_in,
    int    nbounds_in,
    int    gflag,
    SWGridGeometryStruct  *grid_geometry,
    SWGridOptionsStruct   *grid_options,
    int    fault_surface_flag,
    int    bad_bounds_flag,
    NOdeStruct    **nodes_out,
    int           *num_nodes_out,
    EDgeStruct    **edges_out,
    int           *num_edges_out,
    TRiangleStruct  **triangles_out,
    int           *num_triangles_out
)
{
    int              istat;
    double           gxmin, gymin, gxmax, gymax;
    double           dx, dy, xyspace;
    int              ncol, nrow;
    CSW_F            *gdata;
    FAultLineStruct  *flist;
    int              nflist;

    GRidCalcOptions  options;

    NOdeStruct       *nodes;
    EDgeStruct       *edges;
    TRiangleStruct   *triangles;
    int              num_nodes;
    int              num_edges;
    int              num_triangles;

    int              i, j, n, ntot, ntotvert;
    double           *xline2,
                     *yline2,
                     *zline2;
    int              *npline2,
                     *linetypes2,
                     nline2;

    double           *xbounds, *ybounds, *zbounds;
    int              nbounds;

    double           *xpg, *ypg, *zpg;
    int              npg;
    int              vert_flag;
    int              convex_hull_flag;
    int              vused;
    double           vbase[6];

/*
 *  !!!! Uncomment this when you need to write specific debugging files.
    char             fname[100];
 */
    char             fname[200];
    int              do_write;

    do_write = csw_GetDoWrite ();
    if (do_write != 0) {
        strcpy (fname, "exact.xyz");
        grdapi_ptr->grd_WriteLines (
            xline, yline, zline, 1,
            &nline, npline,
            fname);
    }




    FaultSurfaceFlag = fault_surface_flag;

/*
 * The constraints do not need to be converted to skinny polygons.
 */
    grdapi_ptr->grd_SetPolyConstraintFlag (0);

    if (bad_bounds_flag < 1  ||  bad_bounds_flag > 2) {
        bad_bounds_flag = 1;
    }

    if (gflag == 0) {
        gflag = 1;
    }

/*
 * Calculate the trimesh using an intermediate grid.
 * This is all that is currently supported.
 */
    if (gflag != 0) {

        xpg = NULL;
        ypg = NULL;
        zpg = NULL;
        ntot = 0;
        if (npline != NULL) {
            for (i=0; i<nline; i++) {
                ntot += npline[i];
            }
        }

        xpg = (double *)csw_Malloc ((npts + ntot) * 3 * sizeof(double));
        if (xpg == NULL) {
            return -1;
        }
        ypg = xpg + npts + ntot;
        zpg = ypg + npts + ntot;

        n = 0;
        for (i=0; i<npts; i++) {
            if (zpts[i] < 1.e20) {
                xpg[n] = xpts[i];
                ypg[n] = ypts[i];
                zpg[n] = zpts[i];
                n++;
            }
        }

        npg = n;

        vert_flag = vert_GetFlag (gvert);

        vert_ConvertPoints (xpg, ypg, zpg, npg, gvert);
        vert_ConvertPoints (xline, yline, zline, ntot, gvert);
        ntotvert = ntot;
        vert_ConvertPoints (xbounds_in, ybounds_in, zbounds_in, nbounds_in, gvert);

    /*
     * Get or calculate the grid geometry.
     */
        gxmin = 1.e30;
        gymin = 1.e30;
        gxmax = -1.e30;
        gymax = -1.e30;

        if (grid_geometry != NULL  &&  vert_flag == 0) {
            ncol = grid_geometry->ncol;
            nrow = grid_geometry->nrow;
            gxmin = grid_geometry->xmin;
            gymin = grid_geometry->ymin;
            gxmax = grid_geometry->xmax;
            gymax = grid_geometry->ymax;
        }
        else {
            istat = grdapi_ptr->grd_RecommendedSizeFromDouble
                (xpg, ypg, npg, 0,
                 &gxmin, &gymin, &gxmax, &gymax,
                 &ncol, &nrow);
            if (istat == -1) {
                csw_Free (xpg);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
            if (fault_surface_flag) {
                ncol *= 2;
                nrow *= 2;
                ncol /= 3;
                nrow /= 3;
            }
        }

        if (grid_geometry != NULL) {
            ncol = grid_geometry->ncol;
            nrow = grid_geometry->nrow;
        }

        while (ncol * nrow > MAX_DEFAULT_NODES) {
            ncol *= 4;
            nrow *= 4;
            ncol /= 5;
            nrow /= 5;
        }

        if (ncol < 2  ||  nrow < 2  ||
            gxmin >= gxmax  ||  gymin >= gymax) {
            istat = grdapi_ptr->grd_RecommendedSizeFromDouble
                (xpg, ypg, npg, 0,
                 &gxmin, &gymin, &gxmax, &gymax,
                 &ncol, &nrow);
            if (istat == -1) {
                csw_Free (xpg);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
            if (fault_surface_flag) {
                ncol /= 4;
                nrow /= 4;
            }
            else {
                ncol /= 2;
                nrow /= 2;
            }
            while (ncol * nrow > MAX_DEFAULT_NODES) {
                ncol *= 4;
                nrow *= 4;
                ncol /= 5;
                nrow /= 5;
            }
        }

        dx = (gxmax - gxmin) / (ncol - 1);
        dy = (gymax - gymin) / (nrow - 1);
        xyspace = (dx + dy) / 2.0;

    /*
     * If the boundary is not specified, use the bad bounds flag action
     * to get the boundary.
     */
        xbounds = NULL;
        ybounds = NULL;
        zbounds = NULL;
        nbounds = 0;
        convex_hull_flag = 0;

        if (xbounds_in != NULL  &&
            ybounds_in != NULL  &&
            zbounds_in != NULL  &&
            nbounds_in > 3) {
            xbounds = xbounds_in;
            ybounds = ybounds_in;
            zbounds = zbounds_in;
            nbounds = nbounds_in;

            if (xbounds[0] > 1.e20) {
                if (bad_bounds_flag == 1) {
                    xbounds = NULL;
                    ybounds = NULL;
                    zbounds = NULL;
                    nbounds = 0;
                }
                else {
                    zbounds = NULL;
                    istat =
                    grdapi_ptr->grd_CalcConvexHull (
                        xyspace,
                        xpg, ypg, npg,
                        &xbounds, &ybounds, &nbounds);
                    if (istat <= 0) {
                        xbounds = NULL;
                        ybounds = NULL;
                        zbounds = NULL;
                        nbounds = 0;
                    }
                    else {
                        convex_hull_flag = 1;
                    }
                }
            }
        }
        else {
            if (bad_bounds_flag == 1) {
                xbounds = NULL;
                ybounds = NULL;
                zbounds = NULL;
                nbounds = 0;
            }
            else {
                zbounds = NULL;
                istat =
                grdapi_ptr->grd_CalcConvexHull (
                    xyspace,
                    xpg, ypg, npg,
                    &xbounds, &ybounds, &nbounds);
                if (istat <= 0) {
                    xbounds = NULL;
                    ybounds = NULL;
                    zbounds = NULL;
                    nbounds = 0;
                }
                else {
                    convex_hull_flag = 1;
                }
            }
        }

    /*
     * Allocate space for the grid.
     */
        gdata = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (gdata == NULL) {
            csw_Free (xpg);
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * Build the fault arrays if needed.  The gflag must be set to
     * 2 to get faulted grid calculation.
     */
        flist = NULL;
        nflist = 0;
        if (nline > 0  &&  gflag == 2) {
            istat = grdapi_ptr->grd_DoubleFaultArraysToStructs
                (xline, yline, zline,
                 npline, linetypes, nline,
                 &flist, &nflist);
            if (istat == -1) {
                csw_Free (gdata);
                csw_Free (xpg);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
        }

    /*
     * Transfer grid options.
     */
        ConvertGridCalcOptions (grid_options,
                                &options);
        options.triangulate_flag = 0;
        if (FaultSurfaceFlag != 0) {
            options.triangulate_flag = 3;
            options.anisotropy_flag = 0;
            if (FaultSurfaceFlag == 2) {
                options.triangulate_flag = 6;
            }
        }

    /*
     * Calculate the grid.
     */
        istat = grdapi_ptr->grd_CalcGridFromDouble
            (xpg, ypg, zpg, NULL, npg,
             gdata, NULL, NULL,
             ncol, nrow,
             gxmin, gymin, gxmax, gymax,
             flist, nflist,
             &options);
        csw_Free (xpg);
        xpg = NULL;
        ypg = NULL;
        zpg = NULL;
        grdapi_ptr->grd_FreeFaultLineStructs (flist, nflist);
        flist = NULL;
        nflist = 0;
        if (istat == -1) {
            csw_Free (gdata);
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }




    /*
     * Make a temporary array of constraint lines that can be modified
     * (z values back interpolated from grid where needed).
     */
        xline2 = NULL;
        yline2 = NULL;
        zline2 = NULL;
        npline2 = NULL;
        linetypes2 = NULL;
        nline2 = 0;
        if (nline > 0) {
            ntot = 0;
            if (nline > 0  &&  npline != NULL) {
                for (i=0; i<nline; i++) {
                    ntot += npline[i];
                }
            }
            nline2 = nline;
            xline2 = (double *)csw_Malloc (ntot * sizeof(double));
            yline2 = (double *)csw_Malloc (ntot * sizeof(double));
            zline2 = (double *)csw_Malloc (ntot * sizeof(double));
            npline2 = (int *)csw_Malloc ((nline2 + 1) * sizeof(int));
            linetypes2 = (int *)csw_Calloc ((nline2 + 1) * sizeof(int));
            if (xline2 == NULL  ||
                yline2 == NULL  ||
                zline2 == NULL  ||
                npline2 == NULL  ||
                linetypes2 == NULL) {
                csw_Free (xline2);
                csw_Free (yline2);
                csw_Free (zline2);
                csw_Free (npline2);
                csw_Free (linetypes2);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }

            if (nline > 0  &&  npline != NULL) {
                memcpy (xline2, xline, ntot * sizeof(double));
                memcpy (yline2, yline, ntot * sizeof(double));
                if (zline != NULL) {
                    memcpy (zline2, zline, ntot * sizeof(double));
                }
                else {
                    for (j=0; j<ntot; j++) {
                        zline2[j] = 1.e30;
                    }
                }
                memcpy (npline2, npline, nline * sizeof(int));
                if (linetypes != NULL) {
                    memcpy (linetypes2, linetypes, nline * sizeof(int));
                }
            }

        /*
         * If any constraint points are undefined in z, attempt
         * to interpolate them from the grid.
         */
            for (i=0; i<ntot; i++) {
                if (zline2[i] > 1.e20  ||  zline2[i] < -1.e20) {
                    grdapi_ptr->grd_BackInterpolateFromDouble (
                        gdata, ncol, nrow,
                        gxmin, gymin, gxmax, gymax,
                        NULL, 0,
                        xline2+i, yline2+i, zline2+i, 1,
                        GRD_BICUBIC);
                }
            }
        }

    /*
     * Calculate the trimesh from the grid.
     */
        nodes = NULL;
        edges = NULL;
        triangles = NULL;
        num_nodes = 0;
        num_edges = 0;
        num_triangles = 0;
        istat = grdapi_ptr->grd_CalcFaultedTriMeshFromGrid
            (gdata, ncol, nrow,
             gxmin, gymin, gxmax, gymax,
             xline2, yline2, zline2,
             npline2, linetypes2, nline2,
             GRD_EQUILATERAL,
             &nodes, &edges, &triangles,
             &num_nodes, &num_edges, &num_triangles);
        csw_Free (gdata);
        gdata = NULL;
        if (istat == -1) {
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * !!!! Used for specific debugging only.  If needed,
     * uncomment this block of code.
     */
        if (do_write) {
            strcpy (fname, "preclipsteep.tri");
            vused = 0;
            vbase[0] = 1.e30;
            vbase[1] = 1.e30;
            vbase[2] = 1.e30;
            vbase[3] = 1.e30;
            vbase[4] = 1.e30;
            vbase[5] = 1.e30;
            grdapi_ptr->grd_WriteTextTriMeshFile (
                vused, vbase,
                triangles, num_triangles,
                edges, num_edges,
                nodes, num_nodes,
                fname);
        }



    /*
     * Free the temporary constraints.
     */
        csw_Free (xline2);
        csw_Free (yline2);
        csw_Free (zline2);
        csw_Free (npline2);
        csw_Free (linetypes2);
        nline2 = 0;
        xline2 = NULL;
        yline2 = NULL;
        zline2 = NULL;
        npline2 = NULL;
        linetypes2 = NULL;

    /*
     * Clip the trimesh to the boundary if needed.
     */
        if (nbounds > 0) {
            istat = grdapi_ptr->grd_ClipTriMeshToPolygon
                (&nodes, &num_nodes,
                 &edges, &num_edges,
                 &triangles, &num_triangles,
                 xbounds, ybounds, &nbounds, 1, 1);
            if (istat == -1) {
                csw_Free (nodes);
                csw_Free (edges);
                csw_Free (triangles);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
        }

        vert_GetBaseline (
            vbase,
            vbase+1,
            vbase+2,
            vbase+3,
            vbase+4,
            vbase+5,
            &vused, gvert);

    /*
     * !!!! Used for specific debugging only.  If needed,
     * uncomment this block of code.
     */
        if (do_write) {
            strcpy (fname, "clipsteep.tri");
            grdapi_ptr->grd_WriteTextTriMeshFile (
                vused, vbase,
                triangles, num_triangles,
                edges, num_edges,
                nodes, num_nodes,
                fname);
        }



        if (convex_hull_flag == 1  ||  xbounds != xbounds_in) {
            csw_Free (xbounds);
            csw_Free (ybounds);
            xbounds = NULL;
            ybounds = NULL;
        }

    /*
     * Convert the trimesh nodes from steep surface coords to xyz if needed.
     */
        vert_UnconvertTriMeshNodes (nodes, num_nodes, gvert);

    /*
     * Convert the input line and bounds data back if needed.
     */
        vert_UnconvertPoints (xline, yline, zline, ntot, gvert);
        vert_UnconvertPoints (xbounds, ybounds, zbounds, nbounds, gvert);

    /*
     * Assign the trimesh data to the output pointers.
     */
        *nodes_out = nodes;
        *edges_out = edges;
        *triangles_out = triangles;
        *num_nodes_out = num_nodes;
        *num_edges_out = num_edges;
        *num_triangles_out = num_triangles;

    }  /* end of trimesh from grid block */

    return 1;

}  /* end of sw_CalcTriMeshLocally function */


void SWCalc::sw_SetOutputShifts (double x, double y)
{
    XOutputShift = x;
    YOutputShift = y;
    XOutputShift = XOutputShift;
    YOutputShift = YOutputShift;
}

/*------------------------------------------------------------------*/

int SWCalc::sw_OutlinePoints (
    double    *xpts,
    double    *ypts,
    int       npts)
{
    double    *xpoly, *ypoly;
    int       npoly, istat;

    istat = grdapi_ptr->grd_CalcConvexHull (
        0.0,
        xpts, ypts, npts,
        &xpoly, &ypoly, &npoly);
    if (istat == -1) {
        return -1;
    }

    istat =
    SendBackPointOutline (xpoly, ypoly, npoly);
    csw_Free (xpoly);
    csw_Free (ypoly);

    return istat;

}

int SWCalc::SendBackPointOutline (
    double *xpoly,
    double *ypoly,
    int    npoly)
{
    int istat;

    istat =
    jni_call_set_outline_method (
        v_jenv,
        v_jobj,
        xpoly, ypoly, npoly);

    return istat;
}


/*-------------------------------------------------------------*/

int SWCalc::sw_PointInPolygon (
    double    x,
    double    y,
    double    graze,
    double    *xp,
    double    *yp,
    int       np)
{
    int       istat;
    double    gsav;

    polyutils.ply_getgraze (&gsav);
    polyutils.ply_setgraze (graze);

    istat = polyutils.ply_point (xp, yp, np, x, y);

    polyutils.ply_setgraze (gsav);

    return istat;
}

/*--------------------------------------------------------------------------*/

int SWCalc::check_for_steep (
    NOdeStruct    *nodes,
    int           num_nodes,
    EDgeStruct    *edges,
    TRiangleStruct  *triangles,
    int           num_triangles)
{
    int           i;
    double        x[4], y[4], z[4];
    double        zsum, anorm;
    double        *xpts, *ypts, *zpts;

    if (num_triangles < 1) {
        return 0;
    }

    zsum = 0.0;

    for (i=0; i<num_triangles; i++) {
        TrianglePoints (
            triangles+i,
            nodes,
            edges,
            x, y, z);
        TriangleNormal (x, y, z);
        anorm = tnzNorm;
        if (anorm < 0.0) {
            anorm = -anorm;
        }
        zsum += anorm;
    }
    tnxNorm = tnxNorm;
    tnyNorm = tnyNorm;

    zsum /= num_triangles;

    if (zsum > 0.5) {
        return 0;
    }

    xpts = (double *)csw_Malloc (3 * num_nodes * sizeof(double));
    if (xpts == NULL) {
        return 0;
    }
    ypts = xpts + num_nodes;
    zpts = ypts + num_nodes;

    for (i=0; i<num_nodes; i++) {
        xpts[i] = nodes[i].x;
        ypts[i] = nodes[i].y;
        zpts[i] = nodes[i].z;
    }

    VBFlag =
    vert_CalcBaseline (
        xpts, ypts, zpts,
        num_nodes,
        0, /* not a fault */
        &VBC1, &VBC2, &VBC3, &VBX0,
        &VBY0, &VBZ0);

    return VBFlag;
}


/*--------------------------------------------------------------------------------*/

/*
 * Calculate the normal vector to the specified triangle.
 * The normal will always have zero or positive z.  The
 * normal is scaled so that the length is 1.0.
 *
 * The components of the normal are put into the private
 * instance variables tnxNorm, tnyNorm and tnzNorm.
 */
void SWCalc::TriangleNormal (double *x, double *y, double *z)
{
    double    x1, y1t, z1, x2, y2, z2,
              px, py, pz;
    double    dist;

    x1 = x[1] - x[0];
    y1t = y[1] - y[0];
    z1 = z[1] - z[0];
    x2 = x[2] - x[0];
    y2 = y[2] - y[0];
    z2 = z[2] - z[0];

    px = y1t * z2 - z1 * y2;
    py = z1 * x2 - x1 * z2;
    pz = x1 * y2 - y1t * x2;

    dist = px * px + py * py + pz * pz;
    dist = sqrt (dist);

    if (dist <= 1.e-30) {
        tnxNorm = 0.0;
        tnyNorm = 0.0;
        tnzNorm = 1.0;
    }
    else {
        tnxNorm = px / dist;
        tnyNorm = py / dist;
        tnzNorm = pz / dist;
    }

    return;

}


/*
 *****************************************************************************

                      T r i a n g l e P o i n t s

 *****************************************************************************

  Return the x, y, z points for the specified triangle.

*/

void SWCalc::TrianglePoints (TRiangleStruct *tptr,
                            NOdeStruct *nodes,
                            EDgeStruct *edges,
                            double *x,
                            double *y,
                            double *z)

{
    EDgeStruct         *eptr;
    NOdeStruct         *nptr;
    int                n1, n2, n3;

    eptr = edges + tptr->edge1;
    n1 = eptr->node1;
    nptr = nodes + n1;
    x[0] = nptr->x;
    y[0] = nptr->y;
    z[0] = nptr->z;
    n2 = eptr->node2;
    nptr = nodes + n2;
    x[1] = nptr->x;
    y[1] = nptr->y;
    z[1] = nptr->z;

    eptr = edges + tptr->edge2;
    if (eptr->node1 == n1  ||  eptr->node1 == n2) {
        n3 = eptr->node2;
    }
    else {
        n3 = eptr->node1;
    }

    nptr = nodes + n3;
    x[2] = nptr->x;
    y[2] = nptr->y;
    z[2] = nptr->z;

    return;

}  /* end of private TrianglePoints function */




/*
 * Calculate the outline of the specified trimesh and return it
 * to the java code.
 */
int SWCalc::sw_CalcTriMeshOutline (
    double     *xnode,
    double     *ynode,
    double     *znode,
    int        num_node,
    int        *n1edge,
    int        *n2edge,
    int        *t1edge,
    int        *t2edge,
    int        num_edge,
    int        *e1tri,
    int        *e2tri,
    int        *e3tri,
    int        num_tri
)
{
    int        i;
    NOdeStruct     *nodes;
    EDgeStruct     *edges;
    TRiangleStruct *triangles;

    double         *xout, *yout, *zout;
    int            *nodeout;
    int            *ncout, *nvout, npout, maxpts, maxcomp;
    int            istat;

/*
 * Allocate space for a trimesh representation (nodes, edges, triangles)
 */
    nodes = (NOdeStruct *)csw_Calloc
                 (num_node * sizeof(NOdeStruct));
    if (nodes == NULL) {
        return -1;
    }
    edges = (EDgeStruct *)csw_Calloc
                 (num_edge * sizeof(EDgeStruct));
    if (edges == NULL) {
        csw_Free (nodes);
        return -1;
    }
    triangles = (TRiangleStruct *)csw_Calloc
                     (num_tri * sizeof(TRiangleStruct));
    if (triangles == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        return -1;
    }

/*
 * Fill in the trimesh from the input arrays.
 */
    for (i=0; i<num_node; i++) {
        nodes[i].x = xnode[i];
        nodes[i].y = ynode[i];
        nodes[i].z = znode[i];
    }

    for (i=0; i<num_edge; i++) {
        edges[i].node1 = n1edge[i];
        edges[i].node2 = n2edge[i];
        edges[i].tri1 = t1edge[i];
        edges[i].tri2 = t2edge[i];
    }

    for (i=0; i<num_tri; i++) {
        triangles[i].edge1 = e1tri[i];
        triangles[i].edge2 = e2tri[i];
        triangles[i].edge3 = e3tri[i];
    }

/*
 * Allocate space for the output outline points.
 */
    maxpts = num_node + 1;
    maxcomp = 100;

    xout = (double *)csw_Malloc (maxpts * 4 * sizeof(double));
    if (xout == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
        return -1;
    }
    yout = xout + maxpts;
    zout = yout + maxpts;
    nodeout = (int *)(zout + maxpts);

    ncout = (int *)csw_Malloc (maxcomp * 2 * sizeof(int));
    if (ncout == NULL) {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
        csw_Free (xout);
        return -1;
    }
    nvout = ncout + maxcomp;

/*
 * Calculate the outline.
 */
    istat =
    grdapi_ptr->grd_OutlineTriMeshBoundary (
        nodes, num_node,
        edges, num_edge,
        triangles, num_tri,
        xout, yout, zout,
        nodeout,
        &npout, ncout, nvout,
        maxpts, maxcomp);

    csw_Free (nodes);
    nodes = NULL;
    csw_Free (edges);
    edges = NULL;
    csw_Free (triangles);
    triangles = NULL;

    if (istat == -1  ||  npout != 1  ||  ncout[0] != 1) {
        csw_Free (xout);
        csw_Free (ncout);
        return -1;
    }

    jni_call_set_outline3d_method (
        v_jenv,
        v_jobj,
        xout, yout, zout, nvout[0]);

    csw_Free (xout);
    csw_Free (ncout);

    return 1;

}

/*-----------------------------------------------------------------*/

/*
 * Calculate a tri mesh that exactly honors the specified lines
 * and return its nodes, edges and triangles.  The trimesh is clipped
 * to the boundary if the boundary is defined.  If no boundary is
 * defined, a boundary is calculated according to the bad_bounds_flag.
 *
 *  Input:
 *
 *    xpts         An array of the x coordinates of the points.
 *    ypts         An array of the y coordinates of the points.
 *    zpts         An array of the z coordinates of the points.
 *    npts         The number of points.
 *    xline        Optional array of constraint line x coordinates
 *    yline        Optional array of constraint line y coordinates
 *    zline        Optional array of constraint line z coordinates
 *    npline       Optional array specifying the number of points
 *                 in each separate constraint line.
 *    linetypes    Optional array specifying a type for each separate line.
 *    nline        Number of separate constraint lines.
 *    xbounds_in   Optional array of closed border x coordinates.
 *    ybounds_in   Optional array of closed border y coordinates.
 *    zbounds_in   Optional array of closed border z coordinates.
 *    nbounds_in   Number of points in the closed border.
 *    gflag        Currently ignored.
 *    grid_geometry   Grid geometry object or NULL to use default.
 *    grid_options    Grid options object or NULL to use default.
 *    fault_surface_flag
 *                 Set this to 1 if the points are on a fault surface or
 *                 to zero if on another type of surface.
 *    bad_bounds_flag
 *                 Flag telling what to do if the bounds arrays are NULL
 *                 1 = Use the bounding box of the points.
 *                 2 = Use the convex hull of the points.
 *                 3 = Use the constraint lines for an exact boundary.
 *
 */
int SWCalc::sw_CalcExactTriMesh (
    double *xpts,
    double *ypts,
    double *zpts,
    int    npts,
    double *xline,
    double *yline,
    double *zline,
    int    *npline,
    int    *linetypes,
    int    nline,
    double *xbounds_in,
    double *ybounds_in,
    double *zbounds_in,
    int    nbounds_in,
    int    gflag,
    SWGridGeometryStruct  *grid_geometry,
    SWGridOptionsStruct   *grid_options,
    int    fault_surface_flag,
    int    bad_bounds_flag
    )
{
    int              istat;
    double           gxmin, gymin, gxmax, gymax;
    double           dx, dy, xyspace;
    int              ncol, nrow;
    CSW_F            *gdata;
    FAultLineStruct  *flist;
    int              nflist;

    GRidCalcOptions  options;

    NOdeStruct       *nodes;
    EDgeStruct       *edges;
    TRiangleStruct   *triangles;
    int              num_nodes;
    int              num_edges;
    int              num_triangles;

    int              i, j, n, ntot, ntotvert;
    double           *xline2,
                     *yline2,
                     *zline2;
    double           avlen;
    int              *npline2,
                     *linetypes2,
                     nline2;
    int              *line_exact_flags;

    double           *xbounds, *ybounds, *zbounds;
    int              nbounds;

    double           *xpg, *ypg, *zpg;
    int              npg;
    int              vert_flag;
    int              convex_hull_flag;
    int              vused;
    double           vbase[6];
    double           tiny;

    double           avspace, *xsav, *ysav, *zsav;

/*
 *  !!!! Uncomment this when you need to write specific debugging files.
 */
    char             fcname[100];

/*
 * Extract the exact flags from the linetypes array.
 */
    if (linetypes == NULL) {
        assert (0);
    }

    line_exact_flags = (int *)csw_Calloc (nline * sizeof(int));
    if (line_exact_flags == NULL) {
        return -1;
    }
    for (i=0; i<nline; i++) {
        if (linetypes[i] >= 10000) {
            line_exact_flags[i] = 1;
            linetypes[i] -= 10000;
        }
    }

    FaultSurfaceFlag = fault_surface_flag;

/*
 * The constraints do not need to be converted to skinny polygons.
 */
    grdapi_ptr->grd_SetPolyConstraintFlag (0);

    if (bad_bounds_flag < 1  ||  bad_bounds_flag > 3) {
        bad_bounds_flag = 1;
    }

/*
 * Calculate an unconstrained trimesh using
 * an intermediate grid.
 */
    if (gflag != 0) {

        xpg = NULL;
        ypg = NULL;
        zpg = NULL;
        ntot = 0;
        if (npline != NULL) {
            for (i=0; i<nline; i++) {
                ntot += npline[i];
            }
        }

        xpg = (double *)csw_Malloc ((npts + ntot) * 3 * sizeof(double));
        if (xpg == NULL) {
            csw_Free (line_exact_flags);
            return -1;
        }
        ypg = xpg + npts + ntot;
        zpg = ypg + npts + ntot;

        n = 0;
        for (i=0; i<npts; i++) {
            if (zpts[i] < 1.e20) {
                xpg[n] = xpts[i];
                ypg[n] = ypts[i];
                zpg[n] = zpts[i];
                n++;
            }
        }

        if (zline != NULL) {
            for (i=0; i<ntot; i++) {
                if (zline[i] < 1.e20) {
                    xpg[n] = xline[i];
                    ypg[n] = yline[i];
                    zpg[n] = zline[i];
                    n++;
                }
            }
        }

        npg = n;

        vert_flag = vert_GetFlag (gvert);

        vert_ConvertPoints (xpg, ypg, zpg, npg, gvert);
        vert_ConvertPoints (xline, yline, zline, ntot, gvert);
        ntotvert = ntot;
        vert_ConvertPoints (xbounds_in, ybounds_in, zbounds_in, nbounds_in, gvert);

    /*
     * Get or calculate the grid geometry.
     */
        gxmin = 1.e30;
        gymin = 1.e30;
        gxmax = -1.e30;
        gymax = -1.e30;

        if (grid_geometry != NULL  &&  vert_flag == 0) {
            ncol = grid_geometry->ncol;
            nrow = grid_geometry->nrow;
            gxmin = grid_geometry->xmin;
            gymin = grid_geometry->ymin;
            gxmax = grid_geometry->xmax;
            gymax = grid_geometry->ymax;
        }
        else {
            istat = grdapi_ptr->grd_RecommendedSizeFromDouble
                (xpg, ypg, npg, 0,
                 &gxmin, &gymin, &gxmax, &gymax,
                 &ncol, &nrow);
            if (istat == -1) {
                csw_Free (xpg);
                csw_Free (line_exact_flags);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }

            tiny = (gxmax - gxmin + gymax - gymin) / 10.0;
            gxmin -= tiny;
            gymin -= tiny;
            gxmax += tiny;
            gymax += tiny;
            avlen =
            grdapi_ptr->grd_CalcAverageConstraintSegmentLength (
                xline, yline, npline, nline);
            if (avlen > 1.e20) {
                ncol /= 2;
                nrow /= 2;
                if (fault_surface_flag == 1) {
                    ncol /= 2;
                    nrow /= 2;
                }
            }
            else {
                ncol = (int)((gxmax - gxmin) / avlen + .5);
                nrow = (int)((gymax - gymin) / avlen + .5);
            }
        }

        if (grid_geometry != NULL) {
            ncol = grid_geometry->ncol;
            nrow = grid_geometry->nrow;
        }

        while (ncol * nrow > MAX_DEFAULT_NODES) {
            ncol *= 4;
            nrow *= 4;
            ncol /= 5;
            nrow /= 5;
        }

        if (ncol < 2  ||  nrow < 2  ||
            gxmin >= gxmax  ||  gymin >= gymax) {
            istat = grdapi_ptr->grd_RecommendedSizeFromDouble
                (xpg, ypg, npg, 0,
                 &gxmin, &gymin, &gxmax, &gymax,
                 &ncol, &nrow);
            if (istat == -1) {
                csw_Free (xpg);
                csw_Free (line_exact_flags);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
            if (fault_surface_flag) {
                ncol /= 4;
                nrow /= 4;
            }
            else {
                ncol /= 2;
                nrow /= 2;
            }
            while (ncol * nrow > MAX_DEFAULT_NODES) {
                ncol *= 4;
                nrow *= 4;
                ncol /= 5;
                nrow /= 5;
            }
        }

        dx = (gxmax - gxmin) / (ncol - 1);
        dy = (gymax - gymin) / (nrow - 1);
        xyspace = (dx + dy) / 2.0;

    /*
     * If the boundary is not specified, use the bad bounds flag action
     * to get the boundary.
     */
        xbounds = NULL;
        ybounds = NULL;
        zbounds = NULL;
        nbounds = 0;
        convex_hull_flag = 0;

        if (xbounds_in != NULL  &&
            ybounds_in != NULL  &&
            zbounds_in != NULL  &&
            nbounds_in > 3) {
            xbounds = xbounds_in;
            ybounds = ybounds_in;
            zbounds = zbounds_in;
            nbounds = nbounds_in;

            if (xbounds[0] > 1.e20) {
                if (bad_bounds_flag == 1  ||  bad_bounds_flag == 3) {
                    xbounds = NULL;
                    ybounds = NULL;
                    zbounds = NULL;
                    nbounds = 0;
                }
                else {
                    zbounds = NULL;
                    istat =
                    grdapi_ptr->grd_CalcConvexHull (
                        xyspace,
                        xpg, ypg, npg,
                        &xbounds, &ybounds, &nbounds);
                    if (istat <= 0) {
                        xbounds = NULL;
                        ybounds = NULL;
                        zbounds = NULL;
                        nbounds = 0;
                    }
                    else {
                        convex_hull_flag = 1;
                    }
                }
            }
        }
        else {
            if (bad_bounds_flag == 1  ||  bad_bounds_flag == 3) {
                xbounds = NULL;
                ybounds = NULL;
                zbounds = NULL;
                nbounds = 0;
            }
            else {
                zbounds = NULL;
                istat =
                grdapi_ptr->grd_CalcConvexHull (
                    xyspace,
                    xpg, ypg, npg,
                    &xbounds, &ybounds, &nbounds);
                if (istat <= 0) {
                    xbounds = NULL;
                    ybounds = NULL;
                    zbounds = NULL;
                    nbounds = 0;
                }
                else {
                    convex_hull_flag = 1;
                }
            }
        }

    /*
     * Allocate space for the grid.
     */
        gdata = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (gdata == NULL) {
            csw_Free (xpg);
            csw_Free (line_exact_flags);
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * The grid is not faulted.
     */
        flist = NULL;
        nflist = 0;

    /*
     * Transfer grid options.
     */
        ConvertGridCalcOptions (grid_options,
                                &options);
        options.triangulate_flag = 1;
        if (FaultSurfaceFlag != 0) {
            options.triangulate_flag = 3;
            options.anisotropy_flag = 0;
        }

    /*
     * Calculate the grid.
     */
        istat = grdapi_ptr->grd_CalcGridFromDouble
            (xpg, ypg, zpg, NULL, npg,
             gdata, NULL, NULL,
             ncol, nrow,
             gxmin, gymin, gxmax, gymax,
             flist, nflist,
             &options);
        csw_Free (xpg);
        xpg = NULL;
        ypg = NULL;
        zpg = NULL;
        grdapi_ptr->grd_FreeFaultLineStructs (flist, nflist);
        flist = NULL;
        nflist = 0;
        if (istat == -1) {
            csw_Free (gdata);
            csw_Free (line_exact_flags);
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * Make a temporary set of constraint lines that includes
     * the lines passed to this function and the boundaries
     * passed to this function.
     */
        xline2 = NULL;
        yline2 = NULL;
        zline2 = NULL;
        npline2 = NULL;
        linetypes2 = NULL;
        nline2 = 0;
        if (nline > 0  ||  nbounds > 0) {
            ntot = 0;
            if (nline > 0  &&  npline != NULL) {
                for (i=0; i<nline; i++) {
                    ntot += npline[i];
                }
            }
            ntot += nbounds;
            nline2 = nline;
            xline2 = (double *)csw_Malloc (ntot * sizeof(double));
            yline2 = (double *)csw_Malloc (ntot * sizeof(double));
            zline2 = (double *)csw_Malloc (ntot * sizeof(double));
            npline2 = (int *)csw_Malloc ((nline2 + 1) * sizeof(int));
            linetypes2 = (int *)csw_Calloc ((nline2 + 1) * sizeof(int));
            if (xline2 == NULL  ||
                yline2 == NULL  ||
                zline2 == NULL  ||
                npline2 == NULL  ||
                linetypes2 == NULL) {
                csw_Free (xline2);
                csw_Free (yline2);
                csw_Free (zline2);
                csw_Free (npline2);
                csw_Free (linetypes2);
                csw_Free (line_exact_flags);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }

            n = 0;
            if (nline > 0  &&  npline != NULL) {
                ntot -= nbounds;
                memcpy (xline2, xline, ntot * sizeof(double));
                memcpy (yline2, yline, ntot * sizeof(double));
                if (zline != NULL) {
                    memcpy (zline2, zline, ntot * sizeof(double));
                }
                else {
                    for (j=0; j<ntot; j++) {
                        zline2[j] = 1.e30;
                    }
                }
                n += ntot;
                memcpy (npline2, npline, nline * sizeof(int));
                if (linetypes != NULL) {
                    memcpy (linetypes2, linetypes, nline * sizeof(int));
                }
            }
            else {
                ntot = 0;
            }

            if (nbounds > 0  &&  xbounds != NULL  &&  ybounds != NULL) {
                memcpy (xline2+n, xbounds, nbounds * sizeof(double));
                memcpy (yline2+n, ybounds, nbounds * sizeof(double));
                if (zbounds != NULL) {
                    memcpy (zline2+n, zbounds, nbounds * sizeof(double));
                }
                else {
                    for (j=0; j<nbounds; j++) {
                        zline2[n+j] = 1.e30;
                    }
                }
                nline2 = nline;
                npline2[nline2] = nbounds;
                linetypes2[nline2] = 0;
                nline2++;

            }


        /*
         * If any constraint points are undefined in z, attempt
         * to interpolate them from the grid.
         */
            for (i=0; i<ntot+nbounds; i++) {
                if (zline2[i] > 1.e20  ||  zline2[i] < -1.e20) {
                    grdapi_ptr->grd_BackInterpolateFromDouble (
                        gdata, ncol, nrow,
                        gxmin, gymin, gxmax, gymax,
                        NULL, 0,
                        xline2+i, yline2+i, zline2+i, 1,
                        GRD_BILINEAR);
                }
            }
        }

    /*
     * Resample the constraint lines at aproximately the grid spacing.
     */
        dx = (gxmax - gxmin) / (ncol - 1);
        dy = (gymax - gymin) / (nrow - 1);
        avspace = (dx + dy) / 2.0;
        xsav = xline2;
        ysav = yline2;
        zsav = zline2;

        istat =
        ResampleConstraintLines (
            &xline2, &yline2, &zline2,
            npline2, line_exact_flags, nline2, avspace);
        csw_Free (xsav);
        csw_Free (ysav);
        csw_Free (zsav);
        xsav = ysav = zsav = NULL;
        csw_Free (line_exact_flags);
        line_exact_flags = NULL;




        sprintf (fcname, "constraint_%d.xyz", FileID);
        WriteConstraintLines (fcname,
                              xline2, yline2, zline2,
                              npline2, nline2);
        FileID++;

    /*
     * Calculate the trimesh from the grid.  The constraints will not
     * be embeddi=ed in the trimesh, but they are needed to interpolate
     * the z values at the equlateral points.
     */
        nodes = NULL;
        edges = NULL;
        triangles = NULL;
        num_nodes = 0;
        num_edges = 0;
        num_triangles = 0;
        istat = grdapi_ptr->grd_CalcTriMeshFromGrid
            (gdata, ncol, nrow,
             gxmin, gymin, gxmax, gymax,
             xline2, yline2, zline2,
             npline2, linetypes2, nline2,
             GRD_EQUILATERAL,
             &nodes, &edges, &triangles,
             &num_nodes, &num_edges, &num_triangles);
        csw_Free (gdata);
        gdata = NULL;
        if (istat == -1) {
            csw_Free (xline2);
            csw_Free (yline2);
            csw_Free (zline2);
            csw_Free (npline2);
            csw_Free (linetypes2);
            nline2 = 0;
            xline2 = NULL;
            yline2 = NULL;
            zline2 = NULL;
            npline2 = NULL;
            linetypes2 = NULL;
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * Add the constraints to the previously calculated trimesh.
     */
        istat = grdapi_ptr->grd_AddLinesToTriMesh (
            xline2, yline2, zline2,
            npline2, linetypes2, nline2,
            1,
            &nodes, &edges, &triangles,
            &num_nodes, &num_edges, &num_triangles);

    /*
     * Free the temporary constraints.
     */
        csw_Free (xline2);
        csw_Free (yline2);
        csw_Free (zline2);
        csw_Free (npline2);
        csw_Free (linetypes2);
        nline2 = 0;
        xline2 = NULL;
        yline2 = NULL;
        zline2 = NULL;
        npline2 = NULL;
        linetypes2 = NULL;

        if (istat == -1) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * Clip the trimesh to the boundary if needed.
     */
        if (nbounds > 0) {
            istat = grdapi_ptr->grd_ClipTriMeshToPolygon
                (&nodes, &num_nodes,
                 &edges, &num_edges,
                 &triangles, &num_triangles,
                 xbounds, ybounds, &nbounds, 1, 1);
            if (istat == -1) {
                csw_Free (nodes);
                csw_Free (edges);
                csw_Free (triangles);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
        }

        vert_GetBaseline (
            vbase,
            vbase+1,
            vbase+2,
            vbase+3,
            vbase+4,
            vbase+5,
            &vused, gvert);

    /*
     * !!!! Used for specific debugging only.  If needed,
     * uncomment this block of code.
        strcpy (fname, "clipsteep.tri");
        grdapi_ptr->grd_WriteTextTriMeshFile (
            vused, vbase,
            triangles, num_triangles,
            edges, num_edges,
            nodes, num_nodes,
            fname);
     */

        if (convex_hull_flag == 1) {
            csw_Free (xbounds);
            csw_Free (ybounds);
            xbounds = NULL;
            ybounds = NULL;
        }

    /*
     * Convert the trimesh nodes from steep surface coords to xyz if needed.
     */
        vert_UnconvertTriMeshNodes (nodes, num_nodes, gvert);

    /*
     * Convert the input line and bounds data back if needed.
     */
        vert_UnconvertPoints (xline, yline, zline, ntot, gvert);
        vert_UnconvertPoints (xbounds, ybounds, zbounds, nbounds, gvert);

        vert_SendBackBaseline (v_jenv, v_jobj, gvert);

    /*
     * Convert the trimesh to arrays and send them back to the java side.
     */
        istat =
        SendBackTriMesh (nodes, num_nodes,
                         edges, num_edges,
                         triangles, num_triangles);

        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
        nodes = NULL;
        edges = NULL;
        triangles = NULL;
        num_nodes = 0;
        num_edges = 0;
        num_triangles = 0;

        if (istat == -1) {
            return -1;
        }

    }  /* end of trimesh from grid block */


    csw_Free (line_exact_flags);

    return 1;
}


/*--------------------------------------------------------------------------*/

/*
 * Calculate a tri mesh that exactly honors the specified lines
 * and return its nodes, edges and triangles.  The trimesh is clipped
 * to the boundary if the boundary is defined.  If no boundary is
 * defined, a boundary is calculated according to the bad_bounds_flag.
 *
 *  Input:
 *
 *    xpts         An array of the x coordinates of the points.
 *    ypts         An array of the y coordinates of the points.
 *    zpts         An array of the z coordinates of the points.
 *    npts         The number of points.
 *    xline        Optional array of constraint line x coordinates
 *    yline        Optional array of constraint line y coordinates
 *    zline        Optional array of constraint line z coordinates
 *    npline       Optional array specifying the number of points
 *                 in each separate constraint line.
 *    linetypes    Optional array specifying a type for each separate line.
 *    nline        Number of separate constraint lines.
 *    xbounds_in   Optional array of closed border x coordinates.
 *    ybounds_in   Optional array of closed border y coordinates.
 *    zbounds_in   Optional array of closed border z coordinates.
 *    nbounds_in   Number of points in the closed border.
 *    gflag        Currently ignored.
 *    grid_geometry   Grid geometry object or NULL to use default.
 *    grid_options    Grid options object or NULL to use default.
 *    fault_surface_flag
 *                 Set this to 1 if the points are on a fault surface or
 *                 to zero if on another type of surface.
 *    bad_bounds_flag
 *                 Flag telling what to do if the bounds arrays are NULL
 *                 1 = Use the bounding box of the points.
 *                 2 = Use the convex hull of the points.
 *                 3 = Use the constraint lines for an exact boundary.
 *
 */
int SWCalc::sw_CalcExactTriMeshLocally (
    double *xpts,
    double *ypts,
    double *zpts,
    int    npts,
    double *xline,
    double *yline,
    double *zline,
    int    *npline,
    int    *linetypes,
    int    nline,
    double *xbounds_in,
    double *ybounds_in,
    double *zbounds_in,
    int    nbounds_in,
    int    gflag,
    SWGridGeometryStruct  *grid_geometry,
    SWGridOptionsStruct   *grid_options,
    int    fault_surface_flag,
    int    bad_bounds_flag,
    NOdeStruct    **nodes_out,
    int           *num_nodes_out,
    EDgeStruct    **edges_out,
    int           *num_edges_out,
    TRiangleStruct  **triangles_out,
    int           *num_triangles_out
    )
{
    int              istat;
    double           gxmin, gymin, gxmax, gymax;
    double           dx, dy, xyspace;
    int              ncol, nrow;
    CSW_F            *gdata;
    FAultLineStruct  *flist;
    int              nflist;

    GRidCalcOptions  options;

    NOdeStruct       *nodes;
    EDgeStruct       *edges;
    TRiangleStruct   *triangles;
    int              num_nodes;
    int              num_edges;
    int              num_triangles;

    int              i, j, n, ntot, ntotvert;
    double           *xline2,
                     *yline2,
                     *zline2;
    double           avlen;
    int              *npline2,
                     *linetypes2,
                     nline2;

    double           *xbounds, *ybounds, *zbounds;
    int              nbounds;

    double           *xpg, *ypg, *zpg;
    int              npg;
    int              vert_flag;
    int              convex_hull_flag;
    int              vused;
    double           vbase[6];

/*
 *  !!!! Uncomment this when you need to write specific debugging files.
 */
    char             fname[100];
    int              do_write;

    do_write = csw_GetDoWrite ();
    if (do_write != 0) {
        strcpy (fname, "local_exact.xyz");
        grdapi_ptr->grd_WriteLines (
            xline, yline, zline, 1,
            &nline, npline,
            fname);
    }



    FaultSurfaceFlag = fault_surface_flag;

/*
 * The constraints do not need to be converted to skinny polygons.
 */
    grdapi_ptr->grd_SetPolyConstraintFlag (0);

    if (bad_bounds_flag < 1  ||  bad_bounds_flag > 3) {
        bad_bounds_flag = 1;
    }

/*
 * Calculate an unconstrained trimesh using
 * an intermediate grid.
 */
    if (gflag != 0) {

        xpg = NULL;
        ypg = NULL;
        zpg = NULL;
        ntot = 0;
        if (npline != NULL) {
            for (i=0; i<nline; i++) {
                ntot += npline[i];
            }
        }

        xpg = (double *)csw_Malloc ((npts + ntot) * 3 * sizeof(double));
        if (xpg == NULL) {
            return -1;
        }
        ypg = xpg + npts + ntot;
        zpg = ypg + npts + ntot;

        n = 0;
        for (i=0; i<npts; i++) {
            if (zpts[i] < 1.e20) {
                xpg[n] = xpts[i];
                ypg[n] = ypts[i];
                zpg[n] = zpts[i];
                n++;
            }
        }

        if (zline != NULL) {
            for (i=0; i<ntot; i++) {
                if (zline[i] < 1.e20) {
                    xpg[n] = xline[i];
                    ypg[n] = yline[i];
                    zpg[n] = zline[i];
                    n++;
                }
            }
        }

        npg = n;

        vert_flag = vert_GetFlag (gvert);

        vert_ConvertPoints (xpg, ypg, zpg, npg, gvert);
        vert_ConvertPoints (xline, yline, zline, ntot, gvert);
        ntotvert = ntot;
        vert_ConvertPoints (xbounds_in, ybounds_in, zbounds_in, nbounds_in, gvert);

    /*
     * Get or calculate the grid geometry.
     */
        gxmin = 1.e30;
        gymin = 1.e30;
        gxmax = -1.e30;
        gymax = -1.e30;

        if (grid_geometry != NULL  &&  vert_flag == 0) {
            ncol = grid_geometry->ncol;
            nrow = grid_geometry->nrow;
            gxmin = grid_geometry->xmin;
            gymin = grid_geometry->ymin;
            gxmax = grid_geometry->xmax;
            gymax = grid_geometry->ymax;
        }
        else {
            istat = grdapi_ptr->grd_RecommendedSizeFromDouble
                (xpg, ypg, npg, 0,
                 &gxmin, &gymin, &gxmax, &gymax,
                 &ncol, &nrow);
            if (istat == -1) {
                csw_Free (xpg);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
            avlen =
            grdapi_ptr->grd_CalcAverageConstraintSegmentLength (
                xline, yline, npline, nline);
            if (avlen > 1.e20) {
                ncol /= 2;
                nrow /= 2;
                if (fault_surface_flag == 1) {
                    ncol /= 2;
                    nrow /= 2;
                }
            }
            else {
                ncol = (int)((gxmax - gxmin) / avlen + .5);
                nrow = (int)((gymax - gymin) / avlen + .5);
            }
        }

        if (grid_geometry != NULL) {
            ncol = grid_geometry->ncol;
            nrow = grid_geometry->nrow;
        }

        while (ncol * nrow > MAX_DEFAULT_NODES) {
            ncol *= 4;
            nrow *= 4;
            ncol /= 5;
            nrow /= 5;
        }

        if (ncol < 2  ||  nrow < 2  ||
            gxmin >= gxmax  ||  gymin >= gymax) {
            istat = grdapi_ptr->grd_RecommendedSizeFromDouble
                (xpg, ypg, npg, 0,
                 &gxmin, &gymin, &gxmax, &gymax,
                 &ncol, &nrow);
            if (istat == -1) {
                csw_Free (xpg);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
            if (fault_surface_flag) {
                ncol /= 4;
                nrow /= 4;
            }
            else {
                ncol /= 2;
                nrow /= 2;
            }
            while (ncol * nrow > MAX_DEFAULT_NODES) {
                ncol *= 4;
                nrow *= 4;
                ncol /= 5;
                nrow /= 5;
            }
        }

        dx = (gxmax - gxmin) / (ncol - 1);
        dy = (gymax - gymin) / (nrow - 1);
        xyspace = (dx + dy) / 2.0;

    /*
     * If the boundary is not specified, use the bad bounds flag action
     * to get the boundary.
     */
        xbounds = NULL;
        ybounds = NULL;
        zbounds = NULL;
        nbounds = 0;
        convex_hull_flag = 0;

        if (xbounds_in != NULL  &&
            ybounds_in != NULL  &&
            zbounds_in != NULL  &&
            nbounds_in > 3) {
            xbounds = xbounds_in;
            ybounds = ybounds_in;
            zbounds = zbounds_in;
            nbounds = nbounds_in;

            if (xbounds[0] > 1.e20) {
                if (bad_bounds_flag == 1  ||  bad_bounds_flag == 3) {
                    xbounds = NULL;
                    ybounds = NULL;
                    zbounds = NULL;
                    nbounds = 0;
                }
                else {
                    zbounds = NULL;
                    istat =
                    grdapi_ptr->grd_CalcConvexHull (
                        xyspace,
                        xpg, ypg, npg,
                        &xbounds, &ybounds, &nbounds);
                    if (istat <= 0) {
                        xbounds = NULL;
                        ybounds = NULL;
                        zbounds = NULL;
                        nbounds = 0;
                    }
                    else {
                        convex_hull_flag = 1;
                    }
                }
            }
        }
        else {
            if (bad_bounds_flag == 1  ||  bad_bounds_flag == 3) {
                xbounds = NULL;
                ybounds = NULL;
                zbounds = NULL;
                nbounds = 0;
            }
            else {
                zbounds = NULL;
                istat =
                grdapi_ptr->grd_CalcConvexHull (
                    xyspace,
                    xpg, ypg, npg,
                    &xbounds, &ybounds, &nbounds);
                if (istat <= 0) {
                    xbounds = NULL;
                    ybounds = NULL;
                    zbounds = NULL;
                    nbounds = 0;
                }
                else {
                    convex_hull_flag = 1;
                }
            }
        }

    /*
     * Allocate space for the grid.
     */
        gdata = (CSW_F *)csw_Malloc (ncol * nrow * sizeof(CSW_F));
        if (gdata == NULL) {
            csw_Free (xpg);
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * The grid is not faulted.
     */
        flist = NULL;
        nflist = 0;

    /*
     * Transfer grid options.
     */
        ConvertGridCalcOptions (grid_options,
                                &options);
        options.triangulate_flag = 1;
        if (FaultSurfaceFlag != 0) {
            options.triangulate_flag = 3;
            options.anisotropy_flag = 0;
        }

    /*
     * Calculate the grid.
     */
        istat = grdapi_ptr->grd_CalcGridFromDouble
            (xpg, ypg, zpg, NULL, npg,
             gdata, NULL, NULL,
             ncol, nrow,
             gxmin, gymin, gxmax, gymax,
             flist, nflist,
             &options);
        csw_Free (xpg);
        xpg = NULL;
        ypg = NULL;
        zpg = NULL;
        grdapi_ptr->grd_FreeFaultLineStructs (flist, nflist);
        flist = NULL;
        nflist = 0;
        if (istat == -1) {
            csw_Free (gdata);
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * Make a temporary set of constraint lines that includes
     * the lines passed to this function and the boundaries
     * passed to this function.
     */
        xline2 = NULL;
        yline2 = NULL;
        zline2 = NULL;
        npline2 = NULL;
        linetypes2 = NULL;
        nline2 = 0;
        if (nline > 0  ||  nbounds > 0) {
            ntot = 0;
            if (nline > 0  &&  npline != NULL) {
                for (i=0; i<nline; i++) {
                    ntot += npline[i];
                }
            }
            ntot += nbounds;
            nline2 = nline;
            xline2 = (double *)csw_Malloc (ntot * sizeof(double));
            yline2 = (double *)csw_Malloc (ntot * sizeof(double));
            zline2 = (double *)csw_Malloc (ntot * sizeof(double));
            npline2 = (int *)csw_Malloc ((nline2 + 1) * sizeof(int));
            linetypes2 = (int *)csw_Calloc ((nline2 + 1) * sizeof(int));
            if (xline2 == NULL  ||
                yline2 == NULL  ||
                zline2 == NULL  ||
                npline2 == NULL  ||
                linetypes2 == NULL) {
                csw_Free (xline2);
                csw_Free (yline2);
                csw_Free (zline2);
                csw_Free (npline2);
                csw_Free (linetypes2);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }

            n = 0;
            if (nline > 0  &&  npline != NULL) {
                ntot -= nbounds;
                memcpy (xline2, xline, ntot * sizeof(double));
                memcpy (yline2, yline, ntot * sizeof(double));
                if (zline != NULL) {
                    memcpy (zline2, zline, ntot * sizeof(double));
                }
                else {
                    for (j=0; j<ntot; j++) {
                        zline2[j] = 1.e30;
                    }
                }
                n += ntot;
                memcpy (npline2, npline, nline * sizeof(int));
                if (linetypes != NULL) {
                    memcpy (linetypes2, linetypes, nline * sizeof(int));
                }
            }
            else {
                ntot = 0;
            }

/*
            if (nbounds > 0  &&  xbounds != NULL  &&  ybounds != NULL) {
                memcpy (xline2+n, xbounds, nbounds * sizeof(double));
                memcpy (yline2+n, ybounds, nbounds * sizeof(double));
                if (zbounds != NULL) {
                    memcpy (zline2+n, zbounds, nbounds * sizeof(double));
                }
                else {
                    for (j=0; j<nbounds; j++) {
                        zline2[n+j] = 1.e30;
                    }
                }
                nline2 = nline;
                npline2[nline2] = nbounds;
                linetypes2[nline2] = 0;
                nline2++;

            }
*/

        /*
         * If any constraint points are undefined in z, attempt
         * to interpolate them from the grid.
         */
            for (i=0; i<ntot+nbounds; i++) {
                if (zline2[i] > 1.e20  ||  zline2[i] < -1.e20) {
                    grdapi_ptr->grd_BackInterpolateFromDouble (
                        gdata, ncol, nrow,
                        gxmin, gymin, gxmax, gymax,
                        NULL, 0,
                        xline2+i, yline2+i, zline2+i, 1,
                        GRD_BILINEAR);
                }
            }
        }




    /*
     * Calculate the trimesh from the grid.  The constraints will not be
     * embedded into the trimesh, but they are needed to interpolate the
     * z values at the equilateral points.
     */
        nodes = NULL;
        edges = NULL;
        triangles = NULL;
        num_nodes = 0;
        num_edges = 0;
        num_triangles = 0;
        istat = grdapi_ptr->grd_CalcTriMeshFromGrid
            (gdata, ncol, nrow,
             gxmin, gymin, gxmax, gymax,
             xline2, yline2, zline2,
             npline2, linetypes2, nline2,
             GRD_EQUILATERAL,
             &nodes, &edges, &triangles,
             &num_nodes, &num_edges, &num_triangles);
        csw_Free (gdata);
        gdata = NULL;
        if (istat == -1) {
            csw_Free (xline2);
            csw_Free (yline2);
            csw_Free (zline2);
            csw_Free (npline2);
            csw_Free (linetypes2);
            nline2 = 0;
            xline2 = NULL;
            yline2 = NULL;
            zline2 = NULL;
            npline2 = NULL;
            linetypes2 = NULL;
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }

    /*
     * !!!! Used for specific debugging only.  If needed,
     * uncomment this block of code.
     */
        if (do_write) {
            strcpy (fname, "noconstraint.tri");
            vused = 0;
            vbase[0] = 1.e30;
            vbase[1] = 1.e30;
            vbase[2] = 1.e30;
            vbase[3] = 1.e30;
            vbase[4] = 1.e30;
            vbase[5] = 1.e30;
            grdapi_ptr->grd_WriteTextTriMeshFile (
                vused, vbase,
                triangles, num_triangles,
                edges, num_edges,
                nodes, num_nodes,
                fname);
        }

    /*
     * Add the constraints to the previously calculated trimesh.
     */
        istat = grdapi_ptr->grd_AddLinesToTriMesh (
            xline2, yline2, zline2,
            npline2, linetypes2, nline2,
            1,
            &nodes, &edges, &triangles,
            &num_nodes, &num_edges, &num_triangles);

    /*
     * Free the temporary constraints.
     */
        csw_Free (xline2);
        csw_Free (yline2);
        csw_Free (zline2);
        csw_Free (npline2);
        csw_Free (linetypes2);
        nline2 = 0;
        xline2 = NULL;
        yline2 = NULL;
        zline2 = NULL;
        npline2 = NULL;
        linetypes2 = NULL;

        if (istat == -1) {
            csw_Free (nodes);
            csw_Free (edges);
            csw_Free (triangles);
            vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
            return -1;
        }



    /*
     * !!!! Used for specific debugging only.  If needed,
     * uncomment this block of code.
     */
        if (do_write) {
            strcpy (fname, "preclipsteep.tri");
            vused = 0;
            vbase[0] = 1.e30;
            vbase[1] = 1.e30;
            vbase[2] = 1.e30;
            vbase[3] = 1.e30;
            vbase[4] = 1.e30;
            vbase[5] = 1.e30;
            grdapi_ptr->grd_WriteTextTriMeshFile (
                vused, vbase,
                triangles, num_triangles,
                edges, num_edges,
                nodes, num_nodes,
                fname);
        }



    /*
     * Clip the trimesh to the boundary if needed.
     */
        if (nbounds > 0) {
            istat = grdapi_ptr->grd_ClipTriMeshToPolygon
                (&nodes, &num_nodes,
                 &edges, &num_edges,
                 &triangles, &num_triangles,
                 xbounds, ybounds, &nbounds, 1, 1);
            if (istat == -1) {
                csw_Free (nodes);
                csw_Free (edges);
                csw_Free (triangles);
                vert_UnconvertPoints (xline, yline, zline, ntotvert, gvert);
                return -1;
            }
        }

        vert_GetBaseline (
            vbase,
            vbase+1,
            vbase+2,
            vbase+3,
            vbase+4,
            vbase+5,
            &vused, gvert);



    /*
     * !!!! Used for specific debugging only.  If needed,
     * uncomment this block of code.
     */
        if (do_write) {
            strcpy (fname, "clipsteep.tri");
            grdapi_ptr->grd_WriteTextTriMeshFile (
                vused, vbase,
                triangles, num_triangles,
                edges, num_edges,
                nodes, num_nodes,
                fname);
        }




        if (convex_hull_flag == 1) {
            csw_Free (xbounds);
            csw_Free (ybounds);
            xbounds = NULL;
            ybounds = NULL;
        }

    /*
     * Convert the trimesh nodes from steep surface coords to xyz if needed.
     */
        vert_UnconvertTriMeshNodes (nodes, num_nodes, gvert);

    /*
     * Convert the input line and bounds data back if needed.
     */
        vert_UnconvertPoints (xline, yline, zline, ntot, gvert);
        vert_UnconvertPoints (xbounds, ybounds, zbounds, nbounds, gvert);

    /*
     * Assign the trimesh data to the output pointers.
     */
        *nodes_out = nodes;
        *edges_out = edges;
        *triangles_out = triangles;
        *num_nodes_out = num_nodes;
        *num_edges_out = num_edges;
        *num_triangles_out = num_triangles;

    }  /* end of trimesh from grid block */

    return 1;
}


/*--------------------------------------------------------------------------------*/


int SWCalc::ResampleConstraintLines (
    double        **xline_io,
    double        **yline_io,
    double        **zline_io,
    int           *npline_io,
    int           *exact_flags,
    int           nline,
    double        avspace)
{
    int           i, j, n, n_new, npts, istat;
    double        *xa, *ya, *za,
                  *xnew, *ynew, *znew;
    double        *xline, *yline, *zline;

    typedef struct {
        double    *x;
        double    *y;
        double    *z;
        int       npts;
    } _templine;

    _templine     *tline;


/*
 * Allocate space for temporary storage of each resampled line.
 */
    tline = (_templine *)csw_Calloc (nline * sizeof(_templine));
    if (tline == NULL) {
        return -1;
    }

    xline = *xline_io;
    yline = *yline_io;
    zline = *zline_io;

/*
 * Rsample each line at the specified interval.  Put the
 * results for each line into the tline array.
 */
    n = 0;
    for (i=0; i<nline; i++) {

        xa = xline + n;
        ya = yline + n;
        za = zline + n;
        npts = npline_io[i];

        if (npts < 2) {
            tline[i].x = NULL;
            tline[i].y = NULL;
            tline[i].z = NULL;
            tline[i].npts = 0;
            continue;
        }

        n += npts;

        if (exact_flags == NULL) {
            istat =
            gpf_resample_line_xy (xa, ya, za, npts,
                               avspace,
                               &xnew, &ynew, &znew, &n_new);
            if (istat == -1) {
                for (j=0; j<nline; j++) {
                    csw_Free (tline[j].x);
                }
                csw_Free (tline);
                return -1;
            }
        }

        else {
            if (exact_flags[i] == 0) {
                istat =
                gpf_resample_line_xy (xa, ya, za, npts,
                                   avspace,
                                   &xnew, &ynew, &znew, &n_new);
                if (istat == -1) {
                    for (j=0; j<nline; j++) {
                        csw_Free (tline[j].x);
                    }
                    csw_Free (tline);
                    return -1;
                }
            }
            else {
                xnew = (double *)csw_Malloc (3 * npts * sizeof(double));
                if (xnew == NULL) {
                    for (j=0; j<nline; j++) {
                        csw_Free (tline[j].x);
                    }
                    csw_Free (tline);
                    return -1;
                }
                ynew = xnew + npts;
                znew = ynew + npts;
                memcpy (xnew, xa, npts * sizeof(double));
                memcpy (ynew, ya, npts * sizeof(double));
                memcpy (znew, za, npts * sizeof(double));
                n_new = npts;
            }
        }

        tline[i].x = xnew;
        tline[i].y = ynew;
        tline[i].z = znew;
        tline[i].npts = n_new;

    }

/*
 * Allocate space for output.
 */
    n = 0;
    for (i=0; i<nline; i++) {
        n += tline[i].npts;
    }

    xline = (double *)csw_Malloc (n * sizeof(double));
    yline = (double *)csw_Malloc (n * sizeof(double));
    zline = (double *)csw_Malloc (n * sizeof(double));
    if (xline == NULL  ||  yline == NULL  ||  zline == NULL) {
        csw_Free (xline);
        csw_Free (yline);
        csw_Free (zline);
        csw_Free (tline);
        return -1;
    }

    n = 0;
    for (i=0; i<nline; i++) {
        if (tline[i].npts == 0) {
            npline_io[i] = 0;
            continue;
        }
        memcpy (xline+n, tline[i].x, tline[i].npts * sizeof(double));
        memcpy (yline+n, tline[i].y, tline[i].npts * sizeof(double));
        memcpy (zline+n, tline[i].z, tline[i].npts * sizeof(double));
        npline_io[i] = tline[i].npts;
        n += tline[i].npts;
    }

    *xline_io = xline;
    *yline_io = yline;
    *zline_io = zline;

    for (j=0; j<nline; j++) {
        csw_Free (tline[j].x);
    }
    csw_Free (tline);

    return 1;

}



/*------------------------------------------------------------------------------*/

/*
 * Calculate the draped points over the current cached trimesh.
 * Return the calculated points back to the java side.
 */
int SWCalc::sw_CalcDrapedPoints (void)
{
    double            *xout, *yout, *zout;
    int               nout;
    int               istat;

    istat = grdapi_ptr->grd_DrapePointsOnTriMesh (
        drapeId1, drapeId2,
        drapeNodes, numDrapeNodes,
        drapeEdges, numDrapeEdges,
        drapeTriangles, numDrapeTriangles,
        drapeXpoint, drapeYpoint, drapeNpoint,
        &xout, &yout, &zout,
        &nout);

    if (istat == -1) {
        return -1;
    }

    vert_UnconvertPoints (xout, yout, zout, nout, gvert);

    jni_call_add_draped_points_method (
        v_jenv,
        v_jobj,
        xout,
        yout,
        zout,
        nout
    );

    csw_Free (xout);
    csw_Free (yout);
    csw_Free (zout);

    return 1;

}


/*-------------------------------------------------------------------------------------*/

/*
 * Calculate a trimesh with a constant value using the specified boundary
 * polygon.
 */

int SWCalc::sw_CalcConstantTriMesh (
    double              *xbounds,
    double              *ybounds,
    int                 nbounds,
    double              zvalue)
{
    CSW_F               *grid;
    int                 ncol, nrow;
    int                 i, istat;
    double              gx1, gy1, gx2, gy2, tiny, aspect;
    TRiangleStruct      *triangles;
    EDgeStruct          *edges;
    NOdeStruct          *nodes;
    int                 num_triangles, num_nodes, num_edges;
    double              *zbounds;
    double              *xresamp, *yresamp, *zresamp;
    double              avspace;
    int                 nresamp;
    int                 do_write;
    double              v6[6];
    char                fname[200];

    gx1 = 1.e30;
    gy1 = 1.e30;
    gx2 = -1.e30;
    gy2 = -1.e30;

    for (i=0; i<nbounds; i++) {
        if (xbounds[i] < gx1) gx1 = xbounds[i];
        if (ybounds[i] < gy1) gy1 = ybounds[i];
        if (xbounds[i] > gx2) gx2 = xbounds[i];
        if (ybounds[i] > gy2) gy2 = ybounds[i];
    }

    if (gx1 >= gx2  ||  gy1 >= gy2) {
        return -1;
    }

    tiny = (gx2 - gx1 + gy2 - gy1) / 20.0;

    gx1 -= tiny;
    gy1 -= tiny;
    gx2 += tiny;
    gy2 += tiny;

    aspect = (gy2 - gy1) / (gx2 - gx1);

/*
 * Allocate space for a temporary grid used to
 * calculate the trimesh.  Set all the grid
 * values to the constant.
 */
    ncol = (int)(sqrt(4000.0 / aspect) + 0.5);
    if (ncol < 10) ncol = 10;
    if (ncol > 400) ncol = 400;
    nrow = 4000 / ncol;
    avspace = (gx2 - gx1) / (ncol - 1);
    avspace += (gy2 - gy1) / (nrow - 1);
    avspace /= 2.0;

    grid = (CSW_F *)csw_Malloc (ncol * nrow * sizeof (CSW_F));
    if (grid == NULL) {
        return -1;
    }

    for (i=0; i<ncol * nrow; i++) {
        grid[i] = (CSW_F)zvalue;
    }

/*
 * Allocate space for z values of the boundary and set all
 * these z values to the constant z value.
 */
    zbounds = (double *)csw_Malloc (nbounds * sizeof(double));
    if (zbounds == NULL) {
        csw_Free (grid);
        return -1;
    }
    for (i=0; i<nbounds; i++) {
        zbounds[i] = zvalue;
    }

/*
 * Calculate the trimesh from the grid.
 */
    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    num_nodes = 0;
    num_edges = 0;
    num_triangles = 0;
    istat = grdapi_ptr->grd_CalcTriMeshFromGrid
        (grid, ncol, nrow,
         gx1, gy1, gx2, gy2,
         NULL, NULL, NULL,
         NULL, NULL, 0,
         GRD_EQUILATERAL,
         &nodes, &edges, &triangles,
         &num_nodes, &num_edges, &num_triangles);
    csw_Free (grid);
    grid = NULL;
    if (istat == -1) {
        csw_Free (zbounds);
        return -1;
    }

/*
 * Resample the boundary line at close to the grid
 * cell size.  Keep the original points in the process.
 */
    istat =
    gpf_resample_line_segments (
        xbounds, ybounds, zbounds, nbounds,
        avspace,
        &xresamp, &yresamp, &zresamp, &nresamp);
    csw_Free (zbounds);
    zbounds = NULL;
    if (istat == -1) {
        return -1;
    }

/*
 * Add the boundary lines as constraints.
 */
    istat = grdapi_ptr->grd_AddLinesToTriMesh (
        xresamp, yresamp, zresamp,
        &nresamp, NULL, 1,
        1,
        &nodes, &edges, &triangles,
        &num_nodes, &num_edges, &num_triangles);
    if (istat == -1) {
        return -1;
    }

/*
 * Clip the trimesh to the interior of the boundary.
 */
    istat = grdapi_ptr->grd_ClipTriMeshToPolygon
        (&nodes, &num_nodes,
         &edges, &num_edges,
         &triangles, &num_triangles,
         xresamp, yresamp, &nresamp, 1, 1);
    csw_Free (xresamp);
    xresamp = NULL;
    yresamp = NULL;
    zresamp = NULL;
    if (istat == -1) {
        csw_Free (nodes);
        csw_Free (edges);
        csw_Free (triangles);
        return -1;
    }




    do_write = csw_GetDoWrite ();
    if (do_write) {
        sprintf (fname, "postclip.tri");
        grdapi_ptr->grd_WriteTextTriMeshFile (
            0, v6,
            triangles, num_triangles,
            edges, num_edges,
            nodes, num_nodes,
            fname);
    }




/*
 * Send the results back to java.
 */
    istat =
    SendBackTriMesh (nodes, num_nodes,
                     edges, num_edges,
                     triangles, num_triangles);

    csw_Free (nodes);
    csw_Free (edges);
    csw_Free (triangles);
    nodes = NULL;
    edges = NULL;
    triangles = NULL;
    num_nodes = 0;
    num_edges = 0;
    num_triangles = 0;

    return istat;

}



/*-----------------------------------------------------------------------------*/

/*
 * Remove any constraint line that crosses a previously defined
 * constraint line.  Previously defined means coming before in the
 * npline array.  This results in an arbitrary line being kept in
 * light of crossing constraints.
 */
int SWCalc::UncrossConstraints (
    double          *xline,
    double          *yline,
    double          *zline,
    int             *npline,
    int             *nlineout)
{
    int             i, j, n, istat, offset1, offset2, np1, np2;
    int             *icross, nline;
    double          *xa1, *ya1, *za1, *xa2, *ya2, *za2, tiny;
    double          *xw;

    if (ModelXmin >= ModelXmax) {
        tiny = .0001;
    }
    else {
        tiny = (ModelXmax - ModelXmin) / 100000.0;
        if (tiny < .0001) tiny = .0001;
    }

    nline = *nlineout;

    if (nline < 2) {
        return 1;
    }

/*
 * Allocate the icross array, which marks
 * which lines should be removed due to crossing.
 */
    icross = (int *)csw_Calloc (nline * sizeof(int));
    if (icross == NULL) {
        return -1;
    }

/*
 * Allocate a work array to avoid overlap in memcpy.
 */
    n = 0;
    for (i=0; i<nline; i++) {
        if (npline[i] > n) n = npline[i];
    }
    if (n < 100) n = 100;
    xw = (double *)csw_Malloc (n * sizeof(double));
    if (xw == NULL) {
        csw_Free (icross);
        return -1;
    }

/*
 * Flag the crossing lines in the icross array.
 */
    offset1 = 0;
    offset2 = 0;
    for (i=0; i<nline; i++) {

        if (icross[i] == 1) {
            continue;
        }
        xa1 = xline + offset1;
        ya1 = yline + offset1;
        np1 = npline[i];
        offset2 = offset1 + np1;

        for (j=i+1; j<nline; j++) {
            xa2 = xline + offset2;
            ya2 = yline + offset2;
            np2 = npline[j];
            istat =
            gpf_check_for_intersection (
                xa1, ya1, np1,
                xa2, ya2, np2,
                tiny);
            if (istat == -1) {
                csw_Free (icross);
                csw_Free (xw);
                return -1;
            }
            if (istat != 0) {
                icross[j] = 1;
            }
            offset2 += np2;
        }

        offset1 += np1;

    }

/*
 * Remove crossing lines from the xline, yline, zline and npline arrays.
 */
    offset1 = 0;
    offset2 = 0;
    n = 0;

    for (i=0; i<nline; i++) {
        np1 = npline[i];
        if (icross[i] == 1) {
            offset1 += np1;
            continue;
        }
        if (n == i) {
            offset1 += np1;
            offset2 += np1;
            n++;
            continue;
        }
        xa1 = xline + offset1;
        ya1 = yline + offset1;
        za1 = zline + offset1;
        xa2 = xline + offset2;
        ya2 = yline + offset2;
        za2 = zline + offset2;
        offset2 += np1;
        memcpy (xw, xa1, np1 * sizeof(double));
        memcpy (xa2, xw, np1 * sizeof(double));
        memcpy (xw, ya1, np1 * sizeof(double));
        memcpy (ya2, xw, np1 * sizeof(double));
        memcpy (xw, za1, np1 * sizeof(double));
        memcpy (za2, xw, np1 * sizeof(double));
        npline[n] = np1;
        n++;
    }

    *nlineout = n;

    csw_Free (icross);
    csw_Free (xw);

    return 1;

}

