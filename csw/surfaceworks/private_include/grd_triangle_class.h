
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_triangle_class.h

    Define the CSWGrdTriangle class, which refactors the old C code
    from grd_triangle.c.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grd_triangle_class.h.
#endif


/*
    Add nothing above this line
*/
#ifndef GRD_TRIANGLE_CLASS_H
#  define GRD_TRIANGLE_CLASS_H


#include "csw/surfaceworks/include/grd_shared_structs.h"


/*
    Define some constants for the class.
*/
#define TRI_NO_VAL                    1.e30
#define BOUNDARY_EDGE                 99
#define MAX_ITER_TRI                  100
#define CORNER_POINT                  99
#define TRI_INDEX_CHUNK               6
#define CORNER_BIAS                   2.0

#define MAXLIST                       100

#define OUTCHUNK                      1000

#define SWAP_FOR_NULL_REMOVAL         9999
#define FORCE_SWAP                    9998

#define MAX_BOUNDARY_EDGES            100000

#define MAX_CONNECT_EDGES_TRI         10000

#define RIDGE_START_NODE              17
#define RIDGES_ALREADY_TRACED         19
#define TRIANGLE_USED_FOR_RIDGE       1
#define EDGE_USED_FOR_RIDGE           1
#define EDGE_ON_RIDGE                 2
#define NODE_USED_FOR_RIDGE           1

#define MAX_POINTS_IN_COLLECTION      20

#define LIMIT_LINE_FLAG               137

#define _MAX_ADD_REM_                 2000


/*
    Define structures used only in this class.
*/
typedef struct {
    double x,
           y,
           z;
    int    nodenum;
    int    *edgelist = NULL,
           nedge,
           maxedge;
    int    constraint_class;
    int    exact_flag;
    int    lineid;
    char   flag;
    char   deleted;
    char   endflag;
}  RAwPointStruct;

typedef struct {
    int            rp1,
                   rp2;
    int            lineid;
    int            constraint_class;
    int            deleted;
}  RAwLineSegStruct;

typedef struct {
    int   *nodes = NULL;
    int   nlist;
    int   maxlist;
    int   deleted;
    int   forward,
          backward;
    int   poly1,
          poly2,
          uflag;
    char  tflag;
    char  bflag;
} RIdgeLineStruct;

typedef struct {
    int   *list = NULL;
    int   nlist,
          maxlist;
    int   ridgenum;
} CRossNodeStruct;

typedef struct {
    int   trilist[10],
          ridgelist[10],
          edgelist[10],
          nlist,
          nedge,
          nodenum;
} FOrkInfoStruct;

typedef struct {
    double x1, y1, x2, y2;
    int    tri1, tri2;
    int    edge1, edge2;
} CLippedSegment;

typedef struct {
    double x, y, dist;
    int    tnum, edgenum;
    int    graze;
} CLippedPoint;

typedef struct {
    int    points[MAX_POINTS_IN_COLLECTION];
    int    npts;
    int    nlong;
}  POintCollection;

typedef struct {
    int           id1,
                  id2;
    INdexStruct   **index = NULL;
    double        xmin,
                  ymin,
                  xmax,
                  ymax;
    int           ncol,
                  nrow;
    double        xspace,
                  yspace;
}  TRiangleIndexListStruct;

typedef struct {
    int                  id1,
                         id2;
    NOdeEdgeListStruct   *list = NULL;
    int                  nlist;
}  NOdeEdgeCacheStruct;


class CSWGrdTriangle;

#include "csw/surfaceworks/private_include/grd_utils.h"
#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_arith.h"
#include "csw/surfaceworks/private_include/grd_constraint.h"
#include "csw/surfaceworks/private_include/grd_calc.h"
#include "csw/surfaceworks/private_include/grd_tsurf.h"

class CSWGrdTriangle
{

  private:

    CSWGrdArith   *grd_arith_ptr = NULL;
    CSWGrdCalc    *grd_calc_ptr = NULL;
    CSWGrdConstraint  *grd_constraint_ptr = NULL;
    CSWGrdFault   *grd_fault_ptr = NULL;
    CSWGrdTsurf   *grd_tsurf_ptr = NULL;
    CSWGrdUtils   *grd_utils_ptr = NULL;

  public:

    CSWGrdTriangle () {};
    ~CSWGrdTriangle () {};

// It makes no sense to copy construct, move construct,
// assign or move assign an object of this class.  The
// various copy methods are flagged "delete" to prevent
// their use.

    CSWGrdTriangle (const CSWGrdTriangle &old) = delete;
    const CSWGrdTriangle &operator=(const CSWGrdTriangle &old) = delete;
    CSWGrdTriangle (CSWGrdTriangle &&old) = delete;
    const CSWGrdTriangle &operator=(CSWGrdTriangle &&old) = delete;

int do_nothing () {
printf ("\nFrom grd_triangle do nothing\n\n");
fflush (stdout);
return 1;
}

