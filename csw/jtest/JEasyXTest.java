
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
 * This class is used as a main entry for unit testing 2d graphics stuff.
 *
 *  @author Glenn Pinkerton
 *
*/
public class JEasyXTest {

    private static Logger  logger = CSWLogger.getMyLogger ();
    
    static {
        System.loadLibrary ("_csw_all");
    }

    public static void main (String[] args) {

        Runnable run = new Runnable ()
        {
            public void run ()
            {
                JEasyXTest.runMainMethod ();
            }
        };

        SwingUtilities.invokeLater (run);
    }


    public static void runMainMethod ()
    {

        logger.info ("    Running main method in JEasyXTest\n\n");

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

        String  playback_file_name = cpar + "/csw/jtest/utest.ezx";

        JDisplayList.openLogFile (playback_file_name);
        JEasyXTestFrame frame = new JEasyXTestFrame ();
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

/*
        long  maxmem = Runtime.getRuntime().maxMemory ();
        long  totmem = Runtime.getRuntime().totalMemory ();

        maxmem /= 1000000;
        totmem /= 1000000;

        System.out.println ();
        System.out.println ("maxmem = " + maxmem + "   total mem = " + totmem);
        if (msg != null) {
            System.out.println (msg);
        }
        System.out.println ();
        System.out.flush ();
*/

    }

}


class JEasyXTestFrame extends JFrame
{
    private static final long serialVersionUID = 1L;

    private SmallerFrame      frame1 = null;
    private Frame2Frame      frame2 = null;
    private TextFrameTest    frame3 = null;

    private static Logger  logger = CSWLogger.getMyLogger ();
    

