
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_calc.h

    Define the CSWGrdCalc class.  This class refactors the code in the
    old grd_apiP.h and grd_calc.c files.  An application should not use
    this class directly.  The CSWGridApi class (defined in csw/surfaceworks/
    include/grid_api.h header) should be used by an application.

*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grd_calc.h.
#endif


/*
    Add nothing above this line
*/
#ifndef GRD_CALC_H
#  define GRD_CALC_H

#  include "csw/surfaceworks/include/grd_shared_structs.h"


/*
    define some constants for the class
*/
#define ZPCT_BASE                .0025f
#define MAX_LOCAL                200
#define MAX_QUAD                 25
#define MAX_OCTANT               10
#define MAX_ITER                 4
#define MAX_ERROR_DIVISOR        500.0f
#define AVG_ERROR_DIVISOR        5000.0f
#define PROFILE_EXTENSION        5
#define MAX_REPORT_LINES         1000
#define SMALL_NUMBER_OF_POINTS   20
#define MAX_CONTROL              10000
#define MEDIAN_WIDTH             2
#define SAME_POINT_DIVISOR       20000.0f

#define MAX_PASS1                1000
#define MAX_WORK                 30000

#define MIN_ROWS_GCALC           2
#define MIN_COLS_GCALC           2

/*
    define structures used in the class methods
*/
typedef struct {
    CSW_F      *grid = NULL;
    int        ncol,
               nrow;
    CSW_F      x1,
               y1,
               x2,
               y2;
    int        type;
}  COntrolGridStruct;



class CSWGrdCalc;

#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_triangle_class.h"
#include "csw/surfaceworks/private_include/grd_fileio.h"
#include "csw/surfaceworks/private_include/grd_arith.h"
#include "csw/surfaceworks/private_include/grd_utils.h"
#include "csw/surfaceworks/private_include/grd_stats.h"
#include "csw/surfaceworks/private_include/grd_tsurf.h"

// define the main class

class CSWGrdCalc
{

  private:

    CSWGrdArith      *grd_arith_ptr = NULL;
    CSWGrdFault      *grd_fault_ptr = NULL;
    CSWGrdFileio     *grd_fileio_ptr = NULL;
    CSWGrdStats      *grd_stats_ptr = NULL;
    CSWGrdTriangle   *grd_triangle_ptr = NULL;
    CSWGrdTsurf      *grd_tsurf_ptr = NULL;
    CSWGrdUtils      *grd_utils_ptr = NULL;

    void __init (void) {
        ConformalGrid.grid = NULL;
        BaselapGrid.grid = NULL;
        TruncationGrid.grid = NULL;
    }


  public:

    CSWGrdCalc () {__init();};

    void  SetGrdArithPtr (CSWGrdArith *p) {grd_arith_ptr = p;};
    void  SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};
    void  SetGrdFileioPtr (CSWGrdFileio *p) {grd_fileio_ptr = p;};
    void  SetGrdStatsPtr (CSWGrdStats *p) {grd_stats_ptr = p;};
    void  SetGrdTrianglePtr (CSWGrdTriangle *p) {grd_triangle_ptr = p;};
    void  SetGrdTsurfPtr (CSWGrdTsurf *p) {grd_tsurf_ptr = p;};
    void  SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

  private:

/*
  Some work space used in local functions.  These were local static
  variables in the old c functions.
*/
    CSW_F     p_xloc[MAX_LOCAL*2],
              p_yloc[MAX_LOCAL*2],
              p_zloc[MAX_LOCAL*2],
              p_dsq[MAX_LOCAL],
              p_xp[MAX_LOCAL],
              p_yp[MAX_LOCAL],
              p_zp[MAX_LOCAL],
              p_ep[MAX_LOCAL],
              p_pdsq[MAX_LOCAL],
              p_work1[MAX_WORK],
              p_work2[MAX_WORK],
              p_work3[MAX_WORK];

