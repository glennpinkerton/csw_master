
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_graph.h

    Define the CSWPolyGraph class.  This class refactors the functionality
    of the old ply_graph.c functions.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_graph.h.
#endif



#ifndef PLY_GRAPH_H
#define PLY_GRAPH_H

/*
    Define some constants used only in this class.
*/
#define CSW_DZERO                  0.0
#define INTEGER_MULTIPLIER         20
#define TOO_CLOSE                  100
#define VERY_CLOSE                 5
#define GRAZE                      9
#define FUDGE                      2
#define RAW_OFFSET                 5
#define INTEGER_RANGE              50000000
#define BBMARGIN                   100

#define IDENTICAL_GRAZE            2000.0

#define EDGE_CHECK_LENGTH          100

#define LOCAL_MAX_GRID_SIZE        20000
#define MAX_SEG_INDEX              4000
#define MAX_NODE_INDEX             4000
#define LIST_CHUNK                 1000

#define NCOMP_INDEXED              20

#include "csw/utils/include/csw_trig_consts.h"

#define MAXSEG                     100


#define PLY_GRAPH_INTERSECT        1
#define PLY_GRAPH_UNION            2
#define PLY_GRAPH_XOR              3
#define PLY_GRAPH_FRAGMENT         4



/*
    Define some structures only used internally by the CSWPolyGraph class.
*/
typedef struct {
    int            x1, y1, x2, y2;
    int            start, size;
    int            *seglist = NULL,
                   maxseg,
                   nseg,
                   used;
}  BOxStruct;

typedef struct {
    int            *x1 = NULL,
                   *y1 = NULL,
                   *x2 = NULL,
                   *y2 = NULL;
    void           **tag1 = NULL;
    void           **tag2 = NULL;
    int            *bbid = NULL;
    int            *compid = NULL;
    int            nvec;
    int            xmin,
                   ymin,
                   xmax,
                   ymax;
    int            gxmin,
                   gymin,
                   gxmax,
                   gymax;
    int            ncol,
                   nrow,
                   gspace;
    BOxStruct      *bblist = NULL;
    int            nbblist;
    int            inside_grid[LOCAL_MAX_GRID_SIZE * 2];
}  RAwEdgeStruct;

typedef struct {
    int            node1,
                   node2;
    int            bbid;
    int            compid;
    signed char    discarded,
                   nused,
                   used_in_last_loop,
                   superflag,
                   tmpdir,
                   dir12,
                   dir21,
                   setid;
}  SEgmentStruct;

typedef struct {
    int            x,
                   y;
    double         xorig,
                   yorig;
    int            seglist[MAXSEG],
                   nseg;
    void           *tag = NULL;
    char           tempflag;
    char           flags[MAXSEG];
}  PLY_NOdeStruct;

typedef struct {
    int            *x = NULL,
                   *y = NULL;
    void           *tag = NULL;
    int            npts,
                   holeflag;
    int            xmin,
                   ymin,
                   xmax,
                   ymax;
}  COmponentStruct;

typedef struct {
    int    *nodes = NULL;
    int    *segs = NULL;
    int    nloop;
} LOopStruct;



class CSWPolyGraph
{

  public:

    CSWPolyGraph () {InitAllMem ();};
    ~CSWPolyGraph () {FreeAllMem ();};

bool  _bbr_ = false;

  private:

void testForLongLine (int cidx, double dt);
void initIntListForDebug (int *lp, int n);

// Static variables from the old C file are now private class variables.

    int         first {1};
    int         hgrid[LOCAL_MAX_GRID_SIZE * 2];
    int         pgrid[LOCAL_MAX_GRID_SIZE * 2],
                vgrid[LOCAL_MAX_GRID_SIZE * 2];

    int         lastnodenum {-1};

    double               *Xp1 {NULL},
                         *Yp1 {NULL},
                         *Xp2 {NULL},
                         *Yp2 {NULL};
    void                 **Tag1 {NULL},
                         **Tag2 {NULL};
    int                  Np1 {0},
                         Np2 {0},
                         *Nc1 {NULL},
                         *Nc2 {NULL},
                         *Nv1 {NULL},
                         *Nv2 {NULL};
    double               XYTiny {0.0};

