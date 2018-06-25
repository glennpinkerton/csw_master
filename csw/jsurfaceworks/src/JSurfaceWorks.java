
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

//import java.lang.Thread;

import java.util.ArrayList;
import java.util.Date;
//import java.util.Iterator;

import javax.swing.SwingUtilities;

import org.apache.logging.log4j.Logger;

import csw.jeasyx.src.CreateNative;
import csw.jutils.src.Bounds3D;
import csw.jutils.src.ContactPolyline;
import csw.jutils.src.NodeArray3D;
import csw.jutils.src.TriangleArray;
import csw.jutils.src.CSWLogger;

import csw.jutils.src.XYPolyline;
import csw.jutils.src.XYZNode;
import csw.jutils.src.XYZPoint;
import csw.jutils.src.XYZPolyline;

/**
  This class provides an assortment of methods that can be
  used to calculate and manipulate grids and trimeshes.  The functions
  send commands via jni calls to instruct a native surface calculation
  command processor.  The actual calculation work is done in native functions.
<p>
  This class is intended to be a calculation engine only.  You should create
  a new instance as a local variable on the stack, use it as you need, and
  let it get garbage collected when the method goes out of scope.
<p>
  The effect is similar to calling static class methods.  Static methods
  are not used because of the native interface.  When data is returned from the
  native interface, it needs to be returned to the same JSurfaceWorks object
  that initiated the calculation, so I require an object to be instantiated in
  order to use the methods.

  @author Glenn Pinkerton

*/
public class JSurfaceWorks extends JSurfaceWorksBase {

    private static Logger logger = CSWLogger.getMyLogger ();

    static private final int SW_OPEN_LOG_FILE        = 9988;
    static private final int SW_PAUSE_LOG_FILE       = 9987;
    static private final int SW_RESTART_LOG_FILE     = 9986;
    static private final int SW_SET_LINES            = 1;
    static private final int SW_SET_BOUNDS           = 2;
    static private final int SW_CALC_TRI_MESH        = 3;
    static private final int SW_CALC_GRID            = 4;
    static private final int SW_GRID_OPTIONS         = 5;
    static private final int SW_GRID_GEOMETRY        = 6;
    static private final int SW_CALC_DRAPED_LINES    = 10;
    static private final int SW_TRIMESH_DATA         = 11;
    static private final int SW_SET_DRAPE_LINES      = 12;
    static private final int SW_GRID_TO_TRIMESH      = 13;
    static private final int SW_OUTLINE_POINTS       = 14;
    static private final int SW_POINT_IN_POLY        = 15;
    static private final int SW_TRIMESH_OUTLINE      = 16;
    static private final int SW_CALC_EXACT_TRI_MESH  = 17;
    static private final int SW_MODEL_BOUNDS         = 18;
    static private final int SW_SET_DRAPE_POINTS     = 19;
    static private final int SW_CALC_DRAPED_POINTS   = 20;
    static private final int SW_CALC_CONSTANT_TRI_MESH = 21;
    static private final int SW_REMOVE_TRI_INDEX     = 22;
    static private final int SW_REMOVE_ALL_TRI_INDEXES = 23;
    static private final int SW_START_AT_TRI_MESH    = 25;
    static private final int SW_SET_AT_POLYLINE      = 26;
    static private final int SW_CALC_AT_TRI_MESH     = 29;
    static private final int SW_TEST_TRI_SPEED       = 98;
    static private final int SW_CLEAR_DRAPE_CACHE    = 99;

    static private final int SW_PS_ADD_BORDER_SEGMENT         = 100;
    static private final int SW_PS_ADD_CENTERLINE             = 101;
    static private final int SW_PS_SET_POINTS                 = 102;
    static private final int SW_PS_ADD_FAULT_SURFACE          = 104;
    static private final int SW_PS_CALC_SPLIT                 = 105;
    static private final int SW_PS_SET_TEST_STATE             = 106;
    static private final int SW_PS_CLEAR_ALL_DATA             = 107;
    static private final int SW_PS_LINE_TRIM_FRACTION         = 108;
    static private final int SW_PS_CLEAR_HORIZON_DATA         = 109;
    static private final int SW_PS_ADD_HORIZON_PATCH          = 110;
    static private final int SW_PS_START_SEALED_MODEL_DEF     = 111;
    static private final int SW_PS_CALC_SEALED_MODEL          = 112;
    static private final int SW_PS_MODEL_BOUNDS               = 113;
    static private final int SW_PS_SET_SED_SURFACE            = 114;
    static private final int SW_PS_SET_MODEL_BOTTOM           = 115;
    static private final int SW_PS_GET_SEALED_INPUT           = 116;
    static private final int SW_PS_CALC_LINES_FOR_SPLITTING   = 117;
    static private final int SW_CLOSE_TO_SAME_LINE_XY         = 118;

    static private final int SW_PS_START_FAULT_CONNECT_GROUP  = 119;
    static private final int SW_PS_CONNECT_FAULTS             = 120;
    static private final int SW_PS_SET_DETACHMENT             = 121;
    static private final int SW_PS_ADD_CONNECTING_FAULT       = 122;
    static private final int SW_PS_ADD_TO_DETACHMENT          = 123;
    static private final int SW_PS_END_FAULT_CONNECT_GROUP    = 124;
    static private final int SW_PS_SET_LOWER_TMESH            = 125;
    static private final int SW_PS_SET_UPPER_TMESH            = 126;
    static private final int SW_PS_CALC_DETACHMENT            = 127;

    static private final int SW_PS_ADD_FAULT_SURFACE_WITH_DETACHMENT_CONTACT = 128;

    static private final int SW_VERT_CALC_BASELINE            = 200;
    static private final int SW_VERT_SET_BASELINE             = 201;

    static private final int SW_CREATE_3D_TINDEX              = 301;
    static private final int SW_SET_3D_TINDEX_GEOM            = 302;
    static private final int SW_ADD_3D_TINDEX_TRIMESH         = 303;
    static private final int SW_CLEAR_3D_TINDEX               = 304;
    static private final int SW_DELETE_3D_TINDEX              = 305;
    static private final int SW_GET_3D_TRIANGLES              = 306;

    static public  final int SW_GRID_LIMITS_BOUNDARY          = 1;
    static public  final int SW_CONVEX_HULL_BOUNDARY          = 2;

    static private final int SW_CONVERT_NODE_TRIMESH          = 350;

    static private final int SW_EXTEND_FAULT                  = 400;

    static private final int SW_WRITE_TRIMESH_DATA            = 10000;
    static private final int SW_READ_TRIMESH_DATA             = 10001;

    //static private final int SW_WRITE_CSW_GRID                = 10010;
    static private final int SW_READ_CSW_GRID                 = 10011;

 /*
  * These functions boil down to sending Command calls to the native
  * side.  The arrays used in the Command function are allocated here
  * for use in all the API methods.  All except the String data are
  * declared here.  Strings are created as needed by the API methods.
  *
  * The initial sizes for these arrays are compromises.  They are large
  * enough to hold the data for many calculation tasks, and they can be grown
  * if they need to be larger.  Each API function should call the
  * appropriate new method to grow the data array if needed.
  *
  * The Ilist and Dlist arrays are used for counters into data, constants, etc.
  * They never get very large, so they are set to MAX_LIST_ARRAY and not grown
  * from there.
  */
    int[]        Ilist = new int[MAX_LIST_ARRAY];
    long[]       Llist = new long[MAX_LIST_ARRAY];
    double[]     Dlist = new double[MAX_LIST_ARRAY];

    int[]        Idata = new int[HUGE_CHUNK];
    double[]     Ddata = new double[HUGE_CHUNK];

    int          IdataMax = HUGE_CHUNK;
    int          DdataMax = HUGE_CHUNK;


/**
 *  Two constructors are provided.  The "default" constructor takes
 *  no parameters.  This constructor sets the nativeID to -1, which 
 *  makes the init method create native resources.
 */
    public JSurfaceWorks () {
        nativeID = -1;
        init ();
    }


    public JSurfaceWorks (int nid) {
        nativeID = nid;
        init ();
    }


    private void init ()
    {
        if (nativeID < 0) {
            nativeID = CreateNative.createDlistResources ();
            if (nativeID < 0) {
                logger.error
                ("    JSurfaceWorks constructor " +
                 "failed to get native resources.");
            }
            else {
                logger.info ("    JSurfaceWorks constructor succeeded.    ");
            }
        }
    }


    private void newIdata (int size)
    {
        int     istat = 0;
        while (size > IdataMax) {
            IdataMax +=  HUGE_CHUNK;
            istat = 1;
        }
        if (istat == 1) {
            Idata = new int[IdataMax];
        }
    }

    private void newDdata (int size)
    {
        int     istat = 0;
        while (size > DdataMax) {
            DdataMax +=  HUGE_CHUNK;
            istat = 1;
        }
        if (istat == 1) {
            Ddata = new double[DdataMax];
        }
    }

/*-------------------------------------------------------------*/

    public void setModelBounds (
        double      xmin,
        double      ymin,
        double      zmin,
        double      xmax,
        double      ymax,
        double      zmax)
    {
        double[] data = new double[6];

        data[0] = xmin;
        data[1] = ymin;
        data[2] = zmin;
        data[3] = xmax;
        data[4] = ymax;
        data[5] = zmax;

        JSurfaceWorks jsw = new JSurfaceWorks ();

        jsw.sendNativeCommand (
            SW_MODEL_BOUNDS,
            0,
            null,
            data);

        return;

    }


/*-------------------------------------------------------------*/

    public void psSetModelBounds (
        double      xmin,
        double      ymin,
        double      zmin,
        double      xmax,
        double      ymax,
        double      zmax)
    {
        double[] data = new double[6];

        data[0] = xmin;
        data[1] = ymin;
        data[2] = zmin;
        data[3] = xmax;
        data[4] = ymax;
        data[5] = zmax;

        JSurfaceWorks jsw = new JSurfaceWorks ();

        jsw.sendNativeCommand (
            SW_PS_MODEL_BOUNDS,
            0,
            null,
            data);

        return;

    }

/*-------------------------------------------------------------*/

    public void setModelBounds (
        Bounds3D         sp)
    {
        double[] data = new double[6];

        double    xmin, ymin, zmin, xmax, ymax, zmax;

        if (sp == null) {
            xmin = 1.e30;
            ymin = 1.e30;
            zmin = 1.e30;
            xmax = -1.e30;
            ymax = -1.e30;
            zmax = -1.e30;
        }
        else {
            xmin = sp.getMinX ();
            ymin = sp.getMinY ();
            zmin = sp.getMinZ ();
            xmax = sp.getMaxX ();
            ymax = sp.getMaxY ();
            zmax = sp.getMaxZ ();
        }

        data[0] = xmin;
        data[1] = ymin;
        data[2] = zmin;
        data[3] = xmax;
        data[4] = ymax;
        data[5] = zmax;

        JSurfaceWorks jsw = new JSurfaceWorks ();

        jsw.sendNativeCommand (
            SW_MODEL_BOUNDS,
            0,
            null,
            data);

        return;

    }

/*-------------------------------------------------------------*/

    public void psSetModelBounds (
        Bounds3D         sp)
    {
        double[] data = new double[6];

        double    xmin, ymin, zmin, xmax, ymax, zmax;

        if (sp == null) {
            xmin = 1.e30;
            ymin = 1.e30;
            zmin = 1.e30;
            xmax = -1.e30;
            ymax = -1.e30;
            zmax = -1.e30;
        }
        else {
            xmin = sp.getMinX ();
            ymin = sp.getMinY ();
            zmin = sp.getMinZ ();
            xmax = sp.getMaxX ();
            ymax = sp.getMaxY ();
            zmax = sp.getMaxZ ();
        }

        data[0] = xmin;
        data[1] = ymin;
        data[2] = zmin;
        data[3] = xmax;
        data[4] = ymax;
        data[5] = zmax;

        JSurfaceWorks jsw = new JSurfaceWorks ();

        jsw.sendNativeCommand (
            SW_PS_MODEL_BOUNDS,
            0,
            null,
            data);

        return;

    }

/*-------------------------------------------------------------*/


    private int badBoundaryAction = 1;

/**
 Set the action to use when a boundary is null or otherwise invalid in
 subsequent calculation methods.  Currently, this only affects subsequent
 {@link #calcTriMesh} method calls.  If the boundary parameter to this
 method is null or if the first x coordinate in a non null boundary is
 greater than 1.e20, then the badBoundary action is performed to provide
 a boundary for the trimesh.  The possible actions are SW_GRID_LIMITS_BOUNDARY
 (1) or SW_CONVEX_HULL_BOUNDARY (2).
 If an invalid action is specified, or if
 this method is never called, the default value of SW_GRID_LIMITS_BOUNDARY
 is used.
*/
    public void setBadBoundaryAction (int ival)
    {
        if (ival < 1  ||  ival > 2) {
            ival = 1;
        }
        badBoundaryAction = ival;
    }

/**
 Return the current bad boundary action code.  The result will be either
 SW_GRID_LIMITS_BOUNDARY (1) or SW_CONVEX_HULL_BOUNDARY (2).
*/
    public int getBadBoundaryAction ()
    {
        return badBoundaryAction;
    }
/*-------------------------------------------------------------*/

/**
 Old version of calcTriMesh method.
 This version just calls the new version with a faultSurfaceFlag set to false.
 This replicates the old versions behavior.
*/
    public TriMesh calcTriMesh (
        double[] xPointsIn,
        double[] yPointsIn,
        double[] zPointsIn,
        int      numPoints,
        TriMeshConstraint[] lines,
        int      numLines,
        TriMeshConstraint boundary,
        int      gridFlag,
        GridGeometry gridGeometry,
        GridCalcOptions gridOptions)
    {
        return calcTriMesh (
            xPointsIn,
            yPointsIn,
            zPointsIn,
            numPoints,
            lines,
            numLines,
            boundary,
            gridFlag,
            gridGeometry,
            gridOptions,
            false);
    }


/*-------------------------------------------------------------*/

/**
 New version of calcTriMesh method.
 Calculate a triangular mesh from points and constraint lines.  This is the
 primary method of calculating a trimesh to use in a surface.  The trimesh
 will be continuous throughout the area defined by the boundary.  If the boundary
 is null, the convex hull of the points is used for the boundary. If the lines
 array is not null, but the numLines is less than 1, lines will be set to null.
 If numLines is greater than the length of the lines array, it is set to the
 length of the lines array.
<p>
 If you want
 to use a grid as an intermediate step in calculating the tri mesh, set the
 gridFlag parameter to one (1).  If you do not want to use a grid, set the gridFlag
 parameter to zero (0).  If the gridFlag parameter is one, then the specified
 gridOptions object will be used in calculating the grid.  If gridFlag is zero,
 the gridOptions and gridGeometry are ignored.  If gridOptions is null, it is ignored and
 default options are used.  If gridGeometry is null, it is ignored and default
 geometry is used.
<p>
 If you want to use a faulted grid to calculate the trimesh, set the gridFlag
 parameter to 2.
@return Returns a {@link TriMesh} object, or null if an error occurs.
@param xPointsIn Array of point x coordinates.
@param yPointsIn Array of point y coordinates.
@param zPointsIn Array of point z coordinates.
@param numPoints Number of points.
@param lines Optional array of {@link TriMeshConstraint} objects to be
used as internal constraints on the tri mesh.  Set to null if there are none of these.
@param numLines Number of lines in the lines array.
@param boundary Optional {@link TriMeshConstraint} object to be
used as the external boundary the tri mesh.
@param gridFlag Set to one (1) to use a grid as an intermediate setp or set to zero (0) to
not use a grid as an intermediate step.  Set to 2 to use a faulted grid.
@param gridGeometry Optional {@link GridGeometry} object for the intermediate grid.
@param gridOptions Optional {@link GridCalcOptions} object for the intermediate grid.
@param faultSurfaceFlag Set this to true if the trimesh is for a fault surface or
set to false if the trimesh is for anything else.
*/
    public TriMesh calcTriMesh (
        double[] xPointsIn,
        double[] yPointsIn,
        double[] zPointsIn,
        int      numPoints,
        TriMeshConstraint[] lines,
        int      numLines,
        TriMeshConstraint boundary,
        int      gridFlag,
        GridGeometry gridGeometry,
        GridCalcOptions gridOptions,
        boolean  faultSurfaceFlag)
    {

        int              i, j, n;

    /*
     * Check some obvious errors.
     */
        if (xPointsIn == null  ||  yPointsIn == null  ||  zPointsIn == null) {
            if (lines == null  ||  numLines < 2) {
                return null;
            }
        }
        if (numPoints < 3  &&  numLines < 2) {
            return null;
        }

        double[] xPoints;
        double[] yPoints;
        double[] zPoints;

    /*
     * Collect all points and lines and determine if "vertical"
     * surface calculations are in order.
     */
        n = numPoints;
        for (i=0; i<numLines; i++) {
            n += lines[i].numPoints;
        }

        xPoints = new double[n];
        yPoints = new double[n];
        zPoints = new double[n];

        n = 0;
        for (i=0; i<numPoints; i++) {
            xPoints[n] = xPointsIn[n];
            yPoints[n] = yPointsIn[n];
            zPoints[n] = zPointsIn[n];
            n++;
        }

        for (i=0; i<numLines; i++) {
            double[] xa = lines[i].getXArray ();
            double[] ya = lines[i].getYArray ();
            double[] za = lines[i].getZArray ();
            for (j=0; j<lines[i].numPoints; j++) {
                xPoints[n] = xa[j];
                yPoints[n] = ya[j];
                zPoints[n] = za[j];
                n++;
            }
        }

        VertBaseline vbl = vertCalcBaseline(
          xPoints,
          yPoints,
          zPoints,
          n,
          faultSurfaceFlag
        );
        vertSetBaseline(vbl);

    /*
     * Setup point arrays for trimesh calculation.
     */
        if (numPoints >= 3) {
            xPoints = xPointsIn;
            yPoints = yPointsIn;
            zPoints = zPointsIn;
        }
        else {
        /*
         * If there are less than 3 nodes and 2 or more lines,
         * put the first and last points from the lines
         * into the node array so there are enough nodes
         * for trimesh calculation.
         */
            n = numLines * 2;
            xPoints = new double[numPoints + n * 2];
            yPoints = new double[numPoints + n * 2];
            zPoints = new double[numPoints + n * 2];
            for (i=0; i<numPoints; i++) {
                xPoints[i] = xPointsIn[i];
                yPoints[i] = yPointsIn[i];
                zPoints[i] = zPointsIn[i];
            }
            n = numPoints;
            for (i=0; i<numLines; i++) {
                double[] xa = lines[i].getXArray ();
                double[] ya = lines[i].getYArray ();
                double[] za = lines[i].getZArray ();
                xPoints[n] = xa[0];
                yPoints[n] = ya[0];
                zPoints[n] = za[0];
                n++;
                j = lines[i].numPoints - 1;
                xPoints[n] = xa[j];
                yPoints[n] = ya[j];
                zPoints[n] = za[j];
                n++;
            }
        }

    /*
     * Adjust for inconsistencies.
     */
        if (lines != null &&  numLines < 1) {
            lines = null;
            numLines = 0;
        }

        if (lines == null  &&  numLines > 0) {
            numLines = 0;
        }

        if (lines != null) {
            if (numLines > lines.length) {
                numLines = lines.length;
            }
        }

        if (gridFlag != 0) {
            gridFlag = 1;
        }

    /*
     * Send the constraint line data if there is any.
     */
        if (lines != null  &&  numLines > 0) {

            int          ntot, ntmp, zflag, flag;
            long istat;
            double[]     xt, yt, zt;
            i = 0;
            j = 0;
            n = 0;

            Ilist[0] = numLines;
            newIdata (numLines * 2);
            zflag = 0;

            ntot = 0;
            for (i=0; i<numLines; i++) {
                if (lines[i] == null) {
                    continue;
                }
                ntot += lines[i].getNumPoints ();
            }

            if (ntot > 1) {
                newDdata (3 * ntot);
                n = 0;
                for (i=0; i<numLines; i++) {
                    if (lines[i] == null) {
                        Idata[i] = 0;
                        Idata[numLines+i] = 0;
                        continue;
                    }
                    xt = lines[i].getXArray ();
                    yt = lines[i].getYArray ();
                    if (xt == null  ||  yt == null) {
                        Idata[i] = 0;
                        Idata[numLines+i] = 0;
                        continue;
                    }
                    zt = lines[i].getZArray ();
                    ntmp = lines[i].getNumPoints ();
                    flag = lines[i].getFlag ();

                    if (ntmp < 2) ntmp = 0;
                    Idata[i] = ntmp;
                    Idata[i+numLines] = 0;
                    if (ntmp < 1) {
                        continue;
                    }
                    Idata[i+numLines] = flag;

                    for (j=0; j<ntmp; j++) {
                        Ddata[n] = xt[j];
                        Ddata[ntot+n] = yt[j];
                        if (zt != null) {
                            Ddata[ntot*2+n] = zt[j];
                            zflag = 1;
                        }
                        else {
                            Ddata[ntot*2+n] = 1.e30;
                        }
                        n++;
                    }
                }
                Ilist[1] = ntot;
                Ilist[2] = zflag;

                istat = sendNativeCommand (SW_SET_LINES,
                                           0,
                                           Ilist,
                                           null,
                                           null,
                                           null,
                                           Idata,
                                           null,
                                           Ddata);
                if (istat == -1) {
                    return null;
                }

            }

        } // end of sending lines section

    /*
     * Send the boundary if needed.
     */
        if (boundary != null) {

            int         ntmp, zflag;
            long istat;
            double[]    xt, yt, zt;
            i = 0;
            j = 0;
            n = 0;

            xt = boundary.getXArray ();
            yt = boundary.getYArray ();
            if (xt == null  ||  yt == null) {
            }
            else {
                zt = boundary.getZArray ();
                zflag = 0;
                ntmp = boundary.getNumPoints ();
                if (ntmp > 1) {
                    newDdata (ntmp * 3);
                    for (i=0; i<ntmp; i++) {
                        Ddata[i] = xt[i];
                        Ddata[ntmp+i] = yt[i];
                        if (zt != null) {
                            Ddata[ntmp*2+i] = zt[i];
                            zflag = 1;
                        }
                        else {
                            Ddata[ntmp*2+i] = 1.e30;
                        }
                    }

                    Ilist[0] = ntmp;
                    Ilist[1] = zflag;

                    istat = sendNativeCommand (SW_SET_BOUNDS,
                                               0,
                                               Ilist,
                                               Ddata);
                    if (istat == -1) {
                        return null;
                    }
                }

            }

        }  // end of sending boundary section

    /*
     * Send the grid options and geometry if needed.
     */
        if (gridFlag == 1) {
            sendGridCalcOptions (gridOptions);
            sendGridGeometry (gridGeometry);
        }

    /*
     * Send the calculate trimesh command.
     */
        Ilist[0] = numPoints;
        Ilist[1] = gridFlag;
        Ilist[2] = 0;
        Ilist[3] = badBoundaryAction;
        if (faultSurfaceFlag) {
            Ilist[2] = 1;
            if (verticalFaultFlag  ||  staticVfaultFlag) {
                Ilist[2] = 2;
            }
        }
        if (numPoints > 0) {
        newDdata (numPoints * 3);
            for (i=0; i<numPoints; i++) {
                Ddata[i] = xPoints[i];
                Ddata[numPoints+i] = yPoints[i];
                Ddata[numPoints*2+i] = zPoints[i];
            }
        }

        long istat = sendNativeCommand (SW_CALC_TRI_MESH,
                                       1,
                                       Ilist,
                                       Ddata);

        if (istat == -1) {
            return null;
        }

    /*
     * Build the trimesh object from the returned nodes,
     * edges and triangles.  This function is in the base
     * class, where the returned data also resides.
     */
        TriMesh tmesh = buildTriMesh ();

        tmesh.vBase = vbl;

        //tmesh.dumpLimits ();

        return tmesh;

    }

/*---------------------------------------------------------*/

