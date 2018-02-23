
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.lang.Thread;

import java.awt.Color;
import java.awt.Font;
import java.awt.Toolkit;
import java.lang.Math;
import java.util.ArrayList;
import java.util.Iterator;

import javax.swing.SwingUtilities;


//import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;


import csw.jutils.src.XYZPolyline;
import csw.jutils.src.ColorPalette;
import csw.jutils.src.CSWLogger;

import csw.jsurfaceworks.src.Grid;
import csw.jsurfaceworks.src.TriMesh;


/**
  This class has the API functions to draw to and otherwise
  interact with and manage the 2D graphics display list.  This
  class is meant to be a collection of services to draw and
  select to and from the screen.  The display list itself cannot
  be extended by an application.

  @author Glenn Pinkerton

*/
public class JDisplayList extends JDisplayListBase {

    private static Logger logger = CSWLogger.getMyLogger ();

 /*
  * Static methods.
  */

/*--------------------------------------------------------------------*/


 /**
   Make the default constructor not accessible other than by the
   package.  That way the application cannot get a display list
   other than through its accompanying JDisplayListPanel object.
  */
    JDisplayList (JDisplayListPanel dlp)
    {
        if (dlp == null) {
            String emsg =
               new String
                 ("The panel must be specified when constructing a JDisplayList object");
            logger.error (emsg);
            throw new IllegalArgumentException (emsg);
        }
        dlPanel = dlp;

        logger.info ("    JDisplayList constructor succeeded.    ");

        setFont ("arial", Font.PLAIN);
    }

    private String dlist_name = null;
    private double dlist_xminHint = 0.0;
    private double dlist_yminHint = 0.0;
    private double dlist_xmaxHint = 0.0;
    private double dlist_ymaxHint = 0.0;

    private int pageUnitsType = 1;

 /*
  * The collection of API methods used to draw to and otherwise manage the
  * display list follow.  The names are based on the EasyX function names
  * as documented in the EasyX reference manual.  Function names have
  * the gtx_ removed, but the calling parameters are identical to the C
  * syntax described in that manual.  From Java, anything referred to as
  * CSW_Float in the reference should be double.
  *
  * Only a portion of the functions are supplied here.  If more prove
  * useful they can be added later.
  *
  * These functions boil down to sending Command calls to the native
  * side.  The arrays used in the Command function are allocated here
  * for use in all the API methods.  All except the String data are
  * declared here.  Strings are created as needed by the API methods.
  *
  * The initial sizes for these arrays are compromises.  They are large
  * enough to hold the data for many graphics tasks, and they are grown
  * if they need to be larger.  Each API function should call the
  * appropriate new_ method to grow the data array if needed.
  *
  * The Ilist array is used for counters into data, constants, etc.  It
  * never gets very large, so it is set to MAX_LIST_SIZE and not grown from there.
  * The Llist array is use for long values in the same fashion as Ilist
  * is used for int values.
  */
    int[]        Ilist = new int[MAX_LIST_SIZE];
    long[]       Llist = new long[MAX_LIST_SIZE];
    boolean[]    Bdata = new boolean[10000];
    int          BdataMax = 10000;
    short[]      Sdata = new short[10000];
    int          SdataMax = 10000;
    int[]        Idata = new int[10000];
    int          IdataMax = 10000;
    float[]      Fdata = new float[10000];
    int          FdataMax = 10000;
    double[]     Ddata = new double[10000];
    int          DdataMax = 10000;

    private void new_bdata (int size)
    {
        int     istat = 0;
        while (size > BdataMax) {
            BdataMax +=  HUGE_CHUNK;
            istat = 1;
        }
        if (istat == 1) {
            Bdata = new boolean[BdataMax];
        }
    }

    private void new_sdata (int size)
    {
        int     istat = 0;
        while (size > SdataMax) {
            SdataMax +=  HUGE_CHUNK;
            istat = 1;
        }
        if (istat == 1) {
            Sdata = new short[SdataMax];
        }
    }

    private void new_idata (int size)
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

    private void new_fdata (int size)
    {
        int     istat = 0;
        while (size > FdataMax) {
            FdataMax +=  HUGE_CHUNK;
            istat = 1;
        }
        if (istat == 1) {
            Fdata = new float[FdataMax];
        }
    }

    private void new_ddata (int size)
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




/*--------------------------------------------------------------*/

    private static ArrayList<Integer>      finalizedIDList = 
		new ArrayList<Integer> ();
    private static boolean        cleanupNeeded = false;

/**
 * Cleanup the native display list.  This overrides the
 * default object finalize method and deletes the native
 * display list object, if that object has not already been
 * deleted.  The native display list cannot be cleaned up directly
 * from the finalize method because finalize is not in the event
 * thread.  The cleanup is scheduled for the event thread to do
 * later.
 */
    protected void finalize () {

        Integer i = new Integer (nativeDlistID);

        finalizedIDList.add (i);

        if (cleanupNeeded) {
            return;
        }

        cleanupNeeded = true;

  /*
   * Schedule the actual cleanup to happen from the event dispatch
   * thread after the current events have been processed.
   */
        Runnable cleanup = new Runnable ()
        {
            public void run ()
            {
                JDisplayList.cleanupFinalizedDisplayList ();
            }
        };

        SwingUtilities.invokeLater (cleanup);

    }



  /*
   * This method calls native code to free all the memory associated with
   * the finalized trimesh objects currently on the list.  This must complete
   * without thread switching, so it is declared synchronized.
   */
    static private synchronized void cleanupFinalizedDisplayList ()
    {

        int      i, size;
        int[]    ilist = new int [1];

        Integer  iobj;

        size = finalizedIDList.size ();
        for (i=0; i<size; i++) {
            iobj = finalizedIDList.get (i);
            ilist[0] = iobj.intValue();
            sendStaticNativeCommand (
                ilist[0],
                GTX_DELETEWINDOW,
                ilist
            );
        }

        cleanupNeeded = false;

        finalizedIDList.clear ();

    }


/*-------------------------------------------------------------*/


/*-------------------------------------------------------------*/

