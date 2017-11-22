
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
import java.util.ArrayList;

import javax.swing.JFrame;
import javax.swing.WindowConstants;

import csw.jsurfaceworks.src.*;

import csw.jutils.src.XYZPolyline;

import csw.jeasyx.src.DLConst;
import csw.jeasyx.src.JDisplayList;
import csw.jeasyx.src.JEasyXGraphicsPanel;


/*
 ******************************************************************************

                       2d map for split test

 ******************************************************************************
*/

class SplitTest2D extends JFrame
{

	private static final long      serialVersionUID = 1L;

    private String                 fileName;
    private JDisplayList           dl;

    private double[]               xBorder,
                                   yBorder;
    private int                    nBorder;

    public SplitTest2D ()
    {

        setTitle ("Debug Surface Splitting");
        setSize (900, 700);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        Container contentPane = getContentPane ();

        JEasyXGraphicsPanel gpanel = new JEasyXGraphicsPanel ();
        contentPane.add (gpanel, BorderLayout.CENTER);

        dl = gpanel.getDisplayList ();

        initPlot();

    }

    void initPlot ()
    {

        dl.beginPlot ("debug surface splitting",
                      0.0, 0.0, 150.0, 100.0);

        dl.setFrameClip (1);

        dl.createFrame ("debug surface test frame",
                       0.0,
                       0.0,
                       150.0,
                       100.0,
                       10.0,
                       10.0,
                       140.0,
                       90.0,
                       0.0,
                       0.0,
                       150.0,
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

    }

    void updateBorderData (double[] xb,
                           double[] yb,
                           int npts)
    {
        dl.setLineColor (Color.green);
        dl.addLine (xb, yb, npts);
    }

    void drawTriMesh (TriMesh tmesh, String name)
    {
        dl.addTriMesh (name, tmesh, null);
    }


    void updateClineData (ArrayList<XYZPolyline> lines)
    {

        int           i, lsize;
        XYZPolyline   dline;
        double[]      xp, yp;
        int           npts;

        lsize = lines.size();
        if (lsize < 1) {
            return;
        }

        dl.setLineColor (Color.blue);

        for (i=0; i<lsize; i++) {
            dline = lines.get(i);
            if (dline == null) {
                continue;
            }

            xp = dline.getXArray ();
            yp = dline.getYArray ();
            npts = dline.size ();

            dl.addLine (xp, yp, npts);
        }

    }

    void clearData ()
    {
        dl.clear ();
    }

/*-----------------------------------------------------------------------------------*/

    void updatePoints (double[] x,
                       double[] y,
                       double[] z,
                       int      npts)
    {

        dl.setSelectable (null);
        dl.setSymbolColor (Color.blue);
        dl.setTextColor (new Color (0, 0, 200));
        dl.setTextAnchor (8);
        dl.setTextOffset (0.0, -.07);

        for (int i=0; i<npts; i++) {
            dl.addSymbol (x[i],
                          y[i],
                          0.07,
                          0.0,
                          7);
            dl.addNumber (x[i],
                          y[i],
                          0.07,
                          0.0,
                          z[i],
                          1,
                          0);
        }

    }


    void drawExtendedCenterlines (ArrayList<XYZPolyline> lines)
    {

        int           i, lsize;
        XYZPolyline   dline;
        double[]      xp, yp;
        int           npts;

        lsize = lines.size();
        if (lsize < 1) {
            return;
        }

        dl.setLineColor (Color.gray);
        dl.setLinePattern (3, 1.0);
        dl.setLineThickness (.02);

        for (i=0; i<lsize; i++) {
            dline = lines.get(i);
            if (dline == null) {
                continue;
            }

            xp = dline.getXArray ();
            yp = dline.getYArray ();
            npts = dline.size ();

            dl.addLine (xp, yp, npts);
        }

    }

    void drawWorkPolys (ArrayList<XYZPolyline> lines)
    {

        int           i, lsize;
        XYZPolyline   dline;
        double[]      xp, yp;
        int           npts;

        lsize = lines.size();
        if (lsize < 1) {
            return;
        }

        dl.setLineColor (Color.green);
        dl.setLinePattern (0, 1.0);
        dl.setLineThickness (.001);

        for (i=0; i<lsize; i++) {
            dline = lines.get(i);
            if (dline == null) {
                continue;
            }

            xp = dline.getXArray ();
            yp = dline.getYArray ();
            npts = dline.size ();

            dl.addLine (xp, yp, npts);
        }

    }


};
