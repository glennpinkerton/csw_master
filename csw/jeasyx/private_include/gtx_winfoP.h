
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_winfoP.h

    This is a private header file that has function prototypes for the 
    functions found in the gtx_winfo.c file.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gtx_winfoP.h.
#endif



#ifndef GTX_WINFO_P_H
#define GTX_WINFO_P_H

/*
    Include some headers needed for structure typedefs 
    used in the prototypes below.
*/

//#include "csw/utils/private_include/csw_P.h"


int gtx_winfogroupbuttonflag(
    int val);

int gtx_incseqnum(
    int type,
    void * ptr);

int gtx_GetPrimInfo(
    int primnum,
    void ** primptr,
    int * primtype);

int gtx_GetPrimInfo2(
    int primnum,
    int wnum,
    void ** primptr,
    int * primtype);

int gtx_SetPrimInfo(
    int primnum,
    void * primptr,
    int primtype);

int gtx_FreeSeqList(
    int wid);

int gtx_InitMapWindows(
    void);

int gtx_FreeMapWindows(
    void);

int gtx_WindowAvail(
    int nwin);

int gtx_savezoomwidget(
    int wid);

int gtx_resetzoomwidget(
    void);

int gtx_getzoomwidget(
    void);

int gtx_setzoomwidgetnum(
    int num);

int gtx_topoflist(
    void);

int gtx_GetToplevelint(
    int i);

int gtx_deletemapwidget(
    int wid,
    int iflag);

int gtx_AttemptClientKill(
    int cnum);

int gtx_delmapwidgets(
    int wparent);

int gtx_SetZoomDisabled(
    int wid,
    int val);

int gtx_GetZoomDisabled(
    int wid);

int gtx_set_focus_window_for_null(
    int val);

int gtx_get_focus_window_for_null(
    void);

int gtx_widgetnumber(
    int wid);

int gtx_get_data_active(
    void);

int gtx_ShellPositionInfo(
    int wid,
    int * x,
    int * y,
    int * w,
    int * h);

int gtx_getlistwidget(
    int wid);

int gtx_newmaparea(
    int wid,
    int cnum,
    int cwin,
    int order,
    CSW_F scale,
    char * title);

int gtx_ResetintVisibility(
    int wid);

int gtx_SetVisCheckint(
    int wid);

int gtx_CheckForVisibility(
    int w);

int gtx_NullWindowPointers(
    int nwnow);

int gtx_checkforfilldata(
    int parent);

int gtx_checkforshapedata(
    int parent);

int gtx_checkforimagedata(
    int parent);

int gtx_checkforlinedata(
    int parent);

int gtx_checkfortextdata(
    int parent);

int gtx_checkforsymbdata(
    int parent);

int gtx_mapwidget(
    int parent);

char * gtx_getmapmsgtxt(
    int w);

char * gtx_GetInactiveMsgTxt(
    int w);

char * gtx_GetInactiveMsgTxt2(
    int w);

int gtx_setmapmsgtxt(
    int w,
    char * mtext);

char * gtx_getmapmsgtxt2(
    int w);

int gtx_setmapmsgtxt2(
    int w,
    char * mtext);

int gtx_getmsgtextarea(
    int w);

int gtx_getmsgtextarea2(
    int w);

int gtx_createmapwidgets(
    int wparent);

int gtx_setmapwidgets(
    int w,
    int mapw,
    int label,
    int label2);

int gtx_SetMenuBarint(
    int w,
    int menubar);

int gtx_SetFileint(
    int w,
    int filew);

int gtx_SetPlotint(
    int w,
    int plotw);

int gtx_SetGroupint(
    int w,
    int filew,
    int flag);

int gtx_SetZoomint(
    int w,
    int filew,
    int flag);

int gtx_SetExitint(
    int w,
    int filew);

int gtx_SetHelpint(
    int w,
    int filew);

int gtx_SetRedrawint(
    int w,
    int filew);

int gtx_GetMenuBarint(
    int w);

int gtx_GetFileint(
    int w);

int gtx_GetPlotint(
    int w);

int gtx_GetGroupint(
    int w,
    int * flag);

int gtx_GetZoomint(
    int w,
    int * flag);

int gtx_GetExitint(
    int w);

int gtx_GetHelpint(
    int w);

int gtx_GetRedrawint(
    int w);

int gtx_GetExitintByNumber(
    int n);

int gtx_setactive(
    int widin);

