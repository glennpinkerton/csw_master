
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

// Add nothing above this ifndef

#ifndef _SW_CALC_H_
#define _SW_CALC_H_

#include <csw/surfaceworks/include/grid_api.h>
#include <csw/surfaceworks/private_include/grd_triangle_class.h>
#include <csw/surfaceworks/private_include/Vert.h>
#include <csw/utils/private_include/ply_utils.h>

#define MAX_DEFAULT_NODES    1000000

typedef struct {
    int    ncol,
           nrow;
    double xmin,
           ymin,
           xmax,
           ymax;
} SWGridGeometryStruct;

typedef struct {
    int    anisotropyFlag,
           preferredStrike,
           thicknessFlag,
           stepFlag,
           directionalFlag,
           triangulateFlag;
    int    removeCrossingConstraintFlag;
    double logBase;
    int    fastFlag;
    int    outsideMargin,
           insideMargin;
    double minValue,
           maxValue,
           hardMin,
           hardMax;
    int    distancePower,
           strikePower,
           numLocalPoints,
           localSearchPattern;
    double maxSearchDistance;
} SWGridOptionsStruct;





class SWCalc
{

  private:

    CSWGrdAPI  *grdapi_ptr = NULL;

    GRDVert    gvert;
    CSWGrdTriangle   gridtri;
    CSWPolyUtils     polyutils;
    


  public:

    SWCalc () {};
    ~SWCalc () {};

    void SetGrdAPIPtr (CSWGrdAPI *p) {
        grdapi_ptr = p;
    }

  private:

    int       fileid = 1;
    double    XOutputShift = 0.0;
    double    YOutputShift = 0.0;

    double    VBC1 = 1.e30,
              VBC2 = 1.e30,
              VBC3 = 1.e30,
              VBX0 = 1.e30,
              VBY0 = 1.e30,
              VBZ0 = 1.e30;
    int       VBFlag = 0;

    double    tnxNorm,
              tnyNorm,
              tnzNorm;

    double    ModelXmin,
              ModelXmax;
    int       FileID = 1;
    int FaultSurfaceFlag = 0;

    int              drapeId1 = 0,
                     drapeId2 = 0;
    NOdeStruct       *drapeNodes = NULL;
    EDgeStruct       *drapeEdges = NULL;
    TRiangleStruct   *drapeTriangles = NULL;
    int              numDrapeNodes = 0;
    int              numDrapeEdges = 0;
    int              numDrapeTriangles = 0;

    double           *drapeXline = NULL;
    double           *drapeYline = NULL;
    double           *drapeZline = NULL;
    int              *drapeNpline = NULL;
    int              drapeNline = 0;

    double           *drapeXpoint = NULL;
    double           *drapeYpoint = NULL;
    double           *drapeZpoint = NULL;
    int              drapeNpoint = 0;


    int SendBackTriMesh
        (NOdeStruct *nodes, int num_nodes,
         EDgeStruct *edges, int num_edges,
         TRiangleStruct *triangles, int num_triangles);

    int SendBackGrid
        (CSW_F *gdata,
         int ncol,
         int nrow,
         double xmin,
         double ymin,
         double xmax,
         double ymax,
         CSW_F *point_errors,
         int npts,
         char *mask);

    int SendBackPointOutline
        (double    *xpoly,
         double    *ypoly,
         int       npoly);

    void ConvertGridCalcOptions (
        SWGridOptionsStruct *swop,
        GRidCalcOptions *gop);

    int check_for_steep (
        NOdeStruct *nodes,
        int num_nodes,
        EDgeStruct *edges,
        TRiangleStruct *triangles,
        int num_triangles
    );

    void TriangleNormal (double *x, double *y, double *z);

    void TrianglePoints (TRiangleStruct *tptr,
                         NOdeStruct *nodes,
                         EDgeStruct *edges,
                         double *x,
                         double *y,
                         double *z);

    int ResampleConstraintLines (
        double        **xline_io,
        double        **yline_io,
        double        **zline_io,
        int           *npline_io,
        int           *exact_flags,
        int           nline,
        double        avspace);

    int UncrossConstraints (
        double        *xline,
        double        *yline,
        double        *zline,
        int           *npline,
        int           *nlineout);

/*
 * !!!! Used for specific debugging only.  If needed, uncomment this prototype.
 */
    void WriteConstraintLines
    (
        const char *fname,
        double *xline2,
        double *yline2,
        double *zline2,
        int *npline2,
        int nline2
    );

    int SendBackVbase (int vused, double *vbase);


  public:

