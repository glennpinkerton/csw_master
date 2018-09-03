
/*
     ************************************************
     *                                              *
     *    Copyright (1997-2017) Glenn Pinkerton.    *
     *    All rights reserved.                      *
     *                                              *
     ************************************************
*/

/*
grid_api.h

Define the CSWGrdApi class, which provides public functions
that can be used by applications for grid and trimesh operations.
*/

/*
Add nothing above this line
*/
#ifndef GRID_API_H
#  define GRID_API_H

/*
include the csw utilities header file.
*/
#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

#  include <memory>

#  include "csw/utils/include/csw_.h"
#  include "csw/surfaceworks/include/grd_shared_structs.h"


#define  VERSION        "2.2.0"

/*
Define grid option constants.
*/
#define GRD_DEFAULT_OPTIONS          0
#define GRD_PREFERRED_STRIKE         1
#define GRD_OUTSIDE_MARGIN           2
#define GRD_INSIDE_MARGIN            3
#define GRD_FAST_FLAG                4
#define GRD_MIN_VALUE                5
#define GRD_MAX_VALUE                6
#define GRD_REPORT_LINES             7
#define GRD_ANISOTROPY_FLAG          8
#define GRD_DISTANCE_POWER           9
#define GRD_STRIKE_POWER             10
#define GRD_NUM_LOCAL_POINTS         11
#define GRD_LOCAL_SEARCH_PATTERN     12
#define GRD_MAX_SEARCH_DISTANCE      13
#define GRD_THICKNESS_FLAG           14
#define GRD_STEP_GRID_FLAG           15
#define GRD_WORK_MARGIN              16
#define GRD_TRIANGULATE_FLAG         17
#define GRD_DEFAULT_SIZE_MULTIPLIER  18

#define GRD_FAULTED_GRID_FLAG        1001

#define GRD_TRUE                     1
#define GRD_FALSE                    0

#define GRD_QUADRANT_SEARCH          1
#define GRD_OCTANT_SEARCH            2
#define GRD_RADIAL_SEARCH            3

#define GRD_NONE                     0
#define GRD_GLOBAL_ANISOTROPY        1
#define GRD_LOCAL_ANISOTROPY         2

#define GRD_ADD                      1
#define GRD_SUBTRACT                 2
#define GRD_MULTIPLY                 3
#define GRD_DIVIDE                   4
#define GRD_RECIPROCAL               5
#define GRD_EXPONENT                 6
#define GRD_LOG                      7
#define GRD_MINIMUM                  8
#define GRD_MAXIMUM                  9
#define GRD_CONSTANT_MINUS           10
#define GRD_REPLACE_NULLS            11
#define GRD_POWER                    12
#define GRD_CALL_FUNC                999

#define GRD_ONE_GRID_ARITH           1
#define GRD_TWO_GRID_ARITH           2

#define GRD_BILINEAR                 1
#define GRD_BICUBIC                  2
#define GRD_STEP_GRID                3

#define GRD_NORMAL_GRID_FILE         1
#define GRD_THICKNESS_GRID_FILE      2
#define GRD_STEP_GRID_FILE           3

#define GRD_OUTSIDE_ONLY             0
#define GRD_INSIDE_AND_OUTSIDE       1

#define GRD_TAG_SIZE                 90
#define GRD_COMMENT_SIZE             200
#define GRD_MAX_MULTI_FILES          10000

#define GRD_MASK_VALID               '\0'
#define GRD_MASK_INVALID_OUTSIDE     '\1'
#define GRD_MASK_INVALID_INSIDE      '\2'
#define GRD_MASK_INVALID_DERIVED_OUTSIDE '\3'
#define GRD_MASK_INVALID_DERIVED_INSIDE '\4'
#define GRD_MASK_USER_EDIT_RESET     '\77'
#define GRD_MASK_ENABLED_BY_RECALC   '\66'

#define GRD_MASK_CROSSING_FLAG       '\10'

#define GRD_CONTROL_GREATER          1
#define GRD_CONTROL_LESS             2
#define GRD_CONTROL_BETWEEN          3

#define GRD_CONFORMABLE              1
#define GRD_BASELAP                  2
#define GRD_TRUNCATION               3

#define GRD_NAME_LENGTH             64

#define GRD_POSITIVE_UP              1
#define GRD_POSITIVE_DOWN           -1

#define GRD_UNKNOWN_FAULT           0
#define GRD_VERTICAL_FAULT          1
#define GRD_NON_VERTICAL_FAULT      2
#define GRD_BLOCK_POLYGON_FAULT     3

#define GRD_CLIP_MASK_IMAGE         1
#define GRD_INDEXED_COLOR_IMAGE     2
#define GRD_TRUE_COLOR_IMAGE        3

#define GRD_POSITIVE_THICKNESS      1
#define GRD_NEGATIVE_THICKNESS      2

#define GRD_INSIDE_POLYGON          1
#define GRD_OUTSIDE_POLYGON         2

