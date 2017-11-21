
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

import java.lang.Thread;
import java.util.ArrayList;

import csw.jutils.src.XYZPoint;
import csw.jutils.src.XYZPolyline;

/**
 *
 *  This is the base class used only as the foundation for
 *  the JSurfaceWorks class.  This foundation has the functions needed
 *  to communicate with the native EasyX graphics and also to draw the
 *  simple primitives returned from the EasyX graphics.
 *
 *  This class should not be instantiated by itself.
 *
 *  @author Glenn Pinkerton
 *
 */

public class JSurfaceWorksBase {

    private int    initial_tid;

    JSurfaceWorksBase () {initial_tid = Thread.currentThread().hashCode();};

    protected void finalize() throws Throwable {
      super.finalize();
      //endThread (initial_tid);
    }

  /*
   * Define some constants.
   */
    protected static final int SW_END_THREAD = 6723;
    protected static final int MAX_LIST_ARRAY = 30;

    protected static final int SMALL_CHUNK = 20;
    protected static final int MEDIUM_CHUNK = 100;
    protected static final int BIG_CHUNK = 1000;
    protected static final int HUGE_CHUNK = 10000;

/*--------------------------------------------------------------------------*/

  /*
   * Pure java wraps around the native sendCommand function.
   */

/*--------------------------------------------------------------------------*/

    protected synchronized void endThread (int tid)
    {
/*
        long status =
        sendCommand (
            SW_END_THREAD,
            0,
            null,
            null,
            null,
            null,
            null,
            null,
            null,
            tid);
*/
    }

    protected synchronized void endThread ()
    {
        int    tid = Thread.currentThread().hashCode();

/*
        long status =
        sendCommand (
            SW_END_THREAD,
            0,
            null,
            null,
            null,
            null,
            null,
            null,
            null,
            tid);
*/
    }

/*--------------------------------------------------------------------------*/

  /**
   * Send a command that only needs the input integer list
   */
    protected synchronized long sendNativeCommand (
        int         command_id,
        int         expect_return,
        int[]       ilist
    )
    {
        int    tid = Thread.currentThread().hashCode();

        long status =
        sendCommand (
            command_id,
            expect_return,
            ilist,
            null,
            null,
            null,
            null,
            null,
            null,
            tid
        );

        return status;
    }

/*--------------------------------------------------------------------------*/

  /**
   * Send a command that only needs the input integer list
   * and the double data list.  This is a static version that
   * should only be used when no return data are expected from
   * the command.
   */
    protected static long sendStaticNativeCommand (
        int         command_id,
        int         expect_return,
        int[]       ilist,
        double[]    ddata
    )
    {
        if (expect_return == 1) {
            return -1;
        }

        int    tid = Thread.currentThread().hashCode();

        long status =
        sendStaticCommand (
            command_id,
            expect_return,
            ilist,
            null,
            null,
            null,
            null,
            null,
            ddata,
            tid
        );

        return status;
    }

/*--------------------------------------------------------------------------*/

  /**
   * Send a command that only needs the input integer list
   * and the double data list.
   */
    protected synchronized long sendNativeCommand (
        int         command_id,
        int         expect_return,
        int[]       ilist,
        double[]    ddata
    )
    {
        int    tid = Thread.currentThread().hashCode();

        long status =
        sendCommand (
            command_id,
            expect_return,
            ilist,
            null,
            null,
            null,
            null,
            null,
            ddata,
            tid
        );
        return status;
    }

/*--------------------------------------------------------------------------*/

  /**
   * Send a command that only needs the input integer list
   * and the double data list.
   */
    protected synchronized long sendNativeCommand (
        int         command_id,
        int         expect_return,
        int[]       ilist,
        double[]    dlist,
        double[]    ddata
    )
    {
        int    tid = Thread.currentThread().hashCode();

        long status =
        sendCommand (
            command_id,
            expect_return,
            ilist,
            null,
            dlist,
            null,
            null,
            null,
            ddata,
            tid
        );

        return status;

    }

/*--------------------------------------------------------------------------*/

