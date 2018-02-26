
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

 /**
 *  A class that extends JPanel to enable drawing of JDisplayList
 *  info on a graphics2d object.  Only one JDisplayList can be
 *  drawn onto this type of container.  Also, a JDisplayList object
 *  cannot be shared between containers.
 *
 * @author Glenn Pinkerton
 */
package csw.jeasyx.src;

import java.awt.Color;
import java.awt.Cursor;
import java.awt.Frame;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.awt.event.InputEvent;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.awt.geom.Rectangle2D;

import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.SwingUtilities;
import javax.swing.WindowConstants;
import javax.swing.event.MouseInputAdapter;

import org.apache.logging.log4j.Logger;

import csw.jutils.src.CSWLogger;

//import java.util.Date;
//import org.apache.logging.log4j.LogManager;


/**
 This class provides a swing component that can be drawn to using
 {@link JDisplayList} methods.  The panel implicitly creates a
 JDisplayList object, which you can retrieve and use for drawing to
 the panel.
<p>
*/
public class JDisplayListPanel extends JPanel
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    private JDisplayList dlist = null;

    private int panMode = 0;
    private int zoomOutMode = 0;
    private int zoomInMode = 0;
    private int selectMode = 1;
    private int zoomExtentsMode = 0;
    private int zoomToRectangleMode = 0;
    private int zoomBordersFlag = 0;

    private JFrame parentFrame = null;
    private int isListening = 0;

/*---------------------------------------------------------------------------*/

/**
 Construct a graphics panel without zoom/pan controls.
 This constructor allows the panel to automatically determine its frame
 parent.  If the panel has no frame ancestor, it's native resources will
 not be disposed of until the panel is finalized.
*/
    public JDisplayListPanel ()
    {
        JDLPMouseListener   mouse_listen;
        parentFrame = null;
        setBackground (new Color (220, 220, 220, 255));
        setOpaque (true);
        dlist = new JDisplayList (this);

        mouse_listen = new JDLPMouseListener ();
        addMouseListener(mouse_listen);
        addMouseMotionListener(mouse_listen);

        addComponentListener (new JDLPComponentListener ());

        logger.info ("    JDisplayListPanel successfully constructed    ");
    }

/*---------------------------------------------------------------------------*/

  /**
  Return a reference to the JDisplayList object implicitly created by the panel.
  Any modifications done to this display list object will always be reflected in
  this panel.
  */
    public JDisplayList getDisplayList ()
    {
        return dlist;
    }

/*---------------------------------------------------------------------------*/

    void startPanMode ()
    {
        resetModes ();
        panMode = 1;
    }

/*---------------------------------------------------------------------------*/

    void startSelectMode ()
    {
        resetModes ();
        selectMode = 1;
        dlist.setEditor(null);
        setSelectCursor();
    }

/*---------------------------------------------------------------------------*/

    void startUnselectMode ()
    {
        resetModes ();
        dlist.unselectAll ();
        repaint ();
    }

/*---------------------------------------------------------------------------*/

    void startZoomExtentsMode ()
    {
        resetModes ();

        int iframe = dlist.getSingleRescaleFrame ();
        if (iframe != -1) {
            dlist.zoomToExtents (iframe);
            repaint ();
        }
        else {
            zoomExtentsMode = 1;
        }
    }

/*---------------------------------------------------------------------------*/

    void startZoomOutMode ()
    {
        resetModes ();

        int iframe = dlist.getSingleRescaleFrame ();
        if (iframe != -1) {
            dlist.zoomOut (iframe);
            repaint ();
        }
        else {
            zoomOutMode = 1;
        }
    }

/*---------------------------------------------------------------------------*/

    void startZoomInMode ()
    {
        resetModes ();
        zoomInMode = 1;
    }

/*---------------------------------------------------------------------------*/

    void startZoomToRectangleMode ()
    {
        resetModes ();
        zoomToRectangleMode = 1;
        zoomBordersFlag = 0;
    }

/*---------------------------------------------------------------------------*/

    void startZoomBordersToRectangleMode ()
    {
        resetModes ();
        zoomToRectangleMode = 1;
        zoomBordersFlag = 1;
    }

/*---------------------------------------------------------------------------*/

    void redraw ()
    {
        dlist.setNativeDrawNeeded (1);
        resetModes ();
        repaint ();
    }

