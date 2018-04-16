/*
 ***************************************************************************

  This file is used to test the native side of the jeasyx system separately
  from the Java side.  The main simply reads from a command file and calls
  ezx_process_command as if the command came from Java.

  You need to edit the EZXJavaArea.cc file to enable debugging in order to
  run this program.  If you don't the program will either not link or it
  will fail miserably.

 ***************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <csw/hlevutils/src/ThreadGuard.h>

#include <csw/jsurfaceworks/private_include/SurfaceWorksJNI.h>
#include <csw/jsurfaceworks/private_include/SWCommand.h>
#include <csw/utils/include/csw_.h>
#include <csw/utils/private_include/csw_fileio.h>

#define SINGLE_THREAD_ID     1

int main (int argc, char *argv[])
{
    int              command_id, end_flag;
    int              i, cval;
    int              npts, nline, n0, ntot;
    char             *ctmp, local_line[200];
    FILE             *fptr;

    CSWFileioUtil    futil;

    static char      inbuff[1000];

    static int       ilist[1000];
    static long      llist[1000];
    static double    dlist[1000];
    static double    ddata[1000000];
    static float     fdata[1000000];
    static int       idata[1000000];
    static char      cdata[100000];

    argc = argc;
    argv = argv;

    setenv ("CSW_DONT_WRITE_PLAYBACK", "1", 1);

    bool  btest_mode = false;
    char  tfname[200];

    if (argc > 3) {
        if (strcmp (argv[2], "-test") == 0) {
            btest_mode = true;
            strcpy (tfname, argv[3]);
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

    FILE  *prim_file = NULL;

    if (btest_mode) {
        if (jni_get_prim_file_sw() == NULL) {
            prim_file = fopen (tfname, "w");
            jni_set_prim_file_sw (prim_file);
        }
    }

    for (;;) {

        ctmp = futil.csw_fgets (inbuff, 1000, fptr);
        if (ctmp == NULL) {
            break;
        }

        strncpy (local_line, inbuff, 199);
        local_line[199] = '\0';
        csw_StrLeftJust (local_line);
        if (local_line[0] == '\0'  ||  local_line[0] == '#') {
            continue;
        }

        cval = strncmp (local_line, "command=", 8);
        if (cval != 0) {
            continue;
        }

        sscanf (local_line+8, "%d", &command_id);

    /*
     * start of huge switch on each command
     */
        end_flag = 0;
        switch (command_id) {

            case SW_SET_LINES:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

                nline = ilist[0];
                for (i=0; i<nline; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (
                        inbuff,
                        "%d %d",
                        idata+i,
                        idata+nline+i
                    );
                }

                npts = ilist[1];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (
                        inbuff,
                        "%lf %lf %lf",
                        ddata+i,
                        ddata+npts+i,
                        ddata+2*npts+i
                    );
                }

                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );

                break;

        /*----------------------------------------------------------------*/

            case SW_SET_BOUNDS:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (
                        inbuff,
                        "%lf %lf %lf",
                        ddata+i,
                        ddata+npts+i,
                        ddata+2*npts+i
                    );
                }

                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );

                break;

        /*----------------------------------------------------------------*/

            case SW_CALC_CONSTANT_TRI_MESH:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                ilist[3] = 1;
                sscanf (
                    inbuff,
                    "%d %lf",
                    ilist+0,
                    dlist+0
                );

                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (
                        inbuff,
                        "%lf %lf",
                        ddata+i,
                        ddata+npts+i
                    );
                }

                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );

                break;


        /*----------------------------------------------------------------*/

            case SW_CALC_TRI_MESH:
            case SW_CALC_EXACT_TRIMESH:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                ilist[3] = 1;
                sscanf (
                    inbuff,
                    "%d %d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ilist+3
                );

                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (
                        inbuff,
                        "%lf %lf %lf",
                        ddata+i,
                        ddata+npts+i,
                        ddata+2*npts+i
                    );
                }

                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );
                break;

       /*----------------------------------------------------------------*/

           case SW_CALC_GRID:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

               npts = ilist[0];
               for (i=0; i<npts; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (
                       inbuff,
                       "%lf %lf %lf",
                       ddata+i,
                       ddata+npts+i,
                       ddata+2*npts+i
                   );
               }

               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                   NULL, NULL,
                   command_id,
                   SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------------------*/

           case SW_TRIMESH_DATA:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %d %d %d",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3,
                   ilist+4
               );

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------------------*/

           case SW_TRIMESH_OUTLINE:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------*/

           case SW_SET_DRAPE_LINES:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

               nline = ilist[0];
               for (i=0; i<nline; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d",
                           idata+i,
                           idata+nline+i);
               }
               if (end_flag == 1) {
                   break;
               }

               npts = ilist[1];
               for (i=0; i<npts; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+npts+i,
                           ddata+2*npts+i);
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;


        /*----------------------------------------------------------------*/

           case SW_SET_DRAPE_POINTS:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d",
                   ilist+1
               );

               npts = ilist[1];
               for (i=0; i<npts; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+npts+i,
                           ddata+2*npts+i);
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;


        /*----------------------------------------------------------------*/

           case SW_CLEAR_DRAPE_CACHE:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------*/

           case SW_CALC_DRAPED_LINES:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------*/

           case SW_CALC_DRAPED_POINTS:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------*/

           case SW_PS_SET_TEST_STATE:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d",
                   ilist+0
               );

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------*/

           case SW_PS_CALC_SPLIT:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %lf %lf",
                   ilist,
                   ilist+1,
                   dlist,
                   dlist+1
               );

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;


        /*----------------------------------------------------------------*/

           case SW_PS_CALC_LINES_FOR_SPLITTING:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------*/

           case SW_PS_CLEAR_ALL_DATA:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------*/

           case SW_PS_CLEAR_HORIZON_DATA:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------*/

            case SW_PS_ADD_BORDER_SEGMENT:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (
                    inbuff,
                    "%d %d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ilist+3
                );

                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (
                        inbuff,
                        "%lf %lf %lf",
                        ddata+i,
                        ddata+npts+i,
                        ddata+2*npts+i
                    );
                }

                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );

                break;

        /*----------------------------------------------------------------*/

            case SW_PS_ADD_CENTERLINE:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (
                    inbuff,
                    "%d %d %d %d %d %d %d",
                    ilist+0,
                    ilist+1,
                    ilist+2,
                    ilist+3,
                    ilist+4,
                    ilist+5,
                    ilist+6
                );

                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (
                        inbuff,
                        "%lf %lf %lf %lf %lf %lf",
                        ddata+i,
                        ddata+npts+i,
                        ddata+2*npts+i,
                        ddata+3*npts+i,
                        ddata+4*npts+i,
                        ddata+5*npts+i
                    );
                }

                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );

                break;

        /*----------------------------------------------------------------*/

            case SW_PS_SET_POINTS:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (
                        inbuff,
                        "%lf %lf %lf",
                        ddata+i,
                        ddata+npts+i,
                        ddata+2*npts+i
                    );
                }

                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );

                break;

    /*----------------------------------------------------------------------*/

            case SW_VERT_CALC_BASELINE:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff, "%lf %lf %lf",
                            ddata+i,
                            ddata+npts+i,
                            ddata+2*npts+i);
                }

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );

                break;

    /*----------------------------------------------------------------------*/

            case SW_VERT_SET_BASELINE:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (
                    inbuff,
                    "%d",
                    ilist+0
                );

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (
                    inbuff,
                    "%lf %lf %lf %lf %lf %lf",
                    ddata,
                    ddata+1,
                    ddata+2,
                    ddata+3,
                    ddata+4,
                    ddata+5
                );

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );

                break;

    /*----------------------------------------------------------------------*/

            case SW_CONVERT_NODE_TRIMESH:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff,
                        "%d %d",
                        ilist,
                        ilist+1
                       );

                npts = ilist[0];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff,
                            "%lf %lf %lf",
                            ddata+i,
                            ddata+npts+i,
                            ddata+2*npts+i
                           );
                }
                if (end_flag == 1) {
                    break;
                }

                npts = ilist[1];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff,
                            "%d %d %d\n",
                            idata+i,
                            idata+npts+i,
                            idata+2*npts+i
                           );
                }
                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );

                break;

    /*------------------------------------------------------------------------*/

           case SW_WRITE_TRIMESH_DATA:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               strcpy (cdata, inbuff);

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %d %d %d",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3,
                   ilist+4
               );

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf %lf %lf %lf %lf",
                   dlist,
                   dlist+1,
                   dlist+2,
                   dlist+3,
                   dlist+4,
                   dlist+5
               );

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

               break;

    /*----------------------------------------------------------------------*/

            case SW_READ_CSW_GRID:

               ctmp = futil.csw_fgets (cdata, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

               break;

    /*----------------------------------------------------------------------*/

            case SW_READ_TRIMESH_DATA:

               ctmp = futil.csw_fgets (cdata, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               sscanf (inbuff, "%li", llist);

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

               break;

    /*----------------------------------------------------------------------*/

            case SW_GRID_TO_TRIMESH:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf %lf %lf %lf",
                   dlist,
                   dlist+1,
                   dlist+2,
                   dlist+3,
                   dlist+4
               );

               end_flag = 0;
               for (i=0; i<ilist[0]*ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf",
                           ddata+i
                          );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

               break;

    /*----------------------------------------------------------------------*/

            case SW_EXTEND_FAULT:


               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %lf",
                   ilist+0,
                   dlist+0
               );

               end_flag = 0;
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

               break;

    /*------------------------------------------------------------------------*/

           case SW_PS_ADD_FAULT_SURFACE:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %d %d %d %d",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3,
                   ilist+4,
                   ilist+5
               );

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf %lf %lf %lf %lf %lf %lf",
                   dlist,
                   dlist+1,
                   dlist+2,
                   dlist+3,
                   dlist+4,
                   dlist+5,
                   dlist+6,
                   dlist+7);

               end_flag = 0;
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

               break;

    /*------------------------------------------------------------------------*/

           case SW_PS_ADD_FAULT_SURFACE_WITH_DETACHMENT_CONTACT:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %d %d %d %d %d %d",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3,
                   ilist+4,
                   ilist+5,
                   ilist+6,
                   ilist+7
               );

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf %lf %lf %lf %lf %lf %lf",
                   dlist,
                   dlist+1,
                   dlist+2,
                   dlist+3,
                   dlist+4,
                   dlist+5,
                   dlist+6,
                   dlist+7);

               end_flag = 0;
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[0] * 3;
               for (i=0; i<ilist[6]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+n0+i,
                           ddata+n0+ilist[6]+i,
                           ddata+n0+2*ilist[6]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

               break;

    /*---------------------------------------------------------------------------*/

            case SW_OUTLINE_POINTS:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d", ilist);
                for (i=0; i<ilist[0]; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff, "%lf %lf",
                            ddata+i,
                            ddata+ilist[0]+i);
                }
                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

                break;

    /*---------------------------------------------------------------------------*/

            case SW_POINT_IN_POLY:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%d", ilist);

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff, "%lf %lf %lf",
                    dlist,
                    dlist+1,
                    dlist+2);

                for (i=0; i<ilist[0]; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (inbuff, "%lf %lf",
                            ddata+i,
                            ddata+ilist[0]+i);
                }
                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

                break;

    /*---------------------------------------------------------------------------*/

            case SW_MODEL_BOUNDS:
            case SW_PS_MODEL_BOUNDS:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }
                sscanf (inbuff,
                        "%lf %lf %lf %lf %lf %lf",
                        ddata+0,
                        ddata+1,
                        ddata+2,
                        ddata+3,
                        ddata+4,
                        ddata+5
                       );

                sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

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

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                if (ctmp == NULL) {
                    end_flag = 1;
                    break;
                }

                sscanf (inbuff, "%d %d",
                        ilist,
                        ilist+1);

                sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

                break;

    /*-----------------------------------------------------
     *
     * Free all data in the cache used for trimeshes in grd_triangle.c
     */
            case SW_REMOVE_ALL_TRI_INDEXES:

                sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata);

                break;


    /*----------------------------------------------------------------------------*/

           case SW_PS_ADD_HORIZON_PATCH:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %d %d %d %d %lf",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3,
                   ilist+4,
                   ilist+5,
                   dlist+6
               );

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*----------------------------------------------------------------*/

           case SW_PS_START_SEALED_MODEL_DEF:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d",
                   ilist);

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*----------------------------------------------------------------*/

           case SW_PS_CALC_SEALED_MODEL:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf",
                   dlist);

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*----------------------------------------------------------------*/

           case SW_PS_GET_SEALED_INPUT:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*----------------------------------------------------------------*/

          case SW_PS_SET_SED_SURFACE:
          case SW_PS_SET_MODEL_BOTTOM:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %d %d %d %d %lf",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3,
                   ilist+4,
                   ilist+5,
                   dlist+6
               );

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------*/

        case SW_CLOSE_TO_SAME_LINE_XY:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (inbuff,
                       "%d %d %lf",
                       ilist,
                       ilist+1,
                       dlist);
               nline = ilist[0];
               for (i=0; i<nline; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf", ddata+i, ddata+i+nline);
               }
               n0 = nline * 2;
               nline = ilist[1];
               for (i=0; i<nline; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf", ddata+i+n0, ddata+i+nline+n0);
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;


    /*------------------------------------------------------------------------------------*/

           case SW_PS_SET_LOWER_TMESH:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_SET_UPPER_TMESH:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

        /*----------------------------------------------------------------*/

           case SW_PS_CALC_DETACHMENT:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf %lf",
                   dlist, dlist+1, dlist+2
               );

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_SET_DETACHMENT:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_ADD_TO_DETACHMENT:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_ADD_CONNECTING_FAULT:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %d %d %d %d",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3,
                   ilist+4,
                   ilist+5
               );

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf %lf %lf %lf %lf",
                   dlist+0,
                   dlist+1,
                   dlist+2,
                   dlist+3,
                   dlist+4,
                   dlist+5
               );

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_START_FAULT_CONNECT_GROUP:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf",
                   dlist+0
               );

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

           case SW_PS_END_FAULT_CONNECT_GROUP:
           case SW_PS_CONNECT_FAULTS:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_ADD_1D_FAULT_LINE:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %d %d",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3
               );

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf",
                   dlist+0,
                   dlist+1
               );

               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_ADD_1D_HORIZON:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %d %d %d %d %lf",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3,
                   ilist+4,
                   ilist+5,
                   dlist+6
               );

               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 4;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_ADD_1D_HORIZON_POINTS:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %lf",
                   ilist+0,
                   ilist+1,
                   dlist+0
               );

               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_ADD_1D_HORIZON_LINES:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %lf",
                   ilist+0,
                   ilist+1,
                   dlist+0
               );

               ntot = 0;
               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d",
                           idata+i);
                   ntot += idata[i];
               }

               for (i=0; i<ntot; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ntot+i,
                           ddata+2*ntot+i);
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_ADD_1D_HORIZON_GRID:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf %lf %lf %lf %lf",
                   dlist+0,
                   dlist+1,
                   dlist+2,
                   dlist+3,
                   dlist+4,
                   dlist+5
               );

               ntot = ilist[1] * ilist[2];
               for (i=0; i<ntot; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf",
                           ddata+i);
               }

               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*------------------------------------------------------------------------------------*/

           case SW_PS_1D_MODEL_BOUNDS:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf %lf %lf %lf %lf",
                   ddata+0,
                   ddata+1,
                   ddata+2,
                   ddata+3,
                   ddata+4,
                   ddata+5
               );

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*---------------------------------------------------------------------*/

           case SW_BATCH_1D_MESHER_START_DEFINITION:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf",
                   dlist+0
               );

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

           case SW_BATCH_1D_MESHER_SET_MODEL_BOUNDARY:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf %lf %lf %lf %lf",
                   ddata+0,
                   ddata+1,
                   ddata+2,
                   ddata+3,
                   ddata+4,
                   ddata+5
               );

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

           case SW_BATCH_1D_MESHER_ADD_FAULT_LINE:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

           case SW_BATCH_1D_MESHER_CREATE_MASTER_TRI_MESH:

               end_flag = 0;

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

           case SW_BATCH_1D_MESHER_SET_HORIZON_POINTS:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d",
                   ilist+0
               );

               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

           case SW_BATCH_1D_MESHER_SET_HORIZON_LINES:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d",
                   ilist+0
               );

               ntot = 0;
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d",
                           idata+i);
                   ntot += idata[i];
               }

               for (i=0; i<ntot; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ntot+i,
                           ddata+2*ntot+i);
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

           case SW_BATCH_1D_MESHER_CALC_HORIZON_TRI_MESH:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d",
                   ilist+0
               );

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

           case SW_BATCH_1D_MESHER_CLEAR_ALL_DATA:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*---------------------------------------------------------------------*/

           case SW_PS_START_1D_MODEL_DEF:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf",
                   dlist+0
               );

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

           case SW_PS_CALC_1D_MODEL:

               end_flag = 0;

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d",
                   ilist+0
               );

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

           case SW_PS_CLEAR_ALL_1D_DATA:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

    /*---------------------------------------------------------------------*/

           case SW_START_AT_TRI_MESH:

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%d %d %d %d %d %d",
                   ilist+0,
                   ilist+1,
                   ilist+2,
                   ilist+3,
                   ilist+4,
                   ilist+5
               );

               ctmp = futil.csw_fgets (inbuff, 1000, fptr);
               if (ctmp == NULL) {
                   end_flag = 1;
                   break;
               }
               sscanf (
                   inbuff,
                   "%lf %lf %lf %lf %lf %lf",
                   dlist+0,
                   dlist+1,
                   dlist+2,
                   dlist+3,
                   dlist+4,
                   dlist+5
               );

               end_flag = 0;
               nline = ilist[0];
               for (i=0; i<ilist[0]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%lf %lf %lf",
                           ddata+i,
                           ddata+ilist[0]+i,
                           ddata+2*ilist[0]+i);
               }
               if (end_flag == 1) {
                   break;
               }

               for (i=0; i<ilist[1]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d %d %d",
                            idata+i,
                            idata+ilist[1]+i,
                            idata+2*ilist[1]+i,
                            idata+3*ilist[1]+i,
                            idata+4*ilist[1]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               n0 = ilist[1] * 5;
               for (i=0; i<ilist[2]; i++) {
                   ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                   if (ctmp == NULL) {
                       end_flag = 1;
                       break;
                   }
                   sscanf (inbuff, "%d %d %d",
                            idata+n0+i,
                            idata+n0+ilist[2]+i,
                            idata+n0+2*ilist[2]+i
                           );
               }
               if (end_flag == 1) {
                   break;
               }

               sw_process_command (
                    NULL, NULL,
                   command_id,
                    SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

/*-----------------------------------------------------------------*/

           case SW_SET_AT_POLYLINE:

                ctmp = futil.csw_fgets (inbuff, 1000, fptr);
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

                nline = ilist[0];
                for (i=0; i<nline; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (
                        inbuff,
                        "%d %d",
                        idata+i,
                        idata+nline+i
                    );
                }

                npts = ilist[1];
                for (i=0; i<npts; i++) {
                    ctmp = futil.csw_fgets (inbuff, 1000, fptr);
                    if (ctmp == NULL) {
                        end_flag = 1;
                        break;
                    }
                    sscanf (
                        inbuff,
                        "%lf %lf %lf",
                        ddata+i,
                        ddata+npts+i,
                        ddata+2*npts+i
                    );
                }

                if (end_flag == 1) {
                    break;
                }

                sw_process_command (
                    NULL, NULL,
                    command_id,
                    SINGLE_THREAD_ID,
                    ilist,
                    llist,
                    dlist,
                    cdata,
                    idata,
                    fdata,
                    ddata
                );

                break;

/*-----------------------------------------------------------------------*/

            case SW_CALC_AT_TRI_MESH:

               sw_process_command (
                    NULL, NULL,
                   command_id,
                   SINGLE_THREAD_ID,
                   ilist,
                   llist,
                   dlist,
                   cdata,
                   idata,
                   fdata,
                   ddata
               );

               break;

/*-----------------------------------------------------------------------*/

           default:

               break;


        } /* end of huge switch statement */

        if (end_flag == 1) {
            break;
        }

    } /* end of for loop reading the file */

    ThreadGuard::RemoveAllThreadData ();

    if (fptr != NULL) {
        fclose (fptr);
    }

    fprintf (stdout, "SurfaceWorks native test completed.\n");

    unsetenv ("CSW_DONT_WRITE_PLAYBACK");

    return 0;

}
