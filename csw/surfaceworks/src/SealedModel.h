
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
 * Define the SealedModel class.  This class is used to
 * seal surfaces together in a model in preparation for generating a
 * tetrahedral mesh.
 */

#ifndef _SEALEDMODEL_H_
#define _SEALEDMODEL_H_

/*
 * System includes.
 */
#include <stdlib.h>

/*
 * General csw includes.
 */
#include <csw/surfaceworks/include/grid_api.h>
#include <csw/surfaceworks/include/grd_spatial3dtri.h>
#include <csw/surfaceworks/include/grd_xyindex.h>

#include <csw/surfaceworks/private_include/Vert.h>
#include <csw/surfaceworks/private_include/grd_utils.h>

#include <csw/surfaceworks/src/moller.h>
#include <csw/surfaceworks/src/PadSurfaceForSim.h>

/*
 * Define constants for this file.
 */
#define MAX_CORNER_SIZE   2000
#define LOCK_SEAL_FLAG    -99

#define FAULT_HORIZON 1
#define FAULT_FAULT 2
#define HORIZON_HORIZON 3

#define _HORIZON_ID_BASE_   0
#define _FAULT_ID_BASE_     10000
#define _BOUNDARY_ID_BASE_  100000000

#define _LAST_POINT_FLAG_   100000000

#define _HORIZON_TMESH_    1
#define _FAULT_TMESH_      2
#define _BOUNDARY_TMESH_   3

#define _NORTH_ID_         1
#define _SOUTH_ID_         2
#define _EAST_ID_          3
#define _WEST_ID_          4

#define _SED_SURF_ID_      -100
#define _MODEL_BOTTOM_ID_  -200
#define _DETACHMENT_TMESH_ -1000

#define _TOP_ID_           -300
#define _BOTTOM_ID_        -400

#define _MAX_NODE_ATTRIBS_  100
#define _MAX_ATTRIBS_       100


/*
 * Define structures used in the SealedModel class.
 */
typedef struct {
  int               intersection_id_list[1000];
  char              direction_list[1000];
  int               nlist;
  int               surfid;
} _SEaledBorder_;

typedef struct {
  NOdeStruct        *nodes;
  int               num_nodes;
  EDgeStruct        *edges;
  int               num_edges;
  TRiangleStruct    *tris;
  int               num_tris;
  int               is_padded,
                    is_sealed,
                    is_limit;
  int               vflag;
  double            vbase[6];
  int               numIntersects;
  int               id;
  double            xcenter, ycenter;
  int               possible_int_list[1000],
                    num_possible_int_list;
  _SEaledBorder_    sealed_border;
  double            *node_attribs[_MAX_NODE_ATTRIBS_];
  int               node_attrib_indices[_MAX_NODE_ATTRIBS_];
  int               num_node_attrib;
  int               external_id;
  double            age;
  CSW_F             *grid;
  int               ncol, nrow;
  double            gxmin, gymin, gxmax, gymax;
  double            minage, maxage;
  double            zmin, zmax;
  double            *xdetach;
  double            *ydetach;
  double            *zdetach;
  int               ndetach;
  int               detach_id;
  int               detach_list[1000];
  int               num_detach_list;
  int               sealed_to_sides;
} CSWTriMeshStruct;

typedef struct {
  double            *x,
                    *y,
                    *z;
  int               *flags;
  int               npts;
  int               surf1,
                    surf2;
  int               nclose;
  int               embed_flag;
  int               end1_index,
                    end2_index;
  int               splice_partner_1,
                    splice_partner_2;
  int               external_id1,
                    external_id2;
} _INtersectionLine_;

typedef struct {
  int               lineid,
                    pointid;
  int               lineid2,
                    pointid2;
} _ITag_;

typedef struct {
  double            *x,
                    *y,
                    *z;
  _ITag_            *itags;
  int               npts;
  int               surfid;
} _OUtline_;

