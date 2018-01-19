/*
 ***************************************************************************

  Prototypes for jni callback to java method functions.

 ***************************************************************************
*/

#ifndef _SURFACE_WORKS_JNI_H_
#define _SURFACE_WORKS_JNI_H_

#include <stdio.h>

  #ifdef __cplusplus
  extern "C" {
  #endif

    void jni_call_set_error_message_method (
        char *message,
        int  errnum
    );

    void jni_call_store_tri_mesh_method (
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
        int numtri
    );

    void jni_call_add_tri_mesh_method (
        double *xnodes,
        double *ynodes,
        double *znodes,
        int *nodeflags,
        int numnodes,
        int *edgenode1,
        int *edgenode2,
        int *edgetri1,
        int *edgetri2,
        int *edgeflags,
        int numedges,
        int *triedge1,
        int *triedge2,
        int *triedge3,
        int *triflags,
        int numtris
    );

    void jni_call_add_native_at_tri_mesh_method (
        double *xnodes,
        double *ynodes,
        double *znodes,
        int *nodeflags,
        int numnodes,
        int *edgenode1,
        int *edgenode2,
        int *edgetri1,
        int *edgetri2,
        int *edgeflags,
        int numedges,
        int *triedge1,
        int *triedge2,
        int *triedge3,
        int *triflags,
        int numtris
    );

    void jni_call_add_native_ndp_value_method (
        int             id,
        signed char     *v1,
        signed char     *v2,
        signed char     *v3,
        signed char     *v4,
        signed char     *p1,
        signed char     *p2,
        signed char     *p3,
        signed char     *p4,
        int             npts);

    void jni_call_add_grid_method (
        double *data,
        double *derror,
        char *mask,
        int ncol,
        int nrow,
        double xmin,
        double ymin,
        double xmax,
        double ymax,
        int npts
    );

    void jni_call_add_draped_line_method (
        double *x,
        double *y,
        double *z,
        int npts
    );

    void jni_call_add_draped_points_method (
        double *x,
        double *y,
        double *z,
        int npts
    );

    void jni_call_add_corrected_centerline_method (
        double *x,
        double *y,
        double *z,
        int npts
    );

    void jni_call_add_extended_centerline_method (
        double *x,
        double *y,
        double *z,
        int npts
    );

    void jni_call_add_work_poly_method (
        double *x,
        double *y,
        double *z,
        int npts
    );

    void jni_call_add_proto_patch_contact_line_method (
        double *x,
        double *y,
        double *z,
        int npts,
        int patchid1,
        int patchid2
    );

    void jni_call_add_split_line_method (
        double *x,
        double *y,
        double *z,
        int npts,
        int patchid1,
        int patchid2
    );

    void jni_call_add_drape_line_method (
        double *x,
        double *y,
        double *z,
        int npts
    );

    void jni_call_add_poly_label_method (
        double x,
        double y,
        int label
    );

    void jni_call_add_poly_tmesh_method (
        int    polygon_number,
        double *xnodes,
        double *ynodes,
        double *znodes,
        int *nodeflags,
        int numnodes,
        int *edgenode1,
        int *edgenode2,
        int *edgetri1,
        int *edgetri2,
        int *edgeflags,
        int numedges,
        int *triedge1,
        int *triedge2,
        int *triedge3,
        int *triflags,
        int numtris
    );

    void jni_call_set_vert_baseline_method (
        double    c1,
        double    c2,
        double    c3,
        double    x0,
        double    y0,
        double    z0,
        int       iflag
    );

    void jni_call_start_proto_patch_method (int id);
    void jni_call_end_proto_patch_method (int id);
    void jni_call_add_border_segment_method (
        double    *x,
        double    *y,
        double    *z,
        int       npts,
        int       type,
        int       direction);
    void jni_call_add_patch_points_method (
        int       patchid,
        double    *x,
        double    *y,
        double    *z,
        int       npts);
    void jni_call_add_patch_line_method (
        double    *x,
        double    *y,
        double    *z,
        int       npts,
        int       flag);

    int jni_call_set_outline_method (
        double    *x,
        double    *y,
        int       npts);

    int jni_call_set_outline3d_method (
        double    *x,
        double    *y,
        double    *z,
        int       npts);

    void jni_call_add_native_tindex_result_method (
        int         *tmeshid,
        int         *trinum,
        int         nresult);

    void jni_call_add_fault_major_minor_method (
        int         id,
        int         major);


    FILE *jni_get_prim_file_sw ();
    void jni_set_prim_file_sw (FILE *pf);
    void jni_open_prim_file_sw (char *fname);
    void jni_close_prim_file_sw ();


  #ifdef __cplusplus
  }
  #endif

#endif