  /**
   * Send a command that needs all possible parameters.
   */
    protected synchronized long sendNativeCommand (
        int         command_id,
        int         expect_return,
        int[]       ilist,
        long[]      llist,
        double[]    dlist,
        String      cdata,
        int[]       idata,
        float[]     fdata,
        double[]    ddata
    )
    {
        int    tid = Thread.currentThread().hashCode();

        long status =
        sendCommand (
            command_id,
            expect_return,
            ilist,
            llist,
            dlist,
            cdata,
            idata,
            fdata,
            ddata,
            tid
        );

        return status;
    }

/*--------------------------------------------------------------------------*/

  /*
   * Send a command to the native SurfaceWorks command processor.  The command
   * will be executed by the native side and the results are returned via the
   * appropriate java methods.  The method that called this must then
   * take appropriate actions with the output.
   */
    protected native long sendCommand (
        int         command_id,
        int         expect_return,
        int[]       ilist,
        long[]      llist,
        double[]    dlist,
        String      cdata,
        int[]       idata,
        float[]     fdata,
        double[]    ddata,
        int         tid
    );

    protected native static long sendStaticCommand (
        int         command_id,
        int         expect_return,
        int[]       ilist,
        long[]      llist,
        double[]    dlist,
        String      cdata,
        int[]       idata,
        float[]     fdata,
        double[]    ddata,
        int         tid
    );

  /*
   * This is only for dll inclusion purposes.  It is never called.
   */
    public native void bootMe ();



/*--------------------------------------------------------------------------*/

    private TriMesh  nativeTriMesh = null;

/*--------------------------------------------------------------------------*/

  /*
   * Called by the C/C++ side to return a TIN to the Java side.
   *
   * @param xnode an array of node X coordinates
   * @param ynode an array of node Y coordinates
   * @param znode an array of node Z coordinates
   * @param flagnode an array of node flags
   * @param n0edge an array of the edge first node indexes
   * @param n1edge an array of the edge second node indexes
   * @param t0edge an array of the edge first triangle indexes
   * @param t1edge an array of the edge second triangle indexes
   * @param flagedge an array of the edge flags
   * @param e0tri an array of the triangle first edge indexes
   * @param e1tri an array of the triangle second edge indexes
   * @param e2tri an array of the triangle third edge indexes
   * @param flagtri an array of the triangle flags
   */
    public void storeTriMesh (
        double[] xnode,
        double[] ynode,
        double[] znode,
        int[]    flagnode,
        int[]    n0edge,
        int[]    n1edge,
        int[]    t0edge,
        int[]    t1edge,
        int[]    flagedge,
        int[]    e0tri,
        int[]    e1tri,
        int[]    e2tri,
        int[]    flagtri
    ) {
      /* Optionally overridden by subclass. */
    }

  /*
   * Get the returned node, edge and triangle arrays from the currently
   * active calcTriMesh call.
   */
    public void addNativeTriMesh (
        double[] xnode,
        double[] ynode,
        double[] znode,
        int[]    flagnode,
        int      numnode,
        int[]    n0edge,
        int[]    n1edge,
        int[]    t0edge,
        int[]    t1edge,
        int[]    flagedge,
        int      numedge,
        int[]    e0tri,
        int[]    e1tri,
        int[]    e2tri,
        int[]    flagtri,
        int      numtri
    ) {

        if (nativeTriMesh == null) {
            nativeTriMesh = new TriMesh();
        }

        nativeTriMesh.setNodeArrays (xnode, ynode, znode, flagnode, numnode);
        nativeTriMesh.setEdgeArrays (n0edge, n1edge, t0edge, t1edge, flagedge, numedge);
        nativeTriMesh.setTriangleArrays (e0tri, e1tri, e2tri, flagtri, numtri);

        if (currentProtoPatch != null) {
            currentProtoPatch.tmesh = buildTriMesh ();
        }

        return;

    }

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

