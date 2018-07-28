
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

/**
 Package to draw 2d graphical views.
 <p>
 Good starting points for this package documentation are the
 {@link JEasyXGraphicsPanel} and the {@link JDisplayListPanel} classes.  Each
 of these is a swing component that can be put into a hierarchy of other swing
 components and then it can be drawn to.
 <p>
 The actual drawing is done via methods on the {@link JDisplayList} class.
 You cannot create an instance of the display list from an application.  When
 you create either of the swing components for drawing, a display list is
 implicitly created.  You can then retrieve a reference to that display list
 from the component object created.
 <p>
 You will also need to use the DLConst class, but you will not need an instance
 of it.  The DLConst class has public and non public constants needed by
 methods on the other classes in the package.  I put these in a class by
 themselves because they are often used by more than one of the package classes.
 Also, the class has a short name that is a bit easier to deal with when
 defining a constant in application code.
*/
package csw.jeasyx.src;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JToolBar;
import javax.swing.SwingConstants;

import csw.uistuff.JEasyXIcons;
import csw.jutils.src.CSW_UI;

/*---------------------------------------------------------------------------*/

/**
 * This class is a swing component that combines a {@link JDisplayListPanel}
 * and zoom/pan/redraw control buttons.  If the drawing needs to be interacted
 * with, this is probably the appropriate component to use for the drawing.
 * If you just want a static, minimally interactive drawing, create an instance
 * of JDisplayListPanel directly and draw to its display list.
 *
 * @author Glenn Pinkerton
 */
public class JEasyXGraphicsPanel extends JPanel {

    private static final long serialVersionUID = 1L;

 /*
  * graphPanel has package visibility, instead of private, in order to avoid
  * eclipse compile warnings when accessing graphPanel from actionPerformed():
  * "Read access to enclosing field JEasyXGraphicsPanel.graphPanel is emulated
  * by a synthetic accessor method. Increasing its visibility will improve your
  * performance."
  */
  JDisplayListPanel graphPanel;

  private JToolBar uiControlBox;

/*---------------------------------------------------------------------------*/

  private void createPanel(String layoutDirection, boolean useToolbar, int mask)
  {

    graphPanel = new JDisplayListPanel();

    setLayout(new BorderLayout());

    /*
     * Put the actual drawing panel in the center.
     */
    add(graphPanel, BorderLayout.CENTER);

    if (useToolbar) {
      /*
       * Create a control button box on the NORTH, SOUTH, EAST or WEST
       * border depending upon the current value of the layoutDirection
       * member.  The default is WEST.
       */
      uiControlBox = new JToolBar();

      if (layoutDirection.compareTo(BorderLayout.NORTH) == 0) {
        add(uiControlBox, BorderLayout.NORTH);
      } else if (layoutDirection.compareTo(BorderLayout.SOUTH) == 0) {
        add(uiControlBox, BorderLayout.SOUTH);
      } else if (layoutDirection.compareTo(BorderLayout.EAST) == 0) {
        uiControlBox.setOrientation (SwingConstants.VERTICAL);
        add(uiControlBox, BorderLayout.EAST);
      } else {
        // BorderLayout.WEST or invalid string
        uiControlBox.setOrientation (SwingConstants.VERTICAL);
        add(uiControlBox, BorderLayout.WEST);
      }

      /*
       * Set standard look and feel for JToolBars.
       */
      CSW_UI.setToolbarLookAndFeel(uiControlBox);

      /*
       * Create the toolbar buttons as needed according to the
       * mask member.
       */
      Dimension preferredSize = new Dimension(24, 24);

      // Select button
      if ((mask & DLConst.SELECT_BUTTON_MASK)  !=  0) {
        createButton(
          JEasyXIcons.SELECT,
          "Set to Selection Mode",
          new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
              graphPanel.startSelectMode();
            }
          },
          preferredSize
        );
        graphPanel.setSelectionAllowed(true);
      }

