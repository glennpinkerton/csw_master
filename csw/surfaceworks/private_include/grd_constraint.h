
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 *  Define the CSWGrdConstraint class, which refactors the 
 *  old grd_constraint.c file.
 */

#ifndef GRD_CONSTRAINT_H
#define GRD_CONSTRAINT_H

#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

#include "csw/surfaceworks/include/grd_shared_structs.h"


#define MAX_CONNECT_EDGES          10000


/*
 * Structures for this class only.
 */
typedef struct {
    int        index;
    double     t1z1,
               t1z2,
               t2z1,
               t2z2;
} COnstraintEdge;

typedef struct {
    int    *list = NULL;
    int    nlist,
           maxlist;
} NOdeTriListStruct;

typedef struct {
    int    *nodes = NULL;
    int    *edges = NULL;
    int    num_nodes;
    int    num_edges;
    int    end_1_flag;
    int    end_2_flag;
} ORientedFaultStruct;

typedef struct {
    int    lineid[4];
    char   val[4];
    int    next;
} BFlagStruct;

class CSWGrdConstraint;

#include "csw/surfaceworks/private_include/grd_triangle_class.h"
#include "csw/surfaceworks/private_include/grd_fileio.h"
#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_utils.h"

// Define the main class

class CSWGrdConstraint
{

  private:

    CSWGrdFault     *grd_fault_ptr = NULL;
    CSWGrdFileio    *grd_fileio_ptr = NULL;
    CSWGrdTriangle  *grd_triangle_ptr = NULL;
    CSWGrdUtils     *grd_utils_ptr = NULL;

  public:

    CSWGrdConstraint () {};
    ~CSWGrdConstraint () {};

    void SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};
    void SetGrdFileioPtr (CSWGrdFileio *p) {grd_fileio_ptr = p;};
    void SetGrdTrianglePtr (CSWGrdTriangle *p) {grd_triangle_ptr = p;};
    void SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

  private:

    int    p_i1000[1000];
    int    p_i1001[1000];
    int    p_i1002[1000];
    int    p_i1003[1000];
    double p_d10000[10000];
    double p_d10001[10000];
    double p_d10002[10000];

/*
 * Old static file variables become private class variables.
 */
    int                 SeparateIter = 0;
    BFlagStruct         *BflagList = NULL;

    COnstraintEdge      *ConstraintEdges = NULL;
    int                 NumConstraintEdges = 0;
    int                 MaxConstraintEdges = 0;

    ORientedFaultStruct *OrientedFaults = NULL;
    int                 NumOrientedFaults = 0;
    int                 MaxOrientedFaults = 0;

    EDgeStruct          *EdgeList = NULL;
    int                 NumEdges = 0;
    NOdeStruct          *NodeList = NULL;
    int                 NumNodes = 0;
    TRiangleStruct      *TriList = NULL;
    int                 NumTris = 0;
    int                 MaxNodes,
                        MaxEdges,
                        MaxTris;

    int                 NumFlags = 0;

    NOdeTriListStruct   *NodeTriList = NULL;
    NOdeEdgeListStruct  *NodeEdgeList = NULL;
    int                 NumNodeEdgeList = 0;
    int                 NumNodeTriList = 0;

    int             FaultNodes[MAX_CONNECT_EDGES];
    int             FaultEdges[MAX_CONNECT_EDGES];
    int             FaultSplits[MAX_CONNECT_EDGES];
    int             OppositeSplitNodes[MAX_CONNECT_EDGES];
    int             NumFaultNodes = 0;
    int             NumFaultEdges = 0;

    int             Tflag2List[MAX_CONNECT_EDGES];
    int             NumTflag2List = 0;

    double          Xmin, Ymin, Xmax, Ymax;
    double          Tiny = 0.0;

    int             ConnectEdgeFlag = 0;

    int             ClosedAllowed = 0;


