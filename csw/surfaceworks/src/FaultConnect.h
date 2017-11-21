
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#ifndef _FAULTCONNECT_H_
#define _FAULTCONNECT_H_

/*
 * System includes.
 */
#include <stdlib.h>

/*
 * general csw includes
 */
#include <csw/surfaceworks/include/grd_shared_structs.h>

class FaultConnect;

#include "csw/surfaceworks/src/PadSurfaceForSim.h"
#include "csw/surfaceworks/src/SurfaceGroupPlane.h"

/*
 * Define structures used by the FaultConnect class.
 */
typedef struct {
  NOdeStruct        *nodes;
  int               num_nodes;
  EDgeStruct        *edges;
  int               num_edges;
  TRiangleStruct    *tris;
  int               num_tris;
  double            *x,
                    *y,
                    *z;
  int               npts;
  int               flag;
  int               recalc;
  int               id;
  SurfaceGroupPlane *sgp;
} _SUrfStruct_;

typedef struct {
  double            *x,
                    *y,
                    *z;
  int               npts;
  int               fid;
} _ILineStruct_;


#include "csw/surfaceworks/include/grd_spatial3dtri.h"
#include "csw/surfaceworks/include/grd_xyindex.h"

#include "csw/surfaceworks/private_include/grd_fileio.h"
#include "csw/surfaceworks/private_include/grd_utils.h"
#include "csw/surfaceworks/private_include/grd_fault.h"
#include "csw/surfaceworks/private_include/grd_calc.h"
#include "csw/surfaceworks/private_include/grd_triangle_class.h"

/*
 * Define the FaultConnect class.  The implementation is in
 * csw/surfaceworks/src/FaultConnect.cc
 */
class FaultConnect {

 private:

  CSWGrdFileio  *grd_fileio_ptr = NULL;
  CSWGrdUtils   *grd_utils_ptr = NULL;
  CSWGrdFault   *grd_fault_ptr = NULL;
  CSWGrdCalc    *grd_calc_ptr = NULL;
  CSWGrdTriangle   *grd_triangle_ptr = NULL;

  PadSurface    pad_surface_obj;


 public:

/*
 * Constructor and destructor.
 */
  FaultConnect();
  virtual ~FaultConnect();

  void SetGrdFileioPtr (CSWGrdFileio *p) {grd_fileio_ptr = p;};
  void SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};
  void SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};
  void SetGrdCalcPtr (CSWGrdCalc *p) {grd_calc_ptr = p;};
  void SetGrdTrianglePtr (CSWGrdTriangle *p) {grd_triangle_ptr = p;};


/*
 * Public methods.
 */
  int setDetachment (
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
  );

  int addToDetachment (
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
  );

  int addToDetachment (
    double            *x,
    double            *y,
    double            *z,
    int               npts
  );

  int setFault (
    int               id,
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
  );

  int setFault (
    int               id,
    int               sgpflag,
    double            *sgpdata,
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
  );

  int addToFault (
    int               id,
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
  );

  int addToFault (
    int               id,
    double            *x,
    double            *y,
    double            *z,
    int               npts
  );

  int connectFaults (void);

  int getConnectedDetachment (
    NOdeStruct        **nodes,
    int               *num_nodes,
    EDgeStruct        **edges,
    int               *num_edges,
    TRiangleStruct    **tris,
    int               *num_tris,
    int               *sgpflag,
    double            *sgpdata
  );

  int getNumberOfConnectedFaults (void);

  int getConnectedFault (
    int               index,
    int               *id,
    NOdeStruct        **nodes,
    int               *num_nodes,
    EDgeStruct        **edges,
    int               *num_edges,
    TRiangleStruct    **tris,
    int               *num_tris,
    int               *sgpflag,
    double            *sgpdata
  );

  int getFaultContactLine (
    int               index,
    int               *id,
    double            **x,
    double            **y,
    double            **z,
    int               *npts);

  int setDetachmentLowerSurface (
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
  );

  int setDetachmentUpperSurface (
    NOdeStruct        *nodes,
    int               num_nodes,
    EDgeStruct        *edges,
    int               num_edges,
    TRiangleStruct    *tris,
    int               num_tris
  );

  int calcDetachment (double lower_age, double upper_age, double age);

  int getCalculatedDetachment (
    NOdeStruct        **nodes,
    int               *num_nodes,
    EDgeStruct        **edges,
    int               *num_edges,
    TRiangleStruct    **tris,
    int               *num_tris,
    int               *sgpflag,
    double            *sgpdata
  );

  void clearCalculatedDetachmentData (void);

  void setAverageSpacing (double avspace);

/*
 * Public data members.
 */

 private:

/*
 * Private data members.
 */
  _SUrfStruct_        *detach_surf,
                      *detach_lower_surf,
                      *detach_upper_surf,
                      *detach_calc_surf,
                      *pad_detach_surf,
                      *sealed_detach_surf,
                      *input_faults,
                      *pad_faults,
                      *seal_faults;
  int                 num_input_faults,
                      num_pad_faults,
                      num_seal_faults;
  int                 max_input_faults,
                      max_pad_faults,
                      max_seal_faults;

  _ILineStruct_       *fault_contacts,
                      *seal_fault_contacts;
  int                 num_fault_contacts,
                      num_seal_fault_contacts;

  double              dcoefs[3];

  double              dxmin, dymin, dxmax, dymax;
  double              dzmin, dzmax;

  double              tnxNorm, tnyNorm, tnzNorm;

  double              average_edge_length;

  int                 test_data_flag;

/*
 * Private methods.
 */
  void                init (void);
  void                free_mem (void);

  void                FreeLine (_ILineStruct_ *list, int nlist);
  void                FreeSurf (_SUrfStruct_ *list, int nlist);
  void                CleanLine (_ILineStruct_ *list);
  void                CleanSurf (_SUrfStruct_ *list);

  _SUrfStruct_        *FindInputFaultForID (int id);
  _SUrfStruct_        *FindPadFaultForID (int id);
  _SUrfStruct_        *FindSealFaultForID (int id);
  _SUrfStruct_        *NextInputFaultSurf (void);
  _SUrfStruct_        *NextPadFaultSurf (void);
  _SUrfStruct_        *NextSealFaultSurf (void);

  int                 CalcDetachmentPlane
                        (double *xa, double *ya, double *za, int npts);
  double              DetachmentPlaneDist
                        (double x, double y, double z);
  int                 PadDetachment (void);
  int                 PadFault (int index);

  double              EdgeLength (EDgeStruct *eptr, NOdeStruct *nodes);

  void                TriangleNormal (double *x, double *y, double *z);

  void                CalcAverageEdgeLength (void);

  void                CalcXYZLimits (void);

  void CreateTestData (double *xa,
                       double *ya,
                       double *za,
                       int *npts);

};

#endif
