
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_javaP.h

    Prototypes for functions to send data to and from java.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gtx_javaP.h.
#endif


#include <csw/utils/include/csw_.h>

/*  add nothing above this ifdef  */

#ifndef GTX_JAVAP_H
#  define GTX_JAVAP_H

int gtx_AppendFillToJavaArea (
    CSW_F *xy,
    int   npts,
    int   red,
    int   green,
    int   blue,
    int   pattern);

int gtx_AppendLineToJavaArea (
    CSW_F *xy,
    int   npts,
    int   red,
    int   green,
    int   blue,
    int   pattern,
    CSW_F thick);

int gtx_AppendTextToJavaArea (
    CSW_F scaled_x, 
    CSW_F scaled_y, 
    char  *text, 
    int   nc,
    int   red, 
    int   green, 
    int   blue,
    CSW_F angle, 
    CSW_F size, 
    int   font);

int gtx_AppendArcToJavaArea (
    CSW_F scaled_x, 
    CSW_F scaled_y, 
    CSW_F scaled_r1, 
    CSW_F scaled_r2,
    CSW_F ang1, 
    CSW_F ang2, 
    int red, 
    int green, 
    int blue, 
    CSW_F thick, 
    CSW_F angle);

int gtx_AppendFilledArcToJavaArea (
    CSW_F scaled_x, 
    CSW_F scaled_y, 
    CSW_F scaled_r1, 
    CSW_F scaled_r2,
    CSW_F ang1, 
    CSW_F ang2, 
    int red, 
    int green, 
    int blue, 
    CSW_F angle);

int gtx_AppendImageToJavaArea (
    CSW_F scaled_x1,
    CSW_F scaled_y1,
    CSW_F scaled_x2,
    CSW_F scaled_y2,
    int   ncol,
    int   nrow,
    unsigned char *red,
    unsigned char *green,
    unsigned char *blue,
    unsigned char *trans);

#endif

/*  add nothing below this endif  */
