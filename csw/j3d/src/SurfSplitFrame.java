
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jsurfaceworks.src;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.WindowConstants;

import csw.jutils.src.XYZPolyline;
import csw.jutils.src.XYZPolylineList;

import csw.jeasyx.src.DLConst;
import csw.jeasyx.src.DLEditAdapter;
import csw.jeasyx.src.DLLine;
import csw.jeasyx.src.DLSelectable;
import csw.jeasyx.src.JDisplayList;
import csw.jeasyx.src.SingleGraphicsPanel;


/*
 ******************************************************************************

                       test the surface splitting

 ******************************************************************************
*/

class SurfSplitFrame extends JFrame
{
    private String                 fileName;
    private JDisplayList           dl;

    private double[]               xBorder,
                                   yBorder;
    private int                    nBorder;

    private ArrayList              clineList = new ArrayList (20);

    private static class CLine {
        double[] xp;
        double[] yp;
        int      np;
    }

    private class BorderListener extends DLEditAdapter {
        public boolean editFinished (DLSelectable dls)
        {
            SurfSplitFrame.this.enableButtons (true);
            SurfSplitFrame.this.updateBorderData (dls);
            return true;
        }
        public boolean editChanged (DLSelectable dls)
        {
            return false;
        }
        public boolean editMoved (DLSelectable dls)
        {
            return false;
        }
    }

    private class ClineListener extends DLEditAdapter {
        public boolean editFinished (DLSelectable dls)
        {
            SurfSplitFrame.this.enableButtons (true);
            SurfSplitFrame.this.updateClineData (dls);
            return true;
        }
        public boolean editChanged (DLSelectable dls)
        {
            return false;
        }
        public boolean editMoved (DLSelectable dls)
        {
            return false;
        }
    }

    JButton          b1, b2, b3, b4, b5;

    private void enableButtons (boolean bval)
    {
        b1.setEnabled (bval);
        b2.setEnabled (bval);
        b3.setEnabled (bval);
        b4.setEnabled (bval);
        b5.setEnabled (bval);
    }

