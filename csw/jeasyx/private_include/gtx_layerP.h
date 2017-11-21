
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    This defines the layer and item structures used in the DisplayList.
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
#ifndef GTX_LAYERP_H
#define GTX_LAYERP_H

#  include "csw/utils/include/csw_.h"

typedef struct {
    int       number;
    int       selectable_flag;
    char      name[100];
}  LAyerStruct;

typedef struct {
    int       number;
    char      name[100];
}  ITemStruct;


#endif
