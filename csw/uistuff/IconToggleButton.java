
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.uistuff;

import java.net.URL;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.GraphicsEnvironment;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JToggleButton;
import javax.swing.JTextField;
import javax.swing.JComboBox;
import javax.swing.JCheckBox;
import javax.swing.Icon;

import java.text.NumberFormat;
import javax.swing.JFormattedTextField;

import csw.jutils.src.TranslationManager;


/**
 *  This class extends the JToggleButton class and only supplies
 *  constructors to draw an icon on the JToggleButton.
 */
  public class IconToggleButton  extends JToggleButton  implements ActionListener
  {

    public static final long serialVersionUID = 49922256;

    private boolean  bselect_cache;
    private ValueChangeListener  value_change_listener;

  /**
   * Constructor specifies the icon to draw on the toggle button.
   * The initial state of the toggle button is unselected.
   */
    public IconToggleButton (ValueChangeListener vcl, Icon  ic)
    {
      super (ic);
      value_change_listener = vcl;
    }
  
  /**
   * Constructor specifies the icon to draw on the toggle button.
   * The initial selected state of the toggle button is also specified.
   */
    public IconToggleButton (ValueChangeListener vcl, Icon  ic, boolean bselect)
    {
      super (ic, bselect);
      value_change_listener = vcl;
    }
 
  /**
   * Specify the ValueChangeListener which is notified when the 
   * value of the selected state changes.
   */ 
    public void setValueChangeListener (ValueChangeListener vcl)
    {
      value_change_listener = vcl;
    }

  /**
   * Return the current selected value.
   */
    public boolean  getSelectedValue ()
    {
      return bselect_cache;
    }
  
    public void actionPerformed(ActionEvent e)
    {
      boolean bval = isSelected ();
      if (bval == bselect_cache) return;
      bselect_cache = bval;
      if (value_change_listener != null) {
        ValueChangeEvent ev = new ValueChangeEvent (this);
        value_change_listener.valueChange(ev);
      }
    }

  } // end of IconToggleButton class

