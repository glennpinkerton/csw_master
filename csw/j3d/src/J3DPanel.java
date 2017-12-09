
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/**
 This package is used for 3D graphical visualization..
 *
 * @author Glenn Pinkerton
 */
package csw.j3d.src;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.Point;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.image.BufferedImage;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Date;

import javax.swing.AbstractAction;
import javax.swing.ActionMap;
import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.InputMap;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JToolBar;
import javax.swing.KeyStroke;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.border.Border;
import javax.swing.event.MouseInputAdapter;

//import javax.media.opengl.GLAutoDrawable;
import com.jogamp.opengl.GLAutoDrawable;


import com.jogamp.opengl.GL;
//import com.jogamp.opengl.GL4;
import com.jogamp.opengl.GLProfile;
import com.jogamp.opengl.awt.GLCanvas;
import com.jogamp.opengl.GLCapabilities;
//import com.jogamp.opengl.GLDrawable;
//import com.jogamp.opengl.GLDrawableFactory;
import com.jogamp.opengl.GLEventListener;
import com.jogamp.opengl.glu.GLU;

/*
import net.java.games.jogl.GL;
import net.java.games.jogl.GLCanvas;
import net.java.games.jogl.GLCapabilities;
import net.java.games.jogl.GLDrawable;
import net.java.games.jogl.GLDrawableFactory;
import net.java.games.jogl.GLEventListener;
import net.java.games.jogl.GLU;
*/

//import images.jtoolbar.general.JIconsGeneral;

import csw.jsurfaceworks.src.*;

import csw.jutils.src.*;

/**
 * The J3DPanel class is used to draw to and interact with 3D
 * graphical views.  The class supports trimesh surfaces, lines,
 * points and text on the view.  The actual rendering is done via
 * JOGL opengl bindings supported by SGI and SUN.
 * <p>
 * More detail on the workings of the class can be obtained via
 * the references for the public methods.
 */
public class J3DPanel extends JPanel implements GLEventListener
{
    static final int         NORTH = 1;
    static final int         SOUTH = 2;
    static final int         EAST = 3;
    static final int         WEST = 4;
    static final int         TOP = 5;
    static final int         BOTTOM = 6;

    public static final long    serialVersionUID = 39923498;

    private final static double REASONABLE_Z_VALUE_LIMIT = 1e20;

    private boolean isTesting = false;

    private boolean isPropertyApply = false;

    private static J3DPanel currentPanel = null;

    private View3DProperties  view3DProperties;

    private J3DDrawingAttributes drawingAttributes;
    public View3DProperties getViewProperties ()
    {
      return drawingAttributes.getViewProperties();
    }

    private J3DMouseListener  mouse_listen;
    private J3DWheelListener  wheel_listen;

    private JGL               jgl;
    private GLCanvas          canvas = null;

    private JLabel    pickModeLabel,
                      objectNameLabel,
                      xPositionLabel,
                      yPositionLabel,
                      zPositionLabel;
    private JPanel    statusPanel;

    J3DToolBar           toolBar;
    private JPanel       toolBarBox;

    private J3DTool      tool;

    private AtColorLookup   colorLookup;

    private AttributeDefList  atDefList = new AttributeDefList ();

    private double pickTolerance = 0.0;

    /*------------------------------------------------------------------------*/
    /**
     * Default constructor.  Requires a successive call to setView3DProperties().
     */
    public J3DPanel(JFrame  jf) {
      parentFrame = jf;
      initPanel(null);
    }

    public JFrame getParentFrame ()
    {
      return parentFrame;
    }

    public void setView3DProperties (View3DProperties  vp)
    {
        view3DProperties = vp;
        drawingAttributes.setViewProperties (vp);
        if (view3DProperties != null  &&  canvas != null) {
          Color bgc = view3DProperties.getBackgroundColor();
          if (bgc != null) {
            canvas.setBackground (bgc);
          }
        }
    }

  /*
   * The init method is called by all constructors.
   */
    private void initPanel (Object toolbarConstraints) {
        drawingAttributes = new J3DDrawingAttributes(this);

    /*
     * Register as listener for keyboard and mouse events.
     */
        KeyboardActions ka = new KeyboardActions();
        ka.setupKeyboardActions ();

        mouse_listen = new J3DMouseListener ();
        wheel_listen = new J3DWheelListener ();

    /*
     * Initialize to default scaling, translation and rotation for
     * the 3d view.  The current and home values are also initially
     * set to the defaults.
     */
        xScale = 1.1;
        yScale = 1.1;
        zScale = 1.1;
        xScaleDef = 1.1;
        yScaleDef = 1.1;
        zScaleDef = 1.1;
        xScaleHome = 1.1;
        yScaleHome = 1.1;
        zScaleHome = 1.1;

        xTrans = 0.0;
        yTrans = 0.1;
        zTrans = -3.0;
        xTransDef = 0.0;
        yTransDef = 0.1;
        zTransDef = -3.0;
        xTransHome = 0.0;
        yTransHome = 0.1;
        zTransHome = -3.0;

        xRot = 20.0;
        yRot = 40.0;
        zRot = 0.0;
        xRotDef = 20.0;
        yRotDef = 40.0;
        zRotDef = 0.0;
        xRotHome = 20.0;
        yRotHome = 40.0;
        zRotHome = 0.0;

    /*
     * Initialize the xy and z units to meters.
     */
        xyUnits = 1.0;
        zUnits = 1.0;

    /*
     * Set shading and lighting to true.
     */
        drawingAttributes.setDoShading(true);
        drawingAttributes.setDoLighting(true);

    /*
     * By default, the panel is not in a testing mode.
     */
        isTesting = false;

    /*
     * Draw surfaces with solid fill and without wireframe.
     */
        drawingAttributes.setDoSolidFill(true);
        drawingAttributes.setDoWireFrame(false);

    /*
     * Create the scaler object, which is modified when the
     * limits of the view are updated.
     */
        scaler = new Scaler ();

        selectionColor = new Color (0, 220, 220, 200);
        lineSelectionColor = new Color (0, 220, 220, 255);


    /*
     * Create the object that actually interfaces with the jogl
     * opengl bindings.
     */
        jgl = new JGL ();

    /*
     * Now that all the members have default values, create the
     * toolbar, canvas and status bars and add them to the panel.
     * This has to come last because the toolbar queries the
     * members of this panel to fill in defaults for its dialogs.
     */
        setLayout (new BorderLayout());
        setMinimumSize (new Dimension (2, 2));

    /*
     * Create a J3DToolBar and put it in the specified position.
     *
     * Having the toolBarBox be a JPanel with a BoxLayout rather than
     * a Box solves the problem of ghost graphics from other places in
     * the GUI
     */
        toolBar = new J3DToolBar (this);
        toolBarBox = new JPanel();
        toolBarBox.setLayout(new BoxLayout(toolBarBox, BoxLayout.X_AXIS));
        add(toolBarBox, BorderLayout.NORTH);

        toolBarBox.add (toolBar, BorderLayout.NORTH);

    /*
     * Create a GLCanvas and put it in the CENTER position.
     */
        createCanvas ();

        add (BorderLayout.CENTER, canvas);

        createStatusBar ();
        add (BorderLayout.SOUTH, statusPanel);

        nextSolidListID = 1000;
        nextWireListID = 1001;

        currentPanel = this;
        tool = new J3DSelectionTool(this, jgl);

        pickTolerance = 0.0;
    }

/*------------------------------------------------------------------------------------------------*/

    JComboBox<AttributeDef>   atDefCombo;

    public void buildAtDefCombo ()
    {

      AttributeDef        atdef;
      AttributeDef[]      atDefArray;

      int ndef = atDefList.size ();
      if (ndef < 1) {
        atDefCombo = null;
        return;
      }

      atDefArray = new AttributeDef[ndef];
      for (int i=0; i<ndef; i++) {
        atdef = atDefList.get (i);
        atDefArray[i] = atdef;
      }

      atDefCombo = new JComboBox<AttributeDef> (atDefArray);
      atDefCombo.setLightWeightPopupEnabled (false);

      atDefCombo.addActionListener(
        new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            int index =
              J3DPanel.this.atDefCombo.getSelectedIndex ();
            J3DPanel.this.atDefList.resetCurrent (index);
          }
        }
      );

    }

/*--------------------------------------------------------------------------*/

    JComboBox<AttributeDef> getAtDefCombo ()
    {
      return atDefCombo;
    }

/*--------------------------------------------------------------------------*/

    private synchronized void restart ()
    {
        canvas.display ();

        postDrawMethod ();
    }

  /*
   * This method is originally put in to do any hacks needed to work around
   * the 2d-3d interaction problems associated with some graphics cards.
   * If no hacks are needed in the future, this can do nothing.
   */
    private void postDrawMethod ()
    {
        drawPickModeMessageImmediately ();
    }

  /*
   * This is called only by the run method when the canvas is null.
   * The canvas is created and set up in the render thread.  Once it
   * is ready, the calling thread is notified.
   */
    private void createCanvas ()
    {
        GLProfile  profile = GLProfile.getMaxFixedFunc(true);
        GLCapabilities c = new GLCapabilities (profile);
        canvas = new GLCanvas (c);
        canvas.addGLEventListener (this);
        canvas.addMouseListener(mouse_listen);
        canvas.addMouseWheelListener(wheel_listen);
        canvas.addMouseMotionListener(mouse_listen);

        if (view3DProperties != null  &&  canvas != null) {
          Color bgc = view3DProperties.getBackgroundColor();
          if (bgc != null) {
            canvas.setBackground (bgc);
          }
        }

        parentFrame.getContentPane().add(canvas);

    }

    /*----------------------------------------------------------------------------*/
    /**
     * Append additional toolbar to the existing tool bar.
     */
    public void appendToolBar(JToolBar jToolBar)
    {
        toolBarBox.add(jToolBar);
        validate();
        repaint();
    }

    /*----------------------------------------------------------------------------*/
    /**
     * Remove a toolbar from the existing tool bar box.
     */
    public void removeToolBar(JToolBar jToolBar)
    {
        toolBarBox.remove(jToolBar);
        validate();
        repaint();
    }

    /*----------------------------------------------------------------------------*/
    public void setAtColorLookup (AtColorLookup atcolor)
    {
        colorLookup = atcolor;
    }

    /*----------------------------------------------------------------------------*/

    public AttributeDefList getAtDefList ()
    {
        return atDefList;
    }

    /*----------------------------------------------------------------------------*/

    private void createStatusBar ()
    {
        GridLayout hgrid = new GridLayout (1, 1);
        statusPanel = new JPanel (hgrid);

        Border border = BorderFactory.createLineBorder (Color.BLACK, 1);
        Border empty = BorderFactory.createEmptyBorder (1, 3, 1, 0);
        Font font = new Font ("Arial", Font.PLAIN, 12);

        statusPanel.setBorder (border);
        pickModeLabel = new JLabel ("Selection Mode");
        pickModeLabel.setBorder (empty);
        pickModeLabel.setFont (font);
        statusPanel.add (pickModeLabel);
    }

    /**
     * Shows the popup in the current location.
     * @param popup popup
     * @param x x screen location (generally from J3DRightClickInfo)
     * @param y y screen location (generally from J3DRightClickInfo)
     */
    public void showPopup(JPopupMenu popup, int x, int y) {
      popup.show(canvas, x, y);
    }

/*------------------- Panel Data Objects -----------------------------------*/

    ArrayList<J3DLine>           lineList = new ArrayList<J3DLine> ();
    ArrayList<J3DHorizonPatch>   horizonList = new ArrayList<J3DHorizonPatch> ();
    ArrayList<J3DFault>          faultList = new ArrayList<J3DFault> ();
    ArrayList<J3DText>           textList = new ArrayList<J3DText> ();

  /**
  Add a line to the 3d display.  If the specified
  {@link J3DLine} object does not have a valid originalPoints member it will
  not be displayed.
  */
    public void addLine (J3DLine lp)
    {
        if (lp == null) {
            return;
        }

        if (lineList == null) {
            lineList = new ArrayList<J3DLine> (20);
        }

        lineList.add (lp);
        visibleLimitsUpdateNeeded = true;

        drawDirty = true;

        return;
    }

  /**
  Return a list of the lines currently added to the 3d display.  The list is the
  actual internal list of all lines added to the 3d display. It is not a copy.
  The members of the list are references to the data
  added to the display via {@link #addLine}.
  */
    public ArrayList<J3DLine> getLineList ()
    {
        return lineList;
    }

/*----------------------------------------------------------------------------*/

  /**
  Remove a line patch from the display.
  */
    public void removeLine (J3DLine lp)
    {
        if (lp == null  ||  lineList == null) {
            return;
        }

        J3DLine          lp2;
        int              i, n;

        n = lineList.size();
        for (i=0; i<n; i++) {
            lp2 = lineList.get (i);
            if (lp2 == null) {
                continue;
            }
            if (lp2 == lp) {
                lineList.set (i, null);
            }
        }

        for (i=n-1; i>= 0; i--) {
            lp2 = lineList.get (i);
            if (lp2 == null) {
                lineList.remove (i);
            }
        }

        drawDirty = true;

        visibleLimitsUpdateNeeded = true;

        return;
    }


/*---------------------------------------------------------------------------*/

  /**
  Add a fault to the 3d display.  If the specified
  {@link J3DFault} object does not have a valid trimesh member, it will
  not be displayed.
  */
    public void addFault (J3DFault fp)
    {
        if (fp == null) {
            return;
        }

        if (faultList == null) {
            faultList = new ArrayList<J3DFault> (20);
        }

        faultList.add (fp);
        visibleLimitsUpdateNeeded = true;

        drawDirty = true;

        fp.solidListID = -nextSolidListID;
        fp.wireListID = -nextWireListID;
        nextSolidListID += 2;
        nextWireListID += 2;

        return;
    }

  /**
  Return a list of the faults currently added to the 3d display.  The list is the
  actual internal list of all faults added to the 3d display.
  The members of the list are references to the data
  added to the display via {@link #addFault}.
  */
    public ArrayList<J3DFault> getFaultList ()
    {
        return faultList;
    }

/*----------------------------------------------------------------------------*/

  /**
  Remove a fault from the display.
  */
    public void removeFault (J3DFault fp)
    {
        if (fp == null  ||  faultList == null) {
            return;
        }

        J3DFault    fp2;
        int              i, n;

        n = faultList.size();
        for (i=0; i<n; i++) {
            fp2 = faultList.get (i);
            if (fp2 == null) {
                continue;
            }
            if (fp2 == fp) {
                faultList.set (i, null);
            }
        }

        for (i=n-1; i>= 0; i--) {
            fp2 = faultList.get (i);
            if (fp2 == null) {
                faultList.remove (i);
            }
        }

        drawDirty = true;

        visibleLimitsUpdateNeeded = true;

        return;
    }

/*---------------------------------------------------------------------------*/

  /**
  Add a free form text item to the 3d view.  If the specified {@link J3DText}
  does not hava a valid text string or valid xyz location, the text item will
  not be drawn.
  */
    public void addText (J3DText tp)
    {
        if (tp == null) {
            return;
        }

        if (textList == null) {
            textList = new ArrayList<J3DText> (20);
        }

        textList.add (tp);
        visibleLimitsUpdateNeeded = true;

        drawDirty = true;

        return;
    }

  /**
  Return a list of the text items currently added to the 3d display.  The list is the
  actual internal list of all text added to the 3d display.
  The members of the list are references to the data
  added to the display via {@link #addText}.
  */
    public ArrayList<J3DText> getTextList ()
    {
        return textList;
    }


  /**
  Remove a text item from the display.
  */
    public void removeText (J3DText tp)
    {
        if (tp == null  ||  textList == null) {
            return;
        }

        J3DText          tp2;
        int              i, n;

        n = textList.size();
        for (i=0; i<n; i++) {
            tp2 = textList.get (i);
            if (tp2 == null) {
                continue;
            }
            if (tp2 == tp) {
                textList.set (i, null);
            }
        }

        for (i=n-1; i>= 0; i--) {
            tp2 = textList.get (i);
            if (tp2 == null) {
                textList.remove (i);
            }
        }

        drawDirty = true;

        visibleLimitsUpdateNeeded = true;

        return;
    }

/*---------------------------------------------------------------------------*/

  /**
   * Add a fully functional horizon patch to the 3d display.  If the specified
   * {@link J3DHorizonPatch} object does not have a valid trimesh member, it will
   * not be displayed.
   *
   * @return false if patch was added successfully, true if not.
   */
    public boolean addHorizonPatch (J3DHorizonPatch hp) {
        if (hp == null) {
            return(true);
        }

        if (horizonList == null) {
            horizonList = new ArrayList<J3DHorizonPatch> (20);
        }

        horizonList.add (hp);

        visibleLimitsUpdateNeeded = true;

        drawDirty = true;

        hp.solidListID = -nextSolidListID;
        hp.wireListID = -nextWireListID;
        nextSolidListID += 2;
        nextWireListID += 2;

        return(false);
    }

  /**
  Return a list of the horizons currently added to the 3d display.  The list is the
  actual internal list of all horizons added to the 3d display.
  The members of the list are references to the data
  added to the display via {@link #addHorizonPatch}.
  */
    public ArrayList<J3DHorizonPatch> getHorizonList ()
    {
        return horizonList;
    }

