
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.awt.Color;
import java.awt.Container;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Random;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.WindowConstants;

import csw.jutils.src.XYZPolyline;
import csw.jutils.src.LightData;
import csw.jutils.src.View3DProperties;

import csw.jsurfaceworks.src.GridGeometry;
import csw.jsurfaceworks.src.JSurfaceWorks;
import csw.jsurfaceworks.src.TriMesh;

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
import csw.j3d.src.JPoint3D;


/**
 *
 *  This is used for 3d display in the program for debugging
 *  splitting and sealing.
 *
 *  @author Glenn Pinkerton
 *
 */

class J3DFrame extends JFrame
{
    private static final long serialVersionUID = 1396511L;

    public J3DFrame ()
    {

        final SimplestFrame frame = new SimplestFrame ();

        frame.setVisible (true);

        setTitle ("J3D Unit Test");
        setSize (200, 400);
        setResizable (false);
        Container contentPane = getContentPane ();
        contentPane.setLayout (new GridLayout(15,1));

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

        JButton apply_button = new JButton ("Apply");
        apply_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.endSurfaceEdit ();
                }
            }
        });

        JButton hidesurf_button = new JButton ("Hide Selected Surfaces");
        hidesurf_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.hideSelectedSurfaces ();
                }
            }
        });

        JButton unhidesurf_button = new JButton ("Show All Surfaces");
        unhidesurf_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.unhideAllSurfaces ();
                }
            }
        });

        contentPane.add (cline_button);
        contentPane.add (end_button);
        contentPane.add (preview_button);
        contentPane.add (hide_button);
        contentPane.add (apply_button);
        contentPane.add (hidesurf_button);
        contentPane.add (unhidesurf_button);
    }

}


class SimplestFrame extends JFrame
   implements J3DLineEditListener, J3DRightClickListener, J3DSelectListener
{

    private static final long serialVersionUID = 92337651L;

    private J3DPanel      panel;

    private ArrayList<J3DFault> flist;
    private ArrayList<J3DHorizonPatch> hlist;
    private String    sname;

    public SimplestFrame ()
    {
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        setTitle ("Simplest Test");
        setSize (700, 700);
        setResizable (true);

        panel = new J3DPanel (this);
        View3DProperties  v3p = new View3DProperties ();
        panel.setView3DProperties (v3p);

        //panel.addLineEditListener (this);
        panel.addRightClickListener (this);

        Container contentPane = getContentPane ();
        contentPane.add (panel);

        clearAndRedraw ();
    }

    void clearAndRedraw ()
    {

        panel.clearAll (false);

        //panel.getDrawingAttributesForTest().setVerticalExageration (2.0);

        //LightData light = panel.getDrawingAttributesForTest().getLight1Data ();
        //light.setBrightness(100);
        //panel.getDrawingAttributesForTest().setLight1Data (light);

        //light = panel.getDrawingAttributesForTest().getLight2Data ();
        //light.setBrightness(25);
        //light.setEnabled(true);
        //panel.getDrawingAttributesForTest().setLight2Data (light);

        //panel.getDrawingAttributesForTest().setDoWireFrame (false);

        flist = new ArrayList<J3DFault> (20);
        hlist = new ArrayList<J3DHorizonPatch> (20);

        Color  color;

        color = new Color (150, 150, 150, 200);
        sname = "top surface";
        createSurface (0.0, color, false);

        color = new Color (255, 0, 0, 200);
        sname = "middle surface";
        createSurface (-20.0, color, false);

        color = new Color (0, 255, 0, 200);
        sname = "bottom surface";
        createSurface (-50.0, Color.green, false);

        xsurf = -20.0;
        ysurf = -20.0;
        zsurf = -12.00;

        panel.redrawAll ();

    }


    void createSurface (double zmid,
                        Color color, boolean faultFlag)
    {
        J3DHorizonPatch    hp;
        J3DFault      fp;
        int                i, n;

        double    dx = 100.0;
        double    dy = 100.0;
        double    dz = 2.0;
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
            xt = random.nextDouble () * dx * .3;
            yt = random.nextDouble () * dy * .3;
            if (yt > -xt + 30) {
                continue;
            }
            x[n] = xt;
            y[n] = yt;
            z[n] = random.nextDouble () * dz + zmid;
            n++;
            if (x[i] < xmin) xmin = x[i];
            if (y[i] < ymin) ymin = y[i];
            if (x[i] > xmax) xmax = x[i];
            if (y[i] > ymax) ymax = y[i];
        }

        for (i=0; i<30; i++) {
            xt = random.nextDouble () * dx;
            yt = random.nextDouble () * dy;
            if (yt < -xt + 40) {
                continue;
            }
            if (yt > -xt + 60) {
                continue;
            }
            x[n] = xt;
            y[n] = yt;
            z[n] = random.nextDouble () * dz + zmid - 2.0;
            n++;
            if (x[i] < xmin) xmin = x[i];
            if (y[i] < ymin) ymin = y[i];
            if (x[i] > xmax) xmax = x[i];
            if (y[i] > ymax) ymax = y[i];
        }

        for (i=0; i<30; i++) {
            xt = random.nextDouble () * dx;
            yt = random.nextDouble () * dy;
            if (yt < -xt + 70) {
                continue;
            }
            x[n] = xt;
            y[n] = yt;
            z[n] = random.nextDouble () * dz + zmid + 2.0;
            n++;
            if (x[i] < xmin) xmin = x[i];
            if (y[i] < ymin) ymin = y[i];
            if (x[i] > xmax) xmax = x[i];
            if (y[i] > ymax) ymax = y[i];
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
            flist.add (fp);
        }
        else {
            hp = new J3DHorizonPatch ();
            hp.setTriMesh (tmesh);
            hp.setGlobalColor (color);
            hp.setSurfaceName (sname);
            panel.addHorizonPatch (hp);
            hlist.add (hp);
        }

    }

    private double xsurf, ysurf, zsurf;