 /**
  Return the trimesh calculated by the current calculation call.
 */
    protected TriMesh buildTriMesh ()
    {
        TriMesh tmesh = new TriMesh ();
        nativeTriMesh.transfer (tmesh);

        if (vertBaseline == null) {
            tmesh.vBase = null;
        }
        else {
            VertBaseline vb = new VertBaseline ();
            vb.used = vertBaseline.used;
            vb.c1 = vertBaseline.c1;
            vb.c2 = vertBaseline.c2;
            vb.c3 = vertBaseline.c3;
            vb.x0 = vertBaseline.x0;
            vb.y0 = vertBaseline.y0;
            vb.z0 = vertBaseline.z0;
            tmesh.vBase = vb;
        }

        return tmesh;
    }

/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

    protected AttributeTriMesh  nativeAtTriMesh = null;
    private   boolean           activeAtTmesh = false;

/*--------------------------------------------------------------------------*/

  /*
   * Get the returned node, edge and triangle arrays from the currently
   * active calcAtTriMesh call.  The native code will first populate
   * the topology of the trimesh via this call.  Subsequently, multiple
   * calls to add attribute values at the nodes are made via
   * addNativeRDPValue and addNativeNDPValue.  The end of the attribute
   * value addition is signalled by a call to addAtValues with an npts
   * value of zero.
   */
    public void addNativeAtTriMesh (
        double[] xnode,
        double[] ynode,
        double[] znode,
        int[]    flagnode,
        int      numnode,
        int[]    n0edge,
        int[]    n1edge,
        int[]    t0edge,
        int[]    t1edge,
        int[]    flagedge,
        int      numedge,
        int[]    e0tri,
        int[]    e1tri,
        int[]    e2tri,
        int[]    flagtri,
        int      numtri
    ) {

        TriMesh  tmesh;

        tmesh = new TriMesh ();

        tmesh.setNodeArrays (xnode, ynode, znode, flagnode, numnode);
        tmesh.setEdgeArrays (n0edge, n1edge, t0edge, t1edge, flagedge, numedge);
        tmesh.setTriangleArrays (e0tri, e1tri, e2tri, flagtri, numtri);

        nativeAtTriMesh = new AttributeTriMesh(tmesh);

        activeAtTmesh = true;

        return;

    }

/*--------------------------------------------------------------------------*/

    public void addNativeRDPValue (
        int        id,
        double[]   values,
        int        npts)
    {
        if (activeAtTmesh == false  ||
            nativeAtTriMesh == null) {
            return;
        }

        if (npts == 0) {
            activeAtTmesh = false;
            return;
        }

        nativeAtTriMesh.addRDPValue (
            atValueName[id],
            values,
            npts);

    }

/*--------------------------------------------------------------------------*/

    String[] atValueName = new String[100];

    public void addNativeNDPValue (
        int       id,
        byte[]    v1,
        byte[]    v2,
        byte[]    v3,
        byte[]    v4,
        byte[]    p1,
        byte[]    p2,
        byte[]    p3,
        byte[]    p4,
        int       npts)
    {
        if (activeAtTmesh == false  ||
            nativeAtTriMesh == null) {
            return;
        }

        if (npts == 0) {
            activeAtTmesh = false;
            return;
        }

        nativeAtTriMesh.addNDPValue (
            atValueName[id],
            v1, v2, v3, v4,
            p1, p2, p3, p4,
            npts);
    }


/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/

    protected AttributeTriMesh buildAtTriMesh ()
    {
        if (nativeAtTriMesh == null) {
            return null;
        }

        int ntot;
        ntot = nativeAtTriMesh.getNumRDPAttributes ();
        ntot += nativeAtTriMesh.getNumNDPAttributes ();
        if (ntot < 1) {
            return null;
        }

        AttributeTriMesh tmesh = new AttributeTriMesh ();
        nativeAtTriMesh.transfer (tmesh);

        return tmesh;
    }

/*--------------------------------------------------------------------------*/

