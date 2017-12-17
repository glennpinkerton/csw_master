
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jtest;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;
import java.util.Random;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.WindowConstants;

import csw.jsurfaceworks.src.*;

import csw.jutils.src.XYPolyline;
import csw.jutils.src.XYZPolyline;
import csw.jutils.src.ColorPalette;

import csw.jeasyx.src.DLConst;
import csw.jeasyx.src.DLSurfaceProperties;
import csw.jeasyx.src.JDisplayList;
import csw.jeasyx.src.JEasyXGraphicsPanel;

/**
 *
 * This class is used as a main entry for unit testing JSurfaceWorks stuff.
 *
 *  @author Glenn Pinkerton
 *
*/
public class JSWUnitTest {

    static {
        System.loadLibrary ("_csw_all");
    }

    public static void main (String[] args) {

        Runnable run = new Runnable ()
        {
            public void run ()
            {
                JSWUnitTest.runMainMethod ();
            }
        };

        SwingUtilities.invokeLater (run);
    }


    public static void runMainMethod ()
    {

        try {
          UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch(Exception e) {
          System.out.println("Error setting native LAF: " + e);
        }

        JSWUnitTestFrame frame = new JSWUnitTestFrame ();
        frame.setDefaultCloseOperation (JFrame.EXIT_ON_CLOSE);
        frame.setVisible (true);

    }

}

class JSWUnitTestFrame extends JFrame
{

	private static final long serialVersionUID = 1L;

    public JSWUnitTestFrame ()
    {


/*
try {
int idum = System.in.read();
}
catch (Exception e) {
}
*/


        String   cpar = System.getenv ("CSW_PARENT");
        if (cpar == null) {
            cpar = "/home/gpinkerton/gitcsw";
        }
        else {
            if (cpar.isEmpty()) {
                cpar = "/home/gpinkerton/gitcsw";
            }
        }

        String  ezx_playback_file_name = cpar + "/csw/jtest/utest.ezx";
        String  sw_playback_file_name = cpar + "/csw/jtest/utest.sw";

        JDisplayList.openLogFile (ezx_playback_file_name);
        JSurfaceWorks.openLogFile (sw_playback_file_name);

        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        setTitle ("JSurfaceWorks Unit Test");
        setSize (200, 400);
        setResizable (false);
        Container contentPane = getContentPane ();
        contentPane.setLayout (new GridLayout(15,1));

        JButton trimesh_10_button = new JButton ("100 Point TriMesh");
        trimesh_10_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                TriMesh10Frame frame = new TriMesh10Frame ();
                frame.setVisible (true);
          }
        });

        JButton grid_10_button = new JButton ("100 Point Grid");
        grid_10_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                Grid10Frame frame = new Grid10Frame ();
                frame.setVisible (true);
          }
        });

        JButton trimesh_file_button = new JButton ("debug trimesh file");
        trimesh_file_button.setEnabled (false);
        trimesh_file_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                TFileFrame frame = new TFileFrame ();
                frame.setVisible (true);
          }
        });

        JButton grid_file_button = new JButton ("debug grid file");
        grid_file_button.setEnabled (false);
        grid_file_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                GridFileFrame frame = new GridFileFrame ();
                frame.setVisible (true);
          }
        });

        JButton divide_file_button = new JButton ("debug subdivide file");
        divide_file_button.setEnabled (false);
        divide_file_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                DivideFrame frame = new DivideFrame ();
                frame.setVisible (true);
          }
        });

        JButton tmesh_fileio_button = new JButton ("trimesh fileio test");
        tmesh_fileio_button.setEnabled (false);
        tmesh_fileio_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                FileioFrame frame = new FileioFrame ();
                frame.setVisible (true);
          }
        });

        JButton pt_file_button = new JButton ("points and constraints");
        pt_file_button.setEnabled (false);
        pt_file_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                PFileFrame frame = new PFileFrame ();
                frame.setVisible (true);
          }
        });

        JButton tmesh_contour_button = new JButton ("trimesh contour");
        tmesh_contour_button.setEnabled (false);
        tmesh_contour_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                ContourFrame frame = new ContourFrame ();
                frame.setVisible (true);
          }
        });

        JButton bug_file_button = new JButton ("read bug files");
        bug_file_button.setEnabled (false);
        bug_file_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            JSurfaceWorks jsw = new JSurfaceWorks ();
            jsw.readBugFiles ();
          }
        });

        JButton speed_button = new JButton ("trimesh performance test");
        speed_button.setEnabled (false);
        speed_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            JSurfaceWorks jsw = new JSurfaceWorks ();
            jsw.testTriSpeed (1000000);
          }
        });

        JButton resample_button = new JButton ("line decimation test");
        resample_button.setEnabled (false);
        resample_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
              ResampleFrame frame = new ResampleFrame ();
              frame.setVisible (true);
          }
        });

        contentPane.add (trimesh_10_button);
        contentPane.add (grid_10_button);
        contentPane.add (trimesh_file_button);
        contentPane.add (divide_file_button);
        contentPane.add (grid_file_button);
        contentPane.add (tmesh_fileio_button);
        contentPane.add (tmesh_contour_button);
        contentPane.add (pt_file_button);
        contentPane.add (bug_file_button);
        contentPane.add (resample_button);
    }

}

class TriMesh10Frame extends JFrame
{

	private static final long serialVersionUID = 1L;

    public TriMesh10Frame ()
    {
        JSurfaceWorks      sw;
        Random             random;
        double[]           x, y, z;
        int                i;

        random = new Random();
        long  ctw = System.currentTimeMillis ();
        random.setSeed (ctw);
        int np = random.nextInt (1000) + 100;

        setTitle ("TriMesh Test " + np);
        setSize (900, 700);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        JEasyXGraphicsPanel gpanel = new JEasyXGraphicsPanel ();

        Container contentPane = getContentPane ();
        contentPane.add (gpanel);

        JDisplayList dl = gpanel.getDisplayList ();
        sw = new JSurfaceWorks ();

        x = new double[2000];
        y = new double[2000];
        z = new double[2000];

        for (i=0; i<np; i++) {
            x[i] = random.nextDouble () * 100.0;
            y[i] = random.nextDouble () * 100.0;
            z[i] = random.nextDouble () * 50.0;
        }
        x[0] = 0.0;
        y[1] = 0.0;

        sw.setBadBoundaryAction (2);
        TriMesh tmesh = sw.calcTriMesh
        (
            x,
            y,
            z,
            np,
            null,
            0,
            null,
            1,
            null,
            null
        );

        //sw.writeTextTriMeshFile (tmesh, "contour.tri");

        dl.beginPlot ("trimesh test",
                      0.0, 0.0, 100.0, 100.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("trimesh " + np + " test frame",
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);

        DLSurfaceProperties dlp = new DLSurfaceProperties ();
        dlp.setShowNodes (true);
        dlp.setShowCellEdges (true);
        dlp.setCellEdgeColor (Color.white);
        ColorPalette cpal = new ColorPalette();
        double zmin = tmesh.getZMin ();
        double zmax = tmesh.getZMax ();
        double ztiny = (zmax - zmin) / 100.0;
        dl.setImageColors (cpal, zmin-ztiny, zmax+ztiny);

        dl.addTriMesh ("test trimesh " + np,
                       tmesh,
                       dlp);

    }

};

class Grid10Frame extends JFrame
{

	private static final long serialVersionUID = 1L;

