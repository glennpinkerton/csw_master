
/*
         ************************************************
         *                                              *
         *    Copyright (1997-1998) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/


/*
    gtx_current.h

      This is the public header file to be used if a client application
    wants to use C symbolic constants for the gtx_CurrentAttribute call.
    If the client application uses literal values, this header file is
    not needed.

*/

/*  add nothing above this ifndef  */

#ifndef GTX_CURRENT_H

#  define GTX_CURRENT_H

#  include "csw/utils/include/csw_.h"

/*
    Define constants for NULL values.
*/
#  define GTX_NULL_INT             -999999
#  define GTX_NULL_FLOAT           -1.e20

/*
    The data type returned in the value address for an attribute id
    is designated in the comment to the right of each attribute id 
    definition.
*/

/*
    constants defining line attributes
*/
#  define GTX_CURR_LINECOLOR        1           /*  color number returned as int  */
#  define GTX_CURR_LINEPATTERN      2           /*  line pattern number returned as int  */
#  define GTX_CURR_LINETHICK        1010        /*  line thickness returned as CSW_F  */

/*
    constants defining fill attributes
*/
#  define GTX_CURR_FILLCOLOR        10          /*  color for fill returned as int  */
#  define GTX_CURR_FILLPATTERNCOLOR 11          /*  color for cross hatch as int  */
#  define GTX_CURR_FILLPATTERN      12          /*  cross hatch pattern number as int  */
#  define GTX_CURR_FILLPATTERNSIZE  1110        /*  pattern size returned as CSW_F  */
#  define GTX_CURR_FILLPATTERNTHICK 1120        /*  pattern thickness returned as CSW_F  */

/*
    constants defining symbol attributes
*/
#  define GTX_CURR_SYMBOLCOLOR      21          /*  color of symbol returned as int */
#  define GTX_CURR_SYMBCOLOR        21          /*  color of symbol returned as int */
#  define GTX_CURR_SYMBMASK         22          /*  symbol masking flag as int  */

/*
    constants defining text attributes
*/
#  define GTX_CURR_TEXTCOLOR        31          /*  server color number returned as int  */
#  define GTX_CURR_TEXTFILLCOLOR    32          /*  server color number returned as int  */
#  define GTX_CURR_TEXTFONT         33          /*  text font number returned as int  */
#  define GTX_CURR_TEXTTHICK        1310        /*  thickness of text lines as CSW_F  */

/*
    miscellaneous attributes
*/
#  define GTX_CURR_PICKCOLOR        41          /*  color number for highlighting
                                                    picks returned as int  */

/*
    add nothing below this endif
*/
#endif