/*----------------------------------------------------------------------------*/

  /**
  Remove a horizon patch from the display.
  */
    public void removeHorizonPatch (J3DHorizonPatch hp)
    {
        if (hp == null  ||  horizonList == null) {
            return;
        }

        J3DHorizonPatch    hp2;
        int              i, n;

        n = horizonList.size();
        for (i=0; i<n; i++) {
            hp2 = horizonList.get (i);
            if (hp2 == null) {
                continue;
            }
            if (hp2 == hp) {
                horizonList.set (i, null);
            }
        }

        for (i=n-1; i>= 0; i--) {
            hp2 = horizonList.get (i);
            if (hp2 == null) {
                horizonList.remove (i);
            }
        }

        visibleLimitsUpdateNeeded = true;

        drawDirty = true;

        return;
    }

  /**
  Remove everything from the display.  This is useful when you are
  changing to a completely different set of faults, horizons and lines.
  If you are not going to redraw new data immediately, you should
  set the repaint flag to true, and an empty bounding cube will be drawn.
  If you are going to redraw immediately, set the repaint flag to false
  to prevent flashing of the view.
  */
    public void clearAll (boolean repaintFlag)
    {
        clearFaults ();
        clearHorizons ();
        clearLines ();
        visibleXmin = 1.e30;
        visibleYmin = 1.e30;
        visibleZmin = 1.e30;
        visibleXmax = -1.e30;
        visibleYmax = -1.e30;
        visibleZmax = -1.e30;
        if (externalLimitsFlag) {
            visibleXmin = externalXmin;
            visibleYmin = externalYmin;
            visibleZmin = externalZmin;
            visibleXmax = externalXmax;
            visibleYmax = externalYmax;
            visibleZmax = externalZmax;
        }
        if (repaintFlag) {
            localRepaint ();
        }
    }

/*---------------------------------------------------------------------------------*/

  /**
  Remove all of the "permanent" lines from the display.
  */
    public void clearLines ()
    {
        if (lineList == null)
          return;

        lineList.clear ();
    }

/*---------------------------------------------------------------------------------*/

  /**
  Remove all of the fault patches from the display.
  */
    public void clearFaults ()
    {
        if (faultList == null)
          return;

        J3DFault surf;
        int size = faultList.size ();
        for (int i=0; i<size; i++) {
            surf = faultList.get(i);
            if (surf.wireListID > 0) {
                jgl.DeleteList (surf.wireListID);
            }
            if (surf.solidListID > 0) {
                jgl.DeleteList (surf.solidListID);
            }
        }

        faultList.clear ();
        previewFault = null;
    }

/*---------------------------------------------------------------------------------*/

  /**
  Remove all of the horizon patches from the display.
  */
    public void clearHorizons ()
    {
        if (horizonList == null)
          return;

        J3DSurfacePatch surf;
        int size = horizonList.size ();
        for (int i=0; i<size; i++) {
            surf = horizonList.get(i);
            if (surf.wireListID > 0) {
                jgl.DeleteList (surf.wireListID);
            }
            if (surf.solidListID > 0) {
                jgl.DeleteList (surf.solidListID);
            }
        }

        horizonList.clear ();
        previewHorizonPatch = null;
    }

/*---------------------------------------------------------------------------------*/

  /**
  Return the J3DSurface that has the specified trimesh data.  The first
  matching surface found is returned.  If no match is found, null is returned.
  */
    public J3DSurfacePatch getSurfaceForTriMeshData (TriMesh tmesh)
    {
        if (tmesh == null) {
            return null;
        }

        J3DHorizonPatch      hp;
        J3DFault             fp;
        int                  i, size;

        if (horizonList != null) {
            size = horizonList.size();
            for (i=0; i<size; i++) {
                hp = horizonList.get (i);
                if (hp == null) {
                    continue;
                }
                if (hp.getTriMesh().equals(tmesh)) {
                    return hp;
                }
            }
        }

        if (faultList != null) {
            size = faultList.size();
            for (i=0; i<size; i++) {
                fp = faultList.get (i);
                if (fp == null) {
                    continue;
                }
                if (fp.getTriMesh().equals(tmesh)) {
                    return fp;
                }
            }
        }

        return null;

    }

/*---------------------------------------------------------------------------------*/

  /**
  Return the J3DSurface that has the specified callback data.  The first
  matching surface found is returned.  If no match is found, null is returned.
  */
    public J3DSurfacePatch getSurfaceForCallbackData (J3DCallbackObject cbdata)
    {

/*
        if (cbdata == null) {
            return null;
        }

        J3DHorizonPatch      hp;
        J3DFault        fp;
        int                  i, size;

        if (horizonList != null) {
            size = horizonList.size();
            for (i=0; i<size; i++) {
                hp = horizonList.get (i);
                if (hp == null) {
                    continue;
                }
                if (hp.getCallbackData().equals(cbdata)) {
                    return hp;
                }
            }
        }

        if (faultList != null) {
            size = faultList.size();
            for (i=0; i<size; i++) {
                fp = faultList.get (i);
                if (fp == null) {
                    continue;
                }
                if (fp.getCallbackData().equals(cbdata)) {
                    return fp;
                }
            }
        }
*/

        return null;

    }

/*---------------------------------------------------------------------------------*/

  /**
  Return the J3DLine that has the specified callback data.  The first
  matching surface found is returned.  If no match is found, null is returned.
  */
    public J3DLine getLineForCallbackData (J3DCallbackObject cbdata)
    {
/*
        if (cbdata == null) {
            return null;
        }

        J3DLine     lp;
        int         i, size;

        if (lineList != null) {
            size = lineList.size();
            for (i=0; i<size; i++) {
                lp = lineList.get (i);
                if (lp == null) {
                    continue;
                }
                if (lp.getCallbackData().equals(cbdata)) {
                    return lp;
                }
            }
        }
*/

        return null;
    }

    /*---------------------------------------------------------------------------------*/

    void unsetDisplayLists ()
    {

        int                i, n;
        J3DFault      fp;

        if (faultList != null) {
            n = faultList.size();
            for (i=0; i<n; i++) {
                fp = faultList.get(i);
                if (fp == null) {
                    continue;
                }
                if (fp.wireListID > 0) {
                    fp.wireListID = -fp.wireListID;
                }
                if (fp.solidListID > 0) {
                    fp.solidListID = -fp.solidListID;
                }
            }
        }

        J3DHorizonPatch    hp;

        if (horizonList != null) {
            n = horizonList.size();
            for (i=0; i<n; i++) {
                hp = horizonList.get(i);
                if (hp == null) {
                    continue;
                }
                if (hp.wireListID > 0) {
                    hp.wireListID = -hp.wireListID;
                }
                if (hp.solidListID > 0) {
                    hp.solidListID = -hp.solidListID;
                }
            }
        }

    }

  /**
   * Mark all the 3d graphical objects for deletion.  This is the first pass
   * of the mark and sweep cleanup process.  The application controller will
   * call this, and then it will unmark those graphical objects which have
   * valid callback objects associated with them.  This should almost always
   * be called by the {@link J3DController} class located in the csw/controllers
   * package.  Unless you understand this extremely well, do not call this method.
   */
    public void markForDeletion ()
    {

        J3DElement elem;
        int size = faultList.size ();
        for (int i=0; i<size; i++) {
            elem = (J3DElement)faultList.get(i);
            elem.markedForDelete = true;
        }

        size = horizonList.size ();
        for (int i=0; i<size; i++) {
            elem = (J3DElement)horizonList.get(i);
            elem.markedForDelete = true;
        }

        size = lineList.size ();
        for (int i=0; i<size; i++) {
            elem = (J3DElement)lineList.get(i);
            elem.markedForDelete = true;
        }

    }

/*---------------------------------------------------------------------------------*/

  /**
  The surfaces and lines that still have their markedForDeletion flag true are removed
  from their respective lists.
  */
    public void cleanupMarkedForDeletion ()
    {

        J3DSurfacePatch surf;
        J3DLine line;
        int size = faultList.size ();
        for (int i=size-1; i>=0; i--) {
            surf = (J3DSurfacePatch)faultList.get(i);
            if (surf.markedForDelete == true) {
                if (surf.wireListID > 0) {
                    jgl.DeleteList (surf.wireListID);
                    surf.wireListID = -surf.wireListID;
                }
                if (surf.solidListID > 0) {
                    jgl.DeleteList (surf.solidListID);
                    surf.solidListID = -surf.solidListID;
                }
                faultList.remove (i);
            }
        }

        size = horizonList.size ();
        for (int i=size-1; i>=0; i--) {
            surf = (J3DSurfacePatch)horizonList.get(i);
            if (surf.markedForDelete == true) {
                if (surf.wireListID > 0) {
                    jgl.DeleteList (surf.wireListID);
                    surf.wireListID = -surf.wireListID;
                }
                if (surf.solidListID > 0) {
                    jgl.DeleteList (surf.solidListID);
                    surf.solidListID = -surf.solidListID;
                }
                horizonList.remove (i);
            }
        }

        size = lineList.size ();
        for (int i=size-1; i>=0; i--) {
            line = lineList.get(i);
            if (line.markedForDelete == true) {
                lineList.remove (i);
            }
        }

    }


    private void setJGLBackgroundColor ()
    {
      Color _bgc = null;
      if (view3DProperties != null  &&  canvas != null) {
        _bgc = view3DProperties.getBackgroundColor();
      }
      if (_bgc != null) {
        double r = _bgc.getRed() / 255.0;
        double g = _bgc.getGreen() / 255.0;
        double b = _bgc.getBlue() / 255.0;
        jgl.DGBackColor (r, g, b, 1.0);
      }
      else {
        float[] rgb = backgroundColor.getRGBColorComponents(null);
        double r = rgb[0];
        double g = rgb[1];
        double b = rgb[2];
        jgl.DGBackColor (r, g, b, 1.0);
      }
    }


/*--------------The GLEventListener interface (JOGL)-------------------*/

    private boolean displayActive = false;

  /**
  Method used only by jogl, do not call this directly.
   */
    public void init (GLAutoDrawable drawable)
    {

    }

  /**
  Method used only by jogl, do not call this directly.
   */
    public void display (GLAutoDrawable drawable)
    {

        if (drawDirty == false) {
            return;
        }

        drawingAttributes.setViewProperties (view3DProperties);

        boolean   cmove = false;

        GL gl = drawable.getGL();
        GLU glu = new GLU();
        jgl.setGL (gl, glu);

        jgl.InitializeState ();
        jgl.InitializeMaterialProperties ();
        jgl.InitializeLighting ();

        setJGLBackgroundColor ();

        boolean doDraw = true;

        if (pickScheduled  &&  pickMouseEvent != null) {
          double aspectRatio = (double)iwidth / (double)iheight;
            doDraw = tool.pickObject (pickMouseEvent, aspectRatio);
            if (doDraw) {
                if (targetZoomFlag) {
                    setupTargetZoom (pickMouseEvent);
                }
            }
            pickMouseEvent = null;
            listDrawFlag = false;
            if (targetZoomFlag) {
                setPickModeMessage (null);
                cmove = true;
                targetZoomFlag = false;
            }
        }

        if (doDraw) {
            drawMain ();
        }

        jgl.FlushGX ();
        jgl.FinishGX ();

        pickScheduled = false;

        if (cmove) {
            centerCursor ();
        }

    }



  /**
  Method used only by jogl, do not call this directly.
   */
    public void dispose (GLAutoDrawable  ad) 
    {
    }



  /**
  Method used only by jogl, do not call this directly.
   */
    public void reshape (GLAutoDrawable drawable,
                         int ix, int iy, int width, int height)
    {

        drawDirty = true;

        if (fromPickMessage) {
            fromPickMessage = false;
            return;
        }

        GL gl = drawable.getGL();
        GLU glu = new GLU();
        jgl.setGL (gl, glu);

        glInit = true;

        iwidth = width;
        iheight = height;

        setJGLBackgroundColor ();

        jgl.ClearColorAndDepthBuffers ();

        jgl.FlushGX ();
        jgl.FinishGX ();

    }

  /**
  Method used only by jogl, do not call this directly.
   */
    public void displayChanged (GLAutoDrawable drawable,
                                boolean modeChanged,
                                boolean deviceChanged)
    {
    }
/*-----------------Repaint methods-----------------------------------------*/
    boolean repaintFromExpose = false;
    private boolean glInit = false;
    private boolean doRepaint = true;

    public void redrawAllImmediately ()
    {
        if (glInit == false) {
            return;
        }

        if (doRepaint == false) {
            return;
        }

        drawDirty = true;

        paintImmediately(0, 0, getWidth(), getHeight());
    }

    public void redrawAll ()
    {
        localRepaint ();
    }

/*-----------------------------------------------------------------------------*/

/*
 * Call the jogl redisplay method directly, without firing a repaint event.
 * This is needed to quickly respond to "animation" type requests such as
 * panning and rotating.  If the canvas repaint method is called, several
 * other events may be in the event queue and delay the repaint.  This delay
 * makes the pan or rotate look choppy at times.
 *
 * If several methods need to be called for an operation, and each method
 * may call this as a side effect, you can turn off the redraw by setting
 * the doRepaint boolean to false.  After all of the methods have been called,
 * reset doRepaint to true and explicitly call this method.
 *
 * For picking, it seems that the repaint is needed to avoid an exception
 * in the jogl code.
 */

    void localRepaint ()
    {
        if (glInit == false) {
            return;
        }

        if (doRepaint == false) {
            return;
        }

        drawDirty = true;

        repaint ();
    }
/*--------------------------------------------------------------------------*/

/*
 * This method redraws the 3d scene in response to an external repaint call.
 */
    public void paintComponent (Graphics g)
    {

    /*
     * Call the restart method, which draws the 3d scene in its own
     * thread.  If the original repaint was from a simple expose event,
     * the window sometimes has garbage left in it after drawing.  Draw
     * a second time via a second repaint in this case.
     */
        if (canvas != null) {
            restart ();
            if (repaintFromExpose) {
                repaintFromExpose = false;
                restart ();
            }
        }

    }

/*------------------------ Drawing Methods ----------------------------*/

    int                       totalTriangles = 0;
    private int               iwidth, iheight;

    private boolean           drawDirty = true;
    private boolean           isMoving = false;

    private JFrame            parentFrame = null;

    private int               isListening = 0;

    private double            externalXmin = 1.e30,
                              externalYmin = 1.e30,
                              externalZmin = 1.e30,
                              externalXmax = -1.e30,
                              externalYmax = -1.e30,
                              externalZmax = -1.e30;
    private boolean           externalLimitsFlag = false;
    private double            visibleXmin = 1.e30,
                              visibleYmin = 1.e30,
                              visibleZmin = 1.e30,
                              visibleXmax = -1.e30,
                              visibleYmax = -1.e30,
                              visibleZmax = -1.e30;
    boolean                   visibleLimitsUpdateNeeded = true;

    protected Scaler            scaler;
    private double            dTiny = 0.0;

    private static final double PI = 3.1415926;
    private static final double DTOR = PI / 180.0;

    BoundingBoxDraw3D         boundingBox;

    private int               nextWireListID,
                              nextSolidListID;

    private double            xyUnits = 1.0;
    private double            zUnits = 1.0;

    private boolean listDrawFlag = false;
    boolean opaqueFlag = false;

/*-------------------------------------------------------------------------*/

/**
 Set the limits of the 3d view in x, y and z.  This bounding box is
 used for scaling all subsequent surfaces to the 3d view.  If this is
 not called, the limits are calculated from the horizons, faults and
 external surfaces defined for the view.  A food use for this method is
 to set the limits to the entire extent of a model.  If you do this,
 redrawing subsets of the model will not change the shape of the
 bounding box.
*/
    public void setVisibleLimits (
        double    xmin,
        double    ymin,
        double    zmin,
        double    xmax,
        double    ymax,
        double    zmax)
    {
        visibleXmin = xmin;
        visibleYmin = ymin;
        visibleZmin = zmin;
        visibleXmax = xmax;
        visibleYmax = ymax;
        visibleZmax = zmax;
        externalLimitsFlag = true;
    }

/**
 This method disables any limits set via the {@link #setVisibleLimits}
 method and puts the view back into the mode of calculating its own
 limits based on the surfaces to be viewed.
*/
    public void unsetVisibleLimits ()
    {
        visibleXmin = 1.e30;
        visibleYmin = 1.e30;
        visibleZmin = 1.e30;
        visibleXmax = -1.e30;
        visibleYmax = -1.e30;
        visibleZmax = -1.e30;
        externalLimitsFlag = false;
    }

/**
 * Return the maximum distance from a point which would
 * be considered "on" the point
 */
    public double getTolerance (int tfact)
    {
        double tol;
        
        if (visibleXmin >= visibleXmax) {
            return 0.0;
        }

        tol = (visibleXmax - visibleXmin +
               visibleYmax - visibleYmin) / 150.0;
        if (tfact > 0) tol *= tfact;

        return tol;
    }

