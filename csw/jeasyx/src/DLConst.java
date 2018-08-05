
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/**
 * Constants for the csw.jeasyx.src package.
 * This is a repository for constants used internally by the jeasyx package and for
 * public constants used by applications interfacing with the jeasyx package.
 *
 *  @author Glenn Pinkerton
 *
*/

package csw.jeasyx.src;


/**
 This class defines constants needed by public and non public methods of the
 classes in the csw.jeasyx.src package.  Only the public constants
 are available to application objects.  This class cannot be instantiated by
 an application object.  There is no need.  The application object sould just
 directly use the static constants.
*/
public class DLConst {

  /*
   * Package scope constructor insures that this
   * cannot be instantiated outside the jeasyx package.
   */
    DLConst () {}


  /*
   * Define constants for public use.
   */

  // arc closure constants
  /**
  The partial arc will not have its end points connected.
  */
    public final static int OPEN                  = 0;
  /**
  The partial arc will have its end points connected by drawing from one end point
  to the center of the arc and then from the center of the arc to the other end point.
  */
    public final static int PIE                   = 1;
  /**
  The end points will be connected by drawing directly from one to the other.
  */
    public final static int CHORD                 = 2;

  // nitems constants
  /**
   * Number of default symbols available
   */
  public final static int N_SYMBOLS = 140;

  /**
   * Number of default line styles available
   */
  public final static int N_LINESTYLES = 31;

  /**
   * Number of default fonts available
   */
  public final static int N_FONTS = 26;

  // frame aspect ratio constants
  /**
  The frame will always have unit aspect ratio, regardless of resizing, moving, etc.
  */
    public final static int FORCE_UNIT_FRAME_ASPECT_RATIO = 0;
  /**
  The frame can have any aspect ratio, which will change with zooming, resizing, etc.
  */
    public final static int ALLOW_ANY_FRAME_ASPECT_RATIO  = 9217;  //strange number discourages accidental use


  // frame border constants
  /**
  Do not draw any border around the frame.
  */
    public final static int FRAME_NO_BORDER                  = 0;
  /**
  Draw a simple rectangular border around the frame.
  */
    public final static int FRAME_PLAIN_BORDER               = 1;
  /**
  Draw labels, caption and tick marks on the left and bottom sides of the border.
  */
    public final static int FRAME_LABEL_LEFT_BOTTOM_BORDER   = 2;
  /**
  Draw labels, caption and tick marks on the left and top sides of the border.
  */
    public final static int FRAME_LABEL_LEFT_TOP_BORDER      = 3;
  /**
  Draw labels, caption and tick marks on the right and bottom sides of the border.
  */
    public final static int FRAME_LABEL_RIGHT_BOTTOM_BORDER  = 4;
  /**
  Draw labels, caption and tick marks on the right and top sides of the border.
  */
    public final static int FRAME_LABEL_RIGHT_TOP_BORDER     = 5;
  /**
  Draw labels, caption and tick marks on all sides of the border.
  */
    public final static int FRAME_LABEL_ALL_SIDES_BORDER     = 6;


  // attached frame units constants
  /**
  The attached frame value is in page units.  This is the only working option right now.
  */
    public final static int PAGE_UNITS                 = 1;
  /**
  The attached frame value is in inches as measured on the device.  This is not working yet.
  */
    public final static int DEVICE_INCHES              = 2;
  /**
  The attached frame value is in centimeters as measured on the device.  This is not working yet.
  */
    public final static int DEVICE_CM                  = 3;
  /**
  The attached frame value is a percent of the appropriate side of the base frame.  This is not working yet.
  */
    public final static int PERCENT_BASE_FRAME         = 4;
  /**
  The attached frame value is a percent of the appropriate dimension of the window.  This is not working yet.
  */
    public final static int PERCENT_WINDOW             = 5;

  /**
   * The page units used for creating frames and initializing display
   * lists are inches on a hard copy drawing.  In this case, drawing to the
   * screen will scale the page units to fit them on the screen.
   */
    public final static int PAGE_UNITS_DEFAULT         = 1;

