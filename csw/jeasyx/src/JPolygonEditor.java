
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.awt.Insets;
import java.awt.Point;
import java.awt.Robot;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.MouseEvent;
import java.awt.geom.GeneralPath;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.util.ArrayList;
import java.util.Iterator;

import javax.swing.AbstractAction;
import javax.swing.ActionMap;
import javax.swing.InputMap;
import javax.swing.JComponent;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JSeparator;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;
import javax.swing.event.PopupMenuEvent;
import javax.swing.event.PopupMenuListener;

/**
 * Graphic line editor.
 *
 * @author Glenn Pinkerton
 */
public class JPolygonEditor implements JDLEditListener {
    /*
     * Picking Mode options.
     * Note: Changes or additions should also modify isLinePicking() and
     * isLineEditing().
     */
    public static final int  LINE_PICKING = 1;
    public static final int  LINE_PICKING_POLYLINE = 2;
    public static final int  LINE_PICKING_POLYGON = 3;
    public static final int  LINE_PICKING_FUNCTION = 4;
    public static final int  LINE_EDITING = 10;
    public static final int  POINT_PICKING = 20;
    public static final int  POINT_EDITING = 30;

    private JDisplayList         dlist = null;
    private JDisplayListPanel    panel = null;
    private DLEditListener       editListener = null;
    private int                  selectableNum;
    private int                  pickingMode;

    private boolean              appendMode = false,
                                 prependMode = false;

    private ArrayList<EPoint>  dbgLine = null;

    private ArrayList<EPoint>  currentLine = null;
    private ELine     currentEline;
    private int       currentPoint;
    private int       currentSegment;

    private boolean    bsypr = false;
    private boolean    bclick = false;

    private void sypr (String  str) {
      if (!bsypr || !bclick || str == null) return;
      System.out.println (str);
      System.out.flush ();
      return;
    }

    private void sypr () {
      if (!bsypr || !bclick) return;
      System.out.println ();
      System.out.flush ();
      return;
    }

    private void syprcomp (ArrayList<EPoint> line) {
      if (!bsypr || !bclick) return;
      int npts = line.size();
      for (int j=0; j<npts; j++) {
          EPoint ep = line.get(j);
          System.out.println ("   sx = " + ep.sx + "  sy = " + ep.sy);
      }
      System.out.flush ();
    }

    private void pr5 (String str, ArrayList<EPoint> line) {
      if (!bsypr || !bclick) return;
      if (line == null) {
        System.out.println (" pr5 from: " + str + "  line is null");
        System.out.flush ();
        return;
      }
      System.out.println (" pr5 from: " + str);
      EPoint ep = line.get(0);
      System.out.println ("   sx = " + ep.sx + "  sy = " + ep.sy);
      ep = line.get(4);
      System.out.println ("   sx = " + ep.sx + "  sy = " + ep.sy);
      System.out.flush ();
    }
      

/**
 * The polygon editor must have references to a {@link JDisplayList} and
 * a {@link JDisplayListPanel}.  If either is null, an {@link IllegalArgumentException}
 * is thrown.  The {@link DLEditListener} must not be null.  If it is null, an
 * illegal argument exception is thrown.  The snum parameter is the native index
 * of the existing DLSelectable object.  If this is a completely
 * new line, -1 can be used.  In this case, a new selectable object
 * is created and set as the surfrent selectable on the display list.
 */
    public JPolygonEditor (
        JDisplayList          dl,
        JDisplayListPanel     dlp,
        DLEditListener        el,
        int                   snum
    )
    {
        if (dl == null) {
            throw
            new IllegalArgumentException
            ("The JPolygonEditor class must have a valid JDisplayList reference.");
        }

        if (dlp == null) {
            throw
            new IllegalArgumentException
            ("The JPolygonEditor class must have a valid JDisplayListPanel reference.");
        }

        if (el == null) {
            throw
            new IllegalArgumentException
            ("The JPolygonEditor class must have a valid DLEditListener reference.");
        }

    // Create a new seledctable object and set is as the current selectable
    // for the display list.  Upon successful setting in the display list, the
    // native index should be set to non negative.  If this is still negative
    // throw an exception.

        if (snum < 0) {
            DLSelectable dls = new DLSelectable ();
            dl.setSelectable (dls);
            snum = dls.getNativeIndex ();
            if (snum < 0) {
                throw
                new IllegalStateException
                ("The JPolygonEditor class cannot get a valid DLSelectable object" +
                 "from the display list.");
            }
        }

        dlist = dl;
        panel = dlp;
        editListener = el;
        selectableNum = snum;
        this.pickingMode = LINE_PICKING_POLYGON;

        appendMode = true;

        setupKeyboardActions ();
        createPopupMenus ();

        initEditPoints ();

        drawEditPoints ();
        panel.setDigitizingCursor();

    }

/*---------------------------------------------------------------------------*/

    private void setupKeyboardActions ()
    {
        KeyStroke
                      endKS,
                      escKS,
                      deleteKS,
                      bsKS;

        endKS = KeyStroke.getKeyStroke ("END");
        escKS = KeyStroke.getKeyStroke ("ESCAPE");
        deleteKS = KeyStroke.getKeyStroke ("DELETE");
        bsKS = KeyStroke.getKeyStroke ("BACKSPACE");

        ActionMap amap = panel.getActionMap ();
        InputMap imap = panel.getInputMap (JComponent.WHEN_IN_FOCUSED_WINDOW);
        InputMap imap2 = panel.getInputMap (JComponent.WHEN_FOCUSED);

        deleteACT del = new deleteACT();
        amap.put ("del", del);
        imap.put (deleteKS, "del");
        imap.put (bsKS, "del");
        imap2.put (deleteKS, "del");
        imap2.put (bsKS, "del");

        endACT end = new endACT();
        amap.put ("end", end);
        imap.put (endKS, "end");
        imap.put (escKS, "end");
        imap2.put (endKS, "end");
        imap2.put (escKS, "end");

    }

/*------------------------------------------------------------------------------*/

