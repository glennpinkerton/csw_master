
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_ctog.h

    Define the CSWGrdCtog class.  This class refactors the old
    grd_ctog.c stuff.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grd_ctog.h.
#endif


/*
    Add nothing above this line
*/
#ifndef GRD_CTOG_H
#  define GRD_CTOG_H

#  include "csw/surfaceworks/include/grd_shared_structs.h"



/*
 * structures used only in this class.
 */
typedef struct {
    double pct1;
    double zlev1;
    double pct2;
    double zlev2;
    int    ncross;
} _CRossing;

typedef struct {
    double x1, y1, z1,
           x2, y2, z2;
} _FAultVector;

class CSWGrdCtog;

#include "csw/surfaceworks/private_include/grd_utils.h"

class CSWGrdCtog
{

  private:

    CSWGrdUtils    *grd_utils_ptr = NULL;
    CSWGrdFault    *grd_fault_ptr = NULL;


  public:

    CSWGrdCtog () {};
    ~CSWGrdCtog () {};

    void SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};
    void SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};

  private:

/*
  Old static file variables become private class variables.
*/
    _CRossing               *Hcross = NULL,
                            *Vcross = NULL;
    double                  Xmin {0.0}, Ymin {0.0},
                            Xmax {0.0}, Ymax {0.0},
                            Zmin {0.0}, Zmax {0.0},
                            Xspace {0.0}, Yspace {0.0};
    double                  Ztiny = 0.0;
    int                     Ncol {0}, Nrow {0};
    CSW_F                   *Wgrid = NULL;
    int                     *ClosestCrossing = NULL;

    int                     FaultFlag = 0;
    _FAultVector            *FaultVectorList = NULL;
    int                     NumFaultVectors = 0,
                            MaxFaultVectors = 0;



    void SetVectorCrossings (double x1, double y1, double z1,
                             double x2, double y2, double z2);
    void SetCrossingStruct (_CRossing *crptr,
                            double pct,
                            double zlev);
    int InterpolateOnBoundary (void);
    int InterpolateBetweenCrossings (void);
    int CalculateCornerPoints (void);
    void PlaneFit (int nodenum);
    void FreeData (void);
    void FilterPointsForFaults (int nodenum,
                                double *x, 
                                double *y,
                                double *z,
                                int *npts);
    int AddFaultVector (double x1, double y1, double z1,
                        double x2, double y2, double z2);



  public:

    int grd_contours_only (COntourDataStruct *clinesin,
                           int nclines,
                           double xmin, double ymin, double xmax, double ymax,
                           int ncol, int nrow,
                           CSW_F *grid, char *mask);

}; // end of main class

/*
    Add nothing to this file below the following endif
*/
#endif