  /**
   * The page units are in screen pixels.  In this case, the
   * page units are not rescaled for drawing to the screen.
   * However, the page units origin is still the lower left,
   * rather than the usual "native" screen origin in the
   * upper left.  Using this type of page unit is intended for
   * static drawing that is not rescaleable and where the
   * window containing the drawing is not resizeable.
   *
    public final static int PAGE_UNITS_ARE_SCREEN_UNITS = 2;


  // frame attachment constants
  /**
  Do not attach the frame to anything.
  */
    public final static int FRAME_NO_ATTACH            = 0;
  /**
  Attach the right bottom point of this frame to the left bottom point of the base frame.
  */
    public final static int FRAME_ATTACH_LEFT_MIN      = 1;
  /**
  Attach the right middle point on this frame to the left middle point on the base frame.
  */
    public final static int FRAME_ATTACH_LEFT_MIDDLE   = 2;
  /**
  Attach the right top point of this frame to the left top point of the base frame
  */
    public final static int FRAME_ATTACH_LEFT_MAX      = 3;
  /**
  Attach the bottom left corner of this frame to the top left corner of the base frame.
  */
    public final static int FRAME_ATTACH_TOP_MIN       = 4;
  /**
  Attach the bottom middle point of this frame to the top middle point of the base frame.
  */
    public final static int FRAME_ATTACH_TOP_MIDDLE    = 5;
  /**
  Attach the bottom right point of this frame to the top right point of the base frame.
  */
    public final static int FRAME_ATTACH_TOP_MAX       = 6;
  /**
  Attach the left bottom point of this frame to the right bottom point of the base frame.
  */
    public final static int FRAME_ATTACH_RIGHT_MIN     = 7;
  /**
  Attach the left middle point of this frame to the right middle point of the base frame.
  */
    public final static int FRAME_ATTACH_RIGHT_MIDDLE  = 8;
  /**
  Attach the left top point of this frame to the right top point of the base frame.
  */
    public final static int FRAME_ATTACH_RIGHT_MAX     = 9;
  /**
  Attach the top left point of this frame to the bottom left point of the base frame.
  */
    public final static int FRAME_ATTACH_BOTTOM_MIN    = 10;
  /**
  Attach the top middle point of this frame to the bottom middle point of the base frame.
  */
    public final static int FRAME_ATTACH_BOTTOM_MIDDLE = 11;
  /**
  Attach the top right point of this frame to the bottom right point of the base frame.
  */
    public final static int FRAME_ATTACH_BOTTOM_MAX    = 12;


  // frame axis identifier constants
  /**
  Identifies the axis on the left side of the frame.
  */
    public final static int FRAME_LEFT_AXIS       = 1;
  /**
  Identifies the axis on the right side of the frame.
  */
    public final static int FRAME_RIGHT_AXIS      = 2;
  /**
  Identifies the axis on the bottom side of the frame.
  */
    public final static int FRAME_BOTTOM_AXIS     = 3;
  /**
  Identifies the axis on the top side of the frame.
  */
    public final static int FRAME_TOP_AXIS        = 4;


  // text anchor constants
  /**
  Anchor text at and rotate around the bottom left point.
  */
    public final static int TEXT_BOTTOM_LEFT           = 1;
  /**
  Anchor text at and rotate around the bottom center point.
  */
    public final static int TEXT_BOTTOM_CENTER         = 2;
  /**
  Anchor text at and rotate around the bottom right point.
  */
    public final static int TEXT_BOTTOM_RIGHT          = 3;
  /**
  Anchor text at and rotate around the center left point.
  */
    public final static int TEXT_CENTER_LEFT           = 4;
  /**
  Anchor text at and rotate around the exact center of the text.
  */
    public final static int TEXT_CENTER_CENTER         = 5;
  /**
  Anchor text at and rotate around the center right point.
  */
    public final static int TEXT_CENTER_RIGHT          = 6;
  /**
  Anchor text at and rotate around the top left point.
  */
    public final static int TEXT_TOP_LEFT              = 7;
  /**
  Anchor text at and rotate around the top center point.
  */
    public final static int TEXT_TOP_CENTER            = 8;
  /**
  Anchor text at and rotate around the top right point.
  */
    public final static int TEXT_TOP_RIGHT             = 9;