    public JEasyXTestFrame ()
    {

/*
 * Possible wait spot to allow gdb attach to JVM process.
 * Can sometimes work for debug of native C/C++ code.
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
        setSize (220, 750);
        setResizable (false);
        Container contentPane = getContentPane ();
        contentPane.setLayout (new GridLayout(25,1));

        JButton random_button = new JButton ("Random Primitives");
        random_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                RandomPrimFrame frame = new RandomPrimFrame ();
                frame.setVisible(true);
          }
        });

        JButton reverse_button = new JButton ("Reverse Axes");
        reverse_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                ReversePrimFrame frame = new ReversePrimFrame ();
                frame.setVisible(true);
          }
        });

        JButton dashed_button = new JButton ("Dashed Lines");
        dashed_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                DashedLineFrame frame = new DashedLineFrame ();
                frame.setVisible(true);
          }
        });

        JButton lith_button = new JButton ("Lith Patterns");
        lith_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                LithFrame frame = new LithFrame ();
                frame.setVisible(true);
          }
        });

        JButton dashed2_button = new JButton ("Dashed Lines * 2.0");
        dashed2_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                Dashed2LineFrame frame = new Dashed2LineFrame ();
                frame.setVisible(true);
          }
        });

        JButton symbol_button = new JButton ("Symbol Examples");
        symbol_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                SymbolFrame frame = new SymbolFrame ();
                frame.setVisible(true);
          }
        });

        JButton text_button = new JButton ("Fonts 0 - 19");
        text_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                TextTableFrame frame = new TextTableFrame ();
                frame.setVisible(true);
          }
        });

        JButton text2_button = new JButton ("Fonts 101 - 107");
        text2_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                Text2TableFrame frame = new Text2TableFrame ();
                frame.setVisible(true);
          }
        });

        JButton text3_button = new JButton ("Default font");
        text3_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                Text3TableFrame frame = new Text3TableFrame ();
                frame.setVisible(true);
          }
        });

        JButton noaspect_button = new JButton ("No Aspect Frame");
        noaspect_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                NoAspectFrame frame = new NoAspectFrame ();
                frame.setVisible(true);
          }
        });

        JButton small_button = new JButton ("Small Random Frame");
        small_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                SmallerFrame frame = new SmallerFrame ();
                setFrame1 (frame);
                frame.setVisible(true);
          }
        });

        JButton medium_button = new JButton ("Medium Random Frame");
        medium_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                HugeFrame hf = new HugeFrame (50);
                HugeFrameRunnable run_frame = new HugeFrameRunnable (hf);
                SwingUtilities.invokeLater (run_frame);
          }
        });

        JButton big_button = new JButton ("Big Random Frame");
        big_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                HugeFrame hf = new HugeFrame (1000);
                HugeFrameRunnable run_frame = new HugeFrameRunnable (hf);
                SwingUtilities.invokeLater (run_frame);
          }
        });

        JButton huge_button = new JButton ("Huge Random Frame");
        huge_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                HugeFrame hf = new HugeFrame (-1);
                HugeFrameRunnable run_frame = new HugeFrameRunnable (hf);
                SwingUtilities.invokeLater (run_frame);
          }
        });

        JButton frame2_button = new JButton ("Symbol Frame");
        frame2_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                Frame2Frame frame = new Frame2Frame ();
                setFrame2 (frame);
                frame.setVisible(true);
          }
        });

        JButton text_frame_button = new JButton ("Text Frame");
        text_frame_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                TextFrameTest frame = new TextFrameTest ();
                setTextFrameTest (frame);
                frame.setVisible(true);
          }
        });

        JButton frame_layout_button = new JButton ("Frame Layout");
        frame_layout_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                FrameLayoutTest frame = new FrameLayoutTest ();
                frame.setVisible(true);
          }
        });

        JButton grid_button = new JButton ("Grid Test");
        grid_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                GridTest frame = new GridTest ();
                frame.setVisible(true);
          }
        });

        JButton overlap_button = new JButton ("Overlapping Grid Test");
        overlap_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                OverlapTest frame = new OverlapTest ();
                frame.setVisible(true);
          }
        });

        JButton huge_grid_button = new JButton ("Huge Grid Test");
        huge_grid_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                HugeGridTest hg = new HugeGridTest ();
                HugeGridRunnable run_frame = new HugeGridRunnable (hg);
                SwingUtilities.invokeLater (run_frame);
          }
        });

        JButton font_button = new JButton ("Font Test");
        font_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
                FontFrame frame = new FontFrame ();
                frame.setVisible(true);
          }
        });

        JButton sync_button = new JButton ("Synchronized Frame Test");
        sync_button.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
              FrameSyncTest frame = new FrameSyncTest ();
              frame.setVisible(true);
          }
        });

        contentPane.add (dashed_button);
        contentPane.add (dashed2_button);
        contentPane.add (lith_button);
        contentPane.add (symbol_button);
        contentPane.add (text_button);
        contentPane.add (text2_button);
        contentPane.add (text3_button);
        contentPane.add (random_button);
        contentPane.add (small_button);
        contentPane.add (medium_button);
        contentPane.add (big_button);
        contentPane.add (huge_button);
        contentPane.add (reverse_button);
        contentPane.add (frame2_button);
        contentPane.add (text_frame_button);
        contentPane.add (frame_layout_button);
        contentPane.add (grid_button);
        contentPane.add (overlap_button);
        contentPane.add (huge_grid_button);
        contentPane.add (font_button);
        contentPane.add (sync_button);
        contentPane.add (noaspect_button);

        logger.info ("    EasyX Test Frame Constructed Successfully    ");

    }



    SmallerFrame getFrame1 () {return frame1;}
    Frame2Frame getFrame2 () {return frame2;}
    TextFrameTest getTextFrameTest () {return frame3;}
    void setFrame1 (SmallerFrame f) {frame1 = f;}
    void setFrame2 (Frame2Frame f) {frame2 = f;}
    void setTextFrameTest (TextFrameTest f) {frame3 = f;}

}



class RandomPrimFrame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();
    
    public RandomPrimFrame ()
    {
        super (FRAME_WITHOUT_TOOLBAR);

        double[] xline, yline;
        int[]    icomp;

        xline = new double[10];
        yline = new double[10];
        icomp = new int[10];

        setTitle ("Random Primitives");

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("random prim test",
                      0.0, 0.0, 200.0, 200.0);
        dl.setColor (5, 5, 5);

        xline[0] = 10.0;
        yline[0] = 10.0;
        xline[1] = 90.0;
        yline[1] = 20.0;
        xline[2] = 80.0;
        yline[2] = 80.0;
        xline[3] = 15.0;
        yline[3] = 85.0;
        dl.addLine(xline, yline, 4);

        dl.setLineThickness (0.05);
        dl.setLineSmooth (50);
        dl.addLine(xline, yline, 4);

        dl.setLineSmooth (0);
        dl.setLineThickness (.01);

        xline[0] = 40.0;
        yline[0] = 40.0;
        xline[1] = 60.0;
        yline[1] = 40.0;
        xline[2] = 60.0;
        yline[2] = 50.0;
        xline[3] = 45.0;
        yline[3] = 65.0;
        icomp[0] = 4;
        dl.setFillColor (255, 0, 0);
        dl.setBorderColor (0, 255, 0);
        dl.addFill(xline, yline, icomp, 1, 1);

        dl.setFillColor (0, 0, 255);
        dl.setBorderColor (255, 0, 0);
        dl.addCircle (10.0, 10.0, 15.0, 15.0, 10.0, 15.0);

        dl.setFillColor (-1, -1, -1);
        dl.setBorderColor (0, 0, 0);
        dl.addCenteredRectangle (40.0, 12.0, 20.0, 14.0, 3.0, 45.0);

        dl.setFillColor (200, 250, 200);
        dl.setBorderColor (0, 0, 0);
        dl.addArc (80.0, 22.0, 10.0, 7.0, 45.0, 300.0, 0.0, 1);

        dl.setFillColor (-1, -1, -1);
        dl.setBorderColor (255, 0, 0);
        dl.addArc (80.0, 22.0, 10.0, 7.0, 45.0, 300.0, 90.0, 1);

        dl.setTextColor (0, 0, 0);
        dl.setFont ("arial", Font.PLAIN);
        dl.addNumber (15.0, 150.0, .15, 0.0, 1.23, 2, 0);
        dl.addNumber (15.0, 170.0, .15, 0.0, 1.23456e15, 5, 0);
        dl.addNumber (15.0, 190.0, .15, 0.0, 123456789, 0, 1);

        logger.info ("    Random primitives without a freme finished    ");
    }

};

class SymbolFrame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();
    
    public SymbolFrame ()
    {
        super (FRAME_WITHOUT_TOOLBAR);

        double xsym, ysym;
        int i;

        setTitle ("Symbol Examples");
        setSize (900, 700);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("symbol table test",
                      0.0, 0.0, 10.0, 10.0);
        dl.setColor (5, 5, 5);

        dl.setSymbolColor (0, 0, 0);
        dl.setLineThickness (.01);
        dl.setTextOffset (-.2, -.2);
        for (i=0; i<140; i++) {
            xsym = 0.5 * (double)(i%15) + 0.25;
            ysym = 0.5 * (double)(i/15) + 0.25;
            ysym = 10.0 - ysym;
            dl.addSymbol (xsym, ysym, .15, 0.0, i+1);
            dl.addNumber (xsym, ysym, .10, 0.0, (double)(i+1), 0, 0);
        }

        logger.info ("    Symbols with a freme finished    ");
    }
};

class TextTableFrame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();
    
    public TextTableFrame ()
    {

        super (FRAME_WITHOUT_TOOLBAR);

        setTitle ("Text Fonts 0 - 19");
        setSize (1100, 700);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("fonts 0 - 19",
                      0.0, 0.0, 12.0, 12.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("random prim test",
                        0.0,
                        0.0,
                        18.0,
                        12.0,
                        0.0,
                        0.0,
                        18.0,
                        12.0,
                        0.0,
                        0.0,
                        18.0,
                        12.0,
                        1,
                        0,
                        0,
                        DLConst.FRAME_NO_BORDER,
                        null,
                        null,
                        null,
                        DLConst.FRAME_NO_ATTACH,
                        0.0,
                        0.0,
                        0);

        dl.setTextColor (0, 0, 0);
        dl.setTextFillColor (200, 200, 200);

        dl.setFont (0);
        dl.addText (0.0, 10.0, 0.12, 0.0, "Font 0:  ABCdef");

        dl.setFont (1);
        dl.addText (0.0, 9.0, 0.12, 0.0, "Font 1:  ABCdef");

        dl.setFont (2);
        dl.addText (0.0, 8.0, 0.12, 0.0, "Font 2:  ABCdef");

        dl.setFont (3);
        dl.addText (0.0, 7.0, 0.12, 0.0, "Font 3:  ABCdef");

        dl.setFont (4);
        dl.addText (0.0, 6.0, 0.12, 0.0, "Font 4:  ABCdef");

        dl.setFont (5);
        dl.addText (0.0, 5.0, 0.12, 0.0, "Font 5:  ABCdef");

        dl.setFont (6);
        dl.addText (0.0, 4.0, 0.12, 0.0, "Font 6:  ABCdef");

        dl.setFont (7);
        dl.addText (0.0, 3.0, 0.12, 0.0, "Font 7:  ABCdef");

        dl.setFont (8);
        dl.addText (0.0, 2.0, 0.3, 0.0, "Font 8:  ABCdef");

        dl.setFont (9);
        dl.addText (0.0, 1.0, 0.3, 0.0, "Font 9:  ABCdef");

        dl.setFont (10);
        dl.addText (7.0, 10.0, 0.3, 0.0, "Font 10:  ABCdef");

        dl.setFont (11);
        dl.addText (7.0, 9.0, 0.3, 0.0, "Font 11:  ABCdef");

        dl.setFont (12);
        dl.addText (7.0, 8.0, 0.3, 0.0, "Font 12:  ABCdef");

        dl.setFont (13);
        dl.addText (7.0, 7.0, 0.3, 0.0, "Font 13:  ABCdef");

        dl.setFont (14);
        dl.addText (7.0, 6.0, 0.3, 0.0, "Font 14:  ABCdef");

        dl.setFont (15);
        dl.addText (7.0, 5.0, 0.3, 0.0, "Font 15:  ABCdef");

        dl.setFont (16);
        dl.addText (7.0, 4.0, 0.3, 0.0, "Font 16:  ABCdef");

        dl.setFont (17);
        dl.addText (7.0, 3.0, 0.3, 0.0, "Font 17:  ABCdef");

        dl.setFont (18);
        dl.addText (7.0, 2.0, 0.3, 0.0, "Font 18:  ABCdef");

        dl.setFont (19);
        dl.addText (7.0, 1.0, 0.3, 0.0, "Font 19:  ABCdef");

        logger.info ("    Text Table in a frame finished    ");
    }
};

class Text2TableFrame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();
    
    public Text2TableFrame ()
    {
        super (FRAME_WITHOUT_TOOLBAR);

        setTitle ("Text Fonts 101 - 107");
        setSize (400, 400);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("font 101 - 107 test",
                      0.0, 0.0, 8.0, 8.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("font 101 - 107 frame",
                        0.0,
                        0.0,
                        8.0,
                        8.0,
                        0.0,
                        0.0,
                        8.0,
                        8.0,
                        0.0,
                        0.0,
                        8.0,
                        8.0,
                        1,
                        0,
                        0,
                        DLConst.FRAME_NO_BORDER,
                        null,
                        null,
                        null,
                        DLConst.FRAME_NO_ATTACH,
                        0.0,
                        0.0,
                        0);

        dl.setTextColor (0, 0, 0);
        dl.setTextFillColor (200, 200, 200);

        dl.setFont (101);
        dl.addText (0.0, 7.0, 0.12, 0.0, "Font 101:  ABCdef 01234");

        dl.setFont (102);
        dl.addText (0.0, 6.0, 0.12, 0.0, "Font 102:  ABCdef 01234");

        dl.setFont (103);
        dl.addText (0.0, 5.0, 0.12, 0.0, "Font 103:  ABCdef 01234");

        dl.setFont (104);
        dl.addText (0.0, 4.0, 0.12, 0.0, "Font 104:  ABCdef 01234");

        dl.setFont (105);
        dl.addText (0.0, 3.0, 0.12, 0.0, "Font 105:  ABCdef 01234");

        dl.setFont (106);
        dl.addText (0.0, 2.0, 0.12, 0.0, "Font 106:  ABCdef 01234");

        dl.setFont (107);
        dl.addText (0.0, 1.0, 0.12, 0.0, "Font 107:  ABCdef 01234");

        logger.info ("    Text Fonts 101 - 107 in a frame finished    ");
    }
};

class Text3TableFrame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();
    
    public Text3TableFrame ()
    {
        super (FRAME_WITHOUT_TOOLBAR);

        double dy, upi;

        setTitle ("Default Font");
        setSize (1100, 600);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("default font window",
                      0.0, 0.0, 32.0, 14.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("default font frame",
                        0.0,
                        0.0,
                        32.0,
                        14.0,
                        0.0,
                        0.0,
                        32.0,
                        14.0,
                        0.0,
                        0.0,
                        32.0,
                        14.0,
                        1,
                        0,
                        0,
                        DLConst.FRAME_NO_BORDER,
                        null,
                        null,
                        null,
                        DLConst.FRAME_NO_ATTACH,
                        0.0,
                        0.0,
                        0);

        dl.setTextColor (0, 0, 0);
        dl.setTextFillColor (200, 200, 200);

        dl.addText (0.0, 12.0, 0.05, 0.0, "0.05 inches:  ABCdef 01234");

        upi = 3.0;
        dy = .14 * upi;
        dl.addText (0.0, 12.0 - dy, 0.07, 0.0, "0.07 inches:  ABCdef 01234");

        dy += .2 * upi;
        dl.addText (0.0, 12.0 - dy, 0.1, 0.0, "0.1 inches:  ABCdef 01234");

        dy += .24 * upi;
        dl.addText (0.0, 12.0 - dy, 0.12, 0.0, "0.12 inches:  ABCdef 01234");

        dy += .28 * upi;
        dl.addText (0.0, 12.0 - dy, 0.14, 0.0, "0.14 inches:  ABCdef 01234");

        dy += .32 * upi;
        dl.addText (0.0, 12.0 - dy, 0.16, 0.0, "0.16 inches:  ABCdef 01234");

        dy += .36 * upi;
        dl.addText (0.0, 12.0 - dy, 0.18, 0.0, "0.18 inches:  ABCdef 01234");

        dy += .4 * upi;
        dl.addText (0.0, 12.0 - dy, 0.20, 0.0, "0.20 inches:  ABCdef 01234");

        dy += .5 * upi;
        dl.addText (0.0, 12.0 - dy, 0.25, 0.0, "0.25 inches:  ABCdef 01234");

        dy += .6 * upi;
        dl.addText (0.0, 12.0 - dy, 0.30, 0.0, "0.30 inches:  ABCdef 01234");

        dy += .7 * upi;
        dl.addText (0.0, 12.0 - dy, 0.40, 0.0, "0.40 inches:  ABCdef 01234");

        dy += .8 * upi;
        dl.addText (0.0, 12.0 - dy, 0.50, 0.0, "0.50 inches:  ABCdef 01234");

        logger.info ("    Default Font in a frame finished    ");
    }
};


class SmallerFrame
  extends JDLFrame
  implements DLRightClickListener, DLSelectListener, DLZoomPanListener

{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public void zoomPanChanged (String frameName,
                                double oldX1,
                                double oldY1,
                                double oldX2,
                                double oldY2,
                                double newX1,
                                double newY1,
                                double newX2,
                                double newY2)
    {
    }



    public boolean processRightClick (DLRightClickInfo info)
    {
        info.dump ();
        return true;
    }

    public void selectionChanged (DLSelectionInfo info)
    {
        info.dump ();
        return;
    }


    public SmallerFrame ()
    {
        super ();

        double[] xline, yline;
        int[]    icomp;

        DLSelectable    dls;

//GLPGLP
/*
try {
int idum = System.in.read();
}
catch (Exception e) {
}
*/

        logger.info ("    Random Frame Constructed    ");

        xline = new double[10];
        yline = new double[10];
        icomp = new int[10];

        setTitle ("Random Frame Test");

        JDisplayList dl = super.getDL ();

        dl.addZoomPanListener (this);

        dl.addRightClickListener (this);
        dl.addSelectListener (this);

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("random frame 1 test",
                      0.0, 0.0, 20.0, 20.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("Small frame test",
                        0.0,
                        0.0,
                        200.0,
                        200.0,
                        0.0,
                        0.0,
                        20.0,
                        20.0,
                        0.0,
                        0.0,
                        200.0,
                        200.0,
                        1,
                        0,
                        0,
                        DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                        null,
                        null,
                        null,
                        DLConst.FRAME_NO_ATTACH,
                        0.0,
                        0.0,
                        2);

        dl.addLogComment ("setting up frame axis properties");

        AxisProperties ap = new AxisProperties ();
        ap.setLabelFlag (1);
        ap.setTickDirection (-1);
        ap.setCaption ("Right side caption");
        dl.setFrameAxisProperties ("frame1 test", DLConst.FRAME_RIGHT_AXIS, ap);
        ap.setCaption ("Left side caption");
        dl.setFrameAxisProperties ("frame1 test", DLConst.FRAME_LEFT_AXIS, ap);
        ap.setCaption ("Bottom side caption");
        dl.setFrameAxisProperties ("frame1 test", DLConst.FRAME_BOTTOM_AXIS, ap);
        ap.setCaption ("Top side caption");
        dl.setFrameAxisProperties ("frame1 test", DLConst.FRAME_TOP_AXIS, ap);

        dls = new DLSelectable ();
        dl.setSelectable (dls);
        ap.setMajorInterval (200.0);
        ap.setCaption ("Vertical Axis Test");
//        dl.addAxis (185.0, 5.0, 185.0, 195.0, 1000.0, 0.0, -1, ap);

        dls = new DLSelectable ();
        dl.setSelectable (dls);
        ap.setCaption ("Horizontal Axis Test");
        ap.setLabelAngle (90.0);
        ap.setLabelAnchor (4);
        ap.setMajorInterval (20.0);
//        dl.addAxis (10.0, 120.0, 170.0, 120.0, 0.0, 100.0, -1, ap);

        dls = new DLSelectable ();
        dl.setSelectable (dls);
        ap.setCaption ("Horizontal Axis Test");
        ap.setLabelAngle (-90.0);
        ap.setLabelAnchor (4);
        ap.setMajorInterval (20.0);
        ap.setTickDirection (1);
//        dl.addAxis (10.0, 10.0, 170.0, 10.0, 0.0, 100.0, -1, ap);

        dl.addLogComment ("line in a selectable");
        dls = new DLSelectable ();
        dl.setSelectable (dls);

        xline[0] = 10.0;
        yline[0] = 10.0;
        xline[1] = 90.0;
        yline[1] = 20.0;
        xline[2] = 80.0;
        yline[2] = 80.0;
        xline[3] = 15.0;
        yline[3] = 85.0;
        dl.addLine(xline, yline, 4);

        dl.setLineThickness (0.05);
        dl.setLineSmooth (50);
        dl.addLine(xline, yline, 4);

        dl.setLineSmooth (0);
        dl.setLineThickness (.01);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        xline[0] = 40.0;
        yline[0] = 40.0;
        xline[1] = 60.0;
        yline[1] = 40.0;
        xline[2] = 60.0;
        yline[2] = 50.0;
        xline[3] = 45.0;
        yline[3] = 65.0;
        icomp[0] = 4;

        dl.setFillColor (255, 0, 0);
        dl.setBorderColor (0, 255, 0);
        dl.addFill(xline, yline, icomp, 1, 1);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        xline[0] = 140.0;
        yline[0] = 40.0;
        xline[1] = 160.0;
        yline[1] = 40.0;
        xline[2] = 160.0;
        yline[2] = 50.0;
        xline[3] = 145.0;
        yline[3] = 65.0;
        xline[4] = 140.0;
        yline[4] = 40.0;
        icomp[0] = 5;
        xline[5] = 150.0;
        yline[5] = 50.0;
        xline[6] = 155.0;
        yline[6] = 52.0;
        xline[7] = 153.0;
        yline[7] = 55.0;
        xline[8] = 150.0;
        yline[8] = 50.0;
        icomp[1] = 4;

        dl.setFillColor (255, 200, 0);
        dl.setBorderColor (0, 0, 0);
        dl.addFill(xline, yline, icomp, 2, 1);

        xline[0] = 120.0;
        yline[0] = 20.0;
        xline[1] = 170.0;
        yline[1] = 20.0;
        xline[2] = 170.0;
        yline[2] = 70.0;
        xline[3] = 120.0;
        yline[3] = 70.0;
        icomp[0] = 4;

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillColor (255, 200, 0, 100);
        dl.setBorderColor (0, 0, 0, 100);
        dl.setFillPattern (1, 1.0);
        dl.addFill(xline, yline, icomp, 1, 1);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillPattern (0, 1.0);
        dl.setFillColor (0, 0, 255);
        dl.setBorderColor (255, 0, 0);
        dl.addCircle (155.0, 155.0, 145.0, 145.0, 145.0, 155.0);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillColor (255, 255, 0);
        dl.setBorderColor (0, 0, 0);
        dl.setFillPattern (1, 1.0);
        dl.addCenteredRectangle (100.0, 55.0, 20.0, 14.0, 3.0, 0.0);
        dl.setFillPattern (2, 1.0);
        dl.addCenteredRectangle (120.0, 55.0, 20.0, 14.0, 3.0, 0.0);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillPattern (0, 1.0);
        dl.setFillColor (200, 250, 200);
        dl.setBorderColor (0, 0, 0);
        dl.addArc (80.0, 145.0, 10.0, 7.0, 45.0, 300.0, 0.0, 1);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillColor (-1, -1, -1);
        dl.setFillPattern (3, 1.0);
        dl.setBorderColor (255, 0, 0);
        dl.addArc (80.0, 100.0, 10.0, 7.0, 45.0, 300.0, 90.0, 1);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillPattern (0, 1.0);
        dl.setTextColor (0, 0, 0);
        dl.setFont ("arial", Font.PLAIN);
        dl.addNumber (15.0, 150.0, .15, 90.0, 1.23, 2, 0);
        dl.addNumber (15.0, 10.0, .15, -90.0, 1.23456e15, 5, 0);
        dl.setTextAnchor (3);
        dl.addNumber (15.0, 100.0, .15, 0.0, 123456789, 0, 1);
        dl.setTextAnchor (1);
        dl.addNumber (190.0, 100.0, .15, 0.0, 123456789, 0, 1);

        dl.setSelectable (null);

        dl.setFrameToExtents (null, 0.0, 40.0, 40.0, 40.0);

        dl.unsetFrame ();

    }

};

class Frame2Frame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();
    
