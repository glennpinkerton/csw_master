
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * This is the class definition for the DLSurf class.  This class
 * is used to store a trimesh or grid object in the display list.
 * The display list has a list of pointers to objects of this class.
 */

#ifndef _DLSURF_H
#define _DLSURF_H

#  include "csw/jeasyx/private_include/DLContour.h"
#  include "csw/surfaceworks/include/contour_api.h"
#  include "csw/surfaceworks/include/grid_api.h"
#  include "csw/utils/include/csw_.h"
#  include "csw/utils/private_include/ply_utils.h"



class DLSurf {

  private:

    CSWContourApi    conapi_obj;
    CSWGrdAPI        grdapi_obj;
    CSWPolyUtils     ply_utils_obj;

  public:

    DLSurf ();
    ~DLSurf ();

    int              image_id;
    int              saved_image_id;
    int              index_num;

    int              frame_num;
    int              layer_num;
    int              deleted_flag;
    int              visible_flag;

    char             name[100];

    double           gxmin,
                     gymin,
                     gxmax,
                     gymax;

    int              turn_off_reclip;
    int              needs_reclip;
    int              needs_contour_reclip;
    int              needs_recalc;
    int              needs_rotation;

    double           ncdata[201000];

    DLContourProperties conprop;

    void SetSurfaceID (int id);

    int  SetGridData (double *data,
                  int ncol,
                  int nrow,
                  double xmin,
                  double ymin,
                  double width,
                  double height,
                  double angle,
                  FAultLineStruct *faults,
                  int nfaults);

    int SetTriMesh (double *xnode,
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
                    int numtri);

    int  SetImageBands (double *zmin,
                        double *zmax,
                        int *red,
                        int *green,
                        int *blue,
                        int *alpha,
                        int nband);

    int CalcContours (void *dlist);
    int CalcContours (void *dlist, int inum);
    int CalcImage (void *dlist);
    int CalcImage (void *dlist,
                   double xmin, double ymin,
                   double xmax, double ymax,
                   int ncol, int nrow);

    int CalcFaultLines (void *dlist);

    int CalcNodes (void *dlist);
    int CalcEdges (void *dlist);

    void *GetContourProperties (void);
    void SetContourProperties (void *dlprops);
    COntourCalcOptions const *GetCalcOptions (void);
    COntourDrawOptions const *GetDrawOptions (void);

  private:

    int              rotate_grid (void);
    int              calc_contours_via_grid (
                       NOdeStruct    *nodes,
                       int           numnodes,
                       EDgeStruct    *local_edges,
                       int           numedges,
                       TRiangleStruct  *local_triangles,
                       int           numtriangles,
                       COntourOutputRec  **contoursout,
                       int           *ncontoursout,
                       COntourCalcOptions *calc_options);
    int              calc_outline_polygon (void);

    CSW_F            *data = NULL;
    CSW_F            *trigrid = NULL;

    double           *xpoly = NULL,
                     *ypoly = NULL,
                     *zpoly = NULL;
    int              *ncpoly = NULL;
    int              *nvpoly = NULL;
    int              npoly;

    int              ncol,
                     nrow;
    int              tncol,
                     tnrow;
    double           xmin,
                     ymin,
                     angle,
                     width,
                     height;

    FAultLineStruct  *faults = NULL;
    int              nfaults;

    double           zmin_band[1000];
    double           zmax_band[1000];
    int              red_band[1000];
    int              green_band[1000];
    int              blue_band[1000];
    int              alpha_band[1000];
    int              nband;

    COntourCalcOptions  calc_options;
    COntourDrawOptions  draw_options;


    double           last_image_xmin;
    double           last_image_ymin;
    double           last_image_xmax;
    double           last_image_ymax;
    int              last_image_ncol;
    int              last_image_nrow;

    NOdeStruct       *nodes = NULL;
    EDgeStruct       *edges = NULL;
    TRiangleStruct   *triangles = NULL;
    int              num_nodes;
    int              num_edges;
    int              num_triangles;

    NOdeStruct       *cnodes = NULL;
    EDgeStruct       *cedges = NULL;
    TRiangleStruct   *ctriangles = NULL;
    int              cnum_nodes,
                     cnum_edges,
                     cnum_triangles;

    int              tmesh_num;

    double           zscale, last_zscale;
    int              grid_zscale_needed;
    int              tmesh_zscale_needed;
    int              band_zscale_needed;
    int              smooth_needed;

};

/*  do not add anything after this endif  */
#endif