    public Grid10Frame ()
    {
        JSurfaceWorks      sw;
        Random             random;
        double[]           x, y, z;
        int                i;

        random = new Random();
        long  ctw = System.currentTimeMillis ();
        random.setSeed (ctw);
        int np = random.nextInt (1000) + 100;

        setTitle ("Grid Test " + np);
        setSize (900, 700);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        JEasyXGraphicsPanel gpanel = new JEasyXGraphicsPanel ();
        Container contentPane = getContentPane ();
        contentPane.add (gpanel);

        JDisplayList dl = gpanel.getDisplayList ();
        sw = new JSurfaceWorks ();

        x = new double[2000];
        y = new double[2000];
        z = new double[2000];

        for (i=0; i<np; i++) {
            x[i] = random.nextDouble () * 200.0;
            y[i] = random.nextDouble () * 100.0;
            z[i] = random.nextDouble () * 100.0;
        }

        Grid grid = sw.calcGrid
        (
            x,
            y,
            z,
            np,
            null,
            null,
            null,
            null
        );

        dl.beginPlot ("grid test" + np,
                      0.0, 0.0, 200.0, 100.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("grid test frame" + np,
                       0.0,
                       0.0,
                       200.0,
                       100.0,
                       0.0,
                       0.0,
                       200.0,
                       100.0,
                       0.0,
                       0.0,
                       200.0,
                       100.0,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);

        DLSurfaceProperties dlp = new DLSurfaceProperties ();
        dlp.setShowNodes (true);
        ColorPalette cpal = new ColorPalette();
        double zmin = grid.getZMin ();
        double zmax = grid.getZMax ();
        double ztiny = (zmax - zmin) / 100.0;
        dl.setImageColors (cpal, zmin-ztiny, zmax+ztiny);

        dl.addGrid ("test grid 10",
                    grid,
                    dlp);
    }

}

class TFileFrame extends JFrame
{

	private static final long serialVersionUID = 1L;

    //private String                 fileName;
    private JTextField             tField, tField2, tField3;

    private JDisplayList           dl;

    JTextField getField ()
    {
        return tField;
    }

    JTextField getField2 ()
    {
        return tField2;
    }

    JTextField getField3 ()
    {
        return tField3;
    }

    public TFileFrame ()
    {

        setTitle ("Debug TriMesh File");
        setSize (600, 600);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        tField = new JTextField (40);
        tField.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = TFileFrame.this.getField().getText ();
            TFileFrame.this.setFileName (fn);
          }
        });

        JLabel tl = new JLabel ("TriMesh File:", JLabel.RIGHT);
        tl.setLabelFor (tField);

        tField2 = new JTextField (40);
        tField2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = TFileFrame.this.getField2().getText ();
            TFileFrame.this.setLineFileName (fn);
          }
        });

        JLabel tl2 = new JLabel ("Constraint File:", JLabel.RIGHT);
        tl2.setLabelFor (tField2);

        tField3 = new JTextField (40);
        tField3.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = TFileFrame.this.getField3().getText ();
            TFileFrame.this.setEdgeFileName (fn);
          }
        });

        JLabel tl3 = new JLabel ("Edge File:", JLabel.RIGHT);
        tl3.setLabelFor (tField3);

        JPanel lpanel = new JPanel ();
        lpanel.setLayout (new GridLayout (1, 4));
        lpanel.add (tl);
        lpanel.add (tField);
        lpanel.add (tl2);
        lpanel.add (tField2);
        lpanel.add (tl3);
        lpanel.add (tField3);

        Container contentPane = getContentPane ();

        JPanel cpanel = new JPanel ();
        cpanel.setLayout (new BorderLayout ());
        cpanel.add (lpanel, BorderLayout.NORTH);

        JEasyXGraphicsPanel gpanel = new JEasyXGraphicsPanel ();
        cpanel.add (gpanel, BorderLayout.CENTER);

        contentPane.add (cpanel);

        dl = gpanel.getDisplayList ();

        dl.beginPlot ("debug trimesh file",
                      0.0, 0.0, 100.0, 100.0);

    }

    void setLineFileName (String fname)
    {
		fname = fname.trim();
		if (fname.length() < 1) {
			return;
		}

        DBLineList dblist = new DBLineList ();

        try {
            dblist.loadAsciiFile (fname);
        }
        catch (Exception e) {
            System.out.println ("Bad file name for line file");
            System.out.flush ();
            return;
        }

        int nline = dblist.numLines;
        int[] iline = dblist.linePts;
        double[] xnodes = dblist.x;
        double[] ynodes = dblist.y;
        double[] znodes = dblist.z;

        double[] xa;
        double[] ya;
        double[] za;
        int      npts, i, n;

        //vflag = dblist.vflag;

        dl.setFrameClip (1);
        dl.createFrame ("debug trimesh file frame",
                       0.0, 0.0, 100.0, 100.0,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       0.0, 0.0, 100.0, 100.0,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);


        n = 0;
        dl.setLineThickness (.02);
        dl.setLinePattern (0, 1.0);
        dl.setColor (Color.red);
        dl.setTextOffset (.10, -.10);
        for (int ido=0; ido<nline; ido++) {
            npts = iline[ido];

            xa = new double[npts];
            ya = new double[npts];
            za = new double[npts];

            for (i=0; i<npts; i++) {
                xa[i] = xnodes[n];
                ya[i] = ynodes[n];
                za[i] = znodes[n];
                n++;
                dl.addSymbol (xa[i], ya[i], .05, 0.0, 7);
/*
                if (vflag == 0) {
                    dl.addNumber (xa[i], ya[i], .08, 0.0, (double)n, 0, 0);
                }
                else {
                    if (i == 1) {
                        xmid = (xa[0] + xa[1]) / 2.0;
                        ymid = (ya[0] + ya[1]) / 2.0;
                        dl.addNumber (xmid, ymid, .08, 0.0, (double)ido, 0, 0);
                    }
                }
*/
            }

            dl.addLine (xa, ya, npts);
        }

        repaint ();
    }


    void setEdgeFileName (String fname)
    {
        DBEdgeList dblist = new DBEdgeList ();

        try {
            dblist.loadAsciiFile (fname);
        }
        catch (Exception e) {
            System.out.println ("Bad file name for line file");
            System.out.flush ();
            return;
        }


        dl.clear ();

        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("debug trimesh file frame",
                       dblist.getXmin(),
                       dblist.getYmin(),
                       dblist.getXmax(),
                       dblist.getYmax(),
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       dblist.getXmin(),
                       dblist.getYmin(),
                       dblist.getXmax(),
                       dblist.getYmax(),
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);


        int nedge = dblist.getNumEdges();

        double[] x1 = dblist.getX1();
        double[] y1 = dblist.getY1();
        double[] x2 = dblist.getX2();
        double[] y2 = dblist.getY2();

        int[] edgenum = dblist.getEdgeNum();
        int[] edgeflag = dblist.getEdgeFlag();

        int      i;
        double   xmid, ymid;

        double[] xa = new double[10];
        double[] ya = new double[10];

        double vx1 = dblist.getVx1();
        double vy1 = dblist.getVy1();
        double vx2 = dblist.getVx2();
        double vy2 = dblist.getVy2();

        xa[0] = vx1;
        ya[0] = vy1;
        xa[1] = vx2;
        ya[1] = vy2;

        dl.setLineThickness (.001);
        dl.setColor (Color.red);
        dl.addLine (xa, ya, 2);

        dl.setColor (Color.black);
        dl.setTextOffset (.10, .10);

        for (i=0; i<nedge; i++) {
            xa[0] = x1[i];
            ya[0] = y1[i];
            xa[1] = x2[i];
            ya[1] = y2[i];
            xmid = (xa[0] + xa[1]) / 2.0;
            ymid = (ya[0] + ya[1]) / 2.0;
            dl.addNumber (xmid, ymid, .08, 0.0, (double)edgenum[i], 0, 0);
            if (edgeflag[i] == 1) {
                dl.setLinePattern (3, 1.0);
            }
            else {
                dl.setLinePattern (0, 1.0);
            }
            dl.addLine (xa, ya, 2);
        }

        repaint ();
    }

    void setFileName (String fname)
    {
        TriMesh tmesh = new TriMesh ();
        try {
            tmesh.loadAsciiDebugFile (fname);
        }
        catch (Exception e) {
            System.out.println ("Bad file name for trimesh debug file");
            System.out.flush ();
            return;
        }

        double txmin = tmesh.getXMin ();
        double tymin = tmesh.getYMin ();
        double txmax = tmesh.getXMax ();
        double tymax = tmesh.getYMax ();

		System.out.println ("trimesh limits: " + 
				             txmin +  "  " +
							 tymin +  "  " +
							 txmax +  "  " +
							 tymax);
		System.out.flush ();

        double dx = txmax - txmin + tymax - tymin;
        dx /= 40.0;
        txmin -= dx;
        tymin -= dx;
        txmax += dx;
        tymax += dx;

        dl.clear ();

        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("debug trimesh file frame",
                       txmin,
                       tymin,
                       txmax,
                       tymax,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       txmin,
                       tymin,
                       txmax,
                       tymax,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);


        double[] xnodes = tmesh.getNodeXArray();
        double[] ynodes = tmesh.getNodeYArray();
        double[] znodes = tmesh.getNodeZArray();
        int numNodes = tmesh.getNumNodes ();

        int[] en1 = tmesh.getEdgeNode0Array ();
        int[] en2 = tmesh.getEdgeNode1Array ();
        int[] et2 = tmesh.getEdgeTriangle1Array ();
        int numEdges = tmesh.getNumEdges ();

        //int[] te1 = tmesh.getTriangleEdge0Array ();
        //int[] te2 = tmesh.getTriangleEdge1Array ();
        //int[] te3 = tmesh.getTriangleEdge2Array ();

      /*
       * Draw the node locations as labeled x marks.
       */
        int     i;

        dl.setSymbolColor (0, 0, 0);
        dl.setLineThickness (.01);
        for (i=0; i<numNodes; i++) {
            dl.addSymbol (xnodes[i], ynodes[i], .10, 0.0, 8);
            dl.setTextOffset (.10, 0.06);
            dl.setTextColor (Color.black);
            dl.addNumber (xnodes[i], ynodes[i], .10, 0.0, (double)(i), 0, 0);
            dl.setTextOffset (.10, -0.10);
            dl.setTextColor (Color.red);
            dl.addNumber (xnodes[i], ynodes[i], .10, 0.0, znodes[i], 2, 0);
        }

      /*
       * Draw the edge lines.
       */
        double[] xline = new double[2];
        double[] yline = new double[2];
        int    n1, n2;

        double   xt, yt;

        dl.setLineColor (Color.green);
        dl.setLineThickness (.001);
        dl.setTextAnchor (5);
        dl.setTextColor (Color.green);
        dl.setTextOffset (0.0, 0.0);
        for (i=0; i<numEdges; i++) {
            n1 = en1[i];
            n2 = en2[i];
            xline[0] = xnodes[n1];
            yline[0] = ynodes[n1];
            xline[1] = xnodes[n2];
            yline[1] = ynodes[n2];
            if (et2[i] < 0) {
                dl.setLinePattern (3, 1.0);
            }
            else {
                dl.setLinePattern (0, 1.0);
            }
            dl.addLine (xline, yline, 2);
            xt = (xline[0] + xline[1]) / 2.0;
            yt = (yline[0] + yline[1]) / 2.0;
            dl.addNumber (xt, yt, .1, 0.0, (double)i, 0, 0);
        }

      /*
       * Draw the triangle numbers.
        dl.setTextColor (Color.black);
        for (i=0; i<numTriangles; i++) {
            int e1 = te1[i];
            int e2 = te2[i];
            n1 = en1[e1];
            n2 = en2[e1];
            if (en1[e2] != n1  &&  en1[e2] != n2) {
                n3 = en1[e2];
            }
            else {
                n3 = en2[e2];
            }
            double xmid = (xnodes[n1] + xnodes[n2] + xnodes[n3]) / 3.0;
            double ymid = (ynodes[n1] + ynodes[n2] + ynodes[n3]) / 3.0;
            dl.addNumber (xmid, ymid, .1, 0.0, (double)i, 0, 0);
        }
       */

        if (tmesh.xBounds != null  &&
            tmesh.yBounds != null  &&
            tmesh.nBounds > 3) {

            dl.setLineColor (Color.black);
            dl.setLineThickness (.03);
            dl.setLinePattern (3, 1.0);
            dl.addLine (tmesh.xBounds, tmesh.yBounds, tmesh.nBounds);
        }

        repaint ();

    }

};