    private class deleteACT extends AbstractAction
    {
        private static final long serialVersionUID = 1L;

        deleteACT ()
        {
            super ();
        }

        public void actionPerformed (ActionEvent e)
        {
            JPolygonEditor.this.processDelete ();
        }

    } // end of private action inner class

/*------------------------------------------------------------------------------*/

    private class endACT extends AbstractAction
    {
        private static final long serialVersionUID = 1L;

        endACT ()
        {
            super ();
        }

        public void actionPerformed (ActionEvent e)
        {
            JPolygonEditor.this.endEdit ();
        }

    } // end of private action inner class

/*------------------------------------------------------------------------------*/

    private void moveCursor (EPoint ep)
    {
        Robot robot;

        Point pt = new Point ((int)ep.sx, (int)ep.sy);
        SwingUtilities.convertPointToScreen (pt, panel);

        int ix = (int)pt.getX();
        int iy = (int)pt.getY();

        try
        {
            robot = new Robot ();
        }
        catch (Exception e)
        {
            return;
        }

        robot.mouseMove (ix, iy);
    }


/*------------------------------------------------------------------------------*/

    private void processDelete ()
    {
        EPoint          ep, ep1;

pr5 ("processDelete start", dbgLine);

        if (currentLine != null) {

        /*
         * delete the last point if in append mode
         * or the next to last if in append and the line is closed.
         */
            if (appendMode) {

                int n = currentLine.size();

                if (n > 4) {
                    currentLine.remove (n-3);
                }
                else if (n == 4) {
                    currentLine.remove (3);
                    currentLine.remove (2);
                    ep = currentLine.get (1);
                    dragEP = ep;
                    moveCursor (ep);
                }
                else {
                    currentLine.clear ();
                    appendMode = false;
                    prependMode = false;
                }

                callUpdateListeners ();
            }

         /*
          * Delete the selected point in not in append or prepend mode.
          */
            else if (currentPoint >= 0) {

                int n = currentLine.size ();

             /*
              * Special case for deleting the first or last point of a closed line.
              * The first point is removed, making the previous 2nd point the new
              * first point.  The last point (the closure point) is then repositioned
              * to the new first point.
              */
                if (currentPoint == 0  ||  currentPoint == n-1) {
                    currentLine.remove (0);
                    n--;
                    ep = currentLine.get (n-1);
                    ep1 = currentLine.get (0);
                    ep.sx = ep1.sx;
                    ep.sy = ep1.sy;
                    ep.x = ep1.x;
                    ep.y = ep1.y;
                    ep.flag = ep1.flag;
                    ep.fname = ep1.fname;
                    ep.fnum = ep1.fnum;
                    if (n == 3) {
                        currentLine.remove (2);
                        ep = currentLine.get (1);
                        dragEP = ep;
                        moveCursor (ep);
                        currentFrameNum = dlist.findRescaleFrame ((int)ep.sx, (int)ep.sy);
                        appendMode = true;
                    }
                }

                else {

                    if (n == 4) {
                        currentLine.remove (currentPoint);
                        currentLine.remove (2);
                        ep = currentLine.get (1);
                        dragEP = ep;
                        moveCursor (ep);
                        currentFrameNum = dlist.findRescaleFrame ((int)ep.sx, (int)ep.sy);
                        appendMode = true;
                    }

                    else {
                        if (currentLine.size() > 0) {
                            currentLine.remove (currentPoint);
                            n = currentLine.size();
                            if (n < 2) {
                                currentLine.clear ();
                            }
                        }
                    }
                }

                callUpdateListeners ();

            }
        }

pr5 ("processDelete middle", dbgLine);

        drawEditPoints ();

pr5 ("processDelete end", dbgLine);

    }

/*------------------------------------------------------------------------------*/

    private static class ELine {
        DLLine               dline;
        ArrayList<EPoint>    points;
    }

    private static class EPoint {
        public       double    x;
        public       double    y;
        public       double    sx;
        public       double    sy;
        public       int       flag;
        public       String    fname;
        public       int       fnum;
    }

    private ArrayList<ELine>   editLines = 
        new ArrayList<ELine> (10);
    private EPoint             dragEP = null;

/*--------------------------------------------------------------------*/

    private boolean         motionInterest = false;
    private boolean         updateInterest = true;

/*--------------------------------------------------------------------*/

    public void setMotionInterest (boolean bval)
    {
        motionInterest = bval;
    }

    public void setUpdateInterest (boolean bval)
    {
        updateInterest = bval;
    }

    public boolean getMotionInterest ()
    {
        return motionInterest;
    }

    public boolean getUpdateInterest ()
    {
        return updateInterest;
    }


/*--------------------------------------------------------------------*/

/*
 * If the selectableNum is greater than or equal to zero,
 * get the line points from the display list for the selectable
 * object at the start of editing.
 */
    private void initEditPoints ()
    {

        ArrayList<EPoint> editPoints;

        if (selectableNum < 0) {
            return;
        }

        DLSelectable dls = dlist.getSelectable (selectableNum);
        if (dls == null) {
            return;
        }

        ArrayList<DLLine> lines = dls.getLineList ();
        if (lines == null) {
            return;
        }

        int nline = lines.size ();
        if (nline < 1) {
            return;
        }

        int         i, n;

        n = 0;
        for (i=0; i<nline; i++) {
            if (lines.get(i) != null) {
                n = 1;
                break;
            }
        }

        if (n < 1) {
            return;
        }

        DLLine      dline;
        double[]    xp, yp;
        int         npts, j;

        for (i=0; i<nline; i++) {

            dline = lines.get(i);
            if (dline == null) {
                continue;
            }

            editPoints = new ArrayList<EPoint> (100);

            xp = dline.getXPoints ();
            yp = dline.getYPoints ();
            npts = dline.getNumPoints ();
            for (j=0; j<npts; j++) {
                EPoint ep = new EPoint ();
                ep.x = xp[j];
                ep.y = yp[j];
                ep.flag = 0;
                ep.fname = dline.frameName;
                ep.fnum = -1;
                editPoints.add (ep);
            }
            ELine eline = new ELine ();
            eline.dline = dline;
            eline.points = editPoints;
            editLines.add (eline);
        }

    }

/*--------------------------------------------------------------------*/

