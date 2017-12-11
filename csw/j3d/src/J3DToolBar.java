
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.awt.Frame;
import java.awt.Point;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JCheckBox;
//import javax.swing.JComboBox;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JToolBar;

import csw.uistuff.JEasyXIcons;

import csw.jutils.src.View3DProperties;
import csw.jutils.src.TranslationManager;
import csw.jutils.src.CSW_UI;


/**
 * The toolbar for the top of the J3DPanel is created and managed by
 * this class.  This class has package scope.  It does not need to be
 * referenced outside the J3D package.
 *
 * @author Glenn Pinkerton
 */
class J3DToolBar extends JToolBar {

    public static final long    serialVersionUID = 33723498;

    private J3DPanel panel;
    private JButton selectButton;
    private JButton setHomePositionButton;
    private JButton homePositionButton;
    private JButton viewPositionButton;
    private JButton orthogonalOnOffButton;
    private JButton viewAllButton;
    private JButton targetZoomButton;
    private JButton zoomOutButton;
    private JButton optionsButton;

    /*
    private JButton unselectButton;
    private JButton hideButton;
    private JButton unhideButton;
    */

    private JPopupMenu positionPopup;
    private JMenuItem northItem;
    private JMenuItem southItem;
    private JMenuItem eastItem;
    private JMenuItem westItem;
    private JMenuItem topItem;
    private JMenuItem bottomItem;

    /*
    private JPopupMenu unselectPopup;
    private JMenuItem unselectAllItem;
    private JMenuItem unselectSurfItem;
    private JMenuItem unselectFaultItem;
    private JMenuItem unselectLineItem;

    private JPopupMenu hidePopup;
    private JMenuItem hideAllItem;
    private JMenuItem hideSurfItem;
    private JMenuItem hideFaultItem;
    private JMenuItem hideLineItem;

    private JPopupMenu unhidePopup;
    private JMenuItem unhideAllItem;
    private JMenuItem unhideSurfItem;
    private JMenuItem unhideFaultItem;
    private JMenuItem unhideLineItem;
    */

    private Frame parentFrame;
    private View3DPropertiesDialog   optionsDialog;

/*----------------------------------------------------------------------------*/

