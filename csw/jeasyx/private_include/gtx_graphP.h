
/*
         ************************************************
         *                                              *
         *    Copyright (1997-1998) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_graphP.h

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
#error Illegal attempt to include private header file gtx_graphP.h.
#endif



/*
  add nothing above this ifdef
*/
#ifndef GTX_GRAPHP_H
#define GTX_GRAPHP_H

#  include "csw/utils/include/csw_.h"

/*
    include the public header file
*/
#include "csw/jeasyx/include/gtx_graph.h"

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
    char          *text = NULL;
    CSW_F         position;
}  AXisLabelStruct;

typedef struct {
    AXisLabelStruct   *list = NULL;
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
    int           *primnums = NULL,
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
    void          *mirrorptr = NULL;
    AXisLabelSet  labels;
}  GRaphAxisRec;

typedef struct {
    CSW_F         *x = NULL,
                  *y = NULL,
                  *origx = NULL,
                  *origy = NULL,
                  *ebars = NULL,
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
    int           *primnums = NULL,
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
    CSW_F         *x = NULL,
                  *y = NULL,
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
    int           *primnums = NULL,
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
    CSW_F         *x = NULL,
                  *y = NULL,
                  x1, y1, x2, y2,
                  size,
                  thick;
    int           *tag = NULL,
                  color,
                  symbol,
                  mask,
                  npts;
    int           *primnums = NULL,
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
    void          *leglist = NULL;
    int           *freelist = NULL;
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
    int           *primnums = NULL,
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
    int           *primnums = NULL,
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
                  *primnums = NULL,
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
    void                 *gptr = NULL;
    int                  type,
                         num,
                         tag;
}  GRaphPrimRec;

typedef struct {
    int                  type,
                         xaxis,
                         yaxis;
    void                 *prim = NULL;
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
    void                 *parent = NULL;
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
    GRaphPrimRec         **usedgprimlist = NULL;
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
    GRaphScaledPrimRec   *scaledprims = NULL;
    int                  nscaledprim,
                         maxscaledprim;
}  GRaphRec;


typedef struct {
    GRaphRec             *graph_rec;
    char                 name[100];
}  GRaphStruct;


/*
    function prototypes for stand alone C++ functions
*/
int gtx_set_graph_param(
    int tag,
    GRaphUnion value);

int gtx_set_graph_layer(
    int val);

int gtx_set_graph_item(
    int val);

int gtx_set_graph_frame(
    int val);

void gtx_graph_set_page_units_per_inch (CSW_F fval);

int gtx_create_new_graph(
    CSW_F x1,
    CSW_F y1,
    CSW_F x2,
    CSW_F y2,
    char * title,
    char * name,
    int wnum);

int gtx_end_graph(
    void);

int gtx_add_graph_axis(
    int positionin,
    char * caption,
    CSW_F datamin,
    CSW_F datamax,
    CSW_F logbase,
    int labelflagin,
    char * name);

int gtx_add_graph_curve(
    CSW_F * x,
    CSW_F * y,
    CSW_F * ebar,
    int npts,
    char * xaxis,
    char * yaxis,
    char * name,
    char * legtext);

int gtx_set_graph_defaults(
    void);

int gtx_get_graph_number(
    void);

int gtx_draw_graph_from_app(
    void);

int gtx_draw_graph(
    void);

int gtx_free_graph_mem(
    int num);

int gtx_graph_fill_curve(
    char * top,
    char * bottom,
    char * name,
    char * legtext);

int gtx_add_graph_points(
    CSW_F * x,
    CSW_F * y,
    int * tag,
    int npts,
    int symbol,
    int color,
    CSW_F size,
    char * xaxis,
    char * yaxis,
    char * name,
    char * legtext);

int gtx_convert_graph_coords(
    int flag,
    CSW_F * xin,
    CSW_F * yin,
    CSW_F * xout,
    CSW_F * yout,
    int npts,
    int wnum,
    char * grname,
    char * xaxis,
    char * yaxis);

