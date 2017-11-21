
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/*
    grd_shared_structs.h

    Structures and constants previously in grid_api.h have been 
    separated out into this file.  The idea is to make these 
    structures visible to "public" and "private" parts of the
    gridding code.
*/

/*
    Add nothing above this line
*/
#ifndef GRD_SHARED_STRUCTS_H
#  define GRD_SHARED_STRUCTS_H

/*
    include the csw utilities header file.
*/
#ifndef PRIVATE_HEADERS_OK
#define PRIVATE_HEADERS_OK
#endif

#  include "csw/utils/include/csw_.h"

/*
    Define grid option constants.
*/
#define GRD_DEFAULT_OPTIONS          0
#define GRD_PREFERRED_STRIKE         1
#define GRD_OUTSIDE_MARGIN           2
#define GRD_INSIDE_MARGIN            3
#define GRD_FAST_FLAG                4
#define GRD_MIN_VALUE                5
#define GRD_MAX_VALUE                6
#define GRD_REPORT_LINES             7
#define GRD_ANISOTROPY_FLAG          8
#define GRD_DISTANCE_POWER           9
#define GRD_STRIKE_POWER             10
#define GRD_NUM_LOCAL_POINTS         11
#define GRD_LOCAL_SEARCH_PATTERN     12
#define GRD_MAX_SEARCH_DISTANCE      13
#define GRD_THICKNESS_FLAG           14
#define GRD_STEP_GRID_FLAG           15
#define GRD_WORK_MARGIN              16
#define GRD_TRIANGULATE_FLAG         17
#define GRD_DEFAULT_SIZE_MULTIPLIER  18

#define GRD_FAULTED_GRID_FLAG        1001

#define GRD_TRUE                     1
#define GRD_FALSE                    0

#define GRD_QUADRANT_SEARCH          1
#define GRD_OCTANT_SEARCH            2
#define GRD_RADIAL_SEARCH            3

#define GRD_NONE                     0
#define GRD_GLOBAL_ANISOTROPY        1
#define GRD_LOCAL_ANISOTROPY         2

#define GRD_ADD                      1
#define GRD_SUBTRACT                 2
#define GRD_MULTIPLY                 3
#define GRD_DIVIDE                   4
#define GRD_RECIPROCAL               5
#define GRD_EXPONENT                 6
#define GRD_LOG                      7
#define GRD_MINIMUM                  8
#define GRD_MAXIMUM                  9
#define GRD_CONSTANT_MINUS           10
#define GRD_REPLACE_NULLS            11
#define GRD_POWER                    12
#define GRD_CALL_FUNC                999

#define GRD_ONE_GRID_ARITH           1
#define GRD_TWO_GRID_ARITH           2

#define GRD_BILINEAR                 1
#define GRD_BICUBIC                  2
#define GRD_STEP_GRID                3

#define GRD_NORMAL_GRID_FILE         1
#define GRD_THICKNESS_GRID_FILE      2
#define GRD_STEP_GRID_FILE           3
#define GRD_BLENDED_GRID_FILE        4

#define GRD_OUTSIDE_ONLY             0
#define GRD_INSIDE_AND_OUTSIDE       1

#define GRD_TAG_SIZE                 90
#define GRD_COMMENT_SIZE             200
#define GRD_MAX_MULTI_FILES          10000

#define GRD_MASK_VALID               '\0'
#define GRD_MASK_INVALID_OUTSIDE     '\1'
#define GRD_MASK_INVALID_INSIDE      '\2'
#define GRD_MASK_INVALID_DERIVED_OUTSIDE '\3'
#define GRD_MASK_INVALID_DERIVED_INSIDE '\4'
#define GRD_MASK_USER_EDIT_RESET     '\77'
#define GRD_MASK_ENABLED_BY_RECALC   '\66'

#define GRD_MASK_CROSSING_FLAG       '\10'

#define GRD_CONTROL_GREATER          1
#define GRD_CONTROL_LESS             2
#define GRD_CONTROL_BETWEEN          3

#define GRD_CONFORMABLE              1
#define GRD_BASELAP                  2
#define GRD_TRUNCATION               3

#define GRD_NAME_LENGTH             64

#define GRD_POSITIVE_UP              1
#define GRD_POSITIVE_DOWN           -1

#define GRD_UNKNOWN_FAULT           0
#define GRD_VERTICAL_FAULT          1
#define GRD_NON_VERTICAL_FAULT      2
#define GRD_BLOCK_POLYGON_FAULT     3

