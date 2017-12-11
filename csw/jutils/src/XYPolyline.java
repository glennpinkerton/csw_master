
/*
         ************************************************
         *                                              *
         *    Copyright (1997-2007) Glenn Pinkerton.    *
         *    All rights reserved.                      *
         *                                              *
         ************************************************
*/

package csw.jutils.src;

//import java.io.FileNotFoundException;
//import java.io.IOException;

//import org.w3c.dom.Element;

/**
 * Stores x, y nodes for a 2-D polyline.
 */
public class XYPolyline {

  private double[] xArray = null;
  private double[] yArray = null;

  /**
   * Constructor
   * @param size initial size of the polyline.
   */
  public XYPolyline(int size) {
    init(size);
  }

  /**
   * Constructor. Build a polyline from the specified x and y arrays.
   * The x and y arrays may be of different lengths, but only the minimal
   * length of the 2 arrays is used for the XYPolyline.
   *
   * @param x x points to use for polyline
   * @param y y points to use for polyline
   * @throws IllegalArgumentException if either array is null or empty.
   */
  public XYPolyline(double[] x, double[] y) {
    initFromArrays(x, y);
  }

  /**
   * Copy constructor.
   *
   * @param source the XYPolyline from which to copy.
   */
  public XYPolyline(XYPolyline source) {
    initFromArrays(source.xArray, source.yArray);
  }

  private void init(int size) {
    xArray = new double[size];
    yArray = new double[size];
  }

  private void initFromArrays(double[] x, double[] y) {
    if (x == null || y == null)
      throw new IllegalArgumentException("x array and/or y array is null");

    int size = Integer.MAX_VALUE;
    if (x.length < size)
      size = x.length;
    if (y.length < size)
      size = y.length;
    init(size);

    System.arraycopy(x, 0, xArray, 0, size);
    System.arraycopy(y, 0, yArray, 0, size);
  }

  /**
   * Copy the specified x and y arrays into this XYPolyline.  Any data
   * previously stored in the XYPolyline will be deleted when this is
   * called.  If the x,y arrays are of different lengths, the shortest
   * length is used.  This method is used, for example, when a fault
   * line is being added to the model.  The points for the fault
   * line are already in arrays, so it is easier to just copy them
   * over than to add them one at a time.
   *
   * @param x x array to copy
   * @param y y array to copy
   * @throws IllegalArgumentException if either array is null or empty.
   */
  public void setXY(double[] x, double[] y) {
    if (x == null || y == null)
      throw new IllegalArgumentException("x array and/or y array is null");
    int size = Math.min(x.length, y.length);
    if (size < 2)
      throw new IllegalArgumentException("x array and/or y array length < 2");

    initFromArrays(x, y);
  }

  /**
   * Append points to the end of the XYPolyline, maintaining original
   * order of points in each XYPolyline.
   *
   * @param polylineToAppend the XYPolyline to append.
   */
  public void appendXYPolyline(XYPolyline polylineToAppend) {
    int origSize = size();
    /*
     * If this XYPolyline's internal arrays have yet to be initialized,
     * initialize them and just copy the values from the XYPolyline
     * passed in.
     */
    if (origSize <= 0 || xArray == null) {
      init(polylineToAppend.size());
      for(int i = 0; i < polylineToAppend.size(); i++) {
        xArray[i] = polylineToAppend.xArray[i];
        yArray[i] = polylineToAppend.yArray[i];
      }
    } else {
      double[] xArrayCopy = new double[origSize];
      double[] yArrayCopy = new double[origSize];
      System.arraycopy(xArray, 0, xArrayCopy, 0, origSize);
      System.arraycopy(yArray, 0, yArrayCopy, 0, origSize);
      int addedSize = polylineToAppend.size();
      int totalSize = addedSize + origSize;
      xArray = new double[totalSize];
      yArray = new double[totalSize];
      System.arraycopy(xArrayCopy, 0, xArray, 0, origSize);
      System.arraycopy(yArrayCopy, 0, yArray, 0, origSize);
      System.arraycopy(polylineToAppend.xArray, 0, xArray, origSize, addedSize);
      System.arraycopy(polylineToAppend.yArray, 0, yArray, origSize, addedSize);
    }
  }

  /**
   * Append a point to the polyline.
   *
   * @param x x value of point to append.
   * @param y y value of point to append.
   */
  public void appendXY(double x, double y) {
    int totalSize = size()+1;
    double[] xs = new double[totalSize];
    if (xArray != null)
      System.arraycopy(xArray, 0, xs, 0, totalSize-1);
    xs[totalSize-1] = x;
    double[] ys = new double[totalSize];
    if (yArray != null)
      System.arraycopy(yArray, 0, ys, 0, totalSize-1);
    ys[totalSize-1] = y;

    xArray = new double[totalSize];
    System.arraycopy(xs, 0, xArray, 0, totalSize);
    yArray = new double[totalSize];
    System.arraycopy(ys, 0, yArray, 0, totalSize);
  }

  /**
   * Prepend points to the beginning of a XYPolyline, maintaining the
   * original order of points in each XYPolyline.
   *
   * @param polylineToAppend the XYPolyline to append.
   */
  public void prependXYPolyline(XYPolyline polylineToAppend) {
    int origSize = size();
    /*
     * If this XYPolyline's internal arrays have yet to be initialized,
     * initialize them and just copy the values from the XYPolyline
     * passed in.
     */
    if (origSize <= 0 || xArray == null) {
      init(polylineToAppend.size());
      for(int i = 0; i < polylineToAppend.size(); i++) {
        xArray[i] = polylineToAppend.xArray[i];
        yArray[i] = polylineToAppend.yArray[i];
      }
    } else {
      double[] xArrayCopy = new double[origSize];
      double[] yArrayCopy = new double[origSize];
      System.arraycopy(xArray, 0, xArrayCopy, 0, origSize);
      System.arraycopy(yArray, 0, yArrayCopy, 0, origSize);
      int addedSize = polylineToAppend.size();
      int totalSize = addedSize + origSize;
      xArray = new double[totalSize];
      yArray = new double[totalSize];
      System.arraycopy(polylineToAppend.xArray, 0, xArray, 0, addedSize);
      System.arraycopy(polylineToAppend.yArray, 0, yArray, 0, addedSize);
      System.arraycopy(xArrayCopy, 0, xArray, addedSize, origSize);
      System.arraycopy(yArrayCopy, 0, yArray, addedSize, origSize);
    }
  }

  /**
   * Prepend a point to the polyline.
   *
   * @param x x value of the point to append.
   * @param y y value of the point to append.
   */
  public void prependXY(double x, double y) {
    double[] xs = new double[1];
    double[] ys = new double[1];
    xs[0] = x;
    ys[0] = y;
    prependXYPolyline(new XYPolyline(xs, ys));
  }

  /**
   * Sets a new xy value at index.  If index is out of range,
   * nothing will happen.  Any double value is allowed for x
   * and y (i.e. will not check for Dval).
   *
   * @param index index of point to modify.
   * @param x x value--no checking will be performed.
   * @param y y value--no checking will be performed.
   */
  public void setXY(int index, double x, double y) {
    if (index < 0 || index >= size())
      return;
    xArray[index] = x;
    yArray[index] = y;
  }

  /**
   * @return number of points in the polyline.
   */
  public int size(){
    if (xArray == null)
      return(0);
    return xArray.length;
  }

  /**
   * @return a copy of this polyline.
   */
  public XYPolyline getCopy() {
    return(new XYPolyline(this));
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
}
