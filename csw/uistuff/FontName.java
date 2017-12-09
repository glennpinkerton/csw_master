
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
   *  This class uses a private JComboBox to select a font name for
   *  whatever you want it for.  The JComboBox methods are not 
   *  accessible from this class.  The only action you can listen
   *  for is the valueChanged method in the UIFields.ValueChangeListener
   *  interface.  
   */
  public class FontName implements ActionListener
  {

    public static final long serialVersionUID = 49922212;

    private  JComboBox<String>   fontList;
    private  ValueChangeListener  fname_listen;
    private  String          selected_font_name;

    private void setupFontList()
    {
      if (fontList == null)
      {
        GraphicsEnvironment env = GraphicsEnvironment.getLocalGraphicsEnvironment();
        String[] fnames = env.getAvailableFontFamilyNames();
        fontList = new JComboBox<String> (fnames);
        fontList.addActionListener (this);
        fontList.setEditable (false);
      }
      return;
    }

    public FontName (ValueChangeListener  fnl)
    {
      setupFontList ();
      fname_listen = fnl;
    }

    public FontName (ValueChangeListener  fnl,
                     String fname)
    {
      setupFontList ();
      fname_listen = fnl;
      selected_font_name = fname;
      fontList.setSelectedItem (selected_font_name);
    }

    public JComboBox<String>  getComboBox ()
    {
      return fontList;
    }

    public void setFontName (String fname)
    {
      selected_font_name = fname;
      fontList.setSelectedItem (selected_font_name);
    }

    public String  getFontName ()
    {
      return selected_font_name;
    }

    public void setValueChangeListener (ValueChangeListener fnl)
    {
        fname_listen = fnl;
    }

    public void setEnabled (boolean bval)
    {
      if (fontList != null) {
        fontList.setEnabled (bval);
      }
    }

    public void actionPerformed(ActionEvent e)
    {
      String   stmp = (String)fontList.getSelectedItem ();
      if (stmp == null) return;

      if (stmp == selected_font_name) return;
      selected_font_name = stmp;
        
      if (fname_listen != null) {
        ValueChangeEvent ev = new ValueChangeEvent (this);
        fname_listen.valueChange (ev);
      }
    }

  }  // end of FontName class