    private CSWGrid nativeCSWGrid = null;
    protected ArrayList<XYZPolyline> savedFaults = null;

/*--------------------------------------------------------------------------*/

    public void addNativeGrid (
        double[] data,
        double[] errors,
        byte[] mask,
        int ncol,
        int nrow,
        double xmin,
        double ymin,
        double xmax,
        double ymax,
        int npts)
    {
        if (nativeCSWGrid == null) {
            nativeCSWGrid = new CSWGrid ();
        }
        nativeCSWGrid.setNodeMask (mask);
        nativeCSWGrid.setPointErrors (errors);
        nativeCSWGrid.setFaults (savedFaults);
        nativeCSWGrid.setNodeZArray (data);
        nativeCSWGrid.setNCols (ncol);
        nativeCSWGrid.setNRows (nrow);
        nativeCSWGrid.setRotationAngle (0.0);
        nativeCSWGrid.setWidth (xmax - xmin);
        nativeCSWGrid.setHeight (ymax - ymin);
        nativeCSWGrid.setXMin (xmin);
        nativeCSWGrid.setYMin (ymin);

        return;
    }

/*--------------------------------------------------------------------------*/

 /**
  Return the CSWGrid calculated by the current calculation call.
 */
    protected CSWGrid buildCSWGrid ()
    {

        CSWGrid cgrid = new CSWGrid ();
        nativeCSWGrid.transfer (cgrid);
        return cgrid;

    }


/*---------------------------------------------------------------------------*/

    protected ArrayList<XYZPolyline>    drapedLineList = null;

/*--------------------------------------------------------------------------*/

    public void addNativeDrapedLine (
        double[] x,
        double[] y,
        double[] z,
        int npts)
    {
        if (x == null  ||  y == null  ||  z == null) {
            return;
        }
        if (npts < 2) {
            return;
        }

        XYZPolyline la = new XYZPolyline (npts);
        double[] xt = la.getXArray ();
        double[] yt = la.getYArray ();
        double[] zt = la.getZArray ();

        for (int i=0; i<npts; i++) {
            xt[i] = x[i];
            yt[i] = y[i];
            zt[i] = z[i];
        }

        if (drapedLineList == null) {
            drapedLineList = new ArrayList<XYZPolyline>(20);
        }

        drapedLineList.add (la);

        return;
    }

/*--------------------------------------------------------------------------*/

    protected ArrayList<XYZPolyline> buildDrapedLineList ()
    {
        return drapedLineList;
    }

/*--------------------------------------------------------------------------*/

    protected ArrayList<XYZPolyline>    extendedCenterlineList = null;

/*--------------------------------------------------------------------------*/

    public void addExtendedCenterline (
        double[] x,
        double[] y,
        double[] z,
        int npts)
    {
        if (x == null  ||  y == null  ||  z == null) {
            return;
        }
        if (npts < 2) {
            return;
        }

        XYZPolyline la = new XYZPolyline (npts);
        double[] xt = la.getXArray ();
        double[] yt = la.getYArray ();
        double[] zt = la.getZArray ();

        for (int i=0; i<npts; i++) {
            xt[i] = x[i];
            yt[i] = y[i];
            zt[i] = z[i];
        }

        if (extendedCenterlineList == null) {
            extendedCenterlineList = new ArrayList<XYZPolyline> (20);
        }

        extendedCenterlineList.add (la);

        return;
    }

/*--------------------------------------------------------------------------*/

    protected ArrayList<XYZPolyline>    correctedCenterlineList = null;

/*--------------------------------------------------------------------------*/

