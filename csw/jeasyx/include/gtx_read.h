
/*
         ************************************************
         *                                              *
         *    Copyright (1997-1998) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_read.h

      This is the public header file to be used if a client application
    wants to use C symbolic constants for the gtx_read calls.  If the client
    application uses literal values, this header file is not needed.

        For Fortran, use the gtx_read_f.inc include file.
*/

/*  add nothing above this ifndef  */

#ifndef GTX_READ_H
#  define GTX_READ_H

#  include "csw/utils/include/csw_.h"

/*
    constants defining primitive types
*/
#  define GTX_LINEPRIM         1       
#  define GTX_FILLPRIM         2
#  define GTX_SYMBPRIM         3
#  define GTX_SYMBOLPRIM       3
#  define GTX_TEXTPRIM         4
#  define GTX_SHAPEPRIM        5

#  define GTX_GRAPH_CURVE      1
#  define GTX_GRAPH_BARSET     2
#  define GTX_GRAPH_POINTS     3
#  define GTX_GRAPH_PIE_SLICE  4

#  define GTX_LINE_PRIM         1       
#  define GTX_FILL_PRIM         2
#  define GTX_SYMB_PRIM         3
#  define GTX_SYMBOL_PRIM       3
#  define GTX_TEXT_PRIM         4
#  define GTX_SHAPE_PRIM        5

/*
    Constants defining pick motion constraints.
*/
#  define GTX_NO_CONSTRAINT      0
#  define GTX_CONSTRAIN_TO_X     1
#  define GTX_CONSTRAIN_TO_Y     2

/*
    The data type returned in the value address for an attribute id
    is designated in the comment to the right of each attribute id 
    definition.
*/

/*
    constants defining attributes common to all primitive types
*/
#  define GTX_PRIMTYPE         1           /*  primitive type returned as int  */
#  define GTX_FRAMENUM         8           /*  frame number returned as int  */
#  define GTX_LAYERNUM         9           /*  layer number returned as int  */
#  define GTX_ITEMNUM          10          /*  item number returned as int  */
#  define GTX_VISIBLE          11          /*  visibility flag returned as int  */
#  define GTX_GROUPNAME        2001        /*  group name returned as string  */
#  define GTX_FRAMENAME        2008        /*  frame name returned as string  */
#  define GTX_LAYERNAME        2009        /*  layer name returned as string  */
#  define GTX_ITEMNAME         2010        /*  item name returned as string  */

#  define GTX_PRIM_TYPE        1           /*  primitive type returned as int  */
#  define GTX_FRAME_NUM        8           /*  frame number returned as int  */
#  define GTX_LAYER_NUM        9           /*  layer number returned as int  */
#  define GTX_ITEM_NUM         10          /*  item number returned as int  */
#  define GTX_GROUP_NAME       2001        /*  group name returned as string  */
#  define GTX_FRAME_NAME       2008        /*  frame name returned as string  */
#  define GTX_LAYER_NAME       2009        /*  layer name returned as string  */
#  define GTX_ITEM_NAME        2010        /*  item name returned as string  */

/*
    constants defining line attributes
*/
#  define GTX_LINENPTS         2           /*  number of points returned as int  */
#  define GTX_LINECOLOR        3           /*  server color number returned as int  */
#  define GTX_LINEPATTERN      6           /*  line pattern number returned as int  */
#  define GTX_LINESYMBOL       7           /*  symbol posted on line returned as int  */
#  define GTX_LINETHICK        1001        /*  line thickness returned as CSW_F  */
#  define GTX_LINESIZE         1020        /*  line symbol size returned as CSW_F  */

#  define GTX_LINE_NPTS        2           /*  number of points returned as int  */
#  define GTX_LINE_COLOR       3           /*  server color number returned as int  */
#  define GTX_LINE_PATTERN     6           /*  line pattern number returned as int  */
#  define GTX_LINE_SYMBOL      7           /*  symbol posted on line returned as int  */
#  define GTX_LINE_THICK       1001        /*  line thickness returned as CSW_F  */
#  define GTX_LINE_SIZE        1020        /*  line symbol size returned as CSW_F  */

/*  constants defining fill attributes  */