    void  SetGrdArithPtr (CSWGrdArith *p) {grd_arith_ptr = p;};
    void  SetGrdCalcPtr (CSWGrdCalc *p) {grd_calc_ptr = p;};
    void  SetGrdConstraintPtr (CSWGrdConstraint *p) {grd_constraint_ptr = p;};
    void  SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};
    void  SetGrdTsurfPtr (CSWGrdTsurf *p) {grd_tsurf_ptr = p;};
    void  SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

    int   grd_CalcPreciseDoublePlane
                        (double *x, double *y, double *z,
                         int npts, double *coef);
    int   grd_WriteLines
                  (double *x,
                   double *y,
                   double *z,
                   int    np,
                   int    *nc,
                   int    *nv,
                   char *fname);

    int   grd_WritePoints (double *x,
                     double *y,
                     double *z,
                     int npts,
                     char *fname);

    int grd_SubdivideTrianglesFromCenters (
                     int            *trinums,
                     double         *xcenters,
                     double         *ycenters,
                     double         *zcenters,
                     int            ncenters,
                     NOdeStruct     **nodes,
                     int            *numnodes,
                     EDgeStruct     **edges,
                     int            *numedges,
                     TRiangleStruct **triangles,
                     int            *numtriangles);

    int grd_WriteXYZGridFile (
        const char *fname,
        CSW_F   *grid,
        int     ncol,
        int     nrow,
        int     ncoarse,
        double x1, double y1, double x2, double y2);

    int grd_FillNullValues (CSW_F *grid, int ncol, int nrow,
                        CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                        FAultLineStruct *faults, int nfaults,
                        CSW_F nullval, CSW_F *gridout, char *maskout);





    NOdeStruct *getNodeList (void);

    void grd_set_data_check_func (CLIENT_DATA_CHECK_FUNC func_ptr);

    int grd_calc_trimesh_node_normals (
        NOdeStruct      *nodes,
        int             num_nodes,
        EDgeStruct      *edges,
        int             num_edges,
        TRiangleStruct  *tris,
        int             num_tris);
    int grd_free_trimesh_normals (
        NOdeStruct      *nodes,
        int             num_nodes,
        EDgeStruct      *edges,
        int             num_edges,
        TRiangleStruct  *tris,
        int             num_tris);
    double grd_calc_z_for_triangle_normal (
        double          x,
        double          y,
        TRiangleStruct  *tptr);
    double grd_calc_z_for_edge_normal (
        double          x,
        double          y,
        EDgeStruct      *eptr);
    double grd_calc_z_for_node_normal (
        double          x,
        double          y,
        NOdeStruct      *nptr);

    void grd_set_trimesh (
        NOdeStruct      *nodes,
        int             num_nodes,
        EDgeStruct      *edges,
        int             num_edges,
        TRiangleStruct  *tris,
        int             num_tris);
    void grd_unset_trimesh (void);

    void grd_set_dont_do_eq (int ival);

    void setZisAttribute (int ival);

    int grd_insert_crossing_points (
        double      *x1in,
        double      *y1in,
        double      *z1in,
        int         *tag1in,
        int         n1in,
        double      *x2in,
        double      *y2in,
        double      *z2in,
        int         *tag2in,
        int         n2in,
        double      *x1out,
        double      *y1out,
        double      *z1out,
        int         *tag1out,
        int         *n1out,
        double      *x2out,
        double      *y2out,
        double      *z2out,
        int         *tag2out,
        int         *n2out,
        int         maxout,
        double      *xwork,
        double      *ywork,
        double      *zwork,
        int         *tagwork,
        int         maxwork);

    int grd_decimate_tagged_line (
            double *x, double *y, double *z, int *tag, int *n);

    int grd_calc_trimesh (double*, double*, double*, int,
                          double*, double*, double*,
                          int*, int*, int,
                          NOdeStruct**, EDgeStruct**, TRiangleStruct**,
                          int*, int*, int*);
    int grd_remove_nodes_from_trimesh (NOdeStruct **nodelist, int *numnodes,
                                       EDgeStruct **edgelist, int *numedges,
                                       TRiangleStruct **trilist, int *numtris,
                                       int *nodes_to_delete, int num_nodes_to_delete);
    int grd_pointer_grid_to_trimesh
                            (void**, int, int,
                             double, double, double, double,
                             double*, double*, double*,
                             int*, int,
                             int,
                             NOdeStruct**, EDgeStruct**, TRiangleStruct**,
                             int*, int*, int*);
    int grd_grid_to_trimesh (CSW_F*, int, int,
                             double, double, double, double,
                             double*, double*, double*,
                             int*, int*, int,
                             int,
                             NOdeStruct**, EDgeStruct**, TRiangleStruct**,
                             int*, int*, int*);
    int grd_grid_to_equilateral_trimesh
                            (CSW_F*, int, int,
                             double, double, double, double,
                             double*, double*, double*,
                             int*, int*, int,
                             NOdeStruct**, EDgeStruct**, TRiangleStruct**,
                             int*, int*, int*);
    int grd_remove_nulls_from_trimesh (TRiangleStruct*, int*,
                                       EDgeStruct*, int*,
                                       NOdeStruct*, int*,
                                       double, int);

    int grd_calc_grid_from_trimesh (NOdeStruct*, int,
                                    EDgeStruct*, int,
                                    TRiangleStruct*, int,
                                    double,
                                    CSW_F*, int, int,
                                    double, double, double, double);

    int grd_calc_trimesh_bounding_box (NOdeStruct*, int,
                                       EDgeStruct*, int,
                                       TRiangleStruct*, int,
                                       double*, double*, double*, double*,
                                       double*, double*);
    int grd_interpolate_trimesh_values (CSW_F *grid, int ncol, int nrow,
                                        double x1, double y1, double x2, double y2,
                                        NOdeStruct *nodes, int numnodes,
                                        EDgeStruct *edges, int numedges,
                                        TRiangleStruct *triangles, int numtriangles);
    int grd_interpolate_unfaulted_trimesh_values
                                       (CSW_F *grid, int ncol, int nrow,
                                        double x1, double y1, double x2, double y2,
                                        NOdeStruct *nodes, int numnodes,
                                        int flag);

    int grd_get_bug_locations (double *bx, double *by, int *nbugs, int maxbugs);

    int grd_calculate_drainage_polygons (
            CSW_F *grid, int ncol, int nrow,
            double x1, double y1, double x2, double y2,
            int future_flag,
            SPillpointStruct *spill_filter_list,
            int num_spill_filters,
            DRainagesStruct **drainages);

    int grd_break_for_artifact (double x, double y);
    int grd_set_shifts_for_debug (double x, double y);
    int grd_delete_edges_from_trimesh (NOdeStruct *nodes, int numnodes,
                                   EDgeStruct *edges, int numedges,
                                   TRiangleStruct *triangles, int numtriangles,
                                   int *edgenumlist, int nlist,
                                   int swapflag);
    int grd_delete_triangles_from_trimesh (NOdeStruct *nodes, int numnodes,
                                   EDgeStruct *edges, int numedges,
                                   TRiangleStruct *triangles, int numtriangles,
                                   int *trianglenumlist, int nlist);
    int grd_calc_triangle_dips (NOdeStruct *nodes, int numnodes,
                                EDgeStruct *edges, int numedges,
                                TRiangleStruct *triangles, int numtriangles,
                                int degree_flag,
                                double *direction, double *amplitude);
    int grd_get_nodes_for_triangle (TRiangleStruct *triangle,
                                    EDgeStruct *edgelist,
                                    int *n1, int *n2, int *n3);

    int grd_back_interpolate_tri_mesh (TRiangleStruct *triangles, int ntriangles,
                                       EDgeStruct *edges, int nedges,
                                       NOdeStruct *nodes, int numnodes,
                                       double *x, double *y, double *z,
                                       int npts);
    int grd_tri_mesh_from_node_triangles (NOdeTriangleStruct *node_triangles,
                                         int num_node_triangles,
                                         NOdeStruct *nodes, int num_nodes,
                                         EDgeStruct **edges, int *numedges,
                                         TRiangleStruct **triangles, int *numtriangles);

    void grd_set_triangle_index (void *index,
                                 double xmin,
                                 double ymin,
                                 double xmax,
                                 double ymax,
                                 int    ncol,
                                 int    nrow,
                                 double xspace,
                                 double yspace);

    void grd_get_triangle_index (void **index,
                                 double *xmin,
                                 double *ymin,
                                 double *xmax,
                                 double *ymax,
                                 int    *ncol,
                                 int    *nrow,
                                 double *xspace,
                                 double *yspace);

    void grd_free_triangle_index (void *index, int ncells);

    int grd_drape_points_on_tri_mesh (int id1, int id2,
                                    NOdeStruct *nodes, int numnodes,
                                    EDgeStruct *edges, int numedges,
                                    TRiangleStruct *triangles, int numtriangles,
                                    double *xpts, double *ypts,
                                    int npts,
                                    double **xout, double **yout, double **zout,
                                    int *nout);
    int grd_drape_lines_on_tri_mesh (int id1, int id2,
                                    NOdeStruct *nodes, int numnodes,
                                    EDgeStruct *edges, int numedges,
                                    TRiangleStruct *triangles, int numtriangles,
                                    double *xlines, double *ylines,
                                    int *ilines, int nlines,
                                    double **xout, double **yout, double **zout,
                                    int **iout, int *nout);

    int grd_calc_holes_in_tri_mesh (NOdeStruct *nodes, int *num_nodes,
                                    EDgeStruct *edges, int *num_edges,
                                    TRiangleStruct *triangles, int *num_triangles,
                                    double zmin, double zmax);

    int grd_outline_triangles (NOdeStruct *nodes, int num_nodes,
                               EDgeStruct *edges, int num_edges,
                               TRiangleStruct *triangles, int num_triangles,
                               double *xout, double *yout,
                               int *npout, int *ncout, int *nvout,
                               int maxpts, int maxcomps);

    int grd_outline_tri_mesh_boundary
                              (NOdeStruct *nodes, int num_nodes,
                               EDgeStruct *edges, int num_edges,
                               TRiangleStruct *triangles, int num_triangles,
                               double *xout, double *yout, double *zout,
                               int *nodeout,
                               int *npout, int *ncout, int *nvout,
                               int maxpts, int maxcomps);


    int grd_clip_tri_mesh_to_polygon (NOdeStruct **nodes, int *num_nodes,
                                      EDgeStruct **edges, int *num_edges,
                                      TRiangleStruct **triangles, int *num_triangles,
                                      double *xpoly, double *ypoly,
                                      int *polypoints, int npoly, int flag);

    int grd_clip_tri_mesh_to_multiple_polygons
                                     (NOdeStruct *nodes, int *num_nodes,
                                      EDgeStruct *edges, int *num_edges,
                                      TRiangleStruct *triangles, int *num_triangles,
                                      double *xpoly, double *ypoly,
                                      int *polycomps, int *polypoints, int npoly,
                                      int flag);

    int grd_tri_mesh_from_grid_points (
            double *x, double *y, double *z, int npts,
            double *xlines, double *ylines, double *zlines,
            int *line_points, int *line_types, int nlines,
            CSW_F *grid, int ncol, int nrow,
            double xmin, double ymin, double xmax, double ymax,
            NOdeStruct **nodes, int *num_nodes,
            EDgeStruct **edges, int *num_edges,
            TRiangleStruct **triangles, int *num_triangles);

    int grd_set_poly_constraint_flag (int val);

    int grd_filter_data_spikes (double *xin, double *yin, double *zin,
                                int *ibad, int nin,
                                SPikeFilterOptions *options);

    int grd_add_lines_to_trimesh
        (double *xlinesin, double *ylinesin, double *zlinesin,
         int *linepointsin, int *lineflagsin, int nlinesin,
         int exact_flag,
         NOdeStruct **nodes_out, EDgeStruct **edges_out,
         TRiangleStruct **triangles_out,
         int *num_nodes_out, int *num_edges_out, int *num_triangles_out);

    double grd_calc_average_edge_length (
        NOdeStruct   *nodes,
        EDgeStruct   *edges,
        int          num_edges,
        int          long_short_flag);

    int grd_decimate_constraint (
        double       *xline,
        double       *yline,
        double       *zline,
        int          *nline,
        double       decimation_distance);

    int grd_remove_sliver_triangles (
        NOdeStruct         **nodes,
        EDgeStruct         **edges,
        TRiangleStruct     **triangles,
        int                *num_nodes,
        int                *num_edges,
        int                *num_triangles,
        double             edge_length_ratio);

    int grd_remove_tri_index (int id1, int id2);

    void grd_free_tri_index_list (void);

    int grd_set_remove_zero_flag (int ival);

    int grd_chew_up_triangles (
        int            startnode,
        NOdeStruct     *nodes,
        int            *num_nodes,
        EDgeStruct     *edges,
        int            *num_edges,
        TRiangleStruct *tris,
        int            *num_tris);

    int grd_remove_deleted_trimesh_elements (
        NOdeStruct     *nodes,
        int            *num_nodes,
        EDgeStruct     *edges,
        int            *num_edges,
        TRiangleStruct *tris,
        int            *num_tris);

    int grd_reshape_triangles (
        NOdeStruct     *nodes,
        int            *num_nodes,
        EDgeStruct     *edges,
        int            *num_edges,
        TRiangleStruct *tris,
        int            *num_tris);

    int grd_reset_trimesh_zvalues_from_grid (
        NOdeStruct     *nodes,
        int            num_nodes,
        EDgeStruct     *edges,
        int            num_edges,
        TRiangleStruct *tris,
        int            num_tris,
        CSW_F          *grid,
        int            ncol,
        int            nrow,
        double         gxmin,
        double         gymin,
        double         gxmax,
        double         gymax,
        FAultLineStruct  *faults,
        int            nfaults,
        int            interp_flag,
        int            *lineids
        );

    int grd_smooth_trimesh_nodes_directly (
        NOdeStruct     *nodes,
        int            numnodes,
        EDgeStruct     *edges,
        int            numedges,
        TRiangleStruct *tris,
        int            numtris,
        int            sfact,
        double         avlen);

    int grd_smooth_trimesh_nodes (
        NOdeStruct     *nodes,
        int            numnodes,
        EDgeStruct     *edges,
        int            numedges,
        TRiangleStruct *tris,
        int            numtris,
        int            sfact,
        double         avlen);

    int grd_bezier_smooth_trimesh_nodes (
        NOdeStruct     *nodes,
        int            numnodes,
        EDgeStruct     *edges,
        int            numedges,
        TRiangleStruct *tris,
        int            numtris,
        int            sfact);

    int grd_bezier_smooth_triangle (
        TRiangleStruct *tptr,
        EDgeStruct     *edges,
        NOdeStruct     *nodes,
        double         xin,
        double         yin,
        double         *zout);

    int grd_bezier_smooth_triangle_center (
        TRiangleStruct *tptr,
        double         *zout);

    void grd_set_organize_lines_flag (int ival);

    void grd_validate_topology (
        NOdeStruct      *nodes,
        int             numnodes,
        EDgeStruct      *edges,
        int             numedges,
        TRiangleStruct  *tris,
        int             numtris,
        char            *msg);

    int grd_WriteTextTriMeshFile
                 (int vused, double *vbase,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename);

    long grd_AppendTextTriMeshFile
                 (int vused, double *vbase,
                  TRiangleStruct *triangles, int ntriangles,
                  EDgeStruct *edges, int nedges,
                  NOdeStruct *nodes, int numnodes,
                  char *filename);

    int grd_calc_tri_mesh_from_grid (CSW_F *grid, int nc, int nr,
                             double x1, double y1, double x2, double y2,
                             double *xlines, double *ylines, double *zlines,
                             int *linepoints, int *linetypesin, int nlines,
                             int trimesh_style,
                             NOdeStruct **nodes_out, EDgeStruct **edges_out,
                             TRiangleStruct **triangles_out,
                             int *num_nodes_out, int *num_edges_out,
                             int *num_triangles_out);

  private:


