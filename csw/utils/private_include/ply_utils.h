
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_utils.h

    Refactor the old ply_utils.c functions into a CSWPolyUtils class
    definition.  The rationale behind this is to get experience in
    this sort of refactoring and also to try and make the csw code
    thread safe.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_utils.h.
#endif



#ifndef PLY_UTILS_H
#define PLY_UTILS_H

#include "csw/utils/include/csw_errnum.h"
#include "csw/utils/include/csw_timer.h"

#define MAX_GRID_SIZE      250000

/*
    define structures used in the main class
 */
typedef struct {
    int        off;
    int        flg;
    int        nps;
} HList;

typedef struct {
    int        ply;
    int        hol;
} PList;


/*
 * Main class definition starts here.
 */
class CSWPolyUtils
{

  public:

    CSWPolyUtils () {};
    ~CSWPolyUtils ()
    {
        csw_Free (xint);
        csw_Free (yint);
        xint = NULL;
        yint = NULL;
    };

  private:

/*
 *  private variables (used to be static file or function
 *  variables in the old c file)
 */
    double              Epsilon {1.e-8};
    double              MinusEpsilon {-1.e-8};
   
    double              HoleFlag   {1.e29},
                        ScanFlag   {0.0},
                        MaxPout    {0.0},
                        MaxHout    {0.0},
                        MultMem    {3.0};

    int                 MaxPts {0};

    int                 numint {0}, maxint {0};
    double              *xint {NULL}, *yint {NULL};

    double              holeflag   {1.e29},
                        scanflag   {0.0},
                        maxpout    {0.0},
                        maxhout    {0.0},
                        maxpmem    {10000.0},
                        maxhmem    {1000.0},
                        multmem    {3.0};



/*
 *  private methods (used to be static file functions in old c file)
 */
    int CorrectForSmallHoles (double *xmin, double *ymin, double *xmax, double *ymax,
                              int ncomp,
                              double xgmin, double ygmin, double xspace, double yspace,
                              int nd1, int ncol, int nrow,
                              char *iogrid, char *bcell);

    int ply_apndholxyz
                (double *xpoly, double *ypoly, double *zpoly,
                 int *ncomp, int *icomp, int *npt,
                 double *xhole, double *yhole, double *zhole,
                 int holpts, int holev);

    double _minDist (
        double x1, double y1, double x2, double y2,
        double x3, double y3, double x4, double y4);

  public:

