
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_drivers.h

    Define the CSWPolyDrivers class, which replaces the functions 
    previously in ply_drivers.c.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_drivers.h.
#endif



#ifndef PLY_DRIVERS_H
#define PLY_DRIVERS_H

#include "csw/utils/private_include/ply_gridvec.h"

/*
    define structures for this file.
*/
typedef struct {
    int         nps;
    int         off;
} CList;

typedef struct {
    double    *xpoly, *ypoly, *xpa[1], *ypa[1];
    int       iflag, *ic, nc, npt, icomp;
    char      scflag, scf2;
} POlyrec;



class CSWPolyDrivers
{

  private:

    int         orflag {1};
    int         ToggleFlag {0};
    int         Ntotnr {0};

    CSWPolyTraverse  ply_traverse_obj;


  public:

    CSWPolyDrivers () {
        ply_traverse_obj.SetDriverPtr (this);
    };

    int ply_toggleflag (void);
    int ply_intcomps (CSWErrNum &err_obj,
                      CSWPolyGridvec  &ply_gridvec_obj,
                      double *xpolys, double *ypolys, int ncomps, int *icomps,
                      double **xps, double **yps,
                      double *xmins, double *ymins, double *xmaxs, double *ymaxs,
                      double *xpolyc, double *ypolyc, int ncompc, int *icompc,
                      double **xpc, double **ypc,
                      double *xminc, double *yminc, double *xmaxc, double *ymaxc,
                      double *xpoutr, double *ypoutr,
                      int *npcout, int *nhcout, int *ipcout,
                      double *extrax, double *extray,
                      int *nextra, int *extraholes, int maxextra, int maxextraholes);
    int ply_orcomps (double *xpolys, double *ypolys, int ncomps, int *icomps,
                     double **xps, double **yps,
                     double *xmins, double *ymins, double *xmaxs, double *ymaxs,
                     double *xpolyc, double *ypolyc, int ncompc, int *icompc,
                     double **xpc, double **ypc,
                     double *xminc, double *yminc, double *xmaxc, double *ymaxc,
                     double *xpoutr, double *ypoutr, int *npcout, int *nhcout,
                     int *ipcout);
    int ply_iandcomp(
                     CSWPolyGridvec  &ply_gridvec_obj,
                     double xfirst, double yfirst, int firstin, int nowcomp, int firstv,
                     int ncomps, int *icomps,
                     double *xminps, double *yminps, double *xmaxps, double *ymaxps,
                     int ncompc, int *icompc,
                     double *xminpc, double *yminpc, double *xmaxpc, double *ymaxpc,
                     double **xps, double **yps, double **xpc, double **ypc,
                     double *xcomp, double *ycomp, int *npcomp, int *nextvs,
                     double *xnext, double *ynext);
    int ply_iorcomp(double xfirst, double yfirst, int firstin, int nowcomp, int firstv,
                    int ncomps, int *icomps,
                    double *xminps, double *yminps, double *xmaxps, double *ymaxps,
                    int ncompc, int *icompc,
                    double *xminpc, double *yminpc, double *xmaxpc, double *ymaxpc,
                    double **xps, double **yps, double **xpc, double **ypc,
                    double *xcomp, double *ycomp, int *npcomp, int *nextvs,
                    double *xnext, double *ynext);
    int ply_iorcompinit (const char *string, double val);
    int ply_holint  (double *xpolys, double *ypolys, int ncomps, int *icomps,
                     int *jcomps, int nst,
                     double *xpolyc, double *ypolyc, int ncompc, int *icompc,
                     int *jcompc, int nct,
                     double *xpolyr, double *ypolyr, int *ncompr, int *icompr,
                     int *nrt);
    int ply_holunion (double **xhic, double **yhic, int *nhic, int nh,
                      int *icomps, int *icompc,
                      double *xmin, double *ymin, double *xmax, double *ymax,
                      double *xhr, double *yhr, int *nhr, int *ihr, int *iphr,
                      double *extrax, double *extray,
                      int *nextra, int *extraholes, int maxextra, int maxextraholes);

}; // end of main class definition


#endif
/*
    end of header file
    add nothing below this endif
*/