  /*
   * The constructor is given the parent J3DPanel object.  If the parent
   * is null, nothing is created.  The constructor has package scope.
   * An instance of this class is only created from the J3DPanel class.
   */
    J3DToolBar (J3DPanel parent) {
      super();

      if (parent == null) {
        return;
      }

      panel = parent;

    /*
     * Set standard ModelBuilder look and feel for JToolBars.
     */
      CSW_UI.setToolbarLookAndFeel(this);

    /*
     * The "Select" button sets the 3d panel into SELECTION picking mode
     * regardless of what mode it is currently in.
     */
      selectButton = JEasyXIcons.createJButton(
        JEasyXIcons.SELECT
      );
      CSW_UI.setToolbarButtonLookAndFeel(selectButton);
      add(selectButton);
      selectButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.setSelectionMode();
        }
      });

      addSeparator();

    /*
     * The "Set Home Position" button will record the current
     * viewing matrices as the home matrices, to be used on
     * subsequent "Go to Home Position" actions.  If this button
     * is never clicked, a default home position which is the same
     * as the full model view position is used.
     */
      setHomePositionButton = JEasyXIcons.createJButton(
        JEasyXIcons.SET_HOME
      );
      CSW_UI.setToolbarButtonLookAndFeel(setHomePositionButton);
      add(setHomePositionButton);
      setHomePositionButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.saveCurrentView ();
        }
      });

    /*
     * Redraw the 3d view using the "home" matrices.
     */
      // "Go to Home Position" button
      homePositionButton = JEasyXIcons.createJButton(
        JEasyXIcons.GO_TO_HOME
      );
      CSW_UI.setToolbarButtonLookAndFeel(homePositionButton);
      add(homePositionButton);
      homePositionButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.resetToHomeView ();
        }
      });

    /*
     * Create a popup menu with choices for viewing from the
     * north, south, east, west, top or bottom direction.
     */
      positionPopup = new JPopupMenu ();
      positionPopup.setLightWeightPopupEnabled (false);

      northItem =
        new JMenuItem ();
      southItem =
        new JMenuItem ();
      eastItem =
        new JMenuItem ();
      westItem =
        new JMenuItem ();
      topItem =
        new JMenuItem ();
      bottomItem =
        new JMenuItem ();

      positionPopup.add (northItem);
      positionPopup.add (southItem);
      positionPopup.add (eastItem);
      positionPopup.add (westItem);
      positionPopup.add (topItem);
      positionPopup.add (bottomItem);

    /*
     * Create actions to invoke preset J3DPanel views
     * when the popup menu items are selected.
     */
      northItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.presetView (J3DPanel.NORTH);
        }
      });

      southItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.presetView (J3DPanel.SOUTH);
        }
      });

      eastItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.presetView (J3DPanel.EAST);
        }
      });

      westItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.presetView (J3DPanel.WEST);
        }
      });

      topItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.presetView (J3DPanel.TOP);
        }
      });

      bottomItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.presetView (J3DPanel.BOTTOM);
        }
      });

    /*
     * Create the toolbar button for "Preset View Position".  The
     * button action will show the positionPopup menu at the
     * current screen location of the button.
     */
      viewPositionButton = JEasyXIcons.createJButton(
        JEasyXIcons.VIEW_POSITION
      );
      CSW_UI.setToolbarButtonLookAndFeel(viewPositionButton);
      add(viewPositionButton);
      viewPositionButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          showPositionPopup ();
        }
      });

      // "Orthogonal On/Off" button
      orthogonalOnOffButton = JEasyXIcons.createJButton(
        JEasyXIcons.ORTHOGONAL
      );
      orthogonalOnOffButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.toggleProjectionMode ();
          J3DToolBar.this.translateDynamicContents ();
        }
      });
      CSW_UI.setToolbarButtonLookAndFeel(orthogonalOnOffButton);
      add(orthogonalOnOffButton);

      addSeparator();

    /*
     * Redraw the 3d view attempting to fill the window.
     */
      viewAllButton = JEasyXIcons.createJButton(
        JEasyXIcons.ZOOM_FULL
      );
      CSW_UI.setToolbarButtonLookAndFeel(viewAllButton);
      add(viewAllButton);
      viewAllButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.resetToUnitScale ();
        }
      });

      // "Target Zoom" button
      targetZoomButton = JEasyXIcons.createJButton(
        JEasyXIcons.ZOOM_IN
      );
      CSW_UI.setToolbarButtonLookAndFeel(targetZoomButton);
      add(targetZoomButton);
      targetZoomButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          enableTargetZoom ();
        }
      });

      // Zoom out button
      zoomOutButton = JEasyXIcons.createJButton(
        JEasyXIcons.ZOOM_OUT
      );
      CSW_UI.setToolbarButtonLookAndFeel(zoomOutButton);
      CSW_UI.setFeatureUnimplemented(zoomOutButton);
      add(zoomOutButton);
      zoomOutButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {

        }
      });

      addSeparator();

    /*
     * Create the unselect button and associated popup menu.
     */
      /*
      unselectButton = JEasyXIcons.createJButton(
        JEasyXIcons.UNSELECT
      );
      CSW_UI.setToolbarButtonLookAndFeel(unselectButton);
      add(unselectButton);
      unselectButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          showUnselectPopup ();
        }
      });
      unselectPopup = new JPopupMenu ();
      unselectPopup.setLightWeightPopupEnabled (false);
      unselectAllItem = new JMenuItem ();
      unselectSurfItem = new JMenuItem ();
      unselectFaultItem = new JMenuItem ();
      unselectLineItem = new JMenuItem ();
      unselectPopup.add (unselectAllItem);
      unselectPopup.add (unselectSurfItem);
      unselectPopup.add (unselectFaultItem);
      unselectPopup.add (unselectLineItem);
      unselectAllItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.unselectAllObjects ();
        }
      });
      unselectSurfItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.unselectAllHorizons ();
        }
      });
      unselectFaultItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.unselectAllFaults ();
        }
      });
      unselectLineItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.unselectAllLines ();
        }
      });
      */

    /*
     * Create the hide button and associated popup menu.
     */
      /*
      hideButton = JEasyXIcons.createJButton(
        JEasyXIcons.HIDE_SELECTED
      );
      CSW_UI.setToolbarButtonLookAndFeel(hideButton);
      add(hideButton);
      hideButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          showHidePopup ();
        }
      });
      hidePopup = new JPopupMenu ();
      hidePopup.setLightWeightPopupEnabled (false);
      hideAllItem = new JMenuItem ();
      hideSurfItem = new JMenuItem ();
      hideFaultItem = new JMenuItem ();
      hideLineItem = new JMenuItem ();
      hidePopup.add (hideAllItem);
      hidePopup.add (hideSurfItem);
      hidePopup.add (hideFaultItem);
      hidePopup.add (hideLineItem);
      hideAllItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.hideSelectedObjects ();
        }
      });
      hideSurfItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.hideSelectedHorizons ();
        }
      });
      hideFaultItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.hideSelectedFaults ();
        }
      });
      hideLineItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.hideSelectedLines ();
        }
      });
      */


    /*
     * Create the unhide button and associated popup menu.
     */
      /*
      unhideButton = JEasyXIcons.createJButton(
        JEasyXIcons.UNHIDE
      );
      CSW_UI.setToolbarButtonLookAndFeel(unhideButton);
      add(unhideButton);
      unhideButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          showUnhidePopup ();
        }
      });
      unhidePopup = new JPopupMenu ();
      unhidePopup.setLightWeightPopupEnabled (false);
      unhideAllItem = new JMenuItem ();
      unhideSurfItem = new JMenuItem ();
      unhideFaultItem = new JMenuItem ();
      unhideLineItem = new JMenuItem ();
      unhidePopup.add (unhideAllItem);
      unhidePopup.add (unhideSurfItem);
      unhidePopup.add (unhideFaultItem);
      unhidePopup.add (unhideLineItem);
      unhideAllItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.showAllObjects ();
        }
      });
      unhideSurfItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.showAllHorizons ();
        }
      });
      unhideFaultItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.showAllFaults ();
        }
      });
      unhideLineItem.addActionListener (new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          panel.showAllLines ();
        }
      });

      addSeparator();
      */

    /*
     * Create the button to show the lighting options dialog.
     */
      /*
      lightingButton = JEasyXIcons.createJButton(
        JEasyXIcons.LIGHTING_OPTIONS
      );
      CSW_UI.setToolbarButtonLookAndFeel(lightingButton);
      add(lightingButton);
      */

    /*
     * Create the button to show the general view options dialog.
     */
      optionsButton = JEasyXIcons.createJButtonFromText (
        "Properties"
      );
      CSW_UI.setToolbarButtonLookAndFeel(optionsButton);
      add(optionsButton);

    /*
     * Get the view properties from the panel for the initial values
     * in the global options and lighting options dialogs.
     */
      //View3DProperties viewProperties = panel.getViewProperties();


    /*
     * When the global options button is clicked, update the dialog with the
     * current properties and show the dialog.
     * The anonymous class below acts as the ActionListener for the "Options"
     * button.
     */
      optionsButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {

        /*
         * Create the global options dialog if needed.
         */
            if (optionsDialog == null) {
                parentFrame = panel.getParentFrame();
                optionsDialog = new View3DPropertiesDialog(
                  parentFrame,
                  panel,
                  panel.getViewProperties()
                );
              optionsDialog.pack ();
            }

            if (optionsDialog.isVisible () == false) {
              View3DProperties viewProperties = panel.getViewProperties();
              optionsDialog.setViewProperties (viewProperties);
              optionsDialog.setLocationRelativeTo (panel);
              optionsDialog.setVisible(true);
              optionsDialog.repaint ();
            }
          }
        });


      translateAllContents();
    }

    JCheckBox terseCheck;
    boolean terseFlag = false;