#  define GTX_FILLNPTS         2           /*  number of vertices returned as int  */
#  define GTX_FILLCOLOR        3           /*  server color for fill returned as int  */
#  define GTX_FILLPATTERNCOLOR 4           /*  server color for cross hatch as int  */
#  define GTX_FILLBORDERCOLOR  5           /*  server color for border returned as int  */
#  define GTX_FILLPATTERN      6           /*  cross hatch pattern number as int  */
#  define GTX_EDGEPATTERN      15          /*  boundary dash pattern number as int  */
#  define GTX_FILLBORDERTHICK  1001        /*  border line thickness returned as CSW_F  */
#  define GTX_FILLPATTERNSIZE  1020        /*  pattern size returned as CSW_F  */

#  define GTX_FILL_NPTS         2           /*  number of vertices returned as int  */
#  define GTX_FILL_COLOR        3           /*  server color for fill returned as int  */
#  define GTX_FILL_PATTERN_COLOR 4          /*  server color for cross hatch as int  */
#  define GTX_FILL_BORDER_COLOR  5          /*  server color for border returned as int  */
#  define GTX_FILL_PATTERN      6           /*  cross hatch pattern number as int  */
#  define GTX_EDGE_PATTERN      15          /*  boundary dash pattern number as int  */
#  define GTX_FILL_BORDER_THICK  1001       /*  border line thickness returned as CSW_F  */
#  define GTX_FILL_PATTERN_SIZE  1020       /*  pattern size returned as CSW_F  */

/*  constants defining symbol attributes  */

#  define GTX_SYMBOLCOLOR      3           /*  server color of symbol returned as int */
#  define GTX_SYMBOL           7           /*  symbol number returned as int  */
#  define GTX_SYMBOLNUMBER     7           /*  symbol number returned as int  */
#  define GTX_SYMBOLTHICK      1001        /*  thickness of symbol lines as CSW_F  */
#  define GTX_SYMBOLANGLE      1010        /*  symbol angle returned as CSW_F  */
#  define GTX_SYMBOLSIZE       1020        /*  symbol size returned as CSW_F  */
#  define GTX_SYMBOLX          1030        /*  symbol x coordinate returned as CSW_F  */
#  define GTX_SYMBOLY          1040        /*  symbol y coordinate returned as CSW_F  */

#  define GTX_SYMBCOLOR        3           /*  server color of symbol returned as int */
#  define GTX_SYMB             7           /*  symbol number returned as int  */
#  define GTX_SYMBNUMBER       7           /*  symbol number returned as int  */
#  define GTX_SYMBTHICK        1001        /*  thickness of symbol lines as CSW_F  */
#  define GTX_SYMBANGLE        1010        /*  symbol angle returned as CSW_F  */
#  define GTX_SYMBSIZE         1020        /*  symbol size returned as CSW_F  */
#  define GTX_SYMBX            1030        /*  symbol x coordinate returned as CSW_F  */
#  define GTX_SYMBY            1040        /*  symbol y coordinate returned as CSW_F  */

#  define GTX_SYMBOL_COLOR     3           /*  server color of symbol returned as int */
#  define GTX_SYMBOL_NUMBER    7           /*  symbol number returned as int  */
#  define GTX_SYMBOL_THICK     1001        /*  thickness of symbol lines as CSW_F  */
#  define GTX_SYMBOL_ANGLE     1010        /*  symbol angle returned as CSW_F  */
#  define GTX_SYMBOL_SIZE      1020        /*  symbol size returned as CSW_F  */
#  define GTX_SYMBOL_X         1030        /*  symbol x coordinate returned as CSW_F  */
#  define GTX_SYMBOL_Y         1040        /*  symbol y coordinate returned as CSW_F  */

#  define GTX_SYMB_COLOR       3           /*  server color of symbol returned as int */
#  define GTX_SYMB_NUMBER      7           /*  symbol number returned as int  */
#  define GTX_SYMB_THICK       1001        /*  thickness of symbol lines as CSW_F  */
#  define GTX_SYMB_ANGLE       1010        /*  symbol angle returned as CSW_F  */
#  define GTX_SYMB_SIZE        1020        /*  symbol size returned as CSW_F  */
#  define GTX_SYMB_X           1030        /*  symbol x coordinate returned as CSW_F  */
#  define GTX_SYMB_Y           1040        /*  symbol y coordinate returned as CSW_F  */

/*  constants defining text attributes  */

