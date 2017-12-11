
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.uistuff;

//import java.net.URL;
//import java.util.Arrays;
//import java.util.Collections;
//import java.util.List;

//import java.awt.event.ActionListener;
//import java.awt.event.ActionEvent;
//import java.awt.GraphicsEnvironment;

import javax.swing.event.ChangeListener;
import javax.swing.event.ChangeEvent;

//import javax.swing.ImageIcon;
//import javax.swing.JButton;
import javax.swing.JToggleButton;
//import javax.swing.JTextField;
//import javax.swing.JComboBox;
//import javax.swing.JCheckBox;
//import javax.swing.Icon;

//import java.text.NumberFormat;
//import javax.swing.JFormattedTextField;

//import csw.jutils.src.TranslationManager;


/**
 *  This class extends the JToggleButton class and only supplies
 *  constructors to draw text on the JToggleButton.
 */
  public class TextToggleButton  extends JToggleButton
                                 implements  ChangeListener
  {

    public static final long serialVersionUID = 49922234;
    private boolean  bselect_cache;
    private ValueChangeListener  value_change_listener;

  /**
   * Constructor specifies the text string to draw on the toggle button.
   * The initial state of the toggle button is unselected.
   */
    public TextToggleButton (ValueChangeListener vcl, String  label)
    {
      super (label);
      bselect_cache = false;
      value_change_listener = vcl;
      super.addChangeListener (this);
    }
  
  /**
   * Constructor specifies the text string to draw on the toggle button.
   * The initial selected state of the toggle button is also specified.
   */
    public TextToggleButton (ValueChangeListener vcl, String  label, boolean bselect)
    {
      super (label, bselect);
      bselect_cache = bselect;
      value_change_listener = vcl;
      super.addChangeListener (this);
    }

  /**
   * Specify the ValueChangeListener which is notified when the 
   * value of the selected state changes.
   */ 
    public void setValueChangeListener (ValueChangeListener vcl)
    {
      value_change_listener = vcl;
      super.addChangeListener (this);
    }

  /**
   * Return the current selected value.
   */
    public boolean  getSelectedValue ()
    {
      return bselect_cache;
    }
  
    public void stateChanged(ChangeEvent e)
    {
      boolean bval = isSelected ();
      if (bval == bselect_cache) return;
      bselect_cache = bval;
      if (value_change_listener != null) {
        ValueChangeEvent ev = new ValueChangeEvent (this);
        value_change_listener.valueChange(ev);
      }
    }

  } // end of TextToggleButton class