    int                  *InputCompIDFlag1 {NULL};
    int                  *InputCompIDFlag2 {NULL};
    int                  NumInputCompIDFlag1 {0};
    int                  NumInputCompIDFlag2 {0};

    int                  *IsegList {NULL};
    SEgmentStruct        *SegList {NULL};
    PLY_NOdeStruct       *NodeList {NULL};
    COmponentStruct      *CompList {NULL};
    int                  NumSegs {0},
                         MaxSegs {0},
                         NumNodes {0},
                         MaxNodes {0};

    int                  FragmentFlag {0};

    RAwEdgeStruct        Raw1,
                         Raw2;

    int                  *SegIndex[MAX_SEG_INDEX * 16],
                         *NodeIndex[MAX_NODE_INDEX * 2];
    int                  SegIndexNcol {0},
                         SegIndexNrow {0},
                         SegIndexSpace {0},
                         NodeIndexNcol {0},
                         NodeIndexNrow {0},
                         NodeIndexSpace {0};
    int                  IndexXmin {2000000000},
                         IndexYmin {2000000000};
    int                  IndexXmax {-2000000000},
                         IndexYmax {-2000000000};

    double               TestScale {0.0},
                         TestXmin {0.0},
                         TestYmin {0.0};

    double               *Pxout {NULL},
                         *Pyout {NULL};
    void                 **Ptagout {NULL};
    int                  *Pnpout {NULL},
                         *Pncout {NULL},
                         *Pnvout {NULL};
    int                  Pmaxcomp {0},
                         Pmaxnpts {0};

    int                  *Xwork {NULL},
                         *Ywork {NULL};
    void                 **Tagwork {NULL};

    int                  Nwork {0},
                         Ncomp {0};

    int                  IntegerRange {INTEGER_RANGE * INTEGER_MULTIPLIER},
                         IntegerRange2 {INTEGER_RANGE * 2 * INTEGER_MULTIPLIER};

    int                  NewNode1 {-1},
                         NewNode2 {-1};

    int                  GrazeDistance {GRAZE};

    int                  DiscardFlag {1};
    int                  DoNotCheckTempNodes {0};

    int                  PerpX1 {0},
                         PerpY1 {0},
                         PerpX2 {0},
                         PerpY2 {0};



    int                  BBInsert {0};
    int                  CPInsert {0};

    int                  UnionFlag {0},
                         BBnum {-1};

    double               BugX[10],
                         BugY[10];
    int                  Nbug {0};

    int                  *LoopSegments {NULL},
                         NumLoopSegments {0},
                         MaxLoopSegments {0};

    int                  *LoopNodes {NULL},
                         NumLoopNodes {0},
                         MaxLoopNodes {0};

    LOopStruct           *LoopList {NULL};
    int                  NumLoopList {0},
                         MaxLoopList {0};

// Static functions from the old C file are now private class methods

    int SamePoint (double x1, double y1, double x2, double y2);
    int WriteLines (double *x,
                   double *y,
                   double *z,
                   int    np,
                   int    *nc,
                   int    *nv,
                   char *fname);

    int rawxy (int*, int*, int*, int*, int);
    int rawxy2 (int, int, int*, int*, int*, int*, int);
    int segxy (int);
    int nodexy (int, int);
    int intxy (int, int);
    int pds (PLY_NOdeStruct *);
    int prcomp (COmponentStruct*, int);
    int praw (void);

    void write_current_output (void);
    void type_current_loop (void);