 /**
 Convert a rectangular grid to a {@link TriMesh} object.  The grid data are
 taken from a {@link Grid} object.  This is used for
 converting grids to trimeshes.
 */

    public TriMesh gridToTriMesh (
        Grid grid
    )
    {
        if (grid == null) {
            return null;
        }

        double[] gdata = grid.getNodeZArray ();
        int      ncol = grid.getNCols ();
        int      nrow = grid.getNRows ();
        double   xmin = grid.getXMin ();
        double   ymin = grid.getYMin ();
        double   xmax = grid.getXMax ();
        double   ymax = grid.getYMax ();
        double   rang = grid.getRotationAngle ();

        Ilist[0] = ncol;
        Ilist[1] = nrow;
        Dlist[0] = xmin;
        Dlist[1] = ymin;
        Dlist[2] = xmax;
        Dlist[3] = ymax;
        Dlist[4] = rang;

        newDdata (ncol * nrow);
        System.arraycopy (gdata, 0, Ddata, 0, ncol * nrow);

        long istat = sendNativeCommand (SW_GRID_TO_TRIMESH,
                                       1,
                                       Ilist,
                                       null,
                                       Dlist,
                                       null,
                                       null,
                                       null,
                                       Ddata);
        if (istat == -1) {
            return null;
        }

    /*
     * Build the trimesh object from the returned nodes,
     * edges and triangles.  This function is in the base
     * class, where the returned data also resides.
     */
        TriMesh tmesh = buildTriMesh ();
        VertBaseline vbl = new VertBaseline ();
        tmesh.vBase = vbl;
        tmesh.externalGrid = true;

        //tmesh.dumpLimits ();

        return tmesh;
    }


 /**
 Calculate a rectangular grid from a collection of points and optional faults.
 You can also choose to limit the grid to a polygon area.  If the faults arraylist
 is null, no faults are used.  If the boundary
 object is null, no boundary is used.  If gridGeometry is null, a default
 geometry is determined from the input points.  If gridOptions is null, a
 default set of options is used.
 @param xPoints Array of point X coordinates
 @param yPoints Array of point Y coordinates
 @param zPoints Array of point Z coordinates
 @param numPoints Number of points (must be at least 3)
 @param faults  An {@link ArrayList}<XYZPolyline> object containing the fault lines.
 @param gridGeometry {@link GridGeometry} object defining the grid extents and resolution.
 @param gridOptions {@link GridCalcOptions} object controlling how the grid is calculated.
 */
    public CSWGrid calcGrid (
        double[] xPoints,
        double[] yPoints,
        double[] zPoints,
        int      numPoints,
        ArrayList<XYZPolyline> faults,
        TriMeshConstraint boundary,
        GridGeometry gridGeometry,
        GridCalcOptions gridOptions,
        boolean  force_no_smooth)
    {

        int grsmooth = 0;
            if (force_no_smooth == false) {
            if (numPoints > 100) grsmooth = numPoints / 5000 + 1;
            grsmooth = (int) (Math.sqrt((double)grsmooth) + .5);
            grsmooth = (int) (Math.sqrt((double)grsmooth) + .5);
        }
       
    /*
     * Check some obvious errors.
     */
        if (xPoints == null  ||  yPoints == null  ||  zPoints == null) {
            return null;
        }
        if (numPoints < 3) {
            if (numPoints == 1 && gridGeometry != null) {
              // We have a constant grid with defined grid geometry.
              // Create a CSWGrid using the grid geometry and the constant
              // value for each of the grid data nodes.
              int numRows = gridGeometry.getRows();
              int numCols = gridGeometry.getColumns();
              int size = numRows * numCols;
              double[] gridData = new double[size];
              for (int i = 0; i < size; i++) {
                gridData[i] = zPoints[0];
              }
              double xmin = gridGeometry.getXmin();
              double ymin = gridGeometry.getYmin();
              double xmax = gridGeometry.getXmax();
              double ymax = gridGeometry.getYmax();
              double width = (xmax - xmin) / numCols;
              double height = (ymax - ymin) / numRows;
              CSWGrid grid =
                  new CSWGrid(
                    gridData,
                    numCols,
                    numRows,
                    xmin,
                    ymin,
                    width,
                    height,
                    0,    // rotation angle
                    null, // Fault constraints
                    null  // mask
                  );
              return grid;
            }
            return null;
        }

    /*
     * Adjust for inconsistencies.
     */
        int numFaults = 0;
        if (faults != null)
          numFaults = faults.size();
        if (faults != null &&  numFaults < 1) {
            faults = null;
            numFaults = 0;
        }

        if (faults == null  &&  numFaults > 0) {
            numFaults = 0;
        }

    /*
     * Save the faults to be added to the CSWGrid object when the
     * calculation is finished.  This is actually done in the
     * base class.  These are protected variables.
     */
        savedFaults = faults;

    /*
     * Send the contraint line data if there is any.
     */
        if (faults != null  &&  numFaults > 0) {

            int          i, j, n, ntot, ntmp, zflag, flag;
            double[]     xt, yt, zt;

            newIdata (numFaults * 2);
            Ilist[0] = numFaults;
            zflag = 0;

            ntot = 0;
            for (i=0; i<numFaults; i++) {
                XYZPolyline fault = faults.get(i);
                if (fault == null) {
                    continue;
                }
                ntot += fault.size ();
            }

            if (ntot > 1) {
                newDdata (3 * ntot);
                n = 0;
                for (i=0; i<numFaults; i++) {
                    XYZPolyline fault = faults.get(i);
                    if (fault == null) {
                        Idata[i] = 0;
                        Idata[numFaults+i] = 0;
                        continue;
                    }
                    xt = fault.getXArray ();
                    yt = fault.getYArray ();
                    if (xt == null  ||  yt == null) {
                        Idata[i] = 0;
                        Idata[numFaults+i] = 0;
                        continue;
                    }
                    zt = fault.getZArray ();
                    ntmp = fault.size ();
                    flag = TriMeshConstraint.FAULT_CONSTRAINT;

                    if (ntmp < 2) ntmp = 0;
                    Idata[i] = ntmp;
                    Idata[i+numFaults] = 0;
                    if (ntmp < 1) {
                        continue;
                    }
                    Idata[i+numFaults] = flag;

                    for (j=0; j<ntmp; j++) {
                        Ddata[n] = xt[j];
                        Ddata[ntot+n] = yt[j];
                        if (zt != null) {
                            Ddata[ntot*2+n] = zt[j];
                            zflag = 1;
                        }
                        else {
                            Ddata[ntot*2+n] = 1.e30;
                        }
                        n++;
                    }
                }
                Ilist[1] = ntot;
                Ilist[2] = zflag;

                long istat = sendNativeCommand (SW_SET_LINES,
                                           0,
                                           Ilist,
                                           null,
                                           null,
                                           null,
                                           Idata,
                                           null,
                                           Ddata);
                if (istat == -1) {
                    System.out.println ("SW_SET_LINES failed");
                    return null;
                }

            }

        } // end of sending faults section

    /*
     * Send the boundary if needed.
     */
        if (boundary != null) {

            int         ntmp, i, zflag;
            double[]    xt, yt, zt;

            xt = boundary.getXArray ();
            yt = boundary.getYArray ();
            if (xt == null  ||  yt == null) {
            }
            else {
                zt = boundary.getZArray ();
                zflag = 0;
                ntmp = boundary.getNumPoints ();
                if (ntmp > 1) {
                    newDdata (ntmp * 3);
                    for (i=0; i<ntmp; i++) {
                        Ddata[i] = xt[i];
                        Ddata[ntmp+i] = yt[i];
                        if (zt != null) {
                            Ddata[ntmp*2+i] = zt[i];
                            zflag = 1;
                        }
                        else {
                            Ddata[ntmp*2+i] = 1.e30;
                        }
                    }

                    Ilist[0] = ntmp;
                    Ilist[1] = zflag;

                    long istat = sendNativeCommand (SW_SET_BOUNDS,
                                               0,
                                               Ilist,
                                               Ddata);
                    if (istat == -1) {
                        System.out.println ("SW_SET_BOUNDS failed");
                        return null;
                    }
                }

            }

        }  // end of sending boundary section

    /*
     * Send the grid options and geometry.
     */
        sendGridCalcOptions (gridOptions);
        sendGridGeometry (gridGeometry);

    /*
     * Send the calculate grid command.
     */
        Ilist[0] = numPoints;
        Ilist[1] = grsmooth;
        newDdata (numPoints * 3);
        for (int i=0; i<numPoints; i++) {
            Ddata[i] = xPoints[i];
            Ddata[numPoints+i] = yPoints[i];
            Ddata[numPoints*2+i] = zPoints[i];
        }

        long istat = sendNativeCommand (SW_CALC_GRID,
                                       1,
                                       Ilist,
                                       Ddata);
        if (istat == -1) {
            System.out.println("SW_CALC_GRID failed");
            return null;
        }

    /*
     * Build the CSWGrid object from the returned nodes,
     * mask and error arrays.
     */
        CSWGrid cgrid = buildCSWGrid ();

        return cgrid;

    }

/*--------------------------------------------------------------------*/

    /*
     * Send the grid calculation options.  Used by grid or trimesh calc.
     */
    private void sendGridCalcOptions (GridCalcOptions op)
    {
        if (op == null) {
            return;
        }

        Ilist[0] = op.preferredStrikeAngle;
        Ilist[1] = op.outsideMargin;
        Ilist[2] = op.insideMargin;
        Ilist[3] = op.fastFlag;
        Ilist[4] = op.anisotropyFlag;
        Ilist[5] = op.distancePower;
        Ilist[6] = op.strikePower;
        Ilist[7] = op.numLocalPoints;
        Ilist[8] = op.localSearchPattern;
        Ilist[9] = op.directionalFlag;
        Ilist[10] = 0;
        if (op.thicknessFlag) Ilist[10] = 1;
        Ilist[11] = 0;
        if (op.stepFlag) Ilist[11] = 1;
        Ilist[12] = 0;
        Ilist[13] = op.noisyDataFlag;

        Ddata[0] = op.minValue;
        Ddata[1] = op.maxValue;
        Ddata[2] = op.hardMin;
        Ddata[3] = op.hardMax;
        Ddata[4] = op.maxSearchDistance;
        Ddata[5] = 0.0;
        if (op.logFlag) Ddata[5] = 10.0;

        sendNativeCommand (SW_GRID_OPTIONS,
                           0,
                           Ilist,
                           Ddata);
        return;
    }

/*--------------------------------------------------------------------*/

    private void sendGridGeometry (GridGeometry gp)
    {
        if (gp == null) {
            return;
        }

        Ilist[0] = gp.nCols;
        Ilist[1] = gp.nRows;
        Ddata[0] = gp.xMin;
        Ddata[1] = gp.yMin;
        Ddata[2] = gp.xMax;
        Ddata[3] = gp.yMax;

        sendNativeCommand (SW_GRID_GEOMETRY,
                           0,
                           Ilist,
                           Ddata);

        return;

    }


/*---------------------------------------------------------*/

    private long          lastDrapedTriMesh = 0;

/*---------------------------------------------------------*/

    public void setLastDrapedTriMesh (long tmid)
    {
        lastDrapedTriMesh = tmid;
    }

/*---------------------------------------------------------*/

  /*
   * Remove the cached triangle indexes associated with a particular
   * {@link TriMesh} object.  The cached data is part of the native
   * C implementation of line and point draping.  As long as you want
   * to drape onto a trimesh, you should NOT remove the index for the
   * trimesh.  This method should be called, for example, when the
   * patch changes its trimesh and the old trimesh is to be garbage
   * collected.  In this case, you can call this method to free up
   * memory on the native side.  If you call this too often, the draping
   * will run much slower, as each time you drape a line onto the trimesh,
   * new index grids need to be generated.
   *
   * This has been changed to have package scope.  It is now only called
   * from the trimesh cleanupNativeresources method.  This is because of
   * multithreading issues when the native resources are altered from the
   * finalize method of the trimesh.
   */
    static int removeTriangleIndex (TriMesh tmesh)
    {
        int[]    ilist;
        double[] ddata;

        ilist = new int[2];
        ddata = new double[2];

        long id = tmesh.getUniqueID();
        int  id1, id2;
        long left, right;
        left = id >>> 32;
        right = id << 32;
        right = right >>> 32;
        id1 = (int)left;
        id2 = (int)right;

        ilist[0] = id1;
        ilist[1] = id2;

        JSurfaceWorks jsw = new JSurfaceWorks ();

        long istat =
        jsw.sendNativeCommand (SW_REMOVE_TRI_INDEX,
                           0,
                           ilist,
                           ddata);

        return (int)istat;
    }


/*---------------------------------------------------------*/

    static private class FinalizedTmeshID {
        public int id1, id2;
        public int nativeID;
    }

    static ArrayList<FinalizedTmeshID>    finalizedTriMeshList = 
        new ArrayList<FinalizedTmeshID> (10);
    static boolean      cleanupPending = false;

  /**
   * Add a trimesh to the list that needs to have their native resources
   * cleaned up.  This is called from the garbage collection thread.  It
   * needs to complete before that thread is switched out.  Therefore,
   * this method is declared as synchronized.
   */
    static public synchronized void addFinalizedTriMesh
        (TriMesh tmesh)
    {
        FinalizedTmeshID tmid = new FinalizedTmeshID ();

        long id = tmesh.getUniqueID();
        int  id1, id2;
        long left, right;
        left = id >>> 32;
        right = id << 32;
        right = right >>> 32;
        id1 = (int)left;
        id2 = (int)right;

        tmid.id1 = id1;
        tmid.id2 = id2;
        tmid.nativeID = tmesh.getNativeID ();

        finalizedTriMeshList.add (tmid);

  /*
   * Schedule the actual cleanup to happen from the event dispatch
   * thread after the current events have been processed.
   */
        if (cleanupPending) {
            return;
        }

        Runnable cleanup = new Runnable ()
        {
            public void run ()
            {
                JSurfaceWorks.cleanupFinalizedTriMeshList ();
            }
        };

        SwingUtilities.invokeLater (cleanup);

        cleanupPending = true;
    }



