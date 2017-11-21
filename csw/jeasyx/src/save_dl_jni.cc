/*
 *  All of the functions needing jni.h or other associated headers
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
#include <csw/jeasyx/private_include/DisplayListJNI.h>
#include <csw/jeasyx/private_include/EZXCommand.h>
#include "JDisplayList.h"

extern "C" {
#include <csw/jeasyx/private_include/gtx_msgP.h>
}


static jmethodID      FillMethodID = NULL,
                      LineMethodID = NULL,
                      TextMethodID = NULL,
                      ArcMethodID = NULL,
                      FilledArcMethodID = NULL,
                      ImageMethodID = NULL;

static JNIEnv         *JavaEnv;
static jobject        JavaObj;

static int            First = 1;


/*
 * The functions prototyped automatically from the javah utility
 * are all declared as extern "C" in the header file.  To be safe,
 * and to silence an intermittent pc-lint error, any functions 
 * defined as extern "C" in the header file are also defined as
 * extern "C" here.
 */

/*=====================================================================*/

extern "C" {


/*
 * Process an easyx display list command sent from Java.
 * All commands except the Draw command are processed here.  
 * The Draw command is done via a separate draw function.
 */
JNIEXPORT void JNICALL Java_EZXGraphicsCommand_Command
(
   JNIEnv *jnienv,
   jclass jobj,
   jint j_command_id,
   jintArray j_ilist,
   jstring j_cdata,
   jbooleanArray j_bdata,
   jshortArray j_sdata,
   jintArray j_idata,
   jfloatArray j_fdata,
   jdoubleArray j_ddata
) {
    int              command_id;
    char             *cdata;
    unsigned char    *bdata;
    short int        *sdata;
    long             *ilist, *idata;
    float            *fdata;
    double           *ddata;

    jobj = jobj;

/*
 * Get pointers to elements in the java arrays.  These are
 * not copies, but instead point into the java array object
 * directly.
 */
    command_id = j_command_id;
    ilist = jnienv->GetIntArrayElements (j_ilist, JNI_FALSE);
    cdata = (char *)jnienv->GetStringUTFChars (j_cdata, JNI_FALSE);
    bdata = jnienv->GetBooleanArrayElements (j_bdata, JNI_FALSE);
    sdata = jnienv->GetShortArrayElements (j_sdata, JNI_FALSE);
    idata = jnienv->GetIntArrayElements (j_idata, JNI_FALSE);
    fdata = jnienv->GetFloatArrayElements (j_fdata, JNI_FALSE);
    ddata = jnienv->GetDoubleArrayElements (j_ddata, JNI_FALSE);

/*
 * Do whatever is needed from the command.
 */
    ezx_process_command (command_id,
                         ilist,
                         cdata, bdata, sdata, idata, fdata, ddata);
   
/*
 * Even though no copies of the elements were made above, the release 
 * calls are needed.  Without these calls, the memory usage grows without end.
 */ 
    jnienv->ReleaseIntArrayElements (j_ilist, ilist, JNI_FALSE);
    jnienv->ReleaseStringUTFChars (j_cdata, cdata);
    jnienv->ReleaseBooleanArrayElements (j_bdata, bdata, JNI_FALSE);
    jnienv->ReleaseShortArrayElements (j_sdata, sdata, JNI_FALSE);
    jnienv->ReleaseIntArrayElements (j_idata, idata, JNI_FALSE);
    jnienv->ReleaseFloatArrayElements (j_fdata, fdata, JNI_FALSE);
    jnienv->ReleaseDoubleArrayElements (j_ddata, ddata, JNI_FALSE);

    return;
}


/*
 *  Draw the current active display list back to the Java side.
 */
JNIEXPORT void JNICALL Java_JDisplayList_NativeDraw
  (JNIEnv *env, jobject obj)
{
    jclass           cls;
/*
 * Find the Java class methods if this is the first time called
 */
    if (First) {
        
        cls = env->GetObjectClass (obj);

        FillMethodID = env->GetMethodID (cls, "AddNativeFill", 
                                         "([FIIIIII)V");
        if (FillMethodID == NULL) {
            return;
        }

        LineMethodID = env->GetMethodID (cls, "AddNativeLine", 
                                         "([FIIIIIFI)V");
        if (LineMethodID == NULL) {
            return;
        }

        TextMethodID = env->GetMethodID (cls, "AddNativeText", 
                                         "(FF[CIIIIFFII)V");
        if (TextMethodID == NULL) {
            return;
        }

        ArcMethodID = env->GetMethodID (cls, "AddNativeArc", 
                                        "(FFFFFFIIIFFI)V");
        if (ArcMethodID == NULL) {
            return;
        }

        FilledArcMethodID = env->GetMethodID (cls, "AddNativeFilledArc", 
                                             "(FFFFFFIIIFI)V");
        if (FilledArcMethodID == NULL) {
            return;
        }

        ImageMethodID = env->GetMethodID (cls, "AddNativeFill", 
                                          "(FFFFII[B[B[B[BI)V");
        if (ImageMethodID == NULL) {
            return;
        }

        First = 0;

    }

/*
 * The java object and environment are also needed for the calls back to draw.
 */
    JavaObj = obj;
    JavaEnv = env;

/*
 * Send the draw command to the curerent active display list.
 * This will send all the viewable primitives to the java side 
 * via the methods defined above.
 */
    ezx_process_command (GTX_DRAW_CURRENT_VIEW,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL,
                         NULL);

    return;

}

/*
 * This is the end of the block of extern "C" functions.
 */

}

