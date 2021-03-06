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

/*
 * This windows.h include needs to be uncommented in order to
 * use the DebugBreak function.  The DebugBreak function is
 * only available on microsloth.
#ifdef _GUI_MSWIN_
#include <windows.h>
#endif
 */

#include <jni.h>
#include <limits.h>

#include <csw/jsurfaceworks/private_include/SurfaceWorksJNI.h>
#include <csw/jsurfaceworks/private_include/SWCommand.h>

#include "csw_jsurfaceworks_src_JSurfaceWorks_FinalizedTmeshID.h"
#include "csw_jsurfaceworks_src_JSurfaceWorks.h"
#include "csw_jsurfaceworks_src_JSurfaceWorksBase.h"

#define MAX_LIST_ARRAY \
csw_jsurfaceworks_src_JSurfaceWorksBase_MAX_LIST_ARRAY

#define MAX_TRI_NDO  100

/*
 * Set static file variables.
 */
//static JNIEnv         *jenv = NULL;
//static jobject        jobj = NULL;


/*
 * The primfile is used when "recording" of the surface
 * results is desired.  The original reason for this is to
 * record results of test cases for future comparisons. This
 * is not thread safe.  Only use this from the c++ only test
 * programs (which only use single threads).
 */

static FILE           *primfile = NULL;

void jni_set_prim_file_sw (FILE *pf) {
  primfile = pf;
}

FILE *jni_get_prim_file_sw () {
  return primfile;
}

void jni_open_prim_file_sw (char *fname) {
  if (fname != NULL) {
    primfile = fopen (fname, "w");
  }
}

void jni_close_prim_file_sw () {
  if (primfile != NULL) fclose (primfile);
  primfile = NULL;
}


/*
 * This function is only here to make sure that the file
 * gets loaded into the shared library.
 */
JNIEXPORT void JNICALL Java_csw_jsurfaceworks_src_JSurfaceWorksBase_bootMe
  (JNIEnv *env, jobject jobj)
{
    env = env;
    jobj = jobj;
    return;
}

/*
 * Get a command and its data from java.  Convert to 32 bit ints if needed
 * and send the data to the surfaceworks command processor.  Return values are
 * sent back to the java side by calling java methods with appropriate data
 * to build the objects needed for the particular command.  When this function
 * returns, the results will be waiting on the java side, or else an error
 * condition will be reported.  If there is an error, this function will
 * also return -1.  It is possible for this function to return -1 without
 * any error report on the java side.  This can happen if the command id is
 * bad or if a memory allocation error occurs in this function.  In any case,
 * a return value of -1 means that a serious error occurred.
 */
