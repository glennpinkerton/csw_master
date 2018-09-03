
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_line.h

    Define the GPFLine class, which refactors functions in the old
    gpf_line.c file.

    This header file has structure definitions needed to access
    data from the fancy line calculation functions.  The application
    needs to use these structures to retrieve the points and/or text
    primitives calculated by gpf_LineCalc.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_line.h.
#endif


/*
    add nothing above this ifdef
*/

#ifndef GPF_LINE_H
#define GPF_LINE_H

#  include "csw/utils/include/csw_.h"


/*
    define constants for the class 
*/
#define MAXDASH               8
#define MAXLABEL              100

/*
    define structures
*/
typedef struct {
    int           npts;
    CSW_F         *coords;
}  LInePointRec;

typedef struct {
    int           nchar;
    char          *text;
    CSW_F         x,
                  y,
                  angle;
}  LIneTextRec;



class GPFLine
{

  public:

    GPFLine () {};
    ~GPFLine () {Cleanup ();};

    GPFLine (const GPFLine &other) = delete;
    const GPFLine &operator= (const GPFLine &other) = delete;
    GPFLine (const GPFLine &&other) = delete;
    const GPFLine &operator= (const GPFLine &&other) = delete;


  private:

    char                   LineLabel[MAXLABEL];
    int                    LabelFlag = 0;
    CSW_F                  LabelDist = 0.0f;

    int                    DashFlag = 0,
                           Ndash = 0;
    CSW_F                  DashDist[MAXDASH],
                           DashLen = 0.0f;

    int                    TickFlag = 0;
    CSW_F                  TickDist = 0.0f;

    LInePointRec           *PointRecArray = NULL;
    CSW_F                  *PointArray = NULL;
    CSW_F                  *CurrentPoint = NULL;
    int                    NpointRec = 0;

    CSW_F                  *LineDistance = NULL;

    LIneTextRec            *TextRecArray = NULL;
    char                   *TextArray = NULL;
    int                    NtextRec = 0;

    void                   Cleanup (void);

    int                    CalcLineDistances (CSW_F *x, CSW_F *y, int npts);
    int                    CalcLineDashes (CSW_F *x, CSW_F *y, int npts);
    void                   AddDashPoint (CSW_F x, CSW_F y, int flag);

    CSW_F                  p_xanc = 0.0;
    CSW_F                  p_yanc = 0.0;
    int                    p_lastanc = 0;


  public:

    int gpf_ResetLineStyle (void);
    int gpf_SetLineDash (CSW_F *dashes, int ndash);
    int gpf_SetLineLabel (const char *label, int type, int font,
                          CSW_F size, CSW_F dist);
    int gpf_SetLineTicks (int angle, CSW_F dist, CSW_F length);
    int gpf_LineCalc2 (CSW_F *xy, int npts);
    int gpf_LineCalc (CSW_F *xin, CSW_F *yin, int npts);
    int gpf_GetLinePoints (LInePointRec **ptrarray, int *nline);
    int gpf_GetLineText (LIneTextRec **ptrarray, int *ntext);
    int gpf_CleanupLineCalc (void);

};  // end of main class


/*
    add nothing below this endif
*/
#endif
