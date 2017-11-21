
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_spline.h

    Define the GPFSpline class.  This refactors the old gpf_spline.c
    functions.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_spline.h.
#endif



#ifndef GPF_SPLINE_H
#define GPF_SPLINE_H

#include "csw/utils/include/csw_.h"


class GPFSpline
{

  public:

    GPFSpline () {};
    ~GPFSpline () {};

  private:

    int    GhostFlag = 0;

  public:

    int gpf_SplineFit2 (CSW_F *xy, int nin, int flag,
                        CSW_F *xyout, int maxout, CSW_F dres, int *nout);
    int gpf_SplineFit (CSW_F *x, CSW_F *y, int nin, int flag,
                       CSW_F *xout, CSW_F *yout, int maxout, CSW_F dres,
                       int *nout);
    int gpf_CalcSplineDeriv (CSW_F *x, CSW_F *y, int nin,
                             CSW_F *y2, CSW_F *u);
    int gpf_EvalSpline (CSW_F *x, CSW_F *y, int nin,
                        CSW_F *y2, CSW_F dxin, CSW_F *yout, int *nout);

};  // end of main class definition

#endif

/*
    end of header file
    add nothing below this endif
*/
