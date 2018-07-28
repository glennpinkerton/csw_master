
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


// imports flagged as unused by eclipse

//import java.util.Date;
//import org.apache.logging.log4j.LogManager;



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

//        timerTest ();

    }

    /*
    static private void timerTest ()
    {
        double[] d1, d2;
        int      i, j, k, itot, jtot;

        Date     date;
        long     t1, t2;

        date = new Date ();
        t1 = date.getTime ();

        itot = 2000;
        jtot = 2000;

        for (i=0; i<itot; i++) {
            d1 = new double[jtot];
            for (j=0; j<jtot; j++) {
                d1[j] = j + i;
            }

            d2 = new double[jtot+ 2 * i];
            for (j=0; j<jtot + 2 * i; j++) {
                k = j % 100;
                d2[j] = j + i + k;
            }
         }

         date = new Date ();
         t2 = date.getTime ();

         System.out.print ("Elapsed time for itot = ");
         System.out.print (itot);
         System.out.print (" and jtot = ");
         System.out.print (jtot);
         System.out.print (" is ");
         System.out.print (t2 - t1);
         System.out.println (" milliseconds");

         return;
    }
    */


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


class JPolyBoolFrame extends JFrame
{
    private static final long serialVersionUID = 1L;

    public JPolyBoolFrame ()
    {

/*
 * Possible wait spot to allow a gdb attach to the JVM process.
 * This can sometimes work for debug of native C/C++ code.
try {
int idum = System.in.read();
}
catch (Exception e) {
}
*/

        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
        setLocation (screenSize.width - 250, 50);

        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        setTitle ("JDisplayList Unit Test");
        setSize (220, 250);
        setResizable (false);
        Container contentPane = getContentPane ();
        contentPane.setLayout (new GridLayout(6,1));

        JButton show_mem_button = new JButton ("Show Memory");
        show_mem_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            JPolyBool.showMem ("From user button click");
          }
        });

        JButton digitize_button = new JButton ("Digitize Polygons");
        digitize_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                DigitizeFrame frame = new DigitizeFrame ();
                frame.setVisible(true);
          }
        });

        contentPane.add (show_mem_button);
        contentPane.add (digitize_button);

    }



}



class DigitizeFrame extends JDLFrame implements DLEditListener
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();
    
    public DigitizeFrame ()
    {
        super (FRAME_WITHOUT_TOOLBAR);

        setTitle ("Digitize For Boolean");

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (225, 225, 225);
        dl.beginPlot ("digitize poly boolean",
                      0.0, 0.0, 200.0, 200.0);
        dl.setColor (5, 5, 5);

        dl.createFrame ("source poly frame",
                        0.0, 0.0, 100.0, 100.0);

//        dl.setSelectable (new DLSelectable());

        dl.pickNewPolygonGeometry (this, null);



    }

/*
 * Implement the DLEditListener intwerface
 */
    public boolean editChanged (DLSelectable dls) {
        return false;
    }


    public boolean editMoved (DLSelectable dls) {
        return false;
    }


    public boolean editFinished (DLSelectable dls) {
        return false;
    }

};