/*-------------------------------------------------------------------------*/

    private void drawMain ()
    {

        Date date = new Date ();
        long t1 = date.getTime ();

    /*
     * Update the limits of the model if anything has been
     * added or deleted.
     */
        updateVisibleLimits ();

        setJGLBackgroundColor ();

        jgl.ClearColorAndDepthBuffers ();

        setDrawMatrices ();

        setupLighting ();
        if (boundingBox == null) {
            createEmptyBorder ();
        }
        boundingBox.setNumbersFont(drawingAttributes.getAxisNumbersFont());
        boundingBox.setLabelsFont(drawingAttributes.getAxisLabelsFont());

        if (drawingAttributes.getDoSides()) {
            boundingBox.scaleSides (backgroundColor, jgl);
        }

        if (drawingAttributes.getDoBox()) {
            boundingBox.scaleBox (backgroundColor, jgl);
        }

        boundingBox.setAxesFlags (
            drawingAttributes.getDoAxes(),
            drawingAttributes.getDoAxesLabels(),
            drawingAttributes.getDoAxesNumbers());
        boundingBox.scaleAxes (jgl);

    /*
     * Draw the components according to the motion redraw flag.
     */
        if (isMoving == true) {
            if (drawingAttributes.getMotionRedraw() == true) {
                drawComponentsDisplay ();
            }
        }
        else {
            drawComponentsDisplay ();
        }

        if (drawingAttributes.getDoDrawLights()) {
            drawLights (backgroundColor);
        }

    /*
     * Draw the titles using an orthographic projection.
     */
        if (drawingAttributes.getDoDrawTitles()) {
            drawTitles (backgroundColor);
        }

    /*
     * Draw the vertical exaggeration using an orthographic projection.
     */
        if (drawingAttributes.getDoDrawExagerration()) {
            drawExaggerationTitle(backgroundColor);
        }

    /*
     * Draw the compass using an orthographic projection.
     */
        if (drawingAttributes.getDoCompass()) {
            drawCompass (backgroundColor);
        }

        drawDirty = false;

        date = new Date ();
        long t2 = date.getTime ();

        int delta = (int)(t2 - t1);

//System.out.println ("drawing time in milliseconds = "+delta);

    }

/*---------------------------------------------------------------------------------*/

  /*
   * Separate out the matrix setting part of the draw so it can be
   * used more easily in both draw and pick modes.
   */
    void setDrawMatrices ()
    {

        int       x1, y1, w, h;

        jgl.MatrixModeModel ();
        jgl.LoadIdentity ();
        jgl.TranslateD (xTrans, yTrans, zTrans);
        jgl.RotateD (xRot, 1.0, 0.0, 0.0);
        jgl.RotateD (yRot, 0.0, 1.0, 0.0);
        jgl.RotateD (zRot, 0.0, 0.0, 1.0);
        jgl.ScaleD (xScale, yScale, zScale);
        jgl.SetupViewport (iwidth, iheight);

        double aspect = (double)iwidth / (double)iheight;
        jgl.SetupViewingFrustum (aspect, projectionMode);

    }

/*------------------------------------------------------------------------------*/

    public void setXYZUnits (double xy, double z)
    {
        xyUnits = xy;
        zUnits = z;
        visibleLimitsUpdateNeeded = true;
        drawDirty = true;
    }

/*---------------------------------------------------------------------------------*/

    void resetToDefaultTransform ()
    {
        resetToHomeView ();
    }

/*---------------------------------------------------------------------------------*/

    private void setupLighting ()
    {
    /*
     * Set up the lighting if needed.
     */
        LightData light1 = drawingAttributes.getViewProperties().getLight1();
        if (light1.isEnabled()) {
          // note that passing in X,Z,Y is correct ordering
            jgl.UpdateLighting (light1.getX(), light1.getZ(), -light1.getY());
            jgl.SetLightBrightness (light1.getBrightness());
            jgl.SetLightEnabled (1);
        }
        else {
            jgl.SetLightEnabled (0);
        }

        LightData light2 = drawingAttributes.getViewProperties().getLight2();
        if (light2.isEnabled()) {
          // note that passing in X,Z,Y is correct ordering
            jgl.UpdateLighting2 (light2.getX(), light2.getZ(), -light2.getY());
            jgl.SetLight2Brightness (light2.getBrightness());
            jgl.SetLight2Enabled (1);
        }
        else {
            jgl.SetLight2Enabled (0);
        }

        LightData light3 = drawingAttributes.getViewProperties().getLight3();
        if (light3.isEnabled()) {
          // note that passing in X,Z,Y is correct ordering
            jgl.UpdateLighting3 (light3.getX(), light3.getZ(), -light3.getY());
            jgl.SetLight3Brightness (light3.getBrightness());
            jgl.SetLight3Enabled (1);
        }
        else {
            jgl.SetLight3Enabled (0);
        }

        LightData light4 = drawingAttributes.getViewProperties().getLight4();
        if (light4.isEnabled()) {
          // note that passing in X,Z,Y is correct ordering
            jgl.UpdateLighting4 (light4.getX(), light4.getZ(), -light4.getY());
            jgl.SetLight4Brightness (light4.getBrightness());
            jgl.SetLight4Enabled (1);
        }
        else {
            jgl.SetLight4Enabled (0);
        }
    }

/*-------------------------------------------------------------------------------*/
    /**
     * Draw components for display.
     */
    private void drawComponentsDisplay ()
    {

        jgl.SetDrawDepthTest (1);
        jgl.UpdateMatrices ();

        if (drawingAttributes.getDoLighting()) {
            jgl.SetDrawLighting (1);
        }
        else {
            jgl.SetDrawLighting (0);
        }

        if (drawingAttributes.getDoShading()) {
            jgl.SetDrawShading (1);
        }
        else {
            jgl.SetDrawShading (0);
        }

        /*
         * Initialize the total number of triangles
         */
        totalTriangles = 0;

        /*
         * Draw opaque components.
         */
        opaqueFlag = true;
        drawFaults();
        drawFaultSurface (previewFault);
        drawHorizons();
        drawHorizonSurface (previewHorizonPatch);

        /*
         * Draw the lines and text.
         */
        drawLines();
        drawText();
       /*
        * If there are temporary edit points and lines, redraw them.
        */
        drawEditPoints ();

        /*
         * Draw transparent components.
         */
        opaqueFlag = false;
        drawFaults();
        drawFaultSurface (previewFault);
        drawHorizons();
        drawHorizonSurface (previewHorizonPatch);


//System.out.println ("total triangles = "+totalTriangles);
    }
/*----------------------------------------------------------------------*/

    void drawHorizons() {
      int i, n;
      J3DHorizonPatch hp;

      initNames ();
      pushName (J3DConst.HORIZON_SURFACE_NAME);
      pushName (0);

     /*
      * Draw opaque horizons.
      */
      if (horizonList != null) {
          n = horizonList.size();
          for (i=0; i<n; i++) {
              hp = horizonList.get(i);
              if (hp == null) {
                  continue;
              }
              if (hp.isVisible() == false) {
                  continue;
              }
              loadName (i);
              drawHorizonSurface (hp);
          }
      }

    }

    void drawFaults() {
        int i, n;
        J3DFault fp;

        initNames ();
        pushName (J3DConst.FAULT_SURFACE_NAME);
        pushName (0);

       /*
        *  Draw opaque faults.
        */
        if (faultList != null) {
            n = faultList.size();
            for (i=0; i<n; i++) {
                fp = faultList.get(i);
                if (fp == null) {
                    continue;
                }
                if (fp.isVisible() == false) {
                    continue;
                }
                loadName (i);
                drawFaultSurface (fp);
            }
        }
    }

    void drawLines() {
        int i, n;
        J3DLine    lp;
        initNames();
        pushName (J3DConst.LINE3D_NAME);
        pushName (0);
        if (lineList != null) {
            n = lineList.size ();
            for (i=0; i<n; i++) {
                lp = lineList.get (i);
                if (lp == null) {
                    continue;
                }
                if (lp.isVisible() == false) {
                    continue;
                }
                loadName (i);
                drawJ3DLine (lp);
            }
        }
    }

    private void drawText() {
        int i, n;
        J3DText tp;
        initNames();
        pushName (J3DConst.TEXT3D_NAME);
        pushName (0);
        if (textList != null) {
            n = textList.size();
            for (i=0; i<n; i++) {
                tp = textList.get (i);
                if (tp == null) {
                    continue;
                }
                if (tp.isVisible() == false) {
                    continue;
                }
                loadName (i);
                drawJ3DText (tp);
            }
        }
    }

/*----------------------------------------------------------------------*/

    public void setUpdateVisibleLimits (boolean ival)
    {
        visibleLimitsUpdateNeeded = ival;
    }

/*----------------------------------------------------------------------*/

    private void updateVisibleLimits ()
    {
        int                i, n;
        J3DFault           fp;
        J3DHorizonPatch    hp;
        J3DLine            lp;
        J3DText            tp;
        TriMesh            tmesh;
        double             xmin, ymin, zmin,
                           xmax, ymax, zmax;

        if (isPropertyApply == false) {
          if (visibleLimitsUpdateNeeded == false) {
            return;
          }
          if (externalLimitsFlag) {
            return;
          }
        }

      /*
       * Save the old visible limits.
       */
        double oldVisibleXmin = visibleXmin;
        double oldVisibleYmin = visibleYmin;
        double oldVisibleZmin = visibleZmin;
        double oldVisibleXmax = visibleXmax;
        double oldVisibleYmax = visibleYmax;
        double oldVisibleZmax = visibleZmax;

        visibleXmin = 1.e30;
        visibleYmin = 1.e30;
        visibleZmin = 1.e30;
        visibleXmax = -1.e30;
        visibleYmax = -1.e30;
        visibleZmax = -1.e30;

      /*
       * Find the cumulative limits of the faults.
       */
        if (faultList != null) {
            n = faultList.size();
            for (i=0; i<n; i++) {
                fp = faultList.get(i);
                if (fp == null) {
                    continue;
                }
                tmesh = fp.triMesh;
                if (tmesh == null) {
                    if (fp.tm3d == null) {
                        continue;
                    }
                    xmin = fp.tm3d.xmin;
                    ymin = fp.tm3d.ymin;
                    zmin = fp.tm3d.zmin;
                    xmax = fp.tm3d.xmax;
                    ymax = fp.tm3d.ymax;
                    zmax = fp.tm3d.zmax;
                    if (xmin > xmax  ||
                        ymin > ymax  ||
                        zmin > zmax) {
                        continue;
                    }
                }
                else {
                    xmin = tmesh.getXMin();
                    ymin = tmesh.getYMin();
                    zmin = tmesh.getZMin();
                    xmax = tmesh.getXMax();
                    ymax = tmesh.getYMax();
                    zmax = tmesh.getZMax();
                }
                if (xmin < visibleXmin) visibleXmin = xmin;
                if (ymin < visibleYmin) visibleYmin = ymin;
                if (
                  zmin < visibleZmin &&
                  Math.abs(zmin) < REASONABLE_Z_VALUE_LIMIT
                ) {
                  visibleZmin = zmin;
                }
                if (xmax > visibleXmax) visibleXmax = xmax;
                if (ymax > visibleYmax) visibleYmax = ymax;
                if (
                  zmax > visibleZmax &&
                  Math.abs(zmax) < REASONABLE_Z_VALUE_LIMIT
                ) {
                  visibleZmax = zmax;
                }
            }
        }

      /*
       * Find the cumulative limits of the horizons.
       */
        if (horizonList != null) {
            n = horizonList.size();
            for (i=0; i<n; i++) {
                hp = horizonList.get(i);
                if (hp == null) {
                    continue;
                }
                tmesh = hp.triMesh;
                if (tmesh == null) {
                    if (hp.tm3d == null) {
                        continue;
                    }
                    xmin = hp.tm3d.xmin;
                    ymin = hp.tm3d.ymin;
                    zmin = hp.tm3d.zmin;
                    xmax = hp.tm3d.xmax;
                    ymax = hp.tm3d.ymax;
                    zmax = hp.tm3d.zmax;
                    if (xmin > xmax  ||
                        ymin > ymax  ||
                        zmin > zmax) {
                        continue;
                    }
                }
                else {
                    xmin = tmesh.getXMin();
                    ymin = tmesh.getYMin();
                    zmin = tmesh.getZMin();
                    xmax = tmesh.getXMax();
                    ymax = tmesh.getYMax();
                    zmax = tmesh.getZMax();
                }
                if (xmin < visibleXmin) visibleXmin = xmin;
                if (ymin < visibleYmin) visibleYmin = ymin;
                if (
                  zmin < visibleZmin &&
                  Math.abs(zmin) < REASONABLE_Z_VALUE_LIMIT
                ) {
                  visibleZmin = zmin;
                }
                if (xmax > visibleXmax) visibleXmax = xmax;
                if (ymax > visibleYmax) visibleYmax = ymax;
                if (zmax > visibleZmax) visibleZmax = zmax;
            }
        }

      /*
       * If any of the limits has changed, set the changed flag to true.
       */
        boolean isVisibleLimitsChanged = false;
        if (oldVisibleXmax != visibleXmax)
            isVisibleLimitsChanged = true;
        else if (oldVisibleYmax != visibleYmax)
            isVisibleLimitsChanged = true;
        else if (oldVisibleZmax != visibleZmax)
            isVisibleLimitsChanged = true;
        else if (oldVisibleXmin != visibleXmin)
            isVisibleLimitsChanged = true;
        else if (oldVisibleYmin != visibleYmin)
            isVisibleLimitsChanged = true;
        else if (oldVisibleZmin != visibleZmin)
            isVisibleLimitsChanged = true;

        if (isVisibleLimitsChanged) {

          /*
           * Disable the saved display lists for the faults.
           */
            if (faultList != null) {
              n = faultList.size();
              for (i=0; i<n; i++) {
                  fp = faultList.get(i);
                  if (fp == null) {
                      continue;
                  }
                  if (fp.getTriMesh () != null) {
                      fp.tm3d = null;
                  }
                  fp.disableLists();
              }
            }

          /*
           * Disable the saved display lists for the horizons.
           */
            if (horizonList != null) {
                n = horizonList.size();
                for (i=0; i<n; i++) {
                    hp = horizonList.get(i);
                    if (hp == null) {
                        continue;
                    }
                    if (hp.getTriMesh () != null) {
                        hp.tm3d = null;
                    }
                    hp.disableLists();
                }
            }

        }

        visibleLimitsUpdateNeeded = false;

        scaler.setLimits (visibleXmin,
                          visibleYmin,
                          visibleZmin,
                          visibleXmax,
                          visibleYmax,
                          visibleZmax,
                          drawingAttributes.getVerticalExageration()
                         );
        dTiny = (visibleXmax - visibleXmin +
                 visibleYmax - visibleYmin +
                 visibleZmax - visibleZmin) / 100.0;

    /*
     * Create new bounding "cube" objects for the new limits
     */
        createBorder ();

    }

/*----------------------------------------------------------------------*/

    private void createBorder ()
    {
        if (visibleXmin >= visibleXmax  ||
            visibleYmin >= visibleYmax  ||
            visibleZmin >= visibleZmax) {
            return;
        }

        BoundingBoxDraw3D bb =
        new BoundingBoxDraw3D (xyUnits, zUnits,
                               visibleXmin, visibleYmin, visibleZmin,
                               visibleXmax, visibleYmax, visibleZmax,
                               drawingAttributes.getAxisLabelsFont(),
                               drawingAttributes.getAxisNumbersFont());
        bb.setScaler (scaler);

        boundingBox = bb;

        visibleXmin = bb.getPrettyXmin ();
        visibleYmin = bb.getPrettyYmin ();
        visibleZmin = bb.getPrettyZmin ();
        visibleXmax = bb.getPrettyXmax ();
        visibleYmax = bb.getPrettyYmax ();
        visibleZmax = bb.getPrettyZmax ();

        scaler.setLimits (visibleXmin,
                          visibleYmin,
                          visibleZmin,
                          visibleXmax,
                          visibleYmax,
                          visibleZmax,
                          drawingAttributes.getVerticalExageration()
                         );

        dTiny = (visibleXmax - visibleXmin +
                 visibleYmax - visibleYmin +
                 visibleZmax - visibleZmin) / 100.0;

        return;

    }

/*----------------------------------------------------------------------*/

    private void createEmptyBorder ()
    {
        BoundingBoxDraw3D bb =
        new BoundingBoxDraw3D (1.0, 1.0, -.8, -.8, -.8, .8, .8, .8,
            drawingAttributes.getAxisLabelsFont(),
            drawingAttributes.getAxisNumbersFont());
        bb.setScaler (scaler);

        boundingBox = bb;

        visibleXmin = bb.getPrettyXmin ();
        visibleYmin = bb.getPrettyYmin ();
        visibleZmin = bb.getPrettyZmin ();
        visibleXmax = bb.getPrettyXmax ();
        visibleYmax = bb.getPrettyYmax ();
        visibleZmax = bb.getPrettyZmax ();

        scaler.setLimits (visibleXmin,
                          visibleYmin,
                          visibleZmin,
                          visibleXmax,
                          visibleYmax,
                          visibleZmax,
                          1.0
                         );

        dTiny = (visibleXmax - visibleXmin +
                 visibleYmax - visibleYmin +
                 visibleZmax - visibleZmin) / 100.0;

        return;

    }