class FileioFrame extends JFrame
{

	private static final long serialVersionUID = 1L;

    //private String                 fileName;
    private JTextField             tField;

    private JDisplayList           dl;

    JTextField getField ()
    {
        return tField;
    }

    public FileioFrame ()
    {

        setTitle ("Debug TriMesh File");
        setSize (900, 700);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        tField = new JTextField (40);
        tField.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = FileioFrame.this.getField().getText ();
            FileioFrame.this.setFileName (fn);
          }
        });

        JLabel tl = new JLabel ("TriMesh File Name:", JLabel.RIGHT);
        tl.setLabelFor (tField);

        JButton
        b2 = new JButton ("New TriMesh");
        b2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            FileioFrame.this.newTriMesh();
          }
        });

        JButton
        b3 = new JButton ("Read File");
        b3.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            FileioFrame.this.readFile();
          }
        });

        JButton
        b4 = new JButton ("Write Text File");
        b4.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            FileioFrame.this.writeTextFile();
          }
        });

        JButton
        b5 = new JButton ("Write Binary File");
        b5.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            FileioFrame.this.writeBinaryFile();
          }
        });

        JPanel lpanel = new JPanel ();
        lpanel.setLayout (new GridLayout (1, 5));
        lpanel.add (b2);
        lpanel.add (b3);
        lpanel.add (b4);
        lpanel.add (b5);
        lpanel.add (tl);
        lpanel.add (tField);

        Container contentPane = getContentPane ();

        JPanel cpanel = new JPanel ();
        cpanel.setLayout (new BorderLayout ());
        cpanel.add (lpanel, BorderLayout.NORTH);

        JEasyXGraphicsPanel gpanel = new JEasyXGraphicsPanel ();
        cpanel.add (gpanel, BorderLayout.CENTER);

        contentPane.add (cpanel);

        dl = gpanel.getDisplayList ();

        dl.beginPlot ("debug trimesh file",
                      0.0, 0.0, 100.0, 100.0);

    }

    String currentFileName = null;
    TriMesh currentTriMesh = null;

    void setFileName (String fname)
    {
        currentFileName = fname;
    }

    void writeTextFile ()
    {
        currentFileName = tField.getText ();
        if (currentFileName == null  ||  currentTriMesh == null) {
            return;
        }

        JSurfaceWorks jsw = new JSurfaceWorks ();

        jsw.writeTextTriMeshFile (currentTriMesh, currentFileName);

    }

    void writeBinaryFile ()
    {
        currentFileName = tField.getText ();
        if (currentFileName == null  ||  currentTriMesh == null) {
            return;
        }

        JSurfaceWorks jsw = new JSurfaceWorks ();

        jsw.writeBinaryTriMeshFile (currentTriMesh, currentFileName);

    }

    void readFile ()
    {
        currentFileName = tField.getText ();

        if (currentFileName == null) {
            return;
        }

        JSurfaceWorks jsw = new JSurfaceWorks ();

        TriMesh tmesh =
        jsw.readTriMeshFile (currentFileName, 0l);

        dl.clear();

        dl.setFrameClip (1);
        dl.createFrame ("trimesh 10 test frame",
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);

        DLSurfaceProperties dlp = new DLSurfaceProperties ();
        ColorPalette cpal = new ColorPalette();
        double zmin = tmesh.getZMin ();
        double zmax = tmesh.getZMax ();
        double ztiny = (zmax - zmin) / 100.0;
        dl.setImageColors (cpal, zmin-ztiny, zmax+ztiny);

        dl.addTriMesh ("test trimesh 10",
                       tmesh,
                       dlp);

        currentTriMesh = tmesh;

        repaint ();

    }



    void newTriMesh ()
    {
        Random         random;

        random = new Random();

        int ndo = (int)(random.nextDouble() * 100.0);
        if (ndo < 20) ndo = 20;

        double[] x = new double[100];
        double[] y = new double[100];
        double[] z = new double[100];

        for (int i=0; i<ndo; i++) {
            x[i] = random.nextDouble () * 100.0;
            y[i] = random.nextDouble () * 100.0;
            z[i] = random.nextDouble () * 100.0;
        }

        JSurfaceWorks jsw = new JSurfaceWorks ();

        TriMesh tmesh = jsw.calcTriMesh
        (
            x,
            y,
            z,
            ndo,
            null,
            0,
            null,
            1,
            null,
            null
        );

        dl.clear();

        dl.setFrameClip (1);
        dl.createFrame ("trimesh 10 test frame",
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);

        DLSurfaceProperties dlp = new DLSurfaceProperties ();
        ColorPalette cpal = new ColorPalette();
        double zmin = tmesh.getZMin ();
        double zmax = tmesh.getZMax ();
        double ztiny = (zmax - zmin) / 100.0;
        dl.setImageColors (cpal, zmin-ztiny, zmax+ztiny);

        dl.addTriMesh ("test trimesh",
                       tmesh,
                       dlp);

        currentTriMesh = tmesh;

        repaint ();

    }


};



