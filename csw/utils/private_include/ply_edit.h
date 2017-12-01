
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_edit.h

    This header has the class definition for the CSWPolyEdit class.  This
    class replaces the functions in the old ply_edit.c file.  The replacement
    with c++ methods in the class is mostly an exercise to learn c++ better.
    I also hope to make all this stuff thread safe, or at least to come much
    closer.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_edit.h.
#endif



#ifndef PLY_EDIT_H
#define PLY_EDIT_H

#include "csw/utils/include/csw_errnum.h"
#include "csw/utils/include/csw_timer.h"

/*
  define constants for class
*/

#define MAX_STATIC_POLYS    1000
#define BP_CHUNK            10000


/*
    define structures used only in this class
*/

typedef struct {
    double    x, y;
    void      *vp = NULL;
    int       iprev, inext;
    double    xint, yint;
    int       flag;
} _BOundaryPoint;


/*
 * Define a class to replace the old ply_edit.c functions.
 */
class CSWPolyEdit
{

  private:

    _BOundaryPoint      *BPList {NULL};
    int                 NumBPList {0},
                        MaxBPList {0};
    int                 BPChunk {BP_CHUNK};

    int                 StaticStartCompList[MAX_STATIC_POLYS];
    int                 *StartCompList {NULL};

    int                 StaticStartPolyList[MAX_STATIC_POLYS];
    int                 *StartPolyList {NULL};

    double              *Xout {NULL},
                        *Yout {NULL};
    void                **Vpout {NULL};
    int                 *Icout {NULL},
                        *Ipout {NULL},
                        Nout {0};
    int                 Npmax {0},
                        Ncmax {0};

    int                 *Icomp {NULL};
    int                 Npoly {0},
                        Ncomp {0};

    double              SamePointDistance {0.0};

    void    FreeMem (void);
    int     AppendToBPList (double x, double y,
                            void *vp, int iprev);
    int     SplitClosestEdge (double x, double y, void *vpt);
    int     UnionNewComponents (void);
    int     SamePoint (double x1, double y1, double x2, double y2);
    void    FindEdgeIntersections (double x1, double y1,
                                   double x2, double y2);
    _BOundaryPoint
               *FindClosestEdgeIntersection (double x, double y);


  public:

    CSWPolyEdit () {};
    ~CSWPolyEdit () 
    {
        FreeMem ();
    }

    int ply_add_points_to_boundary (double *xp, double *yp, void **vp,
                                    int *icomp, int *ipts, int npoly,
                                    double *xpts, double *ypts,
                                    void **vpts, int npts,
                                    double *xout, double *yout, void **vpout,
                                    int *icout, int *ipout, int *nout,
                                    int npmax, int ncmax);

    int ply_add_tie_line_to_boundary (double *xp, double *yp, void **vp,
                                      int *icomp, int *ipts, int npoly,
                                      double x1, double y1, void *vp1,
                                      double x2, double y2, void *vp2,
                                      double *xout, double *yout, void **vpout,
                                      int *icout, int *ipout, int *nout,
                                      int npmax, int ncmax);
}; // end of main class definition

/*
    end of header file
    add nothing below this endif
*/

#endif
