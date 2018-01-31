
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/


/*
 ***************************************************************************

  This file has functions used to process the EasyX graphics commands sent
  from Java.    This is normally called from the jni function.  I have put
  this extra level in to allow for debugging of the native side only via a
  main program that calls this directly.  This isolates the jni functions
  to a fairly good degree.

 ***************************************************************************
*/

/*
 * This debug flag is the mechanism used for debugging the
 * native side of the 2d graphics code.  Eventually, this
 * will be set to zero, but for the first several usages
 * from application programmers, I will keep it set at 1 so
 * I can debug native code problems that come up.
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * NOTE THAT THIS FLAG SHOULD **NEVER** BE SET TO 1 FOR
 * ANYTHING OTHER THAN DEBUGGING CODE
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */
#define _EZX_DEBUG_LOG_FILE_        1

#include <stdio.h>

#include "csw/jeasyx/private_include/DisplayList.h"
#include "csw/jeasyx/private_include/EZXCommand.h"
#include "csw/jeasyx/private_include/gtx_drawprim.h"

#include "csw/surfaceworks/include/grid_api.h"
#include "csw/hlevutils/src/ThreadGuard.h"
#include "csw/hlevutils/src/GraphicsCanvasManager.h"

#include "csw/jeasyx/private_include/gtx_msgP.h"
#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_scope.h"



/*
 * The functions in this file are called from the C JNI functions, so
 * they are extern C here as well as in the header file.
 */

