
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/



#ifndef _PATCH_SPLIT_H_
#define _PATCH_SPLIT_H_

#include "csw/surfaceworks/include/grid_api.h"

#include <csw/surfaceworks/src/SealedModel.h>
#include <csw/surfaceworks/src/FaultConnect.h>

#include "csw/utils/private_include/ply_utils.h"
#include "csw/utils/private_include/ply_calc.h"
#include "csw/utils/private_include/gpf_utils.h"

#define         PS_UNKNOWN_LINE_TYPE  0
#define         PS_CONTACT_LINE       1
#define         PS_DRAPED_LINE        2
#define         PS_FAULT_CENTER_LINE  3

/*
 * Define structures used in patch spliting
 */
typedef struct {
    double *x = NULL;
    double *y = NULL;
    double *z = NULL;
    int    npts;
    int    id;
} ORiginalPoints;

typedef struct {
    double      *x = NULL,
                *y = NULL,
                *z = NULL;
    double      xleft,
                yleft,
                xright,
                yright;
    int         jleft,
                jright;
    int         npts,
                flag,
                fault_id;
    int         j1,
                j2;
    int         j1orig,
                j2orig;
    int         lock1,
                lock2;
    int         origcline;
    int         left_contact;
    int         right_contact;
    int         major;
} FAultCenterline;

typedef struct {
    double      *x = NULL,
                *y = NULL,
                *z = NULL;
    int         npts;
    int         patchid1;
    int         patchid2;
} COntactLine;

typedef struct {
    double      *x = NULL,
                *y = NULL,
                *z = NULL;
    int         npts,
                flag;
} BAseLine;

typedef struct {
    BAseLine    *list = NULL;
    int         nlist,
                id;
} ORiginalLines;

typedef struct {
    NOdeStruct       *nodes = NULL;
    int              num_nodes;
    EDgeStruct       *edges = NULL;
    int              num_edges;
    TRiangleStruct   *triangles = NULL;
    int              num_triangles;
    int              fault_id,
                     flag;
    int              vused;
    double           vbase[6];
    double           zmin,
                     zmax;
    double           dx,
                     dy;
    int              steep_flag;
    int              extended_flag;
    void             *tindex = NULL;
    double           txmin,
                     tymin,
                     txmax,
                     tymax,
                     txspace,
                     tyspace;
    int              tncol,
                     tnrow;
} FAultSurface;

typedef struct {
    double           *x = NULL,
                     *y = NULL,
                     *z = NULL;
    int              npts;
    int              type;
    int              direction;
    int              flag;
    int              used;
    int              lock1;
    int              lock2;
    int              clindex,
                     findex,
                     workpoly;
} BOrderSegment;

typedef struct {
    double           *x = NULL,
                     *y = NULL,
                     *z = NULL;
    int              npts;
    BOrderSegment    *borderList = NULL;
    int              numBorderList,
                     maxBorderList;
    BAseLine         *lineList = NULL;
    int              numLineList,
                     maxLineList;
    NOdeStruct       *nodes = NULL;
    int              num_nodes;
    EDgeStruct       *edges = NULL;
    int              num_edges;
    TRiangleStruct   *triangles = NULL;
    int              num_triangles;
    int              patchid;
    int              sgpflag;
    double           sgpdata[6];
} PRotoPatch;

typedef struct {
    double           *x = NULL,
                     *y = NULL,
                     *z = NULL;
    int              npts;
    CSW_F            *grid = NULL;
    int              ncol, nrow;
    double           xmin, ymin, xmax, ymax;
    double           *xp = NULL,
                     *yp = NULL,
                     *zp = NULL;
    int              np;
    BAseLine         *minor_lines = NULL;
    int              num_minor_lines;
    int              max_minor_lines;
} WOrkPoly;


// Define the main class

class PATCHSplit
{

  private:

    CSWGrdAPI      *grdapi_ptr = NULL;

    void    *v_jenv = NULL;
    void    *v_jobj = NULL;

  public:

    PATCHSplit () {};
    ~PATCHSplit () {};

    void SetGrdAPIPtr (CSWGrdAPI *p) {
        grdapi_ptr = p;
    }

    void SetJNIPtrs (void *env, void *obj) {
        v_jenv = env;
        v_jobj = obj;
    }

