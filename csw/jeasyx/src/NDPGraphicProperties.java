
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
import java.util.ArrayList;

/**
 * This class stores the information which maps nominal data
 * properties, such as lithology, kerogen, etc. into colors
 * for graphic images.  The idea is that this class defines
 * how the property should be colored on maps, 3d displays,
 * cross sections or any other type of display.  The actual
 * code to draw the property image differs for 3d, map, etc.
 * but each of these should use the same properties as defined
 * by an object of this class.
 *
 * @author Glenn Pinkerton
 */
public class NDPGraphicProperties {

  private class _cvPair_ {
    public int   value;
    public int   red;
    public int   green;
    public int   blue;
    public int   alpha;
  }

  private ArrayList<_cvPair_> cvList = 
	  new ArrayList<_cvPair_> ();

/*---------------------------------------------------------------------------*/
  /**
  Constructor to create an empty NDPGraphicProperties object.
  */
  public NDPGraphicProperties ()
  {
  }

/*---------------------------------------------------------------------------*/
  /**
  Add a value, color pair.  This means that the number specified will
  have the color specified as its "pure" color.
  @param value An integer number that represents some "pure" entity,
  such as a lithology or a kerogen.
  @param color A {@link Color} object specifying the "pure" entity color.
  */
  public void addValueColor (int value, Color color)
  {
    addValueColor (
      value,
      color.getRed (),
      color.getGreen (),
      color.getBlue (),
      color.getAlpha ()
    );
  }


/*---------------------------------------------------------------------------*/
  /**
  Add a value, color pair.  This means that the number specified will
  have the color specified as its "pure" color.  In this version, the
  color is defined by red, green and blue values.  The alpha is set to
  255 (opaque).
  @param value An integer number that represents some "pure" entity,
  such as a lithology or a kerogen.
  @param red The red value (from 0-255) of the color.
  @param green The green value (from 0-255) of the color.
  @param blue The blue value (from 0-255) of the color.
  */
  public void addValueColor (
    int value,
    int red,
    int green,
    int blue)
  {
    addValueColor (
      value,
      red,
      green,
      blue,
      255
    );
  }

/*---------------------------------------------------------------------------*/
  /**
  Add a value, color pair.  This means that the number specified will
  have the color specified as its "pure" color.  In this version, the
  color is defined by red, green, blue and alpha values.
  @param value An integer number that represents some "pure" entity,
  such as a lithology or a kerogen.
  @param red The red value (from 0-255) of the color.
  @param green The green value (from 0-255) of the color.
  @param blue The blue value (from 0-255) of the color.
  @param alpha The alpha value (from 0-255) of the color.
  */
  public void addValueColor (
    int  value,
    int  red,
    int  green,
    int  blue,
    int  alpha
  )
  {
    _cvPair_ cvp;

    int      i, size;

    size = cvList.size();

    for (i=0; i<size; i++) {

      cvp = cvList.get (i);
      if (cvp == null) continue;

      if (cvp.value == value) {
        cvp.red = red;
        cvp.green = green;
        cvp.blue = blue;
        cvp.alpha = alpha;
        return;
      }

    }

    cvp = new _cvPair_ ();

    cvp.value = value;
    cvp.red = red;
    cvp.green = green;
    cvp.blue = blue;
    cvp.alpha = alpha;

    cvList.add (cvp);

    return;

  }

/*------------------------------------------------------------*/

/*
 * Methods to retrieve data from the cvList member.
 * These are only used by the JDisplayList class and
 * are therefore package scope.
 */
  int getNumberOfValues ()
  {
    int size = cvList.size ();
    return size;
  }

  int getValue (int index)
  {
    int size = cvList.size ();

    if (index < 0  ||  index >= size) {
      return -1;
    }

    _cvPair_ cvp = cvList.get (index);

    return cvp.value;
  }

  int getRed (int index)
  {
    int size = cvList.size ();

    if (index < 0  ||  index >= size) {
      return -1;
    }

    _cvPair_ cvp = cvList.get (index);

    return cvp.red;
  }

  int getGreen (int index)
  {
    int size = cvList.size ();

    if (index < 0  ||  index >= size) {
      return -1;
    }

    _cvPair_ cvp = cvList.get (index);

    return cvp.green;
  }

  int getBlue (int index)
  {
    int size = cvList.size ();

    if (index < 0  ||  index >= size) {
      return -1;
    }

    _cvPair_ cvp = cvList.get (index);

    return cvp.blue;
  }

  int getAlpha (int index)
  {
    int size = cvList.size ();

    if (index < 0  ||  index >= size) {
      return -1;
    }

    _cvPair_ cvp = cvList.get (index);

    return cvp.alpha;
  }

} // end of class NDPGraphicProperties definition
