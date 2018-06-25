
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_stats.h

    Define the CSWGrdStats class.  This refactors code originally in
    grd_stats.c
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file grd_stats.h.
#endif


/*
    Add nothing above this line
*/
#ifndef GRD_STATS_H
#  define GRD_STATS_H

#  include "csw/utils/include/csw_trig_consts.h"

#  include "csw/surfaceworks/include/grd_shared_structs.h"

/*
    Define macros for the file
*/
#define MAX_STATS_BINS           25
#define MAX_ELLIPSE              100
#define NDIR                     16
#define NLOCALDIR                6
#define DEGREES                  57.29583f

#define MAX_GLOBAL_POINTS        1000

#define ABSOLUTE_TINY       (0.0)

class CSWGrdStats;

#include "csw/surfaceworks/private_include/grd_fault.h"

class CSWGrdStats
{

  private:

    CSWGrdFault   *grd_fault_ptr = NULL;
    CSWGrdUtils   *grd_utils_ptr = NULL;

  public:

    void   SetGrdFaultPtr (CSWGrdFault *p) {grd_fault_ptr = p;};
    void   SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

    int    grd_global_anisotropy (CSW_F *x, CSW_F *y, CSW_F *z, int n, int nbins,
                                  CSW_F *strike, int *power, CSW_F *aratio, int thickflag);
    int    grd_local_anisotropy (int *local, int nlocal,
                                 CSW_F *x, CSW_F *y, CSW_F *z, int n,
                                 CSW_F *strike, int *power, CSW_F *aratio,
                                 int thickflag);

    int    grd_nugget_effect 
              (CSW_F *xin, CSW_F *yin, CSW_F *zin, int nin,
               CSW_F *global_delta, CSW_F *av_local_delta);

  private:

/*
  Old static file functions become private class methods.
*/
    int    FillInVariogram (CSW_F *bins, int nbins);
    int    AnalyseEllipse (CSW_F *knees, int ndir,
                           CSW_F *strike, int *power, CSW_F *aratio);
    int    FindKnee (CSW_F *bins, int nbins, CSW_F d1, CSW_F d2,
                     CSW_F *zmaxptr, CSW_F *knee);

}; // end of main class definition

/*
    Add nothing to this file below the following endif
*/
#endif