  // text background rectangle constants
  /**
  Do not draw a background rectangle for the text.
  */
    public final static int TEXT_BG_NONE               = 0;
  /**
  Draw a filled rectangle without a border or rounded corners.
  */
    public final static int TEXT_BG_FILLED             = 1;
  /**
  Draw a filled rectangle with a border but without rounded corners.
  */
    public final static int TEXT_BG_FILLED_BORDER      = 2;
  /**
  Draw a filled rectangle without a border but with rounded corners.
  */
    public final static int TEXT_BG_CURVE_FILLED       = 3;
  /**
  Draw a filled rectangle with a border and with rounded corners.
  */
    public final static int TEXT_BG_CURVE_FILLED_BORDER = 4;


  // line arrow tip constants
  /**
  Do not draw any arrows at the last point of subsequent lines.
  */
    public final static int NO_ARROW                    = 0;
  /**
  Draw a simple v arrow at the last point of subsequent lines.
  */
    public final static int SIMPLE_ARROW                = 1;
  /**
  Draw an unfilled triangle with its apex at the last point of subsequent lines.
  */
    public final static int TRIANGLE_ARROW              = 2;
  /**
  Draw a filled triangle with its apex at the last point of subsequent lines.
  */
    public final static int FILLED_TRIANGLE_ARROW       = 3;
  /**
  Draw an unfilled triangle with its base at the last point of subsequent lines.
  */
    public final static int OFFSET_TRIANGLE_ARROW       = 4;
  /**
  Draw a filled triangle with its base at the last point of subsequent lines.
  */
    public final static int FILLED_OFFSET_TRIANGLE_ARROW = 5;

/*
    public final static int LITHOLOGY_TYPE_SANDSTONE = 0;
    public final static int LITHOLOGY_TYPE_SILTSTONE = 1;
    public final static int LITHOLOGY_TYPE_SHALE = 2;
    public final static int LITHOLOGY_TYPE_LIMESTONE = 3;
    public final static int LITHOLOGY_TYPE_DOLOMITE = 4;
    public final static int LITHOLOGY_TYPE_EVAPORITE = 5;
    public final static int LITHOLOGY_TYPE_COAL = 6;
    public final static int LITHOLOGY_TYPE_IGNEOUS = 7;
*/

  /**
  The lithology pattern and color for sandstone will be used for subsequent polygon fills.
  */
    public final static int SANDSTONE = 0;
  /**
  The lithology pattern and color for siltstone will be used for subsequent polygon fills.
  */
    public final static int SILTSTONE = 1;
  /**
  The lithology pattern and color for shale will be used for subsequent polygon fills.
  */
    public final static int SHALE = 2;
  /**
  The lithology pattern and color for limestone will be used for subsequent polygon fills.
  */
    public final static int LIMESTONE = 3;
  /**
  The lithology pattern and color for dolomite will be used for subsequent polygon fills.
  */
    public final static int DOLOMITE = 4;
  /**
  The lithology pattern and color for evaporite will be used for subsequent polygon fills.
  */
    public final static int EVAPORITE = 5;
  /**
  The lithology pattern and color for coal will be used for subsequent polygon fills.
  */
    public final static int COAL = 6;
  /**
  The lithology pattern and color for igneous will be used for subsequent polygon fills.
  */
    public final static int IGNEOUS = 7;


    final static int N_LITHOLOGY_TYPES = 8;


    public final static int SELECT_BUTTON_MASK = 1;
    public final static int UNSELECT_BUTTON_MASK = 1<<1;
    public final static int ZOOM_IN_BUTTON_MASK = 1<<2;
    public final static int ZOOM_OUT_BUTTON_MASK = 1<<3;
    public final static int ZOOM_BUTTON_MASK = 1<<4;
    public final static int ZOOM_FULL_BUTTON_MASK = 1<<5;
    public final static int PAN_BUTTON_MASK = 1<<6;
    public final static int REFRESH_BUTTON_MASK = 1<<7;
    public final static int PRINT_BUTTON_MASK = 1<<8;

    public final static int POPUP_UNSELECT_BUTTON_MASK = 1;
    public final static int POPUP_DELETE_BUTTON_MASK = 1<<1;
    public final static int POPUP_HIDE_BUTTON_MASK = 1<<2;
    public final static int POPUP_UNHIDE_BUTTON_MASK = 1<<3;
    public final static int POPUP_PROPERTIES_BUTTON_MASK = 1<<4;

  /*
   *  Define constants for command id messages.
   *  These are not needed outside the package.
   */
    final static String MSG_STRING_SEPARATOR = "~|~";