    public Frame2Frame ()
    {
        super ();

        double xsym, ysym;
        int i;

        setTitle ("Symbol Examples in Frame");
        setSize (900, 700);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("frame 2 test",
                      0.0, 0.0, 10.0, 10.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("frame2 test",
                        0.0,
                        0.0,
                        100.0,
                        100.0,
                        0.0,
                        0.0,
                        8.0,
                        6.0,
                        0.0,
                        0.0,
                        80.0,
                        60.0,
                        1,
                        0,
                        0,
                        DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                        null,
                        null,
                        null,
                        DLConst.FRAME_NO_ATTACH,
                        0.0,
                        0.0,
                        0);

        dl.setSymbolColor (0, 0, 0);
        dl.setLineThickness (.01);
        dl.setTextOffset (-.2, -.2);

        for (i=0; i<141; i++) {
            xsym = 5.0 * (double)(i%15) + 2.5;
            ysym = 5.0 * (double)(i/15) + 2.5;
            ysym = 60.0 - ysym;
            dl.addSymbol (xsym, ysym, .15, 0.0, i+1);
            dl.addNumber (xsym, ysym, .10, 0.0, (double)(i+1), 0, 0);
        }

        logger.info ("    Symbol Examples in Frame finished");

    }

};

class TextFrameTest extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public TextFrameTest ()
    {
        super ();

        setTitle ("Text Frame Test");
        setSize (900, 600);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("text frame test",
                      0.0, 0.0, 12.0, 12.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("text frame test",
                        0.0,
                        0.0,
                        100.0,
                        100.0,
                        0.0,
                        0.0,
                        10.0,
                        10.0,
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
                        0.0,
                        0);

        dl.setTextColor (0, 0, 0);
        dl.setTextFillColor (200, 200, 200);

        dl.addText (5.0, 100.0, 0.1, 0.0, "0.1 inches:  ABCdef 01234");
        dl.addText (5.0, 90.0, 0.12, 0.0, "0.12 inches:  ABCdef 01234");
        dl.addText (5.0, 80.0, 0.14, 0.0, "0.14 inches:  ABCdef 01234");
        dl.addText (5.0, 70.0, 0.16, 0.0, "0.16 inches:  ABCdef 01234");
        dl.addText (5.0, 60.0, 0.18, 0.0, "0.18 inches:  ABCdef 01234");
        dl.addText (5.0, 50.0, 0.20, 0.0, "0.20 inches:  ABCdef 01234");
        dl.addText (5.0, 40.0, 0.25, 0.0, "0.25 inches:  ABCdef 01234");
        dl.addText (5.0, 30.0, 0.30, 0.0, "0.30 inches:  ABCdef 01234");
        dl.addText (5.0, 20.0, 0.40, 0.0, "0.40 inches:  ABCdef 01234");
        dl.addText (5.0, 10.0, 0.50, 0.0, "0.50 inches:  ABCdef 01234");

        logger.info ("    Text Frame Test finished");
    }

};

class FrameLayoutTest extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public FrameLayoutTest ()
    {
        super ();

        double     angle;

