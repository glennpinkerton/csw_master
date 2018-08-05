
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jtest;

import java.lang.Exception;
import java.lang.Runtime;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridLayout;
import java.awt.Toolkit;
import java.awt.color.ColorSpace;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.font.FontRenderContext;
import java.awt.geom.AffineTransform;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.awt.image.ComponentColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.util.Random;
import java.util.ArrayList;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;
import javax.swing.SwingWorker;
import javax.swing.UIManager;
import javax.swing.WindowConstants;

import org.apache.logging.log4j.Logger;

import csw.jeasyx.src.*;
import csw.jutils.src.ColorPalette;
import csw.jsurfaceworks.src.Grid;
import csw.jutils.src.CSWLogger;



/**
 *
 *  A class to digitize polygon sets for boolean operations
 *  and to display the results.``jjjjjjjjjjjjjjjjjjj
 *
 *  @author Glenn Pinkerton
 *
*/
public class JPolyBool {

    private static Logger  logger = CSWLogger.getMyLogger ();
    
    static {
        String spwd = System.getProperty ("user.dir");
        try {
            System.loadLibrary ("_csw_all");
        }
        catch (UnsatisfiedLinkError use) {
          try {
            String slib = spwd + "/src/dist/lib/lib_csw_all.so";
            System.load (slib);
          }
          catch (UnsatisfiedLinkError use2) {
            System.out.println ();
            System.out.println ("Unsatisfied link error from system load");
            System.out.println ();
            System.out.flush ();
            System.exit(1234);
          }
        }
        catch (Exception e) {
            System.out.println ();
            System.out.println ("Some other error in loading shared lib");
            System.out.println ();
            System.out.flush ();
            System.exit(1234);
        }
    }

    public static void main (String[] args) {

        Runnable run = new Runnable ()
        {
            public void run ()
            {
                JPolyBool.runMainMethod ();
            }
        };

        SwingUtilities.invokeLater (run);
    }


    public static void runMainMethod ()
    {

        logger.info ("    Running main method in JPolyBool\n\n");

        try {
          UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        } catch(Exception e) {
          logger.error ("error setting native Look and Feel");
        }

        String  cpar = System.getenv ("CSW_PARENT");
        if (cpar == null) {
            cpar = "/home/git_glenn/csw_master";
        }
        else {
            if (cpar.isEmpty ()) {
                cpar = "/home/git_glenn/csw_master";
            }
        }

        DigitizeFrame frame = new DigitizeFrame ();
        frame.setDefaultCloseOperation (JFrame.EXIT_ON_CLOSE);

        frame.addWindowListener(new WindowAdapter()
        {
            @Override
            public void windowClosing(WindowEvent e)
            {
                System.out.println ();
                System.out.println("Main Window Closed");
                System.out.flush ();
                e.getWindow().dispose();
                JDLFrameList.removeAllFrames ();
            }
        });


        frame.setVisible (true);

    }



    static void showMem (String  msg)
    {

        long  maxmem = Runtime.getRuntime().maxMemory ();
        long  totmem = Runtime.getRuntime().totalMemory ();
        long  freemem = Runtime.getRuntime().freeMemory ();

        maxmem /= 1000000;
        totmem /= 1000000;
        freemem /= 1000000;

        System.out.println ();
        System.out.println ("maxmem = " + maxmem +
                            "   total mem = " + totmem +
                            "   free mem = " + freemem);
        if (msg != null) {
            System.out.println (msg);
        }
        System.out.println ();
        System.out.flush ();

    }

}



class DigitizeFrame extends JDLFrame implements DLEditListener
{
    private static final long serialVersionUID = 1L;

    private static final int  DIG_TYPE_UNKNOWN = 1;
    private static final int  DIG_TYPE_SOURCE = 1;
    private static final int  DIG_TYPE_CLIP = 2;

    private static Logger  logger = CSWLogger.getMyLogger ();

    private int            dig_type = DIG_TYPE_UNKNOWN;
    private JDisplayList   dl = null;
    private JDisplayListPanel  dlpanel = null;

    private JButton        sourceButton = null;
    private JButton        clipButton = null;
    private JButton        intersectButton = null;
    private JButton        unionButton = null;
    private JButton        xorButton = null;
    private JButton        fragmentButton = null;
    private JButton        clearButton = null;
    private JButton        clearResultButton = null;

    private ArrayList<DLSelectable>   sourceSel = new ArrayList<DLSelectable> ();
    private ArrayList<DLSelectable>   clipSel = new ArrayList<DLSelectable> ();
    private DLSelectable   resultSel = null;

    private ArrayList<DLFill>  sourcePolyList = new ArrayList<DLFill> ();
    private ArrayList<DLFill>  clipPolyList = new ArrayList<DLFill> ();

