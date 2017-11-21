
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gpf_graphPP.h

      This function defines constants and structures needed
    by the EasyX server to process graph data.
*/

/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gpf_graphPP.h.
#endif



/*
  add nothing above this ifdef
*/
#ifndef GPF_GRAPHPP_H
#define GPF_GRAPHPP_H

#  include "csw/utils/include/csw_.h"

/*
    include a public header file.
*/
#include "csw/utils/include/gpf_graph.h"

/*
    include needed private header files.
*/
#include "csw/utils/private_include/gpf_graphP.h"

/*
    define constants for graph structures
*/
#define MAX_TITLE_LEN            200
#define MAX_NAME_LEN             32
#define MAX_CURVES               50
#define MAX_BARS                 50
#define MAX_FILLS                50
#define MAX_LABELS               100
#define MAX_POINTS               50
#define MAX_AXES                 22
#define NAXIS                    9
#define MAX_TITLE_LINES          20
#define MAX_SLICES               200
#define MAX_PRIMS_PER_SLICE      50
#define MAX_GPRIMS               1000
#define MAX_MAJOR_LINES          5000

#define GTX_CURVE_EDIT           1
#define GTX_BAR_EDIT             2
#define GTX_POINT_EDIT           3

#define GTX_CURVE_APPEND         1
#define GTX_BAR_APPEND           2
#define GTX_POINT_APPEND         3

/*
    define structures for graph parts
*/
typedef struct {
    char          *text;
    CSW_F         position;
}  AXisLabelStruct;

typedef struct {
    AXisLabelStruct   *list;
    int               nlist,
                      flag;
}  AXisLabelSet;

typedef struct {
    CSW_F         min,
                  max,
                  axmin,
                  axmax,
                  logbase,
                  majorval,
                  origmajorval,
                  minorval,
                  minorlen,
                  majorlen,
                  labelsize,
                  captionsize,
                  minorthick,
                  majorthick,
                  majorgridthick,
                  minorgridthick,
                  majorgriddashscale,
                  minorgriddashscale,
                  linethick,
                  labelthick,
                  captionthick,
                  labelgap,
                  captiongap;
    CSW_F         over,
                  under,
                  width;
    int           labelfont,
                  labelflag,
                  labelcolor,
                  labelfillcolor,
                  captionfont,
                  captioncolor,
                  captionfillcolor,
                  linecolor,
                  overflowflag,
                  position,
                  majorgridpattern,
                  majorgridcolor,
                  minorgridpattern,
                  minorgridcolor,
                  directionflag,
                  timeformat,
                  basetime,
                  monthflag,
                  ismirror,
                  nlabels;
    int           *primnums,
                  nprim,
                  frame,
                  item,
                  layer;
    char          notneeded,
                  legnotneeded,
                  needsdeleted,
                  autoscale,
                  captiontext[MAX_TITLE_LEN],
                  name[MAX_NAME_LEN + 8];
    void          *mirrorptr;
    AXisLabelSet  labels;
}  GRaphAxisRec;

typedef struct {
    CSW_F         *x,
                  *y,
                  *origx,
                  *origy,
                  *ebars,
                  x1, y1, x2, y2,
                  symbolsize,
                  ebarthick,
                  ebarwidth,
                  symbolthick,
                  thick;
    int           color,
                  symbolnum,
                  pattern,
                  symbolcolor,
                  symbolspace,
                  symbolmask,
                  ebarcolor,
                  smoothflag,
                  norig,
                  npts;
    int           *primnums,
                  nprim,
                  baselineprim,
                  frame,
                  item,
                  layer;
    char          notneeded,
                  legnotneeded,
                  needsdeleted,
                  name[MAX_NAME_LEN],
                  xaxis[MAX_NAME_LEN],
                  yaxis[MAX_NAME_LEN],
                  legendtext[MAX_TITLE_LEN];
    int           style,
                  vertical,
                  labelspace,
                  labelcolor,
                  labelfillcolor,
                  labelfont,
                  basepattern,
                  basecolor;
    CSW_F         labelsize,
                  labelthick,
                  curvedashscale,
                  curvebasedashscale,
                  basevalue,
                  basethick;
    int           legendcolor,
                  legendfillcolor,
                  legendfont;
    CSW_F         legendsize,
                  legendthick;
    int           legptr;
}  GRaphCurveRec;

typedef struct {
    CSW_F         *x,
                  *y,
                  x1, y1, x2, y2,
                  labelsize,
                  labelthick,
                  basevalue,
                  basethick,
                  outlinethick,
                  patternsize,
                  curvedashscale,
                  curvebasedashscale,
                  widthpct,
                  gappct;
    int           layer,
                  item,
                  frame,
                  stackflag,
                  done,
                  npts,
                  shadowcolor,
                  fillcolor,
                  pattcolor,
                  fillpattern,
                  outlinecolor,
                  outlinepattern,
                  labelspace,
                  labelfont,
                  labelcolor,
                  labelfillcolor,
                  basepattern,
                  basecolor;
    int           *primnums,
                  baselineprim,
                  nprim;
    char          notneeded,
                  legnotneeded,
                  needsdeleted,
                  name[MAX_NAME_LEN],
                  xaxis[MAX_NAME_LEN],
                  yaxis[MAX_NAME_LEN],
                  legendtext[MAX_TITLE_LEN];
    int           legendcolor,
                  legendfillcolor,
                  legendfont;
    CSW_F         legendsize,
                  legendthick;
    int           legptr;
}  GRaphBarRec;

