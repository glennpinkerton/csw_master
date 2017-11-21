
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jtest;

import java.awt.Color;
import java.awt.Container;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Random;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.ToolTipManager;
import javax.swing.WindowConstants;

import csw.jsurfaceworks.src.*;

import csw.jutils.src.NodeArray3D;
import csw.jutils.src.XYZPolyline;

import jcore.graph.properties.LightData;
import jcore.graph.properties.View3DProperties;

import csw.j3d.src.DigitizingType;
import csw.j3d.src.J3DFault;
import csw.j3d.src.J3DHorizonPatch;
import csw.j3d.src.J3DLine;
import csw.j3d.src.J3DLineEditListener;
import csw.j3d.src.J3DPanel;
import csw.j3d.src.J3DRightClickInfo;
import csw.j3d.src.J3DRightClickListener;
import csw.j3d.src.J3DSelectListener;
import csw.j3d.src.J3DSelectionChangedInfo;
import csw.j3d.src.J3DSurfacePatch;
import csw.j3d.src.JPoint3D;
import csw.jeasyx.src.JDisplayList;


/**
 *
 * This class is used as a main entry for unit testing 3d graphics stuff.
 *
 *  @author Glenn Pinkerton
 *
*/
public class SplitTest {

    static {
        System.loadLibrary ("jsurfaceworks_test");
    }

    public static void main (String[] args) {

        JSurfaceWorks.openLogFile ("sp_sw.txt");
        JDisplayList.openLogFile ("sp_ez.txt");

        ToolTipManager.sharedInstance().
          setLightWeightPopupEnabled (false);

        SplitTestFrame frame = new SplitTestFrame ();
        frame.setDefaultCloseOperation (JFrame.EXIT_ON_CLOSE);
        frame.show ();

    }

}



class SplitTestFrame extends JFrame
    implements J3DSelectListener
{
    private J3DPanel    panel = null;

    public void selectionChanged (J3DSelectionChangedInfo info)
    {

        ArrayList<?>   hlist;

        hlist = info.getSelectedHorizons ();

        if (hlist == null) {
            return;
        }

        if (hlist.size() != 1) {
            split_button.setEnabled (false);
        }
        else {
            split_button.setEnabled (true);
        }
    }

    public void pickModeChanged ()
    {
    }

    private JButton split_button;

    public SplitTestFrame ()
    {

        final Simple3DFrame frame = new Simple3DFrame ();

        panel = frame.getPanel ();
        if (panel != null) {
            panel.addSelectListener (this);
        }

        frame.show ();

        setTitle ("Split Surface Unit Test");
        setSize (200, 400);
        setResizable (false);
        Container contentPane = getContentPane ();
        contentPane.setLayout (new GridLayout(15,1));

        JButton hfile_button = new JButton ("New Horizon from File");
        hfile_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.newHorizonFromFile ();
                }
            }
        });

        JButton cline_button = new JButton ("Edit Centerlines");
        cline_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.editCenterlines ();
                }
            }
        });

        JButton end_button = new JButton ("End Edit Centerlines");
        end_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.endEditCenterlines ();
                }
            }
        });


        JButton preview_button = new JButton ("Show preview");
        preview_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.showEditSurface (true);
                }
            }
        });

        JButton hide_button = new JButton ("Hide preview");
        hide_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.hideEditSurface ();
                }
            }
        });

        JButton apply_button = new JButton ("Apply preview fault");
        apply_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.applyPreviewFault ();
                }
            }
        });

        split_button = new JButton ("Split Selected Surface");
        split_button.setEnabled (false);
        split_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.splitSurf ();
                }
            }
        });

        JButton reset_button = new JButton ("Reset");
        reset_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.resetAll ();
                }
            }
        });

        //contentPane.add (hfile_button);
        contentPane.add (cline_button);
        contentPane.add (end_button);
        contentPane.add (preview_button);
        contentPane.add (hide_button);
        contentPane.add (apply_button);
        contentPane.add (split_button);
    }

}  // end of SplitTestFrame class



/*-----------------------------------------------------------------*/

class SurfPoints
{
    double[] x;
    double[] y;
    double[] z;
    int      npts;
    J3DSurfacePatch   sp;
}