    public void addCorrectedCenterline (
        double[] x,
        double[] y,
        double[] z,
        int npts)
    {
        if (x == null  ||  y == null  ||  z == null) {
            return;
        }
        if (npts < 2) {
            return;
        }

        XYZPolyline la = new XYZPolyline (npts);
        double[] xt = la.getXArray ();
        double[] yt = la.getYArray ();
        double[] zt = la.getZArray ();

        for (int i=0; i<npts; i++) {
            xt[i] = x[i];
            yt[i] = y[i];
            zt[i] = z[i];
        }

        if (correctedCenterlineList == null) {
            correctedCenterlineList = new ArrayList<XYZPolyline> (20);
        }

        correctedCenterlineList.add (la);

        return;
    }


/*--------------------------------------------------------------------------*/

    protected ArrayList<DBContactLine>    splitLineList = 
		new ArrayList<DBContactLine> (10);

/*--------------------------------------------------------------------------*/

    public void addSplitLine (
        double[] x,
        double[] y,
        double[] z,
        int npts,
        int patchid1,
        int patchid2)
    {
        if (x == null  ||  y == null  ||  z == null) {
            return;
        }
        if (npts < 2) {
            return;
        }

        DBContactLine cl = new DBContactLine (npts);
        double[] xt = cl.getXArray ();
        double[] yt = cl.getYArray ();
        double[] zt = cl.getZArray ();

        for (int i=0; i<npts; i++) {
            xt[i] = x[i];
            yt[i] = y[i];
            zt[i] = z[i];
        }
        cl.npts = npts;
        cl.patchid1 = patchid1;
        cl.patchid2 = patchid2;

        splitLineList.add (cl);

        return;
    }


/*--------------------------------------------------------------------------*/

    protected ArrayList<DBContactLine>    protoPatchContactLineList = 
		new ArrayList<DBContactLine> (10);

/*--------------------------------------------------------------------------*/

    public void addProtoPatchContactLine (
        double[] x,
        double[] y,
        double[] z,
        int npts,
        int patchid1,
        int patchid2)
    {
        if (x == null  ||  y == null  ||  z == null) {
            return;
        }
        if (npts < 2) {
            return;
        }

        DBContactLine cl = new DBContactLine (npts);
        double[] xt = cl.getXArray ();
        double[] yt = cl.getYArray ();
        double[] zt = cl.getZArray ();

        for (int i=0; i<npts; i++) {
            xt[i] = x[i];
            yt[i] = y[i];
            zt[i] = z[i];
        }
        cl.npts = npts;
        cl.patchid1 = patchid1;
        cl.patchid2 = patchid2;

        protoPatchContactLineList.add (cl);

        return;
    }

/*--------------------------------------------------------------------------*/

    protected ArrayList<FaultMajorMinor>    faultMajorMinorList = 
		new ArrayList<FaultMajorMinor> (10);

/*--------------------------------------------------------------------------*/

    public void addFaultMajorMinor (
        int      id,
        int      major)
    {
        FaultMajorMinor    fm = new FaultMajorMinor ();
        fm.id = id;
        fm.major = major;

        faultMajorMinorList.add (fm);

        return;
    }

/*--------------------------------------------------------------------------*/

    protected ArrayList<DBDrapeLine>    drapeLineList = 
		new ArrayList<DBDrapeLine> (10);

/*--------------------------------------------------------------------------*/

    public void addDrapeLine (
        double[] x,
        double[] y,
        double[] z,
        int npts
    )
    {
        if (x == null  ||  y == null  ||  z == null) {
            return;
        }
        if (npts < 2) {
            return;
        }

        DBDrapeLine cl = new DBDrapeLine (npts);
        double[] xt = cl.getXArray ();
        double[] yt = cl.getYArray ();
        double[] zt = cl.getZArray ();

        for (int i=0; i<npts; i++) {
            xt[i] = x[i];
            yt[i] = y[i];
            zt[i] = z[i];
        }

        drapeLineList.add (cl);

        return;
    }

/*--------------------------------------------------------------------------*/

