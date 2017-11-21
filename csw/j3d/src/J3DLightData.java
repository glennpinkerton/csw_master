
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
 * This class holds data for a light source in the 3D view.
 * All the coordinates are in 3d view cube units, ranging
 * from -1 to 1 in each dimension.
 */
public class J3DLightData
{
  private double x; // cube x
  private double y; // cube y
  private double z; // cube z
  private boolean enabled = false;
  private int brightness; // 0 - 100

/**
 * Construct a light at the specified location, brightness and
 * enabled flag.
 */
  public J3DLightData(double x, double y, double z, boolean enabled, int brightness) {
    this.x = x;
    this.y = y;
    this.z = z;
    this.enabled = enabled;
    this.brightness = brightness;
  }

/**
 * Copy constructor.
 */
  public J3DLightData(J3DLightData ld) {
    super(ld);
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
  public void setValues(
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
    brightness = i;
  }

/**
 * @param b
 */
  public void setEnabled(boolean b) {
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