  /*
   * This method calls native code to free all the memory associated with
   * the finalized trimesh objects currently on the list.  This must complete
   * without thread switching, so it is declared synchronized.
   */
    static private synchronized void cleanupFinalizedTriMeshList ()
    {
        int[]    ilist;
        double[] ddata;

        ilist = new int[2];
        ddata = new double[2];

        int size = finalizedTriMeshList.size ();

        int nid = 0;

        for (int i=0; i<size; i++) {

            FinalizedTmeshID tm = finalizedTriMeshList.get (i);
            if (tm == null) {
                continue;
            }

            ilist[0] = tm.id1;
            ilist[1] = tm.id2;

            JSurfaceWorks.sendStaticNativeCommand (SW_REMOVE_TRI_INDEX,
                           nid,
                           0,
                           ilist,
                           ddata);
        }

        cleanupPending = false;
    }


/*---------------------------------------------------------*/

  /**
   * Free all indexes for all trimeshs currently cached in the native C
   * trimesh draping code.  This should be called when you are completely
   * done draping lines or points on the model and you won't be doing any
   * more draping using the cached data for a long time.  For example, if
   * the user switches to a new strat scennario, a new project, etc. then
   * calling this method before the switch can free up significant amounts
   * of memory.
   */
    static public void removeAllTriangleIndexes ()
    {
        int[]    ilist;
        double[] ddata;

        ilist = new int[2];
        ddata = new double[2];

        JSurfaceWorks jsw = new JSurfaceWorks ();

        jsw.sendNativeCommand (SW_REMOVE_ALL_TRI_INDEXES,
                           0,
                           ilist,
                           ddata);
        return;
    }


/*---------------------------------------------------------*/

  /**
  Calculate draped lines on the specified trimesh.  If the trimesh is
  the same as the previous call, the cached trimesh in the native side
  is used.  To clean up the cached data on the native side, specify
  a null tmesh object.
  @param tmesh A {@link TriMesh} object to drape onto.
  @param inputLines A {@link ArrayList}<XYZPolyline> object with lines to drape
   onto the trimesh.
  */
    public ArrayList<XYZPolyline> calcDrapedLines (TriMesh tmesh,
                                ArrayList<XYZPolyline> inputLines)
    {

        int          i, j, n, ntot;
        int          ntmp, zflag;
        double[]     xt, yt, zt;
        XYZPolyline    la;

        if (tmesh == null) {
            sendNativeCommand (
                SW_CLEAR_DRAPE_CACHE,
                0,
                Ilist);

            lastDrapedTriMesh = 0;
            return null;
        }

        if (inputLines == null  ||
            tmesh == null) {
            return null;
        }

        if (tmesh.getNumNodes () < 3  ||
            tmesh.getNumEdges () < 3  ||
            tmesh.getNumTriangles () < 1) {
            return null;
        }

      /*
       * Send the input line data to the native side.
       */
        int numLines = inputLines.size ();

        newIdata (numLines);
        zflag = 0;

        ntot = 0;
        for (i=0; i<numLines; i++) {
            la = inputLines.get (i);
            ntot += la.size ();
        }

        if (ntot < 1) {
            return null;
        }

        VertBaseline vbl = tmesh.getVertBaseline ();
        vertSetBaseline (vbl);

        newDdata (2 * ntot);
        n = 0;
        for (i=0; i<numLines; i++) {
            la = inputLines.get (i);
            xt = la.getXArray ();
            yt = la.getYArray ();
            zt = la.getZArray ();
            ntmp = la.size ();

            if (ntmp < 2) ntmp = 0;
            Idata[i] = ntmp;

            for (j=0; j<ntmp; j++) {
                Ddata[n] = xt[j];
                Ddata[ntot+n] = yt[j];
                Ddata[2*ntot+n] = zt[j];
                n++;
            }
        }
        Ilist[0] = numLines;
        Ilist[1] = ntot;
        Ilist[2] = zflag;

        long istat = sendNativeCommand (SW_SET_DRAPE_LINES,
                                   0,
                                   Ilist,
                                   null,
                                   null,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);
        if (istat == -1) {
            return null;
        }

      /*
       * Send the trimesh data to the native side if it is different from
       * the last trimesh sent.
       */

        if (tmesh.getUniqueID() != lastDrapedTriMesh) {

            Ilist[0] = tmesh.getNumNodes ();
            Ilist[1] = tmesh.getNumEdges ();
            Ilist[2] = tmesh.getNumTriangles ();

            long id = tmesh.getUniqueID();
            int  id1, id2;
            long left, right;
            left = id >>> 32;
            right = id << 32;
            right = right >>> 32;
            id1 = (int)left;
            id2 = (int)right;
            Ilist[3] = id1;
            Ilist[4] = id2;

            newDdata (3 * Ilist[0]);
            newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

            double[] xnode = tmesh.getNodeXArray ();
            double[] ynode = tmesh.getNodeYArray ();
            double[] znode = tmesh.getNodeZArray ();

            ntot = Ilist[0];
            for (i=0; i<ntot; i++) {
                Ddata[i] = xnode[i];
                Ddata[i+ntot] = ynode[i];
                Ddata[i+2*ntot] = znode[i];
            }

            int n0;
            n0 = 0;

            int[] n1 = tmesh.getEdgeNode0Array ();
            int[] n2 = tmesh.getEdgeNode1Array ();
            int[] t1 = tmesh.getEdgeTriangle0Array ();
            int[] t2 = tmesh.getEdgeTriangle1Array ();

            ntot = Ilist[1];
            for (i=0; i<ntot; i++) {
                Idata[n0+i] = n1[i];
                Idata[n0+ntot+i] = n2[i];
                Idata[n0+2*ntot+i] = t1[i];
                Idata[n0+3*ntot+i] = t2[i];
            }

            n0 += 4 * ntot;
            int[] e1 = tmesh.getTriangleEdge0Array ();
            int[] e2 = tmesh.getTriangleEdge1Array ();
            int[] e3 = tmesh.getTriangleEdge2Array ();

            ntot = Ilist[2];
            for (i=0; i<ntot; i++) {
                Idata[n0+i] = e1[i];
                Idata[n0+i+ntot] = e2[i];
                Idata[n0+i+2*ntot] = e3[i];
            }

            istat = sendNativeCommand (SW_TRIMESH_DATA,
                                       0,
                                       Ilist,
                                       null,
                                       null,
                                       null,
                                       Idata,
                                       null,
                                       Ddata);

            if (istat == -1) {
                return null;
            }

            lastDrapedTriMesh = tmesh.getUniqueID();

        }  // end of trimesh data block

      /*
       * Use the trimesh and lines cached on the native side to
       * calculate the draped lines.  When this returns, the
       * individual draped lines are available in the base classes
       * data.
       */
        drapedLineList = null;
        sendNativeCommand (
            SW_CALC_DRAPED_LINES,
            1,
            Ilist);

        ArrayList<XYZPolyline> list = buildDrapedLineList ();

        return list;
    }



/*-------------------------------------------------------------*/

    private boolean verticalFaultFlag = false;

/**
 Set a flag which tells the calculation whether subsequent fault surfaces
 are vertical or not.  If this is set to true, then when a true faultSurfaceFlag
 is specified for any of the calcTriMesh methods, the trimesh to be calculated
 will be considered a vertical fault.
*/
    public void setVerticalFaultFlag (boolean bval)
    {
        verticalFaultFlag = bval;
    }

/**
 Set a static flag which tells the calculation whether subsequent fault surfaces
 are vertical or not.  If this is set to true, then when a true faultSurfaceFlag
 is specified for any of the calcTriMesh methods, the trimesh to be calculated
 will be considered a vertical fault.
 <p>This is only called from the bview import classes.
*/
    private static boolean staticVfaultFlag = false;
    public static void setStaticVfaultFlag (boolean bval)
    {
        staticVfaultFlag = bval;
    }

    public static boolean getStaticVfaultFlag ()
    {
        return staticVfaultFlag;
    }

/*-------------------------------------------------------------*/

/**
 Calculate a triangular mesh from points and constraint lines.  This is the
 primary method of calculating a trimesh to use in a surface.  The trimesh
 will be continuous throughout the area defined by the boundary.  If the boundary
 is null, the convex hull of the points is used for the boundary.
<p>
 If you want
 to use a grid as an intermediate step in calculating the tri mesh, set the
 gridFlag parameter to one (1).  If you do not want to use a grid, set the gridFlag
 parameter to zero (0).  If the gridFlag parameter is one, then the specified
 gridOptions object will be used in calculating the grid.  If gridFlag is zero,
 the gridOptions and gridGeometry are ignored.  If gridOptions is null, it is ignored and
 default options are used.  If gridGeometry is null, it is ignored and default
 geometry is used.
@return Returns a {@link TriMesh} object, or null if an error occurs.
@param nodes A {@link NodeArray3D} object with the input points.
@param lineList An optional {@link ArrayListList}<XYZPolyline> object with the constraint lines.
@param boundary  An optional {@link XYZPolyline} object with a single line defining the boundary.
used as the external boundary the tri mesh.
@param gridFlag Set to one (1) to use a grid as an intermediate setp or set to zero (0) to
not use a grid as an intermediate step.
@param gridGeometry Optional {@link GridGeometry} object for the intermediate grid.
@param gridOptions Optional {@link GridCalcOptions} object for the intermediate grid.
@param faultSurfaceFlag Set to true if the trimesh will become part of a fault surface
or set to false if the trimesh will be used for anything else.
*/
    public TriMesh calcTriMesh (
        ArrayList<XYZNode>        nodes,
        ArrayList<XYZPolyline>    lineList,
        XYZPolyline        boundary,
        int                gridFlag,
        GridGeometry       gridGeometry,
        GridCalcOptions    gridOptions,
        boolean            faultSurfaceFlag)
    {
        int          i, j, n, nl;

    /*
     * Check some obvious errors.
     */
        if (nodes == null) {
            if (lineList == null) {
                return null;
            }
            if (lineList.size () < 2) {
                return null;
            }
            n = 0;
        }
        else {
            n = nodes.size ();
        }

        if (n < 3) {
            if (lineList == null) {
                return null;
            }
            if (lineList.size () < 2) {
                return null;
            }
        }

    /*
     * Check if the surface is steep enough to require
     * special "steep surface" handling.
     */
        VertBaseline vbl =
        vertCalcBaseline (
            nodes,
            lineList,
            faultSurfaceFlag);
        vertSetBaseline (vbl);

    /*
     * If there are less than 3 nodes and 2 or more lines.
     * Put the first and last points from the lines
     * into the node array so there are enough nodes
     * for trimesh calculation.
     */
        if (lineList == null)
          nl = 0;
        else
          nl = lineList.size();
        if (n < 3) {
            ArrayList<XYZNode> na = new ArrayList<XYZNode> (n + 2 * nl);
            if (n > 0  &&  nodes != null) {
                //XYZNode  xyzn2;
                for (i=0; i<n; i++) {
                    //xyzn1 = na.get(i);
                    //xyzn2 = nodes.get(i);
                    //xyzn1 = xyzn2;
                }
            }
            XYZNode  xyzn;
            for (i=0; i<nl; i++) {
                XYZPolyline lt = lineList.get (i);
                double[] xt = lt.getXArray();
                double[] yt = lt.getYArray();
                double[] zt = lt.getZArray();
                xyzn = na.get(n);
                xyzn.setX (xt[0]);
                xyzn.setY (yt[0]);
                xyzn.setZ (zt[0]);
                n++;
                j = xt.length - 1;
                xyzn = na.get(n);
                xyzn.setX (xt[j]);
                xyzn.setY (yt[j]);
                xyzn.setZ (zt[j]);
                n++;
            }
            nodes = na;
        }

    /*
     * For now, grid flag is zero or 1.
     */
        if (gridFlag != 0) {
            gridFlag = 1;
        }

    /*
     * Send the contraint line data if there is any.
     */
        if (lineList != null) {

            int          ntot, ntmp, zflag;
            double[]     xt, yt, zt;

            int numLines = lineList.size ();

            Ilist[0] = numLines;
            newIdata (numLines * 2);
            zflag = 1;

            XYZPolyline la;
            ntot = 0;
            for (i=0; i<numLines; i++) {
                la = lineList.get (i);
                if (la == null) {
                    continue;
                }
                ntot += la.size ();
            }

            if (ntot > 1) {
                newDdata (3 * ntot);
                n = 0;
                for (i=0; i<numLines; i++) {
                    la = lineList.get (i);
                    if (la == null) {
                        Idata[i] = 0;
                        Idata[numLines+i] = 0;
                        continue;
                    }
                    xt = la.getXArray ();
                    yt = la.getYArray ();
                    zt = la.getZArray ();
                    ntmp = la.size ();

                    if (ntmp < 2) ntmp = 0;
                    Idata[i] = ntmp;
                    Idata[i+numLines] = 0;
                    if (ntmp < 1) {
                        continue;
                    }
                    Idata[i+numLines] = 0;

                    for (j=0; j<ntmp; j++) {
                        Ddata[n] = xt[j];
                        Ddata[ntot+n] = yt[j];
                        Ddata[ntot*2+n] = zt[j];
                        n++;
                    }
                }
                Ilist[1] = ntot;
                Ilist[2] = zflag;

                long istat = sendNativeCommand (SW_SET_LINES,
                                           0,
                                           Ilist,
                                           null,
                                           null,
                                           null,
                                           Idata,
                                           null,
                                           Ddata);
                if (istat == -1) {
                    return null;
                }

            }

        } // end of sending lines section

    /*
     * Send the boundary if needed.
     */
        if (boundary != null) {

            int         ntmp, zflag;
            double[]    xt, yt, zt;

            xt = boundary.getXArray ();
            yt = boundary.getYArray ();
            if (xt == null  ||  yt == null) {
            }
            else {
                zt = boundary.getZArray ();
                zflag = 1;
                ntmp = boundary.size ();
                if (ntmp > 1) {
                    newDdata (ntmp * 3);
                    for (i=0; i<ntmp; i++) {
                        Ddata[i] = xt[i];
                        Ddata[ntmp+i] = yt[i];
                        Ddata[ntmp*2+i] = zt[i];
                    }

                    Ilist[0] = ntmp;
                    Ilist[1] = zflag;

                    long istat = sendNativeCommand (SW_SET_BOUNDS,
                                               0,
                                               Ilist,
                                               Ddata);
                    if (istat == -1) {
                        return null;
                    }
                }

            }

        }  // end of sending boundary section

    /*
     * Send the grid options and geometry if needed.
     */
        if (gridFlag == 1) {
            sendGridCalcOptions (gridOptions);
            sendGridGeometry (gridGeometry);
        }

    /*
     * Send the calculate trimesh command.
     */
        int numPoints = 0;
        if (nodes != null) {
            numPoints = nodes.size ();
        }
        Ilist[0] = numPoints;
        Ilist[1] = gridFlag;
        Ilist[2] = 0;
        if (faultSurfaceFlag) {
            Ilist[2] = 1;
            if (verticalFaultFlag  ||  staticVfaultFlag) {
                Ilist[2] = 2;
            }
        }
        Ilist[3] = badBoundaryAction;
        if (numPoints > 0) {
            XYZNode  xyzn;
            for (i=0; i<numPoints; i++) {
                xyzn = nodes.get(i);
                Ddata[i] = xyzn.getX();
                Ddata[numPoints+i] = xyzn.getY();
                Ddata[numPoints*2+i] = xyzn.getZ();
            }
        }

        long istat = sendNativeCommand (SW_CALC_TRI_MESH,
                                       1,
                                       Ilist,
                                       Ddata);
        if (istat == -1) {
            return null;
        }

    /*
     * Build the trimesh object from the returned nodes,
     * edges and triangles.  This function is in the base
     * class, where the returned data also resides.
     */
        TriMesh tmesh = buildTriMesh ();

        tmesh.vBase = vbl;

        //tmesh.dumpLimits ();

        return tmesh;

    }


/*-------------------------------------------------------------*/

/*
 * Group of methods used for splitting and sealing models.
 */

/*-----------------------------------------------------------------------*/

  /**
  Start a sealed model definition.  This method will clear all
  previous fault and horizon data input to a sealed model object.
  A new native SealedModel object is created and used.  If this is
  not called prior to any of the psAdd methods, the psAdd methods
  will fail.
  @param marginFraction The fraction of the x and y extents to pad the
  model to as part of sealing.
  */
    public void psStartSealedModelDefinition (double marginFraction)
    {
        Ilist[0] = (int)(marginFraction * 1000.0 + .5);
        sendNativeCommand (SW_PS_START_SEALED_MODEL_DEF,
                           0,
                           Ilist,
                           null);
    }

/*-----------------------------------------------------------------------*/

  /**
  Convert the faults and horizons added since the {@link #psStartSealedModelDefinition}
  call into sealed faults and horizons.
  */
    public void psCalcSealedModel (double avSpace)
    {
        Dlist[0] = avSpace;

        sendNativeCommand (SW_PS_CALC_SEALED_MODEL,
                           1,
                           Ilist,
                           null,
                           Dlist,
                           null,
                           null,
                           null,
                           null);
    }

/*-----------------------------------------------------------------------*/

  /**
  Return the input horizons from the native sealed model object.  These input
  horizons will be a combination of horizons that needed splitting and horizons
  that don't need splitting.
  */
    public void psGetSealedModelInputHorizons ()
    {
        sendNativeCommand (SW_PS_GET_SEALED_INPUT,
                           1,
                           Ilist,
                           null);
    }

/*-----------------------------------------------------------------------*/


  /**
  Set a flag indicating that only a partial set of data is expected for the
  purpose of testing and debugging part of the splitting algorithm.  When the debug
  state is set to 1, intermediate results are returned to the java side.  If set to
  zero, all data must be properly setup and no intermediate results are returned.
  */
    public void psSetDebugState (int ival)
    {
        Ilist[0] = ival;
        sendNativeCommand (SW_PS_SET_TEST_STATE,
                           0,
                           Ilist,
                           null);
    }

/*-----------------------------------------------------------------------*/

  /**
  Set the fraction for which smaller lines are trimmed in the patch splitting
  process.  If an original fault centerline ends up being split up due to intersections
  with other center lines, there will probably be short lengths of lines left
  over at the ends of the original centerlines.  By default, if these short lengths
  are less than .05 of the total original line length, they are discarded in the
  surface splitting process.  If you want to change from the .05 value, call this method.
 <p>
  This value is essentially a static value, since it is just handed off to the
  native code that does the work.  The value will be preserved in the native code
  until it is changed, even if the JSurfaceWorks java object you are currently
  using gets garbage collected.
  */
    public void psSetLineTrimFraction (double val)
    {
        Ddata[0] = val;
        sendNativeCommand (SW_PS_LINE_TRIM_FRACTION,
                           1,
                           null,
                           Ddata);
    }

/*-----------------------------------------------------------------------*/

  /**
  Calculate intersections between the currently defined faults and time surfaces
  and return them as starting point lines for splitting horizons.  This is only called
  from the {@link mbuilder.controller.SplitAndSeal} class.  Don't mess with this
  yourself.
  */
    public int psCalcLinesForSplitting ()
    {

        long istat =
        sendNativeCommand (SW_PS_CALC_LINES_FOR_SPLITTING,
                           1,
                           Ilist,
                           null,
                           Dlist,
                           null,
                           null,
                           null,
                           null);

        return (int)istat;

    }

/*-----------------------------------------------------------------------*/