    public void update ()
    {
        drawEditPoints ();
    }

/*--------------------------------------------------------------------*/

    private int       buttonNumber = 0;
    private boolean   button1Pressed;
    //private boolean   button2Pressed;
    //private boolean   button3Pressed;

/*--------------------------------------------------------------------*/

    private void setButtonID (MouseEvent e)
    {
        int button_1 = e.getModifiers() & InputEvent.BUTTON1_MASK;
        int button_2 = e.getModifiers() & InputEvent.BUTTON2_MASK;
        int button_3 = e.getModifiers() & InputEvent.BUTTON3_MASK;
        if (button_1 != 0) {
            buttonNumber = 1;
        }
        else if (button_2 != 0) {
            buttonNumber = 2;
        }
        else if (button_3 != 0) {
            buttonNumber = 3;
        }
        else {
            buttonNumber = 0;
        }
    }

/*--------------------------------------------------------------------*/

    MouseEvent       popupMouseEvent = null;

/*--------------------------------------------------------------------*/

    public void mouseClicked (MouseEvent e)
    {

        setButtonID (e);

        findPointOrSegment (e);

        if (currentLine == null) {
            currentLine = new ArrayList<EPoint> (20);
        }

        if (buttonNumber == 1) {
            if (popupActive == 0) {
bclick = true;
                processLeftClick (e);
                drawEditPoints ();
bclick = false;
            }
        }
        else if (buttonNumber == 2) {
            if (popupActive == 0) {
                processMiddleClick (e);
                drawEditPoints ();
            }
        }
        else {
            popupMouseEvent = e;
            processPopupMenu (e);
        }

        if (popupActive == 1) {
            popupActive = 0;
        }

    }

/*--------------------------------------------------------------------*/

    public void mouseMoved (MouseEvent e)
    {
        processMotion (e);
    }

/*--------------------------------------------------------------------*/

    public void mouseDragged (MouseEvent e)
    {
        processMotion (e);
    }

/*--------------------------------------------------------------------*/

    public void mousePressed (MouseEvent e)
    {
        EPoint      ep;

        setButtonID (e);
        if (buttonNumber == 1) {
            button1Pressed = true;
        }
        else if (buttonNumber == 2) {
            //button2Pressed = true;
            return;
        }
        else if (buttonNumber == 3) {
            //button3Pressed = true;
            return;
        }

pr5 ("mousePressed start", dbgLine);

    /*
     * If the button press is on a point or line, make the location
     * the drag point.  Only do this if not in append or prepend mode.
     */
        if (button1Pressed  &&  appendMode == false  &&  prependMode == false) {
            findPointOrSegment (e);
            if (currentLine != null) {
                ep = null;
                if (currentPoint >= 0) {
                    ep = currentLine.get(currentPoint);
                }
                else if (currentSegment >= 0) {
                    ep = new EPoint ();
                    ep.sx = e.getX();
                    ep.sy = e.getY();
                    ep.fname = null;
                    int iframe = dlist.findRescaleFrame (e.getX(),
                                                         e.getY());
                    ep.fnum = iframe;
                    currentLine.add (currentSegment+1, ep);
                    callUpdateListeners ();
                    screenToFrame (ep);
                }
                dragEP = ep;
            }
pr5 ("mousePressed middle", dbgLine);

            drawEditPoints ();

        }

pr5 ("mousePressed end", dbgLine);

        return;

    }

/*--------------------------------------------------------------------*/

    private int drawFromPaint = 0;

    public void draw ()
    {
        drawFromPaint = 1;
        drawEditPoints ();
    }

    private void localRepaint ()
    {
        if (drawFromPaint == 0) {
            panel.repaint ();
        }
        drawFromPaint = 0;
    }

/*--------------------------------------------------------------------*/

    private void addPointFromPopup ()
    {
/*
        MouseEvent e;

        if (popupMouseEvent == null) {
            return;
        }

        e = popupMouseEvent;

        EPoint ep = new EPoint ();
        ep.sx = e.getX();
        ep.sy = e.getY();
        ep.fname = null;
        int iframe = dlist.findRescaleFrame (e.getX(),
                                             e.getY());
        ep.fnum = iframe;
        currentLine.add (currentSegment+1, ep);
        screenToFrame (ep);

        callUpdateListeners ();

        popupMouseEvent = null;

        closeLineIfNeeded (currentLine);

        drawEditPoints ();
*/
    }

/*--------------------------------------------------------------------*/

    private void deleteLineFromPopup ()
    {

        if (currentEline == null) {
            return;
        }

        int index = editLines.indexOf (currentEline);
        if (index >= 0) {
            editLines.remove (index);
        }

        callUpdateListeners ();

        popupMouseEvent = null;

        drawEditPoints ();
    }

/*--------------------------------------------------------------------*/

