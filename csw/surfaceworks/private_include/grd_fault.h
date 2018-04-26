
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_fault.h

    Define the CSWGrdFault class, which refactors the old grd_faults.c
    functions.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grd_fault.h.
#endif


/*
    Add nothing above this line
*/
#ifndef GRD_FAULT_H
#  define GRD_FAULT_H

#  include "csw/surfaceworks/include/grd_shared_structs.h"




/*
    define some constants for the class
*/
#define FAULTED_OUT_VALUE          1.e33f
#define MAX_GRAZED_POINTS          5000
#define OUTSIDE_GRID_FLAG          -999
#define INDEX_CHUNK                1000

#define DUPLICATE_FAULT            -113


/*
    define structures used only in this class
*/
typedef struct {
    CSW_F      x;
    int        n;
}  GRazedPoints;

typedef struct {
    CSW_F      x1,
               y1,
               x2,
               y2;
    int        endflag1,
               endflag2,
               id,
               type,
               flag;
    int        used;
}  FAultVector;

typedef struct {
    int        findex,
               next;
}  FAultIndexLink;

typedef struct {
    int        node,
               backnode,
               fault_num,
               flag,
               next;
    CSW_F      x,
               y,
               z;
}  CRossingElevation;

typedef struct {
    int        vecnum,
               endnum;
    CSW_F      x,
               y;
}  ENdPoint;


class CSWGrdFault;

#include "csw/surfaceworks/private_include/grd_triangle_class.h"
#include "csw/surfaceworks/private_include/grd_utils.h"
#include "csw/surfaceworks/private_include/grd_arith.h"
#include "csw/surfaceworks/private_include/grd_fileio.h"


class CSWGrdFault
{

  private:

    CSWGrdArith     *grd_arith_ptr = NULL;
    CSWGrdFileio    *grd_fileio_ptr = NULL;
    CSWGrdTriangle  *grd_triangle_ptr = NULL;
    CSWGrdUtils     *grd_utils_ptr = NULL;

  public:

    CSWGrdFault () {};

    void SetGrdArithPtr (CSWGrdArith *p) {grd_arith_ptr = p;};
    void SetGrdFileioPtr (CSWGrdFileio *p) {grd_fileio_ptr = p;};
    void SetGrdTrianglePtr (CSWGrdTriangle *p) {grd_triangle_ptr = p;};
    void SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

  private:

    int          p_i2000[2000];
    int          p_i2001[2000];

    double       p_d2000[2000];
    double       p_d2001[2000];
    double       p_d2002[2000];

    int          ntimes = 0;
    int          ncalls = 0;
    int          nmod = 10000;

/*
    Old static file variables become private class variables
*/
    FAultLineStruct   *OrigFaultLines = NULL;
    int               NumOrigFaultLines = 0;
    int               FixCrossingFlag = 1;
    int               *ElevIndex = NULL;
    int               *ElevIndex2 = NULL;
    int               *EndPointIndex = NULL;
    CRossingElevation **ElevList = NULL;
    int               NumElevList = 0,
                      MaxElevList = 0;
    int               NumEndPoints = 0,
                      MaxEndPoints = 0;
    ENdPoint          *EndPointList = NULL;

    GRazedPoints      GrazedPoints[MAX_GRAZED_POINTS];
    int               NumGrazedPoints = 0;
    FAultVector       *FaultVectors = NULL;
    int               NumFaultVectors = 0;
    int               MaxFaultVectors = 0;
    char              *FaultVectorsUsed = NULL;
    int               FaultVectorUsedValue = 0;
    FAultVector       *OriginalFaultVectors = NULL;
    int               NumOriginalFaultVectors = 0;

    FAultIndexLink    *IndexLink = NULL;
    int               NumIndexLink = 0,
                      MaxIndexLink = 0;
    int               *IndexGrid1 = NULL,
                      *IndexGrid2 = NULL;

    char              *ColumnCrossings = NULL,
                      *RowCrossings = NULL,
                      *CellCrossings = NULL,
                      *NodeGrazing = NULL;
    int               *ClosestFault = NULL;

    CSW_F             *Grid = NULL;
    int               Ncol = 0,
                      Nrow = 0;
    int               Ncoarse = 1;