/*--------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------*/

    private void enableTargetZoom ()
    {
        panel.setPickModeMessage ("Click on the zooming position.");
        panel.enableTargetZoomFlag ();
    }

/*--------------------------------------------------------------------------------------*/

  /*
   * Show the view position popup at the bottom of the
   * view position toolbar button.
   */
    private void showPositionPopup ()
    {
      Point p1 = viewPositionButton.getLocationOnScreen ();
      Point p2 = panel.getLocationOnScreen ();
      int   h = viewPositionButton.getHeight ();

      int x = p1.x - p2.x;
      int y = p1.y - p2.y + h;

      positionPopup.show (panel, x, y);

    }

/*--------------------------------------------------------------------------------------*/

  /*
   * Show the view unselect popup at the bottom of the
   * unselect toolbar button.
   */
    /*
    private void showUnselectPopup ()
    {
      Point p1 = unselectButton.getLocationOnScreen ();
      Point p2 = panel.getLocationOnScreen ();
      int   h = unselectButton.getHeight ();

      int x = p1.x - p2.x;
      int y = p1.y - p2.y + h;

      unselectPopup.show (panel, x, y);

    }
    */

/*--------------------------------------------------------------------------------------*/

  /*
   * Show the hide popup at the bottom of the
   * hide toolbar button.
   */
    /*
    private void showHidePopup ()
    {
      Point p1 = hideButton.getLocationOnScreen ();
      Point p2 = panel.getLocationOnScreen ();
      int   h = hideButton.getHeight ();

      int x = p1.x - p2.x;
      int y = p1.y - p2.y + h;

      hidePopup.show (panel, x, y);

    }
    */

