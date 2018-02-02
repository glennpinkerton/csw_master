
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
static jmethodID      FillMethodID = NULL,
                      LineMethodID = NULL,
                      TextMethodID = NULL,
                      ArcMethodID = NULL,
                      FilledArcMethodID = NULL,
                      ImageMethodID = NULL,
                      FrameMethodID = NULL;

static jmethodID      SelectFillMethodID = NULL,
                      SelectLineMethodID = NULL,
                      SelectTextMethodID = NULL,
                      SelectSymbMethodID = NULL,
                      SelectAxisMethodID = NULL,
                      SelectArcMethodID = NULL,
                      SelectRectMethodID = NULL,
                      SelectContourMethodID = NULL,
                      ConvertedXYZMethodID = NULL;

static jmethodID      SymbFillMethodID = NULL,
                      SymbLineMethodID = NULL,
                      SymbArcMethodID = NULL,
                      SymbFilledArcMethodID = NULL;

static jmethodID      FontBoundsMethodID = NULL;

static jmethodID      SetZoomPanDataMethodID = NULL;

static JNIEnv         *JavaEnv;
static jobject        JavaObj;
static jclass         JavaCls;

static int            FunctionSet = 0;

static void update_zoom_pan_method (int command_id);
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
 *  This block of code can be uncommented to enable debug of
 *  this code when called from java.  The DebugBreak function
 *  is only available under microsloth.
#ifdef _GUI_MSWIN_
    static int       first = 1;
    if (first) {
        first = 0;
        DebugBreak ();
    }
#endif
 */

/*
 * The first time this is called, I need to set the function
 * pointer on the TextBounds.c file (located in the utils
 * area).  This is done because there are circular references
 * if I try to use the function name directly from the utils
 * library.
 */
    if (FunctionSet == 0) {
        gtx_SetTextBoundsJNIFunction (
            jni_get_text_bounds
        );
        FunctionSet = 1;
    }

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

    JavaObj = jobj;
    JavaEnv = jnienv;

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
 * If the command is a zoom or pan command, make sure the
 * zoom pan java method is valid.
 */
    update_zoom_pan_method (command_id);

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

    FillMethodID = (*env)->GetMethodID (env, cls, "addNativeFill",
                                     "([FFFIIIIIIII)V");
    if (FillMethodID == NULL) {
        return;
    }

    LineMethodID = (*env)->GetMethodID (env, cls, "addNativeLine",
                                     "([FIIIIIIFIII)V");
    if (LineMethodID == NULL) {
        return;
    }

    TextMethodID = (*env)->GetMethodID (env, cls, "addNativeText",
                                     "(FFLjava/lang/String;IIIIFFIIII)V");
    if (TextMethodID == NULL) {
        return;
    }

    ArcMethodID = (*env)->GetMethodID (env, cls, "addNativeArc",
                                "(FFFFFFIIIIIFFII)V");
    if (ArcMethodID == NULL) {
        return;
    }

    FilledArcMethodID = (*env)->GetMethodID (env, cls, "addNativeFilledArc",
                                     "(FFFFFFIIIIIFFIII)V");
    if (FilledArcMethodID == NULL) {
        return;
    }

    ImageMethodID = (*env)->GetMethodID (env, cls, "addNativeImage",
                                      "(FFFFII[B[B[B[BIIII)V");
    if (ImageMethodID == NULL) {
        return;
    }

    FrameMethodID = (*env)->GetMethodID (env, cls, "addNativeFrame",
                                  "(FFFFDDDDIIIIILjava/lang/String;)V");
    if (FrameMethodID == NULL) {
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
 * The java object and environment are also needed for the calls back to draw.
 */
    JavaObj = obj;
    JavaEnv = env;

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
    jxy = (*JavaEnv)->NewFloatArray (JavaEnv, 2 * npts);
    if (jxy == NULL) {
        return;
    }

    (*JavaEnv)->SetFloatArrayRegion (
        JavaEnv,
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jxy);

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
    jxy = (*JavaEnv)->NewFloatArray (JavaEnv, 2 * npts);
    if (jxy == NULL) {
        return;
    }

    (*JavaEnv)->SetFloatArrayRegion (
        JavaEnv,
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jxy);

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
    jtext = (*JavaEnv)->NewStringUTF (JavaEnv, text);
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jtext);

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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    jred = (*JavaEnv)->NewByteArray (JavaEnv, nt);
    if (jred == NULL) {
        return;
    }

    jgreen = (*JavaEnv)->NewByteArray (JavaEnv, nt);
    if (jgreen == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jred);
        return;
    }

    jblue = (*JavaEnv)->NewByteArray (JavaEnv, nt);
    if (jblue == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jred);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jgreen);
        return;
    }

    jtrans = (*JavaEnv)->NewByteArray (JavaEnv, nt);
    if (jtrans == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jred);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jgreen);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jblue);
        return;
    }