        setTitle ("Frame Layout Test");
        setSize (900, 600);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("frame layout test",
                      0.0, 0.0, 15.0, 12.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("base frame",
                        0.0,
                        0.0,
                        100.0,
                        100.0,
                        0.0,
                        0.0,
                        10.0,
                        10.0,
                        8.0,
                        7.0,
                        63.0,
                        87.0,
                        1,
                        0,
                        DLConst.ALLOW_ANY_FRAME_ASPECT_RATIO,
                        DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                        "Horizontal Caption",
                        "Vertical Caption",
                        null,
                        DLConst.FRAME_NO_ATTACH,
                        0.0,
                        0.0,
                        0);

        double[] xf = new double[10];
        double[] yf = new double[10];
        int[] ic = new int[10];

        xf[0] = 20.0;
        yf[0] = 20.0;
        xf[1] = 80.0;
        yf[1] = 30.0;
        xf[2] = 70.0;
        yf[2] = 78.0;
        xf[3] = 32.0;
        yf[3] = 65.0;
        ic[0] = 4;
        dl.setFillColor (255, 0, 0);
        dl.addFill (xf, yf, ic, 1, 1);

        dl.addSymbol (25.0, 15.0, .2, 0.0, 7);

        dl.setLineThickness (0.005);
        dl.setTextColor (0, 0, 0);
        dl.setTextBackground (DLConst.TEXT_BG_FILLED_BORDER);
        dl.setFillColor (200, 200, 200);

        angle = 30.0;

        dl.setTextOffset (.2, .2);
        dl.setTextAnchor (DLConst.TEXT_BOTTOM_LEFT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 1");

        dl.setTextOffset (-.2, .2);
        dl.setTextAnchor (DLConst.TEXT_BOTTOM_RIGHT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 3");

        dl.setTextOffset (.2, -.2);
        dl.setTextAnchor (DLConst.TEXT_TOP_LEFT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 7");

        dl.setTextOffset (-.2, -.2);
        dl.setTextAnchor (DLConst.TEXT_TOP_RIGHT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 9");

        dl.addSymbol (50.0, 50.0, .2, 0.0, 7);
        dl.setTextBackground (DLConst.TEXT_BG_CURVE_FILLED);

        dl.setTextOffset (0.0, .2);
        dl.setTextAnchor (DLConst.TEXT_BOTTOM_CENTER);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 2");

        dl.setTextOffset (0.0, -.2);
        dl.setTextAnchor (DLConst.TEXT_TOP_CENTER);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 8");

        dl.setTextOffset (.2, 0.0);
        dl.setTextAnchor (DLConst.TEXT_CENTER_LEFT);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 4");

        dl.setTextOffset (-.2, 0.0);
        dl.setTextAnchor (DLConst.TEXT_CENTER_RIGHT);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 6");

        dl.addSymbol (80.0, 80.0, .2, 0.0, 7);
        dl.setTextBackground (DLConst.TEXT_BG_CURVE_FILLED_BORDER);

        dl.setTextOffset (0.0, 0.0);
        dl.setTextAnchor (DLConst.TEXT_CENTER_CENTER);
        dl.addText (80.0, 80.0, 0.15, 0.0, "Anchor jpg 5");
        dl.addSymbol (80.0, 80.0, 3.0, 0.0, 7);

        dl.setTextAnchor (DLConst.TEXT_BOTTOM_LEFT);
        dl.createAttachedFrame ("attached 1",
                                 4.0,
                                 8.0,
                                 "base frame",
                                 DLConst.FRAME_ATTACH_RIGHT_MIN,
                                 1,
                                 0.0,
                                 0.0,
                                 DLConst.PAGE_UNITS,
                                 DLConst.PAGE_UNITS,
                                 DLConst.PAGE_UNITS,
                                 DLConst.PAGE_UNITS
                                );

        dl.addText (0.1, 0.3, 0.20, 0.0, "ATTACH 1 FRAME");

        dl.setTextOffset (-.1, 0.0);
        dl.setTextBackground (DLConst.TEXT_BG_NONE);
        dl.setTextAnchor (DLConst.TEXT_BOTTOM_RIGHT);

        dl.addText (3.5, 1.3, 0.15, 0.0, "first right justified line");

        dl.addText (3.5, 1.6, 0.15, 0.0, "another right just line");

        dl.addText (3.5, 1.9, 0.15, 0.0, "3rd rjust line");

        dl.addText (3.5, 2.2, 0.15, 0.0, "line spelled a lot DIFFERENTLY#$%^&*");

        dl.addText (3.5, 2.5, 0.15, 0.0, "No trailing blanks?      ");

        dl.addText (3.5, 2.8, 0.15, 0.0, "blanks        A!!");

        dl.addText (3.5, 3.1, 0.15, 0.0, "-1234567.89");

        dl.setTextOffset (0.0, 0.0);

        xf[0] = 3.5;
        yf[0] = 1.0;
        xf[1] = 3.5;
        yf[1] = 4.0;
        dl.addLine (xf, yf, 2);

        dl.setTextAnchor (DLConst.TEXT_BOTTOM_LEFT);
        dl.setTextBackground (DLConst.TEXT_BG_CURVE_FILLED_BORDER);

        logger.info ("    Frame Layout Test Finished");
    }

};


class ReversePrimFrame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public ReversePrimFrame ()
    {
        super ();

        double[] xline, yline;
        int[]    icomp;
 
        xline = new double[10];
        yline = new double[10];
        icomp = new int[10];

        setTitle ("Reverse Axes Test");
        setSize (700, 700);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("reverse axis test",
                      0.0, 0.0, 200.0, 200.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("reverse test",
                        0.0,
                        0.0,
                        200.0,
                        200.0,
                        50.0,
                        50.0,
                        150.0,
                        150.0,
                        200.0,
                        200.0,
                        0.0,
                        0.0,
                        1,
                        0,
                        0,
                        DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                        null,
                        null,
                        null,
                        DLConst.FRAME_NO_ATTACH,
                        0.0,
                        0.0,
                        0);

        xline[0] = 10.0;
        yline[0] = 10.0;
        xline[1] = 90.0;
        yline[1] = 20.0;
        xline[2] = 80.0;
        yline[2] = 80.0;
        xline[3] = 15.0;
        yline[3] = 85.0;
        dl.addLine(xline, yline, 4);

        dl.setLineThickness (0.05);
        dl.setLineSmooth (50);
        dl.addLine(xline, yline, 4);

        dl.setLineSmooth (0);
        dl.setLineThickness (.01);

        xline[0] = 40.0;
        yline[0] = 40.0;
        xline[1] = 60.0;
        yline[1] = 40.0;
        xline[2] = 60.0;
        yline[2] = 50.0;
        xline[3] = 45.0;
        yline[3] = 65.0;
        icomp[0] = 4;

        dl.setFillColor (255, 0, 0);
        dl.setBorderColor (0, 255, 0);
        dl.addFill(xline, yline, icomp, 1, 1);

        xline[0] = 140.0;
        yline[0] = 40.0;
        xline[1] = 160.0;
        yline[1] = 40.0;
        xline[2] = 160.0;
        yline[2] = 50.0;
        xline[3] = 145.0;
        yline[3] = 65.0;
        xline[4] = 140.0;
        yline[4] = 40.0;
        icomp[0] = 5;
        xline[5] = 150.0;
        yline[5] = 50.0;
        xline[6] = 155.0;
        yline[6] = 52.0;
        xline[7] = 153.0;
        yline[7] = 55.0;
        xline[8] = 150.0;
        yline[8] = 50.0;
        icomp[1] = 4;

        dl.setFillColor (255, 200, 0);
        dl.setBorderColor (0, 0, 0);
        dl.addFill(xline, yline, icomp, 2, 1);

        dl.setFillColor (0, 0, 255);
        dl.setBorderColor (255, 0, 0);
        dl.addCircle (155.0, 155.0, 145.0, 145.0, 145.0, 155.0);

        dl.setFillColor (-1, -1, -1);
        dl.setBorderColor (0, 0, 0);
        dl.addCenteredRectangle (100.0, 55.0, 20.0, 14.0, 3.0, 45.0);

        dl.setFillColor (200, 250, 200);
        dl.setBorderColor (0, 0, 0);
        dl.addArc (80.0, 145.0, 10.0, 7.0, 45.0, 300.0, 0.0, 1);

        dl.setFillColor (-1, -1, -1);
        dl.setBorderColor (255, 0, 0);
        dl.addArc (80.0, 100.0, 10.0, 7.0, 45.0, 300.0, 90.0, 1);

        dl.unsetFrame ();

        logger.info ("    Reverse Axes Test Finished");

    }

};

class DashedLineFrame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public DashedLineFrame ()
    {
        super (FRAME_WITHOUT_TOOLBAR);

        double[] x, y;
        int i, patnum;

        setTitle ("Dashed Line Examples, dashscale = 1.0");
        setSize (900, 700);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("dashed line test",
                      0.0, 0.0, 10.0, 12.0);
        dl.setColor (5, 5, 5);

        dl.setLineThickness (.04);
        dl.setTextOffset (0.0, .1);
        x = new double[2];
        y = new double[2];

        for (i=0; i<30; i++) {
            x[0] = (double)(i / 10) * 3.0;
            y[0] = 10.5 - (double)(i % 10);
            x[1] = x[0] + 2.5;
            y[1] = y[0];
            patnum = i + 1;
            dl.setLinePattern (patnum, 1.0);
            dl.addLine (x, y, 2);

            String str = "pattern # ";
            str = str.concat (String.valueOf (patnum));
            dl.addText (x[0], y[0], 0.12, 0.0, str);
        }

        logger.info ("   Dashed Line Examples scale 1.0 finished"); 

    }
};

class Dashed2LineFrame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public Dashed2LineFrame ()
    {
        super (FRAME_WITHOUT_TOOLBAR);

        double[] x, y;
        int i, patnum;

        setTitle ("Dashed Line Examples, dashscale = 2.0");
        setSize (900, 700);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("dashed line test 2",
                      0.0, 0.0, 10.0, 12.0);
        dl.setColor (5, 5, 5);

        dl.setLineThickness (.04);
        dl.setTextOffset (0.0, .1);
        x = new double[2];
        y = new double[2];

        for (i=0; i<30; i++) {
            x[0] = (double)(i / 10) * 3.0;
            y[0] = 10.5 - (double)(i % 10);
            x[1] = x[0] + 2.5;
            y[1] = y[0];
            patnum = i + 1;
            dl.setLinePattern (patnum, 2.0);
            dl.addLine (x, y, 2);

            String str = "pattern # ";
            str = str.concat (String.valueOf (patnum));
            dl.addText (x[0], y[0], 0.12, 0.0, str);
        }

        logger.info ("   Dashed Line Examples scale 1.0 finished"); 

    }
};

class GridTest extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public GridTest ()
    {
        super ();

        double[] gdata;
        double   dx, dy, dist;
        int      i, j, k, jstart;

        setTitle ("Grid Test");
        setSize (900, 700);

        JDisplayList dl = super.getDL ();

        dl.beginPlot ("grid test",
                      0.0, 0.0, 100.0, 100.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("grid test frame",
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
                       0.0,
                        0);

        gdata = new double[100];
        for (i=0; i<10; i++) {
            jstart = i * 10;
            dy = i * 10.0;
            for (j=0; j<10; j++) {
                k = jstart + j;
                dx = j * 10.0;
                dist = dx * dx + dy * dy;
                dist = Math.sqrt (dist);
                gdata[k] = dist;
            }
        }
        gdata[55] = 1.e30;
        gdata[45] = 1.e30;

        Grid grid = new Grid (
            gdata,
            10,
            10,
            5.0,
            5.0,
            90.0,
            90.0,
            0.0,
            null
        );

        DLSurfaceProperties dlp = new DLSurfaceProperties ();
        dlp.setShowNodes (true);
        ColorPalette cpal = new ColorPalette();
        dl.setImageColors (cpal, 0.0, 130.0);
        dl.addGrid ("test grid 10",
                    grid,
                    dlp);

        logger.info ("    GridTest Finished");

    }
};




/*
 * Class for testing HugeFrame stuff in separate threads.
 */
class HFWorker extends SwingWorker<Integer, Void> {

    HugeFrame    hf = null;

    HFWorker (HugeFrame  hfin)
    {
         hf = hfin;
    }

    protected Integer doInBackground () {
        hf.populateDlist ();
        return 0;
    }

    protected void done () {
        hf.setVisible (true);
    }

}



/**
 * This class has a runnable that first populates the specified
 * HugeFrame object.  After this object has been populated, the
 * frame is made visible, which triggers the drawing of the objects
 * previously set via the populateDlist method.
 */
class HugeFrameRunnable implements Runnable {

    HugeFrame    hf;

    public HugeFrameRunnable (HugeFrame hfin) {hf = hfin;}

    public void run () {

//  For now, always use threads here
//  When the surfaceworks thread stuff is working,
//  I will use the environment variable for turning
//  threads on and off both here and surfaceworks

      String  tflag = System.getenv ("CSW_DONT_USE_THREADS");

      if (tflag == null  ||  tflag.isEmpty()) {
        HFWorker hfw = new HFWorker (hf);
        hfw.execute ();
      }
      else {
System.out.println ("JEZX not using threads");
System.out.flush ();
        hf.populateDlist ();
        hf.setVisible (true);
      }

    }

}



class HugeFrame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    private int  num_hint = 50;


    public HugeFrame (int nhint)
    {
        super ();
        num_hint = nhint;
    }

    public int getNumHint () {return num_hint;}

    public void populateDlist ()
    {

        int      nhint = num_hint;

        double[] xline, yline;
        int[]    icomp;
        Random   random;
        double   xr, yr;
        int      ndo, idomod;
        double   xymax;

/*
 *  Sometimes the native C/C++ code needs to be debugged while
 *  being run under the Java Virtual Machine.  In my (Glenn) 
 *  experience, this is not always possible (maybe half the
 *  time?).  But, if it is possible, the JVM needs to be waiting
 *  on something like this keyboard input.  During the wait, 
 *  gdb can be started in another window and attached to the JVM
 *  process.
try {
System.out.println();
System.out.println("press return after attaching gdb");
System.out.println();
System.out.flush();
int idum = System.in.read();
}
catch (Exception e) {
}
*/

        DLSelectable dls = null;

        xline = new double[100];
        yline = new double[100];
        icomp = new int[10];

        if (nhint <= 0) {
          ndo = 10000;
          xymax = 10000.0;
          setTitle ("Huge Random Frame Test * " + ndo);
        }
        else {
          ndo = nhint;
          xymax = 2000.0;
          setTitle ("Random Frame Test * " + ndo);
        }

        double prat = (double)nhint / 2000.0;
        if (prat < 1.0) prat = 1.0;
        if (prat > 2.0) prat = 2.0;
        if (nhint == -1) {
            prat = 2.0;
        }

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("huge frame 1 test",
                      0.0, 0.0, 40.0 * prat, 40.0 * prat);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("huge frame1 test",
                        0.0,
                        0.0,
                        xymax,
                        xymax);

        AxisProperties ap = new AxisProperties ();
        ap.setLabelFlag (1);
        ap.setTickDirection (-1);
        ap.setCaption ("Right side caption");
        dl.setFrameAxisProperties ("huge frame1 test", DLConst.FRAME_RIGHT_AXIS, ap);
        ap.setCaption ("Left side caption");
        dl.setFrameAxisProperties ("huge frame1 test", DLConst.FRAME_LEFT_AXIS, ap);
        ap.setCaption ("Bottom side caption");
        dl.setFrameAxisProperties ("huge frame1 test", DLConst.FRAME_BOTTOM_AXIS, ap);
        ap.setCaption ("Top side caption");
        dl.setFrameAxisProperties ("huge frame1 test", DLConst.FRAME_TOP_AXIS, ap);

        random = new Random();
        long seed = 1234579;
        random.setSeed (seed);

        double xyrand = xymax * 1.2;
        double xyrand_shift = xymax / 10.0;

        double tfact = 1.0;

        double shfact = 4;
        if (ndo > 20) shfact = 2;
        if (ndo > 50) shfact = 1;
        if (ndo > 100) shfact = .5;

idomod = ndo / 10;
if (idomod < 100) idomod = 100;

        for (int ido=0; ido<ndo; ido++) {

if (ido % idomod == 0  &&  ido != 0) {
//System.out.println ("in populate loop, ido = " + ido);
//System.out.println ();
//System.out.flush ();
}

            xr = random.nextDouble () * xyrand - xyrand_shift;
            yr = random.nextDouble () * xyrand - xyrand_shift;

            dls = new DLSelectable ();
            dl.setSelectable (dls);

            xline[0] = 10.0 + xr;
            yline[0] = 10.0 + yr;
            xline[1] = 90.0 + xr;
            yline[1] = 20.0 + yr;
            xline[2] = 80.0 + xr;
            yline[2] = 80.0 + yr;
            xline[3] = 15.0 + xr;
            yline[3] = 85.0 + yr;
            dl.addLine(xline, yline, 4);

            xr *= .9;
            yr *= .9;

            xline[0] = 10.0 + xr;
            yline[0] = 10.0 + yr;
            xline[1] = 90.0 + xr;
            yline[1] = 20.0 + yr;
            xline[2] = 80.0 + xr;
            yline[2] = 80.0 + yr;
            xline[3] = 15.0 + xr;
            yline[3] = 85.0 + yr;
            dl.addLine(xline, yline, 4);

            xline[0] = 10.0 + xr;
            yline[0] = -10.0 + yr;
            xline[1] = 90.0 + xr;
            yline[1] = -20.0 + yr;
            xline[2] = 80.0 + xr;
            yline[2] = -80.0 + yr;
            xline[3] = 15.0 + xr;
            yline[3] = -85.0 + yr;

            xr /= .9;
            yr /= .9;

            dl.setLineThickness (0.05);
            dl.setLineSmooth (50);
            dl.addLine(xline, yline, 4);

            dls = new DLSelectable ();
            dl.setSelectable (dls);

            dl.setLineSmooth (0);
            dl.setLineThickness (.01);

            xr = random.nextDouble () * xyrand - 2.0 * xyrand_shift;
            yr = random.nextDouble () * xyrand - 2.0 * xyrand_shift;

            xline[0] = 40.0 + xr;
            yline[0] = 40.0 + yr;
            xline[1] = 60.0 + xr;
            yline[1] = 40.0 + yr;
            xline[2] = 60.0 + xr;
            yline[2] = 50.0 + yr;
            xline[3] = 45.0 + xr;
            yline[3] = 65.0 + yr;
            icomp[0] = 4;

            dl.setFillPattern (0, 1.0);
            dl.setFillColor (255, 0, 0);
            dl.setBorderColor (0, 255, 0);
            dl.addFill(xline, yline, icomp, 1, 1);

            dls = new DLSelectable ();
            dl.setSelectable (dls);

            xline[0] = 140.0 + xr;
            yline[0] = 40.0 + yr;
            xline[1] = 160.0 + xr;
            yline[1] = 40.0 + yr;
            xline[2] = 160.0 + xr;
            yline[2] = 50.0 + yr;
            xline[3] = 145.0 + xr;
            yline[3] = 65.0 + yr;
            xline[4] = 140.0 + xr;
            yline[4] = 40.0 + yr;
            icomp[0] = 5;
            xline[5] = 150.0 + xr;
            yline[5] = 50.0 + yr;
            xline[6] = 155.0 + xr;
            yline[6] = 52.0 + yr;
            xline[7] = 153.0 + xr;
            yline[7] = 55.0 + yr;
            xline[8] = 150.0 + xr;
            yline[8] = 50.0 + yr;
            icomp[1] = 4;

            dl.setFillColor (0, 0, 200);
            dl.setBorderColor (200, 0, 0);
            dl.addFill(xline, yline, icomp, 2, 1);

            dls = new DLSelectable ();
            dl.setSelectable (dls);

            xline[0] = 120.0 + xr;
            yline[0] = 20.0 + yr;
            xline[1] = 170.0 + xr;
            yline[1] = 20.0 + yr;
            xline[2] = 170.0 + xr;
            yline[2] = 70.0 + yr;
            xline[3] = 120.0 + xr;
            yline[3] = 70.0 + yr;
            icomp[0] = 4;

            dl.setFillPattern (5, 1.0);
            dl.setFillPatternColor (0, 0, 0);
            dl.setFillColor (255, 200, 0, 100);
            dl.setBorderColor (0, 0, 0, 100);
            dl.addFill(xline, yline, icomp, 1, 1);

            xr = random.nextDouble () * xyrand - xyrand_shift;
            yr = random.nextDouble () * xyrand - xyrand_shift;

            dls = new DLSelectable ();
            dl.setSelectable (dls);

            dl.setFillColor (0, 0, 255);
            dl.setBorderColor (255, 0, 0);
            dl.addCircle (shfact * 155.0 + xr, shfact * 155.0 + yr,
                          shfact * 145.0 + xr, shfact * 145.0 + yr,
                          shfact * 145.0 + xr, shfact * 155.0 + yr);

            dls = new DLSelectable ();
            dl.setSelectable (dls);

            dl.setFillColor (-1, -1, -1);
            dl.setBorderColor (0, 0, 0);
            dl.addCenteredRectangle (shfact * 100.0 + xr, shfact * 55.0 + yr,
                                     shfact * 20.0, shfact * 14.0,
                                     shfact * 3.0, shfact * 45.0);

            dls = new DLSelectable ();
            dl.setSelectable (dls);

            dl.setFillColor (200, 250, 200);
            dl.setBorderColor (0, 0, 0);
            dl.addArc (shfact * 80.0 + xr, shfact * 145.0 + yr,
                       shfact * 10.0, shfact * 7.0, 45.0, 300.0, 0.0, 1);

            dls = new DLSelectable ();
            dl.setSelectable (dls);

            dl.setFillColor (-1, -1, -1);
            dl.setBorderColor (255, 0, 0);
            dl.addArc (shfact * 80.0 + xr, shfact * -100.0 + yr,
                       shfact * 10.0, shfact * 7.0, 45.0, 300.0, 90.0, 1);

            dls = new DLSelectable ();
            dl.setSelectable (dls);

            dl.setTextColor (0, 0, 0);
            dl.setFont ("arial", Font.ITALIC);
            dl.addNumber (15.0 + xr, 150.0 + yr, .15 * tfact, 0.0, 1.23, 2, 0);
            dl.addNumber (15.0 + xr, 170.0 + yr, .15 * tfact, 0.0, 1.23456e15, 5, 0);
            dl.addNumber (15.0 + xr, 190.0 + yr, .15 * tfact, 0.0, 123456789, 0, 1);

            dl.setFont ("arial", Font.PLAIN);
            dl.setTextColor (0, 225, 0);
            dl.addText (xr + 20, yr + 220, .2 * tfact, 0.0, "Text number: " + ido);


            dls = new DLSelectable ();
            dl.setSelectable (dls);

            dl.setSymbolColor (255, 0, 0);
            dl.addSymbol (xr, yr, .20, 0.0, ido%140 + 1);

            xr = random.nextDouble () * xyrand - xyrand_shift;
            yr = random.nextDouble () * xyrand - xyrand_shift;

            dl.setSymbolColor (0, 255, 0);
            dl.addSymbol (xr, yr, .30, 0.0, ido%130 + 1);

            xr = random.nextDouble () * xyrand - xyrand_shift;
            yr = random.nextDouble () * xyrand - xyrand_shift;

            dl.setSymbolColor (0, 0, 255);
            dl.addSymbol (xr, yr, .40, 0.0, ido%75 + 25);

            dl.setSelectable (null);
        }

        dl.unsetFrame ();

        logger.info ("    Huge Random Frame Test Finished");

    }

};




class LithFrame extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public LithFrame ()
    {
        super ();

