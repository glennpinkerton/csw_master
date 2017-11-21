
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
   * Display a double number on a text field.
   */
  public class DoubleField extends JFormattedTextField
                           implements ActionListener
  {
  
    public static final long serialVersionUID = 49922290;
  
    public static final double  UNKNOWN_VALUE = 1.e30;
    public static final double  UNKNOWN_CHECK = 1.e29;
    public static final double  DEFAULT_MINVAL = -1.e30;
    public static final double  DEFAULT_MAXVAL = 1.e30;
  
    private double dvalue;
    private double maxval;
    private double minval;

    ValueChangeListener  value_change_listener;
  
    /**
     * Constructor that leaves the double value unknown
     */
    public DoubleField(ValueChangeListener  vcl) {
      super (NumberFormat.getNumberInstance ());
      dvalue = UNKNOWN_VALUE;    
      minval = DEFAULT_MINVAL;
      maxval = DEFAULT_MAXVAL;
      value_change_listener = vcl;
      super.addActionListener (this);
    }
  
    /**
     * Constructor that sets the value as specified.
     */
    public DoubleField(ValueChangeListener vcl, double val) {
      super (NumberFormat.getNumberInstance ());
      setVal (val);
      minval = DEFAULT_MINVAL;
      maxval = DEFAULT_MAXVAL;
      value_change_listener = vcl;
      super.addActionListener (this);
    }
  
    /**
     * Constructor that sets the value as specified and sets the
     * min and max values allowed.
     */
    public DoubleField(ValueChangeListener vcl,
                       double val, double dmin, double dmax) {
      super (NumberFormat.getNumberInstance ());
      setVal (val);
      minval = dmin;
      maxval = dmax;
      value_change_listener = vcl;
      super.addActionListener (this);
    }
  
    /**
     * Copy constructor.
     *
     * @param from_dfield {@link DoubleField} from which to copy.
     */
    public DoubleField(DoubleField  from_dfield) {
      super (from_dfield);
      dvalue = from_dfield.dvalue;
      maxval = from_dfield.maxval;
      minval = from_dfield.minval;
      value_change_listener = from_dfield.value_change_listener;
      super.addActionListener (this);
    }

    public void setValueChangeListener (ValueChangeListener  vcl)
    {
      value_change_listener = vcl;
    }
  
    public double getVal ()
    {
      return   dvalue;
    }
  
    public void setVal (double val)
    {
      if (dvalue == val) return;
  
      dvalue = val;
      UpdateTextField ();
    }

    public void setMinMax (double dmin, double dmax)
    {
      minval = dmin;
      maxval = dmax;
    }
  
    private void UpdateTextField ()
    {
      Double  dobj;
      if (dvalue < -UNKNOWN_CHECK  ||  dvalue > UNKNOWN_CHECK) {
          dobj = new Double (Double.NaN);
      }
      else {
          dobj = new Double (dvalue);
      }
      super.setValue (dobj);
    }    

    public void actionPerformed(ActionEvent aev)
    {
      double  dv;
      try {
        String st = getText ();
        dv = Double.parseDouble(st);
      }
      catch (Exception ex) {
        return;
      }
      if (dvalue == dv) return;
      dvalue = dv;

      if (value_change_listener != null) {
        ValueChangeEvent ev = new ValueChangeEvent (this);
        value_change_listener.valueChange (ev);
      }
    }

  }  // end of DoubleField class