  /**
    Add an elliptical arc to the display list.  The arc will be filled with the current
    fill color and fill pattern.  The outline of the arc will be colored with the
    current line color and dashed with the current line dash pattern.  Remember that
    x and y coordinates need to be specified in the appropriate frame or page units.
    The r1 and r2 radii should be in the same units as xc and yc.
    @param xc the x coordinate of the center of the arc
    @param yc the y coordinate of the center of the arc
    @param r1 the x axis radius of the arc (before rotation)
    @param r2 the y axix radius of the arc (before rotation)
    @param ang1 the start angle for the arc, in degrees.
                This is measured counterclockwise from the positive x axis.
    @param anglen the angular length of the arc, in degrees
    @param rang the rotation angle in degrees counterclockwise from + x
    @param closure The closure type for the arc:
    <ul>
    <li> {@link #OPEN}
    <li> {@link #PIE}
    <li> {@link #CHORD}
    </ul>
   */
    public int addArc (double xc, double yc,
                       double r1, double r2,
                       double ang1, double anglen,
                       double rang, int closure)
    {
        Ilist[0] = GTX_ARC_SHAPE;
        Ddata[0] = xc;
        Ddata[1] = yc;
        Ddata[2] = r1;
        Ddata[3] = r2;
        Ddata[4] = ang1;
        Ddata[5] = anglen;
        Ddata[6] = rang;
        Ddata[7] = (double)closure;

        sendNativeCommand (
            GTX_DRAWSHAPE,
            Ilist,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
    Set the type of page units to use for this display list.  The
    current choices are PAGE_UNITS_DEFAULT (1) or
    PAGE_UNITS_ARE_SCREEN_UNITS (2).  The default set upon
    construction of a new display list is PAGE_UNITS_DEFAULT.  You must
    call this method prior to calling the  {link #beginPlot} method.
    If you call this after beginPlot, an {@link IllegalStateException} is
    thrown.
    <p>
    If screen units are used, the application needs to handle resize of
    the window being used by the display list.  The display list will
    not rescale to fit into a new window size as it does when the default
    page units are used.

    @param pageUnitsType   Either PAGE_UNITS_DEFAULT (1) or
                           PAGE_UNITS_ARE_SCREEN_UNITS (2).
  */
    public void setPageUnitsType (int pageUnitsType)
    {
        if (beginPlotCalled) {
            throw
            new IllegalStateException
            ("A page units type change is attemted after calling beginPlot.");
        }
        this.pageUnitsType = pageUnitsType;
    }

    int getPageUnitsType ()
    {
        return pageUnitsType;
    }

    private boolean beginPlotCalled = false;

/*-------------------------------------------------------------*/

  /**
    Initialize a display list for drawing.  The display list should be
    named and optionally you can provide a number to identify the display
    list.  If you specify zero as the number, a unique number is generated
    for the display list.  The name and number are used by the native code
    to lookup the correct native display list associated with the Java
    display list.  If you specify a name and number already in use, the
    currently existing display list will be opened for drawing and the
    hints will be ignored.
    <p>
    The corner point hints should be slightly larger than the largest page
    size you think you might need.  These hints are used to setup some
    resources for the display list.  Remember that these hints are in page
    units, not in world units.
    @param name The name for the new display list
    @param page_xminHint Approximate x of lower left corner
    @param page_yminHint Approximate y of lower left corner
    @param page_xmaxHint Approximate x of upper right corner
    @param page_ymaxHint Approximate y of upper right corner
   */
    public int beginPlot (String name,
                          double page_xminHint,
                          double page_yminHint,
                          double page_xmaxHint,
                          double page_ymaxHint)
    {

        if (beginPlotCalled) {
            return 1;
        }

        beginPlotCalled = true;

        String name_hash = name + dlPanel.hashCode();

    /*
     * get the screen resolution and set it in the native
     * display list.
     */
        Toolkit tk = Toolkit.getDefaultToolkit ();
        Ilist[0] = tk.getScreenResolution ();
        Ilist[1] = pageUnitsType;

        int number = hashCode ();
        Llist[0] = (long)number;
        Ddata[0] = page_xminHint;
        Ddata[1] = page_yminHint;
        Ddata[2] = page_xmaxHint;
        Ddata[3] = page_ymaxHint;

        nativeDlistID =
        sendNativeCommand (
            GTX_CREATEWINDOW,
            Ilist,
            Llist,
            name_hash,
            Ddata
        );

        if (nativeDlistID >= 0) {
            dlist_name = name;
            dlist_xminHint = page_xminHint;
            dlist_yminHint = page_yminHint;
            dlist_xmaxHint = page_xmaxHint;
            dlist_ymaxHint = page_ymaxHint;
            registerFontMethods ();
        }

        return nativeDlistID;
    }

/*-------------------------------------------------------------*/

    boolean fromClear = false;
  /**
    Remove all information from the display list.  This method clears all
    primitives from the display list and resets all the graphic attributes
    to their default values.  If application code needs to redraw based on
    changes in data, changes in properties, etc, it may be best to just clear
    the current drawing with this method and then redraw from scratch.
    <p>
    The frame clip flag, set via the {@link #setFrameClip} method is kept at
    its previous value.  You do not have to reset it after a clear.
   */
    public int clear ()
    {

        if (nativeDlistID < 0) {
            return 1;
        }

      /*
       * This is implemented by deleting the native side display list,
       * followed by creating a new native side display list with the same
       * name and number and limit hints.
       */
        fromClear = true;
        cleanup ();
        beginPlot (dlist_name,
                   dlist_xminHint,
                   dlist_yminHint,
                   dlist_xmaxHint,
                   dlist_ymaxHint);
        fromClear = false;

        setFrameClip (currentFrameClipFlag);

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
    Add a retangle to the display list, positioned at a corner
    of the rectangle.  The width and height can be positive or
    negative to effectively change the x and y position to any of the
    four corners.  The width and height must be in page or frame distance units,
    not in device size units.
    If the corner radius is less than or equal to zero, right angle corners are used.
    @param x X coordinate of the corner point
    @param y Y coordinate of the corner point
    @param width Width of the rectangle, in the same units as x and y
    @param height Height of the rectangle, in the same units as x and y
    @param crad The radius for a curved corner, in the same units as x and y
    @param rang The rotation angle in degrees.
   */
    public int addRectangle (double x, double y,
                             double width, double height,
                             double crad, double rang)
    {
        if (crad < 0.0) {
            crad = 0.0;
        }

        Ilist[0] = -GTX_BOX_SHAPE;
        Ddata[0] = x;
        Ddata[1] = y;
        Ddata[2] = width;
        Ddata[3] = height;
        Ddata[4] = crad;
        Ddata[5] = rang;

        sendNativeCommand (
            GTX_DRAWSHAPE,
            Ilist,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
    Add a retangle to the display list, positioned at the center
    of the rectangle.  The width and height must both be positive
    for this method.  If either is negative, its absolute value is used.
    If the corner radius is less than or wqual to zero, right angle corners are used.
    @param x X coordinate of the corner point
    @param y Y coordinate of the corner point
    @param width Width of the rectangle, in the same units as x and y
    @param height Height of the rectangle, in the same units as x and y
    @param crad The radius for a curved corner, in the same units. as x and y
    @param rang The rotation angle in degrees.
   */
    public int addCenteredRectangle (double x, double y,
                                     double width, double height,
                                     double crad, double rang)
    {
        if (crad < 0.0) {
            crad = 0.0;
        }
        if (width < 0.0) {
            width = -width;
        }
        if (height < 0.0) {
            height = -height;
        }

        Ilist[0] = GTX_BOX_SHAPE;
        Ddata[0] = x;
        Ddata[1] = y;
        Ddata[2] = width;
        Ddata[3] = height;
        Ddata[4] = crad;
        Ddata[5] = rang;

        sendNativeCommand (
            GTX_DRAWSHAPE,
            Ilist,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
    Add a circle to the display list by specifying its center and radius.
    The radius must be in page or frame distance units, not in device size units.
   @param xc X coordinate of the circle center
   @param yc Y coordinate of the circle center.
   @param radius radius of the circle, in the same units as xc and yc.
   */
    public int addCircle (double xc, double yc, double radius)
    {
        Ilist[0] = GTX_ARC_SHAPE;
        Ddata[0] = xc;
        Ddata[1] = yc;
        Ddata[2] = radius;
        Ddata[3] = radius;
        Ddata[4] = 0.0;
        Ddata[5] = 360.0;
        Ddata[6] = 0.0;
        Ddata[7] = 0;

        sendNativeCommand (
            GTX_DRAWSHAPE,
            Ilist,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
     Add a circle to the display list by specifying its center
     and a point on its circumference.
  @param xc X coordinate of the circle center
  @param yc Y coordinate of the circle center
  @param xp X coordinate of a point on the circumference
  @param yp Y coordinate of a point on the circumference
   */
    public int addCircle (double xc, double yc, double xp, double yp)
    {
        double     dx, dy, radius;

        dx = xp - xc;
        dy = yp - yc;
        radius = dx * dx + dy * dy;
        radius = Math.sqrt (radius);

        Ilist[0] = GTX_ARC_SHAPE;
        Ddata[0] = xc;
        Ddata[1] = yc;
        Ddata[2] = radius;
        Ddata[3] = radius;
        Ddata[4] = 0.0;
        Ddata[5] = 360.0;
        Ddata[6] = 0.0;
        Ddata[7] = 0;

        sendNativeCommand (
            GTX_DRAWSHAPE,
            Ilist,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
     Add a circle to the display list by specifying 3 points that
     lie on its circumference.  If the points are colinear, no
     circle is added.
  @param x1 X coordinate of point 1 on the circumference
  @param y1 Y coordinate of point 1 on the circumference
  @param x2 X coordinate of point 2 on the circumference
  @param y2 Y coordinate of point 2 on the circumference
  @param x3 X coordinate of point 3 on the circumference
  @param y3 Y coordinate of point 3 on the circumference
   */
    public int addCircle (double x1, double y1,
                        double x2, double y2,
                        double x3, double y3)
    {
        Ilist[0] = GTX_CIRCLE3_SHAPE;
        Ddata[0] = x1;
        Ddata[1] = y1;
        Ddata[2] = x2;
        Ddata[3] = y2;
        Ddata[4] = x3;
        Ddata[5] = y3;

        sendNativeCommand (
            GTX_DRAWSHAPE,
            Ilist,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
    Set an opaque background color.
    This color is used for the background of all drawing.  By default
    the background is a very light gray.  If invalid color components are
    specified, then the default background is used.
  @param red Red component of the background color (0 - 255)
  @param green Green component of the background color (0 - 255)
  @param blue Blue component of the background color (0 - 255)
   */
    public void setBackgroundColor (int red, int green, int blue)
    {
      setBackgroundColor(
        red,
        green,
        blue,
        255
      );
    }

/*-------------------------------------------------------------*/

  /**
    Set the background color.
    This color is used for the background of all drawing.  By default
    the background is a very light gray.  If invalid color components are
    specified, then the default background is used.
  @param red Red component of the background color (0 - 255)
  @param green Green component of the background color (0 - 255)
  @param blue Blue component of the background color (0 - 255)
  @param alpha Alpha component of the background color (0 - 255)
   */
    public void setBackgroundColor (int red, int green, int blue, int alpha)
    {
        if (red < 0  ||  red > 255  ||
            green < 0  ||  green > 255  ||
            blue < 0  ||  blue > 255  ||
            alpha < 0  ||  alpha > 255) {
            backgroundRed = 230;
            backgroundGreen = 230;
            backgroundBlue = 230;
            backgroundAlpha = 255;
        }
        else {
            backgroundRed = red;
            backgroundGreen = green;
            backgroundBlue = blue;
            backgroundAlpha = alpha;
        }

        return;
    }

/*-------------------------------------------------------------*/

  /**
    Set an opaque or transparent background color.
    This color is used for the background of all drawing.  By default
    the background is an opaque very light gray.  The red, green, blue
    and alpha data from the bgcolor object are used.  If a null color is
    specified, a slightly gray (near white) background is used.
  @param bgcolor Color object for the background
   */
    public void setBackgroundColor (Color bgcolor)
    {
      if (bgcolor == null) {
        setBackgroundColor(
          230, 230, 230, 255
        );
      }
      else {
        setBackgroundColor(
          bgcolor.getRed(),
          bgcolor.getGreen(),
          bgcolor.getBlue(),
          bgcolor.getAlpha()
        );
      }
    }

/*-------------------------------------------------------------*/

  /**
    Set the default foreground color, with an opaque alpha value.
     The color specified here will be used for non filled parts of
     primitives if any of the primitive's color components are
     not valid.
   <p>
   For example, if you use setLineColor(-1, -1, -1) then the subsequent
   line primitives will use the foreground color specified here.  If
   this foreground color is also invalid, then the line will not be
   drawn.  By default, the foreground color is very close to black.
  @param red Red component of the foreground color (0 - 255)
  @param green Green component of the foreground color (0 - 255)
  @param blue Blue component of the foreground color (0 - 255)
   */
    public int setForegroundColor (int red, int green, int blue)
    {
      return(
        setForegroundColor(
          red,
          green,
          blue,
          255
        )
      );
    }

/*-------------------------------------------------------------*/

  /**
    Set the default foreground color, including the alpha value.
     The color specified here will be used for non filled parts of
     primitives if any of the primitive's color components are
     not valid.
   <p>
   For example, if you use setLineColor(-1, -1, -1) then the subsequent
   line primitives will use the foreground color specified here.  If
   this foreground color is also invalid, then the line will not be
   drawn.  By default, the foreground color is very close to black.
  @param red Red component of the foreground color (0 - 255)
  @param green Green component of the foreground color (0 - 255)
  @param blue Blue component of the foreground color (0 - 255)
  @param alpha Alpha component of the foreground color (0 - 255)
   */
    public int setForegroundColor (int red, int green, int blue, int alpha)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = alpha;

        sendNativeCommand (
            GTX_SETFGCOLOR,
            Ilist,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
    Set the default foreground color, including the alpha value.
     The color specified here will be used for non filled parts of
     primitives if any of the primitive's color components are
     not valid.  If a null color is specified, the foreground is set
     to opaque black.
   */
    public int setForegroundColor (Color fgcolor)
    {
      int istat;
      if (fgcolor == null) {
        istat = setForegroundColor (0, 0, 0, 255);
      }
      else {
        istat =
        setForegroundColor(
          fgcolor.getRed(),
          fgcolor.getGreen(),
          fgcolor.getBlue(),
          fgcolor.getAlpha()
        );
      }
      return istat;
    }

/*-------------------------------------------------------------*/

  /**
     Specify an opaque color to use for all subsequent display list objects.
     This affects lines, fills, fill patterns, text, symbols etc.
     The color set here does not affect the background of the drawing.
     If any color coordinate is set to less than 0 or to greater than
     255, the subsequent non fill primitives will use the foreground color
     and subsequent fill primitives will not have any solid color fill.
  @param red Red coordinate of the color (0 - 255)
  @param green Green coordinate of the color (0 - 255)
  @param blue Blue coordinate of the color (0 - 255)
   */
    public int setColor (int red, int green, int blue)
    {
      return(setColor(red, green, blue, 255));
    }

/*-------------------------------------------------------------*/

  /**
     Specify an opaque or transparent color to use for all subsequent display list objects.
     This affects lines, fills, fill patterns, text, symbols etc.
     The color set here does not affect the background of the drawing.
     If any color coordinate is set to less than 0 or to greater than
     255, the subsequent non fill primitives will use the foreground color
     and subsequent fill primitives will not have any solid color fill.
  @param red Red coordinate of the color (0 - 255)
  @param green Green coordinate of the color (0 - 255)
  @param blue Blue coordinate of the color (0 - 255)
  @param alpha Alpha coordinate of the color (0 - 255)
   */
    public int setColor (int red, int green, int blue, int alpha)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = alpha;
        Ilist[4] = GTX_SET_ALL_COLORS_FLAG;

        saveAllColors (red, green, blue, alpha);

        int  status =
        sendNativeCommand(
          GTX_SETCOLOR,
          Ilist,
          Ddata
        );

        return status;
    }

/*-------------------------------------------------------------*/

  /**
     Specify an opaque or transparent color to use for all subsequent display list objects.
     This affects lines, fills, fill patterns, text, symbols etc.
     The color set here does not affect the background of the drawing.
     If a null color is specified, the subsequent non fill primitives will use the foreground color
     and subsequent fill primitives will not have any solid color fill.
  @param color Filled in color object to use for primitive color.
   */
    public int setColor (Color color)
    {
      int       istat;

      if (color == null) {
        istat = setColor (-1, -1, -1, 255);
      }
      else {
        istat =
        setColor(
          color.getRed(),
          color.getGreen(),
          color.getBlue(),
          color.getAlpha()
        );
      }
      return istat;
    }

/*-------------------------------------------------------------*/

  /**
    Specify the minimum gap between attached frames.  The gap value
    is in device size units.  If the value is less than zero, it
    will be changed to zero.  Frames that are not attached can be
    located anywhere and do not conform to this minimum gap.
    <p>
    The default minimum gap, which is used if this method is never
    called, is .1 inches.
  @param gap_value The minimum gap, in device size units.
   */
    public int setMinimumFrameGap (double gap_value)
    {
        if (gap_value < 0.0) {
            gap_value = 0.0;
        }
        Ddata[0] = gap_value;

        sendNativeCommand (
            GTX_FRAME_GAP,
            Ilist,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
     Define and set a drawing frame on the current drawing page.
     Subsequent location coordinates must be in the frame coordinate
     system until the frame is changed or unset.
  <p>
    This method can be used to create a complex frame that either stands
    on its own or that is attached to another frame.  A simpler method,
    {@link #createAttachedFrame createAttachFrame}, can be used for attaching legends, title
    blocks, etc to a stand alone "base" frame.  However, you must use this
    complex method to create the "base" frame before creating any frame
    that will attach to the "base" frame.
  <p>
    There is a yet deeper level of detail you can provide after the frame has
    been created.  You can call the {@link #setFrameAxisProperties setFrameAxisProperties} method to
    specify details of how labeled axes will be generated.  You need to
    specify border labeling in the frame creation in order to set the
    labeling detail later.
  <p>
    The expected limits of the frame (xmin, ymin to xmax, ymax) do not need
    to be exact.  If in doubt, err on the larger side.  However, try to be
    somewhat close.  These values are used to set up some frame resources at
    creation time and there are efficiency advantages to having the limits
    fairly close.
  <p>
    The page corners of the frame do not include borders.  Any attachments to
    the frame will automatically adjust for borders.  If the frame corner points
    (x1,y1 to x2,y2) do not have the same aspect ratio as the page corner points,
    the frame limits are adjusted to have the same aspect ratio (if a unit aspect
    ratio is specified via aspectFlag).
  <p>
    The frame will not participate in zooming and paning unless is is rescaleable.
    If you want to be able to drag on the border and move the frame, set moveable to
    1.  If the aspectFlag is not a valid constant, it is set to force unit aspect.
    If the borderFlag is not a valid constant, it is set to no border.  If the
    attachPosition is not a valis constant, it is set to no attach.
  <p>
    Captions will only be drawn on labelled axes.  You can control this more with the
    {@link #setFrameAxisProperties setFrameAxisProperties} method after creating the frame.  If any of the optional
    String parameters (horizontalCaption, verticalCaption, or attachFrameName) are not
    used, you should set them to null.
  <p>
    The attachPosition, extraGap and perpendicularMove parameters only have meaning
    if the frame is attached (the attachframeName is valid).
  <p>
    If any of the parameters are illegal, an {@link IllegalArgumentException} is thrown.
  @param name Unique name (in this display list) for the frame
  @param xminHint The expected minimum X coordinate of graphics drawn to the frame.
  @param yminHint The expected minimum Y coordinate of graphics drawn to the frame.
  @param xmaxHint The expected maximum X coordinate of graphics drawn to the frame.
  @param ymaxHint The expected maximum Y coordinate of graphics drawn to the frame.
  @param pxmin The page X coordinate of the lower left corner of the frame.
  @param pymin The page Y coordinate of the lower left corner of the frame.
  @param pxmax The page X coordinate of the upper right corner of the frame.
  @param pymax The page Y coordinate of the upper right corner of the frame.
  @param x1 The frame X coordinate initially mapped to the pxmin location.
  @param y1 The frame Y coordinate initially mapped to the pymin location.
  @param x2 The frame X coordinate initially mapped to the pxmax location.
  @param y2 The frame Y coordinate initially mapped to the pxmax location.
  @param rescaleable Set to one (1) to allow rescaling or zero (0) to not allow rescaling.
  @param moveable Set to one (1) to allow moving via the mouse or to zero (0) to not allow moving.
  @param aspectFlag Set to one of the following constants.
  <ul>
  <li> {@link #FORCE_UNIT_FRAME_ASPECT_RATIO}
  <li> {@link #ALLOW_ANY_FRAME_ASPECT_RATIO}
  </ul>
  @param borderFlag Set to one of the following constants.
  <ul>
  <li> {@link #FRAME_NO_BORDER}
  <li> {@link #FRAME_PLAIN_BORDER}
  <li> {@link #FRAME_LABEL_LEFT_BOTTOM_BORDER}
  <li> {@link #FRAME_LABEL_LEFT_TOP_BORDER}
  <li> {@link #FRAME_LABEL_RIGHT_BOTTOM_BORDER}
  <li> {@link #FRAME_LABEL_RIGHT_TOP_BORDER}
  <li> {@link #FRAME_LABEL_ALL_SIDES_BORDER}
  </ul>
  @param horizontalCaption Optional caption string for top and bottom axes, if they are labelled.
  @param verticalCaption Optional caption string for left and right axes, if they are labelled.
  @param attachFrameName Optional name of a frame to attach this frame to.
  @param attachPosition Set to one of the following constants.
  <ul>
  <li> {@link #FRAME_NO_ATTACH}
  <li> {@link #FRAME_ATTACH_LEFT_MIN}
  <li> {@link #FRAME_ATTACH_LEFT_MIDDLE}
  <li> {@link #FRAME_ATTACH_LEFT_MAX}
  <li> {@link #FRAME_ATTACH_TOP_MIN}
  <li> {@link #FRAME_ATTACH_TOP_MIDDLE}
  <li> {@link #FRAME_ATTACH_TOP_MAX}
  <li> {@link #FRAME_ATTACH_RIGHT_MIN}
  <li> {@link #FRAME_ATTACH_RIGHT_MIDDLE}
  <li> {@link #FRAME_ATTACH_RIGHT_MAX}
  <li> {@link #FRAME_ATTACH_BOTTOM_MIN}
  <li> {@link #FRAME_ATTACH_BOTTOM_MIDDLE}
  <li> {@link #FRAME_ATTACH_BOTTOM_MAX}
  </ul>
  @param extraGap This is the additional gap, in device size units, that you want to
                  insert between the base frame and this frame (if this frame is attached).
  @param perpendicularMove The movement, in device size units, that will be applied to
                           the frame in its anchored position.  If the frame is anchored on the left
                           or right, this movement will be vertical.  If anchored on the top or
                           bottom, the movement will be horizontal.
  @param scaleWidthToAttachFrame
    Set this to true if you want to scale the width of an attached frame the same as
                            the frame it is attached to.
  @param scaleHeightToAttachFrame
    Set this to true if you want to scale the height of an attached frame the same as
                            the frame it is attached to.
   */
    public int createFrame (
        String name,
        double xminHint,
        double yminHint,
        double xmaxHint,
        double ymaxHint,
        double pxmin,
        double pymin,
        double pxmax,
        double pymax,
        double x1,
        double y1,
        double x2,
        double y2,
        int rescaleable,
        int moveable,
        int aspectFlag,
        int borderFlag,
        String horizontalCaption,
        String verticalCaption,
        String attachFrameName,
        int attachPosition,
        double extraGap,
        double perpendicularMove,
        boolean scaleWidthToAttachFrame,
        boolean scaleHeightToAttachFrame)
    {

    /*
     * Exceptions for illegal arguments.
     */
        if (xminHint >= xmaxHint  ||  yminHint >= ymaxHint) {
            throw
            new IllegalArgumentException
            ("The expected minimums must be less than the expected maximums");
        }
        double tiny = (xmaxHint - xminHint + ymaxHint - yminHint) / 2000000.0;
        double dx = x2 - x1;
        double dy = y2 - y1;
        if (dx < 0.0) dx = -dx;
        if (dy < 0.0) dy = -dy;
        if (dx <= tiny  ||  dy <= tiny) {
            throw
            new IllegalArgumentException
            ("Either x1 equals x2 or y1 equals y2");
        }
        if (pxmin >= pxmax  ||  pymin >= pymax) {
            throw
            new IllegalArgumentException
            ("The page minimums must be less than the page maximums");
        }

        if (aspectFlag != ALLOW_ANY_FRAME_ASPECT_RATIO) {
            aspectFlag = 0;
        }
        Ilist[0] = borderFlag;
        Ilist[1] = rescaleable;
        Ilist[2] = moveable;
        Ilist[3] = aspectFlag;
        Ilist[4] = attachPosition;
        Ilist[5] = 0;
        if (scaleWidthToAttachFrame == true) {
            Ilist[5] = 1;
        }
        Ilist[6] = 0;
        if (scaleHeightToAttachFrame == true) {
            Ilist[6] = 1;
        }

        String str = name;
        str = str.concat (MSG_STRING_SEPARATOR);
        if (attachFrameName != null) {
            str = str.concat (attachFrameName);
        }
        str = str.concat (MSG_STRING_SEPARATOR);
        if (horizontalCaption != null) {
            str = str.concat (horizontalCaption);
        }
        str = str.concat (MSG_STRING_SEPARATOR);
        if (verticalCaption != null) {
            str = str.concat (verticalCaption);
        }

        Ddata[0] = x1;
        Ddata[1] = y1;
        Ddata[2] = x2;
        Ddata[3] = y2;
        Ddata[4] = pxmin;
        Ddata[5] = pymin;
        Ddata[6] = pxmax;
        Ddata[7] = pymax;
        Ddata[8] = xminHint;
        Ddata[9] = yminHint;
        Ddata[10] = xmaxHint;
        Ddata[11] = ymaxHint;
        Ddata[12] = extraGap;
        Ddata[13] = perpendicularMove;

        sendNativeCommand (
            GTX_CREATEFRAME,
            Ilist,
            str,
            Ddata
        );

        currentFrameName = name;

        return 1;

    }

/*-------------------------------------------------------------*/

 /**
 Adjust the specified frame to fit slightly larger that the extent of the
 data drawn to the frame.  If the frame name is null, the current active
 frame is adjusted.  If you call this after you have finished drawing to
 a frame, then the drawing will look more centered and balanced in the
 frame.
<p>
 If this is called prior to the first painting of the window, the extents
 will try to accounbt for text going outside of the data area, but it cannot
 exactly determine the extents including text.  You can and
 probably will see text strings clipped to the frame even if this is called.
 The versions of this method that allow you to specify extra space inside
 the frame at the sides of the data can be used to mitigate the problem of
 text being clipped.
@param frameName Name of the frame to be set.
 */
    public int setFrameToExtents (
        String frameName
    )
    {
        if (frameName == null) {
            frameName = currentFrameName;
        }

        if (frameName == null) return -1;

        String str = frameName;
        Ddata[0] = 0.0;
        Ddata[1] = 0.0;
        Ddata[2] = 0.0;
        Ddata[3] = 0.0;

        sendNativeCommand (
            GTX_FRAME_NAME_EXTENTS,
            Ilist,
            str,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

 /**
 Adjust the specified frame to fit slightly larger that the extent of the
 data drawn to the frame.  If the frame name is null, the current active
 frame is adjusted.  If you call this after you have finished drawing to
 a frame, then the drawing will look more centered and balanced in the
 frame.
<p>
 If this is called prior to the first painting of the window, the extents
 will not exactly account for text that goes outside of the data area.  You can and
 probably will see text strings clipped to the frame even if this is called.
 You can mitigate this by setting the appropriate margins to make extra space
 if you know you have extra vertical or horizontal text.
<p>
 Positive margins always add space and negative margins always remove space.
@param frameName Name of the frame to be set.
@param xInset Extra space for left and right side, in the X frame units.
@param yInset Extra space for the top and bottom sides, in the Y frame units.
 */
    public int setFrameToExtents (
        String frameName,
        double xInset,
        double yInset)
    {
        if (frameName == null) {
            frameName = currentFrameName;
        }

        if (frameName == null) return -1;

        String str = frameName;
        Ddata[0] = yInset;
        Ddata[1] = xInset;
        Ddata[2] = yInset;
        Ddata[3] = xInset;

        sendNativeCommand (
            GTX_FRAME_NAME_EXTENTS,
            Ilist,
            str,
            Ddata
        );

        return 1;
    }


/*-------------------------------------------------------------*/

 /**
 Adjust the specified frame to fit slightly larger that the extent of the
 data drawn to the frame.  If the frame name is null, the current active
 frame is adjusted.  If you call this after you have finished drawing to
 a frame, then the drawing will look more centered and balanced in the
 frame.
<p>
 If this is called prior to the first painting of the window, the extents
 will not account for text that goes outside of the data area.  You can and
 probably will see text strings clipped to the frame even if this is called.
 You can mitigate this by setting the appropriate margins to make extra space
 if you know you have extra vertical or horizontal text.
<p>
 Positive margins always add space and negative margins always remove space.
@param frameName Name of the frame to be set.
@param topInset Extra space for the top side, in frame y units.
@param leftInset Extra space for the left side, in frame x units.
@param bottomInset Extra space for the bottom side, in frame y units.
@param rightInset Extra space for the right side, in frame x units.
 */
    public int setFrameToExtents (
        String frameName,
        double topInset,
        double leftInset,
        double bottomInset,
        double rightInset)
    {
        if (frameName == null) {
            frameName = currentFrameName;
        }

        if (frameName == null) return -1;

        String str = frameName;
        Ddata[0] = topInset;
        Ddata[1] = leftInset;
        Ddata[2] = bottomInset;
        Ddata[3] = rightInset;

        sendNativeCommand (
            GTX_FRAME_NAME_EXTENTS,
            Ilist,
            str,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
     Define and set a simple attached frame.  This is intended for things like
     legend frames, title frames, etc.  The frame created with this method
     must be attached to a previously created frame.  The existing frame may
     in turn be attached to another frame.  However, eventually the attachment
     chain needs to point to a frame that is specifically located somewhwere
     on the drawing page.
     <p>
     To create a base frame for anchoring to, you need to use the
     <code>createFrame</code> method.
     <p>
     The borderFlag parameter here differs from the borderFlag parameter in
     the createFrame method.  No frame labeling can be done with a frame created
     by this method.  Therefore, borderFlag is either zero for no border or
     1 for drawing a plain border.
     <p>
     The various units parameters are included because they will hopefully be
     useful in the future.  For now, the units are always treated as if {@link #PAGE_UNITS}
     is specified.  A lot more work on attachments is needed to get the other options
     working.
     <p>
     When an attached frame is created, several frame parameters are set to defaults as follows.
    <ul>
    <li> page default lower left is set to 0,0
    <li> page upper right is set to width, height
    <li> frame xmin,ymin is set to 0,0
    <li> frame xmax,ymax is set to width, height
    <li> frame viewable lower left is set to 0,0
    <li> frame viewable upper right is set to width, height
    <li> the frame is not rescaleable
    <li> the frame is moveable
    <li> the frame aspect ratio is forced to 1
    <li> no frame border labeling is done
    <li> no border captions are drawn
    <li> no border tick marks are drawn
    </ul>
     The contents of the frame will be clipped according to the most recent call to the
     setFrameClip method.
    @param name Name of the new frame.  This must not have been used before in this display list.
    @param pageWidth  Width of the frame in width units.
    @param pageHeight Height of the frame in height units.
    @param attachFrameName Name of the frame to attach to.  This frame must have been created
    in this display list prior to calling this method.
    @param attachPosition The position of this frame relative to the frame specified by attachFrameName.
    Set to one of the following constants.
    <ul>
    <li> {@link #FRAME_ATTACH_LEFT_MIN}
    <li> {@link #FRAME_ATTACH_LEFT_MIDDLE}
    <li> {@link #FRAME_ATTACH_LEFT_MAX}
    <li> {@link #FRAME_ATTACH_TOP_MIN}
    <li> {@link #FRAME_ATTACH_TOP_MIDDLE}
    <li> {@link #FRAME_ATTACH_TOP_MAX}
    <li> {@link #FRAME_ATTACH_RIGHT_MIN}
    <li> {@link #FRAME_ATTACH_RIGHT_MIDDLE}
    <li> {@link #FRAME_ATTACH_RIGHT_MAX}
    <li> {@link #FRAME_ATTACH_BOTTOM_MIN}
    <li> {@link #FRAME_ATTACH_BOTTOM_MIDDLE}
    <li> {@link #FRAME_ATTACH_BOTTOM_MAX}
    </ul>
    @param borderFlag  Specify whether a plain border is to be drawn around the frame.
                       Set to zero for no border or to anything else for a plain border.
    @param extraGap This is the additional gap, in gap units, that you want to
                    insert between the base frame and this frame.
    @param perpendicularMove The movement, in move units, that will be applied to
                             the frame in its anchored position.  If the frame is anchored on the left
                             or right, this movement will be vertical.  If anchored on the top or
                             bottom, the movement will be horizontal.
    @param widthUnits Choose from the following (Only PAGE_UNITS currently works):
    <ul>
    <li> {@link #PAGE_UNITS}
    <li> {@link #DEVICE_INCHES}
    <li> {@link #DEVICE_CM}
    <li> {@link #PERCENT_BASE_FRAME}
    <li> {@link #PERCENT_WINDOW}
    </ul>
    @param heightUnits Choose from the following (Only PAGE_UNITS currently works):
    <ul>
    <li> {@link #PAGE_UNITS}
    <li> {@link #DEVICE_INCHES}
    <li> {@link #DEVICE_CM}
    <li> {@link #PERCENT_BASE_FRAME}
    <li> {@link #PERCENT_WINDOW}
    </ul>
    @param gapUnits Choose from the following (Only PAGE_UNITS currently works):
    <ul>
    <li> {@link #PAGE_UNITS}
    <li> {@link #DEVICE_INCHES}
    <li> {@link #DEVICE_CM}
    <li> {@link #PERCENT_BASE_FRAME}
    <li> {@link #PERCENT_WINDOW}
    </ul>
    @param moveUnits Choose from the following (Only PAGE_UNITS currently works):
    <ul>
    <li> {@link #PAGE_UNITS}
    <li> {@link #DEVICE_INCHES}
    <li> {@link #DEVICE_CM}
    <li> {@link #PERCENT_BASE_FRAME}
    <li> {@link #PERCENT_WINDOW}
    </ul>
   */
    public int createAttachedFrame (
        String name,
        double pageWidth,
        double pageHeight,
        String attachFrameName,
        int attachPosition,
        int borderFlag,
        double extraGap,
        double perpendicularMove,
        int widthUnits,
        int heightUnits,
        int gapUnits,
        int moveUnits
    )
    {
        int            istat;

        if (name == null) {
            return -1;
        }
        if (attachFrameName == null) {
            return -1;
        }
        if (pageWidth <= 0.0  ||  pageHeight <= 0.0) {
            return -1;
        }

        double xmin = 0.0;
        double ymin = 0.0;
        double xmax = pageWidth;
        double ymax = pageHeight;
        double x1 = 0.0;
        double y1 = 0.0;
        double x2 = pageWidth;
        double y2 = pageHeight;
        double px1 = 0.0;
        double py1 = 0.0;
        double px2 = pageWidth;
        double py2 = pageHeight;
        int rescaleable = 0;
        int moveable = 1;
        int aspectFlag = 0;

        Ilist[0] = borderFlag;
        Ilist[1] = rescaleable;
        Ilist[2] = moveable;
        Ilist[3] = aspectFlag;
        Ilist[4] = attachPosition;
        Ilist[5] = 0;
        Ilist[6] = 0;

        String str = name;
        str = str.concat (MSG_STRING_SEPARATOR);
        str = str.concat (attachFrameName);

        Ddata[0] = x1;
        Ddata[1] = y1;
        Ddata[2] = x2;
        Ddata[3] = y2;
        Ddata[4] = px1;
        Ddata[5] = py1;
        Ddata[6] = px2;
        Ddata[7] = py2;
        Ddata[8] = xmin;
        Ddata[9] = ymin;
        Ddata[10] = xmax;
        Ddata[11] = ymax;
        Ddata[12] = extraGap;
        Ddata[13] = perpendicularMove;

        istat = sendNativeCommand (
            GTX_CREATEFRAME,
            Ilist,
            str,
            Ddata
        );

        currentFrameName = name;

        return istat;

    }

/*-------------------------------------------------------------*/

  /*
  Change the parameters for a single frame axis.  This version requires you to
  set the individual properties for the axis.  There is also an {@link AxisProperties}
  object that you can use to set properties.
  @param frameName   Name of the frame that holds the axis.
  @param axisID Choose from one of the following:
  <ul>
  <li> {@link #FRAME_LEFT_AXIS}
  <li> {@link #FRAME_RIGHT_AXIS}
  <li> {@link #FRAME_BOTTOM_AXIS}
  <li> {@link #FRAME_TOP_AXIS}
  </ul>
  @param labelFlag Set to 1 to draw labels at major intervals or set to zero to not draw labels.
  @param tickFlag  Set to 1 to draw major and minor ticks.  Set to zero to not draw ticks.
  @param tickDirection Set to 1 for interior ticks.  Set to -1 for exterior ticks.
  @param caption Sting with caption, or null if no caption is wanted.
  @param majorInterval Major tick mark and labeling interval, or -1 to automatically determine.
  @param lineColor {@link Color} for lines
  @param textColor {@link Color} for text
  @param lineThickness Thickness of lines, in device size units.
  @param textSize Size of label text (in device size units).  Caption text will be 1.3 times larger.
  @param textThickness Thickness of label and caption text.
  @param textFont Font to use for labels and caption.  Must be a non filled font (fonts 0-7 or 101 - 107).
   */
    private int setFrameAxisProperties (
        String       frameName,
        int          axisID,
        int          labelFlag,
        int          tickFlag,
        int          tickDirection,
        String       caption,
        double       majorInterval,
        Color        lineColor,
        Color        textColor,
        double       lineThickness,
        double       textSize,
        double       textThickness,
        int          textFont)
    {
        if (frameName == null) {
            return -1;
        }

        String str = frameName;
        str = str.concat (MSG_STRING_SEPARATOR);
        if (caption != null) {
            str = str.concat (caption);
        }

        Ilist[0] = axisID;
        Ilist[1] = labelFlag;
        Ilist[2] = tickFlag;
        Ilist[3] = lineColor.getRed();
        Ilist[4] = lineColor.getGreen();
        Ilist[5] = lineColor.getBlue();
        Ilist[6] = textColor.getRed();
        Ilist[7] = textColor.getGreen();
        Ilist[8] = textColor.getBlue();
        Ilist[9] = textFont;
        Ilist[10] = 0;
        Ilist[11] = tickDirection;
        if (caption != null) {
            Ilist[10] = 1;
        }

        Ddata[0] = majorInterval;
        Ddata[1] = lineThickness;
        Ddata[2] = textSize;
        Ddata[3] = textThickness;

        int istat = sendNativeCommand (
            GTX_FRAME_AXIS_VALUES,
            Ilist,
            str,
            Ddata
        );

        return istat;

    }

/*-------------------------------------------------------------*/

    private void setupAxisFont(AxisProperties properties) {
      if (properties == null || properties.fontName == null)
        return;

      int index = getExistingFontNumber(
        properties.fontName,
        properties.fontStyle
      );
      if (index < 0)
        index = addNewFont(
          properties.fontName,
          properties.fontStyle
        );

      /*Font font = */deriveFontForIndex (index, 1.0);
      properties.textFont = index + 1000;
    }

    /**
 Add an axis that is not on the border of a frame.  The location of the
 first point of the axis is specified in x1, y1.  The second point is
 specified in x2, y2.    The tickDirection flag in the {@link AxisProperties}
 object is interpreted differently for this type of axis.  If the tick direction
 is set to 1, then the ticks are drawn to the left when traveling from x1,y1 to x2,y2.
 If the tick direction is set to -1, the ticks are drawn to the right when moving
 from x1,y1 to x2, y2.
 @param x1 The x coordinate of the first axis point.
 @param y1 The y coordinate of the first axis point.
 @param x2 The x coordinate of the second axis point.
 @param y2 The y coordinate of the second axis point.
 @param firstValue The axis value at x1, y1.
 @param lastValue The axis value at x2, y2.
 @param labelDirection The caption direction for the axis.  To draw the caption on the
 same side as the tick marks, this is 1.  To draw on the opposite side from the tick
 marks, this is -1.
 @param properties An {@link AxisProperties} object with the various
 properties for the axis.  If this is null, a default properties object is created
 and used.
 */
    public int addAxis (
        double          x1,
        double          y1,
        double          x2,
        double          y2,
        double          firstValue,
        double          lastValue,
        int             labelDirection,
        AxisProperties  properties
    ) {
        if (properties == null) {
            properties = new AxisProperties ();
        }

        setupAxisFont(properties);

        String str = properties.caption;

        Ilist[0] = labelDirection;
        Ilist[1] = properties.labelFlag;
        Ilist[2] = properties.tickFlag;
        Ilist[3] = properties.lineColor.getRed();
        Ilist[4] = properties.lineColor.getGreen();
        Ilist[5] = properties.lineColor.getBlue();
        Ilist[6] = properties.textColor.getRed();
        Ilist[7] = properties.textColor.getGreen();
        Ilist[8] = properties.textColor.getBlue();
        Ilist[9] = properties.textFont;
        Ilist[10] = 0;
        Ilist[11] = properties.tickDirection;
        if (properties.caption != null) {
            Ilist[10] = 1;
        }
        if (Ilist[11] == -1) Ilist[11] = -2;
        if (Ilist[11] == 1) Ilist[11] = 2;
        Ilist[12] = properties.labelAnchor;

        Ddata[0] = properties.majorInterval;
        Ddata[1] = properties.lineThickness;
        Ddata[2] = properties.textSize;
        Ddata[3] = properties.textThickness;
        Ddata[4] = x1;
        Ddata[5] = y1;
        Ddata[6] = x2;
        Ddata[7] = y2;
        Ddata[8] = firstValue;
        Ddata[9] = lastValue;
        Ddata[10] = properties.labelAngle;

        int istat = sendNativeCommand (
            GTX_ADD_AXIS,
            Ilist,
            str,
            Ddata
        );

        return istat;

    }

/*-------------------------------------------------------------*/

  /**
  Set the properties of a frame axis, using an {@link AxisProperties} object.
  Create and fill in an AxisProperties object prior to calling this method
  and use it to set the properties of an axis.
  @param frameName   Name of the frame that holds the axis.
  @param axisID Choose from one of the following:
  <ul>
  <li> {@link #FRAME_LEFT_AXIS}
  <li> {@link #FRAME_RIGHT_AXIS}
  <li> {@link #FRAME_BOTTOM_AXIS}
  <li> {@link #FRAME_TOP_AXIS}
  </ul>
  @param properties An {@link AxisProperties} object that has been filled in with
  the desired properties for this axis.
 */
    public int setFrameAxisProperties (
        String           frameName,
        int              axisID,
        AxisProperties   properties
    ) {
        if (properties == null)
          properties = new AxisProperties();

        setupAxisFont(properties);

        int istat =
        setFrameAxisProperties (
            frameName,
            axisID,
            properties.labelFlag,
            properties.tickFlag,
            properties.tickDirection,
            properties.caption,
            properties.majorInterval,
            properties.lineColor,
            properties.textColor,
            properties.lineThickness,
            properties.textSize,
            properties.textThickness,
            properties.textFont
        );

        return istat;
    }

/*-------------------------------------------------------------*/

  /**
     Change the visible window for a scaleable frame.  The value of the borderFlag
     parameter determines whether the frame size changes or not.  If you just want
     to change the viewable contents then set borderFlag to zero (0).  If you want to
     change the contents and resize the border to the limits specified, set
     borderFlag to one (1).
    <p>
     If the border is not resized, then the contents of the limits specified are
     guaranteed to be shown and other frame contents surrounding the specified
     limits will probably also be shown.
    <p>
     If there is no scaleable frame in the display list with the specified name then nothing
     is done.
    <p>
     If you resize the border, and other frames are attached to the resized frame, the
     attachments will screw up.  The attachment code needs a lot of work to correctly
     deal with resized frames.  I suggest you do not use the resize border option
     for now.
    @param name Name of an existing frame.
    @param borderFlag Border resize flag.  Zero means do not resize border.  One means do resize border.
    @param fx1 Frame X coordinate to map to the lower left corner of the frame.
    @param fy1 Frame Y coordinate to map to the lower left corner of the frame.
    @param fx2 Frame X coordinate to map to the upper right corner of the frame
    @param fy2 Frame Y coordinate to map to the upper right corner of the frame
   */
    public int rescaleFrame (String name,
                             int borderFlag,
                             double fx1,
                             double fy1,
                             double fx2,
                             double fy2)
    {
        Ilist[0] = borderFlag;
        Ddata[0] = fx1;
        Ddata[1] = fy1;
        Ddata[2] = fx2;
        Ddata[3] = fy2;

        sendNativeCommand (
            GTX_RESETFRAME,
            Ilist,
            name,
            Ddata
        );

        return 1;

    }


/*-------------------------------------------------------------*/

  /**
  Add a color image to the display list.  The colors are defined
  as separate arrays of red, green, blue and transparency values.
  The values of each range from 0 - 255.  The image will be drawn
  in the rectangle defined by the xmin, ymin and xmax, ymax points.
  If a frame is active, these must be frame coordinate points.  If
  no frame is active, these must be page coordinate points. Note that xmin, ymin
  must be the lower left corner and xmax,ymax must be the upper right corner.
 <p>
  No lookup of colors specified via the {@link #setImageColors} method is done.  There
  must be ncol samples per row and nrow rows for a total of ncol * nrow
  values in each color coordinate array.  The first sample is
  at xmin, ymin and the last sample is at xmax, ymax.
 <p>
  Transparency of zero means completely transparent.  Transparency of 255 means
  completely opaque.
 <p>
  If a frame is active, the corner points must be in frame coordinates.  The actual image
  may be flipped or otherwise mirrored if the frame to page mapping flips or mirrors.
 @param red Array of red color values (0 - 255)
 @param green Array of green color values (0 - 255)
 @param blue Array of blue color values (0 - 255)
 @param trans Array of transparency color values (0 - 255)
 @param ncol Number of columns in the image.
 @param nrow Number of rows in the image.
 @param xmin X coordinate of the lower left corner of the image.
 @param ymin Y coordinate of the lower left corner of the image.
 @param xmax X coordinate of the upper right corner of the image.
 @param ymax Y coordinate of the upper right corner of the image.
   */
    public int addColorImage (boolean[] red,
                              boolean[] green,
                              boolean[] blue,
                              boolean[] trans,
                              int ncol,
                              int nrow,
                              double xmin,
                              double ymin,
                              double xmax,
                              double ymax)
    {
        Ilist[0] = 0;  // color image type
        Ilist[1] = ncol;
        Ilist[2] = nrow;
        Ddata[0] = xmin;
        Ddata[1] = ymin;
        Ddata[2] = xmax;
        Ddata[3] = ymax;

        int ntot = ncol * nrow;
        new_bdata (4 * ntot);

        for (int i=0; i < ntot; i++) {
            Bdata[i] = red[i];
            Bdata[ntot + i] = green[i];
            Bdata[2*ntot + i] = blue[i];
            Bdata[3*ntot + i] = trans[i];
        }

        sendNativeCommand (
            GTX_DRAWIMAGE,
            Ilist,
            null,
            Bdata,
            null,
            null,
            null,
            Ddata
        );

        return 1;

    }



/*-------------------------------------------------------------*/

  /**
  Add a double data image to the display list.  The colors for the
  image will be assigned based on the most recent call to the
  {@link #setImageColors} method.  The image will be drawn in the rectangle defined
  by xmin, ymin, xmax and ymax.  These must be in frame units if a frame
  is active or in page units if no frame is active.   Note that xmin, ymin
  must be the lower left corner and xmax,ymax must be the upper right corner.
  <p>
  There must be ncol * nrow samples in the data array.  The first sample is
  located at xmin, ymin and the last sample is located at xmax, ymax.  Each
  horizontal row has ncol samples and there are nrow number of rows.
  Any data sample greater than or equal to the specified null value
  will be made transparent.  If the null value is set to zero, no null
  checking will be done.
 @param data Array of double data to draw as an image.
 @param ncol Number of columns in the data array.
 @param nrow Number of rows in the data array.
 @param null_value Any data value greater than or equal to this will be transparent.
 @param xmin X coordinate of the lower left corner of the image.
 @param ymin Y coordinate of the lower left corner of the image.
 @param xmax X coordinate of the upper right corner of the image.
 @param ymax Y coordinate of the upper right corner of the image.
   */
    public int addDataImage (double[] data,
                             int ncol,
                             int nrow,
                             double null_value,
                             double xmin,
                             double ymin,
                             double xmax,
                             double ymax)
    {
        Ilist[0] = 1;  // double image type
        Ilist[1] = ncol;
        Ilist[2] = nrow;

        new_ddata (ncol * nrow + 5);
        Ddata[0] = xmin;
        Ddata[1] = ymin;
        Ddata[2] = xmax;
        Ddata[3] = ymax;
        Ddata[4] = null_value;

        for (int i=0; i<ncol*nrow; i++) {
            Ddata[i+5] = data[i];
        }

        sendNativeCommand (
            GTX_DRAWIMAGE,
            Ilist,
            null,
            null,
            null,
            null,
            null,
            Ddata
        );

        return 1;

    }


/*-------------------------------------------------------------*/

  /**
 Add a float data image to the display list.  The colors for the
 image will be assigned based on the most recent call to the
 {@link #setImageColors} method.  The image will be drawn in the rectangle defined
 by xmin, ymin, xmax and ymax.  These must be in frame units if a frame
 is active or in page units if no frame is active.   Note that xmin, ymin
 must be the lower left corner and xmax,ymax must be the upper right corner.
 <p>
 There must be ncol * nrow samples in the data array.  The first sample is
 located at xmin, ymin and the last sample is located at xmax, ymax.  Each
 horizontal row has ncol samples and there are nrow number of rows.
 Any data sample greater than or equal to the specified null value
 will be made transparent.  If the null value is set to zero, no null
 checking will be done.
 @param data Array of float data to draw as an image.
 @param ncol Number of columns in the data array.
 @param nrow Number of rows in the data array.
 @param null_value Any data value greater than or equal to this will be transparent.
 @param xmin X coordinate of the lower left corner of the image.
 @param ymin Y coordinate of the lower left corner of the image.
 @param xmax X coordinate of the upper right corner of the image.
 @param ymax Y coordinate of the upper right corner of the image.
   */
    public int addDataImage (float[] data,
                             int ncol,
                             int nrow,
                             float null_value,
                             double xmin,
                             double ymin,
                             double xmax,
                             double ymax)
    {
        Ilist[0] = 2;  // float image type
        Ilist[1] = ncol;
        Ilist[2] = nrow;

        Ddata[0] = xmin;
        Ddata[1] = ymin;
        Ddata[2] = xmax;
        Ddata[3] = ymax;

        new_fdata (ncol * nrow + 1);
        Fdata[0] = null_value;

        for (int i=0; i<ncol*nrow; i++) {
            Fdata[i+1] = data[i];
        }

        sendNativeCommand (
            GTX_DRAWIMAGE,
            Ilist,
            null,
            null,
            null,
            null,
            Fdata,
            Ddata
        );

        return 1;

    }



/*-------------------------------------------------------------*/

  /**
 Add an int data image to the display list.  The colors for the
 image will be assigned based on the most recent call to the
 {@link #setImageColors} method.  The image will be drawn in the rectangle defined
 by xmin, ymin, xmax and ymax.  These must be in frame units if a frame
 is active or in page units if no frame is active.   Note that xmin, ymin
 must be the lower left corner and xmax,ymax must be the upper right corner.
 <p>
 There must be ncol * nrow samples in the data array.  The first sample is
 located at xmin, ymin and the last sample is located at xmax, ymax.  Each
 horizontal row has ncol samples and there are nrow number of rows.
 Any data sample greater than or equal to the specified null value
 will be made transparent.  If the null value is set to zero, no null
 checking will be done.
 @param data Array of int data to draw as an image.
 @param ncol Number of columns in the data array.
 @param nrow Number of rows in the data array.
 @param null_value Any data value greater than or equal to this will be transparent.
 @param xmin X coordinate of the lower left corner of the image.
 @param ymin Y coordinate of the lower left corner of the image.
 @param xmax X coordinate of the upper right corner of the image.
 @param ymax Y coordinate of the upper right corner of the image.
   */
    public int addDataImage (int[] data,
                             int ncol,
                             int nrow,
                             int null_value,
                             double xmin,
                             double ymin,
                             double xmax,
                             double ymax)
    {
        Ilist[0] = 3;  // int image type
        Ilist[1] = ncol;
        Ilist[2] = nrow;

        Ddata[0] = xmin;
        Ddata[1] = ymin;
        Ddata[2] = xmax;
        Ddata[3] = ymax;

        new_idata (ncol * nrow + 1);
        Idata[0] = null_value;
        for (int i=0; i<ncol*nrow; i++) {
            Idata[i+1] = data[i];
        }

        sendNativeCommand (
            GTX_DRAWIMAGE,
            Ilist,
            null,
            null,
            null,
            Idata,
            null,
            Ddata
        );

        return 1;

    }



/*-------------------------------------------------------------*/

  /**
 Add a short data image to the display list.  The colors for the
 image will be assigned based on the most recent call to the
 {@link #setImageColors} method.  The image will be drawn in the rectangle defined
 by xmin, ymin, xmax and ymax.  These must be in frame units if a frame
 is active or in page units if no frame is active.   Note that xmin, ymin
 must be the lower left corner and xmax,ymax must be the upper right corner.
 <p>
 There must be ncol * nrow samples in the data array.  The first sample is
 located at xmin, ymin and the last sample is located at xmax, ymax.  Each
 horizontal row has ncol samples and there are nrow number of rows.
 Any data sample greater than or equal to the specified null value
 will be made transparent.  If the null value is set to zero, no null
 checking will be done.
 @param data Array of short data to draw as an image.
 @param ncol Number of columns in the data array.
 @param nrow Number of rows in the data array.
 @param null_value Any data value greater than or equal to this will be transparent.
 @param xmin X coordinate of the lower left corner of the image.
 @param ymin Y coordinate of the lower left corner of the image.
 @param xmax X coordinate of the upper right corner of the image.
 @param ymax Y coordinate of the upper right corner of the image.
   */
    public int addDataImage (short[] data,
                             int ncol,
                             int nrow,
                             short null_value,
                             double xmin,
                             double ymin,
                             double xmax,
                             double ymax)
    {
        Ilist[0] = 4;  // short image type
        Ilist[1] = ncol;
        Ilist[2] = nrow;

        Ddata[0] = xmin;
        Ddata[1] = ymin;
        Ddata[2] = xmax;
        Ddata[3] = ymax;

        new_sdata (ncol * nrow + 1);
        Sdata[0] = null_value;
        for (int i=0; i<ncol*nrow; i++) {
            Sdata[i+1] = data[i];
        }

        sendNativeCommand (
            GTX_DRAWIMAGE,
            Ilist,
            null,
            null,
            Sdata,
            null,
            null,
            Ddata
        );

        return 1;

    }


/*-------------------------------------------------------------*/

  /**
 Add a boolean data image to the display list.  The colors for the
 image will be assigned based on the most recent call to the
 {@link #setImageColors} method.  The image will be drawn in the rectangle defined
 by xmin, ymin, xmax and ymax.  These must be in frame units if a frame
 is active or in page units if no frame is active.   Note that xmin, ymin
 must be the lower left corner and xmax,ymax must be the upper right corner.
 <p>
 There must be ncol * nrow samples in the data array.  The first sample is
 located at xmin, ymin and the last sample is located at xmax, ymax.  Each
 horizontal row has ncol samples and there are nrow number of rows.
 Any data sample greater than or equal to the specified null value
 will be made transparent.  If the null value is set to zero, no null
 checking will be done.
 @param data Array of boolean data to draw as an image.
 @param ncol Number of columns in the data array.
 @param nrow Number of rows in the data array.
 @param null_value Any data value greater than or equal to this will be transparent.
 @param xmin X coordinate of the lower left corner of the image.
 @param ymin Y coordinate of the lower left corner of the image.
 @param xmax X coordinate of the upper right corner of the image.
 @param ymax Y coordinate of the upper right corner of the image.
   */
    public int addDataImage (boolean[] data,
                             int ncol,
                             int nrow,
                             boolean null_value,
                             double xmin,
                             double ymin,
                             double xmax,
                             double ymax)
    {
        Ilist[0] = 3;  // int image type
        Ilist[1] = ncol;
        Ilist[2] = nrow;

        Ddata[0] = xmin;
        Ddata[1] = ymin;
        Ddata[2] = xmax;
        Ddata[3] = ymax;

        new_bdata (ncol * nrow + 1);
        Bdata[0] = null_value;
        for (int i=0; i<ncol*nrow; i++) {
            Bdata[i+1] = data[i];
        }

        sendNativeCommand (
            GTX_DRAWIMAGE,
            Ilist,
            null,
            Bdata,
            null,
            null,
            null,
            Ddata
        );

        return 1;

    }


/*-------------------------------------------------------------*/

  /**
  Add a complex filled polygons to the display list.  Complex polygons are packed into the x and y arrays
  as a sequence of polygon components.  Each component has a number of points defining it.  For example,
  if a polygon has its main component and 2 holes, there are 3 components total.  In this case ncomp
  is 3.  If the main component has 10 points, the first hole has 5 points and the second hole has 7 points,
  then npts[0] is 10, npts[1] is 5 and npts[2] is 7.  x[0] to x[9] has the main x coordinates.  y[0] to y[9]
  has main y coordinates.  x[10 = x[14] has hole1 x and y[10] to y[14] has hole 1 y.  x[15] to x[20] has
  hole 2 x and y[15] t y[20] has hole 2 y.
  <p>
  The polygon will be filled with the current fill color and pattern.  If the outline flag is specified
  as 1, the polygon border will be drawn using the current line drawing graphic attributes.
  <p>
  This method is identical to the addPolygon method.
  @param x Packed x coordinates for all polygon components.
  @param y Packed y coordinates for all polygon components.
  @param npts Array with number of points per component.
  @param ncomp Number of components.
  @param outline Set to 1 to draw outline or set to zero to not draw outline.
   */
    public int addFill (double[] x,
                        double[] y,
                        int[] npts,
                        int ncomp,
                        int outline)

    {
        int        ntot, i;

        ntot = 0;
        for (i=0; i<ncomp; i++) {
            ntot += npts[i];
        }
        new_ddata ((ntot + ncomp) * 2);
        for (i=0; i<ntot; i++) {
            Ddata[i] = x[i];
            Ddata[ntot + i] = y[i];
        }
        for (i=0; i<ncomp; i++) {
            Idata[i] = npts[i];
        }

        Ilist[0] = ncomp;
        Ilist[1] = ntot;
        Ilist[2] = outline;

        sendNativeCommand (
            GTX_FILLPOLY,
            Ilist,
            null,
            null,
            null,
            Idata,
            null,
            Ddata
        );

        logger.info ("    Added fill with " + ncomp + " components    ");

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set the fill color and pattern for subsequent polygon fills to the
  lithology and scale specified.  The color alpha will be applied to the solid
  color and the pattern alpha will be applied to the pattern.
  This method will change the fill color and fill pattern set via
  their set methods.
  @param lithType The number of the lithology from the following options.
  <ul>
  <li> {@link DLConst#SANDSTONE}
  <li> {@link DLConst#SILTSTONE}
  <li> {@link DLConst#SHALE}
  <li> {@link DLConst#LIMESTONE}
  <li> {@link DLConst#DOLOMITE}
  <li> {@link DLConst#EVAPORITE}
  <li> {@link DLConst#COAL}
  <li> {@link DLConst#IGNEOUS}
  </ul>
  @param scale Scale factor for the fill pattern.  Scales graeter than 1.0 will
make the pattern less dense.  Scales less than 1.0 will make the pattern more dense.
  @param colorAlpha Alpha value for the solid fill color.  Set from zero (transparent)
  to 255 (opaque).
  @param patternAlpha Alpha value for the fill pattern.  Set from zero (transparent)
  to 255 (opaque).
 */
    public int setLithFill (int lithType,
                            double scale,
                            int colorAlpha,
                            int patternAlpha)
    {
        if (lithType < 0  ||  lithType >= N_LITHOLOGY_TYPES) {
            return -1;
        }

        if (colorAlpha < 0) colorAlpha = 0;
        if (colorAlpha > 255) colorAlpha = 255;
        if (patternAlpha < 0) patternAlpha = 0;
        if (patternAlpha > 255) patternAlpha = 255;

      /*
       * Fill patterns start at 1, not zero.
       */
        setFillPattern (lithType + 1, scale);

      /*
       * Set the fill color.
       */
        Color cbase = LithFill.lithologyColors[lithType];
        Color cnew = new Color (cbase.getRed(),
                                cbase.getGreen(),
                                cbase.getBlue(),
                                colorAlpha
                               );
        setFillColor (cnew);

      /*
       * Set the fill pattern color.
       */
        cnew = new Color (0, 0, 0, patternAlpha);
        setFillPatternColor (cnew);

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set the fill color and pattern for subsequent polygon fills to the
  lithology specified.  The default scale is used for the patterns when
  this version of the method is called.
  The color alpha will be applied to the solid
  color and the pattern alpha will be applied to the pattern.
  This method will change the fill color and fill pattern set via
  their set methods.
  @param lithType The number of the lithology from the following options.
  <ul>
  <li> {@link DLConst#SANDSTONE}
  <li> {@link DLConst#SILTSTONE}
  <li> {@link DLConst#SHALE}
  <li> {@link DLConst#LIMESTONE}
  <li> {@link DLConst#DOLOMITE}
  <li> {@link DLConst#EVAPORITE}
  <li> {@link DLConst#COAL}
  <li> {@link DLConst#IGNEOUS}
  </ul>
  @param colorAlpha Alpha value for the solid fill color.  Set from zero (transparent)
  to 255 (opaque).
  @param patternAlpha Alpha value for the fill pattern.  Set from zero (transparent)
  to 255 (opaque).
 */
    public int setLithFill (int lithType,
                            int colorAlpha,
                            int patternAlpha)
    {
        if (lithType < 0  ||  lithType >= N_LITHOLOGY_TYPES) {
            return -1;
        }

        if (colorAlpha < 0) colorAlpha = 0;
        if (colorAlpha > 255) colorAlpha = 255;
        if (patternAlpha < 0) patternAlpha = 0;
        if (patternAlpha > 255) patternAlpha = 255;

      /*
       * Fill patterns start at 1, not zero.
       */
        setFillPattern (lithType + 1, 1.0);

      /*
       * Set the fill color.
       */
        Color cbase = LithFill.lithologyColors[lithType];
        Color cnew = new Color (cbase.getRed(),
                                cbase.getGreen(),
                                cbase.getBlue(),
                                colorAlpha
                               );
        setFillColor (cnew);

      /*
       * Set the fill pattern color.
       */
        cnew = new Color (0, 0, 0, patternAlpha);
        setFillPatternColor (cnew);

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set an opaque color to be used for filling subsequent polygons,
  boxes and arcs.  If you do not want any solid fill in these
  objects, set any of the color coordinates to -1.
  @param red The red color coordinate (0 - 255)
  @param green The green color coordinate (0 - 255)
  @param blue The blue color coordinate (0 - 255)
   */
    public int setFillColor (int red, int green, int blue)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = 255;
        Ilist[4] = GTX_SET_FILL_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentFillRed = red;
        currentFillGreen = green;
        currentFillBlue = blue;
        currentFillAlpha = 255;

        return 1;

    }

/*-------------------------------------------------------------

  /**
  Set an opaque or transparent color to be used for filling subsequent polygons,
  boxes and arcs.  If you do not want any solid fill in these
  objects, set any of the color coordinates to -1.
  @param red The red color coordinate (0 - 255)
  @param green The green color coordinate (0 - 255)
  @param blue The blue color coordinate (0 - 255)
  @param alpha The alpha color coordinate (0 - 255)
   */
    public int setFillColor (int red, int green, int blue, int alpha)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = alpha;
        Ilist[4] = GTX_SET_FILL_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentFillRed = red;
        currentFillGreen = green;
        currentFillBlue = blue;
        currentFillAlpha = alpha;

        return 1;

    }

/*-------------------------------------------------------------

  /**
  Set an opaque or transparent color to be used for filling subsequent polygons,
  boxes and arcs.  If you do not want any solid fill in these
  objects, specify the color as null.
  @param color Color object with the color for polygon fills.
   */
    public int setFillColor (Color color)
    {
        if (color == null) {
            Ilist[0] = -1;
            Ilist[1] = -1;
            Ilist[2] = -1;
            Ilist[3] = -1;
        }
        else {
            Ilist[0] = color.getRed();
            Ilist[1] = color.getGreen();
            Ilist[2] = color.getBlue();
            Ilist[3] = color.getAlpha();
        }
        Ilist[4] = GTX_SET_FILL_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentFillRed = Ilist[0];
        currentFillGreen = Ilist[1];
        currentFillBlue = Ilist[2];
        currentFillAlpha = Ilist[3];

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set an opaque color to be used for drawing the borders of subsequent polygons,
  boxes and arcs.  If you set any of the color coordinates to -1, then
  the current default foreground color is used.
  @param red The red color coordinate (0 - 255)
  @param green The green color coordinate (0 - 255)
  @param blue The blue color coordinate (0 - 255)
   */
    public int setBorderColor (int red, int green, int blue)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = 255;
        Ilist[4] = GTX_SET_BORDER_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentBorderRed = Ilist[0];
        currentBorderGreen = Ilist[1];
        currentBorderBlue = Ilist[2];
        currentBorderAlpha = Ilist[3];

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set an opaque or transparent color to be used for drawing the borders of subsequent polygons,
  boxes and arcs.  If you set any of the color coordinates to -1, then
  the current default foreground color is used.
  @param red The red color coordinate (0 - 255)
  @param green The green color coordinate (0 - 255)
  @param blue The blue color coordinate (0 - 255)
  @param alpha The alpha color coordinate (0 - 255)
   */
    public int setBorderColor (int red, int green, int blue, int alpha)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = alpha;
        Ilist[4] = GTX_SET_BORDER_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentBorderRed = Ilist[0];
        currentBorderGreen = Ilist[1];
        currentBorderBlue = Ilist[2];
        currentBorderAlpha = Ilist[3];

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set an opaque or transparent color to be used for drawing the borders of subsequent polygons,
  boxes and arcs.  If you specify a null color, then
  the current default foreground color is used.
  @param color The color for the pokygon borders.
   */
    public int setBorderColor (Color color)
    {
        if (color == null) {
            Ilist[0] = -1;
            Ilist[1] = -1;
            Ilist[2] = -1;
            Ilist[3] = -1;
        }
        else {
            Ilist[0] = color.getRed();
            Ilist[1] = color.getGreen();
            Ilist[2] = color.getBlue();
            Ilist[3] = color.getAlpha();
        }
        Ilist[4] = GTX_SET_BORDER_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentBorderRed = Ilist[0];
        currentBorderGreen = Ilist[1];
        currentBorderBlue = Ilist[2];
        currentBorderAlpha = Ilist[3];

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set the fill pattern and fill pattern size. The subsequent fills will
  use the pattern set here.  A pattern number of zero will switch back to
  solid fill only, which is the default.
  <p>
  You also need to have a valid fill pattern color specified via the
  {@link #setFillPatternColor} method.  If you never set the fill pattern
  color, then the default color will work.  However, if you set an invalid
  color via the setFillPatternColor method, no fill patterns will be used
  regardless of what pattern number you specify here.
  <p>
  The pattern scale simply expands or shrinks the pattern.  To expand,
  specify a patternScale greater than 1.  To shrink, specify a patternScale less
  than 1.  Vakis pattern scales are 0.1 to 9.99.  If you specify an invalid
  scale, the closest valid scale is used.
  <p>
  @param patternNumber  A valid fill pattern number or zero to only use solid fill color.
  <ul>
  <li> {@link DLConst#SANDSTONE}
  <li> {@link DLConst#SILTSTONE}
  <li> {@link DLConst#SHALE}
  <li> {@link DLConst#LIMESTONE}
  <li> {@link DLConst#DOLOMITE}
  <li> {@link DLConst#EVAPORITE}
  <li> {@link DLConst#COAL}
  <li> {@link DLConst#IGNEOUS}
  </ul>
  @param patternScale The scale to shrink or expand the base fill pattern (0.1 to 10.0).
  */
    public int setFillPattern (int patternNumber,
                               double patternScale)
    {
        if (patternScale < 0.1) patternScale = 0.1;
        if (patternScale > 9.99) patternScale = 9.99;
        Ilist[0] = patternNumber;
        Ddata[0] = patternScale;

        sendNativeCommand (
            GTX_SETFILLPATTERN,
            Ilist,
            Ddata
        );

        currentFillPattern = patternNumber;
        currentFillPatternScale = patternScale;

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set the font number for subsequent text.  Run the {@link UnitTest}
  application in the csw.jeasyx.src package to look at the available fonts.
  If this is not called, font 102 is used for small text and font 103 is
  used for large text.
  @param font Number of the font to use.
   */
    public int setFont (int font)
    {
        Ilist[0] = font;

        sendNativeCommand (
            GTX_SETFONT,
            Ilist
        );

        currentFont = font;

        return 1;

    }

/*-------------------------------------------------------------*/

 /**
 Set a Java font by specifying its name and style.  This font will be modified by the
 text size specified in subsequent calls to the {@link #addText} method.  If this is not called,
 stroke fonts will be used for subsequent text.
 @param fontName The font name as known to the system.
 @param fontStyle Either Font.PLAIN, Font.BOLD or Font.ITALIC or a logical or
of Font.BOLD|Font.ITALIC.
 */
    public int setFont (String fontName, int fontStyle)
    {
        if (fontName == null) {
            return -1;
        }
        int index = getExistingFontNumber (
            fontName,
            fontStyle);
        if (index < 0) {
            index = addNewFont (
                fontName,
                fontStyle);
        }

        currentFont = index + 1000;

        Ilist[0] = currentFont;

        sendNativeCommand (
            GTX_SETFONT,
            Ilist
        );

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
  Add a grid to the display list.  The grid can be contoured, color filled,
  have its nodes posted or any combination thereof.  The properties parameter
  (see {@link DLSurfaceProperties}) controls how the grid will be visualized.  If
  properties is null, a default surface properties object is constructed and used.
  The grid name should be unique within the grids for this display list.  If the grid parameter
  is null, nothing is added.  If the name is null, nothing is added.
  <p>
  Any color filling will be done with the most recent image colors defined with
  the {@link #setImageColors} method or with the {@link #setImageColorWindows} method.
  @param name The grid name.
  @param grid A {@link Grid} object with the data, faults and grid geometry.
  @param p A {@link DLSurfaceProperties} object specifying how to draw the grid.
  */
    public int addGrid (String name,
                        Grid   grid,
                        DLSurfaceProperties p)
    {

        int i, j, n, npts, nl, ntot;
        double[] x, y, z;

        if (name == null  ||
            grid == null) {
            return 0;
        }

        if (grid.getNCols () < 2  ||
            grid.getNRows () < 2) {
            return 0;
        }

        if (grid.getWidth() <= 0.0  ||
            grid.getHeight() <= 0.0) {
            return 0;
        }

        if (p == null) {
            p = new DLSurfaceProperties ();
        }

      /*
       * Fill in the appropriate arrays with the options and
       * send them to the native side.
       */
        Bdata[0] = p.showContours;
        Bdata[1] = p.showColorFills;
        Bdata[2] = p.showNodes;
        Bdata[3] = p.showNodeValues;
        Bdata[4] = p.showCellEdges;
        Bdata[5] = p.showFaultLines;
        Idata[6] = p.majorInterval;
        Idata[7] = p.tickMajor;
        Idata[8] = p.tickMinor;
        Idata[9] = p.majorColor.getRed();
        Idata[10] = p.majorColor.getGreen();
        Idata[11] = p.majorColor.getBlue();
        Idata[12] = p.majorColor.getAlpha();
        Idata[13] = p.minorColor.getRed();
        Idata[14] = p.minorColor.getGreen();
        Idata[15] = p.minorColor.getBlue();
        Idata[16] = p.minorColor.getAlpha();
        Idata[17] = p.nodeColor.getRed();
        Idata[18] = p.nodeColor.getGreen();
        Idata[19] = p.nodeColor.getBlue();
        Idata[20] = p.nodeColor.getAlpha();
        Idata[21] = p.nodeValueColor.getRed();
        Idata[22] = p.nodeValueColor.getGreen();
        Idata[23] = p.nodeValueColor.getBlue();
        Idata[24] = p.nodeValueColor.getAlpha();
        Idata[25] = p.cellEdgeColor.getRed();
        Idata[26] = p.cellEdgeColor.getGreen();
        Idata[27] = p.cellEdgeColor.getBlue();
        Idata[28] = p.cellEdgeColor.getAlpha();
        Idata[29] = p.faultLineColor.getRed();
        Idata[30] = p.faultLineColor.getGreen();
        Idata[31] = p.faultLineColor.getBlue();
        Idata[32] = p.faultLineColor.getAlpha();
        Idata[33] = p.contourFont;
        Idata[34] = p.nodeFont;
        Idata[35] = p.nodeSymbol;
        Bdata[36] = p.labelMajor;
        Bdata[37] = p.labelMinor;
        Bdata[38] = p.isThickness;
        Bdata[39] = p.showOutline;
        Idata[40] = p.outlineColor.getRed();
        Idata[41] = p.outlineColor.getGreen();
        Idata[42] = p.outlineColor.getBlue();
        Idata[43] = p.outlineColor.getAlpha();
        Idata[44] = p.contourSmoothing * 3;

        Ddata[0] = p.contourMinValue;
        Ddata[1] = p.contourMaxValue;
        Ddata[2] = p.contourInterval;
        Ddata[3] = p.dataLogBase;
        Ddata[4] = p.fillMinValue;
        Ddata[5] = p.fillMaxValue;
        Ddata[6] = p.minorThickness;
        Ddata[7] = p.majorThickness;
        Ddata[8] = p.cellEdgeThickness;
        Ddata[9] = p.faultThickness;
        Ddata[10] = p.minorLabelSize;
        Ddata[11] = p.majorLabelSize;
        Ddata[12] = p.minorLabelSpacing;
        Ddata[13] = p.majorLabelSpacing;
        Ddata[14] = p.minorTickLength;
        Ddata[15] = p.majorTickLength;
        Ddata[16] = p.minorTickSpacing;
        Ddata[17] = p.majorTickSpacing;
        Ddata[18] = p.nodeSymbolSize;
        Ddata[19] = p.nodeValueSize;
        Ddata[20] = p.hardMin;
        Ddata[21] = p.hardMax;
        Ddata[22] = p.zUnitsConversionFactor;

        sendNativeCommand (
            GTX_CONTOUR_PROPERTIES,
            null,
            null,
            Bdata,
            null,
            Idata,
            null,
            Ddata
        );

      /*
       * Send the fault data to the native side.
       */
        ArrayList<XYZPolyline> faults = grid.getFaults ();
        int nfaults = 0;
        if (faults != null) nfaults = faults.size();

        if (faults != null  &&  nfaults > 0) {
            // Calculate the array sizes to pass data to native side.
            Ilist[0] = nfaults;
            n = 0;
            Iterator<XYZPolyline> it = faults.iterator();
            while (it.hasNext()) {
                XYZPolyline fault = it.next();
                if (fault != null) {
                    n += fault.size();
                }
            }

            new_idata (n);
            new_ddata (3 * n);
            ntot = n;
            Ilist[1] = ntot;

            // Add fault data to these arrays
            n = 0;
            nl = 0;
            //it = faults.iterator();
            while (it.hasNext()) {
                XYZPolyline fault = it.next();
                if (fault == null) {
                    continue;
                }
                x = fault.getXArray ();
                y = fault.getYArray ();
                z = fault.getZArray ();
                if (x == null || y == null) {
                    continue;
                }
                npts = fault.size();
                Idata[nl] = npts;
                nl++;
                for (j=0; j<npts; j++) {
                    Ddata[n] =  x[j];
                    Ddata[n+ntot] = y[j];
                    if (z != null) {
                        Ddata[n+2*ntot] = z[j];
                    }
                    else {
                        Ddata[n+2*ntot] = 1.e30;
                    }
                    n++;
                }
            }

            sendNativeCommand (
                GTX_FAULT_LINE_DATA,
                Ilist,
                null,
                null,
                null,
                Idata,
                null,
                Ddata);
        }

      /*
       * Send the grid geometry and data to the native side.
       */
        Ilist[0] = grid.getNCols ();
        Ilist[1] = grid.getNRows ();
        ntot = Ilist[0] * Ilist[1];
        new_ddata (ntot);
        double[] gdata = grid.getNodeZArray ();

        Ddata[0] = grid.getXMin ();
        Ddata[1] = grid.getYMin ();
        Ddata[2] = grid.getWidth ();
        Ddata[3] = grid.getHeight ();
        Ddata[4] = grid.getRotationAngle ();

        for (i=0; i<ntot; i++) {
            Ddata[i+5] = gdata[i];
        }

        sendNativeCommand (
            GTX_GRID_DATA,
            Ilist,
            name,
            Ddata);

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
  Add a line to the display list.  The line will be drawn with the current
  line graphical attributes.
  @param x  Array of x coordinates.
  @param y  Array of y coordinates.
  @param npts Number of points.
  */
    public int addLine (double[] x, double[] y, int npts)
    {
        int         i;
        Ilist[0] = npts;
        new_ddata (npts * 2);
        for (i=0; i<npts; i++) {
            Ddata[i] = x[i];
            Ddata[npts+i] = y[i];
        }

        sendNativeCommand (
            GTX_DRAWLINE,
            Ilist,
            Ddata
        );

        logger.info ("    Added line with " + npts + " points    ");

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Specify if arrows are to be drawn at the last point of
  lines subsequently added to the display list.  Arrows, including the
  fill possibly used for the arrow, are always drawn in the line color.
  If this is never called, no arrows are drawn.
  @param style The arrow style from one of the following.
  <ul>
  <li> {@link #NO_ARROW}
  <li> {@link #SIMPLE_ARROW}
  <li> {@link #TRIANGLE_ARROW}
  <li> {@link #FILLED_TRIANGLE_ARROW}
  <li> {@link #OFFSET_TRIANGLE_ARROW}
  <li> {@link #FILLED_OFFSET_TRIANGLE_ARROW}
  </ul>
   */
    public int setLineArrow (int style)
    {
        Ilist[0] = style;

        sendNativeCommand (
            GTX_ARROW_STYLE,
            Ilist
        );

        currentArrowStyle = style;

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
  Set an opaque color to be used for drawing subsequent lines. If any of
  the color coordinates is set to -1, then the current foreground color is used.
  @param red The red color coordinate (0 - 255)
  @param green The green color coordinate (0 - 255)
  @param blue The blue color coordinate (0 - 255)
  */
    public int setLineColor (int red, int green, int blue)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = 255;
        Ilist[4] = GTX_SET_LINE_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentLineRed = Ilist[0];
        currentLineGreen = Ilist[1];
        currentLineBlue = Ilist[2];
        currentLineAlpha = Ilist[3];

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set an opaque or transparent color to be used for drawing subsequent lines. If any of
  the color coordinates is set to -1, then the current foreground color is used.
  @param red The red color coordinate (0 - 255)
  @param green The green color coordinate (0 - 255)
  @param blue The blue color coordinate (0 - 255)
  @param alpha The alpha color coordinate (0 - 255)
  */
    public int setLineColor (int red, int green, int blue, int alpha)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = alpha;
        Ilist[4] = GTX_SET_LINE_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentLineRed = Ilist[0];
        currentLineGreen = Ilist[1];
        currentLineBlue = Ilist[2];
        currentLineAlpha = Ilist[3];

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set an opaque or transparent color to be used for drawing subsequent lines. If
  a null color is specified, then the current foreground color is used.
  @param color The color object to use for the line color.
  */
    public int setLineColor (Color color)
    {
        if (color == null) {
            Ilist[0] = -1;
            Ilist[1] = -1;
            Ilist[2] = -1;
            Ilist[3] = -1;
        }
        else {
            Ilist[0] = color.getRed();
            Ilist[1] = color.getGreen();
            Ilist[2] = color.getBlue();
            Ilist[3] = color.getAlpha();
        }
        Ilist[4] = GTX_SET_LINE_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentLineRed = Ilist[0];
        currentLineGreen = Ilist[1];
        currentLineBlue = Ilist[2];
        currentLineAlpha = Ilist[3];

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set the line pattern and line pattern size.  There are 30 line patterns
  available, numbered from 1 to 30.  The {@link UnitTest} class can be run
  to see the available line patterns.  If you want a solid line, reset this
  to zero.  By default, solid lines are drawn.  If the patternNumber specified is
  not valid, solid lines are drawn.
  <p>
  The dashScale parameter is used to contract or expand the dash pattern.
  For scales under 1.0, the dashes will be shorter and closer together.
  For scales greater than 1.0, the dashes will be longer and further apart.
  The valid dash scales are 0.1 to 9.99.  If the specified scale is not
  valid, a scale of 1.0 will be used.
  @param patternNumber The line pattern number (1 - 30) or zero for solid lines.
  @param dashScale The scale factor for dashes (0.1 to 9.99).
   */
    public int setLinePattern (int patternNumber,
                               double dashScale)
    {
        if (patternNumber < 1  ||  patternNumber > 30) {
            patternNumber = 0;
        }
        if (dashScale < 0.1  ||  dashScale > 9.99) {
            dashScale = 1.0;
        }

        Ilist[0] = patternNumber;
        Ddata[0] = dashScale;

        sendNativeCommand (
            GTX_SETLINEPATTERN,
            Ilist,
            Ddata
        );

        currentLinePattern = patternNumber;
        currentLineDashScale = dashScale;

        return 1;

    }


/*-------------------------------------------------------------*/

  /**
  Set the smoothing factor for subsequent lines.  The value specified is
  roughly the number of additional points to introduce between original
  line points in an average length segment.  The smoothing is done using a
  spline fit to the original points.  Higher values will give more smoothing
  and lower values less smoothing.  For no smoothing, this can be set to zero.
  By default, no smoothing is done.  If the value is greater than 100, then
  100 is used.
  @param value Smoothing value (0 - 100)
  */
    public int setLineSmooth (int value)
    {
        if (value < 0) value = 0;
        if (value > 100) value = 100;
        Ilist[0] = value;

        sendNativeCommand (
            GTX_SETSMOOTH,
            Ilist
        );

        currentLineSmooth = value;

        return 1;

    }


/*-------------------------------------------------------------*/

  /**
  Set the thickness for subsequent lines, symbols and polygon borders.
  The thickness should be specified in device size units.  Valid thickness
  values are 0.001 to 10.0.  If the specified thickness is not valid, the
  nearest valid value is used.
  @param thickness Thickness for subsequent lines, in device size units.
  */
    public int setLineThickness (double thickness)
    {
        if (thickness < 0.001) thickness = 0.001;
        if (thickness > 10.0) thickness = 10.0;

        Ddata[0] = thickness;

        sendNativeCommand (
            GTX_SETLINETHICK,
            Ilist,
            Ddata
        );

        currentLineThickness = thickness;

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Add a text string representing the specified number to the
  display list.  This just formats a text string and draws the
  text string.  The text will be drawn using the current text
  graphics attributes.  The specified x and y location reflect the
  current text anchor position as specified via the {@link #setTextAnchor}
  method.  The actual visible position can also be affected by the offset
  specified via the {@link #setTextOffset} method.
  <p>
  The size is in device size units and the angle is in degrees.  The ndec
  parameter can specify the number of digits to the right of the decimal
  point.  If this is set to less than or equal to zero the number is drawn
  as an integer.
  <p>
  Also, if an integer number is drawn, and the commaFlag is
  specified as 1, commas will be inserted every 3 digits from the right.
  For example, without commaFlag, the string 1000000000 would be drawn.
  With commaFlag, the string 1,000,000,000 would be drawn.  The number has to be
  larger than 9999 or less than -9999 to get commas inserted.
  @param x The x coordinate for the number's current anchor position.
  @param y The y coordinate for the number's current anchor position.
  @param size The text size, in device size units.
  @param angle The rotation angle in degrees.
  @param value The actual number to draw.
  @param ndec Number of digits to the right of the decimal point.
  @param commaFlag Set to 1 for using commas, or zero for not using commas.
   */
    public int addNumber (double x,
                          double y,
                          double size,
                          double angle,
                          double value,
                          int ndec,
                          int commaFlag)
    {
        Ddata[0] = x;
        Ddata[1] = y;
        Ddata[2] = size;
        Ddata[3] = angle;
        Ddata[4] = value;
        Ilist[0] = ndec;
        Ilist[1] = commaFlag;

        sendNativeCommand (
            GTX_DRAWNUMBER,
            Ilist,
            Ddata
        );

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Static method to open the log file on the native side.  The log file can be
  played back by a native only utility program to debug problems.
  It is ok to keep calls to this method in production code, because the log file
  will be ignored on the native side if the code is compiled for production only.
  <p>
  The log file will be opened the first time you call this method and subsequent
  calls will be ignored.  Also, the log file is done on the native side.  The first
  call to this will cause logging of all commands for all display lists.  This is
  exactly what is needed for debugging via native only playback of the log file.
  You should always open the log file as soon as you get a JDisplayList object.
  That way you are assured that all commands will be logged.
  <p>
  If the file specified exists, it will be overwritten.
  <p>
  Returns 1 if the log file is open.  Returns -1 if an error occurred opening the file.
  @param filename Name of the new log file.
  */
    public static int openLogFile (String filename)
    {
        if (filename == null) {
            return -1;
        }

        int threadid = Thread.currentThread().hashCode();

        int status =
        JDisplayList.sendStaticCommand (
            -1,
            GTX_OPEN_LOG_FILE,
            threadid,
            null,
            null,
            filename,
            null,
            null,
            null,
            null,
            null,
            0
        );

        return status;

    }

/*-------------------------------------------------------------*/

  /**
  Set an opaque color used for subsequent polygon fill patterns.  This is only
  used if you have specified a fill pattern via the {@link #setFillPattern}
  method.   The valid range for each color coordinate is 0 to 255.
  If any of these color coordinates is set outside of the valid range, then
  no fill pattern will be drawn.  In other words, to draw fill patterns, you
  need to specify a valid pattern via setFillPattern and you also need to specify
  a valid color via this method.
 @param red The red color coordinate (0 - 255)
 @param green The green color coordinate (0 - 255)
 @param blue The blue color coordinate (0 - 255)
  */
    public int setFillPatternColor (int red, int green, int blue)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = 255;
        Ilist[4] = GTX_SET_PATTERN_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentFillPatternRed = Ilist[0];
        currentFillPatternGreen = Ilist[1];
        currentFillPatternBlue = Ilist[2];
        currentFillPatternAlpha = Ilist[3];

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set an opaque or transparent color used for subsequent polygon fill patterns.  This is only
  used if you have specified a fill pattern via the {@link #setFillPattern}
  method.   The valid range for each color coordinate is 0 to 255.
  If any of these color coordinates is set outside of the valid range, then
  no fill pattern will be drawn.  In other words, to draw fill patterns, you
  need to specify a valid pattern via setFillPattern and you also need to specify
  a valid color via this method.
 @param red The red color coordinate (0 - 255)
 @param green The green color coordinate (0 - 255)
 @param blue The blue color coordinate (0 - 255)
 @param alpha The alpha color coordinate (0 - 255)
  */
    public int setFillPatternColor (int red, int green, int blue, int alpha)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = alpha;
        Ilist[4] = GTX_SET_PATTERN_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentFillPatternRed = Ilist[0];
        currentFillPatternGreen = Ilist[1];
        currentFillPatternBlue = Ilist[2];
        currentFillPatternAlpha = Ilist[3];

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Set an opaque or transparent color used for subsequent polygon fill patterns.  This is only
  used if you have specified a fill pattern via the {@link #setFillPattern}
  method.   If a null color is specified, then no fill pattern will be drawn.
  In other words, to draw fill patterns, you
  need to specify a valid pattern via setFillPattern and you also need to specify
  a valid color via this method.
  @param color Color object with the fill pattern color in it.
  */
    public int setFillPatternColor (Color color)
    {
        if (color == null) {
            Ilist[0] = -1;
            Ilist[1] = -1;
            Ilist[2] = -1;
            Ilist[3] = -1;
        }
        else {
            Ilist[0] = color.getRed();
            Ilist[1] = color.getGreen();
            Ilist[2] = color.getBlue();
            Ilist[3] = color.getAlpha();
        }
        Ilist[4] = GTX_SET_PATTERN_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentFillPatternRed = Ilist[0];
        currentFillPatternGreen = Ilist[1];
        currentFillPatternBlue = Ilist[2];
        currentFillPatternAlpha = Ilist[3];

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
  Add a complex filled polygons to the display list.  Complex polygons are packed into the x and y arrays
  as a sequence of polygon components.  Each component has a number of points defining it.  For example,
  if a polygon has its main component and 2 holes, there are 3 components total.  In this case ncomp
  is 3.  If the main component has 10 points, the first hole has 5 points and the second hole has 7 points,
  then npts[0] is 10, npts[1] is 5 and npts[2] is 7.  x[0] to x[9] has the main x coordinates.  y[0] to y[9]
  has main y coordinates.  x[10 = x[14] has hole1 x and y[10] to y[14] has hole 1 y.  x[15] to x[20] has
  hole 2 x and y[15] t y[20] has hole 2 y.
  <p>
  The polygon will be filled with the current fill color and pattern.  If the outline flag is specified
  as 1, the polygon border will be drawn using the current line drawing graphic attributes.
  <p>
  This method is identical to the addFill method.
  @param x Packed x coordinates for all polygon components.
  @param y Packed y coordinates for all polygon components.
  @param npts Array with number of points per component.
  @param ncomp Number of components.
  @param outline true=draw outline, false=do not draw outline
   */
    public int addPolygon (double[] x,
                           double[] y,
                           int[] npts,
                           int ncomp,
                           boolean outline)

    {
        int        ntot, i;

        ntot = 0;
        for (i=0; i<ncomp; i++) {
            ntot += npts[i];
        }
        new_ddata ((ntot + ncomp) * 2);
        for (i=0; i<ntot; i++) {
            Ddata[i] = x[i];
            Ddata[ntot + i] = y[i];
        }
        for (i=0; i<ncomp; i++) {
            Idata[i] = npts[i];
        }

        Ilist[0] = ncomp;
        Ilist[1] = ntot;
        Ilist[2] = outline ? 1 : 0; // Ilist[2] expects an int flag

        sendNativeCommand (
            GTX_FILLPOLY,
            Ilist,
            null,
            null,
            null,
            Idata,
            null,
            Ddata
        );

        logger.info ("    Added polygon with " + ncomp + " components    ");

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
     Set the priority for subsequent drawing.  Frames with higher priority
     will be drawn after frames with lower priority.  The higher priority
     frames may overplot any lower priority frames.
     <p>
     This feature is patterned after PGL.  Priorities can range from 0 to 255.
     The following constants are provided for your use.
     <p>
     <li>      DRAW_PRIORITY_LOWEST    0
     <li>      DRAW_PRIORITY_LOW      64
     <li>      DRAW_PRIORITY_NORMAL  128
     <li>      DRAW_PRIORITY_HIGH    192
     <li>      DRAW_PRIORITY_HIGHEST 255
     <p>
     This feature is not yet hooked up, but it is in place if you want to add it
     to application code in anticipation of it working in the future.
     <p>
     Within each frame, the primitives are drawn in the following order:
     <p>
     <li>    All images are drawn first (lowest priority).
     <li>    Large polygon fills (those not used for text or symbols)
     <li>    Large arc fills (those not used for symbols)
     <li>    Large Lines (not used in text or symbols)
     <li>    Large Arcs (not used in symbols)
     <li>    Fills used in symbols
     <li>    Arc fills used in symbols
     <li>    Arcs used in symbols
     <li>    Lines used in symbols
     <li>    Fills used in text background
     <li>    Lines used in text background
     <li>    Fills used for text characters
     <li>    Lines used in text characters are drawn last (highest priority)
   */
    public int setFramePriority (int priority)
    {
        return 1;
    }

/*-------------------------------------------------------------*/

  /**
   Draw the current contents of the display list to the Java window.  You need to
   call this whenever you are temporarily or permanently done adding objects to
   the display list.  It will calculate all the very simple primitives and send
   them to Java for drawing.
  */
    public int redraw ()
    {

        sendNativeCommand (
            GTX_DRAW_CURRENT_VIEW,
            Ilist
        );

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
   Set a previously created frame for subsequent drawing.
   If you specify an invalid frame name, then no frame will
   be used for subsequent drawing.
  @param frameName Name of a previously created frame.
  */
    public int setFrame (String frameName)
    {

        sendNativeCommand (
            GTX_SETFRAME,
            Ilist,
            frameName
        );

        currentFrameName = frameName;

        return 1;
    }


/*-------------------------------------------------------------*/

  /**
   Set the clipping behavior of subsequently created frames.  Once a frame
   has its clipping set, it stays set.  This does not change frame clipping
   temporarily during the drawing process.  Frames are always clipped to their inside.
   By default, this is false, and frames are not clipped.
   <p>
   The frame clip flag is maintained after a call to the {@link #clear} method.
   You do not have to reset it after a call to clear.
   @param clipFlag  Set to true to have subsequent frames clip or set to false
   to have subsequent frames not clip.
   */
    public int setFrameClip (boolean clipFlag)
    {
        Ilist[0] = 1;
        currentFrameClipFlag = 1;
        if (clipFlag == false) {
            Ilist[0] = 0;
            currentFrameClipFlag = 0;
        }

        sendNativeCommand (
            GTX_SETFRAMECLIP,
            Ilist
        );


        return 1;
    }

/*-------------------------------------------------------------*/

  /**
   Set the clipping behavior of subsequently created frames.  Once a frame
   has its clipping set, it stays set.  This does not change frame clipping
   temporarily during the drawing process.  Frames are always clipped to their inside.
   By default, this is zero, and frames are not clipped.
   <p>
   The frame clip flag is maintained after a call to the {@link #clear} method.
   You do not have to reset it after a call to clear.
   @param clipFlag  Set to 1 to have subsequent frames clip or set to zero
   to have subsequent frames not clip.
   */
    public int setFrameClip (int clipFlag)
    {
        Ilist[0] = clipFlag;

        sendNativeCommand (
            GTX_SETFRAMECLIP,
            Ilist
        );

        currentFrameClipFlag = clipFlag;

        return 1;
    }

/*-------------------------------------------------------------*/

  /**
   Set continuous color bands for subsequent data images.  Specify a {@link ColorPalette}
   object and the data values to map to its first and last colors.
   For discreet color bands in an image, you
   need to use the {@link #setImageColorWindows} method.
 <p>
  If the palette object is null, a default palette is constructed and used.  The
  default palette has 256 opaque colors, from blue to cyan to green to yellow to red.
 <p>
  The colors set here will be used for subsequent data images, subsequent
  grid color fills and subsequent trimesh color fills.
  @param palette ColorPalette object with the colors to use.
  @param firstValue Data value for the first color in the palette.
  @param lastValue Data value for the last color in the palette.
   */
    public int setImageColors (ColorPalette palette,
                               double firstValue,
                               double lastValue)
    {
        int         nc, i;
        Color[]     colors;
        double      dz;

        if (palette == null) {
          palette = new ColorPalette();
        }

        nc = palette.getNColors ();
        if (nc < 1) {
            return -1;
        }
        colors = palette.getColors ();

        double minValue = Math.min(firstValue, lastValue);
        double maxValue = Math.max(firstValue, lastValue);

        double dz10 = maxValue - minValue;
        dz = dz10 / nc;

        for (i=0; i<nc; i++) {
          if (firstValue < lastValue) {
            Ddata[i] = firstValue + (i * dz);
            Ddata[nc+i] = Ddata[i] + dz;
          }
          else {
            Ddata[i] = firstValue - (i * dz);
            Ddata[nc+i] = Ddata[i] - dz;
          }
          Idata[i] = colors[i].getRed();
          Idata[nc+i] = colors[i].getGreen();
          Idata[2*nc+i] = colors[i].getBlue();
          Idata[3*nc+i] = colors[i].getAlpha();
        }

        Ddata[0] -= dz10;
        Ddata[2*nc-1] += dz10;

        Ilist[0] = nc;

        sendNativeCommand (
            GTX_IMAGECOLORBANDS,
            Ilist,
            null,
            null,
            null,
            Idata,
            null,
            Ddata
        );

        return 1;
    }

/*-------------------------------------------------------------*/

/**
  Set discreet opaque color windows for subsequent data images.  The windows are
  defined by data ranges and color values (red, green, blue).
 @param fmin Array of minimum data values for the color windows.
 @param fmax Array of maximum data values for the windows.
 @param red  Array of red color values for the windows (0 - 255)
 @param green Array of green color values for the windows (0 - 255)
 @param blue  Array of blue color values for the windows (0 - 255)
 @param ncolors Number of color windows.
*/
    public int setImageColorWindows (double[] fmin,
                                     double[] fmax,
                                     int[] red,
                                     int[] green,
                                     int[] blue,
                                     int ncolors)
    {
        int      i;

        if (ncolors < 1) {
            return 0;
        }

        Ilist[0] = ncolors;
        for (i=0; i<ncolors; i++) {
            Ddata[i] = fmin[i];
            Ddata[ncolors+i] = fmax[i];
            Idata[i] = red[i];
            Idata[ncolors+i] = green[i];
            Idata[2*ncolors+i] = blue[i];
            Idata[3*ncolors+i] = 255;
        }

        sendNativeCommand (
            GTX_IMAGECOLORBANDS,
            Ilist,
            null,
            null,
            null,
            Idata,
            null,
            Ddata
        );

        return 1;

    }
/*-------------------------------------------------------------*/

/**
  Set discreet transparent or opaque color windows for subsequent data images.  The windows are
  defined by data ranges and color values (red, green, blue, alpha). For alpha, 0 is transparent
  and 255 is opaque.
 @param fmin Array of minimum data values for the color windows.
 @param fmax Array of maximum data values for the windows.
 @param red  Array of red color values for the windows (0 - 255)
 @param green Array of green color values for the windows (0 - 255)
 @param blue  Array of blue color values for the windows (0 - 255)
 @param alpha  Array of alpha color values for the windows (0 - 255)
 @param ncolors Number of color windows.
*/
    public int setImageColorWindows (double[] fmin,
                                     double[] fmax,
                                     int[] red,
                                     int[] green,
                                     int[] blue,
                                     int[] alpha,
                                     int ncolors)
    {
        int      i;

        if (ncolors < 1) {
            return 0;
        }

        Ilist[0] = ncolors;
        for (i=0; i<ncolors; i++) {
            Ddata[i] = fmin[i];
            Ddata[ncolors+i] = fmax[i];
            Idata[i] = red[i];
            Idata[ncolors+i] = green[i];
            Idata[2*ncolors+i] = blue[i];
            Idata[3*ncolors+i] = alpha[i];
        }

        sendNativeCommand (
            GTX_IMAGECOLORBANDS,
            Ilist,
            null,
            null,
            null,
            Idata,
            null,
            Ddata
        );

        return 1;

    }

/*-------------------------------------------------------------*/

/**
  Set discreet transparent or opaque color windows for subsequent data images.  The windows are
  defined by data ranges and an array of color objects.  If any color object in the array is
  null, the range associated with the null color will be set to completely transparent.
 @param fmin Array of minimum data values for the color windows.
 @param fmax Array of maximum data values for the windows.
 @param colors Array of {@link Color} objects for the windows.
 @param ncolors Number of color windows.
*/
    public int setImageColorWindows (double[] fmin,
                                     double[] fmax,
                                     Color[] colors,
                                     int ncolors)
    {
        int      i;

        if (ncolors < 1) {
            return 0;
        }

        Ilist[0] = ncolors;
        for (i=0; i<ncolors; i++) {
            Ddata[i] = fmin[i];
            Ddata[ncolors+i] = fmax[i];
            if (colors[i] != null) {
                Idata[i] = colors[i].getRed();
                Idata[ncolors+i] = colors[i].getGreen();
                Idata[2*ncolors+i] = colors[i].getBlue();
                Idata[3*ncolors+i] = colors[i].getAlpha();
            }
            else {
                Idata[i] = 0;
                Idata[ncolors+i] = 0;
                Idata[2*ncolors+i] = 0;
                Idata[3*ncolors+i] = 0;
            }
        }

        sendNativeCommand (
            GTX_IMAGECOLORBANDS,
            Ilist,
            null,
            null,
            null,
            Idata,
            null,
            Ddata
        );

        return 1;

    }

/*-------------------------------------------------------------*/

  /**
     Set the name for subsequent images.
   */
    public int setImageName (String name)
    {

        sendNativeCommand (
            GTX_IMAGENAME,
            Ilist,
            name
        );

        return 1;
    }


/*-------------------------------------------------------------*/

  /**
     Set orientation flags and reporting options for subsequent images.
   */
    public int setImageOptions (int originflag,
                                int column1,
                                int row1,
                                int colspace,
                                int rowspace)
    {
        Ilist[0] = originflag;
        Ilist[1] = column1;
        Ilist[2] = row1;
        Ilist[3] = colspace;
        Ilist[4] = rowspace;

        sendNativeCommand (
            GTX_IMAGEOPTIONS,
            Ilist
        );

        return 1;
    }

/*----------------------------------------------------------------*/

  /**
    Set an item name to be used by subsequent graphics.
   */
    public int setItem (String name)
    {

        sendNativeCommand (
            GTX_SETITEM,
            Ilist,
            name
        );

        currentItemName = name;

        return 1;
    }

/*----------------------------------------------------------------*/

  /**
    Set a layer name to be used by subsequent graphics.
   */
    public int setLayer (String name)
    {

        sendNativeCommand (
            GTX_SETLAYER,
            Ilist,
            name
        );

        currentLayerName = name;

        return 1;
    }

/*----------------------------------------------------------------*/

  /**
    Add a symbol to the display list.
   */
    public int addSymbol (double x,
                          double y,
                          double size,
                          double angle,
                          int symbol)
    {
        Ilist[0] = symbol;
        Ddata[0] = x;
        Ddata[1] = y;
        Ddata[2] = size;
        Ddata[3] = angle;

        sendNativeCommand (
            GTX_DRAWSYMBOL,
            Ilist,
            Ddata
        );

        return 1;
    }


/*----------------------------------------------------------------*/

 /**
  Set an opaque color to be used for drawing subsequent symbols.
 @param red The red color coordinate (0 - 255)
 @param green The green color coordinate (0 - 255)
 @param blue The blue color coordinate (0 - 255)
 */
    public int setSymbolColor (int red, int green, int blue)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = 255;
        Ilist[4] = GTX_SET_SYMBOL_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentSymbolRed = Ilist[0];
        currentSymbolGreen = Ilist[1];
        currentSymbolBlue = Ilist[2];
        currentSymbolAlpha = Ilist[3];

        return 1;

    }

/*----------------------------------------------------------------*/

 /**
  Set an opaque or transparent color to be used for drawing subsequent symbols.
 @param red The red color coordinate (0 - 255)
 @param green The green color coordinate (0 - 255)
 @param blue The blue color coordinate (0 - 255)
 @param alpha The alpha color coordinate (0 - 255)
 */
    public int setSymbolColor (int red, int green, int blue, int alpha)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = alpha;
        Ilist[4] = GTX_SET_SYMBOL_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentSymbolRed = Ilist[0];
        currentSymbolGreen = Ilist[1];
        currentSymbolBlue = Ilist[2];
        currentSymbolAlpha = Ilist[3];

        return 1;

    }

/*----------------------------------------------------------------*/

 /**
  Set an opaque or transparent color to be used for drawing subsequent symbols.
  If a null color is specified, the foreground color is used.
 @param color A color object with the symbol colors in it.
 */
    public int setSymbolColor (Color color)
    {
        if (color == null) {
            Ilist[0] = -1;
            Ilist[1] = -1;
            Ilist[2] = -1;
            Ilist[3] = -1;
        }
        else {
            Ilist[0] = color.getRed();
            Ilist[1] = color.getGreen();
            Ilist[2] = color.getBlue();
            Ilist[3] = color.getAlpha();
        }
        Ilist[4] = GTX_SET_SYMBOL_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentSymbolRed = Ilist[0];
        currentSymbolGreen = Ilist[1];
        currentSymbolBlue = Ilist[2];
        currentSymbolAlpha = Ilist[3];

        return 1;

    }

/*----------------------------------------------------------------*/

  /**
    Set the anchor position for subsequent text and numbers.  The x and y coordinates
    specified in subsequent addText or addNumber calls will be the position of the
    anchor set here.  Also, rotation of subsequent text will be around the anchor
    specifed here.
    <p>
    @param anchor position on the text string where x and y are located
    <ui>
       <li>{@link #TEXT_BOTTOM_LEFT}
       <li>{@link #TEXT_BOTTOM_CENTER}
       <li>{@link #TEXT_BOTTOM_RIGHT}
       <li>{@link #TEXT_CENTER_LEFT}
       <li>{@link #TEXT_CENTER_CENTER}
       <li>{@link #TEXT_CENTER_RIGHT}
       <li>{@link #TEXT_TOP_LEFT}
       <li>{@link #TEXT_TOP_CENTER}
       <li>{@link #TEXT_TOP_RIGHT}
    </ui>
   */
    public int setTextAnchor (int anchor)
    {
        Ilist[0] = anchor;

        sendNativeCommand (
            GTX_TEXTANCHOR,
            Ilist
        );

        currentTextAnchor = anchor;

        return 1;
    }

/*----------------------------------------------------------------*/

  /**
    Set x and y offsets for subsequent text and number drawing.  These
    are specified in device size units.
    An example of using these would be to label symbols.
    You can specify an offset of the symbol size and an anchor of
    BOTTOM_CENTER and then use the same location for the text as for the
    symbol.  This would center the text or number above the symbol.
   */
    public int setTextOffset (double xoffset, double yoffset)
    {
        Ddata[0] = xoffset;
        Ddata[1] = yoffset;

        sendNativeCommand (
            GTX_TEXTOFFSETS,
            Ilist,
            Ddata
        );

        currentTextXOffset = xoffset;
        currentTextYOffset = yoffset;

        return 1;
    }


/*----------------------------------------------------------------*/
  /**
   Specify how the background for subsequent text should be drawn.
   The text can have a filled rectangle in the background.  This may
   be needed to be able to read the text in a crowded area of the drawing.
   If any style of filled background is specified, then the background
   will be colored using the current fill color and the current fill
   border color.  By default, this is set to zero, and no text background
   is drawn.
   @param background_flag Flag specifying text background style as follows:
   <ui>
   <li> {@link #TEXT_BG_NONE}
   <li> {@link #TEXT_BG_FILLED}
   <li> {@link #TEXT_BG_FILLED_BORDER}
   <li> {@link #TEXT_BG_CURVE_FILLED}
   <li> {@link #TEXT_BG_CURVE_FILLED_BORDER}
   </ui>
 */
    public int setTextBackground (int background_flag)
    {
        if (background_flag < 1  ||  background_flag > 4) {
            background_flag = 0;
        }

        Ilist[0] = background_flag;
        sendNativeCommand (
            GTX_TEXTBACKGROUND,
            Ilist,
            Ddata
        );

        currentTextBackgroundFlag = background_flag;

        return 1;
    }





/*----------------------------------------------------------------*/

  /**
  Add a single line text string to the
  display list.  The text will be drawn using the current text
  graphics attributes.  The specified x and y location reflect the
  current text anchor position as specified via the {@link #setTextAnchor}
  method.  The actual visible position can also be affected by the offset
  specified via the {@link #setTextOffset} method.
  The size is in device size units and the angle is in degrees.
  @param x The x coordinate for the number's current anchor position.
  @param y The y coordinate for the number's current anchor position.
  @param size The text size, in device size units.
  @param angle The rotation angle in degrees.
  @param text Single line of text to draw.
  */
    public int addText (double x,
                        double y,
                        double size,
                        double angle,
                        String text)
    {
        Ddata[0] = x;
        Ddata[1] = y;
        Ddata[2] = size;
        Ddata[3] = angle;

        sendNativeCommand (
            GTX_DRAWTEXT,
            Ilist,
            text,
            Ddata
        );

        return 1;

    }


/*----------------------------------------------------------------*/

  /**
  Set an opaque color to be used for drawing subsequent text outlines.
 @param red The red color coordinate (0 - 255)
 @param green The green color coordinate (0 - 255)
 @param blue The blue color coordinate (0 - 255)
   */
    public int setTextColor (int red, int green, int blue)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = 255;
        Ilist[4] = GTX_SET_TEXT_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentTextRed = Ilist[0];
        currentTextGreen = Ilist[1];
        currentTextBlue = Ilist[2];
        currentTextAlpha = Ilist[3];

        return 1;

    }

/*----------------------------------------------------------------*/

  /**
  Set an opaque or transparent opaque color to be used for drawing subsequent text outlines.
 @param red The red color coordinate (0 - 255)
 @param green The green color coordinate (0 - 255)
 @param blue The blue color coordinate (0 - 255)
 @param alpha The alpha color coordinate (0 - 255)
   */
    public int setTextColor (int red, int green, int blue, int alpha)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = alpha;
        Ilist[4] = GTX_SET_TEXT_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentTextRed = Ilist[0];
        currentTextGreen = Ilist[1];
        currentTextBlue = Ilist[2];
        currentTextAlpha = Ilist[3];

        return 1;

    }

/*----------------------------------------------------------------*/

  /**
  Set an opaque or transparent color to be used for drawing subsequent text outlines.
  If a null color is specified, the foreground color is used.
 @param color Color object with the text color in it.
   */
    public int setTextColor (Color color)
    {
        if (color == null) {
            Ilist[0] = -1;
            Ilist[1] = -1;
            Ilist[2] = -1;
            Ilist[3] = -1;
        }
        else {
            Ilist[0] = color.getRed();
            Ilist[1] = color.getGreen();
            Ilist[2] = color.getBlue();
            Ilist[3] = color.getAlpha();
        }
        Ilist[4] = GTX_SET_TEXT_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentTextRed = Ilist[0];
        currentTextGreen = Ilist[1];
        currentTextBlue = Ilist[2];
        currentTextAlpha = Ilist[3];

        return 1;

    }

/*----------------------------------------------------------------*/

  /**
  Set an opaque color to be used for drawing subsequent text fills.
 @param red The red color coordinate (0 - 255)
 @param green The green color coordinate (0 - 255)
 @param blue The blue color coordinate (0 - 255)
   */
    public int setTextFillColor (int red, int green, int blue)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = 255;
        Ilist[4] = GTX_SET_TEXTFILL_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentTextFillRed = Ilist[0];
        currentTextFillGreen = Ilist[1];
        currentTextFillBlue = Ilist[2];
        currentTextFillAlpha = Ilist[3];

        return 1;

    }

/*----------------------------------------------------------------*/

  /**
  Set an opaque or transparent color to be used for drawing subsequent text fills.
 @param red The red color coordinate (0 - 255)
 @param green The green color coordinate (0 - 255)
 @param blue The blue color coordinate (0 - 255)
 @param alpha The alpha color coordinate (0 - 255)
   */
    public int setTextFillColor (int red, int green, int blue, int alpha)
    {
        Ilist[0] = red;
        Ilist[1] = green;
        Ilist[2] = blue;
        Ilist[3] = alpha;
        Ilist[4] = GTX_SET_TEXTFILL_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentTextFillRed = Ilist[0];
        currentTextFillGreen = Ilist[1];
        currentTextFillBlue = Ilist[2];
        currentTextFillAlpha = Ilist[3];

        return 1;

    }

/*----------------------------------------------------------------*/

  /**
  Set an opaque or transparent color to be used for drawing subsequent text fills.
  If a null color is specified, no text fill is done.
 @param color Color object with the text fill color in it.
   */
    public int setTextFillColor (Color color)
    {
        if (color == null) {
            Ilist[0] = -1;
            Ilist[1] = -1;
            Ilist[2] = -1;
            Ilist[3] = -1;
        }
        else {
            Ilist[0] = color.getRed();
            Ilist[1] = color.getGreen();
            Ilist[2] = color.getBlue();
            Ilist[3] = color.getAlpha();
        }
        Ilist[4] = GTX_SET_TEXTFILL_COLOR_FLAG;

        sendNativeCommand (
            GTX_SETCOLOR,
            Ilist
        );

        currentTextFillRed = Ilist[0];
        currentTextFillGreen = Ilist[1];
        currentTextFillBlue = Ilist[2];
        currentTextFillAlpha = Ilist[3];

        return 1;

    }

/*----------------------------------------------------------------*/

  /**
     Set the thickness for subsequent text character lines.
   */
    public int setTextThickness (double thick)
    {
        Ddata[0] = thick;

        sendNativeCommand (
            GTX_SETTEXTTHICK,
            Ilist,
            Ddata
        );

        currentTextThickness = thick;

        return 1;

    }

/*----------------------------------------------------------------*/

  /**
    Unset the current layer, replacing it with nothing.
   */
    public int unsetLayer ()
    {
        sendNativeCommand (
            GTX_UNSETLAYER,
            Ilist,
            Ddata
        );

        currentLayerName = null;

        return 1;
    }

/*----------------------------------------------------------------*/

  /**
    Unset the current item, replacing it with nothing.
   */
    public int unsetItem ()
    {
        sendNativeCommand (
            GTX_UNSETITEM,
            Ilist,
            Ddata
        );

        currentItemName = null;

        return 1;
    }


/*----------------------------------------------------------------*/

  /**
    Unset the current frame, replacing it with nothing.
   */
    public int unsetFrame ()
    {
        sendNativeCommand (
            GTX_UNSETFRAME,
            Ilist,
            Ddata
        );

        currentFrameName = null;

        return 1;
    }


/*-------------------------------------------------------------*/

  /**
   * Cleanup all the native side resources associated with this display list.
   * This should only be called from the JDisplayListPanel when it needs to
   * cleanup itself.  The method is not accessible outside the package.
   */
    void cleanup () {

        if (nativeDlistID == -1) {
            return;
        }

        Ilist[0] = nativeDlistID;
        sendNativeCommand (
            GTX_DELETEWINDOW,
            Ilist
        );

        beginPlotCalled = false;
        nativeDlistID = -1;
        selectableList = null;
        return;
    }



/*-------------------------------------------------------------*/

  /**
  Add a trimesh to the display list.  The trimesh can be contoured, color filled,
  have its nodes posted or any combination thereof.  The properties parameter
  (see {@link DLSurfaceProperties}) controls how the trimesh will be visualized.  If
  properties is null, a default surface properties object is constructed and used.
  The trimesh name should be unique within the trimeshes for this display list.  If the trimesh parameter
  is null, nothing is added.  If the name is null, nothing is added.
  <p>
  Any color filling will be done with the most recent image colors defined with
  the {@link #setImageColors} method or with the {@link #setImageColorWindows} method.
  @param name The trimesh name.
  @param tmesh A {@link TriMesh} object with the data.
  @param p A {@link DLSurfaceProperties} object specifying how to draw the trimesh.
  */

    public int addTriMesh (String name,
                           TriMesh   tmesh,
                           DLSurfaceProperties p)
    {

        int i, ntot;

        if (name == null  ||
            tmesh == null) {
            return 0;
        }

        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();

        if (tmesh.getNumNodes () < 3  ||
            tmesh.getNumEdges () < 3  ||
            tmesh.getNumTriangles () < 1) {
            return 0;
        }

        if (p == null) {
            p = new DLSurfaceProperties ();
        }

      /*
       * Fill in the appropriate arrays with the options and
       * send them to the native side.
       */
        Bdata[0] = p.showContours;
        Bdata[1] = p.showColorFills;
        Bdata[2] = p.showNodes;
        Bdata[3] = p.showNodeValues;
        Bdata[4] = p.showCellEdges;
        Bdata[5] = p.showFaultLines;
        Idata[6] = p.majorInterval;
        Idata[7] = p.tickMajor;
        Idata[8] = p.tickMinor;
        Idata[9] = p.majorColor.getRed();
        Idata[10] = p.majorColor.getGreen();
        Idata[11] = p.majorColor.getBlue();
        Idata[12] = p.majorColor.getAlpha();
        Idata[13] = p.minorColor.getRed();
        Idata[14] = p.minorColor.getGreen();
        Idata[15] = p.minorColor.getBlue();
        Idata[16] = p.minorColor.getAlpha();
        Idata[17] = p.nodeColor.getRed();
        Idata[18] = p.nodeColor.getGreen();
        Idata[19] = p.nodeColor.getBlue();
        Idata[20] = p.nodeColor.getAlpha();
        Idata[21] = p.nodeValueColor.getRed();
        Idata[22] = p.nodeValueColor.getGreen();
        Idata[23] = p.nodeValueColor.getBlue();
        Idata[24] = p.nodeValueColor.getAlpha();
        Idata[25] = p.cellEdgeColor.getRed();
        Idata[26] = p.cellEdgeColor.getGreen();
        Idata[27] = p.cellEdgeColor.getBlue();
        Idata[28] = p.cellEdgeColor.getAlpha();
        Idata[29] = p.faultLineColor.getRed();
        Idata[30] = p.faultLineColor.getGreen();
        Idata[31] = p.faultLineColor.getBlue();
        Idata[32] = p.faultLineColor.getAlpha();
        Idata[33] = p.contourFont;
        Idata[34] = p.nodeFont;
        Idata[35] = p.nodeSymbol;
        Bdata[36] = p.labelMajor;
        Bdata[37] = p.labelMinor;
        Bdata[38] = p.isThickness;
        Bdata[39] = p.showOutline;
        Idata[40] = p.outlineColor.getRed();
        Idata[41] = p.outlineColor.getGreen();
        Idata[42] = p.outlineColor.getBlue();
        Idata[43] = p.outlineColor.getAlpha();
        Idata[44] = p.contourSmoothing * 3;

        Ddata[0] = p.contourMinValue;
        Ddata[1] = p.contourMaxValue;
        Ddata[2] = p.contourInterval;
        Ddata[3] = p.dataLogBase;
        Ddata[4] = p.fillMinValue;
        Ddata[5] = p.fillMaxValue;
        Ddata[6] = p.minorThickness;
        Ddata[7] = p.majorThickness;
        Ddata[8] = p.cellEdgeThickness;
        Ddata[9] = p.faultThickness;
        Ddata[10] = p.minorLabelSize;
        Ddata[11] = p.majorLabelSize;
        Ddata[12] = p.minorLabelSpacing;
        Ddata[13] = p.majorLabelSpacing;
        Ddata[14] = p.minorTickLength;
        Ddata[15] = p.majorTickLength;
        Ddata[16] = p.minorTickSpacing;
        Ddata[17] = p.majorTickSpacing;
        Ddata[18] = p.nodeSymbolSize;
        Ddata[19] = p.nodeValueSize;
        Ddata[20] = p.hardMin;
        Ddata[21] = p.hardMax;
        Ddata[22] = p.zUnitsConversionFactor;

        sendNativeCommand (
            GTX_CONTOUR_PROPERTIES,
            null,
            null,
            Bdata,
            null,
            Idata,
            null,
            Ddata
        );

      /*
       * Send the trimesh data to the native side.
       */
        Ilist[0] = tmesh.getNumNodes ();
        Ilist[1] = tmesh.getNumEdges ();
        Ilist[2] = tmesh.getNumTriangles ();

        new_ddata (3 * Ilist[0]);
        new_idata (Ilist[0] + 5 * Ilist[1] + 4 * Ilist[2]);

        double[] xnode = tmesh.getNodeXArray ();
        double[] ynode = tmesh.getNodeYArray ();
        double[] znode = tmesh.getNodeZArray ();
        int[]    flags = tmesh.getNodeFlagArray ();

        ntot = Ilist[0];
        for (i=0; i<ntot; i++) {
            Ddata[i] = xnode[i];
            Ddata[i+ntot] = ynode[i];
            Ddata[i+2*ntot] = znode[i];
            Idata[i] = flags[i];
        }

        int n0;
        n0 = ntot;

        int[] n1 = tmesh.getEdgeNode0Array ();
        int[] n2 = tmesh.getEdgeNode1Array ();
        int[] t1 = tmesh.getEdgeTriangle0Array ();
        int[] t2 = tmesh.getEdgeTriangle1Array ();
        flags = tmesh.getEdgeFlagArray ();

        ntot = Ilist[1];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = n1[i];
            Idata[n0+ntot+i] = n2[i];
            Idata[n0+2*ntot+i] = t1[i];
            Idata[n0+3*ntot+i] = t2[i];
            Idata[n0+4*ntot+i] = flags[i];
        }

        n0 += 5 * ntot;
        int[] e1 = tmesh.getTriangleEdge0Array ();
        int[] e2 = tmesh.getTriangleEdge1Array ();
        int[] e3 = tmesh.getTriangleEdge2Array ();
        flags = tmesh.getTriangleFlagArray ();

        ntot = Ilist[2];
        for (i=0; i<ntot; i++) {
            Idata[n0+i] = e1[i];
            Idata[n0+i+ntot] = e2[i];
            Idata[n0+i+2*ntot] = e3[i];
            Idata[n0+i+3*ntot] = flags[i];
        }

        sendNativeCommand (
            GTX_TRIMESH_DATA,
            Ilist,
            name,
            null,
            null,
            Idata,
            null,
            Ddata);

        return 1;
    }

/*---------------------------------------------------------------------*/


  /**
  Set a {@link DLSelectable} object that will group the subsequent primitive
  calls into a single selectable item.  To unset the selectable, call this
  with a null object.
  */
    public void setSelectable (DLSelectable s)
    {
        if (s == null) {
            Ilist[0] = -1;
        }

        else {
            addSelectable (s);
            Ilist[0] = s.getNativeIndex ();
        }

        sendNativeCommand (GTX_SET_SELECTABLE,
                           Ilist) ;

        currentSelectable = s;

        return;
    }

  /**
  Permanently erase the primitives for the specified {@link DLSelectable} object.
  This is useful id an edit has changed the primitives and you want to erase the
  old primitives before drawing the new primitives.  If the selectable is null,
  an {@link IllegalArgumentException} is thrown.  If the selectable has not been
  set in the display list, an {@link IllegalStateException} is thrown.
  */
    public void eraseSelectable (DLSelectable s) {

        if (s == null) {
            throw
            new IllegalArgumentException
            ("A null DLSelectable object is specified for the eraseSelectable method.");
        }

        Ilist[0] = s.getNativeIndex ();
        if (Ilist[0] < 0) {
            throw
            new IllegalStateException
            ("An object not in the display list is specified for the eraseSelectable method.");
        }

        sendNativeCommand (GTX_ERASE_SELECTABLE,
                           Ilist);

        return;

    }


  /**
  Retrieve the current selectable object.  A null return value means that there
  is no selectable group at this time.
  */
    public DLSelectable getCurrentSelectable ()
    {
        return currentSelectable;
    }

    public void setSelected(DLSelectable s, boolean isSelected) {
      if (s == null) {
          throw
          new IllegalArgumentException
          ("A null DLSelectable object is specified for the pickSelectable method.");
      }

      Ilist[0] = s.getNativeIndex ();
      if (Ilist[0] < 0) {
          throw
          new IllegalStateException
          ("An object not in the display list is specified for the pickSelectable method.");
      }

      clearSelectedPrims ();

      if (isSelected) {
          Ilist[1] = 1;
          s.isSelected = true;
      } else {
          Ilist[1] = 0;
          s.isSelected = false;
      }

      sendNativeCommand (GTX_SET_SELECT_STATE,
                         Ilist);

      callSelectListeners ();

      return;
    }


/*-------------------------------------------------------------*/

  /**
    Set an opaque select color.
    This color is used for drawing selected objects.  The default is
    opaque cyan.
  @param red Red component of the select color (0 - 255)
  @param green Green component of the select color (0 - 255)
  @param blue Blue component of the select color (0 - 255)
   */
    public void setSelectColor (int red, int green, int blue)
    {
      setSelectColor(
        red,
        green,
        blue,
        255
      );
    }

/*-------------------------------------------------------------*/

  /**
    Set the select color.
    This color is used for drawing selected objects.  The default is
    opaque cyan.
  @param red Red component of the select color (0 - 255)
  @param green Green component of the select color (0 - 255)
  @param blue Blue component of the select color (0 - 255)
  @param alpha Alpha component of the select color (0 - 255)
   */
    public void setSelectColor (int red, int green, int blue, int alpha)
    {
        if (red < 0  ||  red > 255  ||
            green < 0  ||  green > 255  ||
            blue < 0  ||  blue > 255  ||
            alpha < 0  ||  alpha > 255) {
            selectRed = 230;
            selectGreen = 230;
            selectBlue = 230;
            selectAlpha = 255;
        }
        else {
            selectRed = red;
            selectGreen = green;
            selectBlue = blue;
            selectAlpha = alpha;
        }

        return;
    }

/*-------------------------------------------------------------*/

  /**
    Set an opaque or transparent select color.
    This color is used for drawing selected objects.  The default is
    opaque cyan.  If a null color object is specified, the default
    will be used.
  @param bgcolor Color object for the select
   */
    public void setSelectColor (Color bgcolor)
    {
      if (bgcolor == null) {
        setSelectColor(
          0, 255, 255, 255
        );
      }
      else {
        setSelectColor(
          bgcolor.getRed(),
          bgcolor.getGreen(),
          bgcolor.getBlue(),
          bgcolor.getAlpha()
        );
      }
    }

/*-------------------------------------------------------------*/
  /**
  Create a new line by picking points with the mouse.  The {@link DLEditListener}
  parameter must be specified in order to get the results back to the application.  If the
  feedback listener is null, an {@link IllegalArgumentException} is thrown.
  This method allows you to set your own {@link JLineEditor} object.
  If the jle object is null, a default line editor is used.
  <p>
  When the user is finished creating the new line, the editFinished method of
  the feedback listener is called.
  <p>
  The new line will return all of the line graphic attributes currently set as part
  of the {@link DLSelectable} supplied to the listener methods.
  */
  public void editSelectedLineGeometry (DLEditListener fb,
                                        JLineEditor jle, DLSelectable selectable)
    {
        if (fb == null) {
            throw new IllegalArgumentException
                ("Calling pickNewLineGeometry with a null feedback listener.");
        }

        JLineEditor jl;
        if (jle == null) {
            jl = new JLineEditor
            (this, dlPanel, fb, selectable.nativeIndex, JLineEditor.LINE_EDITING);
        }
        else {
            jl = jle;
        }

        setEditor (jl);

    }

  /**
  Create a new line by picking points with the mouse.  The {@link DLEditListener}
  parameter must be specified in order to get the results back to the application.  If the
  feedback listener is null, an {@link IllegalArgumentException} is thrown.
  This method allows you to set your own {@link JLineEditor} object.
  If the jle object is null, a default line editor is used.
  <p>
  When the user is finished creating the new line, the editFinished method of
  the feedback listener is called.
  <p>
  The new line will return all of the line graphic attributes currently set as part
  of the {@link DLSelectable} supplied to the listener methods.
  */
    public void pickNewLineGeometry (DLEditListener fb,
                                     JLineEditor jle)
    {
        if (fb == null) {
            throw new IllegalArgumentException
                ("Calling pickNewLineGeometry with a null feedback listener.");
        }

        JLineEditor jl;
        if (jle == null) {
            jl = new JLineEditor
            (this, dlPanel, fb, -1, JLineEditor.LINE_PICKING);
        }
        else {
            jl = jle;
        }

        setEditor (jl);

    }

    public void pickNewPointGeometry(DLEditListener fb, JLineEditor jle) {
        if (fb == null) {
          throw new IllegalArgumentException(
              "Calling pickNewLineGeometry with a null feedback listener.");
        }

        JLineEditor jl;
        if (jle == null) {
            jl = new JLineEditor(this, dlPanel, fb, -1, JLineEditor.POINT_PICKING);
        } else {
            jl = jle;
        }

        setEditor(jl);

    }



    public void pickNewFunction(DLEditListener fb) {
        if (fb == null) {
            throw new IllegalArgumentException
              ("Calling pickNewLineGeometry with a null feedback listener.");
        }

        JLineEditor jl =
          new JLineEditor(this, dlPanel, fb, -1, JLineEditor.LINE_PICKING_FUNCTION);
        setEditor (jl);
    }


/*--------------------------------------------------------------------*/

  /**
  Create a new polygon by picking points with the mouse.  A {@link DLEditListener}
  must be specified.  If no edit listener is specified, an
  {@link IllegalArgumentException} is thrown.  The method allows the user to specify
  the {@link JLineEditor} object to use for picking the new polygon.  If a null line
  editor object is specified, a default object is used.  The line editor object
  will have its closedLine flag always set to true, regardless of whether it is
  specified or a default is used.
  <p>
  When the user is finished creating the new polygon, the editFinished method of
  the feedback listener is called.
  <p>
  The new polygon will return all of the line graphic attributes currently set as part
  of the {@link DLSelectable} supplied to the listener methods.
  <p>
  This method is identical to the pickNewFillGeometry method.
  */
    public void pickNewPolygonGeometry (DLEditListener fb,
                                        JLineEditor jle)
    {
        if (fb == null) {
            throw new IllegalArgumentException
                ("Calling pickNewPolygonGeometry with a null feedback listener.");
        }

        JLineEditor jl;

        if (jle == null) {
            jl = new JLineEditor
            (this, dlPanel, fb, -1, JLineEditor.LINE_PICKING_POLYGON);
        }
        else {
            jl = jle;
        }
        jl.setClosedLine (true);

        setEditor (jl);

    }

/*--------------------------------------------------------------------*/

  /**
  Create a new polygon by picking points with the mouse.  A {@link DLEditListener}
  must be specified.  If no edit listener is specified, an
  {@link IllegalArgumentException} is thrown.  The method allows the user to specify
  the {@link JLineEditor} object to use for picking the new polygon.  If a null line
  editor object is specified, a default object is used.  The line editor object
  will have its closedLine flag always set to true, regardless of whether it is
  specified or a default is used.
  <p>
  When the user is finished creating the new polygon, the editFinished method of
  the feedback listener is called.
  <p>
  The new polygon will return all of the line graphic attributes currently set as part
  of the {@link DLSelectable} supplied to the listener methods.
  <p>
  This method is identical to the pickNewPolygonGeometry method.
  */
    public void pickNewFillGeometry (DLEditListener fb,
                                     JLineEditor jle)
    {
        if (fb == null) {
            throw new IllegalArgumentException
                ("Calling pickNewFillGeometry with a null feedback listener.");
        }

        JLineEditor jl;

        if (jle == null) {
            jl = new JLineEditor
            (this, dlPanel, fb, -1, JLineEditor.LINE_PICKING_POLYGON);
        }
        else {
            jl = jle;
        }
        jl.setClosedLine (true);

        setEditor (jl);

    }

/*-----------------------------------------------------------------------*/

/*
 * Note that the zpList member is in the JDisplayListBase class.
 */
  /**
  Add a listener for zoom/pan changes.
  */
    public void addZoomPanListener (DLZoomPanListener zpl)
    {
        // do not add a null listener or one that has already been added
        if (zpl == null || zpList.contains(zpl)) {
            return;
        }

        zpList.add (zpl);
        return;
    }

  /**
  Remove a listener for zoom/pan changes.
  */
    public void removeZoomPanListener (DLZoomPanListener zpl)
    {
        int index = zpList.indexOf (zpl);
        if (index < 0) {
            return;
        }
        zpList.remove (index);
        return;
    }

    /**
     * Remove all zoom pan listeners.
     */
    public void removeAllZoomPanListeners() {
      zpList.clear();
    }

/*-------------------------------------------------------------------------*/

  /*
   * Saved versions of the current graphics properties.  Package scope
   * but they are only used by editor classes.
   */
    int
        currentFillRed = 0,
        currentFillGreen = 0,
        currentFillBlue = 0,
        currentFillAlpha = 255,
        currentFillPatternRed = 0,
        currentFillPatternGreen = 0,
        currentFillPatternBlue = 0,
        currentFillPatternAlpha = 255,
        currentBorderRed = 0,
        currentBorderGreen = 0,
        currentBorderBlue = 0,
        currentBorderAlpha = 255,
        currentLineRed = 0,
        currentLineGreen = 0,
        currentLineBlue = 0,
        currentLineAlpha = 255,
        currentSymbolRed = 0,
        currentSymbolGreen = 0,
        currentSymbolBlue = 0,
        currentSymbolAlpha = 255,
        currentTextRed = 0,
        currentTextGreen = 0,
        currentTextBlue = 0,
        currentTextAlpha = 255,
        currentTextFillRed = 0,
        currentTextFillGreen = 0,
        currentTextFillBlue = 0,
        currentTextFillAlpha = 255,

        currentFillPattern = -1,
        currentFont = -1,
        currentArrowStyle = 0,
        currentLinePattern = -1,
        currentLineSmooth = -1,
        currentLineSymbol = 0,

        currentFrameClipFlag = 0,
        currentTextAnchor = 1,
        currentTextBackgroundFlag = 0;

    DLSelectable
        currentSelectable = null;

    String
        currentFrameName = null,
        currentItemName = null,
        currentLayerName = null;

    double
        currentFillPatternScale = 1.0,
        currentLineDashScale = 1.0,
        currentLineThickness = .001,
        currentTextXOffset = 0.0,
        currentTextYOffset = 0.0,
        currentTextThickness = 0.0;

/*-----------------------------------------------------------------*/

    private void saveAllColors (int r, int g, int b, int a)
    {
        currentFillRed = r;
        currentFillGreen = g;
        currentFillBlue = b;
        currentFillAlpha = a;
        currentFillPatternRed = r;
        currentFillPatternGreen = g;
        currentFillPatternBlue = b;
        currentFillPatternAlpha = a;
        currentBorderRed = r;
        currentBorderGreen = g;
        currentBorderBlue = b;
        currentBorderAlpha = a;
        currentLineRed = r;
        currentLineGreen = g;
        currentLineBlue = b;
        currentLineAlpha = a;
        currentSymbolRed = r;
        currentSymbolGreen = g;
        currentSymbolBlue = b;
        currentSymbolAlpha = a;
        currentTextRed = r;
        currentTextGreen = g;
        currentTextBlue = b;
        currentTextAlpha = a;
        currentTextFillRed = r;
        currentTextFillGreen = g;
        currentTextFillBlue = b;
        currentTextFillAlpha = a;
    }

/*-------------------------------------------------------------------*/

    void populateLineAttributes (DLLine dline)
    {
        if (dline == null) {
            return;
        }

        dline.selectableObject = currentSelectable;
        dline.thickness = currentLineThickness;
        dline.dashScale = currentLineDashScale;
        dline.color = new Color (currentLineRed,
                                 currentLineGreen,
                                 currentLineBlue,
                                 currentLineAlpha);
        dline.frameName = currentFrameName;
        dline.layerName = currentLayerName;
        dline.itemName = currentItemName;
        dline.pattern = currentLinePattern;
        dline.symbol = currentLineSymbol;
        dline.arrowStyle = currentArrowStyle;
        dline.nativeIndex = -1;
    }

/*--------------------------------------------------------------------*/

    void readSelectable (int selNum)
    {
        Ilist[0] = selNum;
        sendNativeCommand (
            GTX_EDITPRIM,
            Ilist
        );
    }

/*-------------------------------------------------------------*/

  /**
     Define and set a drawing frame on the current drawing page.
     Subsequent location coordinates must be in the frame coordinate
     system until the frame is changed or unset.
  <p>
    This method can be used to create a complex frame that either stands
    on its own or that is attached to another frame.  A simpler method,
    {@link #createAttachedFrame createAttachFrame}, can be used for attaching legends, title
    blocks, etc to a stand alone "base" frame.  However, you must use this
    complex method to create the "base" frame before creating any frame
    that will attach to the "base" frame.
  <p>
    There is a yet deeper level of detail you can provide after the frame has
    been created.  You can call the {@link #setFrameAxisProperties setFrameAxisProperties} method to
    specify details of how labeled axes will be generated.  You need to
    specify border labeling in the frame creation in order to set the
    labeling detail later.
  <p>
    The expected limits of the frame (xmin, ymin to xmax, ymax) do not need
    to be exact.  If in doubt, err on the larger side.  However, try to be
    somewhat close.  These values are used to set up some frame resources at
    creation time and there are efficiency advantages to having the limits
    fairly close.
  <p>
    The page corners of the frame do not include borders.  Any attachments to
    the frame will automatically adjust for borders.  If the frame corner points
    (x1,y1 to x2,y2) do not have the same aspect ratio as the page corner points,
    the frame limits are adjusted to have the same aspect ratio (if a unit aspect
    ratio is specified via aspectFlag).
  <p>
    The frame will not participate in zooming and paning unless is is rescaleable.
    If you want to be able to drag on the border and move the frame, set moveable to
    1.  If the aspectFlag is not a valid constant, it is set to force unit aspect.
    If the borderFlag is not a valid constant, it is set to no border.  If the
    attachPosition is not a valis constant, it is set to no attach.
  <p>
    Captions will only be drawn on labelled axes.  You can control this more with the
    {@link #setFrameAxisProperties setFrameAxisProperties} method after creating the frame.  If any of the optional
    String parameters (horizontalCaption, verticalCaption, or attachFrameName) are not
    used, you should set them to null.
  <p>
    The attachPosition, extraGap and perpendicularMove parameters only have meaning
    if the frame is attached (the attachframeName is valid).
  <p>
    If any of the parameters are illegal, an {@link IllegalArgumentException} is thrown.
  @param name Unique name (in this display list) for the frame
  @param xminHint The expected minimum X coordinate of graphics drawn to the frame.
  @param yminHint The expected minimum Y coordinate of graphics drawn to the frame.
  @param xmaxHint The expected maximum X coordinate of graphics drawn to the frame.
  @param ymaxHint The expected maximum Y coordinate of graphics drawn to the frame.
  @param pxmin The page X coordinate of the lower left corner of the frame.
  @param pymin The page Y coordinate of the lower left corner of the frame.
  @param pxmax The page X coordinate of the upper right corner of the frame.
  @param pymax The page Y coordinate of the upper right corner of the frame.
  @param x1 The frame X coordinate initially mapped to the pxmin location.
  @param y1 The frame Y coordinate initially mapped to the pymin location.
  @param x2 The frame X coordinate initially mapped to the pxmax location.
  @param y2 The frame Y coordinate initially mapped to the pxmax location.
  @param rescaleable Set to one (1) to allow rescaling or zero (0) to not allow rescaling.
  @param moveable Set to one (1) to allow moving via the mouse or to zero (0) to not allow moving.
  @param aspectFlag Set to one of the following constants.
  <ul>
  <li> {@link #FORCE_UNIT_FRAME_ASPECT_RATIO}
  <li> {@link #ALLOW_ANY_FRAME_ASPECT_RATIO}
  </ul>
  @param borderFlag Set to one of the following constants.
  <ul>
  <li> {@link #FRAME_NO_BORDER}
  <li> {@link #FRAME_PLAIN_BORDER}
  <li> {@link #FRAME_LABEL_LEFT_BOTTOM_BORDER}
  <li> {@link #FRAME_LABEL_LEFT_TOP_BORDER}
  <li> {@link #FRAME_LABEL_RIGHT_BOTTOM_BORDER}
  <li> {@link #FRAME_LABEL_RIGHT_TOP_BORDER}
  <li> {@link #FRAME_LABEL_ALL_SIDES_BORDER}
  </ul>
  @param horizontalCaption Optional caption string for top and bottom axes, if they are labelled.
  @param verticalCaption Optional caption string for left and right axes, if they are labelled.
  @param attachFrameName Optional name of a frame to attach this frame to.
  @param attachPosition Set to one of the following constants.
  <ul>
  <li> {@link #FRAME_NO_ATTACH}
  <li> {@link #FRAME_ATTACH_LEFT_MIN}
  <li> {@link #FRAME_ATTACH_LEFT_MIDDLE}
  <li> {@link #FRAME_ATTACH_LEFT_MAX}
  <li> {@link #FRAME_ATTACH_TOP_MIN}
  <li> {@link #FRAME_ATTACH_TOP_MIDDLE}
  <li> {@link #FRAME_ATTACH_TOP_MAX}
  <li> {@link #FRAME_ATTACH_RIGHT_MIN}
  <li> {@link #FRAME_ATTACH_RIGHT_MIDDLE}
  <li> {@link #FRAME_ATTACH_RIGHT_MAX}
  <li> {@link #FRAME_ATTACH_BOTTOM_MIN}
  <li> {@link #FRAME_ATTACH_BOTTOM_MIDDLE}
  <li> {@link #FRAME_ATTACH_BOTTOM_MAX}
  </ul>
  @param extraGap This is the additional gap, in device size units, that you want to
                  insert between the base frame and this frame (if this frame is attached).
  @param perpendicularMove The movement, in device size units, that will be applied to
                           the frame in its anchored position.  If the frame is anchored on the left
                           or right, this movement will be vertical.  If anchored on the top or
                           bottom, the movement will be horizontal.
   */
    public int createFrame (
        String name,
        double xminHint,
        double yminHint,
        double xmaxHint,
        double ymaxHint,
        double pxmin,
        double pymin,
        double pxmax,
        double pymax,
        double x1,
        double y1,
        double x2,
        double y2,
        int rescaleable,
        int moveable,
        int aspectFlag,
        int borderFlag,
        String horizontalCaption,
        String verticalCaption,
        String attachFrameName,
        int attachPosition,
        double extraGap,
        double perpendicularMove)
    {

    /*
     * Exceptions for illegal arguments.
     */
        if (xminHint >= xmaxHint  ||  yminHint >= ymaxHint) {
            throw
            new IllegalArgumentException
            ("The expected minimums must be less than the expected maximums");
        }
        double tiny = (xmaxHint - xminHint + ymaxHint - yminHint) / 2000000.0;
        double dx = x2 - x1;
        double dy = y2 - y1;
        if (dx < 0.0) dx = -dx;
        if (dy < 0.0) dy = -dy;
        if (dx <= tiny  ||  dy <= tiny) {
            throw
            new IllegalArgumentException
            ("Either x1 equals x2 or y1 equals y2");
        }
        if (pxmin >= pxmax  ||  pymin >= pymax) {
            throw
            new IllegalArgumentException
            ("The page minimums must be less than the page maximums");
        }

        if (aspectFlag != ALLOW_ANY_FRAME_ASPECT_RATIO) {
            aspectFlag = 0;
        }
        Ilist[0] = borderFlag;
        Ilist[1] = rescaleable;
        Ilist[2] = moveable;
        Ilist[3] = aspectFlag;
        Ilist[4] = attachPosition;
        Ilist[5] = 0;
        Ilist[6] = 0;

        String str = name;
        str = str.concat (MSG_STRING_SEPARATOR);
        if (attachFrameName != null) {
            str = str.concat (attachFrameName);
        }
        str = str.concat (MSG_STRING_SEPARATOR);
        if (horizontalCaption != null) {
            str = str.concat (horizontalCaption);
        }
        str = str.concat (MSG_STRING_SEPARATOR);
        if (verticalCaption != null) {
            str = str.concat (verticalCaption);
        }

        Ddata[0] = x1;
        Ddata[1] = y1;
        Ddata[2] = x2;
        Ddata[3] = y2;
        Ddata[4] = pxmin;
        Ddata[5] = pymin;
        Ddata[6] = pxmax;
        Ddata[7] = pymax;
        Ddata[8] = xminHint;
        Ddata[9] = yminHint;
        Ddata[10] = xmaxHint;
        Ddata[11] = ymaxHint;
        Ddata[12] = extraGap;
        Ddata[13] = perpendicularMove;

        sendNativeCommand (
            GTX_CREATEFRAME,
            Ilist,
            str,
            Ddata
        );

        currentFrameName = name;

        return 1;

    }


/*---------------------------------------------------------------------*/

    /**
     * Returns the selectable list.
     */
    public ArrayList<DLSelectable> getSelectableList() {
      return selectableList;
    }

/*---------------------------------------------------------------------*/

}  // end of JDisplayList class definition