typedef struct {
  double            x1, y1, z1, x2, y2, z2;
  int               used;
} _INtersectionSegment_;

typedef struct {
  const _INtersectionLine_ *list;
  int                      nlist;
} _INtersectionLineList_;

typedef struct {
  double    *x,
            *y,
            *z;
  int       npts;
  int       flag1,
            flag2;
} _TRiMeshBoundary_;


/*
 * Define the SealedModel class.  The implementation is in
 * csw/surfaceworks/src/SealedModel.cc
 */
class SealedModel {

 private:

  double        dmcs_1[MAX_CORNER_SIZE];
  double        dmcs_2[MAX_CORNER_SIZE];
  int           imcs_1[MAX_CORNER_SIZE];
  int           imcs_2[MAX_CORNER_SIZE];
  
  PadSurface    pad_surface_obj;
  CSWGrdAPI     grd_api_obj;
  CSWGrdUtils   grd_utils_obj;

  int           faultLineFlags[2000];

  void SetPadGrdAPI () {
    pad_surface_obj.SetGrdAPIPtr (&grd_api_obj);
  }

 public:

  void freeMemory (void);
  SealedModel() {SetPadGrdAPI (); init();};
  virtual ~SealedModel() {freeMemory();};


// Objects of class not meant to be copied or moved.

  private:

    SealedModel (const SealedModel &other) {};
    const SealedModel &operator= (const SealedModel &other) {return *this;};
    SealedModel (const SealedModel &&other) {};
    const SealedModel &operator= (const SealedModel &&other) {return *this;};


  public:



  void setSimSealFlag (int ival);
  int getSimSealFlag (void);

  void setMarginFraction (double val);
  double getMarginFraction (void);

  int addPaddedFault (int id,
                      int vflag, double *vbase,
                      NOdeStruct *nodes, int num_nodes,
                      EDgeStruct *edges, int num_edges,
                      TRiangleStruct *triangles, int num_triangles);
  int setPaddedDetachment (
                      NOdeStruct *nodes, int num_nodes,
                      EDgeStruct *edges, int num_edges,
                      TRiangleStruct *triangles, int num_triangles);

  int addInputFault (NOdeStruct *nodes, int num_nodes,
                     EDgeStruct *edges, int num_edges,
                     TRiangleStruct *triangles, int num_triangles);
  int addInputFault (NOdeStruct *nodes, int num_nodes,
                     EDgeStruct *edges, int num_edges,
                     TRiangleStruct *triangles, int num_triangles,
                     double minage, double maxage);
  int addInputFault (int id,
                     NOdeStruct *nodes, int num_nodes,
                     EDgeStruct *edges, int num_edges,
                     TRiangleStruct *triangles, int num_triangles);
  int addInputFault (int id,
                     NOdeStruct *nodes, int num_nodes,
                     EDgeStruct *edges, int num_edges,
                     TRiangleStruct *triangles, int num_triangles,
                     double minage, double maxage);
  int addInputFault (int id,
                     int vused, double *vbase,
                     NOdeStruct *nodes, int num_nodes,
                     EDgeStruct *edges, int num_edges,
                     TRiangleStruct *triangles, int num_triangles);
  int addInputFault (int id,
                     int vused, double *vbase,
                     NOdeStruct *nodes, int num_nodes,
                     EDgeStruct *edges, int num_edges,
                     TRiangleStruct *triangles, int num_triangles,
                     double minage, double maxage);
  int addInputFault (int id,
                     int vused, double *vbase,
                     NOdeStruct *nodes, int num_nodes,
                     EDgeStruct *edges, int num_edges,
                     TRiangleStruct *triangles, int num_triangles,
                     double minage, double maxage,
                     double *xline, double *yline, double *zline,
                     int nline);
  int addInputFault (int id,
                     int vused, double *vbase,
                     NOdeStruct *nodes, int num_nodes,
                     EDgeStruct *edges, int num_edges,
                     TRiangleStruct *triangles, int num_triangles,
                     double minage, double maxage,
                     double *xline, double *yline, double *zline,
                     int nline, int detachment_id);