JNIEXPORT jlong JNICALL Java_csw_jsurfaceworks_src_JSurfaceWorksBase_sendCommand
   (JNIEnv *jnienv,
    jobject jobj,
    jint j_commandID,
    jint j_expect_return,
    jintArray j_ilist,
    jlongArray j_llist,
    jdoubleArray j_dlist,
    jstring j_cdata,
    jintArray j_idata,
    jfloatArray j_fdata,
    jdoubleArray j_ddata,
    jint    j_tid
   )
{
    jint             *idata;
    float            *fdata;
    double           *ddata;

    long              status;
    int              command_id;
    int              threadid;

    jint             *ilist;
    jlong            *llist;
    double           *dlist;
    char             *cdata;

    long             long_list_array[MAX_LIST_ARRAY];
    long             *llist2;
    int              i, lsize;

    void          *v_jenv = (void *)jnienv;
    void          *v_jobj = (void *)jobj;

    threadid = (int)j_tid;

/*
 * If the int type is not a 32 bit signed integer, a conversion
 * from jint to int will be needed.  If the int is smaller than
 * a 32 bit signed then don't even compile since there will be
 * all kinds of problems.
 */
#if INT_MAX != 2147483647
    jsize            ntot;
    int              ilist2[MAX_LIST_ARRAY];
    int              *idata2[MAX_INT_ARRAYS];
    int              *itmp2;
    int              ibad;
  #if INT_MAX  <  2147483647
    #error The int data type with this compiler is less than 32 bits, ABORT.
  #endif
#endif

    ilist = NULL;
    llist = NULL;
    dlist = NULL;
    cdata = NULL;

    idata = NULL;
    fdata = NULL;
    ddata = NULL;

/*
 * Get native array references from the java arrays.  Do not explicitly
 * ask for copies in this.  However, java may make copies and these
 * native array references all need to be released when the native side
 * no longer needs them.
 */
    command_id = (int)j_commandID;

    if (j_ilist) {
        ilist = (*jnienv)->GetIntArrayElements (jnienv, j_ilist, JNI_FALSE);
    }
    if (j_llist) {
        llist = (*jnienv)->GetLongArrayElements (jnienv, j_llist, JNI_FALSE);
    }
    if (j_cdata) {
        cdata = (char *)(*jnienv)->GetStringUTFChars (jnienv, j_cdata, JNI_FALSE);
    }
    if (j_dlist) {
        dlist = (*jnienv)->GetDoubleArrayElements (jnienv, j_dlist, JNI_FALSE);
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
 * If the long type in C is 64 bits, just cast from the jlong llist to
 * the llist2 array.
 */
    if (llist != NULL) {
        lsize = sizeof(long);
        if (lsize == 8) {
            llist2 = (long *)llist;
        }
        else {
            llist2 = long_list_array;
            for (i=0; i<MAX_LIST_ARRAY; i++) {
                llist2[i] = (long)llist[i];
            }
        }
    }
    else {
        llist2 = NULL;
    }

/*
 * If an int in C is 32 bit signed, it is the
 * same type as a jint, so I can cast the jint
 * pointers to int safely.
 */
#if INT_MAX == 2147483647
    status =
    sw_process_command (v_jenv, v_jobj,
                        command_id,
                        threadid,
                        (int *)ilist,
                        llist2,
                        dlist,
                        cdata,
                        (int *)idata,
                        fdata,
                        ddata
                       );

/*
 * The jint is not the same size as int, so I need to
 * copy the input and modifiable jint arrays into int
 * arrays.
 */
#else

    if (ilist != NULL) {
        for (i=0; i<MAX_LIST_SIZE; i++) {
            ilist2[i] = (int)ilist[i];
        }
    }
    if (llist != NULL) {
        for (i=0; i<MAX_LIST_SIZE; i++) {
            llist2[i] = (int)llist[i];
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
        sw_process_command (v_jenv, v_jobj,
                            command_id,
                            threadid,
                            ilist2,
                            llist2,
                            dlist,
                            cdata,
                            idata2,
                            fdata,
                            ddata,
                            idataMod2,
                            fdataMod,
                            ddataMod,
                            idataOut2, isizeOut,
                            fdataOut, fsizeOut,
                            ddataOut, dsizeOut
                           );
    }
    else {
        status = -1;
    }

  /*
   * Free int input arrays.
   */
    free (idata2);

#endif

/*
 * Even though no copies of the elements were made above, the release
 * calls are needed.  Without these calls, the memory usage grows without end.
 */
    if (j_ilist) {
        (*jnienv)->ReleaseIntArrayElements (jnienv, j_ilist, ilist, JNI_FALSE);
        (*jnienv)->DeleteLocalRef (jnienv, j_ilist);
    }
    if (j_llist) {
        (*jnienv)->ReleaseLongArrayElements (jnienv, j_llist, llist, JNI_FALSE);
        (*jnienv)->DeleteLocalRef (jnienv, j_llist);
    }
    if (j_cdata) {
        (*jnienv)->ReleaseStringUTFChars (jnienv, j_cdata, cdata);
        (*jnienv)->DeleteLocalRef (jnienv, j_cdata);
    }
    if (j_dlist) {
        (*jnienv)->ReleaseDoubleArrayElements (jnienv, j_dlist, dlist, JNI_FALSE);
        (*jnienv)->DeleteLocalRef (jnienv, j_dlist);
    }
    if (j_idata) {
        (*jnienv)->ReleaseIntArrayElements (jnienv, j_idata, idata, JNI_FALSE);
        (*jnienv)->DeleteLocalRef (jnienv, j_idata);
    }
    if (j_fdata) {
        (*jnienv)->ReleaseFloatArrayElements (jnienv, j_fdata, fdata, JNI_FALSE);
        (*jnienv)->DeleteLocalRef (jnienv, j_fdata);
    }
    if (j_ddata) {
        (*jnienv)->ReleaseDoubleArrayElements (jnienv, j_ddata, ddata, JNI_FALSE);
        (*jnienv)->DeleteLocalRef (jnienv, j_ddata);
    }

    return (jlong)status;

}


/*------------------------------------------------------------------------*/

static void write_store_tri_mesh (
    double const *xnode,
    double const *ynode,
    double const *znode,
    int const *nodeflags,
    int numnode,
    int const *n1edge,
    int const *n2edge,
    int const *t1edge,
    int const *t2edge,
    int const *edgeflags,
    int numedge,
    int const *e1tri,
    int const *e2tri,
    int const *e3tri,
    int const *triflags,
    int numtri)
{
    int    i, ndo;

    ndo = numnode;
    if (ndo > MAX_TRI_NDO) ndo = MAX_TRI_NDO;
    fprintf (primfile, "\nStoreTriMesh method\n");
    fprintf (primfile,
      "  number of nodes = %d  node list (10 max) follows:\n", numnode);
    for (i=0; i<ndo; i++) {
      fprintf (primfile, "    x = %.5f   y = %.5f   z = %.5f\n",
                xnode[i], ynode[i], znode[i]);
    }
    fprintf (primfile,
       "  number of edges = %d  edge list (10 max) follows:\n", numedge);
    ndo = numedge;
    if (ndo > MAX_TRI_NDO) ndo = MAX_TRI_NDO;
    for (i=0; i<ndo; i++) {
      fprintf (primfile,
         "    node1 = %d   node2 = %d    tri1 = %d   tri2 = %d\n",
          n1edge[i], n2edge[i], t1edge[i], t2edge[i]);
    }
    fprintf (primfile,
      "  number of triangles = %d  triangle list (10 max) follows:\n", numtri);
    ndo = numtri;
    if (ndo > MAX_TRI_NDO) ndo = MAX_TRI_NDO;
    for (i=0; i<ndo; i++) {
      if (triflags == NULL) {
        fprintf (primfile, "    edge1 = %d   edge2 = %d   edge3 = %d\n",
          e1tri[i], e2tri[i], e3tri[i]);
      }
      else {
        fprintf (primfile,
          "    edge1 = %d   edge2 = %d   edge3 = %d   flag = %d\n",
          e1tri[i], e2tri[i], e3tri[i], triflags[i]);
      }
    }
    
}


void jni_call_store_tri_mesh_method (
    void  *v_jenv,
    void  *v_jobj,
    double const *xnode,
    double const *ynode,
    double const *znode,
    int const *nodeflags,
    int numnode,
    int const *n1edge,
    int const *n2edge,
    int const *t1edge,
    int const *t2edge,
    int const *edgeflags,
    int numedge,
    int const *e1tri,
    int const *e2tri,
    int const *e3tri,
    int const *triflags,
    int numtri)
{
    jint        *jnodeflags;
    jint        *jn1edge,
                *jn2edge,
                *jt1edge,
                *jt2edge,
                *jedgeflags,
                *je1tri,
                *je2tri,
                *je3tri,
                *jtriflags;

    jdoubleArray        j_xnode;
    jdoubleArray        j_ynode;
    jdoubleArray        j_znode;
    jintArray           j_nodeflags;
    jint                j_numnode;
    jintArray           j_n1edge;
    jintArray           j_n2edge;
    jintArray           j_t1edge;
    jintArray           j_t2edge;
    jintArray           j_edgeflags;
    jint                j_numedge;
    jintArray           j_e1tri;
    jintArray           j_e2tri;
    jintArray           j_e3tri;
    jintArray           j_triflags;
    jint                j_numtri;

    if (primfile != NULL) {
        write_store_tri_mesh (
            xnode,
            ynode,
            znode,
            nodeflags,
            numnode,
            n1edge,
            n2edge,
            t1edge,
            t2edge,
            edgeflags,
            numedge,
            e1tri,
            e2tri,
            e3tri,
            triflags,
            numtri);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID StoreTriMeshMethodID = (*jenv)->GetMethodID (jenv, cls, "storeTriMesh",
                                         "([D[D[D[I[I[I[I[I[I[I[I[I[I)V");
    if (StoreTriMeshMethodID == NULL) {
        return;
    }

/*
 * If the native int is not 32 bit signed, need to convert
 * all the int arrays to 32 bit signed.
 */
#if INT_MAX == 2147483647
    jnodeflags = (jint *)nodeflags;
    jn1edge = (jint *)n1edge;
    jn2edge = (jint *)n2edge;
    jt1edge = (jint *)t1edge;
    jt2edge = (jint *)t2edge;
    jedgeflags = (jint *)edgeflags;
    je1tri = (jint *)e1tri;
    je2tri = (jint *)e2tri;
    je3tri = (jint *)e3tri;
    jtriflags = (jint *)triflags;
#else
    #error The trimesh native calc does not support non 32 bit signed int yet.
#endif

/*
 * Put the nodes into java arrays.
 */
    j_xnode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_xnode == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_xnode,
        0,
        numnode,
        (double *)xnode
    );

    j_ynode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_ynode == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_ynode,
        0,
        numnode,
        (double *)ynode
    );

    j_znode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_znode == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_znode,
        0,
        numnode,
        (double *)znode
    );

    j_nodeflags = (*jenv)->NewIntArray (jenv, numnode);
    if (j_nodeflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_nodeflags,
        0,
        numnode,
        jnodeflags
    );

    j_numnode = (jint)numnode;

/*
 * Put the edges into java arrays.
 */
    j_n1edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_n1edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_n1edge,
        0,
        numedge,
        jn1edge
    );

    j_n2edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_n2edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_n2edge,
        0,
        numedge,
        jn2edge
    );

    j_t1edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_t1edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_t1edge,
        0,
        numedge,
        jt1edge
    );

    j_t2edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_t2edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_t2edge,
        0,
        numedge,
        jt2edge
    );

    j_edgeflags = (*jenv)->NewIntArray (jenv, numedge);
    if (j_edgeflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_edgeflags,
        0,
        numedge,
        jedgeflags
    );

    j_numedge = (jint)numedge;

/*
 * Put the triangles into java arrays.
 */
    j_e1tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e1tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e1tri,
        0,
        numtri,
        je1tri
    );

    j_e2tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e2tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e2tri,
        0,
        numtri,
        je2tri
    );

    j_e3tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e3tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        (*jenv)->DeleteLocalRef (jenv, j_e2tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e3tri,
        0,
        numtri,
        je3tri
    );

    j_triflags = (*jenv)->NewIntArray (jenv, numtri);
    if (j_triflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        (*jenv)->DeleteLocalRef (jenv, j_e2tri);
        (*jenv)->DeleteLocalRef (jenv, j_e3tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_triflags,
        0,
        numtri,
        jtriflags
    );

    j_numtri = (jint)numtri;

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        StoreTriMeshMethodID,
        j_xnode,
        j_ynode,
        j_znode,
        j_nodeflags,
        j_numnode,
        j_n1edge,
        j_n2edge,
        j_t1edge,
        j_t2edge,
        j_edgeflags,
        j_numedge,
        j_e1tri,
        j_e2tri,
        j_e3tri,
        j_triflags,
        j_numtri
    );