    private void newLineFromPopup ()
    {
        MouseEvent e;

        if (popupMouseEvent == null) {
            return;
        }

        e = popupMouseEvent;

        int iframe = dlist.findRescaleFrame (e.getX(),
                                             e.getY());
        if (iframe == -1) {
            return;
        }
        currentLine = new ArrayList<EPoint> (20);
        ELine  eline = new ELine ();
        currentEline = eline;
        eline.dline = null;
        eline.points = currentLine;
        editLines.add (eline);
        EPoint ep = new EPoint ();
        ep.sx = e.getX();
        ep.sy = e.getY();
        ep.fnum = iframe;
        screenToFrame (ep);
        currentLine.add (ep);
        ep = new EPoint ();
        ep.sx = e.getX();
        ep.sy = e.getY();
        ep.fnum = iframe;
        screenToFrame (ep);
        currentLine.add (ep);
        dragEP = ep;
        currentFrameNum = iframe;
        appendMode = true;

        callUpdateListeners ();

        popupMouseEvent = null;

        drawEditPoints ();
    }


/*--------------------------------------------------------------------*/

    public void mouseReleased (MouseEvent e)
    {

pr5 ("mouseReleased enter", dbgLine);
        setButtonID (e);
        if (buttonNumber == 1) {
            button1Pressed = false;
        }
        else if (buttonNumber == 2) {
            //button2Pressed = false;
            return;
        }
        else if (buttonNumber == 3) {
            //button3Pressed = false;
        }

        if (appendMode == false  &&  prependMode == false) {
            callUpdateListeners ();
            resetState ();
            drawEditPoints ();
        }

pr5 ("mouseReleased end", dbgLine);
        return;

    }

/*--------------------------------------------------------------------*/

    private void processMotion (MouseEvent e)
    {
        if (dragEP == null) {
            return;
        }

        dragEP.sx = e.getX();
        dragEP.sy = e.getY();
        screenToFrame (dragEP);

pr5 ("mouse motion enter", dbgLine);

        // make sure the component stays closed.
        if (currentLine != null) {
            int lastPoint = currentLine.size() - 1;
            if (currentPoint == 0) {
                EPoint epLast = currentLine.get(lastPoint);
                epLast.x = dragEP.x;
                epLast.y = dragEP.y;
            } 
            else if (currentPoint == lastPoint) {
                EPoint epFirst = currentLine.get(0);
                epFirst.x = dragEP.x;
                epFirst.y = dragEP.y;
            }
        }

        callMotionListeners ();

pr5 ("mouse motion middle", dbgLine);
        drawEditPoints ();
pr5 ("mouse motion end", dbgLine);

    }

/*--------------------------------------------------------------------*/

    private void drawEditPoints ()
    {
        int             bgr, bgg, bgb, r, g, b;
        int             i, j, npts;
        GeneralPath     gpath, gpath2;
        EPoint          ep;
        ArrayList<EPoint>       line;
        ELine           eline;

sypr ("draw edit points called");

        dlist.clearDirectShapes ();

        int lsize = editLines.size ();
        int csize = 0;
        if (currentLine != null) csize = currentLine.size ();
        if (lsize < 1  &&  csize < 1) {
            localRepaint ();
            return;
        }

    /*
     * Decide colors for the editing line.
     */
        bgr = dlist.backgroundRed;
        bgg = dlist.backgroundGreen;
        bgb = dlist.backgroundBlue;

        if (bgr + bgg + bgb < 150) {
            r = 240;
            g = 240;
            b = 240;
        }
        else {
            r = 20;
            g = 20;
            b = 20;
        }

        NativePrim.Frame   frame;

        int fnum = -1;

        int lslast = lsize - 1;
        if (currentLine == null) lslast = lsize;

    /*
     * Create shapes for the edit lines and put them in the display list.
     */
        for (i=0; i<lslast; i++) {
            eline = editLines.get(i);
            line = eline.points;
            if (line == null) {
                continue;
            }
            npts = line.size();
            if (npts < 2) {
                continue;
            }
            ep = line.get(0);
            fnum = dlist.findFrameNumberByName (ep.fname);
            frame = dlist.findFrameByName (ep.fname);
            if (frame == null) {
                frame = dlist.findEditFrame (ep.fnum);
                fnum = ep.fnum;
                if (frame == null) {
                    continue;
                }
            }
sypr ("points in edit draw component = " + line.hashCode());
if (currentLine != null) {
sypr ("currentLine = " + currentLine.hashCode());
}
else {
sypr ("currentLine = null");
}
sypr ("npts in edit draw old component = " + npts);
syprcomp (line);
            for (j=0; j<npts; j++) {
                ep = line.get(j);
                ep.sx = (ep.x - frame.fx1) / frame.xscale + frame.x1;
                ep.sy = (ep.y - frame.fy1) / frame.yscale + frame.y1;
            }
            gpath = new GeneralPath (GeneralPath.WIND_EVEN_ODD);
            gpath.reset ();
            ep = line.get(0);
            gpath.moveTo ((float)ep.sx, (float)ep.sy);
            for (j=1; j<npts; j++) {
                ep = line.get(j);
                if (ep == null) {
                    continue;
                }
                gpath.lineTo ((float)ep.sx, (float)ep.sy);
            }
            dlist.addDirectShape (gpath, r, g, b, 0, fnum);
        }

    /*
     * Draw squares at the line points.
     */
        Rectangle2D    rect;

        for (i=0; i<lslast; i++) {
            eline = editLines.get(i);
            line = eline.points;
            if (line == null) {
                continue;
            }
            npts = line.size();
            if (npts < 2) {
                continue;
            }
            ep = line.get(0);
            fnum = dlist.findFrameNumberByName (ep.fname);
            frame = dlist.findFrameByName (ep.fname);
            if (frame == null) {
                frame = dlist.findEditFrame (ep.fnum);
                fnum = ep.fnum;
                if (frame == null) {
                    continue;
                }
            }
            for (j=0; j<npts; j++) {
                ep = line.get(j);
                if (ep == null  ||  ep == dragEP) {
                    continue;
                }
                rect = new Rectangle2D.Double ();
                rect.setFrameFromCenter (
                    ep.sx,
                    ep.sy,
                    ep.sx+3.0,
                    ep.sy+3.0);
                dlist.addDirectShape (rect, bgr, bgg, bgb, 1, fnum);
                dlist.addDirectShape (rect, r, g, b, 0, fnum);
            }
        }

    /*
     * Draw line segments for the current line
     */
        line = currentLine;
        if (line == null) {
            return;
        }
        npts = line.size();
        if (npts < 2) {
            return;
        }
        gpath2 = new GeneralPath (GeneralPath.WIND_EVEN_ODD);
        ep = line.get(0);
sypr ("draw point 0 = " + ep.sx + "   " + ep.sy);
        gpath2.moveTo ((float)ep.sx, (float)ep.sy);
        for (j=1; j<npts; j++) {
            ep = line.get(j);
            if (ep == null) {
                continue;
            }
sypr ("draw point " + j + " = " + ep.sx + "   " + ep.sy);
            gpath2.lineTo ((float)ep.sx, (float)ep.sy);
        }
        if (npts > 2) {
            ep = line.get(0);
sypr ("closure point = " + ep.sx + "   " + ep.sy);
            gpath2.lineTo ((float)ep.sx, (float)ep.sy);
        }
        dlist.addDirectShape (gpath2, r, g, b, 0, fnum);

        localRepaint ();

        return;

    }

/*----------------------------------------------------------------------*/