/*----------------------------------------------------------------------*/

    void drawHorizonSurface (J3DHorizonPatch hp)
    {
        int             i;

        if (linePickFlag == true) {
            return;
        }

        if (hp == null) {
            return;
        }

        if (hp.isVisible() == false) {
            return;
        }

        TriMesh tmesh = hp.getTriMesh ();
        if (tmesh == null) {
            if (hp.tm3d == null) {
                return;
            }
        }
        Color tmcolor = hp.getGlobalColor ();
        if (tmcolor == null) {
            return;
        }

        Color c = tmcolor;
        tmcolor = new Color (c.getRed(),
                             c.getGreen(),
                             c.getBlue(),
                             c.getAlpha()
                            );

        if (hp.isSelected()) {
            tmcolor = selectionColor;
        }

        if (tmcolor.getAlpha() < 255  && opaqueFlag == true) {
            return;
        }

        if (tmcolor.getAlpha() == 255  &&opaqueFlag == false) {
            return;
        }

        if (
          drawingAttributes.getDoSolidFill() == false  &&
          drawingAttributes.getDoWireFrame() == false
        ) {
            return;
        }

        TriMeshDraw3D tm3d;

        tm3d = hp.tm3d;
        if (tm3d == null) {
            tm3d = new TriMeshDraw3D (tmesh);
            hp.tm3d = tm3d;
        }
        tm3d.setScaler (scaler);

        if (colorLookup == null) {
          tm3d.setColorByAttribute (false);
          tm3d.setColorLookup (null);
          tm3d.setNodeAttributeArray (null);
          tm3d.updateAttributeColors ();
        }
        else {
          tm3d.setColorByAttribute (true);
          tm3d.setColorLookup (colorLookup);
          tm3d.setNodeAttributeArray (null);
          tm3d.updateAttributeColors ();
        }
        tm3d.setLists (hp.wireListID, hp.solidListID);

        totalTriangles += tm3d.numTriangles;

        if (drawingAttributes.getDoSolidFill()  ||  selectionMode == true) {

            jgl.SetOffsetFill ();
            tm3d.setColor (tmcolor);
            jgl.SetDrawMode (JGL.DG_TRIANGLES);
            jgl.SetDrawStyle (JGL.DG_MESH);
            tm3d.scaleAndDrawTriangles (visibleXmin, visibleYmin, visibleZmin,
                                      visibleXmax, visibleYmax, visibleZmax,
                                      drawingAttributes.getVerticalExageration(),
                                      tmcolor,
                                      true, this, jgl);

            if (selectionMode == false) {
                int itmp = tm3d.getLastList ();
                if (itmp > 0) {
                    hp.solidListID = itmp;
                }
            }

        }

        if (drawingAttributes.getDoWireFrame()  &&  selectionMode == false) {

            jgl.UnsetOffsetFill ();
            jgl.SetDrawLineWidth (1.0);
            jgl.SetDrawLighting (0);
            jgl.SetDrawShading (0);
            tm3d.setColorByAttribute (false);
            jgl.SetDrawMode (JGL.DG_TRIANGLES);
            jgl.SetDrawStyle (JGL.DG_WIREFRAME);

          /*
           * If the solid fill is also drawn, contrast the wireframe
           * color with the solid fill color.
           */
            Color wcolor;
            if (drawingAttributes.getDoSolidFill()) {
                int ir = tmcolor.getRed ();
                int ig = tmcolor.getGreen ();
                int ib = tmcolor.getBlue ();
                int icmin = ir;
                int icmax = ir;
                if (ig < icmin) icmin = ig;
                if (ig > icmax) icmax = ig;
                if (ib < icmin) icmin = ib;
                if (ib > icmax) icmax = ib;
                icmax -= icmin;
                int idark;
                if (icmax < 10) {
                    idark = 400;
                }
                else if (icmax < 50) {
                    idark = 350;
                }
                else if (icmax < 100) {
                    idark = 300;
                }
                else if (icmax < 150) {
                    idark = 250;
                }
                else {
                    idark = 150;
                }
                if (ir+ig+ib > idark) {
                    wcolor = new Color (20, 20, 20, 255);
                }
                else {
                    wcolor = new Color (220, 220, 220, 255);
                }
            }

          /*
           * No solid color is drawn, so use the solid color for the
           * wireframe.
           */
            else {
                wcolor = tmcolor;
            }

            tm3d.scaleAndDrawTriangles (visibleXmin, visibleYmin, visibleZmin,
                                        visibleXmax, visibleYmax, visibleZmax,
                                        drawingAttributes.getVerticalExageration(),
                                        wcolor,
                                        false, this, jgl);
            jgl.UnsetOffsetFill ();
            if (drawingAttributes.getDoLighting()) {
                jgl.SetDrawLighting (1);
            }
            if (drawingAttributes.getDoShading()) {
                jgl.SetDrawShading (1);
            }

            int itmp = tm3d.getLastList ();
            if (itmp > 0) {
                hp.wireListID = itmp;
            }

        }

        return;
    }

 /*-------------------------------------------------------------------------------*/

    void drawJ3DLine (J3DLine lp)
    {

        int          size, i;
        XYZPolyline    atmp;
        LineDraw3D   l3d;
        Color        lcolor;
        int          pointFlag;

        if (lp == null) {
            return;
        }

        if (lp.isVisible() == false) {
            return;
        }

        lcolor = lp.getGlobalColor ();
        if (lcolor == null) {
            return;
        }

        if (lp.isSelected()) {
            lcolor = lineSelectionColor;
        }

        if (lp.hidden == false) {
            jgl.SetDrawDepthTest (0);
        }

        pointFlag = lp.drawAsPoints;

    /*
     * The fifth element in the "name" stack identifies this as
     * a line (0) or fill (1).  This is needed in the hit processing
     * done in the JGL object.  So, the stack is extended three places
     * to get the zero in the fifth element.
     */
        pushName (0);
        pushName (0);
        pushName (0);

    /*
     * Draw the draped line if available.
     */
        if (pointFlag != 1) {
            if (lp.drapedLineList != null) {
                size = lp.drapedLineList.size ();
                for (i=0; i<size; i++) {
                    atmp = lp.drapedLineList.get(i);
                    if (atmp == null) {
                        continue;
                    }
                    l3d = new LineDraw3D (atmp);
                    l3d.setScaler (scaler);
                    l3d.scaleAndDrawLine (
                        visibleXmin, visibleYmin, visibleZmin,
                        visibleXmax, visibleYmax, visibleZmax,
                        drawingAttributes.getVerticalExageration(),
                        lcolor,
                        lp.lineWidth,
                        lp.dashFlag,
                        this,
                        jgl);

                }
            }

        /*
         * Draw the original line if it is available and the draped
         * line was not available.
         */
            else if (lp.originalLine != null) {
                l3d = new LineDraw3D (lp.originalLine);
                l3d.setScaler (scaler);
                l3d.scaleAndDrawLine (
                    visibleXmin, visibleYmin, visibleZmin,
                    visibleXmax, visibleYmax, visibleZmax,
                    drawingAttributes.getVerticalExageration(),
                    lcolor,
                    lp.lineWidth,
                    lp.dashFlag,
                    this,
                    jgl);

            }
        }

        if (pointFlag != 0) {
            if (lp.originalLine != null) {
                l3d = new LineDraw3D (lp.originalLine);
                l3d.setScaler (scaler);
                l3d.pointFlag = 1;
                l3d.scaleAndDrawLine (
                    visibleXmin, visibleYmin, visibleZmin,
                    visibleXmax, visibleYmax, visibleZmax,
                    drawingAttributes.getVerticalExageration(),
                    lcolor,
                    lp.lineWidth,
                    lp.dashFlag,
                    this,
                    jgl);

            }
        }

        jgl.SetDrawDepthTest (1);
        popName ();
        popName ();
        popName ();
    }

/*----------------------------------------------------------------------*/

    void drawFaultSurface (J3DFault fp)
    {
        int             i;

        if (linePickFlag == true) {
            return;
        }

        if (fp == null) {
            return;
        }

        if (fp.isVisible() == false) {
            return;
        }

        TriMesh tmesh = fp.getTriMesh ();
        if (tmesh == null) {
            if (fp.tm3d == null) {
                return;
            }
        }
        Color tmcolor = fp.getGlobalColor ();
        if (tmcolor == null) {
            return;
        }

        Color c = tmcolor;
        tmcolor = new Color (c.getRed(),
                             c.getGreen(),
                             c.getBlue(),
                             c.getAlpha()
                            );

        if (fp.isSelected()) {
            tmcolor = selectionColor;
        }

        if (tmcolor.getAlpha() < 255  &&
            opaqueFlag == true) {
            return;
        }

        if (tmcolor.getAlpha() == 255  &&
            opaqueFlag == false) {
            return;
        }

        if (
          drawingAttributes.getDoSolidFill() == false  &&
          drawingAttributes.getDoWireFrame() == false
        ) {
            return;
        }

        TriMeshDraw3D tm3d;

        tm3d = fp.tm3d;
        if (tm3d == null) {
            tm3d = new TriMeshDraw3D (tmesh);
            fp.tm3d = tm3d;
        }
        tm3d.setScaler (scaler);
        if (colorLookup == null) {
          tm3d.setColorByAttribute (false);
          tm3d.setColorLookup (null);
          tm3d.setNodeAttributeArray (null);
          tm3d.updateAttributeColors ();
        }
        else {
          tm3d.setColorByAttribute (true);
          tm3d.setColorLookup (colorLookup);
          tm3d.setNodeAttributeArray (null);
          tm3d.updateAttributeColors ();
        }
        tm3d.setLists (fp.wireListID, fp.solidListID);

        totalTriangles += tm3d.numTriangles;

        if (drawingAttributes.getDoSolidFill()  ||  selectionMode == true) {

            jgl.SetOffsetFill ();
            tm3d.setColor (tmcolor);
            jgl.SetDrawMode (JGL.DG_TRIANGLES);
            jgl.SetDrawStyle (JGL.DG_MESH);
            tm3d.scaleAndDrawTriangles (visibleXmin, visibleYmin, visibleZmin,
                                      visibleXmax, visibleYmax, visibleZmax,
                                      drawingAttributes.getVerticalExageration(),
                                      tmcolor,
                                      true, this, jgl);
            jgl.UnsetOffsetFill ();

            if (selectionMode == false) {
                int itmp = tm3d.getLastList ();
                if (itmp > 0) {
                    fp.solidListID = itmp;
                }
            }

        }

        if (drawingAttributes.getDoWireFrame()  &&  selectionMode == false) {

            jgl.UnsetOffsetFill ();
            jgl.SetDrawLineWidth (1.0);
            jgl.SetDrawLighting (0);
            jgl.SetDrawShading (0);
            tm3d.setColorByAttribute (false);
            jgl.SetDrawMode (JGL.DG_TRIANGLES);
            jgl.SetDrawStyle (JGL.DG_WIREFRAME);

          /*
           * If the solid fill is also drawn, contrast the wireframe
           * color with the solid fill color.
           */
            Color wcolor;
            if (drawingAttributes.getDoSolidFill()) {
                int ir = tmcolor.getRed ();
                int ig = tmcolor.getGreen ();
                int ib = tmcolor.getBlue ();
                int icmin = ir;
                int icmax = ir;
                if (ig < icmin) icmin = ig;
                if (ig > icmax) icmax = ig;
                if (ib < icmin) icmin = ib;
                if (ib > icmax) icmax = ib;
                icmax -= icmin;
                int idark;
                if (icmax < 10) {
                    idark = 400;
                }
                else if (icmax < 50) {
                    idark = 350;
                }
                else if (icmax < 100) {
                    idark = 300;
                }
                else if (icmax < 150) {
                    idark = 250;
                }
                else {
                    idark = 150;
                }
                if (ir+ig+ib > idark) {
                    wcolor = new Color (20, 20, 20, 255);
                }
                else {
                    wcolor = new Color (220, 220, 220, 255);
                }
            }

          /*
           * No solid color is drawn, so use the solid color for the
           * wireframe.
           */
            else {
                wcolor = tmcolor;
            }

            tm3d.scaleAndDrawTriangles (visibleXmin, visibleYmin, visibleZmin,
                                      visibleXmax, visibleYmax, visibleZmax,
                                      drawingAttributes.getVerticalExageration(),
                                      wcolor,
                                      false, this, jgl);

            if (drawingAttributes.getDoLighting()) {
                jgl.SetDrawLighting (1);
            }
            if (drawingAttributes.getDoShading()) {
                jgl.SetDrawShading (1);
            }

            int itmp = tm3d.getLastList ();
            if (itmp > 0) {
                fp.wireListID = itmp;
            }

        }

        return;

    }

/*---------------------------------------------------------------------------*/

    double            xTrans, yTrans, zTrans;
    double            xRot, yRot, zRot, xRotPress;
    double            xScale, yScale, zScale;
    private double            xTransDef, yTransDef, zTransDef;
    private double            xRotDef, yRotDef, zRotDef;
    private double            xScaleDef, yScaleDef, zScaleDef;
    private double            xTransHome, yTransHome, zTransHome;
    private double            xRotHome, yRotHome, zRotHome;
    private double            xScaleHome, yScaleHome, zScaleHome;

    void setTranslations (double xt, double yt, double zt)
    {
        xTrans = xt;
        yTrans = yt;
        zTrans = zt;
        return;
    }

    void setRotations (double xr, double yr, double zr)
    {
        xRot = xr;
        yRot = yr;
        zRot = zr;
        return;
    }

    void setScales (double xs, double ys, double zs)
    {
        xScale = xs;
        yScale = ys;
        zScale = zs;
        return;
    }

    public void adjustScale (double sfact)
    {
        xScale *= sfact;
        yScale *= sfact;
        zScale *= sfact;
    }


  /**
   * Draw the lines and points currently being edited.
   */
    private void drawEditPoints ()
    {

        if (!(tool instanceof J3DLineTool)) {
          return;
        }

        J3DLineTool lineTool = (J3DLineTool) tool;
        int         npoint, i, j, istat;

        initNames();
        pushName (J3DConst.LINE3D_NAME);
        pushName (0);

        npoint = lineTool.editPointList.size ();

        if (npoint == 0) {
            return;
        }

        JPoint3D    p, ps;

    /*
     * If the background is dark, draw in white.  If it is
     * light, draw in black.
     */
        Color pointColor = createContrastColor(backgroundColor);
        Color selectedPointColor = Color.CYAN;
        jgl.DGNodeColor(pointColor);

    /*
     * Draw the currently active draped line.  This line is
     * calculated from addEditPoint, and only needs to be
     * drawn here.
     */
        XYZPolyline     la;

        if (
            (tool instanceof J3DLineTool) &&
            ((J3DLineTool) tool).isLineMode() &&
            npoint > 1
        ) {

            if (lineTool.cacheDrapeList != null) {

                int n1 = lineTool.cacheDrapeList.size ();

                double[] xa;
                double[] ya;
                double[] za;

                jgl.SetDrawMode (JGL.DG_LINESTRIP);
                for (i=0; i<n1; i++) {
                    la = lineTool.cacheDrapeList.get (i);
                    xa = la.getXArray();
                    ya = la.getYArray();
                    za = la.getZArray();
                    int n2 = xa.length;

                    int nt = 0;
                    jgl.StartDrawMode ();
                    jgl.SetDrawStyle (JGL.DG_MESH);
                    jgl.SetDrawCulling (0);
                    for (j=0; j<n2; j++) {
                        ps = scaler.scalePoint (xa[j], ya[j], za[j]);
                        jgl.DGNodeXYZ (ps.x, ps.y, ps.z);
                    }
                    jgl.StopDrawMode ();

                }
            }
        }

        /*
         * Draw the points that are not in lines and any current points.
         */


        jgl.SetDrawMode (JGL.DG_POINTS);
        jgl.SetDrawStyle (JGL.DG_MESH);
        jgl.SetDrawCulling (0);
        jgl.SetDrawPointSize (7.0);
        for (i=0; i<npoint; i++) {
            p = lineTool.editPointList.get(i);
            if (
              tool instanceof J3DLineEditTool &&
              ((J3DLineEditTool) tool).isLineMode()
            ) {
              if (tool.pickedPoint != null && p == tool.pickedPoint) {
                jgl.DGNodeColor(selectedPointColor);
              } else {
                jgl.DGNodeColor(pointColor);
              }
            }
            ps = scaler.scalePoint (p.x, p.y, p.z);
            jgl.StartDrawMode ();
            jgl.DGNodeXYZ (ps.x, ps.y, ps.z);
            jgl.StopDrawMode ();
        }

    /*
     * Draw the currently moving lines and points here.  These should be the
     * last things drawn, and they are drawn without checking the z buffer.
     * This should make them always visible.
     */

/*
        if (lineTool.movingPointList != null) {

            jgl.SetDrawDepthTest (0);
            int nmove = lineTool.movingPointList.size();
            jgl.SetDrawMode (JGL.DG_LINESTRIP);
            jgl.SetDrawStyle (JGL.DG_MESH);
            jgl.SetDrawCulling (0);
            jgl.StartDrawMode ();
            for (i=0; i<nmove; i++) {
                p = (JPoint3D)lineTool.movingPointList.get(i);
                ps = scaler.scalePoint (p.x, p.y, p.z);
                jgl.DGNodeXYZ (ps.x, ps.y, ps.z);
            }
            jgl.StopDrawMode ();
            jgl.SetDrawDepthTest (1);
        }
*/

    /*
     * Set back to lighting effects in place prior to
     * drawing the edit lines and points.
     */
        if (drawingAttributes.getDoLighting()) {
            jgl.SetDrawLighting (1);
        }

    }  // end of updateEdit method