/*
 * Fill in the java color component arrays.
 */
    (*JavaEnv)->SetByteArrayRegion (
        JavaEnv,
        jred,
        0,
        nt,
        (jbyte *)red
    );

    (*JavaEnv)->SetByteArrayRegion (
        JavaEnv,
        jgreen,
        0,
        nt,
        (jbyte *)green
    );

    (*JavaEnv)->SetByteArrayRegion (
        JavaEnv,
        jblue,
        0,
        nt,
        (jbyte *)blue
    );

    (*JavaEnv)->SetByteArrayRegion (
        JavaEnv,
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jred);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jgreen);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jblue);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jtrans);

    return;

}





void jni_call_add_frame_method (
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

    if (FrameMethodID == NULL) {
        return;
    }

    jfname = NULL;
    if (fname != NULL) {
        jfname = (*JavaEnv)->NewStringUTF (JavaEnv, fname);
        if (jfname == NULL) {
            return;
        }
    }

/*
 * Call the java object's method.
 */
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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




void jni_msg (char const *text)
{

    if (text == NULL) {
        return;
    }

    if (text[0] == '\0') {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "%s", text);
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

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

    threadid = (int)j_threadid;

    dlist_index = (int)j_dlist_index;

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
 * The java object and environment are also needed for the calls to
 * send back the select set.
 */
    JavaObj = obj;
    JavaEnv = env;

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

    SelectFillMethodID = (*env)->GetMethodID (env, cls, "addSelectedFill",
    "(I[D[D[IIDDDIIIIIIIIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;III)V");
    if (SelectFillMethodID == NULL) {
        return -1;
    }

    SelectArcMethodID = (*env)->GetMethodID (env, cls, "addSelectedArc",
    "(IDDDDDDDIDDDIIIIIIIIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;III)V");
    if (SelectArcMethodID == NULL) {
        return -1;
    }

    SelectRectMethodID = (*env)->GetMethodID (env, cls, "addSelectedRectangle",
    "(IDDDDDDDDDIIIIIIIIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;III)V");
    if (SelectRectMethodID == NULL) {
        return -1;
    }

    SelectLineMethodID = (*env)->GetMethodID (env, cls, "addSelectedLine",
    "(I[D[DIDDIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;IIII)V");
    if (SelectLineMethodID == NULL) {
        return -1;
    }

    SelectContourMethodID = (*env)->GetMethodID (env, cls, "addSelectedContour",
    "(I[D[DIDDZIIIILjava/lang/String;IDDDDILjava/lang/String;"
    "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    if (SelectContourMethodID == NULL) {
        return -1;
    }

    SelectTextMethodID = (*env)->GetMethodID (env, cls, "addSelectedText",
    "(IDDIDDDDDDIIIIIIIIIIIIIIIIIILjava/lang/String;"
    "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    if (SelectTextMethodID == NULL) {
        return -1;
    }

    SelectSymbMethodID = (*env)->GetMethodID (env, cls, "addSelectedSymb",
    "(IDDIDDDIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    if (SelectSymbMethodID == NULL) {
        return -1;
    }

    SelectAxisMethodID = (*env)->GetMethodID (env, cls, "addSelectedAxis",
    "(IIILjava/lang/String;DIIIIIIDDDIDDDDIIDD"
    "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    if (SelectAxisMethodID == NULL) {
        return -1;
    }

    return 1;
}

/*
 * Draw the selected objects from the current active display list
 * back to the Java side.
 */
JNIEXPORT void JNICALL Java_csw_jeasyx_src_JDisplayListBase_nativeDrawSelected
  (JNIEnv *env, jobject obj, jint j_dlist_index, jint j_threadid)
{
    jclass           cls;
    int              dlist_index;
    int              threadid;


    threadid = (int)j_threadid;

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\n\nCalling nativeDraw\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

    dlist_index = (int)j_dlist_index;
    threadid = (int)j_threadid;

/*
 * Find the Java class methods for sending back graphics data.
 */

    cls = (*env)->GetObjectClass (env, obj);

    FillMethodID = (*env)->GetMethodID (env, cls, "addNativeFill",
                                     "([FFFIIIIIIII)V");
    if (FillMethodID == NULL) {
        return;
    }

    LineMethodID = (*env)->GetMethodID (env, cls, "addNativeLine",
                                     "([FIIIIIIFIII)V");
    if (LineMethodID == NULL) {
        return;
    }

    TextMethodID = (*env)->GetMethodID (env, cls, "addNativeText",
                                     "(FFLjava/lang/String;IIIIFFIIII)V");
    if (TextMethodID == NULL) {
        return;
    }

    ArcMethodID = (*env)->GetMethodID (env, cls, "addNativeArc",
                                    "(FFFFFFIIIIIFFII)V");
    if (ArcMethodID == NULL) {
        return;
    }

    FilledArcMethodID = (*env)->GetMethodID (env, cls, "addNativeFilledArc",
                                         "(FFFFFFIIIIIFFIII)V");
    if (FilledArcMethodID == NULL) {
        return;
    }

    ImageMethodID = (*env)->GetMethodID (env, cls, "addNativeImage",
                                      "(FFFFII[B[B[B[BIIII)V");
    if (ImageMethodID == NULL) {
        return;
    }

    FrameMethodID = (*env)->GetMethodID (env, cls, "addNativeFrame",
                                      "(FFFFDDDDIIIIILjava/lang/String;)V");
    if (FrameMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "Finished with method id assignment\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * The java object and environment are also needed for the calls back to draw.
 */
    JavaObj = obj;
    JavaEnv = env;

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

    if (SelectFillMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_selected_fill\n");
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
        jfname = (*JavaEnv)->NewStringUTF (JavaEnv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*JavaEnv)->NewStringUTF (JavaEnv, lname);
        if (jlname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*JavaEnv)->NewStringUTF (JavaEnv, iname);
        if (jiname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
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
    jxp = (*JavaEnv)->NewDoubleArray (JavaEnv, nptot);
    if (jxp == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
        return;
    }
    (*JavaEnv)->SetDoubleArrayRegion (
        JavaEnv,
        jxp,
        0,
        nptot,
        xp
    );

    jyp = (*JavaEnv)->NewDoubleArray (JavaEnv, nptot);
    if (jyp == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jxp);
        return;
    }
    (*JavaEnv)->SetDoubleArrayRegion (
        JavaEnv,
        jyp,
        0,
        nptot,
        yp
    );

    jnpts = (*JavaEnv)->NewIntArray (JavaEnv, ncomp);
    if (jnpts == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jxp);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jyp);
        return;
    }
    (*JavaEnv)->SetIntArrayRegion (
        JavaEnv,
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

printf ("\nin call to java select fill, sel idx = %d\n\n",
selectable_index);

/*
 * Call the java object method.
 */
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jxp);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jyp);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jnpts);

    return;
}



