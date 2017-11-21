
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    pst_P.h

    This is a private header file that has function prototypes for the 
    functions found in pst_fileio.c.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file pst_P.h.
#endif



#ifndef PST_P_H
#define PST_P_H


/*
  If a C++ compiler is running, all of the prototypes
  must be declared extern "C".
*/
# if defined(__cplusplus)  ||  defined(c_plusplus)
extern "C"
{
# endif

/*
    functions from pst_fileio.c
*/
    int pst_OpenFile (const char *name);
    int pst_Startup (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                     const char *title);
    int pst_SetColor (int r, int g, int b);
    int pst_SetLineWidth (CSW_F val);
    int pst_SetDashes (CSW_F *dash, int ndash, CSW_F scale);
    int pst_SetLineCap (int val);
    int pst_PlotLine (CSW_F *x, CSW_F *y, int n);
    int pst_PlotFill (CSW_F *x, CSW_F *y, int n);
    int pst_EndPlot (int flag);
    int pst_SetPipeFD (int val);
    int pst_PlotFill2 (CSW_F *xin, CSW_F *yin, int *icomp, int ncomp);
    int pst_ImageHeader (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                         int nc, int nr, int colorflag);
    int pst_ImageTrailer (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int pst_ImageRow (unsigned char *data, int ndata, COlorTableRec *colortable);
    int pst_ImageRow24 (unsigned char *data, int ndata);

/*
  The extern "C" block needs to be closed if this
  is being run through a C++ compiler.
*/
# if defined (__cplusplus)  ||  defined (c_plusplus)
}
# endif

#endif
/*
    end of header file
    add nothing below this endif
*/
