
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

/**
 * Stores a single x - y location to define a point in 2D space.
 */
public class XYPoint {

  private double xLocation;
  private double yLocation;

  /**
   * Constructor
   * @param x array of objects which represent x values
   * @param y array of objects which represent y values
   */
  public XYPoint(double x, double y) {
    xLocation = x;
    yLocation = y;
  }

  /**
   * Copy constructor.
   *
   * @param pt {@link XYPoint} from which to copy.
   */
  public XYPoint(XYPoint pt) {
    xLocation = pt.xLocation;
    yLocation = pt.yLocation;
  }

  public double getX ()
  {
    return xLocation;
  }

  public double getY ()
  {
    return yLocation;
  }

  /**
   * Returns a copy (deep) of this XYPoint.
   * @return a copy (deep) of this XYPoint.
   */
  public XYPoint getCopy() {
    return(new XYPoint(this));
  }

}