class PFileFrame extends JFrame
{

	private static final long serialVersionUID = 1L;

    //private String                 fileName;
    private JTextField             tField, tField2;

    private JDisplayList           dl;

    JTextField getField ()
    {
        return tField;
    }

    JTextField getField2 ()
    {
        return tField2;
    }

    public PFileFrame ()
    {

        setTitle ("Points and Constraints");
        setSize (900, 700);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        tField = new JTextField (40);
        tField.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = PFileFrame.this.getField().getText ();
            PFileFrame.this.setLineFileName (fn);
          }
        });

        JLabel tl = new JLabel ("Line File Name:", JLabel.RIGHT);
        tl.setLabelFor (tField);

        tField2 = new JTextField (40);
        tField2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = PFileFrame.this.getField2().getText ();
            PFileFrame.this.setPointFileName (fn);
          }
        });

        JLabel tl2 = new JLabel ("Point File Name:", JLabel.RIGHT);
        tl2.setLabelFor (tField2);

        JButton contourButton = new JButton ("contour");
        contourButton.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            PFileFrame.this.contourPoints ();
          }
        });

        JButton clearButton = new JButton ("clear");
        clearButton.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            PFileFrame.this.clearDrawing ();
          }
        });

        JPanel lpanel = new JPanel ();
        lpanel.setLayout (new GridLayout (1, 5));
        lpanel.add (tl);
        lpanel.add (tField);
        lpanel.add (tl2);
        lpanel.add (tField2);
        lpanel.add (clearButton);
        lpanel.add (contourButton);

        Container contentPane = getContentPane ();

        JPanel cpanel = new JPanel ();
        cpanel.setLayout (new BorderLayout ());
        cpanel.add (lpanel, BorderLayout.NORTH);

        JEasyXGraphicsPanel gpanel = new JEasyXGraphicsPanel ();
        cpanel.add (gpanel, BorderLayout.CENTER);

        contentPane.add (cpanel);

        dl = gpanel.getDisplayList ();

        dl.beginPlot ("debug points and constraints",
                      0.0, 0.0, 100.0, 100.0);

		setupFrame ();

    }


	private void setupFrame ()
	{

        dl.clear ();

        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("point and constraint frame",
                       0.0, 0.0, 100.0, 100.0,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       0.0, 0.0, 100.0, 100.0,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);

		repaint ();

    }

    void clearDrawing ()
    {
        NumPointFile = 0;
		nPoints = 0;
		faultList.clear ();
		setupFrame ();
    }

    private int NumPointFile;

    private double[] xPoints;
    private double[] yPoints;
    private double[] zPoints;
    private int      nPoints;

	private ArrayList<XYZPolyline> faultList = 
		new ArrayList<XYZPolyline> (10);

    void setPointFileName (String fname)
    {

		fname = fname.trim();
		if (fname.length() < 1) {
			return;
		}

        DBPointList dblist = new DBPointList ();

        try {
            dblist.loadAsciiFile (fname);
        }
        catch (Exception e) {
            System.out.println ("Bad file name for point file");
            System.out.flush ();
            return;
        }

      /*
       * Draw the node locations as labeled x marks.
       */
        int     i, npts;

        double[] xnodes = dblist.x;
        double[] ynodes = dblist.y;
        double[] znodes = dblist.z;
        npts = dblist.npts;

        xPoints = xnodes;
        yPoints = ynodes;
        zPoints = znodes;
        nPoints = npts;

        dl.setSymbolColor (0, 0, 0);
        dl.setLineThickness (.001);
        dl.setTextOffset (.06, .06);
        if (NumPointFile == 0) {
            dl.setTextColor (Color.black);
        }
        else {
            dl.setTextColor (Color.blue);
        }
        for (i=0; i<npts; i++) {
            dl.addSymbol (xnodes[i], ynodes[i], .05, 0.0, 8);
            if (znodes[i] < 1.e20) {
                dl.addNumber (xnodes[i], ynodes[i], .08, 0.0, znodes[i], 1, 0);
            }
        }

        NumPointFile++;

        repaint ();

    }



    void contourPoints ()
    {
        if (nPoints < 1) {
			System.out.println ("No points defined for contour.");
            return;
        }

        JSurfaceWorks jsw = new JSurfaceWorks ();

		GridGeometry geom = new GridGeometry (50, 50, 0.0, 0.0, 100.0, 100.0);
        CSWGrid grid = jsw.calcGrid (xPoints, yPoints, zPoints, nPoints,
                                     faultList, null, geom, null);

		if (grid == null) {
			System.out.println ("null grid returned from calcGrid");
			return;
		}

        dl.addGrid ("test grid", grid, null);

        repaint ();
    }



    void setLineFileName (String fname)
    {
        DBLineList dblist = new DBLineList ();

        try {
            dblist.loadAsciiFile (fname);
        }
        catch (Exception e) {
            System.out.println ("Bad file name for line file: " + fname);
            System.out.flush ();
            return;
        }

        int nline = dblist.numLines;
        int[] iline = dblist.linePts;
        double[] xnodes = dblist.x;
        double[] ynodes = dblist.y;
        double[] znodes = dblist.z;

        double[] xa;
        double[] ya;
        double[] za;
        int      npts, i, n;

        dl.setTextOffset (.05, .05);

		faultList.clear ();

        n = 0;
        for (int ido=0; ido<nline; ido++) {
            npts = iline[ido];

            xa = new double[npts];
            ya = new double[npts];
            za = new double[npts];

            for (i=0; i<npts; i++) {
                xa[i] = xnodes[n];
                ya[i] = ynodes[n];
                za[i] = znodes[n];
                n++;
                dl.setColor (Color.blue);
                dl.addSymbol (xa[i], ya[i], .05, 0.0, 7);
                dl.setColor (Color.red);
                dl.addNumber (xa[i], ya[i], .08, 0.0, (double)n, 0, 0);
            }

            dl.setColor (Color.black);
            dl.addLine (xa, ya, npts);
			XYZPolyline cline = new XYZPolyline (xa, ya, za);
			faultList.add (cline);
        }

        repaint ();
    }

};

class ContourFrame extends JFrame
{

	private static final long serialVersionUID = 1L;

    private String                 fileName, fileName2, fileName3;
    private JTextField             tField, tField2, tField3;

    private JDisplayList           dl;

    JTextField getField ()
    {
        return tField;
    }

    JTextField getField2 ()
    {
        return tField2;
    }

    JTextField getField3 ()
    {
        return tField3;
    }

