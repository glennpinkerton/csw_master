
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_symb.h

    Define the GPFSymbol class.  This refactors the old gpf_symbol.c 
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
#error Illegal attempt to include private header file gpf_symbP.h.
#endif


#ifndef GPF_SYMB_H
#define GPF_SYMB_H

#  include "csw/utils/include/csw_.h"

/*  define macros used in symbol support  */

#define MAXSYMTEXT 33
#define MAXSYMITEM 500
#define MAXVECTORXY 2000

/*  define typedefs for symbol structures  */

typedef struct {
    long int     rflag,
                 firstcircle, ncircle,
                 firstline, nline,
                 firsttext, ntext;
}  SYmbolDef;                             /*  main symbol definition structure  */

typedef struct {
    long int      firstvec, nvec, fill;
}  SYmVecList;                            /*  symbol line definition structure  */

typedef struct {
    CSW_F        x, y, r1, r2;
    int          ifill;
    CSW_F        ang1, ang2;
}  SYmCircle;                             /*  symbol circle definition structure  */

typedef struct {
    CSW_F        x, y, size, angle;
    int          nc, font;
    char         text[MAXSYMTEXT];
}  SYmText;                               /*  text definition structure  */



class GPFSymbol
{

  public:

    GPFSymbol () {};
    ~GPFSymbol () {};


  private:

/*  define static variables that are used to build symbols  */

    long int                  TotalNumSymbols=140;
    int                       CurrentSymbol = -1,
                              Rflag2=1;
    CSW_F                     SymAng=0.0f,
                              SinAng=0.0f,
                              CosAng=0.0f,
                              SymXpos=0.0f,
                              SymYpos=0.0f,
                              SymSize=0.0f;

    CSW_F            p_xy[20000];

    int get_arc_points_double
                          (double r1, double r2, double ang1, double anglen,
                           double *xout, double *yout, int *nout, int maxpts);


  public:

    int gpf_getsymbolcircle (int nsym, const SYmCircle **circlelist, int *ncircle);
    int gpf_getsymboltext (int nsym, const SYmText **textlist, int *ntext);
    int gpf_getsymbolvec (int nsym, const SYmVecList **veclist, int *nvec);
    int gpf_getsymbolxy (const SYmVecList *veclist, CSW_F *xy, int *npts);
    int gpf_setsymbolparms (int snum, CSW_F x, CSW_F y, CSW_F size, CSW_F angle);
    int gpf_symlinetrans (CSW_F *xy, int npts);
    int gpf_symcircletrans (CSW_F *x, CSW_F *y, CSW_F *r1, CSW_F *r2, CSW_F *ang1);
    int gpf_symtexttrans (CSW_F *x, CSW_F *y, CSW_F *size, CSW_F *angle);

    int gpf_symbol_lines_and_fills (int snum,
                                    double size,
                                    double *xline,
                                    double *yline,
                                    int *icline,
                                    int *nline,
                                    double *xfill,
                                    double *yfill,
                                    int *ncfill,
                                    int *nfill,
                                    int maxpts,
                                    int maxcomp);


}; // end of main class


/*  do not add anything below this endif  */

#endif

