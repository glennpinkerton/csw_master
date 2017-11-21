
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    gtx_protoP.h

      This is a private header file that has function prototypes for the 
    functions found in the ply_*.c files.  These functions do not need any
    structure or type definitions other than CSW_F, so I group all of the
    prototypes into this single file.
*/


/*
    If an application attempts to include this file, an error is displayed
    at compile time.  This is a private header file, which the application
    should not need.  If the application defines the macro PRIVATE_HEADERS_OK,
    then it can use this file.  Application programmers should be very careful 
    in bypassing private header security in this fashion.
*/
#ifndef PRIVATE_HEADERS_OK
#error Illegal attempt to include private header file gtx_protoP.h.
#endif



#ifndef GTX_PROTO_P_H
#define GTX_PROTO_P_H

//#include "csw/utils/private_include/csw_P.h"

/*
    typedefs for structures and functions
*/

/*
    functions from gtx_drawco.c
*/
    int gtx_InitPatchPrim (void);
    int gtx_FreePatchPrim (void) ;
    int gtx_ResetPatchPrimCount (void);
    int gtx_AddPatchPrim (int num);
    int gtx_DrawPatchesInClientOrder (void);
    int gtx_RedrawInClientOrder (void);
    int gtx_DrawGenericPrim (void *ptr, int type);
    int gtx_WritePrimsInClientOrder (void);
    int gtx_WriteGenericPrim (void *ptr, int type, int *lenout);
    int gtx_PlotPrimsInClientOrder (void);
    int gtx_PlotGenericPrim (void *ptr, int type, int *lenout);
    int gtx_PlotPatchesInClientOrder (void);
    int gtx_DrawGenericPrimBG (void *ptr, int type, int bgflag);

/*
    functions from gtx_freemem.c
*/
    int gtx_freemapmem (int wid);
    int gtx_free_line_mem (int wid);
    int gtx_free_fill_mem (int wid);
    int gtx_free_text_mem (int wid);
    int gtx_free_symb_mem (int wid);
    int gtx_free_shape_mem (int wid);
    int gtx_free_image_mem (int wid);
    int gtx_free_reuse_mem (int wid);

/*
    functions from gtx_frinfo.c
*/
    int gtx_FrameInit (void);
    int gtx_FrameFree (void);
    int gtx_AddFrameName (double x1, double y1, double x2, double y2,
                          CSW_F px1, CSW_F py1, CSW_F px2, CSW_F py2,
                          int borderflag, int windowid,
                          char *name);
    int gtx_getframepagelimits (int num, CSW_F *x1, CSW_F *y1,
                                CSW_F *x2, CSW_F *y2);
    int gtx_ValidateFrameName (char *name, int wind);
    int gtx_DeleteWindowFrames (int windowid);
    int gtx_DeleteFrameName (char *name, int wid);
    int gtx_DeleteFrameNum (int num);
    int gtx_FrameNumFromName (char *name, int wid);
    int gtx_FrameCoords (int wnum, CSW_F x, CSW_F y,
                         double *fx, double *fy, int *fnum);
    int gtx_WhichFrameNum (int wnum, CSW_F x, CSW_F y);
    int gtx_WriteFrameDataToFile (void);