#define GRD_DEEPER_THAN_RULE        1
#define GRD_SHALLOWER_THAN_RULE     2
#define GRD_DEEPER_THAN_DETACHMANT_RULE        3
#define GRD_SHALLOWER_THAN_DETACHMENT_RULE     4


class CSWGrdAPI;


#include "csw/surfaceworks/private_include/grd_arith.h"
#include "csw/surfaceworks/private_include/grd_calc.h"
#include "csw/surfaceworks/private_include/grd_constraint.h"
#include "csw/surfaceworks/private_include/grd_ctog.h"
#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_fileio.h"
#include "csw/surfaceworks/private_include/grd_image.h"
#include "csw/surfaceworks/private_include/grd_stats.h"
#include "csw/surfaceworks/private_include/grd_triangle_class.h"
#include "csw/surfaceworks/private_include/grd_tsurf.h"
#include "csw/surfaceworks/private_include/grd_utils.h"

#include "csw/utils/private_include/csw_fileio.h"


class CSWGrdAPI
{

  private:

    CSWGrdArith        grd_arith_obj;
    CSWGrdFault        grd_fault_obj;
    CSWGrdFileio       grd_fileio_obj;
    CSWGrdImage        grd_image_obj;
    CSWGrdStats        grd_stats_obj;
    CSWGrdTsurf        grd_tsurf_obj;
    CSWGrdUtils        grd_utils_obj;
    CSWGrdCtog         grd_ctog_obj;

    std::unique_ptr<CSWGrdTriangle>
        grd_triangle_obj {new CSWGrdTriangle()};
    std::unique_ptr<CSWGrdCalc>
        grd_calc_obj {new CSWGrdCalc ()};
    std::unique_ptr<CSWGrdConstraint>
        grd_constraint_obj {new CSWGrdConstraint ()};

    CSWFileioUtil      fileio_util;

// The sub objects here tend to use bits and pieces of each other.
// A sub object gets a pointer to any other sub object it uses.
// All of these sub objects and interdependencies are encapsulated
// into this CSWGridAPI object.  This is an excellent reason to
// always use CSWGrdAPI public methods only when writing application
// code.

    void SetObjPtrs () {

        grd_arith_obj.SetGrdFaultPtr (&grd_fault_obj);
        grd_arith_obj.SetGrdUtilsPtr (&grd_utils_obj);

        grd_calc_obj->SetGrdArithPtr (&grd_arith_obj);
        grd_calc_obj->SetGrdFaultPtr (&grd_fault_obj);
        grd_calc_obj->SetGrdFileioPtr (&grd_fileio_obj);
        grd_calc_obj->SetGrdStatsPtr (&grd_stats_obj);
        grd_calc_obj->SetGrdTrianglePtr (grd_triangle_obj.get());
        grd_calc_obj->SetGrdTsurfPtr (&grd_tsurf_obj);
        grd_calc_obj->SetGrdUtilsPtr (&grd_utils_obj);

        grd_constraint_obj->SetGrdFaultPtr (&grd_fault_obj);
        grd_constraint_obj->SetGrdFileioPtr (&grd_fileio_obj);
        grd_constraint_obj->SetGrdTrianglePtr (grd_triangle_obj.get());
        grd_constraint_obj->SetGrdUtilsPtr (&grd_utils_obj);

        grd_ctog_obj.SetGrdFaultPtr (&grd_fault_obj);
        grd_ctog_obj.SetGrdUtilsPtr (&grd_utils_obj);

        grd_fault_obj.SetGrdArithPtr (&grd_arith_obj);
        grd_fault_obj.SetGrdFileioPtr (&grd_fileio_obj);
        grd_fault_obj.SetGrdTrianglePtr (grd_triangle_obj.get());
        grd_fault_obj.SetGrdUtilsPtr (&grd_utils_obj);

        grd_fileio_obj.SetGrdFaultPtr (&grd_fault_obj);
        grd_fileio_obj.SetGrdUtilsPtr (&grd_utils_obj);

        grd_image_obj.SetGrdArithPtr (&grd_arith_obj);
        grd_image_obj.SetGrdFileioPtr (&grd_fileio_obj);
        grd_image_obj.SetGrdUtilsPtr (&grd_utils_obj);

        grd_stats_obj.SetGrdFaultPtr (&grd_fault_obj);
        grd_stats_obj.SetGrdUtilsPtr (&grd_utils_obj);

        grd_triangle_obj.get()->SetGrdArithPtr (&grd_arith_obj);
        grd_triangle_obj.get()->SetGrdCalcPtr (grd_calc_obj.get());
        grd_triangle_obj.get()->SetGrdConstraintPtr (grd_constraint_obj.get());
        grd_triangle_obj.get()->SetGrdFaultPtr (&grd_fault_obj);
        grd_triangle_obj.get()->SetGrdTsurfPtr (&grd_tsurf_obj);
        grd_triangle_obj.get()->SetGrdUtilsPtr (&grd_utils_obj);

        grd_tsurf_obj.SetGrdFileioPtr (&grd_fileio_obj);
        grd_tsurf_obj.SetGrdUtilsPtr (&grd_utils_obj);

        grd_utils_obj.SetGrdArithPtr (&grd_arith_obj);
        grd_utils_obj.SetGrdCalcPtr (grd_calc_obj.get());
        grd_utils_obj.SetGrdFaultPtr (&grd_fault_obj);
        grd_utils_obj.SetGrdTrianglePtr (grd_triangle_obj.get());

    };