/*
 * Delete the local java arrays.
 */
    (*jenv)->DeleteLocalRef (jenv, j_xnode);
    (*jenv)->DeleteLocalRef (jenv, j_ynode);
    (*jenv)->DeleteLocalRef (jenv, j_znode);
    (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
    (*jenv)->DeleteLocalRef (jenv, j_n1edge);
    (*jenv)->DeleteLocalRef (jenv, j_n2edge);
    (*jenv)->DeleteLocalRef (jenv, j_t1edge);
    (*jenv)->DeleteLocalRef (jenv, j_t2edge);
    (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
    (*jenv)->DeleteLocalRef (jenv, j_e1tri);
    (*jenv)->DeleteLocalRef (jenv, j_e2tri);
    (*jenv)->DeleteLocalRef (jenv, j_e3tri);
    (*jenv)->DeleteLocalRef (jenv, j_triflags);
}

/*------------------------------------------------------------------------*/

static void write_add_tri_mesh (
    double      *xnode,
    double      *ynode,
    double      *znode,
    int         *nodeflags,
    int         numnode,
    int         *n1edge,
    int         *n2edge,
    int         *t1edge,
    int         *t2edge,
    int         *edgeflags,
    int         numedge,
    int         *e1tri,
    int         *e2tri,
    int         *e3tri,
    int         *triflags,
    int         numtri)
{
    int    i, ndo;

    ndo = numnode;
    if (ndo > MAX_TRI_NDO) ndo = MAX_TRI_NDO;
    fprintf (primfile, "\nAddTriMesh method\n");
    fprintf (primfile,
       "  number of nodes = %d  node list (10 max) follows:\n", numnode);
    for (i=0; i<ndo; i++) {
      fprintf (primfile, "    x = %.5f   y = %.5f   z = %.5f\n",
                xnode[i], ynode[i], znode[i]);
    }
    fprintf (primfile,
       "  number of edges = %d  edge list (10 max) follows:\n", numedge);
    ndo = numedge;
    if (ndo > MAX_TRI_NDO) ndo = MAX_TRI_NDO;
    for (i=0; i<ndo; i++) {
      fprintf (primfile,
        "    node1 = %d   node2 = %d    tri1 = %d   tri2 = %d\n",
          n1edge[i], n2edge[i], t1edge[i], t2edge[i]);
    }
    fprintf (primfile,
       "  number of triangles = %d  triangle list (10 max) follows:\n", numtri);
    ndo = numtri;
    if (ndo > MAX_TRI_NDO) ndo = MAX_TRI_NDO;
    for (i=0; i<ndo; i++) {
      if (triflags == NULL) {
        fprintf (primfile, "    edge1 = %d   edge2 = %d   edge3 = %d\n",
          e1tri[i], e2tri[i], e3tri[i]);
      }
      else {
        fprintf (primfile,
          "    edge1 = %d   edge2 = %d   edge3 = %d   flag = %d\n",
          e1tri[i], e2tri[i], e3tri[i], triflags[i]);
      }
    }
}


void jni_call_add_tri_mesh_method (
    void  *v_jenv,
    void  *v_jobj,
    double      *xnode,
    double      *ynode,
    double      *znode,
    int         *nodeflags,
    int         numnode,
    int         *n1edge,
    int         *n2edge,
    int         *t1edge,
    int         *t2edge,
    int         *edgeflags,
    int         numedge,
    int         *e1tri,
    int         *e2tri,
    int         *e3tri,
    int         *triflags,
    int         numtri)
{
    jint        *jnodeflags;
    jint        *jn1edge,
                *jn2edge,
                *jt1edge,
                *jt2edge,
                *jedgeflags,
                *je1tri,
                *je2tri,
                *je3tri,
                *jtriflags;

    jdoubleArray        j_xnode;
    jdoubleArray        j_ynode;
    jdoubleArray        j_znode;
    jintArray           j_nodeflags;
    jint                j_numnode;
    jintArray           j_n1edge;
    jintArray           j_n2edge;
    jintArray           j_t1edge;
    jintArray           j_t2edge;
    jintArray           j_edgeflags;
    jint                j_numedge;
    jintArray           j_e1tri;
    jintArray           j_e2tri;
    jintArray           j_e3tri;
    jintArray           j_triflags;
    jint                j_numtri;

    if (primfile != NULL) {
        write_add_tri_mesh (
            xnode,
            ynode,
            znode,
            nodeflags,
            numnode,
            n1edge,
            n2edge,
            t1edge,
            t2edge,
            edgeflags,
            numedge,
            e1tri,
            e2tri,
            e3tri,
            triflags,
            numtri);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddTriMeshMethodID = (*jenv)->GetMethodID (jenv, cls, "addNativeTriMesh",
                                         "([D[D[D[II[I[I[I[I[II[I[I[I[II)V");
    if (AddTriMeshMethodID == NULL) {
        return;
    }

/*
 * If the native int is not 32 bit signed, need to convert
 * all the int arrays to 32 bit signed.
 */
#if INT_MAX == 2147483647
    jnodeflags = (jint *)nodeflags;
    jn1edge = (jint *)n1edge;
    jn2edge = (jint *)n2edge;
    jt1edge = (jint *)t1edge;
    jt2edge = (jint *)t2edge;
    jedgeflags = (jint *)edgeflags;
    je1tri = (jint *)e1tri;
    je2tri = (jint *)e2tri;
    je3tri = (jint *)e3tri;
    jtriflags = (jint *)triflags;
#else
    #error The trimesh native calc does not support non 32 bit signed int yet.
#endif

/*
 * Put the nodes into java arrays.
 */
    j_xnode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_xnode == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_xnode,
        0,
        numnode,
        xnode
    );

    j_ynode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_ynode == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_ynode,
        0,
        numnode,
        ynode
    );

    j_znode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_znode == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_znode,
        0,
        numnode,
        znode
    );

    j_nodeflags = (*jenv)->NewIntArray (jenv, numnode);
    if (j_nodeflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_nodeflags,
        0,
        numnode,
        jnodeflags
    );

    j_numnode = (jint)numnode;

/*
 * Put the edges into java arrays.
 */
    j_n1edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_n1edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_n1edge,
        0,
        numedge,
        jn1edge
    );

    j_n2edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_n2edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_n2edge,
        0,
        numedge,
        jn2edge
    );

    j_t1edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_t1edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_t1edge,
        0,
        numedge,
        jt1edge
    );

    j_t2edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_t2edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_t2edge,
        0,
        numedge,
        jt2edge
    );

    j_edgeflags = (*jenv)->NewIntArray (jenv, numedge);
    if (j_edgeflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_edgeflags,
        0,
        numedge,
        jedgeflags
    );

    j_numedge = (jint)numedge;

/*
 * Put the triangles into java arrays.
 */
    j_e1tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e1tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e1tri,
        0,
        numtri,
        je1tri
    );

    j_e2tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e2tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e2tri,
        0,
        numtri,
        je2tri
    );

    j_e3tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e3tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        (*jenv)->DeleteLocalRef (jenv, j_e2tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e3tri,
        0,
        numtri,
        je3tri
    );

    j_triflags = (*jenv)->NewIntArray (jenv, numtri);
    if (j_triflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        (*jenv)->DeleteLocalRef (jenv, j_e2tri);
        (*jenv)->DeleteLocalRef (jenv, j_e3tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_triflags,
        0,
        numtri,
        jtriflags
    );

    j_numtri = (jint)numtri;

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddTriMeshMethodID,
        j_xnode,
        j_ynode,
        j_znode,
        j_nodeflags,
        j_numnode,
        j_n1edge,
        j_n2edge,
        j_t1edge,
        j_t2edge,
        j_edgeflags,
        j_numedge,
        j_e1tri,
        j_e2tri,
        j_e3tri,
        j_triflags,
        j_numtri
    );