        int i;

        setTitle ("Lith Pattern Examples");
        setSize (900, 700);
        int mask = DLConst.ZOOM_IN_BUTTON_MASK |
                   DLConst.ZOOM_OUT_BUTTON_MASK |
                   DLConst.ZOOM_BUTTON_MASK |
                   DLConst.ZOOM_FULL_BUTTON_MASK;

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("lith pat test",
                      0.0, 0.0, 10.0, 12.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("lith patt frame",
                        -2.0,
                        0.0,
                        18.0,
                        12.0,
                        -2.0,
                        0.0,
                        18.0,
                        12.0,
                        -2.0,
                        0.0,
                        18.0,
                        12.0,
                        1,
                        0,
                        0,
                        DLConst.FRAME_PLAIN_BORDER,
                        null,
                        null,
                        null,
                        DLConst.FRAME_NO_ATTACH,
                        0.0,
                        0.0,
                        0);

        dl.setTextAnchor (2);
        dl.setLineThickness (.01);
        dl.setTextOffset (0.0, .1);
        dl.setFont (105);

        dl.addText (1.5, 10.0, 0.10, 0.0, "SANDSTONE");
        dl.addText (3.5, 10.0, 0.10, 0.0, "SILTSTONE");
        dl.addText (5.5, 10.0, 0.10, 0.0, "SHALE");
        dl.addText (7.5, 10.0, 0.10, 0.0, "LIMESTONE");
        dl.addText (9.5, 10.0, 0.10, 0.0, "DOLOMITE");
        dl.addText (11.5, 10.0, 0.10, 0.0, "EVAPORITE");
        dl.addText (13.5, 10.0, 0.10, 0.0, "COAL");
        dl.addText (15.5, 10.0, 0.10, 0.0, "IGNEOUS");

        dl.setTextOffset (0.0, 0.0);
        dl.setTextAnchor (6);

        double       xc, yc, w, h, cr;

        yc = 9.0;
        w = 1.5;
        h = 1.25;
        cr = 0.15;

        dl.addText (0.0, yc, 0.10, 0.0, "1.0");
        for (i=0; i<8; i++) {
            dl.setLithFill (i, 255, 255);
            xc = i * 2 + 1.5;
            dl.addCenteredRectangle (
                xc, yc,
                w, h,
                cr, 0.0);
        }

        yc = 7.0;
        dl.addText (0.0, yc, 0.10, 0.0, "1.5");
        for (i=0; i<8; i++) {
            dl.setLithFill (i, 1.5, 255, 255);
            xc = i * 2 + 1.5;
            dl.addCenteredRectangle (
                xc, yc,
                w, h,
                cr, 0.0);
        }

        yc = 5.0;
        dl.addText (0.0, yc, 0.10, 0.0, "2.0");
        for (i=0; i<8; i++) {
            dl.setLithFill (i, 2.0, 255, 255);
            xc = i * 2 + 1.5;
            dl.addCenteredRectangle (
                xc, yc,
                w, h,
                cr, 0.0);
        }

        yc = 3.0;
        dl.addText (0.0, yc, 0.10, 0.0, "a=50");
        for (i=0; i<8; i++) {
            dl.setLithFill (i, 25, 50);
            xc = i * 2 + 1.5;
            dl.addCenteredRectangle (
                xc, yc,
                w, h,
                cr, 0.0);
        }

        yc = 1.0;
        dl.addText (0.0, yc, 0.10, 0.0, "a=100");
        for (i=0; i<8; i++) {
            dl.setLithFill (i, 50, 100);
            xc = i * 2 + 1.5;
            dl.addCenteredRectangle (
                xc, yc,
                w, h,
                cr, 0.0);
        }

        logger.info ("    Lith Pattern Examples Finished");

    }
};



class FontFrame extends JFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger logger = CSWLogger.getMyLogger ();