    protected ArrayList<XYZPolyline>    workPolyList = null;

/*--------------------------------------------------------------------------*/

    public void addWorkPoly (
        double[] x,
        double[] y,
        double[] z,
        int npts)
    {
        if (x == null  ||  y == null  ||  z == null) {
            return;
        }
        if (npts < 2) {
            return;
        }

        XYZPolyline la = new XYZPolyline (npts);
        double[] xt = la.getXArray ();
        double[] yt = la.getYArray ();
        double[] zt = la.getZArray ();

        for (int i=0; i<npts; i++) {
            xt[i] = x[i];
            yt[i] = y[i];
            zt[i] = z[i];
        }

        if (workPolyList == null) {
            workPolyList = new ArrayList<XYZPolyline> (20);
        }

        workPolyList.add (la);

        return;
    }

/*--------------------------------------------------------------------------*/

    ArrayList<DBPoint> polyLabelList = null;

/*--------------------------------------------------------------------------*/

    public void addPolyLabel (
        double       x,
        double       y,
        int          label)
    {
        if (polyLabelList == null) {
            polyLabelList = new ArrayList<DBPoint> (20);
        }

        DBPoint p = new DBPoint ();
        p.x = x;
        p.y = y;
        p.ival1 = label;

        polyLabelList.add (p);

        return;
    }

/*--------------------------------------------------------------------------*/
    protected CSWBlendedNodes blendedNodes;

    public void addBlendedNodes(
        int[] values1,
        int[] values2,
        int[] values3,
        int[] values4,
        double[] fractions1,
        double[] fractions2,
        double[] fractions3,
        double[] fractions4
    ) {
        blendedNodes = new CSWBlendedNodes(
            values1,
            values2,
            values3,
            values4,
            fractions1,
            fractions2,
            fractions3,
            fractions4
        );
    }


/*--------------------------------------------------------------------------*/

    VertBaseline   vertBaseline = null;

/*--------------------------------------------------------------------------*/

    public void setVertBaseline (
        double   c1,
        double   c2,
        double   c3,
        double   x0,
        double   y0,
        double   z0,
        int      flag)
    {
        vertBaseline = new VertBaseline ();
        vertBaseline.c1 = c1;
        vertBaseline.c2 = c2;
        vertBaseline.c3 = c3;
        vertBaseline.x0 = x0;
        vertBaseline.y0 = y0;
        vertBaseline.z0 = z0;
        if (flag == 0) {
            vertBaseline.used = false;
        }
        else {
            vertBaseline.used = true;
        }
    }

    VertBaseline getVertBaseline ()
    {
        return vertBaseline;
    }

    protected void clearVertBaseline ()
    {
        vertBaseline = null;
    }

/*--------------------------------------------------------------------------*/

    ArrayList<TriMesh> polyTmeshList = null;
    ArrayList<TriMesh> efaultTmeshList = null;

/*--------------------------------------------------------------------------*/

    public void addPolyTmesh (
        int      polygon_number,
        double[] xnode,
        double[] ynode,
        double[] znode,
        int[]    flagnode,
        int      numnode,
        int[]    n0edge,
        int[]    n1edge,
        int[]    t0edge,
        int[]    t1edge,
        int[]    flagedge,
        int      numedge,
        int[]    e0tri,
        int[]    e1tri,
        int[]    e2tri,
        int[]    flagtri,
        int      numtri)
    {

        TriMesh   tmesh;
        tmesh = new TriMesh();
        tmesh.setNodeArrays (xnode, ynode, znode, flagnode, numnode);
        tmesh.setEdgeArrays (n0edge, n1edge, t0edge, t1edge, flagedge, numedge);
        tmesh.setTriangleArrays (e0tri, e1tri, e2tri, flagtri, numtri);

        if (polygon_number >= 10000) {
            if (efaultTmeshList == null) {
                efaultTmeshList = new ArrayList<TriMesh> (20);
            }
            efaultTmeshList.add (tmesh);
        }
        else {
            tmesh.debug_id1 = polygon_number;
            if (polyTmeshList == null) {
                polyTmeshList = new ArrayList<TriMesh> (20);
            }
            polyTmeshList.add (tmesh);
        }

        return;

    }

/*--------------------------------------------------------------------------*/