/*
 * Delete the local java arrays.
 */
    (*jenv)->DeleteLocalRef (jenv, j_xnode);
    (*jenv)->DeleteLocalRef (jenv, j_ynode);
    (*jenv)->DeleteLocalRef (jenv, j_znode);
    (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
    (*jenv)->DeleteLocalRef (jenv, j_n1edge);
    (*jenv)->DeleteLocalRef (jenv, j_n2edge);
    (*jenv)->DeleteLocalRef (jenv, j_t1edge);
    (*jenv)->DeleteLocalRef (jenv, j_t2edge);
    (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
    (*jenv)->DeleteLocalRef (jenv, j_e1tri);
    (*jenv)->DeleteLocalRef (jenv, j_e2tri);
    (*jenv)->DeleteLocalRef (jenv, j_e3tri);
    (*jenv)->DeleteLocalRef (jenv, j_triflags);

    return;

}

/*------------------------------------------------------------------------*/

static void write_add_grid (
    double *data,
    double *derror,
    char *mask,
    int ncol,
    int nrow,
    double xmin,
    double ymin,
    double xmax,
    double ymax,
    int npts)
{
    int  i, j, k, k2, iskp, iskp2, jskp, jskp2;

    iskp = nrow / 20;
    if (iskp < 1) iskp = 1;
    iskp2 = iskp / 2;
    jskp = ncol / 20;
    if (jskp < 1) jskp = 1;
    jskp2 = jskp / 2;

    fprintf (primfile, "\nWrite add grid method\n");
    fprintf (primfile, "  nrow = %d  ncol = %d  npts = %d\n",
               nrow, ncol, npts);
    fprintf (primfile,
               "  xmin = %.5f  ymin = %.5f  xmax = %.5f  ymax = %.5f\n",
               xmin, ymin, xmax, ymax);
    for (i=iskp2; i<nrow; i+=iskp) { 
      k2 = i * nrow;
      for (j=jskp2; j<ncol; j+=jskp) {
        k = k2 + j;
        fprintf (primfile, "    row %d  col %d:  %.5f\n", i, j, data[k]);
      }
    }
}


void jni_call_add_grid_method (
    void  *v_jenv,
    void  *v_jobj,
    double *data,
    double *derror,
    char *mask,
    int ncol,
    int nrow,
    double xmin,
    double ymin,
    double xmax,
    double ymax,
    int npts)
{

    jdoubleArray    j_data;
    jdoubleArray    j_derror;
    jbyteArray      j_mask;
    int             ntot;

    if (primfile != NULL) {
        write_add_grid (
            data,
            derror,
            mask,
            ncol,
            nrow,
            xmin,
            ymin,
            xmax,
            ymax,
            npts);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddGridMethodID = (*jenv)->GetMethodID (jenv, cls, "addNativeGrid",
                                         "([D[D[BIIDDDDI)V");
    if (AddGridMethodID == NULL) {
        return;
    }

    ntot = ncol * nrow;

    j_data = (*jenv)->NewDoubleArray (jenv, (jsize)ntot);
    if (j_data == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_data,
        0,
        ntot,
        data
    );

    j_mask = (*jenv)->NewByteArray (jenv, (jsize)ntot);
    if (j_mask == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_data);
        return;
    }
    (*jenv)->SetByteArrayRegion (
        jenv,
        j_mask,
        0,
        ntot,
        (jbyte *)mask
    );

    j_derror = NULL;
    if (derror != NULL  &&  npts > 0) {
        j_derror = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
        if (j_derror == NULL) {
            (*jenv)->DeleteLocalRef (jenv, j_data);
            (*jenv)->DeleteLocalRef (jenv, j_mask);
            return;
        }
        (*jenv)->SetDoubleArrayRegion (
            jenv,
            j_derror,
            0,
            npts,
            derror
        );
    }

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddGridMethodID,
        j_data,
        j_derror,
        j_mask,
        (jint)ncol,
        (jint)nrow,
        (jdouble)xmin,
        (jdouble)ymin,
        (jdouble)xmax,
        (jdouble)ymax,
        (jint)npts
    );

    (*jenv)->DeleteLocalRef (jenv, j_data);
    (*jenv)->DeleteLocalRef (jenv, j_mask);
    if (j_derror != NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_derror);
    }

    return;

}

/*------------------------------------------------------------------------*/

static void write_add_draped_line (
    double *x,
    double *y,
    double *z,
    int npts)
{
}


void jni_call_add_draped_line_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile) {
        write_add_draped_line (
            x,
            y,
            z,
            npts);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddDrapedLineMethodID =
            (*jenv)->GetMethodID (jenv, cls, "addNativeDrapedLine",
                                         "([D[D[DI)V");
    if (AddDrapedLineMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddDrapedLineMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}

/*------------------------------------------------------------------------*/

static void write_add_extended_centerline (
    double *x,
    double *y,
    double *z,
    int npts)
{
}

void jni_call_add_extended_centerline_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_add_extended_centerline (
            x,
            y,
            z,
            npts);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddExtendedCenterlineMethodID =
            (*jenv)->GetMethodID (jenv, cls, "addExtendedCenterline",
                                         "([D[D[DI)V");
    if (AddExtendedCenterlineMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddExtendedCenterlineMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}


/*------------------------------------------------------------------------*/

static void write_add_work_poly (
    double *x,
    double *y,
    double *z,
    int npts)
{
}

void jni_call_add_work_poly_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_add_work_poly (
            x,
            y,
            z,
            npts);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddWorkPolyMethodID =
            (*jenv)->GetMethodID (jenv, cls, "addWorkPoly",
                                         "([D[D[DI)V");
    if (AddWorkPolyMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddWorkPolyMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}


/*------------------------------------------------------------------------*/

void write_add_poly_label (
    double x,
    double y,
    int label)
{
}

void jni_call_add_poly_label_method (
    void  *v_jenv,
    void  *v_jobj,
    double x,
    double y,
    int label)
{

    if (primfile != NULL) {
        write_add_poly_label (
            x,
            y,
            label);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddPolyLabelMethodID =
            (*jenv)->GetMethodID (jenv, cls, "addPolyLabel",
                                         "(DDI)V");
    if (AddPolyLabelMethodID == NULL) {
        return;
    }

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddPolyLabelMethodID,
        (jdouble)x,
        (jdouble)y,
        (jint)label
    );

    return;
}

/*------------------------------------------------------------------------*/

static void write_add_poly_tmesh (
    int         pnum,
    double      *xnode,
    double      *ynode,
    double      *znode,
    int         *nodeflags,
    int         numnode,
    int         *n1edge,
    int         *n2edge,
    int         *t1edge,
    int         *t2edge,
    int         *edgeflags,
    int         numedge,
    int         *e1tri,
    int         *e2tri,
    int         *e3tri,
    int         *triflags,
    int         numtri)
{
}

void jni_call_add_poly_tmesh_method (
    void  *v_jenv,
    void  *v_jobj,
    int         pnum,
    double      *xnode,
    double      *ynode,
    double      *znode,
    int         *nodeflags,
    int         numnode,
    int         *n1edge,
    int         *n2edge,
    int         *t1edge,
    int         *t2edge,
    int         *edgeflags,
    int         numedge,
    int         *e1tri,
    int         *e2tri,
    int         *e3tri,
    int         *triflags,
    int         numtri)
{
    jint        *jnodeflags;
    jint        *jn1edge,
                *jn2edge,
                *jt1edge,
                *jt2edge,
                *jedgeflags,
                *je1tri,
                *je2tri,
                *je3tri,
                *jtriflags;

    jdoubleArray        j_xnode;
    jdoubleArray        j_ynode;
    jdoubleArray        j_znode;
    jintArray           j_nodeflags;
    jint                j_numnode;
    jintArray           j_n1edge;
    jintArray           j_n2edge;
    jintArray           j_t1edge;
    jintArray           j_t2edge;
    jintArray           j_edgeflags;
    jint                j_numedge;
    jintArray           j_e1tri;
    jintArray           j_e2tri;
    jintArray           j_e3tri;
    jintArray           j_triflags;
    jint                j_numtri;

    if (primfile != NULL) {
        write_add_poly_tmesh (
            pnum,
            xnode,
            ynode,
            znode,
            nodeflags,
            numnode,
            n1edge,
            n2edge,
            t1edge,
            t2edge,
            edgeflags,
            numedge,
            e1tri,
            e2tri,
            e3tri,
            triflags,
            numtri);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddPolyTmeshMethodID = (*jenv)->GetMethodID (jenv, cls, "addPolyTmesh",
                                         "(I[D[D[D[II[I[I[I[I[II[I[I[I[II)V");
    if (AddPolyTmeshMethodID == NULL) {
        return;
    }
/*
 * If the native int is not 32 bit signed, don't support.
 */
#if INT_MAX == 2147483647
    jnodeflags = (jint *)nodeflags;
    jn1edge = (jint *)n1edge;
    jn2edge = (jint *)n2edge;
    jt1edge = (jint *)t1edge;
    jt2edge = (jint *)t2edge;
    jedgeflags = (jint *)edgeflags;
    je1tri = (jint *)e1tri;
    je2tri = (jint *)e2tri;
    je3tri = (jint *)e3tri;
    jtriflags = (jint *)triflags;
#else
    #error The surfaceworks native code does not support non 32 bit signed int.
#endif

/*
 * Put the nodes into java arrays.
 */
    j_xnode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_xnode == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_xnode,
        0,
        numnode,
        xnode
    );

    j_ynode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_ynode == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_ynode,
        0,
        numnode,
        ynode
    );

    j_znode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_znode == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_znode,
        0,
        numnode,
        znode
    );

    j_nodeflags = (*jenv)->NewIntArray (jenv, numnode);
    if (j_nodeflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_nodeflags,
        0,
        numnode,
        jnodeflags
    );

    j_numnode = (jint)numnode;