    public ContourFrame ()
    {

        setTitle ("Contour TriMesh File");
        setSize (900, 700);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        tField = new JTextField (40);
        tField.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = ContourFrame.this.getField().getText ();
            ContourFrame.this.fileName = fn;
          }
        });

        JLabel tl = new JLabel ("TriMesh File Name:", JLabel.RIGHT);
        tl.setLabelFor (tField);

        tField2 = new JTextField (40);
        tField2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = ContourFrame.this.getField2().getText ();
            ContourFrame.this.fileName2 = fn;
          }
        });

        JLabel tl2 = new JLabel ("XYZ File Name:", JLabel.RIGHT);
        tl2.setLabelFor (tField2);

        tField3 = new JTextField (40);
        tField3.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = ContourFrame.this.getField3().getText ();
            ContourFrame.this.fileName3 = fn;
          }
        });

        JLabel tl3 = new JLabel ("XYZ File Name 2:", JLabel.RIGHT);
        tl3.setLabelFor (tField3);

        JButton
        b5 = new JButton ("Contour");
        b5.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            ContourFrame.this.showContours();
          }
        });

        JPanel lpanel = new JPanel ();
        lpanel.setLayout (new GridLayout (1, 5));
        lpanel.add (tl);
        lpanel.add (tField);
        lpanel.add (tl2);
        lpanel.add (tField2);
        lpanel.add (tl3);
        lpanel.add (tField3);
        lpanel.add (b5);

        Container contentPane = getContentPane ();

        JPanel cpanel = new JPanel ();
        cpanel.setLayout (new BorderLayout ());
        cpanel.add (lpanel, BorderLayout.NORTH);

        JEasyXGraphicsPanel gpanel = new JEasyXGraphicsPanel ();
        cpanel.add (gpanel, BorderLayout.CENTER);

        contentPane.add (cpanel);

        dl = gpanel.getDisplayList ();

        dl.beginPlot ("contour trimesh file",
                      0.0, 0.0, 100.0, 100.0);

    }

    String         currentFileName = null;
    TriMesh        currentTriMesh = null;
    DBPointList    pointList = new DBPointList ();
    DBPointList    pointList2 = new DBPointList ();


    void showContours ()
    {
        TriMesh    tmesh;

        fileName = tField.getText ();
        fileName2 = tField2.getText ();
        fileName3 = tField3.getText ();

        JSurfaceWorks jsw = new JSurfaceWorks ();

        tmesh = null;
        currentFileName = fileName;
        if (fileName.trim().length() > 1) {
            tmesh =
            jsw.readTriMeshFile (currentFileName, 0l);
        }

        if (fileName2.trim().length() > 1) {
            try {
                pointList.loadAsciiFile (fileName2);
            }
            catch (Exception e) {
                System.out.println ("Bad file name for xyz file");
                System.out.flush ();
                pointList.npts = 0;
                pointList.x = null;
                pointList.y = null;
                pointList.z = null;
            }
        }
        else {
            pointList.npts = 0;
            pointList.x = null;
            pointList.y = null;
            pointList.z = null;
        }

        if (fileName3.trim().length() > 1) {
            try {
                pointList2.loadAsciiFile (fileName3);
            }
            catch (Exception e) {
                System.out.println ("Bad file name for xyz file");
                System.out.flush ();
                pointList2.npts = 0;
                pointList2.x = null;
                pointList2.y = null;
                pointList2.z = null;
            }
        }
        else {
            pointList2.npts = 0;
            pointList2.x = null;
            pointList2.y = null;
            pointList2.z = null;
        }

        dl.clear();

        dl.setFrameClip (1);
        dl.createFrame ("trimesh 10 test frame",
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);

        if (tmesh != null) {
            DLSurfaceProperties dlp = new DLSurfaceProperties ();
            dlp.setContourSmoothing (0);
            ColorPalette cpal = new ColorPalette();
            double zmin = tmesh.getZMin ();
            double zmax = tmesh.getZMax ();
            double ztiny = (zmax - zmin) / 100.0;
            dl.setImageColors (cpal, zmin-ztiny, zmax+ztiny);
            dl.addTriMesh ("test trimesh 10",
                           tmesh,
                           dlp);
        }

        int npts = pointList.npts;
        double[] xpts = pointList.x;
        double[] ypts = pointList.y;
        double[] zpts = pointList.z;
        dl.setTextOffset (.05, .05);
        for (int i=0; i<npts; i++) {
            dl.addSymbol (xpts[i], ypts[i], .05, 0.0, 7);
            dl.addNumber (xpts[i], ypts[i], .08, 0.0, zpts[i], 2, 0);
        }

        npts = pointList2.npts;
        xpts = pointList2.x;
        ypts = pointList2.y;
        zpts = pointList2.z;
        dl.setTextOffset (.05, -.1);
        dl.setTextColor (Color.green);
        dl.setSymbolColor (Color.green);
        for (int i=0; i<npts; i++) {
            dl.addSymbol (xpts[i], ypts[i], .05, 0.0, 7);
            dl.addNumber (xpts[i], ypts[i], .08, 0.0, zpts[i], 2, 0);
        }

        currentTriMesh = tmesh;

        repaint ();

    }

};





class DivideFrame extends JFrame
{

	private static final long serialVersionUID = 1L;

   // private String                 fileName;
    private JTextField             tField, tField2, tField3;
    private JButton                flipButton;
    private JButton                flipButton2;

    private JDisplayList           dl;

    private boolean                flipFlag = false;
    private boolean                flipFlag2 = false;

    JTextField getField ()
    {
        return tField;
    }

    JTextField getField2 ()
    {
        return tField2;
    }

    JTextField getField3 ()
    {
        return tField3;
    }

    private boolean frameCreated = false;