#define GRD_CLIP_MASK_IMAGE         1
#define GRD_INDEXED_COLOR_IMAGE     2
#define GRD_TRUE_COLOR_IMAGE        3

#define GRD_POSITIVE_THICKNESS      1
#define GRD_NEGATIVE_THICKNESS      2

#define GRD_INSIDE_POLYGON          1
#define GRD_OUTSIDE_POLYGON         2

#define GRD_DEEPER_THAN_RULE        1
#define GRD_SHALLOWER_THAN_RULE     2
#define GRD_DEEPER_THAN_DETACHMANT_RULE        3
#define GRD_SHALLOWER_THAN_DETACHMENT_RULE     4

/*
    define structures needed for the public functions
*/
    typedef struct {
        int       preferred_strike,
                  outside_margin,
                  inside_margin,
                  fast_flag,
                  report_lines;
        CSW_F     min_value,
                  max_value,
                  hard_min,
                  hard_max;
        int       anisotropy_flag,
                  distance_power,
                  strike_power,
                  num_local_points,
                  local_search_pattern;
        CSW_F     max_search_distance;
        int       thickness_flag;
        int       step_flag;
        int       faulted_flag;
        int       triangulate_flag;
        int       work_margin;
        int       error_number;
    }  GRidCalcOptions;

    typedef struct {
        int       type, firstnode, lastnode;
        CSW_F     *grid1, *grid2;
        CSW_F     x1, y1, x2, y2, x, y;
        int       ncol, nrow, column, row;
        CSW_F     nullvalue, constant;
        CSW_F     value1, value2;
        void      *client_data;
        CSW_F     value_output;
        int       abort_flag;
    }  GRidArithData;

    typedef struct {
        unsigned char      v1, p1,
                           v2, p2,
                           v3, p3,
                           v4, p4;
    }  CSW_BlendedNode;

    typedef struct {
        int                value1,
                           value2,
                           value3,
                           value4;
        CSW_Float          percent1,
                           percent2,
                           percent3,
                           percent4;
        int                nvalues;
        CSW_Float          missing_percent;
    }  CSW_Mixture;

    typedef CSW_BlendedNode CSW_Blended;

    typedef struct {
        CSW_F              x,
                           y,
                           z1,
                           z2,
                           zval;
        int                type,
                           active_flag;
    }  GRidControlPoint;

    typedef struct {
        double         x,
                       y,
                       z;
    }  POint3D;

    typedef struct {
        POint3D     *points;
        int         num_points;
        int         *comp_points;
        int         ncomp;
        int         id;
        int         type;
        int         lclass;
    }  FAultLineStruct;

    typedef struct {
        CSW_F              *grid;
        int                ncol,
                           nrow;
        double             x1,
                           y1,
                           x2,
                           y2;
        int                type,
                           id;
        FAultLineStruct    *faults;
        int                nfaults;
        char               name[GRD_NAME_LENGTH];
    }  GRidStruct;

    typedef struct {
        int            v1, v2, v3;
    }  TRiangleIndex;

    typedef struct {
        POint3D        vertices[3];
    }  TRiangle3D;

    typedef struct {
        int         p1, p2;
    }  SIdeIndex;

    typedef struct {
        int         s1, s2, s3, s4, s5, s6;
    }  HExagonIndex;

    typedef struct {
        POint3D     corners[6];
    }  HExagon3D;

    typedef struct {
        double    xmin,
                  ymin,
                  xmax,
                  ymax;
        CSW_F     *grid;
        char      *mask;
        char      *mask2;
        int       ncol,
                  nrow;
        int       gridtype,
                  error;
        char      tag[GRD_TAG_SIZE + 1];
        char      comment[GRD_COMMENT_SIZE+1];
        FAultLineStruct   *faults;
        int       nfaults;
    }  GRidFileRec;

    typedef struct {
        int       background_color,
                  thickness_flag,
                  smoothing_flag;
        CSW_F     zmin,
                  zmax;
        CSW_F     null_value;
        int       zerofillflag;
    }  GRdImageOptions;

    typedef struct {
        unsigned char   *data;
        int             ncol,
                        nrow;
        double          x1,
                        y1,
                        x2,
                        y2;
        int             image_type;
    }  GRdImage;

    typedef struct {
        unsigned char   *red;
        unsigned char   *green;
        unsigned char   *blue;
        unsigned char   *alpha;
        int             ncol,
                        nrow;
        double          x1,
                        y1,
                        x2,
                        y2;
        char            name[100];
        int             id;
    }  GRdNDPImage;



#define   Z_ABSOLUTE_TINY  (1.e-30) 

