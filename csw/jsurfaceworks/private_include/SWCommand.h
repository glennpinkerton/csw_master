
#ifndef _SW_COMMAND_H_
#define _SW_COMMAND_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SW_END_THREAD         6723
#define SW_OPEN_LOG_FILE      9988
#define SW_PAUSE_LOG_FILE      9987
#define SW_RESTART_LOG_FILE      9986

#define SW_SET_LINES          1
#define SW_SET_BOUNDS         2
#define SW_CALC_TRI_MESH      3
#define SW_CALC_GRID          4
#define SW_GRID_OPTIONS       5
#define SW_GRID_GEOMETRY      6

#define SW_CALC_DRAPED_LINES      10
#define SW_TRIMESH_DATA           11
#define SW_SET_DRAPE_LINES        12

#define SW_GRID_TO_TRIMESH        13
#define SW_OUTLINE_POINTS         14
#define SW_POINT_IN_POLY          15

#define SW_TRIMESH_OUTLINE        16
#define SW_CALC_EXACT_TRIMESH     17

#define SW_MODEL_BOUNDS           18

#define SW_SET_DRAPE_POINTS       19
#define SW_CALC_DRAPED_POINTS     20

#define SW_CALC_CONSTANT_TRI_MESH 21

#define SW_REMOVE_TRI_INDEX       22
#define SW_REMOVE_ALL_TRI_INDEXES 23
#define SW_CALC_BLENDED_GRID      24

#define SW_START_AT_TRI_MESH      25
#define SW_SET_AT_POLYLINE        26
#define SW_ADD_AT_RDP_POINTS      27
#define SW_ADD_AT_NDP_POINTS      28
#define SW_CALC_AT_TRI_MESH       29

#define SW_TEST_TRI_SPEED         98
#define SW_CLEAR_DRAPE_CACHE      99

#define SW_PS_ADD_BORDER_SEGMENT           100
#define SW_PS_ADD_CENTERLINE               101
#define SW_PS_SET_POINTS                   102
#define SW_PS_ADD_LINE                     103
#define SW_PS_ADD_FAULT_SURFACE            104
#define SW_PS_CALC_SPLIT                   105
#define SW_PS_SET_TEST_STATE               106
#define SW_PS_CLEAR_ALL_DATA               107
#define SW_PS_LINE_TRIM_FRACTION           108
#define SW_PS_CLEAR_HORIZON_DATA           109
#define SW_PS_ADD_HORIZON_PATCH            110
#define SW_PS_START_SEALED_MODEL_DEF       111
#define SW_PS_CALC_SEALED_MODEL            112
#define SW_PS_MODEL_BOUNDS                 113
#define SW_PS_SET_SED_SURFACE              114
#define SW_PS_SET_MODEL_BOTTOM             115
#define SW_PS_GET_SEALED_INPUT             116
#define SW_PS_CALC_LINES_FOR_SPLITTING     117
#define SW_CLOSE_TO_SAME_LINE_XY           118

#define SW_PS_START_FAULT_CONNECT_GROUP    119
#define SW_PS_CONNECT_FAULTS               120
#define SW_PS_SET_DETACHMENT               121
#define SW_PS_ADD_CONNECTING_FAULT         122
#define SW_PS_ADD_TO_DETACHMENT            123
#define SW_PS_END_FAULT_CONNECT_GROUP      124
#define SW_PS_SET_LOWER_TMESH              125
#define SW_PS_SET_UPPER_TMESH              126
#define SW_PS_CALC_DETACHMENT              127

#define SW_BATCH_1D_MESHER_START_DEFINITION       140
#define SW_BATCH_1D_MESHER_SET_MODEL_BOUNDARY     141
#define SW_BATCH_1D_MESHER_ADD_FAULT_LINE         142
#define SW_BATCH_1D_MESHER_CREATE_MASTER_TRI_MESH 143
#define SW_BATCH_1D_MESHER_SET_HORIZON_POINTS     144
#define SW_BATCH_1D_MESHER_SET_HORIZON_LINES      145
#define SW_BATCH_1D_MESHER_CALC_HORIZON_TRI_MESH  146
#define SW_BATCH_1D_MESHER_CLEAR_ALL_DATA         147

#define SW_PS_START_1D_MODEL_DEF           150
#define SW_PS_CALC_1D_MODEL                151
#define SW_PS_ADD_1D_FAULT_LINE            152
#define SW_PS_ADD_1D_HORIZON               153
#define SW_PS_1D_MODEL_BOUNDS              154
#define SW_PS_CLEAR_1D_HORIZON_DATA        155
#define SW_PS_CLEAR_ALL_1D_DATA            156
#define SW_PS_ADD_1D_HORIZON_POINTS        157
#define SW_PS_ADD_1D_HORIZON_LINES         158
#define SW_PS_ADD_1D_HORIZON_GRID          159

#define SW_PS_ADD_FAULT_SURFACE_WITH_DETACHMENT_CONTACT            128

#define SW_VERT_CALC_BASELINE              200
#define SW_VERT_SET_BASELINE               201

#define SW_SET_3D_TINDEX                   300
#define SW_CREATE_3D_TINDEX                301
#define SW_SET_3D_TINDEX_GEOM              302
#define SW_ADD_3D_TINDEX_TRIMESH           303
#define SW_CLEAR_3D_TINDEX                 304
#define SW_DELETE_3D_TINDEX                305
#define SW_GET_3D_TRIANGLES                306

#define SW_CONVERT_NODE_TRIMESH            350

#define SW_EXTEND_FAULT                    400

#define SW_WRITE_TRIMESH_DATA              10000
#define SW_READ_TRIMESH_DATA               10001
#define SW_READ_CSW_GRID                   10011

#include <stdio.h>

long sw_process_command (
    int               command_id,
    int               threadid,
    int               *ilist,
    long              *llist,
    double            *dlist,
    char              *cdata,
    int               *idata,
    float             *fdata,
    double            *ddata
);


/* close extern C block */

#ifdef __cplusplus
}
#endif

#endif