/*--------------------------------------------------------------------------*/

  /*
   * Draw the x=0, y=0 and z=0 planes in cube coordinates while in
   * selection mode.  This is done to get an xyz position even if
   * the cursor is not on an object.
   */
    void drawZoomTargets ()
    {
        jgl.ClearColorAndDepthBuffers ();

        jgl.MatrixModeModel ();
        jgl.LoadIdentity ();
        jgl.TranslateD (xTrans, yTrans, zTrans);
        jgl.RotateD (xRot, 1.0, 0.0, 0.0);
        jgl.RotateD (yRot, 0.0, 1.0, 0.0);
        jgl.RotateD (zRot, 0.0, 0.0, 1.0);
        jgl.ScaleD (xScale, yScale, zScale);
        jgl.SetupViewport (iwidth, iheight);


        jgl.SetDrawMode (JGL.DG_QUADS);
        jgl.SetDrawStyle (JGL.DG_MESH);

    /*
     * Draw the x=0 plane.
     */
        initNames ();
        pushName (J3DConst.XZERO_NAME);
        pushName (0);
        pushName (0);
        pushName (0);
        pushName (1);
        jgl.StartDrawMode ();
        jgl.DGNodeXYZ (0.0, -2.0, -2.0);
        jgl.DGNodeXYZ (0.0, 2.0, -2.0);
        jgl.DGNodeXYZ (0.0, 2.0, 2.0);
        jgl.DGNodeXYZ (0.0, -2.0, 2.0);
        jgl.StopDrawMode ();

    /*
     * Draw the y=0 plane.
     */
        initNames ();
        pushName (J3DConst.YZERO_NAME);
        pushName (0);
        pushName (0);
        pushName (0);
        pushName (1);
        jgl.StartDrawMode ();
        jgl.DGNodeXYZ (-2.0, 0.0, -2.0);
        jgl.DGNodeXYZ (2.0, 0.0, -2.0);
        jgl.DGNodeXYZ (2.0, 0.0, 2.0);
        jgl.DGNodeXYZ (-2.0, 0.0, 2.0);
        jgl.StopDrawMode ();

    /*
     * Draw the z=0 plane.
     */
        initNames ();
        pushName (J3DConst.ZZERO_NAME);
        pushName (0);
        pushName (0);
        pushName (0);
        pushName (1);
        jgl.StartDrawMode ();
        jgl.DGNodeXYZ (-2.0, -2.0, 0.0);
        jgl.DGNodeXYZ (2.0, -2.0, 0.0);
        jgl.DGNodeXYZ (2.0, 2.0, 0.0);
        jgl.DGNodeXYZ (-2.0, 2.0, 0.0);
        jgl.StopDrawMode ();

    }

/*---------------------------------------------------------------------------*/

    boolean isSelectionMode ()
    {
        return selectionMode;
    }

/*----------------------------------------------------------------------------------*/

/**
 Save the current matrices used for viewing the data in the homeModelMatrix and
 homeProjectionMatrix arrays.  This is called when the set home view button is
 clicked in the menu bar.
*/
    void saveCurrentView ()
    {

        xTransHome = xTrans;
        yTransHome = yTrans;
        zTransHome = zTrans;

        xScaleHome = xScale;
        yScaleHome = yScale;
        zScaleHome = zScale;

        xRotHome = xRot;
        yRotHome = yRot;
        zRotHome = zRot;

    }

/*----------------------------------------------------------------------------------*/

/**
 Reset the viewing matrices to the last saved values and redraw the data.
*/
    void resetToHomeView ()
    {
        xTrans = xTransHome;
        yTrans = yTransHome;
        zTrans = zTransHome;

        xScale = xScaleHome;
        yScale = yScaleHome;
        zScale = zScaleHome;

        xRot = xRotHome;
        yRot = yRotHome;
        zRot = zRotHome;

        localRepaint ();
    }
/*----------------------------------------------------------------------------------*/

/**
 Attempt to fit the whole view into the window.
*/
    void resetToUnitScale ()
    {

        xScale = 1.05;
        yScale = 1.05;
        zScale = 1.05;

        if (
          drawingAttributes.getDoAxesLabels() == false  &&
          drawingAttributes.getDoAxesNumbers() == false
        ) {
            xScale = 1.1;
            yScale = 1.1;
            zScale = 1.1;
        }

        xTrans = xTransDef;
        yTrans = yTransDef;
        zTrans = zTransDef;

        localRepaint ();
    }

/*----------------------------------------------------------------------------------*/

    void presetView (int view)
    {
        if (view < 1  ||  view > 6) {
            return;
        }

        xTrans = xTransDef;
        yTrans = yTransDef;
        zTrans = zTransDef;
        xScale = xScaleDef;
        yScale = yScaleDef;
        zScale = zScaleDef;

        if (view == TOP) {
            xRot = 90.0;
            yRot = 0.0;
            zRot = 0.0;
        }
        else if (view == BOTTOM) {
            xRot = -90.0;
            yRot = 0.0;
            zRot = 0.0;
        }
        else if (view == NORTH) {
            xRot = 0.0;
            yRot = -180.0;
            zRot = 0.0;
        }
        else if (view == SOUTH) {
            xRot = 0.0;
            yRot = 0.0;
            zRot = 0.0;
        }
        else if (view == EAST) {
            xRot = 0.0;
            yRot = -90.0;
            zRot = 0.0;
        }
        else if (view == WEST) {
            xRot = 0.0;
            yRot = 90.0;
            zRot = 0.0;
        }

        localRepaint ();

        return;

    }

/*-----------------------------------------------------------------------*/

/*
 * This is called the last thing in a redraw.  A compass
 * is drawn in the lower left of the viewport.  The compass
 * is rotated the same as the drawing cube.
 */
    private void drawCompass (Color bgColor)
    {
        jgl.SetupRasterView ();
        int[] vport = jgl.GetViewPort ();

        jgl.SetDrawLighting (0);
        jgl.SetDrawCulling (0);
        jgl.SetDrawShading (0);
        jgl.ClearDepthBuffer ();
        jgl.SetDrawDepthTest (1);

        jgl.SetOffsetFill ();

        double width = vport[2];
        double height = vport[3];

        jgl.MatrixModeModel ();
        jgl.LoadIdentity ();
        jgl.TranslateD (-width / 2.0 + 70, -height / 2.0 + 70, 0.0);
        jgl.RotateD (xRot, 1.0, 0.0, 0.0);
        jgl.RotateD (yRot, 0.0, 1.0, 0.0);
        jgl.RotateD (zRot, 0.0, 0.0, 1.0);
        jgl.ScaleD (.85, .85, .85);

    /*
     * Draw the top and bottom triangles.
     */
        jgl.SetDrawMode (JGL.DG_TRIANGLES);
        jgl.SetDrawStyle (JGL.DG_MESH);
        jgl.StartDrawMode ();

        jgl.DGNodeColor (0, .5, 1.0, 1);
        jgl.DGNodeXYZ (0, 1, -50);
        jgl.DGNodeXYZ (35, 10, 50);
        jgl.DGNodeXYZ (-35, 10, 50);

        jgl.DGNodeColor (.5, .45, 0, 1);
        jgl.DGNodeXYZ (0, -1, -50);
        jgl.DGNodeXYZ (35, -10, 50);
        jgl.DGNodeXYZ (-35, -10, 50);

        jgl.StopDrawMode ();

    /*
     * Draw quads for the sides.
     */
        jgl.SetDrawMode (JGL.DG_QUADS);
        jgl.StartDrawMode ();

        jgl.DGNodeColor (0, .5, 1.0, 1);

        jgl.DGNodeXYZ (0, 1, -50);
        jgl.DGNodeXYZ (35, 10, 50);
        jgl.DGNodeXYZ (35, 0, 50);
        jgl.DGNodeXYZ (0, 0, -50);

        jgl.DGNodeXYZ (0, 1, -50);
        jgl.DGNodeXYZ (-35, 10, 50);
        jgl.DGNodeXYZ (-35, 0, 50);
        jgl.DGNodeXYZ (0, 0, -50);

        jgl.DGNodeXYZ (-35, 10, 50);
        jgl.DGNodeXYZ (35, 10, 50);
        jgl.DGNodeXYZ (35, 0, 50);
        jgl.DGNodeXYZ (-35, 0, 50);

        jgl.DGNodeColor (.5, .45, 0, 1);

        jgl.DGNodeXYZ (0, -1, -50);
        jgl.DGNodeXYZ (35, -10, 50);
        jgl.DGNodeXYZ (35, 0, 50);
        jgl.DGNodeXYZ (0, 0, -50);

        jgl.DGNodeXYZ (0, -1, -50);
        jgl.DGNodeXYZ (-35, -10, 50);
        jgl.DGNodeXYZ (-35, 0, 50);
        jgl.DGNodeXYZ (0, 0, -50);

        jgl.DGNodeXYZ (-35, -10, 50);
        jgl.DGNodeXYZ (35, -10, 50);
        jgl.DGNodeXYZ (35, 0, 50);
        jgl.DGNodeXYZ (-35, 0, 50);

        jgl.StopDrawMode ();

    /*
     * If the background is dark, draw outline in white.  If it is
     * light, draw outline in black.
     */
        jgl.SetDrawLineWidth (1.0);
        jgl.SetDrawStyle(JGL.DG_WIREFRAME);
        jgl.DGNodeColor(createContrastColor(bgColor));

    /*
     * Outline quads for the sides.
     */
        jgl.SetDrawStyle (JGL.DG_WIREFRAME);

        jgl.StartDrawMode ();
        jgl.DGNodeXYZ (0, 1, -50);
        jgl.DGNodeXYZ (35, 10, 50);
        jgl.DGNodeXYZ (35, 0, 50);
        jgl.DGNodeXYZ (0, 0, -50);
        jgl.StopDrawMode ();

        jgl.StartDrawMode ();
        jgl.DGNodeXYZ (0, 1, -50);
        jgl.DGNodeXYZ (-35, 10, 50);
        jgl.DGNodeXYZ (-35, 0, 50);
        jgl.DGNodeXYZ (0, 0, -50);
        jgl.StopDrawMode ();

        jgl.StartDrawMode ();
        jgl.DGNodeXYZ (-35, 10, 50);
        jgl.DGNodeXYZ (35, 10, 50);
        jgl.DGNodeXYZ (35, 0, 50);
        jgl.DGNodeXYZ (-35, 0, 50);
        jgl.StopDrawMode ();

        jgl.StartDrawMode ();
        jgl.DGNodeXYZ (0, -1, -50);
        jgl.DGNodeXYZ (35, -10, 50);
        jgl.DGNodeXYZ (35, 0, 50);
        jgl.DGNodeXYZ (0, 0, -50);
        jgl.StopDrawMode ();

        jgl.StartDrawMode ();
        jgl.DGNodeXYZ (0, -1, -50);
        jgl.DGNodeXYZ (-35, -10, 50);
        jgl.DGNodeXYZ (-35, 0, 50);
        jgl.DGNodeXYZ (0, 0, -50);
        jgl.StopDrawMode ();

        jgl.StartDrawMode ();
        jgl.DGNodeXYZ (-35, -10, 50);
        jgl.DGNodeXYZ (35, -10, 50);
        jgl.DGNodeXYZ (35, 0, 50);
        jgl.DGNodeXYZ (-35, 0, 50);
        jgl.StopDrawMode ();

        jgl.UnsetOffsetFill ();

        return;

    }

/*-----------------------------------------------------------------------*/

/*
 * This is called the last thing in a redraw.  A compass
 * is drawn in the lower left of the viewport.  The compass
 * is rotated the same as the drawing cube.
 */
    private void drawTitles (Color bgColor)
    {
        jgl.SetupRasterView ();
        int[] vport = jgl.GetViewPort ();

        jgl.SetDrawLighting(0);
        jgl.SetDrawCulling(0);
        jgl.SetDrawShading (0);
        jgl.ClearDepthBuffer ();
        jgl.SetDrawDepthTest (0);

        jgl.UnsetOffsetFill ();

        double width = vport[2];
        double height = vport[3];

        jgl.MatrixModeModel ();
        jgl.LoadIdentity ();

        String unitString, xyString, zString, valString;

        if (xyUnits < 0.9) {
            xyString = "feet";
        }
        else if (xyUnits < 2.0) {
            xyString = "meters";
        }
        else if (xyUnits < 1100) {
            xyString = "kilometers";
        }
        else {
            xyString = "miles";
        }

        if (zUnits < 0.9) {
            zString = "feet";
        }
        else if (zUnits < 2.0) {
            zString = "meters";
        }
        else if (zUnits < 1100) {
            zString = "kilometers";
        }
        else {
            zString = "miles";
        }

        unitString = "XY Units: "+xyString+"    Z Units: "+zString;

    /*
     * If the background is dark, draw titles in white.  If it is
     * light, draw titles in black.
     */
        jgl.DGNodeColor(createContrastColor(bgColor));
        double h2 = height / 2;
        double xanc = -(width / 2) + 10;
        Font font = drawingAttributes.getTitlesFont();
        if (drawingAttributes.getTitleString() != null) {
            jgl.DrawTextXYZ (
              xanc,
              h2-30,
              0,
              drawingAttributes.getTitleString(),
              font
            );
        }
        jgl.DrawTextXYZ (xanc, h2-47, 0, unitString, font);

        return;
    }

    private void drawExaggerationTitle(Color bgColor) {
      jgl.SetupRasterView ();
      int[] vport = jgl.GetViewPort ();

      jgl.SetDrawLighting(0);
      jgl.SetDrawCulling(0);
      jgl.SetDrawShading (0);
      jgl.ClearDepthBuffer ();
      jgl.SetDrawDepthTest (0);

      jgl.UnsetOffsetFill ();

      double width = vport[2];
      double height = vport[3];

      jgl.MatrixModeModel ();
      jgl.LoadIdentity ();

      String valString;
      double val;
      int itmp = (int)(drawingAttributes.getVerticalExageration() + .5);
      if (itmp < 10) {
          itmp = (int)(drawingAttributes.getVerticalExageration() * 10);
          val = itmp;
          val /= 10.0;
          valString = String.valueOf (val);
      }
      else {
          valString = String.valueOf (itmp);
      }

      String exagString = "Vertical Exaggeration: "+valString;

  /*
   * If the background is dark, draw titles in white.  If it is
   * light, draw titles in black.
   */
      jgl.DGNodeColor(createContrastColor(bgColor));
      double h2 = height / 2;
      double xanc = -(width / 2) + 10;
      if (drawingAttributes.getTitleString() != null) {
          jgl.DrawTextXYZ (xanc, h2-30, 0, drawingAttributes.getTitleString(), drawingAttributes.getTitlesFont());
      }
      jgl.DrawTextXYZ (xanc, h2-64, 0, exagString, drawingAttributes.getTitlesFont());

      return;
    }


/*-----------------------------------------------------------------------*/

    private void drawLights (Color bgColor)
    {
        jgl.SetDrawMode (JGL.DG_POINTS);
        jgl.SetDrawStyle (JGL.DG_MESH);
        jgl.SetDrawCulling (0);
        jgl.SetDrawPointSize (10.0);

    /*
     * If the background is dark, draw titles in white.  If it is
     * light, draw titles in black.
     */
        jgl.DGNodeColor(createContrastColor(bgColor));

        LightData light1 = drawingAttributes.getViewProperties().getLight1();
        if (light1.isEnabled()) {
            jgl.StartDrawMode ();
          // note that passing in X,Z,Y is correct ordering
            jgl.DGNodeXYZ (light1.getX(), light1.getZ(), -light1.getY());
            jgl.StopDrawMode ();
          // note that passing in X,Z,Y is correct ordering
            jgl.DrawTextXYZ (light1.getX(), light1.getZ(), -light1.getY(),
                             "Light 1", 0.1, 2, 0, 10);
        }

        LightData light2 = drawingAttributes.getViewProperties().getLight2();
        if (light2.isEnabled()) {
            jgl.StartDrawMode ();
          // note that passing in X,Z,Y is correct ordering
            jgl.DGNodeXYZ (light2.getX(), light2.getZ(), -light2.getY());
            jgl.StopDrawMode ();
          // note that passing in X,Z,Y is correct ordering
            jgl.DrawTextXYZ (light2.getX(), light2.getZ(), -light2.getY(),
                             "Light 2", 0.1, 2, 0, 10);
        }

        LightData light3 = drawingAttributes.getViewProperties().getLight3();
        if (light3.isEnabled()) {
            jgl.StartDrawMode ();
          // note that passing in X,Z,Y is correct ordering
            jgl.DGNodeXYZ (light3.getX(), light3.getZ(), -light3.getY());
            jgl.StopDrawMode ();
          // note that passing in X,Z,Y is correct ordering
            jgl.DrawTextXYZ (light3.getX(), light3.getZ(), -light3.getY(),
                             "Light 3", 0.1, 2, 0, 10);
        }

        LightData light4 = drawingAttributes.getViewProperties().getLight4();
        if (light4.isEnabled()) {
            jgl.StartDrawMode ();
          // note that passing in X,Z,Y is correct ordering
            jgl.DGNodeXYZ (light4.getX(), light4.getZ(), -light4.getY());
            jgl.StopDrawMode ();
          // note that passing in X,Z,Y is correct ordering
            jgl.DrawTextXYZ (light4.getX(), light4.getZ(), -light4.getY(),
                             "Light 4", 0.1, 2, 0, 10);
        }

    }


/*-------------------------------------------------------------------------------*/

    private void drawJ3DText (J3DText tp)
    {

        if (tp == null) {
            return;
        }

        if (tp.isVisible() == false) {
            return;
        }

        Color
        tcolor = tp.getGlobalColor ();
        if (tcolor == null) {
            tcolor = getForegroundColor ();
        }

        if (tp.text == null) {
            return;
        }

        if (tp.x > 1.e20  ||  tp.y > 1.e20  ||  tp.z > 1.e20) {
            return;
        }

        if (tp.isSelected()) {
            tcolor = lineSelectionColor;
        }

        if (tp.hidden == false) {
            jgl.SetDrawDepthTest (0);
        }

    /*
     * The fifth element in the "name" stack identifies this as
     * a line (0) or fill (1).  This is needed in the hit processing
     * done in the JGL object.  So, the stack is extended three places
     * to get the zero in the fifth element.
     */
        pushName (0);
        pushName (0);
        pushName (0);

        JPoint3D p = scaler.scalePoint (tp.x, tp.y, tp.z);

        double    r, g, b;
        r = tcolor.getRed() / 255.0;
        g = tcolor.getGreen() / 255.0;
        b = tcolor.getBlue() / 255.0;
        jgl.DGNodeColor (r, g, b, 1.0);

        jgl.DrawTextXYZ (
            p.x,
            p.y,
            p.z,
            tp.text,
            tp.textSize,
            tp.anchor,
            0,
            0);

        jgl.SetDrawDepthTest (1);

        popName ();
        popName ();
        popName ();
    }

