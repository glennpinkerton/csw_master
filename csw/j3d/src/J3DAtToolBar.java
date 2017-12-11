
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.awt.Dimension;
//import java.awt.Frame;

import javax.swing.JComboBox;
import javax.swing.JToolBar;

import csw.jutils.src.TranslationManager;
import csw.jutils.src.CSW_UI;

/**
 * This toolbar is used to display the 3d panel's atComboBox control.
 * When an attribute is selectred in this combo box, an attribute
 * specific toolbar is displayed to the right of the combo box.
 *
 * @author Glenn Pinkerton
 */
class J3DAtToolBar extends JToolBar {

    private J3DPanel      panel;
    private JComboBox<AttributeDef>     atCombo;
    //private JToolBar      propToolBar;

    //private Frame         parentFrame;

    public static final long  serialVersionUID = 630998833;


/*----------------------------------------------------------------------------*/

  /*
   * The constructor is given the parent J3DPanel object.  If the parent
   * is null, nothing is created.  The constructor has package scope.
   * An instance of this class is only created from the J3DPanel class.
   */
    J3DAtToolBar (J3DPanel parent) {
      super();

      if (parent == null) {
        return;
      }

      panel = parent;

    /*
     * Set standard look and feel for JToolBars.
     */
      CSW_UI.setToolbarLookAndFeel(this);

    /*
     * Get the combo box from the panel and put it into the toolbar.
     */
      atCombo = panel.getAtDefCombo ();
      if (atCombo == null) {
        return;
      }

      atCombo.setMaximumSize (
          new Dimension(
              (int)(atCombo.getPreferredSize().getWidth()),
              (int)(atCombo.getPreferredSize().getHeight())
          )
      );

      add (atCombo);

      atCombo.setToolTipText (
        TranslationManager.translate ("Attribute for coloring surfaces"));

    }

}  /* end of class */