    FILE   *LineFilePtr = NULL;

  public:

    CSWGrdAPI () {SetObjPtrs ();};

int do_nothing () {
printf ("\nCSWGrdAPI do nothing\n\n");
return 1;
}

    double grd_vector_angle (double x0, double y0, double z0,
                             double x1, double y1, double z1,
                             double x2, double y2, double z2) {
        double  dang =
        grd_utils_obj.grd_vector_angle (x0, y0, z0,
                                        x1, y1, z1,
                                        x2, y2, z2);
        return dang;
    };

    
    int grd_SetControlPoints (CSW_F*, CSW_F*, CSW_F*, CSW_F*, int*, int);
    int grd_SetControlPointStructs (GRidControlPoint*, int);
    int grd_SetControlSurface (CSW_F*, int, int, CSW_F, CSW_F, CSW_F, CSW_F, int);
    
    int grd_CalcPerpPlaneFit (double *x, double *y, double *z, int npts, double *coef);
    int grd_CalcTrendSurface (CSW_F*, CSW_F*, CSW_F*, int, int,
                              CSW_F*);
    int grd_EvalTrendSurface (CSW_F*, CSW_F*, CSW_F*, int, int,
                              CSW_F*);
    int grd_CalcTrendGrid (CSW_F*, CSW_F*, CSW_F*, int, int,
                           CSW_F*, int, int,
                           CSW_F, CSW_F, CSW_F, CSW_F);
    int grd_CalcTrendGridFromDouble
                          (double*, double*, double*, int, int,
                           CSW_F*, int, int,
                           double, double, double, double);
    int grd_SetConformableSurfaceFromDouble
                      (CSW_F *grid, int ncol, int nrow,
                       double x1, double y1, double x2, double y2);
    int grd_CalcGridFromDouble
                     (double*, double*, double*, CSW_F*, int,
                      CSW_F*, char*, char**, int, int,
                      double, double, double, double,
                      FAultLineStruct*, int,
                      GRidCalcOptions *);
    int grd_CalcGrid (CSW_F*, CSW_F*, CSW_F*, CSW_F*, int,
                      CSW_F*, char*, char**, int, int,
                      CSW_F, CSW_F, CSW_F, CSW_F,
                      FAultLineStruct*, int,
                      GRidCalcOptions *);
    void grd_SetNoisyDataFlag (int ndf);
    int grd_SetCalcOption (int, int, CSW_F);
    int grd_SetCalcOptions (GRidCalcOptions *);
    int grd_DefaultCalcOptions (GRidCalcOptions *);
    int grd_RecommendedSizeFromDouble
                            (double*, double*, int, int noisy_edge,
                             double*, double*, double*, double*,
                             int*, int*);
    int grd_RecommendedSize (CSW_F*, CSW_F*, int,
                             CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                             int*, int*);
    int grd_SetHardNullValues (CSW_F *, char *, int, int,
                               CSW_F, int, CSW_F**);
    int grd_SmoothGrid (CSW_F*, int, int, int,
                        FAultLineStruct*, int,
                        CSW_F, CSW_F, CSW_F, CSW_F,
                        CSW_F, CSW_F, CSW_F**);
    int grd_EdgeNuggetEffect (CSW_F *x, CSW_F *y, CSW_F *z, int npts,
                              CSW_F *global_zdelta, CSW_F *local_zdelta_avg);
    int grd_WriteFile (const char*, const char*,
                       CSW_F*, char*, char *mask2,
                       int, int,
                       double, double, double, double, int,
                       FAultLineStruct*, int);
    int grd_WriteTextFile (CSW_F *grid, int ncol, int nrow,
                           double x1, double y1, double x2, double y2,
                           char *fname);
    int grd_WriteFaultLines (FAultLineStruct *faults, int nfaults, const char *fname);
    int grd_ReadFile (const char*, char*,
                      CSW_F**, char**, char **mask2,
                      int*, int*,
                      double*, double*, double*, double*, int*,
                      FAultLineStruct**, int*);
    int grd_WriteMultipleFile (char*, GRidFileRec*, int);
    int grd_ReadMultipleFile (char*, GRidFileRec*, int);
    void grd_CleanFileRecList (GRidFileRec *list, int nlist);
    int grd_OneGridArith (CSW_F*, CSW_F*,
                          int, int, int, CSW_F,
                          CSW_F, void(*)(GRidArithData *), void*);
    int grd_TwoGridArithFromDouble
                         (CSW_F*, char*,
                          double, double, double, double,
                          int, int,
                          FAultLineStruct*, int,
                          CSW_F*, char*,
                          double, double, double, double,
                          int, int,
                          FAultLineStruct*, int,
                          CSW_F,
                          CSW_F**, char**,
                          double*, double*, double*, double*,
                          int*, int*, int, void(*)(GRidArithData *), void*);
    int grd_TwoGridArith (CSW_F*, char*,
                          CSW_F, CSW_F, CSW_F, CSW_F,
                          int, int,
                          FAultLineStruct*, int,
                          CSW_F*, char*,
                          CSW_F, CSW_F, CSW_F, CSW_F,
                          int, int,
                          FAultLineStruct*, int,
                          CSW_F,
                          CSW_F**, char**,
                          CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                          int*, int*, int, void(*)(GRidArithData *), void*);
    int grd_ResampleGrid (CSW_F*, char*, int, int,
                          CSW_F, CSW_F, CSW_F, CSW_F,
                          FAultLineStruct*, int,
                          CSW_F*, char*, int, int,
                          CSW_F, CSW_F, CSW_F, CSW_F,
                          int);
    int grd_ResampleGridFromDouble (CSW_F*, char*, int, int,
                          double, double, double, double,
                          FAultLineStruct*, int,
                          CSW_F*, char*, int, int,
                          double, double, double, double,
                          int);
    int grd_BackInterpolateFromDouble (CSW_F*, int, int,
                             double, double, double, double,
                             FAultLineStruct*, int,
                             double*, double*, double*, int,
                             int);
    int grd_BackInterpolate (CSW_F*, int, int,
                             CSW_F, CSW_F, CSW_F, CSW_F,
                             FAultLineStruct*, int,
                             CSW_F*, CSW_F*, CSW_F*, int,
                             int);
    int grd_HorizontalGradient (CSW_F*, int, int,
                                CSW_F, CSW_F, CSW_F, CSW_F,
                                FAultLineStruct*, int,
                                CSW_F*, CSW_F*);
    int grd_HorizontalGradientFromDouble
                               (CSW_F*, int, int,
                                double, double, double, double,
                                FAultLineStruct*, int,
                                CSW_F*, CSW_F*);
    int grd_Version (char *);
    int grd_GetErr (void);
    int grd_FillNullValues (CSW_F*, int, int,
                            CSW_F, CSW_F, CSW_F, CSW_F,
                            FAultLineStruct*, int,
                            CSW_F, CSW_F*, char*);
    int grd_XYZFromGrid (CSW_F*, int, int, CSW_F,
                         CSW_F, CSW_F, CSW_F, CSW_F,
                         CSW_F*, CSW_F*, CSW_F*, int*, int);
    int grd_XYZFromGridDouble (CSW_F*, int, int, CSW_F,
                               double, double, double, double,
                               double*, double*, double*, int*, int);
    