void jni_call_add_selected_line (
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
    int             native_index
) {
    jstring            jfname,
                       jlname,
                       jiname;

    jdoubleArray       jxp,
                       jyp;

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
        jfname = (*JavaEnv)->NewStringUTF (JavaEnv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*JavaEnv)->NewStringUTF (JavaEnv, lname);
        if (jlname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*JavaEnv)->NewStringUTF (JavaEnv, iname);
        if (jiname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
            return;
        }
    }

/*
 * Allocate and fill the java arrays with the points.
 */
    jxp = (*JavaEnv)->NewDoubleArray (JavaEnv, npts);
    if (jxp == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
        return;
    }
    (*JavaEnv)->SetDoubleArrayRegion (
        JavaEnv,
        jxp,
        0,
        npts,
        xp
    );

    jyp = (*JavaEnv)->NewDoubleArray (JavaEnv, npts);
    if (jyp == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jxp);
        return;
    }
    (*JavaEnv)->SetDoubleArrayRegion (
        JavaEnv,
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jxp);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jyp);

    return;
}



void jni_call_add_selected_contour (
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
    int             native_index
) {
    jstring            jfname,
                       jlname,
                       jiname;
    jstring            jlabel,
                       jsname;

    jdoubleArray       jxp,
                       jyp;

    if (SelectContourMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_selected_contour\n");
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
        jfname = (*JavaEnv)->NewStringUTF (JavaEnv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*JavaEnv)->NewStringUTF (JavaEnv, lname);
        if (jlname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*JavaEnv)->NewStringUTF (JavaEnv, iname);
        if (jiname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
            return;
        }
    }

    jsname = NULL;
    if (sname != NULL) {
        jsname = (*JavaEnv)->NewStringUTF (JavaEnv, sname);
        if (jsname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
            return;
        }
    }

    jlabel = NULL;
    if (label != NULL) {
        jlabel = (*JavaEnv)->NewStringUTF (JavaEnv, label);
        if (jlabel == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jsname);
            return;
        }
    }