    public DivideFrame ()
    {

        setTitle ("Debug Divide File");
        setSize (1000, 850);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        tField = new JTextField (40);
        tField.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = DivideFrame.this.getField().getText ();
            DivideFrame.this.setFileName (fn);
          }
        });

        JLabel tl = new JLabel ("TriMesh File:", JLabel.RIGHT);
        tl.setLabelFor (tField);

        tField2 = new JTextField (40);
        tField2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = DivideFrame.this.getField2().getText ();
            DivideFrame.this.setLineFileName (fn);
          }
        });

        JLabel tl2 = new JLabel ("Line File:", JLabel.RIGHT);
        tl2.setLabelFor (tField2);

        flipButton = new JButton ("Swap yz");
        flipButton.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            DivideFrame.this.flipYZCoords ();
          }
        });

        tField3 = new JTextField (40);
        tField3.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = DivideFrame.this.getField3().getText ();
            DivideFrame.this.setPointFileName (fn);
          }
        });

        JLabel tl3 = new JLabel ("Point File:", JLabel.RIGHT);
        tl3.setLabelFor (tField3);

        flipButton2 = new JButton ("Swap xz");
        flipButton2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            DivideFrame.this.flipXZCoords ();
          }
        });


        JPanel lpanel = new JPanel ();
        lpanel.setLayout (new GridLayout (1, 4));
        lpanel.add (tl);
        lpanel.add (tField);
        lpanel.add (tl2);
        lpanel.add (tField2);
        lpanel.add (tl3);
        lpanel.add (tField3);
        lpanel.add (flipButton);
        lpanel.add (flipButton2);

        Container contentPane = getContentPane ();

        JPanel cpanel = new JPanel ();
        cpanel.setLayout (new BorderLayout ());
        cpanel.add (lpanel, BorderLayout.NORTH);

        JEasyXGraphicsPanel gpanel =
            new JEasyXGraphicsPanel (BorderLayout.WEST);
        cpanel.add (gpanel, BorderLayout.CENTER);

        contentPane.add (cpanel);

        dl = gpanel.getDisplayList ();

        dl.beginPlot ("debug divide file",
                      0.0, 0.0, 100.0, 100.0);

        frameCreated = false;

    }


    void setPointFileName (String fname)
    {
        DBPointList dblist = new DBPointList ();

        try {
            dblist.loadAsciiFile (fname);
        }
        catch (Exception e) {
            System.out.println ("Bad file name for point file");
            System.out.flush ();
            return;
        }

        double[] xnodes = dblist.x;
        double[] ynodes = dblist.y;
        double[] znodes = dblist.z;
        int np = dblist.npts;

        int      i;

        dl.setFrameClip (1);

        if (frameCreated == false) {
            dl.clear ();
//            dl.setFrameClip (1);
            dl.createFrame ("debug trimesh file frame",
                           -200,
                           -200,
                           200,
                           200,
                           10.0,
                           10.0,
                           90.0,
                           90.0,
                           -200,
                           -200,
                           200,
                           200,
                           1,
                           0,
                           0,
                           DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                           null,
                           null,
                           null,
                           DLConst.FRAME_NO_ATTACH,
                           0.0,
                           0.0);
            frameCreated = true;
        }

        dl.setColor (Color.magenta);

        dl.setTextOffset (.10, -0.10);
        for (i=0; i<np; i++) {
            dl.addSymbol (xnodes[i], ynodes[i], 0.07, 0.0, 7);
            dl.addNumber (xnodes[i], ynodes[i], .10, 0.0, znodes[i], 2, 0);
        }

        repaint ();
    }

    void setLineFileName (String fname)
    {
        DBLineList dblist = new DBLineList ();

        try {
            dblist.setYZFlip (flipFlag);
            dblist.setXZFlip (flipFlag2);
            dblist.loadAsciiFile (fname);
        }
        catch (Exception e) {
            System.out.println ("Bad file name for line file");
            System.out.flush ();
            return;
        }

        double[] xnodes = dblist.x;
        double[] ynodes = dblist.y;
        int[] nc = dblist.linePts;
        int np = dblist.numLines;
        double[] xline;
        double[] yline;

        int      i, j;

        dl.setFrameClip (1);

        if (frameCreated == false) {
            dl.clear ();
//            dl.setFrameClip (1);
            dl.createFrame ("debug trimesh file frame",
                           -200,
                           -200,
                           200,
                           200,
                           10.0,
                           10.0,
                           90.0,
                           90.0,
                           -200,
                           -200,
                           200,
                           200,
                           1,
                           0,
                           0,
                           DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                           null,
                           null,
                           null,
                           DLConst.FRAME_NO_ATTACH,
                           0.0,
                           0.0);
            frameCreated = true;
        }

        dl.setColor (Color.magenta);

        int start = 0;
        for (i=0; i<np; i++) {
            xline = new double[nc[i]];
            yline = new double[nc[i]];
            System.arraycopy (xnodes, start, xline, 0, nc[i]);
            System.arraycopy (ynodes, start, yline, 0, nc[i]);
            dl.addLine (xline, yline, nc[i]);
            for (j=0; j<nc[i]; j++) {
                dl.addSymbol (xline[j], yline[j], 0.07, 0.0, i+1);
            }
            start += nc[i];
        }

        repaint ();
    }


    void flipYZCoords ()
    {
        flipFlag = true;
        frameCreated = false;
        String fn = tField.getText ();
        setFileName (fn);
        fn = tField2.getText ();
        setLineFileName (fn);
        flipFlag = false;
        return;
    }

    void flipXZCoords ()
    {
        flipFlag2 = true;
        frameCreated = false;
        String fn = tField.getText ();
        setFileName (fn);
        fn = tField2.getText ();
        setLineFileName (fn);
        flipFlag2 = false;
        return;
    }


    void setFileName (String fname)
    {
        TriMesh tmesh = new TriMesh ();
        try {
            tmesh.setYZFlip (flipFlag);
            tmesh.setXZFlip (flipFlag2);
            tmesh.loadAsciiDebugFile (fname);
            tmesh.setYZFlip (false);
            tmesh.setXZFlip (false);
        }
        catch (Exception e) {
            System.out.println ("Bad file name for trimesh debug file");
            System.out.flush ();
            return;
        }

        double txmin = tmesh.getXMin ();
        double tymin = tmesh.getYMin ();
        double txmax = tmesh.getXMax ();
        double tymax = tmesh.getYMax ();

        double dx = txmax - txmin + tymax - tymin;
        dx /= 40.0;
        txmin -= dx;
        tymin -= dx;
        txmax += dx;
        tymax += dx;

        Random random = new Random();

        dl.setColor (5, 5, 5);
        dl.setFrameClip (1);

        if (frameCreated == false) {
            dl.clear ();

//            dl.setFrameClip (1);
            dl.createFrame ("debug trimesh file frame",
                       txmin,
                       tymin,
                       txmax,
                       tymax,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       txmin,
                       tymin,
                       txmax,
                       tymax,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);
            frameCreated = true;
        }


        double[] xnodes = tmesh.getNodeXArray();
        double[] ynodes = tmesh.getNodeYArray();
        double[] znodes = tmesh.getNodeZArray();
        int numNodes = tmesh.getNumNodes ();

        int[] en1 = tmesh.getEdgeNode0Array ();
        int[] en2 = tmesh.getEdgeNode1Array ();
        int[] et2 = tmesh.getEdgeTriangle1Array ();
        int[] efa = tmesh.getEdgeFlagArray ();
        int numEdges = tmesh.getNumEdges ();

        int[] te1 = tmesh.getTriangleEdge0Array ();
        int[] te2 = tmesh.getTriangleEdge1Array ();
        int numTriangles = tmesh.getNumTriangles ();

      /*
       * Draw the node locations as labeled x marks.
       */
        int     i, ianc;

        dl.setSymbolColor (0, 0, 0);
        dl.setLineThickness (.01);
        dl.setTextColor (Color.black);
        for (i=0; i<numNodes; i++) {
            dl.addSymbol (xnodes[i], ynodes[i], .03, 0.0, 8);
            ianc = i % 3 + 1;
            dl.setTextAnchor (ianc);
            dl.setTextOffset (.10, 0.06 * ianc);
            dl.setTextColor (Color.black);
            dl.addNumber (xnodes[i], ynodes[i], .10, 0.0, (double)(i), 0, 0);
            ianc = i % 3 + 6;
            dl.setTextOffset (.10, -0.10);
            dl.setTextAnchor (ianc);
            dl.setTextColor (Color.blue);
            dl.addNumber (xnodes[i], ynodes[i], .10, 0.0, znodes[i], 2, 0);
        }

      /*
       * Draw the edge lines.
       */
        double[] xline = new double[2];
        double[] yline = new double[2];
        int    n1, n2;

        double   xt, yt, xdiv;

        dl.setLineColor (Color.green);
        dl.setLineThickness (.001);
        dl.setTextAnchor (5);
        dl.setTextColor (Color.green);
        dl.setTextOffset (0.0, 0.0);
        for (i=0; i<numEdges; i++) {
            n1 = en1[i];
            n2 = en2[i];
            if (n1 < 0  ||  n2 < 0) continue;
            xline[0] = xnodes[n1];
            yline[0] = ynodes[n1];
            xline[1] = xnodes[n2];
            yline[1] = ynodes[n2];
            if (et2[i] < 0) {
                dl.setLinePattern (3, 0.5);
                dl.setLineColor (Color.cyan);
            }
            else {
                dl.setLinePattern (0, 1.0);
                dl.setLineColor (Color.green);
            }
            if (efa[i] != 0) {
                dl.setLinePattern (0, 1.0);
                dl.setLineColor (Color.red);
            }
            xdiv = random.nextDouble ();
            xdiv -= .5;
            xdiv += 2.0;
            dl.addLine (xline, yline, 2);
            xt = xline[0] + (xline[1] - xline[0]) / xdiv;
            yt = yline[0] + (yline[1] - yline[0]) / xdiv;
            dl.addNumber (xt, yt, .15, 0.0, (double)i, 0, 0);
        }

        for (i=0; i<numEdges; i++) {
            n1 = en1[i];
            n2 = en2[i];
            xline[0] = xnodes[n1];
            yline[0] = ynodes[n1];
            xline[1] = xnodes[n2];
            yline[1] = ynodes[n2];
            if (et2[i] < 0) {
                dl.setLineThickness (.01);
                dl.setLinePattern (3, 0.5);
                dl.setLineColor (Color.cyan);
                dl.addLine (xline, yline, 2);
            }
        }

      /*
       * Draw the triangle numbers.
        dl.setTextColor (Color.black);
        for (i=0; i<numTriangles; i++) {
            int e1 = te1[i];
            int e2 = te2[i];
            n1 = en1[e1];
            n2 = en2[e1];
            if (en1[e2] != n1  &&  en1[e2] != n2) {
                n3 = en1[e2];
            }
            else {
                n3 = en2[e2];
            }
            double xmid = (xnodes[n1] + xnodes[n2] + xnodes[n3]) / 3.0;
            double ymid = (ynodes[n1] + ynodes[n2] + ynodes[n3]) / 3.0;
            dl.addNumber (xmid, ymid, .1, 0.0, (double)i, 0, 0);
        }
       */

      /*
       * draw a blue * at the center of each triangle.
       */
        dl.setSymbolColor (Color.blue);
        //double     xmid;
        for (i=0; i<numTriangles; i++) {
            int e1 = te1[i];
            int e2 = te2[i];
            n1 = en1[e1];
            n2 = en2[e1];
            if (en1[e2] != n1  &&  en1[e2] != n2) {
                //n3 = en1[e2];
            }
            else {
                //n3 = en2[e2];
            }
            //xcenter = 0.0;
            //ycenter = 0.0;
            //xmid = (xnodes[n1] + xnodes[n2]) / 2.0;
            //ymid = (ynodes[n1] + ynodes[n2]) / 2.0;
            //xcenter += xmid;
            //ycenter += ymid;
            //xmid = (xnodes[n1] + xnodes[n3]) / 2.0;
            //ymid = (ynodes[n1] + ynodes[n3]) / 2.0;
            //xcenter += xmid;
            //ycenter += ymid;
            //xmid = (xnodes[n3] + xnodes[n2]) / 2.0;
            //ymid = (ynodes[n3] + ynodes[n2]) / 2.0;
            //xcenter += xmid;
            //ycenter += ymid;
            //xcenter /= 3.0;
            //ycenter /= 3.0;
            //dl.addSymbol (xcenter, ycenter, 0.05, 0.0, 19);
        }

        if (tmesh.xBounds != null  &&
            tmesh.yBounds != null  &&
            tmesh.nBounds > 3) {

            dl.setLineColor (Color.black);
            dl.setLineThickness (.03);
            dl.setLinePattern (3, 1.0);
            dl.addLine (tmesh.xBounds, tmesh.yBounds, tmesh.nBounds);
        }

        repaint ();

    }

};