    int grd_TrianglesFromGrid (CSW_F*, int, int,
                               CSW_F, CSW_F, CSW_F, CSW_F,
                               POint3D**, int*,
                               TRiangleIndex**, int*);
    int grd_TrianglesFromGridDouble (CSW_F*, int, int,
                                     double, double, double, double,
                                     POint3D**, int*,
                                     TRiangleIndex**, int*);
    int grd_TrianglesFromIndex (POint3D*,
                                TRiangleIndex*, int,
                                TRiangle3D*);
    
    int grd_HexagonsFromGrid (CSW_F*, int, int,
                              CSW_F, CSW_F, CSW_F, CSW_F,
                              POint3D**, int*,
                              SIdeIndex**, int*,
                              HExagonIndex**, int*);
    int grd_HexagonsFromGridDouble
                               (CSW_F*, int, int,
                                double, double, double, double,
                                POint3D**, int*,
                                SIdeIndex**, int*,
                                HExagonIndex**, int*);
    int grd_HexagonsFromIndex (POint3D*,
                               SIdeIndex*,
                               HExagonIndex*, int,
                               HExagon3D*);
    int grd_FaultStructsToArrays (FAultLineStruct*, int, CSW_F**, CSW_F**, CSW_F**, int**, int**, int*);
    int grd_FaultArraysToStructs (CSW_F*, CSW_F*, CSW_F*, int*, int*, int, FAultLineStruct**, int*);
    int grd_DoubleFaultArraysToStructs (double*, double*, double*,
                                        int*, int*, int, FAultLineStruct**, int*);
    int grd_GetCurrentFaultStructs (FAultLineStruct**, int*);
    int grd_FreeFaultLineStructs (FAultLineStruct*, int);
    int grd_ReadFaultLineFile (char*, FAultLineStruct**, int*);
    int grd_CombineFaults (FAultLineStruct*, int, FAultLineStruct*, int,
                           FAultLineStruct**, int*);
    FAultLineStruct *grd_CopyFaultLineStructs (FAultLineStruct *in, int numin);
    