    CSW_F             Xmin = 0.0f,
                      Ymin = 0.0f,
                      Xmax = 0.0f,
                      Ymax = 0.0f,
                      OrigXmin = 1.e30f,
                      OrigYmin = 1.e30f,
                      OrigXmax = -1.e30f,
                      OrigYmax = -1.e30f,
                      MarginXmin = 1.e30f,
                      MarginYmin = 1.e30f,
                      MarginXmax = -1.e30f,
                      MarginYmax = -1.e30f,
                      Tiny = 0.0f,
                      Tiny2 = 0.0f,
                      Xspace = 0.0f,
                      Yspace = 0.0f;

    CSW_F             EqualTiny = 0.0f,
                      ExtendTiny = 0.0f,
                      CellSize = 0.0f;
    double            NodeOffset = 0.0;

    int               EndFlag = 0,
                      ExtendEndPointFlag = 0;

    CSW_F             Zmin = 0.0f,
                      Zmax = 0.0f;
    CSW_F             NullValue = 1.e30f;
    CSW_F             NullTest = 1.e28f;

    int               NvContourFlag = 0;

    int               NullFillFlag = 1;

    double            XFaultShift = 0.0,
                      YFaultShift = 0.0;

    int               ResampleFlag = 0;
    CSW_F             NewXmin = 1.e30f,
                      NewYmin = 1.e30f,
                      NewXmax = -1.e30f,
                      NewYmax = -1.e30f,
                      NewXspace = -1.0f,
                      NewYspace = -1.0f;
    int               NewNcol = -1,
                      NewNrow = -1;

    int               FaultIndexExtent = 3;

    int               *VecNumWork = NULL;


