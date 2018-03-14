/*
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
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;
import javax.swing.ToolTipManager;
import javax.swing.WindowConstants;

import csw.jutils.src.XYZPoint;
import csw.jutils.src.XYZNode;
import csw.jutils.src.XYZPolyline;
import csw.jutils.src.LightData;
import csw.jutils.src.View3DProperties;

import csw.jsurfaceworks.src.DBLineList;
import csw.jsurfaceworks.src.DBPointList;
import csw.jsurfaceworks.src.JSurfaceWorks;
import csw.jsurfaceworks.src.TriMesh;

import csw.j3d.src.*;



/**
 *
 * This class is used as a main entry for unit testing 3d graphics stuff.
 *
 *  @author Glenn Pinkerton
 *
*/
public class J3DUnitTest {

    //private static Logger  logger = CSWLogger.getMyLogger ();

    static {
        System.loadLibrary ("_csw_all");
        System.loadLibrary ("gluegen-rt");
        System.loadLibrary ("jogl_desktop");
        System.loadLibrary ("jogl_mobile");
        System.loadLibrary ("newt");
    }

    public static void main (String[] args) {

        Runnable run = new Runnable ()
        {
            public void run ()
            {
                J3DUnitTest.runMainMethod ();
            }
        };

        SwingUtilities.invokeLater (run);
    }


    public static void runMainMethod ()
    {


        String  cpar = System.getenv ("CSW_PARENT");
        if (cpar == null) {
            cpar = "/home/git_glenn/csw_master";
        }
        else {
            if (cpar.isEmpty ()) {
                cpar = "/home/git_glenn/csw_master";
            }
        }

        String  playback_file_name = cpar + "/csw/jtest/utest.j3d";

        JSurfaceWorks.openLogFile (playback_file_name);

        UnitTestFrame frame = new UnitTestFrame ();
        frame.setDefaultCloseOperation (JFrame.EXIT_ON_CLOSE);
        frame.setVisible (true);

        ToolTipManager.sharedInstance().
            setLightWeightPopupEnabled (false);

    }

}



class UnitTestFrame extends JFrame
{

    public static final long     serialVersionUID = 1357246;

    public UnitTestFrame ()
    {

/*    	
try {
int idum = System.in.read();
}
catch (Exception e)
{
}
*/

        final SimplestFrame frame = new SimplestFrame ();

        frame.setVisible (true);

        setTitle ("J3D Unit Test");
        setSize (200, 400);
        setResizable (false);
        Container contentPane = getContentPane ();
        contentPane.setLayout (new GridLayout(15,1));


        JButton clear_button = new JButton ("Clear and Redraw");
        clear_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                if (frame != null) {
                    frame.clearAndRedraw ();
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

        JButton file_button = new JButton ("Draw trimesh file");
        file_button.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent ae){
                TmeshFileFrame fileFrame = new TmeshFileFrame ();
                fileFrame.setVisible (true);
            }
        });

        contentPane.add (preview_button);
        contentPane.add (hide_button);
        contentPane.add (apply_button);
        contentPane.add (hidesurf_button);
        contentPane.add (unhidesurf_button);
        contentPane.add (clear_button);
        contentPane.add (file_button);
    }

}