/*---------------------------------------------------------------------------*/

    void resetModes ()
    {
        panMode = 0;
        zoomExtentsMode = 0;
        zoomOutMode = 0;
        zoomInMode = 0;
        zoomToRectangleMode = 0;
        zoomBordersFlag = 0;
        selectMode = 1;

        zoomStarted = 0;
        x1Zoom = 0;
        y1Zoom = 0;
        zoomFrame = -1;

        panStarted = 0;
        x1Pan = 0;
        y1Pan = 0;
        panFrame = -1;

        dlist.clearZoomPanShapes ();

        return;
    }

/*---------------------------------------------------------------------------*/

    public void setDigitizingCursor() {
      setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
    }

    public void setSelectCursor() {
      setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
    }

/*---------------------------------------------------------------------------*/

  /*
   * Override the panel's paintComponent method.  This sets the
   * screen viewport in the JDisplayList and then draws the
   * display list into that viewport.
   */
    public void paintComponent(Graphics g)
    {
        int          x, y, width, height;
        Graphics2D   g2d;

        if (dlist == null) {
            return;
        }

        if (parentFrame == null) {
            listenToFrame ();
        }

        g2d = (Graphics2D)g;

        x = dlist.getDeviceViewportXorigin ();
        y = dlist.getDeviceViewportYorigin ();
        width = dlist.getDeviceViewportWidth ();
        height = dlist.getDeviceViewportHeight ();

        if (x < 0  ||  y < 0  ||  width < 0  ||  height < 0) {
            x = 0;
            y = 0;
            width = getWidth ();
            height = getHeight ();
        }

            x = 0;
            y = 0;
            width = getWidth ();
            height = getHeight ();

        dlist.drawToDevice (g2d, x, y, width, height);

    }

/*---------------------------------------------------------------------------*/
  /**
  Clean up all the native resources associated with this panel.
  If the panel becomes unuseful without its ancestor frame being
  closed, you can call this method to clean up all the native
  resources used to support the drawing on the panel.
  <p>
  This method is automatically called if the parent frame is closed and
  if that frame is set to displose on close.  This is needed for timely
  cleanup of the native side resources.  One cannot use only the finalize
  method to clean up the native resources.  The finalize method may not
  be called in a timely fashion.  To quote Sun's documentation:
  "In practice, do not rely on the finalize method for recycling any
  resources that are in short supply - you simply cannot know when this
  method will be called."  Core Java, Volume 1, Fundamentals, page 131.
  */
    public void cleanup ()
    {
        if (dlist != null) {
            dlist.cleanup ();
        }
    }

    static void cleanup (JDisplayList dl)
    {
        if (dl != null) {
            dl.cleanup ();
        }
    }

/*---------------------------------------------------------------------------*/

    static ArrayList<JDisplayList>    finalizedDlist = 
        new ArrayList<JDisplayList> ();
    static boolean      cleanupNeeded = false;

  /**
  Schedule cleanup of the panel's native resources to be done in the
  event thread as soon as possible.
  */
    protected void finalize ()
    {

        if (dlist == null) {
            return;
        }

        finalizedDlist.add (dlist);

        if (cleanupNeeded) {
            return;
        }

        cleanupNeeded = true;

        Runnable cleanup = new Runnable ()
        {
            public void run ()
            {
                JDisplayListPanel.cleanupFinalizedPanel ();
            }
        };

        SwingUtilities.invokeLater (cleanup);

    }


    static private synchronized void cleanupFinalizedPanel ()
    {
        int         size, i;
        JDisplayList    dl;

        size = finalizedDlist.size ();
        for (i=0; i<size; i++) {
            dl = finalizedDlist.get (i);
            if (dl != null) {
                dl.cleanup ();
            }
        }
        finalizedDlist.clear ();
        cleanupNeeded = false;
    }

/*---------------------------------------------------------------------------*/

    private void listenToFrame ()
    {
        if (parentFrame == null) {
            findFrameParent ();
            if (parentFrame == null) {
                return;
            }
        }

        if (isListening == 0) {
            int operation = parentFrame.getDefaultCloseOperation ();
            if (operation == WindowConstants.DISPOSE_ON_CLOSE) {
                parentFrame.addWindowListener (new JDLPCleanupListener ());
            }
            isListening = 1;
        }

    }


    private void findFrameParent ()
    {
        boolean    istat;
        int        i, size;

        Frame[] farray = Frame.getFrames ();
        if (farray == null) {
            return;
        }

        size = farray.length;
        for (i=0; i<size; i++) {
            if (farray[i] == null) {
                continue;
            }
            istat = farray[i].isAncestorOf (this);
            if (istat) {
                parentFrame = (JFrame)farray[i];
                break;
            }
        }

        return;
    }