    public DigitizeFrame ()
    {
        super (FRAME_WITH_TOOLBAR, true);

        dl = super.getDL ();
        dlpanel = dl.getPanel ();

/*
 * Possible wait spot to allow a gdb attach to the JVM process.
 * This can sometimes work for debug of native C/C++ code.
try {
int idum = System.in.read();
}
catch (Exception e) {
}
 */


        sourceButton =
        addAdditionalTextButton (
             "Source",
             "Digitize Source Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.startSource ();
               }
             },
             null
        );

        clipButton =
        addAdditionalTextButton (
             "Clip",
             "Digitize Clip Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.startClip ();
               }
             },
             null
        );

        intersectButton =
        addAdditionalTextButton (
             "Intersect",
             "Intersect the Red Source and Blue Clip Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.intersectPolys ();
               }
             },
             null
        );
        intersectButton.setEnabled (false);

        unionButton =
        addAdditionalTextButton (
             "Union",
             "Union the Red Source and Blue Clip Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.unionPolys ();
               }
             },
             null
        );
        unionButton.setEnabled (false);

        xorButton =
        addAdditionalTextButton (
             "Xor",
             "Xor the Red Source and Blue Clip Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.xorPolys ();
               }
             },
             null
        );
        xorButton.setEnabled (false);

        fragmentButton =
        addAdditionalTextButton (
             "Fragment",
             "Calculate Fragments the Red Source and Blue Clip Polygon Combination",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.fragmentPolys ();
               }
             },
             null
        );
        fragmentButton.setEnabled (false);

        clearButton =
        addAdditionalTextButton (
             "Clear",
             "Clear (Erase) all polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.clearPolys ();
               }
             },
             null
        );

        clearResultButton =
        addAdditionalTextButton (
             "Clear Result",
             "Clear (Erase) all result polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.clearResultPolys ();
               }
             },
             null
        );
        clearResultButton.setEnabled (false);

        setTitle ("Polygon Boolean Testing");

        dl.setBackgroundColor (225, 225, 225);
        dl.beginPlot ("digitize poly boolean",
                      0.0, 0.0, 200.0, 200.0);
        dl.setColor (5, 5, 5);

        dl.createFrame ("source poly frame",
                        0.0, 0.0, 100.0, 100.0);

    }


    private void startSource () {
        dig_type = DIG_TYPE_SOURCE;
        if (clipButton != null) {
          clipButton.setEnabled (false);
        }
        dl.pickNewPolygonGeometry (this, null);
    }

    private void startClip () {
        dig_type = DIG_TYPE_CLIP;
        if (sourceButton != null) {
          sourceButton.setEnabled (false);
        }
        dl.pickNewPolygonGeometry (this, null);
    }


    private void repaintPanel () {
        dl.clearDirectShapes ();
        dlpanel.repaint ();
    }



    private void intersectPolys () {

        ArrayList<DLFill> dlf_out = new ArrayList<DLFill> ();
        dl.polyIntersect (sourcePolyList,
                          clipPolyList,
                          dlf_out);

        dl.eraseSelectable (resultSel);

        resultSel = new DLSelectable ();
        dl.setSelectable (resultSel);

        dl.setFillColor (100, 100, 100);
        dl.setBorderColor (0, 0, 0);
        dl.setLineThickness(.02);
        int  n = 0;
        for (DLFill dlf : dlf_out) {
            if (dlf == null) continue;
            int ncomp = dlf.getNumComponents ();
            if (ncomp < 1) continue;
            int[] npts_arr = dlf.getNumPoints ();
            double[] xp = dlf.getXPoints();
            double[] yp = dlf.getYPoints();
            dl.addFill (xp, yp, npts_arr, ncomp, 1);
        }

        dl.setSelectable (null);

        repaintPanel ();

        clearResultButton.setEnabled (true);

    }


    private void unionPolys () {
        ArrayList<DLFill> dlf_out = new ArrayList<DLFill> ();
        dl.polyUnion (sourcePolyList,
                      clipPolyList,
                      dlf_out);

/*
        for (DLSelectable dlsel : sourceSel) {
            dl.eraseSelectable (dlsel);
        }
        for (DLSelectable dlsel : clipSel) {
            dl.eraseSelectable (dlsel);
        }
*/
        dl.eraseSelectable (resultSel);

        resultSel = new DLSelectable ();
        dl.setSelectable (resultSel);

        dl.setFillColor (200, 100, 100, 100);
        dl.setBorderColor (200, 0, 0);
        dl.setLineThickness(.02);
        int  n = 0;
        for (DLFill dlf : dlf_out) {
            if (dlf == null) continue;
            int ncomp = dlf.getNumComponents ();
            if (ncomp < 1) continue;
            int[] npts_arr = dlf.getNumPoints ();
            double[] xp = dlf.getXPoints();
            double[] yp = dlf.getYPoints();
            dl.addFill (xp, yp, npts_arr, ncomp, 1);
        }

        dl.setSelectable (null);

        repaintPanel ();

        clearResultButton.setEnabled (true);

    }


    private void xorPolys () {
        ArrayList<DLFill> dlf_out = new ArrayList<DLFill> ();
        dl.polyXor (sourcePolyList,
                    clipPolyList,
                    dlf_out);

        dl.eraseSelectable (resultSel);

        resultSel = new DLSelectable ();
        dl.setSelectable (resultSel);

        dl.setFillColor (100, 100, 200, 100);
        dl.setBorderColor (0, 0, 200);
        dl.setLineThickness(.02);
        int  n = 0;
        for (DLFill dlf : dlf_out) {
            if (dlf == null) continue;
            int ncomp = dlf.getNumComponents ();
            if (ncomp < 1) continue;
            int[] npts_arr = dlf.getNumPoints ();
            double[] xp = dlf.getXPoints();
            double[] yp = dlf.getYPoints();
            dl.addFill (xp, yp, npts_arr, ncomp, 1);
        }

        dl.setSelectable (null);

        repaintPanel ();

        clearResultButton.setEnabled (true);

    }


    private void fragmentPolys () {
        ArrayList<DLFill> dlf_out = new ArrayList<DLFill> ();
        dl.polyFragment (sourcePolyList,
                         clipPolyList,
                         dlf_out);

        dl.eraseSelectable (resultSel);

        resultSel = new DLSelectable ();
        dl.setSelectable (resultSel);

        dl.setLineThickness(.02);
        int  n = 1;
        for (DLFill dlf : dlf_out) {
            if (dlf == null) continue;
            int ncomp = dlf.getNumComponents ();
            if (ncomp < 1) continue;
            int[] npts_arr = dlf.getNumPoints ();
            double[] xp = dlf.getXPoints();
            double[] yp = dlf.getYPoints();
            dl.setFillColor ((n * 20) % 250,  (n * 40) % 250, (n * 60) % 250, 100);
            dl.setBorderColor ((n * 10) % 250,  (n * 20) % 250, (n * 30) % 250);
            n++;
            dl.addFill (xp, yp, npts_arr, ncomp, 1);
        }

        dl.setSelectable (null);

        repaintPanel ();

        clearResultButton.setEnabled (true);

    }


    private void clearPolys () {
        for (DLSelectable dlsel : sourceSel) {
            dl.eraseSelectable (dlsel);
        }
        for (DLSelectable dlsel : clipSel) {
            dl.eraseSelectable (dlsel);
        }
        dl.eraseSelectable (resultSel); 
        repaintPanel ();
        sourceSel.clear();
        clipSel.clear();
        resultSel = null;
        sourcePolyList.clear();
        clipPolyList.clear();
        intersectButton.setEnabled (false);
        unionButton.setEnabled (false);
        xorButton.setEnabled (false);
        fragmentButton.setEnabled (false);
    }


    private void clearResultPolys () {
        dl.eraseSelectable (resultSel); 
        repaintPanel ();
        resultSel = null;
        clearResultButton.setEnabled (false);
    }




