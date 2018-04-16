
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

  This file has functions used to process the SurfaceWorks commands sent
  from Java.  This is normally called from the jni function.  I have put
  this extra level in to allow for debugging of the native side only via a
  main program that calls this directly.  This isolates the jni functions
  fairly well.

 ***************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "csw/utils/private_include/csw_scope.h"

#include <csw/hlevutils/src/ThreadGuard.h>

#include <csw/jsurfaceworks/private_include/PatchSplit.h>
#include <csw/jsurfaceworks/private_include/SWCalc.h>
#include <csw/jsurfaceworks/private_include/SWCommand.h>
#include <csw/surfaceworks/private_include/Vert.h>

#define _SW_DEBUG_LOG_FILE_     1

// The DLSF macro is used to write a separate playback log file for
// each display list object.  This keeps the playback commands from
// being scrambled in concurrent thread situations.

#define DLSF if (swcalc) swcalc->OutputForPlayback(LogFileLine);
#define DLSF2 if (swcalc) swcalc->OutputForPlayback(NULL);


/*
 * Macros to clean up lines, points, etc.
 */
#define free_lines \
{ \
    csw_Free (Xline); \
    csw_Free (Yline); \
    csw_Free (Zline); \
    csw_Free (Npline); \
    csw_Free (LineTypes); \
    Xline = NULL; \
    Yline = NULL; \
    Zline = NULL; \
    Npline = NULL; \
    LineTypes = NULL; \
    Nline = 0; \
}

#define free_points \
{ \
    csw_Free (Xpoint); \
    csw_Free (Ypoint); \
    csw_Free (Zpoint); \
    Xpoint = NULL; \
    Ypoint = NULL; \
    Zpoint = NULL; \
    Npoint = 0; \
}

#define free_bounds \
{ \
    csw_Free (Xbounds); \
    csw_Free (Ybounds); \
    csw_Free (Zbounds); \
    Xbounds = NULL; \
    Ybounds = NULL; \
    Zbounds = NULL; \
    Nbounds = 0; \
}



/*
 * The functions in this file are called from the C JNI functions, so
 * they are extern C here as well as in the header file.
 */
