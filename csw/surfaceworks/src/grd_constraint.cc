
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * This file has the implementation of the methods for the CSWGrdConstraint
 * class.
 *
 * This class is used to build a list of COnstraintEdge structures
 * for a trimesh.  Any edge that is flagged as a GRD_TRIMESH_FAULT_CONDTRAINT
 * will have an entry in the constraint edge list.  These entries are z
 * values for each node based on each triangle shared by the edge.  The
 * index in the edge list of the edge is also in the constraint edge
 * structure.  The EDgeStruct structure will have the index of the
 * constraint edge as its cindex member.
 */

#include <assert.h>
#include <math.h>

#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"

#include "csw/surfaceworks/private_include/grd_triangle_class.h"
#include "csw/surfaceworks/private_include/grd_constraint.h"

/*-----------------------------------------------------------------------------*/


void CSWGrdConstraint::grd_set_connect_edge_flag (int ival)
{
    ConnectEdgeFlag = ival;
}

/*
 * If any vertical fault edges are found, correct them by introducing
 * a second edge.  One edge will tie into the surface on one side of
 * the fault and the other edge will tie into the surface on the other
 * side of the fault.
 */
int CSWGrdConstraint::grd_CorrectVerticalFaults (
                             NOdeStruct       **nodes,
                             int              *num_nodes,
                             EDgeStruct       **edges,
                             int              *num_edges,
                             TRiangleStruct   **triangles,
                             int              *num_triangles)
{
    int           istat, i, n;
    int           do_write;
    char          fname[200];
    double        v6[6];
    EDgeStruct    *eptr;
    NOdeStruct    *nptr;

    if (triangles == NULL  ||  edges == NULL  ||  nodes == NULL) {
        return -1;
    }

    if (*num_nodes < 3  ||  *num_edges < 3  ||  *num_triangles < 1) {
        return -1;
    }

    NodeList = *nodes;
    NumNodes = *num_nodes;
    EdgeList = *edges;
    NumEdges = *num_edges;
    TriList = *triangles;
    NumTris = *num_triangles;
    MaxNodes = NumNodes;
    MaxEdges = NumEdges;
    MaxTris = NumTris;

/*
 * If there are no edges flagged as faults, return with no action.
 */
    n = 0;
    for (i=0; i<NumEdges; i++) {

        eptr = EdgeList + i;

        if (eptr->deleted == 1) {
            continue;
        }

        if (!(eptr->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
              eptr->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT)) {
            continue;
        }

        n++;

    }

    if (n < 1) {
        return 1;
    }

/*
 * Any node that is on a vertical fault but not a vertical fault endpoint
 * needs to have its z value 
 * set to a hard null.  Most will already be set this way, but if an original
 * trimesh node (prior to constraints being added) was exactly on a fault line,
 * it may have a "valid" z value and need to be changed.
 */
    istat = SetNodesOnFaultToNull ();
    if (istat != 1) {
        return -1;
    }

    Xmin = Ymin = 1.e30;
    Xmax = Ymax = -1.e30;

    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        if (nptr->x < Xmin) Xmin = nptr->x;
        if (nptr->y < Ymin) Ymin = nptr->y;
        if (nptr->x > Xmax) Xmax = nptr->x;
        if (nptr->y > Ymax) Ymax = nptr->y;
        nptr->adjusting_node = -1;
    }

    if (Xmin >= Xmax  ||  Ymin >= Ymax) {
        return -1;
    }
    Tiny = (Xmax - Xmin + Ymax - Ymin) / 20000.0;

  /*
   * Make a list of BFlagStruct objects.  This list needs to be
   * as large as the node list can grow to.  I set it to twice
   * the node list size.
   */
    BflagList = (BFlagStruct *)csw_Calloc (NumNodes * 2 * sizeof(BFlagStruct));
    if (BflagList == NULL) {
        return -1;
    }

  /*
   * Initialize the bflag list edge numbers to -1
   * and the edge list lineid numbers to -1.
   */
    for (i=0; i<NumNodes; i++) {
        BflagList[i].lineid[0] = -1;
        BflagList[i].lineid[1] = -1;
        BflagList[i].lineid[2] = -1;
        BflagList[i].lineid[3] = -1;
    }

    for (i=0; i<NumEdges; i++) {
        EdgeList[i].lineid2 = -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        sprintf (fname, "pre_vert_fault.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, v6,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    FreeNodeEdgeList ();

    istat =
    BuildNodeEdgeLists ();
    if (istat == -1) {
        FreeMem ();
        return -1;
    }

    istat = OrientConstraintEdges ();
    if (istat == -1) {
        FreeMem ();
        return -1;
    }

    istat = BuildConstraintEdgeList ();
    if (istat == -1) {
        FreeMem ();
        return -1;
    }

    istat = BuildVerticalFaultTopology ();
    if (istat == -1) {
        FreeMem ();
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        sprintf (fname, "pre_fill_null_nodes.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, v6,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    istat = FillInNullNodes ();
    if (istat == -1) {
        FreeMem ();
        return -1;
    }

    if (do_write == 1) {
        sprintf (fname, "vert_fault.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, v6,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    *nodes = NodeList;
    *num_nodes = NumNodes;
    *edges = EdgeList;
    *num_edges = NumEdges;
    *triangles = TriList;
    *num_triangles = NumTris;

    FreeMem ();

    return 1;

}


/*---------------------------------------------------------------------*/

/*
 * Orient the edges for a particular constraint so that all the edge
 * tri1 triangles are on the right side and all the tri2 triangles
 * are on the left side.
 */
int CSWGrdConstraint::OrientConstraintEdges (void)
{
    int                 i, j, istat, dead_end;
    EDgeStruct          *ep, *ep2;

/*
 * Set the tflag2 members of the edge structures to zero so they
 * can be used to keep track of which edges have already been
 * processed.
 */
    for (i=0; i<NumEdges; i++) {
        EdgeList[i].tflag2 = 0;
    }

/*
 * The node is_intersect member is used to flag nodes on the
 * endpoint of a fault that also coincide with an endpoint
 * of another fault.  In other words, the point where two
 * fault lines intersect.
 */
    for (i=0; i<NumNodes; i++) {
        NodeList[i].is_intersect = 0;
    }

    NumTflag2List = 0;

    ShowBadEdges ();

/*
 * The non vertical fault edges need to be distinguished
 * from the vertical before correcting for vertical faults.
 */
    FlagNVFaultEdges ();

/*
 * Loop through all the edges and orient the fault edges
 * that have two triangles attached to them.
 */
    for (i=0; i<NumEdges; i++) {

        ep = EdgeList + i;
        if (
            !(ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
              ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT) ||
            ep->deleted == 1  ||
            ep->tflag2 == 1) {
            continue;
        }

        if (IsNVFaultEdge (ep)) {
            continue;
        }

        if (ep->isconstraint  &&  ep->tri2 == -1) {
            continue;
        }

        dead_end = FindDeadEndEdge (i);
        if (dead_end == -1) {
            return -1;
        }

        istat = OrientFromDeadEndEdge (dead_end);
        if (istat == -1) {
            return -1;
        }

        for (j=0; j<NumFaultEdges; j++) {
            ep2 = EdgeList + FaultEdges[j];
            ep2->lineid2 = NumOrientedFaults;
        }

        istat = FixLineOrientation ();
        if (istat == -1) {
            return -1;
        }

        istat = AddOrientedFault ();
        if (istat == -1) {
            return -1;
        }

        ResetTflag2 (1);
        NumTflag2List = 0;

    }

    istat =
      FlagConnectedEndPoints ();

    WriteOrientedFaults ();

    return istat;

}

int CSWGrdConstraint::IsNVFaultEdge (EDgeStruct *ep)
{
    if (ep->lineid >= GRD_NV_FAULT_ID_BASE) {
        return 1;
    }
    return 0;
}

/*---------------------------------------------------------------------*/

/*
 * If the endpoint of a fault is co located with a point
 * on another fault with a different id, then split the
 * edge on the end that connects to the other fault.  The
 * 3 new edges and the new node are all flagged for special
 * treatment when the faults are later separated.  The new
 * node and the connecting node on the other fault are
 * defined as pairs of each othr, again for special
 * treatment later.
 */

int CSWGrdConstraint::FlagConnectedEndPoints (void)
{
    int                i, j, k, same;
    int                newnode;
    double             x1, y1, x2, y2,
                       x3, y3;
    NOdeStruct         *nptr;
    ORientedFaultStruct *iptr, *jptr;

    int                do_write;
    char               fname[200];

    if (NumOrientedFaults < 1000000) {
        return 1;
    }

    for (i=0; i<NumOrientedFaults; i++) {

        iptr = OrientedFaults + i;
        nptr = NodeList + iptr->nodes[0];
        x1 = nptr->x;
        y1 = nptr->y;
        nptr = NodeList + iptr->nodes[iptr->num_nodes - 1];
        x2 = nptr->x;
        y2 = nptr->y;

        for (j=0; j<NumOrientedFaults; j++) {
            if (i == j) continue;

            jptr = OrientedFaults + j;

            for (k=0; k<jptr->num_nodes; k++) {
                nptr = NodeList + jptr->nodes[k];
                x3 = nptr->x;
                y3 = nptr->y;

                same = SamePoint(x1, y1, x3, y3);
                if (same) {
                    iptr->end_1_flag = 1;
                    if (ConnectEdgeFlag) {
                        newnode =
                          AddNodeToConnectEdge (
                            iptr, 0,
                            jptr, k);
                        if (newnode == -1) {
                            return -1;
                        }
                    }
                }
                same = SamePoint(x2, y2, x3, y3);
                if (same) {
                    iptr->end_2_flag = 1;
                    if (ConnectEdgeFlag) {
                        newnode =
                          AddNodeToConnectEdge (
                            iptr, 1,
                            jptr, k);
                        if (newnode == -1) {
                            return -1;
                        }
                    }
                }
            }
        }
    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        sprintf (fname, "postflag.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    return 1;

}

/*---------------------------------------------------------------------*/

/*
 * Add new nodes and edges for vertical faults.
 */
int CSWGrdConstraint::BuildVerticalFaultTopology (void)
{
    ORientedFaultStruct  *ofp = NULL;
    int                  i, istat;
    char                 *flags = NULL;
    int                  do_write;
    char                 fname[200];


    auto fscope = [&]()
    {
        csw_Free (flags);
    };
    CSWScopeGuard func_scope_guard (fscope);


    flags = (char *)csw_Malloc (2 * NumEdges * sizeof(char));
    if (flags == NULL) {
        return -1;
    }
    NumFlags = 2 * NumEdges;

    for (i=0; i<NumOrientedFaults; i++) {

        ofp = OrientedFaults + i;

        NumFaultNodes = ofp->num_nodes;
        NumFaultEdges = ofp->num_edges;
        memcpy (FaultNodes, ofp->nodes, NumFaultNodes * sizeof(int));
        memcpy (FaultEdges, ofp->edges, NumFaultEdges * sizeof(int));

        SeparateIter = i;
        istat = SeparateConstraintEdges (ofp, flags);
        if (istat == -1) {
            return -1;
        }

        MatchFaultEdgesAndNodes ();
        UpdateOnBorderNodes ();
    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        sprintf (fname, "preshift.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    istat = ShiftIntersectNodes ();
    if (istat == -1) {
      return -1;
    }

    return 1;

}


/*---------------------------------------------------------------------*/

/*
  Build a list of all constraint edges with their z values set by extrapolating
  from triangles on the correct side of the edge.
*/

int CSWGrdConstraint::BuildConstraintEdgeList (void)
{
    int              i, istat;
    EDgeStruct       *eptr;

    ConstraintEdges = NULL;
    NumConstraintEdges = 0;
    MaxConstraintEdges = 0;

/*
 * Initialize node spillnum members to zero so
 * they can be used to flag the nodes used in
 * constraint edges.
 */
    for (i=0; i<NumNodes; i++) {
        NodeList[i].spillnum = 0;
    }

/*
 * Populate the constraint edge structures initially with 1.e30 values.
 */
    for (i=0; i<NumEdges; i++) {

        eptr = EdgeList + i;

        if (eptr->deleted == 1) {
            continue;
        }

        if (!(eptr->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
              eptr->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT)) {
            continue;
        }

        istat = AddConstraintEdge (i, 1.e30, 1.e30, 1.e30, 1.e30);
        if (istat == -1) {
            return -1;
        }

        eptr->cindex = istat;
        NodeList[eptr->node1].spillnum = 1;
        NodeList[eptr->node2].spillnum = 1;

    }

    if (NumConstraintEdges < 1) {
        return 1;
    }

/*
 * Build the list of triangles that use each node.
 */
    istat = BuildNodeTriList ();
    if (istat == -1) {
        return -1;
    }

    FillInNullNodes ();

/*
 * Calculate the 4 z values for each edge.
 */
    istat = CalcZValues ();
    if (istat == -1) {
        return -1;
    }

    return 1;

}


/*---------------------------------------------------------------------------------------*/

/*
 * Add a constraint edge to the list and return it's index. If the memory allocation
 * fails, -1 is returned.
 */

int CSWGrdConstraint::AddConstraintEdge (int index,
                              double t1z1,
                              double t1z2,
                              double t2z1,
                              double t2z2)
{
    COnstraintEdge   *cptr;

    if (ConstraintEdges == NULL  ||  NumConstraintEdges >= MaxConstraintEdges) {
        MaxConstraintEdges += 100;
        ConstraintEdges = (COnstraintEdge *)
            csw_Realloc (ConstraintEdges, MaxConstraintEdges * sizeof(COnstraintEdge));
    }

    if (ConstraintEdges == NULL) {
        return -1;
    }

    cptr = ConstraintEdges + NumConstraintEdges;
    cptr->index = index;
    cptr->t1z1 = t1z1;
    cptr->t2z1 = t2z1;
    cptr->t1z2 = t1z2;
    cptr->t2z2 = t2z2;

    NumConstraintEdges++;

    return (NumConstraintEdges - 1);

}


/*------------------------------------------------------------------------------*/

/*
 * Build a list of the edges that attach to each node.
 */

int CSWGrdConstraint::BuildNodeEdgeLists (void)
{
    int                   i, ntot, istat;

/*
 * Clean up any existing list first.
 */
    FreeNodeEdgeList ();
    ntot = NumNodes * 2;
    NumNodeEdgeList = ntot;
    NodeEdgeList = (NOdeEdgeListStruct *)csw_Calloc
                   (ntot * sizeof(NOdeEdgeListStruct));
    if (NodeEdgeList == NULL) {
        NumNodeEdgeList = 0;
        return -1;
    }
    for (i=0; i<NumEdges; i++) {
        istat = AddToNodeEdgeList (i, EdgeList[i].node1);
        istat = AddToNodeEdgeList (i, EdgeList[i].node2);
        if (istat == -1) {
            grd_utils_ptr->grd_set_err(1);
            return -1;
        }
    }

    return 1;

}  /*  end of private BuildNodeEdgeLists function  */



/*------------------------------------------------------------------------------------*/

int CSWGrdConstraint::AddToNodeEdgeList (int edgenum, int nodenum)

{
    int                *list, nlist, maxlist;
    NOdeEdgeListStruct *nptr;

    if (nodenum > NumNodeEdgeList) {
        assert (0);
    }

    nptr = NodeEdgeList + nodenum;
    nlist = nptr->nlist;
    maxlist = nptr->maxlist;
    list = nptr->list;

    if (nlist >= maxlist) {
        maxlist += 6;
        list = (int *)csw_Realloc (list, maxlist * sizeof(int));
        if (list == NULL) {
            nptr->list = NULL;
            grd_utils_ptr->grd_set_err (1);
            return -1;
        }
    }

    list[nlist] = edgenum;
    nlist++;

    nptr->maxlist = maxlist;
    nptr->list = list;
    nptr->nlist = nlist;

    return 1;

}  /* end of private AddToNodeEdgeList function  */


/*------------------------------------------------------------------------------------*/

int CSWGrdConstraint::RemoveFromNodeEdgeList (int edgenum, int nodenum)

{
    int                *list, nlist, n, i;
    NOdeEdgeListStruct *nptr;

    nptr = NodeEdgeList + nodenum;
    nlist = nptr->nlist;
    list = nptr->list;

    n = 0;
    for (i=0; i<nlist; i++) {
        if (list[i] == edgenum) continue;
        list[n] = list[i];
        n++;
    }

    nptr->nlist = n;

    return 1;

}  /* end of private RemoveFromNodeEdgeList function  */



/*---------------------------------------------------------------------------------------*/

/*
 * Build a list of each triangle that is connected to each node.
 */
int CSWGrdConstraint::BuildNodeTriList (void)
{
    int             i, istat;
    TRiangleStruct  *tptr;
    EDgeStruct      *e1, *e2;
    int             n1, n2, n3;

/*
 * Allocate space for a new list.
 */
    FreeNodeTriList ();
    NodeTriList = (NOdeTriListStruct *) csw_Calloc
        (NumNodes * sizeof (NOdeTriListStruct));
    if (NodeTriList == NULL) {
        return -1;
    }
    NumNodeTriList = NumNodes;

    for (i=0; i<NumTris; i++) {
        tptr = TriList + i;
        if (tptr->deleted == 1) {
            continue;
        }
        e1 = EdgeList + tptr->edge1;
        e2 = EdgeList + tptr->edge2;
        n1 = e1->node1;
        n2 = e1->node2;
        if (e2->node1 == n1  ||  e2->node1 == n2) {
            n3 = e2->node2;
        }
        else {
            n3 = e2->node1;
        }

        istat = AddTriToList (n1, i);
        if (istat == -1) {
            return -1;
        }
        istat = AddTriToList (n2, i);
        if (istat == -1) {
            return -1;
        }
        istat = AddTriToList (n3, i);
        if (istat == -1) {
            return -1;
        }

    }

    return 1;

}


/*---------------------------------------------------------------------------------*/

void CSWGrdConstraint::FreeNodeEdgeList (void)
{
    int            i, ndo;

    if (NodeEdgeList) {
        ndo = NumNodeEdgeList;
        for (i=0; i<ndo; i++) {
            if (NodeEdgeList[i].list != NULL) {
                csw_Free (NodeEdgeList[i].list);
            }
        }
        csw_Free (NodeEdgeList);
    }
    NodeEdgeList = NULL;

    return;
}


/*--------------------------------------------------------------------------------*/

void CSWGrdConstraint::FreeNodeTriList (void)
{
    int              i;
    NOdeTriListStruct    *nptr;

    if (NodeTriList == NULL) {
        return;
    }

    for (i=0; i<NumNodeTriList; i++) {
        nptr = NodeTriList + i;
        csw_Free (nptr->list);
    }

    csw_Free (NodeTriList);

    NodeTriList = NULL;

    return;

}


/*----------------------------------------------------------------------------------*/

int CSWGrdConstraint::AddTriToList (int nodenum, int trinum)
{
    NOdeTriListStruct      *nptr;
    int                *list, nlist, maxlist;

    nptr = NodeTriList + nodenum;
    list = nptr->list;
    nlist = nptr->nlist;
    maxlist = nptr->maxlist;

    if (list == NULL  ||  nlist >= maxlist) {
        maxlist += 8;
        list = (int *)csw_Realloc (list, maxlist * sizeof(int));
    }

    if (list == NULL) {
        return -1;
    }

    list[nlist] = trinum;
    nlist++;

    nptr->list = list;
    nptr->nlist = nlist;
    nptr->maxlist = maxlist;

    return 1;

}


/*----------------------------------------------------------------------------------*/

/*
 * Use the triangles on the same side of the fault to calculate z values for the
 * edge.  This can only be called from grd_BuildConstraintEdgeList because the
 * original constraint edges must be oriented with the OrientConstraintEdges
 * function before calling this function.
 */
int CSWGrdConstraint::CalcZValues (void)
{
    int                i, j, n1, n2, n3, itri, istat,
                       *tlist1 = NULL, nlist1, tlist[100], nlist;
    int                *t1ivalues = NULL, *t2ivalues = NULL;
    double             zt, sum1, sum2, zt2;
    double             *t1zvalues = NULL, *t2zvalues = NULL;
    TRiangleStruct     *tp1 = NULL, *tp2 = NULL;
    COnstraintEdge     *cp = NULL;
    EDgeStruct         *ep = NULL;


    auto fscope = [&]()
    {
        csw_Free (t1zvalues);
    };
    CSWScopeGuard func_scope_guard (fscope);


/*
 * Allocate space for averaging multiple values calculated at constraint nodes.
 * The z values are initialized to 1.e30.  The counter variable (ivalues variables)
 * are initialized to zero.
 */
    t1zvalues = (double *)csw_Malloc (NumNodes * 2 * sizeof(double));
    if (t1zvalues == NULL) {
        return -1;
    }
    t2zvalues = t1zvalues + NumNodes;

    t1ivalues = (int *)csw_Calloc (NumNodes * 2 * sizeof(int));
    if (t1ivalues == NULL) {
        return -1;
    }
    t2ivalues = t1ivalues + NumNodes;

    for (i=0; i<NumNodes; i++) {
        t1zvalues[i] = 1.e30;
        t2zvalues[i] = 1.e30;
    }

/*
 * Update the averaging list for each node in each constraint edge.
 */
    for (i=0; i<NumConstraintEdges; i++) {

    /*
     * Find the triangles sharing the edge.
     * If the fault edge is also a border edge of the trimesh,
     * then either tp1 or tp2 is set to NULL, depending upon
     * the right or leftness of the single existing triangle.
     */
        cp = ConstraintEdges + i;
        ep = EdgeList + cp->index;
        n1 = ep->node1;
        n2 = ep->node2;
        if (ep->tri1 >= 0) {
            tp1 = TriList + ep->tri1;
        }
        else {
            tp1 = NULL;
        }
        if (ep->tri2 >= 0) {
            tp2 = TriList + ep->tri2;
        }
        else {
            tp2 = NULL;
        }

    /*
     * If both tp1 and tp2 are NULL, there is a program bug, so assert.
     */
        if (tp1 == NULL  &&  tp2 == NULL) {
            assert (0);
        }

    /*
     * Process tri1, which should always be on the
     * right side of the edge after previous orientation.
     * If tri1 is NULL, skip and process tri2 first.
     */
        if (tp1 != NULL) {
            n3 = OppositeNode (tp1, cp->index);
            if (n3 < 0) {
                assert (0);
            }

          /*
           * Find all the triangles that use the opposite node of the
           * constraint triangle that are not also constraint triangles.
           * A maximum of 100 triangles are used.
           */
            tlist1 = NodeTriList[n3].list;
            nlist1 = NodeTriList[n3].nlist;
            if (tlist1 == NULL  ||  nlist1 < 1) {
                assert (0);
            }
            nlist = 0;
            for (j=0; j<nlist1; j++) {
                itri = tlist1[j];
                istat = CheckForConstraintTri (itri);
                if (istat == 1) {
                    continue;
                }
                if (istat == -1) {
                    assert (0);
                }
                tlist[nlist] = itri;
                nlist++;
                if (nlist >= 100) {
                    break;
                }
            }

          /*
           * If no triangles were found, set to a null value
           * and interpolate it later.
           */
            if (nlist < 1) {
                if (t1zvalues[n1] > 1.e20) {
                    t1ivalues[n1] = 1;
                    t1zvalues[n1] = NodeList[n3].z;
                }
                if (t1zvalues[n2] > 1.e20) {
                    t1ivalues[n2] = 1;
                    t1zvalues[n2] = NodeList[n3].z;
                }
            }
            else {
                sum1 = 0.0;
                sum2 = 0.0;
                for (j=0; j<nlist; j++) {
                    zt = EstimateZ (n1, tlist[j]);
                    if (zt < 1.e20) {
                        sum1 += zt;
                        sum2++;
                    }
                }
                if (sum2 <= 0.0) {
                    if (t1zvalues[n1] > 1.e20) {
                        t1ivalues[n1] = 1;
                    }
                    if (t1zvalues[n2] > 1.e20) {
                        t1ivalues[n2] = 1;
                    }
                }
                else {
                    zt = sum1 / sum2;
                    if (t1zvalues[n1] > 1.e20) {
                        t1zvalues[n1] = zt;
                        t1ivalues[n1] = 1;
                    }
                    else {
                        t1zvalues[n1] += zt;
                        t1ivalues[n1] += 1;
                    }
                    if (t1zvalues[n2] > 1.e20) {
                        t1zvalues[n2] = zt;
                        t1ivalues[n2] = 1;
                    }
                    else {
                        t1zvalues[n2] += zt;
                        t1ivalues[n2] += 1;
                    }
                }
            }
        }

    /*
     * Process tri2, which should always be on the
     * left side of the edge after previous orientation.
     */

    /*
     * If tp2 is NULL, the edge is on the trimesh border and
     * I do not change the t2 zvalues or t2ivalues.
     */
        if (tp2 == NULL) {
            if (t2zvalues[n1] > 1.e20) {
                t2ivalues[n1] = 1;
            }
            if (t2zvalues[n2] > 1.e20) {
                t2ivalues[n2] = 1;
            }
        }

        else {
            n3 = OppositeNode (tp2, cp->index);
            if (n3 < 0) {
                assert (0);
            }

          /*
           * Find all the triangles that use the opposite node of the
           * constraint triangle that are not also constraint triangles.
           * A maximum of 100 triangles are used.
           */
            tlist1 = NodeTriList[n3].list;
            nlist1 = NodeTriList[n3].nlist;
            if (tlist1 == NULL  ||  nlist1 < 1) {
                assert (0);
            }
            nlist = 0;
            for (j=0; j<nlist1; j++) {
                itri = tlist1[j];
                istat = CheckForConstraintTri (itri);
                if (istat == 1) {
                    continue;
                }
                tlist[nlist] = itri;
                nlist++;
                if (nlist >= 100) {
                    break;
                }
            }

          /*
           * If no triangles were found, set to a null value
           * and interpolate it later.
           */
            if (nlist < 1) {
                if (t2zvalues[n1] > 1.e20) {
                    t2ivalues[n1] = 1;
                    t2zvalues[n1] = NodeList[n3].z;
                }
                if (t2zvalues[n2] > 1.e20) {
                    t2ivalues[n2] = 1;
                    t2zvalues[n2] = NodeList[n3].z;
                }
            }
            else {
                sum1 = 0.0;
                sum2 = 0.0;
                for (j=0; j<nlist; j++) {
                    zt = EstimateZ (n1, tlist[j]);
                    if (zt < 1.e20) {
                        sum1 += zt;
                        sum2++;
                    }
                }
                if (sum2 < 0.0) {
                    assert (0);
                }
                zt = sum1 / sum2;
                if (t2zvalues[n1] > 1.e20) {
                    t2zvalues[n1] = zt;
                    t2ivalues[n1] = 1;
                }
                else {
                    t2zvalues[n1] += zt;
                    t2ivalues[n1] += 1;
                }
                if (t2zvalues[n2] > 1.e20) {
                    t2zvalues[n2] = zt;
                    t2ivalues[n2] = 1;
                }
                else {
                    t2zvalues[n2] += zt;
                    t2ivalues[n2] += 1;
                }
            }
        }

    /*
     * If tp1 is NULL, do not change its values.
     */
        if (tp1 == NULL) {
            if (t1zvalues[n1] > 1.e20) {
                t1ivalues[n1] = 1;
            }
            if (t1zvalues[n2] > 1.e20) {
                t1ivalues[n2] = 1;
            }
        }

    }  /* end of first loop through the ConstraintEdges list */

/*
 * BUGFIX !!!!
 *  If the z value is null from the above code, leave it as null.
 */
/*
 * If a z value on the fault is not defined, try to
 * use the opposite node on the edge for the z value.
    for (i=0; i<NumConstraintEdges; i++) {
        cp = ConstraintEdges + i;
        ep = EdgeList + cp->index;
        n1 = ep->node1;
        n2 = ep->node2;
        if (t1zvalues[n1] < 1.e20  &&  t1zvalues[n2] < 1.e20  &&
            t2zvalues[n1] < 1.e20  &&  t2zvalues[n2] < 1.e20) {
            continue;
        }
        if (t1zvalues[n1] > 1.e20) {
            t1zvalues[n1] = t1zvalues[n2];
            t1ivalues[n1] = 1;
        }
        if (t2zvalues[n1] > 1.e20) {
            t2zvalues[n1] = t2zvalues[n2];
            t2ivalues[n1] = 1;
        }
        if (t1zvalues[n2] > 1.e20) {
            t1zvalues[n2] = t1zvalues[n1];
            t1ivalues[n2] = 1;
        }
        if (t2zvalues[n2] > 1.e20) {
            t2zvalues[n2] = t2zvalues[n1];
            t2ivalues[n2] = 1;
        }
    }
 */

/*
 * If there are still undefined z values, use the value
 * from the opposite triangle.  For this to happen, the
 * vertical fault trace on the surface must curve back
 * on itself steeply.  Such a curvature is definately
 * a "garbage in" situation.  So, the code to handle it
 * is designed to prevent a crash, not to produce "good"
 * results.
    for (i=0; i<NumConstraintEdges; i++) {
        cp = ConstraintEdges + i;
        ep = EdgeList + cp->index;
        n1 = ep->node1;
        n2 = ep->node2;
        if (t1zvalues[n1] < 1.e20  &&  t1zvalues[n2] < 1.e20  &&
            t2zvalues[n1] < 1.e20  &&  t2zvalues[n2] < 1.e20) {
            continue;
        }
        if (t1zvalues[n1] > 1.e20) {
            t1zvalues[n1] = t2zvalues[n1];
            t1ivalues[n1] = 1;
        }
        if (t2zvalues[n1] > 1.e20) {
            t2zvalues[n1] = t1zvalues[n1];
            t2ivalues[n1] = 1;
        }
        if (t1zvalues[n2] > 1.e20) {
            t1zvalues[n2] = t2zvalues[n2];
            t1ivalues[n2] = 1;
        }
        if (t2zvalues[n2] > 1.e20) {
            t2zvalues[n2] = t1zvalues[n2];
            t2ivalues[n2] = 1;
        }
    }
 */

/*
 * Loop through the constraint edges again.  Each node on each
 * constraint edge is assigned the value from averaging the z
 * estimates calculated at each node (the values in t1zvalues,
 * t2zvalues, t1ivalues and t2ivalues).
 */
    for (i=0; i<NumConstraintEdges; i++) {

        cp = ConstraintEdges + i;
        ep = EdgeList + cp->index;
        n1 = ep->node1;
        n2 = ep->node2;

        if (t1ivalues[n1] < 1  ||
            t1ivalues[n2] < 1  ||
            t2ivalues[n1] < 1  ||
            t2ivalues[n2] < 1) {
            assert (0);
        }

        if (t1zvalues[n1] < 1.e20) {
            cp->t1z1 = t1zvalues[n1] / t1ivalues[n1];
        }
        else {
            cp->t1z1 = 1.e30;
        }
        if (t1zvalues[n2] < 1.e20) {
            cp->t1z2 = t1zvalues[n2] / t1ivalues[n2];
        }
        else {
            cp->t1z2 = 1.e30;
        }
        if (t2zvalues[n1] < 1.e20) {
            cp->t2z1 = t2zvalues[n1] / t2ivalues[n1];
        }
        else {
            cp->t2z1 = 1.e30;
        }
        if (t2zvalues[n2] < 1.e20) {
            cp->t2z2 = t2zvalues[n2] / t2ivalues[n2];
        }
        else {
            cp->t2z2 = 1.e30;
        }

        if (ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT) {
            if (cp->t1z1 > 1.e20) {
                cp->t1z1 = cp->t2z1;
            }
            else if (cp->t2z1 > 1.e20) {
                cp->t2z1 = cp->t1z1;
            }
            else {
                zt2 = cp->t1z1 + cp->t2z1;
                cp->t1z1 = zt2;
                cp->t2z1 = zt2;
            }
            if (cp->t1z2 > 1.e20) {
                cp->t1z2 = cp->t2z2;
            }
            else if (cp->t2z2 > 1.e20) {
                cp->t2z2 = cp->t1z2;
            }
            else {
                zt2 = cp->t1z2 + cp->t2z2;
                cp->t1z2 = zt2;
                cp->t2z2 = zt2;
            }
        }

    }

    return 1;

}


/*----------------------------------------------------------------------------------*/

/*
 * Return the index of the opposite node to the specified edge on the
 * specified triangle.
 */
int CSWGrdConstraint::OppositeNode (TRiangleStruct *tp,
                         int edgenum)
{
    EDgeStruct    *ep;
    int           n1, n2, n3;

    ep = EdgeList + edgenum;
    n1 = ep->node1;
    n2 = ep->node2;

    if (tp->edge1 != edgenum) {
        ep = EdgeList + tp->edge1;
        if (ep->node1 == n1  ||  ep->node1 == n2) {
            n3 = ep->node2;
        }
        else {
            n3 = ep->node1;
        }
        return n3;
    }

    if (tp->edge2 != edgenum) {
        ep = EdgeList + tp->edge2;
        if (ep->node1 == n1  ||  ep->node1 == n2) {
            n3 = ep->node2;
        }
        else {
            n3 = ep->node1;
        }
        return n3;
    }

    if (tp->edge3 != edgenum) {
        ep = EdgeList + tp->edge3;
        if (ep->node1 == n1  ||  ep->node1 == n2) {
            n3 = ep->node2;
        }
        else {
            n3 = ep->node1;
        }
        return n3;
    }

    return -1;

}

/*------------------------------------------------------------------------*/

/*
 * Return the x, y, z points for the specified triangle.
 */
void CSWGrdConstraint::TrianglePoints (TRiangleStruct *tptr,
                            double *x,
                            double *y,
                            double *z)
{
    EDgeStruct         *eptr;
    NOdeStruct         *nptr;
    int                n1, n2, n3;

    eptr = EdgeList + tptr->edge1;
    n1 = eptr->node1;
    nptr = NodeList + n1;
    x[0] = nptr->x;
    y[0] = nptr->y;
    z[0] = nptr->z;
    n2 = eptr->node2;
    nptr = NodeList + n2;
    x[1] = nptr->x;
    y[1] = nptr->y;
    z[1] = nptr->z;

    eptr = EdgeList + tptr->edge2;
    if (eptr->node1 == n1  ||  eptr->node1 == n2) {
        n3 = eptr->node2;
    }
    else {
        n3 = eptr->node1;
    }

    nptr = NodeList + n3;
    x[2] = nptr->x;
    y[2] = nptr->y;
    z[2] = nptr->z;

    return;

}  /* end of private TrianglePoints function */


/*----------------------------------------------------------------------------------*/


double CSWGrdConstraint::EstimateZ (int nodenum, int itri)
{
    double        xtri[3], ytri[3], ztri[3];
    double        xt, yt, zt, coef[3];

    xt = NodeList[nodenum].x;
    yt = NodeList[nodenum].y;

    TrianglePoints (TriList+itri,
                    xtri, ytri, ztri);

    grd_utils_ptr->grd_calc_double_plane (xtri, ytri, ztri, 3, coef);
    zt = coef[0] + xt * coef[1] + yt * coef[2];

    return zt;

}


/*------------------------------------------------------------------------------------*/

int CSWGrdConstraint::CheckForConstraintTri (int itri)
{
    int       n1, n2, n3, istat;

    istat =
    grd_triangle_ptr->grd_get_nodes_for_triangle
       (TriList + itri,
        EdgeList,
        &n1, &n2, &n3);

    if (istat == -1) {
        return -1;
    }

    if (NodeList[n1].spillnum == 1  ||
        NodeList[n2].spillnum == 1  ||
        NodeList[n3].spillnum == 1) {
        return 1;
    }

    return 0;

}



/*-----------------------------------------------------------------------------*/

/*
 * Find a dead end edge that connects along the same fault to the
 * specified edge number.  If a dead end is found, the edge number
 * of the dead end is returned.  If no dead end is found, the fault
 * is actually a closed polygon, which should not happen when this
 * function is called. The return value is -1 if no dead end is found.
 */
int CSWGrdConstraint::FindDeadEndEdge (int edgenum)
{
    int             i, found, n1start, n2start;
    int             *list, nlist, enow, lineid;
    int             nodenow, lastedge;
    EDgeStruct      *ep;

/*
 * Set the start nodes to the edgenum's nodes.
 */
    ep = EdgeList + edgenum;
    n1start = ep->node1;
    n2start = ep->node2;
    lineid = ep->lineid;
    enow = edgenum;
    nodenow = n1start;
    lastedge = edgenum;

    ResetTflag2 (0);

/*
 * Traverse from node to node using fault edges until
 * a dead end is found or until the fault closes on itself.
 */
    for (;;) {

        list = NodeEdgeList[nodenow].list;
        if (list == NULL) {
            return 0;
        }
        nlist = NodeEdgeList[nodenow].nlist;
        found = 0;
        for (i=0; i<nlist; i++) {
            if (list[i] == enow) {
                continue;
            }
            ep = EdgeList + list[i];
            if (ep->deleted == 1) {
                continue;
            }
            if (!(ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
                  ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT)) {
                continue;
            }
            if (ep->lineid != lineid) {
                continue;
            }
            if (ep->tflag2 == 1) {
                continue;
            }
            ep->tflag2 = 1;
            AddTflag2 (list[i]);
            if (ep->node1 == nodenow) {
                nodenow = ep->node2;
            }
            else {
                nodenow = ep->node1;
            }
            enow = list[i];
            lastedge = list[i];
            found = 1;
            break;
        }

    /*
     * No additional fault edge is connected, so this edge is the dead end.
     * Return the last edge number in this case.
     */
        if (found == 0) {
            return lastedge;
        }

        if (nodenow == n1start) {
            break;
        }
        if (nodenow == n2start) {
            break;
        }

    }

/*
 * The fault closes on itself, which should not be the case.
 * Return an error if this happens.
 */
    if (ClosedAllowed == 0) {
        return -1;
    }
    else {
        return 1;
    }

}

/*-----------------------------------------------------------------------------*/

/*
 * Populate the FaultNodes and FaultEdges arrays given the specified
 * end edge of a fault.
 */
int CSWGrdConstraint::OrientFromDeadEndEdge (int edgenum)
{
    int             i, found, found1, found2,
                    n1start, n2start;
    int             *list, nlist, enow, lineid;
    int             nodenow, nodestart;
    EDgeStruct      *ep;

/*
 * Set the start node to the edgenum's node that has only
 * the edgenum as an attached fault edge.
 */
    ep = EdgeList + edgenum;
    n1start = ep->node1;
    n2start = ep->node2;
    lineid = ep->lineid;

    enow = edgenum;

/*
 * Check if the first node has any other edges from this fault connected to it.
 */
    nodenow = n1start;
    list = NodeEdgeList[nodenow].list;
    if (list == NULL) {
        return 0;
    }
    nlist = NodeEdgeList[nodenow].nlist;
    found1 = 0;
    for (i=0; i<nlist; i++) {
        if (list[i] == enow) {
            continue;
        }
        ep = EdgeList + list[i];
        if (ep->deleted == 1) {
            continue;
        }
        if (!(ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
              ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT)) {
            continue;
        }
        if (ep->lineid != lineid) {
            continue;
        }
        if (ep->node1 == nodenow) {
            nodenow = ep->node2;
        }
        else {
            nodenow = ep->node1;
        }
        found1 = 1;
        break;
    }

/*
 * Check if the second node has any other edges from this fault connected to it.
 */
    nodenow = n2start;
    list = NodeEdgeList[nodenow].list;
    if (list == NULL) {
        return 0;
    }
    nlist = NodeEdgeList[nodenow].nlist;
    found2 = 0;
    for (i=0; i<nlist; i++) {
        if (list[i] == enow) {
            continue;
        }
        ep = EdgeList + list[i];
        if (ep->deleted == 1) {
            continue;
        }
    /*
     * BUGFIX !!!!
     * There was a typo which excluded the ! at the start of the if condition.
     */
        if (!(ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
             ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT)) {
            continue;
        }
        if (ep->lineid != lineid) {
            continue;
        }
        if (ep->node1 == nodenow) {
            nodenow = ep->node2;
        }
        else {
            nodenow = ep->node1;
        }
        found2 = 1;
        break;
    }

/*
 * If both nodes have no other edges then the fault is just the single edge.
 */
    if (found1 == 0  &&  found2 == 0) {
        FaultEdges[0] = edgenum;
        FaultNodes[0] = n1start;
        FaultNodes[1] = n2start;
        NumFaultEdges = 1;
        NumFaultNodes = 2;
        return 1;
    }

/*
 * If both nodes have another fault edge, there is a program bug.
 * Assert in this case.
 */
    if (ClosedAllowed == 0) {
        if (found1 == 1  &&  found2 == 1) {
            assert (0);
        }
    }

/*
 * Start at the node that doesn't have another fault edge connected.
 */
    nodestart = -1;
    if (found1 == 0) {
        FaultNodes[0] = n1start;
        FaultNodes[1] = n2start;
        nodenow = n2start;
        nodestart = n2start;
    }
    else if (found2 == 0) {
        FaultNodes[0] = n2start;
        FaultNodes[1] = n1start;
        nodenow = n1start;
        nodestart = n1start;
    }

    FaultEdges[0] = edgenum;
    NumFaultEdges = 1;
    NumFaultNodes = 2;

    ResetTflag2 (0);

/*
 * Traverse from node to node using fault edges until
 * the opposite dead end is found.
 */
    for (;;) {

        list = NodeEdgeList[nodenow].list;
        if (list == NULL) {
            return 0;
        }
        nlist = NodeEdgeList[nodenow].nlist;
        found = 0;
        for (i=0; i<nlist; i++) {
            if (list[i] == enow) {
                continue;
            }
            ep = EdgeList + list[i];
            if (ep->deleted == 1) {
                continue;
            }
            if (!(ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
                  ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT)) {
                continue;
            }
            if (ep->lineid != lineid) {
                continue;
            }
            if (ep->tflag2 == 1) {
                continue;
            }
            ep->tflag2 = 1;
            AddTflag2 (list[i]);
            if (ep->node1 == nodenow) {
                nodenow = ep->node2;
            }
            else {
                nodenow = ep->node1;
            }
            FaultEdges[NumFaultEdges] = list[i];
            FaultNodes[NumFaultNodes] = nodenow;
            NumFaultEdges++;
            NumFaultNodes++;
            found = 1;
            break;
        }

        if (NumFaultNodes >= MAX_CONNECT_EDGES) {
            return -1;
        }

    /*
     * If no additional fault connection is found, we are at the
     * opposite end of the fault line.
     */
        if (found == 0) {
            break;
        }

        if (nodenow == nodestart) {
            break;
        }
        if (nodenow == nodestart) {
            break;
        }

    }

    if (ClosedAllowed == 0) {
        if (found == 1) {
            return -1;
        }
    }

    return 1;

}


/*-----------------------------------------------------------------------------*/

int CSWGrdConstraint::SeparateConstraintEdges (ORientedFaultStruct *ofp,
                                    char *flags)
{
    int                i, j, n1, n2, nedge, ncheck, iedge, itmp,
                       nfirst, nlast, n1new, n2new, prevnew;
    int                n1needed, istat, nextedge;
    double             z1, z2, z3, z4, dtmp;
    EDgeStruct         *ep, *ep2, *epnew;
    NOdeStruct         *np1, *np2, *npnew;
    TRiangleStruct     *tp;
    COnstraintEdge     *cp;
    int                do_write;
    char               fname[200];

    if (NumConstraintEdges < 1) {
        return 1;
    }

    for (i=0; i<NumFaultNodes; i++) {
        np1 = NodeList + FaultNodes[i];
        FaultSplits[i] = np1->splitflag;
        OppositeSplitNodes[i] = -1;
        if (np1->splitflag > 0) {
            ep = EdgeList + np1->splitflag - 1;
            OppositeSplitNodes[i] = ep->node1;
            if (ep->node1 == FaultNodes[i]) {
                OppositeSplitNodes[i] = ep->node2;
            }
        }
    }

    nfirst = FaultNodes[0];
    nlast = FaultNodes[NumFaultNodes - 1];
    ncheck = nfirst;
    prevnew = -1;

    for (i=0; i<NumFaultEdges; i++) {

    /*
     * Find the constraint data for this edge.
     */
        iedge = FaultEdges[i];
        if (i < NumFaultEdges-1) {
            nextedge = FaultEdges[i+1];
        }
        else {
            nextedge = -1;
        }
        cp = NULL;
        for (j=0; j<NumConstraintEdges; j++) {
            cp = ConstraintEdges + j;
            if (cp->index == iedge) {
                break;
            }
        }
        if (cp == NULL) {
            assert (0);
        }

    /*
     * get the elevations for each triangle.
     */
        z1 = cp->t1z1;
        z2 = cp->t1z2;
        z3 = cp->t2z1;
        z4 = cp->t2z2;

    /*
     * Keep the current edge attached to tri1 using the tri1
     * z values.
     */
        ep = EdgeList + iedge;
        n1 = ep->node1;
        n2 = ep->node2;
        if (n2 == ncheck) {
            itmp = n1;
            n1 = n2;
            n2 = itmp;
            dtmp = z1;
            z1 = z2;
            z2 = dtmp;
            dtmp = z3;
            z3 = z4;
            z4 = dtmp;
        }

        np1 = NodeList + n1;
        np2 = NodeList + n2;

        np1->z = z1;
        np2->z = z2;

    /*
     *  DEBUG only  to separate the edges for visualization.
        np1->x += .5;
        np1->y -= .5;
     */

    /*
     * Create a new edge and a new node to use with tri2.
     */

    /*
     * If the original node (n1) is the same as the first
     * or last node in the original fault line, use the
     * original node in the new edge unless the original
     * node is a border node.
     */
        n1needed = 0;
        if (n1 == nfirst) {
            if (np1->on_border != 0  ||  ofp->end_1_flag == 1) {
                n1new = AddNode (np1->x, np1->y, z3, np1->flag);
                if (n1new < 0) {
                    return -1;
                }
    /*
     *  DEBUG only  to separate the edges for visualization.
				NodeList[n1new].x -= .5;
				NodeList[n1new].y += .5;
     */

                np1 = NodeList + n1;
                if (np1->is_intersect) {
                    npnew = NodeList + n1new;
                    npnew->is_intersect = 1;
                    npnew->xorig = np1->xorig;
                    npnew->yorig = np1->yorig;
                    npnew->zorig = np1->zorig;
                    npnew->adjusting_node = np1->adjusting_node;
                }
                n1needed = 1;
                prevnew = n1new;
            }
            else {
                n1new = n1;
            }
        }
        else if (n1 == nlast) {
            if (np1->on_border != 0  ||  ofp->end_2_flag == 1) {
                n1new = AddNode (np1->x, np1->y, z3, np1->flag);
                if (n1new < 0) {
                    return -1;
                }
    /*
     *  DEBUG only  to separate the edges for visualization.
				NodeList[n1new].x -= .5;
				NodeList[n1new].y += .5;
     */

                np1 = NodeList + n1;
                if (np1->is_intersect) {
                    npnew = NodeList + n1new;
                    npnew->is_intersect = 1;
                    npnew->xorig = np1->xorig;
                    npnew->yorig = np1->yorig;
                    npnew->zorig = np1->zorig;
                    npnew->adjusting_node = np1->adjusting_node;
                }
                n1needed = 1;
                prevnew = n1new;
            }
            else {
                n1new = n1;
            }
        }

    /*
     * If the original n1 node is the same as the previously
     * checked node in the FaultEdges array, use the previous
     * added node.
     */
        else if (n1 == ncheck) {
            if (prevnew == -1) {
                assert (0);
            }
            n1new = prevnew;
        }

    /*
     * Add a new n1 node if no previous node can be used.
     */
        else {
            n1new = AddNode (np1->x, np1->y, z3, np1->flag);
            if (n1new < 0) {
                return -1;
            }
    /*
     *  DEBUG only  to separate the edges for visualization.
				NodeList[n1new].x -= .5;
				NodeList[n1new].y += .5;
     */

            np1 = NodeList + n1;
            if (np1->is_intersect) {
                npnew = NodeList + n1new;
                npnew->is_intersect = 1;
                npnew->xorig = np1->xorig;
                npnew->yorig = np1->yorig;
                npnew->zorig = np1->zorig;
                npnew->adjusting_node = np1->adjusting_node;
            }
            prevnew = n1new;
            n1needed = 1;
        }

    /*
     * Make sure any edges connected to n1 that have opposite
     * points on the "new" side of the fault get the n1 node
     * switched to n1new.  The "new" side is the tri2 side for
     * each edge on the fault.
     */
        if (n1needed) {
            istat =
              FixNodeEdgesForOrientation (n1, n1new, flags, iedge, nextedge);
            if (istat == -1) {
                return -1;
            }
        }

    /*
     * Same logic for n2 as n1.
     */
        np1 = NodeList + n1;
        np2 = NodeList + n2;

        if (n2 == nfirst) {
            if (np2->on_border != 0  ||  ofp->end_1_flag == 1) {
                n2new = AddNode (np2->x, np2->y, z3, np2->flag);
                if (n2new < 0) {
                    return -1;
                }
    /*
     *  DEBUG only  to separate the edges for visualization.
				NodeList[n2new].x -= .5;
				NodeList[n2new].y += .5;
     */

                np2 = NodeList + n2;
                if (np2->is_intersect) {
                    npnew = NodeList + n2new;
                    npnew->is_intersect = 1;
                    npnew->xorig = np2->xorig;
                    npnew->yorig = np2->yorig;
                    npnew->zorig = np2->zorig;
                    npnew->adjusting_node = np2->adjusting_node;
                }
                prevnew = n2new;
            }
            else {
                n2new = n2;
            }
        }
        else if (n2 == nlast) {
            if (np2->on_border != 0  ||  ofp->end_2_flag == 1) {
                n2new = AddNode (np2->x, np2->y, z3, np2->flag);
                if (n2new < 0) {
                    return -1;
                }
    /*
     *  DEBUG only  to separate the edges for visualization.
				NodeList[n2new].x -= .5;
				NodeList[n2new].y += .5;
     */

                np2 = NodeList + n2;
                if (np2->is_intersect) {
                    npnew = NodeList + n2new;
                    npnew->is_intersect = 1;
                    npnew->xorig = np2->xorig;
                    npnew->yorig = np2->yorig;
                    npnew->zorig = np2->zorig;
                    npnew->adjusting_node = np2->adjusting_node;
                }
                prevnew = n2new;
            }
            else {
                n2new = n2;
            }
        }
        else if (n2 == ncheck) {
            if (prevnew == -1) {
                assert (0);
            }
            n2new = prevnew;
        }

        else {
            n2new = AddNode (np2->x, np2->y, z4, np2->flag);
            if (n2new < 0) {
                return -1;
            }
    /*
     *  DEBUG only  to separate the edges for visualization.
				NodeList[n2new].x -= .5;
				NodeList[n2new].y += .5;
     */

            np2 = NodeList + n2;
            if (np2->is_intersect) {
                npnew = NodeList + n2new;
                npnew->is_intersect = 1;
                npnew->xorig = np2->xorig;
                npnew->yorig = np2->yorig;
                npnew->zorig = np2->zorig;
                npnew->adjusting_node = np2->adjusting_node;
            }
            prevnew = n2new;
        }

    /*
     * Make sure any edges connected to n2 that have opposite
     * points on the right side of the segment get the n2 node
     * switched to n2new.
     */
        istat =
          FixNodeEdgesForOrientation (n2, n2new, flags, iedge, nextedge);
        if (istat == -1) {
            return -1;
        }

        ncheck = FaultNodes[i+1];

    /*
     * If the tri2 triangle exists, fix it to use the edge connected
     * with the rest of the tri2 edges.
     */
        ep = EdgeList + iedge;
        if (ep->tri2 >= 0) {
            nedge = AddEdge (n1new, n2new, ep->tri2, -1, ep->flag, ep->length);
            if (nedge < 0) {
                return -1;
            }
            if (n1 == nfirst  ||  n1 == nlast) {
                NodeList[n1].splitflag = nedge + 1;
            }
            if (n2 == nfirst  ||  n2 == nlast) {
                NodeList[n2].splitflag = nedge + 1;
            }
            AddToNodeEdgeList (nedge, n1new);
            AddToNodeEdgeList (nedge, n2new);

            ep = EdgeList + iedge;
            ep->pairid = nedge + 1;
            epnew = EdgeList + nedge;
            epnew->pairid = iedge + 1;
            epnew->lineid = ep->lineid;
            epnew->lineid2 = ep->lineid2;
            epnew->on_border = ep->on_border;
            epnew->isconstraint = ep->isconstraint;

        /*
         * Fix the tri2 triangle to use the new edge and nodes.
         */
            tp = TriList + ep->tri2;
            ep2 = ep;

            if (tp->edge1 != iedge) {
                ep = EdgeList + tp->edge1;
                if (ep->node1 == n1) {
                    ep->node1 = n1new;
                }
                else if (ep->node2 == n1) {
                    ep->node2 = n1new;
                }
                else if (ep->node1 == n2) {
                    ep->node1 = n2new;
                }
                else if (ep->node2 == n2) {
                    ep->node2 = n2new;
                }
            }

            if (tp->edge2 != iedge) {
                ep = EdgeList + tp->edge2;
                if (ep->node1 == n1) {
                    ep->node1 = n1new;
                }
                else if (ep->node2 == n1) {
                    ep->node2 = n1new;
                }
                else if (ep->node1 == n2) {
                    ep->node1 = n2new;
                }
                else if (ep->node2 == n2) {
                    ep->node2 = n2new;
                }
            }

            if (tp->edge3 != iedge) {
                ep = EdgeList + tp->edge3;
                if (ep->node1 == n1) {
                    ep->node1 = n1new;
                }
                else if (ep->node2 == n1) {
                    ep->node2 = n1new;
                }
                else if (ep->node1 == n2) {
                    ep->node1 = n2new;
                }
                else if (ep->node2 == n2) {
                    ep->node2 = n2new;
                }
            }
            ep = ep2;

            if (tp->edge1 == iedge) {
                tp->edge1 = nedge;
            }
            else if (tp->edge2 == iedge) {
                tp->edge2 = nedge;
            }
            else if (tp->edge3 == iedge) {
                tp->edge3 = nedge;
            }
        }

    /*
     * Reset the original edge's tri2 to -1.  If the orignal edge
     * was on the trimesh border and its tri1 is non existant,
     * mark the original edge as deleted.
     */
        ep->tri2 = -1;
        ep->tflag = 1;

        if (ep->tri1 == -1) {
            RemoveFromNodeEdgeList (ep-EdgeList, ep->node1);
            RemoveFromNodeEdgeList (ep-EdgeList, ep->node2);
            ep->deleted = 1;
        }

    }

    FixForSplits ();

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        sprintf (fname, "post_sep_constraint.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
        grd_triangle_ptr->grd_validate_topology
           (NodeList, NumNodes,
            EdgeList, NumEdges,
            TriList, NumTris,
            (char *)"from post sep constraint");
    }


    return 1;
}


/*-----------------------------------------------------------------------------*/

int CSWGrdConstraint::FixLineOrientation (void)
{
    int                  i, n1, n2, e1, t1, t2, nopp, itmp;
    int                  *list, nlist, j, lastnode, nextnode, bflag;
    EDgeStruct           *ep;
    NOdeStruct           *np1, *np2, *npopp;
    double               x1, y1, x2, y2, xopp, yopp,
                         ax1, ay1, ax2, ay2, cpz;
    TRiangleStruct       *tp;
    NOdeEdgeListStruct   *neptr;

/*
 * Loop through the FaultEdges list.  For each edge, if the opposite
 * node of the tri1 triangle is to the left of the edge, swap the tri1
 * and tri2 members of the edge.
 */
    for (i=0; i<NumFaultEdges; i++) {
        n1 = FaultNodes[i];
        n2 = FaultNodes[i+1];
        np1 = NodeList + n1;
        np2 = NodeList + n2;

    /*
     * endpoint x and y of the edge.
     */
        x1 = np1->x;
        y1 = np1->y;
        y2 = np2->y;
        x2 = np2->x;

    /*
     * Get the triangles that share the edge.
     */
        e1 = FaultEdges[i];
        ep = EdgeList + e1;
        t1 = ep->tri1;
        t2 = ep->tri2;

    /*
     * get the x and y of the opposite node for tri1
     */
        tp = TriList + t1;
        nopp = OppositeNode (tp, FaultEdges[i]);
        npopp = NodeList + nopp;
        xopp = npopp->x;
        yopp = npopp->y;

    /*
     * Create a vector joining the opposite node with the first edge node.
     * Calculate the cross product of that vector and the vector from the
     * first edge node to the second edge node.  If the cross product is
     * negative, the first triangle attached to the edge is on the left
     * and needs to be swapped with the second triangle attached to the
     * edge.
     */
        ax1 = xopp - x1;
        ay1 = yopp - y1;
        ax2 = x2 - x1;
        ay2 = y2 - y1;
        cpz = ax1 * ay2 - ay1 * ax2;
        if (cpz < 0) {
            itmp = t1;
            t1 = t2;
            t2 = itmp;
            ep->tri1 = t1;
            ep->tri2 = t2;
        }

     /*
      * Flag the opposite node of tri1 by setting bflag to 1
      * and the opposite node of tri2 by setting its bflag to
      * -1.  These are used later to connect edges to the correct
      * side of the fault.
      */
        if (ep->tri1 >= 0) {
            tp = TriList + ep->tri1;
            nopp = OppositeNode (tp, FaultEdges[i]);
            npopp = NodeList + nopp;
            SetBflag (nopp, FaultEdges[i], 1);
        }
        if (ep->tri2 >= 0) {
            tp = TriList + ep->tri2;
            nopp = OppositeNode (tp, FaultEdges[i]);
            npopp = NodeList + nopp;
            SetBflag (nopp, FaultEdges[i], -1);
        }
    }

/*
 * Check all the edges attached to the line interior nodes.  If they
 * do not have the flags set on their opposite nodes, set them.
 */
    for (i=1; i<NumFaultNodes-1; i++) {
        n1 = FaultNodes[i];
        neptr = NodeEdgeList + n1;
        np1 = NodeList + n1;
        list = neptr->list;
        nlist = neptr->nlist;
        lastnode = FaultNodes[i-1];
        nextnode = FaultNodes[i+1];
        x1 = np1->x;
        y1 = np1->y;
        np1 = NodeList + nextnode;
        x2 = np1->x;
        y2 = np1->y;
        for (j=0; j<nlist; j++) {
            ep = EdgeList + list[j];
            n2 = ep->node1;
            if (n2 == n1) {
                n2 = ep->node2;
            }
            if (n2 == lastnode  ||  n2 == nextnode) {
                continue;
            }
            bflag = GetBflag (n2, FaultEdges[i]);
            if (bflag != 0) {
                continue;
            }
            np2 = NodeList + n2;
            xopp = np2->x;
            yopp = np2->y;
            ax1 = xopp - x1;
            ay1 = yopp - y1;
            ax2 = x2 - x1;
            ay2 = y2 - y1;
            cpz = ax1 * ay2 - ay1 * ax2;
            if (cpz >= 0.0) {
                bflag = 1;
            }
            else {
                bflag = -1;
            }
            SetBflag (n2, FaultEdges[i], bflag);
        }
    }

    return 1;

}


/*------------------------------------------------------------------------------------*/

/*
 * Add a node to the list of nodes currently in the triangulation.  Expand
 * the NodeList array as needed.
 */

int CSWGrdConstraint::AddNode (double x, double y, double z, int flag)

{
    NOdeStruct           *nptr;
    int                  size;

    if (NumNodes >= MaxNodes  ||  NodeList == NULL) {
        MaxNodes += 100;
        NodeList = (NOdeStruct *)csw_Realloc
        (NodeList, MaxNodes * sizeof(NOdeStruct));
        if (NodeList == NULL) {
            return -1;
        }
        size = (MaxNodes - NumNodes) * sizeof(NOdeStruct);
        memset (NodeList + NumNodes, 0, size);
    }

    nptr = NodeList + NumNodes;
    nptr->x = x;
    nptr->y = y;
    nptr->z = z;
    nptr->xorig = 1.e30;
    nptr->yorig = 1.e30;
    nptr->zorig = 1.e30;
    nptr->flag = flag;
    nptr->rp = -1;
    nptr->crp = -1;
    nptr->spillnum = -1;
    nptr->splitflag = 0;
    nptr->adjusting_node = -1;
    nptr->deleted = 0;
    nptr->shifted = 0;
    nptr->on_border = 0;
    nptr->is_locked = 0;
    nptr->is_intersect = 0;

    NumNodes++;

    return nptr - NodeList;

}  /*  end of private AddNode function  */




/*------------------------------------------------------------------------------*/

/*
 *  Add a new edge the the list of edges in the triangulation.  The list is
 *  expanded if needed to hold the new edge.
 */

int CSWGrdConstraint::AddEdge (int n1, int n2, int t1, int t2, int flag, double length)

{
    EDgeStruct           *eptr;
    int                  size;

    if (NumEdges >= MaxEdges  ||  EdgeList == NULL) {
        MaxEdges += 100;
        EdgeList = (EDgeStruct *)csw_Realloc
        (EdgeList, MaxEdges * sizeof(EDgeStruct));
        if (EdgeList == NULL) {
            return -1;
        }
        size = (MaxEdges - NumEdges) * sizeof(EDgeStruct);
        memset (EdgeList + NumEdges, 0, size);
    }

    eptr = EdgeList + NumEdges;
    eptr->node1 = n1;
    eptr->node2 = n2;
    eptr->tri1 = t1;
    eptr->tri2 = t2;
    eptr->flag = flag;
    eptr->number = -1;
    eptr->deleted = 0;
    eptr->tflag = 0;
    eptr->length = length;
    eptr->lineid = -1;

    NumEdges++;

    return eptr - EdgeList;

}  /*  end of private AddEdge function  */


/*-------------------------------------------------------------------------------*/

int CSWGrdConstraint::AddTriangle (int e1, int e2, int e3, int flag)
{
    TRiangleStruct      *tptr;
    int                 istat;

    if (NumTris >= MaxTris) {
        istat = ExpandMem2();
        if (istat == -1) {
            return -1;
        }
    }

    tptr = TriList + NumTris;
    tptr->edge1 = e1;
    tptr->edge2 = e2;
    tptr->edge3 = e3;
    tptr->flag = flag;
    tptr->deleted = 0;

    NumTris++;

    return tptr - TriList;

}  /*  end of private AddTriangle function  */


/*--------------------------------------------------------------------------------*/

/*
 * Put the current FaultEdges and FaultNodes into the next OrientedFault
 * structure.  Expand the OrientedFaults list if needed.
 */

int CSWGrdConstraint::AddOrientedFault (void)
{
    ORientedFaultStruct    *ofp = NULL;
    int                    i, ntot, size;
    EDgeStruct             *eptr = NULL;

    bool           bsuccess = false;


    auto fscope = [&]()
    {
        if (bsuccess == false) {
            if (ofp != NULL) {
                csw_Free (ofp->nodes);
                ofp->nodes = NULL;
            }
        }
    };
    CSWScopeGuard func_scope_guard (fscope);


    if (NumFaultNodes < 2  ||  NumFaultEdges < 1) {
        return 1;
    }

    if (NumOrientedFaults >= MaxOrientedFaults  ||  OrientedFaults == NULL) {
        MaxOrientedFaults += 10;
        OrientedFaults = (ORientedFaultStruct *) csw_Realloc
            (OrientedFaults, MaxOrientedFaults * sizeof(ORientedFaultStruct));
        if (OrientedFaults == NULL) {
            return -1;
        }
        size = MaxOrientedFaults - NumOrientedFaults;
        size *= sizeof(ORientedFaultStruct);
        memset (OrientedFaults + NumOrientedFaults, 0, size);
    }

    if (OrientedFaults == NULL) {
        return -1;
    }

    ofp = OrientedFaults + NumOrientedFaults;
    ntot = NumFaultNodes + NumFaultEdges;
    ofp->nodes = (int *)csw_Malloc (ntot * sizeof(int));
    if (ofp->nodes == NULL) {
        return -1;
    }

    ofp->edges = ofp->nodes + NumFaultNodes;
    ofp->num_nodes = NumFaultNodes;
    ofp->num_edges = NumFaultEdges;

    memcpy (ofp->nodes, FaultNodes, NumFaultNodes * sizeof(int));
    memcpy (ofp->edges, FaultEdges, NumFaultEdges * sizeof(int));

    for (i=0; i<NumFaultEdges; i++) {
        eptr = EdgeList + FaultEdges[i];
        eptr->lineid2 = NumOrientedFaults;
    }

    NumOrientedFaults++;

    bsuccess = true;

    return 1;

}



/*-------------------------------------------------------------------------*/

void CSWGrdConstraint::FreeMem (void)
{
    int                  i;
    ORientedFaultStruct  *ofp = NULL;

    FreeNodeTriList ();
    FreeNodeEdgeList ();

    if (OrientedFaults != NULL) {
        for (i=0; i<NumOrientedFaults; i++) {
            ofp = OrientedFaults + i;
            csw_Free (ofp->nodes);
        }
    }
    csw_Free (OrientedFaults);
    OrientedFaults = NULL;
    NumOrientedFaults = 0;
    MaxOrientedFaults = 0;

    csw_Free (ConstraintEdges);
    ConstraintEdges = NULL;
    NumConstraintEdges = 0;
    MaxConstraintEdges = 0;

    csw_Free (BflagList);
    BflagList = NULL;

    return;

}



/*-------------------------------------------------------------------------*/

void CSWGrdConstraint::ResetTflag2 (int ival)
{
    int          i;
    EDgeStruct   *ep;

    for (i=0; i<NumTflag2List; i++) {
        ep = EdgeList + Tflag2List[i];
        ep->tflag2 = (char)ival;
    }

    return;

}


int CSWGrdConstraint::AddTflag2 (int edgenum)
{
    if (NumTflag2List >= MAX_CONNECT_EDGES) {
        return -1;
    }

    Tflag2List[NumTflag2List] = edgenum;
    NumTflag2List++;

    return 1;

}


void CSWGrdConstraint::ShowBadEdges (void)
{
    int           i, do_write;
    EDgeStruct    *ep;

    do_write = csw_GetDoWrite ();
    if (do_write == 0) {
        return;
    }

    for (i=0; i<NumEdges; i++) {
        ep = EdgeList + i;
        if (ep->deleted == 1) {
            continue;
        }
        if (ep->tri1 < 0) {
            printf ("bad edge number %d, tri1 < zero\n", i);
        }
    }

    return;
}



/*--------------------------------------------------------------------*/

/*
 * If the opposite node of an edge attached to nodenum is on the
 * right side of the vector defined by n1 to n2, then switch the
 * nodenum node in the edge to newnodenum.
 */
int CSWGrdConstraint::FixNodeEdgesForOrientation (int nodenum,
                                       int newnodenum,
                                       char *flags,
                                       int constraint_edge_num,
                                       int next_constraint_edge_num)
{
    int                  i, j, nopp;
    EDgeStruct           *ep, *ep2;
    int                  *list, nlist;
    int                  done, needed, nopp2;
    int                  *list2, nlist2;
    int                  *addedges, *addnodes;
    int                  *remedges, *remnodes;
    int                  numadd, numrem;
    int                  do_write;
    char                 fname[100];

    if (nodenum == newnodenum) {
        return 1;
    }

    addedges = p_i1000;
    addnodes = p_i1001;
    remedges = p_i1002;
    remnodes = p_i1003;

  /*
   * The new node number must have the same bflag as its original.
   * This allows faults very close to each other to be split
   * properly.  This is part of the bug 9196 fix.
   */
    CopyAllBflags (nodenum, newnodenum);

  /*
   * Get the list of edges attached to the nodenum node.
   */
    list = NodeEdgeList[nodenum].list;
    nlist = NodeEdgeList[nodenum].nlist;

    if (list == NULL  ||  nlist < 1) {
        return 1;
    }

    if (nlist > 1000) {
        return -1;
    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        sprintf (fname, "fix_node_edges_1.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    memset (flags, 0, NumFlags * sizeof(char));

/*
 * Check for the opposite node being on the "tri2" side of the line.
 */
    needed = 0;
    done = 0;
    numadd = 0;
    numrem = 0;
    for (i=0; i<nlist; i++) {

        if (list[i] == constraint_edge_num  ||
            list[i] == next_constraint_edge_num) {
            continue;
        }

        ep = EdgeList + list[i];
        if (
/*
            ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
            ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT  ||
*/
            ep->deleted == 1) {
            flags[list[i]] = (char)1;
            continue;
        }

        nopp = ep->node1;
        if (nopp == nodenum) {
            nopp = ep->node2;
        }
        if (GetBflag (nopp, constraint_edge_num) == -1)
        {
            if (ep->node1 == nodenum) {
                ep->node1 = newnodenum;
            }
            else {
                ep->node2 = newnodenum;
            }
            if (ep->flag != 0) {
                ChangeOrientedFaultNode (ep->lineid,
                                         nodenum,
                                         newnodenum);
            }
            remedges[numrem] = list[i];
            remnodes[numrem] = nodenum;
            addedges[numadd] = list[i];
            addnodes[numadd] = newnodenum;
            numrem++;
            numadd++;
            if (numrem > 1000) {
                return -1;
            }
            flags[list[i]] = (char)1;
        }
        else if (GetBflag (nopp, constraint_edge_num) == 1) {
            flags[list[i]] = (char)1;
        }
        else {
            needed = 1;
        }
    }

    for (i=0; i<numrem; i++) {
        RemoveFromNodeEdgeList (remedges[i], remnodes[i]);
    }
    for (i=0; i<numadd; i++) {
        AddToNodeEdgeList (addedges[i], addnodes[i]);
    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        sprintf (fname, "fix_node_edges_2.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    if (needed == 0) {
        return 1;
    }

    list = NodeEdgeList[nodenum].list;
    nlist = NodeEdgeList[nodenum].nlist;

/*
 * If any edges were not assignable via the previous loop, then
 * the edges need to be assigned based on their neighboring edges
 * in the triangle fan around the fault node.
 */
    numrem = 0;
    numadd = 0;
    for (;;) {

        done = 0;
        needed = 0;
        for (i=0; i<nlist; i++) {

            if (list[i] == constraint_edge_num) {
                continue;
            }

        /*
         * If the edge has already been done, skip it.
         */
            if (flags[list[i]] == 1) {
                done++;
                continue;
            }

        /*
         * Find the edge list for the opposite node of this edge.
         */
            ep = EdgeList + list[i];
            nopp = ep->node1;
            if (nopp == nodenum) {
                nopp = ep->node2;
            }
            list2 = NodeEdgeList[nopp].list;
            nlist2 = NodeEdgeList[nopp].nlist;

         /*
          * Find the opposite nodes of each edge in this list.
          * If any have bflag set to non zero, use the same
          * bflag value for the nopp node.
          */
            for (j=0; j<nlist2; j++) {
                ep2 = EdgeList + list2[j];
                if (ep2->deleted == 1) {
                    continue;
                }
                nopp2 = ep2->node1;
                if (nopp2 == nopp) {
                    nopp2 = ep2->node2;
                }
                if (nopp2 == nodenum) {
                    continue;
                }
                if (GetBflag (nopp2, constraint_edge_num) != 0) {
                    CopyBflag (nopp2, nopp, constraint_edge_num);
                    done++;
                    needed = 1;
                    if (GetBflag (nopp, constraint_edge_num) == -1) {
                        if (ep->node1 == nodenum) {
                            ep->node1 = newnodenum;
                        }
                        else {
                            ep->node2 = newnodenum;
                        }
                        flags[list[i]] = (char)1;
                        remedges[numrem] = list[i];
                        remnodes[numrem] = nodenum;
                        addedges[numadd] = list[i];
                        addnodes[numadd] = newnodenum;
                        numrem++;
                        numadd++;
                        if (numrem > 1000) {
                            return -1;
                        }
                    }
                    else if (GetBflag (nopp, constraint_edge_num) == 1) {
                        flags[list[i]] = (char)1;
                    }
                    break;
                }
            }
        }

    /*
     * If all the edges connected to the original fault node
     * are done, break out of the loop.
     */
        if (done >= nlist) {
            break;
        }

    /*
     * If all are not done, and no edge was processed in this
     * iteration, break out of the loop.
     */
    /*
     * BUGFIX !!!!
     * This used to assert, but now it should just break out of the loop.
     */
        if (needed == 0) {
            break;
        }
    }

    for (i=0; i<numrem; i++) {
        RemoveFromNodeEdgeList (remedges[i], remnodes[i]);
    }
    for (i=0; i<numadd; i++) {
        AddToNodeEdgeList (addedges[i], addnodes[i]);
    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        sprintf (fname, "fix_node_edges_3.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    return 1;

}

/*--------------------------------------------------------------------------------*/

void CSWGrdConstraint::WriteOrientedFaults (void)
{
    int                    do_write;
    ORientedFaultStruct    *ofp;
    int                    i, j, k, n, npts;
    double                 *x, *y, *z;
    int                    *icomp, *ivec;
    char                   fname[100];

    x = p_d10000;
    y = p_d10001;
    z = p_d10002;
    icomp = p_i1000;
    ivec = p_i1001;

    if (OrientedFaults == NULL) {
        return;
    }

    do_write = csw_GetDoWrite ();
    if (do_write == 0) {
        return;
    }

    n = 0;
    for (i=0; i<NumOrientedFaults; i++) {
        ofp = OrientedFaults + i;
        npts = ofp->num_nodes;
        for (j=0; j<npts; j++) {
            k = ofp->nodes[j];
            x[n] = NodeList[k].x;
            y[n] = NodeList[k].y;
            z[n] = NodeList[k].z;
            n++;
            if (n > 9999) n = 9999;
        }
        ivec[i] = npts;
        icomp[i] = 1;
    }

    strcpy (fname, "orientedfaults.xyz");
    grd_triangle_ptr->grd_WriteLines (x, y, z,
                    NumOrientedFaults, icomp, ivec,
                    fname);
}


/*-------------------------------------------------------------*/

/*
 * It is possible, if vertical fault traces are close to each other
 * relative to the trimesh nominal edge length and also if the same
 * vertical fault traces curve back tightly on themselves, that some
 * nodes on the fault may not have z values assigned.  This type of
 * situation cannot be handled well, but I need to replace these
 * NULL values with something.  In this function, I trace through the
 * NodeEdge list for the NULL node until a non null value is found.
 * This value is used.  This is a case of garbage in, garbage out,
 * but hopefully not garbage in, crash out.
 */
int CSWGrdConstraint::FillInNullNodes (void)
{
    int           i;
    int           nbad, ngood;
    double        zval, zavg;

/*
 * See if any null nodes exist.
 */
    nbad = 0;
    for (i=0; i<NumNodes; i++) {
        if (NodeList[i].deleted == 1) continue;
        if (NodeList[i].z > 1.e20) {
            nbad++;
            break;
        }
    }

    if (nbad == 0) {
        return 1;
    }

/*
 * Find the average value of all "good" nodes.  This will be
 * used if no "good" node is found connected to the "bad" node.
 */
    zavg = 0.0;
    ngood = 0;

    for (i=0; i<NumNodes; i++) {
        if (NodeList[i].deleted == 1) continue;
        if (NodeList[i].z < 1.e20) {
            zavg += NodeList[i].z;
            ngood++;
        }
    }
    if (ngood > 0) {
        zavg /= ngood;
    }
    else {
        zavg = 0.0;
    }

/*
 * Attempt to find a valid z value within 2 levels of each bad node.
 * A level is all the opposite nodes of edges attached to a node.
 */
    for (i=0; i<NumNodes; i++) {
        if (NodeList[i].deleted == 1) continue;
        if (NodeList[i].z > 1.e20) {
            zval = SearchTwoLevels (i);
            if (zval > 1.e20) {
                zval = zavg;
            }
            NodeList[i].z = zval;
        }
    }

    return 1;

}

double CSWGrdConstraint::SearchTwoLevels (int nodenum)
{
    int           i, nlist, *list;
    int           j, nlist2, *list2;
    int           nopp, nopp2;
    double        zval;
    EDgeStruct    *ep, *ep2;

    zval = 1.e30;

    list = NodeEdgeList[nodenum].list;
    nlist = NodeEdgeList[nodenum].nlist;

    if (list == NULL  ||  nlist < 1) {
        return zval;
    }

/*
 * Search the closest neighbors.
 */
    for (i=0; i<nlist; i++) {
        ep = EdgeList + list[i];
        if (ep->deleted == 1) {
            continue;
        }
        nopp = ep->node1;
        if (nopp == nodenum) {
            nopp = ep->node2;
        }
        zval = NodeList[nopp].z;
        if (zval < 1.e20) {
            return zval;
        }
    }

/*
 * Search the closest neighbors of the closest neighbors.
 */
    for (i=0; i<nlist; i++) {
        ep = EdgeList + list[i];
        if (ep->deleted == 1) {
            continue;
        }
        nopp = ep->node1;
        if (nopp == nodenum) {
            nopp = ep->node2;
        }
        list2 = NodeEdgeList[nopp].list;
        nlist2 = NodeEdgeList[nopp].nlist;
        if (list == NULL  ||  nlist < 1) {
            continue;
        }
        for (j=0; j<nlist2; j++) {
            if (list[i] == list2[j]) continue;
            ep2 = EdgeList + list2[j];
            nopp2 = ep2->node1;
            if (nopp2 == nodenum) {
                nopp2 = ep2->node2;
            }
            zval = NodeList[nopp2].z;
            if (zval < 1.e20) {
                return zval;
            }
        }
    }

    return zval;

}


/*----------------------------------------------------------------------*/

/*
 * When faults are close together, a node may be on the right side of one fault
 * but on the left side of other faults.  I support up to 4 faults that can
 * be neighbors of the point now.  This is part of the bug_9196 fix.  All the
 * BflagList functions below are used to keep track of all this.
 */
void CSWGrdConstraint::SetBflag (int node, int edge, int val)
{
    BFlagStruct    *bfp;
    int            i, next, lineid;

    lineid = EdgeList[edge].lineid2;

    bfp = BflagList + node;
    for (i=0; i<4; i++) {
        if (bfp->lineid[i] == lineid) {
            bfp->val[i] = (char)val;
            return;
        }
    }

    next = bfp->next;
    bfp->lineid[next] = lineid;
    bfp->val[next] = (char)val;
    bfp->next++;

    return;

}

int CSWGrdConstraint::GetBflag (int node, int edge)
{
    int           i, lineid;
    BFlagStruct   *bfp;

    lineid = EdgeList[edge].lineid2;

    bfp = BflagList + node;
    for (i=0; i<4; i++) {
        if (bfp->lineid[i] == lineid) {
            return (bfp->val[i]);
        }
    }

    return 0;
}

void CSWGrdConstraint::CopyAllBflags (int node1, int node2)
{
    BFlagStruct     *bfp1, *bfp2;

    if (node1 == node2) {
        return;
    }

    bfp1 = BflagList + node1;
    bfp2 = BflagList + node2;

    memcpy (bfp2, bfp1, sizeof(BFlagStruct));

    return;
}

void CSWGrdConstraint::CopyBflag (int node1, int node2, int edge)
{
    int                i, lineid, val1, next;
    BFlagStruct        *bfp1, *bfp2;

    if (node1 == node2) {
        return;
    }

    bfp1 = BflagList + node1;
    bfp2 = BflagList + node2;

    lineid = EdgeList[edge].lineid2;

    val1 = 99;
    for (i=0; i<4; i++) {
        if (bfp1->lineid[i] == lineid) {
            val1 = bfp1->val[i];
            break;
        }
    }

    if (val1 == 99) {
        return;
    }

    for (i=0; i<4; i++) {
        if (bfp2->lineid[i] == lineid) {
            bfp2->val[i] = (char)val1;
            return;
        }
    }

    next = bfp2->next;
    bfp2->lineid[next] = lineid;
    bfp2->val[next] = (char)val1;

    bfp2->next++;

    return;

}


int CSWGrdConstraint::SamePoint (double x1, double y1, double x2, double y2)

{
    double       dx, dy, dt;

    dx = x1 - x2;
    dy = y1 - y2;
    dt = dx * dx + dy * dy;
    dt = sqrt (dt);
    if (dt <= Tiny) {
        return 1;
    }
    return 0;

}  /*  end of private SamePoint function  */



/*-----------------------------------------------------------------------------*/

int CSWGrdConstraint::SplitIntoFour (int t1in, int t2in, int e1in, int n1in,
                          int *elist)
{
    TRiangleStruct     *tp1, *tp2;
    TRiangleStruct     *tp3, *tp4;
    EDgeStruct         *ep;

    int                nc, n1, n2, n3, n4,
                       e1, e2, e3, e4,
                       t1, t2, t3, t4,
                       e13, e23, e24, e14;

    tp1 = TriList + t1in;
    tp2 = TriList + t2in;
    ep = EdgeList + e1in;

    t1 = t1in;
    t2 = t2in;
    nc = n1in;
    n1 = ep->node1;
    n2 = ep->node2;
    ep->node2 = nc;
    ep->length = NodeDistance(nc, n1);
    n3 = OppositeNode (tp1, ep-EdgeList);
    if (t2in < 0) {
        n4 = -1;
    }
    else {
        n4 = OppositeNode (tp2, ep-EdgeList);
    }

    e1 = ep - EdgeList;
    e2 = AddEdge (n2, nc, -1, -1, 0, NodeDistance(n2, nc));
    e3 = AddEdge (n3, nc, -1, -1, 0, NodeDistance(n3, nc));
    EdgeList[e3].number = 0;
    EdgeList[e3].lineid = 0;

    if (n4 >= 0) {
        e4 = AddEdge (n4, nc, -1, -1, 0, NodeDistance (n4, nc));
        if (e4 < 0) {
            return -1;
        }
    }
    else {
        e4 = -1;
    }

    e13 = FindTriangleEdge (tp1, n1, n3);
    e23 = FindTriangleEdge (tp1, n2, n3);
    t3 = AddTriangle (e23, e2, e3, 0);
    if (n4 >= 0) {
        e24 = FindTriangleEdge (tp2, n2, n4);
        e14 = FindTriangleEdge (tp2, n1, n4);
        t4 = AddTriangle (e24, e2, e4, 0);
    }
    else {
        t4 = -1;
        e24 = -1;
        e14 = -1;
    }

    tp1 = TriList + t1;
    tp3 = TriList + t3;

    if (t2 >= 0  &&  t4 >= 0) {
        tp2 = TriList + t2;
        tp4 = TriList + t4;
    }
    else {
        tp2 = NULL;
        tp4 = NULL;
    }

    tp1->edge1 = e13;
    tp1->edge2 = e1;
    tp1->edge3 = e3;

    if (tp2 != NULL) {
        tp2->edge1 = e14;
        tp2->edge2 = e1;
        tp2->edge3 = e4;
    }

    if (tp2 != NULL  &&  tp4 != NULL) {
        if (EdgeList[e24].tri1 == tp2-TriList) {
            EdgeList[e24].tri1 = tp4 - TriList;
        }
        else {
            EdgeList[e24].tri2 = tp4 - TriList;
        }
    }

    if (EdgeList[e23].tri1 == tp1-TriList) {
        EdgeList[e23].tri1 = tp3 - TriList;
    }
    else {
        EdgeList[e23].tri2 = tp3 - TriList;
    }

    EdgeList[e2].tri1 = tp3 - TriList;
    EdgeList[e3].tri1 = tp1 - TriList;
    EdgeList[e3].tri2 = tp3 - TriList;

    if (tp2 != NULL  &&  tp4 != NULL) {
        EdgeList[e2].tri2 = tp4 - TriList;
        EdgeList[e4].tri1 = tp2 - TriList;
        EdgeList[e4].tri2 = tp4 - TriList;
    }

    elist[0] = e4;
    elist[1] = e2;
    elist[2] = e3;
    elist[3] = e1;

    RemoveFromNodeEdgeList (e1, n2);
    AddToNodeEdgeList (e2, n2);
    AddToNodeEdgeList (e3, n3);

    if (n4 >= 0) {
        AddToNodeEdgeList (e4, n4);
    }

    return 1;

}  /*  end of private SplitIntoFour function  */


/*---------------------------------------------------------------------*/

double CSWGrdConstraint::NodeDistance (int n1, int n2)
{
    NOdeStruct         *nptr;
    double             x1, y1, x2, y2, dx, dy, dt, dist;

    nptr = NodeList + n1;
    x1 = nptr->x;
    y1 = nptr->y;
    nptr = NodeList + n2;
    x2 = nptr->x;
    y2 = nptr->y;

    dx = x2 - x1;
    dy = y2 - y1;
    dt = dx * dx + dy * dy;
    dist = sqrt(dt);

    return dist;

}  /*  end of private NodeDistance function  */



/*------------------------------------------------------------------------------*/

int CSWGrdConstraint::FindTriangleEdge (TRiangleStruct *tp1, int n1, int n2)
{
    EDgeStruct            *ep;

    ep = EdgeList + tp1->edge1;
    if (ep->node1 == n1  &&  ep->node2 == n2) {
        return tp1->edge1;
    }
    if (ep->node1 == n2  &&  ep->node2 == n1) {
        return tp1->edge1;
    }

    ep = EdgeList + tp1->edge2;
    if (ep->node1 == n1  &&  ep->node2 == n2) {
        return tp1->edge2;
    }
    if (ep->node1 == n2  &&  ep->node2 == n1) {
        return tp1->edge2;
    }

    ep = EdgeList + tp1->edge3;
    if (ep->node1 == n1  &&  ep->node2 == n2) {
        return tp1->edge3;
    }
    if (ep->node1 == n2  &&  ep->node2 == n1) {
        return tp1->edge3;
    }

    return -1;

}  /*  end of private FindTriangleEdge function  */

/*--------------------------------------------------------------------------*/

int CSWGrdConstraint::ExpandMem2 (void)

{
    EDgeStruct            *enew, *eptr;
    TRiangleStruct        *tnew, *tptr;
    NOdeStruct            *nnew, *nptr;
    int                   nsize, oldmax;

    oldmax = MaxNodes;
    if (NumNodes >= MaxNodes) {
        MaxNodes += 100;
        nnew = (NOdeStruct *)csw_Realloc (NodeList, MaxNodes * sizeof(NOdeStruct));
        if (!nnew) {
            FreeMem();
            return -1;
        }
        NodeList = nnew;
        nptr = NodeList + oldmax;
        nsize = (MaxNodes - oldmax) * sizeof(NOdeStruct);
        memset (nptr, 0, nsize);
    }

    oldmax = MaxEdges;
    if (NumEdges >= MaxEdges) {
        MaxEdges += 100;
        enew = (EDgeStruct *)csw_Realloc (EdgeList, MaxEdges * sizeof(EDgeStruct));
        if (!enew) {
            FreeMem();
            return -1;
        }
        EdgeList = enew;
        eptr = EdgeList + oldmax;
        nsize = (MaxEdges - oldmax) * sizeof(EDgeStruct);
        memset (eptr, 0, nsize);
    }

    oldmax = MaxTris;
    if (NumTris >= MaxTris) {
        MaxTris += 100;
        tnew = (TRiangleStruct *)csw_Realloc
               (TriList, MaxTris * sizeof(TRiangleStruct));
        if (!tnew) {
            FreeMem();
            return -1;
        }
        TriList = tnew;
        tptr = TriList + oldmax;
        nsize = (MaxTris - oldmax) * sizeof(TRiangleStruct);
        memset (tptr, 0, nsize);
    }

    return 1;

}  /*  end of private ExpandMem2 function  */



/*--------------------------------------------------------------------*/

int CSWGrdConstraint::AddNodeToConnectEdge (
    ORientedFaultStruct    *iptr,
    int                    lastflag,
    ORientedFaultStruct    *jptr,
    int                    jnode)
{
    int          n1, n2, e1, e2, newnode, newedge, oldedge;
    double       x1, y1, x2, y2, xt, yt;
    NOdeStruct   *np1, *np2, *npnew;
    EDgeStruct   *eptr;
    int          istat, elist[4];

    jptr = jptr;
    jnode = jnode;

    n1 = iptr->nodes[0];
    e1 = iptr->edges[0];
    if (lastflag) {
        n1 = iptr->nodes[iptr->num_nodes-1];
        e1 = iptr->edges[iptr->num_edges-1];
    }
    oldedge = e1;

    eptr = EdgeList + e1;

    SwapForIntersectEdge (eptr, n1);

    n2 = eptr->node1;
    if (n2 == n1) {
        n2 = eptr->node2;
    }

    np1 = NodeList + n1;
    np2 = NodeList + n2;
    x1 = np1->x;
    y1 = np1->y;
    x2 = np2->x;
    y2 = np2->y;
    xt = x1 + (x2 - x1) / 10.0;
    yt = y1 + (y2 - y1) / 10.0;

    newnode = AddNode (xt, yt, np1->z, np1->flag);
    istat = SplitIntoFour (
        eptr->tri1, eptr->tri2,
        e1, newnode, elist);
    if (istat == -1) {
        return -1;
    }

    AddToNodeEdgeList (elist[0], newnode);
    AddToNodeEdgeList (elist[1], newnode);
    AddToNodeEdgeList (elist[2], newnode);
    AddToNodeEdgeList (elist[3], newnode);

    e2 = elist[1];
    np1 = NodeList + n1;
    np1->is_intersect = 0;
    npnew = NodeList + newnode;
    npnew->is_intersect = 1;

    npnew->xorig = np1->x;
    npnew->yorig = np1->y;
    npnew->zorig = np1->z;

    eptr = EdgeList + e1;

    if ((eptr->node1 == n1  &&  eptr->node2 == newnode) ||
        (eptr->node1 == newnode  &&  eptr->node2 == n1)) {
        newedge = e2;
        npnew->adjusting_node = e1;
    }
    else {
        newedge = e1;
        npnew->adjusting_node = e2;
    }

    if (newedge != oldedge) {
        EdgeList[newedge].flag = EdgeList[oldedge].flag;
        EdgeList[newedge].number = EdgeList[oldedge].number;
        EdgeList[newedge].lineid = EdgeList[oldedge].lineid;
        EdgeList[newedge].lineid2 = EdgeList[oldedge].lineid2;
        EdgeList[newedge].isconstraint = EdgeList[oldedge].isconstraint;

        EdgeList[oldedge].flag = 0;
        EdgeList[oldedge].number = 0;
        EdgeList[oldedge].lineid = 0;
        EdgeList[oldedge].lineid2 = -1;
        EdgeList[oldedge].isconstraint = 0;
    }

    if (lastflag == 0) {
        iptr->nodes[0] = newnode;
        iptr->edges[0] = newedge;
    }
    else {
        iptr->nodes[iptr->num_nodes-1] = newnode;
        iptr->edges[iptr->num_edges-1] = newedge;
    }

    return 1;

}


/*------------------------------------------------------------------*/

/*
 * If either triangle using the specified edge has more
 * than one non constraint edge, swap the non constraint
 * edge that has inode as one of its endpoints.  This is
 * done to insure that when the eptr edge is split, the new
 * edges created by the split will connect to nodes on the
 * constraint line that eptr intersects with.
 */

void CSWGrdConstraint::SwapForIntersectEdge (EDgeStruct *eptr,
                                  int inode)
{
    int             t1, t2, e1, e2, e3, ein, nc, done;
    TRiangleStruct  *tp;
    EDgeStruct      *ep1, *ep2, *ep3;

    ein = eptr - EdgeList;

    t1 = eptr->tri1;
    if (t1 < 0) {
        return;
    }

    tp = TriList + t1;
    e1 = tp->edge1;
    e2 = tp->edge2;
    e3 = tp->edge3;
    ep1 = EdgeList + e1;
    ep2 = EdgeList + e2;
    ep3 = EdgeList + e3;

    nc = 0;
    if (ep1->flag != 0) {
        nc++;
    }
    if (ep2->flag != 0) {
        nc++;
    }
    if (ep3->flag != 0) {
        nc++;
    }

    done = 0;
    if (nc < 2) {
        if (e1 != ein) {
            if (ep1->node1 == inode  ||  ep1->node2 == inode) {
                SwapEdge (e1);
                done = 1;
            }
        }
        if (e2 != ein  &&  done == 0) {
            if (ep2->node1 == inode  ||  ep2->node2 == inode) {
                SwapEdge (e2);
                done = 1;
            }
        }
        if (e3 != ein  &&  done == 0) {
            if (ep3->node1 == inode  ||  ep3->node2 == inode) {
                SwapEdge (e3);
            }
        }
    }

    t2 = eptr->tri2;
    if (t2 < 0) {
        return;
    }

    tp = TriList + t2;
    e1 = tp->edge1;
    e2 = tp->edge2;
    e3 = tp->edge3;
    ep1 = EdgeList + e1;
    ep2 = EdgeList + e2;
    ep3 = EdgeList + e3;

    nc = 0;
    if (ep1->flag != 0) {
        nc++;
    }
    if (ep2->flag != 0) {
        nc++;
    }
    if (ep3->flag != 0) {
        nc++;
    }

    done = 0;
    if (nc < 2) {
        if (e1 != ein) {
            if (ep1->node1 == inode  ||  ep1->node2 == inode) {
                SwapEdge (e1);
                done = 1;
            }
        }
        if (e2 != ein  &&  done == 0) {
            if (ep2->node1 == inode  ||  ep2->node2 == inode) {
                SwapEdge (e2);
                done = 1;
            }
        }
        if (e3 != ein  &&  done == 0) {
            if (ep3->node1 == inode  ||  ep3->node2 == inode) {
                SwapEdge (e3);
            }
        }
    }

    return;

}

/*------------------------------------------------------------------*/

int CSWGrdConstraint::ShiftIntersectNodes (void)
{
    NOdeStruct       *np1;
    EDgeStruct       *eptr;
    int              istat, i, n1, n2;
    int              do_write;
    char             fname[200];

    for (i=0; i<NumNodes; i++) {
        np1 = NodeList + i;
        if (np1->is_intersect == 0) {
            continue;
        }
        if (np1->adjusting_node < 0) {
            assert (0);
        }
        eptr = EdgeList + np1->adjusting_node;
        if (eptr->deleted == 0) {
            n1 = eptr->node1;
            n2 = eptr->node2;
            if (n1 == i) {
                istat =
                  RemoveIntersectNode (n2);
                if (istat == -1) {
                    return -1;
                }
            }
            else if (n2 == i) {
                istat =
                  RemoveIntersectNode (n1);
                if (istat == -1) {
                    return -1;
                }
            }
        }

        np1->x = np1->xorig;
        np1->y = np1->yorig;
        eptr->deleted = 1;

    }

    do_write = csw_GetDoWrite ();
    if (do_write == 1) {
        sprintf (fname, "postshift.tri");
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            fname);
    }

    return 1;

}

/*----------------------------------------------------------------------------*/

int CSWGrdConstraint::RemoveIntersectNode (int nodenum)
{
    EDgeStruct      *eptr;
    int             i, *list, nlist, n1;

    list = NodeEdgeList[nodenum].list;
    nlist = NodeEdgeList[nodenum].nlist;

    for (i=0; i<nlist; i++) {
        eptr = EdgeList + list[i];
        n1 = eptr->node1;
        if (n1 == nodenum) {
            n1 = eptr->node2;
        }
        RemoveFromNodeEdgeList (list[i], n1);
        eptr->deleted = 1;
    }

    NodeList[nodenum].deleted = 1;
    NodeEdgeList[nodenum].nlist = 0;

    return 1;

}



/*----------------------------------------------------------------------------*/


int CSWGrdConstraint::SwapEdge (int edgenum)

{
    TRiangleStruct         *tp1, *tp2;
    EDgeStruct             *ep, *ep3, *ep4, *eptmp;
    int                    e1, e2, e3, e4,
                           n1, n2, n3, n4,
                           tnum1, tnum2;

    ep = EdgeList + edgenum;
    if (ep->deleted == 1) {
        return 0;
    }
    if (ep->tri2 == -1) {
        return 0;
    }

/*
    Find the two edges of the first triangle that are not the specified edge.
*/
    tp1 = TriList + ep->tri1;
    tp2 = TriList + ep->tri2;

    e1 = e2 = e3 = e4 = -1;

    if (tp1->edge1 != edgenum) {
        e1 = tp1->edge1;
    }
    if (tp1->edge2 != edgenum) {
        if (e1 == -1) {
            e1 = tp1->edge2;
        }
        else {
            e2 = tp1->edge2;
        }
    }
    if (tp1->edge3 != edgenum) {
        e2 = tp1->edge3;
    }

/*
    Find the nodes of the first triangle, in the same order as the edges.
*/
    n1 = CommonNode (edgenum, e1);
    n2 = CommonNode (e1, e2);
    n3 = CommonNode (e2, edgenum);

/*
    Find the edges of the second triangle that are not the diagonal and
    find the remaining node of the quadrilateral.
*/
    if (tp2->edge1 != edgenum) {
        e3 = tp2->edge1;
    }
    if (tp2->edge2 != edgenum) {
        if (e3 == -1) {
            e3 = tp2->edge2;
        }
        else {
            e4 = tp2->edge2;
        }
    }
    if (tp2->edge3 != edgenum) {
        e4 = tp2->edge3;
    }

    ep3 = EdgeList + e3;
    ep4 = EdgeList + e4;

    n4 = CommonNode (e3, e4);

/*
    The edge e3 must be connected to edge e2.  If not, swap
    e3 and e4 edges.  When this is done, the edges e1, e2, e3, to e4
    represent a continuous perimeter of the quadrilateral.  The nodes
    n1, n2, n3, and n4 are the nodes of the perimeter, in order.
*/
    if (CommonNode (e2, e3) == -1) {
        eptmp = ep3;
        ep3 = ep4;
        ep4 = eptmp;
        e3 = ep3 - EdgeList;
        e4 = ep4 - EdgeList;
    }

/*
    The old diagonal was by definition from n1 to n3, so the
    new diagonal must be from n2 to n4.
*/
    ep->node1 = n2;
    ep->node2 = n4;
    ep->length = NodeDistance (n2, n4);

/*
    Remove the edge from the edge lists of n1 and n3, also
    add the edge to the n2 and n4 node lists.
*/
    RemoveFromNodeEdgeList (edgenum, n1);
    RemoveFromNodeEdgeList (edgenum, n3);
    AddToNodeEdgeList (edgenum, n2);
    AddToNodeEdgeList (edgenum, n4);

/*
    Update the triangles with their new edges.
*/
    tp1->edge1 = e1;
    tp1->edge2 = edgenum;
    tp1->edge3 = e4;

    tp2->edge1 = e2;
    tp2->edge2 = e3;
    tp2->edge3 = edgenum;

/*
    Update the triangles for the edges.
*/
    tnum1 = tp1 - TriList;
    tnum2 = tp2 - TriList;
    eptmp = EdgeList + e4;
    if (eptmp->tri1 == tnum2) {
        eptmp->tri1 = tnum1;
    }
    else {
        eptmp->tri2 = tnum1;
    }
    eptmp = EdgeList + e2;
    if (eptmp->tri1 == tnum1) {
        eptmp->tri1 = tnum2;
    }
    else {
        eptmp->tri2 = tnum2;
    }

    return 1;

}  /*  end of private SwapEdge function  */






int CSWGrdConstraint::CommonNode (int e1, int e2)

{
    EDgeStruct         *ep1, *ep2;
    int                result;

    result = -1;
    ep1 = EdgeList + e1;
    ep2 = EdgeList + e2;
    if (ep1->node1 == ep2->node1  ||
        ep1->node1 == ep2->node2) {
        result = ep1->node1;
    }
    else if (ep1->node2 == ep2->node1  ||
             ep1->node2 == ep2->node2) {
        result = ep1->node2;
    }

    return result;

}  /*  end of private CommonNode function  */




/*--------------------------------------------------------------------------*/

/*
 * Create fault line structs from the trimesh edges flagged as
 * constraints.
 */

int CSWGrdConstraint::grd_BuildFaultsFromTriMesh (
  NOdeStruct         *nodes,
  int                nnodes,
  EDgeStruct         *edges,
  int                nedges,
  TRiangleStruct     *tris,
  int                ntris,
  FAultLineStruct    **faults,
  int                *nfaults,
  int                dup_flag)
{
    int           istat, i, n;
    EDgeStruct    *eptr;
    NOdeStruct    *nptr;
    FAultLineStruct   *flist;
    int           nflist;

    if (tris == NULL  ||  edges == NULL  ||  nodes == NULL) {
        return -1;
    }

    if (nnodes < 3  ||  nedges < 3  ||  ntris < 1) {
        return -1;
    }

    NodeList = nodes;
    NumNodes = nnodes;
    EdgeList = edges;
    NumEdges = nedges;
    TriList = tris;
    NumTris = ntris;
    MaxNodes = NumNodes;
    MaxEdges = NumEdges;
    MaxTris = NumTris;

/*
 * If there are no edges flagged as faults, return with no action.
 */
    n = 0;
    for (i=0; i<NumEdges; i++) {

        eptr = EdgeList + i;

        if (eptr->deleted == 1) {
            continue;
        }

        if (!(eptr->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
              eptr->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT)) {
            continue;
        }

        n++;

    }

    if (n < 1) {
        return 1;
    }

    Xmin = Ymin = 1.e30;
    Xmax = Ymax = -1.e30;

    for (i=0; i<NumNodes; i++) {
        nptr = NodeList + i;
        if (nptr->x < Xmin) Xmin = nptr->x;
        if (nptr->y < Ymin) Ymin = nptr->y;
        if (nptr->x > Xmax) Xmax = nptr->x;
        if (nptr->y > Ymax) Ymax = nptr->y;
        nptr->adjusting_node = -1;
    }

    if (Xmin >= Xmax  ||  Ymin >= Ymax) {
        return -1;
    }
    Tiny = (Xmax - Xmin + Ymax - Ymin) / 20000.0;

  /*
   * Make a list of BFlagStruct objects.  This list needs to be
   * as large as the node list can grow to.  I set it to twice
   * the node list size.
   */
    BflagList = (BFlagStruct *)csw_Calloc (NumNodes * 2 * sizeof(BFlagStruct));
    if (BflagList == NULL) {
        return -1;
    }

  /*
   * Initialize the bflag list edge numbers to -1
   * and the edge list lineid numbers to -1.
   */
    for (i=0; i<NumNodes; i++) {
        BflagList[i].lineid[0] = -1;
        BflagList[i].lineid[1] = -1;
        BflagList[i].lineid[2] = -1;
        BflagList[i].lineid[3] = -1;
    }

    for (i=0; i<NumEdges; i++) {
        EdgeList[i].lineid2 = -1;
    }

    FreeNodeEdgeList ();

    istat =
    BuildNodeEdgeLists ();
    if (istat == -1) {
        FreeMem ();
        return -1;
    }

    istat = TraceConstraintEdges ();
    if (istat == -1) {
        FreeMem ();
        return -1;
    }

    if (NumOrientedFaults < 1) {
        FreeMem ();
        return -1;
    }

/*
 * The contiguous fault nodes should now be in the OrientedFaults list.
 * Make fault line structs from these.
 */
    flist = (FAultLineStruct *)csw_Calloc (NumOrientedFaults * sizeof(FAultLineStruct));
    if (flist == NULL) {
        FreeMem ();
        return -1;
    }
    istat =
      MakeFaultLinesFromOrientedFaults (&flist, &nflist, dup_flag);
    if (istat == -1) {
        FreeMem ();
        grd_fault_ptr->grd_free_fault_line_structs (flist, NumOrientedFaults);
        return -1;
    }

    *nfaults = nflist;
    *faults = flist;

    FreeMem ();

    return 1;

}

/*---------------------------------------------------------------------*/

/*
 * Populate the Traced fault list.
 */
int CSWGrdConstraint::TraceConstraintEdges (void)
{
    int                 i, j, istat, dead_end;
    EDgeStruct          *ep, *ep2;

/*
 * Set the tflag2 members of the edge structures to zero so they
 * can be used to keep track of which edges have already been
 * processed.
 */
    for (i=0; i<NumEdges; i++) {
        EdgeList[i].tflag2 = 0;
        EdgeList[i].lineid2 = -1;
    }

/*
 * Connect fault edges together into fault lines.
 */
    ClosedAllowed = 1;
    for (i=0; i<NumEdges; i++) {

        ep = EdgeList + i;
        if (
            !(ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
              ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT) ||
            ep->deleted == 1  ||
            ep->tflag2 == 1) {
            continue;
        }
        if (ep->lineid2 >= 0) {
            continue;
        }

        if (ep->isconstraint  &&  ep->tri2 == -1) {
            continue;
        }

        dead_end = FindDeadEndEdge (i);
        if (dead_end == -1) {
            ClosedAllowed = 0;
            return -1;
        }

        istat = OrientFromDeadEndEdge (dead_end);
        if (istat == -1) {
            return -1;
        }

        for (j=0; j<NumFaultEdges; j++) {
            ep2 = EdgeList + FaultEdges[j];
            ep2->lineid2 = NumOrientedFaults;
        }

        istat = AddOrientedFault ();
        if (istat == -1) {
            ClosedAllowed = 0;
            return -1;
        }

    }

    WriteOrientedFaults ();

    ClosedAllowed = 0;

    return 1;

}


/*-------------------------------------------------------------------------*/

int CSWGrdConstraint::MakeFaultLinesFromOrientedFaults (
  FAultLineStruct **flist,
  int             *nflist,
  int             dup_flag)
{
    ORientedFaultStruct    *ofp = NULL;
    double                 *xf = NULL, *yf = NULL, *zf = NULL,
                           *xline = NULL, *yline = NULL, *zline = NULL;
    int                    *fpts = NULL, *ftypes = NULL;
    int                    istat, i, j, n, ntot, npts, maxline;
    NOdeStruct             *nptr = NULL;
    FAultLineStruct        *faults = NULL;
    int                    nfaults;


    auto fscope = [&]()
    {
        csw_Free (xline);
        csw_Free (fpts);
        csw_Free (xf);
        csw_Free (fpts);
    };
    CSWScopeGuard func_scope_guard (fscope);


    *flist = NULL;
    *nflist = 0;

/*
 * Determine the size of and allocate work arrays.
 */
    ntot = 0;
    maxline = 0;
    for (i=0; i<NumOrientedFaults; i++) {
        ofp = OrientedFaults + i;
        ntot += ofp->num_nodes;
        if (ofp->num_nodes > maxline) {
            maxline = ofp->num_nodes;
        }
    }

    if (ntot < 1  ||  maxline < 1) {
        return 1;
    }

    xline = (double *)csw_Malloc (maxline * 3 * sizeof(double));
    if (xline == NULL) {
        return -1;
    }
    yline = xline + maxline;
    zline = yline + maxline;

    fpts = (int *)csw_Malloc (NumOrientedFaults * 2 * sizeof(int));
    if (fpts == NULL) {
        return -1;
    }
    ftypes = fpts + NumOrientedFaults;

    xf = (double *)csw_Malloc (ntot * 3 * sizeof(double));
    if (xf == NULL) {
        return -1;
    }
    yf = xf + ntot;
    zf = yf + ntot;

/*
 * Fill in the x, y and z fault arrays.
 */
    n = 0;
    for (i=0; i<NumOrientedFaults; i++) {
        ofp = OrientedFaults + i;
        for (j=0; j<ofp->num_nodes; j++) {
            nptr = NodeList + ofp->nodes[j];
            xline[j] = nptr->x;
            yline[j] = nptr->y;
            zline[j] = nptr->z;
        }
        npts = ofp->num_nodes;
        if (dup_flag) {
            CorrectForDuplicates (
                xline, yline, zline, &npts, Tiny);
        }
        memcpy (xf+n, xline, npts*sizeof(double));
        memcpy (yf+n, yline, npts*sizeof(double));
        memcpy (zf+n, zline, npts*sizeof(double));
        fpts[i] = npts;
        n += npts;
        ftypes[i] = GRD_DISCONTINUITY_CONSTRAINT;
    }

/*
 * Convert to fault line structures.
 */
    istat =
      grd_fault_ptr->grd_fault_arrays_to_structs (
        xf, yf, zf,
        fpts, ftypes, NumOrientedFaults,
        &faults, &nfaults);

    if (istat != 1) {
        return -1;
    }

    *flist = faults;
    *nflist = nfaults;

    return 1;

}



/*---------------------------------------------------------------*/

/*
 * Remove duplicate points in the specified arrays.  This is
 * meant for changing a vertical fault split constraint into
 * just one side, either up or down.
 */
void CSWGrdConstraint::CorrectForDuplicates (
    double      *xline,
    double      *yline,
    double      *zline,
    int         *nline,
    double      tiny)
{
    double      x0, y0, dx, dy, dist, dcrit;
    int         i, j, n, npts;
    char        *flags = NULL;
    int         n1, n2;
    double      *xtmp = NULL, *ytmp = NULL, *ztmp = NULL;


    auto fscope = [&]()
    {
        csw_Free (xtmp);
        csw_Free (flags);
    };
    CSWScopeGuard func_scope_guard (fscope);


    dcrit = tiny;
    npts = *nline;

    if (npts < 5) {
        return;
    }
    if (npts %2 == 0) {
        return;
    }
/*
 * If the line is not closed, do nothing.
 */
    dx = xline[0] - xline[npts-1];
    dy = yline[0] - yline[npts-1];
    dist = dx * dx + dy * dy;
    dist = sqrt (dist);
    if (dist > dcrit) {
        return;
    }

/*
 * Unclose the line to check for duplicate points.
 */
    npts--;

/*
 * If there are not exactly 2 non duplicated nodes
 * then return doing nothing.
 */
    n = 0;
    n1 = n2 = -1;
    for (i=0; i<npts; i++) {
        x0 = xline[i];
        y0 = yline[i];
        for (j=0; j<npts; j++) {
            if (j == i) {
                continue;
            }
            dx = xline[j] - x0;
            dy = yline[j] - y0;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist > dcrit) {
                n++;
                if (n1 == -1) {
                    n1 = i;
                }
                else {
                    n2 = i;
                }
            }
        }
    }

    if (n != 2) {
        return;
    }

    if (n1 - n2 != npts /2) {
        return;
    }

/*
 * If the n1th node is not the 0th node, then rotate
 * the arrays to make it the zero node.
 */
    if (n1 != 0) {
        xtmp = (double *)csw_Malloc (npts * 3 * sizeof(double));
        if (xtmp == NULL) {
            return;
        }
        ytmp = xtmp + npts;
        ztmp = ytmp + npts;
        n = 0;
        for (i=n1; i<npts+n1; i++) {
            j = i;
            if (j >= npts) j -= npts;
            xtmp[n] = xline[j];
            ytmp[n] = yline[j];
            ztmp[n] = zline[j];
            n++;
        }
        memcpy (xline, xtmp, npts * sizeof(double));
        memcpy (yline, ytmp, npts * sizeof(double));
        memcpy (zline, ztmp, npts * sizeof(double));
    }

/*
 * Allocate the array flagging nodes to delete.
 */
    flags = (char *)csw_Calloc (npts * sizeof(char));
    if (flags == NULL) {
        return;
    }

    for (i=0; i<npts; i++) {
        if (flags[i]) continue;
        x0 = xline[i];
        y0 = yline[i];
        for (j=0; j<npts; j++) {
            if (j == i  ||
                flags[j]) {
                continue;
            }
            dx = xline[j] - x0;
            dy = yline[j] - y0;
            dist = dx * dx + dy * dy;
            dist = sqrt (dist);
            if (dist <= dcrit) {
                flags[j] = 1;
                break;
            }
        }
    }

    n = 0;
    for (i=0; i<npts; i++) {
        if (flags[i]) continue;
        if (i == n) {
            n++;
            continue;
        }
        xline[n] = xline[i];
        yline[n] = yline[i];
        zline[n] = 1.e30;
        n++;
    }

    *nline = n;

    return;
}



/*-------------------------------------------------------------------*/

void CSWGrdConstraint::ChangeOrientedFaultNode (
    int         lineid,
    int         oldnode,
    int         newnode)
{
    int                   i;
    ORientedFaultStruct   *ofp;

    if (lineid < 0  ||  lineid > NumOrientedFaults-1) {
        return;
    }

    ofp = OrientedFaults + lineid;

    for (i=0; i<ofp->num_nodes; i++) {
        if (ofp->nodes[i] == oldnode) {
            ofp->nodes[i] = newnode;
        }
    }

    return;

}



/*---------------------------------------------------------------------*/

void CSWGrdConstraint::MatchFaultEdgesAndNodes (void)
{
    int        i, nn, ne, n1, n2, n3, n4;
    EDgeStruct *ep;
    ORientedFaultStruct   *ofp;

    for (i=0; i<NumOrientedFaults; i++) {

        ofp = OrientedFaults + i;

        nn = ofp->num_nodes;
        ne = ofp->num_edges;

        if (nn < 2) {
            continue;
        }

    /*
     * If the first node in the fault node list is no longer
     * associated with a node from the first edge, change the
     * first node to agree with the first edge.
     */
        n1 = ofp->nodes[0];
        n2 = ofp->nodes[1];
        ep = EdgeList + ofp->edges[0];
        n3 = ep->node1;
        n4 = ep->node2;

        if (!(n1 == n3  ||  n1 == n4)) {
            if (n2 == n3) {
                ofp->nodes[0] = n4;
                n1 = n4;
            }
            else {
                ofp->nodes[0] = n3;
                n1 = n3;
            }
        }

        if (!(n2 == n3  ||  n2 == n4)) {
            if (n1 == n3) {
                ofp->nodes[1] = n4;
            }
            else {
                ofp->nodes[1] = n3;
            }
        }

    /*
     * If the last node in the fault node list is no longer
     * associated with a node from the last edge, change the
     * last node to agree with the last edge.
     */
        n1 = ofp->nodes[nn-1];
        n2 = ofp->nodes[nn-2];
        ep = EdgeList + ofp->edges[ne-1];
        n3 = ep->node1;
        n4 = ep->node2;

        if (!(n1 == n3  ||  n1 == n4)) {
            if (n2 == n3) {
                ofp->nodes[nn-1] = n4;
                n1 = n4;
            }
            else {
                ofp->nodes[nn-1] = n3;
                n1 = n3;
            }
        }

        if (!(n2 == n3  ||  n2 == n4)) {
            if (n1 == n3) {
                ofp->nodes[nn-2] = n4;
            }
            else {
                ofp->nodes[nn-2] = n3;
            }
        }

    }

    return;

}


/*----------------------------------------------------------------------*/

void CSWGrdConstraint::UpdateOnBorderNodes (void)
{
    EDgeStruct           *eptr;
    NOdeStruct           *nptr;
    int                  i;

    for (i=0; i<NumEdges; i++) {
        eptr = EdgeList + i;
        if (eptr->deleted == 1) {
            continue;
        }
        if (eptr->tri2 >= 0) {
            continue;
        }
        nptr = NodeList + eptr->node1;
        nptr->on_border = 1;
        nptr = NodeList + eptr->node2;
        nptr->on_border = 1;
    }

    return;

}


/*---------------------------------------------------------------------*/

void CSWGrdConstraint::GetFanConnectedEdges
    (int nodenum,
     int edgenum,
     int *list,
     int *nlist,
     int maxlist)
{
    int     e1, e2, t1, tlast, n;
    EDgeStruct   *ep1, *ep2;
    TRiangleStruct   *tp1;

    *nlist = 0;
    e1 = edgenum;
    ep1 = EdgeList + e1;
    if (ep1->tri2 == 0) {
        return;
    }

    list[0] = e1;
    n = 1;
    tlast = -1;

    for (;;) {

        t1 = ep1->tri1;
        if (t1 == tlast) {
            t1 = ep1->tri2;
        }
        if (t1 == -1) {
            break;
        }

        tlast = t1;
        tp1 = TriList + t1;
        e2 = -1;
        if (tp1->edge1 != e1) {
            ep2 = EdgeList + tp1->edge1;
            if (ep2->node1 == nodenum  ||
                ep2->node2 == nodenum) {
                e2 = tp1->edge1;
            }
        }
        if (e2 == -1) {
            if (tp1->edge2 != e1) {
                ep2 = EdgeList + tp1->edge2;
                if (ep2->node1 == nodenum  ||
                    ep2->node2 == nodenum) {
                    e2 = tp1->edge2;
                }
            }
        }
        if (e2 == -1) {
            if (tp1->edge3 != e1) {
                ep2 = EdgeList + tp1->edge3;
                if (ep2->node1 == nodenum  ||
                    ep2->node2 == nodenum) {
                    e2 = tp1->edge3;
                }
            }
        }

        if (e2 == -1) {
            return;
        }

        if (n >= maxlist) {
            return;
        }

        list[n] = e2;
        n++;

        ep1 = EdgeList + e2;

    }

    *nlist = n;
    return;

}


/*---------------------------------------------------------------------*/

void CSWGrdConstraint::FixForSplits (void)
{
    int   *list;
    int          i, j, n1, n2, e1, nlist, newnode;
    EDgeStruct   *ep;
    NOdeStruct   *np;

    list = p_i1000;

    for (i=0; i<NumFaultNodes; i++) {

        if (FaultSplits[i] <= 0) {
            continue;
        }
        e1 = FaultSplits[i] - 1;
        ep = EdgeList + e1;
        n2 = OppositeSplitNodes[i];
        n1 = ep->node1;
        if (n1 == n2) {
            n1 = ep->node2;
        }
        GetFanConnectedEdges (n1, e1, list, &nlist, 1000);
        if (nlist < 1) {
            continue;
        }
        np = NodeList + n1;
        newnode = AddNode (np->x, np->y, 1.e30, np->flag);
        if (newnode < 0) {
            continue;
        }
        for (j=0; j<nlist; j++) {
            ep = EdgeList + list[j];
            if (ep->node1 == n1) {
                ep->node1 = newnode;
            }
            else {
                ep->node2 = newnode;
            }
        }

    }

    return;

}


/*
 * Check all the trimesh nodes for any nodes that are
 * located on a fault line (but not the end point of 
 * a fault line)and also have a "valid"
 * z value (less than 1.e20).  Such nodes need to 
 * have their z values set to "invalid" (1.e30) do
 * the vertical fault z value algorithm can reset 
 * the z appropriately.
 */
int CSWGrdConstraint::SetNodesOnFaultToNull ()
{


    int do_write = csw_GetDoWrite ();
    if (do_write) {
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            (char *)"pre_nodes_on_fault.tri");
    }

  /*
   * Find the edges attached to each node.
   */
    int istat =
    BuildNodeEdgeLists ();
    if (istat == -1) {
        FreeMem ();
        return -1;
    }

    EDgeStruct      *ep = NULL;
    NOdeStruct      *np = NULL;
    int             *list = NULL,
                    nlist,
                    count,
                    i, j;

  /*
   * For each node, determine if it is on a fault but
   * not the end point of a fault.
   */
    for (i=0; i<NumNodes; i++) {

      /*
       * Get the edges attached to the node.
       */
        list = NodeEdgeList[i].list;
        if (list == NULL) {
            continue;
        }
        nlist = NodeEdgeList[i].nlist;

      /*
       * If the node already has an "invalid" z, or if the node
       * has been deleted, skip it.
       */
        np = NodeList + i;
        if (np->z > 1.e20  ||  np->deleted) {
            continue;
        }

      /*
       * Check if more than one edge attached to the node is a
       * fault edge.  A node with more than one non deleted
       * fault edge attached to it is on a fault but not an
       * endpoint of a fault.  Such a node will have its z value
       * set to 1.e30 ("invalid").
       */
        count = 0;
        for (j=0; j<nlist; j++) {
            ep = EdgeList + list[j];
            if (ep->deleted) {
                continue;
            }
            if (ep->flag == GRD_TRIMESH_FAULT_CONSTRAINT  ||
                ep->flag == GRD_TRIMESH_ZERO_FAULT_CONSTRAINT) {
                count++;
                if (count > 1) {
                    np->z = 1.e30;
                    np->zorig = 1.e30;
                    break;
                }
            }
        }

    }

    if (do_write) {
        grd_triangle_ptr->grd_WriteTextTriMeshFile (
            0, NULL,
            TriList, NumTris,
            EdgeList, NumEdges,
            NodeList, NumNodes,
            (char *)"post_nodes_on_fault.tri");
    }

    return 1;

}


/*
 * Find the edges that are part of non vertical faults.  An edge is
 * considered to be part of a non vertical fault if it is a constraint
 * edge and it is chained via other constraint edges having the same
 * id, with the chain closing on itself.  Also, if both unclosed 
 * endpoints of the chain are on the border of the trimesh and both
 * endpoints are separated by a small distance relative to the total
 * length of the chain of efges, the chain is assumed to be a non vertical
 * fault.
 */
void CSWGrdConstraint::FlagNVFaultEdges (void)
{
}