/*==================================================================*/


/*
 * The rest of the file has full blown C++ functions.
 */

void jni_call_add_fill_method (
    float       *xy,
    int         npts,
    int         red,
    int         green,
    int         blue,
    int         pattern,
    int         selectable
) {
    jfloatArray        jxy;

    if (FillMethodID == NULL) {
        return;
    }

/*
 * Allocate and fill the java array with the points.
 */
    jxy = JavaEnv->NewFloatArray (2 * npts);
    if (jxy == NULL) {
        return;
    }

    JavaEnv->SetFloatArrayRegion (
        jxy,
        0,
        npts*2,
        xy
    );    

/*
 * Call the java object's method.
 */
    JavaEnv->CallVoidMethod (
        JavaObj,
        FillMethodID,
        jxy,
        npts,
        red,
        green,
        blue,
        pattern,
        selectable
    );

/*
 * Delete the java points array.
 */
    JavaEnv->DeleteLocalRef (jxy);

    return;

}



void jni_call_add_line_method (
    float       *xy,
    int         npts,
    int         red,
    int         green,
    int         blue,
    int         pattern,
    float       thickness,
    int         selectable
) {
    jfloatArray        jxy;

    if (LineMethodID == NULL) {
        return;
    }

/*
 * Allocate and fill the java array with the points.
 */
    jxy = JavaEnv->NewFloatArray (2 * npts);
    if (jxy == NULL) {
        return;
    }

    JavaEnv->SetFloatArrayRegion (
        jxy,
        0,
        npts*2,
        xy
    );    

/*
 * Call the java object's method.
 */
    JavaEnv->CallVoidMethod (
        JavaObj,
        LineMethodID,
        jxy,
        npts,
        red,
        green,
        blue,
        pattern,
        thickness,
        selectable
    );

/*
 * Delete the java points array.
 */
    JavaEnv->DeleteLocalRef (jxy);

    return;

}



void jni_call_add_text_method (
    float       x,
    float       y,
    char        *text,
    int         red,
    int         green,
    int         blue,
    float       angle,
    float       size,
    int         font,
    int         selectable
) {
    jstring           jtext;

    if (TextMethodID == NULL) {
        return;
    }

/*
 * Allocate and fill the java string.
 */
    jtext = JavaEnv->NewStringUTF (text);
    if (jtext == NULL) {
        return;
    }

/*
 * Call the java object method.
 */
    JavaEnv->CallVoidMethod (
        JavaObj,
        TextMethodID,
        x,
        y,
        jtext,
        red,
        green,
        blue,
        angle,
        size,
        font,
        selectable
    );

/*
 * Delete the java char array.
 */
    JavaEnv->DeleteLocalRef (jtext);

    return;

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
    float       thickness,
    float       angle,
    int         selectable
) {

    if (ArcMethodID == NULL) {
        return;
    }

/*
 * call the java object method.
 */
    JavaEnv->CallVoidMethod (
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
        thickness,
        angle,
        selectable
    );

    return;

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
    float       angle,
    int         selectable
) {

    if (FilledArcMethodID == NULL) {
        return;
    }

/*
 * call the java object method.
 */
    JavaEnv->CallVoidMethod (
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
        angle,
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
    int             selectable
) {
    jbooleanArray   jred, jgreen, jblue, jtrans;
    int             nt;

    if (ImageMethodID == NULL) {
        return;
    }

    nt = ncol * nrow;
    jred = NULL;
    jgreen = NULL;
    jblue = NULL;
    jtrans = NULL;

/*
 * Allocate space for the java color component arrays.
 */
    jred = JavaEnv->NewBooleanArray (nt);
    if (jred == NULL) {
        return;
    }

    jgreen = JavaEnv->NewBooleanArray (nt);
    if (jgreen == NULL) {
        JavaEnv->DeleteLocalRef (jred);
        return;
    }

    jblue = JavaEnv->NewBooleanArray (nt);
    if (jblue == NULL) {
        JavaEnv->DeleteLocalRef (jred);
        JavaEnv->DeleteLocalRef (jgreen);
        return;
    }

    jtrans = JavaEnv->NewBooleanArray (nt);
    if (jtrans == NULL) {
        JavaEnv->DeleteLocalRef (jred);
        JavaEnv->DeleteLocalRef (jgreen);
        JavaEnv->DeleteLocalRef (jblue);
        return;
    }

/*
 * Fill in the java color component arrays.
 */
    JavaEnv->SetBooleanArrayRegion (
        jred,
        0,
        nt,
        red
    );    

    JavaEnv->SetBooleanArrayRegion (
        jgreen,
        0,
        nt,
        green
    );    

    JavaEnv->SetBooleanArrayRegion (
        jblue,
        0,
        nt,
        blue
    );    

    JavaEnv->SetBooleanArrayRegion (
        jtrans,
        0,
        nt,
        trans
    );    

/*
 * Call the java object method.
 */
    JavaEnv->CallVoidMethod (
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
        selectable
    );

/*
 * Delete the java color component arrays.
 */
    JavaEnv->DeleteLocalRef (jred);
    JavaEnv->DeleteLocalRef (jgreen);
    JavaEnv->DeleteLocalRef (jblue);
    JavaEnv->DeleteLocalRef (jtrans);

    return;

}
