
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jeasyx.src;

import java.awt.Color;
import java.awt.Font;

/*--------------------------------------------------------------------------*/

  /**
   This class stores graphical properties used to draw axes on the
   {@link JDisplayList} object.  All of the properties can be publically accessed
   via the appropriate set and get methods of the class.  In addition, the classes
   in the csw.jeasyx.src package can directly access the properties.
   */

public class AxisProperties {

/*--------------------------------------------------------------------------*/

  /*
   * The instance variables have package scope so the JDisplayList can
   * access them directly.
   */
    int       labelFlag;
    int       tickFlag;
    int       tickDirection;
    String    caption;
    double    majorInterval;
    Color     lineColor;
    Color     textColor;
    double    lineThickness;
    double    textSize;
    double    textThickness;
    int       textFont;
    int       labelAnchor;
    double    labelAngle;
    String    fontName;
    int       fontStyle;

/*--------------------------------------------------------------------------*/

  /**
   Create a new object with default axis property values.
  */
    public AxisProperties ()
    {
        setToDefaults ();
        return;
    }

/*--------------------------------------------------------------------------*/

  /**
  Set all the axis properties to their default values.
  */
    private void setToDefaults ()
    {
        labelFlag = 0;
        tickFlag = 1;
        tickDirection = 1;
        caption = null;
        majorInterval = -1.0;
        lineColor = Color.BLACK;
        textColor = Color.BLACK;
        lineThickness = .007;
        textSize = .10;
        textThickness = .01;
        textFont = 1000;
        labelAnchor = -1;
        labelAngle = 0.0;
        fontName = "Arial";
        fontStyle = Font.PLAIN;
    }

/*--------------------------------------------------------------------------*/

  /**
  Set the name and style for a Java font to use for axis labels and captions.  If this
  is not null, it will override the textFont number set via the {@link #setTextFont}
  method.  By default, a plain Arial font is attempted, and if that is not available,
  the system default font is used.
  */
    public void setFontNameAndStyle (
        String      name,
        int         style)
    {
        fontName = name;
        fontStyle = style;
    }

  /**
  Return the name of the Java font to use for labels and captions.
  */
    public String getFontName ()
    {
        return fontName;
    }

  /**
  Return the style of the Java font to use for labels and captions.
  */
    public int getFontStyle ()
    {
        return fontStyle;
    }


  /**
  Set the anchor for axis labels.  See {@link JDisplayList#setTextAnchor}
  for more details.  If you want automatic label anchoring, set this to -1.
  By default, automatic label anchoring is done.
  */
    public void setLabelAnchor (int anchor)
    {
        labelAnchor = anchor;
        if (labelAnchor < 1  ||  labelAnchor > 9) {
            labelAnchor = -1;
        }
    }

  /**
  Get the current label anchor value.  See {@link JDisplayList#setTextAnchor}
  for a list of text anchor values.  If this is returned as -1, then automatic
  anchoring is being done.
  */
    public int getLabelAnchor ()
    {
        return labelAnchor;
    }

/*--------------------------------------------------------------------------*/

  /**
  Specify the angle, in degrees counterclockwise from the positive x axis,
  for the axis label text.  By default, this is zero, and text is drawn
  horizontally.
  */
    public void setLabelAngle (double angle)
    {
        while (angle < -180.0) {
            angle += 360.0;
        }
        while (angle > 180.0) {
            angle -= 360.0;
        }
        labelAngle = angle;
    }

  /**
  Get the current label Angle (in degrees).
  */
    public double getLabelAngle ()
    {
        return labelAngle;
    }

/*--------------------------------------------------------------------------*/

  /**
  Set the axis label flag.  If this is set to one (1), labels will be drawn at major
  intervals along the axis.  If this is set to zero (0), no labels will be draw.
  By default, no labels are drawn.
  */
    public void setLabelFlag (int ival)
    {
        labelFlag = ival;
    }

  /**
  Get the current label flag value.
  */
    public int getLabelFlag ()
    {
        return labelFlag;
    }

/*--------------------------------------------------------------------------*/

  /**
  Set the axis tick mark flag.  If this is set to one (1) then minor and major
  tick marks will be drawn on the axis.  If set to zero (0) then no tick marks
  will be drawn.  The minor tick marks are .05 inches long and the major tick marks
  are .08 inches long.  As of this version, these lengths cannot be changed.
  By default, tick marks are drawn on the axis.
  */
    public void setTickFlag (int ival)
    {
        tickFlag = ival;
    }

  /**
  Get the current tick flag value.
  */
    public int getTickFlag ()
    {
        return tickFlag;
    }


/*--------------------------------------------------------------------------*/