/*
    Old static file variables become private class variables.
*/
    int                  SwapDebug = 0;
    int                  DontFree = 0;
    int                  NewEdgeTflag = 0;

    int                  DontDoEquilateral = 0;

    int                  ListNullNeeded = 0;

    int                  RemoveZeroFlag = 1;
    int                  RemoveNodeForZeroAreaFlag = 0;

    int                  OutsidePointAdjust = 0;

    TRiangleIndexListStruct  *TriIndexList = NULL;
    int                  MaxTriIndexList = 0,
                         NumTriIndexList = 0;

    NOdeEdgeCacheStruct  *NodeEdgeCache = NULL;
    int                  NumNodeEdgeCache = 0,
                         MaxNodeEdgeCache = 0;

    NOdeStruct           *NodeList = NULL, *NlistReturn = NULL;
    EDgeStruct           *EdgeList = NULL, *ElistReturn = NULL;
    TRiangleStruct       *TriangleList = NULL, *TlistReturn = NULL;

    INdexStruct          **TriangleIndexGrid = NULL;
    double               TriangleIndexXmin = 1.e30,
                         TriangleIndexYmin = 1.e30,
                         TriangleIndexXmax = -1.e30,
                         TriangleIndexYmax = -1.e30,
                         TriangleIndexXspace = 1.e30,
                         TriangleIndexYspace = 1.e30;
    int                  TriangleIndexNcol = 0,
                         TriangleIndexNrow = 0;

    INdexStruct          **IndexGrid = NULL;
    double               IndexXmin,
                         IndexYmin,
                         IndexXmax,
                         IndexYmax,
                         IndexXspace,
                         IndexYspace;
    int                  IndexNcol,
                         IndexNrow;

    double               AverageEdgeLength = -1.0;
    int                  MaxIndexPoint = -1;
    int                  NumLinePoints = 0;

    char                 *SwapFlags = NULL;
    int                  FinalSwapFlag = 0,
                         NumSwapped;

    int                  SplitLongFlag = 0;
    int                  SplitLongLength = 0;

    int                  NumNodes,
                         OrigNumNodes = -1,
                         NumEdges,
                         NumTriangles;
    int                  MaxTriangles,
                         MaxEdges,
                         MaxNodes;
    int                  ExpandNodeList = 1,
                         ExpandEdgeList = 1,
                         ExpandTriList = 1;

    int                  BoundaryEdges[MAX_BOUNDARY_EDGES];
    int                  NumBoundaryEdges;

    RAwPointStruct       *RawPoints = NULL;
    RAwPointStruct       *ConstraintRawPoints = NULL;
    int                  NumRawPoints,
                         NumRawCheck,
                         MaxRawPoints;
    int                  NumConstraintRawPoints = 0,
                         MaxConstraintRawPoints = 0;
    RAwLineSegStruct     *RawLines = NULL;
    int                  NumRawLines;

    int                  CheckNullPointers = 0;

    int                  ConstraintSegmentClass;
    int                  ConstraintSegmentNumber;
    int                  ConstraintLineID = -1;
    int                  ConstraintPointStart = 0;
    int                  ClipToGridFlag = 0;

    int                  UseClineFlags = 0;
    int                  *CLineFlags = NULL;
    int                  NumCLineFlags = 0;

    int                  *NodesToLock = NULL;
    int                  NumNodesToLock = 0;
    int                  MaxNodesToLock = 0;

    double               GrazeDistance = 0.0;
    double               AreaPerimeter = 0.0;
    double               AdjustDistance = 0.0;
    double               NullValue = 1.e30;
    double               FaultAdjustDistance = 0.0;

    double               AreaSize = 0.0;

    double               CornerBias = CORNER_BIAS;
    int                  UseCornerFlag = 0;
    int                  EdgeSwapFlag = GRD_SWAP_ANY;
    int                  NumCornerNodes = 0;
    int                  SnapToEdge = 1;

    int                  ConvexHullFlag = 0;

    double               *Xchop = NULL,
                         *Ychop = NULL,
                         *Zchop = NULL;
    int                  *ChopPoints = NULL,
                         *ChopFlags = NULL;
    int                  Nchop;

    int                  NumSame = 0;

    double               GridX1 = 0.0;
    double               GridY1 = 0.0;
    double               GridX2 = 0.0;
    double               GridY2 = 0.0;
    double               GridXspace = 0.0;
    double               GridYspace = 0.0;
    int                  GridNcol = 0;
    int                  GridNrow = 0;

    double               StaticCriticalDistance = -1.0;
    int                  Orignt1,
                         Orignt2;

    int                  ExtendDiagonalsFlag = 0;

    double               SegmentIntX = 0.0,
                         SegmentIntY = 0.0;

    double               *Xline = NULL,
                         *Yline = NULL,
                         *Zline = NULL;
    double               *Xout = NULL,
                         *Yout = NULL,
                         *Zout = NULL;
    double               XoutTiny,
                         XYTiny,
                         XoutLast = 1.e30,
                         YoutLast = 1.e30;
    int                  LastSegmentFlag = 0;
    int                  *Iout = NULL,
                         Nlout,
                         Npout,
                         Npsav,
                         Maxlout,
                         Maxpout;
    double               XlineMin,
                         YlineMin,
                         XlineMax,
                         YlineMax;
    int                  *Iline = NULL,
                         *Iflag = NULL,
                         Nline;
    double               ConstraintX1 = 0.0,
                         ConstraintY1 = 0.0,
                         ConstraintX2 = 0.0,
                         ConstraintY2 = 0.0,
                         ConstraintZ1 = 1.e30,
                         ConstraintZ2 = 1.e30;

    int                  _RemoveFlag = 1;

    int                  PolygonalizeConstraintFlag = 0;

    int                  NtryFlag = 0;

    double               BugX[10],
                         BugY[10];
    int                  Nbugs = 0;

    int                  Ncall,
                         MaxNcall;

    double               Xshift,
                         Yshift;

    RIdgeLineStruct      *RidgeLineList = NULL;
    int                  NumRidgeLines,
                         MaxRidgeLines;
    FOrkInfoStruct       *ForkList = NULL;
    int                  NumFork,
                         MaxFork;
    NOdeEdgeListStruct   *NodeEdgeList = NULL;
    int                  NumNodeEdgeList = 0;

    int                  *PolySegs = NULL;
    POlygonStruct        *PolygonList = NULL;
    int                  NumPolygons,
                         MaxPolygons;
    int                  DrainageFlag;
    int                  LastUpDipRidgeNumber = -1;

    SPillpointStruct     *SpillpointList = NULL;
    int                  NumSpillpoints;
    SPillpointStruct     *SpillFilterList = NULL;
    int                  NumSpillFilters;
    double               SpillFilterDistance;
    int                  SpillTypeForSearch;
    int                  NcolSaddle = 0,
                         NrowSaddle = 0;
    CSW_F                *GridSaddle = NULL;
    double               X1Saddle,
                         Y1Saddle,
                         XspaceSaddle,
                         YspaceSaddle;

    int                  BoundaryRidgeCrossingCount;

    int                  ChopLinesFlag = 1;

    int                  GrazingIntersection = 0;
    double               GrazingXt = 1.e30,
                         GrazingYt = 1.e30;

    CLIENT_DATA_CHECK_FUNC DataCheckFunc = NULL;

    int                  ForceValidate = 0;

    int                  Nsearch = 0,
                         Ntpt = 0;

    double               XCenter = 1.e30;
    double               YCenter = 1.e30;

    int                  ExactFlag = 0;

    char                 *ChewFlags = NULL;

    int                  OrganizeLinesFlag = 1;

    int                  ZisAttribute = 0;

    double               tnxNorm = 0.0;
    double               tnyNorm = 0.0;
    double               tnzNorm = 1.0;

    int                  p_int_1000[1000];

    int                  artifact_flag = 0;

    int                  p_int_mce[MAX_CONNECT_EDGES_TRI];
    double               p_double_mce1[MAX_CONNECT_EDGES_TRI];
    double               p_double_mce2[MAX_CONNECT_EDGES_TRI];

    int                  p_int_mar1[_MAX_ADD_REM_];
    int                  p_int_mar2[_MAX_ADD_REM_];
    int                  p_int_mar3[_MAX_ADD_REM_];
    int                  p_int_mar4[_MAX_ADD_REM_];