/*
 * Put the edges into java arrays.
 */
    j_n1edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_n1edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_n1edge,
        0,
        numedge,
        jn1edge
    );

    j_n2edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_n2edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_n2edge,
        0,
        numedge,
        jn2edge
    );

    j_t1edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_t1edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_t1edge,
        0,
        numedge,
        jt1edge
    );

    j_t2edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_t2edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_t2edge,
        0,
        numedge,
        jt2edge
    );

    j_edgeflags = (*jenv)->NewIntArray (jenv, numedge);
    if (j_edgeflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_edgeflags,
        0,
        numedge,
        jedgeflags
    );

    j_numedge = (jint)numedge;

/*
 * Put the triangles into java arrays.
 */
    j_e1tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e1tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e1tri,
        0,
        numtri,
        je1tri
    );

    j_e2tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e2tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e2tri,
        0,
        numtri,
        je2tri
    );

    j_e3tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e3tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        (*jenv)->DeleteLocalRef (jenv, j_e2tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e3tri,
        0,
        numtri,
        je3tri
    );

    j_triflags = (*jenv)->NewIntArray (jenv, numtri);
    if (j_triflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        (*jenv)->DeleteLocalRef (jenv, j_e2tri);
        (*jenv)->DeleteLocalRef (jenv, j_e3tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_triflags,
        0,
        numtri,
        jtriflags
    );

    j_numtri = (jint)numtri;

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddPolyTmeshMethodID,
        (jint)pnum,
        j_xnode,
        j_ynode,
        j_znode,
        j_nodeflags,
        j_numnode,
        j_n1edge,
        j_n2edge,
        j_t1edge,
        j_t2edge,
        j_edgeflags,
        j_numedge,
        j_e1tri,
        j_e2tri,
        j_e3tri,
        j_triflags,
        j_numtri
    );

/*
 * Delete the local java arrays.
 */
    (*jenv)->DeleteLocalRef (jenv, j_xnode);
    (*jenv)->DeleteLocalRef (jenv, j_ynode);
    (*jenv)->DeleteLocalRef (jenv, j_znode);
    (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
    (*jenv)->DeleteLocalRef (jenv, j_n1edge);
    (*jenv)->DeleteLocalRef (jenv, j_n2edge);
    (*jenv)->DeleteLocalRef (jenv, j_t1edge);
    (*jenv)->DeleteLocalRef (jenv, j_t2edge);
    (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
    (*jenv)->DeleteLocalRef (jenv, j_e1tri);
    (*jenv)->DeleteLocalRef (jenv, j_e2tri);
    (*jenv)->DeleteLocalRef (jenv, j_e3tri);
    (*jenv)->DeleteLocalRef (jenv, j_triflags);

    return;

}

/*------------------------------------------------------------------------*/

static void write_set_vert_baseline (
    double c1,
    double c2,
    double c3,
    double x0,
    double y0,
    double z0,
    int flag)
{
}


void jni_call_set_vert_baseline_method (
    void  *v_jenv,
    void  *v_jobj,
    double c1,
    double c2,
    double c3,
    double x0,
    double y0,
    double z0,
    int flag)
{

    if (primfile != NULL) {
        write_set_vert_baseline (
            c1,
            c2,
            c3,
            x0,
            y0,
            z0,
            flag);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SetVertBaselineMethodID =
            (*jenv)->GetMethodID (jenv, cls, "setVertBaseline",
                                         "(DDDDDDI)V");
    if (SetVertBaselineMethodID == NULL) {
        return;
    }

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SetVertBaselineMethodID,
        (jdouble)c1,
        (jdouble)c2,
        (jdouble)c3,
        (jdouble)x0,
        (jdouble)y0,
        (jdouble)z0,
        (jint)flag
    );

    return;
}


/*------------------------------------------------------------------------*/

static void write_add_corrected_centerline (
    double *x,
    double *y,
    double *z,
    int npts)
{
}

void jni_call_add_corrected_centerline_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_add_corrected_centerline (
            x,
            y,
            z,
            npts);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddCorrectedCenterlineMethodID =
            (*jenv)->GetMethodID (jenv, cls, "addCorrectedCenterline",
                                         "([D[D[DI)V");
    if (AddCorrectedCenterlineMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddCorrectedCenterlineMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}

/*------------------------------------------------------------------------*/

static void write_add_drape_line (
    double *x,
    double *y,
    double *z,
    int npts)
{
}

void jni_call_add_drape_line_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_add_drape_line (
            x,
            y,
            z,
            npts);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddDrapeLineMethodID =
            (*jenv)->GetMethodID (jenv, cls, "addDrapeLine",
                                         "([D[D[DI)V");
    if (AddDrapeLineMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddDrapeLineMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}

/*------------------------------------------------------------------------*/

static void write_add_patch_points (
    int    patchid,
    double *x,
    double *y,
    double *z,
    int npts)
{
}

void jni_call_add_patch_points_method (
    void  *v_jenv,
    void  *v_jobj,
    int    patchid,
    double *x,
    double *y,
    double *z,
    int npts)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_add_patch_points (
            patchid,
            x,
            y,
            z,
            npts);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID CopyPatchPointsMethodID = (*jenv)->GetMethodID (jenv, cls, "copyPatchPoints",
                                           "(I[D[D[DI)V");
    if (CopyPatchPointsMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        CopyPatchPointsMethodID,
        (jint)patchid,
        j_x,
        j_y,
        j_z,
        (jint)npts
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}

/*------------------------------------------------------------------------*/

static void write_add_border_segment (
    double *x,
    double *y,
    double *z,
    int npts,
    int type,
    int direction)
{
}

void jni_call_add_border_segment_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts,
    int type,
    int direction)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_add_border_segment (
            x,
            y,
            z,
            npts,
            type,
            direction);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddBorderSegmentMethodID = (*jenv)->GetMethodID
        (jenv, cls, "addBorderSegment", "([D[D[DIII)V");
    if (AddBorderSegmentMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddBorderSegmentMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts,
        (jint)type,
        (jint)direction
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}