  /*
   *  control messages
   */
    final static int GTX_LOG_COMMENT       =  8887;
    final static int GTX_OPEN_LOG_FILE     =  8888;
    final static int GTX_CLOSE_LOG_FILE    =  8889;
    final static int GTX_DRAW_CURRENT_VIEW =  9999;
    final static int GTX_DRAW_SELECTED     =  9998;
    final static int GTX_UNSELECT_ALL      =  9997;

    final static int GTX_DELETE_SELECTED   =  9000;
    final static int GTX_HIDE_SELECTED     =  9001;
    final static int GTX_UNHIDE_ALL        =  9002;

    final static int GTX_CREATEWINDOW      =  1;
    final static int GTX_OPENWINDOW        =  2;
    final static int GTX_SETCLIPAREA       =  3;
    final static int GTX_PANFRAME          =  4;
    final static int GTX_SETLAYER          =  7;
    final static int GTX_SETITEM           =  8;
    final static int GTX_SETFRAME          =  9;
    final static int GTX_ZOOMFRAME         = 10;
    final static int GTX_ZOOMEXTENTS       = 11;
    final static int GTX_ZOOMOUT           = 12;
    final static int GTX_FRAME_GAP         = 13;
    final static int GTX_FRAME_AXIS_VALUES = 14;
    final static int GTX_UNSETFRAME        = 15;
    final static int GTX_RESETFRAME        = 16;
    final static int GTX_CREATEFRAME       = 17;
    final static int GTX_DEFAULT_ATTRIB    = 18;
    final static int GTX_DELETEWINDOW      = 19;
    final static int GTX_UNSETLAYER        = 20;
    final static int GTX_UNSETITEM         = 21;
    final static int GTX_DISPLAYFILE       = 22;
    final static int GTX_SAVEINFILE        = 23;
    final static int GTX_REDRAWALL         = 24;
    final static int GTX_SETSCREENSIZE     = 26;
    final static int GTX_SETFRAMECLIP      = 27;
    final static int GTX_ADD_AXIS          = 28;
    final static int GTX_FRAME_NAME_EXTENTS = 29;

  /*
   *  graphic attribute messages
   */
    final static int GTX_SETLINETHICK      = 30;
    final static int GTX_SETFONT           = 31;
    final static int GTX_SETCOLOR          = 32;
    final static int GTX_SETFILLPATTERN    = 33;
    final static int GTX_SETLINEPATTERN    = 34;
    final static int GTX_SETVISIBILITY     = 35;
    final static int GTX_SETTEXTTHICK      = 36;
    final static int GTX_SETFILLSCALE      = 37;
    final static int GTX_IMAGECOLORBANDS   = 38;
    final static int GTX_SYMBOLMASK        = 39;
    final static int GTX_SETSMOOTH         = 40;
    final static int GTX_ARROW_STYLE       = 41;
    final static int GTX_SETBGCOLOR        = 42;
    final static int GTX_SETFGCOLOR        = 43;
    final static int GTX_IMAGENAME         = 44;
    final static int GTX_IMAGEOPTIONS      = 45;
    final static int GTX_TEXTANCHOR        = 46;
    final static int GTX_TEXTOFFSETS       = 47;
    final static int GTX_TEXTBACKGROUND    = 48;

    final static int GTX_SET_ALL_COLORS_FLAG     = 0;
    final static int GTX_SET_FILL_COLOR_FLAG     = 1;
    final static int GTX_SET_LINE_COLOR_FLAG     = 2;
    final static int GTX_SET_TEXT_COLOR_FLAG     = 3;
    final static int GTX_SET_SYMBOL_COLOR_FLAG   = 4;
    final static int GTX_SET_PATTERN_COLOR_FLAG  = 5;
    final static int GTX_SET_TEXTFILL_COLOR_FLAG = 6;
    final static int GTX_SET_BORDER_COLOR_FLAG   = 7;

  /*
   *  primitive drawing messages
   */
    final static int GTX_DRAWLINE          = 60;
    final static int GTX_FILLPOLY          = 61;
    final static int GTX_DRAWSYMBOL        = 62;
    final static int GTX_DRAWTEXT          = 63;
    final static int GTX_DRAWSHAPE         = 64;
    final static int GTX_DRAWIMAGE         = 65;
    final static int GTX_DRAWNUMBER        = 66;

    final static int GTX_BOX_SHAPE         = 1;
    final static int GTX_ARC_SHAPE         = 2;
    final static int GTX_CIRCLE3_SHAPE     = 3;