extern "C" {


/*
 ***************************************************************************

    Process a command sent from the java side.  Any returned data
    will be sent back to java via calling java methods from C.

 ***************************************************************************
*/
long sw_process_command (
    void    *v_jenv,
    void    *v_jobj,
    int           command_id,
    long          threadid,
    int           *ilist,
    long          *llist,
    double        *dlist,
    char          *cdata,
    int           *idata,
    float         *fdata,
    double        *ddata
) {
    long          istat;
    int           iistat;
    double        *zp1, *zp2, *zp3;

    int           nedge, ntri, nnode, n0;
    int           first;

    int           nline, nline2, ntot, npts, zflag;
    int           start;

  #if _SW_DEBUG_LOG_FILE_
    int           i;
  #endif

    double    *Xline = NULL,
              *Yline = NULL,
              *Zline = NULL;
    double    *Xpoint = NULL,
              *Ypoint = NULL,
              *Zpoint = NULL;
    double    *Xbounds = NULL,
              *Ybounds = NULL,
              *Zbounds = NULL;
    int       *Npline = NULL;
    int       *LineTypes = NULL;
    int       Nline = 0;
    int       Nbounds = 0;
    int       Npoint = 0;

    SWGridGeometryStruct    *GridGeometry = NULL;
    SWGridOptionsStruct     *GridOptions = NULL;

    SWCalc        *swcalc = NULL;

    auto fscope = [&]()
    {
        DLSF2
    };
    CSWScopeGuard func_scope_guard (fscope);


    PATCHSplit    *psplit = ThreadGuard::GetPatchSplit (threadid);
    if (psplit == NULL) {
        assert (false);
    }
    psplit->SetJNIPtrs (v_jenv, v_jobj);

    GRDVert       *gvert = ThreadGuard::GetGrdVert (threadid);
    if (gvert == NULL) {

        long  istat = ThreadGuard::CreateGrdVert (threadid);
        if (istat == -1) {
            assert (false);
        }
        gvert = ThreadGuard::GetGrdVert (threadid);
        if (gvert == NULL) {
            assert (false);
        }
    }

    CSWGrdAPI     *grdapi = ThreadGuard::GetGrdAPI (threadid);
    if (grdapi == NULL) {

        long  istat = ThreadGuard::CreateGrdAPI (threadid);
        if (istat == -1) {
            assert (false);
        }
        grdapi = ThreadGuard::GetGrdAPI (threadid);
        if (grdapi == NULL) {
            assert (false);
        }
    }

    swcalc = ThreadGuard::GetSWCalc (threadid);
    if (swcalc == NULL) {
         assert (false);
    }
    swcalc->SetJNIPtrs (v_jenv, v_jobj);

    psplit->SetGrdAPIPtr (grdapi);
    swcalc->SetGrdAPIPtr (grdapi);

    fdata = fdata;
    cdata = cdata;
    dlist = dlist;

    first = 1;
    first = first;
    start = 0;
    start = start;

  #if _SW_DEBUG_LOG_FILE_
    bool LogFile = true;
    char LogFileLine[1000];
  #else
    if (command_id == SW_OPEN_LOG_FILE) {
        return 1;
    }
    if (command_id == SW_PAUSE_LOG_FILE) {
        return 1;
    }
    if (command_id == SW_RESTART_LOG_FILE) {
        return 1;
    }
  #endif

  #if _SW_DEBUG_LOG_FILE_
    if (LogFile) {
        sprintf (LogFileLine, "command=%d\n", command_id);
        DLSF
    }
  #endif

/*
 * Process an end thread command outside of the switch statement.
    if (command_id == SW_END_THREAD) {
        ThreadGuard::RemoveThreadData (threadid);
    }
 */

/*
 * This giant switch statement processes each command.
 */
    istat = -1;
    switch (command_id) {

    /*--------------------------------------------------------------
     * Set lines to be used in the next trimesh or grid calculation.
     *
     * ilist[0] has the number of lines
     * ilist[1] has the total number of points
     * ilist[2] has the z flag
     * idata[0 to nlines-1] has the points per line
     * idata[nlines to 2*nlines-1] has the line types
     * ddata[0 to ntot-1] has x coordinates for all lines
     * ddata[ntot to 2*ntot-1] has y coordinates for all lines
     * ddata[2*ntot to 3*ntot-1] has z coordinates for all lines
     */
        case SW_SET_LINES:

          printf ("in SW_SET_LINES section of command processer\n");

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nline; i++) {
                    sprintf (LogFileLine,
                             "%d %d\n",
                             idata[i],
                             idata[nline+i]
                            );
                    DLSF
                }
                npts = ilist[1];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            free_lines;

            nline = ilist[0];
            ntot = ilist[1];
            zflag = ilist[2];
            if (nline < 1  ||  ntot < 2) {
                printf ("nline or ntot bad\n");
                break;
            }

            Nline = nline;

            Xline = (double *)csw_Malloc (ntot * sizeof(double));
            Yline = (double *)csw_Malloc (ntot * sizeof(double));
            if (zflag) {
                Zline = (double *)csw_Malloc (ntot * sizeof(double));
            }
            Npline = (int *)csw_Malloc (nline * sizeof(int));
            LineTypes = (int *)csw_Malloc (nline * sizeof(int));
            if (Xline == NULL  ||
                Yline == NULL  ||
                Zline == NULL  ||
                LineTypes == NULL  ||
                Npline == NULL) {
                free_lines;
                printf ("bad csw_Malloc nline = %d\n", nline);
                break;
            }

            memcpy (Xline, ddata, ntot * sizeof(double));
            memcpy (Yline, ddata+ntot, ntot * sizeof(double));
            if (zflag) {
                memcpy (Zline, ddata+2*ntot, ntot * sizeof(double));
            }
            memcpy (Npline, idata, nline*sizeof(int));
            memcpy (LineTypes, idata+nline, nline * sizeof(int));

            istat = 1;

            break;

    /*--------------------------------------------------------------
     * Set boundary to be used in the next trimesh or grid calculation.
     *
     *  ilist[0] has npts
     *  ilist[1] has zflag
     *  ddata[0-npts-1] has x coordinates
     *  ddata[npts to 2*npts-1] has y coordinates
     *  ddata[2*npts to 3 * npts-1] has z coordinates
     */
        case SW_SET_BOUNDS:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d\n",
                         ilist[0],
                         ilist[1]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            free_bounds;

            npts = ilist[0];
            zflag = ilist[1];

            if (npts < 2) {
                break;
            }

            Xbounds = (double *)csw_Malloc (npts * sizeof(double));
            Ybounds = (double *)csw_Malloc (npts * sizeof(double));
            if (zflag) {
                Zbounds = (double *)csw_Malloc (npts * sizeof(double));
            }
            if (Xbounds == NULL  ||
                Ybounds == NULL  ||
                Zbounds == NULL) {
                free_bounds;
                break;
            }

            Nbounds = npts;
            memcpy (Xbounds, ddata, npts * sizeof(double));
            memcpy (Ybounds, ddata, npts * sizeof(double));
            if (zflag) {
                memcpy (Zbounds, ddata, npts * sizeof(double));
            }

            istat = 1;

            break;

    /*--------------------------------------------------------------
     * Set the model bounding box to use for subsequent trimesh
     * calculations.
     *
     *  ddata[0] is xmin
     *  ddata[1] is ymin
     *  ddata[2] is zmin
     *  ddata[3] is xmax
     *  ddata[4] is ymax
     *  ddata[5] is zmax
     */
        case SW_MODEL_BOUNDS:


          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %.15e\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3],
                         ddata[4],
                         ddata[5]);
                DLSF
            }
          #endif

            swcalc->sw_SetModelBounds (
                ddata[0], ddata[1], ddata[2],
                ddata[3], ddata[4], ddata[5]);
            istat = (long)1;

            break;

    /*--------------------------------------------------------------
     * Set the model bounding box to use for subsequent trimesh
     * calculations.
     *
     *  ddata[0] is xmin
     *  ddata[1] is ymin
     *  ddata[2] is zmin
     *  ddata[3] is xmax
     *  ddata[4] is ymax
     *  ddata[5] is zmax
     */
        case SW_PS_MODEL_BOUNDS:


          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %.15e\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3],
                         ddata[4],
                         ddata[5]);
                DLSF
            }
          #endif

            psplit->ps_SetModelBounds (
                ddata[0], ddata[1], ddata[2],
                ddata[3], ddata[4], ddata[5]);
            istat = (long)1;

            break;

    /*--------------------------------------------------------------
     * Calculate a trimesh using the most recent lines and bounds data
     * as well as the points specified now.
     *
     *  ilist[0] has number of boundary points (npts)
     *  dlist[0] has the constant z value
     *  ddata[0 to npts-1] has the x coordinates
     *  ddata[npts to 2*npts-1] has the y coordinates
     */
        case SW_CALC_CONSTANT_TRI_MESH:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %f\n",
                         ilist[0],
                         dlist[0]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i]
                            );
                    DLSF
                }
            }
          #endif

            npts = ilist[0];
            istat = (long) swcalc->sw_CalcConstantTriMesh (
                ddata,
                ddata+npts,
                npts,
                dlist[0]
            );

            break;

    /*--------------------------------------------------------------
     * Calculate a trimesh using the most recent lines and bounds data
     * as well as the points specified now.
     *
     *  ilist[0] has npts
     *  ilist[1] has gflag
     *  ilist[2] has the fault surface flag
     *  ilist[3] has the bad bounds flag
     *  ddata[0 to npts-1] has the x coordinates
     *  ddata[npts to 2*npts-1] has the y coordinates
     *  ddata[2*npts to 3*npts-1] has the z coordinates
     */
        case SW_CALC_TRI_MESH:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            npts = ilist[0];
            istat = (long) swcalc->sw_CalcTriMesh (
                ddata,
                ddata+npts,
                ddata+npts*2,
                npts,
                Xline,
                Yline,
                Zline,
                Npline,
                LineTypes,
                Nline,
                Xbounds,
                Ybounds,
                Zbounds,
                Nbounds,
                ilist[1],
                GridGeometry,
                GridOptions,
                ilist[2],
                ilist[3]
            );

            free_lines;
            free_bounds;
            csw_Free (GridGeometry);
            csw_Free (GridOptions);
            GridOptions = NULL;
            GridGeometry = NULL;

            break;

    /*--------------------------------------------------------------
     * Calculate a trimesh that exactly honors the most recent lines
     * set and uses the points specified here as well.
     *
     *  ilist[0] has npts
     *  ilist[1] has gflag
     *  ilist[2] has the fault surface flag
     *  ilist[3] has the bad bounds flag
     *  ddata[0 to npts-1] has the x coordinates
     *  ddata[npts to 2*npts-1] has the y coordinates
     *  ddata[2*npts to 3*npts-1] has the z coordinates
     */
        case SW_CALC_EXACT_TRIMESH:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            npts = ilist[0];
            istat = (long) swcalc->sw_CalcExactTriMesh (
                ddata,
                ddata+npts,
                ddata+npts*2,
                npts,
                Xline,
                Yline,
                Zline,
                Npline,
                LineTypes,
                Nline,
                Xbounds,
                Ybounds,
                Zbounds,
                Nbounds,
                ilist[1],
                GridGeometry,
                GridOptions,
                ilist[2],
                ilist[3]
            );

            free_lines;
            free_bounds;
            csw_Free (GridGeometry);
            csw_Free (GridOptions);
            GridOptions = NULL;
            GridGeometry = NULL;

            break;


    /*--------------------------------------------------------------
     * Set the grid calculation options to use with the next
     * calc grid or calc trimesh command.
     *
     * ilist[0] has preferredStrike
     * ilist[1] has outsideMargin
     * ilist[2] has insideMargin
     * ilist[3] has fastFlag
     * ilist[4] has anisotropyFlag
     * ilist[5] has distancePower
     * ilist[6] has strikePower
     * ilist[7] has numLocalPoints
     * ilist[8] has localSearchPattern
     * ilist[9] has directionalFlag  (?)
     * ilist[10] has thicknessFlag
     * ilist[11] has stepFlag
     *
     * ddata[0] has minValue
     * ddata[1] has maxValue
     * ddata[2] has hardMin
     * ddata[3] has hardMax
     * ddata[4] has maxSearchDistance
     * ddata[5] has logBase
     */
        case SW_GRID_OPTIONS:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
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
                         ilist[11]
                        );
                DLSF
                sprintf (LogFileLine,
                         "%f %f %f %f %f %f\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3],
                         ddata[4],
                         ddata[5]
                        );
                DLSF
            }
          #endif

            if (GridOptions == NULL) {
                GridOptions = (SWGridOptionsStruct *)
                              csw_Malloc (sizeof(SWGridOptionsStruct));
            }
            if (GridOptions == NULL) {
                break;
            }

            GridOptions->preferredStrike = ilist[0];
            GridOptions->outsideMargin = ilist[1];
            GridOptions->insideMargin = ilist[2];
            GridOptions->fastFlag = ilist[3];
            GridOptions->anisotropyFlag = ilist[4];
            GridOptions->distancePower = ilist[5];
            GridOptions->strikePower = ilist[6];
            GridOptions->numLocalPoints = ilist[7];
            GridOptions->localSearchPattern = ilist[8];
            GridOptions->thicknessFlag = ilist[10];
            GridOptions->stepFlag =  ilist[11];
            GridOptions->removeCrossingConstraintFlag = ilist[12];

            GridOptions->minValue = ddata[0];
            GridOptions->maxValue = ddata[1];
            GridOptions->hardMin = ddata[2];
            GridOptions->hardMax = ddata[3];
            GridOptions->maxSearchDistance = ddata[4];
            GridOptions->logBase = ddata[5];

            break;

    /*--------------------------------------------------------------
     * Set the grid calculation options to use with the next
     * calc grid or calc trimesh command.
     */

        case SW_GRID_GEOMETRY:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %f %f %f %f\n",
                         ilist[0],
                         ilist[1],
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3]
                        );
                DLSF
            }
          #endif

            if (GridGeometry == NULL) {
                GridGeometry = (SWGridGeometryStruct *)
                               csw_Malloc (sizeof(SWGridGeometryStruct));
            }
            if (GridGeometry == NULL) {
                break;
            }

            GridGeometry->ncol = ilist[0];
            GridGeometry->nrow = ilist[1];
            GridGeometry->xmin = ddata[0];
            GridGeometry->ymin = ddata[1];
            GridGeometry->xmax = ddata[2];
            GridGeometry->ymax = ddata[3];

            break;

    /*--------------------------------------------------------------
     * Calculate a grid using the most recent lines and bounds data
     * as well as the points specified now.
     *
     *  ilist[0] has npts
     *  ilist[1] has smooth factor
     *  ddata[0 to npts-1] has the x coordinates
     *  ddata[npts to 2*npts-1] has the y coordinates
     *  ddata[2*npts to 3*npts-1] has the z coordinates
     */
        case SW_CALC_GRID:

          #if _SW_DEBUG_LOG_FILE_

            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d\n",
                         ilist[0],
                         ilist[1]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) swcalc->sw_CalcGrid (
                ilist[1],
                ddata,
                ddata+npts,
                ddata+npts*2,
                npts,
                Xline,
                Yline,
                Zline,
                Npline,
                LineTypes,
                Nline,
                Xbounds,
                Ybounds,
                Nbounds,
                GridGeometry,
                GridOptions
            );

            free_lines;
            free_bounds;
            csw_Free (GridGeometry);
            csw_Free (GridOptions);
            GridOptions = NULL;
            GridGeometry = NULL;

            break;

    /*--------------------------------------------------------------
     * Set lines to be used in the next draped line calculation.
     *
     * ilist[0] has the number of lines
     * ilist[1] has the total number of points
     * ilist[2] has the z flag
     * idata[0 to nlines-1] has the points per line
     * idata[nlines to 2*nlines-1] has the line types
     * ddata[0 to ntot-1] has x coordinates for all lines
     * ddata[ntot to 2*ntot-1] has y coordinates for all lines
     * ddata[2*ntot to 3*ntot-1] has z coordinates for all lines
     */
        case SW_SET_DRAPE_LINES:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nline; i++) {
                    sprintf (LogFileLine,
                             "%d %d\n",
                             idata[i],
                             idata[nline+i]
                            );
                    DLSF
                }
                npts = ilist[1];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            Nline = 0;

            nline = ilist[0];
            ntot = ilist[1];
            if (nline < 1  ||  ntot < 2) {
                break;
            }

            free_lines;

            Nline = nline;

            Xline = (double *)csw_Malloc (ntot * sizeof(double));
            Yline = (double *)csw_Malloc (ntot * sizeof(double));
            Zline = (double *)csw_Malloc (ntot * sizeof(double));
            Npline = (int *)csw_Malloc (nline * sizeof(int));
            if (Xline == NULL  ||
                Yline == NULL  ||
                Zline == NULL  ||
                Npline == NULL) {
                free_lines;
                break;
            }

            memcpy (Xline, ddata, ntot * sizeof(double));
            memcpy (Yline, ddata+ntot, ntot * sizeof(double));
            memcpy (Zline, ddata+2*ntot, ntot * sizeof(double));
            memcpy (Npline, idata, nline*sizeof(int));

            swcalc->sw_SetDrapeLineCache (Xline, Yline, Zline, Npline, Nline);

            free_lines;

            Xline = NULL;
            Yline = NULL;
            Zline = NULL;
            Npline = NULL;
            Nline = 0;

            istat = 1;

            break;

    /*--------------------------------------------------------------
     * Set points to be used in the next draped point calculation.
     *
     * ilist[1] has the total number of points
     * ddata[0 to ntot-1] has x coordinates for all lines
     * ddata[ntot to 2*ntot-1] has y coordinates for all lines
     * ddata[2*ntot to 3*ntot-1] has z coordinates for all lines
     */
        case SW_SET_DRAPE_POINTS:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[1]
                        );
                DLSF
                npts = ilist[1];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            Npoint = ilist[1];
            Xpoint = (double *)csw_Malloc (Npoint * sizeof(double));
            Ypoint = (double *)csw_Malloc (Npoint * sizeof(double));
            Zpoint = (double *)csw_Malloc (Npoint * sizeof(double));
            if (Xpoint == NULL  ||
                Ypoint == NULL  ||
                Zpoint == NULL) {
                istat = -1;
                break;
            }

            memcpy (Xpoint, ddata, Npoint * sizeof(double));
            memcpy (Ypoint, ddata+Npoint, Npoint * sizeof(double));
            memcpy (Zpoint, ddata+2*Npoint, Npoint * sizeof(double));

            swcalc->sw_SetDrapePointCache (Xpoint, Ypoint, Zpoint, Npoint);

            free_points;

            istat = 1;

            break;

    /*-------------------------------------------------------------
     * Calculate the trimesh outline.
     */
        case SW_TRIMESH_OUTLINE:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];

            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) swcalc->sw_CalcTriMeshOutline (
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;


    /*-------------------------------------------------------------
     * Setup the trimesh cache for draped lines.
     */
        case SW_TRIMESH_DATA:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];

            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) swcalc->sw_SetDrapeTriMeshCache (
                ilist[3],
                ilist[4],
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;

    /*-------------------------------------------------------------
     * Set the sediment surface for the model sealing.  This does
     * not need to be split by fault centerlines.
     *
     * ilist[4] has the horizon id
     * dlist[6] has the age
     */
        case SW_PS_SET_SED_SURFACE:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];

            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d %f\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5],
                         dlist[6]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) psplit->ps_SetSedimentSurface (
                ilist[4],
                dlist[6],
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;

    /*-------------------------------------------------------------
     * Set the model bottom for the model sealing.  This does
     * not need to be split by fault centerlines.
     *
     * ilist[4] has the horizon id
     * dlist[6] has the age
     */
        case SW_PS_SET_MODEL_BOTTOM:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];

            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d %f\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5],
                         dlist[6]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) psplit->ps_SetModelBottom (
                ilist[4],
                dlist[6],
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;

    /*-------------------------------------------------------------
     * Add a horizon to the model sealing.  The horizon does
     * not need to be split by fault centerlines.
     *
     * ilist[4] has the horizon id
     */
        case SW_PS_ADD_HORIZON_PATCH:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];

            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d %f\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5],
                         dlist[6]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) psplit->ps_AddHorizonTriMeshPatch (
                ilist[4],
                dlist[6],
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;

    /*-------------------------------------------------------------
     * Start the definition of faults and horizons for a sealed model.
     */
        case SW_PS_START_SEALED_MODEL_DEF:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]);
                DLSF
            }
          #endif

            istat = (long) psplit->ps_StartSealedModelDefinition (ilist[0]);

            break;

    /*-------------------------------------------------------------
     * Calculate sealed surfaces for the input faults and horizons
     * currently defined in the sealed model.
     */
        case SW_PS_CALC_SEALED_MODEL:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e\n",
                         dlist[0]);
                DLSF
            }
          #endif

            istat = (long) psplit->ps_CalcSealedModel (dlist[0]);

            break;

    /*-------------------------------------------------------------
     * Return the input patches for the sealed model.  This is used to
     * get the split pieces of patches.
     */
        case SW_PS_GET_SEALED_INPUT:

            istat = (long) psplit->ps_GetSealedInput ();

            break;

    /*-------------------------------------------------------------
     * Free all data cached for draped line calculations.
     */
        case SW_CLEAR_DRAPE_CACHE:

            swcalc->sw_ClearDrapeCache ();
            istat = 1;
            break;

    /*-------------------------------------------------------------
     * Calculate the draped points for the current cached trimesh and
     * input line data.
     */
        case SW_CALC_DRAPED_POINTS:
            istat = (long) swcalc->sw_CalcDrapedPoints ();
            break;

    /*-------------------------------------------------------------
     * Calculate the draped lines for the current cached trimesh and
     * input line data.
     */
        case SW_CALC_DRAPED_LINES:
            istat = (long) swcalc->sw_CalcDrapedLines ();
            break;

    /*--------------------------------------------------------------
     * Set the patch split debug state.
     *
     *  ilist[0] is the debug state.
     */
        case SW_PS_SET_TEST_STATE:


          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                DLSF
            }
          #endif

            psplit->ps_SetDebugState (ilist[0]);
            istat = 1;
            break;

    /*--------------------------------------------------------------
     * Clear the patch split data.
     */
        case SW_PS_CLEAR_ALL_DATA:

            psplit->ps_ClearAllData ();
            istat = 1;
            break;

    /*--------------------------------------------------------------
     * Clear the surfaces and centerlines, leaving the faults in place.
     */
        case SW_PS_CLEAR_HORIZON_DATA:

            psplit->ps_ClearHorizonData ();
            istat = 1;
            break;

    /*--------------------------------------------------------------
     * Set the line trim fraction for patch splitting.
     *  ddata[0] has the fraction
     */
        case SW_PS_LINE_TRIM_FRACTION:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%f\n",
                         ddata[0]
                        );
                DLSF
            }
          #endif

            psplit->ps_SetLineTrimFraction (ddata[0]);
            istat = 1;
            break;

    /*--------------------------------------------------------------
     * Intersect input faults and lines currently in the sealed model
     * object and return the intersection lines.
     */
        case SW_PS_CALC_LINES_FOR_SPLITTING:

            istat = (long) psplit->ps_CalcLinesForSplitting ();
            break;

    /*--------------------------------------------------------------
     * Calculate the patch split and return the results.
     *
     *  ilist[0] is the clearData flag
     *  ilist[1] is the id to attach to each split part.
     *  dlist[0] is the age for all split results
     *  dlist[1] is the average edge length
     */
        case SW_PS_CALC_SPLIT:


          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %f %f\n",
                         ilist[0],
                         ilist[1],
                         dlist[0],
                         dlist[1]
                        );
                DLSF
            }
          #endif

            psplit->ps_SetAverageEdgeLength (dlist[1]);

            istat = (long) psplit->ps_CalcSplit (ilist[1], dlist[0]);

            if (ilist[0] == 1) {
                psplit->ps_ClearHorizonData ();
            }

            break;

    /*-------------------------------------------------------------
     * Add a border segment for the patch split.
     *
     *  ilist[0] is npts
     *  ilist[1] is type
     *  ilist[2] is flag
     *  ilist[3] is the zflag
     *  ddata[0] is start of x
     *  ddata[npts] is start of y
     *  ddata[2*npts] is start of z
     */
        case SW_PS_ADD_BORDER_SEGMENT:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            zp1 = ddata;
            zp2 = ddata + ilist[0];
            zp3 = ddata + 2 * ilist[0];
            istat = (long)
            psplit->ps_AddBorderSegment (
                zp1,
                zp2,
                zp3,
                ilist[0],
                ilist[1],
                ilist[2]
            );

            break;

    /*-----------------------------------------------------------------------
     * Set xyz points for the patch to be split.
     *  ilist[0] is npts
     *  ilist[1] is meshflag
     *  ddata[0] starts x coordinates
     *  ddata[npts] starts y coordinates
     *  ddata[2*npts] starts z coordinates
     */
        case SW_PS_SET_POINTS:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d\n",
                         ilist[0],
                         ilist[1]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long)
            psplit->ps_SetPoints (
                ddata,
                ddata+ilist[0],
                ddata+2*ilist[0],
                ilist[0],
                ilist[1]);

            break;

    /*-----------------------------------------------------------------------
     * Add a fault surface to the patch split.
     *
     *  ilist[0] is num_nodes
     *  ilist[1] is num_edges
     *  ilist[2] is num_triangles
     *  ilist[3] is vused
     *  ilist[4] is fault_id
     *  ilist[5] is flag
     *  ilist[6] is nline
     *  ilist[7] is detachment id
     *  dlist[0-5] has 6 vbase values
     *  dlist[6] has minimum age touched
     *  dlist[7] has maximum age touched
     *  ddata[0] starts xnodes
     *  ddata[num_nodes] starts ynodes
     *  ddata[2*num_nodes] starts znodes
     *  ddata[3*num_nodes] starts xline
     *  ddata[3*num_nodes+nline] starts yline
     *  ddata[3*num_nodes+2*nline] starts zline
     *  idata[0] starts n1edge
     *  idata[num_edges] starts n2edge
     *  idata[2*num_edges] starts t1edge
     *  idata[3*num_edges] starts t2edge
     *  idata[4*num_edges] starts e1tri
     *  idata[4*num_edges+num_triangles] starts e2tri
     *  idata[4*num_edges+2*num_triangles] starts e3tri
     */
        case SW_PS_ADD_FAULT_SURFACE_WITH_DETACHMENT_CONTACT:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];
            nline2 = ilist[6];

            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5],
                         ilist[6],
                         ilist[7]
                        );
                DLSF
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %.15e %.15e %.15e\n",
                         dlist[0],
                         dlist[1],
                         dlist[2],
                         dlist[3],
                         dlist[4],
                         dlist[5],
                         dlist[6],
                         dlist[7]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
                n0 = nnode * 3;
                for (i=0; i<nline2; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                              ddata[n0],
                              ddata[n0+nline2+i],
                              ddata[n0+2*nline2+i]
                            );
                    DLSF
                }
            }
          #endif

            n0 = nnode * 3;
            istat = (long)
            psplit->ps_AddFaultSurfaceWithDetachmentContact (
                ilist[4],
                ilist[3],
                dlist,
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri,
                dlist[6],
                dlist[7],
                ddata+n0,
                ddata+n0+nline2,
                ddata+n0+2*nline2,
                nline2,
                ilist[7]);

            break;

    /*-----------------------------------------------------------------------
     * Add a fault surface to the patch split.
     *
     *  ilist[0] is num_nodes
     *  ilist[1] is num_edges
     *  ilist[2] is num_triangles
     *  ilist[3] is vused
     *  ilist[4] is fault_id
     *  ilist[5] is flag
     *  dlist[0-5] has 6 vbase values
     *  dlist[6] has minimum age touched
     *  dlist[7] has maximum age touched
     *  ddata[0] starts xnodes
     *  ddata[num_nodes] starts ynodes
     *  ddata[2*num_nodes] starts znodes
     *  idata[0] starts n1edge
     *  idata[num_edges] starts n2edge
     *  idata[2*num_edges] starts t1edge
     *  idata[3*num_edges] starts t2edge
     *  idata[4*num_edges] starts e1tri
     *  idata[4*num_edges+num_triangles] starts e2tri
     *  idata[4*num_edges+2*num_triangles] starts e3tri
     */
        case SW_PS_ADD_FAULT_SURFACE:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];

            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5]
                        );
                DLSF
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %.15e %.15e %.15e\n",
                         dlist[0],
                         dlist[1],
                         dlist[2],
                         dlist[3],
                         dlist[4],
                         dlist[5],
                         dlist[6],
                         dlist[7]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long)
            psplit->ps_AddFaultSurface (
                ilist[4],
                ilist[3],
                dlist,
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri,
                dlist[6],
                dlist[7]);

            break;

    /*-----------------------------------------------------------------------
     * Add a fault centerline to the patch split.
     *
     *  ilist[0] is npts
     *  ilist[1] is fault_id
     *  ilist[2] is flag
     *  ddata[0] is start of x
     *  ddata[npts] is start of y
     *  ddata[2*npts] is start of z
     */
        case SW_PS_ADD_CENTERLINE:


          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            zp1 = ddata;
            zp2 = ddata + ilist[0];
            zp3 = ddata + 2 * ilist[0];

            istat = (long)
            psplit->ps_AddFaultCenterline (
                zp1,
                zp2,
                zp3,
                ilist[0],
                ilist[1],
                ilist[2]
            );

            break;

    /*-----------------------------------------------------------------------
     * Check if a set of points is best fit by using "steep surface coordinates"
     * The baseline for the new coordinate system is returned back to the Java
     * object.
     *
     *  ilist[0] has npts
     *  ilist[1] has fault_flag
     *  ddata[0] starts xpts
     *  ddata[npts] starts ypts
     *  ddata[2*npts] starts zpts
     */
        case SW_VERT_CALC_BASELINE:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d\n",
                         ilist[0],
                         ilist[1]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long)
            vert_CalcBaselineFromJava (
                v_jenv,
                v_jobj,
                ddata,
                ddata+ilist[0],
                ddata+2*ilist[0],
                ilist[0],
                ilist[1]
            );

            break;

    /*----------------------------------------------------------------------------------
     * Set the baseline to use for steep surface coordinates
     *
     *  ilist[0] has the use flag
     *  ddata[0] has x1
     *  ddata[1] has y1
     *  ddata[2] has x2
     *  ddata[3] has y2
     *  ddata[4] has zscale
     *  ddata[5] has zmin
     */
        case SW_VERT_SET_BASELINE:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                DLSF
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %.15e\n",
                         ddata[0],
                         ddata[1],
                         ddata[2],
                         ddata[3],
                         ddata[4],
                         ddata[5]
                        );
                DLSF
            }
          #endif

            vert_SetBaseline (
                ddata[0],
                ddata[1],
                ddata[2],
                ddata[3],
                ddata[4],
                ddata[5],
                ilist[0],
                *gvert
            );

            istat = 1;

            break;

    /*--------------------------------------------------------------
     *  Convert a node triangle based trimesh to a node edge triangle
     *  based trimesh.
     *
     *  ilist[0] has num_nodes
     *  ilist[1] has num_triangles
     *  ddata starts xnodes
     *  ddata + num_nodes starts ynodes
     *  ddata + 2 * num_nodes starts znodes
     *  idata starts n1tri
     *  idata + num_triangles starts n2tri
     *  idata + 2 * num_triangles starts n3tri
     */
        case SW_CONVERT_NODE_TRIMESH:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d\n",
                         ilist[0],
                         ilist[1]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
                npts = ilist[1];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[i],
                             idata[npts+i],
                             idata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long)
            swcalc->sw_ConvertNodeTrimesh (
                ddata,
                ddata+ilist[0],
                ddata+2*ilist[0],
                ilist[0],
                idata,
                idata+ilist[1],
                idata+2*ilist[1],
                ilist[1]
            );

            break;

    /*-------------------------------------------------------------------
     *  Write a trimesh to the specified file in the specified format.
     *
     *  cdata has the output file name
     *  ilist[0] has num nodes
     *  ilist[1] has num edges
     *  ilist[2] has num tris
     *  ilist[3] has format flag
     *  ilist[4] has the vertical baseline used flag
     *  dlist[0] has the vertical baseline x1
     *  dlist[1] has the vertical baseline y1
     *  dlist[2] has the vertical baseline x2
     *  dlist[3] has the vertical baseline y2
     *  dlist[4] has the vertical baseline zscale
     *  dlist[5] has the vertical baseline zmin
     *  ddata starts xnodes
     *  ddata + num nodes starts ynodes
     *  ddata + 2 * num node starts znodes
     *  idata starts n1edge
     *  idata + nedge starts n2edge
     *  idata + 2*nedge starts t1edge
     *  idata + 3*nedge starts t2edge
     *  idata + 4*nedge starts e1tri
     *  idata + 4*nedge + ntri starts e2tri
     *  idata + 4*nedge + 2*ntri starts e3tri
     */
        case SW_WRITE_TRIMESH_DATA:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];

            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                DLSF
                sprintf (LogFileLine, "\n");
                DLSF
                sprintf (LogFileLine,
                         "%d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4]
                        );
                DLSF
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %.15e\n",
                         dlist[0],
                         dlist[1],
                         dlist[2],
                         dlist[3],
                         dlist[4],
                         dlist[5]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = swcalc->sw_AppendTriMesh (
                cdata,
                ilist[3],
                ilist[4],
                dlist,
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;

    /*------------------------------------------------------------------------
     * Read a trimesh from the specified file and send it back to the java side.
     *
     * cdata has the file name
     */
        case SW_READ_TRIMESH_DATA:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                DLSF
                sprintf (LogFileLine, "\n");
                DLSF
                sprintf (LogFileLine, "%li", llist[0]);
                DLSF
            }
          #endif

            istat = (long) swcalc->sw_ReadTriMeshFromMultiFile (cdata, llist[0]);

            break;

    /*---------------------------------------------------------------------------
     * Read a csw grid file and return the grid to the java side.
     *
     * cdata has the file name
     */
        case SW_READ_CSW_GRID:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                strcpy (LogFileLine, cdata);
                DLSF
                strcpy (LogFileLine, "\n");
                DLSF
            }
          #endif

            istat = (long) swcalc->sw_ReadGrid (cdata);

            break;

    /*---------------------------------------------------------------------------
     * Extend a fault in the nominal strike and dip direction and send the
     * resulting trimesh back to the java side.
     *
     *  ilist[0] has npts
     *  dlist[0] has the extension fraction
     *  ddata starts xnodes
     *  ddata+npts starts ynodes
     *  ddata+2*npts starts znodes
     */
        case SW_EXTEND_FAULT:

            nnode = ilist[0];

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %.15e\n",
                         ilist[0],
                         dlist[0]
                        );
                DLSF
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long)
            swcalc->sw_ExtendFaultFromJava (
                dlist[0],
                ddata,
                ddata+nnode,
                ddata+2*nnode,
                nnode);

            break;

    /*----------------------------------------------------------------------------
     * Convert a rectangular grid into a trimesh and send the trimesh back
     * to the java side.  This has no provisions for faulted grids.
     *
     *  ilist[0] has ncol
     *  ilist[1] has nrow
     *  dlist[0] has xmin
     *  dlist[1] has ymin
     *  dlist[2] has xmax
     *  dlist[3] has ymax
     *  dlist[4] has rang
     *  ddata has the grid data
     */
        case SW_GRID_TO_TRIMESH:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d\n",
                         ilist[0],
                         ilist[1]
                        );
                DLSF
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e\n",
                         dlist[0],
                         dlist[1],
                         dlist[2],
                         dlist[3],
                         dlist[4]
                        );
                DLSF
                nnode = ilist[0] * ilist[1];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e\n",
                             ddata[i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long)
            swcalc->sw_GridToTriMesh (
                ilist[0],
                ilist[1],
                dlist[0],
                dlist[1],
                dlist[2],
                dlist[3],
                dlist[4],
                ddata,
                Xline,
                Yline,
                Zline,
                Npline,
                LineTypes,
                Nline);

            free_lines;
            free_bounds;
            csw_Free (GridGeometry);
            csw_Free (GridOptions);
            GridOptions = NULL;
            GridGeometry = NULL;

            break;

    /*-----------------------------------------------------
     *
     * Calculate the convex hull outline of a set of points.
     *
     *  ilist[0] has the number of points
     *  first npts ddata have x coordinates
     *  next npts ddata have y coordinates
     */
        case SW_OUTLINE_POINTS:

            npts = ilist[0];
          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                DLSF
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i]
                            );
                    DLSF
                }
            }
          #endif

            istat =
            swcalc->sw_OutlinePoints (
                ddata,
                ddata+npts,
                npts);

            break;

    /*-----------------------------------------------------
     *
     * Determine if a point is inside a polygon.
     *
     * dlist[0] has the point x coordinate
     * dlist[1] has the point y coordinate
     * dlist[2] has the grazing value
     * ilist[0] has the number of polygon vertices.
     * ddata has the polygon x coordinates
     * ddata+npts has the polygon y coordinates
     */
        case SW_POINT_IN_POLY:

            npts = ilist[0];
          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d\n",
                         ilist[0]
                        );
                DLSF
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e\n",
                         dlist[0],
                         dlist[1],
                         dlist[2]
                        );
                DLSF
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i]
                            );
                    DLSF
                }
            }
          #endif

            istat =
            swcalc->sw_PointInPolygon (
                dlist[0],
                dlist[1],
                dlist[2],
                ddata,
                ddata+npts,
                npts);

            break;


    /*--------------------------------------------------------------
     *  Determine if 2 lines are very close to overlaping
     *  each other.  Only x and y coordinates are used to
     *  calculate the closeness.
     *
     *  ilist[0] has number of points in line 1 (npts1)
     *  ilist[1] has number of points in line 2 (npts2)
     *  dlist[0] has the critical distance (dcrit)
     *  ddata[0 to npts1-1] has the x coordinates of line 1
     *  ddata[npts1 to 2*npts1-1] has the y coordinates of line 1
     *  ddata[2*npts1 to 2*npts1 + npts2-1] has x for line 2
     *  the rest of ddata has y for line 2
     */
        case SW_CLOSE_TO_SAME_LINE_XY:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %f\n",
                         ilist[0],
                         ilist[1],
                         dlist[0]
                        );
                DLSF
                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i]
                            );
                    DLSF
                }
                n0 = npts * 2;
                npts = ilist[1];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e\n",
                             ddata[i+n0],
                             ddata[npts+i+n0]
                            );
                    DLSF
                }
            }
          #endif

            npts = ilist[0];
            n0 = npts * 2;
            istat = (long) grdapi->grd_CloseToSameLineXY (
                ddata, ddata+npts, ilist[0],
                ddata+n0, ddata+n0+ilist[1], ilist[1],
                dlist[0]);

            break;


    /*-----------------------------------------------------
     *
     * Remove all entries assodiated with a trimesh from the
     * cached index data in grd_triangle.c
     *
     * ilist[0] is id1
     * ilist[1] is id2
     */
        case SW_REMOVE_TRI_INDEX:
          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d\n",
                         ilist[0],
                         ilist[1]
                        );
                DLSF
            }
          #endif

          iistat =
          grdapi->grd_RemoveTriIndex (ilist[0], ilist[1]);

          istat = (long)iistat;

          break;

    /*-----------------------------------------------------
     *
     * Free all data in the cache used for trimeshes in grd_triangle.c
     */
        case SW_REMOVE_ALL_TRI_INDEXES:

          grdapi->grd_FreeTriIndexList ();

          break;

    /*-----------------------------------------------------
     *
     * Start the definition of a fault connnect group.
     */
        case SW_PS_START_FAULT_CONNECT_GROUP:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e\n",
                         dlist[0]
                        );
                DLSF
            }
          #endif

          istat = (long) psplit->ps_StartFaultConnect (dlist[0]);
          break;

    /*-----------------------------------------------------
     *
     * End a native fault group.
     */
        case SW_PS_END_FAULT_CONNECT_GROUP:

          istat = (long) psplit->ps_EndFaultConnect ();
          break;

    /*-----------------------------------------------------
     *
     * Calculate the fault connections.
     */
        case SW_PS_CONNECT_FAULTS:

          istat = (long) psplit->ps_ConnectFaults ();
          break;

    /*-----------------------------------------------------
     *
     * Set the detachment for a fault group.
     *
     *  ilist[0] had nnodes
     *  ilist[1] has nedges
     *  ilist[2] has ntris
     *  ddata has x, y, z of nodes
     *  idata has n1, n2, t1, t2 of edges.
     *  idata + 4 * nedges has e1, e2, e3 of triangles.
     */
        case SW_PS_SET_DETACHMENT:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];
            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) psplit->ps_SetDetachment (
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;

    /*-----------------------------------------------------
     *
     * Set the lower surface for a surface to surface operation.
     *
     *  ilist[0] had nnodes
     *  ilist[1] has nedges
     *  ilist[2] has ntris
     *  ddata has x, y, z of nodes
     *  idata has n1, n2, t1, t2 of edges.
     *  idata + 4 * nedges has e1, e2, e3 of triangles.
     */
        case SW_PS_SET_LOWER_TMESH:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];
            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) psplit->ps_SetLowerSurface (
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;


    /*-----------------------------------------------------
     *
     * Set the upper surface for a surface to surface operation.
     *
     *  ilist[0] had nnodes
     *  ilist[1] has nedges
     *  ilist[2] has ntris
     *  ddata has x, y, z of nodes
     *  idata has n1, n2, t1, t2 of edges.
     *  idata + 4 * nedges has e1, e2, e3 of triangles.
     */
        case SW_PS_SET_UPPER_TMESH:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];
            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) psplit->ps_SetUpperSurface (
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;


/*----------------------------------------------------------------------------------------*/

    /*
     * Calculate a detachment surface between the current lower and upper surfaces.
     */
        case SW_PS_CALC_DETACHMENT:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e\n",
                         dlist[0], dlist[1], dlist[2]
                        );
                DLSF
            }
          #endif

            istat = (long) psplit->ps_CalcDetachment (dlist[0], dlist[1], dlist[2]);

            break;



    /*-----------------------------------------------------
     *
     * Add to the detachment for a fault group.
     *
     *  ilist[0] had nnodes
     *  ilist[1] has nedges
     *  ilist[2] has ntris
     *  ddata has x, y, z of nodes
     *  idata has n1, n2, t1, t2 of edges.
     *  idata + 4 * nedges has e1, e2, e3 of triangles.
     */
        case SW_PS_ADD_TO_DETACHMENT:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];
            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) psplit->ps_AddToDetachment (
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;

    /*------------------------------------------------------------
     *
     * Add a fault to the fault connect group.
     *
     *  ilist[0] had nnodes
     *  ilist[1] has nedges
     *  ilist[2] has ntris
     *  ilist[3] has fault id
     *  ilist[4] has flag
     *  ilist[5] had sgp flag
     *  ddata has x, y, z of nodes
     *  idata has n1, n2, t1, t2 of edges.
     *  idata + 4 * nedges has e1, e2, e3 of triangles.
     */
        case SW_PS_ADD_CONNECTING_FAULT:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];
            n0 = nedge * 4;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5]
                        );
                DLSF
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %.15e\n",
                         dlist[0],
                         dlist[1],
                         dlist[2],
                         dlist[3],
                         dlist[4],
                         dlist[5]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

            istat = (long) psplit->ps_AddConnectingFault (
                ilist[3],
                ilist[4],
                ilist[5],
                dlist,
                ddata,
                ddata+nnode,
                ddata+nnode*2,
                nnode,
                idata,
                idata+nedge,
                idata+nedge*2,
                idata+nedge*3,
                nedge,
                idata+n0,
                idata+n0+ntri,
                idata+n0+ntri*2,
                ntri);

            break;

    /*----------------------------------------------------------
     * Start the data definition for calculation of an
     * attribute trimesh.
     *
     *  ilist[0] has num nodes
     *  ilist[1] has num edges
     *  ilist[2] has num tris
     *  ilist[3] has format flag
     *  ilist[4] has the vertical baseline used flag
     *  ilist[5] has the change trimesh flag
     *  dlist[0] has the vertical baseline x1
     *  dlist[1] has the vertical baseline y1
     *  dlist[2] has the vertical baseline x2
     *  dlist[3] has the vertical baseline y2
     *  dlist[4] has the vertical baseline zscale
     *  dlist[5] has the vertical baseline zmin
     *  ddata starts xnodes
     *  ddata + num nodes starts ynodes
     *  ddata + 2 * num node starts znodes
     *  idata starts n1edge
     *  idata + nedge starts n2edge
     *  idata + 2*nedge starts t1edge
     *  idata + 3*nedge starts t2edge
     *  idata + 4*nedge starts eflags
     *  idata + 5*nedge starts e1tri
     *  idata + 5*nedge + ntri starts e2tri
     *  idata + 5*nedge + 2*ntri starts e3tri
     */
        case SW_START_AT_TRI_MESH:

            nnode = ilist[0];
            nedge = ilist[1];
            ntri = ilist[2];

            n0 = nedge * 5;

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d %d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2],
                         ilist[3],
                         ilist[4],
                         ilist[5]
                        );
                DLSF
                sprintf (LogFileLine,
                         "%.15e %.15e %.15e %.15e %.15e %.15e\n",
                         dlist[0],
                         dlist[1],
                         dlist[2],
                         dlist[3],
                         dlist[4],
                         dlist[5]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nnode; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[nnode+i],
                             ddata[2*nnode+i]
                            );
                    DLSF
                }
                for (i=0; i<nedge; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d %d %d\n",
                             idata[i],
                             idata[nedge+i],
                             idata[2*nedge+i],
                             idata[3*nedge+i],
                             idata[4*nedge+i]
                            );
                    DLSF
                }
                for (i=0; i<ntri; i++) {
                    sprintf (LogFileLine,
                             "%d %d %d\n",
                             idata[n0+i],
                             idata[n0+ntri+i],
                             idata[n0+2*ntri+i]
                            );
                    DLSF
                }
            }
          #endif