/*-------------------Debug methods --------------------------------------*/

    private PrintWriter dbfile = null;

    void dbMessage (String s)
    {
        if (dbfile == null) {
            try
            {
                dbfile = new PrintWriter (
                    new FileWriter ("j3d_dbg.txt")
                );
            }
            catch (Exception e)
            {
                System.out.println ("error creating j3d debug file");
                dbfile = null;
            }
        }

        System.out.println (s);
        System.out.flush ();

        if (dbfile != null) {
            dbfile.println (s);
            dbfile.flush ();
        }

    }

    void dbSleep (int nsec, String msg)
    {

        if (msg != null) {
            dbMessage (msg);
        }

        if (nsec > 0) {
            try {
                Thread.sleep (1000 * nsec);
            }
            catch (Exception e) {
            }
            dbMessage ("Waking up");
        }

        return;
    }

    void dbSleep (int nsec)
    {

        if (nsec > 0) {
            try {
                Thread.sleep (1000 * nsec);
            }
            catch (Exception e) {
            }
        }

        return;
    }

/*-------------------------- Picking methods ------------------------------------*/

    boolean             linePickFlag = false;
    private int         numStackLevels = 0;
    private int[]       pickStackValues = new int[10];

/*
 * Make selectionMode package scope so the various draw classes
 * in the package can access it directly.
 */
    boolean     selectionMode = false;

/*-------------------------------------------------------------------------------*/

/*
 * Thin wrappers around the jgl name functions to
 * check that we are in selection mode prior to calling
 * the jgl method.  The stack is also recorded for 
 * debug purposes.
 */
    void initNames ()
    {
        if (selectionMode) {
            jgl.InitNames ();
            numStackLevels = 0;
            for (int i=0; i<10; i++) {
                pickStackValues[i] = 0;
            }
        }
    }

    void pushName (int ival)
    {
        if (selectionMode) {
            jgl.PushName (ival);
            pickStackValues[numStackLevels] = ival;
            numStackLevels++;
        }
    }

    void loadName (int ival)
    {
        if (selectionMode) {
            jgl.LoadName (ival);
            if (numStackLevels > 0) {
              pickStackValues[numStackLevels-1] = ival;
            }
        }
    }

    void popName ()
    {
        if (selectionMode) {
            jgl.PopName ();
            numStackLevels--;
            if (numStackLevels < 0) {
                numStackLevels = 0;
            }
         }
     }

/*------------------------------------------------------------------------------*/

/*
    Picking methods.  Picking must be done from the display method called by
    jogl.  Therefore, when the mouse click is recorded it just sets up a state
    and then signals for a re rendering of the scene.  The display method
    will call the pickObject method if it is in the picking state and the
    display method will redraw the scene showing any changes made during the
    picking state after the pickObject function returns.
*/

    private boolean pickScheduled = false;
    private MouseEvent  pickMouseEvent = null;

    boolean targetZoomFlag = false;
    boolean targetDrawFlag = false;

/*------------------------------------------------------------------------------*/

    public void enableTargetZoomFlag ()
    {
        targetZoomFlag = true;
    }

/*------------------------------------------------------------------------------*/

    private void scheduleObjectPick (MouseEvent e)
    {
        pickScheduled = true;
        pickMouseEvent = e;
            listDrawFlag = false;
        localRepaint ();
    }

/*--------------------------------------------------------------------------*/

    private void setupTargetZoom (MouseEvent e)
    {
        int ix = e.getX();
        int iy = e.getY();

    /*
     * Scale the current picked point to the unit cube.
     */
        JPoint3D p = scaler.scalePoint (tool.pickedX, tool.pickedY, tool.pickedZ);

    /*
     * The initial translation is based on the location of the
     * picked point raster position relative to the center of the
     * viewport.
     */
        double dx = iwidth / 2 - ix;
        double dy = -(iheight / 2 - iy);
        double div = 300.0;
        xTrans += dx / div;
        yTrans += dy / div;
        xTrans *= xScale;
        yTrans *= yScale;

        int ndo = 0;
        double adx, ady;
        JPoint2D p2;

    /*
     * Sneak up on the center point by simulating a series of
     * scene pans.  Once the raster point of the picked model
     * point is very near the center of the viewport, exit
     * the iterative loop.
     */
        for (;;) {

            jgl.MatrixModeModel ();
            jgl.LoadIdentity ();
            jgl.TranslateD (xTrans, yTrans, zTrans);
            jgl.RotateD (xRot, 1.0, 0.0, 0.0);
            jgl.RotateD (yRot, 0.0, 1.0, 0.0);
            jgl.RotateD (zRot, 0.0, 0.0, 1.0);
            jgl.ScaleD (xScale * 1.5, yScale * 1.5, zScale * 1.5);

            p2 = jgl.GetRasterPos (p.x, p.y, p.z);
            ix = (int)(p2.x + .5);
            iy = (int)(p2.y + .5);
            dx = iwidth / 2 - ix;
            dy = iheight / 2 - iy;
            adx = dx;
            if (adx < 0.0) adx = -adx;
            ady = dy;
            if (ady < 0.0) ady = -ady;
            if (ady < 2  &&  adx < 2) {
                break;
            }
            xTrans += dx / div;
            yTrans += dy / div;
            ndo++;
            if (ndo > 10) {
                break;
            }
        }

    /*
     * Set the scales to 1.5 times their previous values.
     */
        xScale *= 1.5;
        yScale *= 1.5;
        zScale *= 1.5;

        return;
    }

/*-------------------------------------------------------------------------*/

    private void centerCursor ()
    {
        Robot robot;

        Point pt = new Point (iwidth / 2, iheight / 2);
        SwingUtilities.convertPointToScreen (pt, canvas);

        int ix = (int)pt.getX();
        int iy = (int)pt.getY();

        try
        {
            robot = new Robot ();
        }
        catch (Exception e)
        {
            return;
        }

        robot.mouseMove (ix, iy);
    }
/*-----------------------Mouse Listeners----------------------------------*/

    private class J3DWheelListener implements MouseWheelListener
    {
        public void mouseWheelMoved (MouseWheelEvent e)
        {
            int i = e.getWheelRotation ();

            double mult = 1.15;
            if (e.isShiftDown()) {
                mult = 1.02;
            }

            if (i < 0) {
                xScale /= mult;
                yScale /= mult;
                zScale /= mult;
            }
            else {
                xScale *= mult;
                yScale *= mult;
                zScale *= mult;
            }

            J3DPanel.this.isMoving = false;

            J3DPanel.this.localRepaint ();

        }
    }

/*-----------------------------------------------------------------*/

/*
 * The mouse listener will interpret mouse events according
 * to the state of the display list panel.
 */
    private class J3DMouseListener extends MouseInputAdapter
    {

        private int button_1, button_2, button_3;

        private int b1X,
                    b1Y,
                    b2X,
                    b2Y,
                    b3X,
                    b3Y;

        private void setButtonID (MouseEvent e) {
            button_1 = e.getModifiers() & InputEvent.BUTTON1_MASK;
            button_2 = e.getModifiers() & InputEvent.BUTTON2_MASK;
            button_3 = e.getModifiers() & InputEvent.BUTTON3_MASK;
        }

        public void mouseClicked(MouseEvent e)
        {
            e.consume ();
        }

        private void localClick (MouseEvent e)
        {
            scheduleObjectPick (e);
            e.consume ();
        }

        public void mouseDragged(MouseEvent e) {

          boolean isDone = false;
          if (tool instanceof J3DLineEditTool) {
            J3DLineEditTool lineEditTool = (J3DLineEditTool) tool;
            if (lineEditTool.dragPointMode) {
                lineEditTool.processPointDrag (e);
                isDone = true;
            }
            else if (lineEditTool.dragLineMode) {
                lineEditTool.processLineDrag (e);
                isDone = true;
            }
          }
          if (!isDone) {
              if (button_1 != 0  &&  button_2 == 0  &&  button_3 == 0) {
                  panScene (e);
              }
              if (button_1 == 0  &&  button_2 == 0  &&  button_3 != 0) {
                  rotateScene (e);
              }
          }
          if (tool instanceof J3DLineTool) {
              ((J3DLineTool) tool).callMotionListeners ();
          }
          e.consume ();
        }

        public void mouseMoved(MouseEvent e) {
          boolean isDone = false;
          if (tool instanceof J3DLineEditTool) {
            J3DLineEditTool lineEditTool = (J3DLineEditTool) tool;
            if (lineEditTool.dragPointMode) {
              lineEditTool.processPointDrag (e);
              isDone = true;
            }
            else if (lineEditTool.dragLineMode) {
              lineEditTool.processLineDrag (e);
              isDone = true;
            }
          }
          if (!isDone) {
              if (button_1 != 0  &&  button_2 == 0  &&  button_3 == 0) {
                  panScene (e);
              }
              if (button_1 == 0  &&  button_2 == 0  &&  button_3 != 0) {
                  rotateScene (e);
              }
          }
          if (tool instanceof J3DLineTool) {
            ((J3DLineTool) tool).callMotionListeners ();
          }
          e.consume ();
        }

        public void mousePressed(MouseEvent e) {

          setButtonID (e);

            if (button_1 != 0) {
                b1X = e.getX();
                b1Y = e.getY();
                if (tool instanceof J3DLineEditTool) {
                  J3DLineEditTool lineEditTool = (J3DLineEditTool) tool;
                  lineEditTool.startDragPickMode();
                  if (lineEditTool.dragPickScheduled)
                    scheduleObjectPick(e);
                }
            }
            if (button_2 != 0) {
                b2X = e.getX();
                b2Y = e.getY();
            }
            if (button_3 != 0) {
                b3X = e.getX();
                b3Y = e.getY();
                xRotPress = xRot;
                if (b3Y < iheight / 3 ) {
                    xRotPress = -xRot;
                }
            }

            lastMouseX = e.getX();
            lastMouseY = e.getY();
            minD = 10.0;
            e.consume ();
        }

        public void mouseReleased(MouseEvent e) {

            int            dx, dy;

            setButtonID (e);

            dx = 0;
            dy = 0;
            boolean isUnselectEditPoint = false;

            if (button_1 != 0) {
                dx = b1X - e.getX();
                dy = b1Y - e.getY();
                b1X = -1;
                b1Y = -1;
                if (tool instanceof J3DLineEditTool) {
                  J3DLineEditTool lineEditTool = (J3DLineEditTool) tool;
                  if (lineEditTool.isDragMode()) {
                      if (!(Math.abs(dx) < 5 && Math.abs(dy) < 5) ) {
                        scheduleObjectPick(e);
                      } else {
                        isUnselectEditPoint = true;
                        lineEditTool.endDragMode();
                      }
                  }
                  lineEditTool.endDragPickMode();
                }
            }
            if (button_2 != 0) {
                dx = b2X - e.getX();
                dy = b2Y - e.getY();
                b2X = -1;
                b2Y = -1;
            }
            if (button_3 != 0) {
                dx = b3X - e.getX();
                dy = b3Y - e.getY();
                b3X = -1;
                b3Y = -1;
            }

            if (dx < 0) dx = -dx;
            if (dy < 0) dy = -dy;

            if (dx < 5  &&  dy < 5) {
                if (isUnselectEditPoint) {
                  tool.clearPickInfo();
                  localRepaint();
                } else {
                  localClick (e);
                }
            }

            button_1 = 0;
            button_2 = 0;
            button_3 = 0;
            e.consume ();

            if (isMoving) {
                isMoving = false;
                if (J3DPanel.this.drawingAttributes.getMotionRedraw() == false) {
                    J3DPanel.this.localRepaint ();
                }
            }
        }

        public void mouseEntered(MouseEvent e) {
            e.consume ();
        }

        public void mouseExited(MouseEvent e) {
            e.consume ();
        }
    }

/*---------------------------------------------------------------------------*/

    private int    lastMouseX = 0,
                   lastMouseY = 0;
    private double minD = 10.0;

    private void panScene (MouseEvent e)
    {
        int        x = e.getX();
        int        y = e.getY();

        double     dx, dy;
        double     div;

        div = 300.0;
        if (e.isShiftDown()) {
            div = 1500.0;
        }

        dx = x - lastMouseX;
        dy = lastMouseY - y;

        double adx = dx;
        double ady = dy;
        if (adx < 0) adx = -adx;
        if (ady < 0) ady = -ady;
        if (adx < minD  &&  ady < minD) {
            minD -= 2.0;
            return;
        }

        xTrans += dx / div;
        yTrans += dy / div;

        lastMouseX = x;
        lastMouseY = y;

        isMoving = true;

        localRepaint ();
    }

/*---------------------------------------------------------------------------*/

    private void rotateScene (MouseEvent e)
    {
        int        x = e.getX();
        int        y = e.getY();

        double     dx, dy;
        double     div;

        div = 1000.0;
        if (e.isShiftDown()) {
            div = 5000.0;
        }

        dx = x - lastMouseX;
        dy = lastMouseY - y;

        double adx = dx;
        double ady = dy;
        if (adx < 0) adx = -adx;
        if (ady < 0) ady = -ady;
        if (adx < minD  &&  ady < minD) {
            minD -= 2.0;
            return;
        }

        if (xRotPress < -45.0  ||  xRotPress > 135) {
            dx = -dx;
        }

        double dxr = dy * 180.0 / div;
        double dyr = -dx * 180.0 / div;

        xRot -= dxr;
        yRot -= dyr;

        if (xRot > 180) xRot -= 360;
        if (xRot < -180) xRot += 360;

        if (yRot > 180) yRot -= 360;
        if (yRot < -180) yRot += 360;

        lastMouseX = x;
        lastMouseY = y;

        isMoving = true;

        localRepaint ();
    }

/*-----------------------Picking Mode ---------------------------------------------*/

    private boolean fromPickMessage = false;

    /**
     * Set the picking mode for the 3d display to selection mode.
     */
     public void setSelectionMode () {
       tool = new J3DSelectionTool(this, jgl);
       setCursor(null);
       setPickModeMessage (null);
       this.redrawAll();
     }

/*------------------------------------------------------------------------------*/

    public void setPickModeMessage (String msg)
    {
        if (msg == null) {
            msg = "Selection Mode";
        }
        String tmsg = msg;
        if (pickModeLabel != null) {
            pickModeLabel.setText (" "+tmsg);
        }

        fromPickMessage = true;
        repaint ();
    }

    public void drawPickModeMessageImmediately ()
    {
        if (pickModeLabel != null) {
            pickModeLabel.repaint ();
        }
    }

/*------------------------------------------------------------------------------*/

/*
 * Manage select listeners.
 */

    ArrayList<J3DSelectListener> selectListenerList = 
        new ArrayList<J3DSelectListener> ();

    public void addSelectListener (J3DSelectListener sl)
    {
        int index = selectListenerList.indexOf (sl);
        if (index >= 0) {
            return;
        }

        selectListenerList.add (sl);

        return;
    }

    public void removeSelectListener (J3DSelectListener sl)
    {
        int index = selectListenerList.indexOf (sl);
        if (index < 0) {
            return;
        }

        selectListenerList.remove (index);

        return;
    }

    public void removeAllSelectListeners ()
    {
        selectListenerList.clear ();
    }

    void callSelectListeners ()
    {
        ArrayList<J3DFault>        flist;
        ArrayList<J3DHorizonPatch> hlist;
        ArrayList<J3DLine>         linelist;

        int size = selectListenerList.size();
        if (size < 1) {
            return;
        }

        flist = getSelectedFaults ();
        hlist = getSelectedHorizons ();
        linelist = getSelectedLines ();

    /*
     * Create and populate a select info object.
     */
        J3DSelectionChangedInfo info =
            new J3DSelectionChangedInfo ();

        info.xPick = tool.pickedX;
        info.yPick = tool.pickedY;
        info.zPick = tool.pickedZ;

        info.xScreen = tool.pickedScreenX;
        info.yScreen = tool.pickedScreenY;

        info.selectedFaults = flist;
        info.selectedHorizons = hlist;
        info.selectedLines = linelist;

        J3DSelectListener sl;

        for (int i=0; i<size; i++) {
            sl = selectListenerList.get(i);
            if (sl != null) {
                sl.selectionChanged (info);
            }
        }

        return;

    }

    private void callPickModeChangedListeners (int oldmode, int newmode)
    {

        int size = selectListenerList.size ();

        J3DSelectListener sl;

        for (int i=0; i<size; i++) {
            sl = selectListenerList.get(i);
            if (sl != null) {
                sl.pickModeChanged ();
            }
        }

        return;

    }


/*---------------------------------------------------------------------*/