  private:

    CSWPolyUtils   ply_utils_obj;
    CSWPolyCalc    ply_calc_obj;
    CSWPolyUtils   csw_plyutils_obj;



    double         *OrigXBorder = NULL,
                   *OrigYBorder = NULL,
                   *OrigZBorder = NULL;
    int            OrigNBorder = 0;

    FaultConnect  *FConnect = NULL;

    int           local_calc_first = 1;



/*
 * Old file static functions become private class functions.
 */
   void WriteCenterlinesToFile (char *fname);

   void FreeData (void);
   void FreeSingleHorizonData (void);

   int BuildBorderPoints (
    BOrderSegment *blist,
    int           nlist,
    double        **xborder,
    double        **yborder,
    double        **zborder,
    int           *nborder
   );

   int ClipCenterlinesToBorder (void);

   int SamePoint (
    double x1,
    double y1,
    double x2,
    double y2
   );

   int SamePointXYZ (
    double x1,
    double y1,
    double z1,
    double x2,
    double y2,
    double z2
   );

   void UpdateLimits (
    double    *x,
    double    *y,
    double    *z,
    int       npts
   );

   void ClosestBoxCrossing (
    double    bxmin,
    double    bymin,
    double    bxmax,
    double    bymax,
    double    x1,
    double    y1,
    double    x2,
    double    y2,
    double    otherx,
    double    othery,
    double    *xpoint,
    double    *ypoint);

   int FixCrossingCenterlines (int shiftflag);
   int CorrectCloseToBorderLines (void);
   int ExtendCenterlines (void);
   int BuildWorkPolygons (void);
   int CalcWorkPolygonSurfaces (void);
   int AssignMinorLines (
    double  *xlines,
    double  *ylines,
    int     nlines,
    int     *ilines);

   int AddMinorLineToWorkPoly (
    WOrkPoly    *wp,
    double      *x,
    double      *y,
    int         npts,
    int         lock1,
    int         lock2);

   int BuildArraysFromBaseline (
      BAseLine    *blist, int nlist,
      double **xline, double **yline, double **zline,
      int **iline);

   int OnSegment (
    double x1,
    double y1,
    double x2,
    double y2,
    double x,
    double y);

   double BorderDistance (
    double *x,
    double *y,
    int npts,
    double xt,
    double yt);

   double ClineDistance (
    int    index,
    double xt,
    double yt);

   double MinorLineDistance (
    WOrkPoly  *wp,
    double    xt,
    double    yt);

   int InsideBorder (double x, double y);

   int _graze (double x1, double y1, double x2, double y2);
   int _graze2 (double x1, double y1, double x2, double y2);
   void csw_Free_clist (FAultCenterline **list, int n);
   void csw_Free_work_polys (void);

   int CheckFcenterCrossing (
    int                source_index,
    int                end_point_flag,
    double             *x1,
    double             *y1,
    double             *x2,
    double             *y2,
    double             tiny,
    double             *xpoint,
    double             *ypoint
   );

   void InterpolateClineZValues (FAultCenterline *fp);

   int BuildProtoPatchesFromFaultConnectDetachment (void);
   int BuildProtoPatchesFromFaultConnect (void);
   int BuildProtoPatchesFromInputModel (void);
   int BuildProtoPatchesFromSealedModel (void);
   int BuildSplitLines (void);
   int AddPointsToProtoPatch (PRotoPatch *prptr);
   int AddLinesToProtoPatch (PRotoPatch *prptr);
   int SendBackSealedModel (void);
   int SendBackInputModel (void);
   void SendBackProtoPatchContactLines (void);
   void SendBackSplitLines (void);
   int SendBackProtoTriMesh
    (NOdeStruct *nodes, int num_nodes,
     EDgeStruct *edges, int num_edges,
     TRiangleStruct *triangles, int num_triangles);
   void SendBackProtoPatches (void);
   int SendBackFaultMajorMinor (void);

   int AddToOrigPointsList (
    double const *xin, double const *yin, double const *zin,
    int npts, int id);

   int AddToOrigLinesList (
    BAseLine   const *blist,
    int nlines, int id);

