
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2017) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

import java.awt.Color;

import javax.swing.JToolBar;


/**
 */

public abstract class AttributeDef 
{
    protected static final Color TRANSPARENT_GRAY =
      new Color(128, 128, 128, 200);


/**
 Make all surfaces which have no valid attribute data a transparent gray.
 This is the default.
*/
  public static final int NON_DATA_TRANSPARENT_GRAY = 1;
/**
 Draw all surfaces which have no valid attribute values  in their
 solid patch colors.
*/
  public static final int NON_DATA_PATCH_COLOR = 2;

/**
 * Draw all surfaces that have no valid attribute data as invisible.
 */
  public static final int NON_DATA_INVISIBLE = 3;

  private String             label;      //label appearing in the combo box
  private JToolBar           toolbar;    //toolbar to right of combo box
  private boolean            showLegend; //switches legend drawing on and off
  private int                nonDataRule;//either NON_DATA_INVISIBLE or NON_DATA_PATCH_COLOR
  private int                identifier; //optional identifier used by client

/*----------------------------------------------------------------------*/

  protected AttributeDef ()
  {
    nonDataRule = NON_DATA_TRANSPARENT_GRAY;
    showLegend = false;
    toolbar = null;
    label = null;
    identifier = -1;
  }

/*----------------------------------------------------------------------*/

/**
 Specify the label that will appear in the 3d view's attribute combo
 box.  An attribute def with a null label will not be displayed.
*/
  protected void setLabel (String label)
  {
    if (label == null) {
      this.label = null;
    }
    else {
      this.label = new String (label);
    }
  }

/*----------------------------------------------------------------------*/

/**
 Retrieve the label that will be displayed in the 3d view's attribute combo
 box.  If this is null, the attribute definition will not be displayed.
*/
  public String getLabel ()
  {
    return label;
  }

/*----------------------------------------------------------------------*/

/**
 Specify the toolbar that will be displayed when this attribute definition is
 chosen from the 3d view's attribute combo box.  If this is null, the attribute
 definition will not appear in the combo box.
*/
  protected void setToolBar (JToolBar toolbar)
  {
    this.toolbar = toolbar;
  }

/*----------------------------------------------------------------------*/

/**
 Retrieve the toolbar that will be displayed when this attribute definition is
 chosen from the 3d view's attribute combo box.  If this is null, the attribute
 definition will not appear in the combo box.
*/
  public JToolBar getToolBar ()
  {
    return toolbar;
  }

/*----------------------------------------------------------------------*/

/**
 Set whether or not a legend is to be drawn on the 3d view along with
 the attribute values draped on the surfaces.  By default this is false.
*/
  public void setShowLegend (boolean showLegend)
  {
    this.showLegend = showLegend;
  }

/*----------------------------------------------------------------------*/

/**
 Retrieve whether or not a legend is to be drawn on the 3d view along with
 the attribute values draped on the surfaces.  By default this is false.
*/
  public boolean isShowLegend ()
  {
    return showLegend;
  }

/*----------------------------------------------------------------------*/

/**
 Set the rule for how to color surfaces that have no valid values of this
 attribute.  There are two options: {@link #NON_DATA_TRANSPARENT_GRAY} and
 {@link #NON_DATA_PATCH_COLOR}.  The transparent gray option is the default.
*/
  public void setNonDataRule (int nonDataRule)
  {
    if (nonDataRule < 1  ||  nonDataRule > 3) {
      nonDataRule = 1;
    }
    this.nonDataRule = nonDataRule;
  }

/*----------------------------------------------------------------------*/

/**
 Retrieve the rule for how to color surfaces that have no valid values of this
 attribute.  There are two options: {@link #NON_DATA_TRANSPARENT_GRAY} and
 {@link #NON_DATA_PATCH_COLOR}.  The invisible option is the default.
*/
  public int getNonDataRule ()
  {
    return nonDataRule;
  }

  /**
   * Set the identifier, if desired by client.
   * @param identifier integer identifier
   */
  protected void setIdentifier(int identifier) {
  this.identifier = identifier;
  }

  /**
   * Returns the identifier, or -1 if not set.
   */
  public int getIdentifier() {
    return identifier;
  }

  /*
   *  (non-Javadoc)
   * @see java.lang.Object#toString()
   */
  public String toString() {
    return label;
  }

};