/*
 * Implement the DLEditListener interface
 */
    public boolean editChanged (DLSelectable dls) {
        return false;
    }


    public boolean editMoved (DLSelectable dls) {
        return false;
    }


    public boolean editFinished (DLSelectable dls) {

        sourceButton.setEnabled (true);
        clipButton.setEnabled (true);

        if (dls == null) {
            System.out.println ("dls is null in editFinished");
            System.out.println ();
            System.out.flush ();
            dig_type = DIG_TYPE_UNKNOWN;
            return false;
        }

        ArrayList<DLFill> flist = dls.getFillList ();
        if (flist == null) {
            System.out.println ("fill list is null in editFinished");
            System.out.println ();
            System.out.flush ();
            dig_type = DIG_TYPE_UNKNOWN;
            return false;
        }

        if (dig_type == DIG_TYPE_SOURCE) {
            DLSelectable _dls = new DLSelectable ();
            sourceSel.add (_dls);
            dl.setSelectable (_dls);
            setSourceAttributes ();
        }
        else {
            DLSelectable _dls = new DLSelectable ();
            clipSel.add (_dls);
            dl.setSelectable (_dls);
            setClipAttributes ();
        }
        int  n = 0;
        dl.setLineThickness(.002);
        for (DLFill dlf : flist) {
            if (dlf == null) continue;
            int ncomp = dlf.getNumComponents ();
            if (ncomp < 1) continue;
            if (dig_type == DIG_TYPE_SOURCE) {
                sourcePolyList.add (dlf);
            }
            else {
                clipPolyList.add (dlf);
            }
            int[] npts_arr = dlf.getNumPoints ();
            double[] xp = dlf.getXPoints();
            double[] yp = dlf.getYPoints();
            dl.addFill (xp, yp, npts_arr, ncomp, 1);
        }

        dl.setSelectable (null);

        if (sourcePolyList.size() > 0  &&  clipPolyList.size() > 0) {
            intersectButton.setEnabled (true);
            unionButton.setEnabled (true);
            xorButton.setEnabled (true);
            fragmentButton.setEnabled (true);
        }

        dig_type = DIG_TYPE_UNKNOWN;

        return true;

    }


    void setSourceAttributes () {
        dl.setFillColor (-1, -1, -1);
        dl.setBorderColor (255, 0, 0);
    }


    void setClipAttributes () {
        dl.setFillColor (-1, -1, -1);
        dl.setBorderColor (0, 0, 255);
    }

};