  int addInputDetachment (int id,
                     NOdeStruct *nodes, int num_nodes,
                     EDgeStruct *edges, int num_edges,
                     TRiangleStruct *triangles, int num_triangles);
  int addInputHorizon (NOdeStruct *nodes, int num_nodes,
                       EDgeStruct *edges, int num_edges,
                       TRiangleStruct *triangles, int num_triangles);
  int addInputHorizon (int id, double age,
                       NOdeStruct *nodes, int num_nodes,
                       EDgeStruct *edges, int num_edges,
                       TRiangleStruct *triangles, int num_triangles);
  int addInputHorizon (double age,
                       NOdeStruct *nodes, int num_nodes,
                       EDgeStruct *edges, int num_edges,
                       TRiangleStruct *triangles, int num_triangles);
  int addInputGridHorizon (int id, double age,
                           CSW_F *grid,
                           int ncol, int nrow,
                           double gxmin, double gymin,
                           double gxmax, double gymax,
                           double *xpoly, double *ypoly, int npoly,
                           double *xfault, double *yfault, double *zfault,
                           int *ifcomp, int *ifault, int nfault);
  int setSedimentSurface (int id, double age,
                          NOdeStruct *nodes, int num_nodes,
                          EDgeStruct *edges, int num_edges,
                          TRiangleStruct *triangles, int num_triangles);
  int addToSedimentSurface
                         (int id, double age,
                          NOdeStruct *nodes, int num_nodes,
                          EDgeStruct *edges, int num_edges,
                          TRiangleStruct *triangles, int num_triangles);
  int setModelBottom (int id, double age,
                      NOdeStruct *nodes, int num_nodes,
                      EDgeStruct *edges, int num_edges,
                      TRiangleStruct *triangles, int num_triangles);
  int addToModelBottom (
                      int id, double age,
                      NOdeStruct *nodes, int num_nodes,
                      EDgeStruct *edges, int num_edges,
                      TRiangleStruct *triangles, int num_triangles);
  int setSedimentSurface (NOdeStruct *nodes, int num_nodes,
                          EDgeStruct *edges, int num_edges,
                          TRiangleStruct *triangles, int num_triangles);
  int setModelBottom (NOdeStruct *nodes, int num_nodes,
                      EDgeStruct *edges, int num_edges,
                      TRiangleStruct *triangles, int num_triangles);
  void setAverageSpacing (double val);
  double getAverageSpacing (void);


  int sealFaultsToDetachment (void);
  int sealPaddedModel(void);
  int padModel(double pxmin, double pymin, double pzmin,
               double pxmax, double pymax, double pzmax,
               double avspace);
  int padModel (double xyMarginFraction, double zMarginFraction,
                double avspace);

  int padFaultsForSplitLines (void);
  int calcFaultHorizonIntersections (void);

  _INtersectionLineList_ *getHorizonIntersectionLines (void);
  _INtersectionLineList_ *getFaultIntersectionLines (void);
  _INtersectionLineList_ *getTopAndBottomIntersectionLines (void);
  _INtersectionLineList_ *getRawIntersectionLines (void);
  void getInputHorizons (CSWTriMeshStruct **list, int *nlist);
  void getInputFaults (CSWTriMeshStruct **list, int *nlist);
  void getPaddedHorizons (CSWTriMeshStruct **list, int *nlist);
  void getPaddedFaults (CSWTriMeshStruct **list, int *nlist);
  void getShallowBoundaryCopies (CSWTriMeshStruct **list, int *nlist);
  void getSealedHorizons (CSWTriMeshStruct **list, int *nlist);
  void getSealedFaults (CSWTriMeshStruct **list, int *nlist);
  void getPaddedTopAndBottom (CSWTriMeshStruct **list);
  void getSealedTopAndBottom (CSWTriMeshStruct **list);
  CSWTriMeshStruct *getSealedDetachment (void);