/*---------------------------------------------------------------------------*/

    private int     zoomStarted = 0;
    private int     x1Zoom = 0;
    private int     y1Zoom = 0;
    private int     zoomFrame = -1;
    private int     panStarted = 0;
    private int     x1Pan = 0;
    private int     y1Pan = 0;
    private int     panFrame = -1;

/*---------------------------------------------------------------------------*/

    private void startPan (MouseEvent e)
    {
        if (panStarted == 1) {
            return;
        }

        dlist.setNativeDrawNeeded (0);

        int ix = e.getX();
        int iy = e.getY();

        int iframe = dlist.findRescaleFrame (ix, iy);
        if (iframe == -1) {
            return;
        }

        panFrame = iframe;
        x1Pan = ix;
        y1Pan = iy;
        panStarted = 1;

        return;

    }

/*---------------------------------------------------------------------------*/

    private void endPan (MouseEvent e)
    {
        if (panStarted == 0) {
            return;
        }

        int ix = e.getX();
        int iy = e.getY();

        dlist.panFrame (panFrame,
                        x1Pan,
                        y1Pan,
                        ix,
                        iy);

        dlist.clearZoomPanShapes ();

        repaint ();

        resetModes ();

        return;

    }

/*---------------------------------------------------------------------------*/

    private void startRectangleZoom (MouseEvent e)
    {
        if (zoomStarted == 1) {
            return;
        }

        dlist.setNativeDrawNeeded (0);

        int ix = e.getX();
        int iy = e.getY();

        int iframe = dlist.findRescaleFrame (ix, iy);
        if (iframe == -1) {
            return;
        }

        zoomFrame = iframe;
        x1Zoom = ix;
        y1Zoom = iy;
        zoomStarted = 1;

        return;

    }

/*---------------------------------------------------------------------------*/

    private void moveRectangleZoom (MouseEvent e)
    {
        float          xmin, ymin, w, h;

        if (zoomStarted == 0) {
            return;
        }

        dlist.setNativeDrawNeeded (0);

        int ix = e.getX();
        int iy = e.getY();

        w = ix - x1Zoom;
        h = iy - y1Zoom;

        xmin = x1Zoom;
        ymin = y1Zoom;

        if (w < 0.0) {
            xmin = ix;
            w = -w;
        }
        if (h < 0.0) {
            ymin = iy;
            h = -h;
        }

        Rectangle2D.Float rect = new Rectangle2D.Float (xmin, ymin, w, h);
        dlist.clearZoomPanShapes ();
        dlist.addZoomPanShape (
            rect,
            0,  // red
            0,  // green
            0,  // blue
            0   // fill flag
        );

        repaint ();

        return;

    }

/*---------------------------------------------------------------------------*/

    private void endRectangleZoom (MouseEvent e)
    {
        if (zoomStarted == 0) {
            return;
        }

        int ix = e.getX();
        int iy = e.getY();

        int dx = ix - x1Zoom;
        if (dx < 0) dx = -dx;
        if (dx < 2) {
            ix = x1Zoom + 2;
        }
        dx = iy - y1Zoom;
        if (dx < 0) dx = -dx;
        if (dx < 2) {
            iy = y1Zoom + 2;
        }

System.out.println ("calling dlist zoomFrame");
System.out.flush ();

        dlist.zoomFrame (zoomFrame,
                         zoomBordersFlag,
                         x1Zoom,
                         y1Zoom,
                         ix,
                         iy);

        dlist.clearZoomPanShapes ();

        repaint ();

        resetModes ();

        return;

    }

/*---------------------------------------------------------------------------*/

/*
 * the cleanup listener will be called when the frame that owns the
 * display list panel disposes of itself.
 */
    private class JDLPCleanupListener implements WindowListener
    {
        public void windowClosing (WindowEvent e)
        {
            cleanup ();
        }

        public void windowClosed (WindowEvent e)
        {
            cleanup ();
        }

        public void windowIconified (WindowEvent e) {}
        public void windowDeiconified (WindowEvent e) {}
        public void windowActivated (WindowEvent e) {}
        public void windowDeactivated (WindowEvent e) {}
        public void windowOpened (WindowEvent e) {}
    };