    public SurfSplitFrame ()
    {

        setTitle ("Debug Surface Splitting");
        setSize (900, 700);
        setResizable (true);
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        Container contentPane = getContentPane ();

        b1 = new JButton ("Pick border");
        b1.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            SurfSplitFrame.this.pickBorder ();
          }
        });

        b2 = new JButton ("Pick centerline");
        b2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            SurfSplitFrame.this.pickCenterline ();
          }
        });

        b3 = new JButton ("Clear");
        b3.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            SurfSplitFrame.this.clearData ();
          }
        });

        b4 = new JButton ("Show Polygons");
        b4.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            SurfSplitFrame.this.calcPolygonsFromClines ();
          }
        });

        b5 = new JButton ("Points File...");
        b5.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            SurfSplitFrame.this.pointsFromFile ();
          }
        });

        JPanel lpanel = new JPanel ();
        lpanel.setLayout (new GridLayout (1, 5));
        lpanel.add (b5);
        lpanel.add (b1);
        lpanel.add (b2);
        lpanel.add (b3);
        lpanel.add (b4);

        JPanel cpanel = new JPanel ();
        cpanel.setLayout (new BorderLayout ());
        cpanel.add (lpanel, BorderLayout.NORTH);

        SingleGraphicsPanel gpanel = new SingleGraphicsPanel ();
        cpanel.add (gpanel, BorderLayout.CENTER);

        contentPane.add (cpanel);

        dl = gpanel.getDisplayList ();

        initPlot();

    }

    DBPointList       pointList;
    TriMesh           tmesh = null;

    private void pointsFromFile ()
    {
        String answer;

        answer = JOptionPane.showInputDialog (this, "Enter points filename");
        if (answer == null) {
            return;
        }
        if (answer.length() < 1) {
            return;
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
        }

        JSurfaceWorks jsw = new JSurfaceWorks ();
        tmesh = jsw.calcTriMesh (
            pointList.x,
            pointList.y,
            pointList.z,
            pointList.npts,
            null,
            0,
            null,
            0,
            null,
            null);

        updatePoints ();

        return;
    }


    private void initPlot ()
    {

        dl.beginPlot ("debug surface splitting",
                      0.0, 0.0, 100.0, 100.0);

        dl.setFrameClip (1);

        dl.createFrame ("debug surface test frame",
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

        repaint ();

    }

    private void pickBorder ()
    {
        DLSelectable dls = new DLSelectable ();
        dl.setSelectable (dls);

        BorderListener el = new BorderListener ();

        dl.pickNewPolygonGeometry (el, null);

        enableButtons (false);
    }


    private void pickCenterline ()
    {
        DLSelectable dls = new DLSelectable ();
        dl.setSelectable (dls);

        ClineListener el = new ClineListener ();

        dl.pickNewLineGeometry (el, null);

        enableButtons (false);
    }


    private void updateBorderData (DLSelectable dls)
    {
        if (dls == null) {
            return;
        }

        ArrayList lines = dls.getLineList ();
        if (lines == null) {
            return;
        }

        int   i, lsize;
        DLLine   dline;

        lsize = lines.size();
        if (lsize < 1) {
            return;
        }

        dl.eraseSelectable (dls);
        dl.setSelectable (dls);
        dl.setLineColor (Color.green);
        for (i=0; i<lsize; i++) {
            dline = (DLLine)lines.get(i);
            if (dline == null) {
                continue;
            }
            xBorder = dline.getXPoints();
            yBorder = dline.getYPoints();
            nBorder = dline.getNumPoints();
            dl.addLine (xBorder, yBorder, nBorder);
            break;
        }

        dl.clearDirectShapes ();

        repaint ();

    }


    private void updateClineData (DLSelectable dls)
    {

        if (dls == null) {
            return;
        }

        ArrayList lines = dls.getLineList ();
        if (lines == null) {
            return;
        }

        int   i, lsize;
        DLLine   dline;

        lsize = lines.size();
        if (lsize < 1) {
            return;
        }

        dl.eraseSelectable (dls);
        dl.setSelectable (dls);
        dl.setLineColor (Color.blue);

        CLine          cline;

        for (i=0; i<lsize; i++) {
            dline = (DLLine)lines.get(i);
            if (dline == null) {
                continue;
            }
            cline = new CLine ();
            cline.xp = dline.getXPoints();
            cline.yp = dline.getYPoints();
            cline.np = dline.getNumPoints();

            clineList.add (cline);

            dl.addLine (cline.xp, cline.yp, cline.np);
        }

        dl.clearDirectShapes ();
        repaint ();

    }

    private void clearData ()
    {
        dl.clear ();
        xBorder = null;
        yBorder = null;
        nBorder = 0;
        clineList.clear ();
        initPlot ();
    }

/*----------------------------------------------------------------------------*/

    ArrayList polyTmeshList = null;

    private void calcPolygonsFromClines ()
    {
        CLine        cline;

        JSurfaceWorks jsw = new JSurfaceWorks ();

        jsw.psSetDebugState (1);

        if (pointList != null) {
            jsw.psSetPoints (
                pointList.x,
                pointList.y,
                pointList.z,
                pointList.npts
            );
        }

        jsw.psAddBorderSegment (
            xBorder,
            yBorder,
            null,
            nBorder,
            0,
            0
        );

        int lsize = clineList.size ();
        for (int i=0; i<lsize; i++) {
            cline = (CLine)clineList.get (i);
            if (cline == null) {
                continue;
            }
            jsw.psAddFaultLine (
                cline.xp,
                cline.yp,
                null,
                cline.np,
                i,
                0
            );
        }

        jsw.psCalcSplit (true, 0, 0.0);

        polyTmeshList = jsw.getPolyTmeshList ();

        XYZPolylineList list;

        if (polyTmeshList == null) {
            list = jsw.getCorrectedCenterlineList ();
            dl.setLineColor (Color.blue);
            dl.setLinePattern (0, 1.0);
            dl.setLineThickness (.02);
            drawResultList (list);
            list = jsw.getExtendedCenterlineList ();
            dl.setLineColor (Color.black);
            dl.setSymbolColor (Color.red);
            dl.setLinePattern (3, 1.0);
            //drawResultList (list);
        }

        list = jsw.getWorkPolyList ();
        dl.setLineColor (Color.orange);
        dl.setLinePattern (1, 1.0);
        //drawWorkPolyList (list);

        dl.setLinePattern (0, 1.0);
        //drawFillLabels (list);

        ArrayList alist = jsw.getPolyLabelList ();
        //drawPolyLabels (alist);

        drawWorkPolySurfaces ();

        alist = jsw.getProtoPatchContactLineList ();
        drawContactLines (alist);

        repaint ();

    }

    private void drawPolyLabels (ArrayList list)
    {
        int           i, n;
        DBPoint       p;

        if (list == null) return;

        n = list.size();
        if (n < 1) return;

        dl.setTextAnchor (5);
        dl.setTextColor (Color.orange);
        dl.setTextThickness (0.015);
        for (i=0; i<n; i++) {
            p = (DBPoint)list.get (i);
            dl.addNumber (p.x, p.y, .15, 0.0, (double)p.ival1, 0, 0);
        }

        dl.setTextAnchor (1);
        dl.setTextColor (null);

    }


    private void drawResultList (XYZPolylineList list)
    {
        XYZPolyline      line;
        double[]       xp, yp;
        int            lsize, i, j, np;
        double         xmid, ymid;

        if (list == null) {
            return;
        }

        lsize = list.size();
        if (lsize < 1) {
            return;
        }

        dl.setTextAnchor (5);
        dl.setTextThickness (.01);
        dl.setTextColor (Color.black);
        for (i=0; i<lsize; i++) {
            line = (XYZPolyline)list.get (i);
            if (line == null) {
                continue;
            }
            xp = line.getXArray ();
            yp = line.getYArray ();
            np = line.size ();
            dl.addLine (xp, yp, np);
            for (j=0; j<np-1; j++) {
                xmid = (xp[j] + xp[j+1]) / 2.0;
                ymid = (yp[j] + yp[j+1]) / 2.0;
                dl.addNumber (xmid, ymid, 0.1, 0.0, (double)i, 0, 0);
            }
        }

        return;

    }

    private void drawContactLines (ArrayList alist)
    {
        DBContactLine dbl;
        double[]     xp, yp, zp;
        int          np;

        int size = alist.size ();
        dl.setLineColor (Color.red);
        dl.setLinePattern (3, 1.0);
        dl.setTextThickness (.01);
        dl.setTextColor (Color.red);
        for (int i=0; i<size; i++) {
            dbl = (DBContactLine)alist.get (i);
            if (dbl == null) {
                continue;
            }
            xp = dbl.getXArray ();
            yp = dbl.getYArray ();
            np = dbl.npts;
            dl.addLine (xp, yp, np);
            dl.addNumber (xp[0], yp[0], 0.1, 0.0, (double)(dbl.patchid1), 0, 0);
        }

    }


    private void drawWorkPolyList (XYZPolylineList list)
    {
        XYZPolyline      line;
        double[]       xp, yp;
        int            lsize, i, np;

        if (list == null) {
            return;
        }

        lsize = list.size();
        if (lsize < 1) {
            return;
        }

        for (i=0; i<lsize; i++) {
            line = (XYZPolyline)list.get (i);
            if (line == null) {
                continue;
            }
            xp = line.getXArray ();
            yp = line.getYArray ();
            np = line.size ();
            dl.addLine (xp, yp, np);
        }

        return;

    }

    private void drawFillLabels (XYZPolylineList list)
    {
        XYZPolyline      line;
        double[]       xp, yp;
        int            lsize, i, j, np;
        double         xt, yt, x1, y1, x2, y2,
                       wgt, dx, dy, dist;

        if (list == null) {
            return;
        }

        lsize = list.size();
        if (lsize < 1) {
            return;
        }

        dl.setTextAnchor (5);
        dl.setTextThickness (.02);
        dl.setTextColor (Color.orange);
        for (i=0; i<lsize; i++) {
            line = (XYZPolyline)list.get (i);
            if (line == null) {
                continue;
            }
            xp = line.getXArray ();
            yp = line.getYArray ();
            np = line.size ();
            if (np > 3) {
                xt = 0.0;
                yt = 0.0;
                wgt = 0.0;
                for (j=0; j<np - 1; j++) {
                    x1 = xp[j];
                    y1 = yp[j];
                    x2 = xp[j+1];
                    y2 = yp[j+1];
                    dx = x2 - x1;
                    dy = y2 - y1;
                    dist = dx * dx + dy * dy;
                    dist = Math.sqrt (dist);
                    if (dist <= .001) dist = .001;
                    wgt += dist;
                    xt += (x1 + x2) / 2.0 * dist;
                    yt += (y1 + y2) / 2.0 * dist;
                }
                xt /= wgt;
                yt /= wgt;
                dl.addNumber (xt, yt, .3, 0.0, (double)i, 0, 0);
            }
        }
        dl.setTextAnchor (1);
        dl.setTextThickness (.001);
        dl.setTextColor (null);

        return;

    }

    private void updatePoints ()
    {

        if (pointList == null) {
            return;
        }

        clearData ();

        dl.setSelectable (null);
        dl.setSymbolColor (Color.gray);
        dl.setTextColor (new Color (50, 50, 50));
        dl.setTextAnchor (8);
        dl.setTextOffset (0.0, -.07);

        for (int i=0; i<pointList.npts; i++) {
            dl.addSymbol (pointList.x[i],
                          pointList.y[i],
                          0.07,
                          0.0,
                          7);
            dl.addNumber (pointList.x[i],
                          pointList.y[i],
                          0.07,
                          0.0,
                          pointList.z[i],
                          0,
                          0);
        }

        dl.clearDirectShapes ();
        repaint ();

    }


    private void drawWorkPolySurfaces ()
    {
        if (polyTmeshList == null) {
            return;
        }

        TriMesh tmesh;

        int size = polyTmeshList.size ();

        for (int i=0; i<size; i++) {

            tmesh = (TriMesh)polyTmeshList.get (i);
            if (tmesh == null) {
                continue;
            }

            String str = "workpolytrimesh" + i;
            dl.addTriMesh (str, tmesh, null);

        }

        return;

    }

};
