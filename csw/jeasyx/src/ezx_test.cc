
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

  This file is used to test the native side of the jeasyx system separately
  from the Java side.  The main simply reads from a command file and calls
  ezx_process_command as if the command came from Java.

  You need to edit the EZXJavaArea.cc file to enable debugging in order to
  run this program.  If you don't the program will either not link or it
  will fail miserably.

  This program reads from standard input, which will almost always be a file
  redirected via the < operator on the command line.


  In the 2017 refactor, no attempt is made to make this file thread safe.
  The usage in testing will always be from a single thread, so the effort
  to make it thread safe would be wasted.

 ***************************************************************************
*/
#include <stdlib.h>
#include <stdio.h>

#include "csw/jeasyx/private_include/EZXCommand.h"
#include "csw/jeasyx/private_include/gtx_msgP.h"
#include "csw/jeasyx/private_include/DisplayListJNI.h"
#include "csw/utils/include/csw_.h"
#include "csw/utils/private_include/csw_fileio.h"
#include "csw/utils/private_include/TextBounds.h"

#include "csw/hlevutils/src/ThreadGuard.h"

FILE      *dfile = NULL;

#define  MAX_DDATA   70000000


int main (int argc, char *argv[])
{
    int              command_id, cval, end_flag;
    int              dlist_index;
    int              i, nstart, n, ntot, ival1, ival2, ival3, ival4;
    char             *ctmp, local_line[200];
    FILE             *fptr;

    static char      inbuff[1000];

    static long      longlist[100];
    static int       ilist[1000];
    static double    ddata[MAX_DDATA + 100];
    static float     fdata[1000000];
    static int       idata[10000000];
    static short int sdata[1000000];
    static unsigned char  bdata[5000000];
    static char      cdata[100000];

    argc = argc;
    argv = argv;

    setenv ("CSW_DONT_WRITE_PLAYBACK", "1", 1);

    bool  btest_mode = false;
    char  tfname[200];

    tfname[0] = '\0';
    if (argc > 3) {
      if (strcmp (argv[2], "-test") == 0) {
        strcpy (tfname, argv[3]);
        btest_mode = true;
      }
    }

/*
 * Rather than using standard input, open a file specified
 * in argv[1] as the input.  This is done because valgrind
 * needs standard input to be the terminal when it is run.
 */
    if (argc < 2) {
        printf ("You must specify a log file as the first argument.\n");
        return 0;
    }

    if (argv[1] == NULL) {
        printf ("You must specify a log file as the first argument.\n");
        return 0;
    }

    fptr = fopen (argv[1], "rb");
    if (fptr == NULL) {
        printf ("Error opening the specified log file.\n");
        return 0;
    }

    dfile = fopen ("dirt_native.log", "wb");
    dlist_index = -1;

    int    threadid = 0;

    CSWFileioUtil   csw_fileio_obj;

    FILE            *prim_file = NULL;

    if (btest_mode) {
        if (jni_get_prim_file_ezx () == NULL) {
            prim_file = fopen (tfname, "w");
            jni_set_prim_file_ezx  (prim_file);
        }
    }

    for (;;) {

        ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
        if (ctmp == NULL) {
            break;
        }

        strncpy (local_line, inbuff, 199);
        local_line[199] = '\0';
        csw_StrLeftJust (local_line);
        if (local_line[0] == '\0'  ||  local_line[0] == '#') {
            continue;
        }

        cval = strncmp (local_line, "##//", 4);
        if (cval == 0) {
            continue;
        }

        cval = strncmp (local_line, "command=", 8);
        if (cval != 0) {
            continue;
        }

        sscanf (local_line+8, "%d %d", &command_id, &dlist_index);

    /*
     * start of huge switch on each command
     */
        int  ngdata = 0;
        end_flag = 0;
        switch (command_id) {

            case GTX_DRAW_CURRENT_VIEW:

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_DRAW_SELECTED:

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_UNSELECT_ALL:

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_PICKPRIM:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (
                    inbuff,
                    "%d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_DEFAULT_ATTRIB:

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_CREATEWINDOW:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                strcpy (cdata, inbuff);

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (
                    inbuff,
                    "%d %d %d %ld %lf %lf %lf %lf",
                    &dlist_index,
                    ilist+0,
                    ilist+1,
                    longlist+0,
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_OPENWINDOW:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }

                strcpy (cdata, inbuff);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_DELETEWINDOW:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }

                sscanf (inbuff, "%d", ilist+0);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETSCREENSIZE:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }

                sscanf (
                    inbuff,
                    "%d %d",
                    ilist+0,
                    ilist+1
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETCLIPAREA:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }

                sscanf (
                    inbuff,
                    "%lf %lf %lf %lf %d",
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3,
                    ilist+0
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_CREATEFRAME:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                strcpy (cdata, inbuff);

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf %lf %lf",
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3
                );

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf %lf %lf",
                    ddata+4,
                    ddata+5,
                    ddata+6,
                    ddata+7
                );

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf %lf %lf %lf %lf",
                    ddata+8,
                    ddata+9,
                    ddata+10,
                    ddata+11,
                    ddata+12,
                    ddata+13
                );

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d %d %d %d %d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ilist+3,
                    ilist+4,
                    ilist+5,
                    ilist+6
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_ADD_AXIS:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                strcpy (cdata, inbuff);

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }

                sscanf (inbuff, "%d %d %d %d %d %d %d %d %d %d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ilist+3,
                    ilist+4,
                    ilist+5,
                    ilist+6,
                    ilist+7,
                    ilist+8,
                    ilist+9,
                    ilist+10,
                    ilist+11
                );

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3,
                    ddata+4,
                    ddata+5,
                    ddata+6,
                    ddata+7,
                    ddata+8,
                    ddata+9
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;


            case GTX_FRAME_AXIS_VALUES:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                strcpy (cdata, inbuff);

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }

                sscanf (inbuff, "%d %d %d %d %d %d %d %d %d %d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ilist+3,
                    ilist+4,
                    ilist+5,
                    ilist+6,
                    ilist+7,
                    ilist+8,
                    ilist+9,
                    ilist+10,
                    ilist+11
                );

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf %lf %lf",
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;


            case GTX_SETFRAME:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }

                strcpy (cdata, inbuff);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETCOLOR:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }

                sscanf (inbuff,
                        "%d %d %d %d %d",
                        ilist+0,
                        ilist+1,
                        ilist+2,
                        ilist+3,
                        ilist+4);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_DRAWLINE:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d", ilist+0);
                for (i=0; i<ilist[0]; i++) {
                    ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff, "%lf %lf", ddata+i, ddata+ilist[0]+i);
                }

                if (end_flag == 1) {
                    break;
                }

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_FILLPOLY:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2
                );

                for (i=0; i<ilist[0]; i++) {
                    ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff, "%d", idata+i);
                }
                if (end_flag == 1) {
                    break;
                }

                for (i=0; i<ilist[1]; i++) {
                    ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff, "%lf %lf", ddata+i, ddata+ilist[1]+i);
                }
                if (end_flag == 1) {
                    break;
                }

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_DRAWSYMBOL:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf %lf %lf %d",
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3,
                    ilist+0
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_DRAWTEXT:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf %lf %lf",
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3
                );

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                strcpy (cdata, inbuff);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_DRAWNUMBER:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf %lf %lf %lf %d %d",
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3,
                    ddata+4,
                    ilist+0,
                    ilist+1
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_DRAWSHAPE:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff,
                    "%d %lf %lf %lf %lf %lf %lf %lf %lf",
                    ilist+0,
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3,
                    ddata+4,
                    ddata+5,
                    ddata+6,
                    ddata+7
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_DRAWIMAGE:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff,
                    "%d %d %d %lf %lf %lf %lf",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3
                );

                ntot = ilist[1] * ilist[2];

                if (ilist[0] == 0) {
                    for (i=0; i<ntot; i++) {
                        ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                        if (ctmp == NULL) {
                            end_flag = 1;
                            break;
                        }
                        sscanf (inbuff, "%d %d %d %d",
                            &ival1, &ival2, &ival3, &ival4
                        );
                        bdata[i] = (unsigned char)ival1;
                        bdata[ntot+i] = (unsigned char)ival2;
                        bdata[2*ntot+i] = (unsigned char)ival3;
                    }
                }
                else if (ilist[0] == 1) {
                    for (i=4; i<ntot+5; i++) {
                        ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                        if (ctmp == NULL) {
                            end_flag = 1;
                            break;
                        }
                        sscanf (inbuff, "%lf",
                            ddata + i
                        );
                    }
                }
                else if (ilist[0] == 2) {
                    for (i=0; i<ntot+1; i++) {
                        ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                        if (ctmp == NULL) {
                            end_flag = 1;
                            break;
                        }
                        sscanf (inbuff, "%f",
                            fdata + i
                        );
                    }
                }
                else if (ilist[0] == 3) {
                    for (i=0; i<ntot+1; i++) {
                        ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                        if (ctmp == NULL) {
                            end_flag = 1;
                            break;
                        }
                        sscanf (inbuff, "%d",
                            idata + i
                        );
                    }
                }
                else if (ilist[0] == 4) {
                    for (i=0; i<ntot+1; i++) {
                        ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                        if (ctmp == NULL) {
                            end_flag = 1;
                            break;
                        }
                        sscanf (inbuff, "%d",
                            &ival1
                        );
                        sdata[i] = (short int)ival1;
                    }
                }
                else if (ilist[0] == 5) {
                    for (i=0; i<ntot+1; i++) {
                        ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                        if (ctmp == NULL) {
                            end_flag = 1;
                            break;
                        }
                        sscanf (inbuff, "%d",
                            &ival1
                        );
                        bdata[i] = (unsigned char)ival1;
                    }
                }

                if (end_flag == 1) {
                    break;
                }

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_ZOOMFRAME:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d %d %d %d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ilist+3,
                    ilist+4,
                    ilist+5
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

           case GTX_PANFRAME:

               ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (inbuff, "%d %d %d %d %d",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3,
                   ilist+4
               );

               ezx_process_command (
                   dlist_index,
                   command_id,
                   threadid,
                   longlist,
                   ilist,
                   cdata,
                   bdata,
                   sdata,
                   idata,
                   fdata,
                   ddata,
                   dfile
               );

               break;

            case GTX_ZOOMEXTENTS:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d",
                    ilist+0
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_FRAME_NAME_EXTENTS:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                strcpy (cdata, inbuff);

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff,
                        "%lf %lf %lf %lf",
                        ddata,
                        ddata+1,
                        ddata+2,
                        ddata+3
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_ZOOMOUT:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d",
                    ilist+0
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_RESETFRAME:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                strcpy (cdata, inbuff);

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf %lf %lf",
                    ddata+0,
                    ddata+1,
                    ddata+2,
                    ddata+3
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETFRAMECLIP:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d", ilist+0);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETBGCOLOR:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d %d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ilist+3
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETFGCOLOR:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d %d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ilist+3
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_ARROW_STYLE:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d", ilist+0);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETFILLPATTERN:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d %lf",
                    ilist+0,
                    ddata+0
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETLINEPATTERN:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d %lf",
                    ilist+0,
                    ddata+0
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_TEXTBACKGROUND:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d", ilist+0);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETFONT:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d", ilist+0);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETSMOOTH:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d", ilist+0);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETLINETHICK:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf", ddata+0);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETTEXTTHICK:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf", ddata+0);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

           case GTX_SET_SELECTABLE:

               ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (inbuff, "%d", ilist+0);

               ezx_process_command (
                   dlist_index,
                   command_id,
                   threadid,
                   longlist,
                   ilist,
                   cdata,
                   bdata,
                   sdata,
                   idata,
                   fdata,
                   ddata,
                   dfile
               );

               break;

            case GTX_TEXTANCHOR:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d", ilist+0);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_TEXTOFFSETS:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf",
                    ddata+0,
                    ddata+1);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_IMAGECOLORBANDS:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d", ilist+0);

                n = ilist[0];
                for (i=0; i<n; i++) {
                    ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff, "%lf %lf %d %d %d %d",
                        ddata+i, ddata+n+i,
                        idata+i, idata+n+i, idata+2*n+i, idata+3*n+i);
                }

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_IMAGENAME:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                strcpy (cdata, inbuff);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_IMAGEOPTIONS:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d %d %d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ilist+3,
                    ilist+4
                );

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETITEM:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                strcpy (cdata, inbuff);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_SETLAYER:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                strcpy (cdata, inbuff);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_UNSETLAYER:

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_UNSETITEM:

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_UNSETFRAME:

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_CONTOUR_PROPERTIES:

                for (i=0; i<45; i++) {
                    ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff, "%d", idata+i);
                }
                if (end_flag == 1) {
                    break;
                }
                bdata[0] = (unsigned char)idata[0];
                bdata[1] = (unsigned char)idata[1];
                bdata[2] = (unsigned char)idata[2];
                bdata[3] = (unsigned char)idata[3];
                bdata[4] = (unsigned char)idata[4];
                bdata[5] = (unsigned char)idata[5];
                bdata[36] = (unsigned char)idata[36];
                bdata[37] = (unsigned char)idata[37];
                bdata[38] = (unsigned char)idata[38];
                bdata[39] = (unsigned char)idata[39];

                for (i=0; i<23; i++) {
                    ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff, "%lf", ddata+i);
                }
                if (end_flag == 1) {
                    break;
                }

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

            case GTX_FAULT_LINE_DATA:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff,
                    "%d %d",
                    ilist+0,
                    ilist+1);

                for (i=0; i<ilist[0]; i++) {
                    ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff, "%d", idata+i);
                }
                if (end_flag == 1) {
                    break;
                }

                for (i=0; i<ilist[1]; i++) {
                    ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff,
                        "%lf %lf %lf",
                        ddata+i,
                        ddata + i + ilist[1],
                        ddata + i + ilist[1] * 2
                    );
                }
                if (end_flag == 1) {
                    break;
                }

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );

                break;

           case GTX_TRIMESH_DATA:

               ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               strcpy (cdata, inbuff);

               ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }

               sscanf (inbuff,
                       "%d %d %d\n",
                       ilist,
                       ilist+1,
                       ilist+2);

               n = ilist[0];
               for (i=0; i<n; i++) {
                   ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff,
                           "%lf %lf %lf %d\n",
                           ddata+i,
                           ddata+i+n,
                           ddata+i+2*n,
                           idata+i);
               }

               nstart = n;
               n = ilist[1];
               for (i=0; i<n; i++) {
                   ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff,
                           "%d %d %d %d %d\n",
                           idata+nstart+i,
                           idata+nstart+i+n,
                           idata+nstart+i+2*n,
                           idata+nstart+i+3*n,
                           idata+nstart+i+4*n);
               }

               nstart += 5 * n;
               n = ilist[2];
               for (i=0; i<n; i++) {
                   ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff,
                           "%d %d %d %d\n",
                           idata+nstart+i,
                           idata+nstart+i+n,
                           idata+nstart+i+2*n,
                           idata+nstart+i+3*n);
               }

               ezx_process_command (
                   dlist_index,
                   command_id,
                   threadid,
                   longlist,
                   ilist,
                   cdata,
                   bdata,
                   sdata,
                   idata,
                   fdata,
                   ddata,
                   dfile
               );

               break;

           case GTX_GRID_DATA:

               ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }

               strcpy (cdata, ctmp);

               ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (inbuff,
                   "%d %d %lf %lf %lf %lf %lf",
                   ilist+0,
                   ilist+1,
                   ddata+0,
                   ddata+1,
                   ddata+2,
                   ddata+3,
                   ddata+4
               );

               ngdata = ilist[0] * ilist[1];
               for (i=0; i<ilist[0] * ilist[1]; i++) {
                   ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   if (ngdata < MAX_DDATA - 100) {
                       sscanf (inbuff, "%lf", ddata+i+5);
                   }
               }
               if (end_flag == 1  ||  ngdata >= MAX_DDATA - 100) {
                   break;
               }

               ezx_process_command (
                   dlist_index,
                   command_id,
                   threadid,
                   longlist,
                   ilist,
                   cdata,
                   bdata,
                   sdata,
                   idata,
                   fdata,
                   ddata,
                   dfile
               );

               break;


           case GTX_DELETE_SELECTED:

               ezx_process_command (
                   dlist_index,
                   command_id,
                   threadid,
                   longlist,
                   ilist,
                   cdata,
                   bdata,
                   sdata,
                   idata,
                   fdata,
                   ddata,
                   dfile
               );

               break;


           case GTX_HIDE_SELECTED:

               ezx_process_command (
                   dlist_index,
                   command_id,
                   threadid,
                   longlist,
                   ilist,
                   cdata,
                   bdata,
                   sdata,
                   idata,
                   fdata,
                   ddata,
                   dfile
               );

               break;


           case GTX_UNHIDE_ALL:

               ezx_process_command (
                   dlist_index,
                   command_id,
                   threadid,
                   longlist,
                   ilist,
                   cdata,
                   bdata,
                   sdata,
                   idata,
                   fdata,
                   ddata,
                   dfile
               );

               break;

            case GTX_GET_SYMBOL_PARTS:

               ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (inbuff,
                       "%d %lf %lf %d",
                       ilist,
                       ddata,
                       ddata+1,
                       ilist+1);

               ezx_process_command (
                   dlist_index,
                   command_id,
                   threadid,
                   longlist,
                   ilist,
                   cdata,
                   bdata,
                   sdata,
                   idata,
                   fdata,
                   ddata,
                   dfile
               );

               break;

            case GTX_SET_SELECT_STATE:

                ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d %d", ilist+0, ilist+1);

                ezx_process_command (
                    dlist_index,
                    command_id,
                    threadid,
                    longlist,
                    ilist,
                    cdata,
                    bdata,
                    sdata,
                    idata,
                    fdata,
                    ddata,
                    dfile
                );
                break;

            case GTX_IMAGE_FAULT_DATA:

               ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (inbuff,
                       "%d %d",
                       ilist,
                       ilist+1);

               for (i=0; i<ilist[0]; i++) {
                   ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff,
                           "%d",
                           idata+i);
               }

               for (i=0; i<ilist[1]*2; i++) {
                   ctmp = csw_fileio_obj.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff,
                           "%lf",
                           ddata+i);
               }

               ezx_process_command (
                   dlist_index,
                   command_id,
                   threadid,
                   longlist,
                   ilist,
                   cdata,
                   bdata,
                   sdata,
                   idata,
                   fdata,
                   ddata,
                   dfile
               );

               break;


            default:

                break;


        }  /* end of huge switch  */

        dlist_index = 0;

        if (end_flag) {
            break;
        }

    }  /* end of for loop reading command file */

    if (btest_mode) {
        jni_close_prim_file_ezx ();
    }

    ezx_process_command (
        dlist_index,
        GTX_CLOSE_LOG_FILE,
        threadid,
        longlist,
        ilist,
        cdata,
        bdata,
        sdata,
        idata,
        fdata,
        ddata,
        dfile
    );

    ThreadGuard::RemoveAllThreadData ();

    fprintf (stdout, "EasyX native only test completed.\n");

    if (fptr != NULL) {
        fclose (fptr);
    }
    if (dfile != NULL) {
        fclose (dfile);
    }

//    unsetenv ("CSW_DONT_WRITE_PLAYBACK");

    return 1;

}