    ArrayList<XYZPolyline> getCorrectedCenterlineList ()
    {
        return correctedCenterlineList;
    }

/*--------------------------------------------------------------------------*/

    ArrayList<XYZPolyline> getExtendedCenterlineList ()
    {
        return extendedCenterlineList;
    }

/*--------------------------------------------------------------------------*/

    ArrayList<XYZPolyline> getWorkPolyList ()
    {
        return workPolyList;
    }

/*--------------------------------------------------------------------------*/

    ArrayList<DBPoint> getPolyLabelList ()
    {
        return polyLabelList;
    }


/*--------------------------------------------------------------------------*/

    ArrayList<TriMesh> getPolyTmeshList ()
    {
        return polyTmeshList;
    }

/*--------------------------------------------------------------------------*/

    ArrayList<TriMesh> getEfaultTmeshList ()
    {
        return efaultTmeshList;
    }

/*--------------------------------------------------------------------------*/

    ArrayList<DBDrapeLine> getDrapeLineList ()
    {
        return drapeLineList;
    }

/*--------------------------------------------------------------------------*/

    void clearDebugLists ()
    {
        extendedCenterlineList = null;
        workPolyList = null;
        polyLabelList = null;
        polyTmeshList = null;
        efaultTmeshList = null;
        drapeLineList.clear ();
    }

/*---------------------------------------------------------------------------*/

/*
 * Methods to get proto patch data from the native side and populate
 * ProtoPatch objects.
 */

    private ArrayList<ProtoPatch>    protoPatchList = 
		new ArrayList<ProtoPatch> (10);
    private ProtoPatch   currentProtoPatch = null;

/*----------------------------------------------------------------------------*/

    public ArrayList<ProtoPatch>     getProtoPatchList ()
    {
        return protoPatchList;
    }

/*----------------------------------------------------------------------------*/

    public ArrayList<DBContactLine>     getSplitLineList ()
    {
        return splitLineList;
    }

/*----------------------------------------------------------------------------*/

    public ArrayList<DBContactLine>     getProtoPatchContactLineList ()
    {
        return protoPatchContactLineList;
    }

/*----------------------------------------------------------------------------*/

    public void clearReturnedProtoPatches ()
    {
        protoPatchList.clear ();
    }

/*----------------------------------------------------------------------------*/

    public void clearReturnedProtoPatchContacts ()
    {
        protoPatchContactLineList.clear ();
    }

/*----------------------------------------------------------------------------*/

    public ArrayList<FaultMajorMinor> getFaultMajorMinorList ()
    {
        return faultMajorMinorList;
    }

/*--------------------------------------------------------------------------*/

    void startProtoPatch (int id)
    {
        currentProtoPatch = new ProtoPatch ();
        currentProtoPatch.patchid = id;
        return;
    }

/*----------------------------------------------------------------------------*/

    void endProtoPatch (int id)
    {
        if (currentProtoPatch != null) {
            currentProtoPatch.patchid = id;
            protoPatchList.add (currentProtoPatch);
        }
        currentProtoPatch = null;
    }

/*----------------------------------------------------------------------------*/

    void copyPatchPoints (
        int      patchid,
        double[] x,
        double[] y,
        double[] z,
        int      n)
    {
        if (currentProtoPatch == null) {
            return;
        }

        if (n < 1) {
            return;
        }

        currentProtoPatch.copyPoints (
            x, y, z, n);
        currentProtoPatch.patchid = patchid;

        return;

    }

/*----------------------------------------------------------------------------*/