    void grd_FreeImageData(GRdImage *);
    GRdImage *grd_CreateClipMask
                          (double*, double*, int, int*, int*,
                           double, double, double, double,
                           int, int);
    GRdImage *grd_CreateImageGeometry
                          (double, double, double, double,
                           int, int);
    int grd_CreateImage(CSW_F*, int, int,
                        double, double, double, double,
                        GRdImage*, GRdImageOptions*,
                        FAultLineStruct*, int,
                        GRdImage*);
    
    int grd_FreePolygonStructs (POlygonStruct *polygons, int npolygons);
    
    int grd_CalcDoublePlane (double *x, double *y, double *z,
                             int npts, double *coef);
    int grd_CalcPreciseDoublePlane (double *x, double *y, double *z,
                                    int npts, double *coef);
    
    int grd_MaskWithPolygon (char *mask, int ncol, int nrow,
                             double xmin, double ymin,
                             double xmax, double ymax,
                             int flag,
                             double *xpin, double *ypin,
                             int npin, int *icin, int *ipin);
    
    int ply_BuildPolygonsFromTaggedLines
                               (double *xline, double *yline, void **tags,
                                int nline, int *line_pts,
                                double *xpout, double *ypout, void **tagout,
                                int *npolyout, int *nptsout,
                                int maxpts, int maxpoly);
    int ply_BuildPolygonsFromLines (double *xline, double *yline,
                                    int nline, int *line_pts,
                                    double *xpout, double *ypout,
                                    int *npolyout, int *nptsout,
                                    int maxpts, int maxpoly);
    int ply_NestHoles (double *xpoly, double *ypoly,
                       int npoly, int *polypoints,
                       double *xpout, double *ypout,
                       int *npout, int *icout, int *ipout,
                       int maxpoints, int maxcomps);
    
    int grd_DistanceToLine (double *x, double *y, int npts,
                            double xp, double yp, double *dist);
    int grd_RefineContourData (COntourDataStruct *clines,
                               int nclines, int spline_flag,
                               double spacing,
                               COntourDataStruct **clout);
    void grd_CleanupContourData (COntourDataStruct *clist,
                                int nlist);
    int grd_ContourToGrid (COntourDataStruct *clinesin,
                           int nclines,
                           double *xin, double *yin, double *zin,
                           int nin,
                           double x1, double y1, double x2, double y2,
                           int ncol, int nrow,
                           CSW_F *grid, char *mask,
                           GRidCalcOptions *options);
    
    int grd_DumpGridValues (
        char *fname,
        CSW_F   *grid,
        int     ncol,
        int     nrow);
    
    int grd_WriteXYZGridFile (
        const char *fname,
        CSW_F   *grid,
        int     ncol,
        int     nrow,
        int     ncoarse,
        double xmin, double ymin, double xmax, double ymax);
    
    int grd_VariableDecimateLine (
        double         *x,
        double         *y,
        double         *z,
        int            npts,
        double         xmin,
        double         ymin,
        double         zmin,
        double         xmax,
        double         ymax,
        double         zmax,
        double         avspace,
        double         maxspace,
        double         maxrdist,
        double         *xout,
        double         *yout,
        double         *zout,
        int            *nout,
        int            maxout);
    
    int grd_VariableDecimateGrid (
            CSW_F          *grid,
            int            ncol,
            int            nrow,
            double         gxmin,
            double         gymin,
            double         gxmax,
            double         gymax,
            double         x1,
            double         y1,
            double         z1,
            double         x2,
            double         y2,
            double         z2,
            double         avspace,
            double         maxspace,
            double         maxrdist,
            double         *xout,
            double         *yout,
            double         *zout,
            int            *nout,
            int            maxout);
    
        int grd_CloseToSameLineXY (
            double         *xa1,
            double         *ya1,
            int            npts1,
            double         *xa2,
            double         *ya2,
            int            na2,
            double         dcrit);
    
        int grd_ChewUpTriangles (
            int            startnode,
            NOdeStruct     *nodes,
            int            *num_nodes,
            EDgeStruct     *edges,
            int            *num_edges,
            TRiangleStruct *tris,
            int            *num_tris);
    
        int grd_FilterGridSpikes (
            CSW_F    *grid,
            int      ncol,
            int      nrow,
            int      sfact);
    
        int grd_ResampleXYZLine (
            double           *x,
            double           *y,
            double           *z,
            int              npts,
            double           avspace,
            double           *xout,
            double           *yout,
            double           *zout,
            int              *nout,
            int              maxout);
    
    
    // functions originally in grd_triangle.h, but the implementation is
    // in grd_api.cc, so in order to "classify" them, they first are moved 
    // here to grid_api.h
    
    
        int grd_Triangulate  (double *xpts, double *ypts, double *zpts, int npts,
                              double *xlines, double *ylines, double *zlines,
                              int *linepts, int *linetypes, int nlines,
                              NOdeStruct **nodes_out, EDgeStruct **edges_out,
                              TRiangleStruct ** triangles_out,
                              int *num_nodes_out, int *num_edges_out,
                              int *num_tris_out);
    
