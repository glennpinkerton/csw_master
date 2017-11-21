
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#ifndef _PADSURFACE_H_
#define _PADSURFACE_H_

class PadSurface;

#include "csw/surfaceworks/include/grd_shared_structs.h"
#include "csw/surfaceworks/include/grid_api.h"

#include "csw/surfaceworks/src/SurfaceGroupPlane.h"

class PadSurface
{

  private:

    CSWGrdAPI            *grd_api_ptr = NULL;

// Old static file variables become private class variables.

    CSW_F                *ShapeGrid = NULL;
    int                  Ncol = 0,
                         Nrow = 0;
    double               GXmin = 1.e30,
                         GYmin = 1.e30,
                         GXmax = -1.e30,
                         GYmax = -1.e30;

    SurfaceGroupPlane    *Sgp = NULL;

    int                  SimPaddingFlag = 0;

    double               *XDetach = NULL,
                         *YDetach = NULL,
                         *ZDetach = NULL;
    int                  NDetach = 0;


/*
 * Old static file functions become private class methods.
 */
    void ProcessLowList (
        double *za,
        int *lowlist, int nlow,
        double xmin, double xmax,
        double *ymin,
        double *ymax);

    void ProcessHighList (
        double *za,
        int *highlist, int nhigh,
        double xmin, double xmax,
        double *ymin,
        double *ymax);

  public:

    PadSurface () {};
    ~PadSurface () {};

    void SetGrdAPIPtr (CSWGrdAPI *p) {grd_api_ptr = p;};
    
// This function uses grid and trimesh functions to create a new surface which
// is padded out to the specified extents.

    int PadSurfaceForSim(
      NOdeStruct     *nodes_in,
      EDgeStruct     *edges_in,
      TRiangleStruct *tris_in,
      int            num_nodes_in,
      int            num_edges_in,
      int            num_tris_in,
      double         xmin,
      double         xmax,
      double         ymin,
      double         ymax,
      double         zmin,
      double         zmax,
      CSW_F          **grid_out,
      int            *ncol_out,
      int            *nrow_out,
      double         *gxmin_out,
      double         *gymin_out,
      double         *gxmax_out,
      double         *gymax_out,
      NOdeStruct     **nodes_out,
      EDgeStruct     **edges_out,
      TRiangleStruct **tris_out,
      int            *num_nodes_out,
      int            *num_edges_out,
      int            *num_tris_out,
      double         avspace
    );
    
    int PadFaultSurfaceForSim(
      NOdeStruct     *nodes_in,
      EDgeStruct     *edges_in,
      TRiangleStruct *tris_in,
      int            num_nodes_in,
      int            num_edges_in,
      int            num_tris_in,
      double         xmin,
      double         xmax,
      double         ymin,
      double         ymax,
      double         zmin,
      double         zmax,
      CSW_F          **grid_out,
      int            *ncol_out,
      int            *nrow_out,
      double         *gxmin_out,
      double         *gymin_out,
      double         *gxmax_out,
      double         *gymax_out,
      NOdeStruct     **nodes_out,
      EDgeStruct     **edges_out,
      TRiangleStruct **tris_out,
      int            *num_nodes_out,
      int            *num_edges_out,
      int            *num_tris_out,
      double         avspace,
      double         minage,
      double         maxage
    );
    
    int PadFaultSurfaceForSim(
      double         *xin,
      double         *yin,
      double         *zin,
      int            nptsin,
      int            *lowlist,
      int            nlow,
      double         xmin,
      double         xmax,
      double         ymin,
      double         ymax,
      double         zmin,
      double         zmax,
      CSW_F          **grid_out,
      int            *ncol_out,
      int            *nrow_out,
      double         *gxmin_out,
      double         *gymin_out,
      double         *gxmax_out,
      double         *gymax_out,
      NOdeStruct     **nodes_out,
      EDgeStruct     **edges_out,
      TRiangleStruct **tris_out,
      int            *num_nodes_out,
      int            *num_edges_out,
      int            *num_tris_out,
      double         avspace
    );
    
    int PadFaultSurfaceForSim(
      NOdeStruct     *nodes,
      EDgeStruct     *edges,
      TRiangleStruct *tris,
      int            num_nodes,
      int            num_edges,
      int            num_tris,
      int            *lowlist,
      int            nlow,
      int            *highlist,
      int            nhigh,
      double         max_low_extend,
      double         max_high_extend,
      double         xminin,
      double         xmaxin,
      double         yminin,
      double         ymaxin,
      double         zminin,
      double         zmaxin,
      CSW_F          **grid_out,
      int            *ncol_out,
      int            *nrow_out,
      double         *gxmin_out,
      double         *gymin_out,
      double         *gxmax_out,
      double         *gymax_out,
      NOdeStruct     **nodes_out,
      EDgeStruct     **edges_out,
      TRiangleStruct **tris_out,
      int            *num_nodes_out,
      int            *num_edges_out,
      int            *num_tris_out,
      double         avspace,
      double         minage,
      double         maxage
    );
    
    void PadSetPadShapeGrid (
      CSW_F         *grid,
      int           ncol,
      int           nrow,
      double        xmin,
      double        ymin,
      double        xmax,
      double        ymax);
    
    void PadSetSimPaddingFlag (int ival);
    
    void PadSetSurfaceGroupPlane (SurfaceGroupPlane *sptr);
    
    void PadSetDetachmentContact (
      double        *xline,
      double        *yline,
      double        *zline,
      int           nline);
    
};  // end of main class definition
    
#endif