/*------------------------------------------------------------------------*/

void write_start_proto_patch (int id)
{
}

void jni_call_start_proto_patch_method (
    void  *v_jenv,
    void  *v_jobj,
    int   id
)
{

    if (primfile != NULL) {
        write_start_proto_patch (id);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID StartProtoPatchMethodID =
        (*jenv)->GetMethodID (jenv, cls, "startProtoPatch", "(I)V");
    if (StartProtoPatchMethodID == NULL) {
        return;
    }

    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        StartProtoPatchMethodID,
        (jint)id);

    return;
}

/*------------------------------------------------------------------------*/

static void write_end_proto_patch (int id)
{
}

void jni_call_end_proto_patch_method (
    void  *v_jenv,
    void  *v_jobj,
    int id)
{
    if (primfile != NULL) {
        write_end_proto_patch (id);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID EndProtoPatchMethodID =
        (*jenv)->GetMethodID (jenv, cls, "endProtoPatch", "(I)V");
    if (EndProtoPatchMethodID == NULL) {
        return;
    }

    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        EndProtoPatchMethodID,
        (jint)id);

    return;
}

/*------------------------------------------------------------------------*/

static void write_add_patch_line (
    double *x,
    double *y,
    double *z,
    int npts,
    int flag)
{
}

void jni_call_add_patch_line_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts,
    int flag)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_add_patch_line (
            x,
            y,
            z,
            npts,
            flag);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddPatchLineMethodID = (*jenv)->GetMethodID (jenv, cls, "addPatchLine",
                                           "([D[D[DII)V");
    if (AddPatchLineMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddPatchLineMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts,
        (jint)flag
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}

/*------------------------------------------------------------------------*/

static void write_set_outline (
    double *x,
    double *y,
    int npts)
{
}

int jni_call_set_outline_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    int npts)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;

    if (primfile != NULL) {
        write_set_outline (
            x,
            y,
            npts);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return 1;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SetOutlineMethodID = (*jenv)->GetMethodID (jenv, cls, "setOutline",
                              "([D[DI)V");
    if (SetOutlineMethodID == NULL) {
        return -1;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return -1;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_x);
        return -1;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SetOutlineMethodID,
        j_x,
        j_y,
        (jint)npts
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);

    return 1;
}

/*------------------------------------------------------------------------*/

static void write_set_outline3d (
    double *x,
    double *y,
    double *z,
    int npts)
{
}

int jni_call_set_outline3d_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_set_outline3d (
            x,
            y,
            z,
            npts);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return 1;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID SetOutline3DMethodID = (*jenv)->GetMethodID (jenv, cls, "setOutline",
                              "([D[D[DI)V");
    if (SetOutline3DMethodID == NULL) {
        return -1;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return -1;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_x);
        return -1;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_x);
        (*jenv)->DeleteLocalRef (jenv, j_y);
        return -1;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        SetOutline3DMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return 1;
}

/*------------------------------------------------------------------------*/

static void write_add_proto_patch_contact_line (
    double *x,
    double *y,
    double *z,
    int npts,
    int patchid1,
    int patchid2)
{
}

void jni_call_add_proto_patch_contact_line_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts,
    int patchid1,
    int patchid2)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_add_proto_patch_contact_line (
            x,
            y,
            z,
            npts,
            patchid1,
            patchid2);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddProtoPatchContactLineMethodID =
            (*jenv)->GetMethodID (jenv, cls, "addProtoPatchContactLine",
                                         "([D[D[DIII)V");
    if (AddProtoPatchContactLineMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddProtoPatchContactLineMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts,
        (jint)patchid1,
        (jint)patchid2
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}


/*------------------------------------------------------------------------*/

static void write_add_draped_points (
    double *x,
    double *y,
    double *z,
    int npts)
{
}

void jni_call_add_draped_points_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_add_draped_points (
            x,
            y,
            z,
            npts);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddDrapedPointsMethodID =
            (*jenv)->GetMethodID (jenv, cls, "addNativeDrapedPoints",
                                         "([D[D[DI)V");
    if (AddDrapedPointsMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddDrapedPointsMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}

/*------------------------------------------------------------------------*/

static void write_add_native_tindex_result (
    int         *tmeshid,
    int         *trinum,
    int         nresult)
{
}

void jni_call_add_native_tindex_result_method (
    void  *v_jenv,
    void  *v_jobj,
    int         *tmeshid,
    int         *trinum,
    int         nresult)
{
    jint        *jtmeshid;
    jint        *jtrinum;
    jintArray           j_tmeshid;
    jintArray           j_trinum;
    jint                j_nresult;

    if (primfile != NULL) {
        write_add_native_tindex_result (
            tmeshid,
            trinum,
            nresult);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddNativeTindexResultMethodID =
            (*jenv)->GetMethodID (jenv, cls, "addNativeTindexResult",
                                           "([I[II)V");
    if (AddNativeTindexResultMethodID == NULL) {
        return;
    }
/*
 * If the native int is not 32 bit signed, don't support.
 */
#if INT_MAX == 2147483647
    jtmeshid = (jint *)tmeshid;
    jtrinum = (jint *)trinum;
#else
    #error The surfaceworks native code does not support non 32 bit signed int.
#endif

/*
 * Put the triangles into java arrays.
 */
    j_tmeshid = (*jenv)->NewIntArray (jenv, nresult);
    if (j_tmeshid == NULL) {
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_tmeshid,
        0,
        nresult,
        jtmeshid
    );

    j_trinum = (*jenv)->NewIntArray (jenv, nresult);
    if (j_trinum == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_tmeshid);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_trinum,
        0,
        nresult,
        jtrinum
    );

    j_nresult = (jint)nresult;

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddNativeTindexResultMethodID,
        j_tmeshid,
        j_trinum,
        j_nresult
    );

/*
 * Delete the local java arrays.
 */
    (*jenv)->DeleteLocalRef (jenv, j_tmeshid);
    (*jenv)->DeleteLocalRef (jenv, j_trinum);

    return;

}


/*
 * Get a command and its data from java.  Convert to 32 bit ints if needed
 * and send the data to the surfaceworks command processor.  Return values are
 * sent back to the java side by calling java methods with appropriate data
 * to build the objects needed for the particular command.  When this function
 * returns, the results will be waiting on the java side, or else an error
 * condition will be reported.  If there is an error, this function will
 * also return -1.  It is possible for this function to return -1 without
 * any error report on the java side.  This can happen if the command id is
 * bad or if a memory allocation error occurs in this function.  In any case,
 * a return value of -1 means that a serious error occurred.
 *
 * This is the static version of the function.  It is only used when no return data
 * are to be sent back to the java side.
 */