        int grd_RemoveNodesFromTriMesh (NOdeStruct **nodelist, int *numnodes,
                                        EDgeStruct **edgelist, int *numedges,
                                        TRiangleStruct **trilist, int *numtriangles,
                                        int *nodes_to_remove, int num_nodes_to_remove);
    
        int grd_CalcTriMeshFromGrid (CSW_F*, int, int,
                                     double, double, double, double,
                                     double*, double*, double*,
                                     int*, int*, int,
                                     int,
                                     NOdeStruct**, EDgeStruct**, TRiangleStruct**,
                                     int*, int*, int*);
    
        int grd_CalcFaultedTriMeshFromGrid
                                    (CSW_F*, int, int,
                                     double, double, double, double,
                                     double*, double*, double*,
                                     int*, int*, int,
                                     int,
                                     NOdeStruct**, EDgeStruct**, TRiangleStruct**,
                                     int*, int*, int*);
    
        int grd_TriMeshFromGridPoints (
            double *x, double *y, double *z, int npts,
            double *xlines, double *ylines, double *zlines,
            int *line_points, int *line_types, int nlines,
            CSW_F *grid, int ncol, int nrow,
            double xmin, double ymin, double xmax, double ymax,
            NOdeStruct **nodes, int *num_nodes,
            EDgeStruct **edges, int *num_edges,
            TRiangleStruct **triangles, int *num_triangles);
    
        int grd_CalcTriMeshFromPointerGrid
                                    (void**, int, int,
                                     double, double, double, double,
                                     double*, double*, double*,
                                     int*, int*, int,
                                     int,
                                     NOdeStruct**, EDgeStruct**, TRiangleStruct**,
                                     int*, int*, int*);
    
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
    
        int grd_RemoveNullsFromTriMesh (NOdeStruct*, int*,
                                        EDgeStruct*, int*,
                                        TRiangleStruct *, int*,
                                        double, int);
    
        int grd_CalcGridFromTriMesh (NOdeStruct*, int,
                                     EDgeStruct*, int,
                                     TRiangleStruct*, int,
                                     double,
                                     CSW_F*, int, int,
                                     double, double, double, double);
    
        int grd_CalcGridFromTriMeshNodes
                                    (NOdeStruct* nodes, int num_nodes,
                                     CSW_F* grid, int ncol, int nrow,
                                     double xmin, double ymin,
                                     double xmax, double ymax);
    
        int grd_CalcTriMeshBoundingBox (NOdeStruct*, int,
                                        EDgeStruct*, int,
                                        TRiangleStruct*, int,
                                        double*, double*, double*, double*,
                                        double*, double*);
    
        int grd_GetBugLocations (double *bx, double *by, int *nbugs, int maxbugs);
    
        int grd_CalculateDrainagePolygons
               (CSW_F *grid, int ncol, int nrow,
                double x1, double y1, double x2, double y2,
                int future_flag,
                SPillpointStruct *spill_filter_list,
                int num_spill_filters,
                DRainagesStruct **drainages);
    
        int grd_DeleteEdgesFromTriMesh
                                   (NOdeStruct *nodes, int numnodes,
                                    EDgeStruct *edges, int numedges,
                                    TRiangleStruct *triangles, int numtriangles,
                                    int *edgenumlist, int nlist,
                                    int swapflag);
        int grd_DeleteTrianglesFromTriMesh
                                   (NOdeStruct *nodes, int numnodes,
                                    EDgeStruct *edges, int numedges,
                                    TRiangleStruct *triangles, int numtriangles,
                                    int *trianglenumlist, int nlist);
        int grd_CleanupTriMesh (NOdeStruct *nodes, int *numnodesptr,
                                EDgeStruct *edges, int *numedgesptr,
                                TRiangleStruct *triangles, int *numtrianglesptr);
        int grd_CalcTriMeshDips (NOdeStruct *nodes, int numnodes,
                                 EDgeStruct *edges, int numedges,
                                 TRiangleStruct *triangles, int numtriangles,
                                 int degree_flag,
                                 double *direction, double *amplitude);
    
        int grd_FreeDrainagesStruct (DRainagesStruct *dptr);
    
        int grd_WritePoints (double *x,
                             double *y,
                             double *z,
                             int npts,
                             char *fname);
    
        int grd_WriteFloatPoints (CSW_F  *x,
                                  CSW_F  *y,
                                  CSW_F  *z,
                                  int npts,
                                  const char *fname);
    
        int grd_WriteTriMeshFile
                     (TRiangleStruct *triangles, int ntriangles,
                      EDgeStruct *edges, int nedges,
                      NOdeStruct *nodes, int numnodes,
                      double xshift, double yshift,
                      const char *filename);
    
        int grd_ReadTriMeshFile
                     (char *filename,
                      int *vused,
                      double **vbase,
                      TRiangleStruct **triangles, int *ntriangles,
                      EDgeStruct **edges, int *nedges,
                      NOdeStruct **nodes, int *numnodes);
    
