
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.util.ArrayList;

import csw.jutils.src.XYZPoint;
import csw.jutils.src.XYZPolyline;

import csw.jsurfaceworks.src.JSurfaceWorks;
import csw.jsurfaceworks.src.TriMesh;

import csw.j3d.src.PatchPair;

/**
 This is the common base class for lines that can be displayed
 on the J3DPanel object.
*/
public class J3DLine extends J3DElement {
    public static int POLYLINE_TYPE = 1;
    public static int POLYGON_TYPE = 2;
    protected int   type;

/*
 * members have package scope so they can be directly
 * accessed by the J3DPanel class.
 */
    String          lineName;
    int             lineID;

    boolean         pickable;

    double          lineWidth = 1.5;
    boolean         dashFlag = false;
    boolean         hidden = true;

    int             drawAsPoints = 0;

    Object          userData;

  /*
   * The original values.
   */
    XYZPolyline       originalLine;

    J3DSurfacePatch   drapingSurface;
    ArrayList<XYZPolyline>   drapedLineList;

/**
 * Copy the members of the specified 3d line into this object.
 * This does not create new objects for members of the class.
 * It uses the references only.
 */
    public void shallowCopy (J3DLine line)
    {
        lineName = line.lineName;
        lineID = line.lineID;
        userData = line.userData;
        setGlobalColor(line.getGlobalColor());

        // setCallbackData() sets callbackData, Selectable and visible variables
        //setCallbackData(line.getCallbackData());
        pickable = line.pickable;
        hidden = line.hidden;
        lineWidth = line.lineWidth;
        dashFlag = line.dashFlag;
        originalLine = line.originalLine;
        drapingSurface = line.drapingSurface;
        drapedLineList = line.drapedLineList;

        return;
    }

/**
 Create an empty line object for use in the 3D display.  Default
 values are set for all the
 members.  Unless you fill this with data via the various set calls, the
 line will not be drawn.
*/
    public J3DLine () {
      super();
      init();
    }

    private void init() {
        lineName = null;
        lineID = 0;
        pickable = true;
        markedForDelete = false;

        type = POLYLINE_TYPE;
        hidden = true;

        originalLine = null;

        drapingSurface = null;
        drapedLineList = null;
    }

    public int getType ()
    {
        return type;
    }

    /**
    Set the points for the line.  When this is called, the drawAsPoints flag
    is not changed.
    @param la An {@link XYZPolyline} object with the line points.
    */
    public void setLine (XYZPolyline la)
    {
         originalLine = la;
    }

    /**
     * Set the draped line list.
     * @param list a {@link ArrayList}<XYZPolyline> object with the draped lines.
     */
    public void setDrapedLineList(ArrayList<XYZPolyline> list) {
         drapedLineList = list;
    }

    /**
    Set the points for the line and set the drawAsPoints flag to 1,
    which means draw points only.  This method repopulates the
    originalLine member with the points specified.
    @param plist An {@link ArrayList}<XYZPoint> object with the points to draw.
    */
    public void setPoints (ArrayList<XYZPoint> plist)
    {
        if (plist == null) {
            originalLine = null;
            return;
        }

        int size = plist.size();
        originalLine = new XYZPolyline (size);

        double[] xa = originalLine.getXArray ();
        double[] ya = originalLine.getYArray ();
        double[] za = originalLine.getZArray ();

        int i;
        XYZPoint   p;

        for (i=0; i<size; i++) {
            p = plist.get (i);
            xa[i] = p.getX();
            ya[i] = p.getY();
            za[i] = p.getZ();
        }

        drawAsPoints = 1;

    }


    /**
    Set the points for the line and set the drawAsPoints flag to 1,
    which means draw points only.  This method repopulates the
    originalLine member with the points specified.
    @param x Array with x values.
    @param y Array with y values.
    @param z Array with z values.
    */
    public void setPoints (double[] x, double[] y, double[] z)
    {
        if (x == null  ||  y == null  ||  z == null) {
            originalLine = null;
            return;
        }

        int size = x.length;
        originalLine = new XYZPolyline (size);

        double[] xa = originalLine.getXArray ();
        double[] ya = originalLine.getYArray ();
        double[] za = originalLine.getZArray ();

        int i;

        for (i=0; i<size; i++) {
            xa[i] = x[i];
            ya[i] = y[i];
            za[i] = z[i];
        }

        drawAsPoints = 1;

    }


  /**
  Return the original line points as an {@link ArrayList}<XYZPoint> object.  If
  the drawAsPoints flag is not equal to 1, meaning this line is to be drawn
  as points only, then this method returns null.  This is obly designed for
  returning points where a line is acting as a list of individual points.
  */
    public ArrayList<XYZPoint> getPoints ()
    {
        if (drawAsPoints != 1) {
            return null;
        }

        if (originalLine == null) {
            return null;
        }

        ArrayList<XYZPoint> plist = new ArrayList<XYZPoint> ();

        double[] xa = originalLine.getXArray ();
        double[] ya = originalLine.getYArray ();
        double[] za = originalLine.getZArray ();
        int npts = originalLine.size();

        int i;
        XYZPoint   p;

        for (i=0; i<npts; i++) {
            p = new XYZPoint (xa[i], ya[1], za[i]);
            plist.add (p);
        }

        return plist;

    }