/*--------------------------------------------------------------------------------------*/

  /*
   * Show the unhide popup at the bottom of the
   * unhide toolbar button.
   */
    /*
    private void showUnhidePopup ()
    {
      Point p1 = unhideButton.getLocationOnScreen ();
      Point p2 = panel.getLocationOnScreen ();
      int   h = unhideButton.getHeight ();

      int x = p1.x - p2.x;
      int y = p1.y - p2.y + h;

      unhidePopup.show (panel, x, y);

    }
    */

/*-----------------------------------------------------------------------------------------*/

  /*
   * Translate all text items to the language currently being used.
   */
    void translateAllContents() {
      translateStaticContents ();
      translateDynamicContents ();
    }

  /*
   * Translate the text items that do not change with the state of
   * the 3d panel.
   */
    void translateStaticContents ()
    {

    /*
     * Translate the view position popup menu items.
     */
      northItem.setText (TranslationManager.translate ("View from North"));
      southItem.setText (TranslationManager.translate ("View from South"));
      eastItem.setText (TranslationManager.translate ("View from East"));
      westItem.setText (TranslationManager.translate ("View from West"));
      topItem.setText (TranslationManager.translate ("View from Top"));
      bottomItem.setText (TranslationManager.translate ("View from Bottom"));

    /*
     * Translate the tooltips that do not change with the state of the 3d view.
     */

    if (terseFlag == false) {
      selectButton.setToolTipText(
        TranslationManager.translate("Selection mode"));
      setHomePositionButton.setToolTipText
        (TranslationManager.translate("Set home view"));
      homePositionButton.setToolTipText
        (TranslationManager.translate("Go to home view"));
      viewPositionButton.setToolTipText
        (TranslationManager.translate("Preset view direction"));

      viewAllButton.setToolTipText(
        TranslationManager.translate("Zoom to data extents")
      );
      targetZoomButton.setToolTipText(
        TranslationManager.translate("Zoom in to point")
      );
      zoomOutButton.setToolTipText(TranslationManager.translate("Zoom out"));

      /*
      unselectButton.setToolTipText
        (TranslationManager.translate ("Unselect objects."));
      hideButton.setToolTipText
        (TranslationManager.translate ("Hide selected objects."));
      unhideButton.setToolTipText
        (TranslationManager.translate ("Show previously hidden objects."));
      */
    }

    else {
      selectButton.setToolTipText
        (TranslationManager.translate("Select"));
      setHomePositionButton.setToolTipText
        (TranslationManager.translate("Set Home Position"));
      homePositionButton.setToolTipText
        (TranslationManager.translate("Go to Home Position"));
      viewAllButton.setToolTipText
        (TranslationManager.translate("View All"));
      viewPositionButton.setToolTipText
        (TranslationManager.translate("View Position"));

      targetZoomButton.setToolTipText
        (TranslationManager.translate("Target Zoom"));

      /*
      unselectButton.setToolTipText
        (TranslationManager.translate ("Unselect"));
      hideButton.setToolTipText
        (TranslationManager.translate ("Hide Selected"));
      unhideButton.setToolTipText
        (TranslationManager.translate ("Show"));
      */
    }

    /*
     * Translate the unselect popup items.
     */
    /*
      unselectAllItem.setText
        (TranslationManager.translate ("Unselect All Objects"));
      unselectSurfItem.setText
        (TranslationManager.translate ("Unselect Time Surfaces"));
      unselectFaultItem.setText
        (TranslationManager.translate ("Unselect Faults"));
      unselectLineItem.setText
        (TranslationManager.translate ("Unselect Lines"));
    */

    /*
     * Translate the hide popup items.
     */
    /*
      hideAllItem.setText
        (TranslationManager.translate ("Hide All Selected Objects"));
      hideSurfItem.setText
        (TranslationManager.translate ("Hide Selected Time Surfaces"));
      hideFaultItem.setText
        (TranslationManager.translate ("Hide Selected Faults"));
      hideLineItem.setText
        (TranslationManager.translate ("Hide Selected Lines"));
    */

    /*
     * Translate the unhide popup items.
     */
    /*
      unhideAllItem.setText
        (TranslationManager.translate ("Show All Objects"));
      unhideSurfItem.setText
        (TranslationManager.translate ("Show Time Surfaces"));
      unhideFaultItem.setText
        (TranslationManager.translate ("Show Faults"));
      unhideLineItem.setText
        (TranslationManager.translate ("Show Lines"));
    */

    }

  /*
   * Translate the text items that can change with the state of the 3d view.
   */
    void translateDynamicContents ()
    {

     if (terseFlag == false) {

      int pmode = panel.getProjectionMode ();
      if (pmode == J3DPanel.ORTHO) {
        orthogonalOnOffButton.setToolTipText(
          TranslationManager.translate("Perspective view")
        );
        orthogonalOnOffButton.setIcon(
          JEasyXIcons.getImageIcon(JEasyXIcons.PERSPECTIVE)
        );
      }
      else {
        orthogonalOnOffButton.setToolTipText(
          TranslationManager.translate("Orthogonal view")
        );
        orthogonalOnOffButton.setIcon(
          JEasyXIcons.getImageIcon(JEasyXIcons.ORTHOGONAL)
        );
      }
     }

     else {

      int pmode = panel.getProjectionMode ();
      if (pmode == J3DPanel.ORTHO) {
        orthogonalOnOffButton.setToolTipText
          (TranslationManager.translate("Switch to Perspective"));
      }
      else {
        orthogonalOnOffButton.setToolTipText
          (TranslationManager.translate("Switch To Orthogonal"));
      }
     }

    }

}