int gtx_active_swap_flag(
    int val);

int gtx_setbgpixw(
    int w,
    int pix);

int gtx_setbackgroundcolor(
    char color);

int gtx_getbgpixw(
    int w,
    int * pix);

int gtx_setrealized(
    int w);

int gtx_unrealize(
    int w);

int gtx_getrealized(
    int w);

int gtx_set_read_from_file_flag(
    int val);

int gtx_widgetdestroyed(
    int w);

int gtx_getzoomactivemap(
    void);

int gtx_getfocusmap(
    void);

int gtx_getactivemap(
    void);

int gtx_getactivemapwindow(
    void);

int gtx_getfocusmapwindow(
    void);

int gtx_savfillprimgrid(
    void ** g1,
    void ** g2,
    int nc,
    int nr,
    CSW_F xspac,
    CSW_F yspac,
    CSW_F x1,
    CSW_F y1);

int gtx_savshapeprimgrid(
    void ** g1,
    void ** g2,
    int nc,
    int nr,
    CSW_F xspac,
    CSW_F yspac,
    CSW_F x1,
    CSW_F y1);

int gtx_savlineprimgrid(
    void ** g1,
    void ** g2,
    int nc,
    int nr,
    CSW_F xspac,
    CSW_F yspac,
    CSW_F x1,
    CSW_F y1);

int gtx_savtextprimgrid(
    void ** g1,
    void ** g2,
    int nc,
    int nr,
    CSW_F xspac,
    CSW_F yspac,
    CSW_F x1,
    CSW_F y1);

int gtx_savsymbprimgrid(
    void ** g1,
    void ** g2,
    int nc,
    int nr,
    CSW_F xspac,
    CSW_F yspac,
    CSW_F x1,
    CSW_F y1);

int gtx_getfpgridparm(
    int * nc,
    int * nr,
    CSW_F * xspac,
    CSW_F * yspac,
    CSW_F * x1,
    CSW_F * y1);

int gtx_gethpgridparm(
    int * nc,
    int * nr,
    CSW_F * xspac,
    CSW_F * yspac,
    CSW_F * x1,
    CSW_F * y1);

int gtx_getlpgridparm(
    int * nc,
    int * nr,
    CSW_F * xspac,
    CSW_F * yspac,
    CSW_F * x1,
    CSW_F * y1);

int gtx_gettextgridparm(
    int * nc,
    int * nr,
    CSW_F * xspac,
    CSW_F * yspac,
    CSW_F * x1,
    CSW_F * y1);

int gtx_getsymbgridparm(
    int * nc,
    int * nr,
    CSW_F * xspac,
    CSW_F * yspac,
    CSW_F * x1,
    CSW_F * y1);

char ** gtx_getfpgrid1(
    int win);

char ** gtx_getfpgrid1num(
    int wnum);

char ** gtx_getfpgrid2num(
    int wnum);

char ** gtx_getfpgrid2(
    int win);

char ** gtx_gethpgrid1(
    int win);

char ** gtx_gethpgrid1num(
    int wnum);

char ** gtx_gethpgrid2num(
    int wnum);

char ** gtx_gethpgrid2(
    int win);

char ** gtx_getlpgrid1(
    int win);

char ** gtx_getlpgrid1num(
    int wnum);

char ** gtx_getlpgrid2num(
    int wnum);

char ** gtx_getlpgrid2(
    int win);

char ** gtx_gettextgrid1(
    int win);

char ** gtx_gettextgrid2(
    int win);

char ** gtx_gettextgrid1num(
    int wnum);

char ** gtx_gettextgrid2num(
    int wnum);

char ** gtx_getsymbgrid1(
    int win);

char ** gtx_getsymbgrid1num(
    int wnum);

char ** gtx_getsymbgrid2num(
    int wnum);

char ** gtx_getsymbgrid2(
    int win);

int gtx_savfpbulkpointers(
    void * first,
    void * last);

int gtx_savhpbulkpointers(
    void * first,
    void * last);

int gtx_savlpbulkpointers(
    void * first,
    void * last);

int gtx_savtextbulkpointers(
    void * first,
    void * last);

int gtx_savsymbbulkpointers(
    void * first,
    void * last);

int gtx_savimagebulkpointers(
    void * first,
    void * last);

int gtx_savlpxycoord(
    void * firstxy);

int gtx_savfpxycoord(
    void * firstxy);