    /**
    Return the original line points as an {@link XYZPolyline}.
    */
    public XYZPolyline getLine ()
    {
         return originalLine;
    }

    /**
    Specify whether the line should participate in hidden line removal.
    Set the flag to true if you wish to participate in hidden line
    removal or to false if you do not want to participate.  This is mostly
    used in debugging, to make sure a line you are checking for validity is
    visible.  If this is not called, true is assumed, and the line will
    participate in hidden line removal.
    */
    public void setHidden (boolean flag)
    {
        hidden = flag;
    }

    /**
    Query whether the line should participate in hidden line removal.
    True is returned if the line should be hidden or false if it should not be hidden.
    */
    public boolean getHidden ()
    {
        return hidden;
    }

    /**
     Specify the surface that this line is draped onto.  The default is null
     or no draping surface.
    @param dsurf A {@link J3DSurfacePatch} object which this line should be
    draped onto.
    */
    public void setDrapingSurface (J3DSurfacePatch dsurf)
    {
        if (dsurf == drapingSurface) {
            return;
        }

        drapingSurface = dsurf;

    }

    /**
    Get the surface currently used for draping this line onto.
    */
    public J3DSurfacePatch getDrapingSurface ()
    {
        return drapingSurface;
    }

    /**
    Return the line as draped over the draping surface.
    */
    public ArrayList<XYZPolyline> getDrapedLineList ()
    {
        return drapedLineList;
    }

    /**
    Return the user data object reference.  If this has not been set,
    via the {@link #setUserData} method,
    null is returned.  Currently user data is used for debugging the
    surface splitting.  A {@link PatchPair} object that records the fault and
    horizon that use the line is set as the user data.
     */
    public Object getUserData() {
        return userData;
    }

    /**
    Set the user data object reference.  Currently user data is used for debugging the
    surface splitting.  A PatchPair object that records the fault and
    horizon that use the line is set as the user data.  The default is null
    if this method is never called.
    @param obj An object that will be returned via {@link #getUserData}.
     */
    public void setUserData(Object obj) {
        userData = obj;
    }

    /**
    Return the id number of the line.  If this has not been set,
    zero is returned.
     */
    public int getLineID() {
        return lineID;
    }

    /**
    Return the name of the line.  If this has not been set,
    null is returned.
     */
    public String getLineName() {
        return lineName;
    }

    /**
    Set a unique id for the line.  By default this is zero.
    This is optional.  The line will still be drawn regardless of the id value.
     * @param i  Optional unique identifier for the line.
     */
    public void setLineID(int i) {
        lineID = i;
    }

    /**
    Set the name for the line.  This is optional, but it is strongly recommended.
    By default this is null.  The line will be drawn regardless of the line name.
     * @param string
     */
    public void setLineName(String string) {
        lineName = string;
    }

    /**
    Set the line width, in pixels on the screen.  The default is 1.5
    pixels.
    */
    public void setLineWidth (double width)
    {
        lineWidth = width;
    }

    /**
    Get the line width, in pixels on the screen.
    */
    public double getLineWidth ()
    {
        return lineWidth;
    }

    /**
    Set the dash line flag to true to draw a dashed line or to false
    to draw a solid line.  The default is a solid line.
    */
    public void setDashFlag (boolean flag)
    {
        dashFlag = flag;
    }

    /**
    Get the dash flag.  True means draw dashed lines.  False
    means draw solid lines.
    */
    public boolean getDashFlag ()
    {
        return dashFlag;
    }

    /**
    Set the drawAsPoints flag.  If set to zero, only connecting lines are drawn.
    If set to 1, only points are drawn.  If set to 2, points and connecting
    lines are drawn.  If the value is not zero, 1 or 2 zero is used.
    */
    public void setDrawAsPoints (int ival)
    {
        if (ival < 0  ||  ival > 2) {
            ival = 0;
        }
        drawAsPoints = ival;
    }

    /**
    Get the drawAsPoints flag.  If this is zero, only connecting lines are drawn.
    If this is 1, only points are drawn.  If this is 2, points and connecting
    lines are drawn.
    */
    public int getDrawAsPoints ()
    {
        return drawAsPoints;
    }

/*--------------------------------------------------------------------------*/

/**
 * After the undraped nodes and the draping surface are set the way you
 * want them, call this to calculate the draped line.  The list of draped
 * lines is returned and it is also stored in this object.  You can
 * retrieve the list by calling the {@link #getDrapedLineList} method.
 */
    public ArrayList<XYZPolyline> calcDrapedLine ()
    {
        if (originalLine == null  ||  drapingSurface == null) {
            drapedLineList = null;
            return null;
        }

        TriMesh tmesh = drapingSurface.triMesh;
        if (tmesh == null) {
            drapedLineList = null;
            return null;
        }

        int nptemp = originalLine.size();

        if (nptemp < 2) {
            drapedLineList = null;
            return null;
        }

        ArrayList<XYZPolyline> lalist = new ArrayList<XYZPolyline> (1);
        lalist.add (originalLine);

        JSurfaceWorks jsw = new JSurfaceWorks ();
        ArrayList<XYZPolyline> dlist =
          jsw.calcDrapedLines (tmesh,
                               lalist);

        if (dlist == null) {
            return null;
        }

        drapedLineList = dlist;

        return dlist;

    }


}  // end of class definition