class GridFileFrame extends JFrame
{

	private static final long serialVersionUID = 1L;

    //private String                 fileName;
    private JTextField             tField;
    private JTextField             tField2;
    private JCheckBox              checkBox;
    private JCheckBox              checkBox2;

    private JDisplayList           dl;

    JTextField getField ()
    {
        return tField;
    }

    JTextField getField2 ()
    {
        return tField2;
    }

    JCheckBox getCheckBox ()
    {
        return checkBox;
    }

    JCheckBox getCheckBox2 ()
    {
        return checkBox;
    }

    public GridFileFrame ()
    {

        setTitle ("Debug Grid File");
        setSize (600, 600);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        tField = new JTextField (40);
        tField.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            clearAll ();
            String fn = GridFileFrame.this.getField().getText ();
            GridFileFrame.this.setFileName (fn);
            fn = GridFileFrame.this.getField2().getText ();
            GridFileFrame.this.setPointFileName (fn);
          }
        });

        tField2 = new JTextField (40);
        tField2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            clearAll ();
            String fn = GridFileFrame.this.getField().getText ();
            GridFileFrame.this.setFileName (fn);
            fn = GridFileFrame.this.getField2().getText ();
            GridFileFrame.this.setPointFileName (fn);
          }
        });

        JLabel tl = new JLabel ("Grid File:", JLabel.RIGHT);
        tl.setLabelFor (tField);

        JLabel tl2 = new JLabel ("Point File:", JLabel.RIGHT);
        tl.setLabelFor (tField);

        checkBox = new JCheckBox ("Ignore Nulls");
        checkBox2 = new JCheckBox ("Draw Nodes");

        checkBox2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            clearAll ();
            String fn = GridFileFrame.this.getField().getText ();
            GridFileFrame.this.setFileName (fn);
            fn = GridFileFrame.this.getField2().getText ();
            GridFileFrame.this.setPointFileName (fn);
          }
        });

        checkBox.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            clearAll ();
            String fn = GridFileFrame.this.getField().getText ();
            GridFileFrame.this.setFileName (fn);
            fn = GridFileFrame.this.getField2().getText ();
            GridFileFrame.this.setPointFileName (fn);
          }
        });

        JPanel lpanel = new JPanel ();
        lpanel.setLayout (new GridLayout (1, 4));
        lpanel.add (tl);
        lpanel.add (tField);
        lpanel.add (tl2);
        lpanel.add (tField2);
        lpanel.add (checkBox);
        lpanel.add (checkBox2);

        Container contentPane = getContentPane ();

        JPanel cpanel = new JPanel ();
        cpanel.setLayout (new BorderLayout ());
        cpanel.add (lpanel, BorderLayout.NORTH);

        JEasyXGraphicsPanel gpanel =
            new JEasyXGraphicsPanel (BorderLayout.WEST);
        cpanel.add (gpanel, BorderLayout.CENTER);

        contentPane.add (cpanel);

        dl = gpanel.getDisplayList ();

        dl.beginPlot ("debug grid file",
                      0.0, 0.0, 100.0, 100.0);

    }

    void clearAll ()
    {
        dl.clear ();
    }

    void setFileName (String fname)
    {

        if (fname == null) {
            System.out.println ("Bad grid file name.");
            return;
        }

        JSurfaceWorks    jsw;
        jsw = new JSurfaceWorks ();

        boolean ignore = checkBox.isSelected ();

        CSWGrid     grid;
        grid = jsw.readCSWGridFile (fname);

        if (grid == null) {
            System.out.println ("Bad grid file name.");
            return;
        }

        byte[] nmask = null;
        if (ignore == false) {
           grid.setHardNulls ();
           nmask = grid.getNodeMask ();
        }

        double      x1, y1, x2, y2;

        x1 = grid.getXMin ();
        y1 = grid.getYMin ();
        x2 = grid.getXMax ();
        y2 = grid.getYMax ();

        dl.setFrameClip (1);
        dl.createFrame ("grid file frame",
                       x1, y1, x2, y2,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       x1, y1, x2, y2,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);

        dl.addGrid ("test grid", grid, null);

        boolean drawnodes = checkBox2.isSelected ();

        if (drawnodes) {
            int   i, j, k, ncol, nrow;
            double   dx, dy, xt, yt;
            int   iskip;

            ncol = grid.getNCols ();
            nrow = grid.getNRows ();

            iskip = (int)((ncol + nrow) / 50.0);
            if (iskip < 1) iskip = 1;

            dx = (x2 - x1) / (ncol - 1);
            dy = (y2 - y1) / (nrow - 1);

            dl.setLineThickness (.008);
            dl.setTextAnchor (8);
            dl.setTextOffset (0.0, -.06);
            for (i=0; i<nrow; i+=iskip) {
                yt = y1 + i * dy;
                for (j=0; j<ncol; j+=iskip) {
                    k = i * ncol + j;
                    xt = x1 + j * dx;
                    if (grid.getNodeZArray()[k] < 1.e20) {
                        dl.setSymbolColor (Color.blue);
                        dl.setTextColor (Color.blue);
                        dl.addSymbol (xt, yt, .06, 0.0, 7);
                        dl.addNumber (xt, yt, .09, 0.0,
                                      grid.getNodeZArray()[k], 5, 0);
                    }
                    else {
                        dl.setSymbolColor (Color.red);
                        dl.addSymbol (xt, yt, .05, 0.0, 7);
                        dl.addNumber (xt, yt, .07, 0.0, (double)nmask[k], 0, 0);
                    }
                }
            }
        }

        repaint ();

    }


   void setPointFileName (String fname)
   {

       if (fname == null) {
           return;
       }

       DBPointList dblist = new DBPointList ();

       try {
           dblist.loadAsciiFile (fname);
       }
       catch (Exception e) {
           return;
       }

     /*
      * Draw the node locations as labeled x marks.
      */
       int     i, npts;

       double[] xnodes = dblist.x;
       double[] ynodes = dblist.y;
       double[] znodes = dblist.z;
       npts = dblist.npts;

       dl.setSymbolColor (0, 0, 0);
       dl.setLineThickness (.001);
       dl.setTextOffset (.06, .06);
       dl.setTextColor (Color.blue);
       for (i=0; i<npts; i++) {
           dl.addSymbol (xnodes[i], ynodes[i], .05, 0.0, 8);
           dl.addNumber (xnodes[i], ynodes[i], .08, 0.0, znodes[i], 1, 0);
       }

       repaint ();

   }

};