    		int grd_ReadTriMeshFromMultiFile
                 (char *filename, long position,
                  int *vused_out, double **vbase_out,
                  TRiangleStruct **triangles_out, int *ntriangles_out,
                  EDgeStruct **edges_out, int *nedges_out,
                  NOdeStruct **nodes_out, int *numnodes_out);
    
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
    
        int grd_WriteBinaryTriMeshFile
                     (int vused, double *vbase,
                      TRiangleStruct *triangles, int ntriangles,
                      EDgeStruct *edges, int nedges,
                      NOdeStruct *nodes, int numnodes,
                      char *filename);
    
        long grd_AppendBinaryTriMeshFile
                     (int vused, double *vbase,
                      TRiangleStruct *triangles, int ntriangles,
                      EDgeStruct *edges, int nedges,
                      NOdeStruct *nodes, int numnodes,
                      char *filename);
    
        int grd_WriteTriMeshFile2
                     (TRiangleStruct *triangles, int ntriangles,
                      EDgeStruct *edges, int nedges,
                      NOdeStruct *nodes, int numnodes,
                      double xshift, double yshift,
                      double *xb, double *yb, int nb,
                      const char *filename);
    
        int grd_GetNodesForTriangle
                     (TRiangleStruct *triangle,
                      EDgeStruct *edgelist,
                      int *n1, int *n2, int *n3);
    
        int grd_GetXYZForTriangle
                     (TRiangleStruct *tptr,
                      EDgeStruct *edgelist,
                      NOdeStruct *nodelist,
                      double *x, double *y, double *z);
    
        int grd_BackInterpolateTriMeshStruct
                     (TRimeshStruct *tmptr,
                      double *x,
                      double *y,
                      double *z,
                      int npts);
        int grd_BackInterpolateTriMesh
                     (TRiangleStruct *triangles, int ntriangles,
                      EDgeStruct *edges, int nedges,
                      NOdeStruct *nodes, int numnodes,
                      double *x,
                      double *y,
                      double *z,
                      int npts);
        int grd_InterpolateUnfaultedTriMesh (
            CSW_F   *grid, int ncol, int nrow,
            double xmin, double ymin, double xmax, double ymax,
            NOdeStruct *nodes, int num_nodes,
            int flag);
    
        int grd_TriMeshFromNodeTriangles (NOdeTriangleStruct *node_triangles,
                                          int num_node_triangles,
                                          NOdeStruct *nodes, int num_nodes,
                                          EDgeStruct **edges, int *numedges,
                                          TRiangleStruct **triangles, int *numtriangles);
    
        void grd_SetTriangleIndex (
            void      *index,
            double    xmin,
            double    ymin,
            double    xmax,
            double    ymax,
            int       ncol,
            int       nrow,
            double    xspace,
            double    yspace);
    
        void grd_GetTriangleIndex (
            void      **index,
            double    *xmin,
            double    *ymin,
            double    *xmax,
            double    *ymax,
            int       *ncol,
            int       *nrow,
            double    *xspace,
            double    *yspace);
    
        void grd_FreeTriangleIndex (
            void *index,
            int  ncells);
    
        int grd_DrapePointsOnTriMesh (int id1, int id2,
                                     NOdeStruct *nodes, int numnodes,
                                     EDgeStruct *edges, int numedges,
                                     TRiangleStruct *triangles, int numtriangles,
                                     double *xpts, double *ypts, int npts,
                                     double **xout, double **yout, double **zout,
                                     int *nout);
    
        int grd_DrapeLinesOnTriMesh (int id1, int id2,
                                     NOdeStruct *nodes, int numnodes,
                                     EDgeStruct *edges, int numedges,
                                     TRiangleStruct *triangles, int numtriangles,
                                     double *xlines, double *ylines,
                                     int *ilines, int nlines,
                                     double **xout, double **yout, double **zout,
                                     int **iout, int *nout);
    
        int grd_CalcHolesInTriMesh (NOdeStruct *nodes, int *num_nodes,
                                    EDgeStruct *edges, int *num_edges,
                                    TRiangleStruct *triangles, int *num_triangles,
                                    double zmin, double zmax);
    
        int grd_OutlineTriangles
                                 (NOdeStruct *nodes, int num_nodes,
                                  EDgeStruct *edges, int num_edges,
                                  TRiangleStruct *triangles, int num_triangles,
                                  double *xout, double *yout,
                                  int *npout, int *ncout, int *nvout,
                                  int maxpts, int maxcomp);
    
        int grd_CalcConvexHull (double seglen,
                                double *xpts, double *ypts, int npts,
                                double **xpoly, double **ypoly, int *npoly);
    
        int grd_OutlineTriMeshBoundary
                                 (NOdeStruct *nodes, int num_nodes,
                                  EDgeStruct *edges, int num_edges,
                                  TRiangleStruct *triangles, int num_triangles,
                                  double *xout, double *yout, double *zout,
                                  int *nodeout,
                                  int *npout, int *ncout, int *nvout,
                                  int maxpts, int maxcomp);
    
    
        int grd_ClipTriMeshToPolygon (NOdeStruct **nodes, int *num_nodes,
                                      EDgeStruct **edges, int *num_edges,
                                      TRiangleStruct **triangles, int *num_triangles,
                                      double *xpoly, double *ypoly,
                                      int *polypoints, int npoly, int flag);
    