/*
  Old static file variables become private class variables
*/
    double            PointNodeRatio {1.0};

    int               SparseFlag {0};
    CSW_F             FaultedErrorMax {1.e30f};

    CSW_F             OnFault {1.e32f};
    int               FaultFmt {0};

    GRidControlPoint  *ControlPoints {NULL};
    int               NumControlPoints {0},
                      NumControlAdjust {0};
    CSW_F             Xcontrol[MAX_CONTROL],
                      Ycontrol[MAX_CONTROL],
                      Zcontrol[MAX_CONTROL];
    int               NumFlatControl {0};
    int               UseControlInDataTable {0};

    CSW_F             Pass1X[MAX_PASS1];
    CSW_F             Pass1Y[MAX_PASS1];
    CSW_F             Pass1Z[MAX_PASS1];
    int               Npass1 {0};
    int               MaxPass1 {MAX_PASS1};

    CSW_F             *ConformableGrid = NULL;
    int               ConformableNcol = 0;
    int               ConformableNrow = 0;
    double            ConformableGXmin = 1.e30;
    double            ConformableGYmin = 1.e30;
    double            ConformableGXmax = -1.e30;
    double            ConformableGYmax = -1.e30;

    int ConformableFlag = 0;

    COntrolGridStruct ConformalGrid,
                      BaselapGrid,
                      TruncationGrid;
    CSW_F             *ShapeGrid {NULL},
                      *DifferenceGrid {NULL},
                      *ClippingGrid {NULL};

    int               ConformalFlag {0},
                      BaselapFlag {0},
                      TruncationFlag {0};
    int               UseShapeGrid {0};

    int               DefaultSizeMultiplier {1};

    int               WildInteger {99999999};
    CSW_F             WildFloat {1.e20f};

    int               *LinkList {NULL},
                      *DataTable {NULL},
                      *ClosestPoint {NULL},
                      *Left {NULL},
                      *Right {NULL};

    CSW_F             *Grid {NULL},
                      *Gwork1 {NULL},
                      *Gwork2 {NULL},
                      *GGwork1 {NULL},
                      *GGwork2 {NULL},
                      *Gwork3 {NULL},
                      *TrendGrid {NULL},
                      *EmptyAreaGrid {NULL};

    CSW_F             LowPlateau {-1.e30f};
    CSW_F             HighPlateau {1.e30f};

    int               *OrigIdata {NULL};
    int               *ExtraPoints {NULL};
    int               Nextra {0};
    CSW_F             *Xdata {NULL},
                      *Ydata {NULL},
                      *Zdata {NULL},
                      *OrigXdata {NULL},
                      *OrigYdata {NULL},
                      *OrigZdata {NULL},
                      *Zerr {NULL},
                      *ZerrBC {NULL};
    int               FreeZdataFlag {0};
    CSW_F             Xsmall[10],
                      Ysmall[10],
                      Zsmall[10];
    int               *Izdata {NULL};
    int               Ndata {0},
                      MaxData {0},
                      OrigNdata {0};

    CSW_F             Xmin {0.0}, Ymin {0.0}, Xmax {0.0}, Ymax {0.0},
                      Xspace {0.0}, Yspace {0.0},
                      DXmin {0.0}, DYmin {0.0}, DXmax {0.0}, DYmax {0.0},
                      OrigXmin {0.0}, OrigXmax {0.0}, OrigYmin {0.0}, OrigYmax {0.0};
    CSW_F             Xrange {0.0}, Yrange {0.0}, Zmin {0.0}, Zmax {0.0},
                      ZminusVal {0.0}, Zrange {0.0}, Ztiny {0.0},
                      Zmean {0.0}, Gmin {0.0}, Gmax {0.0};
    CSW_F             Rxmin {0.0}, Rymin {0.0}, Rxmax {0.0}, Rymax {0.0},
                      Rgxmin {0.0}, Rgymin {0.0}, Rgxmax {0.0}, Rgymax {0.0};
    int               Rmaxcol {0}, Rmincol {0}, Rmaxrow {0}, Rminrow {0};

    int               Ndivide {1};
    int               Extension {4};
    int               Ncol {0}, Nrow {0},
                      OrigNcol {0}, OrigNrow {0},
                      ReportNcol {0}, ReportNrow {0},
                      ColZero {0}, RowZero {0},
                      Ncoarse {0},
                      NcoarseOrig {0},
                      Nccol {0}, Ncrow {0};
    int               TrendDistance {0},
                      TrendSmoothFlag {0},
                      InLineFlag {0};
    CSW_F             FaultedCriticalDistance {1.e30f};

    char              *Icoarse {NULL},
                      *ZeroCoarse {NULL},
                      *NodeMask {NULL};
    CSW_F             *AnisoStrike {NULL},
                      *AnisoPower {NULL},
                      *AratioGrid {NULL};
    int               Firstq1 {0}, Firstq2 {0}, Firstq3 {0}, Firstq4 {0};

    CSW_F             SmoothingFactor {3.0f},
                      PreferredStrike {-1000.0f},
                      PointDensity {.1f},
                      BicubCutoff {20.0f};
    int               OutsideBoundaryMargin {3},
                      InsideBoundaryMargin {100};
    CSW_F             GridMaxValue {1.e30f},
                      GridMinValue {-1.e30f};
    CSW_F             HardMaxValue {1.e30f},
                      HardMinValue {-1.e30f};

    int               FastGridFlag {0};

    int               ReportLines {20};

    CSW_F             MaxError {0.0f},
                      AvgError {0.0f};

    CSW_F             OptPreferredStrike {-1000.0f};
    int               OptOutsideBoundaryMargin {3};
    int               OptInsideBoundaryMargin {100};
    CSW_F             OptGridMinValue {-1.e30f};
    CSW_F             OptGridMaxValue {1.e30f};
    CSW_F             OptHardMinValue {-1.e30f};
    CSW_F             OptHardMaxValue {1.e30f};
    int               OptReportLines {20};
    int               OptFastGridFlag {0};
    int               OptAnisotropyFlag {0},
                      OptDistancePower {2},
                      OptStrikePower {6},
                      OptNumLocalPoints {MAX_LOCAL},
                      OptLocalSearchPattern {GRD_QUADRANT_SEARCH},
                      OptThicknessFlag {0},
                      OptFaultedFlag {0},
                      OptTriangulateFlag {0},
                      OptDefaultSizeMultiplier {1},
                      OptWorkMargin {-1},
                      OptStepGridFlag {0};
    CSW_F             OptMaxSearchDistance {1.e30f};

    int               AnisotropyFlag {0},
                      StrikeSearch {1},
                      ErrorSearch {0},
                      DistancePower {2},
                      StrikePower {6},
                      NumLocalPoints {MAX_LOCAL},
                      LocalSearchPattern {GRD_QUADRANT_SEARCH},
                      ThicknessFlag {0},
                      OriginalThicknessFlag {0},
                      FaultedFlag {0},
                      TriangulateFlag {0},
                      VerticalFaultFlag {0},
                      NoTrendFlag {0},
                      EmptyRegionFlag {1},
                      WorkMargin {-1},
                      StepGridFlag {0};
    int               NoisyDataFlag = 0;
    CSW_F             MaxSearchDistance {1.e30f};
    int               MaxNodeDistance {100000};

    CSW_F             AnisotropyRatio {1.0f};

    CSW_F             TinyGraze {0.0f};
    CSW_F             TinySum {0.0f};

    int               FlatGridFlag {0};

    int               *ClosestFault {NULL};

    int               MedianFlag {0};

    CSW_F             ZlocMin {1.e30f},
                      ZlocMax {-1.e30f};

    CSW_F             CoincidentDistance {0.0};

    double            XOutputShift {0.0};
    double            YOutputShift {0.0};
    int               debug_output_flag {0};