int gtx_savtextchar(
    void * firstxy);

int gtx_savfilllinkbulk(
    void * first,
    void * last);

int gtx_savshapelinkbulk(
    void * first,
    void * last);

int gtx_savimagelinkbulk(
    void * first,
    void * last);

char * gtx_getlinelinkbulk(
    int win);

char * gtx_getfilllinkbulk(
    int win);

char * gtx_getshapelinkbulk(
    int win);

char * gtx_getimagelinkbulk(
    int win);

char * gtx_getsymblinkbulk(
    int win);

char * gtx_gettextlinkbulk(
    int win);

int gtx_savlinelinkbulk(
    void * first,
    void * last);

int gtx_savtextlinkbulk(
    void * first,
    void * last);

int gtx_savsymblinkbulk(
    void * first,
    void * last);

char * gtx_getbulkpointers(
    int w,
    int vp);

int gtx_setfordrawdata(
    int w);

int gtx_SetDrawDataActive(
    int val);

int gtx_reshapelimits(
    int w);

int gtx_updatehomelim(
    double x1,
    double y1,
    double x2,
    double y2);

int gtx_setpxywindow(
    double x1,
    double y1,
    double x2,
    double y2);

int gtx_gethomelimits(
    double * x1,
    double * y1,
    double * x2,
    double * y2);

int gtx_getsetupstuff(
    int wnum,
    char * title,
    char * bgcolor,
    int * order,
    CSW_F * scale);

int gtx_getoriglimits(
    double * x1,
    double * y1,
    double * x2,
    double * y2);

int gtx_zoompercent(
    void);

int gtx_getpxywindow(
    int w,
    double * x1,
    double * y1,
    double * x2,
    double * y2);

int gtx_resetzoomlimits(
    double x1,
    double y1,
    double x2,
    double y2);

int gtx_getzoomlimits(
    double * x1,
    double * y1,
    double * x2,
    double * y2);

int gtx_getmapwinxy(
    int w,
    int * ix1,
    int * iy1,
    int * ix2,
    int * iy2);

int gtx_resizeflag(
    void);

int gtx_firstexpose(
    int w);

int gtx_firstmsg(
    int w);

int gtx_setfirstexpose(
    int w);

int gtx_getclientnum(
    int w);

int gtx_getclientnumofwidget(
    int w);

int gtx_set_null_widget_client_num(
    int val);

int gtx_setclientwindownum(
    int w,
    int cwin);

int gtx_getclientwindownum(
    int w);

int gtx_client_win_for_server_win(
    int wnum);

int gtx_MarkDeletedClientWindows(
    int cnum);

int gtx_SaveGroupInfoWinNum(
    int wnum,
    void * bulklist,
    void * childlist,
    void * groupname,
    int ngroup,
    int nchild,
    int ngroupname,
    int groupsince);

int gtx_SetWindowGroupInfo(
    int w,
    void * bulklist,
    void * childlist,
    void * groupname,
    int ngroup,
    int nchild,
    int ngroupname,
    int groupsince);

int gtx_GetWindowGroupInfo(
    int w,
    void ** pa,
    int * ia);

int gtx_SetAllWinPickFlags(
    int val);

int gtx_OkToDoPick(
    void);

int gtx_SetAllWinActiveMsg(
    char * val,
    char * val2);

int gtx_SetPickWinActiveMsg(
    char * val,
    char * val2);

int gtx_ResetPickMessages(
    void);

int gtx_SetWinPickFlag(
    int wnum,
    int val);

int gtx_GetWinPickFlag(
    int wnum);

int gtx_GetConstantScale(
    int w,
    CSW_F * scale);

int gtx_GetWindowUnits(
    int w,
    CSW_F * units);

int gtx_GetRedrawOrder(
    int w);

int gtx_GetPrimNumTotal(
    int w);

void * gtx_GetFirstLineXY(
    int w);

void * gtx_GetFirstLineBulk(
    int w);

void * gtx_GetLastLineBulk(
    int w);

void * gtx_GetFirstLineLinkBulk(
    int w);

void * gtx_GetLastLineLinkBulk(
    int w);

void * gtx_GetFirstFillXY(
    int w);

void * gtx_GetFirstFillBulk(
    int w);

void * gtx_GetLastFillBulk(
    int w);

void * gtx_GetFirstShapeBulk(
    int w);

void * gtx_GetLastShapeBulk(
    int w);