class SimplestFrame extends JFrame
   implements J3DLineEditListener, J3DRightClickListener, J3DSelectListener
{
    public static final long    serialVersionUID = 917331;

    private J3DPanel      panel;

    private ArrayList<J3DSurfacePatch> slist;
    private String    sname;

    public SimplestFrame ()
    {
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        setTitle ("Simplest Test");
        setSize (700, 700);
        setResizable (true);

        View3DProperties vp = new View3DProperties ();
        vp.setVerticalExaggeration (2.0);
        panel = new J3DPanel (this);
        panel.setView3DProperties (vp);

        panel.addRightClickListener (this);

        panel.setBackgroundColor (Color.white);

        Container contentPane = getContentPane ();
        contentPane.add (panel);

        createLinePopup ();

        clearAndRedraw ();
    }

    void clearAndRedraw ()
    {

        panel.clearAll (false);

        LightData light = panel.getDrawingAttributes().getLight1Data ();
        light.setBrightness(100);
        panel.getDrawingAttributes().setLight1Data (light);

        light = panel.getDrawingAttributes().getLight2Data ();
        light.setBrightness(25);
        light.setEnabled(true);
        panel.getDrawingAttributes().setLight2Data (light);

        panel.getDrawingAttributes().setDoWireFrame (false);

        slist = new ArrayList<J3DSurfacePatch> (20);

        Color  color;

        color = new Color (50, 50, 250, 255);
        sname = "wierd surface";
        createSurface (0.0, 10.0, -1.0, 40.0, 40.0, 2.00, color, false);

        color = new Color (255, 0, 0, 128);
        sname = "Red surface";
        createSurface (0.0, 0.0, 0.0, 50.0, 50.0, 2.00, color, false);

        sname = "Green surface";
        createSurface (0.0, 10.0, -2.00, 40.0, 40.0, 2.00, Color.green, false);

        sname = "Gray surface";
        color = new Color (140, 140, 140, 180);
        createSurface (100.0, 40.0, -7.50, 30.0, 40.0, 2.00, color, false);

        sname = "Orange surface";
        createSurface (0.0, 0.0, -10.00, 50.0, 50.0, 2.00, Color.orange, false);

        sname = "Blue surface";
        color = new Color (0, 0, 255, 200);
        createSurface (-20.0, 0.0, -5.00, 60.0, 50.0, 2.00, color, false);

/*
        createPlane (Color.green, 0.0, 1.0, 0.0);
        createPlane (Color.red, 0.0, 0.0, 1.0);
*/

        //xsurf = -20.0;
        //ysurf = -20.0;
        //zsurf = -12.00;

        J3DText  tp;

        tp = new J3DText ();
        tp.setAnchor (5);
        tp.setXYZ (100.0, 100.0, 0.0);
        tp.setTextSize (1.0);
        tp.setText ("Text Line");
        tp.setGlobalColor (null);
        //panel.addText (tp);

        //addRandomPoints ();

        panel.redrawAll ();

    }

    //private Frame              parentFrame = null;


    void createPlane (Color color, double c0, double c1, double c2)
    {
        J3DHorizonPatch    hp;
        //J3DFault      fp;
        int                i;

        Random random = new Random();
        random.setSeed (1234567);

        double[] x = new double[10];
        double[] y = new double[10];
        double[] z = new double[10];

        double xmin = 1.e30;
        double ymin = 1.e30;
        double xmax = -1.e30;
        double ymax = -1.e30;
        for (i=0; i<10; i++) {
            x[i] = random.nextDouble () * 10.0;
            y[i] = random.nextDouble () * 10.0;
            z[i] = c0 + c1 * x[i] + c2 * y[i];
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

        //GridGeometry geom =
        //new GridGeometry (100, 100, xmin, ymin, xmax, ymax);

        TriMesh tmesh = sw.calcTriMesh
        (
            x,
            y,
            z,
            10,
            null,
            0,
            null,
            1,
            null,  // geom
            null
        );

        hp = new J3DHorizonPatch ();
        hp.setTriMesh (tmesh);
        hp.setGlobalColor (color);
        hp.setSurfaceName (sname);
        panel.addHorizonPatch (hp);
        slist.add (hp);

    }



    void createSurface (double x0, double y0, double z0,
                        double dx, double dy, double dz,
                        Color color, boolean faultFlag)
    {
        J3DHorizonPatch    hp;
        J3DFault      fp;
        int                i;

        z0 *= 2.0;
        dz *= 2.0;

        double[] x = new double[200];
        double[] y = new double[200];
        double[] z = new double[200];

        Random random = new Random();
        long  ctw = System.currentTimeMillis ();
        random.setSeed (ctw);

        double xmin = 1.e30;
        double ymin = 1.e30;
        double xmax = -1.e30;
        double ymax = -1.e30;

        int  np = random.nextInt (100);
        for (i=0; i<np; i++) {
            x[i] = random.nextDouble () * dx + x0;
            y[i] = random.nextDouble () * dy + y0;
            z[i] = random.nextDouble () * dz + z0;
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

        //GridGeometry geom =
        //new GridGeometry (100, 100, xmin, ymin, xmax, ymax);

        TriMesh tmesh = sw.calcTriMesh
        (
            x,
            y,
            z,
            10,
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
        }
        else {
            hp = new J3DHorizonPatch ();
            hp.setTriMesh (tmesh);
            hp.setGlobalColor (color);
            hp.setSurfaceName (sname);
            panel.addHorizonPatch (hp);
            slist.add (hp);
        }

    }

/*----------------------------------------------------------------------------------*/

    private void addRandomPoints ()
    {
        Random random = new Random();
        random.setSeed (1234567);

        ArrayList<XYZPoint>    plist;
        J3DLine         jline;
        XYZPoint        p;

        int i;
        double    x, y, z;

        plist = new ArrayList<XYZPoint> ();

        for (i=0; i<10; i++) {
            x = random.nextDouble () * 100;
            y = random.nextDouble () * 100;
            z = random.nextDouble () * 10 - 10;
            p = new XYZPoint (x, y, z);
            plist.add (p);
        }

        jline = new J3DLine ();
        jline.setPoints (plist);
        jline.setHidden (false);
        jline.setGlobalColor (Color.white);

        panel.addLine (jline);

    }


    //private double xsurf, ysurf, zsurf;

/*----------------------------------------------------------------------------------*/

    private J3DFault newFault = null;
    //private boolean editActive = false;

   //private boolean linePickFlag = true;

    //private static final int NEW_FAULT_PATCH_MODE = 1;
    //private static final int NEW_HORIZON_PATCH_MODE = 2;

/*----------------------------------------------------------------------------------*/

    public void endSurfaceEdit ()
    {


        showEditSurface (false);
        panel.addFault (newFault);
        newFault = null;
        panel.setPreviewFault (null);

        panel.setSelectionMode();

        panel.redrawAll ();

        //editActive = false;

        return;
    }
/*
    private void cancelSurfaceEdit ()
    {

        newFault = null;

        panel.redrawAll ();

        editActive = false;

        return;
    }
*/
    /*
    private void resetSurfaceEdit ()
    {

        if (newFault == null) {
            newFault = new J3DFault ();
        }

        newFault.setVisible(false);

        panel.redrawAll ();

        return;
    }
*/
/*--------------------------------------------------------------------------*/

    public void showEditSurface (boolean draw)
    {
        int               i;

        if (newFault == null) {
            newFault = new J3DFault ();
        }

        ArrayList<XYZNode>       na = null;

        ArrayList<J3DLine>  selectedLines = panel.getSelectedLines ();
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
        lalist = new ArrayList<XYZPolyline>(size);
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
          na,
          lalist,
          null,
          1,
          null,
          null,
          faultFlag);

        newFault.setVisible(true);
        newFault.setTriMesh (tmesh);
        newFault.setTm3d (null);

        if (draw) {
            panel.setPreviewFault (newFault);
            panel.redrawAll ();
        }

        return;



    }

/*--------------------------------------------------------------------------*/

    public void hideEditSurface ()
    {

        if (newFault == null) {
            newFault = new J3DFault ();
        }
        newFault.setVisible(false);

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
/*
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
*/
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
        int type = info.getClickedObjectType ();
        if (type == J3DRightClickInfo.LINE) {
            showLinePopup (info);
        }
    }

/*----------------------------------------------------------------------------------*/

    JPopupMenu linePopup = null;

    JMenuItem deleteItem;
    JMenuItem newFaultItem;
    JMenuItem propertiesItem;

/*----------------------------------------------------------------------------------*/

    void createLinePopup ()
    {

    /*
     * Create a popup menu with choices for viewing from the
     * north, south, east, west, top or bottom direction.
     */
        linePopup = new JPopupMenu ();
        linePopup.setLightWeightPopupEnabled (false);

        deleteItem =
            new JMenuItem ("Delete line");
        newFaultItem =
            new JMenuItem ("New Fault...");
        propertiesItem =
            new JMenuItem ("Properties...");

        linePopup.add (deleteItem);
        linePopup.add (newFaultItem);
        linePopup.add (propertiesItem);

    /*
     * Create actions to invoke preset J3DPanel views
     * when the popup menu items are selected.
     */
        deleteItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
            }
        });

        newFaultItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
            }
        });

        propertiesItem.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
            }
        });

    }