JNIEXPORT jlong JNICALL Java_csw_jsurfaceworks_src_JSurfaceWorksBase_sendStaticCommand
   (JNIEnv *jnienv,
    jclass cls,
    jint j_commandID,
    jint j_expect_return,
    jintArray j_ilist,
    jlongArray j_llist,
    jdoubleArray j_dlist,
    jstring j_cdata,
    jintArray j_idata,
    jfloatArray j_fdata,
    jdoubleArray j_ddata,
    jint   j_tid
   )
{
    jint             *idata;
    float            *fdata;
    double           *ddata;

    long              status;
    int              command_id;
    int              threadid;

    jint             *ilist;
    jlong            *llist;
    double           *dlist;
    char             *cdata;

    long             long_list_array[MAX_LIST_ARRAY];
    long             *llist2;
    int              i, lsize;

    void    *v_jenv = (void *)jnienv;
    void    *v_jobj = NULL;

    threadid = (int)j_tid;

/*
 *  This block of code can be uncommented to enable debug of
 *  this code when called from java.
    static int       first = 1;
    if (first) {
        first = 0;
        DebugBreak ();
    }
 */

    cls = cls;

/*
 * If the int type is not a 32 bit signed integer, a conversion
 * from jint to int will be needed.  If the int is smaller than
 * a 32 bit signed then don't even compile since there will be
 * all kinds of problems.
 */
#if INT_MAX != 2147483647
    jsize            ntot;
    int              ilist2[MAX_LIST_ARRAY];
    int              *idata2[MAX_INT_ARRAYS];
    int              *itmp2;
    int              ibad;
  #if INT_MAX  <  2147483647
    #error The int data type with this compiler is less than 32 bits, ABORT.
  #endif
#endif

    ilist = NULL;
    llist = NULL;
    dlist = NULL;
    cdata = NULL;

    idata = NULL;
    fdata = NULL;
    ddata = NULL;

/*
 * Get native array references from the java arrays.  Do not explicitly
 * ask for copies in this.  However, java may make copies and these
 * native array references all need to be released when the native side
 * no longer needs them.
 */
    command_id = (int)j_commandID;
    if (j_ilist) {
        ilist = (*jnienv)->GetIntArrayElements (jnienv, j_ilist, JNI_FALSE);
    }
    if (j_llist) {
        llist = (*jnienv)->GetLongArrayElements (jnienv, j_llist, JNI_FALSE);
    }
    if (j_cdata) {
        cdata = (char *)(*jnienv)->GetStringUTFChars (jnienv, j_cdata, JNI_FALSE);
    }
    if (j_dlist) {
        dlist = (*jnienv)->GetDoubleArrayElements (jnienv, j_dlist, JNI_FALSE);
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
 * If the long type in C is 64 bits, just cast from the jlong llist to
 * the llist2 array.
 */
    if (llist != NULL) {
        lsize = sizeof(long);
        if (lsize == 8) {
            llist2 = (long *)llist;
        }
        else {
            llist2 = long_list_array;
            for (i=0; i<MAX_LIST_ARRAY; i++) {
                llist2[i] = (long)llist[i];
            }
        }
    }
    else {
        llist2 = NULL;
    }

/*
 * If an int in C is 32 bit signed, it is the
 * same type as a jint, so I can cast the jint
 * pointers to int safely.
 */
#if INT_MAX == 2147483647
    status =
    sw_process_command (v_jenv, v_jobj,
                        command_id,
                        threadid,
                        (int *)ilist,
                        llist2,
                        dlist,
                        cdata,
                        (int *)idata,
                        fdata,
                        ddata
                       );

/*
 * The jint is not the same size as int, so I need to
 * copy the input and modifiable jint arrays into int
 * arrays.
 */
#else

    if (ilist != NULL) {
        for (i=0; i<MAX_LIST_SIZE; i++) {
            ilist2[i] = (int)ilist[i];
        }
    }
    if (llist != NULL) {
        for (i=0; i<MAX_LIST_SIZE; i++) {
            llist2[i] = (int)llist[i];
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
        sw_process_command (v_jenv, v_jobj,
                            command_id,
                            threadid,
                            ilist2,
                            llist2,
                            dlist,
                            cdata,
                            idata2,
                            fdata,
                            ddata,
                            idataMod2,
                            fdataMod,
                            ddataMod,
                            idataOut2, isizeOut,
                            fdataOut, fsizeOut,
                            ddataOut, dsizeOut
                           );
    }
    else {
        status = -1;
    }

  /*
   * Free int input arrays.
   */
    free (idata2);

#endif

/*
 * Even though no copies of the elements were made above, the release
 * calls are needed.  Without these calls, the memory usage grows without end.
 */
    if (j_ilist) {
        (*jnienv)->ReleaseIntArrayElements (jnienv, j_ilist, ilist, JNI_FALSE);
    }
    if (j_llist) {
        (*jnienv)->ReleaseLongArrayElements (jnienv, j_llist, llist, JNI_FALSE);
    }
    if (j_cdata) {
        (*jnienv)->ReleaseStringUTFChars (jnienv, j_cdata, cdata);
    }
    if (j_dlist) {
        (*jnienv)->ReleaseDoubleArrayElements (jnienv, j_dlist, dlist, JNI_FALSE);
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

    return (jlong)status;

}

/*------------------------------------------------------------------------*/

static void write_add_split_line (
    double *x,
    double *y,
    double *z,
    int npts,
    int patchid1,
    int patchid2)
{
}

void jni_call_add_split_line_method (
    void  *v_jenv,
    void  *v_jobj,
    double *x,
    double *y,
    double *z,
    int npts,
    int patchid1,
    int patchid2)
{
    jdoubleArray    j_x;
    jdoubleArray    j_y;
    jdoubleArray    j_z;

    if (primfile != NULL) {
        write_add_split_line (
            x,
            y,
            z,
            npts,
            patchid1,
            patchid2);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddSplitLineMethodID =
            (*jenv)->GetMethodID (jenv, cls, "addSplitLine",
                                         "([D[D[DIII)V");
    if (AddSplitLineMethodID == NULL) {
        return;
    }

    j_x = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_x == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_x,
        0,
        npts,
        x
    );

    j_y = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_y == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_y,
        0,
        npts,
        y
    );

    j_z = (*jenv)->NewDoubleArray (jenv, (jsize)npts);
    if (j_z == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_z,
        0,
        npts,
        z
    );

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddSplitLineMethodID,
        j_x,
        j_y,
        j_z,
        (jint)npts,
        (jint)patchid1,
        (jint)patchid2
    );

    (*jenv)->DeleteLocalRef (jenv, j_x);
    (*jenv)->DeleteLocalRef (jenv, j_y);
    (*jenv)->DeleteLocalRef (jenv, j_z);

    return;
}

/*------------------------------------------------------------------------*/

static void write_add_fault_major_minor (
    int         id,
    int         major)
{
}

void jni_call_add_fault_major_minor_method (
    void  *v_jenv,
    void  *v_jobj,
    int         id,
    int         major)
{
    jint        j_id;
    jint        j_major;

    if (primfile != NULL) {
        write_add_fault_major_minor (
            id,
            major);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddFaultMajorMinorMethodID =
        (*jenv)->GetMethodID (jenv, cls, "addFaultMajorMinor", "(II)V");
    if (AddFaultMajorMinorMethodID == NULL) {
        return;
    }

    j_id = (jint)id;
    j_major = (jint)major;

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddFaultMajorMinorMethodID,
        j_id,
        j_major
    );

    return;

}


/*------------------------------------------------------------------------*/

static void write_add_native_at_tri_mesh (
    double      *xnode,
    double      *ynode,
    double      *znode,
    int         *nodeflags,
    int         numnode,
    int         *n1edge,
    int         *n2edge,
    int         *t1edge,
    int         *t2edge,
    int         *edgeflags,
    int         numedge,
    int         *e1tri,
    int         *e2tri,
    int         *e3tri,
    int         *triflags,
    int         numtri)
{
}

void jni_call_add_native_at_tri_mesh_method (
    void  *v_jenv,
    void  *v_jobj,
    double      *xnode,
    double      *ynode,
    double      *znode,
    int         *nodeflags,
    int         numnode,
    int         *n1edge,
    int         *n2edge,
    int         *t1edge,
    int         *t2edge,
    int         *edgeflags,
    int         numedge,
    int         *e1tri,
    int         *e2tri,
    int         *e3tri,
    int         *triflags,
    int         numtri)
{
    jint        *jnodeflags;
    jint        *jn1edge,
                *jn2edge,
                *jt1edge,
                *jt2edge,
                *jedgeflags,
                *je1tri,
                *je2tri,
                *je3tri,
                *jtriflags;

    jdoubleArray        j_xnode;
    jdoubleArray        j_ynode;
    jdoubleArray        j_znode;
    jintArray           j_nodeflags;
    jint                j_numnode;
    jintArray           j_n1edge;
    jintArray           j_n2edge;
    jintArray           j_t1edge;
    jintArray           j_t2edge;
    jintArray           j_edgeflags;
    jint                j_numedge;
    jintArray           j_e1tri;
    jintArray           j_e2tri;
    jintArray           j_e3tri;
    jintArray           j_triflags;
    jint                j_numtri;

    if (primfile != NULL) {
        write_add_native_at_tri_mesh (
            xnode,
            ynode,
            znode,
            nodeflags,
            numnode,
            n1edge,
            n2edge,
            t1edge,
            t2edge,
            edgeflags,
            numedge,
            e1tri,
            e2tri,
            e3tri,
            triflags,
            numtri);
    }

    if (v_jenv == NULL  ||  v_jobj == NULL) {
        return;
    }

    JNIEnv *jenv = (JNIEnv *)v_jenv;
    jobject jobj = (jobject)v_jobj;

    jclass cls = (*jenv)->GetObjectClass (jenv, jobj);

    jmethodID AddNativeAtTriMeshMethodID =
        (*jenv)->GetMethodID (jenv, cls, "addNativeAtTriMesh",
          "([D[D[D[II[I[I[I[I[II[I[I[I[II)V");
    if (AddNativeAtTriMeshMethodID == NULL) {
        return;
    }

/*
 * If the native int is not 32 bit signed, need to convert
 * all the int arrays to 32 bit signed.
 */
#if INT_MAX == 2147483647
    jnodeflags = (jint *)nodeflags;
    jn1edge = (jint *)n1edge;
    jn2edge = (jint *)n2edge;
    jt1edge = (jint *)t1edge;
    jt2edge = (jint *)t2edge;
    jedgeflags = (jint *)edgeflags;
    je1tri = (jint *)e1tri;
    je2tri = (jint *)e2tri;
    je3tri = (jint *)e3tri;
    jtriflags = (jint *)triflags;
#else
    #error The trimesh native calc does not support non 32 bit signed int yet.
#endif

/*
 * Put the nodes into java arrays.
 */
    j_xnode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_xnode == NULL) {
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_xnode,
        0,
        numnode,
        xnode
    );

    j_ynode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_ynode == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_ynode,
        0,
        numnode,
        ynode
    );

    j_znode = (*jenv)->NewDoubleArray (jenv, numnode);
    if (j_znode == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        return;
    }
    (*jenv)->SetDoubleArrayRegion (
        jenv,
        j_znode,
        0,
        numnode,
        znode
    );

    j_nodeflags = (*jenv)->NewIntArray (jenv, numnode);
    if (j_nodeflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_nodeflags,
        0,
        numnode,
        jnodeflags
    );

    j_numnode = (jint)numnode;

