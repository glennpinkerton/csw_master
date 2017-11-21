
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

import java.util.ArrayList;

/**
 * Stores x, y, z coorinates for a 3D polyline object.  This is a utility
 * class and thus it does not know or care about the units of the x, y and
 * z values.
 *
 * @author Glenn Pinkerton
 */
public class XYZPolyline
{
    private double[] xArray;
    private double[] yArray;
    private double[] zArray;
    private int      flag;

  /**
   * Create a line array from an ArrayList of XYZPoints.
   */
    public XYZPolyline(ArrayList<XYZPoint> in_points) {

        int  size = in_points.size();

        xArray = new double[size];
        yArray = new double[size];
        zArray = new double[size];
        flag = -1;

        XYZPoint  p;
        for (int i=0; i<size; i++) {
            p = in_points.get (i);
            xArray[i] = p.getX();
            yArray[i] = p.getY();
            zArray[i] = p.getZ();
        }

    }


  /**
   * Create a line array with double arrays of the specified size.  The
   * arrays can be retrieved with the {@link #getXArray}, {@link #getYArray}
   * and {@link #getZArray} methods.  The returned arrays can then be
   * filled in.
   */
    public XYZPolyline(int size) {
        xArray = new double[size];
        yArray = new double[size];
        zArray = new double[size];
        flag = -1;
    }

  /**
   * Constructor that takes 3 double arrays.  This is the preferred constructor.
   *
   * @param x double array containing x values.
   * @param y double array containing y values.
   * @param z double array containing z values.
   */
    public XYZPolyline(double[] x, double[] y, double[] z) {
        int size = x.length;

        xArray = new double[size];
        yArray = new double[size];
        zArray = new double[size];

        System.arraycopy (x, 0, xArray, 0, size);
        System.arraycopy (y, 0, yArray, 0, size);
        System.arraycopy (z, 0, zArray, 0, size);

        flag = -1;

    }

  /**
   * Constructor that takes 3 double arrays and a flag value.
   *
   * @param x double array containing x values.
   * @param y double array containing y values.
   * @param z double array containing z values.
   * @param flag int value to flag the line in some fashion
   */
    public XYZPolyline(double[] x, double[] y, double[] z, int flagval) {
        int size = x.length;

        xArray = new double[size];
        yArray = new double[size];
        zArray = new double[size];

        System.arraycopy (x, 0, xArray, 0, size);
        System.arraycopy (y, 0, yArray, 0, size);
        System.arraycopy (z, 0, zArray, 0, size);

        flag = flagval;

    }

  /**
   * Copy constructor
   *
   * @param polyline XYZPolyline from which to copy.
   */
    public XYZPolyline(XYZPolyline polyline) {
        Copy (polyline);
    }

  /**
   *  (non-Javadoc)
   * @see jcore.geometry.NodeArray#getCopy()
   */
    public XYZPolyline getCopy() {
        XYZPolyline polyline = new XYZPolyline(this);
        return(polyline);
    }

  /**
   * Set the flag value for this line.  If no flag is set via this method
   * or via a constructor, then the default flag value is set to -1.
   */
    public void setFlag (int flagval)
    {
        flagval = flag;
    }

  /**
   * Return the current flag value.  If the flag was never set, -1 is returned.
   */
    public int getFlag ()
    {
        return flag;
    }

  /*
   * Private method to copy from the specified polyline into this
   * polyline.
   */
    private void Copy (XYZPolyline polyline)
    {
        int arraySize = polyline.xArray.length;
        System.arraycopy (polyline.xArray, 0, xArray, 0, arraySize);
        System.arraycopy (polyline.yArray, 0, yArray, 0, arraySize);
        System.arraycopy (polyline.zArray, 0, zArray, 0, arraySize);
        flag = polyline.flag;
    }


  /**
   * Return the length of the x, y and z arrays.
   */
    public int size()
    {
        int isize = xArray.length;
        return isize;
    }

  /**
   * Set the X, Y, and Z values at the specified index
   * @param index index of point whose z value is to change
   * @param x new x value for point at index
   * @param y new y value for point at index
   * @param z new z value for point at index
   */
    public void setXYZ(int index, double x, double y, double z) {
        if (index < 0 || index >= size()) return;
        xArray[index] = x;
        yArray[index] = y;
        zArray[index] = z;
    }

  /**
   * Return the internal x array.
   */
    public double[] getXArray ()
    {
        return xArray;
    }

  /**
   * Return the internal y array.
   */
    public double[] getYArray ()
    {
        return yArray;
    }

  /**
   * Return the internal z array.
   */
    public double[] getZArray ()
    {
        return zArray;
    }

  /**
   * Return a copy of the internal x array.
   */
    public double[] getXArrayCopy ()
    {
        int size = xArray.length;
        double[] copy = new double[size];
        System.arraycopy (xArray, 0, copy, 0, size);
        return copy;
    }

  /**
   * Return a copy of the internal y array.
   */
    public double[] getYArrayCopy ()
    {
        int size = yArray.length;
        double[] copy = new double[size];
        System.arraycopy (yArray, 0, copy, 0, size);
        return copy;
    }

  /**
   * Return a copy of the internal z array.
   */
    public double[] getZArrayCopy ()
    {
        int size = zArray.length;
        double[] copy = new double[size];
        System.arraycopy (zArray, 0, copy, 0, size);
        return copy;
    }

    /**
     * Return the minimum X value of the polyline or 1.e30 if
     * the polyline is empty.
     */
    public double getMinX ()
    {
        double val = MathUtils.getMin (xArray);
        return val;
    }

    /**
     * Return the minimum Y value of the polyline or 1.e30 if
     * the polyline is empty.
     */
    public double getMinY ()
    {
        double val = MathUtils.getMin (xArray);
        return val;
    }

    /**
     * Return the minimum Y value of the polyline or 1.e30 if
     * the polyline is empty.
     */
    public double getMinZ ()
    {
        double val = MathUtils.getMin (zArray);
        return val;
    }

    /**
     * Return the maximum X value of the polyline or -1.e30 if
     * the polyline is empty.
     */
    public double getMaxX ()
    {
        double val = MathUtils.getMax (xArray);
        return val;
    }

    /**
     * Return the maximum Y value of the polyline or -1.e30 if
     * the polyline is empty.
     */
    public double getMaxY ()
    {
        double val = MathUtils.getMax (xArray);
        return val;
    }

    /**
     * Return the maximum Z value of the polyline or -1.e30 if
     * the polyline is empty.
     */
    public double getMaxZ ()
    {
        double val = MathUtils.getMax (zArray);
        return val;
    }

}