        int grd_ClipTriMeshToMultiplePolygons
                                     (NOdeStruct *nodes, int *num_nodes,
                                      EDgeStruct *edges, int *num_edges,
                                      TRiangleStruct *triangles, int *num_triangles,
                                      double *xpoly, double *ypoly,
                                      int *polycomps, int *polypoints, int npoly,
                                      int flag);
    
        int grd_AddLinesToTriMesh
            (double *xlinesin, double *ylinesin, double *zlinesin,
             int *linepointsin, int *lineflagsin, int nlinesin,
             int exact_flag,
             NOdeStruct **nodes_out, EDgeStruct **edges_out,
             TRiangleStruct **triangles_out,
             int *num_nodes_out, int *num_edges_out, int *num_triangles_out);
    
        int grd_SetPolyConstraintFlag (int val);
    
        int grd_FilterDataSpikes (double *xin, double *yin, double *zin,
                                  int *ibad, int nin,
                                  SPikeFilterOptions *options);
    
        double grd_CalcAverageEdgeLength (
            NOdeStruct *nodes,
            EDgeStruct *edges,
            int        num_edges,
            int        short_long_flag);
    
        double grd_CalcAverageConstraintSegmentLength (
            double *xline,
            double *yline,
            int *npline,
            int nline);
    
        int grd_DecimateConstraint (
            double     *xline,
            double     *yline,
            double     *zline,
            int        *nline,
            double     decimation_distance);
    
        int grd_RemoveSliverTriangles (
            NOdeStruct         **nodes,
            EDgeStruct         **edges,
            TRiangleStruct     **triangles,
            int                *num_nodes,
            int                *num_edges,
            int                *num_triangles,
            double             edgr_length_ratio);
    
        int grd_RemoveTriIndex (int id1, int id2);
    
        void grd_FreeTriIndexList (void);
    
        int grd_WriteLines (
            double *x,
            double *y,
            double *z,
            int    np,
            int    *nc,
            int    *nv,
            char *fname);
        int grd_WriteFlaggedLines (
            double *x,
            double *y,
            double *z,
            int    np,
            int    *nc,
            int    *nv,
            int    *flags,
            char *fname);
        int grd_WriteFloatLines (
            CSW_F  *x,
            CSW_F  *y,
            CSW_F  *z,
            int    np,
            int    *nc,
            int    *nv,
            const char *fname);
    
        int grd_StartLineFile (char *name);
    
        void grd_EndLineFile (void);
    
        int grd_AddLineToFile (double *x,
                           double *y,
                           double *z,
                           int    np);
    
        int grd_InsertCrossingPoints (
            double *x1, double *y1, double *z1, int n1,
            double *x2, double *y2, double *z2, int n2,
            double *x3, double *y3, double *z3, int *tag3, int *n3,
            double *x4, double *y4, double *z4, int *tag4, int *n4,
            int maxout,
            double *xw, double *yw, double *zw, int *tagw, int maxwork);
    
        int grd_DecimateTaggedLine (
            double *x, double *y, double *z, int *tag, int *n);
    
        int grd_RemoveDeletedTriMeshElements (
            NOdeStruct      *nodes,
            int             *num_nodes,
            EDgeStruct      *edges,
            int             *num_edges,
            TRiangleStruct  *tris,
            int             *num_tris);
    
        int grd_ReshapeTriangles (
            NOdeStruct      *nodes,
            int             *num_nodes,
            EDgeStruct      *edges,
            int             *num_edges,
            TRiangleStruct  *tris,
            int             *num_tris);
    
        int grd_ResetTriMeshZValuesFromGrid (
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
            void           *faults,
            int            nfaults,
            int            interp_flag,
            int            *lineids
            );
    
        int grd_SmoothTriMeshNodes (
            NOdeStruct     *nodes,
            int            numnodes,
            EDgeStruct     *edges,
            int            numedges,
            TRiangleStruct *tris,
            int            numtris,
            int            sfact,
            double         avlen);
    
    
      private:
    
        int       WildInt = 100000000;
        int       ConstantGrid (CSW_F*, char*, int, int, CSW_F);
    
    /*
    double DistanceTo3DBox (
        double          x,
        double          y,
        double          z,
        double          x1,
        double          y1,
        double          z1,
        double          x2,
        double          y2,
        double          z2);
    double DistanceTo3DSphere (
        double          x,
        double          y,
        double          z,
        double          x1,
        double          y1,
        double          z1,
        double          x2,
        double          y2,
        double          z2);
    int CheckClosePoint (
        double          x,
        double          y,
        double          rdist,
        char            *flags,
        int             nc,
        int             nr,
        double          x1,
        double          y1,
        double          x2,
        double          y2);
    */
    
    
}; // end of main class
    
    
/*
    Add nothing to this file below the following endif
*/
#endif