void * gtx_GetFirstImageBulk(
    int w);

void * gtx_GetLastImageBulk(
    int w);

void * gtx_GetFirstFillLinkBulk(
    int w);

void * gtx_GetLastFillLinkBulk(
    int w);

void * gtx_GetFirstTextXY(
    int w);

void * gtx_GetFirstTextBulk(
    int w);

void * gtx_GetLastTextBulk(
    int w);

void * gtx_GetFirstTextLinkBulk(
    int w);

void * gtx_GetLastTextLinkBulk(
    int w);

void * gtx_GetFirstSymbXY(
    int w);

void * gtx_GetFirstSymbBulk(
    int w);

void * gtx_GetLastSymbBulk(
    int w);

void * gtx_GetFirstSymbLinkBulk(
    int w);

void * gtx_GetLastSymbLinkBulk(
    int w);

void * gtx_GetFirstShapeLinkBulk(
    int w);

void * gtx_GetLastShapeLinkBulk(
    int w);

void * gtx_GetFirstImageLinkBulk(
    int w);

void * gtx_GetLastImageLinkBulk(
    int w);

int gtx_FreeAllMapWindows(
    void);

int gtx_RedrawAllServerWindows(
    void);

int gtx_UpdateMapArea(
    int w,
    int cnum,
    int cwin,
    int order,
    CSW_F scale,
    char * title);

int gtx_GetFreeGraphWin(
    int* gwin);

int gtx_SetintDirName(
    int w,
    char * ptr);

char * gtx_GetintDirName(
    int w);

char * gtx_GetWindowDirName(
    int w);

int gtx_get_graph_list(
    int wnum,
    void ** ptr,
    int * nptr);

int gtx_get_legend_list(
    int wnum,
    void ** ptr,
    int * nptr);

int gtx_set_graph_list(
    int wnum,
    void * ptr,
    int nptr);

int gtx_set_legend_list(
    int wnum,
    void * ptr,
    int nptr);

int gtx_get_nerased(
    int wnum);

int gtx_set_nerased(
    int wnum,
    int val);

void * gtx_get_reuseptr(
    int wnum);

int gtx_set_reuseptr(
    int wnum,
    void * ptr);

int gtx_set_last_prim_ptr(
    int wnum,
    int type,
    void * ptr);

int gtx_get_last_prim_ptr(
    int wnum,
    int type,
    void ** ptr);

int gtx_get_embedded_window(
    int wnum,
    int * wid);

int gtx_set_embedded_window(
    int wnum,
    int wid);

int gtx_mapwidgetnum(
    int wnum);

int gtx_embedded_pick_setup(
    int flag,
    int cnum);

int gtx_embedded_pick_shutdown(
    int cnum);

int gtx_option_from_window(
    int wnum,
    int tag,
    int * value);

int gtx_set_options_in_window(
    int wnum,
    void * voptr);

int gtx_change_window_title(
    int wnum,
    char * title);

int gtx_add_available_seqnum(
    int num,
    int wid);

int gtx_sort_unused_seqnum(
    int wid);

int gtx_set_winfo_client_active(
    int val);

int gtx_set_first_prim_ptr(
    int wnum,
    int type,
    void * ptr);

int gtx_get_first_prim_ptr(
    int wnum,
    int type,
    void ** ptr);

int gtx_set_first_prim_widget(
    int w);

int gtx_setfocus(
    int widin,
    int flag);

int gtx_focus_window_num(
    void);

int gtx_set_pick_layers_for_window(
    int wnum,
    int * layer,
    int nlayer);

int gtx_get_pick_layers_for_window(
    int wnum,
    int ** layer,
    int * nlayer);

int gtx_set_async_tag(
    int wnum,
    int tag,
    int motion);

int gtx_get_async_tag(
    int wnum);

int gtx_get_motion_feedback_tag(
    int wnum);

int gtx_get_client_windows(
    int cnum,
    int * wlist,
    int * nwin);

int gtx_check_client_async(
    int cnum,
    int wnum);

int gtx_save_last_messages(
    int w);

int gtx_get_last_messages(
    int w,
    char * msg1,
    char * msg2);

int gtx_reset_last_messages(
    int w);

int gtx_unswap_if_needed(
    int wnum);

int gtx_update_for_erased_window(
    int wnum);

int gtx_window_limits_for_client (
    int, int, int);

#endif
/*
    end of header file
    add nothing below this endif
*/
