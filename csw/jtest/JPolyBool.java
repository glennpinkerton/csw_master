
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
import java.lang.Math;

import java.io.FileOutputStream;
import java.io.ObjectOutputStream;
import java.io.FileInputStream;
import java.io.ObjectInputStream;
import java.io.IOException;

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
 *  and to display the results.
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
    private JButton        randomButton = null;
    private JButton        random4Button = null;
    private JButton        gridButton = null;
    private JButton        grid2Button = null;
    private JButton        intersectButton = null;
    private JButton        unionButton = null;
    private JButton        xorButton = null;
    private JButton        clearButton = null;
    private JButton        clearResultButton = null;
    private JButton        saveButton = null;
    private JButton        restoreButton = null;

    private ArrayList<DLSelectable>   sourceSel = new ArrayList<DLSelectable> ();
    private ArrayList<DLSelectable>   clipSel = new ArrayList<DLSelectable> ();
    private DLSelectable   resultSel = null;

    private ArrayList<DLFill>  sourcePolyList = new ArrayList<DLFill> ();
    private ArrayList<DLFill>  clipPolyList = new ArrayList<DLFill> ();

    public DigitizeFrame ()
    {
        super (FRAME_WITH_TOOLBAR, true);

        setSize (800, 800);

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

        randomButton =
        addAdditionalTextButton (
             "Random",
             "Generate Random Source Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.generateRandom ();
               }
             },
             null
        );

        random4Button =
        addAdditionalTextButton (
             "Random4",
             "Generate 4 Random Source Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.clearPolys ();
                 DigitizeFrame.this.generateRandom4 ();
               }
             },
             null
        );

        gridButton =
        addAdditionalTextButton (
             "Grid",
             "Generate Sealed Grid of Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.generateGrid ();
               }
             },
             null
        );

        grid2Button =
        addAdditionalTextButton (
             "Grid 2",
             "Generate Sealed Grid of Many Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.generateGrid2 ();
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

        saveButton =
        addAdditionalTextButton (
             "Save",
             "Save Current Source and Clip Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.saveInputPolys ();
               }
             },
             null
        );

        restoreButton =
        addAdditionalTextButton (
             "Restore",
             "Restore Most Recently Saved Source and Clip Polygons",
             new ActionListener() {
               public void actionPerformed(ActionEvent ae){
                 DigitizeFrame.this.restoreInputPolys ();
               }
             },
             null
        );



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





    private void dumpSourcePolys () {

        for (DLFill dlf : sourcePolyList) {
            DLSelectable _dls = new DLSelectable ();
            sourceSel.add (_dls);
            dl.setSelectable (_dls);
            setSourceAttributes ();
            int ncomp = dlf.getNumComponents ();
            if (ncomp < 1) continue;
            int[] npts_arr = dlf.getNumPoints ();
        }

    }


    private void drawPolys () {

        dl.setLineThickness(.002);

        for (DLFill dlf : sourcePolyList) {
            DLSelectable _dls = new DLSelectable ();
            sourceSel.add (_dls);
            dl.setSelectable (_dls);
            setSourceAttributes ();
            int ncomp = dlf.getNumComponents ();
            if (ncomp < 1) continue;
            int[] npts_arr = dlf.getNumPoints ();
            double[] xp = dlf.getXPoints();
            double[] yp = dlf.getYPoints();
            dl.addFill (xp, yp, npts_arr, ncomp, 1);
        }

        for (DLFill dlf : clipPolyList) {
            DLSelectable _dls = new DLSelectable ();
            clipSel.add (_dls);
            dl.setSelectable (_dls);
            setClipAttributes ();
            int ncomp = dlf.getNumComponents ();
            if (ncomp < 1) continue;
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
        }

        dig_type = DIG_TYPE_UNKNOWN;

        repaintPanel ();

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

/*
System.out.println ();
System.out.println ("Intersect finished with " + dlf_out.size() +
          " polygons in result");
System.out.println ();
System.out.flush ();
*/

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

        dl.eraseSelectable (resultSel);

        resultSel = new DLSelectable ();
        dl.setSelectable (resultSel);

        dl.setFillColor (200, 100, 100, 100);
        dl.setBorderColor (200, 0, 0);
        dl.setLineThickness(.02);

/*
System.out.println ();
System.out.println ("Union finished with " + dlf_out.size() +
          " polygons in result");
System.out.println ();
System.out.flush ();
*/

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

/*
System.out.println ();
System.out.println ("Xor finished with " + dlf_out.size() +
          " polygons in result");
System.out.println ();
System.out.flush ();
*/

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


/*
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
            dl.setFillColor ((n * 40) % 250,  (n * 30) % 250, (n * 20) % 250, 100);
            dl.setBorderColor ((n * 10) % 250,  (n * 20) % 250, (n * 30) % 250);
            n++;
            dl.addFill (xp, yp, npts_arr, ncomp, 0);
            dlf.outlineComponents (dl);
        }

        dl.setSelectable (null);

        repaintPanel ();

        clearResultButton.setEnabled (true);

    }
*/


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
    }


    private void clearResultPolys () {
        dl.eraseSelectable (resultSel); 
        repaintPanel ();
        resultSel = null;
        clearResultButton.setEnabled (false);
    }


    @SuppressWarnings("unchecked")
    private void restoreInputPolys () {

      clearPolys ();

      String  fparent = System.getenv ("CSW_PARENT");
      String  fdir = fparent + "/csw/jtest/";
      try {
         FileInputStream fileIn =
         new FileInputStream(fdir + "source_polys.ser");
         ObjectInputStream oin = new ObjectInputStream(fileIn);
         sourcePolyList = (ArrayList<DLFill>) oin.readObject();
         oin.close();
         fileIn.close();
         fileIn =
         new FileInputStream(fdir + "clip_polys.ser");
         oin = new ObjectInputStream(fileIn);
         clipPolyList = (ArrayList<DLFill>) oin.readObject();
         oin.close();
         fileIn.close();
      }
      catch (IOException ex) {
         System.out.println("IO exception for restore");
         ex.printStackTrace();
         return;
      }
      catch (ClassNotFoundException ex) {
         System.out.println("class not found for restore");
         ex.printStackTrace();
         return;
      }

      drawPolys ();
 
    }



    private void saveInputPolys () {

      String  fparent = System.getenv ("CSW_PARENT");
      String  fdir = fparent + "/csw/jtest/";
      try {
         FileOutputStream fileOut =
         new FileOutputStream(fdir + "source_polys.ser");
         ObjectOutputStream out = new ObjectOutputStream(fileOut);
         out.writeObject(sourcePolyList);
         out.close();
         fileOut.close();
         fileOut =
         new FileOutputStream(fdir + "clip_polys.ser");
         out = new ObjectOutputStream(fileOut);
         out.writeObject(clipPolyList);
         out.close();
         fileOut.close();
      } catch (IOException ex) {
         System.out.println("IO exception for save");
         ex.printStackTrace();
      }
 
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
        }

        dig_type = DIG_TYPE_UNKNOWN;

        return true;

    }



    private void generateRandom4 () {

        Random  ran = new Random ();

        int maxpts = 10;
        int mprand = 100;

        String stm = System.getenv ("PBOOL_RANDOM_MAXPTS");
        try {
            Integer iob = Integer.parseInt (stm);
            mprand = iob.intValue();
        }
        catch (Throwable ex) {
        }
        if (mprand > 100000) mprand = 100000;
        maxpts = ran.nextInt (mprand) + mprand / 2;
        if (maxpts < 10) maxpts = 10;

        double  dang = Math.PI * 2.0 / (maxpts - 1);

        DLSelectable _dls = new DLSelectable ();
        sourceSel.add (_dls);
        dl.setSelectable (_dls);
        setSourceAttributes ();
        dl.setLineThickness(.002);

        for (int i=0; i<4; i++) {

            double xc = ran.nextDouble () * 30.0 +  i * 60.0;
            double yc = ran.nextDouble () * 30.0 +  i * 60.0;
            if (i == 2) {
                xc = 25.0;
                yc = 75.0;
            }
            if (i == 3) {
                xc = 75.0;
                yc = 25.0;
            }
            double rad = ran.nextDouble () * 20.0 + 10.0;
            double rad11 = rad * 1.1;
            if (xc < rad11) xc = rad11;
            if (yc < rad11) yc = rad11;
            if (xc > 100. - rad11) xc = 100. - rad11;
            if (yc > 100. - rad11) yc = 100. - rad11;

            double[] xgr = new double[maxpts * 3 / 2];
            double[] ygr = new double[maxpts * 3 / 2];
            int[] npgr = new int[2];

            int  nxy = 0;
            double  ang = 0.0;
            npgr[0] = maxpts;

            for (int j=0; j<maxpts-1; j++) {
                double rt = ran.nextDouble () * rad * .05 + rad * .5;
                double xt = xc + rt * Math.cos(ang);
                double yt = yc + rt * Math.sin(ang);
                xgr[nxy] = xt;
                ygr[nxy] = yt;
                nxy++;
                ang += dang;
            }
            xgr[nxy] = xgr[0];
            ygr[nxy] = ygr[0];
            nxy++;

            int holebase = nxy;
            ang = 0.0;
            npgr[1] = maxpts / 2;

            for (int j=0; j<maxpts/2-1; j++) {
                double rt = ran.nextDouble () * rad * .02 + rad * .2;
                double xt = xc + rt * Math.cos(ang);
                double yt = yc + rt * Math.sin(ang);
                xgr[nxy] = xt;
                ygr[nxy] = yt;
                nxy++;
                ang +=  2.0 * dang;
            }
            xgr[nxy] = xgr[holebase];
            ygr[nxy] = ygr[holebase];

            DLFill  dlf = new DLFill ();
            dlf.setNumComponents (2);
            dlf.setNumPoints (npgr);
            dlf.setXPoints (xgr);
            dlf.setYPoints (ygr);

            sourcePolyList.add (dlf);

            dl.addFill (xgr, ygr, npgr, 2, 1);
        }

        dl.setSelectable (null);

        repaintPanel ();

    }


    private void generateRandom () {

        Random  ran = new Random ();

        int maxpts = 10;
        int mprand = 100;

        String stm = System.getenv ("PBOOL_RANDOM_MAXPTS");
        try {
            Integer iob = Integer.parseInt (stm);
            mprand = iob.intValue();
        }
        catch (Throwable ex) {
        }
        maxpts = ran.nextInt (mprand) + mprand / 10;
        if (maxpts < 10) maxpts = 10;

        double  dang = Math.PI * 2.0 / (maxpts - 1);

        DLSelectable _dls = new DLSelectable ();
        sourceSel.add (_dls);
        dl.setSelectable (_dls);
        setSourceAttributes ();
        dl.setLineThickness(.002);

        for (int i=0; i<1; i++) {

            double xc = ran.nextDouble () * 50.0 +  25.0;
            double yc = ran.nextDouble () * 50.0 +  25.0;
            double rad = ran.nextDouble () * 30.0 + 10.0;
            double rad11 = rad * 1.1;
            if (xc < rad11) xc = rad11;
            if (yc < rad11) yc = rad11;
            if (xc > 100. - rad11) xc = 100. - rad11;
            if (yc > 100. - rad11) yc = 100. - rad11;

            double[] xgr = new double[maxpts * 3 / 2];
            double[] ygr = new double[maxpts * 3 / 2];
            int[] npgr = new int[2];

            int  nxy = 0;
            double  ang = 0.0;
            npgr[0] = maxpts;

            for (int j=0; j<maxpts-1; j++) {
                double rt = ran.nextDouble () * rad * .01 + rad * .5;
                double xt = xc + rt * Math.cos(ang);
                double yt = yc + rt * Math.sin(ang);
                xgr[nxy] = xt;
                ygr[nxy] = yt;
                nxy++;
                ang += dang;
            }
            xgr[nxy] = xgr[0];
            ygr[nxy] = ygr[0];
            nxy++;

            int holebase = nxy;
            ang = 0.0;
            npgr[1] = maxpts / 2;

            for (int j=0; j<maxpts/2-1; j++) {
                double rt = ran.nextDouble () * rad * .005 + rad * .2;
                double xt = xc + rt * Math.cos(ang);
                double yt = yc + rt * Math.sin(ang);
                xgr[nxy] = xt;
                ygr[nxy] = yt;
                nxy++;
                ang +=  2.0 * dang;
            }
            xgr[nxy] = xgr[holebase];
            ygr[nxy] = ygr[holebase];

            DLFill  dlf = new DLFill ();
            dlf.setNumComponents (2);
            dlf.setNumPoints (npgr);
            dlf.setXPoints (xgr);
            dlf.setYPoints (ygr);

            sourcePolyList.add (dlf);

            dl.addFill (xgr, ygr, npgr, 2, 1);
        }

        dl.setSelectable (null);

        repaintPanel ();

    }


    void setSourceAttributes () {
        dl.setFillColor (-1, -1, -1);
        dl.setBorderColor (255, 0, 0);
    }


    void setClipAttributes () {
        dl.setFillColor (-1, -1, -1);
        dl.setBorderColor (0, 0, 255);
    }



    private void generateGrid () {

        Random  ran = new Random ();

        DLSelectable _dls = new DLSelectable ();
        sourceSel.add (_dls);
        dl.setSelectable (_dls);
        setSourceAttributes ();
        dl.setLineThickness(.002);

        double[] xgr = new double[5];
        double[] ygr = new double[5];
        int[] npgr = new int[1];

        npgr[0] = 5;
        xgr[0] = 10.0;
        ygr[0] = 10.0;
        xgr[1] = 90.0;
        ygr[1] = 10.0;
        xgr[2] = 90.0;
        ygr[2] = 30.0;
        xgr[3] = 10.0;
        ygr[3] = 30.0;
        xgr[4] = xgr[0];
        ygr[4] = ygr[0];

        DLFill  dlf = new DLFill ();
        dlf.setNumComponents (2);
        dlf.setNumPoints (npgr);
        dlf.setXPoints (xgr);
        dlf.setYPoints (ygr);

        sourcePolyList.add (dlf);
        dl.addFill (xgr, ygr, npgr, 1, 1);

        xgr = new double[5];
        ygr = new double[5];
        npgr = new int[1];

        npgr[0] = 5;
        xgr[0] = 10.0;
        ygr[0] = 90.0;
        xgr[1] = 90.0;
        ygr[1] = 90.0;
        xgr[2] = 90.0;
        ygr[2] = 70.0;
        xgr[3] = 10.0;
        ygr[3] = 70.0;
        xgr[4] = xgr[0];
        ygr[4] = ygr[0];

        dlf = new DLFill ();
        dlf.setNumComponents (2);
        dlf.setNumPoints (npgr);
        dlf.setXPoints (xgr);
        dlf.setYPoints (ygr);

        sourcePolyList.add (dlf);
        dl.addFill (xgr, ygr, npgr, 1, 1);

        xgr = new double[5];
        ygr = new double[5];
        npgr = new int[1];

        npgr[0] = 5;
        xgr[0] = 10.0;
        ygr[0] = 70.0;
        xgr[1] = 30.0;
        ygr[1] = 70.0;
        xgr[2] = 30.0;
        ygr[2] = 30.0;
        xgr[3] = 10.0;
        ygr[3] = 30.0;
        xgr[4] = xgr[0];
        ygr[4] = ygr[0];

        dlf = new DLFill ();
        dlf.setNumComponents (2);
        dlf.setNumPoints (npgr);
        dlf.setXPoints (xgr);
        dlf.setYPoints (ygr);

        sourcePolyList.add (dlf);
        dl.addFill (xgr, ygr, npgr, 1, 1);

        xgr = new double[5];
        ygr = new double[5];
        npgr = new int[1];

        npgr[0] = 5;
        xgr[0] = 70.0;
        ygr[0] = 70.0;
        xgr[1] = 90.0;
        ygr[1] = 70.0;
        xgr[2] = 90.0;
        ygr[2] = 30.0;
        xgr[3] = 70.0;
        ygr[3] = 30.0;
        xgr[4] = xgr[0];
        ygr[4] = ygr[0];

        dlf = new DLFill ();
        dlf.setNumComponents (2);
        dlf.setNumPoints (npgr);
        dlf.setXPoints (xgr);
        dlf.setYPoints (ygr);

        sourcePolyList.add (dlf);
        dl.addFill (xgr, ygr, npgr, 1, 1);


        dl.setSelectable (null);

        repaintPanel ();

    }


    private void generateGrid2 ()
    {
        int  ncol = 3, nrow = 3;
        String stm = System.getenv ("PBOOL_GRID2_MAX");
        try {
            Integer iob = Integer.parseInt (stm);
            nrow = iob.intValue();
            ncol = nrow;
        }
        catch (Throwable ex) {
        }
        if (nrow > 100) nrow = 100;
        if (nrow < 3) nrow = 3;
        ncol = nrow;

        double  xspace = 80.0 / (double)(ncol - 1);
        double  yspace = 80.0 / (double)(nrow - 1);
        double  xsp5 = xspace / 5.0;
        double  ysp5 = yspace / 5.0;

        double[] xoff = new double[nrow * ncol];
        double[] yoff = new double[nrow * ncol];

        Random rand = new Random (12357);

        for (int i=0; i<nrow*ncol; i++) {
            xoff[i] = rand.nextDouble () * xsp5;
            xoff[i] -= .5 * xoff[i];
            yoff[i] = rand.nextDouble () * ysp5;
            yoff[i] -= .5 * yoff[i];
        }

        double[] xp = null;
        double[] yp = null;

        int[] npa = null;

        DLFill dlf = null;

        DLSelectable _dls = new DLSelectable ();
        sourceSel.add (_dls);
        dl.setSelectable (_dls);

        setSourceAttributes ();
        dl.setLineThickness(.002);

        int  k = 0;

System.out.println ();
System.out.println ("start grid2 source   " + nrow + "    " + ncol);
System.out.flush ();



        for (int i=0; i<nrow-1; i++) {
          double y1 = i * yspace + 10.0;
          double y2 = (i + 1) * yspace + 10.0;
          int ianc = i * ncol;
          for (int j=0; j<ncol - 1; j++) {
            npa = new int[1];
            npa[0] = 5;
            k = ianc + j;
            double x1 = j * xspace + 10.0;
            double x2 = (j + 1) * xspace + 10.0;
            xp = new double[5];
            xp[0] = x1 + xoff[k];
            xp[1] = x1 + xoff[k+ncol];
            xp[2] = x2 + xoff[k+1+ncol];
            xp[3] = x2 + xoff[k+1];
            xp[4] = x1 + xoff[k];
            yp = new double[5];
            yp[0] = y1 + yoff[k];
            yp[1] = y2 + yoff[k+ncol];
            yp[2] = y2 + yoff[k+1+ncol];
            yp[3] = y1 + yoff[k+1];
            yp[4] = y1 + yoff[k];
            dlf = new DLFill ();
            dlf.setNumComponents (1);
            dlf.setNumPoints (npa);
            dlf.setXPoints (xp);
            dlf.setYPoints (yp);

if (k % 100 == 0) {
System.out.println ("    source k = " + k);
System.out.flush ();
}

            sourcePolyList.add (dlf);
            dl.addFill (xp, yp, npa, 1, 1);
          }
        }

System.out.println ("finished grid2 source");
System.out.flush ();

        setClipAttributes ();
        dl.setLineThickness(.002);

        DLSelectable clip_dls = new DLSelectable ();
        clipSel.add (clip_dls);
        dl.setSelectable (clip_dls);

        for (int i=0; i<nrow-1; i++) {
          double y1 = i * yspace + 12.0;
          double y2 = (i + 1) * yspace + 12.0;
          yp = new double[5];
          yp[0] = y1;
          yp[1] = y2;
          yp[2] = y2;
          yp[3] = y1;
          yp[4] = y1;
          for (int j=0; j<ncol - 1; j++) {
            npa = new int[1];
            npa[0] = 5;
            double x1 = j * xspace + 12.0;
            double x2 = (j + 1) * xspace + 12.0;
            xp = new double[5];
            xp[0] = x1;
            xp[1] = x1;
            xp[2] = x2;
            xp[3] = x2;
            xp[4] = x1;
            dlf = new DLFill ();
            dlf.setNumComponents (1);
            dlf.setNumPoints (npa);
            dlf.setXPoints (xp);
            dlf.setYPoints (yp);

k = i * ncol + j;
if (k % 100 == 0) {
System.out.println ("    clip k = " + k);
System.out.flush ();
}

            clipPolyList.add (dlf);
            dl.addFill (xp, yp, npa, 1, 1);
          }
        }

        dl.setSelectable (null);

System.out.println ("finished grid2 clip");
System.out.flush ();

        intersectButton.setEnabled (true);
        unionButton.setEnabled (true);
        xorButton.setEnabled (true);

        repaintPanel ();

System.out.println ("after grid2 repaint");
System.out.flush ();



    }

};