    public FontFrame ()
    {
  
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        setTitle ("Java Font Test");
        setSize (900, 700);
        setResizable (true);
        LocalPanel panel = new LocalPanel ();
        Container contentPane = getContentPane ();
        contentPane.add (panel);

        logger.info ("    Java Font Test Finished");

    }

    private static class LocalPanel extends JPanel
    {

        private static final long serialVersionUID = 1L;

        private static Logger  logger = CSWLogger.getMyLogger ();

        public void paintComponent(Graphics gin)
        {
            Graphics2D   g2d;

            g2d = (Graphics2D)gin;

            g2d.setPaint (getBackground());
            g2d.fillRect (0, 0, getWidth(), getHeight());

            Toolkit tk = Toolkit.getDefaultToolkit ();
            int dpi = tk.getScreenResolution ();

            int size = dpi / 10 + 1;

            String str = "test";
            String str2 = "Flipped test";

        /*
         * Create a properly sized font and get the rectangular
         * bounds of the string using this font.
         */
            Font font = new Font ("Serif",
                                  Font.PLAIN,
                                  2*size);

            g2d.setPaint (Color.blue);
            g2d.setFont (font);
            g2d.drawString (str, 100, 100);

            FontRenderContext frc =
            new FontRenderContext (
                new AffineTransform(),
                false,
                false);

            Rectangle2D bounds =
            font.getStringBounds (str2, frc);

            float ascent = font.getLineMetrics(str2, frc).getAscent();

            int height = (int)(bounds.getHeight() + .5);
            int width = (int)(bounds.getWidth() + .5);

    /*
     * Create a buffered image the size of the string bounds
     * and with a format suitable for handing off to the opengl
     * drawPixels function.
     */
            WritableRaster raster =
            Raster.createInterleavedRaster (
                DataBuffer.TYPE_BYTE,
                width, height,
                4,
                null);

            ComponentColorModel cmod =
            new ComponentColorModel (
                ColorSpace.getInstance(ColorSpace.CS_sRGB),
                new int[] {8, 8, 8, 8},
                true,
                false,
                ComponentColorModel.TRANSLUCENT,
                DataBuffer.TYPE_BYTE);

            BufferedImage bi =
            new BufferedImage (cmod,
                               raster,
                               false,
                               null);

    /*
     * Draw the string into the image, flipping it in y
     * since opengl starts in the lower left.
     */
            Graphics2D g = bi.createGraphics ();

            //g.setPaint (new Color (255, 255, 255, 0));
            //g.fillRect (0, 0, width, height);

            AffineTransform gt = new AffineTransform ();
            gt.translate (0.0, (double)height);
            gt.scale (1.0, -1.0);
            g.transform (gt);

            g.setFont (font);
            g.setPaint (Color.black);
            g.drawString (str2, 0, ascent);

            g2d.drawImage (bi, null, 100, 300);

            logger.info ("    LocalPanel paint component finished");
        }

    }  // end of private LocalPanel class
};




class FrameSyncTest extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public FrameSyncTest ()
    {
        super ();

        double     angle;

        setTitle ("Frame Sync Test");
        setSize (900, 600);

        JDisplayList dl = super.getDL ();

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("frame sync test",
                      0.0, 0.0, 15.0, 12.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);

