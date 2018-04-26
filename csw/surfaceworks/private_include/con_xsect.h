
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    con_xsect.h

    This header has the class definition for the new CSWConXsect
    class.  This class replaces the functionality in the old 
    con_xsect.c file.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file con_xsect.h.
#endif


/*
    Add nothing above this ifndef
*/
#ifndef CON_XSECT_H
#define CON_XSECT_H

/*
    Define constants used only in this class.
*/
#define MINUS_NULL               -1.e19f
#define PLUS_NULL                 1.e19f

#define _FILL_CHUNK              100
#define _LINE_CHUNK              100

#define UNSHIFT                  UnshiftGrids(); UnshiftXsects();

class CSWConXsect;

#  include "csw/surfaceworks/include/con_shared_structs.h"
#  include "csw/surfaceworks/private_include/grd_arith.h"
#  include "csw/surfaceworks/private_include/grd_utils.h"

class CSWConXsect
{

  private:

    CSWGrdArith          *grd_arith_ptr = NULL;
    CSWGrdUtils          *grd_utils_ptr = NULL;

  public:

    CSWConXsect () {};

    void SetGrdArithPtr (CSWGrdArith *p) {grd_arith_ptr = p;};
    void SetGrdUtilsPtr (CSWGrdUtils *p) {grd_utils_ptr = p;};

  private:

/*
    Static file variables from the old C code are private class variables
    in the C++ class.
*/
    double               Xmin {0.0},
                         Ymin {0.0},
                         XminSav {0.0},
                         YminSav {0.0},
                         Xmax {0.0},
                         Ymax {0.0},
                         Xspace {0.0},
                         Yspace {0.0},
                         Dspace {0.0};
    CSW_F                Zmin {0.0}, 
                         Zmax {0.0};
    int                  Ncol {0},
                         Nrow {0};
    CSW_F                *Xwork {NULL},
                         *Ywork {NULL},
                         *Zwork {NULL},
                         *Dwork {NULL};
    int                  MaxWork {0};

    CRossSectionTraceStruct    *InputTraceList {NULL};
    GRidStruct                 *InputGridList {NULL};
    int                        NumTrace {0},
                               NumGrid {0};

    CSW_F                      PageUnitsPerInch {1.0};


/*
  Static functions in the old C code are private class methods.
*/
    int FreeGridList (GRidStruct *sorted_list,
                      GRidStruct *list, int ngrid);
    int FreeOutputLines (CRossSectionLineStruct *list, int nlist);
    int FreeOutputFills (CRossSectionFillStruct *list, int nlist);
    int ShiftGrids (void);
    int UnshiftGrids (void);
    int ShiftXsects (void);
    int UnshiftXsects (void);
    int MovePoints (double *x, double *y, double *z, double *d, int n);
    int CalcAxisIntervals (int nhint, CSW_F dmin, CSW_F dmax,
                           CSW_F *axmin, CSW_F *axmax,
                           CSW_F *axmajor, CSW_F *axminor);
    int AppendLinesToOutput (GRaphLineRec *grlines, int ngrlines,
                             CRossSectionLinePrimitive **linesin,
                             int *nlinesin, int *maxlinesin);
    int AppendTextToOutput (GRaphTextRec *grtext, int ngrtext, 
                            CRossSectionLinePrimitive **linesin,
                            int *nlinesin, int *maxlinesin);
    int FreeLinePrims (CRossSectionLinePrimitive *list, int nlist);
    int FreeFillPrims (CRossSectionFillPrimitive *list, int nlist);

  public:

    int con_calc_cross_section
             (GRidStruct *input_gridlist, int ngrid,
              CRossSectionTraceStruct *section_list, int nsection,
              CRossSectionLineStruct **lines, int *nlines,
              CRossSectionFillStruct **fills, int *nfills,
              CSW_F page_units_per_inch);
    int con_calc_cross_section_polygons
                 (CRossSectionLineStruct *line1,
                  CRossSectionLineStruct *line2,
                  CRossSectionFillStruct **fills,
                  int *nfills,
                  CSW_F page_units_per_inch);
    int con_draw_cross_section
           (CRossSectionLineStruct *lines, int nlines,
            CRossSectionFillStruct *fills, int nfills,
            int zflag, int traceid,
            char *xcaption, char *ycaption,
            CSW_F hscale, CSW_F zscale,
            CRossSectionLinePrimitive **lprimin, int *nlprimin,
            CRossSectionFillPrimitive **fprimin, int *nfprimin);
    int con_free_cross_section
            (CRossSectionLineStruct *lines, int nlines,
             CRossSectionFillStruct *fills, int nfills);
    int con_free_cross_section_drawing
              (CRossSectionLinePrimitive *lines, int nlines,
               CRossSectionFillPrimitive *fills, int nfills);

}; // end of main class

/*
    End of the header file, add nothing below this line.
*/
#endif