/*----------------------------------------------------------------------------------*/

    void showLinePopup (J3DRightClickInfo info)
    {
        int        ix, iy;

        ix = info.getXScreen ();
        iy = info.getYScreen ();

        linePopup.show (panel, ix, iy);

    }

/*----------------------------------------------------------------------------------*/

}


/*----------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------*/

class TmeshFileFrame extends JFrame
{
    public static final long     serialVersionUID = 1341046;

    //private String                 fileName;
    private JTextField             tField;
    private JTextField             tField2;
    private JTextField             tField3;

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

    private J3DPanel      panel;

    public TmeshFileFrame ()
    {
        setDefaultCloseOperation (WindowConstants.DISPOSE_ON_CLOSE);

        setTitle ("Draw Trimesh File");
        setSize (700, 700);
        setResizable (true);

        View3DProperties vp = new View3DProperties ();
        vp.setVerticalExaggeration (2.0);
        panel = new J3DPanel (this);

        tField = new JTextField (40);
        tField.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            TmeshFileFrame.this.setFileName ();
          }
        });

        JLabel tl = new JLabel ("TriMesh File:", JLabel.RIGHT);
        tl.setLabelFor (tField);

        tField2 = new JTextField (40);
        tField2.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            TmeshFileFrame.this.setLineFileName ();
          }
        });

        JLabel tl2 = new JLabel ("Line File:", JLabel.RIGHT);
        tl2.setLabelFor (tField2);

        tField3 = new JTextField (40);
        tField3.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            TmeshFileFrame.this.setPointFileName ();
          }
        });

        JLabel tl3 = new JLabel ("Point File:", JLabel.RIGHT);
        tl3.setLabelFor (tField2);

        JButton clearButton = new JButton ("Clear");
        clearButton.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent ae){
            TmeshFileFrame.this.clearData ();
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
        lpanel.add (clearButton);

        JPanel cpanel = new JPanel ();
        cpanel.setLayout (new BorderLayout ());
        cpanel.add (lpanel, BorderLayout.NORTH);
        cpanel.add (panel, BorderLayout.CENTER);

        Container contentPane = getContentPane ();
        contentPane.add (cpanel);

    }

    private ArrayList<TriMesh> tmeshList =
      new ArrayList<TriMesh> (10);
    private ArrayList<DBLineList> lineList =
      new ArrayList<DBLineList> (10);
    private ArrayList<DBPointList> pointList =
      new ArrayList<DBPointList> (10);

    void clearData ()
    {
        tmeshList.clear ();
        lineList.clear ();
        pointList.clear ();
        clearAndRedraw ();
    }

    void setFileName ()
    {
        TriMesh tmesh;
        String  fname;

        fname = getField().getText ();

        tmesh = TriMesh.createFromAsciiDebugFile (fname);

        if (tmesh == null) {
            System.out.println ("bad file name");
            return;
        }

        tmeshList.add (tmesh);

        clearAndRedraw ();

    }

    void setLineFileName ()
    {
        DBLineList  dbline;
        String  fname;

        fname = getField2().getText ();

        dbline = new DBLineList ();

        try {
            dbline.loadAsciiFile (fname);
        }
        catch (Exception e) {
            System.out.println ("Bad file name for line file");
            System.out.flush ();
            return;
        }

        lineList.add (dbline);

        clearAndRedraw ();

    }

    void setPointFileName ()
    {
        DBPointList  dbline;
        String  fname;

        fname = getField3().getText ();

        dbline = new DBPointList ();

        try {
            dbline.loadAsciiFile (fname);
        }
        catch (Exception e) {
            System.out.println ("Bad file name for point file");
            System.out.flush ();
            return;
        }

        pointList.add (dbline);

        clearAndRedraw ();

    }

    void clearAndRedraw ()
    {

        panel.clearAll (false);

        LightData light = panel.getDrawingAttributes().getLight1Data ();
        light.setBrightness(100);
        panel.getDrawingAttributes().setLight1Data (light);

        light = panel.getDrawingAttributes().getLight2Data ();
        light.setBrightness(25);
        light.setEnabled(true);
        panel.getDrawingAttributes().setLight2Data (light);

        panel.getDrawingAttributes().setDoWireFrame (true);

        J3DHorizonPatch hp;

        int    size, ido;
        TriMesh tmesh;

        size = tmeshList.size ();
        for (ido=0; ido<size; ido++) {
            tmesh = tmeshList.get (ido);
            if (tmesh == null) continue;
            hp = new J3DHorizonPatch ();
            hp.setTriMesh (tmesh);
            hp.setGlobalColor (Color.gray);
            hp.setSurfaceName ("testsurf");
            panel.addHorizonPatch (hp);

        /*
            tmesh.calcTriangleCenters ();
            double[] xc = tmesh.getXCenters ();
            double[] yc = tmesh.getYCenters ();
            double[] zc = tmesh.getZCenters ();
            int    nc = tmesh.getNumCenters ();

            J3DText    jt;

            for (int i=0; i<nc; i++) {
                if (xc[i] < 360000  ||  xc[i] > 375000) continue;
                if (yc[i] < 7208000  ||  yc[i] > 7232000) continue;
                jt = new J3DText ();
                jt.setAnchor (5);
                jt.setXYZ (xc[i], yc[i], zc[i]);
                jt.setText (""+i);
                jt.setGlobalColor (Color.red);
                panel.addText (jt);
            }
          */
        }

        DBLineList   dbline;
        XYZPolyline  pline;
        J3DLine      line3d;
        double[]     xa, ya, za;
        int          n;

        size = lineList.size ();
        for (ido=0; ido<size; ido++) {
            dbline = lineList.get (ido);
            if (dbline == null) continue;
            n = 0;
            for (int i=0; i<dbline.numLines; i++) {
                pline = new XYZPolyline (dbline.linePts[i]);
                xa = pline.getXArray ();
                ya = pline.getYArray ();
                za = pline.getZArray ();
                for (int j=0; j<dbline.linePts[i]; j++) {
                    xa[j] = dbline.x[n];
                    ya[j] = dbline.y[n];
                    za[j] = dbline.z[n];
                    n++;
                }
                line3d = new J3DLine ();
                line3d.setLine (pline);
                line3d.setGlobalColor (Color.red);
                line3d.setLineWidth (2.0);
                panel.addLine (line3d);
            }
        }


        DBPointList   dbpoint;

        size = pointList.size ();
        for (ido=0; ido<size; ido++) {
            dbpoint = pointList.get (ido);
            if (dbpoint == null) continue;
            line3d = new J3DLine ();
            line3d.setPoints (dbpoint.x, dbpoint.y, dbpoint.z);
            line3d.setGlobalColor (Color.blue);
            line3d.setLineWidth (1.0);
            panel.addLine (line3d);
        }

        panel.redrawAll ();

    }


}