typedef struct {
    CSW_F         *x,
                  *y,
                  x1, y1, x2, y2,
                  size,
                  thick;
    int           *tag,
                  color,
                  symbol,
                  mask,
                  npts;
    int           *primnums,
                  nprim,
                  frame,
                  item,
                  layer;
    char          notneeded,
                  legnotneeded,
                  needsdeleted,
                  name[MAX_NAME_LEN],
                  xaxis[MAX_NAME_LEN],
                  yaxis[MAX_NAME_LEN],
                  legendtext[MAX_TITLE_LEN];
    int           legendcolor,
                  legendfillcolor,
                  legendfont;
    CSW_F         legendsize,
                  legendthick;
    int           legptr;
}  GRaphPointsRec;

typedef struct {
    int           position,
                  anchor,
                  autoanchor,
                  bordercolor,
                  textfont,
                  textfillcolor,
                  textcolor;
    int           usecount,
                  nfree,
                  nleg;
    void          *leglist;
    int           *freelist;
    int           usedflag,
                  outputflag,
                  firstptr,
                  lastptr;
    CSW_F         textsize,
                  borderthick,
                  bwidth,
                  titleheight,
                  textthick;
    CSW_F         x1, y1, y2, autox1;
    int           *primnums,
                  nprim,
                  frame,
                  item,
                  layer;
    int           titleprims[MAX_TITLE_LINES],
                  ntprims;
    char          notneeded,
                  legnotneeded,
                  title[MAX_TITLE_LEN];
    char          name[MAX_NAME_LEN];
}  GRaphLegendRec;

typedef struct {
    int           color,
                  fillcolor,
                  font,
                  position;
    CSW_F         size,
                  thick,
                  gap;
    char          notneeded,
                  legnotneeded,
                  needsdeleted,
                  title[MAX_TITLE_LEN];
    int           *primnums,
                  nprim,
                  frame,
                  item,
                  layer;
}  GRaphTitleRec;

typedef struct {
    CSW_F         patsize;
    int           color,
                  pattern,
                  pattcolor,
                  topnum,
                  bottomnum,
                  ntop,
                  nbottom,
                  *primnums,
                  nprim,
                  frame,
                  item,
                  layer;
    char          notneeded,
                  legnotneeded,
                  needsdeleted,
                  name[MAX_NAME_LEN],
                  xaxis[MAX_NAME_LEN],
                  yaxis[MAX_NAME_LEN],
                  legendtext[MAX_TITLE_LEN];
    int           legendcolor,
                  legendfillcolor,
                  legendfont;
    CSW_F         legendsize,
                  legendthick;
    int           legptr;
}  GRaphCurveFillRec;

typedef struct {
    void                 *gptr;
    int                  type,
                         num,
                         tag;
}  GRaphPrimRec;

typedef struct {
    int                  type,
                         xaxis,
                         yaxis;
    void                 *prim;
}  GRaphScaledPrimRec;

typedef struct {
    CSW_F                value,
                         labelsize,
                         linethick,
                         labelthick,
                         patsize,
                         dashscale;
    int                  fillcolor,
                         fillpattern,
                         pattcolor,
                         linecolor,
                         linepatt,
                         labelcolor,
                         labelfillcolor,
                         labelfont,
                         primnums[MAX_PRIMS_PER_SLICE],
                         nprim,
                         frame,
                         item,
                         layer,
                         highlight;
    int                  needsdeleted,
                         notneeded;
    char                 name[MAX_NAME_LEN],
                         label[MAX_TITLE_LEN];
    void                 *parent;
}  PIeSliceRec;

typedef struct {
    GRaphCurveRec        *curves[MAX_CURVES];
    int                  ncurves;
    GRaphBarRec          *bars[MAX_BARS];
    int                  nbars;
    GRaphCurveFillRec    *fills[MAX_FILLS];
    int                  nfills;
    GRaphPointsRec       *points[MAX_POINTS];
    int                  npoints;
    PIeSliceRec          *slices[MAX_SLICES];
    int                  nslices;
    int                  pie_sort_array[MAX_SLICES];
    GRaphAxisRec         *axes[MAX_AXES];
    GRaphLegendRec       *legend;
    GRaphTitleRec        title;
    char                 name[MAX_NAME_LEN];
    int                  windownum;
    CSW_F                x1,
                         y1,
                         x2,
                         y2,
                         gx1,
                         gy1,
                         gx2,
                         gy2;
    int                  drawneeded;
    GRaphPrimRec         *gprimlist[MAX_GPRIMS];
    GRaphPrimRec         **usedgprimlist;
    int                  ngprim;
    int                  maxusedgprim,
                         nusedgprim;
    int                  majorgridprims[MAX_MAJOR_LINES];
    int                  nmajorgrid,
                         nerasemajorgrid;
    int                  axisdeleted;
    int                  pie_flag,
                         pie_direction,
                         pie_start_angle,
                         pie_sort_flag,
                         pie_inside_labels,
                         pie_edge_labels,
                         pie_legend_labels,
                         pie_border_flag,
                         pie_other_slice;
    CSW_F                pie_xmin,
                         pie_ymin,
                         pie_xmax,
                         pie_ymax;
    GRaphScaledPrimRec   *scaledprims;
    int                  nscaledprim,
                         maxscaledprim;
}  GRaphRec;

/*
  add nothing below this endif
*/
#endif