    int ply_apndhol (double *xpoly, double *ypoly, int *ncomp, int *icomp, int *npt,
                     double *xhole, double *yhole, int holpts, int holev);
    int ply_copy (double *xpoly, double *ypoly, int npts, double *xpout, double *ypout);
    int ply_globalim (double *xmin, double *ymin, double *xmax, double *ymax, int npts,
                      double *gxmin, double *gymin, double *gxmax, double *gymax);
    int ply_graze(double x, double y, double xc, double yc);
    int ply_graze1 (CSW_F x, CSW_F y, CSW_F xc, CSW_F yc);
    int ply_setgraze(double x);
    int ply_getgraze(double *x);
    int ply_grid_set (double *xpmin, double *ypmin, double *xpmax, double *ypmax,
                      int numpoly,
                      double xgmin, double ygmin, double xgmax, double ygmax,
                      double *xspace, double *yspace);
    int ply_holdel (double *xpoly, double *ypoly,
                    int *ncomp, int *icomp, int nhole);
    int ply_holnest (double *xpoly, double *ypoly, int ncomp, int *icomp,
                     double *xpout, double *ypout, int *npout, int *ncout,
                     int *icout);
    int ply_holnestxyz (double *xpoly, double *ypoly, double *zpoly,
                        int ncomp, int *icomp,
                        double *xpout, double *ypout, double *zpout,
                        int *npout, int *ncout, int *icout,
                        int maxpts, int maxcomp);
    int ply_intop (char flag, double x, double y, int nsize);
    int ply_limits ( double *xpoly, double *ypoly, int *packpoly, int numpoly,
                     double *xpmin, double *ypmin, double *xpmax, double *ypmax,
                     double *xgmin, double *ygmin, double *xgmax, double *ygmax);
    int ply_glimits ( double *xpoly, double *ypoly, int *packpoly, int numpoly,
                      double *xgmin, double *ygmin, double *xgmax, double *ygmax);
    int ply_orient (int flag, double *xpoly, double *ypoly, int npts,
                    double xpmin, double ypmin, double xpmax, double ypmax);
    int ply_parms(char flag, const char *string, double *val);
    int ply_reorg(double *xpoly, double *ypoly, int npts,
                  double xmin, double ymin, char flag);
    int ply_revers (double *xpoly, double *ypoly, int npts);
    int ply_rotpts (double *x, double *y, int *nptsin, int nrot);
    int ply_scan(char *hgrid, char *vgrid, char *iogrid, char *bcell,
                 int nd1, int nd2, int ncol, int nrow,
                 double *xpoly, double *ypoly, int *packpoly, int numpoly,
                 double xgmin, double ygmin, double xspace, double yspace,
                 double *xpmin, double *ypmin, double *xpmax, double *ypmax);
    int ply_scan_integer(char *hgrid, char *vgrid, char *iogrid, char *bcell,
                         int nd1, int nd2, int ncol, int nrow,
                         int *x1, int *y1, int *x2, int *y2, int nvec);
    int ply_scan1(double **xpoly, double **ypoly, int *npts, int ncomp,
                  char *hgrid, char *vgrid, char *iogrid, char *bcell,
                  int ncol, int nrow, int nd1, int nd2,
                  double xspace, double yspace, double xgmin, double ygmin,
                  double xmin, double ymin, double xmax, double ymax);
    int ply_segint(double x1, double y1, double x2, double y2,
                   double x3, double y3, double x4, double y4,
                   double *x, double *y);
    int ply_segint_integer (int x1, int y1, int x2, int y2,
                            int x3, int y3, int x4, int y4, int *x, int *y);
    int ply_side_set (char *hgrid, char *vgrid,
                      int nd1, int nd2, int ncol, int nrow,
                      double xspac, double yspac, double xmin, double ymin,
                      double *xpoly, double *ypoly, int *packpoly, int npoly );
    int ply_xvec (double x1, double y1, double x2, double y2,
                  char *hgrid, char *vgrid,
                  int nd1, int nd2, int ncol, int nrow,
                  double xspac, double yspac, double xmin, double ymin);
    int ply_point_integer (int *x1, int *y1, int *x2, int *y2,
                           int numvec, int p, int q);
    int ply_point (double *xpoly, double *ypoly,
                   int numvert,
                   double p, double q);
    int ply_point1 (CSW_F *xpoly, CSW_F *ypoly,
                    int numvert,
                    CSW_F p, CSW_F q);
    int ply_pointpa (double **xv, double **yv, int nc,
                     int *ic, double p, double q);
    int ply_pointpa1 (CSW_F **xv, CSW_F **yv, int nc,
                      int *ic, CSW_F p, CSW_F q);
    int ply_points(double *xpoly, double *ypoly, int *packpoly, int numpoly,
                   double *xpts, double *ypts, int npts,
                   int *inside);
    int ply_spoint (double *xpoly, double *ypoly,
                    int *polypack, int npoly,
                    double p, double q,
                    double *xpmin, double *ypmin, double *xpmax, double *ypmax);
    int ply_PolyInsidePoly1 (CSW_F *x1, CSW_F *y1, int *icomp1, int ncomp1,
                             CSW_F graze, CSW_F *x2, CSW_F *y2, int npts);
    int ply_PolyInsidePoly2 (double *x1, double *y1, int *icomp1, int ncomp1,
                             double graze, double *x2, double *y2, int npts);
    int ply_pointonlineseg (double x1, double y1, double x2, double y2,
                            double x3, double y3);
    int ply_CalcSmallOffset (double x1, double y1, double x2, double y2,
                             double *x3, double *y3);
    int ply_extendlineseg (double x1, double y1, double *x2, double *y2);
    int ply_CalcLeftOffset (int mult,
                            double x1, double y1, double x2, double y2,
                            double *x3, double *y3);
    int ply_thinoutdups (double *x, double *y, int *npts);
    int ply_checkedge (double x1, double y1, double x2, double y2,
                       double **xp, double **yp, int np, int *ip);
    int ply_PlineInOut (double *xp1, double *yp1,
                        int np1, int *ip1, int *jp1,
                        double *x1, double *y1, int n1);
    int ply_UtilMaxPts (int val);

}; // end of main class definition


#endif
/*
    end of header file
    add nothing below this endif
*/