/*
 * Put the edges into java arrays.
 */
    j_n1edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_n1edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_n1edge,
        0,
        numedge,
        jn1edge
    );

    j_n2edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_n2edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_n2edge,
        0,
        numedge,
        jn2edge
    );

    j_t1edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_t1edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_t1edge,
        0,
        numedge,
        jt1edge
    );

    j_t2edge = (*jenv)->NewIntArray (jenv, numedge);
    if (j_t2edge == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_t2edge,
        0,
        numedge,
        jt2edge
    );

    j_edgeflags = (*jenv)->NewIntArray (jenv, numedge);
    if (j_edgeflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_edgeflags,
        0,
        numedge,
        jedgeflags
    );

    j_numedge = (jint)numedge;

/*
 * Put the triangles into java arrays.
 */
    j_e1tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e1tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e1tri,
        0,
        numtri,
        je1tri
    );

    j_e2tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e2tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e2tri,
        0,
        numtri,
        je2tri
    );

    j_e3tri = (*jenv)->NewIntArray (jenv, numtri);
    if (j_e3tri == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        (*jenv)->DeleteLocalRef (jenv, j_e2tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_e3tri,
        0,
        numtri,
        je3tri
    );

    j_triflags = (*jenv)->NewIntArray (jenv, numtri);
    if (j_triflags == NULL) {
        (*jenv)->DeleteLocalRef (jenv, j_xnode);
        (*jenv)->DeleteLocalRef (jenv, j_ynode);
        (*jenv)->DeleteLocalRef (jenv, j_znode);
        (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
        (*jenv)->DeleteLocalRef (jenv, j_n1edge);
        (*jenv)->DeleteLocalRef (jenv, j_n2edge);
        (*jenv)->DeleteLocalRef (jenv, j_t1edge);
        (*jenv)->DeleteLocalRef (jenv, j_t2edge);
        (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
        (*jenv)->DeleteLocalRef (jenv, j_e1tri);
        (*jenv)->DeleteLocalRef (jenv, j_e2tri);
        (*jenv)->DeleteLocalRef (jenv, j_e3tri);
        return;
    }
    (*jenv)->SetIntArrayRegion (
        jenv,
        j_triflags,
        0,
        numtri,
        jtriflags
    );

    j_numtri = (jint)numtri;

/*
 * Call the java object's method.
 */
    (*jenv)->CallVoidMethod (
        jenv,
        jobj,
        AddNativeAtTriMeshMethodID,
        j_xnode,
        j_ynode,
        j_znode,
        j_nodeflags,
        j_numnode,
        j_n1edge,
        j_n2edge,
        j_t1edge,
        j_t2edge,
        j_edgeflags,
        j_numedge,
        j_e1tri,
        j_e2tri,
        j_e3tri,
        j_triflags,
        j_numtri
    );

/*
 * Delete the local java arrays.
 */
    (*jenv)->DeleteLocalRef (jenv, j_xnode);
    (*jenv)->DeleteLocalRef (jenv, j_ynode);
    (*jenv)->DeleteLocalRef (jenv, j_znode);
    (*jenv)->DeleteLocalRef (jenv, j_nodeflags);
    (*jenv)->DeleteLocalRef (jenv, j_n1edge);
    (*jenv)->DeleteLocalRef (jenv, j_n2edge);
    (*jenv)->DeleteLocalRef (jenv, j_t1edge);
    (*jenv)->DeleteLocalRef (jenv, j_t2edge);
    (*jenv)->DeleteLocalRef (jenv, j_edgeflags);
    (*jenv)->DeleteLocalRef (jenv, j_e1tri);
    (*jenv)->DeleteLocalRef (jenv, j_e2tri);
    (*jenv)->DeleteLocalRef (jenv, j_e3tri);
    (*jenv)->DeleteLocalRef (jenv, j_triflags);

    return;

}