    private void closeLineIfNeeded (ArrayList<EPoint> line)
    {

        if (line == null) {
            return;
        }

        int npts = line.size ();
        if (npts < 3) {
            return;
        }

        EPoint    ep1, ep2, ep3, ep;
        double    dx, dy;

        ep1 = line.get(0);
        ep2 = line.get(npts-1);

    // If the last and next to last points are coincident,
    // make the last point the same location as the first point
    // and do not add another "closing" point.
        if (npts > 2) {
            ep3 = line.get (npts - 2);
            dx = ep3.sx - ep2.sx;
            dy = ep3.sy - ep2.sy;
            if (dx < 0) dx = -dx;
            if (dy < 0) dy = -dy;
            if (dx < 3  &&  dy < 3) {
                ep2.sx = ep1.sx;
                ep2.sy = ep1.sy;
                ep2.x = ep1.x;
                ep2.y = ep1.y;
                ep2.flag = ep1.flag;
                ep2.fname = ep1.fname;
                ep2.fnum = ep1.fnum;
                return;
            }
        }

        dx = ep1.sx - ep2.sx;
        dy = ep1.sy - ep2.sy;
        if (dx < 0) dx = -dx;
        if (dy < 0) dy = -dy;

        if (dx > 0  ||  dy > 0) {
            ep = new EPoint ();
            ep.sx = ep1.sx;
            ep.sy = ep1.sy;
            ep.x = ep1.x;
            ep.y = ep1.y;
            ep.flag = ep1.flag;
            ep.fname = ep1.fname;
            ep.fnum = ep1.fnum;
            line.add (ep);
        }

    }

/*----------------------------------------------------------------------*/

    private void screenToFrame (EPoint ep)
    {
        NativePrim.Frame      f = null;

        if (ep == null) {
            return;
        }

        if (ep.fname != null) {
            f = dlist.findFrameByName (ep.fname);
        }
        else if (ep.fnum >= 0) {
            f = dlist.findEditFrame (ep.fnum);
        }

        if (f != null) {
            ep.x = (ep.sx - f.x1) * f.xscale + f.fx1;
            ep.y = (ep.sy - f.y1) * f.yscale + f.fy1;
        }

        return;

    }


/*----------------------------------------------------------------------*/

    private void findPointOrSegment (MouseEvent e)
    {
        double        xp, yp;
        EPoint        ep, ep1, ep2;

        ArrayList<EPoint>     line;
        ELine         eline;
        int           lsize, npts, i, j, jmin, jlmin;
        double        dist, minDist;


    /*
     * If button 1 was clicked, and we are in prepend or append mode,
     * don't do anything here.
     */
        if (buttonNumber == 1) {
            if (prependMode  ||  appendMode) {
                return;
            }
        }

    /*
     * Initialize as if nothing is near the point.
     */
        currentLine = null;
        currentEline = null;
        currentPoint = -1;
        currentSegment = -1;

        lsize = editLines.size();
        if (lsize < 1) {
            return;
        }

        xp = e.getX();
        yp = e.getY();

    /*
     * Check for a close vertex.
     */
        jmin = -1;
        jlmin = -1;
        minDist = 1.e30;

        for (i=0; i<lsize; i++) {
            eline = editLines.get(i);
            line = eline.points;
            if (line == null) {
                continue;
            }
            npts = line.size();
            if (npts < 1) {
                continue;
            }
            for (j=0; j<npts; j++) {
                ep = line.get (j);
                if (ep == null) {
                    continue;
                }
                dist = Point2D.distance (xp, yp, ep.sx, ep.sy);
                if (dist < 3.0) {
                    currentLine = line;
                    currentEline = eline;
                    currentPoint = j;
                    return;
                }
                if (dist < minDist) {
                    minDist = dist;
                    jmin = j;
                    jlmin = i;
                }
            }
        }

        if (jmin >= 0  &&  jlmin >= 0) {
            if (minDist < 5.0) {
                eline = editLines.get(jlmin);
                currentLine = eline.points;
                currentEline = eline;
                currentPoint = jmin;
                return;
            }
        }

    /*
     * Check for a close line segment.
     */
        jlmin = -1;
        jmin = -1;
        minDist = 1.e30;

        for (i=0; i<lsize; i++) {
            eline = editLines.get(i);
            line = eline.points;
            if (line == null) {
                continue;
            }
            npts = line.size();
            if (npts < 2) {
                continue;
            }
            for (j=0; j<npts-1; j++) {
                ep1 = line.get (j);
                ep2 = line.get (j+1);
                if (ep1 == null  ||  ep2 == null) {
                    continue;
                }
                dist = Line2D.ptSegDist (ep1.sx, ep1.sy,
                                         ep2.sx, ep2.sy,
                                         xp, yp);
                if (dist < 2.0) {
                    currentLine = line;
                    currentEline = eline;
                    currentSegment = j;
                    return;
                }
                if (dist < minDist) {
                    minDist = dist;
                    jmin = j;
                    jlmin = i;
                }
            }
        }

        if (jmin >= 0  &&  jlmin >= 0) {
            if (minDist < 4.0) {
                eline = editLines.get(jlmin);
                currentLine = eline.points;
                currentEline = eline;
                currentSegment = jmin;
                return;
            }
        }

        return;

    }

/*----------------------------------------------------------------------------*/

