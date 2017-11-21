
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    easyx_api.h

      This header file supplies prototypes for all of the public 
    functions in the EasyX API.  If you are writing a C++ application
    you need to include this file to access the EasyX API.  If you
    are writing in ANSI C, include this file if you want full prototype
    checking by the compiler.
*/

/*
    Add nothing above this line
*/
#ifndef EASYX_API_H
#  define EASYX_API_H

#  include "csw/utils/include/csw_.h"

/*
    Define constants for the interface.
*/
#define GTX_NO_ARROW                 0
#define GTX_SIMPLE_ARROW             1
#define GTX_TRIANGLE                 2
#define GTX_FILLED_TRIANGLE          3
#define GTX_OFFSET_TRIANGLE          4
#define GTX_OFFSET_FILLED_TRIANGLE   5

  /*
      If a C++ compiler is running, all of the prototypes
      must be declared extern "C".
  */
#    if defined(__cplusplus)  ||  defined(c_plusplus)
        extern "C"
        {
#    endif

      /*
        These functions were originally awkwardly named.  New names
        of equivalent functions are now in place and documented, 
        but these are still available for backward compatability.
      */
        int gtx_BgnPlt (char *, char, CSW_F, CSW_F, CSW_F, CSW_F,
                        int, CSW_F, int);  /* gtx_BeginPlot */
        int gtx_EndPlt (void);  /* gtx_EndPlot */
        int gtx_SymbColor (int); /* gtx_SymbolColor */
        int gtx_LineThk (CSW_F); /* gtx_LineThickness */
        int gtx_TextThk (CSW_F); /* gtx_TextThickness */
        int gtx_GraphAxis (int, int, int, CSW_F, CSW_F, CSW_F,
                           char *, char *);  /* gtx_CreateGraphAxis */
        int gtx_ReadFillAttrib (int, int*, CSW_F*, char*, int);  /* gtx_ReadFillAttribute */
        int gtx_ReadLineAttrib (int, int*, CSW_F*, char*, int);  /* gtx_ReadLineAttribute */
        int gtx_ReadTextAttrib (int, int*, CSW_F*, char*, int);  /* gtx_ReadTextAttribute */
        int gtx_ReadSymbolAttrib (int, int*, CSW_F*, char*, int);/* gtx_ReadSymbolAttribute */
        int gtx_ReadShapeAttrib (int, int*, CSW_F*, char*, int); /* gtx_ReadShapeAttribute */
        int gtx_PattColor (int);  /* gtx_PatternColor */


      /*
          Control Functions
      */
        int gtx_InitClient (char *);
        int gtx_initclientf (char*, int*);
        int gtx_initclientf_ (char*, int*);
        int gtx_SetWindowOption (int, int);
        int gtx_GetWindowOption (int, int*);
        int gtx_RemoveClient (void);
        int gtx_GroupButton (int);
        int gtx_BeginPlot (char *, char, CSW_F, CSW_F, CSW_F, CSW_F,
                           int, CSW_F, int);
        int gtx_BgnPlot (char *, char, CSW_F, CSW_F, CSW_F, CSW_F,
                         int, CSW_F, int);
        int gtx_beginplotf (char*, char*, CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                            int*, CSW_F*, int*, int*);
        int gtx_beginplotf_ (char*, char*, CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                            int*, CSW_F*, int*, int*);
        int gtx_EndPlot (void);
        int gtx_EndPlt (void);
        int gtx_PlotOp (int);
        int gtx_FillOp (int);
        int gtx_Restart (int);
        int gtx_ReStart (int);
        int gtx_CreateFrame (double, double, double, double,
                             CSW_F, CSW_F, CSW_F, CSW_F,
                             int, char *);
        int gtx_createframef (double*, double*, double*, double*,
                             CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                             int*, char*, int*);
        int gtx_createframef_ (double*, double*, double*, double*,
                             CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                             int*, char*, int*);
        int gtx_UnsetFrame (void);
        int gtx_SetFrame (char *);
        int gtx_setframef (char*, int*);
        int gtx_setframef_ (char*, int*);
        int gtx_SetFrameNum (int);
        int gtx_FreeFrame (char *, int);
        int gtx_FreeFrameNum (int, int);
        int gtx_SetFrameClip (int);
        int gtx_DeleteWindow (int);
        int gtx_EraseWindow (int);
        int gtx_FreeWindow (int);
        int gtx_ErrNum (void);
        int gtx_RedrawAllWindows (void);
        int gtx_ChangeWindowTitle (char *);
        int gtx_Time (int);
        int gtx_UnpauseServer (void);
        int gtx_ResetDrawingLimits (void);
        int gtx_GetDrawingLimits (CSW_F*, CSW_F*, CSW_F*, CSW_F*);
        
      /*
          Graphic attribute functions
      */
        int gtx_Color (int);
        int gtx_LineColor (int);
        int gtx_FillColor (int);
        int gtx_PatternColor (int);
        int gtx_PattColor (int);
        int gtx_SymbColor (int);
        int gtx_SymbolColor (int);
        int gtx_TextColor (int);
        int gtx_TextFillColor (int);
        int gtx_PickColor (int);
        int gtx_ColorTable (int);
        int gtx_Font (int);
        int gtx_LineThickness (CSW_F);
        int gtx_LineThk (CSW_F);
        int gtx_TextThickness (CSW_F);
        int gtx_TextThk (CSW_F);
        int gtx_FillPattern (int, CSW_F, CSW_F);
        int gtx_LinePattern (int, CSW_F);
        int gtx_SetImageColors (CSW_F *, CSW_F *, int *, int);
        int gtx_SetImageName (char *);
        int gtx_setimagenamef (char*, int*);
        int gtx_setimagenamef_ (char*, int*);
        int gtx_SetImageOptions (int, int, int, int, int);
        int gtx_MaskSymbol (int);
        int gtx_LineSmooth (int);
        int gtx_LineArrow (int);
        int gtx_CurrentAttribute (int, int*, CSW_F*);

      /*
          Custom color functions
      */
        int gtx_HowManyColors (void);
        int gtx_GrabColors (int);
        int gtx_SetRGB (int, int, int, int);
        int gtx_SetRGBArray (int, int *, int *, int *, int *);
        int gtx_SetHLS (int, int, int, int);
        int gtx_SetHLSArray (int, int *, int *, int *, int *);
        int gtx_SetGray (int, int);
        int gtx_SetGrayArray (int, int *, int *);
        int gtx_RampGray (int, int, int, int);
        int gtx_RampRGB (int, int, int,
                         int, int, int,
                         int, int);
        int gtx_RampHLS (int, int, int,
                         int, int, int,
                         int, int);

      /*
          Default color functions
      */
        int gtx_ResetDefRGB (int, int, int, int);
        int gtx_ResetDefRGBArray (int, int *, int *, int *, int *);
        int gtx_ResetDefHLS (int, int, int, int);
        int gtx_ResetDefHLSArray (int, int *, int *, int *, int *);
        int gtx_ResetDefGray (int, int);
        int gtx_ResetDefGrayArray (int, int *, int *);
        int gtx_GetDefRGB (int, int *, int *, int *, int *);
        int gtx_ResetDefPrompt (void);
        int gtx_ClearPrivateColors (void);
        int gtx_ClosestColor (int *, int *, int *, int *, int);

      /*
          Data structure functions
      */
        int gtx_EditFlag (int);
        int gtx_SetLayer (char *);
        int gtx_setlayerf (char*, int*);
        int gtx_setlayerf_ (char*, int*);
        int gtx_SetLayerNum (int);
        int gtx_UnsetLayer (void);
        int gtx_SetItem (char *);
        int gtx_setitemf (char*, int*);
        int gtx_setitemf_ (char*, int*);
        int gtx_SetItemNum (int);
        int gtx_UnsetItem (void);
        int gtx_FreeLayer (char *);
        int gtx_FreeLayerNum (int);
        int gtx_FreeItem (char *);
        int gtx_FreeItemNum (int);
        int gtx_StartGroup (char *);
        int gtx_startgroupf (char*, int*);
        int gtx_startgroupf_ (char*, int*);
        int gtx_EndGroup (void);
        int gtx_EndAllGroups (void);

      /*
          Drawing functions
      */
        int gtx_Line (CSW_F *, CSW_F *, int);
        int gtx_Fill (CSW_F *, CSW_F *, int *, int, int);
        int gtx_Polygon (CSW_F *, CSW_F *, int *, int, int);
        int gtx_Plot (CSW_F, CSW_F, int);
        int gtx_Text (CSW_F, CSW_F, CSW_F, CSW_F, char *, int);
        int gtx_textf (CSW_F*, CSW_F*, CSW_F*, CSW_F*, char *, int*, int*);
        int gtx_textf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*, char *, int*, int*);
        int gtx_Number (CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, int);
        int gtx_Symbol (CSW_F, CSW_F, CSW_F, CSW_F, int);
        int gtx_Arc (CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, int);
        int gtx_Circle (CSW_F, CSW_F, CSW_F);
        int gtx_Circle2 (CSW_F, CSW_F, CSW_F, CSW_F);
        int gtx_Circle3 (CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F);
        int gtx_Box (CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F);
        int gtx_CenteredBox (CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F);
        int gtx_RawColorImage (unsigned char *, int, int, 
                               CSW_F, CSW_F);
        int gtx_ColorImage (unsigned char *, int, int, int, 
                            CSW_F, CSW_F, CSW_F, CSW_F);
        int gtx_8BitImage (char *, int, int, int,
                           CSW_F, CSW_F, CSW_F, CSW_F);
        int gtx_16BitImage (short *, int, int, int,
                           CSW_F, CSW_F, CSW_F, CSW_F);
        int gtx_32BitImage (int *, int, int, int,
                           CSW_F, CSW_F, CSW_F, CSW_F);
        int gtx_FloatImage (CSW_F *, int, int, CSW_F,
                           CSW_F, CSW_F, CSW_F, CSW_F);
        int gtx_PlotFlush (void);
        int gtx_TextLength (char *, int, int, CSW_F, CSW_F *);
        int gtx_textlengthf (char*, int*, int*, CSW_F*, CSW_F*, int*);
        int gtx_textlengthf_ (char*, int*, int*, CSW_F*, CSW_F*, int*);
        int gtx_NumberLength (CSW_F, int, int, CSW_F, CSW_F *);
        int gtx_SplineFit (CSW_F*, CSW_F*, int, int,
                           CSW_F*, CSW_F*, int, CSW_F, int*);
        int gtx_XFlush (void);

      /*
        The following two functions are not documented.  They are only
        used in some of the test programs, and should not be considered
        for use by applications.
      */
        int gtx_ConvertFrameArray (CSW_F *, int);
        int gtx_ConvertFramePoint (CSW_F *, CSW_F *);

      /*
          Picking functions
      */ 
        int gtx_SetupPick (int);
        int gtx_EndPick (void);
        int gtx_AddPickLayer (char *);
        int gtx_addpicklayerf (char*, int*);
        int gtx_addpicklayerf_ (char*, int*);
        int gtx_AddPickLayerNum (int);
        int gtx_DelPickLayer (char *);
        int gtx_delpicklayerf (char*, int*);
        int gtx_delpicklayerf_ (char*, int*);
        int gtx_DelPickLayerNum (int);
        int gtx_AddPickWindow (int);
        int gtx_DelPickWindow (int);
        int gtx_PickError (int);
        int gtx_PickMsg (char *);
        int gtx_PickErase (int);
        int gtx_ErasePrims (int, int*, int);
        int gtx_EraseGroup (int, char*);
        int gtx_erasegroupf (int*, char*, int*);
        int gtx_erasegroupf_ (int*, char*, int*);
        int gtx_PickPrim (void);
        int gtx_PickMany (void);
        int gtx_PickMove (int);
        int gtx_PickLine (int, CSW_F *, CSW_F *, int *, int *, int *, int *);
        int gtx_PickPoint (CSW_F *, CSW_F *, double *, double *);
        int gtx_GroupNameSel (char *, int);
        int gtx_groupnameself (char*, int*, int*);
        int gtx_groupnameself_ (char*, int*, int*);
        int gtx_SetSelectPrim (int);
        int gtx_GetPageXY (CSW_F *, CSW_F *);
        int gtx_GetFrameXY (double *, double *, char*);
        int gtx_getframexyf (double*, double*, char*);
        int gtx_getframexyf_ (double*, double*, char*);
        int gtx_GetPageDeltaXY (CSW_F *, CSW_F *);
        int gtx_GetFrameDeltaXY (double *, double *);
        int gtx_GetPrimNum (void);
        int gtx_GetGroupNum (void);
        int gtx_GetGroupName (char *);
        int gtx_getgroupnamef (char*, int*);
        int gtx_getgroupnamef_ (char*, int*);
        int gtx_GetWindowNum (void);
        int gtx_GetImageInfo (int *, int *, char *);
        int gtx_getimageinfof (int*, int*, char*, int*);
        int gtx_getimageinfof_ (int*, int*, char*, int*);
        int gtx_PageToFrame (int, int, CSW_F *, CSW_F *, int, double *, double *);
        int gtx_FrameToPage (int, int, double *, double *, int, CSW_F *, CSW_F *);
        int gtx_ArcPageToFrame (int, int,
                                CSW_F, CSW_F, CSW_F, CSW_F,
                                double *, double *, double *, double *);
        int gtx_BoxPageToFrame (int, int,
                                CSW_F, CSW_F, CSW_F, CSW_F, CSW_F,
                                double *, double *, double *, double *, double *);
        int gtx_PageToImage (int, int, 
                             CSW_F *, CSW_F *, int, int *, int *);
        int gtx_ImageToPage (int, int,
                             int *, int *, int, CSW_F *, CSW_F *);
        int gtx_BoxCenter (CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F *, CSW_F *);
        int gtx_BoxLowerLeft (CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F *, CSW_F *);
        int gtx_Unpick (void);
        int gtx_UnPick (void);

      /*
          Async picking functions
      */
        int gtx_AddAsyncCallback (int, char*, void(*)(int, void*, void*), void*);
        int gtx_AsyncPickGraph (int);
        int gtx_AsyncPickLine (int);
        int gtx_AsyncPickMany (int);
        int gtx_AsyncPickMove (int, int);
        int gtx_AsyncPickMoveGraph (int, int);
        int gtx_AsyncPickPoint (int);
        int gtx_AsyncPickPrim (int);
        int gtx_CancelAsyncPick (int, int);
        int gtx_ProcessAsyncPicks (void);
        int gtx_RemoveAsyncCallback (int, char*, void(*)(int, void*, void*), void*);
        int gtx_SetAsyncSignalHandler (void(*)(int));
        int gtx_XtAppMainLoop (void*);
        int gtx_ResetPickFilter (int);

      /*
          Embedded window functions
      */
        int gtx_ProcessXEvent (int, void*);
        int gtx_SetWindowId (int);

      /*
          Display list read functions
      */
        int gtx_ReadPrimType (int, int);
        int gtx_ReadFillData (int, int, CSW_F *, CSW_F *, int, int *, int *);
        int gtx_ReadFillAttribute (int, int*, CSW_F*, char*, int);
        int gtx_readfillattributef (int*, int*, CSW_F*, char*, int*, int*);
        int gtx_readfillattributef_ (int*, int*, CSW_F*, char*, int*, int*);
        int gtx_ReadLineData (int, int, CSW_F *, CSW_F *, int, int *, int *);
        int gtx_ReadLineAttribute (int, int*, CSW_F*, char*, int);
        int gtx_readlineattributef (int*, int*, CSW_F*, char*, int*, int*);
        int gtx_readlineattributef_ (int*, int*, CSW_F*, char*, int*, int*);
        int gtx_ReadMoreXY (CSW_F *, CSW_F *, int, int *, int *);
        int gtx_ReadTextData (int, int, CSW_F *, CSW_F *, 
                              char *, int, int *, int *);
        int gtx_readtextdataf (int*, int*, CSW_F*, CSW_F*, char*, int*, int*, int*, int*);
        int gtx_readtextdataf_ (int*, int*, CSW_F*, CSW_F*, char*, int*, int*, int*, int*);
        int gtx_ReadTextAttribute (int, int*, CSW_F*, char*, int);
        int gtx_readtextattributef (int*, int*, CSW_F*, char*, int*, int*);
        int gtx_readtextattributef_ (int*, int*, CSW_F*, char*, int*, int*);
        int gtx_ReadMoreText (char *, int, int *, int *);
        int gtx_readmoretextf (char*, int*, int*, int*, int*);
        int gtx_readmoretextf_ (char*, int*, int*, int*, int*);
        int gtx_ReadSymbolData (int, int, CSW_F *, CSW_F *, int *);
        int gtx_readsymboldataf (int*, int*, CSW_F*, CSW_F*, int*, int*);
        int gtx_readsymboldataf_ (int*, int*, CSW_F*, CSW_F*, int*, int*);
        int gtx_ReadSymbolAttribute (int, int*, CSW_F*, char*, int);
        int gtx_ReadShapeData (int, int, CSW_F*, CSW_F*, int*);
        int gtx_ReadShapeAttribute (int, int*, CSW_F*, char*, int);

      /*
          Aliasing functions
      */
        int gtx_SetColorAlias (int *, int *, int);
        int gtx_SetFontAlias (int *, int *, int);
        int gtx_SetSymbolAlias (int *, int *, int);

      /*
          File I/O functions
      */
        int gtx_DisplayFile (char *,
                             char *, char *, 
                             CSW_F *, CSW_F *, CSW_F *, CSW_F *,
                             int *, CSW_F *);
        int gtx_displayfilef (char *, char *, char *, 
                             CSW_F *, CSW_F *, CSW_F *, CSW_F *,
                             int *, CSW_F *, int*);
        int gtx_displayfilef_ (char *, char *, char *, 
                              CSW_F *, CSW_F *, CSW_F *, CSW_F *,
                              int *, CSW_F *, int*);
        int gtx_SaveInFile (int, char *);
        int gtx_saveinfilef (int*, char *, int*);
        int gtx_saveinfilef_ (int*, char *, int*);

      /*
          Graph functions
      */
        int gtx_DrawGraph (void);
        int gtx_DrawPieChart (void);
        int gtx_EndGraph (void);
        int gtx_EndLegend (void);
        int gtx_EndPieChart (void);
        int gtx_CreatePieChart (CSW_F, CSW_F, CSW_F, CSW_F, char *, char *, int);
        int gtx_createpiechartf (CSW_F*, CSW_F*, CSW_F*, CSW_F*, char*, char*, int*, int*);
        int gtx_createpiechartf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*, char*, char*, int*, int*);
        int gtx_CreateGraph (CSW_F, CSW_F, CSW_F, CSW_F, char *, char *);
        int gtx_creategraphf (CSW_F*, CSW_F*, CSW_F*, CSW_F*, char*, char*, int*);
        int gtx_creategraphf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*, char*, char*, int*);
        int gtx_CreateLegend (CSW_F, int, CSW_F, CSW_F, char *, char *);
        int gtx_createlegendf (CSW_F*, int*, CSW_F*, CSW_F*, char*, char*, int*);
        int gtx_createlegendf_ (CSW_F*, int*, CSW_F*, CSW_F*, char*, char*, int*);
        int gtx_CreatePieSlice (char*, char*, CSW_F, int, int, int, int);
        int gtx_createpieslicef (char*, char*, CSW_F*, int*, int*, int*, int*, int*);
        int gtx_createpieslicef_ (char*, char*, CSW_F*, int*, int*, int*, int*, int*);
        int gtx_CreateGraphCurve (CSW_F *, CSW_F *, CSW_F *, int,
                                  char *, char *, char *, char *);
        int gtx_creategraphcurvef (CSW_F*, CSW_F*, CSW_F*, int*,
                                  char*, char*, char*, char*, int*);
        int gtx_creategraphcurvef_ (CSW_F*, CSW_F*, CSW_F*, int*,
                                  char*, char*, char*, char*, int*);
        int gtx_CreateGraphBars (int, CSW_F*, CSW_F*, char**, int,
                                 char *, char *, char *, char *);
        int gtx_startgraphbarsf (int*, CSW_F*, CSW_F*, int*,
                                 char*, char*, char*, char*);
        int gtx_startgraphbarsf_ (int*, CSW_F*, CSW_F*, int*,
                                 char*, char*, char*, char*);
        int gtx_addbarlabelsf (char*, int*);
        int gtx_addbarlabelsf_ (char*, int*);
        int gtx_endbarlabelsf (int*);
        int gtx_endbarlabelsf_ (int*);
        int gtx_CreateGraphPoints (CSW_F *, CSW_F *, int *, int,
                                   int, int, CSW_F,
                                   char *, char *, char *, char *);
        int gtx_creategraphpointsf (CSW_F*, CSW_F*, int*, int*,
                                   int*, int*, CSW_F*,
                                   char*, char*, char*, char*, int*);
        int gtx_creategraphpointsf_ (CSW_F*, CSW_F*, int*, int*,
                                   int*, int*, CSW_F*,
                                   char*, char*, char*, char*, int*);
        int gtx_CreateGraphFill (char *, char *, char *, char *);
        int gtx_creategraphfillf (char*, char*, char*, char*, int*);
        int gtx_creategraphfillf_ (char*, char*, char*, char*, int*);

        int gtx_CreateGraphAxis (int, int, int, CSW_F, CSW_F, CSW_F,
                                 char *, char *);
        int gtx_creategraphaxisf (int*, int*, int*, CSW_F*, CSW_F*, CSW_F*,
                                 char*, char*, int*);
        int gtx_creategraphaxisf_ (int*, int*, int*, CSW_F*, CSW_F*, CSW_F*,
                                 char*, char*, int*);
        int gtx_CalcAxisIntervals (CSW_F, CSW_F, int,
                                   CSW_F*, CSW_F*, CSW_F*, CSW_F*);
        int gtx_SetAxisLabels (char *, char **, CSW_F *, int, int);
        int gtx_startaxislabelsf (char*, CSW_F*, int*, int*);
        int gtx_startaxislabelsf_ (char*, CSW_F*, int*, int*);
        int gtx_addaxislabelsf (char*, int*);
        int gtx_addaxislabelsf_ (char*, int*);
        int gtx_endaxislabelsf (int*);
        int gtx_endaxislabelsf_ (int*);
        int gtx_CreateBarAxis (int, char **, int, char *, char *);

        int gtx_SetGraphParam (int, int, CSW_F, char *);
        int gtx_setgraphparamf (int*, int*, CSW_F*, char*, int*);
        int gtx_setgraphparamf_ (int*, int*, CSW_F*, char*, int*);
        int gtx_GetGraphParam (int, int *, CSW_F *, char *, int);
        int gtx_getgraphparamf (int*, int*, CSW_F*, char*, int*, int*);
        int gtx_getgraphparamf_ (int*, int*, CSW_F*, char*, int*, int*);
        int gtx_ConvertGraphCoords (int, CSW_F*, CSW_F*, CSW_F*, CSW_F*, int,
                                    int, char*, char*, char*);
        int gtx_convertgraphcoordsf (int*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, int*,
                                    int*, char*, char*, char*, int*);
        int gtx_convertgraphcoordsf_ (int*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, int*,
                                    int*, char*, char*, char*, int*);

        int gtx_PickGraph (void);
        int gtx_PickMoveGraph (int);
        int gtx_GetGraphDeltaXY (CSW_F *x, CSW_F *y);
        int gtx_GetGraphType (int *);
        int gtx_GetGraphName (char *);
        int gtx_getgraphnamef (char *);
        int gtx_getgraphnamef_ (char *);
        int gtx_GetGraphCurve (char *, int *);
        int gtx_getgraphcurvef (char *, int *);
        int gtx_getgraphcurvef_ (char *, int *);
        int gtx_GetGraphBarset (char *, char *, CSW_F *);
        int gtx_getgraphbarsetf (char *, char *, CSW_F *);
        int gtx_getgraphbarsetf_ (char *, char *, CSW_F *);
        int gtx_GetGraphPoints (char *, int *);
        int gtx_getgraphpointsf (char *, int *);
        int gtx_getgraphpointsf_ (char *, int *);
        int gtx_GetGraphAxes (char *, char *);
        int gtx_getgraphaxesf (char *, char *);
        int gtx_getgraphaxesf_ (char *, char *);
        int gtx_GetGraphXY (CSW_F *, CSW_F *);

        int gtx_GetGraphPieSlice (char*, char*, CSW_F*, int*, int*);
        int gtx_getgraphpieslicef (char*, char*, CSW_F*, int*, int*);
        int gtx_getgraphpieslicef_ (char*, char*, CSW_F*, int*, int*);

        int gtx_OpenGraph (char *);
        int gtx_opengraphf (char*, int*);
        int gtx_opengraphf_ (char*, int*);
        int gtx_OpenLegend (char *);
        int gtx_openlegendf (char*, int*);
        int gtx_openlegendf_ (char*, int*);
        int gtx_OpenPieChart (char*);
        int gtx_openpiechartf (char*, int*);
        int gtx_openpiechartf_ (char*, int*);

        int gtx_DeleteGraphBars (char *);
        int gtx_deletegraphbarsf (char*, int*);
        int gtx_deletegraphbarsf_ (char*, int*);
        int gtx_DeleteGraphCurve (char *);
        int gtx_deletegraphcurvef (char*, int*);
        int gtx_deletegraphcurvef_ (char*, int*);
        int gtx_DeleteGraphFill (char *);
        int gtx_deletegraphfillf (char*, int*);
        int gtx_deletegraphfillf_ (char*, int*);
        int gtx_DeleteGraphPoints (char *);
        int gtx_deletegraphpointsf (char*, int*);
        int gtx_deletegraphpointsf_ (char*, int*);
        int gtx_DeleteGraphAxis (char *);
        int gtx_deletegraphaxisf (char*, int*);
        int gtx_deletegraphaxisf_ (char*, int*);
        int gtx_DeletePieSlice (char *);
        int gtx_deletepieslicef (char*, int*);
        int gtx_deletepieslicef_ (char*, int*);

        int gtx_ChangeGraphTitle (char *, int);
        int gtx_changegraphtitlef (char *, int*, int*);
        int gtx_changegraphtitlef_ (char *, int*, int*);

        int gtx_EditGraphCurve (char*, CSW_F*, CSW_F*, CSW_F*, int, int, char*);
        int gtx_editgraphcurvef (char*, CSW_F*, CSW_F*, CSW_F*, int*, int*, char*, int*);
        int gtx_editgraphcurvef_ (char*, CSW_F*, CSW_F*, CSW_F*, int*, int*, char*, int*);
        int gtx_EditGraphPoints (char*, CSW_F*, CSW_F*, int*, int,
                                 int, int, CSW_F, int, char*);
        int gtx_editgraphpointsf (char*, CSW_F*, CSW_F*, int*, int*,
                                 int*, int*, CSW_F*, int*, char*, int*);
        int gtx_editgraphpointsf_ (char*, CSW_F*, CSW_F*, int*, int*,
                                 int*, int*, CSW_F*, int*, char*, int*);
        int gtx_EditGraphBars (char*, int, CSW_F*, CSW_F*, char**, int, int, char*);
        int gtx_endeditbarlabelsf (int*, int*);
        int gtx_endeditbarlabelsf_ (int*, int*);
        int gtx_EditPieSlice (char*, char*, CSW_F, int, int, int, int, int);
        int gtx_editpieslicef (char*, char*, CSW_F*, int*, int*, int*, int*, int*, int*);
        int gtx_editpieslicef_ (char*, char*, CSW_F*, int*, int*, int*, int*, int*, int*);

        int gtx_ResetSliceParam (char*, int, int, CSW_F, char*);
        int gtx_resetsliceparamf (char*, int*, int*, CSW_F*, char*, int*);
        int gtx_resetsliceparamf_ (char*, int*, int*, CSW_F*, char*, int*);
        int gtx_ResetCurveParam (char*, int, int, CSW_F, char*);
        int gtx_resetcurveparamf (char*, int*, int*, CSW_F*, char*, int*);
        int gtx_resetcurveparamf_ (char*, int*, int*, CSW_F*, char*, int*);
        int gtx_ResetPointParam (char*, int, int, CSW_F, char*);
        int gtx_resetpointparamf (char*, int*, int*, CSW_F*, char*, int*);
        int gtx_resetpointparamf_ (char*, int*, int*, CSW_F*, char*, int*);
        int gtx_ResetBarParam (char*, int, int, CSW_F, char*);
        int gtx_resetbarparamf (char*, int*, int*, CSW_F*, char*, int*);
        int gtx_resetbarparamf_ (char*, int*, int*, CSW_F*, char*, int*);
        int gtx_ResetFillParam (char*, int, int, CSW_F, char*);
        int gtx_resetfillparamf (char*, int*, int*, CSW_F*, char*, int*);
        int gtx_resetfillparamf_ (char*, int*, int*, CSW_F*, char*, int*);

        int gtx_GraphText (char*, char*, 
                           CSW_F, CSW_F, CSW_F, CSW_F, char*, int);
        int gtx_graphtextf (char*, char*, 
                            CSW_F*, CSW_F*, CSW_F*, CSW_F*, char*, int*, int*);
        int gtx_graphtextf_ (char*, char*, 
                             CSW_F*, CSW_F*, CSW_F*, CSW_F*, char*, int*, int*);
        int gtx_GraphLine (char*, char*,
                           CSW_F*, CSW_F*, int);
        int gtx_graphlinef (char*, char*,
                            CSW_F*, CSW_F*, int*, int*);
        int gtx_graphlinef_ (char*, char*,
                             CSW_F*, CSW_F*, int*, int*);
        int gtx_GraphFill (char*, char*,
                           CSW_F*, CSW_F*, int*, int, int);
        int gtx_graphfillf (char*, char*,
                            CSW_F*, CSW_F*, int*, int*, int*, int*);
        int gtx_graphfillf_ (char*, char*,
                             CSW_F*, CSW_F*, int*, int*, int*, int*);
        int gtx_GraphSymbol (char*, char*,
                             CSW_F, CSW_F, CSW_F, CSW_F, int);
        int gtx_graphsymbolf (char*, char*,
                              CSW_F*, CSW_F*, CSW_F*, CSW_F*, int*, int*);
        int gtx_graphsymbolf_ (char*, char*,
                              CSW_F*, CSW_F*, CSW_F*, CSW_F*, int*, int*);
        int gtx_GraphArc (char*, char*,
                          CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, int);
        int gtx_grapharcf (char*, char*,
                           CSW_F*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, int*, int*);
        int gtx_grapharcf_ (char*, char*,
                           CSW_F*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, int*, int*);
        int gtx_GraphCircle (char*, char*,
                             CSW_F, CSW_F, CSW_F);
        int gtx_graphcirclef (char*, char*,
                             CSW_F*, CSW_F*, CSW_F*, int*);
        int gtx_graphcirclef_ (char*, char*,
                             CSW_F*, CSW_F*, CSW_F*, int*);
        int gtx_GraphBox (char*, char*,
                          CSW_F, CSW_F, CSW_F, CSW_F, CSW_F, CSW_F);
        int gtx_graphboxf (char*, char*,
                          CSW_F*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, int*);
        int gtx_graphboxf_ (char*, char*,
                          CSW_F*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, CSW_F*, int*);

    /*
        graph convenience functions
    */
        int gtx_SimpleXYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                               CSW_F*, CSW_F*, int);
        int gtx_SimpleLogXYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                                  CSW_F*, CSW_F*, int);
        int gtx_SimpleXLogYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                                  CSW_F*, CSW_F*, int);
        int gtx_SimpleLogXLogYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                                     CSW_F*, CSW_F*, int);

        int gtx_XYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                         char*, CSW_F*, CSW_F*, int, int,
                         char*, char*, char*);
        int gtx_xygraphf (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                         char*, CSW_F*, CSW_F*, int*, int*,
                         char*, char*, char*, int*);
        int gtx_xygraphf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                         char*, CSW_F*, CSW_F*, int*, int*,
                         char*, char*, char*, int*);
        int gtx_LogXYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                            char*, CSW_F*, CSW_F*, int, int,
                            char*, char*, char*);
        int gtx_logxygraphf (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                            char*, CSW_F*, CSW_F*, int*, int*,
                            char*, char*, char*, int*);
        int gtx_logxygraphf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                            char*, CSW_F*, CSW_F*, int*, int*,
                            char*, char*, char*, int*);
        int gtx_XLogYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                            char*, CSW_F*, CSW_F*, int, int,
                            char*, char*, char*);
        int gtx_xlogygraphf (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                            char*, CSW_F*, CSW_F*, int*, int*,
                            char*, char*, char*, int*);
        int gtx_xlogygraphf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                            char*, CSW_F*, CSW_F*, int*, int*,
                            char*, char*, char*, int*);
        int gtx_LogXLogYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                            char*, CSW_F*, CSW_F*, int, int,
                            char*, char*, char*);
        int gtx_logxlogygraphf (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                            char*, CSW_F*, CSW_F*, int*, int*,
                            char*, char*, char*, int*);
        int gtx_logxlogygraphf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                            char*, CSW_F*, CSW_F*, int*, int*,
                            char*, char*, char*, int*);

        int gtx_MultiXYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                              char*, CSW_F*, CSW_F*, int*, int, int,
                              int*, int*, int*, int,
                              char*, char*, char*);
        int gtx_multixygraphf (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                              char*, CSW_F*, CSW_F*, int*, int*, int*,
                              int*, int*, int*, int*,
                              char*, char*, char*, int*);
        int gtx_multixygraphf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                              char*, CSW_F*, CSW_F*, int*, int*, int*,
                              int*, int*, int*, int*,
                              char*, char*, char*, int*);
        int gtx_MultiLogXYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                              char*, CSW_F*, CSW_F*, int*, int, int,
                              int*, int*, int*, int,
                              char*, char*, char*);
        int gtx_multilogxygraphf (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                              char*, CSW_F*, CSW_F*, int*, int*, int*,
                              int*, int*, int*, int*,
                              char*, char*, char*, int*);
        int gtx_multilogxygraphf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                              char*, CSW_F*, CSW_F*, int*, int*, int*,
                              int*, int*, int*, int*,
                              char*, char*, char*, int*);
        int gtx_MultiXLogYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                              char*, CSW_F*, CSW_F*, int*, int, int,
                              int*, int*, int*, int,
                              char*, char*, char*);
        int gtx_multixlogygraphf (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                              char*, CSW_F*, CSW_F*, int*, int*, int*,
                              int*, int*, int*, int*,
                              char*, char*, char*, int*);
        int gtx_multixlogygraphf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                              char*, CSW_F*, CSW_F*, int*, int*, int*,
                              int*, int*, int*, int*,
                              char*, char*, char*, int*);
        int gtx_MultiLogXLogYGraph (CSW_F, CSW_F, CSW_F, CSW_F,
                              char*, CSW_F*, CSW_F*, int*, int, int,
                              int*, int*, int*, int,
                              char*, char*, char*);
        int gtx_multilogxlogygraphf (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                              char*, CSW_F*, CSW_F*, int*, int*, int*,
                              int*, int*, int*, int*,
                              char*, char*, char*, int*);
        int gtx_multilogxlogygraphf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                              char*, CSW_F*, CSW_F*, int*, int*, int*,
                              int*, int*, int*, int*,
                              char*, char*, char*, int*);

        int gtx_SimpleHistogram (CSW_F, CSW_F, CSW_F, CSW_F,
                                 CSW_F, CSW_F, CSW_F*, int);
        int gtx_SimpleLogHistogram (CSW_F, CSW_F, CSW_F, CSW_F,
                                    CSW_F, CSW_F, CSW_F*, int);
        int gtx_Histogram (CSW_F, CSW_F, CSW_F, CSW_F,
                           char*, CSW_F, CSW_F, CSW_F*, int, int,
                           char*, char*, char*);
        int gtx_histogramf (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                           char*, CSW_F*, CSW_F*, CSW_F*, int*, int*,
                           char*, char*, char*, int*);
        int gtx_histogramf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                           char*, CSW_F*, CSW_F*, CSW_F*, int*, int*,
                           char*, char*, char*, int*);
        int gtx_LogHistogram (CSW_F, CSW_F, CSW_F, CSW_F,
                              char*, CSW_F, CSW_F, CSW_F*, int, int,
                              char*, char*, char*);
        int gtx_loghistogramf (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                              char*, CSW_F*, CSW_F*, CSW_F*, int*, int*,
                              char*, char*, char*, int*);
        int gtx_loghistogramf_ (CSW_F*, CSW_F*, CSW_F*, CSW_F*,
                              char*, CSW_F*, CSW_F*, CSW_F*, int*, int*,
                              char*, char*, char*, int*);

        int gtx_SimplePieChart (CSW_F, CSW_F, CSW_F, CSW_F, CSW_F*, int);
        int gtx_PieChart (CSW_F, CSW_F, CSW_F, CSW_F,
                          char*, char*, int,
                          CSW_F*, char**, int*, int*, int);

  /*
      The extern "C" block needs to be closed if this 
      is being run through a C++ compiler.
  */
#    if defined (__cplusplus)  ||  defined (c_plusplus)
        }
#    endif

/*
    Add nothing to this file below the following endif
*/
#endif
