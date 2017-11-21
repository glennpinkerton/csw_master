
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    con_triangle_class.h

    Define the CSWConTriangle class, which replaces the functions in the 
    old con_triangle.c file.
*/

#ifndef CON_TRIANGLE_CLASS_H
#  define CON_TRIANGLE_CLASS_H

#include "csw/surfaceworks/include/con_shared_structs.h"


/*
  Define constants needed for the file.
*/
#define TINY_DIVISOR        1000000.0
#define CONTOUR_LINE_CHUNK  10

#define MAXDIV   4

class CSWConTriangle;

#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_utils.h"
#include "csw/surfaceworks/private_include/grd_fileio.h"
#include "csw/surfaceworks/private_include/grd_triangle_class.h"
#include "csw/surfaceworks/private_include/con_calc.h"

class CSWConTriangle
{

  private:

    CSWConCalc    *con_calc_ptr {NULL};
    CSWGrdFault   *grd_fault_ptr {NULL};
    CSWGrdUtils   *grd_utils_ptr {NULL};
    CSWGrdFileio  *grd_fileio_ptr {NULL};
    CSWGrdTriangle  *grd_triangle_ptr {NULL};

  public:

    CSWConTriangle () {};
    ~CSWConTriangle () {};

    void SetConCalcPtr (CSWConCalc *p) {con_calc_ptr = p;};
    void SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};
    void SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};
    void SetGrdFileioPtr (CSWGrdFileio *p) {grd_fileio_ptr = p;};
    void SetGrdTrianglePtr (CSWGrdTriangle *p) {grd_triangle_ptr = p;};

  private:

/*
  File static variables changed to private class variables.
*/
    TRiangleStruct      *TriangleList {NULL};
    EDgeStruct          *EdgeList {NULL};
    NOdeStruct          *NodeList {NULL};

    int                 NumTriangles {0};
    int                 NumEdges {0};
    int                 NumNodes {0};

    COntourOutputRec    *ContourLines {NULL};
    int                 NumContourLines {0};
    int                 MaxContourLines {0};

    double              *XconWork {NULL},
                        *YconWork {NULL};
    int                 NumConWork {0};
    double              *ZvalSave {NULL};
    double              ZvalAdjust {0.0};
    double              Zmin {0.0},
                        Zmax {0.0},
                        Ztiny {0.0};

    double              DeltaZCrit {0.0};

    double              ContourLevel {0.0};

    double              ContourMin {0.0},
                        ContourMax {0.0};
    double              NullValue {0.0};

    int                 MajorFlag {0};
    int                 MajorInterval {5};
    int                 DownhillFlag {0};
    int                 ClosureFlag {0};

    int                 Nmajor {0},
                        Nminor {0};
    CSW_F               *MajorList {NULL},
                        *MinorList {NULL};
    double              ContourInterval {0.0},
                        ContourBase {0.0};
    double              LogBase {0.0};

    int                 TracingFaultFlag {0};

    double tnxNorm {0.0},
           tnyNorm {0.0},
           tnzNorm {0.0};

    int    *TriNums {NULL};
    double *xCenters {NULL};
    double *yCenters {NULL};
    double *zCenters {NULL};
    int    NumCenters {0};
    int    MaxCenters {0};


/*
  File static functions changed to class private methods
*/
    int EdgeZrange (EDgeStruct *e);
    int CalcContours (void);
    int TraceContourLevel (void);
    int TraceSingleContour (EDgeStruct *estart);
    EDgeStruct *FindExitEdge (EDgeStruct *enow, TRiangleStruct *tnow);
    void AddEdgePoint (EDgeStruct *enow);
    int PointLimits (void);
    int OutputContourLine (void);
    int AdjustNodeValues (void);
    void FreeMem (void);
    void SetDownhill (EDgeStruct *e1, EDgeStruct *e2);
    int SetContourLimits (void);
    void AdjustNodesForInterval (void);

    int SmoothTriangles (int smfact, int maxdivide);

    void TrianglePoints (TRiangleStruct *tptr,
                         NOdeStruct *nodes,
                         EDgeStruct *edges,
                         double *x,
                         double *y,
                         double *z);

    void TriangleNormal (double *x, double *y, double *z);
    int CalcNodeNormals (void);

    void SetTriangleSubdivideFlag (TRiangleStruct *tptr, int ido);
    void SubdivideTriangleFromCenter (TRiangleStruct *tptr);
    void CleanupTriNormals (void);
    void CleanupNodeNormals (void);
    double CalcZForTriangle (double x, double y,
                             TRiangleStruct *tptr);
    double EdgeLength (int iedge);


  public:

    int con_contour_grid_using_trimesh 
                  (CSW_F *grid, int ncol, int nrow, double nullval,
                   double x1, double y1, double x2, double y2,
                   FAultLineStruct *faults, int nfaults,
                   COntourOutputRec **contours, int *ncontours,
                   COntourCalcOptions *options);
                       


    int con_contours_from_trimesh
                              (TRiangleStruct *trianglelist, int numtriangles,
                               EDgeStruct *edgelist, int numedges,
                               NOdeStruct *nodelist, int numnodes,
                               double null_value,
                               COntourOutputRec **contours_out,
                               int *num_contours_out,
                               COntourCalcOptions *con_calc_options);

    int con_smooth_trimesh (
                        NOdeStruct **nodelist, int *numnodes,
                        EDgeStruct **edgelist, int *numedges,
                        TRiangleStruct **trianglelist, int *numtriangles,
                        int sfact);

};  // end of main class

#endif