class ResampleFrame extends JFrame
{

	private static final long serialVersionUID = 1L;

    private String                 fileName, fileName2;
    private JTextField             tField, tField2;

    private JDisplayList           dl;

    JTextField getField ()
    {
        return tField;
    }

    JTextField getField2 ()
    {
        return tField2;
    }

    public ResampleFrame ()
    {

        setTitle ("Contour TriMesh File");
        setSize (900, 700);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        tField = new JTextField (40);
        tField.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = ResampleFrame.this.getField().getText ();
            ResampleFrame.this.fileName = fn;
          }
        });

        JLabel tl = new JLabel ("Original Line File Name:", JLabel.RIGHT);
        tl.setLabelFor (tField);

        tField2 = new JTextField (40);
        tField2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            String fn = ResampleFrame.this.getField2().getText ();
            ResampleFrame.this.fileName2 = fn;
          }
        });

        JLabel tl2 = new JLabel ("Average Spacing:", JLabel.RIGHT);
        tl2.setLabelFor (tField2);

        JButton
        b5 = new JButton ("Update");
        b5.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            ResampleFrame.this.updateLines();
          }
        });


        JPanel lpanel = new JPanel ();
        lpanel.setLayout (new GridLayout (1, 5));
        lpanel.add (tl);
        lpanel.add (tField);
        lpanel.add (tl2);
        lpanel.add (tField2);
        lpanel.add (b5);

        Container contentPane = getContentPane ();

        JPanel cpanel = new JPanel ();
        cpanel.setLayout (new BorderLayout ());
        cpanel.add (lpanel, BorderLayout.NORTH);

        JEasyXGraphicsPanel gpanel = new JEasyXGraphicsPanel ();
        cpanel.add (gpanel, BorderLayout.CENTER);

        contentPane.add (cpanel);

        dl = gpanel.getDisplayList ();

        dl.beginPlot ("resample line test",
                      0.0, 0.0, 100.0, 100.0);

    }

    String         currentFileName = null;
    TriMesh        currentTriMesh = null;
    DBPointList    pointList = new DBPointList ();

    void updateLines ()
    {

        fileName = tField.getText ();
        fileName2 = tField2.getText ();
        double    avspace;

        JSurfaceWorks jsw = new JSurfaceWorks ();

        if (fileName.trim().length() > 1) {
            try {
                pointList.loadAsciiFile (fileName);
            }
            catch (Exception e) {
                System.out.println ("Bad file name for line file");
                System.out.flush ();
                return;
            }
        }
        else {
            System.out.println ("Bad file name for line file");
            System.out.flush ();
            return;
        }

        if (fileName2.trim().length() > 1) {
            try {
                avspace =
                Double.parseDouble (fileName2);
            }
            catch (Exception e) {
                System.out.println ("Bad avspace for resampling");
                System.out.flush ();
                return;
            }
        }
        else {
            System.out.println ("Bad avspace for resampling");
            System.out.flush ();
            return;
        }

        dl.clear();

        dl.setFrameClip (1);
        dl.createFrame ("line resample test frame",
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       10.0,
                       10.0,
                       90.0,
                       90.0,
                       0.0,
                       0.0,
                       100.0,
                       100.0,
                       1,
                       0,
                       0,
                       DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                       null,
                       null,
                       null,
                       DLConst.FRAME_NO_ATTACH,
                       0.0,
                       0.0);

        int npts = pointList.npts;
        double[] xpts = pointList.x;
        double[] ypts = pointList.y;
        dl.setColor (Color.black);
        for (int i=0; i<npts; i++) {
            dl.addSymbol (xpts[i], ypts[i], .07, 0.0, 7);
        }
        dl.addLine (xpts, ypts, npts);

        XYPolyline xypline, xyrsamp;
        XYZPolyline xyzpline, xyzrsamp;
        double[]  xa, ya, za;

    /*
     * Create the input xy polyline.
     */
        xypline = new XYPolyline (npts);
        xa = xypline.getXArrayCopy ();
        ya = xypline.getYArrayCopy ();
        System.arraycopy (xpts, 0, xa, 0, npts);
        System.arraycopy (ypts, 0, ya, 0, npts);
        xypline.setXY(xa, ya);

    /*
     * Resample and draw the xy polyline.
     */
        xyrsamp =
        jsw.resampleLine (xypline, avspace);

        xa = xyrsamp.getXArrayCopy ();
        ya = xyrsamp.getYArrayCopy ();

        int rsize = xyrsamp.size();
        dl.setColor (Color.red);
        for (int i=0; i<rsize; i++) {
            ya[i] += 20.0;
            dl.addSymbol (xa[i], ya[i], .05, 0.0, 8);
        }
        dl.addLine (xa, ya, rsize);

    /*
     * Create an input xyz polyline
     */
        xyzpline = new XYZPolyline (npts);
        xa = xyzpline.getXArray ();
        ya = xyzpline.getYArray ();
        za = xyzpline.getZArray ();
        System.arraycopy (xpts, 0, xa, 0, npts);
        System.arraycopy (ypts, 0, ya, 0, npts);
        for (int i=0; i<npts; i++) {
            za[i] = i;
        }

    /*
     * Resample and draw the xyz polyline.
     */
        xyzrsamp =
        jsw.resampleLine (xyzpline, avspace);

        xa = xyzrsamp.getXArray ();
        ya = xyzrsamp.getYArray ();
        za = xyzrsamp.getZArray ();

        rsize = xyzrsamp.size();
        dl.setColor (Color.green);
        dl.setTextOffset (.05, .05);
        dl.setTextAnchor (1);
        dl.setTextThickness (.01);
        for (int i=0; i<rsize; i++) {
            ya[i] += 30.0;
            dl.addSymbol (xa[i], ya[i], .05, 0.0, 8);
            dl.addNumber (xa[i], ya[i], .08, 0.0, za[i], 2, 0);
        }
        dl.addLine (xa, ya, rsize);

    /*
     * Resample and draw the xy polyline on a segment by segment basis.
     */
        xyrsamp =
        jsw.resampleLineSegments (xypline, avspace);

        xa = xyrsamp.getXArrayCopy ();
        ya = xyrsamp.getYArrayCopy ();

        rsize = xyrsamp.size();
        dl.setColor (Color.blue);
        for (int i=0; i<rsize; i++) {
            ya[i] += 50.0;
            dl.addSymbol (xa[i], ya[i], .05, 0.0, 8);
        }
        dl.addLine (xa, ya, rsize);

    /*
     * Resample and draw the xyz polyline on a segment by segment basis.
     */
        xyzrsamp =
        jsw.resampleLineSegments (xyzpline, avspace);

        xa = xyzrsamp.getXArray ();
        ya = xyzrsamp.getYArray ();
        za = xyzrsamp.getZArray ();

        rsize = xyzrsamp.size();
        dl.setColor (Color.magenta);
        dl.setTextOffset (.05, .05);
        dl.setTextAnchor (1);
        dl.setTextThickness (.01);
        for (int i=0; i<rsize; i++) {
            ya[i] += 60.0;
            dl.addSymbol (xa[i], ya[i], .05, 0.0, 8);
            dl.addNumber (xa[i], ya[i], .08, 0.0, za[i], 2, 0);
        }
        dl.addLine (xa, ya, rsize);

        repaint ();

    }

};