  /**
  Determine if the two specified lines are essentially the same line.
  This is done by finding the distance of each point of line 2 from the
  line specified in line 1.  Only points that project perpendicularly
  onto line 1 segments are used.  If the average distance is greater
  than dcrit, then the method returns zero, meaning the lines should
  be treated as distinct and separate.  If the average distance is
  less than or equal to dcrit, the lines are close to the same and they
  may be treated as the same line for some applications.

  This is currently used to decide whether a fault surface intersection
  should be used to split a surface or whether a user supplied fault centerline
  is essentially in the same location as the intersection and should be
  used instead of the intersection.

  @param xline1 X coordinates for line 1.
  @param yline1 Y coordinates for line 1.
  @param nline1 Number of points in line 1.
  @param xline2 X coordinates for line 2.
  @param yline2 Y coordinates for line 2.
  @param nline2 Number of points in line 2.
  @param dcrit  Critical distance for line closeness.

  @return Zero if the lines are separate and distinct or 1 if
  the lines are essentially the same line.

  */

    public int closeToSameLineXY (
        double[]    xline1,
        double[]    yline1,
        int         nline1,
        double[]    xline2,
        double[]    yline2,
        int         nline2,
        double      dcrit)
    {

        newDdata (nline1*2 + nline2*2);

        int         i, n0;

        for (i=0; i<nline1; i++) {
            Ddata[i] = xline1[i];
            Ddata[nline1+i] = yline1[i];
        }

        n0 = nline1 * 2;
        for (i=0; i<nline2; i++) {
            Ddata[n0+i] = xline2[i];
            Ddata[n0+nline2+i] = yline2[i];
        }

        Ilist[0] = nline1;
        Ilist[1] = nline2;
        Dlist[0] = dcrit;

        long istat =
            sendNativeCommand (
                SW_CLOSE_TO_SAME_LINE_XY,
                0,
                Ilist,
                Dlist,
                Ddata);

        return (int)istat;

    }




/*-------------------------------------------------------------------------*/