    void addBorderSegment (
        double[] x,
        double[] y,
        double[] z,
        int      npts,
        int      type,
        int      direction)
    {

        if (currentProtoPatch == null) {
            return;
        }

        if (npts < 1) {
            return;
        }

        currentProtoPatch.addBorderSegment (
            x, y, z, npts, type, direction);

        return;
    }

/*----------------------------------------------------------------------------*/

    void addPatchLine (
        double[] x,
        double[] y,
        double[] z,
        int      npts,
        int      flag)
    {

        if (currentProtoPatch == null) {
            return;
        }

        if (npts < 1) {
            return;
        }

        currentProtoPatch.addPatchLine (
            x, y, z, npts, flag);

        return;
    }


/*----------------------------------------------------------------------------*/

    JSResult2D     outlineResult = null;
    JSResult3D     outlineResult3D = null;

/*----------------------------------------------------------------------------*/

    void setOutline (
        double[] x,
        double[] y,
        int      npts)
    {

        if (npts < 3) {
            return;
        }

        outlineResult = new JSResult2D ();

        double[] xa = new double[npts];
        double[] ya = new double[npts];
        System.arraycopy (x, 0, xa, 0, npts);
        System.arraycopy (y, 0, ya, 0, npts);

        outlineResult.x = xa;
        outlineResult.y = ya;
        outlineResult.npts = npts;

        return;
    }

    protected JSResult2D getOutlineResult ()
    {
        return outlineResult;
    }

/*----------------------------------------------------------------------------*/

    void setOutline (
        double[] x,
        double[] y,
        double[] z,
        int      npts)
    {

        if (npts < 3) {
            return;
        }

        outlineResult3D = new JSResult3D ();

        double[] xa = new double[npts];
        double[] ya = new double[npts];
        double[] za = new double[npts];
        System.arraycopy (x, 0, xa, 0, npts);
        System.arraycopy (y, 0, ya, 0, npts);
        System.arraycopy (z, 0, za, 0, npts);

        outlineResult3D.x = xa;
        outlineResult3D.y = ya;
        outlineResult3D.z = za;
        outlineResult3D.npts = npts;

        return;
    }

    protected JSResult3D getOutlineResult3D ()
    {
        return outlineResult3D;
    }

/*---------------------------------------------------------------------------*/

    protected ArrayList<XYZPoint>    drapedPointList = null;

/*--------------------------------------------------------------------------*/

    public void addNativeDrapedPoints (
        double[] x,
        double[] y,
        double[] z,
        int npts)
    {
        if (x == null  ||  y == null  ||  z == null) {
            return;
        }
        if (npts < 1) {
            return;
        }

        if (drapedPointList == null) {
            drapedPointList = new ArrayList<XYZPoint>();
        }

        double    xt, yt, zt;
        XYZPoint    xp;
        for (int i=0; i<npts; i++) {
            xt = x[i];
            yt = y[i];
            zt = z[i];
            xp = new XYZPoint (xt, yt, zt);
            drapedPointList.add (xp);
        }

        return;
    }

/*--------------------------------------------------------------------------*/

    protected ArrayList<XYZPoint> buildDrapedPointList ()
    {
        return drapedPointList;
    }

/*----------------------------------------------------------------------------*/

    TriangleIndex3DResultList    tindexResult;

    protected TriangleIndex3DResultList getTriangleIndex3DResultList ()
    {
        return tindexResult;
    }

    public void addNativeTindexResult (
        int[] tmeshid,
        int[] trinum,
        int   nresult)
    {
        tindexResult = new TriangleIndex3DResultList (nresult);

        for (int i=0; i<nresult; i++) {
            tindexResult.addTriangle (tmeshid[i], trinum[i]);
        }

        return;
    }



}  // end of JsurfaceWorksBase class
