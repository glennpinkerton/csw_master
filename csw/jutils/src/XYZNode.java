
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
 * Extends the XYZPoint class by adding a flag member.  The flag
 * can be optionally used by surface calculation or point, line,
 * polygon topology stuff.  At present (May 2017) nothing uses the
 * flag member, but I have it for possible future use.
 *
 * @author Glenn Pinkerton
 */
public class XYZNode extends XYZPoint {

  private int    flag_value;

  /**
   * Constructor
   * @param x x location
   * @param y y location
   * @param z z location
   * @param flag  flag value
   */
  public XYZNode(double x, double y, double z, int flag) {
    super (x, y, z);
    flag_value = flag;
  }

  /**
   * Copy constructor.
   *
   * @param pt {@link XYZNode} from which to copy.
   */
  public XYZNode(XYZNode nd) {
    super (nd);
    flag_value = nd.flag_value;
  }

  public void setFlag (int fval)
  {
    flag_value = fval;
  }

  public int getFlag ()
  {
     return flag_value;
  }

  public XYZNode getCopy() {
    return(new XYZNode(this));
  }

}  // end of main class
