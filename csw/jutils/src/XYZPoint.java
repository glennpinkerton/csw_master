
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
 * Stores a single x, y, z triplet to define a point in space.
 *
 * @author Glenn Pinkerton
 */
public class XYZPoint {

  private double     xLocation, yLocation, zLocation;

  /**
   * Constructor
   * @param x x location
   * @param y y location
   * @param z z location
   */
  public XYZPoint(double x, double y, double z) {
    xLocation = x;
    yLocation = y;
    zLocation = z;
  }

  /**
   * Copy constructor.
   *
   * @param pt {@link XYZPoint} from which to copy.
   */
  public XYZPoint(XYZPoint pt) {
    xLocation = pt.xLocation;
    yLocation = pt.yLocation;
    zLocation = pt.zLocation;
  }

  public void setX (double xval)
  {
    xLocation = xval;
  }

  public void setY (double yval)
  {
    yLocation = yval;
  }

  public void setZ (double zval)
  {
    zLocation = zval;
  }

  public double getX ()
  {
    return xLocation;
  }

  public double getY ()
  {
    return yLocation;
  }

  public double getZ ()
  {
    return zLocation;
  }

  public XYZPoint getCopy() {
    return(new XYZPoint(this));
  }

}  // end of main class