/*
    functions from gtx_linepick.c
*/
    int gtx_SetupLinePick (int nmax, char *msg, int cnum, int wnum);
    int gtx_SetLinePickTranslations (void);
    int gtx_UnSetLinePickTranslations (void);
    int gtx_ResetLinePick (void);
    int gtx_SetLinePickWindow (void);
    int gtx_AddLinePickPoint (int ix, int iy);
    int gtx_RedrawLinePickVectors (void);
    int gtx_UndoLastLinePick (void);
    int gtx_UndoAllLinePick (void);
    int gtx_LpickCheckWin (void);
    int gtx_ShutDownLinePick (int cnum);
    int gtx_FreeLpickMem (void);
    int gtx_DrawLinePickVector (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int gtx_EraseLinePickVector (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int gtx_remove_line_pick_state_for_client (int cnum);

/*
    functions from gtx_mapparms.c
*/
    int gtx_savplotlimits (double x1, double y1, double x2, double y2);
    int gtx_moveplotlimits (void);
    int gtx_initplotlimits (void);

/*
    functions from gtx_movepick.c
*/
    int gtx_SetupMovePick (char *msg, int cnum, int wnum);
    int gtx_UndoMovePick (void);
    int gtx_OffsetLinePrim (void * vptr, CSW_F dx, CSW_F dy);
    int gtx_OffsetFillPrim (void *vptr, CSW_F dx, CSW_F dy);
    int gtx_OffsetSymbPrim (void *vptr, CSW_F dx, CSW_F dy);
    int gtx_OffsetTextPrim (void *vptr, CSW_F dx, CSW_F dy);
    int gtx_OffsetShapePrim (void *vptr, CSW_F dx, CSW_F dy);
    int gtx_set_graph_move_flag (int val, int wnum);
    int gtx_remove_move_pick_state_for_client (int cnum);
    int gtx_set_move_constraint_flag (int val, int wnum, int cnum);

/*
    functions from gtx_reindex.c
*/
    int gtx_reindex (int flag);
    int gtx_get_used_line_index (int wnum, void **vptr);
    int gtx_get_used_fill_index (int wnum, void **vptr);
    int gtx_get_used_symb_index (int wnum, void **vptr);
    int gtx_get_used_text_index (int wnum, void **vptr);
    int gtx_get_used_shape_index (int wnum, void **vptr);
    int gtx_get_used_line_prim (int wnum, void **vptr);
    int gtx_get_used_fill_prim (int wnum, void **vptr);
    int gtx_get_used_symb_prim (int wnum, void **vptr);
    int gtx_get_used_text_prim (int wnum, void **vptr);
    int gtx_get_used_shape_prim (int wnum, void **vptr);
    int gtx_get_used_line_coords (int wnum, int npts, CSW_F **xy, int *maxpts);
    int gtx_get_used_fill_coords (int wnum, int npts, CSW_F **xy, int *maxpts);
    int gtx_get_used_text_chars (int wnum, int nchar, char *text, char **chardat,
                                 int *maxchar);
    int gtx_bad_used (void);

/*
    functions from gtx_sdrive.c
*/
    void *gtx_CurrentGroupPtr (void);
    int gtx_SetHardDrawFlag (int val);
    int gtx_GetHardDrawFlag (void);
    int gtx_SetKindOfHardDrawFlag (int val);
    int gtx_GetKindOfHardDrawFlag (void);
    int gtx_CurrentWindowNum (void);
    int gtx_setclientlock (int val);
    int gtx_set_in_catch_up_loop (int val);
    int gtx_set_erasing_windows_flag (int val);
    int gtx_InitDispatch (void);
    int gtx_setdrawinit (void);
    int gtx_newserverwin (char *title, char color, CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2,
                          int orderflag, CSW_F scale);
    int gtx_DispatchMsg (void);
    int gtx_setexitpoplock (int val);
    int gtx_GetCurrentClientNum (void);
    int gtx_DecodeColorNum (int ival);
    int gtx_SendBackPickData (void *buff, int size, int status);
    int gtx_SendBackXYData (CSW_F *x, CSW_F *y, int n, int fnum, int wnum, int imnum);
    int gtx_getactiveframelimits (CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gtx_dispatch_pick_msg (void);
    int gtx_get_graph_scaling_stuff (int*, int*, int*);
    int gtx_set_graph_scaling_stuff (int, int, int);

/*
    functions from gtx_setcmap.c
*/
    int gtx_setcmap (void);
    int gtx_pixnum (int gtxindex, int *pixnum);
    int gtx_HowManyCustomColorCells (void);
    int gtx_GrabCustomColorCells (int num, int flag);
    int gtx_FreeCustomColorCell (int ncolor);
    int gtx_SetCustomColorCell (int n, int r, int g, int b);
    int gtx_InitCustomColorData (void);
    int gtx_FreeCustomColorData (void);
    int gtx_ReadColorMap (void);
    int gtx_OpenColorMapFile (void);
    int gtx_CompressColormapFileRecs (int ncolors);
    int gtx_PrintColorWarning (void);
    int gtx_GetDefColorInfo (int num, int *red, int *green, int *blue, int *flag);
    int gtx_ResetPrivateColor (int num, int red, int green, int blue);
    int gtx_GetColorAvailableFlag (void);
    int gtx_WriteColorDataToFile (void);
    int gtx_ColorMapForPlot (void);
    int gtx_FindClosestColor (int in, int r1, int g1, int b1, int *cnum);
    int gtx_SetMinClosestColor (int val);
    int gtx_FindCustomRGB (int n, int *r, int *g, int *b);
    int gtx_GetVisualDepth (void);
    int gtx_SetDoneWithResetFlag (int val);
    int gtx_CleanColorsForExit (void);

/*
    functions from gtx_xylimit.c
*/
    int gtx_getusermaplim (CSW_F *x1, CSW_F *y1, CSW_F *x2, CSW_F *y2);
    int gtx_initxylim (void);
    int gtx_setuserxylim (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int gtx_updatedataxylim (CSW_F x1, CSW_F y1, CSW_F x2, CSW_F y2);
    int gtx_installxylim (void);

/*
    functions from server.c
*/
    int gtx_FreeAllInitializeData (void);

/*
    end of header file
    add nothing below this endif
*/
#endif
