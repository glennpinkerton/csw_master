
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/


/*
 * All of the functions needing jni.h or other associated headers
 * are grouped together in this file.  These include commands to
 * manipulate the display list and commands to draw the display list
 * back to the Java side.
 */

/*
 * These are the class methods on the Java JDisplayList class that need
 * to be used to send back drawing data.  These can be set the
 * first time they are needed and be used again for the entire
 * program execution.  Several functions need them so they are
 * declared as static file variables.
 */


#include <jni.h>
#include <limits.h>
#include <stdio.h>

#include <csw/utils/private_include/TextBounds.h>

#include <csw/jeasyx/private_include/DisplayListJNI.h>
#include <csw/jeasyx/private_include/EZXCommand.h>
#include <csw/jeasyx/private_include/gtx_msgP.h>

#include "csw/jeasyx/src/csw_jeasyx_src_JDisplayListBase_FontDef.h"
#include "csw/jeasyx/src/csw_jeasyx_src_JDisplayListBase.h"
#include "csw/jeasyx/src/csw_jeasyx_src_JDisplayList.h"

#define _MAX_LONG_    1

#define DEBUG_JNI_FILE 0

#if INT_MAX != 2147483647
#define MAX_LIST_SIZE \
csw_jeasyx_src_JDisplayListBase_MAX_LIST_SIZE
#endif

/*
 * The following static variables should be thread safe (I hope)
 * They are set once and then read by all threads.  It should not
 * matter which thread sets them since they will not change.
 */

static int setup_return_select_method_ids(JNIEnv *env, jclass cls);

#if DEBUG_JNI_FILE
static FILE           *dbfile = NULL;
static char           *cenv;
static char           dbname[500];
static char           fileline[1000];
#endif


/*
 * The primfile is used when "recording" of the graphical
 * results is desired.  The original reason for this is to
 * record results of test cases for future comparisons. This
 * is not thread safe.  Only use this from the c++ only test
 * programs (which only use single threads).
 */

static FILE           *primfile = NULL;


void jni_set_prim_file_ezx (FILE *pf) {
  primfile = pf;
}

FILE *jni_get_prim_file_ezx () {
  return primfile;
}

void jni_open_prim_file_ezx (char *fname) {
  if (fname != NULL) {
    primfile = fopen (fname, "w");
  }
}

void jni_close_prim_file_ezx () {
  if (primfile != NULL) fclose (primfile);
  primfile = NULL;
}




JNIEXPORT jint JNICALL Java_csw_jeasyx_src_CreateNative_nativeCreateDlist
  (JNIEnv *jenv, jclass jcls)
{

    int dlist_index = ezx_create_new_dlist_data ();

    return (jint)dlist_index;

}




/*
 * Process an easyx display list command sent from Java.
 * All commands except the Draw command are processed here.
 * The Draw command is done via a separate draw function.
 */