/*
          istat =
            StartAtTriMesh (
               ilist[5],
               ilist[4],
               dlist,
               ddata,
               ddata+nnode,
               ddata+nnode*2,
               nnode,
               idata,
               idata+nedge,
               idata+nedge*2,
               idata+nedge*3,
               idata+nedge*4,
               nedge,
               idata+n0,
               idata+n0+ntri,
               idata+n0+ntri*2,
               ntri
            );
*/

          break;

    /*----------------------------------------------------------
     * Set the set of constraint lines to be applied to all attribute
     * calculations for the currently active trimesh.
     *
     * ilist[0] has the number of lines
     * ilist[1] has the total number of points
     * ilist[2] has the z flag
     * idata[0 to nlines-1] has the points per line
     * idata[nlines to 2*nlines-1] has the line types
     * ddata[0 to ntot-1] has x coordinates for all lines
     * ddata[ntot to 2*ntot-1] has y coordinates for all lines
     * ddata[2*ntot to 3*ntot-1] has z coordinates for all lines
     */
        case SW_SET_AT_POLYLINE:

          #if _SW_DEBUG_LOG_FILE_
            if (LogFile) {
                sprintf (LogFileLine,
                         "%d %d %d\n",
                         ilist[0],
                         ilist[1],
                         ilist[2]
                        );
                DLSF
                nline = ilist[0];
                for (i=0; i<nline; i++) {
                    sprintf (LogFileLine,
                             "%d %d\n",
                             idata[i],
                             idata[nline+i]
                            );
                    DLSF
                }
                npts = ilist[1];
                for (i=0; i<npts; i++) {
                    sprintf (LogFileLine,
                             "%.15e %.15e %.15e\n",
                             ddata[i],
                             ddata[npts+i],
                             ddata[2*npts+i]
                            );
                    DLSF
                }
            }
          #endif

/*
            istat =
              SetAtConstraintLines (
                ilist[0],
                idata,
                ddata,
                ddata+ilist[1],
                ddata+ilist[1]*2);
*/

            break;

    /*----------------------------------------------------------------
     * Calculate the draped attribute points
     * based on the constraints and points specified for the active
     * input attribute trimesh.  Send the results back to java.
     */
        case SW_CALC_AT_TRI_MESH:

/*
            istat =
              CalcAtTriMesh ();
*/

            break;


        default:
            break;

    }  /* end of giant switch statement */

    return istat;
}




/*
 * End of extern "C" declaration
 */
}

