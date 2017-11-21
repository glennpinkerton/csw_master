
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_shape.h

    Define the GPFShape class.  This class refactors the old gpf_shapes.c
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
#error Illegal attempt to include private header file gpf_shape.h.
#endif



#ifndef GPF_SHAPE_P_H
#define GPF_SHAPE_P_H

#include "csw/utils/include/csw_trig_consts.h"

#define MAXFASTCIRCLE            1024


class GPFShape
{
  public:

    GPFShape () {};
    ~GPFShape () {};

  private:

    double p_xc [MAXFASTCIRCLE + 10];
    double p_yc [MAXFASTCIRCLE + 10];
    int    p_first = 1;


  public:

    int gpf_arclimits (CSW_F xc, CSW_F yc, CSW_F r1, CSW_F r2,
                       CSW_F ang1, CSW_F anglen, CSW_F rotang, int flag,
                       CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gpf_makeoriginbox (CSW_F width, CSW_F height, CSW_F *x, CSW_F *y);
    int gpf_curveboxcorners (CSW_F *xr, CSW_F *yr, CSW_F crad,
                             CSW_F **xout, CSW_F **yout, int *nout);
    int gpf_fastcirclegen (CSW_F r, int n, CSW_F *x, CSW_F *y, int *nout);
    int gpf_rotatepoints (CSW_F *x, CSW_F *y, int npt, CSW_F ang);
    int gpf_translatepoints (CSW_F *x, CSW_F *y, int npt,
                             CSW_F x0, CSW_F y0);
    int gpf_fudgeangle (CSW_F *ang, CSW_F mod);
    int gpf_calcarcpoints (CSW_F r1, CSW_F r2, CSW_F ang1, CSW_F ang2,
                           int nmax, int fillflag,
                           CSW_F *xy, int *npts);
    int gpf_calcarcpoints2 (CSW_F r1, CSW_F r2, CSW_F ang1in, CSW_F anglen,
                            int nmax, int fillflag,
                            CSW_F *xout, CSW_F *yout, int *nout);
    int gpf_calc_point_on_arc (CSW_F xc, CSW_F yc, CSW_F r1, CSW_F r2,
                               CSW_F rotang, CSW_F angle,
                               CSW_F *x, CSW_F *y);
    int gpf_boxlltocenter (CSW_F x, CSW_F y, CSW_F w, CSW_F h, CSW_F ang,
                           CSW_F *xc, CSW_F *yc);
    int gpf_boxcentertoll (CSW_F xc, CSW_F yc, CSW_F w, CSW_F h, CSW_F ang,
                           CSW_F *x, CSW_F *y);
    int gpf_boxlimits (CSW_F xc, CSW_F yc, CSW_F w, CSW_F h, CSW_F ang,
                       CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gpf_calcboxpoints (CSW_F xc, CSW_F yc, CSW_F w, CSW_F h, CSW_F ang,
                           CSW_F *xr, CSW_F *yr);

}; // end of class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