/*
    Define constants used in calling trimesh functions.
*/
#define GRD_SWAP_ANY                2
#define GRD_SWAP_NONE               3
#define GRD_SWAP_AS_FLAGGED         4
#define GRD_SWAP_FLAG               0
#define GRD_DONT_SWAP_FLAG          -99

#define GRD_CELL_DIAGONALS          1
#define GRD_EQUILATERAL             2

#define GRD_CHECK_FOR_NULL_POINTER  0.0

#define GRD_TRIMESH_FAULT_CONSTRAINT   1
#define GRD_TRIMESH_ZERO_FAULT_CONSTRAINT   2
#define GRD_TRIMESH_NV_FAULT_CONSTRAINT   1111
#define GRD_TRIMESH_ZERO_NV_FAULT_CONSTRAINT   2222
#define GRD_NV_FAULT_ID_BASE              1000000
#define GRD_TRIMESH_INSIDE_FAULT  99

#define GRD_MODEL_SIDE_CONSTRAINT      -209
#define GRD_UNDEFINED_CONSTRAINT       -219
#define GRD_DISCONTINUITY_CONSTRAINT   -319
#define GRD_ZERO_DISCONTINUITY_CONSTRAINT   -320
#define GRD_BOUNDARY_CONSTRAINT        -419
#define GRD_CONTOUR_CONSTRAINT         -519

#define GRD_SOFT_NULL_VALUE         -1.e20
#define GRD_HARD_NULL_VALUE         1.e30

#define GRD_USE_ALL_EDGES               0
#define GRD_IGNORE_LONG_EDGES           1
#define GRD_IGNORE_SHORT_EDGES          2

/*
    Define structures used to pass data to and from the
    triangulation code.
*/

typedef struct {
    int    npts,
           max,
           *list;
}  INdexStruct;

typedef struct {
    double          *x,
                    *y,
                    *z;
    int             npts,
                    number;
}  POlygonStruct;

typedef struct {
    double          *x,
                    *y;
    double          zlev;
    int             npts;
}  COntourDataStruct;

typedef struct {
    double          x,
                    y,
                    z;
    int             *polygons_using,
                    num_polygons_using,
                    max_polygons_using;
}  SPillpointStruct;

typedef double (*CLIENT_DATA_CHECK_FUNC) (void *ptr);

typedef struct {
    double          nx,
                    ny,
                    nz;
    double          zcenter;
    int             subflag;
    int             count;
}  TRiNormStruct;

typedef struct {
    double          x,
                    y,
                    z,
                    zsave;
    double          xorig,
                    yorig,
                    zorig;
    int             rp;
    int             crp;
    int             flag;
    int             spillnum;
    int             splitflag;
    int             adjusting_node;
    void            *client_data;
    char            deleted;
    char            shifted;
    char            bflag;
    char            on_border;
    char            is_locked;
    char            is_intersect;
    TRiNormStruct   *norm;
}  NOdeStruct;

typedef struct {
    int             node1,
                    node2,
                    tri1,
                    tri2,
                    flag,
                    number,
                    lineid,
                    lineid2;
    double          length;
    int             cindex;
    int             pairid;
    char            deleted;
    char            tflag;
    char            tflag2;
    char            on_border;
    char            isconstraint;
    void            *client_data;
}  EDgeStruct;

typedef struct {
    int             edge1,
                    edge2,
                    edge3,
                    flag;
    int             used_flag;
    char            deleted;
    void            *client_data;
    TRiNormStruct   *norm;
}  TRiangleStruct;

typedef struct {
    int             node1,
                    node2,
                    node3;
}  NOdeTriangleStruct;

typedef struct {
    SPillpointStruct  *spillpoint_list;
    int               total_spillpoints;
    POlygonStruct     *polygon_list;
    int               total_polygons;
    NOdeStruct        *nodes;
    int               numnodes;
    EDgeStruct        *edges;
    int               numedges;
    TRiangleStruct    *triangles;
    int               numtriangles;
    POlygonStruct     *ridgelist;
    int               numridges;
} DRainagesStruct;

typedef struct {
    NOdeStruct      *nodes;
    int numnodes;
    EDgeStruct *edges;
    int numedges;
    TRiangleStruct *triangles;
    int numtriangles;
} TRimeshStruct;

typedef struct {
    int   *list;
    int   nlist;
    int   maxlist;
    int   done;
    int   boundary;
} NOdeEdgeListStruct;

typedef struct {
    int       max_num_points;
    double    max_dist;
    double    smoothing_factor;
} SPikeFilterOptions;



/*
    Add nothing to this file below the following endif
*/
#endif