    private int          currentFrameNum = -1;

/*----------------------------------------------------------------------------*/

    private void processMiddleClick (MouseEvent e)
    {
        int     nclick = e.getClickCount ();

        if (nclick > 1) {
            return;
        }

        if (currentLine != null) {
            processDelete ();
            return;
        }
    }

/*----------------------------------------------------------------------------*/

    private void processLeftClick (MouseEvent e)
    {

        EPoint  ep;

sypr ();
sypr ("process left click entered editLines size = " + editLines.size());
sypr ("  sx = " + e.getX() + "  sy = " + e.getY());
pr5 ("#100", dbgLine);

        int iframe = dlist.findRescaleFrame (e.getX(),
                                             e.getY());

        if (iframe == -1) {
            return;
        }

    /*
     * If the alt key is down and the click is on a point, delete the point.
        if (
          e.isAltDown()  &&
          appendMode == false  &&
          prependMode == false &&
          pickingMode != POINT_EDITING
        ) {
            if (currentLine != null) {
                if (currentPoint >= 0) {
                    processDelete ();
                    return;
                }
            }
        }
     */

    /*
     * If the click is not on any point or segment, it can start
     * a new line in append mode.  Two points are added at the
     * start location.  The first point is "locked" in place as
     * the start of the line and the second is the point that will
     * be subsequently dragged via mouse motion to its location.
     */
        if (currentLine == null) {
            startNewComponent ();
            addFirstTwoPoints (e, iframe);
            callUpdateListeners ();
            return;
        }

        if (e.getClickCount() == 2) {
bsypr = true;
sypr ("double click count in left click npts = " + currentLine.size());

            closeLineIfNeeded (currentLine);
            endCurrentLine ();
            callUpdateListeners ();
pr5 ("#7", dbgLine);
            return;            
        }

        if (currentLine.size() == 0) {
            addFirstTwoPoints (e, iframe);
            return;
        }        

    /*
     * If we are in append or prepend mode, a single click will
     * add a new point to the start or end, respectively of the
     * current line array.  This new point becomes the dragEP
     * point, implicitly "locking" the previously appended point
     * (which was the previous dragEP point) in place.
     */
        if (appendMode) {
            ep = new EPoint ();
            ep.sx = e.getX();
            ep.sy = e.getY();
            ep.fnum = currentFrameNum;
            screenToFrame (ep);
            int npts = currentLine.size ();
            currentLine.add (ep);
            currentSegment = -1;
            dragEP = ep;
            callUpdateListeners ();
            return;
        }

        if (prependMode) {
            ep = new EPoint ();
            ep.sx = e.getX();
            ep.sy = e.getY();
            ep.fnum = currentFrameNum;
            screenToFrame (ep);
            currentLine.add (0, ep);
            dragEP = ep;
            currentSegment = -1;
            callUpdateListeners ();
            return;
        }

        return;
    }


/*----------------------------------------------------------------------------*/

private void startNewComponent ()
{

    currentLine = new ArrayList<EPoint> (20);

    return;

}


private void endCurrentLine ()
{

sypr ("endCurrentLine nline = " + editLines.size ());
sypr (" line pointers at end  " + currentLine.hashCode() + "   " +
  editLines.get(editLines.size() - 1).points.hashCode());

syprcomp (editLines.get(editLines.size() - 1).points);

dbgLine = currentLine;

    currentLine = null;
    return;

}


/*----------------------------------------------------------------------------*/

private void addFirstTwoPoints (MouseEvent e, int iframe)
{

    ELine  eline = new ELine ();
    currentEline = eline;
    eline.dline = null;
    eline.points = currentLine;
    editLines.add (eline);
    EPoint ep = new EPoint ();
    ep.sx = e.getX();
    ep.sy = e.getY();
    ep.fnum = iframe;
    screenToFrame (ep);
    currentLine.add (ep);
    ep = new EPoint ();
    ep.sx = e.getX();
    ep.sy = e.getY();
    ep.fnum = iframe;
    screenToFrame (ep);
    currentLine.add (ep);
    dragEP = ep;
    currentFrameNum = iframe;
    appendMode = true;

}


/*----------------------------------------------------------------------------*/

    private void endEdit ()
    {
        for (ELine eline : editLines) {
          closeLineIfNeeded(eline.points);
        }
        callEndListeners ();
        dlist.setSelectionMode();
    }

/*----------------------------------------------------------------------------*/

    private void cancelEdit()
    {
        dlist.setSelectionMode();
        dlist.clearDirectShapes ();
        // This reloads the original edit points.
        DLSelectable dls = dlist.getSelectable (selectableNum);
        dlist.setSelected(dls, true);
        panel.repaint ();
    }

/*----------------------------------------------------------------------------*/

    private void startEditMode ()
    {
        if (appendMode  &&  currentLine != null) {
            endAppendMode ();
        }
        else if (prependMode  &&  currentLine != null) {
            endPrependMode ();
        }
        return;
    }

/*----------------------------------------------------------------------------*/

