
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

import java.text.NumberFormat;
import javax.swing.JFormattedTextField;

/**
 * Display a double number on a text field.
 */
public class DoubleField extends JFormattedTextField {

  public static final long serialVersionUID = 49922234;

  static public double  UNKNOWN_VALUE = 1.e30;
  static public double  UNKNOWN_CHECK = 1.e29;

  private double dvalue;

  /**
   * Constructor that leaves the double value unknown
   */
  public DoubleField() {
    super (NumberFormat.getNumberInstance ());
    dvalue = UNKNOWN_VALUE;    
  }

  /**
   * Constructor that sets the value as specified.
   */
  public DoubleField(double  val) {
    super (NumberFormat.getNumberInstance ());
    setValue (val);
  }

  /**
   * Copy constructor.
   *
   * @param from_dfield {@link DoubleField} from which to copy.
   */
  public DoubleField(DoubleField  from_dfield) {
    super (from_dfield);
    dvalue = from_dfield.dvalue;
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

}  // end of DoubleField class