/*
 * Allocate and fill the java arrays with the points.
 */
    jxp = (*JavaEnv)->NewDoubleArray (JavaEnv, npts);
    if (jxp == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jsname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jlabel);
        return;
    }
    (*JavaEnv)->SetDoubleArrayRegion (
        JavaEnv,
        jxp,
        0,
        npts,
        xp
    );

    jyp = (*JavaEnv)->NewDoubleArray (JavaEnv, npts);
    if (jyp == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jsname);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jlabel);
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jxp);
        return;
    }
    (*JavaEnv)->SetDoubleArrayRegion (
        JavaEnv,
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jsname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jlabel);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jxp);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jyp);

    return;
}



void jni_call_add_selected_rectangle (
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
    int             native_index
) {
    jstring            jfname,
                       jlname,
                       jiname;

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
        jfname = (*JavaEnv)->NewStringUTF (JavaEnv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*JavaEnv)->NewStringUTF (JavaEnv, lname);
        if (jlname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*JavaEnv)->NewStringUTF (JavaEnv, iname);
        if (jiname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);

    return;
}



void jni_call_add_selected_arc (
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
    int             native_index
) {
    jstring            jfname,
                       jlname,
                       jiname;

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
        jfname = (*JavaEnv)->NewStringUTF (JavaEnv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*JavaEnv)->NewStringUTF (JavaEnv, lname);
        if (jlname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*JavaEnv)->NewStringUTF (JavaEnv, iname);
        if (jiname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);

    return;
}



void jni_call_add_selected_text (
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
    int             native_index
) {
    jstring            jfname,
                       jlname,
                       jiname;
    jstring            jtextdata;

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
        jfname = (*JavaEnv)->NewStringUTF (JavaEnv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*JavaEnv)->NewStringUTF (JavaEnv, lname);
        if (jlname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*JavaEnv)->NewStringUTF (JavaEnv, iname);
        if (jiname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
            return;
        }
    }

    jtextdata = NULL;
    if (textdata != NULL) {
        jtextdata = (*JavaEnv)->NewStringUTF (JavaEnv, textdata);
        if (jtextdata == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jtextdata);

    return;
}



void jni_call_add_selected_symb (
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
    int             native_index
) {
    jstring            jfname,
                       jlname,
                       jiname;
    jstring            jsname;

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
        jfname = (*JavaEnv)->NewStringUTF (JavaEnv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*JavaEnv)->NewStringUTF (JavaEnv, lname);
        if (jlname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*JavaEnv)->NewStringUTF (JavaEnv, iname);
        if (jiname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
            return;
        }
    }

    jsname = NULL;
    if (sname != NULL) {
        jsname = (*JavaEnv)->NewStringUTF (JavaEnv, sname);
        if (jsname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
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
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);

    return;
}

