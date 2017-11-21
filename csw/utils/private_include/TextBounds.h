
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

#ifndef _GTX_TEXT_BOUNDS_H
#define _GTX_TEXT_BOUNDS_H

#include "csw/utils/include/csw_.h"

#    if defined(__cplusplus)  ||  defined(c_plusplus)
extern "C"
{
#endif

void gtx_SetTextBoundsJNIFunction (int (*func)(char*, int, float, float*));

void gtx_GetTextBounds (
    char    *text,
    int     font_num,
    CSW_F   size,
    CSW_F   *bounds);

#    if defined(__cplusplus)  ||  defined(c_plusplus)
}  // end of extern "C"
#endif

#endif  