int gtx_set_axis_labels(
    char * axisname,
    char ** labels,
    CSW_F * positions,
    int nlabels,
    int positionflag);

int gtx_add_graph_bar_set(
    int flag,
    CSW_F * x,
    CSW_F * y,
    char ** labels,
    int npts,
    char * xaxis,
    char * yaxis,
    char * name,
    char * legtext);

int gtx_write_graphs_to_file(
    void);

int gtx_read_graphs_from_file(
    int pos,
    int wnum);

int gtx_set_graph_file_prims_flag(
    int val);

int gtx_update_graph_ptr(
    int primnum,
    GRaphRec * gptr,
    int type,
    int tag,
    int num);

int gtx_add_used_prim_graph_ptr(
    GRaphPrimRec * ptr);

int gtx_setup_graph_pick_select_set(
    int primnum);

int gtx_fill_pick_for_graph(
    int primnum,
    void * pickrec);

int gtx_adjust_graph_for_move(
    int primnum,
    void * pickrec);

int gtx_create_global_legend(
    CSW_F xanchor,
    int anchorflag,
    CSW_F y1,
    CSW_F y2,
    char * title,
    char * name);

int gtx_end_global_legend(
    void);

int gtx_open_graph(
    char * name,
    int wnum);

int gtx_open_global_legend(
    char * name);

int gtx_delete_graph_curve(
    char * name,
    int flag);

int gtx_delete_graph_barset(
    char * name,
    int flag);

int gtx_delete_graph_points(
    char * name,
    int flag);

int gtx_delete_graph_fill(
    char * name);

int gtx_delete_graph_axis(
    char * name);

int gtx_change_graph_title(
    char * title,
    int flag);

int gtx_adjust_prim_graph_ptr(
    int primnum,
    int num);

int gtx_reset_graph_element_param(
    int type,
    char * name,
    int tag,
    int ival,
    CSW_F fval,
    char * cval);

int gtx_reset_slice_param(
    char * name,
    int tag,
    int ival,
    CSW_F fval,
    char * cval);

int gtx_reset_curve_param(
    char * name,
    int tag,
    int ival,
    CSW_F fval,
    char * cval);

int gtx_reset_point_param(
    char * name,
    int tag,
    int ival,
    CSW_F fval,
    char * cval);

int gtx_reset_bar_param(
    char * name,
    int tag,
    int ival,
    CSW_F fval,
    char * cval);

int gtx_reset_fill_param(
    char * name,
    int tag,
    int ival,
    CSW_F fval,
    char * cval);

int gtx_set_graph_client_num(
    int val);

int gtx_remove_graph_client_entry(
    int cnum);

int gtx_find_graph_tracking(
    int wnum,
    CSW_F x,
    CSW_F y,
    int max,
    char * gname,
    char ** axnames,
    CSW_F * axvals,
    int * naxis);

int gtx_check_graph_axes (
    char*, char*, int*, int*);

int gtx_translate_graph_point_array (
    CSW_F*, int, int, int, CSW_F**);

int gtx_translate_graph_point (
    CSW_F, CSW_F, int, int, CSW_F*, CSW_F*);

int gtx_get_graph_corner_points (
    int, CSW_F*, CSW_F*, CSW_F*, CSW_F*);

int gtx_add_scaled_graph_prim (
    void*, int, int, int);

int gtx_set_graph_prim_redraw_wnum (int);

int gtx_redraw_graph_scaled_prims (
    int, int, CSW_F, CSW_F, CSW_F, CSW_F);

int gtx_set_graph_being_drawn (int);

int gtx_create_pie_chart (
    char*, char*, CSW_F, CSW_F, CSW_F, CSW_F, int, int);

int gtx_create_pie_slice (char*, char*, CSW_F, int, int, int, int);

int gtx_delete_pie_slice (char*, int);

int gtx_open_pie_chart (char*, int);

int gtx_end_pie_chart (void);

int gtx_draw_pie_chart_from_app (void);

/*
  add nothing below this endif
*/
#endif