/*
    Old static file functions become private class methods
*/
    int               FindDifferentZValue (CSW_F *x, CSW_F *y, CSW_F *z);

    int               SetupDataTable (int zflag, int first);
    int               SetupDistanceTable (void);
    int               CollectLocalPoints
                         (int i, int j, int start, int end,
                          int maxquad, int maxloc,
                          int *listout, int *nlist, int *nquad);
    int               ProcessLocalPoints
                         (int *list, int nlist, int nquad,
                          int irow, int jcol, int cp,
                          CSW_F *value);
    int               HalfPlaneSwitch
                         (int ido,
                          CSW_F *xloc, CSW_F *yloc,
                          CSW_F *zloc, CSW_F *dsq,
                          int nlist,
                          CSW_F *xp, CSW_F *yp, CSW_F *zp,
                          CSW_F *ep, CSW_F *pdsq, int *npout);
    int               CalcLocalNodes (void);
    int               AssignTrendNodes (void);
    int               FillInCoarse (void);
    int               FillInPerimeter (void);
    int               StrikeSmoothGrid (void);
    int               SimpleSmoothGrid (void);
    int               SmoothErrors (void);
    int               StrikeSmoothFaultedGrid (void);
    int               SimpleSmoothFaultedGrid (void);
    int               SmoothFaultedErrors (void);