extern "C" {

int ezx_process_command (
    int           dlist_index,
    int           command_id,
    int           threadid,
    long          *llist,
    int           *ilist,
    char          *cdata,
    unsigned char *bdata,
    short int     *sdata,
    int           *idata,
    float         *fdata,
    double        *ddata,
    FILE          *dfile)
{

    CDisplayList  *dlist = NULL;

    int           istat = 0;
    int           native_dlist_id = 0;
    int           primnum = -1;

    int itype;
    int ncol;
    int nrow;
    int n, nstart;

    FILE  *LogFile = NULL;


    auto fscope = [&]()
    {
        if (LogFile) {
            fflush (LogFile);
        }
    };
    CSWScopeGuard func_scope_guard (fscope);



#if _EZX_DEBUG_LOG_FILE_
    int           i, ntot;
    char          LogFileLine[1000];
#endif

    native_dlist_id = -999;
    dfile = dfile;


// Get a pointer to the canvas manager associated with this thread.

    CanvasManager    *canvas_manager_ptr = ThreadGuard::GetCanvasManager (threadid);

    if (canvas_manager_ptr == NULL) {
        return -1;
    }

    /*
     * Commands to open and close log files.
     * cdata has the log file name
     */
#if _EZX_DEBUG_LOG_FILE_
    LogFile = ThreadGuard::GetEZLogFile (threadid);
    if (command_id == GTX_OPEN_LOG_FILE) {
        if (LogFile == NULL) {
            LogFile = fopen (cdata, "wb");
        }
        ThreadGuard::SetEZLogFile (threadid, LogFile);
        return 1;
    }

    if (command_id ==  GTX_CLOSE_LOG_FILE) {
        if (LogFile) {
            fclose (LogFile);
            LogFile = NULL;
            ThreadGuard::SetEZLogFile (threadid, NULL);
        }
        return 1;
    }
#else
    if (command_id == GTX_OPEN_LOG_FILE) {
        return 1;
    }
    if (command_id ==  GTX_CLOSE_LOG_FILE) {
        return 1;
    }
#endif


/*
 * If this is a delete window command, do that and nothing else.
 */
    if (command_id == GTX_DELETEWINDOW) {

      #if _EZX_DEBUG_LOG_FILE_
        if (LogFile) {
            if (command_id < GTX_OPEN_LOG_FILE  ||
                command_id > GTX_CLOSE_LOG_FILE) {
                sprintf (LogFileLine, "\ncommand=%d\n", command_id);
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine, "%d\n", ilist[0]);
                fputs (LogFileLine, LogFile);
            }
        }
      #endif

        istat = canvas_manager_ptr->ezx_RemoveGraphicsCanvasFromManager (ilist[0]);
        return istat;
    }

/*
 * if command logging is active, and if the command is not
 * to open or close the log file, write the command= line
 * to the log file.
 */
#if _EZX_DEBUG_LOG_FILE_
    if (LogFile) {
        if (command_id < GTX_OPEN_LOG_FILE  ||
            command_id > GTX_CLOSE_LOG_FILE) {
            sprintf (LogFileLine, "\ncommand=%d %d\n",
                     command_id, dlist_index);
            fputs (LogFileLine, LogFile);
        }
    }
#endif


/*--------------------------------------------------------------
 *  If this is a GET_SYMBOL_PARTS command, do that and nothing else
 *
 * ilist[0] has the symbol number
 * ddata[0] has the symbol size
 * ddata[1] has the symbol angle
 * ilist[1] has the device dpi
 */
    if (command_id == GTX_GET_SYMBOL_PARTS)
    {

        GTXDrawPrim   gtx_drawprim_obj;

      #if _EZX_DEBUG_LOG_FILE_
        if (LogFile) {
            sprintf (LogFileLine, "command=%d 0\n", command_id);
            fputs (LogFileLine, LogFile);
            sprintf (LogFileLine,
                     "%d %.15e %.15e %d\n",
                     ilist[0],
                     ddata[0],
                     ddata[1],
                     ilist[1]);
            fputs (LogFileLine, LogFile);
        }
      #endif

        istat =
        gtx_drawprim_obj.gtx_SendBackSymbolParts (ilist[0],
                                        ddata[0],
                                        ddata[1],
                                        ilist[1]);

       return 1;
    }


    canvas_manager_ptr->ezx_SetActiveGraphicsCanvas (dlist_index);

    long java_num = dlist_index;
    char *name = cdata;

    int  new_dlindex = -1;

    dlist = canvas_manager_ptr->ezx_GetActiveDisplayList ();

    if (dlist == NULL) {
        void  *v_env = ezx_get_void_jenv (threadid, NULL);
        void  *v_obj = ezx_get_void_jobj (threadid, NULL);
        istat =
            canvas_manager_ptr->ezx_AddGraphicsCanvasToManager
                (name, java_num, v_env, v_obj);
        if (istat == -1) {
            return -1;
        }
        dlist =
            canvas_manager_ptr->ezx_GetActiveDisplayList ();
        if (dlist == NULL) {
            return -1;
        }
        new_dlindex = canvas_manager_ptr->ezx_GetActiveIndex ();
    }

    CSWGrdAPI    *gapi = ThreadGuard::GetGrdAPI (threadid);
    dlist->SetGrdAPIPtr (gapi);

    native_dlist_id = istat;

// short circuit to see if still crashes
//if (true) {
//return 1;
//}


    primnum = -1;

/*
 * This giant switch statement processes each command.
 * If we get this far, the dlist pointer is valid.
 */
    istat = 1;
    switch (command_id) {

    /*--------------------------------------------------------------
     * Put a comment into the log file.
     */
        case GTX_LOG_COMMENT:
          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine, "\n##// %s\n", cdata);
                fputs (LogFileLine, LogFile);
            }
          #endif

            break;

    /*--------------------------------------------------------------
     * Draw the entire display list.
     */
        case GTX_DRAW_CURRENT_VIEW:

            dlist->Draw (NULL);
            break;

    /*--------------------------------------------------------------
     * Draw the selected objects in the display list.
     */
        case GTX_DRAW_SELECTED:

            dlist->DrawSelected ();
            break;

    /*--------------------------------------------------------------
     * Change the selected objects to unselected objects in the display list.
     */
        case GTX_UNSELECT_ALL:

            dlist->UnselectAll ();
            break;

    /*--------------------------------------------------------------
     *  Delete the selected objects;
     */
        case GTX_DELETE_SELECTED:

            dlist->DeleteSelected ();
            break;

    /*--------------------------------------------------------------
     *  Make the selected objects invisible.
     */
        case GTX_HIDE_SELECTED:

            dlist->HideSelected ();
            break;

    /*--------------------------------------------------------------
     * Turn all invisible objects to visible.
     */
        case GTX_UNHIDE_ALL:

            dlist->UnhideAll ();
            break;

    /*--------------------------------------------------------------
     * Set all graphic attributes to defaults.
     */
        case GTX_DEFAULT_ATTRIB:

            dlist->SetDefaultGraphicAttributes ();
            break;

    /*--------------------------------------------------------------
     * Create a new display list.
     *
     *  cdata has the name for the display list
     *  llist[0] has the java hashcode id for the JDisplayList object
     *  ilist[0] has the screen dpi
     *  ilist[1] has the page units type
     *  ddata[0] has the approximate xmin in page units
     *  ddata[1] has the approximate ymin in page units
     *  ddata[2] has the approximate xmax in page units
     *  ddata[3] has the approximate ymax in page units
     */
        case GTX_CREATEWINDOW:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%d %d %d %ld %.15e %.15e %.15e %.15e\n",
                         new_dlindex,
                         ilist[0],
                         ilist[1],
                         llist[0],
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetDrawingBoundsHint (
                ddata[0],
                ddata[1],
                ddata[2],
                ddata[3]
            );

            dlist->SetPageUnitsType (ilist[1]);

            dlist->SetScreenDPI (ilist[0]);

            break;

    /*---------------------------------------------------------------
     * Set the screen size for drawing the display list.
     *
     *  ilist[0] has the screen width in java user space units
     *  ilist[1] has the screen height in java user space units
     */
        case GTX_SETSCREENSIZE:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d\n",
                         ilist[0],
                         ilist[1]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetScreenBounds (
                0.0,
                0.0,
                (double)ilist[0],
                (double)ilist[1]
            );

            break;

    /*---------------------------------------------------------------
     * Set the clipping area in either page or java user units.
     *
     *  ddata[0] has xmin
     *  ddata[1] has ymin
     *  ddata[2] has xmax
     *  ddata[3] has ymax
     *  ilist[0] has flag for page (1) or java (2) units
     */
        case GTX_SETCLIPAREA:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %d\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3],
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            if (ilist[0] == 1) {
                dlist->SetPageClipLimits (
                    ddata[0],
                    ddata[1],
                    ddata[2],
                    ddata[3]
                );
            }
            else {
                dlist->SetScreenClipLimits (
                    ddata[0],
                    ddata[1],
                    ddata[2],
                    ddata[3]
                );
            }

            break;

    /*--------------------------------------------------------------
     * Set the minimum frame gap
     *
     *  ddata[0] has the frame gap
     */
        case GTX_FRAME_GAP:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e\n",
                         ddata[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetMinimumFrameSeparation (ddata[0]);
            break;

    /*---------------------------------------------------------------
     * Create a frame for subsequent drawing.
     *
     *  ddata[0] has frame x1
     *  ddata[1] has frame y1
     *  ddata[2] has frame x2
     *  ddata[3] has frame y2
     *  ddata[4] has page x1
     *  ddata[5] has page y1
     *  ddata[6] has page x2
     *  ddata[7] has page y2
     *  ddata[8] has frame global xmin
     *  ddata[9] has frame global ymin
     *  ddata[10] has frame global xmax
     *  ddata[11] has frame global ymax
     *  ddata[12] has extra gap
     *  ddata[13] has perpendicular move
     *  ilist[0] has border flag
     *  ilist[1] has rescaleable flag
     *  ilist[2] has moveable flag
     *  ilist[3] has aspect flag
     *  ilist[4] has attach position
     *  ilist[5] has scale width to attach
     *  ilist[6] has scale height to attach
     *  cdata[0] has name, attach frame name, and captions
     */
        case GTX_CREATEFRAME:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3]
                        );
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e\n",
                         ddata[4],
                         ddata[5],
                         ddata[6],
                         ddata[7]
                        );
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %.15e\n",
                         ddata[8],
                         ddata[9],
                         ddata[10],
                         ddata[11],
                         ddata[12],
                         ddata[13]
                        );
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5],
                         ilist[6]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = dlist->CreateFrame (
                ilist[1],
                ilist[2],
                ilist[3],
                ddata[8],
                ddata[9],
                ddata[10],
                ddata[11],
                ddata[0],
                ddata[1],
                ddata[2],
                ddata[3],
                (CSW_F)ddata[4],
                (CSW_F)ddata[5],
                (CSW_F)ddata[6],
                (CSW_F)ddata[7],
                ilist[0],
                ilist[4],
                ddata[12],
                ddata[13],
                cdata,
                ilist[5],
                ilist[6]
            );

            break;


    /*---------------------------------------------------------
     * Add an axis that is not part of a frame border.
     *
     *  cdata has the optional axis caption
     *  ilist[0] has label direction
     *  ilist[1] has label_flag
     *  ilist[2] has tick_flag
     *  ilist[3] has line_red
     *  ilist[4] has line_green
     *  ilist[5] has line_blue
     *  ilist[6] has text_red
     *  ilist[7] has text_green
     *  ilist[8] has text_blue
     *  ilist[9] has text_font
     *  ilist[10] has caption flag
     *  ilist[11] has tick direction
     *  ddata[0] has major_interval
     *  ddata[1] has line_thickness
     *  ddata[2] has text_size
     *  ddata[3] has text_thickness
     *  ddata[4] has x1
     *  ddata[5] has y1
     *  ddata[6] has x2
     *  ddata[7] has y2
     *  ddata[8] has firstValue
     *  ddata[9] has lastValue
     */
        case GTX_ADD_AXIS:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                LogFileLine[0] = '\0';
                if (cdata != NULL) {
                    strcpy (LogFileLine, cdata);
                }
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d %d %d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5],
                         ilist[6],
                         ilist[7],
                         ilist[8],
                         ilist[9],
                         ilist[10],
                         ilist[11]);
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %.15e %.15e %.15e %.15e %.15e\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3],
                         ddata[4],
                         ddata[5],
                         ddata[6],
                         ddata[7],
                         ddata[8],
                         ddata[9]);
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat =
            dlist->AddAxis (cdata,
                            ilist[0],
                            ilist[1],
                            ilist[2],
                            ilist[11],
                            ddata[0],
                            ilist[3],
                            ilist[4],
                            ilist[5],
                            ilist[6],
                            ilist[7],
                            ilist[8],
                            ddata[1],
                            ddata[2],
                            ddata[3],
                            ilist[9],
                            ilist[10],
                            ddata[4],
                            ddata[5],
                            ddata[6],
                            ddata[7],
                            ddata[8],
                            ddata[9],
                            ilist[12],
                            ddata[10]
                           );

            break;

    /*---------------------------------------------------------
     * Change the axis parameters for a single frame axis.
     *
     *  cdata has the frame name and optionally the axis caption
     *  ilist[0] has axis_id
     *  ilist[1] has label_flag
     *  ilist[2] has tick_flag
     *  ilist[3] has line_red
     *  ilist[4] has line_green
     *  ilist[5] has line_blue
     *  ilist[6] has text_red
     *  ilist[7] has text_green
     *  ilist[8] has text_blue
     *  ilist[9] has text_font
     *  ilist[10] has caption flag
     *  ilist[11] has tick direction
     *  ddata[0] has major_interval
     *  ddata[1] has line_thickness
     *  ddata[2] has text_size
     *  ddata[3] has text_thickness
     */
        case GTX_FRAME_AXIS_VALUES:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d %d %d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5],
                         ilist[6],
                         ilist[7],
                         ilist[8],
                         ilist[9],
                         ilist[10],
                         ilist[11]);
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3]);
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat =
            dlist->SetFrameAxisValues (cdata,
                                       ilist[0],
                                       ilist[1],
                                       ilist[2],
                                       ilist[11],
                                       ddata[0],
                                       ilist[3],
                                       ilist[4],
                                       ilist[5],
                                       ilist[6],
                                       ilist[7],
                                       ilist[8],
                                       ddata[1],
                                       ddata[2],
                                       ddata[3],
                                       ilist[9],
                                       ilist[10]);

            break;

    /*---------------------------------------------------------
     * Reset the frame clip coordinates.
     *
     *  cdata has the frame name
     *  ilist[0] has the resize border flag
     *  ddata[0] has the new fx1
     *  ddata[1] has the new fy1
     *  ddata[2] has the new fx2
     *  ddata[3] has the new fy2
     */
        case GTX_RESETFRAME:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%d %.15e %.15e %.15e %.15e\n",
                         ilist[0],
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3]);
                fputs (LogFileLine, LogFile);
            }
          #endif

          istat = dlist->RescaleFrame (cdata,
                                       ilist[0],
                                       ddata[0],
                                       ddata[1],
                                       ddata[2],
                                       ddata[3]);

          break;

    /*---------------------------------------------------------
     * Respond to a zoom request on the frame.
     *
     *  ilist[0] has the frame number
     *  ilist[1] has the rescale border flag
     *  ilist[2] has ix1
     *  ilist[3] has iy1
     *  ilist[4] has ix2
     *  ilist[5] has iy2
     */
        case GTX_ZOOMFRAME:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5]);
                fputs (LogFileLine, LogFile);
            }
          #endif

          istat = dlist->RescaleFrame (ilist[0],
                                       ilist[1],
                                       ilist[2],
                                       ilist[3],
                                       ilist[4],
                                       ilist[5]);

          break;

    /*---------------------------------------------------------
     * Respond to a pan request on the frame.
     *
     *  ilist[0] has the frame number
     *  ilist[2] has ix1
     *  ilist[2] has iy1
     *  ilist[3] has ix2
     *  ilist[4] has iy2
     */
        case GTX_PANFRAME:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4]);
                fputs (LogFileLine, LogFile);
            }
          #endif

          istat = dlist->PanFrame (ilist[0],
                                   ilist[1],
                                   ilist[2],
                                   ilist[3],
                                   ilist[4]);

          break;

    /*---------------------------------------------------------
     * Zoom to the data extents of the frame.
     *
     *  ilist[0] has the frame number
     */
        case GTX_ZOOMEXTENTS:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

          istat = dlist->ZoomExtents (ilist[0]);

          break;

    /*---------------------------------------------------------
     * Zoom to the data extents of the frame identified by name.
     *
     *  cdata has the frame name
     */
        case GTX_FRAME_NAME_EXTENTS:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%s\n",
                         cdata
                        );
                fputs (LogFileLine, LogFile);
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = dlist->ZoomExtentsForFrameName (
                cdata,
                ddata[0],
                ddata[1],
                ddata[2],
                ddata[3]);

            break;

    /*---------------------------------------------------------
     * Zoom out the frame
     *
     *  ilist[0] has the frame number
     */
        case GTX_ZOOMOUT:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

          istat = dlist->ZoomOut (ilist[0]);

          break;

    /*---------------------------------------------------------
     * Enable a frame that was previously created.
     *
     *  cdata has the frame name
     */
        case GTX_SETFRAME:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = dlist->SetFrame (cdata);
            break;

    /*--------------------------------------------------------------
     * Set color to be used for all subsequent drawing primitives.
     *
     *  ilist[0] = red component (0-255)
     *  ilist[1] = green component (0-255)
     *  ilist[2] = blue component (0-255)
     *  ilist[3] = alpha component (0-255)
     *  ilist[4] = color type
     */
        case GTX_SETCOLOR:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = 1;
            if (ilist[4] == GTX_SET_ALL_COLORS_FLAG) {
                dlist->SetColor (ilist[0],
                                 ilist[1],
                                 ilist[2],
                                 ilist[3]
                                );
            }
            else if (ilist[4] == GTX_SET_FILL_COLOR_FLAG) {
                dlist->SetFillColor (ilist[0],
                                     ilist[1],
                                     ilist[2],
                                     ilist[3]
                                    );
            }
            else if (ilist[4] == GTX_SET_LINE_COLOR_FLAG) {
                dlist->SetLineColor (ilist[0],
                                     ilist[1],
                                     ilist[2],
                                     ilist[3]
                                    );
            }
            else if (ilist[4] == GTX_SET_PATTERN_COLOR_FLAG) {
                dlist->SetFillPatternColor (ilist[0],
                                            ilist[1],
                                            ilist[2],
                                            ilist[3]
                                           );
            }
            else if (ilist[4] == GTX_SET_SYMBOL_COLOR_FLAG) {
                dlist->SetSymbolColor (ilist[0],
                                       ilist[1],
                                       ilist[2],
                                       ilist[3]
                                      );
            }
            else if (ilist[4] == GTX_SET_TEXT_COLOR_FLAG) {
                dlist->SetTextColor (ilist[0],
                                     ilist[1],
                                     ilist[2],
                                     ilist[3]
                                    );
            }
            else if (ilist[4] == GTX_SET_TEXTFILL_COLOR_FLAG) {
                dlist->SetTextFillColor (ilist[0],
                                         ilist[1],
                                         ilist[2],
                                         ilist[3]
                                        );
            }
            else if (ilist[4] == GTX_SET_BORDER_COLOR_FLAG) {
                dlist->SetFillBorderColor (ilist[0],
                                           ilist[1],
                                           ilist[2],
                                           ilist[3]
                                          );
            }

            break;

    /*--------------------------------------------------------------
     * Set color to be used for the drawing background.
     *
     *  ilist[0] = red component (0-255)
     *  ilist[1] = green component (0-255)
     *  ilist[2] = blue component (0-255)
     *  ilist[3] = alpha component (0-255)
     */
        case GTX_SETBGCOLOR:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = 1;
            dlist->SetBackgroundColor (ilist[0],
                                       ilist[1],
                                       ilist[2],
                                       ilist[3]
                                      );
            break;

    /*--------------------------------------------------------------
     * Set color to be used for the default drawing foreground
     *
     *  ilist[0] = red component (0-255)
     *  ilist[1] = green component (0-255)
     *  ilist[2] = blue component (0-255)
     *  ilist[3] = alpha component (0-255)
     */
        case GTX_SETFGCOLOR:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = 1;
            dlist->SetForegroundColor (ilist[0],
                                       ilist[1],
                                       ilist[2],
                                       ilist[3]
                                      );
            break;


    /*--------------------------------------------------------------
     * Set the frame clipping flag.
     *
     *  ilist[0] has the frame clip flag
     */
        case GTX_SETFRAMECLIP:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

          dlist->SetFrameClip (ilist[0]);
          break;


    /*--------------------------------------------------------------
     * Draw a line using the current line graphic attributes.
     * Points are in page or frame units depending on if a
     * frame is active.
     *
     *  ilist[0] has the number of points
     *  ddata[0] to ddata[npts-1] has line x coordinates
     *  ddata[npts] to ddata[2*npts-1] has y coordinates
     */
        case GTX_DRAWLINE:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
                for (i=0; i<ilist[0]; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e\n",
                             ddata[i],
                             ddata[ilist[0]+i]);
                    fputs (LogFileLine, LogFile);
                }
            }
          #endif

            istat = dlist->AddLine (
                ddata,
                ddata+ilist[0],
                ilist[0]
            );

            break;

    /*--------------------------------------------------------------
     * Set the line endpoint arrow style.
     *
     *  ilist[0] has the arrow style flag
     */
        case GTX_ARROW_STYLE:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = 1;
            dlist->SetLineArrow (ilist[0]);
            break;

    /*--------------------------------------------------------------
     * Draw a polygon fill using the current fill graphic attributes.
     * Points are in page or frame units depending on if a
     * frame is active.
     *
     *  ilist[0] has the number of polygon components (ncomp)
     *  ilist[1] has the total number of points (npts)
     *  ilist[2] has the outline flag
     *  idata[0] to idata[ncomp-1] has the number of points for each component
     *  ddata[0] to ddata[npts - 1] has x coordinates
     *  ddata[npts] to ddata[2 * npts - 1] has y coordinates
     */
        case GTX_FILLPOLY:


          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]
                        );
                fputs (LogFileLine, LogFile);
                for (i=0; i<ilist[0]; i++) {
                    sprintf (LogFileLine,
                             "%d\n",
                             idata[i]);
                    fputs (LogFileLine, LogFile);
                }
                for (i=0; i<ilist[1]; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e\n",
                             ddata[i],
                             ddata[ilist[1] + i]);
                    fputs (LogFileLine, LogFile);
                }
            }
          #endif

            istat = dlist->AddFill (
                ddata,
                ddata+ilist[1],
                idata,
                ilist[0],
                ilist[2]
            );

            break;

    /*--------------------------------------------------------------
     * Draw a symbol using the current symbol graphic attributes.
     * Point location is in page or frame units depending on if a
     * frame is active.
     *
     *  ddata[0] has the x coordinate
     *  ddata[1] has the y coordinate
     *  ddata[2] has the size in size units
     *  ddata[3] has the angle in degrees
     *  ilist[0] has the symbol number
     */
        case GTX_DRAWSYMBOL:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %d\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3],
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = dlist->AddSymb (
                ddata[0],
                ddata[1],
                ddata[2],
                ddata[3],
                ilist[0]
            );

            break;

    /*--------------------------------------------------------------
     * Set the flag for text background drawing.
     *
     *  ilist[0] has the text background flag
     */
        case GTX_TEXTBACKGROUND:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

          dlist->SetTextBackground (ilist[0]);

          break;

    /*--------------------------------------------------------------
     * Draw a text string using the current text graphic attributes.
     * Point location is in page or frame units depending on if a
     * frame is active.
     *
     *  ddata[0] has x coordinate of lower left of text
     *  ddata[1] has y coordinate of lower left
     *  ddata[2] has text height in size units
     *  ddata[3] has the angle in degrees
     *  cdata has the actual text string
     */
        case GTX_DRAWTEXT:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3]
                        );
                fputs (LogFileLine, LogFile);
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = dlist->AddText (
                ddata[0],
                ddata[1],
                ddata[2],
                ddata[3],
                cdata
            );

            break;

    /*--------------------------------------------------------------
     * Draw a number as a text string using the current text graphic attributes.
     * Point location is in page or frame units depending on if a
     * frame is active.
     *
     *  ddata[0] has x coordinate of lower left of text
     *  ddata[1] has y coordinate of lower left
     *  ddata[2] has text height in size units
     *  ddata[3] has the angle in degrees
     *  ddata[4] is the actual value to draw
     *  ilist[0] has the number of decimal places to draw,
     *  ilist[1] has the comma flag
     */
        case GTX_DRAWNUMBER:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %d %d\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3],
                         ddata[4],
                         ilist[0],
                         ilist[1]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = dlist->AddNumber (
                ddata[0],
                ddata[1],
                ddata[2],
                ddata[3],
                ddata[4],
                ilist[0],
                ilist[1]
            );

            break;

    /*--------------------------------------------------------------
     * Draw a shape using the current shape graphic attributes.
     * Point locations are in page or frame units depending on if a
     * frame is active.
     *
     *  ilist[0] has the shape type
     *  ddata[0] to ddata[9] has the shape values
     */
        case GTX_DRAWSHAPE:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %.15e %.15e %.15e %.15e %.15e %.15e %.15e %.15e",
                         ilist[0],
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3],
                         ddata[4],
                         ddata[5],
                         ddata[6],
                         ddata[7]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat = dlist->AddShape (
                ilist[0],
                ddata
            );

            break;

    /*--------------------------------------------------------------
     * Draw an image using the current image color setup.
     * Point locations are in page or frame units depending on if a
     * frame is active.
     *
     *  ilist[0] has the image type
     *            0 for color
     *            1 for double data
     *            2 for float data
     *            3 for int data
     *            4 for short data
     *            5 for boolean (unsigned char) data
     * for all types
     *  ilist[1] has ncol
     *  ilist[2] has nrow
     *  ddata[0] has xmin
     *  ddata[1] has ymin
     *  ddata[2] has xmax
     *  ddata[3] has ymax
     I for double type
     *  ddata[4] has double null value
     *  ddata[5] to end has double data
     * for float type
     *  fdata[0] has float null value
     *  fdata[1] to end has float data
     * for int type
     *  idata[0] has int null value
     *  idata[1] to end has int data
     * for short type
     *  sdata[0] has short null value
     *  sdata[1] to end has short data
     * for boolean type
     *  bdata[0] has boolean null value
     *  bdata[1] to end has boolean data
     * for color type
     *  bdata[0] to bdata[ncol*nrow-1] has red
     *  bdata[ncol*nrow] to bdata[2*ncol*nrow-1] has green
     *  bdata[2*ncol*nrow] to bdata[3*ncol*nrow-1] has blue
     *  bdata[3*ncol*nrow] to bdata[4*ncol*nrow-1] has transparency
     */
        case GTX_DRAWIMAGE:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %.15e %.15e %.15e %.15e\n",
                          ilist[0],
                          ilist[1],
                          ilist[2],
                          ddata[0],
                          ddata[1],
                          ddata[2],
                          ddata[3]
                        );
                fputs (LogFileLine, LogFile);
                ntot = ilist[1] * ilist[2];
                if (ilist[0] == 0) {
                    for (i=0; i<ntot; i++) {
                        sprintf (LogFileLine,
                                 "%d %d %d %d\n",
                                 bdata[i],
                                 bdata[i+ntot],
                                 bdata[i+2*ntot],
                                 bdata[i+3*ntot]
                                );
                        fputs (LogFileLine, LogFile);
                    }
                }
                else if (ilist[0] == 1) {
                    for (i=4; i<ntot+5; i++) {
                        sprintf (LogFileLine,
                                 "%.15e\n",
                                 ddata[i]
                                );
                        fputs (LogFileLine, LogFile);
                    }
                }
                else if (ilist[0] == 2) {
                    for (i=0; i<ntot; i++) {
                        sprintf (LogFileLine,
                                 "%.15e\n",
                                 fdata[i]
                                );
                        fputs (LogFileLine, LogFile);
                    }
                }
                else if (ilist[0] == 3) {
                    for (i=0; i<ntot; i++) {
                        sprintf (LogFileLine,
                                 "%d\n",
                                 idata[i]
                                );
                        fputs (LogFileLine, LogFile);
                    }
                }
                else if (ilist[0] == 4) {
                    for (i=0; i<ntot; i++) {
                        sprintf (LogFileLine,
                                 "%hd\n",
                                 sdata[i]
                                );
                        fputs (LogFileLine, LogFile);
                    }
                }
                else if (ilist[0] == 5) {
                    for (i=0; i<ntot; i++) {
                        sprintf (LogFileLine,
                                 "%hd\n",
                                 bdata[i]
                                );
                        fputs (LogFileLine, LogFile);
                    }
                }
            }
          #endif

            itype = ilist[0];
            ncol = ilist[1];
            nrow = ilist[2];

            if (itype == 0) {
                istat = dlist->AddColorImage (
                    bdata,
                    bdata + ncol * nrow,
                    bdata + 2 * ncol * nrow,
                    bdata + 3 * ncol * nrow,
                    ncol,
                    nrow,
                    ddata[0],
                    ddata[1],
                    ddata[2],
                    ddata[3]
                );
            }
            else if (itype == 1) {
                istat = dlist->AddDataImage (
                    ddata+5,
                    ncol,
                    nrow,
                    ddata[5],
                    ddata[0],
                    ddata[1],
                    ddata[2],
                    ddata[3]
                );
            }
            else if (itype == 2) {
                istat = dlist->AddDataImage (
                    fdata+1,
                    ncol,
                    nrow,
                    fdata[0],
                    ddata[0],
                    ddata[1],
                    ddata[2],
                    ddata[3]
                );
            }
            else if (itype == 3) {
                istat = dlist->AddDataImage (
                    idata+1,
                    ncol,
                    nrow,
                    idata[0],
                    ddata[0],
                    ddata[1],
                    ddata[2],
                    ddata[3]
                );
            }
            else if (itype == 4) {
                istat = dlist->AddDataImage (
                    sdata+1,
                    ncol,
                    nrow,
                    sdata[0],
                    ddata[0],
                    ddata[1],
                    ddata[2],
                    ddata[3]
                );
            }
            else if (itype == 5) {
                istat = dlist->AddDataImage (
                    bdata+1,
                    ncol,
                    nrow,
                    bdata[0],
                    ddata[0],
                    ddata[1],
                    ddata[2],
                    ddata[3]
                );
            }
            else {
                istat = -1;
            }

            break;

    /*--------------------------------------------------------------
     * Set the fill pattern and size.
     *
     *  ilist[0] has the pattern number
     *  ddata[0] has the pattern scale
     */
        case GTX_SETFILLPATTERN:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %.15e\n",
                         ilist[0],
                         ddata[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetFillPattern (ilist[0],
                                   ddata[0]);
            break;

    /*--------------------------------------------------------------
     * Set the line pattern and size.
     *
     *  ilist[0] has the pattern number
     *  ddata[0] has the pattern scale
     */
        case GTX_SETLINEPATTERN:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %.15e\n",
                         ilist[0],
                         ddata[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetLinePattern (ilist[0],
                                   ddata[0]);
            break;

    /*--------------------------------------------------------------
     * Set the text font
     *
     *  ilist[0] has the font number
     */
        case GTX_SETFONT:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetTextFont (ilist[0]);
            break;

    /*--------------------------------------------------------------
     * Set the line smoothing factor.
     *
     *  ilist[0] has the smoothing factor
     */
        case GTX_SETSMOOTH:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetLineSmoothFlag (ilist[0]);
            break;

    /*--------------------------------------------------------------
     * Set the line thickness.
     *
     *  ddata[0] has the thickness
     */
        case GTX_SETLINETHICK:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e\n",
                         ddata[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetLineThickness (ddata[0]);
            break;

    /*--------------------------------------------------------------
     * Set the text outline thickness.
     *
     *  ddata[0] has the thickness
     */
        case GTX_SETTEXTTHICK:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e\n",
                         ddata[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetTextThickness (ddata[0]);
            break;

    /*--------------------------------------------------------------
     * Set the text anchor
     *
     *  ilist[0] has the anchor
     */
        case GTX_TEXTANCHOR:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetTextAnchor (ilist[0]);
            break;

    /*--------------------------------------------------------------
     * Set the text offsets
     *
     *  ddata[0] has the x offset in inches
     *  ddata[1] has the y offset in inches
     */
        case GTX_TEXTOFFSETS:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e %.15e\n",
                         ddata[0],
                         ddata[1]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetTextOffsets (ddata[0], ddata[1]);
            break;

    /*--------------------------------------------------------------
     * Set the data image color bands.
     *
     *  ilist[0] has the number of bands (n)
     *  ddata[0] to ddata[n-1] has data mins
     *  ddata[n] to ddata[2*n-1] has data mins
     *  idata[0] to idata[n-1] has red
     *  idata[n] to idata[2*n-1] has green
     *  idata[2*n] to idata[3*n-1] has blue
     *  idata[3*n] to idata[4*n-1] has alpha
     */
        case GTX_IMAGECOLORBANDS:

            n = ilist[0];

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                fputs (LogFileLine, LogFile);
                for (i=0; i<n; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %d %d %d %d\n",
                             ddata[i], ddata[n+i],
                             idata[i], idata[n+i], idata[2*n+i], idata[3*n+i]);
                    fputs (LogFileLine, LogFile);
                }
            }
          #endif

          dlist->SetImageColors (ddata, ddata+n,
                                 idata, idata+n, idata+2*n, idata+3*n, n);

          break;

    /*--------------------------------------------------------------
     * Set the image name.
     *
     *  cdata has the name
     */
        case GTX_IMAGENAME:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);
            }
          #endif

          dlist->SetImageName (cdata);

          break;

    /*--------------------------------------------------------------
     * Set the image options.
     *
     *  ilist[0] has the originflag
     *  ilist[1] has column1
     *  ilist[2] has row1
     *  ilist[3] has colspace
     *  ilist[4] has rowspace
     */
        case GTX_IMAGEOPTIONS:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4]
                        );
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetImageOptions (ilist[0],
                                    ilist[1],
                                    ilist[2],
                                    ilist[3],
                                    ilist[4]);

            break;

    /*--------------------------------------------------------------
     * Set the item name
     *
     *  cdata has the item name
     */
        case GTX_SETITEM:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetItemName (cdata);
            break;

    /*--------------------------------------------------------------
     * Set the layer name
     *
     *  cdata has the layer name
     */
        case GTX_SETLAYER:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetLayerName (cdata);
            break;

    /*--------------------------------------------------------------
     * Unset the layer, replacing it with nothing.
     */
        case GTX_UNSETLAYER:

            dlist->UnsetLayer ();
            break;

    /*--------------------------------------------------------------
     * Unset the item, replacing it with nothing.
     */
        case GTX_UNSETITEM:

            dlist->UnsetItem ();
            break;

    /*--------------------------------------------------------------
     * Unset the frame, replacing it with nothing.
     */
        case GTX_UNSETFRAME:

            dlist->UnsetFrame ();
            break;

    /*-------------------------------------------------------------
     * Set the contour properties.  The idata and ddata
     * arrays are parsed in the display list object for
     * contour properties.  The boolean values are converted to
     * integer before anything else is done.
     */
        case GTX_CONTOUR_PROPERTIES:

          idata[0] = (int)bdata[0];
          idata[1] = (int)bdata[1];
          idata[2] = (int)bdata[2];
          idata[3] = (int)bdata[3];
          idata[4] = (int)bdata[4];
          idata[5] = (int)bdata[5];
          idata[36] = (int)bdata[36];
          idata[37] = (int)bdata[37];
          idata[38] = (int)bdata[38];
          idata[39] = (int)bdata[39];

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                for (i=0; i<45; i++) {
                    sprintf (LogFileLine, "%d\n", idata[i]);
                    fputs (LogFileLine, LogFile);
                }
                for (i=0; i<23; i++) {
                    sprintf (LogFileLine, "%.15e\n", ddata[i]);
                    fputs (LogFileLine, LogFile);
                }
            }
          #endif

            dlist->SetTmpContourProperties (idata, ddata);

            break;

    /*--------------------------------------------------------------
     * Set faults for subsequent grids and trimeshs.
     */
        case GTX_FAULT_LINE_DATA:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d\n",
                         ilist[0],
                         ilist[1]);
                fputs (LogFileLine, LogFile);
                for (i=0; i<ilist[0]; i++) {
                    sprintf (LogFileLine,
                             "%d\n",
                             idata[i]);
                    fputs (LogFileLine, LogFile);
                }
                for (i=0; i<ilist[1]; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[i+ilist[1]],
                             ddata[i+2*ilist[1]]);
                    fputs (LogFileLine, LogFile);
                }
            }
          #endif

            istat =
            dlist->SetTmpFaultLines (ilist[0],
                                     ilist[1],
                                     idata,
                                     ddata,
                                     ddata+ilist[1],
                                     ddata+2*ilist[1]);

            break;

    /*--------------------------------------------------------------
     * Set the trimesh data.
     *
     *  cdata has the trimesh name
     *  ilist[0] has numnodes
     *  ilist[1] has numedges
     *  ilist[2] has numtriangles
     *  ddata[0 to numnodes-1] has xnodes
     *  ddata[numnodes to 2*numnodes-1] has ynodes
     *  ddata[2*numnodes to 3*numnodes-1] has znodes
     *  idata[0 to numnodes-1 has node flags]
     *  next numedges of idata has n1edges
     *  next numedges of idata has n2edges
     *  next numedges of idata has t1edges
     *  next numedges of idata has t2edges
     *  next numedges of idata has edgeflag
     *  next numtriangles of idata has e1tri
     *  next numtriangles of idata has e2tri
     *  next numtriangles of idata has e3tri
     *  next numtriangles of idata has triflag
     */
        case GTX_TRIMESH_DATA:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);

                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]);
                fputs (LogFileLine, LogFile);

                n = ilist[0];
                for (i=0; i<n; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e %d\n",
                             ddata[i],
                             ddata[i+n],
                             ddata[i+2*n],
                             idata[i]);
                    fputs (LogFileLine, LogFile);
                }
                nstart = n;
                n = ilist[1];
                for (i=0; i<n; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d %d\n",
                             idata[nstart + i],
                             idata[nstart + n + i],
                             idata[nstart + 2 * n + i],
                             idata[nstart + 3 * n + i],
                             idata[nstart + 4 * n + i]);
                    fputs (LogFileLine, LogFile);
                }

                nstart += 5 * n;
                n = ilist[2];
                for (i=0; i<n; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[nstart + i],
                             idata[nstart + n + i],
                             idata[nstart + 2 * n + i],
                             idata[nstart + 3 * n + i]);
                    fputs (LogFileLine, LogFile);
                }

            }
          #endif

            nstart = ilist[0] + 5 * ilist[1];

            istat =
            dlist->AddTriMesh (cdata,
                               ddata,
                               ddata+ilist[0],
                               ddata+2*ilist[0],
                               idata,
                               ilist[0],
                               idata+ilist[0],
                               idata+ilist[0]+ilist[1],
                               idata+ilist[0]+2*ilist[1],
                               idata+ilist[0]+3*ilist[1],
                               idata+ilist[0]+4*ilist[1],
                               ilist[1],
                               idata+nstart,
                               idata+nstart+ilist[2],
                               idata+nstart+2*ilist[2],
                               idata+nstart+3*ilist[2],
                               ilist[2]);

            break;

    /*--------------------------------------------------------------
     *
     * ilist[0] = nfaults
     * ilist[1] = nfault_total
     * idata[0-nfault] = points per fault
     * ddata[0-ntotal-1] = x fault coords
     * ddata[ntotal-2*ntotal-1] = y fault coords
     */
        case GTX_IMAGE_FAULT_DATA:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine, "%d %d\n",
                         ilist[0],
                         ilist[1]);
                fputs (LogFileLine, LogFile);
                for (i=0; i<ilist[0]; i++) {
                    sprintf (LogFileLine, "%d\n", idata[i]);
                    fputs (LogFileLine, LogFile);
                }
                for (i=0; i<ilist[1]*2; i++) {
                    sprintf (LogFileLine, "%.15e\n", ddata[i]);
                    fputs (LogFileLine, LogFile);
                }
            }
          #endif

            istat =
              dlist->SetImageFaultData (
                ddata, ddata+ilist[1],
                idata, ilist[0], ilist[1]);

            break;

    /*--------------------------------------------------------------
     * Set the grid geometry and data.
     *
     *  cdata has the grid name
     *  ilist[0] has ncol
     *  ilist[1] has nrow
     *  ddata[0] has xmin
     *  ddata[1] has ymin
     *  ddata[2] has width
     *  ddata[3] has height
     *  ddata[4] has angle
     *  ddata[5] has start of data
     */
        case GTX_GRID_DATA:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                strcat (LogFileLine, "\n");
                fputs (LogFileLine, LogFile);

                sprintf (LogFileLine,
                         "%d %d %.15e %.15e %.15e %.15e %.15e\n",
                         ilist[0],
                         ilist[1],
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3],
                         ddata[4]);
                fputs (LogFileLine, LogFile);

                n = ilist[0] * ilist[1];
                for (i=0; i<n; i++) {
                    sprintf (LogFileLine,
                             "%.15e\n",
                             ddata[i+5]);
                    fputs (LogFileLine, LogFile);
                }
            }
          #endif

            istat =
            dlist->AddGrid (cdata,
                            ddata+5,
                            ilist[0],
                            ilist[1],
                            ddata[0],
                            ddata[1],
                            ddata[2],
                            ddata[3],
                            ddata[4]);

            break;

    /*--------------------------------------------------------------
     * Pick the closest primitive to the cursor position in the specified
     * frame and return the results to the java side.
     *
     * ilist[0] has the frame number
     * ilist[1] has the screen x
     * ilist[2] has the screen y
     */
        case GTX_PICKPRIM:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {

                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]);
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat =
            dlist->PickFrameObject (ilist[0],
                                    ilist[1],
                                    ilist[2]);

            break;

    /*--------------------------------------------------------------
     * Pick the closest primitive to the cursor position in the specified
     * frame and return the results to the java side.
     *
     * ilist[0] has the frame number on input and the prim num on output
     * ilist[1] has the screen x
     * ilist[2] has the screen y
     */
        case GTX_GET_PRIM_NUM:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {

                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]);
                fputs (LogFileLine, LogFile);
            }
          #endif

            istat =
            dlist->GetFrameObject (ilist[0],
                                   ilist[1],
                                   ilist[2]);

            primnum = istat;

            break;


    /*--------------------------------------------------------------
     * Convert from screen coordinates to frame coordinates.
     *
     * ilist[0] has the frame number
     * ilist[1] has the screen x
     * ilist[2] has the screen y
     */
        case GTX_CONVERT_TO_FRAME:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {

                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]);
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->ConvertToFrame (ilist[0],
                                   ilist[1],
                                   ilist[2]);

            break;

    /*--------------------------------------------------------------
     * Return the primitives associated with the specified selectable
     * number to the java side.
     *
     * ilist[0] has the selectable object index
     */
        case GTX_EDITPRIM:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {

                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]);
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->ReadSelectableObject (ilist[0]);

            break;

    /*--------------------------------------------------------------
     * Set the selectable object index.
     *
     * ilist[0] has the selectable object index
     */
        case GTX_SET_SELECTABLE:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {

                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]);
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetSelectableNum (ilist[0]);

            break;

    /*--------------------------------------------------------------
     * Erase the primitives for the specified selectable object index.
     *
     * ilist[0] has the selectable object index
     */
        case GTX_ERASE_SELECTABLE:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {

                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]);
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->EraseSelectableNum (ilist[0]);

            break;

    /*--------------------------------------------------------------
     * Set the selection status for the specified selectable object index.
     *
     * ilist[0] has the selectable object index
     * ilist[1] has the selection status (0 for unselected, 1 for selected)
     */
        case GTX_SET_SELECT_STATE:

          #if _EZX_DEBUG_LOG_FILE_
            if (LogFile) {

                sprintf (LogFileLine,
                         "%d %d\n",
                         ilist[0],
                         ilist[1]);
                fputs (LogFileLine, LogFile);
            }
          #endif

            dlist->SetSelectableState(ilist[0], ilist[1]);

            break;

    /*--------------------------------------------------------------
     * For an unrecognized command, return an error
     */
        default:

            istat = -1;
            break;


    }  /* end of huge switch statement */

    if (istat >= 0  &&  native_dlist_id >= 0) {
        istat = native_dlist_id;
    }

    if (primnum >= 0) {
        ilist[0] = primnum;
    }

    return istat;

}


void *ezx_get_void_jenv (int threadid, void *v_jenv)
{
    void *vp = ThreadGuard::GetVoidJenv (threadid, v_jenv);
    return vp;
}


void *ezx_get_void_jobj (int threadid, void *v_jobj)
{
    void *vp = ThreadGuard::GetVoidJobj (threadid, v_jobj);
    return vp;
}




/*
 * End of extern "C" declaration
 */
}