/*
 * Old static file functions become private class functions.
 */
    int FindTriangleEdge (TRiangleStruct *tp1, int n1, int n2);
    double NodeDistance (int n1, int n2);

    void SwapForIntersectEdge (EDgeStruct *eptr, int inode);
    int SwapEdge (int edgenum);
    int CommonNode (int e1, int e2);

    int SplitIntoFour (int t1in, int t2in, int e1in, int n1in,
                          int *elist);
    int AddTriangle (int e1, int e2, int e3, int flag);
    int ExpandMem2 (void);

    int ShiftIntersectNodes (void);
    int RemoveIntersectNode (int nodenum);

    int AddNodeToConnectEdge (
      ORientedFaultStruct   *iptr,
      int                   lastflag,
      ORientedFaultStruct   *jptr,
      int                   jnode);
    int SamePoint (double x1, double y1, double x2, double y2);

    void WriteOrientedFaults (void);
    void ShowBadEdges (void);

    void ResetTflag2 (int ival);
    int AddTflag2 (int index);

    int AddConstraintEdge (int index,
                           double t1z1,
                           double t1z2,
                           double t2z1,
                           double t2z2);

    int BuildNodeTriList (void);
    void FreeNodeTriList (void);
    int AddTriToList (int nodenum, int trinum);

    int OppositeNode (TRiangleStruct *tptr,
                         int edgenum);
    int CalcZValues (void);
    int CheckForConstraintTri (int itri);
    double EstimateZ (int nodenum, int trinum);
    void TrianglePoints (TRiangleStruct *tptr,
                         double *x,
                         double *y,
                         double *z);

    int OrientConstraintEdges (void);
    int FlagConnectedEndPoints (void);
    int BuildConstraintEdgeList (void);
    int BuildVerticalFaultTopology (void);
    int SeparateConstraintEdges (ORientedFaultStruct *ofp,
                                    char *flags);
    int FindDeadEndEdge (int edgenum);
    int OrientFromDeadEndEdge (int edgenum);
    int FixLineOrientation (void);
    int FixNodeEdgesForOrientation (int nodenum,
                                    int newnodenum,
                                    char *flags,
                                    int iedge,
                                    int nextedge);

    int BuildNodeEdgeLists (void);
    void FreeNodeEdgeList (void);
    int AddToNodeEdgeList (int edgenum, int nodenum);
    int RemoveFromNodeEdgeList (int edgenum, int nodenum);

    int AddEdge
      (int n1, int n2, int t1, int t2, int flag, double length);
    int AddNode (double x, double y, double z, int flag);

    int AddOrientedFault (void);
    void FreeMem (void);

    int FillInNullNodes (void);
    double SearchTwoLevels (int nodenum);

    void SetBflag (int node, int edge, int val);
    int GetBflag (int node, int edge);
    void CopyAllBflags (int node1, int node2);
    void CopyBflag (int node1, int node2, int edge);

    int TraceConstraintEdges (void);
    int MakeFaultLinesFromOrientedFaults
      (FAultLineStruct **flist,
       int             *nflist,
       int             dup_flag);
    void CorrectForDuplicates (
                double *xline,
                double *yline,
                double *zline,
                int *npts,
                double tiny);

    void ChangeOrientedFaultNode (int lineid, int oldnode, int newnode);

    void MatchFaultEdgesAndNodes (void);

    void UpdateOnBorderNodes (void);
    void FixForSplits (void);
    void GetFanConnectedEdges (int nodenum,
                               int edgenum,
                               int *list,
                               int *nlist,
                               int maxlist);

    int SetNodesOnFaultToNull (void);

    int IsNVFaultEdge (EDgeStruct *ep);
    void FlagNVFaultEdges (void);




  public:

    int grd_CorrectVerticalFaults
                            (NOdeStruct **nodes,
                             int        *num_nodes,
                             EDgeStruct **edges,
                             int        *num_edges,
                             TRiangleStruct  **triangles,
                             int        *num_triangles);

    int grd_BuildFaultsFromTriMesh (
      NOdeStruct         *nodes,
      int                nnodes,
      EDgeStruct         *edges,
      int                nedges,
      TRiangleStruct     *tris,
      int                ntris,
      FAultLineStruct    **faults,
      int                *nfaults,
      int                dup_flag);

    void grd_set_connect_edge_flag (int ival);


}; // end of main class

#endif