/*
 * Manage the right click listeners.
 */

    ArrayList<J3DRightClickListener> rightClickListenerList = 
        new ArrayList<J3DRightClickListener> ();

    public void addRightClickListener(J3DRightClickListener sl) {
      int index = rightClickListenerList.indexOf(sl);
      if (index >= 0) {
        return;
      }

      rightClickListenerList.add(sl);

      return;
    }

    public void removeRightClickListener(J3DRightClickListener sl) {
      int index = rightClickListenerList.indexOf(sl);
      if (index < 0) {
        return;
      }

      rightClickListenerList.remove(index);

      return;
    }

    public void removeAllRightClickListeners() {
      rightClickListenerList.clear();
    }

/*---------------------------------------------------------------------------*/

/*
 * Manage the visibility listeners.
 */

    ArrayList<J3DVisibleListener> visibleListenerList = 
        new ArrayList<J3DVisibleListener> ();

    public void addVisibleListener (J3DVisibleListener vl)
    {
        int index = visibleListenerList.indexOf (vl);
        if (index >= 0) {
            return;
        }

        visibleListenerList.add (vl);

        return;
    }

    public void removeVisibleListener (J3DVisibleListener vl)
    {
        int index = visibleListenerList.indexOf (vl);
        if (index < 0) {
            return;
        }

        visibleListenerList.remove (index);

        return;
    }

    public void removeAllVisibleListeners ()
    {
        visibleListenerList.clear ();
    }

    private void callVisibleListeners ()
    {
        ArrayList<J3DFault>         flist;
        ArrayList<J3DHorizonPatch>  hlist;
        ArrayList<J3DLine>          linelist;

        int size = visibleListenerList.size();
        if (size < 1) {
            return;
        }

        flist = getVisibleFaults ();
        hlist = getVisibleHorizons ();
        linelist = getVisibleLines ();

        J3DVisibleListener vl;

        J3DVisibilityChangedInfo info = new J3DVisibilityChangedInfo ();

        info.visibleFaults = flist;
        info.visibleHorizons = hlist;
        info.visibleLines = lineList;

        flist = getInvisibleFaults ();
        hlist = getInvisibleHorizons ();
        linelist = getInvisibleLines ();

        info.invisibleFaults = flist;
        info.invisibleHorizons = hlist;
        info.invisibleLines = lineList;

        for (int i=0; i<size; i++) {
            vl = visibleListenerList.get(i);
            if (vl != null) {
                vl.visibilityChanged (info);
            }
        }

        return;
    }

 /*------------------------------------------------------------------------------*/

/**
 *  Get a list of currently visible faults.   If no faults are visible, the list
 *  will be empty.
 */
    public ArrayList<J3DFault> getVisibleFaults ()
    {
        ArrayList<J3DFault> al = new ArrayList<J3DFault> ();
        if (faultList == null) {
            return al;
        }

        int size = faultList.size ();
        J3DFault  fp;

        for (int i=0; i<size; i++) {
            fp = faultList.get(i);
            if (fp != null) {
                if (fp.isVisible()) {
                    al.add (fp);
                }
            }
        }

        return al;
    }

/*---------------------------------------------------------------------------------*/

/**
 *  Get a list of currently visible horizons.   If no horizons are visible, the list
 *  will be empty.
 */
    public ArrayList<J3DHorizonPatch> getVisibleHorizons ()
    {
        ArrayList<J3DHorizonPatch> al = new ArrayList<J3DHorizonPatch> ();
        if (horizonList == null) {
            return al;
        }

        int size = horizonList.size ();
        J3DHorizonPatch  hp;

        for (int i=0; i<size; i++) {
            hp = horizonList.get(i);
            if (hp != null) {
                if (hp.isVisible()) {
                    al.add (hp);
                }
            }
        }

        return al;
    }

/*---------------------------------------------------------------------------------*/

/**
 *  Return an {@link ArrayList}<J3DLine> of all J3DLine objects currently visible.
 *  If there are none, the array list will be empty.
 */
    private ArrayList<J3DLine> getVisibleLines ()
    {
        if (lineList == null) {
            return null;
        }

        int i;

        int n = lineList.size ();

        J3DLine lp;

    /*
     * Create and populate the returned array of J3DLine
     * objects.
     */
        ArrayList<J3DLine> lines = new ArrayList<J3DLine> ();

        for (i=0; i<n; i++) {
            lp = lineList.get (i);
            if (lp == null) {
                continue;
            }
            if (lp.isVisible() == false) {
                continue;
            }
            lines.add (lp);
        }

        return lines;
    }

/*--------------------------------------------------------------------------------*/

/**
 *  Get a list of currently invisible faults.   If no faults are invisible, the list
 *  will be empty.
 */
    public ArrayList<J3DFault> getInvisibleFaults ()
    {
        ArrayList<J3DFault> al = new ArrayList<J3DFault> ();
        if (faultList == null) {
            return al;
        }

        int size = faultList.size ();
        J3DFault  fp;

        for (int i=0; i<size; i++) {
            fp = faultList.get(i);
            if (fp != null) {
                if (fp.isVisible()  ==  false) {
                    al.add (fp);
                }
            }
        }

        return al;
    }

/*---------------------------------------------------------------------------------*/

/**
 *  Get a list of currently invisible horizons.   If no horizons are invisible, the list
 *  will be empty.
 */
    public ArrayList<J3DHorizonPatch> getInvisibleHorizons ()
    {
        ArrayList<J3DHorizonPatch> al = new ArrayList<J3DHorizonPatch> ();
        if (horizonList == null) {
            return al;
        }

        int size = horizonList.size ();
        J3DHorizonPatch  hp;

        for (int i=0; i<size; i++) {
            hp = horizonList.get(i);
            if (hp != null) {
                if (hp.isVisible()  ==  false) {
                    al.add (hp);
                }
            }
        }

        return al;
    }

/*----------------------------------------------------------------------*

/**
 *  Return an {@link ArrayList}<J3DLine> of all J3DLine objects currently invisible.
 *  If there are none, the array list will be empty.
 */
    public ArrayList<J3DLine> getInvisibleLines ()
    {
        if (lineList == null) {
            return null;
        }

        int i;

        int n = lineList.size ();

        J3DLine lp;

    /*
     * Create and populate the returned array of J3DLine
     * objects.
     */
        ArrayList<J3DLine> lines = new ArrayList<J3DLine> ();

        for (i=0; i<n; i++) {
            lp = lineList.get (i);
            if (lp == null) {
                continue;
            }
            if (lp.isVisible() == true) {
                continue;
            }
            lines.add (lp);
        }

        return lines;

    }


/*--------------------Selected Getters ---------------------------------*/

/**
 *  Return an {@link ArrayList}<J3DLine> of all J3DLine objects currently selected regardless
 *  of the line visibilities.  If there are none, the array list will be empty.
 */
    public ArrayList<J3DLine> getSelectedLines ()
    {
        if (lineList == null) {
            return null;
        }

        int i;

        int n = lineList.size ();

        J3DLine lp;

    /*
     * Create and populate the returned array of J3DLine
     * objects.
     */
        ArrayList<J3DLine> lines = new ArrayList<J3DLine> ();

        for (i=0; i<n; i++) {
            lp = lineList.get (i);
            if (lp == null) {
                continue;
            }
            if (lp.isSelected() == false) {
                continue;
            }
            lines.add (lp);
        }

        return lines;
    }

  /*---------------------------------------------------------------------------------*/

/**
 *  Get a list of currently selected faults.   If no faults are selected, the list
 *  will be empty.
 */
    public ArrayList<J3DFault> getSelectedFaults ()
    {
        ArrayList<J3DFault> al = new ArrayList<J3DFault> ();
        if (faultList == null) {
            return al;
        }

        int size = faultList.size ();
        J3DFault  fp;

        for (int i=0; i<size; i++) {
            fp = faultList.get(i);
            if (fp != null) {
                if (fp.isSelected()) {
                    al.add (fp);
                }
            }
        }

        return al;
    }

/*---------------------------------------------------------------------------------*/

/**
 *  Get a list of currently selected horizons.   If no horizons are selected, the list
 *  will be empty.
 */
    public ArrayList<J3DHorizonPatch> getSelectedHorizons ()
    {
        ArrayList<J3DHorizonPatch> al = new ArrayList<J3DHorizonPatch> ();
        if (horizonList == null) {
            return al;
        }

        int size = horizonList.size ();
        J3DHorizonPatch  hp;

        for (int i=0; i<size; i++) {
            hp = horizonList.get(i);
            if (hp != null) {
                if (hp.isSelected()) {
                    al.add (hp);
                }
            }
        }

        return al;
    }

/*----------------------------------------------------------------------------------*/

/*
 * Manage the attributes that control the general drawing.
 */

    /**
    Use this for a perspective projection.
   */
    public static final int   PERSPECTIVE = 0;
   /**
    Use this for an orthogonal projection.
   */
    public static final int   ORTHO = 1;

    int                       projectionMode;
    Color backgroundColor = Color.black;
    private Color             selectionColor;

    private Color             lineSelectionColor;

    /**
     * Get the J3DDrawingAttributes currently in place.
     */
    public J3DDrawingAttributes getDrawingAttributes() {
      return drawingAttributes;
    }

/*-----------------------------------------------------------------------*/

/**
 *  If the projection is currently perspective, change it to orthographic.  If
 *  the projection is currently orthographic, change it to perspective.
 */
      public void toggleProjectionMode ()
      {
          if (projectionMode == PERSPECTIVE) {
              projectionMode = ORTHO;
          }
          else {
              projectionMode = PERSPECTIVE;
          }
          localRepaint ();
      }

/*-----------------------------------------------------------------------*/

      public int getProjectionMode ()
      {
          return projectionMode;
      }

/*--------------------------------------------------------------------------------*/

      public Color getForegroundColor ()
      {

      /*
       * If the background is dark, return a white foreground. If
       * the background is light, return a black foreground color.
       */
          return createContrastColor(backgroundColor);
      }

/*-----------------------------------------------------------------------*/

/**
 *  Set the projection mode to {@link #PERSPECTIVE} or {@link #ORTHO}.
 *  If the specified value is not one of these choices, a perspective
 *  projection mode is assumed.
 *  @param  ival The projection mode to use.
 */
      public void setProjectionMode (int ival)
      {
          if (ival < 0  ||  ival > 1) {
              ival = 0;
          }
          int ilast = projectionMode;
          projectionMode = ival;

          if (ilast != projectionMode) {
              localRepaint ();
          }
      }

/*-----------------------------------------------------------------------*/

/**
 * Creates a color that can be seen against the specified color.
 *
 * If the color is dark, the contrasting color will be white.  If
 * the color is light, the contrasting color will be black.
 *
 * @param color The color to contrast against.
 * @return the contrasting color
 */
      public Color createContrastColor (Color color) {

          Color contrastColor;

          float[] rgb = color.getRGBColorComponents (null);
          double rgbtot = rgb[0] + rgb[1] + rgb[2];
          if (rgbtot > 2.0) {
              contrastColor = new Color (0.0f, 0.0f, 0.0f);
          }
          else {
              contrastColor = new Color (1.0f, 1.0f, 1.0f);
          }

          return (contrastColor);
      }

/*-----------------------------------------------------------------------------*/

/**
 * Set the background color
 */
      public void setBackgroundColor(Color color) {
        backgroundColor = color;
      }


/*----------------------------  Previews  -------------------------------*/

      private J3DFault      previewFault = null;
      private J3DHorizonPatch    previewHorizonPatch = null;

/*----------------------------------------------------------------------*/

/**
 *  Set the preview fault surface of the display.  Preview faults are viewable
 *  but not pickable.  Only one preview fault can be shown at a time.  This is
 *  intended to be used by a fault editor to monitor changes in the surface
 *  prior to commiting the fault to the data.
 */
      public void setPreviewFault (J3DFault fp)
      {
          previewFault = fp;
          drawDirty = true;
          visibleLimitsUpdateNeeded = true;
      }

/*----------------------------------------------------------------------*/

/**
 *  Set the preview horizon surface of the display.  Preview horizons are viewable
 *  but not pickable.  Only one preview horizon can be shown at a time.  This is
 *  intended to be used by a horizon editor to monitor changes in the surface
 *  prior to commiting the horizon patch to the data.
 */
    public void setPreviewHorizon (J3DHorizonPatch fp)
    {
        previewHorizonPatch = fp;
        drawDirty = true;
        visibleLimitsUpdateNeeded = true;
    }

/*----------------------------------------------------------------------------*/

/*
 * Functions to hide or selet based on object type.
 */

/*----------------------------------------------------------------------------*/

/**
 *  Make all objects invisible.  The select state of the objects
 *  is not changed.
 */
      public void hideAllObjects ()
      {
          doRepaint = false;
          hideAllHorizons ();
          hideAllFaults ();
          hideAllLines ();
          doRepaint = true;
          localRepaint ();
      }

/*----------------------------------------------------------------------*/