        dl.createFrame ("base frame",
                        0.0,
                        0.0,
                        100.0,
                        100.0,
                        0.0,
                        0.0,
                        10.0,
                        10.0,
                        0.0,
                        0.0,
                        99.0,
                        99.0,
                        1,
                        0,
                        0,
                        DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                        "Horizontal Caption",
                        "Vertical Caption",
                        null,
                        DLConst.FRAME_NO_ATTACH,
                        0.0,
                        0.0,
                        0);

        double[] xf = new double[10];
        double[] yf = new double[10];
        int[] ic = new int[10];

        xf[0] = 20.0;
        yf[0] = 20.0;
        xf[1] = 80.0;
        yf[1] = 30.0;
        xf[2] = 70.0;
        yf[2] = 78.0;
        xf[3] = 32.0;
        yf[3] = 65.0;
        ic[0] = 4;
        dl.setFillColor (255, 0, 0);
        dl.addFill (xf, yf, ic, 1, 1);

        dl.addSymbol (25.0, 15.0, .2, 0.0, 7);

        dl.setLineThickness (0.005);
        dl.setTextColor (0, 0, 0);
        dl.setTextBackground (DLConst.TEXT_BG_FILLED_BORDER);
        dl.setFillColor (200, 200, 200);

        angle = 30.0;