    private void startExtensionMode ()
    {
pr5 ("startExtensionMode enter", dbgLine);
        int last = currentLine.size () - 1;
        EPoint startEP = null;
        if (currentPoint == last) {
            startEP = currentLine.get(last);
            appendMode = true;
        } else {
            startEP = currentLine.get(0);
            prependMode = true;
        }
        currentFrameNum =
              dlist.findRescaleFrame ((int) startEP.sx, (int) startEP.sy);
        EPoint ep = new EPoint ();
        ep.sx = startEP.sx;
        ep.sy = startEP.sy;
        ep.fnum = startEP.fnum;
        ep.x = startEP.x;
        ep.y = startEP.y;
        ep.fname = startEP.fname;
        ep.flag = startEP.flag;
        if (appendMode) {
            currentLine.add (ep);
        } else {
            currentLine.add (0, ep);
        }
        dragEP = ep;
        currentSegment = -1;
pr5 ("startExtensionMode end", dbgLine);
        return;
    }

/*----------------------------------------------------------------------------*/

    private void endPrependMode ()
    {
        int n = currentLine.size ();
        if (n > 0) {
            currentLine.remove (0);
        }
        callUpdateListeners ();
        resetState ();
    }

/*----------------------------------------------------------------------------*/

    private void endAppendMode ()
    {
        int n = currentLine.size ();
        if (n > 1) {
            currentLine.remove (n-2);
        }

        callUpdateListeners ();
        resetState ();
    }

/*----------------------------------------------------------------------------*/

    private void resetState ()
    {
        currentLine = null;
        currentEline = null;
        currentFrameNum = -1;
        dragEP = null;
        appendMode = false;
        prependMode = false;
        currentPoint  = -1;
        currentSegment = -1;
    }

/*----------------------------------------------------------------------------*/

    private int popupActive = 0;

    private class PopListen implements PopupMenuListener
    {
        public void popupMenuCanceled (PopupMenuEvent e)
        {
            JPolygonEditor.this.popupActive = 1;
        }

        public void popupMenuWillBecomeVisible (PopupMenuEvent e)
        {
            JPolygonEditor.this.popupActive = 2;
        }

        public void popupMenuWillBecomeInvisible (PopupMenuEvent e)
        {
            JPolygonEditor.this.popupActive = 1;
        }

    }

/*----------------------------------------------------------------------------*/

    private
    JMenuItem      editModeItem,
                   extensionModeItem,
                   endEditItem,
                   endPolylineItem,
                   endPolygonItem,
                   cancelEditItem,
                   deletePointItem,
                   addPointItem,
                   newLineItem,
                   deleteLineItem;

    private void createPopupMenus ()
    {

        editModeItem = makeJMenuItem ("Edit Mode");
        editModeItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                JPolygonEditor.this.startEditMode ();
                JPolygonEditor.this.popupActive = 0;
            }
        });

        extensionModeItem = makeJMenuItem ("Extension Mode");
        extensionModeItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                JPolygonEditor.this.startExtensionMode();
                JPolygonEditor.this.popupActive = 0;
            }
        });

        endEditItem = makeJMenuItem ("End Edit");
        endEditItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                JPolygonEditor.this.endEdit ();
                JPolygonEditor.this.popupActive = 0;
            }
        });

        cancelEditItem = makeJMenuItem ("Cancel Edit");
        cancelEditItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                JPolygonEditor.this.cancelEdit ();
                JPolygonEditor.this.popupActive = 0;
            }
        });

        deletePointItem = makeJMenuItem ("Delete Point");
        deletePointItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                JPolygonEditor.this.processDelete ();
                JPolygonEditor.this.popupActive = 0;
            }
        });

        addPointItem = makeJMenuItem ("Add Point");
        addPointItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                JPolygonEditor.this.addPointFromPopup ();
                JPolygonEditor.this.popupActive = 0;
            }
        });

        endPolygonItem = makeJMenuItem ("End Polygon Component");
        endPolygonItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                JPolygonEditor.this.endEdit ();
                JPolygonEditor.this.popupActive = 0;
            }
        });

        newLineItem = makeJMenuItem ("Start New Polygon Component");
        newLineItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                JPolygonEditor.this.newLineFromPopup ();
                JPolygonEditor.this.popupActive = 0;
            }
        });

        deleteLineItem = makeJMenuItem ("Delete Polygon Component");
        deleteLineItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                JPolygonEditor.this.deleteLineFromPopup ();
                JPolygonEditor.this.popupActive = 0;
            }
        });

        return;

    }

/*----------------------------------------------------------------------------*/

    private JMenuItem makeJMenuItem (String txt)
    {
        JMenuItem       jb;
        jb = new JMenuItem (txt);
        jb.setBorderPainted (false);
        jb.setMargin (new Insets (3, 1, 3, 1));
        return jb;
    }

