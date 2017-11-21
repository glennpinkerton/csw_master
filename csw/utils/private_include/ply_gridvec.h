
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    ply_gridvec.h

    This header defines the CSWPolyGridvec class, which is a refactor of the
    old functions in ply_gridvec.c.  The refactor was done mostly for getting
    more C++ experience.  I also hope to get the code much closer to being
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
#error Illegal attempt to include private header file ply_gridvec.h.
#endif



#ifndef PLY_GRIDVEC_H
#define PLY_GRIDVEC_H


typedef struct {
    double       x1,
                 y1,
                 x2,
                 y2,
                 xg1,
                 yg1,
                 spac;
    char         *grid = NULL;
    int          nc,
                 nr;
}  POlyGrid;




class CSWPolyGridvec
{

  private:

    int                  Ncol{0},
                         Nrow{0};
    double               xspac{0.0},
                         yspac{0.0},
                         xgmin{0.0},
                         ygmin{0.0};
    POlyGrid             gstruct1,
                         gstruct2,
                         *clipg{NULL},
                         *srcg{NULL},
                         *ActiveStruct{NULL};
    char                 *ActiveGrid{NULL};

    int                  first{1};


  public:

    CSWPolyGridvec () {};
    ~CSWPolyGridvec () {
        clipg = NULL;
        srcg = NULL;
        free (gstruct1.grid);
        gstruct1.grid = NULL;
        gstruct2.grid = NULL;
    };

    int ply_initvecgrid (void);
    int ply_setupvecgrids (double *xc, double *yc, int nc, int *ic,
                           double *xs, double *ys, int ns, int *is);
    int ply_setgridforvec (double x1, double y1, double x2, double y2);
    int ply_SetActiveGrid (int val);
    int ply_SetSrcClipGrids (int flag);
    int ply_checkgridforvec (double x1, double y1, double x2, double y2);

};  // end of main class definition

#endif
/*
    end of header file
    add nothing below this endif
*/