    void free_drape_lines (void);
    void free_drape_points (void);
    void free_drape_trimesh (void);
    
    void sw_SetModelBounds (
        double xmin,
        double ymin,
        double zmin,
        double xmax,
        double ymax,
        double zmax);
    
    int sw_CalcTriMesh (
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
        double *zbounds,
        int    nbounds,
        int    gflag,
        SWGridGeometryStruct  *grid_geometry,
        SWGridOptionsStruct   *grid_options,
        int    fault_surface_flag,
        int    bad_bounds_flag
    );
    
    int sw_CalcExactTriMesh (
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
        double *zbounds,
        int    nbounds,
        int    gflag,
        SWGridGeometryStruct  *grid_geometry,
        SWGridOptionsStruct   *grid_options,
        int    fault_surface_flag,
        int    bad_bounds_flag
    );
    
    int sw_CalcExactTriMeshLocally (
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
        double *zbounds,
        int    nbounds,
        int    gflag,
        SWGridGeometryStruct  *grid_geometry,
        SWGridOptionsStruct   *grid_options,
        int    fault_surface_flag,
        int    bad_bounds_flag,
        NOdeStruct    **nodes,
        int           *num_nodes,
        EDgeStruct    **edges,
        int           *num_edges,
        TRiangleStruct  **triangles,
        int           *num_trangles
    );
    
    int sw_CalcTriMeshLocally (
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
        double *zbounds,
        int    nbounds,
        int    gflag,
        SWGridGeometryStruct  *grid_geometry,
        SWGridOptionsStruct   *grid_options,
        int    fault_surface_flag,
        int    bad_bounds_flag,
        NOdeStruct    **nodes,
        int           *num_nodes,
        EDgeStruct    **edges,
        int           *num_edges,
        TRiangleStruct  **triangles,
        int           *num_trangles
    );
    
    int sw_CalcConstantTriMesh (
        double     *xbounds,
        double     *ybounds,
        int        nbounds,
        double     zvalue);
    
    int sw_CalcGrid (
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
    );
    
    void sw_SetDrapeLineCache (
        double     *x,
        double     *y,
        double     *z,
        int        *np,
        int        nlines
    );
    
    void sw_SetDrapePointCache (
        double     *x,
        double     *y,
        double     *z,
        int        npts
    );
    
    void sw_ClearDrapeCache (void);
    
    int sw_SetDrapeTriMeshCache (
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
    );
    
    int sw_CalcDrapedLines (void);
    
    int sw_CalcDrapedPoints (void);
    
    int sw_ConvertNodeTrimesh (
        double     *xnodes,
        double     *ynodes,
        double     *znodes,
        int        num_nodes,
        int        *n1tri,
        int        *n2tri,
        int        *n3tri,
        int        num_tri);
    
    int sw_WriteTriMesh (
        char       *fname,
        int        bflag,
        int        vused,
        double     *vbase,
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
    );
    long sw_AppendTriMesh (
        char       *fname,
        int        bflag,
        int        vused,
        double     *vbase,
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
    );
    
    int sw_ReadTriMesh (
        char       *fname
    );
    int sw_ReadTriMeshFromMultiFile(
      char *fname,
      long position
    );
    
    int sw_ExtendFaultFromJava (
        double           extension_fraction,
        double           *xnode,
        double           *ynode,
        double           *znode,
        int              nnode);
    
    int sw_ExtendFault (
        double           x_extension_fraction,
        double           y_extension_fraction,
        double           *xnode,
        double           *ynode,
        double           *znode,
        int              nnode,
        TRiangleStruct   **triangles_out,
        int              *ntri_out,
        EDgeStruct       **edges_out,
        int              *nedge_out,
        NOdeStruct       **nodes_out,
        int              *nnode_out);
    
    int sw_GridToTriMesh (
        int              ncol,
        int              nrow,
        double           xmin,
        double           ymin,
        double           xmax,
        double           ymax,
        double           rang,
        double           *gdata,
        double           *xline,
        double           *yline,
        double           *zline,
        int              *npline,
        int              *lineflags,
        int              nline);
    
    void sw_SetOutputShifts (double x, double y);
    
    int sw_OutlinePoints (
        double *xpts,
        double *ypts,
        int    npts);
    
    int sw_PointInPolygon (
        double x,
        double y,
        double graze,
        double *xpts,
        double *ypts,
        int    npts);
    
    int sw_CalcTriMeshOutline (
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
    );
    
    int sw_ReadGrid (char *fname);
    
}; // end of main class
    
    
// Add nothing after this endif
    
#endif