/*----------------------------------------------------------------------------*/

    private void processPopupMenu (MouseEvent e)
    {
        JPopupMenu pop = new JPopupMenu ();
        pop.addPopupMenuListener (new PopListen ());

        if (appendMode || prependMode) {
            if (currentLine != null && currentLine.size() > 2) {
                pop.add (editModeItem);
                pop.add (new JSeparator());
            }
            boolean isItemAdded = false;
            if (
              (pickingMode == LINE_PICKING || pickingMode == LINE_PICKING_POLYLINE) &&
              (currentLine != null && currentLine.size() > 2)
            ) {
                pop.add (endPolylineItem);
                isItemAdded = true;
            }
            if (
              (pickingMode == LINE_PICKING || pickingMode == LINE_PICKING_POLYGON) &&
              (currentLine != null && currentLine.size() > 3)
            ) {
                pop.add (endPolygonItem);
                isItemAdded = true;
            }
            if (
                pickingMode == LINE_EDITING ||
                pickingMode == LINE_PICKING_FUNCTION
            ) {
                pop.add(endEditItem);
                isItemAdded = true;
            }
            if (isItemAdded) {
              pop.add (new JSeparator());
            }
            pop.add (cancelEditItem);
            pop.show (panel, e.getX(), e.getY());
            return;

        }

        if (currentPoint >= 0) {
            pop.add (deletePointItem);
            int lastPoint = currentLine.size() - 1;
            pop.add (new JSeparator());
            pop.add (endEditItem);
            pop.add (cancelEditItem);
            pop.show (panel, e.getX(), e.getY());
            return;
        }

        if (currentSegment >= 0) {
            pop.add (addPointItem);
            pop.add (new JSeparator());
            pop.add (endEditItem);
            pop.add (cancelEditItem);
            pop.show (panel, e.getX(), e.getY());
            return;
        }

        if (currentLine == null) {
            pop.add (endEditItem);
            pop.add (cancelEditItem);
            pop.show (panel, e.getX(), e.getY());
            return;
        }

        return;

    }
/*----------------------------------------------------------------------------*/

    private boolean isLinePicking() {
        return (
            pickingMode == LINE_PICKING ||
            pickingMode == LINE_PICKING_POLYLINE ||
            pickingMode == LINE_PICKING_POLYGON ||
            pickingMode == LINE_PICKING_FUNCTION
        );
    }

    /*
    private boolean isLineEditing() {
        return (pickingMode == LINE_EDITING);
    }
    */

    private boolean isPointPicking() {
        return (pickingMode == POINT_PICKING);
    }

    private boolean isPointEditing() {
        return (pickingMode == POINT_EDITING);
    }

/*----------------------------------------------------------------------------*/

    private int rebuildSelectableLines ()
    {
        DLSelectable dls = dlist.getSelectable (selectableNum);
        if (dls == null) {
            return -1;
        }

        ArrayList<DLLine> lines;
        lines = dls.lineList;
        if (lines == null) {
            lines = new ArrayList<DLLine> (10);
            dls.lineList = lines;
        }

        DLLine        dline;
        boolean       dlfound;
        int           i;

      /*
       * If any dline in the original list has no counterpart in the
       * edit list, that dline has been deleted and it is removed from
       * the selectable's line list.
       */
        int lsize = lines.size();
        if (lsize > 0) {
            for (i=lsize-1; i>=0; i--) {
                dline = lines.get (i);
                if (dline == null) {
                    lines.remove (i);
                    continue;
                }
                dlfound = findDline (dline);
                if (dlfound == false) {
                    lines.remove (i);
                    continue;
                }
            }
        }

        ELine          eline;
        int            index;

      /*
       * If an edit line has a counterpart in the selectable line
       * list (after the removal of lines that no longer exist),
       * then update its geometry.  If there is no counterpart,
       * create a new dline with the current graphic attributes
       * and add it to the selectable's line list.
       */
        int esize = editLines.size ();
        for (i=0; i<esize; i++) {
            eline = editLines.get (i);
            assignXYToDline (eline);
            if (eline.dline != null) {
                index = lines.indexOf (eline.dline);
                if (index < 0) {
                    lines.add (eline.dline);
                }
            }
        }

        return 1;

    }

/*----------------------------------------------------------------------------*/

    private void assignXYToDline (ELine eline)
    {
        if (eline == null) {
            return;
        }

        if (eline.points == null) {
            eline.dline = null;
            return;
        }

        if (
          eline.points.size() < 2 &&
          (!isPointPicking() && !isPointEditing())
        ) {
            eline.dline = null;
            return;
        } else if (eline.points.size() < 1) {
            eline.dline = null;
            return;
        }

        if (eline.dline == null) {
            eline.dline = new DLLine ();
            dlist.populateLineAttributes (eline.dline);
        }

        double[]        xp, yp;
        int             i;
        EPoint          ep;

        int size = eline.points.size ();
        xp = new double[size];
        yp = new double[size];

        for (i=0; i<size; i++) {
            ep = eline.points.get (i);
            xp[i] = ep.x;
            yp[i] = ep.y;
        }

        eline.dline.xPoints = xp;
        eline.dline.yPoints = yp;
        eline.dline.numPoints = size;

    }

/*----------------------------------------------------------------------------*/

    private boolean findDline (DLLine dline)
    {
        int        i, size;
        ELine      eline;

        size = editLines.size();
        for (i=0; i<size; i++) {
            eline = editLines.get (i);
            if (eline.dline == dline) {
                if (eline.points != null) {
                    if (eline.points.size() > 1) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

/*----------------------------------------------------------------------------*/

    private void callMotionListeners ()
    {
        if (motionInterest == false) {
            return;
        }

        rebuildSelectableLines ();

        DLSelectable dls = dlist.getSelectable (selectableNum);
        if (dls == null) {
            return;
        }

        boolean b = editListener.editMoved (dls);

        if (b) {
            initEditPoints ();
        }
    }

/*----------------------------------------------------------------------------*/

    private void callUpdateListeners ()
    {
        if (updateInterest == false) {
            return;
        }

        rebuildSelectableLines ();

        DLSelectable dls = dlist.getSelectable (selectableNum);
        if (dls == null) {
            return;
        }

        boolean b = editListener.editChanged (dls);

        if (b) {
            initEditPoints ();
        }
    }

/*----------------------------------------------------------------------------*/

    private void callEndListeners ()
    {

        rebuildSelectableLines ();

        DLSelectable dls = dlist.getSelectable (selectableNum);
        if (dls == null) {
            return;
        }

        boolean b = editListener.editFinished (dls);

        if (b == true) {
            dlist.clearDirectShapes ();
            panel.repaint ();
        }

    }

/*----------------------------------------------------------------------------*/

}  // end of JPolygonEditor class
