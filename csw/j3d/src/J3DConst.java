
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;


/**
 * This class holds constant values used by various 3D viewing classes.
 */
public class J3DConst
{

/*
 * These defines are used for the top level "names" in the
 * opengl name stack.  They are used as part of the
 * selected object identification process.
 *
 * The naming stack uses 5 or 6 levels to identify a selection.
 *
 *    level 1:  The class of object as defined below.
 *
 *    level 2:  The object sequence number within the class.
 *
 *    level 3:  A subobject identifier, either EDGES_NAME,
 *              TRIANGLES_NAME or NODES_NAME to specify
 *              which type of data the subsequent element
 *              sequence number represents.  If this is not
 *              pertinent, it is set to INVALID_NAME_VALUE.
 *
 *              For a draped line, level 3 is the draped line object
 *              number relative to the list of draped lines for the
 *              surface.
 *
 *    level 4:  The zero based sequence number of an object element,
 *              for example a tetrahedron in a volume or a triangle
 *              in a surface.  If this doesn't apply for a particular
 *              class of object, this value will be INVALID_NAME_VALUE.
 *
 *              For draped lines, this is the polyline within the
 *              draped line object.
 *
 *    level 5:  Identification of the type of graphics used
 *              for the component, i.e., line or color fill.
 *              Zero for line, 1 for color fill.
 *
 *    level 6:  For draped lines, the 6th level is the segment number
 *              within a draped polyline.
 */
    static public final int  SURFACE_NAME            = 10;
    static public final int  HORIZON_SURFACE_NAME    = 11;
    static public final int  FAULT_SURFACE_NAME      = 12;
    static public final int  EXTERNAL_SURFACE_NAME   = 13;
    static public final int  WELL_NAME               = 20;
    static public final int  LINE3D_NAME             = 30;
    static public final int  POINT_NAME              = 40;
    static public final int  MIG3D_NAME              = 50;
    static public final int  TRAP_NAME               = 60;
    static public final int  VOLUME_NAME             = 70;
    static public final int  AXIS_NAME               = 80;
    static public final int  DEBUG_LINE_NAME         = 90;
    static public final int  VFAULT_NAME             = 100;
    static public final int  CULTURE_NAME            = 110;
    static public final int  TEXT3D_NAME             = 120;

    static public final int  EDGES_NAME              = 1000;
    static public final int  TRIANGLES_NAME          = 2000;
    static public final int  NODES_NAME              = 3000;
    static public final int  QUAD_NAME               = 4000;

    static public final int  INVALID_NAME_VALUE      = 1989795979;

    static public final int  _TOP_TRAP_SURFACE_FLAG     = 100000;
    static public final int  _BOTTOM_TRAP_SURFACE_FLAG  = 200000;

    static public final int  _X_AXIS_NAME            = 1;
    static public final int  _Y_AXIS_NAME            = 2;
    static public final int  _Z_AXIS_NAME            = 3;

    static final int         XZERO_NAME              = 990;
    static final int         YZERO_NAME              = 991;
    static final int         ZZERO_NAME              = 992;


/*
 * These defines are for geologic sorts of classes.
 * Any object might belong to one of these classes
 * as well as the geometric class.  For instance,
 * a surface and an interpretation line might both
 * be part of defining a fault.
 */
    static public final int  UNKNOWN_DATA             = -1;
    static public final int  HORIZON_DATA             = 10;
    static public final int  UNCONFORMITY_DATA        = 20;
    static public final int  VFAULT_DATA              = 30;
    static public final int  MODEL_TOP_DATA           = 40;
    static public final int  MODEL_BOTTOM_DATA        = 50;
    static public final int  TRAP3D_DATA              = 60;
    static public final int  MIG3D_LINE_DATA          = 70;
    static public final int  WELL_DATA                = 80;

    static public final int  _WELLS_SORTED_BY_NAME_        = 1;
    static public final int  _WELLS_SORTED_BY_TYPE_        = 2;


}  // end of J3DConst class