/*
 * !!!! for debug only
    int                  FileID = 1;
 */


/*
 *  Old static file functions become private class methods.
 */
    int RemoveNullsFromTriMesh (double nullval, int edge_swap_flag);

    void AveragePairedZValues (void);
    int SameNodePosition (int n1, int n2);

    int FilterConstraintLines (
        double    *xlines,
        double    *ylines,
        double    *zlines,
        int       *linepoints,
        int       *lineflags,
        int       *nlines);
    int FilterSingleConstraintLine (
        double    *x,
        double    *y,
        int       npts);

    int AddStaticTriIndex (int id1, int id2);
    int AddTriIndex (int id1, int id2,
                     INdexStruct  **index,
                     double xmin, double ymin, double xmax, double ymax,
                     int ncol, int nrow,
                     double xspace, double yspace);
    int RemoveTriIndex (int id1, int id2);
    TRiangleIndexListStruct *FindTriIndex (int id1, int id2);
    void FreeTriIndexList (void);

    int AddStaticNodeEdge (int id1, int id2);
    int AddNodeEdge (int id1, int id2,
                     NOdeEdgeListStruct *list,
                     int nlist);
    int RemoveNodeEdge (int id1, int id2);
    NOdeEdgeCacheStruct *FindNodeEdge (int id1, int id2);
    void FreeNodeEdgeCache (void);
    int IsNodeEdgeInCache (NOdeEdgeListStruct *list);

    int GetNodeEdgeList (int nodenum, int **elist);

    int WhackEdgesInsideFaultPolygons (void);
    int ConnectFaultEdges (int edgenum);

    int CheckForCoincidentConstraintPoint (double x, double y);

    int CheckForOppositeNodeMatch (int edgenum, int nodenum);

    double InterpolateZ
               (double x1, double y1, double z1,
                double x2, double y2, double z2,
                double xint, double yint);

    int FindExistingTriangleNode
        (double xt, double yt, double zt, int checkFan);
    int SetupExistingTriangleNode
        (double xt, double yt, double zt);

    int UnsplitFromCentralNode
    (int center_node, int edge1, int edge2);

    int GetShellAroundNode
       (int center_node_num,
        int **nodes_out,
        int *num_nodes_out,
        int **edges_out,
        int *num_edges_out);

    int PreDeleteNodesCloseToConstraints (void);
    int RemoveNodesCloseToConstraints (void);
    int RemoveNodesCloseToConstraints2 (void);

    int CorrectOutsideConstraintPoints (void);
    int AdjustForOutsideConstraintPoints (void);

    int SwapCentralNodeEdges (
        int    center_node,
        int    edge1,
        int    edge2);

    int CheckNeighborRemove (int nodenum);

    int RemoveNodesOnConstraints (void);
    int CheckForNodeOnConstraint
        (RAwPointStruct *rp1, RAwPointStruct *rp2);
    int CheckEdgeListForOppositeNodeOnSegment (
        int        n1,
        int        n2,
        int        *edgenum);

    int FlagEdgeNodes (void);
    int FlagEdgeNonConstraintNodes (void);

    void TriDebugFunc1(void);

    void dumpEdgeInfo (int edgenum);

    int pr_tri (int tnum);
    void PrintRidgeInfo (void);
    void PrintRidgeNodes (int ridgenum);

    void ValidateEdgeIntersection (const char *msg);
    void ValidateConstraints (void);
    void ValidateExactConstraints (void);
    void ValidateTriangles (char *msg);
    void ValidateTriangleShapes (void);

    int CreateTriangleIndexGrid (void);
    int AddToIndexGrid (int start, int end);
    int CreateIndexGrid (double, double, double, double);
    int AddIndexTriangle (INdexStruct*, int);
    int AddIndexPoint (INdexStruct*, int);
    int GetIndexPointXY (int index, int trinum, double *distmin);
    int GetIndexPoints (double xmin, double ymin, double xmax, double ymax,
                           int *list, int maxlist);
    int SwapEdge (int);
    int SwapEdge2 (int);
    int GetSwapCoords (int edgenum,
                       double *x1, double *y1,
                       double *x2, double *y2,
                       int *node1, int *node2);
    int GetSwapCoordsForNodeRemoval
                         (int edgenum,
                          double *x1, double *y1,
                          double *x2, double *y2,
                          int *node1, int *node2);
    int SwapCornerEdge (int);
    int SwapEdgeForNodeRemoval (int);
    int CommonNode (int, int);
    int LocalCommonNode (int, int, EDgeStruct*);
    double NodeDistance (int, int);
    double NodeDistanceXYZ (int, int);
    int PointInTriangle (double, double, int);
    int PointInTriangle3 (double, double,
                          double, double,
                          double, double,
                          double, double);
    int TriangleBounds (int, double*, double*, double*, double*);
    int FindPointInTriangle (int);
    int SplitTriangle (int, int);
    int SplitTriangleXYZ (int triangle_num,
                          double xp, double yp, double zp);
    int SubdivideTriangles (void);
    int FreeMem (void);
    void ListNull (void);
    int AddNode (double, double, double, int);
    int AddEdge (int, int, int, int, int);
    int AddTriangle (int, int, int, int);
    int ExpandMem (void);
    int ExpandMem2 (void);

    int AddRawLineSeg (RAwPointStruct*, RAwPointStruct*, int lineid);
    int BuildRawPointEdgeLists (void);
    int BuildNodeEdgeLists (void);
    int AddEdgeToRawPoint (RAwPointStruct*, int);
    int ApplyConstraints (void);

    int CompressRawLines (void);

    int OppositeEdge (TRiangleStruct*, int);
    int LocalOppositeEdge (TRiangleStruct*, int, EDgeStruct*);
    int OppositeNode (int, int);
    int LocalOppositeNode (int, int,
                           EDgeStruct*, TRiangleStruct*);
    int EdgeIntersect (int, int, int,
                       double*, double*, double*);
    int EdgeIntersectXY (int, int, int allow_graze,
                         double x1, double y1, double x2, double y2,
                         double*, double*, double*);
    int ProcessRawLines (void);
    int SwapExactLines (void);
    int ProcessConstraintSegment (int, int*, int, int);
    int CreateNewTriangleForConstraint (int node1, int node2);
    int SwapForExactConstraintSegment (int, int*, int, int);
    int FindTriangleEdge (TRiangleStruct*, int, int);
    int SamePointAsNode (double, double, int);

    void FatalMessage(char *);

    int ChopLines (double *xlines, double *ylines, double *zlines,
                   int *linepoints, int *lineflags, int nlines,
                   double xmin, double ymin, double xmax, double ymax,
                   int npts);
    void FreeChopData (void);

    int SamePointTiny (double x1, double y1, double x2, double y2, double tiny);
    int SamePoint (double x1, double y1, double x2, double y2);
    int SameRawPoint (int p1, int p2);
    int AddConstraintRawPoint (NOdeStruct*, int*, int);
    int AdjustConstraintNode (int n1, int edgenum);

    int RemoveBorderNode (int nodenum,
                          int *elist,
                          int nlist);
    int RemoveNode (int nodenum);
    int AddEdgeToNodeList (int nodenum, int edgenum);
    void RemoveEdgeFromNodeList (int nodenum, int edgenum);

    void RemoveZeroLengthEdge (int edgenum);
    void RemoveZeroLengthEdges (void);
    void RemoveZeroAreaTriangles (void);
    int FindMiddleNode (int n1, int n2, int n3);
    int  CheckZeroAreaTriangle (int index);
    void RemoveZeroAreaTriangle (int tnum);

    double CalcEquilateralness (int n1, int n2, int n3);
    int NodeOnSegment (int nchk, int n1, int n2, double *dperp);
    int NodeOnSegmentForNodeRemoval (int nchk, int n1, int n2, double *dperp);
    int NodeOnEdge (int, int);
    int SplitFromEdge (int edgenum, int nodenum, int *elist);
    void SplitSingleTriFromEdge (int told,
                                 int eold,
                                 int enew,
                                 int esplit,
                                 int tnew,
                                 int edgetrinum);

    int NodeIsNull (int);
    void WhackEdge (int);
    int RemoveDeletedElements (void);

    void TrianglePoints (TRiangleStruct*, double*, double*, double*);
    void LocalTrianglePoints (TRiangleStruct*,
                              NOdeStruct *nodes, EDgeStruct *edges,
                              double*, double*, double*);

    int FixLineDefects (double *x, double *y, double *z,
                        int *linepoints, int *lineflags, int nlines);
    void WeedClosePoints (double *x, double *y, void **tag, int npts, double,
                          double *xout, double *yout, void **tagout, int *nout);
    int NodeOnConstraint (int);
    void RecursiveAdjust (int nodenum, int t1, int t2);
    int SnapToRawPoint (int start, int end, double xin, double yin);

    int FindInterpolationTriangle (int nodenum,
                                   int *list, int nlist,
                                   NOdeStruct *nodes,
                                   EDgeStruct *edges,
                                   TRiangleStruct *triangles,
                                   double softchk, double hardchk);
    int BuildTempNodeEdgeLists (EDgeStruct *edges, int nedges,
                                NOdeStruct *nodes, double softchk,
                                int **lists, int *nlists, int *maxlists);
    int CheckNullCorners (int tnum, double soft, double hard,
                          NOdeStruct *nodes, EDgeStruct *edges,
                          TRiangleStruct *triangles);
    double InterpolateConstraintZvalue (double xt, double yt);
    double TrianglePlaneZvalue (int tnum, double x, double y);
    double ChooseTrianglePlaneZvalue (int t1, int t2, double x, double y);

    int SetPossibleRidgeEdges (void);
    int BuildInitialDrainageEdgeList (void);
    int CleanInitialDrainageEdgeList (void);
    int AddToNodeEdgeList (int edgenum, int nodenum);
    int RemoveFromNodeEdgeList (int edgenum, int nodenum);
    int AddNodeToRidgeLine (RIdgeLineStruct *rptr, int nodenum);
    int PrependNodeToRidgeLine (RIdgeLineStruct *rptr, int nodenum);
    RIdgeLineStruct *NextRidgeLine (void);
    void DeleteHangingRidgeLines (void);
    int BuildPolygonsFromRidgeLines (void);
    int TraceSinglePolygon (int start_node, RIdgeLineStruct *start_ridge_line);
    int ChooseConnectingRidge (int *list, int nlist, int me,
                                  int nodenum, int *right_left_flag);
    int AddPolygonToOutput (int nseg);

    int CheckForEdgeMaximum (int nodenum);
    int CheckForEdgeMinimum (int nodenum);
    int TraceByDip (int nodenum);
    double OppositeZvalue (EDgeStruct *eptr, int node);
    int AdjacentEdge (TRiangleStruct *tptr, int e1, int node);
    int CheckForSaddle (int nodenum);
    int CheckForBoundarySaddle (int nodenum);
    int FindRidgeExit (TRiangleStruct *tptr,
                       int nodenum,
                       EDgeStruct *eptrin);
    int FindDownExit (TRiangleStruct *tptr,
                      int nodenum);
    int TraceRidgeUpDip (int start_node, int next_edge_num, int eflag);
    int CrossesOnEndPoint (EDgeStruct *eptr);
    int IsEdgeOnRidge (int edgenum);

    int CalcTriangleSlope (TRiangleStruct *tptr,
                           double *strike, double *dip, double *coef);

    int SplitRidge (RIdgeLineStruct *rptr);
    int SplitRidgeAtNode (int nodenum);
    int AddPolygonToSpillpoint (int nodenum, int polynum);

    int AddBoundaryLowsToSpillpointList (void);
    int CheckForIdenticalRidges (RIdgeLineStruct *rp1, RIdgeLineStruct *rp2);
    void DeleteIdenticalRidges (void);
    int CheckPolygonForSpillpoint (int nseg);
    void DeleteRidgeLine (RIdgeLineStruct *rptr);

    int AddSplitNodeToEdge (int edgenum);
    int AssignRidgeToEdge (int edgenum, int ridgenum);
    void FreeRidgeFromEdge (void *vptr);
    int SplitRidgeAtEdge (int edgenum);
    int ChooseForkingRidges (int *trilist, int *ridgelist, int nlist, int nodenum,
                             int *edgelist, int nedge);
    int SaveForkingRidgeInfo (int *trilist, int *ridgelist, int nlist, int nodenum,
                              int *edgelist, int nedge);

    int CommonEdge (int tn1, int tn2);
    int OrderTrianglesAroundNode (int nodenum,
                                  int *ordered_list,
                                  int *num_ordered,
                                  int *edgelist,
                                  int nedge);
    int CornerNode (TRiangleStruct *tp1,
                    TRiangleStruct *tp2,
                    int nodenum);

    int CombineBoundaryRidges (void);
    int ExtendBoundaryRidge (RIdgeLineStruct *);
    int CreateBoundaryRidges (void);
    void DeleteBoundaryRidges (void);
    int SearchSpillFilters (double x, double y);

    int VerifySaddle (int nodenum);
    int VerifyEdgeSaddle (int nodenum);
    int CheckGridInteriorSaddle (int col, int row);
    int CheckGridEdgeSaddle (int col, int row);

    int FindEdgeAtNode (int n1, int n2);

    int FindTriangleContainingPoint (double x, double y);
    int FindEdgeContainingPoint (int trinum, double x, double y);
    int SearchIndexForTriangle (double xp, double yp, int k);
    int TraceLineOnTriMesh (double *xlines,
                            double *ylines,
                            int npts);
    int ProcessDrapedSegment (double x1, double y1,
                              double x2, double y2);
    int AppendOutPoint (double x, double y, double z);
    int OutPointGraze (double x1, double y1, double x2, double y2);

    int ClipSegmentToTriMesh (double x1, double y1,
                              double x2, double y2,
                              CLippedSegment **results,
                              int *nresults);
    int ClipLineToTriMesh (double *xline, double *yline, int npts,
                           double **xout, double **yout,
                           int **iout, int *nout);

    int IsConstraintPoint (double x, double y,
                           double *xl, double *yl, int nltot);
    int AdjustForConstraintPoints (void);

    int SplitLongEdges (void);

    int CheckTriangleNodeGraze (int itri, double x, double y);


    int FindExistingGridCornerNode (double xt, double yt, double zt, int checkFan);
    void ResetGridStaticValues (void);

    void AddNodeToLock (int nodenum);
    void LockNodesToLock (void);
    int FindEdgeInList (int *list, int nlist, int n1, int n2);
    void ExtendVectors (double *x1, double *y1,
                        double *x2, double *y2,
                        double *x3, double *y3,
                        double *x4, double *y4);
    int CheckTriangleFanForGridCornerNode (int node, double x, double y);
    void ShrinkVector2 (double x1, double y1,
                        double *x2, double *y2);

    void RecurseChew (int index, int *nwork, int *work1, int *work2);

    double CalcLocalZ (double *xloc, double *yloc, double *zloc, int nloc);

    int CalcBezierControlPoints (TRiangleStruct *tptr,
                                 double *bcpoints);
    int CalcBezierControlPointsFromNodes (
    int n1, int n2, int n3, double *bpoints);

    double CalcZForNodeNormal (double x, double y,
                               NOdeStruct *nptr);
    double CalcZForTriangleNormal (double x, double y,
                                   TRiangleStruct *nptr);
    double CalcZForEdgeNormal (double x, double y,
                               EDgeStruct *eptr);

    int SmoothTrimeshUsingGrid (int sfact, double avlen);

    void TrianglePoints2 (TRiangleStruct *tptr,
                          NOdeStruct *nodes,
                          EDgeStruct *edges,
                          double *x,
                          double *y,
                          double *z);

    void TriangleNormal (double *x, double *y, double *z);
    int CalcNodeNormals (void);

    void CleanupTriNormals (void);
    void CleanupNodeNormals (void);

    void CorrectBorderEdgeForClippedConstraint (int i);
    int RawPointBorderEdgeIntersect
        (int rpnum, double *xintout, double *yintout);

    int OrganizeConstraintLines (double **xlines,
                                 double **ylines,
                                 double **zlines,
                                 int    **lplines,
                                 int    **lflines,
                                 int    *nlines);
    int ConcatLines (double *xlines,
                     double *ylines,
                     double *zlines,
                     int    *lplines,
                     int    *lflines,
                     int    *nlines);
    int OrganizeCrossingLines (double *xlines,
                               double *ylines,
                               double *zlines,
                               int    *lplines,
                               int    *lflines,
                               int    *nlines,
                               int    npmax,
                               int    nlmax);

    int CreateTriMeshFaults (FAultLineStruct **faults,
                             int             *nfaults);

    void null_array (double *ptr, int n);
    int  _CheckTcheck (int *list, int tnum);

    int RemoveNodeOnConstraint (
        int    center_node,
        int    edge1,
        int    edge2);


}; // end of main class

/*
    Add nothing to this file below the following endif
*/
#endif
