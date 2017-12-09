
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

//import csw.jutils.src.TranslationManager;


/**
 * Class reporting the object for which the value changed.  This is sent 
 * in the call to the ValueChangeListener valueChange method.
 */
  public class ValueChangeEvent
  {
    private Object  value_change_object;
  
  /**
   *  Constructor is package scope.  Only create this from the uistuff package.
   */
    ValueChangeEvent  (Object v_o)
    {
      value_change_object = v_o;
    }

    public Object getValueChangeObject () 
    {
      return value_change_object;
    }

  }