  int analyzeSealedModel (char *filename);

  int createTetgenInput (double **nodex,
                         double **nodey,
                         double **nodez,
                         int    **nodemark,
                         int    *numnodes,
                         int    **facetnode1,
                         int    **facetnode2,
                         int    **facetnode3,
                         int    **facetmark,
                         int    *numfacets);

  int writeTetgenSmeshFile (char const *pathname);

 protected:

/*
 * Data Members.
 */
  int                   vbEmbedFlag;
  int                   simSealFlag;
  int                   simOutputFlag;
  int                   sealFlagLock;
  double                marginFraction;

  CSWTriMeshStruct      *HorizonList,
                        *FaultList;
  int                   NumHorizonList,
                        MaxHorizonList,
                        NumFaultList,
                        MaxFaultList;

  CSWTriMeshStruct      *InputHorizonList,
                        *InputFaultList;
  int                   NumInputHorizonList,
                        MaxInputHorizonList,
                        NumInputFaultList,
                        MaxInputFaultList;
  CSWTriMeshStruct      *InputDetachmentList;
  int                   NumInputDetachmentList,
                        MaxInputDetachmentList;
  CSWTriMeshStruct      *NorthBoundarySurface,
                        *SouthBoundarySurface,
                        *EastBoundarySurface,
                        *WestBoundarySurface;
  CSWTriMeshStruct      *EmbeddedNorthBoundarySurface,
                        *EmbeddedSouthBoundarySurface,
                        *EmbeddedEastBoundarySurface,
                        *EmbeddedWestBoundarySurface;

  CSWTriMeshStruct      *InputSedimentSurface,
                        *InputModelBottom;
  CSWTriMeshStruct      *PaddedSedimentSurface,
                        *PaddedModelBottom;
  CSWTriMeshStruct      *SealedSedimentSurface,
                        *SealedModelBottom;
  CSWTriMeshStruct      *PaddedTop,
                        *PaddedBottom;
  CSWTriMeshStruct      *SealedTop,
                        *SealedBottom;

  int                   TopHorizonNumber,
                        BottomHorizonNumber;

  double                averageSpacing;
  double                verticalBoundarySpacing;
  double                modelGrazeDistance;

  double                maxRadialDist;

  CSWTriMeshStruct      *PaddedHorizonList,
                        *PaddedFaultList;
  int                   NumPaddedHorizonList,
                        MaxPaddedHorizonList,
                        NumPaddedFaultList,
                        MaxPaddedFaultList;

  CSWTriMeshStruct      *PaddedDetachmentList;
  int                   NumPaddedDetachmentList,
                        MaxPaddedDetachmentList;

  CSWTriMeshStruct      *PaddedDetachment;
  CSWTriMeshStruct      *SealedDetachment;

  double                padXmin, padYmin, padZmin,
                        padXmax, padYmax, padZmax;
  double                padYNorth, padYSouth, padXEast, padXWest;

  double                modelXmin, modelYmin, modelZmin,
                        modelXmax, modelYmax, modelZmax;
  double                modelTiny;

  double                origModelZmin, origModelZmax;
  double                modelZScale;

  Spatial3DTriangleIndex  *InputTriangle3DIndex;
  Spatial3DTriangleIndex  *PaddedTriangle3DIndex;

  _INtersectionSegment_ *WorkIntersectionSegments;
  int                   NumWorkIntersectionSegments,
                        MaxWorkIntersectionSegments;

  _INtersectionLine_    *WorkIntersectionLines;
  int                   NumWorkIntersectionLines,
                        MaxWorkIntersectionLines;

  _INtersectionLine_    *IntersectionLines;
  int                   NumIntersectionLines;

  CSWTriMeshStruct      *SealedHorizonList,
                        *SealedFaultList;
  int                   NumSealedHorizonList,
                        MaxSealedHorizonList,
                        NumSealedFaultList,
                        MaxSealedFaultList;

