
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    con_shared_structs.h

    This header has the structure definitions which are needed
    by both the CSWContourApi class and the CSWConCalc class.
*/

/*
    Add nothing above this ifndef
*/
#ifndef CON_SHARED_STRUCTS_H
#define CON_SHARED_STRUCTS_H


#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

#  include "csw/utils/include/csw_.h"
#  include "csw/utils/private_include/gpf_graphP.h"


#  include "csw/utils/include/csw_trig_consts.h"

/*
    Define contour calculation options.
*/
#  define CON_DEFAULT_OPTIONS           0
#  define CON_SMOOTHING                 1
#  define CON_BASE_VALUE                2
#  define CON_LOG_BASE                  3
#  define CON_CONVERT_TO_LOG            4
#  define CON_NULL_VALUE                5
#  define CON_MINOR_CROWD               6
#  define CON_MAJOR_CROWD               7
#  define CON_FAST_FLAG                 8
#  define CON_FILL_PRECISION            9
#  define CON_STEP_GRID_FLAG           10
#  define CON_RESAMPLE_FLAG            11
#  define CON_THICKNESS_FLAG           12
#  define CON_CONTOUR_IN_FAULTS        13

#  define CON_FAULTED_GRID_FLAG      1001

#  define MAX_CONTOUR_LABEL_SIZE       32
#  define MAX_CONTOURS               2000
#  define MAX_FILL_POLY                 6

#  define CON_NONE                      0
#  define CON_POSITIVE_THICKNESS        1
#  define CON_NEGATIVE_THICKNESS        2

/*
    define some constants for the contour drawing
*/
#  define CON_MAJOR_TICK_SPACING        1
#  define CON_MAJOR_TEXT_SPACING        2
#  define CON_MAJOR_TICK_LEN            3
#  define CON_MAJOR_TEXT_SIZE           4
#  define CON_MINOR_TICK_SPACING        5
#  define CON_MINOR_TEXT_SPACING        6
#  define CON_MINOR_TICK_LEN            7
#  define CON_MINOR_TEXT_SIZE           8
#  define CON_TICK_DIRECTION            9
#  define CON_MAJOR_TEXT_FONT          10
#  define CON_MINOR_TEXT_FONT          11

#  define CON_SIMPLE_BLOCK_FONT         1
#  define CON_SIMPLE_CURVE_FONT         2
#  define CON_SIMPLE_ITALIC_FONT        3
#  define CON_COMPLEX_FONT              4
#  define CON_COMPLEX_ITALIC_FONT       5

#define CON_NAME_LENGTH                64



/*
    Define the structure for returning contour lines.
*/
typedef struct {
    CSW_Float     zvalue;
    char          text[MAX_CONTOUR_LABEL_SIZE+1];
    int           npts;
    CSW_Float     *x, *y;
    char          major;
    char          downhill;
    char          closure;
    char          local_min_max;
    char          expect_double;
    char          faultflag;
}  COntourOutputRec;

/*
    Structure for returning contour fills.
*/
typedef struct {
    CSW_Float     x[MAX_FILL_POLY],
                  y[MAX_FILL_POLY];
    int           npts;
    CSW_Float     zlow,
                  zhigh;
    int           color;
    char          expect_double;
}  COntourFillRec;

/*
    Structure for contour calculation options.
*/
typedef struct {

    /*  contour calculation options  */
    int           convert_to_log,
                  smoothing,
                  resample_flag,
                  thickness_flag;
    CSW_Float     base_value,
                  log_base,
                  null_value,
                  minor_crowd,
                  major_crowd;
    int           fast_flag,
                  faulted_flag,
                  contour_in_faults_flag,
                  step_flag;
    CSW_Float     fill_precision;

    /*  contour interval stuff  */
    CSW_Float     contour_interval;
    int           major_spacing;
    CSW_Float     first_contour,
                  last_contour,
                  base_grid_value,
                  top_grid_value,
                  minor_contours[MAX_CONTOURS],
                  major_contours[MAX_CONTOURS];
    int           nminor,
                  nmajor;
    int           error_number;
}  COntourCalcOptions;



/*
    define structures that the line, text and fill
    primitives are returned in.
*/
typedef struct {
    int           npts;
    CSW_Float     *x, *y;
    int           majorflag,
                  tickflag,
                  textflag,
                  faultflag;
}  COntourLinePrim;

typedef struct {
    int           nchar;
    char          *text;
    int           font;
    CSW_Float     size;
    CSW_Float     *x, *y, *angles;
    int           majorflag;
}  COntourTextPrim;

typedef struct {
    int           npts;
    CSW_Float     x[6], y[6];
}  COntourFillPrim;

/*
    Structure for contour drawing options.
*/
typedef struct {
    CSW_Float     major_tick_spacing,
                  major_text_spacing,
                  major_tick_len,
                  major_text_size,
                  minor_tick_spacing,
                  minor_text_spacing,
                  minor_tick_len,
                  minor_text_size;
    int           tick_direction,
                  minor_text_font,
                  major_text_font;
    int           error_number;
    char          text_prepend[200],
                  text_append[200];
}  COntourDrawOptions;


/*
    Define some constants for the cross section interface.
*/
#define CON_XSECT_AXIS_LAYER        1
#define CON_XSECT_SURFACE_LAYER     2
#define CON_XSECT_FILL_LAYER        3

/*
    Define structures for cross sections.
*/
    typedef struct {
        double             *x,
                           *y;
        int                npts;
        char               name[CON_NAME_LENGTH];
        int                type,
                           id;
    }  CRossSectionTraceStruct;

    typedef struct {
        double             *x,
                           *y,
                           *dist,
                           *z;
        int                npts;
        int                traceid;
        char               tracename[CON_NAME_LENGTH];
        int                gridid,
                           gridtype;
        char               gridname[CON_NAME_LENGTH];
    }  CRossSectionLineStruct;

    typedef struct {
        double             *dist,
                           *z;
        int                npts;
        int                traceid;
        char               tracename[CON_NAME_LENGTH];
        int                topgridid,
                           topgridtype,
                           bottomgridid,
                           bottomgridtype;
        char               topgridname[CON_NAME_LENGTH],
                           bottomgridname[CON_NAME_LENGTH];
    }  CRossSectionFillStruct;

    typedef struct {
        CSW_F              *x,
                           *y;
        CSW_F              thick;
        int                npts;
        int                layer,
                           id,
                           textflag;
    }  CRossSectionLinePrimitive;

    typedef struct {
        CSW_F              *x,
                           *y;
        int                npts;
        int                layer,
                           topid,
                           bottomid;
    }  CRossSectionFillPrimitive;


/*
    End of the header file, add nothing below this line.
*/
#endif
