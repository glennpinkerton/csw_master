
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
 * Interface used by many uistuff controls to report a changed value.
 */
  public interface ValueChangeListener
  {
    public void valueChange (ValueChangeEvent e);
  }  // end of ValueChangeListener interface


