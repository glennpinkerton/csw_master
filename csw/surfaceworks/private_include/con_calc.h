
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    con_calc.h

    This header has the class definition for the new CSWConCalc
    class.  This class replaces the functionality in the old 
    con_calc.c file.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file con_calc.h.
#endif


/*
    Add nothing above this ifndef
*/
#ifndef CON_CALC_H
#define CON_CALC_H

#include "csw/surfaceworks/include/con_shared_structs.h"


/*
    define constants for the class
*/
#define DEBUG_WRITE_GRID_FILE 0

#define LINE_BUFFER_CHUNK      2000
#define MAX_CONTOUR_LEVELS     2000
#define SMOOTH_SIZE            .04f
#define SMOOTH_SIZE2           .05f
#define MAX_NODES              500000000

#define MYSIGNED
#define SMOOTH_MARGIN          4
#define MIN_LOG_BASE           1.01f

#define MAX_HISTO              1000

#define MAX_SUB_GRID_SIZE      200

#define Z_TINY_DIVISOR         50000.0f

#define TINY_PERCENT           0.01f
#define BIG_PERCENT            0.99f

#define MIN_COLS_CON_CALC 2
#define MIN_ROWS_CON_CALC 2

#define MAX_LOOKUP        10000
#define MAX_BANDS         200
#define MAX_DETAIL        50
#define FILL_CHUNK        1000
#define NO_COLOR_FLAG     -2000000000

typedef int               COnColor;


/*
    define structures for the class
*/
typedef struct {
    CSW_F      z;
    char       major;
}  COntourVal;


class CSWConCalc;

#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_arith.h"
#include "csw/surfaceworks/private_include/grd_utils.h"


class CSWConCalc
{

  private:

    CSWGrdFault     *grd_fault_ptr = NULL;
    CSWGrdArith     *grd_arith_ptr = NULL;
    CSWGrdUtils     *grd_utils_ptr = NULL;

  public:

    void SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};
    void SetGrdArithPtr (CSWGrdArith *p) {grd_arith_ptr = p;};
    void SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

  private:

// private variables

    int           first {1};

    int           WildInt {1000000000};
    CSW_F         WildFloat {1.e20};

    CSW_F         Xmin {0.0},
                  Ymin {0.0},
                  Xmax {0.0},
                  Ymax {0.0},
                  Xspace {0.0},
                  Yspace {0.0},
                  Scale {-1.0};

    double        GridShift {0.0};
    CSW_F         Zmin {0.0},
                  Zmax {0.0},
                  HistoZmin {0.0},
                  HistoZmax {0.0},
                  Zt2 {0.0},
                  *Grid {NULL},
                  *NoNullGrid {NULL},
                  *OriginalGrid {NULL};

    int           Ncol {0},
                  Nrow {0};

    int           OptContourSmoothing {3},
                  OptContourResampleFlag {1},
                  OptContourThicknessFlag {0},
                  OptContourFastFlag {0},
                  OptStepGridFlag {0},
                  OptFaultedFlag {0},
                  OptContourInFaultsFlag {0},
                  OptContourLogConvert {0};
    CSW_F         OptContourBaseValue {0.0},
                  OptContourLogBase {0.0},
                  OptContourNullValue {1.e28},
                  OptContourMinorCrowd {0.0},
                  OptContourMajorCrowd {0.0},
                  OptFillPrecision {.02};

    int           ContourSmoothing {3},
                  FaultSmoothing {3},
                  DoSmoothing {0},
                  ContourResampleFlag {1},
                  ContourThicknessFlag {0},
                  ThicknessFlagIsSet {0},
                  SmoothMargin {2},
                  ContourFastFlag {0},
                  ContourLogConvert {0},
                  NumMajor {0},
                  NumMinor {0},
                  DoLogContours {0},
                  StepGridFlag {0},
                  FaultedFlag {0},
                  ContourInFaultsFlag {0},
                  MajorSpacing {0};
    int           ZeroFillColor {-1};
    int           TempZeroFillColor {-1};
    CSW_F         ContourInterval {-1.0},
                  ContourBaseValue {0.0},
                  BaseValueForContour {0.0},
                  ContourLogBase {0.0},
                  ContourNullValue {1.e28},
                  ContourMinorCrowd {0.0},
                  ContourMajorCrowd {0.0},
                  MinorContours[MAX_CONTOUR_LEVELS],
                  MajorContours[MAX_CONTOUR_LEVELS],
                  BaseGridValue {-1.e30},
                  TopGridValue {1.e30},
                  FirstContour {0.0},
                  LastContour {0.0};
    CSW_F         SubGridSize {50.0},
                  FillPrecision {.02};

    int           *BottomSide {NULL},
                  *TopSide {NULL},
                  *LeftSide {NULL},
                  *RightSide {NULL};

    MYSIGNED char *Hcrossing {NULL},
                  *Vcrossing {NULL},
                  *Hcrossing2 {NULL},
                  *Vcrossing2 {NULL},
                  *SmoothFlags {NULL};

    CSW_F         **SmoothSubgrids {NULL};
    int           SubgridCols {0},
                  SubgridRows {0};
    CSW_F         BicubCutoff = {20.0};

    CSW_F         *Xbuf1 {NULL},
                  *Ybuf1 {NULL},
                  *Xbuf2 {NULL},
                  *Ybuf2 {NULL};
    int           *CellBuf {NULL};
    char          *CrowdBuf {NULL},
                  *CrowdWork {NULL},
                  *Dibuf1 {NULL};
    int           DiMajor {0};
    int           Nbuf1 {0},
                  MaxBuf1 {0},
                  Nbuf2 {0},
                  MaxBuf2 {0},
                  MiddleStart {0};


    COntourVal    Cvals[2 * MAX_CONTOUR_LEVELS];
    int           Ncvals {0};

    COntourOutputRec    *ContourData {NULL};
    int           NconData {0},
                  MaxConData {0};

    CSW_F         OutputZlev {0.0};
    int           OutputMajor {0},
                  OutputDownhill {0},
                  OutputClosure {0};

    int           MajorCrowd {0},
                  MinorCrowd {0};

    CSW_F         MinorGradient {-1.0},
                  MajorGradient {-1.0};

    char          *FaultCellCrossings {NULL},
                  *FaultColumnCrossings {NULL},
                  *FaultRowCrossings {NULL},
                  *FaultNodeGraze {NULL};
    int           *ClosestFault {NULL};

    CSW_F         DefaultNumberOfContours {20.0};

    CSW_F         XCellMin {0.0},
                  YCellMin {0.0},
                  XCellMax {0.0},
                  YCellMax {0.0};

    int           RangeCheckNeeded {1};


   COnColor       ColorLookup[MAX_LOOKUP];

   CSW_F          ColorBandLow[MAX_BANDS],
                  ColorBandHigh[MAX_BANDS];
   int            ColorBandColor[MAX_BANDS];
   int            NColorBands = 0;

   CSW_F          ColorZmin,
                  ColorZdelta;

   COntourFillRec *FillPolys = NULL;
   int            NFillPolys = 0,
                  MaxFillPolys = 0;

   int            SubCols = 5,
                  SubRows = 5;

   CSW_F          *SubGrid = NULL;
   COnColor       *SubCband = NULL;
   int            FirstDetail = 1;