    int SetBugLocation (int ix, int iy);
    int SetupRawVectors (double*, double*, void**, int, int*, int*,
                            double*, double*, void**, int, int*, int*);
    int ResetRawVectors (void);
    int SetupEdgeGrids (void);
    int SetHorizontalCrossingsForVector (int, int, int, int, int*,
                                            int, int, int, int, int);
    int SetVerticalCrossingsForVector (int, int, int, int, int*,
                                          int, int, int, int, int);
    int SegintInteger (int, int, int, int, int, int, int, int,
                          int*, int*);
    int PointInteger (int*, int*, int*, int*, int, int, int,
                         BOxStruct *, int);
    int InsideOutside (int, int, int);
    int FreeAllMem (void);
    int InitAllMem (void);
    int BuildInitialLists (void);
    int InsertSegmentPieces (int, int, int, int, int, int, int);
    int InsertPiece (int, int, int, int, int, int, int, int);
    int CheckExistingNode (int, int);
    int IntersectGraphs (void);
    int UnionGraphs (void);
    int XorGraphs (void);
    int FragmentGraphs (void);
    int RemoveTemporaryNodes (void);
    int CalculateSegmentIntersections (void);
    int SplitSegments (int index_seg1, int index_seg2, int, int, int);
    int BuildPolygonComponents (void);
    int BuildPolygonComponentsForFragments (void);
    int BuildXorComponents (void);
    int RemoveDuplicateSegments (void);
    int RemoveProblemSegments (void);
    int DiscardInteriorSegment (int);
    int NestHoles (void);
    int IndexedNestHoles (void);
    int BuildCompVecs (int*, int*, int*, int*, COmponentStruct*);
    int BuildOutputPolygons (void);
    int SnapNodes (void);
    int SnapPoint (int*, int*);
    int RemoveSegmentFromNode (int, PLY_NOdeStruct*);
    int InsideEdgeCheck (int, int, int, int, int);
    int OutsideEdgeCheck (int, int, int, int, int);
    int TooClose (int, int, int, int);
    int VeryClose (int, int, int, int);
    int IntInside (int, int, int);
    int FreeIndex (int**, int);
    int AddLoopSegment (int segnum);
    int AddLoopNode (int nodenum);
    int AddToLoopList (void);
    int CheckForSuperPolygon (void);
    void FreeLoopList (void);
    int ChooseExitSegment (PLY_NOdeStruct*, int, int*, int);
    int ChooseExitSegmentForFragments (int*, PLY_NOdeStruct*, int, int*, int);
    int ChooseXorExitSegment (PLY_NOdeStruct*, int, int, int*, int);
    int PerpPoints (int, int*, int*, int*, int*);
    int AddToBBList (int, int, int);
    int FreeBBLists (void);
    int RemoveOverlaps (void);
    int ConnectForOverlap (int ioldseg, int middle_node, int opposite_node);
    int PointOnLine (int, int, int, int, int, int);

    int IdenticalInput (double*, double*, int, int*, int*,
                           double*, double*, int, int*, int*);
    int CopyPolygons   (double*, double*, int, int*, int*,
                           double*, double*, int*, int*, int*);

    void CompressSegNodeLists (void);

    int AddUntouchedInputPolygons (int *nc, int *nv);

    int _ply_boolean_ (double*, double*, void**, int, int*, int*,
                     double*, double*, void**, int, int*, int*,
                     int,
                     double*, double*, void**, int*, int*, int*,
                     int, int);

  public:

    void plr (char *grid, int i1, int i2, int ncol);
    void prow (char *grid, int j1, int j2, int i1, int ncol);

    int _test_inside_outside (double*, double*, int, int*, double, double);
    int _test_hole_nesting (int**, int**, int*, int,
                            double*, double*, int*, int*, int*);
    int ply_boolean (double*, double*, void**, int, int*, int*,
                     double*, double*, void**, int, int*, int*,
                     int,
                     double*, double*, void**, int*, int*, int*,
                     int, int);
    int ply_union_components (double*, double*, void**, int, int*, int*,
                              double*, double*, void**, int*, int*, int*,
                              int, int);
    int ply_get_bug_locations (double *bx, double *by, int *nbugs, int maxbugs);
    int ply_nest_holes (double *xpoly, double *ypoly,
                        int npoly, int *polypoints,
                        double *xpout, double *ypout,
                        int *npout, int *icout, int *ipout,
                        int max_points, int max_comps);

    int ply_build_polygons_from_lines (double *xline, double *yline,
                                       int nline, int *line_pts,
                                       double *xpout, double *ypout,
                                       int *npolyout, int *nptsout,
                                       int maxpts, int maxpoly);
    int ply_build_polygons_from_tagged_lines
                                      (double *xline, double *yline, void **tags,
                                       int nline, int *line_pts,
                                       double *xpout, double *ypout, void **tagout,
                                       int *npolyout, int *nptsout,
                                       int maxpts, int maxpoly);

}; // end of main class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