    int               Ncheck1 = 0,
                      Ncheck2 = 0;
    int               Ncheck3 = 0,
                      Ncheck4 = 0;
    int               Ncheck5 = 0,
                      Ncheck6 = 0;
    int               Ncheck7 = 0,
                      Ncheck8 = 0;


/*
    Old static functions become private class functions. 
*/
    int AdjustProblemFaults (void);
    int BuildColumnIndex (void);
    int BuildRowIndex (void);
    int BuildCellIndex (void);
    int BuildDistanceTable (void);
    int SetColumnCrossingForVector
        (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2, int n);
    int SetRowCrossingForVector
        (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int SaveOriginalFaultVectors (void);
    int ClipFaultsToGrid (void);
    int GetVectors
        (int r1, int r2, int c1, int c2,
         int **vecnums, int *nvec);
    int FillInNulls (CSW_F *grid, int ncol, int nrow);
    int ExtendVector
        (CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2, CSW_F length);
    int ShortenVector
        (CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int SetupRowCrossingElevations (void);
    int SetupColumnCrossingElevations (void);
    int FreeCrossingElevations (void);
    int AddCrossingElevation (int kcell, CRossingElevation *eptr);
    int FillFromCrossings
        (int i1, int i2, int j1, int j2, CSW_F *bwork);
    int EvaluateCrossing (int node, int neighbor, CSW_F *value);
    int CheckForFaultGrazing (CSW_F x, CSW_F y);
    int CheckForFaultGrazing2 (double x, double y);
    int AdjustForFaultGrazing (CSW_F *xin, CSW_F *yin);
    int PerpOffset (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                    CSW_F xt, CSW_F yt, CSW_F distance,
                    CSW_F *xout, CSW_F *yout);
    int PerpOffset2 (double x1, double y1, double x2, double y2,
                     double xt, double yt, double distance,
                     double *xout, double *yout);
    int BuildEndPointIndex (void);
    int AddEndPoint (int k, CSW_F x, CSW_F y, int i, int endnum);
    int AverageNeighbors (int kcell, CSW_F x, CSW_F y, CSW_F *zout);
    int SeparateFaultVectors (void);
    int CopyFaultLinesToOrig (FAultLineStruct *faults, int nfaults);
    int CopyFaultLines (FAultLineStruct *faults, int nfaults,
                        FAultLineStruct *fout);
    int FaultClosed (FAultLineStruct *fptr);
    int UnionFaults (void);
    int WritePolygons (char *fname,
       double *xp1, double *yp1, int np1, int *nc1, int *nv1,
       double *xp2, double *yp2, int np2, int *nc2, int *nv2);

    int ExpandLevel (CSW_F *grid1, CSW_F *grid2, int ncol, int nrow,
                     CSW_F nval, int flag);
    int RemoveDuplicateFaults (FAultLineStruct *flist, int nlist,
                               FAultLineStruct **fout, int *nfout);
    int CheckDuplicateFaults (FAultLineStruct *f1, FAultLineStruct *f2);
    int AppendFaultVector (int ivec, int endflag1, int endflag2,
                           double x1, double y1, double x2, double y2);
    int ZigZagToAvoidNodes (void);
    int SplitFaultVector (int ivec, CSW_F xpin, CSW_F ypin);
    void AdjustFaultsForGridGeometry (CSW_F *xf, CSW_F *yf,
                                      int *npts, int nf);
    int InsideBox (CSW_F xt, CSW_F yt,
                   CSW_F xmin, CSW_F ymin,
                   CSW_F xmax, CSW_F ymax);

    int CheckIfLineIsExtendable (CSW_F *x, CSW_F *y, int npts,
                                 int *extend_first, int *extend_last);

    int ResampleConstraintLines (
        double        **xline_io,
        double        **yline_io,
        double        **zline_io,
        int           *npline_io,
        int           *exact_flags,
        int           nline,
        double        avspace);

    int FixCrossingFaults (void);
    void FreeDoubleArrays (double **alist, int nlist);

    void ShiftXY (CSW_F *x, CSW_F *y, int nlines, int *npts);





  public:

    int grd_define_fault_vectors (FAultLineStruct *faults, int nfaults);
    int grd_shift_fault_vectors (FAultLineStruct *faults, int nfaults,
                             double x, double y,
                             FAultLineStruct **fault_out, int *nfault_out);
    int grd_define_and_shift_fault_vectors
        (FAultLineStruct *faults, int nfaults,
         double x, double y);
    int grd_set_fault_zlimits (CSW_F z1, CSW_F z2);
    void grd_set_fault_index_extent (int extent);
    int grd_build_fault_indices
        (CSW_F *grid, int ncol, int nrow,
         CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int grd_extend_faults_to_margins (CSW_F orig_xmin,
                                      CSW_F orig_ymin,
                                      CSW_F orig_xmax,
                                      CSW_F orig_ymax,
                                      CSW_F xmin,
                                      CSW_F ymin,
                                      CSW_F xmax,
                                      CSW_F ymax);
    int grd_free_faults (void);
    int grd_free_fault_indices (void);
    int grd_check_fault_blocking
       (int irow, int jcol,
        CSW_F x2, CSW_F y2, CSW_F *weight);
    int grd_check_grid_fault_blocking 
       (int jcol, int irow, int jcol2, int irow2, CSW_F *weight);
    int grd_fault_check_needed (int, int, int);
    int grd_interp_in_faulted_area
        (CSW_F *x, CSW_F *y, CSW_F *z, int irow, int jcol,
         int nlist, CSW_F *grid, int nc);
    int grd_get_closest_fault_ptr (int **ptr);

    int grd_print_checks (void);
    int grd_faulted_bicub_interp_2
        (CSW_F *grid, int ncol, int nrow, CSW_F nullval, int kcell,
         CSW_F *x, CSW_F *y, CSW_F *z, int npts);
    int grd_faulted_bilin_interp_2
        (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
         CSW_F *x, CSW_F *y, CSW_F *z, int npts);
    int grd_faulted_bicub_interp
        (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
         CSW_F xref, CSW_F yref, int irow, int jcol,
         CSW_F *x, CSW_F *y, CSW_F *z, int npts);
    int grd_faulted_bilin_interp
        (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
         CSW_F xref, CSW_F yref, int irow, int jcol,
         CSW_F *x, CSW_F *y, CSW_F *z, int npts);
    int con_faulted_bicub_interp_2
        (CSW_F *grid, int ncol, int nrow, CSW_F nullval, int kcell,
         CSW_F *x, CSW_F *y, CSW_F *z, int npts);
    int con_faulted_bilin_interp_2
        (CSW_F *grid, int ncol, int nrow, CSW_F nullval, int kcell,
         CSW_F *x, CSW_F *y, CSW_F *z, int npts);
    int con_faulted_bicub_interp
        (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
         CSW_F xref, CSW_F yref, int irow, int jcol,
         CSW_F *x, CSW_F *y, CSW_F *z, int npts);
    int con_faulted_bilin_interp
        (CSW_F *grid, int ncol, int nrow, CSW_F nullval,
         CSW_F xref, CSW_F yref, int irow, int jcol,
         CSW_F *x, CSW_F *y, CSW_F *z, int npts);
    int grd_set_fault_data_grazing
        (CSW_F *x, CSW_F *y, int npts);
    int grd_reset_fault_data_grazing
        (CSW_F *x, int **list, int *nlist);

    int con_find_fault_intersection
        (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
         int kcell, int flag,
         CSW_F *xint, CSW_F *yint);
    int con_get_fault_cell_crossings
        (char **cell, char **col, char **row,
         int **closest, char **nodegraze);
    int grd_set_faulted_contour_level (CSW_F value);
    int con_faulted_smoothing (int level);
    int grd_faulted_smoothing (CSW_F **sgrid, int level);
    int grd_resample_faulted_grid
        (CSW_F *gout, int ncout, int nrout,
         CSW_F x1out, CSW_F y1out, CSW_F x2out, CSW_F y2out,
         int flag, int nskipin);
    int grd_resample_faulted_mask
        (char *maskin, char *maskout, int ncout, int nrout,
         CSW_F x1out, CSW_F y1out, CSW_F x2out, CSW_F y2out);
    int grd_back_interpolate_faulted
        (CSW_F *grid, int ncol, int nrow,
         CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
         CSW_F *x, CSW_F *y, CSW_F *z,
         int npts, int flag);
    int grd_set_fault_end_flag (int val);
    int grd_get_fault_end_point
        (int k, CSW_F *x, CSW_F *y);

    int con_correct_narrow_grid
        (CSW_F **newgrid, int *newncol, int *newnrow);

    int grd_get_current_fault_structs
        (FAultLineStruct **fault_lines, int *num_fault_lines);
    int grd_free_fault_line_structs
        (FAultLineStruct *flist, int nlist);
    int grd_fault_structs_to_arrays
        (FAultLineStruct *fault_list, int nlist,
         CSW_F **xfault, CSW_F **yfault, CSW_F **zfault,
         int **fault_points, int **linetypes, int *nfault);
    int grd_fault_arrays_to_structs
        (CSW_F *xfault, CSW_F *yfault, CSW_F *zfault,
         int *fault_points, int *linetypes, int nfault,
         FAultLineStruct **fault_list, int *nlist);
    int grd_fault_structs_to_arrays_2
        (FAultLineStruct *fault_list, int nlist,
         double **xfault, double **yfault, double **zfault,
         int **fault_points, int **linetypes, int *nfault);
    int grd_fault_arrays_to_structs_2
        (double *xfault, double *yfault, double *zfault,
         int *fault_points, int *linetypes, int nfault,
         FAultLineStruct **fault_list, int *nlist);
    int con_null_inside_faults (void);
    int grd_set_fault_option
        (int tag, int ival, CSW_F fval, char *cval);
    int con_null_points_in_faults
        (CSW_F *xin, CSW_F *yin, CSW_F *zin, int npts);
    int con_mask_inside_faults (char *mask);

    int grd_read_fault_line_file
        (char *filename, FAultLineStruct **faults, int *nfaults);

    int grd_fill_faulted_nulls (CSW_F nullval);

    int grd_combine_faults
        (FAultLineStruct *faults1, int nfaults1,
         FAultLineStruct *faults2, int nfaults2,
         FAultLineStruct **faultsout, int *nfaultsout);

    int grd_faulted_hgrad (CSW_F *slope_grid,
                           CSW_F *direction_grid);

    FAultLineStruct *grd_copy_fault_line_structs
        (FAultLineStruct *in, int n);

    void grd_set_grazing_grid_nodes (void);
    void grd_set_grazing_trimesh_nodes
        (NOdeStruct *nodes, int numnodes);
    int grd_set_fault_resample_null_fill_flag (int value);
    int grd_check_for_fault_grazing (double xt, double yt);

    int grd_get_current_fault_vectors (double **xline,
                                       double **yline,
                                       double **zline,
                                       int **iline,
                                       int *nline);

    int grd_get_current_fault_lines (double **xline,
                                     double **yline,
                                     double **zline,
                                     int **iline,
                                     int *nline,
                                     double avspace);

    int grd_set_fault_resample_geometry (int flag,
                                         CSW_F x1, CSW_F y1,
                                         CSW_F x2, CSW_F y2,
                                         int nc, int nr);

    void grd_set_fix_fault_crossing_flag (int ival);

    void grd_set_debug_color_val (double val);

};  // end of main class definition

/*
    Add nothing to this file below the following endif
*/
#endif