  /**
  Set the direction that tick marks will be drawn.  If this is set to 1
  then the tick marks will be drawn towards the inside of the frame or graph.
  If set to -1  then tick marks are drawn towards the outside of the frame or graph.
  By default, tick marks are drawn on the inside.  If the axis that this object is
  applied to is not part of the border of a frame or graph, then a tick direction
  of 1 means draw ticks to the left and -1 means draw ticks to the right.
  */
    public void setTickDirection (int ival)
    {
        tickDirection = ival;
    }

  /**
  Get the current value of the tick direction flag.
  */
    public int getTickDirection ()
    {
        return tickDirection;
    }


/*--------------------------------------------------------------------------*/

  /**
   * A simpler method to call from Java applications.
   *
   * @param color the Color to use for setting the Line RGB values.
   */
  public void setLineColor(Color color) {
    if (color == null)
      return; // QWER could throw exception?
    lineColor = color;
  }
/*--------------------------------------------------------------------------*/

  /**
   * A simpler method to call from Java applications.
   *
   * @param color the Color to use for setting the Text RGB values.
   */
  public void setTextColor(Color color) {
    if (color == null)
      return; // QWER could throw exception?
    textColor = color;
  }

/*--------------------------------------------------------------------------*/

  /**
  Set the font to use for drawing text on the axis.  This font is used for both
  labels and the caption.  By default font number 103 is used.  The permissable
  fonts are font 0 to font 7 and font 101 to font 107.  If an invalid font is
  specified, font 103 will be used.
  */
    public void setTextFont (int ival)
    {
        if (ival < 0  ||  ival > 107) {
            ival = 103;
        }
        else if (ival > 7  &&  ival < 101) {
            ival = 103;
        }
        textFont = ival;
    }

  /**
  Get the current text font number.
  */
    public int getTextFont ()
    {
        return textFont;
    }

/*--------------------------------------------------------------------------*/

  /**
  Set the interval between major tick marks.  This is specified in axis value units.
  By default, the major interval is dynamically chosen based
  on the visible portion of the axis.  To force the default behavior, you can set
  this to less than or equal to zero (0).
  */
    public void setMajorInterval (double dval)
    {
        if (dval <= 0.0) {
            dval = -1.0;
        }
        majorInterval = dval;
    }

  /**
  Get the current major interval.  A return value of -1.0 indicates that the default
  major interval behavior is being used.
  */
    public double getMajorInterval ()
    {
        return majorInterval;
    }

/*--------------------------------------------------------------------------*/

  /**
  Set the thickness of lines used in the axis.  The line thickness is used for the axis line
  and for the axis tick marks.  This value should be in device size units (i.e. inches).
  By default, lines are .007 inches thick.  If the specified thickness is less than or equal
  to zero or if it is greater than .25 inches, the default is used.
  */
    public void setLineThickness (double dval)
    {
        if (dval <= 0.0  ||  dval > 0.25) {
            dval = 0.01;
        }
        lineThickness = dval;
    }

  /**
  Get the current line thickness value.
  */
    public double getLineThickness ()
    {
        return lineThickness;
    }

/*--------------------------------------------------------------------------*/

  /**
  Set the text size for the axis.  The size should be specified in device size units
  (i.e. inches).  This size will be used for the axis labels and the axis caption will
  be 1.3 times larger than this value.  By default, the labels are .1 inches and the
  caption is .13 inches.  If a value less than or equal to zero (0) or greater than 1.0 is specified,
  the default is used.
  */
    public void setTextSize (double dval)
    {
        if (dval <= 0.0  ||  dval > 1.0) {
            dval = 0.1;
        }
        textSize = dval;
    }

  /**
  Get the current label text size .  The caption text will be 1.3 times larger.
  */
    public double getTextSize ()
    {
        return textSize;
    }

/*--------------------------------------------------------------------------*/

  /**
  Set the thickness of lines used for drawing text.  This is applied to both axis labels
  and the axis caption.  By default the text is .01 inches thick.  If the specified thickness
  is less than or equal to zero or greater than .1 inches, then the default is used.
  */
    public void setTextThickness (double dval)
    {
        if (dval <= 0.0  ||  dval > 0.1) {
            dval = 0.1;
        }
        textThickness = dval;
    }

  /**
  Get the current text thickness.
  */
    public double getTextThickness ()
    {
        return textThickness;
    }

/*--------------------------------------------------------------------------*/

  /**
  Set the caption string for the axis.  The caption will be drawn centered and
  parallel to the axis.  If you do not want a caption drawn, set this to null.
  By default it is null and no caption is drawn.
  */
    public void setCaption (String sval)
    {
        caption = sval;
    }

  /**
  Get a copy of the caption string.  If there is no caption, a null reference is
  returned.
  */
    public String getCaption ()
    {
        return caption;
    }


}  // end of AxisProperties class.