  _OUtline_             **HorizonOutlineList;

  _INtersectionLine_    *SealedHorizonIntersects;
  int                   NumSealedHorizonIntersects,
                        MaxSealedHorizonIntersects;

  _INtersectionLine_    *SealedTopAndBottomIntersects;
  int                   NumSealedTopAndBottomIntersects,
                        MaxSealedTopAndBottomIntersects;

  _INtersectionLine_    *SealedFaultIntersects;
  int                   NumSealedFaultIntersects,
                        MaxSealedFaultIntersects;

  _INtersectionLine_    **WorkFaultIntersects;
  int                   NumWorkFaultIntersects,
                        MaxWorkFaultIntersects;

  char                  AttribNameBuffer[100*_MAX_ATTRIBS_];
  char                  *AttribNames[_MAX_ATTRIBS_];
  int                   NumAttribs;

  int                   sideFaultStart,
                        sideFaultEnd;

/*
 * Members use to create tetgen output.
 */
  double                *TetgenX,
                        *TetgenY,
                        *TetgenZ;
  int                   *TetgenNodeMark;
  int                   *TetgenNode1,
                        *TetgenNode2,
                        *TetgenNode3;
  int                   *TetgenFacetMark;
  int                   NumTetgenNodes,
                        MaxTetgenNodes,
                        NumTetgenFacets,
                        MaxTetgenFacets;

/*
 * Members needed to support node indexing for speeding
 * up searches for nodes close to a point.
 */
  int       **NodeIndexGrid;
  double    IndexXmin, IndexYmin, IndexXspace, IndexYspace;
  int       IndexNcol, IndexNrow;

  int                   IndexPaddedFaults;


/*
 * General private methods.
 */
  void   init (void);
  void   free_mem (void);

/*
 * Private methods for doing the various sealing tasks.
 */

  int    SealTopAndBottom (void);
  int    UpdateIntersectionsForCrossing (CSWTriMeshStruct *tmesh);
  int    SnapLineToMaster (double *xmaster,
                           double *ymaster,
                           double *zmaster,
                           int    nmaster,
                           double *xline,
                           double *yline,
                           double *zline,
                           int    *nlineio);

  void   CalcModelGrazeDistance (void);
  int    CalcInputSurfacePaddedFaultIntersectionLines
                           (int surf1_num,
                            int surf1_type,
                            int surf2_num,
                            int surf2_type);
  int    CalcInputSurfaceIntersectionLines
                           (int surf1_num,
                            int surf1_type,
                            int surf2_num,
                            int surf2_type);
  int    CalcPaddedSurfaceIntersectionLines
                           (int surf1_num,
                            int surf1_type,
                            int surf2_num,
                            int surf2_type);
  int    CalcPaddedSurfaceBoundaryIntersections (void);

  void   CalcModelBounds (void);
  int    CalcTriMeshCentroid (CSWTriMeshStruct *tmesh,
                              double *xcenter,
                              double *ycenter);

  int    CreateVerticalBoundaries (void);

  int    CreateInput3DTriangleIndex (void);
  int    CreatePadded3DTriangleIndex (void);
  int    CreateDetachment3DTriangleIndex (void);

  void   Calc3DBox (TRiangleStruct *tptr,
                    EDgeStruct     *edges,
                    NOdeStruct     *nodes,
                    double         *txmin,
                    double         *tymin,
                    double         *tzmin,
                    double         *txmax,
                    double         *tymax,
                    double         *tzmax,
                    double         *tinyout);

  int    CalcTriangleIntersection (TRiangleStruct *tp1,
                                   TRiangleStruct *tp2,
                                   double tiny,
                                   EDgeStruct *s1edges,
                                   NOdeStruct *s1nodes,
                                   EDgeStruct *s2edges,
                                   NOdeStruct *s2nodes);

  int    FindOverlapWorkSegments (void);
  int    ConnectIntersectionSegments (int tmeshid1,
                                      int tmeshid2);

