
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_traverse.h
    
    This header has the class definition for the CSWPolyTraverse
    class.  This class refactors the functions found previously
    in ply_traverse.c.  The main reason for the refactoer is to 
    get me some additional experience with c++.  Another reason is
    to try and get the csw libraary code thread safe, or at least
    a lot closer to thread safe.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_traverse.h.
#endif



#ifndef PLY_TRAVERSE_H
#define PLY_TRAVERSE_H

#include "csw/utils/include/csw_errnum.h"
#include "csw/utils/include/csw_timer.h"

#include "csw/utils/private_include/ply_gridvec.h"


class CSWPolyDrivers;

/*
 * Definitiopn for the main CSWPolyTraverse class
 */
class CSWPolyTraverse
{

  private:

    int          orflag {0};
    int          lastx2 {0},
                 PrevInOut {0},
                 orflag2 {0};
    double       CloseX {1.e30f},
                 CloseY {1.e30f};

    int          MaxTrvOut {0};

    CSWPolyDrivers   *ply_drivers_ptr = NULL;


  public:

    CSWPolyTraverse () {};
    ~CSWPolyTraverse () {};

// It makes no sense to copy construct, move construct,
// assign or move assign an object of this class.  The
// various copy methods are flagged "delete" to prevent
// their use.

    CSWPolyTraverse (const CSWPolyTraverse &old) = delete;
    const CSWPolyTraverse &operator=(const CSWPolyTraverse &old) = delete;
    CSWPolyTraverse (CSWPolyTraverse &&old) = delete;
    const CSWPolyTraverse &operator=(CSWPolyTraverse &&old) = delete;

    void SetDriverPtr (CSWPolyDrivers *p) {ply_drivers_ptr = p;};

    int ply_SetMaxTrvOut (int val);
    int ply_trvint (double *xvply, double *yvply,
                    int nvpt, int startv, int *nextv, int cflag,
                    double xstart, double ystart, int iflag,
                    double **xpoly, double **ypoly, int npoly, int *npts,
                    double *xminp, double *yminp, double *xmaxp, double *ymaxp,
                    double *xint, double *yint, int *npint, int *nsint,
                    double *xout, double *yout, int *nout, int outflag);
    int ply_GetPrevInOut (void);
    int ply_SetPrevInOut (int val);
    int ply_trvintinit (const char *string, double val);
    int ply_sidint(CSWPolyGridvec &ply_gridvec_obj,
                   double **xpoly, double **ypoly, int npoly, int *npts,
                   double x1, double y1, double x2, double y2,
                   double *xout, double *yout, int *nhole, int *npoint);
    int ply_sidint(double **xpoly, double **ypoly, int npoly, int *npts,
                   double x1, double y1, double x2, double y2,
                   double *xout, double *yout, int *nhole, int *npoint);
    int ply_sidintinit (const char *string, double val);
    int ply_setclosingpoint (double x, double y);
    int ply_getclosingpoint (double *x, double *y);
    int ply_ClipVecToArea(double **xpoly, double **ypoly, int npoly, int *npts,
                          double x1, double y1, double x2, double y2,
                          double *xout, double *yout);

}; // end of main class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
