
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.j3d.src;

/**
 Class for free form text displayed on the J3DPanel object.  This is currently
 only intended to help with debugging and only horizontal text is supported.
*/
public class J3DText extends J3DElement {
    protected int   type;

/*
 * members have package scope so they can be directly
 * accessed by the J3DPanel class.
 */
    String          text;
    int             anchor;
    double          textSize = 0.1;
    boolean         hidden = false;
    double          x, y, z;

/**
 Create an empty text object for use in the 3D display.  Default
 values are set for all the
 members.  Unless you fill this with data via the various set calls, the
 text will not be drawn.
*/
    public J3DText () {
      super();
      init();
    }

    private void init() {
        text = null;
        markedForDelete = false;

        hidden = false;

        x = 1.e30;
        y = 1.e30;
        z = 1.e30;

    }

    /**
    Set the anchor position for the text.  1 = bottom left; 2 = bottom center;
    3 = bottom right; 4 = middle left; 5 = middle center; 6 = middle right;
    7 = top left; 8 = top center; 9 = top right.  The default is bottom left.
    */
    public void setAnchor (int ival)
    {
        anchor = ival;
    }

    public int getAnchor ()
    {
        return anchor;
    }

    /**
    Set the x, y and z coordinates, in model units, for the text.
    */
    public void setXYZ (double xval, double yval, double zval)
    {
        x = xval;
        y = yval;
        z = zval;
    }

    public double getX ()
    {
        return x;
    }

    public double getY ()
    {
        return y;
    }

    public double getZ ()
    {
        return z;
    }

    /**
    Set the text to draw.
    @param str A String object with the text to draw.
    */
    public void setText (String str)
    {
         text = str;
    }

    /**
    Return the text.
    */
    public String getText ()
    {
        return text;
    }

    /**
    Specify whether the text should participate in hidden line removal.
    Set the flag to true if you wish to participate in hidden line
    removal or to false if you do not want to participate.
    If this is not called, false is assumed, and the line will
    not participate in hidden line removal.
    */
    public void setHidden (boolean flag)
    {
        hidden = flag;
    }

    /**
    Query whether the text should participate in hidden line removal.
    True is returned if the text should be hidden or false if it should not be hidden.
    */
    public boolean getHidden ()
    {
        return hidden;
    }

    /**
    Set the text size, in inches on the screen.  The default is 0.1 inches.
    */
    public void setTextSize (double size)
    {
        textSize = size;
    }

    /**
    Get the text size, in inches on the screen.
    */
    public double getTextSize ()
    {
        return textSize;
    }

/*--------------------------------------------------------------------------*/


}  // end of class definition
