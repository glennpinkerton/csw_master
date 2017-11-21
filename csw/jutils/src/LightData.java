
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
 * This class holds data for a light source in the 3D view.
 *
 * @author Glenn Pinkerton
 */
public class LightData
{
  // instance variables
  private double x; // cube x
  private double y; // cube y
  private double z; // cube z
  private boolean enabled = false;
  private int brightness; // 0 - 100

  /* instance initializer */
  {
    brightness = 50;
  }

  /**
   * Construct a light at 0, 0, 0 with default brightness.  The light
   * is initially disabled.
   */
  public LightData(double x, double y, double z, boolean enabled, int brightness) {
    this.x = x;
    this.y = y;
    this.z = z;
    this.enabled = enabled;
    this.brightness = brightness;
  }

  /**
   * Copy constructor.
   */
  public LightData(LightData ld) {
    x = ld.x;
    y = ld.y;
    z = ld.z;
    enabled = ld.enabled;
    brightness = ld.brightness;
  }

  void dump() {
    System.out.println("x, y, z = " + x + " " + y + " " + z);
    System.out.println("enabled = " + enabled);
    System.out.println("brightness = " + brightness);
  }

  /**
   * Set all lighting values.
   *
   * @param x
   * @param y
   * @param z
   * @param enabled
   * @param brightness
   * @return true if any value was changed (i.e. a parameter forced a change),
   * false otherwise.
   */
  public boolean setValues(
    double x,
    double y,
    double z,
    boolean enabled,
    int brightness
  ) {
    this.x = x;
    this.y = y;
    this.z = z;
    this.enabled = enabled;
    this.brightness = brightness;
    return(true);
  }

  /**
   * Copies values from ld.
   *
   * @param ld LightData to copy.
   * @return true if any values actually changed, false otherwise.
   */
  public boolean setValues(LightData ld) {
    return(
      setValues(
        ld.x,
        ld.y,
        ld.z,
        ld.enabled,
        ld.brightness
      )
    );
  }

  /**
   * @return brightness
   */
  public int getBrightness() {
    return brightness;
  }

  /**
   * @return isEnabled
   */
  public boolean isEnabled() {
    return enabled;
  }

  /**
   * @return x
   */
  public double getX() {
    return x;
  }

  /**
   * @return y
   */
  public double getY() {
    return y;
  }

  /**
   * @return z
   */
  public double getZ() {
    return z;
  }

  /**
   * @param i
   */
  public void setBrightness(int i) {
    if (brightness == i)
      return;
    brightness = i;
  }

  /**
   * @param b
   */
  public void setEnabled(boolean b) {
    if (enabled == b)
      return;
    enabled = b;
  }

  /**
   * @param d
   */
  public void setX(double d) {
    x = d;
  }

  /**
   * @param d
   */
  public void setY(double d) {
    y = d;
  }

  /**
   * @param d
   */
  public void setZ(double d) {
    z = d;
  }
}

