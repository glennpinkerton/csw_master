
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_cutline.h

    This header defines the CSWPolyCutline class.  This is a refactor
    of the old ply_cutline.c free standing C functions.  The primary
    reason for the refactor is to get more C++ experience.  The secondary
    refactor reason is to make the code thread safe, or at least a lot 
    closer to thread safe.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file ply_cutline.h.
#endif



#ifndef PLY_CUTLINE_H
#define PLY_CUTLINE_H

#include "csw/utils/include/csw_errnum.h"
#include "csw/utils/include/csw_timer.h"


class CSWPolyCutline
{

  private:

    double              **XHoles {NULL},
                        **YHoles {NULL},
                        *Tmp {NULL};
    int                 *IHoles {NULL},
                        NHoles {0};


/*
    function "prototypes"
*/

    int InsertAHole (double *xp, double *yp, int nnin,
                     double *xhole, double *yhole, int nholein, int *nnout);


  public:

    CSWPolyCutline () {};
    ~CSWPolyCutline () {};

// It makes no sense to copy construct, move construct,
// assign or move assign an object of this class.  The
// various copy methods are flagged "delete" to prevent
// their use.

    CSWPolyCutline (const CSWPolyCutline &old) = delete;
    const CSWPolyCutline &operator=(const CSWPolyCutline &old) = delete;
    CSWPolyCutline (CSWPolyCutline &&old) = delete;
    const CSWPolyCutline &operator=(CSWPolyCutline &&old) = delete;

    int ply_AddCutLines (double *xin, double *yin, int ncompin, int *nptsin,
                         double *xout, double *yout, int *npolyout, int *nptsout);

}; // end of main class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