   int AddToProtoContactLines (
    double    *x,
    double    *y,
    double    *z,
    int       npts,
    int       id1,
    int       id2);

   int AddToSplitLines (
    double    *x,
    double    *y,
    double    *z,
    int       npts,
    int       id1,
    int       id2);

   void FreeBaseLineArray (BAseLine *blist, int nlist);
   void FreePatchList (void);
   void FreeProtoContactLines (void);
   void FreeSplitLines (void);
   void FreeOrigLinesList (void);
   void FreeOrigPointsList (void);

/*
 * Old static file variables become private class variables.
 */
   double            AverageEdgeLength = -1.0;

   SealedModel       *SModel = NULL;

   int               DebugState = 0;

   BOrderSegment     *BorderIn = NULL;
   int               NumBorderIn = 0,
                     MaxBorderIn = 0;

   WOrkPoly          *WorkPolyList = NULL;
   int               NumWorkPolyList,
                     MaxWorkPolyList;

   FAultCenterline       *Fcenter = NULL;
   int                   NumFcenter = 0,
                         MaxFcenter = 0;

   double            *XPointsIn = NULL,
                     *YPointsIn = NULL,
                     *ZPointsIn = NULL;
   int               NumPointsIn = 0;
   int               MeshFlag = 0;

   BAseLine          *LinesIn = NULL;
   int               NumLinesIn = 0,
                     MaxLinesIn = 0;
   int               NumLinePoints = 0;

   BAseLine          *WorkLines = NULL;
   int               NumWorkLines = 0,
                     MaxWorkLines = 0;

   double            XYTiny = -1.e30;
   double            GrazeTiny = -1.e30;
   double            ZTiny = -1.e30;
   double            Xmin = 1.e30,
                     Ymin = 1.e30,
                     Xmax = -1.e30,
                     Ymax = -1.e30,
                     Zmin = 1.e30,
                     Zmax = -1.e30;

   double            BorderXint = 1.e30,
                     BorderYint = 1.e30;

   int               LastCrossingLine = -1;
   int               LastCrossingSegment = -1;
   double            LastXint = 1.e30;
   double            LastYint = 1.e30;

   double            BorderXmin = 1.e30;
   double            BorderYmin = 1.e30;
   double            BorderXmax = -1.e30;
   double            BorderYmax = -1.e30;

   double            LineTrimFraction = .05;

   PRotoPatch        *PatchList = NULL;
   int               NumPatchList = 0;

   COntactLine       *ProtoContactLines = NULL;
   int               NumProtoContactLines = 0;
   int               MaxProtoContactLines = 0;

   COntactLine       *SplitLines = NULL;
   int               NumSplitLines = 0;
   int               MaxSplitLines = 0;

   ORiginalPoints    *OrigPointsList = NULL;
   int               NumOrigPointsList = 0,
                     MaxOrigPointsList = 0;

   ORiginalLines     **OrigLinesList = NULL;
   int               NumOrigLinesList = 0,
                     MaxOrigLinesList = 0;

   double            ExtendFraction = 0.05;





  public:

/*
 * Function prototypes for patch splitting.
 */
   int ps_AddBorderSegment (
    double       *x,
    double       *y,
    double       *z,
    int          npts,
    int          type,
    int          flag);

   int ps_AddFaultCenterline (
    double       *x,
    double       *y,
    double       *z,
    int          npts,
    int          fault_id,
    int          flag);

   int ps_SetPoints (
    double       *x,
    double       *y,
    double       *z,
    int          npts,
    int          meshflag);

   int ps_AddLine (
    double       *x,
    double       *y,
    double       *z,
    int          npts,
    int          flag);

   int ps_AddFaultSurface (
    int          id,
    int          vused,
    double       *vbase,
    double       *x,
    double       *y,
    double       *z,
    int          num_nodes,
    int          *n1,
    int          *n2,
    int          *t1,
    int          *t2,
    int          num_edges,
    int          *e1,
    int          *e2,
    int          *e3,
    int          num_triangles,
    double       minage,
    double       maxage);