void jni_call_add_selected_axis (
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
    int         prim_num
)
{
    jstring            jfname,
                       jlname,
                       jiname;
    jstring            jcaption;

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
        jfname = (*JavaEnv)->NewStringUTF (JavaEnv, fname);
        if (jfname == NULL) {
            return;
        }
    }

    jlname = NULL;
    if (lname != NULL) {
        jlname = (*JavaEnv)->NewStringUTF (JavaEnv, lname);
        if (jlname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            return;
        }
    }

    jiname = NULL;
    if (iname != NULL) {
        jiname = (*JavaEnv)->NewStringUTF (JavaEnv, iname);
        if (jiname == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
            return;
        }
    }

    jcaption = NULL;
    if (caption != NULL) {
        jcaption = (*JavaEnv)->NewStringUTF (JavaEnv, caption);
        if (jcaption == NULL) {
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
            (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
            return;
        }
    }

/*
 * Call the java object method.
 */
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jfname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jlname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jiname);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jcaption);

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
    jclass           cls;
    int              dlist_index;
    int              threadid;
    int              status;
    jint             ilist[10];


    threadid = (int)j_threadid;

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

/*
 * Fill the ilist array.
 */
    ilist[0] = j_sel_num;

/*
 * Find the Java class methods for sending back the selected data.
 */

    cls = (*env)->GetObjectClass (env, obj);

    SelectFillMethodID = (*env)->GetMethodID (env, cls, "addSelectedFill",
    "(I[D[D[IIDDDIIIIIIIIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;III)V");
    if (SelectFillMethodID == NULL) {
        return -1;
    }

    SelectArcMethodID = (*env)->GetMethodID (env, cls, "addSelectedArc",
    "(IDDDDDDDIDDDIIIIIIIIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;III)V");
    if (SelectArcMethodID == NULL) {
        return -1;
    }

    SelectRectMethodID = (*env)->GetMethodID (env, cls, "addSelectedRectangle",
    "(IDDDDDDDDDIIIIIIIIIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;III)V");
    if (SelectRectMethodID == NULL) {
        return -1;
    }

    SelectLineMethodID = (*env)->GetMethodID (env, cls, "addSelectedLine",
    "(I[D[DIDDIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;IIII)V");
    if (SelectLineMethodID == NULL) {
        return -1;
    }

    SelectContourMethodID = (*env)->GetMethodID (env, cls, "addSelectedContour",
    "(I[D[DIDDZIIIILjava/lang/String;IDDDDILjava/lang/String;"
    "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    if (SelectContourMethodID == NULL) {
        return -1;
    }

    SelectTextMethodID = (*env)->GetMethodID (env, cls, "addSelectedText",
    "(IDDIDDDDDDIIIIIIIIIIIIIIIIIILjava/lang/String;"
    "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    if (SelectTextMethodID == NULL) {
        return -1;
    }

    SelectSymbMethodID = (*env)->GetMethodID (env, cls, "addSelectedSymb",
    "(IDDIDDDIIIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    if (SelectSymbMethodID == NULL) {
        return -1;
    }

    SelectAxisMethodID = (*env)->GetMethodID (env, cls, "addSelectedAxis",
    "(IIILjava/lang/String;DIIIIIIDDDIDDDDIIDD"
    "Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    if (SelectAxisMethodID == NULL) {
        return -1;
    }

/*
 * The java object and environment are also needed for the calls to
 * send back the select set.
 */
    JavaObj = obj;
    JavaEnv = env;

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

/*
 * Get the parts for the specified symbol.  This is a static method
 * that does not need a display list available to work.  This should 
 * be synchronized for thread safety in the java code.
 */