/*---------------------------------------------------------------------------*/

/*
 * The component listener is needed to insure that a repaint is
 * always done when the panel is resized.
 */
    private class JDLPComponentListener extends ComponentAdapter
    {
        public void componentResized (ComponentEvent e)
        {

        /*
         * If the display list is using screen coordinates as its
         * "page units" then do not repaint in response to resize.
         * Any resize processing should be done by the application
         * when using screen coordinates as page units.
         */
            int type = dlist.getPageUnitsType ();
            if (type == 2) {
                return;
            }

            repaint ();
        }

        public void componentHidden (ComponentEvent e)
        {
        }

        public void componentMoved (ComponentEvent e)
        {
        }

        public void componentShown (ComponentEvent e)
        {
            repaint ();
        }
    }

/*---------------------------------------------------------------------------*/

/*
 * The mouse listener will interpret mouse events according
 * to the state of the display list panel.
 */
    private class JDLPMouseListener extends MouseInputAdapter
    {

        private int button_1, button_2, button_3;
        private boolean editFlag = false;

        private int b1X,
                    b1Y,
                    b2X,
                    b2Y,
                    b3X,
                    b3Y;

        private void setButtonID (MouseEvent e) {
            button_1 = e.getModifiers() & InputEvent.BUTTON1_MASK;
            button_2 = e.getModifiers() & InputEvent.BUTTON2_MASK;
            button_3 = e.getModifiers() & InputEvent.BUTTON3_MASK;
        }

        public void mouseClicked(MouseEvent e)
        {
        }

        private void localClick (MouseEvent e)
        {
            setButtonID (e);

          /*
           * If in zoom rectangle mode, it is possible
           * to start or end the zoom mode on a click.
           */
            if (zoomToRectangleMode == 1  &&  button_1 != 0) {
                int zflag = zoomStarted;
                startRectangleZoom (e);
                if (zflag == 1) {
                    endRectangleZoom (e);
                }
                return;
            }

          /*
           * In zoomExtentsMode, redraw to the complete limits of the
           * frame enclosing the click.
           */
            if (zoomExtentsMode == 1  &&  button_1 != 0) {
                zoomToExtents (e);
                return;
            }

            if (zoomOutMode == 1  &&  button_1 != 0) {
                zoomOut (e);
                return;
            }

            if (zoomInMode == 1  &&  button_1 != 0) {
                zoomIn (e);
                return;
            }

            if (panMode == 1  &&  button_1 != 0) {
                int pflag = panStarted;
                startPan (e);
                if (pflag == 1) {
                    endPan (e);
                }
                return;
            }

            editFlag = dlist.processMouseEvent (e,
                                                MouseEvent.MOUSE_CLICKED,
                                                button_1,
                                                button_2,
                                                button_3);

            if (editFlag == true) {
                return;
            }

          /*
           * Selection mode is on.
           */
            if (selectMode == 1) {
                selectObject (e, button_1, button_3);
            }

        }
        public void mouseDragged(MouseEvent e) {

            setButtonID (e);

          /*
           * If in zoom rectangle mode, it is possible to redraw the
           * zoom rectangle on a mouse drag.
           */
            if (zoomToRectangleMode == 1  &&  button_1 != 0) {
                moveRectangleZoom (e);
                return;
            }

            editFlag = dlist.processMouseEvent (e,
                                                MouseEvent.MOUSE_DRAGGED,
                                                button_1,
                                                button_2,
                                                button_3);
        }
        public void mouseMoved(MouseEvent e) {

          /*
           * If in zoom rectangle mode, it is possible to redraw the
           * zoom rectangle on a mouse drag.
           */
            if (zoomToRectangleMode == 1) {
                moveRectangleZoom (e);
                return;
            }

            editFlag = dlist.processMouseEvent (e,
                                                MouseEvent.MOUSE_MOVED,
                                                button_1,
                                                button_2,
                                                button_3);

        }
        public void mousePressed(MouseEvent e) {

            setButtonID (e);

            if (button_1 != 0) {
                b1X = e.getX();
                b1Y = e.getY();
            }
            if (button_2 != 0) {
                b2X = e.getX();
                b2Y = e.getY();
            }
            if (button_3 != 0) {
                b3X = e.getX();
                b3Y = e.getY();
            }

          /*
           * If in zoom rectangle mode, it is possible
           * to start zoom mode on a press.
           */
            if (zoomToRectangleMode == 1  &&  button_1 != 0) {
                if (zoomStarted == 0) {
                    startRectangleZoom (e);
                    zoomPress = 1;
                    return;
                }
            }
            editFlag = dlist.processMouseEvent (e,
                                                MouseEvent.MOUSE_PRESSED,
                                                button_1,
                                                button_2,
                                                button_3);
        }

        private int zoomPress = 0;

        public void mouseReleased(MouseEvent e) {

            int            dx, dy;

            setButtonID (e);

            dx = 0;
            dy = 0;

            if (button_1 != 0) {
                dx = b1X - e.getX();
                dy = b1Y - e.getY();
                b1X = -1;
                b1Y = -1;
            }
            if (button_2 != 0) {
                dx = b2X - e.getX();
                dy = b2Y - e.getY();
                b2X = -1;
                b2Y = -1;
            }
            if (button_3 != 0) {
                dx = b3X - e.getX();
                dy = b3Y - e.getY();
                b3X = -1;
                b3Y = -1;
            }

            if (dx < 0) dx = -dx;
            if (dy < 0) dy = -dy;

          /*
           * If in zoom rectangle mode, it is possible
           * to end the zoom mode on a release.
           */
            if (zoomToRectangleMode == 1  &&  button_1 != 0)
            {
                if (zoomPress == 1  &&  (dx > 4 ||  dy > 4))
                {
                    endRectangleZoom (e);
                    zoomPress = 0;
                    return;
                }
            }
            editFlag = dlist.processMouseEvent (e,
                                                MouseEvent.MOUSE_RELEASED,
                                                button_1,
                                                button_2,
                                                button_3);

            if (dx < 5  &&  dy < 5) {
                if (zoomPress == 1) {
                    zoomStarted = 0;
                    zoomPress = 0;
                }
                localClick (e);
            }

        }


        public void mouseEntered(MouseEvent e) {
        }


        public void mouseExited(MouseEvent e) {
        }

    };



    private void zoomToExtents (MouseEvent e)
    {

        int ix = e.getX();
        int iy = e.getY();

        int iframe = dlist.findRescaleFrame (ix, iy);
        if (iframe == -1) {
            return;
        }

        dlist.zoomToExtents (iframe);

        repaint ();

        return;

    }

    private void zoomOut (MouseEvent e)
    {

        int ix = e.getX();
        int iy = e.getY();

        int iframe = dlist.findRescaleFrame (ix, iy);
        if (iframe == -1) {
            return;
        }

        dlist.zoomOut (iframe);

        repaint ();

        return;

    }

    private void zoomIn (MouseEvent e)
    {

        int ix = e.getX();
        int iy = e.getY();

        int iframe = dlist.findRescaleFrame (ix, iy);
        if (iframe == -1) {
            return;
        }

        dlist.zoomIn (iframe, ix, iy);

        repaint ();

        return;

    }

    private void selectObject (MouseEvent e, int button_1, int button_3)
    {

        if (selectionAllowed == false) {
            return;
        }

        int ix = e.getX();
        int iy = e.getY();

        // Button 1
        if (button_1 != 0) {
            if (!e.isControlDown()) {
                dlist.unselectAll();
                dlist.pickFrameObject (ix, iy);
            } else {
                int index = dlist.getSelectableIndex(ix, iy);
                if (index < 0) {
                    return;
                }
                DLSelectable dls = dlist.getSelectable(index);
                dlist.setSelected(dls, !dls.isSelected);
            }

            repaint ();

        // Button 3
        } else if (button_3 != 0) {
            int index = dlist.getSelectableIndex(ix, iy);
            if (index < 0) {
              return;
            }
            DLSelectable dls = dlist.getSelectable(index);
            // If it's not selected then select it.
            if (!dls.isSelected) {
                if (!e.isControlDown()) {
                    dlist.unselectAll();
                }
                dlist.setSelected(dls, true);
            }
            dlist.processRightClick(e);
        }

        return;

    }

    private boolean selectionAllowed = false;

  /**
  Make the graphics in the panel selectable or not selectable.  By default,
  when a {@link JDisplayListPanel} is created, the graphics are not
  selectable.  
  */
    public void setSelectionAllowed (boolean b)
    {
        selectionAllowed = b;
    }

}
