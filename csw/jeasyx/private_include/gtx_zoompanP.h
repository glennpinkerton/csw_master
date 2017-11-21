
/*
         ************************************************
         *                                              *
         *    Copyright (1997-1998) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_zoompanP.h

      This header file defines constants and structures used to read
    and write easyx graphics data to the portable graphics file.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gtx_zoompanP.h.
#endif


/*
    add nothing above this ifdef
*/
#ifndef GTX_ZOOMPANP_H
#define GTX_ZOOMPANP_H

#  include "csw/utils/include/csw_.h"

int gtx_DispatchZoom(
    void);

int gtx_zoomflag(
    void);

int gtx_setzoomflag(
    int val);

int gtx_setPanPct(
    double xpct,
    double ypct);

int gtx_panmapwindow(
    void);

int gtx_zoommapwindow(
    void);

int gtx_lastzoomflag(
    void);

int gtx_resetzoomflags(
    int imsg);

int gtx_resetzoomflags2(
    int val);

int gtx_SetZoomFlag2(
    int val);

int gtx_GetZoomFlag2(
    int wnum);

int gtx_redraw_prezoom_msg(
    int wid);

int gtx_get_zoom_pixmap_flag(
    void);

/*
    add nothing below this endif
*/
#endif