JNIEXPORT jint JNICALL Java_csw_jeasyx_src_JDisplayListBase_sendCommand
(
   JNIEnv *jnienv,
   jobject jobj,
   jint j_dlist_index,
   jint j_command_id,
   jint j_thread_id,
   jlongArray j_llist,
   jintArray j_ilist,
   jstring j_cdata,
   jbooleanArray j_bdata,
   jshortArray j_sdata,
   jintArray j_idata,
   jfloatArray j_fdata,
   jdoubleArray j_ddata,
   jint selectable
) {
    int              status, i;
    int              command_id;
    int              threadid;
    int              dlist_index;
    char             *cdata;
    unsigned char    *bdata;
    short int        *sdata;
    jint             *ilist,
                     *idata;
    float            *fdata;
    double           *ddata;

    jlong            *jllist;
    long             llist[100];

    threadid = (int)j_thread_id;

/*
 * If the int type is not a 32 bit signed integer, a conversion
 * from jint to int will be needed.  If the int is smaller than
 * a 32 bit signed then don't even compile since there will be
 * all kinds of problems.
 */
#if INT_MAX != 2147483647
    jsize            ntot;
    int              ilist2[1000];
    int              *idata;
    int              ibad;
  #if INT_MAX  <  2147483647
    #error The int data type with this compiler is less than 32 bits, ABORT.
  #endif
#endif

  #if DEBUG_JNI_FILE
    if (dbfile == NULL) {
        cenv = getenv ("SRC_TREE");
        if (cenv) {
            strcpy (dbname, cenv);
            terminate_directory_with_separator (dbname);
        }
        else {
            dbname[0] = '\0';
        }
        strcat(dbname, "ezx_debug_jni.txt");
        dbfile = fopen (dbname, "wb");
        strcpy (fileline, "first line of debug file\n");
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
    if (dbfile) {
        sprintf (fileline, "\n\nEntering sendCommand function\n");
        if (dbfile) {
            fputs (fileline, dbfile);
            fflush (dbfile);
        }
    }
  #endif

    jllist = NULL;
    ilist = NULL;
    cdata = NULL;
    bdata = NULL;
    sdata = NULL;
    idata = NULL;
    fdata = NULL;
    ddata = NULL;


/*
 * Get pointers to elements in the java arrays.  These are
 * not copies, but instead point into the java array object
 * directly.
 */
    command_id = (int)j_command_id;
    dlist_index = (int)j_dlist_index;

    ezx_set_void_ptrs (dlist_index, (void *)jnienv, (void *)jobj);

    if (command_id == GTX_CREATEWINDOW) {
        gtx_SetTextBoundsJNIFunction (jni_get_text_bounds);
    }

    if (j_llist) {
        jllist = (*jnienv)->GetLongArrayElements (jnienv, j_llist, JNI_FALSE);
    }
  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed GetLongArrayElements get %p %p\n", j_llist, jllist);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_ilist) {
        ilist = (*jnienv)->GetIntArrayElements (jnienv, j_ilist, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed GetIntArrayElements %p %p\n", j_ilist, ilist);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_cdata) {
        cdata = (char *)(*jnienv)->GetStringUTFChars (jnienv, j_cdata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed GetStringUTFChars %p %p\n", j_cdata, cdata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_bdata) {
        bdata = (*jnienv)->GetBooleanArrayElements (jnienv, j_bdata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed GetBooleanArrayElements %p %p\n", j_bdata, bdata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_sdata) {
        sdata = (*jnienv)->GetShortArrayElements (jnienv, j_sdata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed GetShortArrayElements %p %p\n", j_sdata, sdata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_idata) {
        idata = (*jnienv)->GetIntArrayElements (jnienv, j_idata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed GetIntArrayElements %p %p\n", j_idata, idata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_fdata) {
        fdata = (*jnienv)->GetFloatArrayElements (jnienv, j_fdata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed GetFloatArrayElements %p %p\n", j_fdata, fdata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_ddata) {
        ddata = (*jnienv)->GetDoubleArrayElements (jnienv, j_ddata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed GetDoubleArrayElements %p %p\n", j_ddata, ddata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif


/*
 * Assign 64 bit integer jlong values to C long values.
 * A C long value may be 64 bits or 32 bits, depending upon
 * the operating system.  There is only one command that
 * passes a long currently, so I only copy the first _MAX_LONG_
 * values.
 */
    if (jllist != NULL) {
        for (i=0; i<_MAX_LONG_; i++) {
            llist[i] = (long)jllist[i];
        }
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed long int conversion\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Do whatever is needed from the command.
 */
    selectable = selectable;

/*
 * If an int in C is 32 bit signed, it is the
 * same type as a jint, so I
 * just cast the ilist and idata pointers to
 * int in the call to process the command.
 */
#if INT_MAX == 2147483647
    status =
    ezx_process_command (dlist_index,
                         command_id,
                         threadid,
                         llist,
                         (int *)ilist,
                         cdata,
                         bdata,
                         sdata,
                         (int *)idata,
                         fdata,
                         ddata,
                         NULL);
/*
 * the jint is not the same size as int, so I need to
 * copy ilist and idata into int arrays.
 */
#else
    if (ilist != NULL) {
        for (i=0; i<MAX_LIST_SIZE; i++) {
            ilist2[i] = (int)ilist[i];
        }
    }
    idata2 = NULL;
    ibad = 0;
    if (j_idata != NULL) {
        ntot = GetArrayLength (jnienv, j_idata);
        idata2 = (int *)malloc (ntot * sizeof(int));
        if (idata2) {
            for (i=0; i<ntot; i++) {
                idata2[i] = (int) idata[i];
            }
        }
        else {
            ibad = 1;
        }
    }
    if (ibad == 0) {
        status =
        ezx_process_command (dlist_index,
                             command_id,
                             threadid,
                             llist,
                             ilist2,
                             cdata,
                             bdata,
                             sdata,
                             idata2,
                             fdata,
                             ddata,
                             NULL);
    }
    free (idata2);

    if (ibad == 1) {
        status = -1;
    }

#endif

    if (command_id == GTX_GET_PRIM_NUM) {
        if (ilist[0] >= 0) status = ilist[0];
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed process command\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Even though no copies of the elements were made above, the release
 * calls are needed.  Without these calls, the memory usage grows without end.
 */
    if (j_llist) {
        (*jnienv)->ReleaseLongArrayElements (jnienv, j_llist, jllist, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed ReleaseLongArrayElements %p %p\n", j_llist, jllist);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_ilist) {
        (*jnienv)->ReleaseIntArrayElements (jnienv, j_ilist, ilist, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed ReleaseIntArrayElements %p %p\n", j_ilist, ilist);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_cdata) {
        (*jnienv)->ReleaseStringUTFChars (jnienv, j_cdata, cdata);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed ReleaseStringUTFChars %p %p\n", j_cdata, cdata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_bdata) {
        (*jnienv)->ReleaseBooleanArrayElements (jnienv, j_bdata, bdata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed ReleaseBooleanArrayElements %p %p\n", j_bdata, bdata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_sdata) {
        (*jnienv)->ReleaseShortArrayElements (jnienv, j_sdata, sdata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed ReleaseShortArrayElements %p %p\n", j_sdata, sdata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_idata) {
        (*jnienv)->ReleaseIntArrayElements (jnienv, j_idata, idata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed ReleaseIntArrayElements %p %p\n", j_idata, idata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_fdata) {
        (*jnienv)->ReleaseFloatArrayElements (jnienv, j_fdata, fdata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed ReleaseFloatArrayElements %p %p\n", j_fdata, fdata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    if (j_ddata) {
        (*jnienv)->ReleaseDoubleArrayElements (jnienv, j_ddata, ddata, JNI_FALSE);
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed ReleaseDoubleArrayElements %p %p\n", j_ddata, ddata);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
    sprintf (fileline, "Return from sendCommand status = %d\n", status);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    return (jint)status;
}




/*
 *  Draw the current active display list back to the Java side.
 */
JNIEXPORT void JNICALL Java_csw_jeasyx_src_JDisplayListBase_nativeDraw
  (JNIEnv *env, jobject obj, jint j_dlist_index, jint j_threadid)
{
    jclass           cls;
    int              dlist_index, threadid;

    static int       first_call = 1;

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\n\nCalling nativeDraw\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    dlist_index = (int)j_dlist_index;
    threadid = (int)j_threadid;

    ezx_set_void_ptrs (dlist_index, (void *)env, (void *)obj);

/*
 * Find the Java class methods for sending back graphics data.
 * These are all class methods of the JDisplayListBase java
 * class.  They should be thread safe cached since they are not
 * changed after initial assignment.
 */
 
  if (first_call == 1) {

    first_call = 0;

    cls = (*env)->GetObjectClass (env, obj);

    int    sel_stat = 
    setup_return_select_method_ids(env, cls);
    if (sel_stat == -1) {
        return;
    }

  } /* end of first_call block */


  #if DEBUG_JNI_FILE
    sprintf (fileline, "Finished with method id assignment\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif


/*
 * Send the draw command to the current active display list.
 * This will send all the viewable primitives to the java side
 * via the methods defined above.
 */
    ezx_process_command (
                         dlist_index,
                         GTX_DRAW_CURRENT_VIEW,
                         threadid,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL);

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Returning from nativeDraw\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    return;

}

static void write_fill_method_params (
    float       *xy,
    float       thickness,
    float       patScale,
    int         npts,
    int         red,
    int         green,
    int         blue,
    int         alpha,
    int         pattern,
    int         frame_num,
    int         selectable)
{
    if (primfile == NULL) {
        return;
    }

    fprintf (primfile, "\n");
    fprintf (primfile, "fill prim:\n");
    fprintf (primfile, "  thick = %.3f\n", thickness);
    fprintf (primfile, "  scale = %.4f\n", patScale);
    fprintf (primfile, "  npts = %d\n", npts);
    fprintf (primfile, "  red = %d\n", red);
    fprintf (primfile, "  green = %d\n", green);
    fprintf (primfile, "  blue = %d\n", blue);
    fprintf (primfile, "  alpha = %d\n", alpha);
    fprintf (primfile, "  pattern = %d\n", pattern);
    fprintf (primfile, "  frame_num = %d\n", frame_num);
    fprintf (primfile, "  selectable = %d\n", selectable);

    int n = 0;
    for (int i=0; i<npts; i++)  {
      fprintf (primfile, "    %.1f  %.1f\n", xy[n], xy[n+1]);
      n += 2;
    }
}


void jni_call_add_fill_method (
    void    *v_jenv,
    void    *v_jobj,
    float       *xy,
    float       thickness,
    float       patScale,
    int         npts,
    int         red,
    int         green,
    int         blue,
    int         alpha,
    int         pattern,
    int         frame_num,
    int         selectable)
{
    jfloatArray        jxy;

    if (primfile) {
        write_fill_method_params (
          xy,
          thickness,
          patScale,
          npts,
          red,
          green,
          blue,
          alpha,
          pattern,
          frame_num,
          selectable);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID FillMethodID = (*jenv)->GetMethodID (jenv, jcls, "addNativeFill",
                                     "([FFFIIIIIIII)V");
    if (FillMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_fill_method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Allocate and fill the java array with the points.
 */
    jxy = (*jenv)->NewFloatArray (jenv, 2 * npts);
    if (jxy == NULL) {
        return;
    }

    (*jenv)->SetFloatArrayRegion (
        jenv,
        jxy,
        0,
        npts*2,
        xy
    );

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        FillMethodID,
        jxy,
        thickness,
        patScale,
        npts,
        red,
        green,
        blue,
        alpha,
        pattern,
        frame_num,
        selectable
    );

/*
 * Delete the java points array.
 */
    (*jenv)->DeleteLocalRef (jenv, jxy);

    return;

}




static void write_line_method_params (
    float       *xy,
    int         npts,
    int         red,
    int         green,
    int         blue,
    int         alpha,
    int         pattern,
    float       thickness,
    int         frame_num,
    int         image_id,
    int         selectable)
{
    if (primfile == NULL) {
        return;
    }

    fprintf (primfile, "\n");
    fprintf (primfile, "line prim:\n");
    fprintf (primfile, "  npts = %d\n", npts);
    fprintf (primfile, "  red = %d\n", red);
    fprintf (primfile, "  green = %d\n", green);
    fprintf (primfile, "  blue = %d\n", blue);
    fprintf (primfile, "  alpha = %d\n", alpha);
    fprintf (primfile, "  pattern = %d\n", pattern);
    fprintf (primfile, "  thick = %.3f\n", thickness);
    fprintf (primfile, "  frame_num = %d\n", frame_num);
    fprintf (primfile, "  image_id = %d\n", image_id);
    fprintf (primfile, "  selectable = %d\n", selectable);

    int n = 0;
    for (int i=0; i<npts; i++)  {
      fprintf (primfile, "    %.1f  %.1f\n", xy[n], xy[n+1]);
      n += 2;
    }
}


void jni_call_add_line_method (
    void    *v_jenv,
    void    *v_jobj,
    float       *xy,
    int         npts,
    int         red,
    int         green,
    int         blue,
    int         alpha,
    int         pattern,
    float       thickness,
    int         frame_num,
    int         image_id,
    int         selectable
) {
    jfloatArray        jxy;

    if (primfile) {
        write_line_method_params (
          xy,
          npts,
          red,
          green,
          blue,
          alpha,
          pattern,
          thickness,
          frame_num,
          image_id,
          selectable);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID LineMethodID = (*jenv)->GetMethodID (jenv, jcls, "addNativeLine",
                                     "([FIIIIIIFIII)V");
    if (LineMethodID == NULL) {
        return;
    }


  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_line_method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Allocate and fill the java array with the points.
 */
    jxy = (*jenv)->NewFloatArray (jenv, 2 * npts);
    if (jxy == NULL) {
        return;
    }

    (*jenv)->SetFloatArrayRegion (
        jenv,
        jxy,
        0,
        npts*2,
        xy
    );

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        LineMethodID,
        jxy,
        npts,
        red,
        green,
        blue,
        alpha,
        pattern,
        thickness,
        frame_num,
        image_id,
        selectable
    );

/*
 * Delete the java points array.
 */
    (*jenv)->DeleteLocalRef (jenv, jxy);

    return;

}


static void write_text_method_params (
    float       x,
    float       y,
    char        *text,
    int         red,
    int         green,
    int         blue,
    int         alpha,
    float       angle,
    float       size,
    int         font,
    int         frame_num,
    int         image_id,
    int         selectable)
{
    if (primfile == NULL) {
        return;
    }

    fprintf (primfile, "\n");
    fprintf (primfile, "text prim:\n");
    fprintf (primfile, "  x = %.1f   y = %.1f\n", x, y);
    fprintf (primfile, "  text = %s\n", text);
    fprintf (primfile, "  red = %d\n", red);
    fprintf (primfile, "  green = %d\n", green);
    fprintf (primfile, "  blue = %d\n", blue);
    fprintf (primfile, "  alpha = %d\n", alpha);
    fprintf (primfile, "  angle = %.3f\n", angle);
    fprintf (primfile, "  size = %.2f\n", size);
    fprintf (primfile, "  font = %d\n", font);
    fprintf (primfile, "  frame_num = %d\n", frame_num);
    fprintf (primfile, "  image_id = %d\n", image_id);
    fprintf (primfile, "  selectable = %d\n", selectable);

}

void jni_call_add_text_method (
    void    *v_jenv,
    void    *v_jobj,
    float       x,
    float       y,
    char        *text,
    int         red,
    int         green,
    int         blue,
    int         alpha,
    float       angle,
    float       size,
    int         font,
    int         frame_num,
    int         image_id,
    int         selectable
) {
    jstring           jtext;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID TextMethodID = (*jenv)->GetMethodID (jenv, jcls, "addNativeText",
                                     "(FFLjava/lang/String;IIIIFFIIII)V");
    if (TextMethodID == NULL) {
        return;
    }

    if (primfile != NULL) {
        write_text_method_params (
          x,
          y,
          text,
          red,
          green,
          blue,
          alpha,
          angle,
          size,
          font,
          frame_num,
          image_id,
          selectable
        );
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_text_method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Allocate and fill the java string.
 */
    jtext = (*jenv)->NewStringUTF (jenv, text);
    if (jtext == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        TextMethodID,
        x,
        y,
        jtext,
        red,
        green,
        blue,
        alpha,
        angle,
        size,
        font,
        frame_num,
        image_id,
        selectable
    );

/*
 * Delete the java char array.
 */
    (*jenv)->DeleteLocalRef (jenv, jtext);

    return;

}


static void write_arc_method_params (
    float       x,
    float       y,
    float       r1,
    float       r2,
    float       ang1,
    float       ang2,
    int         closure,
    int         red,
    int         green,
    int         blue,
    int         alpha,
    float       thickness,
    float       angle,
    int         frame_num,
    int         selectable)
{
    if (primfile == NULL) {
        return;
    }

    fprintf (primfile, "\n");
    fprintf (primfile, "arc prim:\n");
    fprintf (primfile, "  x = %.1f   y = %.1f\n", x, y);
    fprintf (primfile, "  r1 = %.1f   r2 = %.1f\n", r1, r2);
    fprintf (primfile, "  ang1 = %.1f   ang2 = %.1f\n", ang1, ang2);
    fprintf (primfile, "  closure = %d\n", closure);
    fprintf (primfile, "  red = %d\n", red);
    fprintf (primfile, "  green = %d\n", green);
    fprintf (primfile, "  blue = %d\n", blue);
    fprintf (primfile, "  alpha = %d\n", alpha);
    fprintf (primfile, "  thickness = %.3f\n", thickness);
    fprintf (primfile, "  angle = %.3f\n", angle);
    fprintf (primfile, "  frame_num = %d\n", frame_num);
    fprintf (primfile, "  selectable = %d\n", selectable);

}

void jni_call_add_arc_method (
    void    *v_jenv,
    void    *v_jobj,
    float       x,
    float       y,
    float       r1,
    float       r2,
    float       ang1,
    float       ang2,
    int         closure,
    int         red,
    int         green,
    int         blue,
    int         alpha,
    float       thickness,
    float       angle,
    int         frame_num,
    int         selectable
) {

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID ArcMethodID = (*jenv)->GetMethodID (jenv, jcls, "addNativeArc",
                                "(FFFFFFIIIIIFFII)V");
    if (ArcMethodID == NULL) {
        return;
    }

    if (primfile != NULL) {
        write_arc_method_params (
          x,
          y,
          r1,
          r2,
          ang1,
          ang2,
          closure,
          red,
          green,
          blue,
          alpha,
          thickness,
          angle,
          frame_num,
          selectable
        );
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_arc_method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        ArcMethodID,
        x,
        y,
        r1,
        r2,
        ang1,
        ang2,
        closure,
        red,
        green,
        blue,
        alpha,
        thickness,
        angle,
        frame_num,
        selectable
    );

    return;

}



static void write_filled_arc_method_params (
    float       x,
    float       y,
    float       r1,
    float       r2,
    float       ang1,
    float       ang2,
    int         closure,
    int         red,
    int         green,
    int         blue,
    int         alpha,
    float       thickness,
    float       angle,
    int         pattern,
    int         frame_num,
    int         selectable)
{
    if (primfile == NULL) {
        return;
    }

    fprintf (primfile, "\n");
    fprintf (primfile, "arc prim:\n");
    fprintf (primfile, "  x = %.1f   y = %.1f\n", x, y);
    fprintf (primfile, "  r1 = %.1f   r2 = %.1f\n", r1, r2);
    fprintf (primfile, "  ang1 = %.1f   ang2 = %.1f\n", ang1, ang2);
    fprintf (primfile, "  closure = %d\n", closure);
    fprintf (primfile, "  red = %d\n", red);
    fprintf (primfile, "  green = %d\n", green);
    fprintf (primfile, "  blue = %d\n", blue);
    fprintf (primfile, "  alpha = %d\n", alpha);
    fprintf (primfile, "  thickness = %.3f\n", thickness);
    fprintf (primfile, "  angle = %.3f\n", angle);
    fprintf (primfile, "  pattern = %d\n", pattern);
    fprintf (primfile, "  frame_num = %d\n", frame_num);
    fprintf (primfile, "  selectable = %d\n", selectable);

}



void jni_call_add_filled_arc_method (
    void    *v_jenv,
    void    *v_jobj,
    float       x,
    float       y,
    float       r1,
    float       r2,
    float       ang1,
    float       ang2,
    int         closure,
    int         red,
    int         green,
    int         blue,
    int         alpha,
    float       thickness,
    float       angle,
    int         pattern,
    int         frame_num,
    int         selectable
) {

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID FilledArcMethodID = (*jenv)->GetMethodID (jenv, jcls, "addNativeFilledArc",
                                     "(FFFFFFIIIIIFFIII)V");
    if (FilledArcMethodID == NULL) {
        return;
    }

    if (primfile != NULL) {
        write_filled_arc_method_params (
          x,
          y,
          r1,
          r2,
          ang1,
          ang2,
          closure,
          red,
          green,
          blue,
          alpha,
          thickness,
          angle,
          pattern,
          frame_num,
          selectable
        );
    }



  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_filled_arc_method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        FilledArcMethodID,
        x,
        y,
        r1,
        r2,
        ang1,
        ang2,
        closure,
        red,
        green,
        blue,
        alpha,
        thickness,
        angle,
        pattern,
        frame_num,
        selectable
    );

    return;

}



void jni_call_add_image_method (
    void    *v_jenv,
    void    *v_jobj,
    float           x1,
    float           y1,
    float           x2,
    float           y2,
    int             ncol,
    int             nrow,
    unsigned char   *red,
    unsigned char   *green,
    unsigned char   *blue,
    unsigned char   *trans,
    int             frame_num,
    int             has_lines,
    int             image_id,
    int             selectable
) {
    jbyteArray      jred, jgreen, jblue, jtrans;
    int             nt;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID ImageMethodID = (*jenv)->GetMethodID (jenv, jcls, "addNativeImage",
                                      "(FFFFII[B[B[B[BIIII)V");
    if (ImageMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_image_method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    nt = ncol * nrow;
    jred = NULL;
    jgreen = NULL;
    jblue = NULL;
    jtrans = NULL;

/*
 * Allocate space for the java color component arrays.
 */
    jred = (*jenv)->NewByteArray (jenv, nt);
    if (jred == NULL) {
        return;
    }

    jgreen = (*jenv)->NewByteArray (jenv, nt);
    if (jgreen == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jred);
        return;
    }

    jblue = (*jenv)->NewByteArray (jenv, nt);
    if (jblue == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jred);
        (*jenv)->DeleteLocalRef (jenv, jgreen);
        return;
    }

    jtrans = (*jenv)->NewByteArray (jenv, nt);
    if (jtrans == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jred);
        (*jenv)->DeleteLocalRef (jenv, jgreen);
        (*jenv)->DeleteLocalRef (jenv, jblue);
        return;
    }

/*
 * Fill in the java color component arrays.
 */
    (*jenv)->SetByteArrayRegion (
        jenv,
        jred,
        0,
        nt,
        (jbyte *)red
    );

    (*jenv)->SetByteArrayRegion (
        jenv,
        jgreen,
        0,
        nt,
        (jbyte *)green
    );

    (*jenv)->SetByteArrayRegion (
        jenv,
        jblue,
        0,
        nt,
        (jbyte *)blue
    );

    (*jenv)->SetByteArrayRegion (
        jenv,
        jtrans,
        0,
        nt,
        (jbyte *)trans
    );

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        ImageMethodID,
        x1,
        y1,
        x2,
        y2,
        ncol,
        nrow,
        jred,
        jgreen,
        jblue,
        jtrans,
        frame_num,
        has_lines,
        image_id,
        selectable
    );

/*
 * Delete the java color component arrays.
 */
    (*jenv)->DeleteLocalRef (jenv, jred);
    (*jenv)->DeleteLocalRef (jenv, jgreen);
    (*jenv)->DeleteLocalRef (jenv, jblue);
    (*jenv)->DeleteLocalRef (jenv, jtrans);

    return;

}





void jni_call_add_frame_method (
    void    *v_jenv,
    void    *v_jobj,
    float       x1,
    float       y1,
    float       x2,
    float       y2,
    double      fx1,
    double      fy1,
    double      fx2,
    double      fy2,
    int         borderflag,
    int         clipflag,
    int         scaleable,
    int         scale_to_attach_frame,
    int         frame_num,
    char        *fname
) {
    jstring            jfname;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID FrameMethodID = (*jenv)->GetMethodID (jenv, jcls, "addNativeFrame",
                                  "(FFFFDDDDIIIIILjava/lang/String;)V");
    if (FrameMethodID == NULL) {
        return;
    }

    jfname = NULL;
    if (fname != NULL) {
        jfname = (*jenv)->NewStringUTF (jenv, fname);
        if (jfname == NULL) {
            return;
        }
    }

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        FrameMethodID,
        x1,
        y1,
        x2,
        y2,
        fx1,
        fy1,
        fx2,
        fy2,
        borderflag,
        clipflag,
        scaleable,
        scale_to_attach_frame,
        frame_num,
        jfname
    );

    return;

}




JNIEXPORT jint JNICALL Java_csw_jeasyx_src_JDisplayListBase_nativePick
  (JNIEnv *env,
   jobject obj,
   jint j_dlist_index,
   jint j_threadid,
   jint j_frame_num,
   jint j_ix,
   jint j_iy)
{
    int              dlist_index;
    int              threadid;
    int              status;
    jint             ilist[10];

    dlist_index = (int)j_dlist_index;
    threadid = (int)j_threadid;

    ezx_set_void_ptrs (dlist_index, (void *)env, (void *)obj);

/*
 * Fill the ilist array.
 */
    ilist[0] = j_frame_num;
    ilist[1] = j_ix;
    ilist[2] = j_iy;

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Completed GetIntArrayElements %p %p\n", j_ilist, ilist);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif


/*
 * Send the pick command to the current active display list.
 */

/*
 * If an int in C is 32 bit signed, it is the
 * same type as a jint, so I
 * just cast the ilist and idata pointers to
 * int in the call to process the command.
 */
#if INT_MAX == 2147483647
    status =
    ezx_process_command (dlist_index,
                         GTX_PICKPRIM,
                         threadid,
                         NULL,
                         (int *)ilist,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL);
/*
 * the jint is not the same size as int, so I need to
 * copy ilist into an int array.
 */
#else
    memset (ilist2, 0, 10 * sizeof(int));
    if (ilist != NULL) {
        for (i=0; i<10; i++) {
            ilist2[i] = (int)ilist[i];
        }
    }
    status =
    ezx_process_command (dlist_index,
                         GTX_PICKPRIM,
                         threadid,
                         NULLllist,
                         ilist2,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL);
#endif

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Returning from nativeDraw\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    return status;

}




static int setup_return_select_method_ids(JNIEnv  *env, jclass  cls) {

/*
 * Find the Java class methods for sending back the selected data.
 */


    return 1;
}

/*
 * Draw the selected objects from the current active display list
 * back to the Java side.
 */
JNIEXPORT void JNICALL Java_csw_jeasyx_src_JDisplayListBase_nativeDrawSelected
  (JNIEnv *env, jobject obj, jint j_dlist_index, jint j_threadid)
{
    int              dlist_index;
    int              threadid;

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\n\nCalling nativeDraw\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    dlist_index = (int)j_dlist_index;

    ezx_set_void_ptrs (dlist_index, (void *)env, (void *)obj);

    threadid = (int)j_threadid;

/*
 * Find the Java class methods for sending back graphics data.
 */

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Finished with method id assignment\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Send the draw command to the current active display list.
 * This will send all the viewable primitives to the java side
 * via the methods defined above.
 */
    ezx_process_command (
                         dlist_index,
                         GTX_DRAW_SELECTED,
                         threadid,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL);

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Returning from nativeSelectDraw\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    return;

}



void jni_call_add_selected_fill (
    void    *v_jenv,
    void    *v_jobj,
    int             selectable_index,
    double          *xp,
    double          *yp,
    int             *npts_in,
    int             ncomp,
    double          thick,
    double          patscale,
    double          dashscale,
    int             red_fill,
    int             green_fill,
    int             blue_fill,
    int             alpha_fill,
    int             red_pat,
    int             green_pat,
    int             blue_pat,
    int             alpha_pat,
    int             red_border,
    int             green_border,
    int             blue_border,
    int             alpha_border,
    char            *fname,
    char            *lname,
    char            *iname,
    int             fillpat,
    int             linepat,
    int             native_index
) {
    jstring            jfname,
                       jlname,
                       jiname;

    jdoubleArray       jxp,
                       jyp;
    jintArray          jnpts;

    int                i, nptot;
    jint               *npts;

#if INT_MAX != 2147483647
    jint               np32[1000];
  #if INT_MAX  <  2147483647
    #error The int data type with this compiler is less than 32 bits, ABORT.
  #endif
#endif

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SelectFillMethodID = (*jenv)->GetMethodID (jenv, jcls, "addSelectedFill",
    "(I[D[D[IIDDDIIIIIIIIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;III)V");

    if (SelectFillMethodID == NULL) {
        return;
    }

/*
 * Allocate and fill the java strings.
 */
    jfname = NULL;
    if (fname != NULL) {
        jfname = (*jenv)->NewStringUTF (jenv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*jenv)->NewStringUTF (jenv, lname);
        if (jlname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*jenv)->NewStringUTF (jenv, iname);
        if (jiname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            return;
        }
    }

    nptot = 0;
    for (i=0; i<ncomp; i++) {
        nptot += npts_in[i];
    }

#if INT_MAX != 2147483647
    if (ncomp > 1000) ncomp = 1000;
    for (i=0; i<ncomp; i++) {
        np32[i] = (jint)npts_in[i];
    }
    npts = np32;
#else
    npts = (jint *)npts_in;
#endif

/*
 * Allocate and fill the java arrays with the points.
 */
    jxp = (*jenv)->NewDoubleArray (jenv, nptot);
    if (jxp == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jfname);
        (*jenv)->DeleteLocalRef (jenv, jlname);
        (*jenv)->DeleteLocalRef (jenv, jiname);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        jxp,
        0,
        nptot,
        xp
    );

    jyp = (*jenv)->NewDoubleArray (jenv, nptot);
    if (jyp == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jfname);
        (*jenv)->DeleteLocalRef (jenv, jlname);
        (*jenv)->DeleteLocalRef (jenv, jiname);
        (*jenv)->DeleteLocalRef (jenv, jxp);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        jyp,
        0,
        nptot,
        yp
    );

    jnpts = (*jenv)->NewIntArray (jenv, ncomp);
    if (jnpts == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jfname);
        (*jenv)->DeleteLocalRef (jenv, jlname);
        (*jenv)->DeleteLocalRef (jenv, jiname);
        (*jenv)->DeleteLocalRef (jenv, jxp);
        (*jenv)->DeleteLocalRef (jenv, jyp);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        jnpts,
        0,
        ncomp,
        npts
    );

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Correct for -1 color values.  If any color value is less than zero
 * that means the part of the primitive pertaining to that color
 * value is not drawn.  In these cases, all 4 color values for that
 * part of the primitive are set to zero.
 */
    if (red_fill < 0  ||  green_fill < 0  || blue_fill < 0) {
        red_fill = 0;
        green_fill = 0;
        blue_fill = 0;
        alpha_fill = 0;
    }

    if (red_pat < 0  ||  green_pat < 0  || blue_pat < 0) {
        red_pat = 0;
        green_pat = 0;
        blue_pat = 0;
        alpha_pat = 0;
    }

    if (red_border < 0  ||  green_border < 0  || blue_border < 0) {
        red_border = 0;
        green_border = 0;
        blue_border = 0;
        alpha_border = 0;
    }

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SelectFillMethodID,
        (jint)selectable_index,
        jxp,
        jyp,
        jnpts,
        (jint)ncomp,
        thick,
        patscale,
        dashscale,
        (jint)red_fill,
        (jint)green_fill,
        (jint)blue_fill,
        (jint)alpha_fill,
        (jint)red_pat,
        (jint)green_pat,
        (jint)blue_pat,
        (jint)alpha_pat,
        (jint)red_border,
        (jint)green_border,
        (jint)blue_border,
        (jint)alpha_border,
        jfname,
        jlname,
        jiname,
        (jint)fillpat,
        (jint)linepat,
        (jint)native_index
    );

/*
 * Delete local references to arrays and strings.
 */
    (*jenv)->DeleteLocalRef (jenv, jfname);
    (*jenv)->DeleteLocalRef (jenv, jlname);
    (*jenv)->DeleteLocalRef (jenv, jiname);
    (*jenv)->DeleteLocalRef (jenv, jxp);
    (*jenv)->DeleteLocalRef (jenv, jyp);
    (*jenv)->DeleteLocalRef (jenv, jnpts);

    return;
}



void jni_call_add_selected_line (
    void    *v_jenv,
    void    *v_jobj,
    int             selectable_index,
    double          *xp,
    double          *yp,
    int             npts,
    double          thick,
    double          dashscale,
    int             red_line,
    int             green_line,
    int             blue_line,
    int             alpha_line,
    char            *fname,
    char            *lname,
    char            *iname,
    int             linepat,
    int             symbol,
    int             arrow_style,
    int             native_index) 
{
    jstring            jfname,
                       jlname,
                       jiname;

    jdoubleArray       jxp,
                       jyp;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SelectLineMethodID = (*jenv)->GetMethodID (jenv, jcls, "addSelectedLine",
    "(I[D[DIDDIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;IIII)V");

    if (SelectLineMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_selected_line\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Allocate and fill the java strings.
 */
    jfname = NULL;
    if (fname != NULL) {
        jfname = (*jenv)->NewStringUTF (jenv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*jenv)->NewStringUTF (jenv, lname);
        if (jlname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*jenv)->NewStringUTF (jenv, iname);
        if (jiname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            return;
        }
    }

/*
 * Allocate and fill the java arrays with the points.
 */
    jxp = (*jenv)->NewDoubleArray (jenv, npts);
    if (jxp == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jfname);
        (*jenv)->DeleteLocalRef (jenv, jlname);
        (*jenv)->DeleteLocalRef (jenv, jiname);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        jxp,
        0,
        npts,
        xp
    );

    jyp = (*jenv)->NewDoubleArray (jenv, npts);
    if (jyp == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jfname);
        (*jenv)->DeleteLocalRef (jenv, jlname);
        (*jenv)->DeleteLocalRef (jenv, jiname);
        (*jenv)->DeleteLocalRef (jenv, jxp);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        jyp,
        0,
        npts,
        yp
    );

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Correct for -1 color values.  If any color value is less than zero
 * that means the part of the primitive pertaining to that color
 * value is not drawn.  In these cases, all 4 color values for that
 * part of the primitive are set to zero.
 */
    if (red_line < 0  ||  green_line < 0  || blue_line < 0) {
        red_line = 0;
        green_line = 0;
        blue_line = 0;
        alpha_line = 0;
    }

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SelectLineMethodID,
        (jint)selectable_index,
        jxp,
        jyp,
        (jint)npts,
        thick,
        dashscale,
        (jint)red_line,
        (jint)green_line,
        (jint)blue_line,
        (jint)alpha_line,
        jfname,
        jlname,
        jiname,
        (jint)linepat,
        (jint)symbol,
        (jint)arrow_style,
        native_index
    );

/*
 * Delete local references to arrays and strings.
 */
    (*jenv)->DeleteLocalRef (jenv, jfname);
    (*jenv)->DeleteLocalRef (jenv, jlname);
    (*jenv)->DeleteLocalRef (jenv, jiname);
    (*jenv)->DeleteLocalRef (jenv, jxp);
    (*jenv)->DeleteLocalRef (jenv, jyp);

    return;
}



void jni_call_add_selected_contour (
    void    *v_jenv,
    void    *v_jobj,
    int             selectable_index,
    double          *xp,
    double          *yp,
    int             npts,
    double          zval,
    double          thick,
    int             major,
    int             red_cont,
    int             green_cont,
    int             blue_cont,
    int             alpha_cont,
    char            *label,
    int             label_font,
    double          label_size,
    double          label_space,
    double          tick_len,
    double          tick_space,
    int             tick_dir,
    char            *fname,
    char            *lname,
    char            *iname,
    char            *sname,
    int             native_index)
{
    jstring            jfname,
                       jlname,
                       jiname;
    jstring            jlabel,
                       jsname;

    jdoubleArray       jxp,
                       jyp;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SelectContourMethodID = (*jenv)->GetMethodID (jenv, jcls, "addSelectedContour",
    "(I[D[DIDDZIIIILjava/lang/String;IDDDDILjava/lang/String;"
    "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

    if (SelectContourMethodID == NULL) {
        return;
    }

/*
 * Allocate and fill the java strings.
 */
    jfname = NULL;
    if (fname != NULL) {
        jfname = (*jenv)->NewStringUTF (jenv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*jenv)->NewStringUTF (jenv, lname);
        if (jlname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*jenv)->NewStringUTF (jenv, iname);
        if (jiname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            return;
        }
    }

    jsname = NULL;
    if (sname != NULL) {
        jsname = (*jenv)->NewStringUTF (jenv, sname);
        if (jsname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            (*jenv)->DeleteLocalRef (jenv, jiname);
            return;
        }
    }

    jlabel = NULL;
    if (label != NULL) {
        jlabel = (*jenv)->NewStringUTF (jenv, label);
        if (jlabel == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            (*jenv)->DeleteLocalRef (jenv, jiname);
            (*jenv)->DeleteLocalRef (jenv, jsname);
            return;
        }
    }

/*
 * Allocate and fill the java arrays with the points.
 */
    jxp = (*jenv)->NewDoubleArray (jenv, npts);
    if (jxp == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jfname);
        (*jenv)->DeleteLocalRef (jenv, jlname);
        (*jenv)->DeleteLocalRef (jenv, jiname);
        (*jenv)->DeleteLocalRef (jenv, jsname);
        (*jenv)->DeleteLocalRef (jenv, jlabel);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        jxp,
        0,
        npts,
        xp
    );

    jyp = (*jenv)->NewDoubleArray (jenv, npts);
    if (jyp == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jfname);
        (*jenv)->DeleteLocalRef (jenv, jlname);
        (*jenv)->DeleteLocalRef (jenv, jiname);
        (*jenv)->DeleteLocalRef (jenv, jsname);
        (*jenv)->DeleteLocalRef (jenv, jlabel);
        (*jenv)->DeleteLocalRef (jenv, jxp);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        jyp,
        0,
        npts,
        yp
    );

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Correct for -1 color values.  If any color value is less than zero
 * that means the part of the primitive pertaining to that color
 * value is not drawn.  In these cases, all 4 color values for that
 * part of the primitive are set to zero.
 */
    if (red_cont < 0  ||  green_cont < 0  || blue_cont < 0) {
        red_cont = 0;
        green_cont = 0;
        blue_cont = 0;
        alpha_cont = 0;
    }

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SelectContourMethodID,
        (jint)selectable_index,
        jxp,
        jyp,
        (jint)npts,
        zval,
        thick,
        (jboolean)major,
        (jint)red_cont,
        (jint)green_cont,
        (jint)blue_cont,
        (jint)alpha_cont,
        jlabel,
        label_font,
        label_size,
        label_space,
        tick_len,
        tick_space,
        tick_dir,
        jfname,
        jlname,
        jiname,
        jsname,
        native_index
    );

/*
 * Delete local references to arrays and strings.
 */
    (*jenv)->DeleteLocalRef (jenv, jfname);
    (*jenv)->DeleteLocalRef (jenv, jlname);
    (*jenv)->DeleteLocalRef (jenv, jiname);
    (*jenv)->DeleteLocalRef (jenv, jsname);
    (*jenv)->DeleteLocalRef (jenv, jlabel);
    (*jenv)->DeleteLocalRef (jenv, jxp);
    (*jenv)->DeleteLocalRef (jenv, jyp);

    return;
}



void jni_call_add_selected_rectangle (
    void    *v_jenv,
    void    *v_jobj,
    int             selectable_index,
    double          xc,
    double          yc,
    double          xr,
    double          yr,
    double          crad,
    double          rang,
    double          thick,
    double          patscale,
    double          dashscale,
    int             red_fill,
    int             green_fill,
    int             blue_fill,
    int             alpha_fill,
    int             red_pat,
    int             green_pat,
    int             blue_pat,
    int             alpha_pat,
    int             red_border,
    int             green_border,
    int             blue_border,
    int             alpha_border,
    char            *fname,
    char            *lname,
    char            *iname,
    int             fillpat,
    int             linepat,
    int             native_index)
{
    jstring            jfname,
                       jlname,
                       jiname;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SelectRectMethodID = (*jenv)->GetMethodID
     (jenv, jcls, "addSelectedRectangle",
    "(IDDDDDDDDDIIIIIIIIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;III)V");

    if (SelectRectMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_selected_rectangle\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Allocate and fill the java strings.
 */
    jfname = NULL;
    if (fname != NULL) {
        jfname = (*jenv)->NewStringUTF (jenv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*jenv)->NewStringUTF (jenv, lname);
        if (jlname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*jenv)->NewStringUTF (jenv, iname);
        if (jiname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            return;
        }
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Correct for -1 color values.  If any color value is less than zero
 * that means the part of the primitive pertaining to that color
 * value is not drawn.  In these cases, all 4 color values for that
 * part of the primitive are set to zero.
 */
    if (red_fill < 0  ||  green_fill < 0  || blue_fill < 0) {
        red_fill = 0;
        green_fill = 0;
        blue_fill = 0;
        alpha_fill = 0;
    }

    if (red_pat < 0  ||  green_pat < 0  || blue_pat < 0) {
        red_pat = 0;
        green_pat = 0;
        blue_pat = 0;
        alpha_pat = 0;
    }

    if (red_border < 0  ||  green_border < 0  || blue_border < 0) {
        red_border = 0;
        green_border = 0;
        blue_border = 0;
        alpha_border = 0;
    }

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SelectRectMethodID,
        (jint)selectable_index,
        xc,
        yc,
        xr,
        yr,
        crad,
        rang,
        thick,
        patscale,
        dashscale,
        (jint)red_fill,
        (jint)green_fill,
        (jint)blue_fill,
        (jint)alpha_fill,
        (jint)red_pat,
        (jint)green_pat,
        (jint)blue_pat,
        (jint)alpha_pat,
        (jint)red_border,
        (jint)green_border,
        (jint)blue_border,
        (jint)alpha_border,
        jfname,
        jlname,
        jiname,
        (jint)fillpat,
        (jint)linepat,
        (jint)native_index
    );

/*
 * Delete local references to arrays and strings.
 */
    (*jenv)->DeleteLocalRef (jenv, jfname);
    (*jenv)->DeleteLocalRef (jenv, jlname);
    (*jenv)->DeleteLocalRef (jenv, jiname);

    return;
}



void jni_call_add_selected_arc (
    void    *v_jenv,
    void    *v_jobj,
    int             selectable_index,
    double          xc,
    double          yc,
    double          xr,
    double          yr,
    double          ang1,
    double          anglen,
    double          rang,
    int             closure,
    double          thick,
    double          patscale,
    double          dashscale,
    int             red_fill,
    int             green_fill,
    int             blue_fill,
    int             alpha_fill,
    int             red_pat,
    int             green_pat,
    int             blue_pat,
    int             alpha_pat,
    int             red_border,
    int             green_border,
    int             blue_border,
    int             alpha_border,
    char            *fname,
    char            *lname,
    char            *iname,
    int             fillpat,
    int             linepat,
    int             native_index)
{
    jstring            jfname,
                       jlname,
                       jiname;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SelectArcMethodID = (*jenv)->GetMethodID
     (jenv, jcls, "addSelectedArc",
    "(IDDDDDDDIDDDIIIIIIIIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;III)V");

    if (SelectArcMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_selected_arc\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Allocate and fill the java strings.
 */
    jfname = NULL;
    if (fname != NULL) {
        jfname = (*jenv)->NewStringUTF (jenv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*jenv)->NewStringUTF (jenv, lname);
        if (jlname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*jenv)->NewStringUTF (jenv, iname);
        if (jiname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            return;
        }
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Correct for -1 color values.  If any color value is less than zero
 * that means the part of the primitive pertaining to that color
 * value is not drawn.  In these cases, all 4 color values for that
 * part of the primitive are set to zero.
 */
    if (red_fill < 0  ||  green_fill < 0  || blue_fill < 0) {
        red_fill = 0;
        green_fill = 0;
        blue_fill = 0;
        alpha_fill = 0;
    }

    if (red_pat < 0  ||  green_pat < 0  || blue_pat < 0) {
        red_pat = 0;
        green_pat = 0;
        blue_pat = 0;
        alpha_pat = 0;
    }

    if (red_border < 0  ||  green_border < 0  || blue_border < 0) {
        red_border = 0;
        green_border = 0;
        blue_border = 0;
        alpha_border = 0;
    }

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SelectArcMethodID,
        (jint)selectable_index,
        xc,
        yc,
        xr,
        yr,
        ang1,
        anglen,
        rang,
        (jint)closure,
        thick,
        patscale,
        dashscale,
        (jint)red_fill,
        (jint)green_fill,
        (jint)blue_fill,
        (jint)alpha_fill,
        (jint)red_pat,
        (jint)green_pat,
        (jint)blue_pat,
        (jint)alpha_pat,
        (jint)red_border,
        (jint)green_border,
        (jint)blue_border,
        (jint)alpha_border,
        jfname,
        jlname,
        jiname,
        (jint)fillpat,
        (jint)linepat,
        (jint)native_index
    );

/*
 * Delete local references to arrays and strings.
 */
    (*jenv)->DeleteLocalRef (jenv, jfname);
    (*jenv)->DeleteLocalRef (jenv, jlname);
    (*jenv)->DeleteLocalRef (jenv, jiname);

    return;
}



void jni_call_add_selected_text (
    void    *v_jenv,
    void    *v_jobj,
    int             selectable_index,
    double          xp,
    double          yp,
    int             anchor,
    double          thick,
    double          bgthick,
    double          angle,
    double          size,
    double          xoff,
    double          yoff,
    int             red_text,
    int             green_text,
    int             blue_text,
    int             alpha_text,
    int             red_fill,
    int             green_fill,
    int             blue_fill,
    int             alpha_fill,
    int             red_bgfill,
    int             green_bgfill,
    int             blue_bgfill,
    int             alpha_bgfill,
    int             red_bgborder,
    int             green_bgborder,
    int             blue_bgborder,
    int             alpha_bgborder,
    int             bgflag,
    int             font,
    char            *textdata,
    char            *fname,
    char            *lname,
    char            *iname,
    int             native_index)
{
    jstring            jfname,
                       jlname,
                       jiname;
    jstring            jtextdata;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SelectTextMethodID = (*jenv)->GetMethodID (jenv, jcls, "addSelectedText",
    "(IDDIDDDDDDIIIIIIIIIIIIIIIIIILjava/lang/String;"
    "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

    if (SelectTextMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_selected_text\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Allocate and fill the java strings.
 */
    jfname = NULL;
    if (fname != NULL) {
        jfname = (*jenv)->NewStringUTF (jenv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*jenv)->NewStringUTF (jenv, lname);
        if (jlname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*jenv)->NewStringUTF (jenv, iname);
        if (jiname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            return;
        }
    }

    jtextdata = NULL;
    if (textdata != NULL) {
        jtextdata = (*jenv)->NewStringUTF (jenv, textdata);
        if (jtextdata == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            (*jenv)->DeleteLocalRef (jenv, jiname);
            return;
        }
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Correct for -1 color values.  If any color value is less than zero
 * that means the part of the primitive pertaining to that color
 * value is not drawn.  In these cases, all 4 color values for that
 * part of the primitive are set to zero.
 */
    if (red_text < 0  ||  green_text < 0  || blue_text < 0) {
        red_text = 0;
        green_text = 0;
        blue_text = 0;
        alpha_text = 0;
    }

    if (red_fill < 0  ||  green_fill < 0  || blue_fill < 0) {
        red_fill = 0;
        green_fill = 0;
        blue_fill = 0;
        alpha_fill = 0;
    }

    if (red_bgfill < 0  ||  green_bgfill < 0  || blue_bgfill < 0) {
        red_bgfill = 0;
        green_bgfill = 0;
        blue_bgfill = 0;
        alpha_bgfill = 0;
    }

    if (red_bgborder < 0  ||  green_bgborder < 0  || blue_bgborder < 0) {
        red_bgborder = 0;
        green_bgborder = 0;
        blue_bgborder = 0;
        alpha_bgborder = 0;
    }

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SelectTextMethodID,
        (jint)selectable_index,
        xp,
        yp,
        anchor,
        thick,
        bgthick,
        angle,
        size,
        xoff,
        yoff,
        (jint)red_text,
        (jint)green_text,
        (jint)blue_text,
        (jint)alpha_text,
        (jint)red_fill,
        (jint)green_fill,
        (jint)blue_fill,
        (jint)alpha_fill,
        (jint)red_bgfill,
        (jint)green_bgfill,
        (jint)blue_bgfill,
        (jint)alpha_bgfill,
        (jint)red_bgborder,
        (jint)green_bgborder,
        (jint)blue_bgborder,
        (jint)alpha_bgborder,
        bgflag,
        font,
        jtextdata,
        jfname,
        jlname,
        jiname,
        (jint)native_index
    );

/*
 * Delete local references to arrays and strings.
 */
    (*jenv)->DeleteLocalRef (jenv, jfname);
    (*jenv)->DeleteLocalRef (jenv, jlname);
    (*jenv)->DeleteLocalRef (jenv, jiname);
    (*jenv)->DeleteLocalRef (jenv, jtextdata);

    return;
}



void jni_call_add_selected_symb (
    void    *v_jenv,
    void    *v_jobj,
    int             selectable_index,
    double          xp,
    double          yp,
    int             number,
    double          thick,
    double          angle,
    double          size,
    int             red_symb,
    int             green_symb,
    int             blue_symb,
    int             alpha_symb,
    char            *fname,
    char            *lname,
    char            *iname,
    char            *sname,
    int             native_index)
{
    jstring            jfname,
                       jlname,
                       jiname;
    jstring            jsname;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SelectSymbMethodID = (*jenv)->GetMethodID (jenv, jcls, "addSelectedSymb",
    "(IDDIDDDIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

    if (SelectSymbMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_selected_symb\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Allocate and fill the java strings.
 */
    jfname = NULL;
    if (fname != NULL) {
        jfname = (*jenv)->NewStringUTF (jenv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*jenv)->NewStringUTF (jenv, lname);
        if (jlname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*jenv)->NewStringUTF (jenv, iname);
        if (jiname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            return;
        }
    }

    jsname = NULL;
    if (sname != NULL) {
        jsname = (*jenv)->NewStringUTF (jenv, sname);
        if (jsname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            (*jenv)->DeleteLocalRef (jenv, jiname);
            return;
        }
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Calling the actual java method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Correct for -1 color values.  If any color value is less than zero
 * that means the part of the primitive pertaining to that color
 * value is not drawn.  In these cases, all 4 color values for that
 * part of the primitive are set to zero.
 */
    if (red_symb < 0  ||  green_symb < 0  || blue_symb < 0) {
        red_symb = 0;
        green_symb = 0;
        blue_symb = 0;
        alpha_symb = 0;
    }

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SelectSymbMethodID,
        (jint)selectable_index,
        xp,
        yp,
        number,
        thick,
        angle,
        size,
        (jint)red_symb,
        (jint)green_symb,
        (jint)blue_symb,
        (jint)alpha_symb,
        jfname,
        jlname,
        jiname,
        jsname,
        (jint)native_index
    );

/*
 * Delete local references to arrays and strings.
 */
    (*jenv)->DeleteLocalRef (jenv, jfname);
    (*jenv)->DeleteLocalRef (jenv, jlname);
    (*jenv)->DeleteLocalRef (jenv, jiname);

    return;
}

void jni_call_add_selected_axis (
    void    *v_jenv,
    void    *v_jobj,
    int         label_flag,
    int         tick_flag,
    int         tick_direction,
    char        *caption,
    double      major_interval,
    int         line_red,
    int         line_green,
    int         line_blue,
    int         text_red,
    int         text_green,
    int         text_blue,
    double      line_thickness,
    double      text_size,
    double      text_thickness,
    int         text_font,
    double      fx1,
    double      fy1,
    double      fx2,
    double      fy2,
    int         label_dir,
    int         selectable_object_num,
    double      afirst,
    double      alast,
    char        *fname,
    char        *lname,
    char        *iname,
    int         prim_num)
{
    jstring            jfname,
                       jlname,
                       jiname;
    jstring            jcaption;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SelectAxisMethodID = (*jenv)->GetMethodID (jenv, jcls, "addSelectedAxis",
    "(IIILjava/lang/String;DIIIIIIDDDIDDDDIIDD"
    "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");

    if (SelectAxisMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_selected_axis\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Allocate and fill the java strings.
 */
    jfname = NULL;
    if (fname != NULL) {
        jfname = (*jenv)->NewStringUTF (jenv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*jenv)->NewStringUTF (jenv, lname);
        if (jlname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*jenv)->NewStringUTF (jenv, iname);
        if (jiname == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            return;
        }
    }

    jcaption = NULL;
    if (caption != NULL) {
        jcaption = (*jenv)->NewStringUTF (jenv, caption);
        if (jcaption == NULL) {
            (*jenv)->DeleteLocalRef (jenv, jfname);
            (*jenv)->DeleteLocalRef (jenv, jlname);
            (*jenv)->DeleteLocalRef (jenv, jiname);
            return;
        }
    }

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SelectAxisMethodID,
        (jint)label_flag,
        (jint)tick_flag,
        (jint)tick_direction,
        jcaption,
        major_interval,
        (jint)line_red,
        (jint)line_green,
        (jint)line_blue,
        (jint)text_red,
        (jint)text_green,
        (jint)text_blue,
        line_thickness,
        text_size,
        text_thickness,
        (jint)text_font,
        fx1,
        fy1,
        fx2,
        fy2,
        (jint)label_dir,
        (jint)selectable_object_num,
        afirst,
        alast,
        jfname,
        jlname,
        jiname,
        (jint)prim_num
    );

/*
 * Delete local references to arrays and strings.
 */
    (*jenv)->DeleteLocalRef (jenv, jfname);
    (*jenv)->DeleteLocalRef (jenv, jlname);
    (*jenv)->DeleteLocalRef (jenv, jiname);
    (*jenv)->DeleteLocalRef (jenv, jcaption);

    return;

}



/*
 ************************************************************************

                          n a t i v e E d i t

 ************************************************************************
*/

/*
 * Class:     csw_jeasyx_src_JDisplayListBase
 * Method:    nativeEdit
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_csw_jeasyx_src_JDisplayListBase_nativeEdit
  (JNIEnv *env,
   jobject obj,
   jint j_dlist_index,
   jint j_threadid,
   jint j_sel_num)
{
    int              dlist_index;
    int              threadid;
    int              status;
    jint             ilist[10];


/*
 * If the int type is not a 32 bit signed integer, a conversion
 * from jint to int will be needed.  If the int is smaller than
 * a 32 bit signed then don't even compile since there will be
 * all kinds of problems.
 */
#if INT_MAX != 2147483647
    int              ilist2[1000];
  #if INT_MAX  <  2147483647
    #error The int data type with this compiler is less than 32 bits, ABORT.
  #endif
#endif

    dlist_index = (int)j_dlist_index;
    threadid = (int)j_threadid;

    dlist_index = (int)j_dlist_index;

    ezx_set_void_ptrs (dlist_index, (void *)env, (void *)obj);

    threadid = (int)j_threadid;
/*
 * Fill the ilist array.
 */
    ilist[0] = j_sel_num;

/*
 * Send the pick command to the current active display list.
 */

/*
 * If an int in C is 32 bit signed, it is the
 * same type as a jint, so I
 * just cast the ilist and idata pointers to
 * int in the call to process the command.
 */
#if INT_MAX == 2147483647
    status =
    ezx_process_command (dlist_index,
                         GTX_EDITPRIM,
                         threadid,
                         NULL,
                         (int *)ilist,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL);
/*
 * the jint is not the same size as int, so I need to
 * copy ilist into an int array.
 */
#else
    memset (ilist2, 0, 10 * sizeof(int));
    if (ilist != NULL) {
        for (i=0; i<10; i++) {
            ilist2[i] = (int)ilist[i];
        }
    }
    status =
    ezx_process_command (dlist_index,
                         GTX_EDITPRIM,
                         threadid,
                         NULLllist,
                         ilist2,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL);
#endif

    return status;

}

/*-------------------------------------------------------------------------*/

int jni_get_text_bounds (
    int     dlist_index,
    const char    *text,
    int     font_num,
    float   fsize,
    float   *bounds)
{

    jstring      jtext;
    jint         jfont_num;
    jdouble      jfsize;
    jdoubleArray jbounds;

    double       *ddata;
    double       dtest[3];

    bounds[0] = 0.0;
    bounds[1] = fsize;
    bounds[2] = 0.0;

    if (text == NULL  ||  dlist_index < 0) {
        return -1;
    }

    void   *v_jenv = NULL;
    void   *v_jobj = NULL;

    int  vstat = ezx_get_jenv (dlist_index,
                               &v_jenv,
                               &v_jobj);

    int  tlen = strlen (text);
    bounds[0] = .75 * fsize * tlen;

    jmethodID      FontBoundsMethodID = NULL;

    if (vstat == -1  ||  v_jenv == NULL  ||  v_jobj == NULL) {
        return -1;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    FontBoundsMethodID = (*jenv)->GetMethodID (jenv, jcls, "getTextBounds",
                                     "(Ljava/lang/String;ID[D)V");

    if (FontBoundsMethodID == NULL) {
        return -1;
    }

    jtext = (*jenv)->NewStringUTF (jenv, text);
    if (jtext == NULL) {
        return -1;
    }

    jfont_num = (jint)font_num;
    jfsize = (jdouble)fsize;

    jbounds = (*jenv)->NewDoubleArray (jenv, 3);
    if (jbounds == NULL) {
        (*jenv)->DeleteLocalRef (jenv, jtext);
        return -1;
    }

    dtest[0] = (double)bounds[0];
    dtest[1] = (double)fsize;;
    dtest[2] = 0.0;

    (*jenv)->SetDoubleArrayRegion (
        jenv,
        jbounds,
        0,
        3,
        dtest);

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        FontBoundsMethodID,
        jtext,
        jfont_num,
        jfsize,
        jbounds);

    ddata = (*jenv)->GetDoubleArrayElements (jenv, jbounds, NULL);

    if (ddata != NULL) {
        bounds[0] = (float)ddata[0];
        bounds[1] = (float)ddata[1];
        bounds[2] = (float)ddata[2];
    }

    (*jenv)->DeleteLocalRef (jenv, jtext);
    (*jenv)->DeleteLocalRef (jenv, jbounds);

    return 1;

}



/*-------------------------------------------------------------------------*/

void jni_return_converted_xyz (
    void    *v_jenv,
    void    *v_jobj,
    double  x,
    double  y,
    double  z)
{

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID ConvertedXYZMethodID = (*jenv)->GetMethodID (jenv, jcls, "setConvertedXYZ",
                                     "(DDD)V");

    if (ConvertedXYZMethodID == NULL) {
        return;
    }

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        ConvertedXYZMethodID,
        x,
        y,
        z);

    return;

}

/*
 * Convert screen coordinates to frame coordinates.
 */
JNIEXPORT jint JNICALL Java_csw_jeasyx_src_JDisplayListBase_convertToFrame
  (JNIEnv *env, jobject obj, jint j_dlist_index,
   jint j_threadid,  jint fnum, jint x, jint y)
{
    int              dlist_index, threadid;
    int              local_list[3];

    dlist_index = (int)j_dlist_index;
    threadid = (int)j_threadid;

    ezx_set_void_ptrs (dlist_index, (void *)env, (void *)obj);

/*
 * Convert the point and call the method to set the results back in the java side.
 */
    local_list[0] = (int)fnum;
    local_list[1] = (int)x;
    local_list[2] = (int)y;

    ezx_process_command (
                         dlist_index,
                         GTX_CONVERT_TO_FRAME,
                         threadid,
                         NULL,
                         local_list,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL);

    return (jint)1;

}


void jni_call_set_zoom_pan_data_method (
    void  *v_jenv,
    void  *v_jobj,
    char      *fname,
    double    ox1,
    double    oy1,
    double    ox2,
    double    oy2,
    double    nx1,
    double    ny1,
    double    nx2,
    double    ny2
) {
    jstring           jname;

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv  *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass jcls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SetZoomPanDataMethodID = (*jenv)->GetMethodID (jenv, jcls, "setZoomPanData",
                                     "(Ljava/lang/String;DDDDDDDD)V");
    if (SetZoomPanDataMethodID == NULL) {
        return;
    }

/*
 * Allocate and fill the java string.
 */
    jname = (*jenv)->NewStringUTF (jenv, fname);
    if (jname == NULL) {
        return;
    }

/*
 * Call the java object method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SetZoomPanDataMethodID,
        jname,
        ox1, oy1, ox2, oy2,
        nx1, ny1, nx2, ny2
    );

    return;

}


/*-----------------------------------------------------------------------------*/

/*
 * Process an easyx command from a static java context.  This is currently
 * only used for deleting the native resources for a display list when the
 * java display list object is finalized.
 */
JNIEXPORT jint JNICALL Java_csw_jeasyx_src_JDisplayListBase_sendStaticCommand
(
   JNIEnv *jnienv,
   jclass jobj,
   jint j_dlist_index,
   jint j_command_id,
   jint j_thread_id,
   jlongArray j_llist,
   jintArray j_ilist,
   jstring j_cdata,
   jbooleanArray j_bdata,
   jshortArray j_sdata,
   jintArray j_idata,
   jfloatArray j_fdata,
   jdoubleArray j_ddata,
   jint selectable
) {
    int              status, i;
    int              command_id;
    int              threadid;
    int              dlist_index;
    char             *cdata;
    unsigned char    *bdata;
    short int        *sdata;
    jint             *ilist,
                     *idata;
    float            *fdata;
    double           *ddata;

    jlong            *jllist;
    long             llist[100];


// ???? thread lock with static command ????

    threadid = (int)j_thread_id;

/*
 * If the int type is not a 32 bit signed integer, a conversion
 * from jint to int will be needed.  If the int is smaller than
 * a 32 bit signed then don't even compile since there will be
 * all kinds of problems.
 */
#if INT_MAX != 2147483647
    jsize            ntot;
    int              ilist2[1000];
    int              *idata;
    int              ibad;
  #if INT_MAX  <  2147483647
    #error The int data type with this compiler is less than 32 bits, ABORT.
  #endif
#endif

    jllist = NULL;
    ilist = NULL;
    cdata = NULL;
    bdata = NULL;
    sdata = NULL;
    idata = NULL;
    fdata = NULL;
    ddata = NULL;


/*
 * Get pointers to elements in the java arrays.  These are
 * not copies, but instead point into the java array object
 * directly.
 */
    command_id = (int)j_command_id;
    dlist_index = (int)j_dlist_index;
    if (j_llist) {
        jllist = (*jnienv)->GetLongArrayElements (jnienv, j_llist, JNI_FALSE);
    }

    if (j_ilist) {
        ilist = (*jnienv)->GetIntArrayElements (jnienv, j_ilist, JNI_FALSE);
    }

    if (j_cdata) {
        cdata = (char *)(*jnienv)->GetStringUTFChars (jnienv, j_cdata, JNI_FALSE);
    }

    if (j_bdata) {
        bdata = (*jnienv)->GetBooleanArrayElements (jnienv, j_bdata, JNI_FALSE);
    }

    if (j_sdata) {
        sdata = (*jnienv)->GetShortArrayElements (jnienv, j_sdata, JNI_FALSE);
    }

    if (j_idata) {
        idata = (*jnienv)->GetIntArrayElements (jnienv, j_idata, JNI_FALSE);
    }

    if (j_fdata) {
        fdata = (*jnienv)->GetFloatArrayElements (jnienv, j_fdata, JNI_FALSE);
    }

    if (j_ddata) {
        ddata = (*jnienv)->GetDoubleArrayElements (jnienv, j_ddata, JNI_FALSE);
    }

/*
 * Assign 64 bit integer jlong values to C long values.
 * A C long value may be 64 bits or 32 bits, depending upon
 * the operating system.  There is only one command that
 * passes a long currently, so I only copy the first _MAX_LONG_
 * values.
 */
    if (jllist != NULL) {
        for (i=0; i<_MAX_LONG_; i++) {
            llist[i] = (long)jllist[i];
        }
    }

/*
 * Do whatever is needed from the command.
 */
    selectable = selectable;

/*
 * If an int in C is 32 bit signed, it is the
 * same type as a jint, so I
 * just cast the ilist and idata pointers to
 * int in the call to process the command.
 */
#if INT_MAX == 2147483647
    status =
    ezx_process_command (dlist_index,
                         command_id,
                         threadid,
                         llist,
                         (int *)ilist,
                         cdata,
                         bdata,
                         sdata,
                         (int *)idata,
                         fdata,
                         ddata,
                         NULL);
/*
 * the jint is not the same size as int, so I need to
 * copy ilist and idata into int arrays.
 */
#else
    if (ilist != NULL) {
        for (i=0; i<MAX_LIST_SIZE; i++) {
            ilist2[i] = (int)ilist[i];
        }
    }
    idata2 = NULL;
    ibad = 0;
    if (j_idata != NULL) {
        ntot = GetArrayLength (jnienv, j_idata);
        idata2 = (int *)malloc (ntot * sizeof(int));
        if (idata2) {
            for (i=0; i<ntot; i++) {
                idata2[i] = (int) idata[i];
            }
        }
        else {
            ibad = 1;
        }
    }
    if (ibad == 0) {
        status =
        ezx_process_command (dlist_index,
                             command_id,
                             threadid,
                             llist,
                             ilist2,
                             cdata,
                             bdata,
                             sdata,
                             idata2,
                             fdata,
                             ddata,
                             NULL);
    }
    free (idata2);

    if (ibad == 1) {
        status = -1;
    }

#endif

/*
 * Even though no copies of the elements were made above, the release
 * calls are needed.  Without these calls, the memory usage grows without end.
 */
    if (j_llist) {
        (*jnienv)->ReleaseLongArrayElements (jnienv, j_llist, jllist, JNI_FALSE);
    }

    if (j_ilist) {
        (*jnienv)->ReleaseIntArrayElements (jnienv, j_ilist, ilist, JNI_FALSE);
    }

    if (j_cdata) {
        (*jnienv)->ReleaseStringUTFChars (jnienv, j_cdata, cdata);
    }

    if (j_bdata) {
        (*jnienv)->ReleaseBooleanArrayElements (jnienv, j_bdata, bdata, JNI_FALSE);
    }

    if (j_sdata) {
        (*jnienv)->ReleaseShortArrayElements (jnienv, j_sdata, sdata, JNI_FALSE);
    }

    if (j_idata) {
        (*jnienv)->ReleaseIntArrayElements (jnienv, j_idata, idata, JNI_FALSE);
    }

    if (j_fdata) {
        (*jnienv)->ReleaseFloatArrayElements (jnienv, j_fdata, fdata, JNI_FALSE);
    }

    if (j_ddata) {
        (*jnienv)->ReleaseDoubleArrayElements (jnienv, j_ddata, ddata, JNI_FALSE);
    }

    jobj = jobj;

    return (jint)status;
}