      // Unselect button
      if ((mask & DLConst.UNSELECT_BUTTON_MASK)  !=  0) {
        createButton(
          JEasyXIcons.UNSELECT,
          "Unselect All",
          new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
              graphPanel.startUnselectMode();
            }
          },
          preferredSize
        );
      }

      uiControlBox.addSeparator();

      // Zoom in button
      if ((mask & DLConst.ZOOM_IN_BUTTON_MASK)  !=  0) {
        createButton(
          JEasyXIcons.ZOOM_IN,
          "Zoom In at a Point",
          new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
              graphPanel.startZoomInMode();
            }
          },
          preferredSize
        );
      }

      // Zoom out button
      if ((mask & DLConst.ZOOM_OUT_BUTTON_MASK)  !=  0) {
        createButton(
          JEasyXIcons.ZOOM_OUT,
          "Zoom Out from the Center",
          new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
              graphPanel.startZoomOutMode();
            }
          },
          preferredSize
        );
      }

      // Zoom rectangle button
      if ((mask & DLConst.ZOOM_BUTTON_MASK)  !=  0) {
        createButton(
          JEasyXIcons.ZOOM,
          "Zoom to Rectangle",
          new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
              graphPanel.startZoomToRectangleMode();
            }
          },
          preferredSize
        );
      }

      // Zoom extents button
      if ((mask & DLConst.ZOOM_FULL_BUTTON_MASK)  !=  0) {
        createButton(
          JEasyXIcons.ZOOM_FULL,
          "Zoom to Data Extents",
          new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
              graphPanel.startZoomExtentsMode();
            }
          },
          preferredSize
        );
      }

      uiControlBox.addSeparator();

      // pan button
      if ((mask & DLConst.PAN_BUTTON_MASK) != 0) {
        createButton(
          JEasyXIcons.PAN,
          "Pan",
          new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
              graphPanel.startPanMode();
            }
          },
          preferredSize
        );
      }

      uiControlBox.addSeparator();

      // redraw button
      if ((mask & DLConst.REFRESH_BUTTON_MASK) != 0) {
        createButton(
          JEasyXIcons.REFRESH,
          "Refresh the drawing",
          new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
              graphPanel.redraw();
            }
          },
          preferredSize
        );
      }

      // print button
      if ((mask & DLConst.PRINT_BUTTON_MASK) != 0) {
        createButton(
          JEasyXIcons.PRINT,
          "Print (not yet working)",
          new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
              //graphPanel.print();
            }
          },
          preferredSize
        );
      }

      uiControlBox.addSeparator();
      uiControlBox.addSeparator();

    }
  }

/*--------------------------------------------------------------------------*/

 /*
  * Build the additionalButtonList one button at a time.
  * I use it for edit buttons, etc.
  */
  public void addAdditionalTextButton (
    String labelText,
    String toolTipText,
    ActionListener al,
    Dimension preferredSize)
  {
    JButton button = new JButton (labelText);
    if (toolTipText != null) {
      button.setToolTipText(toolTipText);
    }
    if (al != null) {
      button.addActionListener(al);
    }
    if (preferredSize != null) {
      button.setMaximumSize(preferredSize);
    }
    addUIControl(button);
    CSW_UI.setToolbarButtonLookAndFeel(button);
  }

 /*
  * Convenience method to create a button for the toolbar.
  */
  private void createButton(
    String type,
    String toolTipText,
    ActionListener al,
    Dimension preferredSize
  )
  {
    JButton button = JEasyXIcons.createJButton(type);
    if (toolTipText != null) {
      button.setToolTipText(toolTipText);
    }
    if (al != null) {
      button.addActionListener(al);
    }
    if (preferredSize != null) {
      button.setMaximumSize(preferredSize);
    }
    addUIControl(button);
    CSW_UI.setToolbarButtonLookAndFeel(button);
  }

  public void addUIControl(JComponent jc) {
    uiControlBox.add(jc);
  }
  public void addUIControlAndSeparator(JComponent jc) {
    uiControlBox.add(jc);
    uiControlBox.addSeparator();
  }
  public void addUISeparator() {
    uiControlBox.addSeparator();
  }
  public void removeUIControl(JComponent jc) {
    uiControlBox.remove(jc);
  }
/*---------------------------------------------------------------------------*/

  /**
   * Create a graphics panel with zoom/pan controls along the top side.
   */
  public JEasyXGraphicsPanel() {
    super();
    createPanel(BorderLayout.NORTH, true/*useToolbar*/, 0xffffffff);
  }

  public JEasyXGraphicsPanel(boolean useToolbar) {
    super();
    createPanel(BorderLayout.NORTH, useToolbar, 0xffffffff);
  }