/*
    private methods
*/

    int          FindColorBand (CSW_F zval);
    int          SearchRawColorBands (CSW_F zval);
    int          OutputContourFill (CSW_F *x, CSW_F *y,
                                          int npts, int index, int rflag);
    int          DoDetailColor (int irow, int jcol, COnColor *cband);
    int          QuickDetail (int irow, int jcol, COnColor *cband);


    int          AdjustGridForContourLevel (CSW_F zlev);
    int          AdjustGridForContourInterval (void);
    int          AdjustSubgrid (CSW_F *grid, int n, CSW_F zlev);
    int          SetToPositiveNulls (void);
    int          TraceContours (void);
    int          TraceCellToCell
                    (int irow, int jcol, int side, CSW_F zlev);
    int          SmoothContour (void);
    int          OutputContours (void);
    int          PutInOutputStruct (CSW_F *xptr, CSW_F *yptr, int len);
    int          SamePoint
                     (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2, CSW_F tiny);
    int          TooCrowded (int kcell);
    int          SetSmoothFlags (void);
    int          CheckForBicub (int irow, int jcol);
    int          SmoothCell (int node, int side1, int side2,
                             CSW_F xend, CSW_F yend);
    int          TraceSubgrid (int irow, int jcol, int side,
                               CSW_F zlev, CSW_F xend, CSW_F yend,
                               CSW_F *grid, int nrow, int ncol,
                               CSW_F xspace, CSW_F yspace,
                               CSW_F xmin, CSW_F ymin);
    int          AppendToSmooth (CSW_F x, CSW_F y);
    int          ConvertToLogBase (CSW_F *grid);

    int          FormatNumber (CSW_F value, CSW_F logbase, char *buf);
    void         CleanupZeros (char *buf);
    int          FreeMem (void);
    int          StepGraze (CSW_F z1, CSW_F z2);
    int          GetClosestPointValue (CSW_F *x, CSW_F *y, CSW_F *z,
                                       int n, CSW_F *val);
    int          MoreThanTwoSteps (int irow, int jcol);
    int          CheckGridRange (CSW_F *vlist, int npts, int *nflag);
    int          FindSimpleGridLimits (CSW_F *grid, int ndata);
    int          FindHistoGridLimits (CSW_F *grid, int ndata);

    int          ShiftInputGrid (CSW_F *grid, int ncol, int nrow);

    int          SaddleCheck (CSW_F z1, CSW_F z2, CSW_F z3, CSW_F z4);
    int          SaddleTweak (int kcell, int *di, CSW_F zlev);

    int          CheckContourOrientation (void);
    int          CheckDoubleContourOrientation (void);
    int          CheckSmoothedContourOrientation (void);
    int          CheckSmoothedDoubleContourOrientation (void);
    void         RemoveNullBorder (CSW_F *grid,
                                   int *ncol, int *nrow,
                                   CSW_F *xmin, CSW_F *ymin,
                                   CSW_F *xmax, CSW_F *ymax,
                                   CSW_F nullvalue,
                                   CSW_F **gridout);
    void         CorrectResampledThickness
                     (CSW_F *grid, int ncol, int nrow, int nskip);

    int          BuildContourArrays (void);

  public:

    int con_set_calc_option (int tag, int ival, CSW_F fval);
    int con_set_calc_options (COntourCalcOptions *options);
    int con_default_calc_options (COntourCalcOptions *options);
    int con_set_contour_intervals (CSW_F contour_interval,
                                   int major_spacing,
                                   CSW_F first_contour,
                                   CSW_F last_contour,
                                   CSW_F *minor_contours, int nminor,
                                   CSW_F *major_contours, int nmajor);
    int con_calc_contours (CSW_F *rawgridinput, int ncol, int nrow,
                           CSW_F x1, CSW_F y1,
                           CSW_F x2, CSW_F y2, CSW_F scale,
                           COntourOutputRec **contours, int *ncont,
                           COntourCalcOptions *options);
    int con_free_contours (COntourOutputRec *list, int nlist);

    int con_build_color_bands (CSW_F cint,
                               CSW_F first, CSW_F last,
                               int maxband,
                               CSW_F *low, CSW_F *high,
                               int *nband);
    int con_set_color_bands (CSW_F *lowlist, CSW_F *highlist,
                             int *colorlist, int nlist);
    int con_set_zero_fill_color (int color);
    int con_calc_color_fills (CSW_F *gridin, int ncol, int nrow,
                              CSW_F xmin, CSW_F ymin,
                              CSW_F xmax, CSW_F ymax,
                              CSW_F scale,
                              COntourFillRec **fills, int *nfills,
                              COntourCalcOptions *options);
    int con_free_color_fills (COntourFillRec *list, int nlist);

    int con_resample_for_smoothing
            (CSW_F *grid, char *mask, int ncol, int nrow,
             CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
             CSW_F **gridout, char **maskout,
             int *ncolout, int *nrowout);

    int con_auto_grid_limits (CSW_F *grid, int ncol, int nrow,
                              CSW_F *zmin, CSW_F *zmax);

    int con_format_number (CSW_F value, CSW_F logbase, char *buf);
    int con_calc_default_contour_interval (CSW_F *zlist, int nlist,
                                           CSW_F *conint,
                                           CSW_F *conmin, CSW_F *conmax,
                                           int *major);
    int con_clip_contours_to_polygon
                               (int inside_outside_flag,
                                COntourOutputRec *contours, int ncontours,
                                double *xpoly, double *ypoly, int *ipoly, int npoly,
                                COntourOutputRec **conclip, int *nconclip);

}; // end of main class definition



/*
    End of the header file, add nothing below this line.
*/
#endif