        dl.setTextOffset (.2, .2);
        dl.setTextAnchor (DLConst.TEXT_BOTTOM_LEFT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 1");

        dl.setTextOffset (-.2, .2);
        dl.setTextAnchor (DLConst.TEXT_BOTTOM_RIGHT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 3");

        dl.setTextOffset (.2, -.2);
        dl.setTextAnchor (DLConst.TEXT_TOP_LEFT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 7");

        dl.setTextOffset (-.2, -.2);
        dl.setTextAnchor (DLConst.TEXT_TOP_RIGHT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 9");

        dl.addSymbol (50.0, 50.0, .2, 0.0, 7);
        dl.setTextBackground (DLConst.TEXT_BG_CURVE_FILLED);

        dl.setTextOffset (0.0, .2);
        dl.setTextAnchor (DLConst.TEXT_BOTTOM_CENTER);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 2");

        dl.setTextOffset (0.0, -.2);
        dl.setTextAnchor (DLConst.TEXT_TOP_CENTER);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 8");

        dl.setTextOffset (.2, 0.0);
        dl.setTextAnchor (DLConst.TEXT_CENTER_LEFT);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 4");

        dl.setTextOffset (-.2, 0.0);
        dl.setTextAnchor (DLConst.TEXT_CENTER_RIGHT);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 6");

        dl.addSymbol (80.0, 80.0, .2, 0.0, 7);
        dl.setTextBackground (DLConst.TEXT_BG_CURVE_FILLED_BORDER);

        dl.setTextOffset (0.0, 0.0);
        dl.setTextAnchor (DLConst.TEXT_CENTER_CENTER);
        dl.addText (80.0, 80.0, 0.15, 0.0, "Anchor jpg 5");
        dl.setSymbolColor (200, 0, 0);
        dl.addSymbol (80.0, 80.0, 3.0, 0.0, 7);

        dl.setTextAnchor (DLConst.TEXT_BOTTOM_LEFT);


        dl.createFrame ("top frame",
                        0.0,
                        0.0,
                        100.0,
                        100.0,
                        0.0,
                        0.0,
                        10.0,
                        10.0,
                        0.0,
                        0.0,
                        99.0,
                        99.0,
                        1,
                        0,
                        0,
                        DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                        "Horizontal Caption",
                        "Vertical Caption",
                        "base frame",
                        DLConst.FRAME_ATTACH_RIGHT_MIDDLE,
                        0.5,
                        0.0,
                        true, true, 0);

        xf = new double[10];
        yf = new double[10];
        ic = new int[10];

        xf[0] = 20.0;
        yf[0] = 20.0;
        xf[1] = 80.0;
        yf[1] = 30.0;
        xf[2] = 70.0;
        yf[2] = 78.0;
        xf[3] = 32.0;
        yf[3] = 65.0;
        ic[0] = 4;
        dl.setFillColor (255, 0, 0);
        dl.addFill (xf, yf, ic, 1, 1);

        dl.addSymbol (25.0, 15.0, .2, 0.0, 7);

        dl.setLineThickness (0.005);
        dl.setTextColor (0, 0, 0);
        dl.setTextBackground (DLConst.TEXT_BG_FILLED_BORDER);
        dl.setFillColor (200, 200, 200);

        angle = 30.0;

        dl.setTextOffset (.2, .2);
        dl.setTextAnchor (DLConst.TEXT_BOTTOM_LEFT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 1");

        dl.setTextOffset (-.2, .2);
        dl.setTextAnchor (DLConst.TEXT_BOTTOM_RIGHT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 3");

        dl.setTextOffset (.2, -.2);
        dl.setTextAnchor (DLConst.TEXT_TOP_LEFT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 7");

        dl.setTextOffset (-.2, -.2);
        dl.setTextAnchor (DLConst.TEXT_TOP_RIGHT);
        dl.addText (25.0, 15.0, 0.15, angle, "Anchor jpg 9");

        dl.addSymbol (50.0, 50.0, .2, 0.0, 7);
        dl.setTextBackground (DLConst.TEXT_BG_CURVE_FILLED);

        dl.setTextOffset (0.0, .2);
        dl.setTextAnchor (DLConst.TEXT_BOTTOM_CENTER);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 2");

        dl.setTextOffset (0.0, -.2);
        dl.setTextAnchor (DLConst.TEXT_TOP_CENTER);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 8");

        dl.setTextOffset (.2, 0.0);
        dl.setTextAnchor (DLConst.TEXT_CENTER_LEFT);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 4");

        dl.setTextOffset (-.2, 0.0);
        dl.setTextAnchor (DLConst.TEXT_CENTER_RIGHT);
        dl.addText (50.0, 50.0, 0.15, angle, "Anchor jpg 6");

        dl.addSymbol (80.0, 80.0, .2, 0.0, 7);
        dl.setTextBackground (DLConst.TEXT_BG_CURVE_FILLED_BORDER);

        dl.setTextOffset (0.0, 0.0);
        dl.setTextAnchor (DLConst.TEXT_CENTER_CENTER);
        dl.addText (80.0, 80.0, 0.15, 0.0, "Anchor jpg 5");
        dl.setSymbolColor (0, 200, 0);
        dl.setLineThickness (.02);
        dl.addSymbol (80.0, 80.0, 3.0, 0.0, 7);

        dl.setTextAnchor (DLConst.TEXT_BOTTOM_LEFT);

        logger.info ("    Synchronized frames constructed");
    }
};


class NoAspectFrame extends JDLFrame implements DLRightClickListener, DLSelectListener
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public boolean processRightClick (DLRightClickInfo info)
    {
        info.dump ();
        return true;
    }

    public void selectionChanged (DLSelectionInfo info)
    {
        info.dump ();
        return;
    }


    public NoAspectFrame ()
    {
        super ();

        double[] xline, yline;
        int[]    icomp;
 
        DLSelectable    dls;

        xline = new double[10];
        yline = new double[10];
        icomp = new int[10];

        setTitle ("No Aspect Frame Test");

        JDisplayList dl = super.getDL ();

        dl.addRightClickListener (this);
        dl.addSelectListener (this);

        dl.setBackgroundColor (255, 255, 255);
        dl.beginPlot ("random frame 1 test",
                      0.0, 0.0, 200.0, 200.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("random frame1 test",
                        0.0,
                        0.0,
                        250.0,
                        200.0,
                        0.0,
                        0.0,
                        250.0,
                        200.0,
                        0.0,
                        0.0,
                        250.0,
                        200.0,
                        1,
                        0,
                        DLConst.ALLOW_ANY_FRAME_ASPECT_RATIO,
                        DLConst.FRAME_LABEL_ALL_SIDES_BORDER,
                        null,
                        null,
                        null,
                        DLConst.FRAME_NO_ATTACH,
                        0.0,
                        0.0,
                        0);

        AxisProperties ap = new AxisProperties ();
        ap.setLabelFlag (1);
        ap.setTickDirection (-1);
        ap.setCaption ("Right side caption");
        dl.setFrameAxisProperties ("frame1 test", DLConst.FRAME_RIGHT_AXIS, ap);
        ap.setCaption ("Left side caption");
        dl.setFrameAxisProperties ("frame1 test", DLConst.FRAME_LEFT_AXIS, ap);
        ap.setCaption ("Bottom side caption");
        dl.setFrameAxisProperties ("frame1 test", DLConst.FRAME_BOTTOM_AXIS, ap);
        ap.setCaption ("Top side caption");
        dl.setFrameAxisProperties ("frame1 test", DLConst.FRAME_TOP_AXIS, ap);

        dls = new DLSelectable ();
        dl.setSelectable (dls);
        ap.setMajorInterval (200.0);
        ap.setCaption ("Vertical Axis Test");
        dl.addAxis (185.0, 5.0, 185.0, 195.0, 1000.0, 0.0, -1, ap);

        dls = new DLSelectable ();
        dl.setSelectable (dls);
        ap.setCaption ("Horizontal Axis Test");
        ap.setLabelAngle (90.0);
        ap.setLabelAnchor (4);
        ap.setMajorInterval (20.0);
        dl.addAxis (10.0, 120.0, 170.0, 120.0, 0.0, 100.0, -1, ap);

        dls = new DLSelectable ();
        dl.setSelectable (dls);
        ap.setCaption ("Horizontal Axis Test");
        ap.setLabelAngle (-90.0);
        ap.setLabelAnchor (4);
        ap.setMajorInterval (20.0);
        ap.setTickDirection (1);
        dl.addAxis (10.0, 10.0, 170.0, 10.0, 0.0, 100.0, -1, ap);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        xline[0] = 10.0;
        yline[0] = 10.0;
        xline[1] = 90.0;
        yline[1] = 20.0;
        xline[2] = 80.0;
        yline[2] = 80.0;
        xline[3] = 15.0;
        yline[3] = 85.0;
        dl.addLine(xline, yline, 4);

        dl.setLineThickness (0.05);
        dl.setLineSmooth (50);
        dl.addLine(xline, yline, 4);

        dl.setLineSmooth (0);
        dl.setLineThickness (.01);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        xline[0] = 40.0;
        yline[0] = 40.0;
        xline[1] = 60.0;
        yline[1] = 40.0;
        xline[2] = 60.0;
        yline[2] = 50.0;
        xline[3] = 45.0;
        yline[3] = 65.0;
        icomp[0] = 4;

        dl.setFillColor (255, 0, 0);
        dl.setBorderColor (0, 255, 0);
        dl.addFill(xline, yline, icomp, 1, 1);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        xline[0] = 140.0;
        yline[0] = 40.0;
        xline[1] = 160.0;
        yline[1] = 40.0;
        xline[2] = 160.0;
        yline[2] = 50.0;
        xline[3] = 145.0;
        yline[3] = 65.0;
        xline[4] = 140.0;
        yline[4] = 40.0;
        icomp[0] = 5;
        xline[5] = 150.0;
        yline[5] = 50.0;
        xline[6] = 155.0;
        yline[6] = 52.0;
        xline[7] = 153.0;
        yline[7] = 55.0;
        xline[8] = 150.0;
        yline[8] = 50.0;
        icomp[1] = 4;

        dl.setFillColor (255, 200, 0);
        dl.setBorderColor (0, 0, 0);
        dl.addFill(xline, yline, icomp, 2, 1);

        xline[0] = 120.0;
        yline[0] = 20.0;
        xline[1] = 170.0;
        yline[1] = 20.0;
        xline[2] = 170.0;
        yline[2] = 70.0;
        xline[3] = 120.0;
        yline[3] = 70.0;
        icomp[0] = 4;

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillColor (255, 200, 0, 100);
        dl.setBorderColor (0, 0, 0, 100);
        dl.setFillPattern (1, 1.0);
        dl.addFill(xline, yline, icomp, 1, 1);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillPattern (0, 1.0);
        dl.setFillColor (0, 0, 255);
        dl.setBorderColor (255, 0, 0);
        dl.addCircle (155.0, 155.0, 145.0, 145.0, 145.0, 155.0);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillColor (255, 255, 0);
        dl.setBorderColor (0, 0, 0);
        dl.setFillPattern (1, 1.0);
        dl.addCenteredRectangle (100.0, 55.0, 20.0, 14.0, 3.0, 0.0);
        dl.setFillPattern (2, 1.0);
        dl.addCenteredRectangle (120.0, 55.0, 20.0, 14.0, 3.0, 0.0);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillPattern (0, 1.0);
        dl.setFillColor (200, 250, 200);
        dl.setBorderColor (0, 0, 0);
        dl.addArc (80.0, 145.0, 10.0, 7.0, 45.0, 300.0, 0.0, 1);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillColor (-1, -1, -1);
        dl.setFillPattern (3, 1.0);
        dl.setBorderColor (255, 0, 0);
        dl.addArc (80.0, 100.0, 10.0, 7.0, 45.0, 300.0, 90.0, 1);

        dls = new DLSelectable ();
        dl.setSelectable (dls);

        dl.setFillPattern (0, 1.0);
        dl.setTextColor (0, 0, 0);
        dl.setFont ("arial", Font.PLAIN);
        dl.addNumber (15.0, 150.0, .15, 90.0, 1.23, 2, 0);
        dl.addNumber (15.0, 10.0, .15, -90.0, 1.23456e15, 5, 0);
        dl.setTextAnchor (3);
        dl.addNumber (15.0, 100.0, .15, 0.0, 123456789, 0, 1);
        dl.setTextAnchor (1);
        dl.addNumber (190.0, 100.0, .15, 0.0, 123456789, 0, 1);

        dl.setSelectable (null);

        dl.setFrameToExtents (null, 0.0, 40.0, 40.0, 40.0);

        dl.unsetFrame ();

        logger.info ("    No Aspect Frame Constructed");
    }

};



class OverlapTest extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    public OverlapTest ()
    {
        super ();

        double[] gdata;
        double   dx, dy, dist;
        int      i, j, k, jstart;

        setTitle ("Grid Overlap Test");
        setSize (900, 700);

        JDisplayList dl = super.getDL ();

        dl.beginPlot ("grid test",
                      0.0, 0.0, 100.0, 100.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("grid test frame",
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
                       0.0,
                       0);

        gdata = new double[100];
        for (i=0; i<10; i++) {
            jstart = i * 10;
            dy = i * 10.0;
            for (j=0; j<10; j++) {
                k = jstart + j;
                dx = j * 10.0;
                dist = dx * dx + dy * dy;
                dist = Math.sqrt (dist);
                gdata[k] = dist;
            }
        }
        gdata[55] = 1.e30;
        gdata[45] = 1.e30;

        Grid grid = new Grid (
            gdata,
            10,
            10,
            5.0,
            5.0,
            60.0,
            60.0,
            0.0,
            null
        );

        DLSurfaceProperties dlp = new DLSurfaceProperties ();
        dlp.setShowNodes (true);
        ColorPalette cpal = new ColorPalette();
        dl.setImageColors (cpal, 0.0, 130.0);
        dl.addGrid ("test grid 10",
                    grid,
                    dlp);

        grid = new Grid (
            gdata,
            10,
            10,
            25.0,
            25.0,
            80.0,
            80.0,
            0.0,
            null
        );

        dlp = new DLSurfaceProperties ();
        dlp.setShowNodes (true);
        cpal = new ColorPalette();
        cpal.setBrightness (2);
        dl.setImageColors (cpal, 0.0, 130.0);
        dl.addGrid ("test grid 11",
                    grid,
                    dlp);

        grid = new Grid (
           gdata,
           10,
           10,
           15.0,
           45.0,
           70.0,
           90.0,
           0.0,
           null
        );

        dlp = new DLSurfaceProperties ();
        dlp.setShowNodes (true);
        cpal = new ColorPalette();
        cpal.setBrightness (8);
        dl.setImageColors (cpal, 0.0, 130.0);
        dl.addGrid ("test grid 12",
                   grid,
                   dlp);

        logger.info ("    Grid Overlap Test Constructed");

    }
};

class HugeGridTest extends JDLFrame
{
    private static final long serialVersionUID = 1L;

    private static Logger  logger = CSWLogger.getMyLogger ();

    private static final int  MAX_HUGE = 8000;

    public HugeGridTest ()
    {
        super ();
    }

    public void populateDlist ()
    {

        double[] gdata;
        double   dx, dy, dist;
        int      i, j, k, jstart;

        setTitle ("Huge Grid Test");
        setSize (900, 700);

        JDisplayList dl = super.getDL ();

        dl.beginPlot ("grid test",
                      0.0, 0.0, 100.0, 100.0);
        dl.setColor (5, 5, 5);

        dl.setFrameClip (1);
        dl.createFrame ("grid test frame",
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
                       0.0,
                        0);

        Random random = new Random();
        long seed = 1234579;
        random.setSeed (seed);
        double  yspace = .1;
        double  xspace = .1;
        double  gmax = -1.e30;

        int    nhuge = 1000;
        double nfact = 0.0;

        String  tflag = System.getenv ("CSW_NHUGE");

        if (!(tflag == null  ||  tflag.isEmpty())) {
            try {
                int  nh = Integer.parseInt (tflag);
                nhuge = nh;
            }
            catch (Throwable ex) {
            }
        }

        if (nhuge > MAX_HUGE) nhuge = MAX_HUGE;

        JEasyXTest.showMem ("Start huge grid nhuge = " + nhuge);

        try {
            gdata = new double[nhuge * nhuge];
        }
        catch (Throwable ex) {
            System.out.println ();
            System.out.println ("Cannot new the huge grid array for nhuge = " + nhuge);
            System.out.println (ex.getMessage());
            System.out.println ();
            return;
        }

        for (i=0; i<nhuge; i++) {
            jstart = i * nhuge;
            dy = (double)i * yspace;
            for (j=0; j<nhuge; j++) {
                k = jstart + j;
                dx = (double)j * xspace;
                dist = dx * dx + dy * dy;
                dist = Math.sqrt (dist);
                double dnoise = random.nextDouble () - .5;
                gdata[k] = dist + dnoise * nfact;
                if (gdata[k] > gmax) gmax = gdata[k];
            }
        }

        try {
          Grid grid = new Grid (
            gdata,
            nhuge,
            nhuge,
            5.0,
            5.0,
            90.0,
            90.0,
            0.0,
            null
          );

          DLSurfaceProperties dlp = new DLSurfaceProperties ();
          ColorPalette cpal = new ColorPalette();
          dl.setImageColors (cpal, 0.0, gmax * 1.01);
          dl.addGrid ("test huge grid",
                        grid,
                        dlp);

          logger.info ("    HugeGridTest populate Finished");
        }
        catch (Throwable ex) {
          System.out.println ("Exception thrown in huge grid calculation and/or display");
          System.out.println (ex.getMessage());
          System.out.println ();
          System.out.flush ();
          return;
        }

        JEasyXTest.showMem ("End huge grid nhuge = " + nhuge);

    }
};

/*
 * Class for running the HugeGridTest stuff in separate threads.
 */
class HGWorker extends SwingWorker<Integer, Void> {

    HugeGridTest    hg = null;

    HGWorker (HugeGridTest  hgin)
    {
         hg = hgin;
    }

    protected Integer doInBackground () {
        hg.populateDlist ();
        return 0;
    }

    protected void done () {
        hg.setVisible (true);
    }

}



/**
 * This class has a runnable that first populates the specified
 * HugeFrame object.  After this object has been populated, the
 * frame is made visible, which triggers the drawing of the objects
 * previously set via the populateDlist method.
 */
class HugeGridRunnable implements Runnable {

    HugeGridTest     hg;

    public HugeGridRunnable (HugeGridTest hgin) {hg = hgin;}

    public void run () {

//  I will use the environment variable for turning
//  threads on and off both here and surfaceworks

      String  tflag = System.getenv ("CSW_DONT_USE_THREADS");

      if (tflag == null  ||  tflag.isEmpty()) {
        HGWorker hgw = new HGWorker (hg);
        hgw.execute ();
      }
      else {
System.out.println ("Huge Grid not using threads");
System.out.flush ();
        hg.populateDlist ();
        hg.setVisible (true);
      }

    }
}
