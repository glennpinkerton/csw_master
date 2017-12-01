
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    con_draw.h

    CDefine the CSWConDraw class, which refactors the old con_draw.c
    file.

*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file con_draw.h.
#endif


/*
    Add nothing above this ifndef
*/
#ifndef CON_DRAW_H
#define CON_DRAW_H

#  include "csw/surfaceworks/include/con_shared_structs.h"


/*
    define constants for the file
*/
#define MAX_TEXT_LABELS            1000
#define LINE_CHUNK                 50
#define TEXT_CHUNK                 50

#define MAX_TEXT_VEC             500
#define MAX_TEXT_PARTS            50
#define DEGTORAD           .01745329f

class CSWConDraw;

#include "csw/surfaceworks/private_include/grd_utils.h"

class CSWConDraw
{

  private:

    CSWGrdUtils    *grd_utils_ptr = NULL;

  public:

    CSWConDraw () {};
    ~CSWConDraw () {FreeWork ();};

    void  SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

  private:

    void FreeWork (void);

/*
    Old static file variables become private class variables
*/
    CSW_F         *Xline {NULL},
                  *Yline {NULL},
                  *Xwork {NULL},
                  *Ywork {NULL},
                  *LineDistance {NULL};
    int           Nline {0},
                  MaxLine {0};

    CSW_F         OptMajorTickSpacing {0.0f},
                  OptMajorTextSpacing {10.0f},
                  OptMajorTickLen {0.07f},
                  OptMajorTextSize {0.1f};
    CSW_F         OptMinorTickSpacing {0.0f},
                  OptMinorTextSpacing {0.0f},
                  OptMinorTickLen {0.04f},
                  OptMinorTextSize {0.07f};
    int           OptTickDirection {1},
                  OptMinorTextFont {2},
                  OptMajorTextFont {2};

    int           MajorFlag {0};
    int           FaultFlag {0};
    CSW_F         MajorTickSpacing {0.0f},
                  MajorTextSpacing {10.0f},
                  MajorTickLen {0.07f},
                  MajorTextSize {0.1f};
    CSW_F         MinorTickSpacing {0.0f},
                  MinorTextSpacing {0.0f},
                  MinorTickLen {0.04f},
                  MinorTextSize {0.07f};
    int           TickDirection {1},
                  MinorTextFont {2},
                  MajorTextFont {2};
    int           ClosedContour {0};
    int           TickLeftRight {0};

    CSW_F         TickSpacing {0.0f},
                  TextSpacing {0.0f},
                  TickLen {0.0f},
                  TextSize {0.0f};

    int           TickFlag {0},
                  TextFlag {0},
                  TextFont {2};

    CSW_F         Xmin {0.0f},
                  Ymin {0.0f},
                  Pxmin {0.0f},
                  Pymin {0.0f},
                  Pxmax {0.0f},
                  Pymax {0.0f},
                  Scalex {1.0f},
                  Scaley {1.0f};

    CSW_F         CharWidths[MAX_CONTOUR_LABEL_SIZE];
    int           NcharWidths {0};
    char          TextLabel[MAX_CONTOUR_LABEL_SIZE];

    CSW_F         Tiny {0.0f};

    COntourLinePrim   *OutputLineRecs {NULL};
    int           NoutputLine {0};
    int           MaxOutputLine {0};
    COntourTextPrim   *OutputTextRecs {NULL};
    int           NoutputText {0};
    int           MaxOutputText {0};

    CSW_F         TextGap1[MAX_TEXT_LABELS],
                  TextGap2[MAX_TEXT_LABELS];
    int           NtextSeg {1};

    int           FontLookup[5] {102, 102, 102, 102, 102};


/*
    Old static file functions become private class functions
*/

    int               SetPointDistances (void);
    int               PointAtDistance (int ibase, CSW_F dist,
                                       CSW_F *x, CSW_F *y,
                                       CSW_F *angle, CSW_F *slope,
                                       int *nextibase);
    int               PerpendicularPoint (CSW_F x1, CSW_F y1,
                                          CSW_F x2, CSW_F y2, 
                                          CSW_F xanc, CSW_F yanc,
                                          int direction,
                                          CSW_F dist, CSW_F *xp, CSW_F *yp);
    int               PerpendicularIntersect (CSW_F x1, CSW_F y1,
                                              CSW_F x2, CSW_F y2,
                                              CSW_F x, CSW_F y,
                                              CSW_F *xp, CSW_F *yp);
    int               CheckCurvature (CSW_F dist1, CSW_F dist2,
                                      CSW_F *curvemax);
    int               GenerateTicks (CSW_F dist1, CSW_F dist2);
    int               SeparateLabelSegments (void);
    int               PositionLabels (void);

    int               OutputWholeLine (void);
    int               AppendVecToOutput (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int               AppendLineToOutput (CSW_F *x, CSW_F *y, int npts);
    int               AppendTextToOutput (char *text,
                                          CSW_F *xt, CSW_F *yt, CSW_F *at,
                                          int nt);

    int               FreeOutputMem (void);

    int               CalcCharWidths (char *text);

  

  public:

    int con_set_draw_scale
            (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
             CSW_F px1, CSW_F py1, CSW_F px2, CSW_F py2,
             int dflag);
    int con_set_draw_option (int tag, int ival, CSW_F fval);
    int con_set_draw_options (COntourDrawOptions *options);
    int con_default_draw_options (COntourDrawOptions *options);
    int con_draw_line
            (COntourOutputRec *cptr, CSW_F *char_widths,
             COntourLinePrim **lptr, int *nline,
             COntourTextPrim **tptr, int *ntext,
             COntourDrawOptions *options);
    int con_draw_fill
            (COntourFillRec *cptr,
             COntourFillPrim **fptr, int *nfill);
    int con_free_drawing
            (COntourLinePrim *lines, int nlines,
             COntourTextPrim *text, int ntext,
             COntourFillPrim *fills, int nfills);
    int con_draw_text
            (COntourTextPrim *textptr,
             COntourLinePrim **lineptr, int *nlines);

}; // end of main class


/*
    End of the header file, add nothing below this line.
*/
#endif