  int    AddWorkIntersectionLine (double *xline,
                                  double *yline,
                                  double *zline,
                                  int    nline,
                                  int    tmeshid1,
                                  int    tmeshid2);

  int    ConnectCloseWorkIntersectionLines (void);
  int    AppendIline (double *xw, double *yw, double*zw,
                      int *nw,
                       _INtersectionLine_ *lp, int idir);
  int    PrependIline (double *xw, double *yw, double*zw,
                       int *nw,
                       _INtersectionLine_ *lp, int idir);

  int    AddWorkLinesToResults (void);

  void   CalcWorkSegmentBounds (
             double *xmin, double *ymin, double *zmin,
             double *xmax, double *ymax, double *zmax);
  int    SamePointXYZ (double x1, double y1, double z1,
                       double x2, double y2, double z2,
                       double tiny);
  int    SamePointXYZ (double x1, double y1, double z1,
                       double x2, double y2, double z2);
  int    SameValue (double val1, double val2, double tiny);

  void   FreeWorkFaultIntersects (void);
  void   FreeWorkIntersectionLines (void);
  void   FreeTmesh (CSWTriMeshStruct *tmeshlist, int nt);
  void   FreeOutlineList (_OUtline_ **list, int nlist);

  int    AddPaddedFault (int id,
                         NOdeStruct *nodes, int num_nodes,
                         EDgeStruct *edges, int num_edges,
                         TRiangleStruct *triangles, int num_triangles,
                         int vflag, double *vbase,
                         double minage, double maxage,
                         double *xline, double *yline, double *zline,
                         int nline, int detach_id);
  int    AddPaddedDetachment (int id,
                              NOdeStruct *nodes, int num_nodes,
                              EDgeStruct *edges, int num_edges,
                              TRiangleStruct *triangles, int num_triangles);
  int    AddPaddedHorizon (int id, double age,
                           NOdeStruct *nodes, int num_nodes,
                           EDgeStruct *edges, int num_edges,
                           TRiangleStruct *triangles, int num_triangles,
                           double xcenter, double ycenter);
  int    AddSealedHorizon (CSWTriMeshStruct *tmesh);
  int    AddSealedHorizonIntersect (
            double         *x,
            double         *y,
            double         *z,
            int            npts,
            int            orig_lineid);
  int    AddSealedFaultIntersect (
            double         *x,
            double         *y,
            double         *z,
            int            npts,
            int            isurf1,
            int            isurf2);
  int    AddSealedTopAndBottomIntersect (
            double         *x,
            double         *y,
            double         *z,
            int            npts,
            int            orig_line_id);
  int    AddSealedFault (CSWTriMeshStruct *tmesh);
  void   FreePaddedLists (void);
  void   FreeSealedLists (void);
  void   FreeVerticalBoundaries (void);

  int    OutlineAndCropHorizons (void);
  _OUtline_ *CalcSealedHorizonOutline (CSWTriMeshStruct *tmesh);
  CSWTriMeshStruct *CropSurface (
                     CSWTriMeshStruct *tmesh);
  int    EmbedEdgesInDetachment (void);
  int    EmbedDetachmentEdgesInFault (int id);
  int    EmbedEdgesInFault (int id);
  int    EmbedEdgesInBoundary (int id);
  int    EmbedLineInBoundary (
           int id,
           double *xc,
           double *yc,
           double *zc,
           int    nc);
  int    add_constraint_for_embed (
           double    *xadd,
           double    *yadd,
           double    *zadd,
           int       nadd,
           double    **xcout,
           double    **ycout,
           double    **zcout,
           int       ntot);
  int    EmbedPointsInSurface (
           CSWTriMeshStruct   *tmesh,
           double    *xlines,
           double    *ylines,
           double    *zlines,
           int       *nplines,
           int       nlines);
  int    EmbedPointsInBoundarySurface(
           CSWTriMeshStruct *tmesh,
           double    *xlines,
           double    *ylines,
           double    *zlines,
           int       *nplines,
           int       *lineflags,
           int       nlines);