/**
 *  Make all faults invisible.  The select state of the faults
 *  is not changed.
 */
      public void hideAllFaults ()
      {

          int                i, n;
          J3DFault      fp;

      /*
       * Make fault surfaces invisible.
       */
          if (faultList != null) {
              n = faultList.size();
              for (i=0; i<n; i++) {
                fp = faultList.get(i);
                if (fp == null) {
                    continue;
                }
                if (fp.isVisible() == false) {
                    continue;
                }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 *  Make all horizon surfaces invisible.  The select state of the
 *  horizons is not changed.
 */
      public void hideAllHorizons ()
      {
          int                i, n;
          J3DHorizonPatch    hp;

      /*
       * Make horizon surfaces invisible.
       */
          if (horizonList != null) {
              n = horizonList.size();
              for (i=0; i<n; i++) {
                  hp = horizonList.get(i);
                  if (hp == null) {
                      continue;
                  }
                  if (hp.isVisible() == false) {
                      continue;
                  }

              }

              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 *  Make all lines invisible.  The select state of the
 *  lines is not changed.
 */
      public void hideAllLines ()
      {
          int                i, n;
          J3DLine            lp;

      /*
       * Make selected lines invisible.
       */
          if (lineList != null) {
              n = lineList.size();
              for (i=0; i<n; i++) {
                  lp = lineList.get(i);
                  if (lp == null) {
                      continue;
                  }
                  if (lp.isVisible() == false) {
                      continue;
                  }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 *  Make all selected faults invisible.  The select state of the faults
 *  is not changed.
 */
      public void hideSelectedObjects ()
      {
          doRepaint = false;
          hideSelectedHorizons ();
          hideSelectedFaults ();
          hideSelectedLines ();
          doRepaint = true;
          localRepaint ();
      }

/*----------------------------------------------------------------------*/

/**
 *  Make all selected faults invisible.  The select state of the faults
 *  is not changed.
 */
      public void hideSelectedFaults ()
      {

          int                i, n;
          J3DFault      fp;

      /*
       * Make selected fault surfaces invisible.
       */
          if (faultList != null) {
              n = faultList.size();
              for (i=0; i<n; i++) {
                fp = faultList.get(i);
                if (fp == null) {
                    continue;
                }
                if (fp.isVisible() == false) {
                    continue;
                }
                if (fp.isSelected() == false) {
                    continue;
                }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 *  Make all selected horizon surfaces invisible.  The select state of the
 *  horizons is not changed.
 */
      public void hideSelectedHorizons ()
      {
          int                i, n;
          J3DHorizonPatch    hp;
          ArrayList<J3DHorizonPatch> selectedPatches = new ArrayList<J3DHorizonPatch>();
      /*
       * Make selected horizon surfaces invisible.
       */


/*
          if (horizonList != null) {
              n = horizonList.size();
              for (i=0; i<n; i++) {
                  hp = horizonList.get(i);
                  if (hp == null) {
                      continue;
                  }
                  if (hp.isVisible() == false) {
                      continue;
                  }
                  if (hp.isSelected() == false) {
                      continue;
                  }

                  selectedPatches.add(hp.getCallbackData());

              }
              callSelectListeners ();
              localRepaint ();
          }
*/

      }

/*----------------------------------------------------------------------*/

/**
 *  Make all selected lines invisible.  The select state of the
 *  lines is not changed.
 */
      public void hideSelectedLines ()
      {
          int                i, n;
          J3DLine            lp;

      /*
       * Make selected lines invisible.
       */
          if (lineList != null) {
              n = lineList.size();
              for (i=0; i<n; i++) {
                  lp = lineList.get(i);
                  if (lp == null) {
                      continue;
                  }
                  if (lp.isVisible() == false) {
                      continue;
                  }
                  if (lp.isSelected() == false) {
                      continue;
                  }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 *  Make all fault surfaces, horizon surfaces and lines in the 3d display visible.
 */
      public void showAllObjects ()
      {
          showAllFaults ();
          showAllHorizons ();
          showAllLines ();
      }

/*----------------------------------------------------------------------*/

/**
 *  Make all surfaces in the 3d display visible.  This does not affect lines
 *  in the display.
 */
      public void showAllFaults ()
      {

          int                i, n;
          J3DFault      fp;

      /*
       * Make all faults visible.
       */
          if (faultList != null) {
              n = faultList.size();
              for (i=0; i<n; i++) {
                  fp = faultList.get(i);
                  if (fp == null) {
                      continue;
                  }
                  if (fp.isVisible() == true) {
                      continue;
                  }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*-----------------------------------------------------------------------*/

/**
 * Make all horizons visible.
 */
      public void showAllHorizons ()
      {

          int                i, n;
          J3DHorizonPatch    hp;

          if (horizonList != null) {
              n = horizonList.size();
              for (i=0; i<n; i++) {
                  hp = horizonList.get(i);
                  if (hp == null) {
                      continue;
                  }
                  if (hp.isVisible() == true) {
                      continue;
                  }

              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*-----------------------------------------------------------------------*/

/**
 * Make all lines visible.
 */
      public void showAllLines ()
      {

          int                i, n;
          J3DLine            lp;

          if (lineList != null) {
              n = lineList.size();
              for (i=0; i<n; i++) {
                  lp = lineList.get(i);
                  if (lp == null) {
                      continue;
                  }
                  if (lp.isVisible() == true) {
                      continue;
                  }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 * Change all objects in the panel to selected.  This acts on
 * visible and invisible objects.  No {@link J3DSelectListener} is
 * called as the result of calling this method.
 */
      public void selectAllObjects ()
      {
          selectAllFaults ();
          selectAllHorizons ();
          selectAllLines ();
      }

/*----------------------------------------------------------------------*/

/**
 * Change all faults in the panel to selected.  This acts on
 * visible objects.  No {@link J3DSelectListener} is
 * called as the result of calling this method.
 */
      public void selectAllFaults ()
      {

          int                i, n;
          J3DFault      fp;

      /*
       * Select and recolor selected fault surfaces.
       */
          if (faultList != null) {
              n = faultList.size();
              for (i=0; i<n; i++) {
                  fp = faultList.get(i);
                  if (fp == null) {
                      continue;
                  }
                  if (!(fp.isVisible())) {
                      continue;
                  }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 * Change all horizons in the panel to selected.  This acts on
 * visible objects.  No {@link J3DSelectListener} is
 * called as the result of calling this method.
 */
      public void selectAllHorizons ()
      {

          int                i, n;
          J3DHorizonPatch    hp;

      /*
       * Select and recolor selected horizon surfaces.
       */
          if (horizonList != null) {
              n = horizonList.size();
              for (i=0; i<n; i++) {
                  hp = horizonList.get(i);
                  if (hp == null) {
                      continue;
                  }
                  if (!(hp.isVisible())) {
                      continue;
                  }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 * Change all lines in the panel to selected.  This acts on
 * visible objects.  No {@link J3DSelectListener} is
 * called as the result of calling this method.
 */
      public void selectAllLines ()
      {

          int                i, n;
          J3DLine            lp;

      /*
       * Select and recolor selected horizon surfaces.
       */
          if (lineList != null) {
              n = lineList.size();
              for (i=n-1; i>=0; i--) {
                  lp = lineList.get(i);
                  if (lp == null) {
                      continue;
                  }
                  if (!(lp.isVisible())) {
                      continue;
                  }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 * Change all objects in the panel to unselected.  This acts on
 * visible and invisible objects.  No {@link J3DSelectListener} is
 * called as the result of calling this method.
 */
      public void unselectAllObjects ()
      {
          unselectAllFaults ();
          unselectAllHorizons ();
          unselectAllLines ();
      }

/*----------------------------------------------------------------------*/

/**
 * Change all faults in the panel to unselected.  This acts on
 * visible and invisible objects.  No {@link J3DSelectListener} is
 * called as the result of calling this method.
 */
      public void unselectAllFaults ()
      {

          int                i, n;
          J3DFault      fp;

      /*
       * Unselect and recolor selected fault surfaces.
       */
          if (faultList != null) {
              n = faultList.size();
              for (i=0; i<n; i++) {
                  fp = faultList.get(i);
                  if (fp == null) {
                      continue;
                  }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 * Change all horizons in the panel to unselected.  This acts on
 * visible and invisible objects.  No {@link J3DSelectListener} is
 * called as the result of calling this method.
 */
      public void unselectAllHorizons ()
      {

          int                i, n;
          J3DHorizonPatch    hp;

      /*
       * Unselect and recolor selected horizon surfaces.
       */
          if (horizonList != null) {
              n = horizonList.size();
              for (i=0; i<n; i++) {
                  hp = horizonList.get(i);
                  if (hp == null) {
                      continue;
                  }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/**
 * Change all lines in the panel to unselected.  This acts on
 * visible and invisible objects.  No {@link J3DSelectListener} is
 * called as the result of calling this method.
 */
      public void unselectAllLines ()
      {

          int                i, n;
          J3DLine            lp;

      /*
       * Unselect and recolor selected horizon surfaces.
       */
          if (lineList != null) {
              n = lineList.size();
              for (i=n-1; i>=0; i--) {
                  lp = lineList.get(i);
                  if (lp == null) {
                      continue;
                  }
              }
              callSelectListeners ();
              localRepaint ();
          }
      }

/*----------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

    /**
     * Private inner class providing keyboard actions.
     */
    private class KeyboardActions {

        private void setupKeyboardActions ()
        {
            KeyStroke     upKS,
                          downKS,
                          homeKS,
                          LKS,
                          RKS,
                          shiftRKS,
                          shiftLKS,
                          UKS,
                          DKS,
                          shiftUKS,
                          shiftDKS,
                          NKS,
                          FKS,
                          shiftNKS,
                          shiftFKS,
                          lKS,
                          rKS,
                          shiftrKS,
                          shiftlKS,
                          uKS,
                          dKS,
                          shiftuKS,
                          shiftdKS,
                          nKS,
                          fKS,
                          shiftnKS,
                          shiftfKS,
                          HKS,
                          hKS,
                          shifthKS,
                          shiftHKS,
                          shiftUpKS,
                          shiftDownKS,
                          controlUpKS,
                          controlDownKS,
                          shiftControlUpKS,
                          shiftControlDownKS;

            upKS = KeyStroke.getKeyStroke ("UP");
            downKS = KeyStroke.getKeyStroke ("DOWN");
            homeKS = KeyStroke.getKeyStroke ("HOME");
            HKS = KeyStroke.getKeyStroke ("control H");
            hKS = KeyStroke.getKeyStroke ("control h");
            shifthKS = KeyStroke.getKeyStroke ("control shift h");
            shiftHKS = KeyStroke.getKeyStroke ("control shift H");
            shiftUpKS = KeyStroke.getKeyStroke ("shift UP");
            shiftDownKS = KeyStroke.getKeyStroke ("shift DOWN");
            controlUpKS = KeyStroke.getKeyStroke ("control UP");
            controlDownKS = KeyStroke.getKeyStroke ("control DOWN");
            shiftControlUpKS = KeyStroke.getKeyStroke ("shift control UP");
            shiftControlDownKS = KeyStroke.getKeyStroke ("shift control DOWN");

            LKS = KeyStroke.getKeyStroke ("control L");
            RKS = KeyStroke.getKeyStroke ("control R");
            shiftLKS = KeyStroke.getKeyStroke ("control shift L");
            shiftRKS = KeyStroke.getKeyStroke ("control shift R");
            UKS = KeyStroke.getKeyStroke ("control U");
            DKS = KeyStroke.getKeyStroke ("control D");
            shiftUKS = KeyStroke.getKeyStroke ("control shift U");
            shiftDKS = KeyStroke.getKeyStroke ("control shift D");
            NKS = KeyStroke.getKeyStroke ("control N");
            FKS = KeyStroke.getKeyStroke ("control F");
            shiftNKS = KeyStroke.getKeyStroke ("control shift N");
            shiftFKS = KeyStroke.getKeyStroke ("control shift F");

            lKS = KeyStroke.getKeyStroke ("control l");
            rKS = KeyStroke.getKeyStroke ("control r");
            shiftlKS = KeyStroke.getKeyStroke ("control shift l");
            shiftrKS = KeyStroke.getKeyStroke ("control shift r");
            uKS = KeyStroke.getKeyStroke ("control u");
            dKS = KeyStroke.getKeyStroke ("control d");
            shiftuKS = KeyStroke.getKeyStroke ("control shift u");
            shiftdKS = KeyStroke.getKeyStroke ("control shift d");
            nKS = KeyStroke.getKeyStroke ("control n");
            fKS = KeyStroke.getKeyStroke ("control f");
            shiftnKS = KeyStroke.getKeyStroke ("control shift n");
            shiftfKS = KeyStroke.getKeyStroke ("control shift f");

            ActionMap amap = getActionMap ();
            InputMap imap = getInputMap (JComponent.WHEN_IN_FOCUSED_WINDOW);
            //InputMap imap = getInputMap (JComponent.WHEN_FOCUSED);

            upACT up = new upACT();
            amap.put ("up", up);
            imap.put (upKS, "up");

            downACT down = new downACT();
            amap.put ("down", down);
            imap.put (downKS, "down");

            homeACT home = new homeACT();
            amap.put ("home", home);
            imap.put (homeKS, "home");
            imap.put (hKS, "home");
            imap.put (HKS, "home");
            imap.put (shifthKS, "home");
            imap.put (shiftHKS, "home");

            shiftUpACT shiftUp = new shiftUpACT();
            amap.put ("shiftUp", shiftUp);
            imap.put (shiftUpKS, "shiftUp");

            shiftDownACT shiftDown = new shiftDownACT();
            amap.put ("shiftDown", shiftDown);
            imap.put (shiftDownKS, "shiftDown");

            controlUpACT controlUp = new controlUpACT();
            amap.put ("controlUp", controlUp);
            imap.put (controlUpKS, "controlUp");

            controlDownACT controlDown = new controlDownACT();
            amap.put ("controlDown", controlDown);
            imap.put (controlDownKS, "controlDown");

            shiftControlUpACT shiftControlUp = new shiftControlUpACT();
            amap.put ("shiftControlUp", shiftControlUp);
            imap.put (shiftControlUpKS, "shiftControlUp");

            shiftControlDownACT shiftControlDown = new shiftControlDownACT();
            amap.put ("shiftControlDown", shiftControlDown);
            imap.put (shiftControlDownKS, "shiftControlDown");

            leftACT left = new leftACT();
            amap.put ("left", left);
            imap.put (LKS, "left");
            imap.put (lKS, "left");

            rightACT right = new rightACT();
            amap.put ("right", right);
            imap.put (RKS, "right");
            imap.put (rKS, "right");

            shiftLeftACT shiftLeft = new shiftLeftACT();
            amap.put ("shiftLeft", shiftLeft);
            imap.put (shiftLKS, "shiftLeft");
            imap.put (shiftlKS, "shiftLeft");

            shiftRightACT shiftRight = new shiftRightACT();
            amap.put ("shiftRight", shiftRight);
            imap.put (shiftRKS, "shiftRight");
            imap.put (shiftrKS, "shiftRight");

            UACT U = new UACT();
            amap.put ("U", U);
            imap.put (UKS, "U");
            imap.put (uKS, "U");

            DACT D = new DACT();
            amap.put ("D", D);
            imap.put (DKS, "D");
            imap.put (dKS, "D");

            shiftUACT shiftU = new shiftUACT();
            amap.put ("shiftU", shiftU);
            imap.put (shiftUKS, "shiftU");
            imap.put (shiftuKS, "shiftU");

            shiftDACT shiftD = new shiftDACT();
            amap.put ("shiftD", shiftD);
            imap.put (shiftDKS, "shiftD");
            imap.put (shiftdKS, "shiftD");

            FACT F = new FACT();
            amap.put ("F", F);
            imap.put (FKS, "F");
            imap.put (fKS, "F");

            NACT N = new NACT();
            amap.put ("N", N);
            imap.put (NKS, "N");
            imap.put (nKS, "N");

            shiftFACT shiftF = new shiftFACT();
            amap.put ("shiftF", shiftF);
            imap.put (shiftFKS, "shiftF");
            imap.put (shiftfKS, "shiftF");

            shiftNACT shiftN = new shiftNACT();
            amap.put ("shiftN", shiftN);
            imap.put (shiftNKS, "shiftN");
            imap.put (shiftnKS, "shiftN");
        }

    /*------------------------------------------------------------------------------*/

        private class upACT extends AbstractAction
        {
            public static final long    serialVersionUID = 12349876;

            upACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                xScale /= 1.15;
                yScale /= 1.15;
                zScale /= 1.15;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class downACT extends AbstractAction
        {
            public static final long    serialVersionUID = 22349876;
            downACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                xScale *= 1.15;
                yScale *= 1.15;
                zScale *= 1.15;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class homeACT extends AbstractAction
        {
            public static final long    serialVersionUID = 32349876;
            homeACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                resetToDefaultTransform ();
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class shiftUpACT extends AbstractAction
        {
            public static final long    serialVersionUID = 42349876;
            shiftUpACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                xScale /= 1.02;
                yScale /= 1.02;
                zScale /= 1.02;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class shiftDownACT extends AbstractAction
        {
            public static final long    serialVersionUID = 42349876;
            shiftDownACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                xScale *= 1.02;
                yScale *= 1.02;
                zScale *= 1.02;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class controlUpACT extends AbstractAction
        {
            public static final long    serialVersionUID = 52349876;
            controlUpACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {

            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class controlDownACT extends AbstractAction
        {
            public static final long    serialVersionUID = 62349876;
            controlDownACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {

            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class shiftControlUpACT extends AbstractAction
        {
            public static final long    serialVersionUID = 72349876;
            shiftControlUpACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {

            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class shiftControlDownACT extends AbstractAction
        {
            public static final long    serialVersionUID = 82349876;
            shiftControlDownACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {

            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class leftACT extends AbstractAction
        {
            public static final long    serialVersionUID = 92349876;
            leftACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                xTrans -= .15;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class rightACT extends AbstractAction
        {
            public static final long    serialVersionUID = 192349876;
            rightACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                xTrans += .15;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class
    /*------------------------------------------------------------------------------*/

        private class shiftLeftACT extends AbstractAction
        {
            public static final long    serialVersionUID = 292349876;
            shiftLeftACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                xTrans -= .02;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class shiftRightACT extends AbstractAction
        {
            public static final long    serialVersionUID = 392349876;
            shiftRightACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                xTrans += .02;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class


    /*------------------------------------------------------------------------------*/

        private class UACT extends AbstractAction
        {
            public static final long    serialVersionUID = 492349876;
            UACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                yTrans += .15;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class DACT extends AbstractAction
        {
            public static final long    serialVersionUID = 592349876;
            DACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                yTrans -= .15;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class
    /*------------------------------------------------------------------------------*/

        private class shiftUACT extends AbstractAction
        {
            public static final long    serialVersionUID = 692349876;
            shiftUACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                yTrans += .02;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class shiftDACT extends AbstractAction
        {
            public static final long    serialVersionUID = 792349876;
            shiftDACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                yTrans -= .02;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class NACT extends AbstractAction
        {
            public static final long    serialVersionUID = 892349876;
            NACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                zTrans += .15;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class FACT extends AbstractAction
        {
            public static final long    serialVersionUID = 992349876;
            FACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                zTrans -= .15;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class
    /*------------------------------------------------------------------------------*/

        private class shiftNACT extends AbstractAction
        {
            public static final long    serialVersionUID = 1992349876;
            shiftNACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                zTrans += .02;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    /*------------------------------------------------------------------------------*/

        private class shiftFACT extends AbstractAction
        {
            public static final long    serialVersionUID = 29923498;
            shiftFACT ()
            {
                super ();
            }

            public void actionPerformed (ActionEvent e)
            {
                zTrans -= .02;
                J3DPanel.this.isMoving = true;
                J3DPanel.this.localRepaint();
            }

        } // end of private action inner class

    } // end of private KeyboardActions inner class

/*---------------------------------------------------------------------------------*/
/*               End of KeyboardActions inner class                                */
/*---------------------------------------------------------------------------------*/


    private boolean debugProperties = false;
    private int NumSetProp = 0;

    public void setDebugProperties (boolean bval)
    {
      debugProperties = true;
    }

/*---------------------------------------------------------------------------------*/

    public void resetAtValues ()
    {
        resetFaultAts ();
        resetHorizonAts ();
    }

/*---------------------------------------------------------------------------------*/

  /**
  Remove the attribute values from all faults.
  */
    public void resetFaultAts ()
    {
        if (faultList == null)
          return;

        J3DSurfacePatch surf;
        int size = faultList.size ();
        for (int i=0; i<size; i++) {
            surf = (J3DSurfacePatch)faultList.get(i);
            if (surf.wireListID > 0) {
                jgl.DeleteList (surf.wireListID);
                surf.wireListID = -surf.wireListID;
            }
            if (surf.solidListID > 0) {
                jgl.DeleteList (surf.solidListID);
                surf.solidListID = -surf.solidListID;
            }
            if (surf.tm3d != null) {
                surf.tm3d.setAttributeColorsDirectly (null, null, null, null, 0);
            }
        }
    }

/*---------------------------------------------------------------------------------*/

  /**
  Remove the attribute values from all horizons.
  */
    public void resetHorizonAts ()
    {
        if (horizonList == null)
          return;

        J3DSurfacePatch surf;
        int size = horizonList.size ();
        for (int i=0; i<size; i++) {
            surf = horizonList.get(i);
            if (surf.wireListID > 0) {
                jgl.DeleteList (surf.wireListID);
                surf.wireListID = -surf.wireListID;
            }
            if (surf.solidListID > 0) {
                jgl.DeleteList (surf.solidListID);
                surf.solidListID = -surf.solidListID;
            }
            if (surf.tm3d != null) {
                surf.tm3d.setAttributeColorsDirectly (null, null, null, null, 0);
            }
        }
    }


  /**
  */
  public void setIsPropertyApply (boolean bval) 
  {
    isPropertyApply = bval;
    if (bval) {
      drawingAttributes.setViewProperties (view3DProperties);
        if (view3DProperties != null  &&  canvas != null) {
          Color _bgc = view3DProperties.getBackgroundColor();
          if (_bgc != null) {
            canvas.setBackground (_bgc);
          }
        }
    }
  }

}  // end of main class