/*-----------------------------------------------------------------*/


class Simple3DFrame extends JFrame
   implements J3DLineEditListener, J3DRightClickListener
{
    private J3DPanel      panel;
    private SplitTest2D   s2d;

    public J3DPanel getPanel ()
    {
        return panel;
    }

    private ArrayList<J3DLine> commonLines = null;

    private ArrayList<? extends J3DSurfacePatch> slist = null;
    private ArrayList<SurfPoints> surfPointsList = null;
    private String    sname = null;

    public Simple3DFrame ()
    {
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        setTitle ("PatchSplit 3D Test");
        setSize (700, 700);
        setResizable (true);

        View3DProperties vp = new View3DProperties ();
        vp.setVerticalExaggeration (2.0);
        panel = new J3DPanel (vp, true);

        panel.addRightClickListener (this);

        s2d = new SplitTest2D ();
        s2d.show ();

        Container contentPane = getContentPane ();
        contentPane.add (panel);

        surfPointsList = new ArrayList<SurfPoints> (20);
        slist = new ArrayList<? extends J3DSurfacePatch> (20);
        commonLines = new ArrayList<J3DLine> (20);

        createInitialSurfaces ();

        clearAndRedraw ();

    }

    void resetAll ()
    {
        slist.clear ();
        commonLines.clear ();
        surfPointsList.clear ();
        clearAndRedraw ();
    }

    void addToCommonLines (J3DLine j3dline)
    {
        if (j3dline == null) {
            return;
        }

        commonLines.add (j3dline);

    }

    void clearAndRedraw ()
    {
        J3DSurfacePatch     sp;
        J3DFault       fp;
        J3DHorizonPatch     hp;

        panel.clearAll (false);

        LightData light = panel.getDrawingAttributesForTest().getLight1Data ();
        light.setBrightness(100);
        panel.getDrawingAttributesForTest().setLight1Data (light);

        light = panel.getDrawingAttributesForTest().getLight2Data ();
        light.setBrightness(25);
        light.setEnabled(true);
        panel.getDrawingAttributesForTest().setLight2Data (light);

        panel.getDrawingAttributesForTest().setDoWireFrame (false);

        int type = 0;
        int size = slist.size();
        for (int i=0; i<size; i++) {
            sp = slist.get(i);
            type = sp.getType ();
            if (type == 2) {
                fp = (J3DFault)sp;
                panel.addFault (fp);
            }
            else {
                hp = (J3DHorizonPatch)sp;
                panel.addHorizonPatch (hp);
            }
        }

        size = faultList.size ();
        for (int i=0; i<size; i++) {
            fp = faultList.get (i);
            panel.addFault (fp);
        }

        panel.repaint ();
    }


    void createSurface (double[] xpts,
                        double[] ypts,
                        double[] zpts,
                        int npts,
                        Color color,
                        boolean faultFlag)
    {
        J3DHorizonPatch    hp;
        J3DFault      fp;
        int                i;

        JSurfaceWorks sw = new JSurfaceWorks ();

        TriMesh tmesh = sw.calcTriMesh
        (
            xpts,
            ypts,
            zpts,
            npts,
            null,
            0,
            null,
            1,
            null,
            null
        );

        NodeArray3D na = new NodeArray3D (xpts, ypts, zpts);

        if (faultFlag) {
            fp = new J3DFault ();
            fp.setTriMesh (tmesh);
            fp.setGlobalColor (color);
            fp.setSurfaceName (sname);
            fp.setOriginalNodes (na);
            slist.add (fp);
        }
        else {
            hp = new J3DHorizonPatch ();
            hp.setTriMesh (tmesh);
            hp.setGlobalColor (color);
            hp.setSurfaceName (sname);
            hp.setOriginalNodes (na);
            slist.add (hp);
        }

    }

    private double xsurf, ysurf, zsurf;

    private J3DFault   newFault;

    private ArrayList<J3DFault>  faultList = 
		new ArrayList<J3DFault> (1);

/*-------------------------------------------------------------------------------------*/

    public void applyPreviewFault ()
    {
        newFault = new J3DFault ();

        panel.setPreviewFault (null);
        newFault = null;
        showEditSurface (false);

        panel.addFault (newFault);
        newFault.setSurfaceID (faultList.size());
        faultList.add (newFault);
        newFault = null;

        panel.unselectAllLines ();
        panel.redrawAll ();

    }

    private int    numNew = 0;

    public void newHorizonFromFile ()
    {
        boolean istat = pointsFromFile ();
        if (istat == false) {
            return;
        }

        if (pointList == null) {
            return;
        }

        int   r, g, b;
        int nc = numNew % 8;

        if (nc == 0) {
            r = 32;
            g = 32;
            b = 255;
        }
        else if (nc == 1) {
            r = 32;
            g = 255;
            b = 32;
        }
        else if (nc == 2) {
            r = 255;
            g = 32;
            b = 32;
        }
        else if (nc == 3) {
            r = 32;
            g = 255;
            b = 255;
        }
        else if (nc == 4) {
            r = 255;
            g = 32;
            b = 255;
        }
        else if (nc == 5) {
            r = 255;
            g = 255;
            b = 32;
        }
        else if (nc == 6) {
            r = 64;
            g = 128;
            b = 172;
        }
        else {
            r = 172;
            g = 128;
            b = 64;
        }

        Color color = new Color (r, g, b, 200);

        createSurface (
            pointList.x,
            pointList.y,
            pointList.z,
            pointList.npts,
            color,
            false);

        numNew ++;

        clearAndRedraw ();
    }

    DBPointList       pointList;

    private boolean pointsFromFile ()
    {
        String answer;

        answer = JOptionPane.showInputDialog (this, "Enter points filename");
        if (answer == null) {
            return false;
        }
        if (answer.length() < 1) {
            return false;
        }

        pointList = new DBPointList ();

        try {
            pointList.loadAsciiFile (answer);
        }
        catch (Exception e) {
            pointList = null;
            JOptionPane.showMessageDialog (
                this,
                "Error opening or reading this file"
            );
            return false;
        }

        return true;
    }

    private double calcExtendFraction (TriMesh tmesh)
    {
        double          fraction;

        fraction = 0.1;

        boolean isSteep = false;

        if (tmesh.vBase != null) {
            isSteep = tmesh.vBase.isUsed ();
        }

        double tzmin = tmesh.getZMin();
        double tzmax = tmesh.getZMax();

        double mzmin = 1.e30;
        double mzmax = -1.e30;

        J3DSurfacePatch  sp;
        TriMesh          tm;
        double           z1, z2;

        int size = slist.size();
        for (int i=0; i<size; i++) {
            sp = (J3DSurfacePatch)slist.get (i);
            tm = sp.getTriMesh ();
            if (tm == null) {
                continue;
            }
            z1 = tm.getZMin();
            z2 = tm.getZMax();
            if (z1 < mzmin) mzmin = z1;
            if (z2 > mzmax) mzmax = z2;
        }

        z1 = mzmax - tzmax;
        z2 = tzmin - mzmin;
        if (z1 > z2) {
            if (z1 > 0.0) {
                fraction = z1 / (tzmax - tzmin);
            }
        }
        else {
            if (z2 > 0.0) {
                fraction = z2 / (tzmax - tzmin);
            }
        }

        if (isSteep) {
            fraction *= 1.1;
        }
        else {
            fraction *= 2.0;
        }

        return fraction;

    }

/*----------------------------------------------------------------------*/

    private double       xMin, yMin, xMax, yMax;

    public void splitSurf ()
    {
        ArrayList<J3DHorizonPatch> hlist = panel.getSelectedHorizons ();
        if (hlist == null) {
            return;
        }

        if (hlist.size() != 1) {
            return;
        }

        JSurfaceWorks jsw = new JSurfaceWorks ();
        jsw.psClearAllData ();

        J3DSurfacePatch    sp;
        J3DFault      fp;
        J3DHorizonPatch    hp;
        J3DLine            j3dline;

        int                type;

        s2d.clearData ();
        s2d.initPlot ();

    /*
     * Send all the faults to the patch split algorithm.
     */
System.out.println ("sending faults to native");
        int size = faultList.size();
        for (int i=0; i<size; i++) {
            fp = faultList.get(i);
            jsw.psAddFaultSurface (
                fp.getTriMesh (),
                fp.getSurfaceID (),
                0, -1.0, -1.0);
        }

    /*
     * Use the bounding box of the currently selected horizon as the initial border.
     */
        hp = hlist.get (0);
        TriMesh tmesh = hp.getTriMesh ();
        xMin = tmesh.getXMin ();
        yMin = tmesh.getYMin ();
        xMax = tmesh.getXMax ();
        yMax = tmesh.getYMax ();

        double[] xb = new double[5];
        double[] yb = new double[5];
        xb[0] = xMin;
        yb[0] = yMin;
        xb[1] = xMax;
        yb[1] = yMin;
        xb[2] = xMax;
        yb[2] = yMax;
        xb[3] = xMin;
        yb[3] = yMax;
        xb[4] = xMin;
        yb[4] = yMin;
System.out.println ("sending border to native");
        jsw.psAddBorderSegment (xb, yb, null, 5, 0, 0);
        s2d.updateBorderData (xb, yb, 5);

    /*
     * Add centerlines and points for the current selected horizon to the
     * patch split algorithm.
     */
        PatchPair pair;
        XYZPolyline la;
        size = commonLines.size();
        for (int i=0; i<size; i++) {
            j3dline = (J3DLine)commonLines.get (i);
            if (j3dline == null) continue;
            pair = (PatchPair)j3dline.getUserData ();
            if (pair == null) continue;
            if (pair.sp1 == hp  &&  pair.sp2 != null) {
                ArrayList<XYZPolyline> plist = j3dline.getDrapedLineList ();
                s2d.updateClineData (plist);
                int psize = plist.size ();
                for (int j=0; j<psize; j++) {
                    XYZPolyline pl = (XYZPolyline)(plist.get (j));
                    if (pl == null) {
                        continue;
                    }
System.out.println ("sending centerline to native");
                    jsw.psAddFaultCenterline (
                        pl,
                        pair.sp2.getSurfaceID(),
                        0);
                }
                SurfPoints spt = findSurfPoints (hp);
                splitColor = hp.getGlobalColor ();
                jsw.psSetPoints (spt.x, spt.y, spt.z, spt.npts);
                s2d.updatePoints (spt.x, spt.y, spt.z, spt.npts);
            }
        }

        panel.unselectAllHorizons ();
        s2d.repaint ();

    /*
     * Call the native function to do the patch split.
     */
System.out.println ("calling patch split");
        jsw.psSetDebugState (1);
        jsw.psCalcSplit (false, 0, 0.0);

    /*
     * Retrieve and draw results.
     */
        ArrayList<XYZPolyline> cl = jsw.getCorrectedCenterlineList ();
     //   s2d.drawExtendedCenterlines (cl);
        drawCenterlines (cl);

        ArrayList<XYZPolyline> cl2 = jsw.getExtendedCenterlineList ();
     //   s2d.drawExtendedCenterlines (cl2);

        ArrayList<XYZPolyline> pl = jsw.getWorkPolyList ();
     //   s2d.drawWorkPolys (pl);

        ArrayList<DBContactLine> clist = jsw.getProtoPatchContactLineList ();
        drawContactLines (clist);

        ArrayList<DBDrapeLine> dlist = jsw.getDrapeLineList ();
        //drawDrapeLines (dlist);

        ArrayList<TriMesh> eflist = jsw.getEfaultTmeshList ();
        drawExtendedFaults (eflist);

     // hp.setOpaque (0.5);
        hp.setVisible (false);

        ArrayList<ProtoPatch> prlist = jsw.getProtoPatchList ();
        drawProtoPatches (prlist);

        panel.redrawAll ();

    }

/*----------------------------------------------------------------------*/

    public void editCenterlines ()
    {
        panel.digitizeLineGeometry(this, DigitizingType.POLYLINE);
        panel.setPickModeMessage ("Creating fault centerlines.");
    }

/*----------------------------------------------------------------------------------*/

    public void endEditCenterlines ()
    {
        panel.setPickModeMessage (null);

        panel.setSelectionMode();
    }


/*----------------------------------------------------------------------------------*/

/*
 * Implement the J3DLineEditListener methods.
 */

  /**
  Method called from J3DPanel when a point in a line has been
  added, deleted or moved.  This is not used in the unit test.
  */
    public void lineChanged (J3DLine line3d)
    {
    }

  /**
  Method called from J3DPanel while a point is being incrementally moved.
  This is not used in the unit test.
  */
    public void lineMoved (J3DLine line3d)
    {
    }

  /**
  Method called from J3DPanel when a particular line is finished being edited.
  This can be called when the end edit popup menu item is selected, or when
  a double click is done to signal the end of inserting a line, or when a
  line on a new surface is started, which implicitly ends the previous line.
  */
    public void lineFinished (J3DLine line3d)
    {
        if (line3d == null) {
            return;
        }
        XYZPolyline la = line3d.getLine ();
        if (la.size() < 2) {
            return;
        }

        PatchPair pair = new PatchPair ();
        pair.type = line3d.getType ();
        pair.sp1 = line3d.getDrapingSurface ();
        line3d.setUserData (pair);

        panel.addLine (line3d);
        addToCommonLines (line3d);

        return;
    }

  /**
  Method called from J3DPanel when the editing mode is completely finished.
  */
    public void editCompletelyFinished ()
    {
    }

/*---------------------------------------------------------------------------------*/

/*
 * Implement the J3DRightClickListener methods.
 */
    public void processRightClick (J3DRightClickInfo info)
    {
    }



/*--------------------------------------------------------------------------*/

    public void showEditSurface (boolean draw)
    {
        int               i, n;
        double[]          xa;
        double[]          ya;
        double[]          za;

        if (newFault == null) {
            newFault = new J3DFault ();
        }

        NodeArray3D     na = null;
        JPoint3D        p;

        ArrayList selectedLines = panel.getSelectedLines ();
        if (selectedLines == null) {
            newFault = null;
            return;
        }
        if (selectedLines.size() < 2) {
            newFault = null;
            return;
        }

        JSurfaceWorks jsw = new JSurfaceWorks ();

        boolean faultFlag = false;
        jsw.setBadBoundaryAction (2);
        faultFlag = true;

        XYZPolyline        la;
        ArrayList<XYZPolyline>    lalist, dlist;

        J3DLine   jl;
        int size = selectedLines.size();
        lalist = new ArrayList<XYZPolyline> (size);
        for (i=0; i<size; i++) {
            jl = (J3DLine)selectedLines.get(i);
            dlist = jl.getDrapedLineList ();
            if (dlist == null) {
                la = jl.getLine ();
                lalist.add (la);
            }
            else {
                int ndlist = dlist.size();
                for (int j=0; j<ndlist; j++) {
                    la = (XYZPolyline)dlist.get (j);
                    lalist.add (la);
                }
            }

          /*
           * When draw is false, the patch is being set up for
           * possible use as a fault in splitting, so the new
           * fault patch is paired with the horizon (already
           * setup) in the PatchPair user data.
           */
            if (draw == false) {
                Object uobj = jl.getUserData ();
                if (uobj != null  &&  uobj instanceof PatchPair) {
                    PatchPair pair = (PatchPair)uobj;
                    pair.sp2 = newFault;
                }
            }
        }

        TriMesh tmesh = null;
        tmesh =
        jsw.calcTriMesh (
          na,
          lalist,
          null,
          1,
          null,
          null,
          faultFlag);

        newFault.setVisible (true);
        newFault.setTriMesh (tmesh);

        if (draw) {
            panel.setPreviewFault (newFault);
            panel.redrawAll ();
        }

        return;

    }

/*--------------------------------------------------------------------------*/

    public void hideEditSurface ()
    {
        int               i, n;
        double[]          xa;
        double[]          ya;
        double[]          za;

        if (newFault == null) {
            newFault = new J3DFault ();
        }
        newFault.setVisible (false);

        panel.redrawAll ();

    }


    void createInitialSurfaces ()
    {

        panel.clearAll (false);
        s2d.clearData ();
        s2d.initPlot ();

        panel.getDrawingAttributesForTest().setVerticalExageration (2.0);

        LightData light = panel.getDrawingAttributesForTest().getLight1Data ();
        light.setBrightness(100);
        panel.getDrawingAttributesForTest().setLight1Data (light);

        light = panel.getDrawingAttributesForTest().getLight2Data ();
        light.setBrightness(25);
        light.setEnabled(true);
        panel.getDrawingAttributesForTest().setLight2Data (light);

        panel.getDrawingAttributesForTest().setDoWireFrame (false);

        slist = new ArrayList<? extends J3DSurfacePatch> (20);

        Color  color;

        color = new Color (150, 150, 150, 200);
        sname = "top surface";
        createInitialSurface (0.0, color, false);

        color = new Color (255, 0, 0, 200);
        sname = "middle surface";
        createInitialSurface (-20.0, color, false);

        color = new Color (0, 0, 255, 200);
        sname = "bottom surface";
        createInitialSurface (-50.0, color, false);

        xsurf = -20.0;
        ysurf = -20.0;
        zsurf = -12.00;

        panel.redrawAll ();

    }


    void createInitialSurface (double zmid,
                        Color color, boolean faultFlag)
    {
        J3DHorizonPatch    hp;
        J3DFault      fp;
        J3DSurfacePatch    sp;
        int                i, n;

        double    dx = 150.0;
        double    dy = 100.0;
        double    dz = 4.0;
        double    xt, yt;

        double[] x = new double[100];
        double[] y = new double[100];
        double[] z = new double[100];

        Random random = new Random();
        random.setSeed (1234567);

        double xmin = 1.e30;
        double ymin = 1.e30;
        double xmax = -1.e30;
        double ymax = -1.e30;

        n = 0;
        for (i=0; i<30; i++) {
            xt = random.nextDouble () * dx;
            if (xt > 40) {
                continue;
            }
            yt = random.nextDouble () * dy;
            x[n] = xt;
            y[n] = yt;
            z[n] = random.nextDouble () * dz + zmid;
            n++;
            if (x[n] < xmin) xmin = x[n];
            if (y[n] < ymin) ymin = y[n];
            if (x[n] > xmax) xmax = x[n];
            if (y[n] > ymax) ymax = y[n];
        }

        for (i=0; i<30; i++) {
            xt = random.nextDouble () * dx;
            if (xt < 55  ||  xt > 95) {
                continue;
            }
            yt = random.nextDouble () * dy;
            x[n] = xt;
            y[n] = yt;
            z[n] = random.nextDouble () * dz + zmid - 5.0;
            n++;
            if (x[n] < xmin) xmin = x[n];
            if (y[n] < ymin) ymin = y[n];
            if (x[n] > xmax) xmax = x[n];
            if (y[n] > ymax) ymax = y[n];
        }

        for (i=0; i<30; i++) {
            xt = random.nextDouble () * dx;
            if (xt < 110) {
                continue;
            }
            yt = random.nextDouble () * dy;
            x[n] = xt;
            y[n] = yt;
            z[n] = random.nextDouble () * dz + zmid + 5.0;
            n++;
            if (x[n] < xmin) xmin = x[n];
            if (y[n] < ymin) ymin = y[n];
            if (x[n] > xmax) xmax = x[n];
            if (y[n] > ymax) ymax = y[n];
        }

        double tiny = (xmax - xmin + ymax - ymin) /40.0;
        xmin -= tiny;
        ymin -= tiny;
        xmax += tiny;
        ymax += tiny;

        JSurfaceWorks sw = new JSurfaceWorks ();

        GridGeometry geom =
        new GridGeometry (100, 100, xmin, ymin, xmax, ymax);

        TriMesh tmesh = sw.calcTriMesh
        (
            x,
            y,
            z,
            n,
            null,
            0,
            null,
            1,
            null,  // geom
            null
        );

        if (faultFlag) {
            fp = new J3DFault ();
            fp.setTriMesh (tmesh);
            fp.setGlobalColor (color);
            fp.setSurfaceName (sname);
            panel.addFault (fp);
            slist.add (fp);
            sp = fp;
        }
        else {
            hp = new J3DHorizonPatch ();
            hp.setTriMesh (tmesh);
            hp.setGlobalColor (color);
            hp.setSurfaceName (sname);
            panel.addHorizonPatch (hp);
            slist.add (hp);
            sp = hp;
        }

        SurfPoints spt = new SurfPoints ();
        spt.x = x;
        spt.y = y;
        spt.z = z;
        spt.npts = n;
        spt.sp = sp;

        surfPointsList.add (spt);

        s2d.updatePoints (x, y, z, n);
        s2d.repaint ();

    }

/*-------------------------------------------------------------*/

    private SurfPoints findSurfPoints (J3DSurfacePatch sp)
    {
        SurfPoints spt;

        int size = surfPointsList.size ();
        for (int i=0; i<size; i++) {
            spt = surfPointsList.get (i);
            if (spt.sp.equals(sp)) {
                return spt;
            }
        }
        return null;
    }

/*-------------------------------------------------------------*/

    private void drawContactLines (ArrayList<DBContactLine> clist)
    {
        int              i, size;
        DBContactLine    dbcl;
        XYZPolyline      pxyz;
        J3DLine          jline;

        if (clist == null) {
            return;
        }

        size = clist.size ();
        for (i=0; i<size; i++) {
            dbcl = clist.get (i);
            if (dbcl == null) {
                continue;
            }

            pxyz = new XYZPolyline (dbcl);

            jline = new J3DLine ();
            jline.setLine (pxyz);
            jline.setHidden (false);
            jline.setGlobalColor (Color.black);
            jline.setLineID (3);
            jline.setLineWidth (3.0);
            panel.addLine (jline);
        }

        return;
    }

/*-------------------------------------------------------------*/

    private void drawCenterlines (ArrayList<XYZPolyline> cl)
    {
        int              i, size;
        XYZPolyline      pxyz;
        J3DLine          jline;

        if (cl == null) {
            return;
        }

        size = cl.size ();
        for (i=0; i<size; i++) {

            pxyz = (XYZPolyline)cl.get(i);

            jline = new J3DLine ();
            jline.setLine (pxyz);
            jline.setHidden (false);
            jline.setGlobalColor (Color.blue);
            jline.setLineID (3);
            jline.setLineWidth (3.0);
            panel.addLine (jline);
        }

        return;
    }

/*-------------------------------------------------------------*/

    private void drawDrapeLines (ArrayList<DBDrapeLine> dlist)
    {
        int              i, size;
        DBDrapeLine      dbcl;
        XYZPolyline      pxyz;
        J3DLine          jline;

        if (dlist == null) {
            return;
        }

        size = dlist.size ();
        for (i=0; i<size; i++) {
            dbcl = dlist.get (i);
            if (dbcl == null) {
                continue;
            }

            pxyz = new XYZPolyline (dbcl);

            jline = new J3DLine ();
            jline.setLine (pxyz);
            jline.setHidden (false);
            jline.setGlobalColor (Color.green);
            jline.setLineID (4);
            jline.setLineWidth (1.0);
            panel.addLine (jline);
        }

        return;
    }


/*-------------------------------------------------------------*/

    private void drawExtendedFaults (ArrayList<TriMesh> flist)
    {
        int              i, size;
        J3DFault    fp;
        TriMesh          tmesh;

        if (flist == null) {
            return;
        }

        size = flist.size ();
        for (i=0; i<size; i++) {
            tmesh = flist.get (i);
            if (tmesh == null) {
                continue;
            }
            fp = new J3DFault ();
            fp.setTriMesh (tmesh);
            fp.setGlobalColor (Color.gray);
            panel.addFault (fp);
        }

        return;
    }

/*-------------------------------------------------------------*/

    private Color splitColor;

    private void drawProtoPatches (ArrayList<ProtoPatch> flist)
    {
        int              i, size;
        J3DHorizonPatch  hp;
        TriMesh          tmesh;
        ProtoPatch       patch;

        if (flist == null) {
            return;
        }

        size = flist.size ();
        for (i=0; i<size; i++) {
            patch = flist.get (i);
            tmesh = patch.getTriMesh ();
            if (tmesh == null) {
                continue;
            }
            hp = new J3DHorizonPatch ();
            hp.setTriMesh (tmesh);
            hp.setGlobalColor (splitColor);
            hp.setOpaque (1.0);
            panel.addHorizonPatch (hp);
            String name = "proto_patch"+i;
            s2d.drawTriMesh (tmesh, name);
        }

        return;
    }


/*----------------------------------------------------------------*/



};  // end of Simple3DFrame class