  int    EmbedPointsInFaultSurface (
           CSWTriMeshStruct   *tmesh,
           double    *xlines,
           double    *ylines,
           double    *zlines,
           int       *nplines,
           int       nlines);

  _OUtline_  *BuildHorizonOutline (
           double    *xpoly,
           double    *ypoly,
           void      **vtag,
           int       npoly,
           NOdeStruct   *nodes,
           int       num_nodes,
           CSWTriMeshStruct  *tmesh);
  double ZFromIntList (
           double   xt,
           double   yt,
           double   tiny,
           int      tmeshid);
  int    CreateNodeIndex (
           NOdeStruct  *nodes,
           int      numnodes,
           double   xmin,
           double   ymin,
           double   xmax,
           double   ymax);
  void   CleanupNodeIndex (void);
  double ZFromNodes (
           double   xt,
           double   yt,
           NOdeStruct  *nodes,
           double   tiny);

  void   MarkIntersectionLinesToEmbed (
           double   *xpoly,
           double   *ypoly,
           double   *zpoly,
           int      npoly,
           int      *lines,
           int      nlines);

  void   CheckUnpaddedFaultIntersects (void);
  int    CheckSurfaceIntersection (
           CSWTriMeshStruct    *surf1,
           CSWTriMeshStruct    *surf2);

  int    ClipDetachmentFaultToIntersections (
           CSWTriMeshStruct *tmesh);
  int    ClipFaultToIntersections (
           CSWTriMeshStruct *tmesh);

  int    ClipBoundaryToIntersections (
           CSWTriMeshStruct *tmesh);

  int    CreateSealedHorizonIntersects (void);
  int    CreateSealedTopAndBottomIntersects (void);
  int    CreateSealedFaultIntersects (void);
  _OUtline_ *BuildOutlineFromSealedBorder (_SEaledBorder_ *sbord);
  _OUtline_ *BuildOutlineFromSealedTopAndBottomBorder (_SEaledBorder_ *sbord);

  int    FixupCrossingIntersects (void);

  int    CheckForPointInList (
           double    x,
           double    y,
           double    *xlist,
           double    *ylist,
           XYIndex2D *xyindex,
           double    *xnew,
           double    *ynew);

  int    ChooseSharedPoint (
           double    x1,
           double    y1,
           double    z1,
           double    x2,
           double    y2,
           double    z2,
           int       iline1,
           int       iline2);

  double IlineDistance (
           _INtersectionLine_ *iptr,
           double x, double y, double z);

  int    FindThirdIntersectLine (
           int    ipoint1,
           int    iline1);

  int    ProcessThirdLine (
           int    ipoint1,
           int    iline1,
           int    iline3);

  int    ProcessFaultWorkList (void);
  int    ProcessFaultWorkLine (_INtersectionLine_ *ipw);

  int    AddWorkFaultIntersect (_INtersectionLine_ *ipw);

  int    BuildFaultOutlineFromSealedIntersects (
           _INtersectionLine_ **sfplist, int nsfp,
           _INtersectionLine_ **shplist, int nshp,
           CSWTriMeshStruct  *tmesh,
           double **xc, double **yc, double **zc,
           int *nc, int **npc);

  int    GetBoundaryCornerNodes (
           int          boundary_id,
           double       xval,
           double       yval,
           double       *zout,
           int          *nodeout,
           int          *nout,
           int          maxout);

  int    FixBoundaryCorners (void);
  int    FixBoundaryForSharedCorner (
           int          boundary_id,
           double       *myzvals,
           int          *mynodes,
           int          mynval,
           double       *otherzvals,
           int          othernval);

  int    ResampleSealedIntersects (void);

  int    CorrectForEndPointSpacing (
           double       *xin,
           double       *yin,
           double       *zin,
           int          nin,
           double       *xout,
           double       *yout,
           double       *zout,
           int          *nout,
           int          maxout);