/*---------------------------------------------------------------------------*/

  /**
   * Create a graphics panel with zoom/pan controls, specifying where
   * to layout the buttons.
   *
   * @param layoutDirection specifies where to layout the buttons
   * (preferably use the BorderLayout constants NORTH, SOUTH, EAST
   * or WEST to represent the strings "North", "South", "East" or "West").
   */
  public JEasyXGraphicsPanel(String layoutDirection) {
    super();
    createPanel(layoutDirection, true/*useToolbar*/, 0xffffffff);
  }

/*---------------------------------------------------------------------------*/

  /**
   * Create a graphics panel with zoom/pan controls along the top side,
   * specifying which buttons are visible.
   *
   * @param buttonMask This mask is a bitwise or of the following constants:
   * <ul>
     <li> {@link DLConst#SELECT_BUTTON_MASK}
     <li> {@link DLConst#UNSELECT_BUTTON_MASK}
     <li> {@link DLConst#ZOOM_IN_BUTTON_MASK}
     <li> {@link DLConst#ZOOM_OUT_BUTTON_MASK}
     <li> {@link DLConst#ZOOM_BUTTON_MASK}
     <li> {@link DLConst#ZOOM_FULL_BUTTON_MASK}
     <li> {@link DLConst#PAN_BUTTON_MASK}
     <li> {@link DLConst#REFRESH_BUTTON_MASK}
     <li> {@link DLConst#PRINT_BUTTON_MASK}
     </ul>
   */
  public JEasyXGraphicsPanel(int buttonMask) {
    super();
    createPanel(BorderLayout.NORTH, true/*useToolbar*/, buttonMask);
  }

/*---------------------------------------------------------------------------*/

  /**
   * Create a graphics panel with zoom/pan controls, specifying where
   * to layout the buttons and which buttons are visible.
   *
   * @param layoutDirection specifies where to layout the buttons
   * (preferably use the BorderLayout constants NORTH, SOUTH, EAST
   * or WEST to represent the strings "North", "South", "East" or "West").
   * @param buttonMask This mask is a bitwise or of the following constants:
   * <ul>
     <li> {@link DLConst#SELECT_BUTTON_MASK}
     <li> {@link DLConst#UNSELECT_BUTTON_MASK}
     <li> {@link DLConst#ZOOM_IN_BUTTON_MASK}
     <li> {@link DLConst#ZOOM_OUT_BUTTON_MASK}
     <li> {@link DLConst#ZOOM_BUTTON_MASK}
     <li> {@link DLConst#ZOOM_FULL_BUTTON_MASK}
     <li> {@link DLConst#PAN_BUTTON_MASK}
     <li> {@link DLConst#REFRESH_BUTTON_MASK}
     <li> {@link DLConst#PRINT_BUTTON_MASK}
     </ul>
   */
  public JEasyXGraphicsPanel(String layoutDirection, int buttonMask) {
    super();
    createPanel(layoutDirection, true/*useToolbar*/, buttonMask);
  }

/*---------------------------------------------------------------------------*/

  /**
  Return a reference to the JDisplayListPanel object that is implicitly
  created by the JEasyXGraphicsPanel.
  */
  public JDisplayListPanel getDisplayListPanel () {
    return graphPanel;
  }

  public void forceZoomQuit ()
  {
    graphPanel.resetModes ();
  }


/*---------------------------------------------------------------------------*/

  /**
  Return a reference to the JDisplayList object that is implicitly created by
  the JEasyXGraphicsPanel.  Any modification done to this display
  list object will always be reflected in this graphics panel.
  */
  public JDisplayList getDisplayList () {
    return graphPanel.getDisplayList ();
  }

/*---------------------------------------------------------------------------*/

  /**
  Clean up all the native resources associated with this panel. If the panel
  becomes unuseful without its ancestor frame being closed, you can call this
  method to clean up all the native resources used to support the drawing on
  the panel.
  */
  public void cleanup ()
  {
    graphPanel.cleanup ();
  }

}