#  define GTX_TEXTNCHAR        2           /*  number of characters returned as int  */
#  define GTX_TEXTCOLOR        3           /*  server color number returned as int  */
#  define GTX_TEXTFONT         6           /*  text font number returned as int  */
#  define GTX_TEXTSYMBOL       7           /*  symbol posted with text as int  */
#  define GTX_TEXTTHICK        1001        /*  thickness of text lines as CSW_F  */
#  define GTX_TEXTANGLE        1010        /*  text angle as CSW_F  */
#  define GTX_TEXTSIZE         1020        /*  text size as CSW_F  */
#  define GTX_TEXTX            1030        /*  x position of text as CSW_F  */
#  define GTX_TEXTY            1040        /*  y position of text as CSW_F  */

#  define GTX_TEXT_NCHAR       2           /*  number of characters returned as int  */
#  define GTX_TEXT_COLOR       3           /*  server color number returned as int  */
#  define GTX_TEXT_FONT        6           /*  text font number returned as int  */
#  define GTX_TEXT_SYMBOL      7           /*  symbol posted with text as int  */
#  define GTX_TEXT_THICK       1001        /*  thickness of text lines as CSW_F  */
#  define GTX_TEXT_ANGLE       1010        /*  text angle as CSW_F  */
#  define GTX_TEXT_SIZE        1020        /*  text size as CSW_F  */
#  define GTX_TEXT_X           1030        /*  x position of text as CSW_F  */
#  define GTX_TEXT_Y           1040        /*  y position of text as CSW_F  */

/*  constant for shape type  */

#  define GTX_SHAPETYPE        12          /*  type of shape (arc or box)  */
#  define GTX_BOXSHAPE         1           /*  constant to compare returned 
                                               shape type to to identify a box  */
#  define GTX_ARCSHAPE         2           /*  constant to compare returned 
                                               shape type to to identify an arc  */

#  define GTX_SHAPE_TYPE       12          /*  type of shape (arc or box)  */
#  define GTX_BOX_SHAPE        1           /*  constant to compare returned 
                                               shape type to to identify a box  */
#  define GTX_ARC_SHAPE        2           /*  constant to compare returned 
                                               shape type to to identify an arc  */
/*  constants defining arc attributes  */

#  define GTX_ARCX             1500        /*  x coordinate of arc center  */
#  define GTX_ARCY             1510        /*  y coordinate of arc center  */
#  define GTX_ARCR1            1520        /*  x axis radius of the arc    */
#  define GTX_ARCR2            1530        /*  y axis radius of the arc    */
#  define GTX_ARCANG1          1540        /*  start angle of the arc      */
#  define GTX_ARCANGLEN        1550        /*  angular length of the arc   */
#  define GTX_ARCRANG          1560        /*  rotation angle for x axis radius  */
#  define GTX_ARCFILLFLAG      57          /*  chord, pie or no fill       */

#  define GTX_ARC_X            1500        /*  x coordinate of arc center  */
#  define GTX_ARC_Y            1510        /*  y coordinate of arc center  */
#  define GTX_ARC_R1           1520        /*  x axis radius of the arc    */
#  define GTX_ARC_R2           1530        /*  y axis radius of the arc    */
#  define GTX_ARC_ANG1         1540        /*  start angle of the arc      */
#  define GTX_ARC_ANGLEN       1550        /*  angular length of the arc   */
#  define GTX_ARC_RANG         1560        /*  rotation angle for x axis radius  */
#  define GTX_ARC_FILL_FLAG    57          /*  chord, pie or no fill       */

/*  constants defining box attributes  */

#  define GTX_BOXCENTERX       1500        /*  x coordinate of box center  */
#  define GTX_BOXCENTERY       1510        /*  y coordinate of box center  */
#  define GTX_BOXWIDTH         1520        /*  x axis radius of the box    */
#  define GTX_BOXHEIGHT        1530        /*  y axis radius of the box    */
#  define GTX_BOXCRAD          1540        /*  curve radius for corners    */
#  define GTX_BOXRANG          1550        /*  rotation angle for the box  */

#  define GTX_BOX_CENTER_X     1500        /*  x coordinate of box center  */
#  define GTX_BOX_CENTER_Y     1510        /*  y coordinate of box center  */
#  define GTX_BOX_WIDTH        1520        /*  x axis radius of the box    */
#  define GTX_BOX_HEIGHT       1530        /*  y axis radius of the box    */
#  define GTX_BOX_CRAD         1540        /*  curve radius for corners    */
#  define GTX_BOX_RANG         1550        /*  rotation angle for the box  */

/*  add nothing below this endif  */

#endif