  int    AddTetgenNode (
           double           x,
           double           y,
           double           z,
           int              mark);
  int    AddTetgenFacet (
           int              n1,
           int              n2,
           int              n3,
           int              mark);
  void   FreeTetgenData (void);

  int    FindCloseSealedHorizonPoint (
           double *xio,
           double *yio,
           double *zio,
           double dcrit);

  int    SnapSealedFaultPointsToSealedHorizonPoints (void);

  int    AddNewAttributeName (char const *atname);

  int    AddToModelBottom (
           NOdeStruct        *nodes_in,
           int               num_nodes,
           EDgeStruct        *edges_in,
           int               num_edges,
           TRiangleStruct    *triangles_in,
           int               num_triangles);

  int    AddToSedimentSurface (
           NOdeStruct        *nodes_in,
           int               num_nodes,
           EDgeStruct        *edges_in,
           int               num_edges,
           TRiangleStruct    *triangles_in,
           int               num_triangles);

  int    SpliceFaultIntersectionLines (void);
  int    MoveSplicePoint (int spliceid,
                          double xt,
                          double yt,
                          double zt);

  int    InsertPointInLine (double *xa, double *ya, double *za,
                            int *iflags,
                            int npts, int index, int iflagval,
                            double xt, double yt, double zt);

  CSWTriMeshStruct
         *FindPaddedTriMesh (int id);

  int AssignShapeSurfaces (void);
  int FindShapeSurface (int id, int *list, int nlist);

  void WriteDebugFiles (void);

  void WriteIlinesToFile (
    _INtersectionLine_ **ilist,
    int                nlist,
    char               *fname);

  int CopyIline (_INtersectionLine_ *old_line,
                 _INtersectionLine_ *new_line);

  int BetweenPointsXYZ (
    double x, double y, double z,
    double x1, double y1, double z1,
    double x2, double y2, double z2);

  int CheckForClosedPath (_INtersectionLine_ **list, int nlist);

  int CreateDefaultPaddedTop (void);
  int CreateDefaultPaddedBottom (void);

  void SnapTopLine (_INtersectionLine_ *iptr);
  void SnapBottomLine (_INtersectionLine_ *iptr);

  void SnapTopSurface (void);
  void SnapBottomSurface (void);

  CSWTriMeshStruct *FindPaddedFaultForID (int id);
  int ReshapePaddedFault (CSWTriMeshStruct *tmesh,
                          CSWTriMeshStruct *stmesh);

  double CalcEdgeLength (EDgeStruct *eptr, NOdeStruct *nodes);
  void WriteIntersectionLines (char *fname);
  void WritePartialIntersectionLines (char *fname, int start);
  void WritePartialIntersectionLines (char *fname, int start, int end);
  void WriteSealedHorizonIntersects (char *fname);

  int FindFaultExternalID (int id);

  double GetPaddedSurfaceAge (int id);

  void SetSharedOutlinePoints (
    double            *xpoly,
    double            *ypoly,
    void              **tags,
    int               npoly,
    _INtersectionLine_  *bptr);

  void MakeExtensionLine (
    double       x1,
    double       y,
    double       x2,
    double       space,
    double       **xa,
    double       **ya,
    double       **za,
    int          *na);

  void ExtendIntersectLines (void);

  inline int NearZero (double val, double tiny)
    {
      if (val <= tiny  &&  val >= -tiny) {
        return 1;
      }
      return 0;
    };

  int CorrectToFixedFaultLines (double avspace);

  int FixSealedFaultBoundaryIntersects (void);
  int ClipILineToPoly (
    _INtersectionLine_    *iptr,
    double                *xp,
    double                *yp,
    double                *zp,
    int                   np,
    GRDVert               &gvert);

  void calcFaultZLimits (CSWTriMeshStruct *fault);

  int SplitLongSegments (
    double    *xout,
    double    *yout,
    double    *zout,
    int       *nout,
    int       maxout);

};
#endif