JNIEXPORT jint JNICALL Java_csw_jeasyx_src_JDisplayListBase_getNativeSymbolParts
(
    JNIEnv *env,
    jclass cls,
    jint j_threadid,
    jint j_symbol_number,
    jdouble j_symbol_size,
    jdouble j_symbol_angle,
    jint j_dpi
)
{
    jint    jstat;
    int     istat;
    int     symb, dpi;
    double  size, angle;
    int     ilist[2];
    double  ddata[2];

    int threadid;

    JavaCls = cls;
    JavaEnv = env;

    threadid = (int)j_threadid;

    SymbFillMethodID =
        (*env)->GetStaticMethodID (env, cls, "addSymbNativeFill",
                                   "([FFFIIIIIIIII)V");
    if (SymbFillMethodID == NULL) {
        return -1;
    }

    SymbLineMethodID =
        (*env)->GetStaticMethodID (env, cls, "addSymbNativeLine",
                                   "([FIIIIIIFIII)V");
    if (SymbLineMethodID == NULL) {
        return -1;
    }

    SymbArcMethodID =
        (*env)->GetStaticMethodID (env, cls, "addSymbNativeArc",
                                   "(FFFFFFIIIIIFFIII)V");
    if (SymbArcMethodID == NULL) {
        return -1;
    }

    SymbFilledArcMethodID =
        (*env)->GetStaticMethodID (env, cls, "addSymbNativeFilledArc",
                                   "(FFFFFFIIIIIFFIIII)V");
    if (SymbFilledArcMethodID == NULL) {
        return -1;
    }

    symb = (int)j_symbol_number;
    size = (double)j_symbol_size;
    angle = (double)j_symbol_angle;
    dpi = (int)j_dpi;

/*
 * Use the process command so the command gets logged if needed.
 */
    ilist[0] = symb;
    ilist[1] = dpi;
    ddata[0] = size;
    ddata[1] = angle;

    istat =
    ezx_process_command (
                         0,
                         GTX_GET_SYMBOL_PARTS,
                         threadid,
                         NULL,
                         ilist,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         ddata,
                         NULL);
    jstat = (jint)istat;

    return jstat;

}