  /*
   *  picking messages
   */
    final static int GTX_SELECTGROUPNAME   = 70;
    final static int GTX_SET_SELECTABLE    = 71;
    final static int GTX_PICKFILTER        = 72;
    final static int GTX_PICKPRIM          = 73;
    final static int GTX_UNPICK            = 74;
    final static int GTX_PICKGROUP         = 75;
    final static int GTX_ERASEPRIM         = 76;
    final static int GTX_ERASEFLAG         = 77;
    final static int GTX_PICKLINE          = 78;
    final static int GTX_PICKMOVE          = 79;
    final static int GTX_EDITPRIM          = 80;
    final static int GTX_ERASE_SELECTABLE  = 81;
    final static int GTX_CONVERT_TO_FRAME  = 82;
    final static int GTX_SET_SELECT_STATE  = 83;
    final static int GTX_GET_PRIM_NUM      = 84;

    final static int GTX_SET_ASYNC_FLAG    = 90;
    final static int GTX_POLL_ASYNC_PICK   = 91;
    final static int GTX_CANCEL_ASYNC_PICK = 92;

  /*
   * Trimesh, contour and grid message constants.
   */
    final static int GTX_TRIMESH_DATA      = 101;
    final static int GTX_CONTOUR_PROPERTIES = 110;
    final static int GTX_CONTOUR_LINE      = 111;
    final static int GTX_GRID_DATA         = 121;
    final static int GTX_FAULT_LINE_DATA   = 131;
    final static int GTX_IMAGE_FAULT_DATA  = 135;

  /*
   * Polygon boolean messages.
   */
    final static int GTX_POLYGON_BOOLEAN  = 184;
    final static int GTX_POLYGON_BOOLEAN_2  = 186;

    final static int POLY_INTERSECT_OP = 1;
    final static int POLY_UNION_OP = 2;
    final static int POLY_XOR_OP = 3;
    final static int POLY_FRAGMENT_OP = 4;

  /*
   *  graph and chart messages
   */
    final static int GTX_CREATE_GRAPH     = 200;
    final static int GTX_CREATE_AXIS      = 201;
    final static int GTX_CREATE_CURVE     = 202;
    final static int GTX_DRAW_GRAPH       = 203;
    final static int GTX_END_GRAPH        = 204;
    final static int GTX_GRAPH_PARAM      = 205;
    final static int GTX_CREATE_CURVE_FILL= 206;
    final static int GTX_CREATE_POINTS    = 207;
    final static int GTX_CONVERT_GRAPH    = 208;
    final static int GTX_AXIS_LABELS      = 209;
    final static int GTX_CREATE_BARS      = 210;
    final static int GTX_CREATE_LEGEND    = 211;
    final static int GTX_END_LEGEND       = 212;
    final static int GTX_OPEN_GRAPH       = 213;
    final static int GTX_OPEN_LEGEND      = 214;
    final static int GTX_DELETE_CURVE     = 215;
    final static int GTX_DELETE_POINTS    = 216;
    final static int GTX_DELETE_BARS      = 217;
    final static int GTX_DELETE_CURVE_FILL= 218;
    final static int GTX_DELETE_AXIS      = 219;
    final static int GTX_CHANGE_GRAPH_TITLE=220;
    final static int GTX_RESET_GRAPH_PARAM= 221;
    final static int GTX_PRIM_AXES        = 222;

    final static int GTX_CREATE_PIE_CHART = 270;
    final static int GTX_CREATE_PIE_SLICE = 271;
    final static int GTX_DELETE_PIE_SLICE = 272;
    final static int GTX_EDIT_PIE_SLICE   = 273;
    final static int GTX_OPEN_PIE_CHART   = 274;
    final static int GTX_END_PIE_CHART    = 275;
    final static int GTX_DRAW_PIE_CHART   = 276;
    final static int GTX_CONVERT_PIE      = 277;

  /*
   *  misc. messages
   */
    final static int GTX_EDITFLAG        = 2000;
    final static int GTX_LIMIT_MSG       = 4000;

  /*
   * Display list size scaling constants.
   */
    final static int FRAME_NO_SIZE_SCALE = 0;
    final static int FRAME_EXACT_SIZE_SCALE = 1;
    final static int FRAME_AUTO_SIZE_SCALE = 2;

};