/*
    int               StrikeSmoothFaultedErrors (void);
    int               StrikeSmoothErrors (void);
*/
    int               CalcErrorGrid (int iter);
    int               CalcErrorAtNode
                         (int irow, int jcol,
                          int *list, int nlist, CSW_F *error);
    int               CalcFaultedErrorAtNode
                         (int irow, int jcol,
                          int *list, int nlist, CSW_F *error);
    int               CheckForBicub (int irow, int jcol);
    int               MaskBadNodes (void);
    int               DivideRightBoundary (int start, int end);
    int               DivideLeftBoundary (int start, int end);
    int               IterateToFinalGrid (void);
    int               FillInStepGrid (void);
    int               SmoothStepGrid (void);
    int               StepGraze (CSW_F z1, CSW_F z2);
    int               DataPointErrors (CSW_F *maxerrptr, CSW_F *avgerrptr);
    int               FaultedDataPointErrors
                          (CSW_F *maxerrptr, CSW_F *avgerrptr);
    int               RefineGrid (void);
    int               RefineFaultedGrid (void);
    int               StrikeAverage
                          (CSW_F *x, CSW_F *y, CSW_F *z,
                           int n, int irow, int jcol,
                           CSW_F *val, CSW_F *stwgt);
    int               ExtendCoarseNodes (int iter, int nmin);
    int               ExtendFaultedCoarseNodes (int iter, int nmin);
    int               FitBestTrendSurface (void);
    int               ClipToMinMax (void);
    int               ClipOutputGridToMinMax (CSW_F *grid, int ncol, int nrow);
    int               GenerateReport (char **report);
    int               DetermineListFormat (int *lines, int nlines, char *fmt);
    int               CalcStrikeLine (int irow, int jcol,
                                      CSW_F *x1, CSW_F *y1,
                                      CSW_F *x2, CSW_F *y2, CSW_F *swgt);
    int               FilterPointsForStrike
                         (int *list1, int n1, int *list2, int *n2,
                          int nmax, int nquad, int irow, int jcol,
                          CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int               CalcLocalAnisotropy (void);

    int               FreeMem (void);
    int               TrendPointErrors (void);

    int               CheckClosePointFaulting
                         (int irow, int jcol, int nc);
    int               AdjustForZeros
                         (CSW_F *x, CSW_F *y, CSW_F *z, int *nlist);
    int               CheckForZeroThickness (void);

    int               AdjustForControlPoints (void);
    int               AdjustCornersForControl
                         (CSW_F x, CSW_F y, CSW_F zerr);

    int               CheckFlatControl
                         (CSW_F *x, CSW_F *y, CSW_F *z, int npts);

    int               BuildShapeGrid (void);
    int               ExtendUsingShapeGrid (void);
    int               AdjustForShapeGrid (void);

    int               MedianFilter (void);
    int               AdjustForErrors (void);

    void              AdjustForUnderflow (void);

    int               AddToExtraPoints (int i);

    void              AddDataPoint (double x, double y, double z);
    void              AddPointsToEmptyRegions (void);
    void              AddPass1Point (double x1, double y1, double z1,
                                     double x2, double y2, double z2,
                                     double dzstrike, double avspace);
    void              AddPass1PointForEdge
                                       (double x1, double y1, double z1,
                                        double x2, double y2, double z2,
                                        EDgeStruct *eptr,
                                        double dzstrike, double avspace);
    void              CalcEmptyAreaGrid (void);
    int               CheckForCoincidentPointInList (int irow, int jcol,
                                                     int *list, int nlist);
    int               CheckForNodePoints (CSW_F *x, CSW_F *y, int npts,
                                          double x1, double y1, double x2, double y2,
                                          int ncol, int nrow);

    void              MovingAverage (CSW_F *grid, int ncol, int nrow,
                                     int smfact, int lightflag);
    void              RemoveSpikes (CSW_F *grid, int ncol, int nrow,
                                     int smfact, int nc);



  public:

    void grd_set_noisy_data_flag (int ndf);

    int grd_set_conformable_surface_from_double
                          (CSW_F *grid, int ncol, int nrow,
                           double x1, double y1, double x2, double y2);

    int grd_WriteFaultLines (
        FAultLineStruct   *faults,
        int               nfaults,
        const char        *fname);

    void grd_set_output_shifts (double x, double y);;
    void grd_set_debug_output_flag (int ival);

    int grd_set_control_points (GRidControlPoint *list, int nlist);
    int grd_set_control_surface
       (CSW_F *grid, int ncol, int nrow,
        CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2, int type);

    int grd_calc_grid_from_double
       (CSW_F *x, CSW_F *y, CSW_F *z, CSW_F *err, int npts,
        CSW_F *in_grid, char *in_mask, char **report,
        int ncol, int nrow,
        CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
        FAultLineStruct *faults, int nfaults,
        GRidCalcOptions *options);

    int grd_calc_grid
       (CSW_F *x, CSW_F *y, CSW_F *z, CSW_F *err, int npts,
        CSW_F *in_grid, char *in_mask, char **report,
        int ncol, int nrow,
        CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
        GRidCalcOptions *options);
    int grd_set_calc_option (int tag, int ival, CSW_F fval);
    int grd_set_calc_options (GRidCalcOptions *options);
    int grd_default_calc_options (GRidCalcOptions *options);
    int grd_smooth_plateau_grid
                        (CSW_F *grid, int ncol, int nrow, int smfact,
                         FAultLineStruct *faults, int nfaults,
                         CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                         CSW_F minval, CSW_F maxval, CSW_F **smgrid,
                         CSW_F low, CSW_F high);
    int grd_smooth_grid (CSW_F *grid, int ncol, int nrow, int smfact,
                         FAultLineStruct *faults, int nfaults,
                         CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                         CSW_F minval, CSW_F maxval, CSW_F **smgrid);


    int grd_filter_grid_spikes (
        CSW_F      *grid,
        int        ncol,
        int        nrow,
        int        sfact);

};  // end of main class definition

/*
    Add nothing to this file below the following endif
*/
#endif