void jni_call_add_symb_fill_method (
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
    int         image_id,
    int         selectable
) {
    jfloatArray        jxy;

    if (SymbFillMethodID == NULL) {
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
    jxy = (*JavaEnv)->NewFloatArray (JavaEnv, 2 * npts);
    if (jxy == NULL) {
        return;
    }

    (*JavaEnv)->SetFloatArrayRegion (
        JavaEnv,
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
    (*JavaEnv)->CallStaticVoidMethod (
        JavaEnv,
        JavaCls,
        SymbFillMethodID,
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
        image_id,
        selectable
    );

/*
 * Delete the java points array.
 */
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jxy);

    return;

}



void jni_call_add_symb_line_method (
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

    if (SymbLineMethodID == NULL) {
        return;
    }

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\nEntered jni_call_add_symb_line_method\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Allocate and fill the java array with the points.
 */
    jxy = (*JavaEnv)->NewFloatArray (JavaEnv, 2 * npts);
    if (jxy == NULL) {
        return;
    }

    (*JavaEnv)->SetFloatArrayRegion (
        JavaEnv,
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
    (*JavaEnv)->CallStaticVoidMethod (
        JavaEnv,
        JavaCls,
        SymbLineMethodID,
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
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jxy);

    return;

}



void jni_call_add_symb_arc_method (
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
    int         image_id,
    int         selectable
) {

    if (SymbArcMethodID == NULL) {
        return;
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
    (*JavaEnv)->CallStaticVoidMethod (
        JavaEnv,
        JavaCls,
        SymbArcMethodID,
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
        image_id,
        selectable
    );

    return;

}



void jni_call_add_symb_filled_arc_method (
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
    int         image_id,
    int         selectable
) {

    if (SymbFilledArcMethodID == NULL) {
        return;
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
    (*JavaEnv)->CallStaticVoidMethod (
        JavaEnv,
        JavaCls,
        SymbFilledArcMethodID,
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
        image_id,
        selectable
    );

    return;

}


/*-------------------------------------------------------------------------*/

int jni_get_text_bounds (
    char    *text,
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
    bounds[1] = 0.0;
    bounds[2] = 0.0;

    if (FontBoundsMethodID == NULL) {
        return -1;
    }

    jtext = (*JavaEnv)->NewStringUTF (JavaEnv, text);
    if (jtext == NULL) {
        return -1;
    }

    jfont_num = (jint)font_num;
    jfsize = (jdouble)fsize;

    jbounds = (*JavaEnv)->NewDoubleArray (JavaEnv, 3);
    if (jbounds == NULL) {
        (*JavaEnv)->DeleteLocalRef (JavaEnv, jtext);
        return -1;
    }

    dtest[0] = 10.0;
    dtest[1] = 20.0;
    dtest[2] = 30.0;

    (*JavaEnv)->SetDoubleArrayRegion (
        JavaEnv,
        jbounds,
        0,
        3,
        dtest);

/*
 * Call the java object method.
 */
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
        FontBoundsMethodID,
        jtext,
        jfont_num,
        jfsize,
        jbounds);

    ddata = (*JavaEnv)->GetDoubleArrayElements (JavaEnv, jbounds, NULL);

    if (ddata != NULL) {
        bounds[0] = (float)ddata[0];
        bounds[1] = (float)ddata[1];
        bounds[2] = (float)ddata[2];
    }

    (*JavaEnv)->DeleteLocalRef (JavaEnv, jtext);
    (*JavaEnv)->DeleteLocalRef (JavaEnv, jbounds);

    return 1;

}



/*
 *  Draw the current active display list back to the Java side.
 */
JNIEXPORT void JNICALL Java_csw_jeasyx_src_JDisplayListBase_setFontMethods
  (JNIEnv *env, jobject obj)
{
    jclass           cls;

  #if DEBUG_JNI_FILE
    sprintf (fileline, "\n\nCalling nativeDraw\n");
    if (dbfile) {
        fputs (fileline, dbfile);
        fflush (dbfile);
    }
  #endif

/*
 * Find the Java class methods for getting font data.
 * The same method will be used by all threads, so the actual
 * caching of the method id hefre is not thread critical.
 * However, the java code which does the work should be synchronized.
 */
    cls = (*env)->GetObjectClass (env, obj);

    FontBoundsMethodID = (*env)->GetMethodID (env, cls, "getTextBounds",
                                     "(Ljava/lang/String;ID[D)V");

    return;

}

/*-------------------------------------------------------------------------*/

void jni_return_converted_xyz (
    double  x,
    double  y,
    double  z)
{
    if (ConvertedXYZMethodID == NULL) {
        return;
    }

/*
 * Call the java object method.
 */
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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
    jclass           cls;
    int              dlist_index, threadid;
    int              local_list[3];


    threadid = (int)j_threadid;

    dlist_index = (int)j_dlist_index;
    threadid = (int)j_threadid;

    JavaEnv = env;
    JavaObj = obj;

/*
 * Find the Java class methods for sending back graphics data.
 */

    cls = (*env)->GetObjectClass (env, obj);

    ConvertedXYZMethodID = (*env)->GetMethodID (env, cls, "setConvertedXYZ",
                                     "(DDD)V");
    if (ConvertedXYZMethodID == NULL) {
        return (jint)-1;
    }

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

    ConvertedXYZMethodID = NULL;

    return (jint)1;

}


static void update_zoom_pan_method (int command_id)
{
    jclass               cls;

    if (command_id == GTX_PANFRAME  ||
        command_id == GTX_ZOOMFRAME  ||
        command_id == GTX_ZOOMEXTENTS ||
        command_id == GTX_ZOOMOUT)
    {
        cls = (*JavaEnv)->GetObjectClass (JavaEnv, JavaObj);
        SetZoomPanDataMethodID = (*JavaEnv)->GetMethodID (JavaEnv, cls, "setZoomPanData",
                                     "(Ljava/lang/String;DDDDDDDD)V");
    }

}


void jni_call_set_zoom_pan_data_method (
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

    if (SetZoomPanDataMethodID == NULL) {
        return;
    }

/*
 * Allocate and fill the java string.
 */
    jname = (*JavaEnv)->NewStringUTF (JavaEnv, fname);
    if (jname == NULL) {
        return;
    }

/*
 * Call the java object method.
 */
    (*JavaEnv)->CallVoidMethod (
        JavaEnv,
        JavaObj,
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

    JavaEnv = jnienv;

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