   int ps_AddFaultSurfaceWithDetachmentContact (
    int          id,
    int          vused,
    double       *vbase,
    double       *x,
    double       *y,
    double       *z,
    int          num_nodes,
    int          *n1,
    int          *n2,
    int          *t1,
    int          *t2,
    int          num_edges,
    int          *e1,
    int          *e2,
    int          *e3,
    int          num_triangles,
    double       minage,
    double       maxage,
    double       *xline,
    double       *yline,
    double       *zline,
    int          nline,
    int          detach_id
  );

   int ps_AddHorizonTriMeshPatch (
    int          id,
    double       age,
    double       *xnodes,
    double       *ynodes,
    double       *znodes,
    int          num_nodes,
    int          *n1edge,
    int          *n2edge,
    int          *t1edge,
    int          *t2edge,
    int          num_edges,
    int          *e1tri,
    int          *e2tri,
    int          *e3tri,
    int          num_tris);

   int ps_SetSedimentSurface (
    int          id,
    double       age,
    double       *xnodes,
    double       *ynodes,
    double       *znodes,
    int          num_nodes,
    int          *n1edge,
    int          *n2edge,
    int          *t1edge,
    int          *t2edge,
    int          num_edges,
    int          *e1tri,
    int          *e2tri,
    int          *e3tri,
    int          num_tris);

   int ps_SetModelBottom (
    int          id,
    double       age,
    double       *xnodes,
    double       *ynodes,
    double       *znodes,
    int          num_nodes,
    int          *n1edge,
    int          *n2edge,
    int          *t1edge,
    int          *t2edge,
    int          num_edges,
    int          *e1tri,
    int          *e2tri,
    int          *e3tri,
    int          num_tris);

   int ps_StartSealedModelDefinition (int marginPct);

   int ps_CalcSealedModel (double avspace);

   int ps_CalcDetachment (double lower_age, double upper_age, double age);

   int ps_GetSealedInput (void);

   void ps_SetDebugState (int ival);

   void ps_ClearAllData (void);
   void ps_ClearHorizonData (void);

   void ps_SetLineTrimFraction (double val);

   int ps_CalcSplit (int id, double age);

   void ps_SetModelBounds (
    double xmin,
    double ymin,
    double zmin,
    double xmax,
    double ymax,
    double zmax
   );

   int ps_CalcLinesForSplitting (void);
   int ps_StartFaultConnect (double avspace);
   int ps_EndFaultConnect (void);
   int ps_ConnectFaults (void);
   int ps_SetDetachment (
    double          *xnode,
    double          *ynode,
    double          *znode,
    int             num_nodes,
    int             *n1edge,
    int             *n2edge,
    int             *t1edge,
    int             *t2edge,
    int             num_edges,
    int             *e1tri,
    int             *e2tri,
    int             *e3tri,
    int             num_tris);
   int ps_AddToDetachment (
    double          *xnode,
    double          *ynode,
    double          *znode,
    int             num_nodes,
    int             *n1edge,
    int             *n2edge,
    int             *t1edge,
    int             *t2edge,
    int             num_edges,
    int             *e1tri,
    int             *e2tri,
    int             *e3tri,
    int             num_tris);
   int ps_AddConnectingFault (
    int             id,
    int             flag,
    int             sgpflag,
    double          *sgpdata,
    double          *xnode,
    double          *ynode,
    double          *znode,
    int             num_nodes,
    int             *n1edge,
    int             *n2edge,
    int             *t1edge,
    int             *t2edge,
    int             num_edges,
    int             *e1tri,
    int             *e2tri,
    int             *e3tri,
    int             num_tris);

   int ps_SetLowerSurface (
    double          *xnode,
    double          *ynode,
    double          *znode,
    int             num_nodes,
    int             *n1edge,
    int             *n2edge,
    int             *t1edge,
    int             *t2edge,
    int             num_edges,
    int             *e1tri,
    int             *e2tri,
    int             *e3tri,
    int             num_tris);
   int ps_SetUpperSurface (
    double          *xnode,
    double          *ynode,
    double          *znode,
    int             num_nodes,
    int             *n1edge,
    int             *n2edge,
    int             *t1edge,
    int             *t2edge,
    int             num_edges,
    int             *e1tri,
    int             *e2tri,
    int             *e3tri,
    int             num_tris);

   void ps_SetExtendFraction (double val);

   void ps_SetAverageEdgeLength (double avlen);

}; // end of main class


#endif