/*----------------------------------------------------------------------------------*/

    private J3DFault newFault = null;
    private boolean editActive = false;

    private boolean linePickFlag = true;

    private static final int NEW_FAULT_PATCH_MODE = 1;
    private static final int NEW_HORIZON_PATCH_MODE = 2;

/*----------------------------------------------------------------------------------*/

    public void editCenterlines ()
    {
        if (editActive) {
            return;
        }

        //panel.digitizeLineGeometry(this, DigitizingType.POLYLINE);
        panel.setPickModeMessage ("Creating fault centerlines.");
    }

/*----------------------------------------------------------------------------------*/

    public void endEditCenterlines ()
    {
        panel.setPickModeMessage (null);

        editActive = false;
        panel.setSelectionMode();
    }

/*----------------------------------------------------------------------------------*/

    public void endSurfaceEdit ()
    {

        J3DFault     fp;
        J3DHorizonPatch   hp;

        showEditSurface (false);
        panel.addFault (newFault);
        newFault = null;
        panel.setPreviewFault (null);

        panel.setSelectionMode();

        panel.redrawAll ();

        editActive = false;

        return;
    }

    private void cancelSurfaceEdit ()
    {

        newFault = null;

        eraseEditLines ();

        panel.redrawAll ();

        editActive = false;

        return;
    }

    private void resetSurfaceEdit ()
    {

        if (newFault == null) {
            newFault = new J3DFault ();
        }

        newFault.setVisible (false);

        eraseEditLines ();

        panel.redrawAll ();

        return;
    }

    private ArrayList<J3DLine>  constraintLineList = null;

    private void eraseEditLines ()
    {
        int        i, size;
        J3DLine    line;

        if (constraintLineList == null) {
            return;
        }

        size = constraintLineList.size();

        for (i=0; i<size; i++) {
            line = constraintLineList.get (i);
            panel.removeLine (line);
        }

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

        JPoint3D        p;

        ArrayList<J3DLine> selectedLines = panel.getSelectedLines ();
        if (selectedLines == null) {
            return;
        }
        if (selectedLines.size() < 2) {
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
            jl = selectedLines.get(i);
            dlist = jl.getDrapedLineList ();
            if (dlist == null) {
                la = jl.getLine ();
                lalist.add (la);
            }
            else {
                int ndlist = dlist.size();
                for (int j=0; j<ndlist; j++) {
                    la = dlist.get (j);
                    lalist.add (la);
                }
            }
        }

        TriMesh tmesh = null;
        tmesh =
        jsw.calcTriMesh (
          null,
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

/*-------------------------------------------------------------------------------*/

    public void hideSelectedSurfaces ()
    {
        panel.hideSelectedObjects ();
        panel.redrawAll ();
    }

/*-------------------------------------------------------------------------------*/

    public void unhideAllSurfaces ()
    {
        panel.showAllObjects ();
        panel.redrawAll ();
    }

/*-------------------------------------------------------------------------------*/

    private JButton makeJButton (String txt)
    {
        JButton       jb;
        jb = new JButton (txt);
        jb.setBorderPainted (false);
        jb.setMargin (new Insets (3, 0, 3, 0));
        return jb;
    }

    private JCheckBox makeJCheckBox (String txt, boolean b)
    {
        JCheckBox     jb;
        jb = new JCheckBox (txt, b);
        jb.setBorderPainted (false);
        jb.setMargin (new Insets (3, 0, 3, 0));
        return jb;
    }

/*--------------------------------------------------------------------------------*/

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

        constraintLineList.add (line3d);

        panel.addLine (line3d);

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
 * Implement the J3DSelectListener methods
 */
    public void selectionChanged (J3DSelectionChangedInfo info)
    {
    }

    public void pickModeChanged ()
    {
    }


/*----------------------------------------------------------------------------------*/

/*
 * Implement the J3DRightClickListener methods.
 */
    public void processRightClick (J3DRightClickInfo info)
    {
    }

/*----------------------------------------------------------------------------------*/

}