  /**
  Add a fault surface to the model sealing input.  This is passed directly to the
  native SealedModel object.  In this version, the contact line between the fault
  and its detachment is specified.  The sealing of the fault will maintain this
  contact line exactly and thie line will also define the lowest (in elevation)
  edge of the sealed fault.  The contact line must be in program units.  The
  best thing to do is to make a copy of the contactline from the data base and
  convert that copy to program units.
  This surface stays in effect until a call to {@link #psClearAllData} is made.
  @param tmesh A {@link TriMesh} object that defines the fault surface.
  @param faultID An integer id tagged onto the sealed version eventually
  output by the model sealing.
  @param flag An optional flag that isn't really used for anything yet.
  @param cpoly A {@link ContactPolyline} object defining the exact seal
  between the fault and its detachment.
  @param detach_id A unique integer id for the detachment surface connected
  to this fault.
  */
    public int psAddFaultSurface (
        TriMesh    tmesh,
        int        faultID,
        int        flag,
        double     minage,
        double     maxage,
        ContactPolyline  cpoly,
        int        detach_id)
    {
        int        i;
        int        nline;
        double[]   xline;
        double[]   yline;
        double[]   zline;

        xline = cpoly.getPolyline().getXArray ();
        yline = cpoly.getPolyline().getYArray ();
        zline = cpoly.getPolyline().getZArray ();
        nline = xline.length;

        VertBaseline vb = tmesh.getVertBaseline ();

        if (vb == null) {
            vb = vertCalcBaseline (
                tmesh.getNodeXArray (),
                tmesh.getNodeYArray (),
                tmesh.getNodeZArray (),
                tmesh.getNumNodes (),
                true);
        }
        tmesh.vBase = vb;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[4] = faultID;
        Ilist[5] = flag;
        Ilist[6] = nline;
        Ilist[7] = detach_id;
        Dlist[10] = 1.e30; // no shape z value

        if (vb == null) {
            Ilist[3] = -1;
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
        }
        else {
            if (vb.used) {
                Ilist[3] = 1;
            }
            Ilist[4] = faultID;
            Dlist[0] = vb.c1;
            Dlist[1] = vb.c2;
            Dlist[2] = vb.c3;
            Dlist[3] = vb.x0;
            Dlist[4] = vb.y0;
            Dlist[5] = vb.z0;
        }

        if (tmesh.vBase != null) {
            Ilist[3] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[3] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        Dlist[6] = minage;
        Dlist[7] = maxage;

        newDdata (3 * Ilist[0] + 3 * nline);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 3 * ntot;

        for (i=0; i<nline; i++) {
            Ddata[n0+i] = xline[i];
            Ddata[n0+nline+i] = yline[i];
            Ddata[n0+2*nline+i] = zline[i];
        }

        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat = sendNativeCommand (
            SW_PS_ADD_FAULT_SURFACE_WITH_DETACHMENT_CONTACT,
            0,
            Ilist,
            null,
            Dlist,
            null,
            Idata,
            null,
            Ddata);

        return (int)istat;
    }


/*-----------------------------------------------------------------------*/

  /**
  Split the patch that was set up with {@link #psSetPoints},
  {@link #psAddBorderSegment}, {@link #psAddFaultCenterline} etc.  The split
  patches are added to the sealed model input as horizon surface patches.
  If you
  set the clearData flag to true, the native side horizon data is all freed when
  the calculation is finished.  The avlen parameter controls the triangle size in
  the split patches.  If avlen is set to less than or equal to zero, the triangle
  size is determined automatically.
  @param clearData Set to true to free the native side horizon data used to split
  the surface.
  @param id All of the surfaces produced from the split will have this id attached
  to them.  The sealed versions of all these surfaces will also return this id.
  @param age The age, in millions of years, to use for all patches produced from
  this split.
  @param avlen The average triangle edge length for the output patches.

  */
    public void psCalcSplit (boolean clearData, int id, double age, double avlen)
    {
        Ilist[0] = 0;
        if (clearData) {
            Ilist[0] = 1;
        }
        Ilist[1] = id;
        Dlist[0] = age;
        Dlist[1] = avlen;
        sendNativeCommand (SW_PS_CALC_SPLIT,
                           1,
                           Ilist,
                           null,
                           Dlist,
                           null,
                           null,
                           null,
                           null);

    }

/*-----------------------------------------------------------------------*/

  /**
  Split the patch that was set up with {@link #psSetPoints},
  {@link #psAddBorderSegment}, {@link #psAddFaultCenterline} etc.  The split
  patches are added to the sealed model input as horizon surface patches.
  If you
  set the clearData flag to true, the native side horizon data is all freed when
  the calculation is finished.
  @param clearData Set to true to free the native side horizon data used to split
  the surface.
  @param id All of the surfaces produced from the split will have this id attached
  to them.  The sealed versions of all these surfaces will also return this id.
  @param age The age, in millions of years, to use for all patches produced from
  this split.

  */
    public void psCalcSplit (boolean clearData, int id, double age)
    {
        Ilist[0] = 0;
        if (clearData) {
            Ilist[0] = 1;
        }
        Ilist[1] = id;
        Dlist[0] = age;
        Dlist[1] = -1.0;
        sendNativeCommand (SW_PS_CALC_SPLIT,
                           1,
                           Ilist,
                           null,
                           Dlist,
                           null,
                           null,
                           null,
                           null);

    }


/*-----------------------------------------------------------------------*/

  /**
  Clear all data currently set up for patch splitting.
  */
    public void psClearAllData ()
    {
        sendNativeCommand (SW_PS_CLEAR_ALL_DATA,
                           0,
                           Ilist,
                           null);
    }

/*-----------------------------------------------------------------------*/

  /**
  Clear the horizon data currently set up for patch splitting.  When a {@link #psCalcSplit} call is
  made, you can also tell it to clean up data when it is finished with the calculation
  if you want.  This is useful if you have a set of fault surfaces that can split several
  different horizons.  You can set up all the fault surfaces at the start.  Then, you can
  set up, calculate, and clear a horizon at a time using the same faults.
  */
    public void psClearHorizonData ()
    {
        sendNativeCommand (SW_PS_CLEAR_HORIZON_DATA,
                           0,
                           Ilist,
                           null);
    }

/*-----------------------------------------------------------------------*/

  /**
  Set the points used for the unsplit input patch.  These will be broken up into
  separate patches as split by the fault centerlines.  This version
  of the method takes separate x, y and z arrays.
  */
    public int psSetPoints (
        double[] x,
        double[] y,
        double[] z,
        int npts)
    {
        if (x == null  ||  y == null  ||  z == null  ||  npts < 3) {
            return -1;
        }

        newDdata (3 * npts);

        for (int i=0; i<npts; i++) {
            Ddata[i] = x[i];
            Ddata[npts+i] = y[i];
            Ddata[2*npts+i] = z[i];
        }

        Ilist[0] = npts;
        Ilist[1] = 0;

        long istat =
        sendNativeCommand (SW_PS_SET_POINTS,
                           0,
                           Ilist,
                           Ddata);

        return (int)istat;
    }

/*-----------------------------------------------------------------------*/

  /**
  Set the points used for the unsplit input patch.  These will be broken up into
  separate patches as split by the fault centerlines.  This version
  of the method takes a {@link TriMesh} object as input.
  */
    public int psSetPoints (
        TriMesh tmesh)
    {
        if (tmesh == null) {
            return -1;
        }

        double[] x = tmesh.getNodeXArray();
        double[] y = tmesh.getNodeYArray();
        double[] z = tmesh.getNodeZArray();

        int npts = tmesh.getNumNodes ();

        if (x == null  ||  y == null  ||  z == null  ||  npts < 3) {
            return -1;
        }

        newDdata (3 * npts);

        for (int i=0; i<npts; i++) {
            Ddata[i] = x[i];
            Ddata[npts+i] = y[i];
            Ddata[2*npts+i] = z[i];
        }

        Ilist[0] = npts;
        Ilist[1] = 1;

        long istat =
        sendNativeCommand (SW_PS_SET_POINTS,
                           0,
                           Ilist,
                           Ddata);

        return (int)istat;

    }

/*-----------------------------------------------------------------------*/

  /**
  Set the points used for the unsplit input patch.  These will be broken up into
  separate patches as split by the fault centerlines.  This version
  of the method takes a single {@link NodeArray3D} object.
  */
    public int psSetPoints (
        NodeArray3D nodes)
    {
        if (nodes == null) {
            return -1;
        }

        double[] x = nodes.getXArray();
        double[] y = nodes.getYArray();
        double[] z = nodes.getZArray();

        int npts = nodes.size ();

        if (x == null  ||  y == null  ||  z == null  ||  npts < 3) {
            return -1;
        }

        newDdata (3 * npts);

        for (int i=0; i<npts; i++) {
            Ddata[i] = x[i];
            Ddata[npts+i] = y[i];
            Ddata[2*npts+i] = z[i];
        }

        Ilist[0] = npts;
        Ilist[1] = 0;

        long istat =
        sendNativeCommand (SW_PS_SET_POINTS,
                           0,
                           Ilist,
                           Ddata);

        return (int)istat;

    }

/*-----------------------------------------------------------------------*/

  /**
  Add a border polygon segment to be used by the splitting algorithm.  These should
  represent the border of the patch prior to splitting.
  */
    public int psAddBorderSegment (
        double[]    xp,
        double[]    yp,
        double[]    zp,
        int         npts,
        int         type,
        int         flag
    )
    {
        if (xp == null  ||  yp == null  ||  npts < 2) {
            return -1;
        }

        newDdata (3 * npts);

        int zflag = 0;

        for (int i=0; i<npts; i++) {
            Ddata[i] = xp[i];
            Ddata[npts+i] = yp[i];
            if (zp != null) {
                Ddata[2*npts+i] = zp[i];
                if (zp[i] < 1.e20) {
                    zflag = 1;
                }
            }
            else {
                Ddata[2*npts+i] = 1.e30;
            }
        }

        Ilist[0] = npts;
        Ilist[1] = type;
        Ilist[2] = flag;
        Ilist[3] = zflag;

        long istat =
        sendNativeCommand (SW_PS_ADD_BORDER_SEGMENT,
                           0,
                           Ilist,
                           Ddata);

        return (int)istat;
    }

/*-------------------------------------------------------------------------*/

  /**
  Add the outline of the specified trimesh as a border segment to use in the
  patch splitting algorithm.
  @param tmesh A {@link TriMesh} object which defines the border via its outline.
  */
    public int psAddBorderSegment (
        TriMesh    tmesh
    )
    {
        JSResult3D r3d;

        r3d = outlineTriMesh (tmesh);
        if (r3d == null) {
            return -1;
        }

        int istat =
        psAddBorderSegment (
            r3d.x,
            r3d.y,
            r3d.z,
            r3d.npts,
            0,
            0);

        return istat;

    }


/*-------------------------------------------------------------------------*/

  /**
  Add a horizon surface patch to the model sealing input.  The patch is added as is.
  It is not split by any fault centerlines.
  @param tmesh A {@link TriMesh} object that defines the horizon surface patch.
  @param horizonID An identifier for the horizon.  This will be tagged onto the
  sealed version of the horizon.
  @param flag Set to 1 if this horizon is a fault detachment surface.
   Set to zero for anything else.
  @param age The age of the surface in millions of years.
  */
    public int psAddHorizonSurface (
        TriMesh    tmesh,
        int        horizonID,
        int        flag,
        double     age)
    {
        int        i;

        VertBaseline vb = tmesh.getVertBaseline ();

        if (vb == null) {
            vb = vertCalcBaseline (
                tmesh.getNodeXArray (),
                tmesh.getNodeYArray (),
                tmesh.getNodeZArray (),
                tmesh.getNumNodes (),
                false);
        }
        tmesh.vBase = vb;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[4] = horizonID;
        Ilist[5] = flag;

        if (vb == null) {
            Ilist[3] = -1;
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
        }
        else {
            if (vb.used) {
                Ilist[3] = 1;
            }
            Ilist[4] = horizonID;
            Dlist[0] = vb.c1;
            Dlist[1] = vb.c2;
            Dlist[2] = vb.c3;
            Dlist[3] = vb.x0;
            Dlist[4] = vb.y0;
            Dlist[5] = vb.z0;
        }

        Dlist[6] = age;

        if (tmesh.vBase != null) {
            Ilist[3] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[3] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat = sendNativeCommand (SW_PS_ADD_HORIZON_PATCH,
                                   1,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);

        return (int)istat;
    }


/*-------------------------------------------------------------------------*/

  /**
  Set the sediment surface for the model sealing.
  The sediment surface is not split by any fault centerlines.
  @param tmesh A {@link TriMesh} object that defines the sediment surface.
  @param horizonID An identifier for the sediment surface.  This will be tagged onto the
  sealed version of the surface.
  @param flag An optional flag not used for anything yet.
  */
    public int psAddToSedimentSurface (
        TriMesh    tmesh,
        int        horizonID,
        int        flag,
        double     age)
    {
        int        i;

        VertBaseline vb = tmesh.getVertBaseline ();

        if (vb == null) {
            vb = vertCalcBaseline (
                tmesh.getNodeXArray (),
                tmesh.getNodeYArray (),
                tmesh.getNodeZArray (),
                tmesh.getNumNodes (),
                false);
        }
        tmesh.vBase = vb;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[4] = horizonID;
        Ilist[5] = flag;

        if (vb == null) {
            Ilist[3] = -1;
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
        }
        else {
            if (vb.used) {
                Ilist[3] = 1;
            }
            Ilist[4] = horizonID;
            Dlist[0] = vb.c1;
            Dlist[1] = vb.c2;
            Dlist[2] = vb.c3;
            Dlist[3] = vb.x0;
            Dlist[4] = vb.y0;
            Dlist[5] = vb.z0;
        }

        if (tmesh.vBase != null) {
            Ilist[3] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[3] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        Dlist[6] = age;

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat = sendNativeCommand (SW_PS_SET_SED_SURFACE,
                                   1,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);

        return (int)istat;
    }

/*-------------------------------------------------------------------------*/

  /**
  Set the model bottom for the model sealing.
  The model bottom is not split by any fault centerlines.
  @param tmesh A {@link TriMesh} object that defines the model bottom.
  @param horizonID An identifier for the model bottom.  This will be tagged onto the
  sealed version of the surface.
  @param flag An optional flag not used for anything yet.
  */
    public int psAddToModelBottom (
        TriMesh    tmesh,
        int        horizonID,
        int        flag,
        double     age)
    {
        int        i;

        VertBaseline vb = tmesh.getVertBaseline ();

        if (vb == null) {
            vb = vertCalcBaseline (
                tmesh.getNodeXArray (),
                tmesh.getNodeYArray (),
                tmesh.getNodeZArray (),
                tmesh.getNumNodes (),
                false);
        }
        tmesh.vBase = vb;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[4] = horizonID;
        Ilist[5] = flag;

        if (vb == null) {
            Ilist[3] = -1;
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
        }
        else {
            if (vb.used) {
                Ilist[3] = 1;
            }
            Ilist[4] = horizonID;
            Dlist[0] = vb.c1;
            Dlist[1] = vb.c2;
            Dlist[2] = vb.c3;
            Dlist[3] = vb.x0;
            Dlist[4] = vb.y0;
            Dlist[5] = vb.z0;
        }

        if (tmesh.vBase != null) {
            Ilist[3] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[3] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        Dlist[6] = age;

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat = sendNativeCommand (SW_PS_SET_MODEL_BOTTOM,
                                   1,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);

        return (int)istat;
    }

/*-------------------------------------------------------------------------*/

  /**
  Add a fault surface to the model sealing input.  This is passed directly to the
  native SealedModel object.
  This surface stays in effect until a call to {@link #psClearAllData} is made.
  @param tmesh A {@link TriMesh} object that defines the fault surface.
  @param faultID An integer id tagged onto the sealed version eventually
  output by the model sealing.
  @param flag An optional flag that isn't really used for anything yet.
  */
    public int psAddFaultSurface (
        TriMesh    tmesh,
        int        faultID,
        int        flag,
        double     minage,
        double     maxage)
    {
        int        i;

        VertBaseline vb = tmesh.getVertBaseline ();

        if (vb == null) {
            vb = vertCalcBaseline (
                tmesh.getNodeXArray (),
                tmesh.getNodeYArray (),
                tmesh.getNodeZArray (),
                tmesh.getNumNodes (),
                true);
        }
        tmesh.vBase = vb;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[4] = faultID;
        Ilist[5] = flag;
        Dlist[10] = 1.e30; // no shape z value

        if (vb == null) {
            Ilist[3] = -1;
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
        }
        else {
            if (vb.used) {
                Ilist[3] = 1;
            }
            Ilist[4] = faultID;
            Dlist[0] = vb.c1;
            Dlist[1] = vb.c2;
            Dlist[2] = vb.c3;
            Dlist[3] = vb.x0;
            Dlist[4] = vb.y0;
            Dlist[5] = vb.z0;
        }

        if (tmesh.vBase != null) {
            Ilist[3] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[3] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        Dlist[6] = minage;
        Dlist[7] = maxage;

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat = sendNativeCommand (SW_PS_ADD_FAULT_SURFACE,
                                   0,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);

        return (int)istat;
    }

/*-------------------------------------------------------------------------*/

  /**
  Add a fault centerline that intersects the surface patch being split.  This version
  takes separate x, y and z arrays as input.
  */
    public int psAddFaultLine (
        double[]        xp,
        double[]        yp,
        double[]        zp,
        int             npts,
        int             faultID,
        int             flag
    )
    {
        if (xp == null  ||  yp == null  ||  npts < 2) {
            return -1;
        }

        newDdata (npts * 6);

        int zflag = 0;

        for (int i=0; i<npts; i++) {
            Ddata[i] = xp[i];
            Ddata[npts+i] = yp[i];

            if (zp != null) {
                Ddata[2*npts+i] = zp[i];
                if (zp[i] < 1.e20) {
                    zflag = 1;
                }
            }
            else {
                Ddata[2*npts+i] = 1.e30;
            }

        }

        Ilist[0] = npts;
        Ilist[1] = faultID;
        Ilist[2] = flag;
        Ilist[3] = zflag;

        long istat =
        sendNativeCommand (SW_PS_ADD_CENTERLINE,
                           0,
                           Ilist,
                           Ddata);

        return (int)istat;
    }

/*-------------------------------------------------------------------------*/


  /**
  Add a fault centerline that intersects the surface patch being split.  This version
  takes a XYZPolyline object as input.
  */
    public int psAddFaultCenterline (
        XYZPolyline       line,
        int             faultID,
        int             flag
    )
    {
        if (line == null) {
            return -1;
        }

        int npts = line.size ();
        double[] xp = line.getXArray ();
        double[] yp = line.getYArray ();
        double[] zp = line.getZArray ();

        if (xp == null  ||  yp == null  ||  npts < 2) {
            return -1;
        }

        newDdata (npts * 6);

        int zflag = 0;

        for (int i=0; i<npts; i++) {
            Ddata[i] = xp[i];
            Ddata[npts+i] = yp[i];

            if (zp != null) {
                Ddata[2*npts+i] = zp[i];
                if (zp[i] < 1.e20) {
                    zflag = 1;
                }
            }
            else {
                Ddata[2*npts+i] = 1.e30;
            }

        }

        Ilist[0] = npts;
        Ilist[1] = faultID;
        Ilist[2] = flag;
        Ilist[3] = zflag;

        long istat =
        sendNativeCommand (SW_PS_ADD_CENTERLINE,
                           0,
                           Ilist,
                           Ddata);

        return (int)istat;
    }


/*-----------------------------------------------------------------------
 *
 * The following group of methods are related to "vertical" or at least steeply
 * dipping surfaces.  These surfaces need special handling in the native code.
 * The java code needs to determine if such handling is needed and tell the native
 * code about what is needed.  The handling instuctions are encapsulated in a
 * VertBaseline object.
 */

/*----------------------------------------------------------------------*/

/**
 Determine if a set of points is best represented using "steep surface coordinates".
 The method returns a {@link VertBaseline} object which contains the information
 needed to fit a steep surface to the set of points.  If an error occurs, a null object
 is returned.
 @param xpts X coordinates to be used to build a surface.
 @param ypts Y coordinates to be used to build a surface.
 @param zpts Z coordinates to be used to build a surface.
 @param npts Number of points.
 @param faultFlag  True if the surface to be built is a fault or false if the surface
 to be built is not a fault.
*/
    public VertBaseline vertCalcBaseline (
        double[]    xpts,
        double[]    ypts,
        double[]    zpts,
        int         npts,
        boolean     faultFlag
    )
    {

        newDdata (npts * 3);

        Ilist[0] = npts;
        if (faultFlag) {
            Ilist[1] = 1;
        }
        else {
            Ilist[1] = 0;
        }

        for (int i=0; i<npts; i++) {
            Ddata[i] = xpts[i];
            Ddata[npts + i] = ypts[i];
            Ddata[npts * 2 + i] = zpts[i];
        }

        long istat =
        sendNativeCommand (SW_VERT_CALC_BASELINE,
                           1,
                           Ilist,
                           Ddata);

        if (istat == -1) {
            return null;
        }

        VertBaseline vbl = getVertBaseline ();

        return vbl;

    }

/*----------------------------------------------------------------------------*/

/**
Set the vertical baseline data for subsequent calculations.  You should specify a
{@link VertBaseline} object that was previously calculated via the {@link #vertCalcBaseline}
method.  If the specified baseline is null, then the native calculations are set to
not use a vertical baseline at all.  It is not correct to ignore the null baseline,
because that will leave the native code using the last correct baseline, which probably
will not be correct for the current data.
*/
    public void vertSetBaseline (VertBaseline vbl)
    {
        if (vbl == null) {
            Ilist[0] = 0;
            Ddata[0] = 1.e30;
            Ddata[1] = 1.e30;
            Ddata[2] = 1.e30;
            Ddata[3] = 1.e30;
            Ddata[4] = 1.e30;
            Ddata[5] = 1.e30;
        }

        else {
            if (vbl.used) {
                Ilist[0] = 1;
            }
            else {
                Ilist[0] = 0;
            }

            Ddata[0] = vbl.c1;
            Ddata[1] = vbl.c2;
            Ddata[2] = vbl.c3;
            Ddata[3] = vbl.x0;
            Ddata[4] = vbl.y0;
            Ddata[5] = vbl.z0;
        }

        sendNativeCommand (SW_VERT_SET_BASELINE,
                           1,
                           Ilist,
                           Ddata);
    }

/*---------------------------------------------------------------------*/

/**
 Determine if a set of points is best represented using "steep surface coordinates".
 The method returns a {@link VertBaseline} object which contains the information
 needed to fit a steep surface to the set of points.  If an error occurs, a null object
 is returned.
 @param nodes {@link NodeArray3D} object with the nodes.
 @param lineList {@link ArrayList}<XYZPolyline> object with the constraint lines.
 @param faultFlag  True if the surface to be built is a fault or false if the surface
 to be built is not a fault.
*/

    public VertBaseline vertCalcBaseline (
        ArrayList<XYZNode>     nodes,
        ArrayList<XYZPolyline>   lineList,
        boolean         faultFlag
    )
    {
        int             i, j, n, npts, ntot;
        XYZPolyline la;

    /*
     * Find the total number of points and line points.  Make sure
     * the double data buffer is large enough for this many points.
     */
        ntot = 0;
        if (nodes != null) {
            ntot = nodes.size ();
        }
        if (lineList != null) {
            for (i=0; i<lineList.size(); i++) {
                la = lineList.get (i);
                if (la == null) {
                    continue;
                }
                ntot += la.size ();
            }
        }

        newDdata (ntot * 3);

    /*
     * Copy the points into the double data buffer.
     */
        npts = 0;
        if (nodes != null) {
            npts = nodes.size ();

            if (npts > 0) {
                XYZNode  xyzn;
                for (i=0; i<npts; i++) {
                    xyzn = nodes.get(i);
                    Ddata[i] = xyzn.getX();
                    Ddata[ntot+i] = xyzn.getY();
                    Ddata[ntot*2+i] = xyzn.getZ();
                }
            }
        }

        n = npts;

    /*
     * Copy the line points into the double data buffer.
     */
        if (lineList != null) {

            int          ntmp;
            double[]     xt, yt, zt;

            int numLines = lineList.size ();

            for (i=0; i<numLines; i++) {
                la = lineList.get (i);
                if (la == null) {
                    continue;
                }
                xt = la.getXArray ();
                yt = la.getYArray ();
                zt = la.getZArray ();
                ntmp = la.size ();

                if (ntmp < 2) {
                    continue;
                }
                for (j=0; j<ntmp; j++) {
                    Ddata[n] = xt[j];
                    Ddata[ntot+n] = yt[j];
                    Ddata[ntot*2+n] = zt[j];
                    n++;
                }
            }
        }

        Ilist[0] = ntot;
        Ilist[1] = 0;
        if (faultFlag) {
            Ilist[1] = 1;
        }

        long istat =
        sendNativeCommand (SW_VERT_CALC_BASELINE,
                           1,
                           Ilist,
                           Ddata);

        if (istat == -1) {
            return null;
        }

        VertBaseline vbl = getVertBaseline ();

        return vbl;

    }

/*----------------------------------------------------------*/

/**
 Convert a node and triangle list to a node, edge, triangle {@link TriMesh}
 object.  The {@link NodeArray3D} input and the {@link TriangleArray} input
 should be from an ExternalDataTSurf object.  I cannot reference the
 external data tsurf object directly here because the package that defines
 it has not been built yet when this package is being built.
*/
    public TriMesh convertTriMesh (
        NodeArray3D    nodes,
        TriangleArray  tris
    )
    {

    /*
     * Make sure the tsurf and its parts are not null or empty.
     */
        if (tris == null) {
            return null;
        }

        int num_tris = tris.getSize ();
        if (num_tris < 1) {
            return null;
        }

        if (nodes == null) {
            return null;
        }

        int num_nodes = nodes.size ();
        if (num_nodes < 3) {
            return null;
        }

    /*
     * Get the internal node and triangle primitive arrays and
     * copy them to the native interface buffers.
     */
        double[] xn = nodes.getXArray ();
        double[] yn = nodes.getYArray ();
        double[] zn = nodes.getZArray ();
        int[] n1 = tris.getNode1Array ();
        int[] n2 = tris.getNode2Array ();
        int[] n3 = tris.getNode3Array ();

        newDdata (3 * num_nodes);
        newIdata (3 * num_tris);

        int i;

        for (i=0; i<num_nodes; i++) {
            Ddata[i] = xn[i];
            Ddata[num_nodes+i] = yn[i];
            Ddata[2*num_nodes+i] = zn[i];
        }
        Ilist[0] = num_nodes;

        for (i=0; i<num_tris; i++) {
            Idata[i] = n1[i];
            Idata[num_tris+i] = n2[i];
            Idata[2*num_tris+i] = n3[i];
        }
        Ilist[1] = num_tris;

    /*
     * Convert the trimesh.  This also calculates a vertical
     * baseline.  To make sure we don't use an old baseline,
     * the baseline is cleared before calling the convert method.
     */
        clearVertBaseline ();
        long istat =
        sendNativeCommand (SW_CONVERT_NODE_TRIMESH,
                           1,
                           Ilist,
                           null,
                           null,
                           null,
                           Idata,
                           null,
                           Ddata);
        if (istat == -1) {
            return null;
        }

    /*
     * Build the trimesh object from the returned nodes,
     * edges and triangles.  This function is in the base
     * class, where the returned data also resides.
     */
        TriMesh tmesh = buildTriMesh ();

    /*
     * Set the steep coordinate info for the trimesh.
     */
        VertBaseline vbl = getVertBaseline ();
        clearVertBaseline ();
        if (vbl == null) {
            vbl = new VertBaseline ();
        }
        tmesh.vBase = vbl;

    /*
     * Set the trimesh external data flag to true.
     */
        tmesh.externalTsurf = true;

        //tmesh.dumpLimits ();

        return tmesh;

    }

/*------------------------------------------------------------------*/

  /**
   * Write a {@link TriMesh} object to a portable binary file.  If the file is successfully
   * written, a long value is returned which represents the position in the file where the
   * data was written.  If the file could not be written, -1 is returned.
   *
   * @param tmesh A {@link TriMesh} object to be written to the binary file.
   * @param fname The name of the new trimesh file.  If a file by this name exists
   * it will be appended to.
   * @return position in file if written correctly, -1 otherwise.
   */
    public long writeBinaryTriMeshFile (
      TriMesh tmesh,
      String fname
    ) {
      if (fname == null  ||  tmesh == null)
        return -1L;

      long position = writeTriMeshFile(
        tmesh,
        fname,
        true
      );
      return(position);
    }

/*------------------------------------------------------------------*/

  /**
   * Write a {@link TriMesh} object to an ascii text file.  If the file is successfully
   * written, a long value is returned which represents the position in the file where the
   * data was written.  If the file could not be written, -1 is returned.
   *
   * @param tmesh A {@link TriMesh} object to be written to the binary file.
   * @param fname The name of the new trimesh file.  If a file by this name exists
   * it will be appended to.
   * @return position in file if written correctly, -1 otherwise.
   */
    public long writeTextTriMeshFile(
      TriMesh tmesh,
      String fname
    ) {
      if (fname == null  ||  tmesh == null)
        return(-1L);

      long position = writeTriMeshFile (
        tmesh,
        fname,
        false
      );
      return(position);
    }

/*------------------------------------------------------------------*/

/**
 Read a trimesh from the specified file.  The file can be a binary file written
 via the {@link #writeBinaryTriMeshFile} method or it can be a text file written
 using the {@link #writeTextTriMeshFile} method.  On success, a valid TriMesh
 object is returned.  On failure, null is returned.
@param fname File name to read.
*/
    public TriMesh readTriMeshFile(String fname, long position) {
      if (fname == null) {
        return null;
      }
//      System.out.print("readTriMeshFile("+fname+", "+position+")");
      Llist[0] = position;
      long istat = sendNativeCommand(
        SW_READ_TRIMESH_DATA,
        1,
        null,
        Llist,
        null,
        fname,
        null,
        null,
        null
      );
//      System.out.println(" istat="+istat);
      if (istat == -1) {
        System.out.println ("error reading trimesh file");
        return null;
      }
      /*
       * Build the trimesh object from the returned nodes,
       * edges and triangles.  This function is in the base
       * class, where the returned data also resides.
       */
      TriMesh tmesh = buildTriMesh();
      return(tmesh);
    }


/*------------------------------------------------------------------*/

/**
 Read a csw grid file of the specified name.  The file is used to create a
 {@link CSWGrid} object which is returned.
 @param fname File name to read.
*/
    public CSWGrid readCSWGridFile (String fname)
    {
      sendNativeCommand(
        SW_READ_CSW_GRID,
        1,
        null,
        null,
        null,
        fname,
        null,
        null,
        null
      );
      CSWGrid cgrid = buildCSWGrid();
      return cgrid;
    }

/*------------------------------------------------------------------*/


    private long writeTriMeshFile (
        TriMesh    tmesh,
        String     fname,
        boolean    bflag
    )
    {
        int        i;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[3] = 0;
        if (bflag) {
            Ilist[3] = 1;
        }

        Ilist[4] = -1;
        Dlist[0] = 1.e30;
        Dlist[1] = 1.e30;
        Dlist[2] = 1.e30;
        Dlist[3] = 1.e30;

        if (tmesh.vBase != null) {
            Ilist[4] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[4] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long position = sendNativeCommand (SW_WRITE_TRIMESH_DATA,
                                   0,
                                   Ilist,
                                   null,
                                   Dlist,
                                   fname,
                                   Idata,
                                   null,
                                   Ddata);
        return(position);
    }

/*-----------------------------------------------------------*/

    public TriMesh extendFault (
        TriMesh     tmesh,
        double      extendFraction
    )
    {
        int        i;

        if (tmesh == null) {
            return null;
        }

    /*
     * Set the steep surface coordinates if needed.
     */
        vertSetBaseline (tmesh.vBase);

    /*
     * Send just the nodes of the trimesh for extension.
     */
        Ilist[0] = tmesh.getNumNodes ();
        Dlist[0] = extendFraction;

        newDdata (3 * Ilist[0]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        long istat = sendNativeCommand (SW_EXTEND_FAULT,
                                   1,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   null,
                                   null,
                                   Ddata);

        if (istat == -1) {
            return null;
        }

    /*
     * Build the trimesh object from the returned nodes,
     * edges and triangles.  This function is in the base
     * class, where the returned data also resides.
     */
        TriMesh tm2 = buildTriMesh ();

        //tm2.dumpLimits ();

        return tm2;

    }

/*------------------------------------------------------------------*/

  /*
   * package scope, only used for debug from unit test.
   */
    public void readBugFiles ()
    {
        DBLineList       dbl;
        DBPointList      dbp;

        dbl = new DBLineList ();

        dbp = new DBPointList ();

        try {
            dbl.loadAsciiFile ("bug.lines");
        }
        catch (Exception e) {
            System.out.println ("Bad file name for line file");
            System.out.flush ();
            return;
        }

        int nline = dbl.numLines;
        int[] iline = dbl.linePts;
        double[] xnodes = dbl.x;
        double[] ynodes = dbl.y;
        double[] znodes = dbl.z;

        double[] xa;
        double[] ya;
        double[] za;
        int      npts, i, n;
        XYZPolyline pline;
        ArrayList<XYZPolyline> plist = new ArrayList<XYZPolyline> (nline);

        n = 0;
        for (int ido=0; ido<nline; ido++) {
            npts = iline[ido];

            pline = new XYZPolyline (npts);

            xa = pline.getXArray ();
            ya = pline.getYArray ();
            za = pline.getZArray ();

            for (i=0; i<npts; i++) {
                xa[i] = xnodes[n];
                ya[i] = ynodes[n];
                za[i] = znodes[n];
                n++;
            }
            plist.add (pline);
        }

        try {
            dbp.loadAsciiFile ("bug.points");
        }
        catch (Exception e) {
            System.out.println ("Bad file name for point file");
            System.out.flush ();
            return;
        }

        npts = dbp.npts;
        xnodes = dbp.x;
        ynodes = dbp.y;
        znodes = dbp.z;

        ArrayList<XYZNode> na = new ArrayList<XYZNode> (npts);

        XYZNode   nd;

        for (i=0; i<npts; i++) {
            nd = na.get(i);
            nd.setX (xnodes[i]);
            nd.setY (ynodes[i]);
            nd.setZ (znodes[i]);
            nd.setFlag (0);
        }

        calcTriMesh (
            na,
            plist,
            null,
            1,
            null,
            null,
            true);

    }

/*------------------------------------------------------*/

/*
 * Return the outer boundary of a trimesh.  The boundary does not
 * include any holes that may be present in the trimesh.
 */
    public JSResult3D outlineTriMesh (
        TriMesh     tmesh
    )
    {

        int         i;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long lstat =
        sendNativeCommand (SW_TRIMESH_OUTLINE,
                           1,
                           Ilist,
                           null,
                           null,
                           null,
                           Idata,
                           null,
                           Ddata);

        if (lstat == -1) {
            return null;
        }

        JSResult3D  result;

        result = getOutlineResult3D ();

        return result;

    }

/*------------------------------------------------------*/

/**
 Calculate the convex hull outline of the specified set of points.
 A {@link JSResult2D} object is filled in with the outline x and
 y arrays and returned by the method.
@param xpts Array with x coordinates of the points.
@param ypts Array with y coordinates of the points.
@param npts Number of points.
*/
    public JSResult2D outlinePoints (
        double[] xpts,
        double[] ypts,
        int      npts)
    {

        newDdata (npts * 2);
        Ilist[0] = npts;
        for (int i=0; i<npts; i++) {
            Ddata[i] = xpts[i];
            Ddata[i+npts] = ypts[i];
        }

        long lstat =
        sendNativeCommand (
                 SW_OUTLINE_POINTS,
                 1,
                 Ilist,
                 Ddata);

        if (lstat == -1) {
            return null;
        }

        JSResult2D r = getOutlineResult ();
        return r;
    }


/*------------------------------------------------------*/

    private double      grazingValue = 1.0;

/**
Set the grazing value for use with polygon operations.  Currently the
only method this affects is {@link #pointInPoly}.  If the specified
value is less than or equal to zero, 1.0 is used.
*/
    public void setGrazingValue (double value)
    {
        if (value <= 0.0) {
            value = 1.0;
        }
        grazingValue = value;
    }

/**
Determine if the x,y point is inside, outside or on the edge of
the specified polygon.  This is used for a simple non self intersecting
polygon without holes.  A return value of 1 means the point is
inside.  A return value of -1 means the point is outside.  A return
value of zero means the point is on the edge.
@param x X coordinate of point to test.
@param y Y coordinate of point to test.
@param pline An {@link XYPolyline} object that has the x, y points of the polygon.
*/
    public int pointInPoly (
        double      x,
        double      y,
        XYPolyline  pline)
    {
        if (pline == null) {
            return -1;
        }

        double[]     xa, ya;
        int          npts;

        xa = pline.getXArrayCopy ();
        ya = pline.getYArrayCopy ();
        npts = pline.size ();

        int istat =
        pointInPoly (x, y, xa, ya, npts);

        return istat;

    }

/**
Determine if the x,y point is inside, outside or on the edge of
the specified polygon.  This is used for a simple non self intersecting
polygon without holes.  A return value of 1 means the point is
inside.  A return value of -1 means the point is outside.  A return
value of zero means the point is on the edge.
@param x X coordinate of point to test.
@param y Y coordinate of point to test.
@param xpts Array with x coordinates of the polygon.
@param ypts Array with y coordinates of the polygon.
@param npts Number of points.
*/
    public int pointInPoly (
        double   x,
        double   y,
        double[] xpts,
        double[] ypts,
        int      npts)
    {

        newDdata (npts * 2);
        Ilist[0] = npts;
        for (int i=0; i<npts; i++) {
            Ddata[i] = xpts[i];
            Ddata[i+npts] = ypts[i];
        }

        Dlist[0] = x;
        Dlist[1] = y;
        Dlist[2] = grazingValue;

        long lstat =
        sendNativeCommand (SW_POINT_IN_POLY,
                           0,
                           Ilist,
                           null,
                           Dlist,
                           null,
                           null,
                           null,
                           Ddata);

        int istat = (int)lstat;

        return istat;

    }

/*-----------------------------------------------------------*/

    public void testTriSpeed (int npts)
    {
        Ilist[0] = npts;
        Date date = new Date ();
        long l1 = date.getTime();
        sendNativeCommand (SW_TEST_TRI_SPEED,
                     0,
                     Ilist,
                     null,
                     null,
                     null,
                     null,
                     null,
                     null);
        date = new Date ();
        long l2 = date.getTime() - l1;

        System.out.println ("time in milliseconds = "+l2);
    }


/*-------------------------------------------------------------*/

/**
 Calculate a trimesh that exactly honors the specified lines.  This is the
 trimesh calculation method used for creating sealed surfaces.  A grid with
 (for all intensive purposes) square cells is used as an intermediate step
 in creating the trimesh.
 If gridOptions is null, it is ignored in creating the intermediate grid and
 default options are used.  If gridGeometry is null, it is ignored and default
 geometry is used for the intermediate grid.
@return Returns a {@link TriMesh} object, or null if an error occurs.
@param nodes An optional {@link NodeArray3D} object with the input points.
@param lineList A mandatory {@link ArrayList}<XYZPolyline> object with the constraint lines.
@param boundary  An optional {@link XYZPolyline} object with a single line defining the boundary.
used as the external boundary the tri mesh.
@param gridGeometry Optional {@link GridGeometry} object for the intermediate grid.
@param gridOptions Optional {@link GridCalcOptions} object for the intermediate grid.
@param faultSurfaceFlag Set to true if the trimesh will become part of a fault surface
or set to false if the trimesh will be used for anything else.
*/
    public TriMesh calcExactTriMesh (
        ArrayList<XYZNode>        nodes,
        ArrayList<XYZPolyline>    lineList,
        XYZPolyline        boundary,
        GridGeometry       gridGeometry,
        GridCalcOptions    gridOptions,
        boolean            faultSurfaceFlag)
    {
        int          i, j, n, nl;

    /*
     * Check some obvious errors.
     */
        if (lineList == null) {
            return null;
        }

        if (lineList.size() < 2) {
            return null;
        }

    /*
     * Check if the surface is steep enough to require
     * special "steep surface" handling.
     */
        VertBaseline vbl =
        vertCalcBaseline (
            nodes,
            lineList,
            faultSurfaceFlag);
        vertSetBaseline (vbl);

    /*
     * If there are less than 3 nodes and 2 or more lines.
     * Put the first and last points from the lines
     * into the node array so there are enough nodes
     * for trimesh calculation.
     */
        if (nodes == null) {
            n = 0;
        }
        else {
            n = nodes.size();
        }
        nl = lineList.size();
        if (n < 3) {
            ArrayList<XYZNode> na = new ArrayList<XYZNode> (n + 2 * nl);
            if (n > 0  &&  nodes != null) {
                //XYZNode xyzn1;
                for (i=0; i<n; i++) {
                    //xyzn1 = nodes.get(i);
                    //xyzn2 = na.get(i);
                    //xyzn2 = xyzn1;
                }
            }
            XYZNode  xyzn;
            for (i=0; i<nl; i++) {
                XYZPolyline lt = lineList.get (i);
                double[] xt = lt.getXArray();
                double[] yt = lt.getYArray();
                double[] zt = lt.getZArray();
                xyzn = na.get(n);
                xyzn.setX (xt[0]);
                xyzn.setY (yt[0]);
                xyzn.setZ (zt[0]);
                n++;
                j = xt.length - 1;
                xyzn = na.get(n);
                xyzn.setX (xt[j]);
                xyzn.setY (yt[j]);
                xyzn.setZ (zt[j]);
                n++;
            }
            nodes = na;
        }

    /*
     * Send the contraint line data.
     */
        int          ntot, ntmp, zflag;
        double[]     xt, yt, zt;

        int numLines = lineList.size ();

        Ilist[0] = numLines;
        newIdata (numLines * 2);
        zflag = 1;

        XYZPolyline la;
        ntot = 0;
        for (i=0; i<numLines; i++) {
            la = lineList.get (i);
            if (la == null) {
                continue;
            }
            ntot += la.size ();
        }

        if (ntot > 1) {
            newDdata (3 * ntot);
            n = 0;
            for (i=0; i<numLines; i++) {
                la = lineList.get (i);
                if (la == null) {
                    Idata[i] = 0;
                    Idata[numLines+i] = 0;
                    continue;
                }
                xt = la.getXArray ();
                yt = la.getYArray ();
                zt = la.getZArray ();
                ntmp = la.size ();
                if (ntmp < 2) ntmp = 0;
                Idata[i] = ntmp;
                Idata[i+numLines] = 0;
                if (ntmp < 1) {
                    continue;
                }
                Idata[i+numLines] = 0;

                for (j=0; j<ntmp; j++) {
                    Ddata[n] = xt[j];
                    Ddata[ntot+n] = yt[j];
                    Ddata[ntot*2+n] = zt[j];
                    n++;
                }
            }
            Ilist[1] = ntot;
            Ilist[2] = zflag;

            long istat = sendNativeCommand (SW_SET_LINES,
                                       0,
                                       Ilist,
                                       null,
                                       null,
                                       null,
                                       Idata,
                                       null,
                                       Ddata);
            if (istat == -1) {
                return null;
            }

        } // end of sending lines section

    /*
     * Send the boundary if needed.
     */
        if (boundary != null) {

            xt = boundary.getXArray ();
            yt = boundary.getYArray ();
            if (xt == null  ||  yt == null) {
            }
            else {
                zt = boundary.getZArray ();
                zflag = 1;
                ntmp = boundary.size ();
                if (ntmp > 1) {
                    newDdata (ntmp * 3);
                    for (i=0; i<ntmp; i++) {
                        Ddata[i] = xt[i];
                        Ddata[ntmp+i] = yt[i];
                        Ddata[ntmp*2+i] = zt[i];
                    }

                    Ilist[0] = ntmp;
                    Ilist[1] = zflag;

                    long istat = sendNativeCommand (SW_SET_BOUNDS,
                                               0,
                                               Ilist,
                                               Ddata);
                    if (istat == -1) {
                        return null;
                    }
                }

            }

        }  // end of sending boundary section

    /*
     * Send the grid options and geometry.
     */
        sendGridCalcOptions (gridOptions);
        sendGridGeometry (gridGeometry);

    /*
     * Send the calculate trimesh command.
     */
        int numPoints = 0;
        if (nodes != null) {
            numPoints = nodes.size ();
        }
        Ilist[0] = numPoints;
        Ilist[1] = 1;
        Ilist[2] = 0;
        if (faultSurfaceFlag) {
            Ilist[2] = 1;
        }
        Ilist[3] = 3;
        if (numPoints > 0) {
            XYZNode   xyzn;
            for (i=0; i<numPoints; i++) {
                xyzn = nodes.get(i);
                Ddata[i] = xyzn.getX();
                Ddata[numPoints+i] = xyzn.getY();
                Ddata[numPoints*2+i] = xyzn.getZ();
            }
        }

        long istat = sendNativeCommand (SW_CALC_EXACT_TRI_MESH,
                                       1,
                                       Ilist,
                                       Ddata);
        if (istat == -1) {
            return null;
        }

    /*
     * Build the trimesh object from the returned nodes,
     * edges and triangles.  This function is in the base
     * class, where the returned data also resides.
     */
        TriMesh tmesh = buildTriMesh ();

        tmesh.vBase = vbl;

        //tmesh.dumpLimits ();

        return tmesh;

    }

/*---------------------------------------------------------*/

 /**
  * Clear all the native side draped line caches, including trimesh data
  * and any input data from draping lines and points.  It is not strictly
  * neccessary to call this method, but calling it when you are completely
  * done with a draping operation will remove several "still reachable"
  * memory loss records in the valgrind output.
  */
    static public void clearDrapeCache ()
    {

        JSurfaceWorks jsw = new JSurfaceWorks ();
        jsw.calcDrapedPoints ((TriMesh)null, (ArrayList<XYZPolyline>)null);

        return;
    }

/*---------------------------------------------------------*/

  /**
  Calculate draped points on the specified trimesh.  If the trimesh is
  the same as the previous call, the cached trimesh in the native side
  is used.  To clean up the cached data on the native side, specify
  a null tmesh object.
  <p>
  The results are returned as an {@link ArrayList}<XYZPoint> object.  The x and y
  coordinates will be the same as the input points.  The z coordinate will
  be the elevation of the trimesh at the input point.  There may be less
  points returned than were passed to the method.  If a point is not located
  on the trimesh, it is ignored and not returned.
  @param tmesh A {@link TriMesh} object to drape onto.
  @param inputLines A {@link ArrayList}<XYZPolyline> object.  The points defined
   in these lines will be draped onto the tri mesh.
  */
    public ArrayList<XYZPoint> calcDrapedPoints (TriMesh tmesh,
                                ArrayList<XYZPolyline> inputLines)
    {

        int          i, j, n, ntot;
        int          ntmp, zflag;
        double[]     xt, yt, zt;
        XYZPolyline    la;

        if (tmesh == null) {
            sendNativeCommand (
                SW_CLEAR_DRAPE_CACHE,
                0,
                Ilist);

            lastDrapedTriMesh = 0;
            return null;
        }

        if (inputLines == null  ||
            tmesh == null) {
            return null;
        }

        if (tmesh.getNumNodes () < 3  ||
            tmesh.getNumEdges () < 3  ||
            tmesh.getNumTriangles () < 1) {
            return null;
        }

      /*
       * Send the input line data to the native side.
       */
        int numLines = inputLines.size ();

        newIdata (numLines);
        zflag = 0;

        ntot = 0;
        for (i=0; i<numLines; i++) {
            la = inputLines.get (i);
            ntot += la.size ();
        }

        if (ntot < 1) {
            return null;
        }

        VertBaseline vbl = tmesh.getVertBaseline ();
        vertSetBaseline (vbl);

        newDdata (2 * ntot);
        n = 0;
        for (i=0; i<numLines; i++) {
            la = inputLines.get (i);
            xt = la.getXArray ();
            yt = la.getYArray ();
            zt = la.getZArray ();
            ntmp = la.size ();

            if (ntmp < 1) ntmp = 0;
            Idata[i] = ntmp;

            for (j=0; j<ntmp; j++) {
                Ddata[n] = xt[j];
                Ddata[ntot+n] = yt[j];
                Ddata[2*ntot+n] = zt[j];
                n++;
            }
        }
        Ilist[0] = numLines;
        Ilist[1] = ntot;
        Ilist[2] = zflag;

        long istat = sendNativeCommand (SW_SET_DRAPE_POINTS,
                                   0,
                                   Ilist,
                                   null,
                                   null,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);
        if (istat == -1) {
            return null;
        }

      /*
       * Send the trimesh data to the native side if it is different from
       * the last trimesh sent.
       */

        if (tmesh.getUniqueID() != lastDrapedTriMesh) {

            Ilist[0] = tmesh.getNumNodes ();
            Ilist[1] = tmesh.getNumEdges ();
            Ilist[2] = tmesh.getNumTriangles ();

            long id = tmesh.getUniqueID();
            int  id1, id2;
            long left, right;
            left = id >>> 32;
            right = id << 32;
            right = right >>> 32;
            id1 = (int)left;
            id2 = (int)right;
            Ilist[3] = id1;
            Ilist[4] = id2;

            newDdata (3 * Ilist[0]);
            newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

            double[] xnode = tmesh.getNodeXArray ();
            double[] ynode = tmesh.getNodeYArray ();
            double[] znode = tmesh.getNodeZArray ();

            ntot = Ilist[0];
            for (i=0; i<ntot; i++) {
                Ddata[i] = xnode[i];
                Ddata[i+ntot] = ynode[i];
                Ddata[i+2*ntot] = znode[i];
            }

            int n0;
            n0 = 0;

            int[] n1 = tmesh.getEdgeNode0Array ();
            int[] n2 = tmesh.getEdgeNode1Array ();
            int[] t1 = tmesh.getEdgeTriangle0Array ();
            int[] t2 = tmesh.getEdgeTriangle1Array ();

            ntot = Ilist[1];
            for (i=0; i<ntot; i++) {
                Idata[n0+i] = n1[i];
                Idata[n0+ntot+i] = n2[i];
                Idata[n0+2*ntot+i] = t1[i];
                Idata[n0+3*ntot+i] = t2[i];
            }

            n0 += 4 * ntot;
            int[] e1 = tmesh.getTriangleEdge0Array ();
            int[] e2 = tmesh.getTriangleEdge1Array ();
            int[] e3 = tmesh.getTriangleEdge2Array ();

            ntot = Ilist[2];
            for (i=0; i<ntot; i++) {
                Idata[n0+i] = e1[i];
                Idata[n0+i+ntot] = e2[i];
                Idata[n0+i+2*ntot] = e3[i];
            }

            istat = sendNativeCommand (SW_TRIMESH_DATA,
                                       0,
                                       Ilist,
                                       null,
                                       null,
                                       null,
                                       Idata,
                                       null,
                                       Ddata);

            if (istat == -1) {
                return null;
            }

            lastDrapedTriMesh = tmesh.getUniqueID();

        }  // end of trimesh data block

      /*
       * Use the trimesh and lines cached on the native side to
       * calculate the draped lines.  When this returns, the
       * individual draped lines are available in the base classes
       * data.
       */
        drapedLineList = null;
        sendNativeCommand (
            SW_CALC_DRAPED_POINTS,
            1,
            Ilist);

        ArrayList<XYZPoint> list = buildDrapedPointList ();

        return list;
    }

/*-------------------------------------------------------------------------*/


/**
 Calculate a triangular mesh with a constant z value.
@param  zvalue The constant elevation for the trimesh.
@param  pline An {@link XYPolyline} object with the boudary of the trimesh.
@return Returns a {@link TriMesh} object, or null if an error occurs.
*/
    public TriMesh calcTriMesh (
        double             zvalue,
        XYPolyline         pline)
    {
        int          i;

    /*
     * Check some obvious errors.
     */
        if (zvalue > 1.e20  ||  zvalue < -1.e20) {
            return null;
        }
        if (pline == null) {
            return null;
        }

    /*
     * Fill the Ddata array with the xy boundary points.
     */
        int         ntmp;
        double[]    xt, yt;

        xt = pline.getXArrayCopy ();
        yt = pline.getYArrayCopy ();
        ntmp = pline.size ();
        if (ntmp < 3) {
            return null;
        }

        newDdata (ntmp * 2);
        for (i=0; i<ntmp; i++) {
            Ddata[i] = xt[i];
            Ddata[ntmp+i] = yt[i];
        }

        Ilist[0] = ntmp;

    /*
     * Send the calculate trimesh command.
     */
        Dlist[0] = zvalue;
        long istat = sendNativeCommand (SW_CALC_CONSTANT_TRI_MESH,
                                       1,
                                       Ilist,
                                       Dlist,
                                       Ddata);
        if (istat == -1) {
            return null;
        }

    /*
     * Build the trimesh object from the returned nodes,
     * edges and triangles.  This function is in the base
     * class, where the returned data also resides.
     */
        TriMesh tmesh = buildTriMesh ();

        tmesh.vBase = null;

        //tmesh.dumpLimits ();

        return tmesh;

    }

/*-----------------------------------------------------------------*/

    /**
     * Resample a line at equally spaced points as close as possible to the specified
     * average spacing.  The first and last points of the line are not changed.  However,
     * other points in the middle of the line will be moved to the closest resampled point.
     * @param inputLine An {@link XYPolyline} object to be resampled.
     * @param avspace Close approximation to the average spacing on the output line.
     * @return The resampled XYPolyline.
     */
    public XYPolyline resampleLine (
        XYPolyline      inputLine,
        double          avspace)
    {
        double[]   line_dist;
        double     dx, dy, dist, dtot;
        double[]   x, y;
        double[]   xt, yt, zt;
        int        istat, i, nout, nmax, ibase;
        int[]      nextbase;

    /*
     * Obvious errors.
     */
        if (inputLine == null) {
            return null;
        }
        if (avspace <= 0.0) {
            return null;
        }

        double[]               xin, yin, zin;
        int                    npts;

        xin = inputLine.getXArrayCopy ();
        yin = inputLine.getYArrayCopy ();
        npts = inputLine.size ();
        zin = new double[npts];

    /*
     * Allocate space for distance to point arrays.
     */
        line_dist = new double[npts];

    /*
     * Fill in distance along line at each point values.
     */
        line_dist[0] = 0.0;
        dtot = 0.0;
        for (i=1; i<npts; i++) {
            dx = xin[i-1] - xin[i];
            dy = yin[i-1] - yin[i];
            dist = dx * dx + dy * dy;
            dist = Math.sqrt (dist);
            dtot += dist;
            line_dist[i] = dtot;
        }

    /*
     * Allocate space for output arrays.
     */
        nmax = (int)(dtot / avspace + .5);
        if (nmax > 1000 * npts) {
            return null;
        }
        nmax += 3;

        x = new double[nmax];
        y = new double[nmax];

        nout = nmax - 1;

        avspace = dtot / (nout - 1);
        ibase = 0;
        x[0] = xin[0];
        y[0] = yin[0];

        xt = new double[1];
        yt = new double[1];
        zt = new double[1];
        nextbase = new int[1];

    /*
     * Find points at avspace distances along the line and put them into
     * the output arrays.
     */
        for (i=1; i<nout-1; i++) {

            dtot = i * avspace;
            istat = PointAtDistance (line_dist, npts,
                                     xin, yin, zin,
                                     ibase, dtot,
                                     xt, yt, zt,
                                     nextbase);
            if (istat != 1) {
                return null;
            }

            ibase = nextbase[0];

            x[i] = xt[0];
            y[i] = yt[0];

        }

        x[nout-1] = xin[npts-1];
        y[nout-1] = yin[npts-1];

        double[]               xout, yout;

        XYPolyline pout = new XYPolyline (nout);
        xout = pout.getXArrayCopy ();
        yout = pout.getYArrayCopy ();

        System.arraycopy (x, 0, xout, 0, nout);
        System.arraycopy (y, 0, yout, 0, nout);
        pout.setXY(xout, yout);
        return pout;

    }

    /*
     * Find a point at a specified distance along a line.  This is used
     * by the line resampling methods.
     */
    private int PointAtDistance (double[] LineDistance, int Nline,
                                double[] Xline, double[] Yline, double[] Zline,
                                int ibase, double dist,
                                double[] x, double[] y, double[] z,
                                int[] nextibase)

    {
        int           i, i1, i2;
        double        dx, dy, dz, d1, d2, pct;

        i1 = Nline - 2;
        i2 = i1 + 1;
        for (i=ibase; i<Nline; i++) {
            if (dist < LineDistance[i]) {
                i1 = i - 1;
                i2 = i;
                break;
            }
        }

        if (i1 < 0) i1 = 0;

        d1 = LineDistance[i1];
        d2 = LineDistance[i2];

        if (d2 - d1 == 0.0f)
            pct = 0.0f;
        else
            pct = (dist - d1) / (d2 - d1);

        dx = Xline[i2] - Xline[i1];
        dy = Yline[i2] - Yline[i1];
        dz = Zline[i2] - Zline[i1];

        dx *= pct;
        dy *= pct;
        dz *= pct;

        x[0] = Xline[i1] + dx;
        y[0] = Yline[i1] + dy;
        z[0] = Zline[i1] + dz;

        nextibase[0] = i1;

        return 1;

    }  /*  end of PointAtDistance function  */



/*-----------------------------------------------------------------*/

    /**
     * Resample each segment of a line as close as possibly to the specifed average
     * spacing.  The original points are not moved in this method.
     * @param inputLine An {@link XYPolyline} object to be resampled.
     * @param avspace Close approximation to the average spacing on the output line.
     * @return The resampled XYPolyline.
     */
    public XYPolyline resampleLineSegments (
        XYPolyline       inputLine,
        double           avspace)
    {
        double     dx, dy, dist, dtot;
        double[]   x, y;
        double     xt, yt;
        int        i, j, jdo, n, nmax;

    /*
     * Obvious errors.
     */
        if (inputLine == null) {
            return null;
        }
        if (avspace <= 0.0) {
            return null;
        }

        double[]               xin, yin;
        int                    npts;

        xin = inputLine.getXArrayCopy ();
        yin = inputLine.getYArrayCopy ();
        npts = inputLine.size ();

    /*
     * Find the total distance along the line.
     */
        dtot = 0.0;
        for (i=1; i<npts; i++) {
            dx = xin[i-1] - xin[i];
            dy = yin[i-1] - yin[i];
            dist = dx * dx + dy * dy;
            dist = Math.sqrt (dist);
            dtot += dist;
        }

    /*
     * Allocate space for output arrays.
     */
        nmax = (int)(dtot / avspace + .5);
        if (nmax > 1000 * npts) {
            return null;
        }
        nmax += npts;
        nmax *= 2;

        x = new double[nmax];
        y = new double[nmax];

        x[0] = xin[0];
        y[0] = yin[0];

    /*
     * Find points at close to avspace distances along each segment
     * of the line and put them into the output arrays.
     */
        n = 1;
        for (i=1; i<npts; i++) {

            dx = xin[i] - xin[i-1];
            dy = yin[i] - yin[i-1];
            dist = dx * dx + dy * dy;
            dist = Math.sqrt (dist);

            if (dist < avspace * 1.5) {
                x[n] = xin[i];
                y[n] = yin[i];
                n++;
                continue;
            }

            jdo = (int)(dist / avspace) + 1;
            dx /= jdo;
            dy /= jdo;

            for (j=1; j<jdo; j++) {
                xt = xin[i-1] + dx * j;
                yt = yin[i-1] + dy * j;
                x[n] = xt;
                y[n] = yt;
                n++;
            }

            x[n] = xin[i];
            y[n] = yin[i];
            n++;

        }

        double[]    xout, yout;

        XYPolyline pout = new XYPolyline (n);
        xout = pout.getXArrayCopy ();
        yout = pout.getYArrayCopy ();

        System.arraycopy (x, 0, xout, 0, n);
        System.arraycopy (y, 0, yout, 0, n);
        pout.setXY(xout, yout);
        return pout;

    }

/*-----------------------------------------------------------------*/

    /*
     * Resample a line at equally spaced points as close as possible to the specified
     * average spacing.  The first and last points of the line are not changed.  However,
     * points in the middle of the line are moved to the closest resampled location.
     * @param inputLine An {@link XYZPolyline} object to be resampled.
     * @param avspace Close approximation to the average spacing on the output line.
     * @param The resampled XYZPolyline.
     */
    public XYZPolyline resampleLine (
        XYZPolyline     inputLine,
        double          avspace)
    {
        double[]   line_dist;
        double     dx, dy, dz, dist, dtot;
        double[]   x, y, z;
        double[]   xt, yt, zt;
        int        istat, i, nout, nmax, ibase;
        int[]      nextbase;

    /*
     * Obvious errors.
     */
        if (inputLine == null) {
            return null;
        }
        if (avspace <= 0.0) {
            return null;
        }

        double[]               xin, yin, zin;
        int                    npts;

        xin = inputLine.getXArray ();
        yin = inputLine.getYArray ();
        zin = inputLine.getZArray ();
        npts = inputLine.size ();

    /*
     * Allocate space for distance to point arrays.
     */
        line_dist = new double[npts];

    /*
     * Fill in distance along line at each point values.
     */
        line_dist[0] = 0.0;
        dtot = 0.0;
        for (i=1; i<npts; i++) {
            dx = xin[i-1] - xin[i];
            dy = yin[i-1] - yin[i];
            if (zin[i-1] > -1.e19  &&  zin[i] > -1.e19  &&
                zin[i-1] < 1.e19  &&  zin[i] < 1.e19) {
                dz = zin[i-1] - zin[i];
            }
            else {
                dz = 0.0;
            }
            dist = dx * dx + dy * dy + dz * dz;
            dist = Math.sqrt (dist);
            dtot += dist;
            line_dist[i] = dtot;
        }

    /*
     * Allocate space for output arrays.
     */
        nmax = (int)(dtot / avspace + .5);
        if (nmax > 1000 * npts) {
            return null;
        }
        nmax += 3;

        x = new double[nmax];
        y = new double[nmax];
        z = new double[nmax];

        nout = nmax - 1;

        avspace = dtot / (nout - 1);
        ibase = 0;
        x[0] = xin[0];
        y[0] = yin[0];
        z[0] = zin[0];

        xt = new double[1];
        yt = new double[1];
        zt = new double[1];
        nextbase = new int[1];

    /*
     * Find points at avspace distances along the line and put them into
     * the output arrays.
     */
        for (i=1; i<nout-1; i++) {

            dtot = i * avspace;
            istat = PointAtDistance (line_dist, npts,
                                     xin, yin, zin,
                                     ibase, dtot,
                                     xt, yt, zt,
                                     nextbase);
            if (istat != 1) {
                return null;
            }

            ibase = nextbase[0];

            x[i] = xt[0];
            y[i] = yt[0];
            z[i] = zt[0];

        }

        x[nout-1] = xin[npts-1];
        y[nout-1] = yin[npts-1];
        z[nout-1] = zin[npts-1];

        double[]               xout, yout, zout;

        XYZPolyline pout = new XYZPolyline (nout);
        xout = pout.getXArray ();
        yout = pout.getYArray ();
        zout = pout.getZArray ();

        System.arraycopy (x, 0, xout, 0, nout);
        System.arraycopy (y, 0, yout, 0, nout);
        System.arraycopy (z, 0, zout, 0, nout);

        return pout;

    }


/*-----------------------------------------------------------------*/

    /**
     * Resample each segment of a line as close as possibly to the specifed average
     * spacing.  The original points are not moved in this method.
     * @param inputLine An {@link XYZPolyline} object to be resampled.
     * @param avspace Close approximation to the average spacing on the output line.
     * @return The resampled XYZPolyline.
     */
    public XYZPolyline resampleLineSegments (
        XYZPolyline      inputLine,
        double           avspace)
    {
        double     dx, dy, dz, dist, dtot;
        double[]   x, y, z;
        double     xt, yt, zt;
        int        i, j, jdo, n, nmax;

    /*
     * Obvious errors.
     */
        if (inputLine == null) {
            return null;
        }
        if (avspace <= 0.0) {
            return null;
        }

        double[]               xin, yin, zin;
        int                    npts;

        xin = inputLine.getXArray ();
        yin = inputLine.getYArray ();
        zin = inputLine.getZArray ();
        npts = inputLine.size ();

    /*
     * Find the total distance along the line.
     */
        dtot = 0.0;
        for (i=1; i<npts; i++) {
            dx = xin[i-1] - xin[i];
            dy = yin[i-1] - yin[i];
            if (zin[i-1] > -1.e19  &&  zin[i] > -1.e19  &&
                zin[i-1] < 1.e19  &&  zin[i] < 1.e19) {
                dz = zin[i-1] - zin[i];
            }
            else {
                dz = 0.0;
            }
            dist = dx * dx + dy * dy + dz * dz;
            dist = Math.sqrt (dist);
            dtot += dist;
        }

    /*
     * Allocate space for output arrays.
     */
        nmax = (int)(dtot / avspace + .5);
        if (nmax > 1000 * npts) {
            return null;
        }
        nmax += npts;
        nmax *= 2;

        x = new double[nmax];
        y = new double[nmax];
        z = new double[nmax];

        x[0] = xin[0];
        y[0] = yin[0];
        z[0] = zin[0];

    /*
     * Find points at close to avspace distances along each segment
     * of the line and put them into the output arrays.
     */
        n = 1;
        for (i=1; i<npts; i++) {

            dx = xin[i] - xin[i-1];
            dy = yin[i] - yin[i-1];
            if (zin[i-1] > -1.e19  &&  zin[i] > -1.e19  &&
                zin[i-1] < 1.e19  &&  zin[i] < 1.e19) {
                dz = zin[i] - zin[i-1];
            }
            else {
                dz = 0.0;
            }
            dist = dx * dx + dy * dy + dz * dz;
            dist = Math.sqrt (dist);

            if (dist < avspace * 1.5) {
                x[n] = xin[i];
                y[n] = yin[i];
                z[n] = zin[i];
                n++;
                continue;
            }

            jdo = (int)(dist / avspace) + 1;
            dx /= jdo;
            dy /= jdo;
            dz /= jdo;

            for (j=1; j<jdo; j++) {
                xt = xin[i-1] + dx * j;
                yt = yin[i-1] + dy * j;
                if (zin[i-1] > -1.e19  &&  zin[i-1] < 1.e19) {
                    zt = zin[i-1] + dz * j;
                }
                else {
                    zt = zin[i-1];
                }
                x[n] = xt;
                y[n] = yt;
                z[n] = zt;
                n++;
            }

            x[n] = xin[i];
            y[n] = yin[i];
            z[n] = zin[i];
            n++;

        }

        double[]    xout, yout, zout;

        XYZPolyline pout = new XYZPolyline (n);
        xout = pout.getXArray ();
        yout = pout.getYArray ();
        zout = pout.getZArray ();

        System.arraycopy (x, 0, xout, 0, n);
        System.arraycopy (y, 0, yout, 0, n);
        System.arraycopy (z, 0, zout, 0, n);

        return pout;

    }

/*-----------------------------------------------------------*/

    /*
     * This group of methods is used to pair up a TriangleIndex3D object with
     * its C++ "peer" object.
     */

    /**
     Create a new {@link TriangleIndex3D} object along with its C++ "peer"
     object.  The initial bounds of the index are set as specified.
     */
    public TriangleIndex3D
      create3DTriangleIndex (
        double xmin, double ymin, double zmin,
        double xmax, double ymax, double zmax)
    {
        int     id1, id2;
        TriangleIndex3D t3d;

        t3d = TriangleIndex3D.instance();
        id1 = t3d.getID1 ();
        id2 = t3d.getID2 ();

        Dlist[0] = xmin;
        Dlist[1] = ymin;
        Dlist[2] = zmin;
        Dlist[3] = xmax;
        Dlist[4] = ymax;
        Dlist[5] = zmax;
        Ilist[0] = id1;
        Ilist[1] = id2;

        long istat = sendNativeCommand (SW_CREATE_3D_TINDEX,
                                       0,
                                       Ilist,
                                       Dlist,
                                       Ddata);

        if (istat != 1) {
            return null;
        }

        return t3d;

    }


    /*
     * The remainder of the methods are package scope, being
     * used only from methods on the TriangleIndex3D class.
     */

    /*
     * Called from the TriangleIndex3D.setGeometry method.
     */
    int swSet3DTriangleIndexGeometry (
        int       id1,
        int       id2,
        double    xmin,
        double    ymin,
        double    zmin,
        double    xmax,
        double    ymax,
        double    zmax,
        double    xspace,
        double    yspace,
        double    zspace)
    {
        Dlist[0] = xmin;
        Dlist[1] = ymin;
        Dlist[2] = zmin;
        Dlist[3] = xmax;
        Dlist[4] = ymax;
        Dlist[5] = zmax;
        Dlist[6] = xspace;
        Dlist[7] = yspace;
        Dlist[8] = zspace;
        Ilist[0] = id1;
        Ilist[1] = id2;

        long istat = sendNativeCommand (SW_SET_3D_TINDEX_GEOM,
                                       0,
                                       Ilist,
                                       Dlist,
                                       Ddata);

        return (int)istat;

    }

    /*
     * Called from the TriangleIndex3D.addTriMesh method.
     */
    int swAdd3DTriangleIndexTriMesh (
        int       id1,
        int       id2,
        int       tmeshid,
        TriMesh   tmesh)
    {
        int       ntot;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[3] = id1;
        Ilist[4] = id2;
        Ilist[5] = tmeshid;

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int   i;

        ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat =
          sendNativeCommand (SW_ADD_3D_TINDEX_TRIMESH,
                             0,
                             Ilist,
                             null,
                             null,
                             null,
                             Idata,
                             null,
                             Ddata);

        return (int)istat;

    }


    /*
     *  Called from the TriangleIndex3D Clear method.
     */
    void swClear3DTriangleIndex (int id1, int id2)
    {
        Ilist[0] = id1;
        Ilist[1] = id2;

        sendNativeCommand (SW_CLEAR_3D_TINDEX,
                           0,
                           Ilist,
                           Dlist,
                           Ddata);

        return;

    }


/*---------------------------------------------------------------*/

    static ArrayList<FinalizedTmeshID>    finalized3DIndexList = 
        new ArrayList<FinalizedTmeshID> (10);
    static boolean      cleanup3DPending = false;

    /*
     * Called from the TriangleIndex3D finalize method.  Adds the
     * specified id's to a list of 3d index id's to free on the native
     * side and schedules the actual freeing to be done later in the
     * event thread.
     */
    static synchronized void addFinalized3DTriangleIndex (int id1, int id2)
    {

        FinalizedTmeshID tmid = new FinalizedTmeshID ();

        tmid.id1 = id1;
        tmid.id2 = id2;

        finalized3DIndexList.add (tmid);

        if (cleanup3DPending) {
            return;
        }

        Runnable cleanup = new Runnable ()
        {
            public void run ()
            {
                JSurfaceWorks.cleanupFinalized3DList ();
            }
        };

        SwingUtilities.invokeLater (cleanup);

        cleanup3DPending = true;

    }


    private static synchronized void cleanupFinalized3DList ()
    {
        int[]      ilist;
        double[]   ddata;
        FinalizedTmeshID   tmid;

        ilist = new int[2];
        ddata = new double[2];

        int size = finalized3DIndexList.size ();
        int nid = 0;

        for (int i=0; i<size; i++) {
            tmid = finalized3DIndexList.get (i);
            ilist[0] = tmid.id1;
            ilist[1] = tmid.id2;

            sendStaticNativeCommand (SW_DELETE_3D_TINDEX,
                                     //nativeID,
                                     0,
                                     0,
                                     ilist,
                                     ddata);
        }

        cleanup3DPending = false;

        return;

    }


    /*
     * Called from TriangleIndex3D.getTriangles method.
     */
    int swGet3DTriangles (
        int        id1,
        int        id2,
        double     xmin,
        double     ymin,
        double     zmin,
        double     xmax,
        double     ymax,
        double     zmax)

    {
        Ilist[0] = id1;
        Ilist[1] = id2;

        Dlist[0] = xmin;
        Dlist[1] = ymin;
        Dlist[2] = zmin;
        Dlist[3] = xmax;
        Dlist[4] = ymax;
        Dlist[5] = zmax;

        long istat = sendNativeCommand (SW_GET_3D_TRIANGLES,
                                       0,
                                       Ilist,
                                       Dlist,
                                       Ddata);

        return (int)istat;

    }



/*-------------------------------------------------------------*/

/*
 * Methods used to calculate fault connection groups.
 */

/*-----------------------------------------------------------------------*/

  /**
  Start a fault connection definition.
  A new native FaultConnect object is created and used.  If this is
  not called prior to any of the other fault connection methods, the
  other fault connection methods will fail.
  */
    public void psStartFaultConnectGroupDefinition (double avSpace)
    {

        Dlist[0] = avSpace;

        sendNativeCommand (SW_PS_START_FAULT_CONNECT_GROUP,
                           0,
                           Ilist,
                           null,
                           Dlist,
                           null,
                           null,
                           null,
                           null);
    }


/*-----------------------------------------------------------------------*/

  /**
  End a fault connection definition.
  The current native fault connect object is destroyed when this is called.
  */
    public void psEndFaultConnectGroupDefinition ()
    {

        sendNativeCommand (SW_PS_END_FAULT_CONNECT_GROUP,
                           0,
                           Ilist,
                           null);
    }


/*-----------------------------------------------------------------------*/

  /**
   Connect the faults specified by {@link #psAddConnectingFault} with the detachment
   surface defined by {@link #psSetDetachment}.
  */
    public void psConnectFaults ()
    {

        long istat =
        sendNativeCommand (SW_PS_CONNECT_FAULTS,
                           1,
                           Ilist,
                           null);

    /*
     * TODO
     * If istat is -2, the detachment surface or a fault surface is too noisy
     * to seal together.  A message to this affect is needed.
     */
        if (istat == -2) {
            System.out.println
            ("More than 1 intersection line between a fault and detachment.");
        }
    }

/*-----------------------------------------------------------------------*/

  /**
  Add a fault that will be connected to the detachment in the currently open
  fault connection group.  It is up to the calling object to make sure there
  is an open connection group by calling {@link #psStartFaultConnectGroupDefinition}
  before calling this method.
  @param tmesh A {@link TriMesh} object that defines the fault surface.
  @param faultID An integer id tagged onto the connected fault eventually
  output by the fault connecting.
  @param flag An optional flag that isn't really used for anything yet.
  */
    public int psAddConnectingFault (
        TriMesh    tmesh,
        int        faultID,
        int        flag)
    {

        int        i;

        VertBaseline vb = tmesh.getVertBaseline ();

        if (vb == null) {
            vb = vertCalcBaseline (
                tmesh.getNodeXArray (),
                tmesh.getNodeYArray (),
                tmesh.getNodeZArray (),
                tmesh.getNumNodes (),
                true);
        }
        tmesh.vBase = vb;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[3] = faultID;
        Ilist[4] = flag;

        if (vb == null) {
            Ilist[5] = -1;
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
        }
        else {
            Ilist[5] = 0;
            if (vb.used) {
                Ilist[5] = 1;
            }
            Dlist[0] = vb.c1;
            Dlist[1] = vb.c2;
            Dlist[2] = vb.c3;
            Dlist[3] = vb.x0;
            Dlist[4] = vb.y0;
            Dlist[5] = vb.z0;
        }

        if (tmesh.vBase != null) {
            Ilist[5] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[5] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat = sendNativeCommand (SW_PS_ADD_CONNECTING_FAULT,
                                   0,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);

        return (int)istat;
    }

/*-------------------------------------------------------------------------*/

  /**
  Set the detachment surface for a
  fault connection group.  It is up to the calling object to make sure there
  is an open connection group by calling {@link #psStartFaultConnectGroupDefinition}
  before calling this method.
  @param tmesh A {@link TriMesh} object that defines the fault surface.
  @param faultID An integer id tagged onto the connected fault eventually
  output by the fault connecting.
  @param flag An optional flag that isn't really used for anything yet.
  */
    public int psSetDetachment (
        TriMesh    tmesh,
        int        faultID,
        int        flag)
    {

        int        i;

        VertBaseline vb = tmesh.getVertBaseline ();

        if (vb == null) {
            vb = vertCalcBaseline (
                tmesh.getNodeXArray (),
                tmesh.getNodeYArray (),
                tmesh.getNodeZArray (),
                tmesh.getNumNodes (),
                true);
        }
        tmesh.vBase = vb;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[4] = faultID;
        Ilist[5] = flag;

        if (vb == null) {
            Ilist[3] = -1;
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
        }
        else {
            Ilist[3] = 0;
            if (vb.used) {
                Ilist[3] = 1;
            }
            Ilist[4] = faultID;
            Dlist[0] = vb.c1;
            Dlist[1] = vb.c2;
            Dlist[2] = vb.c3;
            Dlist[3] = vb.x0;
            Dlist[4] = vb.y0;
            Dlist[5] = vb.z0;
        }

        if (tmesh.vBase != null) {
            Ilist[3] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[3] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat = sendNativeCommand (SW_PS_SET_DETACHMENT,
                                   0,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);

        return (int)istat;
    }

/*-------------------------------------------------------------------------*/

  /**
  Set the lower trimesh to be used for a surface to surface operation.
  These operations only include {@link #calcDetachment} for now.
  @param tmesh A {@link TriMesh} object that defines the lower surface.
  */
    public int setLowerTriMesh (
        TriMesh    tmesh)
    {

        int        i;

        VertBaseline vb = tmesh.getVertBaseline ();

        if (vb == null) {
            vb = vertCalcBaseline (
                tmesh.getNodeXArray (),
                tmesh.getNodeYArray (),
                tmesh.getNodeZArray (),
                tmesh.getNumNodes (),
                true);
        }
        tmesh.vBase = vb;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();

        if (vb == null) {
            Ilist[3] = -1;
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
        }
        else {
            Ilist[3] = 0;
            if (vb.used) {
                Ilist[3] = 1;
            }
            Dlist[0] = vb.c1;
            Dlist[1] = vb.c2;
            Dlist[2] = vb.c3;
            Dlist[3] = vb.x0;
            Dlist[4] = vb.y0;
            Dlist[5] = vb.z0;
        }

        if (tmesh.vBase != null) {
            Ilist[3] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[3] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat = sendNativeCommand (SW_PS_SET_LOWER_TMESH,
                                   0,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);

        return (int)istat;
    }

/*-------------------------------------------------------------------------*/

  /**
  Set the upper trimesh to be used for a surface to surface operation.
  These operations only include {@link #calcDetachment} for now.
  @param tmesh A {@link TriMesh} object that defines the upper surface.
  */
    public int setUpperTriMesh (
        TriMesh    tmesh)
    {

        int        i;

        VertBaseline vb = tmesh.getVertBaseline ();

        if (vb == null) {
            vb = vertCalcBaseline (
                tmesh.getNodeXArray (),
                tmesh.getNodeYArray (),
                tmesh.getNodeZArray (),
                tmesh.getNumNodes (),
                true);
        }
        tmesh.vBase = vb;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();

        if (vb == null) {
            Ilist[3] = -1;
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
        }
        else {
            if (vb.used) {
                Ilist[3] = 1;
            }
            Dlist[0] = vb.c1;
            Dlist[1] = vb.c2;
            Dlist[2] = vb.c3;
            Dlist[3] = vb.x0;
            Dlist[4] = vb.y0;
            Dlist[5] = vb.z0;
        }

        if (tmesh.vBase != null) {
            Ilist[3] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[3] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat = sendNativeCommand (SW_PS_SET_UPPER_TMESH,
                                   0,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);

        return (int)istat;
    }

/*-------------------------------------------------------------*/

  /**
  Calculate a detachment trimesh between the most recently set lower trimesh
  and upper trimesh.  Use {@link #setLowerTriMesh} and {@link #setUpperTriMesh}
  to set the lower and upper constraining trimeshes respectively.  The detachment
  is a very smooth trimesh that lies between the constraints and does not
  intersect either constraint.
  */
    public int calcDetachment (double lowerAge, double upperAge, double age)
    {

        Dlist[0] = lowerAge;
        Dlist[1] = upperAge;
        Dlist[2] = age;

        long istat = sendNativeCommand (SW_PS_CALC_DETACHMENT,
                                   1,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);
        if (istat == -1) {
            return -1;
        }

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Add to the detachment surface for a
  fault connection group.  It is up to the calling object to make sure there
  is an open connection group by calling {@link #psStartFaultConnectGroupDefinition}
  before calling this method.
  @param tmesh A {@link TriMesh} object that defines the fault surface.
  @param faultID An integer id tagged onto the connected fault eventually
  output by the fault connecting.
  @param flag An optional flag that isn't really used for anything yet.
  */
    public int psAddToDetachment (
        TriMesh    tmesh,
        int        faultID,
        int        flag)
    {

        int        i;

        VertBaseline vb = tmesh.getVertBaseline ();

        if (vb == null) {
            vb = vertCalcBaseline (
                tmesh.getNodeXArray (),
                tmesh.getNodeYArray (),
                tmesh.getNodeZArray (),
                tmesh.getNumNodes (),
                true);
        }
        tmesh.vBase = vb;

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[4] = faultID;
        Ilist[5] = flag;

        if (vb == null) {
            Ilist[3] = -1;
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
        }
        else {
            if (vb.used) {
                Ilist[3] = 1;
            }
            Ilist[4] = faultID;
            Dlist[0] = vb.c1;
            Dlist[1] = vb.c2;
            Dlist[2] = vb.c3;
            Dlist[3] = vb.x0;
            Dlist[4] = vb.y0;
            Dlist[5] = vb.z0;
        }

        if (tmesh.vBase != null) {
            Ilist[3] = 0;
            if (tmesh.vBase.used == true) {
                Ilist[3] = 1;
            }
            Dlist[0] = tmesh.vBase.c1;
            Dlist[1] = tmesh.vBase.c2;
            Dlist[2] = tmesh.vBase.c3;
            Dlist[3] = tmesh.vBase.x0;
            Dlist[4] = tmesh.vBase.y0;
            Dlist[5] = tmesh.vBase.z0;
        }

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 4 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        int ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
        }

        n0 += 4 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        long istat = sendNativeCommand (SW_PS_ADD_TO_DETACHMENT,
                                   0,
                                   Ilist,
                                   null,
                                   Dlist,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);

        return (int)istat;
    }


/*-------------------------------------------------------------------------*/

    private String[]        atNameArray = new String[100];
    //private int             numAtNameArray;

/**
Start the definition of an attribute trimesh.  An attribute trimesh is
an existing trimesh that has attribute values calculated at each of its
nodes.  Subsequent to calling this method, you should use the
h.  Once all data have been setup the {@link #calcAtTriMesh} method
can be used to calculate an {@link AttributeTriMesh} object.
@param tmesh A {@link TriMesh} object that already has its x, y and z
topology calculated.
@param changeTriMesh Set to true if the trimesh should be changed to honor
any polylines set for the attributes.  Set to false to not change the original
xyz trimesh topology.
*/
    public void startAtTriMesh (
      TriMesh        tmesh,
      boolean        changeTriMesh)

    {
        int          i, ntot;
    /*
     * Send the original trimesh and the flag.
     */
        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();
        Ilist[4] = 0;

        //long id = tmesh.getUniqueID();
        //long left, right;
        //left = id >>> 32;
        //right = id << 32;
        //right = right >>> 32;
        //int id1 = (int)left;
        //int id2 = (int)right;
        Ilist[5] = 0;
        if (changeTriMesh) {
            Ilist[5] = 1;
        }

        VertBaseline vbase = tmesh.getVertBaseline ();
        if (vbase == null) {
            Dlist[0] = 1.e30;
            Dlist[1] = 1.e30;
            Dlist[2] = 1.e30;
            Dlist[3] = 1.e30;
            Dlist[4] = 1.e30;
            Dlist[5] = 1.e30;
            Ilist[4] = 0;
        }
        else {
            Dlist[0] = vbase.c1;
            Dlist[1] = vbase.c2;
            Dlist[2] = vbase.c3;
            Dlist[3] = vbase.x0;
            Dlist[4] = vbase.y0;
            Dlist[5] = vbase.z0;
            Ilist[4] = 0;
            if (vbase.used) {
                Ilist[4] = 1;
            }
        }

        newDdata (3 * Ilist[0]);
        newIdata (Ilist[0] + 5 * Ilist[1] + 3 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();

        ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
        }

        int n0;
        n0 = 0;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();
        int[] ef = tmesh.getEdgeFlagArray ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
            Idata[n0+4*ntot+i] = ef[i];
        }

        n0 += 5 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
        }

        nativeAtTriMesh = null;

            sendNativeCommand (SW_START_AT_TRI_MESH,
                               0,
                               Ilist,
                               null,
                               Dlist,
                               null,
                               Idata,
                               null,
                               Ddata);

        atNameArray[0] = null;
        //numAtNameArray = 1;

    }

/*-------------------------------------------------------------------------*/

/**
 Set constraint lines to be used in spreading attributes across a trimesh.
 For example, litology borders might be a set of constraint lines.
 @param lineList An {@link ArrayList}<XYZPolyline> object specifying the lines
 to use as constraints.  If this is set to null, no constraint lines are used.
*/

    public int setAtConstraintLines (ArrayList<XYZPolyline> lineList)
    {

      int          n, ntmp, zflag, ntot, i;
      double[]     xt, yt, zt;

      if (lineList == null) {
        Ilist[0] = 0;
        sendNativeCommand (SW_SET_LINES,
                           0,
                           Ilist,
                           null,
                           null,
                           null,
                           Idata,
                           null,
                           Ddata);
        return 1;
      }

      int numLines = lineList.size ();

      Ilist[0] = numLines;
      newIdata (numLines * 2);
      zflag = 1;

      XYZPolyline la;
      ntot = 0;
      for (i=0; i<numLines; i++) {
        la = lineList.get (i);
        if (la == null) {
          continue;
        }
        ntot += la.size ();
      }

      if (ntot > 1) {
        newDdata (3 * ntot);
        n = 0;
        for (i=0; i<numLines; i++) {
          la = lineList.get (i);
          if (la == null) {
            Idata[i] = 0;
            Idata[numLines+i] = 0;
            continue;
          }
          xt = la.getXArray ();
          yt = la.getYArray ();
          zt = la.getZArray ();
          ntmp = la.size ();

          if (ntmp < 2) ntmp = 0;
          Idata[i] = ntmp;
          Idata[i+numLines] = 0;
          if (ntmp < 1) {
            continue;
          }

          for (int j=0; j<ntmp; j++) {
            Ddata[n] = xt[j];
            Ddata[ntot+n] = yt[j];
            Ddata[ntot*2+n] = zt[j];
            n++;
          }
        }
        Ilist[1] = ntot;
        Ilist[2] = zflag;

        long lstat = sendNativeCommand (
                                   SW_SET_AT_POLYLINE,
                                   0,
                                   Ilist,
                                   null,
                                   null,
                                   null,
                                   Idata,
                                   null,
                                   Ddata);
        if (lstat == -1) {
          return -1;
        }

      }

      return 1;

    }


/*-------------------------------------------------------------------------*/


    /*
    private int addToAtNameArray (String name)
    {
        int        i, same, id;
        String     tstr, str;

        if (name == null) {
            return 0;
        }

        tstr = name.trim ();
        if (tstr.equals ("")) {
            return 0;
        }

        for (i=1; i<numAtNameArray; i++) {
            str = atNameArray[i];
            if (str == null) continue;
            same = str.compareToIgnoreCase (name);
            if (same == 0) {
                return i;
            }
        }

        id = numAtNameArray;
        atNameArray[id] = new String (name);
        numAtNameArray++;

        return id;

    }
    */

/*-------------------------------------------------------------------------*/

/**
 Calculate and return an {@link AttributeTriMesh} object based on the various
 points and constraint lines defined since the most recent call to the
 {@link #startAtTriMesh} method.
*/
    public AttributeTriMesh calcAtTriMesh ()
    {

        long istat = sendNativeCommand (SW_CALC_AT_TRI_MESH,
                                        1,
                                        Ilist,
                                        Ddata);
        if (istat == -1) {
            return null;
        }

        AttributeTriMesh atmesh =
          buildAtTriMesh ();
        return atmesh;
    }


/*----------------------------------------------------------*/

/**
  Convert x,y,z node coordinate arrays accompanied by n1, n2, n3 nodes
  per triangle arrays to a fully topological trimesh.
*/
    public TriMesh convertTriMesh (
        double[]       xn,
        double[]       yn,
        double[]       zn,
        int[]          n1,
        int[]          n2,
        int[]          n3
    )
    {

    /*
     * Make sure the input is valid.
     */
        if (xn == null  ||  yn == null  ||  zn == null) {
            return null;
        }
        if (n1 == null  ||  n2 == null  ||  n3 == null) {
            return null;
        }

        int    l1, l2, l3;

        l1 = xn.length;
        l2 = yn.length;
        l3 = zn.length;
        if (l1 != l2  ||  l1 != l3) {
            return null;
        }
        int num_nodes = l1;

        l1 = n1.length;
        l2 = n2.length;
        l3 = n3.length;
        if (l1 != l2  ||  l1 != l3) {
            return null;
        }
        int num_tris = l1;

        newDdata (3 * num_nodes);
        newIdata (3 * num_tris);

        int i;

        for (i=0; i<num_nodes; i++) {
            Ddata[i] = xn[i];
            Ddata[num_nodes+i] = yn[i];
            Ddata[2*num_nodes+i] = zn[i];
        }
        Ilist[0] = num_nodes;

        for (i=0; i<num_tris; i++) {
            Idata[i] = n1[i];
            Idata[num_tris+i] = n2[i];
            Idata[2*num_tris+i] = n3[i];
        }
        Ilist[1] = num_tris;

    /*
     * Convert the trimesh.  This also calculates a vertical
     * baseline.  To make sure we don't use an old baseline,
     * the baseline is cleared before calling the convert method.
     */
        clearVertBaseline ();
        long istat =
        sendNativeCommand (SW_CONVERT_NODE_TRIMESH,
                           1,
                           Ilist,
                           null,
                           null,
                           null,
                           Idata,
                           null,
                           Ddata);
        if (istat == -1) {
            return null;
        }

    /*
     * Build the trimesh object from the returned nodes,
     * edges and triangles.  This function is in the base
     * class, where the returned data also resides.
     */
        TriMesh tmesh = buildTriMesh ();

    /*
     * Set the steep coordinate info for the trimesh.
     */
        VertBaseline vbl = getVertBaseline ();
        clearVertBaseline ();
        if (vbl == null) {
            vbl = new VertBaseline ();
        }
        tmesh.vBase = vbl;

    /*
     * Set the trimesh external data flag to true.
     */
        tmesh.externalTsurf = true;

        //tmesh.dumpLimits ();

        return tmesh;

    }

/*-------------------------------------------------------------------------*/


  /**
  Calculate draped points on the specified trimesh.  If the trimesh is
  the same as the previous call, the cached trimesh in the native side
  is used.  To clean up the cached data on the native side, specify
  a null tmesh object.
  <p>
  The results are returned as an {@link ArrayList}<XYZPoint> object.  The x and y
  coordinates will be the same as the input points.  The z coordinate will
  be the elevation of the trimesh at the input point.  There may be less
  points returned than were passed to the method.  If a point is not located
  on the trimesh, it is ignored and not returned.
  @param tmesh A {@link TriMesh} object to drape onto.
  @param inputLines A {@link ArrayList}<XYZPoint> object.  The points defined
   will be draped onto the tri mesh.
  */
    public ArrayList<XYZPoint> calcDrapedPointsFromPoints (TriMesh tmesh,
                                ArrayList<XYZPoint> inputPoints)
    {

        XYZPolyline    in_line = new XYZPolyline (inputPoints);
        ArrayList<XYZPolyline> in_lines = 
            new ArrayList<XYZPolyline> (1);
        in_lines.add (in_line);

        ArrayList<XYZPoint> out_list =
            calcDrapedPoints (tmesh, in_lines);

        return out_list;
    }

}  // end of main JSurfaceWorks class



/*---------------------------------------------------------*/
